/**********************************************************************************************************************************************************
**
** SOURCE FILE NAME		:	DMLs after TMKO- Tracker report.c
**
** Author				:	AWW917697 
**
** Date					:	14/01/2020                                                                                                                                                                                                             
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
FILE *Childs; 

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

/*******************************************************************************
    Function:       ITK_user_main
    Description:    This is ITK main function. The program starts from here. 
*******************************************************************************/
int ITK_user_main(int argc, char *argv[])
{ 
		char    * Part_Number		= NULL;
		char	* SVR	= NULL;
		char	* RevisionRule		= NULL;
//		char    * Part_Number		= "002351";
//		char	* RevisionRule		= "Latest Working";
//		char	* RevisionRule		= "Latest Released";
//		char	* SVR				= "12345678000R_NR";
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
		logical modB=FALSE;
		logical modBCol=FALSE;

		Part_Number		= ITK_ask_cli_argument("-Platform=");
		SVR                = ITK_ask_cli_argument("-SVR=");
		RevisionRule	= ITK_ask_cli_argument("-RevisionRule=");		


						
	ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ) ;
//	ITK_init_module("ercpsup","ERCpsup2019","Engineering");	//production
	ITK_init_module("ercpsup","XYT1ESA","dba");	//cmitest
	printf("   Login Sucessfull******\n\t"); fflush(stdout);
//	ITK_init_module("infodba","infodba","dba");		//uadev
	ITK_set_bypass(true);

					ITEM_find_item(Part_Number, &plat);

					ITEM_ask_latest_rev(plat, &rev);


					QRY_find("VariantRule", &VariqueryTag);
					
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
							Report=fopen("/tmp/GatewayReport.xml","w");							
															if (Report == NULL)
														{
																		printf("ERROR: XML Cannot open File\n");
																		return -1;
														}
														else
														{
																		printf("XML File opened successfully.\n");
														}
														write2xml(Report,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
														write2xml(Report,"\n<Top>\n");

														write2xml(Report,"<Item_ID>");
														write2xml(Report,Part_Number);
														write2xml(Report,"</Item_ID>");

														printf("Date: %d-%d-%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday);
														write2xml(Report,"\n<Date>");
														fprintf(Report,"%d-%d-%d",tm.tm_mday, tm.tm_mon + 1,  tm.tm_year + 1900);
														write2xml(Report,"</Date>\n");

														write2xml(Report,"<SVR>");
														write2xml(Report,SVR);
														write2xml(Report,"</SVR>");

														write2xml(Report,"<RevisionRule>");
														write2xml(Report,RevisionRule);
														write2xml(Report,"</RevisionRule>");

									
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
								//	BOM_set_window_pack_all(bom_window, FALSE);							
									BOM_set_window_top_line( bom_window, NULLTAG, rev, NULLTAG, &top_line );
									BOM_line_ask_child_lines(top_line, &n_lines, &lines);
									BOM_window_hide_unconfigured(bom_window)   ;
									BOM_window_show_variants(bom_window)   ;										 
									BOM_window_apply_variant_configuration(bom_window,1,&VarientRuletag)   ;									
									BOM_window_hide_variants(bom_window) ;
									BOM_line_ask_child_lines(top_line, &n_lines, &lines);
									printf("\n After BOM_window_apply_variant_configuration-----\n"); fflush(stdout);

				getchilds(n_lines,lines,VarientRuletag,rule);		
				write2xml(Report,"</Top>\n");	
				fclose(Report);	
		return 0;
}

int getchilds(int n_lines,tag_t *lines,tag_t VarientRuletag,tag_t rule)
{
		char *tmkoapplicability=NULL;
		char *name=NULL;
		char *bomrevid=NULL;
		tag_t	bom_window=NULLTAG;
		tag_t	bom_window1=NULLTAG;
		char *bomrevid1=NULL;
		int count1=0;
		int count2=0;
		int DMLcount=0;
		int tmko_DMLcount=0;
		int taskcount=0;
		int tmko_taskcount=0;
		tag_t *sub_childs=NULL;
		int i=0;
		char *value=NULL;
		char *project=NULL;
		tag_t queryTag;
		char *dsgngrp=NULL;
		char *prttype=NULL;
		char *desc=NULL;
		char *DRStatus=NULL;
		char *keyworddec=NULL;
		char *DMLrlstype=NULL;
		char *level=NULL;
		char *item_id=NULL;
		tag_t	top_line=NULLTAG;
		char *myid=NULL;
		char *rev_tmkoapp=NULL;
		char *tmkoDate=NULL;
		char *rev_Id=NULL;
		char *Part_type=NULL;
		int n_entries = 3;
		char *owner=NULL;
		char *lifecycle=NULL;
		char *descp=NULL;
		char *moddescp=NULL;
		char *desgrp=NULL;
		tag_t child_item=NULLTAG;
		tag_t child_item1=NULLTAG;
		tag_t child_rev=NULLTAG;
		tag_t child_rev1=NULLTAG;
		int count=0;
		int tmkocount=0;
		int tmkocount2=0;
		int *obj_lvl=0;
		GRM_relation_t *sec_obj=NULLTAG;
		GRM_relation_t *tmko_sec_obj=NULLTAG;
		GRM_relation_t *tmko_sec_obj2=NULLTAG;
		GRM_relation_t *tmko_sec_obj3=NULLTAG;
		GRM_relation_t *sec_obj2=NULLTAG;
		GRM_relation_t *sec_obj3=NULLTAG;
		GRM_relation_t *DMLS=NULLTAG;
		GRM_relation_t *tmko_DMLS=NULLTAG;
		char **relation=NULL;
		int j=0;
		int k=0;
		int q=0;
		int o=0;
		int y=0;
		char *sec_obj_type=NULL;
		char *MAIN_DML_ID=NULL;
		char *GATEMdrstatus=NULL;
		char *GATEMcloserdate=NULL;
		char *DMLRlstype=NULL;
		char *DML_Id=NULL;
		char *myDMLid=NULL;
		char *DML_objtype=NULL;
		char *ERCDML_ID=NULL;
		char *ERCDML_RlsDate=NULL;
		char *ERCDML_DRStatus=NULL;
		char *Synopsis=NULL;
		char *ERCDML_prjcode=NULL;
		char *ERCDML_rlstype=NULL;
		char *ERCDML_reason=NULL;
		char *ERCDML_lifecyclestate=NULL;
		char *ERCDML_creator=NULL;
		tag_t relType;
		tag_t SolItemrelType;
		tag_t relTypetsk;
		tag_t primary_object;
		tag_t tmko_primary_object;
		tag_t task;
		tag_t tmko_task;
		tag_t DMLTAG;
		tag_t tmko_DMLTAG;
		tag_t DMLRevTag;
		tag_t tmko_DMLRevTag;
		char c[] = "&"; 
	    char d[] = "&#38;";
		char e[] = "'"; 
	    char f[] = "&#39;";
		char g[] = "\""; 
	    char h[] = "&#34;";
		char *result = NULL;
		char *result1 = NULL;
		char *result2 = NULL;
		char *result3 = NULL;
		char *result4 = NULL;
		char *result5 = NULL;
		char *result7 = NULL;
		char *result8 = NULL;
		char *result9 = NULL;

		char *result11 = NULL;
		char *result12 = NULL;
		char *result13 = NULL;

		char *result14 = NULL;
		char *result15 = NULL;
		char *result16 = NULL;

		char *tmkoresult11 = NULL;
		char *tmkoresult12 = NULL;
		char *tmkoresult13 = NULL;

		char *tmkoresult14 = NULL;
		char *tmkoresult15 = NULL;
		char *tmkoresult16 = NULL;

		char *qry_entries4[3] = {"ID","Name","Date Released"};
		char **qry_values = (char **) MEM_alloc(50 * sizeof(char *));
		int resultCount=0;
		tag_t *qry_output;

		//tmko

		char *tmkoitemID=NULL;
		char *tmko_tmkoapp=NULL;
		char *tmko_tmkoDate=NULL;
		char *tmko_Part_type=NULL;
		char *tmko_descp=NULL;
		char *tmkoresult=NULL;
		char *tmkoresult1=NULL;
		char *tmkoresult2=NULL;
		char *tmko_rev_Id=NULL;
		char *tmko_moddescp=NULL;
		char *tmkoresult3=NULL;
		char *tmkoresult4=NULL;
		char *tmkoresult5=NULL;
		char *tmko_desgrp=NULL;
		char *tmko_owner=NULL;
		char *tmko_DRStatus=NULL;
		char *tmko_keyworddec=NULL;
		char *tmkoresult7=NULL;
		char *tmkoresult8=NULL;
		char *tmkoresult9=NULL;
		char *tmko_DMLRlstype=NULL;
		char *tmko_MAIN_DML_ID=NULL;
		char *tmko_GATEMdrstatus=NULL;
		char *tmko_GATEMcloserdate=NULL;
		char *tmko_DML_objtype=NULL;							
		char *tmko_DML_ID=NULL;
		char *tmko_DML_RlsDate=NULL;
		char *tmko_DML_DRStatus=NULL;
		char *tmko_Synopsis=NULL;
		char *tmko_DML_prjcode=NULL;
		char *tmko_DML_rlstype=NULL;
		char *tmko_DML_reason=NULL;
		char *tmko_DML_lifecyclestate=NULL;
		char *tmko_DML_creator=NULL;
		int tmkoj=0;
		int tmkoq=0;
		int tmko_o=0;

	
	for(i=0;i<n_lines;i++)
	{
		AOM_UIF_ask_value(lines[i],"bl_item_item_id",&item_id);
		printf("\n Childs ID %s",item_id);
		AOM_UIF_ask_value(lines[i],"bl_rev_item_revision_id",&bomrevid);
		ITEM_find_item(item_id, &child_item1);
		ITEM_find_rev(item_id,bomrevid,&child_rev1);
		AOM_UIF_ask_value(lines[i],"bl_level_starting_0",&level);

		AOM_ask_value_string(lines[i],"bl_Design_t5_tmkoapplicability",&tmkoapplicability);
	printf("\n tmkoapplicability ------------------------> %s",tmkoapplicability);

	if( tc_strcmp ( tmkoapplicability , "YES" ) == 0 )
	{
	AOM_UIF_ask_value(lines[i],"bl_line_name",&name);
	printf("\n Childs Name %s",name);		
	
		ITEM_find_item(item_id, &child_item);
			ITEM_find_rev(item_id,bomrevid,&child_rev );
			AOM_UIF_ask_value(child_rev,"t5_ProjectCode",&project);
			AOM_UIF_ask_value(child_rev,"t5_DesignGrp",&dsgngrp);
			AOM_UIF_ask_value(child_rev,"t5_PartType",&prttype);

				write2xml(Report,"<logo>\n");

						printf("\nWriting to XML\n");
	
						fprintf(Report,"<field key =\"Level\">");
						fprintf(Report,"%s",level);
						fprintf(Report,"</field>\n");

						write2xml(Report,"<field key =\"Part Number\">");
						write2xml(Report,item_id);
						write2xml(Report,"</field>\n");

						AOM_UIF_ask_value(child_rev,"t5_tmkoappcomp",&rev_tmkoapp);
						write2xml(Report,"<field key =\"TMKO Applicability\">");
						write2xml(Report,tmkoapplicability);
						write2xml(Report,"</field>\n");

						AOM_UIF_ask_value(child_item,"t5_dateoftmko",&tmkoDate);
						write2xml(Report,"<field key =\"TMKO Date\">");
						write2xml(Report,tmkoDate);
						write2xml(Report,"</field>\n");

						AOM_UIF_ask_value(child_rev,"t5_PartType",&Part_type);
						write2xml(Report,"<field key =\"Part Type\">");
						write2xml(Report,Part_type);
						write2xml(Report,"</field>\n");
						
						AOM_UIF_ask_value(child_rev,"object_desc",&descp);
						result = replaceWord(descp, c, d);
						result1 = replaceWord(result, e, f);
						result2 = replaceWord(result1, g, h);
						write2xml(Report,"<field key =\"Description\">");
						write2xml(Report,result2);
						write2xml(Report,"</field>\n");

						AOM_UIF_ask_value(child_rev,"current_revision_id",&rev_Id);
						write2xml(Report,"<field key =\"Revision Seq\">");
						write2xml(Report,rev_Id);
						write2xml(Report,"</field>\n");	
						
						AOM_UIF_ask_value(child_rev,"t5_DocRemarks",&moddescp);
						result3 = replaceWord(moddescp, c, d);
						result4 = replaceWord(result3, e, f);
						result5 = replaceWord(result4, g, h);
						write2xml(Report,"<field key =\"Modification Description\">");
						write2xml(Report,result5);
						write2xml(Report,"</field>\n");

						AOM_UIF_ask_value(child_rev,"t5_DesignGrp",&desgrp);
						write2xml(Report,"<field key =\"Design Group\">");
						write2xml(Report,desgrp);
						write2xml(Report,"</field>\n");	

						AOM_UIF_ask_value(child_rev,"owning_user",&owner);
						write2xml(Report,"<field key =\"Owner\">");
						write2xml(Report,owner);
						write2xml(Report,"</field>\n");	

						AOM_UIF_ask_value(child_rev,"t5_PartStatus",&DRStatus);
						write2xml(Report,"<field key =\"Part DR Status\">");
						write2xml(Report,DRStatus);
						write2xml(Report,"</field>\n");	
						
						AOM_UIF_ask_value(child_rev,"t5_CategoryName",&keyworddec);
						result7 = replaceWord(keyworddec, c, d);
						result8 = replaceWord(result7, e, f);
						result9 = replaceWord(result8, g, h);
						write2xml(Report,"<field key =\"Keyword Desc\">");
						write2xml(Report,result9);
						write2xml(Report,"</field>\n");									
						
						// Gate Maturation DML
						(GRM_find_relation_type("CMReferences",&relType));
						(GRM_find_relation_type("T5_DMLTaskRelation",&relTypetsk));
						if(relType != NULLTAG)
						{
							GRM_list_primary_objects(child_rev , relType,&count, &sec_obj);
							for (j=0;j<count ;j++ )
						{ 
							primary_object = sec_obj[j].primary;
						if(relTypetsk != NULLTAG)
						{
							GRM_list_primary_objects(primary_object , relTypetsk,&count2, &sec_obj2);					
							DMLTAG = sec_obj2[0].primary;
							AOM_ask_value_string(DMLTAG,"t5_rlstype",&DMLRlstype);
						if (tc_strcmp(DMLRlstype,"TODR") ==0)
						{			
							AOM_UIF_ask_value(DMLTAG,"item_id",&MAIN_DML_ID); 	
							AOM_UIF_ask_value(DMLTAG,"t5_cDRstatus",&GATEMdrstatus); 	
							AOM_UIF_ask_value(DMLTAG,"CMClosureDate",&GATEMcloserdate); 
						}
						}
						}							
						} 	 
						printf("MAIN_DML_ID ------------------> %s\n",MAIN_DML_ID);
						write2xml(Report,"<field key =\"Gate Maturation DML\">");
						write2xml(Report,MAIN_DML_ID);
						write2xml(Report,"</field>\n");
						printf("GATEMdrstatus ------------------> %s\n",GATEMdrstatus);
						write2xml(Report,"<field key =\"Gate Maturation DML DR Status\">");
						write2xml(Report,GATEMdrstatus);
						write2xml(Report,"</field>\n");
						printf("GATEMcloserdate ------------------> %s\n",GATEMcloserdate);
						write2xml(Report,"<field key =\"Gate Maturation Closure Date\">");
						write2xml(Report,GATEMcloserdate);
						write2xml(Report,"</field>\n");

						// ERC DML
						GRM_find_relation_type("CMHasSolutionItem",&SolItemrelType);
						GRM_list_primary_objects(child_rev , SolItemrelType,&taskcount, &sec_obj3);
						for (q=0;q<taskcount ;q++)
						{ 
							task = sec_obj3[q].primary;
							GRM_list_primary_objects(task , relTypetsk,&DMLcount, &DMLS);
							for (o=0;o<DMLcount ;o++ )
						{ 
							DMLRevTag = DMLS[o].primary;
							AOM_UIF_ask_value(DMLRevTag,"object_type",&DML_objtype);
							if (tc_strcmp(DML_objtype,"ERC DML Revision") ==0)
						{
							AOM_UIF_ask_value(DMLRevTag,"item_id",&ERCDML_ID);
							AOM_UIF_ask_value(DMLRevTag,"date_released",&ERCDML_RlsDate);
							AOM_UIF_ask_value(DMLRevTag,"t5_cDRstatus",&ERCDML_DRStatus);
							AOM_UIF_ask_value(DMLRevTag,"object_name",&Synopsis);
							result11 = replaceWord(Synopsis, c, d);
							result12 = replaceWord(result11, e, f);
							result13 = replaceWord(result12, g, h);
							AOM_UIF_ask_value(DMLRevTag,"t5_cprojectcode",&ERCDML_prjcode);
							AOM_UIF_ask_value(DMLRevTag,"t5_rlstype",&ERCDML_rlstype);
							AOM_UIF_ask_value(DMLRevTag,"t5_reason",&ERCDML_reason);
							result14 = replaceWord(ERCDML_reason, c, d);
							result15 = replaceWord(result14, e, f);
							result16 = replaceWord(result15, g, h);
							AOM_UIF_ask_value(DMLRevTag,"release_status_list",&ERCDML_lifecyclestate);
							AOM_UIF_ask_value(DMLRevTag,"owning_user",&ERCDML_creator);
						}
						}
						}
						printf("ERCDML_ID ------------------> %s\n",ERCDML_ID);
						if(tc_strlen(ERCDML_ID) == 0)
							{
								printf("ERCDML_ID is 000000\n");
							}
						else
							{
									write2xml(Report,"<field key =\"DML No\">");
									write2xml(Report,ERCDML_ID);
									write2xml(Report,"</field>\n");

									write2xml(Report,"<field key =\"DML Release Date\">");
									write2xml(Report,ERCDML_RlsDate);
									write2xml(Report,"</field>\n");

									write2xml(Report,"<field key =\"DML DR Status\">");
									write2xml(Report,ERCDML_DRStatus);
									write2xml(Report,"</field>\n");						

									write2xml(Report,"<field key =\"Synopsis\">");
									write2xml(Report,result13);
									write2xml(Report,"</field>\n");

									write2xml(Report,"<field key =\"DML Project Code\">");
									write2xml(Report,ERCDML_prjcode);
									write2xml(Report,"</field>\n");

									write2xml(Report,"<field key =\"Release Type\">");
									write2xml(Report,ERCDML_rlstype);
									write2xml(Report,"</field>\n");

									write2xml(Report,"<field key =\"Reason\">");
									write2xml(Report,result16);
									write2xml(Report,"</field>\n");

									write2xml(Report,"<field key =\"Lifecycle State\">");
									write2xml(Report,ERCDML_lifecyclestate);
									write2xml(Report,"</field>\n");

									write2xml(Report,"<field key =\"DML Creator\">");
									write2xml(Report,ERCDML_creator);
									write2xml(Report,"</field>\n");
							}

						write2xml(Report,"</logo>\n");

						//tmko code....

						QRY_find("Qry_DesRevERCRelEffeDate", &queryTag);

						qry_values[0] = item_id;
						qry_values[1] = "T5_LcsErcRlzd" ;
						qry_values[2] = tmkoDate ;

						QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &qry_output);
						printf(" Count of Query Values of Date of TMKO ------------------>  %d \n",resultCount);
						for(y=0;y<resultCount;y++)
						{	
							AOM_UIF_ask_value(qry_output[y],"current_revision_id",&tmko_rev_Id); 
							if (tc_strcmp(tmko_rev_Id,rev_Id) !=0)
						{

							write2xml(Report,"<logo>\n");
							fprintf(Report,"<field key =\"Level\">");
							fprintf(Report,"</field>\n");

							AOM_UIF_ask_value(qry_output[y],"item_id",&tmkoitemID);
							write2xml(Report,"<field key =\"Part Number\">");
							write2xml(Report,tmkoitemID);
							write2xml(Report,"</field>\n");

							AOM_UIF_ask_value(qry_output[y],"t5_tmkoappcomp",&tmko_tmkoapp);
							write2xml(Report,"<field key =\"TMKO Applicability\">");
							write2xml(Report,tmkoapplicability);
							write2xml(Report,"</field>\n");

							AOM_UIF_ask_value(child_item,"t5_dateoftmko",&tmko_tmkoDate);
							write2xml(Report,"<field key =\"TMKO Date\">");
							write2xml(Report,tmko_tmkoDate);
							write2xml(Report,"</field>\n");

							AOM_UIF_ask_value(qry_output[y],"t5_PartType",&tmko_Part_type);
							write2xml(Report,"<field key =\"Part Type\">");
							write2xml(Report,tmko_Part_type);
							write2xml(Report,"</field>\n");

							AOM_UIF_ask_value(qry_output[y],"object_desc",&tmko_descp);
							tmkoresult = replaceWord(tmko_descp, c, d);
							tmkoresult1 = replaceWord(result, e, f);
							tmkoresult2 = replaceWord(result1, g, h);
							write2xml(Report,"<field key =\"Description\">");
							write2xml(Report,tmkoresult2);
							write2xml(Report,"</field>\n");

							
							write2xml(Report,"<field key =\"Revision Seq\">");
							write2xml(Report,tmko_rev_Id);
							write2xml(Report,"</field>\n");	

							AOM_UIF_ask_value(qry_output[y],"t5_DocRemarks",&tmko_moddescp);
							tmkoresult3 = replaceWord(tmko_moddescp, c, d);
							tmkoresult4 = replaceWord(tmkoresult3, e, f);
							tmkoresult5 = replaceWord(tmkoresult4, g, h);
							write2xml(Report,"<field key =\"Modification Description\">");
							write2xml(Report,tmkoresult5);
							write2xml(Report,"</field>\n");

							AOM_UIF_ask_value(qry_output[y],"t5_DesignGrp",&tmko_desgrp);
							write2xml(Report,"<field key =\"Design Group\">");
							write2xml(Report,tmko_desgrp);
							write2xml(Report,"</field>\n");

							AOM_UIF_ask_value(qry_output[y],"owning_user",&tmko_owner);
							write2xml(Report,"<field key =\"Owner\">");
							write2xml(Report,tmko_owner);
							write2xml(Report,"</field>\n");

							AOM_UIF_ask_value(qry_output[y],"t5_PartStatus",&tmko_DRStatus);
							write2xml(Report,"<field key =\"Part DR Status\">");
							write2xml(Report,tmko_DRStatus);
							write2xml(Report,"</field>\n");	

							AOM_UIF_ask_value(qry_output[y],"t5_CategoryName",&tmko_keyworddec);
							tmkoresult7 = replaceWord(keyworddec, c, d);
							tmkoresult8 = replaceWord(result7, e, f);
							tmkoresult9 = replaceWord(result8, g, h);
							write2xml(Report,"<field key =\"Keyword Desc\">");
							write2xml(Report,tmkoresult9);
							write2xml(Report,"</field>\n");

							// Gate Maturation DML
							GRM_list_primary_objects(qry_output[y] , relType,&tmkocount, &tmko_sec_obj);
								for (tmkoj=0;tmkoj<tmkocount ;tmkoj++ )
							{ 
								tmko_primary_object = tmko_sec_obj[tmkoj].primary;							
								GRM_list_primary_objects(tmko_primary_object , relTypetsk,&tmkocount2, &tmko_sec_obj2);					
								tmko_DMLTAG = tmko_sec_obj2[0].primary;
								AOM_ask_value_string(tmko_DMLTAG,"t5_rlstype",&tmko_DMLRlstype);
							if (tc_strcmp(tmko_DMLRlstype,"TODR") ==0)
							{			
								AOM_UIF_ask_value(tmko_DMLTAG,"item_id",&tmko_MAIN_DML_ID); 	
								AOM_UIF_ask_value(tmko_DMLTAG,"t5_cDRstatus",&tmko_GATEMdrstatus); 	
								AOM_UIF_ask_value(tmko_DMLTAG,"CMClosureDate",&tmko_GATEMcloserdate); 
							}							
							}							
							printf("TMKO_MAIN_DML_ID ------------------> %s\n",tmko_MAIN_DML_ID);
							write2xml(Report,"<field key =\"Gate Maturation DML\">");
							write2xml(Report,tmko_MAIN_DML_ID);
							write2xml(Report,"</field>\n");
							printf("TMKO_GATEMdrstatus ------------------> %s\n",tmko_GATEMdrstatus);
							write2xml(Report,"<field key =\"Gate Maturation DML DR Status\">");
							write2xml(Report,tmko_GATEMdrstatus);
							write2xml(Report,"</field>\n");
							printf("TMKO_GATEMcloserdate ------------------> %s\n",tmko_GATEMcloserdate);
							write2xml(Report,"<field key =\"Gate Maturation Closure Date\">");
							write2xml(Report,tmko_GATEMcloserdate);
							write2xml(Report,"</field>\n");

							// ERC DML
							GRM_list_primary_objects(qry_output[y] , SolItemrelType,&tmko_taskcount, &tmko_sec_obj3);
							for (tmkoq=0;tmkoq<tmko_taskcount ;tmkoq++)
							{

								tmko_task = tmko_sec_obj3[tmkoq].primary;
								GRM_list_primary_objects(tmko_task , relTypetsk,&tmko_DMLcount, &tmko_DMLS);
								for (tmko_o=0;tmko_o<tmko_DMLcount ;tmko_o++ )
								{

									tmko_DMLRevTag = tmko_DMLS[tmko_o].primary;
									AOM_UIF_ask_value(tmko_DMLRevTag,"object_type",&tmko_DML_objtype);
									if (tc_strcmp(tmko_DML_objtype,"ERC DML Revision") ==0)
								{
										AOM_UIF_ask_value(tmko_DMLRevTag,"item_id",&tmko_DML_ID);
										AOM_UIF_ask_value(tmko_DMLRevTag,"date_released",&tmko_DML_RlsDate);
										AOM_UIF_ask_value(tmko_DMLRevTag,"t5_cDRstatus",&tmko_DML_DRStatus);
										AOM_UIF_ask_value(tmko_DMLRevTag,"object_name",&tmko_Synopsis);
										tmkoresult11 = replaceWord(tmko_Synopsis, c, d);
										tmkoresult12 = replaceWord(tmkoresult11, e, f);
										tmkoresult13 = replaceWord(tmkoresult12, g, h);
										AOM_UIF_ask_value(tmko_DMLRevTag,"t5_cprojectcode",&tmko_DML_prjcode);
										AOM_UIF_ask_value(tmko_DMLRevTag,"t5_rlstype",&tmko_DML_rlstype);
										AOM_UIF_ask_value(tmko_DMLRevTag,"t5_reason",&tmko_DML_reason);
										tmkoresult14 = replaceWord(tmko_DML_reason, c, d);
										tmkoresult15 = replaceWord(tmkoresult14, e, f);
										tmkoresult16 = replaceWord(tmkoresult15, g, h);
										AOM_UIF_ask_value(tmko_DMLRevTag,"release_status_list",&tmko_DML_lifecyclestate);
										AOM_UIF_ask_value(tmko_DMLRevTag,"owning_user",&tmko_DML_creator);
								}
								}

								printf("TMKO_ERCDML_ID ------------------> %s\n",tmko_DML_ID);								
								if(tc_strlen(tmko_DML_ID) == 0)
							{
								printf("TMKO_ERCDML_ID is 000000\n");
							}
							else
							{
								write2xml(Report,"<field key =\"DML No\">");
								write2xml(Report,tmko_DML_ID);
								write2xml(Report,"</field>\n");

								write2xml(Report,"<field key =\"DML Release Date\">");
								write2xml(Report,tmko_DML_RlsDate);
								write2xml(Report,"</field>\n");

								write2xml(Report,"<field key =\"DML DR Status\">");
								write2xml(Report,tmko_DML_DRStatus);
								write2xml(Report,"</field>\n");						

								write2xml(Report,"<field key =\"Synopsis\">");
								write2xml(Report,tmkoresult13);
								write2xml(Report,"</field>\n");

								write2xml(Report,"<field key =\"DML Project Code\">");
								write2xml(Report,tmko_DML_prjcode);
								write2xml(Report,"</field>\n");

								write2xml(Report,"<field key =\"Release Type\">");
								write2xml(Report,tmko_DML_rlstype);
								write2xml(Report,"</field>\n");

								write2xml(Report,"<field key =\"Reason\">");
								write2xml(Report,tmkoresult16);
								write2xml(Report,"</field>\n");

								write2xml(Report,"<field key =\"Lifecycle State\">");
								write2xml(Report,tmko_DML_lifecyclestate);
								write2xml(Report,"</field>\n");

								write2xml(Report,"<field key =\"DML Creator\">");
								write2xml(Report,tmko_DML_creator);
								write2xml(Report,"</field>\n");
							}
							}
							write2xml(Report,"</logo>\n");
						}
						}				
						
	}
					BOM_line_ask_child_lines(lines[i],&count1,&sub_childs);					
					if(count1>0)
					{
							getchilds(count1,sub_childs,VarientRuletag,rule);		
					}
	}
	if(lines)
		MEM_free(lines);													
    return 0;
}




