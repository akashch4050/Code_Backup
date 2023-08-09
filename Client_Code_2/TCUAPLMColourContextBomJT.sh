. /user/uaprod/.bash_profile
#54422524000R_E
echo "In TCUAPLMColourContextBomJT.sh shell... [$1  $2  $3  $4  $5 $6]"
CLR_CTXT_BOM_JT_BASEDIR=/user/tcuatstk/Tessellation_Programs/ColorJt
#todaysdt="$(date +'%d_%m_%Y_%H_%M_%S')"
todaysdt=$5
ColorBasedBOMJTNameS1=`echo "$1_$4_$todaysdt" | sed 's/[,():+ &\"\\\.]/_/g'`
echo "ColorBasedBOMJTNameS1: $ColorBasedBOMJTNameS1"
mkdir /tmp/$ColorBasedBOMJTNameS1
cd /tmp/$ColorBasedBOMJTNameS1
#mkdir $ColorBasedBOMJTNameS1
#cd $ColorBasedBOMJTNameS1
dir=`pwd`
echo "dir: $dir"
revRule=`echo $6 | tr '_' ' '`
echo $revRule
/user/uaprod/shells/ColorModule/tm_GetPlatformSVRBomWithCompCode -u=loader -pf=/user/uaprod/shells/Admin/loaderpasswdFile.pwf -g=dba -i="$1" -j="$2" -k=$3 -s="$4" -lvl=99
if [ $? != 0 ]
then
echo -e "\n\n Problem in tm_GetPlatformSVRBomWithCompCode for BOM expansion ???? \n"
exit 1
fi
clrschlogCnt=`ls -ltr *.clrschlog | wc -l`
if [ $clrschlogCnt -gt 0 ]
then
        LineNum=0
        LineNumber=0
        ls *.clrschlog | cut -d' ' -f1 | sort -u > clr_RGB_files.txt
        cat clr_RGB_files.txt
        for valClrFile in `ls *.clrschlog | cut -d' ' -f1 | sort -u`
        do
                LineNum=`expr $LineNum + 1`
                while read clrid
                do
                        colourID=`echo $clrid | cut -d"," -f2`
                        echo $colourID >> PLM_"$LineNum".clr
                done < $valClrFile
                sort -u PLM_"$LineNum".clr > PLM_"$LineNum".clr1
                mv PLM_"$LineNum".clr1 PLM_"$LineNum".clr
        done
        for ClrIdsFile in `ls PLM_*.clr | cut -d' ' -f1`
        do
                LineNumber=`expr $LineNumber + 1`
                /user/uaprod/shells/ColorModule/tm_GetRGBofColorID -u=loader -pf=/user/uaprod/shells/Admin/loaderpasswdFile.pwf -g=dba -i=$ClrIdsFile -o="RGBDB_PLM_$LineNumber.txt"
        done
fi
##### Processing for DPDS EXE
cat RGBDB_PLM_*.txt >> RGBDB_PLM_temp.txt
cat RGBDB_PLM_temp.txt | sort -u >> RGBDB_PLM.txt

cp $CLR_CTXT_BOM_JT_BASEDIR/RGBDB.txt /tmp/$ColorBasedBOMJTNameS1
cp $CLR_CTXT_BOM_JT_BASEDIR/RGB_Material.txt /tmp/$ColorBasedBOMJTNameS1

while read line
do
CLRID=""
CLRID=`echo $line | cut -d':' -f1`
SortCLRID=""
SortCLRID=`cat RGBDB_PLM.txt | grep $CLRID`
if [ "x$SortCLRID" == "x" ];then
        echo $line >> RGBDB_PLM.txt
fi
done < RGBDB.txt

cat RGBDB_PLM.txt | sort -u >> RGBDB_PLM_Final_temp.txt
cat RGBDB_PLM_Final_temp.txt | sed -e 's/Transparency,0/Transparency,1/g' > RGBDB_PLM_Final.txt

#############VPX Integration Starts ###############
ls *.clrschlog > ColorSchmInfo.txt
ls -ltr RGBDB.txt RGB_Material.txt RGBDB_PLM_Final.txt RGB_Material.txt
while read line
do
CLRSCHMNAME=""
CLRSCHMNAME=`echo $line | cut -d'.' -f1`
echo "CLRSCHMNAME==$CLRSCHMNAME"
$CLR_CTXT_BOM_JT_BASEDIR/GenVisReport.exe $line RGBDB_PLM_Final.txt RGB_Material.txt $CLRSCHMNAME
done < /tmp/$ColorBasedBOMJTNameS1/ColorSchmInfo.txt
ls -ltr *vpx
if test -s t5MulExpBOMJTFile_"$1"_"$4".txt
then
	$CLR_CTXT_BOM_JT_BASEDIR/LoadPropertiesXML_D t5MulExpBOMJTFile_"$1"_"$4".txt t5MulExpBOMJTFile_"$1"_"$4"
	if [ $? != 0 ]
	then
		echo -e "\n\n Problem in LoadPropertiesXML_D for XML file ???? \n"
		exit 1
	fi
else
	echo "File is not available in directory....."
fi
echo "TCUAPLMColourContextBomJT.sh shell is completed..."
