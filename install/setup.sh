#!/bin/sh

# gets directory of this file to find the others
INSTALLPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

echo Installing dependencies

sh $INSTALLPATH/setupDependencies.sh

echo Creating build directories

sh $INSTALLPATH/setupBuild.sh

echo Done

