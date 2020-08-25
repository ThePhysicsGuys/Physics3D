#!/bin/sh

# gets directory of this file to find the others
INSTALLPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

echo Installing dependencies

sh $INSTALLPATH/setupDependenciesUbuntu.sh

echo Creating build directories

sh $INSTALLPATH/setupBuildUbuntu.sh

echo Done

