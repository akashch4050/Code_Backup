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

char* subString_1 (
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
;

void ReverseStr1(char str[])
{
    int i,j;
    char temp[100];
    for(i=strlen(str)-1,j=0; i+1!=0; --i,++j)
    {
        temp[j]=str[i];
    }
    temp[j]='\0';
    strcpy(str,temp);
}
;

int CheckRelstatus(ObjectPtr LeftObjP ,ObjectPtr RightObjP ,string RelClass ,	integer* mfail)
{
		int returnVal=0;
		SqlPtr RelsqlPtr	=NULL;

		SetOfObjects relObjSo	=NULL;

		 string LeftObidDupS	=NULL;
		  string LeftObid		=NULL;
		  string RightObidDups	=NULL;
		  string RightObid		=NULL;
		  status dstat = OKAY;

		char *mod_name="RelationChk";

		printf("\n Inside Relation existence check\n");
		//objDump(LeftObjP);
		
		t5CheckDstat(objGetAttribute(LeftObjP,OBIDAttr,&LeftObidDupS));
		LeftObid=nlsStrDup(LeftObidDupS);
		
		//objDump(RightObjP);
		t5CheckDstat(objGetAttribute(RightObjP,OBIDAttr,&RightObidDups));
		RightObid=nlsStrDup(RightObidDups);


		t5CheckDstat(oiSqlCreateSelect(&RelsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(RelsqlPtr,LeftAttr,LeftObid));
		t5CheckDstat(oiSqlWhereAND(RelsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(RelsqlPtr,RightAttr,RightObid));

		t5CheckMfail(QueryWhere(RelClass,RelsqlPtr,&relObjSo,mfail));

		if (setSize(relObjSo)==0)
		
		{returnVal=1;}
		
		else
		{returnVal=2;}

CLEANUP:
	printf("In the cleanup of RelationChk");

	if (relObjSo)	t5FreeSetOfObjects(relObjSo);
	if (RelsqlPtr)	t5FreeSqlPtr(RelsqlPtr);


EXIT:
	return returnVal;

}

status t5_setTaskAttribute
(
	ObjectPtr NewDMLTaskObjDuP,
	string	Lifecycle,
	string	WbsDisposition,
	string	WbsStatus,
	string	WbsOriginator,
	string	Creator,
	string	WbsAnalyst,
	integer* mfail
)
{
	
	status			dstat					=	OKAY;
   	char			*mod_name				=	"t5_setTaskAttribute";
	
	//t5MethodInitWMD("t5_setTaskAttribute");
	printf("\nInside Task Attribute Update...!!!");fflush(stdout);

	printf("\nDisposition : %s, WBS Status : %s , Analyst : %s",WbsDisposition,WbsStatus,WbsAnalyst);fflush(stdout);
	if(dstat = objSetAttribute(NewDMLTaskObjDuP,LifeCycleStateAttr,"LcsAplRlzd"))  goto CLEANUP;
	if(dstat = objSetAttribute(NewDMLTaskObjDuP,WbsDispositionAttr,WbsDisposition))  goto CLEANUP;
	if(dstat = objSetAttribute(NewDMLTaskObjDuP,WbsStatusAttr,WbsStatus))  goto CLEANUP;
	if(dstat = objSetAttribute(NewDMLTaskObjDuP,WbsOriginatorAttr,"APLAna1"))goto CLEANUP;
	if(dstat = objSetAttribute(NewDMLTaskObjDuP,CreatorAttr,"APLAna1"))goto CLEANUP;
	if(dstat = objSetAttribute(NewDMLTaskObjDuP,WbsAnalystAttr,WbsAnalyst))goto CLEANUP;
	//Need to handle APL Released date stampped.
	//t5CheckMfail(t0UpdateObject(&NewDMLTaskObjDuP,mfail));


CLEANUP:
	t5PrintCleanUpModName;

EXIT:
	t5CheckDstatAndReturn;
}

status t5_UpdCSIASLOC(ObjectPtr NewTaskPartObj, string CS, string IA, string SLOC,FILE *FileName,integer* mfail)
{
	status			dstat					=	OKAY;
   	char			*mod_name				=	"t5_UpdCSIASLOC";
	
	string			sPartNumber				=	NULL;
	string			sPartNumberDup			=	NULL;
	
	//t5MethodInitWMD("t5_UpdCSIASLOC");
	printf("\nInside t5_UpdCSIASLOC");fflush(stdout);

	printf("\nCS : %s, IA : %s, SLOC : %s",CS,IA,SLOC);fflush(stdout);
	
	if (NewTaskPartObj!=NULL)
	{
		if(dstat = objGetAttribute(NewTaskPartObj,PartNumberAttr,&sPartNumber))  goto CLEANUP;
		if (!nlsIsStrNull(sPartNumber))
		{
			sPartNumberDup	=	nlsStrDup(sPartNumber);
		}
		if (!nlsIsStrNull(sPartNumberDup))
		{
			if(dstat = objSetAttribute(NewTaskPartObj,t5CarMakeBuyIndicatorAttr,CS))  goto CLEANUP;
			if(dstat = objSetAttribute(NewTaskPartObj,t5CarIntialAgencyAttr,IA))  goto CLEANUP;
			if(dstat = objSetAttribute(NewTaskPartObj,t5CarStoreLocationAttr,SLOC))  goto CLEANUP;

			t5CheckMfail(t0UpdateObject(&NewTaskPartObj,mfail));

			printf("\nCS, IA, SLOC Update for the part : %s ",sPartNumberDup);fflush(stdout);
		}
	}
	else
	{
		printf("\nPlease check the Part object...!!!");fflush(stdout);
	}
CLEANUP:
	t5PrintCleanUpModName;

EXIT:
	t5CheckDstatAndReturn;
}

status t5_UpdEffectivity(ObjectPtr NewTaskPartObj, string sStartDate, string sEndDate,FILE *FileName,integer* mfail)
{
	status			dstat					=	OKAY;
   	char			*mod_name				=	"t5_UpdEffectivity";
	
	string			sPartNumber				=	NULL;
	string			sPartNumberDup			=	NULL;
	string			sRevision				=	NULL;
	string			sRevisionDup			=	NULL;
	string			sSequence				=	NULL;
	string			sSequenceDup			=	NULL;
	string			StrPrevStrtDateEx		=	NULL;
	string			StrCurDate				=	NULL;
	string			StrStartDate			=	NULL;
	string			StrEndDate				=	NULL;
	string			StrPrevStrtDate			=	NULL;
	string			StrPrevEndDate			=	NULL;
	string			StrEffFrmDatPre			=	NULL;

	ObjectPtr		TaskPartsViewObjP		=	NULL;
	ObjectPtr		genContextObj			=	NULL;
	ObjectPtr		contextObj				=	NULL;
	ObjectPtr		PrevRevObj				=	NULL;
	ObjectPtr		RevEffNewObj			=	NULL;
	ObjectPtr		RevEffObj				=	NULL;
	ObjectPtr		RevEffDlgObj			=	NULL;

	SetOfObjects	ViewSO					=	NULL;
	SetOfObjects	soExpObj				=	NULL;
	SetOfObjects	relObjSet				=	NULL;
	SetOfObjects	RevEffObjSet			=	NULL;
	SetOfObjects	RevEffRelObjSet			=	NULL;
	SetOfObjects	ExtraObjSet				=	NULL;


	SetOfStrings	ExtraStrSet				=	NULL;

	SqlPtr			GetvewSqlPtr			=	NULL;

	//t5MethodInitWMD("t5_UpdEffectivity");

	printf("\nInside t5_UpdEffectivity");fflush(stdout);

	printf("\nStart Date : %s, End Date : %s",sStartDate,sEndDate);fflush(stdout);

	if (NewTaskPartObj!=NULL)
	{
		if(dstat = objGetAttribute(NewTaskPartObj,PartNumberAttr,&sPartNumber))  goto CLEANUP;
		if (!nlsIsStrNull(sPartNumber))
		{
			sPartNumberDup	=	nlsStrDup(sPartNumber);
		}

		if(dstat = objGetAttribute(NewTaskPartObj,RevisionAttr,&sRevision))  goto CLEANUP;
		if (!nlsIsStrNull(sRevision))
		{
			sRevisionDup	=	nlsStrDup(sRevision);
		}

		if(dstat = objGetAttribute(NewTaskPartObj,SequenceAttr,&sSequence))  goto CLEANUP;
		if (!nlsIsStrNull(sSequence))
		{
			sSequenceDup	=	nlsStrDup(sSequence);
		}

		printf("\nPart number : %s, Revision : %s, Sequence : %s",sPartNumberDup,sRevisionDup,sSequenceDup);fflush(stdout);
		
		//ReverseStr(sStartDate);
		StrCurDate	= nlsStrDup(sStartDate) ;
		StrEndDate	= nlsStrDup("9999/12/31");
		StrPrevStrtDate	= nlsStrDup("");
		StrPrevEndDate	= nlsStrDup(StrCurDate);
		StrPrevStrtDateEx = sysGetOffsetDate(StrCurDate, 0, -1, 0);

		printf("\n StrCurDate : [%s]\n", StrCurDate); fflush(stdout);
		printf("\n StrEndDate : [%s]\n", StrEndDate); fflush(stdout);
		printf("\n StrPrevStrtDate : [%s]\n", StrPrevStrtDate); fflush(stdout);
		printf("\n StrPrevEndDate : [%s]\n", StrPrevEndDate); fflush(stdout);
		printf("\n StrPrevStrtDateEx : [%s]\n", StrPrevStrtDateEx); fflush(stdout);


		//start effectivity stamping

		t5CheckDstat(oiSqlCreateSelect(&GetvewSqlPtr));
		t5CheckDstat(oiSqlWhereLE(GetvewSqlPtr,ViewNameAttr,"APL"));
		t5CheckDstat(oiSqlWhereAND(GetvewSqlPtr));
		t5CheckDstat(oiSqlWhereLE(GetvewSqlPtr,ViewNetworkAttr,"EAS"));
		t5CheckDstat(QueryDbObject(VewClass,GetvewSqlPtr,0,TRUE,SC_SCOPE_OF_SESSION,&ViewSO,mfail));
		
		printf("\nNo of View found : %d",setSize(ViewSO));fflush(stdout);
		if (setSize(ViewSO)>0)
			TaskPartsViewObjP=setGet(ViewSO,0);
		//objDump(TaskPartsViewObjP);
		t5CheckMfail(IntSetUpContext(objClass(NewTaskPartObj),NewTaskPartObj,&genContextObj,mfail));
		t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass, genContextObj, &contextObj, mfail));
		t5CheckDstat(objSetAttribute(contextObj,CfgItemIdAttr,"GlobalCtxt"));
		t5CheckMfail(SetNavigateViewPref(contextObj,TRUE,"EAS","APL",mfail));
		t5CheckDstat(objSetObject(genContextObj, ConfigCtxtBlobAttr, contextObj));

		t5CheckMfail(ExpandRelationWithCtxt(RevEffDClass,NewTaskPartObj,"RvfCfgItemInStrBIApc",genContextObj,SC_SCOPE_OF_SESSION ,NULL, &soExpObj, &relObjSet, mfail));
		
		printf("\nNo of soExpObj found : %d",setSize(soExpObj));fflush(stdout);

		if (setSize(soExpObj)!=0)
		{
			printf("\n Effectivity Already exists for this Part1 \n");fflush(stdout);
			if (nlsStrCmp(sRevisionDup,"NR")==0)
			{
				printf("\nNR Revision Found...!!!");fflush(stdout);
				ExtraStrSet	=	NULL;
				ExtraObjSet	=	NULL;
				
				if(dstat = objGetObject(setGet(relObjSet, 0), AssocRelationObjAttr, &RevEffObj));

				printf("\n NR DATE StrCurDate : %s",StrCurDate);fflush(stdout);
				printf("\n NR DATE StrEndDate : %s",StrEndDate);fflush(stdout);
				if(dstat = SetUpDialog(RevEffDClass, NULL, "UpdateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;
				if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
				if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffFromAttr, StrCurDate)) goto EXIT ;
				if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrEndDate))  goto EXIT;
				//objDump(RevEffDlgObj);
				if(dstat = UpdateRevEffObject(RevEffObj, TaskPartsViewObjP, RevEffDlgObj, mfail)) goto EXIT;
				//objDump(setGet(relObjSet,0));
				if(dstat = UpdateEffectivity(setGet(relObjSet,0), NewTaskPartObj, TaskPartsViewObjP, StrCurDate, StrEndDate, mfail)) goto EXIT;
				if(*mfail  != USC_OKAY)
				{
					printf("\n1Current revision having effectivity issue...!!!");fflush(stdout);
					fprintf(FileName,"%s NR Revision having effectivity issue.\n",sPartNumberDup);fflush(FileName);
				}
			}
			else
			{
				printf("\nNon NR Revision found...!!!");fflush(stdout);
				if(PrevRevObj != NULL)
				{
					PrevRevObj = NULL;
				}

				if(dstat = GetPrevOfficialRev(NewTaskPartObj, &PrevRevObj, mfail)) goto EXIT;
				if(PrevRevObj != NULL)
				{
					if(dstat = ExpandRelationWithCtxt(RevEffDClass, PrevRevObj, "RvfCfgItemInStrBIApc", genContextObj, SC_SCOPE_OF_SESSION, NULL, &RevEffObjSet, &RevEffRelObjSet, mfail)) goto EXIT;
					printf("\n Prev-Rev setSize(RevEffRelObjSet) : [%d]\n", setSize(RevEffRelObjSet)); fflush(stdout);

					if(setSize(RevEffRelObjSet) == 0)
					{
						printf("\nPrevious revision effectivity not found...!!!");fflush(stdout);
						/*Efectivity Not Present, create effectivity for previous revision*/
						if(dstat = SetUpDialog(RevEffClass, NULL, "CreateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;
						if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
						if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffFromAttr, StrPrevStrtDateEx)) goto EXIT ;
						if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrPrevEndDate))  goto EXIT;

						/*NewEffDlgRelObj : Newly created revision effectivity relation object.*/
						//if(dstat = CreateRevEff2(RevEffClass, PrevRevObj, NULL, TaskPartsViewObjP, RevEffDlgObj, &RevEffNewObj, mfail))goto EXIT ;
						t5CheckDstat(CreateEffectivity(PrevRevObj,TaskPartsViewObjP,StrPrevStrtDateEx,StrPrevEndDate,mfail));
						if(*mfail  != USC_OKAY)
						{
							printf("\nPrevious revision having effectivity creation problem...!!!");fflush(stdout);
							fprintf(FileName,"%s, %s Previous Revision effectivity create issue.\n",sPartNumberDup,sRevisionDup);fflush(FileName);
						}
					}
					else
					{
						printf("\nPrevious revision effectivity found...!!!");fflush(stdout);
						if(dstat = objGetObject(setGet(RevEffRelObjSet, 0), AssocRelationObjAttr, &RevEffObj));
						
						ExtraStrSet	=	NULL;
						ExtraObjSet	=	NULL;
						if(dstat = SetUpDialog(RevEffDClass, NULL, "UpdateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;

						if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
						if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrPrevEndDate))  goto EXIT;
						if(dstat = UpdateRevEffObject(RevEffObj, TaskPartsViewObjP, RevEffDlgObj, mfail)) goto EXIT;
						if(dstat = UpdateEffectivity(setGet(RevEffRelObjSet,0), PrevRevObj, TaskPartsViewObjP, StrPrevStrtDate, StrPrevEndDate, mfail)) goto EXIT;
						if(*mfail  != USC_OKAY)
						{
							printf("\nPrevious revision having effectivity update problem...!!!");fflush(stdout);
							fprintf(FileName,"%s, %s Previous Revision effectivity update issue.\n",sPartNumberDup,sRevisionDup);fflush(FileName);
						}
					}
				}
				else
				{
					printf("\nPrevious revision not found...!!!");fflush(stdout);
					fprintf(FileName,"%s, %s Previous Revision not found in system.\n",sPartNumberDup,sRevisionDup);fflush(FileName);
				}

				/*Effetivity for current revision start*/

				if(dstat = ExpandRelationWithCtxt(RevEffDClass, NewTaskPartObj, "RvfCfgItemInStrBIApc", genContextObj, SC_SCOPE_OF_SESSION, NULL, &RevEffObjSet, &RevEffRelObjSet, mfail)) goto EXIT;
				printf("\n setSize(RevEffRelObjSet) : [%d]\n", setSize(RevEffRelObjSet)); fflush(stdout);
				
				StrStartDate= sysGetOffsetDate(StrCurDate, 0, +1, 0);
				if (setSize(RevEffRelObjSet)>0)
				{
					printf("\ncurrent revision effectivity found...!!!");fflush(stdout);
					printf("\ncurrent revision effectivity found StrStartDate : %s",StrStartDate);fflush(stdout);
					printf("\ncurrent revision effectivity found StrEndDate : %s",StrEndDate);fflush(stdout);
					ExtraStrSet	=	NULL;
					ExtraObjSet	=	NULL;

					if(dstat = SetUpDialog(RevEffClass, NULL, "CreateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;
					if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
					if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffFromAttr, StrStartDate)) goto EXIT ;
					if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrEndDate))  goto EXIT;

					if(dstat = UpdateEffectivity(setGet(RevEffRelObjSet,0), NewTaskPartObj, TaskPartsViewObjP, StrStartDate, StrEndDate, mfail)) goto EXIT;
					if(*mfail  != USC_OKAY)
					{
						printf("\n2Current revision having effectivity issue...!!!");fflush(stdout);
						fprintf(FileName,"%s, %s current Revision effectivity update issue\n",sPartNumberDup,sRevisionDup);fflush(FileName);
					}
					//goto CLEANUP;
				}
				else
				{
					printf("\ncurrent revision create effectivity found StrStartDate : %s",StrStartDate);fflush(stdout);
					printf("\ncurrent revision create effectivity found StrEndDate : %s",StrEndDate);fflush(stdout);
					ExtraStrSet	=	NULL;
					ExtraObjSet	=	NULL;

					if(dstat = SetUpDialog(RevEffClass, NULL, "CreateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;
					if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
					if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffFromAttr, StrStartDate)) goto EXIT ;
					if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrEndDate))  goto EXIT;

					//if(dstat = CreateRevEff2(RevEffClass, NewTaskPartObj, NULL, TaskPartsViewObjP, RevEffDlgObj, &RevEffNewObj, mfail))goto EXIT ;
					t5CheckDstat(CreateEffectivity(NewTaskPartObj,TaskPartsViewObjP,StrStartDate,StrEndDate,mfail));
					if(*mfail  != USC_OKAY)
					{
						printf("\n3Current revision having effectivity issue...!!!");fflush(stdout);
						fprintf(FileName,"%s, %s current Revision effectivity create issue\n",sPartNumberDup,sRevisionDup);fflush(FileName);
					}
				}
			}
			//goto CLEANUP;
		}
		else
		{
			//StrCurDate	= sysGetDate2() ;
			if (nlsStrCmp(sRevisionDup,"NR")==0)
			{
				StrStartDate	= nlsStrDup(StrCurDate);
				printf("\nNR Revision Found...!!!");fflush(stdout);
				printf("\nNR current revision create effectivity found StrStartDate : %s",StrStartDate);fflush(stdout);
				printf("\nNR current revision create effectivity found StrEndDate : %s",StrEndDate);fflush(stdout);
				
				//if(dstat = objGetObject(setGet(relObjSet, 0), AssocRelationObjAttr, &RevEffObj));

				ExtraStrSet	=	NULL;
				ExtraObjSet	=	NULL;

				if(dstat = SetUpDialog(RevEffClass, NULL, "CreateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;
				if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
				if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffFromAttr, StrStartDate)) goto EXIT ;
				if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrEndDate))  goto EXIT;
				//if(dstat = CreateRevEff2(RevEffClass, NewTaskPartObj, NULL, TaskPartsViewObjP, RevEffDlgObj, &RevEffNewObj, mfail))goto EXIT ;
				t5CheckDstat(CreateEffectivity(NewTaskPartObj,TaskPartsViewObjP,StrStartDate,StrEndDate,mfail));
				//if(dstat = UpdateRevEffObject(RevEffObj, TaskPartsViewObjP, RevEffDlgObj, mfail)) goto EXIT;
				//if(dstat = UpdateEffectivity(setGet(relObjSet,0), NewTaskPartObj, TaskPartsViewObjP, StrStartDate, StrEndDate, mfail)) goto EXIT;
				if(*mfail  != USC_OKAY)
				{
					printf("\n4Current revision having effectivity issue...!!!");fflush(stdout);
					fprintf(FileName,"%s, %s NR Revision effectivity create issue\n",sPartNumberDup,sRevisionDup);fflush(FileName);
				}
			}
			else
			{
				printf("\nNon NR Revision found...!!!");fflush(stdout);
				if(PrevRevObj != NULL)
				{
					PrevRevObj = NULL;
				}

				if(dstat = GetPrevOfficialRev(NewTaskPartObj, &PrevRevObj, mfail)) goto EXIT;
				if(PrevRevObj != NULL)
				{
					if(dstat = ExpandRelationWithCtxt(RevEffDClass, PrevRevObj, "RvfCfgItemInStrBIApc", genContextObj, SC_SCOPE_OF_SESSION, NULL, &RevEffObjSet, &RevEffRelObjSet, mfail)) goto EXIT;
					printf("\n Prev-Rev setSize(RevEffRelObjSet) : [%d]\n", setSize(RevEffRelObjSet)); fflush(stdout);

					if(setSize(RevEffRelObjSet) == 0)
					{
						printf("\n Previous revision effectivity not found...!!!");fflush(stdout);
						printf("\n Previous DATE StrPrevStrtDateEx : %s",StrPrevStrtDateEx);fflush(stdout);
						printf("\n Previous DATE StrPrevEndDate : %s",StrPrevEndDate);fflush(stdout);
						/*Efectivity Not Present, create effectivity for previous revision*/
						if(dstat = SetUpDialog(RevEffClass, NULL, "CreateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;
						if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
						if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffFromAttr, StrPrevStrtDateEx)) goto EXIT ;
						if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrPrevEndDate))  goto EXIT;

						/*NewEffDlgRelObj : Newly created revision effectivity relation object. TaskPartsViewObjP*/
						//if(dstat = CreateRevEff2(RevEffClass, PrevRevObj, NULL, TaskPartsViewObjP, RevEffDlgObj, &RevEffNewObj, mfail))goto EXIT ;
						t5CheckDstat(CreateEffectivity(PrevRevObj,TaskPartsViewObjP,StrPrevStrtDateEx,StrPrevEndDate,mfail));
						//t5CheckDstat(CreateEffectivity(PrevRevObj,TaskPartsViewObjP,StrPrevStrtDateEx,StrPrevEndDate,mfail));
						if(*mfail  != USC_OKAY)
						{
							printf("\nPrevious revision having effectivity creation problem...!!!");fflush(stdout);
							fprintf(FileName,"%s, %s Previous Revision effectivity create issue\n",sPartNumberDup,sRevisionDup);fflush(FileName);
						}
					}
					else
					{
						printf("\nPrevious revision effectivity found...!!!");fflush(stdout);
						if(dstat = objGetObject(setGet(RevEffRelObjSet, 0), AssocRelationObjAttr, &RevEffObj));
						
						ExtraStrSet	=	NULL;
						ExtraObjSet	=	NULL;
						if(dstat = SetUpDialog(RevEffDClass, NULL, "UpdateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;

						if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
						if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrPrevEndDate))  goto EXIT;
						if(dstat = UpdateRevEffObject(RevEffObj, TaskPartsViewObjP, RevEffDlgObj, mfail)) goto EXIT;
						if(dstat = UpdateEffectivity(setGet(RevEffRelObjSet,0), PrevRevObj, TaskPartsViewObjP, StrPrevStrtDate, StrPrevEndDate, mfail)) goto EXIT;
						if(*mfail  != USC_OKAY)
						{
							printf("\nPrevious revision having effectivity update problem...!!!");fflush(stdout);
							fprintf(FileName,"%s, %s Previous Revision effectivity update issue\n",sPartNumberDup,sRevisionDup);fflush(FileName);
						}
					}
				}
				else
				{
					printf("\nPrevious revision not found...!!!");fflush(stdout);
				}

				/*Effetivity for current revision start*/

				if(dstat = ExpandRelationWithCtxt(RevEffDClass, NewTaskPartObj, "RvfCfgItemInStrBIApc", genContextObj, SC_SCOPE_OF_SESSION, NULL, &RevEffObjSet, &RevEffRelObjSet, mfail)) goto EXIT;
				printf("\n setSize(RevEffRelObjSet) : [%d]\n", setSize(RevEffRelObjSet)); fflush(stdout);
				
				StrStartDate= sysGetOffsetDate(sStartDate, 0, +1, 0);
				if (setSize(RevEffRelObjSet)>0)
				{
					printf("\ncurrent revision effectivity found...!!!");fflush(stdout);
					if(dstat = objGetObject(setGet(RevEffRelObjSet, 0), AssocRelationObjAttr, &RevEffObj));
					ExtraStrSet	=	NULL;
					ExtraObjSet	=	NULL;

					if(dstat = SetUpDialog(RevEffDClass, NULL, "UpdateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;
					if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
					if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffFromAttr, StrStartDate)) goto EXIT ;
					if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrEndDate))  goto EXIT;
					if(dstat = UpdateRevEffObject(RevEffObj, TaskPartsViewObjP, RevEffDlgObj, mfail)) goto EXIT;
					if(dstat = UpdateEffectivity(setGet(RevEffRelObjSet,0), NewTaskPartObj, TaskPartsViewObjP, StrPrevStrtDate, StrPrevEndDate, mfail)) goto EXIT;
					if(*mfail  != USC_OKAY)
					{
						printf("\nCurrent revision having effectivity issue...!!!");fflush(stdout);
						fprintf(FileName,"%s, %s current Revision effectivity update issue\n",sPartNumberDup,sRevisionDup);fflush(FileName);
					}
					//goto CLEANUP;
				}
				else
				{
					ExtraStrSet	=	NULL;
					ExtraObjSet	=	NULL;

					if(dstat = SetUpDialog(RevEffClass, NULL, "CreateDialogC", &ExtraStrSet, &ExtraObjSet, &RevEffDlgObj, mfail)) goto EXIT;
					if(dstat = objSetAttribute(RevEffDlgObj, CfgItemIdAttr, "GlobalCtxt"))  goto EXIT;
					if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffFromAttr, StrStartDate)) goto EXIT ;
					if(dstat = objSetAttribute(RevEffDlgObj, DisplayDateEffToAttr, StrEndDate))  goto EXIT;

					//if(dstat = CreateRevEff2(RevEffClass, NewTaskPartObj, NULL, TaskPartsViewObjP, RevEffDlgObj, &RevEffNewObj, mfail))goto EXIT ;
					t5CheckDstat(CreateEffectivity(NewTaskPartObj,TaskPartsViewObjP,StrStartDate,StrEndDate,mfail));
					if(*mfail  != USC_OKAY)
					{
						printf("\nCurrent revision having effectivity issue...!!!");fflush(stdout);
						fprintf(FileName,"%s, %s Previous Revision effectivity create issue\n",sPartNumberDup,sRevisionDup);fflush(FileName);
					}
				}
			}
		}
	}

CLEANUP:
	t5PrintCleanUpModName;

EXIT:
	t5CheckDstatAndReturn;
}

status AMDMLCreateFunc(string project,string DMLNameS,string sTaskNumber,string sDesignGrp,string WbsNameMain,string WbsDescriptionMain,ObjectPtr *AMDmlObj,integer* mfail)
{
	char *mod_name = "AMDMLCreateFunc";
	//integer mfail = USC_OKAY;
	status	dstat = OKAY;

	ObjectPtr DmlCreateDia				=	NULL;
	ObjectPtr DmlObj					=	NULL;
	ObjectPtr UsrObj					=	NULL;
	ObjectPtr CntrlObj					=	NULL;
	ObjectPtr tskObj					=	NULL;
	ObjectPtr dialogObj					=	NULL;

	string			StrUsrAgn1			=	NULL;
	string			StrUsrAgn1Dup		=	NULL;
	string			StrUsrLoc1			=	NULL;
	string			StrUsrLoc1Dup		=	NULL;
	string			sTaskNum			=	NULL;
	string			sTaskNumDup			=	NULL;
	
	SetOfStrings	extraStrECN			=	NULL;
	SetOfObjects	extraObjECN			=	NULL;
	SetOfObjects	UsrObjSet			=	NULL;
	SetOfObjects	soCntrlObj			=	NULL;
	SetOfObjects	brkTaskObjsTo		=	NULL;
	SetOfObjects	extraObj			=	NULL;

	SetOfStrings	DsgnGrpValue			= NULL;
	SetOfStrings	extraStr				= NULL;

	SqlPtr			Sql_ptr				=	NULL;
	SqlPtr			sqlPtr				=	NULL;

	//t5MethodInitWMD("AMDMLCreateFunc");

	printf("\nIn AMDMLCreateFunc : %s",DMLNameS); fflush(stdout);
	
	DsgnGrpValue	= setCreate(1);

	low_set_add_str(DsgnGrpValue,sDesignGrp);

    printf("\nBefore set attributes\n");	fflush(stdout);
	if (dstat = smSetSessionUsrName ("APLAna1")) goto EXIT;
	if(Sql_ptr) oiSqlDispose(Sql_ptr);sqlPtr = NULL;
	if(dstat = oiSqlCreateSelect(&Sql_ptr)) goto EXIT;
	//if(dstat = oiSqlWhereEQ(Sql_ptr,ParticipantAttr, "APLAna1")) goto EXIT;
	if(dstat = oiSqlWhereEQ(Sql_ptr,ParticipantAttr, "APLAna1")) goto EXIT;
	printf("\nQuery User...!!!");fflush(stdout);
	if(dstat = QueryDbObject(UsrClass, Sql_ptr, 1, TRUE, SC_SCOPE_OF_SESSION, &UsrObjSet, mfail)) goto EXIT;
	oiSqlPrint(Sql_ptr);fflush(stdout);
	printf("\nQuery User... %d!!!",setSize(UsrObjSet));fflush(stdout);
	//if (*mfail) goto CLEANUP;
	if(setSize(UsrObjSet)>0)
	{
		UsrObj = setGet(UsrObjSet,0);

		if(dstat=objGetAttribute(UsrObj, t5UsrLocationAttr, &StrUsrLoc1)) goto EXIT;
		if (!nlsIsStrNull(StrUsrLoc1)) StrUsrLoc1Dup = nlsStrDup(StrUsrLoc1);

		printf("\n APLAna1 StrUsrLoc1Dup : [%s]", StrUsrLoc1Dup); fflush(stdout);

		if(nlsStrCmp(StrUsrLoc1Dup,"CARPLT")!=0)
		{
			if(dstat=objSetAttribute(UsrObj,t5UsrLocationAttr,"CARPLT")) goto EXIT;
			if (dstat = t0UpdateObject(&UsrObj,mfail))goto EXIT;
			printf("\n Location Updated\n");fflush(stdout);
		}
		//TZ 3.59

		if(dstat=objGetAttribute(UsrObj, t5UsrAgencyAttr, &StrUsrAgn1)) goto EXIT;
		if (!nlsIsStrNull(StrUsrAgn1)) StrUsrAgn1Dup = nlsStrDup(StrUsrAgn1);

		printf("\n APLAna1 StrUsrAgn1Dup : [%s]", StrUsrAgn1Dup); fflush(stdout);

		if(nlsStrCmp(StrUsrAgn1Dup,"APL")!=0)
		{
			if(dstat=objSetAttribute(UsrObj,t5UsrAgencyAttr,"APL")) goto EXIT;
			if (dstat = t0UpdateObject(&UsrObj,mfail))goto EXIT;
			printf("\n Location Updated\n");fflush(stdout);
		}
	}
	printf("\n Query Driver VC...!!!"); fflush(stdout);
	if(sqlPtr) oiSqlDispose(sqlPtr); sqlPtr = NULL;
	if (dstat = oiSqlCreateSelect(&sqlPtr)) goto CLEANUP;
	if (dstat = oiSqlWhereEQ(sqlPtr,t5SyscdAttr,"APLVErr")) goto CLEANUP;
	if (dstat = oiSqlWhereAND(sqlPtr)) goto CLEANUP;
	if (dstat = oiSqlWhereEQ(sqlPtr,t5SubSyscdAttr,"SignOffErr")) goto CLEANUP;
	if (dstat = oiSqlWhereAND(sqlPtr)) goto CLEANUP;
	if (dstat = oiSqlWhereEQ(sqlPtr,t5Userinfo1Attr,"DriverVC")) goto CLEANUP;
	if (dstat = oiSqlWhereAND(sqlPtr)) goto CLEANUP;
	if (dstat = oiSqlWhereEQ(sqlPtr,t5Userinfo2Attr,StrUsrLoc1Dup)) goto CLEANUP;
	if (dstat = oiSqlWhereAND(sqlPtr)) goto CLEANUP;
	if (dstat = oiSqlWhereEQ(sqlPtr,t5Userinfo4Attr,"Live")) goto CLEANUP;
	if (dstat = QueryDbObject(t5CntrolClass,sqlPtr,-1,FALSE,SC_SCOPE_OF_SESSION,&soCntrlObj,mfail)) goto CLEANUP;
	if(sqlPtr) oiSqlDispose(sqlPtr); sqlPtr = NULL;

	printf("\n\n Driver VC Control Object count is %d\n",setSize(soCntrlObj)); fflush(stdout);
	if(setSize(soCntrlObj)>0)
	{
		CntrlObj	= setGet(soCntrlObj,0);
		if(dstat=objSetAttribute(setGet(soCntrlObj,0),t5Userinfo4Attr,"NLive")) goto CLEANUP;
		t5CheckMfail(t0UpdateObject(&CntrlObj,mfail));
	}

	if(dstat = SetUpDialog(CmChNtItClass, NULL, CreateDialogCAttr, &extraStrECN, &extraObjECN, &DmlCreateDia, mfail)) goto CLEANUP;
	//if(*mfail) goto CLEANUP;

	printf("start setting \n");	fflush(stdout);

	if (dstat = smSetSessionUsrName ("APLAna1")) goto EXIT;
	if(dstat=objSetAttribute(DmlCreateDia,WbsIDAttr,DMLNameS)) goto CLEANUP;
	if(dstat=objSetAttribute(DmlCreateDia,"t5EcnType","APLSTR")) goto CLEANUP;
	if(dstat=objSetAttribute(DmlCreateDia,"ProjectName",project)) goto CLEANUP;
	if(dstat=objSetAttribute(DmlCreateDia,"WbsName",WbsNameMain)) goto CLEANUP;
	if(dstat=objSetAttribute(DmlCreateDia,"WbsDescription",WbsDescriptionMain)) goto CLEANUP;
	//if(drvVCNumDup)	if(dstat=objSetAttribute(DmlCreateDia,"t5DriverVCNo",drvVCNumDup)) goto EXIT;
	if(dstat=objSetList(DmlCreateDia,DesignGroupListAttr,DsgnGrpValue)) goto EXIT;
	if(dstat=objSetAttribute(DmlCreateDia,WbsPriorityAttr,"CcfPriority3")) goto CLEANUP;
	if(dstat=objSetAttribute(DmlCreateDia,ReasonAttr,"CcfDefect")) goto CLEANUP;

	printf("Before create dml\n");	fflush(stdout);
	//objDump(DmlCreateDia);

	if(dstat=CreateObject(CmChNtItClass,DmlCreateDia,&DmlObj,mfail)) goto CLEANUP;
	printf("dump...DmlObj.. \n");	fflush(stdout);
	//objDump(DmlObj);
	
	if (dstat = smSetSessionUsrName ("super user")) goto EXIT;
	if(dstat=SetUpDialog("CmChNtIt",DmlObj,"UpdateKeyDialogC",&extraStr,&extraObj,&dialogObj,mfail)) goto EXIT;
	//setting Task Number in Dialog and updating the Task Number.
	if(dstat=objSetAttribute(dialogObj,"WbsID",DMLNameS)) goto EXIT;
  //printf("\n Taking dmp second time");
	//objDump(dialogObj);
	if(dstat=(UpdateKeyValueObject(DmlObj,dialogObj,mfail))) goto EXIT;

	if(dstat = ExpandObject5(CmPlRvRvClass,DmlObj,"CMPlanBreaksDownInTo",SC_SCOPE_OF_SESSION,&brkTaskObjsTo,mfail)) goto CLEANUP;
	printf("\nNo of task found : %d",setSize(brkTaskObjsTo));fflush(stdout);
	//sTaskNumber
	if (setSize(brkTaskObjsTo)>0)
	{
		tskObj	=	setGet(brkTaskObjsTo,0);
		
		if(dstat=objGetAttribute(tskObj, WbsIDAttr, &sTaskNum)) goto EXIT;
		if (!nlsIsStrNull(sTaskNum)) sTaskNumDup = nlsStrDup(sTaskNum);

		printf("\nTask Number : %s",sTaskNumDup);fflush(stdout);

		if (nlsStrCmp(sTaskNumDup,sTaskNumber)!=0)
		{
			printf("\nUpdate Task Key...");fflush(stdout);
			dialogObj	=	NULL;
			if(dstat=SetUpDialog("CmTaskIt",tskObj,"UpdateKeyDialogC",&extraStr,&extraObj,&dialogObj,mfail)) goto EXIT;
			//setting Task Number in Dialog and updating the Task Number.
			if(dstat=objSetAttribute(dialogObj,"WbsID",sTaskNumber)) goto EXIT;
			//printf("\n Taking dmp second time");
			//objDump(dialogObj);
			if(dstat=(UpdateKeyValueObject(tskObj,dialogObj,mfail))) goto EXIT;
		}
	}
	
	//if (dstat = smSetSessionUsrName ("Loader")) goto EXIT;
	//if (*mfail)
	//{
	//		printf("\n DML mfail:%d  ........\n",*mfail);fflush(stdout);
	//		mfail=USC_OKAY;
	//}

	printf("\n DML has been Loaded Succesfully End .....\n");fflush(stdout);

    if(DmlObj)
    {
		if(dstat = objCopy(AMDmlObj,DmlObj)) goto CLEANUP;
    }
	if(setSize(soCntrlObj)>0)
	{
		CntrlObj	= setGet(soCntrlObj,0);
		if(dstat=objSetAttribute(setGet(soCntrlObj,0),t5Userinfo4Attr,"NLive")) goto CLEANUP;
		t5CheckMfail(t0UpdateObject(&CntrlObj,mfail));
	}

	CLEANUP:
		printf("\n In CLEANUP Section  \n");fflush(stdout);
			if(DmlObj)   objDispose(DmlObj);
			if(DmlCreateDia)   objDispose(DmlCreateDia);

	EXIT:
		printf("\n In EXIT Section  \n");fflush(stdout);
		if(dstat != OKAY) uiShowFatalError(dstat,WHERE);
		return(dstat);
}


int main (int argc, char *argv[])
{
	int stat							=	0;
	int Relcheck						=	0;
	int	iERCDmlFnd						=	0;
	
	string LoginS						=	NULL;
	string PasswordS					=	NULL;
	string DMLNameDupS					=	NULL;
	string DMLNameS						=	NULL;
	string DMLSynoNameDupS				=	NULL;
	string DMLSynoNameS					=	NULL;
	string DMLProjNameDupS				=	NULL;
	string DMLProjNameS					=	NULL;
	string DMLDesGrpDupS				=	NULL;
	string DMLDesGrpS					=	NULL;
	string DMLDescDupS					=	NULL;
	string DMLDescS						=	NULL;
	string DMLDispositionDupS			=	NULL;
	string DMLDispositionS				=	NULL;
	string DMLAuthorisedByDupS			=	NULL;
	string DMLAuthorisedByS				=	NULL;
	string CreatorDupS					=	NULL;
	string CreatorS						=	NULL;
	string RequestorDupS				=	NULL;
	string RequestorS					=	NULL;
	string AdministratorDupS			=	NULL;
	string AdministratorS				=	NULL;
	string DmlLcsDupS					=	NULL;
	string DmlLcsS						=	NULL;
	string DMLStatDupS					=	NULL;
	string DMLStatS						=	NULL;
	string DMLPlanningStatusDupS		=	NULL;
	string DMLPlanningStatusS			=	NULL;
	string DMLClosureDupS				=	NULL;
	string DMLClosureS					=	NULL;
	string ReasonDupS					=	NULL;
	string ReasonS						=	NULL;
	string t5DmlTypeDupS				=	NULL;
	string t5DmlTypeS					=	NULL;
	string t5RlzLetterRefDupS			=	NULL;
	string t5RlzLetterRefS				=	NULL;
	string t5ReleaseNotes1DupS			=	NULL;
	string t5ReleaseNotes1S				=	NULL;
	string t5DmlAddendaDupS				=	NULL;
	string t5DmlAddendaS				=	NULL;
	string t5PartStatusDupS				=	NULL;
	string t5PartStatusS				=	NULL;
	string t5CloserDaDupS				=	NULL;
	string t5CloserDaS					=	NULL;
	string	DMLTaskDupS					=	NULL;
	string	DMLTaskS					=	NULL;
	string	DMLTaskDescDupS				=	NULL;
	string	DMLTaskDescS				=	NULL;
	string	DMLTaskDsgGrpDupS			=	NULL;
	string	DMLTaskDsgGrpS				=	NULL;
	string	DMLTaskLcsDupS				=	NULL;
	string	DMLTaskLcsS					=	NULL;
	string	DMLTaskStatuDupS			=	NULL;
	string	DMLTaskStatuS				=	NULL;
	string	DMLTaskDispDupS				=	NULL;
	string	DMLTaskDispS				=	NULL;
	string	TskCreatorDupS				=	NULL;
	string	TskCreatorS					=	NULL;
	string	TskRequestorDupS			=	NULL;
	string	TskRequestorS				=	NULL;
	string	TskAnalystDupS				=	NULL;
	string	TskAnalystS					=	NULL;
	string	TskAdministratorDupS		=	NULL;
	string	TskAdministratorS			=	NULL;
	string	DMLOwnerNamesDupS			=	NULL;
	string	DMLOwnerNames				=	NULL;
	string	BasicDMLNumDupS				=	NULL;
	string	BasicDMLNumS				=	NULL;
	string	DMLClosureTimeStampDupS		=	NULL;
	string	DMLClosureTimeStampS		=	NULL;
	string	ErrFile						=	NULL;

	string	TaskPartNameDupS			=	NULL;
	string	TaskPartRevDupS				=	NULL;
	string	TaskPartSeqDupS				=	NULL;
	string	TaskPartNameS				=	NULL;
	string	TaskPartRevS				=	NULL;
	string	TaskPartSeqS				=	NULL;

	string	Effstart_dateDupS			=	NULL;
	string	Effend_dateDupS				=	NULL;
	string	Effstart_dateS				=	NULL;
	string	Effend_dateS				=	NULL;

	string	AnalystDupS					=	NULL;
	string	AnalystS					=	NULL;

	string	currentwrkhost				=	NULL;
	string	DMLTaskMstrObidDupS			=	NULL;
	string	DMLTaskMstrObidS			=	NULL;

	string	CS							=	NULL;
	string	CSDup						=	NULL;
	string	IA							=	NULL;
	string	IADup						=	NULL;
	string	SLOC						=	NULL;
	string	SLOCDup						=	NULL;

	string	ChkStartVewBitPoS			=	NULL;
	string	ChkStartVewBitPoDupS		=	NULL;

	ObjectPtr sessionObj				=	NULL;
	ObjectPtr NewDMLObjP				=	NULL;
	ObjectPtr NewDMLObjDuP				=	NULL;
	ObjectPtr NewDMLTaskObjP			=	NULL;
	ObjectPtr NewDMLTaskObjDuP			=	NULL;
	ObjectPtr TaskMstrObjP				=	NULL;
	ObjectPtr TaskPartobjP				=	NULL;
	ObjectPtr AMDmlObj					=	NULL;
	ObjectPtr NewTaskPartObjRelP		=	NULL;

	SetOfObjects DMLObjSo				=	NULL;
	SetOfObjects ERCDMLObjSo			=	NULL;
	SetOfObjects DMLTaskObjSO			=	NULL;
	SetOfObjects DMLMstrObjSo			=	NULL;
	SetOfObjects TaskPartsObjSO			=	NULL;

	SqlPtr DMLSqlPtr					=	NULL;
	SqlPtr DMLTaskSqlPtr				=	NULL;
	SqlPtr ERCDMLSqlPtr					=	NULL;
	SqlPtr DMLTaskMstrSqlPtr			=	NULL;
	SqlPtr TaskPartSqlPtr				=	NULL;
	SqlPtr sqlPtr						=	NULL;

	string			strDBScope = NULL;
	string			usrDept		= NULL;
	string			usrLocation = NULL;
	string			usrAgency	= NULL;
	SetOfObjects	myPrefSet   = NULL;
	ObjectPtr		myPref		= NULL;

	FILE	*fptr						=	NULL;
	t5MethodInitWMD("APL_DML_STAMPING");

	/* enable multibyte features of Metaphase */

	usrDept			= nlsStrAlloc(30);
	usrLocation		= nlsStrAlloc(8);
	usrAgency		= nlsStrAlloc(10);
	strDBScope      = nlsStrAlloc(25);

	LoginS					=	argv[1];
	PasswordS				=	argv[2];
	DMLNameDupS				=	argv[3];
	printf("\n DML-Part stampig start...!!! \n");

	DMLNameS			=	nlsStrDup(DMLNameDupS);
	DMLSynoNameDupS		=	argv[4];
	DMLSynoNameS		=	nlsStrDup(DMLSynoNameDupS);
	DMLProjNameDupS		=	argv[5];
	DMLProjNameS		=	nlsStrDup(DMLProjNameDupS);	
	DMLDesGrpDupS		=	argv[6];
	DMLDesGrpS			=	nlsStrDup(DMLDesGrpDupS);
	DMLDescDupS			=	argv[7];
	DMLDescS			=	nlsStrDup(DMLDescDupS);

	DMLDispositionDupS	=	argv[8];
	DMLDispositionS		=	nlsStrDup(DMLDispositionDupS);
	DMLAuthorisedByDupS	=	argv[9];
	DMLAuthorisedByS	=	nlsStrDup(DMLAuthorisedByDupS);

	CreatorDupS			=	argv[10];
	CreatorS			=	nlsStrDup(CreatorDupS);
	RequestorDupS		=	argv[11];
	RequestorS			=	nlsStrDup(RequestorDupS);
	AnalystDupS			=	argv[12];
	AnalystS			=	nlsStrDup(AnalystDupS);
	AdministratorDupS	=	argv[13];
	AdministratorS		=	nlsStrDup(AdministratorDupS);
	DmlLcsDupS			=	argv[14];
	DmlLcsS				=	nlsStrDup(DmlLcsDupS);
	DMLStatDupS			=	argv[15];
	DMLStatS			=	nlsStrDup(DMLStatDupS);
	DMLPlanningStatusDupS	=	argv[16];
	DMLPlanningStatusS	=	nlsStrDup(DMLPlanningStatusDupS);
	DMLClosureDupS		=	argv[17];
	DMLClosureS			=	nlsStrDup(DMLClosureDupS);
	ReasonDupS			=	argv[18];
	ReasonS				=	nlsStrDup(ReasonDupS);
	t5DmlTypeDupS		=	argv[19];
	t5DmlTypeS			=	nlsStrDup(t5DmlTypeDupS);
	t5RlzLetterRefDupS	=	argv[20];
	t5RlzLetterRefS		=	nlsStrDup(t5RlzLetterRefDupS);
	t5ReleaseNotes1DupS	=	argv[21];
	t5ReleaseNotes1S	=	nlsStrDup(t5ReleaseNotes1DupS);
	t5DmlAddendaDupS	=	argv[22];
	t5DmlAddendaS		=	nlsStrDup(t5DmlAddendaDupS);
	t5PartStatusDupS	=	argv[23];
	t5PartStatusS		=	nlsStrDup(t5PartStatusDupS);
	t5CloserDaDupS	    =	argv[24];
	t5CloserDaS			=	nlsStrDup(t5CloserDaDupS);

	DMLTaskDupS			=	argv[25];
	DMLTaskS			=	nlsStrDup(DMLTaskDupS);
	DMLTaskDescDupS		=	argv[26];
	DMLTaskDescS		=	nlsStrDup(DMLTaskDescDupS);
	DMLTaskDsgGrpDupS	=	argv[27];
	DMLTaskDsgGrpS		=	nlsStrDup(DMLTaskDsgGrpDupS);
	DMLTaskLcsDupS		=	argv[28];
	DMLTaskLcsS			=	nlsStrDup(DMLTaskLcsDupS);
	DMLTaskStatuDupS	=	argv[29];
	DMLTaskStatuS		=	nlsStrDup(DMLTaskStatuDupS);
	DMLTaskDispDupS		=	argv[30]; 
	DMLTaskDispS		=	nlsStrDup(DMLTaskDispDupS);

	TskCreatorDupS		=	argv[31]; 
	TskCreatorS			=	nlsStrDup(TskCreatorDupS);

	TskRequestorDupS	=	argv[32]; 
	TskRequestorS		=	nlsStrDup(TskRequestorDupS);

	TskAnalystDupS		=	argv[33]; 
	TskAnalystS			=	nlsStrDup(TskAnalystDupS);

	TskAdministratorDupS=	argv[34]; 
	TskAdministratorS	=	nlsStrDup(TskAdministratorDupS);

	DMLOwnerNamesDupS	=	argv[35];
	DMLOwnerNames		=	nlsStrDup(DMLOwnerNamesDupS);
	BasicDMLNumDupS		=	argv[36];
	BasicDMLNumS		=	nlsStrDup(BasicDMLNumDupS);

	DMLClosureTimeStampDupS	=	argv[37];
	DMLClosureTimeStampS=nlsStrDup(DMLClosureTimeStampDupS);

	ErrFile				=	argv[38];
	printf("\n ...check 2... \n");fflush(stdout);

	if (!nlsIsStrNull(argv[39]))	//TODO: check this.
	{
		TaskPartNameDupS=nlsStrAlloc(nlsStrLen(argv[39])+1);
		TaskPartRevDupS=nlsStrAlloc(nlsStrLen(argv[40])+1);
		TaskPartSeqDupS=nlsStrAlloc(nlsStrLen(argv[41])+1);
		
		TaskPartNameDupS	=	argv[39]; 
		TaskPartNameS=nlsStrDup(TaskPartNameDupS);

		TaskPartRevDupS		=	argv[40];
		TaskPartRevS=nlsStrDup(TaskPartRevDupS);

		TaskPartSeqDupS		=	argv[41];
		TaskPartSeqS=nlsStrDup(TaskPartSeqDupS);
		printf("\n TaskPartNameS=%s,TaskPartRevS=%s,TaskPartSeqS=%s\n",TaskPartNameS,TaskPartRevS,TaskPartSeqS); fflush(stdout);
	}

	if (!nlsIsStrNull(argv[42])) 	//TODO: check this.
	{		

		Effstart_dateDupS	=	nlsStrAlloc(nlsStrLen(argv[42])+1);
		Effend_dateDupS		=	nlsStrAlloc(nlsStrLen(argv[43])+1);

		Effstart_dateDupS	=	argv[42];
		Effstart_dateS=nlsStrDup(Effstart_dateDupS);

		Effend_dateDupS		=	argv[43];
		Effend_dateS		=	nlsStrDup(Effend_dateDupS);
		printf("\n Effstart_dateS=%s, Effend_dateS=%s \n",Effstart_dateS,Effend_dateS);
	}

	CS			=	argv[44];
	CSDup		=	nlsStrDup(CS);
	IA			=	argv[45];
	IADup		=	nlsStrDup(IA);
	SLOC		=	argv[46];
	SLOCDup		=	nlsStrDup(SLOC);

	t5CheckDstat(clInitMB2 (argc, &argv, NULL));
	t5CheckDstat(clTestNetwork ());
	t5CheckDstat(clInitialize2 (FALSE));
	t5CheckDstat(clLogin2 (LoginS,PasswordS,&stat));
	if (stat!=OKAY)
	{
		printf("\n Invalid User Name or PasswordS : %s, %s \n", LoginS, PasswordS);fflush(stdout);
		goto EXIT;
	}
	t5CheckDstat(OpenStartupPrefs("StuPref",&stat));
//	if(mfail)
//	{
//		printf("\nStart up pref not set.");fflush(stdout);
//		goto CLEANUP;
//	}
	if (dstat = getUsrDetails(usrDept,usrLocation,usrAgency,mfail)) goto EXIT;
	//Commented for DB Scope set.
	/*if(!nlsStrCmp(usrLocation,"PUNE"))
	{
	    nlsStrCpy(strDBScope,"HNJ & SU DB SCope");
	}
	else if(!nlsStrCmp(usrLocation,"JSR"))
	{
	 	  nlsStrCpy(strDBScope,"JSR_DB_SCOPE_FOR_Release");
	}
	else if(!nlsStrCmp(usrLocation,"LKO"))
	{
	  nlsStrCpy(strDBScope,"LKO_DB_SCOPE_FOR_Release");
	}

	printf("\nInside to Set DB Scope \n");fflush(stdout);
	t5CheckDstat(oiSqlCreateSelect(&sqlPtr));
	t5CheckDstat(oiSqlWhereEQ(sqlPtr,BrowserWindowNameAttr,strDBScope));
	t5CheckMfail(QueryDbObject(SavDbPClass,sqlPtr,1,TRUE,SC_SCOPE_OF_SESSION,&myPrefSet,mfail));

	printf("\nNo of Pref are :%d\n",setSize(myPrefSet));fflush(stdout);
	if(setSize(myPrefSet) > 0)	myPref = setGet(myPrefSet,0);

	if(myPref)
	{
		if(dstat =Open(myPref,mfail));
		if(*mfail) goto CLEANUP;
	}
	if(sqlPtr) oiSqlDispose(sqlPtr);
	printf("\nAfter Setting Up Scop \n");fflush(stdout);*/

	
//	if (dstat=smGetSessionObject(&sessionObj)) goto CLEANUP;
//	if(dstat=objGetAttribute(sessionObj,OwnerDbNameAttr,&currentwrkhost)) goto CLEANUP;
//	printf("\n currentwrkhost %s \n",currentwrkhost); fflush(stdout);
//	
//	if(argv[29])
//	{
//		ScopeS=nlsStrAlloc(nlsStrLen(argv[29])+1);
//		ScopeS=argv[29];
//		t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScp,mfail));
//		for (ii=0;ii<setSize(dbScp) ; ii++)
//		{
//			docobjst=low_set_get(dbScp,ii);
//			printf("\n DB pref check before... :[%s] ",docobjst);fflush(stdout);
//		}
//		result = strtok( ScopeS, ":_:" );
//		while(!nlsIsStrNull(result))
//		{
//			low_set_add_str_unique(dbScp, result);
//			printf("\n [%s] \n", result); fflush(stdout);
//			result = strtok( NULL, ":_:" );
//		}
//	
//		t5CheckDstat(SetDatabaseScope(PdmSessnClass,dbScp,mfail));
//
//		t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScp,mfail));
//
//		for (ii=0;ii<low_set_size(dbScp) ; ii++)
//		{
//			docobjst=low_set_get(dbScp,ii);
//			printf("\n DB pref check - after... :[%s] ",docobjst);fflush(stdout);
//		}
//
//	}


	printf("\n LoginS = %s, PasswordS = %s, DMLNameS = %s,DMLSynoNameS = %s, DMLProjNameS = %s, DMLDesGrpS = %s,DMLDescS = %s\n",LoginS,PasswordS,DMLNameS,DMLSynoNameS,DMLProjNameS,DMLDesGrpS,DMLDescS); fflush(stdout);
	printf("\n DMLDispositionS = %s, DMLAuthorisedByS = %s, CreatorS = %s, RequestorS = %s, AnalystS = %s, AdministratorS = %s, DmlLcsS = %s, DMLStatS = %s, DMLClosureS = %s, DMLPlanningStatusS = %s \n",DMLDispositionS,DMLAuthorisedByS,CreatorS,RequestorS,AnalystS,AdministratorS,DmlLcsS,DMLStatS,DMLClosureS, DMLPlanningStatusS); fflush(stdout);
	printf("\n DMLTaskS = %s, DMLTaskDescS = %s, DMLTaskDsgGrpS = %s, DMLTaskLcsS = %s, DMLTaskStatuS = %s, DMLTaskDispS = %s, DMLOwnerNames = %s ErrFile = %s\n",DMLTaskS,DMLTaskDescS,DMLTaskDsgGrpS,DMLTaskLcsS,DMLTaskStatuS,DMLTaskDispS,DMLOwnerNames,ErrFile); fflush(stdout);
	printf("\n t5DmlAddendaS = %s, ReasonS = %s, DR Status = %s, ",t5DmlAddendaS,ReasonS,t5PartStatusS); fflush(stdout);
	printf("\n t5RlzLetterRefS = %s, t5ReleaseNotes1S = %s",t5RlzLetterRefS,t5ReleaseNotes1S); fflush(stdout);
	printf("\n CS = %s, IA = %s, SLOC = %s",CSDup,IADup,SLOCDup); fflush(stdout);
	
	fptr = fopen(ErrFile,"a+");

	if (nlsStrCmp(DMLProjNameS,"5445")==0 && nlsStrStr(DMLNameS,"AM")!=NULL)
	{
		string	revision	=	NULL;
		revision	=	strtok( TaskPartRevS, ";" );
		printf("\n 1inside task part relation %s, %s 5s \n",TaskPartNameS,revision,TaskPartSeqS);
		if (TaskPartsObjSO) t5FreeSetOfObjects(TaskPartsObjSO);

		t5CheckDstat(oiSqlCreateSelect(&TaskPartSqlPtr));
		t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,PartNumberAttr,TaskPartNameS));
		t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
		t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,RevisionAttr,revision));
		t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
		t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,SequenceAttr,TaskPartSeqS));
		//t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
		//t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,OwnerNameAttr,"Release Vault"));

		t5CheckMfail(QueryWhere(PartClass,TaskPartSqlPtr,&TaskPartsObjSO,mfail));

		printf("\nNo of Part found : %d",setSize(TaskPartsObjSO));fflush(stdout);

		if (setSize(TaskPartsObjSO)==1)
		{
			Relcheck	=	0;
			TaskPartobjP=setGet(TaskPartsObjSO,0);
			//Relcheck=CheckRelstatus(TaskPartobjP,TaskPartobjP,"CmRslts",mfail);
			//if (Relcheck==2)
			{
				printf("\nStamp Part of 5445 Project, so no need to check relationship with task.");fflush(stdout);

				//Stamp CS,IA,SLOC on Part.
				
				t5_UpdCSIASLOC(TaskPartobjP,  CSDup, IADup, SLOCDup,fptr,mfail);
				t5_UpdEffectivity(TaskPartobjP, Effstart_dateS, Effend_dateS,fptr,mfail);


			}
		}
	}
	else if (nlsStrStr(DMLNameS,"PP")!=NULL || nlsStrStr(DMLNameS,"PM")!=NULL)
	{
		printf("\nPP/PM DML Found...!!!");fflush(stdout);

		string DMLNumber		=	NULL;
		
		DMLNumber	=	subString_1(DMLNameS,0,14);
		printf("\nDMLNumber : %s",DMLNumber);fflush(stdout);

		if (DMLObjSo) t5FreeSetOfObjects(DMLObjSo);

		t5CheckDstat(oiSqlCreateSelect(&DMLSqlPtr));
		t5CheckDstat(oiSqlWhereEQ(DMLSqlPtr,WbsIDAttr,DMLNumber));
		t5CheckMfail(QueryWhere(CmChNtItClass,DMLSqlPtr,&DMLObjSo,mfail));

		printf("\nNo of APL DML found : %d",setSize(DMLObjSo));fflush(stdout);

		if (setSize(DMLObjSo)==0)
		{
			printf("\nCheck for ERC DML");fflush(stdout);
			string ERCDMLNUMBER	=	NULL;
			ERCDMLNUMBER	=	subString_1(DMLNameS,0,10);
			printf("\nERC DML NUMBER : %s",ERCDMLNUMBER);fflush(stdout);

			if (ERCDMLObjSo) t5FreeSetOfObjects(ERCDMLObjSo);
			t5CheckDstat(oiSqlCreateSelect(&ERCDMLSqlPtr));
			t5CheckDstat(oiSqlWhereEQ(ERCDMLSqlPtr,WbsIDAttr,ERCDMLNUMBER));
			t5CheckMfail(QueryWhere(CmChRqItClass,ERCDMLSqlPtr,&DMLObjSo,mfail));

			printf("\nNo of ERC DML Found : %d",setSize(DMLObjSo));fflush(stdout);

			if (setSize(DMLObjSo)<=0)
			{
				fprintf(fptr,"\n%s ERC DML Not found, so APL DML not created",ERCDMLNUMBER);fflush(fptr);
				goto CLEANUP;
			}
			else
			{
				iERCDmlFnd++;
				fprintf(fptr,"\n%s ERC DML found, but APL DML not created",ERCDMLNUMBER);fflush(fptr);
			}
		}
		//else
		{
			printf("\n \t\t DML  Found...!!!\t\t\n");
			NewDMLObjP=setGet(DMLObjSo,0);
			t5CheckDstat(objCopy(&NewDMLObjDuP,NewDMLObjP));

			if(!nlsIsStrNull(DMLTaskS))
			{

				string TaskNumber		=	NULL;
				
				if (iERCDmlFnd>0)
				{
					TaskNumber	=	subString_1(DMLTaskS,0,13);	//17PP081000_47
				}
				else
				{
					TaskNumber	=	subString_1(DMLTaskS,0,17);	//17PP081000_47_APL
				}
				printf("\nDMLNumber : %s",DMLNumber);fflush(stdout);

				if (DMLTaskObjSO) t5FreeSetOfObjects(DMLTaskObjSO);
				t5CheckDstat(oiSqlCreateSelect(&DMLTaskSqlPtr));
				t5CheckDstat(oiSqlWhereEQ(DMLTaskSqlPtr,WbsIDAttr,TaskNumber));		
				t5CheckMfail(QueryWhere(CmTaskItClass,DMLTaskSqlPtr,&DMLTaskObjSO,mfail));

				printf("\nNo Of Task found : %d",setSize(DMLTaskObjSO));fflush(stdout);

				if (setSize(DMLTaskObjSO)<=0)
				{
					printf("\n%s Task not found...",TaskNumber);fflush(stdout);
					fprintf(fptr,"\n%s Task not found",TaskNumber);fflush(fptr);
					//If DML will load then need to set Attribute on Task also
				}
				else
				{
					printf("\n Task Already Loaded !!!\t\t \n");
					NewDMLTaskObjP=setGet(DMLTaskObjSO,0);
					t5CheckDstat(objCopy(&NewDMLTaskObjDuP,NewDMLTaskObjP));

					//t5CheckDstat(t5_setTaskAttribute(NewDMLTaskObjDuP,"LcsAplRlzd",DMLTaskDispS,DMLTaskStatuS,"APLAna1","APLAna1",TskAnalystS));

					t5CheckDstat(objGetAttribute(NewDMLTaskObjDuP,ItemMstrOBIDAttr,&DMLTaskMstrObidDupS));
					DMLTaskMstrObidS=nlsStrDup(DMLTaskMstrObidDupS);

					if (DMLMstrObjSo) t5FreeSetOfObjects(DMLMstrObjSo);

					t5CheckDstat(oiSqlCreateSelect(&DMLTaskMstrSqlPtr));
					t5CheckDstat(oiSqlWhereEQ(DMLTaskMstrSqlPtr,OBIDAttr,DMLTaskMstrObidS));
					t5CheckMfail(QueryWhere(CmTaskMrClass,DMLTaskMstrSqlPtr,&DMLMstrObjSo,mfail));

					printf("\nNo of Task Master found : %d",setSize(DMLMstrObjSo));fflush(stdout);

					TaskMstrObjP= setGet(DMLMstrObjSo,0);

					Relcheck=CheckRelstatus(NewDMLObjDuP,TaskMstrObjP,"CmPlStrc",mfail);

					printf("\n\n relcheck for Breaks down into Product Plan =%d\n",Relcheck);
					if (Relcheck==1)
					{
						printf("\nDML and Task not exist");fflush(stdout);
						fprintf(fptr,"\n%s DML and %s Task relation not exist, please check",DMLNumber,DMLTaskS);fflush(fptr);
						//FOR AM DML CODE CHANGE REQUIRED,
						//COMMENT GOTO STATEMENT AND CREATE THE RELATION.
						goto CLEANUP;
					}
					else
					{
						printf("\n %s DML and %s Task relation exist  \n",DMLNumber,DMLTaskS);
					}
					if (iERCDmlFnd==0)
					{
						if(dstat=ChangeStateItem(NewDMLTaskObjP,"LcsAplRlzd",mfail));
					}
					if(!nlsIsStrNull(TaskPartNameS))
					{
						printf("\n 2inside task part relation\n");

						string	revision	=	NULL;
						revision	=	strtok( TaskPartRevS, ";" );
						printf("\n 2inside task part relation %s, %s %s\n",TaskPartNameS,revision,TaskPartSeqS);

						if (TaskPartsObjSO) t5FreeSetOfObjects(TaskPartsObjSO);

						t5CheckDstat(oiSqlCreateSelect(&TaskPartSqlPtr));
						t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,PartNumberAttr,TaskPartNameS));
						t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
						t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,RevisionAttr,revision));
						t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
						t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,SequenceAttr,TaskPartSeqS));
						//t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
						//t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,OwnerNameAttr,"Release Vault"));

						t5CheckMfail(QueryWhere(PartClass,TaskPartSqlPtr,&TaskPartsObjSO,mfail));

						printf("\nNo of Part found : %d",setSize(TaskPartsObjSO));fflush(stdout);

						if (setSize(TaskPartsObjSO)==1)
						{
							Relcheck	=	0;
							TaskPartobjP=setGet(TaskPartsObjSO,0);
							Relcheck=CheckRelstatus(NewDMLTaskObjDuP,TaskPartobjP,"CmRslts",mfail);
							if (Relcheck==2)
							{
								printf("\nReleation exist between part and task");fflush(stdout);

								//Stamp CS,IA,SLOC on Part.
								
								t5_UpdCSIASLOC(TaskPartobjP,  CSDup, IADup, SLOCDup,fptr,mfail);
								t5_UpdEffectivity(TaskPartobjP, Effstart_dateS, Effend_dateS,fptr,mfail);


							}
							else if (Relcheck==1)
							{
								//FOR AM DML CODE CHANGE REQUIRED,
								//COMMENT GOTO STATEMENT AND CREATE THE RELATION.
								printf("\nRelation not exist between Task %s and Partnumber %s, Revision %s, Sequence : %s",DMLTaskS,TaskPartNameS,revision,TaskPartSeqS);fflush(stdout);
								fprintf(fptr,"\nRelation not exist between Task %s and Partnumber %s, Revision %s, Sequence : %s",DMLTaskS,TaskPartNameS,revision,TaskPartSeqS);fflush(fptr);
							}
						}
						else if (setSize(TaskPartsObjSO)>1)
						{
							printf("\nMore than one part found with same revision and sequence");fflush(stdout);
							fprintf(fptr,"\n Part Number %s having more object with same revision and sequence",TaskPartNameS);fflush(fptr);
						}
						else
						{
							printf("\nNo part found with same revision and sequence");fflush(stdout);
							fprintf(fptr,"\n Part Number %s is not availble in PLM",TaskPartNameS);fflush(fptr);
						}

					}

				}
			}
			//Stamp DML and transfer to release cault
			if (iERCDmlFnd==0)
			{
				if(dstat=ChangeStateItem(NewDMLObjP,"LcsAplRlzd",mfail));
			}
		}
	}
	else if (nlsStrStr(DMLNameS,"AM")!=NULL)
	{
		printf("\nChecking for AMDML");fflush(stdout);

		
		string DMLNumber		=	NULL;
		
		DMLNumber	=	subString_1(DMLNameS,0,14);
		printf("\nDMLNumber : %s",DMLNumber);fflush(stdout);

		if (DMLObjSo) t5FreeSetOfObjects(DMLObjSo);

		t5CheckDstat(oiSqlCreateSelect(&DMLSqlPtr));
		t5CheckDstat(oiSqlWhereEQ(DMLSqlPtr,WbsIDAttr,DMLNumber));
		t5CheckMfail(QueryWhere(CmChNtItClass,DMLSqlPtr,&DMLObjSo,mfail));

		printf("\nNo of AM DML found : %d",setSize(DMLObjSo));fflush(stdout);

		if (setSize(DMLObjSo)>0)
		{
			printf("\nAM DML %s is already present, Proceed same as PP/PM series DML.",DMLNumber);fflush(stdout);
			//find the task attached with DML, check part is attached with task aur not if not attach the part and update the CS/IA/SLOC/Effectivity of the part
			AMDmlObj=setGet(DMLObjSo,0);
			if (AMDmlObj!=NULL)
			{
				//Attached Part with Task,only if task is created
				if(!nlsIsStrNull(DMLTaskS))
				{

					string TaskNumber		=	NULL;
			
					TaskNumber	=	subString_1(DMLTaskS,0,17);	//17PP081000_47_APL
					printf("\nDMLNumber : %s",TaskNumber);fflush(stdout);

					if (DMLTaskObjSO) t5FreeSetOfObjects(DMLTaskObjSO);
					t5CheckDstat(oiSqlCreateSelect(&DMLTaskSqlPtr));
					t5CheckDstat(oiSqlWhereEQ(DMLTaskSqlPtr,WbsIDAttr,TaskNumber));		
					t5CheckMfail(QueryWhere(CmTaskItClass,DMLTaskSqlPtr,&DMLTaskObjSO,mfail));

					printf("\nNo Of Task found : %d",setSize(DMLTaskObjSO));fflush(stdout);

					if (setSize(DMLTaskObjSO)<=0)
					{
						printf("\n%s Task not found...",TaskNumber);fflush(stdout);
						fprintf(fptr,"\n%s Task not found, need to check why task is not created after DML created.",TaskNumber);fflush(fptr);
						//If DML will load then need to set Attribute on Task also
					}
					else
					{
						printf("\n Task Already Loaded !!!\t\t \n");
						NewDMLTaskObjP=setGet(DMLTaskObjSO,0);
						t5CheckDstat(objCopy(&NewDMLTaskObjDuP,NewDMLTaskObjP));

						//t5CheckDstat(t5_setTaskAttribute(NewDMLTaskObjDuP,"LcsAplRlzd",DMLTaskDispS,DMLTaskStatuS,"APLAna1","APLAna1",TskAnalystS));

						t5CheckDstat(objGetAttribute(NewDMLTaskObjDuP,ItemMstrOBIDAttr,&DMLTaskMstrObidDupS));
						DMLTaskMstrObidS=nlsStrDup(DMLTaskMstrObidDupS);

						if (DMLMstrObjSo) t5FreeSetOfObjects(DMLMstrObjSo);

						t5CheckDstat(oiSqlCreateSelect(&DMLTaskMstrSqlPtr));
						t5CheckDstat(oiSqlWhereEQ(DMLTaskMstrSqlPtr,OBIDAttr,DMLTaskMstrObidS));
						t5CheckMfail(QueryWhere(CmTaskMrClass,DMLTaskMstrSqlPtr,&DMLMstrObjSo,mfail));

						printf("\nNo of Task Master found : %d",setSize(DMLMstrObjSo));fflush(stdout);

						TaskMstrObjP= setGet(DMLMstrObjSo,0);

						Relcheck=CheckRelstatus(AMDmlObj,TaskMstrObjP,"CmPlStrc",mfail);

						printf("\n\n relcheck for Breaks down into Product Plan =%d\n",Relcheck);
						if (Relcheck==1)
						{
							printf("\nDML and Task not exist");fflush(stdout);
							fprintf(fptr,"\n%s DML and %s Task relation not exist, please check",DMLNumber,DMLTaskS);fflush(fptr);
							//FOR AM DML CODE CHANGE REQUIRED,
							//COMMENT GOTO STATEMENT AND CREATE THE RELATION.

							//goto CLEANUP;
						}
						else
						{
							printf("\n %s DML and %s Task relation exist  \n",DMLNumber,DMLTaskS);
						}

						if(!nlsIsStrNull(TaskPartNameS))
						{
							printf("\n 3inside task part relation\n");

							string	revision	=	NULL;
							revision	=	strtok( TaskPartRevS, ";" );
							printf("\n 3inside task part relation %s, %s %s \n",TaskPartNameS,revision,TaskPartSeqS);
							if (TaskPartsObjSO) t5FreeSetOfObjects(TaskPartsObjSO);

							t5CheckDstat(oiSqlCreateSelect(&TaskPartSqlPtr));
							t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,PartNumberAttr,TaskPartNameS));
							t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
							t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,RevisionAttr,revision));
							t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
							t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,SequenceAttr,TaskPartSeqS));
							//t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
							//t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,OwnerNameAttr,"Release Vault"));

							t5CheckMfail(QueryWhere(PartClass,TaskPartSqlPtr,&TaskPartsObjSO,mfail));

							printf("\nNo of Part found : %d",setSize(TaskPartsObjSO));fflush(stdout);

							if (setSize(TaskPartsObjSO)==1)
							{
								Relcheck	=	0;
								TaskPartobjP=setGet(TaskPartsObjSO,0);
								Relcheck=CheckRelstatus(NewDMLTaskObjDuP,TaskPartobjP,"CmRslts",mfail);
								if (Relcheck==2)
								{
									printf("\nReleation exist between part and task");fflush(stdout);

									//Stamp CS,IA,SLOC on Part.
									
									t5_UpdCSIASLOC(TaskPartobjP,  CSDup, IADup, SLOCDup,fptr,mfail);
									t5_UpdEffectivity(TaskPartobjP, Effstart_dateS, Effend_dateS,fptr,mfail);


								}
								else if (Relcheck==1)
								{
									//FOR AM DML CODE CHANGE REQUIRED,
									//COMMENT GOTO STATEMENT AND CREATE THE RELATION.
									printf("\nRelation not exist between Task %s and Partnumber %s, Revision %s, Sequence : %s create relatoin",DMLTaskS,TaskPartNameS,TaskPartRevS,TaskPartSeqS);fflush(stdout);
									t5CheckMfail(CreateRelation(CmRsltsClass,NewDMLTaskObjDuP,"CMPrdPlnResultsInBusItem",TaskPartobjP,&NewTaskPartObjRelP,mfail));
									t5CheckDstat(objGetAttribute(TaskPartobjP,StartingViewBitPosAttr,&ChkStartVewBitPoS));
									printf("\nOld ChkStartVewBitPoS=%s",ChkStartVewBitPoS);
								
									t5CheckDstat(objSetAttribute(TaskPartobjP,StartingViewBitPosAttr,"1"));
									t5CheckDstat(objGetAttribute(TaskPartobjP,StartingViewBitPosAttr,&ChkStartVewBitPoDupS));
									printf("\t New ChkStartVewBitPoDupS=%s\n",ChkStartVewBitPoDupS);
									//fprintf(fptr,"\nRelation not exist between Task %s and Partnumber %s, Revision %s, Sequence : %s",DMLTaskS,TaskPartNameS,TaskPartRevS,TaskPartSeqS);fflush(fptr);

									t5_UpdCSIASLOC(TaskPartobjP,  CSDup, IADup, SLOCDup,fptr,mfail);
									t5_UpdEffectivity(TaskPartobjP, Effstart_dateS, Effend_dateS,fptr,mfail);
								}
							}
							else if (setSize(TaskPartsObjSO)>1)
							{
								printf("\nMore than one part found with same revision and sequence");fflush(stdout);
								fprintf(fptr,"\n Part Number %s having more object with same revision and sequence",TaskPartNameS);fflush(fptr);
							}
							else
							{
								printf("\nNo part found with same revision and sequence");fflush(stdout);
								fprintf(fptr,"\n Part Number %s is not availble in PLM",TaskPartNameS);fflush(fptr);
							}

						}

					}
				}
			}
			else
			{
				//find the task attached with DML, check part is attached with task aur not if not attach the part and update the CS/IA/SLOC/Effectivity of the part
				printf("\nDML create issue.");fflush(stdout);
			}

		}
		else
		{
			printf("\nGoing to create AM DML");fflush(stdout);
			//status AMDMLCreateFunc(string project,string DMLNameS,string sDesignGrp,string WbsNameMain,string WbsDescriptionMain,ObjectPtr *AMDmlObj)
			
			string TaskNumber		=	NULL;

			TaskNumber	=	subString_1(DMLTaskS,0,17);	//17PP081000_47_APL
			printf("\nDMLNumber : %s",DMLNumber);fflush(stdout);

			if(dstat = AMDMLCreateFunc(DMLProjNameS,DMLNumber,TaskNumber,DMLDesGrpS,DMLSynoNameS,DMLSynoNameS,&AMDmlObj,mfail)) goto EXIT;
			if (AMDmlObj!=NULL)
			{
				//Attached Part with Task,only if task is created
				if(!nlsIsStrNull(DMLTaskS))
				{

					TaskNumber		=	NULL;
			
					TaskNumber	=	subString_1(DMLTaskS,0,17);	//17PP081000_47_APL
					printf("\nDMLNumber : %s",DMLNumber);fflush(stdout);

					if (DMLTaskObjSO) t5FreeSetOfObjects(DMLTaskObjSO);
					t5CheckDstat(oiSqlCreateSelect(&DMLTaskSqlPtr));
					t5CheckDstat(oiSqlWhereEQ(DMLTaskSqlPtr,WbsIDAttr,TaskNumber));		
					t5CheckMfail(QueryWhere(CmTaskItClass,DMLTaskSqlPtr,&DMLTaskObjSO,mfail));

					printf("\nNo Of Task found : %d",setSize(DMLTaskObjSO));fflush(stdout);

					if (setSize(DMLTaskObjSO)<=0)
					{
						printf("\n%s Task not found...",DMLTaskS);fflush(stdout);
						fprintf(fptr,"\n%s Task not found, need to check why task is not created after DML created.",DMLTaskS);fflush(fptr);
						//If DML will load then need to set Attribute on Task also
					}
					else
					{
						printf("\n Task Already Loaded !!!\t\t \n");
						NewDMLTaskObjP=setGet(DMLTaskObjSO,0);
						t5CheckDstat(objCopy(&NewDMLTaskObjDuP,NewDMLTaskObjP));

						//t5CheckDstat(t5_setTaskAttribute(NewDMLTaskObjDuP,"LcsAplRlzd",DMLTaskDispS,DMLTaskStatuS,"APLAna1","APLAna1",TskAnalystS));

						t5CheckDstat(objGetAttribute(NewDMLTaskObjDuP,ItemMstrOBIDAttr,&DMLTaskMstrObidDupS));
						DMLTaskMstrObidS=nlsStrDup(DMLTaskMstrObidDupS);

						if (DMLMstrObjSo) t5FreeSetOfObjects(DMLMstrObjSo);

						t5CheckDstat(oiSqlCreateSelect(&DMLTaskMstrSqlPtr));
						t5CheckDstat(oiSqlWhereEQ(DMLTaskMstrSqlPtr,OBIDAttr,DMLTaskMstrObidS));
						t5CheckMfail(QueryWhere(CmTaskMrClass,DMLTaskMstrSqlPtr,&DMLMstrObjSo,mfail));

						printf("\nNo of Task Master found : %d",setSize(DMLMstrObjSo));fflush(stdout);

						TaskMstrObjP= setGet(DMLMstrObjSo,0);

						Relcheck=CheckRelstatus(AMDmlObj,TaskMstrObjP,"CmPlStrc",mfail);

						printf("\n\n relcheck for Breaks down into Product Plan =%d\n",Relcheck);
						if (Relcheck==1)
						{
							printf("\nDML and Task not exist");fflush(stdout);
							fprintf(fptr,"\n%s DML and %s Task relation not exist, please check",DMLNumber,DMLTaskS);fflush(fptr);
							//FOR AM DML CODE CHANGE REQUIRED,
							//COMMENT GOTO STATEMENT AND CREATE THE RELATION.

							//goto CLEANUP;
						}
						else
						{
							printf("\n %s DML and %s Task relation exist  \n",DMLNumber,DMLTaskS);
						}

						if(!nlsIsStrNull(TaskPartNameS))
						{
							printf("\n 1inside task part relation %s\n",TaskPartRevS);

							string	revision	=	NULL;
							revision	=	strtok( TaskPartRevS, ";" );
							printf("\n 1inside task part relation %s, %s 5s \n",TaskPartNameS,revision,TaskPartSeqS);
							if (TaskPartsObjSO) t5FreeSetOfObjects(TaskPartsObjSO);

							t5CheckDstat(oiSqlCreateSelect(&TaskPartSqlPtr));
							t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,PartNumberAttr,TaskPartNameS));
							t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
							t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,RevisionAttr,revision));
							t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
							t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,SequenceAttr,TaskPartSeqS));
							//t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
							//t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,OwnerNameAttr,"Release Vault"));

							t5CheckMfail(QueryWhere(PartClass,TaskPartSqlPtr,&TaskPartsObjSO,mfail));

							printf("\nNo of Part found : %d",setSize(TaskPartsObjSO));fflush(stdout);

							if (setSize(TaskPartsObjSO)==1)
							{
								Relcheck	=	0;
								TaskPartobjP=setGet(TaskPartsObjSO,0);
								Relcheck=CheckRelstatus(TaskPartobjP,TaskPartobjP,"CmRslts",mfail);
								if (Relcheck==2)
								{
									printf("\nReleation exist between part and task");fflush(stdout);

									//Stamp CS,IA,SLOC on Part.
									
									t5_UpdCSIASLOC(TaskPartobjP,  CSDup, IADup, SLOCDup,fptr,mfail);
									t5_UpdEffectivity(TaskPartobjP, Effstart_dateS, Effend_dateS,fptr,mfail);


								}
								else if (Relcheck==1)
								{
									//FOR AM DML CODE CHANGE REQUIRED,
									//COMMENT GOTO STATEMENT AND CREATE THE RELATION.
									printf("\nRelation not exist between Task %s and Partnumber %s, Revision %s, Sequence : %s create relatoin",DMLTaskS,TaskPartNameS,TaskPartRevS,TaskPartSeqS);fflush(stdout);
									t5CheckMfail(CreateRelation(CmRsltsClass,NewDMLTaskObjDuP,"CMPrdPlnResultsInBusItem",TaskPartobjP,&NewTaskPartObjRelP,mfail));
									t5CheckDstat(objGetAttribute(TaskPartobjP,StartingViewBitPosAttr,&ChkStartVewBitPoS));
									printf("\nOld ChkStartVewBitPoS=%s",ChkStartVewBitPoS);
								
									t5CheckDstat(objSetAttribute(TaskPartobjP,StartingViewBitPosAttr,"1"));
									t5CheckDstat(objGetAttribute(TaskPartobjP,StartingViewBitPosAttr,&ChkStartVewBitPoDupS));
									printf("\t New ChkStartVewBitPoDupS=%s\n",ChkStartVewBitPoDupS);
									//fprintf(fptr,"\nRelation not exist between Task %s and Partnumber %s, Revision %s, Sequence : %s",DMLTaskS,TaskPartNameS,TaskPartRevS,TaskPartSeqS);fflush(fptr);

									t5_UpdCSIASLOC(TaskPartobjP,  CSDup, IADup, SLOCDup,fptr,mfail);
									t5_UpdEffectivity(TaskPartobjP, Effstart_dateS, Effend_dateS,fptr,mfail);
								}
							}
							else if (setSize(TaskPartsObjSO)>1)
							{
								printf("\nMore than one part found with same revision and sequence");fflush(stdout);
								fprintf(fptr,"\n Part Number %s having more object with same revision and sequence",TaskPartNameS);fflush(fptr);
							}
							else
							{
								printf("\nNo part found with same revision and sequence");fflush(stdout);
								fprintf(fptr,"\n Part Number %s is not availble in PLM",TaskPartNameS);fflush(fptr);
							}

						}

					}
				}
			}
			else
			{
				//find the task attached with DML, check part is attached with task aur not if not attach the part and update the CS/IA/SLOC/Effectivity of the part
				printf("\nDML create issue.");fflush(stdout);
			}
		}
	}
	else
	{
		if (nlsStrCmp(DMLProjNameS,"5445")==0)
		{
			string	revision	=	NULL;
			revision	=	strtok( TaskPartRevS, ";" );
			printf("\n 1inside task part relation %s, %s 5s \n",TaskPartNameS,revision,TaskPartSeqS);
			if (TaskPartsObjSO) t5FreeSetOfObjects(TaskPartsObjSO);

			t5CheckDstat(oiSqlCreateSelect(&TaskPartSqlPtr));
			t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,PartNumberAttr,TaskPartNameS));
			t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
			t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,RevisionAttr,revision));
			t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
			t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,SequenceAttr,TaskPartSeqS));
			//t5CheckDstat(oiSqlWhereAND(TaskPartSqlPtr));
			//t5CheckDstat(oiSqlWhereEQ(TaskPartSqlPtr,OwnerNameAttr,"Release Vault"));

			t5CheckMfail(QueryWhere(PartClass,TaskPartSqlPtr,&TaskPartsObjSO,mfail));

			printf("\nNo of Part found : %d",setSize(TaskPartsObjSO));fflush(stdout);

			if (setSize(TaskPartsObjSO)==1)
			{
				Relcheck	=	0;
				TaskPartobjP=setGet(TaskPartsObjSO,0);
				//Relcheck=CheckRelstatus(TaskPartobjP,TaskPartobjP,"CmRslts",mfail);
				//if (Relcheck==2)
				{
					printf("\nStamp Part of 5445 Project, so no need to check relationship with task.");fflush(stdout);

					//Stamp CS,IA,SLOC on Part.
					
					t5_UpdCSIASLOC(TaskPartobjP,  CSDup, IADup, SLOCDup,fptr,mfail);
					t5_UpdEffectivity(TaskPartobjP, Effstart_dateS, Effend_dateS,fptr,mfail);


				}
			}
		}
	}


CLEANUP:
	t5PrintCleanUpModName;

EXIT:
	t5CheckDstatAndReturn;
	return (*mfail);
}