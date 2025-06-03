@echo off
setlocal enabledelayedexpansion
echo ====================================================================
echo                     UNREAL ENGINE PLUGIN UPDATER
echo ====================================================================
echo.

REM Get script directory for finding the config file
set BATCH_DIR=%~dp0

REM Read configuration from config.ini
set CONFIG_FILE=%BATCH_DIR%UpdaterConfig.ini

if not exist "%CONFIG_FILE%" (
    echo Configuration file not found: %CONFIG_FILE%
    echo Preparing to create the configuration file...
    echo.
    
    REM Ask user for Unreal Engine base path
    echo Please enter the base directory containing Unreal Engine installations
    echo [Press enter for default: C:\Program Files\Epic Games\Unreal Engine]
    set /p USER_UE_PATH="> "
    if "!USER_UE_PATH!"=="" set USER_UE_PATH=C:\Program Files\Epic Games\Unreal Engine
    echo.

    REM Ask user for input base path
    echo Please enter the base directory where the old/updatable plugins are stored
    echo [Press enter for default: %BATCH_DIR%OldPlugins]
    set /p USER_INPUT_PATH="> "
    if "!USER_INPUT_PATH!"=="" set USER_INPUT_PATH=%BATCH_DIR%OldPlugins
    
    REM Create default input directory if it doesn't exist
    if "!USER_INPUT_PATH!"=="%BATCH_DIR%OldPlugins" (
        if not exist "%BATCH_DIR%OldPlugins" (
            echo Creating default input directory: %BATCH_DIR%OldPlugins
            echo.
            mkdir "%BATCH_DIR%OldPlugins" 2>nul
            if errorlevel 1 (
                echo ERROR: Failed to create output directory
                echo Please check if you have permission to create this directory.
            )
        )
    )
    echo.
    
    REM Ask user for output base path
    echo Please enter the base directory where updated plugins will be stored
    echo [Press enter for default: %BATCH_DIR%UpdatedPlugins]
    set /p USER_OUTPUT_PATH="> "
    if "!USER_OUTPUT_PATH!"=="" set USER_OUTPUT_PATH=%BATCH_DIR%UpdatedPlugins
    echo.

    REM Create config.ini with default values
    (
        echo REM Configuration file for UpdatePlugin.bat
        echo REM Edit these paths to match your system
        echo.
        echo REM Base directory containing Unreal Engine installations
        echo UE_BASE_PATH=!USER_UE_PATH!
        echo.
        echo REM Base directory where old/updatable plugins are stored
        echo INPUT_BASE_PATH=!USER_INPUT_PATH!
        echo.
        echo REM Base directory where updated plugins will be stored
        echo OUTPUT_BASE_PATH=!USER_OUTPUT_PATH!
    ) > "%CONFIG_FILE%"
    
    echo Default configuration file created at: %CONFIG_FILE%
    echo Delete or edit this file to change the paths if necessary.
    echo.
) else (
    echo Using configuration file : %CONFIG_FILE%
    echo.
)

REM Read UE_BASE_PATH from config file
for /f "tokens=1,2 delims==" %%a in ('type "%CONFIG_FILE%" ^| findstr /r /c:"^UE_BASE_PATH="') do (
    set UE_BASE_PATH=%%b
)

REM Read UE_BASE_PATH from config file
for /f "tokens=1,2 delims==" %%a in ('type "%CONFIG_FILE%" ^| findstr /r /c:"^INPUT_BASE_PATH="') do (
    set INPUT_BASE_PATH=%%b
)

REM Read OUTPUT_BASE_PATH from config file
for /f "tokens=1,2 delims==" %%a in ('type "%CONFIG_FILE%" ^| findstr /r /c:"^OUTPUT_BASE_PATH="') do (
    set OUTPUT_BASE_PATH=%%b
)

REM Check if the Unreal Engine base path exists
if not exist "!UE_BASE_PATH!" (
    echo ERROR: Unreal Engine path "!UE_BASE_PATH!" not found.
    echo Please check the path in !CONFIG_FILE! and try again.
    pause
    exit /b 1
)

REM Check if the plugin input base path exists
if not exist "!INPUT_BASE_PATH!" (
    echo ERROR: Plugin input path "!INPUT_BASE_PATH!" not found.
    echo Please check the path in !CONFIG_FILE! and try again.
    pause
    exit /b 1
)

REM Check and create if the plugin output base path does not exist
if not exist "!OUTPUT_BASE_PATH!" (
    echo Creating output directory: !OUTPUT_BASE_PATH!
    mkdir "!OUTPUT_BASE_PATH!" 2>nul
    if errorlevel 1 (
        echo.
        echo ERROR: Failed to create output directory "!OUTPUT_BASE_PATH!"
        echo Please check if you have permission to create this directory.
        pause
        exit /b 1
    )
)

echo Enter the Unreal Engine version you want to update to (example: 5.5):
set /p UE_VERSION="> "
set UE_PATH=!UE_BASE_PATH!\UE_!UE_VERSION!
echo.

REM Check if the specific UE version path exists
if not exist "!UE_PATH!" (
    echo ERROR: Unreal Engine version path "!UE_PATH!" not found.
    echo Please check that UE_!UE_VERSION! exists in the specified Unreal Engine folder.
    pause
    exit /b 1
)

set BATCHFILES=!UE_PATH!\Engine\Build\BatchFiles

REM Check if BatchFiles directory exists
if not exist "!BATCHFILES!" (
    echo ERROR: BatchFiles directory "!BATCHFILES!" not found.
    echo Please check that this is a valid Unreal Engine installation.
    pause
    exit /b 1
)

REM Extract drive letter from UE_PATH
set UE_DRIVE=!UE_PATH:~0,2!
!UE_DRIVE!
cd !BATCHFILES!

echo Enter the name of the plugin you want to update (example: MyPlugin):
set /p PLUGIN_NAME="> "
set PLUGIN_PATH=!INPUT_BASE_PATH!\!PLUGIN_NAME!
echo.

REM Check if the plugin directory exists
if not exist "!PLUGIN_PATH!" (
    echo ERROR: Plugin directory "!PLUGIN_PATH!" not found.
    echo Please check the plugin name/directory and try again.
    pause
    exit /b 1
)

REM Find the .uplugin file in the specified directory
set UPLUGIN_PATH=
for %%F in ("!PLUGIN_PATH!\*.uplugin") do (
    set UPLUGIN_PATH=%%F
    goto :found_uplugin
)

:found_uplugin
if "!UPLUGIN_PATH!"=="" (
    echo ERROR: No .uplugin file found in "!PLUGIN_PATH!"
    echo Please check the path and integrity of the plugin and try again.
    pause
    exit /b 1
)

echo Found .uplugin file: !UPLUGIN_PATH!
echo.

REM Ask user for save directory name
echo Please enter the directory name where the updated plugin will be saved
echo [Press enter for default: %PLUGIN_NAME% %UE_VERSION%]
set /p FOLDER="> "
if "!FOLDER!"=="" set FOLDER=%PLUGIN_NAME% %UE_VERSION%
echo.

set OUTPUT_PATH=!OUTPUT_BASE_PATH!\!FOLDER!

REM Create output directory if it doesn't exist
if not exist "!OUTPUT_PATH!" (
    echo Creating output directory: !OUTPUT_PATH!
    echo.
    mkdir "!OUTPUT_PATH!" 2>nul
    if errorlevel 1 (
        echo ERROR: Failed to create output directory "!OUTPUT_PATH!"
        echo Please check if you have permission to create this directory.
        pause
        exit /b 1
    )
)

echo ====================================================================
echo                    Running BuildPlugin command...
echo ====================================================================
echo.
RunUAT.bat BuildPlugin -plugin="!UPLUGIN_PATH!" -package="!OUTPUT_PATH!"
if errorlevel 1 (
    echo.
    echo ERROR: BuildPlugin command failed.
    pause
    exit /b 1
)

echo.
echo ====================================================================
echo Plugin successfully built and packaged to: 
echo !OUTPUT_PATH!
echo ====================================================================
echo.
pause