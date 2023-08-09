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
#include <qry/qry.h>
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
#include <bom/bom.h>
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
#include <tc/tc.h>
#include <pie/pie.h>
#include <bom/bom.h>
#include <tccore/aom_prop.h>
#include <dirent.h>

int z;


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

void setAddStr(int *count,char ***strset,char* str)
{
	*count=*count+1;
	//printf("\n setAddStr1 %d",*count);fflush(stdout);
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
	//printf("\n Added in Set ===%s",(*strset)[*count-1]);fflush(stdout);
}

char *ltrim(char *s) 
{     
    while(isspace(*s)) s++;     
    return s; 
}  

char *rtrim(char *s) 
{     
    char* back;
    int len = strlen(s);

    if(len == 0)
        return(s); 

    back = s + len;     
    while(isspace(*--back));     
    *(back+1) = '\0';     
    return s; 
}  

char *trim(char *s) 
{     
    return rtrim(ltrim(s));  
}

 int queryCountryObject(char* info1 ,int *n_cntrObj)
{
	int ifail = ITK_ok;
	tag_t* cntrObj = NULL;

	printf("\n\n Inside queryCountryObject......");fflush(stdout);

	tag_t        query           = NULLTAG;

	char
	*qry_entries[] = { "SYSCD","SUBSYSCD","Information-1"},
	*qry_values[]  = {"DMLScopeDocument","t5_PlcCode",info1};

	ITK_CALL(QRY_find("Control Objects...", &query));
	printf("\n query found");fflush(stdout);
	ITK_CALL(QRY_execute(query,3, qry_entries, qry_values, n_cntrObj, &cntrObj));
	printf("\n query executed");fflush(stdout);

	printf("\n queryCountryObject ==[%d] \n",*n_cntrObj);fflush(stdout);
	

	return ifail;
} 

void setFindStr(char **view_list,int count,char *str,int *found)
{

	//printf("\n **setFindStr");fflush(stdout);
	int k=0;
	*found=0;
	for(k=0;k<count;k++)
	{

		//printf("\n[%d]tc_strcmp(%s,%s)",k,view_list[k],str);fflush(stdout);
		if(tc_strstr(view_list[k],str)!= 0)
		{
			*found=1;
			break;

		}

	}
	//printf("\n **setFindStr found= %d",*found);fflush(stdout);


}

//main function
int ITK_user_main( int argc , char* argv[] )
{
	DIR *dr;
	struct dirent *en;
	
	int 	totalFiles 				= 0;
	int 	totalLines 				= 0;
	int 	nCntrlObj 				= 0;
	
	char* 	path					= NULL;
	char 	**setOfFolderFile 		= NULL;
	char 	**setOfLines 			= NULL;
	char 	**qryEntry 				= (char **) MEM_alloc(2 * sizeof(char *));
	char 	**qryValue 				= (char **) MEM_alloc(2 * sizeof(char *));
	
	tag_t 	query 					= NULLTAG;
	tag_t* 	cntrObj 				= NULL;
	//char* fileName = ITK_ask_cli_argument("-f=");
	
	
	path	=(char *) MEM_alloc(500);
	
	ITK_CALL(ITK_auto_login());
	
	printf("\n\n\n *************** Welcome to attachDmlScopeDoc by Pratik ***************");fflush(stdout);
	
	qryEntry[0]="SYSCD";
	qryEntry[1]="SUBSYSCD";
	
	qryValue[0]="DMLScopeDocument";
	qryValue[1]="FolderPath";

	ITK_CALL(QRY_find("Control Objects...", &query));
	printf("\n query found");fflush(stdout);
	ITK_CALL(QRY_execute(query,2, qryEntry, qryValue, &nCntrlObj, &cntrObj));
	printf("\n query executed");fflush(stdout);
	
	printf("\n nCntrlObj ==========> %d \n",nCntrlObj);fflush(stdout);
	
	if(nCntrlObj>0)
	{
		char* condLive = NULL;
		
		
		ITK_CALL(AOM_ask_value_string(cntrObj[0], "t5_Userinfo1", &condLive));
		printf("\n Condition Live: %s\n",condLive);fflush(stdout);
		
		
		
		if(tc_strcmp(condLive,"TRUE")==0)
		{
			FILE* fp = NULL;
			char* folderPath = NULL;
			
			int i = 0;
			
			ITK_CALL(AOM_ask_value_string(cntrObj[0], "t5_Userinfo2", &folderPath));
			printf("\n folderPath: %s\n",folderPath);fflush(stdout);
			
			tc_strcpy(path,"");
			tc_strcpy(path,folderPath);
			
			dr = opendir(folderPath);	//open all or present directory
			
			if (dr) 
			{
				while ((en = readdir(dr)) != NULL) 
				{
					char* fileName = NULL;
					
					fileName =  en->d_name;
					printf(" %s\n",fileName); //print all directory name
					setAddStr(&totalFiles,&setOfFolderFile,fileName);
				}
			
				closedir(dr); //close all directory
			}			
			printf("\n totalFiles ==========> %d",totalFiles);fflush(stdout);
			
			
			
			
			for(i=0;i<totalFiles;i++)
			{
				char* folderFileName = NULL;
				char* fullPathName = NULL;
				tag_t Newdataset = NULLTAG;
				
				fullPathName=MEM_alloc(1000);
				
				folderFileName = setOfFolderFile[i];
				
				tc_strcpy(fullPathName,"");
				tc_strcpy(fullPathName,path);
				tc_strcat(fullPathName,"/");
				
				if(tc_strstr(folderFileName,"DSAS_")!=NULL)
				{
						char* dsas = NULL;
						char* ercDML = NULL;
						char* ercDML1 = NULL;
						char* folderFileNameDup= NULL;
						char 	**ercDmlEntry 				= (char **) MEM_alloc(2 * sizeof(char *));
						char 	**ercDmlValue 				= (char **) MEM_alloc(2 * sizeof(char *));
						char	**keys						= (char **) MEM_alloc(1 * sizeof(char *));//={"creation_date"};
						
						int ercDmlCount = 0;
						int num_to_sort = 0;
						int orders[]={1};
						
						tag_t ercQuery = NULLTAG;
						tag_t* so_ercDml = NULL;

						dsas=(char *) MEM_alloc(500);
						ercDML=(char *) MEM_alloc(500);
						ercDML1=(char *) MEM_alloc(500);
						folderFileNameDup=(char *) MEM_alloc(500);
						
						printf("\n inside DSAS");fflush(stdout);
						
						
						dsas 	= tc_strtok(folderFileName,"_");
						ercDML1 = tc_strtok(NULL,"_");
						ercDML 	= tc_strtok(ercDML1,".");
						
						printf("\n %d. ercDML number =========> %s",i +1, ercDML);fflush(stdout);
						
						tc_strcpy(folderFileNameDup,"");
						tc_strcpy(folderFileNameDup,dsas);
						tc_strcat(folderFileNameDup,"_");
						tc_strcat(folderFileNameDup,ercDML);
						
						printf("\n folderFileNameDup =========> %s",folderFileNameDup);fflush(stdout);
						
						tc_strcat(fullPathName,folderFileNameDup);
						tc_strcat(fullPathName,".pdf");
						
						//ercDmlEntry[1]="Type";
						//ercDmlEntry[0]="Name";
						ercDmlEntry[0]="ID";
						
						//ercDmlValue[1]="ERC DML";
						ercDmlValue[0]= ercDML;
						
						keys[0] = "creation_date";
						

						//ITK_CALL(QRY_find("General...", &ercQuery));
						ITK_CALL(QRY_find("ERC DML", &ercQuery));
						printf("\n query found");fflush(stdout);
						//ITK_CALL(QRY_execute(ercQuery,1, ercDmlEntry, ercDmlValue, &ercDmlCount, &so_ercDml));
						
						ITK_CALL(QRY_execute_with_sort(ercQuery, 1, ercDmlEntry, ercDmlValue,num_to_sort,keys,orders, &ercDmlCount, &so_ercDml));
						printf("\n query executed");fflush(stdout);
						
						printf("\n ercDmlCount ==========> %d ",ercDmlCount);fflush(stdout);
						
						if(ercDmlCount>0)
						{
							char* revId = NULL;
							char* objType = NULL;
							
							tag_t relation = NULLTAG;
							ITK_CALL(AOM_ask_value_string(so_ercDml[0], "item_revision_id", &revId));
							printf("\n revId: %s",revId);fflush(stdout);
							
							ITK_CALL(AOM_ask_value_string(so_ercDml[0], "object_type", &objType));
							printf("\n objType: %s",objType);fflush(stdout);
							
							if(tc_strcmp(objType,"ChangeRequestRevision")==0)
							{
								int relCount = 0;
								int caseNumber = 0;
								tag_t* solObj = NULL;
								
								ITK_CALL(GRM_find_relation_type("IMAN_reference",&relation));
								ITK_CALL(GRM_list_secondary_objects_only(so_ercDml[0],relation,&relCount,&solObj));
								
								printf("\n relCount =========> %d",relCount);
								if(relCount>0)
								{
									int loop = 0;
									
									for(loop=0; loop<relCount; loop++)
									{
										char* pdfName = NULL;
									
										ITK_CALL(AOM_ask_value_string(solObj[loop],"object_name",&pdfName));
										
										printf("\n %d. pdfName ========> %s, %s",loop+1,pdfName,folderFileNameDup);fflush(stdout);
										
										if(tc_strcmp(pdfName,folderFileNameDup)==0)
										{
											if(Newdataset != NULLTAG)
											{
												Newdataset = NULLTAG;
											}
											
											Newdataset = solObj[loop];
											
											caseNumber = 1;
											break;
										}
									}
									
								}
								
								printf("\n caseNumber =========> %d",caseNumber);fflush(stdout);
								
								tag_t Newdataset_t = NULLTAG;

								if(caseNumber==0)
								{
									tag_t PdfType = NULLTAG;
									
									if(Newdataset != NULLTAG)
									{
										Newdataset = NULLTAG;
									}
									
									printf("\n pdf is not attached");fflush(stdout);
									
									ITK_CALL(AE_find_datasettype2("PDF",&PdfType));
									if(PdfType != NULLTAG)
									{
										printf("\n pdf dataset type found");fflush(stdout);
									}
									else
									{
										printf("\n pdf dataset type not found");fflush(stdout);
									}
									ITK_CALL(AE_create_dataset_with_id(PdfType,folderFileNameDup,folderFileNameDup, NULL, NULL, &Newdataset));
									ITK_CALL(AE_save_myself(Newdataset));
									
									//AOM_save(Newdataset);
									
									if(Newdataset != NULLTAG)
									{
										tag_t datasetRel = NULLTAG;
										tag_t PdfLatest = NULLTAG;
										
										tag_t *datasetObj = NULL;
										
										int count = 0;
										char* newPdfName = NULL;
										
										printf("\n dataset is created");fflush(stdout);
										
										ITK_CALL(GRM_create_relation(so_ercDml[0],Newdataset,relation,NULLTAG,&datasetRel));
										printf("\n GRM_create_relation");fflush(stdout);
										ITK_CALL(AOM_load(datasetRel));
										printf("\n AOM_load");fflush(stdout);
										ITK_CALL(GRM_save_relation(datasetRel));
										printf("\n GRM_save_relation");fflush(stdout);
										
									}
									else
									{
										printf("\n dataset is not created");fflush(stdout);
									}									
										
									
								}
								else
								{
									
									printf("\n pdf already attached");fflush(stdout);
								}
								
								ITK_CALL(AE_ask_dataset_latest_rev(Newdataset,&Newdataset_t));
								printf("\n AE_ask_dataset_latest_rev");fflush(stdout);							
								ITK_CALL(AOM_refresh(Newdataset_t,TRUE));
								printf("\n AOM_refresh");fflush(stdout);
								ITK_CALL(AOM_lock(Newdataset_t));
								printf("\n AOM_lock");fflush(stdout);
								
								if(caseNumber != 0)
								{
									ITK_CALL(AE_remove_dataset_named_ref2(Newdataset_t,"PDF_Reference"));
									printf("\n AE_remove_dataset_named_ref2");fflush(stdout);
								}
					
								ITK_CALL(AE_import_named_ref(Newdataset_t,"PDF_Reference",fullPathName,NULL,SS_BINARY));
								printf("\n AE_import_named_ref");fflush(stdout);
								ITK_CALL(AE_save_myself(Newdataset_t));
								printf("\n AE_save_myself");fflush(stdout);
								ITK_CALL(AOM_unlock(Newdataset_t));
								printf("\n AOM_unlock");fflush(stdout);
								remove(fullPathName);
							}
						}
					
				}
				printf("\n\n");fflush(stdout);				
			}			
		}
		else
		{
			printf("\n function is not live.");fflush(stdout);
		}
	}
	
	
	
	
	
	return 0;
}