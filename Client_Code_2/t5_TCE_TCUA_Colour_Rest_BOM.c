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
#include <tel.h>
#include <Mtimsgh.h>
#define NUMBER 5000

char* ChangeDateFormat(char*,  char*,  char*, char *, char *);
status t5ExpPartShowUsesCLPartCtxt(ObjectPtr ,ObjectPtr ,SetOfObjects *,SetOfObjects *,integer*);

int dayofweek(int d, int m, int y) 
{ 
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 }; 
	y -= m < 3; 
	return ( y + y/4 - y/100 + y/400 + t[m-1] + d) % 7; 
}
char* subString
(
  char* mainStringf , 
  int fromCharf     , 
  int toCharf		  
)
{
      int i;
      char *retStringf;
    //  retStringf = (char*) malloc(toCharf+1);
      retStringf = (char*) nlsStrAlloc(toCharf+1);
      for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
      *(retStringf+i) = '\0';
      return retStringf;
}

status t5ShowUsesCLPartMsgFunction_TCUA(ObjectPtr ColourPart,boolean	ShowInWindow,ObjectPtr genContextObjSTD,SetOfObjects *UsesColParts,integer* mfail)
{
	char		*mod_name	= "t5ShowUsesCLPartMsgFunction_TCUA";
	status		dstat		= OKAY;


	string		projectcode			= NULL;
	string		VehClass			= NULL;
	string		PartCatCode			= NULL;
	string		ColSrl				= NULL;
	string		assyCompCode			= NULL;
	string		assyColSrl			= NULL;
	string		subColSrl			= NULL;
	string		colourID			= NULL;
	string		colourIDDup			= NULL;
	string		comCode				= NULL;
	string		comCodeDup			= NULL;
	string		vcCS				= NULL;
	string		vcCSDup				= NULL;
	string		SchemeName			= NULL;
	string		Inputpart			= NULL;
	string		AssyPartNo			= NULL;
	string		SchmPrtNo			= NULL;
	string		SchRev				= NULL;
	string		SchSeq				= NULL;
	string		SchemeDesc			= NULL;
	string		SchName				= NULL;
	string		ColorSrl			= NULL;
	string		IntSchmInd			= NULL;
	string		IntSchmIndDup			= NULL;
	string		Iscoated			= NULL;
	string		IscoatedDup			= NULL;
	string		colourInd			= NULL;
	string		colourIndDup			= NULL;
	string		CoatedType			= NULL;
	string		CoatedTypeDup			= NULL;
	string		IscoatedSchm			= NULL;
	string		IscoatedSchmDup			= NULL;
	string		ccolourInd			= NULL;
	string		ccolourIndDup			= NULL;
	string		PartType			= NULL;
	string		PartTypeDup			= NULL;
	string		Coated				= NULL;

	SetOfObjects	ProjectSet			= NULL;
	SetOfObjects	soSchmObj			= NULL;
	SetOfObjects	ColSchmSet			= NULL;
	SetOfObjects	nonColourParts			= NULL;
	SetOfObjects	t5ItmRevs			= NULL;
	SetOfObjects	t5ItmRevRels			= NULL;
	SetOfObjects	childObjSO			= NULL;
	SetOfObjects	childRelObjSO			= NULL;
	SetOfObjects	t5ItmMstr			= NULL;
	SetOfObjects	t5ItmMstrRel			= NULL;
	SetOfObjects	colourParts			= NULL;
	SetOfObjects	LatPartObjs			= NULL;
	SetOfObjects	soClMas				= NULL;
	SetOfObjects	extraObj			= NULL;
	SET_PTR		ColourPartSet			= NULL;
	SET_PTR  	ColourPartSet1			= NULL;



	SetOfStrings	ssCs				= NULL;
	ObjectPtr	nonColourPart			= NULL;
	ObjectPtr	genContextObjTmp		= NULL;
	ObjectPtr	CtxtObjTmp			= NULL;
	ObjectPtr	t5ItmRev			= NULL;
	ObjectPtr	genContextObjOP			= NULL;
	ObjectPtr	contextObjOP			= NULL;
	ObjectPtr	childObj			= NULL;
	ObjectPtr	itemRevObj			= NULL;
	ObjectPtr	colourPart			= NULL;
	ObjectPtr	ClMasObj			= NULL;
	ObjectPtr	SchmObj				= NULL;
	ObjectPtr	orgObj1				= NULL;

	ObjectPtr	tmpSchmObj			= NULL;
	ObjectPtr	dialogObj2			= NULL;

	ObjectPtr	LatPartObj			= NULL;
	ObjectPtr	CopyCtxtObjTmp			= NULL;
	string		VehClassDup			= NULL;
	string		ColourInd			= NULL;
	string		Colid				= NULL;
	string		VColourInd			= NULL;
	string		EffectiveDate			= NULL;
	string		SchemeRev			= NULL;
	string		SchemeSeq			= NULL;
	string		tmpVehClass			= NULL;
	string		Veh_value			= NULL;
	string		VehTypeList			= NULL;
	NVSET		textInserts			= NULL;
	SetOfStrings	distinctVehClass		= NULL;
	SetOfStrings	extraStr			= NULL;



	SqlPtr		a_sql				= NULL;
	SqlPtr		Sql				= NULL;
	SqlPtr		sql1				= NULL;


	boolean		AssyHasFlag			= FALSE;
	boolean         ans				= FALSE;

	string		partType			= NULL;
	string		VCObid				= NULL;
	string		LeftSchm			= NULL;
	string		SchmName			= NULL;
	SqlPtr		sql				= NULL;
	SqlPtr		Sql_ptr				= NULL;
	SetOfObjects	SchmRelSet			= NULL;
	ObjectPtr	SchmRelObj			= NULL;
	ObjectPtr	SchmObjE			= NULL;

	SetOfObjects	TasksOfCLParts			= NULL;
	SetOfObjects	tmpColSchmSet			= NULL;
	SetOfObjects	soControls			= NULL;
	SetOfObjects	coatedParts			= NULL;
	SetOfObjects	coatedPartsRel			= NULL;
	SetOfObjects	CoatedPartObjs			= NULL;
	SetOfObjects	soExpObj			= NULL;
	SetOfObjects	relObjSet			= NULL;
	SetOfObjects	soCntrlObj			= NULL;

	SetOfObjects  	MasterOrgSet			= NULL;
	ObjectPtr	MasterOrgObj			= NULL;
	SetOfStrings	SetOfVC				= NULL;
	ObjectPtr	TaskObj				= NULL;
	string		TaskNo				= NULL;
	string		TaskNoDup			= NULL;
	string		tmpTaskNo			= NULL;
	string		scheme1				= NULL;
	string		scheme2				= NULL;
	string		scheme3				= NULL;
	string		scheme4				= NULL;
	string		scheme5				= NULL;
	string projectcodedup=NULL;
	string assyCompCodedup=NULL;
	string assyColSrldup=NULL;
	string		tOrganization			= NULL;
	string		IscoatedPartNo			= NULL;
	string		viewName			= NULL;
	string		AssyOrgId			= NULL;
	//string          start_date	                = NULL;
	string          ttRev		                = NULL;
	string          ttSeq		                = NULL;
	//string          revision		                = NULL;

	ObjectPtr	ctxtObj				= NULL; // Gss 2.45
	ObjectPtr	winObj				= NULL; // Gss 2.45
	ObjectPtr	Obj					= NULL; // Gss 2.45
	string		viwNme			= NULL;		// Gss 2.45
	boolean		bln		= FALSE;

	char colourScheme[300];
	char assyHasClSch[300];
	char schemeName	[200];
	ObjectPtr	CopyColourPartLop			= NULL;


	int TabSize=0,p=0,strLen=0,i=0,k=0,c=0,t=0,q=0,SchIndex,VehListSize=0,FlagR40=0;
	*mfail = USC_OKAY;

	colourIDDup	=low_getspace(100);
	comCodeDup	=low_getspace(100);
	PartCatCode	=low_getspace(100);


	tmpVehClass	=low_getspace(100);
	Veh_value	=low_getspace(100);
	VehTypeList	=low_getspace(100);


	SchemeDesc	=low_getspace(500);
	SchName		=low_getspace(40);
	SchRev		=low_getspace(2);
	SchSeq		=low_getspace(2);
	subColSrl	=low_getspace(100);
	ColorSrl	=low_getspace(100);
	distinctVehClass=setCreate(100);
	TaskNoDup=nlsStrAlloc(50);

	objCopy(&CopyColourPartLop,ColourPart);
	if(dstat=objGetAttribute(ColourPart,t5ColourIndAttr,&ColourInd)) goto EXIT;
	if (dstat = objGetAttribute(ColourPart,RevisionAttr,&ttRev)) goto EXIT;
	if (dstat = objGetAttribute(ColourPart,SequenceAttr,&ttSeq)) goto EXIT;
	if(dstat=objGetAttribute(ColourPart,t5CoatedAttr,&Coated)) goto EXIT;
	if(dstat=objGetAttribute(ColourPart,t5ColourIDAttr,&Colid)) goto EXIT;
	if (dstat = objGetAttribute(ColourPart,PartNumberAttr,&Inputpart)) goto EXIT;
	if (nlsStrStr(Inputpart,"R40")  || nlsStrStr(Inputpart,"L40"))
	{
		FlagR40=1;
	}
	printf("2.45  Inside t5ShowUsesColourPart Function for inputs ::: Partnumber[%s] [%s] [%s] :::Colour Ind[%s] :::Coated Ind[%s]   \n",Inputpart,ttRev,ttSeq,ColourInd,Coated);fflush(stdout);

	if(nlsStrCmp(ColourInd,"C") && nlsStrCmp(Coated,"C") )
	{
		/*t5CheckDstat(uiShowText("t5ColourIndNotFoundErr0", NULL, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
		*mfail = NOT_OKAY;*/
		goto CLEANUP;
	}

	if (dstat = objGetAttribute(ColourPart,ProjectNameAttr,&projectcodedup)) goto EXIT;
	projectcode=nlsStrDup(projectcodedup);
	if (dstat = objGetAttribute(ColourPart,t5PrtCatCodeAttr,&assyCompCodedup)) goto EXIT;
	assyCompCode=nlsStrDup(assyCompCodedup);
	if (dstat = objGetAttribute(ColourPart,t5ColourIDAttr,&assyColSrldup)) goto EXIT;
	assyColSrl=nlsStrDup(assyColSrldup);
	if((dstat = oiSqlCreateSelect(&Sql)) ||
	(dstat = oiSqlWhereBegParen(Sql)) ||
	(dstat = oiSqlWhereEQ(Sql,PartNumberAttr,Inputpart)) ||
	(dstat = oiSqlWhereAND(Sql)) ||
	(dstat = oiSqlWhereEQ(Sql, SupersededAttr,"-")) ||
	(dstat = oiSqlWhereEndParen(Sql)) ||
	(dstat = QueryDbObject(PartClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &LatPartObjs, mfail))) goto CLEANUP;

	if (setSize(LatPartObjs)>0)
	{
		printf("2.45   t5ShowUsesColourPart::Latest colour part found [Query Superseded]\n");fflush(stdout);
		LatPartObj=setGet(LatPartObjs,0);
	}
	else
	{
		printf("2.45   t5ShowUsesColourPart::Latest colour part not found [Query Superseded]\n");fflush(stdout);
		goto CLEANUP;
	}



	if(!nlsStrCmp(ColourInd,"C"))
	{
		if(nlsIsStrNull(Colid))
		{
			/*t5CheckDstat(uiShowText("t5CompCodeNotFoundErr0", NULL, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
			*mfail = NOT_OKAY;*/
			goto CLEANUP;
		}
	}

	if(ShowInWindow == TRUE) //Gss User Ctxt
	{
		t5CheckMfail(IntSetUpContext(objClass(ColourPart),ColourPart,&genContextObjTmp,mfail)) ;
		t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass,genContextObjTmp,&CtxtObjTmp,mfail));
		objCopy(&CopyCtxtObjTmp,CtxtObjTmp);
		t5CheckDstat(objGetAttribute(CopyCtxtObjTmp, NavigateViewNameAttr, &viewName));
		printf("2.45  t5ShowUsesColourPart::View Name Given in Context Is: %s \n",viewName);

		if(EffectiveDate) EffectiveDate=NULL;
		if(Sql_ptr)	oiSqlDispose(Sql_ptr);	Sql_ptr = NULL;
		t5CheckDstat(oiSqlCreateSelect(&Sql_ptr));
		t5CheckDstat(oiSqlWhereBegParen(Sql_ptr));
		t5CheckDstat(oiSqlWhereEQ(Sql_ptr,t5SyscdAttr,"UsesP"));
		t5CheckDstat(oiSqlWhereEndParen(Sql_ptr));
		t5CheckDstat(QueryDbObject(t5CntrolClass,Sql_ptr,-1,FALSE,SC_SCOPE_OF_SESSION,&soCntrlObj,mfail));
		if(Sql_ptr) oiSqlDispose(Sql_ptr); Sql_ptr = NULL;

		t5CheckDstat(objSetAttribute(CtxtObjTmp,CfgItemIdAttr,"GlobalCtxt"));
		t5CheckDstat(objGetAttribute(CtxtObjTmp, NavigateViewNameAttr, &viewName));
		printf("2.45  viewName :%s \n",viewName); fflush(stdout);
		t5CheckDstat(objSetObject(genContextObjTmp,ConfigCtxtBlobAttr,CtxtObjTmp));
		t5CheckMfail(SetExpOnRevInCtxt(DSesGnCClass,genContextObjTmp,"PscLastRev",mfail));

		t5CheckMfail(ExpandRelationWithCtxt(RevEffDClass,ColourPart,"RvfCfgItemInStrBIApc",genContextObjTmp,SC_SCOPE_OF_SESSION ,NULL, &soExpObj, &relObjSet, mfail));
	}
	else //Gss Passed Ctxt 2.45 genContextObjSTD
	{
		t5CheckDstat(objGetObject(genContextObjSTD, ConfigCtxtBlobAttr, &ctxtObj)); // Gss Getting SubObject of the Object
		t5CheckDstat(objGetAttribute(ctxtObj, NavigateViewNameAttr, &viwNme));
		if(!nlsIsStrNull(viwNme)) viewName = nlsStrDup(viwNme);
		t5CheckMfail(ExpandRelationWithCtxt(RevEffDClass,ColourPart,"RvfCfgItemInStrBIApc",genContextObjSTD,SC_SCOPE_OF_SESSION ,NULL, &soExpObj, &relObjSet, mfail));
	}

	if(setSize(relObjSet)>0)
	{
		t5CheckDstat(objGetAttribute (setGet(relObjSet,0),DateEffectiveFromAttr,&EffectiveDate));
		printf("2.45  t5ShowUsesColourPart::Inside RevEff Object Found for Colour Part [%s]:: and EffectiveDate is [%s] ",Inputpart,EffectiveDate);fflush(stdout);
	}

	t5CheckMfail(ExpandObject5("t5ClInfo",LatPartObj,"t5ColPrtforAsm",SC_SCOPE_OF_SESSION,&nonColourParts,mfail)) ;
	printf("2.45  t5ShowUsesColourPart::Has Non Colour Part found: [%d] \n ",setSize(nonColourParts));fflush(stdout);
	if (setSize(nonColourParts)>0)
	{
		printf("2.45  t5ShowUsesColourPart::Inside Has Non Colour Part found :%s:\n ",viewName);fflush(stdout);
		MasterOrgSet=setCreate(10);

		if(nlsStrCmp(viewName,"APL")==0 || nlsStrCmp(viewName,"STD")==0 )
		{
			for(k=0;k<setSize(nonColourParts) ;k++)
			{
				nonColourPart=setGet(nonColourParts,k);
				if (dstat = objGetAttribute(nonColourPart,OrganizationIDAttr,&AssyOrgId)) goto EXIT;
				printf("2.45  Input View Name [%s]:::and AssyOrgId [%s] \n",viewName,AssyOrgId);fflush(stdout);
				if (!nlsStrCmp(AssyOrgId,"APLPUNE"))
				{
				    low_set_add(MasterOrgSet,nonColourPart);
				    break;
				}
			}
			printf("2.45  setSize(MasterOrgSet) after APL org check:%d\n",setSize(MasterOrgSet) ); fflush(stdout);
			if (setSize(MasterOrgSet)<=0)
			{
				  low_set_add(MasterOrgSet,setGet(nonColourParts,0));
			}
		}
		if(nlsStrCmp(viewName,"APL1")==0 || nlsStrCmp(viewName,"STD1")==0 )
		{
			for(k=0;k<setSize(nonColourParts) ;k++)
			{
				nonColourPart=setGet(nonColourParts,k);
				if (dstat = objGetAttribute(nonColourPart,OrganizationIDAttr,&AssyOrgId)) goto EXIT;
				printf("2.45  AssyOrgId :%s\n",AssyOrgId);fflush(stdout);
				if (!nlsStrCmp(AssyOrgId,"APLAHD"))
				{
				    low_set_add(MasterOrgSet,nonColourPart);
				    break;
				}
			}
			printf("2.45  setSize(MasterOrgSet) after APL org check:%d\n",setSize(MasterOrgSet) ); fflush(stdout);
			if (setSize(MasterOrgSet)<=0)
			{
				  low_set_add(MasterOrgSet,setGet(nonColourParts,0));
			}
		}
		if(nlsStrCmp(viewName,"ERC")==0)
		{
			 low_set_add(MasterOrgSet,setGet(nonColourParts,0));
		}

		printf("2.45  setSize(MasterOrgSet) before taking Rev:%d\n",setSize(MasterOrgSet) ); fflush(stdout);
		if (setSize(MasterOrgSet)>0)
		{

			MasterOrgObj=low_set_get(MasterOrgSet,0);
			if(ShowInWindow == TRUE) //Gss User Ctxt
			{
				t5CheckMfail(IntSetUpContext(objClass(MasterOrgObj),MasterOrgObj,&genContextObjTmp,mfail)) ;
				t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass,genContextObjTmp,&CtxtObjTmp,mfail));
				t5CheckDstat(objSetAttribute(CtxtObjTmp,CfgItemIdAttr,"GlobalCtxt"));
				if(!nlsIsStrNull(EffectiveDate))
				{
					//t5CheckDstat(objSetAttribute(CtxtObjTmp,EffectiveDateAttr,EffectiveDate));
					t5CheckDstat(objSetAttribute(CtxtObjTmp,EffectiveDateAttr,""));
				}
				else
				{
					t5CheckDstat(objSetAttribute(CtxtObjTmp,EffectiveDateAttr,""));
				}


				t5CheckDstat(objSetObject(genContextObjTmp,ConfigCtxtBlobAttr,CtxtObjTmp));
				t5CheckMfail(SetExpOnRevInCtxt(DSesGnCClass,genContextObjTmp,"PscLastRev",mfail));

				t5CheckMfail(ExpandRelationWithCtxt(ItemRevClass,
							MasterOrgObj,
							"StrucBIRevsOfItemMstr",
							 genContextObjTmp,
							 SC_SCOPE_OF_SESSION,
							 NULL,
							 &t5ItmRevs,
							&t5ItmRevRels,
							mfail))  ;
			}
			else //Gss Passed Ctxt 2.45  genContextObjSTD
			{
				if(!nlsIsStrNull(EffectiveDate))
				{
					//t5CheckDstat(objSetAttribute(ctxtObj,EffectiveDateAttr,EffectiveDate));
					t5CheckDstat(objSetAttribute(ctxtObj,EffectiveDateAttr,""));
				}
				else
				{
					t5CheckDstat(objSetAttribute(ctxtObj,EffectiveDateAttr,""));
				}
				t5CheckMfail(ExpandRelationWithCtxt(ItemRevClass,
							MasterOrgObj,
							"StrucBIRevsOfItemMstr",
							 genContextObjSTD,
							 SC_SCOPE_OF_SESSION,
							 NULL,
							 &t5ItmRevs,
							&t5ItmRevRels,
							mfail))  ;
			}


			printf("2.45  SetSize t5ItmRevs: %d \n ",setSize(t5ItmRevs));fflush(stdout);

			if (setSize(t5ItmRevs)==0)
			{
				printf("2.45  No ItemRev found Hence EXIT.....\n");fflush(stdout);
				/*t5CheckDstat(uiShowText("t5ItemRevErr01", NULL, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
				*mfail = NOT_OKAY;*/
				goto CLEANUP;
			}
			else
			{
				t5ItmRev=setGet(t5ItmRevs,0);
				if (dstat = objGetAttribute(t5ItmRev,PartNumberAttr,&AssyPartNo)) goto EXIT;
				if (dstat = objGetAttribute(t5ItmRev,RevisionAttr,&ttRev)) goto EXIT;
				if (dstat = objGetAttribute(t5ItmRev,SequenceAttr,&ttSeq)) goto EXIT;

				printf("2.45  After Expanding with Effective Date::PartNumber::[%s][%s][%s]\n",AssyPartNo,ttRev,ttSeq);fflush(stdout);

				if(VColourInd)  VColourInd=NULL;
				if(dstat=objGetAttribute(t5ItmRev,t5ColourIndAttr,&VColourInd)) goto EXIT;

				if(nlsStrCmp(VColourInd,"N")==0 && !nlsStrCmp(ColourInd,"C"))
				{
					/*t5CheckDstat(uiShowText("t5ColourIndErr0", NULL, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
					*mfail = NOT_OKAY;*/
					goto CLEANUP;
				}
			}
		}
	}
	else		//Shreyas 14-Apr-14
	{
		printf("\nRelation Of Non Colour Part Does Not Exist With Colour Part.\n");fflush(stdout);
		goto CLEANUP;
	}

	//Added by Sharvari on 12/05/2010
	if(!nlsStrCmp(Coated,"C") && !nlsStrCmp(ColourInd,"N") )
	{
		ColourPartSet1=setCreate(200);

		printf("2.45  Inside Non Colour Coated Part \n"); fflush(stdout);

		t5CheckDstat(objGetAttribute(ColourPart,OrganizationIDAttr,&tOrganization));
		t5CheckMfail(IntSetUpContext(objClass(t5ItmRev),t5ItmRev,&genContextObjOP,mfail));
		t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass, genContextObjOP, &contextObjOP, mfail));
		printf("2.45  tOrganization :%s \n",tOrganization); fflush(stdout);
		if (nlsStrStr(tOrganization,"APL"))
		{
		     t5CheckDstat(SetNavigateViewPref(contextObjOP,TRUE,"EAS","APL",mfail));
		}


		t5CheckMfail(objSetAttribute(contextObjOP,CfgItemIdAttr,"GlobalCtxt"));
		t5CheckDstat(objSetAttribute(contextObjOP,EffectiveDateAttr,""));
		t5CheckDstat(objSetObject(genContextObjOP,ConfigCtxtBlobAttr,contextObjOP));
		t5CheckMfail(SetExpOnRevInCtxt(DSesGnCClass,genContextObjOP,"PscLastRev",mfail));

		t5FreeSetOfObjects(childObjSO);

		t5CheckMfail(ExpandObject2(AsRevRevClass,t5ItmRev,"PartsInAssembly",SC_SCOPE_OF_SESSION,&childObjSO,&childRelObjSO,mfail)) ;

		printf("2.45   No of Uses Parts found : %d \n ",setSize(childObjSO));fflush(stdout);

		if(setSize(childObjSO)>0)
		{
			for(i=0;i<setSize(childObjSO);i++)
			{

				//printf("2.45  Inside childObjSO found i is :%d\n ",i);fflush(stdout);
				childObj=setGet(childObjSO,i);
				if (Iscoated)   Iscoated=NULL;
				if (colourInd)   colourInd=NULL;
				if (dstat = objGetAttribute(childObj,t5CoatedAttr,&Iscoated)) goto EXIT;
				if(!nlsIsStrNull(Iscoated)) IscoatedDup = nlsStrDup(Iscoated);
				if (dstat = objGetAttribute(childObj,t5ColourIndAttr,&colourInd)) goto EXIT;
				if(!nlsIsStrNull(colourInd)) colourIndDup = nlsStrDup(colourInd);
				printf("2.45  Iscoated :%s and colourInd:%s \n",Iscoated,colourInd); fflush(stdout);
				if(!nlsStrCmp(colourIndDup,"N") && !nlsStrCmp(IscoatedDup,"Y"))
				{
					t5CheckMfail(ExpandObject2(ItemRevClass,childObj,"ItemMstrForStrucBIRev",SC_SCOPE_OF_SESSION,&t5ItmMstr,&t5ItmMstrRel,mfail)) ;
					itemRevObj=setGet(t5ItmMstr,0);
					printf("2.45  itemrev obj \n"); fflush(stdout);
					t5CheckMfail(ExpandObject2(t5ClInfoClass,itemRevObj,"t5AsmhasColPrt",SC_SCOPE_OF_SESSION,&coatedParts,&coatedPartsRel,mfail)) ;
					printf("2.45  SetSize coatedParts: %d \n ",setSize(coatedParts));fflush(stdout);
					if (setSize(coatedParts)>0)
					{
						if (dstat = objGetAttribute(low_set_get(coatedParts,0),PartNumberAttr,&IscoatedPartNo)) goto EXIT;
						printf("2.45  IscoatedPartNo :%s\n",IscoatedPartNo); fflush(stdout);
						if((dstat = oiSqlCreateSelect(&Sql)) ||
						(dstat = oiSqlWhereBegParen(Sql)) ||
						(dstat = oiSqlWhereEQ(Sql,PartNumberAttr,IscoatedPartNo)) ||
						(dstat = oiSqlWhereAND(Sql)) ||
						(dstat = oiSqlWhereEQ(Sql, SupersededAttr,"-")) ||
						(dstat = oiSqlWhereEndParen(Sql)) ||
						(dstat = QueryDbObject(PartClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &CoatedPartObjs, mfail))) goto CLEANUP;
						printf("2.45  setSize(CoatedPartObjs) :%d\n",setSize(CoatedPartObjs)); fflush(stdout);
						if (setSize(CoatedPartObjs)>0)
						{
							low_set_add(ColourPartSet1,low_set_get(CoatedPartObjs,0));
						}
						//break;
					}
					else
					{
						printf("2.45  else no coated found last\n");
						low_set_add(ColourPartSet1,childObj);
					}

				}
				if(!nlsStrCmp(colourIndDup,"N") && !nlsStrCmp(IscoatedDup,"N"))
				{
					printf("2.45  else last\n");
					low_set_add(ColourPartSet1,childObj);
					//break;
				}
				if(!nlsStrCmp(colourIndDup,"Y"))
				{
					printf("2.45  else colourind Y\n");
					low_set_add(ColourPartSet1,childObj);
					//break;
				}
			}
			if(setSize(ColourPartSet1)>0)
			{
				printf("2.45  setSize(ColourPartSet1):%d\n",setSize(ColourPartSet1)); fflush(stdout);
				t5CheckDstat(uiActivateBrowserWindow());
				t5CheckMfail(GenerateBrowser(PartClass,BrwNewWinValue,NULL,NULL,mfail));
				for (i=0;i<setSize(ColourPartSet1) ;i++ )
				{
					if(dstat = RefreshObject2(setGet(ColourPartSet1,i),TRUE,mfail)) goto EXIT;
					if(dstat = RefreshItObj(setGet(ColourPartSet1,i),TRUE,TRUE,&bln,&Obj,mfail)) goto EXIT;
				}

				uiGetWindowObject(&winObj);
				SetColTableOnBrw("d5ColStr",winObj,NULL,NULL,mfail);

				uiSetListViewColumns(winObj);

				t5CheckMfail(ShowListInBrowser(PartClass,PDO_BROWSER_ADD_TO,ColourPartSet1,NULL,NULL,NULL,mfail));

				  t5CheckMfail(uiSetWindowTitle ("Uses Colour-Coated Part"));
				t5CheckDstat(uiSetBrowserView("BrwListView"));




				t5CheckDstat(uiActivateBrowserWindow());
			}
			else
			{
				/*t5CheckDstat(uiShowText("t5ColourStrErr0", NULL, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
				*mfail = NOT_OKAY;*/
				goto CLEANUP;
			}

	        }
		else
		{
			/*t5CheckDstat(uiShowText("t5ColourStrErrC1", NULL, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
			*mfail = NOT_OKAY;*/
			goto CLEANUP;
		}
        }
	/****************Query for Master *************************/

	if(!nlsStrCmp(ColourInd,"C"))
	{
		if(sql1)	oiSqlDispose(sql1);	sql1 = NULL;
		if(soClMas)
		{
			//objDisposeAll(soClMas);
			soClMas = NULL;
		}
		if((dstat = oiSqlCreateSelect(&sql1)) ||
		(dstat = oiSqlWhereBegParen(sql1)) ||
		(dstat = oiSqlWhereEQ(sql1,t5Category_CodeAttr,assyCompCode)) ||
		(dstat = oiSqlWhereEndParen(sql1)) ||
		(dstat = QueryDbObject(t5CatCdClass, sql1, 0, TRUE, SC_SCOPE_OF_SESSION, &soClMas, mfail))) goto CLEANUP;
		printf("2.45  **Colour Master Query Set Size:%d\n",setSize(soClMas));fflush(stdout);
		if(setSize(soClMas)>0)
		{
			ClMasObj=setGet(soClMas,0);
			if (dstat = objGetAttribute(ClMasObj,t5InternalSchmAttr,&IntSchmInd)) goto EXIT;
			if(!nlsIsStrNull(IntSchmInd)) IntSchmIndDup = nlsStrDup(IntSchmInd);
		}

		/*******************************************************************************************************/
		if(t5ItmRev!=NULL)			//Shreyas 14-Apr-14
		{
			t5CheckDstat(objGetAttribute(t5ItmRev,PartTypeAttr,&partType)) ;
		}
		if(nlsStrCmp(partType,"VC")!=0)
		{
			if(dstat =  ExpandObject5( CmRsltsClass,ColourPart,"BusItemIsResultOfCMPrdPln",SC_SCOPE_OF_SESSION,&TasksOfCLParts,mfail));
			printf("2.45  Tasks found for Colour Part:%d\n",setSize(TasksOfCLParts));fflush(stdout);
			if(setSize(TasksOfCLParts)>0)
			{
				for(i=0;i<setSize(TasksOfCLParts);i++)
				{
					TaskObj=low_set_get(TasksOfCLParts,i);
					if(TaskNo) TaskNo=NULL;

					if (dstat = objGetAttribute(TaskObj,WbsIDAttr,&TaskNo)) goto EXIT;
					
					/*Start - Commented the code for Multi Plant DML implementation by JSR Team
					if ((nlsStrStr(TaskNo,"APLPUNE")==NULL || nlsStrStr(TaskNo,"APL")==NULL) && (nlsStrStr(TaskNo,"APL1")==NULL  || nlsStrStr(TaskNo,"APLSGR")==NULL ))
					End - Commented the code for Multi Plant DML implementation by JSR Team*/
					
					//START Commenting Code Changes for STD-JSR ECN 
					//if (nlsStrStr(TaskNo,"APL")==NULL)				/*Start/End Multi Plant DML implementation by JSR Team*/
					 if( FlagR40 || (( nlsStrStr(TaskNo,"APL")==NULL && nlsStrStr(TaskNo,"STD")==NULL)))/*Start/End Multi Plant DML implementation by JSR Team*/
					//END Commenting Code Changes for STD-JSR ECN
					{
						if(!nlsIsStrNull(TaskNo)) nlsStrCpy(TaskNoDup,TaskNo);
						if(tmpTaskNo) tmpTaskNo=NULL;
						tmpTaskNo=subString(TaskNoDup,0,10);
						printf("2.45  **going to query Control Object for DML:%s\n",tmpTaskNo);fflush(stdout);

						if((dstat = oiSqlCreateSelect(&sql1)) ||
						(dstat = oiSqlWhereBegParen(sql1)) ||
						(dstat = oiSqlWhereEQ(sql1,t5VCDmlNoAttr,tmpTaskNo)) ||
						(dstat = oiSqlWhereEndParen(sql1)) ||
						(dstat = QueryDbObject(t5ColCtlClass, sql1, 0, TRUE, SC_SCOPE_OF_SESSION, &soControls, mfail))) goto CLEANUP;
						printf("2.45  **Control Object Found by Query:%d\n",setSize(soControls));fflush(stdout);
						SetOfVC  = setCreate(5);
						if (setSize(soControls)>0)
						{

							t5CheckDstat(objGetAttribute(setGet(soControls,0),t5SchmNomenclature1Attr,&scheme1));
							if(!nlsIsStrNull(scheme1)) low_set_add_str(SetOfVC,scheme1);
							t5CheckDstat(objGetAttribute(setGet(soControls,0),t5SchmNomenclature2Attr,&scheme2));
							if(!nlsIsStrNull(scheme2)) low_set_add_str(SetOfVC,scheme2);
							t5CheckDstat(objGetAttribute(setGet(soControls,0),t5SchmNomenclature3Attr,&scheme3));
							if(!nlsIsStrNull(scheme3)) low_set_add_str(SetOfVC,scheme3);
							t5CheckDstat(objGetAttribute(setGet(soControls,0),t5SchmNomenclature4Attr,&scheme4));
							if(!nlsIsStrNull(scheme4))  low_set_add_str(SetOfVC,scheme4);
							t5CheckDstat(objGetAttribute(setGet(soControls,0),t5SchmNomenclature5Attr,&scheme5));
							if(!nlsIsStrNull(scheme5)) low_set_add_str(SetOfVC,scheme5);
						}

						printf("2.45  No of Colour Schemes found for DML==%s :%d\n",tmpTaskNo,setSize(SetOfVC));fflush(stdout);

						if (setSize(SetOfVC)>0)
						{
							for(i=0;i<setSize(SetOfVC);i++)
							{
								printf("2.45  %d.going to Query scheme for :%s  \n",i,setGetStr(SetOfVC,i));fflush(stdout);
								if(!nlsIsStrNull(EffectiveDate) && setSize(soCntrlObj)>0)
								{
									printf("2.45  Inside EffectiveDate option being used  \n");fflush(stdout);
									if(ColSchmSet) ColSchmSet=NULL;
									if (dstat = oiSqlCreateSelect(&sql) |
										    oiSqlWhereBegParen(sql) |
										    oiSqlWhereLE(sql,PlannedRelDateAttr,EffectiveDate)|
										    oiSqlWhereAND(sql)|
										    oiSqlWhereEQ(sql,NomenclatureAttr,setGet(SetOfVC,i)) |
										    oiSqlWhereAND(sql) |
										//    oiSqlWhereEQ(sql, OwnerNameAttr,"Release Vault") |
										     oiSqlWhereLike(sql, OwnerNameAttr,"%Release Vault") |
										    oiSqlWhereEndParen(sql))
									if(*mfail)goto CLEANUP;
									if (dstat = QueryDbObject(t5ClSchmClass,sql,0,TRUE,SC_SCOPE_OF_SESSION,&tmpColSchmSet,mfail))   ;
									if(*mfail)goto CLEANUP;

								}
								else
								{
									printf("2.45  Inside EffectiveDate option not being used  \n");fflush(stdout);
									if(ColSchmSet) ColSchmSet=NULL;
									if (dstat = oiSqlCreateSelect(&sql) |
										    oiSqlWhereBegParen(sql) |
										    oiSqlWhereEQ(sql,NomenclatureAttr,setGet(SetOfVC,i)) |
										    oiSqlWhereAND(sql) |
										//    oiSqlWhereEQ(sql, OwnerNameAttr,"Release Vault") |
										    oiSqlWhereLike(sql, OwnerNameAttr,"%Release Vault") |
										    oiSqlWhereEndParen(sql))
									if(*mfail)goto CLEANUP;
									if (dstat = QueryDbObject(t5ClSchmClass,sql,0,TRUE,SC_SCOPE_OF_SESSION,&tmpColSchmSet,mfail))   ;
									if(*mfail)goto CLEANUP;

								}
								printf("2.45  %d: Scheme found through Control Object for scheme ===%s:%d  \n",i,setGetStr(SetOfVC,i),setSize(tmpColSchmSet));fflush(stdout);

								if(setSize(tmpColSchmSet)>0)
								{
									for(t=0;t<setSize(tmpColSchmSet);t++)
									{
										t5CheckDstat(ulyAddObjectToSet(low_set_get(tmpColSchmSet,t),&soSchmObj));
									}
									low_set_empty(tmpColSchmSet);
									printf("2.45  %d:Scheme Added====:%d  \n",i,setSize(soSchmObj));fflush(stdout);
								}
							}
						}

						if (setSize(soSchmObj)>0)
						{
							printf("2.45  Total Scheme found through Control Object:%d  \n",setSize(soSchmObj));fflush(stdout);
							for(t=0;t<setSize(soSchmObj);t++)
							{
								printf("\nCheck 1\n");fflush(stdout);
								if(dstat =objGetTableSize(low_set_get(soSchmObj,t),t5SchFullTabAttr,&TabSize)) goto EXIT;
								printf("\n2.45  TabSize is %d\n", TabSize);fflush(stdout);
								if (dstat = objGetAttribute(low_set_get(soSchmObj,t),NomenclatureAttr,&SchemeName)) goto EXIT;
								printf("2.45  %d. Filtering the Schemes %s for %s amd %s  : and %s \n",t,SchemeName,assyCompCode,assyColSrl,AssyPartNo);fflush(stdout);

								for (p=0;p<TabSize;p++)
								{

									if(dstat = objGetTableAttribute(low_set_get(soSchmObj,t),t5SchFullTabAttr,p,t5PrtCatCodeAttr,&PartCatCode)) goto EXIT;
									if(dstat = objGetTableAttribute(low_set_get(soSchmObj,t),t5SchFullTabAttr,p,t5ClSrlAttr,&ColSrl)) goto EXIT;
									if(dstat = objGetTableAttribute(low_set_get(soSchmObj,t),t5SchFullTabAttr,p,t5SchmPrtCAttr,&SchmPrtNo)) goto EXIT;

									if((!nlsIsStrNull(PartCatCode))&&(!nlsIsStrNull(ColSrl)))
									{
										strLen=strlen(ColSrl);
										subColSrl=subString(ColSrl,3,strLen);
										nlsStrTrimTrailWhiteSpace(subColSrl);
										//printf("2.45  Checking in Full table: PartCatCode  ColSrl & Scheme Part No: %s ,%s ,%s \n",PartCatCode,subColSrl,SchmPrtNo);fflush(stdout);
									}

									if (nlsStrCmp(IntSchmIndDup,"T")==0)
									{
									if(!nlsIsStrNull(SchmPrtNo))
									{
										nlsStrTrimTrailWhiteSpace(SchmPrtNo);

										if(nlsStrCmp(PartCatCode,assyCompCode)==0 && nlsStrCmp(subColSrl,assyColSrl)==0 && nlsStrStr(SchmPrtNo,AssyPartNo)!=NULL)
										{
											printf("2.45  A. Inside Fields Match in Scheme for Scheme %s \n",SchemeName);fflush(stdout);
											//printf("2.45  Comp code Comparision:: %s %s",PartCatCode,assyCompCode);fflush(stdout);
											//printf("2.45  Colour ID Comparision:: %s %s",subColSrl,assyColSrl);fflush(stdout);
											//printf("2.45  Part NO   Comparision:: %s %s",SchmPrtNo,AssyPartNo);fflush(stdout);
											t5CheckDstat(ulyAddObjectToSet(low_set_get(soSchmObj,t),&ColSchmSet));
											break;
										}
									}
									else
									{
										if(nlsStrCmp(PartCatCode,assyCompCode)==0 && nlsStrCmp(subColSrl,assyColSrl)==0 )
										{
											printf("2.45  B. Inside Fields Match in Scheme for Scheme %s \n",SchemeName);fflush(stdout);
											//printf("2.45  Comp code Comparision:: %s %s",PartCatCode,assyCompCode);fflush(stdout);
											//printf("2.45  Colour ID Comparision:: %s %s",subColSrl,assyColSrl);fflush(stdout);
											//printf("2.45  Part NO   Comparision:: %s %s",SchmPrtNo,AssyPartNo);fflush(stdout);
											t5CheckDstat(ulyAddObjectToSet(low_set_get(soSchmObj,t),&ColSchmSet));
											break;
										}

									}
									}
									else
									{
										if(nlsStrCmp(PartCatCode,assyCompCode)==0 && nlsStrCmp(subColSrl,assyColSrl)==0 )
										{
											printf("2.45  C. Inside Fields Match in Scheme for Scheme %s \n",SchemeName);fflush(stdout);
											//printf("2.45  Comp code Comparision:: %s %s",PartCatCode,assyCompCode);fflush(stdout);
											//printf("2.45  Colour ID Comparision:: %s %s",subColSrl,assyColSrl);fflush(stdout);
											//printf("2.45  Part NO   Comparision:: %s %s",SchmPrtNo,AssyPartNo);fflush(stdout);
											t5CheckDstat(ulyAddObjectToSet(low_set_get(soSchmObj,t),&ColSchmSet));
											break;
										}
									}
								}


							}
							goto L1;
						}

					}

				}
			}
		}
		/*******************************************************************************************************/


		if(nlsStrCmp(partType,"VC")==0)
		{
				printf("2.45  Inside uses colour part being used for VC   \n");fflush(stdout);
				t5CheckDstat(objGetAttribute(t5ItmRev,OBIDAttr,&VCObid)) ;

				printf("2.45  VC has OBID :%s\n",VCObid);fflush(stdout);
				if(sql)	oiSqlDispose(sql);	sql = NULL;
				if(SchmRelSet)
				{
					objDisposeAll(SchmRelSet);
					SchmRelSet = NULL;
				}

				if (dstat = oiSqlCreateSelect(&sql) |
					    oiSqlWhereBegParen(sql) |
					    oiSqlWhereEQ(sql,RightAttr,VCObid) |
					    oiSqlWhereAND(sql) |
					    oiSqlWhereEQ(sql,t5ColVCAttr,Inputpart) |
					    oiSqlWhereEndParen(sql))
				if(*mfail)goto CLEANUP;
				if (dstat = QueryDbObject(t5ClCfgIClass,sql,1,TRUE,SC_SCOPE_OF_SESSION,&SchmRelSet,mfail))   ;
				if(*mfail)goto CLEANUP;

				printf("2.45  No of Colour Scheme found setSize[SchmRelSet] :%d\n",setSize(SchmRelSet));fflush(stdout);
				SchmRelObj = setGet(SchmRelSet,0);
				t5CheckDstat(objGetAttribute(SchmRelObj ,LeftAttr,&LeftSchm)) ;
				printf("2.45  LeftSchm :%s \n",LeftSchm);fflush(stdout);

				if (dstat = oiSqlCreateSelect(&sql) |
					    oiSqlWhereBegParen(sql) |
					    oiSqlWhereEQ(sql,OBIDAttr,LeftSchm) |
					    oiSqlWhereEndParen(sql))
				if(*mfail)goto CLEANUP;
				if (dstat = QueryDbObject(t5ClSchmClass,sql,1,TRUE,SC_SCOPE_OF_SESSION,&ColSchmSet,mfail))   ;
				if(*mfail)goto CLEANUP;

				SchmObjE = setGet(ColSchmSet,0);

				if (dstat = objGetAttribute(SchmObjE,NomenclatureAttr,&SchmName)) goto EXIT;

				if(!nlsIsStrNull(EffectiveDate) && setSize(soCntrlObj)>0)
				{
					printf("2.45  Inside EffectiveDate option being used  \n");fflush(stdout);
					if(ColSchmSet) ColSchmSet=NULL;
					if (dstat = oiSqlCreateSelect(&sql) |
						    oiSqlWhereBegParen(sql) |
						    oiSqlWhereLE(sql,PlannedEffDateAttr,EffectiveDate)|
						    oiSqlWhereAND(sql)|
						    oiSqlWhereEQ(sql,NomenclatureAttr,SchmName) |
						    oiSqlWhereAND(sql) |
						//    oiSqlWhereEQ(sql, OwnerNameAttr,"Release Vault") |
						   oiSqlWhereLike(sql, OwnerNameAttr,"%Release Vault") |
						    oiSqlWhereEndParen(sql))
					if(*mfail)goto CLEANUP;
					if (dstat = QueryDbObject(t5ClSchmClass,sql,0,TRUE,SC_SCOPE_OF_SESSION,&ColSchmSet,mfail))   ;
					if(*mfail)goto CLEANUP;

					if (setSize(ColSchmSet) <1)
					{
						if (dstat = dlow_nvs_set(&textInserts,"EffectiveD",EffectiveDate)) goto EXIT;
						/*t5CheckDstat(uiShowText("SchemeNotFoundE", textInserts, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
						*mfail = NOT_OKAY;*/
						goto CLEANUP;
					}
				}
				else
				{
					printf("2.45  Inside EffectiveDate option not being used  \n");fflush(stdout);
					if(ColSchmSet) ColSchmSet=NULL;
					if (dstat = oiSqlCreateSelect(&sql) |
						    oiSqlWhereBegParen(sql) |
						    oiSqlWhereEQ(sql,NomenclatureAttr,SchmName) |
						    oiSqlWhereAND(sql) |
						//    oiSqlWhereEQ(sql, OwnerNameAttr,"Release Vault") |
						    oiSqlWhereLike(sql, OwnerNameAttr,"%Release Vault") |
						    oiSqlWhereEndParen(sql))
					if(*mfail)goto CLEANUP;
					if (dstat = QueryDbObject(t5ClSchmClass,sql,0,TRUE,SC_SCOPE_OF_SESSION,&ColSchmSet,mfail))   ;
					if(*mfail)goto CLEANUP;
				}
		}
		else
		{
			printf("2.45  Inside uses colour part not being used for VC  \n");fflush(stdout);
			if ((dstat = oiSqlCreateSelect(&a_sql)) ||
			(dstat = oiSqlWhereEQ(a_sql,ProjectNameAttr,projectcode)) ||
			(dstat = QueryWhere(t0NPProjClass,a_sql,&ProjectSet,mfail)) )	goto EXIT;
			if (*mfail) goto CLEANUP;
			printf("2.45  No of Projects Found:%d \n",setSize(ProjectSet));
			if(VehClass) VehClass=NULL;
			if (dstat = objGetAttribute(low_set_get(ProjectSet,0),t5VehicleClassAttr,&VehClass)) goto EXIT;
			printf("2.45  VehClass found for project %s:  %s \n",projectcode,VehClass);fflush(stdout);
			if(!nlsIsStrNull(VehClass))
			{
				VehClassDup = nlsStrDup(VehClass);
			}
			else
			{
				if (dstat = dlow_nvs_set(&textInserts,"projcode",projectcode)) goto EXIT;
				/*t5CheckDstat(uiShowText("t5VehClassNotFound", textInserts, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
				*mfail = NOT_OKAY;*/
				goto CLEANUP;
			}

			printf("2.45  EffectiveDate === %s \n",EffectiveDate);fflush(stdout);
			if(!nlsIsStrNull(EffectiveDate) && setSize(soCntrlObj)>0)
				{
			printf("2.45  Inside EffectiveDate option being used  \n");fflush(stdout);
			if(nlsStrCmp(Coated,"C")==0)
			{
				if((dstat = oiSqlCreateSelect(&Sql)) ||
				(dstat = oiSqlWhereBegParen(Sql)) ||
				(dstat = oiSqlWhereLE(Sql,PlannedEffDateAttr,EffectiveDate))||
				(dstat = oiSqlWhereAND(Sql))||
				(dstat = oiSqlWhereEQ(Sql,t5VehClassAttr,VehClassDup)) ||
				(dstat = oiSqlWhereAND(Sql)) ||
				(dstat = oiSqlWhereEQ(Sql, t5CoatedAttr,"Y")) ||
				(dstat = oiSqlWhereAND(Sql)) ||
				//(dstat = oiSqlWhereEQ(Sql, OwnerNameAttr,"Release Vault")) ||
				(dstat = oiSqlWhereLike(Sql, OwnerNameAttr,"%Release Vault")) ||
				(dstat = oiSqlWhereEndParen(Sql)) ||
				(dstat = QueryDbObject(t5ClSchmClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &soSchmObj, mfail))) goto CLEANUP;
				printf("2.45  A.Total Scheme found for Veh class==%s :::: Coated Ind== Y :::: with EffectiveDate date ==%s ::: is== %d\n",VehClassDup,EffectiveDate,setSize(soSchmObj));fflush(stdout);

			}
			else
			{
				if((dstat = oiSqlCreateSelect(&Sql)) ||
				(dstat = oiSqlWhereBegParen(Sql)) ||
				(dstat = oiSqlWhereLE(Sql,PlannedEffDateAttr,EffectiveDate))||
				(dstat = oiSqlWhereAND(Sql))||
				(dstat = oiSqlWhereEQ(Sql,t5VehClassAttr,VehClassDup)) ||
				(dstat = oiSqlWhereAND(Sql)) ||
				//(dstat = oiSqlWhereEQ(Sql, OwnerNameAttr,"Release Vault")) ||
				(dstat = oiSqlWhereLike(Sql, OwnerNameAttr,"%Release Vault")) ||
				(dstat = oiSqlWhereEndParen(Sql)) ||
				(dstat = QueryDbObject(t5ClSchmClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &soSchmObj, mfail))) goto CLEANUP;
				printf("2.45  B.Total Scheme found for Veh class==%s :::: Coated Ind== N :::: with EffectiveDate date ==%s ::: is== %d\n",VehClassDup,EffectiveDate,setSize(soSchmObj));fflush(stdout);
			}
		}
		else
		{
			printf("2.45  Inside EffectiveDate option not being used  \n");fflush(stdout);
			if(nlsStrCmp(Coated,"C")==0)
			{
				if((dstat = oiSqlCreateSelect(&Sql)) ||
				(dstat = oiSqlWhereBegParen(Sql)) ||
				(dstat = oiSqlWhereEQ(Sql,t5VehClassAttr,VehClassDup)) ||
				(dstat = oiSqlWhereAND(Sql)) ||
				(dstat = oiSqlWhereEQ(Sql, t5CoatedAttr,"Y")) ||
				(dstat = oiSqlWhereAND(Sql)) ||
			//	(dstat = oiSqlWhereEQ(Sql, OwnerNameAttr,"Release Vault")) ||
			(dstat = oiSqlWhereLike(Sql, OwnerNameAttr,"%Release Vault")) ||
				(dstat = oiSqlWhereEndParen(Sql)) ||
				(dstat = QueryDbObject(t5ClSchmClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &soSchmObj, mfail))) goto CLEANUP;
				printf("2.45  C.Total Scheme found for Veh class==%s :::: Coated Ind== Y :::: is== %d\n",VehClassDup,setSize(soSchmObj));fflush(stdout);
			}
			else
			{
				if((dstat = oiSqlCreateSelect(&Sql)) ||
				(dstat = oiSqlWhereBegParen(Sql)) ||
				(dstat = oiSqlWhereEQ(Sql,t5VehClassAttr,VehClassDup)) ||
				(dstat = oiSqlWhereAND(Sql)) ||
				//(dstat = oiSqlWhereEQ(Sql, OwnerNameAttr,"Release Vault")) ||
				(dstat = oiSqlWhereLike(Sql, OwnerNameAttr,"%Release Vault")) ||
				(dstat = oiSqlWhereEndParen(Sql)) ||
				(dstat = QueryDbObject(t5ClSchmClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &soSchmObj, mfail))) goto CLEANUP;
				printf("2.45  D.Total Scheme found for Veh class==%s :::: Coated Ind== N :::: is== %d\n",VehClassDup,setSize(soSchmObj));fflush(stdout);
			}
		}

		if (setSize(soSchmObj)<1)
		{

			if (dstat = dlow_nvs_set(&textInserts,"VehClass",VehClassDup)) goto EXIT;
			if(dstat = uiShowConfirm("SchemeHelp01", textInserts, UI_MODAL_QUESTION, &ans)) goto EXIT;
			if(!ans)
			{
				printf("2.45  Inside No Option selected  \n");fflush(stdout);
				*mfail=NOT_OKAY;
				goto EXIT;
			}
			else
			{
				printf("2.45  Inside Query with other Veh Class:: showing Veh Class  \n");fflush(stdout);
				low_set_add_str_unique(distinctVehClass,"BUS");
				low_set_add_str_unique(distinctVehClass,"CAR");
				low_set_add_str_unique(distinctVehClass,"HCV");
				low_set_add_str_unique(distinctVehClass,"LCV");
				low_set_add_str_unique(distinctVehClass,"LMV");

				low_set_add_str_unique(distinctVehClass,"MCV");
				low_set_add_str_unique(distinctVehClass,"PRD");
				low_set_add_str_unique(distinctVehClass,"UV_VAN");
				low_set_add_str_unique(distinctVehClass,"SEMI VEHICLE");

				if(dstat = ConstructDialog("ShwVLSX",NULL,NULL,&extraStr,&extraObj,mfail,&dialogObj2)) goto EXIT;
				if(*mfail) goto EXIT;


				for (t=0; t<setSize(distinctVehClass); t++ )
				{
					printf("2.45  ****distinct Veh Class found %d \n",setSize(distinctVehClass));
					tmpVehClass=low_set_get(distinctVehClass,t);
					printf("2.45  Veh Class...%s",tmpVehClass);
					if(!nlsIsStrNull(tmpVehClass))
					{
						if(dstat=objNameValueSet(dialogObj2,t5ShwVehClassListAttr,tmpVehClass,"-")) goto EXIT;
					}

				}

				if(dstat = InteractWithUser(dialogObj2,NULL,NULL,&extraStr,&extraObj,mfail)) goto EXIT;
				printf("2.45  After InteractWithUser.... .... \n");fflush(stdout);

				if(dstat = objNameValueSize(dialogObj2, t5ShwVehClassListAttr,&VehListSize)) goto EXIT;
				if(dstat = objCopy(&orgObj1,dialogObj2)) goto EXIT;

				for(i=0;i<VehListSize;i++)
				{
					if(VehTypeList) VehTypeList=NULL;
					if(Veh_value) Veh_value=NULL;
					if(dstat = objNameValueGet_byi(orgObj1,t5ShwVehClassListAttr,i,&VehTypeList,&Veh_value)) goto EXIT;

					if(nlsStrCmp(Veh_value,"+")== 0)
					{
						printf("2.45  Veh Class Selected ...%s %s",VehTypeList,Veh_value);
						printf("2.45  Inside second loop of Query Scheme for Veh Class === %s \n",VehTypeList);fflush(stdout);
						if(!nlsIsStrNull(EffectiveDate) && setSize(soCntrlObj)>0)
						{
							printf("2.45  Inside EffectiveDate option being used  \n");fflush(stdout);
							if(nlsStrCmp(Coated,"C")==0)
							{
								if((dstat = oiSqlCreateSelect(&Sql)) ||
								(dstat = oiSqlWhereBegParen(Sql)) ||
								(dstat = oiSqlWhereLE(Sql,PlannedEffDateAttr,EffectiveDate))||
								(dstat = oiSqlWhereAND(Sql))||
								(dstat = oiSqlWhereEQ(Sql,t5VehClassAttr,VehTypeList)) ||
								(dstat = oiSqlWhereAND(Sql)) ||
								(dstat = oiSqlWhereEQ(Sql, t5CoatedAttr,"Y")) ||
								(dstat = oiSqlWhereAND(Sql)) ||
								//(dstat = oiSqlWhereEQ(Sql, OwnerNameAttr,"Release Vault")) ||
								(dstat = oiSqlWhereLike(Sql, OwnerNameAttr,"%Release Vault")) ||
								(dstat = oiSqlWhereEndParen(Sql)) ||
								(dstat = QueryDbObject(t5ClSchmClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &soSchmObj, mfail))) goto CLEANUP;
								printf("2.45  A.Total Scheme found for Veh class==%s :::: Coated Ind== Y :::: with EffectiveDate date ==%s ::: is== %d\n",VehTypeList,EffectiveDate,setSize(soSchmObj));fflush(stdout);

							}
							else
							{
								if((dstat = oiSqlCreateSelect(&Sql)) ||
								(dstat = oiSqlWhereBegParen(Sql)) ||
								(dstat = oiSqlWhereLE(Sql,PlannedEffDateAttr,EffectiveDate))||
								(dstat = oiSqlWhereAND(Sql))||
								(dstat = oiSqlWhereEQ(Sql,t5VehClassAttr,VehTypeList)) ||
								(dstat = oiSqlWhereAND(Sql)) ||
							//	(dstat = oiSqlWhereEQ(Sql, OwnerNameAttr,"Release Vault")) ||
							        (dstat = oiSqlWhereLike(Sql, OwnerNameAttr,"%Release Vault")) ||
								(dstat = oiSqlWhereEndParen(Sql)) ||
								(dstat = QueryDbObject(t5ClSchmClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &soSchmObj, mfail))) goto CLEANUP;
								printf("2.45  B.Total Scheme found for Veh class==%s :::: Coated Ind== N :::: with EffectiveDate date ==%s ::: is== %d\n",VehTypeList,EffectiveDate,setSize(soSchmObj));fflush(stdout);
							}
						}
						else
						{
							printf("2.45  Inside EffectiveDate option not being used  \n");fflush(stdout);
							if(nlsStrCmp(Coated,"C")==0)
							{
								if((dstat = oiSqlCreateSelect(&Sql)) ||
								(dstat = oiSqlWhereBegParen(Sql)) ||
								(dstat = oiSqlWhereEQ(Sql,t5VehClassAttr,VehTypeList)) ||
								(dstat = oiSqlWhereAND(Sql)) ||
								(dstat = oiSqlWhereEQ(Sql, t5CoatedAttr,"Y")) ||
								(dstat = oiSqlWhereAND(Sql)) ||
							//	(dstat = oiSqlWhereEQ(Sql, OwnerNameAttr,"Release Vault")) ||
							        (dstat = oiSqlWhereLike(Sql, OwnerNameAttr,"%Release Vault")) ||
								(dstat = oiSqlWhereEndParen(Sql)) ||
								(dstat = QueryDbObject(t5ClSchmClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &soSchmObj, mfail))) goto CLEANUP;
								printf("2.45  C.Total Scheme found for Veh class==%s :::: Coated Ind== Y :::: is== %d\n",VehTypeList,setSize(soSchmObj));fflush(stdout);
							}
							else
							{
								if((dstat = oiSqlCreateSelect(&Sql)) ||
								(dstat = oiSqlWhereBegParen(Sql)) ||
								(dstat = oiSqlWhereEQ(Sql,t5VehClassAttr,VehTypeList)) ||
								(dstat = oiSqlWhereAND(Sql)) ||
							//	(dstat = oiSqlWhereEQ(Sql, OwnerNameAttr,"Release Vault")) ||
								(dstat = oiSqlWhereLike(Sql, OwnerNameAttr,"%Release Vault")) ||
								(dstat = oiSqlWhereEndParen(Sql)) ||
								(dstat = QueryDbObject(t5ClSchmClass, Sql, 0, TRUE, SC_SCOPE_OF_SESSION, &soSchmObj, mfail))) goto CLEANUP;
								printf("2.45  D.Total Scheme found for Veh class==%s :::: Coated Ind== N :::: is== %d\n",VehTypeList,setSize(soSchmObj));fflush(stdout);
							}
						}


					}
				}


			}
		}
		/*************************************************************/

		printf("2.45  IntSchmIndDup %s   \n",IntSchmIndDup);fflush(stdout);
		for(t=0;t<setSize(soSchmObj);t++)
		{

			if(dstat =objGetTableSize(low_set_get(soSchmObj,t),t5SchFullTabAttr,&TabSize)) goto EXIT;
			//printf("2.45  TabSize is %d", TabSize);fflush(stdout);
			if (dstat = objGetAttribute(low_set_get(soSchmObj,t),NomenclatureAttr,&SchemeName)) goto EXIT;
			printf("2.45  %d. Filtering the Schemes %s for %s amd %s  : and %s \n",t,SchemeName,assyCompCode,assyColSrl,AssyPartNo);fflush(stdout);

			for (p=0;p<TabSize;p++)
			{

				if(dstat = objGetTableAttribute(low_set_get(soSchmObj,t),t5SchFullTabAttr,p,t5PrtCatCodeAttr,&PartCatCode)) goto EXIT;
				if(dstat = objGetTableAttribute(low_set_get(soSchmObj,t),t5SchFullTabAttr,p,t5ClSrlAttr,&ColSrl)) goto EXIT;
				if(dstat = objGetTableAttribute(low_set_get(soSchmObj,t),t5SchFullTabAttr,p,t5SchmPrtCAttr,&SchmPrtNo)) goto EXIT;

				if((!nlsIsStrNull(PartCatCode))&&(!nlsIsStrNull(ColSrl)))
				{
					strLen=strlen(ColSrl);
					subColSrl=subString(ColSrl,3,strLen);
					nlsStrTrimTrailWhiteSpace(subColSrl);
					//printf("2.45  Checking in Full table: PartCatCode  ColSrl & Scheme Part No: %s ,%s ,%s \n",PartCatCode,subColSrl,SchmPrtNo);fflush(stdout);
				}

				if (nlsStrCmp(IntSchmIndDup,"T")==0)
				{
					if(!nlsIsStrNull(SchmPrtNo))
					{
						nlsStrTrimTrailWhiteSpace(SchmPrtNo);

						if(nlsStrCmp(PartCatCode,assyCompCode)==0 && nlsStrCmp(subColSrl,assyColSrl)==0 && nlsStrStr(SchmPrtNo,AssyPartNo)!=NULL)
						{
							printf("2.45  A. Inside Fields Match in Scheme for Scheme %s \n",SchemeName);fflush(stdout);
							//printf("2.45  Comp code Comparision:: %s %s",PartCatCode,assyCompCode);fflush(stdout);
							//printf("2.45  Colour ID Comparision:: %s %s",subColSrl,assyColSrl);fflush(stdout);
							//printf("2.45  Part NO   Comparision:: %s %s",SchmPrtNo,AssyPartNo);fflush(stdout);
							t5CheckDstat(ulyAddObjectToSet(low_set_get(soSchmObj,t),&ColSchmSet));
							break;
						}
					}
					else
					{
						if(nlsStrCmp(PartCatCode,assyCompCode)==0 && nlsStrCmp(subColSrl,assyColSrl)==0 )
						{
							printf("2.45  B. Inside Fields Match in Scheme for Scheme %s \n",SchemeName);fflush(stdout);
							//printf("2.45  Comp code Comparision:: %s %s",PartCatCode,assyCompCode);fflush(stdout);
							//printf("2.45  Colour ID Comparision:: %s %s",subColSrl,assyColSrl);fflush(stdout);
							//printf("2.45  Part NO   Comparision:: %s %s",SchmPrtNo,AssyPartNo);fflush(stdout);
							t5CheckDstat(ulyAddObjectToSet(low_set_get(soSchmObj,t),&ColSchmSet));
							break;
						}
					}
				}
				else
				{
					if(nlsStrCmp(PartCatCode,assyCompCode)==0 && nlsStrCmp(subColSrl,assyColSrl)==0 )
					{
						printf("2.45  C. Inside Fields Match in Scheme for Scheme %s \n",SchemeName);fflush(stdout);
						//printf("2.45  Comp code Comparision:: %s %s",PartCatCode,assyCompCode);fflush(stdout);
						//printf("2.45  Colour ID Comparision:: %s %s",subColSrl,assyColSrl);fflush(stdout);
						//printf("2.45  Part NO   Comparision:: %s %s",SchmPrtNo,AssyPartNo);fflush(stdout);
						t5CheckDstat(ulyAddObjectToSet(low_set_get(soSchmObj,t),&ColSchmSet));
						break;
					}
				}
			}


		}
		}
		L1:;
		printf("2.45   Total Scheme Found for assyCompCode : %s assyColSrl: %s projectcode %s PartNumber: %s ::::: %d \n",assyCompCode,assyColSrl,projectcode,AssyPartNo,setSize(ColSchmSet));fflush(stdout);

		if (setSize(ColSchmSet) <1)
		{
			/*if (dstat = dlow_nvs_set(&textInserts,"CLPART",Inputpart)) goto EXIT;
			t5CheckDstat(uiShowText("t5NoSchemeFoundErr", textInserts, UI_ATTENTION_TEXT, dstat, WHERE) ) ;*/
			goto CLEANUP;
		}

		if (setSize(ColSchmSet) >0)
		{
			printf("2.45  Total scheme selected by user : %d\n",setSize(ColSchmSet));fflush(stdout);
			t5CheckMfail(t5SortBOMObjsByDate(&ColSchmSet,&ColSchmSet,mfail));
			printf("2.45  After shorting by Date: %d\n",setSize(ColSchmSet));fflush(stdout);
			for(t=0;t<setSize(ColSchmSet);t++)
			{
				if (dstat = objGetAttribute(low_set_get(ColSchmSet,t),NomenclatureAttr,&SchemeName)) goto EXIT;
				if (dstat = objGetAttribute(low_set_get(ColSchmSet,t),RevisionAttr,&SchemeRev)) goto EXIT;
				if (dstat = objGetAttribute(low_set_get(ColSchmSet,t),SequenceAttr,&SchemeSeq)) goto EXIT;
				printf("2.45  %d===== : %s %s %s\n",t,SchemeName,SchemeRev,SchemeSeq);fflush(stdout);

			}
			SchIndex=setSize(ColSchmSet);
			SchIndex=SchIndex-1;

			SchmObj=low_set_get(ColSchmSet,SchIndex);

				if(dstat = objCopy(&tmpSchmObj,SchmObj)) goto EXIT;
				if(dstat =objGetTableSize(tmpSchmObj,t5SchFullTabAttr,&TabSize)) goto EXIT;
				if (IscoatedSchm)   IscoatedSchm=NULL;
				if (dstat = objGetAttribute(SchmObj,t5CoatedAttr,&IscoatedSchm)) goto EXIT;
				if(!nlsIsStrNull(IscoatedSchm)) IscoatedSchmDup = nlsStrDup(IscoatedSchm);

				printf("2.45  Is coated Schm : %s \n",IscoatedSchmDup);fflush(stdout);

				ssCs = low_set_create_str(TabSize);
				for (q=0;q<TabSize;q++)
				{
					if(dstat = objGetTableAttribute(tmpSchmObj,t5SchFullTabAttr,q,t5PrtCatCodeAttr,&PartCatCode)) goto EXIT;
					if(dstat = objGetTableAttribute(tmpSchmObj,t5SchFullTabAttr,q,t5ClSrlAttr,&ColorSrl)) goto EXIT;
					//printf("2.45  PartCatCode & ColorSrl : %s ,%s \n",PartCatCode,ColorSrl);fflush(stdout);

					if((!nlsIsStrNull(PartCatCode))&&(!nlsIsStrNull(ColorSrl)))
					{
						strLen=nlsStrLen(ColorSrl);
						subColSrl=subString(ColorSrl,3,strLen);
						nlsStrTrimTrailWhiteSpace(subColSrl);
						//printf("2.45  PartCatCode & subColSrl : %s ,%s \n",PartCatCode,subColSrl);fflush(stdout);

						nlsStrCpy(colourScheme,PartCatCode);
						nlsStrCat(colourScheme,"_") ;
						nlsStrCat(colourScheme,subColSrl) ;
						//printf("2.45  colourScheme :%s \n",colourScheme);fflush(stdout);
						low_set_add_str(ssCs,colourScheme);
					}

				}
				printf("2.45  Size[ssCs] :%d \n",setSize(ssCs));fflush(stdout);

				ColourPartSet=setCreate(100);

				if(ShowInWindow == TRUE) //Gss User Ctxt
				{
					t5CheckMfail(IntSetUpContext(objClass(t5ItmRev),t5ItmRev,&genContextObjOP,mfail));
					t5CheckMfail(GetCfgCtxtObjFromCtxt(DSesGnCClass, genContextObjOP, &contextObjOP, mfail));
					t5CheckMfail(objSetAttribute(contextObjOP,CfgItemIdAttr,"GlobalCtxt"));
					//t5CheckDstat(objSetAttribute(contextObjOP,EffectiveDateAttr,""));
					t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsAplRlzd","+"));
					t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsErcRlzd","+"));
					t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsReview","-"));
					t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsWorking","-"));
					t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsAPLWrkg","+"));
					t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsSTDRlzd","+"));
					t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsSTDWrkg","+"));
					t5CheckDstat(objSetObject(genContextObjOP,ConfigCtxtBlobAttr,contextObjOP));
					t5CheckMfail(SetExpOnRevInCtxt(DSesGnCClass,genContextObjOP,"PscLastRev",mfail));

					t5FreeSetOfObjects(childObjSO);
					t5CheckMfail(ExpandRelationWithCtxt("AsRevRev",
								t5ItmRev,
								"PartsInAssembly",
								 genContextObjOP,
								 SC_SCOPE_OF_SESSION,
								 NULL,
								 &childObjSO,
								&childRelObjSO,
								mfail))  ;
				}
				else //Gss Passed Ctxt 2.45 genContextObjSTD
				{
					t5CheckMfail(ExpandRelationWithCtxt("AsRevRev",
								t5ItmRev,
								"PartsInAssembly",
								 genContextObjSTD,
								 SC_SCOPE_OF_SESSION,
								 NULL,
								 &childObjSO,
								&childRelObjSO,
								mfail))  ;
				}

				printf("2.45   No of Uses Parts found : %d \n ",setSize(childObjSO));fflush(stdout);

				if(setSize(childObjSO)>0)
				{
					for(i=0;i<setSize(childObjSO);i++)
					{

					//printf("2.45  Inside childObjSO found i is :%d\n ",i);fflush(stdout);
					childObj=setGet(childObjSO,i);
					if (Iscoated)   Iscoated=NULL;
					if (colourInd)   colourInd=NULL;
					if (PartType)   PartType=NULL;
					if (dstat = objGetAttribute(childObj,t5CoatedAttr,&Iscoated)) goto EXIT;
					if(!nlsIsStrNull(Iscoated)) IscoatedDup = nlsStrDup(Iscoated);
					if (dstat = objGetAttribute(childObj,t5ColourIndAttr,&colourInd)) goto EXIT;
					if(!nlsIsStrNull(colourInd)) colourIndDup = nlsStrDup(colourInd);
					if (dstat = objGetAttribute(childObj,PartTypeAttr,&PartType)) goto EXIT;
					if(!nlsIsStrNull(PartType)) PartTypeDup = nlsStrDup(PartType);

					if (dstat = objGetAttribute(childObj,t5PrtCatCodeAttr,&comCode)) goto EXIT;
					if(!nlsIsStrNull(comCode)) comCodeDup = nlsStrDup(comCode);

					t5CheckMfail(ExpandObject2(ItemRevClass,childObj,"ItemMstrForStrucBIRev",SC_SCOPE_OF_SESSION,&t5ItmMstr,&t5ItmMstrRel,mfail)) ;
					//printf("2.45  SetSize t5ItmMstr: %d \n ",setSize(t5ItmMstr));fflush(stdout);
					itemRevObj=setGet(t5ItmMstr,0);

					t5CheckMfail(ExpandObject5("t5ClInfo",itemRevObj,"t5AsmhasColPrt",SC_SCOPE_OF_SESSION,&colourParts,mfail)) ;
					printf("2.45  SetSize colourParts: %d \n ",setSize(colourParts));fflush(stdout);
					if (setSize(colourParts)>0)
					{
						AssyHasFlag=FALSE;
						for(k=0;k<setSize(colourParts);k++)
						{
							//printf("2.45  Inside colourParts found k is :%d\n ",k);fflush(stdout);
							colourPart=low_set_get(colourParts,k);
							if (dstat = objGetAttribute(colourPart,t5ColourIDAttr,&colourID)) goto EXIT;
							if(!nlsIsStrNull(colourID)) colourIDDup = nlsStrDup(colourID);

							if(nlsStrCmp(colourIndDup,"Y")==0)
							{

							//printf("2.45  comCode & colourID : %s ,%s \n",comCodeDup,colourIDDup);fflush(stdout);
							nlsStrCpy(assyHasClSch,comCodeDup);
							nlsStrCat(assyHasClSch,"_") ;
							nlsStrCat(assyHasClSch,colourIDDup) ;
							//printf("2.45  assyHasClSch :%s \n",assyHasClSch);fflush(stdout);
							for(c=0;c<setSize(ssCs);c++)
							{
								//printf("2.45  Inside Inner loop (VCHasClSch) Group\n");fflush(stdout);
								vcCS=low_set_get_str(ssCs,c);
								if(!nlsIsStrNull(vcCS)) vcCSDup = nlsStrDup(vcCS);
								//printf("2.45  vcCSDup Group: %s \n",vcCSDup);fflush(stdout);
								if(nlsStrCmp(vcCSDup,assyHasClSch)==0)
								{
									printf("2.45  Comp Code & Colour ID match: %s %s\n",vcCSDup,assyHasClSch);fflush(stdout);

									if(nlsStrCmp(PartTypeDup,"V")==0 || nlsStrCmp(PartTypeDup,"VC")==0)
									{
									AssyHasFlag=TRUE;
									low_set_add(ColourPartSet,colourPart);
									break;
									}
									else
									{
									if(nlsStrCmp(IscoatedSchmDup,"Y")==0)
									{
										if(nlsStrCmp(colourIndDup,"Y")==0 && nlsStrCmp(IscoatedDup,"Y")==0)
										{
											if (CoatedType)   CoatedType=NULL;
											if (dstat = objGetAttribute(colourPart,t5CoatedAttr,&CoatedType)) goto EXIT;
											if(!nlsIsStrNull(CoatedType)) CoatedTypeDup = nlsStrDup(CoatedType);

											if (dstat = objGetAttribute(colourPart,t5ColourIndAttr,&ccolourInd)) goto EXIT;
											if(!nlsIsStrNull(ccolourInd)) ccolourIndDup = nlsStrDup(ccolourInd);

											printf("2.45  Colour YES Coated YES: %s %s",ccolourIndDup,CoatedTypeDup);
											if(nlsStrCmp(CoatedTypeDup,"C")==0 && nlsStrCmp(ccolourIndDup,"C")==0 )
											{
												low_set_add(ColourPartSet,colourPart);
												printf("2.45  After Adding  Colour  Coated Part SetSize ColourPartSet: %d \n ",setSize(ColourPartSet));fflush(stdout);
												AssyHasFlag=TRUE;
												break;
											}
										}
										if(nlsStrCmp(colourIndDup,"Y")==0 && nlsStrCmp(IscoatedDup,"N")==0)
										{
											if (CoatedType)   CoatedType=NULL;
											if (dstat = objGetAttribute(colourPart,t5CoatedAttr,&CoatedType)) goto EXIT;
											if(!nlsIsStrNull(CoatedType)) CoatedTypeDup = nlsStrDup(CoatedType);

											if (dstat = objGetAttribute(colourPart,t5ColourIndAttr,&ccolourInd)) goto EXIT;
											if(!nlsIsStrNull(ccolourInd)) ccolourIndDup = nlsStrDup(ccolourInd);

											printf("2.45  Colour YES Coated YES: %s %s",ccolourIndDup,CoatedTypeDup);
											if(nlsStrCmp(CoatedTypeDup,"N")==0 && nlsStrCmp(ccolourIndDup,"C")==0 )
											{
												low_set_add(ColourPartSet,colourPart);
												printf("2.45  After Finding  Colour  Coated Part SetSize ColourPartSet: %d \n ",setSize(ColourPartSet));fflush(stdout);

												AssyHasFlag=TRUE;
												break;
											}
										}

									}
									else
									{
										if (CoatedType)   CoatedType=NULL;
										if (dstat = objGetAttribute(colourPart,t5CoatedAttr,&CoatedType)) goto EXIT;
										if(!nlsIsStrNull(CoatedType)) CoatedTypeDup = nlsStrDup(CoatedType);

										if (dstat = objGetAttribute(colourPart,t5ColourIndAttr,&ccolourInd)) goto EXIT;
										if(!nlsIsStrNull(ccolourInd)) ccolourIndDup = nlsStrDup(ccolourInd);

										printf("2.45  Colour YES Coated NO: %s %s",ccolourIndDup,CoatedTypeDup);
										if(nlsStrCmp(CoatedTypeDup,"N")==0 && nlsStrCmp(ccolourIndDup,"C")==0 )
										{
										low_set_add(ColourPartSet,colourPart);
										printf("2.45  After Adding  colour Part SetSize ColourPartSet: %d \n ",setSize(ColourPartSet));fflush(stdout);
										AssyHasFlag=TRUE;
										break;
										}
									}
									}
								}
							}
							}
							else
							{
							if(nlsStrCmp(IscoatedSchmDup,"Y")==0)
							{
							if((nlsStrCmp(colourIndDup,"N")==0 && nlsStrCmp(IscoatedDup,"Y")==0))
							{
								if (CoatedType)   CoatedType=NULL;
								if (dstat = objGetAttribute(colourPart,t5CoatedAttr,&CoatedType)) goto EXIT;
								if(!nlsIsStrNull(CoatedType)) CoatedTypeDup = nlsStrDup(CoatedType);
								printf("2.45  Colour NO Coated YES: %s",CoatedTypeDup);
								if(nlsStrCmp(CoatedTypeDup,"C")==0)
								{
									low_set_add(ColourPartSet,colourPart);
									printf("2.45  After Adding  Coated Part SetSize ColourPartSet: %d \n ",setSize(ColourPartSet));fflush(stdout);
									AssyHasFlag=TRUE;

								}
							}
							}
							}
							if(AssyHasFlag==TRUE)
							break;

						}
					}
					if((AssyHasFlag !=TRUE)||(setSize(colourParts)<1))
					{
						low_set_add(ColourPartSet,childObj);
						printf("2.45 After Adding  Basic Parts SetSize ColourPartSet: %d \n ",setSize(ColourPartSet));fflush(stdout);
					}


					}
				}

				// Activation of the current window is required before generating new browser
				/******* Gss 2.45 **********/
				if (ShowInWindow == TRUE )
				{
					if(setSize(ColourPartSet)>0)
					{
					if (dstat = objGetAttribute(SchmObj,NomenclatureAttr,&SchName)) goto EXIT;
					if (dstat = objGetAttribute(SchmObj,RevisionAttr,&SchRev)) goto EXIT;
					if (dstat = objGetAttribute(SchmObj,SequenceAttr,&SchSeq)) goto EXIT;
					if (dstat = objGetAttribute(SchmObj,t5ClSrlAttr,&ColorSrl)) goto EXIT;
					if (dstat = objGetAttribute(SchmObj,t5ClSchmDescAttr,&SchemeDesc)) goto EXIT;
					if (dstat = objGetAttribute(SchmObj,t5ColourIDAttr,&colourIDDup)) goto EXIT;

					//printf("2.45  Colour Scheme :%s ColorSrl: %s Rev :%s & Seq : %s \n",SchName,ColorSrl,SchRev,SchSeq);fflush(stdout);

					nlsStrCpy(schemeName,SchName);
					nlsStrCat(schemeName, ",");
					nlsStrCat(schemeName, SchRev);
					nlsStrCat(schemeName, ",");
					nlsStrCat(schemeName, SchSeq);
					nlsStrCat(schemeName, ",");
					nlsStrCat(schemeName,ColorSrl);
					nlsStrCat(schemeName, "-");
					nlsStrCat(schemeName,colourIDDup);
					nlsStrCat(schemeName, " ");
					nlsStrCat(schemeName, "FOR ");
					nlsStrCat(schemeName,Inputpart);
					nlsStrCat(schemeName, "   ");
					nlsStrCat(schemeName, "DESC-");
					nlsStrCat(schemeName, SchemeDesc);
					//printf("2.45  schemeName : %s \n",schemeName);fflush(stdout);


//					t5CheckDstat(uiActivateBrowserWindow());
//					t5CheckMfail(GenerateBrowser(PartClass,BrwRelRghtValue,NULL,NULL,mfail));
//					t5CheckMfail(ShowListInBrowser(PartClass,PDO_BROWSER_ADD_TO,ColourPartSet,NULL,NULL,NULL,mfail));
//					t5CheckMfail(uiSetWindowTitle (schemeName));
//					t5CheckDstat(uiSetBrowserView("BrwListView"));
//					// Activating the browser window after adding set
//					t5CheckDstat(uiActivateBrowserWindow());

					t5CheckDstat(uiActivateBrowserWindow());
					for (i=0;i<setSize(ColourPartSet) ;i++ )
					{
						if(dstat = RefreshObject2(setGet(ColourPartSet,i),TRUE,mfail)) goto EXIT;
						if(dstat = RefreshItObj(setGet(ColourPartSet,i),TRUE,TRUE,&bln,&Obj,mfail)) goto EXIT;
					}
					t5CheckMfail(GenerateBrowser(PartClass,BrwNewWinValue,NULL,NULL,mfail));
					t5CheckDstat(uiSetBrowserView("BrwListView"));

					uiGetWindowObject(&winObj);
					SetColTableOnBrw("d5ColStr",winObj,NULL,NULL,mfail);

					uiSetListViewColumns(winObj);


					t5CheckMfail(ShowListInBrowser(PartClass,PDO_BROWSER_ADD_TO,ColourPartSet,NULL,NULL,NULL,mfail));

					  t5CheckMfail(uiSetWindowTitle (schemeName));


					t5CheckDstat(uiActivateBrowserWindow());


					}
					else
					{
						/*t5CheckDstat(uiShowText("t5ColourStrErr1", NULL, UI_ATTENTION_TEXT, dstat, WHERE) ) ;
						*mfail = NOT_OKAY;*/
						goto CLEANUP;
					}
				}
				else
				{
					printf("2.45 Return in a SET :%d:\n",setSize(ColourPartSet));fflush(stdout);
					//ColourPartSet
					*UsesColParts=ColourPartSet;
				}
				/******* Gss 2.45 **********/
		}
	}


	CLEANUP:
	EXIT:
	t5CheckDstatAndReturn;
};

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
		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsAPLWrkg","-"));
		 t5CheckDstat(objNameValueSet(contextObjOP,LCStateListAttr,"LcsErcRlzd","-"));
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
	string			APLReveffDatePrevToDup=NULL;
	string			StdReveffDatePrevToDup=NULL;




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
			fprintf(fp_DMLPartInfo,"-^");
		}

		if(!nlsIsStrNull(APLReveffDateToDup))			//PARTNUMBER APL EFF TO DATE;
		{
			fprintf(fp_DMLPartInfo,"%s",APLReveffDateToDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo,"-^");
		}

		if(!nlsIsStrNull(stdReveffDateFromDup))			//PARTNUMBER STD EFF FROM DATE;
		{
			fprintf(fp_DMLPartInfo,"%s",stdReveffDateFromDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo,"-^");
		}

		if(!nlsIsStrNull(stdReveffDateToDup))		//PARTNUMBER STD EFF TO DATE;
		{
			fprintf(fp_DMLPartInfo,"%s",stdReveffDateToDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo,"-^");
		}

		if((!nlsIsStrNull(APLReveffDateFromDup)) && (nlsStrCmp(APLReveffDateFromDup,"-")!=0))		//PARTNUMBER APL RLZD FROM DATE;
		{
			
			printf("\n APLReveffDateFromDup :%s",APLReveffDateFromDup);fflush(stdout);
			APLReveffDatePrevToDup = sysGetOffsetDate(APLReveffDateFromDup, 0, -1, 0);
			
			fprintf(fp_DMLPartInfo,"%s",APLReveffDatePrevToDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo,"-^");
		}

		if((!nlsIsStrNull(stdReveffDateFromDup)) && (nlsStrCmp(stdReveffDateFromDup,"-")!=0))			///PARTNUMBER STD RLZD FROM DATE;
		{
			printf("\n stdReveffDateFromDup :%s",stdReveffDateFromDup);fflush(stdout);
			StdReveffDatePrevToDup = sysGetOffsetDate(stdReveffDateFromDup, 0, -1, 0);

			fprintf(fp_DMLPartInfo,"%s",StdReveffDatePrevToDup);
			fprintf(fp_DMLPartInfo,"^");
		}
		else
		{
			fprintf(fp_DMLPartInfo,"-^");
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
				fprintf(fp_DMLPartInfo,"-^");
			}

			if(!nlsIsStrNull(APLReveffDateToDup))			//PARTNUMBER APL EFF TO DATE;
			{
				fprintf(fp_DMLPartInfo,"%s",APLReveffDateToDup);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo,"-^");
			}

			if(!nlsIsStrNull(stdReveffDateFromDup))			//PARTNUMBER STD EFF FROM DATE;
			{
				fprintf(fp_DMLPartInfo,"%s",stdReveffDateFromDup);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo,"-^");
			}

			if(!nlsIsStrNull(stdReveffDateToDup))		//PARTNUMBER STD EFF TO DATE;
			{
				fprintf(fp_DMLPartInfo,"%s",stdReveffDateToDup);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo,"-^");
			}

			if((!nlsIsStrNull(APLReveffDateFromDup)) && (nlsStrCmp(APLReveffDateFromDup,"-")!=0))		//PARTNUMBER APL RLZD FROM DATE;
			{
				
				printf("\n APLReveffDateFromDup :%s",APLReveffDateFromDup);fflush(stdout);
				APLReveffDatePrevToDup = sysGetOffsetDate(APLReveffDateFromDup, 0, -1, 0);
				
				fprintf(fp_DMLPartInfo,"%s",APLReveffDatePrevToDup);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo,"-^");
			}

			if((!nlsIsStrNull(stdReveffDateFromDup)) && (nlsStrCmp(stdReveffDateFromDup,"-")!=0))			///PARTNUMBER STD RLZD FROM DATE;
			{
				printf("\n stdReveffDateFromDup :%s",stdReveffDateFromDup);fflush(stdout);
				StdReveffDatePrevToDup = sysGetOffsetDate(stdReveffDateFromDup, 0, -1, 0);

				fprintf(fp_DMLPartInfo,"%s",StdReveffDatePrevToDup);
				fprintf(fp_DMLPartInfo,"^");
			}
			else
			{
				fprintf(fp_DMLPartInfo,"-^");
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

	 objDump(cntxtForAPLView);

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

status GetTCPartBOMDiffForERCAPLColour(ObjectPtr TCPartObjP,string assyPartNo,string assyRev,string assySeq,string PartOrgIDDup,string configCtxt,string InpDmlNo,FILE *fp_DMLBomList,integer* mfail )
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

		ObjectPtr genContextObjAPL=NULL;
	ObjectPtr		CtxtObjTmpAPL			= NULL;
	ObjectPtr genContextObjERC=NULL;
	ObjectPtr		CtxtObjTmpERC			= NULL;


	//FILE			*matfp						= NULL;
	char decimalQty[10];
	int			qty=0;
	int			z=0;

	t5MethodInit("GetTCPartBOMDiffForERCAPLColour");

	 printf("\n Input parameter to func [%s]:[%s]:[%s]:[%s]:[%s]:\n", assyPartNo,assyRev,assySeq,configCtxt,PartOrgIDDup);fflush(stdout);
	// cntxtForERCView = setCnxtObjBOMDIff(TCPartObjP,"ERC","GlobalCtxt",mfail);

	if(nlsStrStr(InpDmlNo,"AM")!=NULL)
	{
		t5CheckDstat(SetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObjERC,mfail)) ;
		t5CheckDstat(GetCfgCtxtObjFromCtxt(DSesScc2Class,genContextObjERC,&CtxtObjTmpERC,mfail));
		t5CheckDstat(SetNavigateViewPref(CtxtObjTmpERC,TRUE,"EAS","ERC",mfail));
		t5CheckDstat(objSetAttribute(CtxtObjTmpERC,CfgItemIdAttr,"GlobalCtxt"));
		t5CheckDstat(objSetAttribute(CtxtObjTmpERC,PsmExpIncludeZeroQtyAttr,"+"));
		// t5CheckDstat(objSetAttribute(CtxtObjTmpERC,NavigateViewNameAttr,"ERC"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsAplRlzd","+"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsSTDRlzd","+"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsSTDWrkg","+"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsErcRlzd","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsReview","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsWorking","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsAPLWrkg","+"));
		//t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,NavigateViewBitPosAttr,"1"));
		if (dstat = objSetAttribute(CtxtObjTmpERC,ExpandOnRevisionAttr,"PscLastRev")) goto CLEANUP;
		t5CheckDstat(objSetObject(genContextObjERC,ConfigCtxtBlobAttr,CtxtObjTmpERC));
	}
	else if(nlsStrStr(InpDmlNo,"SM")!=NULL)
	{
		t5CheckDstat(SetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObjERC,mfail)) ;
		t5CheckDstat(GetCfgCtxtObjFromCtxt(DSesScc2Class,genContextObjERC,&CtxtObjTmpERC,mfail));
		t5CheckDstat(SetNavigateViewPref(CtxtObjTmpERC,TRUE,"EAS","APL",mfail));
		t5CheckDstat(objSetAttribute(CtxtObjTmpERC,CfgItemIdAttr,"GlobalCtxt"));
		t5CheckDstat(objSetAttribute(CtxtObjTmpERC,PsmExpIncludeZeroQtyAttr,"+"));
		// t5CheckDstat(objSetAttribute(CtxtObjTmpERC,NavigateViewNameAttr,"ERC"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsAplRlzd","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsSTDRlzd","+"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsSTDWrkg","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsErcRlzd","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsReview","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsWorking","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpERC,LCStateListAttr,"LcsAPLWrkg","+"));
		//t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,NavigateViewBitPosAttr,"1"));
		if (dstat = objSetAttribute(CtxtObjTmpERC,ExpandOnRevisionAttr,"PscLastRev")) goto CLEANUP;
		t5CheckDstat(objSetObject(genContextObjERC,ConfigCtxtBlobAttr,CtxtObjTmpERC));
	
	}


	 if (dstat = t5ShowUsesCLPartMsgFunction_TCUA(TCPartObjP,FALSE,genContextObjERC,&soPrtsForERC,mfail)) goto CLEANUP;// Added by Varun
	 //if (dstat = t5ActShowUsesCLPartMsg_Migration(TCPartObjP,cntxtForERCView,&soPrtsForERC,&soPrtsRelObjForERC,mfail)) goto CLEANUP;// Added by Varun

	 printf("\n Size of soPrtsForERC is %d \n", setSize(soPrtsForERC));fflush(stdout);

	 //cntxtForAPLView = setCnxtObjBOMDIff(TCPartObjP,"APL",configCtxt,mfail);

	 if(nlsStrStr(InpDmlNo,"AM")!=NULL)
	{
		t5CheckDstat(SetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObjAPL,mfail)) ;
		t5CheckDstat(GetCfgCtxtObjFromCtxt(DSesScc2Class,genContextObjAPL,&CtxtObjTmpAPL,mfail));
		t5CheckDstat(SetNavigateViewPref(CtxtObjTmpAPL,TRUE,"EAS","APL",mfail));
		t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,CfgItemIdAttr,"GlobalCtxt"));
		t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,PsmExpIncludeZeroQtyAttr,"+"));
		//t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,NavigateViewNameAttr,"APL"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsAplRlzd","+"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsSTDRlzd","+"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsSTDWrkg","+"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsErcRlzd","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsReview","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsWorking","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsAPLWrkg","+"));
		//t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,NavigateViewBitPosAttr,"1"));
		if (dstat = objSetAttribute(CtxtObjTmpAPL,ExpandOnRevisionAttr,"PscLastRev")) goto CLEANUP;
		t5CheckDstat(objSetObject(genContextObjAPL,ConfigCtxtBlobAttr,CtxtObjTmpAPL));
	}
	else if(nlsStrStr(InpDmlNo,"SM")!=NULL)
	{
		t5CheckDstat(SetUpContext(objClass(TCPartObjP),TCPartObjP,&genContextObjAPL,mfail)) ;
		t5CheckDstat(GetCfgCtxtObjFromCtxt(DSesScc2Class,genContextObjAPL,&CtxtObjTmpAPL,mfail));
		t5CheckDstat(SetNavigateViewPref(CtxtObjTmpAPL,TRUE,"EAS","STD",mfail));
		t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,CfgItemIdAttr,"GlobalCtxt"));
		t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,PsmExpIncludeZeroQtyAttr,"+"));
		//t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,NavigateViewNameAttr,"APL"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsAplRlzd","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsSTDRlzd","+"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsSTDWrkg","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsErcRlzd","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsReview","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsWorking","-"));
		t5CheckDstat(objNameValueSet(CtxtObjTmpAPL,LCStateListAttr,"LcsAPLWrkg","+"));
		//t5CheckDstat(objSetAttribute(CtxtObjTmpAPL,NavigateViewBitPosAttr,"1"));
		if (dstat = objSetAttribute(CtxtObjTmpAPL,ExpandOnRevisionAttr,"PscLastRev")) goto CLEANUP;
		t5CheckDstat(objSetObject(genContextObjAPL,ConfigCtxtBlobAttr,CtxtObjTmpAPL));
	
	
	}



	 if (dstat = t5ShowUsesCLPartMsgFunction_TCUA(TCPartObjP,FALSE,genContextObjAPL,&soPrtsForAPL,mfail)) goto CLEANUP;// Added by Varun	
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

				printf("%s:%s:%s:%s:",deletedPartDup,deletedPartRevDup,deletedPartSeqDup,deletedPartQtyDup);fflush(stdout);

				//if(dstat=objGetAttribute(setGet(soPrtsRelObjForERC,newcount),DisplayQuantityAttr,&deletedPartQty)) goto EXIT;
				//if (!nlsIsStrNull(deletedPartQty)) deletedPartQtyDup = nlsStrDup(deletedPartQty);
				
				//541232410101^O^5^ERC^541232600105^ADDED^20SM901026_STD^1^
				if(nlsStrStr(InpDmlNo,"AM")!=NULL)
				{
				
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
				}
				else if(nlsStrStr(InpDmlNo,"SM")!=NULL)
				{
					
					fprintf(fp_DMLBomList,"%s",assyPartNo);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,"%s",assyRev);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,"%s",assySeq);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,PartOrgIDDup);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,"%s",deletedPartDup);			//PARTNUMBER;								
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,"DELETED");						//(DELETED FROM APLC VIEW);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,InpDmlNo);		
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

				printf("\n APL BOM PARTS is %s:%s:%s:%s:",addedPartDup,addedPartRevDup,addedPartSeqDup,addedPartQtyDup);fflush(stdout);
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

				//if(dstat=objGetAttribute(setGet(soPrtsRelObjForAPL,newcount),DisplayQuantityAttr,&addedPartQty)) goto EXIT;
				//if (!nlsIsStrNull(addedPartQty)) addedPartQtyDup = nlsStrDup(addedPartQty);

				printf("%s:%s:%s:%s:",addedPartDup,addedPartRevDup,addedPartSeqDup,addedPartQtyDup);fflush(stdout);
				
				if(nlsStrStr(InpDmlNo,"AM")!=NULL)
				{
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
				}
				else if(nlsStrStr(InpDmlNo,"SM")!=NULL)
				{
					
					fprintf(fp_DMLBomList,"%s",assyPartNo);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,"%s",assyRev);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,"%s",assySeq);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,PartOrgIDDup);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,"%s",addedPartDup);			//PARTNUMBER;								
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,"ADDED");						//(DELETED FROM APLC VIEW);
					fprintf(fp_DMLBomList,"^");
					fprintf(fp_DMLBomList,InpDmlNo);		
					//fprintf(fp_DMLBomList,"%s",deletedPartQtyDup);			//PARTNUMBER QTY;
					//fprintf(fp_DMLBomList,"^");
					if(!nlsIsStrNull(addedPartQtyDup))
					{
						fprintf(fp_DMLBomList,"%s",addedPartQtyDup);
						fprintf(fp_DMLBomList,"^");
					}
					else
					{
						fprintf(fp_DMLBomList,"1^");
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
	string PartTypeVal=NULL;
	string	PartTypeValDup=NULL;
	string ColPartNumberVal1=NULL;
	string	ColPartNumberVal1Dup=NULL;
	string ColPartNumberRev1=NULL;
	string	ColPartNumberRev1Dup=NULL;
	string ColPartNumberSeq1=NULL;
	string	ColPartNumberSeq1Dup=NULL;
	string	ColPartOrgID=NULL;
	string	ColPartOrgIDDup=NULL;
						

	SetOfObjects  DMLResultSO = NULL ;
	SetOfObjects  SetOfTasksObjs = NULL ;
	SetOfObjects  setOfAssmblyObjs = NULL ;
	SetOfObjects  setOfColAssmblyObjs = NULL ;
	SetOfObjects  TCColPartObjPSet = NULL ;
	SetOfObjects  ErcDMLObjs = NULL ;
	ObjectPtr	DmlObjPtr = NULL ;
	ObjectPtr	APLDmlObjPtr = NULL ;
	ObjectPtr	TaskObjPtr = NULL ;
	ObjectPtr	TCPartObjP = NULL ;
	ObjectPtr	TCColPartObjP = NULL ;
	ObjectPtr	TCColPartObjP1 = NULL ;
	SqlPtr	InputDMLSqlPtr = NULL ;
	SqlPtr	ColPrtQrysql = NULL ;

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
		SetOfStrings		ItemMstrObjs		=	NULL;
		SetOfStrings		colourParts		=	NULL;
	SqlPtr	PrtQrysql = NULL ;

	string  InpPartNumberS=NULL;
	string  InpDmlNo=NULL;
	string  InpTaskNo=NULL;
	string  InpPartNumberRevS=NULL;
	string  InpPartNumberSeqS=NULL;
	string t5CompCode=NULL;
	string t5CompCodeDup=NULL;
	int clrPrt=0;
	int clpartCnt=0;

	int dmlCnt =0;
	int taskCnt =0;
	int partCnt =0;
	int ii =0;
	int aplDmlCnt =0;
	int dateIntVal=0;
	int monthIntVal=0;
	int yearIntVal=0;
	int foundnewmatch1=0;
	int aplRestAMDMLgrp98Found =0;
	string  InPutFileNamePART=NULL;
	FILE	*fip	= NULL;
	char inputline[3000];
	string		LineReadS=NULL;
	string ColPartNumberVal=NULL;
	string ColPartNumberValDup=NULL;
	string ColPartNumberRev=NULL;
	string ColPartNumberRevDup=NULL;
	string ColPartNumberSeq=NULL;
	string ColPartNumberSeqDup=NULL;
	SetOfStrings      dbScp	=          NULL;

	
	t5MethodInitWMD("t5_TCE_TCUA_Colour_Rest_BOM");

	printf("\n Executing... t5_TCE_TCUA_Colour_Rest_BOM.c ... \n");

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
	InPutFileNamePART=nlsStrAlloc(200);
	TCColPartObjPSet = setCreate(1);

	LoginS=argv[1];
	PasswordS=argv[2];
	ProjectName=argv[3];
	PlantName=argv[4];
	workingPath=argv[5];
	InPutFileNamePART=argv[6];
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


	if (argc < 7 || argc > 7)
	{
		printf("\nSYNTEX ERROR :: \n");
		printf("\nSYNTEX SHOULD BE :: <EXE><USER-LOGIN><PASSWD><PROJECTNAME><PLANTNAME(like PlantName1,PlantName2 ...)><WORKINGPATH><INPUTFILE>\n");
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
	if(!nlsIsStrNull(FromDateStr)) FromDateStrDup = nlsStrDup(FromDateStr);

	curr_dateDup=sysGetOffsetDate(curr_date,0,1,0);
	printf("\n curr_dateDup =%s \n",curr_dateDup);fflush(stdout);

	curr_dateDup=strssra(curr_dateDup, "-", "/");

	printf("\n 111 curr_dateDup =%s \n",curr_dateDup);fflush(stdout);
	if(!nlsIsStrNull(curr_dateDup)) curr_dateDup1 = nlsStrDup(curr_dateDup);

	DMLValSet		= setCreate(500);
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

			InpDmlNo=strtok(LineReadS,"^");
			//InpTaskNo=strtok(NULL,"^");
			InpPartNumberS=strtok(NULL,"^");
			InpPartNumberRevS=strtok(NULL,"^");
			InpPartNumberSeqS=strtok(NULL,"^");

			printf("\n Input File No %s,%s,%s,%s,%s \n",InpDmlNo,InpTaskNo,InpPartNumberS,InpPartNumberRevS,InpPartNumberSeqS); fflush(stdout);

			setOfAssmblyObjs=NULL;
			if (dstat = oiSqlCreateSelect(&PrtQrysql)) goto EXIT;
			if(dstat=(oiSqlWhereBegParen(PrtQrysql)));
			if(dstat = oiSqlWhereEQ(PrtQrysql,PartNumberAttr,InpPartNumberS)) goto EXIT;			
			if(dstat = oiSqlWhereAND(PrtQrysql)) goto EXIT;
			if(dstat = oiSqlWhereEQ(PrtQrysql,RevisionAttr,InpPartNumberRevS)) goto EXIT;			
			if(dstat = oiSqlWhereAND(PrtQrysql)) goto EXIT;
			if(dstat = oiSqlWhereEQ(PrtQrysql,SequenceAttr,InpPartNumberSeqS)) goto EXIT;			
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

						if(dstat = ExpandObject5("ItemRev",TCPartObjP,"ItemMstrForStrucBIRev",TRUE,&ItemMstrObjs,mfail))  goto CLEANUP;
						if(setSize(ItemMstrObjs)>0)
						{
							if(dstat=ExpandObject5("t5ClInfo",setGet(ItemMstrObjs,0),"t5AsmhasColPrt",SC_SCOPE_OF_SESSION,&colourParts,mfail)) goto  EXIT;
							if(setSize(colourParts)>0)
							{
								for(clrPrt=0;clrPrt<setSize(colourParts);clrPrt++)
								{
									TCColPartObjP=NULL;
									TCColPartObjP=setGet(colourParts,clrPrt);

									ColPartNumberVal=NULL;
									ColPartNumberValDup=NULL;
									foundnewmatch1=0;

									
									if(dstat=ulyFindObjectInSet(TCColPartObjP, TCColPartObjPSet , PartNumberAttr , 0 , &foundnewmatch1)) goto EXIT;
									printf("\n foundnewmatch1:%d \n",foundnewmatch1); fflush(stdout);
									if (foundnewmatch1 < 0)
									{
										t5CheckDstat(ulyAddObjectToSet(TCColPartObjP,&TCColPartObjPSet));
										printf("\n TCColPartObjPSet:%d \n",setSize(TCColPartObjPSet)); fflush(stdout);
										
										if(dstat = objGetAttribute(TCColPartObjP,PartNumberAttr,&ColPartNumberVal))goto EXIT;
										if(!nlsIsStrNull(ColPartNumberVal)) ColPartNumberValDup=nlsStrDup(ColPartNumberVal);
										printf("\n  Col TC Part Number:%s \n",ColPartNumberValDup); fflush(stdout);							

										ColPartNumberRev=NULL;
										ColPartNumberRevDup=NULL;

										if(dstat = objGetAttribute(TCColPartObjP,RevisionAttr,&ColPartNumberRev))goto EXIT;
										if(!nlsIsStrNull(ColPartNumberRev))ColPartNumberRevDup=nlsStrDup(ColPartNumberRev);
										printf("\n  Col TC Part Revision:%s \n",ColPartNumberRevDup); fflush(stdout);

										ColPartNumberSeq=NULL;
										ColPartNumberSeqDup=NULL;

										if(dstat = objGetAttribute(TCColPartObjP,SequenceAttr,&ColPartNumberSeq))goto EXIT;
										if(!nlsIsStrNull(ColPartNumberSeq))ColPartNumberSeqDup=nlsStrDup(ColPartNumberSeq);
										printf("\n Col TC Part Sequence:%s \n",ColPartNumberSeqDup); fflush(stdout);

										setOfColAssmblyObjs=NULL;
										if (dstat = oiSqlCreateSelect(&ColPrtQrysql)) goto EXIT;
										if(dstat=(oiSqlWhereBegParen(ColPrtQrysql)));
										if(dstat = oiSqlWhereEQ(ColPrtQrysql,PartNumberAttr,ColPartNumberValDup)) goto EXIT;			
										if(dstat = oiSqlWhereAND(ColPrtQrysql)) goto EXIT;
										if(dstat = oiSqlWhereEQ(ColPrtQrysql,OwnerNameAttr,"Release Vault")) goto EXIT;
										if(dstat = oiSqlAscOrder(ColPrtQrysql,CreationDateAttr)) goto EXIT;
										if(dstat=(oiSqlWhereEndParen(ColPrtQrysql))) goto EXIT;
										if(dstat = QueryDbObject(PartClass,ColPrtQrysql,0,TRUE,SC_SCOPE_OF_SESSION,&setOfColAssmblyObjs,mfail)) goto EXIT;
										if(dstat=oiSqlPrint(ColPrtQrysql))goto EXIT;
										if(ColPrtQrysql) oiSqlDispose(ColPrtQrysql); ColPrtQrysql = NULL;
										printf("\n  setSize(setOfColAssmblyObjs):%d \n",setSize(setOfColAssmblyObjs)); fflush(stdout);
										if(setSize(setOfColAssmblyObjs)>0)
										{
											clpartCnt=0;
											for(clpartCnt=0;clpartCnt<setSize(setOfColAssmblyObjs);clpartCnt++)
											{
													
													TCColPartObjP1=NULL;
													TCColPartObjP1=setGet(setOfColAssmblyObjs,clpartCnt);

													ColPartNumberVal1=NULL;
													ColPartNumberVal1Dup=NULL;

													if(dstat = objGetAttribute(TCColPartObjP1,PartNumberAttr,&ColPartNumberVal1))goto EXIT;
													if(!nlsIsStrNull(ColPartNumberVal1)) ColPartNumberVal1Dup=nlsStrDup(ColPartNumberVal1);
													printf("\n  111 Col TC Part Number:%s \n",ColPartNumberVal1Dup); fflush(stdout);							

													ColPartNumberRev1=NULL;
													ColPartNumberRev1Dup=NULL;

													if(dstat = objGetAttribute(TCColPartObjP1,RevisionAttr,&ColPartNumberRev1))goto EXIT;
													if(!nlsIsStrNull(ColPartNumberRev1))ColPartNumberRev1Dup=nlsStrDup(ColPartNumberRev1);
													printf("\n 111 Col TC Part Revision:%s \n",ColPartNumberRev1Dup); fflush(stdout);

													ColPartNumberSeq1=NULL;
													ColPartNumberSeq1Dup=NULL;

													if(dstat = objGetAttribute(TCColPartObjP1,SequenceAttr,&ColPartNumberSeq1))goto EXIT;
													if(!nlsIsStrNull(ColPartNumberSeq1))ColPartNumberSeq1Dup=nlsStrDup(ColPartNumberSeq1);
													printf("\n Col TC Part Sequence1:%s \n",ColPartNumberSeq1Dup); fflush(stdout);

													ColPartOrgID=NULL;
													ColPartOrgIDDup=NULL;

													if(dstat = objGetAttribute(TCColPartObjP1,OrganizationIDAttr,&ColPartOrgID))goto EXIT;
													if(!nlsIsStrNull(ColPartOrgID))ColPartOrgIDDup=nlsStrDup(ColPartOrgID);
													printf("\n TC Part OrgID:%s \n",ColPartOrgIDDup); fflush(stdout);

													printf("\n Calling GetTCPartBOMDiffForERCAPLColour for APL colour.."); fflush(stdout);
													t5CheckMfail(GetTCPartBOMDiffForERCAPLColour(TCColPartObjP1,ColPartNumberVal1Dup,ColPartNumberRev1Dup,ColPartNumberSeq1Dup,ColPartOrgIDDup,configCtxt,InpDmlNo,fp_DMLBomList,mfail));
		
													/*printf("\n Calling GetTCPartOptionalCS for ERC.."); fflush(stdout);
													t5CheckMfail(GetTCPartOptionalCS(TCPartObjP,GenOptionalCS,fp_DMLOPCSInfo,fp_DMLErrorLog,mfail));
													printf("\n Done GetTCPartOptionalCS for ERC.."); fflush(stdout);*/

													/*if((setSize(setOfColAssmblyObjs)-1)==clpartCnt)
													{
														
														printf("\nInside DMl colour part printing info..."); fflush(stdout);
													
														if(!nlsIsStrNull(integerStr))
														{
															fprintf(fp_PlantDmlList,integerStr);						
															fprintf(fp_PlantDmlList,"^");
														}
														else
														{
															fprintf(fp_PlantDmlList," ^");
														}

														if(!nlsIsStrNull(InpDmlNo))
														{
															fprintf(fp_PlantDmlList,InpDmlNo);						
															fprintf(fp_PlantDmlList,"^");
														}
														else
														{
															fprintf(fp_PlantDmlList," ^");
														}
														if(!nlsIsStrNull(InpTaskNo))
														{
															fprintf(fp_PlantDmlList,InpTaskNo);						
															fprintf(fp_PlantDmlList,"^");
														}
														else
														{
															fprintf(fp_PlantDmlList," ^");
														}

														if(!nlsIsStrNull(ColPartNumberVal1Dup))
														{
															fprintf(fp_PlantDmlList,ColPartNumberVal1Dup);						
															fprintf(fp_PlantDmlList,"^");
														}
														else
														{
															fprintf(fp_PlantDmlList," ^");
														}

														if(!nlsIsStrNull(ColPartNumberRev1Dup))
														{
															fprintf(fp_PlantDmlList,ColPartNumberRev1Dup);						
															fprintf(fp_PlantDmlList,"^");
														}
														else
														{
															fprintf(fp_PlantDmlList," ^");
														}

														if(!nlsIsStrNull(ColPartNumberSeq1Dup))
														{
															fprintf(fp_PlantDmlList,ColPartNumberSeq1Dup);						
															fprintf(fp_PlantDmlList,"^");
														}
														else
														{
															fprintf(fp_PlantDmlList," ^");
														}
														fprintf(fp_PlantDmlList,"\n");
														
													}*/
											
											}
										}

									}
								}
							}
						}
						break;

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

