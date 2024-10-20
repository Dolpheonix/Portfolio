using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Grpc.Core;
using ProtoObject;
using Google.Protobuf.WellKnownTypes;
using MySql.Data.MySqlClient;
using System.Collections.Specialized;
using Google.Protobuf.Collections;
using System.Text.Json;
using ZstdSharp.Unsafe;
using MySqlX.XDevAPI;

// GrpcGameServer
// 게임 내의 클라이언트의 요청을 처리하는 메인 서버
// 1. 로그인, 회원가입, 닉네임 설정, 게임 저장
// 2. 위치, 애니메이션, 착용 장비 등 멀티플레이어 replication
namespace GrpcGameServer
{
    // 서버에 접속한 클라이언트(유저) 정보
    public class Client
    {
        public int UserIdx { get; set; }        // DB에 접근하기 위한 유저 고유 인덱스
        public int CurrentMapIdx { get; set; }  // 유저가 위치한 맵
        public Location Location { get; set; }  // 맵상의 위치

        // 애니메이션 replication 용 Boolean
        public bool Running { get; set; }   // 달리는 중인가?
        public bool Jumping { get; set; }   // 점프 중인가?
        public bool Equipped { get; set; }  // 무장 상태인가?

        public Equipment Equipment { get; set; } // 현재 선택된 장비

        // Stream Writer -> replication을 위한 다른 유저들의 정보가 전달됨
        public IServerStreamWriter<Location> LocationWriter { get; set; }               // 임의 유저의 위치 변화를 전송
        public IServerStreamWriter<RepBoolean> RepBooleanWriter { get; set; }           // 임의 유저의 RepBoolean 변화를 전송
        public IServerStreamWriter<Equipment> EquipmentWriter { get; set; }             // 임의 유저의 장비 선택 변화를 전송
        public IServerStreamWriter<ResourceChange> ResourceChangeWriter { get; set; }   // 맵 상의 리소스 상태 변화를 전송
        public IServerStreamWriter<MapTransition> MapTransitionWriter { get; set; }     // 임의 유저의 맵 진입/탈출을 전송

        public Client()
        {
            UserIdx = 1;
            CurrentMapIdx = -1;
            Location = new Location();
            Running = false;
            Jumping = false;
            Equipped = false;
        }

        public Client(int userIdx) : this()
        {
            UserIdx = userIdx;
        }
    }

    // 각 맵의 Global State
    public class MapState
    {
        public int mMapIdx { get; set; }                            // 맵의 인덱스
        public List<bool> mEnemyState { get; set; }                 // 맵상의 i번째 몹이 살아있는가?
        public List<bool> mItemState { get; set; }                  // 맵상의 i번째 아이템이 존재하는가?
        public Dictionary<string, Client> mClients { get; set; }    // 맵에 위치한 클라이언트(유저) 목록

        public MapState()
        {
            mMapIdx = -1;
            mClients = new Dictionary<string, Client>();
        }
    }

    // 게임 서버(Grpc 컴파일러로 생성된 GameServiceBase의 구현 클래스)
    public class GameServiceImpl : GameService.GameServiceBase
    {
        private Dictionary<string, Client> mTotalClients;   // 현재 접속중인 클라이언트 리스트
        private List<MapState> mMapStates;                  // 맵 상태 목록
        private MySqlConnection mSqlConnection;             // DB 연결 오브젝트
        private object mLock;                               // 뮤텍스 (client 목록에 접근 시 사용)

        public GameServiceImpl()
        {
            mTotalClients = new Dictionary<string, Client>();
            mMapStates = new List<MapState>();
            mLock = new object();

            // Mysql 데이터베이스에 연결
            if (Connect())
            {
                Console.WriteLine("DB Connected...");
            }

            InitMapState();
        }

        // 로그인 요청 처리
        public override Task<PlayerInfo> TryLogin(LoginInfo request, ServerCallContext context)
        {
            string id = request.Id;
            string pw = request.Password;
            string client = context.Peer;

            Console.WriteLine($"{client}: Login Asked");

            PlayerInfo pi = new PlayerInfo();

            // Login SP 호출
            using (MySqlCommand cmd = new MySqlCommand("CALL TryLogin(@id, @pw)", mSqlConnection))
            {
                cmd.Parameters.AddWithValue("@id", id);
                cmd.Parameters.AddWithValue("@pw", pw);

                using (MySqlDataReader reader = cmd.ExecuteReader())
                {
                    // DB Query에서 에러 발생
                    if (!reader.Read())
                    {
                        Console.WriteLine($"DB ERROR");
                        //TEMP: 우선 로그인 실패로 처리함
                        pi.Map = 10000;

                        return Task.FromResult(pi);
                    }

                    int userIdx = reader.GetInt32(reader.GetOrdinal("userIdx"));
                    
                    // 로그인 실패
                    if (userIdx < 0)
                    {
                        Console.WriteLine($"{client}: Login Failed");
                        pi.Map = 10000; // 실패를 알리는 flag 역할
                        return Task.FromResult(pi);
                    }

                    // userIdx로 db를 조회해, 나머지 플레이어 정보를 가져옴
                    FillPlayerInfo(userIdx, pi);

                    mTotalClients[client] = new Client(userIdx); // Client 목록에 추가

                    Console.WriteLine($"{client}: Login Succeeded");
                }
            }

            return Task.FromResult(pi);
        }

        // 회원가입 요청 처리
        public override Task<PlayerInfo> TryRegister(LoginInfo request, ServerCallContext context)
        {
            string id = request.Id;
            string pw = request.Password;
            string client = context.Peer;

            Console.WriteLine($"{client}: Register Asked");

            PlayerInfo pi = new PlayerInfo();

            // 회원가입 SP 호출
            using (MySqlCommand cmd = new MySqlCommand("CALL TryRegister(@id, @pw)", mSqlConnection))
            {
                cmd.Parameters.AddWithValue("@id", id);
                cmd.Parameters.AddWithValue("@pw", pw);

                using (MySqlDataReader reader = cmd.ExecuteReader())
                {
                    // DB 쿼리에서 에러 발생
                    if (!reader.Read())
                    {
                        Console.WriteLine($"DB ERROR");
                        //TEMP: 우선 회원가입 실패 처리
                        pi.Map = 10000;
                        return Task.FromResult<PlayerInfo>(pi);
                    }

                    int userIdx = reader.GetInt32(reader.GetOrdinal("userIdx"));
                    // 회원가입 실패
                    if (userIdx < 0)
                    {
                        Console.WriteLine($"{client}: Register Failed");
                        pi.Map = 10000;
                        return Task.FromResult(pi);
                    }

                    mTotalClients[client] = new Client(userIdx);

                    // 기본 데이터 생성
                    MakeDefaultPlayerInfo(pi);
                    pi.UserIdx = (ulong)userIdx;

                    // DB에 기본 데이터를 저장
                    SavePlayerInfo(pi, context);
                }
            }

            return Task.FromResult(pi);
        }

        // 닉네임 설정 함수
        public override Task<Nickname> SetNickname(Nickname request, ServerCallContext context)
        {
            string client = context.Peer;

            Console.WriteLine($"{client}: Nickname Setting Asked");

            // 비정상 접속
            if (!mTotalClients.ContainsKey(client))
            {
                //TODO : 비정상 접속 처리
                return Task.FromResult<Nickname>(null);
            }

            int userIdx = mTotalClients[client].UserIdx;
            Nickname response = new Nickname();

            // 닉네임 설정 SP 실행
            using (MySqlCommand cmd = new MySqlCommand("CALL SetNickname(@userIdx, @nickname)", mSqlConnection))
            {
                cmd.Parameters.AddWithValue("@userIdx", userIdx);
                cmd.Parameters.AddWithValue("nickname", request.Nickname_);

                using (MySqlDataReader reader = cmd.ExecuteReader())
                {
                    // 성공했다면, 설정된 닉네임을 그대로 반환
                    if (reader.Read() && reader.GetBoolean(reader.GetOrdinal("Succeeded")))
                    {
                        response.Nickname_ = request.Nickname_;
                    }
                    // 실패했다면, failed를 반환
                    else
                    {
                        response.Nickname_ = "failed";
                    }
                }
            }

            return Task.FromResult(response);
        }

        // 유저 데이터 저장
        public override Task<Empty> Save(PlayerInfo request, ServerCallContext context)
        {           
            SavePlayerInfo(request, context);

            return Task.FromResult(new Empty());
        }

        // 수신한 위치 정보 처리
        public override async Task<Empty> SendLocation(IAsyncStreamReader<Location> requestStream, ServerCallContext context)
        {
            string client = context.Peer;
            Location request = new Location();

            // location 위치가 전송될때마다 처리
            while (await requestStream.MoveNext())
            {
                request = requestStream.Current;

                lock (mLock)
                {
                    if (!mTotalClients.ContainsKey(client))
                    {
                        // 클라이언트는 접속이 끊겼는데, stream 객체만 남아있음
                        Console.WriteLine($"{client}: excluded from client list. disconnect location stream.");

                        return new Empty();
                    }

                    // client 객체에 적용
                    mTotalClients[client].Location = request;

                    int mapIdx = mTotalClients[client].CurrentMapIdx;
                    if(mapIdx < 0)
                    {
                        //TODO: 맵 인덱스가 비정상임 -> 접속 차단 OR 무시
                        continue;
                    }

                    // 같은 맵에 위치한 유저들에게만 전송
                    foreach (var pair in mMapStates[mapIdx].mClients)
                    {
                        var tosend = pair.Value;
     
                        if (tosend.LocationWriter != null)
                        {
                            tosend.LocationWriter.WriteAsync(request);
                        }
                    }
                }
            }

            return new Empty();
        }

        // 달리기/점프/장비 상태 전송
        public override async Task<Empty> SendRepBoolean(IAsyncStreamReader<RepBoolean> requestStream, ServerCallContext context)
        {
            string client = context.Peer;
            RepBoolean request;

            while (await requestStream.MoveNext())
            {
                request = requestStream.Current;

                lock (mLock)
                {
                    if (!mTotalClients.ContainsKey(client))
                    {
                        // 클라이언트는 접속이 끊겼는데, stream 객체만 남아있음
                        Console.WriteLine($"{client}: excluded from client list. disconnect repboolean stream.");

                        return new Empty();
                    }

                    // client 객체에 적용
                    switch (request.Type)
                    {
                        case RepBoolean.Types.RepType.Running:
                            mTotalClients[client].Running = request.Boolean;
                            break;
                        case RepBoolean.Types.RepType.Jumping:
                            mTotalClients[client].Jumping = request.Boolean;
                            break;
                        case RepBoolean.Types.RepType.Equippped:
                            mTotalClients[client].Equipped = request.Boolean;
                            break;
                        default:
                            break;
                    }

                    int mapIdx = mTotalClients[client].CurrentMapIdx;
                    if (mapIdx < 0)
                    {
                        //TODO: 맵 인덱스가 비정상임 -> 접속 차단 OR 무시
                        continue;
                    }

                    // 같은 맵에 위치한 유저들에게만 전송
                    foreach (var pair in mMapStates[mapIdx].mClients)
                    {
                        var tosend = pair.Value;

                        if (tosend.RepBooleanWriter != null)
                        {
                            tosend.RepBooleanWriter.WriteAsync(request);
                        }
                    }
                }
            }

            return new Empty();
        }

        // 장비 변경 전송
        public override Task<Empty> SendEquipmentChange(Equipment request, ServerCallContext context)
        {

            string client = context.Peer;

            lock (mLock)
            {
                if (!mTotalClients.ContainsKey(client))
                {
                    // 클라이언트는 접속이 끊겼는데, 요청이 들어옴
                    Console.WriteLine($"{client}: excluded from client list. exit task.");

                    return Task.FromResult(new Empty());
                }

                // client 객체에 적용
                mTotalClients[client].Equipment = request;

                int mapIdx = mTotalClients[client].CurrentMapIdx;
                if (mapIdx < 0)
                {
                    //TODO: 맵 인덱스가 비정상임 -> 접속 차단 OR 무시
                    return Task.FromResult(new Empty());
                }

                // 같은 맵에 위치한 유저들에게만 전송
                foreach (var pair in mMapStates[mapIdx].mClients)
                {
                    var tosend = pair.Value;

                    if (tosend.EquipmentWriter != null)
                    {
                        tosend.EquipmentWriter.WriteAsync(request);
                    }
                }
            }

            return Task.FromResult(new Empty());
        }

        // 맵 리소스 변화 전송
        public override Task<Empty> SendMapResourceChange(ResourceChange request, ServerCallContext context)
        {
            string client = context.Peer;

            lock(mLock)
            {
                if (!mTotalClients.ContainsKey(client))
                {
                    // 클라이언트는 접속이 끊겼는데, 요청이 들어옴
                    Console.WriteLine($"{client}: excluded from client list. exit task.");

                    return Task.FromResult(new Empty());
                }

                int mapIdx = mTotalClients[client].CurrentMapIdx;
                if (mapIdx < 0)
                {
                    //TODO: 맵 인덱스가 비정상임 -> 접속 차단 OR 무시
                    return Task.FromResult(new Empty());
                }

                // mapstate 객체에 적용
                if (request.ResType == ResourceChange.Types.ResourceType.Enemy)
                {
                    if (request.ResChangeType == ResourceChange.Types.ChangeType.Remove)
                    {
                        DeleteEnemy(mapIdx, request.ResIdx);
                    }
                }
                else
                {
                    if (request.ResChangeType == ResourceChange.Types.ChangeType.Remove)
                    {
                        DeleteItem(mapIdx, request.ResIdx);
                    }
                }

                // 같은 맵에 위치한 유저들에게만 전송
                foreach (var pair in mMapStates[mapIdx].mClients)
                {
                    var tosend = pair.Value;

                    if (tosend.ResourceChangeWriter != null)
                    {
                        tosend.ResourceChangeWriter.WriteAsync(request);
                    }
                }
            }

            return Task.FromResult(new Empty());
        }

        // 맵 이탈/진입 전송
        public override Task<Empty> SendMapTransition(MapTransition request, ServerCallContext context)
        {
            string client = context.Peer;

            lock (mLock)
            {
                if (!mTotalClients.ContainsKey(client))
                {
                    // 클라이언트는 접속이 끊겼는데, 요청이 들어옴
                    Console.WriteLine($"{client}: excluded from client list. exit task.");

                    return Task.FromResult(new Empty());
                }

                uint beforeIdx = request.Before;
                uint afterIdx =  request.After;

                var caller = mTotalClients[client];
                // client 객체에 적용
                caller.CurrentMapIdx = (int)request.After;

                // 게임을 시작할 때는 beforeIdx와 afterIdx가 같음 ==> beforeIdx 처리 부분은 무시해도 됨
                // beforeIdx 처리
                // 1. 유저가 맵에서 사라짐을 알림
                // 2. 사라진 유저는 mapstate에서 삭제
                if (beforeIdx != afterIdx)
                {
                    // before 맵에서 클라이언트 삭제
                    mMapStates[(int)beforeIdx].mClients.Remove(client);

                    foreach(var pair in mMapStates[(int)beforeIdx].mClients)
                    {
                        var tosend = pair.Value;

                        // before 맵에 존재하는 클라이언트들에게 해당 유저가 맵에서 없어짐을 알림
                        if (tosend.MapTransitionWriter != null)
                        {
                            tosend.MapTransitionWriter.WriteAsync(request);
                        }
                    }
                }

                // afterIdx 처리
                // 1. 유저가 맵에 진입함을 알림
                // 2. 진입한 유저에게 기존 유저의 정보를 전송
                foreach (var pair in mMapStates[(int)afterIdx].mClients)
                {
                    var tosend = pair.Value;

                    // caller가 맵에 등장함 -> 맵에 추가해야 함
                    if(tosend.MapTransitionWriter != null)
                    {
                        tosend.MapTransitionWriter.WriteAsync(request);
                    }

                    // MapTransition을 보내면, tosend 클라이언트의 게임에 multiplayer 액터가 생성됨
                    // 진입한 유저의 equip 정보를 전송해야 함 (run, jump는 맵 진입시에 false이므로 따로 전송할 필요 없음)
                    if (tosend.RepBooleanWriter != null && tosend.EquipmentWriter != null)
                    {
                        RepBoolean equipBoolean = new RepBoolean();
                        equipBoolean.UserIdx = (ulong)caller.UserIdx;
                        equipBoolean.Type = RepBoolean.Types.RepType.Equippped;
                        equipBoolean.Boolean = caller.Equipped;

                        tosend.RepBooleanWriter.WriteAsync(equipBoolean);
                        tosend.EquipmentWriter.WriteAsync(caller.Equipment);
                    }
                    // 진입한 유저에게도, 기존 유저들의 정보를 전송해야 함
                    if(caller.MapTransitionWriter != null && caller.LocationWriter != null && caller.RepBooleanWriter != null && caller.EquipmentWriter != null)
                    {
                        // maptransition을 보내서 더미 액터를 생성
                        MapTransition maptransition = new MapTransition();
                        maptransition.UserIdx = (ulong)tosend.UserIdx;
                        maptransition.Before = 10000;
                        maptransition.After = (uint)afterIdx;
                        caller.MapTransitionWriter.WriteAsync(maptransition);

                        // 위치 정보 전송
                        caller.LocationWriter.WriteAsync(tosend.Location);

                        // Replication Boolean 전송
                        RepBoolean runboolean = new RepBoolean();
                        runboolean.UserIdx = (ulong)tosend.UserIdx;
                        runboolean.Type = RepBoolean.Types.RepType.Running;
                        runboolean.Boolean = tosend.Running;
                        caller.RepBooleanWriter.WriteAsync(runboolean);

                        RepBoolean jumpBoolean = new RepBoolean();
                        jumpBoolean.UserIdx = (ulong)tosend.UserIdx;
                        jumpBoolean.Type = RepBoolean.Types.RepType.Jumping;
                        jumpBoolean.Boolean = tosend.Jumping;
                        caller.RepBooleanWriter.WriteAsync(jumpBoolean);

                        RepBoolean equipBoolean = new RepBoolean();
                        equipBoolean.UserIdx = (ulong)tosend.UserIdx;
                        equipBoolean.Type = RepBoolean.Types.RepType.Equippped;
                        equipBoolean.Boolean = tosend.Equipped;
                        caller.RepBooleanWriter.WriteAsync(equipBoolean);

                        // 장비 상태 전송
                        caller.EquipmentWriter.WriteAsync(tosend.Equipment);
                    }
                }

                // 맵의 리소스 상태를 전송
                // enemy
                for(int i = 0; i < mMapStates[(int)afterIdx].mEnemyState.Count; ++i)
                {
                    // 기본적으로, 모든 리소스는 클라이언트 상에 스폰되기 때문에, 사라졌을 때만 보냄
                    var enemyres = mMapStates[(int)afterIdx].mEnemyState[i];
                    if(!enemyres)
                    {
                        ResourceChange resch = new ResourceChange();
                        resch.ResType = ResourceChange.Types.ResourceType.Enemy;
                        resch.ResChangeType = ResourceChange.Types.ChangeType.Remove;
                        resch.MapIdx = afterIdx;
                        resch.ResIdx = (ulong)i;

                        caller.ResourceChangeWriter.WriteAsync(resch);
                    }
                }
                // item
                for (int i = 0; i < mMapStates[(int)afterIdx].mItemState.Count; ++i)
                {
                    // 기본적으로, 모든 리소스는 클라이언트 상에 스폰되기 때문에, 사라졌을 때만 보냄
                    var itemres = mMapStates[(int)afterIdx].mItemState[i];
                    if (!itemres)
                    {
                        ResourceChange resch = new ResourceChange();
                        resch.ResType = ResourceChange.Types.ResourceType.Item;
                        resch.ResChangeType = ResourceChange.Types.ChangeType.Remove;
                        resch.MapIdx = afterIdx;
                        resch.ResIdx = (ulong)i;

                        caller.ResourceChangeWriter.WriteAsync(resch);
                    }
                }

                // after 맵의 클라이언트 리스트에 추가
                mMapStates[(int)afterIdx].mClients[client] = caller;
            }

            return Task.FromResult(new Empty());
        }

        // 위치 정보 Broadcast
        public override async Task<Status> BroadcastLocation(Empty request, IServerStreamWriter<Location> responseStream, ServerCallContext context)
        {
            if (!mTotalClients.ContainsKey(context.Peer))
            {
                return Status.DefaultCancelled;
            }
            lock (mLock)
            {
                mTotalClients[context.Peer].LocationWriter = responseStream;
            }

            // 연결이 끊기기 전까지 유지해야 함 (동작은 mClients로 관리하므로)
            while (context.CancellationToken.IsCancellationRequested)
            {
                await Task.Delay(1000);
            }

            return Status.DefaultSuccess;
        }

        // 애니메이션 상태 Broadcast
        public override async Task<Status> BroadcastRepBoolean(Empty request, IServerStreamWriter<RepBoolean> responseStream, ServerCallContext context)
        {
            if (!mTotalClients.ContainsKey(context.Peer))
            {
                return Status.DefaultCancelled;
            }
            lock (mLock)
            {
                mTotalClients[context.Peer].RepBooleanWriter = responseStream;
            }

            // 연결이 끊기기 전까지 유지해야 함 (동작은 mClients로 관리하므로)
            while (context.CancellationToken.IsCancellationRequested)
            {
                await Task.Delay(1000);
            }

            return Status.DefaultSuccess;
        }

        // 장비 변경 Broadcast
        public override async Task<Status> BroadcastEquipmentChange(Empty request, IServerStreamWriter<Equipment> responseStream, ServerCallContext context)
        {
            if (!mTotalClients.ContainsKey(context.Peer))
            {
                return Status.DefaultCancelled;
            }
            lock (mLock)
            {
                mTotalClients[context.Peer].EquipmentWriter = responseStream;
            }

            // 연결이 끊기기 전까지 유지해야 함 (동작은 mClients로 관리하므로)
            while (context.CancellationToken.IsCancellationRequested)
            {
                await Task.Delay(1000);
            }

            return Status.DefaultSuccess;
        }

        // 리소스 변화 Broadcast
        public override async Task<Status> BroadcastMapResourceChange(Empty request, IServerStreamWriter<ResourceChange> responseStream, ServerCallContext context)
        {
            if (!mTotalClients.ContainsKey(context.Peer))
            {
                return Status.DefaultCancelled;
            }
            lock (mLock)
            {
                mTotalClients[context.Peer].ResourceChangeWriter = responseStream;
            }

            // 연결이 끊기기 전까지 유지해야 함 (동작은 mClients로 관리하므로)
            while (context.CancellationToken.IsCancellationRequested)
            {
                await Task.Delay(1000);
            }

            return Status.DefaultSuccess;
        }

        // 맵 전환 Broadcast
        public override async Task<Status> BroadcastMapTransition(Empty request, IServerStreamWriter<MapTransition> responseStream, ServerCallContext context)
        {
            if (!mTotalClients.ContainsKey(context.Peer))
            {
                return Status.DefaultCancelled;
            }
            lock (mLock)
            {
                mTotalClients[context.Peer].MapTransitionWriter = responseStream;
            }

            // 연결이 끊기기 전까지 유지해야 함 (동작은 mClients로 관리하므로)
            while (context.CancellationToken.IsCancellationRequested)
            {
                await Task.Delay(1000);
            }

            return Status.DefaultSuccess;
        }

        // Map State 초기화
        public void InitMapState()
        {
            // 초기 맵 상태 설정
            string filePath = @"..\..\..\..\GameData\LevelInfo.json";
            string jsonString = File.ReadAllText(filePath);

            JsonDocument doc = JsonDocument.Parse(jsonString);
            JsonElement root = doc.RootElement;

            foreach (JsonElement levelInfo in root.GetProperty("LevelInfoList").EnumerateArray())
            {
                MapState ms = new MapState();
                ms.mMapIdx = levelInfo.GetProperty("Index").GetInt32();

                string mapName = levelInfo.GetProperty("Name").GetString();
                string levelDataPath = @$"..\..\..\..\GameData\LevelData\{mapName}\";

                string enemyDataPath = levelDataPath + "Enemy.json";
                string enemyJsonString = File.ReadAllText(enemyDataPath);
                using (JsonDocument enemydoc = JsonDocument.Parse(enemyJsonString))
                {
                    JsonElement enemyroot = enemydoc.RootElement;
                    ms.mEnemyState = new List<bool>(enemyroot.GetProperty("EnemyList").GetArrayLength());
                }

                string itemDataPath = levelDataPath + "Item.json";
                string itemJsonString = File.ReadAllText(itemDataPath);
                using (JsonDocument itemdoc = JsonDocument.Parse(itemJsonString))
                {
                    JsonElement itemroot = itemdoc.RootElement;
                    ms.mItemState = new List<bool>(itemroot.GetProperty("ItemList").GetArrayLength());
                }

                mMapStates.Add(ms);
            }
        }

        public void DeleteEnemy(int mapIdx, ulong enemyIdx)
        {
            mMapStates[mapIdx].mEnemyState[(int)enemyIdx] = false;
        }

        public void DeleteItem(int mapIdx, ulong itemIdx)
        {
            mMapStates[mapIdx].mItemState[(int)itemIdx] = false;
        }

        // DB를 조회해 PlayerInfo 구조체를 채움
        public void FillPlayerInfo(int userIdx, PlayerInfo pi)
        {
            // 기본 플레이어 정보 fetching
            using (MySqlCommand cmd = new MySqlCommand("CALL GetPlayerInfo(@userIdx)", mSqlConnection))
            {
                cmd.Parameters.AddWithValue("@userIdx", userIdx);
                using (MySqlDataReader reader = cmd.ExecuteReader())
                {
                    MakePlayerInfoFromResultSet(reader, pi);
                }
            }

            // 인벤토리 fetching
            using (MySqlCommand cmd = new MySqlCommand("CALL GetInventory(@userIdx)", mSqlConnection))
            {
                cmd.Parameters.AddWithValue("@userIdx", userIdx);
                using (MySqlDataReader reader = cmd.ExecuteReader())
                {
                    MakeInventoryFromResultSet(reader, pi);
                }
            }

            // 퀘스트 상태 Fetching
            using (MySqlCommand cmd = new MySqlCommand("CALL GetQuestStatus(@userIdx)", mSqlConnection))
            {
                cmd.Parameters.AddWithValue("@userIdx", userIdx);
                using (MySqlDataReader reader = cmd.ExecuteReader())
                {
                    MakeQuestStatusFromResultSet(reader, pi);
                }
            }
        }

        static void MakePlayerInfoFromResultSet(MySqlDataReader reader, PlayerInfo outInfo)
        {
            if (reader.Read())
            {
                outInfo.Name = reader.GetString(reader.GetOrdinal("nickname"));
                outInfo.Level = reader.GetUInt32(reader.GetOrdinal("playerLevel"));
                outInfo.Map = reader.GetUInt32(reader.GetOrdinal("mapIdx"));
                outInfo.LocX = reader.GetFloat(reader.GetOrdinal("loc_x"));
                outInfo.LocY = reader.GetFloat(reader.GetOrdinal("loc_y"));
                outInfo.LocZ = reader.GetFloat(reader.GetOrdinal("loc_z"));
                outInfo.Gold = reader.GetUInt64(reader.GetOrdinal("gold"));
            }
        }

        static void MakeInventoryFromResultSet(MySqlDataReader reader, PlayerInfo outInfo)
        {
            Inventory refInventory = outInfo.Inventory;
            int typenum = System.Enum.GetValues(typeof(Inventory.Types.ItemType)).Length;
            for (int i = 0; i < typenum; ++i)
            {
                refInventory.TypeInventory.Add(new TypeInventory());
            }

            while (reader.Read())
            {
                string typeStr = reader.GetString(reader.GetOrdinal("itemType"));

                GameItem gi = new GameItem();

                switch (typeStr)
                {
                    case "CLOTH":
                        refInventory.TypeInventory[0].Items.Add(gi);
                        break;
                    case "WEAPON":
                        refInventory.TypeInventory[1].Items.Add(gi);
                        break;
                    case "ITEM":
                        refInventory.TypeInventory[2].Items.Add(gi);
                        break;
                    default:
                        //TODO : db 패치 에러
                        Console.WriteLine("Wrong Inventory Type");
                        break;
                }

                gi.Index = reader.GetUInt64(reader.GetOrdinal("infoIndex"));
                gi.Num = reader.GetUInt64(reader.GetOrdinal("num"));
            }
        }

        static void MakeQuestStatusFromResultSet(MySqlDataReader reader, PlayerInfo outInfo)
        {
            var qss = outInfo.QuestStatus;
            while (reader.Read())
            {
                uint qIdx = reader.GetUInt32(reader.GetOrdinal("questIdx"));

                if (qIdx >= qss.Count)
                {
                    for (int i = 0; i <= qIdx; ++i)
                    {
                        QuestStatus nqs = new QuestStatus();
                        qss.Add(nqs);
                    }
                }

                QuestStatus qs = qss[(int)qIdx];
                // 메인 퀘스트 정보는 한번만 기록
                if (qs.Index != qIdx)
                {
                    qs.Index = qIdx;

                    string progressStr = reader.GetString("progress");
                    switch (progressStr)
                    {
                        case "Unavailable":
                            qs.ProgressType = QuestStatus.Types.QuestProgressType.Unavailable;
                            break;
                        case "Available":
                            qs.ProgressType = QuestStatus.Types.QuestProgressType.Available;
                            break;
                        case "InProgress":
                            qs.ProgressType = QuestStatus.Types.QuestProgressType.Inprogress;
                            break;
                        case "Completable":
                            qs.ProgressType = QuestStatus.Types.QuestProgressType.Completable;
                            break;
                        case "Completed":
                            qs.ProgressType = QuestStatus.Types.QuestProgressType.Completed;
                            break;
                        default:
                            // TODO : db fetch 에러
                            break;
                    }

                    qs.CurrPhase = reader.GetUInt32(reader.GetOrdinal("currPhase"));
                    qs.Completed = reader.GetUInt32(reader.GetOrdinal("completed"));
                }

                uint sqIdx = reader.GetUInt32(reader.GetOrdinal("subquestIdx"));
                if (sqIdx >= qs.SubStatus.Count)
                {
                    for (int i = 0; i < sqIdx; ++i)
                    {
                        QuestStatus.Types.SubQuestStatus nsqs = new QuestStatus.Types.SubQuestStatus();
                        qs.SubStatus.Add(nsqs);
                    }
                }

                var sqs = qs.SubStatus[(int)sqIdx];
                sqs.BStarted = reader.GetBoolean(reader.GetOrdinal("bStarted"));
                sqs.BCompleted = reader.GetBoolean(reader.GetOrdinal("bCompleted"));
                sqs.CurrAmount = reader.GetUInt32(reader.GetOrdinal("currAmount"));
            }
        }

        static void MakeDefaultPlayerInfo(PlayerInfo outInfo)
        {
            outInfo.Level = 1;
            outInfo.Map = 0;
            outInfo.LocX = 0.0f;
            outInfo.LocY = 0.0f;
            outInfo.LocZ = -100.0f;
            outInfo.Gold = 0;

            int itemTypeCount = System.Enum.GetValues(typeof(Inventory.Types.ItemType)).Length;
            for (int i = 0; i < itemTypeCount; ++i)
            {
                TypeInventory ti = new TypeInventory();
                outInfo.Inventory.TypeInventory.Add(ti);
            }

            // QuestStatus 생성
            // Quest 게임 데이터를 참조해 Status 배열을 생성한다
            string jsonFilePath = @"..\..\..\..\GameData\Quest.json";
            string jsonString = File.ReadAllText(jsonFilePath);

            JsonDocument doc = JsonDocument.Parse(jsonString);
            JsonElement root = doc.RootElement;

            foreach (JsonElement quest in root.GetProperty("QuestList").EnumerateArray())
            {
                var qs = new QuestStatus
                {
                    Index = quest.GetProperty("Index").GetUInt32(),
                    Type = quest.GetProperty("Type").GetString() == "Serial"
                                    ? QuestStatus.Types.QuestType.Serial
                                    : QuestStatus.Types.QuestType.Parallel,
                    CurrPhase = 0,
                    Completed = 0,
                    ProgressType = QuestStatus.Types.QuestProgressType.Available //TEMP : 퀘스트 수주 조건을 구현하지 않음
                };

                foreach (JsonElement subQuest in quest.GetProperty("SubQuests").EnumerateArray())
                {
                    var sqs = new QuestStatus.Types.SubQuestStatus
                    {
                        BStarted = false,
                        BCompleted = false,
                        CurrAmount = 0
                    };

                    string typeStr = subQuest.GetProperty("Type").GetString();
                    switch (typeStr)
                    {
                        case "Arrival":
                            sqs.Type = QuestStatus.Types.SubQuestStatus.Types.SubQuestType.Arrival;
                            break;
                        case "Hunt":
                            sqs.Type = QuestStatus.Types.SubQuestStatus.Types.SubQuestType.Hunt;
                            break;
                        case "Item":
                            sqs.Type = QuestStatus.Types.SubQuestStatus.Types.SubQuestType.Item;
                            break;
                        case "Action":
                            sqs.Type = QuestStatus.Types.SubQuestStatus.Types.SubQuestType.Action;
                            break;
                    }

                    qs.SubStatus.Add(sqs);
                }

                outInfo.QuestStatus.Add(qs);
            }
        }

        private bool SavePlayerInfo(PlayerInfo saveInfo, ServerCallContext context)
        {
            if (mTotalClients.ContainsKey(context.Peer))
            {
                return false;
            }

            int userIdx = mTotalClients[context.Peer].UserIdx;

            using (MySqlCommand cmd = new MySqlCommand("CALL SavePlayerInfo(@userIdx, @name, @level, @map, @locx, @locy, @locz, @gold)", mSqlConnection))
            {
                cmd.Parameters.AddWithValue("@userIdx", userIdx);
                cmd.Parameters.AddWithValue("@name", saveInfo.Name);
                cmd.Parameters.AddWithValue("@level", saveInfo.Level);
                cmd.Parameters.AddWithValue("@map", saveInfo.Map);
                cmd.Parameters.AddWithValue("@locx", saveInfo.LocX);
                cmd.Parameters.AddWithValue("@locy", saveInfo.LocY);
                cmd.Parameters.AddWithValue("@locz", saveInfo.LocZ);
                cmd.Parameters.AddWithValue("@gold", saveInfo.Gold);

                cmd.ExecuteNonQuery();
            }

            for (int i = 0; i < saveInfo.Inventory.TypeInventory.Count; ++i)
            {
                var typeInventory = saveInfo.Inventory.TypeInventory[i];
                for (int j = 0; j < typeInventory.Items.Count; ++j)
                {
                    var item = typeInventory.Items[j];
                    using (MySqlCommand cmd = new MySqlCommand("CALL SaveInventory(@userIdx, @itemType, @inventoryIndex, @infoIndex, @num)", mSqlConnection))
                    {
                        cmd.Parameters.AddWithValue("@userIdx", userIdx);
                        switch (i)
                        {
                            case 0:
                                cmd.Parameters.AddWithValue("@itemType", "CLOTH");
                                break;
                            case 1:
                                cmd.Parameters.AddWithValue("@itemType", "WEAPON");
                                break;
                            case 2:
                                cmd.Parameters.AddWithValue("@itemType", "ITEM");
                                break;
                            default:
                                //TOOD : Server Data Error
                                break;
                        }
                        cmd.Parameters.AddWithValue("@inventoryIndex", j);
                        cmd.Parameters.AddWithValue("@infoIndex", item.Index);
                        cmd.Parameters.AddWithValue("@num", item.Num);

                        cmd.ExecuteNonQuery();
                    }
                }
            }

            for (int i = 0; i < saveInfo.QuestStatus.Count; ++i)
            {
                var qs = saveInfo.QuestStatus[i];
                for (int j = 0; j < qs.SubStatus.Count; ++j)
                {
                    var sqs = qs.SubStatus[j];

                    using (MySqlCommand cmd = new MySqlCommand("CALL SaveQueststatus(@userIdx, @questIdx, @progress, @subquestIndex, " +
                                                               "@currPhase, @completed, @bStarted, @bCompleted, @currAmount)", mSqlConnection))
                    {
                        cmd.Parameters.AddWithValue("@userIdx", userIdx);
                        cmd.Parameters.AddWithValue("@questIdx", qs.Index);
                        switch (qs.ProgressType)
                        {
                            case QuestStatus.Types.QuestProgressType.Unavailable:
                                cmd.Parameters.AddWithValue("@progress", "Unavailable");
                                break;
                            case QuestStatus.Types.QuestProgressType.Available:
                                cmd.Parameters.AddWithValue("@progress", "Available");
                                break;
                            case QuestStatus.Types.QuestProgressType.Inprogress:
                                cmd.Parameters.AddWithValue("@progress", "InProgress");
                                break;
                            case QuestStatus.Types.QuestProgressType.Completable:
                                cmd.Parameters.AddWithValue("@progress", "Completable");
                                break;
                            case QuestStatus.Types.QuestProgressType.Completed:
                                cmd.Parameters.AddWithValue("@progress", "Completed");
                                break;
                            default:
                                break;
                        }
                        cmd.Parameters.AddWithValue("@subquestIndex", j);
                        cmd.Parameters.AddWithValue("@currPhase", qs.CurrPhase);
                        cmd.Parameters.AddWithValue("@completed", qs.Completed);
                        cmd.Parameters.AddWithValue("@bStarted", sqs.BStarted);
                        cmd.Parameters.AddWithValue("@bCompleted", sqs.BCompleted);
                        cmd.Parameters.AddWithValue("@currAmount", sqs.CurrAmount);

                        cmd.ExecuteNonQuery();
                    }
                }
            }

            return true;
        }

        // db 재연결
        public bool Connect()
        {
            string server = "localhost";
            int port = 3306;
            string database = "portfolio";
            string id = "root";
            string password = "redapple67#";

            string connectionString = string.Format("Server={0};Port={1};Database={2};Uid={3};Pwd={4}", server, port, database, id, password);
            mSqlConnection = new MySqlConnection(connectionString);
            try
            {
                mSqlConnection.Open();
            }
            catch (MySqlException e)
            {
                Console.WriteLine(e.ToString());
                return false;
            }

            return true;
        }

        public void ListClients()
        {
            foreach (var client in mTotalClients)
            {
                Console.WriteLine($"{client.Key}: {client.Value.UserIdx}");
            }
        }
    }
}
