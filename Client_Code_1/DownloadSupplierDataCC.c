/*
scp uadev@172.22.97.94:/home/uadev/devgroups/apn/DownloadSupplierData/DownloadSupplierDataCC .
DownloadSupplierDataCC -u=loader -pf=/user/uaprod/shells/Admin/loaderpasswdFile.pwf -g=dba
*/
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
#include <ae/ae.h>
#include <setjmp.h>
#include <ae/ae_errors.h>
#include <ae/ae_types.h> 
#include <unidefs.h>
#include <user_exits/user_exit_msg.h>
#include <pom/enq/enq.h>
#include <ug_va_copy.h>
#include <itk/mem.h>
#include <tie/tie_errors.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
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
#include <tccore/project.h>
#include <sa/groupmember.h>
#include <pie/pie.h>

#define PS_where_used_all_levels   -1 
#define ITK_errStore (EMH_USER_error_base + 3)
/*char* subString (char* mainStringf ,int fromCharf,int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(200);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}*/
#define ITK_CALL(X) (report_error( __FILE__, __LINE__, #X, (X)))

char* usernameDir = NULL;

static int report_error( char *file, int line, char *function, int return_code)
{
	if (return_code != ITK_ok)
	{
		int				index = 0;
		int				n_ifails = 0;
		const int*		severities = 0;
		const int*		ifails = 0;
		const char**	texts = NULL;

		EMH_ask_errors( &n_ifails, &severities, &ifails, &texts);
		printf("%3d error(s)\n", n_ifails);fflush(stdout);
		for( index=0; index<n_ifails; index++)
		{
			printf("ERROR: %d\tERROR MSG: %s\n", ifails[index], texts[index]);fflush(stdout);
			TC_write_syslog("ERROR: %d\tERROR MSG: %s\n", ifails[index], texts[index]);
			printf ("FUNCTION: %s\nFILE: %s LINE: %d\n\n", function, file, line);fflush(stdout);
			TC_write_syslog("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
		}
		EMH_clear_errors();
		
	}
	return return_code;
}
static void ask_property_value_by_name_ActHndlr(tag_t object, char *prop_name,char **prop_value)
{
    tag_t prop_tag = NULLTAG;

    ITK_CALL(PROP_UIF_ask_property_by_name(object, prop_name, &prop_tag));
    ITK_CALL(PROP_UIF_ask_value(prop_tag, prop_value));
}
static tag_t ask_item_revision_from_bom_line_ActHndlr(tag_t bom_line)
{
    tag_t item_revision = NULLTAG;
    char *item_id = NULL, *rev_id = NULL;
    
    ITK_CALL(AOM_ask_value_string(bom_line, "bl_item_item_id", &item_id ));
    ITK_CALL(AOM_ask_value_string(bom_line, "bl_rev_item_revision_id", &rev_id));
    ITK_CALL(ITEM_find_rev(item_id, rev_id, &item_revision));
    
    if (item_id) MEM_free(item_id);
    if (rev_id) MEM_free(rev_id);
    return item_revision;
}
void GetDifromDesignRev_ActHndlrCC(
									tag_t bom_line,
									int* depth,
									char* inputline,
									tag_t* item_rev_tags,
									char* SubDirectoryName,
									char* PartSubDirectoryName,
									char* DownLoadType,
									char* PartSubDirectoryNameFinal,
									char* timestamp,
									char* VendorName,
									int cadTyp_num_val,
									char** DownloadFormatList
								)
{
	tag_t reln_type =NULLTAG;
	int n_attchs = 0;
	//GRM_relation_t *rellist;
	tag_t* secondary_objects;
	int i = 0;
	int j = 0;
	tag_t primary = NULLTAG;
	int referencenumberfound = 0;
	char refname[AE_reference_size_c + 1];
	AE_reference_type_t reftype;
	tag_t refobject = NULLTAG;
	char orig_name[IMF_filename_size_c + 1];
	char jt_orig_name[IMF_filename_size_c + 1];
	tag_t objTypeTag = NULLTAG;
	char type_name[TCTYPE_name_size_c+1];
	char *prefSupplierDataDir = NULL;
	char* DownloadPath = NULL;
	char* SupplierDataDir = NULL;
	char* SupplierDataLogPath = NULL;

	FILE* SupplierDatafp = NULL;

	char* DirectoryName = NULL;
	tag_t prop_tag = NULLTAG;
	char *type = NULL;
	tag_t user_tag = NULLTAG;
	//char* usernameDir;
	char* usernameDirFin = NULL;
	char* CadDataDownloadDir = NULL;

	int iFmtCnt = 0;
	char* item_id = NULL;
	char* rev_id = NULL;
	char* LatPartRevSeq = NULL;
	char* RelTransMatrix = NULL;
	char* sPartType = NULL;
	logical Is_Suppressed;
	int iIs_Suppressed;
	char* PartRelStat = NULL;
	char jtFname[100];
	FILE* jtfp = NULL;
	int jtFmtFlag = 0;
	tag_t LatestRev = NULLTAG;
	int SuperSed = 0;
	tag_t tItemTag = NULLTAG;
	char* RevSeqStr = NULL;
	char* newPatRev = NULL;
	char* newPatSeq = NULL;
	char* ItemRevSeq = NULL;
	//const char *relation_type_name[] = {"IMAN_specification","IMAN_reference","IMAN_Rendering"};//allowed relation type only 3
	//int ireltyp = 0;
	/*tag_t *relation_type;*/

	printf("\nGetDi no of file format found:%d~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",cadTyp_num_val);
	
	/*for (iFmtCnt = 0; iFmtCnt < cadTyp_num_val  ;iFmtCnt++ )
	{
		printf("\nDownload format %d -> %s",iFmtCnt,DownloadFormatList[iFmtCnt]);
	}

	exit(0);*/

	tc_strcpy(jt_orig_name,"");
	tc_strcpy(jt_orig_name,"NoJtAttached");

	//ITK_CALL(POM_get_user_id(&usernameDir));
	printf("\nSession usernameDir:%s",usernameDir);fflush(stdout);


	ITK_CALL( PREF_ask_char_value("SupplierDataDir", 0 , &SupplierDataDir ) );
	printf("\nSupplierDataDir**:%s",SupplierDataDir ) ;fflush(stdout);

	if( SupplierDataDir == NULL )
	{
		printf("\npreference not found hence default value is /tmp");fflush(stdout);
		SupplierDataDir=(char *) MEM_alloc(50 * sizeof(char ));
		tc_strcpy(SupplierDataDir,"/tmp");
	}

	//if(((*depth)-1)==0)
	//{
		for (iFmtCnt = 0; iFmtCnt < cadTyp_num_val  ;iFmtCnt++ )
		{
			if(tc_strcmp(DownloadFormatList[iFmtCnt],"JTVis")==0)
			{
				jtFmtFlag = 1;
				break;
			}
		}
	//}

	for (iFmtCnt = 0; iFmtCnt < cadTyp_num_val  ;iFmtCnt++ )
	{
		printf("\nDownload format %d -> %s",iFmtCnt,DownloadFormatList[iFmtCnt]);
		
		printf("\nGetDifromDesignRev_ActHndlr VendorName:%s",VendorName);fflush(stdout);
		printf("\nPartSubDirectoryNameFinal GetAllChild_ActHndlr in %s",PartSubDirectoryNameFinal);fflush(stdout);

		DirectoryName = (char *) MEM_alloc(250 * sizeof(char ));
		CadDataDownloadDir = (char *) MEM_alloc(350 * sizeof(char ));
		usernameDirFin = (char *) MEM_alloc(30 * sizeof(char ));

		
		//printf("\nDownLoadType:%s",DownLoadType);fflush(stdout);
		printf("\nDownLoadType:%s",DownloadFormatList[iFmtCnt]);fflush(stdout);

		ITK_CALL(TCTYPE_ask_object_type(*item_rev_tags,&objTypeTag));
		ITK_CALL(TCTYPE_ask_name(objTypeTag,type_name));

		ITK_CALL(AOM_ask_value_string(*item_rev_tags,"object_string",&ItemRevSeq));
		printf("\nGoing for secondary_objects_only type_name [%s]	ItemRevSeq : %s ",type_name,ItemRevSeq); fflush(stdout);

		ITK_CALL(GRM_list_secondary_objects_only(*item_rev_tags,reln_type,&n_attchs,&secondary_objects));
		printf("\nn_attchs:%d",n_attchs);fflush(stdout);
		for (i= 0; i < n_attchs; i++)
		{
			printf("\nsecondary i:%d",i);fflush(stdout);
			//primary=rellist[i].secondary;
			primary=secondary_objects[i];
			
			ITK_CALL(TCTYPE_ask_object_type(primary,&objTypeTag));
			ITK_CALL(TCTYPE_ask_name(objTypeTag,type_name));
			printf("\tType Name:%s",type_name);fflush(stdout);


			if(tc_strcmp(type_name,"Design Revision Master")==0)
			{
				printf("\thence Skipping");fflush(stdout);
				continue;
			}
			else if(tc_strcmp(type_name,"Design Revision")==0)
			{
				printf("\thence Skipping");fflush(stdout);
				continue;
			}
			else if(tc_strcmp(type_name,"Design")==0)
			{
				printf("\thence Skipping");fflush(stdout);
				continue;
			}
			
			if(
				(tc_strcmp(type_name,"CMI2Product")==0) ||
				(tc_strcmp(type_name,"CMI2Part")==0) ||
				(tc_strcmp(type_name,"CMI2Drawing")==0) ||
				(tc_strcmp(type_name,"CMI2Cgr")==0) ||
				(tc_strcmp(type_name,"DirectModel")==0) ||
				(tc_strcmp(type_name,"PDF")==0) ||
				(tc_strcmp(type_name,"ProDrw")==0) ||
				(tc_strcmp(type_name,"ProAsm")==0) ||
				(tc_strcmp(type_name,"ProPrt")==0)
			)
			{
				ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
				printf("\nreferencenumberfound:%d",referencenumberfound);fflush(stdout);
				for(j = 0 ;j < referencenumberfound; j++)
				{
					printf("\nj:%d",j);fflush(stdout);
					ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
					ask_property_value_by_name_ActHndlr(refobject, "Type", &type);
					printf("\treference type:%s",type);//CMI2TypeFormfflush(stdout);
					if(tc_strcmp(type,"CMI2BoundingBoxForm")==0)
					{
						printf("\nSkipping:%s",type);fflush(stdout);
						continue;
					}
					else if(tc_strcmp(type,"CMI2AuxFileInfoForm")==0)
					{
						printf("\nSkipping:%s",type);fflush(stdout);
						continue;
					}
					else if(tc_strcmp(type,"CMI2TypeForm")==0)
					{
						printf("\nSkipping:%s",type);fflush(stdout);
						continue;
					}
					ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));
					printf("\nnSupplierData orig_name:%s",orig_name);fflush(stdout);
					
					//printf("\nDownLoadTypeDI:%s",DownLoadType);fflush(stdout);
					printf("\nDownLoadTypeDI:%s",DownloadFormatList[iFmtCnt]);fflush(stdout);
					//if(tc_strcmp(DownLoadType,"JTVis")==0)
					if(tc_strcmp(DownloadFormatList[iFmtCnt],"JTVis")==0)
					{
						jtFmtFlag = 1;
						if(tc_strstr(orig_name,".jt")==NULL)
						{
							printf("\nJT download skipping");fflush(stdout);
							continue;
						}
						else
						{
							tc_strcpy(jt_orig_name,"");
							tc_strcpy(jt_orig_name,orig_name);
						}
					}
					//else if(tc_strcmp(DownLoadType,"PDFVis")==0)
					else if(tc_strcmp(DownloadFormatList[iFmtCnt],"PDFVis")==0)
					{
						if(tc_strstr(orig_name,".pdf")==NULL)
						{
							continue;
						}
						else if (*depth != 1 ) //skipp all other level for pdf download
						{
							continue;
						}

					}
					//else if(tc_strcmp(DownLoadType,"CGR")==0)
					else if(tc_strcmp(DownloadFormatList[iFmtCnt],"CGR")==0)
					{
						if(tc_strstr(orig_name,".cgr")==NULL)
						{
							continue;
						}
					}
					//else if(tc_strcmp(DownLoadType,"hm")==0)
					else if(tc_strcmp(DownloadFormatList[iFmtCnt],"hm")==0)
					{
						if(tc_strstr(orig_name,".hm")==NULL)
						{
							continue;
						}
					}
					//else if(tc_strcmp(DownLoadType,"STEPF")==0)
					else if(tc_strcmp(DownloadFormatList[iFmtCnt],"STEPF")==0)
					{
						if(tc_strstr(orig_name,".stp")==NULL)
						{
							continue;
						}
					}
					//else if(tc_strcmp(DownLoadType,"VDAF")==0)
					else if(tc_strcmp(DownloadFormatList[iFmtCnt],"VDAF")==0)
					{
						if(tc_strstr(orig_name,".vda")==NULL)
						{
							continue;
						}
					}
					//else if(tc_strcmp(DownLoadType,"CAD3D")==0)
					else if(tc_strcmp(DownloadFormatList[iFmtCnt],"CAD3D")==0)
					{
						printf("\ntype_name:%s",type_name);fflush(stdout);
						printf("\ntype:%s",type);fflush(stdout);
						//CMI2Part	CATPart
						//CMI2Product CATProduct
						//Creo assembly	AsmFile .asm	ProAsm
						//Creo part	PrtFile .prt	ProPrt
						if(
							(tc_strstr(type_name,"Creo assembly")==NULL) 
							&& (tc_strstr(type_name,"Creo part")==NULL)
							&& (tc_strstr(type_name,"ProAsm")==NULL)
							&& (tc_strstr(type_name,"ProPrt")==NULL)
							&& (tc_strstr(type_name,"CMI2Part")==NULL)
							&& (tc_strstr(type_name,"CMI2Product")==NULL)
							)
						{
							continue;
						}
						else if(tc_strstr(orig_name,".jpg")!=NULL)
						{
							continue;
						}
						else if(tc_strstr(orig_name,".cgr")!=NULL)
						{
							continue;
						}
					}
					//else if(tc_strcmp(DownLoadType,"CAD2D")==0)
					else if(tc_strcmp(DownloadFormatList[iFmtCnt],"CAD2D")==0)
					{
						//Creo drawing	DrwFile .drw
						printf("\ntype_name:%s",type_name);fflush(stdout);
						printf("\ntype:%s",type);fflush(stdout);
						//if((tc_strstr(type_name,"Pro")==NULL) && (tc_strstr(type_name,"CMI2")==NULL))//CATDrawing
						if((tc_strstr(type_name,"Creo drawing")==NULL)&& (tc_strstr(type_name,"ProDrw")==NULL) && (tc_strstr(type_name,"CMI2Drawing")==NULL))
						{
							continue;
						}
						else if(tc_strstr(orig_name,".jpg")!=NULL)
						{
							continue;
						}
						else if(tc_strstr(orig_name,".cgr")!=NULL)
						{
							continue;
						}
					}

					tc_strcpy(CadDataDownloadDir,PartSubDirectoryNameFinal);
					tc_strcat(CadDataDownloadDir,"/");
					tc_strcat(CadDataDownloadDir,orig_name);

					printf("\nPartSubDirectoryNameFinal:%s",PartSubDirectoryNameFinal);fflush(stdout);
					printf("\nDownloading...:%s",CadDataDownloadDir);fflush(stdout);
					IMF_export_file(refobject,CadDataDownloadDir);
					
					printf("\nGetDifromDesignRev_ActHndlr SupplierDataDir:%s",SupplierDataDir);
					printf("\nGetDifromDesignRev_ActHndlr usernameDir:%s",usernameDir);
					printf("\nGetDifromDesignRev_ActHndlr SubDirectoryName:%s",SubDirectoryName);
					SupplierDataLogPath=(char *) MEM_alloc(200 * sizeof(char ));//create log file to attach to folder
					tc_strcpy(SupplierDataLogPath,SupplierDataDir);
					tc_strcat(SupplierDataLogPath,"/");
					tc_strcat(SupplierDataLogPath,usernameDir);
					tc_strcat(SupplierDataLogPath,"/");
					tc_strcat(SupplierDataLogPath,SubDirectoryName);
					tc_strcat(SupplierDataLogPath,"/");
					tc_strcat(SupplierDataLogPath,usernameDir);
					tc_strcat(SupplierDataLogPath,"_");
					tc_strcat(SupplierDataLogPath,VendorName);
					tc_strcat(SupplierDataLogPath,"_");
					tc_strcat(SupplierDataLogPath,PartSubDirectoryName);
					tc_strcat(SupplierDataLogPath,"_");
					tc_strcat(SupplierDataLogPath,timestamp);
					tc_strcat(SupplierDataLogPath,".txt");

					printf("\nSupplierDataLogPath:%s",SupplierDataLogPath);fflush(stdout);
					SupplierDatafp = fopen(SupplierDataLogPath,"a");
					fprintf(SupplierDatafp,"%s\n",CadDataDownloadDir);
					

				}
			}
		}
	}
	
	if(jtFmtFlag == 1)
	{
		printf("\njt bom download flag is on\n");fflush(stdout);
		sprintf(jtFname,"%s/%s/%s/BOMList_JTFiles_%s.txt",SupplierDataDir,usernameDir,SubDirectoryName,PartSubDirectoryName);
		jtfp = fopen(jtFname,"a");
		printf("jt bom download filename:%s\n",jtFname);fflush(stdout);
		
		ITK_CALL(AOM_ask_value_string(bom_line, "bl_item_item_id", &item_id ));
		ITK_CALL(AOM_ask_value_string(bom_line, "bl_rev_item_revision_id", &rev_id));
		
		
		RevSeqStr = (char *) MEM_alloc(5 * sizeof(char ));
		newPatRev = (char *) MEM_alloc(5 * sizeof(char ));
		newPatSeq = (char *) MEM_alloc(5 * sizeof(char ));
		tc_strcpy(RevSeqStr,rev_id);
		newPatRev = strtok ( RevSeqStr, ";" );
		newPatSeq = strtok ( NULL, ";" );

		printf("\nnewPatRev:%s",newPatRev);
		printf("\nnewPatSeq:%s::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::",newPatSeq);

		ITK_CALL(AOM_ask_value_string(*item_rev_tags,"t5_PartType",&sPartType));
		ITK_CALL(AOM_UIF_ask_value(*item_rev_tags,"release_status_list",&PartRelStat));
		
		ITK_CALL(AOM_ask_value_logical(bom_line, "bl_is_occ_suppressed", &Is_Suppressed));
		if(Is_Suppressed == 1)
		{
			iIs_Suppressed = 1;
		}
		else
		{
			iIs_Suppressed = 0;
		}

		ITK_CALL(AOM_ask_value_string(bom_line, "bl_plmxml_occ_xform", &RelTransMatrix));
		printf("%d^%s^%s^%s^%s^%s^%s^%d^%s^\n",(*depth)-1,item_id,newPatRev,newPatSeq,sPartType,jt_orig_name,PartRelStat,iIs_Suppressed,RelTransMatrix);fflush(stdout);
		fprintf(jtfp,"%d^%s^%s^%s^%s^%s^%s^%d^%s^\n",(*depth)-1,item_id,newPatRev,newPatSeq,sPartType,jt_orig_name,PartRelStat,iIs_Suppressed,RelTransMatrix);
		if(jtfp!=NULL)
		{
			fclose(jtfp);
		}
		//printf("\nFile closed");
	}
	if(SupplierDatafp!=NULL)
	{
		fclose(SupplierDatafp);
	}
	//}
	
}
void GetAllChild_ActHndlrCC(
							char* inputline,
							tag_t* top_line,
							int depth,
							char* SubDirectoryName,
							char* PartSubDirectoryName,
							char* DownLoadType,
							char* PartSubDirectoryNameFinal,
							char* timestamp,
							char* VendorName,
							int cadTyp_num_val,
							char** DownloadFormatList
						)
{
	int n = 0;
	tag_t* children = NULLTAG;
	int k = 0;
	int Item_ID = 0;
	int Item_RevSeq = 0;
	char *Item_ID_str = NULL;
	char *Item_RevSeq_str = NULL;
	tag_t ChildItemRev = NULLTAG;
	
	depth++;
	printf("\nGetAllChild_ActHndlr VendorName:%s",VendorName);fflush(stdout);
	printf("\nPartSubDirectoryNameFinal GetAllChild_ActHndlr in %s",PartSubDirectoryNameFinal);fflush(stdout);
	ITK_CALL( BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
	ITK_CALL( BOM_line_look_up_attribute ("bl_rev_item_revision_id",&Item_RevSeq));
	ITK_CALL(BOM_line_ask_attribute_string(*top_line, Item_ID, &Item_ID_str));
	ITK_CALL(BOM_line_ask_attribute_string(*top_line, Item_RevSeq, &Item_RevSeq_str));
	printf("\n%d]Item_ID_str: [%s] [%s]",depth,Item_ID_str,Item_RevSeq_str);fflush(stdout);
	//bl_plmxml_occ_xform
	ChildItemRev = ask_item_revision_from_bom_line_ActHndlr(*top_line);
	
	if (ChildItemRev==NULLTAG)
	{
		printf("\nNULLTAG FOUND FOR BOM LINE ITEM : %s",Item_ID_str);fflush(stdout);
	}
	else
	{
		GetDifromDesignRev_ActHndlrCC(
										*top_line,
										&depth,
										inputline,
										&ChildItemRev,
										SubDirectoryName,
										PartSubDirectoryName,
										DownLoadType,
										PartSubDirectoryNameFinal,
										timestamp,
										VendorName,
										cadTyp_num_val,
										DownloadFormatList
									);

		ITK_CALL(BOM_line_ask_child_lines (*top_line, &n, &children));

		printf("\nAssembly:%s no of child found:%d",Item_ID_str,n);fflush(stdout);
		for (k = 0; k < n; k++)
		{
			GetAllChild_ActHndlrCC(
									inputline,
									&children[k],
									depth,
									SubDirectoryName,
									PartSubDirectoryName,
									DownLoadType,
									PartSubDirectoryNameFinal,
									timestamp,
									VendorName,
									cadTyp_num_val,
									DownloadFormatList
								);
		}
	}
}

void ExpandRecursively_ActHndlrCC
								(
									char* cnfgContext,
									char* inputline,
									tag_t* item_rev_tags,
									tag_t* FolderobjTag,
									char* DownLoadType,
									char* PartSubDirectoryNameFinal,
									char* SubDirectoryName,
									char* PartSubDirectoryName,
									char* timestamp,
									char* SupplierDataDir,
									char* usernameDir,
									char* VendorName,
									int cadTyp_num_val,
									char** DownloadFormatList
								)
{
	tag_t window = NULLTAG;
	tag_t top_line = NULLTAG;
	tag_t rule = NULLTAG;
	int Item_ID = 0;
	char *Item_ID_str = NULL;
	char *sPartNumber = NULL;
	char *DatasetFileNamePath = NULL;
	char *DatasetFileName = NULL;
	char *t5_ViewName = NULL;
	char *t5_CfgContext = NULL;

	tag_t dataset = NULLTAG; 
    tag_t ds_type = NULLTAG; 
    tag_t tool = NULLTAG; 
    tag_t relation_type = NULLTAG; 
    tag_t relation = NULLTAG; 
    tag_t file = NULLTAG;
	IMF_file_t descriptor;
	FILE* datasetfp = NULL;
	tag_t query = NULLTAG;
	int n_entries	= 3;
	char *qry_entries[3] = {"SYSCD","SUBSYSCD","Information-2"};
   char *qry_values[3];
	tag_t *cntr_objects = NULL;
	char* ClosureRule = NULL;
	char* CxtRevRuleName = NULL;
	int iObjCntr = 0;
	int n_found = 0;

	PIE_scope_t scope;
	int n_closure_tags;
	tag_t* closure_tags;
	tag_t closure_tag;

	
	/************************Query for context and closure rule*************************/
	
	ITK_CALL(QRY_find("Control Objects...", &query));
	if(query == NULLTAG)
	{
		printf("QuerySupCtx Query not found CreateQuery in query builder");fflush(stdout);
	}
	else
	{
		//t5_Syscd=SupCxt	t5_SubSyscd=view t5_Userinfo1=closurerule	t5_Userinfo2=cfgcontext	t5_Userinfo3=cfgvalue
		ITK_CALL(AOM_ask_value_string(*FolderobjTag,"t5_ViewName",&t5_ViewName));
		ITK_CALL(AOM_ask_value_string(*FolderobjTag,"t5_CfgContext",&t5_CfgContext));

		printf("\nexp View :%s",t5_ViewName);fflush(stdout);
		printf("\nexp CfgContext :%s",t5_CfgContext);fflush(stdout);

		qry_values[0] = "SupCxt";
		qry_values[1] = t5_ViewName;
		qry_values[2] = t5_CfgContext;

		ITK_CALL(QRY_execute(query, n_entries, qry_entries, qry_values, &n_found, &cntr_objects));
		
		if(n_found <= 0)
		{
			printf("Control object not found for view and context Raise with Admin to Create:%s,%s",t5_ViewName);fflush(stdout);
		}
		else
		{
			for (iObjCntr = 0; iObjCntr < n_found; iObjCntr++)
			{
				ITK_CALL ( AOM_ask_value_string(cntr_objects[iObjCntr],"t5_Userinfo1",&ClosureRule));//ClosureRule
				ITK_CALL ( AOM_ask_value_string(cntr_objects[iObjCntr],"t5_Userinfo3",&CxtRevRuleName));//CxtRevRuleName
			}

			/************************Query for context and closure rule*************************/

			printf("\nExpandRecursively_ActHndlr VendorName:%s",VendorName);fflush(stdout);
			printf("\nPartSubDirectoryNameFinal ExpandRecursively_ActHndlr in %s",PartSubDirectoryNameFinal);fflush(stdout);
			
			ITK_CALL(BOM_create_window (&window));
			ITK_CALL(CFM_find( CxtRevRuleName, &rule ));
			//ITK_CALL(CFM_find( "ERC release and above", &rule ));
			if(rule != NULLTAG)
			{
				ITK_CALL(BOM_set_window_config_rule( window, rule ));
			}
			else
			{
				printf("\nNo revision rule find create in control object SupCxt");fflush(stdout);
			}
			
			if( tc_strcmp(t5_ViewName,"ERC") != 0 )	//not applied closure rule for ERC
			{
				ITK_CALL(PIE_find_closure_rules2(ClosureRule,scope,&n_closure_tags,&closure_tags));
				printf("\n n_closure_tags:%d ..............",n_closure_tags);fflush(stdout);
				if(n_closure_tags==1)
				{
					closure_tag=closure_tags[0];
					ITK_CALL(BOM_window_set_closure_rule(window,closure_tag,0,NULL,NULL));
				}
			}

			ITK_CALL(BOM_set_window_pack_all (window, FALSE));
			ITK_CALL(BOM_set_window_top_line (window,null_tag, *item_rev_tags, null_tag, &top_line));

			ITK_CALL( BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
			ITK_CALL(BOM_line_ask_attribute_string(top_line, Item_ID, &Item_ID_str));
			printf("\nItem_ID_str#:%s",Item_ID_str);fflush(stdout);

			GetAllChild_ActHndlrCC(
									inputline,
									&top_line,
									0,
									SubDirectoryName,
									PartSubDirectoryName,
									DownLoadType,
									PartSubDirectoryNameFinal,
									timestamp,
									VendorName,
									cadTyp_num_val,
									DownloadFormatList
								);
			
			//below code commented for ITK client code 18.07.2018
			//create dataset
			ITK_CALL(AE_find_datasettype("Text", &ds_type));
			ITK_CALL(AE_find_tool("Notepad", &tool));

			//create dataset
			ITK_CALL(AE_create_dataset_with_id(ds_type, inputline, "SupplierDataset", inputline, "NR", &dataset));
			ITK_CALL(AE_set_dataset_tool( dataset, tool));
			ITK_CALL(AE_set_dataset_format(dataset, "TEXT_REF"));
			ITK_CALL(AOM_save(dataset)); 

			DatasetFileName = (char*) MEM_alloc(100*sizeof( char ));
			DatasetFileNamePath = (char*) MEM_alloc(200*sizeof( char ));
			//CompletionMsg = (char*) MEM_alloc(300*sizeof( char ));

			tc_strcpy(DatasetFileName,usernameDir);
			tc_strcat(DatasetFileName,"_");
			tc_strcat(DatasetFileName,VendorName);
			tc_strcat(DatasetFileName,"_");
			tc_strcat(DatasetFileName,PartSubDirectoryName);
			tc_strcat(DatasetFileName,"_");
			tc_strcat(DatasetFileName,timestamp);
			tc_strcat(DatasetFileName,".txt");

			tc_strcpy(DatasetFileNamePath,SupplierDataDir);
			tc_strcat(DatasetFileNamePath,"/");
			tc_strcat(DatasetFileNamePath,usernameDir);
			tc_strcat(DatasetFileNamePath,"/");
			tc_strcat(DatasetFileNamePath,SubDirectoryName);
			tc_strcat(DatasetFileNamePath,"/");
			tc_strcat(DatasetFileNamePath,usernameDir);
			tc_strcat(DatasetFileNamePath,"_");
			tc_strcat(DatasetFileNamePath,VendorName);
			tc_strcat(DatasetFileNamePath,"_");
			tc_strcat(DatasetFileNamePath,PartSubDirectoryName);
			tc_strcat(DatasetFileNamePath,"_");
			tc_strcat(DatasetFileNamePath,timestamp);
			tc_strcat(DatasetFileNamePath,".txt");
			
			datasetfp = fopen(DatasetFileNamePath,"a");
			fprintf(datasetfp,"\nDownLoad completed");
			/*tc_strcpy(CompletionMsg,"DownLoad Completed on path ");
			tc_strcat(CompletionMsg,DatasetFileNamePath);
			EMH_store_error_s1(EMH_severity_error,ITK_errStore,DatasetFileNamePath);*/
			//import file
			printf("\nExpandRecursively_ActHndlr DatasetFileName:%s",DatasetFileName);fflush(stdout);
			printf("\nExpandRecursively_ActHndlr DatasetFileNamePath:%s",DatasetFileNamePath);fflush(stdout);
			if(datasetfp!=NULL)
			{
				fclose(datasetfp);
			}
			
			ITK_CALL(IMF_import_file(DatasetFileNamePath, DatasetFileName, SS_TEXT,&file, &descriptor));
			ITK_CALL(AOM_save(file));
			
			ITK_CALL(AOM_refresh(dataset, TRUE));
			ITK_CALL(AOM_lock(dataset));

			ITK_CALL(AE_add_dataset_named_ref(dataset, "Text", AE_PART_OF,file));
			ITK_CALL(AE_save_myself(dataset));
			ITK_CALL(AOM_unlock(dataset));
			
			ITK_CALL(FL_insert(*FolderobjTag, dataset, -1));
			ITK_CALL(AOM_save(*FolderobjTag)); 
		}
	}
}
Create_SupplierDirCC(tag_t* item_rev_tags,char* PartSubDirectoryNameFinal,char* SubDirectoryName,char* PartSubDirectoryName,char* timestamp,char* SupplierDataDir,char* usernameDir,char* VendorName)
{
	tag_t user_tag=NULLTAG;
//	time_t now;
//	struct tm *timeinfo;
	char *sPartNumber = NULL;
	char*  cRevSeq = NULL;
	char *newPatRev = NULL;
	char *newPatSeq = NULL;

	char* DirectoryName = NULL;
	char* usernameDirFin = NULL;
	
	DirectoryName = (char *) MEM_alloc(250 * sizeof(char ));
	usernameDirFin = (char *) MEM_alloc(100 * sizeof(char ));
	
	printf("\nSession Username:",usernameDir);fflush(stdout);
	printf("\nCreate_SupplierDirCC VendorName:",VendorName);fflush(stdout);

//	time(&now);
//	timeinfo = localtime(&now);
//	strftime(timestamp, 20, "%Y-%m-%d-%H_%M_%S", timeinfo);

	printf("\nTimestamp for Directory:",timestamp);fflush(stdout);
	
	ITK_CALL(AOM_UIF_ask_value(*item_rev_tags,"item_id",&sPartNumber));
	
	printf("\nsPartNumber4:%s",sPartNumber);fflush(stdout);


	ITK_CALL(AOM_UIF_ask_value(*item_rev_tags,"item_revision_id",&cRevSeq));
	printf("\nAOM_UIF_ask_value cRevSeq:%s", cRevSeq);fflush(stdout);
	newPatRev = strtok ( cRevSeq, ";" );
	newPatSeq = strtok ( NULL, ";" );

	if(newPatSeq== NULL)
	{
		newPatSeq = (char*) MEM_alloc(3*sizeof(char));
		tc_strcpy(newPatSeq,"99");
	}

	printf("\nRevision:%s",newPatRev);fflush(stdout);
	printf("\nSequence:%s",newPatSeq);fflush(stdout);
	

	//tc_strcpy(SubDirectoryName,usernameDir);
	tc_strcpy(SubDirectoryName,VendorName);//13.08.2018
	tc_strcat(SubDirectoryName,"_");
	tc_strcat(SubDirectoryName,sPartNumber);
	tc_strcat(SubDirectoryName,"_");
	tc_strcat(SubDirectoryName,newPatRev);
	tc_strcat(SubDirectoryName,"_");
	tc_strcat(SubDirectoryName,newPatSeq);
	tc_strcat(SubDirectoryName,"_");
	tc_strcat(SubDirectoryName,timestamp);//date and timestamp

	

	tc_strcpy(PartSubDirectoryName,sPartNumber);
	tc_strcat(PartSubDirectoryName,"_");
	tc_strcat(PartSubDirectoryName,newPatRev);
	tc_strcat(PartSubDirectoryName,"_");
	tc_strcat(PartSubDirectoryName,newPatSeq);

	printf("\nusername:%s",usernameDir);fflush(stdout);

	printf("\nSupplier Data SubDirectoryName:%s",SubDirectoryName);fflush(stdout);

		
	//create user directory under mounted dir
	tc_strcpy(usernameDirFin,SupplierDataDir);//pref dir
	tc_strcat(usernameDirFin,"/");//username dir
	tc_strcat(usernameDirFin,usernameDir);//username dir
	if(mkdir(usernameDirFin,00777)==-1)
	{
		printf("\nerror creating session User directory preference directory %s or already exist",usernameDirFin);fflush(stdout);
	}
	printf("\nCreate_SupplierDir UserDirectory:%s",usernameDirFin);fflush(stdout);

	//create timestamp directory under user directory
	tc_strcpy(DirectoryName,SupplierDataDir); //directory from pref
	tc_strcat(DirectoryName,"/");
	tc_strcat(DirectoryName,usernameDir);
	tc_strcat(DirectoryName,"/");
	tc_strcat(DirectoryName,SubDirectoryName);//directory with timestamp
	if(mkdir(DirectoryName,00777)==-1)
	{
		printf("\nerror creating Supplier directory preference directory %s or already exist",DirectoryName);fflush(stdout);
	}
	printf("\nCreate_SupplierDir DirectoryName:%s",DirectoryName);fflush(stdout);
	
	//create part revision sequence directory under timestamp directory
	tc_strcpy(PartSubDirectoryNameFinal,DirectoryName);
	tc_strcat(PartSubDirectoryNameFinal,"/");
	tc_strcat(PartSubDirectoryNameFinal,PartSubDirectoryName);

	if(mkdir(PartSubDirectoryNameFinal,00777)==-1)
	{
		printf("\nerror creating Supplier Part directory preference directory %s or already exist",PartSubDirectoryNameFinal);fflush(stdout);
	}
	printf("\nCreate_SupplierDir PartSubDirectoryNameFinal:%s",PartSubDirectoryNameFinal);fflush(stdout);

}
callt5DataDownLoadMechCC(tag_t* objTag,FILE** fp,int* i,char* DwnloadFrmtSelctStr)
{
	tag_t objTypTag = NULLTAG;
	tag_t objTypeTag1 = NULLTAG;
	char ObjTyp[TCTYPE_name_size_c+1];
	char type_name[TCTYPE_name_size_c+1];
	char* FolderObjectName = NULL;
	char** DownloadFormatList = NULL;
	char** PartnumberList = NULL;
	char* DownloadFormat = NULL;
	int cadTyp_num_val = 0;
	char* VendorName = NULL;
	char* cnfgContextVal = NULL;
	int num_values = 0;
	//int i = 0;
	int count = 0;
	tag_t* tags = NULLTAG;
	tag_t item_rev_tags = NULLTAG;
	char* sPartNumber = NULL;
	char* sDescription = NULL;
	char* cRevSeq = NULL;
	char *newPatRev = NULL;
	char *newPatSeq = NULL;
	char* WriteLine = NULL;
	char* DownLoadedBy = NULL;
	tag_t user_tag = NULLTAG;
	char* username = NULL;
	char* PartClass = NULL;
	char* ViewName = NULL;
	char* tok;
	char* tok1;
	char partarr[50] = { 0 };


	DownloadFormat = (char *) MEM_alloc(50 * sizeof(char ));
	PartClass = (char *) MEM_alloc(20 * sizeof(char ));
	
	ITK_CALL(TCTYPE_ask_object_type(*objTag,&objTypTag));
	ITK_CALL(TCTYPE_ask_name(objTypTag,ObjTyp));
	printf("\ncallt5DataDownLoadMech Object type allowed is folder: [%s]", ObjTyp);fflush(stdout);

	ITK_CALL(AOM_UIF_ask_value(*objTag,"object_name",&FolderObjectName));
	printf("\nFolderObjectName:%s",FolderObjectName);fflush(stdout);
	//ITK_CALL(AOM_ask_value_strings(*objTag,"t5_PckgDataTyp",&cadTyp_num_val,&DownloadFormatList));
	ITK_CALL(AOM_ask_value_strings(*objTag,"t5_FileType",&cadTyp_num_val,&DownloadFormatList));
	printf("\nNo of cad type selected:%d only downloading data for first cad type",cadTyp_num_val);
	DownloadFormat = DownloadFormatList[0];
	printf("\nDownloadFormat:%s",DownloadFormat);fflush(stdout);

	ITK_CALL(AOM_ask_value_string(*objTag,"t5_VendorID",&VendorName));//old t5_PackVendor       new t5_VendorID
	if(VendorName == NULL)
	{
		VendorName = (char *) MEM_alloc(50*sizeof(char ));
		tc_strcpy(VendorName,"No_Supplier");
	}
	printf("\nmain VendorName:%s",VendorName);fflush(stdout);



	ITK_CALL(AOM_UIF_ask_value(*objTag,"t5_CfgContext",&cnfgContextVal));
	//ITK_CALL(AOM_ask_value_string(*objTag,"t5_CfgContext",&cnfgContextVal));
	if(cnfgContextVal == NULL)
	{
		cnfgContextVal = (char *) MEM_alloc(50 * sizeof(char ));
		tc_strcpy(cnfgContextVal,"ERC Review And Above");
		//tc_strcpy(cnfgContextVal,"Latest Working");
		//tc_strcpy(cnfgContextVal,"ERC release and above");
	}
	
	printf("\ncnfgContext:%s",cnfgContextVal);fflush(stdout);

	ITK_CALL(AOM_ask_value_string(*objTag,"t5_ViewName",&ViewName));
	if(ViewName == NULL)
	{
		ViewName = (char *) MEM_alloc(50 * sizeof(char ));
		tc_strcpy(ViewName,"ERC");
	}
	printf("\nViewName:%s",ViewName);fflush(stdout);
	
	//ITK_CALL(POM_get_user_id(&DownLoadedBy));
	tc_strdup(usernameDir,&DownLoadedBy);
	printf("\nDownLoadedBy userid:%s",DownLoadedBy);

	ITK_CALL(SA_find_user2(usernameDir,&user_tag));

	ITK_CALL(SA_ask_user_person_name2(user_tag,&username));

	//ITK_CALL(POM_get_user(&username,&user_tag));
	printf("\nDownLoadedBy username:%s",username);

	ITK_CALL(AOM_UIF_ask_values(*objTag,"t5_PartNumList",&num_values,&PartnumberList));
	
	printf("\nProcessing part in list:%d:%s",*i,PartnumberList[*i]);
	WriteLine = (char *) MEM_alloc(500 * sizeof(char ));

	
	tok = NULL;
	tok1 = NULL;
	tc_strcpy(partarr,"");
	printf("\nPartnumber5:%s",PartnumberList[0]);fflush(stdout);
	strcpy(partarr,PartnumberList[*i]);
	partarr[strlen(PartnumberList[*i])]= '\0';
	tok = strtok(partarr,"/");
	if(tok!=NULL)
	{
			tok1 = strtok(NULL,"/");
	}
	printf("\t%s\t%s\t%s",partarr,tok,tok1);

	//printf("\nPartnumber:%s",PartnumberList[*i]);fflush(stdout);
	ITK_CALL(ITEM_find(partarr,&count,&tags));
	if( count > 0 )
	{
		printf("\na Query Supplier Part Number:%s found in tcua",PartnumberList[*i]);fflush(stdout);
		ITK_CALL(ITEM_find_revision(tags[0],tok1,&item_rev_tags));
		//ITEM_ask_latest_rev(tags[0],&item_rev_tags);

		ITK_CALL(AOM_UIF_ask_value(item_rev_tags,"item_id",&sPartNumber));

		ITK_CALL(AOM_ask_value_string(item_rev_tags,"object_desc",&sDescription));

		ITK_CALL(TCTYPE_ask_object_type(item_rev_tags,&objTypeTag1));
		ITK_CALL(TCTYPE_ask_name(objTypeTag1,type_name));
		
		if(tc_strcmp(type_name,"Design Revision")==0)
		{
			tc_strcpy(PartClass,"Assembly");
		}
		else
		{
			tc_strcpy(PartClass,"Not Valid Class for part");
		}
		
		ITK_CALL(AOM_UIF_ask_value(item_rev_tags,"item_revision_id",&cRevSeq));
		
		newPatRev = strtok ( cRevSeq, ";" );
		newPatSeq = strtok ( NULL, ";" );
		if(newPatSeq== NULL)
		{
			newPatSeq = (char*) MEM_alloc(3*sizeof(char));
			tc_strcpy(newPatSeq,"99");
		}

		printf("\nPartNumber1:%s",sPartNumber);fflush(stdout);
		printf("\nPatRev:%s",newPatRev);fflush(stdout);
		printf("\nPatSeq:%s",newPatSeq);fflush(stdout);

		tc_strcpy(WriteLine,sPartNumber);
		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,newPatRev);
		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,newPatSeq);
		
		
		//if(tc_strcmp(DownloadFormat,"CAD3D")==0)
		if(tc_strstr(DwnloadFrmtSelctStr,"CAD3D")!=NULL)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");
		}
		//if(tc_strcmp(DownloadFormat,"CAD2D")==0)
		if(tc_strstr(DwnloadFrmtSelctStr,"CAD2D")!=NULL)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");
		}
		if(tc_strstr(DwnloadFrmtSelctStr,"Converted")!=NULL)
		//if(tc_strcmp(DownloadFormat,"Converted")==0)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");
		}
		if(tc_strstr(DwnloadFrmtSelctStr,"JTVis")!=NULL)
		//if(tc_strcmp(DownloadFormat,"JTVis")==0)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");
		}
		if(tc_strstr(DwnloadFrmtSelctStr,"PDFVis")!=NULL)
		//if(tc_strcmp(DownloadFormat,"PDFVis")==0)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");
		}

		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,VendorName);
		
		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,DownLoadedBy);
		
		tc_strcat(WriteLine,":>:");
		tc_strcat(WriteLine,username);
		
		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,"CE Vault");
		
		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,cnfgContextVal);

		tc_strcat(WriteLine,":;:PartNumber:;:NA:;:NA:;:NA:;:NA:;:");

		tc_strcat(WriteLine,sDescription);

		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,PartClass);

		tc_strcat(WriteLine,":;:DD-MM-YY");

		tc_strcat(WriteLine,":;:Default");//download dir
		
		tc_strcat(WriteLine,":;:-");//single dir

		tc_strcat(WriteLine,":;:+");//Multiple dir dir

		tc_strcat(WriteLine,":;:Default");//catia version
		
		if(tc_strstr(DwnloadFrmtSelctStr,"VDAF")!=NULL)
		//if(tc_strcmp(DownloadFormat,"VDAF")==0)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");
		}
		if(tc_strstr(DwnloadFrmtSelctStr,"STEPF")!=NULL)
		//if(tc_strcmp(DownloadFormat,"STEPF")==0)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");
		}

		tc_strcat(WriteLine,":;:-");//iges option is commented
		
		if(tc_strstr(DwnloadFrmtSelctStr,"CGR")!=NULL)
		//if(tc_strcmp(DownloadFormat,"CGR")==0)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");
		}
		if(tc_strstr(DwnloadFrmtSelctStr,"HM")!=NULL)
		//if(tc_strcmp(DownloadFormat,"HM")==0)
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"+");
		}
		else
		{
			tc_strcat(WriteLine,":;:");
			tc_strcat(WriteLine,"-");

		}
		
		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,"");//downloaders remark
		
		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,"-");//download pdf

		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,"99");//download pdf

		//- -
		
		tc_strcat(WriteLine,":;:-:;:-");//unable to find property name in mth also

		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,ViewName);//view name

		
		tc_strcat(WriteLine,":;::;:-");//unable to find property name in mth also

		tc_strcat(WriteLine,":;:");
		tc_strcat(WriteLine,"1");//include zero quantity
		

		tc_strcat(WriteLine,":;:TML_LOCAL_SHARE:>:msdcad_sys:>:msdcad_sysy1:>:animesh.mishra@tatatechnologies.com:>::;::;:");
		printf("\n*****####WriteLine:%s",WriteLine);

		fprintf(*fp,"%s\n",WriteLine);
	}
	
}

callJtFileShellCC(char* VendorName,char* inputline,char* SupplierDataDir,char* usernameDir,char* timestamp)
{
	//CollectJTFiles_IND_Relative_PLM.sh <BOM File> < Download JT Dir> < NEW JT Directory> 
	char* JTDataShellDir = NULL;
	char* JTinputFile = NULL;
	char* JTFileDir = NULL;
	char* NewJTDir = NULL;
	char* JtShellCommandLine = NULL;

	JTinputFile = (char *) MEM_alloc(200*sizeof(char ));
	JTFileDir = (char *) MEM_alloc(200*sizeof(char ));
	NewJTDir = (char *) MEM_alloc(400*sizeof(char ));
	JtShellCommandLine = (char *) MEM_alloc(1000*sizeof(char ));

	ITK_CALL( PREF_ask_char_value("JTDataShellDir", 0 , &JTDataShellDir ) );
	printf("\nJTDataShellDir : %s",JTDataShellDir ) ;fflush(stdout);

	if( JTDataShellDir == NULL )
	{
		printf("\nJTDataShellDir preference not found hence default value is /tmp");fflush(stdout);
		JTDataShellDir = (char *) MEM_alloc(50 * sizeof(char ));
		tc_strcpy(JTDataShellDir,"/tmp");
	}

	//BomListJTFile -> /proj/plmdownload/loader/M64061_54421624R_B_1_2018-09-14-09_51_37/BOMList_JTFiles_54421624R_B_1.txt
	//Downloaded JT File Dir->	/proj/plmdownload/loader/M64061_54421624R_B_1_2018-09-14-09_51_37/54421624R_B_1

	tc_strcpy(JTinputFile,SupplierDataDir);
	tc_strcat(JTinputFile,"/");
	tc_strcat(JTinputFile,usernameDir);
	tc_strcat(JTinputFile,"/");
	tc_strcat(JTinputFile,VendorName);
	tc_strcat(JTinputFile,"_");
	tc_strcat(JTinputFile,inputline);
	tc_strcat(JTinputFile,"_");
	tc_strcat(JTinputFile,timestamp);
	tc_strcat(JTinputFile,"/");
	
	tc_strcpy(JTFileDir,JTinputFile);
	tc_strcpy(NewJTDir,JTinputFile);
	
	tc_strcat(JTinputFile,"BOMList_JTFiles_");
	tc_strcat(JTinputFile,inputline);
	tc_strcat(JTinputFile,".txt");

	tc_strcat(JTFileDir,inputline);

	printf("\nJTDataShell: %s",JTDataShellDir);fflush(stdout);
	printf("\nJTDataShell Input1: %s",JTinputFile);fflush(stdout);
	printf("\nJTDataShell Input2: %s",JTFileDir);fflush(stdout);

	tc_strcat(NewJTDir,"JT_");
	tc_strcat(NewJTDir,inputline);
	if(mkdir(NewJTDir,00777)==-1)
	{
		printf("\nerror creating Supplier directory preference directory %s or already exist",NewJTDir);fflush(stdout);
	}
	printf("\nNew JT Directory JTDataShell Input3: %s",NewJTDir);fflush(stdout);
	
	tc_strcpy(JtShellCommandLine,JTDataShellDir);
	tc_strcat(JtShellCommandLine,"/CollectJTFiles_IND_Relative_PLM.sh");
	tc_strcat(JtShellCommandLine," ");
	tc_strcat(JtShellCommandLine,JTinputFile);
	tc_strcat(JtShellCommandLine," ");
	tc_strcat(JtShellCommandLine,JTFileDir);
	tc_strcat(JtShellCommandLine," ");
	tc_strcat(JtShellCommandLine,NewJTDir);
	printf("\nCommandLine:%s",JtShellCommandLine);fflush(stdout);
	system(JtShellCommandLine);
	printf("\nSystem JtShellCommandLine executed");fflush(stdout);
}
int DownloadSupplierData_Func(char* PackageNum)
{
	tag_t roottask = NULLTAG;
	tag_t *attachment = NULLTAG;
	tag_t objTag = NULLTAG;
	int no_attach = 0;
	tag_t objTypTag = NULLTAG;
	char ObjTyp[TCTYPE_name_size_c+1];
	char* FolderObjectName = NULL;
	char* cnfgContext = NULL;
	char* cnfgContextVal = NULL;
	char* VendorName = NULL;
	char** PartnumberList = NULL;
	int count = 0;
	tag_t* tags = NULLTAG;
	tag_t item_rev_tags = NULLTAG;
	tag_t user_tag=NULLTAG;

	char* inputline = NULL;
	char* DownLoadType = NULL;
	char* DownloadFormat = NULL;
	char** DownloadFormatList = NULL;
	char* PartSubDirectoryNameFinal = NULL;
	char* SubDirectoryName = NULL;
	char* PartSubDirectoryName = NULL;
	char* SupplierDataDir = NULL;
	char* ContextPrefValue = NULL;
	char* timestamp = NULL;
	//char* usernameDir = NULL;
	int num_values = 0;
	int i = 0;
	int cadTyp_num_val = 0;
	char* fInputFileName = NULL;
	//char* userid = NULL;
	time_t now;
	struct tm *timeinfo;

	int num_values1 = 0;
	char** PartnumberList1 = NULL;
	int count1 = 0;
	tag_t* tags1 = NULLTAG;
	tag_t item_rev_tags1 = NULLTAG;
	char* cRevSeq1 = NULL;
	char* newPatRev1 = NULL;
	char* newPatSeq1 = NULL;
	char* DownLoadedBy = NULL;
	//tag_t user_tag = NULLTAG;
	char* username = NULL;
	FILE *fp = NULL;
	char* SupplierDataShell = NULL;
	char* CommandLine = NULL;
	
	char* atok;
	char* atok1;


	char* tok;
	char* tok1;
	char partarr[50] = { 0 };
	int revCnt = 0;
	char* cRevSeq = NULL;
	char* DwnloadFrmtSelctStr = NULL;
	int ifmtCnt = 0;

	char* cRevSeq2 = NULL;
	char* newPatRev2 = NULL;
	char* newPatSeq2 = NULL;
	char* checked_outStat = NULL;

	tag_t* PkgobjTag = NULLTAG;
	WSO_search_criteria_t criteria;
	int count2 = 0;

	inputline = (char *) MEM_alloc(50*sizeof(char ));
	DownLoadType = (char *) MEM_alloc(50*sizeof(char ));
	PartSubDirectoryNameFinal = (char *) MEM_alloc(300);
	SubDirectoryName = (char *) MEM_alloc(100 * sizeof(char ));
	PartSubDirectoryName = (char *) MEM_alloc(100 * sizeof(char ));
	timestamp = (char *) MEM_alloc(20 * sizeof(char ));
	cnfgContext = (char *) MEM_alloc(50 * sizeof(char ));
	DownloadFormat = (char *) MEM_alloc(50 * sizeof(char ));
	fInputFileName = (char *) MEM_alloc(200 * sizeof(char ));
	CommandLine = (char *) MEM_alloc(250 * sizeof(char ));
	DwnloadFrmtSelctStr = (char *) MEM_alloc(250 * sizeof(char ));
	
	ITK_CALL(WSOM_clear_search_criteria(&criteria));
	tc_strcpy(criteria.class_name,"T5_Package");
	
	printf("\nPackage Number  :%s",PackageNum);fflush(stdout);

	tc_strcpy(criteria.name,PackageNum);

	ITK_CALL(WSOM_search(criteria,&count2,&PkgobjTag));

	printf("\nNo of package found :%d",count2);fflush(stdout);
	if(count2 > 0)
	{
		objTag = PkgobjTag[0];


		ITK_CALL( PREF_ask_char_value("SupplierDataDir", 0 , &SupplierDataDir ) );
		printf("\nSupplierDataDir**:%s",SupplierDataDir ) ;fflush(stdout);

		if( SupplierDataDir == NULL )
		{
			printf("\npreference not found hence default value is /tmp");fflush(stdout);
			SupplierDataDir=(char *) MEM_alloc(50 * sizeof(char ));
			tc_strcpy(SupplierDataDir,"/tmp");
		}


		ITK_CALL( PREF_ask_char_value("SupplierDataShell", 0 , &SupplierDataShell ) );
		printf("\nSupplierDataShell**:%s",SupplierDataShell ) ;fflush(stdout);

		if( SupplierDataShell == NULL )
		{
			printf("\npreference not found hence default value is /tmp");fflush(stdout);
			SupplierDataShell = (char *) MEM_alloc(50 * sizeof(char ));
			tc_strcpy(SupplierDataShell,"/tmp");
		}
		
		//ITK_CALL(POM_get_user_id(&usernameDir));

		printf("\nuserid:%s***********",usernameDir);

		printf("\nPackage workflow started here");fflush(stdout);
		/*ITK_CALL(EPM_ask_root_task(msg.task, &roottask));
		ITK_CALL(EPM_ask_attachments(roottask, EPM_target_attachment,&no_attach, &attachment));
		printf("\nNo of Attachements: [%d]", no_attach);fflush(stdout);
		if(no_attach>0)
		{
			objTag = attachment[0];
		}*/

		ITK_CALL(TCTYPE_ask_object_type(objTag,&objTypTag));
		ITK_CALL(TCTYPE_ask_name(objTypTag,ObjTyp));
		printf("\nObject type allowed is folder: [%s]", ObjTyp);fflush(stdout);
		if(tc_strcmp(ObjTyp,"T5_Package")== 0 )
		{

			ITK_CALL(AOM_UIF_ask_value(objTag,"object_name",&FolderObjectName));
			printf("\nFolderObjectName:%s",FolderObjectName);fflush(stdout);
			//ITK_CALL(AOM_ask_value_strings(objTag,"t5_PckgDataTyp",&cadTyp_num_val,&DownloadFormatList));
			ITK_CALL(AOM_ask_value_strings(objTag,"t5_FileType",&cadTyp_num_val,&DownloadFormatList));

			printf("\nNo of cad type selected:%d only downloading data for first cad type",cadTyp_num_val);fflush(stdout);
			if(cadTyp_num_val <= 0)
			{
				printf("\nNo cadtype selected hence exiting from process");fflush(stdout);
				return 0;
			}
			

			//first format copied other are strcat
			tc_strcpy(DwnloadFrmtSelctStr,DownloadFormatList[0]);
			tc_strcat(DwnloadFrmtSelctStr,";");

			for (ifmtCnt = 1; ifmtCnt < cadTyp_num_val ; ifmtCnt++ )
			{
				tc_strcat(DwnloadFrmtSelctStr,DownloadFormatList[ifmtCnt]);
				tc_strcat(DwnloadFrmtSelctStr,";");
			}
			printf("\nDwnloadFrmtSelctStr:%s",DwnloadFrmtSelctStr);fflush(stdout);
			
			DownloadFormat = DownloadFormatList[0];
			printf("\nDownloadFormat:%s",DownloadFormat);fflush(stdout);

			
			if(tc_strcmp(DownloadFormat,"CAD3D")==0)
			{
				tc_strcpy(DownLoadType,"CAD3D");
			}
			else if (tc_strcmp(DownloadFormat,"CAD2D")==0)
			{
				tc_strcpy(DownLoadType,"CAD2D");
			}
			else if(
				tc_strcmp(DownloadFormat,"CGR")==0
			)
			{
				tc_strcpy(DownLoadType,"CGR");
			}
			else if(
				tc_strcmp(DownloadFormat,"Converted")==0
				)
			{
				tc_strcpy(DownLoadType,"Converted");
			}
			else if(
				tc_strcmp(DownloadFormat,"HM")==0
			)
			{
				tc_strcpy(DownLoadType,"HM");
			}
			else if(
				tc_strcmp(DownloadFormat,"JTVis")==0
			)
			{
				tc_strcpy(DownLoadType,"JTVis");
			}
			else if(
				tc_strcmp(DownloadFormat,"PDFVis")==0
			)
			{
				tc_strcpy(DownLoadType,"PDFVis");
			}
			else if(
				tc_strcmp(DownloadFormat,"STEPF")==0
			)
			{
				tc_strcpy(DownLoadType,"STEPF");
			}
			else if(
				tc_strcmp(DownloadFormat,"VDAF")==0
			)
			{
				tc_strcpy(DownLoadType,"VDAF");
			}

			printf("\nDownLoadTypefn:%s",DownLoadType);
			ITK_CALL(AOM_ask_value_string(objTag,"t5_VendorID",&VendorName));//old t5_PackVendor       new t5_VendorID
			if(VendorName == NULL)
			{
				VendorName = (char *) MEM_alloc(50*sizeof(char ));
				tc_strcpy(VendorName,"No_Supplier");
			}
			printf("\nVendorName:%s",VendorName);fflush(stdout);

			ITK_CALL(AOM_ask_value_string(objTag,"t5_CfgContext",&cnfgContextVal));
			printf("\ncnfgContext:%s",cnfgContextVal);fflush(stdout);
			
			//context	1
			if(tc_strcmp(cnfgContextVal,"APLRel")==0)
			{
				ITK_CALL( PREF_ask_char_value("APLRel", 0 , &ContextPrefValue ) );
				if( ContextPrefValue == NULL )
				{
					printf("\npreference not found hence default value is APL Released and Above");fflush(stdout);
					ContextPrefValue=(char *) MEM_alloc(50 * sizeof(char ));
					tc_strcpy(ContextPrefValue,"APL Released and Above");
					tc_strcpy(cnfgContext,"APL Released and Above");
				}
				else
				{
					tc_strcpy(cnfgContext,ContextPrefValue);
				}

				
			}
			else if(tc_strcmp(cnfgContextVal,"LatRel")==0)
			{
				ITK_CALL( PREF_ask_char_value("LatRel", 0 , &ContextPrefValue ) );
				if( ContextPrefValue == NULL )
				{
					printf("\npreference not found hence default value is ERC released and above");fflush(stdout);
					ContextPrefValue=(char *) MEM_alloc(50 * sizeof(char ));
					tc_strcpy(ContextPrefValue,"ERC released and above");
					tc_strcpy(cnfgContext,"ERC released and above");
				}
				else
				{
					tc_strcpy(cnfgContext,ContextPrefValue);
				}
				
			}
			else if(tc_strcmp(cnfgContextVal,"LatVault")==0)
			{
				ITK_CALL( PREF_ask_char_value("LatVault", 0 , &ContextPrefValue ) );
				if( ContextPrefValue == NULL )
				{
					printf("\npreference not found hence default value is ERC Review And Above");fflush(stdout);
					ContextPrefValue=(char *) MEM_alloc(50 * sizeof(char ));
					tc_strcpy(ContextPrefValue,"ERC Review And Above");
					tc_strcpy(cnfgContext,"ERC Review And Above");
					//tc_strcpy(ContextPrefValue,"ERC release and above");
					//tc_strcpy(cnfgContext,"ERC release and above");
				}
				else
				{
					tc_strcpy(cnfgContext,ContextPrefValue);
				}
			}
			else if(tc_strcmp(cnfgContextVal,"STDRel")==0)
			{
				ITK_CALL( PREF_ask_char_value("STDRel", 0 , &ContextPrefValue ) );
				if( ContextPrefValue == NULL )
				{
					printf("\npreference not found hence default value is STD Released and Above");fflush(stdout);
					ContextPrefValue=(char *) MEM_alloc(50 * sizeof(char ));
					tc_strcpy(ContextPrefValue,"STD Released and Above");
					tc_strcpy(cnfgContext,"STD Released and Above");
				}
				else
				{
					tc_strcpy(cnfgContext,ContextPrefValue);
				}
			}
			
			printf("\nExpanding with context:%s",cnfgContext);

			ITK_CALL(AOM_UIF_ask_values(objTag,"t5_PartNumList",&num_values,&PartnumberList));
			printf("\nNo of parts to be download:%d",num_values);
			
			if(num_values > 0)
			{
				//****************************to generate below file name in /tmp****************************
				time(&now);
				timeinfo = localtime(&now);
				strftime(timestamp, 20, "%Y-%m-%d-%H_%M_%S", timeinfo);
				
				//ITK_CALL(POM_get_user_id(&DownLoadedBy));
				tc_strdup(usernameDir,&DownLoadedBy);
				printf("\nDownLoadedBy userid:%s",DownLoadedBy);

				ITK_CALL(SA_find_user2(usernameDir,&user_tag));

				ITK_CALL(SA_ask_user_person_name2(user_tag,&username));

				//ITK_CALL(POM_get_user(&username,&user_tag));
				printf("\nDownLoadedBy username:%s",username);

				ITK_CALL(AOM_UIF_ask_values(objTag,"t5_PartNumList",&num_values1,&PartnumberList1));
				
				atok = NULL;
				atok1 = NULL;
				tc_strcpy(partarr,"");
				printf("\nPartnumber2:%s",PartnumberList1[0]);fflush(stdout);
				strcpy(partarr,PartnumberList1[0]);
				partarr[strlen(PartnumberList1[0])]= '\0';
				atok = strtok(partarr,"/");
				if(atok!=NULL)
				{
						atok1 = strtok(NULL,"/");
				}
				printf("\t%s\t%s\t%s",partarr,atok,atok1);

				ITK_CALL(ITEM_find(partarr,&count1,&tags1));
				//ITEM_ask_latest_rev(tags1[0],&item_rev_tags1);
				
				ITK_CALL(ITEM_find_revision(tags1[0],atok1,&item_rev_tags1));

				ITK_CALL(AOM_UIF_ask_value(item_rev_tags1,"item_revision_id",&cRevSeq1));

				newPatRev1 = strtok ( cRevSeq1, ";" );
				newPatSeq1 = strtok ( NULL, ";" );
				if(newPatSeq1== NULL)
				{
					newPatSeq1 = (char*) MEM_alloc(3*sizeof(char));
					tc_strcpy(newPatSeq1,"99");
				}

				printf("\nPartnumberList1[0]:%s",partarr);fflush(stdout);
				printf("\nnewPatRev:%s",newPatRev1);fflush(stdout);
				printf("\nnewPatSeq:%s",newPatSeq1);fflush(stdout);

				tc_strcpy(fInputFileName,"/tmp/");//linux
				//tc_strcpy(fInputFileName,"d:\\");//windows
				tc_strcat(fInputFileName,DownLoadedBy);
				tc_strcat(fInputFileName,"_");
				tc_strcat(fInputFileName,VendorName);
				tc_strcat(fInputFileName,"_");
				tc_strcat(fInputFileName,partarr);
				tc_strcat(fInputFileName,"_");
				tc_strcat(fInputFileName,newPatRev1);
				tc_strcat(fInputFileName,"_");
				tc_strcat(fInputFileName,newPatSeq1);
				tc_strcat(fInputFileName,"_");
				tc_strcat(fInputFileName,timestamp);
				tc_strcat(fInputFileName,".txt");

				printf("\nfInputFileName:%s",fInputFileName);fflush(stdout);
				
				
				//****************************to generate below file name in /tmp****************************

				fp = fopen(fInputFileName,"a");
				printf("\nNo of parts to be download start:%d",num_values);

				for (i = 0; i < num_values; i++)
				{
					tok = NULL;
					tok1 = NULL;
					tc_strcpy(partarr,"");
					printf("\nPartnumber3:%s:%d",PartnumberList[i],i);fflush(stdout);
					strcpy(partarr,PartnumberList1[i]);
					partarr[strlen(PartnumberList1[i])]= '\0';
					tok = strtok(partarr,"/");
					if(tok!=NULL)
					{
							tok1 = strtok(NULL,"/");
					}
					printf("\t%s\t%s\t%s",partarr,tok,tok1);
					ITK_CALL(ITEM_find(partarr,&count,&tags));
					printf("\ncount for ITEM_find(partarr,&count,&tags):%d",count);fflush(stdout);
					if( count > 0 )
					{
						printf("\nb Query Supplier Part Number:%s found in tcua",partarr);fflush(stdout);
						//ITEM_ask_latest_rev(tags[0],&item_rev_tags);
						ITK_CALL(ITEM_find_revision(tags[0],tok1,&item_rev_tags));
						
						ITK_CALL(AOM_UIF_ask_value(item_rev_tags,"checked_out",&checked_outStat));
						
						if(tc_strcmp(checked_outStat,"Y") == 0)
						{
							printf("\nPart revision seq to be processed is checked-put %s [%s] skipping for data download",PartnumberList1[i],checked_outStat);fflush(stdout);
							continue;
						}

						ITK_CALL(AOM_UIF_ask_value(item_rev_tags,"item_revision_id",&cRevSeq2));

						newPatRev2 = strtok ( cRevSeq2, ";" );
						newPatSeq2 = strtok ( NULL, ";" );
						if(newPatSeq2== NULL)
						{
							newPatSeq2 = (char*) MEM_alloc(3*sizeof(char));
							tc_strcpy(newPatSeq2,"99");
						}

						printf("\nPartnumberList1[0]:%s",partarr);fflush(stdout);
						printf("\nnewPatRev2:%s",newPatRev2);fflush(stdout);
						printf("\nnewPatSeq2:%s",newPatSeq2);fflush(stdout);


						tc_strcpy(inputline,partarr);
						tc_strcat(inputline,"_");
						tc_strcat(inputline,newPatRev2);
						tc_strcat(inputline,"_");
						tc_strcat(inputline,newPatSeq2);
						
						tc_strcpy(PartSubDirectoryNameFinal,"");
						Create_SupplierDirCC(&item_rev_tags,PartSubDirectoryNameFinal,SubDirectoryName,PartSubDirectoryName,timestamp,SupplierDataDir,usernameDir,VendorName);
						printf("\nDownloadSupplierData_Func Path for file download:%s",PartSubDirectoryNameFinal);fflush(stdout);
						printf("\nDownloadSupplierData_Func Path for file SubDirectoryName download:%s",SubDirectoryName);fflush(stdout);
						printf("\nDownloadSupplierData_Func Path for file PartSubDirectoryName download:%s",PartSubDirectoryName);fflush(stdout);
						printf("\nDownloadSupplierData_Func Path for file SupplierDataDir download:%s",SupplierDataDir);fflush(stdout);
						printf("\nDownloadSupplierData_Func Path for file usernameDir download:%s",usernameDir);fflush(stdout);

						ExpandRecursively_ActHndlrCC(	
														cnfgContext,
														inputline,
														&item_rev_tags,
														&objTag,
														DownLoadType,
														PartSubDirectoryNameFinal,
														SubDirectoryName,
														PartSubDirectoryName,
														timestamp,
														SupplierDataDir,
														usernameDir,
														VendorName,
														cadTyp_num_val,
														DownloadFormatList
													);

						

						callt5DataDownLoadMechCC(&objTag,&fp,&i,DwnloadFrmtSelctStr);
						
						printf("\nSupplierDataShell:%s",SupplierDataShell);fflush(stdout);
						printf("\nfInputFileName:%s",fInputFileName);fflush(stdout);
						
						tc_strcpy(CommandLine,SupplierDataShell);
						tc_strcat(CommandLine,"/t5DataDownLoadMech.sh");
						tc_strcat(CommandLine," ");
						tc_strcat(CommandLine,fInputFileName);
						printf("\nCommandLine:%s",CommandLine);fflush(stdout);
						system(CommandLine);
						printf("\nSystem command executed");fflush(stdout);

						callJtFileShellCC(VendorName,inputline,SupplierDataDir,usernameDir,timestamp);
					}
					else
					{
						printf("\nc Query Supplier Part Number:%s not found in tcua",PartnumberList[i]);fflush(stdout);
					}
				}
			}
		}
	}
	if(fp!=NULL)
	{
		fclose(fp);
	}
	return 0;
}
/*int DownloadSupplierData_register_action_handlers()
{
	int retcode = ITK_ok;

	if ( retcode == ITK_ok )
	{
		retcode = EPM_register_action_handler ("DownloadSupplierData", "DownloadSupplierData",(EPM_action_handler_t)DownloadSupplierData_Func );
		printf("\nafter registerring handler EPM_register_action_handler");fflush(stdout);
	}
	return retcode;
}

extern int DownloadSupplierData_action(int *decision, va_list args)
{
	int status = ITK_ok;
	*decision=ALL_CUSTOMIZATIONS;	

	printf("\n********Before DownloadSupplierData_register_action_handlers ");fflush(stdout);
	status = DownloadSupplierData_register_action_handlers();
	printf("\nAfter DownloadSupplierData_register_action_handlers");fflush(stdout);
	return status;
}

extern DLLAPI int DownloadSupplierData_register_callbacks ()
{
	int ifail    = ITK_ok;
	printf("\n********Before calling CUSTOM_EXIT_ftn_t..DownloadSupplierData_action");fflush(stdout);
    ifail=CUSTOM_register_exit("DownloadSupplierData","USER_init_module",(CUSTOM_EXIT_ftn_t)DownloadSupplierData_action);
	printf("\n********After calling CUSTOM_EXIT_ftn_t..DownloadSupplierData_action");fflush(stdout);
	
    return ( ITK_ok );
}*/
extern int ITK_user_main (int argc, char ** argv )
{
	
	char* UserName		= NULL;
	char* Password		= NULL;
	char* InpPackage	= NULL;

	//ITK_CALL(ITK_init_module("loader" ,"loader7","dba")) ;
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_auto_login( ));
    ITK_CALL(ITK_set_journalling( TRUE ));

	//UserName = ITK_ask_cli_argument("-u=");
	//Password = ITK_ask_cli_argument("-p=");
	InpPackage = ITK_ask_cli_argument("-d=");
	usernameDir = ITK_ask_cli_argument("-h=");

	printf("\nInput Package Number : %s ",InpPackage); fflush(stdout);
	printf("\nSession User : %s ",usernameDir); fflush(stdout);

	DownloadSupplierData_Func(InpPackage);

	return ITK_ok;
}