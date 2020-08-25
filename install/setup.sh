#!/bin/sh

echo Installing dependencies

sh setupDependencies.sh

echo Creating build directories

sh setupBuild.sh

echo Done
