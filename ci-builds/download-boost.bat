@echo off

@rem this script assumes, that it's first parameter is Boost minor version
@rem this script will export BOOST_ROOT variable pointing to Boost root directory

setlocal enabledelayedexpansion

SET BOOST_VER=%1

if "%BOOST_VER%" == "" (
    echo "Please provide Boost minor version as the first parameter."
    exit /B 1
)

pushd %cd%

SET CI_ROOT="%TEMP%\ci-build-tmp"
SET BOOST_X_ROOT="%CI_ROOT%\boost"

echo "Downloading boost 1.%BOOST_VER% from sourceforge."

set BOOST_DOWNLOAD_URL="http://sourceforge.net/projects/boost/files/boost/1.%BOOST_VER%.0/boost_1_%BOOST_VER%_0.7z/download"
set DOWNLOAD_ROOT="%CI_ROOT%\download"
mkdir "%DOWNLOAD_ROOT%"
mkdir "%BOOST_X_ROOT%"

curl -fSL -o "%DOWNLOAD_ROOT%\boost.7z" "%BOOST_DOWNLOAD_URL%"
if %errorlevel% neq 0 exit /b 1

echo "Extracting Boost."
7z x "%DOWNLOAD_ROOT%\boost.7z" -o "%BOOST_X_ROOT%"
if %errorlevel% neq 0 exit /b 1

SETX BOOST_ROOT="%BOOST_X_ROOT%\boost_1_%BOOST_VER%_0"
echo "Boost succesfully set up in %BOOST_ROOT%"

popd
