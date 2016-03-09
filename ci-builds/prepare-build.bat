@echo off

@rem this script assumes, that:
@rem   - BOOST_ROOT variable is pointing to Boost root directory
@rem   - GENERATOR variable is set to proper CMake generator

echo "Running CMake with GENERATOR=%GENERATOR% and BOOST_ROOT=%BOOST_ROOT%"

pushd %cd%

mkdir build-zz
cd build-zz

cmake -G "%GENERATOR%" ..\thrash
if %errorlevel% neq 0 exit /b 1

popd
