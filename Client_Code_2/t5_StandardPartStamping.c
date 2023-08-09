/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File				:   t5GetErcPartDetails.c
*  Author			:   Deepti Meshram
*  Module			:   TCUA Downloader.
*  Purpose			:   To get Part CS,StoreLocation,Rev Eff of APL,STDSI and LCS for input=InputPartList.txt(Partno^Rev^Seq)
*
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes
***************************************************************************/

#include <cl.h>
#include <usc.h>
#include <pdmroot.h>
#include <pdmsessn.h>
#include <relation.h>
#include <pdmc.h>
#include <msglcm.h>
#include <msgapc.h>
#include <msgtel.h>
#include <status.h>
#include <sc.h>
#include <ft.h>
#include <ReadFile.h>
#include <tel.h>
#include <Mtimsgh.h>
#define NUMBER 5000

char* ChangeDateFormat(char*,  char*,  char*, char *, char *);

/****************FUNCTION TO GET PART DETAILS(CS,STORELOCATION,LCS,APL EFF,STD EFF)********************************/
status GetTCPartInfoForAPL(ObjectPtr TCPartObjP,string sIndicator,string StoreLocation,string configCtxt,FILE* fp,integer* mfail )
{
	

	string t5CarStLoc=NULL;
	string t5CarStLocDup=NULL;
	string t5CarMakeBuyInd=NULL;
	string t5CarMakeBuyIndDup=NULL;
	string t5PartNumber=NULL;
	string t5PartNumberDup=NULL;
	string t5PartNumberRev=NULL;
	string t5PartNumberRevDup=NULL;
	string t5PartNumberSeq=NULL;
	string t5PartNumberSeqDup=NULL;
	string t5PartNumberLCS=NULL;
	string t5PartNumberLCSDup=NULL;
	string APLReveffDateFrom=NULL;
	string APLReveffDateFromDup=NULL;
	string APLReveffDateTo=NULL;
	string APLReveffDateToDup=NULL;
	string stdReveffDateFrom=NULL;
	string stdReveffDateFromDup=NULL;
	string stdReveffDateTo=NULL;
	string stdReveffDateToDup=NULL;
	string t5PartOrgID=NULL;
	string t5PartOrgIDDup=NULL; //END
	string taskNoDup=NULL;
	string DMLNo=NULL;
	string	DMLNoDup=NULL;
	string DMLEcnType=NULL;
	string					DMLEcnTypeDup=NULL;
	string DMLAPLRlzDt=NULL;
	string					DMLAPLRlzDtDup=NULL;
	string			taskNo=NULL;
	string			EpaPlantNameDup=NULL;
	string			EpaPlantName=NULL;
	string			EPAStat=NULL;
	string			EPAStatDup=NULL;
	ObjectPtr		genContextObj	= NULL;
	ObjectPtr		contextObj		= NULL;
	
	SetOfObjects	aplExpObj		= NULL;
	SetOfObjects	aplrelObjSet	= NULL;
	SetOfObjects	stdExpObj		= NULL;
	SetOfObjects	stdrelObjSet	= NULL;
	SetOfObjects	soTaskObjs	= NULL;
	SetOfObjects	soDmlObjs	= NULL;
	SetOfObjects	CurrEPAObjs	= NULL;
	SetOfObjects	EPAObjSo	= NULL;
	string GlblPlantName = NULL;
	string myPlantDup = NULL;
	string taskNoLCS=NULL;
	string		taskNoLCSDup=NULL;

	string DescriptionVal=NULL; //END
	string DescriptionValDup=NULL; //END
	string PartTypeVal=NULL; //END
	string PartTypeValDup=NULL; //END

	string t5CMVRCertificationS =	NULL;
	string t5CMVRCertificationDupS =	NULL;
	string t5ClassificationHazDupS =	NULL;
	string t5ClassificationHazS =	NULL;
	string t5ColourIDS =	NULL;
	string t5ColourIDDupS =	NULL;
	string t5ConfigIDS =	NULL;
	string t5ConfigIDDupS =	NULL;
	string t5DismantableS =	NULL;
	string t5DismantableDupS =	NULL;
	string t5DsgnDeptS =	NULL;
	string t5DsgnDeptDupS =	NULL;
	string t5EnvelopeDimenS =	NULL;
	string t5EnvelopeDimenDupS =	NULL;
	string t5HazardousContS =	NULL;
	string t5HazardousContDupS =	NULL;
	string t5HomologationReqdS =	NULL;
	string t5HomologationReqdDupS =	NULL;
	string t5ListRecSparess =	NULL;
	string t5ListRecSparesDups =	NULL;
	string t5NcPartNoS =	NULL;
	string t5PartCodeS =	NULL;
	string t5NcPartNoDupS =	NULL;
	string t5PartCodeDupS =	NULL;
	string t5PartPropertyS =	NULL;
	string t5PartPropertyDupS =	NULL;
	string t5PartStatusS =	NULL;
	string t5PartStatusDupS =	NULL;
	string t5PkgStdS =	NULL;
	string t5PkgStdDupS =	NULL;
	string t5ProductS =	NULL;
	string t5ProductDupS =	NULL;
	string t5PrtCatCodeS =	NULL;
	string t5PrtCatCodeDupS =	NULL;
	string t5RecoverableS =	NULL;
	string t5RecoverableDupS =	NULL;
	string t5RecyclabilityS =	NULL;
	string t5RecyclabilityDupS =	NULL;
	string t5RefPartNumberS =	NULL;
	string t5RefPartNumberDupS =	NULL;
	string t5ReliabilityS =	NULL;
	string t5ReliabilityDupS =	NULL;
	string t5SpareCriteriaS =	NULL;
	string t5SpareCriteriaDupS =	NULL;
	string t5SpareIndS =	NULL;
	string t5SpareIndDupS =	NULL;
	string t5SurfPrtStdS =	NULL;
	string t5SurfPrtStdDupS =	NULL;
	string t5SamplesToApprS =	NULL;
	string t5SamplesToApprDupS =	NULL;
	string t5AsmDisposalS =	NULL;
	string t5AsmDisposalDupS =	NULL;
	string t5FinDisposalInstrS =	NULL;
	string t5FinDisposalInstrDupS =	NULL;
	string t5RPDisposalInstrS =	NULL;
	string t5RPDisposalInstrDupS =	NULL;
	string t5SPDisposalInstrS =	NULL;
	string t5SPDisposalInstrDupS =	NULL;
	string t5WIPDisposalInstrS =	NULL;
	string t5WIPDisposalInstrDupS =	NULL;
	string t5LastModByS =	NULL;
	string t5LastModByDupS =	NULL;
	string t5VerCreatorS =	NULL;
	string t5VerCreatorDupS =	NULL;
	string t5CAEDocES =	NULL;
	string t5CAEDocEDupS =	NULL;
	string t5CoatedS =	NULL;
	string t5CoatedDupS =	NULL;
	string t5ConvDocS =	NULL;
	string t5ConvDocDupS =	NULL;
	string t5AplCopyOfErcRevS =	NULL;
	string t5AplCopyOfErcRevDupS =	NULL;
	string t5AplCopyOfErcSeqS =	NULL;
	string t5AplCopyOfErcSeqDupS =	NULL;
	string t5AppCodeS =	NULL;
	string t5AppCodeDupS =	NULL;
	string t5RqstNumS =	NULL;
	string t5RqstNumDupS =	NULL;
	string t5RsnCodeS =	NULL;
	string t5RsnCodeDupS =	NULL;
	string t5SurfaceAreaS =	NULL;
	string t5SurfaceAreaDupS =	NULL;
	string t5VolumeS =	NULL;
	string t5VolumeDupS =	NULL;
	string t5ErcIndNameS =	NULL;
	string t5ErcIndNameDupS =	NULL;
	string t5PostRelReqS =	NULL;
	string t5PostRelReqDupS =	NULL;
	string t5ItmCategoryS =	NULL;
	string t5ItmCategoryDupS =	NULL;
	string t5CopReqS =	NULL;
	string t5CopReqDupS =	NULL;
	string t5AplInvalidateS =	NULL;
	string t5AplInvalidateDupS =	NULL;
	string t5PrtValiStatusS =	NULL;
	string t5PrtValiStatusDupS =	NULL;
	string t5DRSubStateS =	NULL;
	string t5DRSubStateDupS =	NULL;
	string t5KnxtDocIndS =	NULL;
	string t5KnxtDocIndDupS =	NULL;
	string t5SimValS =	NULL;
	string t5SimValDupS =	NULL;
	string t5PerYieldS =	NULL;
	string t5PerYieldDupS =	NULL;

	string PartCreDateDupS =	NULL;
	string PartCreDateS =	NULL;
	string PartModDateDupS =	NULL;
	string PartModDateS =	NULL;
	string PartCreatorDup =	NULL;
	string PartCreator =	NULL;


	string t5AltPartNoS =	NULL;
	string t5AltPartNoDupS =	NULL;
	string t5RolledupWtS =	NULL;
	string t5RolledupWtDupS =	NULL;
	string t5EstSheetReqdS =	NULL;
	string t5EstSheetReqdDupS =	NULL;
	string t5PFDModReqdS =	NULL;
	string t5PFDModReqdDupS =	NULL;
	string t5ToolIndentReqdS =	NULL;
	string t5ToolIndentReqdDupS =	NULL;
	string CategoryNameDupS =	NULL;
	string CategoryNameS =	NULL;
	string ModDescriptionDupS=NULL;
	string ModDescriptionS=NULL;
	string DrawingNoDupS=NULL;
	string DrawingNoS=NULL;
	string DrawingIndDupS=NULL;
	string DrawingIndS=NULL;
	string MaterialDupS=NULL;
	string MaterialS=NULL;
	string MaterialInDrwDupS=NULL;
	string MaterialInDrwS=NULL;
	string DeignOwnUnitDupS=NULL;
	string DeignOwnUnitS=NULL;
	string ModelIndDupS=NULL;
	string ModelIndS=NULL;
	string LeftRhDupS=NULL;
	string LeftRhS=NULL;
	string ColourIndDupS=NULL;
	string ColourIndS=NULL;
	string WeightDupS=NULL;
	string WeightS=NULL;
	string MaterialThickNessDupS=NULL;
	string MaterialThickNessS=NULL;
	string ProjectCodeDupS=NULL;
	string ProjectCodeS=NULL;
	string DesignGroupDupS=NULL;
	string DesignGroupS=NULL;
	string UnitOfMeasureS=NULL;
	string UnitOfMeasureDupS=NULL;

	
	int dmlCnt=0;
	int stdsiRlzDMLFnd=0;
	int aplRlzDMLFnd=0;
	int stdsiWrkDMLFnd=0;
	int aplWrkDMLFnd=0;
	int mepacnt=0;
	int GotEPASamePltFlg=0;
	int GotEPASamePltLCSRlzd=0;
	SetOfObjects	partMasterSO=NULL;
	SetOfObjects	partMasterRelSO=NULL;
	ObjectPtr		MasterObjOP=NULL;


	t5MethodInit("GetTCPartInfoForAPL");

	printf("\n Calling GetTCPartInfoForAPL......%s:%s:%s",configCtxt,sIndicator,StoreLocation);fflush(stdout);

	
	if(dstat=objGetAttribute(TCPartObjP,PartNumberAttr,&t5PartNumber))goto EXIT;
	if(!nlsIsStrNull(t5PartNumber))t5PartNumberDup=nlsStrDup(t5PartNumber);

	if(dstat=objGetAttribute(TCPartObjP,RevisionAttr,&t5PartNumberRev))goto EXIT;
	if(!nlsIsStrNull(t5PartNumberRev))t5PartNumberRevDup=nlsStrDup(t5PartNumberRev);

	if(dstat=objGetAttribute(TCPartObjP,SequenceAttr,&t5PartNumberSeq))goto EXIT;
	if(!nlsIsStrNull(t5PartNumberSeq))t5PartNumberSeqDup=nlsStrDup(t5PartNumberSeq);

	if(dstat=objGetAttribute(TCPartObjP,sIndicator,&t5CarMakeBuyInd))goto EXIT;
	if(!nlsIsStrNull(t5CarMakeBuyInd))t5CarMakeBuyIndDup=nlsStrDup(t5CarMakeBuyInd);

	if(dstat=objGetAttribute(TCPartObjP,StoreLocation,&t5CarStLoc))goto EXIT;
	if(!nlsIsStrNull(t5CarStLoc))t5CarStLocDup=nlsStrDup(t5CarStLoc);			
	
	if(dstat=objGetAttribute(TCPartObjP,LifeCycleStateAttr,&t5PartNumberLCS))goto EXIT;
	if(!nlsIsStrNull(t5PartNumberLCS))t5PartNumberLCSDup=nlsStrDup(t5PartNumberLCS);
	
	if(dstat=objGetAttribute(TCPartObjP,OrganizationIDAttr,&t5PartOrgID))goto EXIT;
	if(!nlsIsStrNull(t5PartOrgID))t5PartOrgIDDup=nlsStrDup(t5PartOrgID);

	printf("\n PartDetials :%s,%s,%s,%s,%s,%s,%s",t5PartNumberDup,t5PartNumberRevDup,t5PartNumberSeqDup,
		t5CarMakeBuyIndDup,t5CarStLocDup,t5PartNumberLCSDup,t5PartOrgIDDup);fflush(stdout);


	t5CheckMfail(IntSetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObj,mfail));
	t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass, genContextObj, &contextObj, mfail));
	t5CheckDstat(objSetAttribute(contextObj,CfgItemIdAttr,configCtxt));
	t5CheckMfail(SetNavigateViewPref(contextObj,TRUE,"EAS","APL",mfail));
	t5CheckDstat(objSetObject(genContextObj, ConfigCtxtBlobAttr, contextObj));
	t5CheckMfail(ExpandRelationWithCtxt(RevEffDClass,TCPartObjP,"RvfCfgItemInStrBIApc",genContextObj,SC_SCOPE_OF_SESSION ,NULL, &aplExpObj, &aplrelObjSet, mfail));
	printf("\n  setSize(aplExpObj):%d \n",setSize(aplExpObj)); fflush(stdout);
	if(setSize(aplExpObj)>0)
	{
		if(dstat = objGetAttribute(setGet(aplrelObjSet,0),DateEffectiveFromAttr,&APLReveffDateFrom)) goto EXIT;
		if(!nlsIsStrNull(APLReveffDateFrom)) APLReveffDateFromDup = nlsStrDup(APLReveffDateFrom);

		if(dstat = objGetAttribute(setGet(aplrelObjSet,0),DateEffectiveToAttr,&APLReveffDateTo)) goto EXIT;
		if(!nlsIsStrNull(APLReveffDateTo))	APLReveffDateToDup = nlsStrDup(APLReveffDateTo);
	}
	else
	{
		APLReveffDateFromDup = nlsStrDup("-");
		APLReveffDateToDup = nlsStrDup("-");
	}
	printf("\n APLReveffDateFromDup::%s \n", APLReveffDateFromDup);fflush(stdout);
	printf("\n APLReveffDateToDup::%s \n", APLReveffDateToDup);fflush(stdout);
	
	t5CheckMfail(IntSetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObj,mfail));
	t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass, genContextObj, &contextObj, mfail));
	t5CheckDstat(objSetAttribute(contextObj,CfgItemIdAttr,configCtxt));
	t5CheckMfail(SetNavigateViewPref(contextObj,TRUE,"EAS","STD",mfail));
	t5CheckDstat(objSetObject(genContextObj, ConfigCtxtBlobAttr, contextObj));
	t5CheckMfail(ExpandRelationWithCtxt(RevEffDClass,TCPartObjP,"RvfCfgItemInStrBIApc",genContextObj,SC_SCOPE_OF_SESSION ,NULL, &stdExpObj, &stdrelObjSet, mfail));
	printf("\n  setSize(stdExpObj):%d \n",setSize(stdExpObj)); fflush(stdout);
	if(setSize(stdExpObj)>0)
	{
		if(dstat = objGetAttribute(setGet(stdrelObjSet,0),DateEffectiveFromAttr,&stdReveffDateFrom)) goto EXIT;
		if(!nlsIsStrNull(stdReveffDateFrom)) stdReveffDateFromDup = nlsStrDup(stdReveffDateFrom);

		if(dstat = objGetAttribute(setGet(stdrelObjSet,0),DateEffectiveToAttr,&stdReveffDateTo)) goto EXIT;
		if(!nlsIsStrNull(stdReveffDateTo))	stdReveffDateToDup = nlsStrDup(stdReveffDateTo);
	}
	else
	{
		stdReveffDateFromDup = nlsStrDup("-");
		stdReveffDateToDup = nlsStrDup("-");
	}
		

		fprintf(fp,"0");//(level);
		fprintf(fp,"^");
		fprintf(fp,"%s",t5PartNumberDup);			//PARTNUMBER;								
		fprintf(fp,"^");
		fprintf(fp,"%s",t5PartNumberRevDup);		//PARTNUMBER REV;
		fprintf(fp,"^");
		fprintf(fp,"%s",t5PartNumberSeqDup);		//PARTNUMBER SEQ;
		fprintf(fp,"^");
		if(!nlsIsStrNull(t5PartOrgIDDup))		//PARTNUMBER ORGID;
		{
			fprintf(fp,"%s",t5PartOrgIDDup);
			fprintf(fp,"^");
		}
		else
		{
			fprintf(fp," ^");
		}

		if(!nlsIsStrNull(t5CarMakeBuyIndDup))		//PARTNUMBER CARMAKEBUY;
		{
			fprintf(fp,"%s",t5CarMakeBuyIndDup);
			fprintf(fp,"^");
		}
		else
		{
			fprintf(fp," ^");
		}

		if(!nlsIsStrNull(t5CarStLocDup))			//PARTNUMBER CAR STORE LOCATION;
		{
			fprintf(fp,"%s",t5CarStLocDup);
			fprintf(fp,"^");
		}
		else
		{
			fprintf(fp," ^");
		}

		if(!nlsIsStrNull(t5PartNumberLCSDup))		//PARTNUMBER LCS ;
		{
			fprintf(fp,"%s",t5PartNumberLCSDup);
			fprintf(fp,"^");
		}
		else
		{
			fprintf(fp," ^");
		}

		if(!nlsIsStrNull(APLReveffDateFromDup))			//PARTNUMBER APL EFF FROM DATE;
		{
			fprintf(fp,"%s",APLReveffDateFromDup);
			fprintf(fp,"^");
		}
		else
		{
			fprintf(fp," ^");
		}

		if(!nlsIsStrNull(APLReveffDateToDup))			//PARTNUMBER APL EFF TO DATE;
		{
			fprintf(fp,"%s",APLReveffDateToDup);
			fprintf(fp,"^");
		}
		else
		{
			fprintf(fp," ^");
		}

		if(!nlsIsStrNull(stdReveffDateFromDup))			//PARTNUMBER STD EFF FROM DATE;
		{
			fprintf(fp,"%s",stdReveffDateFromDup);
			fprintf(fp,"^");
		}
		else
		{
			fprintf(fp," ^");
		}

		if(!nlsIsStrNull(stdReveffDateToDup))		//PARTNUMBER STD EFF TO DATE;
		{
			fprintf(fp,"%s",stdReveffDateToDup);
			fprintf(fp,"^");
		}
		else
		{
			fprintf(fp," ^");
		}


		fprintf(fp,"\n");
	
	CLEANUP:

			t5PrintCleanUpModName;

	EXIT:
			t5CheckDstatAndReturn;
}


int main(int argc, char *argv[])
{
	int stat=0;
	string  LoginS=NULL;
	string  PasswordS=NULL;
	string  OutFileNamePART=NULL;
	string  OutFileNameAPLPART=NULL;
	string  InPutFileNamePART=NULL;
	string  InpPartNumberS=NULL;
	string  InpPartNumberRev=NULL;
	string  InpPartNumberSeq=NULL;
	string PartNumberVal=NULL;
	string PartNumberValDup=NULL;
	string PartNumberRev=NULL;
	string PartNumberRevDup=NULL;
	string PartNumberSeq=NULL;
	string PartNumberSeqDup=NULL;
	string		docobjst	=	NULL;
	string PartOrgID=NULL;
	string		PartOrgIDDup=NULL;
	string		partCntVal=NULL;
	string		LineReadS=NULL;
	SetOfObjects  setOfAssmblyObjs = NULL ;
	ObjectPtr	TCPartObjP = NULL ;
	SqlPtr	PrtQrysql = NULL ;
	string PartProjName=NULL;
	string PlantName=NULL;
	string				PartProjNameDup=NULL;
	string partType=NULL;
	string				partTypeDup=NULL;
	string				sPlntNmeDup1=NULL;
	string				csIndicator=NULL;
	string				StoreLocation=NULL;
	string				configCtxt=NULL;

	FILE	*fp	= NULL;
	FILE	*fp_Error	= NULL;
	FILE	*fip	= NULL;

	int partCnt =0;
	int ii =0;
	int partNotFound =0;

	SetOfStrings      dbScp	=          NULL;
	char inputline[3000];

	
	t5MethodInitWMD("t5_StandardPartStamping");

	printf("\n Executing... t5_StandardPartStamping.c ... \n");fflush(stdout);

	LoginS=nlsStrAlloc(200);
	PasswordS=nlsStrAlloc(200);
	InpPartNumberS=nlsStrAlloc(200);
	InpPartNumberRev=nlsStrAlloc(200);
	InpPartNumberSeq=nlsStrAlloc(200);
	OutFileNamePART=nlsStrAlloc(200);
	OutFileNameAPLPART=nlsStrAlloc(200);
	InPutFileNamePART=nlsStrAlloc(200);
	PlantName=nlsStrAlloc(200);
	csIndicator=nlsStrAlloc(200);
	sPlntNmeDup1=nlsStrAlloc(200);
	StoreLocation=nlsStrAlloc(200);
	configCtxt=nlsStrAlloc(200);

	LoginS=argv[1];
	PasswordS=argv[2];
	//InpPartNumberS=argv[3];
	//InpPartNumberRev=argv[4];
	//InpPartNumberSeq=argv[5];
	InPutFileNamePART=argv[3];
	OutFileNamePART=argv[4];
	PlantName=argv[5];

	printf("\n InPutFileNamePART %s :OutFileNamePART :::%s:::\n",InPutFileNamePART,OutFileNamePART);fflush(stdout);
	//printf("\n !!!!!!!!!!!!Starting here!!!!!!!! \n");

	if (argc < 6 || argc > 6)
	{
		printf("\nSYNTEX ERROR :: \n");
		printf("\nSYNTEX SHOULD BE :: <EXE><USER-LOGIN><PASSWD><InputFileName><OutputFileName><PlantName>\n");
		printf("\nTRY AGAIN !!!\n");
		goto CLEANUP;
	}
	
	if((nlsStrCmp(PlantName,"PlantName1")==0) || (nlsStrCmp(PlantName,"PlantName4")==0))
	{
		nlsStrCpy(sPlntNmeDup1,"CARPLT");
		nlsStrCpy(csIndicator,"t5CarMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5CarStoreLocation");
		nlsStrCpy(configCtxt,"GlobalCtxt");		
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,csIndicator);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName13")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"PUVBU");
		nlsStrCpy(csIndicator,"t5PunUVMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5PunUVStoreLocation");
		nlsStrCpy(configCtxt,"PUVCtxt");	
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,csIndicator);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName2")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"PNR");
		nlsStrCpy(csIndicator,"t5PnrMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5PnrStoreLocation");
		nlsStrCpy(configCtxt,"PNRCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,csIndicator);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName3")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"PCVBU");
		nlsStrCpy(csIndicator,"t5PunMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5PunStoreLocation");
		nlsStrCpy(configCtxt,"GlobalCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,csIndicator);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName6")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"AHD");
		nlsStrCpy(csIndicator,"t5AhdMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5AhdStoreLocation");
		nlsStrCpy(configCtxt,"GlobalCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,csIndicator);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName8")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"JSR");
		nlsStrCpy(csIndicator,"t5JsrMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5JsrStoreLocation");
		nlsStrCpy(configCtxt,"JsrCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,csIndicator);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName9")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"LKO");
		nlsStrCpy(csIndicator,"t5LkoMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5LkoStoreLocation");
		nlsStrCpy(configCtxt,"LkoCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,csIndicator);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName11")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"DWD");
		nlsStrCpy(csIndicator,"t5DwdMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5DwdStoreLocation");
		nlsStrCpy(configCtxt,"DWDCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,csIndicator);fflush(stdout);
	}
	else
	{
		printf("\nSYNTEX ERROR :: \n");
		printf("\n PLEASE ENTER CORRECT PLANTNAME(EX:PlantName1,PlantName13,PlantName2 etc..)\n");
		printf("\nTRY AGAIN !!!\n");
		goto CLEANUP;
	}


	

	/* enable multibyte features of Metaphase */
	t5CheckDstat(clInitMB2 (argc, &argv, NULL));

	/* Check to see if the Metaphase network is available. If not, set dstat.*/
	t5CheckDstat(clTestNetwork ());

	/*  Initialize the command line session.    */
	t5CheckDstat(clInitialize2 (FALSE));

	t5CheckDstat(clLogin2 (LoginS,PasswordS,&stat));

	if (stat!=OKAY)
	{
		printf("\n Invalid User Name or PasswordS : %s,%s \n", LoginS, PasswordS);  fflush(stdout);
		goto EXIT;
	}


	t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScp,mfail));
	for (ii=0;ii<setSize(dbScp) ; ii++)
	{
			docobjst=low_set_get(dbScp,ii);
			printf("\n DB pref check before... :%s\n",docobjst);fflush(stdout);
	}

	low_set_add_str(dbScp, "supprod");
	low_set_add_str(dbScp, "suhprod");

	t5CheckDstat(SetDatabaseScope(PdmSessnClass,dbScp,mfail));
	for (ii=0;ii<low_set_size(dbScp) ; ii++)
	{
			docobjst=low_set_get(dbScp,ii);
			printf("\n DB pref check -after... :%s\n",docobjst);fflush(stdout);
	}

	
	fp=fopen(OutFileNamePART,"a+");
	if(fp==NULL)
	{
				printf("\n%s:fp file is not created...!!!!\n",OutFileNamePART);fflush(stdout);
	}
	fflush(fp);

	nlsStrCpy(OutFileNameAPLPART,"Error_PartNotFound.txt");
	fp_Error=fopen(OutFileNameAPLPART,"a+");
	if(fp_Error==NULL)
	{
				printf("\n%s:fp_Error file is not created...!!!!\n",OutFileNameAPLPART);fflush(stdout);
	}
	fflush(fp_Error);

	
	fip=fopen(InPutFileNamePART,"r");
	if(fip!=NULL)
	{
		//inputline=(char *) MEM_alloc(1000);
		while(fgets(inputline,1000,fip)!=NULL)
		{
			if(!nlsIsStrNull(LineReadS))nlsStrFree(LineReadS);
			LineReadS	= nlsStrAlloc(500);

			if(!nlsIsStrNull(inputline))
			{
				nlsStrCpy(LineReadS,inputline);
			}

			InpPartNumberS=strtok(LineReadS,"^");
			//InpPartNumberRev=strtok(NULL,"^");
			//InpPartNumberSeq=strtok(NULL,"^");

			setOfAssmblyObjs=NULL;
	
			if (dstat = oiSqlCreateSelect(&PrtQrysql)) goto EXIT;
			if(dstat=(oiSqlWhereBegParen(PrtQrysql)));
			if(dstat = oiSqlWhereEQ(PrtQrysql,PartNumberAttr,InpPartNumberS)) goto EXIT;
			if(dstat = oiSqlWhereAND(PrtQrysql)) goto EXIT;
			//if(dstat = oiSqlWhereEQ(PrtQrysql,RevisionAttr,InpPartNumberRev)) goto EXIT;
			//if(dstat = oiSqlWhereAND(PrtQrysql)) goto EXIT;
			if(dstat = oiSqlWhereEQ(PrtQrysql,OwnerNameAttr,"Release Vault")) goto EXIT;
			if(dstat = oiSqlAscOrder(PrtQrysql,CreationDateAttr)) goto EXIT;
			if(dstat=(oiSqlWhereEndParen(PrtQrysql))) goto EXIT;
			if(dstat = QueryDbObject(PartClass,PrtQrysql,0,TRUE,SC_SCOPE_OF_SESSION,&setOfAssmblyObjs,mfail)) goto EXIT;
			if(dstat=oiSqlPrint(PrtQrysql))goto EXIT;
			if(PrtQrysql) oiSqlDispose(PrtQrysql); PrtQrysql = NULL;
			printf("\n  setSize(setOfAssmblyObjs):%d \n",setSize(setOfAssmblyObjs)); fflush(stdout);
			
			if(setSize(setOfAssmblyObjs)>0)
			{
				for(partCnt=0;partCnt<setSize(setOfAssmblyObjs);partCnt++)
				{
					TCPartObjP=NULL;
					TCPartObjP=setGet(setOfAssmblyObjs,partCnt);

					PartNumberVal=NULL;
					PartNumberValDup=NULL;

					if(dstat = objGetAttribute(TCPartObjP,PartNumberAttr,&PartNumberVal))goto EXIT;
					if(!nlsIsStrNull(PartNumberVal)) PartNumberValDup=nlsStrDup(PartNumberVal);
					printf("\n  TC Part Number:%s \n",PartNumberValDup); fflush(stdout);

					PartNumberRev=NULL;
					PartNumberRevDup=NULL;

					if(dstat = objGetAttribute(TCPartObjP,RevisionAttr,&PartNumberRev))goto EXIT;
					if(!nlsIsStrNull(PartNumberRev))PartNumberRevDup=nlsStrDup(PartNumberRev);
					printf("\n  TC Part Revision:%s \n",PartNumberRevDup); fflush(stdout);

					
					PartNumberSeq=NULL;
					PartNumberSeqDup=NULL;

					if(dstat = objGetAttribute(TCPartObjP,SequenceAttr,&PartNumberSeq))goto EXIT;
					if(!nlsIsStrNull(PartNumberSeq))PartNumberSeqDup=nlsStrDup(PartNumberSeq);
					printf("\n TC Part Sequence:%s \n",PartNumberSeqDup); fflush(stdout);

					PartOrgID=NULL;
					PartOrgIDDup=NULL;

					if(dstat = objGetAttribute(TCPartObjP,OrganizationIDAttr,&PartOrgID))goto EXIT;
					if(!nlsIsStrNull(PartOrgID))PartOrgIDDup=nlsStrDup(PartOrgID);
					printf("\n TC Part OrgID:%s \n",PartOrgIDDup); fflush(stdout);

					PartProjName=NULL;
					PartProjNameDup=NULL;

					if(dstat = objGetAttribute(TCPartObjP,ProjectNameAttr,&PartProjName))goto EXIT;
					if(!nlsIsStrNull(PartProjName))PartProjNameDup=nlsStrDup(PartProjName);
					printf("\n TC Part PartProjNameDup:%s \n",PartProjNameDup); fflush(stdout);

					partType=NULL;
					partTypeDup=NULL;
					
					if((nlsStrCmp(PartProjNameDup,"1111")==0)) 
					{
						t5CheckMfail(GetTCPartInfoForAPL(TCPartObjP,csIndicator,StoreLocation,configCtxt,fp,mfail));
						printf("\n == Standard Part PartProjName:%s:%s:%s:%s:%d \n",PartNumberValDup,PartNumberRevDup,PartNumberSeqDup,PartProjName,nlsStrLen(PartNumberValDup)); fflush(stdout);
					}
					else
					{
						if(dstat = objGetAttribute(TCPartObjP,PartTypeAttr,&partType))goto EXIT;
						if(!nlsIsStrNull(partType))partTypeDup=nlsStrDup(partType);
						printf("\n TC Part partTypeDup:%s \n",partTypeDup); fflush(stdout);

						if((nlsStrCmp(partTypeDup,"T")==0) || (nlsStrCmp(partTypeDup,"V")==0) || (nlsStrCmp(partTypeDup,"VC")==0) || (nlsStrCmp(partTypeDup,"CKDVC")==0))
						{
							printf("\n Not Standard Part PartProjName:%s:%s:%s:%s:%d \n",PartNumberValDup,PartNumberRevDup,PartNumberSeqDup,PartProjName,nlsStrLen(PartNumberValDup)); fflush(stdout);
						}
						else
						{
							if(nlsStrLen(PartNumberValDup)==11)
							{
								printf("\n == Standard Part PartProjName:%s:%s:%s:%s:%d \n",PartNumberValDup,PartNumberRevDup,PartNumberSeqDup,PartProjName,nlsStrLen(PartNumberValDup)); fflush(stdout);
								t5CheckMfail(GetTCPartInfoForAPL(TCPartObjP,csIndicator,StoreLocation,configCtxt,fp,mfail));
							}
						}
					}


				}
			
			}
			else
			{
				partNotFound++;
				
				if(!nlsIsStrNull(partCntVal)) nlsStrFree(partCntVal);
				partCntVal = nlsStrAlloc(30);

				sprintf(partCntVal,"%d",partNotFound);

				fprintf(fp_Error,partCntVal);//(level);
				fprintf(fp_Error,"^");
				fprintf(fp_Error,"%s",InpPartNumberS);			//PARTNUMBER;								
				fprintf(fp_Error,"^");
				//fprintf(fp_Error,"%s",InpPartNumberRev);		//PARTNUMBER REV;
				//fprintf(fp_Error,"^");
				//fprintf(fp_Error,"%s",InpPartNumberSeq);		//PARTNUMBER SEQ;
				//fprintf(fp_Error,"^");
				fprintf(fp_Error,"\n");
			
			}
		}
	}
				
	if(fp) fclose(fp); fp=NULL;
	if(fp_Error) fclose(fp_Error); fp_Error=NULL;
	if(fip) fclose(fip); fip=NULL;


CLEANUP:
		t5PrintCleanUpModName;


EXIT:
		t5CheckDstatAndReturn;
}
;

