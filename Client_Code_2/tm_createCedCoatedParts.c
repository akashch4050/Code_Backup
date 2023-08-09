/******************************************************************************
* CVS: $Id
*
* COPYRIGHT 2008  MNM.  ALL RIGHTS RESERVED
*
*
* *------------------------------------------------------------------------------
** Filename		:tm_createCedCoatedParts.c
*
* Description		: > This Utility is to do unit testing of functions .
* Module  		        
* Since		    :    

* ENVIRONMENT		:    C++, ITK

*
* History

*------------------------------------------------------------------------------
* Date         	 Name						Description of Change
* 05/03/2021   	Sanjoy Mondal			    Initial Code

* -----------------------------------------------------------------------------
*
*****************************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <tccore/custom.h>
#include <ict/ict_userservice.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <sa/imanfile.h>
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
#include <sa/tcfile.h>
#include <tcinit/tcinit.h>
#include <tccore/tctype.h>
#include <time.h>
#include <ae/ae.h>                  /* for dataset id and rev */
#include <setjmp.h>
#include <ae/ae_errors.h>           /* for dataset id and rev */
#include <ae/ae_types.h>            /* for dataset id and rev */
#include <unidefs.h>
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
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <pie/pie.h>
#include <bom/bom_attr.h>
#include <bom/bom_tokens.h>

/*
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
}*/
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
#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) { PrintErrorStack(); return ifail;}
#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}

static void handle_itk_error
    ( int stat,                               /* <I> */
      int source_code_line,                   /* <I> */
      const char *source_code_file_name       /* <I> */
    );

#define ITK(x)                                                             \
{                                                                          \
    if ( stat == ITK_ok )                                                  \
    {                                                                      \
        if ( (stat = (x)) != ITK_ok )                                      \
        {                                                                  \
            handle_itk_error( stat, __LINE__, __FILE__ );                  \
        }                                                                  \
    }                                                                      \
}

static void Write_To_Log(char *format, ...)
{
    char msg[1000];
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    printf(msg);
    TC_write_syslog(msg);
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
    const int *pSevLst = NULL;
    const int *pErrCdeLst = NULL;
    const char **pMsgLst = NULL;
    register int i = 0;

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst );



    fprintf( stderr, "Reivse Error(s): \n");
	Write_To_Log("Revise Error(s): \n");
    for ( i = 0; i < iNumErrs; i++ )
    {
        fprintf( stderr, "\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
		Write_To_Log("\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i]);
        fprintf( stderr, "\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
		Write_To_Log("\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i]);
    }
    return ITK_ok;
}



//Function start
void setAddTagObj(int *count,tag_t **objlist,tag_t obj )
{

	*count=*count+1;
	if(*count==1)
	{
		printf("\n ****setAddTagObj1");fflush(stdout);
		(*objlist) = (tag_t *)malloc((*count ) * sizeof(tag_t ));

	}
	else
	{
		printf("\n ****setAddTagObj2");fflush(stdout);
		(*objlist) = (tag_t *)realloc((*objlist),(*count ) * sizeof(tag_t ));
	}

	(*objlist)[*count-1] = obj; //malloc((strlen(view_id)+1) * sizeof(char));
}


void t5AddTagObjToSet(int *count,tag_t **objlist,tag_t obj )
{

	*count=*count+1;
	if(*count==1)
	{
		//printf("\n ****setAddTagObj1");fflush(stdout);
		(*objlist) = (tag_t *)malloc((*count ) * sizeof(tag_t ));

	}
	else
	{
		//printf("\n ****setAddTagObj2");fflush(stdout);
		(*objlist) = (tag_t *)realloc((*objlist),(*count ) * sizeof(tag_t ));
	}

	(*objlist)[*count-1] = obj; //malloc((strlen(view_id)+1) * sizeof(char));
}

/* Function to add string into a set of string */
void t5AddStrToSet(int *count,char ***strset,char* str)
{

	*count=*count+1;
	//printf("\n setAddStr %d",*count);fflush(stdout);
	if(*count==1)
	{
		(*strset) = (char **)malloc((*count ) * sizeof(char *));
	}
	else
	{
		(*strset) = (char **)realloc((*strset),(*count ) * sizeof(char *));
	}
	(*strset)[*count-1] = malloc((strlen(str)+1) * sizeof(char));
	 tc_strcpy((*strset)[*count-1],str);
	 //printf("\n setAddStr===%s",(*strset)[*count-1]);fflush(stdout);

}

void t5FindStrInSet(char **str_list,int count,char *str,int *found)
{

	int k=0;
	*found=0;
	for(k=0;k<count;k++)
	{

		//printf("\n[%d]tc_strcmp(%s,%s)",k,str_list[k],str);fflush(stdout);
		if(tc_strcmp(str_list[k],str)==0)
		{
			*found=1;
			break;
		}

	}
}

///CODE START HERE
int t5GetItemRevison(char* InputPart)
{
	//MT start
	tag_t   OutPutTag     = NULLTAG;
	int n_tags_found2=0;
	tag_t *tags_found2 = NULL;
	
	//char **attrs2 = (char **) MEM_alloc(200 * sizeof(char *));
	//char **values2 = (char **) MEM_alloc(200 * sizeof(char *));
	int n_tags_foundd2=0;
	int n_tags_foundd3=0;
	tag_t *tags_foundd3 = NULL;
	tag_t *tags_foundd2 = NULL;
	//char **attrss2 = (char **) MEM_alloc(200 * sizeof(char *));
	//char **valuess2 = (char **) MEM_alloc(200 * sizeof(char *));
	const char *attrss2[2];
	const char *valuess2[2];
	const char *attrss3[2];
	const char *valuess3[2];
	//MT end
	//printf("\n FUN:part no :%s.", InputPart);fflush(stdout);
	//GETTING RELEASED REVISION
	const char *attrs2[2];
	const char *values2[2];
	attrs2[0] ="item_id";
	attrs2[1] ="object_type";
	values2[0] = (char *)InputPart;
	values2[1] = "Design";
	if(ITEM_find_items_by_key_attributes(2,attrs2, values2, &n_tags_found2, &tags_found2)!=ITK_ok)PrintErrorStack();
	//printf("\n FUN:count n_tags_found2 :%d.", n_tags_found2);fflush(stdout);

	if (n_tags_found2>0)
	{
		OutPutTag= tags_found2[0];
	}
	else
	{
		///printf("\n FUN:Inside T5_EE_Part:%s.", InputPart);fflush(stdout);
		attrss2[0] ="item_id";
		attrss2[1] ="object_type";
		valuess2[0] = (char *)InputPart;
		valuess2[1] = "T5_EE_Part";
		if(ITEM_find_items_by_key_attributes(2,attrss2, valuess2, &n_tags_foundd2, &tags_foundd2)!=ITK_ok)PrintErrorStack();
		//printf("\n FUN:EE part count :%d.", n_tags_foundd2);fflush(stdout);
		if (n_tags_foundd2>0)
		{
			OutPutTag= tags_foundd2[0];
		}
		else
		{
			//printf("\n FUN:Inside T5_ClrPart:%s.", InputPart);fflush(stdout);
			attrss3[0] ="item_id";
			attrss3[1] ="object_type";
			valuess3[0] = (char *)InputPart;
			valuess3[1] = "T5_ClrPart";
			if(ITEM_find_items_by_key_attributes(2,attrss3, valuess3, &n_tags_foundd3, &tags_foundd3)!=ITK_ok)PrintErrorStack();
			//printf("\n FUN:T5_ClrPart part count :%d.", n_tags_foundd3);fflush(stdout);
			if (n_tags_foundd3>0)
			{
				OutPutTag= tags_foundd3[0];
			}
			else
			{
				//printf("\n FUN:Inside T5_ArchModule:%s.", InputPart);fflush(stdout);
				attrss3[0] ="item_id";
				attrss3[1] ="object_type";
				valuess3[0] = (char *)InputPart;
				valuess3[1] = "T5_ArchModule";
				if(ITEM_find_items_by_key_attributes(2,attrss3, valuess3, &n_tags_foundd3, &tags_foundd3)!=ITK_ok)PrintErrorStack();
				//printf("\n FUN:T5_ArchModule part count :%d.", n_tags_foundd3);fflush(stdout);
				if (n_tags_foundd3>0)
				{
					OutPutTag= tags_foundd3[0];
				}
			}
		}
	}
	if(OutPutTag==NULLTAG)
	{
		//printf("\n FUN: NULL tag for part no:%s.", InputPart);fflush(stdout);
	}
	else
	{
		//printf("\n FUN: tag found for part no:%s.", InputPart);fflush(stdout);
	}
	/*if(attrs2) MEM_free(attrs2);
	if(attrss2) MEM_free(attrss2);
	if(values2) MEM_free(values2);
	if(valuess2) MEM_free(valuess2);*/

	return OutPutTag;	
}

int getItemRevObject(char *partNumber, char *object_type, tag_t *tcPart)
{
	int ifail = ITK_ok;
	int  count 	= 0;
	tag_t *itemObj = NULLTAG;	
	char **qry_entries = (char **) MEM_alloc(10 * sizeof(char *));
	char **qry_values = (char **) MEM_alloc(10 * sizeof(char *));
	tag_t query = NULLTAG;
	//printf("\n\n object_type ===> [%s]\n",object_type);fflush(stdout);
	
	qry_entries[0] = "Type";
	qry_entries[1] = "Item ID";
	
	qry_values[0] = object_type;
	qry_values[1] = partNumber;
		
	CALLAPI(QRY_find("Item Revision...", &query));
	CALLAPI(QRY_execute(query, 2, qry_entries, qry_values, &count, &itemObj));
	printf("\n SAN:tm_createCedCoatedParts: getItemRevObject: count==>%d\n", count);fflush(stdout);

	if(count>0)
	{
		*tcPart = itemObj[count-1];
	}
	
	return ifail;
}



int getGeneralObject(char *partNumber, char *object_type, tag_t *tcPart)
{
	int ifail = ITK_ok;
	int  count 	= 0;
	tag_t *itemObj = NULLTAG;	
	char **qry_entries = (char **) MEM_alloc(10 * sizeof(char *));
	char **qry_values = (char **) MEM_alloc(10 * sizeof(char *));
	tag_t query = NULLTAG;
	//printf("\n\n object_type ===> [%s]\n",object_type);fflush(stdout);
	
	qry_entries[0] = "Type";
	qry_entries[1] = "Name";
	
	qry_values[0] = object_type;
	qry_values[1] = partNumber;
		
	CALLAPI(QRY_find("General...", &query));
	CALLAPI(QRY_execute(query, 2, qry_entries, qry_values, &count, &itemObj));
	printf("\n SAN:tm_createCedCoatedParts: getGeneralObject: count==>%d\n", count);fflush(stdout);

	if(count>0)
	{
		*tcPart = itemObj[count-1];
	}
	
	return ifail;
}

//Solve BOM by applying SVR on Platform and get unit bom
int tm_SolveAndGetUnitBom(tag_t pltFrmTag, tag_t svrTag,int *cnt, tag_t **moduleList)
{
	int ifail = ITK_ok;
	tag_t view_type = NULLTAG;
	tag_t pltFrmItemTag = NULLTAG;
	int n_bom_views = 0;
	tag_t* bom_views = NULLTAG;
	tag_t bom_view = NULLTAG;
	
	tag_t bom_window = NULLTAG;
	tag_t revRule = NULLTAG;
	
	tag_t* closureRules = NULLTAG;
	int ruleFound = 0;
	tag_t closureRuleTag = NULLTAG;
	tag_t top_line = NULLTAG;
	tag_t bom_variant_rule = NULLTAG;
	tag_t bomVariantTypeTag = NULLTAG;
	char bom_var_type_name[TCTYPE_name_size_c+1];
	tag_t var_exp_block = NULLTAG;
	
	tag_t* lines = NULLTAG;
	int n_lines = 0;
	
	int i=0;
	tag_t childObj = NULLTAG;
	char* child_item_id = NULL;
	
	int n_lines2 = 0;
	tag_t* lines2 = NULLTAG;
	
	int n_optns = 0;
	tag_t* options = NULLTAG;
	tag_t* option_revs = NULLTAG;
	
	int n_lines1 = 0;
	logical is_bom_modified = FALSE;
	
	int j=0;
	int k =0;
	
	tag_t childObj2 = NULLTAG;
	char* child_item_id2 = NULL;
	
	int n_lines3 = 0;
	tag_t* lines3 = NULLTAG;
	
	tag_t childObj3 = NULLTAG;
	char* child_item_id3 = NULL;
	int IntAttr = 0;
	char* module_rel_status = NULL;
	
	
	printf("\n SAN:tm_createCedCoatedParts: Start of funtion tm_SolveAndGetUnitBom\n");fflush(stdout);
	
	if(pltFrmTag != NULLTAG)
	{
		CALLAPI ( PS_find_view_type( "view", &view_type ));
		printf("\n SAN:tm_createCedCoatedParts: PS_find_view_type is found......\n" ); fflush(stdout);

		if ( view_type != NULLTAG )
		{
			CALLAPI ( ITEM_ask_item_of_rev( pltFrmTag, &pltFrmItemTag ));
			CALLAPI ( ITEM_list_bom_views( pltFrmItemTag, &n_bom_views, &bom_views ));

			bom_view = bom_views[0];
		}
		else
			printf("\n SAN:tm_createCedCoatedParts: View not found....\n"); fflush(stdout);		
	}
	
	
	if( bom_view != NULLTAG )
	{
		
		CALLAPI(BOM_create_window( &bom_window ));
		//CALLAPI(CFM_find( "ERC release and above", &rule ))
		CALLAPI(CFM_find( "Latest Working", &revRule ))
		CALLAPI(BOM_set_window_config_rule( bom_window, revRule ));				
		CALLAPI(PIE_find_closure_rules( "BOMLineskipforunconfigured",PIE_TEAMCENTER, &ruleFound, &closureRules ));
		printf ("\n SAN:tm_createCedCoatedParts: Closure Rule count %d \n",ruleFound);fflush(stdout);
		
		if(ruleFound>0)
		{
			closureRuleTag = closureRules[0];
			printf("\n SAN:tm_createCedCoatedParts: Closure Rule found....\n"); fflush(stdout);	
		}

		if(closureRuleTag!=NULLTAG) CALLAPI(BOM_window_set_closure_rule( bom_window,closureRuleTag, 0, NULL,NULL ));
		CALLAPI(BOM_set_window_pack_all(bom_window, FALSE));
		CALLAPI(BOM_set_window_top_line( bom_window, NULLTAG, pltFrmTag, NULLTAG, &top_line ));
		CALLAPI(BOM_window_ask_variant_rule(bom_window, &bom_variant_rule ));
		
		CALLAPI(TCTYPE_ask_object_type(bom_variant_rule,&bomVariantTypeTag));
		CALLAPI(TCTYPE_ask_name(bomVariantTypeTag,bom_var_type_name));
		printf("\n SAN:tm_createCedCoatedParts: bom_var_type_name[%s]\n",bom_var_type_name);fflush(stdout);
		//Get the variant expression block associated with the specified Item Revision.
		CALLAPI(ITEM_ask_rev_variants(pltFrmTag, &var_exp_block));
		
		if(top_line!=NULLTAG)
		{
			CALLAPI(BOM_line_ask_child_lines(top_line, &n_lines, &lines));
			printf("\n SAN:tm_createCedCoatedParts: Before setting option n_lines: %d\n", n_lines);fflush(stdout);
			CALLAPI(BOM_window_hide_unconfigured(bom_window))   ;
			CALLAPI(BOM_window_show_variants(bom_window))   ;
						
			CALLAPI(BOM_window_apply_variant_configuration(bom_window,1,&svrTag))   ;
			printf("\n SAN:tm_createCedCoatedParts: After BOM_window_apply_variant_configuration-----\n"); fflush(stdout);

			CALLAPI(BOM_window_hide_variants(bom_window))   ;

			CALLAPI(BOM_line_ask_child_lines(top_line, &n_lines, &lines));
			printf("\n SAN:tm_createCedCoatedParts: After setting option n_lines: %d\n", n_lines);fflush(stdout);
			
			if(n_lines >0)
			{
				for (i=0;i<n_lines ;i++ )
				{
					if(childObj) childObj=NULLTAG;
					childObj=lines[i];
					CALLAPI(AOM_ask_value_string(childObj, "bl_item_item_id", &child_item_id ));

					CALLAPI(BOM_line_ask_child_lines(childObj, &n_lines2, &lines2));
					printf("\n SAN:tm_createCedCoatedParts: [%d] child_item_id[%s] \t count [%d]",i+1,child_item_id,n_lines2);fflush(stdout);

				}				
			}
			
			 
			CALLAPI(CFM_find("ERC release and above", &revRule))
			//CALLAPI(CFM_find( "Latest Working", &rule ))
			
			//Apply Revision rule 
			CALLAPI(BOM_set_window_config_rule( bom_window, revRule));
			
			// get the list of options and their revisions referenced by the given BOM variant rule.
			CALLAPI (BOM_variant_rule_ask_options( bom_variant_rule, &n_optns, &options, &option_revs ) )
			printf("\n SAN:tm_createCedCoatedParts: No of Option values in the rule:%d\n",n_optns);fflush(stdout);
			
			
			if(top_line!=NULLTAG )
			{
				
				CALLAPI(BOM_line_ask_child_lines(top_line, &n_lines1, &lines));
				printf("\n SAN:tm_createCedCoatedParts: Before setting option n_lines1: %d \n", n_lines1);fflush(stdout);
				
				if(bom_window != NULLTAG)
				{
					CALLAPI(BOM_window_hide_unconfigured(bom_window))   ;
					CALLAPI(BOM_window_show_variants(bom_window))   ;
					printf("\n SAN:tm_createCedCoatedParts: After inside bom_window apply variant condition\n"); fflush(stdout);			
					CALLAPI(BOM_window_apply_variant_configuration(bom_window,1,&svrTag))   ;
					printf("\n SAN:tm_createCedCoatedParts: After BOM_window_apply_variant_configuration\n"); fflush(stdout);

					CALLAPI(BOM_window_hide_variants(bom_window))   ;
					CALLAPI(BOM_window_ask_is_modified(bom_window,&is_bom_modified));
					
					if(is_bom_modified)
					{
						printf("\n SAN:tm_createCedCoatedParts: Bom is modified....\n");fflush(stdout);
					}
					else
					{
						printf("\n SAN:tm_createCedCoatedParts: Bom is  not modified....\n");fflush(stdout);
					}
					
					CALLAPI(BOM_line_ask_child_lines(top_line, &n_lines1, &lines));
					printf("\n SAN:tm_createCedCoatedParts: After setting option n_lines1: %d \n", n_lines1);fflush(stdout);
					
					printf("\n SAN:tm_createCedCoatedParts: Get the unit bom (module) start...\n");fflush(stdout);
					
					if (n_lines1 >0)
					{
						int unitBomCnt=0;
						int strCnt = 0;
						char** strBuff = NULL;
						int foundStr = 0;
						for (j=0;j<n_lines1 ;j++ )
						{
							if(childObj2) childObj2=NULLTAG;
							childObj2=lines[j];
							CALLAPI(AOM_ask_value_string(childObj2, "bl_item_item_id", &child_item_id2));
							CALLAPI(BOM_line_ask_child_lines(childObj2, &n_lines3, &lines3));
							
							if(n_lines3 >0)
							{
								for (k=0;k<n_lines3 ;k++ )
								{
									if(childObj3) childObj3=NULLTAG;
									childObj3=lines3[k];
									CALLAPI(AOM_ask_value_string(childObj3, "bl_item_item_id", &child_item_id3 ));
									CALLAPI(BOM_line_look_up_attribute ("bl_rev_last_release_status",&IntAttr));
									CALLAPI(BOM_line_ask_attribute_string(childObj3, IntAttr, &module_rel_status));
									
									//more than one module
									if(k>0)
									{
										t5FindStrInSet(strBuff,strCnt,child_item_id3,&foundStr);
										
										if(foundStr==1)
										{
											//t5AddStrToSet(&strCnt,&strBuff,child_item_id3);
											printf("\n SAN:tm_createCedCoatedParts: Multi module case [%s]. Skip it\n",child_item_id3);fflush(stdout);
											continue;
											
										}
										else
										{
											printf("\n SAN:tm_createCedCoatedParts: Multi module case [%s]. Check it \n",child_item_id3);fflush(stdout);
											printf("[%d]child_item_id3[%d]==>%s, module_rel_status===>%s\n",j,k,child_item_id3,module_rel_status);fflush(stdout);
											t5AddTagObjToSet(cnt,moduleList,childObj3);
											unitBomCnt++;
											break;											
											
										}

																				
									}
									else
									{
										printf("[%d]child_item_id3[%d]==>%s, module_rel_status===>%s\n",j,k,child_item_id3,module_rel_status);fflush(stdout);
										t5AddTagObjToSet(cnt,moduleList,childObj3);
										unitBomCnt++;
									}

									
									t5AddStrToSet(&strCnt,&strBuff,child_item_id3);

								}
							}
							
						}
						printf("\nTotal no of unit bom===>%d\n",unitBomCnt);fflush(stdout);
					}
					
					
					
				}
				
				
				
			}


			
			
			
		}
		
		
		
		
		
		//CALLAPI(BOM_save_window(bom_window))
		CALLAPI(BOM_close_window(bom_window))
		
	}
	
	
	
	printf("\n SAN:tm_createCedCoatedParts: End of funtion tm_SolveAndGetUnitBom\n");fflush(stdout);
	return ifail;
}





typedef struct PartModel_
{
 int level;
 tag_t childItemRev;
 tag_t childBomLine;
} *PartModel; 

void setAddPartModel(int *count,PartModel **objlist,PartModel obj )
{

	*count=*count+1;
	printf("\n ****count==>%d", *count);fflush(stdout);
	if(*count==1)
	{
		//printf("\n ****setAddPartModel1");fflush(stdout);
		(*objlist) = (PartModel *)malloc((*count ) * sizeof(PartModel ));
		//printf("\n ***setAddPartModel2");fflush(stdout);
	}
	else
	{
		//printf("\n **setAddPartModel3");fflush(stdout);
		(*objlist) = (PartModel *)realloc((*objlist),(*count) * sizeof(PartModel ));
		//printf("\n ***setAddPartModel4");fflush(stdout);
		//printf("\n **setAddPartModel5");fflush(stdout);
	}

	(*objlist)[*count-1] = obj; //malloc((strlen(view_id)+1) * sizeof(char));
	printf("\n **setAddPartModel");fflush(stdout);
}

int getRevisionRuleObject(char *revisionRuleName, tag_t *revisonRuleObj)
{
	int ifail = ITK_ok;
	int flagRevRule =0;
	//get the Revision Rule Object
	

	CALLAPI(CFM_find(revisionRuleName, revisonRuleObj));
	if (revisonRuleObj != NULLTAG)
	{
		//printf("\nFind revRule\n");fflush(stdout);
		flagRevRule = 1;
		
	}
	
	//printf("\n flagRevRule=>[%d] ",flagRevRule);fflush(stdout);
	
	return ifail;
}

int getClosureRuleObject(char* closureRuleName,tag_t *closureRuleObj)
{
	int ifail = ITK_ok;
	PIE_scope_t scope;
	int closureTagCnt = 0;
	tag_t *closureTags = NULLTAG;
	int flagClosureRule = 0;
	//get the Revision Rule Object
	


	scope=PIE_TEAMCENTER;
	CALLAPI(PIE_find_closure_rules2(closureRuleName,scope,&closureTagCnt,&closureTags));
	//printf("\n closureTagCnt:%d ..............",closureTagCnt);fflush(stdout);
	if(closureTagCnt==1)
	{
		*closureRuleObj=closureTags[0];
		flagClosureRule=1;
	}
	//printf("\n flagClosureRule=>[%d] ",flagClosureRule);fflush(stdout);
	
	return ifail;
}

int getBomViewRevObject(tag_t partRevObj,char* bvrName, tag_t* bomViewObj)
{
	int ifail = ITK_ok;
	const char	*prog_name 		="getBomViewRevObject";
	tag_t partMaster 		= NULLTAG;
	int n_bom_views         = 0;
	tag_t *bom_views        = NULL;
	tag_t viewObj 			= NULLTAG;
	tag_t view_type 		= NULLTAG;
	char* view_type_name = NULL;
	char* partNumber = NULL;
	char* partRev = NULL;
	if(partRevObj == NULLTAG)
	{
		//printf("\ngetBomViewRevObject: Part Object is NULLTAG\n");fflush(stdout);
		return ifail;
	}	
	// Get the Item from the revision
	CALLAPI(ITEM_ask_item_of_rev(partRevObj, &partMaster)); 
	
	//Get the tags of all bom views related to the Item.
	CALLAPI(ITEM_list_bom_views(partMaster, &n_bom_views, &bom_views ));
	CALLAPI(AOM_ask_value_string(partRevObj,"item_id",&partNumber));
	CALLAPI(AOM_ask_value_string(partRevObj,"item_revision_id",&partRev));
	//printf("\n Part number===>%s,%s\n",partNumber,partRev);fflush(stdout);	


	if(n_bom_views>0)
	{
		int j = 0;
		//printf("\nSAN: %s.%s having below bom views...\n", partNumber,partRev);fflush(stdout);
		for(j=0;j<n_bom_views;j++)
		{
			//Get the view type of the BOMView
			CALLAPI(PS_ask_bom_view_type(bom_views[j],&view_type))
			
			//Get the name of the view type
			CALLAPI(PS_ask_view_type_name(view_type, &view_type_name));
			//printf("\nView Type===>%s\n", view_type_name);fflush(stdout);
			
			
			if(tc_strcmp(view_type_name, bvrName) == 0 )
			{
				viewObj = bom_views[j];
				break;
			}						
		}
		*bomViewObj = viewObj;
		
	}
	return ifail;
	
}

int queryLatestSTDReleasedPartForPlant(char* partNo, char* objClass, char* plant, tag_t* latestSTDPart)
{
	int ifail = ITK_ok;
	tag_t partMstrTag = NULLTAG;
	char* type = NULL;
	
	//printf("\nIn queryLatestSTDReleasedPartForPlant...\n\nInput parameters:\nPart No: %s\nObject Class: %s\nPlant: %s\n",partNo,objClass,plant);fflush(stdout);
	
	CALLAPI(ITEM_find_item(partNo,&partMstrTag));
	
	if(partMstrTag!=NULLTAG)
	{
		CALLAPI(AOM_ask_value_string(partMstrTag,"object_type",&type));
		//printf("\nType of the Part = %s\n",type);fflush(stdout);
	}
	
	
	if(tc_strcmp(type,objClass)==0)
	{
		
		int partRevCnt = 0;	
		tag_t* partRevTagObjs = NULLTAG;
		tag_t partRevTag = NULLTAG;
		
		CALLAPI(ITEM_list_all_revs (partMstrTag, &partRevCnt, &partRevTagObjs));
		//printf("\n Part Revision counts :%d\n",partRevCnt);fflush(stdout);
		
		if(partRevCnt>0)
		{
			int i=0;

			for(i=partRevCnt-1;i>=0;i--)
			{
				partRevTag = partRevTagObjs[i];
				char* itemId = NULL;
				char* partRev = NULL;
				char* checkedOut = NULL;
				char* rel_status = NULL;
				CALLAPI(AOM_ask_value_string(partRevTag,"item_id",&itemId))
				CALLAPI(AOM_ask_value_string(partRevTag,"item_revision_id",&partRev))
				CALLAPI(AOM_UIF_ask_value(partRevTag, "release_status_list", &rel_status))
				
				//checked_out
				CALLAPI(AOM_UIF_ask_value(partRevTag,"checked_out",&checkedOut));
				//printf("\nqueryLatestSTDReleasedPartForPlant:Part object[%d]==>  %s,%s. Checkedout==>%s, Release status==>%s\n",i,itemId,partRev,checkedOut,rel_status);fflush(stdout);
				
				//Skip the checked out part
				if(tc_strcmp(checkedOut, "Y") == 0 )
				{
					continue;
				}
				
				if(tc_strcmp(plant,"CAR")==0 && tc_strstr(rel_status,"STDSIC Released")!=NULL)
				{
				
					*latestSTDPart = partRevTag;
					break;
				}
				else if(tc_strcmp(plant,"CVBU JSR")==0 && tc_strstr(rel_status,"STDSIJ Released")!=NULL)
				{
					
					*latestSTDPart = partRevTag;
					break;
				}
				else if((tc_strcmp(plant,"CVBU LKO")==0 || tc_strcmp(plant,"PCBU LKO")==0)&& tc_strstr(rel_status,"STDSIL Released")!=NULL)
				{
					
					*latestSTDPart = partRevTag;
					break;
				}
				else if(tc_strcmp(plant,"CVBU PUNE")==0 && tc_strstr(rel_status,"STDSIP Released")!=NULL)
				{
					
					*latestSTDPart = partRevTag;
					break;
				}
				else if(tc_strcmp(plant,"DHARWAD")==0 && tc_strstr(rel_status,"STDSID Released")!=NULL)
				{
					*latestSTDPart = partRevTag;
					break;
				}
				else if(tc_strcmp(plant,"PCBU and CVBU")==0 && tc_strstr(rel_status,"STDSIP Released")!=NULL)
				{
					*latestSTDPart = partRevTag;
					break;
				}
				else if((tc_strcmp(plant,"CVBU PNR")==0 || tc_strcmp(plant,"SMALLCAR PNR")==0)&& tc_strstr(rel_status,"STDSIU Released")!=NULL)
				{
					*latestSTDPart = partRevTag;
					break;
				}				
				else if(tc_strcmp(plant,"SMALLCAR AHD")==0 && tc_strstr(rel_status,"STDSIA Released")!=NULL)
				{
					*latestSTDPart = partRevTag;
					break;
				}	
				else if(tc_strcmp(plant,"PUVBU")==0 && tc_strstr(rel_status,"STDSIV Released")!=NULL)
				{
					*latestSTDPart = partRevTag;
					break;
				}
				else if(tc_strcmp(plant,"ALL")==0 && tc_strstr(rel_status,"Released")!=NULL && tc_strstr(rel_status,"STDSI")!=NULL)
				{					
					*latestSTDPart = partRevTag;
					break;
				}
				else
				{
					//printf("\nINCORRECT INPUT PARAMETRES.Check the input parameters of the function......\n");fflush(stdout)
				}
				
			}	
			
			if(*latestSTDPart==NULLTAG)
			{
				//printf("\nINCORRECT INPUT PARAMETRES or no data found. Please check..\n");fflush(stdout);
			}
		
		}
	}
	
	//printf("\nEND of  queryLatestSTDReleasedPartForPlant...\n");fflush(stdout);
	return ifail;
}

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


void setAddInt(int *count,int **objlist,int obj )
{

	*count=*count+1;
	if(*count==1)
	{
		//printf("\n ****setAddInt1");fflush(stdout);
		(*objlist) = (int *)malloc((*count ) * sizeof(int ));
	}
	else
	{
		//printf("\n **setAddInt2");fflush(stdout);
		(*objlist) = (int *)realloc((*objlist),(*count ) * sizeof(int ));
	}

	(*objlist)[*count-1] = obj; //malloc((strlen(view_id)+1) * sizeof(char));


}


void setFindInt(int *int_list,int count,int in,int *found)
{

	int k=0;
	*found=0;
	for(k=0;k<count;k++)
	{

		//printf("\n[%d]tc_strcmp(%s,%s)",k,str_list[k],str);fflush(stdout);
		if(int_list[k]== in)
		{
			*found=1;
			break;
		}

	}
}


int isEligibleForCEDPartCreation(tag_t partRevTag, logical* result)
{
	int ifail = ITK_ok;
	
	char* itemId12 = NULL;
	char* partLCS = NULL;	
	char* rel_status = NULL;
	char* partDesgGrp = NULL;
	int* setOfDesignGrp = NULL;
	int designCnt = 0;
	int i =0;
	
	logical spareIndicator = false;
	logical validLcs = false;
	logical validDesGrp = false;
	logical validDesignation = false;
	
	int partDesGrpInt = 0;
	char* desigStr = NULL;
	char* PartType = NULL;
	
	CALLAPI(AOM_ask_value_string(partRevTag,"item_id",&itemId12));//object_name item_id
	//printf("\nSAN: isEligibleForCEDPartCreation:12 digit part no===>%s\n",itemId12);fflush(stdout);
	CALLAPI(AOM_ask_value_logical(partRevTag,"t5_SpareInd",&spareIndicator))
	
	//printf("\nSAN: isEligibleForCEDPartCreation: Spare Indicator of the part %s =====>%d\n",itemId12,spareIndicator);fflush(stdout);
	
	
	//Check for LC State - STD1 released
	
	CALLAPI(AOM_UIF_ask_value(partRevTag,"release_status_list",&partLCS))
	//printf("\n SAN: isEligibleForCEDPartCreation:LC State of part Rev: %s ", partLCS);fflush(stdout);
	
	if((tc_strstr(partLCS,"STDSIP Released")!=NULL)||(tc_strstr(partLCS,"STDSIC Released")!=NULL))//T5_LcsStdRlzd STDSIC Released
	{
		validLcs = true;
	}
	
	//Check for Design grp - 31 and 60 till 99 - t5_DesignGrp
	CALLAPI(AOM_ask_value_string(partRevTag,"t5_DesignGrp",&partDesgGrp));//object_name item_id
	//printf("\nSAN: isEligibleForCEDPartCreation:Design Group===>%s\n",partDesgGrp);fflush(stdout);
	partDesGrpInt = atoi(partDesgGrp);
	//partDesGrpInt=67;
	//printf("\nSAN: isEligibleForCEDPartCreation:Design Group in int format===>%d\n",partDesGrpInt);fflush(stdout);
	setAddInt(&designCnt,&setOfDesignGrp,31);
	for(i=60;i<=99;i++)
	{
		setAddInt(&designCnt,&setOfDesignGrp,i);
	}
	
	//printf("\n SAN: isEligibleForCEDPartCreation:Size of Design group set ===>%d\n",designCnt);fflush(stdout);
	
	int found =0;
	setFindInt(setOfDesignGrp,designCnt,partDesGrpInt,&found);
	
	if(found==1)
	{
		//printf("\nSAN: isEligibleForCEDPartCreation:Found Design group %s\n",partDesgGrp);fflush(stdout);
		validDesGrp = true;
	}
	else
	{
		//printf("\n SAN: isEligibleForCEDPartCreation:NOT Found Design group %s\n",partDesgGrp);fflush(stdout);
		validDesGrp = false;
	}
	
	//Designation (9th and 10th digit)= For part 33,69,71,82, 86, 92 and 99 and for assemblies 01, 02 and 03 - 
	   /*
		find parttype -
		if assembly then check for 9th and 10th digit - it should be 01,02 and 03
		else
			it should be 33, 69,71,82,86,92 and 99
	 */
	 int len = 0;
	 len=strlen(itemId12);
	 if(len==12)
	 {
		 desigStr = subString(itemId12,8,2);
		 
		 //strncpy(desigStr, itemId12+8, 2);
	 
		if(desigStr!=NULL)
		{
			CALLAPI(AOM_UIF_ask_value(partRevTag,"t5_PartType",&PartType));
			//printf("\n SAN: isEligibleForCEDPartCreation:PartType is :%s\n",PartType);fflush(stdout);
			
			if(tc_strcmp(PartType,"Assembly")==0)
			{
				if((tc_strcmp(desigStr,"01")==0 )|| (tc_strcmp(desigStr,"02")==0 ) || (tc_strcmp(desigStr,"03")==0 ))
				{
					validDesignation = true;
				}
				else
				{
					validDesignation = false;
				}
			}
			else
			{
				if((tc_strcmp(desigStr,"33")==0 )|| (tc_strcmp(desigStr,"69")==0 ) || (tc_strcmp(desigStr,"71")==0 )|| (tc_strcmp(desigStr,"82")==0 )|| (tc_strcmp(desigStr,"86")==0 )|| (tc_strcmp(desigStr,"92")==0 )|| (tc_strcmp(desigStr,"99")==0 ))
				{
					validDesignation = true;
				}
				else
				{
					validDesignation = false;
				}
			}
			
		}
		else
		{
			//printf("\nSAN: isEligibleForCEDPartCreation:ERROR- desigStr is NULL. Should not have happen. Please check\n");fflush(stdout);
			validDesignation=false;
		}
	 }
	//printf("\n SAN: isEligibleForCEDPartCreation:Designation of part %s===>%s\n",itemId12,desigStr);fflush(stdout);
	
	//printf("\nspareIndicator:%d\nvalidLcs:%d\nvalidDesGrp:%d\nvalidDesignation:%d\n",spareIndicator,validLcs,validDesGrp,validDesignation);fflush(stdout);
	   
	if(spareIndicator && validLcs && validDesGrp && validDesignation)
	{
		*result = TRUE;
	}
	else
	{
		*result = FALSE;
	}
	
	return ifail;
} 



int tmExpandPartAndGetCEDPartListsRecursive(tag_t window,tag_t partRevObj,int reqLevel,int *level,char* bvrViewName,tag_t partRevisionRule,tag_t closureRuleObj,PartModel** partModelList, int * partModelListCnt)
{
	int ifail = ITK_ok;
	const char				*prog_name 		="tmExpandPartAndGetCEDPartListsRecursive";
	//printf("\nStart of tmExpandPartAndGetCEDPartListsRecursive....\n");fflush(stdout);
	
	//printf("\nSAN:level====>%d\n",*level);fflush(stdout);
	//printf("\nSAN: reqLevel====>%d\n",reqLevel);fflush(stdout);
	
	
	if(*level>=reqLevel)
	{
		return;
	}
	char* partNumberStr = NULL;
	tag_t bomViewObj = NULLTAG;
	int flagBvr = 0;
	CALLAPI(AOM_ask_value_string(partRevObj,"item_id",&partNumberStr))
	
	printf("\n[%d] of [%d], %s",*level,reqLevel,partNumberStr);fflush(stdout);
	//printf("\nSAN: partNumberStr====>%s\n",partNumberStr);fflush(stdout);
	CALLAPI(getBomViewRevObject(partRevObj,bvrViewName, &bomViewObj));
	if(bomViewObj != NULLTAG)
	{
		//printf("\n BOM View Obj not null........\n");fflush(stdout);
		flagBvr = 1;
	}
	else
	{
		flagBvr = 0;
	}
	//printf("\n flagBvr================>%d\n", flagBvr);fflush(stdout);
	
	
	//printf("\n flagClosureRule================>%d\n", flagClosureRule);fflush(stdout);
	
	//tag_t window = NULLTAG;
	//ITKCALL(BOM_create_window (&window));
	ITKCALL(BOM_set_window_config_rule(window,partRevisionRule));
	if(closureRuleObj != NULLTAG)
		ITKCALL(BOM_window_set_closure_rule(window,closureRuleObj,0,NULL,NULL));
	
	tag_t t_ItemTag 		= NULLTAG;
	tag_t top_line			= NULLTAG;
	tag_t  *childs		    = NULLTAG;
	int child_count         = 0;
	int i=0;
	if(flagBvr == 1)
	{
		CALLAPI(BOM_set_window_top_line(window, t_ItemTag,partRevObj ,bomViewObj, &top_line));
		CALLAPI(BOM_line_ask_child_lines (top_line, &child_count, &childs));
		//printf("\nNo of child objects are n : %d\n",child_count);fflush(stdout);
		
		*level = *level + 1;
		tag_t childItemTag = NULLTAG;
		tag_t childItemRevObj = NULLTAG;
		char* partNumberStr1 = NULL;
		char* partNameStr = NULL;
		PartModel partMdl = NULL;
		//lineVectorLeaf = (BOMLineVector)malloc( sizeof(BOMLineVector ));
		if(child_count>0)
		{
			for (i = 0; i< child_count; i++)
			{
				if(childs[i]==NULLTAG)
				{
					// printf("\nSan test- null tag found...\n");fflush(stdout);
				}
				BOM_line_unpack (childs[i]);
				BOM_line_look_up_attribute (( char * )bomAttr_lineItemRevTag , &childItemTag);
				BOM_line_ask_attribute_tag(childs[i], childItemTag, &childItemRevObj);
				if(childItemRevObj != NULLTAG)
				{
					
					CALLAPI(AOM_ask_value_string(childItemRevObj,"item_id",&partNumberStr1));
					//printf("\n partNumberStr1================>%s\n", partNumberStr1);fflush(stdout);
					
					CALLAPI(AOM_ask_value_string(childItemRevObj,"object_string",&partNameStr));
					//printf("\n partNameStr================>%s\n", partNameStr);fflush(stdout);
					printf("\n%s[%d] -- %s", partNumberStr,i,partNumberStr1);fflush(stdout);

					
					
					tag_t stdPartRevObj = NULLTAG;
					CALLAPI(queryLatestSTDReleasedPartForPlant(partNumberStr1,"Design","ALL",&stdPartRevObj));
					
					if(stdPartRevObj!=NULLTAG)
					{
						logical isValid = false;
						char* partname = NULL;
						CALLAPI(AOM_ask_value_string(stdPartRevObj,"object_string",&partname));
						//printf("\nSAN:CEDCre: Latest STD Released Part: %s\n",partname);fflush(stdout);
						CALLAPI(isEligibleForCEDPartCreation(stdPartRevObj,&isValid));
						//isValid = true;
						//printf("\nSAN:CEDCre:: Is Eligible part for CED part creation====>%d\n",isValid);fflush(stdout);
						if(isValid)
						{
							//printf("\nSAN:CEDCre:Part ADDED to eligible Part Set====>%s\n",partname);fflush(stdout);
							partMdl = (PartModel)malloc(sizeof(PartModel));
							partMdl->level = *level;
							partMdl->childItemRev = stdPartRevObj;
							partMdl->childBomLine = childs[i];
						
							setAddPartModel(partModelListCnt, partModelList, partMdl);	
						}
						else
						{
							//printf("\nSAN:CEDCre:NOT VALID PART---Skipping part %s for CED part creation........\n",partname);fflush(stdout);
						}						

					}
					else
					{
						//printf("\nPart %s is not STD Released. So skipping this part...\n",partNameStr);fflush(stdout);
						//continue;
					}
					

					
					
					//printf("\n partModelListCnt from partmodel================>%d\n", *partModelListCnt);fflush(stdout);
					tmExpandPartAndGetCEDPartListsRecursive(window,childItemRevObj,reqLevel,level,bvrViewName,partRevisionRule,closureRuleObj,partModelList, partModelListCnt);
				}
			}
					
		}

		
		*level = *level - 1;
	}
	
	return ifail;
	
}



int tmExpandPartAndGetCEDPartLists(tag_t partRevObj, int reqLevel, PartModel** partModelList, int *partModelListCnt, char* bvrViewName, char * revisionRuleName, char* closureRuleName)
{
	int ifail = ITK_ok;
	const char				*prog_name 		="tmExpandPartAndGetCEDPartLists";
	printf("\nStart of tmExpandPartAndGetCEDPartLists......\n");fflush(stdout);
	int level = 0;
	tag_t bomViewObj = NULLTAG;
	int flagRevRule = 0;

	tag_t partRevisionRule = NULLTAG;
	//printf("\nRevision rule name:====>%s",revisionRuleName);fflush(stdout);
	CALLAPI(getRevisionRuleObject(revisionRuleName,&partRevisionRule));
	
	if(partRevisionRule !=NULLTAG)
	{
		//printf("\n Revision Rule not null........\n");fflush(stdout);
		flagRevRule = 1;
	}
	else
	{
		flagRevRule = 0;
	}
	
	//printf("\n flagRevRule============>[%d] ",flagRevRule);fflush(stdout);
	
		
	CALLAPI(getBomViewRevObject(partRevObj,bvrViewName, &bomViewObj));
	
	tag_t closureRuleObj = NULLTAG;
	int flagClosureRule = 0;
	CALLAPI(getClosureRuleObject(closureRuleName,&closureRuleObj));
	if(closureRuleObj!=NULLTAG)
	{
		
		//printf("\n Closure Rule not null........\n");fflush(stdout);
		flagClosureRule = 1;
	}
	else
	{
		//printf("\n Closure Rule NULL........\n");fflush(stdout);
		flagClosureRule = 0;
	}
	
	if(bomViewObj == NULLTAG)
	{
		//printf("\n BOM VIEW Revision does not exist........\n");fflush(stdout);
		return;
	}
	else
	{
		if(flagRevRule == 1)
		{
			tag_t window = NULLTAG;
			CALLAPI(BOM_create_window (&window));
			//CALLAPI(tmExpandPartAndGetCEDPartListsRecursive(window,partRevObj,reqLevel,&level,bvrViewName,partRevisionRule,closureRuleName,partModelList, partModelListCnt));
			CALLAPI(tmExpandPartAndGetCEDPartListsRecursive(window,partRevObj,reqLevel,&level,bvrViewName,partRevisionRule,closureRuleObj,partModelList, partModelListCnt));
			CALLAPI(BOM_close_window(window))
		}
	}
	
	printf("\nEnd of tmExpandPartAndGetCEDPartLists......\n");fflush(stdout);
	
	return ifail;
}


int relatePrimaryWithSecondaryObj(tag_t primaryTag, tag_t secondaryTag, char* relation )
{
	int ifail = ITK_ok;
	tag_t relation_type = NULLTAG;
	tag_t primSecRelation = NULLTAG;
	CALLAPI(GRM_find_relation_type(relation,&relation_type));
	if(relation_type!=NULLTAG)
	{
		CALLAPI(GRM_create_relation(primaryTag, secondaryTag, relation_type,  NULLTAG, &primSecRelation));
		CALLAPI(GRM_save_relation(primSecRelation));
	}
	else
	{
		printf("\nNo relation %s exists...Please check. Very serious error.\n",relation);fflush(stdout);
	}
	 
	return ifail;
}



int queryLatestSpareKitRev(char* partno, tag_t* latestPart)
{
	int ifail = ITK_ok;
	tag_t partMstrTag = NULLTAG;
	tag_t partRev = NULLTAG;
	char* type = NULL;
	CALLAPI(ITEM_find_item(partno,&partMstrTag));
	
	if(partMstrTag!=NULLTAG)
	{
		CALLAPI(AOM_UIF_ask_value(partMstrTag,"object_type",&type));
		printf("\nType of the Part = %s\n",type);fflush(stdout);
		
		if(type!=NULL)
		{
			if(tc_strcmp(type, "Spare Kit")==0 || tc_strcmp(type, "Design")==0 )
			{
				//printf("\nPart type is Spare Kit part\n");fflush(stdout);
				
				CALLAPI(ITEM_ask_latest_rev(partMstrTag,&partRev));
				if(partRev !=NULLTAG)
				{
					*latestPart = partRev;
				}
				
			}
			
		}
		
	}
	
	return ifail;
}



int createNewCEDCoatedPart(tag_t partRevTag, char* plant, tag_t* newCedPart)
{
	int ifail = ITK_ok;
	tag_t rev_type_tag = NULLTAG;
	tag_t rev_create_input_tag = NULLTAG;
	tag_t item_create_input_tag = NULLTAG;
	char* itemId12 = NULL;
	tag_t item_type_tag = NULLTAG;
	tag_t cedPartMaster = NULLTAG;
	char* itemId12rev = NULL;
	char* itemId12desc = NULL;
	char* ceditemId12desc = NULL;
	char* itemId12prtType = NULL;
	char* itemId12prjcode = NULL;
	char* itemId12DsgGrp = NULL;
	
	printf("\nSAN:creCEDPart: Inside CED Coated part creation logic...........\n");fflush(stdout);
	
	if(partRevTag == NULLTAG)
	{
		printf("\nSAN:creCEDPart::partRevTag tag is NULL. Do nothing.....\n");fflush(stdout);
		return ifail;
	}
	
	CALLAPI(AOM_ask_value_string(partRevTag,"item_id",&itemId12));//object_name item_id
	printf("\nSAN:creCEDPart:12 digit part no===>%s\n",itemId12);fflush(stdout);
	
	
	CALLAPI(AOM_ask_value_string(partRevTag,"item_revision_id",&itemId12rev));//object_name item_id
	printf("\nSAN:creCEDPart:12 digit part no revision===>%s\n",itemId12rev);fflush(stdout);	
	
	
	CALLAPI(AOM_ask_value_string(partRevTag,"object_desc",&itemId12desc));//Description
	printf("\nSAN:creCEDPart:12 digit part no description===>%s\n",itemId12desc);fflush(stdout);
	
	ceditemId12desc = (char*)MEM_alloc(strlen(itemId12desc)*sizeof(char) + 30);
	tc_strcpy(ceditemId12desc,"");
	tc_strcat(ceditemId12desc,"CED-");
	tc_strcat(ceditemId12desc,itemId12desc);
			
	printf("\nNew CED Part desc ceditemId12desc:%s\n",ceditemId12desc);fflush(stdout);	
	
	
	
	CALLAPI(AOM_ask_value_string(partRevTag,"t5_PartType",&itemId12prtType));//Part Type
	printf("\nSAN:creCEDPart:12 digit part no Part Type===>%s\n",itemId12prtType);fflush(stdout);
	
	CALLAPI(AOM_ask_value_string(partRevTag,"t5_ProjectCode",&itemId12prjcode));//Part Type
	printf("\nSAN:creCEDPart:12 digit part no Project Code===>%s\n",itemId12prjcode);fflush(stdout);
	
	
	CALLAPI(AOM_ask_value_string(partRevTag,"t5_DesignGrp",&itemId12DsgGrp));//Part Type
	printf("\nSAN:creCEDPart:12 digit part no Design Group===>%s\n",itemId12DsgGrp);fflush(stdout);
	
	//TCTYPE_find_type("Design Revision", NULL, &rev_type_tag);//T5_SparKitRevision
	
	
	
	TCTYPE_find_type("T5_SparKitRevision", NULL, &rev_type_tag);
	if(rev_type_tag!=NULLTAG)
	{
		
		tag_t cedLatestPartTag = NULLTAG;
		printf("\nSAN:creCEDPart:rev_type_tag Tag Found."); fflush(stdout);
		
		
		char* cedPartNo=(char *) MEM_alloc(20);
		tc_strcpy(cedPartNo,"");
		tc_strcpy(cedPartNo,itemId12);
		tc_strcat(cedPartNo,"35");
		
		printf("\nSAN:creCEDPart:14 digit  CED part no===>%s\n",cedPartNo);fflush(stdout);
		
		

		printf("\nCreating CED coated parts NR revision- first time.....\n");fflush(stdout);
		TCTYPE_construct_create_input(rev_type_tag, &rev_create_input_tag);
		AOM_set_value_string(rev_create_input_tag, "object_name", cedPartNo);
		AOM_set_value_string(rev_create_input_tag, "item_revision_id", "NR;1"); //itemId12rev
		
		AOM_set_value_string(rev_create_input_tag, "object_desc", ceditemId12desc); //itemId12rev
		AOM_set_value_string(rev_create_input_tag, "t5_PartType", itemId12prtType); //itemId12rev
		AOM_set_value_string(rev_create_input_tag, "t5_ProjectCode", itemId12prjcode); //itemId12rev
		AOM_set_value_string(rev_create_input_tag, "t5_DesignGrp", itemId12DsgGrp); //itemId12rev
		
		TCTYPE_find_type("T5_SparKit", NULL, &item_type_tag);
		if(item_type_tag!=NULLTAG)
		{
				printf("\nSAN:creCEDPart: item_type_tag Tag Found."); fflush(stdout);
				
				
		}
		TCTYPE_construct_create_input(item_type_tag, &item_create_input_tag);

		AOM_set_value_string(item_create_input_tag, "item_id", cedPartNo);
		AOM_set_value_string(item_create_input_tag, "object_name", cedPartNo);
		AOM_set_value_tag(item_create_input_tag, "revision",rev_create_input_tag);

		//Create CED Part and its  Revision
		TCTYPE_create_object(item_create_input_tag, &cedPartMaster);
		
		if(cedPartMaster!=NULLTAG)
		{
			printf("\nSAN:creCEDPart: CED Part created."); fflush(stdout);					 
			AOM_save(cedPartMaster);				
		}
		
		
		//Get Item revision
		tag_t * ced_rev_list = NULLTAG;
		int ced_item_count = 0;
		tag_t cedPartRev = NULLTAG;
		ITEM_list_all_revs (cedPartMaster, &ced_item_count, &ced_rev_list);
		
		if(ced_item_count>0)
		{
			cedPartRev = ced_rev_list[0];
			*newCedPart = cedPartRev;
		}
		
		
	}


	return ifail;
}




int addStatusToTag(tag_t partTag,char* plant)
{
	int ifail = ITK_ok;
	tag_t   release_status =NULLTAG;
	char* ReleaseStatus = NULL;
	logical retain_released_date = TRUE;
	
	//APLC Working - T5_LcsAPLWrkg
	//APLA Working - T5_LcsAPLaWrkg
	
	//STDSIC Working- T5_LcsSTDWrkg
	//STDSIC Released - T5_LcsStdRlzd
	
	//ERC Released - T5_LcsErcRlzd
	
	if(tc_strcmp(plant,"CAR")==0 || tc_strcmp(plant,"PCBU")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLC Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}
	else if(tc_strcmp(plant,"CVBU JSR")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLjWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLJ Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLjWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}
	
	else if(tc_strcmp(plant,"CVBU PUNE")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLpWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLP Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLpWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}
	else if(tc_strcmp(plant,"DHARWAD")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLdWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLD Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLdWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}	
	else if(tc_strcmp(plant,"PCBU and CVBU")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLpWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLP Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLpWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}
	else if(tc_strcmp(plant,"CVBU PNR")==0 || tc_strcmp(plant,"SMALLCAR PNR")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLuWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLU Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLuWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}
	else if(tc_strcmp(plant,"SMALLCAR AHD")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLaWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLA Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLaWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}
	else if(tc_strcmp(plant,"PUVBU")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLvWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLV Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLvWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}
	else if(tc_strcmp(plant,"ALL")==0)
	{
		CALLAPI(CR_create_release_status("T5_LcsAPLWrkg",&release_status));
		CALLAPI( AOM_ask_name(release_status, &ReleaseStatus));

		if((strcmp(ReleaseStatus,"APLC Working")==0)||strcmp(ReleaseStatus,"T5_LcsAPLWrkg")==0 )
		{
			CALLAPI(EPM_add_release_status(release_status,1,&partTag,retain_released_date)); 
		}
	}
	return ifail;
}



int creCEDCoatedParts(tag_t* partRevList, int cnt)
{
	int ifail = ITK_ok;
	int i=0;
	printf("\n\n*******************Inside CED PART CREATION LOGIC*********************************\n");fflush(stdout);	
	printf("\ncnt===>%d\n",cnt);fflush(stdout);
	
	
	
	tag_t partRev = NULLTAG;
	for(i=0;i<cnt;i++)
	{
		char* partnm = NULL;
		char* itmid = NULL;
		tag_t partMstr = NULLTAG;
		
		partRev = partRevList[i];
		
		CALLAPI(AOM_ask_value_string(partRev,"object_string",&partnm))
		printf("\n111...Creating CED Coated for part %s\n",partnm);fflush(stdout);
		
		
		CALLAPI(ITEM_ask_item_of_rev(partRev,&partMstr));
		CALLAPI(AOM_ask_value_string(partMstr,"item_id",&itmid))
		printf("\n222..Part Master ID %s\n",itmid);fflush(stdout);
		
		char* cedPartNo=(char *) MEM_alloc(20);
		tc_strcpy(cedPartNo,"");
		tc_strcpy(cedPartNo,itmid);
		tc_strcat(cedPartNo,"35");

		printf("\n333...14 digit  CED part no===>%s\n",cedPartNo);fflush(stdout);
		
		int cnt1=0;
		tag_t* cedTags = NULLTAG;
		CALLAPI(AOM_ask_value_tags(partRev,"T5_HasSpareKit",&cnt1,&cedTags));
		
		printf("\n\ni===>%d\n",i);fflush(stdout);
		printf("\n4444..... cnt1===>%d\n",cnt1);fflush(stdout);
		
		logical flagCEDPartCre = false;
		if(cnt1>0)
		{
			printf("\n5555... Already having CED coated parts and is related to the part revision\n");fflush(stdout);
			
			printf("\nCheck if correct ced coated part is attached to the part rev\n");fflush(stdout);
			int k =0;
			logical flag = false;
			logical flag1 = false;
			for(k=0;k<cnt1;k++)
			{
				char* cedid = NULL;
				flag = false;
				CALLAPI(AOM_ask_value_string(cedTags[k],"item_id",&cedid));
				printf("\ncedPartNo=%s cedid=%s\n",cedPartNo,cedid);fflush(stdout);
				if(tc_strcmp(cedPartNo,cedid)==0)
				{
					printf("\nAlready correct CED part attached to the part rev, so skip this part revision...\n");fflush(stdout);
					flag = true;
					flag1 = true;
					//Check correct master is attached to the ced part if not then attach it.
					printf("\nCheck correct master is attached to the ced part if not then attach it.\n");fflush(stdout);
					int cnt3 = 0;
					tag_t* prtmstrTags = NULLTAG;
					
					CALLAPI(AOM_ask_value_tags(cedTags[k],"T5_SpareKitForPart",&cnt3,&prtmstrTags));
					
					if(cnt3>0)
					{
						flag1 = true;
						int l=0;
						tag_t  prtMstrObj= NULLTAG;
						for(l=0;l<cnt3;l++)
						{
							char* prtmstid = NULL;
							CALLAPI(AOM_ask_value_string(prtmstrTags[l],"item_id",&prtmstid));
							printf("\nitmid=%s prtmstid=%s\n",itmid,prtmstid);fflush(stdout);
							
							if(tc_strcmp(itmid,prtmstid)==0)
							{
								printf("\nCorrect Part master is already attched to the ced part\n");fflush(stdout);
								flag1 = false;
								break;
							}
						}
					}
						
					if(flag1)
					{
						//Not correct master is attched to ced part so attach it.
						printf("\nNot correct master is attched to ced part so attach correct master to it.\n");fflush(stdout);
						CALLAPI(relatePrimaryWithSecondaryObj(cedTags[k],partMstr,"T5_SpareKitForPart"));
						printf("\nPart Master %s is related to the CED Part %s\n",itmid,cedPartNo);fflush(stdout);							
						
					}
					
					
					
					break;
				}
				
				
			}
			if(flag)
			{
				//skip this part. all relations are correct
				printf("\nSkip the part %s for ced part creation...\n",partnm);fflush(stdout);
				continue;
			}
			else
			{
				printf("\nNot correct CED coated part is present/related to the part revision...\n");fflush(stdout);
				flagCEDPartCre = true;
			}
			
		}
		printf("\nFlag for CED Part creation(flagCEDPartCre): %d\n");fflush(stdout);
		if(flagCEDPartCre || cnt1==0)
		{
			tag_t cedLatestPartTag = NULLTAG;
			CALLAPI(queryLatestSpareKitRev(cedPartNo,&cedLatestPartTag));
			if(cedLatestPartTag!=NULLTAG)
			{
				char* cedPartNo1 = NULL;
				CALLAPI(AOM_ask_value_string(cedLatestPartTag,"item_id",&cedPartNo1))
				if(tc_strcmp(cedPartNo,cedPartNo1)==0)
				{
					printf("\n6666... Already having CED coated parts but not related to the part revision\n");fflush(stdout);
					//Relate the CED Part with partrev
					
					//CALLAPI(relatePrimaryWithSecondaryObj(partRev,cedLatestPartTag,"T5_HasSpareKit")); //sanjoy commenting - due to sparekit part relation validation issue - sco 404
					printf("\nPart Revision %s is related to the CED Part %s\n",partnm,cedPartNo);fflush(stdout);
					
					//Check master is attached to the ced part rev
					int cnt2 = 0;
					tag_t* mstrTags = NULLTAG;
					CALLAPI(AOM_ask_value_tags(cedLatestPartTag,"T5_SpareKitForPart",&cnt2,&mstrTags));
					printf("\n7777..... cnt2===>%d\n",cnt2);fflush(stdout);
					if(cnt2==0)
					{
						printf("\n8888.. No master related to spare kit\n");fflush(stdout);
						CALLAPI(relatePrimaryWithSecondaryObj(cedLatestPartTag,partMstr,"T5_SpareKitForPart"));
						printf("\nPart Master %s is related to the CED Part %s\n",itmid,cedPartNo);fflush(stdout);
					}
					if(cnt2>0)
					{
						logical flg = true;
						//Check if any other master is attached or not
						printf("\n9999.. Check if any other master is attached. If attached then attach then attach the correct one..\n");fflush(stdout);
						int j=0;
						for(j=0;j<cnt2;j++)
						{
							char* mstrid = NULL;
							flg = true;
							CALLAPI(AOM_ask_value_string(mstrTags[j],"item_id",&mstrid))
							printf("\nitmid=%s mstrid=%s\n",itmid,mstrid);fflush(stdout);
							if(tc_strcmp(itmid,mstrid)==0)
							{
								printf("\naaa.. Already master is attached...\n");fflush(stdout);
								flg = false;
								break;
							}
						}
						
						if(flg)
						{
							printf("\nbbbb.. No master related to spare kit\n");fflush(stdout);
							CALLAPI(relatePrimaryWithSecondaryObj(cedLatestPartTag,partMstr,"T5_SpareKitForPart"));
							printf("\nPart Master %s is related to the CED Part %s\n",itmid,cedPartNo);fflush(stdout);
		
						}
					}
				}
				
			}
			else
			{
				printf("\nNo CED Part . Need to create it...\n");fflush(stdout);
				tag_t newCedPart = NULLTAG;
				CALLAPI(createNewCEDCoatedPart(partRev, "ALL", &newCedPart))
				if(newCedPart!=NULLTAG)
				{
					//Relate this part to the 12 digit part. //No need to revise
					//CALLAPI(relatePrimaryWithSecondaryObj(partRev,newCedPart,"T5_HasSpareKit"));//sanjoy commenting - due to sparekit part relation validation issue - sco 404
					//Also relate this ced part with master.
					CALLAPI(relatePrimaryWithSecondaryObj(newCedPart,partMstr,"T5_SpareKitForPart"));
					
					//Add status
					CALLAPI(addStatusToTag(newCedPart,"ALL"));
				}				
				
			}
			
		}
		
		
		if(cedPartNo!=NULL)MEM_free(cedPartNo);
	}
	
	printf("\n\n*******************Finished CED PART CREATION LOGIC*********************************\n");fflush(stdout);
	return ifail;
}


int isDR3PandAboveDML(tag_t dmlTag, logical *result)
{
	int ifail = ITK_ok;
	*result = FALSE;
	if(dmlTag != NULLTAG)
	{
		char* objTyp = NULL;
		CALLAPI(AOM_ask_value_string(dmlTag,"object_type",&objTyp));
		printf("\nDML object type: [%s]", objTyp);fflush(stdout);
		if(tc_strcmp(objTyp,"ChangeRequestRevision")==0)
		{
			char* relzType = NULL;
			char* dmlProjCode = NULL;
			char* plntToPlnt = NULL;
			char* dmlDrStatus = NULL;
			char* tagetPlant = NULL;
			
			CALLAPI(AOM_ask_value_string(dmlTag,"t5_rlstype",&relzType));
			CALLAPI(AOM_ask_value_string(dmlTag,"t5_cprojectcode",&dmlProjCode));
			CALLAPI(AOM_ask_value_string(dmlTag,"t5_PlntToPlnt",&plntToPlnt));
			CALLAPI(AOM_ask_value_string(dmlTag,"t5_cDRstatus",&dmlDrStatus));
			CALLAPI(AOM_ask_value_string(dmlTag,"t5_TargetPlant",&tagetPlant));
			printf("\nRelease Type: %s\nDML Project Code: %s\nFromPlantToPlant: %s\nDml Dr Status:%s\nTarget Pant: %s\n",relzType,dmlProjCode,plntToPlnt,dmlDrStatus,tagetPlant);fflush(stdout);
			
			if(tc_strcmp(relzType,"TODR")==0)
			{
				
				if(plntToPlnt!=NULL)
				{
					if(tc_strcmp(plntToPlnt,"AR3 to AR3P")==0 || tc_strcmp(plntToPlnt,"DR3 to DR3P")==0  ||tc_strcmp(plntToPlnt,"AR3P to AR4")==0 || tc_strcmp(plntToPlnt,"DR3P to DR4")==0 || tc_strcmp(plntToPlnt,"DR3 to DR4")==0 || tc_strcmp(plntToPlnt,"AR3 to AR4")==0)
					{
						*result = TRUE;
					}
				}
				
			}
			else if(tc_strcmp(relzType,"Veh")==0 || tc_strcmp(relzType,"SCR")==0)
			{
				if(dmlDrStatus!=NULL)
				{
					if(tc_strcmp(dmlDrStatus,"DR3P")==0 || tc_strcmp(dmlDrStatus,"AR3P")==0 || tc_strcmp(dmlDrStatus,"DR4")==0 || tc_strcmp(dmlDrStatus,"AR4")==0 || tc_strcmp(dmlDrStatus,"DR5")==0 || tc_strcmp(dmlDrStatus,"AR5")==0)
					{
						*result = TRUE;
					}
				}
			}
			else
			{
				printf("\nThe DML Release Type is not Veh/TODR/SCR\n");fflush(stdout);
			}
			
		}
		else
		{
			printf("\nNot a valid DML Revision object\n");fflush(stdout);
		}
	}		
	
	
	return ifail;
}


int ITK_user_main(int argc, char* argv[])
{
	
	int ifail = ITK_ok;
	
	// char* platformId = NULL;
	// char* variantRuleNm = NULL;
	char* dmlNo = NULL;

	printf("\n SAN:tm_createCedCoatedParts: *********** Start Of  Main function ************* \n");fflush(stdout);
	
	//dmlTaskNo = ITK_ask_cli_argument( "-d=");
	//san -pl=X4 -vr=5442SUPSVR13_NR
	//san -pl=X4 -vr=54455124000R_F
	//san -pl=X4 -vr=54455524000R_B
	//./tm_createCedCoatedParts -d=18PP141219
	
	//platformId = ITK_ask_cli_argument( "-pl=");
	
	//variantRuleNm = ITK_ask_cli_argument( "-vr=");
	
	dmlNo = ITK_ask_cli_argument( "-d=");
	
	
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	
	if(ITK_auto_login() == ITK_ok)	
	{
		tag_t user_tag = NULLTAG;
		char* username = NULL;
		// tag_t svrTag = NULLTAG;
		// tag_t pltFrmTag = NULLTAG;		
		tag_t* moduleList = NULLTAG;
		tag_t dmlTag = NULLTAG;
		int cnt=0;
				
		ITK_CALL(ITK_set_journalling( TRUE ));
		printf("\n\nSAN:tm_createCedCoatedParts: Login successfull.\n");fflush(stdout);		
		printf("\nSAN:tm_createCedCoatedParts: argc==>%d\n",argc);		
		printf("\nSAN:tm_createCedCoatedParts: Input argument: DML No[%s]\n",dmlNo);fflush(stdout);
		POM_get_user(&username,&user_tag);
		printf("\nSAN:tm_createCedCoatedParts: Session User Name[%s]\n",username);fflush(stdout);		
		
		
		//Snapshot logic
		//16PP251682
		
		//Get DML task object - san -d=19PP291019_00  /13PP253F07_00, 
		//san -d=13PP253F07_00
		//ced -d=16PP251681_00
		//bomSolve -pl=X4 -vr=54455524000R_B
		//cedgm -d=13PP253G47_00
		
		//If CCVC then  check for snapshot
		//if no snapshot then solve the bom to get the unit module
		
		//if VC then expannd 1st level to get the Vehicle and then check if snapshot is present, if not then solve the bom
		if(dmlNo!=NULL)
		{
			CALLAPI(getItemRevObject(dmlNo,"ChangeRequestRevision",&dmlTag));fflush(stdout);
			if(dmlTag==NULLTAG)
			{
				printf("\nSAN:tm_createCedCoatedParts: No DML  found with name as %s\n",dmlNo);
				return ifail;
			}
			else
			{
				
				logical drFlg = false;
				
				printf("\nSAN:tm_createCedCoatedParts: Got DML  Tag with name as %s\n",dmlNo);
				
				//Execute the logic here.
				
				//Check for DR status of the DML.

				CALLAPI(isDR3PandAboveDML(dmlTag,&drFlg));
				printf("\nSAN:tm_createCedCoatedParts: DR flag for creating CED coated parts: %d\n",drFlg);fflush(stdout);
				
				if(drFlg)
				{
					//valid DR status - execute the logic for CED part creation

					int taskCnt = 0;
					tag_t* dmlTasks = NULLTAG;
					tag_t taskTag = NULLTAG;
					char* dmlRlzType = NULL;
					int partModelListCnt = 0;
					PartModel * partModelList;					
					CALLAPI(AOM_ask_value_string(dmlTag,"t5_rlstype",&dmlRlzType));
					printf("\n Release Type: %s\n",dmlRlzType);fflush(stdout);
					
					//get the dml task object
					CALLAPI(AOM_ask_value_tags(dmlTag,"T5_DMLTaskRelation",&taskCnt,&dmlTasks));
					
					printf("\nSAN:tm_createCedCoatedParts: No of Tasks of the DML is %d\n",taskCnt);fflush(stdout);
					
					if(taskCnt>0)
					{
						int iTsk= 0;
						for(iTsk=0;iTsk<taskCnt;iTsk++)
						{
							char* taskType = NULL;
							taskTag = dmlTasks[iTsk];
							CALLAPI(AOM_ask_value_string(taskTag,"object_type",&taskType));
							printf("\nSAN:tm_createCedCoatedParts: Class of Task is %s\n",taskType);fflush(stdout);

							if(tc_strcmp(taskType,"T5_ChangeTaskRevision")==0)
							{
								char* taskNo = NULL;
								
								CALLAPI(AOM_ask_value_string(taskTag,"item_id",&taskNo));
								printf("\nSAN:tm_createCedCoatedParts: DML Task Number %s\n",taskNo);fflush(stdout);
								
								if(tc_strcmp(dmlRlzType,"Veh")==0)//For Vehicle Release DML
								{
									int partCnt = 0;
									tag_t* partTags = NULLTAG;
									CALLAPI(AOM_ask_value_tags(taskTag,"CMReferences",&partCnt,&partTags))
									printf("\nSAN:tm_createCedCoatedParts: No of parts in the dml task[%s] : %d\n",taskNo,partCnt);fflush(stdout);
									
									
									
								}
								else if(tc_strcmp(dmlRlzType,"TODR")==0)//For GM DML
								{
									int partCnt = 0;
									tag_t* partTags = NULLTAG;
									
									printf("\nSAN:tm_createCedCoatedParts: For Gate Maturation DML logic\n");fflush(stdout);
									CALLAPI(AOM_ask_value_tags(taskTag,"CMReferences",&partCnt,&partTags))
									printf("\nSAN:tm_createCedCoatedParts: No of parts in the task[%s] : %d\n",dmlNo,partCnt);fflush(stdout);
									
									if(partCnt>0)
									{
										int iPrt =0;
										tag_t partTag = NULLTAG;
										char* sPartClass = NULL;
										char* sPartNo = NULL;
										char* sPartType = NULL;
										
										for(iPrt=0;iPrt<partCnt;iPrt++)
										{
											if(iPrt>=1) continue;
											partTag = partTags[iPrt];
											CALLAPI(AOM_ask_value_string(partTag,"object_type",&sPartClass));
											printf("\nSAN:tm_createCedCoatedParts: Part Class: [%s]",sPartClass);fflush(stdout);
											
											if(tc_strcmp(sPartClass,"Design Revision")==0)
											{
												CALLAPI(AOM_ask_value_string(partTag,"item_id",&sPartNo));
												CALLAPI(AOM_ask_value_string(partTag,"t5_PartType",&sPartType));
												printf("SAN:tm_createCedCoatedParts: Part No[%s] Part Type: [%s]\n",sPartNo,sPartType);fflush(stdout);
												
												if(tc_strcmp(sPartType,"CCVC")==0)
												{
													tag_t CCVSnpSht_Rel_Type = NULLTAG;													
													tag_t* snapShotTags = NULLTAG;
													int snpShtCnt = 0;
													tag_t snapShotTag = NULLTAG;
													tag_t snapShotTypeTag = NULLTAG;
													char   snapShotTypeName[TCTYPE_name_size_c+1];
													char* sSnapShotNo = NULL;
													tag_t  ModlRevTag = NULLTAG;
													tag_t snapBomWindow = NULLTAG;
													tag_t snapBOMlineTag = NULLTAG;
													int chldSnpBOMlineCnt = 0;
													tag_t* snpBomLineTags = NULLTAG;
													int ar = 0;
													tag_t snpBomLineTag = NULLTAG;
													char* sArcModuleNo = NULL;
													tag_t ArcMdlNo_tag = NULLTAG;
													tag_t ArcMdlNo_tg = NULLTAG;													
													//if part type == CCVC then check for snapshot
													printf("\n %s is a CCVC vehicle-- Check for the snapshot\n", sPartNo);fflush(stdout);
													
													CALLAPI(GRM_find_relation_type("T5_PartHasSnapShot", &CCVSnpSht_Rel_Type));
													if(CCVSnpSht_Rel_Type!=NULLTAG)
													{
														snpShtCnt = 0;
														CALLAPI(GRM_list_secondary_objects_only(partTag,CCVSnpSht_Rel_Type,&snpShtCnt,&snapShotTags));
														printf("\n SAN:tm_createCedCoatedParts: No of Snapshots attached to ccvc[%s] is [%d]", sPartNo,snpShtCnt);fflush(stdout);
														
														if(snpShtCnt>0)
														{
															snapShotTag = snapShotTags[0];
															
															CALLAPI(TCTYPE_ask_object_type(snapShotTag,&snapShotTypeTag));
															CALLAPI(TCTYPE_ask_name(snapShotTypeTag,snapShotTypeName));
															printf("\n SAN:tm_createCedCoatedParts: snapShotTypeName:: [%s] \n",snapShotTypeName); fflush(stdout);
															
															if(tc_strcmp(snapShotTypeName,"Snapshot")==0)
															{
																CALLAPI(AOM_ask_value_string(snapShotTag,"object_name",&sSnapShotNo));
																printf("\n SAN:tm_createCedCoatedParts: SnapshotName:[%s]\n",sSnapShotNo);fflush(stdout);
																
																CALLAPI(AOM_ask_value_tag(snapShotTag,"topLine",&ModlRevTag));
																
																CALLAPI(BOM_create_window_from_snapshot(snapShotTag,&snapBomWindow));
																
																CALLAPI(BOM_set_window_top_line(snapBomWindow, NULLTAG, ModlRevTag, NULLTAG, &snapBOMlineTag));
																CALLAPI(BOM_line_ask_child_lines(snapBOMlineTag, &chldSnpBOMlineCnt, &snpBomLineTags));
																
																printf("\n SAN:tm_createCedCoatedParts: CCV Arch Node Count:%d:",chldSnpBOMlineCnt);fflush(stdout);
																
																for(ar =0 ; ar < chldSnpBOMlineCnt; ar++)
																{
																	snpBomLineTag = snpBomLineTags[ar];
																	CALLAPI(AOM_ask_value_string(snpBomLineTag,"bl_item_item_id",&sArcModuleNo));
																	//printf("\n Arc_Node:%d:%d: sArcModuleNo:%s ......\n",chldSnpBOMlineCnt,ar,sArcModuleNo);fflush(stdout);
																	ArcMdlNo_tag= t5GetItemRevison(sArcModuleNo);
																	if(ArcMdlNo_tag==NULLTAG)
																	{
																		printf("\n CCV:ArcMdlNo_tag is NULL.\n");fflush(stdout);
																		//return 0;
																	}
																	else
																	{
																		CALLAPI((ITEM_ask_latest_rev (ArcMdlNo_tag, &ArcMdlNo_tg)));
																		if(ArcMdlNo_tg==NULLTAG)
																		{
																			printf("\n CCV:Object not found ArcMdlNo_tg...!!\n");fflush(stdout);
																			//return 0;
																		}
																		else
																		{
																			//Get Modules from Arc Nodes
																			tag_t MdlBOMwindow = NULLTAG;
																			tag_t MdltagRule = NULLTAG;
																			int MdlCount = 0;
																			tag_t* POclosRul = NULLTAG;
																			tag_t POcloseRul_tag = NULLTAG;
																			tag_t MdlBOMline = NULLTAG;
																			int MdlchildCount=0;
																			tag_t* MdlTopBLchildLine = NULLTAG;
																			char* sPartNum = NULLTAG;
																			CALLAPI(BOM_create_window(&MdlBOMwindow));
																			CALLAPI(CFM_find("ERC release and above", &MdltagRule));
																			CALLAPI(BOM_set_window_config_rule( MdlBOMwindow, MdltagRule ))
																			CALLAPI(PIE_find_closure_rules("BOMLineskipforunconfigured",PIE_TEAMCENTER, &MdlCount, &POclosRul ))
																			if (MdlCount > 0)
																			{
																				POcloseRul_tag = POclosRul[0];
																				//printf ("CCV: POcloseRul_tag closure rule found \n");fflush(stdout);
																			}
																			CALLAPI(BOM_window_set_closure_rule(MdlBOMwindow,POcloseRul_tag, 0, NULL,NULL ));
																			CALLAPI(BOM_set_window_top_line(MdlBOMwindow, NULLTAG, ArcMdlNo_tg, NULLTAG, &MdlBOMline))
																			CALLAPI(BOM_line_ask_child_lines(MdlBOMline, &MdlchildCount, &MdlTopBLchildLine))
																			printf("\n Arc_Node:%d:%d: sArcModuleNo:%s ",chldSnpBOMlineCnt,ar,sArcModuleNo);fflush(stdout);
																			printf("\t No of Module:%d ",MdlchildCount);fflush(stdout);
																			
																			CALLAPI(BOM_close_window(MdlBOMwindow));
																			
																			CALLAPI(AOM_ask_value_string(ArcMdlNo_tg,"item_id",&sPartNum));
																			printf("\t Part No to be expanded:%s ",sPartNum);fflush(stdout);
																			//tmExpandPartAndGetCEDPartLists(ArcMdlNo_tg,99, &partModelList, &partModelListCnt, "view", "ERC release and above", "BOMLineskipforunconfigured");
																			tmExpandPartAndGetCEDPartLists(ArcMdlNo_tg,99, &partModelList, &partModelListCnt, "view", "ERC release and above", "");
																			//tmExpandPartAndGetCEDPartLists(ArcMdlNo_tg,99, &partModelList, &partModelListCnt, "view", "Latest Working", "");
																			
																		}
																		
																	}
																	
																	
																}
																
																
															}
															
															//tmExpandPartAndGetCEDPartLists(snapShotTag,99, &partModelList, &partModelListCnt, "view", "Latest Working", "");
															
															
														}
														else
														{
															//solve the bom - TODO
														}
														
													}
													else
													{
														//Solve the bom to get unit module - TODO
													}												
												}
																								
											}
											else
											{
												printf("\n SAN:tm_createCedCoatedParts: Part Class:[%s] is not Valid. Skip this part",sPartClass);fflush(stdout);
											}											
										}
									}
									
									
								}
								else
								{
									//Not valid DML Release type.
								}
								
							}
							else
							{
								printf("\nSAN:tm_createCedCoatedParts: Task is not of type T5_ChangeTaskRevision. So Skip this...\n");fflush(stdout);
							}
							
						}
							
					}
					
					
					
					//CALLAPI(AOM_ask_value_tags(dmlTag,"CMReferences",&partCnt,&partTags))
					//printf("\n SAN:tm_createCedCoatedParts: No of parts in the task[%s] : %d\n",dmlNo,partCnt);fflush(stdout);					
					
				

					printf("\n No of eligible CED parts [%d]\n",partModelListCnt);fflush(stdout);
					
					//print eligilbe parts for ced creation
					
					int elgCnt=0;
					tag_t elgPartTag = NULLTAG;
					char* sEligPartNum = NULL;
					tag_t* eligiblePartTagList = NULLTAG;
					int cedpartCnt = 0;	
					for(elgCnt=0;elgCnt<partModelListCnt;elgCnt++)
					{
						elgPartTag = partModelList[elgCnt]->childItemRev;
						//setAddTagObj(&cedpartCnt,&eligiblePartTagList,elgPartTag);
						CALLAPI(AOM_ask_value_string(elgPartTag,"object_string",&sEligPartNum));
						printf("\n Part[%d]:%s ",elgCnt,sEligPartNum);fflush(stdout);
						
					}
					
					printf("\n End of Snapshot logic...\n");fflush(stdout);
					printf("\n No of eligible CED parts 1111111 [%d]\n",cedpartCnt);fflush(stdout);
					//CALLAPI(creCEDCoatedParts(eligiblePartTagList,cedpartCnt));				
				}
				else
				{
					//Not Valid DR status for CED part creation.
					
					printf("\nSAN:tm_createCedCoatedParts: NOT VALID DR STATUS OF THE DML. So NO CED Part Creation\n");fflush(stdout);
				}
				
				
				
				
				
				
				
				
				
				
				
			}
			
		}
		else
		{
			printf("\nCheck the input parameter of the shell script.\nWrong Input parameter as dml no is NULL\n");fflush(stdout);
		}
		
		
		
/* 		return ifail;
		
		if(dmlTag!=NULLTAG)
		{
			int partCnt = 0;
			tag_t* partTags = NULLTAG;
			int i=0;
			tag_t partTag = NULLTAG;
			char* sPartType = NULL;
			char* sPartClass = NULL;
			char* sPartNo = NULL;
			tag_t CCVSnpSht_Rel_Type = NULLTAG;
			
			tag_t* snapShotTags = NULLTAG;
			int snpShtCnt = 0;
			tag_t snapShotTag = NULLTAG;
			int partModelListCnt = 0;
			PartModel * partModelList;
			
			tag_t snapShotTypeTag = NULLTAG;
			char   snapShotTypeName[TCTYPE_name_size_c+1];
			char* sSnapShotNo = NULL;
			tag_t  ModlRevTag = NULLTAG;
			tag_t snapBomWindow = NULLTAG;
			tag_t snapBOMlineTag = NULLTAG;
			int chldSnpBOMlineCnt = 0;
			tag_t* snpBomLineTags = NULLTAG;
			int ar = 0;
			tag_t snpBomLineTag = NULLTAG;
			char* sArcModuleNo = NULL;
			tag_t ArcMdlNo_tag = NULLTAG;
			tag_t ArcMdlNo_tg = NULLTAG;
			
			//CALLAPI(AOM_ask_value_tags(dmlTag,"CMHasSolutionItem",&partCnt,&partTags))
			
			CALLAPI(AOM_ask_value_tags(dmlTag,"CMReferences",&partCnt,&partTags))
			printf("\n SAN:tm_createCedCoatedParts: No of parts in the task[%s] : %d\n",dmlNo,partCnt);fflush(stdout);
			if(partCnt>0)
			{
				for(i=0;i<partCnt;i++)
				{
					partTag = partTags[i];
					CALLAPI(AOM_ask_value_string(partTag,"object_type",&sPartClass));
					printf("\n SAN:tm_createCedCoatedParts: Part Class: [%s]",sPartClass);fflush(stdout);
					
					if(tc_strcmp(sPartClass,"Design Revision")==0)
					{
						CALLAPI(AOM_ask_value_string(partTag,"item_id",&sPartNo));
						CALLAPI(AOM_ask_value_string(partTag,"t5_PartType",&sPartType));
						printf(" SAN:tm_createCedCoatedParts: Part No[%s] Part Type: [%s]\n",sPartNo,sPartType);fflush(stdout);
						
						//only for part number - 54426424R
						
						if(tc_strstr(sPartNo,"54426424R")!=NULL)
						{
							printf("\nAllowed part - 54426424R *********************************************\n");fflush(stdout);
						}
						else
						{
							printf("\n NOT Allowed part - skipp this part *********************************************\n");fflush(stdout);
							continue;
						}
						

						
						if(tc_strcmp(sPartType,"V")==0)
						{
							printf("\n %s is a CCVC vehicle-- Check for the snapshot\n", sPartNo);fflush(stdout);
							
							CALLAPI(GRM_find_relation_type("T5_PartHasSnapShot", &CCVSnpSht_Rel_Type));
							if(CCVSnpSht_Rel_Type!=NULLTAG)
							{
								snpShtCnt = 0;
								CALLAPI(GRM_list_secondary_objects_only(partTag,CCVSnpSht_Rel_Type,&snpShtCnt,&snapShotTags));
								printf("\n SAN:tm_createCedCoatedParts: No of Snapshots attached to ccvc[%s] is [%d]", sPartNo,snpShtCnt);fflush(stdout);
								
								if(snpShtCnt>0)
								{
									snapShotTag = snapShotTags[0];
									
									CALLAPI(TCTYPE_ask_object_type(snapShotTag,&snapShotTypeTag));
									CALLAPI(TCTYPE_ask_name(snapShotTypeTag,snapShotTypeName));
									printf("\n SAN:tm_createCedCoatedParts: snapShotTypeName:: [%s] \n",snapShotTypeName); fflush(stdout);
									
									if(tc_strcmp(snapShotTypeName,"Snapshot")==0)
									{
										CALLAPI(AOM_ask_value_string(snapShotTag,"object_name",&sSnapShotNo));
										printf("\n SAN:tm_createCedCoatedParts: SnapshotName:[%s]\n",sSnapShotNo);fflush(stdout);
										
										CALLAPI(AOM_ask_value_tag(snapShotTag,"topLine",&ModlRevTag));
										
										CALLAPI(BOM_create_window_from_snapshot(snapShotTag,&snapBomWindow));
										
										CALLAPI(BOM_set_window_top_line(snapBomWindow, NULLTAG, ModlRevTag, NULLTAG, &snapBOMlineTag));
										CALLAPI(BOM_line_ask_child_lines(snapBOMlineTag, &chldSnpBOMlineCnt, &snpBomLineTags));
										
										printf("\n SAN:tm_createCedCoatedParts: CCV Arch Node Count:%d:",chldSnpBOMlineCnt);fflush(stdout);
										
										for(ar =0 ; ar < chldSnpBOMlineCnt; ar++)
										{
											snpBomLineTag = snpBomLineTags[ar];
											CALLAPI(AOM_ask_value_string(snpBomLineTag,"bl_item_item_id",&sArcModuleNo));
											//printf("\n Arc_Node:%d:%d: sArcModuleNo:%s ......\n",chldSnpBOMlineCnt,ar,sArcModuleNo);fflush(stdout);
											ArcMdlNo_tag= t5GetItemRevison(sArcModuleNo);
											if(ArcMdlNo_tag==NULLTAG)
											{
												printf("\n CCV:ArcMdlNo_tag is NULL.\n");fflush(stdout);
												//return 0;
											}
											else
											{
												CALLAPI((ITEM_ask_latest_rev (ArcMdlNo_tag, &ArcMdlNo_tg)));
												if(ArcMdlNo_tg==NULLTAG)
												{
													printf("\n CCV:Object not found ArcMdlNo_tg...!!\n");fflush(stdout);
													//return 0;
												}
												else
												{
													//Get Modules from Arc Nodes
													tag_t MdlBOMwindow = NULLTAG;
													tag_t MdltagRule = NULLTAG;
													int MdlCount = 0;
													tag_t* POclosRul = NULLTAG;
													tag_t POcloseRul_tag = NULLTAG;
													tag_t MdlBOMline = NULLTAG;
													int MdlchildCount=0;
													tag_t* MdlTopBLchildLine = NULLTAG;
													char* sPartNum = NULLTAG;
													CALLAPI(BOM_create_window(&MdlBOMwindow));
													CALLAPI(CFM_find("ERC release and above", &MdltagRule));
													CALLAPI(BOM_set_window_config_rule( MdlBOMwindow, MdltagRule ))
													CALLAPI(PIE_find_closure_rules("BOMLineskipforunconfigured",PIE_TEAMCENTER, &MdlCount, &POclosRul ))
													//CALLAPI(PIE_find_closure_rules("BOMLineSkipZeroQtyMaskUnconfig",PIE_TEAMCENTER, &MdlCount, &POclosRul ))
													if (MdlCount > 0)
													{
														POcloseRul_tag = POclosRul[0];
														//printf ("CCV: POcloseRul_tag closure rule found \n");fflush(stdout);
													}
													CALLAPI(BOM_window_set_closure_rule(MdlBOMwindow,POcloseRul_tag, 0, NULL,NULL ));
													CALLAPI(BOM_set_window_top_line(MdlBOMwindow, NULLTAG, ArcMdlNo_tg, NULLTAG, &MdlBOMline))
													CALLAPI(BOM_line_ask_child_lines(MdlBOMline, &MdlchildCount, &MdlTopBLchildLine))
													printf("\n Arc_Node:%d:%d: sArcModuleNo:%s ",chldSnpBOMlineCnt,ar,sArcModuleNo);fflush(stdout);
													printf("\t No of Module:%d ",MdlchildCount);fflush(stdout);
													
													int mdll =0;
													char* MdlNo = NULL;
													char* Mdl_DR = NULL;
													tag_t MdlLstRev_tag = NULLTAG;
													for(mdll =0 ; mdll < MdlchildCount; mdll++)
													{
														CALLAPI(AOM_ask_value_string(MdlTopBLchildLine[mdll],"bl_item_item_id",&MdlNo));
														CALLAPI(AOM_ask_value_string(MdlTopBLchildLine[mdll],"bl_Design Revision_t5_PartStatus",&Mdl_DR));
														printf("\n C10:Arc_Module:[%s] Total Arc_module:[%d] Taken Arc_module:[%d] Total Modules:[%d] Taken Module:[%d] Module no:[%s] Module DR:[%s]",sArcModuleNo,chldSnpBOMlineCnt,ar,MdlchildCount,mdll,MdlNo,Mdl_DR);fflush(stdout);
														//AAAAAAAAAAAAAA 
														
														int MdlchildCount1 = 0;
														tag_t* MdlTopBLchildLine1 = NULLTAG;
														CALLAPI(BOM_line_ask_child_lines(MdlTopBLchildLine[mdll], &MdlchildCount1, &MdlTopBLchildLine1))
														
														printf("\t ****************** No of Unit Bom :%d ",MdlchildCount1);fflush(stdout);
										
													}
													
													
													
													
													CALLAPI(BOM_close_window(MdlBOMwindow));
													
													CALLAPI(AOM_ask_value_string(ArcMdlNo_tg,"item_id",&sPartNum));
													printf("\t Part No to be expanded:%s ",sPartNum);fflush(stdout);
													//tmExpandPartAndGetCEDPartLists(ArcMdlNo_tg,99, &partModelList, &partModelListCnt, "view", "ERC release and above", "BOMLineskipforunconfigured");
													//tmExpandPartAndGetCEDPartLists(ArcMdlNo_tg,1, &partModelList, &partModelListCnt, "view", "", "");
													//tmExpandPartAndGetCEDPartLists(ArcMdlNo_tg,99, &partModelList, &partModelListCnt, "view", "Latest Working", "");
												}
												
											}
											
											
										}
										
										
									}
									
									//tmExpandPartAndGetCEDPartLists(snapShotTag,99, &partModelList, &partModelListCnt, "view", "Latest Working", "");
									
									
								}
								else
								{
									//solve the bom - TODO
								}
								
							}
							else
							{
								//Solve the bom to get unit module - TODO
							}
							
						}
					}
					else
					{
						printf("\n SAN:tm_createCedCoatedParts: Part Class:[%s] is not Valid. Skip this part",sPartClass);fflush(stdout);
					}
					
				}
			}
			
			
		
			printf("\n No of eligible CED parts [%d]\n",partModelListCnt);fflush(stdout);
			
			//print eligilbe parts for ced creation
			
			int elgCnt=0;
			tag_t elgPartTag = NULLTAG;
			char* sEligPartNum = NULL;
			tag_t* eligiblePartTagList = NULLTAG;
			int cedpartCnt = 0;	
			for(elgCnt=0;elgCnt<partModelListCnt;elgCnt++)
			{
				elgPartTag = partModelList[elgCnt]->childItemRev;
				//setAddTagObj(&cedpartCnt,&eligiblePartTagList,elgPartTag);
				CALLAPI(AOM_ask_value_string(elgPartTag,"object_string",&sEligPartNum));
				printf("\n Part[%d]:%s ",elgCnt,sEligPartNum);fflush(stdout);
				
			}
			
			printf("\n End of Snapshot logic...\n");fflush(stdout);
			printf("\n No of eligible CED parts 1111111 [%d]\n",cedpartCnt);fflush(stdout);
			//CALLAPI(creCEDCoatedParts(eligiblePartTagList,cedpartCnt));
		}

		
		//CALLAPI(creCEDCoatedParts(partModelList,partModelListCnt));
		
		 */
		
		
		//return ifail;
		
/* 		if(platformId==NULL || variantRuleNm == NULL)
		{
			printf("\n SAN:tm_createCedCoatedParts: Please enter variant rule name and platform id.\n SAN:tm_createCedCoatedParts: Exiting from the program....\n");fflush(stdout);
			return ifail;
		}
		
		
		
		
		//Get SVR tag
		CALLAPI(getGeneralObject(variantRuleNm,"VariantRule",&svrTag));fflush(stdout);
		if(svrTag==NULLTAG)
		{
			printf("\n SAN:tm_createCedCoatedParts: No Variant Rule found with name as %s\n",variantRuleNm);
			return ifail;
		}

		//Get Platform Revision Tag
		
		CALLAPI(getItemRevObject(platformId,"T5_PlatformRevision",&pltFrmTag));
		if(pltFrmTag==NULLTAG)
		{
			printf("\n SAN:tm_createCedCoatedParts: No Platform revision found with name as %s\n",platformId);fflush(stdout);
			return ifail;
		}		
		
		CALLAPI(tm_SolveAndGetUnitBom(pltFrmTag,svrTag,&cnt,&moduleList))
		
		printf("\n SAN:tm_createCedCoatedParts: No of Modules=====>%d\n",cnt);fflush(stdout);
		
		 */
		
		
	}	
	

	printf("\n SAN:tm_createCedCoatedParts: *********** End Of Main function ************* \n");fflush(stdout);	
	return ifail;
}



