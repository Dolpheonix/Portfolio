use portfolio;

create TABLE UserInfo
(
	id VARCHAR(100) NOT NULL,
    pw VARCHAR(100) NOT NULL,
    userIdx INT NOT NULL AUTO_INCREMENT PRIMARY KEY
);
create TABLE PlayerInfo
(
	userIdx 	INT NOT NULL,
    nickname 	VARCHAR(100) NOT NULL,
    playerLevel	INT NOT NULL,
    mapIdx		INT NOT NULL,
    loc_x		FLOAT NOT NULL,
    loc_y		FLOAT NOT NULL,
    loc_z		FLOAT NOT NULL,
    gold		BIGINT NOT NULL
);
create TABLE Inventory
(
	userIdx 		INT NOT NULL,
    itemType 		ENUM('CLOTH', 'WEAPON', 'ITEM') NOT NULL,
	inventoryIdx 	INT NOT NULL,
    infoIndex 		INT NOT NULL,
    num 			INT NOT NULL
);
create TABLE QuestStatus
(
	userIdx		INT NOT NULL,
    questIdx	INT NOT NULL,
    progress	ENUM('Unavailable', 'Available', 'InProgress', 'Completable', 'Completed') NOT NULL,
    subquestIdx INT NOT NULL,
    currPhase	INT NOT NULL,
    completed	INT NOT NULL,
    bStarted	BOOLEAN NOT NULL,
    bCompleted	BOOLEAN NOT NULL,
    currAmount	INT NOT NULL
);
