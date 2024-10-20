@echo off
set GRPC_CSHARP_PLUGIN_PATH=C:\vcpkg\installed\x64-windows\tools\grpc\grpc_csharp_plugin.exe
set PROTO_DIR=.

protoc -I %PROTO_DIR% --grpc_out=%PROTO_DIR% --plugin=protoc-gen-grpc=%GRPC_CSHARP_PLUGIN_PATH% %PROTO_DIR%\ProtoObject.proto
protoc -I %PROTO_DIR% --csharp_out=%PROTO_DIR% %PROTO_DIR%\ProtoObject.proto

echo Compilation finished
pause