
echo Installing dependencies

call %~dp0\setupDependencies.bat

echo Creating build directories

call %~dp0\setupBuild.bat

echo Done
