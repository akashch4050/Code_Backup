/***************************************************************************************************************************************
* Copyright (c) 2004 Tata Technologies Limited (TTL). All Rights Reserved
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File					:	ExCatiaV5Reg.c
*  Created By			:	Moorthy J
*  Created On			:	07-07-2006
*  Project				:	TATA MOTORS - PLM CADBOM (CMI) Catia V4/Catia V5 Data Migration
*  Purpose				:	This code is mainly Register as a V4(.model)/V5 Objects(.CATPart,.CATProduct,.CATDrawing,.CMIArchieve)
*							and check in to Respective Vaults as per the input.
*							Then creates an supersedes relationship
*
* Modification History :
* S.No    Date        CR No      Modified By            Modification Notes
* -----------------------------------------------------------------------------------------------------------------------------------------
*    1	21/04/2008				 Naveen S			> Registering the CATIA DI
*													> Creating Document,TC Part
*													> Creating Attach,PartDoc
*
*	2   29/03/2009     			 Ramesh Sharma		> Modified PartDoc Relation for latest Documents only
*													> Removed Delete relation (Debug)
*													> Creating Infodwg Relations
*													> setting View to Delete existing Uses Parts (Debug)
*
*	3   01/07/2013				 Nikhil Dixit       > Corrected the code for Upload Failures
*													> Modified the Code to enable CE Vault Loading for Live Projects
*													> Enabling Converted Data Loading and t5MulCad and t5MulPrd document Creation
*													> Did Code Restructuring and Added Proper Comments to ease debugging using Logfile.
*													> Solved CheckIn Problem for HNJ Downloaded Parts.
*													> All Parts loading with Class Type as Assembly (09/08/2013)
*  Methods Calling      :   This Program is called through Shell script TC_CadLoader.sh
*  How to Run           :   TC_CadLoader.sh input.txt > test.out &
*  
*********************************************************************************************************************************************/
 

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
//#include <pie/pie.h>

string Flag_1						=	NULL;
int ChechRevType(string, string, integer*);
int SupersedesChk(FILE*,string ,string, string, integer*);
ObjectPtr CheckAndCreateDoc( string, string, string, string, string, string, string, integer*);
ObjectPtr LoadDocRevSeq(ObjectPtr, string, string, string, integer*);
ObjectPtr LoadPartRevSeq(ObjectPtr, string, string, string,string, string,string,string,integer*);
ObjectPtr AssignLoadPartToView(ObjectPtr,integer*);
ObjectPtr CheckAndCreatePart(string, string, string, string, string, string, string, string, string, string,string,string,int,integer*);
int CheckRelstatus(ObjectPtr ,ObjectPtr ,string ,integer*);
status t5CheckInCstAndMstrObject(ObjectPtr PrtCstMstrObj,integer *mfail);


status t0UpdateObject(ObjectPtr *itemObj,integer *mfail)
{
        char            *mod_name       =   "t0UpdateObject";
        status          dstat           =   OKAY;
        string          SclsName        =   NULL;
        string          SclsNameDup     =   NULL;
        string          SframeName      =   NULL;
	string          SframeNameDup       =   NULL;

		//printf("\n .... inside t0UpdateObject ...  updating object ... \n"); fflush(stdout);
	if ( dstat = objGetAttribute(*itemObj,ClassAttr,&SclsName)) 
	    goto CLEANUP;
	SclsNameDup=nlsStrDup(SclsName);
	//printf("\n .... inside t0UpdateObject ...updating object ..SclsName=[%s]...1111\n",SclsName); fflush(stdout);
        if (dstat = BeginDbFrame(SclsName,&SframeName,mfail))
            goto CLEANUP;
        if (*mfail) 
	    goto CLEANUP;
	SframeNameDup=nlsStrDup(SframeName);
	//printf("\n .... inside t0UpdateObject ...  updating object ...SframeName=[%s]...222\n",SframeName); fflush(stdout);
        //objDump(*itemObj); fflush(stdout);
        if (dstat = UpdateDbObject(*itemObj,mfail))
            goto CLEANUP;
        //printf("\nAfter UpdateDbObject ....33333.\n");fflush(stdout);

        if (*mfail != 0)
        {
		//printf("\n .... inside t0UpdateObject ...  updating object ...4444444444 \n"); fflush(stdout);
            if (dstat = ClearDbFrame(SclsNameDup,SframeNameDup,mfail))
				goto CLEANUP;
            goto CLEANUP;
        }
	//printf("\n .... inside t0UpdateObject ...  updating object ... 555555555\n"); fflush(stdout);
	//printf("\nParameters to End DB Frame are:[%s],[%s]",SclsNameDup,SframeNameDup);
        if (dstat = EndDbFrame(SclsNameDup,SframeNameDup,mfail))
            goto CLEANUP;
        if (*mfail) 
			goto CLEANUP;

CLEANUP:
		//printf("\n.... in cleanup t0UpdateObject ...."); fflush(stdout);
    t5PrintCleanUpModName;
		
EXIT:
    if (dstat != OKAY) dlow_return_trace(mod_name, dstat);

    return (dstat);
}

ObjectPtr AssignLoadPartToView(ObjectPtr LoadPrtObjP,integer* mfail)
{
	char *mod_name="AssignLoadPartToView";


	status dstat = OKAY;


	SqlPtr ViewSql			=NULL;


	SetOfObjects ViewSO		=NULL;
	SetOfObjects extraObja		=NULL;

	ObjectPtr viewObjP			=NULL;
	ObjectPtr oViewDia			=NULL;

	SetOfStrings extraStra  = NULL;

	string LoadPartClassDupS  = NULL;
	string LoadPartClassS  = NULL;

	printf("Inside assigning View");fflush(stdout);


	t5CheckDstat(objGetClass(LoadPrtObjP,&LoadPartClassDupS));

	if (!nlsIsStrNull(LoadPartClassDupS)) LoadPartClassS=nlsStrDup(LoadPartClassDupS);



	if(dstat = SetUpDialog(LoadPartClassS,NULL,"AssignToVewNtwkDialog2C",&extraStra,&extraObja,&oViewDia,mfail)) goto EXIT;
	if(dstat = objSetAttribute(oViewDia,NewViewNetworkAttr,"EAS") )   goto EXIT ;
	//if(dstat = objSetAttribute(oViewDia,NewStartingViewNameAttr,"ERC") )   goto EXIT ;//Need to change for APL
	if(dstat = objSetAttribute(oViewDia,NewStartingViewNameAttr,"APL") )   goto EXIT ;//Need to change for APL
	if(dstat= AssignItmToViewNetwork(LoadPrtObjP,oViewDia,mfail))  goto EXIT ;

	if((dstat = oiSqlCreateSelect(&ViewSql)) ||
	(dstat = oiSqlWhereBegParen(ViewSql)) ||
	//(dstat = oiSqlWhereEQ(ViewSql,ViewNameAttr ,"ERC")) ||
	(dstat = oiSqlWhereEQ(ViewSql,ViewNameAttr ,"APL")) ||
	(dstat = oiSqlWhereAND(ViewSql)) ||
	(dstat = oiSqlWhereEQ(ViewSql,ViewNetworkAttr,"EAS")) ||//Need to change for APL
	(dstat = oiSqlWhereEndParen(ViewSql)) ||
	(dstat = QueryWhere(VewClass,ViewSql,&ViewSO,mfail))
	) goto EXIT;

	if(setSize(ViewSO))
	{
		viewObjP= (ObjectPtr)low_set_get(ViewSO,0);
	}
	printf("Inside Freezing the view");fflush(stdout);

	if(dstat = FreezeViewObject(LoadPrtObjP,viewObjP,mfail))  goto EXIT ;
		
CLEANUP:

		if (ViewSO)	t5FreeSetOfObjects(ViewSO);
		if (ViewSql)	t5FreeSqlPtr(ViewSql);
EXIT:
			return(LoadPrtObjP);
}

int CheckRelstatus(ObjectPtr LeftObjP ,ObjectPtr RightObjP ,string RelClass ,	integer* mfail)
{
		int returnVal			=	0;
		SqlPtr RelsqlPtr		=	NULL;

		SetOfObjects relObjSo	=	NULL;

		string LeftObidDupS		=	NULL;
		string LeftObid			=	NULL;
		string RightObidDups	=	NULL;
		string RightObid		=	NULL;
		status dstat = OKAY;

		char *mod_name="RelationChk";

		printf("\n CheckRelstatus(): Inside Relation existence check\n");fflush(stdout);
		
		
		t5CheckDstat(objGetAttribute(LeftObjP,OBIDAttr,&LeftObidDupS));
		if (!nlsIsStrNull(LeftObidDupS)) LeftObid=nlsStrDup(LeftObidDupS);
	
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
	printf("\n CheckRelstatus(): In CLEANUP ");fflush(stdout);

	if (relObjSo)	t5FreeSetOfObjects(relObjSo);
	if (RelsqlPtr)	t5FreeSqlPtr(RelsqlPtr);


EXIT:
	return returnVal;
}


char* subStr(char* mainStringf ,int fromCharf,	int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(toCharf+1);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}


int SupersedesChk(FILE *FileName,char* PartNumber ,char* NewRevS ,	char* NewSeq ,	integer* mfail)
{
	char *mod_name="SupersedesChk";

	int returnVal=0;
	int SeqInt=0;
	status dstat = OKAY;

	SqlPtr PartSqlPtr				=	NULL;
	SqlPtr GetSupersedSqlPtr		=	NULL;

	SetOfObjects RevObjSo			=	NULL;
	SetOfObjects ProAsmSO			=	NULL;
	SetOfObjects ChekInSupSedSO		=	NULL;
	SetOfObjects ChekOutSupSedSO	=	NULL;
	SetOfObjects RevSupSedSO		=	NULL;
	
	ObjectPtr RevObjP				=	NULL;
	ObjectPtr SupSedObjP			=	NULL;
		
    string SupSedRevisionDupS		=	NULL;
    string SupSedRevisionS			=	NULL;
//  string SupSedSequenDupS			=	NULL;
    string SupSedSequenS			=	NULL;
    string tempstr					=	NULL;

	
	if ((nlsStrCmp(NewSeq,"1")==0) && (nlsStrCmp(NewRevS,"NR")==0))
		{
			printf("\n SupersedesChk(): Upload check Bypassed for [%s] as Rev [%s] and Seq [%s]",PartNumber,NewRevS,NewSeq);fflush(stdout);
			returnVal=1;		
		}

		else
		{
			if (nlsStrCmp(NewSeq,"1")==0)
			{				
				t5CheckDstat(oiSqlCreateSelect(&PartSqlPtr));
				t5CheckDstat(oiSqlWhereEQ(PartSqlPtr,RevisionAttr,NewRevS));
				t5CheckDstat(oiSqlWhereAND(PartSqlPtr));
				t5CheckDstat(oiSqlWhereEQ(PartSqlPtr,SequenceAttr,"1"));
				
				t5CheckMfail(QueryDbObject(PartClass,PartSqlPtr,1,TRUE,SC_SCOPE_OF_SESSION,&RevObjSo,mfail));

				printf("\n SupersedesChk(): setSize(RevObjSo)= [%d] && NewRevS = [%s]",setSize(RevObjSo),NewRevS);fflush(stdout);
				if(SupSedRevisionS!=NULL) nlsStrFree(SupSedRevisionS);
				if(SupSedSequenS!=NULL) nlsStrFree(SupSedSequenS);
				if(tempstr!=NULL) nlsStrFree(tempstr);

				if (setSize(RevObjSo)>0)
				{
					RevObjP=setGet(RevObjSo,0);
					t5CheckDstat(ExpandObject5(ChekInRClass,RevObjP,"OriginalOfCheckedInItem", SC_SCOPE_OF_SESSION, &ChekInSupSedSO, mfail));
					t5CheckDstat(ExpandObject5(ChekOutRClass,RevObjP,"OriginalOfCheckedOutItem", SC_SCOPE_OF_SESSION, &ChekOutSupSedSO, mfail));
					t5CheckDstat(ExpandObject5(ReviseRClass,RevObjP,"PreviousRevisionOfItem", SC_SCOPE_OF_SESSION, &RevSupSedSO, mfail));
					printf("\n SupersedesChk():  setSize(ChekInSupSedSO)=%d ,setSize(ChekOutSupSedSO)=%d , setSize(RevSupSedSO)=%d",setSize(ChekInSupSedSO),setSize(ChekOutSupSedSO),setSize(RevSupSedSO));fflush(stdout);
					if (setSize(ChekInSupSedSO)>0)
					{
						SupSedObjP=setGet(ChekInSupSedSO,0);
					}
					else if (setSize(ChekOutSupSedSO)>0)
					{
						SupSedObjP=setGet(ChekOutSupSedSO,0);
					}
					else if (setSize(RevSupSedSO)>0)
					{
						SupSedObjP=setGet(RevSupSedSO,0);
					}
				
					t5CheckDstat(objGetAttribute(SupSedObjP,RevisionAttr,&SupSedRevisionDupS));
					if(SupSedRevisionDupS)  SupSedRevisionS=nlsStrDup(SupSedRevisionDupS);				

					printf("\n SupersedesChk(): SupSedRevisionS = [%s] ",SupSedRevisionS);fflush(stdout);

					t5CheckDstat(oiSqlCreateSelect(&GetSupersedSqlPtr));
					t5CheckDstat(oiSqlWhereEQ(GetSupersedSqlPtr,PartNumberAttr,PartNumber));
					t5CheckDstat(oiSqlWhereAND(GetSupersedSqlPtr));
					t5CheckDstat(oiSqlWhereEQ(GetSupersedSqlPtr,RevisionAttr,SupSedRevisionS));
					t5CheckDstat(oiSqlWhereAND(GetSupersedSqlPtr));
					t5CheckDstat(oiSqlWhereEQ(GetSupersedSqlPtr,OwnerNameAttr,"Release Vault"));

					t5CheckMfail(QueryWhere(PartClass,GetSupersedSqlPtr,&ProAsmSO,mfail));
				}
			}
			else
			{
				printf("\n SupersedesChk(): Sequence being Uploaded [%s] ",NewSeq);fflush(stdout);
				SeqInt = atoi (NewSeq);
				SeqInt =(SeqInt-1);
				tempstr=nlsStrAlloc(2);
				sprintf(tempstr,"%d",SeqInt);
				NewSeq = nlsStrDup(tempstr);

				printf("\n SupersedesChk(): Sequence being checked for [%s] \n",NewSeq);fflush(stdout);

				t5CheckDstat(oiSqlCreateSelect(&GetSupersedSqlPtr));
				t5CheckDstat(oiSqlWhereEQ(GetSupersedSqlPtr,PartNumberAttr,PartNumber));
				t5CheckDstat(oiSqlWhereAND(GetSupersedSqlPtr));
				t5CheckDstat(oiSqlWhereEQ(GetSupersedSqlPtr,RevisionAttr,NewRevS));
				t5CheckDstat(oiSqlWhereAND(GetSupersedSqlPtr));
				t5CheckDstat(oiSqlWhereEQ(GetSupersedSqlPtr,SequenceAttr,NewSeq));

				t5CheckMfail(QueryWhere(PartClass,GetSupersedSqlPtr,&ProAsmSO,mfail));
				SupSedRevisionS=nlsStrAlloc(10);
				SupSedSequenS=nlsStrAlloc(10);
				SupSedRevisionS=nlsStrCpy(SupSedRevisionS,NewRevS);
				SupSedSequenS=nlsStrCpy(SupSedSequenS,NewSeq);
			}
			
		
			printf("\n SupersedesChk(): Number of TC Parts: [%d]",setSize(ProAsmSO));fflush(stdout);

			if (setSize(ProAsmSO) >0)
			{
				if (SupSedSequenS!=NULL)
				{
				printf("\n SupersedesChk(): Earlier Revion TC Part [%s], [%s], [%s] are AVAILABLE",PartNumber,SupSedRevisionS,SupSedSequenS);fflush(stdout);
					
				}
				else
				{
					printf("\n SupersedesChk(): Earlier Revion TC Part [%s], [%s] ,Release Vault  FOUND",PartNumber,SupSedRevisionS);fflush(stdout);				
				}							
				returnVal=1;
		
			}
			else
			{
				if (SupSedSequenS!=NULL)
				{
					printf("\n SupersedesChk(): TC Part [%s],[%s],[%s]  NOT FOUND",PartNumber,SupSedRevisionS,SupSedSequenS);fflush(stdout);
					fprintf(FileName,"\n *#:_:#* [%s],[%s],[%s] is NOT FOUND \n",PartNumber,SupSedRevisionS,SupSedSequenS);
				}
				else
				{
					printf("\n SupersedesChk(): TC Part [%s], [%s] ,Release Vault NOT FOUND",PartNumber,SupSedRevisionS);fflush(stdout);
					fprintf(FileName,"\n (*#:_:#*) [%s],[%s],Release Vault NOT FOUND \n",PartNumber,SupSedRevisionS);				
				}
				returnVal=2;
			}		
		}
		
CLEANUP:
	printf("\n SupersedesChk(): In CLEANUP");fflush(stdout);

EXIT:
	return returnVal;
}


int ChechRevType(char* RevisionS ,	char* CurRevS ,	integer* mfail)
{
	int returnVal=0;

	printf("\n\n ChechRevType(): CurRevS = [%s] , RevisionS = [%s]  " ,CurRevS, RevisionS);fflush(stdout);
	if (!nlsStrCmp("NR",CurRevS))
	{
		printf("\n ChechRevType(): Current Revision is NR so we have to load it ");fflush(stdout);
		returnVal=1;
	}
	else if (!nlsStrCmp("NR",RevisionS))
	{
		printf("\n ChechRevType(): Already Higher version of Loading Available ");fflush(stdout);
		returnVal=2;
	}
	else if (nlsStrLen(CurRevS) > nlsStrLen(RevisionS))
	{
		printf("\n ChechRevType(): Already Higher version of Loading Available ");fflush(stdout);
		returnVal=2;
	}
	else if (nlsStrLen(CurRevS) < nlsStrLen(RevisionS))
	{
		printf("\n ChechRevType(): We have to load the Data ");fflush(stdout);
		returnVal=1;
	}/*  Directly taking Both are same length */
	else
	{
		printf("\n ChechRevType(): We have to do some operation MainCurS = [%s] , MainRevS = [%s], RemCurI = %d, RemRevI = %d ",subStr(CurRevS,0,1),subStr(RevisionS,0,1),atoi(subStr(CurRevS,1,4)), atoi(subStr(RevisionS,1,4)));fflush(stdout);
		if ( atoi(subStr(CurRevS,1,4)) > atoi(subStr(RevisionS,1,4)))
		{
			printf("\n ChechRevType(): Already Higher version of Loading Available ");fflush(stdout);
			returnVal=2;
		}
		else if ( atoi(subStr(CurRevS,1,4)) < atoi(subStr(RevisionS,1,4)))
		{
			printf("\n ChechRevType(): Continue Loading ");fflush(stdout);
			returnVal=1;
		}
		else if ( atoi(subStr(CurRevS,1,4)) == atoi(subStr(RevisionS,1,4)))
		{
			if(nlsStrCmp(subStr(RevisionS,0,1),subStr(CurRevS,0,1)) == 0 )
			{
				printf("\n ChechRevType(): nlsStrLen(MainCurS) == 1 && (!nlsStrCmp(MainRevS,MainCurS)) so, continue seq loading");fflush(stdout);
				returnVal=1;
			}
			else if(nlsStrCmp(subStr(RevisionS,0,1),subStr(CurRevS,0,1)) < 0 )
			{
				printf("\n ChechRevType(): Already Higher version of Loading Available ");fflush(stdout);
				returnVal=2;
			}
			else
			{
				printf("\n ChechRevType(): Load the Revision MainRevS ");fflush(stdout);
				returnVal=1;
			}
		}
	}
EXIT:
	return returnVal;
}

DelDocPartRel(ObjectPtr BIObjP,integer* mfail)
{
	char *mod_name="DelDocPartRel";
	status dstat = OKAY;
	SetOfObjects PrtRelSO=NULL;
	SetOfObjects PrtSO=NULL;
	int mulcnt=0;
	ObjectPtr PrtObjP = NULL ;

	t5CheckMfail(ExpandObject(PartDocClass,BIObjP,"PartsDescribedByDocument",&PrtSO,&PrtRelSO,mfail));
	printf("\n Document attached to Part = [%d]",setSize(PrtSO));


	for (mulcnt=0;mulcnt<setSize(PrtSO) ;mulcnt++ )
	{
		PrtObjP=setGet(PrtSO,mulcnt);
		printf("\n LatestTZUpdate: I am deleting the duplicate Relation of CatProduct document");
		t5CheckMfail(DeleteRelation2(low_set_get(PrtRelSO,mulcnt),mfail));

	}

	CLEANUP:
		printf("\n DelDocPartRel(): In the CLEANUP of DelDocPartRel ");fflush(stdout);

    EXIT:
		
		printf("\n DelDocPartRel(): In the CLEANUP of DelDocPartRel ");fflush(stdout);
}

ObjectPtr LoadDocRevSeq
(
	ObjectPtr BIObjP,
	string LoadRevisionS,
	string LoadSequenceS,
	string LoadVaultStatusS,
	integer* mfail
)
{
	char *mod_name="LoadDocRevSeq";

	string CurRevDupS = NULL;
	string CurRevS = NULL;
	string CurSeqDupS = NULL;
	string CurSeqS = NULL;
	string CheckedOutDupS = NULL;
	string CheckedOutS = NULL;
	string OwnerCheckDupS = NULL;
	string OwnerCheckS = NULL;

	ObjectPtr BIDialogueObjP = NULL ;
	ObjectPtr SecBIObjP = NULL ;

	SetOfObjects BIchkSO=NULL;
	SetOfObjects BIObjrelchkSO=NULL;
	SetOfObjects failedSO=NULL;
	SetOfObjects extraObj = NULL ;
	SetOfObjects DataPdmItemSO = NULL ;
	SetOfObjects DIObjRelSO = NULL ;

	SetOfStrings extraStr  = NULL;

	int LoopCnt1 =0;

	status dstat = OKAY;

	t5CheckDstat(objGetAttribute(BIObjP,RevisionAttr,&CurRevDupS));
	CurRevS=nlsStrDup(CurRevDupS);
	t5CheckDstat(objGetAttribute(BIObjP,SequenceAttr,&CurSeqDupS));
	CurSeqS=nlsStrDup(CurSeqDupS);
	t5CheckDstat(objGetAttribute(BIObjP,CheckedOutAttr,&CheckedOutDupS));
	CheckedOutS=nlsStrDup(CheckedOutDupS);

	printf("\n LoadDocRevSeq(): CurRevS = [%s] CurSeqS = [%s]",CurRevS,CurSeqS);fflush(stdout);
	printf("\n\n LoadDocRevSeq(): LoadRevisionS = [%s] \t CurRevS = [%s]",LoadRevisionS,CurRevS); fflush(stdout);
	while(nlsStrCmp(LoadRevisionS,CurRevS)!=0)
	{
		printf("\n LoadDocRevSeq(): Revise Doc Changing Rev Before Expand Relationship");fflush(stdout);
		t5CheckMfail(ExpandObject(ReviseRClass,BIObjP,"NextRevisionOfItem",&BIchkSO,&BIObjrelchkSO,mfail));

		if(setSize(BIchkSO)==0)				// Revise Code. If Next Revision Does Not Exists. 
		{
			if (nlsStrCmp(CheckedOutS,"+") == 0)
			{
				printf("\n LoadDocRevSeq(): Doc Load inside Check In");fflush(stdout);
				t5CheckMfail(CheckInItem(BIObjP,"Release Vault", mfail));
			}
			else 
			{
				t5CheckDstat(objGetAttribute(BIObjP,OwnerNameAttr,&OwnerCheckDupS));
				OwnerCheckS=nlsStrDup(OwnerCheckDupS);
				printf("\n LoadDocRevSeq(): OwnerCheckS = [%s]",OwnerCheckS);fflush(stdout);
				if (nlsStrCmp(OwnerCheckS,"Release Vault")!=0)
				{
					printf("\n LoadDocRevSeq(): Doc Load inside tranfer");fflush(stdout);
					t5CheckMfail(SetUpDialog(objClass(BIObjP),BIObjP,"TransferDialogC",&extraStr,&extraObj, &BIDialogueObjP,mfail));
					t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"Release Vault"));
					t5CheckDstat(objSetAttribute(BIDialogueObjP,IncludeAttachedDIsAttr,"-"));

					t5CheckMfail(TransferAllObject(BIObjP,BIDialogueObjP,&failedSO,mfail));
				}

			}
			t5CheckMfail(PerformFreeze(BIObjP,mfail));
			//t5CheckDstat(objSetAttribute(BIObjP,LifeCycleStateAttr,"LcsErcRlzd"));//Need to change for APL
			t5CheckDstat(objSetAttribute(BIObjP,LifeCycleStateAttr,"LcsAplRlzd"));//Need to change for APL
			t5CheckMfail(t0UpdateObject(&BIObjP,mfail));

			t5CheckMfail(ReviseItem(BIObjP,&SecBIObjP,mfail));
			DelDocPartRel(SecBIObjP,mfail);
			BIObjP = SecBIObjP ;
		}
		else
		{
			BIObjP= (ObjectPtr)low_set_get(BIchkSO,0);
		}

		t5CheckDstat(objGetAttribute(BIObjP,RevisionAttr,&CurRevDupS));
		CurRevS=nlsStrDup(CurRevDupS);
		t5CheckDstat(objGetAttribute(BIObjP,SequenceAttr,&CurSeqDupS));
		CurSeqS=nlsStrDup(CurSeqDupS);
		t5CheckDstat(objGetAttribute(BIObjP,CheckedOutAttr,&CheckedOutDupS));
		CheckedOutS=nlsStrDup(CheckedOutDupS);

		printf("\n LoadDocRevSeq(): Revise Doc Changing Rev  End CurRevS = [%s] CurSeqS = [%s] CheckedOutS = [%s]" ,CurRevS,CurSeqS,CheckedOutS);fflush(stdout);
	}

	if (nlsStrCmp(LoadRevisionS,CurRevS)==0  && nlsStrCmp(LoadSequenceS,CurSeqS)==0)
	{
		printf("\n LoadDocRevSeq(): Inside same rev and seq NOT CHECKIN ");
	}
	else if (nlsStrCmp(CheckedOutS,"-") == 0) // check In case for  Load revsion and sequence =1 
	{
		printf("\n LoadDocRevSeq(): Inside Checked Out - and VaultStatusS - [%s] CurRevS - [%s] ",LoadVaultStatusS,CurRevS);fflush(stdout);
		t5CheckMfail(SetUpDialog(objClass(BIObjP),BIObjP,"TransferDialogC",&extraStr,&extraObj, &BIDialogueObjP,mfail));
//		t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"WIP Vault"));
		if (nlsStrCmp(Flag_1,"1") == 0)
		{		
			printf("\n LoadDocRevSeq(): Flag_1 ################ = [%s] ",Flag_1);fflush(stdout);
			t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"CE Vault"));			
		}
		else 
		{		
			t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"WIP Vault"));	
		}

		t5CheckDstat(objSetAttribute(BIDialogueObjP,IncludeAttachedDIsAttr,"-"));
		t5CheckMfail(TransferAllObject(BIObjP,BIDialogueObjP,&failedSO,mfail));
	}
	else
	{
//		t5CheckMfail(CheckInItem(BIObjP,"WIP Vault", mfail));		
		if (nlsStrCmp(Flag_1,"1") == 0)
		{	
			printf("\n LoadDocRevSeq(): Flag_1 ################ = [%s] ",Flag_1);	fflush(stdout);
			t5CheckMfail(CheckInItem(BIObjP,"CE Vault", mfail));
		}
		else
		{		
			t5CheckMfail(CheckInItem(BIObjP,"WIP Vault", mfail));	
		}
	}

	t5CheckDstat(objGetAttribute(BIObjP,SequenceAttr,&CurSeqDupS));
	CurSeqS=nlsStrDup(CurSeqDupS);
	
	printf("\n LoadDocRevSeq() : Before Checkin-Checkout sequence change :: LoadSequenceS = [%s] CurSeqS = [%s]",LoadSequenceS,CurSeqS);fflush(stdout);
	while(atoi(LoadSequenceS)>atoi(CurSeqS))		// Checkin - Checkout Code.
	{
		t5CheckMfail(PerformFreeze(BIObjP,mfail));
		//t5CheckDstat(objSetAttribute(BIObjP,LifeCycleStateAttr,"LcsWorking"));//Need to change for APL
		t5CheckDstat(objSetAttribute(BIObjP,LifeCycleStateAttr,"LcsAPLWrkg"));//Need to change for APL
		//t5CheckDstat(ChangeStateItem(BIObjP,"LcsReview",mfail));
		t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
		printf("\n LoadDocRevSeq():Checkin-Checkout sequence change ::  SequenceS = [%s], CurSeqS = [%s] ",LoadSequenceS,CurSeqS );fflush(stdout);
		t5CheckMfail(CheckOutItem(BIObjP,&SecBIObjP,mfail));
		DelDocPartRel(SecBIObjP,mfail);
		t5CheckMfail(ExpandObject(AttachClass,SecBIObjP,"DataItemsAttachedToBusItem",&DataPdmItemSO ,&DIObjRelSO,mfail));
		if(setSize(DIObjRelSO)>0)
		{
			printf("\n Inside for .. deltion of document" );fflush(stdout);
			for (LoopCnt1=0;LoopCnt1<setSize(DIObjRelSO) ; LoopCnt1++)
			{
				t5CheckMfail(DeleteRelation2(setGet(DIObjRelSO,LoopCnt1),mfail));
			}
			
		}

		//t5CheckMfail(CheckInItem(SecBIObjP,NULL,mfail));
		if (atoi(LoadSequenceS) > atoi(CurSeqS) + 1) /// NOTCHECKIN
		{
			if (nlsStrCmp(Flag_1,"1") == 0)
			{	
				printf("\n LoadDocRevSeq(): Flag_1 ################ = [%s] ",Flag_1);	fflush(stdout);
				t5CheckMfail(CheckInItem(SecBIObjP,"CE Vault", mfail));
			}
			else
			{		
				t5CheckMfail(CheckInItem(SecBIObjP,"WIP Vault", mfail));	
			}
		}  /// NOTCHECKIN
		BIObjP = SecBIObjP ;

		t5CheckDstat(objGetAttribute(BIObjP,SequenceAttr,&CurSeqDupS));
		CurSeqS=nlsStrDup(CurSeqDupS);

		////addeded by SKS 
//		if (atoi(LoadSequenceS)==atoi(CurSeqS))
//		{
			t5CheckMfail(ExpandObject(AttachClass,BIObjP,"DataItemsAttachedToBusItem",&DataPdmItemSO ,&DIObjRelSO,mfail));
			if(setSize(DIObjRelSO)>0)
			{
				printf("\n Inside for .. deltion of document" );fflush(stdout);
				for (LoopCnt1=0;LoopCnt1<setSize(DIObjRelSO) ; LoopCnt1++)
				{
					t5CheckMfail(DeleteRelation2(setGet(DIObjRelSO,LoopCnt1),mfail));
				}
				
			}
//		}
		///// ended by SKS
	}
	t5CheckMfail(PerformFreeze(BIObjP,mfail));
	t5CheckMfail(t0UpdateObject(&BIObjP,mfail))


/*	Test Commenting 1/27/2014	
	if ((nlsStrCmp(LoadVaultStatusS,"Released")!=0) && (nlsStrCmp(CurSeqS,LoadSequenceS)==0))
	{
		printf("\n LoadDocRevSeq(): in same ");fflush(stdout);
		t5CheckDstat(objSetAttribute(BIObjP,LifeCycleStateAttr,"LcsWorking"));fflush(stdout);
		//t5CheckDstat(ChangeStateItem(BIObjP,"LcsReview",mfail));
		t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
	}
	else if ((nlsStrCmp(LoadVaultStatusS,"Released")==0) && (nlsStrCmp(CurSeqS,LoadSequenceS)==0))
	{
		printf("\n LoadDocRevSeq(): Transfer to Release Vault ");fflush(stdout);
		t5CheckMfail(SetUpDialog(objClass(BIObjP),BIObjP,"TransferDialogC",&extraStr,&extraObj, &BIDialogueObjP,mfail));
		t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"Release Vault"));
		t5CheckDstat(objSetAttribute(BIDialogueObjP,IncludeAttachedDIsAttr,"-"));

		//t5CheckMfail(TransferObject(BIObjP,VaultLocObjP,mfail));
		t5CheckMfail(TransferAllObject(BIObjP,BIDialogueObjP,&failedSO,mfail));

		t5CheckDstat(objSetAttribute(BIObjP,LifeCycleStateAttr,"LcsErcRlzd"));
		//t5CheckDstat(ChangeStateItem(BIObjP,"LcsErcRlzd",mfail));
		t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
	}
	else
	{
		printf("\n LoadDocRevSeq(): The higher revision of Document is available  \n");fflush(stdout);
	}
*/

	CLEANUP:
		printf("\n LoadDocRevSeq(): In the CLEANUP of LoadDocRevSeq ");fflush(stdout);
		t5FreeSetOfObjects(BIchkSO);
		t5FreeSetOfObjects(BIObjrelchkSO);
		t5FreeSetOfObjects(failedSO);
		t5FreeSetOfObjects(extraObj);

		t5FreeSetOfStrings(extraStr);

    EXIT:
		return(BIObjP);
}


ObjectPtr LoadPartRevSeq
(
	ObjectPtr BIObjP,
	string LoadRevisionS,
	string LoadSequenceS,
	string LoadVaultStatusS,
	string RevCheckInd,
	string mcreator,
	string VerCreator,
	string moddesc,
	integer* mfail
)
{
	char *mod_name="LoadPartRevSeq";

	int LHRHFlag = 0;
	int CountLHRH = 0;
	int vercnt = 0;
	//int k = 0;
	string CurRevDupS = NULL;
	string CurRevS = NULL;
	string CurSeqDupS = NULL;
	string CurSeqS = NULL;
	string CheckedOutDupS = NULL;
	string CheckedOutS = NULL;
	string VerTcDesc = NULL;
	string VerTcDescDup = NULL;
	string OwnerCheckS = NULL;
	string OwnerCheckDupS = NULL;


	ObjectPtr BIDialogueObjP = NULL ;
	ObjectPtr SecBIObjP = NULL ;
	ObjectPtr BIMstrObjP = NULL ;
	ObjectPtr BIMstrObjPDuP = NULL ;
	ObjectPtr LHRHRelObjP = NULL ;
	ObjectPtr NewLHRHObjRelP = NULL ;

	SetOfObjects BIchkSO=NULL;
	SetOfObjects BIObjrelchkSO=NULL;
	SetOfObjects failedSO=NULL;
	SetOfObjects extraObj = NULL ;
	SetOfObjects BIMstrchkSO = NULL ;
	SetOfObjects BIMstrrelchkSO = NULL ;
	SetOfObjects BIRightMstrSO = NULL ;
	SetOfObjects BIRightMstrrelSO = NULL ;
	SetOfObjects BILeftMstrSO = NULL ;
	SetOfObjects BILeftMstrrelSO = NULL ;
	SetOfObjects ChkPartSO = NULL ;
	SetOfObjects AllDoctempSO = NULL ;
	SetOfObjects VerDelDocRelSO = NULL ;
	SetOfObjects VerDelDocSO = NULL ;
	int newRevCreFlag = 0; // flag added on 09 April 2018 by sks so that operation on already created rev seq should be avoided
	int newSeqCreFlag = 0; // flag added on 09 April 2018 by sks so that operation on already created rev seq should be avoided
	SetOfStrings extraStr  = NULL;

	status dstat = OKAY;
	
	printf("\n\n Inside-LoadPartRevSeq(): \n");fflush(stdout);

	t5CheckMfail(ExpandObject(ItemRevClass,BIObjP,"ItemMstrForStrucBIRev",&BIMstrchkSO,&BIMstrrelchkSO,mfail));

	printf("\n LoadPartRevSeq(): %d ",setSize(BIMstrchkSO));fflush(stdout);

	if (setSize(BIMstrchkSO)==1)
	{
		BIMstrObjPDuP=setGet(BIMstrchkSO,0);
		t5CheckDstat(objCopy(&BIMstrObjP,BIMstrObjPDuP));
		t5CheckMfail(t0UpdateObject(&BIMstrObjP,mfail));
		printf("\n 222");fflush(stdout);
		if(dstat=RefreshObject(BIMstrObjP,mfail)) goto EXIT;
		
		t5CheckMfail(ExpandObject(t5LRRelnClass,BIMstrObjP,"t5AsmLeftHasRight",&BIRightMstrSO,&BIRightMstrrelSO,mfail));
		t5CheckMfail(ExpandObject(t5LRRelnClass,BIMstrObjP,"t5AsmRightHasLeft",&BILeftMstrSO,&BILeftMstrrelSO,mfail));
		if ((setSize(BIRightMstrrelSO)==0)&&(setSize(BILeftMstrrelSO)==0))
		{
			printf("\n LoadPartRevSeq(): There is no LH RH Relation\n");fflush(stdout);
		}
		else 
		{		
			printf("\n LoadPartRevSeq(): There is LH RH Relation \n LoadPartRevSeq(): Deleting Relation for uploading TC Part\n");fflush(stdout);
			
			if (setSize(BIRightMstrrelSO)>0)
			{
				printf("\n LoadPartRevSeq(): setSize(BIRightMstrrelSO) = [%d]",setSize(BIRightMstrrelSO));fflush(stdout);
				for(CountLHRH=0;CountLHRH<setSize(BIRightMstrrelSO);CountLHRH++)				
				{
					LHRHFlag=1;
					t5CheckDstat(objSetAttribute(BIMstrObjP,t5LeftRhAttr,"NA"));
					t5CheckMfail(t0UpdateObject(&BIMstrObjP,mfail));

					LHRHRelObjP=setGet(BIRightMstrrelSO,CountLHRH);
					t5CheckMfail(DeleteRelation2(LHRHRelObjP,mfail));
				}
			}
			else 
			{
				printf("\n LoadPartRevSeq(): setSize(BILeftMstrrelSO) = [%d]",setSize(BILeftMstrrelSO));fflush(stdout);
				for(CountLHRH=0;CountLHRH<setSize(BILeftMstrrelSO);CountLHRH++)				
				{
					LHRHFlag=2;
					t5CheckDstat(objSetAttribute(BIMstrObjP,t5LeftRhAttr,"NA"));
					t5CheckMfail(t0UpdateObject(&BIMstrObjP,mfail));
					
					LHRHRelObjP=setGet(BILeftMstrrelSO,CountLHRH);
					t5CheckMfail(DeleteRelation2(LHRHRelObjP,mfail));
				}
			}
		}
	}

	t5CheckDstat(objGetAttribute(BIObjP,RevisionAttr,&CurRevDupS));
	CurRevS=nlsStrDup(CurRevDupS);
	t5CheckDstat(objGetAttribute(BIObjP,SequenceAttr,&CurSeqDupS));
	CurSeqS=nlsStrDup(CurSeqDupS);

	t5CheckDstat(objGetAttribute(BIObjP,CheckedOutAttr,&CheckedOutDupS));
	if (!nlsIsStrNull(CheckedOutDupS))CheckedOutS=nlsStrDup(CheckedOutDupS);


	while(nlsStrCmp(LoadRevisionS,CurRevS)!=0)
	{
		newRevCreFlag=1;

		t5CheckDstat(objGetAttribute(BIObjP,CheckedOutAttr,&CheckedOutDupS));
		if (!nlsIsStrNull(CheckedOutDupS))CheckedOutS=nlsStrDup(CheckedOutDupS);
		t5CheckDstat(objGetAttribute(BIObjP,OwnerNameAttr,&OwnerCheckDupS));
		if (!nlsIsStrNull(OwnerCheckDupS))OwnerCheckS=nlsStrDup(OwnerCheckDupS);
		printf("\n Inside loading revison LoadPartRevSeq(): Owner of the Part is = [%s] checkeout Attr = [%s] Load Revison  = [%s]  current revison = [%s] \n  ",OwnerCheckS,CheckedOutS,LoadRevisionS,CurRevS);fflush(stdout);

		
		
		t5CheckMfail(ExpandObject(ReviseRClass,BIObjP,"NextRevisionOfItem",&BIchkSO,&BIObjrelchkSO,mfail));

		if(setSize(BIchkSO)==0)
		{
			if (nlsStrCmp(CheckedOutS,"+") == 0 && nlsStrCmp(OwnerCheckS,"Release Vault")!=0)
			{
				t5CheckMfail(CheckInItem(BIObjP,"Release Vault", mfail));
			}
			else
			{	if (nlsStrCmp(OwnerCheckS,"Release Vault")!=0)
				{
					t5CheckMfail(SetUpDialog(objClass(BIObjP),BIObjP,"TransferDialogC",&extraStr,&extraObj, &BIDialogueObjP,mfail));
					t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"Release Vault"));
					t5CheckDstat(objSetAttribute(BIDialogueObjP,IncludeAttachedDIsAttr,"+"));

					t5CheckMfail(TransferAllObject(BIObjP,BIDialogueObjP,&failedSO,mfail));
				}
			}
			t5CheckMfail(PerformFreeze(BIObjP,mfail));
			//t5CheckDstat(objSetAttribute(BIObjP,LifeCycleStateAttr,"LcsErcRlzd"));
			//t5CheckDstat(ChangeStateItem(BIObjP,"LcsErcRlzd",mfail));//Need to change for APL
			t5CheckDstat(ChangeStateItem(BIObjP,"LcsAplRlzd",mfail));//Need to change for APL
			
			t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
			if(dstat=RefreshObject(BIObjP,mfail)) goto EXIT;

			t5CheckMfail(ReviseItem(BIObjP,&SecBIObjP,mfail));
			t5CheckMfail(ExpandObject(PartDocClass,SecBIObjP,"DocumentsDescribingPart",&VerDelDocSO,&VerDelDocRelSO,mfail));
			for (vercnt=0;vercnt<setSize(VerDelDocSO);vercnt++)
			{
				t5CheckMfail(DeleteRelation2(low_set_get(VerDelDocRelSO,vercnt),mfail));
			}
			if (nlsStrCmp(RevCheckInd,"NA")==0)
			{
				printf("\n LoadPartRevSeq(): I am Deleting the Revised TC Ptrts and updating the description with Versioned at last");fflush(stdout);

				t5CheckDstat(objGetAttribute(SecBIObjP,NomenclatureAttr,&VerTcDesc));
				printf("\n LoadPartRevSeq(): Before nlsStrStr VerTcDesc =[%s] ",VerTcDesc);fflush(stdout);
				if (nlsStrCmp(RevCheckInd,"NA")==0)
				{
					printf("\n LoadPartRevSeq(): I am Deleting the Checkout/checkin TC Ptrts and updating the description with Versioned at last");fflush(stdout);
					//objDump(SecBIObjP);
					t5CheckMfail(ExpandObject(PartDocClass,SecBIObjP,"DocumentsDescribingPart",&VerDelDocSO,&VerDelDocRelSO,mfail));
					printf("\n LoadPartRevSeq(): Number of Docs to ver TC %d",setSize(VerDelDocSO));fflush(stdout);
					for (vercnt=0;vercnt<setSize(VerDelDocSO);vercnt++)
					{
						//t5CheckMfail(DeleteRelation2(low_set_get(VerDelDocRelSO,vercnt),mfail));
					}
					t5CheckDstat(objGetAttribute(SecBIObjP,NomenclatureAttr,&VerTcDescDup));
					VerTcDesc=nlsStrDup(VerTcDescDup);
					printf("\n LoadPartRevSeq(): Before nlsStrStr VerTcDesc =[%s] ",VerTcDesc);fflush(stdout);
					/*if (nlsStrStr(VerTcDesc,"-Versioned")==NULL)
					{
							printf("There is no \"-Versioned \" in the description so I am adding ");fflush(stdout);
						if (!nlsIsStrNull(VerTcDesc))
						
						{
							printf("inside Concat ");fflush(stdout);

							VerTcDesc=nlsStrCat(VerTcDesc,"-Versioned");
						}
					}*/
					printf("\n LoadPartRevSeq(): VerTcDesc = [%s]",VerTcDesc);fflush(stdout);
					t5CheckDstat(objSetAttribute(SecBIObjP,NomenclatureAttr,VerTcDesc));
				}
			}
			BIObjP = SecBIObjP ;
  		    t5CheckDstat(objSetAttribute(BIObjP,t5DocRemarksAttr,moddesc));
			t5CheckDstat(objSetAttribute(BIObjP,t5VerCreatorAttr,VerCreator));
			t5CheckDstat(objSetAttribute(BIObjP,CreatorAttr,mcreator));
			t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
			if(dstat=RefreshObject(BIObjP,mfail)) goto EXIT;


		}
		else
		{
			BIObjP= (ObjectPtr)low_set_get(BIchkSO,0);
		}

		t5CheckDstat(objGetAttribute(BIObjP,RevisionAttr,&CurRevDupS));
		CurRevS=nlsStrDup(CurRevDupS);
		t5CheckDstat(objGetAttribute(BIObjP,CheckedOutAttr,&CheckedOutDupS));
		CheckedOutS=nlsStrDup(CheckedOutDupS);

		printf("\n LoadPartRevSeq(): RevisionAttr = [%s] SequenceAttr = [%s] \n",CurRevS,CurSeqS);fflush(stdout);
	}
	if (newRevCreFlag==1)
	{
		if (nlsStrCmp(CheckedOutS,"-") == 0)
		{
			printf("\n LoadPartRevSeq(): Inside newRevCreFlag  Checked Out - and VaultStatusS - [%s] CurRevS - [%s]",LoadVaultStatusS,CurRevS);fflush(stdout);
			t5CheckMfail(SetUpDialog(objClass(BIObjP),BIObjP,"TransferDialogC",&extraStr,&extraObj, &BIDialogueObjP,mfail));
			if (nlsStrCmp(Flag_1,"1") == 0)
			{
				t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"CE Vault"));
			}
			else
			{
				t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"WIP Vault"));
			}
			t5CheckDstat(objSetAttribute(BIDialogueObjP,IncludeAttachedDIsAttr,"-"));
			t5CheckMfail(TransferAllObject(BIObjP,BIDialogueObjP,&failedSO,mfail));
		}
		else
		{
			if (nlsStrCmp(Flag_1,"1") == 0)
			{			t5CheckMfail(CheckInItem(BIObjP,"CE Vault", mfail));		}
			else
			{			t5CheckMfail(CheckInItem(BIObjP,"WIP Vault", mfail));		}
			}
	}
	
	t5CheckDstat(objGetAttribute(BIObjP,SequenceAttr,&CurSeqDupS));
	CurSeqS=nlsStrDup(CurSeqDupS);

	while(atoi(LoadSequenceS)>atoi(CurSeqS))
	{
		newSeqCreFlag=1;
		t5CheckMfail(PerformFreeze(BIObjP,mfail));
		t5CheckDstat(ChangeStateItem(BIObjP,"LcsAPLWrkg",mfail));//Need to change for APL
		t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
		if(dstat=RefreshObject(BIObjP,mfail)) goto EXIT;

		t5CheckDstat(objGetAttribute(BIObjP,CheckedOutAttr,&CheckedOutDupS));
		if (!nlsIsStrNull(CheckedOutDupS))CheckedOutS=nlsStrDup(CheckedOutDupS);
		t5CheckDstat(objGetAttribute(BIObjP,OwnerNameAttr,&OwnerCheckDupS));
		if (!nlsIsStrNull(OwnerCheckDupS))OwnerCheckS=nlsStrDup(OwnerCheckDupS);
		
		printf("\n Inside Loading sequence LoadPartRevSeq(): Owner of the Part is = [%s] checkeout Attr = [%s] Load seq  = [%s]  current seq = [%s] \n  ",OwnerCheckS,CheckedOutS,LoadSequenceS,CurSeqS);fflush(stdout);
		//printf("\n LoadPartRevSeq(): SequenceS = [%s], CurSeqS = [%s] ",LoadSequenceS,CurSeqS);	fflush(stdout);
		t5CheckMfail(CheckOutItem(BIObjP,&SecBIObjP,mfail));
			t5CheckMfail(ExpandObject(PartDocClass,SecBIObjP,"DocumentsDescribingPart",&VerDelDocSO,&VerDelDocRelSO,mfail));
			for (vercnt=0;vercnt<setSize(VerDelDocSO);vercnt++)
			{
				t5CheckMfail(DeleteRelation2(low_set_get(VerDelDocRelSO,vercnt),mfail));
			}

		if (atoi(LoadSequenceS) > atoi(CurSeqS)+1)  //NOTCHECKIN
		{
			if (nlsStrCmp(Flag_1,"1") == 0)
			{			t5CheckMfail(CheckInItem(SecBIObjP,"CE Vault", mfail));		}
			else
			{			t5CheckMfail(CheckInItem(SecBIObjP,"WIP Vault", mfail));		}
		} //NOTCHECKIN
		
		if (nlsStrCmp(RevCheckInd,"NA")==0)
		{
			printf("\n LoadPartRevSeq(): I am Deleting the Checkout/checkin TC Ptrts and updating the description with Versioned at last");	fflush(stdout);
			//objDump(SecBIObjP);
			t5CheckMfail(ExpandObject(PartDocClass,SecBIObjP,"DocumentsDescribingPart",&VerDelDocSO,&VerDelDocRelSO,mfail));
			printf("\n LoadPartRevSeq(): Number of Docs to ver TC [%d]",setSize(VerDelDocSO));fflush(stdout);
			for (vercnt=0;vercnt<setSize(VerDelDocSO);vercnt++)
			{
				//t5CheckMfail(DeleteRelation2(low_set_get(VerDelDocRelSO,vercnt),mfail));
			}
			t5CheckDstat(objGetAttribute(SecBIObjP,NomenclatureAttr,&VerTcDescDup));
			VerTcDesc=nlsStrDup(VerTcDescDup);
			printf("\n LoadPartRevSeq(): Before nlsStrStr VerTcDesc = [%s] ",VerTcDesc);fflush(stdout);
			/*if (nlsStrStr(VerTcDesc,"-Versioned")==NULL)
			{
					printf("There is no \"-Versioned \" in the description so I am adding ");fflush(stdout);
				if (!nlsIsStrNull(VerTcDesc))
				
				{
					printf("inside Concat ");fflush(stdout);

					VerTcDesc=nlsStrCat(VerTcDesc,"-Versioned");
				}
			}*/
			printf("\n LoadPartRevSeq(): VerTcDesc = [%s]",VerTcDesc);fflush(stdout);
			t5CheckDstat(objSetAttribute(SecBIObjP,NomenclatureAttr,VerTcDesc));
		}

		BIObjP = SecBIObjP ;
		
		if(dstat=RefreshObject(BIObjP,mfail)) goto EXIT;
        t5CheckDstat(objSetAttribute(BIObjP,t5DocRemarksAttr,moddesc));
		t5CheckDstat(objSetAttribute(BIObjP,t5VerCreatorAttr,VerCreator));
		t5CheckDstat(objSetAttribute(BIObjP,CreatorAttr,mcreator));


		if(dstat=t0UpdateObject(&BIObjP,mfail)) goto CLEANUP;
//		t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
		if(dstat=RefreshObject(BIObjP,mfail)) goto EXIT;

		t5CheckDstat(objGetAttribute(BIObjP,SequenceAttr,&CurSeqDupS));
		CurSeqS=nlsStrDup(CurSeqDupS);

		t5CheckDstat(objGetAttribute(BIObjP,CheckedOutAttr,&CheckedOutDupS));
		if (!nlsIsStrNull(CheckedOutDupS))CheckedOutS=nlsStrDup(CheckedOutDupS);
		t5CheckDstat(objGetAttribute(BIObjP,OwnerNameAttr,&OwnerCheckDupS));
		if (!nlsIsStrNull(OwnerCheckDupS))OwnerCheckS=nlsStrDup(OwnerCheckDupS);
		
		printf("\n LoadPartRevSeq(): Owner of the Part is = [%s] checkeout Attr = [%s] Load seq  = [%s]  current seq = [%s] \n  ",OwnerCheckS,CheckedOutS,LoadSequenceS,CurSeqS);fflush(stdout);

	}
	/*if (newSeqCreFlag==1 || newRevCreFlag==1) //NOTCHECKIN
	{
		
		t5CheckMfail(PerformFreeze(BIObjP,mfail));

		if ((nlsStrCmp(LoadVaultStatusS,"Released")!=0) && (nlsStrCmp(CurSeqS,LoadSequenceS)==0))
		{
			printf("\n LoadPartRevSeq(): Transfer to WIP Vault ");
			t5CheckDstat(ChangeStateItem(BIObjP,"LcsReview",mfail));
			t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
		}
		else if ((nlsStrCmp(LoadVaultStatusS,"Released")==0) && (nlsStrCmp(CurSeqS,LoadSequenceS)==0))
		{
			printf("\n LoadPartRevSeq(): Transfer to Release Vault ");fflush(stdout);
			t5CheckMfail(SetUpDialog(objClass(BIObjP),BIObjP,"TransferDialogC",&extraStr,&extraObj, &BIDialogueObjP,mfail));
			t5CheckDstat(objSetAttribute(BIDialogueObjP,DestOwnerNameAttr,"Release Vault"));
			t5CheckDstat(objSetAttribute(BIDialogueObjP,IncludeAttachedDIsAttr,"-"));

			t5CheckMfail(TransferAllObject(BIObjP,BIDialogueObjP,&failedSO,mfail));

			t5CheckDstat(ChangeStateItem(BIObjP,"LcsErcRlzd",mfail));
			t5CheckMfail(t0UpdateObject(&BIObjP,mfail));
		}
		else
		{
			printf("\n LoadPartRevSeq(): The higher Seq of TC Part is available \n");fflush(stdout);

		}
	}*/ //NOTCHECKIN

	

	if (LHRHFlag==1)
	{
			printf("\n LoadPartRevSeq(): LH RH Relation Being Created");fflush(stdout);
			printf("\n LoadPartRevSeq(): setSize(BILeftMstrSO) = [%d] setSize(BIRightMstrSO) = [%d]",setSize(BILeftMstrSO),setSize(BIRightMstrSO));fflush(stdout);
			t5CheckDstat(objSetAttribute(BIMstrObjP,t5LeftRhAttr,"LH CATIA"));
				t5CheckMfail(t0UpdateObject(&BIMstrObjP,mfail));

			t5CheckMfail(CreateRelation2(t5LRRelnClass,NULL,BIMstrObjP,setGet(BIRightMstrSO,0),&NewLHRHObjRelP,mfail));
	
	}
	else if (LHRHFlag==2)
	{
		printf("\n LoadPartRevSeq(): LH RH Relation Being Created");fflush(stdout);
		t5CheckDstat(objSetAttribute(BIMstrObjP,t5LeftRhAttr,"RH CATIA"));
		t5CheckMfail(t0UpdateObject(&BIMstrObjP,mfail));
		t5CheckMfail(CreateRelation2(t5LRRelnClass,NULL,setGet(BILeftMstrSO,0),BIMstrObjP,&NewLHRHObjRelP,mfail));
	}

	CLEANUP:
		printf("\n LoadPartRevSeq(): In the CLEANUP of LoadPartRevSeq");fflush(stdout);
		if  (BIchkSO) t5FreeSetOfObjects(BIchkSO);
		if (BIObjrelchkSO) t5FreeSetOfObjects(BIObjrelchkSO);
		if (failedSO) t5FreeSetOfObjects(failedSO);
		if (extraObj) t5FreeSetOfObjects(extraObj);
		if (ChkPartSO) t5FreeSetOfObjects(ChkPartSO);
		if (AllDoctempSO) t5FreeSetOfObjects(AllDoctempSO);

		t5FreeSetOfStrings(extraStr);

    EXIT:
		return(BIObjP);

	//mcreator
}


ObjectPtr CheckAndCreatePart
(
	string CheckPartNameS,
	string CheckRevisionS,
	string CheckSequenceS,
	string CheckClassS,
	string CheckDesignGrpS,
	string CheckProjectS,
	string CheckDescriptionS,
	string CheckVaultStatusS,
	string CheckPartTypeS,
	string mcreator,
	string VerCreator,
	string moddesc,
	int CheckIn,
	integer* mfail
)
{
	char *mod_name="CheckAndCreatePart";

	ObjectPtr PartCreateDia = NULL ;
	ObjectPtr NewPartObjP = NULL ;
	ObjectPtr PartObjP = NULL ;

	string VerDescriptionS = NULL;
	string descS = NULL;
	string CrePartTypeS;
	string CrePartprojectCodeS;

	status dstat = OKAY;
	printf("\n\n Inside CheckAndCreatePart()");fflush(stdout);
	printf("\n CheckAndCreatePart(): CheckClassS = [%s]",CheckClassS);fflush(stdout);

//	descS = subString(CheckDescriptionS, 0, 38);
	nlsStrTruncate(CheckDescriptionS,39);

	t5CheckMfail(ConstructArgsForCreate(CheckClassS,&PartCreateDia,mfail));
	t5CheckDstat(objSetAttribute(PartCreateDia,PartNumberAttr,CheckPartNameS));
	t5CheckDstat(objSetAttribute(PartCreateDia,ProjectNameAttr,CheckProjectS));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5DesignGroupAttr,CheckDesignGrpS));
	t5CheckDstat(objSetAttribute(PartCreateDia,NomenclatureAttr,CheckDescriptionS));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5CMVRCertificationReqdAttr,"-"));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5DismantableAttr,"-"));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5ListRecSparesAttr,"-"));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5SpareIndAttr,"-"));
	t5CheckDstat(objSetAttribute(PartCreateDia,PartTypeAttr,CheckPartTypeS));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5MatlClassAttr,"ABS"));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5WeightAttr,"0.001"));//Need to Handle weight
	t5CheckDstat(objSetAttribute(PartCreateDia,t5DrawingIndAttr,"N"));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5MaterialAttr,"NA"));
	t5CheckDstat(objSetAttribute(PartCreateDia,t5DsgnOwnAttr,"PROPUN"));
	t5CheckDstat(objSetAttribute(PartCreateDia, OrganizationIDAttr,"APLPUNE")) ;
	t5CheckDstat(objSetAttribute(PartCreateDia, OrgParticipantNameAttr,"[APLPUNE] APLPUNE"));
	
	//t5CheckDstat(objSetAttribute(PartCreateDia,CategoryNameAttr,"TMETC"));///// aDDED ON 30-Sep-2016 by VAP and sks
	//Added by Hemal
	//if (nlsStrCmp(CheckPartTypeS,"D")==0)
	{
		t5CheckDstat(objSetAttribute(PartCreateDia,"t5tmkoappli","NA"));
	}
	//Need to change for APL added organization name in code

//Added to give keyword to parts other than module with NR revision
	if ((nlsStrCmp(CheckPartTypeS,"M")!=0) && (nlsStrCmp(CheckRevisionS,"NR")==0))
	{
	  printf("\n Providing keyword for parts other than Module for NR Revision");fflush(stdout);
	  t5CheckDstat(objSetAttribute(PartCreateDia,CategoryNameAttr,"UAAPL"));
	}

	
    if (nlsStrCmp(CheckPartTypeS,"M")==0)
	{
	  printf("\n Updating keyword for module");fflush(stdout);
	  t5CheckDstat(objSetAttribute(PartCreateDia,CategoryNameAttr,CheckDescriptionS));
	}
	//t5CheckDstat(objSetAttribute(PartCreateDia,t5DsgnOwnAttr,"TELPUN"));
	//t5CheckDstat(objSetAttribute(PartCreateDia,CategoryNameAttr,"TRILLIX"));///// aDDED ON 30-Sep-2016 by VAP and sks 
	
	if (nlsStrCmp(CheckProjectS,"1111"))
	{
		printf("\n Inside category TMETC");fflush(stdout);
		t5CheckDstat(objSetAttribute(PartCreateDia,t5SpareCriteriaAttr,"non BOM – Dummy Part"));///// aDDED ON 30-Sep-2016 by VAP and sks 
	}
	else
	{
		printf("\n Inside category STD");fflush(stdout);
		t5CheckDstat(objSetAttribute(PartCreateDia,t5SpareCriteriaAttr,"STD"));///// aDDED ON 30-Sep-2016 by VAP and sks 
	}
	t5CheckDstat(objSetAttribute(PartCreateDia,t5DsgnDeptAttr,"Gunsan-Design"));
	
//	printf("\nMohan chi help ghetali ahe");fflush(stdout);
//	objDump(PartCreateDia);
//	printf("\nMohan chi help ghetali and got output");fflush(stdout);
	
	printf("\n CheckAndCreatePart(): Before CreateObject\n");fflush(stdout);
	t5CheckMfail(CreateObject(CheckClassS,PartCreateDia,&NewPartObjP,mfail));
	
	printf("\n CheckAndCreatePart(): After CreateObject");fflush(stdout);
	PartObjP=NewPartObjP;

	if (CheckIn==1)
	{
		printf("\n Check in...!!!");fflush(stdout);
		if (nlsStrCmp(Flag_1,"1") == 0)
		{			t5CheckMfail(CheckInItem(PartObjP,"CE Vault", mfail));		}
		else
		{			t5CheckMfail(CheckInItem(PartObjP,"WIP Vault", mfail));		}

		t5CheckMfail(PerformFreeze(PartObjP,mfail));
		printf("\n Stamp LCs");fflush(stdout);
		t5CheckDstat(ChangeStateItem(PartObjP,"LcsAPLWrkg",mfail));//Need to change for APL
		if(dstat = t5CheckInCstAndMstrObject(PartObjP,mfail))
		{
			printf("\n Error in transfering the Part master and cost. Please check");fflush(stdout);
		}
			
	}
	else if (CheckIn==2)
	{
		t5CheckMfail(CheckInItem(PartObjP,"Release Vault",mfail));
		t5CheckMfail(PerformFreeze(PartObjP,mfail));
		t5CheckDstat(ChangeStateItem(PartObjP,"LcsAplRlzd",mfail));//Need to change for APL
		if(dstat = t5CheckInCstAndMstrObject(PartObjP,mfail))
		{
			printf("\n Error in transfering the Part master and cost. Please check");fflush(stdout);
		}
	}

	VerDescriptionS=nlsStrAlloc(nlsStrLen(CheckDescriptionS)+nlsStrLen("-Versioned")+1);
	nlsStrCpy(VerDescriptionS,CheckDescriptionS);
	//nlsStrCat(VerDescriptionS,"-Versioned");

	t5CheckDstat(objSetAttribute(PartObjP,NomenclatureAttr,VerDescriptionS));
//	t5CheckDstat(objSetAttribute(PartObjP,t5VerCreatorAttr,"Loader-UK"));
	printf("\nSetting creator and version creator of New TC part");fflush(stdout);
	t5CheckDstat(objSetAttribute(PartObjP,StartingViewBitPosAttr,"1"));
	t5CheckDstat(objSetAttribute(PartObjP,t5VerCreatorAttr,VerCreator));
	t5CheckDstat(objSetAttribute(PartObjP,t5DocRemarksAttr,moddesc));
	t5CheckDstat(objSetAttribute(PartObjP,CreatorAttr,mcreator));
	t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
	t5CheckDstat(objGetAttribute(PartObjP,PartTypeAttr,&CrePartTypeS));



	if (nlsStrCmp(CrePartTypeS,"M")==0)

	{
		printf(" \n Setting the attributes for TC part having part type module");fflush(stdout);
		t5CheckDstat(objSetAttribute(PartObjP,t5DsgnDeptAttr,"Pune-Design"));
		t5CheckDstat(objSetAttribute(PartObjP,t5CMVRCertificationReqdAttr,"-"));
		t5CheckDstat(objSetAttribute(PartObjP,t5HomologationReqdAttr,"N"));
		t5CheckDstat(objSetAttribute(PartObjP,t5SpareIndAttr,"-"));
		t5CheckDstat(objSetAttribute(PartObjP,t5SpareCriteriaAttr,"PHN:Phantom Assembly-like Kits,Sets,TPL etc"));
		t5CheckDstat(objSetAttribute(PartObjP,t5RefPartNumberAttr,"N"));
		t5CheckDstat(objSetAttribute(PartObjP,t5ColourIndAttr,"N"));
		t5CheckDstat(objSetAttribute(PartObjP,t5PrtCatCodeAttr,""));
		t5CheckDstat(objSetAttribute(PartObjP,t5DrawingIndAttr,"N"));
		t5CheckDstat(objSetAttribute(PartObjP,t5DrawingNoAttr,"N"));
		t5CheckDstat(objSetAttribute(PartObjP,t5MatlClassAttr,""));
		t5CheckDstat(objSetAttribute(PartObjP,t5MaterialAttr,""));
		t5CheckDstat(objSetAttribute(PartObjP,t5MThicknessAttr,""));
		t5CheckDstat(objSetAttribute(PartObjP,t5EnvelopeDimensionsAttr,"XXX"));
		t5CheckDstat(objSetAttribute(PartObjP,t5SamplesToApprAttr,"N"));
		//t5CheckMfail(t0UpdateObject(&PartObjP,mfail))
		//Need to change for APL add organization name for APL
			t5CheckDstat(objGetAttribute(PartObjP,ProjectNameAttr,&CrePartprojectCodeS));
		if ( nlsStrCmp(CrePartprojectCodeS,"5442")==0 || nlsStrCmp(CrePartprojectCodeS,"5447")==0)
		{
			t5CheckDstat(objSetAttribute(PartObjP,t5PartStatusAttr,"DR3"));
		}
        else if ( nlsStrCmp(CrePartprojectCodeS,"2856")==0 || nlsStrCmp(CrePartprojectCodeS,"2853")==0 || nlsStrCmp(CrePartprojectCodeS,"5715")==0 || nlsStrCmp(CrePartprojectCodeS,"5724")==0 || nlsStrCmp(CrePartprojectCodeS,"5825")==0)
        {
			t5CheckDstat(objSetAttribute(PartObjP,t5PartStatusAttr,"AR3"));
        }
			
	   else
		{
		   printf("\n This Project code is not configured for module");fflush(stdout);
		   goto CLEANUP;
		}
	}
	t5CheckMfail(t0UpdateObject(&PartObjP,mfail))


	//t5CheckDstat(objSetAttribute(PartObjP,LifeCycleStateAttr,"LcsErcRlzd"));

	CLEANUP:
		printf("\n CheckAndCreatePart(): IN CLEANUP ");fflush(stdout);

	EXIT:
		return(PartObjP);
}

ObjectPtr CheckAndCreateDoc
(
	string CheckDocNameS,
	string CheckClassS,
	string CheckDesignGrpS,
	string CheckProjectS,
	string CheckDescriptionS,
	string CheckBomIndicatorS,
	string CheckDocumentTypeS,
	integer* mfail
)
{
	char *mod_name="CheckAndCreateDoc";

	ObjectPtr DocCreateDia = NULL ;
	ObjectPtr NewObjP = NULL ;
	ObjectPtr CreateDocObjP = NULL ;

	status dstat = OKAY;
	
	printf("\n\n Inside CheckAndCreateDoc(): ");fflush(stdout);

	
	t5CheckMfail(ConstructArgsForCreate(CheckClassS,&DocCreateDia,mfail));
	printf("\n Inside CheckAndCreateDoc(): CheckClassS = [%s] ",CheckClassS);fflush(stdout);
	t5CheckDstat(objSetAttribute(DocCreateDia,DocumentNameAttr,CheckDocNameS));

	if (nlsStrCmp(CheckClassS,"t5DrwDoc")==0 || nlsStrCmp(CheckClassS,"DrwDoc")==0)  //Added to create drawing document
    {
         if(dstat = objSetTableAttribute(DocCreateDia,t5TSDetailsAttr,0,t5RefStdNoAttr,"11103,TS")); fflush(stdout); //goto EXIT;
    }

	if (nlsStrCmp(CheckClassS,"t5MulCad")==0)
	{
		printf("\n Inside CheckAndCreateDoc(): Setting Value for t5DesignGroup1 as we are cerating t5MulCad Doument ");fflush(stdout);
		t5CheckDstat(objSetAttribute(DocCreateDia,t5DesignGroup1Attr,CheckDesignGrpS));
		t5CheckDstat(objSetAttribute(DocCreateDia,t5DocumentType2Attr,CheckDocumentTypeS));
		printf("\n Inside CheckAndCreateDoc(): Setting Value for t5Real as we are cerating t5MulCad Doument ");fflush(stdout);
		t5CheckDstat(objSetAttribute(DocCreateDia,"t5RealAttr","1"));  
	}
	else
	{
		t5CheckDstat(objSetAttribute(DocCreateDia,t5DesignGroupAttr,CheckDesignGrpS));
		t5CheckDstat(objSetAttribute(DocCreateDia,t5DesDocRemarksAttr,CheckDescriptionS));
		t5CheckDstat(objSetAttribute(DocCreateDia,t5BOMAppearAttr,CheckBomIndicatorS));
		t5CheckDstat(objSetAttribute(DocCreateDia,t5DocumentType1Attr,CheckDocumentTypeS));
	}
	
	t5CheckDstat(objSetAttribute(DocCreateDia,ProjectNameAttr,CheckProjectS));
	t5CheckDstat(objSetAttribute(DocCreateDia,DocumentDescriptionAttr,CheckDescriptionS));
	
	t5CheckDstat(objSetAttribute(DocCreateDia, OrganizationIDAttr,"APLPUNE")) ;
	t5CheckDstat(objSetAttribute(DocCreateDia, OrgParticipantNameAttr,"[APLPUNE] APLPUNE"));

	t5CheckMfail(CreateObject(CheckClassS,DocCreateDia,&NewObjP,mfail));
	CreateDocObjP=NewObjP;
//	t5CheckMfail(CheckInItem(CreateDocObjP,"WIP Vault", mfail));
//	if (nlsStrCmp(Flag_1,"1") == 0)
//	{		t5CheckMfail(CheckInItem(CreateDocObjP,"CE Vault", mfail));	}
//	else
//	{		t5CheckMfail(CheckInItem(CreateDocObjP,"WIP Vault", mfail)); }
//
//	t5CheckDstat(objSetAttribute(CreateDocObjP,LifeCycleStateAttr,"LcsWorking"));
//	t5CheckMfail(t0UpdateObject(&CreateDocObjP,mfail));
//	t5CheckMfail(PerformFreeze(CreateDocObjP,mfail));

	CLEANUP:
		printf("\n Inside CheckAndCreateDoc(): CLEANUP");fflush(stdout);
	EXIT:
		return(CreateDocObjP);
}


int main (int argc, char *argv[])
{

	int LoopCnt1						=	0;  
	int LoopCnt3						=	0;
	int LoadCheck						=	0;
	int stat							=	0;
	int k								=	0;
	int ii								=	0;
	int CreFlag							=	0;
	int RevChkId						=	0;
	int RelChkFlag						=	0;
	int delCnt							=	0;
	int mulcnt							=	0;

	char *result						=	NULL;

	FILE	*fp							=	NULL;
	FILE	*fpDn							=	NULL;

	boolean ans							=	FALSE;

	SqlPtr getWorkLoc					=	NULL;
	SqlPtr getDI						=	NULL;
	SqlPtr DocRelSqlPtr					=	NULL;
	SqlPtr DocSqlPtr					=	NULL;
	SqlPtr PartsqlPtr					=	NULL;
	SqlPtr VisSqlPtr					=	NULL;
	SqlPtr SuperSedSqlPtr				=	NULL;
	SqlPtr usrsql3						=	NULL;
	SqlPtr RefDrwsqlPtr					=	NULL;
	SqlPtr RefPrtsqlPtr					=	NULL;
	SqlPtr ViewSqlPtr					=	NULL;
	SqlPtr TCPartsqlPtr					=	NULL;

	SetOfObjects WorkLocSO				=	NULL;
	SetOfObjects CatiaFileSO			=	NULL;
	SetOfObjects CatiaDIsChkSO			=	NULL;
	SetOfObjects DIObjRelSO				=	NULL;
	SetOfObjects DataPdmItemSO			=	NULL;
	SetOfObjects extraObj				=	NULL;
	SetOfObjects LatestDocSO			=	NULL;
	SetOfObjects LatestPartSO			=	NULL;
	SetOfObjects DocPdmRelSO			=	NULL;
	SetOfObjects DocPdmItemSO			=	NULL;
	SetOfObjects PartMstrSO				=	NULL;
	SetOfObjects PartMstrRelSO			=	NULL;
	SetOfObjects VisObjSO				=	NULL;
	SetOfObjects VisRelSO				=	NULL;
	SetOfObjects BIObjNxtRevSO			=	NULL;
	SetOfObjects BIObjNxtRevRelSO		=	NULL;
	SetOfObjects BIObjNxtSeqSO			=	NULL;
	SetOfObjects BIObjNxtRelRelSO		=	NULL;
	SetOfObjects DocDataPdmItemSO		=	NULL;
	SetOfObjects DocDIObjRelSO			=	NULL;
	SetOfObjects AttachByDocItemRelSO	=	NULL;
	SetOfObjects AttachByDocItemSO		=	NULL;
	SetOfObjects LoadDocRelSqlPtrSO		=	NULL;
	SetOfObjects PrvSupersedObjSO		=	NULL;
	SetOfObjects PrvSupersedRelObjSO	=	NULL;
	SetOfObjects AllDoctempSO			=	NULL;
	SetOfObjects ChkPartSO				=	NULL;
	SetOfObjects ChkPartRelSO			=	NULL;
	SetOfObjects SupersedObjSO			=	NULL;
	SetOfObjects RefDrwDocSO			=	NULL;
	SetOfObjects ChkRefDrwSO			=	NULL;
	SetOfObjects RefDrwPartSO			=	NULL;
	SetOfObjects DelDocSO				=	NULL;
	SetOfObjects DelDocRelSO			=	NULL;
	SetOfObjects soview					=	NULL;
	SetOfObjects MulcadPrtSO			=	NULL;
	SetOfObjects MulcadPrtRelSO			=	NULL;
	SetOfObjects TCPartSO				=	NULL;
	SetOfObjects failedSO			=	NULL;
	SetOfObjects ExistingObjRelP			=	NULL;

	ObjectPtr view						=	NULL;
	ObjectPtr PartMstrRelObjP			=	NULL;
	ObjectPtr WorklocObjP				=	NULL;
	ObjectPtr CatiaDIObjP				=	NULL;
	ObjectPtr CatiaFileP				=	NULL;
	ObjectPtr DocObjP					=	NULL;
	ObjectPtr TranDialogueObjP			=	NULL;
	ObjectPtr NewObjRelP				=	NULL;
	ObjectPtr PartObjP					=	NULL;
	ObjectPtr AttachByDocItemObjP		=	NULL;
	ObjectPtr SupersedObjP				=	NULL;
	ObjectPtr RelItemObjP				=	NULL;
	ObjectPtr DocRelDialogObjP			=	NULL;
	ObjectPtr DocRelObjP				=	NULL;
	ObjectPtr NewDocRelObjP				=	NULL;
    ObjectPtr RefDrwPartObjP			=	NULL;
    ObjectPtr RefDrwDocObjP				=	NULL;
    ObjectPtr DrwDialogObjP				=	NULL;
    ObjectPtr NewRefRelObjP				=	NULL;
    ObjectPtr genContextObjP			=	NULL;
    ObjectPtr contextObjP				=	NULL;
    ObjectPtr DelDocObjP				=	NULL;
    ObjectPtr MulCadPrtObjP				=	NULL;
	ObjectPtr allDocsObj1 = NULL;
	SetOfObjects allDocsSO1 = NULL;
	ObjectPtr convCad1Obj = NULL;
	ObjectPtr allTC1 = NULL;
	SetOfObjects convCad2SO = NULL;
	SetOfObjects docRelObjs = NULL;
	int j=0,dcCnt=0;
	
	string docClassNameS						=	NULL;
	string DIRelPth						=	NULL;
	string DIRelPathDup						=	NULL;
	string docClassNameDupS						=	NULL;
	string LoginS						=	NULL;
	string PasswordS					=	NULL;
	string WorkLocS						=	NULL;
	string CatiaFileNameS				=	NULL;
	string PartNameS					=	NULL;
	string RevisionS					=	NULL;
	string SequenceS					=	NULL;
	string VaultStatusS					=	NULL;
	string ProjectCodeS					=	NULL;
	string DesignGrpS					=	NULL;
	string DescriptionS					=	NULL;
	string DISequenceS					=	NULL;
	string SupersededFileSeqS			=	NULL;
	string WorkRelPathS					=	NULL;
	string BomIndicatorS				=	NULL;
    string UnRegFileNameS				=	NULL;
    string UnRegFileNameDupS			=	NULL;
    string DIOwnerNameS					=	NULL;
    string CurRevS						=	NULL;
    string CurRevDupS					=	NULL;
    string DIChkWorkRelPathS			=	NULL;
    string DIChkWorkRelPathDupS			=	NULL;
    string CurSeqS						=	NULL;
    string CurSeqDupS					=	NULL;
    string DIChkSeqS					=	NULL;
    string DIChkSeqDupS					=	NULL;
    string DocClassDupS					=	NULL;
    string DocClassS					=	NULL;
    string SupersedesDISeqS				=	NULL;
    string SupersedesDISeqDupS			=	NULL;
    string SupersedesDIClassS			=	NULL;
    string SupersedesDIClassDupS		=	NULL;
    string DIClassDupS					=	NULL;
    string DIClassS						=	NULL;
    string DocObidS						=	NULL;
    string DocObidDupS					=	NULL;
    string RefDrwName					=	NULL;
    string RefDrwRev					=	NULL;
    string RefDrwSeq					=	NULL;
    string DocRevisionS					=	NULL;
    string DocSequenceS					=	NULL;
    string DelDocClassDupS				=	NULL;
    string DelDocClassS					=	NULL;
    string DelDocObidDupS				=	NULL;
    string DelDocObidS					=	NULL;
    string SupFileName					=	NULL;
    string DIDiffOwnerNameS				=	NULL;
    string BoundMinXS					=	NULL;
    string BoundMinYS					=	NULL;
    string BoundMinZS					=	NULL;
    string BoundMaxXS					=	NULL;
    string BoundMaxYS					=	NULL;
    string BoundMaxZS					=	NULL;
    string MulCadPrtSupS				=	NULL;
    string ScopeS						=	NULL;
    string DiDescS						=	NULL;
    string RevChkIdS					=	NULL;
    string ModDescriptionS				=	NULL;
    string tempS						=	NULL;
    string moddtemp						=	NULL;
    string DocumentClassS				=	NULL;
    string DocumentNameS				=	NULL;
	string docobjst						=	NULL;
	string KeywordDescriptionS			=	NULL;
	string InitialCS					=	NULL;
	string CheckedOutDupS				=	NULL;
	string CheckedOutS					=	NULL;
	string test							=	NULL;
	string temp1S						=	NULL;
	string VisFile						=	NULL;
	string DiOwnName					=	NULL;
	string RepPath					=	NULL;
	string PrtType					=	NULL;
	string mcreator					=	NULL;
	string VerCreator					=	NULL;
	string moddesc					=	NULL;
	string fpDnStrFile					=	NULL;
	string fpStrFile					=	NULL;
	string SupFileNamefpEr					=	NULL;
	string SupFileNamefpDn					=	NULL;
	string DRstatus					=	NULL;
 
	SetOfStrings    dbScp				=	NULL;
	SetOfStrings extraStr				=	NULL;
	int CheckInSts =2,DrwInd=0;

	t5MethodInitWMD("ExCatiaV5Reg");

	/* enable multibyte features of Metaphase */
	t5CheckDstat(clInitMB2 (argc, &argv, NULL));
	t5CheckDstat(clTestNetwork ());
	t5CheckDstat(clInitialize2 (FALSE));


	LoginS					=	nlsStrAlloc(nlsStrLen(argv[1])+1);
	PasswordS				=	nlsStrAlloc(nlsStrLen(argv[2])+1);
	WorkLocS				=	nlsStrAlloc(nlsStrLen(argv[3])+1);
	CatiaFileNameS			=	nlsStrAlloc(nlsStrLen(argv[4])+1);
	PartNameS				=	nlsStrAlloc(nlsStrLen(argv[5])+1);
	RevisionS				=	nlsStrAlloc(nlsStrLen(argv[6])+1);
	SequenceS				=	nlsStrAlloc(nlsStrLen(argv[7])+1);
	VaultStatusS			=	nlsStrAlloc(nlsStrLen(argv[8])+1);
	ProjectCodeS			=	nlsStrAlloc(nlsStrLen(argv[9])+1);
	DesignGrpS				=	nlsStrAlloc(nlsStrLen(argv[10])+1);
	DescriptionS			=	nlsStrAlloc(nlsStrLen(argv[11])+1);
	PrtType					=	nlsStrAlloc(nlsStrLen(argv[12])+1);
	SupersededFileSeqS		=	nlsStrAlloc(nlsStrLen(argv[13])+1);
	WorkRelPathS			=	nlsStrAlloc(nlsStrLen(argv[14])+1);
	BomIndicatorS			=	nlsStrAlloc(nlsStrLen(argv[15])+1);
	RefDrwName				=	nlsStrAlloc(nlsStrLen(argv[16])+1);
	RefDrwRev				=	nlsStrAlloc(nlsStrLen(argv[17])+1);
	RefDrwSeq				=	nlsStrAlloc(nlsStrLen(argv[18])+1);
	DocRevisionS			=	nlsStrAlloc(nlsStrLen(argv[19])+1);
	DocSequenceS			=	nlsStrAlloc(nlsStrLen(argv[20])+1);
	SupFileName				=	nlsStrAlloc(nlsStrLen(argv[21])+1);
	SupFileNamefpEr				=	nlsStrAlloc(nlsStrLen(argv[21])+1);
	SupFileNamefpDn				=	nlsStrAlloc(nlsStrLen(argv[21])+1);
	DocumentClassS			=	nlsStrAlloc(nlsStrLen(argv[31])+1);
	DocumentNameS			=	nlsStrAlloc(nlsStrLen(argv[32])+1);
	KeywordDescriptionS		=	nlsStrAlloc(nlsStrLen(argv[33])+1);
	InitialCS				=	nlsStrAlloc(nlsStrLen(argv[34])+1);
	VisFile					=	nlsStrAlloc(nlsStrLen(argv[35])+1);
	RepPath					=	nlsStrAlloc(nlsStrLen(argv[36])+1);
	Flag_1					=	nlsStrAlloc(5);
	mcreator				=	nlsStrAlloc(nlsStrLen(argv[37])+1);
	VerCreator				=	nlsStrAlloc(nlsStrLen(argv[38])+1);
	moddesc				   =	nlsStrAlloc(nlsStrLen(argv[39])+1);
	DRstatus				  =	nlsStrAlloc(nlsStrLen(argv[40])+1);
	//FileVisPath					=	nlsStrAlloc(200);
	

	LoginS					=	argv[1];
	PasswordS				=	argv[2];
	WorkLocS				=	argv[3];
	CatiaFileNameS			=	argv[4];
	PartNameS				=	argv[5];
	RevisionS				=	argv[6];
	SequenceS				=	argv[7];
	VaultStatusS			=	argv[8];
	ProjectCodeS			=	argv[9];
	DesignGrpS				=	argv[10];
	DescriptionS			=	argv[11];
	PrtType					=	argv[12];
	SupersededFileSeqS		=	argv[13];
	WorkRelPathS			=	argv[14];
	BomIndicatorS			=	argv[15];
	RefDrwName				=	argv[16];
	RefDrwRev				=	argv[17];
	RefDrwSeq				=	argv[18];
	DocRevisionS			=	argv[19];
	DocSequenceS			=	argv[20];
	SupFileName				=	argv[27];
	Flag_1					=	argv[30];
	DocumentClassS			=	argv[31];
	DocumentNameS			=	argv[32];
	KeywordDescriptionS		=	argv[33];
	InitialCS				=	argv[34];
	VisFile					=	argv[35];
	RepPath					=	argv[36];
	mcreator				=	argv[37];
	VerCreator				=	argv[38];
	moddesc					=	argv[39];
	DRstatus				=	argv[40];

		
	if (nlsStrCmp(argv[23],"NULL")!=0)
	{
			//printf("\n Bounding Value not NULL ");

		BoundMinXS			=	nlsStrAlloc(nlsStrLen(argv[21])+1);
		BoundMinYS			=	nlsStrAlloc(nlsStrLen(argv[22])+1);
		BoundMinZS			=	nlsStrAlloc(nlsStrLen(argv[23])+1);
		BoundMaxXS			=	nlsStrAlloc(nlsStrLen(argv[24])+1);
		BoundMaxYS			=	nlsStrAlloc(nlsStrLen(argv[25])+1);
		BoundMaxZS			=	nlsStrAlloc(nlsStrLen(argv[26])+1);

		BoundMinXS			=	argv[21];
		BoundMinYS			=	argv[22];
		BoundMinZS			=	argv[23];
		BoundMaxXS			=	argv[24];
		BoundMaxYS			=	argv[25];
		BoundMaxZS			=	argv[26];
	}

	t5CheckDstat(clLogin2 (LoginS,PasswordS,&stat));
	if (stat!=OKAY)
	{
		printf("\n Invalid User Name or PasswordS : %s, %s \n", LoginS, PasswordS);fflush(stdout);
		goto EXIT;
	}
	tempS = nlsStrAlloc(200);
	moddtemp= nlsStrAlloc(200);
	fpDnStrFile= nlsStrAlloc(200);
	fpStrFile= nlsStrAlloc(200);
	nlsStrCpy(moddtemp,"");
	nlsStrCpy(moddtemp,"---TCUA APL Loaded---");
	nlsStrCat(moddtemp,moddesc);
	nlsStrCpy(tempS,"");
	temp1S = nlsStrAlloc(200);
	nlsStrCpy(temp1S,"");
	//nlsStrCpy(FileVisPath,"");

//	printf("\n%s",LoginS);
//	printf("\n%s",PasswordS);
//	printf("\n%s",WorkLocS);
//	printf("\n%s",CatiaFileNameS);
//	printf("\n%s",PartNameS);
//	printf("\n%s",RevisionS);
//	printf("\n%s",SequenceS);
//	printf("\n%s",VaultStatusS);
//	printf("\n%s",ProjectCodeS);
//	printf("\n%s",DesignGrpS);
//	printf("\n%s",DescriptionS);
//	printf("\n%s",PrtType);
//	printf("\n%s",SupersededFileSeqS);
//	printf("\n%s",WorkRelPathS);
//	printf("\n%s",BomIndicatorS);
//	printf("\n%s",RefDrwName);
//	printf("\n%s",RefDrwRev);
//	printf("\n%s",RefDrwSeq);
//	printf("\n%s",DocRevisionS);
//	printf("\n%s",DocSequenceS);
//	printf("\n%s",SupFileName);
//	printf("\n%s",Flag_1);
//	printf("\n%s",DocumentClassS);
//	printf("\n%s",DocumentNameS);
//	printf("\n%s",KeywordDescriptionS);
//	printf("\n%s",InitialCS);
//	printf("\n%s",VisFile);
//	printf("\n%s",RepPath);
//	printf("\n%s",mcreator);
//	printf("\n%s",VerCreator);
//	printf("\n%s",moddtemp);

	printf("\n LoginS \t = [%s] \n PasswordS \t = [%s] \n WorkLocS \t = [%s] \n CatiaFileNameS  = [%s] \n PartNameS \t = [%s] \n RevisionS \t = [%s] \n SequenceS \t = [%s] \n VaultStatusS \t = [%s] \n ProjectCodeS \t = [%s] \n DesignGrpS \t = [%s] \n DescriptionS \t = [%s] \n PrtType \t = [%s] \n WorkRelPathS \t = [%s] \n BomIndicatorS \t = [%s] \n SupersededFileSeqS = [%s]", LoginS, PasswordS, WorkLocS, CatiaFileNameS, PartNameS, RevisionS, SequenceS, VaultStatusS, ProjectCodeS, DesignGrpS, DescriptionS , PrtType, WorkRelPathS, BomIndicatorS, SupersededFileSeqS); fflush(stdout);
	printf("\n RefDrwName \t = [%s] \n RefDrwRev \t = [%s] \n RefDrwSeq \t = [%s] \n DocRevisionS \t = [%s] \n DocSequenceS \t = [%s]",RefDrwName,RefDrwRev,RefDrwSeq,DocRevisionS,DocSequenceS); fflush(stdout);
	printf("\n SupFileName \t = [%s] \n DocumentClassS\t= [%s] \n DocumentNameS \t = [%s]",SupFileName,DocumentClassS,DocumentNameS); fflush(stdout);
	printf("\n KeywordDescriptionS \t = [%s] \n InitialCS \t= [%s]",KeywordDescriptionS,InitialCS); fflush(stdout);
	printf("\n\n BoundMinXS = %s \t BoundMinYS = %s \t BoundMinZS = %s \t BoundMaxXS = %s \t BoundMaxYS = %s \t BoundMaxZS = %s",BoundMinXS,BoundMinYS,BoundMinZS,BoundMaxXS,BoundMaxYS,BoundMaxZS);
	printf("\n RepPath \t = [%s] \n VisFile \t= [%s]",RepPath,VisFile); fflush(stdout);
	printf("\n Creator \t = [%s] \n Version creator \t= [%s], DR Status = [%s] ",mcreator,VerCreator,DRstatus); fflush(stdout);


	/*if(dstat = OpenStartupPrefs("StuPref",&mfail));
	if(mfail) goto CLEANUP;*/
	nlsStrCpy(SupFileNamefpEr,SupFileName);
	//nlsStrCpy(fpStrFile,"");
	nlsStrCpy(fpStrFile,SupFileNamefpEr);
	nlsStrCat(fpStrFile,"_Error.txt");
	
	nlsStrCpy(SupFileNamefpDn,SupFileName);
	//nlsStrCpy(fpDnStrFile,"");
	nlsStrCpy(fpDnStrFile,SupFileNamefpDn);
	nlsStrCat(fpDnStrFile,"_Completed.txt");

	printf("\n File names : %s, %s \n", fpStrFile, fpDnStrFile);fflush(stdout);
	fp=fopen(fpStrFile,"a+");
	fflush(fp);

	 if (fp!=NULL)
  {
    printf("\n File open \n");fflush(stdout);
    
  }
  else
	{
	  printf("\n File not open \n");fflush(stdout);
	}

	fpDn=fopen(fpDnStrFile,"a+");
	fflush(fpDn);
		 if (fpDn!=NULL)
  {
    printf("\n File fpDn open \n");fflush(stdout);
    
  }
  else
	{
	  printf("\n File  fpDn not open \n");fflush(stdout);
	}
	//fprintf(fp,"\n TC Part [%s,%s,%s] \n",PartNameS,RevisionS,SequenceS);fflush(stdout);
	if (argv[28])
	{
		RevChkIdS=nlsStrAlloc(nlsStrLen(argv[28])+1);

		RevChkIdS=argv[28];
		printf("\n RevChkIdS = [%s]",RevChkIdS);fflush(stdout);
		if (nlsStrCmp(RevChkIdS,"NA")==0)
		{
			printf("\n Previous Revision check by passed \n");fflush(stdout);
		}
		else
		{
			printf("\n Inside Previous Revision check \n ");fflush(stdout);

			RevChkId=SupersedesChk(fp,PartNameS,RevisionS, SequenceS, mfail);
			printf("\n RevChkId [%d] ",RevChkId); fflush(stdout);
			if (RevChkId !=1)
			{
				goto EXIT;
			}
		}
	}

	if(argv[29])
	{
		ScopeS=nlsStrAlloc(nlsStrLen(argv[29])+1);
		ScopeS=argv[29];


	t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScp,mfail));
	for (ii=0;ii<setSize(dbScp) ; ii++)
	{
		docobjst=low_set_get(dbScp,ii);
		printf("\n DB pref check before... :[%s] ",docobjst);fflush(stdout);
	}
		result = strtok( ScopeS, ":_:" );
		while(!nlsIsStrNull(result))
		{
			low_set_add_str_unique(dbScp, result);
			printf("\n [%s] \n", result); fflush(stdout);
			result = strtok( NULL, ":_:" );
		}
	
		t5CheckDstat(SetDatabaseScope(PdmSessnClass,dbScp,mfail));

		t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScp,mfail));

		for (ii=0;ii<low_set_size(dbScp) ; ii++)
		{
			docobjst=low_set_get(dbScp,ii);
			printf("\n DB pref check - after... :[%s] ",docobjst);fflush(stdout);
		}

	}

// 0. START : Checking if TC Part is already Present or Not. GOTO CLEANUP if TC Part Found.

	t5CheckDstat(oiSqlCreateSelect(&TCPartsqlPtr));
	t5CheckDstat(oiSqlWhereEQ(TCPartsqlPtr,PartNumberAttr,PartNameS));
	t5CheckDstat(oiSqlWhereAND(TCPartsqlPtr));
	t5CheckDstat(oiSqlWhereEQ(TCPartsqlPtr,RevisionAttr,RevisionS));
	t5CheckDstat(oiSqlWhereAND(TCPartsqlPtr));
	t5CheckDstat(oiSqlWhereEQ(TCPartsqlPtr,SequenceAttr,SequenceS));
	t5CheckMfail(QueryWhere(PartClass,TCPartsqlPtr,&TCPartSO,mfail));
	printf("\n TC Part %s rev %s seq %s  size : [%d] ",PartNameS,RevisionS,SequenceS,setSize(TCPartSO)); fflush(stdout);

	if (setSize(TCPartSO) > 0)
	{
		printf("\n \t\t\t TC Part %s rev %s seq %s  found size : [%d] ",PartNameS,RevisionS,SequenceS,setSize(TCPartSO)); fflush(stdout);
		if(dstat = ExpandObject2(PartDocClass, setGet(TCPartSO,0),"DocumentsDescribingPart",SC_SCOPE_OF_SESSION,&allDocsSO1,&docRelObjs,mfail)) goto CLEANUP;
		for(j=0; j<setSize(allDocsSO1); j++ ) 
		{			

			allDocsObj1 = NULL;
			convCad2SO = NULL;
			allDocsObj1 = low_set_get(allDocsSO1,j);
			//objDump(allDocsObj);
			t5CheckDstat(objGetAttribute(allDocsObj1,ClassAttr,&docClassNameS));
			if(docClassNameS) docClassNameDupS = nlsStrDup(docClassNameS);
			printf("\ndocClassNameDupS --%s  ",docClassNameDupS);fflush(stdout);
			t5CheckDstat(ExpandObject4(AttachClass,allDocsObj1, "DataItemsAttachedToBusItem", &convCad2SO, mfail));
			if(convCad2SO) 
			{	
				for(dcCnt=0;dcCnt<setSize(convCad2SO);dcCnt++)
				{
						convCad1Obj = low_set_get(convCad2SO,dcCnt);
						if (dstat = objGetAttribute(convCad1Obj,WorkingRelativePathAttr,&DIRelPathDup)) goto EXIT;
						if(!nlsIsStrNull(DIRelPathDup)) DIRelPth = nlsStrDup(DIRelPathDup);
						if (nlsStrStr(DIRelPth,CatiaFileNameS) )
						{
							printf("\n \t\t\t TC Part [%s,%s,%s] Already Found with DI [%s] in Teamcenter Hence Not Loading.... \n",PartNameS,RevisionS,SequenceS,CatiaFileNameS); fflush(stdout);
							///fprintf(fp,"\n TC Part [%s,%s,%s] Already Found with DI [%s] in Teamcenter Hence Not Loading.... \n",PartNameS,RevisionS,SequenceS,CatiaFileNameS);fflush(fp);
							goto CLEANUP;
						}
				}
			}
		}
		j=0;
	}

	string TcRev					=	NULL;
	string TcSeq					=	NULL;
	SetOfObjects TCPartSO_1					=	NULL;


	t5CheckDstat(oiSqlCreateSelect(&TCPartsqlPtr));
	t5CheckDstat(oiSqlWhereEQ(TCPartsqlPtr,PartNumberAttr,PartNameS));
	t5CheckDstat(oiSqlWhereAND(TCPartsqlPtr));
	t5CheckDstat(oiSqlWhereEQ(TCPartsqlPtr,SupersededAttr,"-"));
	t5CheckMfail(QueryWhere(PartClass,TCPartsqlPtr,&TCPartSO_1,mfail));
	int ByPassFlag=0;
	if (setSize(TCPartSO_1)>1)
	{
		fprintf(fp,"\n TC Part [%s]  has more than 1 superseded false .... \n",PartNameS);fflush(fp);
		goto CLEANUP;
	}
	else if (setSize(TCPartSO_1)==1)
	{
		allTC1 = low_set_get(TCPartSO_1,0);
		if (dstat = objGetAttribute(allTC1,RevisionAttr,&TcRev)) goto EXIT;
		if (dstat = objGetAttribute(allTC1,SequenceAttr,&TcSeq)) goto EXIT;

		printf("\n Diffence -- %d \n  ",nlsStrCmp(RevisionS,TcRev));fflush(stdout);

		if ((!nlsStrCmp(TcRev,"Z") &&  !nlsStrCmp(RevisionS,"a1")))
		{
			ByPassFlag=1;
		}
		if ((!nlsStrCmp(TcRev,"z1") &&  !nlsStrCmp(RevisionS,"a2")))
		{
			ByPassFlag=1;
		}
		if ((!nlsStrCmp(TcRev,"z2") &&  !nlsStrCmp(RevisionS,"a3")))
		{
			ByPassFlag=1;
		}
		if ((!nlsStrCmp(TcRev,"NR") &&  !nlsStrCmp(RevisionS,"A")))
		{
			ByPassFlag=1;
		}

		if (ByPassFlag==0)
		{
			if (nlsStrCmp(RevisionS,TcRev) > 1)
			{
				fprintf(fp,"\n To be loaded TC Part [%s,%s,%s] has  a revsion missing as per latest revision [%s]  \n",PartNameS,RevisionS,SequenceS,TcRev);fflush(fp);
				printf("\n To be loaded TC Part [%s,%s,%s] has  a revsion missing as per latest revision [%s]  \n",PartNameS,RevisionS,SequenceS,TcRev);fflush(stdout);
			}
		}

	}
	else if (setSize(TCPartSO_1)==0)
	{
		if (nlsStrCmp(RevisionS,"NR"))
		{
			fprintf(fp,"\n To be loaded TC Part [%s,%s,%s] has  a revsion missing  \n",PartNameS,RevisionS,SequenceS);fflush(fp);
		}
	}

	fprintf(fpDn,"CADLOAD~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~\n",CatiaFileNameS, PartNameS, RevisionS, SequenceS, VaultStatusS, ProjectCodeS, DesignGrpS, DescriptionS ,PrtType, BomIndicatorS); fflush(stdout);fflush(fpDn);
	
	if (!nlsStrCmp(RevisionS,"NR")  && !nlsStrCmp(SequenceS,"1"))
	{
		CheckInSts=0;
	}
	else if (!nlsStrCmp(RevisionS,"NR"))
	{
		CheckInSts=1;
	}
	else
	{
		CheckInSts=2;
	}

// 0. END : Checking if TC Part is already Present or Not. GOTO CLEANUP if TC Part Found.

// 1. START : Checking if Document is Present or Not. If LatestDocSO > 1 then quitting or if LatestDocSO==1 continue to further steps
	
	// ----------------------------------------------------------------------------------------------------------------------------------------------------
	if(nlsStrCmp(CatiaFileNameS,"NULL")!=0)
	{
		printf("\n CATIA File Name no is : [%s] ",CatiaFileNameS); fflush(stdout);
		t5CheckDstat(oiSqlCreateSelect(&DocSqlPtr));
		if (nlsStrCmp(DocumentClassS,"t5MulCad")==0 || nlsStrCmp(DocumentClassS,"t5MulPrd")==0)
		{				
			t5CheckDstat(oiSqlWhereEQ(DocSqlPtr,DocumentNameAttr,DocumentNameS));
			t5CheckDstat(oiSqlWhereAND(DocSqlPtr));
			t5CheckDstat(oiSqlWhereEQ(DocSqlPtr,SupersededAttr,"-"));
		}
		else
		{
			t5CheckDstat(oiSqlWhereEQ(DocSqlPtr,DocumentNameAttr,PartNameS));
			t5CheckDstat(oiSqlWhereAND(DocSqlPtr));
			t5CheckDstat(oiSqlWhereEQ(DocSqlPtr,SupersededAttr,"-"));
		}

		if((nlsStrStr(CatiaFileNameS,".CATPart")!=NULL)||(nlsStrStr(CatiaFileNameS,".model")!=NULL)||(nlsStrStr(CatiaFileNameS,".CMIArchive")!=NULL)) // Begin - Added By Naveen on 31- Mar - 2008 to reduce the program length.
		{
			if (nlsStrCmp(DocumentClassS,"t5MulCad")==0)
			{
				printf("\n Querying if t5MulCad is already present in PLM");fflush(stdout);
				t5CheckMfail(QueryWhere(t5MulCadClass,DocSqlPtr,&LatestDocSO,mfail));
				printf("\n No of Latest t5MulCad = [%d]",setSize(LatestDocSO)); fflush(stdout);
			}
			else
			{
				t5CheckMfail(QueryWhere(DesDocClass,DocSqlPtr,&LatestDocSO,mfail));
				printf("\n No of Latest DesDoc = [%d]",setSize(LatestDocSO)); fflush(stdout);
			}
		}
		else if (nlsStrStr(CatiaFileNameS,".CATProduct")!=NULL)
		{
			if (nlsStrCmp(DocumentClassS,"t5MulPrd")==0)
			{
				printf("\n Querying if t5MulPrd is already present in PLM");fflush(stdout);
				t5CheckMfail(QueryWhere(t5MulPrdClass,DocSqlPtr,&LatestDocSO,mfail));
				printf("\n No of Latest t5MulPrd = [%d]",setSize(LatestDocSO)); fflush(stdout);
			}
			else
			{
				t5CheckMfail(QueryWhere(x0PrdDocClass,DocSqlPtr,&LatestDocSO,mfail));
				printf("\n No of Latest x0PrdDoc = [%d]",setSize(LatestDocSO)); fflush(stdout);
			}
		}
		else if (nlsStrStr(CatiaFileNameS,".CATDrawing")!=NULL)
		{
			t5CheckMfail(QueryWhere(t5DrwDocClass,DocSqlPtr,&LatestDocSO,mfail));
			printf("\n No of Latest t5DrwDoc = [%d]",setSize(LatestDocSO)); fflush(stdout);
		}
		else if (nlsStrStr(CatiaFileNameS,".drw")!=NULL)
		{
			t5CheckMfail(QueryWhere(t5DrwDocClass,DocSqlPtr,&LatestDocSO,mfail)); 
			printf("\n No of Latest t5DrwDoc = [%d]",setSize(LatestDocSO)); fflush(stdout);
		}		
		else if (nlsStrStr(CatiaFileNameS,".asm")!=NULL || nlsStrStr(CatiaFileNameS,".prt")!=NULL)
		{
			t5CheckMfail(QueryWhere(DesDocClass,DocSqlPtr,&LatestDocSO,mfail)); 
			printf("\n No of Latest t5DrwDoc = [%d]",setSize(LatestDocSO)); fflush(stdout);
		}
		else
		{
			printf("\n CatiaFileNameS :: [%s] is not Valid for loading ",CatiaFileNameS);fflush(stdout);
			goto EXIT;
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------
		if (setSize(LatestDocSO) > 1)
		{
			printf("\n\n No of Latest Documents for [%s] = [%d] Hence Quitting \n",PartNameS,setSize(LatestDocSO));fflush(stdout);
			fprintf(fp,"\n No of Latest Documents for [%s] = [%d] Hence Quitting \n",PartNameS,setSize(LatestDocSO));fflush(fp);
			goto EXIT;
		}
		// ----------------------------------------------------------------------------------------------------------------------------------------------------
		
		if (setSize(LatestDocSO)==1)
			DocObjP=setGet(LatestDocSO,0);

		// ----------------------------------------------------------------------------------------------------------------------------------------------------
		
		if (setSize(LatestDocSO)==0)
		{	
			printf("\n DocLoad: NO Document Found Hence Creating the Document..  \n ");fflush(stdout);
			if((nlsStrStr(CatiaFileNameS,".CATPart")!=NULL)||(nlsStrStr(CatiaFileNameS,".CMIArchive")!=NULL)||(nlsStrStr(CatiaFileNameS,".model")!=NULL))
			{
				if (nlsStrCmp(DocumentClassS,"t5MulCad")==0)
				{
					printf("\n DocLoad: Creating t5MulCad Document for CATPart ");fflush(stdout);
					printf("\n DocLoad: DocumentNameS = [%s] DesignGrpS = [%s] ProjectCodeS = [%s]",DocumentNameS,DesignGrpS,ProjectCodeS);fflush(stdout);
					DocObjP=CheckAndCreateDoc(DocumentNameS,"t5MulCad",DesignGrpS,ProjectCodeS,DescriptionS,BomIndicatorS,"Catia Des.Doc",mfail);
				}
				else
				{
					printf("\n DocLoad: Creating DesDoc Document for  CATPart ");fflush(stdout);
					DocObjP=CheckAndCreateDoc(PartNameS,"DesDoc",DesignGrpS,ProjectCodeS,DescriptionS,BomIndicatorS,"Catia Des.Doc",mfail);
				}
			}
			else if(nlsStrStr(CatiaFileNameS,".CATProduct")!=NULL)
			{
				if (nlsStrCmp(DocumentClassS,"t5MulPrd")==0)
				{
					printf("\n DocLoad: Creating t5MulPrd Document for CATProduct ");fflush(stdout);
					printf("\n DocLoad: DocumentNameS = %s DesignGrpS = %s ProjectCodeS = %s",DocumentNameS,DesignGrpS,ProjectCodeS);fflush(stdout);
					DocObjP=CheckAndCreateDoc(DocumentNameS,"t5MulPrd",DesignGrpS,ProjectCodeS,DescriptionS,BomIndicatorS,"Catia Des.Doc",mfail);
					printf("\n DocLoad: After CheckAndCreateDoc() ");fflush(stdout);
				}
				else
				{
					printf("\n DocLoad: Creating x0PrdDoc Document for CATProduct ");fflush(stdout);
					DocObjP=CheckAndCreateDoc(PartNameS,"x0PrdDoc",DesignGrpS,ProjectCodeS,DescriptionS,BomIndicatorS,"Catia Des.Doc",mfail);
				}
			}
			else if(nlsStrStr(CatiaFileNameS,".CATDrawing")!=NULL)
			{
				DrwInd=1;
				DocObjP=CheckAndCreateDoc(PartNameS,"t5DrwDoc",DesignGrpS,ProjectCodeS,DescriptionS,BomIndicatorS,"Catia Drw.Doc",mfail);
			}
			else if(nlsStrStr(CatiaFileNameS,".asm")!=NULL || nlsStrStr(CatiaFileNameS,".prt")!=NULL)
			{
				DocObjP=CheckAndCreateDoc(PartNameS,"DesDoc",DesignGrpS,ProjectCodeS,DescriptionS,BomIndicatorS,"Proe Des.Doc",mfail);
			}
			else if(nlsStrStr(CatiaFileNameS,".drw")!=NULL)
			{
				DrwInd=1;
				//DocObjP=CheckAndCreateDoc(PartNameS,"DrwDoc",DesignGrpS,ProjectCodeS,DescriptionS,BomIndicatorS,"Proe Drw.Doc",mfail);
				DocObjP=CheckAndCreateDoc(PartNameS,"t5DrwDoc",DesignGrpS,ProjectCodeS,DescriptionS,BomIndicatorS,"Proe Drw.Doc",mfail);  //Class correction
			}
			else
			{

			}
		}

		t5CheckDstat(objGetAttribute(DocObjP,RevisionAttr,&CurRevDupS));
		CurRevS=nlsStrDup(CurRevDupS);
		t5CheckDstat(objGetAttribute(DocObjP,SequenceAttr,&CurSeqDupS));
		CurSeqS=nlsStrDup(CurSeqDupS);
		
		printf("\n To_Load_Doc_RevisionS = [%s]  Cur_Doc_Rev_In_Present = [%s]",DocRevisionS,CurRevS);fflush(stdout);

		/* ChechRevType function compare the current revision of document/Part  anf rev of doc (which is going to lOadd)
		   LoadCheck==1 Document/Part need to be loaded
		   LoadCheck==2 Higher revision Document/Part already presnt
		*/

		LoadCheck = ChechRevType(DocRevisionS, CurRevS, mfail);

		if (LoadCheck==1 && nlsStrCmp(DocRevisionS,CurRevS)==0)
		{
			printf("\n DocLoad: Doc Seq Checking; \t CurSeqS = [%s] \t DocSequenceS = [%s]",CurSeqS,DocSequenceS);fflush(stdout);
			if (atoi(CurSeqS)>atoi(DocSequenceS))
			{
				LoadCheck = 2;
			}
		}

		printf("\n DocLoad: Doc LoadCheck = [%d]",LoadCheck);fflush(stdout);

		if ( LoadCheck == 1)
		{
			if (nlsStrCmp(DocumentClassS,"t5MulPrd")==0 || nlsStrCmp(DocumentClassS,"t5MulCad")==0)
			{
				printf("\n DocLoad: DocumentClassS => %s Hence Skipping LoadDocRevSeq() ",DocumentClassS);fflush(stdout);
			}
			else
			{
				printf("\n DocLoad: Befor Calling LoadDocRevSeq() for Documents other than t5MulCad and t5MulPrd \n"); fflush(stdout);

				DocObjP=LoadDocRevSeq(DocObjP,DocRevisionS, DocSequenceS,VaultStatusS, mfail);
				//objDump(DocObjP);
			}
			
//			if (DocObjP) /////////////////commented by SKS 08-Oct
//				t5CheckMfail(ExpandObject(AttachClass,DocObjP,"DataItemsAttachedToBusItem",&DataPdmItemSO ,&DIObjRelSO,mfail));
//			
//			t5CheckDstat(objGetAttribute(DocObjP,SequenceAttr,&test));
//			printf("\n DocLoad: No of Attached DI to Document Sequence [%s] = [%d]",test,setSize(DataPdmItemSO)); fflush(stdout);
//
//			if (setSize(DataPdmItemSO)>0)
//			{
//				for ( LoopCnt3=0; LoopCnt3<setSize(DataPdmItemSO) ; LoopCnt3++)
//				{
//					AttachByDocItemObjP=setGet(DataPdmItemSO,LoopCnt3);
//
//					t5CheckDstat(objGetAttribute(AttachByDocItemObjP,WorkingRelativePathAttr,&DIChkWorkRelPathDupS));
//					DIChkWorkRelPathS=nlsStrDup(DIChkWorkRelPathDupS);
//
//					t5CheckDstat(objGetAttribute(AttachByDocItemObjP,SequenceAttr,&DIChkSeqDupS));
//					DIChkSeqS=nlsStrDup(DIChkSeqDupS);
//
//
//					t5CheckMfail(ExpandObject(AttachClass,AttachByDocItemObjP,"BusItemsAttachingDataItem",&AttachByDocItemSO ,&AttachByDocItemRelSO,mfail));
//					printf("\n DocLoad: WorkRelPathS = [%s] DIChkWorkRelPathS = [%s] DISequenceS = [%s] DIChkSeqS = [%s]",WorkRelPathS,DIChkWorkRelPathS,DISequenceS,DIChkSeqS);
//					if ((nlsStrCmp(WorkRelPathS,DIChkWorkRelPathS)==0)&&(nlsStrCmp(DISequenceS,DIChkSeqS)==0))
//					{
//						/*SKS This condition is useless in present condition of Trilllix. 
//						DISequenceS DIChkSeqS are never gonna be same.
//						As DI from trillix always get loaded with 1 sequence*/
//						printf("\n DocLoad: The DI of same Workingrelativepath and sequence is attached the latest document"); fflush(stdout);
//						CreFlag=1;
//					}
//					else if (setSize(AttachByDocItemSO)==1)
//					{
//						printf("\n DocLoad: The DI  is attached only with the latest document"); fflush(stdout);								
//					}
//					else
//					{
//						printf("\n DocLoad: Deleting Relation Of Old DI  to  Current Doc"); fflush(stdout);
//						//commenting removed by vineet for deleting Nr 1 Data item relation with Nr 2 document on 07/04/2016
//						//printf("\nRemoving the commenting for delete relationship of old data item with current document Before\n");fflush(stdout);
//						//t5CheckMfail(DeleteRelation2(setGet(DIObjRelSO,LoopCnt3),mfail));
//						//printf("\nRemoving the commenting for delete relationship of old data item with current document After\n");fflush(stdout);
//					}							
//				}
//			} /////////////////commented  ended by SKS 08-Oct

			if (CreFlag==1)
			{
				goto PART;
			}

//			printf("\n DocLoad: Creating Relation Of Current DI  to  Current Doc"); fflush(stdout);
//			t5CheckMfail(CreateRelation(AttachClass,DocObjP,"DataItemsAttachedToBusItem",CatiaDIObjP,&NewObjRelP,mfail));
//			RefreshObject(CatiaDIObjP,mfail);				
		}
		else
		{
			printf("\n DocLoad: Already Doc Have higher revision. So, nothing doing with the DI & Doc. Searching for the Part \n"); fflush(stdout);
		}
	
	printf("\n DocLoad: After Document Loading"); fflush(stdout);
	// ----------------------------------------------------------------------------------------------------------------------------------------------------

// 1. END : Checking if Document is Present or Not. If LatestDocSO>1 then Quit or-if LatestDocSO==1 Assign Pointer, or-if LatestDocSO==0 Create The Doc

// 2. START : Querrying CATIA File w.r.t WorkingRelativePath attribute. Quit if Already Found. 
	string CatiaFilenameDup = NULL;
	string WorkRelPathSup = NULL;
	string tempstr = NULL;
	string NewSeq = NULL;
	SqlPtr getDISup = NULL;
	SetOfObjects DIsChkSO = NULL;
	ObjectPtr SupSededDIObj = NULL;
	WorkRelPathSup  = nlsStrAlloc(200);
	int SeqInt=0;

	if (!nlsIsStrNull(CatiaFileNameS)) 
	{
		CatiaFilenameDup =nlsStrDup(CatiaFileNameS);
		strtok(CatiaFilenameDup,".");
	}
	printf("\n CatiaFilenameDup <%s> CatiaFileNameS <%s>",CatiaFilenameDup,CatiaFileNameS); fflush(stdout);
	nlsStrCpy(WorkRelPathSup,"*/"); //// added on 4 june for differenating PE parts data
	if (nlsStrStr(CatiaFileNameS,".prt")!=NULL )
	{
		nlsStrCat(WorkRelPathSup,CatiaFilenameDup);
		nlsStrCat(WorkRelPathSup,".prt*");
	}
	else if (nlsStrStr(CatiaFileNameS,".asm")!=NULL )
	{
		nlsStrCat(WorkRelPathSup,CatiaFilenameDup);
		nlsStrCat(WorkRelPathSup,".asm*");
	}
	else if (nlsStrStr(CatiaFileNameS,".drw")!=NULL)
	{
		nlsStrCat(WorkRelPathSup,CatiaFilenameDup);
		nlsStrCat(WorkRelPathSup,".drw*");
	}
	else
	{
		nlsStrCat(WorkRelPathSup,CatiaFileNameS);
	}
	
	printf("\n ging to search Working relative path <%s>",WorkRelPathSup); fflush(stdout);

	t5CheckDstat(oiSqlCreateSelect(&getDISup));
	t5CheckDstat(oiSqlWhereLike(getDISup,WorkingRelativePathAttr,WorkRelPathSup));
	t5CheckDstat(oiSqlWhereAND(getDISup));
	t5CheckDstat(oiSqlWhereEQ(getDISup,SupersededAttr,"-"));
	t5CheckDstat(oiSqlDescOrder(getDISup,CreationDateAttr));
	t5CheckMfail(oiSqlPrint(getDISup));
	if (nlsStrStr(CatiaFileNameS,".CATPart")!=NULL)
	{
		t5CheckMfail(QueryWhere(x0CatPrtClass,getDISup,&DIsChkSO,mfail));
	}
	else if (nlsStrStr(CatiaFileNameS,".CATProduct")!=NULL)
	{
		t5CheckMfail(QueryWhere(x0CatPrdClass,getDISup,&DIsChkSO,mfail));
	}
	else if (nlsStrStr(CatiaFileNameS,".CATDrawing")!=NULL)
	{
		t5CheckMfail(QueryWhere(x0CatDrwClass,getDISup,&DIsChkSO,mfail)); // End - Added By Ramesh on 12- Mar - 2009 to handle DI in wrong class.
	}
	else if (nlsStrStr(CatiaFileNameS,".prt")!=NULL)
	{
		t5CheckMfail(QueryWhere(ProPrtClass,getDISup,&DIsChkSO,mfail)); 
	}
	else if (nlsStrStr(CatiaFileNameS,".asm")!=NULL)
	{
		t5CheckMfail(QueryWhere(ProAsmClass,getDISup,&DIsChkSO,mfail)); 
	}
	else if (nlsStrStr(CatiaFileNameS,".drw")!=NULL)
	{
		t5CheckMfail(QueryWhere(ProDrwClass,getDISup,&DIsChkSO,mfail)); 
	}
	
	printf("\n Working relative path <%s> Superseded false data found %d",WorkRelPathSup,setSize(DIsChkSO)); fflush(stdout);
	if (setSize(DIsChkSO)==1)
	{
		SupSededDIObj=setGet(DIsChkSO,0);
		t5CheckDstat(objGetAttribute(SupSededDIObj,SequenceAttr,&DIChkSeqDupS));
		SeqInt=atoi(DIChkSeqDupS) + 1;

	}
	else if (setSize(DIsChkSO)==0)
	{
		SeqInt=1;
	}
	else
	{
		int sup=1;
		ObjectPtr SupDIObj = NULL;
		printf("\n More than 1 superseded data found for <%s> ",WorkRelPathSup);fflush(stdout);

		for (sup=1;sup < setSize(DIsChkSO); sup++)
		{
			SupDIObj=setGet(DIsChkSO,sup);
			t5CheckDstat(objSetAttribute(SupDIObj,SupersededAttr,"+"));
			t5CheckMfail(t0UpdateObject(&SupDIObj,mfail));
		}

			
		t5CheckDstat(oiSqlCreateSelect(&getDISup));
		t5CheckDstat(oiSqlWhereLike(getDISup,WorkingRelativePathAttr,WorkRelPathSup));
		t5CheckDstat(oiSqlWhereAND(getDISup));
		t5CheckDstat(oiSqlWhereEQ(getDISup,SupersededAttr,"-"));
		t5CheckDstat(oiSqlDescOrder(getDISup,CreationDateAttr));
		t5CheckMfail(oiSqlPrint(getDISup));
		if (nlsStrStr(CatiaFileNameS,".CATPart")!=NULL)
		{
			t5CheckMfail(QueryWhere(x0CatPrtClass,getDISup,&DIsChkSO,mfail));
		}
		else if (nlsStrStr(CatiaFileNameS,".CATProduct")!=NULL)
		{
			t5CheckMfail(QueryWhere(x0CatPrdClass,getDISup,&DIsChkSO,mfail));
		}
		else if (nlsStrStr(CatiaFileNameS,".CATDrawing")!=NULL)
		{
			t5CheckMfail(QueryWhere(x0CatDrwClass,getDISup,&DIsChkSO,mfail)); // End - Added By Ramesh on 12- Mar - 2009 to handle DI in wrong class.
		}
		else if (nlsStrStr(CatiaFileNameS,".prt")!=NULL)
		{
			t5CheckMfail(QueryWhere(ProPrtClass,getDISup,&DIsChkSO,mfail)); 
		}
		else if (nlsStrStr(CatiaFileNameS,".asm")!=NULL)
		{
			t5CheckMfail(QueryWhere(ProAsmClass,getDISup,&DIsChkSO,mfail)); 
		}
		else if (nlsStrStr(CatiaFileNameS,".drw")!=NULL)
		{
			t5CheckMfail(QueryWhere(ProDrwClass,getDISup,&DIsChkSO,mfail)); 
		}

		printf("\n Working relative path <%s> Superseded false data found %d after correction",WorkRelPathSup,setSize(DIsChkSO)); fflush(stdout);
		if (setSize(DIsChkSO)==1)
		{
			SupSededDIObj=setGet(DIsChkSO,0);
			t5CheckDstat(objGetAttribute(SupSededDIObj,SequenceAttr,&DIChkSeqDupS));
			SeqInt=atoi(DIChkSeqDupS) + 1;

		}
		else if (setSize(DIsChkSO)==0)
		{
			SeqInt=1;
		}
		else
		{
			printf("\n More than 1 superseded data found for <%s> after correction ",WorkRelPathSup);fflush(stdout);
	
			fprintf(fp,"\n More than 1 superseded data found for <%s>  after correction",WorkRelPathSup); fflush(fp);
			goto CLEANUP;
		}
	}
	
	if (nlsStrCmp(Flag_1,"1") == 0)
	{
		nlsStrCat(tempS,"Vloc/");
		nlsStrCat(tempS,ProjectCodeS);
		nlsStrCat(tempS,"/");
		nlsStrCat(tempS,DesignGrpS);
		nlsStrCat(tempS,"/");
		nlsStrCat(tempS,PartNameS);
		nlsStrCat(tempS,"_");
		nlsStrCat(tempS,RevisionS);
		nlsStrCat(tempS,"_");
		nlsStrCat(tempS,SequenceS);
		nlsStrCat(tempS,"/");
		nlsStrCat(tempS,WorkRelPathS);
		printf("\n CE Vault WorkRelPathS is [%s] ",tempS); fflush(stdout);
		printf("\n Querring w.r.t Working Relative Path of CE Vault "); fflush(stdout);
		t5CheckDstat(oiSqlCreateSelect(&getDI));
		t5CheckDstat(oiSqlWhereEQ(getDI,WorkingRelativePathAttr,tempS));
//		t5CheckDstat(oiSqlWhereAND(getDI));
//		t5CheckDstat(oiSqlWhereEQ(getDI,SequenceAttr,DISequenceS));

		WorkRelPathS = NULL;
		WorkRelPathS=nlsStrDup(tempS);
		printf("\n NEW Value for WorkRelPathS = [%s] ",WorkRelPathS); fflush(stdout);
	}
	else
	{	
		nlsStrCat(tempS,RevisionS);
		nlsStrCat(tempS,";");
		nlsStrCat(tempS,SequenceS);
		printf("\n Querring w.r.t Working Relative Path of WIP Vault "); fflush(stdout);
		printf("\n WorkRelPathS [%s] and DISequenceS = [%s] and tempRevSeq = [%s]",WorkRelPathS, DISequenceS,tempS); fflush(stdout);

		t5CheckDstat(oiSqlCreateSelect(&getDI));
		t5CheckDstat(oiSqlWhereEQ(getDI,WorkingRelativePathAttr,WorkRelPathS));
		t5CheckDstat(oiSqlWhereAND(getDI));
		t5CheckDstat(oiSqlWhereEQ(getDI,UpToDateStatusAttr,tempS));
	}
	//t5CheckMfail(QueryWhere(IndepBinClass,getDI,&DIsChkSO,mfail));
	if (nlsStrStr(CatiaFileNameS,".CATPart")!=NULL)
	{
		t5CheckMfail(QueryWhere(x0CatPrtClass,getDI,&CatiaDIsChkSO,mfail));
	}
	else if (nlsStrStr(CatiaFileNameS,".CATProduct")!=NULL)
	{
		t5CheckMfail(QueryWhere(x0CatPrdClass,getDI,&CatiaDIsChkSO,mfail));
	}
	else if (nlsStrStr(CatiaFileNameS,".CATDrawing")!=NULL)
	{
		t5CheckMfail(QueryWhere(x0CatDrwClass,getDI,&CatiaDIsChkSO,mfail)); // End - Added By Ramesh on 12- Mar - 2009 to handle DI in wrong class.
	}
	else if (nlsStrStr(CatiaFileNameS,".prt")!=NULL)
	{
		t5CheckMfail(QueryWhere(ProPrtClass,getDI,&CatiaDIsChkSO,mfail)); 
	}
	else if (nlsStrStr(CatiaFileNameS,".asm")!=NULL)
	{
		t5CheckMfail(QueryWhere(ProAsmClass,getDI,&CatiaDIsChkSO,mfail)); 
	}
	else if (nlsStrStr(CatiaFileNameS,".drw")!=NULL)
	{
		t5CheckMfail(QueryWhere(ProDrwClass,getDI,&CatiaDIsChkSO,mfail)); 
	}
	else
	{
		t5CheckMfail(QueryWhere(IndepBinClass,getDI,&CatiaDIsChkSO,mfail));
	}

	printf("\n After Querying the No. of CATIA Objects [%s] Found = [%d]",WorkRelPathS, setSize(CatiaDIsChkSO)); fflush(stdout);

	// ----------------------------------------------------------------------------------------------------------------------------------------------------
	if (setSize(CatiaDIsChkSO)==0)
	{
		t5CheckDstat(oiSqlCreateSelect(&getWorkLoc));
		t5CheckDstat(oiSqlWhereEQ(getWorkLoc,OwnerDirNameAttr,WorkLocS));
		t5CheckMfail(QueryWhere(WorkSDirClass,getWorkLoc,&WorkLocSO,mfail));

		if(setSize(WorkLocSO)!=1)
		{
			printf("\n Invalid work Location Name Please Check it or No of work location for the input is %d . So, Please use some otherwork location \n",setSize(WorkLocSO));fflush(stdout);
			
			goto CLEANUP;
		}

		WorklocObjP=setGet(WorkLocSO,0);

		t5CheckMfail(ExpandObject5(OdrContClass,WorklocObjP,"ContentsOfOwnerDirectory",SC_SCOPE_OF_SESSION, &CatiaFileSO,mfail));
		printf("\n Size of the object [%d]",setSize(CatiaFileSO)); fflush(stdout);

		for(LoopCnt1=0;LoopCnt1<setSize(CatiaFileSO);LoopCnt1++)
		{
			CatiaFileP=setGet(CatiaFileSO,LoopCnt1);
			t5CheckDstat(objGetAttribute(CatiaFileP,RelativePathAttr,&UnRegFileNameDupS));
			UnRegFileNameS=nlsStrDup(UnRegFileNameDupS);
			printf("\n UnRegFileNameS  <%s>  CatiaFileNameS <%s>",UnRegFileNameS,CatiaFileNameS); fflush(stdout);
			if(!nlsStrCmp(UnRegFileNameS,CatiaFileNameS))
			{
				printf("\n UnRegFileNameS  <%s>  CatiaFileNameS <%s>",UnRegFileNameS,CatiaFileNameS); fflush(stdout);
				t5CheckMfail(IsRegistered(CatiaFileP,&ans,mfail));
				if((ans==FALSE))
				{
					printf("\n Before Registering....");fflush(stdout);
					//Registering CATPart and setting boundary values
					if(nlsStrStr(CatiaFileNameS,".CATPart")!=NULL) // Begin - Added By Naveen on 31- Mar - 2008 to reduce the program length.
					{
						t5CheckMfail(RegisterItem(x0CatPrtClass,CatiaFileP,&CatiaDIObjP,mfail));
												
						if (!nlsIsStrNull(BoundMinXS))  t5CheckDstat(objSetAttribute(CatiaDIObjP,x0CatiaBndBoxMinXAttr,BoundMinXS));
			
						if (!nlsIsStrNull(BoundMinYS)) t5CheckDstat(objSetAttribute(CatiaDIObjP,x0CatiaBndBoxMinYAttr,BoundMinYS));
		
						if (!nlsIsStrNull(BoundMinZS)) t5CheckDstat(objSetAttribute(CatiaDIObjP,x0CatiaBndBoxMinZAttr,BoundMinZS));  
																																			
						if (!nlsIsStrNull(BoundMaxXS)) t5CheckDstat(objSetAttribute(CatiaDIObjP,x0CatiaBndBoxMaxXAttr,BoundMaxXS));	
																																			
						if (!nlsIsStrNull(BoundMaxYS)) t5CheckDstat(objSetAttribute(CatiaDIObjP,x0CatiaBndBoxMaxYAttr,BoundMaxYS));	
																																			
						if (!nlsIsStrNull(BoundMaxZS)) t5CheckDstat(objSetAttribute(CatiaDIObjP,x0CatiaBndBoxMaxZAttr,BoundMaxZS));
					}
					else if(nlsStrStr(CatiaFileNameS,".CATProduct")!=NULL)
					{
						t5CheckMfail(RegisterItem(x0CatPrdClass,CatiaFileP,&CatiaDIObjP,mfail));
					}
					else if(nlsStrStr(CatiaFileNameS,".CATDrawing")!=NULL)
					{
						t5CheckMfail(RegisterItem(x0CatDrwClass,CatiaFileP,&CatiaDIObjP,mfail));
					}
					else if(nlsStrStr(CatiaFileNameS,".model")!=NULL)
					{
						t5CheckMfail(RegisterItem(x0CTModClass,CatiaFileP,&CatiaDIObjP,mfail));
					}
					else if(nlsStrStr(CatiaFileNameS,".CMIArchive")!=NULL)
					{
						t5CheckMfail(RegisterItem(x0CatArcClass,CatiaFileP,&CatiaDIObjP,mfail)); 
					}
					else if(nlsStrStr(CatiaFileNameS,".asm")!=NULL)
					{
						t5CheckMfail(RegisterItem(ProAsmClass,CatiaFileP,&CatiaDIObjP,mfail)); 
					}
					else if(nlsStrStr(CatiaFileNameS,".prt")!=NULL)
					{
						t5CheckMfail(RegisterItem(ProPrtClass,CatiaFileP,&CatiaDIObjP,mfail)); 
					}
					else if(nlsStrStr(CatiaFileNameS,".drw")!=NULL)
					{
						t5CheckMfail(RegisterItem(ProDrwClass,CatiaFileP,&CatiaDIObjP,mfail)); 
					}
//					printf("\n Setting Working Relative Path Attr as [%s] ",WorkRelPathS); fflush(stdout);
//					t5CheckDstat(objSetAttribute(CatiaDIObjP,WorkingRelativePathAttr,WorkRelPathS));
					if (nlsStrCmp(Flag_1,"1") != 0)
					{
						printf("\n>>>>>>>>>> Inside stamping Up 2 Dt St [%s]<<<<<<<<<<<<<<\n",tempS); fflush(stdout);
						t5CheckDstat(objSetAttribute(CatiaDIObjP,UpToDateStatusAttr,tempS));
					}
					printf("\n DONE Registering the CATIA Part [%s]",CatiaFileNameS); fflush(stdout);
					tempstr=nlsStrAlloc(5);
					sprintf(tempstr,"%d",SeqInt);
					NewSeq = nlsStrDup(tempstr);
					DISequenceS = nlsStrDup(tempstr);
					printf("\n  Sequence to be stamped for DI [%s] [%s] \n",NewSeq,DISequenceS);fflush(stdout);
					t5CheckDstat(objSetAttribute(CatiaDIObjP,SequenceAttr,NewSeq));
					t5CheckMfail(t0UpdateObject(&CatiaDIObjP,mfail));
					RefreshObject(CatiaDIObjP,mfail);
				}
				else
				{
					printf("\n [%s] Already registered \n",CatiaFileNameS);fflush(stdout);
					t5CheckDstat(objCopy(&CatiaDIObjP,CatiaFileP));
					t5CheckMfail(t0UpdateObject(&CatiaDIObjP,mfail));
				}
				break;
			}
		}
	}
	else if (setSize(CatiaDIsChkSO)==1)
	{	
		CatiaDIObjP=setGet(CatiaDIsChkSO,0);
		t5CheckDstat(objGetAttribute(CatiaDIObjP,OwnerNameAttr,&DIDiffOwnerNameS));
		printf("\n In DB DIOwnerNameS = [%s]",DIDiffOwnerNameS);fflush(stdout);
		if (nlsStrStr(DIDiffOwnerNameS,"Vault")==NULL)
		{
			printf("\n WorkingRelativePath = [%s] Sequence = [%s] is already registered and is in ownership 0f [%s] \n TC Status [%s], [%s], [%s] Doc Status [%s], [%s]. Pls check DI",WorkRelPathS,DISequenceS,DIDiffOwnerNameS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);
			fprintf(fp,"\n WorkingRelativePath = [%s] Sequence = [%s] is already registered and is in ownership 0f [%s] \n TC Status [%s], [%s], [%s] Doc Status [%s], [%s]. Pls check DI",WorkRelPathS,DISequenceS,DIDiffOwnerNameS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);
			
			goto CLEANUP;
		}
		else if ((strcmp(VaultStatusS,"WIP")==0) && (nlsStrCmp(DIDiffOwnerNameS,"Release Vault")==0))
		{
			printf("\n WorkingRelativePath = [%s] Sequence = [%s] is Released already and so cannot be transferred to WIP \n TC Status [%s], [%s], [%s] Doc Status [%s], [%s]",WorkRelPathS,DISequenceS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);
			fprintf(fp,"\n WorkingRelativePath = [%s] Sequence = [%s] is Released already and so cannot be transferred to WIP \n TC Status [%s], [%s], [%s] Doc Status [%s], [%s]",WorkRelPathS,DISequenceS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);

			goto EXIT;
		}
		else if ((strcmp(VaultStatusS,"Released")==0) && (nlsStrCmp(DIDiffOwnerNameS,"WIP Vault")==0))
		{
			printf("\n [%s] is  registered already and is being Released Now.\n",WorkRelPathS); fflush(stdout);
		}
		else if (nlsStrCmp(RefDrwName,"NULL")!=0)
		{
			printf("\n Checking Availablity of DI for Ref drw Relation [%s],[%s] \n",WorkRelPathS,DISequenceS); fflush(stdout);		
		}
		else
		{
			printf("\n WorkingRelativePath = [%s] Sequence = [%s] is  registered already and is with same owner [%s] \n TC Status [%s], [%s], [%s] Doc Status [%s], [%s].\n",WorkRelPathS,DISequenceS,VaultStatusS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);
			fprintf(fp,"\n WorkingRelativePath = [%s] Sequence = [%s] is registered already and is with same owner [%s] \n TC Status [%s], [%s], [%s] Doc Status [%s], [%s].\n",WorkRelPathS,DISequenceS,VaultStatusS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);			
		}		
	}
	else
	{
		fprintf(fp,"\n More than one DI with Same WorkingRelativePath = [%s] Sequence = [%s].\n TC Status [%s], [%s], [%s] Doc Status [%s], [%s].",WorkRelPathS,DISequenceS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);
		printf("\n More than one DI with Same WorkingRelativePath = [%s] Sequence = [%s]. \n TC Status [%s], [%s], [%s] Doc Status [%s], [%s].",WorkRelPathS,DISequenceS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);
		if (nlsStrCmp(RefDrwName,"NULL")==0)
		{
			goto EXIT;
		}
		else
		{
			goto REFDOCREL;
		}
	}

	if(CatiaDIObjP)
	{
		t5CheckDstat(objSetAttribute(CatiaDIObjP,ProjectNameAttr,ProjectCodeS));
		t5CheckMfail(t0UpdateObject(&CatiaDIObjP,mfail));
	}
	else
	{
		fprintf(fp,"\n DI object not found in workloc or not registered WorkingRelativePath = [%s] Sequence = [%s].\n TC Status [%s], [%s], [%s] Doc Status [%s], [%s].",WorkRelPathS,DISequenceS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);
		printf("\n  DI object not found in workloc or not registered  WorkingRelativePath = [%s] Sequence = [%s]. \n TC Status [%s], [%s], [%s] Doc Status [%s], [%s].",WorkRelPathS,DISequenceS,PartNameS,RevisionS,SequenceS,DocRevisionS,DocSequenceS); fflush(stdout);
	}
	// ----------------------------------------------------------------------------------------------------------------------------------------------------
// 2. END : Querrying CATIA File w.r.t WorkingRelativePath attribute. Quit if Already Found. 

	/*if (strcmp(VaultStatusS,"Released")==0 && nlsStrCmp(CheckedOutS,"super user")!=0) /// NOTCHECKIN    Document Checkin case
	{
		printf("\n Transferin DOCUMENT to RELEASE VAULT"); fflush(stdout);
		t5CheckMfail(SetUpDialog(objClass(DocObjP),DocObjP,"TransferDialogC",&extraStr,&extraObj, &TranDialogueObjP,mfail));
		t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"Release Vault"));
		t5CheckDstat(objSetAttribute(TranDialogueObjP,IncludeAttachedDIsAttr,"-"));
		t5CheckMfail(TransferAllObject(DocObjP,TranDialogueObjP,&failedSO,mfail));
		printf("\n Document Transfered to Release Vault"); fflush(stdout);
	}*/ // NOTCHECKIN
	
	DataPdmItemSO	= NULL;
	DIObjRelSO		= NULL;

	if (DocObjP)t5CheckMfail(ExpandObject(AttachClass,DocObjP,"DataItemsAttachedToBusItem",&DataPdmItemSO ,&DIObjRelSO,mfail));
		//printf("objDump of Currect Doc");
		//objDump(DocObjP);//Vineet
		
	printf("\n Creating Relation Of Current DI  with Current Doc"); fflush(stdout);
	printf("\n DocLoad: No of Attached DI = [%d]",setSize(DataPdmItemSO)); fflush(stdout);
//	if (setSize(DataPdmItemSO)==0)  // commented by SKS to create relation form more than 1 DI (Spec Doc)
//	{
	t5CheckMfail(GetRelations(AttachClass,DocObjP,"DataItemsAttachedToBusItem",CatiaDIObjP,&ExistingObjRelP,mfail)); // sks added to get any existing relation if 0 relation found then create relation 
	if (setSize(ExistingObjRelP)==0)
	{
	
		t5CheckMfail(CreateRelation(AttachClass,DocObjP,"DataItemsAttachedToBusItem",CatiaDIObjP,&NewObjRelP,mfail));
		printf("\n Relation of Document with DI created.."); fflush(stdout);
		t5CheckDstat(objGetAttribute(DocObjP,OwnerNameAttr,&CheckedOutDupS));
		CheckedOutS=nlsStrDup(CheckedOutDupS);
		/*if (nlsStrCmp(CheckedOutS,"Loader")==0 || nlsStrCmp(CheckedOutS,"super user")==0) // NOTCHECKIN Document Checkin case
		{
			printf("\n Document is Currently Checked Out. Checking In the Document to %s Vault",VaultStatusS); fflush(stdout);
			
			if (strcmp(VaultStatusS,"Released")==0)
			{	
				t5CheckMfail(CheckInItem(DocObjP,"Release Vault", mfail));
			}
			else
			{
				t5CheckMfail(SetUpDialog(objClass(DocObjP),DocObjP,"TransferDialogC",&extraStr,&extraObj, &TranDialogueObjP,mfail));
				if ((strcmp(VaultStatusS,"WIP")==0 || strcmp(VaultStatusS,"HNJ")==0) && nlsStrCmp(Flag_1,"0")==0)			
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"WIP Vault"));
				if ((strcmp(VaultStatusS,"WIP")==0 || strcmp(VaultStatusS,"HNJ")==0) && nlsStrCmp(Flag_1,"1") == 0)			
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"CE Vault"));
				if (strcmp(VaultStatusS,"CE")==0)			
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"CE Vault"));

				t5CheckDstat(objSetAttribute(TranDialogueObjP,IncludeAttachedDIsAttr,"+"));
				t5CheckMfail(TransferAllObject(DocObjP,TranDialogueObjP,&failedSO,mfail));
				t5CheckMfail(PerformFreeze(DocObjP,mfail));
			}
		}*/ // NOTCHECKIN
	}
//
//	} // commented by SKS to create relation form more than 1 DI (Spec Doc)
		
		RefreshObject(CatiaDIObjP,mfail);

		if(CatiaDIObjP==NULL)
		{
			goto CLEANUP;
		}
		RefreshObject(CatiaDIObjP,mfail);
			
		t5CheckDstat(objGetAttribute(CatiaDIObjP,OwnerNameAttr,&DIOwnerNameS));
		printf("\n Owner Name of the Catia File is [%s]", DIOwnerNameS); fflush(stdout);

		//Editing for CE Vault.
		/*if ((nlsStrCmp(DIOwnerNameS,"Release Vault")!=0) && (nlsStrCmp(DIOwnerNameS,"WIP Vault")!=0) && (nlsStrCmp(DIOwnerNameS,"CE Vault")!=0)) //NOTCHECKIN
		{
			printf("\n VaultStatusS name is --> [%s]", VaultStatusS); fflush(stdout);
			if (strcmp(VaultStatusS,"Released")==0)
			{
				printf("\n Check in to Release Vault"); fflush(stdout);

				//t5CheckMfail(CheckInItem(CatiaDIObjP,"Release Vault", mfail));
				t5CheckDstat(SetUpDialog(objClass(CatiaDIObjP),CatiaDIObjP,"TransferDialogC",&extraStr,&extraObj,&TranDialogueObjP,mfail));

				if (nlsStrCmp(Flag_1,"1") == 0)
				{
					printf("\n Transfering to CE Vault First.. "); fflush(stdout);
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"CE Vault"));
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerDirNameAttr,"CE_Vault_Loc"));					
					//t5CheckDstat(objSetAttribute(TranDialogueObjP,ZBlobAttr,"-"));
					t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));
					
					TranDialogueObjP=NULL;
					extraStr=NULL;
					extraObj=NULL;

					t5CheckMfail(RefreshObject(CatiaDIObjP,mfail));
					printf("\n Transfering Release Vault from CE Vault"); fflush(stdout);
					t5CheckDstat(SetUpDialog(objClass(CatiaDIObjP),CatiaDIObjP,"TransferDialogC",&extraStr,&extraObj,&TranDialogueObjP,mfail));
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"Release Vault"));
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerDirNameAttr,"Rel_Vault_Loc"));
					
					printf("\n Before TransferObject "); fflush(stdout);					
					t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));
					printf("\n Transfered to Release Vault"); fflush(stdout);
					t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsErcRlzd"));
				}
				else
				{	
					printf("\n Trasnferring it to Non Live Rel Vault "); fflush(stdout);
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"Release Vault"));					
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerDirNameAttr,"Rel_CAD_Vault_Loc"));

					t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));

					t5CheckDstat(objSetAttribute(CatiaDIObjP,WorkingRelativePathAttr,WorkRelPathS)); // modified to solve 13 digit working rel path issue of WIP vault parts
					if(dstat=t0UpdateObject(&CatiaDIObjP,mfail))goto EXIT;
				}
					//				t5CheckDstat(objSetAttribute(TranDialogueObjP,ZBlobAttr,"-"));
				
					//				t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));
				
				t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsErcRlzd"));
			}

			if ((strcmp(VaultStatusS,"WIP")==0 || strcmp(VaultStatusS,"HNJ")==0) && nlsStrCmp(Flag_1,"0")==0)
			{
				printf("\n Check in to WIP Vault"); fflush(stdout);
				//t5CheckMfail(CheckInItem(CatiaDIObjP,"WIP Vault",mfail));
				t5CheckDstat(SetUpDialog(objClass(CatiaDIObjP),CatiaDIObjP,"TransferDialogC",&extraStr,&extraObj,&TranDialogueObjP,mfail));
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"WIP Vault"));
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerDirNameAttr,"WIP_CAD_Vault_Loc"));				
				t5CheckDstat(objSetAttribute(TranDialogueObjP,ZBlobAttr,"-"));
				
				t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));			
				
				t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsReview"));
				t5CheckDstat(objSetAttribute(CatiaDIObjP,WorkingRelativePathAttr,WorkRelPathS)); // modified to solve 13 digit working rel path issue of WIP vault parts
				if(dstat=t0UpdateObject(&CatiaDIObjP,mfail))goto EXIT;
			}
			// CE CASE 	
			if ((strcmp(VaultStatusS,"WIP")==0 || strcmp(VaultStatusS,"HNJ")==0) && nlsStrCmp(Flag_1,"1") == 0)
			{
				printf("\n Downloaded as WIP But Checkin To CE Vault @ PUNE "); fflush(stdout);
				t5CheckDstat(SetUpDialog(objClass(CatiaDIObjP),CatiaDIObjP,"TransferDialogC",&extraStr,&extraObj,&TranDialogueObjP,mfail));
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"CE Vault"));
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerDirNameAttr,"CE_Vault_Loc"));
				t5CheckDstat(objSetAttribute(TranDialogueObjP,ZBlobAttr,"-"));				

				t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));

				t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsReview"));
				printf("\n Refresh Object"); fflush(stdout);
				if(dstat=RefreshObject(CatiaDIObjP,mfail)) goto EXIT;
				printf("\n t0Update Object"); fflush(stdout);
				if(dstat=t0UpdateObject(&CatiaDIObjP,mfail))goto EXIT;
				printf("\n DONE t0Update Object"); fflush(stdout);
			}
			if (strcmp(VaultStatusS,"CE")==0)
			{
				printf("\n Check in to CE Vault"); fflush(stdout);
				//t5CheckMfail(CheckInItem(CatiaDIObjP,"WIP Vault",mfail));
				t5CheckDstat(SetUpDialog(objClass(CatiaDIObjP),CatiaDIObjP,"TransferDialogC",&extraStr,&extraObj,&TranDialogueObjP,mfail));
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"CE Vault"));
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerDirNameAttr,"CE_Vault_Loc"));
				t5CheckDstat(objSetAttribute(TranDialogueObjP,ZBlobAttr,"-"));
				
				t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));

				t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsReview"));
			}
			// CE CASE 	
			
			//t5CheckDstat(objSetAttribute(CatiaDIObjP,SequenceAttr,DISequenceS));
			t5CheckDstat(objSetAttribute(CatiaDIObjP,ProjectNameAttr,ProjectCodeS));

			//t5CheckDstat(objSetAttribute(CatiaDIObjP,WorkingRelativePathAttr,WorkRelPathS));
			//t5CheckDstat(objSetAttribute(CatiaDIObjP,RelativePathAttr,CatiaFileNameS));
			
			//DiDescS=nlsStrAlloc(nlsStrLen(WorkRelPathS)+1);

			//nlsStrCpy(DiDescS,WorkRelPathS);
			//strtok( DiDescS, "." );
			//printf("\n Data Item Description = [%s] ",DiDescS); fflush(stdout);
			//t5CheckDstat(objSetAttribute(CatiaDIObjP,DataItemDescAttr,DiDescS));
			t5CheckDstat(objSetAttribute(CatiaDIObjP,CheckedOutAttr,"-"));
			t5CheckMfail(PerformFreeze(CatiaDIObjP,mfail));
			t5CheckMfail(t0UpdateObject(&CatiaDIObjP,mfail));
		}
		else
		{
			if (((strcmp(VaultStatusS,"Released")==0) && (nlsStrCmp(DIOwnerNameS,"Release Vault")==0)) || ((strcmp(VaultStatusS,"WIP")==0) && (nlsStrCmp(DIOwnerNameS,"WIP Vault")==0) ||(nlsStrCmp(DIOwnerNameS,"CE Vault")==0)))			// (strcmp(VaultStatusS,"CE")==0) && (nlsStrCmp(DIOwnerNameS,"CE Vault")==0))
			{
				t5CheckMfail(PerformFreeze(CatiaDIObjP,mfail));
				printf("\n The catia part has already bein registered and checkin to same vaults ...  [%s] \n",DIOwnerNameS);fflush(stdout);
			}
			else
			{
				printf("\n Different Vault So, Transfer of ownership \n"); fflush(stdout);
				t5CheckDstat(SetUpDialog(objClass(CatiaDIObjP),CatiaDIObjP,"TransferDialogC",&extraStr,&extraObj,&TranDialogueObjP,mfail));
				if (strcmp(VaultStatusS,"Released")==0)
				{
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"Release Vault"));
				}
				if (strcmp(VaultStatusS,"WIP")==0 && nlsStrCmp(Flag_1,"0") == 0)
				{
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"WIP Vault"));
				}
				if (strcmp(VaultStatusS,"CE")==0)
				{
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"CE Vault"));
				}
				if (strcmp(VaultStatusS,"WIP")==0 && nlsStrCmp(Flag_1,"1") == 0)
				{
					t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"CE Vault"));
				}

				t5CheckDstat(objSetAttribute(TranDialogueObjP,ZBlobAttr,"-"));
				t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));

				if (strcmp(VaultStatusS,"Released")==0)
					t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsErcRlzd"));

				if (strcmp(VaultStatusS,"WIP")==0 || strcmp(VaultStatusS,"CE")==0)
					t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsWorking"));

				printf("\n After TransferObject Dialog \n"); fflush(stdout);
			}
		} */  //NOTCHECKIN

		if (CatiaDIObjP)
		{
			if ( nlsStrCmp(VisFile,"") && !nlsIsStrNull(VisFile))
			{
				printf("\n Inside Vis File write \n"); fflush(stdout);
				string DiObId=NULL;
				string DiRelPath=NULL;
				FILE	*fpVis	=	NULL;

				if(dstat=RefreshObject(CatiaDIObjP,mfail)) goto EXIT;
				t5CheckDstat(objGetAttribute(CatiaDIObjP,OBIDAttr,&DiObId));
				t5CheckDstat(objGetAttribute(CatiaDIObjP,RelativePathAttr,&DiRelPath));
				t5CheckDstat(objGetAttribute(CatiaDIObjP,OwnerNameAttr,&DiOwnName));

	//			nlsStrCat(FileVisPath,RepPath);
	//			nlsStrCat(FileVisPath,"/VisJtPdf.txt");
				printf("\n VisFile %s,%s,%s,%s,%s,%s,\n\n",RepPath,DiObId,DiRelPath,VisFile,DiOwnName,Flag_1); fflush(stdout);
				
				fpVis=fopen(RepPath,"a+");fflush(fpVis);
				fprintf(fpVis,"%s,%s,%s,%s,%s,\n",DiObId,DiRelPath,DiOwnName,VisFile,Flag_1);fflush(fpVis);
				if(fpVis) fclose(fpVis);
			}


		}
		if (SupSededDIObj && CatiaDIObjP)
		{
			t5CheckDstat(objGetAttribute(SupSededDIObj,SequenceAttr,&SupersedesDISeqDupS));
			SupersedesDISeqS=nlsStrDup(SupersedesDISeqDupS);
			printf("\n SupersedesDISeqS <%s> \n",SupersedesDISeqS);fflush(stdout);

			t5CheckDstat(objGetClass(SupSededDIObj,&SupersedesDIClassDupS));
			SupersedesDIClassS=nlsStrDup(SupersedesDIClassDupS);

			RefreshObject(CatiaDIObjP,mfail);
			t5CheckDstat(objGetClass(CatiaDIObjP,&DIClassDupS));
			DIClassS=nlsStrDup(DIClassDupS);
			t5CheckMfail(ExpandObject(ChekInRClass,CatiaDIObjP,"OriginalOfCheckedInItem",&PrvSupersedObjSO ,&PrvSupersedRelObjSO,mfail));
			//t5CheckMfail(ExpandObject(ChekOutRClass,CatiaDIObjP,"OriginalOfCheckedOutItem",&PrvSupersedObjSO ,&PrvSupersedRelObjSO,mfail));
			printf("\n PrvSupersedObjSO = [%d] SupersedesDISeqS = [%s] DISequenceS=[%s]",setSize(PrvSupersedObjSO),SupersedesDISeqS,DISequenceS);fflush(stdout);
			if ((setSize(PrvSupersedObjSO)==0) && (nlsStrCmp(SupersedesDISeqS,DISequenceS)!=0) && (nlsStrCmp(SupersedesDIClassS,DIClassS)==0))
			{				
				t5CheckMfail(CreateRelation(ChekInRClass,CatiaDIObjP,"OriginalOfCheckedInItem",SupSededDIObj,&RelItemObjP,mfail));
				//t5CheckMfail(CreateRelation(ChekOutRClass,CatiaDIObjP,"OriginalOfCheckedOutItem",SupSededDIObj,&RelItemObjP,mfail));
				t5CheckDstat(objSetAttribute(SupSededDIObj, SupersededAttr, "+"));
				t5CheckDstat(objSetAttribute(SupSededDIObj, SupersededByCkiCkoAttr, "+"));
				t5CheckMfail(t0UpdateObject(&SupSededDIObj,mfail));
				RefreshObject(SupSededDIObj,mfail);
				
				printf("\n Supersedes Relation created \n");fflush(stdout);
			}
			else
			{
				printf("\n Supersedes Relation Not created \n");fflush(stdout);
			}			
			
		}
		else
		{
			printf("\n The superseded DI  is NULL \n ");fflush(stdout);		
		}
	}

	/////////////////////////DI creation ended///////////////////////////////////////////

PART:

	printf("\n\n PART: is Called...  ");fflush(stdout);
	printf("\n PART: In Part RefDrwName = [%s]",RefDrwName);  fflush(stdout);
	if (nlsStrCmp(RefDrwName,"NULL")==0)
	{				
		t5CheckDstat(oiSqlCreateSelect(&PartsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(PartsqlPtr,PartNumberAttr,PartNameS));
		t5CheckDstat(oiSqlWhereAND(PartsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(PartsqlPtr,SupersededAttr,"-"));

		t5CheckMfail(QueryWhere(PartClass,PartsqlPtr,&LatestPartSO,mfail));
		printf("\n PART: After Query Number of TC Parts Found = [%d]",setSize(LatestPartSO));fflush(stdout);

		if (setSize(LatestPartSO)==0)
		{
			if((nlsStrStr(CatiaFileNameS,".CATProduct")!=NULL)||(nlsStrStr(CatiaFileNameS,".CATDrawing")!=NULL)||(nlsStrStr(CatiaFileNameS,".CMIArchive")!=NULL)||(nlsStrStr(CatiaFileNameS,".model")!=NULL)||(nlsStrStr(CatiaFileNameS,"NULL")!=NULL) || (nlsStrStr(CatiaFileNameS,".CATPart")!=NULL) || (nlsStrStr(CatiaFileNameS,".asm")!=NULL) || (nlsStrStr(CatiaFileNameS,".prt")!=NULL) || (nlsStrStr(CatiaFileNameS,".drw")!=NULL))
			{
				printf("\n PART: Creating Part with Class Type as %s \n",PrtType);fflush(stdout);
				if (nlsStrCmp(PrtType,"G")==0)
				{
					PartObjP=CheckAndCreatePart(PartNameS,RevisionS,SequenceS,"t5GrpID",DesignGrpS,ProjectCodeS,DescriptionS,VaultStatusS,PrtType,mcreator,VerCreator,moddtemp,CheckInSts,mfail);
				}
				else if (nlsStrCmp(DesignGrpS,"16")==0)
				{
					PartObjP=CheckAndCreatePart(PartNameS,RevisionS,SequenceS,"t5EeAsm",DesignGrpS,ProjectCodeS,DescriptionS,VaultStatusS,PrtType,mcreator,VerCreator,moddtemp,CheckInSts,mfail);
				}
				else
				{
					PartObjP=CheckAndCreatePart(PartNameS,RevisionS,SequenceS,"Assembly",DesignGrpS,ProjectCodeS,DescriptionS,VaultStatusS,PrtType,mcreator,VerCreator,moddtemp,CheckInSts,mfail);
					
				}
			}


//string mcreator,VerCreator
//	string VerCreator,
//			if(nlsStrStr(CatiaFileNameS,".CATPart")!=NULL)
//			{
//				printf("\n PART: Creating Component");fflush(stdout);
//				PartObjP=CheckAndCreatePart(PartNameS,RevisionS,SequenceS,"Cmponent",DesignGrpS,ProjectCodeS,DescriptionS,VaultStatusS,"C",mfail);
//			}
		}
		else
		{
			printf("\n PART: Inside setSize(LatestPartSO) > 0");fflush(stdout);
			PartObjP=setGet(LatestPartSO,0);		
		}
		
		t5CheckDstat(objGetAttribute(PartObjP,RevisionAttr,&CurRevDupS));
		CurRevS=nlsStrDup(CurRevDupS);
		t5CheckDstat(objGetAttribute(PartObjP,SequenceAttr,&CurSeqDupS));
		CurSeqS=nlsStrDup(CurSeqDupS);
		
		printf("\n PART: Updating KeyWord Description and Initial CS ");fflush(stdout);
		if (nlsStrCmp(KeywordDescriptionS,"NULL")!=0) objSetAttribute(PartObjP,CategoryNameAttr,KeywordDescriptionS);
		if (nlsStrCmp(InitialCS,"NULL")!=0) objSetAttribute(PartObjP,MakeBuyIndicatorAttr,InitialCS);

		t5CheckMfail(t0UpdateObject(&PartObjP,mfail));

		printf("\n PART: Calling ChechRevType():");fflush(stdout);
		/* ChechRevType function compare the current revision of document  anf rev of doc (which is going to lOadd)
		   LoadCheck==1 Document need to be loaded
		   LoadCheck==2 Higher revision Document already presnt
		*/
		LoadCheck=ChechRevType(RevisionS, CurRevS, mfail);
		printf("\n PART: TC LoadCheck = [%d]",LoadCheck);fflush(stdout);


		if ( LoadCheck == 1)
		{
			printf("\n PART: Calling LoadPartRevSeq():");fflush(stdout);
			PartObjP=LoadPartRevSeq(PartObjP,RevisionS, SequenceS,VaultStatusS,RevChkIdS,mcreator,VerCreator,moddtemp,mfail);
			t5CheckDstat(objGetAttribute(PartObjP,SequenceAttr,&CurSeqS));
			t5CheckDstat(objGetAttribute(PartObjP,RevisionAttr,&CurRevS));
			string CurRevDupS = NULL;
			string CurSeqDupS = NULL;
			if (!nlsIsStrNull(CurRevS))CurRevDupS=nlsStrDup(CurRevS);
			if (!nlsIsStrNull(CurSeqS))CurSeqDupS=nlsStrDup(CurSeqS);
			string OwnerCheckDupS = NULL;
			string OwnerCheckS = NULL;
			t5CheckDstat(objGetAttribute(PartObjP,CheckedOutAttr,&CheckedOutDupS));
			if (!nlsIsStrNull(CheckedOutDupS))CheckedOutS=nlsStrDup(CheckedOutDupS);
			t5CheckDstat(objGetAttribute(PartObjP,OwnerNameAttr,&OwnerCheckDupS));
			if (!nlsIsStrNull(OwnerCheckDupS))OwnerCheckS=nlsStrDup(OwnerCheckDupS);
			
			if(!nlsStrCmp(CurRevDupS,RevisionS) && !nlsStrCmp(CurSeqDupS,SequenceS))   
			{                                                                    
				//Added to change the description every time we create the Part of higher rev or seq 
				PartObjP=AssignLoadPartToView(PartObjP,mfail);
				t5CheckMfail(objSetAttribute(PartObjP,NomenclatureAttr,DescriptionS));
				t5CheckMfail(objSetAttribute(PartObjP,t5PartStatusAttr,DRstatus));
				t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
				t5CheckDstat(ChangeStateItem(PartObjP,"LcsAPLWrkg",mfail));//Added for APL Working LCS
			}
		
			printf("\n After LoadPartRevSeq(): Owner of the Part is = [%s] checkeout Attr = [%s] current Rev  = [%s]  current seq = [%s] \n  ",OwnerCheckS,CheckedOutS,CurRevDupS,CurSeqDupS);fflush(stdout);					
		}
		else
		{
			printf("\n PART: Already TC Part Have higher revision. \n");fflush(stdout);
			goto CLEANUP;
		}
	}
	else
	{
		printf("\n Ref DRw case Hence Not Creating TC Part   \n");fflush(stdout);
		goto REFDOCREL;
	}

	string MasterOwnerNm = NULL;
	SetOfObjects PartObjMstSo = NULL;
	SetOfObjects PartObjMstSoRel = NULL;
	//PartObjMstSo and PartObjMstSoRel variables should be SetOfObjects and not SetOfStrings
	//SetOfStrings PartObjMstSo = NULL;
	//SetOfStrings PartObjMstSoRel = NULL;
	t5CheckMfail(ExpandObject2(ItemRevClass,PartObjP,"ItemMstrForStrucBIRev",SC_SCOPE_OF_SESSION,&PartObjMstSo,&PartObjMstSoRel,mfail)) ;  
	printf("\n Master count is %d\n",setSize(PartObjMstSo));fflush(stdout);
	if (setSize(PartObjMstSo)>0)
	{
		t5CheckDstat(objGetAttribute(setGet(PartObjMstSo,0),OwnerNameAttr,&MasterOwnerNm));
		printf("\n Master owner name is %s\n",MasterOwnerNm);fflush(stdout);
		if (nlsStrStr(MasterOwnerNm,"Vault")==NULL)
		{
			printf("\n Inside t5CheckInCstAndMstrObject is \n");fflush(stdout);
			if(dstat = t5CheckInCstAndMstrObject(PartObjP,mfail))
			{
				printf("\n Error in transfering the Part master and cost. Please check");fflush(stdout);
			}
		}
	}
	else
	{
		printf("\n Master not found ");fflush(stdout);
	}
PARTDOCREL:	

	printf("\n\n PARTDOCREL: is Called...  %d \n",DrwInd);fflush(stdout);
	if (DrwInd)
	{
		t5CheckMfail(objSetAttribute(PartObjP,t5DrawingIndAttr,"D"));
		t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
	}

	printf("\n PARTDOCREL: In PARTDOCREL ");  fflush(stdout);	
	t5CheckDstat(objGetAttribute(PartObjP,RevisionAttr,&CurRevDupS));
	if (!nlsIsStrNull(CurRevDupS)) CurRevS=nlsStrDup(CurRevDupS);
	t5CheckDstat(objGetAttribute(PartObjP,SequenceAttr,&CurSeqDupS));
	if (!nlsIsStrNull(CurSeqDupS)) CurSeqS=nlsStrDup(CurSeqDupS);
	printf("\n PARTDOCREL: CurRevS = [%s] RevisionS = [%s] CurSeqS = [%s] SequenceS = [%s]",CurRevS,RevisionS,CurSeqS,SequenceS); fflush(stdout);
	if(!nlsStrCmp(CurRevS,RevisionS) && !nlsStrCmp(CurSeqS,SequenceS))
	{	
		RefreshObject(PartObjP,mfail);		
		if (AllDoctempSO)
		{
			t5FreeSetOfObjects(AllDoctempSO);
		}
		
		t5CheckDstat(oiSqlCreateSelect(&DocRelSqlPtr));
		printf("\n PARTDOCREL: CatiaFileNameS = [%s] \t DocumentClassS = [%s] ",CatiaFileNameS, DocumentClassS); fflush(stdout);
		if (nlsStrCmp(DocumentClassS,"t5MulCad")==0 || nlsStrCmp(DocumentClassS,"t5MulPrd")==0)
		{
			printf("\n PARTDOCREL: This is the Converted Document Case hence, PartNameS = [%s]",DocumentNameS); fflush(stdout);
			t5CheckDstat(oiSqlWhereEQ(DocRelSqlPtr,DocumentNameAttr,DocumentNameS));
		}
		else
		{
			t5CheckDstat(oiSqlWhereEQ(DocRelSqlPtr,DocumentNameAttr,PartNameS));
			t5CheckDstat(oiSqlWhereAND(DocRelSqlPtr));
			t5CheckDstat(oiSqlWhereEQ(DocRelSqlPtr,RevisionAttr,DocRevisionS));
			t5CheckDstat(oiSqlWhereAND(DocRelSqlPtr));
			t5CheckDstat(oiSqlWhereEQ(DocRelSqlPtr,SequenceAttr,DocSequenceS));
		}

		if((nlsStrStr(CatiaFileNameS,".CATPart")!=NULL)||(nlsStrStr(CatiaFileNameS,".CMIArchive")!=NULL)||(nlsStrStr(CatiaFileNameS,".model")!=NULL))
		{
			if (nlsStrCmp(DocumentClassS,"t5MulCad")==0)
			{
				printf("\n PARTDOCREL: Querying t5MulCad "); fflush(stdout);
				t5CheckMfail(QueryWhere("t5MulCad",DocRelSqlPtr,&AllDoctempSO,mfail));
			}
			else
			{
				t5CheckMfail(QueryWhere("DesDoc",DocRelSqlPtr,&AllDoctempSO,mfail));
			}
		}
		else if(nlsStrStr(CatiaFileNameS,".CATProduct")!=NULL)
		{
			if (nlsStrCmp(DocumentClassS,"t5MulPrd")==0)
			{
				printf("\n PARTDOCREL: Querying t5MulPrd "); fflush(stdout);
				t5CheckMfail(QueryWhere("t5MulPrd",DocRelSqlPtr,&AllDoctempSO,mfail));
			}
			else
			{
				t5CheckMfail(QueryWhere("x0PrdDoc",DocRelSqlPtr,&AllDoctempSO,mfail));
			}
		}
		else if(nlsStrStr(CatiaFileNameS,".CATDrawing")!=NULL)
		{
			t5CheckMfail(QueryWhere("t5DrwDoc",DocRelSqlPtr,&AllDoctempSO,mfail));	
		}
		else if(nlsStrStr(CatiaFileNameS,".asm")!=NULL ||nlsStrStr(CatiaFileNameS,".prt")!=NULL)
		{
			t5CheckMfail(QueryWhere("DesDoc",DocRelSqlPtr,&AllDoctempSO,mfail));	
		}
		else if(nlsStrStr(CatiaFileNameS,".drw")!=NULL)
		{
			//t5CheckMfail(QueryWhere("DrwDoc",DocRelSqlPtr,&AllDoctempSO,mfail));
			t5CheckMfail(QueryWhere("t5DrwDoc",DocRelSqlPtr,&AllDoctempSO,mfail)); //Correction od Class
		}
		else
		{
			printf("\n PARTDOCREL: No match found inside ELSE"); fflush(stdout);
		}

		
		
		printf("\n PARTDOCREL: Total number of docs = [%d]",setSize(AllDoctempSO)); fflush(stdout);
		if (setSize(AllDoctempSO)==0)
		{
			t5CheckDstat(objSetAttribute(PartObjP,t5DrawingIndAttr,"N"));
			t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
		}

		
		for(k=0;k<setSize(AllDoctempSO);k++)
		{
			DocRelObjP=(ObjectPtr)low_set_get(AllDoctempSO,k);
			if(DocObidDupS) nlsStrFree(DocObidDupS);
			//if(ChkPartRelObidDupS) nlsStrFree(ChkPartRelObidDupS);
						
			t5CheckDstat(objGetAttribute(DocRelObjP,ClassAttr,&DocClassDupS));
			DocClassS=nlsStrDup(DocClassDupS);
			t5CheckDstat(objGetAttribute(DocRelObjP,OBIDAttr,&DocObidDupS));
			DocObidS=nlsStrDup(DocObidDupS);
			
		/*	t5CheckDstat(objGetAttribute(PartObjP,OBIDAttr,&ChkPartRelObidDupS));
			ChkPartRelObidS=nlsStrDup(ChkPartRelObidDupS);*/

			RefreshObject(PartObjP,mfail);
			if(!nlsStrCmp("x0PrdDoc",DocClassS))
			{
				printf("\n PARTDOCREL: Inside CatProd doc\n"); fflush(stdout);
				t5CheckDstat(objSetAttribute(PartObjP,t5ModelIndicatorAttr,"Y"));
				t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
				t5CheckDstat(objSetAttribute(PartObjP,t5DrawingIndAttr,"N"));
				t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
				
				RelChkFlag=CheckRelstatus(PartObjP ,DocRelObjP ,"g2PrdDoc",mfail);
			
				printf("\n PARTDOCREL: RelChkFlag=%d\n",RelChkFlag); fflush(stdout);
				if (RelChkFlag==1)
				{
					printf("\n PARTDOCREL: CATProduct Doc of [%s] [%s] is Being  attached to TC Part",DocRevisionS,DocSequenceS); fflush(stdout);
					t5CheckMfail(CreateRelation2(g2PrdDocClass,DocRelDialogObjP,PartObjP,DocRelObjP,&NewDocRelObjP,mfail));
					printf("\n PARTDOCREL: g2PrdDoc Relation Successfully created "); fflush(stdout);
				}
				else
				{	
					printf(" \n PARTDOCREL: CATProduct Doc of [%s] [%s] is already  attached to TC Part \n",DocRevisionS,DocSequenceS); fflush(stdout);
				}										
			}
			else if (!nlsStrCmp("t5DrwDoc",DocClassS))		
			{
				t5CheckDstat(objSetAttribute(PartObjP,t5DrawingIndAttr,"D"));
				t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
				RelChkFlag=CheckRelstatus(PartObjP ,DocRelObjP ,"PartDoc",mfail);	
				printf("\n PARTDOCREL: RelChkFlag = [%d]",RelChkFlag); fflush(stdout);
				if (RelChkFlag==1)
				{
					printf("\n PARTDOCREL: DrwDoc of [%s] [%s] is Being  attached to TC Part",DocRevisionS,DocSequenceS); fflush(stdout);
					t5CheckMfail(CreateRelation2(PartDocClass,DocRelDialogObjP,PartObjP,DocRelObjP,&NewDocRelObjP,mfail));
					printf("\n PARTDOCREL: Part Doc Relation Successfully created"); fflush(stdout);													
				}
				else
				{	
					printf("\n PARTDOCREL: DrwDoc [%s] [%s] is  already attached to TC Part ",DocRevisionS,DocSequenceS); fflush(stdout);											
				}		
				printf("\n PARTDOCREL: Part Doc Relation Successfully created "); fflush(stdout);
			}
			else if (!nlsStrCmp("DesDoc",DocClassS))	
			{
				printf("\n PARTDOCREL: Going to Create Relation B/W Part and DesDoc");fflush(stdout);
				t5CheckDstat(objSetAttribute(PartObjP,t5ModelIndicatorAttr,"Y"));
				t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
				t5CheckDstat(objSetAttribute(PartObjP,t5DrawingIndAttr,"N"));
				t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
			
				RelChkFlag=CheckRelstatus(PartObjP ,DocRelObjP ,"PartDoc",mfail);

				printf("\n PARTDOCREL: Going to Create Relation B/W Part and DesDoc RelChkFlag --- [%d]",RelChkFlag);fflush(stdout);

				if (RelChkFlag==1)
				{
					printf("\n PARTDOCREL: DesDoc of [%s] [%s] is Being  attached to TC Part ",DocRevisionS,DocSequenceS); fflush(stdout);
					t5CheckMfail(CreateRelation2(PartDocClass,DocRelDialogObjP,PartObjP,DocRelObjP,&NewDocRelObjP,mfail));
					printf("\n PARTDOCREL: Part Doc Relation Successfully created "); fflush(stdout);													
				}
				else
				{
					printf("\n PARTDOCREL: DesDoc [%s] [%s] is  already attached to TC Part ",DocRevisionS,DocSequenceS); fflush(stdout);
				}
			}
			else if (!nlsStrCmp("t5MulCad",DocumentClassS) || !nlsStrCmp("t5MulPrd",DocumentClassS))	
			{
				t5CheckDstat(objSetAttribute(PartObjP,t5ModelIndicatorAttr,"Y"));
				t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
				t5CheckDstat(objSetAttribute(PartObjP,t5DrawingIndAttr,"N"));
				t5CheckMfail(t0UpdateObject(&PartObjP,mfail));
			
				RelChkFlag=CheckRelstatus(PartObjP ,DocRelObjP ,"PartDoc",mfail);

				printf("\n PARTDOCREL: Going to Create Relation B/W Part and t5MulCad / t5MulPrd RelChkFlag --- [%d]",RelChkFlag);fflush(stdout);

				if (RelChkFlag==1)
				{
					printf("\n PARTDOCREL: 5MulCad / t5MulPrd of [%s] [%s] is Being  attached to TC Part ",DocRevisionS,DocSequenceS); fflush(stdout);
					t5CheckMfail(CreateRelation2(PartDocClass,DocRelDialogObjP,PartObjP,DocRelObjP,&NewDocRelObjP,mfail));
					printf("\n PARTDOCREL: Part 5MulCad / t5MulPrd Relation Successfully created "); fflush(stdout);													
				}
				else
				{
					printf("\n PARTDOCREL: 5MulCad / t5MulPrd [%s] [%s] is  already attached to TC Part ",DocRevisionS,DocSequenceS); fflush(stdout);
				}
			}
			else
			{
				printf("\n PARTDOCREL: Check the Document Class \n"); fflush(stdout);
			}		
		}

LatestTZUpdate:
			
			printf("\n\n LatestTZUpdate: is Called...  ");fflush(stdout);
			printf("\n LatestTZUpdate:Deleting The Old documents "); fflush(stdout);
			//objDump(PartObjP);
			
			t5CheckMfail(ExpandObject(PartDocClass,PartObjP,"DocumentsDescribingPart",&DelDocSO,&DelDocRelSO,mfail));
			printf("\n LatestTZUpdate: setSize(DelDocSO) = [%d]",setSize(DelDocSO)); fflush(stdout);
			for (delCnt=0;delCnt<setSize(DelDocSO) ;delCnt++ )
			{
				DelDocObjP=setGet(DelDocSO,delCnt);

				t5CheckDstat(objGetAttribute(DelDocObjP,ClassAttr,&DelDocClassDupS));
				DelDocClassS=nlsStrDup(DelDocClassDupS);
				t5CheckDstat(objGetAttribute(DelDocObjP,OBIDAttr,&DelDocObidDupS));
				DelDocObidS=nlsStrDup(DelDocObidDupS);
				printf("\n LatestTZUpdate: DelDocClassS = [%s],DocClassS = [%s],DelDocObidS = [%s],DocObidS = [%s]",DelDocClassS,DocClassS,DelDocObidS,DocObidS); fflush(stdout);
				//Avoiding the MulCad Document  advancing with TC Part of the higher Revision
				if (nlsStrCmp(DelDocClassS,"t5MulCad")==0)
				{
					printf("\n LatestTZUpdate: Inside check for converted document relation");
					t5CheckMfail(ExpandObject(PartDocClass,DelDocObjP,"PartsDescribedByDocument",&MulcadPrtSO,&MulcadPrtRelSO,mfail));
					printf("\n LatestTZUpdate: Number of TC Parts  attached to converted document = [%d]",setSize(MulcadPrtSO));

					if (setSize(MulcadPrtSO)!=1)
					{
						for (mulcnt=0;mulcnt<setSize(MulcadPrtSO) ;mulcnt++ )
						{
							MulCadPrtObjP=setGet(MulcadPrtSO,mulcnt);

							t5CheckDstat(objGetAttribute(MulCadPrtObjP,SupersededAttr,&MulCadPrtSupS));
							if (nlsStrCmp(MulCadPrtSupS,"-")==0)
							{
								printf("\n LatestTZUpdate: I am deleting the duplicate Relation of CatProduct document");
								t5CheckMfail(DeleteRelation2(low_set_get(MulcadPrtRelSO,mulcnt),mfail));
							}							
						}
					}
				}

				if (nlsStrCmp(DelDocClassS,DocClassS)==0 && nlsStrCmp(DelDocObidS,DocObidS)!=0)
				{
					printf("\n LatestTZUpdate: Other  Document except latest of same class are being deleted \n"); fflush(stdout);
					//commenting removed by vineet for deleting Nr 1 Document relation with Nr 2 TC part on 12/04/2016
					t5CheckMfail(DeleteRelation2(low_set_get(DelDocRelSO,delCnt),mfail));
					// vineet end on 12/04/2016
				}
			}
		}
		else
		{
			printf("\n LatestTZUpdate: The Part is superseded. Hence not Creating/deleting the Existing Partdoc Relation \n"); fflush(stdout);
		}
		
		
REFDOCREL:

	printf("\n\n REFDOCREL: is Called...");fflush(stdout);
	printf ("\n REFDOCREL: Inside REF Doc Rel RefDrwName = [%s] RefDrwRev = [%s]  RefDrwSeq = [%s] ",RefDrwName,RefDrwRev,RefDrwSeq);fflush(stdout);
	if (nlsStrCmp(RefDrwName,"NULL")!=0)
	{
		printf("\n REFDOCREL:Querying TC PArt");fflush(stdout);
		t5CheckDstat(oiSqlCreateSelect(&RefPrtsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(RefPrtsqlPtr,PartNumberAttr,RefDrwName));
		t5CheckDstat(oiSqlWhereAND(RefPrtsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(RefPrtsqlPtr,RevisionAttr,RefDrwRev));
		t5CheckDstat(oiSqlWhereAND(RefPrtsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(RefPrtsqlPtr,SequenceAttr,RefDrwSeq));
	
		t5CheckMfail(QueryWhere(PartClass,RefPrtsqlPtr,&RefDrwPartSO,mfail));

		if (setSize(RefDrwPartSO)==0)
		{
			printf(" REFDOCREL :The REF Drw Part is not found ");fflush(stdout);
			goto CLEANUP;
		}
		else
		{				
			RefDrwPartObjP=setGet(RefDrwPartSO,0);

			/*t5CheckDstat(objGetAttribute(RefDrwPartObjP,OBIDAttr,&RefDrwPartObidDupS));
				RefDrwPartObidS=nlsStrDup(RefDrwPartObidDupS);*/
		}

		printf("\n REFDOCREL : Querying Ref Doc \n");fflush(stdout);
		t5CheckDstat(oiSqlCreateSelect(&RefDrwsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(RefDrwsqlPtr,DocumentNameAttr,PartNameS));
		t5CheckDstat(oiSqlWhereAND(RefDrwsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(RefDrwsqlPtr,RevisionAttr,RevisionS));
		t5CheckDstat(oiSqlWhereAND(RefDrwsqlPtr));
		t5CheckDstat(oiSqlWhereEQ(RefDrwsqlPtr,SequenceAttr,SequenceS));
		
		t5CheckMfail(QueryWhere(t5DrwDocClass,RefDrwsqlPtr,&RefDrwDocSO,mfail));
		if (setSize(RefDrwDocSO)==0)
		{
			printf("\n REFDOCREL : The REF Drw Doc  is not found ");fflush(stdout);
			goto CLEANUP;
		}
		else
			{
			RefDrwDocObjP=setGet(RefDrwDocSO,0);
			/*t5CheckDstat(objGetAttribute(RefDrwDocObjP,OBIDAttr,&RefDrwDocObidDupS));
			RefDrwDocObidS=nlsStrDup(RefDrwDocObidDupS);*/
			}
		
			printf("\n REFDOCREL : Checking Relation\n");fflush(stdout);
			
			RelChkFlag=0;
			RelChkFlag=CheckRelstatus(RefDrwPartObjP ,RefDrwDocObjP ,"InfoDwg",mfail);	
			t5CheckDstat(objSetAttribute(RefDrwPartObjP,t5DrawingIndAttr,"D"));
			t5CheckMfail(t0UpdateObject(&RefDrwPartObjP,mfail));
		
		if (RelChkFlag==1)
		{
			printf("\n REFDOCREL : RefDrw REl is being Created");fflush(stdout);
			
			t5CheckMfail(CreateRelation2(InfoDwgClass,DrwDialogObjP,RefDrwPartObjP,RefDrwDocObjP,&NewRefRelObjP,mfail));
		}
		else
		{
			printf("\n REFDOCREL:RefDrw REl exist Already");	fflush(stdout);
		}	
	}
	
	


	CLEANUP:
			printf("\n\n ExCatiaV5Reg Program is in CLEANUP Mode ! ! ! \n");fflush(stdout);
			t5PrintCleanUpModName;
			if (WorkLocSO) t5FreeSetOfObjects(WorkLocSO);
			if (CatiaFileSO) t5FreeSetOfObjects(CatiaFileSO);
			if (CatiaDIsChkSO) t5FreeSetOfObjects(CatiaDIsChkSO);
			if (DIObjRelSO) t5FreeSetOfObjects(DIObjRelSO);
			if (DataPdmItemSO) t5FreeSetOfObjects(DataPdmItemSO);
			if (LatestDocSO) t5FreeSetOfObjects(LatestDocSO);
			if (LatestPartSO) t5FreeSetOfObjects(LatestPartSO);
			if (DocPdmRelSO) t5FreeSetOfObjects(DocPdmRelSO);
			if (DocPdmItemSO) t5FreeSetOfObjects(DocPdmItemSO);
			if (PartMstrSO) t5FreeSetOfObjects(PartMstrSO);
			if (PartMstrRelSO) t5FreeSetOfObjects(PartMstrRelSO);
			if (VisObjSO) t5FreeSetOfObjects(VisObjSO);
			if (VisRelSO) t5FreeSetOfObjects(VisRelSO);
			if (BIObjNxtRevSO) t5FreeSetOfObjects(BIObjNxtRevSO);
			if (BIObjNxtRevRelSO) t5FreeSetOfObjects(BIObjNxtRevRelSO);
			if (BIObjNxtSeqSO) t5FreeSetOfObjects(BIObjNxtSeqSO);
			if (BIObjNxtRelRelSO) t5FreeSetOfObjects(BIObjNxtRelRelSO);
			if (DocDataPdmItemSO) t5FreeSetOfObjects(DocDataPdmItemSO);
			if (DocDIObjRelSO) t5FreeSetOfObjects(DocDIObjRelSO);
			if (AttachByDocItemRelSO) t5FreeSetOfObjects(AttachByDocItemRelSO);
			if (AttachByDocItemSO) t5FreeSetOfObjects(AttachByDocItemSO);
			if (LoadDocRelSqlPtrSO) t5FreeSetOfObjects(LoadDocRelSqlPtrSO);
			if (PrvSupersedObjSO) t5FreeSetOfObjects(PrvSupersedObjSO);
			if (PrvSupersedRelObjSO) t5FreeSetOfObjects(PrvSupersedRelObjSO);
			if (AllDoctempSO) t5FreeSetOfObjects(AllDoctempSO);
			if (ChkPartSO) t5FreeSetOfObjects(ChkPartSO);
			if (ChkPartRelSO) t5FreeSetOfObjects(ChkPartRelSO);
			if (SupersedObjSO) t5FreeSetOfObjects(SupersedObjSO);
			if (extraObj) t5FreeSetOfObjects(extraObj);
			if (RefDrwPartSO) t5FreeSetOfObjects(RefDrwPartSO);
			if (RefDrwDocSO) t5FreeSetOfObjects(RefDrwDocSO);
			if (ChkRefDrwSO) t5FreeSetOfObjects(ChkRefDrwSO);
			if (DelDocRelSO) t5FreeSetOfObjects(DelDocRelSO);
			if (DelDocSO) t5FreeSetOfObjects(DelDocSO);

	
			if (extraStr) t5FreeSetOfStrings(extraStr);
			if (getWorkLoc) t5FreeSqlPtr(getWorkLoc);
			if (getDI) t5FreeSqlPtr(getDI);
			if (DocRelSqlPtr) t5FreeSqlPtr(DocRelSqlPtr);
			if (DocSqlPtr) t5FreeSqlPtr(DocSqlPtr);
			if (PartsqlPtr) t5FreeSqlPtr(PartsqlPtr);
			if (VisSqlPtr) t5FreeSqlPtr(VisSqlPtr);
			if (usrsql3) t5FreeSqlPtr(usrsql3);
			if (!nlsIsStrNull(DiDescS)) t5FreeString(DiDescS); 
			if(fp) fclose(fp);
			if(fpDn) fclose(fpDn);
	
	EXIT:

		t5CheckDstatAndReturn;
	return (*mfail);
}

//------------------------------------------------------------------------------------------------