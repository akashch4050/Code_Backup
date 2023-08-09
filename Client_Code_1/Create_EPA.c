/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  Author		 :   Sagar Baviskar
*  Module		 :   Create DML and Task then Qry Part and Attach to Task
*  Code			 :   Create_DML_Task.c
*  Created on	 :   July 21 2020
*
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes
***************************************************************************/


#include <ae/dataset.h>
#include <ae/dataset_msg.h>
#include <ai/sample_err.h>
#include <bom/bom.h>
#include <ctype.h>
#include <epm/cr_effectivity.h>
#include <epm/epm.h>
#include <epm/releasestatus.h>
#include <fclasses/tc_string.h>
#include <ict/ict_userservice.h>
#include <itk/mem.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <pom/pom/pom.h>
#include <ps/ps.h>
#include <ps/ps_errors.h>
#include <rdv/arch.h>
#include <res/res_itk.h>
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
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/custom.h>
#include <tccore/grm.h>
#include <tccore/grm_msg.h>
#include <tccore/grmtype.h>
#include <tccore/iman_msg.h>
#include <tccore/imantype.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tccore/item_msg.h>
#include <tccore/tctype.h>
#include <tccore/uom.h>
#include <tccore/workspaceobject.h>
#include <tcinit/tcinit.h>
#include <textsrv/textserver.h>
#include <time.h>
#include <user_exits/user_exits.h>

#define NULLDATE   (POM_null_date()) 
#define ITK_Prjerr (EMH_USER_error_base + 8)
#define ITK_err 919002
#define ITK_errStore1 (EMH_USER_error_base + 5)
#define NUM_ENTRIES 1
#define TE_MAXLINELEN  128
#define _CRT_SECURE_NO_DEPRECATE


#define Debug TRUE

#define ITK_CALL(X) 							\
		if(Debug)								\
		{										\
			printf(#X);							\
		}										\
		fflush(NULL);							\
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
			printf("\t%3d error(s)\n", n_ifails);							\
			for( index=0; index<n_ifails; index++)							\
			{																\
				printf("\tError #%d, %s\n", ifails[index], texts[index]);	\
			}																\
			return status;													\
		}																	\
		else									\
		{										\
			if(Debug)							\
			printf("\tSUCCESS\n");				\
		}										\

int tm_removeRlzDate(tag_t objrev)
{
	int status;
	char	*class_name	=	NULL;
	tag_t DtRlzd = NULLTAG;
	
	printf("\nInside tm_removeRlzDate");fflush(stdout);
	ITK_CALL(AOM_ask_value_string(objrev,"object_name",&class_name));
	printf("\nclass_name : %s",class_name);fflush(stdout);

	ITK_CALL(AOM_lock(objrev));//Design Revision, date_released
	ITK_CALL(POM_attr_id_of_attr("date_released","T5_EPARevision",&DtRlzd));
	ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
	ITK_CALL(POM_set_attr_date(1,&objrev,DtRlzd,NULLDATE));
	ITK_CALL(AOM_save(objrev));
	ITK_CALL(AOM_refresh(objrev,TRUE));
	ITK_CALL(AOM_save(objrev));
	ITK_CALL(AOM_unlock(objrev));

	return status;
}

int tm_updaterlzdate(tag_t objrev, char *date)
{
	int status;

	char	*class_name	=	NULL;

	tag_t RlzStat_AttrID = NULLTAG;
	date_t	EPACloserDt_date;

	printf("\nInside tm_removeRlzDate : %s",date);fflush(stdout);
	ITK_CALL(AOM_ask_value_string(objrev,"object_name",&class_name));
	printf("\nclass_name : %s",class_name);fflush(stdout);
	

	ITK_CALL(AOM_lock(objrev));
	ITK_CALL(ITK_string_to_date(date, &EPACloserDt_date));
	ITK_CALL (POM_attr_id_of_attr("date_released", "T5_EPARevision", &RlzStat_AttrID));
	ITK_CALL (POM_set_attr_date(1, &objrev, RlzStat_AttrID, EPACloserDt_date));
	ITK_CALL (AOM_save(objrev));
	ITK_CALL(AOM_refresh(objrev,0));
	ITK_CALL(AOM_unlock(objrev));

	return status;
}

int AddStatus(tag_t itemrev,char *LCS,char *EPACloserDt)
{
	int status;
	int ss=0;
	int status_count =0;
	int	stdWrkStatusFlg=0;
    int	stdRlzdStatusFlg=0;
	char *class_name = NULL;
	tag_t* status_list = NULLTAG;
	tag_t release_status = NULLTAG;
	tag_t ClosureDtAttrId = NULLTAG;
	tag_t DtRlzd = NULLTAG;
	
	logical retain_released_date=false;
	date_t EPACloserDt_date;
	
	printf("\n Inside AddStatus function... \n");fflush(stdout);
	printf("\n LCS : %s\n",LCS);fflush(stdout);
	ITK_CALL( WSOM_ask_release_status_list(itemrev,&status_count,&status_list));
	printf("\n status_count: %d\n",status_count);fflush(stdout);
	ITK_CALL(AOM_lock(itemrev));
	
	if (status_count>0)
	{
		for(ss=0; ss<status_count ; ss++)
		{
			ITK_CALL(AOM_ask_value_string(status_list[ss],"object_name",&class_name));
			printf("\n class_name: %s\n",class_name);fflush(stdout);
			
			if(tc_strcmp(class_name,"T5_LcsSTDWrkg")==0)
			{
				stdWrkStatusFlg ++;
			}
			else if(tc_strcmp(class_name,"T5_LcsStdRlzd")==0)
			{
				stdRlzdStatusFlg ++;
			}			
		}

		if (tc_strcmp(LCS,"LcsSTDWrkg")==0)
		{
			if (stdWrkStatusFlg == 0)
			{
				ITK_CALL(CR_create_release_status("T5_LcsSTDWrkg",&release_status));
				ITK_CALL(EPM_add_release_status(release_status,1,&itemrev,retain_released_date));
				ITK_CALL(AOM_refresh(release_status,TRUE));
				ITK_CALL(AOM_lock(itemrev));
				ITK_CALL(AOM_save(itemrev));
				printf("\n Release Status added Successfully(T5_LcsSTDWrkg)...\n");fflush(stdout);
			}
		}
		if (tc_strcmp(LCS,"LcsSTDRlzd")==0)
		{
			if (stdRlzdStatusFlg == 0)
			{
				ITK_CALL(CR_create_release_status("T5_LcsStdRlzd",&release_status));
				ITK_CALL(EPM_add_release_status(release_status,1,&itemrev,retain_released_date));
				ITK_CALL(AOM_refresh(release_status,TRUE));
				ITK_CALL(AOM_lock(itemrev));
				ITK_CALL(AOM_save(itemrev));
				printf("\n Release Status added Successfully(T5_LcsStdRlzd)...\n");fflush(stdout);

//				//
//				ITK_CALL(ITK_string_to_date(EPACloserDt, &EPACloserDt_date));
//				ITK_CALL(AOM_lock(release_status));//Design Revision, date_released Design_0_Revision_alt
//				ITK_CALL(POM_attr_id_of_attr("date_released","ReleaseStatus",&DtRlzd));
//				ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
//				ITK_CALL(POM_set_attr_date(1,&release_status,DtRlzd,EPACloserDt_date));
//				ITK_CALL(AOM_save(release_status));
//				ITK_CALL(AOM_refresh(release_status,TRUE));
//				ITK_CALL(AOM_save(release_status));
//				ITK_CALL(AOM_unlock(release_status));
//				//

				if (tc_strcmp(EPACloserDt," ")!=0)
				{
					ITK_CALL(ITK_string_to_date(EPACloserDt, &EPACloserDt_date));
					ITK_CALL(POM_attr_id_of_attr("date_released","T5_EPARevision",&ClosureDtAttrId));
					ITK_CALL(AOM_refresh(itemrev,TRUE));
					ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
					ITK_CALL(POM_set_attr_date(1,&itemrev,ClosureDtAttrId,EPACloserDt_date));
					printf("\n Date Release added Successfully...\n");fflush(stdout);
				}	
			}
		}
	}
	else
	{
		if (tc_strcmp(LCS,"LcsSTDWrkg")==0)
		{			
				ITK_CALL(CR_create_release_status("T5_LcsSTDWrkg",&release_status));
				ITK_CALL(EPM_add_release_status(release_status,1,&itemrev,retain_released_date));
				ITK_CALL(AOM_refresh(release_status,TRUE));
				ITK_CALL(AOM_lock(itemrev));
				ITK_CALL(AOM_save(itemrev));
				printf("\n Release Status added Successfully(T5_LcsSTDWrkg)...\n");fflush(stdout);			
		}
		if (tc_strcmp(LCS,"LcsSTDRlzd")==0)
		{			
				ITK_CALL(CR_create_release_status("T5_LcsStdRlzd",&release_status));
				ITK_CALL(EPM_add_release_status(release_status,1,&itemrev,retain_released_date));
				ITK_CALL(AOM_refresh(release_status,TRUE));
				ITK_CALL(AOM_lock(itemrev));
				ITK_CALL(AOM_save(itemrev));
				printf("\n Release Status added Successfully(T5_LcsStdRlzd)...\n");fflush(stdout);
				
				//
				ITK_CALL(ITK_string_to_date(EPACloserDt, &EPACloserDt_date));
				ITK_CALL(AOM_lock(release_status));//Design Revision, date_released Design_0_Revision_alt
				ITK_CALL(POM_attr_id_of_attr("date_released","ReleaseStatus",&DtRlzd));
				ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
				ITK_CALL(POM_set_attr_date(1,&release_status,DtRlzd,EPACloserDt_date));
				ITK_CALL(AOM_save(release_status));
				ITK_CALL(AOM_refresh(release_status,TRUE));
				ITK_CALL(AOM_save(release_status));
				ITK_CALL(AOM_unlock(release_status));
				//
				
				tm_removeRlzDate(itemrev);
				tm_updaterlzdate(itemrev,EPACloserDt);
//				if (tc_strcmp(EPACloserDt," ")!=0)
//				{
//					ITK_CALL(ITK_string_to_date(EPACloserDt, &EPACloserDt_date));
//					if (EPACloserDt_date!=NULLDATE)
//					{
//					}
//					else
//					{
//						printf("\nEPACloserDt_date is nulltag");fflush(stdout);
//					}
//					ITK_CALL(AOM_refresh(itemrev,TRUE));
//					ITK_CALL(POM_attr_id_of_attr("date_released","T5_EPARevision",&ClosureDtAttrId));					
//					ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
//					ITK_CALL(POM_set_attr_date(1,&itemrev,ClosureDtAttrId,NULLDATE));
//
//					ITK_CALL(POM_attr_id_of_attr("date_released","T5_EPARevision",&ClosureDtAttrId));					
//					ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
//					ITK_CALL(POM_set_attr_date(1,&itemrev,ClosureDtAttrId,EPACloserDt_date));
//					printf("\n Date Release added Successfully...\n");fflush(stdout);
//				}					
		}
	}
	ITK_CALL(AOM_unlock(itemrev))
	
	return status;
}



extern int ITK_user_main (int argc, char ** argv )
{    
	
	int status;
	int	ercdml_tags_found	= 0;
	char *inputfile=NULL;	
	char *EPAName=NULL;	
	char *EPATaskName=NULL;
	char *EPALCS=NULL;
	char *EPACloserDt=NULL;
	char *Proj_code=NULL;
	char *PartNo=NULL;
	char *PartNoRev=NULL;
	char *PartNoSeq=NULL;	
	char *inputline=NULL;	
	char *item_id=NULL;
	char *DML_no=NULL;
	char *Proj_Code=NULL;
	char *itemRevSeq=NULL;
	char* class_name=NULL;
	tag_t class_id = NULLTAG;
	logical	log1;
	

	FILE *fp=NULL;
	FILE *fperror=NULL;
	FILE *fpexception=NULL;
	
	
	const char		*qry_entries[1];
	const char		*qry_values[1];
	const char		*apl_dml_task_qry_entries[1];
	const char		*apl_dml_task_qry_values[1];
	
	tag_t	EpaTag	= NULLTAG;
	tag_t	EpaTagRev = NULLTAG;
	tag_t	EpaTaskTag	= NULLTAG;
	tag_t   EpaTaskTagRev = NULLTAG;
	tag_t	relation = NULLTAG;
	int  number_found=0;
	int epa_task_number_found = 0;	
	tag_t *epa_task_tags=NULLTAG;
	
	tag_t	relation_type= NULLTAG;
    
	WSO_search_criteria_t  	criteria;
	tag_t *list_of_WSO_tags=NULLTAG;	
	tag_t	tsk_part_sol_rel_type= NULLTAG;

	char epa_task_rev_id[TCTYPE_name_size_c+1]="NR";
	const char *attrs[1];
	const char *values[1];
	const char *epa_task_qry_entries[1];
	const char *epa_task_qry_values[1];
	tag_t *tags_found = NULL;
	int n_tags_found=0;
	tag_t item=NULLTAG;
	tag_t rev=NULLTAG;
	tag_t         Fndrelation = NULLTAG;
	tag_t         FndDMLTaskrelation = NULLTAG;
	tag_t         FndAPLERCDMLrelation = NULLTAG;
	tag_t			epa_tsk_part__rel	= NULLTAG;
	


	inputfile = ITK_ask_cli_argument("-i=");
	
	ITK_CALL(ITK_auto_login( ));  
    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Auto login .......");fflush(stdout);

	
	fp=fopen(inputfile,"r");
	fperror=fopen("error_APLDMLCreation.log","a+");
	fpexception=fopen("Exception_APLDMLCreation.log","a+");
	if(fp!=NULL)
	{
		inputline=(char *) MEM_alloc(5000);
		while(fgets(inputline,5000,fp)!=NULL)
		{
			printf("\n File rease.");fflush(stdout);
			fputs(inputline,stdout);
			
			//PM29694700^PM29694700_06PM165036_APLC^LcsSTDWrkg^10-Jul-2019 00:00^5724^544261000101^G^2^
			//EPA lCS,date_released,Task LCS,project code,
			//desc= TCE Migrated EPA

			EPAName=NULL;
			EPATaskName=NULL;
			EPALCS=NULL;
			EPACloserDt=NULL;
			Proj_code=NULL;
			PartNo=NULL;
			PartNoRev=NULL;
			PartNoSeq=NULL;
			
			EPAName=strtok(inputline,"^");				
			EPATaskName=strtok(NULL,"^");
			EPALCS=strtok(NULL,"^");
			EPACloserDt=strtok(NULL,"^");
			Proj_code=strtok(NULL,"^");
			PartNo=strtok(NULL,"^");			
			PartNoRev=strtok(NULL,"^");
			PartNoSeq=strtok(NULL,"^");

			//Input data from file
			printf("\n Given input data from file is:\n EPAName :[%s]",EPAName);fflush(stdout);			
			printf("\n EPATaskName :[%s]",EPATaskName);fflush(stdout);			
			printf("\n EPALCS :[%s]",EPALCS);fflush(stdout);
			printf("\n EPACloserDt :[%s]",EPACloserDt);fflush(stdout);
			printf("\n Proj_code :[%s]",Proj_code);fflush(stdout);
			printf("\n PartNo :[%s]",PartNo);fflush(stdout);
			printf("\n PartNoRev :[%s]",PartNoRev);fflush(stdout);
			printf("\n PartNoSeq :[%s]",PartNoSeq);fflush(stdout);
			
			
			EpaTag = NULLTAG;
			EpaTagRev = NULLTAG;
			EpaTaskTag = NULLTAG;
			EpaTaskTagRev = NULLTAG;
			relation = NULLTAG;
			Fndrelation = NULLTAG;
			epa_tsk_part__rel = NULLTAG;
			rev = NULLTAG;
			
			//Find EPA
			attrs[0] ="item_id";
			values[0] = (char *)EPAName;
			//Querying with item id
			n_tags_found =0;
			tags_found = NULLTAG;
			ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
			printf("n_tags_found  --->[%d]\n",n_tags_found);fflush(stdout);
			
			if(n_tags_found==0)
			{				
				EpaTag = NULLTAG;
				EpaTagRev = NULLTAG;
				EpaTaskTag = NULLTAG;
				EpaTaskTagRev = NULLTAG;
				
				printf("\n EPAName is --->%s \n",EPAName);fflush(stdout);
    			ITK_CALL(ITEM_create_item(EPAName,EPAName,"T5_EPA","NR",&EpaTag,&EpaTagRev));
				
				ITK_CALL(AOM_save(EpaTag));
	 			ITK_CALL(AOM_save(EpaTagRev));	

				/*ITK_CALL(POM_class_of_instance(EpaTagRev,&class_id)); 
				ITK_CALL(POM_name_of_class(class_id,&class_name));
				printf("\n Class Name is : %s\n",class_name);fflush(stdout);
				//ITK_CALL(POM_unload_instances(1,&PartRev_tag));
				ITK_CALL(POM_load_instances(1,&EpaTagRev,class_id,1));
				ITK_CALL(POM_is_loaded(EpaTagRev,&log1));
				if(log1 == 1)
				{
					 printf(" Load Success\n " );
				}
				else
				printf(" Load Failure\n"  );*/			
	
				
				ITK_CALL(AOM_set_value_string(EpaTagRev,"object_desc","TCE Migrated EPA"));
			
				if (tc_strcmp(Proj_code," ") != 0)
				{
					ITK_CALL(AOM_set_value_string(EpaTagRev,"t5_cprojectcode",Proj_code));
				}
				
				AddStatus(EpaTagRev,EPALCS,EPACloserDt); //Adding Status 

                if(EpaTagRev != NULLTAG)
				{				
					printf("EPA Revision Created [%s]\n",EPAName);
					ITK_CALL(ITEM_create_item(EPATaskName,EPATaskName,"T5_EPATask","NR",&EpaTaskTag,&EpaTaskTagRev));
						
					ITK_CALL(AOM_save(EpaTaskTag));
	 				ITK_CALL(AOM_save(EpaTaskTagRev));
					
					AddStatus(EpaTaskTagRev,EPALCS,EPACloserDt); //Adding Status
					
					if(EpaTaskTagRev != NULLTAG)
					{
					    printf("EPA Task Created [%s]\n",EPATaskName);

						ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation", &relation_type));
						ITK_CALL(GRM_create_relation(EpaTagRev, EpaTaskTagRev, relation_type, NULLTAG, &relation););
						ITK_CALL(GRM_save_relation(relation));
						printf("\n Relation between EPA Rev & EPA Task Rev Created \n");

						attrs[0] ="item_id";
						values[0] = (char *)PartNo;

						printf("Part seraching:[%s],[%s],[%s],[%s],[%s]\n",PartNo,PartNoRev,PartNoSeq,attrs[0],values[0]);

						ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
						printf("\n n_tags_found %d...\n",n_tags_found);fflush(stdout);
						
						if(n_tags_found==0)
						{
							fprintf(fperror,"Part not found  :[%s],[%s],[%s]\n",PartNo,PartNoRev,PartNoSeq);
				
						}
						else
						{
							item = tags_found[0];
							printf("\n Part already exists\n");fflush(stdout);

							itemRevSeq = NULL;
							itemRevSeq=(char *) MEM_alloc(32);
							strcpy(itemRevSeq,PartNoRev);
							strcat(itemRevSeq,";");
							strcat(itemRevSeq,PartNoSeq);

							printf(" Part Rev [%s]\n",itemRevSeq);fflush(stdout);

							ITK_CALL(ITEM_find_revision(item,itemRevSeq,&rev));
							if(rev != NULLTAG)
							{
								ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&tsk_part_sol_rel_type));
								ITK_CALL(GRM_find_relation(EpaTaskTagRev, rev, tsk_part_sol_rel_type ,&Fndrelation));
								
								if(Fndrelation)
								{
									printf("\n\t Relation Already Exist:[%s],[%s],[%s],[%s]\n",EPATaskName,PartNo,PartNoRev,PartNoSeq);fflush(stdout);
									fprintf(fpexception,"\n\t Relation Already Exist:[%s],[%s],[%s],[%s]\n",EPATaskName,PartNo,PartNoRev,PartNoSeq);
								}
								else
								{
									ITK_CALL(GRM_create_relation(EpaTaskTagRev, rev, tsk_part_sol_rel_type,  NULLTAG, &epa_tsk_part__rel));
									ITK_CALL(GRM_save_relation(epa_tsk_part__rel));
									printf("\n Relation between EPA Task Rev & Part Rev Created \n");
								}							
																		
							}
						}

					}
					
				
				}		
										
			}
			else
			{
				printf("EPA Already Created...Finding EPA Task...\n");fflush(stdout);
				EpaTagRev = NULLTAG;
				EpaTagRev = tags_found[0];

				epa_task_qry_entries[0] ="item_id";
				epa_task_qry_values[0] = EPATaskName;
				ITKCALL(ITEM_find_item_revs_by_key_attributes(1,epa_task_qry_entries,epa_task_qry_values,epa_task_rev_id,&epa_task_number_found,&epa_task_tags));
				
				printf("\n No of EPA Task Found = %d\n",epa_task_number_found);fflush(stdout);

				if (epa_task_number_found == 0)
				{
					printf("EPA Task Not Found... Creating Task...\n");fflush(stdout);

					ITK_CALL(ITEM_create_item(EPATaskName,EPATaskName,"T5_EPATask","NR",&EpaTaskTag,&EpaTaskTagRev));
						
					ITK_CALL(AOM_save(EpaTaskTag));
	 				ITK_CALL(AOM_save(EpaTaskTagRev));	
					
					AddStatus(EpaTaskTagRev,EPALCS,EPACloserDt); //Adding Status

					if(EpaTaskTagRev != NULLTAG)
					{
					    printf("EPA Task Created [%s]\n",EPATaskName);

						ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation", &relation_type));
						ITK_CALL(GRM_create_relation(EpaTagRev, EpaTaskTagRev, relation_type, NULLTAG, &relation););
						ITK_CALL(GRM_save_relation(relation));
						printf("\n Relation between EPA Rev & EPA Task Rev Created \n");

						attrs[0] ="item_id";
						values[0] = (char *)PartNo;

						printf("Part seraching:[%s],[%s],[%s],[%s],[%s]\n",PartNo,PartNoRev,PartNoSeq,attrs[0],values[0]);

						ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
						printf("\n n_tags_found %d...\n",n_tags_found);fflush(stdout);
						
						if(n_tags_found==0)
						{
							fprintf(fperror,"Part not found  :[%s],[%s],[%s]\n",PartNo,PartNoRev,PartNoSeq);
				
						}
						else
						{
							item = tags_found[0];
							printf("\n Part already exists\n");fflush(stdout);

							itemRevSeq = NULL;
							itemRevSeq=(char *) MEM_alloc(32);
							strcpy(itemRevSeq,PartNoRev);
							strcat(itemRevSeq,";");
							strcat(itemRevSeq,PartNoSeq);

							printf(" Part Rev [%s]\n",itemRevSeq);fflush(stdout);

							ITK_CALL(ITEM_find_revision(item,itemRevSeq,&rev));
							if(rev != NULLTAG)
							{
								ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&tsk_part_sol_rel_type));
								printf(" Part Rev [%s]\n",itemRevSeq);fflush(stdout);
								ITK_CALL(GRM_find_relation(EpaTaskTagRev, rev, tsk_part_sol_rel_type ,&Fndrelation));
								
								if(Fndrelation)
								{
									printf("\n\t Relation Already Exist:[%s],[%s],[%s],[%s]\n",EPATaskName,PartNo,PartNoRev,PartNoSeq);fflush(stdout);
									fprintf(fpexception,"\n\t Relation Already Exist:[%s],[%s],[%s],[%s]\n",EPATaskName,PartNo,PartNoRev,PartNoSeq);
								}
								else
								{
									ITK_CALL(GRM_create_relation(EpaTaskTagRev, rev, tsk_part_sol_rel_type,  NULLTAG, &epa_tsk_part__rel));
									ITK_CALL(GRM_save_relation(epa_tsk_part__rel));
									printf("\n Relation between EPA Task Rev & Part Rev Created \n");
								}							
																		
							}
						}

					}
				}
				else
				{
					EpaTaskTagRev = epa_task_tags[0];
					printf("EPA Task Already Created...Finding Part...\n");fflush(stdout);

					attrs[0] ="item_id";
					values[0] = (char *)PartNo;

					printf("Part seraching:[%s],[%s],[%s],[%s],[%s]\n",PartNo,PartNoRev,PartNoSeq,attrs[0],values[0]);

					ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
					printf("\n n_tags_found %d...\n",n_tags_found);fflush(stdout);
					
					if(n_tags_found==0)
					{
						fprintf(fperror,"Part not found  :[%s],[%s],[%s]\n",PartNo,PartNoRev,PartNoSeq);
				
					}
					else
					{
						item = tags_found[0];
						printf("\n Part already exists\n");fflush(stdout);

						itemRevSeq = NULL;
						itemRevSeq=(char *) MEM_alloc(32);
						strcpy(itemRevSeq,PartNoRev);
						strcat(itemRevSeq,";");
						strcat(itemRevSeq,PartNoSeq);

						printf(" Part Rev [%s]\n",itemRevSeq);fflush(stdout);

						ITK_CALL(ITEM_find_revision(item,itemRevSeq,&rev));
						if(rev != NULLTAG)
						{
							ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&tsk_part_sol_rel_type));
							ITK_CALL(GRM_find_relation(EpaTaskTagRev, rev, tsk_part_sol_rel_type ,&Fndrelation));							
							
							if(Fndrelation)
							{
								printf("\n\t Relation Already Exist:[%s],[%s],[%s],[%s]\n",EPATaskName,PartNo,PartNoRev,PartNoSeq);fflush(stdout);
								fprintf(fpexception,"\n\t Relation Already Exist:[%s],[%s],[%s],[%s]\n",EPATaskName,PartNo,PartNoRev,PartNoSeq);
							}
							else
							{
								ITK_CALL(GRM_create_relation(EpaTaskTagRev, rev, tsk_part_sol_rel_type,  NULLTAG, &epa_tsk_part__rel));
								ITK_CALL(GRM_save_relation(epa_tsk_part__rel));
								printf("\n Relation between EPA Task Rev & Part Rev Created \n");
							}							
																	
						}
					}
				}
			}
			printf("-----------EPA LIST COMPLETED-----------------------------------------\n");fflush(stdout);
		}

		printf("-----------EPA LOADING SUCCESS-----------------------------------------\n");fflush(stdout);
	
		return status;
		ITKCALL(POM_logout(true));
	}
}

// compile Create_EPA.c
// linkitk -o Create_EPA Create_EPA.o
// Create_EPA -u=aplloader -pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba > 2856_Log.log &
// Create_EPA -u=infodba -p=APLinfo2020 -g=dba -i=EPAData.txt > 5724_Log.log &
// scp tcuaadev@172.22.97.90:/user/tcuaadev/devgroups/sagar/Data_Migration/Create_EPA/Create_EPA .
// Create_EPA -u=aplloader -pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba -i=EPAData.txt > FinalLog.log &