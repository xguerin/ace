#!/bin/bash

OCLINT_FLAGS="                -rc SHORT_VARIABLE_NAME=1"
OCLINT_FLAGS="${OCLINT_FLAGS} -rc NCSS_METHOD=40"
OCLINT_FLAGS="${OCLINT_FLAGS} -rc LONG_METHOD=100"
OCLINT_FLAGS="${OCLINT_FLAGS} -rc LONG_VARIABLE_NAME=50"

OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=AvoidDefaultArgumentsOnVirtualMethods"
OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=AvoidPrivateStaticMembers"
OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=BitwiseOperatorInConditional"
OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=GotoStatement"
OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=MissingDefaultStatement"
OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=ParameterReassignment"
OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=TooFewBranchesInSwitchStatement"
OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=UnnecessaryElseStatement"
OCLINT_FLAGS="${OCLINT_FLAGS} -disable-rule=InvertedLogic"

# Lint the source files
#
which oclint-json-compilation-database 2>&1 >/dev/null

if [ $? == 0 ]
then
	oclint-json-compilation-database -p $BUILD_DIR -e $BUILD_DIR -- ${OCLINT_FLAGS} -o $BUILD_DIR/oclint_report.txt
else
  echo "Error: linting requires OCLint"
fi
