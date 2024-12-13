@echo off

:-----------------------------------------------------------------------------------------------------------------------------
REM  관리자 권한을 확인합니다.
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"

REM 오류 플래그가 설정되어 있으면 관리자 권한이 없습니다.
if '%errorlevel%' NEQ '0' (goto UACPrompt) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params = %*:"=""
    echo UAC.ShellExecute "cmd.exe", "/c %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    REM 관리자 권한으로 실행하는 경우 기본 경로가 달라서, 이를 bat파일 경로(%~dp0)로 변경한다
    CD /D "%~dp0"
:-----------------------------------------------------------------------------------------------------------------------------

echo [Build Sharpmake]
dotnet build --configuration Release .\Engine\Source\Programs\Sharpmake\Sharpmake.Application\Sharpmake.Application.csproj

echo [vcpkg task]
cd Engine\Source\Programs\
if not exist "Engine\Source\Programs\vcpkg" (
	git clone https://github.com/microsoft/vcpkg
	call .\vcpkg\bootstrap-vcpkg.bat
)
.\vcpkg\vcpkg install boost:x64-windows
.\vcpkg\vcpkg integrate install

pause