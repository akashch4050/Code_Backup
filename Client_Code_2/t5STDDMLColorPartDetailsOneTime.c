/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File				:   TmetcGetPartListForAquilla.c
*  Author			:   Deepti Meshram
*  Module			:   TCUA Downloader.
*  Purpose			:   To get TC Part List for Aquilla Prject 5442
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
char* subString(char*,int,int);

int dayofweek(int d, int m, int y) 
{ 
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 }; 
	y -= m < 3; 
	return ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7; 
}

ObjectPtr setCnxtObjBOMDIff(ObjectPtr thisObj,string inputViewName,string configCtxt,int *mfail)
{
		char* mod_name="setCnxtObjBOMDIff";

		ObjectPtr       genContextObjOP		= NULL;
		ObjectPtr       contextObjOP			= NULL;
		status          dstat				= OKAY;

		printf("\n****Calling setCnxtObjBOMDIff****,%s,%s\n",configCtxt,inputViewName);fflush(stdout);

		  t5CheckMfail(SetUpContext(objClass(thisObj),thisObj,&genContextObjOP,mfail));
		 t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass,genContextObjOP,&contextObjOP, mfail));
		 t5CheckDstat(objSetAttribute(contextObjOP,ExpandOnRevisionAttr,"PscLastRev"));
		 //t5CheckDstat(objSetAttribute(contextObjOP,"NavigateViewBitPos","0"));
		 //t5CheckDstat(objSetAttribute(contextObjOP,"CfgItemId","GlobalCtxt"));
		 t5CheckDstat(objSetAttribute(contextObjOP,"CfgItemId",configCtxt));
		 //t5CheckDstat(objSetAttribute(contextObjOP,NavigateViewNetworkAttr,"EAS"));
		 //t5CheckDstat(objSetAttribute(contextObjOP,NavigateViewNameAttr,inputViewName));
		 t5CheckDstat(SetNavigateViewPref(contextObjOP,TRUE,"EAS",inputViewName,mfail)) ;
		 t5CheckDstat(objSetAttribute(contextObjOP,PsmExpIncludeZeroQtyAttr,"+"));
		 t5CheckDstat(objSetAttribute(contextObjOP,PsmExpDisableCfgCtxAttr,"-"));

		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsSTDRlzd","+"));
		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsSTDWrkg","+"));
		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsAplRlzd","+"));
		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsAPLWrkg","+"));
		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsErcRlzd","+"));
		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsReview","-"));
		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsWorking","-"));
		 t5CheckDstat(objSetObject(genContextObjOP,ConfigCtxtBlobAttr,contextObjOP));
		 return genContextObjOP;

		CLEANUP:
			printf("\nIn CLEANUP setCnxtObjBOMDIff"); fflush(stdout);
		EXIT:
			printf("\nIn CLEANUP and EXIT of setCnxtObjBOMDIff"); fflush(stdout);
			if (dstat != OKAY)  uiShowFatalError( dstat, WHERE );
			return genContextObjOP;

}


//FUNCTION TO GET APL PART INFO WHICH ARE REQUIRED FOR PART CREATION IN TCUA
status GetTCPartInfoForAPL(ObjectPtr TCPartObjP,string PartOrgID,string csIndicator,string StoreLocation,string configCtxt,FILE* fp_DMLPartInfo,integer* mfail )
{
	

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
		
	ObjectPtr		genContextObj	= NULL;
	ObjectPtr		contextObj		= NULL;
	
	SetOfObjects	stdExpObj		= NULL;
	SetOfObjects	aplExpObj		= NULL;
	SetOfObjects	stdrelObjSet	= NULL;
	SetOfObjects	aplrelObjSet	= NULL;
    SetOfObjects	soTaskObjs	= NULL;
	SetOfObjects	CurrEPAObjs	= NULL;
	SetOfObjects	EPAObjSo	= NULL;

	int stdsiRlzDMLFnd=0;
	int aplRlzDMLFnd=0;
	int stdsiWrkDMLFnd=0;
	int aplWrkDMLFnd=0;
	int mepacnt=0;
	int GotEPASamePltFlg=0;
	int GotEPASamePltLCSRlzd=0;
	int dmlCnt=0;
	string taskNoLCS=NULL;
	string		taskNoLCSDup=NULL;
	string taskNoDup=NULL;
	string taskNo=NULL;
	string GlblPlantName = NULL;
	string myPlantDup = NULL;
	string EpaPlantNameDup = NULL;
	string EpaPlantName = NULL;
	string			EPAStat=NULL;
	string			EPAStatDup=NULL;




	t5MethodInit("GetTCPartInfoForAPL");

	printf("\n Calling GetTCPartInfoForAPL......");fflush(stdout);
	printf("\n PartOrgID %s......StoreLocation %s,csIndicator %s,configCtxt %s",PartOrgID,StoreLocation,csIndicator,configCtxt);fflush(stdout);

	if((nlsStrCmp(PartOrgID,"ERC")==0) ||(nlsStrCmp(PartOrgID,"APLPUNE")==0))
	{
			
		if(dstat=objGetAttribute(TCPartObjP,PartNumberAttr,&t5PartNumber))goto EXIT;
		if(!nlsIsStrNull(t5PartNumber))t5PartNumberDup=nlsStrDup(t5PartNumber);

		if(dstat=objGetAttribute(TCPartObjP,RevisionAttr,&t5PartNumberRev))goto EXIT;
		if(!nlsIsStrNull(t5PartNumberRev))t5PartNumberRevDup=nlsStrDup(t5PartNumberRev);

		if(dstat=objGetAttribute(TCPartObjP,SequenceAttr,&t5PartNumberSeq))goto EXIT;
		if(!nlsIsStrNull(t5PartNumberSeq))t5PartNumberSeqDup=nlsStrDup(t5PartNumberSeq);

		
		if(dstat =  ExpandObject5(CmRsltsClass,TCPartObjP,"BusItemIsResultOfCMPrdPln",SC_SCOPE_OF_SESSION,&soTaskObjs,mfail));
		if(setSize(soTaskObjs)>0)
		{
			stdsiRlzDMLFnd=0;
			aplRlzDMLFnd=0;
			stdsiWrkDMLFnd=0;
			aplWrkDMLFnd=0;
			for(dmlCnt=0;dmlCnt<setSize(soTaskObjs);dmlCnt++)
			{
				taskNoDup=NULL;
				taskNo=NULL;

				taskNoLCS=NULL;
				taskNoLCSDup=NULL;				

				if(dstat = objGetAttribute(setGet(soTaskObjs,dmlCnt),WbsIDAttr,&taskNo)) goto EXIT;
				if (!nlsIsStrNull(taskNo)) taskNoDup = nlsStrDup(taskNo);
				printf("\n taskNoDup::%s \n", taskNoDup);fflush(stdout);

				if(nlsStrLen(taskNoDup)>13)
				{
				
					if (!nlsIsStrNull(GlblPlantName)) nlsStrFree(GlblPlantName);
					GlblPlantName	=nlsStrAlloc(10);
					if (dstat=t5GetTaskDmlLastValue(taskNoDup,GlblPlantName,mfail)) goto EXIT;
					if(!nlsIsStrNull(GlblPlantName)) myPlantDup = nlsStrDup(GlblPlantName);
					printf("\n Plant found from Task Name is: %s",myPlantDup);fflush(stdout);

					if(!nlsIsStrNull(myPlantDup))
					{
						if((nlsStrCmp(myPlantDup,"APL")==0) || (nlsStrCmp(myPlantDup,"APLPUNE")==0))
						{
							if(dstat = objGetAttribute(setGet(soTaskObjs,dmlCnt),LifeCycleStateAttr,&taskNoLCS)) goto EXIT;
							if (!nlsIsStrNull(taskNoLCS)) taskNoLCSDup = nlsStrDup(taskNoLCS);
							printf("\n taskNoLCSDup::%s \n", taskNoLCSDup);fflush(stdout);
							
							if(nlsStrCmp(taskNoLCSDup,"LcsSTDRlzd")==0)
							{
								stdsiRlzDMLFnd++;
							}

							if(nlsStrCmp(taskNoLCSDup,"LcsSTDWrkg")==0)
							{
								stdsiWrkDMLFnd++;
							}
							if(nlsStrCmp(taskNoLCSDup,"LcsAplRlzd")==0)
							{
								aplRlzDMLFnd++;
							}

							if(nlsStrCmp(taskNoLCSDup,"LcsAPLWrkg")==0)
							{
								aplWrkDMLFnd++;
							}
						
						}
					
					
					}
				}

			
			}

			CurrEPAObjs=NULL;
			GotEPASamePltFlg=0;
			GotEPASamePltLCSRlzd=0;
			if(stdsiRlzDMLFnd>0)
			{	
				
				if(dstat = ExpandObject4("t5MepaRe",TCPartObjP,"CmAssemblyContainedByMepa",&CurrEPAObjs,mfail)) goto EXIT;
				printf("\n setSize(CurrEPAObjs):  %d \n",setSize(CurrEPAObjs));fflush(stdout);
				if (setSize(CurrEPAObjs)>0)
				{
					
					for(mepacnt=0; mepacnt<setSize(CurrEPAObjs); mepacnt++)
					{
						EPAObjSo=NULL;
						if(dstat = ExpandObject5(CmPlRvRvClass,low_set_get(CurrEPAObjs,mepacnt),"CMPlanRollsUpInTo",SC_SCOPE_OF_SESSION,&EPAObjSo,mfail)) goto CLEANUP;
						printf("\n setSize(EPAObjSo):  %d \n",setSize(EPAObjSo));fflush(stdout);

						if (setSize(EPAObjSo)>0)
						{
						
							EpaPlantNameDup=NULL;
							EpaPlantName=NULL;

							EPAStat=NULL;
							EPAStatDup=NULL;

							if( dstat = objGetAttribute(low_set_get(EPAObjSo,0),"EpaPlantA",&EpaPlantName)) goto CLEANUP;
							if(!nlsIsStrNull(EpaPlantName)) EpaPlantNameDup = nlsStrDup(EpaPlantName);
							printf("\n ******EpaPlantNameDup: [%s]\n ",EpaPlantNameDup);fflush(stdout);

							if(!nlsStrCmp(EpaPlantNameDup,"P"))
							{
								if( dstat = objGetAttribute(low_set_get(EPAObjSo,0),t5EpaStatusAttr,&EPAStat)) goto CLEANUP;
								if(!nlsIsStrNull(EPAStat)) EPAStatDup = nlsStrDup(EPAStat);
								printf("\n EPA Number EPAStatDup : %s",EPAStatDup);fflush(stdout);
							
								GotEPASamePltFlg++;
								if(!nlsIsStrNull(EPAStatDup)) 
								{
									if(!nlsStrCmp(EPAStatDup,"F"))
									{
										GotEPASamePltLCSRlzd++;
									
									}
									
								}
							
							
							
							}
						
						}
					}
					if(GotEPASamePltLCSRlzd>0)
					{
						t5PartNumberLCSDup=	nlsStrDup("LcsSTDRlzd");
					}
					else if((GotEPASamePltFlg >0) && (GotEPASamePltLCSRlzd==0))
					{
						t5PartNumberLCSDup=	nlsStrDup("LcsSTDWrkg");
					}
					else
					{
						t5PartNumberLCSDup=	nlsStrDup("LcsSTDRlzd");
					}
				}
				else
				{
					t5PartNumberLCSDup=	nlsStrDup("LcsSTDRlzd");
				}	
						
				
				
			}
			else if(stdsiWrkDMLFnd>0)
			{	
				t5PartNumberLCSDup=	nlsStrDup("LcsSTDWrkg");
			}
			else if(aplRlzDMLFnd>0)
			{	
				t5PartNumberLCSDup=	nlsStrDup("LcsAplRlzd");
			}
			else if(aplWrkDMLFnd>0)
			{	
				t5PartNumberLCSDup=	nlsStrDup("LcsAPLWrkg");
			}
			else if((aplRlzDMLFnd==0) && (stdsiRlzDMLFnd==0) && (stdsiWrkDMLFnd==0) && (aplWrkDMLFnd==0))
			{	
				if(dstat=objGetAttribute(TCPartObjP,LifeCycleStateAttr,&t5PartNumberLCS))goto EXIT;
				if(!nlsIsStrNull(t5PartNumberLCS))t5PartNumberLCSDup=nlsStrDup(t5PartNumberLCS);
			}
		}
		else
		{
			if(dstat=objGetAttribute(TCPartObjP,LifeCycleStateAttr,&t5PartNumberLCS))goto EXIT;
			if(!nlsIsStrNull(t5PartNumberLCS))t5PartNumberLCSDup=nlsStrDup(t5PartNumberLCS);
		}

		if(dstat=objGetAttribute(TCPartObjP,OrganizationIDAttr,&t5PartOrgID))goto EXIT;
		if(!nlsIsStrNull(t5PartOrgID))t5PartOrgIDDup=nlsStrDup(t5PartOrgID);

		printf("\n Inside ERC PartDetials :%s,%s,%s,%s,%s",t5PartNumberDup,t5PartNumberRevDup,t5PartNumberSeqDup,t5PartNumberLCSDup,t5PartOrgIDDup);fflush(stdout);


		t5CheckMfail(IntSetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObj,mfail));
		t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass, genContextObj, &contextObj, mfail));
		//t5CheckDstat(objSetAttribute(contextObj,CfgItemIdAttr,"GlobalCtxt"));
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
		
		t5CheckMfail(IntSetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObj,mfail));
		t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass, genContextObj, &contextObj, mfail));
		//t5CheckDstat(objSetAttribute(contextObj,CfgItemIdAttr,"GlobalCtxt"));
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
			

		fprintf(fp_DMLPartInfo,"0");							//(level);
		fprintf(fp_DMLPartInfo,"^");
		fprintf(fp_DMLPartInfo,"%s",t5PartNumberDup);			//PARTNUMBER;								
		fprintf(fp_DMLPartInfo,"^");
		fprintf(fp_DMLPartInfo,"%s",t5PartNumberRevDup);		//PARTNUMBER REV;
		fprintf(fp_DMLPartInfo,"^");
		fprintf(fp_DMLPartInfo,"%s",t5PartNumberSeqDup);		//PARTNUMBER SEQ;
		fprintf(fp_DMLPartInfo,"^");

		if(!nlsIsStrNull(t5PartOrgIDDup))		//PARTNUMBER ORGID ;
		{
			fprintf(fp_DMLPartInfo,"%s",t5PartOrgIDDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo," ^");
		}

		if(!nlsIsStrNull(t5PartNumberLCSDup))		//PARTNUMBER LCS ;
		{
			fprintf(fp_DMLPartInfo,"%s",t5PartNumberLCSDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo," ^");
		}

		if(!nlsIsStrNull(APLReveffDateFromDup))			//PARTNUMBER STD EFF FROM DATE;
		{
			fprintf(fp_DMLPartInfo,"%s",APLReveffDateFromDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo," ^");
		}

		if(!nlsIsStrNull(APLReveffDateToDup))			//PARTNUMBER APL EFF TO DATE;
		{
			fprintf(fp_DMLPartInfo,"%s",APLReveffDateToDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo," ^");
		}

		
		if(!nlsIsStrNull(stdReveffDateFromDup))			//PARTNUMBER STD EFF FROM DATE;
		{
			fprintf(fp_DMLPartInfo,"%s",stdReveffDateFromDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo," ^");
		}

		if(!nlsIsStrNull(stdReveffDateToDup))		//PARTNUMBER STD EFF TO DATE;
		{
			fprintf(fp_DMLPartInfo,"%s",stdReveffDateToDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo," ^");
		}

		fprintf(fp_DMLPartInfo,"\n");
	
	}
	
	
	
	
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
	string  ProjectName=NULL;
	string  InpDMLNumberS=NULL;
	string PartNumberVal=NULL;
	string PartNumberValDup=NULL;
	string PartNumberRev=NULL;
	string PartNumberRevDup=NULL;
	string PartNumberSeq=NULL;
	string PartNumberSeqDup=NULL;
	string		docobjst	=	NULL;
	string		taskNumber=NULL;
	string		taskNumberDup=NULL;
	string apldmlLCSState=NULL;
	string		apldmlLCSStateDup=NULL;
	string aplDmlNo=NULL;
	string	aplDmlNoDup=NULL;
	string	PartOrgID=NULL;
	string	PartOrgIDDup=NULL;
	string	PlantName=NULL;
	string	DmlNoRule=NULL;
	string	sPlntNmeDup1=NULL;
	string	sys_date=NULL;
	string	integerStr=NULL;
	string	OutFilePlantDMLList=NULL;
	string	OutFilePlantDMLTaskPartInfo=NULL;
	string taskLifeCycle=NULL;
	string taskLifeCycleDup=NULL;
	string aplDmlNoRlz=NULL;
	string aplDmlNoRlzDup=NULL;
	string aplDmltaskRlzDt=NULL;
	string aplDmltaskRlzDtDup=NULL;
	string aplDmlNoSTDSIRlz=NULL;
	string	aplDmlNoSTDSIRlzDup=NULL;
	string	aplDmlTaskSTDSIRlzDt=NULL;
	string	aplDmlTaskSTDSIRlzDtDup=NULL;
	string	OutFilePlantDMLPartInfo=NULL;
	string	OutFilePlantDMLPartCADInfo=NULL;
	string PartDrwInd=NULL;
	string	PartDrwIndDup=NULL;
	string	OutFilePlantDMLErrorLog=NULL;
	string	OutFilePlantDMLBomList=NULL;
	string	OutFilePlantDMLOPCSInfo=NULL;
	string aplDmlNoEcnType=NULL;
	string	aplDmlNoEcnTypeDup=NULL;
	string aplDMLObjsClass=NULL;
	string			aplDMLObjsClassDup=NULL;
	string PartDegnGrp=NULL;
	string PartDegnGrpDup=NULL;
	string csIndicator=NULL;
	string StoreLocation=NULL;
	string configCtxt=NULL;
	string PartObid=NULL;
	string PartObidDup=NULL;
	string GenOptionalCS=NULL;
	string taskDesgGrpDup=NULL;
	string taskDesgGrp=NULL;
	string aplAMDmlCreator=NULL;
	string aplAMDmlCreatorDup=NULL;
	string aplAMDmlSynp=NULL;
	string aplAMDmlSynpDup=NULL;
	string aplAMDmlProjCode=NULL;
	string aplAMDmlProjCodeDup=NULL;
	string aplAMDmlDriverVC=NULL;
	string aplAMDmlDriverVCDup=NULL;
	string aplAMDmlDesc=NULL;
	string aplAMDmlDescDup=NULL;
	string aplAMLastUpBy=NULL;
	string aplAMLastUpByDup=NULL;
	string aplAMLastUpDt=NULL;
	string aplAMLastUpDtDup=NULL;
	string aplAMDRStatus=NULL;
	string	aplAMDRStatusDup=NULL;
	string DMLObjsClass=NULL;
	string DMLObjsClassDup=NULL;
	//string DateFolder=NULL;
	string workingPath=NULL;
	string PartColourInd=NULL;
	string PartColourIndDup=NULL;
						

	//SetOfObjects  DMLResultSO = NULL ;
	SetOfObjects  DMLTaskResultSO = NULL ;
	//SetOfObjects  SetOfTasksObjs = NULL ;
	SetOfObjects  setOfAssmblyObjs = NULL ;
	SetOfObjects  ErcDMLObjs = NULL ;
	SetOfObjects  APLDMLObjs = NULL ;
	ObjectPtr	DmlObjPtr = NULL ;
	ObjectPtr	APLDmlObjPtr = NULL ;
	ObjectPtr	TaskObjPtr = NULL ;
	ObjectPtr	TCPartObjP = NULL ;
	SqlPtr	InputDMLSqlPtr = NULL ;

	string			curr_date					= NULL;
	string			curr_datecpy					= NULL;
	string			curr_dateDup					= NULL;
	string			curr_dateDup1					= NULL;
	string			FromDateStr					= NULL;
	string			FromDateStrDup					= NULL;
	string			yearVal					= NULL;
	string			monthVal					= NULL;
	string			dateVal					= NULL;

	//string aplReason=NULL;
	string				aplReasonDup=NULL;

	FILE	*fp_PlantDmlList= NULL;
	FILE	*fp_PlantDMLTaskPartInfo= NULL;
	FILE	*fp_DMLPartInfo	= NULL;
	//FILE	*fp_DMLPartCADInfo	= NULL;
	FILE	*fp_DMLErrorLog	= NULL;
	//FILE	*fp_DMLBomList	= NULL;
	//FILE	*fp_DMLOPCSInfo	= NULL;
	//SetOfStrings DMLListSet     =  NULL;
		SetOfStrings		DMLValSet		=	NULL;


	int dmlCnt =0;
	//int taskCnt =0;
	int partCnt =0;
	int ii =0;
	int aplDmlCnt =0;
	int dateIntVal=0;
	int monthIntVal=0;
	int yearIntVal=0;
	int aplRestAMDMLgrp98Found =0;

	SetOfStrings      dbScp	=          NULL;

	
	t5MethodInitWMD("t5_STDDMLPartDetails");

	printf("\n Executing... t5_STDDMLPartDetails.c ... \n");

	LoginS=nlsStrAlloc(200);
	PasswordS=nlsStrAlloc(200);
	InpDMLNumberS=nlsStrAlloc(200);
	ProjectName=nlsStrAlloc(200);
	PlantName=nlsStrAlloc(200);
	workingPath=nlsStrAlloc(200);
	//DateFolder=nlsStrAlloc(200);
	DmlNoRule=nlsStrAlloc(200);
	sPlntNmeDup1=nlsStrAlloc(200);
	OutFilePlantDMLList=nlsStrAlloc(200);
	OutFilePlantDMLTaskPartInfo=nlsStrAlloc(200);
	OutFilePlantDMLPartInfo=nlsStrAlloc(200);
	OutFilePlantDMLPartCADInfo=nlsStrAlloc(200);
	OutFilePlantDMLErrorLog=nlsStrAlloc(200);
	OutFilePlantDMLBomList=nlsStrAlloc(200);
	OutFilePlantDMLOPCSInfo=nlsStrAlloc(200);
	csIndicator=nlsStrAlloc(50);
	StoreLocation=nlsStrAlloc(50);
	configCtxt=nlsStrAlloc(50);
	GenOptionalCS=nlsStrAlloc(50);
	curr_datecpy=nlsStrAlloc(50);
	//FromDateStrDup=nlsStrAlloc(50);
	//curr_dateDup1=nlsStrAlloc(50);

	LoginS=argv[1];
	PasswordS=argv[2];
	ProjectName=argv[3];
	PlantName=argv[4];
	workingPath=argv[5];
	//DateFolder=argv[6];
	//FromDateStrDup=argv[6];
	//curr_dateDup1=argv[7];


	//printf("\n !!!!!!!!!!!!Starting here!!!!!!!! \n");

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


	if (argc < 6 || argc > 6)
	{
		printf("\nSYNTEX ERROR :: \n");
		printf("\nSYNTEX SHOULD BE :: <EXE><USER-LOGIN><PASSWD><PROJECTNAME><PLANTNAME(like PlantName1,PlantName2 ...)><WORKINGPATH>\n");
		printf("\nTRY AGAIN !!!\n");
		goto CLEANUP;
	}
	printf("\n workingPath:::%s",workingPath);fflush(stdout);

	if((nlsStrCmp(PlantName,"PlantName1")==0) || (nlsStrCmp(PlantName,"PlantName4")==0))
	{
		nlsStrCpy(sPlntNmeDup1,"CARPLT");
		nlsStrCpy(DmlNoRule,"%APL");
		nlsStrCpy(csIndicator,"t5CarMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5CarStoreLocation");
		nlsStrCpy(GenOptionalCS,  "t5CarOptionalCS");
		nlsStrCpy(configCtxt,"GlobalCtxt");		
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName13")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"PUVBU");
		nlsStrCpy(DmlNoRule,"%APLV");
		nlsStrCpy(csIndicator,"t5PunUVMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5PunUVStoreLocation");
		nlsStrCpy(GenOptionalCS,  "t5PunUVOptionalCS");
		nlsStrCpy(configCtxt,"PUVCtxt");	
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName2")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"PNR");
		nlsStrCpy(DmlNoRule,"%APLU");
		nlsStrCpy(csIndicator,"t5PnrMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5PnrStoreLocation");
		nlsStrCpy(GenOptionalCS,  "t5PnrOptionalCS");
		nlsStrCpy(configCtxt,"PNRCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName3")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"PCVBU");
		nlsStrCpy(DmlNoRule,"%APL");
		nlsStrCpy(csIndicator,"t5PunMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5PunStoreLocation");
		nlsStrCpy(GenOptionalCS,  "t5PunOptionalCS");
		nlsStrCpy(configCtxt,"GlobalCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName6")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"AHD");
		nlsStrCpy(DmlNoRule,"%APLS");
		nlsStrCpy(csIndicator,"t5AhdMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5AhdStoreLocation");
		nlsStrCpy(GenOptionalCS,  "t5AhdOptionalCS");
		nlsStrCpy(configCtxt,"GlobalCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName8")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"JSR");
		nlsStrCpy(DmlNoRule,"%APLJ");
		nlsStrCpy(csIndicator,"t5JsrMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5JsrStoreLocation");
		nlsStrCpy(GenOptionalCS,  "t5JsrOptionalCS");
		nlsStrCpy(configCtxt,"JsrCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName9")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"LKO");
		nlsStrCpy(DmlNoRule,"%APLL");
		nlsStrCpy(csIndicator,"t5LkoMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5LkoStoreLocation");
		nlsStrCpy(GenOptionalCS,  "t5LkoOptionalCS");
		nlsStrCpy(configCtxt,"LkoCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(nlsStrCmp(PlantName,"PlantName11")==0)
	{
		nlsStrCpy(sPlntNmeDup1,"DWD");
		nlsStrCpy(DmlNoRule,"%APLD");
		nlsStrCpy(csIndicator,"t5DwdMakeBuyIndicator");
		nlsStrCpy(StoreLocation,"t5DwdStoreLocation");
		nlsStrCpy(GenOptionalCS,  "t5DwdOptionalCS");
		nlsStrCpy(configCtxt,"DWDCtxt");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else
	{
		printf("\nSYNTEX ERROR :: \n");
		printf("\n PLEASE ENTER CORRECT PLANTNAME(EX:PlantName1,PlantName13,PlantName2 etc..)\n");
		printf("\nTRY AGAIN !!!\n");
		goto CLEANUP;
	}
					

	sys_date = sysGetDateTime();
	low_strrpl(sys_date,'-','_');
	low_strrpl(sys_date,'/','_');
	low_strrpl(sys_date,':','_');

	nlsStrCpy(OutFilePlantDMLList,workingPath);
	nlsStrCat(OutFilePlantDMLList,"/");
	nlsStrCat(OutFilePlantDMLList,ProjectName);
	nlsStrCat(OutFilePlantDMLList,"_");
	nlsStrCat(OutFilePlantDMLList,sPlntNmeDup1);
	nlsStrCat(OutFilePlantDMLList,"_");
	nlsStrCat(OutFilePlantDMLList,"DMLList");
	nlsStrCat(OutFilePlantDMLList,".txt");

	printf("\n OutFilePlantDMLList %s !!!!\n",OutFilePlantDMLList);fflush(stdout);

	nlsStrCpy(OutFilePlantDMLErrorLog,workingPath);
	nlsStrCat(OutFilePlantDMLErrorLog,"/");
	nlsStrCat(OutFilePlantDMLErrorLog,ProjectName);
	nlsStrCat(OutFilePlantDMLErrorLog,"_");
	nlsStrCat(OutFilePlantDMLErrorLog,sPlntNmeDup1);
	nlsStrCat(OutFilePlantDMLErrorLog,"_ErrorLogInfo");
	nlsStrCat(OutFilePlantDMLErrorLog,".txt");

	printf("\n OutFilePlantDMLErrorLog %s !!!!\n",OutFilePlantDMLErrorLog);fflush(stdout);

	fp_DMLErrorLog=fopen(OutFilePlantDMLErrorLog,"w");
	if(fp_DMLErrorLog==NULL)
	{
		printf("\n%s:fp_DMLErrorLog file is not created...!!!!\n",OutFilePlantDMLErrorLog);fflush(stdout);
	}

	nlsStrCpy(OutFilePlantDMLTaskPartInfo,workingPath);
	nlsStrCat(OutFilePlantDMLTaskPartInfo,"/");
	nlsStrCat(OutFilePlantDMLTaskPartInfo,ProjectName);
	nlsStrCat(OutFilePlantDMLTaskPartInfo,"_");
	nlsStrCat(OutFilePlantDMLTaskPartInfo,sPlntNmeDup1);
	nlsStrCat(OutFilePlantDMLTaskPartInfo,"_DMLInfo");
	nlsStrCat(OutFilePlantDMLTaskPartInfo,".txt");

	printf("\n OutFilePlantDMLTaskPartInfo %s !!!!\n",OutFilePlantDMLTaskPartInfo);fflush(stdout);

	fp_PlantDMLTaskPartInfo=fopen(OutFilePlantDMLTaskPartInfo,"w");
	if(fp_PlantDMLTaskPartInfo==NULL)
	{
		printf("\n%s:fp_PlantDMLTaskPartInfo file is not created...!!!!\n",OutFilePlantDMLTaskPartInfo);fflush(stdout);
	}


	nlsStrCpy(OutFilePlantDMLPartInfo,workingPath);
	nlsStrCat(OutFilePlantDMLPartInfo,"/");
	nlsStrCat(OutFilePlantDMLPartInfo,ProjectName);
	nlsStrCat(OutFilePlantDMLPartInfo,"_");
	nlsStrCat(OutFilePlantDMLPartInfo,sPlntNmeDup1);
	nlsStrCat(OutFilePlantDMLPartInfo,"_PartListInfo");
	nlsStrCat(OutFilePlantDMLPartInfo,".txt");

	printf("\n OutFilePlantDMLPartInfo %s !!!!\n",OutFilePlantDMLPartInfo);fflush(stdout);

	fp_DMLPartInfo=fopen(OutFilePlantDMLPartInfo,"w");
	if(fp_DMLPartInfo==NULL)
	{
		printf("\n%s:fp_DMLPartInfo file is not created...!!!!\n",OutFilePlantDMLPartInfo);fflush(stdout);
	}

	fp_PlantDmlList=fopen(OutFilePlantDMLList,"w");
	if(fp_PlantDmlList==NULL)
	{
		printf("\n%s:fp_PlantDmlList file is not created...!!!!\n",OutFilePlantDMLList);fflush(stdout);
	}

	

	curr_date=sysGetDate();

	nlsStrCpy(curr_datecpy,curr_date);
	printf("\n curr_datecpy =%s \n",curr_datecpy);fflush(stdout);

	yearVal		= strtok(curr_datecpy,"-");
	monthVal	= strtok(NULL,"-");
	dateVal		= strtok(NULL,"-");

	yearIntVal= atoi(yearVal);

	monthIntVal= atoi(monthVal);

	dateIntVal= atoi(dateVal);
	
	int day = dayofweek(dateIntVal, monthIntVal, yearIntVal); 
	printf ("day  %d", day); fflush(stdout);

	if(day==1)
	{
		FromDateStr=sysGetOffsetDate(curr_date,0,-3,0);
	}
	else
	{
		FromDateStr=sysGetOffsetDate(curr_date,0,-1,0);
	}
	printf("\n FromDateStr =%s \n",FromDateStr);fflush(stdout);

	FromDateStr=strssra(FromDateStr, "-", "/");

	printf("\n 111 FromDateStr =%s \n",FromDateStr);fflush(stdout);
	if(!nlsIsStrNull(FromDateStr)) FromDateStrDup = nlsStrDup(FromDateStr);

	curr_dateDup=sysGetOffsetDate(curr_date,0,1,0);
	printf("\n curr_dateDup =%s \n",curr_dateDup);fflush(stdout);

	curr_dateDup=strssra(curr_dateDup, "-", "/");

	printf("\n 111 curr_dateDup =%s \n",curr_dateDup);fflush(stdout);
	if(!nlsIsStrNull(curr_dateDup)) curr_dateDup1 = nlsStrDup(curr_dateDup);

	printf("\n 111 FromDateStrDup =%s ::: curr_dateDup1 %s\n",FromDateStr,curr_dateDup1);fflush(stdout);

	DMLValSet		= setCreate(500);

//	low_set_add_str_unique(DMLValSet,"00PP000148_00_APL");
//low_set_add_str_unique(DMLValSet,"00PP000231_00_APL");
//low_set_add_str_unique(DMLValSet,"00PP000232_00_APL");
//low_set_add_str_unique(DMLValSet,"00PP000233_00_APL");
//low_set_add_str_unique(DMLValSet,"13PM324000_42_APL");
//low_set_add_str_unique(DMLValSet,"13PM331000_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331001_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331002_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331003_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331004_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331005_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331006_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331007_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331008_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331009_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331010_83_APL");
//low_set_add_str_unique(DMLValSet,"13PM331010_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PM331015_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP253000_09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253002_14_APL");
//low_set_add_str_unique(DMLValSet,"13PP253005_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253011_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253011_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253015_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253015_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253016_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253016_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253017_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253017_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253018_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253018_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253019_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253019_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253020_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253020_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253023_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253023_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253024_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253025_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253025_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253026_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253026_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253029_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253029_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253038_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253039_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253040_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253041_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253043_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253044_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253044_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253045_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253045_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253046_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253046_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253047_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253047_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253053_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253054_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253055_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253056_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253057_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253058_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253059_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253060_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253060_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253061_09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253062_09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253063_09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253064_14_APL");
//low_set_add_str_unique(DMLValSet,"13PP253065_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253066_09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253067_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253068_09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253069_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253071_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253072_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253072_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253073_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253074_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253075_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253076_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253077_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253079_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253082_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253083_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253088_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253088_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253089_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253089_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253096_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253097_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253098_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253099_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253101_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253101_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253109_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253109_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253110_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253110_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253115_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253115_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253116_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253116_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253117_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253123_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253123_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253126_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253126_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253130_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253133_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253133_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR12_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR13_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR14_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR17_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR18_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR19_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR20_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR21_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR22_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR23_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR24_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR25_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR26_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR27_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR28_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR29_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR30_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR31_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR32_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR33_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR34_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR35_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR36_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR37_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR39_APL");
//low_set_add_str_unique(DMLValSet,"13PP253142_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP253143_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP253144_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253144_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR12_APL");
//low_set_add_str_unique(DMLValSet,"13PP253145_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253151_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR12_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR13_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR14_APL");
//low_set_add_str_unique(DMLValSet,"13PP253152_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253153_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253155_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253156_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253157_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP253158_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253160_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253160_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253160_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253160_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253161_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253161_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253161_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253161_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253161_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253161_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253161_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253164_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253164_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253164_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253164_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR12_APL");
//low_set_add_str_unique(DMLValSet,"13PP253165_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253166_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253166_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253166_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253166_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253167_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253167_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253167_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253168_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253170_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253170_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253170_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253172_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253173_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253173_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253173_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253173_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253174_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253174_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253175_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253176_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253176_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253176_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253177_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253177_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253180_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253183_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253203_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253207_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253207_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253208_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253208_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253211_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253211_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253214_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP253215_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253216_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253217_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253217_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253217_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253217_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253217_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253217_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253217_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253219_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253219_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253221_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253222_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253222_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253224_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253225_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253225_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253225_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253225_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253225_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253227_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253227_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253229_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253229_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253231_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253231_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253231_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253231_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253231_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253231_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253234_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253234_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253236_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253240_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253243_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP253243_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253248_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253248_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253248_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253248_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253248_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP253248_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP253248_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253249_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253249_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253255_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253255_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253256_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253258_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253258_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253258_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253258_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253258_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253258_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253258_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253258_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253265_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253267_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253277_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253278_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253286_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253286_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253288_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253293_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253293_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253293_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253293_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253293_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253293_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP253293_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP253293_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253294_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253294_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253297_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253298_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253298_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253298_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253299_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253299_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253299_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253305_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253308_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253309_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253310_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253310_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253311_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253312_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253312_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253312_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253312_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253313_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253313_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253313_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253313_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253315_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253315_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253320_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253321_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253326_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253328_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253341_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253341_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253341_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253342_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253342_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253345_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253345_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253347_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253347_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253348_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253348_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253362_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253366_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253366_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253368_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253383_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253383_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253384_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253385_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253386_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253386_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253387_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253387_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253388_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253388_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253389_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253389_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253390_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253390_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253392_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253393_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253394_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253395_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253397_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253397_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253399_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253399_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253399_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253400_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253400_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253402_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253402_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253405_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253405_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253406_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253407_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253409_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253410_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253411_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253412_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253415_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253539_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253539_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253540_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253540_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253540_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253541_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253543_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253543_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253545_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253545_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253546_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253547_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253547_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253548_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253548_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253548_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253548_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP253548_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253549_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253549_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253549_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP253549_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253550_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253550_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253551_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253552_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253552_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253555_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253555_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253556_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253557_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253558_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253558_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253559_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253560_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253560_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253561_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253562_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253562_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253565_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253566_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253566_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253567_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253568_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253569_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253570_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253570_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP253571_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253573_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253576_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253578_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253579_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253579_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253580_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253580_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253582_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253583_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253585_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253587_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253590_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253592_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253596_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253600_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253601_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253601_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253602_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253602_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253609_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253615_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253616_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253621_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253626_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253626_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253627_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253628_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253628_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253634_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253636_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253636_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253640_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253645_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253646_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253648_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253649_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253651_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253652_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253655_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253655_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253656_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253657_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253661_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253662_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253668_VC01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253670_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253671_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253676_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253677_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253678_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253678_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP253682_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253683_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253686_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253687_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253695_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253698_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253698_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP253700_VC01_APL");
//low_set_add_str_unique(DMLValSet,"13PP253701_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253701_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253703_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253705_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253708_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253710_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253711_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253712_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253712_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253713_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253714_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253716_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253718_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253719_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253720_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253721_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253722_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253723_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253724_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253726_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253727_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253727_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP253728_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253729_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253730_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253731_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253732_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253736_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253738_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253739_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253741_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253742_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253743_16_APL");
//low_set_add_str_unique(DMLValSet,"13PP253746_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253747_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253748_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253749_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253750_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253751_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP253772_00_APL");
//low_set_add_str_unique(DMLValSet,"13PP254000_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254001_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254002_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254002_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP254003_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254005_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254005_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP254006_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254007_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254008_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254009_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254010_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254011_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254011_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP254012_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254013_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254019_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254021_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254021_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP254023_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254025_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254027_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254027_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP254028_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254029_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254029_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP254030_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254031_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254033_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP254034_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254035_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254036_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254042_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254044_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254045_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254046_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254047_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254049_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254050_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254050_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP254051_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254051_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP254052_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254052_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP254053_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254055_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254056_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254057_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254058_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254059_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254060_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254062_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP254064_24_APL");
//low_set_add_str_unique(DMLValSet,"13PP254064_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256001_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256001_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256002_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256002_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256003_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256003_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256004_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256004_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256005_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256007_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256007_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256009_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256009_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256011_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256011_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256013_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256013_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256014_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256015_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256016_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256017_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256017_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256018_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256018_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP256019_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256019_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP256020_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256020_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP256021_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256021_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP256022_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP256029_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP256030_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP256031_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP256034_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256034_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP256035_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256035_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP256035_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP256035_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP256035_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP256036_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256036_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP256037_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256040_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256040_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP256043_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP256043_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP256044_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256045_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256045_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP256047_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256048_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256049_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256051_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP256052_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP256053_29_APL");
//low_set_add_str_unique(DMLValSet,"13PP257000_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP257001_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP257002_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP257003_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP257003_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP257006_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP257008_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP257009_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP258000_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258001_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258005_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258007_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258008_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258009_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258010_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258011_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258012_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258013_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258025_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258026_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP258026_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP258026_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP258026_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP258030_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP258031_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258031_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP258032_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258032_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP258033_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258033_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP258034_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258034_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP258035_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258035_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP258036_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258037_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258040_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258040_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP258044_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258045_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258045_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP258051_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258051_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP258053_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP258056_31_APL");
//low_set_add_str_unique(DMLValSet,"13PP259000_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259000_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP259002_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259002_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP259003_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259003_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP259005_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259005_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP259007_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259007_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP259007_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP259010_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259010_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP259010_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP259011_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259011_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP259013_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259013_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP259014_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259014_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP259018_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259018_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP259020_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP259025_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP259031_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259031_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP259032_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP259032_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP259032_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP259033_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259034_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259036_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259036_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP259036_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP259037_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP259038_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259039_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259039_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP259039_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP259041_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259041_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP259042_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259045_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259046_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259048_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259049_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259052_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259053_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259055_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259055_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP259056_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP259063_32_APL");
//low_set_add_str_unique(DMLValSet,"13PP260000_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260002_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260003_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260005_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260006_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260007_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260007_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP260008_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260009_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260016_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260016_35_APL");
//low_set_add_str_unique(DMLValSet,"13PP260017_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260019_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260020_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260022_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP260024_35_APL");
//low_set_add_str_unique(DMLValSet,"13PP260025_33_APL");
//low_set_add_str_unique(DMLValSet,"13PP261000_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261001_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261002_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261003_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261005_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261006_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261007_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261008_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261009_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261010_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261011_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261012_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261014_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261015_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261016_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261026_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261027_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261031_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261032_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP261032_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP261032_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261033_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP261033_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261034_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261034_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP261034_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP261034_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP261034_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261037_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261038_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP261038_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP261038_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261043_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261043_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP261043_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP261043_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP261043_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP261043_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP261043_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP261044_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261046_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261047_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261047_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP261048_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261052_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261053_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261054_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261055_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261056_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261056_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP261057_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP261058_40_APL");
//low_set_add_str_unique(DMLValSet,"13PP262000_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262001_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262003_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262004_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262004_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP262005_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262005_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP262007_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262007_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP262008_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262008_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP262009_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262009_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP262010_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262010_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP262012_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262012_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP262014_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262014_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP262016_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262016_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP262017_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262018_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262024_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262025_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262027_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262029_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262030_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP262030_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262033_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP262033_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP262033_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP262033_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP262033_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP262033_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP262033_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP262033_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262037_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262038_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262040_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP262040_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP262040_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP262040_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262041_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262042_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262042_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP262042_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262044_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP262044_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262045_43_APL");
//low_set_add_str_unique(DMLValSet,"13PP262045_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP262045_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262046_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262047_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262048_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP262048_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262051_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262052_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262053_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262054_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262055_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262057_43_APL");
//low_set_add_str_unique(DMLValSet,"13PP262058_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262059_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262059_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262063_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262063_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262064_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262069_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262072_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262073_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262074_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262078_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262079_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262080_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262080_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP262081_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262086_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262088_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262091_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP262093_42_APL");
//low_set_add_str_unique(DMLValSet,"13PP262A02_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262A03_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP262A03_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP262A04_SP_APL");
//low_set_add_str_unique(DMLValSet,"13PP263000_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263001_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263002_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263003_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263004_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263005_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263006_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263006_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP263007_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263009_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263009_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP263010_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263010_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP263011_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263011_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP263012_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263022_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263023_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP263024_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263025_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263026_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263031_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263032_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263033_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263035_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263037_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263038_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263042_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263044_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP263045_46_APL");
//low_set_add_str_unique(DMLValSet,"13PP264000_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264000_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP264001_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264001_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264002_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264002_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264004_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264004_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP264005_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264005_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP264006_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264007_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264008_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264008_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264009_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264009_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264010_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264010_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP264011_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264011_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP264012_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264013_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264013_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264017_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264017_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264018_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264018_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP264018_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264021_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264021_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP264021_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP264021_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP264023_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264024_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264024_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264025_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264026_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264026_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264026_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264027_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264027_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264028_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264028_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264031_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264031_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264031_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP264031_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264032_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264033_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264034_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264035_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264037_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264037_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264039_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264044_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264044_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP264044_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264044_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264044_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP264044_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP264044_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264046_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264046_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264048_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264048_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP264048_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264048_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP264048_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP264049_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264052_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264053_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264054_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264054_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP264057_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264059_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264060_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264061_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264062_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP264063_47_APL");
//low_set_add_str_unique(DMLValSet,"13PP265001_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265002_50_APL");
//low_set_add_str_unique(DMLValSet,"13PP265002_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP265007_50_APL");
//low_set_add_str_unique(DMLValSet,"13PP265007_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP265008_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265009_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265011_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265012_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265013_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265013_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP265014_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265014_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP265015_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265015_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP265019_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP265020_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP265020_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP265020_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP265025_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP265026_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP265028_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265029_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265029_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP265031_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP265031_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP265033_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265034_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265039_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP265039_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP265039_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP265043_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265043_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP265044_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265047_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265047_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP265048_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265051_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265053_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP265054_49_APL");
//low_set_add_str_unique(DMLValSet,"13PP266001_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266001_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266002_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266002_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266003_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266003_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266004_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266005_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266006_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266007_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266007_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266008_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266008_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266009_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266010_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266011_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266013_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266014_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266014_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266015_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266016_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266016_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266018_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266018_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266021_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266024_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266024_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266024_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266025_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266026_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266026_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266027_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266028_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266028_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266028_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266028_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266029_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266029_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266030_04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266030_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266030_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266030_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266031_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266031_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266033_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266033_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266034_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266035_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266035_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266036_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266036_CL_APL");
//low_set_add_str_unique(DMLValSet,"13PP266036_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266037_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266038_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266038_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266039_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266039_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266041_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266041_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266042_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266057_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266060_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266060_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266060_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266060_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266060_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266060_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP266060_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266062_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266063_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266063_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266063_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266065_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266070_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266070_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266070_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266074_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266075_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266078_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266079_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266079_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266079_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266079_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266082_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266082_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266082_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266082_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266082_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266082_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP266082_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP266082_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266083_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266083_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266083_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266083_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266090_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266090_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266090_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266090_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266090_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266090_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266091_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266091_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266091_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266092_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266096_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR12_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR13_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR14_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR16_APL");
//low_set_add_str_unique(DMLValSet,"13PP266100_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266101_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266101_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266101_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266104_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266104_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266106_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266107_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266107_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266107_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266107_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266109_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266110_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266110_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266110_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266110_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266110_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266110_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR12_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR13_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR14_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR16_APL");
//low_set_add_str_unique(DMLValSet,"13PP266111_PR17_APL");
//low_set_add_str_unique(DMLValSet,"13PP266112_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266112_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266112_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266113_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266113_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266115_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266115_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266115_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266115_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266116_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266117_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266117_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266117_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266117_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266117_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266120_53_APL");
//low_set_add_str_unique(DMLValSet,"13PP266125_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266126_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266126_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266126_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266127_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266127_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266127_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266128_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266128_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266128_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266128_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266128_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266128_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266128_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266129_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266129_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266129_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266130_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266130_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266130_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266131_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266131_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266137_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266137_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266137_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266137_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266140_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266140_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266140_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266141_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266141_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266141_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266142_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266144_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266144_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266144_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266144_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266144_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266144_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266146_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266147_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266148_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266150_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266150_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266151_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266151_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266151_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266154_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266155_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266155_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266155_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266155_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266156_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266157_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266159_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266167_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266168_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266169_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266169_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266169_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266175_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266175_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266175_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266175_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266176_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266178_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266178_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266179_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266181_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266181_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266182_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266184_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266184_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266187_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266190_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266190_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP266194_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266195_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266196_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266196_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266201_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266203_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266205_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266206_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266207_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266207_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266208_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266209_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266211_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266211_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266212_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266214_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266214_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266224_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266229_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266232_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266234_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266234_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266234_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP266234_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP266234_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266235_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266236_30_APL");
//low_set_add_str_unique(DMLValSet,"13PP266236_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP266236_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266236_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP266236_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP266237_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266240_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266246_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266247_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266249_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266251_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266252_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266255_15_APL");
//low_set_add_str_unique(DMLValSet,"13PP266256_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266256_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP266257_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266258_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266259_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266260_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266264_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266265_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266266_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266267_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266269_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266272_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266273_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266275_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266276_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266277_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266278_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266279_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266280_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266282_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266283_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266284_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266285_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266288_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266289_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266290_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266293_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266294_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266295_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266297_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266298_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266301_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266302_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP266303_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266304_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP266305_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP266315_54_APL");
//low_set_add_str_unique(DMLValSet,"13PP267000_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP267000_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP268000_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268000_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP268001_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268001_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP268002_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268003_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268004_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268006_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268007_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268012_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP268016_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268018_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268021_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268022_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268023_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268027_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268028_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP268029_58_APL");
//low_set_add_str_unique(DMLValSet,"13PP269000_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269001_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269002_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269004_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269005_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269006_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269007_50_APL");
//low_set_add_str_unique(DMLValSet,"13PP269007_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269007_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269007_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269007_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269007_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269008_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269008_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269008_64_APL");
//low_set_add_str_unique(DMLValSet,"13PP269008_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269009_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269009_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269011_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269011_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269012_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269012_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269013_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269013_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269014_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269014_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269015_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269016_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269016_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269017_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269017_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269018_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269020_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269020_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269020_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269020_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269020_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269020_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269020_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269020_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269021_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269022_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269022_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269023_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269024_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269024_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269025_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269026_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269026_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269027_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269027_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269028_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269029_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269032_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269033_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269034_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269035_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269036_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269037_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269038_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269039_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269039_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269040_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269040_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269041_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269041_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269042_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269043_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269043_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269045_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269046_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269047_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269048_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269048_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269049_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269049_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269050_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269050_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269051_76_APL");
//low_set_add_str_unique(DMLValSet,"13PP269052_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269052_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269053_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269054_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269055_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269056_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269057_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269058_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269059_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269063_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269066_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269067_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269067_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269068_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269068_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269070_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269070_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269073_99_APL");
//low_set_add_str_unique(DMLValSet,"13PP269073_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269074_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269074_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269075_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269075_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269080_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269080_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269081_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269081_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269082_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269083_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269084_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269085_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269088_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269089_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269090_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269091_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269091_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269092_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269093_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269094_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269095_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269096_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269097_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269097_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269098_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269098_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269099_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269099_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269100_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269100_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269101_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269102_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269102_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269104_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269106_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269107_99_APL");
//low_set_add_str_unique(DMLValSet,"13PP269107_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269109_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269113_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269121_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269123_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269125_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269126_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269128_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269131_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269131_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269132_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269132_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269134_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269134_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269136_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269136_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269144_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269144_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269146_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269147_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269151_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269152_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269153_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269154_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269155_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269164_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269164_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269165_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269165_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269166_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269169_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269170_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269170_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269173_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269175_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269177_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269178_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269179_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269180_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269182_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269184_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269185_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269186_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269188_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269190_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269192_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269193_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269193_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269194_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269198_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269200_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269202_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269202_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269205_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269207_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269209_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269213_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269215_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269215_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269216_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269216_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269218_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269219_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269220_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269221_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269224_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269225_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269231_99_APL");
//low_set_add_str_unique(DMLValSet,"13PP269235_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269236_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269237_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269238_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269239_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269240_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269241_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269242_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269244_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269245_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269246_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269247_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269248_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269249_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269254_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269254_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269255_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269257_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269258_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269260_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269262_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269266_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269267_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269270_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269271_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269272_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269273_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269274_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269274_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269275_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269277_99_APL");
//low_set_add_str_unique(DMLValSet,"13PP269277_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269278_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269281_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269284_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269285_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269286_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269287_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269287_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269289_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269290_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269295_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269296_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269297_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269297_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269298_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269298_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269299_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269299_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269300_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269301_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269301_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269302_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269302_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269302_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269302_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269302_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269302_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269303_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269303_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269304_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269304_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269305_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269305_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269306_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269307_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269308_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269309_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269310_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269311_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269312_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269313_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269314_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269314_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269315_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269317_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269318_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269318_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269319_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269320_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269320_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269321_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269321_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269322_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269322_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269323_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269323_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269324_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269325_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269327_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269327_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269328_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269328_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269328_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269328_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269329_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269329_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269330_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269330_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269330_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269330_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269331_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269331_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269332_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269333_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269333_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269334_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269334_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269335_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269336_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269338_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269339_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269339_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269340_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269340_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269340_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269340_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269341_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269341_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269342_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269342_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269343_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269345_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269345_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269346_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269347_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269348_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269348_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269349_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269349_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269351_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269351_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269351_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269352_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269352_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269352_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269353_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269353_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269357_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269357_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269358_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269358_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269358_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269360_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269360_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269361_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269361_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269362_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269363_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269364_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269365_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269366_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269367_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269367_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269367_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269367_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269367_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269367_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269368_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269368_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269368_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269369_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269369_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269369_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269369_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269369_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269369_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269371_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269371_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269372_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269373_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269373_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269374_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269374_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269376_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269377_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269378_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269378_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269378_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269379_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269379_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269383_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269383_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269384_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269385_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269385_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269386_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269388_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269390_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269390_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269390_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269394_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269394_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269394_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269407_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269412_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269415_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269416_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269436_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269440_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269452_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269457_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269459_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269463_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269463_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269465_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269469_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269472_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269473_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269474_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269474_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269474_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269488_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269488_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269489_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269489_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269489_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269489_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269489_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269489_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269489_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP269489_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269496_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269498_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269498_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269498_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269503_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269507_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269507_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269507_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269507_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269507_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269510_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269510_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269510_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269510_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269510_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269511_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269511_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269511_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269511_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269511_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269511_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP269511_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269512_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269512_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269512_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269512_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269512_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP269512_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269516_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269516_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269516_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269519_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269520_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269520_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269522_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269522_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269522_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269525_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269525_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269529_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269532_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269534_99_APL");
//low_set_add_str_unique(DMLValSet,"13PP269534_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269538_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269539_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269544_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269545_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269545_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269545_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269545_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269545_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269545_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP269545_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269548_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269548_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269548_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269548_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269548_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269549_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269549_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269549_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP269549_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269550_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR09_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR10_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR11_APL");
//low_set_add_str_unique(DMLValSet,"13PP269551_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269553_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269553_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269553_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269553_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269553_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269553_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269554_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269554_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269554_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269555_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269555_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269559_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269559_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269560_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269561_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269562_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269563_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269563_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269564_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269566_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269566_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269566_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR06_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR07_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR08_APL");
//low_set_add_str_unique(DMLValSet,"13PP269568_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269569_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269570_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269570_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269574_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269574_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269574_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269574_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269574_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269574_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269575_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269575_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269575_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269576_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269577_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269577_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269577_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269577_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269578_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269578_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269580_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269581_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269581_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269584_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269586_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269586_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269591_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269593_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269595_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269595_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269595_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269601_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269602_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269607_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269609_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269609_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269609_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269609_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269609_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269609_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269609_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269610_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269611_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269614_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269617_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269617_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269617_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269618_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269618_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269618_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269619_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269619_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269621_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269622_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269623_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269624_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269626_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269627_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269629_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269630_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269630_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269631_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269631_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269632_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269632_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269632_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269633_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269636_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269636_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269638_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269639_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269640_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269640_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269642_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269643_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269644_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269644_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269646_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269646_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269651_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269651_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269651_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269651_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269651_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269651_PR05_APL");
//low_set_add_str_unique(DMLValSet,"13PP269651_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269652_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269652_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269655_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269656_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269657_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269657_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269658_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269658_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269659_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269659_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269660_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269660_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269660_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269661_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269661_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269661_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269661_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269661_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269663_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269665_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269666_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269666_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269670_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269672_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269674_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269674_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269675_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269676_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269679_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269683_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269686_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269686_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269687_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269687_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269688_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269688_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269692_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269692_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269699_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269699_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269699_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269701_92_APL");
//low_set_add_str_unique(DMLValSet,"13PP269702_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269704_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269704_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269705_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269705_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269706_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269708_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269708_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269710_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269711_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269711_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269711_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269713_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269714_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269720_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269720_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269721_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269722_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269723_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP269724_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269725_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269725_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269725_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269726_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269728_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269728_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269728_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269728_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269734_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269736_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269737_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269737_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269737_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269740_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269741_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269741_65_APL");
//low_set_add_str_unique(DMLValSet,"13PP269742_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269743_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269746_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269746_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269747_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269748_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269748_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269750_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269751_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269753_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269754_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269756_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269757_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269757_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269758_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269758_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269760_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269763_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269764_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269766_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269770_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269770_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269771_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269772_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269772_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269772_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269773_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269774_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269775_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269776_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269776_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269776_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269777_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269777_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269779_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269785_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269786_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269787_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269789_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269791_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269793_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269793_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269794_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269795_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269796_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269797_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269798_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269798_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269800_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269800_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269802_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269802_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269805_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269806_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269806_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269807_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269809_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269810_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269812_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269812_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269812_PR04_APL");
//low_set_add_str_unique(DMLValSet,"13PP269812_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269814_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269815_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269815_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269816_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269817_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269818_81_APL");
//low_set_add_str_unique(DMLValSet,"13PP269819_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269820_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269821_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269822_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269823_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269823_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269823_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269825_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269826_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269828_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269829_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269829_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269833_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269834_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269836_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269836_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269837_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269837_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269839_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP269840_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269842_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269843_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269844_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269845_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269846_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269848_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269850_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269850_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269854_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269855_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269857_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269860_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269860_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269861_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269862_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269863_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269865_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269866_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269868_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269869_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269870_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269870_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269874_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269875_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269877_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269877_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269878_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269879_91_APL");
//low_set_add_str_unique(DMLValSet,"13PP269879_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269882_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269884_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269886_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269887_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269887_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269887_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269888_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269890_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269890_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269891_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269892_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269893_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269893_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269895_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269896_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269896_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269897_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269898_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269900_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP269901_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269902_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269903_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269904_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269906_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269907_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269907_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269907_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269907_PR02_APL");
//low_set_add_str_unique(DMLValSet,"13PP269907_PR03_APL");
//low_set_add_str_unique(DMLValSet,"13PP269907_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269909_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269911_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269911_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269912_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269912_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269912_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269913_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269913_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269914_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269914_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269915_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269915_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269917_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269919_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269919_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269920_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269920_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269923_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP269923_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269924_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269924_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269928_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269929_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269930_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269932_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269935_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269936_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269937_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269937_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269938_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269940_PR01_APL");
//low_set_add_str_unique(DMLValSet,"13PP269940_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269942_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269943_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269944_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269945_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269946_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269946_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269947_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269947_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269949_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP269950_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269951_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269953_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269954_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269955_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP269956_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269960_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269960_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269961_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269963_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269963_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269964_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269965_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269966_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269966_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269968_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269968_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269969_PR_APL");
//low_set_add_str_unique(DMLValSet,"13PP269971_55_APL");
//low_set_add_str_unique(DMLValSet,"13PP269972_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269973_74_APL");
//low_set_add_str_unique(DMLValSet,"13PP269973_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269976_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269979_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269979_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269980_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269983_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP269985_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269986_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269989_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP269990_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP269991_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269994_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP269994_73_APL");
//low_set_add_str_unique(DMLValSet,"13PP269994_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP269997_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269998_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP269998_NONERC_APL");
//low_set_add_str_unique(DMLValSet,"13PP269999_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP270001_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP270002_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP270008_82_APL");
//low_set_add_str_unique(DMLValSet,"13PP270012_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP270013_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP270013_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP270013_62_APL");
//low_set_add_str_unique(DMLValSet,"13PP270014_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP270015_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP270020_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP270023_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP270025_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP270027_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP270028_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP270029_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP270030_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP270031_61_APL");
//low_set_add_str_unique(DMLValSet,"13PP270032_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270033_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP270036_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP270037_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP270038_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270041_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270042_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP270042_63_APL");
//low_set_add_str_unique(DMLValSet,"13PP270043_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP270044_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP270045_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP270048_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270049_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP270052_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP270054_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270058_CO_APL");
//low_set_add_str_unique(DMLValSet,"13PP270059_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270060_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270063_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP270064_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270065_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP270066_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270068_60_APL");
//low_set_add_str_unique(DMLValSet,"13PP270073_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270079_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270080_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270084_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP270085_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP270087_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP270088_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP270092_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP270093_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP270094_69_APL");
//low_set_add_str_unique(DMLValSet,"13PP270102_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP270103_88_APL");
//low_set_add_str_unique(DMLValSet,"13PP270104_67_APL");
//low_set_add_str_unique(DMLValSet,"13PP270110_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP270117_68_APL");
//low_set_add_str_unique(DMLValSet,"13PP270119_72_APL");
//low_set_add_str_unique(DMLValSet,"13PP270120_83_APL");
//low_set_add_str_unique(DMLValSet,"13PP270126_97_APL");
//low_set_add_str_unique(DMLValSet,"13PP270130_69_APL");
//low_set_add_str_unique(DMLValSet,"14AM901467_00_APL");
//low_set_add_str_unique(DMLValSet,"14AM902339_00_APL");
//low_set_add_str_unique(DMLValSet,"14AM902340_00_APL");
//low_set_add_str_unique(DMLValSet,"14AM902449_00_APL");
//low_set_add_str_unique(DMLValSet,"14AM902451_00_APL");
//low_set_add_str_unique(DMLValSet,"14PP100000_35_APL");
//low_set_add_str_unique(DMLValSet,"14PP100001_PR_APL");
//low_set_add_str_unique(DMLValSet,"14PP110000_50_APL");
//low_set_add_str_unique(DMLValSet,"14PP696001_00_APL");
//low_set_add_str_unique(DMLValSet,"14PP696002_00_APL");
//low_set_add_str_unique(DMLValSet,"14PP696003_00_APL");
//low_set_add_str_unique(DMLValSet,"14PP696004_00_APL");
//low_set_add_str_unique(DMLValSet,"14PP696005_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901017_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901115_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901121_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901667_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901716_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901787_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901824_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901847_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901864_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901865_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901908_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901928_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM901949_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM902419_54_APL");
//low_set_add_str_unique(DMLValSet,"15AM902433_54_APL");
//low_set_add_str_unique(DMLValSet,"15AM902479_32_APL");
//low_set_add_str_unique(DMLValSet,"15AM902636_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM902636_PR_APL");
//low_set_add_str_unique(DMLValSet,"15AM902792_33_APL");
//low_set_add_str_unique(DMLValSet,"15AM902793_33_APL");
//low_set_add_str_unique(DMLValSet,"15AM902795_42_APL");
//low_set_add_str_unique(DMLValSet,"15AM902797_46_APL");
//low_set_add_str_unique(DMLValSet,"15AM902798_60_APL");
//low_set_add_str_unique(DMLValSet,"15AM902799_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM902804_46_APL");
//low_set_add_str_unique(DMLValSet,"15AM902807_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM902814_00_APL");
//low_set_add_str_unique(DMLValSet,"15AM902902_54_APL");
//low_set_add_str_unique(DMLValSet,"15AM902946_54_APL");
//low_set_add_str_unique(DMLValSet,"15AM902948_69_APL");
//low_set_add_str_unique(DMLValSet,"16AM901670_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM902437_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM902507_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM902519_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM902641_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM902789_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM902795_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM904125_60_APL");
//low_set_add_str_unique(DMLValSet,"16AM904136_60_APL");
//low_set_add_str_unique(DMLValSet,"16AM904196_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM904214_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM904250_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM904477_60_APL");
//low_set_add_str_unique(DMLValSet,"16AM904510_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM904569_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM904584_00_APL");
//low_set_add_str_unique(DMLValSet,"16AM904676_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM901786_26_APL");
//low_set_add_str_unique(DMLValSet,"17AM901955_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM902122_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM902230_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM902491_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM902725_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM903385_26_APL");
//low_set_add_str_unique(DMLValSet,"17AM903603_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM903778_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM903935_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM904164_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM904169_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM904328_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM904479_00_APL");
//low_set_add_str_unique(DMLValSet,"17AM904578_00_APL");
//low_set_add_str_unique(DMLValSet,"17PP098002_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP098003_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP098004_PR03_APL");
//low_set_add_str_unique(DMLValSet,"17PP098004_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP099001_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP099001_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP099003_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR03_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR04_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR05_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR06_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR07_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR08_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR09_APL");
//low_set_add_str_unique(DMLValSet,"17PP103000_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103001_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103003_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103003_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103003_PR03_APL");
//low_set_add_str_unique(DMLValSet,"17PP103003_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103006_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103006_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103006_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103008_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103010_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103010_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103010_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103011_CO_APL");
//low_set_add_str_unique(DMLValSet,"17PP103011_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103012_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103012_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103013_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103013_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103014_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103016_69_APL");
//low_set_add_str_unique(DMLValSet,"17PP103019_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103019_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103019_PR03_APL");
//low_set_add_str_unique(DMLValSet,"17PP103019_PR04_APL");
//low_set_add_str_unique(DMLValSet,"17PP103019_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103021_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103021_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103021_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103023_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103023_PR03_APL");
//low_set_add_str_unique(DMLValSet,"17PP103023_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103024_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103024_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103025_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103033_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103033_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103033_PR04_APL");
//low_set_add_str_unique(DMLValSet,"17PP103033_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103038_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103040_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103043_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103043_PR02_APL");
//low_set_add_str_unique(DMLValSet,"17PP103043_PR03_APL");
//low_set_add_str_unique(DMLValSet,"17PP103043_PR_APL");
//low_set_add_str_unique(DMLValSet,"17PP103045_60_APL");
//low_set_add_str_unique(DMLValSet,"17PP103045_PR01_APL");
//low_set_add_str_unique(DMLValSet,"17PP103045_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM901047_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM901097_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM901099_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM901140_65_APL");
//low_set_add_str_unique(DMLValSet,"18AM901224_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM901303_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM901534_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM901760_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM901789_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902097_15_APL");
//low_set_add_str_unique(DMLValSet,"18AM902101_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902201_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902305_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM902320_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902338_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902351_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902377_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902402_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902427_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM902427_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM902485_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902503_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902555_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902611_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902784_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902803_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM902808_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903187_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903187_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR03_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR04_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR05_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR06_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR07_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR08_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR09_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR10_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR11_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR12_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR13_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR14_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR15_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR16_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR17_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR18_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR19_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR20_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR21_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR22_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR23_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR24_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR25_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR26_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR27_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR28_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR29_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR30_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR31_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR32_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR33_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR34_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR35_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR36_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR37_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR38_APL");
//low_set_add_str_unique(DMLValSet,"18AM903245_PR39_APL");
//low_set_add_str_unique(DMLValSet,"18AM903382_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903508_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903632_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903823_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903824_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903867_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903887_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903904_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM903938_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903942_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM903984_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM903994_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM903994_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904078_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904078_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904079_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904079_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904080_67_APL");
//low_set_add_str_unique(DMLValSet,"18AM904080_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904081_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904081_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904082_67_APL");
//low_set_add_str_unique(DMLValSet,"18AM904082_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904084_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904084_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904088_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904089_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904090_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904090_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904091_67_APL");
//low_set_add_str_unique(DMLValSet,"18AM904091_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904092_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904092_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904093_81_APL");
//low_set_add_str_unique(DMLValSet,"18AM904093_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904094_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904094_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904095_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904095_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904097_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904097_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904098_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904098_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904099_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904099_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904100_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904100_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904102_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904102_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904103_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904103_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904104_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904104_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904105_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904105_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904105_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904106_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904106_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904112_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904112_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904114_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904114_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904115_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904115_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904117_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904117_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904119_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904119_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904121_31_APL");
//low_set_add_str_unique(DMLValSet,"18AM904121_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904122_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904122_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904123_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904123_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904128_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904128_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904129_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904129_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904130_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904130_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904131_33_APL");
//low_set_add_str_unique(DMLValSet,"18AM904131_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904132_33_APL");
//low_set_add_str_unique(DMLValSet,"18AM904132_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904133_40_APL");
//low_set_add_str_unique(DMLValSet,"18AM904133_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904134_40_APL");
//low_set_add_str_unique(DMLValSet,"18AM904134_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904135_40_APL");
//low_set_add_str_unique(DMLValSet,"18AM904135_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904136_46_APL");
//low_set_add_str_unique(DMLValSet,"18AM904136_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904137_46_APL");
//low_set_add_str_unique(DMLValSet,"18AM904137_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904138_31_APL");
//low_set_add_str_unique(DMLValSet,"18AM904138_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904143_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904148_24_APL");
//low_set_add_str_unique(DMLValSet,"18AM904148_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904149_24_APL");
//low_set_add_str_unique(DMLValSet,"18AM904149_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904150_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904150_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904151_29_APL");
//low_set_add_str_unique(DMLValSet,"18AM904151_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904152_29_APL");
//low_set_add_str_unique(DMLValSet,"18AM904152_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904153_29_APL");
//low_set_add_str_unique(DMLValSet,"18AM904154_30_APL");
//low_set_add_str_unique(DMLValSet,"18AM904154_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904155_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904155_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904156_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904156_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904157_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904157_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904158_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904158_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904159_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904159_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904160_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904160_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904161_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904161_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904162_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904162_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904163_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904163_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904164_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904164_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904165_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904165_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904165_PR03_APL");
//low_set_add_str_unique(DMLValSet,"18AM904166_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904166_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904167_49_APL");
//low_set_add_str_unique(DMLValSet,"18AM904167_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904168_49_APL");
//low_set_add_str_unique(DMLValSet,"18AM904168_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904169_49_APL");
//low_set_add_str_unique(DMLValSet,"18AM904169_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904170_49_APL");
//low_set_add_str_unique(DMLValSet,"18AM904170_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904171_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904171_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904172_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904172_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904173_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904173_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904174_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904174_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904175_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904175_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904176_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904176_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904177_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904177_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904178_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904178_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904179_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904179_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904180_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904180_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904182_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904182_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904183_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904183_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904184_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904185_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904185_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904186_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904186_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904187_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904187_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904188_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904188_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904189_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904189_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904192_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904192_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904193_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904193_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904194_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904194_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904195_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904195_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904196_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904196_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904197_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904197_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904198_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904198_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904199_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904199_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904200_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904200_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904201_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904201_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904202_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904202_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904203_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904203_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904204_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904204_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904205_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904205_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904206_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904206_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904207_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904207_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904208_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904208_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904209_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904209_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904210_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904210_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904211_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904211_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904212_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904212_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904213_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904213_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904214_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904214_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904215_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904216_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904216_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904217_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904217_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904218_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904219_58_APL");
//low_set_add_str_unique(DMLValSet,"18AM904219_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904220_58_APL");
//low_set_add_str_unique(DMLValSet,"18AM904220_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904221_58_APL");
//low_set_add_str_unique(DMLValSet,"18AM904221_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904222_58_APL");
//low_set_add_str_unique(DMLValSet,"18AM904222_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904223_58_APL");
//low_set_add_str_unique(DMLValSet,"18AM904223_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904224_58_APL");
//low_set_add_str_unique(DMLValSet,"18AM904224_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904225_58_APL");
//low_set_add_str_unique(DMLValSet,"18AM904225_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904226_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904226_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904227_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904227_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904228_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904228_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904229_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904229_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904230_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904230_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904231_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904231_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904232_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904232_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904233_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904233_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904234_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904234_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904235_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904235_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904236_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904236_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904237_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904237_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904238_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904238_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904239_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904239_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904240_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904240_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904241_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904241_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904242_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904242_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904243_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904243_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904244_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904244_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904245_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904245_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904246_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904246_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904247_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904247_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904248_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904248_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904249_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904249_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904250_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904250_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904251_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904251_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904252_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904252_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904253_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904253_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904254_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904254_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904255_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904255_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904256_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904256_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904257_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904257_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904258_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904258_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904259_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904259_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904260_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904260_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904261_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904261_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904262_97_APL");
//low_set_add_str_unique(DMLValSet,"18AM904262_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904263_97_APL");
//low_set_add_str_unique(DMLValSet,"18AM904263_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904264_97_APL");
//low_set_add_str_unique(DMLValSet,"18AM904264_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904265_97_APL");
//low_set_add_str_unique(DMLValSet,"18AM904265_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904266_97_APL");
//low_set_add_str_unique(DMLValSet,"18AM904266_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904267_91_APL");
//low_set_add_str_unique(DMLValSet,"18AM904267_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904268_93_APL");
//low_set_add_str_unique(DMLValSet,"18AM904268_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904269_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904269_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904270_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904270_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904271_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904271_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904272_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904272_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904273_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904273_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904274_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904274_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904275_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904275_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904276_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904276_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904277_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904277_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904277_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904278_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904278_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904279_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904279_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904280_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904280_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904281_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904281_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904282_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904282_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904283_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904283_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904284_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904284_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904285_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904286_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904286_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904287_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904287_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904288_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904288_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904289_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904289_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904290_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904290_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904291_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904291_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904292_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904292_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904293_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904293_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904294_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904295_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904295_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904296_97_APL");
//low_set_add_str_unique(DMLValSet,"18AM904296_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904297_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904297_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904298_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904298_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904299_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904299_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904300_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904300_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904301_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904301_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904302_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904303_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904303_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904304_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904304_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904305_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904305_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904306_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904307_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904307_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904308_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904308_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904309_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904309_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904310_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904310_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904311_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904311_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904312_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904312_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904313_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904313_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904314_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904314_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904315_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904315_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904316_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904316_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904317_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904317_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904318_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904318_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904319_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904319_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904320_97_APL");
//low_set_add_str_unique(DMLValSet,"18AM904320_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904401_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904402_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904442_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904609_26_APL");
//low_set_add_str_unique(DMLValSet,"18AM904617_61_APL");
//low_set_add_str_unique(DMLValSet,"18AM904617_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904619_61_APL");
//low_set_add_str_unique(DMLValSet,"18AM904619_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904620_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904621_61_APL");
//low_set_add_str_unique(DMLValSet,"18AM904621_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904622_61_APL");
//low_set_add_str_unique(DMLValSet,"18AM904622_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904623_73_APL");
//low_set_add_str_unique(DMLValSet,"18AM904623_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904624_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904624_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904625_30_APL");
//low_set_add_str_unique(DMLValSet,"18AM904625_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904626_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904626_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904627_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904627_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904628_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904628_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904629_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904629_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904630_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904630_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904631_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904632_55_APL");
//low_set_add_str_unique(DMLValSet,"18AM904632_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904635_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904635_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904636_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904636_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904637_94_APL");
//low_set_add_str_unique(DMLValSet,"18AM904637_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904753_61_APL");
//low_set_add_str_unique(DMLValSet,"18AM904753_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904756_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM904756_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904757_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904758_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904763_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904763_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904766_32_APL");
//low_set_add_str_unique(DMLValSet,"18AM904766_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904769_31_APL");
//low_set_add_str_unique(DMLValSet,"18AM904771_46_APL");
//low_set_add_str_unique(DMLValSet,"18AM904772_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904772_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904773_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904775_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904783_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904783_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904784_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904787_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904787_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904789_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904789_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904791_49_APL");
//low_set_add_str_unique(DMLValSet,"18AM904791_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904792_49_APL");
//low_set_add_str_unique(DMLValSet,"18AM904792_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904793_49_APL");
//low_set_add_str_unique(DMLValSet,"18AM904793_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904794_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904794_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904795_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904795_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904796_42_APL");
//low_set_add_str_unique(DMLValSet,"18AM904796_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904797_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904798_29_APL");
//low_set_add_str_unique(DMLValSet,"18AM904798_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904800_29_APL");
//low_set_add_str_unique(DMLValSet,"18AM904800_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904801_29_APL");
//low_set_add_str_unique(DMLValSet,"18AM904801_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904803_24_APL");
//low_set_add_str_unique(DMLValSet,"18AM904803_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904804_24_APL");
//low_set_add_str_unique(DMLValSet,"18AM904804_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904806_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904807_58_APL");
//low_set_add_str_unique(DMLValSet,"18AM904807_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904808_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904808_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904808_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904809_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904809_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904810_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904810_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904817_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904817_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904820_16_APL");
//low_set_add_str_unique(DMLValSet,"18AM904820_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904821_72_APL");
//low_set_add_str_unique(DMLValSet,"18AM904821_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904823_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904823_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904823_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904824_47_APL");
//low_set_add_str_unique(DMLValSet,"18AM904824_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904825_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904831_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904831_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904833_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904833_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904837_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904837_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904839_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904840_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904841_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904842_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904842_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904846_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904846_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904905_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM904969_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_PR03_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_PR04_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_PR05_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_PR06_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_PR07_APL");
//low_set_add_str_unique(DMLValSet,"18AM904973_PR08_APL");
//low_set_add_str_unique(DMLValSet,"18AM904980_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904980_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904981_54_APL");
//low_set_add_str_unique(DMLValSet,"18AM904981_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904982_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904982_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904983_69_APL");
//low_set_add_str_unique(DMLValSet,"18AM904983_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904984_55_APL");
//low_set_add_str_unique(DMLValSet,"18AM904984_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904985_55_APL");
//low_set_add_str_unique(DMLValSet,"18AM904985_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904986_55_APL");
//low_set_add_str_unique(DMLValSet,"18AM904986_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904987_55_APL");
//low_set_add_str_unique(DMLValSet,"18AM904987_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904988_68_APL");
//low_set_add_str_unique(DMLValSet,"18AM904989_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904989_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904990_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM904990_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM904999_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905003_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905003_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905006_83_APL");
//low_set_add_str_unique(DMLValSet,"18AM905006_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905010_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905011_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905015_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905025_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905053_PR01_APL");
//low_set_add_str_unique(DMLValSet,"18AM905053_PR02_APL");
//low_set_add_str_unique(DMLValSet,"18AM905053_PR03_APL");
//low_set_add_str_unique(DMLValSet,"18AM905053_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905071_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905093_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905119_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905122_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905123_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905129_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905170_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905220_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905221_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905224_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905270_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905293_60_APL");
//low_set_add_str_unique(DMLValSet,"18AM905293_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905357_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905462_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905467_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905497_88_APL");
//low_set_add_str_unique(DMLValSet,"18AM905534_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905557_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905629_26_APL");
//low_set_add_str_unique(DMLValSet,"18AM905629_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905651_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905652_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905745_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905767_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905811_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905811_PR_APL");
//low_set_add_str_unique(DMLValSet,"18AM905844_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM905913_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM906003_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM906005_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM906137_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM906174_00_APL");
//low_set_add_str_unique(DMLValSet,"18AM906190_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901009_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901047_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901054_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901084_26_APL");
//low_set_add_str_unique(DMLValSet,"19AM901118_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901122_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901125_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901148_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901161_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901185_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901208_PR_APL");
//low_set_add_str_unique(DMLValSet,"19AM901317_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901318_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901319_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901320_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901321_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901322_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901323_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901324_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901325_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901394_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901395_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901417_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901455_PR_APL");
//low_set_add_str_unique(DMLValSet,"19AM901466_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM901508_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901590_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901592_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901593_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901594_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901630_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901714_15_APL");
//low_set_add_str_unique(DMLValSet,"19AM901737_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901746_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901788_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901806_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901949_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM901991_26_APL");
//low_set_add_str_unique(DMLValSet,"19AM902024_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902025_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902067_26_APL");
//low_set_add_str_unique(DMLValSet,"19AM902077_26_APL");
//low_set_add_str_unique(DMLValSet,"19AM902082_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902115_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902116_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902117_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902152_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902178_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902179_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902185_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902186_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902189_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902197_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902438_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902443_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902601_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902602_26_APL");
//low_set_add_str_unique(DMLValSet,"19AM902605_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902649_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902677_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902689_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902716_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902721_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902729_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902734_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902751_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902752_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902760_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902772_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902777_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902792_26_APL");
//low_set_add_str_unique(DMLValSet,"19AM902799_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902821_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902825_26_APL");
//low_set_add_str_unique(DMLValSet,"19AM902827_54_APL");
//low_set_add_str_unique(DMLValSet,"19AM902828_54_APL");
//low_set_add_str_unique(DMLValSet,"19AM902829_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902836_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902843_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902847_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902849_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902853_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902855_54_APL");
//low_set_add_str_unique(DMLValSet,"19AM902857_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902862_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902865_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902866_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902889_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902896_PR_APL");
//low_set_add_str_unique(DMLValSet,"19AM902900_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902901_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902902_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902903_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902905_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902908_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902911_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902912_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902913_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902915_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902917_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902918_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902921_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902922_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902931_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902946_PR01_APL");
//low_set_add_str_unique(DMLValSet,"19AM902946_PR02_APL");
//low_set_add_str_unique(DMLValSet,"19AM902946_PR_APL");
//low_set_add_str_unique(DMLValSet,"19AM902952_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902956_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902958_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902959_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902960_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902962_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902970_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902972_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902973_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902975_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM902976_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902978_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902979_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902981_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902982_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902992_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902996_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM902997_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903000_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903008_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903016_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903045_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903057_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903059_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903063_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903081_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903083_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903112_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903119_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903163_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903174_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903206_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903247_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903250_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903251_54_APL");
//low_set_add_str_unique(DMLValSet,"19AM903289_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903293_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903306_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903319_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903321_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903326_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903329_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903332_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903335_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903336_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903379_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903394_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903396_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903416_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903479_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903483_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903513_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903565_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903577_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903581_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903595_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903601_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903658_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903659_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903691_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903749_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903751_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903765_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903773_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903782_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903788_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM903799_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM903902_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904027_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904054_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904075_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM904076_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM904103_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904160_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM904217_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904226_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904265_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904298_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904319_PR_APL");
//low_set_add_str_unique(DMLValSet,"19AM904349_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904399_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904400_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904405_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904446_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904467_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904497_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904505_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM904508_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904519_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904525_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904526_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM904562_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904579_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904608_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904634_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904662_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904685_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904716_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM904732_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904745_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904753_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904758_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904792_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904801_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904870_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904884_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904890_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904898_40_APL");
//low_set_add_str_unique(DMLValSet,"19AM904925_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904926_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904928_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904931_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904933_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904935_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904941_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904957_26_APL");
//low_set_add_str_unique(DMLValSet,"19AM904967_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904970_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904984_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM904991_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905002_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905007_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905008_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905009_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905010_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905013_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905015_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905051_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905059_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905062_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM905078_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905082_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905100_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905117_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM905120_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905121_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905122_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905126_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905131_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905138_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905179_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905180_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905181_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905182_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905208_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905214_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905218_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905258_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM905285_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905299_60_APL");
//low_set_add_str_unique(DMLValSet,"19AM905311_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905325_00_APL");
//low_set_add_str_unique(DMLValSet,"19AM905356_00_APL");

	
	t5CheckDstat(oiSqlCreateSelect(&InputDMLSqlPtr));
	t5CheckDstat(oiSqlWhereEQ(InputDMLSqlPtr,ProjectNameAttr,"5442"));
	t5CheckDstat(oiSqlWhereAND(InputDMLSqlPtr));
	t5CheckDstat(oiSqlWhereLike(InputDMLSqlPtr,WbsIDAttr,DmlNoRule));
	t5CheckDstat(oiSqlWhereAND(InputDMLSqlPtr));
	t5CheckDstat(oiSqlWhereIsNotNull(InputDMLSqlPtr,ClosureTimeStampAttr));
	t5CheckMfail(QueryDbObject(CmTaskItClass,InputDMLSqlPtr,-1,TRUE,SC_SCOPE_OF_SESSION,&DMLTaskResultSO,mfail));
	t5CheckDstat(oiSqlPrint(InputDMLSqlPtr ));
	printf("\n No:of Dml's are :- %d\n",setSize(DMLTaskResultSO));fflush(stdout);
	if(InputDMLSqlPtr) oiSqlDispose(InputDMLSqlPtr); InputDMLSqlPtr = NULL;
	if(setSize(DMLTaskResultSO)>0)
	{
		
		for(dmlCnt=0;dmlCnt<setSize(DMLTaskResultSO);dmlCnt++)
		{
				
			APLDmlObjPtr=NULL;
			APLDmlObjPtr=setGet(DMLTaskResultSO,dmlCnt);

			aplDmlNo=NULL;
			aplDmlNoDup=NULL;

			if(dstat= objGetAttribute(APLDmlObjPtr,WbsIDAttr,&aplDmlNo)) goto  EXIT ;
			if (!nlsIsStrNull(aplDmlNo)) aplDmlNoDup=nlsStrDup(aplDmlNo);
		
			aplDmlCnt++;
			
			if (!nlsIsStrNull(integerStr)) nlsStrFree(integerStr);
			integerStr = nlsStrAlloc(100);
			sprintf(integerStr, "%d", aplDmlCnt);

			if(!nlsIsStrNull(integerStr))
			{
				fprintf(fp_PlantDmlList,integerStr);						
				fprintf(fp_PlantDmlList,"^");
			}
			else
			{
				fprintf(fp_PlantDmlList," ^");
			}

			if(!nlsIsStrNull(aplDmlNoDup))
			{
				fprintf(fp_PlantDmlList,aplDmlNoDup);						
				fprintf(fp_PlantDmlList,"^");
			}
			else
			{
				fprintf(fp_PlantDmlList," ^");
			}
			
			fprintf(fp_PlantDmlList,"\n");
		}

	
		dmlCnt=0;
		for(dmlCnt=0;dmlCnt<setSize(DMLTaskResultSO);dmlCnt++)
		{
			TaskObjPtr=NULL;
			TaskObjPtr=setGet(DMLTaskResultSO,dmlCnt);	
			
			APLDMLObjs=NULL;
			if(dstat = ExpandObject5("CmPlRvRv",TaskObjPtr,"CMPlanRollsUpInTo",SC_SCOPE_OF_SESSION,&APLDMLObjs,mfail)) goto EXIT;

			printf("\n  APL DML  fetched : %d  \n",setSize(APLDMLObjs));fflush(stdout);
			if(setSize(APLDMLObjs)>0)
			{
				aplDMLObjsClass=NULL;
				aplDMLObjsClassDup=NULL;

				aplDmlNoEcnType=NULL;
				aplDmlNoEcnTypeDup=NULL;

				DmlObjPtr=setGet(APLDMLObjs,0);
				
				
				if(dstat = objGetAttribute(DmlObjPtr,ClassAttr,&aplDMLObjsClass)) goto EXIT;
				if(!nlsIsStrNull(aplDMLObjsClass)) aplDMLObjsClassDup=nlsStrDup(aplDMLObjsClass);
				
				aplDmlNo=NULL;
				aplDmlNoDup=NULL;

				if(dstat= objGetAttribute(DmlObjPtr,WbsIDAttr,&aplDmlNo)) goto  EXIT ;
				if (!nlsIsStrNull(aplDmlNo)) aplDmlNoDup=nlsStrDup(aplDmlNo);

				if(nlsStrCmp(aplDMLObjsClassDup,"CmChNtIt")==0)
				{

					ErcDMLObjs=NULL;
					if(dstat = ExpandObject5("CmPlRvRv",DmlObjPtr,"CMPlanRollsUpInTo",SC_SCOPE_OF_SESSION,&ErcDMLObjs,mfail)) goto EXIT;

					printf("\n  ERC DML  fetched : %d  \n",setSize(ErcDMLObjs));fflush(stdout);
					if(setSize(ErcDMLObjs)>0)
					{
						DMLObjsClass=NULL;
						DMLObjsClassDup=NULL;
						
						
						if(dstat = objGetAttribute(setGet(ErcDMLObjs,0),ClassAttr,&DMLObjsClass)) goto EXIT;
						if(!nlsIsStrNull(DMLObjsClass)) DMLObjsClassDup=nlsStrDup(DMLObjsClass);						

						if(nlsStrCmp(DMLObjsClassDup,"CmChRqIt")==0)
						{

							if(dstat= objGetAttribute(setGet(ErcDMLObjs,0),t5DmlTypeAttr,&aplDmlNoEcnType)) goto  EXIT ;
							if (!nlsIsStrNull(aplDmlNoEcnType)) aplDmlNoEcnTypeDup=nlsStrDup(aplDmlNoEcnType);
							printf("\n After mod aplDmlNoEcnTypeDup %s\n",aplDmlNoEcnTypeDup);fflush(stdout);
						}
					}
					else
					{
						if(dstat= objGetAttribute(DmlObjPtr,t5EcnTypeAttr,&aplDmlNoEcnType)) goto  EXIT ;
						if (!nlsIsStrNull(aplDmlNoEcnType)) aplDmlNoEcnTypeDup=nlsStrDup(aplDmlNoEcnType);
						printf("\n After mod aplDmlNoEcnTypeDup %s\n",aplDmlNoEcnTypeDup);fflush(stdout);
					
					}

					apldmlLCSState=NULL;
					apldmlLCSStateDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,LifeCycleStateAttr,&apldmlLCSState)) goto  EXIT ;
					if (!nlsIsStrNull(apldmlLCSState)) apldmlLCSStateDup=nlsStrDup(apldmlLCSState);

					aplDmlNoRlz=NULL;
					aplDmlNoRlzDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,ActualRelDateAttr,&aplDmlNoRlz)) goto  EXIT ;
					if (!nlsIsStrNull(aplDmlNoRlz)) aplDmlNoRlzDup=nlsStrDup(aplDmlNoRlz);

					aplDmlNoSTDSIRlz=NULL;
					aplDmlNoSTDSIRlzDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,ClosureTimeStampAttr,&aplDmlNoSTDSIRlz)) goto  EXIT ;
					if (!nlsIsStrNull(aplDmlNoSTDSIRlz)) aplDmlNoSTDSIRlzDup=nlsStrDup(aplDmlNoSTDSIRlz);

				}
			}

			if (!nlsIsStrNull(aplDmlNoEcnTypeDup))
			{
						aplAMDmlCreator=NULL;
						aplAMDmlCreatorDup=NULL;

						if(dstat= objGetAttribute(DmlObjPtr,CreatorAttr,&aplAMDmlCreator)) goto  EXIT ;
						if (!nlsIsStrNull(aplAMDmlCreator)) aplAMDmlCreatorDup=nlsStrDup(aplAMDmlCreator);

						printf("\n aplAMDmlCreatorDup %s\n",aplAMDmlCreatorDup);fflush(stdout);

						aplAMDmlSynp=NULL;
						aplAMDmlSynpDup=NULL;

						if(dstat= objGetAttribute(DmlObjPtr,WbsNameAttr,&aplAMDmlSynp)) goto  EXIT ;
						if (!nlsIsStrNull(aplAMDmlSynp)) aplAMDmlSynpDup=nlsStrDup(aplAMDmlSynp);

						printf("\n Before aplAMDmlSynpDup %s\n",aplAMDmlSynpDup);fflush(stdout);

						if (!nlsIsStrNull(aplAMDmlSynpDup))
						{
							low_strrpl(aplAMDmlSynpDup, '\n', ' ');
							low_strrpl(aplAMDmlSynpDup, ',', ';');
							low_strrpl(aplAMDmlSynpDup, '&', 'and');

							aplAMDmlSynpDup=subString(aplAMDmlSynpDup,0,240);
						}

						printf("\n After aplAMDmlSynpDup %s\n",aplAMDmlSynpDup);fflush(stdout);

						aplAMDmlProjCode=NULL;
						aplAMDmlProjCodeDup=NULL;

						if(dstat= objGetAttribute(DmlObjPtr,ProjectNameAttr,&aplAMDmlProjCode)) goto  EXIT ;
						if (!nlsIsStrNull(aplAMDmlProjCode)) aplAMDmlProjCodeDup=nlsStrDup(aplAMDmlProjCode);

						printf("\n aplAMDmlProjCodeDup %s\n",aplAMDmlProjCodeDup);fflush(stdout);

						aplAMDmlDriverVC=NULL;
						aplAMDmlDriverVCDup=NULL;

						if(dstat= objGetAttribute(DmlObjPtr,t5DriverVCNoAttr,&aplAMDmlDriverVC)) goto  EXIT ;
						if (!nlsIsStrNull(aplAMDmlDriverVC)) aplAMDmlDriverVCDup=nlsStrDup(aplAMDmlDriverVC);

						printf("\n aplAMDmlDriverVCDup %s\n",aplAMDmlDriverVCDup);fflush(stdout);

						aplAMDmlDesc=NULL;
						aplAMDmlDescDup=NULL;

						if(dstat= objGetAttribute(DmlObjPtr,WbsDescriptionAttr,&aplAMDmlDesc)) goto  EXIT ;
						if (!nlsIsStrNull(aplAMDmlDesc)) aplAMDmlDescDup=nlsStrDup(aplAMDmlDesc);

						printf("\n Before aplAMDmlDescDup %s\n",aplAMDmlDescDup);fflush(stdout);

						if (!nlsIsStrNull(aplAMDmlDescDup))
						{
							low_strrpl(aplAMDmlDescDup, '\n', ' ');
							low_strrpl(aplAMDmlDescDup, ',', ';');
							low_strrpl(aplAMDmlDescDup, '&', 'and');

							aplAMDmlDescDup=subString(aplAMDmlDescDup,0,240);
						}

						printf("\n After aplAMDmlDescDup %s\n",aplAMDmlDescDup);fflush(stdout);

						aplAMLastUpDt=NULL;
						aplAMLastUpDtDup=NULL;

						if(dstat= objGetAttribute(DmlObjPtr,DateUpdatedAttr,&aplAMLastUpDt)) goto  EXIT ;
						if (!nlsIsStrNull(aplAMLastUpDt)) aplAMLastUpDtDup=nlsStrDup(aplAMLastUpDt);

						printf("\n aplAMLastUpDtDup %s\n",aplAMLastUpDtDup);fflush(stdout);

						aplAMLastUpBy=NULL;
						aplAMLastUpByDup=NULL;

						if(dstat= objGetAttribute(DmlObjPtr,t5LastModByAttr,&aplAMLastUpBy)) goto  EXIT ;
						if (!nlsIsStrNull(aplAMLastUpBy)) aplAMLastUpByDup=nlsStrDup(aplAMLastUpBy);

						printf("\n aplAMLastUpByDup %s\n",aplAMLastUpByDup);fflush(stdout);

						aplAMDRStatus=NULL;
						aplAMDRStatusDup=NULL;

						if(dstat= objGetAttribute(DmlObjPtr,t5PartStatusAttr,&aplAMDRStatus)) goto  EXIT ;
						if (!nlsIsStrNull(aplAMDRStatus)) aplAMDRStatusDup=nlsStrDup(aplAMDRStatus);

						printf("\n aplAMDRStatusDup %s\n",aplAMDRStatusDup);fflush(stdout);
				
				
			}


			taskNumber=NULL;
			taskNumberDup=NULL;
			
			if(dstat = objGetAttribute(TaskObjPtr,WbsIDAttr,&taskNumber))goto EXIT;
			if(!nlsIsStrNull(taskNumber)) taskNumberDup=nlsStrDup(taskNumber);
			printf("\n  taskNumberDup:%s \n",taskNumberDup); fflush(stdout);

			taskDesgGrpDup=NULL;
			taskDesgGrp=NULL;
			
			if(dstat = objGetAttribute(TaskObjPtr,t5DesignGroupAttr,&taskDesgGrp))goto EXIT;
			if(!nlsIsStrNull(taskDesgGrp)) taskDesgGrpDup=nlsStrDup(taskDesgGrp);
			printf("\n  taskDesgGrpDup:%s \n",taskDesgGrpDup); fflush(stdout);

			taskLifeCycle=NULL;
			taskLifeCycleDup=NULL;
			
			if(dstat = objGetAttribute(TaskObjPtr,LifeCycleStateAttr,&taskLifeCycle))goto EXIT;
			if(!nlsIsStrNull(taskLifeCycle)) taskLifeCycleDup=nlsStrDup(taskLifeCycle);
			printf("\n  taskLifeCycleDup:%s \n",taskLifeCycleDup); fflush(stdout);

			aplDmltaskRlzDt=NULL;
			aplDmltaskRlzDtDup=NULL;

			if(dstat= objGetAttribute(TaskObjPtr,ActualRelDateAttr,&aplDmltaskRlzDt)) goto  EXIT ;
			if (!nlsIsStrNull(aplDmltaskRlzDt)) aplDmltaskRlzDtDup=nlsStrDup(aplDmltaskRlzDt);

			aplDmlTaskSTDSIRlzDt=NULL;
			aplDmlTaskSTDSIRlzDtDup=NULL;

			if(dstat= objGetAttribute(TaskObjPtr,ClosureTimeStampAttr,&aplDmlTaskSTDSIRlzDt)) goto  EXIT ;
			if (!nlsIsStrNull(aplDmlTaskSTDSIRlzDt)) aplDmlTaskSTDSIRlzDtDup=nlsStrDup(aplDmlTaskSTDSIRlzDt);
			
			setOfAssmblyObjs=NULL;
			
			if(dstat=(ExpandObject5(CmRsltsClass,TaskObjPtr,"CMPrdPlnResultsInBusItem",SC_SCOPE_OF_SESSION,&setOfAssmblyObjs,mfail)));
			printf("\n Size of setOfAssmblyObjs is %d \n", setSize(setOfAssmblyObjs));fflush(stdout);

				if(setSize(setOfAssmblyObjs)>0)
				{
					partCnt=0;
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

						PartDegnGrp=NULL;
						PartDegnGrpDup=NULL;

						if(dstat = objGetAttribute(TCPartObjP,t5DesignGroupAttr,&PartDegnGrp))goto EXIT;
						if(!nlsIsStrNull(PartDegnGrp)) PartDegnGrpDup=nlsStrDup(PartDegnGrp);
						printf("\n  TC Part Number Design Grp:%s \n",PartDegnGrpDup); fflush(stdout);

						PartObid=NULL;
						PartObidDup=NULL;

						if(dstat = objGetAttribute(TCPartObjP,OBIDAttr,&PartObid))goto EXIT;
						if(!nlsIsStrNull(PartObid)) PartObidDup=nlsStrDup(PartObid);
						printf("\n  TC Part Number OBID to query in g2asmpos for optional cs:%s \n",PartObidDup); fflush(stdout);

						if(nlsStrStr(PartOrgIDDup,"APL")!=NULL)
						{
							if(nlsStrStr(PartNumberValDup,"R40")!=NULL)
							{
								printf("\n  R40 part found so redirecting to exception log :[%s]:[%s]:[%s] \n",PartNumberValDup,PartNumberRevDup,PartNumberSeqDup); fflush(stdout);
								fprintf(fp_DMLErrorLog,"\n R40 part found :[%s]:[%s]:[%s] \n",PartNumberValDup,PartNumberRevDup,PartNumberSeqDup);
								continue;
							
							}
							if(nlsStrStr(PartNumberValDup,"98R")!=NULL)
							{
								if(nlsStrCmp(PartDegnGrpDup,"98")==0)
								{
									printf("\n  98R part found  :[%s]:[%s]:[%s] \n",PartNumberValDup,PartNumberRevDup,PartNumberSeqDup); fflush(stdout);
									aplRestAMDMLgrp98Found++;
								}
							
							}
						}

						PartDrwInd=NULL;
						PartDrwIndDup=NULL;

						if(dstat = objGetAttribute(TCPartObjP,t5DrawingIndAttr,&PartDrwInd))goto EXIT;
						if(!nlsIsStrNull(PartDrwInd))PartDrwIndDup=nlsStrDup(PartDrwInd);
						printf("\n TC Part Drawing ID:%s \n",PartDrwIndDup); fflush(stdout);

						PartColourInd=NULL;
						PartColourIndDup=NULL;

						if(dstat = objGetAttribute(TCPartObjP,t5ColourIndAttr,&PartColourInd))goto EXIT;
						if(!nlsIsStrNull(PartColourInd))PartColourIndDup=nlsStrDup(PartColourInd);
						printf("\n TC Part PartColourIndDup:%s \n",PartColourIndDup); fflush(stdout);

						if(!nlsIsStrNull(PartColourIndDup))
						{
							if(nlsStrCmp(PartColourIndDup,"C")==0)
							{
								printf("\n TC Part PartColourIndDup:%s:[%s]:[%s]:[%s] is found.Hence Skipped \n",PartColourIndDup,PartNumberValDup,PartNumberRevDup,PartNumberSeqDup); fflush(stdout);
							}
						
						}

						if(!nlsIsStrNull(aplDmlNoDup))
						{
							printf("\n 11111 DML"); fflush(stdout);
							fprintf(fp_PlantDMLTaskPartInfo,aplDmlNoDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}
						printf("\n 11111"); fflush(stdout);

						if(!nlsIsStrNull(apldmlLCSStateDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,apldmlLCSStateDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}
						printf("\n 222"); fflush(stdout);

						if(!nlsIsStrNull(aplDmlNoRlzDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplDmlNoRlzDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo,"-^");
						}
						printf("\n 333"); fflush(stdout);

						if(!nlsIsStrNull(aplDmlNoSTDSIRlzDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplDmlNoSTDSIRlzDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo,"-^");
						}

						printf("\n 444"); fflush(stdout);
					
						if(!nlsIsStrNull(taskNumberDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,taskNumberDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}
						printf("\n 555"); fflush(stdout);

						if(!nlsIsStrNull(taskDesgGrpDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,taskDesgGrpDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(taskLifeCycleDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,taskLifeCycleDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}
						printf("\n 777"); fflush(stdout);

						if(!nlsIsStrNull(aplDmltaskRlzDtDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplDmltaskRlzDtDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo,"-^");
						}

						printf("\n 888"); fflush(stdout);

						if(!nlsIsStrNull(aplDmlTaskSTDSIRlzDtDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplDmlTaskSTDSIRlzDtDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo,"-^");
						}
						
						printf("\n 999"); fflush(stdout);

						if(!nlsIsStrNull(PartNumberValDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,PartNumberValDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}
						printf("\n 2323"); fflush(stdout);

						if(!nlsIsStrNull(PartNumberRevDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,PartNumberRevDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(PartNumberSeqDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,PartNumberSeqDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(aplAMDmlCreatorDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplAMDmlCreatorDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(aplAMDmlSynpDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplAMDmlSynpDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(aplAMDmlProjCodeDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplAMDmlProjCodeDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(aplAMDmlDriverVCDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplAMDmlDriverVCDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}


						if(!nlsIsStrNull(aplAMDmlDescDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplAMDmlDescDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(aplAMLastUpDtDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplAMLastUpDtDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo,"-^");
						}

						if(!nlsIsStrNull(aplAMLastUpByDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplAMLastUpByDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(aplDmlNoEcnTypeDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplDmlNoEcnTypeDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}

						if(!nlsIsStrNull(aplAMDRStatusDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplAMDRStatusDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}
						printf("\n trty"); fflush(stdout);

						if(!nlsIsStrNull(aplReasonDup))
						{
							fprintf(fp_PlantDMLTaskPartInfo,aplReasonDup);						
							fprintf(fp_PlantDMLTaskPartInfo,"^");
						}
						else
						{
							fprintf(fp_PlantDMLTaskPartInfo," ^");
						}
	
						fprintf(fp_PlantDMLTaskPartInfo,"\n");

						printf("\n 111aplRestAMDMLgrp98Found...%d\n",aplRestAMDMLgrp98Found);fflush(stdout);
						
						if (nlsStrStr(PartOrgIDDup,"APL")!=NULL)
						{
							printf("\n Calling GetTCPartInfoForAPL for APL.."); fflush(stdout);
							t5CheckMfail(GetTCPartInfoForAPL(TCPartObjP,PartOrgIDDup,csIndicator,StoreLocation,configCtxt,fp_DMLPartInfo,mfail));
							
						}
						else
						{
							if(nlsStrCmp(PartOrgIDDup,"ERC")==0)
							{
								printf("\n Calling GetTCPartInfoForAPL for ERC.."); fflush(stdout);
								t5CheckMfail(GetTCPartInfoForAPL(TCPartObjP,PartOrgIDDup,csIndicator,StoreLocation,configCtxt,fp_DMLPartInfo,mfail));
							}

						}

					
					}
				
				
				}	

			
		}
	
	}

	if(fp_PlantDmlList) fclose(fp_PlantDmlList); fp_PlantDmlList=NULL;
	if(fp_PlantDMLTaskPartInfo) fclose(fp_PlantDMLTaskPartInfo); fp_PlantDMLTaskPartInfo=NULL;
	if(fp_DMLPartInfo) fclose(fp_DMLPartInfo); fp_DMLPartInfo=NULL;
	if(fp_DMLErrorLog) fclose(fp_DMLErrorLog); fp_DMLErrorLog=NULL;
	//if(fp_DMLBomList) fclose(fp_DMLBomList); fp_DMLBomList=NULL;
	//if(fp_DMLOPCSInfo) fclose(fp_DMLOPCSInfo); fp_DMLOPCSInfo=NULL;
	
	printf("Incremental Data Download complete...");fflush(stdout);

CLEANUP:
		t5PrintCleanUpModName;
		printf("Inside CLEANUP...");fflush(stdout);


EXIT:
		t5CheckDstatAndReturn;
	printf("Inside EXIT...");fflush(stdout);
}
;

