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
status GetTCPartInfoForAPL(ObjectPtr TCPartObjP,FILE* fp,integer* mfail )
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

	printf("\n Calling GetTCPartInfoForAPL......");fflush(stdout);

	
	if(dstat=objGetAttribute(TCPartObjP,PartNumberAttr,&t5PartNumber))goto EXIT;
	if(!nlsIsStrNull(t5PartNumber))t5PartNumberDup=nlsStrDup(t5PartNumber);

	if(dstat=objGetAttribute(TCPartObjP,RevisionAttr,&t5PartNumberRev))goto EXIT;
	if(!nlsIsStrNull(t5PartNumberRev))t5PartNumberRevDup=nlsStrDup(t5PartNumberRev);

	if(dstat=objGetAttribute(TCPartObjP,SequenceAttr,&t5PartNumberSeq))goto EXIT;
	if(!nlsIsStrNull(t5PartNumberSeq))t5PartNumberSeqDup=nlsStrDup(t5PartNumberSeq);

	if(dstat=objGetAttribute(TCPartObjP,"t5CarMakeBuyIndicator",&t5CarMakeBuyInd))goto EXIT;
	if(!nlsIsStrNull(t5CarMakeBuyInd))t5CarMakeBuyIndDup=nlsStrDup(t5CarMakeBuyInd);

	if(dstat=objGetAttribute(TCPartObjP,"t5CarStoreLocation",&t5CarStLoc))goto EXIT;
	if(!nlsIsStrNull(t5CarStLoc))t5CarStLocDup=nlsStrDup(t5CarStLoc);	

	
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

	printf("\n PartDetials :%s,%s,%s,%s,%s,%s,%s",t5PartNumberDup,t5PartNumberRevDup,t5PartNumberSeqDup,
		t5CarMakeBuyIndDup,t5CarStLocDup,t5PartNumberLCSDup,t5PartOrgIDDup);fflush(stdout);


	t5CheckMfail(IntSetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObj,mfail));
	t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass, genContextObj, &contextObj, mfail));
	t5CheckDstat(objSetAttribute(contextObj,CfgItemIdAttr,"GlobalCtxt"));
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
	t5CheckDstat(objSetAttribute(contextObj,CfgItemIdAttr,"GlobalCtxt"));
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

		

		if (nlsStrStr(t5PartOrgIDDup,"APL")!=NULL)
		{
			if(dstat =objGetAttribute(TCPartObjP,NomenclatureAttr,&DescriptionVal))goto EXIT;
			if(!nlsIsStrNull(DescriptionVal))	 DescriptionValDup=nlsStrDup(DescriptionVal);
			if(!nlsIsStrNull(DescriptionValDup)) DescriptionValDup=low_strssra(DescriptionValDup,"\n"," ");	

			if(!nlsIsStrNull(DescriptionValDup))
			{
				fprintf(fp,"%s",DescriptionValDup);		//DESCRPTION
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}
		
			if(dstat =objGetAttribute(TCPartObjP,PartTypeAttr,&PartTypeVal))goto EXIT;
			if(!nlsIsStrNull(PartTypeVal)) PartTypeValDup=nlsStrDup(PartTypeVal);

			if(!nlsIsStrNull(PartTypeValDup))
			{
				fprintf(fp,"1^ASSY_TYPE_UA^%s^",PartTypeValDup);//PARTTYPE;
			}
			else
			{
				fprintf(fp,"1^ASSY_TYPE_UA^%s^","");//(PARTTYPE);
			}
		
			if(dstat =objGetAttribute(TCPartObjP,ProjectNameAttr,&ProjectCodeDupS))goto EXIT;
			if(!nlsIsStrNull(ProjectCodeDupS)) ProjectCodeS=nlsStrDup(ProjectCodeDupS);

			if(!nlsIsStrNull(ProjectCodeS))		//(PROJECT CODE);
			{
				fprintf(fp,"%s",ProjectCodeS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}
		
			if(dstat =objGetAttribute(TCPartObjP,t5DesignGroupAttr,&DesignGroupDupS))goto EXIT;
			if(!nlsIsStrNull(DesignGroupDupS)) DesignGroupS=nlsStrDup(DesignGroupDupS);

			if(!nlsIsStrNull(DesignGroupS))			//(Design GRP);
			{
				fprintf(fp,"%s",DesignGroupS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5DocRemarksAttr,&ModDescriptionDupS))goto EXIT;
			if(!nlsIsStrNull(ModDescriptionDupS)) ModDescriptionS=nlsStrDup(ModDescriptionDupS);
			if(!nlsIsStrNull(ModDescriptionS)) ModDescriptionS=low_strssra(ModDescriptionS,"\n"," ");

			if(!nlsIsStrNull(ModDescriptionS))
			{
				fprintf(fp,"%s",ModDescriptionS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}
			
			if(dstat =objGetAttribute(TCPartObjP,t5DrawingNoAttr,&DrawingNoDupS))goto EXIT;
			if(!nlsIsStrNull(DrawingNoDupS)) DrawingNoS=nlsStrDup(DrawingNoDupS);

			if(!nlsIsStrNull(DrawingNoS))
			{
				fprintf(fp,"%s",DrawingNoS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}
			
			if(dstat =objGetAttribute(TCPartObjP,t5DrawingIndAttr,&DrawingIndDupS))goto EXIT;
			if(!nlsIsStrNull(DrawingIndDupS)) DrawingIndS=nlsStrDup(DrawingIndDupS);

			if(!nlsIsStrNull(DrawingIndS))
			{
				fprintf(fp,"%s",DrawingIndS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5MatlClassAttr,&MaterialDupS))goto EXIT;
			if(!nlsIsStrNull(MaterialDupS)) MaterialS=nlsStrDup(MaterialDupS);

			if(!nlsIsStrNull(MaterialS)) MaterialS=low_strssra(MaterialS,"\n"," ");

			if(!nlsIsStrNull(MaterialS))
			{
				fprintf(fp,"%s",MaterialS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5MaterialAttr,&MaterialInDrwDupS))goto EXIT;
			if(!nlsIsStrNull(MaterialInDrwDupS)) MaterialInDrwS=nlsStrDup(MaterialInDrwDupS);

			if(!nlsIsStrNull(MaterialInDrwS)) MaterialInDrwS=low_strssra(MaterialInDrwS,"\n"," ");

			if(!nlsIsStrNull(MaterialInDrwS))
			{
				fprintf(fp,"%s",MaterialInDrwS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5MThicknessAttr,&MaterialThickNessDupS))goto EXIT;
			if(!nlsIsStrNull(MaterialThickNessDupS)) MaterialThickNessS=nlsStrDup(MaterialThickNessDupS);

			if(!nlsIsStrNull(MaterialThickNessS))
			{
				fprintf(fp,"%s",MaterialThickNessS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			
			partMasterSO=NULL;
			partMasterRelSO=NULL;
			MasterObjOP=NULL;

			t5CheckMfail(ExpandObject2(ItemRevClass,TCPartObjP,"ItemMstrForStrucBIRev",SC_SCOPE_OF_SESSION,&partMasterSO,&partMasterRelSO,mfail)) ;
			if(setSize(partMasterSO)>0)
			{
				MasterObjOP=setGet(partMasterSO,0);
			}

			if(dstat =objGetAttribute(MasterObjOP,t5LeftRhAttr,&LeftRhDupS))goto EXIT;
			if(!nlsIsStrNull(LeftRhDupS)) LeftRhS=nlsStrDup(LeftRhDupS);

			if(!nlsIsStrNull(LeftRhS))
			{
				fprintf(fp,"%s",LeftRhS);
				fprintf(fp,"^");
			}else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(MasterObjOP,DefaultUnitOfMeasureAttr,&UnitOfMeasureDupS))goto EXIT;
			if(!nlsIsStrNull(UnitOfMeasureDupS)) UnitOfMeasureS=nlsStrDup(UnitOfMeasureDupS);

			if(dstat =objGetAttribute(TCPartObjP,t5DsgnOwnAttr,&DeignOwnUnitDupS))goto EXIT;
			if(!nlsIsStrNull(DeignOwnUnitDupS)) DeignOwnUnitS=nlsStrDup(DeignOwnUnitDupS);

			if(!nlsIsStrNull(DeignOwnUnitS))
			{
				fprintf(fp,"%s",DeignOwnUnitS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5ModelIndicatorAttr,&ModelIndDupS))goto EXIT;
			if(!nlsIsStrNull(ModelIndDupS)) ModelIndS=nlsStrDup(ModelIndDupS);

			if(!nlsIsStrNull(ModelIndS))
			{
				fprintf(fp,"%s",ModelIndS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}


			
			if(!nlsIsStrNull(UnitOfMeasureS))
			{
				fprintf(fp,"%s",UnitOfMeasureS);
				fprintf(fp,"^");
			}else
			{
					fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5ColourIndAttr,&ColourIndDupS))goto EXIT;
			if(!nlsIsStrNull(ColourIndDupS)) ColourIndS=nlsStrDup(ColourIndDupS);

			if(!nlsIsStrNull(ColourIndS))
			{
				fprintf(fp,"%s",ColourIndS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5WeightAttr,&WeightDupS))goto EXIT;
			if(!nlsIsStrNull(WeightDupS)) WeightS=nlsStrDup(WeightDupS);

			if(!nlsIsStrNull(WeightS))
			{
				fprintf(fp,"%s",WeightS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5CMVRCertificationReqd",&t5CMVRCertificationDupS))goto EXIT;
			if(!nlsIsStrNull(t5CMVRCertificationDupS)) t5CMVRCertificationS=nlsStrDup(t5CMVRCertificationDupS);

			if(!nlsIsStrNull(t5CMVRCertificationS))
			{
				fprintf(fp,"%s",t5CMVRCertificationS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ClassificationHazardous",&t5ClassificationHazDupS))goto EXIT;
			if(!nlsIsStrNull(t5ClassificationHazDupS)) t5ClassificationHazS=nlsStrDup(t5ClassificationHazDupS);

			if(!nlsIsStrNull(t5ClassificationHazS))
			{
				fprintf(fp,"%s",t5ClassificationHazS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ColourID",&t5ColourIDDupS))goto EXIT;
			if(!nlsIsStrNull(t5ColourIDDupS)) t5ColourIDS=nlsStrDup(t5ColourIDDupS);

			if(!nlsIsStrNull(t5ColourIDS))
			{
				fprintf(fp,"%s",t5ColourIDS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ConfigID",&t5ConfigIDDupS))goto EXIT;
			if(!nlsIsStrNull(t5ConfigIDDupS)) t5ConfigIDS=nlsStrDup(t5ConfigIDDupS);

			if(!nlsIsStrNull(t5ConfigIDS))
			{
				fprintf(fp,"%s",t5ConfigIDS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Dismantable",&t5DismantableDupS))goto EXIT;
			if(!nlsIsStrNull(t5DismantableDupS)) t5DismantableS=nlsStrDup(t5DismantableDupS);

			if(!nlsIsStrNull(t5DismantableS))
			{
				fprintf(fp,"%s",t5DismantableS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5DsgnDept",&t5DsgnDeptDupS))goto EXIT;
			if(!nlsIsStrNull(t5DsgnDeptDupS)) t5DsgnDeptS=nlsStrDup(t5DsgnDeptDupS);

			if(!nlsIsStrNull(t5DsgnDeptS))
			{
				fprintf(fp,"%s",t5DsgnDeptS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5EnvelopeDimensions",&t5EnvelopeDimenDupS))goto EXIT;
			if(!nlsIsStrNull(t5EnvelopeDimenDupS)) t5EnvelopeDimenS=nlsStrDup(t5EnvelopeDimenDupS);

			if(!nlsIsStrNull(t5EnvelopeDimenS))
			{
				fprintf(fp,"%s",t5EnvelopeDimenS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5HazardousContents",&t5HazardousContDupS))goto EXIT;
			if(!nlsIsStrNull(t5HazardousContDupS)) t5HazardousContS=nlsStrDup(t5HazardousContDupS);

			if(!nlsIsStrNull(t5HazardousContS))
			{
				fprintf(fp,"%s",t5HazardousContS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5HomologationReqd",&t5HomologationReqdDupS))goto EXIT;
			if(!nlsIsStrNull(t5HomologationReqdDupS)) t5HomologationReqdS=nlsStrDup(t5HomologationReqdDupS);

			if(!nlsIsStrNull(t5HomologationReqdS))
			{
				fprintf(fp,"%s",t5HomologationReqdS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ListRecSpares",&t5ListRecSparesDups))goto EXIT;
			if(!nlsIsStrNull(t5ListRecSparesDups)) t5ListRecSparess=nlsStrDup(t5ListRecSparesDups);

			if(!nlsIsStrNull(t5ListRecSparess))
			{
				fprintf(fp,"%s",t5ListRecSparess);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5NcPartNo",&t5NcPartNoDupS))goto EXIT;
			if(!nlsIsStrNull(t5NcPartNoDupS)) t5NcPartNoS=nlsStrDup(t5NcPartNoDupS);

			if(!nlsIsStrNull(t5NcPartNoS))
			{
				fprintf(fp,"%s",t5NcPartNoS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PartCode",&t5PartCodeDupS))goto EXIT;
			if(!nlsIsStrNull(t5PartCodeDupS)) t5PartCodeS=nlsStrDup(t5PartCodeDupS);

			if(!nlsIsStrNull(t5PartCodeS))
			{
				fprintf(fp,"%s",t5PartCodeS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PartProperty",&t5PartPropertyDupS))goto EXIT;
			if(!nlsIsStrNull(t5PartPropertyDupS)) t5PartPropertyS=nlsStrDup(t5PartPropertyDupS);

			if(!nlsIsStrNull(t5PartPropertyS))
			{
				fprintf(fp,"%s",t5PartPropertyS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PartStatus",&t5PartStatusDupS))goto EXIT;
			if(!nlsIsStrNull(t5PartStatusDupS)) t5PartStatusS=nlsStrDup(t5PartStatusDupS);

			if(!nlsIsStrNull(t5PartStatusS))
			{
				fprintf(fp,"%s",t5PartStatusS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PkgStd",&t5PkgStdDupS))goto EXIT;
			if(!nlsIsStrNull(t5PkgStdDupS)) t5PkgStdS=nlsStrDup(t5PkgStdDupS);

			if(!nlsIsStrNull(t5PkgStdS))
			{
				fprintf(fp,"%s",t5PkgStdS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Product",&t5ProductDupS))goto EXIT;
			if(!nlsIsStrNull(t5ProductDupS)) t5ProductS=nlsStrDup(t5ProductDupS);

			if(!nlsIsStrNull(t5ProductS))
			{
				fprintf(fp,"%s",t5ProductS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PrtCatCode",&t5PrtCatCodeDupS))goto EXIT;
			if(!nlsIsStrNull(t5PrtCatCodeDupS)) t5PrtCatCodeS=nlsStrDup(t5PrtCatCodeDupS);

			if(!nlsIsStrNull(t5PrtCatCodeS))
			{
				fprintf(fp,"%s",t5PrtCatCodeS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Recoverable",&t5RecoverableDupS))goto EXIT;
			if(!nlsIsStrNull(t5RecoverableDupS)) t5RecoverableS=nlsStrDup(t5RecoverableDupS);

			if(!nlsIsStrNull(t5RecoverableS))
			{
				fprintf(fp,"%s",t5RecoverableS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Recyclability",&t5RecyclabilityDupS))goto EXIT;
			if(!nlsIsStrNull(t5RecyclabilityDupS)) t5RecyclabilityS=nlsStrDup(t5RecyclabilityDupS);

			if(!nlsIsStrNull(t5RecyclabilityS))
			{
				fprintf(fp,"%s",t5RecyclabilityS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RefPartNumber",&t5RefPartNumberDupS))goto EXIT;
			if(!nlsIsStrNull(t5RefPartNumberDupS)) t5RefPartNumberS=nlsStrDup(t5RefPartNumberDupS);

			if(!nlsIsStrNull(t5RefPartNumberS))
			{
				fprintf(fp,"%s",t5RefPartNumberS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Reliability",&t5ReliabilityDupS))goto EXIT;
			if(!nlsIsStrNull(t5ReliabilityDupS)) t5ReliabilityS=nlsStrDup(t5ReliabilityDupS);

			if(!nlsIsStrNull(t5ReliabilityS))
			{
				fprintf(fp,"%s",t5ReliabilityS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SpareCriteria",&t5SpareCriteriaDupS))goto EXIT;
			if(!nlsIsStrNull(t5SpareCriteriaDupS)) t5SpareCriteriaS=nlsStrDup(t5SpareCriteriaDupS);

			if(!nlsIsStrNull(t5SpareCriteriaS))
			{
				fprintf(fp,"%s",t5SpareCriteriaS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SpareInd",&t5SpareIndDupS))goto EXIT;
			if(!nlsIsStrNull(t5SpareIndDupS)) t5SpareIndS=nlsStrDup(t5SpareIndDupS);

			if(!nlsIsStrNull(t5SpareIndS))
			{
				fprintf(fp,"%s",t5SpareIndS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}
			

			if(dstat =objGetAttribute(TCPartObjP,"t5SurfPrtStd",&t5SurfPrtStdDupS))goto EXIT;
			if(!nlsIsStrNull(t5SurfPrtStdDupS)) t5SurfPrtStdS=nlsStrDup(t5SurfPrtStdDupS);

			if(!nlsIsStrNull(t5SurfPrtStdS))
			{
				fprintf(fp,"%s",t5SurfPrtStdS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SamplesToAppr",&t5SamplesToApprDupS))goto EXIT;
			if(!nlsIsStrNull(t5SamplesToApprDupS)) t5SamplesToApprS=nlsStrDup(t5SamplesToApprDupS);

			if(!nlsIsStrNull(t5SamplesToApprS))
			{
				fprintf(fp,"%s",t5SamplesToApprS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AsmDisposalInstr",&t5AsmDisposalDupS))goto EXIT;
			if(!nlsIsStrNull(t5AsmDisposalDupS)) t5AsmDisposalS=nlsStrDup(t5AsmDisposalDupS);

			if(!nlsIsStrNull(t5AsmDisposalS))
			{
				fprintf(fp,"%s",t5AsmDisposalS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5FinDisposalInstr",&t5FinDisposalInstrDupS))goto EXIT;
			if(!nlsIsStrNull(t5FinDisposalInstrDupS)) t5FinDisposalInstrS=nlsStrDup(t5FinDisposalInstrDupS);

			if(!nlsIsStrNull(t5FinDisposalInstrS))
			{
				fprintf(fp,"%s",t5FinDisposalInstrS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RPDisposalInstr",&t5RPDisposalInstrDupS))goto EXIT;
			if(!nlsIsStrNull(t5RPDisposalInstrDupS)) t5RPDisposalInstrS=nlsStrDup(t5RPDisposalInstrDupS);

			if(!nlsIsStrNull(t5RPDisposalInstrS))
			{
				fprintf(fp,"%s",t5RPDisposalInstrS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SPDisposalInstr",&t5SPDisposalInstrDupS))goto EXIT;
			if(!nlsIsStrNull(t5SPDisposalInstrDupS)) t5SPDisposalInstrS=nlsStrDup(t5SPDisposalInstrDupS);

			if(!nlsIsStrNull(t5SPDisposalInstrS))
			{
				fprintf(fp,"%s",t5SPDisposalInstrS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5WIPDisposalInstr",&t5WIPDisposalInstrDupS))goto EXIT;
			if(!nlsIsStrNull(t5WIPDisposalInstrDupS)) t5WIPDisposalInstrS=nlsStrDup(t5WIPDisposalInstrDupS);

			if(!nlsIsStrNull(t5WIPDisposalInstrS))
			{
				fprintf(fp,"%s",t5WIPDisposalInstrS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5LastModBy",&t5LastModByDupS))goto EXIT;
			if(!nlsIsStrNull(t5LastModByDupS)) t5LastModByS=nlsStrDup(t5LastModByDupS);

			if(!nlsIsStrNull(t5LastModByS))
			{
				fprintf(fp,"%s",t5LastModByS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5VerCreator",&t5VerCreatorDupS))goto EXIT;
			if(!nlsIsStrNull(t5VerCreatorDupS)) t5VerCreatorS=nlsStrDup(t5VerCreatorDupS);

			if(!nlsIsStrNull(t5VerCreatorS))
			{
				fprintf(fp,"%s",t5VerCreatorS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5CAEDocE",&t5CAEDocEDupS))goto EXIT;
			if(!nlsIsStrNull(t5CAEDocEDupS)) t5CAEDocES=nlsStrDup(t5CAEDocEDupS);

			if(!nlsIsStrNull(t5CAEDocES))
			{
				fprintf(fp,"%s",t5CAEDocES);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Coated",&t5CoatedDupS))goto EXIT;
			if(!nlsIsStrNull(t5CoatedDupS)) t5CoatedS=nlsStrDup(t5CoatedDupS);

			if(!nlsIsStrNull(t5CoatedS))
			{
				fprintf(fp,"%s",t5CoatedS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ConvDoc",&t5ConvDocDupS))goto EXIT;
			if(!nlsIsStrNull(t5ConvDocDupS)) t5ConvDocS=nlsStrDup(t5ConvDocDupS);

			if(!nlsIsStrNull(t5ConvDocS))
			{
				fprintf(fp,"%s",t5ConvDocS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AplCopyOfErcRev",&t5AplCopyOfErcRevDupS))goto EXIT;
			if(!nlsIsStrNull(t5AplCopyOfErcRevDupS)) t5AplCopyOfErcRevS=nlsStrDup(t5AplCopyOfErcRevDupS);

			if(!nlsIsStrNull(t5AplCopyOfErcRevS))
			{
				fprintf(fp,"%s",t5AplCopyOfErcRevS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AplCopyOfErcSeq",&t5AplCopyOfErcSeqDupS))goto EXIT;
			if(!nlsIsStrNull(t5AplCopyOfErcSeqDupS)) t5AplCopyOfErcSeqS=nlsStrDup(t5AplCopyOfErcSeqDupS);

			if(!nlsIsStrNull(t5AplCopyOfErcSeqS))
			{
				fprintf(fp,"%s",t5AplCopyOfErcSeqS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AppCode",&t5AppCodeDupS))goto EXIT;
			if(!nlsIsStrNull(t5AppCodeDupS)) t5AppCodeS=nlsStrDup(t5AppCodeDupS);

			if(!nlsIsStrNull(t5AppCodeS))
			{
				fprintf(fp,"%s",t5AppCodeS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RqstNum",&t5RqstNumDupS))goto EXIT;
			if(!nlsIsStrNull(t5RqstNumDupS)) t5RqstNumS=nlsStrDup(t5RqstNumDupS);

			if(!nlsIsStrNull(t5RqstNumS))
			{
				fprintf(fp,"%s",t5RqstNumS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RsnCode",&t5RsnCodeDupS))goto EXIT;
			if(!nlsIsStrNull(t5RsnCodeDupS)) t5RsnCodeS=nlsStrDup(t5RsnCodeDupS);

			if(!nlsIsStrNull(t5RsnCodeS))
			{
				fprintf(fp,"%s",t5RsnCodeS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SurfaceArea",&t5SurfaceAreaDupS))goto EXIT;
			if(!nlsIsStrNull(t5SurfaceAreaDupS)) t5SurfaceAreaS=nlsStrDup(t5SurfaceAreaDupS);

			if(!nlsIsStrNull(t5SurfaceAreaS))
			{
				fprintf(fp,"%s",t5SurfaceAreaS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Volume",&t5VolumeDupS))goto EXIT;
			if(!nlsIsStrNull(t5VolumeDupS)) t5VolumeS=nlsStrDup(t5VolumeDupS);

			if(!nlsIsStrNull(t5VolumeS))
			{
				fprintf(fp,"%s",t5VolumeS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ErcIndName",&t5ErcIndNameDupS))goto EXIT;
			if(!nlsIsStrNull(t5ErcIndNameDupS)) t5ErcIndNameS=nlsStrDup(t5ErcIndNameDupS);

			
			if(!nlsIsStrNull(t5ErcIndNameS))
			{
				fprintf(fp,"%s",t5ErcIndNameS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PostRelReq",&t5PostRelReqDupS))goto EXIT;
			if(!nlsIsStrNull(t5PostRelReqDupS)) t5PostRelReqS=nlsStrDup(t5PostRelReqDupS);

			if(!nlsIsStrNull(t5PostRelReqS))
			{
				fprintf(fp,"%s",t5PostRelReqS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ItmCategory",&t5ItmCategoryDupS))goto EXIT;
			if(!nlsIsStrNull(t5ItmCategoryDupS)) t5ItmCategoryS=nlsStrDup(t5ItmCategoryDupS);

			if(!nlsIsStrNull(t5ItmCategoryS))
			{
				fprintf(fp,"%s",t5ItmCategoryS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5CopReq",&t5CopReqDupS))goto EXIT;
			if(!nlsIsStrNull(t5CopReqDupS)) t5CopReqS=nlsStrDup(t5CopReqDupS);

			if(!nlsIsStrNull(t5CopReqS))
			{
				fprintf(fp,"%s",t5CopReqS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AplInvalidate",&t5AplInvalidateDupS))goto EXIT;
			if(!nlsIsStrNull(t5AplInvalidateDupS)) t5AplInvalidateS=nlsStrDup(t5AplInvalidateDupS);

			if(!nlsIsStrNull(t5AplInvalidateS))
			{
				fprintf(fp,"%s",t5AplInvalidateS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PrtValiStatus",&t5PrtValiStatusDupS))goto EXIT;
			if(!nlsIsStrNull(t5PrtValiStatusDupS)) t5PrtValiStatusS=nlsStrDup(t5PrtValiStatusDupS);

			if(!nlsIsStrNull(t5PrtValiStatusS))
			{
				fprintf(fp,"%s",t5PrtValiStatusS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5DRSubState",&t5DRSubStateDupS))goto EXIT;
			if(!nlsIsStrNull(t5DRSubStateDupS)) t5DRSubStateS=nlsStrDup(t5DRSubStateDupS);

			if(!nlsIsStrNull(t5DRSubStateS))
			{
				fprintf(fp,"%s",t5DRSubStateS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5KnxtDocInd",&t5KnxtDocIndDupS))goto EXIT;
			if(!nlsIsStrNull(t5KnxtDocIndDupS)) t5KnxtDocIndS=nlsStrDup(t5KnxtDocIndDupS);

			if(!nlsIsStrNull(t5KnxtDocIndS))
			{
				fprintf(fp,"%s",t5KnxtDocIndS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SimVal",&t5SimValDupS))goto EXIT;
			if(!nlsIsStrNull(t5SimValDupS)) t5SimValS=nlsStrDup(t5SimValDupS);

			if(!nlsIsStrNull(t5SimValS))
			{
				fprintf(fp,"%s",t5SimValS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PerYield",&t5PerYieldDupS))goto EXIT;
			if(!nlsIsStrNull(t5PerYieldDupS)) t5PerYieldS=nlsStrDup(t5PerYieldDupS);

			if(!nlsIsStrNull(t5PerYieldS))
			{
				fprintf(fp,"%s",t5PerYieldS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AltPartNo",&t5AltPartNoDupS))goto EXIT;
			if(!nlsIsStrNull(t5AltPartNoDupS)) t5AltPartNoS=nlsStrDup(t5AltPartNoDupS);

			if(!nlsIsStrNull(t5AltPartNoS))
			{
				fprintf(fp,"%s",t5AltPartNoS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RolledupWt",&t5RolledupWtDupS))goto EXIT;
			if(!nlsIsStrNull(t5RolledupWtDupS)) t5RolledupWtS=nlsStrDup(t5RolledupWtDupS);

			if(!nlsIsStrNull(t5RolledupWtS))
			{
				fprintf(fp,"%s",t5RolledupWtS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5EstSheetReqd",&t5EstSheetReqdDupS))goto EXIT;
			if(!nlsIsStrNull(t5EstSheetReqdDupS)) t5EstSheetReqdS=nlsStrDup(t5EstSheetReqdDupS);

			if(!nlsIsStrNull(t5EstSheetReqdS))
			{
				fprintf(fp,"%s",t5EstSheetReqdS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PFDModReqd",&t5PFDModReqdDupS))goto EXIT;
			if(!nlsIsStrNull(t5PFDModReqdDupS)) t5PFDModReqdS=nlsStrDup(t5PFDModReqdDupS);

			if(!nlsIsStrNull(t5PFDModReqdS))
			{
				fprintf(fp,"%s",t5PFDModReqdS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ToolIndentReqd",&t5ToolIndentReqdDupS))goto EXIT;
			if(!nlsIsStrNull(t5ToolIndentReqdDupS)) t5ToolIndentReqdS=nlsStrDup(t5ToolIndentReqdDupS);

			if(!nlsIsStrNull(t5ToolIndentReqdDupS))
			{
				fprintf(fp,"%s",t5ToolIndentReqdDupS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}


			if(dstat =objGetAttribute(TCPartObjP,"CategoryName",&CategoryNameDupS))goto EXIT;
			if(!nlsIsStrNull(CategoryNameDupS)) CategoryNameS=nlsStrDup(CategoryNameDupS);

			if(!nlsIsStrNull(CategoryNameS))
			{
				fprintf(fp,"%s",CategoryNameS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"CreationDate",&PartCreDateDupS))goto EXIT;
			if(!nlsIsStrNull(PartCreDateDupS)) PartCreDateS=nlsStrDup(PartCreDateDupS);

			if(!nlsIsStrNull(PartCreDateS))
			{
				fprintf(fp,"%s",PartCreDateS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"LastUpdate",&PartModDateDupS))goto EXIT;
			if(!nlsIsStrNull(PartModDateDupS)) PartModDateS=nlsStrDup(PartModDateDupS);

			if(!nlsIsStrNull(PartModDateS))
			{
				fprintf(fp,"%s",PartModDateS);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"Creator",&PartCreatorDup))goto EXIT;
			if(!nlsIsStrNull(PartCreatorDup)) PartCreator=nlsStrDup(PartCreatorDup);

			if(!nlsIsStrNull(PartCreator))
			{
				fprintf(fp,"%s",PartCreator);
				fprintf(fp,"^");
			}
			else
			{
				fprintf(fp," ^");
			}
		
		
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

	FILE	*fp	= NULL;
	FILE	*fp_Error	= NULL;
	FILE	*fip	= NULL;

	int partCnt =0;
	int ii =0;
	int partNotFound =0;

	SetOfStrings      dbScp	=          NULL;
	char inputline[3000];

	
	t5MethodInitWMD("tm_CarryOverPartStamp");

	printf("\n Executing... tm_CarryOverPartStamp.c ... \n");fflush(stdout);

	LoginS=nlsStrAlloc(200);
	PasswordS=nlsStrAlloc(200);
	InpPartNumberS=nlsStrAlloc(200);
	InpPartNumberRev=nlsStrAlloc(200);
	InpPartNumberSeq=nlsStrAlloc(200);
	OutFileNamePART=nlsStrAlloc(200);
	OutFileNameAPLPART=nlsStrAlloc(200);
	InPutFileNamePART=nlsStrAlloc(200);

	LoginS=argv[1];
	PasswordS=argv[2];
	InPutFileNamePART=argv[3];
	OutFileNamePART=argv[4];

	printf("\n InPutFileNamePART %s :OutFileNamePART :::%s:::\n",InPutFileNamePART,OutFileNamePART);fflush(stdout);
	//printf("\n !!!!!!!!!!!!Starting here!!!!!!!! \n");


	if (argc < 5 || argc > 5)
	{
		printf("\nSYNTEX ERROR :: \n");
		printf("\nSYNTEX SHOULD BE :: <EXE><USER-LOGIN><PASSWD><InputFileName><OutputFileName>\n");
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
		while(fgets(inputline,1000,fip)!=NULL)
		{
			if(!nlsIsStrNull(LineReadS))nlsStrFree(LineReadS);
			LineReadS	= nlsStrAlloc(500);

			if(!nlsIsStrNull(inputline))
			{
				nlsStrCpy(LineReadS,inputline);
			}

			InpPartNumberS=strtok(LineReadS,"^");
			
			setOfAssmblyObjs=NULL;
	
			if (dstat = oiSqlCreateSelect(&PrtQrysql)) goto EXIT;
			if(dstat=(oiSqlWhereBegParen(PrtQrysql)));
			if(dstat = oiSqlWhereEQ(PrtQrysql,PartNumberAttr,InpPartNumberS)) goto EXIT;
			if(dstat = oiSqlWhereAND(PrtQrysql)) goto EXIT;
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

					t5CheckMfail(GetTCPartInfoForAPL(TCPartObjP,fp,mfail));



					
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

