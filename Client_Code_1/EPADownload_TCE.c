/**********************************************************************************************
* Copyright (c) 2004 Tata Technologies Limited (TTL). All Rights
* Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
* Program Name	    : EPA Downloader.c
* Author		                : Dayanand Amdapure
* Created on	            : wednesday, Nov 14, 2018
* Project                      : TATA MOTORS - ECN Pending Report
* Methods Defined : EPADownload_TCE
*
*PURPOSE:
*
*     1. Given input  of plant and project code
*     2. Download all the EPA, EPA Task and relations details into file.
*     3. All the meta data details will write in flat file.
*
*  MAIN LOGIC OF THE PROGRAM:
*     1. This Program is specially used for EPA donwloader of EPA migration from TCE to TCUA.
*
* Modification History :
* S.No    Date        CR No      Modified By            Modification Notes
*
*
*********************************************************************************************/
//static char *_rcsvid ="$Id: EPADownload_TCE.c,v 1.1 2019/01/24 05:25:27 dmm06484 Exp root $";

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
#include <tel.h>
#include <oi.h>
#include <ui.h>

char* subString
(
  char* mainStringf ,
  int fromCharf     ,
  int toCharf
)
{
      int i;
      char *retStringf;
      retStringf = (char*) malloc(toCharf+1);
      for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
      *(retStringf+i) = '\0';
      return retStringf;
}

// Start main function
int main(int argc, char *argv[])
{
	FILE		*fprel	;
	FILE		*fpvalid	;
	FILE		*fpInvalid	;
	FILE		*fpepa	;

    MODNAME ("main:EPADownload_TCE");
    int  stat  =0;
	status dstat =OKAY;
	string		  Username				= NULL;
	string		  Password				= NULL;
	string 	  date					= NULL;
	string 	  LCS						= NULL;
	string 	  ClosureTime				= NULL;
	string 	  ClosureTimeDup			= NULL;
	string 	  APLPNE					= NULL;
	string 	  APLSGR					= NULL;
    string 	  ProjectNameCon			= NULL;
	string 	  ECRNumDup				= NULL;
	string		  DMLDescription		= NULL;
	string		  DMLDescriptionDup		= NULL;
	string		  ReasonDesc		= NULL;
	string		  ReasonDescDup		= NULL;
	string		  ReasonCode		= NULL;
	string		  ReasonCodeDup		= NULL;
	string		  ReasonValDup		= NULL;
	string		  WbsAnalystStd		= NULL;
    string      docobjst        =   NULL;
    string      EPAName        =   NULL;
    string      EPASubject        =   NULL;
    string      t5ErcDMLNoDup        =   NULL;
	string		 t5ErcDMLNo=NULL;
	string		 t5EpaTaskNo=NULL;
	string		 t5EpaTaskNoDup=NULL;
	string		 EPANameDup=NULL;
	string		 EpaPlantName=NULL;
	string   AQEngineer=NULL;
	string   AQEngineerDup=NULL;
	string   AQGroupLeader=NULL;
	string   AQGroupLeaderDup=NULL;
	string   ActionByAgency=NULL;
	string   ActionByAgencyDup=NULL;
	string   ActionTobeTaken=NULL;
	string   ActionTobeTakenDup=NULL;
	string   ClosureComments=NULL;
	string   ClosureCommentsDup=NULL;
	string   Customer=NULL;
	string   CustomerDup=NULL;
	string   DateOfIntroInVC2=NULL;
	string   DateOfIntroInVC2Dup=NULL;
	string   DateOfIntroInVC3=NULL;
	string   DateOfIntroInVC3Dup=NULL;
	string   DateOfIntroInVC4=NULL;
	string   DateOfIntroInVC4Dup=NULL;
	string   DateOfIntroInVCE2=NULL;
	string   DateOfIntroInVCE2Dup=NULL;
	string   DateOfIntroInVCE3=NULL;
	string   DateOfIntroInVCE3Dup=NULL;
	string   EmailAdd=NULL;
	string   EmailAddDup=NULL;
	string   EpaMeetinDate=NULL;
	string  EpaMeetinDateDup=NULL;
	string  EpaPlantA=NULL;
	string  EpaPlantADup=NULL;
	string  InterchangeCodes=NULL;
	string  InterchangeCodesDup=NULL;
	string  MEPA_NO=NULL;
	string  MEPA_NODup=NULL;
	string  MbpaCreDate=NULL;
	string  MbpaCreDateDup=NULL;
	string  MbpaCretor=NULL;
	string  MbpaCretorDup=NULL;
	string  MepaCreDate=NULL;
	string  MepaCreDateDup=NULL;
	string  MepaCretor=NULL;
	string  MepaCretorDup=NULL;
	string  PlanningStatus=NULL;
	string  PlanningStatusDup=NULL;
	string  WbsPriority=NULL;
	string  WbsPriorityDup=NULL;
	string  t5AggreCutOffNo=NULL;
	string  t5AggreCutOffNoDup=NULL;
	string  t5AggreCutOffNo1=NULL;
	string  t5AggreCutOffNo1Dup=NULL;
	string  t5AppModelList=NULL;
	string  t5AppModelListDup=NULL;
	string  t5BomComu=NULL;
	string  t5BomComuDup=NULL;
	string  t5BreakPointDate=NULL;
	string  t5BreakPointDateDup=NULL;
	string  t5BtchCode=NULL;
	string  t5BtchCodeDup=NULL;
	string  t5CTChange=NULL;
	string  t5CTChangeDup=NULL;
	string  t5Category_code1=NULL;
	string  t5Category_code1Dup=NULL;
	string  t5Category_code2=NULL;
	string  t5Category_code2Dup=NULL;
	string  t5Category_code3=NULL;
	string  t5Category_code3Dup=NULL;
	string  t5Category_code4=NULL;
	string  t5Category_code4Dup=NULL;
	string  t5Category_code5=NULL;
	string  t5Category_code5Dup=NULL;
	string  t5Category_code6=NULL;
	string  t5Category_code6Dup=NULL;
	string  t5ChasisNo=NULL;
	string  t5ChasisNoDup=NULL;
	string  t5ChasisNo2=NULL;
	string  t5ChasisNo2Dup=NULL;
	string  t5ChasisNo3=NULL;
	string  t5ChasisNo3Dup=NULL;
	string  t5ChasisNo4=NULL;
	string  t5ChasisNo4Dup=NULL;
	string  t5ChasisNoE=NULL;
	string  t5ChasisNoEDup=NULL;
	string  t5ChasisNoE2=NULL;
	string  t5ChasisNoE2Dup=NULL;
	string  t5ChasisNoE3=NULL;
	string  t5ChasisNoE3Dup=NULL;
	string  t5ChasisNoE4=NULL;
	string  t5ChasisNoE4Dup=NULL;
	string  t5ChasisTypeList=NULL;
	string  t5ChasisTypeListDup=NULL;
	string  t5CommnMepaStr=NULL;
	string  t5CommnMepaStrDup=NULL;
	string  t5CostReduction=NULL;
	string  t5CostReductionDup=NULL;
	string  t5DesignGroup=NULL;
	string  t5DesignGroupDup=NULL;
	string  t5DisposalAction1=NULL;
	string  t5DisposalAction1Dup=NULL;
	string  t5DisposalAction2=NULL;
	string  t5DisposalAction2Dup=NULL;
	string  t5DisposalAction3=NULL;
	string  t5DisposalAction3Dup=NULL;
	string  t5DisposalAction4=NULL;
	string  t5DisposalAction4Dup=NULL;
	string  t5DisposalAction5=NULL;
	string  t5DisposalAction5Dup=NULL;
	string  t5DumCompln=NULL;
	string  t5DumComplnDup=NULL;
	string  t5DumEpNo=NULL;
	string  t5DumEpNoDup=NULL;
	string  t5ECNTstRepNo=NULL;
	string  t5ECNTstRepNoDup=NULL;
	string  t5EPACategory_code1=NULL;
	string  t5EPACategory_code1Dup=NULL;
	string  t5EPAClosureDate=NULL;
	string  t5EPAClosureDateDup=NULL;
	string   t5EpaClass=NULL;
	string   t5EpaClassDup=NULL;
	string   t5EpaClassDate=NULL;
	string   t5EpaClassDateDup=NULL;
	string   t5EpaFolUpInd=NULL;
	string   t5EpaFolUpIndDup=NULL;
	string   t5EpaSet=NULL;
	string   t5EpaSetDup=NULL;
	string   t5EpaStatus=NULL;
	string   t5EpaStatusDup=NULL;
	string   t5EpaTskCrDate=NULL;
	string   t5EpaTskCrDateDup=NULL;
	string   t5EpaType=NULL;
	string   t5EpaTypeDup=NULL;
	string   t5EpaValidity=NULL;
	string   t5EpaValidityDup=NULL;
	string   t5FialReq=NULL;
	string   t5FialReqDup=NULL;
	string   t5Introduction_type=NULL;
	string   t5Introduction_typeDup=NULL;
	string   t5IsActionReqbySer=NULL;
	string   t5IsActionReqbySerDup=NULL;
	string   t5IsDumEp=NULL;
	string   t5IsDumEpDup=NULL;
	string   t5IsStructEPA=NULL;
	string   t5IsStructEPADup=NULL;
	string   t5IsSubAggReq=NULL;
	string   t5IsSubAggReqDup=NULL;
	string   t5IsToolReq=NULL;
	string   t5IsToolReqDup=NULL;
	string   t5IsTryOutReq=NULL;
	string   t5IsTryOutReqDup=NULL;
	string   t5LogisticsRemarks=NULL;
	string   t5LstPODate=NULL;
	string   t5LstSmplDate=NULL;
	string   t5MBPARemarks=NULL;
	string   t5MEPAQADate=NULL;
	string   t5MailAlert=NULL;
	string   t5MbpaDesc=NULL;
	string   t5MbpaNo=NULL;
	string   t5MepaReason=NULL;
	string   t5ModOfIntrod=NULL;
	string   t5ModelList=NULL;
	string   t5POCheckStatus=NULL;
	string   t5POLockDate=NULL;
	string   t5PPMRemarks=NULL;
	string   t5PartYesNo=NULL;
	string   t5PartialReason=NULL;
	string   t5QACRemarks=NULL;
	string   t5ReadinessDate=NULL;
	string   t5ReleaseNotes=NULL;
	string   t5RemarkAppModel=NULL;
	string   t5RemarksBP=NULL;
	string   t5RemarksEPAClosure=NULL;
	string   t5RemarksMEPAQA=NULL;
	string   t5RemarksPOLock=NULL;
	string   t5RemarksReadiness=NULL;
	string   t5RemarksStocks=NULL;
	string   t5RemarksTryout=NULL;
	string   t5RevRemarks=NULL;
	string   t5RjVECNo=NULL;
	string   t5RygSts=NULL;
	string   t5SORSendDate=NULL;
	string   t5ServAction=NULL;
	string   t5ServActionAgg=NULL;
	string   t5StocksSCMDate=NULL;
	string   t5TSRemarks=NULL;
	string   t5TargetDtIntro=NULL;
	string   t5TargetDtIntroHis=NULL;
	string   t5TmpEcnNo=NULL;
	string   t5LogisticsRemarksDup=NULL;
	string   t5LstPODateDup=NULL;
	string   t5LstSmplDateDup=NULL;
	string   t5MBPARemarksDup=NULL;
	string   t5MEPAQADateDup=NULL;
	string   t5MailAlertDup=NULL;
	string   t5MbpaDescDup=NULL;
	string   t5MbpaNoDup=NULL;
	string   t5MepaReasonDup=NULL;
	string   t5ModOfIntrodDup=NULL;
	string   t5ModelListDup=NULL;
	string   t5POCheckStatusDup=NULL;
	string   t5POLockDateDup=NULL;
	string   t5PPMRemarksDup=NULL;
	string   t5PartYesNoDup=NULL;
	string   t5PartialReasonDup=NULL;
	string   t5QACRemarksDup=NULL;
	string   t5ReadinessDateDup=NULL;
	string   t5ReleaseNotesDup=NULL;
	string   t5RemarkAppModelDup=NULL;
	string   t5RemarksBPDup=NULL;
	string   t5RemarksEPAClosureDup=NULL;
	string   t5RemarksMEPAQADup=NULL;
	string   t5RemarksPOLockDup=NULL;
	string   t5RemarksReadinessDup=NULL;
	string   t5RemarksStocksDup=NULL;
	string   t5RemarksTryoutDup=NULL;
	string   t5RevRemarksDup=NULL;
	string   t5RjVECNoDup=NULL;
	string   t5RygStsDup=NULL;
	string   t5SORSendDateDup=NULL;
	string   t5ServActionDup=NULL;
	string   t5ServActionAggDup=NULL;
	string   t5StocksSCMDateDup=NULL;
	string   t5TSRemarksDup=NULL;
	string   t5TargetDtIntroDup=NULL;
	string   t5TargetDtIntroHisDup=NULL;
	string   t5TmpEcnNoDup=NULL;
	string   t5TrgtDtIntRem=NULL;
	string   t5TryoutDate=NULL;
	string   t5VDRemarks=NULL;
	string   t5VQARemarks=NULL;
	string   t5tcfAggregate=NULL;
	string   t5tcfAggregate1=NULL;
	string   tentativeDateLogistic=NULL;
	string   tentativeDateSCM=NULL;
	string   tx0Addressee=NULL;
	string   tx0DateReceived=NULL;
	string   tx0DocCreateDate=NULL;
	string   t5TrgtDtIntRemDup=NULL;
	string   t5TryoutDateDup=NULL;
	string   t5VDRemarksDup=NULL;
	string   t5VQARemarksDup=NULL;
	string   t5tcfAggregateDup=NULL;
	string   t5tcfAggregate1Dup=NULL;
	string   tentativeDateLogisticDup=NULL;
	string   tentativeDateSCMDup=NULL;
	string   tx0AddresseeDup=NULL;
	string   tx0DateReceivedDup=NULL;
	string   tx0DocCreateDateDup=NULL;
	string   ChkPartNumber=NULL;
	string   ChkPartNumberDup=NULL;
	string   ChkPartRevision=NULL;
	string   ChkPartRevisionDup=NULL;
	string   ChkPartSequence=NULL;
	string   ChkPartSequenceDup=NULL;
	string   PartnumberDup=NULL;
	string   Revision=NULL;
	string   Sequence=NULL;
	string  changeType= NULL;
	string  InterchangeCodesV= NULL;
	string  IsPartDev= NULL;
	string  IsToolReq= NULL;
	string  MBPAStoreloc= NULL;
	string  NewQtyL= NULL;
	string  PartType= NULL;
	string  PoGohead= NULL;
	string  PrtSupplier= NULL;
	string  ReplacePartNumber= NULL;
	string  ReqForMepa= NULL;
	string  TryOutNum= NULL;
	string  SampleIRMR= NULL;
	string  VINNum= NULL;
	string  PartnumberDupIn = NULL;
	string  RevisionIn = NULL;
	string  SequenceIn = NULL;
	string  Assembly = NULL;
	string  changeTypeIn = NULL;
	string  Finished = NULL;
	string  InHouseStock = NULL;
	string  InHousetTotal = NULL;
	string  InShop = NULL;
	string  IsPartDevIn = NULL;
	string  MBPAStorelocIn = NULL;
	string  OldQtyL = NULL;
	string  Peculiarity = NULL;
	string  PecuValid = NULL;
	string  PoGoheadIn = NULL;
	string  PrtSupplierIn = NULL;
	string  RawParts = NULL;
	string  ReqForMepaIn = NULL;
	string  SampleIRMRIn = NULL;
	string  SPD = NULL;
	string  SupplierEnd = NULL;
	string  Transit = NULL;
	string  WareHouse = NULL;

    string   EpaValidParts=NULL;
    string   EpaInValidParts=NULL;
    string   EpaWholeStr=NULL;
    string   EpaFilePath=NULL;
    string   EpaRelFilePath=NULL;
    string   EpaValidPartsFilePath=NULL;
    string   EpaInValidPartsFilePath=NULL;

	SetPtr              SigState                = NULL;
	SqlPtr				Qry_sql					= NULL;
    SetOfStrings dbScpe   =   NULL;
	SetOfObjects		ErcDmlObjs				= NULL;
	SetOfObjects		ERCDMLObjs				= NULL;
    SetOfObjects	t5EpaObjs	= NULL;
    SetOfObjects	EpaTaskObjs	= NULL;
    SetOfObjects	EpaTaskRelObjs	= NULL;
    SetOfObjects	EpaPartObjs	= NULL;
    SetOfObjects	EpaPartRelObjs= NULL;
    SetOfObjects	EpaModelAppl= NULL;
    SetOfObjects	EpaChassisList= NULL;
    SetOfObjects	EpaModelList= NULL;

	ObjectPtr			EpaObj			    = NULL;
	ObjectPtr			EpaTaskObj			    = NULL;
	ObjectPtr			ChkPartObj			    = NULL;

	int					mfail				    = OKAY;
	int					ii				    = 0;
	int					m		    = 0;
	int					counter					= 0;
	int					SRNo					= 0;
	 int                 ValTabSize    = 0;
	 int                 InvalidTabSize    = 0;
	 int                 q    = 0;
	int                  i = 0;

    Username=nlsStrAlloc(20);
    Password=nlsStrAlloc(20);

    date =nlsStrAlloc(30);
    APLPNE =nlsStrAlloc(100);
    ECRNumDup =nlsStrAlloc(1000);
    APLSGR =nlsStrAlloc(1000);
    WbsAnalystStd =nlsStrAlloc(1000);
    ClosureTime	= nlsStrAlloc(40);
    ClosureTimeDup	= nlsStrAlloc(1000);
	EpaInValidPartsFilePath=nlsStrAlloc(500);
	EpaValidPartsFilePath=nlsStrAlloc(500);
	EpaRelFilePath=nlsStrAlloc(500);
	EpaFilePath=nlsStrAlloc(500);
	DMLDescription=nlsStrAlloc(1000);
	DMLDescriptionDup=nlsStrAlloc(1500);
	ReasonDesc=nlsStrAlloc(500);
	ReasonDescDup=nlsStrAlloc(1000);
	ReasonCode=nlsStrAlloc(500);
	ReasonCodeDup=nlsStrAlloc(1000);
	ReasonValDup=nlsStrAlloc(500);
    EpaWholeStr=nlsStrAlloc(5000);
    nlsStrCpy(EpaWholeStr, "");

    EpaValidParts=nlsStrAlloc(5000);
    nlsStrCpy(EpaValidParts, "");
    EpaInValidParts=nlsStrAlloc(5000);
    nlsStrCpy(EpaInValidParts, "");

    LCS=nlsStrAlloc(20);
	strcpy(LCS,"LcsErcRlzd");

	EpaPlantName=argv[1];
	ProjectNameCon=argv[2];
	//Password=argv[2];
	strcpy(Username,"super user");
	strcpy(Password,"abc123");

	strcpy(date,"2012/01/01");



        strcpy(EpaValidPartsFilePath,"");
    	nlsStrCat(EpaValidPartsFilePath,"EPAValidParts.txt");
		fpvalid=fopen(EpaValidPartsFilePath,"w+");
		if(fpvalid==NULL)
		{
		 printf("\n%s:file is not created...!!!!\n",EpaValidPartsFilePath);fflush(stdout);
		}
		fflush(fpvalid);
		//fprintf(fpvalid,"\n");

        strcpy(EpaInValidPartsFilePath,"");
    	nlsStrCat(EpaInValidPartsFilePath,"EPAInValidParts.txt");
		fpInvalid=fopen(EpaInValidPartsFilePath,"w+");
		if(fpInvalid==NULL)
		{
		 printf("\n%s:file is not created...!!!!\n",EpaInValidPartsFilePath);fflush(stdout);
		}
		fflush(fpInvalid);
		//fprintf(fpInvalid,"\n");

        strcpy(EpaRelFilePath,"");
    	nlsStrCat(EpaRelFilePath,"EPARelMetaDataDetails.txt");
		fprel=fopen(EpaRelFilePath,"w+");
		if(fprel==NULL)
		{
		 printf("\n%s:file is not created...!!!!\n",EpaRelFilePath);fflush(stdout);
		}
		fflush(fprel);
		//fprintf(fprel,"\n");

		strcpy(EpaFilePath,"");
		nlsStrCat(EpaFilePath,"EPAMetaDataDetails.txt");

		fpepa=fopen(EpaFilePath,"w+");
		if(fpepa==NULL)
		{
		 printf("\n%s:file is not created...!!!!\n",EpaFilePath);fflush(stdout);
		}
		fflush(fpepa);
		//fprintf(fpepa,"\n");

			/* enable multibyte features of Metaphase */
		if (dstat = clInitMB2 (argc, &argv, NULL)) goto CLEANUP;
		if (dstat = clTestNetwork ()) goto CLEANUP;
		if (dstat = clInitialize2 (TRUE)) goto CLEANUP;
		if (dstat = clLogin2(Username,Password,&stat)) goto CLEANUP;
		if (stat != OKAY)
		{
			printf("\nInvalid User Name or Password !!!!!!!!!!!!\n");
		}
		SigState = setCreate(5);
		low_set_add(SigState,"LcmIntRdy");
		low_set_add(SigState,"LcmIntReq");

		/* Aniket code start********************************************/

		t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScpe,&mfail));
		for (ii=0;ii<setSize(dbScpe) ; ii++)
		{
		docobjst=low_set_get(dbScpe,ii);
		printf("\n DB pref check before... :%s\n",docobjst);fflush(stdout);
		}

		low_set_add_str(dbScpe, "supprod");
		low_set_add_str(dbScpe, "suhprod");
		low_set_add_str(dbScpe, "sujprod");
	//	low_set_add_str_unique(dbScpe, "sujprod");
		low_set_add_str_unique(dbScpe, "sulprod");

		t5CheckDstat(smSetSessionDbScope(dbScpe));

		//t5CheckDstat(SetDatabaseScope(PdmSessnClass,dbScpe,&mfail));


		t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScpe,&mfail));
		for (ii=0;ii<low_set_size(dbScpe) ; ii++)
		{
		docobjst=low_set_get(dbScpe,ii);
		printf("\n DB pref check -after... :%s\n",docobjst);fflush(stdout);
		}
							printf("\n EpaPlantName.. :%s\n",EpaPlantName);fflush(stdout);
							printf("\n ProjectNameCon... :%s\n",ProjectNameCon);fflush(stdout);

							if(dstat=(oiSqlCreateSelect(&Qry_sql))) goto EXIT;
							if(dstat=(oiSqlWhereBegParen(Qry_sql)))goto EXIT;
							if(dstat=(oiSqlWhereEQ(Qry_sql,EpaPlantAAttr, EpaPlantName))) goto EXIT;
  				            if(dstat=(oiSqlWhereAND(Qry_sql))) goto EXIT;
							if(dstat=(oiSqlWhereEQ(Qry_sql,ProjectNameAttr, ProjectNameCon))) goto EXIT;
							if(dstat=(oiSqlWhereEndParen(Qry_sql))) goto EXIT;
							if(dstat=(QueryDbObject(t5StdDmlClass,Qry_sql,0,TRUE,SC_SCOPE_OF_SESSION,&t5EpaObjs,&mfail)))  goto EXIT;
							oiSqlPrint(Qry_sql);
							if(Qry_sql) oiSqlDispose(Qry_sql); Qry_sql = NULL;
							printf("\nSetsize of t5QryObjs : %d \n",setSize(t5EpaObjs));fflush(stdout);
							if(setSize(t5EpaObjs)>0)
							{
							 ErcDmlObjs=NULL;
							 EPAName= NULL;
							 EPASubject= NULL;
							 for(counter=0;counter<setSize(t5EpaObjs);counter++)
							 {
								SRNo = SRNo +1;
								EpaObj = setGet(t5EpaObjs, counter);
								if(dstat = objGetAttribute(EpaObj,WbsIDAttr,&EPAName)) goto EXIT;
								if(!nlsIsStrNull(EPAName)) EPANameDup=nlsStrDup(EPAName);

								 // Start for EPA Relationship Details
								if(EpaObj)
								{
									if (dstat =ExpandObject4(CmPlRvRvClass, EpaObj, "CMPlanRollsUpInTo", &ErcDmlObjs,&mfail)) goto EXIT;
						      		printf("\n\n No.of EPA to ERC DMLs:%d",setSize(ErcDmlObjs));fflush(stdout);
									if(setSize(ErcDmlObjs)>0)
									 {
											fprintf(fprel,"EpaDml^%s", EPANameDup);fflush(stdout);
										   for(m = 0; m < setSize(ErcDmlObjs); m++)
										  {
											if(dstat=objGetAttribute(low_set_get(ErcDmlObjs, m), WbsIDAttr, &t5ErcDMLNo)) goto CLEANUP;
											if(!nlsIsStrNull(t5ErcDMLNo)) t5ErcDMLNoDup=nlsStrDup(t5ErcDMLNo);
											fprintf(fprel,"^%s", t5ErcDMLNoDup);fflush(stdout);
										  }
									    fprintf(fprel,"\n");fflush(stdout);
									 }
									if (dstat = ExpandObject("CmPlRvRv",EpaObj,"CMPlanBreaksDownInTo",&EpaTaskObjs,&EpaTaskRelObjs,&mfail))goto EXIT;
						      		printf("\n\n No.of EPA Tasks:%d",setSize(EpaTaskObjs));fflush(stdout);
									if(setSize(EpaTaskObjs)>0)
									 {

										   for(m = 0; m < setSize(EpaTaskObjs); m++)
										  {

											EpaTaskObj = low_set_get(EpaTaskObjs, m);
											if(dstat=objGetAttribute(EpaTaskObj, WbsIDAttr, &t5EpaTaskNo)) goto CLEANUP;
											if(!nlsIsStrNull(t5EpaTaskNo)) t5EpaTaskNoDup=nlsStrDup(t5EpaTaskNo);

											if (dstat = ExpandObject("t5MepaRe",EpaTaskObj,"CmMepaContainsAssembly",&EpaPartObjs,&EpaPartRelObjs,&mfail))goto EXIT;
											if (setSize(EpaPartObjs)>0)
											{
												for (ii=0;ii<setSize(EpaPartObjs);ii++)
												{
													 fprintf(fprel,"EpaTask^%s", EPANameDup);fflush(stdout);
													 fprintf(fprel,"^%s", t5EpaTaskNoDup);fflush(stdout);
													ChkPartNumber = NULL;
													ChkPartNumberDup = NULL;
													ChkPartObj = NULL;
													ChkPartObj = low_set_get(EpaPartObjs,ii);
													if (dstat = objGetAttribute(ChkPartObj, PartNumberAttr,&ChkPartNumber)) goto EXIT;
													if(!nlsIsStrNull(ChkPartNumber)) ChkPartNumberDup = nlsStrDup(ChkPartNumber);
													if (dstat = objGetAttribute(ChkPartObj, RevisionAttr,&ChkPartRevision)) goto EXIT;
													if(!nlsIsStrNull(ChkPartRevision)) ChkPartRevisionDup = nlsStrDup(ChkPartRevision);
													if (dstat = objGetAttribute(ChkPartObj, SequenceAttr,&ChkPartSequence)) goto EXIT;
													if(!nlsIsStrNull(ChkPartSequence)) ChkPartSequenceDup = nlsStrDup(ChkPartSequence);
													fprintf(fprel,"^%s,%s,%s", ChkPartNumberDup,ChkPartRevisionDup,ChkPartSequenceDup);fflush(stdout);
												  fprintf(fprel,"\n");fflush(stdout);
												}
											}
										  }
									 }

									// Start For EPA Valid Parts
									if(dstat =objGetTableSize(EpaObj,t5EpaCmTabAttr,&ValTabSize)) goto EXIT;
									printf("\n\n No.of EPA ValTabSize:%d",ValTabSize);fflush(stdout);
									if(ValTabSize>0)
									{
										for (q=0;q<ValTabSize;q++)
										{
											EpaValidParts = NULL;
											EpaValidParts=nlsStrAlloc(5000);
											nlsStrCpy(EpaValidParts, "");
											fprintf(fpvalid,"EpaValid^%s", EPANameDup);fflush(stdout);
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,PartNumberAttr,&PartnumberDup)) goto EXIT;
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,RevisionAttr,&Revision)) goto EXIT;
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,SequenceAttr,&Sequence)) goto EXIT;
											if (PartnumberDup)
											{
												   PartnumberDup=low_strssra(PartnumberDup,"\n",";");
											}
											if (PartnumberDup)
											{
												   PartnumberDup=low_strssra(PartnumberDup,"^",";");
											}

											if(!nlsIsStrNull(PartnumberDup))
											 {
											nlsStrCat(EpaValidParts, PartnumberDup);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
											nlsStrCat(EpaValidParts, "^");
											 }

											if (Revision)
											{
												nlsStrCat(EpaValidParts, Revision);
												nlsStrCat(EpaValidParts, "^");
											}
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
											nlsStrCat(EpaValidParts, "^");
											 }


											if (Sequence)
											{
												nlsStrCat(EpaValidParts, Sequence);
												nlsStrCat(EpaValidParts, "^");
											}
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
											nlsStrCat(EpaValidParts, "^");
											 }


											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5changeTypeAttr,&changeType)) goto EXIT;
											if (changeType)
											{
												   changeType=low_strssra(changeType,"\n",";");
											}
											if (changeType)
											{
												   changeType=low_strssra(changeType,"^",";");
											}

											if(!nlsIsStrNull(changeType))
											 {
											nlsStrCat(EpaValidParts, changeType);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }

											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,InterchangeCodesAttr,&InterchangeCodesV)) goto EXIT;
											if (InterchangeCodesV)
											{
												   InterchangeCodesV=low_strssra(InterchangeCodesV,"\n",";");
											}
											if (InterchangeCodesV)
											{
												   InterchangeCodesV=low_strssra(InterchangeCodesV,"^",";");
											}

											if(!nlsIsStrNull(InterchangeCodesV))
											 {
											nlsStrCat(EpaValidParts, InterchangeCodesV);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5IsPartDevAttr,&IsPartDev)) goto EXIT;
											if (IsPartDev)
											{
												   IsPartDev=low_strssra(IsPartDev,"\n",";");
											}
											if (IsPartDev)
											{
												   IsPartDev=low_strssra(IsPartDev,"^",";");
											}

											if(!nlsIsStrNull(IsPartDev))
											 {
											nlsStrCat(EpaValidParts, IsPartDev);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5IsToolReqAttr,&IsToolReq)) goto EXIT;
											if (IsToolReq)
											{
												   IsToolReq=low_strssra(IsToolReq,"\n",";");
											}
											if (IsToolReq)
											{
												   IsToolReq=low_strssra(IsToolReq,"^",";");
											}

											if(!nlsIsStrNull(IsToolReq))
											 {
											nlsStrCat(EpaValidParts, IsToolReq);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5MBPAStorelocAttr,&MBPAStoreloc)) goto EXIT;
											if (MBPAStoreloc)
											{
												   MBPAStoreloc=low_strssra(MBPAStoreloc,"\n",";");
											}
											if (MBPAStoreloc)
											{
												   MBPAStoreloc=low_strssra(MBPAStoreloc,"^",";");
											}

											if(!nlsIsStrNull(MBPAStoreloc))
											 {
											nlsStrCat(EpaValidParts, MBPAStoreloc);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5NewQtyLAttr,&NewQtyL)) goto EXIT;
											if (NewQtyL)
											{
												   NewQtyL=low_strssra(NewQtyL,"\n",";");
											}
											if (NewQtyL)
											{
												   NewQtyL=low_strssra(NewQtyL,"^",";");
											}

											if(!nlsIsStrNull(NewQtyL))
											 {
											nlsStrCat(EpaValidParts, NewQtyL);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5PartTypeAttr,&PartType)) goto EXIT;
											if (PartType)
											{
												   PartType=low_strssra(PartType,"\n",";");
											}
											if (PartType)
											{
												   PartType=low_strssra(PartType,"^",";");
											}

											if(!nlsIsStrNull(PartType))
											 {
											nlsStrCat(EpaValidParts, PartType);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5PoGoheadAttr,&PoGohead)) goto EXIT;
											if (PoGohead)
											{
												   PoGohead=low_strssra(PoGohead,"\n",";");
											}
											if (PoGohead)
											{
												   PoGohead=low_strssra(PoGohead,"^",";");
											}

											if(!nlsIsStrNull(PoGohead))
											 {
											nlsStrCat(EpaValidParts, PoGohead);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5PrtSupplierAttr,&PrtSupplier)) goto EXIT;
											if (PrtSupplier)
											{
												   PrtSupplier=low_strssra(PrtSupplier,"\n",";");
											}
											if (PrtSupplier)
											{
												   PrtSupplier=low_strssra(PrtSupplier,"^",";");
											}

											if(!nlsIsStrNull(PrtSupplier))
											 {
											nlsStrCat(EpaValidParts, PrtSupplier);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5ReplacePartNumberAttr,&ReplacePartNumber)) goto EXIT;
											if (ReplacePartNumber)
											{
												   ReplacePartNumber=low_strssra(ReplacePartNumber,"\n",";");
											}
											if (ReplacePartNumber)
											{
												   ReplacePartNumber=low_strssra(ReplacePartNumber,"^",";");
											}

											if(!nlsIsStrNull(ReplacePartNumber))
											 {
											nlsStrCat(EpaValidParts, ReplacePartNumber);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5ReqForMepaAttr,&ReqForMepa)) goto EXIT;
											if (ReqForMepa)
											{
												   ReqForMepa=low_strssra(ReqForMepa,"\n",";");
											}
											if (ReqForMepa)
											{
												   ReqForMepa=low_strssra(ReqForMepa,"^",";");
											}

											if(!nlsIsStrNull(ReqForMepa))
											 {
											nlsStrCat(EpaValidParts, ReqForMepa);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5SampleIRMRAttr,&SampleIRMR)) goto EXIT;
											if (SampleIRMR)
											{
												   SampleIRMR=low_strssra(SampleIRMR,"\n",";");
											}
											if (SampleIRMR)
											{
												   SampleIRMR=low_strssra(SampleIRMR,"^",";");
											}

											if(!nlsIsStrNull(SampleIRMR))
											 {
											nlsStrCat(EpaValidParts, SampleIRMR);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5TryOutNumAttr,&TryOutNum)) goto EXIT;
											if (TryOutNum)
											{
												   TryOutNum=low_strssra(TryOutNum,"\n",";");
											}
											if (TryOutNum)
											{
												   TryOutNum=low_strssra(TryOutNum,"^",";");
											}

											if(!nlsIsStrNull(TryOutNum))
											 {
											nlsStrCat(EpaValidParts, TryOutNum);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCmTabAttr,q,t5VINNumAttr,&VINNum)) goto EXIT;
											if (VINNum)
											{
												   VINNum=low_strssra(VINNum,"\n",";");
											}
											if (VINNum)
											{
												   VINNum=low_strssra(VINNum,"^",";");
											}

											if(!nlsIsStrNull(VINNum))
											 {
											nlsStrCat(EpaValidParts, VINNum);
											nlsStrCat(EpaValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaValidParts, " ");
    										nlsStrCat(EpaValidParts, "^");
											 }
											fprintf(fpvalid,"^%s", EpaValidParts);fflush(stdout);
										    fprintf(fpvalid,"\n");fflush(stdout);
										}
									}
									 // End For EPA Valid Parts

                                     // Start For EPA InValid Parts
                                    if(dstat =objGetTableSize(EpaObj,t5EpaCrTabAttr,&InvalidTabSize)) goto EXIT;
									printf("\n\n No.of EPA InvalidTabSize:%d",InvalidTabSize);fflush(stdout);
									if(InvalidTabSize>0)
									{

										for (q=0;q<InvalidTabSize;q++)
										{
											EpaInValidParts = NULL;
											EpaInValidParts=nlsStrAlloc(5000);
											nlsStrCpy(EpaInValidParts, "");
											 fprintf(fpInvalid,"EpaInValid^%s", EPANameDup);fflush(stdout);
                                             PartnumberDupIn = NULL;
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,PartNumberAttr,&PartnumberDupIn)) goto EXIT;
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,RevisionAttr,&RevisionIn)) goto EXIT;
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,SequenceAttr,&SequenceIn)) goto EXIT;
											if (PartnumberDupIn)
											{
												   PartnumberDupIn=low_strssra(PartnumberDupIn,"\n",";");
											}
											if (PartnumberDupIn)
											{
												   PartnumberDupIn=low_strssra(PartnumberDupIn,"^",";");
											}

											if(!nlsIsStrNull(PartnumberDupIn))
											 {
											nlsStrCat(EpaInValidParts, PartnumberDupIn);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }

											if (RevisionIn)
											{
												nlsStrCat(EpaInValidParts, RevisionIn);
												nlsStrCat(EpaInValidParts, "^");
											}
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }

											if (SequenceIn)
											{
												nlsStrCat(EpaInValidParts, SequenceIn);
												nlsStrCat(EpaInValidParts, "^");
											}
										   else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }

											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5DisposalAction5Attr,&Assembly)) goto EXIT;
											if (Assembly)
											{
												   Assembly=low_strssra(Assembly,"\n",";");
											}
											if (Assembly)
											{
												   Assembly=low_strssra(Assembly,"^",";");
											}

											if(!nlsIsStrNull(Assembly))
											 {
											nlsStrCat(EpaInValidParts, Assembly);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5changeTypeAttr,&changeTypeIn)) goto EXIT;
											if (changeTypeIn)
											{
												   changeTypeIn=low_strssra(changeTypeIn,"\n",";");
											}
											if (changeTypeIn)
											{
												   changeTypeIn=low_strssra(changeTypeIn,"^",";");
											}

											if(!nlsIsStrNull(changeTypeIn))
											 {
											nlsStrCat(EpaInValidParts, changeTypeIn);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5DisposalAction2Attr,&Finished)) goto EXIT;
											if (Finished)
											{
												   Finished=low_strssra(Finished,"\n",";");
											}
											if (Finished)
											{
												   Finished=low_strssra(Finished,"^",";");
											}

											if(!nlsIsStrNull(Finished))
											 {
											nlsStrCat(EpaInValidParts, Finished);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5InHouseStockAttr,&InHouseStock)) goto EXIT;
											if (InHouseStock)
											{
												   InHouseStock=low_strssra(InHouseStock,"\n",";");
											}
											if (InHouseStock)
											{
												   InHouseStock=low_strssra(InHouseStock,"^",";");
											}

											if(!nlsIsStrNull(InHouseStock))
											 {
											nlsStrCat(EpaInValidParts, InHouseStock);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5InHousetTotalAttr,&InHousetTotal)) goto EXIT;
											if (InHousetTotal)
											{
												   InHousetTotal=low_strssra(InHousetTotal,"\n",";");
											}
											if (InHousetTotal)
											{
												   InHousetTotal=low_strssra(InHousetTotal,"^",";");
											}

											if(!nlsIsStrNull(InHousetTotal))
											 {
											nlsStrCat(EpaInValidParts, InHousetTotal);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5DisposalAction1Attr,&InShop)) goto EXIT;
											if (InShop)
											{
												   InShop=low_strssra(InShop,"\n",";");
											}
											if (InShop)
											{
												   InShop=low_strssra(InShop,"^",";");
											}

											if(!nlsIsStrNull(InShop))
											 {
											nlsStrCat(EpaInValidParts, InShop);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5IsPartDevAttr,&IsPartDevIn)) goto EXIT;
											if (IsPartDevIn)
											{
												   IsPartDevIn=low_strssra(IsPartDevIn,"\n",";");
											}
											if (IsPartDevIn)
											{
												   IsPartDevIn=low_strssra(IsPartDevIn,"^",";");
											}

											if(!nlsIsStrNull(IsPartDevIn))
											 {
											nlsStrCat(EpaInValidParts, IsPartDevIn);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5MBPAStorelocAttr,&MBPAStorelocIn)) goto EXIT;
											if (MBPAStorelocIn)
											{
												   MBPAStorelocIn=low_strssra(MBPAStorelocIn,"\n",";");
											}
											if (MBPAStorelocIn)
											{
												   MBPAStorelocIn=low_strssra(MBPAStorelocIn,"^",";");
											}

											if(!nlsIsStrNull(MBPAStorelocIn))
											 {
											nlsStrCat(EpaInValidParts, MBPAStorelocIn);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5OldQtyLAttr,&OldQtyL)) goto EXIT;
											if (OldQtyL)
											{
												   OldQtyL=low_strssra(OldQtyL,"\n",";");
											}
											if (OldQtyL)
											{
												   OldQtyL=low_strssra(OldQtyL,"^",";");
											}

											if(!nlsIsStrNull(OldQtyL))
											 {
											nlsStrCat(EpaInValidParts, OldQtyL);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5PeculiarityAttr,&Peculiarity)) goto EXIT;
											if (Peculiarity)
											{
												   Peculiarity=low_strssra(Peculiarity,"\n",";");
											}
											if (Peculiarity)
											{
												   Peculiarity=low_strssra(Peculiarity,"^",";");
											}

											if(!nlsIsStrNull(Peculiarity))
											 {
											nlsStrCat(EpaInValidParts, Peculiarity);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5PecuValidAttr,&PecuValid)) goto EXIT;
											if (PecuValid)
											{
												   PecuValid=low_strssra(PecuValid,"\n",";");
											}
											if (PecuValid)
											{
												   PecuValid=low_strssra(PecuValid,"^",";");
											}

											if(!nlsIsStrNull(PecuValid))
											 {
											nlsStrCat(EpaInValidParts, PecuValid);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5PoGoheadAttr,&PoGoheadIn)) goto EXIT;
											if (PoGoheadIn)
											{
												   PoGoheadIn=low_strssra(PoGoheadIn,"\n",";");
											}
											if (PoGoheadIn)
											{
												   PoGoheadIn=low_strssra(PoGoheadIn,"^",";");
											}

											if(!nlsIsStrNull(PoGoheadIn))
											 {
											nlsStrCat(EpaInValidParts, PoGoheadIn);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5PrtSupplierAttr,&PrtSupplierIn)) goto EXIT;
											if (PrtSupplierIn)
											{
												   PrtSupplierIn=low_strssra(PrtSupplierIn,"\n",";");
											}
											if (PrtSupplierIn)
											{
												   PrtSupplierIn=low_strssra(PrtSupplierIn,"^",";");
											}

											if(!nlsIsStrNull(PrtSupplierIn))
											 {
											nlsStrCat(EpaInValidParts, PrtSupplierIn);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5DisposalAction4Attr,&RawParts)) goto EXIT;
											if (RawParts)
											{
												   RawParts=low_strssra(RawParts,"\n",";");
											}
											if (RawParts)
											{
												   RawParts=low_strssra(RawParts,"^",";");
											}

											if(!nlsIsStrNull(RawParts))
											 {
											nlsStrCat(EpaInValidParts, RawParts);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5ReqForMepaAttr,&ReqForMepaIn)) goto EXIT;
											if (ReqForMepaIn)
											{
												   ReqForMepaIn=low_strssra(ReqForMepaIn,"\n",";");
											}
											if (ReqForMepaIn)
											{
												   ReqForMepaIn=low_strssra(ReqForMepaIn,"^",";");
											}

											if(!nlsIsStrNull(ReqForMepaIn))
											 {
											nlsStrCat(EpaInValidParts, ReqForMepaIn);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5SampleIRMRAttr,&SampleIRMRIn)) goto EXIT;
											if (SampleIRMRIn)
											{
												   SampleIRMRIn=low_strssra(SampleIRMRIn,"\n",";");
											}
											if (SampleIRMRIn)
											{
												   SampleIRMRIn=low_strssra(SampleIRMRIn,"^",";");
											}

											if(!nlsIsStrNull(SampleIRMRIn))
											 {
											nlsStrCat(EpaInValidParts, SampleIRMRIn);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5DisposalAction3Attr,&SPD)) goto EXIT;
											if (SPD)
											{
												   SPD=low_strssra(SPD,"\n",";");
											}
											if (SPD)
											{
												   SPD=low_strssra(SPD,"^",";");
											}

											if(!nlsIsStrNull(SPD))
											 {
											nlsStrCat(EpaInValidParts, SPD);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5SupplierEndAttr,&SupplierEnd)) goto EXIT;
											if (SupplierEnd)
											{
												   SupplierEnd=low_strssra(SupplierEnd,"\n",";");
											}
											if (SupplierEnd)
											{
												   SupplierEnd=low_strssra(SupplierEnd,"^",";");
											}

											if(!nlsIsStrNull(SupplierEnd))
											 {
											nlsStrCat(EpaInValidParts, SupplierEnd);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5TransitAttr,&Transit)) goto EXIT;
											if (Transit)
											{
												   Transit=low_strssra(Transit,"\n",";");
											}
											if (Transit)
											{
												   Transit=low_strssra(Transit,"^",";");
											}

											if(!nlsIsStrNull(Transit))
											 {
											nlsStrCat(EpaInValidParts, Transit);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											if(dstat = objGetTableAttribute(EpaObj,t5EpaCrTabAttr,q,t5WareHouseAttr,&WareHouse)) goto EXIT;
											if (WareHouse)
											{
												   WareHouse=low_strssra(WareHouse,"\n",";");
											}
											if (WareHouse)
											{
												   WareHouse=low_strssra(WareHouse,"^",";");
											}

											if(!nlsIsStrNull(WareHouse))
											 {
											nlsStrCat(EpaInValidParts, WareHouse);
											nlsStrCat(EpaInValidParts, "^");
											 }
											 else
											 {
											nlsStrCat(EpaInValidParts, " ");
											nlsStrCat(EpaInValidParts, "^");
											 }
											fprintf(fpInvalid,"^%s", EpaInValidParts);fflush(stdout);
										   fprintf(fpInvalid,"\n");fflush(stdout);
										}
									}
									 // End For EPA InValid Parts

								}
								else
								{
									printf("\nNo EPA\n");fflush(stdout);
									continue;
								}
                                // End for EPA Relationship Details

                                 // Start for EPA details
                                if (EPANameDup)
								{
									   EPANameDup=low_strssra(EPANameDup,"\n",";");
								}
                                if (EPANameDup)
								{
									   EPANameDup=low_strssra(EPANameDup,"^",";");
								}

								if(!nlsIsStrNull(EPANameDup))
								 {
								nlsStrCat(EpaWholeStr, EPANameDup);
                                nlsStrCat(EpaWholeStr, "^");
								 }
								 else
								 {
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								 }

								if(dstat = objGetAttribute(EpaObj,AQEngineerAttr,&AQEngineer)) goto EXIT;
								if(!nlsIsStrNull(AQEngineer)) AQEngineerDup=nlsStrDup(AQEngineer);
                                if (AQEngineerDup)
								{
									   AQEngineerDup=low_strssra(AQEngineerDup,"\n",";");
								}
                               if (AQEngineerDup)
								{
									   AQEngineerDup=low_strssra(AQEngineerDup,"^",";");
								}

								if(!nlsIsStrNull(AQEngineerDup))
								 {
								nlsStrCat(EpaWholeStr, AQEngineerDup);
                                nlsStrCat(EpaWholeStr, "^");
								 }
								 else
								 {
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								 }

								if(dstat = objGetAttribute(EpaObj,AQGroupLeaderAttr,&AQGroupLeader)) goto EXIT;
								if(!nlsIsStrNull(AQGroupLeader)) AQGroupLeaderDup=nlsStrDup(AQGroupLeader);
                                if (AQGroupLeaderDup)
								{
									   AQGroupLeaderDup=low_strssra(AQGroupLeaderDup,"\n",";");
								}
                                 if (AQGroupLeaderDup)
								{
									   AQGroupLeaderDup=low_strssra(AQGroupLeaderDup,"^",";");
								}

								if(!nlsIsStrNull(AQGroupLeaderDup))
								{
								nlsStrCat(EpaWholeStr, AQGroupLeaderDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,ActionByAgencyAttr,&ActionByAgency)) goto EXIT;
								if(!nlsIsStrNull(ActionByAgency)) ActionByAgencyDup=nlsStrDup(ActionByAgency);
                                if (ActionByAgencyDup)
								{
									   ActionByAgencyDup=low_strssra(ActionByAgencyDup,"\n",";");
								}
                                if (ActionByAgencyDup)
								{
									   ActionByAgencyDup=low_strssra(ActionByAgencyDup,"^",";");
								}

                                if(!nlsIsStrNull(ActionByAgencyDup))
								{
								nlsStrCat(EpaWholeStr, ActionByAgencyDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,ActionTobeTakenAttr,&ActionTobeTaken)) goto EXIT;
								if(!nlsIsStrNull(ActionTobeTaken)) ActionTobeTakenDup=nlsStrDup(ActionTobeTaken);
                                if (ActionTobeTakenDup)
								{
									   ActionTobeTakenDup=low_strssra(ActionTobeTakenDup,"\n",";");
								}
                                if (ActionTobeTakenDup)
								{
									   ActionTobeTakenDup=low_strssra(ActionTobeTakenDup,"^",";");
								}

								if(!nlsIsStrNull(ActionTobeTakenDup))
								{
								nlsStrCat(EpaWholeStr, ActionTobeTakenDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,ClosureCommentsAttr,&ClosureComments)) goto EXIT;
								if(!nlsIsStrNull(ClosureComments)) ClosureCommentsDup=nlsStrDup(ClosureComments);
							    if (ClosureCommentsDup)
								{
									   ClosureCommentsDup=low_strssra(ClosureCommentsDup,"\n",";");
								}
								if (ClosureCommentsDup)
								 {
									   ClosureCommentsDup=low_strssra(ClosureCommentsDup,"^",";");
								}
                                if(!nlsIsStrNull(ClosureCommentsDup))
								{
								nlsStrCat(EpaWholeStr, ClosureCommentsDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,CustomerAttr,&Customer)) goto EXIT;
								if(!nlsIsStrNull(Customer)) CustomerDup=nlsStrDup(Customer);
          					    if (CustomerDup)
								{
									   CustomerDup=low_strssra(CustomerDup,"\n",";");
								}
								if (CustomerDup)
								 {
									   CustomerDup=low_strssra(CustomerDup,"^",";");
								}
								if(!nlsIsStrNull(CustomerDup))
								{
								nlsStrCat(EpaWholeStr, CustomerDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,DateOfIntroInVC2Attr,&DateOfIntroInVC2)) goto EXIT;
								if(!nlsIsStrNull(DateOfIntroInVC2)) DateOfIntroInVC2Dup=nlsStrDup(DateOfIntroInVC2);
                                if (DateOfIntroInVC2Dup)
								{
									   DateOfIntroInVC2Dup=low_strssra(DateOfIntroInVC2Dup,"\n",";");
								}
								if (DateOfIntroInVC2Dup)
								 {
									   DateOfIntroInVC2Dup=low_strssra(DateOfIntroInVC2Dup,"^",";");
								}
								if(!nlsIsStrNull(DateOfIntroInVC2Dup))
								{
								nlsStrCat(EpaWholeStr, DateOfIntroInVC2Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,DateOfIntroInVC3Attr,&DateOfIntroInVC3)) goto EXIT;
								if(!nlsIsStrNull(DateOfIntroInVC3)) DateOfIntroInVC3Dup=nlsStrDup(DateOfIntroInVC3);
								if (DateOfIntroInVC3Dup)
								{
									   DateOfIntroInVC3Dup=low_strssra(DateOfIntroInVC3Dup,"\n",";");
								}
								if (DateOfIntroInVC3Dup)
								 {
									   DateOfIntroInVC3Dup=low_strssra(DateOfIntroInVC3Dup,"^",";");
								}
                                if(!nlsIsStrNull(DateOfIntroInVC3Dup))
								{
								nlsStrCat(EpaWholeStr, DateOfIntroInVC3Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,DateOfIntroInVC4Attr,&DateOfIntroInVC4)) goto EXIT;
								if(!nlsIsStrNull(DateOfIntroInVC4)) DateOfIntroInVC4Dup=nlsStrDup(DateOfIntroInVC4);
								if (DateOfIntroInVC4Dup)
								{
									   DateOfIntroInVC4Dup=low_strssra(DateOfIntroInVC4Dup,"\n",";");
								}
								if (DateOfIntroInVC4Dup)
								 {
									   DateOfIntroInVC4Dup=low_strssra(DateOfIntroInVC4Dup,"^",";");
								}
                                if(!nlsIsStrNull(DateOfIntroInVC4Dup))
								{
								nlsStrCat(EpaWholeStr, DateOfIntroInVC4Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,DateOfIntroInVCE2Attr,&DateOfIntroInVCE2)) goto EXIT;
								if(!nlsIsStrNull(DateOfIntroInVCE2)) DateOfIntroInVCE2Dup=nlsStrDup(DateOfIntroInVCE2);
                  				if (DateOfIntroInVCE2Dup)
								{
									   DateOfIntroInVCE2Dup=low_strssra(DateOfIntroInVCE2Dup,"\n",";");
								}
								if (DateOfIntroInVCE2Dup)
								 {
									   DateOfIntroInVCE2Dup=low_strssra(DateOfIntroInVCE2Dup,"^",";");
								}

								if(!nlsIsStrNull(DateOfIntroInVCE2Dup))
								{
								nlsStrCat(EpaWholeStr, DateOfIntroInVCE2Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,DateOfIntroInVCE3Attr,&DateOfIntroInVCE3)) goto EXIT;
								if(!nlsIsStrNull(DateOfIntroInVCE3)) DateOfIntroInVCE3Dup=nlsStrDup(DateOfIntroInVCE3);
                            	if (DateOfIntroInVCE3Dup)
								{
									   DateOfIntroInVCE3Dup=low_strssra(DateOfIntroInVCE3Dup,"\n",";");
								}
								if (DateOfIntroInVCE3Dup)
								 {
									   DateOfIntroInVCE3Dup=low_strssra(DateOfIntroInVCE3Dup,"^",";");
								}

								if(!nlsIsStrNull(DateOfIntroInVCE3Dup))
								{
								nlsStrCat(EpaWholeStr, DateOfIntroInVCE3Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,EmailAddAttr,&EmailAdd)) goto EXIT;
								if(!nlsIsStrNull(EmailAdd)) EmailAddDup=nlsStrDup(EmailAdd);
                               	if (EmailAddDup)
								{
									   EmailAddDup=low_strssra(EmailAddDup,"\n",";");
								}
								if (EmailAddDup)
								 {
									   EmailAddDup=low_strssra(EmailAddDup,"^",";");
								}
								if(!nlsIsStrNull(EmailAddDup))
								{
								nlsStrCat(EpaWholeStr, EmailAddDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,EpaMeetinDateAttr,&EpaMeetinDate)) goto EXIT;
								if(!nlsIsStrNull(EpaMeetinDate)) EpaMeetinDateDup=nlsStrDup(EpaMeetinDate);
								if (EpaMeetinDateDup)
								{
									   EpaMeetinDateDup=low_strssra(EpaMeetinDateDup,"\n",";");
								}
								if (EpaMeetinDateDup)
								 {
									   EpaMeetinDateDup=low_strssra(EpaMeetinDateDup,"^",";");
								}
                                if(!nlsIsStrNull(EpaMeetinDateDup))
								{
								nlsStrCat(EpaWholeStr, EpaMeetinDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,EpaPlantAAttr,&EpaPlantA)) goto EXIT;
								if(!nlsIsStrNull(EpaPlantA)) EpaPlantADup=nlsStrDup(EpaPlantA);
								if (EpaPlantADup)
								{
									   EpaPlantADup=low_strssra(EpaPlantADup,"\n",";");
								}
								if (EpaPlantADup)
								 {
									   EpaPlantADup=low_strssra(EpaPlantADup,"^",";");
								}
                                if(!nlsIsStrNull(EpaPlantADup))
								{
								nlsStrCat(EpaWholeStr, EpaPlantADup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,InterchangeCodesAttr,&InterchangeCodes)) goto EXIT;
								if(!nlsIsStrNull(InterchangeCodes)) InterchangeCodesDup=nlsStrDup(InterchangeCodes);
								if (InterchangeCodesDup)
								{
									   InterchangeCodesDup=low_strssra(InterchangeCodesDup,"\n",";");
								}
								if (InterchangeCodesDup)
								 {
									   InterchangeCodesDup=low_strssra(InterchangeCodesDup,"^",";");
								}

                                if(!nlsIsStrNull(InterchangeCodesDup))
								{
								nlsStrCat(EpaWholeStr, InterchangeCodesDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,MEPA_NOAttr,&MEPA_NO)) goto EXIT;
								if(!nlsIsStrNull(MEPA_NO)) MEPA_NODup=nlsStrDup(MEPA_NO);
								if (MEPA_NODup)
								{
									   MEPA_NODup=low_strssra(MEPA_NODup,"\n",";");
								}
								if (MEPA_NODup)
								 {
									   MEPA_NODup=low_strssra(MEPA_NODup,"^",";");
								}

                                if(!nlsIsStrNull(MEPA_NODup))
								{
								nlsStrCat(EpaWholeStr, MEPA_NODup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,MbpaCreDateAttr,&MbpaCreDate)) goto EXIT;
								if(!nlsIsStrNull(MbpaCreDate)) MbpaCreDateDup=nlsStrDup(MbpaCreDate);
								if (MbpaCreDateDup)
								{
									   MbpaCreDateDup=low_strssra(MbpaCreDateDup,"\n",";");
								}
								if (MbpaCreDateDup)
								 {
									   MbpaCreDateDup=low_strssra(MbpaCreDateDup,"^",";");
								}

                                if(!nlsIsStrNull(MbpaCreDateDup))
								{
								nlsStrCat(EpaWholeStr, MbpaCreDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,MbpaCretorAttr,&MbpaCretor)) goto EXIT;
								if(!nlsIsStrNull(MbpaCretor)) MbpaCretorDup=nlsStrDup(MbpaCretor);
								if (MbpaCretorDup)
								{
									   MbpaCretorDup=low_strssra(MbpaCretorDup,"\n",";");
								}
								if (MbpaCretorDup)
								 {
									   MbpaCretorDup=low_strssra(MbpaCretorDup,"^",";");
								}

                                if(!nlsIsStrNull(MbpaCretorDup))
								{
								nlsStrCat(EpaWholeStr, MbpaCretorDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,MepaCreDateAttr,&MepaCreDate)) goto EXIT;
								if(!nlsIsStrNull(MepaCreDate)) MepaCreDateDup=nlsStrDup(MepaCreDate);
								if (MepaCreDateDup)
								{
									   MepaCreDateDup=low_strssra(MepaCreDateDup,"\n",";");
								}
								if (MepaCreDateDup)
								 {
									   MepaCreDateDup=low_strssra(MepaCreDateDup,"^",";");
								}


                                if(!nlsIsStrNull(MepaCreDateDup))
								{
								nlsStrCat(EpaWholeStr, MepaCreDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,MepaCretorAttr,&MepaCretor)) goto EXIT;
								if(!nlsIsStrNull(MepaCretor)) MepaCretorDup=nlsStrDup(MepaCretor);
								if (MepaCretorDup)
								{
									   MepaCretorDup=low_strssra(MepaCretorDup,"\n",";");
								}
								if (MepaCretorDup)
								 {
									   MepaCretorDup=low_strssra(MepaCretorDup,"^",";");
								}

                                if(!nlsIsStrNull(MepaCretorDup))
								{
								nlsStrCat(EpaWholeStr, MepaCretorDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,PlanningStatusAttr,&PlanningStatus)) goto EXIT;
								if(!nlsIsStrNull(PlanningStatus)) PlanningStatusDup=nlsStrDup(PlanningStatus);
								if (PlanningStatusDup)
								{
									   PlanningStatusDup=low_strssra(PlanningStatusDup,"\n",";");
								}
								if (PlanningStatusDup)
								 {
									   PlanningStatusDup=low_strssra(PlanningStatusDup,"^",";");
								}

                                if(!nlsIsStrNull(PlanningStatusDup))
								{
								nlsStrCat(EpaWholeStr, PlanningStatusDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,WbsPriorityAttr,&WbsPriority)) goto EXIT;
								if(!nlsIsStrNull(WbsPriority)) WbsPriorityDup=nlsStrDup(WbsPriority);
								if (WbsPriorityDup)
								{
									   WbsPriorityDup=low_strssra(WbsPriorityDup,"\n",";");
								}
								if (WbsPriorityDup)
								 {
									   WbsPriorityDup=low_strssra(WbsPriorityDup,"^",";");
								}

                                if(!nlsIsStrNull(WbsPriorityDup))
								{
								nlsStrCat(EpaWholeStr, WbsPriorityDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5AggreCutOffNoAttr,&t5AggreCutOffNo)) goto EXIT;
								if(!nlsIsStrNull(t5AggreCutOffNo)) t5AggreCutOffNoDup=nlsStrDup(t5AggreCutOffNo);
								if (t5AggreCutOffNoDup)
								{
									   t5AggreCutOffNoDup=low_strssra(t5AggreCutOffNoDup,"\n",";");
								}
								if (t5AggreCutOffNoDup)
								 {
									   t5AggreCutOffNoDup=low_strssra(t5AggreCutOffNoDup,"^",";");
								}

                                if(!nlsIsStrNull(t5AggreCutOffNoDup))
								{
								nlsStrCat(EpaWholeStr, t5AggreCutOffNoDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

						    	if(dstat = objGetAttribute(EpaObj,t5AggreCutOffNo1Attr,&t5AggreCutOffNo1)) goto EXIT;
								if(!nlsIsStrNull(t5AggreCutOffNo1)) t5AggreCutOffNo1Dup=nlsStrDup(t5AggreCutOffNo1);
								if (t5AggreCutOffNo1Dup)
								{
									   t5AggreCutOffNo1Dup=low_strssra(t5AggreCutOffNo1Dup,"\n",";");
								}
								if (t5AggreCutOffNo1Dup)
								 {
									   t5AggreCutOffNo1Dup=low_strssra(t5AggreCutOffNo1Dup,"^",";");
								}

                                if(!nlsIsStrNull(t5AggreCutOffNo1Dup))
								{
								nlsStrCat(EpaWholeStr, t5AggreCutOffNo1Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

                                t5AppModelListDup = NULL;
							    t5AppModelListDup =nlsStrAlloc(100);
								nlsStrCpy(t5AppModelListDup,"");
    							if(dstat = objListGetList(EpaObj,t5AppModelListAttr,&EpaModelAppl)) goto  EXIT ;
								if(setSize(EpaModelAppl)>0)
								{
								for(i=0;i<setSize(EpaModelAppl);i++)
									{
										if(t5AppModelList) t5AppModelList=NULL;
			                         	if(dstat=objListGet(EpaObj,t5AppModelListAttr,i,&t5AppModelList)) goto EXIT;
										printf("\n AppModel List Selected ...%s \n",t5AppModelList);
										nlsStrCat(t5AppModelListDup,t5AppModelList);
										nlsStrCat(t5AppModelListDup,",");
										}
								}
								if (t5AppModelListDup)
								{
									   t5AppModelListDup=low_strssra(t5AppModelListDup,"\n",";");
								}
								if (t5AppModelListDup)
								 {
									   t5AppModelListDup=low_strssra(t5AppModelListDup,"^",";");
								}

                                if(!nlsIsStrNull(t5AppModelListDup))
								{
								nlsStrCat(EpaWholeStr, t5AppModelListDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5BomComuAttr,&t5BomComu)) goto EXIT;
								if(!nlsIsStrNull(t5BomComu)) t5BomComuDup=nlsStrDup(t5BomComu);
								if (t5BomComuDup)
								{
									   t5BomComuDup=low_strssra(t5BomComuDup,"\n",";");
								}
								if (t5BomComuDup)
								 {
									   t5BomComuDup=low_strssra(t5BomComuDup,"^",";");
								}

                                if(!nlsIsStrNull(t5BomComuDup))
								{
								nlsStrCat(EpaWholeStr, t5BomComuDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5BreakPointDateAttr,&t5BreakPointDate)) goto EXIT;
								if(!nlsIsStrNull(t5BreakPointDate)) t5BreakPointDateDup=nlsStrDup(t5BreakPointDate);
								if (t5BreakPointDateDup)
								{
									   t5BreakPointDateDup=low_strssra(t5BreakPointDateDup,"\n",";");
								}
								if (t5BreakPointDateDup)
								 {
									   t5BreakPointDateDup=low_strssra(t5BreakPointDateDup,"^",";");
								}

                                if(!nlsIsStrNull(t5BreakPointDateDup))
								{
								nlsStrCat(EpaWholeStr, t5BreakPointDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5BtchCodeAttr,&t5BtchCode)) goto EXIT;
								if(!nlsIsStrNull(t5BtchCode)) t5BtchCodeDup=nlsStrDup(t5BtchCode);
								if (t5BtchCodeDup)
								{
									   t5BtchCodeDup=low_strssra(t5BtchCodeDup,"\n",";");
								}
								if (t5BtchCodeDup)
								 {
									   t5BtchCodeDup=low_strssra(t5BtchCodeDup,"^",";");
								}
                                if(!nlsIsStrNull(t5BtchCodeDup))
								{
								nlsStrCat(EpaWholeStr, t5BtchCodeDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5CTChangeAttr,&t5CTChange)) goto EXIT;
								if(!nlsIsStrNull(t5CTChange)) t5CTChangeDup=nlsStrDup(t5CTChange);
								if (t5CTChangeDup)
								{
									   t5CTChangeDup=low_strssra(t5CTChangeDup,"\n",";");
								}
								if (t5CTChangeDup)
								 {
									   t5CTChangeDup=low_strssra(t5CTChangeDup,"^",";");
								}
                                if(!nlsIsStrNull(t5CTChangeDup))
								{
								nlsStrCat(EpaWholeStr, t5CTChangeDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5Category_code1Attr,&t5Category_code1)) goto EXIT;
								if(!nlsIsStrNull(t5Category_code1))t5Category_code1Dup=nlsStrDup(t5Category_code1);
								if (t5Category_code1Dup)
								{
									   t5Category_code1Dup=low_strssra(t5Category_code1Dup,"\n",";");
								}
								if (t5Category_code1Dup)
								 {
									   t5Category_code1Dup=low_strssra(t5Category_code1Dup,"^",";");
								}
                               	if (t5Category_code1Dup)
								 {
									   t5Category_code1Dup=low_strssra(t5Category_code1Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5Category_code1Dup))
								{
								nlsStrCat(EpaWholeStr, t5Category_code1Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5Category_code2Attr,&t5Category_code2)) goto EXIT;
								if(!nlsIsStrNull(t5Category_code2))t5Category_code2Dup=nlsStrDup(t5Category_code2);
								if (t5Category_code2Dup)
								{
									   t5Category_code2Dup=low_strssra(t5Category_code2Dup,"\n",";");
								}
								if (t5Category_code2Dup)
								 {
									   t5Category_code2Dup=low_strssra(t5Category_code2Dup,"^",";");
								}
                               	if (t5Category_code2Dup)
								 {
									   t5Category_code2Dup=low_strssra(t5Category_code2Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5Category_code2Dup))
								{
								nlsStrCat(EpaWholeStr, t5Category_code2Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5Category_code3Attr,&t5Category_code3)) goto EXIT;
								if(!nlsIsStrNull(t5Category_code3))t5Category_code3Dup=nlsStrDup(t5Category_code3);
								if (t5Category_code3Dup)
								{
									   t5Category_code3Dup=low_strssra(t5Category_code3Dup,"\n",";");
								}
								if (t5Category_code3Dup)
								 {
									   t5Category_code3Dup=low_strssra(t5Category_code3Dup,"^",";");
								}

							    if(!nlsIsStrNull(t5Category_code3Dup))
								{
								nlsStrCat(EpaWholeStr, t5Category_code3Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5Category_code4Attr,&t5Category_code4)) goto EXIT;
								if(!nlsIsStrNull(t5Category_code4))t5Category_code4Dup=nlsStrDup(t5Category_code4);
								if (t5Category_code4Dup)
								{
									   t5Category_code4Dup=low_strssra(t5Category_code4Dup,"\n",";");
								}
								if (t5Category_code4Dup)
								 {
									   t5Category_code4Dup=low_strssra(t5Category_code4Dup,"^",";");
								}
  							    if(!nlsIsStrNull(t5Category_code4Dup))
								{
								nlsStrCat(EpaWholeStr, t5Category_code4Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5Category_code5Attr,&t5Category_code5)) goto EXIT;
								if(!nlsIsStrNull(t5Category_code5))t5Category_code5Dup=nlsStrDup(t5Category_code5);
								if (t5Category_code5Dup)
								{
									   t5Category_code5Dup=low_strssra(t5Category_code5Dup,"\n",";");
								}
                               	if (t5Category_code5Dup)
								 {
									   t5Category_code5Dup=low_strssra(t5Category_code5Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5Category_code5Dup))
								{
								nlsStrCat(EpaWholeStr, t5Category_code5Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5Category_code6Attr,&t5Category_code6)) goto EXIT;
								if(!nlsIsStrNull(t5Category_code6))t5Category_code6Dup=nlsStrDup(t5Category_code6);
								if (t5Category_code6Dup)
								{
									   t5Category_code6Dup=low_strssra(t5Category_code6Dup,"\n",";");
								}
                               	if (t5Category_code6Dup)
								 {
									   t5Category_code6Dup=low_strssra(t5Category_code6Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5Category_code6Dup))
								{
								nlsStrCat(EpaWholeStr, t5Category_code6Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ChasisNoAttr,&t5ChasisNo)) goto EXIT;
								if(!nlsIsStrNull(t5ChasisNo))t5ChasisNoDup=nlsStrDup(t5ChasisNo);
								if (t5ChasisNoDup)
								{
									   t5ChasisNoDup=low_strssra(t5ChasisNoDup,"\n",";");
								}
                               	if (t5ChasisNoDup)
								 {
									   t5ChasisNoDup=low_strssra(t5ChasisNoDup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisNoDup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisNoDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ChasisNo2Attr,&t5ChasisNo2)) goto EXIT;
								if(!nlsIsStrNull(t5ChasisNo2))t5ChasisNo2Dup=nlsStrDup(t5ChasisNo2);
								if (t5ChasisNo2Dup)
								{
									   t5ChasisNo2Dup=low_strssra(t5ChasisNo2Dup,"\n",";");
								}
                               	if (t5ChasisNo2Dup)
								 {
									   t5ChasisNo2Dup=low_strssra(t5ChasisNo2Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisNo2Dup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisNo2Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ChasisNo3Attr,&t5ChasisNo3)) goto EXIT;
								if(!nlsIsStrNull(t5ChasisNo3))t5ChasisNo3Dup=nlsStrDup(t5ChasisNo3);
								if (t5ChasisNo3Dup)
								{
									   t5ChasisNo3Dup=low_strssra(t5ChasisNo3Dup,"\n",";");
								}
                               	if (t5ChasisNo3Dup)
								 {
									   t5ChasisNo3Dup=low_strssra(t5ChasisNo3Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisNo3Dup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisNo3Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ChasisNo4Attr,&t5ChasisNo4)) goto EXIT;
								if(!nlsIsStrNull(t5ChasisNo4))t5ChasisNo4Dup=nlsStrDup(t5ChasisNo4);
								if (t5ChasisNo4Dup)
								{
									   t5ChasisNo4Dup=low_strssra(t5ChasisNo4Dup,"\n",";");
								}
                               	if (t5ChasisNo4Dup)
								 {
									   t5ChasisNo4Dup=low_strssra(t5ChasisNo4Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisNo4Dup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisNo4Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ChasisNoEAttr,&t5ChasisNoE)) goto EXIT;
								if(!nlsIsStrNull(t5ChasisNoE))t5ChasisNoEDup=nlsStrDup(t5ChasisNoE);
								if (t5ChasisNoEDup)
								{
									   t5ChasisNoEDup=low_strssra(t5ChasisNoEDup,"\n",";");
								}
                               	if (t5ChasisNoEDup)
								 {
									   t5ChasisNoEDup=low_strssra(t5ChasisNoEDup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisNoEDup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisNoEDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ChasisNoE2Attr,&t5ChasisNoE2)) goto EXIT;
								if(!nlsIsStrNull(t5ChasisNoE2))t5ChasisNoE2Dup=nlsStrDup(t5ChasisNoE2);
								if (t5ChasisNoE2Dup)
								{
									   t5ChasisNoE2Dup=low_strssra(t5ChasisNoE2Dup,"\n",";");
								}
                               	if (t5ChasisNoE2Dup)
								 {
									   t5ChasisNoE2Dup=low_strssra(t5ChasisNoE2Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisNoE2Dup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisNoE2Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ChasisNoE3Attr,&t5ChasisNoE3)) goto EXIT;
								if(!nlsIsStrNull(t5ChasisNoE3))t5ChasisNoE3Dup=nlsStrDup(t5ChasisNoE3);
								if (t5ChasisNoE3Dup)
								{
									   t5ChasisNoE3Dup=low_strssra(t5ChasisNoE3Dup,"\n",";");
								}
                               	if (t5ChasisNoE3Dup)
								 {
									   t5ChasisNoE3Dup=low_strssra(t5ChasisNoE3Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisNoE3Dup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisNoE3Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ChasisNoE4Attr,&t5ChasisNoE4)) goto EXIT;
								if(!nlsIsStrNull(t5ChasisNoE4))t5ChasisNoE4Dup=nlsStrDup(t5ChasisNoE4);
								if (t5ChasisNoE4Dup)
								{
									   t5ChasisNoE4Dup=low_strssra(t5ChasisNoE4Dup,"\n",";");
								}
                               	if (t5ChasisNoE4Dup)
								 {
									   t5ChasisNoE4Dup=low_strssra(t5ChasisNoE4Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisNoE4Dup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisNoE4Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

                                t5ChasisTypeList = NULL;
                                t5ChasisTypeListDup = NULL;
							    t5ChasisTypeListDup =nlsStrAlloc(100);
								nlsStrCpy(t5ChasisTypeListDup,"");
    							if(dstat = objListGetList(EpaObj,t5ChasisTypeListAttr,&EpaChassisList)) goto  EXIT ;
								if(setSize(EpaChassisList)>0)
								{
									for(i=0;i<setSize(EpaChassisList);i++)
									{
										if(t5ChasisTypeList) t5ChasisTypeList=NULL;
			                         	if(dstat=objListGet(EpaObj,t5ChasisTypeListAttr,i,&t5ChasisTypeList)) goto EXIT;
										printf("\n Veh Class Chassis Selected ...%s \n",t5ChasisTypeList);
										nlsStrCat(t5ChasisTypeListDup,t5ChasisTypeList);
										nlsStrCat(t5ChasisTypeListDup,",");
									}
								}
								if (t5ChasisTypeListDup)
								{
									   t5ChasisTypeListDup=low_strssra(t5ChasisTypeListDup,"\n",";");
								}
                               	if (t5ChasisTypeListDup)
								 {
									   t5ChasisTypeListDup=low_strssra(t5ChasisTypeListDup,"^",";");
								}
							    if(!nlsIsStrNull(t5ChasisTypeListDup))
								{
								nlsStrCat(EpaWholeStr, t5ChasisTypeListDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5CommnMepaStrAttr,&t5CommnMepaStr)) goto EXIT;
								if(!nlsIsStrNull(t5CommnMepaStr))t5CommnMepaStrDup=nlsStrDup(t5CommnMepaStr);
								if (t5CommnMepaStrDup)
								{
									   t5CommnMepaStrDup=low_strssra(t5CommnMepaStrDup,"\n",";");
								}
                               	if (t5CommnMepaStrDup)
								 {
									   t5CommnMepaStrDup=low_strssra(t5CommnMepaStrDup,"^",";");
								}
							    if(!nlsIsStrNull(t5CommnMepaStrDup))
								{
								nlsStrCat(EpaWholeStr, t5CommnMepaStrDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5CostReductionAttr,&t5CostReduction)) goto EXIT;
								if(!nlsIsStrNull(t5CostReduction))t5CostReductionDup=nlsStrDup(t5CostReduction);
								if (t5CostReductionDup)
								{
									   t5CostReductionDup=low_strssra(t5CostReductionDup,"\n",";");
								}
                               	if (t5CostReductionDup)
								 {
									   t5CostReductionDup=low_strssra(t5CostReductionDup,"^",";");
								}
							    if(!nlsIsStrNull(t5CostReductionDup))
								{
								nlsStrCat(EpaWholeStr, t5CostReductionDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5DesignGroupAttr,&t5DesignGroup)) goto EXIT;
								if(!nlsIsStrNull(t5DesignGroup))t5DesignGroupDup=nlsStrDup(t5DesignGroup);
								if (t5DesignGroupDup)
								{
									   t5DesignGroupDup=low_strssra(t5DesignGroupDup,"\n",";");
								}
                               	if (t5DesignGroupDup)
								 {
									   t5DesignGroupDup=low_strssra(t5DesignGroupDup,"^",";");
								}
							    if(!nlsIsStrNull(t5DesignGroupDup))
								{
								nlsStrCat(EpaWholeStr, t5DesignGroupDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5DisposalAction1Attr,&t5DisposalAction1)) goto EXIT;
								if(!nlsIsStrNull(t5DisposalAction1))t5DisposalAction1Dup=nlsStrDup(t5DisposalAction1);
								if (t5DisposalAction1Dup)
								{
									   t5DisposalAction1Dup=low_strssra(t5DisposalAction1Dup,"\n",";");
								}
                               	if (t5DisposalAction1Dup)
								 {
									   t5DisposalAction1Dup=low_strssra(t5DisposalAction1Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5DisposalAction1Dup))
								{
								nlsStrCat(EpaWholeStr, t5DisposalAction1Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5DisposalAction2Attr,&t5DisposalAction2)) goto EXIT;
								if(!nlsIsStrNull(t5DisposalAction2))t5DisposalAction2Dup=nlsStrDup(t5DisposalAction2);
								if (t5DisposalAction2Dup)
								{
									   t5DisposalAction2Dup=low_strssra(t5DisposalAction2Dup,"\n",";");
								}
                               	if (t5DisposalAction2Dup)
								 {
									   t5DisposalAction2Dup=low_strssra(t5DisposalAction2Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5DisposalAction2Dup))
								{
								nlsStrCat(EpaWholeStr, t5DisposalAction2Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5DisposalAction3Attr,&t5DisposalAction3)) goto EXIT;
								if(!nlsIsStrNull(t5DisposalAction3))t5DisposalAction3Dup=nlsStrDup(t5DisposalAction3);
								if (t5DisposalAction3Dup)
								{
									   t5DisposalAction3Dup=low_strssra(t5DisposalAction3Dup,"\n",";");
								}
                               	if (t5DisposalAction3Dup)
								 {
									   t5DisposalAction3Dup=low_strssra(t5DisposalAction3Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5DisposalAction3Dup))
								{
								nlsStrCat(EpaWholeStr, t5DisposalAction3Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}


								if(dstat = objGetAttribute(EpaObj,t5DisposalAction4Attr,&t5DisposalAction4)) goto EXIT;
								if(!nlsIsStrNull(t5DisposalAction4))t5DisposalAction4Dup=nlsStrDup(t5DisposalAction4);
								if (t5DisposalAction4Dup)
								{
									   t5DisposalAction4Dup=low_strssra(t5DisposalAction4Dup,"\n",";");
								}
                               	if (t5DisposalAction4Dup)
								 {
									   t5DisposalAction4Dup=low_strssra(t5DisposalAction4Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5DisposalAction4Dup))
								{
								nlsStrCat(EpaWholeStr, t5DisposalAction4Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5DisposalAction5Attr,&t5DisposalAction5)) goto EXIT;
								if(!nlsIsStrNull(t5DisposalAction5))t5DisposalAction5Dup=nlsStrDup(t5DisposalAction5);
								if (t5DisposalAction5Dup)
								{
									   t5DisposalAction5Dup=low_strssra(t5DisposalAction5Dup,"\n",";");
								}
                               	if (t5DisposalAction5Dup)
								 {
									   t5DisposalAction5Dup=low_strssra(t5DisposalAction5Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5DisposalAction5Dup))
								{
								nlsStrCat(EpaWholeStr, t5DisposalAction5Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,"t5DumCompln",&t5DumCompln)) goto EXIT;
								if(!nlsIsStrNull(t5DumCompln))t5DumComplnDup=nlsStrDup(t5DumCompln);
								if (t5DumComplnDup)
								{
									   t5DumComplnDup=low_strssra(t5DumComplnDup,"\n",";");
								}
                               	if (t5DumComplnDup)
								 {
									   t5DumComplnDup=low_strssra(t5DumComplnDup,"^",";");
								}
							    if(!nlsIsStrNull(t5DumComplnDup))
								{
								nlsStrCat(EpaWholeStr, t5DumComplnDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5DumEpNoAttr,&t5DumEpNo)) goto EXIT;
								if(!nlsIsStrNull(t5DumEpNo))t5DumEpNoDup=nlsStrDup(t5DumEpNo);
								if (t5DumEpNoDup)
								{
									   t5DumEpNoDup=low_strssra(t5DumEpNoDup,"\n",";");
								}
                               	if (t5DumEpNoDup)
								 {
									   t5DumEpNoDup=low_strssra(t5DumEpNoDup,"^",";");
								}
							    if(!nlsIsStrNull(t5DumEpNoDup))
								{
								nlsStrCat(EpaWholeStr, t5DumEpNoDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ECNTstRepNoAttr,&t5ECNTstRepNo)) goto EXIT;
								if(!nlsIsStrNull(t5ECNTstRepNo))t5ECNTstRepNoDup=nlsStrDup(t5ECNTstRepNo);
								if (t5ECNTstRepNoDup)
								{
									   t5ECNTstRepNoDup=low_strssra(t5ECNTstRepNoDup,"\n",";");
								}
                               	if (t5ECNTstRepNoDup)
								 {
									   t5ECNTstRepNoDup=low_strssra(t5ECNTstRepNoDup,"^",";");
								}
							    if(!nlsIsStrNull(t5ECNTstRepNoDup))
								{
								nlsStrCat(EpaWholeStr, t5ECNTstRepNoDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EPACategory_code1Attr,&t5EPACategory_code1)) goto EXIT;
								if(!nlsIsStrNull(t5EPACategory_code1))t5EPACategory_code1Dup=nlsStrDup(t5EPACategory_code1);
								if (t5EPACategory_code1Dup)
								{
									   t5EPACategory_code1Dup=low_strssra(t5EPACategory_code1Dup,"\n",";");
								}
                               	if (t5EPACategory_code1Dup)
								 {
									   t5EPACategory_code1Dup=low_strssra(t5EPACategory_code1Dup,"^",";");
								}
                               	if (t5EPACategory_code1Dup)
								 {
									   t5EPACategory_code1Dup=low_strssra(t5EPACategory_code1Dup,"^",";");
								}
							    if(!nlsIsStrNull(t5EPACategory_code1Dup))
								{
								nlsStrCat(EpaWholeStr, t5EPACategory_code1Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EPAClosureDateAttr,&t5EPAClosureDate)) goto EXIT;
								if(!nlsIsStrNull(t5EPAClosureDate))t5EPAClosureDateDup=nlsStrDup(t5EPAClosureDate);
								if (t5EPAClosureDateDup)
								{
									   t5EPAClosureDateDup=low_strssra(t5EPAClosureDateDup,"\n",";");
								}
                               	if (t5EPAClosureDateDup)
								 {
									   t5EPAClosureDateDup=low_strssra(t5EPAClosureDateDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EPAClosureDateDup))
								{
								nlsStrCat(EpaWholeStr, t5EPAClosureDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EpaClassAttr,&t5EpaClass)) goto EXIT;
								if(!nlsIsStrNull(t5EpaClass))t5EpaClassDup=nlsStrDup(t5EpaClass);
								if (t5EpaClassDup)
								{
									   t5EpaClassDup=low_strssra(t5EpaClassDup,"\n",";");
								}
                               	if (t5EpaClassDup)
								 {
									   t5EpaClassDup=low_strssra(t5EpaClassDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EpaClassDup))
								{
								nlsStrCat(EpaWholeStr, t5EpaClassDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EpaClassDateAttr,&t5EpaClassDate)) goto EXIT;
								if(!nlsIsStrNull(t5EpaClassDate))t5EpaClassDateDup=nlsStrDup(t5EpaClassDate);
								if (t5EpaClassDateDup)
								{
									   t5EpaClassDateDup=low_strssra(t5EpaClassDateDup,"\n",";");
								}
                               	if (t5EpaClassDateDup)
								 {
									   t5EpaClassDateDup=low_strssra(t5EpaClassDateDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EpaClassDateDup))
								{
								nlsStrCat(EpaWholeStr, t5EpaClassDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EpaFolUpIndAttr,&t5EpaFolUpInd)) goto EXIT;
								if(!nlsIsStrNull(t5EpaFolUpInd))t5EpaFolUpIndDup=nlsStrDup(t5EpaFolUpInd);
								if (t5EpaFolUpIndDup)
								{
									   t5EpaFolUpIndDup=low_strssra(t5EpaFolUpIndDup,"\n",";");
								}
                               	if (t5EpaFolUpIndDup)
								 {
									   t5EpaFolUpIndDup=low_strssra(t5EpaFolUpIndDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EpaFolUpIndDup))
								{
								nlsStrCat(EpaWholeStr, t5EpaFolUpIndDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EpaSetAttr,&t5EpaSet)) goto EXIT;
								if(!nlsIsStrNull(t5EpaSet))t5EpaSetDup=nlsStrDup(t5EpaSet);
								if (t5EpaSetDup)
								{
									   t5EpaSetDup=low_strssra(t5EpaSetDup,"\n",";");
								}
                               	if (t5EpaSetDup)
								 {
									   t5EpaSetDup=low_strssra(t5EpaSetDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EpaSetDup))
								{
								nlsStrCat(EpaWholeStr, t5EpaSetDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EpaStatusAttr,&t5EpaStatus)) goto EXIT;
								if(!nlsIsStrNull(t5EpaStatus))t5EpaStatusDup=nlsStrDup(t5EpaStatus);
								if (t5EpaStatusDup)
								{
									   t5EpaStatusDup=low_strssra(t5EpaStatusDup,"\n",";");
								}
                               	if (t5EpaStatusDup)
								 {
									   t5EpaStatusDup=low_strssra(t5EpaStatusDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EpaStatusDup))
								{
								nlsStrCat(EpaWholeStr, t5EpaStatusDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EpaTskCrDateAttr,&t5EpaTskCrDate)) goto EXIT;
								if(!nlsIsStrNull(t5EpaTskCrDate))t5EpaTskCrDateDup=nlsStrDup(t5EpaTskCrDate);
 								if (t5EpaTskCrDateDup)
								{
									   t5EpaTskCrDateDup=low_strssra(t5EpaTskCrDateDup,"\n",";");
								}
                               	if (t5EpaTskCrDateDup)
								 {
									   t5EpaTskCrDateDup=low_strssra(t5EpaTskCrDateDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EpaTskCrDateDup))
								{
								nlsStrCat(EpaWholeStr, t5EpaTskCrDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EpaTypeAttr,&t5EpaType)) goto EXIT;
								if(!nlsIsStrNull(t5EpaType))t5EpaTypeDup=nlsStrDup(t5EpaType);
 								if (t5EpaTypeDup)
								{
									   t5EpaTypeDup=low_strssra(t5EpaTypeDup,"\n",";");
								}
                               	if (t5EpaTypeDup)
								 {
									   t5EpaTypeDup=low_strssra(t5EpaTypeDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EpaTypeDup))
								{
								nlsStrCat(EpaWholeStr, t5EpaTypeDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5EpaValidityAttr,&t5EpaValidity)) goto EXIT;
								if(!nlsIsStrNull(t5EpaValidity))t5EpaValidityDup=nlsStrDup(t5EpaValidity);
 								if (t5EpaValidityDup)
								{
									   t5EpaValidityDup=low_strssra(t5EpaValidityDup,"\n",";");
								}
                               	if (t5EpaValidityDup)
								 {
									   t5EpaValidityDup=low_strssra(t5EpaValidityDup,"^",";");
								}
							    if(!nlsIsStrNull(t5EpaValidityDup))
								{
								nlsStrCat(EpaWholeStr, t5EpaValidityDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5FialReqAttr,&t5FialReq)) goto EXIT;
								if(!nlsIsStrNull(t5FialReq))t5FialReqDup=nlsStrDup(t5FialReq);
 								if (t5FialReqDup)
								{
									   t5FialReqDup=low_strssra(t5FialReqDup,"\n",";");
								}
                               	if (t5FialReqDup)
								 {
									   t5FialReqDup=low_strssra(t5FialReqDup,"^",";");
								}
							    if(!nlsIsStrNull(t5FialReqDup))
								{
								nlsStrCat(EpaWholeStr, t5FialReqDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5Introduction_typeAttr,&t5Introduction_type)) goto EXIT;
								if(!nlsIsStrNull(t5Introduction_type))t5Introduction_typeDup=nlsStrDup(t5Introduction_type);
 								if (t5Introduction_typeDup)
								{
									   t5Introduction_typeDup=low_strssra(t5Introduction_typeDup,"\n",";");
								}
                               	if (t5Introduction_typeDup)
								 {
									   t5Introduction_typeDup=low_strssra(t5Introduction_typeDup,"^",";");
								}
							    if(!nlsIsStrNull(t5Introduction_typeDup))
								{
								nlsStrCat(EpaWholeStr, t5Introduction_typeDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5IsActionReqbySerAttr,&t5IsActionReqbySer)) goto EXIT;
								if(!nlsIsStrNull(t5IsActionReqbySer))t5IsActionReqbySerDup=nlsStrDup(t5IsActionReqbySer);
 								if (t5IsActionReqbySerDup)
								{
									   t5IsActionReqbySerDup=low_strssra(t5IsActionReqbySerDup,"\n",";");
								}
                               	if (t5IsActionReqbySerDup)
								 {
									   t5IsActionReqbySerDup=low_strssra(t5IsActionReqbySerDup,"^",";");
								}
							    if(!nlsIsStrNull(t5IsActionReqbySerDup))
								{
								nlsStrCat(EpaWholeStr, t5IsActionReqbySerDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5IsDumEpAttr,&t5IsDumEp)) goto EXIT;
								if(!nlsIsStrNull(t5IsDumEp))t5IsDumEpDup=nlsStrDup(t5IsDumEp);
 								if (t5IsDumEpDup)
								{
									   t5IsDumEpDup=low_strssra(t5IsDumEpDup,"\n",";");
								}
                               	if (t5IsDumEpDup)
								 {
									   t5IsDumEpDup=low_strssra(t5IsDumEpDup,"^",";");
								}
							    if(!nlsIsStrNull(t5IsDumEpDup))
								{
								nlsStrCat(EpaWholeStr, t5IsDumEpDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5IsStructEPAAttr,&t5IsStructEPA)) goto EXIT;
								if(!nlsIsStrNull(t5IsStructEPA))t5IsStructEPADup=nlsStrDup(t5IsStructEPA);
 								if (t5IsStructEPADup)
								{
									   t5IsStructEPADup=low_strssra(t5IsStructEPADup,"\n",";");
								}
                               	if (t5IsStructEPADup)
								 {
									   t5IsStructEPADup=low_strssra(t5IsStructEPADup,"^",";");
								}
							    if(!nlsIsStrNull(t5IsStructEPADup))
								{
								nlsStrCat(EpaWholeStr, t5IsStructEPADup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5IsSubAggReqAttr,&t5IsSubAggReq)) goto EXIT;
								if(!nlsIsStrNull(t5IsSubAggReq))t5IsSubAggReqDup=nlsStrDup(t5IsSubAggReq);
 								if (t5IsSubAggReqDup)
								{
									   t5IsSubAggReqDup=low_strssra(t5IsSubAggReqDup,"\n",";");
								}
                               	if (t5IsSubAggReqDup)
								 {
									   t5IsSubAggReqDup=low_strssra(t5IsSubAggReqDup,"^",";");
								}
							    if(!nlsIsStrNull(t5IsSubAggReqDup))
								{
								nlsStrCat(EpaWholeStr, t5IsSubAggReqDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5IsToolReqAttr,&t5IsToolReq)) goto EXIT;
								if(!nlsIsStrNull(t5IsToolReq))t5IsToolReqDup=nlsStrDup(t5IsToolReq);
 								if (t5IsToolReqDup)
								{
									   t5IsToolReqDup=low_strssra(t5IsToolReqDup,"\n",";");
								}
                               	if (t5IsToolReqDup)
								 {
									   t5IsToolReqDup=low_strssra(t5IsToolReqDup,"^",";");
								}
							    if(!nlsIsStrNull(t5IsToolReqDup))
								{
								nlsStrCat(EpaWholeStr, t5IsToolReqDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5IsTryOutReqAttr,&t5IsTryOutReq)) goto EXIT;
								if(!nlsIsStrNull(t5IsTryOutReq))t5IsTryOutReqDup=nlsStrDup(t5IsTryOutReq);
 								if (t5IsTryOutReqDup)
								{
									   t5IsTryOutReqDup=low_strssra(t5IsTryOutReqDup,"\n",";");
								}
                               	if (t5IsTryOutReqDup)
								 {
									   t5IsTryOutReqDup=low_strssra(t5IsTryOutReqDup,"^",";");
								}
							    if(!nlsIsStrNull(t5IsTryOutReqDup))
								{
								nlsStrCat(EpaWholeStr, t5IsTryOutReqDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5LogisticsRemarksAttr,&t5LogisticsRemarks)) goto EXIT;
								if(!nlsIsStrNull(t5LogisticsRemarks))t5LogisticsRemarksDup=nlsStrDup(t5LogisticsRemarks);
 								if (t5LogisticsRemarksDup)
								{
									   t5LogisticsRemarksDup=low_strssra(t5LogisticsRemarksDup,"\n",";");
								}
                               	if (t5LogisticsRemarksDup)
								 {
									   t5LogisticsRemarksDup=low_strssra(t5LogisticsRemarksDup,"^",";");
								}
							    if(!nlsIsStrNull(t5LogisticsRemarksDup))
								{
								nlsStrCat(EpaWholeStr, t5LogisticsRemarksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5LstPODateAttr,&t5LstPODate)) goto EXIT;
								if(!nlsIsStrNull(t5LstPODate))t5LstPODateDup=nlsStrDup(t5LstPODate);
 								if (t5LstPODateDup)
								{
									   t5LstPODateDup=low_strssra(t5LstPODateDup,"\n",";");
								}
                               	if (t5LstPODateDup)
								 {
									   t5LstPODateDup=low_strssra(t5LstPODateDup,"^",";");
								}
							    if(!nlsIsStrNull(t5LstPODateDup))
								{
								nlsStrCat(EpaWholeStr, t5LstPODateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5LstSmplDateAttr,&t5LstSmplDate)) goto EXIT;
								if(!nlsIsStrNull(t5LstSmplDate))t5LstSmplDateDup=nlsStrDup(t5LstSmplDate);
 								if (t5LstSmplDateDup)
								{
									   t5LstSmplDateDup=low_strssra(t5LstSmplDateDup,"\n",";");
								}
                               	if (t5LstSmplDateDup)
								 {
									   t5LstSmplDateDup=low_strssra(t5LstSmplDateDup,"^",";");
								}
							    if(!nlsIsStrNull(t5LstSmplDateDup))
								{
								nlsStrCat(EpaWholeStr, t5LstSmplDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5MBPARemarksAttr,&t5MBPARemarks)) goto EXIT;
								if(!nlsIsStrNull(t5MBPARemarks))t5MBPARemarksDup=nlsStrDup(t5MBPARemarks);
 								if (t5MBPARemarksDup)
								{
									   t5MBPARemarksDup=low_strssra(t5MBPARemarksDup,"\n",";");
								}
                               	if (t5MBPARemarksDup)
								 {
									   t5MBPARemarksDup=low_strssra(t5MBPARemarksDup,"^",";");
								}
							    if(!nlsIsStrNull(t5MBPARemarksDup))
								{
								nlsStrCat(EpaWholeStr, t5MBPARemarksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5MEPAQADateAttr,&t5MEPAQADate)) goto EXIT;
								if(!nlsIsStrNull(t5MEPAQADate))t5MEPAQADateDup=nlsStrDup(t5MEPAQADate);
 								if (t5MEPAQADateDup)
								{
									   t5MEPAQADateDup=low_strssra(t5MEPAQADateDup,"\n",";");
								}
                               	if (t5MEPAQADateDup)
								 {
									   t5MEPAQADateDup=low_strssra(t5MEPAQADateDup,"^",";");
								}
							    if(!nlsIsStrNull(t5MEPAQADateDup))
								{
								nlsStrCat(EpaWholeStr, t5MEPAQADateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5MailAlertAttr,&t5MailAlert)) goto EXIT;
								if(!nlsIsStrNull(t5MailAlert))t5MailAlertDup=nlsStrDup(t5MailAlert);
 								if (t5MailAlertDup)
								{
									   t5MailAlertDup=low_strssra(t5MailAlertDup,"\n",";");
								}
                               	if (t5MailAlertDup)
								 {
									   t5MailAlertDup=low_strssra(t5MailAlertDup,"^",";");
								}
							    if(!nlsIsStrNull(t5MailAlertDup))
								{
								nlsStrCat(EpaWholeStr, t5MailAlertDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5MbpaDescAttr,&t5MbpaDesc)) goto EXIT;
								if(!nlsIsStrNull(t5MbpaDesc))t5MbpaDescDup=nlsStrDup(t5MbpaDesc);
 								if (t5MbpaDescDup)
								{
									   t5MbpaDescDup=low_strssra(t5MbpaDescDup,"\n",";");
								}
                               	if (t5MbpaDescDup)
								 {
									   t5MbpaDescDup=low_strssra(t5MbpaDescDup,"^",";");
								}
							    if(!nlsIsStrNull(t5MbpaDescDup))
								{
								nlsStrCat(EpaWholeStr, t5MbpaDescDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5MbpaNoAttr,&t5MbpaNo)) goto EXIT;
								if(!nlsIsStrNull(t5MbpaNo))t5MbpaNoDup=nlsStrDup(t5MbpaNo);
 								if (t5MbpaNoDup)
								{
									   t5MbpaNoDup=low_strssra(t5MbpaNoDup,"\n",";");
								}
                               	if (t5MbpaNoDup)
								 {
									   t5MbpaNoDup=low_strssra(t5MbpaNoDup,"^",";");
								}
							    if(!nlsIsStrNull(t5MbpaNoDup))
								{
								nlsStrCat(EpaWholeStr, t5MbpaNoDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5MepaReasonAttr,&t5MepaReason)) goto EXIT;
								if(!nlsIsStrNull(t5MepaReason))t5MepaReasonDup=nlsStrDup(t5MepaReason);
 								if (t5MepaReasonDup)
								{
									   t5MepaReasonDup=low_strssra(t5MepaReasonDup,"\n",";");
								}
                               	if (t5MepaReasonDup)
								 {
									   t5MepaReasonDup=low_strssra(t5MepaReasonDup,"^",";");
								}
							    if(!nlsIsStrNull(t5MepaReasonDup))
								{
								nlsStrCat(EpaWholeStr, t5MepaReasonDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ModOfIntrodAttr,&t5ModOfIntrod)) goto EXIT;
								if(!nlsIsStrNull(t5ModOfIntrod))t5ModOfIntrodDup=nlsStrDup(t5ModOfIntrod);
 								if (t5ModOfIntrodDup)
								{
									   t5ModOfIntrodDup=low_strssra(t5ModOfIntrodDup,"\n",";");
								}
                               	if (t5ModOfIntrodDup)
								 {
									   t5ModOfIntrodDup=low_strssra(t5ModOfIntrodDup,"^",";");
								}
							    if(!nlsIsStrNull(t5ModOfIntrodDup))
								{
								nlsStrCat(EpaWholeStr, t5ModOfIntrodDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

                                t5ModelList = NULL;
                                t5ModelListDup = NULL;
							    t5ModelListDup =nlsStrAlloc(100);
								nlsStrCpy(t5ModelListDup,"");
    							if(dstat = objListGetList(EpaObj,t5ChasisTypeListAttr,&EpaModelList)) goto  EXIT ;
								if(setSize(EpaModelList)>0)
								{
									for(i=0;i<setSize(EpaModelList);i++)
									{
										if(t5ModelList) t5ModelList=NULL;
			                         	if(dstat=objListGet(EpaObj,t5MbpaModelAttr,i,&t5ModelList)) goto EXIT;
										printf("\n Veh Class Selected ...%s \n",t5ModelList);
										nlsStrCat(t5ModelListDup,t5ModelList);
										nlsStrCat(t5ModelListDup,",");
									}
								}
 								if (t5ModelListDup)
								{
									   t5ModelListDup=low_strssra(t5ModelListDup,"\n",";");
								}
                               	if (t5ModelListDup)
								 {
									   t5ModelListDup=low_strssra(t5ModelListDup,"^",";");
								}
							    if(!nlsIsStrNull(t5ModelListDup))
								{
								nlsStrCat(EpaWholeStr, t5ModelListDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5POCheckStatusAttr,&t5POCheckStatus)) goto EXIT;
								if(!nlsIsStrNull(t5POCheckStatus))t5POCheckStatusDup=nlsStrDup(t5POCheckStatus);
 								if (t5POCheckStatusDup)
								{
									   t5POCheckStatusDup=low_strssra(t5POCheckStatusDup,"\n",";");
								}
                               	if (t5POCheckStatusDup)
								 {
									   t5POCheckStatusDup=low_strssra(t5POCheckStatusDup,"^",";");
								}
							    if(!nlsIsStrNull(t5POCheckStatusDup))
								{
								nlsStrCat(EpaWholeStr, t5POCheckStatusDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5POLockDateAttr,&t5POLockDate)) goto EXIT;
								if(!nlsIsStrNull(t5POLockDate))t5POLockDateDup=nlsStrDup(t5POLockDate);
 								if (t5POLockDateDup)
								{
									   t5POLockDateDup=low_strssra(t5POLockDateDup,"\n",";");
								}
                               	if (t5POLockDateDup)
								 {
									   t5POLockDateDup=low_strssra(t5POLockDateDup,"^",";");
								}
							    if(!nlsIsStrNull(t5POLockDateDup))
								{
								nlsStrCat(EpaWholeStr, t5POLockDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5PPMRemarksAttr,&t5PPMRemarks)) goto EXIT;
								if(!nlsIsStrNull(t5PPMRemarks))t5PPMRemarksDup=nlsStrDup(t5PPMRemarks);
 								if (t5PPMRemarksDup)
								{
									   t5PPMRemarksDup=low_strssra(t5PPMRemarksDup,"\n",";");
								}
                               	if (t5PPMRemarksDup)
								 {
									   t5PPMRemarksDup=low_strssra(t5PPMRemarksDup,"^",";");
								}
							    if(!nlsIsStrNull(t5PPMRemarksDup))
								{
								nlsStrCat(EpaWholeStr, t5PPMRemarksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5PartYesNoAttr,&t5PartYesNo)) goto EXIT;
								if(!nlsIsStrNull(t5PartYesNo))t5PartYesNoDup=nlsStrDup(t5PartYesNo);
   								if (t5PartYesNoDup)
								{
									   t5PartYesNoDup=low_strssra(t5PartYesNoDup,"\n",";");
								}
                               	if (t5PartYesNoDup)
								 {
									   t5PartYesNoDup=low_strssra(t5PartYesNoDup,"^",";");
								}
							    if(!nlsIsStrNull(t5PartYesNoDup))
								{
								nlsStrCat(EpaWholeStr, t5PartYesNoDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5PartialReasonAttr,&t5PartialReason)) goto EXIT;
								if(!nlsIsStrNull(t5PartialReason))t5PartialReasonDup=nlsStrDup(t5PartialReason);
   								if (t5PartialReasonDup)
								{
									   t5PartialReasonDup=low_strssra(t5PartialReasonDup,"\n",";");
								}
                               	if (t5PartialReasonDup)
								 {
									   t5PartialReasonDup=low_strssra(t5PartialReasonDup,"^",";");
								}
							    if(!nlsIsStrNull(t5PartialReasonDup))
								{
								nlsStrCat(EpaWholeStr, t5PartialReasonDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,"t5QACRemarks",&t5QACRemarks)) goto EXIT;
								if(!nlsIsStrNull(t5QACRemarks))t5QACRemarksDup=nlsStrDup(t5QACRemarks);
								if (t5QACRemarksDup)
								{
									   t5QACRemarksDup=low_strssra(t5QACRemarksDup,"\n",";");
								}
                               	if (t5QACRemarksDup)
								 {
									   t5QACRemarksDup=low_strssra(t5QACRemarksDup,"^",";");
								}
							    if(!nlsIsStrNull(t5QACRemarksDup))
								{
								nlsStrCat(EpaWholeStr, t5QACRemarksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ReadinessDateAttr,&t5ReadinessDate)) goto EXIT;
								if(!nlsIsStrNull(t5ReadinessDate))t5ReadinessDateDup=nlsStrDup(t5ReadinessDate);
								if (t5ReadinessDateDup)
								{
									   t5ReadinessDateDup=low_strssra(t5ReadinessDateDup,"\n",";");
								}
                               	if (t5ReadinessDateDup)
								 {
									   t5ReadinessDateDup=low_strssra(t5ReadinessDateDup,"^",";");
								}
							    if(!nlsIsStrNull(t5ReadinessDateDup))
								{
								nlsStrCat(EpaWholeStr, t5ReadinessDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ReleaseNotesAttr,&t5ReleaseNotes)) goto EXIT;
								if(!nlsIsStrNull(t5ReleaseNotes))t5ReleaseNotesDup=nlsStrDup(t5ReleaseNotes);
								if (t5ReleaseNotesDup)
								{
									   t5ReleaseNotesDup=low_strssra(t5ReleaseNotesDup,"\n",";");
								}
                               	if (t5ReleaseNotesDup)
								 {
									   t5ReleaseNotesDup=low_strssra(t5ReleaseNotesDup,"^",";");
								}
							    if(!nlsIsStrNull(t5ReleaseNotesDup))
								{
								nlsStrCat(EpaWholeStr, t5ReleaseNotesDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RemarkAppModelAttr,&t5RemarkAppModel)) goto EXIT;
								if(!nlsIsStrNull(t5RemarkAppModel))t5RemarkAppModelDup=nlsStrDup(t5RemarkAppModel);
								if (t5RemarkAppModelDup)
								{
									   t5RemarkAppModelDup=low_strssra(t5RemarkAppModelDup,"\n",";");
								}
                               	if (t5RemarkAppModelDup)
								 {
									   t5RemarkAppModelDup=low_strssra(t5RemarkAppModelDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RemarkAppModelDup))
								{
								nlsStrCat(EpaWholeStr, t5RemarkAppModelDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RemarksBPAttr,&t5RemarksBP)) goto EXIT;
								if(!nlsIsStrNull(t5RemarksBP))t5RemarksBPDup=nlsStrDup(t5RemarksBP);
								if (t5RemarksBPDup)
								{
									   t5RemarksBPDup=low_strssra(t5RemarksBPDup,"\n",";");
								}
                               	if (t5RemarksBPDup)
								 {
									   t5RemarksBPDup=low_strssra(t5RemarksBPDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RemarksBPDup))
								{
								nlsStrCat(EpaWholeStr, t5RemarksBPDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RemarksEPAClosureAttr,&t5RemarksEPAClosure)) goto EXIT;
								if(!nlsIsStrNull(t5RemarksEPAClosure))t5RemarksEPAClosureDup=nlsStrDup(t5RemarksEPAClosure);
								if (t5RemarksEPAClosureDup)
								{
									   t5RemarksEPAClosureDup=low_strssra(t5RemarksEPAClosureDup,"\n",";");
								}
                               	if (t5RemarksEPAClosureDup)
								 {
									   t5RemarksEPAClosureDup=low_strssra(t5RemarksEPAClosureDup,"^",";");
								}
								if(!nlsIsStrNull(t5RemarksEPAClosureDup))
								{
								nlsStrCat(EpaWholeStr, t5RemarksEPAClosureDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RemarksMEPAQAAttr,&t5RemarksMEPAQA)) goto EXIT;
								if(!nlsIsStrNull(t5RemarksMEPAQA))t5RemarksMEPAQADup=nlsStrDup(t5RemarksMEPAQA);
								if (t5RemarksMEPAQADup)
								{
									   t5RemarksMEPAQADup=low_strssra(t5RemarksMEPAQADup,"\n",";");
								}
                               	if (t5RemarksMEPAQADup)
								 {
									   t5RemarksMEPAQADup=low_strssra(t5RemarksMEPAQADup,"^",";");
								}
						        if(!nlsIsStrNull(t5RemarksMEPAQADup))
								{
								nlsStrCat(EpaWholeStr, t5RemarksMEPAQADup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RemarksPOLockAttr,&t5RemarksPOLock)) goto EXIT;
								if(!nlsIsStrNull(t5RemarksPOLock))t5RemarksPOLockDup=nlsStrDup(t5RemarksPOLock);
								if (t5RemarksPOLockDup)
								{
									   t5RemarksPOLockDup=low_strssra(t5RemarksPOLockDup,"\n",";");
								}
                               	if (t5RemarksPOLockDup)
								 {
									   t5RemarksPOLockDup=low_strssra(t5RemarksPOLockDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RemarksPOLockDup))
								{
								nlsStrCat(EpaWholeStr, t5RemarksPOLockDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RemarksReadinessAttr,&t5RemarksReadiness)) goto EXIT;
								if(!nlsIsStrNull(t5RemarksReadiness))t5RemarksReadinessDup=nlsStrDup(t5RemarksReadiness);
								if (t5RemarksReadinessDup)
								{
									   t5RemarksReadinessDup=low_strssra(t5RemarksReadinessDup,"\n",";");
								}
                               	if (t5RemarksReadinessDup)
								 {
									   t5RemarksReadinessDup=low_strssra(t5RemarksReadinessDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RemarksReadinessDup))
								{
								nlsStrCat(EpaWholeStr, t5RemarksReadinessDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RemarksStocksAttr,&t5RemarksStocks)) goto EXIT;
								if(!nlsIsStrNull(t5RemarksStocks))t5RemarksStocksDup=nlsStrDup(t5RemarksStocks);
								if (t5RemarksStocksDup)
								{
									   t5RemarksStocksDup=low_strssra(t5RemarksStocksDup,"\n",";");
								}
                               	if (t5RemarksStocksDup)
								 {
									   t5RemarksStocksDup=low_strssra(t5RemarksStocksDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RemarksStocksDup))
								{
								nlsStrCat(EpaWholeStr, t5RemarksStocksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RemarksTryoutAttr,&t5RemarksTryout)) goto EXIT;
								if(!nlsIsStrNull(t5RemarksTryout))t5RemarksTryoutDup=nlsStrDup(t5RemarksTryout);
								if (t5RemarksTryoutDup)
								{
									   t5RemarksTryoutDup=low_strssra(t5RemarksTryoutDup,"\n",";");
								}
                               	if (t5RemarksTryoutDup)
								 {
									   t5RemarksTryoutDup=low_strssra(t5RemarksTryoutDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RemarksTryoutDup))
								{
								nlsStrCat(EpaWholeStr, t5RemarksTryoutDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,"t5RevRemarks",&t5RevRemarks)) goto EXIT;
								if(!nlsIsStrNull(t5RevRemarks))t5RevRemarksDup=nlsStrDup(t5RevRemarks);
								if (t5RevRemarksDup)
								{
									   t5RevRemarksDup=low_strssra(t5RevRemarksDup,"\n",";");
								}
                               	if (t5RevRemarksDup)
								 {
									   t5RevRemarksDup=low_strssra(t5RevRemarksDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RevRemarksDup))
								{
								nlsStrCat(EpaWholeStr, t5RevRemarksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5RjVECNoAttr,&t5RjVECNo)) goto EXIT;
								if(!nlsIsStrNull(t5RjVECNo))t5RjVECNoDup=nlsStrDup(t5RjVECNo);
								if (t5RjVECNoDup)
								{
									   t5RjVECNoDup=low_strssra(t5RjVECNoDup,"\n",";");
								}
                               	if (t5RjVECNoDup)
								 {
									   t5RjVECNoDup=low_strssra(t5RjVECNoDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RjVECNoDup))
								{
								nlsStrCat(EpaWholeStr, t5RjVECNoDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,"t5RygSts",&t5RygSts)) goto EXIT;
								if(!nlsIsStrNull(t5RygSts))t5RygStsDup=nlsStrDup(t5RygSts);
								if (t5RygStsDup)
								{
									   t5RygStsDup=low_strssra(t5RygStsDup,"\n",";");
								}
                               	if (t5RygStsDup)
								 {
									   t5RygStsDup=low_strssra(t5RygStsDup,"^",";");
								}
						        if(!nlsIsStrNull(t5RygStsDup))
								{
								nlsStrCat(EpaWholeStr, t5RygStsDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5SORSendDateAttr,&t5SORSendDate)) goto EXIT;
								if(!nlsIsStrNull(t5SORSendDate))t5SORSendDateDup=nlsStrDup(t5SORSendDate);
								if (t5SORSendDateDup)
								{
									   t5SORSendDateDup=low_strssra(t5SORSendDateDup,"\n",";");
								}
                               	if (t5SORSendDateDup)
								 {
									   t5SORSendDateDup=low_strssra(t5SORSendDateDup,"^",";");
								}
						        if(!nlsIsStrNull(t5SORSendDateDup))
								{
								nlsStrCat(EpaWholeStr, t5SORSendDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ServActionAttr,&t5ServAction)) goto EXIT;
								if(!nlsIsStrNull(t5ServAction))t5ServActionDup=nlsStrDup(t5ServAction);
								if (t5ServActionDup)
								{
									   t5ServActionDup=low_strssra(t5ServActionDup,"\n",";");
								}
                               	if (t5ServActionDup)
								 {
									   t5ServActionDup=low_strssra(t5ServActionDup,"^",";");
								}
						        if(!nlsIsStrNull(t5ServActionDup))
								{
								nlsStrCat(EpaWholeStr, t5ServActionDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5ServActionAggAttr,&t5ServActionAgg)) goto EXIT;
								if(!nlsIsStrNull(t5ServActionAgg))t5ServActionAggDup=nlsStrDup(t5ServActionAgg);
								if (t5ServActionAggDup)
								{
									   t5ServActionAggDup=low_strssra(t5ServActionAggDup,"\n",";");
								}
                               	if (t5ServActionAggDup)
								 {
									   t5ServActionAggDup=low_strssra(t5ServActionAggDup,"^",";");
								}
						        if(!nlsIsStrNull(t5ServActionAggDup))
								{
								nlsStrCat(EpaWholeStr, t5ServActionAggDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5StocksSCMDateAttr,&t5StocksSCMDate)) goto EXIT;
								if(!nlsIsStrNull(t5StocksSCMDate))t5StocksSCMDateDup=nlsStrDup(t5StocksSCMDate);
								if (t5StocksSCMDateDup)
								{
									   t5StocksSCMDateDup=low_strssra(t5StocksSCMDateDup,"\n",";");
								}
                               	if (t5StocksSCMDateDup)
								 {
									   t5StocksSCMDateDup=low_strssra(t5StocksSCMDateDup,"^",";");
								}
						        if(!nlsIsStrNull(t5StocksSCMDateDup))
								{
								nlsStrCat(EpaWholeStr, t5StocksSCMDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5TSRemarksAttr,&t5TSRemarks)) goto EXIT;
								if(!nlsIsStrNull(t5TSRemarks))t5TSRemarksDup=nlsStrDup(t5TSRemarks);
								if (t5TSRemarksDup)
								{
									   t5TSRemarksDup=low_strssra(t5TSRemarksDup,"\n",";");
								}
                               	if (t5TSRemarksDup)
								 {
									   t5TSRemarksDup=low_strssra(t5TSRemarksDup,"^",";");
								}
						        if(!nlsIsStrNull(t5TSRemarksDup))
								{
								nlsStrCat(EpaWholeStr, t5TSRemarksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5TargetDtIntroAttr,&t5TargetDtIntro)) goto EXIT;
								if(!nlsIsStrNull(t5TargetDtIntro))t5TargetDtIntroDup=nlsStrDup(t5TargetDtIntro);
								if (t5TargetDtIntroDup)
								{
									   t5TargetDtIntroDup=low_strssra(t5TargetDtIntroDup,"\n",";");
								}
                               	if (t5TargetDtIntroDup)
								 {
									   t5TargetDtIntroDup=low_strssra(t5TargetDtIntroDup,"^",";");
								}
						        if(!nlsIsStrNull(t5TargetDtIntroDup))
								{
								nlsStrCat(EpaWholeStr, t5TargetDtIntroDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5TargetDtIntroHisAttr,&t5TargetDtIntroHis)) goto EXIT;
								if(!nlsIsStrNull(t5TargetDtIntroHis))t5TargetDtIntroHisDup=nlsStrDup(t5TargetDtIntroHis);
								if (t5TargetDtIntroHisDup)
								{
									   t5TargetDtIntroHisDup=low_strssra(t5TargetDtIntroHisDup,"\n",";");
								}
                               	if (t5TargetDtIntroHisDup)
								 {
									   t5TargetDtIntroHisDup=low_strssra(t5TargetDtIntroHisDup,"^",";");
								}
								if(!nlsIsStrNull(t5TargetDtIntroHisDup))
								{
								nlsStrCat(EpaWholeStr, t5TargetDtIntroHisDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5TmpEcnNoAttr,&t5TmpEcnNo)) goto EXIT;
								if(!nlsIsStrNull(t5TmpEcnNo))t5TmpEcnNoDup=nlsStrDup(t5TmpEcnNo);
								if (t5TmpEcnNoDup)
								{
									   t5TmpEcnNoDup=low_strssra(t5TmpEcnNoDup,"\n",";");
								}
                               	if (t5TmpEcnNoDup)
								 {
									   t5TmpEcnNoDup=low_strssra(t5TmpEcnNoDup,"^",";");
								}
						        if(!nlsIsStrNull(t5TmpEcnNoDup))
								{
								nlsStrCat(EpaWholeStr, t5TmpEcnNoDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5TrgtDtIntRemAttr,&t5TrgtDtIntRem)) goto EXIT;
								if(!nlsIsStrNull(t5TrgtDtIntRem))t5TrgtDtIntRemDup=nlsStrDup(t5TrgtDtIntRem);
								if (t5TrgtDtIntRemDup)
								{
									   t5TrgtDtIntRemDup=low_strssra(t5TrgtDtIntRemDup,"\n",";");
								}
                               	if (t5TrgtDtIntRemDup)
								 {
									   t5TrgtDtIntRemDup=low_strssra(t5TrgtDtIntRemDup,"^",";");
								}
								if(!nlsIsStrNull(t5TrgtDtIntRemDup))
								{
								nlsStrCat(EpaWholeStr, t5TrgtDtIntRemDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5TryoutDateAttr,&t5TryoutDate)) goto EXIT;
								if(!nlsIsStrNull(t5TryoutDate))t5TryoutDateDup=nlsStrDup(t5TryoutDate);
								if (t5TryoutDateDup)
								{
									   t5TryoutDateDup=low_strssra(t5TryoutDateDup,"\n",";");
								}
                               	if (t5TryoutDateDup)
								 {
									   t5TryoutDateDup=low_strssra(t5TryoutDateDup,"^",";");
								}
						        if(!nlsIsStrNull(t5TryoutDateDup))
								{
								nlsStrCat(EpaWholeStr, t5TryoutDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5VDRemarksAttr,&t5VDRemarks)) goto EXIT;
								if(!nlsIsStrNull(t5VDRemarks))t5VDRemarksDup=nlsStrDup(t5VDRemarks);
								if (t5VDRemarksDup)
								{
									   t5VDRemarksDup=low_strssra(t5VDRemarksDup,"\n",";");
								}
                               	if (t5VDRemarksDup)
								 {
									   t5VDRemarksDup=low_strssra(t5VDRemarksDup,"^",";");
								}
                               	if (t5VDRemarksDup)
								 {
									   t5VDRemarksDup=low_strssra(t5VDRemarksDup,"^",";");
								}
								if(!nlsIsStrNull(t5VDRemarksDup))
								{
								nlsStrCat(EpaWholeStr, t5VDRemarksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5VQARemarksAttr,&t5VQARemarks)) goto EXIT;
								if(!nlsIsStrNull(t5VQARemarks))t5VQARemarksDup=nlsStrDup(t5VQARemarks);
								if (t5VQARemarksDup)
								{
									   t5VQARemarksDup=low_strssra(t5VQARemarksDup,"\n",";");
								}
                               	if (t5VQARemarksDup)
								 {
									   t5VQARemarksDup=low_strssra(t5VQARemarksDup,"^",";");
								}
								if(!nlsIsStrNull(t5VQARemarksDup))
								{
								nlsStrCat(EpaWholeStr, t5VQARemarksDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5tcfAggregateAttr,&t5tcfAggregate)) goto EXIT;
								if(!nlsIsStrNull(t5tcfAggregate))t5tcfAggregateDup=nlsStrDup(t5tcfAggregate);
								if (t5tcfAggregateDup)
								{
									   t5tcfAggregateDup=low_strssra(t5tcfAggregateDup,"\n",";");
								}
                               	if (t5tcfAggregateDup)
								 {
									   t5tcfAggregateDup=low_strssra(t5tcfAggregateDup,"^",";");
								}
								if(!nlsIsStrNull(t5tcfAggregateDup))
								{
								nlsStrCat(EpaWholeStr, t5tcfAggregateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,t5tcfAggregate1Attr,&t5tcfAggregate1)) goto EXIT;
								if(!nlsIsStrNull(t5tcfAggregate1))t5tcfAggregate1Dup=nlsStrDup(t5tcfAggregate1);
								if (t5tcfAggregate1Dup)
								{
									   t5tcfAggregate1Dup=low_strssra(t5tcfAggregate1Dup,"\n",";");
								}
                               	if (t5tcfAggregate1Dup)
								 {
									   t5tcfAggregate1Dup=low_strssra(t5tcfAggregate1Dup,"^",";");
								}
								if(!nlsIsStrNull(t5tcfAggregate1Dup))
								{
								nlsStrCat(EpaWholeStr, t5tcfAggregate1Dup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,tentativeDateLogisticAttr,&tentativeDateLogistic)) goto EXIT;
								if(!nlsIsStrNull(tentativeDateLogistic))tentativeDateLogisticDup=nlsStrDup(tentativeDateLogistic);
								if (tentativeDateLogisticDup)
								{
									   tentativeDateLogisticDup=low_strssra(tentativeDateLogisticDup,"\n",";");
								}
                               	if (tentativeDateLogisticDup)
								 {
									   tentativeDateLogisticDup=low_strssra(tentativeDateLogisticDup,"^",";");
								}
								if(!nlsIsStrNull(tentativeDateLogisticDup))
								{
								nlsStrCat(EpaWholeStr, tentativeDateLogisticDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,tentativeDateSCMAttr,&tentativeDateSCM)) goto EXIT;
								if(!nlsIsStrNull(tentativeDateSCM))tentativeDateSCMDup=nlsStrDup(tentativeDateSCM);
								if (tentativeDateSCMDup)
								{
									   tentativeDateSCMDup=low_strssra(tentativeDateSCMDup,"\n",";");
								}
                               	if (tentativeDateSCMDup)
								 {
									   tentativeDateSCMDup=low_strssra(tentativeDateSCMDup,"^",";");
								}
								if(!nlsIsStrNull(tentativeDateSCMDup))
								{
								nlsStrCat(EpaWholeStr, tentativeDateSCMDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,tx0AddresseeAttr,&tx0Addressee)) goto EXIT;
								if(!nlsIsStrNull(tx0Addressee))tx0AddresseeDup=nlsStrDup(tx0Addressee);
								if (tx0AddresseeDup)
								{
									   tx0AddresseeDup=low_strssra(tx0AddresseeDup,"\n",";");
								}
                               	if (tx0AddresseeDup)
								 {
									   tx0AddresseeDup=low_strssra(tx0AddresseeDup,"^",";");
								}
								if(!nlsIsStrNull(tx0AddresseeDup))
								{
								nlsStrCat(EpaWholeStr, tx0AddresseeDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,tx0DateReceivedAttr,&tx0DateReceived)) goto EXIT;
								if(!nlsIsStrNull(tx0DateReceived))tx0DateReceivedDup=nlsStrDup(tx0DateReceived);
								if (tx0DateReceivedDup)
								{
									   tx0DateReceivedDup=low_strssra(tx0DateReceivedDup,"\n",";");
								}
                               	if (tx0DateReceivedDup)
								 {
									   tx0DateReceivedDup=low_strssra(tx0DateReceivedDup,"^",";");
								}
								if(!nlsIsStrNull(tx0DateReceivedDup))
								{
								nlsStrCat(EpaWholeStr, tx0DateReceivedDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								if(dstat = objGetAttribute(EpaObj,tx0DocCreateDateAttr,&tx0DocCreateDate)) goto EXIT;
								if(!nlsIsStrNull(tx0DocCreateDate))tx0DocCreateDateDup=nlsStrDup(tx0DocCreateDate);
								if (tx0DocCreateDateDup)
								{
									   tx0DocCreateDateDup=low_strssra(tx0DocCreateDateDup,"\n",";");
								}
                               	if (tx0DocCreateDateDup)
								 {
									   tx0DocCreateDateDup=low_strssra(tx0DocCreateDateDup,"^",";");
								}
								if(!nlsIsStrNull(tx0DocCreateDateDup))
								{
								nlsStrCat(EpaWholeStr, tx0DocCreateDateDup);
                                nlsStrCat(EpaWholeStr, "^");
								}
								else
								{
								nlsStrCat(EpaWholeStr, " ");
                                nlsStrCat(EpaWholeStr, "^");
								}

								printf("\n EpaWholeStr [%s] \n",EpaWholeStr);fflush(stdout);
								fprintf(fpepa," %s \n", EpaWholeStr);fflush(stdout);

							 }
                             fclose(fpepa);
                             fclose(fprel);
							}

		dstat = clReleaseServers ();
		CLEANUP:


			if(ERCDMLObjs)objDisposeAll(ERCDMLObjs);

		   clLogout ();
		   clTerminate ();

		EXIT:
		   if (dstat != OKAY)
		      uiShowFatalError (dstat, WHERE);
		return (dstat);
}
;

