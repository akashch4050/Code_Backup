/**********************************************************************************************************************************************************
**
** SOURCE FILE NAME		:	IFD&STD_Part_report.c
**
** Author				:	AWW917697 
**
** Date					:	14/9/2019                                                                                                                                                                                                             
**																																	                                                              
** command to run		:
**
**************************************************************************************************************************************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#define NUM_ENTRIES 1
#define TE_MAXLINELEN  128 
#include <ae/dataset.h>
#include <ae/dataset_msg.h>
#include <ai/sample_err.h>
#include <bom/bom.h>
#include <bom/bom_attr.h>
#include <epm/cr_effectivity.h>
#include <epm/epm.h>
#include <epm/releasestatus.h>
#include <fclasses/tc_string.h>
#include <ict/ict_userservice.h>
#include <itk/mem.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <pie/pie.h>
#include <pom/pom/pom.h>
#include <property/prop.h>
#include <ps/ps.h>
#include <ps/ps_errors.h>
#include <res/reservation.h>
#include <sa/imanfile.h>
#include <sa/sa.h>
#include <sa/tcfile.h>
#include <sa/user.h>
#include <ss/ss_errors.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tc/iman.h>
#include <tc/preferences.h>
#include <tc/tc.h>
#include <tc/tc_macros.h>
#include <tc/tc_startup.h> 
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/grm.h>
#include <tccore/grm_msg.h>
#include <tccore/iman_msg.h>
#include <tccore/imantype.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tccore/tctype.h>
#include <tccore/workspaceobject.h>
#include <tcinit/tcinit.h>
#include <textsrv/textserver.h>
#include <time.h>
#include <unidefs.h>
#include <user_exits/epm_toolkit_utils.h>
#define PROP_UIF_ask_value_msg   "PROP_UIF_ask_value"


void write2xml(FILE * , char*);
int ctr=0;

 int a=0;

FILE *Report; 

void write2xml(FILE *Report , char* str)
{
	fprintf(Report,str);
	ctr++;
}

char *replaceWord(const char *s, const char *oldW, 
                                 const char *newW) 
{ 
    char *result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
  
    // Counting the number of times old word 
    // occur in the string 
    for (i = 0; s[i] != '\0'; i++) 
    { 
        if (strstr(&s[i], oldW) == &s[i]) 
        { 
            cnt++; 
  
            // Jumping to index after the old word. 
            i += oldWlen - 1; 
        } 
    } 
  
    // Making new string of enough length 
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) 
    { 
        // compare the substring with the result 
        if (strstr(s, oldW) == s) 
        { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
    return result; 
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
;

void print_requirement()
{
     printf(
        "\n all the following parameters are mandatory for procedure"
        " USAGE:\n"
        " -Part_Number=<Item ID> (required)\n"
        " -Only_SpareKit_Assembly=<Yes or No>(case sensetive) (required)\n"
        );    
}

/*******************************************************************************
    Function:       ITK_user_main
    Description:    This is ITK main function. The program starts from here. 
*******************************************************************************/
int ITK_user_main(int argc, char *argv[])
{ 
		char    * Part_Number		= NULL;
		char	* RevisionRule		= NULL;
		char	* SVR				= NULL;
		tag_t	plat=NULLTAG;
		tag_t	rev=NULLTAG;
		tag_t	bom_window=NULLTAG;
		tag_t	top_line=NULLTAG;
		int		n_lines=0;
		tag_t	*lines=NULL;
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);

		
		tag_t VariqueryTag			 = NULLTAG;
		char **valuesNonColSvr		 = (char **) MEM_alloc(1 * sizeof(char *));
		int n_entriesV				 = 1;
		int resultCountVNCol		 = 0;
		char *qry_entries3[1]		 = {"Name"};
		tag_t * outputVNCol_tags		 =NULLTAG;

		tag_t   rule				 =NULLTAG;
		tag_t   VarientRuletag		 =NULLTAG;

		int rulefound= 0;
		tag_t *closurerule = NULL;
		tag_t close_tag;
		char **rulename = NULL;
		char **rulevalue = NULL;
		tag_t *bom_variant_config=NULL;


		Part_Number		= ITK_ask_cli_argument("-VC_No=");
		SVR                = ITK_ask_cli_argument("-SVR=");
		RevisionRule	= ITK_ask_cli_argument("-RevisionRule=");
		


						
	ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ) ;
	ITK_init_module("ercpsup","ERCpsup2019","Engineering");
//	ITK_init_module("ercpsup","XYT1ESA","dba");
//	ITK_init_module("infodba","infodba","dba");
	ITK_set_bypass(true);

	if (tc_strlen(Part_Number) == 0)
	{
		printf("\nPartNumber is NULL............. \n\t"); fflush(stdout);

					ITEM_find_item("X4", &plat);

					ITEM_ask_latest_rev(plat, &rev);


					QRY_find("VariantRule", &VariqueryTag);
					printf("\n\t After IFERR_REPORT : QRY_find .... "); fflush(stdout);

					if(VariqueryTag)
					{
						printf("\nFound Query 'VariantRule' \n\t"); fflush(stdout);
					}
					else
					{
						printf("\nNot Found Query 'VariantRule' \n\t"); fflush(stdout);
					}

					valuesNonColSvr[0] = SVR ;

					QRY_execute(VariqueryTag, n_entriesV, qry_entries3, valuesNonColSvr, &resultCountVNCol, &outputVNCol_tags);

					printf("   n_entriesV:%d   resultCount:%d \n\t",n_entriesV,resultCountVNCol); fflush(stdout);

					if (resultCountVNCol > 0)
					{
						VarientRuletag = outputVNCol_tags[0];
					}
					else
					{
						printf ("\n NonColour-SVR-VarientRule not present  [%s]\n", SVR); fflush(stdout);
						exit (0);
					}
													
						//	Report=fopen("/home/uadev/devgroups/ashish_w/XML/Report.xml","w");
							Report=fopen("/tmp/Comp_Code_Hierarchy_Report.xml","w");
															if (Report == NULL)
														{
																		printf("ERROR: XML Cannot open File\n");
																		return -1;
														}
														else
														{
																		printf("XML File opened successfully.\n");
														}
														write2xml(Report,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
														write2xml(Report,"\n<Top>\n");

														write2xml(Report,"<Item_ID>");
														write2xml(Report,"X4");
														write2xml(Report,"</Item_ID>");

														printf("Date: %d-%d-%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday);
														write2xml(Report,"\n<Date>");
														fprintf(Report,"%d-%d-%d",tm.tm_mday, tm.tm_mon + 1,  tm.tm_year + 1900);
														write2xml(Report,"</Date>\n");

														write2xml(Report,"<SVR>");
														write2xml(Report,SVR);
														write2xml(Report,"</SVR>");


						printf(" before BOM_create_window..\n"); fflush(stdout);   
									BOM_create_window( &bom_window );
									CFM_find(RevisionRule, &rule );
									BOM_set_window_config_rule( bom_window, rule );
									PIE_find_closure_rules( "BOMViewClosureRuleERC",PIE_TEAMCENTER, &rulefound, &closurerule );
									if (rulefound > 0)
									{
										close_tag = closurerule[0];
										printf ("closure rule found \n");fflush(stdout);
									}
									BOM_window_set_closure_rule( bom_window,close_tag, 0, rulename,rulevalue );
									BOM_set_window_pack_all(bom_window, true);							
									BOM_set_window_top_line( bom_window, NULLTAG, rev, NULLTAG, &top_line );
									BOM_line_ask_child_lines(top_line, &n_lines, &lines);
									BOM_window_hide_unconfigured(bom_window)   ;
									BOM_window_show_variants(bom_window)   ;										 
									BOM_window_apply_variant_configuration(bom_window,1,&VarientRuletag)   ;									
									BOM_window_hide_variants(bom_window) ;
									BOM_line_ask_child_lines(top_line, &n_lines, &lines);
									printf("\n After BOM_window_apply_variant_configuration-----\n"); fflush(stdout);
	

				getchilds(n_lines,lines);		
															write2xml(Report,"</Top>\n");	
															fclose(Report);	
		}
		else
	{

					ITEM_find_item(Part_Number, &plat);

					ITEM_ask_latest_rev(plat, &rev);

					Report=fopen("/tmp/Comp_Code_Hierarchy_Report.xml","w");
															if (Report == NULL)
														{
																		printf("ERROR: XML Cannot open File\n");
																		return -1;
														}
														else
														{
																		printf("XML File opened successfully.\n");
														}
														write2xml(Report,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
														write2xml(Report,"\n<Top>\n");

														write2xml(Report,"<Item_ID>");
														write2xml(Report,Part_Number);
														write2xml(Report,"</Item_ID>");

														printf("Date: %d-%d-%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday);
														write2xml(Report,"\n<Date>");
														fprintf(Report,"%d-%d-%d",tm.tm_mday, tm.tm_mon + 1,  tm.tm_year + 1900);
														write2xml(Report,"</Date>\n");

														write2xml(Report,"<SVR>");
														write2xml(Report," ");
														write2xml(Report,"</SVR>");


						printf(" before BOM_create_window..\n"); fflush(stdout);   
									BOM_create_window( &bom_window );
									CFM_find(RevisionRule, &rule );
									BOM_set_window_config_rule( bom_window, rule );
									PIE_find_closure_rules( "BOMViewClosureRuleERC",PIE_TEAMCENTER, &rulefound, &closurerule );
									if (rulefound > 0)
									{
										close_tag = closurerule[0];
										printf ("closure rule found \n");fflush(stdout);
									}
									BOM_window_set_closure_rule( bom_window,close_tag, 0, rulename,rulevalue );
									BOM_set_window_pack_all(bom_window, true);							
									BOM_set_window_top_line( bom_window, NULLTAG, rev, NULLTAG, &top_line );								
									BOM_line_ask_child_lines(top_line, &n_lines, &lines);
									printf("\n After BOM_window_apply_variant_configuration-----\n"); fflush(stdout);
	

				getchilds(n_lines,lines);		
															write2xml(Report,"</Top>\n");	
															fclose(Report);	
	}					
		return 0;
}

int getchilds(int n_lines,tag_t *lines)
{
		char *name=NULL;
		char *bl_item_object_type=NULL;
		char *ColIND=NULL;
		char *Compcode=NULL;
		char *level=NULL;
		char *t5_Coated=NULL;
		int count1=0;
		tag_t *sub_childs=NULL;
		int i=0;
		char *value=NULL;
		char *project=NULL;
		char *dsgngrp=NULL;
		char *prttype=NULL;
		char *desc=NULL;
		char *item_id=NULL;
		char *myid=NULL;
		char *rev_Id=NULL;
		char *Part_type=NULL;
		char *owner=NULL;
		char *lifecycle=NULL;
		char *descp=NULL;
		tag_t child_item=NULLTAG;
		tag_t child_rev=NULLTAG;
		int count=0;
		int *obj_lvl=0;
		GRM_relation_t *sec_obj=NULLTAG;
		char **relation=NULL;
		int j=0;
		char *sec_obj_type=NULL;
		char *DML_Id=NULL;
		char *myDMLid=NULL;
		tag_t relType;
		tag_t primary_object;
		char c[] = "&"; 
	    char d[] = "&#38;";
		char *result = NULL;

	for(i=0;i<n_lines;i++)
	{
	
	AOM_UIF_ask_value(lines[i],"bl_item_object_type",&bl_item_object_type);
	printf("\n bl_item_object_type -----------> %s",bl_item_object_type);
	AOM_UIF_ask_value(lines[i],"bl_T5_ClrPartRevision_t5_ColourInd",&ColIND);
	AOM_UIF_ask_value(lines[i],"bl_line_name",&name);
	printf("\n Childs Name %s",name);

	AOM_UIF_ask_value(lines[i],"bl_item_item_id",&item_id);
	printf("\n Childs ID %s",item_id);

	printf("\n Color Indicator --------------> %s",ColIND);
	if( tc_strcmp ( bl_item_object_type , "Architecture Module" ) == 0 )
	{
		BOM_line_ask_all_child_lines(lines[i],&count1,&sub_childs);
		if(count1>0)
		{
		getchilds(count1,sub_childs);
		}
	}
	else
	{
	

	if( tc_strcmp ( ColIND , "Y" ) == 0 )
		{
		printf(" Color Indicator is Y _________________________________>>>\n"); fflush(stdout); 
	
		ITEM_find_item(item_id, &child_item);
			ITEM_ask_latest_rev(child_item, &child_rev);
	
			AOM_UIF_ask_value(child_rev,"t5_PrtCatCode",&Compcode);

	
				write2xml(Report,"<logo>\n");

						printf("\nWriting to XML");

						//level
						AOM_UIF_ask_value(lines[i],"bl_level_starting_0",&level);
						fprintf(Report,"<field key =\"Level\">");
						fprintf(Report,"%s",level);
						fprintf(Report,"</field>\n");


						//Comp Code
						write2xml(Report,"<field key =\"Comp Code\">");
						AOM_UIF_ask_value(child_rev,"t5_PrtCatCode",&Compcode);
						write2xml(Report,Compcode);
						write2xml(Report,"</field>\n");


						//Desc
						AOM_UIF_ask_value(child_rev,"object_desc",&descp);
						result = replaceWord(descp, c, d);
						write2xml(Report,"<field key =\"Description\">");
						write2xml(Report,result);
						write2xml(Report,"</field>\n");


						//Part_NO
						write2xml(Report,"<field key =\"Part Number\">");
						write2xml(Report,item_id);
						write2xml(Report,"</field>\n");


						//Revision ID
						AOM_UIF_ask_value(child_rev,"current_revision_id",&rev_Id);
						write2xml(Report,"<field key =\"Revision Seq\">");
						write2xml(Report,rev_Id);
						write2xml(Report,"</field>\n");	

						//Colour Indicator
						write2xml(Report,"<field key =\"Colour Indicator\">");
						write2xml(Report,ColIND);
						write2xml(Report,"</field>\n");

						//Coated Indicator
						AOM_UIF_ask_value(child_rev,"t5_Coated",&t5_Coated);
						write2xml(Report,"<field key =\"Coated Indicator\">");
						write2xml(Report,t5_Coated);
						write2xml(Report,"</field>\n");	

						//Owner
						AOM_UIF_ask_value(child_rev,"owning_user",&owner);
						write2xml(Report,"<field key =\"Owner\">");
						write2xml(Report,owner);
						write2xml(Report,"</field>\n");	

				write2xml(Report,"</logo>\n");
		

	BOM_line_ask_all_child_lines(lines[i],&count1,&sub_childs);

	if(count1>0)
		{
		getchilds(count1,sub_childs);
		}
		}
	}
	}
					if(lines)
						MEM_free(lines);
													
						return 0;
}




