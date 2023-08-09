/*************************************************************************
* Copyright (c) 2004 Tata Technologies Limited (TTL). All Rights
* Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
* Program Name	  : t5TrnsfrToVault.c
* Author		  : Vineet Pardeshi
* Created on	  : 21st April 2016
* Project         : TATA MOTORS - PLM CADBOM
* Methods Defined : Main
*PURPOSE:
* To Transfer Superseded false TC Parts with all attached superseded false Bi documents and 4D Documents
* Modification History :
* S.No    Date        CR No      Modified By            Modification Notes
*
******************************************************************************/

#include <pdmc.h>
#include <msgapc.h>
#include <ft.h>
#include <psm.h>
#include <pdmroot.h>
#include <msgomf.h>
#include <mutproto.h>
#include <msg.h>
#include <meta.h>
#include <mserv.h>
#include <Mtimsg.h>
#include <serv.h>
#include <sys.h>
#include <cl.h>
#include <nls.h>
#include <ui.h>
#include <usc.h>
#include <cfg.h>
#include <relation.h>
#include <msgtel.h>
#include <tel.h>
#include <mqueue.h>      
#include <unistd.h>        
#include <errno.h>      
#include <fcntl.h>        
#include <stdlib.h>        
#include <stdio.h>
status t5CheckInCstAndMstrObject(ObjectPtr PrtCstMstrObj,integer *mfail);
int main(int argc, char *argv[])
{	
	
	int stat = 0;
	int j=0;
	//int jj=0;
	//int jjj=0;
	string LoginS = NULL ;
	string PasswordS = NULL ;
	string partNumberS = NULL ;
	string revisionS = NULL ;
	string sequenceS = NULL ; 
	string JTRelPathDup = NULL ;
	string JTRelPath = NULL ;
	string DIRelPathDup = NULL ;
	string DIRelPath = NULL ;
	//SetOfObjects convDocSO = NULL;
	//SetOfObjects convCadSO = NULL;
	SetOfObjects convJTSO = NULL;
	SetOfObjects allDocsSO = NULL;
	SetOfObjects TCPartSO = NULL;
	SetOfObjects convCad1SO = NULL;
	SetOfObjects latestRelDocs = NULL;
	SetOfObjects latestDocs = NULL;
	SetOfObjects docRelObjs = NULL;
	SetOfObjects	UsesPartsForColSO		    = NULL;
	SetOfObjects	UsesRelPartsForColSO	     = NULL;
	SetOfObjects	CmponentObjSO	     = NULL;
	//SetOfObjects DdocRelObjs = NULL;
	//SetOfObjects DFMEADocsSO = NULL;
	//SetOfObjects DataItemDfmeaSO = NULL;
	//SetOfObjects DataItemDADSO = NULL;
	//SetOfObjects DADdocsSO = NULL;
	//SetOfObjects DADdocRelSO = NULL;
	//ObjectPtr convDocObj = NULL;
	//ObjectPtr convCadObj = NULL;
	//ObjectPtr DFMEADocsObj = NULL;
	//ObjectPtr DADdocsObj = NULL;
	ObjectPtr convJTObj = NULL;
	ObjectPtr allDocsObj = NULL;
	ObjectPtr TCPartObj = NULL;
	ObjectPtr convCad1Obj = NULL;
	ObjectPtr genContextObjOP = NULL;
	ObjectPtr contextObjOP = NULL;
	//ObjectPtr DataItemDfmeaObj = NULL;
	//ObjectPtr DataItemDADObj = NULL;
	SqlPtr convDocsql = NULL ;
	SqlPtr CompSqlPtr = NULL ;
	string		docClassNameS = NULL ;
	string		docClassNameDupS = NULL;
	string		assyOBIDDup = NULL ;
	string		assyOBID = NULL;
	//string		DfmeadocClassNameS = NULL ;
	//string		DfmeadocClassNameDupS = NULL ;
	//string		DADdocClassNameS = NULL ;
	//string		DADdocClassNameDupS = NULL ;
	string		PartSupersededS = NULL ;
	string		PartSupersededDupS = NULL;
	string		TCPartOwnerS = NULL ;
	string		TCPartOwnerDupS = NULL;
	ObjectPtr	TrndialogObj	= NULL;
	//ObjectPtr	TrndialogObj1	= NULL;
	ObjectPtr	TrndialogObj2	= NULL;
	ObjectPtr	TrndialogObj3	= NULL;
	ObjectPtr	TrndialogObj4	= NULL;
	ObjectPtr	TrndialogObj5	= NULL;
	ObjectPtr	ChildPartRelPtr	= NULL;
	ObjectPtr	RightPartObjP	= NULL;
//	ObjectPtr	TrndialogObj6	= NULL;
//	ObjectPtr	TrndialogObj7	= NULL;
	ObjectPtr	PartObjMsPtr	= NULL;
	string		PartOwnerNameS		= NULL;
	string		PartDirOwnerNameS	= NULL;
	string		PartOwnerNameS_CE		= NULL;
	string		PartDirOwnerNameS_CE	= NULL;
	string		ToVaultS			= NULL;
	string		fromVault			= NULL;
	string		docOwnerDupS = NULL;
	string		docOwnerS = NULL;
	string		TCCompRevisionDupS = NULL;
	string		TCCompRevisionS = NULL;
	string		TCCompSequenceDupS = NULL;
	string		TCCompSequenceS = NULL;
	//string		DfmeadocOwnerS = NULL;
	//string		DfmeadocOwnerDupS = NULL;
	//string		DADdocOwnerS = NULL;
	//string		DADdocOwnerDupS = NULL;
	string		DIVaultDup = NULL;
	string		DIVault = NULL;
	string		JtVaultDup = NULL;
	string		JtVault = NULL;
	SetOfObjects extraObj	= NULL;
	SetOfStrings extraStr	= NULL;
	string		timestamp	= NULL;
	string		ProjCode	= NULL;
	string		docNameS = NULL ;
	string		docNameDupS = NULL ;
	string		TCComponentDupS = NULL ;
	string		TCComponentS = NULL ;
	//string		DfmeadocNameS = NULL ;
	//string		DfmeadocNameDupS = NULL ;
	//string		DADdocNameS = NULL ;
	//string		DADdocNameDupS = NULL ;
	//string		DfmeaDIRelPathDup = NULL ;
	//string		DfmeaDIRelPath = NULL ;
	//string		DfmeaDIVaultDup = NULL ;
	//string		DfmeaDIVault = NULL ;

	//string		DADDIRelPathDup = NULL ;
	//string		DADDIRelPath = NULL ;
	//string		DADDIVaultDup = NULL ;
	//string		DADDIVault = NULL ;
	//boolean     found  = FALSE;
	//char		*myframe1=NULL;
	//char		*myframe2=NULL;
	FILE		*fpDI	= NULL;
	int			sizeOfObjects = 0,jtFlag=0;
	string		docobjst	= NULL;
	int			ii = 0,RelFlag=0,ij = 0;
	int x = 0;
	//int xx = 0;
	//int xxx = 0;
	SetOfStrings dbScp	=          NULL;

	t5MethodInitWMD("t5TrnsfrToVault");

	LoginS=nlsStrAlloc(50);
	PasswordS=nlsStrAlloc(50);
	partNumberS=nlsStrAlloc(20);
	revisionS=nlsStrAlloc(20);
	sequenceS=nlsStrAlloc(100);
	ToVaultS = nlsStrAlloc(100);
	fromVault = nlsStrAlloc(100);
	PartOwnerNameS = nlsStrAlloc(100);
	PartDirOwnerNameS = nlsStrAlloc(100);
	PartOwnerNameS_CE = nlsStrAlloc(100);
	PartDirOwnerNameS_CE = nlsStrAlloc(100);

	LoginS=argv[1];
	PasswordS=argv[2];
	partNumberS=argv[3];
	revisionS=argv[4];
	sequenceS=argv[5];
	ToVaultS=argv[6];
	fromVault=argv[7];

	


	t5CheckDstat(clInitMB2 (argc, &argv, NULL));
	t5CheckDstat(clTestNetwork ());
	t5CheckDstat(clInitialize2 (FALSE));
	t5CheckDstat(clLogin2 (LoginS,PasswordS,&stat));
	if (stat!=OKAY)
	{
		printf("\n Invalid User Name or PasswordS : %s,%s \n", LoginS, PasswordS);  fflush(stdout);
		goto CLEANUP;
	}

	t5CheckDstat(GetDatabaseScope(PdmSessnClass,&dbScp,mfail));
	for (ii=0;ii<setSize(dbScp) ; ii++)
	{

		docobjst=low_set_get(dbScp,ii);
		printf("\n DB pref check before... :%s\n",docobjst);fflush(stdout);
	}
	

	low_set_add_str(dbScp, "supprod");
	//low_set_add_str(dbScp, "suhprod");
	//low_set_add_str(dbScp, "sujprod");
	//low_set_add_str(dbScp, "sulprod");

	t5CheckDstat(SetDatabaseScope(PdmSessnClass,dbScp,mfail));
	for (ii=0;ii<low_set_size(dbScp) ; ii++)
	{
		docobjst=low_set_get(dbScp,ii);
		printf("\n DB pref check -after... :%s\n",docobjst);fflush(stdout);
	}		
	//Trim White Spaces
	low_trimb(partNumberS);
	low_trimb(revisionS);
	low_trimb(sequenceS);


	fpDI=fopen("Transfer_Log.txt","a");
	fflush(fpDI);

	//Querying statement for the input part
	t5CheckDstat(oiSqlCreateSelect(&convDocsql));
	t5CheckDstat(oiSqlWhereEQ(convDocsql,PartNumberAttr,partNumberS));
	t5CheckDstat(oiSqlWhereAND(convDocsql));
	t5CheckDstat(oiSqlWhereEQ(convDocsql,RevisionAttr,revisionS));
	t5CheckDstat(oiSqlWhereAND(convDocsql));
	t5CheckDstat(oiSqlWhereEQ(convDocsql,SequenceAttr,sequenceS));

	//printf("t5DI2Rel %s &&",DIRelPath);fflush(stdout);
	//Printing Sql Statement
	oiSqlPrint(convDocsql);	

	//Querying the data
	t5CheckDstat(QueryWhere3(PartClass,convDocsql,0,SC_SCOPE_OF_SESSION ,&TCPartSO,mfail)) ;
	if(*mfail) goto CLEANUP;

	printf("\n total TC part found %d \n",setSize(TCPartSO));fflush(stdout);

	fprintf(fpDI,"%s,",partNumberS);
	fprintf(fpDI,"%s,",revisionS);
	fprintf(fpDI,"%s,",sequenceS);

	//fclose(fpDI);

	if(nlsStrStr(ToVaultS,"Released")!=NULL)
	{
		t5CheckDstat(objGetAttribute(low_set_get(TCPartSO,0),ProjectNameAttr,&ProjCode));
		printf("\n vault location: Rel_Vault_Loc (Release Vault for Live Projects) \n");fflush(stdout);
		nlsStrCpy(PartOwnerNameS,"Release Vault");
		nlsStrCpy(PartDirOwnerNameS,"Rel_Vault_Loc");

		nlsStrCpy(PartOwnerNameS_CE,"CE Vault");
		nlsStrCpy(PartDirOwnerNameS_CE,"CE_Vault_Loc");
		jtFlag=1;
		if(nlsStrStr(fromVault,"Release")!=NULL)
		{
			RelFlag=1;
		}
	}
	else if(nlsStrCmp(ToVaultS,"CE Vault")==0)
	{
		
		nlsStrCpy(PartOwnerNameS_CE,"CE Vault");
		nlsStrCpy(PartDirOwnerNameS_CE,"CE_Vault_Loc");
		printf("\n vault location: CE_Vault_Loc \n");fflush(stdout);
		nlsStrCpy(PartOwnerNameS,"CE Vault");
		nlsStrCpy(PartDirOwnerNameS,"CE_Vault_Loc");
		jtFlag=1;
	}
	else if(nlsStrCmp(ToVaultS,"WIP Vault")==0)
	{
		printf("\n vault location: WIP_CAD_Vault_Loc \n");fflush(stdout);
		nlsStrCpy(PartOwnerNameS,"WIP Vault");
		nlsStrCpy(PartDirOwnerNameS,"WIP_CAD_Vault_Loc");
		jtFlag=0;
	}
	else if(nlsStrCmp(ToVaultS,"HNJ WIP Vault")==0)
	{
		printf("\n vault location: HNJ_WIP_Vault_Loc \n");fflush(stdout);
		nlsStrCpy(PartOwnerNameS,"HNJ WIP Vault");
		nlsStrCpy(PartDirOwnerNameS,"HNJ_WIP_Vault_Loc");
		jtFlag=0;
	}
	else if(nlsStrCmp(ToVaultS,"HNJ CE Vault")==0)
	{
		printf("\n vault location: HNJ_CE_Vault_Loc \n");fflush(stdout);
		nlsStrCpy(PartOwnerNameS,"HNJ CE Vault");
		nlsStrCpy(PartDirOwnerNameS,"HNJ_CE_Vault_Loc");
		jtFlag=1;
		nlsStrCpy(PartOwnerNameS_CE,"HNJ CE Vault");
		nlsStrCpy(PartDirOwnerNameS_CE,"HNJ_CE_Vault_Loc");
	}
	else if(nlsStrCmp(ToVaultS,"JSR WIP Vault")==0)
	{
		printf("\n vault location: JSR_WIP_Vault_Loc \n");fflush(stdout);
		nlsStrCpy(PartOwnerNameS,"JSR WIP Vault");
		nlsStrCpy(PartDirOwnerNameS,"JSR_WIP_Vault_Loc");
		jtFlag=0;
	}
	else if(nlsStrCmp(ToVaultS,"JSR CE Vault")==0)
	{
		printf("\n vault location: JSR_CE_Vault_Loc \n");fflush(stdout);
		nlsStrCpy(PartOwnerNameS,"JSR CE Vault");
		nlsStrCpy(PartDirOwnerNameS,"JSR_CE_Vault_Loc");
		jtFlag=1;
		nlsStrCpy(PartOwnerNameS_CE,"JSR CE Vault");
		nlsStrCpy(PartDirOwnerNameS_CE,"JSR_CE_Vault_Loc");
	}
	else if(nlsStrCmp(ToVaultS,"LKN CE Vault")==0)
	{
		printf("\n vault location: LKN_CE_Vault_Loc \n");fflush(stdout);
		nlsStrCpy(PartOwnerNameS,"LKN CE Vault");
		nlsStrCpy(PartDirOwnerNameS,"LKN_CE_Vault_Loc");
		jtFlag=1;
		nlsStrCpy(PartOwnerNameS_CE,"LKN CE Vault");
		nlsStrCpy(PartDirOwnerNameS_CE,"LKN_CE_Vault_Loc");
	}
	

	printf("check 1");fflush(stdout);
	sizeOfObjects = setSize(TCPartSO);	
	if(TCPartSO)
	{
		TCPartObj = low_set_get(TCPartSO,0);
		//printf("\nFound TC Part Obj");fflush(stdout);
		// Applying Context here
//		t5CheckMfail(SetUpContext(objClass(TCPartObj),TCPartObj,&genContextObjOP,mfail));
//		t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass,genContextObjOP,&contextObjOP, mfail));
//		t5CheckDstat(objSetAttribute(contextObjOP,NavigateViewBitPosAttr,"0"));
//		t5CheckDstat(objSetAttribute(contextObjOP,NavigateViewNetworkAttr,"EAS")); 
//		t5CheckDstat(objSetAttribute(contextObjOP,NavigateViewNameAttr, "ERC"));
//		t5CheckDstat(objSetAttribute(contextObjOP,ExpandOnRevisionAttr,"PscLastRev"));
//		t5CheckDstat(objSetAttribute(contextObjOP,PsmExpIncludeZeroQtyAttr,"-"));
//		t5CheckDstat(objSetAttribute(contextObjOP,CfgItemIdAttr,"GlobalCtxt"));
//
//		t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsSTDRlzd","+"));
//		t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsSTDWrkg","+"));
//		t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsAplRlzd","+"));
//		t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsAPLWrkg","+"));
//		t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsErcRlzd","+"));
//		t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsReview","+"));
//		t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsWorking","+"));
//		t5CheckDstat(objSetObject(genContextObjOP,ConfigCtxtBlobAttr,contextObjOP));
//
//		if (dstat = ExpandRelationWithCtxt("AsRevRev",TCPartObj,"PartsInAssembly",genContextObjOP,SC_SCOPE_OF_SESSION,NULL,&UsesPartsForColSO,&UsesRelPartsForColSO,mfail)) goto CLEANUP;
//		if (*mfail) goto CLEANUP;
////	if (dstat = ExpandRelationWithCtxt("AsRevRev",thisObj,"PartsInAssembly",GenContxtObjP,SC_SCOPE_OF_SESSION,NULL,&FirstLvlItemObjs,&FirstLvlRelObjs,mfail)) goto CLEANUP;
//
////if (dstat = ExpandRelationWithCtxt("AsRevRev",setGet(FirstLvlItemObjs,iz),"PartsInAssembly",GenContxtObjP,SC_SCOPE_OF_SESSION,NULL,&FirstLvlItem1Objs,&FirstLvlRel1Objs,mfail)) goto CLEANUP;
//
//		printf("\n setSize(UsesPartsForColSO)  ---->%d\n",setSize(UsesPartsForColSO) );fflush(stdout);
//		printf("\n setSize(UsesRelPartsForColSO)  ---->%d\n",setSize(UsesRelPartsForColSO) );fflush(stdout);
//		for(ij=0;ij<setSize(UsesRelPartsForColSO);ij++)
//		{
//			ChildPartRelPtr=setGet(UsesRelPartsForColSO,ij);
//			t5CheckDstat(objGetAttribute(ChildPartRelPtr,RightAttr,&assyOBIDDup));
//			if(!nlsIsStrNull(assyOBIDDup)) assyOBID = nlsStrDup(assyOBIDDup);
//			printf("\nassyOBID=%s",assyOBID);fflush(stdout);
//			t5CheckDstat(oiSqlCreateSelect(&CompSqlPtr));
//			t5CheckDstat(oiSqlWhereEQ(CompSqlPtr,OBIDAttr,assyOBID));
//			t5CheckMfail(QueryWhere(PartClass,CompSqlPtr,&CmponentObjSO,mfail));
//			if(setSize(CmponentObjSO)==0) goto EXIT;
//			printf ("\n Querying the Component is completed ..... \n");
//			{
//			RightPartObjP = (ObjectPtr)low_set_get(CmponentObjSO,0);
//			t5CheckDstat(objGetAttribute(RightPartObjP,PartNumberAttr,&TCComponentDupS));
//			TCComponentS=nlsStrDup(TCComponentDupS);
//			printf("\nTCComponentS=%s",TCComponentS);fflush(stdout);
//			t5CheckDstat(objGetAttribute(RightPartObjP,RevisionAttr,&TCCompRevisionDupS));
//			TCCompRevisionS=nlsStrDup(TCCompRevisionDupS);
//			printf("\nTCCompRevisionS=%s",TCCompRevisionS);fflush(stdout);
//			t5CheckDstat(objGetAttribute(RightPartObjP,SequenceAttr,&TCCompSequenceDupS));
//			TCCompSequenceS=nlsStrDup(TCCompSequenceDupS);
//			printf("\nTCCompSequenceS=%s",TCCompSequenceS);fflush(stdout);
//			}
//			goto EXIT;
//		t5CheckDstat(objGetAttribute(TCPartObj,SupersededAttr,&PartSupersededS));
//		if(PartSupersededS) PartSupersededDupS = nlsStrDup(PartSupersededS);
//		printf("\nPartSupersededDupS -->%s",PartSupersededDupS);fflush(stdout);
//		t5CheckDstat(objGetAttribute(TCPartObj,OwnerNameAttr,&TCPartOwnerS));
//		if(TCPartOwnerS) TCPartOwnerDupS = nlsStrDup(TCPartOwnerS);
//		if (nlsStrCmp(TCPartOwnerDupS,"CE Vault")==0)
//		{

		if(RelFlag!=1)
		{
			if(dstat=SetUpDialog(PartClass,TCPartObj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj4,mfail));
			if(dstat=objSetAttribute(TrndialogObj4,DestOwnerNameAttr,PartOwnerNameS));
			if(dstat=objSetAttribute(TrndialogObj4,IncludeAttachedDIsAttr,"-"));	
			printf("\n\t ****Started with Part transfer to %s****\n",PartOwnerNameS);fflush(stdout);
			timestamp=sysGetCTime(); 
			printf("\n Start Time is \t-- %s Part nos %s rev %s seq %s \n",timestamp,partNumberS,revisionS,sequenceS);fflush(stdout);
			if(dstat=TransferObject(TCPartObj,TrndialogObj4,mfail)) goto CLEANUP;
			//1.Vineet for TC part transfer error 
			if(*mfail)
			{
				//If transfer TC fails.
				printf("TRANSFER of TC part failed while trasnsferring to %s",PartOwnerNameS);
				fprintf(fpDI,"%s,",partNumberS);
				//goto CLEANUP;
			}
			else
			{
			printf("\n\t ****Done with Part transfer****\n");fflush(stdout);
			timestamp=sysGetCTime(); 
			
			printf("\n Setting Life Cycle state of TC part \n\n\n"); fflush(stdout);
			//if(dstat=objSetAttribute(TCPartObj,LifeCycleStateAttr,"LcsErcRlzd")) goto EXIT;
			//if(dstat=ChangeStateItem(TCPartObj,"LcsAplRlzd",mfail));
			if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;
			printf("\n Setting frozen true for TC part \n\n\n"); fflush(stdout);
			if(dstat=objSetAttribute(TCPartObj,FrozenIndicatorAttr,"+")) goto EXIT;
			string	FrzView		=	NULL;
			string	FrzViewDup	=	NULL;
			t5CheckDstat(objGetAttribute(TCPartObj,FrozenViewMaskAttr,&FrzView));
			if (!nlsIsStrNull(FrzView))
			{
				FrzViewDup	=	nlsStrDup(FrzView);
			}

			string	PartType		=	NULL;
			string	PartTypeDup	=	NULL;
			t5CheckDstat(objGetAttribute(TCPartObj,PartTypeAttr,&PartType));
			if (!nlsIsStrNull(PartType))
			{
				PartTypeDup	=	nlsStrDup(PartType);
			}
			
			string	OrgID		=	NULL;
			string	OrgIDDup	=	NULL;
			t5CheckDstat(objGetAttribute(TCPartObj, OrganizationIDAttr, &OrgID)) ;
			if (!nlsIsStrNull(OrgID))
			{
				OrgIDDup	=	nlsStrDup(OrgID);
			}

			string	StrVewName		=	NULL;
			string	StrVewNameDup	=	NULL;
			t5CheckDstat(objGetAttribute(TCPartObj, StartingViewNameAttr, &StrVewName)) ;
			if (!nlsIsStrNull(StrVewName))
			{
				StrVewNameDup	=	nlsStrDup(StrVewName);
			}
			
			string	t5DocRemarks		=	NULL;
			string	t5DocRemarksDup	=	NULL;
			t5CheckDstat(objGetAttribute(TCPartObj, t5DocRemarksAttr, &t5DocRemarks)) ;
			if (!nlsIsStrNull(t5DocRemarks))
			{
				t5DocRemarksDup	=	nlsStrDup(t5DocRemarks);
			}

			printf("\nFrzViewDup : %s, PartTypeDup : %s, OrgIDDup : %s, StrVewNameDup %s, t5DocRemarksDup : %s",FrzViewDup,PartTypeDup,OrgIDDup,StrVewNameDup,t5DocRemarksDup);fflush(stdout);
			if (!nlsIsStrNull(FrzViewDup))
			{
				if (nlsStrCmp(FrzViewDup,"0")==0)
				{
					
					if (nlsStrCmp(PartTypeDup,"SA")==0 || nlsStrStr(OrgIDDup,"APL")!=NULL || nlsStrStr(StrVewNameDup,"APL")!=NULL || nlsStrStr(t5DocRemarksDup,"APL")!=NULL)
					{
						printf("\n-1 View Update");fflush(stdout);
						t5CheckDstat(objSetAttribute(TCPartObj,FrozenViewMaskAttr,"-1"));
						if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;
						t5CheckDstat(objSetAttribute(TCPartObj,StartingViewBitPosAttr,"1"));
						if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;
					}
					else
					{
						printf("\n01 View Update");fflush(stdout);
						t5CheckDstat(objSetAttribute(TCPartObj,FrozenViewMaskAttr,"01"));
						if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;
					}
				}
				else if (nlsStrStr(FrzViewDup,"01")!=NULL)
				{
					printf("\nAlready View Stamp");fflush(stdout);
					if (nlsStrCmp(PartTypeDup,"SA")==0 || nlsStrStr(OrgIDDup,"APL")!=NULL || nlsStrStr(StrVewNameDup,"APL")!=NULL || nlsStrStr(t5DocRemarksDup,"APL")!=NULL)
					{
						printf("\nelse -1 View Update");fflush(stdout);
						t5CheckDstat(objSetAttribute(TCPartObj,FrozenViewMaskAttr,"-1"));
						if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;
						t5CheckDstat(objSetAttribute(TCPartObj,StartingViewBitPosAttr,"1"));
						if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;

					}
				}
				else
				{
					printf("\nelse2 -1 View Update");fflush(stdout);
					t5CheckDstat(objSetAttribute(TCPartObj,FrozenViewMaskAttr,"-1"));
					if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;
				}
			}
			//t5CheckDstat(objSetAttribute(TCPartObj,StartingViewBitPosAttr,"1"));
			//if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;

			printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
			printf("\n ------------------------------- Bravooooo!!!------------------------------- \n");
			}
			
		}
		//if(RelFlag==1)
		{
			string	LifeCycleDup	=	NULL;
			string	LifeCycle		=	NULL;
			if(dstat = objGetAttribute(TCPartObj,LifeCycleStateAttr,&LifeCycle))  goto CLEANUP;
			if (!nlsIsStrNull(LifeCycle))
			{
				LifeCycleDup	=	nlsStrDup(LifeCycle);
			}
			if (nlsStrCmp(LifeCycleDup,"LcsAplRlzd")!=0 && nlsStrCmp(LifeCycleDup,"LcsSTDRlzd")!=0 && nlsStrCmp(LifeCycleDup,"LcsSTDWrkg")!=0)
			{
				t5CheckDstat(objSetAttribute(TCPartObj,FrozenViewMaskAttr,"01"));
				if(dstat=t0UpdateObject(&TCPartObj,mfail))goto EXIT;
				if(dstat=ChangeStateItem(TCPartObj,"LcsAplRlzd",mfail));
			}
			
			goto CLEANUP;//Remove while loading AMDML
		}

		if(TCPartObj) 
		{
			//t5CheckDstat(ExpandObject4(PartDocClass,TCPartObj,"DocumentsDescribingPart", &allDocsSO, mfail));
			
			if(dstat = ExpandObject2(PartDocClass, TCPartObj,"DocumentsDescribingPart",SC_SCOPE_OF_SESSION,&allDocsSO,&docRelObjs,mfail)) goto CLEANUP;
			if(*mfail) goto CLEANUP; 

			printf("\n total doc found %d \n",setSize(allDocsSO));fflush(stdout);

			// Get Latest documents from set 
			if(nlsStrCmp(PartSupersededDupS,"-")==0)
			{
			if(dstat = t5GetLatestDocumnets(allDocsSO,docRelObjs,&latestDocs,&latestRelDocs,mfail)) goto CLEANUP;
			if(*mfail) goto CLEANUP;
			}
			else
			{
				latestDocs=low_set_cpy(latestDocs,allDocsSO);
				printf("\n set cpy size %d \n",setSize(latestDocs));fflush(stdout);
			}
			//printf("\nIn Documnet Loop Obj -- %d", setSize(allDocsSO));fflush(stdout);
			if(latestDocs)
			{
				for(j=0; j<setSize(latestDocs); j++ ) 
				{			
					convJTObj = NULL;
					allDocsObj = NULL;
					convCad1SO = NULL;
					allDocsObj = low_set_get(latestDocs,j);
					//objDump(allDocsObj);
					t5CheckDstat(objGetAttribute(allDocsObj,OwnerNameAttr,&docOwnerS));
					if(docOwnerS) docOwnerDupS = nlsStrDup(docOwnerS);
					t5CheckDstat(objGetAttribute(allDocsObj,DocumentNameAttr,&docNameS));
					if(docNameS) docNameDupS = nlsStrDup(docNameS);					
					t5CheckDstat(objGetAttribute(allDocsObj,ClassAttr,&docClassNameS));
					if(docClassNameS) docClassNameDupS = nlsStrDup(docClassNameS);
					printf("\ndocClassNameDupS --%s   owner --%s",docClassNameDupS,docOwnerDupS);fflush(stdout);
					printf("\ndocClassNameDupS --%s   owner --%s",objClass(allDocsObj),docOwnerDupS);fflush(stdout);
					//if(nlsStrCmp(docOwnerDupS,"Release Vault")) //if docOwnerDupS is not equal to Release Vault. if non zero
					if(nlsStrCmp(docOwnerDupS,"CE Vault") == 0)
					{				

						if(dstat=SetUpDialog(objClass(allDocsObj),allDocsObj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj3,mfail));
						if(dstat=objSetAttribute(TrndialogObj3,DestOwnerNameAttr,PartOwnerNameS));
						if(dstat=objSetAttribute(TrndialogObj3,IncludeAttachedDIsAttr,"-"));
						printf("\n\t ****Started with BUSINESS DOC transfer to %s****\n",PartOwnerNameS);fflush(stdout);
						timestamp=sysGetCTime(); 
						printf("\n Start Time is \t-- %s \n",timestamp);fflush(stdout);

						if(dstat=TransferObject(allDocsObj,TrndialogObj3,mfail)) goto CLEANUP;
						
						printf("\n Setting Life Cycle state of Document \n\n\n"); fflush(stdout);
						if(dstat=objSetAttribute(allDocsObj,LifeCycleStateAttr,"LcsAplRlzd")) goto EXIT;
						if(dstat=t0UpdateObject(&allDocsObj,mfail))goto EXIT;

						printf("\n Setting frozen true for Document  \n\n\n"); fflush(stdout);
						if(dstat=objSetAttribute(allDocsObj,FrozenIndicatorAttr,"+")) goto EXIT;
						if(dstat=t0UpdateObject(&allDocsObj,mfail))goto EXIT;

						//2.Vineet for Business Document transfer error 
						if(*mfail)
						{
						//If transfer Doc fails.
						printf("TRANSFER of Document failed while trasnsferring to %s",PartOwnerNameS);
						fprintf(fpDI,"%s,",docNameDupS);
						//goto CLEANUP;
						}
						else
						{
						printf("\n\t ****Done with BUSINESS DOC transfer****\n");fflush(stdout);
						timestamp=sysGetCTime(); 
						printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
						printf("\n ------------------------------- Bravooooo!!!------------------------------- \n");
						}
					}
									printf("\n\nAgain setting the attributes of Release for document ");fflush(stdout);
									printf("\n Setting Life Cycle state of Data item \n\n\n"); fflush(stdout);
									if(dstat=objSetAttribute(allDocsObj,LifeCycleStateAttr,"LcsAplRlzd")) goto EXIT;
									if(dstat=t0UpdateObject(&allDocsObj,mfail))goto EXIT;

									printf("\n Setting frozen true for Cad data  \n\n\n"); fflush(stdout);
									if(dstat=objSetAttribute(allDocsObj,FrozenIndicatorAttr,"+")) goto EXIT;
									if(dstat=t0UpdateObject(&allDocsObj,mfail))goto EXIT;
					if(allDocsObj)
					{				
						//printf("\nFound Document Obj");fflush(stdout);
						t5CheckDstat(ExpandObject4(AttachClass,allDocsObj, "DataItemsAttachedToBusItem", &convCad1SO, mfail));
						if(convCad1SO) 
						{	
							for(x=0; x<setSize(convCad1SO); x++ ) 
							{	
								convCad1Obj = low_set_get(convCad1SO,x);
								if (dstat = objGetAttribute(convCad1Obj,RelativePathAttr,&DIRelPathDup)) goto EXIT;
								if(DIRelPathDup) DIRelPath = nlsStrDup(DIRelPathDup);
								printf("\n data item \t-- %s \n",DIRelPath);fflush(stdout);


								if (dstat = objGetAttribute(convCad1Obj,OwnerNameAttr,&DIVaultDup)) goto EXIT;
								if(DIVaultDup) DIVault = nlsStrDup(DIVaultDup);
								printf("\nThe owner of data item is %s",DIVault);fflush(stdout);
								//there may be some error while transferring Revised TC part for cad data and JT. If DIVault is not release Vault then it will go inside the loop and transfer the data item to PartOwnerNameS. I think it will be problem for generic checked in into release vault
								//if( (RelFlag == 1) || nlsStrCmp(DIVault,"Release Vault")!=0) //if Di owner WIP vault will go inside loop, if release vault will not go inside the loop
								if( (RelFlag != 1) && nlsStrCmp(DIVault,"CE Vault")==0)
								{
									
									if(dstat=SetUpDialog(objClass(convCad1Obj),convCad1Obj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj,mfail));
									if(dstat=objSetAttribute(TrndialogObj,DestOwnerNameAttr,PartOwnerNameS));
									printf("\n data item to be trasferred to -- %s \n",PartOwnerNameS);fflush(stdout);
									if(dstat=objSetAttribute(TrndialogObj,DestOwnerDirNameAttr,PartDirOwnerNameS));
									printf("\n data item to be trasferred to vault location  -- %s \n",PartDirOwnerNameS);fflush(stdout);
									if(dstat=objSetAttribute(TrndialogObj,IncludeApdObjectsAttr,"-"));
										
									printf("\n\t ****Started with transfer for CAD Data item to %s****\n",PartOwnerNameS);fflush(stdout);
									timestamp=sysGetCTime(); 
									printf("\n Start Time is \t-- %s \n",timestamp);fflush(stdout);


									if(dstat=TransferObject(convCad1Obj,TrndialogObj,mfail)) goto CLEANUP;

									printf("\n Setting Life Cycle state of Data item \n\n\n"); fflush(stdout);
									if(dstat=objSetAttribute(convCad1Obj,LifeCycleStateAttr,"LcsAplRlzd")) goto EXIT;
									if(dstat=t0UpdateObject(&convCad1Obj,mfail))goto EXIT;

									printf("\n Setting frozen true for Cad data  \n\n\n"); fflush(stdout);
									if(dstat=objSetAttribute(convCad1Obj,FrozenIndicatorAttr,"+")) goto EXIT;
									if(dstat=t0UpdateObject(&convCad1Obj,mfail))goto EXIT;

									//3.Vineet for CAD DI transfer error 
									if(*mfail)
									{
									//If transfer di fails.
									printf("TRANSFER of data item %s failed while trasnsferring to %s",DIRelPath,PartOwnerNameS);
									fprintf(fpDI,"%s,",DIRelPath);
									//goto CLEANUP;
									}
									else
									{
									printf("\n\t ****Done with CAD Data Item transfer****\n");fflush(stdout);

									timestamp=sysGetCTime(); 
									printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
									printf("\n ------------------------------- Bravooooo!!!------------------------------- \n");
									}
								}
								printf("\n\nAgain setting the attributes of Release for data item");fflush(stdout);
									printf("\n Setting Life Cycle state of Data item \n\n\n"); fflush(stdout);
									if(dstat=objSetAttribute(convCad1Obj,LifeCycleStateAttr,"LcsAplRlzd")) goto EXIT;
									if(dstat=t0UpdateObject(&convCad1Obj,mfail))goto EXIT;

									printf("\n Setting frozen true for Cad data  \n\n\n"); fflush(stdout);
									if(dstat=objSetAttribute(convCad1Obj,FrozenIndicatorAttr,"+")) goto EXIT;
									if(dstat=t0UpdateObject(&convCad1Obj,mfail))goto EXIT;

							
								if(convCad1Obj) 
								{
									
									t5CheckDstat(ExpandObject4(GenVisClass,convCad1Obj,"ItmToVisSourceOfVis", &convJTSO, mfail));

									if(convJTSO && (jtFlag==1))
									{
										//printf("\nFound JT File Obj");fflush(stdout);
										convJTObj = low_set_get(convJTSO,0);

										if (dstat = objGetAttribute(convJTObj,OwnerNameAttr,&JtVaultDup)) goto EXIT;
										if(JtVaultDup) JtVault = nlsStrDup(JtVaultDup);
										printf("\nThe owner of jt is %s",JtVault);fflush(stdout);
										if (dstat = objGetAttribute(convJTObj,RelativePathAttr,&JTRelPathDup)) goto EXIT;
										if(JTRelPathDup) JTRelPath = nlsStrDup(JTRelPathDup);
										printf("\n JT \t-- %s \n",JTRelPath);fflush(stdout);
//										if(nlsStrCmp(JtVault,"Release Vault")!=0 && (RelFlag==1) )	
//										{
////											if (dstat = objGetAttribute(convJTObj,RelativePathAttr,&JTRelPathDup)) goto EXIT;
////											if(JTRelPathDup) JTRelPath = nlsStrDup(JTRelPathDup);
////											printf("\n JT \t-- %s \n",JTRelPath);fflush(stdout);
//		
//											if(dstat=SetUpDialog(objClass(convJTObj),convJTObj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj1,mfail));
//											if(dstat=objSetAttribute(TrndialogObj1,DestOwnerNameAttr,PartOwnerNameS_CE));
//											if(dstat=objSetAttribute(TrndialogObj1,DestOwnerDirNameAttr,PartDirOwnerNameS_CE));
//											if(dstat=objSetAttribute(TrndialogObj1,IncludeApdObjectsAttr,"-"));
//												
//											printf("\n\t ****Started with transfer JT to CE and trasferring jt to %s****\n",PartOwnerNameS_CE);fflush(stdout);
//											timestamp=sysGetCTime(); 
//											printf("\n Start Time is \t-- %s \n",timestamp);fflush(stdout);
//											if(dstat=TransferObject(convJTObj,TrndialogObj1,mfail)) goto CLEANUP;
//											//4.1 Vineet for Jt transfer error 
//											if(*mfail)
//											{
//											//If transfer jt transfer to CE fails.
//											printf("TRANSFER of jt %s failed while trasnsferring to %s",JTRelPath,PartOwnerNameS_CE);
//											fprintf(fpDI,"%s,",JTRelPath);
//											//goto CLEANUP;
//											}
//											else
//											{
//											printf("\n\t ****Done with transfer JT 2 CE****\n");fflush(stdout);
//											timestamp=sysGetCTime(); 
//											printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
//											printf("\n ------------------------------- Bravooooo JT Transfered!!!------------------------------- \n");
//											}
//											RefreshDbObject(convJTObj,mfail);
//										}
										if(nlsStrCmp(JtVault,"CE Vault")== 0 && (RelFlag!=1))
										{
											if(dstat=SetUpDialog(objClass(convJTObj),convJTObj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj2,mfail));
											if(dstat=objSetAttribute(TrndialogObj2,DestOwnerNameAttr,PartOwnerNameS));
											if(dstat=objSetAttribute(TrndialogObj2,DestOwnerDirNameAttr,PartDirOwnerNameS));
											if(dstat=objSetAttribute(TrndialogObj2,IncludeApdObjectsAttr,"-"));
											printf("\n\t ****Started with transfer JT (transfer to Rel and) to %s****\n",PartOwnerNameS);fflush(stdout);
											timestamp=sysGetCTime(); 
											printf("\n Start Time is \t-- %s \n",timestamp);fflush(stdout);
											if(dstat=TransferObject(convJTObj,TrndialogObj2,mfail)) goto CLEANUP;
											//4.2 Vineet for Jt transfer error 
											if(*mfail)
											{
											//If transfer jt fail to Release fails.
											printf("TRANSFER of jt %s failed while trasnsferring to %s",JTRelPath,PartOwnerNameS);
											fprintf(fpDI,"%s,",JTRelPath);
											//goto CLEANUP;
											}
											else
											{
											printf("\n\t ****Done with transfer JT 2 Rel****\n");fflush(stdout);
										
											printf("\n Setting frozen true for Jt data  \n\n\n"); fflush(stdout);
											if(dstat=objSetAttribute(convJTObj,FrozenIndicatorAttr,"+")) goto EXIT;
											if(dstat=t0UpdateObject(&convJTObj,mfail))goto EXIT;
										
											timestamp=sysGetCTime(); 
											printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
											printf("\n ------------------------------- Bravooooo JT Rel Transfered!!!------------------------------- \n");	
											}
										}
										printf("\n\nAgain setting the attributes of Frozen for jt item");fflush(stdout);
											printf("\n Setting frozen true for Jt data  \n\n\n"); fflush(stdout);
											if(dstat=objSetAttribute(convJTObj,FrozenIndicatorAttr,"+")) goto EXIT;
											if(dstat=t0UpdateObject(&convJTObj,mfail))goto EXIT;
									}
								}
							}	
						}
					}
				}

			}
				allDocsObj = NULL;
		}										
			allDocsSO = NULL;
//		if(TCPartObj)
//		{
//			printf("\n Loop for direct DFMEA Documents starts");
//
//			//t5CheckDstat(ExpandObject4(PartDocClass,TCPartObj,"DocumentsDescribingPart", &allDocsSO, mfail));
//			if(dstat = ExpandObject2(t5DfmStcClass,TCPartObj,"t5PartHasDfm",SC_SCOPE_OF_SESSION,&DFMEADocsSO,&DdocRelObjs,mfail)) goto CLEANUP;
//			if(*mfail) goto CLEANUP; 
//
//			printf("\n total DFMEAdoc found %d \n",setSize(DFMEADocsSO));fflush(stdout);
//		}
//			// Get Latest documents from set 
//			if(nlsStrCmp(PartSupersededDupS,"-")==0)
//			{
//			if(dstat = t5GetLatestDocumnets(DFMEADocsSO,DdocRelObjs,&latestDfmeaDocs,&latestRelDfmeaDocs,mfail)) goto CLEANUP;
//			if(*mfail) goto CLEANUP;
//			}
//			else
//			{
//				latestDocs=low_set_cpy(latestDocs,allDocsSO);
//				printf("\n set cpy size %d \n",setSize(latestDocs));fflush(stdout);
//			}
//			//printf("\nIn Documnet Loop Obj -- %d", setSize(allDocsSO));fflush(stdout);
//			if(DFMEADocsSO)
//			{
//				for(jj=0; jj<setSize(DFMEADocsSO); jj++ ) 
//				{			
////					convJTObj = NULL;
////					allDocsObj = NULL;
//					DFMEADocsObj = NULL;
////					convCad1SO = NULL;
//					DFMEADocsObj = low_set_get(DFMEADocsSO,jj);
////					//objDump(allDocsObj);
//					t5CheckDstat(objGetAttribute(DFMEADocsObj,OwnerNameAttr,&DfmeadocOwnerS));
//					if(DfmeadocOwnerS) DfmeadocOwnerDupS = nlsStrDup(DfmeadocOwnerS);
//					t5CheckDstat(objGetAttribute(DFMEADocsObj,t5FmeaNumAttr,&DfmeadocNameS));
//					if(DfmeadocNameS) DfmeadocNameDupS = nlsStrDup(DfmeadocNameS);					
//					t5CheckDstat(objGetAttribute(DFMEADocsObj,ClassAttr,&DfmeadocClassNameS));
//					if(DfmeadocClassNameS) DfmeadocClassNameDupS = nlsStrDup(DfmeadocClassNameS);
//					printf("\ndocClassNameDupS --%s   owner --%s",DfmeadocClassNameDupS,DfmeadocOwnerDupS);fflush(stdout);
//					printf("\ndocClassNameDupS --%s   owner --%s",objClass(DFMEADocsObj),DfmeadocOwnerDupS);fflush(stdout);
//					if((nlsStrStr(DfmeadocOwnerDupS,"Vault")!=NULL)) //added to check if in vault
//					{
//						printf("\n Dfmea Document is present in Vault");fflush(stdout);
//						if(nlsStrCmp(DfmeadocOwnerDupS,"Release Vault"))
//						{				
//
//							if(dstat=SetUpDialog(objClass(DFMEADocsObj),DFMEADocsObj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj5,mfail));
//							if(dstat=objSetAttribute(TrndialogObj5,DestOwnerNameAttr,PartOwnerNameS));
//							if(dstat=objSetAttribute(TrndialogObj5,IncludeAttachedDIsAttr,"-"));
//							printf("\n\t ****Started with DFMEA DOC transfer to %s****\n",PartOwnerNameS);fflush(stdout);
//							timestamp=sysGetCTime(); 
//							printf("\n Start Time is \t-- %s \n",timestamp);fflush(stdout);
//							if(dstat=TransferObject(DFMEADocsObj,TrndialogObj5,mfail)) goto CLEANUP;
//							//5.Vineet for DFMEA Document transfer error 
//							if(*mfail)
//							{
//							//If transfer Doc fails.
//							printf("TRANSFER of DFMEA Document failed while trasnsferring to %s",PartOwnerNameS);
//							fprintf(fpDI,"%s,",DfmeadocNameDupS);
//							//goto CLEANUP;
//							}
//							else
//							{
//							printf("\n\t ****Done with DFMEA DOC transfer****\n");fflush(stdout);
//							timestamp=sysGetCTime(); 
//							printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
//							printf("\n ------------------------------- Bravooooo!!!------------------------------- \n");
//							}
//						}
//						if(DFMEADocsObj)
//						{				
//							//printf("\nFound Document Obj");fflush(stdout);
//							t5CheckDstat(ExpandObject4(AttachClass,DFMEADocsObj,"DataItemsAttachedToBusItem",&DataItemDfmeaSO, mfail));
//							if(DataItemDfmeaSO) 
//							{	
//								for(xx=0; xx<setSize(DataItemDfmeaSO); xx++ ) 
//								{	
//									DataItemDfmeaObj = low_set_get(DataItemDfmeaSO,xx);
//									if (dstat = objGetAttribute(DataItemDfmeaObj,RelativePathAttr,&DfmeaDIRelPathDup)) goto EXIT;
//									if(DfmeaDIRelPathDup) DfmeaDIRelPath = nlsStrDup(DfmeaDIRelPathDup);
//									printf("\n data item \t-- %s \n",DfmeaDIRelPath);fflush(stdout);
//									if (dstat = objGetAttribute(DataItemDfmeaObj,OwnerNameAttr,&DfmeaDIVaultDup)) goto EXIT;
//									if(DfmeaDIVaultDup) DfmeaDIVault = nlsStrDup(DfmeaDIVaultDup);
//									//there may be some error while transferring Revised TC part for cad data and JT
//									if( (RelFlag == 1) || nlsStrCmp(DfmeaDIVault,"Release Vault")!=0)
//									{
//										if(dstat=SetUpDialog(objClass(DataItemDfmeaObj),DataItemDfmeaObj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj6,mfail));
//										if(dstat=objSetAttribute(TrndialogObj6,DestOwnerNameAttr,PartOwnerNameS));
//										if(dstat=objSetAttribute(TrndialogObj6,DestOwnerDirNameAttr,PartDirOwnerNameS));
//										if(dstat=objSetAttribute(TrndialogObj6,IncludeApdObjectsAttr,"-"));
//											
//										printf("\n\t ****Started with transfer of DFMEA Data item to %s****\n",PartOwnerNameS);fflush(stdout);
//										timestamp=sysGetCTime(); 
//										printf("\n Start Time is \t-- %s \n",timestamp);fflush(stdout);
//
//
//										if(dstat=TransferObject(DataItemDfmeaObj,TrndialogObj6,mfail)) goto CLEANUP;
//										//6.Vineet for DFMEA DI transfer error 
//										if(*mfail)
//										{
//										//If transfer di fails.
//										printf("TRANSFER of data item %s failed while trasnsferring to %s",DfmeaDIRelPath,PartOwnerNameS);
//										fprintf(fpDI,"%s,",DfmeaDIRelPath);
//										//goto CLEANUP;
//										}
//										else
//										{
//										printf("\n\t ****Done with DFMEA Data Item transfer****\n");fflush(stdout);
//										timestamp=sysGetCTime(); 
//										printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
//										printf("\n ------------------------------- Bravooooo!!!------------------------------- \n");
//										}
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//			DFMEADocsObj=NULL;
//			DFMEADocsSO=NULL;

//			if(TCPartObj)
//			{
//				printf("\n Loop for direct DAD Documents starts");
//
//			//t5CheckDstat(ExpandObject4(PartDocClass,TCPartObj,"DocumentsDescribingPart", &allDocsSO, mfail));
//			if(dstat = ExpandObject2(t5DADStcClass,TCPartObj,"t5PartHasDAD",SC_SCOPE_OF_SESSION,&DADdocsSO,&DADdocRelSO,mfail)) goto CLEANUP;
//			if(*mfail) goto CLEANUP; 
//
//			printf("\n total DAD found %d \n",setSize(DADdocsSO));fflush(stdout);
//			}
//			if(DADdocsSO)
//			{
//				for(jjj=0; jjj<setSize(DADdocsSO); jjj++ ) 
//				{			
////					convJTObj = NULL;
////					allDocsObj = NULL;
//					DADdocsObj = NULL;
////					convCad1SO = NULL;
//					DADdocsObj = low_set_get(DADdocsSO,jjj);
////					//objDump(allDocsObj);
//					t5CheckDstat(objGetAttribute(DADdocsObj,OwnerNameAttr,&DADdocOwnerS));
//					if(DADdocOwnerS) DADdocOwnerDupS = nlsStrDup(DADdocOwnerS);
//					t5CheckDstat(objGetAttribute(DADdocsObj,t5DsgnNumAttr,&DADdocNameS));
//					if(DADdocNameS) DADdocNameDupS = nlsStrDup(DADdocNameS);					
//					t5CheckDstat(objGetAttribute(DADdocsObj,ClassAttr,&DADdocClassNameS));
//					if(DADdocClassNameS) DADdocClassNameDupS = nlsStrDup(DADdocClassNameS);
//					printf("\ndocClassNameDupS --%s   owner --%s",DADdocClassNameDupS,DADdocOwnerDupS);fflush(stdout);
//					printf("\ndocClassNameDupS --%s   owner --%s",objClass(DADdocsObj),DADdocOwnerDupS);fflush(stdout);
//					if((nlsStrStr(DADdocOwnerDupS,"Vault")!=NULL))
//					{
//						printf("\n DAD Document is present in Vault");fflush(stdout);
//						if(nlsStrCmp(DADdocOwnerDupS,"Release Vault"))
//						{				
//
//							if(dstat=SetUpDialog(objClass(DADdocsObj),DADdocsObj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj7,mfail));
//							if(dstat=objSetAttribute(TrndialogObj7,DestOwnerNameAttr,PartOwnerNameS));
//							if(dstat=objSetAttribute(TrndialogObj7,IncludeAttachedDIsAttr,"-"));
//							printf("\n\t ****Started with DAD DOC transfer to %s****\n",PartOwnerNameS);fflush(stdout);
//							timestamp=sysGetCTime(); 
//							printf("\n Start Time is \t-- %s \n",timestamp);fflush(stdout);
//							if(dstat=TransferObject(DADdocsObj,TrndialogObj7,mfail)) goto CLEANUP;
//							//7.Vineet for DAD Document transfer error 
//							if(*mfail)
//							{
//							//If transfer Doc fails.
//							printf("TRANSFER of DAD Document failed while trasnsferring to %s",PartOwnerNameS);
//							fprintf(fpDI,"%s,",DADdocNameDupS);
//							//goto CLEANUP;
//							}
//							else
//							{
//							printf("\n\t ****Done with DAD DOC transfer****\n");fflush(stdout);
//							timestamp=sysGetCTime(); 
//							printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
//							printf("\n ------------------------------- Bravooooo!!!------------------------------- \n");
//							}
//						}
//						if(DADdocsObj)
//						{				
//							//printf("\nFound Document Obj");fflush(stdout);
//							t5CheckDstat(ExpandObject4(AttachClass,DADdocsObj,"DataItemsAttachedToBusItem",&DataItemDADSO, mfail));
//							if(DataItemDADSO) 
//							{	
//								for(xxx=0; xxx<setSize(DataItemDADSO); xxx++ ) 
//								{	
//									DataItemDADObj = low_set_get(DataItemDADSO,xxx);
//									if (dstat = objGetAttribute(DataItemDADObj,RelativePathAttr,&DADDIRelPathDup)) goto EXIT;
//									if(DADDIRelPathDup) DADDIRelPath = nlsStrDup(DADDIRelPathDup);
//									printf("\n data item \t-- %s \n",DADDIRelPath);fflush(stdout);
//									if (dstat = objGetAttribute(DataItemDADObj,OwnerNameAttr,&DADDIVaultDup)) goto EXIT;
//									if(DADDIVaultDup) DADDIVault = nlsStrDup(DADDIVaultDup);
//									//there may be some error while transferring Revised TC part for cad data and JT
//									if( (RelFlag == 1) || nlsStrCmp(DADDIVault,"Release Vault")!=0)
//									{
//										if(dstat=SetUpDialog(objClass(DataItemDADObj),DataItemDADObj,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj8,mfail));
//										if(dstat=objSetAttribute(TrndialogObj8,DestOwnerNameAttr,PartOwnerNameS));
//										if(dstat=objSetAttribute(TrndialogObj8,DestOwnerDirNameAttr,PartDirOwnerNameS));
//										if(dstat=objSetAttribute(TrndialogObj8,IncludeApdObjectsAttr,"-"));
//											
//										printf("\n\t ****Started with transfer for DAD Data item to %s****\n",PartOwnerNameS);fflush(stdout);
//										timestamp=sysGetCTime(); 
//										printf("\n Start Time is \t-- %s \n",timestamp);fflush(stdout);
//
//
//										if(dstat=TransferObject(DataItemDADObj,TrndialogObj8,mfail)) goto CLEANUP;
//										//8.Vineet for DAD DI transfer error 
//										if(*mfail)
//										{
//										//If transfer di fails.
//										printf("TRANSFER of data item %s failed while trasnsferring to %s",DADDIRelPath,PartOwnerNameS);
//										fprintf(fpDI,"%s,",DADDIRelPath);
//										//goto CLEANUP;
//										}
//										else
//										{
//										printf("\n\t ****Done with DAD Data Item transfer****\n");fflush(stdout);
//										timestamp=sysGetCTime(); 
//										printf("\n End Time is \t-- %s \n",timestamp);fflush(stdout);
//										printf("\n ------------------------------- Bravooooo!!!------------------------------- \n");
//										}
//									}
//								}
//							}
//						}
//					}
//					DADdocsObj=NULL;
//				}
//			}
			//DADdocsObj=NULL;
			//DADdocsSO=NULL;
//	}
	//Master Transfer
	string MasterOwnerNm = NULL;
	SetOfObjects PartObjMstSo = NULL;
	SetOfObjects PartObjMstSoRel = NULL;
	t5CheckMfail(ExpandObject2(ItemRevClass,TCPartObj,"ItemMstrForStrucBIRev",SC_SCOPE_OF_SESSION,&PartObjMstSo,&PartObjMstSoRel,mfail)) ;  
	printf("\n Master count is %d\n",setSize(PartObjMstSo));fflush(stdout);
	if (setSize(PartObjMstSo)>0)
	{
		PartObjMsPtr=(setGet(PartObjMstSo,0));
		t5CheckDstat(objGetAttribute(setGet(PartObjMstSo,0),OwnerNameAttr,&MasterOwnerNm));
		printf("\n Master owner name is %s\n",MasterOwnerNm);fflush(stdout);
//		if (nlsStrStr(MasterOwnerNm,"Vault")==NULL)
//		{
//			printf("\n Inside t5CheckInCstAndMstrObject is \n");fflush(stdout);
//			if(dstat = t5CheckInCstAndMstrObject(TCPartObj,mfail))
//			{
//				printf("\n Error in transfering the Part master and cost. Please check");fflush(stdout);
//			}
//		}
		if ((nlsStrStr(MasterOwnerNm,"Vault")==NULL) || (nlsStrCmp(MasterOwnerNm,"CE Vault")==0) )
		{
			printf("\n Transferring master to Release Vault \n");fflush(stdout);
			if(dstat=SetUpDialog(objClass(PartObjMsPtr),PartObjMsPtr,"TransferDialogC",&extraStr,&extraObj,&TrndialogObj5,mfail));
			if(dstat=objSetAttribute(TrndialogObj5,DestOwnerNameAttr,"Release Vault"));
			//if(dstat=objSetAttribute(TrndialogObj5,IncludeAttachedDIsAttr,"-"));	
			printf("\n\t ****Started with Part Master Transfer transfer to Release Vault****\n",PartOwnerNameS);fflush(stdout);
			timestamp=sysGetCTime(); 
			printf("\n Start Time is \t-- %s Part Mater Transfer \n",timestamp);fflush(stdout);
			if(dstat=TransferObject(PartObjMsPtr,TrndialogObj5,mfail)) goto CLEANUP;
			printf("\n ------------------------------- Bravooooo!!!------------------------------- \n");

		}
	}
	else
	{
		printf("\n Master not found ");fflush(stdout);
	}
	//For Next official Revision 08/12/2018
printf("\n Making the TC part Official if it is released \n");fflush(stdout);
	string PartFinalOwnerS = NULL;
	string PartFinalOwnerDupS = NULL;
	t5CheckDstat(objGetAttribute(TCPartObj,OwnerNameAttr,&PartFinalOwnerS));
	if(PartFinalOwnerS) PartFinalOwnerDupS = nlsStrDup(PartFinalOwnerS);
	if (nlsStrCmp(PartFinalOwnerDupS,"Release Vault")==0)
	{
			if(dstat = MakeOfficialObject(TCPartObj,NULL,mfail))goto CLEANUP;
			printf("\nTC part made official\n");fflush(stdout);
	}
	}
	//RightPartObjP = NULL;

	fprintf(fpDI,"\n");
	TCPartObj = NULL;
//	RightPartObjP = NULL;

	CLEANUP:
		t5PrintCleanUpModName;
		//if(convDocSO) objDisposeAll(convDocSO);
		//if(convCadSO) objDisposeAll(convCadSO);
		if(convJTSO) objDisposeAll(convJTSO);
		if(allDocsSO) objDisposeAll(allDocsSO);
		if(TCPartSO) objDisposeAll(TCPartSO);
		//if(convCadObj) convCadObj = NULL;
		if(convJTObj) convJTObj = NULL;
		if(allDocsObj) allDocsObj = NULL;
		//if(RightPartObjP) RightPartObjP = NULL;
		if(TCPartObj) TCPartObj = NULL;
		t5FreeSqlPtr(convDocsql);
		printf("\nCLEANUP OF t5TrnsfrToVault \n");	fflush(stdout);
	EXIT:
		//t5CheckDstatAndReturn;
		printf("\nEXITTING t5TrnsfrToVault \n");fflush(stdout);
	printf("\n***************Transfer for next part starts*********************\n\n\n\\n\n\n");fflush(stdout);
}


/*
t5TrnsfrToVault "super user" abc123 "2829300215R"  "NR" "1" "Release Vault" "CE Vault"
t5TrnsfrToVault "super user" abc123 "253403100200" "NR" "1" "Release Vault" "super user"
for official: t5TrnsfrToVault "Loader" "123loader"  "2829052091R" "NR" "1" "Released"
*/





