/**********************************************************************************************************************************************************
**
** SOURCE FILE NAME: APLGMLDML.c
**
** Functions:- This utility takes item_id and attribute_value as input from file and updates the value of specific attribute of latest revision of perticular
			   item found from input item_id and writes appropriate information into output file.
**
**
**	Date							Author								Modification
**	03/06/2020						Hemal Sukhanandi					Creation
**
** command to run:
** GateMaturation -u=<username> -p=<password> -g=<group> -taskno=<taskno>
***********************************************************************************************************************************************************/

#include <time.h>
#include <stdio.h>
#include <tc/tc.h>
#include <tc/emh.h>
#include <bom/bom.h>
#include <tc/folder.h>
#include <tccore/aom.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <tc/tc_macros.h>
#include <bom/bom_attr.h>
#include <tc/tc_startup.h>
//#include <vector>
#include <tccore/project.h>
#include <tcinit/tcinit.h>
#include <cfm/cfm.h>
#include <qry/qry.h>
#include <tc/preferences.h>
#include <tccore/aom_prop.h>
#include <fclasses/tc_string.h>
#include <tccore/workspaceobject.h>
#include <ps/ps.h>
#include <pie/pie.h>
#include <ps/ps_errors.h>
#include <stdlib.h>
#define PROP_UIF_set_value_msg		"PROP_UIF_set_value"
#define PROP_set_value_tag_msg		"PROP_set_value_tag"
#define PROP_set_value_string_msg   "PROP_set_value_string"
#define ITK_CALL(X) (report_error( __FILE__, __LINE__, #X, (X)))


//using namespace std;
int error_code	= ITK_ok;
FILE 		*output 			= NULL;

static int PrintErrorStack( void )
/*
*
* PURPOSE : Function to dump the ITK error stack
*
* RETURN : causes program termination. If you made it here
*          you're not coming back modified for cust.c to not call exit()
*          but to just print the error stack
*
* NOTES : This version will always return ITK_ok, which is quite strange
*           actually. But if the error reporting was "OK" then that makes
*           sense
*
*/
{
    int iNumErrs = 0;
    const int *pSevLst;
    const int *pErrCdeLst;
    const char **pMsgLst;
    register int i = 0;

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst );
	//fprintf( stderr, "in PrintErrorStack iNumErrs :%d \n",iNumErrs);
    for ( i = 0; i < iNumErrs; i++ )
    {
		fprintf( stderr, "Error(PrintErrorStack): \n");
        fprintf( stderr, "\t%6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
    }
    return ITK_ok;
}
/********************************************************************************/

struct BomChldStrut
{
	tag_t child_objs;//CHild
	tag_t child_objs_bvr;//BVR
	int child_objs_lvl;//LVL
}*get_BomChldStrut;

char* subString (char* mainStringf ,int fromCharf,int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(200);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}

void reverse(char *s)
{
   int length, c;
   char *begin, *end, temp;
 
   length = tc_strlen(s);
   begin  = s;
   end    = s;
 
   for (c = 0; c < length - 1; c++)
      end++;
 
   for (c = 0; c < length/2; c++)
   {        
      temp   = *end;
      *end   = *begin;
      *begin = temp;
 
      begin++;
      end--;
   }
}


void Multi_Get_Part_BOM_Lvl(tag_t inputPart,int reqLevel,int level,tag_t closure_tag,tag_t revRule,char ViewBVR[100],struct BomChldStrut BomChldStrut[],int* StructChldCnt)
{
	int ifail;
    int iChildItemTag=0;
	char * ItemName ;
	int k=0;
    int n=0;
    int assbvr=0;
    int flagBVR=0;
	int 	n_values_bvr=0;
	tag_t			*bvr_assy_part= NULLTAG;
	tag_t			bvr_tag= NULLTAG;
	tag_t   t_ChildItemRev;
	tag_t*	childrenTag	= NULLTAG;
	char *view_name=NULL;
	tag_t	window 				= NULLTAG;
	char*			partTok					=NULL;
	char*			viewTok					=NULL;
	tag_t	top_line			= NULLTAG;
	int bvrfound=0;
	tag_t  *children=NULLTAG;
	char* partNumber 	= NULL;

	printf("\n Inside Multi_Get_Part_BOM_Lvl[%d] ...\n",*StructChldCnt);

	ITKCALL(AOM_ask_value_string(inputPart,"item_id",&partNumber));
	printf("\n Part number===>%s\n",partNumber);fflush(stdout);

	printf("\n ViewBVR=>[%s] ...\n",ViewBVR);

	if( level >= reqLevel )
	{
		goto CLEANUP;
	}

	ITKCALL(ITEM_rev_list_bom_view_revs(inputPart, &n_values_bvr, &bvr_assy_part));
	printf("\n n_values_bvr=>[%d] ",n_values_bvr);fflush(stdout);

	if(n_values_bvr==0)
	{
		flagBVR=0;
	}
	else if(n_values_bvr>0)
	{
		printf("\n BVR found");fflush(stdout);
		for(assbvr=0;assbvr<n_values_bvr;assbvr++)
		{
			ITKCALL(AOM_ask_value_string(bvr_assy_part[assbvr],"object_name",&view_name));
			printf("\n view_name %s",view_name);fflush(stdout);
			partTok = strtok (view_name,"-");
			viewTok = strtok (NULL,"-");
			printf("\n viewTok %s",viewTok);fflush(stdout);
			if(strlen(viewTok)>0)
			{
				//if(tc_strcmp(viewTok,"View")==0)
				if(tc_strcmp(viewTok,ViewBVR)==0)
				{
					flagBVR=1;
					bvr_tag=bvr_assy_part[assbvr];
					break;
				}
			}
		}
		if(flagBVR!=1)
		{
			for(assbvr=0;assbvr<n_values_bvr;assbvr++)
			{
				ITKCALL(AOM_ask_value_string(bvr_assy_part[assbvr],"object_name",&view_name));
				printf("\n view_name %s",view_name);fflush(stdout);
				partTok = strtok (view_name,"-");
				viewTok = strtok (NULL,"-");
				printf("\n viewTok %s",viewTok);fflush(stdout);
				if(strlen(viewTok)>0)
				{
					//if(tc_strcmp(viewTok,ViewBVR)==0)
					if(tc_strcmp(viewTok,"View")==0)
					{
						flagBVR=1;
						bvr_tag=bvr_assy_part[assbvr];
						break;
					}
				}
			}
		}

	}
	printf("\n flagBVR=>[%d] ",flagBVR);fflush(stdout);

	ITKCALL(BOM_create_window (&window));
	ITKCALL(BOM_set_window_config_rule(window,revRule));
	ITKCALL(BOM_window_set_closure_rule(window,closure_tag,0,NULL,NULL));
	//ITKCALL(BOM_set_window_pack_all (window, true));
	if(flagBVR==1)
	{
		BOM_set_window_top_line(window, null_tag,inputPart ,bvr_tag, &top_line);
		ITKCALL(BOM_line_ask_child_lines (top_line, &n, &children));
		printf("\n\n\t\t No of child objects are n : %d\n",n);fflush(stdout);

		level = level + 1;
		for (k = 0; k < n; k++)
		{
			BOM_line_unpack (children[k]);
			BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
			BOM_line_ask_attribute_tag(children[k], iChildItemTag, &t_ChildItemRev);
			if(t_ChildItemRev!=NULLTAG)
			{
				AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemName);
				*StructChldCnt	=	*StructChldCnt+1;
				//get_BomChldStrut[*StructChldCnt].child_objs = childrenTag[k]; Hemal
				BomChldStrut[*StructChldCnt].child_objs = t_ChildItemRev;
				BomChldStrut[*StructChldCnt].child_objs_bvr=children[k];
				BomChldStrut[*StructChldCnt].child_objs_lvl=level;

				Multi_Get_Part_BOM_Lvl(t_ChildItemRev,reqLevel,level,closure_tag,revRule,ViewBVR,BomChldStrut,StructChldCnt);
			}
		}
		level = level - 1;
	}

	MEM_free (childrenTag);

	CLEANUP:
		 printf("\n Inside Multi_Get_Part_BOM_Lvl CLEANUP");fflush(stdout);
}
int Get_Part_BOM_Lvl(tag_t VehicleObj,int reqLevel,char*  closureRuleName,char*  revRuleName,char* ViewBVR,struct BomChldStrut BomChldStrut[] ,int* StructChldCnt)
{
	int   ifail				= 0;

	tag_t revRule 			= NULLTAG;
	char* vehicleNumber 	= NULL;
	int j					= 0;
	PIE_scope_t scope;
	int 	n_closure_tags;
	tag_t * 	closure_tags;
	tag_t  	closure_tag;
	int k1=0;
	int n=0;
	int 	n_values_bvr=0;
	tag_t			*bvr_assy_part= NULLTAG;
	tag_t			bvr_tag= NULLTAG;
	int level 				= 0;
	tag_t	window 				= NULLTAG;
	tag_t	top_line			= NULLTAG;
	tag_t	objChild			= NULLTAG;
	tag_t	objChild_bvr			= NULLTAG;
	int child_lvl=0;
	char	*c_Qty						=	NULL;
	tag_t  *children1=NULLTAG;
	int iChildItemTag=0;
	int assbvr=0;
	int bvrfound=0;
	int flagRevRule=0;
	int flagClosureRule=0;
	char *view_name=NULL;
	char*			partTok					=NULL;
	char*			viewTok					=NULL;
	tag_t   t_ChildItemRev;
	char * ItemName ;

	printf("\nInside Get_Part_BOM_Lvl ....\n");

	if(VehicleObj == NULLTAG)
	{
		printf("\n VehicleObj is NULLTAG\n");fflush(stdout);
		return ifail;
	}
	ITKCALL(AOM_ask_value_string(VehicleObj,"item_id",&vehicleNumber));
	printf("\n Part number===>%s\n",vehicleNumber);fflush(stdout);

	printf("\nBefore Size of StructChldCnt==>%d\n",*StructChldCnt);fflush(stdout);
	printf("\nBefore reqLevel==>%d\n",reqLevel);fflush(stdout);
	printf("\nBefore level==>%d\n",level);fflush(stdout);
	printf("\nclosureRuleName==>%s\n",closureRuleName);fflush(stdout);
	printf("\nrevRuleName==>%s\n",revRuleName);fflush(stdout);
	printf("\nViewBVR==>%s\n",ViewBVR);fflush(stdout);

	ITKCALL(ITEM_rev_list_bom_view_revs(VehicleObj, &n_values_bvr, &bvr_assy_part));
	printf("\n n_values_bvr=>[%d] ",n_values_bvr);fflush(stdout);
	if(n_values_bvr>0)
	{
		printf("\n BVR found");fflush(stdout);
		for(assbvr=0;assbvr<n_values_bvr;assbvr++)
		{
			ITKCALL(AOM_ask_value_string(bvr_assy_part[assbvr],"object_name",&view_name));
			printf("\n view_name %s",view_name);fflush(stdout);
			partTok = strtok (view_name,"-");
			viewTok = strtok (NULL,"-");
			printf("\n viewTok %s",viewTok);fflush(stdout);
			if(strlen(viewTok)>0)
			{
				if(tc_strcmp(viewTok,ViewBVR)==0)
				{
					bvrfound++;
					bvr_tag=bvr_assy_part[assbvr];
					break;
				}
			}
		}

	}

	printf("\n bvrfound=>[%d] ",bvrfound);fflush(stdout);
	if(bvrfound==0)
	{
		printf("\n**********BVR NOT Exist*********\n");fflush(stdout);
	}
	else
	{
		ITKCALL(CFM_find(revRuleName, &revRule));
		if (revRule != NULLTAG)
		{
			printf("\nFind revRule\n");fflush(stdout);
			flagRevRule=1;
		}

		scope=PIE_TEAMCENTER;
		ITKCALL(PIE_find_closure_rules2(closureRuleName,scope,&n_closure_tags,&closure_tags));
		printf("\n n_closure_tags:%d ..............",n_closure_tags);fflush(stdout);
		if(n_closure_tags==1)
		{
			closure_tag=closure_tags[0];
			flagClosureRule=1;
		}
		printf("\n flagClosureRule=>[%d] ",flagClosureRule);fflush(stdout);
		printf("\n flagRevRule=>[%d] ",flagRevRule);fflush(stdout);
		if((flagClosureRule==1) && (flagRevRule==1))
		{

			Multi_Get_Part_BOM_Lvl(VehicleObj,reqLevel,level,closure_tag,revRule,ViewBVR,BomChldStrut,StructChldCnt);
		}
		else
		{
			printf("\n******Revision or Closure Rule Not Found******\n");fflush(stdout);
		}

	}
	printf("\nAfter Size of StructChldCnt==>%d\n",*StructChldCnt);fflush(stdout);

	for (j=1;j<= *StructChldCnt;j++ )
	{
		objChild		= NULLTAG;
		objChild_bvr	= NULLTAG;
		c_Qty			= NULL;
		child_lvl       =0;

		printf("\nPrint Item ID==>%d\n",j);fflush(stdout);

		objChild=BomChldStrut[j].child_objs;
		objChild_bvr=BomChldStrut[j].child_objs_bvr;
		child_lvl=BomChldStrut[j].child_objs_lvl;

		AOM_ask_value_string(objChild,"item_id",&ItemName);
		printf("\nItemName==>%s\n",ItemName);fflush(stdout);

		printf("\nchild_lvl==>%d\n",child_lvl);fflush(stdout);

		AOM_ask_value_string(objChild_bvr,"bl_quantity",&c_Qty);
		printf("\nQty==>%s\n",c_Qty);fflush(stdout);

	}
	return ifail;
}
void  GetPlantForDMLORTask( char * item_id ,char  **getPlant)
{
    printf( "item_id:%s\n", item_id);

	char *item_idCpy =NULL;
	char *revStr =NULL;
	char *tmpStr =NULL;
	char *revStr1 =NULL;
	item_idCpy	=	(char *)MEM_alloc(50);

	tc_strcpy(item_idCpy,item_id);
	reverse(item_idCpy);
	revStr	=	(char *)MEM_alloc(50);
	tc_strcpy(revStr,item_idCpy);
	printf("\n revStr Find %s.......",revStr);fflush(stdout);
	tmpStr	=	strtok(revStr,"_");
	printf("\n tmpStr Find %s.......",tmpStr);fflush(stdout);
	revStr1	=	(char *)MEM_alloc(50);
	reverse(tmpStr);
	tc_strcpy(revStr1,tmpStr);
	printf("\n revStr1 Find %s.......",revStr1);fflush(stdout);
	int ilength	=	0;
	ilength	=	tc_strlen(revStr1+5);
	*getPlant=	(char *)MEM_alloc(50);
	tc_strcpy(*getPlant,revStr1);

	printf( "getPlant:%s\n", *getPlant);
}

void getDMLUserGroup(char	*sPlantName, char	*sUserGroup)
{

	printf("\nInside getDMLUserGroup");fflush(stdout);
	printf("\nDML Plant name : %s",sPlantName);fflush(stdout);
	sUserGroup	=	(char	*)MEM_alloc(tc_strlen(sPlantName)+10);
	if (tc_strcmp(sPlantName,"APLC")==0)
	{
		tc_strcpy(sUserGroup,"APLCAR");
	}
	else if(tc_strcmp(sPlantName,"APLP")==0)
	{
		tc_strcpy(sUserGroup,"APLPUNE");
	}
	else if (tc_strcmp(sPlantName,"APLJ")==0)
	{
		tc_strcpy(sUserGroup,"APLJSR");
	}
	else if (tc_strcmp(sPlantName,"APLL")==0)
	{
		tc_strcpy(sUserGroup,"APLLKO");
	}
	else if (tc_strcmp(sPlantName,"APLV")==0)
	{
		tc_strcpy(sUserGroup,"APLUV");
	}
	else if (tc_strcmp(sPlantName,"APLA")==0)
	{
		tc_strcpy(sUserGroup,"APLAHD");
	}
	else if (tc_strcmp(sPlantName,"APLD")==0)
	{
		tc_strcpy(sUserGroup,"APLDWD");
	}
	else if (tc_strcmp(sPlantName,"APLU")==0)
	{
		tc_strcpy(sUserGroup,"APLUTK");
	}
	else
	{
		tc_strcpy(sUserGroup,"APLCAR");
	}

	printf("\nUser Group : %s",sUserGroup);fflush(stdout);
}

void  get_CtrlVal_APLStateInf( char * Plt_Code ,char  APlRevwLC[40],char  AplWrkngLC[40],char  AplRlzdLC[40],char  StdWrkngLC[40],char  StdRlzdLC[40], char PlantLCFlag[40], char StdPlant[40], char StdRevRule[40], char StdCLosRule[40],char StdView[40])
{
	 char    *qry_entryCntrl1[3]  = {"SYSCD","SUBSYSCD","Delete Indicator"};	
	 char	**qry_valuesCntrl1= (char **) MEM_alloc(5 * sizeof(char *));

	 int cntrlcnt=0;
	 int  control_number_found1=0;

	 tag_t *list_of_WSO_cntrl_tags1=NULLTAG;

	 tag_t   qryTagCntrl1     = NULLTAG;
	 int     n_entryCntrl1    = 3;
	 char *APlRevw 			= NULL;
	 char *AplWrkng			= NULL;
	 char *AplRlzd			= NULL;
	 char *StdWrkng			= NULL;
	 char *StdRlzd			= NULL;
	 char *Plant			= NULL;
	 char *stdCode			= NULL;
	 char *RevRule		= NULL;
	 char *stdClsRule		= NULL;
	 char *stdBvr			= NULL;
	 char *PlantOneChar		= NULL;

	 char* viewtocpy		= NULL;
	 int max_char_size = 80;
	 viewtocpy = (char *)MEM_alloc(max_char_size * sizeof(char));

	printf("\n Plt_Code:%s",Plt_Code);fflush(stdout);
	PlantOneChar=subString(Plt_Code,3,1);
	printf("\n PlantOneChar:%s",PlantOneChar);fflush(stdout);
	tc_strcpy(viewtocpy,"");
	tc_strcat(viewtocpy,"APL");
	tc_strcat(viewtocpy,PlantOneChar);
	tc_strcat(viewtocpy," View");
	printf("\n View to copy:%s",viewtocpy);fflush(stdout);
	 

	 if(QRY_find("Control Objects...", &qryTagCntrl1));
			if (qryTagCntrl1)
			{
				printf("\n Control Object Query Found \n");fflush(stdout);
				qry_valuesCntrl1[0]="APLStateInf";
				qry_valuesCntrl1[1]=Plt_Code;
				qry_valuesCntrl1[2]="0";
				
				if(QRY_execute(qryTagCntrl1, n_entryCntrl1, qry_entryCntrl1, qry_valuesCntrl1, &control_number_found1, &list_of_WSO_cntrl_tags1));
			}
			else
			{
				printf("\n Control Object Query not Found \n");fflush(stdout);
			}	
			
			printf("\n control_number_found1 is : %d\n",control_number_found1);fflush(stdout);

			if(control_number_found1>0)
			{
				
				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo1", &APlRevw);
				printf("\n APlRevw: %s\n",APlRevw);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo2", &AplWrkng);
				printf("\n AplWrkng: %s\n",AplWrkng);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo3", &AplRlzd);
				printf("\n AplRlzd: %s\n",AplRlzd);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo4", &StdWrkng);
				printf("\n StdWrkng: %s\n",StdWrkng);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo5", &StdRlzd);
				printf("\n StdRlzd: %s\n",StdRlzd);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo6", &Plant);
				printf("\n Plant: %s\n",Plant);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo7", &stdCode);
				printf("\n stdCode: %s\n",stdCode);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo8", &RevRule);
				printf("\n RevRule: %s\n",RevRule);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo9", &stdClsRule);
				printf("\n stdClsRule: %s\n",stdClsRule);fflush(stdout);

				//AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo10", &stdBvr);
				//printf("\n stdBvr: %s\n",stdBvr);fflush(stdout);

							
				if(tc_strlen(APlRevw)>0) 
				{ 
					tc_strcpy(APlRevwLC,APlRevw);
				}
				if(tc_strlen(AplWrkng)>0) 
				{ 
					tc_strcpy(AplWrkngLC,AplWrkng);
				}
				if(tc_strlen(AplRlzd)>0) 				
				{
					tc_strcpy(AplRlzdLC,AplRlzd);
				}
				if(tc_strlen(StdWrkng)>0) 
				{
					tc_strcpy(StdWrkngLC,StdWrkng);
				}
				if(tc_strlen(StdRlzd)>0) 
				{
					tc_strcpy(StdRlzdLC,StdRlzd);
				}
				if(tc_strlen(Plant)>0) 
				{
					tc_strcpy(PlantLCFlag,Plant);
				}
				if(tc_strlen(stdCode)>0) 
				{
					tc_strcpy(StdPlant,stdCode);
				}
				if(tc_strlen(RevRule)>0) 
				{
					tc_strcpy(StdRevRule,RevRule);
				}
				if(tc_strlen(stdClsRule)>0) 
				{
					tc_strcpy(StdCLosRule,stdClsRule);
				}
				if(tc_strlen(viewtocpy)>0) 
				{
					tc_strcpy(StdView,viewtocpy);
				}
			}
			else
			{
				tc_strcpy(APlRevwLC,"T5_LcsAplReview");
				tc_strcpy(AplWrkngLC,"T5_LcsAPLWrkg");
				tc_strcpy(AplRlzdLC,"T5_LcsAplRlzd");
				tc_strcpy(StdWrkngLC,"T5_LcsSTDWrkg");
				tc_strcpy(StdRlzdLC,"T5_LcsStdRlzd");		
				tc_strcpy(PlantLCFlag,"CAR");
				tc_strcpy(StdPlant,"STDC");
				tc_strcpy(StdRevRule,"APLC Release and above");
				tc_strcpy(StdCLosRule,"BOMViewClosureRuleSTDC");
				tc_strcpy(StdView,viewtocpy);
			}
}

int CreateRelFun(tag_t objTag, char* Toplnt, char*  PrtDRst,tag_t rev_tag, char * ProjInfo)
{
	int		ifail 				=	ITK_ok;
	int		CreatAllow			=	0;
	int		number_found		=	0;
	int		i					=	0;
	int		count				=	0;

	char	*revDsgGrp			=	NULL;
	char	*revprojcode		=	NULL;
	char	*revprtnum			=	NULL;
	char	*dmlNo				=	NULL;

	tag_t	tsk_CMRef_type		=	NULLTAG;
	tag_t	*list_of_WSO_tags	=	NULLTAG;
	tag_t	pfolder_tag			=	NULLTAG;
	tag_t	Ref_Rel				=	NULLTAG;
	tag_t	relation1			=	NULLTAG;
	tag_t	tsk_dml_rel_type	=	NULLTAG;
	tag_t	*DMLRevision		=	NULLTAG;

	WSO_search_criteria_t  	criteria;

	printf("\nInside CreateRelFun");fflush(stdout);
	printf("\nToplnt : %s",Toplnt);fflush(stdout);
	printf("\nPrtDRst : %s",PrtDRst);fflush(stdout);
	printf("\nProjInfo : %s",ProjInfo);fflush(stdout);

	if(AOM_UIF_ask_value(rev_tag,"item_id",&revprtnum)!=ITK_ok)PrintErrorStack();
	if(AOM_UIF_ask_value(rev_tag,"t5_DesignGrp",&revDsgGrp)!=ITK_ok)PrintErrorStack();
	if(AOM_UIF_ask_value(rev_tag,"t5_ProjectCode",&revprojcode)!=ITK_ok)PrintErrorStack();

	printf("\nCreate Part Number : %s, Design Group : %s, Project code : %s",revprtnum,revDsgGrp,revprojcode);fflush(stdout);

	printf("\n");fflush(stdout);

	if((tc_strcmp(PrtDRst,"")==0) || (tc_strcmp(PrtDRst,"NA")==0))
	{
		CreatAllow=1;
		printf("NAcase:Flag is [%d]\n",CreatAllow);fflush(stdout);
	}
	else if(((tc_strcmp(Toplnt,"DR5")==0)|| (tc_strcmp(Toplnt,"AR5")==0)) && ((tc_strcmp(PrtDRst,"DR4")==0)|| (tc_strcmp(PrtDRst,"AR4")==0)))
	{
		CreatAllow=1;
		printf("R5:Flag is [%d]\n",CreatAllow);fflush(stdout);
	}
	else if(((tc_strcmp(Toplnt,"DR4")==0)|| (tc_strcmp(Toplnt,"AR4")==0)) && ((tc_strcmp(PrtDRst,"DR3")==0)|| (tc_strcmp(PrtDRst,"AR3")==0)))
	{
		CreatAllow=1;
		printf("R4:Flag is [%d]\n",CreatAllow);fflush(stdout);
	}
	else if(((tc_strcmp(Toplnt,"DR4")==0)|| (tc_strcmp(Toplnt,"AR4")==0)) && ((tc_strcmp(PrtDRst,"DR3P")==0)|| (tc_strcmp(PrtDRst,"AR3P")==0)))
	{
		CreatAllow=1;
		printf("R4P:Flag is [%d]\n",CreatAllow);fflush(stdout);
	}
	else if(((tc_strcmp(Toplnt,"DR3")==0)|| (tc_strcmp(Toplnt,"AR3")==0)) && ((tc_strcmp(PrtDRst,"DR2")==0)|| (tc_strcmp(PrtDRst,"AR2")==0)))
	{
		CreatAllow=1;
		printf("R3:Flag is [%d]\n",CreatAllow);fflush(stdout);
	}
	else if(((tc_strcmp(Toplnt,"DR3P")==0)|| (tc_strcmp(Toplnt,"AR3P")==0)) && ((tc_strcmp(PrtDRst,"DR2")==0)|| (tc_strcmp(PrtDRst,"AR2")==0)))
	{
		CreatAllow=1;
		printf("R3P:Flag is [%d]\n",CreatAllow);fflush(stdout);
	}
	else if(((tc_strcmp(Toplnt,"DR2")==0)|| (tc_strcmp(Toplnt,"AR2")==0)) && ((tc_strcmp(PrtDRst,"DR1")==0)|| (tc_strcmp(PrtDRst,"AR1")==0)))
	{
		CreatAllow=1;
		printf("R2:Flag is [%d]\n",CreatAllow);fflush(stdout);
	}
	else if(((tc_strcmp(Toplnt,"DR1")==0)|| (tc_strcmp(Toplnt,"AR1")==0)) && ((tc_strcmp(PrtDRst,"DR0")==0)|| (tc_strcmp(PrtDRst,"AR0")==0)))
	{
		CreatAllow=1;
		printf("R1:Flag is [%d]\n",CreatAllow);fflush(stdout);
	}
	printf("create allow : %d",CreatAllow);fflush(stdout);
	if (CreatAllow >0)
	{
		printf("Inside create allow : %d",CreatAllow);fflush(stdout);
		if(GRM_find_relation_type("CMReferences", &tsk_CMRef_type)!=ITK_ok)PrintErrorStack();
		if (tsk_CMRef_type!=NULLTAG)
		{
			printf("\n relation lock");
			if ((rev_tag!=NULLTAG) || (objTag!=NULLTAG))
			{
				ifail =GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type);		
				if (tsk_dml_rel_type!=NULLTAG)
				{
					if(GRM_list_primary_objects_only(objTag,tsk_dml_rel_type,&count,&DMLRevision))PrintErrorStack();
					ifail = AOM_UIF_ask_value(DMLRevision[0],"item_id",&dmlNo);fflush(stdout);
					printf("\n DML number %s",dmlNo);
				}
				else
				{
					printf("relation is not found for task and dml");
				}
				char *Fld_Des=NULL;
				Fld_Des=(char *) MEM_alloc(50);

				tc_strcpy(Fld_Des,"");
				tc_strcat(Fld_Des,"DML-");
				tc_strcat(Fld_Des,dmlNo);

				printf("\n folder description %s",Fld_Des);
				if((tc_strstr(ProjInfo,revprojcode) != NULL))
				{
					printf("%s Project is live for APL GM DML",revprojcode);fflush(stdout);
					WSOM_clear_search_criteria(&criteria);
					strcpy(criteria.name,"Parts For Gate Maturation");
					strcpy(criteria.desc,Fld_Des);
					ifail = WSOM_search(criteria, &number_found, &list_of_WSO_tags);
					printf("\nNumber of folders found are %d\n",number_found);

					if (number_found>0)
					{
						i=i++;
						printf("\n folder insert");fflush(stdout);
						ifail = AOM_lock(list_of_WSO_tags[0]);
						ifail = FL_insert(list_of_WSO_tags[0],rev_tag,i);
						ifail = AOM_save(list_of_WSO_tags[0]);
						ifail = AOM_unlock(list_of_WSO_tags[0]);
						ifail =AOM_refresh(list_of_WSO_tags[0],TRUE);
					}
					else
					{
						printf("\n folder description111111 %s",Fld_Des);
						ifail = FL_create("Parts For Gate Maturation",Fld_Des,&pfolder_tag);
						if (pfolder_tag!=NULLTAG)
						{
							ifail = GRM_find_relation_type("CMReferences", &Ref_Rel);
							printf("\n 22 save");
							printf("\n 33 save");
							ifail =AOM_refresh(pfolder_tag,TRUE);
							//ifail = AOM_lock(DMLRevision[0]);
							ifail = AOM_lock(objTag);
							printf("\n 44 save");
							if (Ref_Rel!=NULLTAG)
							{
								printf("Ref_Rel is not NULLTAG");fflush(stdout);
								ifail = GRM_create_relation(objTag,pfolder_tag,Ref_Rel,NULLTAG,&relation1);
								printf("\n folder save");
								if (relation1!=NULLTAG)
								{
									printf("relation1 is not null tag");fflush(stdout);
									printf("\n 41 save");
									ifail =AOM_refresh(relation1,TRUE);
									ifail = GRM_save_relation(relation1);
									//ifail = AOM_unlock(DMLRevision[0]);
									ifail = AOM_unlock(objTag);
									//ifail =AOM_refresh(relation1,TRUE);
									//ifail = AOM_save(relation1);
									printf("\n relation Parts For Gate Maturation  unlock");
									printf("\n folder Parts For Gate Maturation unlock");
									//ifail =AOM_refresh(relation1,FALSE);
									//ifail =AOM_refresh(DMLRevision[0],FALSE);
								}
								else
								{
									printf("relation1 is null tag");fflush(stdout);
								}
							}
							else
							{
								printf("Ref_Rel is NULLTAG");fflush(stdout);
							}
						}
						else
						{
							printf("\nFolder is not created....");fflush(stdout);
						}
					}
				}
				else
				{
					printf("\nPart number : %s is not live for APL GM DML : %s",revprtnum,revprojcode);fflush(stdout);
				}
			}
			else
			{
				printf("\nEither task or part is nulltag");fflush(stdout);
			}
		}
		else
		{
			printf("\ntsk_CMRef_type is NULLTAG");fflush(stdout);
		}
	}
	else
	{
		printf("Part is not allow to create relation with Task in reference");fflush(stdout);
	}

	return 0;
}

void  getPlantDetailsAttr( char *PlantName ,char  getPlantCR[40])
{
	//char   *PlantName=NULL;

	char    *qry_entryCntrl1[3]  = {"SYSCD","SUBSYSCD","Delete Indicator"};	
	char	**qry_valuesCntrl1= (char **) MEM_alloc(5 * sizeof(char *));

	int cntrlcnt=0;
	int  control_number_found1=0;

	tag_t *list_of_WSO_cntrl_tags1=NULLTAG;

	tag_t   qryTagCntrl1    = NULLTAG;
	int     n_entryCntrl1   = 3;
	char	*MakeBuy 		= NULL;
	char	*OptionalCS		= NULL;
	char	*InitialAgncy	= NULL;
	char	*StoreLoc		= NULL;
	char	*UserAgncy		= NULL;
	char	*PlantCLosRule	= NULL;
	char	*UserLoc		= NULL;
	char	*PlantOneChar	= NULL;
	char	* APlPltNm		= NULL;
	int max_char_size = 80;
	APlPltNm = (char *)MEM_alloc(max_char_size * sizeof(char));

	printf("Inside getPlantDetailsAttr plant name : %s",PlantName);fflush(stdout);
		
	if(QRY_find("Control Objects...", &qryTagCntrl1));
	if (qryTagCntrl1)
	{
		printf("\n Control Object Query Found \n");fflush(stdout);
		qry_valuesCntrl1[0]="APLPltInf";
		qry_valuesCntrl1[1]=PlantName;
		qry_valuesCntrl1[2]="0";
		
		if(QRY_execute(qryTagCntrl1, n_entryCntrl1, qry_entryCntrl1, qry_valuesCntrl1, &control_number_found1, &list_of_WSO_cntrl_tags1));
	}
	else
	{
		printf("\n Control Object Query not Found for GM DML \n");fflush(stdout);
	}	
	
	printf("\n control_number_found1 is : %d\n",control_number_found1);fflush(stdout);

	if(control_number_found1>0)
	{
		for(cntrlcnt=0;cntrlcnt<control_number_found1;cntrlcnt++)
		{
			AOM_ask_value_string( list_of_WSO_cntrl_tags1[cntrlcnt], "t5_Userinfo1", &MakeBuy);
			printf("\n MakeBuy: %s\n",MakeBuy);fflush(stdout);

			AOM_ask_value_string( list_of_WSO_cntrl_tags1[cntrlcnt], "t5_Userinfo2", &OptionalCS);
			printf("\n OptionalCS: %s\n",OptionalCS);fflush(stdout);

			AOM_ask_value_string( list_of_WSO_cntrl_tags1[cntrlcnt], "t5_Userinfo3", &InitialAgncy);
			printf("\n InitialAgncy: %s\n",InitialAgncy);fflush(stdout);

			AOM_ask_value_string( list_of_WSO_cntrl_tags1[cntrlcnt], "t5_Userinfo4", &StoreLoc);
			printf("\n StoreLoc: %s\n",StoreLoc);fflush(stdout);

			AOM_ask_value_string( list_of_WSO_cntrl_tags1[cntrlcnt], "t5_Userinfo5", &UserAgncy);
			printf("\n UserAgncy: %s\n",UserAgncy);fflush(stdout);

			AOM_ask_value_string( list_of_WSO_cntrl_tags1[cntrlcnt], "t5_Userinfo6", &PlantCLosRule);
			printf("\n PlantCLosRule: %s\n",PlantCLosRule);fflush(stdout);

			AOM_ask_value_string( list_of_WSO_cntrl_tags1[cntrlcnt], "t5_Userinfo7", &UserLoc);
			printf("\n UserLoc: %s\n",UserLoc);fflush(stdout);

			//tc_strcpy(getPlantCS,MakeBuy);
			//tc_strcpy(getPlantOptCS,OptionalCS);
			//tc_strcpy(getPlantIA,InitialAgncy);
			//tc_strcpy(getPlantStore,StoreLoc);
			//tc_strcpy(getUserAgency,UserAgncy);
			tc_strcpy(getPlantCR,PlantCLosRule);
			//tc_strcpy(getUsrLocation,UserLoc);


		}
	}
	else
	{
		//tc_strcpy(getPlantCS,"t5_PunMakeBuyIndicator");
		//tc_strcpy(getPlantOptCS,"bl_occ_t5_PunOptionalCS");
		//tc_strcpy(getPlantIA,"t5_PunIntialAgency");
		//tc_strcpy(getPlantStore,"t5_PunStoreLocation");
		//tc_strcpy(getUserAgency,"PUN");
		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLP");
		//tc_strcpy(getUsrLocation,"CVBU PUNE");
	}
}

void  get_CtrlVal_APLLcsInf( char * Tsk_Own_Grp ,char  RevRuleInfo1[40],char  BVRInfo2[40],char  CurMskInfo3[40])
{
	 char    *qry_entryCntrl1[3]  = {"SYSCD","SUBSYSCD","Delete Indicator"};	
	 char	**qry_valuesCntrl1= (char **) MEM_alloc(5 * sizeof(char *));

	 int cntrlcnt=0;
	 int  control_number_found1=0;

	 tag_t *list_of_WSO_cntrl_tags1=NULLTAG;

	 tag_t   qryTagCntrl1     = NULLTAG;
	 int     n_entryCntrl1    = 3;
	 char *RevRule 			= NULL;
	 char *viewfrmCtrl		= NULL;
	 char *CurVwMask		= NULL;
	 //char *Info4Ctrl		= NULL;
	 //char *Info5Ctrl		= NULL;
	// char *Info6Ctrl		= NULL;
	// char *Info7Ctrl		= NULL;
	// char *Info8Ctrl		= NULL;
	// char *Info9Ctrl		= NULL;

	 if(QRY_find("Control Objects...", &qryTagCntrl1));
			if (qryTagCntrl1)
			{
				printf("\n Control Object Query Found \n");fflush(stdout);
				qry_valuesCntrl1[0]="APLLcsInf";
				qry_valuesCntrl1[1]=Tsk_Own_Grp;
				qry_valuesCntrl1[2]="0";
				
				if(QRY_execute(qryTagCntrl1, n_entryCntrl1, qry_entryCntrl1, qry_valuesCntrl1, &control_number_found1, &list_of_WSO_cntrl_tags1));
			}
			else
			{
				printf("\n Control Object Query not Found \n");fflush(stdout);
			}	
			
			printf("\n control_number_found1 is : %d\n",control_number_found1);fflush(stdout);

			if(control_number_found1>0)
			{
				
				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo1", &RevRule);
				printf("\n RevRule: %s\n",RevRule);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo2", &viewfrmCtrl);
				printf("\n viewfrmCtrl: %s\n",viewfrmCtrl);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo3", &CurVwMask);
				printf("\n CurVwMask: %s\n",CurVwMask);fflush(stdout);

				tc_strcpy(RevRuleInfo1,RevRule);
				tc_strcpy(BVRInfo2,viewfrmCtrl);
				tc_strcpy(CurMskInfo3,CurVwMask);
					
			}
			else
			{
				tc_strcpy(RevRuleInfo1,"ERC release and above");
				tc_strcpy(BVRInfo2,"View");
				tc_strcpy(CurMskInfo3,"bl_occ_t5_CurrentViewMaskC");
			}
}

int tm_GateMaturaion(tag_t tTaskTag, char	*cPlantName)
{
	int		ifail 				=	ITK_ok;
	int		dmlcount			=	0;
	int		DR_rsltCount		=	0;
	int		CMRefcount			=	0;
	int		Mstr				=	0;
	int		num_found			=	0;
	int		idml				=	0;
	int		iChildItemTag		=	0;		
	int		itsk				=	0;
	int		iadml				=	0;
	int		GMDMLFound			=	0;
	int     DR_n_entry			=	1;

	char	*ObjTyp				=	NULL;
	char	*class_name1		=	NULL;
	char	*DMLtype			=	NULL;
	char	*DMLECNtype			=	NULL;
	char	*DMLProj			=	NULL;
	char	*PlntToPlnt			=	NULL;
	char	*PlntToPlnt1		=	NULL;
	char	*FromPlnt			=	NULL;
	char	*ToPlnt				=	NULL;
	char	*TskNm				=	NULL;
	char	*LiveProjInfo		=	NULL;
	char	*PartMstr_no		=	NULL;
	char	*sUserGroup			=	NULL;
	char	*AMDMLtype			=	NULL;
	char	*ItemRevOwnGrp		=	NULL;
	char	*ItemRevDRStatus	=	NULL;

	char	*ItemType			=	NULL;
	char	*ItemName			=	NULL;
	char	*ItemRev			=	NULL;
	
	char    *DR_qry_entry[1]  = {"SYSCD"};
	char    **DR_qry_values2     =  (char **) MEM_alloc(10 * sizeof(char *));
	
	logical is_latest;

	tag_t	tsk_dml_rel_type	=	NULLTAG;		
	tag_t	*DMLRevision		=	NULLTAG;
	tag_t	class_id1			=	NULLTAG;	
	tag_t   DRqryTag			=	NULLTAG;
	tag_t	*DRCntrlObjectTag	=	NULLTAG;
	tag_t	tsk_CMRef_type		=	NULLTAG;
	tag_t	*TaskCMRef			=	NULLTAG;
	tag_t	AssyMstrtag			=	NULLTAG;
	tag_t	tag_query			=	NULLTAG;
	tag_t	DMLRevTag			=	NULLTAG;
	tag_t	*itemclass1			=	NULLTAG;
	tag_t	objChild_BVR		=	NULLTAG;
	tag_t	t_ChildItemRev_BS	=	NULLTAG;
	tag_t	t_apldml			=	NULLTAG;

	char AplRvwLC[40];
	char AplWrkngLC[40];
	char AplRlzd[40];
	char Stdwrknglc[40];
	char Stdrlzdlc[40];
	char PltLcs[40];
	char StdPlant[40];
	char StdRevRule[40];
	char StdCLosRule[40];
	char StdView[40];
	char RevRuleInfo1[40];
	char BVRInfo2[40];
	char Info3[40];
	char closureRuleName[40];

	printf("\nInside tm_GateMaturaion");fflush(stdout);
	printf("\nAPL GM DML Plant NAME : %s",cPlantName);fflush(stdout);
	get_CtrlVal_APLStateInf(cPlantName,AplRvwLC,AplWrkngLC,AplRlzd,Stdwrknglc,Stdrlzdlc,PltLcs,StdPlant,StdRevRule,StdCLosRule,StdView);
	printf("\nPlant name: %s, View : %s",cPlantName,StdView);fflush(stdout);
	printf("\nPlant name: %s, StdRevRule : %s",cPlantName,StdRevRule);fflush(stdout);

	//getDMLUserGroup(cPlantName, sUserGroup);
	//printf("\nsUserGroup : %s",sUserGroup);fflush(stdout);
		


	if(AOM_ask_value_string(tTaskTag,"object_type",&ObjTyp)!=ITK_ok)PrintErrorStack();
	if(tc_strcmp(ObjTyp,"T5_APLTaskRevision")==0)
	{
		ITKCALL(AOM_UIF_ask_value(tTaskTag,"owning_group",&sUserGroup));
		printf("\n  taskowning_groupVal:%s ..............",sUserGroup);fflush(stdout);

		get_CtrlVal_APLLcsInf(sUserGroup,RevRuleInfo1,BVRInfo2,Info3);
		getPlantDetailsAttr(cPlantName ,closureRuleName);

		printf("\nAPL GM DML Revsion Rule : %s",RevRuleInfo1);fflush(stdout);
		printf("\nAPL GM DML Revsion BVRInfo2 : %s",BVRInfo2);fflush(stdout);
		printf("\nAPL GM DML Revsion Info3 : %s",Info3);fflush(stdout);
		printf("\nAPL GM DML Revsion closureRuleName : %s",closureRuleName);fflush(stdout);

		if (GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type)!=ITK_ok)PrintErrorStack();
		if (tsk_dml_rel_type!=NULLTAG)
		{
			if (GRM_list_primary_objects_only(tTaskTag,tsk_dml_rel_type,&dmlcount,&DMLRevision)!=ITK_ok)PrintErrorStack();
			printf("\n APL GM: DML Revision from Task: %d.",dmlcount);fflush(stdout);
			if (dmlcount>0)
			{
				for (idml=0;idml<dmlcount ;idml++ )
				{
					if (ITEM_rev_sequence_is_latest(DMLRevision[idml],&is_latest)!=ITK_ok)PrintErrorStack();
					if(is_latest != true)
					{
						printf("\nAPL GM: is_latest is not true.");fflush(stdout);
					}
					else
					{	
						DMLtype		=	NULL;
						DMLECNtype	=	NULL;
						DMLProj		=	NULL;
						PlntToPlnt	=	NULL;
						PlntToPlnt1	=	NULL;
						DMLRevTag = DMLRevision[idml];
						ifail = POM_class_of_instance(DMLRevTag,&class_id1);
						ifail = POM_name_of_class(class_id1,&class_name1);
						printf("\n GM: class_name found1 MR :%s.",class_name1);fflush(stdout);
						ifail = AOM_ask_value_string(DMLRevTag,"t5_EcnType",&DMLtype);
						ifail = AOM_ask_value_string(DMLRevTag,"t5_rlstype",&DMLECNtype);
						ifail = AOM_ask_value_string(DMLRevTag,"t5_cprojectcode",&DMLProj);
						ifail = AOM_ask_value_string(DMLRevTag,"t5_PlntToPlnt",&PlntToPlnt);
						ifail = AOM_UIF_ask_value(DMLRevTag,"t5_PlntToPlnt",&PlntToPlnt1);
						printf("\n GM: DMLtype:[%s] DMLProj:[%s] PlntToPlnt:[%s].[%s], ECN Type : [%s]",DMLtype,DMLProj,PlntToPlnt,PlntToPlnt1,DMLECNtype);fflush(stdout);

						if(tc_strcmp(DMLtype,"TODRAPL")==0)
						{
							if(PlntToPlnt!=NULL)
							{
								char	*TaskDsgGrp	=	NULL;
								FromPlnt=subString(PlntToPlnt, 0, 3);
								if (tc_strstr(PlntToPlnt,"3P")!=NULL)
								{
									ToPlnt=subString(PlntToPlnt, 7, 4);
								}
								else
								{
									ToPlnt=subString(PlntToPlnt, 7, 3);
								}
								ifail = AOM_UIF_ask_value(tTaskTag,"item_id",&TskNm);//Task Number
								TaskDsgGrp=subString(TskNm,11,2);
								printf("\n GM: Task: [%s] TaskDsgGrp:[%s] FromPlnt:[%s] ToPlnt:[%s]\n",TskNm,TaskDsgGrp,FromPlnt,ToPlnt);fflush(stdout);
								if(tc_strstr(TskNm,"CO")==NULL)
								{
									if(QRY_find("ControlObjects", &DRqryTag));
									if (DRqryTag)
									{
										printf("\n APLCREVali:Found Query ControlObjects \n");fflush(stdout);
									}
									else
									{
										printf("\n APLCREVali:Not Found Query ControlObjects \n");fflush(stdout);
									}

									DR_qry_values2[0] = "APLCREVali";
									if(QRY_execute(DRqryTag, DR_n_entry, DR_qry_entry, DR_qry_values2, &DR_rsltCount, &DRCntrlObjectTag));
									printf(" \n APLCREVali:DR_rsltCount :%d:", DR_rsltCount); fflush(stdout);
									if(DR_rsltCount > 0)
									{
										if (AOM_ask_value_string(DRCntrlObjectTag[0],"t5_Userinfo5",&LiveProjInfo)!=ITK_ok)   PrintErrorStack();
										printf("\n APL GM DML LiveProjInfo:%s\n",LiveProjInfo);fflush(stdout);
									}
									ifail = GRM_find_relation_type("CMReferences", &tsk_CMRef_type);
									if (tsk_CMRef_type!=NULLTAG)
									{
										ifail = GRM_list_secondary_objects_only(tTaskTag,tsk_CMRef_type,&CMRefcount,&TaskCMRef);
										printf("\n APL GM DML: Task CMRefcount: %d",CMRefcount);fflush(stdout);
										if(CMRefcount > 0)
										{
											Mstr	=	0;
											for(Mstr=0;Mstr<CMRefcount;Mstr++)
											{
												printf("\n GM:Design Mstr:%d",Mstr);fflush(stdout);
												AssyMstrtag=TaskCMRef[Mstr];
												ifail = AOM_ask_value_string(AssyMstrtag,"item_id",&PartMstr_no);
												printf("\n GM:Part Mstr_no:%s",PartMstr_no);fflush(stdout);

												//Find the latest APLC Released part
												ITKCALL(QRY_find("Driver VC Query",&tag_query));
												printf("Searching...\n");fflush(stdout);
												int		n_entries		=	2;
												char	*entries[2]		=	{"Item ID","Release Status"};
												char	**values		=	(char **) MEM_alloc(10 * sizeof(char *));
												char	*DriverVCLCS	=	NULL;
												char	*DriverVC		=	NULL;

												DriverVC	=	(char *)MEM_alloc(tc_strlen(PartMstr_no)+50);
												tc_strcpy(DriverVC,PartMstr_no);

												DriverVCLCS	=	(char *)MEM_alloc(tc_strlen(AplRlzd)+50);
												tc_strcpy(DriverVCLCS,AplRlzd);
												printf("\nDrive VC LCS : %s",DriverVCLCS);fflush(stdout);
												printf("nArgument passed in is: %s...\n",DriverVC);fflush(stdout);
												
												num_found	=	0;
												itemclass1	=	NULLTAG;

												values[0]=DriverVC;
												values[1]=DriverVCLCS;

												if(QRY_execute(tag_query,n_entries, entries, values,&num_found,&itemclass1))	PrintErrorStack();
												printf("\nNo of Part Found : %d",num_found);fflush(stdout);
												
												if(num_found>0)
												{
													printf("\nEXP APL GM DML Revsion Rule : %s",RevRuleInfo1);fflush(stdout);
													printf("\nEXP APL GM DML Revsion BVRInfo2 : %s",BVRInfo2);fflush(stdout);
													printf("\nEXP APL GM DML Revsion Info3 : %s",Info3);fflush(stdout);

													tag_t	AssyRev		=	NULLTAG;
													AssyRev				=	itemclass1[0];
													
													//Explode the BOM
													int	childBS = 0;
													int	StructCntBdyShProdPlan	= 0;
													struct			BomChldStrut	ChldStrutBdySh[5000];

													ITKCALL(Get_Part_BOM_Lvl(AssyRev,1,closureRuleName,RevRuleInfo1,BVRInfo2,ChldStrutBdySh,&StructCntBdyShProdPlan));
													printf("\n\t\t StructCntBdyShProdPlan:%d",StructCntBdyShProdPlan);fflush(stdout);

													for(childBS = 1;childBS <= StructCntBdyShProdPlan; childBS++)
													{
														ItemType			=	NULL;
														ItemName			=	NULL;
														ItemRev				=	NULL;
														ItemRevOwnGrp		=	NULL;
														GMDMLFound			=	0;

														objChild_BVR		=	NULLTAG;
														t_ChildItemRev_BS	=	NULLTAG;
														iChildItemTag		=	0;


														objChild_BVR=ChldStrutBdySh[childBS].child_objs_bvr;
														t_ChildItemRev_BS=ChldStrutBdySh[childBS].child_objs;

														//ITKCALL(BOM_line_unpack (objChild_BVR));
														//ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
														//ITKCALL(BOM_line_ask_attribute_tag(objChild_BVR, iChildItemTag, &t_ChildItemRev_BS));

														ITKCALL(AOM_ask_value_string(t_ChildItemRev_BS,"t5_PartType",&ItemType));
														printf("\n child ItemType:%s ..............",ItemType);fflush(stdout);

														ITKCALL(AOM_ask_value_string(t_ChildItemRev_BS,"item_id",&ItemName));
														printf("\n child ItemName:%s ..............",ItemName);fflush(stdout);

														ITKCALL(AOM_ask_value_string(t_ChildItemRev_BS,"item_revision_id",&ItemRev));
														printf("\n child ItemRev:%s ..............",ItemRev);fflush(stdout);//owning_group

														//ITKCALL(AOM_ask_value_string(t_ChildItemRev_BS,"owning_group",&ItemRevOwnGrp));
														//printf("\n child ItemRevOwnGrp:%s ..............",ItemRevOwnGrp);fflush(stdout);

														ITKCALL(AOM_ask_value_string(t_ChildItemRev_BS,"t5_PartStatus",&ItemRevDRStatus));
														printf("\n child ItemRevDRStatus:%s ..............",ItemRevDRStatus);fflush(stdout);
														
														//FOR APL GM DML ONLY SA will be attached
														//if (tc_strcmp(ItemType,"SA")==0 && tc_strstr(ItemRevOwnGrp,"APL")!=NULL)
														if (tc_strcmp(ItemType,"SA")==0)
														{
															printf("\n[%s] having part type [%s] and owing group [%s]",ItemName,ItemType,ItemRevOwnGrp);fflush(stdout);
															int iGMDMLFnd	=	0;
															int	n_tskfund	=	0;
															int *levelsaa;
															char	**relations	=  NULL;
															tag_t	*referencers = NULLTAG;
															ITKCALL(WSOM_where_referenced(t_ChildItemRev_BS,1,&n_tskfund,&levelsaa,&referencers,&relations));

															printf("\nNo of Reference Found : %d",n_tskfund);fflush(stdout);
															if (n_tskfund>0)
															{
																GMDMLFound=0;
																for (itsk=0; itsk<n_tskfund;itsk++ )
																{
																	int		dmlcnt	=	0;
																	char	*taskname	=	NULL;
																	tag_t	*DMLRev	=	NULLTAG;
																	  
																	ITKCALL(GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type));
																	ITKCALL(GRM_list_primary_objects_only(referencers[itsk],tsk_dml_rel_type,&dmlcnt,&DMLRev));
																	printf("\nNo of DML found : %d",dmlcnt);fflush(stdout);
																	//ITKCALL(AOM_ask_value_string(referencers[itsk],"item_id",&taskname));
																	printf("\n child ItemName:%s ..............",ItemName);fflush(stdout);

																	if (dmlcnt>0)
																	{
																		for (iadml-0;iadml<dmlcnt;iadml++)
																		{
																			t_apldml	=	NULLTAG;
																			AMDMLtype	=	NULL;
																			t_apldml	=	DMLRev[iadml];
																			ITKCALL(AOM_ask_value_string(t_apldml,"t5_EcnType",&AMDMLtype));

																			if (tc_strcpy(AMDMLtype,"TODRAPL")==0)
																			{
																				printf("\n Part is already attached to GM DML task. %s",ItemName);
																				GMDMLFound=1;
																				break;
																			}
																		}
																		if (GMDMLFound>0)
																		{
																			printf("\nGMDMLFound> 0 so break");fflush(stdout);
																			break;
																		}
																	}
																}
															}
															if (GMDMLFound==0)
															{
																//if GMDML not found attach that part with GM DML AplRlzd
																int		st_count1		=	0;
																int		iAPLRlzFnd		=	0;
																int		iRlz			=	0;
																char	*WSO_Name		=	NULL;
																tag_t	*status_list1	=	NULLTAG;

																ITKCALL(WSOM_ask_release_status_list(t_ChildItemRev_BS,&st_count1,&status_list1));
																printf("\n 111..st_count1 :%d is\n",st_count1);fflush(stdout);

																for (iRlz=0;iRlz<st_count1 ;iRlz++)
																{
																	WSO_Name=NULL;
																	ITKCALL(AOM_ask_name(status_list1[iRlz],&WSO_Name));
																	printf("\n 111..ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);
																	if (tc_strcmp(WSO_Name,AplRlzd)==0)
																	{
																		iAPLRlzFnd++;
																		printf("\niAPLRlzFnd : %d",iAPLRlzFnd);fflush(stdout);
																		break;
																	}
																}
																if (iAPLRlzFnd>0)
																{
																	//If APL Released found, then attached the part with task
																	ifail = CreateRelFun( tTaskTag,ToPlnt,ItemRevDRStatus,t_ChildItemRev_BS,LiveProjInfo);
																}
															}
														}//IF SA condition ends
													}//BOM Line for loop ends
													
												}

											}//References Item loop ends
										}
										else
										{
											printf("\nAPL GM DML No Design revision found in Reference Relation...");fflush(stdout);
										}
									}
									else
									{
										printf("\n APL GM DML Reference relation not found...");fflush(stdout);
									}
								}
								else
								{
									printf("\n GM: No Expanssion for CO task.");fflush(stdout);
								}
							}
							else
							{
								printf("\n GM: t5_PlntToPlnt is null.");fflush(stdout);
							}
						}
						else
						{
							printf("\n GM: Other than GM DML.");fflush(stdout);
						}
					}
				}
			}
			else
			{
				printf("\nNo DML tag found....");fflush(stdout);
			}
		}
		else
		{
			printf("\nT5_DMLTaskRelation relation is not found...");fflush(stdout);
		}
	}
	else
	{
		printf("\nObject type is not APL Task Revision...");fflush(stdout);
	}

	return ifail;
}
extern int ITK_user_main(int argc, char *argv[])
{
	int				ifail 			=	ITK_ok;
	int				totalDmlCnt		=	0;
	int				idml			=	0;

	char			*dmlnob			=	NULL;
	char			*cPlantName		=	NULL;
	char			*item_type		=	NULL;
	char			*type_name		=	NULL;
	char			*DmlEcnTypDup	=	NULL;
	char			*cdmlnumber		=	NULL;
	char			*getPlant		=	NULL;

	tag_t			task_tag			=	NULLTAG;
	tag_t			*dmlRevTagList		=	NULLTAG;
	tag_t			dmlrevtag			=	NULLTAG;
	tag_t			tsk_rev_tag			=	NULLTAG;
	tag_t			objTypeTag			=	NULLTAG;
	tag_t			task_dml_rel_type	=	NULLTAG;
	
	dmlnob 		= ITK_ask_cli_argument("-dmlno=");

	ifail = ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
	printf("\n Auto login ");fflush(stdout);
	ifail = ITK_auto_login( );
	ifail = ITK_set_journalling( TRUE);
	printf("\n Auto login .......\n");fflush(stdout);
	if (ifail != ITK_ok)
	{
		printf("Error in Login to Teamcenter\n");
		return ifail;
	}
	else
	{
		printf("\nLogin Successfull.....!!\n");
		printf("\nWelcome to Teamcenter.....!!\n");
	}
	ifail = ITK_set_bypass(true);
	if (ifail != ITK_ok)
	{
		printf("\nUser is not Privileged Admin User \n\n");
		return -1;
	}
	ifail = ITEM_find_item( dmlnob, &task_tag );
	printf("\n Task Find 123.......: %s",dmlnob);fflush(stdout);

	if (task_tag != NULLTAG)
	{
		printf("\n Task Find2 .......");fflush(stdout);
		ifail = ITEM_ask_type2 (task_tag, &item_type);
		printf("\nTASK Type : %s",item_type);fflush(stdout);
		
		//CALLAPI(ITEM_find_revision(task_tag,"A",&tsk_rev_tag)) ;
		ITKCALL(ITEM_ask_latest_rev(task_tag,&tsk_rev_tag)) ;

		if (tsk_rev_tag  != NULLTAG)
		{
			if(TCTYPE_ask_object_type(tsk_rev_tag,&objTypeTag));
			if(TCTYPE_ask_name2(objTypeTag,&type_name));
			printf("\n type_name : %s",type_name);fflush(stdout);

			if(strcmp(type_name,"T5_APLTaskRevision")==0)
			{
				//apl_dml_signoff_checks_Func(tsk_rev_tag);
				//Find DML number
				ITKCALL(GRM_find_relation_type("T5_DMLTaskRelation", &task_dml_rel_type));
				ITKCALL(GRM_list_primary_objects_only(tsk_rev_tag,task_dml_rel_type,&totalDmlCnt,&dmlRevTagList));
				printf("\nNo of dml found : %d",totalDmlCnt);fflush(stdout);

				if (totalDmlCnt>0)
				{
					for (idml=0;idml<totalDmlCnt ;idml++ )
					{
						dmlrevtag	=	dmlRevTagList[idml];

						ITKCALL(AOM_ask_value_string(dmlrevtag,"item_id",&cdmlnumber));
						printf("\ncheckAMDMLForGMDML cdmlnumber:%s",cdmlnumber);fflush(stdout);

						ITKCALL(AOM_ask_value_string(dmlrevtag,"t5_EcnType",&DmlEcnTypDup));
						printf("\ncheckAMDMLForGMDML DmlEcnTypDup:%s",DmlEcnTypDup);fflush(stdout);
						
						if (tc_strstr(cdmlnumber,"APL")!=NULL)
						{
							GetPlantForDMLORTask(cdmlnumber ,&getPlant);
						}
						printf("\n Plant name : %s",getPlant);fflush(stdout);
						if (tc_strcmp(DmlEcnTypDup,"TODRAPL")==0)
						{
							//TOO DML FOUND
							printf("\nAPL GM DML found...");fflush(stdout);
							//tm_GateMaturaion(tag_t tTaskTag, char	*cPlantName)
							tm_GateMaturaion(tsk_rev_tag,getPlant);
						}
					}
				}
			}
		}
	}

	printf("\nEnd of program.....!!\n\n");
	printf("\n*****************************\n");
	ifail = ITK_exit_module(true);
	return ifail;
}