@echo off

pushd build
cl -FC -Zi -DTB_WINDOWS=1 ..\CppTwitchSoundBot\Main.cpp ..\CppTwitchSoundBot\TwitchSocket.cpp /EHsc
popd

pushd assets
start ..\build\Main.exe
popd