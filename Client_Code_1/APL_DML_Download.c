#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sa/sa.h>
#include "unidefs.h"
#define TE_MAXLINELEN  128
#include <unidefs.h>
#include <itk/mem.h>
#include <tcinit/tcinit.h>
#include <tccore/item.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ps/ps_errors.h>
#include <tccore/item_errors.h>
#include <tc/emh.h>
#include <ps/ps.h>
#include <ps/ps_errors.h>
#include <ai/sample_err.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <tccore/item_msg.h>
#include <bom/bom.h>
#include <ae/dataset.h>
#include <ps/ps_errors.h>
#include <sa/sa.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fclasses/tc_string.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <sa/tcfile.h>
#include <tcinit/tcinit.h>
#include <tccore/tctype.h>
#include <tccore/method.h>
#include <property/prop.h>
#include <property/prop_msg.h>
#include <tccore/iman_msg.h>
#include <res/reservation.h>
#include <tccore/aom.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <tccore/tctype_msg.h>
#include <ict/ict_userservice.h>
#include <tc/wsouif_errors.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <sa/imanfile.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <itk/mem.h>
#include <ss/ss_errors.h>
#include <sa/user.h>
#include <pom/pom/pom_tokens.h>
#include <unidefs.h>
#include <property/prop.h>
#include <property/propdesc.h>
#include <tccore/tc_msg.h>
#include <lov/liblov_exports.h>
#include <lov/liblov_undef.h>
#include <ug_va_copy.h>
#include <tccore/grm.h>
#define CHECK_FAIL if (ifail != 0) { printf("line %d (ifail %d)\n", __LINE__, ifail); return 0;}
static void Write_To_Log(char *format, ...)
{
    char msg[1000];
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    printf(msg);
    printf(msg);
}
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
    int *pSevLst = NULL;
    int *pErrCdeLst = NULL;
    char **pMsgLst = NULL;
    register int i = 0;

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst );
    fprintf( stderr, "Checkout Error(s): \n");
	Write_To_Log("Checkout Error(s): \n");
    for ( i = 0; i < iNumErrs; i++ )
    {
        fprintf( stderr, "\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
		Write_To_Log("\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i]);
        fprintf( stderr, "\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
		Write_To_Log("\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i]);
    }
//    	 919003: error_919003, "Modification Description"
//	 919003: error_919003, "Modification Description"
//	 919003: error_919003, "Part Type"
//	 919003: error_919003, "Part Type"
//	 919099: error_919099, "CAP_FOR_PISTON_ROD_PRT", "A", "2"
//	 919099: error_919099, "CAP_FOR_PISTON_ROD_PRT", "A", "2"

    return ITK_ok;
}
int DMLFlag=0;
#define ITK_CALL(X) 							\
		status=X; 								\
		if (status != ITK_ok ) 					\
		{										\
			int				index = 0;			\
			int				n_ifails = 0;		\
			const int*		severities = 0;		\
			const int*		ifails = 0;			\
			const char**	texts = NULL;		\
												\
			EMH_ask_errors( &n_ifails, &severities, &ifails, &texts);		\
			printf("%3d error(s) with #X\n", n_ifails);						\
			for( index=0; index<n_ifails; index++)							\
			{																\
				printf("\tError #%d, %s\n", ifails[index], texts[index]);	\
			}																\
		}																	\
	;
static void print_bom (tag_t line,tag_t Parent, int depth,FILE *fdf,FILE *fus,FILE *fL,FILE *fS,FILE *fPro,char *req_item,char* FilePath,int DepthInput);
char* subString (char* mainStringf ,int fromCharf,int toCharf);

 char* get_ReleaseStatus(tag_t itemrev)
 {
	int		status				= 0;
	int st_count1=0,cnt=0,VaultStsFlagRel=0,VaultStsFlagWIP=0;
	tag_t*    status_list1=NULLTAG;
	char *WSO_Name = NULL;
	char* VaultSts = NULL;

	ITK_CALL(WSOM_ask_release_status_list(itemrev,&st_count1,&status_list1));
	printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);

	if(st_count1==1)
	{
		ITK_CALL(AOM_ask_name(status_list1[0],&WSO_Name));
		printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);
		//if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0 || tc_strcmp(WSO_Name,"ERC Released")==0)
		if (tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0)
		{
			VaultSts="Released";
			//ITK_CALL(AOM_ask_value_string(status_list1[cnt],"effectivity_text",&effectivityStr));
			//printf("\effectivityStr----><%s>\n",effectivityStr );
		}
		//else if (tc_strcmp(WSO_Name,"T5_LcsReview")==0)
		else if (tc_strcmp(WSO_Name,"T5_LcsAplReview")==0 || tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0)
		{
			VaultSts="WIP";
		}
		else
		{
			VaultSts="WIP";
		}
	}
	else if(st_count1>1)
	{
		for (cnt=0;cnt< st_count1;cnt++ )
		{
			ITK_CALL(AOM_ask_name(status_list1[cnt],&WSO_Name));
			printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);  //T5_LcsReview //ERC Released or T5_LcsErcRlzd// Released
			//if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0 || tc_strcmp(WSO_Name,"ERC Released")==0 )
			if (tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0)
			{
				VaultStsFlagRel=1;
				//ITK_CALL(AOM_ask_value_string(status_list1[cnt],"effectivity_text",&effectivityStr));
				//printf("\effectivityStr----><%s>\n",effectivityStr );
			}
			else if (tc_strcmp(WSO_Name,"T5_LcsAplReview")==0 || tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0)//change for Plant base
			{
				VaultStsFlagWIP=1;
			}
		}
		if (VaultStsFlagRel==1)
		{
			VaultSts="Released";
		}
		else if(VaultStsFlagWIP==1)
		{
			VaultSts="WIP";
		}
		else
		{
			VaultSts="WIP";
		}
	}
	else
	{
		VaultSts="NOSTS";
	}
	return VaultSts;
 }

int Complete_DML_Download(char* DmlIdS, char* filenameS)
{
	int		i,j,cnt					= 0;
	int		status				= 0;
	int		flag				= 0;
	int		n_tasks				= 0;
	int		n_parts				= 0;
	int		st_count1				= 0;
	int		ifail				= ITK_ok;

	char*	item_type				= NULL;
	char	*AllDmlAttrS			= NULL;
	char	*AllTaskAttrS			= NULL;
	char	*AllTaskAttrDupS		= NULL;
	// DML Attributes
    char	*Dml_item_idS			= NULL;
    char	*t5_basicdmlS			= NULL;
    char	*object_nameS			= NULL;
    char	*project_idsS			= NULL;
    char	*t5designgroupS			= NULL;
    char	*object_descS			= NULL;
    char	*CMDispositionS			= NULL;
    char	*requestor_user_idS		= NULL;
    char	*analyst_user_idS		= NULL;
    char	*release_status_listS	= NULL;
    char	*owning_userS			= NULL;
    char	*CMClosureS				= NULL;
    char	*CMClosureDateS			= NULL;
	char	*ClosureTmStmp			= NULL;
	char	*ClosureTmStmpFinal		= NULL;
	char	*t5_basicdmlNum			= NULL;
	char	*DRStatus				= NULL;
	char	*DMLtype				= NULL;
	// Task Attributes
    char	*TaskItemIdS			= NULL;
    char	*TaskObjectDescS		= NULL;
    char	*DesignGroupS			= NULL;
    char	*TaskDespositionS		= NULL;
    char	*RequestorS				= NULL;
    char	*TaskAnalystUserIdS		= NULL;
	char	*Creator				= NULL;
	char	*CreatorName			= NULL;
	char	*CreatorId				= NULL;

    char	*PartNumberS			= NULL;
    char	*RevisionS				= NULL;
    char	*SequenceS				= NULL;
    char	*DmlReview				= NULL;
    char	*mcreator				= NULL;
    char	*mcreatorName			= NULL;
    char	*ReviewName				= NULL;
    char	*ReviewId			= NULL;
	char *WSO_Name = NULL;
	char* DMLClosureStmp = NULL;
	tag_t dml_tag					= NULLTAG;
	tag_t dml_rev_tag				= NULLTAG;
	tag_t *task_obj_tag				= NULLTAG;
	tag_t *part_obj_tag				= NULLTAG;
	tag_t reln_type_tag				= NULLTAG;
	tag_t task_rev_tag				= NULLTAG;
	tag_t*    status_list1=NULLTAG;
	FILE	*DML_Download_fp		=	NULL;
	char* effectivityStr= NULL;
	char* effectivityStrFinal= NULL;
	char* object_nameS_O= NULL;
	int      month      = 0;          /* Month */
    int      day        = 0;          /* Day */
    int      year       = 0;          /* Year */
    int      hour       = 0;          /* Hour */
    int      minute     = 0;          /* Minutes */
    int      second     = 0;          /* Seconds */
	int      CTSmonth   = 0;          /* Month */
    int      CTSday     = 0;          /* Day */
    int      CTSyear    = 0;          /* Year */
    int      CTShour    = 0;          /* Hour */
    int      CTSminute  = 0;          /* Minutes */
    int      CTSsecond  = 0;          /* Seconds */

	char	*t5PunMakeBuyIndS		=	NULL;
	char	*t5PunIntialAgS			=	NULL;
	char	*t5PunStoreLocationS	=	NULL;

	AllDmlAttrS = ( char * ) MEM_alloc(600);
	effectivityStrFinal = ( char * ) MEM_alloc(30);
	AllDmlAttrS=strcpy(AllDmlAttrS,"");

	mcreator = (char *) MEM_alloc( 200 * sizeof(char) );
	mcreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	ReviewName = (char *) MEM_alloc( 200 * sizeof(char) );
	ReviewId = (char *) MEM_alloc( 200 * sizeof(char) );
	ClosureTmStmpFinal = ( char * ) MEM_alloc(30);
	DMLClosureStmp = ( char * ) MEM_alloc(30);

	DML_Download_fp=fopen(filenameS,"w");
	fflush(DML_Download_fp);

	// Query the DML in PLM. 
	ifail = ITEM_find_item( DmlIdS, &dml_tag );

	if (dml_tag != NULLTAG)
	{
		printf("\n\n Complete_DML_Download : DML found ! ! ! \n\n");  fflush(stdout);
		printf("\n\n Complete_DML_Download : [%s] \n\n",AllDmlAttrS);  fflush(stdout);


		// find the type of dml_tag
		ifail = ITEM_ask_type2 (dml_tag, &item_type);
		printf("\n Complete_DML_Download : item_typeS [%s] \n",item_type);  fflush(stdout);
		    

		// Find the DML-Revision tag.
		//ITK_CALL(ITEM_find_revision(dml_tag,"NR",&dml_rev_tag)) ;
		ITK_CALL(ITEM_ask_latest_rev(dml_tag,&dml_rev_tag)) ;
		if (dml_rev_tag  != NULLTAG)
		{
			printf("\n Complete_DML_Download : item_typeS [%s] \n",item_type);  fflush(stdout);
			
			// Collecting DML-Revision Attributes for Download.

			//1
			if (AOM_ask_value_string(dml_rev_tag,"item_id",&Dml_item_idS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : item_id : %s \n",Dml_item_idS); fflush(stdout);
			if (Dml_item_idS)               
				strcat(AllDmlAttrS,Dml_item_idS);
			strcat(AllDmlAttrS,",");
			printf("\n\n Complete_DML_Download : [%s] \n\n",AllDmlAttrS);  fflush(stdout);

			
			//2
			if (AOM_ask_value_string(dml_rev_tag,"object_name",&object_nameS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : object_nameS : %s \n",object_nameS); fflush(stdout);
			STRNG_replace_str(object_nameS,",",";",&object_nameS_O);
			printf("\n Complete_DML_Download : object_nameS : =%s\n",object_nameS_O);
			if (object_nameS_O)               
				strcat(AllDmlAttrS,object_nameS_O);
			strcat(AllDmlAttrS,",");


			//3
			//if (AOM_ask_value_string(dml_rev_tag,"project_ids",&project_idsS)!=ITK_ok)   PrintErrorStack();
			if (AOM_ask_value_string(dml_rev_tag,"t5_cprojectcode",&project_idsS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : project_ids : %s \n",project_idsS); fflush(stdout);
			if (project_idsS)               
				strcat(AllDmlAttrS,project_idsS);
			strcat(AllDmlAttrS,",");			


			//4
			char **DesignGroupList=NULL;
			int num =0;
			if (AOM_ask_value_strings(dml_rev_tag,"t5_crdesigngroup",&num,&DesignGroupList)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : t5designgroup : %s , %d\n",DesignGroupList[0],num); fflush(stdout);
			if (DesignGroupList[0])               
				strcat(AllDmlAttrS,DesignGroupList[0]);
			strcat(AllDmlAttrS,",");


			//5
			if (AOM_ask_value_string(dml_rev_tag,"object_desc",&object_descS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : object_desc : %s \n",object_descS); fflush(stdout);
			if (object_descS)               
				strcat(AllDmlAttrS,object_descS);
			strcat(AllDmlAttrS,",");


			//6
			if (AOM_ask_value_string(dml_rev_tag,"CMDisposition",&CMDispositionS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : CMDisposition : %s \n",CMDispositionS); fflush(stdout);
			if (CMDispositionS)               
				strcat(AllDmlAttrS,CMDispositionS);
			strcat(AllDmlAttrS,",");


			//7 Authorised By Not Present in TCUA
             
			strcat(AllDmlAttrS,"ercpsup");
			strcat(AllDmlAttrS,",");


			//8 Change review Board in TCUA 
			if (AOM_UIF_ask_value(dml_rev_tag,"ChangeReviewBoard",&DmlReview)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : DmlReview : %s \n",DmlReview); fflush(stdout);
			ReviewName = strtok(DmlReview,"(");
			ReviewId= strtok(NULL,")");
			if (ReviewId)               
				strcat(AllDmlAttrS,ReviewId);
			strcat(AllDmlAttrS,",");

			//9
			strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");

			//10 DML owner name
							strcat(AllDmlAttrS,"super user");	//temp value
			strcat(AllDmlAttrS,",");

			//11 Creator Not Present in TCUA 
			ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"owning_user",&Creator));
			//if (AOM_ask_value_string(dml_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : Creator : %s \n",Creator); fflush(stdout);
			CreatorName = strtok(Creator,"(");
			CreatorId= strtok(NULL,")");
			if (CreatorId)               
				strcat(AllDmlAttrS,CreatorId);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,"");  
			//fprintf(DML_Download_fp,",");

			//12 DML requestor
			ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"requestor_user_id",&requestor_user_idS));
			//if (AOM_ask_value_string(dml_rev_tag,"requestor_user_id",&requestor_user_idS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : requestor_user_id : %s \n",requestor_user_idS); fflush(stdout);
			if (requestor_user_idS)               
				strcat(AllDmlAttrS,requestor_user_idS);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,requestor_user_idS);  
			//fprintf(DML_Download_fp,",");

			//13 DML Analyst
			ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"analyst_user_id",&analyst_user_idS));
			//if (AOM_ask_value_string(dml_rev_tag,"analyst_user_id",&analyst_user_idS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : analyst_user_id : %s \n",analyst_user_idS); fflush(stdout);
			if (analyst_user_idS)               
				strcat(AllDmlAttrS,analyst_user_idS);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,analyst_user_idS);  
			//fprintf(DML_Download_fp,",");

			//14 Administrator not present in TCUA
			//if (AOM_ask_value_string(dml_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
			//printf("\n Complete_DML_Download : ??? : %s \n",???); fflush(stdout);
			//if (t5_basicdmlS)               
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,"");  
			//fprintf(DML_Download_fp,",");

			//15 Planning Status not present in TCUA
			//if (AOM_ask_value_string(dml_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
			//printf("\n Complete_DML_Download : ??? : %s \n",???); fflush(stdout);
			//if (t5_basicdmlS)               
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,"");  
			//fprintf(DML_Download_fp,",");

			//16 DML closure
			if (AOM_ask_value_string(dml_rev_tag,"CMClosure",&CMClosureS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : CMClosure : %s \n",CMClosureS); fflush(stdout);
			if (CMClosureS)               
				strcat(AllDmlAttrS,CMClosureS);//CcfPlanClosed in TCE,Closed in TCUA
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,CMClosureS);  
			//fprintf(DML_Download_fp,",");

			//17 DML Reason
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");

			//18 DML Type ,object_type confirm once ???????
			ITK_CALL(AOM_ask_value_string(dml_rev_tag,"t5_rlstype",&DMLtype));
			printf("\n Complete_DML_Download : object_type : %s \n",DMLtype); fflush(stdout);
			if (DMLtype)strcat(AllDmlAttrS,DMLtype);
			strcat(AllDmlAttrS,",");

			//19 DML Release Letter Ref
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");

			//20 DML Release notes
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");

			//21 DML Addenda
			if (AOM_ask_value_string(dml_rev_tag,"t5_basicdml",&t5_basicdmlNum)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : t5_basicdml : %s \n",t5_basicdmlNum); fflush(stdout);
			char *temp = NULL;
			if(t5_basicdmlNum == NULL)
			{
				strcat(AllDmlAttrS,t5_basicdmlNum);
			}
			else
			{
				temp = subString(Dml_item_idS,0,7);
				printf("\n Complete_DML_Download : t5_basicdml substring : %s \n",temp); fflush(stdout);
				strcat(AllDmlAttrS,temp);
			}
				
			strcat(AllDmlAttrS,",");

			//22 DML Part status (DR status)
			ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"t5_cDRstatus",&DRStatus));
			printf("\n Complete_DML_Download : DRStatus : %s \n",DRStatus); fflush(stdout);
			if(DRStatus)
				strcat(AllDmlAttrS,DRStatus);
			strcat(AllDmlAttrS,",");

			//23 DML Closure Date
			//ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"CMClosureDate",&CMClosureDateS));
			printf("\n Complete_DML_Download : CMClosureDate : %s \n",CMClosureDateS); fflush(stdout);
			//ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"date_released",&ClosureTmStmp));
			ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"t5_APLReleaseDate",&ClosureTmStmp));
			printf("\n Complete_DML_Download : date_released : %s \n",ClosureTmStmp); fflush(stdout);
			if (ClosureTmStmp!=NULL)
			{
				//ClosureTmStmpFinal = strtok(ClosureTmStmp," ");
				DATE_string_to_date ( ClosureTmStmp , "%d-%b-%Y %H:%M:%S", &CTSmonth , &CTSday ,&CTSyear , &CTShour , &CTSminute , &CTSsecond);
				printf("\nClosureTmStmpFinal----><%s> %d-%d-%d:%d:%d:%d\n",ClosureTmStmpFinal,CTSday,CTSmonth,CTSyear,CTShour,CTSminute,CTSsecond);
				CTSmonth++;
				//sprintf(DMLClosureStmp,"%d/%d/%d-%d:%d:%d:000",CTSyear,CTSmonth,CTSday,CTShour,CTSminute,CTSsecond);
			}

			/*if (CMClosureDateS != NULL)               
				strcat(AllDmlAttrS,CMClosureDateS);
			else
			{*/
				CMClosureDateS = ( char * ) MEM_alloc(30);
				sprintf(CMClosureDateS,"%d/%d/%d",CTSyear,CTSmonth,CTSday);
				if (CTSmonth<10)
				{
					
					if (CTSday<10)
					{
						sprintf(CMClosureDateS,"%d/0%d/0%d",CTSyear,CTSmonth,CTSday);
					}
					else
					{
						sprintf(CMClosureDateS,"%d/0%d/%d",CTSyear,CTSmonth,CTSday);
					}

				}
				else
				{
					if (CTSday<10)
					{
						sprintf(CMClosureDateS,"%d/%d/0%d",CTSyear,CTSmonth,CTSday);
					}
					else
					{
						sprintf(CMClosureDateS,"%d/%d/%d",CTSyear,CTSmonth,CTSday);
					}

				}
				printf("\n CMClosureDateS : %s \n",CMClosureDateS); fflush(stdout);
				strcat(AllDmlAttrS,CMClosureDateS);
			//}
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,CMClosureDateS);  
			//fprintf(DML_Download_fp,",");

			//24 Change review Board in TCUA 
			if (AOM_UIF_ask_value(dml_rev_tag,"ChangeReviewBoard",&DmlReview)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : DmlReview : %s \n",DmlReview); fflush(stdout);
			ReviewName = strtok(DmlReview,"(");
			printf("\n ReviewName : %s \n",ReviewName); fflush(stdout);
			ReviewId= strtok(NULL,")");
			printf("\n ReviewId : %s \n",ReviewId); fflush(stdout);
			if (ReviewId)               
				strcat(AllDmlAttrS,ReviewId);
			else
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");

			//25 Closure time stamp
			//TCE : 2018/02/26-12:35:43:503
			printf("\n kk.start DMLClosureStmp\n"); fflush(stdout);
			sprintf(DMLClosureStmp,"%d/%d/%d-%d:%d:00:000",CTSyear,CTSmonth,CTSday,CTShour,CTSminute);
			printf("\n kk.DMLClosureStmp: %s \n",DMLClosureStmp); fflush(stdout);
			if (DMLClosureStmp)     
				strcat(AllDmlAttrS,DMLClosureStmp);
			strcat(AllDmlAttrS,",");


			printf("\n Complete_DML_Download : AllDmlAttrS : [%s] \n",AllDmlAttrS); fflush(stdout);


			// Expand the DML-Revision using relation "T5_DMLTaskRelation" to get Task tag.
			ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation",&reln_type_tag));
			if (reln_type_tag	!=	NULLTAG)
			{
				if ( (ifail = GRM_list_secondary_objects_only( dml_rev_tag, reln_type_tag, &n_tasks, &task_obj_tag )) != ITK_ok )
				{
					return ifail;
				}			
				printf("\n Complete_DML_Download : No of Tasks are : %d \n",n_tasks); fflush(stdout);

				for (i=0;i<n_tasks ;i++ )
				{
					// Find the Task-Revision Tag.
					task_rev_tag = task_obj_tag[i];
					//ITK_CALL(ITEM_find_revision(task_obj_tag[i],"NR",&task_rev_tag)) ;
					if (task_rev_tag  != NULLTAG)
					{
						AllTaskAttrS = (  char * )MEM_alloc(600);

						AllTaskAttrS=strcpy(AllTaskAttrS,AllDmlAttrS);
						strcat(AllTaskAttrS,"#");

						//fprintf(DML_Download_fp,"#");
						// Collecting Task-Revision Attributes for Download.
						// 1 task
						if (AOM_ask_value_string(task_rev_tag,"item_id",&TaskItemIdS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev item_id : %s \n",TaskItemIdS); fflush(stdout);
						if (TaskItemIdS)               
							strcat(AllTaskAttrS,TaskItemIdS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,TaskItemIdS); 
						//fprintf(DML_Download_fp,"#");

						// 2 task desc
						ITK_CALL(AOM_UIF_ask_value(task_rev_tag,"object_desc",&TaskObjectDescS));
						//if (AOM_ask_value_string(task_rev_tag,"object_desc",&TaskObjectDescS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev TaskObjectDescS : %s \n",TaskObjectDescS); fflush(stdout);
						if (TaskObjectDescS == NULL)               
							strcat(AllTaskAttrS,TaskObjectDescS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,TaskItemIdS);
						//fprintf(DML_Download_fp,"#");

						// 3 task design grp
						if (AOM_ask_value_string(task_rev_tag,"t5designgroup",&DesignGroupS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev DesignGroupS : %s \n",DesignGroupS); fflush(stdout);
						if (DesignGroupS)               
							strcat(AllTaskAttrS,DesignGroupS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,TaskObjectDescS);
						//fprintf(DML_Download_fp,"#");

						// 4 LCS attribute not present in TCUA.
						//if (AOM_ask_value_string(task_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
						//printf("\n Complete_DML_Download : Task-Rev ???: %s \n",???); fflush(stdout);
						//if (CMClosureDateS)               
							strcat(AllTaskAttrS,"");
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,"");
						//fprintf(DML_Download_fp,"#");
						
						// 5 TaskStatus is not present in TCUA.
						//if (AOM_ask_value_string(task_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
						//printf("\n Complete_DML_Download : Task-Rev ???: %s \n",???); fflush(stdout);
						//if (CMClosureDateS)               
							strcat(AllTaskAttrS,"");
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,"");
						//fprintf(DML_Download_fp,"#");

						// 6 task disposition
						if (AOM_ask_value_string(task_rev_tag,"CMDisposition",&TaskDespositionS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev TaskDespositionS : %s \n",TaskDespositionS); fflush(stdout);
						if (TaskDespositionS)               
							strcat(AllTaskAttrS,TaskDespositionS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,TaskDespositionS);
						//fprintf(DML_Download_fp,"#");
												
						// 7 Creator is not present in TCUA.
						//if (AOM_ask_value_string(task_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
						//printf("\n Complete_DML_Download : Task-Rev ???: %s \n",???); fflush(stdout);
						//if (CMClosureDateS)               
							strcat(AllTaskAttrS,CMClosureDateS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,"");
						//fprintf(DML_Download_fp,"#");

						// 8 task requestor
						if (AOM_ask_value_string(task_rev_tag,"requestor_user_id",&RequestorS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev RequestorS : %s \n",RequestorS); fflush(stdout);
						if (RequestorS)               
							strcat(AllTaskAttrS,RequestorS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,RequestorS);
						//fprintf(DML_Download_fp,"#");

						// 9 task analyst
						if (AOM_ask_value_string(task_rev_tag,"analyst_user_id",&TaskAnalystUserIdS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev TaskAnalystUserIdS : %s \n",TaskAnalystUserIdS); fflush(stdout);
						if (TaskAnalystUserIdS)               
							strcat(AllTaskAttrS,TaskAnalystUserIdS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,TaskAnalystUserIdS);
						//fprintf(DML_Download_fp,"#");

						//10 task admin
							strcat(AllTaskAttrS,"");
						strcat(AllTaskAttrS,"#");

						AllTaskAttrDupS = strdup(AllTaskAttrS);

						printf("\n Complete_DML_Download : AllTaskAttrS : %s \n",AllTaskAttrS); fflush(stdout);
						printf("\n Complete_DML_Download : AllTaskAttrDupS : %s \n",AllTaskAttrDupS); fflush(stdout);

						// Expand the Task-Revision using CMHasSolutionItem to get the TC Part.
						reln_type_tag = NULLTAG;
						ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&reln_type_tag));
						if (reln_type_tag	!=	NULLTAG)
						{
							if ( (ifail = GRM_list_secondary_objects_only( task_rev_tag, reln_type_tag, &n_parts, &part_obj_tag )) != ITK_ok )
							{
								return ifail;
							}
							printf("\n Complete_DML_Download : No of TC Parts attached to the Task are : %d \n",n_parts); fflush(stdout);
							for (j=0;j<n_parts ;j++ )
							{
								fflush(DML_Download_fp);
								fprintf(DML_Download_fp,AllTaskAttrDupS);
								
								fprintf(DML_Download_fp,"$");
								// Collecting Part Attributes for DML Download.
								// 1
								if (AOM_ask_value_string(part_obj_tag[j],"item_id",&PartNumberS)!=ITK_ok)   PrintErrorStack();
								printf("\n Complete_DML_Download : Task-Rev PartNumberS : %s \n",PartNumberS); fflush(stdout);
								fprintf(DML_Download_fp,PartNumberS);
								fprintf(DML_Download_fp,"$");

								// 2
								if (AOM_ask_value_string(part_obj_tag[j],"item_revision_id",&RevisionS)!=ITK_ok)   PrintErrorStack();
								printf("\n Complete_DML_Download : Task-Rev RevisionS : %s \n",RevisionS); fflush(stdout);
								fprintf(DML_Download_fp,RevisionS);
								fprintf(DML_Download_fp,"$");

								// 3
								//if (AOM_ask_value_string(part_obj_tag[j],"sequence_id",&SequenceS)!=ITK_ok)   PrintErrorStack();
								if (AOM_UIF_ask_value(part_obj_tag[j],"sequence_id",&SequenceS)!=ITK_ok)   PrintErrorStack();
								printf("\n Complete_DML_Download : Task-Rev SequenceS : %s \n",SequenceS); fflush(stdout);
								fprintf(DML_Download_fp,SequenceS);
								fprintf(DML_Download_fp,"$");


								// 4 Effectivity Start Date is not present in TCUA
								//if (AOM_ask_value_string(part_obj_tag[j],"???",&???)!=ITK_ok)   PrintErrorStack();
								//printf("\n Complete_DML_Download : Task-Rev ??? : %s \n",???); fflush(stdout);
								ITK_CALL(WSOM_ask_release_status_list(part_obj_tag[j],&st_count1,&status_list1));
								printf("\n Release Status Count is :%d \n",st_count1);fflush(stdout);
								if(st_count1==1)
								{
									ITK_CALL(AOM_ask_name(status_list1[0],&WSO_Name));
									printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);
									//if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0 || tc_strcmp(WSO_Name,"ERC Released")==0)
									if (tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0)
									{
										ITK_CALL(AOM_ask_value_string(status_list1[cnt],"effectivity_text",&effectivityStr));
										printf("\n effectivityStr----><%s>\n",effectivityStr );
										if (effectivityStr!=NULL)
										{
											//STRNG_replace_str(effectivityStr," to UP (NONE)","",&effectivityStrFinal);
											effectivityStrFinal = strtok(effectivityStr," ");
											//fprintf(DML_Download_fp,effectivityStrFinal);
											DATE_string_to_date ( effectivityStrFinal , "%d-%b-%Y", &month , &day ,&year , &hour , &minute , &second);
											printf("\effectivityStrFinal----><%s> %d-%d-%d\n",effectivityStrFinal,day,month,year);
											month++;
											fprintf(DML_Download_fp,"%d/",year);
											if (month<10)
											{
												fprintf(DML_Download_fp,"0%d",month);
											}
											else
											{
												fprintf(DML_Download_fp,"%d",month);
											}

											if (day<10)
											{
												fprintf(DML_Download_fp,"/0%d",day);
											}
											else
											{
												fprintf(DML_Download_fp,"/%d",day);
											}
											//fprintf(DML_Download_fp,"%d/%d/%d",month,day,year);
											//fprintf(DML_Download_fp,"%d/%d/%d",year,month,day);
										}
										
										fprintf(DML_Download_fp,"$");
										// 5 Effectivity End Date is not present in TCUA
										//if (AOM_ask_value_string(part_obj_tag[j],"???",&???)!=ITK_ok)   PrintErrorStack();
										//printf("\n Complete_DML_Download : Task-Rev ??? : %s \n",???); fflush(stdout);
										fprintf(DML_Download_fp,"12/31/9999");
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,"Released");
										fprintf(DML_Download_fp,"$");
									}
									else if(tc_strcmp(WSO_Name,"T5_LcsAplReview")==0 || tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0)
									{
										fprintf(DML_Download_fp,"");
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,"");
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,"WIP");
										fprintf(DML_Download_fp,"$");
									}

								}
								else
								{

									printf("\n Release status count greater than 1 \n"); fflush(stdout);
									int cntSts=0,VaultStsFlagRel=0;
									for (cntSts=0;cntSts< st_count1;cntSts++ )
									{
										ITK_CALL(AOM_ask_name(status_list1[cntSts],&WSO_Name));
										printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);  //T5_LcsReview //ERC Released or T5_LcsErcRlzd// Released
										//if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0 || tc_strcmp(WSO_Name,"ERC Released")==0 )
										if (tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0)
										{
											VaultStsFlagRel=1;
											ITK_CALL(AOM_ask_value_string(status_list1[cntSts],"effectivity_text",&effectivityStr));
											printf("\n effectivityStr----><%s>\n",effectivityStr );
											if (effectivityStr!=NULL)
											{
												effectivityStrFinal = strtok(effectivityStr," ");
											}
											break;
										}
									}
									if (VaultStsFlagRel==1)
									{
										DATE_string_to_date ( effectivityStrFinal , "%d-%b-%Y", &month , &day ,&year , &hour , &minute , &second);
										printf("\effectivityStrFinal----><%s> %d-%d-%d\n",effectivityStrFinal,day,month,year);
										month++;
										//fprintf(DML_Download_fp,"%d/%d/%d",month,day,year);
										//fprintf(DML_Download_fp,"%d/%d/%d",year,month,day);
										fprintf(DML_Download_fp,"%d/",year);
										if (month<10)
										{
											fprintf(DML_Download_fp,"0%d",month);
										}
										else
										{
											fprintf(DML_Download_fp,"%d",month);
										}

										if (day<10)
										{
											fprintf(DML_Download_fp,"/0%d",day);
										}
										else
										{
											fprintf(DML_Download_fp,"/%d",day);
										}
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,"12/31/9999");
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,"Released");
										fprintf(DML_Download_fp,"$");
									}
									else
									{
										fprintf(DML_Download_fp,"");
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,"");
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,"");
										fprintf(DML_Download_fp,"$");
									}
								}
								if ( AOM_ask_value_string(part_obj_tag[j],"t5_CarMakeBuyIndicator",&t5PunMakeBuyIndS));//Change PNR make buy
								fprintf(DML_Download_fp,t5PunMakeBuyIndS);
								fprintf(DML_Download_fp,"$");
								if ( AOM_ask_value_string(part_obj_tag[j],"t5_CarIntialAgency",&t5PunIntialAgS));//change PNR Initial Agency
								fprintf(DML_Download_fp,t5PunIntialAgS);
								fprintf(DML_Download_fp,"$");
								if ( AOM_ask_value_string(part_obj_tag[j],"t5_CarStoreLocation",&t5PunStoreLocationS));//Store Location
								fprintf(DML_Download_fp,t5PunStoreLocationS);
								fprintf(DML_Download_fp,"$");
								fprintf(DML_Download_fp,"\n");
							}
						}
					}
				}
			}
			fflush(DML_Download_fp);
		}
		else
		{
			printf("\n XXXX \n");  fflush(stdout);
		}

	}
	if (flag == 0)
	{
		printf("\n Complete_DML_Download : DML Not Found \n ");  fflush(stdout);		
	}
	fclose(DML_Download_fp);


}
;

extern int ITK_user_main (int argc, char ** argv )
{
    int     status;
	char    *req_item,*FileDown,*FileUses = NULL,*PartNumberS=NULL,*DMLtype=NULL;
	char    *FileDown2,*FileUses2 = NULL,*File_LhRh,*File_Weld,*FilePath,*File_Pro,*File_Dml,*DepthInputStr;
	char    rev_id[ITEM_id_size_c+1];
	tag_t   window, window2, rule, item_tag = NULLTAG, top_line;
	FILE	*fd,*fu,*fLhRh,*fAttr,*fpro;
	int i=0,j=0,n_tasks=0,n_parts=0,DepthInput=0;
	int		ifail				= ITK_ok;
	tag_t dml_tag= NULLTAG;
	char* item_type= NULL;
	tag_t dml_rev_tag= NULLTAG;
	tag_t reln_type_tag= NULLTAG;
	tag_t *task_obj_tag= NULLTAG;
	tag_t task_rev_tag= NULLTAG;
	tag_t *part_obj_tag= NULLTAG;

    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_auto_login( ));
    ITK_CALL(ITK_set_journalling( TRUE ));

    req_item = ITK_ask_cli_argument("-i=");
	FileDown2 = ITK_ask_cli_argument("-f1=");
	FileUses2 = ITK_ask_cli_argument("-f2=");
	FilePath = ITK_ask_cli_argument("-Path=");
	DepthInputStr = ITK_ask_cli_argument("-Depth=");

	FileDown=(char *) MEM_alloc(200 * sizeof(char ));
	FileUses=(char *) MEM_alloc(200 * sizeof(char ));
	File_LhRh=(char *) MEM_alloc(200 * sizeof(char ));
	File_Weld=(char *) MEM_alloc(200 * sizeof(char ));
	File_Pro=(char *) MEM_alloc(200 * sizeof(char ));
	File_Dml=(char *) MEM_alloc(200 * sizeof(char ));

	//strcpy(FileDown,"/home/trlprod/TCUA/Downloader/");
	//strcpy(FileDown,"/home/ua83prod/TCUA_Download/downloader/");
	//strcpy(FileDown,"/home/trlprod/TCUA/Downloader/");
	printf("\nInside the downloader %s\n",req_item);
	strcpy(FileDown,FilePath);
	strcat(FileDown,req_item);
	strcat(FileDown,"/");
	strcat(FileDown,FileDown2);
	printf("\n part file is %s \n",FileDown);
	//strcpy(FileUses,"/home/trlprod/TCUA/Downloader/");
	//strcpy(FileUses,"/home/ua83prod/TCUA_Download/downloader/");
	//strcpy(FileUses,"/home/trlprod/TCUA/Downloader/");
	strcpy(FileUses,FilePath);
	strcat(FileUses,req_item);
	strcat(FileUses,"/");
	strcat(FileUses,FileUses2);
	printf("\n Uses part file is %s \n",FileUses);

	strcpy(File_LhRh,FilePath);
	strcat(File_LhRh,req_item);
	strcat(File_LhRh,"/");
	strcat(File_LhRh,req_item);
	strcat(File_LhRh,"_LhRhInp.txt");
	printf("\n LH RH part file is %s \n",File_LhRh);

	strcpy(File_Weld,FilePath);
	strcat(File_Weld,req_item);
	strcat(File_Weld,"/");
	strcat(File_Weld,req_item);
	strcat(File_Weld,"_ATTR.txt");
	printf("\n Weld part file is %s \n",File_Weld);

	strcpy(File_Pro,FilePath);
	strcat(File_Pro,req_item);
	strcat(File_Pro,"/");
	strcat(File_Pro,req_item);
	strcat(File_Pro,"_ProAsm.txt");

	strcpy(File_Dml,FilePath);
	strcat(File_Dml,req_item);
	strcat(File_Dml,"/");
	strcat(File_Dml,req_item);
	strcat(File_Dml,"_Dml.txt");

	
	DepthInput=atoi(DepthInputStr);
	printf("\nDepthInput is %d \n",DepthInput);
	fd=fopen(FileDown,"w");
    if(fd==NULL)
	{
		printf("\nError in opening the file \n");
	}

	fu=fopen(FileUses,"w");
    if(fu==NULL)
	{
		printf("\nError in opening the file \n");
	}

	fLhRh=fopen(File_LhRh,"w");
    if(fLhRh==NULL)
	{
		printf("\nError in opening the file \n");
	}

	fAttr=fopen(File_Weld,"w");
    if(fAttr==NULL)
	{
		printf("\nError in opening the file \n");
	}

	fpro=fopen(File_Pro,"w");
    if(fpro==NULL)
	{
		printf("\nError in opening the file \n");
	}

	

	if ( req_item )
    {
		//Complete_DML_Download(req_item,File_Dml);

		ITK_CALL(ITEM_find_item( req_item, &dml_tag ));

		if (dml_tag != NULLTAG)
		{
			printf("\n\n Main Item found ! ! ! \n\n");  fflush(stdout);
			
			ITK_CALL(ITEM_ask_type2 (dml_tag, &item_type));
			printf("\n Main item_typeS [%s] \n",item_type);  fflush(stdout);
			   
			// Find the DML-Revision tag. 
			//if (tc_strcmp(item_type,"ChangeRequest")==0)
			if (tc_strcmp(item_type,"T5_APLDML")==0)
			{
				printf("\n\n Inside DML ! ! ! \n\n");  fflush(stdout);
				Complete_DML_Download(req_item,File_Dml);
				//ITK_CALL(ITEM_find_revision(dml_tag,"A",&dml_rev_tag)) ;
				ITK_CALL(ITEM_ask_latest_rev(dml_tag,&dml_rev_tag)) ;
				if (dml_rev_tag  != NULLTAG)
				{
					ITK_CALL(AOM_ask_value_string(dml_rev_tag,"t5_rlstype",&DMLtype));
					printf("\n Complete_DML_Download : object_type : %s \n",DMLtype); fflush(stdout);

					if (tc_strcmp(DMLtype,"Veh")==0)
					{
						DMLFlag=1;  // If veh  DML falg = 1
					}

					ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation",&reln_type_tag));
					if (reln_type_tag	!=	NULLTAG)
					{
						if ( (ifail = GRM_list_secondary_objects_only( dml_rev_tag, reln_type_tag, &n_tasks, &task_obj_tag )) != ITK_ok )
						{
									return ifail;
						}
						//ITK_CALL(GRM_list_secondary_objects_only( dml_rev_tag, reln_type_tag, &n_tasks, &task_obj_tag ));
			
						printf("\n  Main : No of Tasks are : %d \n",n_tasks); fflush(stdout);

						for (i=0;i<n_tasks ;i++ )
						{
							task_rev_tag = task_obj_tag[i];
							if (task_rev_tag  != NULLTAG)
							{
								ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&reln_type_tag));
								if (reln_type_tag	!=	NULLTAG)
								{
									ITK_CALL(GRM_list_secondary_objects_only( task_rev_tag, reln_type_tag, &n_parts, &part_obj_tag ));

									printf("\n Main No of TC Parts attached to the Task are : %d \n",n_parts); fflush(stdout);
									for (j=0;j<n_parts ;j++ )
									{
										
										ITK_CALL(AOM_ask_value_string(part_obj_tag[j],"item_id",&PartNumberS));
										printf("\n Main Part number: %s \n",PartNumberS); fflush(stdout);

										//get_revision(PartNumberS); // Packing the BOmline
										//continue;
										if (DMLFlag!=1)
										{
											stamp_find_no(part_obj_tag[j]);
										}
										//Code is commented, Design Revision is attached with Task, so no need to query the part again
//										tag_t *tags_found = NULL;
//										int n_tags_found= 0;
//										char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
//										char **values = (char **) MEM_alloc(1 * sizeof(char *));
//									
//										attrs[0] ="item_id";
//										values[0] = (char *)PartNumberS;
//										
//										ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
//										MEM_free(attrs);
//										MEM_free(values);
//										if (n_tags_found == 0)
//										{
//											printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", PartNumberS);
//											exit (0);
//										}
//										else if (n_tags_found > 1)
//										{
//											MEM_free(tags_found);
//											EMH_store_initial_error(EMH_severity_error,ITEM_multiple_items_returned);
//											printf ( "More than one items matched with id %s\n", PartNumberS);
//											exit (0);
//										}
										//item_tag = tags_found[0];
										item_tag = part_obj_tag[j];
										//MEM_free(tags_found);
										ITK_CALL(BOM_create_window (&window));
										//ITK_CALL(CFM_find( "Latest by Creation Date", &rule ));
										//ITK_CALL(CFM_find( "ERC release and above", &rule ));
										ITK_CALL(CFM_find( "APLC Release and above", &rule ));
										//ITK_CALL(CFM_find( "Latest Released", &rule ));
										ITK_CALL(BOM_set_window_config_rule( window, rule ));
										if (DMLFlag==1)
										{
											ITK_CALL(BOM_set_window_pack_all (window, false));
										}
										else
										{
											ITK_CALL(BOM_set_window_pack_all (window, true));
										}
										//ITK_CALL(BOM_set_window_top_line (window, item_tag, null_tag, null_tag, &top_line));
										ITK_CALL(BOM_set_window_top_line (window, null_tag, item_tag, null_tag, &top_line));
										ITK_CALL( BOM_window_show_suppressed ( window ));
										print_bom (top_line,NULLTAG, 0,fd,fu,fLhRh,fAttr,fpro,req_item,FilePath,DepthInput);
									}
								}
							}
						}
					}
				}
			}
			else if(tc_strcmp(item_type,"Design")==0)
			{
				get_revision(req_item); // Packing the Bomline
				printf("\n\n Inside Design ! ! ! \n\n");  fflush(stdout);
					tag_t *tags_found = NULL;
					int n_tags_found= 0;
					char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
					char **values = (char **) MEM_alloc(1 * sizeof(char *));
				
					attrs[0] ="item_id";
					values[0] = (char *)req_item;
					
					ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
					MEM_free(attrs);
					MEM_free(values);
					if (n_tags_found == 0)
					{
						printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", req_item);
						exit (0);
					}
					else if (n_tags_found > 1)
					{
						MEM_free(tags_found);
						EMH_store_initial_error(EMH_severity_error,ITEM_multiple_items_returned);
						printf ( "More than one items matched with id %s\n", req_item);
						exit (0);
					}
					item_tag = tags_found[0];
					MEM_free(tags_found);
					ITK_CALL(BOM_create_window (&window));
					//ITK_CALL(CFM_find( "APLC Release and above", &rule ));
					ITK_CALL(CFM_find( "ERC release and above", &rule ));
					//ITK_CALL(CFM_find( "Latest by Creation Date", &rule ));
					//ITK_CALL(CFM_find( "Latest Released", &rule ));
					ITK_CALL(BOM_set_window_config_rule( window, rule ));
					ITK_CALL(BOM_set_window_pack_all (window, true));
					ITK_CALL(BOM_set_window_top_line (window, item_tag, null_tag, null_tag, &top_line));
					//ITK_CALL(BOM_set_window_top_line (window, null_tag, item_tag, null_tag, &top_line));
					ITK_CALL( BOM_window_show_suppressed ( window ));
					print_bom (top_line,NULLTAG, 0,fd,fu,fLhRh,fAttr,fpro,req_item,FilePath,DepthInput);
			}
		}

		fclose(fd);
		fclose(fu);
		fclose(fLhRh);
		fclose(fAttr);
		fclose(fpro);
	}

	printf("\n MAIN FUNCTION : : FINISH\n");
	return status;
}
static void print_ATTR (tag_t reva,FILE *fpAssy)
 {
	int ifail;
	char *name, *sequence_no, *quantity;
	tag_t item_tags;
	char *Item_id_par=NULL;
	char *Item_id_seq=NULL;
	char *Item_id_rev=NULL;
	tag_t *children;
	tag_t refjtname;
	int iChildItemTag;
	tag_t jtfile;
	int status;
	int cnt=0;
	int cnt1=0;
	int ii=0;
	int referencenumberfound=0;
	char *Item_id_par_des=NULL;
	char *vol_name=NULL;
	char *nod_name=NULL;
	char *word=NULL;
	char *refset_name=NULL;
	int i, n,j;
	int k = 0;
	logical  override;
	logical  override1 = false;
	tag_t item=NULLTAG;
	//tag_t reva=NULLTAG;
	tag_t vol=NULLTAG;
	tag_t *tags_found = NULL;
	tag_t   t_ChildItemRev;

	char* ModDescriptionDupS=NULL;
	char* mod_desc=NULL;
	char* DrawingNoDupS=NULL;
	char* Materialclass=NULL;
	char* DrawingIndDupS=NULL;
	char* DrawingIndS=NULL;
	char* MaterialDupS=NULL;
	char* MaterialS=NULL;
	char* MaterialInDrwDupS=NULL;
	char* MaterialInDrwS=NULL;
	char* LeftRhDupS=NULL;
	char* LeftRhS=NULL;
	char* DeignOwnUnitDupS=NULL;
	char* DeignOwnUnitS=NULL;
	char* ModelIndDupS=NULL;
	char* ModelIndS=NULL;
	char* UnitOfMeasureDupS=NULL;
	char* UnitOfMeasureS=NULL;
	char* ColourIndDupS=NULL;
	char* ColourIndS=NULL;
	char* WeightDupS=NULL;
	char* WeightS=NULL;
	char* MaterialThickNessS=NULL;
	char* DrawingNoS=NULL;
	char* projectcode=NULL;
	char* designgroup=NULL;
	char * rel_list=NULL;
	char * lifecycleS=NULL;
	char * desc=NULL;
	char * partType=NULL;
	double dweight;
	char * t5SpareIndS=NULL;
	char * t5SpareCriteriaS=NULL;
	char * t5ReliabilityS=NULL;
	char * t5RefPartNumberS=NULL;
	char * t5RecyclabilityS=NULL;
	char * t5RecoverableS=NULL;
	char * t5PunMakeBuyIndS=NULL;
	char * t5PunIntialAgS=NULL;
	char * t5PrtCatCodeS=NULL;
	char * t5ProductS=NULL;
	char * t5PkgStdS=NULL;
	char * t5PartStatusS=NULL;
	char * t5PartPropertyS=NULL;
	char * t5PartCodeS=NULL;
	char * t5NcPartNoS=NULL;
	char * t5ListRecSparess=NULL;
	char * t5HomologationReqdS=NULL;
	char * t5HazardousContS=NULL;
	char * t5EnvelopeDimenS=NULL;
	char * t5DismantableS=NULL;
	char * t5ConfigIDS=NULL;
	char * t5ColourIDS=NULL;
	char * t5ClassificationHazS=NULL;
	char * t5CMVRCertificationS=NULL;
	char * t5SurfPrtStdS=NULL;
	char * t5SamplesToApprS=NULL;
	char * t5AsmDisposalS=NULL;
	char * t5FinDisposalInstrS=NULL;
	char * t5RPDisposalInstrS=NULL;
	char * t5SPDisposalInstrS=NULL;
	char * t5WIPDisposalInstrS=NULL;
	char * t5LastModByS=NULL;
	char * t5VerCreatorS=NULL;
	char * t5CAEDocES=NULL;
	char * t5CoatedS=NULL;
	char * t5ConvDocS=NULL;
	char * t5AplCopyOfErcRevS=NULL;
	char * t5AppCodeS=NULL;
	char * t5RqstNumS=NULL;
	char * t5RsnCodeS=NULL;
	char * t5SurfaceAreaS=NULL;
	char * t5VolumeS=NULL;
	char * t5CarIntialAgencyS=NULL;
	char * t5CarMakeBuyIndiS=NULL;
	char * t5ErcIndNameS=NULL;
	char * t5PostRelReqS=NULL;
	char * t5ItmCategoryS=NULL;
	char * t5CopReqS=NULL;
	char * t5AplInvalidateS=NULL;
	char * t5PrtValiStatusS=NULL;
	char * t5DRSubStateS=NULL;
	char * t5KnxtDocIndS=NULL;
	char * t5SimValS=NULL;
	char * t5PerYieldS=NULL;
	char * t5PunStoreLocationS=NULL;
	char * t5AltPartNoS=NULL;
	char * t5RolledupWtS=NULL;
	char * t5PFDModReqdS=NULL;
	char * t5ToolIndentReqdS=NULL;
	char * CategoryNameS=NULL;
	char * t5DsgnDeptS=NULL;
	char * t5AplCopyOfErcSeqS=NULL;
	char *FromDate;
	char *Replacedmod_desc;
	char *ReplacedMaterialInDrwS;
	char *ToDate;
	char *CoatedInd=NULL;
	char *CompCode=NULL;
	int	   status_count=0;
	tag_t* status_list = NULLTAG;
	int si;
	int lat_seq_id_int;
	int lat_seq_id_int_1;
	char *itemRevSeq = NULL;
	int desclength = 0,revdesccnt=0;
	int desclength1 = 0,revdesccnt1=0;
	int desccnt = 0,n_entries=2;
	char   *ReleaseStatus=NULL;
	char   *PrtTypeDup=NULL;
	//done
	int n_tags_found= 0;
	char RelStat[200] = { 0 };
	char RelStatfile[20] = { 0 };
	tag_t                   *attachments  = NULLTAG;
	tag_t                   dataset        = NULLTAG;
	tag_t                   refobject      = NULLTAG;
	tag_t relation_type, relation;
	tag_t *         related_occs = NULLTAG;
	tag_t *         related_items =  NULLTAG;
	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));
	char   refname[AE_reference_size_c + 1];
	char   pathname[SS_MAXPATHLEN + 1];
	char   pathnamee[SS_MAXPATHLEN + 1];
	char   orig_name[IMF_filename_size_c + 1];
	AE_reference_type_t     reftype;

	//Changes Hemal
	int		st_count1				= 0;

	int      month      = 0;          /* Month */
    int      day        = 0;          /* Day */
    int      year       = 0;          /* Year */
    int      hour       = 0;          /* Hour */
    int      minute     = 0;          /* Minutes */
    int      second     = 0;          /* Seconds */
	int      CTSmonth   = 0;          /* Month */
    int      CTSday     = 0;          /* Day */
    int      CTSyear    = 0;          /* Year */
    int      CTShour    = 0;          /* Hour */
    int      CTSminute  = 0;          /* Minutes */
    int      CTSsecond  = 0;          /* Seconds */

	char	*PartNumberS		=	NULL;
	char	*RevisionS			=	NULL;
	char	*SequenceS			=	NULL;
	char	*WSO_Name			=	NULL;
	char	*effectivityStr		=	NULL;
	char	*effectivityStrFinal=	NULL;
	
	tag_t	*status_list1		=	NULLTAG;
	//Changes Ends Hemal

	PrtTypeDup = ( char * ) MEM_alloc(10);
	effectivityStrFinal = ( char * ) MEM_alloc(30);

   if( AOM_ask_value_string(reva,"item_id",&Item_id_par)!=ITK_ok);  
   //if( AOM_ask_value_string(reva,"sequence_id",&Item_id_rev)!=ITK_ok);
   if( AOM_ask_value_string(reva,"item_revision_id",&Item_id_seq)!=ITK_ok);
   if( AOM_ask_value_string(reva,"t5_ProjectCode",&projectcode));
   if( AOM_ask_value_string(reva,"t5_PartType",&partType));


   	if (!tc_strcmp(partType,"DC") || !tc_strcmp(partType,"DA") || !tc_strcmp(partType,"IFD") || !tc_strcmp(partType,"CM"))
	{
		
		strcpy(PrtTypeDup,"D");
	}
	else 
	{
		strcpy(PrtTypeDup,partType);
	}

	// if ( AOM_set_value_int(reva,"sequence_id",&sequence_id));
	if ( AOM_ask_value_string(reva,"t5_DesignGrp",&designgroup));
	if ( AOM_ask_value_string(reva,"object_desc",&desc));
	if ( AOM_ask_value_string(reva,"t5_DocRemarks",&mod_desc));
	if ( AOM_ask_value_string(reva,"t5_DrawingNo",&DrawingNoS));
	if ( AOM_ask_value_string(reva,"t5_DrawingInd",&DrawingIndS));
	
	

	if ( AOM_ask_value_string(reva,"t5_CategoryName",&CategoryNameS)); //Keyword desc
	if ( AOM_ask_value_string(reva,"t5_Material",&MaterialInDrwS));
	if ( AOM_ask_value_string(reva,"t5_MThickness",&MaterialThickNessS));
	if ( AOM_ask_value_string(reva,"t5_DsgnOwn",&DeignOwnUnitS));
	if ( AOM_ask_value_string(reva,"t5_ColourInd",&ColourIndS));
	if ( AOM_ask_value_string(reva,"t5_Coated",&CoatedInd));
	
	if ( AOM_ask_value_string(reva,"t5_PrtCatCode",&CompCode));
	//if ( AOM_ask_value_string(reva,"t5_Weight",&dweight));
	if ( AOM_ask_value_double(reva,"t5_Weight",&dweight));
	if ( AOM_ask_value_string(reva,"t5_MatlClass",&Materialclass));
	if ( AOM_ask_value_string(reva,"t5_EnvelopeDimensions",&t5EnvelopeDimenS));
	if ( AOM_ask_value_string(reva,"t5_RolledupWt",&t5RolledupWtS));//create it


	char   *mcreator= NULL;
	char   *mcreatorName= NULL;
	char   *VCreator= NULL;
	char   *VCreatorName= NULL;
	char   *VerCreator= NULL;
	char   *Creator= NULL;

	mcreator = (char *) MEM_alloc( 200 * sizeof(char) );
	mcreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	VCreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	VerCreator = (char *) MEM_alloc( 200 * sizeof(char) );
	
		
	ITK_CALL(AOM_UIF_ask_value(reva,"owning_user",&Creator)); //Creator
	ITK_CALL(AOM_ask_value_string(reva,"t5_VerCreator",&VCreator));  //Version Creator

	mcreatorName = strtok(Creator,"(");
	mcreator = strtok(NULL,")");
	VCreatorName = strtok(VCreator,"(");
	VerCreator = strtok(NULL,")");





	//if ( AOM_ask_value_string(reva,"t5_RefPartNumber",&t5RefPartNumberS));
	if ( AOM_ask_value_string(reva,"t5_CarMakeBuyIndicator",&t5PunMakeBuyIndS));//Change PNR make buy
	if ( AOM_ask_value_string(reva,"t5_CarIntialAgency",&t5PunIntialAgS));//change PNR Initial Agency
	if ( AOM_ask_value_string(reva,"t5_CarStoreLocation",&t5PunStoreLocationS));//Store Location
	//if ( AOM_ask_value_string(reva,"t5_PrtCatCode",&t5PrtCatCodeS));
	//if ( AOM_ask_value_string(reva,"t5_Product",&t5ProductS));
	//if ( AOM_ask_value_string(reva,"t5_PkgStd",&t5PkgStdS));
	//if ( AOM_ask_value_string(reva,"t5_PartStatus",&t5PartStatusS));//Create it
	//if ( AOM_ask_value_string(reva,"t5_PartProperty",&t5PartPropertyS));
	//if ( AOM_ask_value_string(reva,"t5_PartCode",&t5PartCodeS));
	//if ( AOM_ask_value_string(reva,"t5_NcPartNo",&t5NcPartNoS));
	//if ( AOM_ask_value_string(reva,"t5_HomologationReqd",&t5HomologationReqdS));
	//
	//if ( AOM_ask_value_string(reva,"t5_ConfigID",&t5ConfigIDS));//change its name right now it is ConfigID
	//if ( AOM_ask_value_string(reva,"t5_ColourID",&t5ColourIDS));
	//if ( AOM_ask_value_string(reva,"t5_SurfPrtStd",&t5SurfPrtStdS));
	//if ( AOM_ask_value_string(reva,"t5_AsmDisposalInstr",&t5AsmDisposalS));
	//if ( AOM_ask_value_string(reva,"t5_FinDisposalInstr",&t5FinDisposalInstrS));
	//if ( AOM_ask_value_string(reva,"t5_RPDisposalInstr",&t5RPDisposalInstrS));
	//if ( AOM_ask_value_string(reva,"t5_SPDisposalInstr",&t5SPDisposalInstrS));
	//if ( AOM_ask_value_string(reva,"t5_WIPDisposalInstr",&t5WIPDisposalInstrS));
	//if ( AOM_ask_value_string(reva,"t5_VerCreator",&t5VerCreatorS));//Create it
	//if ( AOM_ask_value_string(reva,"t5_Coated",&t5CoatedS));
	//if ( AOM_ask_value_string(reva,"t5_AplCopyOfErcRev",&t5AplCopyOfErcRevS));
	//if ( AOM_ask_value_string(reva,"t5_Volume",&t5VolumeS));
	//if ( AOM_ask_value_string(reva,"t5_SurfaceArea",&t5SurfaceAreaS));
	//if ( AOM_ask_value_string(reva,"t5_RqstNum",&t5RqstNumS));
	//if ( AOM_ask_value_string(reva,"t5_RsnCode",&t5RsnCodeS));
	//if ( AOM_ask_value_string(reva,"t5_CarIntialAgency",&t5CarIntialAgencyS));
	//if ( AOM_ask_value_string(reva,"t5_CarMakeBuyIndicator",&t5CarMakeBuyIndiS));
	//if ( AOM_ask_value_string(reva,"t5_ErcIndName",&t5ErcIndNameS));
	//if ( AOM_ask_value_string(reva,"t5_ItmCategory",&t5ItmCategoryS));
	//if ( AOM_ask_value_string(reva,"t5_AplInvalidate",&t5AplInvalidateS));//create it
	//if ( AOM_ask_value_string(reva,"t5_PrtValiStatus",&t5PrtValiStatusS));//create it
	//if ( AOM_ask_value_string(reva,"t5_DRSubState",&t5DRSubStateS));//create it
	//if ( AOM_ask_value_string(reva,"t5_KnxtDocInd",&t5KnxtDocIndS));//create it
	//if ( AOM_ask_value_string(reva,"t5_SimVal",&t5SimValS));//create it
	//if ( AOM_ask_value_string(reva,"t5_PerYield",&t5PerYieldS));//create it
	//if ( AOM_ask_value_string(reva,"t5_PunStoreLocation",&t5PunStoreLocationS));//create it
	//if ( AOM_ask_value_string(reva,"t5_AltPartNo",&t5AltPartNoS));//create it
	//
	//if ( AOM_ask_value_string(reva,"t5_EstSheetReqd",&t5RolledupWtS));//create it
	//if ( AOM_ask_value_string(reva,"t5_PFDModReqd",&t5PFDModReqdS));//create it
	//if ( AOM_ask_value_string(reva,"t5_ToolIndentReqd",&t5ToolIndentReqdS));//create it
	//  
	//  // Logical Properties are being populated below
	//if ( AOM_UIF_ask_value(reva,"t5_CAEDocE",&t5CAEDocES));
	//if ( AOM_UIF_ask_value(reva,"t5_CMVRCertificationReqd",&t5CMVRCertificationS));
	//if ( AOM_UIF_ask_value(reva,"t5_ConvDoc",&t5ConvDocS));
	//if ( AOM_UIF_ask_value(reva,"t5_CopReq",&t5CopReqS));
	//if ( AOM_UIF_ask_value(reva,"t5_Dismantable",&t5DismantableS));
	//if ( AOM_UIF_ask_value(reva,"t5_HazardousContents",&t5HazardousContS));
	//if ( AOM_UIF_ask_value(reva,"t5_ListRecSpares",&t5ListRecSparess));
	//if ( AOM_UIF_ask_value(reva,"t5_PostRelReq",&t5PostRelReqS));
	//if ( AOM_UIF_ask_value(reva,"t5_Recoverable",&t5RecoverableS));
	//if ( AOM_UIF_ask_value(reva,"t5_Recyclability",&t5RecyclabilityS));
	//if ( AOM_UIF_ask_value(reva,"t5_SamplesToAppr",&t5SamplesToApprS));
	//if ( AOM_UIF_ask_value(reva,"t5_SpareInd",&t5SpareIndS));
	if (WSOM_ask_release_status_list(reva,&status_count,&status_list));
	tc_strcpy(RelStat,"");
	printf("\n number of statuses: %d \n",  status_count);fflush(stdout);
	for (ii = 0; ii < status_count; ii++)
	{
		AOM_ask_name(status_list[ii], &ReleaseStatus);
		printf("\t ReleaseStatus: %s\n", ReleaseStatus);fflush(stdout);
		tc_strcat(RelStat,"[");
		tc_strcat(RelStat,ReleaseStatus);
		tc_strcat(RelStat,"]");
		tc_strcat(RelStat,",");
		
	}

	if(tc_strstr(RelStat,"T5_LcsAplRlzd")!=NULL)
	  {
				 tc_strcpy(RelStatfile,"");
				 tc_strcat(RelStatfile,"Released");
	  }
	  else
	  {
				 tc_strcpy(RelStatfile,"");
				 tc_strcat(RelStatfile,"WIP");
	  }

	printf("\n  Item ID is  ---->%s\n",Item_id_par);

	// for removing special chars

	if(tc_strcmp(mod_desc,"NULL")!=0)
	{
		desclength = tc_strlen(mod_desc);
		for(revdesccnt=0;revdesccnt<desclength;revdesccnt++)
		{
			if(mod_desc[revdesccnt]=='\n')
				mod_desc[revdesccnt] =' ';

			if(mod_desc[revdesccnt]=='`')
				mod_desc[revdesccnt] =' ';

			if(mod_desc[revdesccnt]=='~')
				mod_desc[revdesccnt] =' ';

			if(mod_desc[revdesccnt]=='|')
				mod_desc[revdesccnt] =' ';

			if(mod_desc[revdesccnt]=='$')
				mod_desc[revdesccnt] =' ';

			if(mod_desc[revdesccnt]=='#')
				mod_desc[revdesccnt] =' ';

			if(mod_desc[revdesccnt]=='@')
				mod_desc[revdesccnt] =' ';

			if(mod_desc[revdesccnt]=='^')
				mod_desc[revdesccnt] =' ';

			if(mod_desc[revdesccnt]=='!')
				mod_desc[revdesccnt] =' ';
			printf("\n [%c]\n",mod_desc[revdesccnt]);fflush(stdout);
		}
		Replacedmod_desc = (char *)MEM_alloc (desclength * sizeof(char));
		tc_strcpy(Replacedmod_desc,"");
		tc_strcat(Replacedmod_desc,mod_desc);
	}


	if(tc_strcmp(MaterialInDrwS,"NULL")!=0)
	{
		desclength1 = tc_strlen(MaterialInDrwS);
		for(revdesccnt1=0;revdesccnt1<desclength1;revdesccnt1++)
		{
			if(MaterialInDrwS[revdesccnt1]=='\n')
				MaterialInDrwS[revdesccnt1] =' ';

			if(MaterialInDrwS[revdesccnt1]=='`')
				MaterialInDrwS[revdesccnt1] =' ';

			if(MaterialInDrwS[revdesccnt1]=='~')
				MaterialInDrwS[revdesccnt1] =' ';

			if(MaterialInDrwS[revdesccnt1]=='|')
				MaterialInDrwS[revdesccnt1] =' ';

			if(MaterialInDrwS[revdesccnt1]=='$')
				MaterialInDrwS[revdesccnt1] =' ';

			if(MaterialInDrwS[revdesccnt1]=='#')
				MaterialInDrwS[revdesccnt1] =' ';

			if(MaterialInDrwS[revdesccnt1]=='@')
				MaterialInDrwS[revdesccnt1] =' ';

			if(MaterialInDrwS[revdesccnt1]=='^')
				MaterialInDrwS[revdesccnt1] =' ';

			if(MaterialInDrwS[revdesccnt1]=='!')
				MaterialInDrwS[revdesccnt1] =' ';
			printf("\n [%c]\n",MaterialInDrwS[revdesccnt1]);fflush(stdout);
		}
		ReplacedMaterialInDrwS = (char *)MEM_alloc (desclength1 * sizeof(char));
		tc_strcpy(ReplacedMaterialInDrwS,"");
		tc_strcat(ReplacedMaterialInDrwS,MaterialInDrwS);
	}
	fprintf (fpAssy,"%s^%s^%s^%s^%s^%s^%s^%s^%s^%s^%s^%s^%s^%s^%s^",Item_id_par,Item_id_seq,projectcode,PrtTypeDup,designgroup,desc,Replacedmod_desc,DrawingNoS,DrawingIndS,CategoryNameS,MaterialInDrwS,MaterialThickNessS,DeignOwnUnitS,ColourIndS,CoatedInd);
	//fprintf (fpAssy,"%s^%.3f^%s^%s^%s^%s^%s^%s^%s^%s^\n",CompCode,dweight,Materialclass,t5EnvelopeDimenS,t5RolledupWtS,mcreator,VerCreator,t5PunMakeBuyIndS,t5PunIntialAgS,t5PunStoreLocationS);
	fprintf (fpAssy,"%s^%.3f^%s^%s^%s^%s^%s^%s^%s^%s^",CompCode,dweight,Materialclass,t5EnvelopeDimenS,t5RolledupWtS,mcreator,VerCreator,t5PunMakeBuyIndS,t5PunIntialAgS,t5PunStoreLocationS);

	fprintf(fpAssy,"$");
	if (AOM_ask_value_string(reva,"item_id",&PartNumberS)!=ITK_ok)   PrintErrorStack();
	fprintf(fpAssy,PartNumberS);
	fprintf(fpAssy,"$");
	if (AOM_ask_value_string(reva,"item_revision_id",&RevisionS)!=ITK_ok)   PrintErrorStack();
	fprintf(fpAssy,RevisionS);
	fprintf(fpAssy,"$");
	if (AOM_UIF_ask_value(reva,"sequence_id",&SequenceS)!=ITK_ok)   PrintErrorStack();
	printf("\n Complete_DML_Download : Task-Rev SequenceS : %s \n",SequenceS); fflush(stdout);
	fprintf(fpAssy,SequenceS);
	fprintf(fpAssy,"$");
	
	// 4 Effectivity Start Date is not present in TCUA
	ITK_CALL(WSOM_ask_release_status_list(reva,&st_count1,&status_list1));
	printf("\n Release Status Count is :%d \n",st_count1);fflush(stdout);
	if(st_count1==1)
	{
			ITK_CALL(AOM_ask_name(status_list1[0],&WSO_Name));
			printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);
			//if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0 || tc_strcmp(WSO_Name,"ERC Released")==0)
			if (tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0)
			{
				ITK_CALL(AOM_ask_value_string(status_list1[cnt],"effectivity_text",&effectivityStr));
				printf("\n effectivityStr----><%s>\n",effectivityStr );
				if (effectivityStr!=NULL)
				{
					//STRNG_replace_str(effectivityStr," to UP (NONE)","",&effectivityStrFinal);
					effectivityStrFinal = strtok(effectivityStr," ");
					//fprintf(DML_Download_fp,effectivityStrFinal);
					DATE_string_to_date ( effectivityStrFinal , "%d-%b-%Y", &month , &day ,&year , &hour , &minute , &second);
					printf("\effectivityStrFinal----><%s> %d-%d-%d\n",effectivityStrFinal,day,month,year);
					month++;
					fprintf(fpAssy,"%d/",year);
					if (month<10)
					{
						fprintf(fpAssy,"0%d",month);
					}
					else
					{
						fprintf(fpAssy,"%d",month);
					}

					if (day<10)
					{
						fprintf(fpAssy,"/0%d",day);
					}
					else
					{
						fprintf(fpAssy,"/%d",day);
					}
					//fprintf(fpAssy,"%d/%d/%d",month,day,year);
					//fprintf(fpAssy,"%d/%d/%d",year,month,day);
				}
				
				fprintf(fpAssy,"$");
				// 5 Effectivity End Date is not present in TCUA
				//if (AOM_ask_value_string(part_obj_tag[j],"???",&???)!=ITK_ok)   PrintErrorStack();
				//printf("\n Complete_DML_Download : Task-Rev ??? : %s \n",???); fflush(stdout);
				fprintf(fpAssy,"12/31/9999");
				fprintf(fpAssy,"$");
				fprintf(fpAssy,"Released");
				fprintf(fpAssy,"$");
			}
			else if(tc_strcmp(WSO_Name,"T5_LcsAplReview")==0 || tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0)
			{
				fprintf(fpAssy,"");
				fprintf(fpAssy,"$");
				fprintf(fpAssy,"");
				fprintf(fpAssy,"$");
				fprintf(fpAssy,"WIP");
				fprintf(fpAssy,"$");
			}

		}
		else
		{

			printf("\n Release status count greater than 1 \n"); fflush(stdout);
			int cntSts=0,VaultStsFlagRel=0;
			for (cntSts=0;cntSts< st_count1;cntSts++ )
			{
				ITK_CALL(AOM_ask_name(status_list1[cntSts],&WSO_Name));
				printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);  //T5_LcsReview //ERC Released or T5_LcsErcRlzd// Released
				//if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0 || tc_strcmp(WSO_Name,"ERC Released")==0 )
				if (tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0)
				{
					VaultStsFlagRel=1;
					ITK_CALL(AOM_ask_value_string(status_list1[cntSts],"effectivity_text",&effectivityStr));
					printf("\n effectivityStr----><%s>\n",effectivityStr );
					if (effectivityStr!=NULL)
					{
						effectivityStrFinal = strtok(effectivityStr," ");
					}
					break;
				}
			}
			if (VaultStsFlagRel==1)
			{
				DATE_string_to_date ( effectivityStrFinal , "%d-%b-%Y", &month , &day ,&year , &hour , &minute , &second);
				printf("\effectivityStrFinal----><%s> %d-%d-%d\n",effectivityStrFinal,day,month,year);
				month++;
				//fprintf(fpAssy,"%d/%d/%d",month,day,year);
				//fprintf(fpAssy,"%d/%d/%d",year,month,day);
				fprintf(fpAssy,"%d/",year);
				if (month<10)
				{
					fprintf(fpAssy,"0%d",month);
				}
				else
				{
					fprintf(fpAssy,"%d",month);
				}

				if (day<10)
				{
					fprintf(fpAssy,"/0%d",day);
				}
				else
				{
					fprintf(fpAssy,"/%d",day);
				}
				fprintf(fpAssy,"$");
				fprintf(fpAssy,"12/31/9999");
				fprintf(fpAssy,"$");
				fprintf(fpAssy,"Released");
				fprintf(fpAssy,"$");
			}
			else
			{
				fprintf(fpAssy,"");
				fprintf(fpAssy,"$");
				fprintf(fpAssy,"");
				fprintf(fpAssy,"$");
				fprintf(fpAssy,"");
				fprintf(fpAssy,"$");
			}
		}
		fprintf(fpAssy,"\n");

    //MEM_free (children);
    //MEM_free (name);
    //MEM_free (sequence_no);
    //MEM_free (quantity);
    //MEM_free (orig_name);
 }

//-------------------------------------------Printing the DESIGN REVISON DATA--------------------------------------------------------------
int print_DesignRevisionData(tag_t itemrev,FILE *fdf)
{
	int				status 						= ITK_ok;
	int				r_count 					= 0;
	
	tag_t			item_tag 					= NULLTAG;
	tag_t			rev_tag						= NULLTAG;
	tag_t			*rev_list					= NULLTAG;
	
	char 			*rev_id						= NULL;


	char   *moddesc= NULL;
	char   *DrSts= NULL;
	char   *moddescDup= NULL;
	
	char   *mcreator= NULL;
	char   *mcreatorName= NULL;
	char   *VCreator= NULL;
	char   *VCreatorName= NULL;
	char   *VerCreator= NULL;
	char   *Creator= NULL;

	mcreator = (char *) MEM_alloc( 200 * sizeof(char) );
	mcreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	VCreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	VerCreator = (char *) MEM_alloc( 200 * sizeof(char) );
	
		
	ITK_CALL(AOM_UIF_ask_value(itemrev,"owning_user",&Creator)); //Creator
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_VerCreator",&VCreator));  //Version Creator

	mcreatorName = strtok(Creator,"(");
	mcreator = strtok(NULL,")");
	VCreatorName = strtok(VCreator,"(");
	VerCreator = strtok(NULL,")");

	ITK_CALL(AOM_ask_value_string(itemrev,"t5_DocRemarks",&moddesc));  //modification desc 
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_PartStatus",&DrSts));  //modification desc 


	printf("\n creator------->%s\n",moddesc);
	STRNG_replace_str(moddesc,"\n"," ",&moddescDup);
	printf("\n creator------->%s\n",moddescDup);

	printf("\n mcreatorName------->%s\n",mcreatorName);
	printf("\n creator------->%s\n",mcreator);
	printf("\n VerCreator------->%s\n",VerCreator);
	printf("\n DR status------->%s\n",DrSts);

	fprintf(fdf,mcreator);
	fprintf(fdf,"~");
	fprintf(fdf,VerCreator);
	fprintf(fdf,"~");
	fprintf(fdf,moddescDup);
	fprintf(fdf,"~");
	fprintf(fdf,DrSts);
	fprintf(fdf,"~");
	fprintf(fdf,"\n");
	
	return status;
}

//-------------------------------------------Packing the BOM--------------------------------------------------------------

int get_revision(char* item_id)
{
	int				ifail 						= ITK_ok;
	int				r_count 					= 0;
	
	tag_t			item_tag 					= NULLTAG;
	tag_t			rev_tag						= NULLTAG;
	tag_t			*rev_list					= NULLTAG;
	
	char 			*rev_id						= NULL;
	
	/*char 			*prev_rev_id				= NULL;
	char 			*rev_rel_status				= NULL;
	char			*status						= "ERC Working";*/
	
	ifail = ITEM_find_item (item_id, &item_tag);
	CHECK_FAIL;
	ifail = ITEM_ask_latest_rev (item_tag, &rev_tag);
	if(rev_tag == NULLTAG)
		{
			printf("Object not found in TC...!!\n");
			printf("***************************\n");
			return 0;
		}
		else
		{
			printf("\nObject Found...!!\n");
			ifail = AOM_UIF_ask_value (rev_tag, "item_revision_id", &rev_id);
			CHECK_FAIL;
			printf("\nLatest Revision ID is:%s\n",rev_id);
			
			ifail = stamp_find_no(rev_tag);
			CHECK_FAIL;
		}
	MEM_free(rev_id);
	
	return ifail;
}


/********************************************************************************************
    Function:       stamp_find_no
    Description:    This function takes latest rev_tag as input send it to Structure Manager
					and stamp find_no on it's child_bomline if two consecutive bomline's ID
					is matched.
********************************************************************************************/
int stamp_find_no(tag_t rev_tag)
{
	int				ifail 						= ITK_ok;
	int				i 							= 0;
	int				j 							= 0;
	int				child_count 				= 0;
	
	char			*find_no					= NULL;
	char			*child1_id					= NULL;
	char			*child2_id					= NULL;
	
	tag_t			window						= NULLTAG;
	tag_t			top_bom_line				= NULLTAG;
	tag_t			*bomline_childs				= NULLTAG;
	
	
	ifail = BOM_create_window(&window);
	if (window != NULLTAG)
	{
		printf("\nBOM Window created...\n");
	}
	else
	{
		printf("\nWindow is null\n");
	}
	ifail = BOM_set_window_top_line(window,NULLTAG,rev_tag,NULLTAG,&top_bom_line);
	ifail = BOM_window_show_suppressed ( window );
	if(ifail == ITK_ok)
		{
			printf("BOM Window Top_Line Set....\n");
		}
		else
		{
			printf("Window is null....\n");
		}
		
	ifail = BOM_line_ask_all_child_lines(top_bom_line, &child_count, &bomline_childs);
	CHECK_FAIL;
	printf("Total child bomlines are:%d\n",child_count);
	if(child_count > 0)
	{
		for(i=0;i<child_count-1;i++)
			{
				ifail = AOM_ask_value_string(bomline_childs[i], "bl_item_item_id", &child1_id);
				printf("\nBomline[%d] id is:%s",i,child1_id);
				
				ifail = AOM_ask_value_string(bomline_childs[i], "bl_sequence_no", &find_no);
				printf("\nFind No. of bomline[%d] is:%s\n",i,find_no);
				
				for(j=i+1;j<child_count;j++)
					{
						ifail = AOM_ask_value_string(bomline_childs[j], "bl_item_item_id", &child2_id);
						printf("\nBomline[%d] id is:%s",j,child2_id);
						if (tc_strcmp(child1_id,child2_id) == 0)
							{
								printf("\n[%d]&[%d] bomline Matched...!!",i,j);
								printf("\n******************************\n");
								//ifail = AOM_refresh (bomline_childs[j], 1);
								ifail = AOM_UIF_set_value (bomline_childs[j], "bl_sequence_no", find_no);
								
								ifail = BOM_save_window (window);
								
								//ifail = AOM_save (bomline_childs[j]);
								//ifail = AOM_refresh (bomline_childs[j], 0);
								if(ifail == ITK_ok)
									{
										printf("\nFind no.[%s]stamped....!!\n",find_no);
										printf("\n******************************\n");
									}
								else
									{
										printf("\nFailed to stamp find no...!!\n");
										printf("\n****************************\n");
									}
							}
						else
							{
								printf("\n[%d]&[%d] bomline Not Matched...!!\n",i,j);
								printf("\n**********************************\n");
							}
					}
			}
			
			ifail = BOM_set_window_pack_all(window, true);
			if(ifail == ITK_ok)
			{
				printf("\nAll Bomline Packed....!!\n");
				printf("\n**************************\n");
				ifail = BOM_save_window (window);
			}
			else
			{
				printf("\nFailed to pack bomlines...!!");
			}
			
			MEM_free(child1_id);
			MEM_free(child2_id);
			MEM_free(find_no);
	}
	else
	{
		printf("\nBomline Child not found...!!\n");
		printf("\n****************************\n");
		return 0;
	}
	
	MEM_free(bomline_childs);
	
	return ifail;
}
//---------------------------------------------------------------------------------------------------------
static void print_bom (tag_t line,tag_t Parent, int depth,FILE *fdf,FILE *fus,FILE *fLhRh,FILE *fAttr,FILE *fPro,char *req_item,char *FilePath,int DepthInput)
 {
    int    status;
	int    k, n,int_ent_sequence;
	int    Item_ID,Item_Revision,Item_Description,parent_item_seq=0,QuantityAttrId,TransforationMatrixAttrId,Item_WeldPrts,PrtTypeAtr,iChildItemTag;
	int    n_attchs,i,referencenumberfound,j,referencenumberfoundJT,jJT,n_attchsLH,n_attchsWeld,iWeld,n_attchs_JT,referencenumberfoundPdf,n_attchs_ProMem, iProAsm;
	int    numBoundingBoxes,ctr;
	int    Parentsequence_id=0;
	int    dum=0;

	double *  boundingBoxes;
	char   *name, *Item_ID_str,*Item_Revision_str,*Item_Description_str,*Item_WeldPrts_str,*ProAsmItemName;
	char   *dumy_Item_ID_str=NULL;
	char   *catiafileName=NULL;
	char   *projectcode=NULL;
	char   *projectcode2=NULL;
	char   *desgngrp=NULL;
	char   *desgngrp2=NULL;
	char   *ret_id,*ret_rev;
	char   type_name[TCTYPE_name_size_c+1];
	char   type_name2[TCTYPE_name_size_c+1];
	char   type_name_rel[TCTYPE_name_size_c+1];
	char   type_name_JT[TCTYPE_name_size_c+1];
	char   type_name_Pdf[TCTYPE_name_size_c+1];
	char   refname[AE_reference_size_c + 1];
	char   refnameJT[AE_reference_size_c + 1];
	char   refnamePdf[AE_reference_size_c + 1];
	char   orig_name[IMF_filename_size_c + 1];
	char   orig_nameJT[IMF_filename_size_c + 1];
	char   orig_namePdf[IMF_filename_size_c + 1];
	char   orig_nameJT_Tmp[IMF_filename_size_c + 1];
	char   orig_name_Tmp[IMF_filename_size_c + 1];
	char   orig_namePdf_Tmp[IMF_filename_size_c + 1];
	char   *enterprise_sequence;
	char   *pdfName;
	char   *AppOwner;
	char   *Weld_Item;
	char   pathname[SS_MAXPATHLEN + 1];
	char   pathnameJT[SS_MAXPATHLEN + 1];
	char   pathnamePdf[SS_MAXPATHLEN + 1];
	char   *ent_seq_str;
	char   *parentrev=NULL;
	char   *PrtType=NULL;
	char   *PrtTypeDup=NULL;
	char   *parent=NULL;
	char   *Quantity=NULL;
	char   *TransforationMatrix=NULL;
	char   *TransforationMatrix_O=NULL;
	char   *parentseq=NULL;
	char   *parentdesc=NULL;
	char   *PatSeq= NULL;
	char   *ITemRevSeq= NULL;
	char   *Item_Revision_str_O= NULL;
	char   *Item_Revision_str_O1= NULL;
	char   *parentrev_O= NULL;
	char   *tokenDI= NULL;
	char   *tokenJT= NULL;
	char   *tokenPdf= NULL;
	char   *LH_RH_Value_Item= NULL;
	char   *ProMember= NULL;
	char   *RH_Value_Item= NULL;
	char   *RH_Item= NULL;
	char   *PrtAsmRev_O= NULL;
	char   *PrtAsmRev= NULL;
	char   *VaultSts= NULL;
	char* item_type= NULL;
	char* effectivityStr= NULL;
	int		sysreturn		= 0,iJT;
	//char*  Bypass="5442T6B0197001,544263600116,544263600115,544263600115,12160500601"; //Bypass the uses part  for the given part list
	
	tag_t  objTypeTag_rel =NULLTAG;
	tag_t  ParentItemRevTag =NULLTAG;
	tag_t  *secondary_objects,primary,objTypeTag,refobject=NULLTAG,primaryJT,objTypeTagJT,refobjectJT,primaryRH,relationstrRH,primaryWeld,primaryPdf,objTypeTagPdf,refobjectPdf,primaryAsm;
	tag_t  *children,itemrev,sequence_id_c;
	tag_t  reln_type =NULLTAG;
	tag_t  reln_type_Render =NULLTAG;
	tag_t  reln_typeLH =NULLTAG;
	tag_t  reln_typeWeld =NULLTAG;
	tag_t  relationstr =NULLTAG;
	tag_t  relationstrJT =NULLTAG;
	tag_t  item_tag =NULLTAG;
	tag_t  reln_type_ProMem =NULLTAG;
	tag_t  reln_type_ProDrw =NULLTAG;    
	tag_t  t_ChildItemRev =NULLTAG;
	tag_t*    status_list1=NULLTAG;
	char *WSO_Name = NULL;
	GRM_relation_t *rellist, *rellistLH,*rellistWeld,*rellist_JT,*rellist_Pdf,*rellist_ProMem,*rellist_ProDrw;
	AE_reference_type_t     reftype;
	AE_reference_type_t     reftypeJT;
	AE_reference_type_t     reftypePdf;
	tag_t *tags_Itm = NULL;
    int n_tags_found= 0;
	int cnt = 0;
	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));
	char	*type_nameD	=	NULL;
	int	iDsg	=	0;

	int iPdf=0,n_attchs_Pdf=0,jPdf=0,CadCount=0,st_count1=0,VaultStsFlagRel=0,VaultStsFlagWIP=0,n_attchs_ProDrw=0;
	
	
	depth ++;

	printf("\nprinting fdf depth after:%d  %d*****************************",depth,DepthInput);
    ITK_CALL( BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
	ITK_CALL(BOM_line_ask_attribute_string(line, Item_ID, &Item_ID_str));
	printf("\n1] Item_ID_str =%s\n",Item_ID_str);
	//printf("\n1] Item_ID_strstr %s = %d\n",Bypass,tc_strstr(Item_ID_str,Bypass));  // Bypass the uses part 
	//if (tc_strstr(Bypass,Item_ID_str)!=NULL)
	//{
		//goto LAST;
	//}
	attrs[0] ="item_id";
	values[0] = (char *)Item_ID_str;

	ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs,values, &n_tags_found, &tags_Itm));
	MEM_free(attrs);
	MEM_free(values);
	if (n_tags_found > 0)
	{
		for (iDsg=0;iDsg<n_tags_found ;iDsg++ )
		{
			item_tag = tags_Itm[iDsg];
			if(TCTYPE_ask_object_type(item_tag,&objTypeTag));
			if(TCTYPE_ask_name2(objTypeTag,&type_nameD));
			printf("\n\n\t\t AssyTag type_name := %s", type_nameD);fflush(stdout);

			if (tc_strcmp(type_nameD,"Design Revision")==0 || tc_strcmp(type_nameD,"Design_0_Revision_alt")==0)
			{
			}
			else
			{
				printf("\n %s is not Design, so continue",Item_ID_str);fflush(stdout);
				continue;
			}
		}
		//item_tag = tags_Itm[0];
		MEM_free(tags_Itm);

		AOM_ask_value_string(item_tag,"t5_LeftRh",&LH_RH_Value_Item);

		if (tc_strcmp(LH_RH_Value_Item,"LH CATIA") || tc_strcmp(LH_RH_Value_Item,"RH CATIA") || tc_strcmp(LH_RH_Value_Item,"RH PROE") || tc_strcmp(LH_RH_Value_Item,"LH PROE"))
		{
			ITK_CALL(GRM_find_relation_type("T5_LRReln",&reln_typeLH));
			ITK_CALL(GRM_list_secondary_objects(item_tag,reln_typeLH,&n_attchsLH,&rellistLH));
			printf("\n LH_RH_Value_Item ----------------------------->>>>>>%s %d\n",LH_RH_Value_Item,n_attchsLH);

			if (n_attchsLH==1)
			{
				primaryRH=rellistLH[0].secondary;
				relationstrRH=rellistLH[0].the_relation;

				AOM_ask_value_string(primaryRH,"object_name",&RH_Item);
				AOM_ask_value_string(primaryRH,"t5_LeftRh",&RH_Value_Item);

				printf("\n RH_Value_Item ----------------------------->>>>>>%s %s\n",RH_Item,RH_Value_Item);
				if ((strstr(LH_RH_Value_Item,"LH") && strstr(RH_Value_Item,"RH")) || (strstr(LH_RH_Value_Item,"RH") && strstr(RH_Value_Item,"LH")))
				{
					fprintf(fLhRh,Item_ID_str);
					fprintf(fLhRh,",");
					fprintf(fLhRh,LH_RH_Value_Item);
					fprintf(fLhRh,",");
					fprintf(fLhRh,RH_Item);
					fprintf(fLhRh,",");
					fprintf(fLhRh,RH_Value_Item);
					fprintf(fLhRh,",\n");
				}
				else
				{

				}
			}
			else if (n_attchsLH > 1)
			{

			}
			else
			{

			}
		}




		ITK_CALL( BOM_line_look_up_attribute ("bl_rev_item_revision_id",&Item_Revision));
		ITK_CALL(BOM_line_ask_attribute_string(line, Item_Revision, &Item_Revision_str));
		printf("\n2] Item_Revision_str =%s\n",Item_Revision_str);
		if (tc_strlen(Item_Revision_str)==0)   //// Applied for unconfigured BOM
		{
			goto LAST;
		}
	
		STRNG_replace_str(Item_Revision_str,";","~",&Item_Revision_str_O);
		STRNG_replace_str(Item_Revision_str,";",",",&Item_Revision_str_O1);
		printf("\n2.1] Item_Revision_str =%s\n",Item_Revision_str_O);





		ITK_CALL(BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &parent_item_seq));
		ITK_CALL(BOM_line_ask_attribute_tag(line, parent_item_seq, &itemrev));

		ITK_CALL(GRM_find_relation_type("T5_HasWeldParts",&reln_typeWeld));
		ITK_CALL(GRM_list_secondary_objects(itemrev,reln_typeWeld,&n_attchsWeld,&rellistWeld));
		printf("\n n_attchsWeld = %d\n",n_attchsWeld);
		
	//	if (n_attchsWeld>0)
	//	{
	//		fprintf(fWeld,Item_ID_str);
	//		fprintf(fWeld,",");
	//		fprintf(fWeld,Item_Revision_str_O1);
	//		fprintf(fWeld,",");
	//		for (iWeld=0;iWeld<n_attchsWeld;iWeld++)
	//		{
	//			primaryWeld=rellistWeld[iWeld].secondary;
	//			AOM_ask_value_string(primaryWeld,"object_name",&Weld_Item);
	//			printf("\n3] Weld_Item =%s\n",Weld_Item);
	//			fprintf(fWeld,Weld_Item);
	//			fprintf(fWeld,",");
	//		}
	//		fprintf(fWeld,"\n");
	//	}



		//ITK_CALL(AOM_ask_value_int(itemrev,"sequence_id",&sequence_id_c));
		ITK_CALL(AOM_ask_value_int(itemrev,"sequence_id",&sequence_id_c));
		printf("\n3] Sequence =%d\n",sequence_id_c);
		ITemRevSeq = (char *) MEM_alloc( 5 * sizeof(char) );
		tokenDI = (char *) MEM_alloc( 200 * sizeof(char) );
		tokenJT = (char *) MEM_alloc( 200 * sizeof(char) );
		sprintf(ITemRevSeq,"%d",sequence_id_c);

		ITK_CALL( BOM_line_look_up_attribute ("bl_rev_object_desc",&Item_Description));
		ITK_CALL(BOM_line_ask_attribute_string(line, Item_Description, &Item_Description_str));
		printf("\n4] Item_Description_str =%s\n",Item_Description_str);

		



		ITK_CALL(AOM_ask_value_string(itemrev,"t5_PartType",&PrtTypeDup));
		ITK_CALL(AOM_ask_value_string(itemrev,"t5_ProjectCode",&projectcode));
		ITK_CALL(AOM_ask_value_string(itemrev,"t5_DesignGrp",&desgngrp));

		PrtType = ( char * ) MEM_alloc(10);
		if (!tc_strcmp(PrtTypeDup,"DC") || !tc_strcmp(PrtTypeDup,"DA") || !tc_strcmp(PrtTypeDup,"IFD") || !tc_strcmp(PrtTypeDup,"CM"))
		{
			
			strcpy(PrtType,"D");
		}
		else 
		{
			strcpy(PrtType,PrtTypeDup);
		}




		VaultSts=get_ReleaseStatus( itemrev);

		printf("\n      PrtTypeAtr----><%s>\n",PrtType );
		printf("\n projectcode------->%s\n",projectcode);
		printf("\n desgngrp------->%s\n",desgngrp);
		printf("\n VaultSts------->%s\n",VaultSts);


		ITK_CALL(AOM_ask_value_string(itemrev,"gov_classification",&enterprise_sequence));
		printf("\n -----------------------enterprise_sequence -->%s\n",enterprise_sequence);

		ITK_CALL(AOM_ask_value_string(itemrev,"t5_ApplicationOwner",&AppOwner));
		printf("\n -----------------------AppOwner -->%s\n",AppOwner);
		if (AppOwner)
		{
			if (strlen(AppOwner)==0)
			{
				AppOwner = ( char * ) MEM_alloc(10);
				strcpy(AppOwner,"ProCat");
			}
		}
		else
		{
			AppOwner = ( char * ) MEM_alloc(10);
			strcpy(AppOwner,"ProCat");
		}

		int_ent_sequence=atoi(enterprise_sequence);

	//dumy_Item_ID_str=MEM_string_copy(Item_ID_str) ;




		if (Parent == null_tag)
		{
			printf("\nThis is first line\n");
		}
		else
		{
			ITK_CALL(  BOM_line_look_up_attribute ("bl_rev_item_revision_id",&Item_Revision));
			ITK_CALL(  BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
			ITK_CALL(  BOM_line_ask_attribute_string(Parent, Item_Revision, &parentrev));
			ITK_CALL(  BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &parent_item_seq));
			ITK_CALL(  BOM_line_look_up_attribute ("bl_rev_object_desc",&Item_Description));


			ITK_CALL(BOM_line_ask_attribute_string(Parent, Item_ID, &parent));
			ITK_CALL(BOM_line_look_up_attribute("bl_quantity", &QuantityAttrId));
			ITK_CALL(BOM_line_look_up_attribute("bl_plmxml_abs_xform", &TransforationMatrixAttrId));
			ITK_CALL(  BOM_line_ask_attribute_string(line,QuantityAttrId, &Quantity));
			ITK_CALL(  BOM_line_ask_attribute_string(line,TransforationMatrixAttrId, &TransforationMatrix));

			ITK_CALL(BOM_line_ask_attribute_tag(Parent, parent_item_seq, &ParentItemRevTag));

			ITK_CALL(AOM_ask_value_int(ParentItemRevTag,"sequence_id",&Parentsequence_id));


			PatSeq=malloc(3);
			sprintf(PatSeq,"%d",Parentsequence_id);
			ITK_CALL(BOM_line_ask_attribute_string(Parent, Item_Description, &parentdesc));
			printf("\n       parent---->%s\n", parent);
			printf("\n       parentrev---->%s\n",parentrev );
			printf("\n       PatSeq---->%s\n",PatSeq );
			printf("\n       parentdesc---->%s\n",parentdesc );
			printf("\n       Quantity----><%s>\n",Quantity );
			printf("\n       TransforationMatrix---->%s\n",TransforationMatrix );
			printf("\n       DMLFlag---->%d\n",DMLFlag );

			ITK_CALL (TCTYPE_ask_object_type(itemrev,&objTypeTag));
			ITK_CALL (TCTYPE_ask_name(objTypeTag,type_name2));
			printf("\n Started for type_nameee allow only for selected class  = %s\n", type_name2);fflush(stdout);

			STRNG_replace_str(parentrev,";",",",&parentrev_O);
			STRNG_replace_str(TransforationMatrix," ",",",&TransforationMatrix_O);
			printf("\n2.1] parentrev =%s\n",parentrev_O);
			printf("\n2.1] parentrev =%s\n",TransforationMatrix_O);
			//char* TM01= NULL;
			//char* TM02= NULL;
			//char* TM03= NULL;
			//char* TM04= NULL;
			//char* TM05= NULL;
			//char* TM06= NULL;
			//char* TM07= NULL;
			//char* TM08= NULL;
			//char* TM09= NULL;
			//char* TM10= NULL;
			//char* TM11= NULL;
			//char* TM12= NULL;
			//char* TM13= NULL;
			//char* TM14= NULL;
			//char* TM15= NULL;
			//char* TM16= NULL;

			//TM01=tc_strtok(TransforationMatrix_O,",");
			//TM02 = tc_strtok(NULL,",");
			//TM03 = tc_strtok(NULL,",");
			//TM04 = tc_strtok(NULL,",");
			//TM05 = tc_strtok(NULL,",");
			//TM06 = tc_strtok(NULL,",");
			//TM07 = tc_strtok(NULL,",");
			//TM08 = tc_strtok(NULL,",");
			//TM09 = tc_strtok(NULL,",");
			//TM10 = tc_strtok(NULL,",");
			//TM11 = tc_strtok(NULL,",");
			//TM12 = tc_strtok(NULL,",");
			//TM13 = tc_strtok(NULL,",");
			//TM14 = tc_strtok(NULL,",");
			//TM15 = tc_strtok(NULL,",");
			//TM16 = tc_strtok(NULL,",");
			//printf("\n <%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s>,<%s> \n",TM01,TM02,TM03,TM04,TM05,TM06,TM07,TM08,TM09,TM10,TM11,TM12,TM13,TM14,TM15,TM16);
			//double TM13F;
			//double TM14F;
			//double TM15F;
			//int TM13Flag=0,TM14Flag=0,TM15Flag=0;
			//if (strstr(TM13,"e"))TM13Flag=1;
			//if (strstr(TM14,"e"))TM14Flag=1;
			//if (strstr(TM15,"e"))TM15Flag=1;

			//TM13F=atof(TM13);
			//TM14F=atof(TM14);
			//TM15F=atof(TM15);
			//TM13F=TM13F*1000;
			//TM14F=TM14F*1000;
			//TM15F=TM15F*1000;
			////sprintf(TM01F, "%e", TM01);
			//if (TM13Flag==1)
			//{
				//printf("\n TM13F =%.14e\n",TM13F);
			//}
			//else
			//{
				//printf("\n TM13F =%.16f\n",TM13F);
			//}

			//if (TM14Flag==1)
			//{
				//printf("\n TM14F =%.14e\n",TM14F);
			//}
			//else
			//{
				//printf("\n TM14F =%.16f\n",TM14F);
			//}

			//if (TM15Flag==1)
			//{
				//printf("\n TM15F =%.14e\n",TM15F);
			//}
			//else
			//{
				//printf("\n TM15F =%.16f\n",TM15F);
			//}
			

			if(tc_strcmp(type_name2,"Design Revision")==0)
			{
				fprintf(fus,parent);
				fprintf(fus,",");
				fprintf(fus,parentrev_O);
				fprintf(fus,",");
	//			fprintf(fus,PatSeq);
	//			fprintf(fus,",");
				fprintf(fus,Item_ID_str);
				fprintf(fus,",");
				fprintf(fus,Item_Revision_str_O1);
	//			fprintf(fus,",");
	//			fprintf(fus,ITemRevSeq);
				fprintf(fus,",");
				fprintf(fus,Item_ID_str);
				fprintf(fus,".1");
				fprintf(fus,",");
				fprintf(fus,TransforationMatrix_O);
	//			fprintf(fus,"1");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"1");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"1");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"0");
	//			fprintf(fus,",");
	//			fprintf(fus,"1");
				fprintf(fus,",");
				if(strlen(Quantity)>0)
				{
					fprintf(fus,Quantity);
				}
				else
				{
					//fprintf(fus,"-"); //as per discussion with Manisha madam and borkar on 06-jun-2018 it will be 1 if qty is blank
					fprintf(fus,"1");
				}
				fprintf(fus,",");
				fprintf(fus,"reptest");
				fprintf(fus,"\n");
				//bomAttr_AbsTransformMatrix         "bl_abs_xform_matrix"
				//bomAttr_OccTransformMatrix         "bl_occ_xform_matrix"
				//fprintf(fus,Item_Description_str);
				//fprintf(fus,",");
				//fprintf(fus,"NA");
				//fprintf(fus,",");
				//fprintf(fus,parentdesc);
				//fprintf(fus,",");
				//fprintf(fus,occName);
				//fprintf(fus,",");
				//fprintf(fus,strQty);
				//fprintf(fus,",");
				//fprintf(fus,"-");
				//fprintf(fus,",");
				//fprintf(fus,"reptest");
			}
		}
	
		if (tc_strcmp(projectcode,"1111")==0)
		{
			goto LAST;
		}
		if ( depth > DepthInput)
		{
			goto LAST;
		}

		print_ATTR(itemrev,fAttr);
		//ITK_CALL(GRM_list_secondary_objects_only(itemrev,reln_type,&n_attchs,&secondary_objects));
		ITK_CALL(GRM_list_secondary_objects(itemrev,reln_type,&n_attchs,&rellist));
		printf("\n Total n attches =%d\n",n_attchs);
		if(n_attchs>0)  // need to comment start here if only module is required in cad output file 
		{
			ITK_CALL(GRM_find_relation_type("IMAN_Rendering",&reln_type_Render));
			ITK_CALL(GRM_list_secondary_objects(itemrev,reln_type_Render,&n_attchs_JT,&rellist_JT));
			printf("\n Total JT..............%d",n_attchs_JT);fflush(stdout);
			for (i= 0; i < n_attchs; i++)
			{
				printf("\n Inside loop ..............");fflush(stdout);
				//primary=secondary_objects[i];
				primary=rellist[i].secondary;
				relationstr=rellist[i].the_relation;

				ITK_CALL(TCTYPE_ask_object_type(primary,&objTypeTag));
				ITK_CALL(TCTYPE_ask_name(objTypeTag,type_name));
				printf("\n Type Name:%s ..............",type_name);fflush(stdout);
				printf("\n -----------------------AppOwner -->%s\n",AppOwner);

				if( (tc_strcmp(type_name,"CATProduct")==0 ||tc_strcmp(type_name,"CMI2Product")==0 || tc_strcmp(type_name,"t5Alias")==0 || tc_strcmp(type_name,"CATPart")==0 || tc_strcmp(type_name,"CMI2Part")==0 || tc_strcmp(type_name,"CMI2AuxPart")==0) && strstr(AppOwner,"Cat"))
				{
					CadCount++;
					printf("\n--------inside if of n attaches Catia Product CAt part---------------------\n");fflush(stdout);
					ITK_CALL(AE_ask_dataset_id_rev(primary,&ret_id,&ret_rev));
					ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
					printf("\nreferencenumberfound =%d\n",referencenumberfound);

					for(j=0;j<referencenumberfound;j++)
					{
						printf("\n -------- inside referencenumberfound CAT PRD PRT--------------------- \n");
						ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
						printf("\n ---name ref type CAT PRD PRT----- :%s\n",refname);
						if(tc_strcmp(refname,"CMIAuxFileInfo") && tc_strcmp(refname,"CMI2BBInfo") && tc_strcmp(refname,"CMI2TypeInfo"))
						{
							ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
							ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));
							printf("\n orig_name is :%s\n",orig_name);
							if(strstr(orig_name,"jt")==NULL)
							{
								printf("\nenterprise %d \n",int_ent_sequence);
								printf("\nua %d\n",sequence_id_c);
								tokenDI = strtok(orig_name,".");
								printf("\ntokenDI %s\n",tokenDI);
								ctr=int_ent_sequence+1;
								printf("\nNot in spec loop %s\n",Item_ID_str);									
								fprintf(fdf,Item_ID_str);						//1]  Part nos                                    
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,projectcode);						//2] projectcode                                  
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,desgngrp);							//3] desgngrp                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,Item_Description_str);				//4] Description                                  
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,orig_name_Tmp);						//5] Cad data if no cad data then NULL is stamped 
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"NULL");							//6] ???????                                      
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,Item_Revision_str_O);				//7] Rev and sequence of design revison           
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,PrtType);							//8] Part type of part                            
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,VaultSts);								//9] Part type of part                            
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,orig_name_Tmp);						//10] Cad data if no cad data then NULL is stamped
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,Item_Revision_str_O);				//11] Rev and sequence of design revison          
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"NULL");							//12] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"NULL");							//13] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"NULL");							//14] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"NULL");							//15] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"NULL");							//16] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"NULL");							//17] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"0");								//18] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"Y");								//19] bom INDICATOR                               
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"-");								//20] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"-");								//21] ???????                                     
								fprintf(fdf,"~");								                                                    
								fprintf(fdf,"reptest#");						//22] ???????                                     
								fprintf(fdf,"NULL#");
								fprintf(fdf,"NULL#");
								fprintf(fdf,"NULL");
								fprintf(fdf,"~");
								
					
								printf("\norig_name issssssss 11---->%s\n",orig_name);
								if (!strstr(orig_name,"Spec"))
								{
									for (iJT= 0; iJT < n_attchs_JT; iJT++)
									{
										printf("\n Inside loop JT .............. %d \n",iJT);fflush(stdout);
										//primary=secondary_objects[i];
										primaryJT=rellist_JT[iJT].secondary;
										//relationstrJT=rellist_JT[iJT].the_relation;

										ITK_CALL(TCTYPE_ask_object_type(primaryJT,&objTypeTagJT));
										ITK_CALL(TCTYPE_ask_name(objTypeTagJT,type_name_JT));

										if(tc_strcmp(type_name_JT,"DirectModel")==0 && (!tc_strcmp(PrtType,"SA") ||  !tc_strcmp(PrtType,"D") || !tc_strcmp(PrtType,"DA") || !tc_strcmp(PrtType,"DC")))
										{
											
											ITK_CALL(AE_ask_dataset_ref_count(primaryJT,&referencenumberfoundJT));
											printf("\n JT found......%d",referencenumberfoundJT);fflush(stdout);
											for(jJT=0;jJT<referencenumberfoundJT;jJT++)
											{
												ITK_CALL(AE_find_dataset_named_ref(primaryJT,jJT,refnameJT,&reftypeJT,&refobjectJT));
												ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT));
												ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT_Tmp));
												printf("\n orig_nameJT is :%s  jJT Conut %d\n",orig_nameJT,jJT);
												tokenJT = strtok(orig_nameJT,".jt");
												printf("\n tokenJT %s tokenDI%s\n",tokenJT,tokenDI);
												if(tc_strcmp(tokenJT,tokenDI)==0)
												{
													fprintf(fdf,orig_nameJT_Tmp);
													sprintf(pathnameJT,"%s%s/%s",FilePath,req_item,orig_nameJT_Tmp );
													printf("\n pathnameJT=%s\n",pathnameJT);
													IMF_export_file(refobjectJT,pathnameJT);
													
												}
											}
										}
									}
								}
								fprintf(fdf,"~");
								print_DesignRevisionData(itemrev,fdf);
								
								//sprintf(pathname,"%s/%s/%s","/home/trlprod/TCUA/Downloader",req_item,orig_name );
								sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
								printf("\n pathname=%s\n",pathname);
								IMF_export_file(refobject,pathname);
							}
						}
					}
				}
				else if((tc_strcmp(type_name,"CATDrawing")==0||tc_strcmp(type_name,"CMI2Drawing")==0) && strstr(AppOwner,"Cat"))
				{
					CadCount++;
					ITK_CALL(TCTYPE_ask_object_type(relationstr,&objTypeTag_rel));
					ITK_CALL(TCTYPE_ask_name(objTypeTag_rel,type_name_rel));

					printf("\n--------inside if of n attaches---------------------\n");fflush(stdout);

					ITK_CALL(AE_ask_dataset_id_rev(primary,&ret_id,&ret_rev));
					ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
					printf("\nreferencenumberfound =%d\n",referencenumberfound);

					for(j=0;j<referencenumberfound;j++)
					{
						printf("\n--------inside referencenumberfound---------------------\n");
						ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
						printf("\n ---name ref type CAT drw----- :%s\n",refname);
						if(tc_strcmp(refname,"CATDrawing")==0)
						{
						ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
						ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));
						printf("\n orig_name is Cat :%s %s\n",orig_name,orig_name_Tmp);

						tokenDI = strtok(orig_name,".");
						printf("\ntokenDI Drawing Loop %s\n",tokenDI);

						if(tc_strcmp(type_name_rel,"IMAN_reference")!=0)
						{
							if(strstr(orig_name_Tmp,"pdf")==NULL)
							{

									ctr=int_ent_sequence+1;


										printf("\nNot in spec loop  %s\n",Item_ID_str);

										fprintf(fdf,Item_ID_str);					//1]  Part nos                                    
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,projectcode);					//2] projectcode                                  
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,desgngrp);						//3] desgngrp                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,Item_Description_str);			//4] Description                                  
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,orig_name_Tmp);					//5] Cad data if no cad data then NULL is stamped 
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//6] ???????                                      
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,Item_Revision_str_O);			//7] Rev and sequence of design revison           
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,PrtType);						//8] Part type of part                            
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,VaultSts);							//9] Part type of part                            
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,orig_name_Tmp);					//10] Cad data if no cad data then NULL is stamped
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,Item_Revision_str_O);			//11] Rev and sequence of design revison          
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//12] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//13] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//14] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//15] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//16] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//17] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"0");							//18] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"Y");							//19] bom INDICATOR                               
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"-");							//20] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"-");							//21] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"reptest#");					//22] ???????                                     
										

										if(tc_strcmp(type_name_rel,"IMAN_reference")==0)
										{
											fprintf(fdf,Item_ID_str);
											fprintf(fdf,"#");
											fprintf(fdf,Item_Revision_str_O);

										}
										else
										{
											fprintf(fdf,"NULL#");
											fprintf(fdf,"NULL#");
											fprintf(fdf,"NULL");
										}
										
										fprintf(fdf,"~");
										char 	relative_path[SS_MAXPATHLEN] ;
										for (iPdf= 0; iPdf < n_attchs; iPdf++)
										{
											printf("\n Inside loop Pdf .............. %d \n",iPdf);fflush(stdout);
											//primary=secondary_objects[i];
											primaryPdf=rellist[iPdf].secondary;
											//relationstrJT=rellist_JT[iJT].the_relation;

											ITK_CALL(TCTYPE_ask_object_type(primaryPdf,&objTypeTagPdf));
											ITK_CALL(TCTYPE_ask_name(objTypeTagPdf,type_name_Pdf));

											if(tc_strcmp(type_name_Pdf,"PDF")==0)
											{
												ITK_CALL(AE_ask_dataset_ref_count(primaryPdf,&referencenumberfoundPdf));
												printf("\n Pdf found......%d",referencenumberfoundPdf);fflush(stdout);
												//for(jPdf=0;jPdf<referencenumberfoundPdf;jPdf++)
												//{
													jPdf=0;
													ITK_CALL(AE_find_dataset_named_ref(primaryPdf,jPdf,refnamePdf,&reftypePdf,&refobjectPdf));
													ITK_CALL( IMF_ask_original_file_name(refobjectPdf,orig_namePdf));
													ITK_CALL( IMF_ask_original_file_name(refobjectPdf,orig_namePdf_Tmp)); //file_name
													ITK_CALL(AOM_ask_value_string(refobjectPdf,"file_name",&pdfName));
													printf("\n orig_namePdf is :%s  jPdf Count %d >>>>>>> %s\n",orig_namePdf,jPdf,pdfName);
													tokenPdf = strtok(orig_namePdf,".pdf");
													printf("\n tokenPdf %s tokenDI%s\n",tokenPdf,tokenDI);
													if(tc_strcmp(tokenPdf,tokenDI)==0)
													{
														fprintf(fdf,orig_namePdf_Tmp);    //23] JT/PDF             
														fprintf(fdf,"#");    //23] JT/PDF             
														sprintf(pathnamePdf,"%s%s/%s",FilePath,req_item,orig_namePdf_Tmp );
														printf("\n pathnamePdf=%s\n",pathnamePdf);
														IMF_export_file(refobjectPdf,pathnamePdf);
														
													}
													
													IMF_ask_file_pathname(refobjectPdf,SS_UNIX_MACHINE,relative_path);
													printf("\n relative_path=%s\n",relative_path);
													fprintf(fdf,relative_path);
												//}
											}
										}

										fprintf(fdf,"~");
											print_DesignRevisionData(itemrev,fdf);
											
										printf("\norig_name issssssss ---->%s\n",orig_name_Tmp);
										//sprintf(pathname,"%s/%s/%s","/home/trlprod/TCUA/Downloader",req_item,orig_name );
										sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
										printf("\npathname=%s\n",pathname);
										IMF_export_file(refobject,pathname);
										

							}
						}
					}
					}
				}
				else if((tc_strcmp(type_name,"ProAsm")==0 ||tc_strcmp(type_name,"ProPrt")==0) && strstr(AppOwner,"Pro"))
				{
					printf("\n--------inside if of n attaches Pro part Asm---------------------\n");fflush(stdout);
					CadCount++;

					if ( depth <= DepthInput)
					{
						/// Pro Assembly member relationship
						ITK_CALL(GRM_find_relation_type("Pro2_membership",&reln_type_ProMem));
						ITK_CALL(GRM_list_secondary_objects(primary,reln_type_ProMem,&n_attchs_ProMem,&rellist_ProMem));
						printf("\n PRO ASM relation found <%d> \n",n_attchs_ProMem);fflush(stdout);

						for (iProAsm=0;iProAsm<n_attchs_ProMem;iProAsm++)  
						{
							primaryAsm=rellist_ProMem[iProAsm].secondary;
							AOM_ask_value_string(primaryAsm,"item_id",&ProAsmItemName);
							VaultSts=get_ReleaseStatus(primaryAsm);
							printf("\n ProAsmItemName =%s, %s\n",ProAsmItemName,VaultSts);fflush(stdout);
							fprintf(fPro,"ProAsmRel~");
							fprintf(fPro,Item_ID_str);
							fprintf(fPro,"~");
							fprintf(fPro,Item_Revision_str_O);
							fprintf(fPro,"~");
							fprintf(fPro,ProAsmItemName);
							fprintf(fPro,"~");
							ITK_CALL(AOM_ask_value_string(primaryAsm,"item_revision_id",&PrtAsmRev))
							printf("\n2] PrtAsmRev =%s\n",PrtAsmRev);
							STRNG_replace_str(PrtAsmRev,";","~",&PrtAsmRev_O);
							fprintf(fPro,PrtAsmRev_O);
							fprintf(fPro,"~");
							fprintf(fPro,VaultSts);
							fprintf(fPro,"\n");
						}
					}

					ITK_CALL(AE_ask_dataset_id_rev(primary,&ret_id,&ret_rev));
					ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
					printf("\nreferencenumberfound =%d\n",referencenumberfound);

					for(j=0;j<referencenumberfound;j++)
					{
						printf("\n -------- inside referencenumberfound --------------------- \n");
						ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
						printf("\n ---test for FSC proxy error----- :%s\n",refname);
						if(!tc_strcmp(refname,"AsmFile")  || !tc_strcmp(refname,"PrtFile"))
						{
							ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
							ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));

							printf("\n orig_name is :%s\n",orig_name);

							if(strstr(orig_name,"jt")==NULL)
							{
								printf("\nenterprise %d \n",int_ent_sequence);
								printf("\nua %d\n",sequence_id_c);

								tokenDI = strtok(orig_name,".");
								printf("\ntokenDI %s\n",tokenDI);

									ctr=int_ent_sequence+1;


										printf("\nNot in spec loop %s\n",Item_ID_str);

										fprintf(fdf,Item_ID_str);						//1]  Part nos                                    
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,projectcode);						//2] projectcode                                  
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,desgngrp);							//3] desgngrp                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,Item_Description_str);				//4] Description                                  
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,orig_name_Tmp);						//5] Cad data if no cad data then NULL is stamped 
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//6] ???????                                      
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,Item_Revision_str_O);				//7] Rev and sequence of design revison           
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,PrtType);							//8] Part type of part                            
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,VaultSts);								//9] Part type of part                            
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,orig_name_Tmp);						//10] Cad data if no cad data then NULL is stamped
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,Item_Revision_str_O);				//11] Rev and sequence of design revison          
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//12] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//13] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//14] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//15] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//16] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//17] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"0");								//18] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"Y");								//19] bom INDICATOR                               
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"-");								//20] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"-");								//21] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"reptest#");						//22] ???????                                     
										fprintf(fdf,"NULL#");
										fprintf(fdf,"NULL#");
										fprintf(fdf,"NULL");
										fprintf(fdf,"~");
										
					
										printf("\norig_name issssssss 11---->%s\n",orig_name);
										for (iJT= 0; iJT < n_attchs_JT; iJT++)
										{
											printf("\n Inside loop JT .............. %d \n",iJT);fflush(stdout);
											//primary=secondary_objects[i];
											primaryJT=rellist_JT[iJT].secondary;
											//relationstrJT=rellist_JT[iJT].the_relation;

											ITK_CALL(TCTYPE_ask_object_type(primaryJT,&objTypeTagJT));
											ITK_CALL(TCTYPE_ask_name(objTypeTagJT,type_name_JT));

											if(tc_strcmp(type_name_JT,"DirectModel")==0 && !tc_strcmp(PrtType,"C"))
											{
												
												ITK_CALL(AE_ask_dataset_ref_count(primaryJT,&referencenumberfoundJT));
												printf("\n JT found......%d",referencenumberfoundJT);fflush(stdout);
												for(jJT=0;jJT<referencenumberfoundJT;jJT++)
												{
													ITK_CALL(AE_find_dataset_named_ref(primaryJT,jJT,refnameJT,&reftypeJT,&refobjectJT));
													ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT));
													ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT_Tmp));
													printf("\n orig_nameJT is :%s  jJT Conut %d\n",orig_nameJT,jJT);
													tokenJT = strtok(orig_nameJT,".jt");
													printf("\n tokenJT %s tokenDI%s\n",tokenJT,tokenDI);
													if(tc_strcmp(tokenJT,tokenDI)==0)
													{

														fprintf(fdf,orig_nameJT_Tmp);  //23] JT/PDF             
														sprintf(pathnameJT,"%s%s/%s",FilePath,req_item,orig_nameJT_Tmp );
														printf("\n pathnameJT=%s\n",pathnameJT);
														IMF_export_file(refobjectJT,pathnameJT);
														
													}
												}
											}
										}

											fprintf(fdf,"~");
											print_DesignRevisionData(itemrev,fdf);
											
											//sprintf(pathname,"%s/%s/%s","/home/trlprod/TCUA/Downloader",req_item,orig_name );
											sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
											printf("\n pathname=%s\n",pathname);
											IMF_export_file(refobject,pathname);
										

							}
						}
					}
				}
				else if((!tc_strcmp(type_name,"ProDrw") )  && strstr(AppOwner,"Pro"))
				{
					CadCount++; 
					printf("\n 2] PrtDrwRev =%s\n",Item_ID_str);

					ITK_CALL(GRM_find_relation_type("Pro2_drawing_model",&reln_type_ProDrw)); 
					ITK_CALL(GRM_list_secondary_objects(primary,reln_type_ProDrw,&n_attchs_ProDrw,&rellist_ProDrw));
					printf("\n PRO Drw relation found <%d> \n",n_attchs_ProDrw);fflush(stdout);
											
					
					if (n_attchs_ProDrw>0)
					{
						fprintf(fPro,"ProDrwRel~");
						fprintf(fPro,Item_ID_str);
						fprintf(fPro,"~");
						fprintf(fPro,Item_Revision_str_O);
						fprintf(fPro,"~");
						fprintf(fPro,Item_ID_str);
						fprintf(fPro,"~");
						fprintf(fPro,Item_Revision_str_O);
						fprintf(fPro,"\n");
					}
					
					ITK_CALL(TCTYPE_ask_object_type(relationstr,&objTypeTag_rel));
					ITK_CALL(TCTYPE_ask_name(objTypeTag_rel,type_name_rel));

					printf("\n--------inside if of n attaches Pro Drw---------------------\n");fflush(stdout);

					ITK_CALL(AE_ask_dataset_id_rev(primary,&ret_id,&ret_rev));
					ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
					printf("\nreferencenumberfound =%d\n",referencenumberfound);

					for(j=0;j<referencenumberfound;j++)
					{
						printf("\n--------inside referencenumberfound---------------------\n");
						ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));

						if(!tc_strcmp(refname,"DrwFile"))
						{
							ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
							ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));
							printf("\n orig_name is :%s\n",orig_name);

							tokenDI = strtok(orig_name,".");
							printf("\ntokenDI Drawing Loop %s\n",tokenDI);

							if(tc_strcmp(type_name_rel,"IMAN_reference")!=0)
							{
								if(strstr(orig_name_Tmp,"pdf")==NULL)
								{

									ctr=int_ent_sequence+1;

									fprintf(fdf,Item_ID_str);					//1]  Part nos
									fprintf(fdf,"~");
									fprintf(fdf,projectcode);					//2] projectcode
									fprintf(fdf,"~");
									fprintf(fdf,desgngrp);						//3] desgngrp
									fprintf(fdf,"~");
									fprintf(fdf,Item_Description_str);			//4] Description
									fprintf(fdf,"~");
									fprintf(fdf,orig_name_Tmp);					//5] Cad data if no cad data then NULL is stamped
									fprintf(fdf,"~");
									fprintf(fdf,"NULL");						//6] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,Item_Revision_str_O);			//7] Rev and sequence of design revison 
									fprintf(fdf,"~");
									fprintf(fdf,PrtType);						//8] Part type of part 
									fprintf(fdf,"~");
									fprintf(fdf,VaultSts);							//9] Part type of part 
									fprintf(fdf,"~");
									fprintf(fdf,orig_name_Tmp);					//10] Cad data if no cad data then NULL is stamped
									fprintf(fdf,"~");
									fprintf(fdf,Item_Revision_str_O);			//11] Rev and sequence of design revison 
									fprintf(fdf,"~");
									fprintf(fdf,"NULL");						//12] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"NULL");						//13] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"NULL");						//14] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"NULL");						//15] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"NULL");						//16] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"NULL");						//17] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"0");							//18] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"Y");							//19] bom INDICATOR 
									fprintf(fdf,"~");
									fprintf(fdf,"-");							//20] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"-");							//21] ??????? 
									fprintf(fdf,"~");
									fprintf(fdf,"reptest#");					//22] ??????? 
							

									if(tc_strcmp(type_name_rel,"IMAN_reference")==0)
									{
										fprintf(fdf,Item_ID_str);
										fprintf(fdf,"#");
										fprintf(fdf,Item_Revision_str_O);

									}
									else
									{
										fprintf(fdf,"NULL#");
										fprintf(fdf,"NULL#");
										fprintf(fdf,"NULL");
									}
									fprintf(fdf,"~");
				
									for (iPdf= 0; iPdf < n_attchs; iPdf++)
									{
										printf("\n Inside loop Pdf .............. %d \n",iPdf);fflush(stdout);
										//primary=secondary_objects[i];
										primaryPdf=rellist[iPdf].secondary;
										//relationstrJT=rellist_JT[iJT].the_relation;

										ITK_CALL(TCTYPE_ask_object_type(primaryPdf,&objTypeTagPdf));
										ITK_CALL(TCTYPE_ask_name(objTypeTagPdf,type_name_Pdf));

										if(tc_strcmp(type_name_Pdf,"PDF")==0)
										{
											ITK_CALL(AE_ask_dataset_ref_count(primaryPdf,&referencenumberfoundPdf));
											printf("\n Pdf found......%d",referencenumberfoundPdf);fflush(stdout);
											for(jPdf=0;jPdf<referencenumberfoundPdf;jPdf++)
											{
												ITK_CALL(AE_find_dataset_named_ref(primaryPdf,jPdf,refnamePdf,&reftypePdf,&refobjectPdf));
												ITK_CALL( IMF_ask_original_file_name(refobjectPdf,orig_namePdf));
												ITK_CALL( IMF_ask_original_file_name(refobjectPdf,orig_namePdf_Tmp));
												printf("\n orig_namePdf is :%s  jPdf Count %d\n",orig_namePdf,jPdf);
												tokenPdf = strtok(orig_namePdf,".pdf");
												printf("\n tokenPdf %s tokenDI%s\n",tokenPdf,tokenDI);
												if(tc_strcmp(tokenPdf,tokenDI)==0)
												{
													fprintf(fdf,orig_namePdf_Tmp);   //23] JT/PDF             
													sprintf(pathnamePdf,"%s%s/%s",FilePath,req_item,orig_namePdf_Tmp );
													printf("\n pathnamePdf=%s\n",pathnamePdf);
													IMF_export_file(refobjectPdf,pathnamePdf);
												}
											}
										}
									}
			
									fprintf(fdf,"~");
									print_DesignRevisionData(itemrev,fdf);
									
									printf("\norig_name issssssss ---->%s\n",orig_name_Tmp);
									//sprintf(pathname,"%s/%s/%s","/home/trlprod/TCUA/Downloader",req_item,orig_name );
									sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
									printf("\npathname=%s\n",pathname);
									IMF_export_file(refobject,pathname);
								}
							}
						}
					}
				}
				else
				{
					printf("\n Attaches Is handled in program \n");
				}
			}

		}
		else
		{
			printf("\nThere are no data item attached to it \n");
			
		}     // need to comment till here if only module is required in cad output file 
		if (CadCount==0)
		{	
			fprintf(fdf,Item_ID_str);					//1]  Part nos                                    
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,projectcode);					//2] projectcode                                  
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,desgngrp);						//3] desgngrp                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,Item_Description_str);			//4] Description                                  
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"NULL");						//5] Cad data if no cad data then NULL is stamped 
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"NULL");						//6] ???????                                      
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,Item_Revision_str_O);			//7] Rev and sequence of design revison           
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,PrtType);						//8] Part type of part                            
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,VaultSts);							//9] Part type of part                            
			fprintf(fdf,"~");							                                                    
														//10] Cad data if no cad data then NULL is stamped
			fprintf(fdf,"~");							                                                    
									fprintf(fdf,"~");	//11] Rev ~ sequence of design revison          
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"NULL");						//12] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"NULL");						//13] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"NULL");						//14] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"NULL");						//15] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"NULL");						//16] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"NULL");						//17] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"0");							//18] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"Y"); 							//19] bom INDICATOR                               
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"-");							//20] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"-");							//21] ???????                                     
			fprintf(fdf,"~");							                                                    
			fprintf(fdf,"reptest#");					//22] ???????                                     
			fprintf(fdf,"NULL#");
			fprintf(fdf,"NULL#");
			fprintf(fdf,"NULL");
			fprintf(fdf,"~");
														//23] JT/PDF             
			fprintf(fdf,"");//vis file
			fprintf(fdf,"~");
			print_DesignRevisionData(itemrev,fdf);
			
		}
		if ( depth <= DepthInput)
		{
			ITK_CALL(BOM_line_ask_child_lines (line, &n, &children));
			for (k = 0; k < n; k++)
			{
				ITK_CALL(ITEM_ask_type2 (children[k], &item_type));
				printf("\n children item_typeS [%s] \n",item_type);  fflush(stdout);
				print_bom (children[k],line,depth,fdf,fus,fLhRh,fAttr,fPro,req_item,FilePath,DepthInput);
			}
		}
  }
LAST:
	printf("\n Inside Last\n");  fflush(stdout);
}
 //--------------------------------------------------------------------------------------------------------------------------
char* subString (char* mainStringf ,int fromCharf,int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(3);
	for(i=0; i < toCharf; i++ )
			  *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}

