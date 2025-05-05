@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

REM === 参数检测 ===
set CONFIG=%1
set ARCH=%2

if "%CONFIG%"=="" (
    echo [ERROR] Configuration not specified（Debug 或 Release）
    goto :eof
)

if "%ARCH%"=="" (
    echo [ERROR] Platform not specified（x86 或 x64）
    goto :eof
)

REM === 变量初始化 ===
set BASE_LIB=
set OUTPUT_LIB=
set VCPKG_LIB_DIR=

if /i "%ARCH%"=="x64" (
    if /i "%CONFIG%"=="Debug" (
        set BASE_LIB=..\x64\Debug\geek_base.lib
        set OUTPUT_LIB=..\x64\Debug\geek.lib
        set VCPKG_LIB_DIR=..\vcpkg_installed\x64-windows-static\x64-windows-static\debug\lib
    ) else (
        set BASE_LIB=..\x64\Release\geek_base.lib
        set OUTPUT_LIB=..\x64\Release\geek.lib
        set VCPKG_LIB_DIR=..\vcpkg_installed\x64-windows-static\x64-windows-static\lib
    )
) else (
    if /i "%CONFIG%"=="Debug" (
        set BASE_LIB=..\Debug\geek_base.lib
        set OUTPUT_LIB=..\Debug\geek.lib
        set VCPKG_LIB_DIR=..\vcpkg_installed\x86-windows-static\x86-windows-static\debug\lib
    ) else (
        set BASE_LIB=..\Release\geek_base.lib
        set OUTPUT_LIB=..\Release\geek.lib
        set VCPKG_LIB_DIR=..\vcpkg_installed\x86-windows-static\x86-windows-static\lib
    )
)

REM === 检查路径 ===
if not exist "%BASE_LIB%" (
    echo [ERROR] geek_base.lib not found: "%BASE_LIB%"
    goto :eof
)

if not exist "%VCPKG_LIB_DIR%" (
    echo [ERROR] vcpkg lib directory not found: "%VCPKG_LIB_DIR%"
    goto :eof
)

REM === 删除已有输出文件 ===
if exist "%OUTPUT_LIB%" del /f /q "%OUTPUT_LIB%"

REM === 构建合并命令 ===
set LIB_CMD=lib /OUT:"%OUTPUT_LIB%" "%BASE_LIB%"

for %%f in ("%VCPKG_LIB_DIR%\*.lib") do (
    set LIB_CMD=!LIB_CMD! "%%f"
)

echo [COMMAND] !LIB_CMD!
call !LIB_CMD!

echo [DONE] Successfully generated: %OUTPUT_LIB%
endlocal
