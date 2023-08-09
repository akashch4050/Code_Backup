/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  Author		 :   Deepti Meshram
*  Module		 :   TCUA Desing Rev BOM Uploader
*  Code			 :   t5APLPartBOMCreation.c
*  Created on	 :   March 28, 2018
*
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes
***************************************************************************/

#define ITK_Prjerr (EMH_USER_error_base + 8)
#define ITK_err 919002
#define ITK_errStore1 (EMH_USER_error_base + 5)
#define NUM_ENTRIES 1
#define TE_MAXLINELEN  128
#define _CRT_SECURE_NO_DEPRECATE
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

#define ONE "1-Mtr"
#define TWO "2-Kg"
#define THREE "3-Ltr"
#define FOUR "4-Nos"
#define FIVE "5-Sq.Mtr"
#define SIX "6-Sets"
#define SEVEN "7-Tonne"
#define EIGHT "8-Cu.Mtr"
#define NINE "9-Thsnds"
#define EIGHT "8-Cu.Mtr"

#define ERCREVIEW	"T5_LcsReview"
#define ERCWORKING	"T5_LcsWorking"
#define ERCRELEASED "T5_LcsErcRlzd"
#define APLWORKING  "T5_LcsAPLWrkg"
#define APLRELEASED "T5_LcsAplRlzd"
#define STDWORKING  "T5_LcsSTDWrkg"
#define STDRELEASED "T5_LcsStdRlzd"
#define APLREVIEW	"T5_LcsAplReview"



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

static char* default_empty_to_A(char *s)
{
    return (NULL == s ? s : ('\0' == s[0] ? "A" : s));
}

static int initialise_attribute (char *name,  int *attribute)
{

	int mode;
    int status;

	ITK_CALL(BOM_line_look_up_attribute (name, attribute));
	ITK_CALL(BOM_line_ask_attribute_mode (*attribute, &mode));
	if (mode != BOM_attribute_mode_string)
	{ 
		printf ("Help,  attribute %s has mode %d,  I want a string\n", name, mode);
		exit(0);
	}
	return status;
}



extern int ITK_user_main (int argc, char ** argv )
{    
	
	int status;
	char *inputfile=NULL;
	char *PlantName=NULL;
	char *dmlNo=NULL;
	char *drStatusOld=NULL;
	char *objectString=NULL;
	char *item_id_value=NULL;
	char *current_id_value=NULL;
	char *drStatus=NULL;
	char *designGrp=NULL;
	char *dmlNoLCS=NULL;
	char *dmlAPLRlzdDt=NULL;
	char *dmlSTDSIRlzdDt=NULL;
	char *dmlTaskNo=NULL;
	char *dmlTaskLCS=NULL;
	char *dmlTaskAPLRlzdDt=NULL;
	char *dmlTaskSTDRlzdDt=NULL;
	char *PartNo=NULL;
	char *PartNoRev=NULL;
	char *PartNoSeq=NULL;
	char *dmlNoCpy=NULL;
	char *inputline=NULL;
	char *ercdmlNo=NULL;
	char *apldmlno=NULL;
	char *item_id=NULL;
	char *DML_no=NULL;
	char *Proj_Code=NULL;
	char *erc_dml_name=NULL;
	char *erc_dml_desc=NULL;
	char *erc_dml_rlzType=NULL;
	char *DmlNoRule=NULL;
	char *sPlntNmeDup1=NULL;
	char *itemRevSeq=NULL;
	char *ercdmlNo1=NULL;

	char *aplAMDmlCreatorDup=NULL;
	char *apldmlNoCpy=NULL;
	//char *Proj_Code=NULL;
	char *aplAMDmlDriverVCDup=NULL;
	//char *erc_dml_desc=NULL;
	char *aplAMLastUpDtDup=NULL;
	char *aplAMLastUpByDup=NULL;
	char *aplDmlNoEcnTypeDup=NULL;
	char *aplDmlNoDRStatusDup=NULL;

	char  dml_type[TCTYPE_name_size_c+1];
	char **DesignGroupList;
	WSO_search_criteria_t  	criteria_erc;

	FILE *fp=NULL;
	FILE *fperror=NULL;
	FILE *fpexception=NULL;
	const char		*erc_dml_qry_entries[1];
	const char		*erc_dml_qry_values[1];
	char			erc_dml_rev_id[TCTYPE_name_size_c+1]="NR";
	char			apl_dml_rev_id[TCTYPE_name_size_c+1]="A";
	char			apl_dml_task_rev_id[TCTYPE_name_size_c+1]="A";
	const char		*apl_dml_qry_entries[1];
	const char		*apl_dml_qry_values[1];
	const char		*apl_dml_task_qry_entries[1];
	const char		*apl_dml_task_qry_values[1];
	int				ercdml_tags_found	= 0;
	tag_t ERCDMLTag = NULLTAG;
	tag_t	ObjTypProj	= NULLTAG;
	tag_t			*erc_dml_tag	= NULLTAG;
	tag_t	ERCObjTypProj	= NULLTAG;
	int  apl_number_found=0;
	tag_t *list_of_WSO_apl_tags=NULLTAG;
	tag_t *list_of_WSO_apl_task_tags=NULLTAG;
	tag_t			APLDTypeTag			= NULLTAG;
	tag_t			APLDCreInTag		= NULLTAG;
	int num=0;	
	char*			tempString			= NULL;
	char*			aplTaskno			= NULL;
	tag_t			APLDMLTag			= NULLTAG;
	tag_t			APLDMLRevTag		= NULLTAG;
	tag_t	relation_type= NULLTAG;
	char**			stringArrayAPLD		= NULL;
	tag_t			APLDRevTypeTag		= NULLTAG;
	tag_t  aplrelation = NULLTAG;
	tag_t			APLDRevCreInTag		= NULLTAG;
	int			    	n_strings			= 1;
	int			    	index			= 0;
	int			    	apl_task_number_found			= 0;
	int			    	i			= 0;
	int			    	estCnt			= 0;
		int				l_strings			= 300;
    tag_t  NewDMLAttrId = NULLTAG;
    tag_t  NewDMLRevAttrId = NULLTAG;
	WSO_search_criteria_t  	criteria;
	tag_t *list_of_WSO_tags=NULLTAG;
	tag_t			APLTTypeTag			= NULLTAG;
	tag_t			APLTCreInTag		= NULLTAG;
	tag_t			APLTRevTypeTag		= NULLTAG;
	tag_t			APLTRevCreInTag		= NULLTAG;
	char*			tempStringt			= NULL;
	tag_t			APLTaskTag			= NULLTAG;
	tag_t			APLTaskRevTag		= NULLTAG;
	char**			stringArrayAPLT		= NULL;
	tag_t  apltaskrelation = NULLTAG;
	tag_t			tsk_part_sol_rel_type= NULLTAG;
	const char *attrs[1];
	const char *values[1];
	tag_t *tags_found = NULL;
	int n_tags_found=0;
	tag_t item=NULLTAG;
	tag_t rev=NULLTAG;
	tag_t         Fndrelation = NULLTAG;
	tag_t         FndDMLTaskrelation = NULLTAG;
	tag_t         FndAPLERCDMLrelation = NULLTAG;
	tag_t			tsk_part_APL_rel	= NULLTAG;
	char ChangeaplAMLastUpDtDup[20]={0};
	date_t apl_last_md_dt ;
	date_t *apl_last_md_dt_cpy = NULL;
		tag_t	itemTypeTag_cdss							= NULLTAG;
	char  type_itemRev[TCTYPE_name_size_c+1];
    tag_t  NewTaskAttrId = NULLTAG;

		char *PltNm=NULL;
		char *ProcEdnNo=NULL;
		char *ShpNm=NULL;
		char *ConcatedStr = NULL;




	inputfile = ITK_ask_cli_argument("-i=");
	PlantName = ITK_ask_cli_argument("-plname=");

	ITK_CALL(ITK_auto_login( ));  
    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Auto login .......");fflush(stdout);

	stringArrayAPLD = (char**)malloc( n_strings * sizeof *stringArrayAPLD );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLD[index] = (char*)malloc( l_strings + 1 );
	}

	stringArrayAPLT = (char**)malloc( n_strings * sizeof *stringArrayAPLT );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLT[index] = (char*)malloc( l_strings + 1 );
	}

	
	if((tc_strcmp(PlantName,"PlantName1")==0) || (tc_strcmp(PlantName,"PlantName4")==0))
	{
		DmlNoRule=NULL;
		DmlNoRule=(char *) MEM_alloc(100);

		tc_strcpy(DmlNoRule,"APLC");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName13")==0)
	{
		DmlNoRule=NULL;
		DmlNoRule=(char *) MEM_alloc(100);
		tc_strcpy(DmlNoRule,"APLV");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName2")==0)
	{
		DmlNoRule=NULL;
		DmlNoRule=(char *) MEM_alloc(100);
		tc_strcpy(DmlNoRule,"APLU");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName3")==0)
	{
		DmlNoRule=NULL;
		DmlNoRule=(char *) MEM_alloc(100);
		tc_strcpy(DmlNoRule,"APLP");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName6")==0)
	{
		DmlNoRule=NULL;
		DmlNoRule=(char *) MEM_alloc(100);
		tc_strcpy(DmlNoRule,"APLS");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName8")==0)
	{
		DmlNoRule=NULL;
		DmlNoRule=(char *) MEM_alloc(100);
		tc_strcpy(DmlNoRule,"APLJ");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName9")==0)
	{
		DmlNoRule=NULL;
		DmlNoRule=(char *) MEM_alloc(100);
		tc_strcpy(DmlNoRule,"APLL");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName11")==0)
	{
		DmlNoRule=NULL;
		DmlNoRule=(char *) MEM_alloc(100);
		tc_strcpy(DmlNoRule,"APLD");
		printf("Plant Name After Conversion ==> [%s],[%s]\n",sPlntNmeDup1,DmlNoRule);fflush(stdout);
	}
	else
	{
		printf("\nSYNTEX ERROR :: \n");
		printf("\n PLEASE ENTER CORRECT PLANTNAME(EX:PlantName1,PlantName13,PlantName2 etc..)\n");
		printf("\nTRY AGAIN !!!\n");
		return status;
	}
	
	fp=fopen(inputfile,"r");
	fperror=fopen("error_APLDMLCreation.log","a+");
	fpexception=fopen("Exception_APLDMLCreation.log","a+");
	if(fp!=NULL)
	{
		inputline=(char *) MEM_alloc(1000);
		while(fgets(inputline,1000,fp)!=NULL)
		{
			printf("\n File rease.");fflush(stdout);
			fputs(inputline,stdout);
			
			dmlNo=NULL;	
			//drStatus=NULL;			
			
			
			dmlNo=strtok(inputline,"^");
			//drStatus=strtok(NULL,"^");
			printf("\n drStatus::::[%s]:[%s]\n",dmlNo,drStatus);fflush(stdout);

			ercdml_tags_found=0;
			dmlNoCpy=NULL;
			dmlNoCpy=(char *) MEM_alloc(1000);
			tc_strcpy(dmlNoCpy,dmlNo);


				apl_dml_qry_entries[0] ="item_id";
				apl_dml_qry_values[0] = dmlNo;
				//ITKCALL(ITEM_find_item_revs_by_key_attributes(1,apl_dml_qry_entries,apl_dml_qry_values,apl_dml_rev_id,&apl_number_found,&list_of_WSO_apl_tags));
				ITKCALL(ITEM_find_items_by_key_attributes(1, apl_dml_qry_entries, apl_dml_qry_values,&apl_number_found, &list_of_WSO_apl_tags));

				
				printf("\n apl_number_found is :[%d]\n",apl_number_found);fflush(stdout);
				if(apl_number_found > 0)
				{
					for(estCnt=0;estCnt<apl_number_found;estCnt++)
					{
					APLDMLRevTag=list_of_WSO_apl_tags[estCnt];
					if ((APLDMLRevTag != NULLTAG))
					{
						ITKCALL (TCTYPE_ask_object_type(APLDMLRevTag,&itemTypeTag_cdss));
						ITKCALL (TCTYPE_ask_name(itemTypeTag_cdss,type_itemRev));

						printf("\ntype_itemRev [%s]",type_itemRev);fflush(stdout);
						
						if((tc_strcmp(type_itemRev,"T5_EstimateSheet")==0))
						{
						
							ITKCALL(AOM_ask_value_string(APLDMLRevTag, "object_string", &objectString));
							ITKCALL(AOM_ask_value_string(APLDMLRevTag, "item_id", &item_id_value));
							ITKCALL(AOM_ask_value_string(APLDMLRevTag, "current_id", &current_id_value));

							ITKCALL(AOM_ask_value_string(APLDMLRevTag, "t5_ShopName", &ShpNm));
							ITKCALL(AOM_ask_value_string(APLDMLRevTag, "t5_ESPlantName", &PltNm));
							ITKCALL(AOM_ask_value_string(APLDMLRevTag, "t5_ProcEdnNumber", &ProcEdnNo));

							printf("\n PltNm: [%s]",PltNm);fflush(stdout);
							printf("\n ShpNm: [%s]",ShpNm);(stdout);
							printf("\n ProcEdnNo: [%s]",ProcEdnNo);fflush(stdout);
							printf("\n item_id_value: [%s]",item_id_value);fflush(stdout);

							ConcatedStr = NULL;
							ConcatedStr=(char *) MEM_alloc(100);
							strcpy(ConcatedStr,item_id_value);
							strcat(ConcatedStr,"-");
							strcat(ConcatedStr,PltNm);
							strcat(ConcatedStr,"-");
							strcat(ConcatedStr,ShpNm);
							strcat(ConcatedStr,"-");
							strcat(ConcatedStr,ProcEdnNo);

							printf("\n ConcatedStr: [%s]",ConcatedStr);fflush(stdout);

							
							if((tc_strcmp(ConcatedStr,"")!=0))
							{
								printf("\n Inside setting DR STatus...OLD ID[%s]:OLD  Current ID [%s]:OLD Object String [%s]",item_id_value,current_id_value,objectString);fflush(stdout);
								
								ITK_CALL(AOM_lock(APLDMLRevTag));
								
								ITK_CALL(POM_attr_id_of_attr("item_id","T5_EstimateSheet",&NewTaskAttrId));
								ITK_CALL(AOM_refresh(APLDMLRevTag,TRUE));
								ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
								ITK_CALL(POM_set_attr_string(1,&APLDMLRevTag,NewTaskAttrId,ConcatedStr));
								ITK_CALL(AOM_save(APLDMLRevTag));
								ITK_CALL(AOM_refresh(APLDMLRevTag,TRUE));


							}
							break;
						}
					
					  }
					}
				}
				else
				{
					printf("\n\t APL DML Not found\n",ercdmlNo);
	
				
				}


			}
			
					
				printf("-----------DML LIST COMPLETED-----------------------------------------\n");fflush(stdout);

		}
	
				
			


	printf("-----------DML LOADING SUCCESS-----------------------------------------\n");fflush(stdout);

	
	return status;

}

//tm_Estimate_Sheet_Data_Update -u=aplloader -pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba -i=Estimate2.txt -plname=PlantName1