#include<tccore/item.h>
#include<string.h>
#include<tccore/aom_prop.h>
#include<sa/tcfile.h>
#include<ae/dataset.h>
#include<unidefs.h>
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <tccore/tctype.h>
#include <sa/imanfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <tc/preferences.h>
#include <lov/lov_msg.h>
#include <ss/ss_errors.h>
#include <sa/user.h>
#include <tccore/tc_msg.h>
#include <ae/dataset_msg.h>
#include <tc/tc.h>
#include <tc/emh.h>
#include <ecm/ecm.h>
#include <fclasses/tc_string.h>
#include <tccore/item_errors.h>
#include <tcinit/tcinit.h>
#include <time.h>
#include <ae/ae.h>                  /* for dataset id and rev */
#include <setjmp.h>
#include <ae/ae_errors.h>           /* for dataset id and rev */
#include <ae/ae_types.h>            /* for dataset id and rev */
#include <user_exits/user_exit_msg.h>
#include <pom/enq/enq.h>
#include <ug_va_copy.h>
#include <itk/mem.h>
#include <tie/tie_errors.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tc/tc_startup.h>
#include <user_exits/user_exits.h>
#include <tccore/method.h>
#include <property/prop.h>
#include <property/prop_msg.h>
#include <property/prop_errors.h>
#include <tccore/item.h>
#include <lov/lov.h>
#include <sa/sa.h>
#include <sa/site.h>
#include <res/res_itk.h>
#include <res/reservation.h>
#include <tccore/workspaceobject.h>
#include <tc/wsouif_errors.h>
#include <tccore/aom.h>
#include <publication/dist_user_exits.h>
#include <form/form.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <constants/constants.h>
#include <tc/emh_const.h>
#include <sa/groupmember.h>
#include <tc/tc_arguments.h>
#include <cfm/cfm.h>
#define ITKCALL( argument )                                             \
{                                                                       \
    int retcode = argument;                                             \
    if ( retcode != ITK_ok ) {                                          \
        char* s;                                                        \
        printf( " "#argument "\n" );                                    \
        printf( "  returns [%d]\n", retcode );                          \
        EMH_ask_error_text (retcode, &s);                               \
        printf( "  Teamcenter ERROR: [%s]\n", s);           \
        printf( "  in file ["__FILE__"], line [%d]\n\n", __LINE__ );    \
        if (s != 0) MEM_free (s);                                       \
    }                                                                   \
}

#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) { PrintErrorStack(); return ifail;}

int		TotalAffectedGrp  = 0;
int 	TotalResltparts   = 0;
int 	TotalAddparts     = 0;
int 	TotalDelparts     = 0;
int 	TotalQtychngparts = 0;


#define ITK_CALL(x) {           \
	int stat;                     \
	char *err_string;             \
	if( (stat = (x)) != ITK_ok)   \
	{                             \
	EMH_get_error_string (NULLTAG, stat, &err_string);                 \
	printf ("ERROR: %d ERROR MSG: %s.\n", stat, err_string);           \
	printf ("FUNCTION: %s\nFILE: %s LINE: %d\n",#x, __FILE__, __LINE__); \
	if(err_string) MEM_free(err_string);                                \
	exit (EXIT_FAILURE);                                                   \
	}                                                                    \
}



extern int ITK_user_main (int argc, char ** argv )
{
	int     status;

	char 		*FromDt				=	NULL;
	char 		*ToDt				=	NULL;
	char		*FromDtDup			=	NULL;
	char		*ToDtDup			=	NULL;
	char		*frmDt				=	NULL;
	char		*FileName			=	NULL;
	char		*FlLoc				=	NULL;

	void ***report;

	FILE	*DML_List		=	NULL;

	date_t		FromDt_t;
	date_t		ToDt_t;
	date_t		vals[2];

	FromDt			=	ITK_ask_cli_argument("-DF=");
	ToDt			=	ITK_ask_cli_argument("-DT=");
	FlLoc			=	ITK_ask_cli_argument("-FL=");

	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_auto_login( ));
    ITK_CALL(ITK_set_journalling( TRUE ));

	printf("\n From Date ==> %s || To Date ==> %s ",FromDt,ToDt);fflush(stdout);

	//Convert String date to date_t

	char	*strToBereplaced;
	char	*strToBeUsedInstead;

	strToBereplaced	=	(char*)malloc(5);
	tc_strcpy(strToBereplaced,"-");
	strToBeUsedInstead	=	(char*)malloc(5);
	tc_strcpy(strToBeUsedInstead,"_");

	ITK_CALL(STRNG_replace_str (ToDt,strToBereplaced,strToBeUsedInstead,&frmDt));
	printf("\nFlLoc : %s, frmDt ==> %s",frmDt,FlLoc);fflush(stdout);
	FileName	=	(char*)malloc(500);
	tc_strcpy(FileName,FlLoc);
	tc_strcat(FileName,"APL_Released_DML_");
	tc_strcat(FileName,frmDt);
	tc_strcat(FileName,".txt");
	printf("\nFilename : %s",FileName);fflush(stdout);


	char 		*FromDt1			=	NULLTAG;
	char 		*ToDt1				=	NULLTAG;
	int  rows = 0, columns = 0;
	const char * select_attrs[]={"puid"};

	FromDt1 = strdup(FromDt);
	ToDt1	= strdup(ToDt);

	ITK_string_to_date(FromDt1,&FromDt_t);
	ITK_string_to_date(ToDt1,&ToDt_t);

	vals[0]=FromDt_t;
	vals[1]=ToDt_t;
	
	
	/** Creating a POM Query **/
	ITK_CALL(POM_enquiry_create("find_Dml_Number"));
	printf("***** POM_enquiry_create Query created *********\n");fflush(stdout);

	/** Adding select attribute to Query **/

	ITK_CALL(POM_enquiry_add_select_attrs ("find_Dml_Number","T5_APLDMLRevision",1,select_attrs));

	printf("***** POM_enquiry_add_select_attrs created *********\n");fflush(stdout);

	/**Setting Released Date **/
	ITK_CALL(POM_enquiry_set_date_value ("find_Dml_Number","expr_dates", 2 ,(const date_t*)vals,POM_enquiry_bind_value));

	/** creating between Expression **/

	ITK_CALL(POM_enquiry_set_attr_expr ("find_Dml_Number","expr_dates_between","T5_APLDMLRevision","t5_APLReleaseDate",POM_enquiry_between,"expr_dates"));

	/**Setting the Where Expression **/

	printf("******Before Executing POM_enquiry_set_where_expr *********\n");fflush(stdout);

	ITK_CALL(POM_enquiry_set_where_expr ("find_Dml_Number","expr_dates_between"));

	printf("******After  Executing POM_enquiry_set_where_expr *********\n");fflush(stdout);

	printf("******Before Executing POM_enquiry_execute *********\n");fflush(stdout);
	POM_enquiry_add_order_attr ("find_Dml_Number", "POM_application_object", "t5_APLReleaseDate", POM_enquiry_asc_order);
	ITK_CALL(POM_enquiry_execute("find_Dml_Number", &rows, &columns, &report));
	
	printf("******After Executing POM_enquiry_execute *********\n");fflush(stdout);
	ITK_CALL(POM_enquiry_delete("find_Dml_Number"));
	
	printf(" \n number of rows %d and coloumn %d\n",rows,columns);fflush(stdout);
	
	int iDML	=	0;
	if (rows > 0)
	{
		DML_List=fopen(FileName,"w");
		fflush(DML_List);
		for (iDML=0; iDML<rows ;iDML++ )
		{
			char	*AplDMLNum		=	NULL;
			char	*Proj_Code		=	NULL;
			char	*rel_type		=	NULL;

			int		iDMLCnt			=	0;

			tag_t	DMLTag			=	NULLTAG;

			DMLTag	=	 *(tag_t*)(report[iDML][0]);

			ITK_CALL(AOM_ask_value_string(DMLTag,"item_id",&AplDMLNum));
			ITK_CALL(AOM_ask_value_string( DMLTag, "t5_cprojectcode", &Proj_Code));
			
			if (tc_strcmp(Proj_Code,"5445")==0)
			{
				//printf("\nDML Number : %s, Project Code : %s",AplDMLNum,Proj_Code);fflush(stdout);

				int st_count		=	0;
				int iStCnt		=	0;
				tag_t*			status_list			=	NULLTAG;
				ITK_CALL(WSOM_ask_release_status_list(DMLTag,&st_count,&status_list));
				//printf("\n\n\t\t Child Status Count  is :%d",st_count);	fflush(stdout);
				if (st_count>0)
				{
					for (iStCnt=0;iStCnt<st_count ;iStCnt++ )
					{
						char* c_st_class_name	=	NULL;
						char* c_st_ObjType	=	NULL;
						ITK_CALL(AOM_ask_value_string(status_list[iStCnt],"object_name",&c_st_class_name));
						//printf("\n c_st_class_name: %s\n",c_st_class_name);fflush(stdout);
						
						ITK_CALL(AOM_ask_value_string(status_list[iStCnt],"object_type",&c_st_ObjType));
						//printf("\n rlz status object_type: %s\n",c_st_ObjType);fflush(stdout);

						if (tc_strstr(AplDMLNum,"AM")!=NULL)
						{
							ITK_CALL(AOM_ask_value_string( DMLTag, "t5_EcnType", &rel_type));
							if (tc_strcmp(rel_type,"APLSTR")==0)
							{
								iDMLCnt++;
							}
						}
						else
						{
							ITK_CALL(AOM_ask_value_string( DMLTag, "t5_rlstype", &rel_type));
							if (tc_strcmp(rel_type,"Veh")==0 || tc_strcmp(rel_type,"MR")==0 || tc_strcmp(rel_type,"TPL")==0)
							{
								iDMLCnt++;
							}
						}
						//printf("\nRelease Type : %s",rel_type);fflush(stdout);
						if(tc_strcmp(c_st_class_name,"T5_LcsAplRlzd")==0 && iDMLCnt>0)
						{
							char *Aplrlzdate = NULL;
							//printf("\n Revision s is APL released");fflush(stdout);
							ITK_CALL(AOM_UIF_ask_value(status_list[iStCnt],"date_released",&Aplrlzdate));
							//printf("\n\t itemid--Aplrlzdate:: [%s]\n\t",Aplrlzdate);;fflush(stdout);
							printf("\nDML Number : %s, Project Code : %s, Release Type : %s",AplDMLNum,Proj_Code,rel_type);fflush(stdout);
							fprintf(DML_List,"%s\n",AplDMLNum);fflush(DML_List);
							break;
						}
					}
				}
				else
				{
					//printf("\nAPL Release Status is not present for DML %s",AplDMLNum);fflush(stdout);
				}
			}
		}
		fclose(DML_List);
	}
}