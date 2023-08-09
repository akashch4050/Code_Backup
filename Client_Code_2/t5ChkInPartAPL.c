/***************************************************************************************************************
* Copyright (c) 2004 Tata Technologies Limited (TTL). All Rights Reserved
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File					:	ExProeMemR.c
*  Created By			:	Naveen Sabapathy
*  Created On			:	Friday, July 08, 2011
*  Project				:	TATA MOTORS - PLM CADBOM (CMI) Catia V4/Catia V5 Data Migration
*  Purpose				:	This code is mainly Register as a V4(.model)/V5 Objects(.CATPart,.CATProduct,.CATDrawing,.CMIArchieve)
*							and check in to Respective Vaults as per the input.
*							Then creates an supersedes relationship
*
*  
*****************************************************************************************************************/


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

int CheckInDataItem(ObjectPtr CatiaDIObjP,string VaultStatusS,string Flag_1,string ProjectCodeS,integer* mfail)
{
	ObjectPtr TranDialogueObjP=NULL;
	SetOfObjects extraObj=NULL;
	string DIOwnerNameS=NULL;
	string DIName=NULL;
	SetOfStrings extraStr=NULL;
	
	t5MethodInit("CheckInDataItem");
	
	t5CheckDstat(objGetAttribute(CatiaDIObjP,RelativePathAttr,&DIName));
	printf("\nCheckIn of Catia File >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> [%s]", DIName); fflush(stdout);
	t5CheckDstat(objGetAttribute(CatiaDIObjP,OwnerNameAttr,&DIOwnerNameS));
	
	printf("\n Owner Name of the Catia File is [%s]", DIOwnerNameS); fflush(stdout);

	if ((nlsStrCmp(DIOwnerNameS,"Release Vault")!=0) && (nlsStrCmp(DIOwnerNameS,"WIP Vault")!=0) && (nlsStrCmp(DIOwnerNameS,"CE Vault")!=0))
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
				//t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsErcRlzd"));
				t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsAplRlzd"));
			}
			else
			{	
				printf("\n Trasnferring it to Non Live Rel Vault "); fflush(stdout);
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"Release Vault"));					
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerDirNameAttr,"Rel_CAD_Vault_Loc"));

				t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));

				//t5CheckDstat(objSetAttribute(CatiaDIObjP,WorkingRelativePathAttr,WorkRelPathS)); // modified to solve 13 digit working rel path issue of WIP vault parts
				if(dstat=t0UpdateObject(&CatiaDIObjP,mfail))goto EXIT;
			}
			//t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsErcRlzd"));
			t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsAplRlzd"));
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
			
			//t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsReview"));
			t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsAPLWrkg"));
			//t5CheckDstat(objSetAttribute(CatiaDIObjP,WorkingRelativePathAttr,WorkRelPathS)); // modified to solve 13 digit working rel path issue of WIP vault parts
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

			//t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsReview"));
			t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsAPLWrkg"));
			printf("\n Refresh Object"); fflush(stdout);
			if(dstat=RefreshObject(CatiaDIObjP,mfail)) goto EXIT;
			printf("\n t0Update Object"); fflush(stdout);
			if(dstat=t0UpdateObject(&CatiaDIObjP,mfail))goto EXIT;
			printf("\n DONE t0Update Object"); fflush(stdout);
		}
		t5CheckDstat(objSetAttribute(CatiaDIObjP,ProjectNameAttr,ProjectCodeS));
		t5CheckDstat(objSetAttribute(CatiaDIObjP,CheckedOutAttr,"-"));
		t5CheckMfail(PerformFreeze(CatiaDIObjP,mfail));
		t5CheckMfail(t0UpdateObject(&CatiaDIObjP,mfail));
	}
	else
	{
		if (((strcmp(VaultStatusS,"Released")==0) && (nlsStrCmp(DIOwnerNameS,"Release Vault")==0)) || ((strcmp(VaultStatusS,"WIP")==0) && ((nlsStrCmp(DIOwnerNameS,"WIP Vault")==0) ||(nlsStrCmp(DIOwnerNameS,"CE Vault")==0))))			// (strcmp(VaultStatusS,"CE")==0) && (nlsStrCmp(DIOwnerNameS,"CE Vault")==0))
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
			if (strcmp(VaultStatusS,"WIP")==0 && nlsStrCmp(Flag_1,"1") == 0)
			{
				t5CheckDstat(objSetAttribute(TranDialogueObjP,DestOwnerNameAttr,"CE Vault"));
			}

			t5CheckDstat(objSetAttribute(TranDialogueObjP,ZBlobAttr,"-"));
			t5CheckMfail(TransferObject(CatiaDIObjP,TranDialogueObjP,mfail));

			if (strcmp(VaultStatusS,"Released")==0)
				t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsAplRlzd"));

			if (strcmp(VaultStatusS,"WIP")==0 || strcmp(VaultStatusS,"CE")==0)
				t5CheckDstat(objSetAttribute(CatiaDIObjP,LifeCycleStateAttr, "LcsAPLWrkg"));

			printf("\n After TransferObject Dialog \n"); fflush(stdout);
		}
	} 

	CLEANUP:

	EXIT:
	t5CheckDstatAndReturn;
	return (*mfail);


}
int CheckInRecR(ObjectPtr PartPtr,string RelStatus,ObjectPtr genContextObj,string Flag_1,integer* mfail)
{
	SetOfObjects soPrtsObjs = NULL;
	SetOfObjects soprtRelObj = NULL;
	SetOfObjects allDocsSO = NULL;
	SetOfObjects docRelObjs = NULL;
	SetOfObjects convCad1SO = NULL;
	int PrtChkInCnt=0;
	int PrtCount=0;
	int j=0;
	int k=0;
	int stat=0;
	ObjectPtr ChildPart= NULL;
	ObjectPtr allDocsObj= NULL;
	ObjectPtr DataItemPtr= NULL;
	string PrtNos= NULL;
	string PrtRev= NULL;
	string PrtSeq= NULL;
	string PrtOwner= NULL;
	string PrtOwnerDup= NULL;
	string CheckedOutDupS= NULL;
	string CheckedOutS= NULL;
	string DIOwnerNameS= NULL;
	string ProjectCodeS= NULL;
	string DocOwnerNameS= NULL;
	string PrtOwnerNameS= NULL;
	t5MethodInit("CheckInRecR");

	if(dstat=ExpandRelationWithCtxt("AsRevRev",
	PartPtr,
	"PartsInAssembly",
	genContextObj,
	SC_SCOPE_OF_SESSION,
	NULL,
	&soPrtsObjs,
	&soprtRelObj,
	mfail))  ;
	printf("\n Nos of child part %d ", setSize(soPrtsObjs)); fflush(stdout);
	for (PrtCount=0;PrtCount<setSize(soPrtsObjs);PrtCount++)
	{
		ChildPart=setGet(soPrtsObjs,PrtCount);
		if(dstat=objGetAttribute(ChildPart,PartNumberAttr,&PrtNos));
		if(dstat=objGetAttribute(ChildPart,RevisionAttr,&PrtRev));
		if(dstat=objGetAttribute(ChildPart,SequenceAttr,&PrtSeq));
		if(dstat=objGetAttribute(ChildPart,OwnerNameAttr,&PrtOwner));
		if(!nlsIsStrNull(PrtOwner)) PrtOwnerDup = nlsStrDup(PrtOwner);

		if (nlsStrStr(PrtOwnerDup,"Vault"))
		{
			PrtChkInCnt++;
		}
		else
		{
			printf("\n >>>>>>>>>>>>>>>>>>>> Going to Check iN child part %s  %s %s  %s", PrtNos,PrtRev,PrtSeq,PrtOwnerDup); fflush(stdout);
			CheckInRecR(ChildPart,RelStatus,genContextObj,Flag_1,mfail);
		}
	}
	if(dstat = ExpandObject2(PartDocClass, PartPtr,"DocumentsDescribingPart",SC_SCOPE_OF_SESSION,&allDocsSO,&docRelObjs,mfail)) goto CLEANUP;
	printf("\n  all the child part are chechecked iN. Now going to check i n main tc part \n"); fflush(stdout);
	if(dstat=objGetAttribute(PartPtr,ProjectNameAttr,&ProjectCodeS));
	for(j=0; j<setSize(allDocsSO); j++ ) 
	{
		allDocsObj=setGet(allDocsSO,j);
		objGetAttribute(allDocsObj,CheckedOutAttr,&CheckedOutDupS);
		CheckedOutS=nlsStrDup(CheckedOutDupS);
		t5CheckDstat(objGetAttribute(allDocsObj,OwnerNameAttr,&DocOwnerNameS));
		printf("\n Owner Name of the document is [%s]", DocOwnerNameS); fflush(stdout);
		if (nlsStrCmp(CheckedOutS,"+") == 0 || !nlsStrStr(DocOwnerNameS,"Vault"))
		{
			if (nlsStrCmp(Flag_1,"1") == 0)
			{	
				printf("\n LoadDoc Flag_1 ################ = [%s] ",Flag_1);	
				t5CheckMfail(CheckInItem(allDocsObj,"CE Vault", mfail));
			}
			else
			{		
				t5CheckMfail(CheckInItem(allDocsObj,"WIP Vault", mfail));	
			}
		}
		
		t5CheckDstat(ExpandObject4(AttachClass,allDocsObj, "DataItemsAttachedToBusItem", &convCad1SO, mfail));
		for(k=0; k<setSize(convCad1SO); k++ ) 
		{
			DataItemPtr=setGet(convCad1SO,k);
			t5CheckDstat(objGetAttribute(DataItemPtr,OwnerNameAttr,&DIOwnerNameS));
			printf("\n CheckInRecR Owner Name of the Catia File is [%s]", DIOwnerNameS); fflush(stdout);
			if ((nlsStrCmp(DIOwnerNameS,"Release Vault")!=0) && (nlsStrCmp(DIOwnerNameS,"WIP Vault")!=0) && (nlsStrCmp(DIOwnerNameS,"CE Vault")!=0)) //NOTCHECKIN
			{
				CheckInDataItem( DataItemPtr,RelStatus,Flag_1,ProjectCodeS,mfail);
			}

		}
	}

	t5CheckDstat(objGetAttribute(PartPtr,OwnerNameAttr,&PrtOwnerNameS));
	printf("\n In DB PrtOwnerNameS = [%s]",PrtOwnerNameS);fflush(stdout);
	if (nlsStrStr(PrtOwnerNameS,"Vault")==NULL)
	{
		if (nlsStrCmp(Flag_1,"1") == 0)
		{	
			printf("\n Load Part Flag_1 ################ = [%s] ",Flag_1);	
			t5CheckMfail(CheckInItem(PartPtr,"CE Vault", mfail));
		}
		else
		{		
			t5CheckMfail(CheckInItem(PartPtr,"WIP Vault", mfail));	
		}

		t5CheckDstat(ChangeStateItem(PartPtr,"LcsAPLWrkg",mfail));
		if(dstat = t5CheckInCstAndMstrObject(PartPtr,mfail))
		{
				printf("\n Error in transfering the Part master and cost. Please check");fflush(stdout);
		}
	}
	CLEANUP:
	EXIT:
	t5CheckDstatAndReturn;
	return (*mfail);

};

int main (int argc, char *argv[])
{
	int stat=0,j=0;
	string LoginS = NULL ;
	string PasswordS = NULL ;
	string RelStatus = NULL;
	string Flag_1 = NULL;
	string PartNumerAsm = NULL;
	string PartNosAsmRev = NULL;
	string PartNosAsmSeq = NULL;

	string docClassNameS = NULL;
	string docClassNameDupS = NULL;
	string CadAsmClass = NULL;
	string CadCmpClass = NULL;
	string PartNosCmp = NULL;
	string PartNosRev = NULL;
	string PartNosSeq = NULL;

	SetOfObjects LeftObjSO=NULL;
	SetOfObjects RightObjSO=NULL;
	SetOfObjects RelnObjSO=NULL;
	SetOfObjects PartAsmSo=NULL;
	SetOfObjects PartCmpSo=NULL;
	SetOfObjects allDocsSO=NULL;
	SetOfObjects docRelObjs=NULL;


	ObjectPtr genContextObj= NULL;
	ObjectPtr CtxtObjTmp= NULL;
	ObjectPtr LeftObjP = NULL ;
	ObjectPtr RightObjP = NULL ;
	ObjectPtr NewRObjP = NULL ;
	ObjectPtr TCPartObj = NULL ;
	ObjectPtr PartPtr = NULL ;


	ObjectPtr allDocsObj = NULL;
	SetOfObjects convCad1SO = NULL;

	ObjectPtr 		allDocsObj1 = NULL;
	SetOfObjects		convCad2SO = NULL;
	SetOfObjects		allDocsSO1 = NULL;

//	SqlPtr LeftSqlPtr = NULL ;
//	SqlPtr RightSqlPtr = NULL ;
//	SqlPtr RelnSqlPtr = NULL ;
	SqlPtr SqlPtrAsm = NULL ;
//	SqlPtr SqlPtrCmp = NULL ;

	FILE	*Prob=NULL;

	t5MethodInitWMD("ChkInR");
		/* enable multibyte features of Metaphase */
	t5CheckDstat(clInitMB2 (argc, &argv, NULL));

	/* Check to see if the Metaphase network is available. If not, set dstat.*/
	t5CheckDstat(clTestNetwork ());


	/*  Initialize the command line session.    */
		t5CheckDstat(clInitialize2 (FALSE));




	LoginS=nlsStrAlloc(nlsStrLen(argv[1])+1);
	PasswordS=nlsStrAlloc(nlsStrLen(argv[2])+1);
	RelStatus=nlsStrAlloc(nlsStrLen(argv[3])+1);
	PartNumerAsm=nlsStrAlloc(nlsStrLen(argv[4])+1);
	PartNosAsmRev=nlsStrAlloc(nlsStrLen(argv[5])+1);
	PartNosAsmSeq=nlsStrAlloc(nlsStrLen(argv[6])+1);
	Flag_1=nlsStrAlloc(nlsStrLen(argv[7])+1);
	
	//RightRelnOwnS=nlsStrAlloc(nlsStrLen(argv[11])+1);

	LoginS=argv[1];
	PasswordS=argv[2];
	RelStatus=argv[3];
	PartNumerAsm=argv[4];
	PartNosAsmRev=argv[5];
	PartNosAsmSeq=argv[6];
	Flag_1=argv[7];

	t5CheckDstat(clLogin2 (LoginS,PasswordS,&stat));
	if (stat!=OKAY)
	{
		printf("\n Invalid User Name or PasswordS : %s,%s \n", LoginS, PasswordS);  fflush(stdout);
		//fprintf(fp4,"\n Invalid User Name or PasswordS : %s,%s \n", LoginS, PasswordS);  
		goto EXIT;
	}


		int			ii = 0;string		docobjst	= NULL;
	SetOfStrings dbScp	=          NULL;
	t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScp,mfail));
	for (ii=0;ii<setSize(dbScp) ; ii++)
	{

		docobjst=low_set_get(dbScp,ii);
		printf("\n DB pref check before... :%s\n",docobjst);fflush(stdout);
	}
	

	low_set_add_str(dbScp, "supprod");
	low_set_add_str(dbScp, "suhprod");
	//low_set_add_str(dbScp, "sujprod");

	t5CheckDstat(SetDatabaseScope(PdmSessnClass,dbScp,mfail));
	for (ii=0;ii<low_set_size(dbScp) ; ii++)
	{
		docobjst=low_set_get(dbScp,ii);
		printf("\n DB pref check -after... :%s\n",docobjst);fflush(stdout);
	}	

	Prob=fopen("RelationProblem.txt","w");
	fflush(Prob);

	printf("\n Before left Query && Flag_1 = [%s]\n",Flag_1); fflush(stdout);
	if (nlsStrCmp(PartNosAsmRev,"-")==0 && nlsStrCmp(PartNosAsmSeq,"-")==0)
	{
		t5CheckDstat(oiSqlCreateSelect(&SqlPtrAsm));
		t5CheckDstat(oiSqlWhereEQ(SqlPtrAsm,PartNumberAttr,PartNumerAsm));
		t5CheckDstat(oiSqlWhereAND(SqlPtrAsm));
		t5CheckDstat(oiSqlWhereEQ(SqlPtrAsm,SupersededAttr,"-"));
		t5CheckMfail(QueryWhere("Part",SqlPtrAsm,&PartAsmSo,mfail));
		oiSqlPrint(SqlPtrAsm);
	}
	else
	{
		t5CheckDstat(oiSqlCreateSelect(&SqlPtrAsm));
		t5CheckDstat(oiSqlWhereEQ(SqlPtrAsm,PartNumberAttr,PartNumerAsm));
		t5CheckDstat(oiSqlWhereAND(SqlPtrAsm));
		t5CheckDstat(oiSqlWhereEQ(SqlPtrAsm,RevisionAttr,PartNosAsmRev));
		t5CheckDstat(oiSqlWhereAND(SqlPtrAsm));
		t5CheckDstat(oiSqlWhereEQ(SqlPtrAsm,SequenceAttr,PartNosAsmSeq));
		t5CheckMfail(QueryWhere("Part",SqlPtrAsm,&PartAsmSo,mfail));
		oiSqlPrint(SqlPtrAsm);
	}
	printf("\n PartNum %s %s %s SetSize(PartAsmSo) = [%d]\n",PartNumerAsm,PartNosAsmRev,PartNosAsmSeq,setSize(PartAsmSo)); fflush(stdout);
	if (setSize(PartAsmSo)>0)
	{
		PartPtr=setGet(PartAsmSo,0);
		if(dstat=IntSetUpContext(objClass(PartPtr),PartPtr,&genContextObj,mfail)) ;
		if(dstat=GetCfgCtxtObjFromCtxt(DSesGnCClass,genContextObj,&CtxtObjTmp,mfail));
		if(dstat=SetNavigateViewPref(CtxtObjTmp,TRUE,"EAS","APL",mfail));
		if(dstat=objSetAttribute(CtxtObjTmp,PsmExpIncludeZeroQtyAttr,"+"));
		if(dstat=objSetAttribute(CtxtObjTmp,CfgItemIdAttr,"GlobalCtxt"));
		if(dstat=objSetObject(genContextObj,ConfigCtxtBlobAttr,CtxtObjTmp));
		if(dstat=SetExpOnRevInCtxt(DSesGnCClass,genContextObj,"PscLastRev",mfail));
		CheckInRecR(PartPtr,RelStatus,genContextObj,Flag_1,mfail);		
	}
	else
	{
	}

CLEANUP:

	EXIT:
	fclose(Prob);

	t5CheckDstatAndReturn;
	return (*mfail);
}

//------------------------------------------------------------------------------------------------
