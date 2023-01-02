pushd %~dp0
C:\workspace\etc\vcpkg\vcpkg\installed\x64-windows\tools\protobuf\protoc.exe --proto_path=./ --grpc_out=./ --cpp_out=./ ./match.proto --plugin=protoc-gen-grpc=C:\workspace\etc\vcpkg\vcpkg\installed\x64-windows\tools\grpc\grpc_cpp_plugin.exe
PAUSE