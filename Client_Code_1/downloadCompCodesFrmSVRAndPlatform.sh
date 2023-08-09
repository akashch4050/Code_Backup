. /user/uaprod/.bash_profile
echo "In Shell downloadCompCodesFrmSVRAndPlatform.sh start ...$1 $2 $3 $4"
cd /tmp/
/user/uaprod/shells/ColorModule/tm_GetCompCodesFrmSVRAndPlatform -u=loader -pf=/user/uaprod/shells/Admin/loaderpasswdFile.pwf -g=dba -i="$1" -j="$2" -k="$3" -s="$4" -lvl=90
echo "Sorting ComCode File..."
sort -u "$1"_"$4"_CompCode.txt > "$1"_"$4"_CompCode.txt1
mv "$1"_"$4"_CompCode.txt1 "$1"_"$4"_CompCode.txt
chmod 777 "$1"_"$4"_CompCode.txt
echo "Shell downloadCompCodesFrmSVRAndPlatform.sh processing is completed."
