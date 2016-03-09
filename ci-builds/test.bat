@echo off

pushd %cd%

echo "Testing..."

.\build-zz\Debug\Example-Person
if %errorlevel% neq 0 exit /b 1

echo "Testing done."

popd
