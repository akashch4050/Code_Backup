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

status GetTCPartJTCreationForAPL(ObjectPtr TCPartObjP,FILE *fp_DMLPartJTCADInfo,FILE *fp_DMLErrorLog,integer* mfail )
{

	int LatestDoc=0;
	int DataItem=0;
	//int VisFile=0;
	int ConvertedFlag =0;

	string  TCPartOwnerNameDupS=NULL;
	string  TCPartOwnerNameS=NULL;
	string  DocBIClassNameDupS=NULL;
	string  DocBIClassNameS=NULL;
	string  VisFileRelPathDupS=NULL;
	string  VisFileRelPathS=NULL;
	string  VisFileWorkingRelativePathDupS=NULL;
	string  VisFileWorkingRelativePathS=NULL;
	string  VisFileSequenceDupS=NULL;
	string  VisFileSequenceS=NULL;
	string  VisFileClassDupS=NULL;
	string  VisFileClassS=NULL;
	string  VisFileOwnerNameDupS=NULL;
	string  VisFileOwnerNameS=NULL;
	string  TCPartNumberDupS=NULL;
	string  TCPartNumberS=NULL;
	string  TCPartRevisionDupS=NULL;
	string  TCPartRevisionS=NULL;
	string  TCPartSequenceDupS=NULL;
	string  TCPartSequenceS=NULL;
	string  TCPartTypeDupS=NULL;
	string  TCPartTypeS=NULL;
	string  VisFileOwnerDirNameS=NULL;
	string  VisFileOwnerDirNameDupS=NULL;
	string  DiClassS=NULL;
	string  DiClassDupS=NULL;
	string fullPathattrDup=NULL;
	string fullPathattr=NULL;
	string CatiaDisS=NULL;
	string CatiaDisDupS=NULL;

	SetOfObjects  JTUsesPartFilePhyItemSO = NULL ;
	SetOfObjects  JTUsesPartFileRelSO = NULL ;
	SetOfObjects  DocumentSO = NULL ;
	SetOfObjects  DocumentRelSO = NULL ;
	SetOfObjects  LatestDocSO = NULL ;
	SetOfObjects  LatestRelDocSO = NULL ;
	SetOfObjects  DataItemSO = NULL ;
	SetOfObjects  VisualizationFileSO = NULL ;

	ObjectPtr LatestDocObjP=NULL;
	ObjectPtr NewDataItemObjP=NULL;
	//ObjectPtr DataItemObjP=NULL;
	//ObjectPtr NewVisFileObjP=NULL;
	//ObjectPtr VisFileObjP=NULL;
	ObjectPtr proprtrevObjP=NULL;

	t5MethodInit("GetTCPartJTCreationForAPL");

	t5CheckDstat(objGetAttribute(TCPartObjP,OwnerNameAttr,&TCPartOwnerNameDupS));
	if(!nlsIsStrNull(TCPartOwnerNameDupS)) TCPartOwnerNameS=nlsStrDup(TCPartOwnerNameDupS);
	printf("\n TCPartOwnerNameS:%s \n",TCPartOwnerNameS); fflush(stdout);

	t5CheckDstat(objGetAttribute(TCPartObjP,PartTypeAttr,&TCPartTypeDupS));
	if(!nlsIsStrNull(TCPartTypeDupS))TCPartTypeS=nlsStrDup(TCPartTypeDupS);
	
	t5CheckDstat(objGetAttribute(TCPartObjP,PartNumberAttr,&TCPartNumberDupS));
	if(!nlsIsStrNull(TCPartNumberDupS))TCPartNumberS=nlsStrDup(TCPartNumberDupS);
	
	t5CheckDstat(objGetAttribute(TCPartObjP,RevisionAttr,&TCPartRevisionDupS));
	if(!nlsIsStrNull(TCPartRevisionDupS))TCPartRevisionS=nlsStrDup(TCPartRevisionDupS);
	
	t5CheckDstat(objGetAttribute(TCPartObjP,SequenceAttr,&TCPartSequenceDupS));
	if(!nlsIsStrNull(TCPartSequenceDupS))TCPartSequenceS=nlsStrDup(TCPartSequenceDupS);

	printf("\n Executing Part Number:[%s],TCPartTypeS :[%s]",TCPartNumberS,TCPartTypeS);fflush(stdout);

	if (!nlsStrCmp(TCPartOwnerNameS,"WIP Vault") ||!nlsStrCmp(TCPartOwnerNameS,"Release Vault") ||!nlsStrCmp(TCPartOwnerNameS,"JSR WIP Vault") ||!nlsStrCmp(TCPartOwnerNameS,"LKO WIP Vault") ||!nlsStrCmp(TCPartOwnerNameS,"HNJ WIP Vault") ||!nlsStrCmp(TCPartOwnerNameS,"JSR Release Vault") ||!nlsStrCmp(TCPartOwnerNameS,"LKO Release Vault") ||!nlsStrCmp(TCPartOwnerNameS,"HNJ Release Vault") || nlsStrStr(TCPartOwnerNameS,"CE Vault")!=NULL)
	{
		t5CheckMfail(ExpandObject2(PartDocClass,TCPartObjP,"DocumentsDescribingPart",SC_SCOPE_OF_SESSION,&DocumentSO,&DocumentRelSO,mfail));

		t5CheckMfail(t5GetLatestDocumnets(DocumentSO,DocumentRelSO,&LatestDocSO,&LatestRelDocSO,mfail));

		printf("\n LatestDocSO --->:[%d]",setSize(LatestDocSO));fflush(stdout);
        ConvertedFlag = 0;
		for(LatestDoc=0;LatestDoc<setSize(LatestDocSO);LatestDoc++)
		{
			LatestDocObjP=((ObjectPtr)setGet(LatestDocSO,LatestDoc));

			t5CheckDstat(objGetAttribute(LatestDocObjP,ClassAttr,&DocBIClassNameDupS));
			if(!nlsIsStrNull(DocBIClassNameDupS)) DocBIClassNameS=nlsStrDup(DocBIClassNameDupS);
			printf("\n Document Class:[%s]",DocBIClassNameS);fflush(stdout);
			
			if(nlsStrCmp(DocBIClassNameS,"t5DrwDoc")==0)
			{
                t5CheckMfail(ExpandObject5(AttachClass,LatestDocObjP,"DataItemsAttachedToBusItem",SC_SCOPE_OF_SESSION,&DataItemSO,mfail));

				if(setSize(DataItemSO)>0)
				  {
					//CODE For Getting the Data Item Files
					for(DataItem=0;DataItem<setSize(DataItemSO);DataItem++)
					{
						NewDataItemObjP=NULL;

						NewDataItemObjP=setGet(DataItemSO,DataItem);

						DiClassS=NULL;
						DiClassDupS=NULL;

						t5CheckDstat(objGetAttribute(NewDataItemObjP,ClassAttr,&DiClassDupS));
						if(!nlsIsStrNull(DiClassDupS)) DiClassS=nlsStrDup(DiClassDupS);
						printf("\n Data item class name:[%s]",DiClassS);fflush(stdout);

						//t5CheckDstat(objCopy(&DataItemObjP,NewDataItemObjP));
						
						if (nlsStrCmp(DiClassS,"PdfFile")==0)
						{
							
							//NewVisFileObjP=setGet(DataItemSO,0);

							//t5CheckDstat(objCopy(&VisFileObjP,NewVisFileObjP));
							
							t5CheckDstat(objGetAttribute(NewDataItemObjP,RelativePathAttr,&VisFileRelPathDupS));
							if(!nlsIsStrNull(VisFileRelPathDupS)) VisFileRelPathS=nlsStrDup(VisFileRelPathDupS);
							
							t5CheckDstat(objGetAttribute(NewDataItemObjP,DisplayedNameAttr,&CatiaDisDupS));
							if(!nlsIsStrNull(CatiaDisDupS)) CatiaDisS=nlsStrDup(CatiaDisDupS);
							
							t5CheckDstat(objGetAttribute(NewDataItemObjP,WorkingRelativePathAttr,&VisFileWorkingRelativePathDupS));
							if(!nlsIsStrNull(VisFileWorkingRelativePathDupS)) VisFileWorkingRelativePathS=nlsStrDup(VisFileWorkingRelativePathDupS);
							
							t5CheckDstat(objGetAttribute(NewDataItemObjP,SequenceAttr,&VisFileSequenceDupS));
							if(!nlsIsStrNull(VisFileSequenceDupS)) VisFileSequenceS=nlsStrDup(VisFileSequenceDupS);
							
							t5CheckDstat(objGetAttribute(NewDataItemObjP,ClassAttr,&VisFileClassDupS));
							if(!nlsIsStrNull(VisFileClassDupS)) VisFileClassS=nlsStrDup(VisFileClassDupS);
							
							t5CheckDstat(objGetAttribute(NewDataItemObjP,OwnerNameAttr,&VisFileOwnerNameDupS));
							if(!nlsIsStrNull(VisFileOwnerNameDupS)) VisFileOwnerNameS=nlsStrDup(VisFileOwnerNameDupS);
							
							t5CheckDstat(objGetAttribute(NewDataItemObjP,OwnerDirNameAttr,&VisFileOwnerDirNameDupS));
							if(!nlsIsStrNull(VisFileOwnerDirNameDupS)) VisFileOwnerDirNameS=nlsStrDup(VisFileOwnerDirNameDupS);
							
							proprtrevObjP=NULL;
							t5CheckDstat(GetInfoItem(NewDataItemObjP,&proprtrevObjP,mfail));
							
							t5CheckDstat(objGetAttribute(proprtrevObjP,FullPathAttr,&fullPathattr)) ;
							if(!nlsIsStrNull(fullPathattr)) fullPathattrDup=nlsStrDup(fullPathattr);

							printf("\n CatiaDisS :%s:\n",CatiaDisS); fflush(stdout);
							printf("\n fullPathattrDup :%s:\n",fullPathattrDup); fflush(stdout);


							if (!nlsStrCmp(VisFileOwnerNameS,"WIP Vault") ||!nlsStrCmp(VisFileOwnerNameS,"Release Vault")  ||!nlsStrCmp(VisFileOwnerNameS,"JT Vault") ||!nlsStrCmp(VisFileOwnerNameS,"JSR WIP Vault") ||!nlsStrCmp(VisFileOwnerNameS,"LKO WIP Vault") ||!nlsStrCmp(VisFileOwnerNameS,"HNJ WIP Vault") ||!nlsStrCmp(VisFileOwnerNameS,"JSR Release Vault") ||!nlsStrCmp(VisFileOwnerNameS,"LKO Release Vault") ||!nlsStrCmp(VisFileOwnerNameS,"HNJ Release Vault") || nlsStrStr(VisFileOwnerNameS,"CE Vault")!=NULL)
							{
								fprintf(fp_DMLPartJTCADInfo,"%s",TCPartNumberS);
								fprintf(fp_DMLPartJTCADInfo,"^");
								fprintf(fp_DMLPartJTCADInfo,"%s",TCPartNumberS);
								fprintf(fp_DMLPartJTCADInfo,"^");
								fprintf(fp_DMLPartJTCADInfo,"%s",TCPartRevisionS);
								fprintf(fp_DMLPartJTCADInfo,"^");
								fprintf(fp_DMLPartJTCADInfo,"%s",TCPartSequenceS);
								fprintf(fp_DMLPartJTCADInfo,"^");
								fprintf(fp_DMLPartJTCADInfo,"%s",CatiaDisS);
								fprintf(fp_DMLPartJTCADInfo,"^");
								fprintf(fp_DMLPartJTCADInfo,"%s",fullPathattrDup);
								fprintf(fp_DMLPartJTCADInfo,"^");
								fprintf(fp_DMLPartJTCADInfo,"1");
								fprintf(fp_DMLPartJTCADInfo,"\n");
							}											
						
						}

					}

				  }			
			}
			else
			{
				printf("\n For Part [%s]:[%s]:[%s] ,Document Class is other than t5DrwDoc..:[%s]",TCPartNumberS,TCPartRevisionS,TCPartSequenceS,DocBIClassNameS);fflush(stdout);	
				fprintf(fp_DMLErrorLog,"\n For Part [%s]:[%s]:[%s] ,Document Class is other than t5DrwDoc..:[%s]",TCPartNumberS,TCPartRevisionS,TCPartSequenceS,DocBIClassNameS);
			}

		}


	}
	else
	{
		printf("\n The Resultant TC Part is not in Vault(Func)..\n"); fflush(stdout);
		fprintf(fp_DMLErrorLog,"\n For Part [%s]:[%s]:[%s] is not in Vault..",TCPartNumberS,TCPartRevisionS,TCPartSequenceS);


	}


CLEANUP:
		//printf("\n Function GetTCPartInfoForContextBOMViewJTCreationBOMLevel (Executing GetTCPartInfoForContextBOMViewJTCreationBOMLevel Function) CLEANUP..\n"); fflush(stdout);
		t5PrintCleanUpModName;
		t5FreeSetOfObjects(JTUsesPartFilePhyItemSO);
		t5FreeSetOfObjects(JTUsesPartFileRelSO);
		t5FreeSetOfObjects(DocumentSO);
		t5FreeSetOfObjects(DocumentRelSO);
		t5FreeSetOfObjects(LatestDocSO);
		t5FreeSetOfObjects(LatestRelDocSO);
		t5FreeSetOfObjects(DataItemSO);
		t5FreeSetOfObjects(VisualizationFileSO);

EXIT:
		//printf("\n Function GetTCPartInfoForContextBOMViewJTCreationBOMLevel (Executing GetTCPartInfoForContextBOMViewJTCreationBOMLevel Function) EXIT..\n"); fflush(stdout);
		t5CheckDstatAndReturn;

}

//FUNCTION TO GET OPTIONAL CS FOR PART CREATION 
status GetTCPartOptionalCS(ObjectPtr TCPartObjP,string GenOptionalCS,FILE* fp_DMLOPCSInfo,FILE* fp_DMLErrorLog,integer* mfail )
{
	string PartObidVal=NULL;
	string PartObidValDup=NULL;
	string PartNoVal=NULL;
	string PartNoValDup=NULL;
	string PartNoRev=NULL;
	string PartNoRevDup=NULL;
	string PartNoSeq=NULL;
	string PartNoSeqDup=NULL;
	string instanceName1=NULL;
	string instanceName1Dup=NULL;
	string OPCSVal=NULL;
	string OPCSValDup=NULL;
	string PartNoOrgIDDup=NULL;
	string PartNoOrgID=NULL;
	string rightOBID=NULL;
	string rightOBIDDup=NULL;
	string PrtNumberVal=NULL;
	string PrtNumberValDup=NULL;

	SqlPtr sqlPtr=NULL;
	SqlPtr d_sql=NULL;
	SetOfObjects g2asmCntObjs=NULL;
	SetOfObjects setOfPartsObjs=NULL;
	ObjectPtr childPartObj=NULL;
	int c=0;
	
	t5MethodInit("GetTCPartOptionalCS");	
	
	printf("\n  GenOptionalCS:%s \n",GenOptionalCS); fflush(stdout);

	if(dstat = objGetAttribute(TCPartObjP,OBIDAttr,&PartObidVal))goto EXIT;
	if(!nlsIsStrNull(PartObidVal)) PartObidValDup=nlsStrDup(PartObidVal);
	printf("\n  PartObidValDup:%s \n",PartObidValDup); fflush(stdout);	

	if(dstat = objGetAttribute(TCPartObjP,PartNumberAttr,&PartNoVal))goto EXIT;
	if(!nlsIsStrNull(PartNoVal)) PartNoValDup=nlsStrDup(PartNoVal);
	printf("\n  PartNoValDup:%s \n",PartNoValDup); fflush(stdout);	

	if(dstat = objGetAttribute(TCPartObjP,RevisionAttr,&PartNoRev))goto EXIT;
	if(!nlsIsStrNull(PartNoRev)) PartNoRevDup=nlsStrDup(PartNoRev);
	printf("\n  PartNoRevDup:%s \n",PartNoRevDup); fflush(stdout);	

	if(dstat = objGetAttribute(TCPartObjP,SequenceAttr,&PartNoSeq))goto EXIT;
	if(!nlsIsStrNull(PartNoSeq)) PartNoSeqDup=nlsStrDup(PartNoSeq);
	printf("\n  PartNoSeqDup:%s \n",PartNoSeqDup); fflush(stdout);

	if(dstat = objGetAttribute(TCPartObjP,OrganizationIDAttr,&PartNoOrgID))goto EXIT;
	if(!nlsIsStrNull(PartNoOrgID)) PartNoOrgIDDup=nlsStrDup(PartNoOrgID);
	printf("\n  PartNoOrgIDDup:%s \n",PartNoOrgIDDup); fflush(stdout);


	if(dstat = oiSqlCreateSelect(&sqlPtr)) goto EXIT ;
	if(dstat = oiSqlWhereBegParen(sqlPtr)) goto EXIT ;
	if(dstat = oiSqlWhereEQ(sqlPtr,LeftAttr,PartObidValDup)) goto EXIT ;
	if(dstat = oiSqlWhereEndParen(sqlPtr)) goto EXIT ;
	if(dstat = oiSqlWhereAND(sqlPtr)) goto EXIT;
	if(dstat = oiSqlWhereIsNotNull(sqlPtr,GenOptionalCS)) goto CLEANUP;	
	if(dstat = QueryDbObject("g2AsmPos",sqlPtr,0,TRUE,SC_SCOPE_OF_SESSION,&g2asmCntObjs,mfail)) goto EXIT;
	if(dstat=(oiSqlPrint(sqlPtr )))goto EXIT;
	if(sqlPtr) oiSqlDispose(sqlPtr); sqlPtr = NULL;
	printf("\n In side optional CS query in g2asmpos :[%d]\n",setSize(g2asmCntObjs));fflush(stdout);
	if(setSize(g2asmCntObjs)>0)
	{
		for(c=0;c<setSize(g2asmCntObjs);c++)
		{
			
			instanceName1=NULL;
			instanceName1Dup=NULL;

			if(dstat = objGetAttribute(setGet(g2asmCntObjs,c),"g0InstanceName",&instanceName1)) goto CLEANUP;
			if (!nlsIsStrNull(instanceName1)) instanceName1Dup = nlsStrDup(instanceName1);

			OPCSVal=NULL;
			OPCSValDup=NULL;

			if(dstat = objGetAttribute(setGet(g2asmCntObjs,c),t5CarOptionalCSAttr,&OPCSVal)) goto CLEANUP;
			if (!nlsIsStrNull(OPCSVal)) OPCSValDup = nlsStrDup(OPCSVal);

			rightOBID=NULL;
			rightOBIDDup=NULL;
			setOfPartsObjs=NULL;

			if(dstat = objGetAttribute(setGet(g2asmCntObjs,c),RightAttr,&rightOBID)) goto CLEANUP;
			if (!nlsIsStrNull(rightOBID)) rightOBIDDup = nlsStrDup(rightOBID);

			if(dstat=(oiSqlCreateSelect(&d_sql))) goto EXIT;
			if(dstat=(oiSqlWhereEQ(d_sql,ItemMstrOBIDAttr,rightOBIDDup))) goto EXIT;
			if(dstat = oiSqlDescOrder(d_sql,CreationDateAttr)) goto EXIT;
			if(dstat=(QueryDbObject(PartClass,d_sql,0,TRUE,SC_SCOPE_OF_SESSION,&setOfPartsObjs,mfail)))  goto EXIT;
			if(dstat=(oiSqlPrint(d_sql))) goto EXIT;
			if(d_sql) oiSqlDispose(d_sql); d_sql = NULL;
			printf("\n size of setOfPartsObjs is ::%d \n",setSize(setOfPartsObjs));fflush(stdout);
			if(setSize(setOfPartsObjs)>0)
			{
				childPartObj=NULL;
				PrtNumberVal=NULL;
				PrtNumberValDup=NULL;
				childPartObj=setGet(setOfPartsObjs,0);
														
				if(dstat = objGetAttribute(childPartObj,"PartNumber",&PrtNumberVal)) goto CLEANUP;
				if (!nlsIsStrNull(PrtNumberVal)) PrtNumberValDup = nlsStrDup(PrtNumberVal);
				printf("\n PrtNumberValDup %s",PrtNumberValDup); fflush(stdout);
			
			}

			printf("\n OPCSValDup:[%s]:[%s]:[%s]:[%s]:[%s]:[%s]:[%s]\n",PartNoValDup,PartNoRevDup,PartNoSeqDup,instanceName1Dup,OPCSValDup,rightOBIDDup,PrtNumberValDup);fflush(stdout);

			if (!nlsIsStrNull(PrtNumberValDup))
			{
				fprintf(fp_DMLOPCSInfo,"%s",PartNoValDup);
				fprintf(fp_DMLOPCSInfo,"^");
				fprintf(fp_DMLOPCSInfo,"%s",PartNoRevDup);
				fprintf(fp_DMLOPCSInfo,"^");
				fprintf(fp_DMLOPCSInfo,"%s",PartNoSeqDup);
				fprintf(fp_DMLOPCSInfo,"^");
				fprintf(fp_DMLOPCSInfo,"%s",PartNoOrgIDDup);
				fprintf(fp_DMLOPCSInfo,"^");
				fprintf(fp_DMLOPCSInfo,"%s",PrtNumberValDup);			
				fprintf(fp_DMLOPCSInfo,"^");
				fprintf(fp_DMLOPCSInfo,"%s",OPCSValDup);
				fprintf(fp_DMLOPCSInfo,"^");
				fprintf(fp_DMLOPCSInfo,"\n");
			}

			printf("\n After printing OPCSValDup:[%s]:[%s]:[%s]:[%s]:[%s]:[%s]:[%s]\n",PartNoValDup,PartNoRevDup,PartNoSeqDup,instanceName1Dup,OPCSValDup,rightOBIDDup,PrtNumberValDup);fflush(stdout);

		
		}
	
	
	
	
	}


CLEANUP:
		t5PrintCleanUpModName;
		

EXIT:
		t5CheckDstatAndReturn;

}
//FUNCTION TO GET APL PART INFO WHICH ARE REQUIRED FOR PART CREATION IN TCUA
status GetTCPartInfoForAPL(ObjectPtr TCPartObjP,string PartOrgID,string csIndicator,string StoreLocation,string configCtxt,FILE* fp_DMLPartInfo,integer* mfail )
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
		
	ObjectPtr		genContextObj	= NULL;
	ObjectPtr		contextObj		= NULL;
	
	SetOfObjects	aplExpObj		= NULL;
	SetOfObjects	aplrelObjSet	= NULL;
	SetOfObjects	stdExpObj		= NULL;
	SetOfObjects	stdrelObjSet	= NULL;
	SetOfObjects	partMasterSO=NULL;
	SetOfObjects	partMasterRelSO=NULL;
	ObjectPtr		MasterObjOP=NULL;

    SetOfObjects	soTaskObjs	= NULL;
	//SetOfObjects	soDmlObjs	= NULL;
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

	if(nlsStrCmp(PartOrgID,"ERC")==0)
	{
			
		if(dstat=objGetAttribute(TCPartObjP,PartNumberAttr,&t5PartNumber))goto EXIT;
		if(!nlsIsStrNull(t5PartNumber))t5PartNumberDup=nlsStrDup(t5PartNumber);

		if(dstat=objGetAttribute(TCPartObjP,RevisionAttr,&t5PartNumberRev))goto EXIT;
		if(!nlsIsStrNull(t5PartNumberRev))t5PartNumberRevDup=nlsStrDup(t5PartNumberRev);

		if(dstat=objGetAttribute(TCPartObjP,SequenceAttr,&t5PartNumberSeq))goto EXIT;
		if(!nlsIsStrNull(t5PartNumberSeq))t5PartNumberSeqDup=nlsStrDup(t5PartNumberSeq);

		if(dstat=objGetAttribute(TCPartObjP,csIndicator,&t5CarMakeBuyInd))goto EXIT;
		if(!nlsIsStrNull(t5CarMakeBuyInd))t5CarMakeBuyIndDup=nlsStrDup(t5CarMakeBuyInd);

		if(dstat=objGetAttribute(TCPartObjP,StoreLocation,&t5CarStLoc))goto EXIT;
		if(!nlsIsStrNull(t5CarStLoc))t5CarStLocDup=nlsStrDup(t5CarStLoc);	

		//if(dstat=objGetAttribute(TCPartObjP,LifeCycleStateAttr,&t5PartNumberLCS))goto EXIT;
		//if(!nlsIsStrNull(t5PartNumberLCS))t5PartNumberLCSDup=nlsStrDup(t5PartNumberLCS);

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

		printf("\n Inside ERC PartDetials :%s,%s,%s,%s,%s,%s,%s",t5PartNumberDup,t5PartNumberRevDup,t5PartNumberSeqDup,
			t5CarMakeBuyIndDup,t5CarStLocDup,t5PartNumberLCSDup,t5PartOrgIDDup);fflush(stdout);


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

		if(!nlsIsStrNull(t5CarMakeBuyIndDup))		//PARTNUMBER CARMAKEBUY;
		{
			fprintf(fp_DMLPartInfo,"%s",t5CarMakeBuyIndDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo," ^");
		}

		if(!nlsIsStrNull(t5CarStLocDup))			//PARTNUMBER CAR STORE LOCATION;
		{
			fprintf(fp_DMLPartInfo,"%s",t5CarStLocDup);
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

		if(!nlsIsStrNull(APLReveffDateFromDup))			//PARTNUMBER APL EFF FROM DATE;
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
	else
	{
	
	
		if(dstat=objGetAttribute(TCPartObjP,PartNumberAttr,&t5PartNumber))goto EXIT;
		if(!nlsIsStrNull(t5PartNumber))t5PartNumberDup=nlsStrDup(t5PartNumber);

		if(dstat=objGetAttribute(TCPartObjP,RevisionAttr,&t5PartNumberRev))goto EXIT;
		if(!nlsIsStrNull(t5PartNumberRev))t5PartNumberRevDup=nlsStrDup(t5PartNumberRev);

		if(dstat=objGetAttribute(TCPartObjP,SequenceAttr,&t5PartNumberSeq))goto EXIT;
		if(!nlsIsStrNull(t5PartNumberSeq))t5PartNumberSeqDup=nlsStrDup(t5PartNumberSeq);

		if(dstat=objGetAttribute(TCPartObjP,csIndicator,&t5CarMakeBuyInd))goto EXIT;
		if(!nlsIsStrNull(t5CarMakeBuyInd))t5CarMakeBuyIndDup=nlsStrDup(t5CarMakeBuyInd);

		if(dstat=objGetAttribute(TCPartObjP,StoreLocation,&t5CarStLoc))goto EXIT;
		if(!nlsIsStrNull(t5CarStLoc))t5CarStLocDup=nlsStrDup(t5CarStLoc);	

		//if(dstat=objGetAttribute(TCPartObjP,LifeCycleStateAttr,&t5PartNumberLCS))goto EXIT;
		//if(!nlsIsStrNull(t5PartNumberLCS))t5PartNumberLCSDup=nlsStrDup(t5PartNumberLCS);

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

			//APLReveffDateFromDup = nlsStrDup("2018/04/29");
			//APLReveffDateToDup = nlsStrDup("9999/12/31");
		}
		
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
			
			fprintf(fp_DMLPartInfo,"0");//(level);
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

			if(!nlsIsStrNull(t5CarMakeBuyIndDup))		//PARTNUMBER CARMAKEBUY;
			{
				fprintf(fp_DMLPartInfo,"%s",t5CarMakeBuyIndDup);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(!nlsIsStrNull(t5CarStLocDup))			//PARTNUMBER CAR STORE LOCATION;
			{
				fprintf(fp_DMLPartInfo,"%s",t5CarStLocDup);
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

			if(!nlsIsStrNull(APLReveffDateFromDup))			//PARTNUMBER APL EFF FROM DATE;
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
			
			if(dstat =objGetAttribute(TCPartObjP,NomenclatureAttr,&DescriptionVal))goto EXIT;
			if(!nlsIsStrNull(DescriptionVal))	 DescriptionValDup=nlsStrDup(DescriptionVal);
			if(!nlsIsStrNull(DescriptionValDup)) DescriptionValDup=low_strssra(DescriptionValDup,"\n"," ");	

			if(!nlsIsStrNull(DescriptionValDup))
			{
				fprintf(fp_DMLPartInfo,"%s",DescriptionValDup);		//DESCRPTION
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}
		
			if(dstat =objGetAttribute(TCPartObjP,PartTypeAttr,&PartTypeVal))goto EXIT;
			if(!nlsIsStrNull(PartTypeVal)) PartTypeValDup=nlsStrDup(PartTypeVal);

			if(!nlsIsStrNull(PartTypeValDup))
			{
				fprintf(fp_DMLPartInfo,"1^ASSY_TYPE_UA^%s^",PartTypeValDup);//PARTTYPE;
			}
			else
			{
				fprintf(fp_DMLPartInfo,"1^ASSY_TYPE_UA^%s^","");//(PARTTYPE);
			}
		
			if(dstat =objGetAttribute(TCPartObjP,ProjectNameAttr,&ProjectCodeDupS))goto EXIT;
			if(!nlsIsStrNull(ProjectCodeDupS)) ProjectCodeS=nlsStrDup(ProjectCodeDupS);

			if(!nlsIsStrNull(ProjectCodeS))		//(PROJECT CODE);
			{
				fprintf(fp_DMLPartInfo,"%s",ProjectCodeS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}
		
			if(dstat =objGetAttribute(TCPartObjP,t5DesignGroupAttr,&DesignGroupDupS))goto EXIT;
			if(!nlsIsStrNull(DesignGroupDupS)) DesignGroupS=nlsStrDup(DesignGroupDupS);

			if(!nlsIsStrNull(DesignGroupS))			//(Design GRP);
			{
				fprintf(fp_DMLPartInfo,"%s",DesignGroupS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5DocRemarksAttr,&ModDescriptionDupS))goto EXIT;
			if(!nlsIsStrNull(ModDescriptionDupS)) ModDescriptionS=nlsStrDup(ModDescriptionDupS);
			if(!nlsIsStrNull(ModDescriptionS)) ModDescriptionS=low_strssra(ModDescriptionS,"\n"," ");

			if(!nlsIsStrNull(ModDescriptionS))
			{
				fprintf(fp_DMLPartInfo,"%s",ModDescriptionS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}
			
			if(dstat =objGetAttribute(TCPartObjP,t5DrawingNoAttr,&DrawingNoDupS))goto EXIT;
			if(!nlsIsStrNull(DrawingNoDupS)) DrawingNoS=nlsStrDup(DrawingNoDupS);

			if(!nlsIsStrNull(DrawingNoS))
			{
				fprintf(fp_DMLPartInfo,"%s",DrawingNoS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}
			
			if(dstat =objGetAttribute(TCPartObjP,t5DrawingIndAttr,&DrawingIndDupS))goto EXIT;
			if(!nlsIsStrNull(DrawingIndDupS)) DrawingIndS=nlsStrDup(DrawingIndDupS);

			if(!nlsIsStrNull(DrawingIndS))
			{
				fprintf(fp_DMLPartInfo,"%s",DrawingIndS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5MatlClassAttr,&MaterialDupS))goto EXIT;
			if(!nlsIsStrNull(MaterialDupS)) MaterialS=nlsStrDup(MaterialDupS);

			if(!nlsIsStrNull(MaterialS)) MaterialS=low_strssra(MaterialS,"\n"," ");

			if(!nlsIsStrNull(MaterialS))
			{
				fprintf(fp_DMLPartInfo,"%s",MaterialS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5MaterialAttr,&MaterialInDrwDupS))goto EXIT;
			if(!nlsIsStrNull(MaterialInDrwDupS)) MaterialInDrwS=nlsStrDup(MaterialInDrwDupS);

			if(!nlsIsStrNull(MaterialInDrwS)) MaterialInDrwS=low_strssra(MaterialInDrwS,"\n"," ");

			if(!nlsIsStrNull(MaterialInDrwS))
			{
				fprintf(fp_DMLPartInfo,"%s",MaterialInDrwS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5MThicknessAttr,&MaterialThickNessDupS))goto EXIT;
			if(!nlsIsStrNull(MaterialThickNessDupS)) MaterialThickNessS=nlsStrDup(MaterialThickNessDupS);

			if(!nlsIsStrNull(MaterialThickNessS))
			{
				fprintf(fp_DMLPartInfo,"%s",MaterialThickNessS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
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
				fprintf(fp_DMLPartInfo,"%s",LeftRhS);
				fprintf(fp_DMLPartInfo,"^");
			}else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(MasterObjOP,DefaultUnitOfMeasureAttr,&UnitOfMeasureDupS))goto EXIT;
			if(!nlsIsStrNull(UnitOfMeasureDupS)) UnitOfMeasureS=nlsStrDup(UnitOfMeasureDupS);

			if(dstat =objGetAttribute(TCPartObjP,t5DsgnOwnAttr,&DeignOwnUnitDupS))goto EXIT;
			if(!nlsIsStrNull(DeignOwnUnitDupS)) DeignOwnUnitS=nlsStrDup(DeignOwnUnitDupS);

			if(!nlsIsStrNull(DeignOwnUnitS))
			{
				fprintf(fp_DMLPartInfo,"%s",DeignOwnUnitS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5ModelIndicatorAttr,&ModelIndDupS))goto EXIT;
			if(!nlsIsStrNull(ModelIndDupS)) ModelIndS=nlsStrDup(ModelIndDupS);

			if(!nlsIsStrNull(ModelIndS))
			{
				fprintf(fp_DMLPartInfo,"%s",ModelIndS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}


			
			if(!nlsIsStrNull(UnitOfMeasureS))
			{
				fprintf(fp_DMLPartInfo,"%s",UnitOfMeasureS);
				fprintf(fp_DMLPartInfo,"^");
			}else
			{
					fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5ColourIndAttr,&ColourIndDupS))goto EXIT;
			if(!nlsIsStrNull(ColourIndDupS)) ColourIndS=nlsStrDup(ColourIndDupS);

			if(!nlsIsStrNull(ColourIndS))
			{
				fprintf(fp_DMLPartInfo,"%s",ColourIndS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,t5WeightAttr,&WeightDupS))goto EXIT;
			if(!nlsIsStrNull(WeightDupS)) WeightS=nlsStrDup(WeightDupS);

			if(!nlsIsStrNull(WeightS))
			{
				fprintf(fp_DMLPartInfo,"%s",WeightS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5CMVRCertificationReqd",&t5CMVRCertificationDupS))goto EXIT;
			if(!nlsIsStrNull(t5CMVRCertificationDupS)) t5CMVRCertificationS=nlsStrDup(t5CMVRCertificationDupS);

			if(!nlsIsStrNull(t5CMVRCertificationS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5CMVRCertificationS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ClassificationHazardous",&t5ClassificationHazDupS))goto EXIT;
			if(!nlsIsStrNull(t5ClassificationHazDupS)) t5ClassificationHazS=nlsStrDup(t5ClassificationHazDupS);

			if(!nlsIsStrNull(t5ClassificationHazS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ClassificationHazS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ColourID",&t5ColourIDDupS))goto EXIT;
			if(!nlsIsStrNull(t5ColourIDDupS)) t5ColourIDS=nlsStrDup(t5ColourIDDupS);

			if(!nlsIsStrNull(t5ColourIDS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ColourIDS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ConfigID",&t5ConfigIDDupS))goto EXIT;
			if(!nlsIsStrNull(t5ConfigIDDupS)) t5ConfigIDS=nlsStrDup(t5ConfigIDDupS);

			if(!nlsIsStrNull(t5ConfigIDS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ConfigIDS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Dismantable",&t5DismantableDupS))goto EXIT;
			if(!nlsIsStrNull(t5DismantableDupS)) t5DismantableS=nlsStrDup(t5DismantableDupS);

			if(!nlsIsStrNull(t5DismantableS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5DismantableS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5DsgnDept",&t5DsgnDeptDupS))goto EXIT;
			if(!nlsIsStrNull(t5DsgnDeptDupS)) t5DsgnDeptS=nlsStrDup(t5DsgnDeptDupS);

			if(!nlsIsStrNull(t5DsgnDeptS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5DsgnDeptS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5EnvelopeDimensions",&t5EnvelopeDimenDupS))goto EXIT;
			if(!nlsIsStrNull(t5EnvelopeDimenDupS)) t5EnvelopeDimenS=nlsStrDup(t5EnvelopeDimenDupS);

			if(!nlsIsStrNull(t5EnvelopeDimenS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5EnvelopeDimenS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5HazardousContents",&t5HazardousContDupS))goto EXIT;
			if(!nlsIsStrNull(t5HazardousContDupS)) t5HazardousContS=nlsStrDup(t5HazardousContDupS);

			if(!nlsIsStrNull(t5HazardousContS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5HazardousContS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5HomologationReqd",&t5HomologationReqdDupS))goto EXIT;
			if(!nlsIsStrNull(t5HomologationReqdDupS)) t5HomologationReqdS=nlsStrDup(t5HomologationReqdDupS);

			if(!nlsIsStrNull(t5HomologationReqdS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5HomologationReqdS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ListRecSpares",&t5ListRecSparesDups))goto EXIT;
			if(!nlsIsStrNull(t5ListRecSparesDups)) t5ListRecSparess=nlsStrDup(t5ListRecSparesDups);

			if(!nlsIsStrNull(t5ListRecSparess))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ListRecSparess);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5NcPartNo",&t5NcPartNoDupS))goto EXIT;
			if(!nlsIsStrNull(t5NcPartNoDupS)) t5NcPartNoS=nlsStrDup(t5NcPartNoDupS);

			if(!nlsIsStrNull(t5NcPartNoS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5NcPartNoS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PartCode",&t5PartCodeDupS))goto EXIT;
			if(!nlsIsStrNull(t5PartCodeDupS)) t5PartCodeS=nlsStrDup(t5PartCodeDupS);

			if(!nlsIsStrNull(t5PartCodeS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PartCodeS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PartProperty",&t5PartPropertyDupS))goto EXIT;
			if(!nlsIsStrNull(t5PartPropertyDupS)) t5PartPropertyS=nlsStrDup(t5PartPropertyDupS);

			if(!nlsIsStrNull(t5PartPropertyS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PartPropertyS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PartStatus",&t5PartStatusDupS))goto EXIT;
			if(!nlsIsStrNull(t5PartStatusDupS)) t5PartStatusS=nlsStrDup(t5PartStatusDupS);

			if(!nlsIsStrNull(t5PartStatusS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PartStatusS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PkgStd",&t5PkgStdDupS))goto EXIT;
			if(!nlsIsStrNull(t5PkgStdDupS)) t5PkgStdS=nlsStrDup(t5PkgStdDupS);

			if(!nlsIsStrNull(t5PkgStdS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PkgStdS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Product",&t5ProductDupS))goto EXIT;
			if(!nlsIsStrNull(t5ProductDupS)) t5ProductS=nlsStrDup(t5ProductDupS);

			if(!nlsIsStrNull(t5ProductS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ProductS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PrtCatCode",&t5PrtCatCodeDupS))goto EXIT;
			if(!nlsIsStrNull(t5PrtCatCodeDupS)) t5PrtCatCodeS=nlsStrDup(t5PrtCatCodeDupS);

			if(!nlsIsStrNull(t5PrtCatCodeS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PrtCatCodeS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Recoverable",&t5RecoverableDupS))goto EXIT;
			if(!nlsIsStrNull(t5RecoverableDupS)) t5RecoverableS=nlsStrDup(t5RecoverableDupS);

			if(!nlsIsStrNull(t5RecoverableS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5RecoverableS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Recyclability",&t5RecyclabilityDupS))goto EXIT;
			if(!nlsIsStrNull(t5RecyclabilityDupS)) t5RecyclabilityS=nlsStrDup(t5RecyclabilityDupS);

			if(!nlsIsStrNull(t5RecyclabilityS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5RecyclabilityS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RefPartNumber",&t5RefPartNumberDupS))goto EXIT;
			if(!nlsIsStrNull(t5RefPartNumberDupS)) t5RefPartNumberS=nlsStrDup(t5RefPartNumberDupS);

			if(!nlsIsStrNull(t5RefPartNumberS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5RefPartNumberS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Reliability",&t5ReliabilityDupS))goto EXIT;
			if(!nlsIsStrNull(t5ReliabilityDupS)) t5ReliabilityS=nlsStrDup(t5ReliabilityDupS);

			if(!nlsIsStrNull(t5ReliabilityS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ReliabilityS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SpareCriteria",&t5SpareCriteriaDupS))goto EXIT;
			if(!nlsIsStrNull(t5SpareCriteriaDupS)) t5SpareCriteriaS=nlsStrDup(t5SpareCriteriaDupS);

			if(!nlsIsStrNull(t5SpareCriteriaS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5SpareCriteriaS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SpareInd",&t5SpareIndDupS))goto EXIT;
			if(!nlsIsStrNull(t5SpareIndDupS)) t5SpareIndS=nlsStrDup(t5SpareIndDupS);

			if(!nlsIsStrNull(t5SpareIndS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5SpareIndS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}
			

			if(dstat =objGetAttribute(TCPartObjP,"t5SurfPrtStd",&t5SurfPrtStdDupS))goto EXIT;
			if(!nlsIsStrNull(t5SurfPrtStdDupS)) t5SurfPrtStdS=nlsStrDup(t5SurfPrtStdDupS);

			if(!nlsIsStrNull(t5SurfPrtStdS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5SurfPrtStdS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SamplesToAppr",&t5SamplesToApprDupS))goto EXIT;
			if(!nlsIsStrNull(t5SamplesToApprDupS)) t5SamplesToApprS=nlsStrDup(t5SamplesToApprDupS);

			if(!nlsIsStrNull(t5SamplesToApprS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5SamplesToApprS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AsmDisposalInstr",&t5AsmDisposalDupS))goto EXIT;
			if(!nlsIsStrNull(t5AsmDisposalDupS)) t5AsmDisposalS=nlsStrDup(t5AsmDisposalDupS);

			if(!nlsIsStrNull(t5AsmDisposalS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5AsmDisposalS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5FinDisposalInstr",&t5FinDisposalInstrDupS))goto EXIT;
			if(!nlsIsStrNull(t5FinDisposalInstrDupS)) t5FinDisposalInstrS=nlsStrDup(t5FinDisposalInstrDupS);

			if(!nlsIsStrNull(t5FinDisposalInstrS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5FinDisposalInstrS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RPDisposalInstr",&t5RPDisposalInstrDupS))goto EXIT;
			if(!nlsIsStrNull(t5RPDisposalInstrDupS)) t5RPDisposalInstrS=nlsStrDup(t5RPDisposalInstrDupS);

			if(!nlsIsStrNull(t5RPDisposalInstrS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5RPDisposalInstrS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SPDisposalInstr",&t5SPDisposalInstrDupS))goto EXIT;
			if(!nlsIsStrNull(t5SPDisposalInstrDupS)) t5SPDisposalInstrS=nlsStrDup(t5SPDisposalInstrDupS);

			if(!nlsIsStrNull(t5SPDisposalInstrS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5SPDisposalInstrS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5WIPDisposalInstr",&t5WIPDisposalInstrDupS))goto EXIT;
			if(!nlsIsStrNull(t5WIPDisposalInstrDupS)) t5WIPDisposalInstrS=nlsStrDup(t5WIPDisposalInstrDupS);

			if(!nlsIsStrNull(t5WIPDisposalInstrS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5WIPDisposalInstrS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5LastModBy",&t5LastModByDupS))goto EXIT;
			if(!nlsIsStrNull(t5LastModByDupS)) t5LastModByS=nlsStrDup(t5LastModByDupS);

			if(!nlsIsStrNull(t5LastModByS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5LastModByS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5VerCreator",&t5VerCreatorDupS))goto EXIT;
			if(!nlsIsStrNull(t5VerCreatorDupS)) t5VerCreatorS=nlsStrDup(t5VerCreatorDupS);

			if(!nlsIsStrNull(t5VerCreatorS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5VerCreatorS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5CAEDocE",&t5CAEDocEDupS))goto EXIT;
			if(!nlsIsStrNull(t5CAEDocEDupS)) t5CAEDocES=nlsStrDup(t5CAEDocEDupS);

			if(!nlsIsStrNull(t5CAEDocES))
			{
				fprintf(fp_DMLPartInfo,"%s",t5CAEDocES);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Coated",&t5CoatedDupS))goto EXIT;
			if(!nlsIsStrNull(t5CoatedDupS)) t5CoatedS=nlsStrDup(t5CoatedDupS);

			if(!nlsIsStrNull(t5CoatedS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5CoatedS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ConvDoc",&t5ConvDocDupS))goto EXIT;
			if(!nlsIsStrNull(t5ConvDocDupS)) t5ConvDocS=nlsStrDup(t5ConvDocDupS);

			if(!nlsIsStrNull(t5ConvDocS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ConvDocS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AplCopyOfErcRev",&t5AplCopyOfErcRevDupS))goto EXIT;
			if(!nlsIsStrNull(t5AplCopyOfErcRevDupS)) t5AplCopyOfErcRevS=nlsStrDup(t5AplCopyOfErcRevDupS);

			if(!nlsIsStrNull(t5AplCopyOfErcRevS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5AplCopyOfErcRevS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AplCopyOfErcSeq",&t5AplCopyOfErcSeqDupS))goto EXIT;
			if(!nlsIsStrNull(t5AplCopyOfErcSeqDupS)) t5AplCopyOfErcSeqS=nlsStrDup(t5AplCopyOfErcSeqDupS);

			if(!nlsIsStrNull(t5AplCopyOfErcSeqS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5AplCopyOfErcSeqS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AppCode",&t5AppCodeDupS))goto EXIT;
			if(!nlsIsStrNull(t5AppCodeDupS)) t5AppCodeS=nlsStrDup(t5AppCodeDupS);

			if(!nlsIsStrNull(t5AppCodeS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5AppCodeS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RqstNum",&t5RqstNumDupS))goto EXIT;
			if(!nlsIsStrNull(t5RqstNumDupS)) t5RqstNumS=nlsStrDup(t5RqstNumDupS);

			if(!nlsIsStrNull(t5RqstNumS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5RqstNumS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RsnCode",&t5RsnCodeDupS))goto EXIT;
			if(!nlsIsStrNull(t5RsnCodeDupS)) t5RsnCodeS=nlsStrDup(t5RsnCodeDupS);

			if(!nlsIsStrNull(t5RsnCodeS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5RsnCodeS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SurfaceArea",&t5SurfaceAreaDupS))goto EXIT;
			if(!nlsIsStrNull(t5SurfaceAreaDupS)) t5SurfaceAreaS=nlsStrDup(t5SurfaceAreaDupS);

			if(!nlsIsStrNull(t5SurfaceAreaS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5SurfaceAreaS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5Volume",&t5VolumeDupS))goto EXIT;
			if(!nlsIsStrNull(t5VolumeDupS)) t5VolumeS=nlsStrDup(t5VolumeDupS);

			if(!nlsIsStrNull(t5VolumeS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5VolumeS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ErcIndName",&t5ErcIndNameDupS))goto EXIT;
			if(!nlsIsStrNull(t5ErcIndNameDupS)) t5ErcIndNameS=nlsStrDup(t5ErcIndNameDupS);

			
			if(!nlsIsStrNull(t5ErcIndNameS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ErcIndNameS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PostRelReq",&t5PostRelReqDupS))goto EXIT;
			if(!nlsIsStrNull(t5PostRelReqDupS)) t5PostRelReqS=nlsStrDup(t5PostRelReqDupS);

			if(!nlsIsStrNull(t5PostRelReqS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PostRelReqS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ItmCategory",&t5ItmCategoryDupS))goto EXIT;
			if(!nlsIsStrNull(t5ItmCategoryDupS)) t5ItmCategoryS=nlsStrDup(t5ItmCategoryDupS);

			if(!nlsIsStrNull(t5ItmCategoryS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ItmCategoryS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5CopReq",&t5CopReqDupS))goto EXIT;
			if(!nlsIsStrNull(t5CopReqDupS)) t5CopReqS=nlsStrDup(t5CopReqDupS);

			if(!nlsIsStrNull(t5CopReqS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5CopReqS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AplInvalidate",&t5AplInvalidateDupS))goto EXIT;
			if(!nlsIsStrNull(t5AplInvalidateDupS)) t5AplInvalidateS=nlsStrDup(t5AplInvalidateDupS);

			if(!nlsIsStrNull(t5AplInvalidateS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5AplInvalidateS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PrtValiStatus",&t5PrtValiStatusDupS))goto EXIT;
			if(!nlsIsStrNull(t5PrtValiStatusDupS)) t5PrtValiStatusS=nlsStrDup(t5PrtValiStatusDupS);

			if(!nlsIsStrNull(t5PrtValiStatusS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PrtValiStatusS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5DRSubState",&t5DRSubStateDupS))goto EXIT;
			if(!nlsIsStrNull(t5DRSubStateDupS)) t5DRSubStateS=nlsStrDup(t5DRSubStateDupS);

			if(!nlsIsStrNull(t5DRSubStateS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5DRSubStateS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5KnxtDocInd",&t5KnxtDocIndDupS))goto EXIT;
			if(!nlsIsStrNull(t5KnxtDocIndDupS)) t5KnxtDocIndS=nlsStrDup(t5KnxtDocIndDupS);

			if(!nlsIsStrNull(t5KnxtDocIndS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5KnxtDocIndS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5SimVal",&t5SimValDupS))goto EXIT;
			if(!nlsIsStrNull(t5SimValDupS)) t5SimValS=nlsStrDup(t5SimValDupS);

			if(!nlsIsStrNull(t5SimValS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5SimValS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PerYield",&t5PerYieldDupS))goto EXIT;
			if(!nlsIsStrNull(t5PerYieldDupS)) t5PerYieldS=nlsStrDup(t5PerYieldDupS);

			if(!nlsIsStrNull(t5PerYieldS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PerYieldS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5AltPartNo",&t5AltPartNoDupS))goto EXIT;
			if(!nlsIsStrNull(t5AltPartNoDupS)) t5AltPartNoS=nlsStrDup(t5AltPartNoDupS);

			if(!nlsIsStrNull(t5AltPartNoS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5AltPartNoS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5RolledupWt",&t5RolledupWtDupS))goto EXIT;
			if(!nlsIsStrNull(t5RolledupWtDupS)) t5RolledupWtS=nlsStrDup(t5RolledupWtDupS);

			if(!nlsIsStrNull(t5RolledupWtS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5RolledupWtS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5EstSheetReqd",&t5EstSheetReqdDupS))goto EXIT;
			if(!nlsIsStrNull(t5EstSheetReqdDupS)) t5EstSheetReqdS=nlsStrDup(t5EstSheetReqdDupS);

			if(!nlsIsStrNull(t5EstSheetReqdS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5EstSheetReqdS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5PFDModReqd",&t5PFDModReqdDupS))goto EXIT;
			if(!nlsIsStrNull(t5PFDModReqdDupS)) t5PFDModReqdS=nlsStrDup(t5PFDModReqdDupS);

			if(!nlsIsStrNull(t5PFDModReqdS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5PFDModReqdS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"t5ToolIndentReqd",&t5ToolIndentReqdDupS))goto EXIT;
			if(!nlsIsStrNull(t5ToolIndentReqdDupS)) t5ToolIndentReqdS=nlsStrDup(t5ToolIndentReqdDupS);

			if(!nlsIsStrNull(t5ToolIndentReqdDupS))
			{
				fprintf(fp_DMLPartInfo,"%s",t5ToolIndentReqdDupS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}


			if(dstat =objGetAttribute(TCPartObjP,"CategoryName",&CategoryNameDupS))goto EXIT;
			if(!nlsIsStrNull(CategoryNameDupS)) CategoryNameS=nlsStrDup(CategoryNameDupS);

			if(!nlsIsStrNull(CategoryNameS))
			{
				fprintf(fp_DMLPartInfo,"%s",CategoryNameS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"CreationDate",&PartCreDateDupS))goto EXIT;
			if(!nlsIsStrNull(PartCreDateDupS)) PartCreDateS=nlsStrDup(PartCreDateDupS);

			if(!nlsIsStrNull(PartCreDateS))
			{
				fprintf(fp_DMLPartInfo,"%s",PartCreDateS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"LastUpdate",&PartModDateDupS))goto EXIT;
			if(!nlsIsStrNull(PartModDateDupS)) PartModDateS=nlsStrDup(PartModDateDupS);

			if(!nlsIsStrNull(PartModDateS))
			{
				fprintf(fp_DMLPartInfo,"%s",PartModDateS);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo," ^");
			}

			if(dstat =objGetAttribute(TCPartObjP,"Creator",&PartCreatorDup))goto EXIT;
			if(!nlsIsStrNull(PartCreatorDup)) PartCreator=nlsStrDup(PartCreatorDup);

			if(!nlsIsStrNull(PartCreator))
			{
				fprintf(fp_DMLPartInfo,"%s",PartCreator);
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

status GetTCPartBOMDiffForERCAPL(ObjectPtr TCPartObjP,string assyPartNo,string assyRev,string assySeq,string PartOrgIDDup,string configCtxt,FILE *fp_DMLBomList,integer* mfail )
{
	ObjectPtr		cntxtForERCView				= NULL;
	ObjectPtr		cntxtForAPLView				= NULL;
	ObjectPtr		ChildRelOP					= NULL;
	ObjectPtr		revEffObj					= NULL;
	SetOfObjects	soPrtsForERC				= NULL;
	SetOfObjects	soPrtsRelObjForERC			= NULL;
	SetOfObjects	soPrtsForAPL				= NULL;
	SetOfObjects	soPrtsRelObjForAPL			= NULL;
	int				newcount					= 0;
	int				foundnewmatch				= 0;
	string			deletedPart					= NULL;
	string			deletedPartDup				= NULL;
	string			deletedPartRev				= NULL;
	string			deletedPartRevDup			= NULL;
	string			deletedPartSeq				= NULL;
	string			deletedPartSeqDup			= NULL;
	string			deletedPartQty				= NULL;
	string			deletedPartQtyDup			= NULL;	
	string			addedPart					= NULL;
	string			addedPartDup				= NULL;
	string			addedPartRev				= NULL;
	string			addedPartRevDup				= NULL;
	string			addedPartSeq				= NULL;
	string			addedPartSeqDup				= NULL;
	string			addedPartQty				= NULL;
	string			addedPartQtyDup				= NULL;
	string			QuantityDup					= NULL;
	string			Quantity					= NULL;
	//string			matfilename					= NULL;
	string			ChildRelClassNameDupS		= NULL;
	string			ChildRelClassNameS			= NULL;
	string			RelnObid					= NULL;
	string			RelnObidDup					= NULL;
	string			Matrix11Dup					= NULL;
	string			Matrix11					= NULL;
	string			Matrix12Dup					= NULL;
	string			Matrix12= NULL;
	string			Matrix13Dup= NULL;
	string			Matrix13= NULL ;
	string			Matrix14Dup= NULL;
	string			Matrix14= NULL;
	string			Matrix21Dup= NULL;
	string			Matrix21= NULL;
	string			Matrix22Dup= NULL;
	string			Matrix22= NULL;
	string			Matrix23Dup= NULL;
	string			Matrix23= NULL;
	string			Matrix24Dup= NULL;
	string			Matrix24= NULL;
	string			Matrix31Dup= NULL;
	string			Matrix31= NULL;
	string			Matrix32Dup= NULL;
	string			Matrix32= NULL;
	string			Matrix33Dup= NULL;
	string			Matrix33= NULL;
	string			Matrix34Dup= NULL;
	string			Matrix34= NULL;
	string			Matrix41Dup= NULL;
	string			Matrix41= NULL;
	string			Matrix42Dup= NULL;
	string			Matrix42= NULL;
	string			Matrix43Dup= NULL;
	string			Matrix43= NULL;
	string			Matrix44Dup= NULL;
	string			Matrix44= NULL;
	string			Instname= NULL;
	string			Instnamedup= NULL;
	string			ChildPartMQRelTMatrix12S= NULL;
	string			ChildPartMQRelTMatrix13S= NULL;
	string			ChildPartMQRelTMatrix14S= NULL;
	string			ChildPartMQRelTMatrix21S= NULL;
	string			ChildPartMQRelTMatrix22S= NULL;
	string			ChildPartMQRelTMatrix23S= NULL;
	string			ChildPartMQRelTMatrix24S= NULL;
	string			ChildPartMQRelTMatrix31S= NULL;
	string			ChildPartMQRelTMatrix32S= NULL;
	string			ChildPartMQRelTMatrix33S= NULL;
	string			ChildPartMQRelTMatrix34S= NULL;
	string			ChildPartMQRelTMatrix41S= NULL;
	string			ChildPartMQRelTMatrix42S= NULL;
	string			ChildPartMQRelTMatrix43S= NULL;
	string			ChildPartMQRelTMatrix44S= NULL;
	string		LeftPartObjOBIDS	=	NULL ;
	string		RightPartMstrObjOBIDS11	=	NULL ;
		SqlPtr		MultyQtySqlPtr		=	NULL ;
	SetOfObjects	MultyQtyRelPartSO	=	NULL;
	ObjectPtr	RelQtyPartObjP		=	NULL ;
	string		ChildPartQuantityS	=	NULL;
	int		ChildMQPart		=	0;
	string		ChildPartMQRelTMatrix11S=	NULL;
	string indexS=NULL;
	string indexDupS=NULL;
	string newInstancename=NULL;
		int		sizeoftable		=	0;


	//FILE			*matfp						= NULL;
	char decimalQty[10];
	int			qty=0;
	int			z=0;

	t5MethodInit("GetTCPartBOMDiffForERCAPL");

	 printf("\n Input parameter to func [%s]:[%s]:[%s]:[%s]:[%s]:\n", assyPartNo,assyRev,assySeq,configCtxt,PartOrgIDDup);fflush(stdout);
	 cntxtForERCView = setCnxtObjBOMDIff(TCPartObjP,"ERC","GlobalCtxt",mfail);

	 if(dstat = ExpandRelationWithCtxt(AsRevRevClass,TCPartObjP,"PartsInAssembly",cntxtForERCView,SC_SCOPE_OF_SESSION,NULL,&soPrtsForERC,&soPrtsRelObjForERC,mfail));
	 printf("\n Size of soPrtsForERC is %d \n", setSize(soPrtsForERC));fflush(stdout);


	 cntxtForAPLView = setCnxtObjBOMDIff(TCPartObjP,"APL",configCtxt,mfail);

	 if(dstat = ExpandRelationWithCtxt(AsRevRevClass,TCPartObjP,"PartsInAssembly",cntxtForAPLView,SC_SCOPE_OF_SESSION,NULL,&soPrtsForAPL,&soPrtsRelObjForAPL,mfail));
	 printf("\n Size of soPrtsForAPL is %d \n", setSize(soPrtsForAPL));fflush(stdout);

	if(setSize(soPrtsForERC)>0)
	{
		for(newcount=0 ; newcount  < setSize(soPrtsForERC) ; newcount++)
		{
			printf("\n PINSIDE BOM COMPARE...\n");fflush(stdout);
			foundnewmatch=0;
			if(dstat=ulyFindObjectInSet(setGet(soPrtsForERC,newcount), soPrtsForAPL , PartNumberAttr , 0 , &foundnewmatch)) goto EXIT;
			if (foundnewmatch < 0)
			{
				printf("\n PART NOT FOUND IN APL BOM...Deleted from APL BOM\n");fflush(stdout);
				
				deletedPart=NULL;
				deletedPartDup=NULL;

				if(dstat=objGetAttribute(setGet(soPrtsForERC,newcount),PartNumberAttr,&deletedPart)) goto EXIT;
				if (!nlsIsStrNull(deletedPart)) deletedPartDup = nlsStrDup(deletedPart);

				deletedPartRev=NULL;
				deletedPartRevDup=NULL;

				if(dstat=objGetAttribute(setGet(soPrtsForERC,newcount),RevisionAttr,&deletedPartRev)) goto EXIT;
				if (!nlsIsStrNull(deletedPartRev)) deletedPartRevDup = nlsStrDup(deletedPartRev);

				deletedPartSeq=NULL;
				deletedPartSeqDup=NULL;

				if(dstat=objGetAttribute(setGet(soPrtsForERC,newcount),SequenceAttr,&deletedPartSeq)) goto EXIT;
				if (!nlsIsStrNull(deletedPartSeq)) deletedPartSeqDup = nlsStrDup(deletedPartSeq);

				deletedPartQty=NULL;
				deletedPartQtyDup=NULL;

				if(dstat=objGetAttribute(setGet(soPrtsRelObjForERC,newcount),DisplayQuantityAttr,&deletedPartQty)) goto EXIT;
				if (!nlsIsStrNull(deletedPartQty)) deletedPartQtyDup = nlsStrDup(deletedPartQty);
				
				fprintf(fp_DMLBomList,"DELETED");						//(DELETED FROM APLC VIEW);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,PartOrgIDDup);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",assyPartNo);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",assyRev);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",assySeq);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",deletedPartDup);			//PARTNUMBER;								
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",deletedPartRevDup);			//PARTNUMBER REV;
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",deletedPartSeqDup);			//PARTNUMBER SEQ;
				fprintf(fp_DMLBomList,"^");
				//fprintf(fp_DMLBomList,"%s",deletedPartQtyDup);			//PARTNUMBER QTY;
				//fprintf(fp_DMLBomList,"^");
				if(!nlsIsStrNull(deletedPartQtyDup))
				{
					fprintf(fp_DMLBomList,"%s",deletedPartQtyDup);
					fprintf(fp_DMLBomList,"^");
				}
				else
				{
					fprintf(fp_DMLBomList,"1^");
				}

				fprintf(fp_DMLBomList,"\n");

			
			}	
		}
	}


	if(setSize(soPrtsForAPL)>0)
	{
		newcount=0;
		for(newcount=0 ; newcount  < setSize(soPrtsForAPL) ; newcount++)
		{
			foundnewmatch=0;
			if(dstat=ulyFindObjectInSet(setGet(soPrtsForAPL,newcount), soPrtsForERC , PartNumberAttr , 0 , &foundnewmatch)) goto EXIT;
			if (foundnewmatch < 0)
			{
				printf("\n PART NOT FOUND IN ERC BOM...Added in APL BOM\n");fflush(stdout);
				
				addedPart=NULL;
				addedPartDup=NULL;

				if(dstat=objGetAttribute(setGet(soPrtsForAPL,newcount),PartNumberAttr,&addedPart)) goto EXIT;
				if (!nlsIsStrNull(addedPart)) addedPartDup = nlsStrDup(addedPart);

				addedPartRev=NULL;
				addedPartRevDup=NULL;

				if(dstat=objGetAttribute(setGet(soPrtsForAPL,newcount),RevisionAttr,&addedPartRev)) goto EXIT;
				if (!nlsIsStrNull(addedPartRev)) addedPartRevDup = nlsStrDup(addedPartRev);

				addedPartSeq=NULL;
				addedPartSeqDup=NULL;

				if(dstat=objGetAttribute(setGet(soPrtsForAPL,newcount),SequenceAttr,&addedPartSeq)) goto EXIT;
				if (!nlsIsStrNull(addedPartSeq)) addedPartSeqDup = nlsStrDup(addedPartSeq);

				addedPartQty=NULL;
				addedPartQtyDup=NULL;

				if(dstat=objGetAttribute(setGet(soPrtsRelObjForAPL,newcount),DisplayQuantityAttr,&addedPartQty)) goto EXIT;
				if (!nlsIsStrNull(addedPartQty)) addedPartQtyDup = nlsStrDup(addedPartQty);

				printf("%s:%s:%s:%s:",addedPartDup,addedPartRevDup,addedPartSeqDup,addedPartQtyDup);fflush(stdout);
				
				fprintf(fp_DMLBomList,"ADDED");//(ADDED IN APLC VIEW);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,PartOrgIDDup);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",assyPartNo);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",assyRev);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",assySeq);
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",addedPartDup);				//PARTNUMBER;								
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",addedPartRevDup);			//PARTNUMBER REV;
				fprintf(fp_DMLBomList,"^");
				fprintf(fp_DMLBomList,"%s",addedPartSeqDup);			//PARTNUMBER SEQ;
				fprintf(fp_DMLBomList,"^");
				//fprintf(fp_DMLBomList,"%s",addedPartQtyDup);			//PARTNUMBER QTY;
				//fprintf(fp_DMLBomList,"^");
				//fprintf(fp_DMLBomList,"\n");

				if(!nlsIsStrNull(addedPartQtyDup))
				{
					fprintf(fp_DMLBomList,"%s",addedPartQtyDup);
					fprintf(fp_DMLBomList,"^");
				}
				else
				{
					fprintf(fp_DMLBomList,"1^");
				}



				ChildRelOP=setGet(soPrtsRelObjForAPL,newcount);
				//if(dstat=objGetAttribute(ChildRelOP,LeftNameAttr,&LeftPartNumberDupS)) goto EXIT;
				//if(LeftPartNumberDupS)LeftPartNumberS=nlsStrDup(LeftPartNumberDupS);

				//if(dstat=objGetAttribute(ChildRelOP,RightNameAttr,&RightPartNumberDupS)) goto EXIT;
				//if(RightPartNumberDupS)RightPartNumberS=nlsStrDup(RightPartNumberDupS);
				
				if(dstat=objGetAttribute(ChildRelOP,QuantityAttr,&QuantityDup)) goto EXIT;
				if(QuantityDup!=NULL)Quantity=nlsStrDup(QuantityDup);

				//LeftPartNumberS=strtok(LeftPartNumberS,",");
				//RightPartNumberS=strtok(RightPartNumberS,",");

//				if (!nlsIsStrNull(matfilename))nlsStrFree(matfilename);
//				matfilename=nlsStrAlloc(3000);
//				nlsStrCpy(matfilename,assyPartNo);
//				nlsStrCat(matfilename,".");
//				nlsStrCat(matfilename,addedPartDup);
//				nlsStrCat(matfilename,"_BOM.mat.txt");
//
//				matfp=fopen(matfilename,"w");
				t5CheckDstat(objGetAttribute(ChildRelOP,LToRNameAttr,&ChildRelClassNameDupS));
				ChildRelClassNameS=nlsStrDup(ChildRelClassNameDupS);

				t5CheckDstat(objGetObject(ChildRelOP,AssocRelationObjAttr,&revEffObj));
				t5CheckDstat(objGetAttribute(revEffObj,OBIDAttr,&RelnObidDup));
				RelnObid=nlsStrDup(RelnObidDup);

				printf("\n Matrix for [%s][%s][%s]",assyPartNo,addedPartDup,ChildRelClassNameS);

				if(!nlsStrCmp(ChildRelClassNameS,"Uses Parts"))
				{
						printf("\n Uses Parts");
						sprintf(decimalQty,"%d",atoi(Quantity));
						if(nlsStrCmp(decimalQty,Quantity)==0)
						{
							qty=atoi(decimalQty);
						}else{
							qty=1;
						}
						if(dstat=objGetAttribute(ChildRelOP,TMatrix11Attr,&Matrix11Dup)) goto EXIT;
						if(Matrix11Dup!=NULL)
						Matrix11=nlsStrDup(Matrix11Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix12Attr,&Matrix12Dup)) goto EXIT;
						if(Matrix12Dup!=NULL)
						Matrix12=nlsStrDup(Matrix12Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix13Attr,&Matrix13Dup)) goto EXIT;
						if(Matrix13Dup!=NULL)
						Matrix13=nlsStrDup(Matrix13Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix14Attr,&Matrix14Dup)) goto EXIT;
						if(Matrix14Dup!=NULL)
						Matrix14=nlsStrDup(Matrix14Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix21Attr,&Matrix21Dup)) goto EXIT;
						if(Matrix21Dup!=NULL)
						Matrix21=nlsStrDup(Matrix21Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix22Attr,&Matrix22Dup)) goto EXIT;
						if(Matrix22Dup!=NULL)
						Matrix22=nlsStrDup(Matrix22Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix23Attr,&Matrix23Dup)) goto EXIT;
						if(Matrix23Dup!=NULL)
						Matrix23=nlsStrDup(Matrix23Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix24Attr,&Matrix24Dup)) goto EXIT;
						if(Matrix24Dup!=NULL)
						Matrix24=nlsStrDup(Matrix24Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix31Attr,&Matrix31Dup)) goto EXIT;
						if(Matrix31Dup!=NULL)
						Matrix31=nlsStrDup(Matrix31Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix32Attr,&Matrix32Dup)) goto EXIT;
						if(Matrix32Dup!=NULL)
						Matrix32=nlsStrDup(Matrix32Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix33Attr,&Matrix33Dup)) goto EXIT;
						if(Matrix33Dup!=NULL)
						Matrix33=nlsStrDup(Matrix33Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix34Attr,&Matrix34Dup)) goto EXIT;
						if(Matrix34Dup!=NULL)
						Matrix34=nlsStrDup(Matrix34Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix41Attr,&Matrix41Dup)) goto EXIT;
						if(Matrix41Dup!=NULL)
						Matrix41=nlsStrDup(Matrix41Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix42Attr,&Matrix42Dup)) goto EXIT;
						if(Matrix42Dup!=NULL)
						Matrix42=nlsStrDup(Matrix42Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix43Attr,&Matrix43Dup)) goto EXIT;
						if(Matrix43Dup!=NULL)
						Matrix43=nlsStrDup(Matrix43Dup);

						if(dstat=objGetAttribute(ChildRelOP,TMatrix44Attr,&Matrix44Dup)) goto EXIT;
						if(Matrix44Dup!=NULL)
						Matrix44=nlsStrDup(Matrix44Dup);

						t5CheckDstat(objGetAttribute(ChildRelOP,g0InstanceNameAttr,&Instname));
						Instnamedup=nlsStrDup(Instname);

						//for(z=0;z<qty;z++)
						//{

							fprintf(fp_DMLBomList,"%s^%s^%s^%s",Matrix11,Matrix12,Matrix13,Matrix14);
							fprintf(fp_DMLBomList,"^%s^%s^%s^%s",Matrix21,Matrix22,Matrix23,Matrix24);
							fprintf(fp_DMLBomList,"^%s^%s^%s^%s",Matrix31,Matrix32,Matrix33,Matrix34);
							fprintf(fp_DMLBomList,"^%s^%s^%s^%s^%s^",Matrix41,Matrix42,Matrix43,Matrix44,Instnamedup);
							//fflush(matfp);
						//}

				}else
				{


								printf("\n Uses Elements");

								t5CheckDstat(objGetAttribute(ChildRelOP,LeftAttr,&LeftPartObjOBIDS));
								printf("\n Multi Quantity Assembly PartNumber(Left OBID) is:%s \n",LeftPartObjOBIDS);
								t5CheckDstat(objGetAttribute(ChildRelOP,RightAttr,&RightPartMstrObjOBIDS11));
								//printf("\n Multi Quantity Children PartNumber(Right OBID) is:%s \n",RightPartMstrObjOBIDS11);
								t5CheckDstat(oiSqlCreateSelect(&MultyQtySqlPtr));
								t5CheckDstat(oiSqlWhereEQ(MultyQtySqlPtr,OBIDAttr,RelnObid));
								t5CheckMfail(QueryWhere3(x2AsmPoQClass,MultyQtySqlPtr,0,SC_SCOPE_OF_SESSION,&MultyQtyRelPartSO,mfail));
								printf("\n Set Size of the Object in x2AsmPoQ is:%d \n",setSize(MultyQtyRelPartSO));

								if(setSize(MultyQtyRelPartSO)!=0)
								{
									RelQtyPartObjP = (ObjectPtr)low_set_get(MultyQtyRelPartSO,0);
									//RelQtyPartObjP =setGet(MultyQtyRelPartSO,0);
									t5CheckDstat(objGetTableSize(RelQtyPartObjP,"x0TrafoList",&sizeoftable));
									//printf("\n Full table name size is After Setting:%d \n",sizeoftable);
									t5CheckDstat(objGetAttribute(RelQtyPartObjP,QuantityAttr,&ChildPartQuantityS));
									//printf("\n Multi Quantity Value is:%s \n",ChildPartQuantityS);
									sprintf(decimalQty,"%d",atoi(ChildPartQuantityS));
									if(nlsStrCmp(decimalQty,ChildPartQuantityS)==0)
									{
									qty=atoi(ChildPartQuantityS);
									}else{
									qty=1;
									}

									printf("\n Multi Quantity Is : [%d,%d] ",qty,sizeoftable);
									//printf("\n MulitQunatity Valuse is (After Int Conv):%d \n",ChildPartMQuantityS);
									for(ChildMQPart=0;ChildMQPart<sizeoftable;ChildMQPart++)
									{



									//objDump(RelQtyPartObjP);
									//printf("\n Current Running Serial for the Index Matrix is:%d \n",ChildMQPart);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix11Attr,&ChildPartMQRelTMatrix11S));
									//printf("\n Getting Full table x0TrafoListAttr  TMatrix11Attr value is:%s \n",ChildPartMQRelTMatrix11S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,x0TrafoIndexAttr,&indexDupS));
									indexS=nlsStrDup(indexDupS);
									if(nlsIsStrNull(indexS))
									{
										continue;
										fprintf(fp_DMLBomList,"1.0^0.0^0.0^0.0^0.0^1.0^0.0^0.0^0.0^0.0^1.0^0.0^0.0^0.0^0.0^1.0^");
										//fflush(matfp);
									}
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix12Attr,&ChildPartMQRelTMatrix12S));
									//printf("\n Getting Full table x0TrafoListAttr  TMatrix12Attr value is:%s \n",ChildPartMQRelTMatrix12S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix13Attr,&ChildPartMQRelTMatrix13S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix13Attr value is:%s \n",ChildPartMQRelTMatrix13S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix14Attr,&ChildPartMQRelTMatrix14S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix14Attr value is:%s \n",ChildPartMQRelTMatrix14S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix21Attr,&ChildPartMQRelTMatrix21S));
									//printf("\n Getting Full table x0TrafoListAttr  TMatrix21Attr value is:%s \n",ChildPartMQRelTMatrix21S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix22Attr,&ChildPartMQRelTMatrix22S));
									//printf("\n Getting Full table x0TrafoListAttr  TMatrix22Attr value is:%s \n",ChildPartMQRelTMatrix22S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix23Attr,&ChildPartMQRelTMatrix23S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix23Attr value is:%s \n",ChildPartMQRelTMatrix23S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix24Attr,&ChildPartMQRelTMatrix24S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix24Attr value is:%s \n",ChildPartMQRelTMatrix24S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix31Attr,&ChildPartMQRelTMatrix31S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix31Attr value is:%s \n",ChildPartMQRelTMatrix31S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix32Attr,&ChildPartMQRelTMatrix32S));
									//printf("\n Getting Full table x0TrafoListAttr  TMatrix32Attr value is:%s \n",ChildPartMQRelTMatrix32S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix33Attr,&ChildPartMQRelTMatrix33S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix33Attr value is:%s \n",ChildPartMQRelTMatrix33S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix34Attr,&ChildPartMQRelTMatrix34S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix34Attr value is:%s \n",ChildPartMQRelTMatrix34S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix41Attr,&ChildPartMQRelTMatrix41S));
									//printf("\n Getting Full table x0TrafoListAttr   TMatrix41Attr value is:%s \n",ChildPartMQRelTMatrix41S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix42Attr,&ChildPartMQRelTMatrix42S));
									//printf("\n Getting Full table x0TrafoListAttr  TMatrix42Attr value is:%s \n",ChildPartMQRelTMatrix42S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix43Attr,&ChildPartMQRelTMatrix43S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix43Attr value is:%s \n",ChildPartMQRelTMatrix43S);
									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,TMatrix44Attr,&ChildPartMQRelTMatrix44S));
									//printf("\n Getting Full table x0TrafoListAttr TMatrix44Attr value is:%s \n",ChildPartMQRelTMatrix44S);

									t5CheckDstat(objGetTableAttribute(RelQtyPartObjP,x0TrafoListAttr,ChildMQPart,g0InstanceNameAttr,&Instname));
									Instnamedup=nlsStrDup(Instname);

									printf("\n 1.Instnamedup name:[%s]",Instnamedup);

									if(nlsIsStrNull(newInstancename))nlsStrFree(newInstancename);
									newInstancename=nlsStrAlloc(500);

									if(nlsIsStrNull(Instnamedup))
									{

												t5CheckDstat(objGetAttribute(RelQtyPartObjP,g0InstanceNameAttr,&Instname));
												printf("\n 1.Instance name:[%s]",Instname);
												Instnamedup=nlsStrDup(Instname);

												nlsStrCpy(newInstancename,Instnamedup);
												nlsStrCat(newInstancename,"#");
												nlsStrCat(newInstancename,indexS);
												printf("\n Get Attribute 1 :[%s]",newInstancename);
									}else
									{
												nlsStrCpy(newInstancename,Instnamedup);
												printf("\n Get Attribute 2 :[%s]",newInstancename);
									}

									fprintf(fp_DMLBomList,"%s^%s^%s^%s",ChildPartMQRelTMatrix11S,ChildPartMQRelTMatrix12S,ChildPartMQRelTMatrix13S,ChildPartMQRelTMatrix14S);
									fprintf(fp_DMLBomList,"^%s^%s^%s^%s",ChildPartMQRelTMatrix21S,ChildPartMQRelTMatrix22S,ChildPartMQRelTMatrix23S,ChildPartMQRelTMatrix24S);
									fprintf(fp_DMLBomList,"^%s^%s^%s^%s",ChildPartMQRelTMatrix31S,ChildPartMQRelTMatrix32S,ChildPartMQRelTMatrix33S,ChildPartMQRelTMatrix34S);
									fprintf(fp_DMLBomList,"^%s^%s^%s^%s^%s^",ChildPartMQRelTMatrix41S,ChildPartMQRelTMatrix42S,ChildPartMQRelTMatrix43S,ChildPartMQRelTMatrix44S,newInstancename);

									}
								}
				}
				fprintf(fp_DMLBomList,"\n");
			}	
		}
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
						

	SetOfObjects  DMLResultSO = NULL ;
	SetOfObjects  SetOfTasksObjs = NULL ;
	SetOfObjects  setOfAssmblyObjs = NULL ;
	SetOfObjects  ErcDMLObjs = NULL ;
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

	string aplReason=NULL;
	string				aplReasonDup=NULL;

	FILE	*fp_PlantDmlList= NULL;
	FILE	*fp_PlantDMLTaskPartInfo= NULL;
	FILE	*fp_DMLPartInfo	= NULL;
	FILE	*fp_DMLPartCADInfo	= NULL;
	FILE	*fp_DMLErrorLog	= NULL;
	FILE	*fp_DMLBomList	= NULL;
	FILE	*fp_DMLOPCSInfo	= NULL;
	//SetOfStrings DMLListSet     =  NULL;
		SetOfStrings		DMLValSet		=	NULL;


	int dmlCnt =0;
	int taskCnt =0;
	int partCnt =0;
	int ii =0;
	int aplDmlCnt =0;
	int dateIntVal=0;
	int monthIntVal=0;
	int yearIntVal=0;
	int aplRestAMDMLgrp98Found =0;

	SetOfStrings      dbScp	=          NULL;

	
	t5MethodInitWMD("t5APLDMLColorPartDetailsOneTime");

	printf("\n Executing... t5APLDMLColorPartDetailsOneTime.c ... \n");

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
	FromDateStrDup=nlsStrAlloc(50);
	curr_dateDup1=nlsStrAlloc(50);

	LoginS=argv[1];
	PasswordS=argv[2];
	ProjectName=argv[3];
	PlantName=argv[4];
	workingPath=argv[5];
	//DateFolder=argv[6];
	///FromDateStrDup=argv[6];
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
	//nlsStrCat(OutFilePlantDMLList,"_");
	//nlsStrCat(OutFilePlantDMLList,sys_date);
	nlsStrCat(OutFilePlantDMLList,".txt");

	printf("\n OutFilePlantDMLList %s !!!!\n",OutFilePlantDMLList);fflush(stdout);

	nlsStrCpy(OutFilePlantDMLErrorLog,workingPath);
	nlsStrCat(OutFilePlantDMLErrorLog,"/");
	nlsStrCat(OutFilePlantDMLErrorLog,ProjectName);
	nlsStrCat(OutFilePlantDMLErrorLog,"_");
	nlsStrCat(OutFilePlantDMLErrorLog,sPlntNmeDup1);
	nlsStrCat(OutFilePlantDMLErrorLog,"_ErrorLogInfo");
	//nlsStrCat(OutFilePlantDMLErrorLog,"_");
	//nlsStrCat(OutFilePlantDMLErrorLog,sys_date);
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
	//nlsStrCat(OutFilePlantDMLTaskPartInfo,"_");
	//nlsStrCat(OutFilePlantDMLTaskPartInfo,sys_date);
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
	//nlsStrCat(OutFilePlantDMLPartInfo,"_");
	//nlsStrCat(OutFilePlantDMLPartInfo,sys_date);
	nlsStrCat(OutFilePlantDMLPartInfo,".txt");

	printf("\n OutFilePlantDMLPartInfo %s !!!!\n",OutFilePlantDMLPartInfo);fflush(stdout);

	fp_DMLPartInfo=fopen(OutFilePlantDMLPartInfo,"w");
	if(fp_DMLPartInfo==NULL)
	{
		printf("\n%s:fp_DMLPartInfo file is not created...!!!!\n",OutFilePlantDMLPartInfo);fflush(stdout);
	}

	nlsStrCpy(OutFilePlantDMLPartCADInfo,workingPath);
	nlsStrCat(OutFilePlantDMLPartCADInfo,"/");
	nlsStrCat(OutFilePlantDMLPartCADInfo,ProjectName);
	nlsStrCat(OutFilePlantDMLPartCADInfo,"_");
	nlsStrCat(OutFilePlantDMLPartCADInfo,sPlntNmeDup1);
	nlsStrCat(OutFilePlantDMLPartCADInfo,"_PartCADInfo");
	//nlsStrCat(OutFilePlantDMLPartCADInfo,"_");
	//nlsStrCat(OutFilePlantDMLPartCADInfo,sys_date);
	nlsStrCat(OutFilePlantDMLPartCADInfo,".txt");

	printf("\n OutFilePlantDMLPartCADInfo %s !!!!\n",OutFilePlantDMLPartCADInfo);fflush(stdout);

	fp_DMLPartCADInfo=fopen(OutFilePlantDMLPartCADInfo,"w");
	if(fp_DMLPartCADInfo==NULL)
	{
		printf("\n%s:fp_DMLPartCADInfo file is not created...!!!!\n",OutFilePlantDMLPartCADInfo);fflush(stdout);
	}


	nlsStrCpy(OutFilePlantDMLBomList,workingPath);
	nlsStrCat(OutFilePlantDMLBomList,"/");
	nlsStrCat(OutFilePlantDMLBomList,ProjectName);
	nlsStrCat(OutFilePlantDMLBomList,"_");
	nlsStrCat(OutFilePlantDMLBomList,sPlntNmeDup1);
	nlsStrCat(OutFilePlantDMLBomList,"_BOMLIST");
	//nlsStrCat(OutFilePlantDMLBomList,"_");
	//nlsStrCat(OutFilePlantDMLBomList,sys_date);
	nlsStrCat(OutFilePlantDMLBomList,".txt");

	printf("\n OutFilePlantDMLBomList %s !!!!\n",OutFilePlantDMLBomList);fflush(stdout);

	fp_DMLBomList=fopen(OutFilePlantDMLBomList,"w");
	if(fp_DMLBomList==NULL)
	{
		printf("\n%s:fp_DMLBomList file is not created...!!!!\n",OutFilePlantDMLBomList);fflush(stdout);
	}

	nlsStrCpy(OutFilePlantDMLOPCSInfo,workingPath);
	nlsStrCat(OutFilePlantDMLOPCSInfo,"/");
	nlsStrCat(OutFilePlantDMLOPCSInfo,ProjectName);
	nlsStrCat(OutFilePlantDMLOPCSInfo,"_");
	nlsStrCat(OutFilePlantDMLOPCSInfo,sPlntNmeDup1);
	nlsStrCat(OutFilePlantDMLOPCSInfo,"_OptionalCSList");
	//nlsStrCat(OutFilePlantDMLOPCSInfo,"_");
	//nlsStrCat(OutFilePlantDMLOPCSInfo,sys_date);
	nlsStrCat(OutFilePlantDMLOPCSInfo,".txt");

	printf("\n OutFilePlantDMLOPCSInfo %s !!!!\n",OutFilePlantDMLOPCSInfo);fflush(stdout);

	fp_DMLOPCSInfo=fopen(OutFilePlantDMLOPCSInfo,"w");
	if(fp_DMLOPCSInfo==NULL)
	{
		printf("\n%s:fp_DMLOPCSInfo file is not created...!!!!\n",OutFilePlantDMLOPCSInfo);fflush(stdout);
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
		//FromDateStr=sysGetOffsetDate(curr_date,0,-3,0);
		FromDateStr=sysGetOffsetDate(curr_date,0,-3,0);
	}
	else
	{
		FromDateStr=sysGetOffsetDate(curr_date,0,-1,0);
		//FromDateStr=sysGetOffsetDate(curr_date,0,-6,0);
	}
	printf("\n FromDateStr =%s \n",FromDateStr);fflush(stdout);

	FromDateStr=strssra(FromDateStr, "-", "/");

	printf("\n 111 FromDateStr =%s \n",FromDateStr);fflush(stdout);
	//if(!nlsIsStrNull(FromDateStr)) FromDateStrDup = nlsStrDup(FromDateStr);

	curr_dateDup=sysGetOffsetDate(curr_date,0,1,0);
	printf("\n curr_dateDup =%s \n",curr_dateDup);fflush(stdout);

	curr_dateDup=strssra(curr_dateDup, "-", "/");

	printf("\n 111 curr_dateDup =%s \n",curr_dateDup);fflush(stdout);
	//if(!nlsIsStrNull(curr_dateDup)) curr_dateDup1 = nlsStrDup(curr_dateDup);

	DMLValSet		= setCreate(500);


//16_10_2019_1
low_set_add_str_unique(DMLValSet,"10PP362413_APL");
low_set_add_str_unique(DMLValSet,"13PP253123_APL");
low_set_add_str_unique(DMLValSet,"13PP253131_APL");
low_set_add_str_unique(DMLValSet,"13PP253136_APL");
low_set_add_str_unique(DMLValSet,"13PP253142_APL");
low_set_add_str_unique(DMLValSet,"13PP253143_APL");
low_set_add_str_unique(DMLValSet,"13PP253145_APL");
low_set_add_str_unique(DMLValSet,"13PP253152_APL");
low_set_add_str_unique(DMLValSet,"13PP253158_APL");
low_set_add_str_unique(DMLValSet,"13PP253160_APL");
low_set_add_str_unique(DMLValSet,"13PP253161_APL");
low_set_add_str_unique(DMLValSet,"13PP253164_APL");
low_set_add_str_unique(DMLValSet,"13PP253165_APL");
low_set_add_str_unique(DMLValSet,"13PP253166_APL");
low_set_add_str_unique(DMLValSet,"13PP253167_APL");
low_set_add_str_unique(DMLValSet,"13PP253168_APL");
low_set_add_str_unique(DMLValSet,"13PP253169_APL");
low_set_add_str_unique(DMLValSet,"13PP253170_APL");
low_set_add_str_unique(DMLValSet,"13PP253171_APL");
low_set_add_str_unique(DMLValSet,"13PP253172_APL");
low_set_add_str_unique(DMLValSet,"13PP253173_APL");
low_set_add_str_unique(DMLValSet,"13PP253174_APL");
low_set_add_str_unique(DMLValSet,"13PP253175_APL");
low_set_add_str_unique(DMLValSet,"13PP253176_APL");
low_set_add_str_unique(DMLValSet,"13PP253177_APL");
low_set_add_str_unique(DMLValSet,"13PP253203_APL");
low_set_add_str_unique(DMLValSet,"13PP253215_APL");
low_set_add_str_unique(DMLValSet,"13PP253217_APL");
low_set_add_str_unique(DMLValSet,"13PP253222_APL");
low_set_add_str_unique(DMLValSet,"13PP253224_APL");
low_set_add_str_unique(DMLValSet,"13PP253225_APL");
low_set_add_str_unique(DMLValSet,"13PP253226_APL");
low_set_add_str_unique(DMLValSet,"13PP253230_APL");
low_set_add_str_unique(DMLValSet,"13PP253231_APL");
low_set_add_str_unique(DMLValSet,"13PP253533_APL");
low_set_add_str_unique(DMLValSet,"13PP253538_APL");
low_set_add_str_unique(DMLValSet,"13PP253539_APL");
low_set_add_str_unique(DMLValSet,"13PP253540_APL");
low_set_add_str_unique(DMLValSet,"13PP253542_APL");
low_set_add_str_unique(DMLValSet,"13PP253543_APL");
low_set_add_str_unique(DMLValSet,"13PP253544_APL");
low_set_add_str_unique(DMLValSet,"13PP253545_APL");
low_set_add_str_unique(DMLValSet,"13PP253546_APL");
low_set_add_str_unique(DMLValSet,"13PP253547_APL");
low_set_add_str_unique(DMLValSet,"13PP253548_APL");
low_set_add_str_unique(DMLValSet,"13PP253549_APL");
low_set_add_str_unique(DMLValSet,"13PP253550_APL");
low_set_add_str_unique(DMLValSet,"13PP253551_APL");
low_set_add_str_unique(DMLValSet,"13PP253552_APL");
low_set_add_str_unique(DMLValSet,"13PP253555_APL");
low_set_add_str_unique(DMLValSet,"13PP253556_APL");
low_set_add_str_unique(DMLValSet,"13PP253557_APL");
low_set_add_str_unique(DMLValSet,"13PP253558_APL");
low_set_add_str_unique(DMLValSet,"13PP253559_APL");
low_set_add_str_unique(DMLValSet,"13PP253560_APL");
low_set_add_str_unique(DMLValSet,"13PP253561_APL");
low_set_add_str_unique(DMLValSet,"13PP253562_APL");
low_set_add_str_unique(DMLValSet,"13PP253565_APL");
low_set_add_str_unique(DMLValSet,"13PP253566_APL");
low_set_add_str_unique(DMLValSet,"13PP253567_APL");
low_set_add_str_unique(DMLValSet,"13PP253568_APL");
low_set_add_str_unique(DMLValSet,"13PP253570_APL");
low_set_add_str_unique(DMLValSet,"13PP253571_APL");
low_set_add_str_unique(DMLValSet,"13PP253573_APL");
low_set_add_str_unique(DMLValSet,"13PP253576_APL");
low_set_add_str_unique(DMLValSet,"13PP253577_APL");
low_set_add_str_unique(DMLValSet,"13PP253578_APL");
low_set_add_str_unique(DMLValSet,"13PP253579_APL");
low_set_add_str_unique(DMLValSet,"13PP253580_APL");
low_set_add_str_unique(DMLValSet,"13PP253584_APL");
low_set_add_str_unique(DMLValSet,"13PP253585_APL");
low_set_add_str_unique(DMLValSet,"13PP253586_APL");
low_set_add_str_unique(DMLValSet,"13PP253587_APL");
low_set_add_str_unique(DMLValSet,"13PP253588_APL");
low_set_add_str_unique(DMLValSet,"13PP253589_APL");
low_set_add_str_unique(DMLValSet,"13PP253590_APL");
low_set_add_str_unique(DMLValSet,"13PP253591_APL");
low_set_add_str_unique(DMLValSet,"13PP253592_APL");
low_set_add_str_unique(DMLValSet,"13PP253593_APL");
low_set_add_str_unique(DMLValSet,"13PP253594_APL");
low_set_add_str_unique(DMLValSet,"13PP253595_APL");
low_set_add_str_unique(DMLValSet,"13PP253596_APL");
low_set_add_str_unique(DMLValSet,"13PP253597_APL");
low_set_add_str_unique(DMLValSet,"13PP253600_APL");
low_set_add_str_unique(DMLValSet,"13PP253601_APL");
low_set_add_str_unique(DMLValSet,"13PP253602_APL");
low_set_add_str_unique(DMLValSet,"13PP253603_APL");
low_set_add_str_unique(DMLValSet,"13PP253605_APL");
low_set_add_str_unique(DMLValSet,"13PP253606_APL");
low_set_add_str_unique(DMLValSet,"13PP253608_APL");
low_set_add_str_unique(DMLValSet,"13PP253609_APL");
low_set_add_str_unique(DMLValSet,"13PP253610_APL");
low_set_add_str_unique(DMLValSet,"13PP253611_APL");
low_set_add_str_unique(DMLValSet,"13PP253612_APL");
low_set_add_str_unique(DMLValSet,"13PP253613_APL");
low_set_add_str_unique(DMLValSet,"13PP253614_APL");
low_set_add_str_unique(DMLValSet,"13PP253615_APL");
low_set_add_str_unique(DMLValSet,"13PP253616_APL");
low_set_add_str_unique(DMLValSet,"13PP253618_APL");
low_set_add_str_unique(DMLValSet,"13PP253619_APL");
low_set_add_str_unique(DMLValSet,"13PP253622_APL");
low_set_add_str_unique(DMLValSet,"13PP253623_APL");
low_set_add_str_unique(DMLValSet,"13PP253624_APL");
low_set_add_str_unique(DMLValSet,"13PP253626_APL");
low_set_add_str_unique(DMLValSet,"13PP253628_APL");
low_set_add_str_unique(DMLValSet,"13PP253629_APL");
low_set_add_str_unique(DMLValSet,"13PP253630_APL");
low_set_add_str_unique(DMLValSet,"13PP253635_APL");
low_set_add_str_unique(DMLValSet,"13PP253636_APL");
low_set_add_str_unique(DMLValSet,"13PP253639_APL");
low_set_add_str_unique(DMLValSet,"13PP253640_APL");
low_set_add_str_unique(DMLValSet,"13PP253642_APL");
low_set_add_str_unique(DMLValSet,"13PP253646_APL");
low_set_add_str_unique(DMLValSet,"13PP253647_APL");
low_set_add_str_unique(DMLValSet,"13PP253648_APL");
low_set_add_str_unique(DMLValSet,"13PP253649_APL");
low_set_add_str_unique(DMLValSet,"13PP253650_APL");
low_set_add_str_unique(DMLValSet,"13PP253651_APL");
low_set_add_str_unique(DMLValSet,"13PP253652_APL");
low_set_add_str_unique(DMLValSet,"13PP253653_APL");
low_set_add_str_unique(DMLValSet,"13PP253655_APL");
low_set_add_str_unique(DMLValSet,"13PP253656_APL");
low_set_add_str_unique(DMLValSet,"13PP253657_APL");
low_set_add_str_unique(DMLValSet,"13PP253658_APL");
low_set_add_str_unique(DMLValSet,"13PP253659_APL");
low_set_add_str_unique(DMLValSet,"13PP253660_APL");
low_set_add_str_unique(DMLValSet,"13PP253661_APL");
low_set_add_str_unique(DMLValSet,"13PP253662_APL");
low_set_add_str_unique(DMLValSet,"13PP253663_APL");
low_set_add_str_unique(DMLValSet,"13PP253665_APL");
low_set_add_str_unique(DMLValSet,"13PP253669_APL");
low_set_add_str_unique(DMLValSet,"13PP253670_APL");
low_set_add_str_unique(DMLValSet,"13PP253671_APL");
low_set_add_str_unique(DMLValSet,"13PP253672_APL");
low_set_add_str_unique(DMLValSet,"13PP253674_APL");
low_set_add_str_unique(DMLValSet,"13PP253676_APL");
low_set_add_str_unique(DMLValSet,"13PP253677_APL");
low_set_add_str_unique(DMLValSet,"13PP253678_APL");
low_set_add_str_unique(DMLValSet,"13PP253682_APL");
low_set_add_str_unique(DMLValSet,"13PP253686_APL");
low_set_add_str_unique(DMLValSet,"13PP253687_APL");
low_set_add_str_unique(DMLValSet,"13PP253688_APL");
low_set_add_str_unique(DMLValSet,"13PP253692_APL");
low_set_add_str_unique(DMLValSet,"13PP253693_APL");
low_set_add_str_unique(DMLValSet,"13PP253695_APL");
low_set_add_str_unique(DMLValSet,"13PP253698_APL");
low_set_add_str_unique(DMLValSet,"13PP253702_APL");
low_set_add_str_unique(DMLValSet,"13PP253703_APL");
low_set_add_str_unique(DMLValSet,"13PP253704_APL");
low_set_add_str_unique(DMLValSet,"13PP253705_APL");
low_set_add_str_unique(DMLValSet,"13PP253708_APL");
low_set_add_str_unique(DMLValSet,"13PP253709_APL");
low_set_add_str_unique(DMLValSet,"13PP253710_APL");
low_set_add_str_unique(DMLValSet,"13PP253713_APL");
low_set_add_str_unique(DMLValSet,"13PP253714_APL");
low_set_add_str_unique(DMLValSet,"13PP253719_APL");
low_set_add_str_unique(DMLValSet,"13PP253720_APL");
low_set_add_str_unique(DMLValSet,"13PP253721_APL");
low_set_add_str_unique(DMLValSet,"13PP253722_APL");
low_set_add_str_unique(DMLValSet,"13PP253723_APL");
low_set_add_str_unique(DMLValSet,"13PP253724_APL");
low_set_add_str_unique(DMLValSet,"13PP253726_APL");
low_set_add_str_unique(DMLValSet,"13PP253727_APL");
low_set_add_str_unique(DMLValSet,"13PP253728_APL");
low_set_add_str_unique(DMLValSet,"13PP253729_APL");
low_set_add_str_unique(DMLValSet,"13PP253730_APL");
low_set_add_str_unique(DMLValSet,"13PP253731_APL");
low_set_add_str_unique(DMLValSet,"13PP253732_APL");
low_set_add_str_unique(DMLValSet,"13PP253733_APL");
low_set_add_str_unique(DMLValSet,"13PP253734_APL");
low_set_add_str_unique(DMLValSet,"13PP253736_APL");
low_set_add_str_unique(DMLValSet,"13PP253738_APL");
low_set_add_str_unique(DMLValSet,"13PP253739_APL");
low_set_add_str_unique(DMLValSet,"13PP253740_APL");
low_set_add_str_unique(DMLValSet,"13PP253741_APL");
low_set_add_str_unique(DMLValSet,"13PP253742_APL");
low_set_add_str_unique(DMLValSet,"13PP253743_APL");
low_set_add_str_unique(DMLValSet,"13PP253746_APL");
low_set_add_str_unique(DMLValSet,"13PP253747_APL");
low_set_add_str_unique(DMLValSet,"13PP253749_APL");
low_set_add_str_unique(DMLValSet,"13PP253750_APL");
low_set_add_str_unique(DMLValSet,"13PP253751_APL");
low_set_add_str_unique(DMLValSet,"13PP253752_APL");
low_set_add_str_unique(DMLValSet,"13PP253754_APL");
low_set_add_str_unique(DMLValSet,"13PP253755_APL");
low_set_add_str_unique(DMLValSet,"13PP253756_APL");
low_set_add_str_unique(DMLValSet,"13PP253761_APL");
low_set_add_str_unique(DMLValSet,"13PP253762_APL");
low_set_add_str_unique(DMLValSet,"13PP253764_APL");
low_set_add_str_unique(DMLValSet,"13PP253765_APL");
low_set_add_str_unique(DMLValSet,"13PP253766_APL");
low_set_add_str_unique(DMLValSet,"13PP253767_APL");
low_set_add_str_unique(DMLValSet,"13PP253768_APL");
low_set_add_str_unique(DMLValSet,"13PP253769_APL");
low_set_add_str_unique(DMLValSet,"13PP253772_APL");
low_set_add_str_unique(DMLValSet,"13PP266036_APL");

	t5CheckDstat(oiSqlCreateSelect(&InputDMLSqlPtr));
	t5CheckMfail(oiSqlWhereIN(InputDMLSqlPtr,WbsIDAttr,DMLValSet,mfail));
	t5CheckDstat(oiSqlWhereAND(InputDMLSqlPtr));
	t5CheckDstat(oiSqlWhereIsNotNull(InputDMLSqlPtr,ActualRelDateAttr));
	t5CheckDstat(oiSqlPrint(InputDMLSqlPtr ));
	t5CheckMfail(QueryDbObject(CmChNtItClass,InputDMLSqlPtr,-1,TRUE,SC_SCOPE_OF_SESSION,&DMLResultSO,mfail));
	if(InputDMLSqlPtr) oiSqlDispose(InputDMLSqlPtr); InputDMLSqlPtr = NULL;
	printf("\n No:of Dml's are :- %d\n",setSize(DMLResultSO));fflush(stdout);

	if(setSize(DMLResultSO)>0)
	{
		
		for(dmlCnt=0;dmlCnt<setSize(DMLResultSO);dmlCnt++)
		{
				
			APLDmlObjPtr=NULL;
			APLDmlObjPtr=setGet(DMLResultSO,dmlCnt);

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
		for(dmlCnt=0;dmlCnt<setSize(DMLResultSO);dmlCnt++)
		{
			
			DmlObjPtr=NULL;
			aplRestAMDMLgrp98Found=0;
			DmlObjPtr=setGet(DMLResultSO,dmlCnt);

			aplDmlNo=NULL;
			aplDmlNoDup=NULL;

			if(dstat= objGetAttribute(DmlObjPtr,WbsIDAttr,&aplDmlNo)) goto  EXIT ;
			if (!nlsIsStrNull(aplDmlNo)) aplDmlNoDup=nlsStrDup(aplDmlNo);

			printf("\n aplDmlNoDup %s\n",aplDmlNoDup);fflush(stdout);

			aplDmlNoEcnType=NULL;
			aplDmlNoEcnTypeDup=NULL;

			if (!nlsIsStrNull(aplDmlNoDup))
			{
				if(nlsStrStr(aplDmlNoDup,"AM")!=NULL)
				{
					printf("\n Inside AM DML\n");fflush(stdout);
					if(dstat= objGetAttribute(DmlObjPtr,t5EcnTypeAttr,&aplDmlNoEcnType)) goto  EXIT ;
					if (!nlsIsStrNull(aplDmlNoEcnType)) aplDmlNoEcnTypeDup=nlsStrDup(aplDmlNoEcnType);
					printf("\n aplDmlNoEcnTypeDup %s\n",aplDmlNoEcnTypeDup);fflush(stdout);
					if (!nlsIsStrNull(aplDmlNoEcnTypeDup))
					{
						if(nlsStrCmp(aplDmlNoEcnTypeDup,"APLSTR")==0)
						{
							aplRestAMDMLgrp98Found++;
						}
					
					}

					aplAMDmlCreator=NULL;
					aplAMDmlCreatorDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,CreatorAttr,&aplAMDmlCreator)) goto  EXIT ;
					if (!nlsIsStrNull(aplAMDmlCreator)) aplAMDmlCreatorDup=nlsStrDup(aplAMDmlCreator);

					printf("\n aplAMDmlCreatorDup %s\n",aplAMDmlCreatorDup);fflush(stdout);

					aplAMDmlSynp=NULL;
					aplAMDmlSynpDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,WbsNameAttr,&aplAMDmlSynp)) goto  EXIT ;
					if (!nlsIsStrNull(aplAMDmlSynp)) aplAMDmlSynpDup=nlsStrDup(aplAMDmlSynp);

					if (!nlsIsStrNull(aplAMDmlSynpDup))
					{
						low_strrpl(aplAMDmlSynpDup, '\n', ' ');
						low_strrpl(aplAMDmlSynpDup, ',', ';');
					}

					printf("\n aplAMDmlSynpDup %s\n",aplAMDmlSynpDup);fflush(stdout);

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

					if (!nlsIsStrNull(aplAMDmlDescDup))
					{
						low_strrpl(aplAMDmlDescDup, '\n', ' ');
						low_strrpl(aplAMDmlDescDup, ',', ';');
					}

					printf("\n aplAMDmlDescDup %s\n",aplAMDmlDescDup);fflush(stdout);

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
				else if(nlsStrStr(aplDmlNoDup,"MC")!=NULL)
				{
					printf("\n Inside MC Series DML....\n");fflush(stdout);	
					continue;
				}
				else
				{
					printf("\n Else AMDML not found...so getting the data of PP\n");fflush(stdout);
					
					aplAMDmlProjCode=NULL;
					aplAMDmlProjCodeDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,ProjectNameAttr,&aplAMDmlProjCode)) goto  EXIT ;
					if (!nlsIsStrNull(aplAMDmlProjCode)) aplAMDmlProjCodeDup=nlsStrDup(aplAMDmlProjCode);

					printf("\n aplAMDmlProjCodeDup %s\n",aplAMDmlProjCodeDup);fflush(stdout);

					aplAMDmlDesc=NULL;
					aplAMDmlDescDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,WbsDescriptionAttr,&aplAMDmlDesc)) goto  EXIT ;
					if (!nlsIsStrNull(aplAMDmlDesc)) aplAMDmlDescDup=nlsStrDup(aplAMDmlDesc);

					if (!nlsIsStrNull(aplAMDmlDescDup))
					{
						low_strrpl(aplAMDmlDescDup, '\n', ' ');
						low_strrpl(aplAMDmlDescDup, ',', ';');
					}

					printf("\n aplAMDmlDescDup %s\n",aplAMDmlDescDup);fflush(stdout);

					aplAMDmlSynp=NULL;
					aplAMDmlSynpDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,WbsNameAttr,&aplAMDmlSynp)) goto  EXIT ;
					if (!nlsIsStrNull(aplAMDmlSynp)) aplAMDmlSynpDup=nlsStrDup(aplAMDmlSynp);

					if (!nlsIsStrNull(aplAMDmlSynpDup))
					{
						low_strrpl(aplAMDmlSynpDup, '\n', ' ');
						low_strrpl(aplAMDmlSynpDup, ',', ';');
					}

					printf("\n aplAMDmlSynpDup %s\n",aplAMDmlSynpDup);fflush(stdout);

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
							printf("\n aplDmlNoEcnTypeDup %s\n",aplDmlNoEcnTypeDup);fflush(stdout);
						}
					}

					/*if (!nlsIsStrNull(aplDmlNoEcnTypeDup))
					{
						if((nlsStrCmp(aplDmlNoEcnTypeDup,"CP")==0) || (nlsStrCmp(aplDmlNoEcnTypeDup,"CM")==0) || (nlsStrCmp(aplDmlNoEcnTypeDup,"ECU")==0))
						{
							printf("\n aplDmlNoEcnTypeDup %s is found.Hence skipping the DML..\n",aplDmlNoEcnTypeDup);fflush(stdout);
							continue;
						}
					}*/

					aplAMDRStatus=NULL;
					aplAMDRStatusDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,t5PartStatusAttr,&aplAMDRStatus)) goto  EXIT ;
					if (!nlsIsStrNull(aplAMDRStatus)) aplAMDRStatusDup=nlsStrDup(aplAMDRStatus);

					printf("\n aplAMDRStatusDup %s\n",aplAMDRStatusDup);fflush(stdout);

					aplReason=NULL;
					aplReasonDup=NULL;

					if(dstat= objGetAttribute(DmlObjPtr,ReasonAttr,&aplReason)) goto  EXIT ;
					if (!nlsIsStrNull(aplReason)) aplReasonDup=nlsStrDup(aplReason);

					printf("\n aplReasonDup %s\n",aplReasonDup);fflush(stdout);

					


				}
			
			}
			
			printf("\n aplRestAMDMLgrp98Found...%d\n",aplRestAMDMLgrp98Found);fflush(stdout);	
			


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



			SetOfTasksObjs=NULL;
			
			if(dstat=(ExpandObject5(CmPlRvRvClass,DmlObjPtr,"CMPlanBreaksDownInTo",SC_SCOPE_OF_SESSION,&SetOfTasksObjs,mfail)));
			printf("\n Size of SetOfTasksObjs is %d \n", setSize(SetOfTasksObjs));fflush(stdout);

			if(setSize(SetOfTasksObjs)>0)
			{

				taskCnt=0;
				for(taskCnt=0;taskCnt<setSize(SetOfTasksObjs);taskCnt++)
				{
					TaskObjPtr=NULL;
					TaskObjPtr=setGet(SetOfTasksObjs,taskCnt);

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

							printf("\n 666"); fflush(stdout);

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
							

							
							
							//IF DML IS AMDML//

							
							fprintf(fp_PlantDMLTaskPartInfo,"\n");

							printf("\n 111aplRestAMDMLgrp98Found...%d\n",aplRestAMDMLgrp98Found);fflush(stdout);	

							
							if (nlsStrStr(PartOrgIDDup,"APL")!=NULL)
							{
								printf("\n Calling GetTCPartInfoForAPL for APL.."); fflush(stdout);
								t5CheckMfail(GetTCPartInfoForAPL(TCPartObjP,PartOrgIDDup,csIndicator,StoreLocation,configCtxt,fp_DMLPartInfo,mfail));
								
								printf("\n Calling GetTCPartJTCreationForAPL for APL.."); fflush(stdout);
								t5CheckMfail(GetTCPartJTCreationForAPL(TCPartObjP,fp_DMLPartCADInfo,fp_DMLErrorLog,mfail));
								if(aplRestAMDMLgrp98Found>0)
								{
									printf("\n Calling GetTCPartBOMDiffForERCAPL for APL.."); fflush(stdout);
									t5CheckMfail(GetTCPartBOMDiffForERCAPL(TCPartObjP,PartNumberValDup,PartNumberRevDup,PartNumberSeqDup,PartOrgIDDup,configCtxt,fp_DMLBomList,mfail));
								}
								
								
							}
							else
							{
								if(nlsStrCmp(PartOrgIDDup,"ERC")==0)
								{
									printf("\n Calling GetTCPartInfoForAPL for ERC.."); fflush(stdout);
									t5CheckMfail(GetTCPartInfoForAPL(TCPartObjP,PartOrgIDDup,csIndicator,StoreLocation,configCtxt,fp_DMLPartInfo,mfail));
									if(aplRestAMDMLgrp98Found>0)
									{
										printf("\n Calling GetTCPartBOMDiffForERCAPL for ERC.."); fflush(stdout);
										t5CheckMfail(GetTCPartBOMDiffForERCAPL(TCPartObjP,PartNumberValDup,PartNumberRevDup,PartNumberSeqDup,PartOrgIDDup,configCtxt,fp_DMLBomList,mfail));
									}
								}

							}

							printf("\n Calling GetTCPartOptionalCS for ERC.."); fflush(stdout);
							t5CheckMfail(GetTCPartOptionalCS(TCPartObjP,GenOptionalCS,fp_DMLOPCSInfo,fp_DMLErrorLog,mfail));
							printf("\n Done GetTCPartOptionalCS for ERC.."); fflush(stdout);

						
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
	if(fp_DMLBomList) fclose(fp_DMLBomList); fp_DMLBomList=NULL;
	if(fp_DMLOPCSInfo) fclose(fp_DMLOPCSInfo); fp_DMLOPCSInfo=NULL;
	
	printf("Incremental Data Download complete...");fflush(stdout);

CLEANUP:
		t5PrintCleanUpModName;
		printf("Inside CLEANUP...");fflush(stdout);


EXIT:
		t5CheckDstatAndReturn;
	printf("Inside EXIT...");fflush(stdout);
}
;

