#!/bin/bash

. /user/omfprod/.bash_profile

export ORACLE_HOME=/user/oraclnt/10ghome/lib
export PATH=/proj/PLMLoading/UAFiles/EstimateSheet_TCE_TCUA_Migration:$PATH
now="$(date +'%d_%m_%Y')"
echo "Starting backUp at $now, please wait..."
mkdir $now
WorkingPath="/proj/PLMLoading/UAFiles/EstimateSheet_TCE_TCUA_Migration/"
cd ${WorkingPath}
${WorkingPath}GetESData Loader 123loader 5442 CAR + $WorkingPath$now 1 "" "" > $WorkingPath$now/$now.log &
wait
if [ $? != 0 ]
	then
		echo "Problem in GetESData data pulling.."
		exit 1
	else
		echo "GetESData data pulling Executed Successfully .."
		ssh aplstdgrp@plmpapp2c4 -n "sh /user/aplstdgrp/Dhanashri/EstimateSheet_TCE_TCUA_Migration/IncrementalESDataLoadToTCUA.sh $now"			

fi