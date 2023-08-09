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


int main (int argc, char *argv[])
{
	int stat=0,j=0,LoopCnt1=0;
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

	SetOfObjects ChldObjSet=NULL;
	SetOfObjects ChldRelObjSet=NULL;

	SetOfObjects PartAsmSo=NULL;


	ObjectPtr genContextObj= NULL;
	ObjectPtr CtxtObjTmp= NULL;
	ObjectPtr LeftObjP = NULL ;
	ObjectPtr RightObjP = NULL ;
	ObjectPtr NewRObjP = NULL ;
	ObjectPtr TCPartObj = NULL ;
	ObjectPtr PartPtr = NULL ;


	ObjectPtr PartMstrRelObjPX2 = NULL;
	SetOfObjects convCad1SO = NULL;

	ObjectPtr 		view = NULL;
	ObjectPtr 		PartMstrRelObjP = NULL;
	SetOfObjects		convCad2SO = NULL;
	SetOfObjects		soview = NULL;
	SetOfObjects		ChldRelObjSetX2 = NULL;
	SetOfObjects		ChldObjSetX2 = NULL;

	

//	SqlPtr LeftSqlPtr = NULL ;
//	SqlPtr RightSqlPtr = NULL ;
//	SqlPtr RelnSqlPtr = NULL ;
	SqlPtr SqlPtrAsm = NULL ;
	SqlPtr ViewSqlPtr = NULL ;
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
	
	
	//RightRelnOwnS=nlsStrAlloc(nlsStrLen(argv[11])+1);

	LoginS=argv[1];
	PasswordS=argv[2];
	RelStatus=argv[3];
	PartNumerAsm=argv[4];
	PartNosAsmRev=argv[5];
	PartNosAsmSeq=argv[6];
	

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

	t5CheckDstat(oiSqlCreateSelect(&SqlPtrAsm));
	t5CheckDstat(oiSqlWhereEQ(SqlPtrAsm,PartNumberAttr,PartNumerAsm));
	t5CheckDstat(oiSqlWhereAND(SqlPtrAsm));
	t5CheckDstat(oiSqlWhereEQ(SqlPtrAsm,RevisionAttr,PartNosAsmRev));
	t5CheckDstat(oiSqlWhereAND(SqlPtrAsm));
	t5CheckDstat(oiSqlWhereEQ(SqlPtrAsm,SequenceAttr,PartNosAsmSeq));
	t5CheckMfail(QueryWhere("Part",SqlPtrAsm,&PartAsmSo,mfail));
	oiSqlPrint(SqlPtrAsm);
	printf("\n setSize(PartAsmSo) %s = [%d]\n",PartNumerAsm,setSize(PartAsmSo)); fflush(stdout);
	if (setSize(PartAsmSo)>0)
	{
		PartPtr=setGet(PartAsmSo,0);
		if(dstat=IntSetUpContext(objClass(PartPtr),PartPtr,&genContextObj,mfail)) ;
		if(dstat=GetCfgCtxtObjFromCtxt(DSesGnCClass,genContextObj,&CtxtObjTmp,mfail));
		//if(dstat=SetNavigateViewPref(CtxtObjTmp,TRUE,"EAS","ERC",mfail));
		if(dstat=SetNavigateViewPref(CtxtObjTmp,TRUE,"EAS","APL",mfail));
		if(dstat=objSetAttribute(CtxtObjTmp,PsmExpIncludeZeroQtyAttr,"+"));
		if(dstat=objSetAttribute(CtxtObjTmp,CfgItemIdAttr,"GlobalCtxt"));
		if(dstat=objSetObject(genContextObj,ConfigCtxtBlobAttr,CtxtObjTmp));
		if(dstat=SetExpOnRevInCtxt(DSesGnCClass,genContextObj,"PscLastRev",mfail));
		

		if((dstat = oiSqlCreateSelect(&ViewSqlPtr)) ||
		(dstat = oiSqlWhereBegParen(ViewSqlPtr)) ||
		//(dstat = oiSqlWhereEQ(ViewSqlPtr,ViewNameAttr ,"ERC")) ||
		(dstat = oiSqlWhereEQ(ViewSqlPtr,ViewNameAttr ,"APL")) ||
		(dstat = oiSqlWhereAND(ViewSqlPtr)) ||
		(dstat = oiSqlWhereEQ(ViewSqlPtr,ViewNetworkAttr,"EAS")) ||
		(dstat = oiSqlWhereEndParen(ViewSqlPtr)) ||
		(dstat = QueryWhere(VewClass,ViewSqlPtr,&soview,mfail))
		 ) goto EXIT;

		view= (ObjectPtr)low_set_get(soview,0);

		if(dstat = ExpandRelationWithCtxt("g2AsmPos",PartPtr,"g2AsmPosPartMstrsInAss",genContextObj,SC_SCOPE_OF_SESSION ,NULL, &ChldObjSet, &ChldRelObjSet, mfail)) goto EXIT;
		printf("\n PART:%s  revision and Sequence before deletion Size  g2asmpoas= [%d]",PartNumerAsm, setSize(ChldRelObjSet));fflush(stdout);
		for (LoopCnt1=0 ; LoopCnt1 < setSize(ChldRelObjSet) ; LoopCnt1++)
		{
			PartMstrRelObjP=low_set_get(ChldRelObjSet,LoopCnt1);
			printf("\nDeleting Previous uses Part relation");;fflush(stdout);
			t5CheckMfail(DeleteStructRelation2(PartMstrRelObjP,view,mfail));
		}
		if(dstat = ExpandRelationWithCtxt("g2AsmPos",PartPtr,"g2AsmPosPartMstrsInAss",genContextObj,SC_SCOPE_OF_SESSION ,NULL, &ChldObjSet, &ChldRelObjSet, mfail)) goto EXIT;
		printf("\n PART:%s  revision and Sequence aafter deletion Size in g2asmpoas  = [%d]",PartNumerAsm, setSize(ChldObjSet));fflush(stdout);

		if(dstat = ExpandRelationWithCtxt("x2AsmPoQ",PartPtr,"x2AsmPoQPartMstrsInAss",genContextObj,SC_SCOPE_OF_SESSION ,NULL, &ChldObjSetX2, &ChldRelObjSetX2, mfail)) goto EXIT;
		printf("\n PART:%s  revision and Sequence before deletion Size x2asmpoq = [%d]",PartNumerAsm, setSize(ChldRelObjSetX2));fflush(stdout);
		for (LoopCnt1=0 ; LoopCnt1 < setSize(ChldRelObjSetX2) ; LoopCnt1++)
		{
			PartMstrRelObjPX2=low_set_get(ChldRelObjSetX2,LoopCnt1);
			printf("\nDeleting Previous uses Part relation");;fflush(stdout);
			t5CheckMfail(DeleteStructRelation2(PartMstrRelObjPX2,view,mfail));
		}
		if(dstat = ExpandRelationWithCtxt("x2AsmPoQ",PartPtr,"x2AsmPoQPartMstrsInAss",genContextObj,SC_SCOPE_OF_SESSION ,NULL, &ChldObjSetX2, &ChldRelObjSetX2, mfail)) goto EXIT;
		printf("\n PART:%s  revision and Sequence aafter deletion Size in x2asmpoq  = [%d]",PartNumerAsm, setSize(ChldRelObjSetX2));fflush(stdout);
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
