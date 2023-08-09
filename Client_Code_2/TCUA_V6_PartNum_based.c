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
#include <string.h>

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
static void print_bom (tag_t line,tag_t Parent, int depth,FILE *fdf,FILE *fus,FILE *fL,FILE *fS,FILE *fPro,char *req_item,char* FilePath,int DepthInput,FILE *fmissingds);
static void print_data_item(tag_t this_itemRev,char *attr);
static void  getCadData(tag_t itemrev,FILE *fdf,FILE *fPro,char *FilePath,char *req_item,char *Item_ID_str,char *Item_Revision_str_O,FILE *fmissingds);
char* subString (char* mainStringf ,int fromCharf,int toCharf);

/*
int Complete_DML_Download(char* DmlIdS, char* filenameS)
{
	int		i,j					= 0;
	int		status				= 0;
	int		flag				= 0;
	int		n_tasks				= 0;
	int		n_parts				= 0;
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
	// Task Attributes
    char	*TaskItemIdS			= NULL;
    char	*TaskObjectDescS		= NULL;
    char	*DesignGroupS			= NULL;
    char	*TaskDespositionS		= NULL;
    char	*RequestorS				= NULL;
    char	*TaskAnalystUserIdS		= NULL;

    char	*PartNumberS			= NULL;
    char	*RevisionS				= NULL;
    char	*SequenceS				= NULL;

	tag_t dml_tag					= NULLTAG;
	tag_t dml_rev_tag				= NULLTAG;
	tag_t *task_obj_tag				= NULLTAG;
	tag_t *part_obj_tag				= NULLTAG;
	tag_t reln_type_tag				= NULLTAG;
	tag_t task_rev_tag				= NULLTAG;

	FILE	*DML_Download_fp		=	NULL;

	AllDmlAttrS = ( char * ) MEM_alloc(600);
	AllDmlAttrS=strcpy(AllDmlAttrS,"");

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
		ITK_CALL(ITEM_find_revision(dml_tag,"NR",&dml_rev_tag)) ;
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

			
			
			//if (AOM_ask_value_string(dml_rev_tag,"t5_basicdml",&t5_basicdmlS)!=ITK_ok)   PrintErrorStack();
			//printf("\n Complete_DML_Download : t5_basicdml : %s \n",t5_basicdmlS); fflush(stdout);
			//if (t5_basicdmlS)               
				//strcat(AllDmlAttrS,t5_basicdmlS);
			//strcat(AllDmlAttrS,",");


			//2
			if (AOM_ask_value_string(dml_rev_tag,"object_name",&object_nameS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : object_nameS : %s \n",object_nameS); fflush(stdout);
			if (object_nameS)               
				strcat(AllDmlAttrS,object_nameS);
			strcat(AllDmlAttrS,",");


			//3
			//if (AOM_ask_value_string(dml_rev_tag,"project_ids",&project_idsS)!=ITK_ok)   PrintErrorStack();
			if (AOM_ask_value_string(dml_rev_tag,"t5_cprojectcode",&project_idsS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : project_ids : %s \n",project_idsS); fflush(stdout);
			if (project_idsS)               
				strcat(AllDmlAttrS,project_idsS);
			strcat(AllDmlAttrS,",");			


			//4
			if (AOM_ask_value_string(dml_rev_tag,"t5designgroup",&t5designgroupS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : t5designgroup : %s \n",t5designgroupS); fflush(stdout);
			if (t5designgroupS)               
				strcat(AllDmlAttrS,t5designgroupS);
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
			//if (AOM_ask_value_string(dml_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
			//printf("\n Complete_DML_Download : ??? : %s \n",???); fflush(stdout);
			//if (t5_basicdmlS)               
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,"");  
			//fprintf(DML_Download_fp,",");

			//9 Creator Not Present in TCUA 
			//if (AOM_ask_value_string(dml_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
			//printf("\n Complete_DML_Download : ??? : %s \n",???); fflush(stdout);
			//if (t5_basicdmlS)               
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,"");  
			//fprintf(DML_Download_fp,",");


			//10
			if (AOM_ask_value_string(dml_rev_tag,"requestor_user_id",&requestor_user_idS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : requestor_user_id : %s \n",requestor_user_idS); fflush(stdout);
			if (requestor_user_idS)               
				strcat(AllDmlAttrS,requestor_user_idS);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,requestor_user_idS);  
			//fprintf(DML_Download_fp,",");

			//11
			if (AOM_ask_value_string(dml_rev_tag,"analyst_user_id",&analyst_user_idS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : analyst_user_id : %s \n",analyst_user_idS); fflush(stdout);
			if (analyst_user_idS)               
				strcat(AllDmlAttrS,analyst_user_idS);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,analyst_user_idS);  
			//fprintf(DML_Download_fp,",");

			//12 Administrator not present in TCUA
			//if (AOM_ask_value_string(dml_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
			//printf("\n Complete_DML_Download : ??? : %s \n",???); fflush(stdout);
			//if (t5_basicdmlS)               
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,"");  
			//fprintf(DML_Download_fp,",");

			//13 LCS not present in TCUA
			//if (AOM_ask_value_string(dml_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
			//printf("\n Complete_DML_Download : ??? : %s \n",???); fflush(stdout);
			//if (t5_basicdmlS)               
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,"");  
			//fprintf(DML_Download_fp,",");

			//14
			if (AOM_ask_value_string(dml_rev_tag,"release_status_list",&release_status_listS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : release_status_list : %s \n",release_status_listS); fflush(stdout);
			if (release_status_listS)               
				strcat(AllDmlAttrS,release_status_listS);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,release_status_listS);  
			//fprintf(DML_Download_fp,",");

			//15
			if (AOM_ask_value_string(dml_rev_tag,"owning_user",&owning_userS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : owning_user : %s \n",owning_userS); fflush(stdout);
			if (owning_userS)               
				strcat(AllDmlAttrS,owning_userS);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,owning_userS);  
			//fprintf(DML_Download_fp,",");
			
			//16 Planning Status not present in TCUA
			//if (AOM_ask_value_string(dml_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
			//printf("\n Complete_DML_Download : ??? : %s \n",???); fflush(stdout);
			//if (t5_basicdmlS)               
				strcat(AllDmlAttrS,"");
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,"");  
			//fprintf(DML_Download_fp,",");

			//17
			if (AOM_ask_value_string(dml_rev_tag,"CMClosure",&CMClosureS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : CMClosure : %s \n",CMClosureS); fflush(stdout);
			if (CMClosureS)               
				strcat(AllDmlAttrS,CMClosureS);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,CMClosureS);  
			//fprintf(DML_Download_fp,",");

			//18
			if (AOM_ask_value_string(dml_rev_tag,"CMClosureDate",&CMClosureDateS)!=ITK_ok)   PrintErrorStack();
			printf("\n Complete_DML_Download : CMClosureDate : %s \n",CMClosureDateS); fflush(stdout);
			if (CMClosureDateS)               
				strcat(AllDmlAttrS,CMClosureDateS);
			strcat(AllDmlAttrS,",");
			//fprintf(DML_Download_fp,CMClosureDateS);  
			//fprintf(DML_Download_fp,",");


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
						// 1
						if (AOM_ask_value_string(task_rev_tag,"item_id",&TaskItemIdS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev item_id : %s \n",TaskItemIdS); fflush(stdout);
						if (TaskItemIdS)               
							strcat(AllTaskAttrS,TaskItemIdS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,TaskItemIdS); 
						//fprintf(DML_Download_fp,"#");

						// 2
						if (AOM_ask_value_string(task_rev_tag,"object_desc",&TaskObjectDescS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev TaskObjectDescS : %s \n",TaskObjectDescS); fflush(stdout);
						if (TaskObjectDescS)               
							strcat(AllTaskAttrS,TaskObjectDescS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,TaskItemIdS);
						//fprintf(DML_Download_fp,"#");

						// 3
						if (AOM_ask_value_string(task_rev_tag,"t5designgroup",&DesignGroupS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev DesignGroupS : %s \n",TaskObjectDescS); fflush(stdout);
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
						strcat(AllTaskAttrS,",");
						//fprintf(DML_Download_fp,"");
						//fprintf(DML_Download_fp,"#");
						
						// 5 TaskStatus is not present in TCUA.
						//if (AOM_ask_value_string(task_rev_tag,"???",&???)!=ITK_ok)   PrintErrorStack();
						//printf("\n Complete_DML_Download : Task-Rev ???: %s \n",???); fflush(stdout);
						//if (CMClosureDateS)               
							strcat(AllTaskAttrS,"");
						strcat(AllTaskAttrS,",");
						//fprintf(DML_Download_fp,"");
						//fprintf(DML_Download_fp,"#");

						// 6 
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

						// 8 
						if (AOM_ask_value_string(task_rev_tag,"requestor_user_id",&RequestorS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev RequestorS : %s \n",RequestorS); fflush(stdout);
						if (RequestorS)               
							strcat(AllTaskAttrS,RequestorS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,RequestorS);
						//fprintf(DML_Download_fp,"#");

						// 9
						if (AOM_ask_value_string(task_rev_tag,"analyst_user_id",&TaskAnalystUserIdS)!=ITK_ok)   PrintErrorStack();
						printf("\n Complete_DML_Download : Task-Rev TaskAnalystUserIdS : %s \n",TaskAnalystUserIdS); fflush(stdout);
						if (TaskAnalystUserIdS)               
							strcat(AllTaskAttrS,TaskAnalystUserIdS);
						strcat(AllTaskAttrS,"#");
						//fprintf(DML_Download_fp,TaskAnalystUserIdS);
						//fprintf(DML_Download_fp,"#");

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
								if (AOM_ask_value_string(part_obj_tag[j],"sequence_id",&SequenceS)!=ITK_ok)   PrintErrorStack();
								printf("\n Complete_DML_Download : Task-Rev SequenceS : %s \n",SequenceS); fflush(stdout);
								fprintf(DML_Download_fp,SequenceS);
								fprintf(DML_Download_fp,"$");


								// 4 Effectivity Start Date is not present in TCUA
								//if (AOM_ask_value_string(part_obj_tag[j],"???",&???)!=ITK_ok)   PrintErrorStack();
								//printf("\n Complete_DML_Download : Task-Rev ??? : %s \n",???); fflush(stdout);
								ITK_CALL(WSOM_ask_release_status_list(part_obj_tag[j],&st_count1,&status_list1));
								printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);
								if(st_count1==1)
								{
									ITK_CALL(AOM_ask_name(status_list1[0],&WSO_Name));
									printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);
									if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0)
									{
										ITK_CALL(AOM_ask_value_string(status_list1[cnt],"effectivity_text",&effectivityStr));
										printf("\effectivityStr----><%s>\n",effectivityStr );
										fprintf(DML_Download_fp,effectivityStr);
										fprintf(DML_Download_fp,"$");
										// 5 Effectivity End Date is not present in TCUA
										//if (AOM_ask_value_string(part_obj_tag[j],"???",&???)!=ITK_ok)   PrintErrorStack();
										//printf("\n Complete_DML_Download : Task-Rev ??? : %s \n",???); fflush(stdout);
										fprintf(DML_Download_fp,"31-12-9999");
										fprintf(DML_Download_fp,"$");
									}
									else
									{
										fprintf(DML_Download_fp,"");
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,"");
										fprintf(DML_Download_fp,"$");
									}

								}
								else
								{
									fprintf(DML_Download_fp,"");
									fprintf(DML_Download_fp,"$");
									fprintf(DML_Download_fp,"");
									fprintf(DML_Download_fp,"$");
								}
								


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
*/
extern int ITK_user_main (int argc, char ** argv )
{
    int     status;
	char    *req_item,*FileDown,*FileUses = NULL,*PartNumberS=NULL;
	char    *FileDown2,*FileUses2 = NULL,*File_LhRh,*File_Weld,*FilePath,*File_Pro,*File_Dml,*DepthInputStr,*FileMissingDs;
	char    rev_id[ITEM_id_size_c+1];
	tag_t   window, window2, rule, item_tag = NULLTAG, top_line;
	FILE	*fd,*fu,*fLhRh,*fweld,*fpro,*fmissingds;
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
	FileMissingDs=(char *) MEM_alloc(200 * sizeof(char ));

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
	strcat(File_Weld,"_Weld.txt");
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

	strcpy(FileMissingDs,FilePath);
	strcat(FileMissingDs,req_item);
	strcat(FileMissingDs,"/");
	strcat(FileMissingDs,req_item);
	strcat(FileMissingDs,"_MissingDataset.txt");

	
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

	fweld=fopen(File_Weld,"w");
    if(fweld==NULL)
	{
		printf("\nError in opening the file \n");
	}

	fpro=fopen(File_Pro,"w");
    if(fpro==NULL)
	{
		printf("\nError in opening the file \n");
	}


	fmissingds=fopen(FileMissingDs,"w");
    if(fmissingds==NULL)
	{
		printf("\n Error in opening the file \n");
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
			if (tc_strcmp(item_type,"ChangeRequest")==0)
			{
				printf("\n\n Inside DML ! ! ! \n\n");  fflush(stdout);
				ITK_CALL(ITEM_find_revision(dml_tag,"NR",&dml_rev_tag)) ;
				if (dml_rev_tag  != NULLTAG)
				{
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

										tag_t *tags_found = NULL;
										int n_tags_found= 0;
										char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
										char **values = (char **) MEM_alloc(1 * sizeof(char *));
									
										attrs[0] ="item_id";
										values[0] = (char *)PartNumberS;
										
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
										ITK_CALL(CFM_find( "Latest by Creation Date", &rule ));
										ITK_CALL(BOM_set_window_config_rule( window, rule ));
										ITK_CALL(BOM_set_window_pack_all (window, true));
										ITK_CALL(BOM_set_window_top_line (window, item_tag, null_tag, null_tag, &top_line));
										print_bom (top_line,NULLTAG, 0,fd,fu,fLhRh,fweld,fpro,req_item,FilePath,DepthInput,fmissingds);
									}
								}
							}
						}
					}
				}
			}
			else if(tc_strcmp(item_type,"Design")==0)
			{
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
					ITK_CALL(CFM_find( "Latest by Creation Date", &rule ));
					ITK_CALL(BOM_set_window_config_rule( window, rule ));
					ITK_CALL(BOM_set_window_pack_all (window, true));
					ITK_CALL(BOM_set_window_top_line (window, item_tag, null_tag, null_tag, &top_line));
					print_bom (top_line,NULLTAG, 0,fd,fu,fLhRh,fweld,fpro,req_item,FilePath,DepthInput,fmissingds);
			}
		}

		fclose(fd);
		fclose(fu);
		fclose(fLhRh);
		fclose(fweld);
		fclose(fpro);
	}

	printf("\n MAIN FUNCTION : : FINISH\n");
	return status;
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
static void print_bom (tag_t line,tag_t Parent, int depth,FILE *fdf,FILE *fus,FILE *fLhRh,FILE *fWeld,FILE *fPro,char *req_item,char *FilePath,int DepthInput, FILE *fmissingds)
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
	char   *VerCreator=NULL;
	char   *Creator=NULL;
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
	char   *Weld_Item;
	char   pathname[SS_MAXPATHLEN + 1];
	char   pathnameJT[SS_MAXPATHLEN + 1];
	char   pathnamePdf[SS_MAXPATHLEN + 1];
	char   *ent_seq_str;
	char   *parentrev=NULL;
	char   *PrtType=NULL;
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
	char   *mcreator= NULL;
	char   *moddesc= NULL;
	char   *mcreatorName= NULL;
	char   *VCreator= NULL;
	char   *VCreatorName= NULL;
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
	char*  Bypass="5442T6B0197001,544263600116,544263600115,544263600115,12160500601";

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
	tag_t  t_ChildItemRev =NULLTAG;
	tag_t*    status_list1=NULLTAG;
	char *WSO_Name = NULL;
	GRM_relation_t *rellist, *rellistLH,*rellistWeld,*rellist_JT,*rellist_Pdf,*rellist_ProMem;
	AE_reference_type_t     reftype;
	AE_reference_type_t     reftypeJT;
	AE_reference_type_t     reftypePdf;
	tag_t *tags_Itm = NULL;
    int n_tags_found= 0;
	int cnt = 0;
	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));
	int ActualDepth=0;

	char   *t5DrawingInd= NULL;
	char   *t5MThickness= NULL;
	char   *t5Weight= NULL;
	char   *t5EnvelopeDimensions= NULL;
	char   *t5VolumeS= NULL;
	char   *t5SurfaceAreaS= NULL;
	char   *t5SurfPrtStdS= NULL;
	char   *t5_CategoryName= NULL;
	char   *t5Material= NULL;

	

	int iPdf=0,n_attchs_Pdf=0,jPdf=0,CadCount=0,st_count1=0,VaultStsFlagRel=0,VaultStsFlagWIP=0;
	
	
	depth ++;

	printf("\nprinting fdf depth after:%d  %d*****************************",depth,DepthInput);//fdf

    ITK_CALL( BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
	ITK_CALL(BOM_line_ask_attribute_string(line, Item_ID, &Item_ID_str));
	printf("\n1] Item_ID_str =%s\n",Item_ID_str);
	printf("\n1] Item_ID_strstr %s = %d\n",Bypass,tc_strstr(Item_ID_str,Bypass));
	if (tc_strstr(Bypass,Item_ID_str)!=NULL)
	{
		goto LAST;
	}
	attrs[0] ="item_id";
	values[0] = (char *)Item_ID_str;

	ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs,values, &n_tags_found, &tags_Itm));
	MEM_free(attrs);
	MEM_free(values);
	if (n_tags_found > 0)
	{
	
	item_tag = tags_Itm[0];
    MEM_free(tags_Itm);

	AOM_ask_value_string(item_tag,"t5_LeftRh",&LH_RH_Value_Item);

	if (strcmp(LH_RH_Value_Item,"LH CATIA") || strcmp(LH_RH_Value_Item,"RH CATIA") || strcmp(LH_RH_Value_Item,"RH PROE") || strcmp(LH_RH_Value_Item,"LH PROE"))
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
	STRNG_replace_str(Item_Revision_str,";",",",&Item_Revision_str_O);
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



    ITK_CALL(AOM_ask_value_int(itemrev,"sequence_id",&sequence_id_c));
	printf("\n3] Sequence =%d\n",sequence_id_c);
	ITemRevSeq = (char *) MEM_alloc( 5 * sizeof(char) );
	tokenDI = (char *) MEM_alloc( 200 * sizeof(char) );
	tokenJT = (char *) MEM_alloc( 200 * sizeof(char) );
	mcreator = (char *) MEM_alloc( 200 * sizeof(char) );
	mcreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	VCreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	VerCreator = (char *) MEM_alloc( 200 * sizeof(char) );
	sprintf(ITemRevSeq,"%d",sequence_id_c);

	t5DrawingInd = (char *) MEM_alloc( 200 * sizeof(char) );
	t5MThickness = (char *) MEM_alloc( 200 * sizeof(char) );
	t5Weight = (char *) MEM_alloc( 200 * sizeof(char) );
	t5EnvelopeDimensions = (char *) MEM_alloc( 200 * sizeof(char) );
	t5VolumeS = (char *) MEM_alloc( 200 * sizeof(char) );
	t5SurfaceAreaS = (char *) MEM_alloc( 200 * sizeof(char) );
	t5SurfPrtStdS = (char *) MEM_alloc( 200 * sizeof(char) );	
	t5_CategoryName = (char *) MEM_alloc( 200 * sizeof(char) );	
	t5Material = (char *) MEM_alloc( 200 * sizeof(char) );	

	ITK_CALL( BOM_line_look_up_attribute ("bl_rev_object_desc",&Item_Description));
	ITK_CALL(BOM_line_ask_attribute_string(line, Item_Description, &Item_Description_str));
	printf("\n4] Item_Description_str =%s\n",Item_Description_str);

	ITK_CALL(AOM_UIF_ask_value(itemrev,"object_desc",&t5_CategoryName));		//*******************************Test	
	ITK_CALL(AOM_UIF_ask_value(itemrev,"t5_PartType",&PrtType));			
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_ProjectCode",&projectcode));
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_DesignGrp",&desgngrp));
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_Material",&t5Material));
	ITK_CALL(AOM_UIF_ask_value(itemrev,"owning_user",&Creator)); //Creator

	ITK_CALL(AOM_ask_value_string(itemrev,"t5_DocRemarks",&moddesc));  //modification desc 
	ITK_CALL ( AOM_ask_value_string(itemrev,"t5_DrawingInd",&t5DrawingInd));
	ITK_CALL ( AOM_ask_value_string(itemrev,"t5_MThickness",&t5MThickness));
	ITK_CALL ( AOM_ask_value_string(itemrev,"t5_Weight",&t5Weight));
	ITK_CALL ( AOM_ask_value_string(itemrev,"t5_EnvelopeDimensions",&t5EnvelopeDimensions));
	ITK_CALL ( AOM_ask_value_string(itemrev,"t5_Volume",&t5VolumeS));
	ITK_CALL ( AOM_ask_value_string(itemrev,"t5_SurfaceArea",&t5SurfaceAreaS));
	ITK_CALL ( AOM_ask_value_string(itemrev,"t5_SurfPrtStd",&t5SurfPrtStdS));
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_VerCreator",&VCreator));  //Version Creator



	ITK_CALL(WSOM_ask_release_status_list(itemrev,&st_count1,&status_list1));
	printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);

	if(st_count1==1)
	{
		ITK_CALL(AOM_ask_name(status_list1[0],&WSO_Name));
		printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);
		if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0)
		{
			VaultSts="Released";
			ITK_CALL(AOM_ask_value_string(status_list1[cnt],"effectivity_text",&effectivityStr));
			printf("\effectivityStr----><%s>\n",effectivityStr );
		}
		else if (tc_strcmp(WSO_Name,"T5_LcsReview")==0)
		{
			VaultSts="CE Vault";
		}
		else
		{
			VaultSts="CE Vault";
		}
	}
	else if(st_count1>1)
	{
		for (cnt=0;cnt< st_count1;cnt++ )
		{
			ITK_CALL(AOM_ask_name(status_list1[cnt],&WSO_Name));
			printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);  //T5_LcsReview //ERC Released or T5_LcsErcRlzd// Released
			if (tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0)
			{
				VaultStsFlagRel=1;
				ITK_CALL(AOM_ask_value_string(status_list1[cnt],"effectivity_text",&effectivityStr));
				printf("\effectivityStr----><%s>\n",effectivityStr );
			}
			else if (tc_strcmp(WSO_Name,"T5_LcsReview")==0)
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
			VaultSts="CE Vault";
		}
		else
		{
			VaultSts="CE Vault";
		}
	}

	
	mcreatorName = strtok(Creator,"(");
	mcreator = strtok(NULL,")");


	VCreatorName = strtok(VCreator,"(");
	VerCreator = strtok(NULL,")");



	printf("\n       PrtTypeAtr----><%s>\n",PrtType );
	printf("\n projectcode------->%s\n",projectcode);
	printf("\n desgngrp------->%s\n",desgngrp);
	printf("\n mcreatorName------->%s\n",mcreatorName);
	printf("\n creator------->%s\n",mcreator);
	printf("\n VerCreator------->%s\n",VerCreator);

	ITK_CALL(AOM_ask_value_string(itemrev,"gov_classification",&enterprise_sequence));
    printf("\n -----------------------enterprise_sequence -->%s\n",enterprise_sequence);

	int_ent_sequence=atoi(enterprise_sequence);

	//dumy_Item_ID_str=MEM_string_copy(Item_ID_str) ;




	if (Parent == null_tag)
	{
		printf("\nThis is first line\n");
		Parent=line;
	}
	//else
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

		ITK_CALL (TCTYPE_ask_object_type(itemrev,&objTypeTag));
		ITK_CALL (TCTYPE_ask_name(objTypeTag,type_name2));
		printf("\n Started for type_nameee allow only for selected class  = %s\n", type_name);fflush(stdout);

		STRNG_replace_str(parentrev,";",",",&parentrev_O);
		STRNG_replace_str(TransforationMatrix," ",",",&TransforationMatrix_O);
		printf("\n2.1] parentrev =%s\n",parentrev_O);
		printf("\n2.1] parentrev =%s\n",TransforationMatrix_O);
		//itoa(DepthInput, DepthStr, 10);
		//printf("\nDepth String=%s",DepthStr);
		if(strcmp(type_name2,"Design Revision")==0)
		{	
			ActualDepth=depth-1;
			fprintf(fus,"%d",ActualDepth);
			fprintf(fus,",");
			fprintf(fus,Item_ID_str);
			fprintf(fus,",");
			fprintf(fus,Item_Revision_str_O1);
			fprintf(fus,",");
			fprintf(fus,t5_CategoryName);
			fprintf(fus,",");
			if(strlen(Quantity)>0)
			{
				fprintf(fus,Quantity);
			}
			else
			{
				fprintf(fus,"-");
			}
//			fprintf(fus,",");
//			fprintf(fus,ITemRevSeq);
			fprintf(fus,",");
			fprintf(fus,PrtType);
			fprintf(fus,",");
			fprintf(fus,VaultSts);
			fprintf(fus,",");
			fprintf(fus,projectcode);
			fprintf(fus,",");
			fprintf(fus,desgngrp);
			fprintf(fus,",");
			fprintf(fus,t5DrawingInd);
			fprintf(fus,",");
			fprintf(fus,t5MThickness);
			fprintf(fus,",");
			fprintf(fus,t5Weight);
			fprintf(fus,",");
			fprintf(fus,t5EnvelopeDimensions);
			fprintf(fus,",");
			fprintf(fus,t5VolumeS);
			fprintf(fus,",");
			fprintf(fus,t5SurfaceAreaS);
			fprintf(fus,",");
			fprintf(fus,t5SurfPrtStdS);
			fprintf(fus,",");
			fprintf(fus,VCreator);
			fprintf(fus,",");
			fprintf(fus,t5Material);
			fprintf(fus,",");
		//	fprintf(fus,"reptest");
			fprintf(fus,"\n");


		



		}
	}

	//ITK_CALL(GRM_list_secondary_objects_only(itemrev,reln_type,&n_attchs,&secondary_objects));
	getCadData( itemrev,fdf,fPro,FilePath,req_item,Item_ID_str,Item_Revision_str_O,fmissingds);
	if ( depth <= DepthInput)
	{
		ITK_CALL(BOM_line_ask_child_lines (line, &n, &children));
		for (k = 0; k < n; k++)
		{
			ITK_CALL(ITEM_ask_type2 (children[k], &item_type));
			printf("\n children item_typeS [%s] \n",item_type);  fflush(stdout);
			print_bom (children[k],line,depth,fdf,fus,fLhRh,fWeld,fPro,req_item,FilePath,DepthInput,fmissingds);
		}
	}
  }
LAST:
	printf("\n Inside Last\n");  fflush(stdout);
}



static void  getCadData(tag_t itemrev,FILE *fdf,FILE *fPro,char *FilePath,char *req_item,char *Item_ID_str,char *Item_Revision_str_O,FILE *fmissingds)
{
    int    status;
	int    k, n,int_ent_sequence;
//	int    Item_ID,Item_Revision,Item_Description,parent_item_seq=0,QuantityAttrId,TransforationMatrixAttrId,Item_WeldPrts,PrtTypeAtr,iChildItemTag;
	int    n_attchs,i,referencenumberfound,j,referencenumberfoundJT,jJT,n_attchsLH,n_attchsWeld,iWeld,n_attchs_JT,referencenumberfoundPdf,n_attchs_ProMem, iProAsm;
	int    numBoundingBoxes,ctr;
	int    Parentsequence_id=0;
	int    dum=0;

	double *  boundingBoxes;
	char   *name,*Item_Revision_str,*Item_Description_str,*Item_WeldPrts_str,*ProAsmItemName;
	char   *dumy_Item_ID_str=NULL;
	char   *catiafileName=NULL;
	char   *projectcode=NULL;
	char   *projectcode2=NULL;
	char   *desgngrp=NULL;
	char   *VerCreator=NULL;
	char   *Creator=NULL;
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
	char   *Weld_Item;
	char   pathname[SS_MAXPATHLEN + 1];
	char   pathnameJT[SS_MAXPATHLEN + 1];
	char   pathnamePdf[SS_MAXPATHLEN + 1];
	char   *ent_seq_str;
	char   *parentrev=NULL;
	char   *PrtType=NULL;
	char   *parent=NULL;
	char   *Quantity=NULL;
	char   *TransforationMatrix=NULL;
	char   *TransforationMatrix_O=NULL;
	char   *parentseq=NULL;
	char   *parentdesc=NULL;
	char   *PatSeq= NULL;
	char   *Item_Revision_str_O1= NULL;
	char   *parentrev_O= NULL;
	char   *tokenDI= NULL;
	char   *tokenJT= NULL;
	char   *mcreator= NULL;
	char   *moddesc= NULL;
	char   *mcreatorName= NULL;
	char   *VCreator= NULL;
	char   *VCreatorName= NULL;
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


	tag_t  objTypeTag_rel =NULLTAG;
	tag_t  ParentItemRevTag =NULLTAG;
	tag_t  *secondary_objects,primary,objTypeTag,refobject=NULLTAG,primaryJT,objTypeTagJT,refobjectJT,primaryRH,relationstrRH,primaryWeld,primaryPdf,objTypeTagPdf,refobjectPdf,primaryAsm;
	tag_t  *children,sequence_id_c;
	tag_t  reln_type =NULLTAG;
	tag_t  reln_type_Render =NULLTAG;
	tag_t  reln_typeLH =NULLTAG;
	tag_t  reln_typeWeld =NULLTAG;
	tag_t  relationstr =NULLTAG;
	tag_t  relationstrJT =NULLTAG;
	tag_t  item_tag =NULLTAG;
	tag_t  reln_type_ProMem =NULLTAG;
	tag_t  t_ChildItemRev =NULLTAG;
	tag_t*    status_list1=NULLTAG;
	char *WSO_Name = NULL;
	GRM_relation_t *rellist, *rellistLH,*rellistWeld,*rellist_JT,*rellist_Pdf,*rellist_ProMem;
	AE_reference_type_t     reftype;
	AE_reference_type_t     reftypeJT;
	AE_reference_type_t     reftypePdf;
	tag_t *tags_Itm = NULL;
    int n_tags_found= 0;
	int cnt = 0;
int iPdf=0,n_attchs_Pdf=0,jPdf=0,CadCount=0,st_count1=0,VaultStsFlagRel=0,VaultStsFlagWIP=0,cadAvblFlag=0;

ITK_CALL(GRM_list_secondary_objects(itemrev,reln_type,&n_attchs,&rellist));
	printf("\nTotal n attches to %s=%d\n",Item_ID_str,n_attchs);
	if(n_attchs>0)
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
			if(strcmp(type_name,"CATProduct")==0 ||strcmp(type_name,"CMI2Product")==0 || strcmp(type_name,"t5Alias")==0 || strcmp(type_name,"CATPart")==0 || strcmp(type_name,"CMI2Part")==0 || strcmp(type_name,"CMI2AuxPart")==0)
			{
				CadCount++;
				cadAvblFlag=1;
				printf("\n--------inside if of n attaches Catia Product CAt part---------------------\n");fflush(stdout);
				ITK_CALL(AE_ask_dataset_id_rev(primary,&ret_id,&ret_rev));
				ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
				printf("\nreferencenumberfound =%d\n",referencenumberfound);

				for(j=0;j<referencenumberfound;j++)
				{
					printf("\n -------- inside referencenumberfound --------------------- \n");
					ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
					printf("\n ---test for FSC proxy error----- :%s\n",refname);
					if(strcmp(refname,"CMIAuxFileInfo") && strcmp(refname,"CMI2BBInfo") && strcmp(refname,"CMI2TypeInfo"))
					{
						ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
						ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));

						printf("\n orig_name is :%s\n",orig_name);

						if(strstr(orig_name,"jt")==NULL)
						{
							printf("\nenterprise %d \n",int_ent_sequence);
							printf("\nua %d\n",sequence_id_c);
							tokenDI = (char *) MEM_alloc( 200 * sizeof(char) );
							tokenDI = strtok(orig_name,".");
							printf("\ntokenDI %s\n",tokenDI);
	//						if(int_ent_sequence<sequence_id_c)
	//						{
								//ctr=int_ent_sequence+1;
	//							for (ctr;ctr<=sequence_id_c ;ctr++ )
	//							{
									//dum=ctr;
									//ent_seq_str = (char *) MEM_alloc( 5 * sizeof(char) );
									//sprintf(ent_seq_str,"%d",dum);

									printf("\nNot in spec loop %s\n",Item_ID_str);									
									fprintf(fdf,Item_ID_str);						/*1]  Part nos*/                                    
									fprintf(fdf,",");
									fprintf(fdf,Item_Revision_str_O);				/*7] Rev and sequence of design revison */          
									fprintf(fdf,",");
									fprintf(fdf,orig_name_Tmp);						/*5] Cad data if no cad data then NULL is stamped*/ 
									fprintf(fdf,",");
									fprintf(fdf,type_name);						
									fprintf(fdf,",");
//
//									fprintf(fdf,projectcode);						/*2] projectcode*/                                  
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,desgngrp);							/*3] desgngrp*/                                     
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,Item_Description_str);				/*4] Description*/                                  
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,orig_name_Tmp);						/*5] Cad data if no cad data then NULL is stamped*/ 
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"NULL");							/*6] ??????? */                                     
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,Item_Revision_str_O);				/*7] Rev and sequence of design revison */          
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,PrtType);							/*8] Part type of part */                           
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,VaultSts);								/*9] Part type of part */                           
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,orig_name_Tmp);						/*10] Cad data if no cad data then NULL is stamped*/
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,Item_Revision_str_O);				/*11] Rev and sequence of design revison */         
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"NULL");							/*12] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"NULL");							/*13] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"NULL");							/*14] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"NULL");							/*15] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"NULL");							/*16] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"NULL");							/*17] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"0");								/*18] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"Y");								/*19] bom INDICATOR */                              
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"-");								/*20] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"-");								/*21] ??????? */                                    
//									fprintf(fdf,",");								                                                    
//									fprintf(fdf,"reptest#");						/*22] ??????? */                                    
//									fprintf(fdf,"NULL#");
//									fprintf(fdf,"NULL#");
//									fprintf(fdf,"NULL");
//									fprintf(fdf,",");
								
				
									printf("\norig_name issssssss 11---->%s\n",orig_name);
//									if (!strstr(orig_name,"Spec"))
//									{
//										for (iJT= 0; iJT < n_attchs_JT; iJT++)
//										{
//											printf("\n Inside loop JT .............. %d \n",iJT);fflush(stdout);
//											//primary=secondary_objects[i];
//											primaryJT=rellist_JT[iJT].secondary;
//											//relationstrJT=rellist_JT[iJT].the_relation;
//
//											ITK_CALL(TCTYPE_ask_object_type(primaryJT,&objTypeTagJT));
//											ITK_CALL(TCTYPE_ask_name(objTypeTagJT,type_name_JT));
//
//											if(strcmp(type_name_JT,"DirectModel")==0 /*&& !strcmp(PrtType,"Component")*/)
//											{
//												
//												ITK_CALL(AE_ask_dataset_ref_count(primaryJT,&referencenumberfoundJT));
//												printf("\n JT found......%d",referencenumberfoundJT);fflush(stdout);
//												for(jJT=0;jJT<referencenumberfoundJT;jJT++)
//												{
//													ITK_CALL(AE_find_dataset_named_ref(primaryJT,jJT,refnameJT,&reftypeJT,&refobjectJT));
//													ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT));
//													ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT_Tmp));
//													printf("\n orig_nameJT is :%s  jJT Conut %d\n",orig_nameJT,jJT);
//													tokenJT = strtok(orig_nameJT,".jt");
//													printf("\n tokenJT %s tokenDI%s\n",tokenJT,tokenDI);
//													if(strcmp(tokenJT,tokenDI)==0)
//													{
//														//fprintf(fdf,orig_nameJT_Tmp);
//														sprintf(pathnameJT,"%s%s/%s",FilePath,req_item,orig_nameJT_Tmp );
//														printf("\n pathnameJT=%s\n",pathnameJT);
//														//IMF_export_file(refobjectJT,pathnameJT);
//														
//													}
//												}
//											}
//										}
//									}
									/*fprintf(fdf,",");
									fprintf(fdf,mcreator);
									fprintf(fdf,",");
									fprintf(fdf,VerCreator);*/
									fprintf(fdf,",");
									fprintf(fdf,"\n");
									//sprintf(pathname,"%s/%s/%s","/home/trlprod/TCUA/Downloader",req_item,orig_name );
									sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
									printf("\n pathname=%s\n",pathname);
									IMF_export_file(refobject,pathname);
									
	//							}
	//						}
						}
					}
				}
			}
			else if(strcmp(type_name,"CATDrawing")==0||strcmp(type_name,"CMI2Drawing")==0)
			{
				CadCount++;
				cadAvblFlag=1;
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

					ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
					ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));
					printf("\n orig_name is :%s\n",orig_name);

					tokenDI = strtok(orig_name,".");
					printf("\ntokenDI Drawing Loop %s\n",tokenDI);

					if(strcmp(type_name_rel,"IMAN_reference")!=0)
					{
						if(strstr(orig_name_Tmp,"pdf")==NULL)
						{
//							printf("\nenterprise %d \n",int_ent_sequence);
//							printf("\nua %d\n",sequence_id_c);

//							if(int_ent_sequence<sequence_id_c)
//							{
								ctr=int_ent_sequence+1;
//								for (ctr;ctr<=sequence_id_c ;ctr++ )
//								{
//									dum=ctr;
//									ent_seq_str = (char *) MEM_alloc( 5 * sizeof(char) );
//									sprintf(ent_seq_str,"%d",dum);

									printf("\nNot in spec loop  %s\n",Item_ID_str);

									fprintf(fdf,Item_ID_str);					/*1]  Part nos*/                                    
									fprintf(fdf,",");	
									fprintf(fdf,Item_Revision_str_O);			/*7] Rev and sequence of design revison */          
									fprintf(fdf,",");
									fprintf(fdf,orig_name_Tmp);					/*] Cad data if no cad data then NULL is stamped*/ 
									fprintf(fdf,",");
									fprintf(fdf,type_name);						
									fprintf(fdf,",");
	
									
//									fprintf(fdf,projectcode);					/*2] projectcode*/                                  
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,desgngrp);						/*3] desgngrp*/                                     
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,Item_Description_str);			/*4] Description*/                                  
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,orig_name_Tmp);					/*5] Cad data if no cad data then NULL is stamped*/ 
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"NULL");						/*6] ??????? */                                     
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,Item_Revision_str_O);			/*7] Rev and sequence of design revison */          
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,PrtType);						/*8] Part type of part */                           
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,VaultSts);							/*9] Part type of part */                           
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,orig_name_Tmp);					/*10] Cad data if no cad data then NULL is stamped*/
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,Item_Revision_str_O);			/*11] Rev and sequence of design revison */         
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"NULL");						/*12] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"NULL");						/*13] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"NULL");						/*14] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"NULL");						/*15] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"NULL");						/*16] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"NULL");						/*17] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"0");							/*18] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"Y");							/*19] bom INDICATOR */                              
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"-");							/*20] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"-");							/*21] ??????? */                                    
//									fprintf(fdf,",");							                                                    
//									fprintf(fdf,"reptest#");					/*22] ??????? */                                    
//									fprintf(fdf,",");

								/*	if(strcmp(type_name_rel,"IMAN_reference")==0)
									{
										fprintf(fdf,Item_ID_str);
										fprintf(fdf,"#");
										fprintf(fdf,Item_Revision_str_O);
//										fprintf(fdf,"#");
//										fprintf(fdf,ent_seq_str);
									}
									else
									{
										fprintf(fdf,"NULL#");
										fprintf(fdf,"NULL#");
										fprintf(fdf,"NULL");
									}
									*/

									for (iPdf= 0; iPdf < n_attchs; iPdf++)
									{
										printf("\n Inside loop Pdf .............. %d \n",iPdf);fflush(stdout);
										//primary=secondary_objects[i];
										primaryPdf=rellist[iPdf].secondary;
										//relationstrJT=rellist_JT[iJT].the_relation;

										ITK_CALL(TCTYPE_ask_object_type(primaryPdf,&objTypeTagPdf));
										ITK_CALL(TCTYPE_ask_name(objTypeTagPdf,type_name_Pdf));

										if(strcmp(type_name_Pdf,"PDF")==0)
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
												if(strcmp(tokenPdf,tokenDI)==0)
												{
													//fprintf(fdf,orig_namePdf_Tmp);    /*23] JT/PDF */            
													sprintf(pathnamePdf,"%s%s/%s",FilePath,req_item,orig_namePdf_Tmp );
													printf("\n pathnamePdf=%s\n",pathnamePdf);
													//IMF_export_file(refobjectPdf,pathnamePdf);
													
												}
											}
										}
									}

									/*fprintf(fdf,",");
									fprintf(fdf,mcreator);
									fprintf(fdf,",");
									fprintf(fdf,VerCreator);*/
									fprintf(fdf,",");
									fprintf(fdf,"\n");
									printf("\norig_name issssssss ---->%s\n",orig_name_Tmp);
									//sprintf(pathname,"%s/%s/%s","/home/trlprod/TCUA/Downloader",req_item,orig_name );
									sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
									printf("\npathname=%s\n",pathname);
									IMF_export_file(refobject,pathname);
									
//								}
//							}
						}
					}
				}
			}
			else if(strcmp(type_name,"ProAsm")==0 ||strcmp(type_name,"ProPrt")==0 )
			{
				printf("\n--------inside if of n attaches Pro part Asm---------------------\n");fflush(stdout);
				CadCount++;
				cadAvblFlag=1;

				ITK_CALL(GRM_find_relation_type("Pro2_membership",&reln_type_ProMem));
				ITK_CALL(GRM_list_secondary_objects(primary,reln_type_ProMem,&n_attchs_ProMem,&rellist_ProMem));
				printf("\n PRO ASM relation found <%d> \n",n_attchs_ProMem);fflush(stdout);

				for (iProAsm=0;iProAsm<n_attchs_ProMem;iProAsm++)  
				{
					primaryAsm=rellist_ProMem[iProAsm].secondary;
					AOM_ask_value_string(primaryAsm,"item_id",&ProAsmItemName);
					printf("\n ProAsmItemName =%s\n",ProAsmItemName);fflush(stdout);
					fprintf(fPro,Item_ID_str);
					fprintf(fPro,",");
					fprintf(fPro,Item_Revision_str_O);
					fprintf(fPro,",");
					fprintf(fPro,ProAsmItemName);
					fprintf(fPro,",");
					ITK_CALL(AOM_ask_value_string(primaryAsm,"item_revision_id",&PrtAsmRev))
					printf("\n2] PrtAsmRev =%s\n",PrtAsmRev);
					STRNG_replace_str(PrtAsmRev,";",",",&PrtAsmRev_O);
					fprintf(fPro,PrtAsmRev_O);
					fprintf(fPro,"\n");
				}

				ITK_CALL(AE_ask_dataset_id_rev(primary,&ret_id,&ret_rev));
				ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
				printf("\nreferencenumberfound =%d\n",referencenumberfound);

				for(j=0;j<referencenumberfound;j++)
				{
					printf("\n -------- inside referencenumberfound --------------------- \n");
					ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
					printf("\n ---test for FSC proxy error----- :%s\n",refname);
					if(!strcmp(refname,"AsmFile")  || !strcmp(refname,"PrtFile"))
					{
						ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
						ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));

						printf("\n orig_name is :%s\n",orig_name);

//						if(strstr(orig_name,"jt")==NULL)
//						{
//							printf("\nenterprise %d \n",int_ent_sequence);
//							printf("\nua %d\n",sequence_id_c);
//
//							tokenDI = strtok(orig_name,".");
//							printf("\ntokenDI %s\n",tokenDI);
//	//						if(int_ent_sequence<sequence_id_c)
//	//						{
//								ctr=int_ent_sequence+1;
//	//							for (ctr;ctr<=sequence_id_c ;ctr++ )
//	//							{
////									dum=ctr;
////									ent_seq_str = (char *) MEM_alloc( 5 * sizeof(char) );
////									sprintf(ent_seq_str,"%d",dum);
//
//									printf("\nNot in spec loop %s\n",Item_ID_str);
//
//									fprintf(fdf,Item_ID_str);						/*1]  Part nos*/                                    
//									fprintf(fdf,",");	
//									fprintf(fdf,Item_Revision_str_O);				/*] Rev and sequence of design revison */          
//									fprintf(fdf,",");
//									fprintf(fdf,orig_name_Tmp);						/*] Cad data if no cad data then NULL is stamped*/ 
//									fprintf(fdf,",");
////									fprintf(fdf,projectcode);						/*2] projectcode*/                                  
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,desgngrp);							/*3] desgngrp*/                                     
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,Item_Description_str);				/*4] Description*/                                  
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,orig_name_Tmp);						/*5] Cad data if no cad data then NULL is stamped*/ 
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"NULL");							/*6] ??????? */                                     
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,Item_Revision_str_O);				/*7] Rev and sequence of design revison */          
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,PrtType);							/*8] Part type of part */                           
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,VaultSts);								/*9] Part type of part */                           
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,orig_name_Tmp);						/*10] Cad data if no cad data then NULL is stamped*/
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,Item_Revision_str_O);				/*11] Rev and sequence of design revison */         
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"NULL");							/*12] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"NULL");							/*13] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"NULL");							/*14] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"NULL");							/*15] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"NULL");							/*16] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"NULL");							/*17] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"0");								/*18] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"Y");								/*19] bom INDICATOR */                              
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"-");								/*20] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"-");								/*21] ??????? */                                    
////									fprintf(fdf,",");								                                                    
////									fprintf(fdf,"reptest#");						/*22] ??????? */                                    
////									fprintf(fdf,"NULL#");
////									fprintf(fdf,"NULL#");
////									fprintf(fdf,"NULL");
////									fprintf(fdf,",");
//									
//				
//									printf("\norig_name issssssss 11---->%s\n",orig_name);
////									for (iJT= 0; iJT < n_attchs_JT; iJT++)
////									{
////										printf("\n Inside loop JT .............. %d \n",iJT);fflush(stdout);
////										//primary=secondary_objects[i];
////										primaryJT=rellist_JT[iJT].secondary;
////										//relationstrJT=rellist_JT[iJT].the_relation;
////
////										ITK_CALL(TCTYPE_ask_object_type(primaryJT,&objTypeTagJT));
////										ITK_CALL(TCTYPE_ask_name(objTypeTagJT,type_name_JT));
////
////										if(strcmp(type_name_JT,"DirectModel")==0 && !strcmp(PrtType,"C"))
////										{
////											
////											ITK_CALL(AE_ask_dataset_ref_count(primaryJT,&referencenumberfoundJT));
////											printf("\n JT found......%d",referencenumberfoundJT);fflush(stdout);
////											for(jJT=0;jJT<referencenumberfoundJT;jJT++)
////											{
////												ITK_CALL(AE_find_dataset_named_ref(primaryJT,jJT,refnameJT,&reftypeJT,&refobjectJT));
////												ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT));
////												ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT_Tmp));
////												printf("\n orig_nameJT is :%s  jJT Conut %d\n",orig_nameJT,jJT);
////												tokenJT = strtok(orig_nameJT,".jt");
////												printf("\n tokenJT %s tokenDI%s\n",tokenJT,tokenDI);
////												if(strcmp(tokenJT,tokenDI)==0)
////												{
////
////													//fprintf(fdf,orig_nameJT_Tmp);  /*23] JT/PDF */            
////													sprintf(pathnameJT,"%s%s/%s",FilePath,req_item,orig_nameJT_Tmp );
////													printf("\n pathnameJT=%s\n",pathnameJT);
////													//IMF_export_file(refobjectJT,pathnameJT);
////													
////												}
////											}
////										}
////									}
//
//										/*fprintf(fdf,",");
//										fprintf(fdf,mcreator);
//										fprintf(fdf,",");
//										fprintf(fdf,VerCreator);*/
//										fprintf(fdf,",");
//										fprintf(fdf,"\n");
//
//										//sprintf(pathname,"%s/%s/%s","/home/trlprod/TCUA/Downloader",req_item,orig_name );
//										sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
//										printf("\n pathname=%s\n",pathname);
//										IMF_export_file(refobject,pathname);
//									
//	//							}
//	//						}
//						}
					}
				}
			}
			else if(!strcmp(type_name,"ProDrw"))
			{
				CadCount++;
				cadAvblFlag=1;
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

					if(!strcmp(refname,"DrwFile"))
					{
						ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
						ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));
						printf("\n orig_name is :%s\n",orig_name);

						tokenDI = strtok(orig_name,".");
						printf("\ntokenDI Drawing Loop %s\n",tokenDI);

						if(strcmp(type_name_rel,"IMAN_reference")!=0)
						{
							if(strstr(orig_name_Tmp,"pdf")==NULL)
							{
	//							printf("\nenterprise %d \n",int_ent_sequence);
	//							printf("\nua %d\n",sequence_id_c);

	//							if(int_ent_sequence<sequence_id_c)
	//							{
									ctr=int_ent_sequence+1;
	//								for (ctr;ctr<=sequence_id_c ;ctr++ )
	//								{
//										dum=ctr;
//										ent_seq_str = (char *) MEM_alloc( 5 * sizeof(char) );
//										sprintf(ent_seq_str,"%d",dum);


										fprintf(fdf,Item_ID_str);					/*1]  Part nos*/										
										fprintf(fdf,",");
										fprintf(fdf,Item_Revision_str_O);			/*7] Rev and sequence of design revison */
										fprintf(fdf,",");
//										fprintf(fdf,projectcode);					/*2] projectcode*/
//										fprintf(fdf,",");
//										fprintf(fdf,desgngrp);						/*3] desgngrp*/
//										fprintf(fdf,",");
//										fprintf(fdf,Item_Description_str);			/*4] Description*/
//										fprintf(fdf,",");
										fprintf(fdf,orig_name_Tmp);					/*5] Cad data if no cad data then NULL is stamped*/
										fprintf(fdf,",");
										fprintf(fdf,type_name);						
										fprintf(fdf,",");
//										fprintf(fdf,"NULL");						/*6] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,Item_Revision_str_O);			/*7] Rev and sequence of design revison */
//										fprintf(fdf,",");
//										fprintf(fdf,PrtType);						/*8] Part type of part */
//										fprintf(fdf,",");
//										fprintf(fdf,VaultSts);							/*9] Part type of part */
//										fprintf(fdf,",");
//										fprintf(fdf,orig_name_Tmp);					/*10] Cad data if no cad data then NULL is stamped*/
//										fprintf(fdf,",");
//										fprintf(fdf,Item_Revision_str_O);			/*11] Rev and sequence of design revison */
//										fprintf(fdf,",");
//										fprintf(fdf,"NULL");						/*12] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"NULL");						/*13] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"NULL");						/*14] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"NULL");						/*15] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"NULL");						/*16] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"NULL");						/*17] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"0");							/*18] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"Y");							/*19] bom INDICATOR */
//										fprintf(fdf,",");
//										fprintf(fdf,"-");							/*20] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"-");							/*21] ??????? */
//										fprintf(fdf,",");
//										fprintf(fdf,"reptest#");					/*22] ??????? */
//										fprintf(fdf,",");

											
/*
											if(strcmp(type_name_rel,"IMAN_reference")==0)
											{
												fprintf(fdf,Item_ID_str);
												fprintf(fdf,"#");
												fprintf(fdf,Item_Revision_str_O);
		//										fprintf(fdf,"#");
		//										fprintf(fdf,ent_seq_str);
											}
											else
											{
												fprintf(fdf,"NULL#");
												fprintf(fdf,"NULL#");
												fprintf(fdf,"NULL");
											}
											fprintf(fdf,",");
										*/
										for (iPdf= 0; iPdf < n_attchs; iPdf++)
										{
											printf("\n Inside loop Pdf .............. %d \n",iPdf);fflush(stdout);
											//primary=secondary_objects[i];
											primaryPdf=rellist[iPdf].secondary;
											//relationstrJT=rellist_JT[iJT].the_relation;

											ITK_CALL(TCTYPE_ask_object_type(primaryPdf,&objTypeTagPdf));
											ITK_CALL(TCTYPE_ask_name(objTypeTagPdf,type_name_Pdf));

											if(strcmp(type_name_Pdf,"PDF")==0)
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
													if(strcmp(tokenPdf,tokenDI)==0)
													{

															//fprintf(fdf,orig_namePdf_Tmp);   /*23] JT/PDF */            
															sprintf(pathnamePdf,"%s%s/%s",FilePath,req_item,orig_namePdf_Tmp );
															printf("\n pathnamePdf=%s\n",pathnamePdf);
															//IMF_export_file(refobjectPdf,pathnamePdf);
														
													}
												}
											}
										}
												
											/*fprintf(fdf,",");
											fprintf(fdf,mcreator);
											fprintf(fdf,",");
											fprintf(fdf,VerCreator);*/
											fprintf(fdf,",");
											fprintf(fdf,"\n");
											printf("\norig_name issssssss ---->%s\n",orig_name_Tmp);
											//sprintf(pathname,"%s/%s/%s","/home/trlprod/TCUA/Downloader",req_item,orig_name );
											sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
											printf("\npathname=%s\n",pathname);
											IMF_export_file(refobject,pathname);
										
	//								}
	//							}
							}  
						}
					}
				}
			}
			else
			{
				
			}
		}
	}
	else
	{
		printf("\nThere are no data item attached to it \n");

		
	}

	if(cadAvblFlag==0)
	{
				printf("\n No Cad data found... \n");
				fprintf(fmissingds,Item_ID_str);					/*1]  Part nos*/										
				fprintf(fmissingds,",");
				fprintf(fmissingds,Item_Revision_str_O);			/*7] Rev and sequence of design revison */
				fprintf(fmissingds,",");
				fprintf(fmissingds,"\n");
	
	}
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
