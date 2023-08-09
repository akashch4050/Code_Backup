#!/bin/bash

. /user/omfprod/.bash_profile

export ORACLE_HOME=/user/oraclnt/10ghome/lib
export PATH=/proj/PLMLoading/UAFiles/APL_STDSI_TCE_TCUA_Data_Mig/IncrementalDataUpload_APL_CarryOverPart:$PATH
now="$(date +'%d_%m_%Y')"
echo "Starting backUp at $now, please wait..."
cd /proj/PLMLoading/UAFiles/APL_STDSI_TCE_TCUA_Data_Mig/IncrementalDataUpload_APL_CarryOverPart
mkdir $now
WorkingPath="/proj/PLMLoading/UAFiles/APL_STDSI_TCE_TCUA_Data_Mig/IncrementalDataUpload_APL_CarryOverPart"
cd ${WorkingPath}
echo "CarryOver data pulling Executing .."
ssh aplstdgrp@plmpapp2c4 -n "sh /user/aplstdgrp/Deepti/TCE_TCUA_CarryOver_Stamp/IncrementalDataStamp/IncrementalTCUADataCarryOverLoad.sh"

cd /proj/PLMLoading/UAFiles/APL_STDSI_TCE_TCUA_Data_Mig/IncrementalDataUpload_APL_CarryOverPart/$now/
cp /proj/PLMLoading/UAFiles/APL_STDSI_TCE_TCUA_Data_Mig/IncrementalDataUpload_APL_CarryOverPart/Part_List_$now.txt .

t5_CarryOverPartStamp dmm06484 abc123 Part_List_$now.txt Part_List_Info_$now.txt > Part_Data_TCE_Info.log 

echo "CarryOver data pulling Executing completed..passing for data stamping in TCUA"

ssh aplstdgrp@plmpapp2c4 -n "sh /user/aplstdgrp/Deepti/TCE_TCUA_CarryOver_Stamp/IncrementalDataStamp/IncrementalTCUADataCarryOverStamp.sh"

