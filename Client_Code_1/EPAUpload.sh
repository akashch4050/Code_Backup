######################################################################################
# Copyright (c) 2004 Tata Technologies Limited (TTL). All Rights Reserved.
#
# This software is the confidential and proprietary information of TTL.
# You shall not disclose such confidential information and shall use it
# only in accordance with the terms of the license agreement.
#
# Author		  : Dayanand Amdapure
# Created on	  : Nov 23, 2018
# Project         : TATA MOTORS PLM
#
#######################################################################################

#user=$1
#pass=$2


echo "Loading EPA In TCUA"
/home/infodba/EPAMigration/EPACreate -i=EPAMetaDataDetails.txt
if [ $? != 0 ]
then
	echo "Problem in EPACreate"
	exit 1
fi
