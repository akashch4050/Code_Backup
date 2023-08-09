#define _CRT_SECURE_NO_DEPRECATE
#define NUM_ENTRIES 1
#define TE_MAXLINELEN  128
#include <epm/epm.h>
#include <ae/dataset_msg.h>
#include <tccore/iman_msg.h>
#include <ps/ps.h>
#include <pie/pie.h>//Added by Anshul Multilevel BOM explode
#include <ps/ps_errors.h>
#include <time.h>
#include <ai/sample_err.h>
#include <tc/tc.h>
#include <tccore/grm_msg.h>
#include <tccore/workspaceobject.h>
#include <bom/bom.h>
#include <ae/dataset.h>
#include <ps/ps_errors.h>
#include <sa/sa.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fclasses/tc_string.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <sa/tcfile.h>
#include <epm/releasestatus.h>
#include <tcinit/tcinit.h>
#include <tccore/tctype.h>
#include <res/reservation.h>
#include <tccore/aom.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <sa/imanfile.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <itk/mem.h>
#include <ss/ss_errors.h>
#include <sa/user.h>
#include <tccore/grm.h>
#include <tccore/item_msg.h>
#include <string.h>
#include <epm/cr_effectivity.h>
#include <tc/folder.h>
#include "tm_apl_std_common_function.c"

#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) {  return ifail;}

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
		}

int apl_dml_create(tag_t  DMLTag)
{
	char	*item_sequence_id;
	char	*DmlAnalyst_name=NULL;
	char	*DmlChangeSpecialist=NULL;
	int		status;
	int max_char_size = 80;
	char*			DMLAPL					=NULL;
	char*			Suffix					=NULL;
	tag_t			rootTask				=NULLTAG;
	char		*CurrentTask					= NULL;
	char       *parent_name				=NULL;
	char       *Proj_Code				=NULL;
	char*			Part_no				= NULL;
	char*			PartTypeStr			= NULL;
	char*			type_name1			= NULL;

	int n_tags_found = 0;
	tag_t			*attachments			=NULLTAG;
	int   ifail				= 0;
	
	int
	error_code	= ITK_ok,
	n_entries	= 2,
	n_found		= 0,
	num=0,
	i=0,
	j=0,
	ii			= 0,i1=0;
	tag_t
	query			= NULLTAG,
	*cntr_objects	= NULL,
	objTypeTag=NULLTAG,
	item=NULLTAG,
	rev=NULLTAG,
	user			= NULLTAG;
	char *item_id 	   = NULL;
	char *erc_item_id 	   = NULL;
	char *PlantName 	   = NULL;
	char *subSyscd 	   = NULL;//Start Plant Aggregate
	logical	delInd	= false;//TZ 1.43
	char *DesGrpVal 	   = NULL;//Start Plant Aggregate
	char *DesGrpTmp 	   = NULL;//Start Plant Aggregate
	tag_t *tags_found = NULL;
	int	  noAttachment,noTaskAttachment 	= 0;
	int	  iDes 	= 0;//Start Plant Aggregate

	tag_t			item_apl					=NULLTAG;
	tag_t			rev_apl					=NULLTAG;
	tag_t			APLDRevTypeTag		= NULLTAG;
	tag_t			APLDRevCreInTag		= NULLTAG;
	tag_t*			TaskRevision		= NULLTAG;
	tag_t*			PartTags			= NULLTAG;

	char**			stringArrayAPLD		= NULL;
	char**			stringArrayAPLT		= NULL;
	int			    	n_strings			= 1;
	int			    	n_strings_dsg		= 99;//Start Plant Aggregate
	int			    	cnt		= 0;//Start Plant Aggregate
	int			    	cnt1		= 0;//Start Plant Aggregate
	int			    	FlagFound		= 0;//Start Plant Aggregate
	int					FlagSnapshot    = 0;
	char*			object_type			= NULL;
	

	//char *item_id_dup 	   = NULL;
	int   apl_task_number_found;
	int  apl_number_found;
	int * erc_number_found;
	int  control_number_found;
	int tskcnterc =0;
	tag_t *Dml_ercTasks= NULLTAG;
	tag_t tsk_HSI_rel_type = NULLTAG;
	char* tsk_object_name = NULL ;
	char* tsk_object_type = NULL ;
	char   *DMLNumber =NULL;
    char   *taskGrp	  =NULL;

	tag_t *TskHSItems = NULLTAG;
	int TskHSIcount =0;
	int flagDMLhdr =0;

	tag_t *list_of_WSO_tags=NULLTAG;
	tag_t *list_of_WSO_erc_tags=NULLTAG;
	tag_t *list_of_WSO_cntrl_tags=NULLTAG;
	tag_t			APLDTypeTag			= NULLTAG;
	tag_t			APLDCreInTag		= NULLTAG;
	tag_t			APLDMLTag			= NULLTAG;
	tag_t			TaskRevTag			= NULLTAG;
	tag_t			tsk_part_sol_rel_type= NULLTAG;
	tag_t			AssyTag				= NULLTAG;
	tag_t			tsk_part_APL_rel	= NULLTAG;

	//Dhanashri added// UATZ1.45
	
    tag_t *list_of_WSO_cntrl_tags1=NULLTAG;

	tag_t   qryTagCntrl1     = NULLTAG;
	int     n_entryCntrl1    = 3;
	char    *qry_entryCntrl1[3]  = {"SYSCD","SUBSYSCD","Delete Indicator"};	
	char	**qry_valuesCntrl1= (char **) MEM_alloc(5 * sizeof(char *));

	int cntrlcnt=0;
	int  control_number_found1=0;
	int  FlagGotFrmToplt=0;

    char *FrmPltToPlt 	   = NULL;

	//End Dhanashri added// UATZ1.45

	//Priti added//
	tag_t			class_id			= NULLTAG;
	char			*class_name			= NULL;

	int				index				= 0;
	int				iDML				= 0;
	int				l_strings			= 500;
	char*			tempStringt			= NULL;
	char*			APL_Task_No			= NULL;
	int				count				= 0;
	int				TaskCnt				= 0;
	int				PartCnt				= 0;
	int				AttCnt				= 0;
	int				k					= 0;
	int				cntTaskD			= 0;

	tag_t			APLDMLRevTag		= NULLTAG;
	tag_t			APLTTypeTag			= NULLTAG;
	tag_t			APLTCreInTag		= NULLTAG;
	tag_t			APLTRevTypeTag		= NULLTAG;
	tag_t			APLTRevCreInTag		= NULLTAG;
	tag_t			APLTaskTag			= NULLTAG;
	tag_t			APLTaskRevTag		= NULLTAG;
	tag_t         Fndrelation = NULLTAG;
    tag_t  NewDMLAttrId = NULLTAG;
    tag_t  NewDMLRevAttrId = NULLTAG;

	char *item_erc_id_dup = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *item_erc_id_dup1 = (char *)MEM_alloc(max_char_size * sizeof(char));
	char*			tempString			= NULL;

     char *erc_dml_name  = NULL;
	 char *erc_dml_desc  = NULL;
	 char *erc_rel_type  = NULL;
	 char *erc_dr_status  = NULL;
	 char *erc_FrmToPlnt  = NULL;
	char *item_name   = NULL;
	char *Design_group = NULL;
	char *RlsType = NULL;
	char *DML_no = NULL;
	char **DesignGroupList;
	char **DesignGroupStr= NULL;// Plant Aggregate
	char **DesignGroupStr1= NULL;// Plant Aggregate
	char **TaskDesignGroupStr= NULL;// Plant Aggregate
	char *DesGrpFrst= NULL;// Plant Aggregate
	char *DesGrpFrstTmp= NULL;// Plant Aggregate
	int	DesGrpFrstTmp1;// Plant Aggregate
	char	DesGrpFrstTmp2[20];// Plant Aggregate
	char *DesGrpLst= NULL;// Plant Aggregate
	char   type_name[TCTYPE_name_size_c+1];
	tag_t	relation_type,relation	,propTag	= NULLTAG;
	tag_t  aplrelation = NULLTAG;
	tag_t  apltaskrelation = NULLTAG;
	WSO_search_criteria_t  	criteria;
	WSO_search_criteria_t  	criteria_erc;
	WSO_search_criteria_t  	criteria_control;
	const char		*apl_dml_qry_entries[1];
	const char		*apl_dml_qry_values[1];
	const char		*apl_task_qry_entries[1];
	const char		*apl_task_qry_values[1];
	char			apl_dml_rev_id[TCTYPE_name_size_c+1]="A";
	char			apl_dml_task_rev_id[TCTYPE_name_size_c+1]="A";

	tag_t   qryTagCntrl     = NULLTAG;
	int     n_entryCntrl    = 3;
	char    *qry_entryCntrl[3]  = {"Name","SUBSYSCD","Delete Indicator"};	
	char	**qry_valuesCntrl= (char **) MEM_alloc(5 * sizeof(char *));

	char    *qry_entryCntrlPB[4]  = {"Name","SUBSYSCD","Delete Indicator","Information-1"};	
	char	**qry_valuesCntrlPB= (char **) MEM_alloc(6 * sizeof(char *));
	int		control_number_foundPrj		=	0;
	int		clrSvrCnt					=	0;
	int		n_entryCntrlPB					=	4;	//Deepti TZ3.52

	tag_t   qryTagCntrlPB				=	NULLTAG;
	tag_t   dml_svr_sol_rel_type		=	NULLTAG;
	tag_t   dml_svr_APL_rel				=	NULLTAG;
	tag_t	*list_of_WSO_cntrlPrj_tags	=	NULLTAG;
	tag_t	*clrSvrTags					=	NULLTAG;

	int		n_entryCntrlUnitBOM			=	4;
	int		control_number_foundUnitBOM	=	0;

	char    *qry_entryCntrlUnitBOM[4]  = {"SYSCD","SUBSYSCD","Delete Indicator","Information-1"};	//HEMAL TZ1.52
	char	**qry_valuesCntrlUnitBOM= (char **) MEM_alloc(5 * sizeof(char *));//HEMAL TZ1.52

	tag_t   qryTagCntrlUnitBom			=	NULLTAG;
	tag_t *list_of_WSO_cntrl_tagsUnitBOM=NULLTAG;
	tag_t  reln_type_Reference =NULLTAG;

	int		n_attchs_ref		=	0;
	tag_t	*rellist_refs		=	NULLTAG;



	char *item_id_dup = (char *)MEM_alloc(max_char_size * sizeof(char));
    printf("\n **************inside apl_dml_create***************\n ");fflush(stdout);

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
	//Start Plant Aggregate
	DesignGroupStr = (char**)malloc( n_strings_dsg * sizeof *DesignGroupStr );
	for( index=0; index<n_strings_dsg; index++ )
	{
		DesignGroupStr[index] = (char*)malloc( l_strings + 1 );
	}

	DesignGroupStr1 = (char**)malloc( n_strings_dsg * sizeof *DesignGroupStr1 );
	for( index=0; index<n_strings_dsg; index++ )
	{
		DesignGroupStr1[index] = (char*)malloc( l_strings + 1 );
	}

	TaskDesignGroupStr = (char**)malloc( n_strings_dsg * sizeof *TaskDesignGroupStr );
	for( index=0; index<n_strings_dsg; index++ )
	{
		TaskDesignGroupStr[index] = (char*)malloc( l_strings + 1 );
	}

	// DesGrpTmp = (char *)MEM_alloc(max_char_size * sizeof(char));
	//END Plant Aggregate


	if(TCTYPE_ask_object_type(DMLTag,&objTypeTag));
	if(TCTYPE_ask_name(objTypeTag,type_name));
	printf("\n     type_name changes done: for workspaceobject     %s\n", type_name);fflush(stdout);
	

	if(strcmp(type_name,"ChangeRequestRevision")==0)
	{
		printf("\n inside class ChangeRequestRevision......\n");fflush(stdout);
		AOM_ask_value_string( DMLTag, "item_id", &item_id);
		AOM_ask_value_string( DMLTag, "current_id", &DML_no);
		AOM_ask_value_strings(DMLTag,"t5_crdesigngroup",&num,&DesignGroupList);
		AOM_ask_value_string( DMLTag, "t5_cprojectcode", &Proj_Code);
		AOM_ask_value_string( DMLTag, "object_name", &erc_dml_name);
		AOM_ask_value_string( DMLTag, "object_desc", &erc_dml_desc);
		AOM_ask_value_string( DMLTag, "t5_rlstype", &erc_rel_type);
		AOM_ask_value_string( DMLTag, "t5_cDRstatus", &erc_dr_status);
		AOM_ask_value_string( DMLTag, "t5_PlntToPlnt", &erc_FrmToPlnt);

		printf("\n Proj_Code : %s\n",Proj_Code);fflush(stdout);
		printf("\n item_id : %s\n",item_id);fflush(stdout);
		printf("\n DML_no : %s\n",DML_no);fflush(stdout);		
		printf("\n num is : %d\n",num);fflush(stdout);
		printf("\n erc_dml_name is : %s\n",erc_dml_name);fflush(stdout);
		printf("\n erc_dml_desc is : %s\n",erc_dml_desc);fflush(stdout);
		printf("\n erc_rel_type is : %s\n",erc_rel_type);fflush(stdout);
		printf("\n erc_dr_status is : %s\n",erc_dr_status);fflush(stdout);
		printf("\n erc_FrmToPlnt is : %s\n",erc_FrmToPlnt);fflush(stdout);

		/*WSOM_clear_search_criteria(&criteria_control);
		strcpy(criteria_control.name,"PlantDML");
		strcpy(criteria_control.class_name,"T5_ControlObject");
		status	= WSOM_search(criteria_control, &control_number_found, &list_of_WSO_cntrl_tags);*/
		
		//char	*qry_valuesCntrl[3] = {"PlantDML",Proj_Code,"0"};


		//Added By Dhanashri to handle TODR case UATZ1.45
		FlagGotFrmToplt=0;
		if(tc_strcmp(erc_rel_type,"TODR")==0)
		{
			control_number_found1=0;
			list_of_WSO_cntrl_tags1=NULL;

			if(QRY_find("Control Objects...", &qryTagCntrl1));
			if (qryTagCntrl1)
			{
				printf("\n Control Object Query Found \n");fflush(stdout);
				qry_valuesCntrl1[0]="GMDMLApp";
				qry_valuesCntrl1[1]="Live";
				qry_valuesCntrl1[2]="0";
				
				if(QRY_execute(qryTagCntrl1, n_entryCntrl1, qry_entryCntrl1, qry_valuesCntrl1, &control_number_found1, &list_of_WSO_cntrl_tags1));
			}
			else
			{
				printf("\n Control Object Query not Found for GM DML \n");fflush(stdout);
			}	
			
			printf("\n control_number_found1 is : %d\n",control_number_found1);fflush(stdout);

			for(cntrlcnt=0;cntrlcnt<control_number_found1;cntrlcnt++)
				{
				    AOM_ask_value_string( list_of_WSO_cntrl_tags1[cntrlcnt], "t5_Userinfo1", &FrmPltToPlt);
					printf("\n FrmPltToPlt: %s\n",FrmPltToPlt);fflush(stdout);
		
					if(tc_strcmp(FrmPltToPlt,erc_FrmToPlnt)==0)
							{
								printf("\n inside got plant .. ");fflush(stdout);
								FlagGotFrmToplt=1;
								break;
							}
							else
							{
								FlagGotFrmToplt=2;
								printf("\n FlagGotFrmToplt set to 2 ");fflush(stdout);

							}
				}
		}
		printf("\n FlagGotFrmToplt: %d\n",FlagGotFrmToplt);fflush(stdout);
		if ((tc_strcmp(erc_rel_type,"SSR")==0) && (((tc_strcmp(DesignGroupList[0],"60")==0)||(tc_strcmp(DesignGroupList[0],"61")==0)||(tc_strcmp(DesignGroupList[0],"62")==0)||(tc_strcmp(DesignGroupList[0],"63")==0)||(tc_strcmp(DesignGroupList[0],"65")==0)||(tc_strcmp(DesignGroupList[0],"72")==0)||(tc_strcmp(DesignGroupList[0],"73")==0)||(tc_strcmp(DesignGroupList[0],"74")==0)||(tc_strcmp(DesignGroupList[0],"81")==0)||(tc_strcmp(DesignGroupList[0],"88")==0))))            
		{
			printf("\nCondition for SnapShot Realease DML Ceation Satisfied   ");fflush(stdout);
			FlagSnapshot=1;	
		}
		if(FlagGotFrmToplt!=2)
		{
		//End Of Code Added By Dhanashri to handle TODR case UATZ1.45

		control_number_found=0;
		list_of_WSO_cntrl_tags=NULL;

		if(QRY_find("Control Objects...", &qryTagCntrl));
		if (qryTagCntrl)
		{
			printf("\n Control Object Query Found \n");fflush(stdout);
			qry_valuesCntrl[0]="PlantDML";
			qry_valuesCntrl[1]=Proj_Code;
			qry_valuesCntrl[2]="0";
			
			if(QRY_execute(qryTagCntrl, n_entryCntrl, qry_entryCntrl, qry_valuesCntrl, &control_number_found, &list_of_WSO_cntrl_tags));
		}
		else
		{
			printf("\n Control Object Query not Found \n");fflush(stdout);
		}		
		
		printf("\n control_number_found is : %d\n",control_number_found);fflush(stdout);
		for(j=0;j<control_number_found;j++)
		{
			//AOM_ask_value_string( list_of_WSO_cntrl_tags[j], "t5_SubSyscd", &subSyscd);
	        printf("\n Proj_Code: %s\n",Proj_Code);fflush(stdout);

			AOM_ask_value_logical( list_of_WSO_cntrl_tags[j], "t5_DelInd", &delInd);
	        printf("\n delInd: %d\n",delInd);fflush(stdout);

			//if((strcmp(subSyscd,Proj_Code)==0) && (delInd==0))
			//{
				cntTaskD=0;
				printf("\n j++++++++++++++++++[%d]\n",j);fflush(stdout);

				item_erc_id_dup = NULL;
				item_erc_id_dup =	(char *)MEM_alloc(max_char_size * sizeof(char));

				strcpy(item_erc_id_dup,item_id);
				AOM_ask_value_string( list_of_WSO_cntrl_tags[j], "t5_Userinfo1", &PlantName);
				printf("\n PlantName11: %s\n",PlantName);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags[j], "t5_Userinfo3", &DesGrpVal);
			    printf("\n DesGrpVal: %s\n",DesGrpVal);fflush(stdout);

				strcat (item_erc_id_dup,"_");
				strcat (item_erc_id_dup,PlantName);

				printf("\n item_erc_id_dup: %s\n",item_erc_id_dup);fflush(stdout);
				apl_number_found=0;
				/*WSOM_clear_search_criteria(&criteria_erc);
				strcpy(criteria_erc.name,item_erc_id_dup);
				strcpy(criteria_erc.class_name,"T5_APLDMLRevision");
				status	= WSOM_search(criteria_erc, &apl_number_found, &list_of_WSO_erc_tags);

				printf("\n apl_number_found is :[%d]\n",apl_number_found);fflush(stdout);*/

				apl_dml_qry_entries[0] ="item_id";
				apl_dml_qry_values[0] = item_erc_id_dup;
				ITKCALL(ITEM_find_item_revs_by_key_attributes(1,apl_dml_qry_entries,apl_dml_qry_values,apl_dml_rev_id,&apl_number_found,&list_of_WSO_erc_tags));
				
				printf("\n apl_number_found is :[%d]\n",apl_number_found);fflush(stdout);

				if(apl_number_found == 0)
				{
					MEM_free(list_of_WSO_erc_tags);
					//Start Plant Aggregate
					cnt =0;				
					if((tc_strcmp(DesGrpVal,"")!=0) && (tc_strcmp(DesGrpVal,"00-99")!=0))
					{
						if(tc_strcmp(DesGrpVal,"")!=0)
						{
							DesGrpTmp = strtok(DesGrpVal, ", ");
							while (DesGrpTmp != NULL)
							{
								printf("VAULE:%s\n", DesGrpTmp);
								tc_strcpy( DesignGroupStr[cnt],DesGrpTmp);
								cnt++;
								DesGrpTmp = strtok(NULL, ",");
								printf("VAULE DesGrpTmp:%s\n", DesGrpTmp);
							}
						 }

						 printf("\ncnt=[%d]\n",cnt);fflush(stdout);

						//TOKEN BASED on -
						DesGrpTmp = NULL;
						cnt1 =0;
						for(iDes=0;iDes<cnt;iDes++)
						{
							 DesGrpTmp = NULL;
							 printf("\niDes Value=[%s]\n",DesignGroupStr[iDes]);fflush(stdout);

							 if(tc_strcmp(DesignGroupStr[iDes],"")!=0)
							{
								 if(tc_strstr(DesignGroupStr[iDes],"-")!=NULL)
								{
									DesGrpFrst	=strtok(DesignGroupStr[iDes],"-");
									DesGrpLst	=strtok(NULL,"-");

									printf("\nDesGrpFrst=[%s]\n",DesGrpFrst);fflush(stdout);

								//	tc_strcpy( DesignGroupStr1[cnt1],DesGrpFrst);
								//	cnt1++;

									DesGrpFrstTmp1=atoi(DesGrpFrst);

									 while (DesGrpFrstTmp1!=atoi(DesGrpLst))
									 {
										   sprintf(DesGrpFrstTmp2,"%d",DesGrpFrstTmp1);
											 if(tc_strcmp(DesGrpFrstTmp2,"0")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"00");
												printf("\nInside DesGrpFrst********\n");fflush(stdout);
										   }
										 else  if(tc_strcmp(DesGrpFrstTmp2,"1")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"01");
										   }
										   else  if(tc_strcmp(DesGrpFrstTmp2,"2")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"02");
										   }
											 else  if(tc_strcmp(DesGrpFrstTmp2,"3")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"03");
										   }
											 else  if(tc_strcmp(DesGrpFrstTmp2,"4")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"04");
										   }
											 else  if(tc_strcmp(DesGrpFrstTmp2,"5")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"05");
										   }
											 else  if(tc_strcmp(DesGrpFrstTmp2,"6")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"06");
										   }
											 else  if(tc_strcmp(DesGrpFrstTmp2,"7")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"07");
										   }
											 else  if(tc_strcmp(DesGrpFrstTmp2,"8")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"08");
										   }
											 else  if(tc_strcmp(DesGrpFrstTmp2,"9")==0)
										  {
												tc_strcpy(DesignGroupStr1[cnt1],"09");
										   }
										   else
											{
											   printf("\nDesGrpFrstTmp2=[%s]\n",DesGrpFrstTmp2);fflush(stdout);
												tc_strcpy(DesignGroupStr1[cnt1],DesGrpFrstTmp2);
											}
											cnt1++;
											 DesGrpFrstTmp1=atoi(DesGrpFrstTmp2);
											 DesGrpFrstTmp1++;
									 }
									 printf("\nDesGrpLst=[%s]\n",DesGrpLst);fflush(stdout);
									 tc_strcpy( DesignGroupStr1[cnt1],DesGrpLst);
									 cnt1++;
								}
								else
								{
									printf("\nCOPIED=[%s]\n",DesignGroupStr[iDes]);fflush(stdout);
									tc_strcpy( DesignGroupStr1[cnt1],DesignGroupStr[iDes]);
									cnt1++;
								}
							 }
						 }
						 printf("\nAfter All Token cnt1=[%d]\n",cnt1);fflush(stdout);

						 for(iDes=0;iDes<cnt1;iDes++)
						 {
							 printf("\nDesignGroupStr1[iDes] Value=[%s]\n",DesignGroupStr1[iDes]);fflush(stdout);
						 }
					}
					else
					{
						printf("\n*********No range mentioned in PlantDML Control Object->all task should be created*********\n");fflush(stdout);
					}
					//END Plant Aggregate
					printf("\n apl_number_found is111 : %d\n",apl_number_found);fflush(stdout);
					
					
					tsk_HSI_rel_type = NULLTAG;
					tsk_object_name = NULL ;
					tsk_object_type = NULL ;
					TskHSItems = NULLTAG;
					TskHSIcount = 0;
					flagDMLhdr  = 0;
					DMLNumber =NULL;
					taskGrp	  =NULL;
					

					CALLAPI(AOM_ask_value_tags(DMLTag,"T5_DMLTaskRelation",&count,&TaskRevision));
					printf("\n No of Task in ERCDML : %d\n",count);fflush(stdout);

					//if(strcmp(erc_rel_type,"SVR")==0)  // Start For SVR Release
					if((strcmp(erc_rel_type,"SVR")==0 || strcmp(erc_rel_type,"TODR")==0  ) || (FlagSnapshot==1))  // added by Dhanashri TZ1.45 for TODR
					{
						CALLAPI(GRM_find_relation_type("CMReferences", &tsk_HSI_rel_type));
					}
					else if(strcmp(erc_rel_type,"UVF")==0)  // added by Deepti TZ1.54 for UVF
					{
						CALLAPI(GRM_find_relation_type("T5_ModuleVF_to_Change", &tsk_HSI_rel_type));
					}
					else
					{
						CALLAPI(GRM_find_relation_type("CMHasSolutionItem", &tsk_HSI_rel_type));
					}
					
					printf("\nAfter All Token cnt1=[%d]\n",cnt1);fflush(stdout);

					if (count>0)
					{
						for (i1=0;i1<count ;i1++ )
						{
							if (tsk_HSI_rel_type!=NULLTAG)
							{
								tsk_object_name = NULL ;
								tsk_object_type = NULL ;

								if(AOM_ask_value_string(TaskRevision[i1],"object_name",&tsk_object_name)==ITK_ok)
								if(AOM_ask_value_string(TaskRevision[i1],"object_type",&tsk_object_type)==ITK_ok)
								printf("\n\n\t\t tsk_object_type is :%s",tsk_object_type);fflush(stdout);

								if(strcmp(tsk_object_type,"T5_ChangeTaskRevision")==0)
								{
									DMLNumber = strtok( tsk_object_name, "_" );
									taskGrp	  = strtok ( NULL, "_" );
									printf("\n\n\t\t taskGrp is :%s",taskGrp); fflush(stdout);
								
									CALLAPI(GRM_list_secondary_objects_only(TaskRevision[i1], tsk_HSI_rel_type, &TskHSIcount, &TskHSItems));
									printf("\n\t i1: %d  Task: %s --->> TskHSIcount partcnt:  %d ",i1, tsk_object_name, TskHSIcount);fflush(stdout);

									if((TskHSIcount!=0))
									{
										if((tc_strcmp(DesGrpVal,"")!=0) && (tc_strcmp(DesGrpVal,"00-99")!=0))
										{
											printf("\n\n\t\t cnt1 is :%d",cnt1); fflush(stdout);
											for(iDes=0;iDes<cnt1;iDes++)
											{
												printf("\n\n\t\t taskGrp is :%s",taskGrp); fflush(stdout);
												printf("\n\n\t\t DesignGroupStr1[iDes] is :%s",DesignGroupStr1[iDes]); fflush(stdout);
												if(tc_strcmp(taskGrp,DesignGroupStr1[iDes])==0)
												{
													printf("\n\n\t\t ******MATCHES******\n"); fflush(stdout);
													flagDMLhdr=1;
													break;
												}
											}
										}
										else
										{
											flagDMLhdr=1;
											break;
										}									
									}
									else
									{
										flagDMLhdr=0;
									}
								}
							}
						}

						printf("\nflagDMLhdr[%d]\n",flagDMLhdr);fflush(stdout);
						if(flagDMLhdr!=0 || FlagSnapshot!=0 )
						{							
							CALLAPI( TCTYPE_find_type( "T5_APLDML", NULL, &APLDTypeTag) );
							CALLAPI( TCTYPE_construct_create_input( APLDTypeTag, &APLDCreInTag) );

							CALLAPI( TCTYPE_find_type( "T5_APLDMLRevision", NULL, &APLDRevTypeTag) );
							CALLAPI( TCTYPE_construct_create_input( APLDRevTypeTag, &APLDRevCreInTag) );

							tc_strcpy( stringArrayAPLD[0], erc_dml_name);
							CALLAPI( TCTYPE_set_create_display_value( APLDCreInTag, "object_name", 1, (const char**)stringArrayAPLD) );
							tc_strcpy( stringArrayAPLD[0], erc_dml_desc);
							CALLAPI( TCTYPE_set_create_display_value( APLDCreInTag, "object_desc", 1, (const char**)stringArrayAPLD) );

							CALLAPI( AOM_tag_to_string(APLDRevCreInTag, &tempString) );
							tc_strcpy( stringArrayAPLD[0], tempString);
							printf("\nTest0.D1..[%s]\n",stringArrayAPLD[0]);
							CALLAPI( TCTYPE_set_create_display_value( APLDCreInTag, "revision", 1,(const char**)stringArrayAPLD) );

							tc_strcpy( stringArrayAPLD[0], "A");
							CALLAPI( TCTYPE_set_create_display_value( APLDRevCreInTag, "item_revision_id", 1, (const char**)stringArrayAPLD) );

							printf("\nTest1..\n");
							CALLAPI( TCTYPE_create_object(APLDCreInTag, &APLDMLTag) );
							printf("\nTest2..\n");

							AOM_save(APLDMLTag);							

							//CALLAPI( FL_user_update_newstuff_folder(APLDMLTag) );

							CALLAPI(ITEM_ask_latest_rev(APLDMLTag,&APLDMLRevTag));

							if (DMLTag != NULLTAG && APLDMLRevTag != NULLTAG)
							{
								GRM_find_relation_type("T5_DMLTaskRelation",&relation_type);
								GRM_create_relation(DMLTag, APLDMLRevTag, relation_type,  NULLTAG, &aplrelation);
								GRM_save_relation(aplrelation);
								printf("\n APL DML Created for ERC DML  : %s\n",item_erc_id_dup);fflush(stdout);
								//Update Project code and other attributes on APL DML
								CALLAPI(AOM_lock(APLDMLRevTag));
								CALLAPI(AOM_set_value_string(APLDMLRevTag,"t5_cprojectcode",Proj_Code));
								CALLAPI(AOM_set_value_string(APLDMLRevTag,"t5_rlstype",erc_rel_type));								
								CALLAPI(AOM_set_value_string(APLDMLRevTag,"t5_cDRstatus",erc_dr_status));								
								CALLAPI(AOM_set_value_string(APLDMLRevTag,"t5_PlntToPlnt",erc_FrmToPlnt));								
								CALLAPI( AOM_save(APLDMLRevTag) );
								CALLAPI( AOM_unlock(APLDMLRevTag) );
							}

							for(i=0;i<count;i++)
							{	
								FlagFound=0;
								if (tsk_HSI_rel_type!=NULLTAG)
								{
									tsk_object_name =NULL;
									tsk_object_type =NULL;

									if(AOM_ask_value_string(TaskRevision[i],"object_name",&tsk_object_name)==ITK_ok)
									if(AOM_ask_value_string(TaskRevision[i],"object_type",&tsk_object_type)==ITK_ok)
									printf("\n\n\t\t tsk_object_type is :%s",tsk_object_type);fflush(stdout);

									if(strcmp(tsk_object_type,"T5_ChangeTaskRevision")==0)
									{
										DMLNumber = strtok( tsk_object_name, "_" );
										taskGrp	  = strtok ( NULL, "_" );
										printf("\n\n\t\t taskGrp is :%s",taskGrp); fflush(stdout);
										if (tc_strcmp(erc_rel_type,"SSR")==0) 
										{
											
											if (((tc_strcmp(taskGrp,"60")==0)||(tc_strcmp(taskGrp,"61")==0)||(tc_strcmp(taskGrp,"62")==0)||(tc_strcmp(taskGrp,"63")==0)||(tc_strcmp(taskGrp,"65")==0)||(tc_strcmp(taskGrp,"72")==0)||(tc_strcmp(taskGrp,"73")==0)||(tc_strcmp(taskGrp,"74")==0)||(tc_strcmp(taskGrp,"81")==0)||(tc_strcmp(taskGrp,"88")==0)))
											{
												printf("\n\n\t\t taskGrp is Creatig Snapshot released APL DML",taskGrp); fflush(stdout);
											}
											else
											{
												continue;
											}
										}
										CALLAPI(GRM_list_secondary_objects_only(TaskRevision[i], tsk_HSI_rel_type, &TskHSIcount, &TskHSItems));
										printf("\n\t i: %d  Task: %s --->> TskHSIcount partcnt:  %d ",i, tsk_object_name, TskHSIcount);fflush(stdout);

										if((TskHSIcount!=0))
										{
											if((tc_strcmp(DesGrpVal,"")!=0) && (tc_strcmp(DesGrpVal,"00-99")!=0))
											{
												for(iDes=0;iDes<cnt1;iDes++)
												{
													if(tc_strcmp(taskGrp,DesignGroupStr1[iDes])==0)
													{
														FlagFound=1;
														break;
													}
												}
											}
											else
											{
												FlagFound=1;
											}

											printf("\n Item ID changed --> item_erc_id_dup:%s",item_erc_id_dup);

											if(FlagFound==1)
											{
												if (APLDMLTag != NULLTAG)
												{
												   if(POM_attr_id_of_attr("item_id","T5_APLDML",&NewDMLAttrId));
												   if(AOM_refresh(APLDMLTag,TRUE));
												   if(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
												   if(POM_set_attr_string(1,&APLDMLTag,NewDMLAttrId,item_erc_id_dup));
												   if(AOM_save(APLDMLTag));
												   if(AOM_refresh(APLDMLTag,TRUE));
												   if(tm_updateObjMasterFormName(APLDMLTag, item_erc_id_dup));//Update MASTER
												}

											   if (APLDMLRevTag != NULLTAG)
											   {
												   if(POM_attr_id_of_attr("item_id","T5_APLDMLRevision",&NewDMLRevAttrId));
												   if(AOM_refresh(APLDMLRevTag,TRUE));
												   if(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
												   if(POM_set_attr_string(1,&APLDMLRevTag,NewDMLRevAttrId,item_erc_id_dup));
												   if(AOM_save(APLDMLRevTag));
												   if(AOM_refresh(APLDMLRevTag,TRUE));
												   if(tm_updateObjRevMasterFormName(APLDMLRevTag, item_erc_id_dup));//Update MASTER
											   }

											   printf("\n item_erc_id_dup is : %s\n",item_erc_id_dup);fflush(stdout);
											   strcpy(item_erc_id_dup1,item_erc_id_dup);

											   DMLAPL = strtok (item_erc_id_dup,"_");
											   Suffix = strtok (NULL,"_");

											   item_erc_id_dup = NULL;
												item_erc_id_dup =	(char *)MEM_alloc(max_char_size * sizeof(char));
												strcpy(item_erc_id_dup,item_erc_id_dup1);

												printf("\n DMLAPL is : %s\n",DMLAPL);fflush(stdout);
												printf("\n Suffix is : %s\n",Suffix);fflush(stdout);

												strcpy(item_id_dup,DMLAPL);
												strcat (item_id_dup,"_");
												strcat (item_id_dup,taskGrp);
												strcat (item_id_dup,"_");
												strcat(item_id_dup,Suffix);

												printf("\n item_id_dup AFTER :%s\n",item_id_dup);fflush(stdout);
												
												apl_task_number_found=0;
												/*WSOM_clear_search_criteria(&criteria);
												strcpy(criteria.name,item_id_dup);
												strcpy(criteria.class_name,"T5_APLTaskRevision");
												status	= WSOM_search(criteria, &apl_task_number_found, &list_of_WSO_tags);
												printf("\n\n\t\t apl_task_number_found count in DB is : %d\n",apl_task_number_found);fflush(stdout);*/

												apl_task_qry_entries[0] ="item_id";
												apl_task_qry_values[0] = item_id_dup;
												ITKCALL(ITEM_find_item_revs_by_key_attributes(1,apl_task_qry_entries,apl_task_qry_values,apl_dml_task_rev_id,&apl_task_number_found,&list_of_WSO_tags));
												printf("\n\n\t\t apl_task_number_found count in DB is : %d\n",apl_task_number_found);fflush(stdout);

												if(apl_task_number_found == 0)
												{
													MEM_free(list_of_WSO_tags);
													printf("\n item_id_dup :%s and Design_group :%s and DML_no :%s \n ",item_id_dup,taskGrp,DML_no);fflush(stdout);
												
													CALLAPI( TCTYPE_find_type( "T5_APLTask", NULL, &APLTTypeTag) );
													CALLAPI( TCTYPE_construct_create_input( APLTTypeTag, &APLTCreInTag) );

													CALLAPI( TCTYPE_find_type( "T5_APLTaskRevision", NULL, &APLTRevTypeTag) );
													CALLAPI( TCTYPE_construct_create_input( APLTRevTypeTag, &APLTRevCreInTag) );


													tc_strcpy( stringArrayAPLT[0], item_id_dup);
													CALLAPI( TCTYPE_set_create_display_value( APLTCreInTag, "item_id", 1,(const char**)stringArrayAPLT) );
													tc_strcpy( stringArrayAPLT[0], erc_dml_name);
													CALLAPI( TCTYPE_set_create_display_value( APLTCreInTag, "object_name", 1,(const char**)stringArrayAPLT) );

													CALLAPI( AOM_tag_to_string(APLTRevCreInTag, &tempStringt) );
													tc_strcpy( stringArrayAPLT[0], tempStringt);
													printf("\nTest0.4..[%s]\n",stringArrayAPLT[0]);
													CALLAPI( TCTYPE_set_create_display_value( APLTCreInTag, "revision", 1,(const char**) stringArrayAPLT) );
													tc_strcpy( stringArrayAPLT[0], "A");
													CALLAPI( TCTYPE_set_create_display_value( APLTRevCreInTag, "item_revision_id", 1,(const char**)stringArrayAPLT) );
													tc_strcpy( stringArrayAPLT[0], taskGrp);
													CALLAPI( TCTYPE_set_create_display_value( APLTRevCreInTag, "t5_crdesigngroup", 1,(const char**)stringArrayAPLT) );
													tc_strcpy( stringArrayAPLT[0], Proj_Code);
													CALLAPI( TCTYPE_set_create_display_value( APLTRevCreInTag, "t5_cprojectcode", 1,(const char**)stringArrayAPLT) );
													
													CALLAPI( TCTYPE_create_object( APLTCreInTag, &APLTaskTag) );
													CALLAPI( AOM_save(APLTaskTag) );
													CALLAPI( AOM_unlock(APLTaskTag) );

													tc_strcpy(TaskDesignGroupStr[cntTaskD],taskGrp);
													cntTaskD++;

													CALLAPI(ITEM_ask_latest_rev(APLTaskTag,&APLTaskRevTag));
													printf("\n Item created first time only with item_id \n");fflush(stdout);
													
													if (APLDMLRevTag != NULLTAG && APLTaskRevTag != NULLTAG)
													{
														GRM_find_relation_type("T5_DMLTaskRelation",&relation_type);
														GRM_create_relation(APLDMLRevTag, APLTaskRevTag, relation_type,  NULLTAG, &apltaskrelation);
														GRM_save_relation(apltaskrelation);
														printf("\n APL DML Created for ERC DML  : %s\n",item_erc_id_dup);fflush(stdout);

														TaskRevTag = TaskRevision[i];
														CALLAPI(AOM_ask_value_string(TaskRevTag,"object_type",&object_type));
														printf("\n\n\t\t object_type is :%s",object_type);fflush(stdout);

														//For Veh SVR is in Regerence Items asd CCVC is in Solution Item, so both Relation created 
														if (strcmp(erc_rel_type,"Veh")==0)
														{
															if(strcmp(object_type,"T5_ChangeTaskRevision")==0)
															{
																erc_item_id = NULL;
																AOM_ask_value_string( TaskRevTag, "item_id", &erc_item_id);
																if(strstr(item_id_dup,erc_item_id))  // Check the respective ERC task only
																{
																	PartCnt=0;
																	CALLAPI(AOM_ask_value_tags(TaskRevTag,"CMReferences",&PartCnt,&PartTags));
																	printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
																	if (PartCnt>0)
																	{
																		GRM_find_relation_type("CMReferences",&tsk_part_sol_rel_type);
																		for (k=0;k<PartCnt ;k++ )
																		{
																			printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
																			AssyTag=PartTags[k];

																			if(AssyTag != NULLTAG && APLTaskRevTag != NULLTAG && tsk_part_sol_rel_type != NULLTAG)
																			{
																				CALLAPI(GRM_find_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type ,&Fndrelation));
																				if(Fndrelation)
																				{
																					printf("\n\t Relation Already Exist.\n" );fflush(stdout);
																				}
																				else
																				{
																					GRM_create_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type,  NULLTAG, &tsk_part_APL_rel);
																					GRM_save_relation(tsk_part_APL_rel);
																				}
																				printf("\n\n\t\t APL DML Cre:Object_PartType is :%s\n",PartTypeStr);fflush(stdout);
																			}
																		}
																	}
																} // End Check the respective ERC task only
															}
														}
														//SVR Attached for Vehicle DML Ends

														//if(strcmp(erc_rel_type,"SVR")==0)  // Start For SVR Release
														if(strcmp(erc_rel_type,"SVR")==0 || strcmp(erc_rel_type,"TODR")==0 || strcmp(erc_rel_type,"SSR")==0)  // added by Dhanashri TZ1.45 for TODR
														{
															if(strcmp(object_type,"T5_ChangeTaskRevision")==0)
															{
																erc_item_id = NULL;
																AOM_ask_value_string( TaskRevTag, "item_id", &erc_item_id);
																if(strstr(item_id_dup,erc_item_id))  // Check the respective ERC task only
																{
																	PartCnt=0;
																	CALLAPI(AOM_ask_value_tags(TaskRevTag,"CMReferences",&PartCnt,&PartTags));
																	printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
																	if (PartCnt>0)
																	{
																		GRM_find_relation_type("CMReferences",&tsk_part_sol_rel_type);
																		for (k=0;k<PartCnt ;k++ )
																		{
																			printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
																			AssyTag=PartTags[k];
																			/*
																			CALLAPI(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
																			printf("\n\n\t\t APL DML Cre:SVR No  is :%s",Part_no);	fflush(stdout);

																			if(TCTYPE_ask_object_type(AssyTag,&objTypeTag));
																			if(TCTYPE_ask_name2(objTypeTag,&type_name1));
																			printf("\n\n\t\t APL DML Cre:SVR type_name1 := %s", type_name1);fflush(stdout);

																			CALLAPI(AOM_ask_value_string(AssyTag,"object_type",&PartTypeStr));
																			printf("\n\n\t\t APL DML Cre:SVR PartTypeStr := %s", PartTypeStr);fflush(stdout);
																			 */

																			if(AssyTag != NULLTAG && APLTaskRevTag != NULLTAG && tsk_part_sol_rel_type != NULLTAG)
																			{

																				CALLAPI(GRM_find_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type ,&Fndrelation));
																				if(Fndrelation)
																				{
																					printf("\n\t Relation Already Exist.\n" );fflush(stdout);
																				}
																				else
																				{
																				GRM_create_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type,  NULLTAG, &tsk_part_APL_rel);
																				GRM_save_relation(tsk_part_APL_rel);
																				}
																				printf("\n\n\t\t APL DML Cre:Object_PartType is :%s\n",PartTypeStr);fflush(stdout);

																			}
																		}
																	}
																} // End Check the respective ERC task only
															}
														}
														else if(strcmp(erc_rel_type,"UVF")==0)  // added by Deepti TZ1.54 for UVF
														{
															if(strcmp(object_type,"T5_ChangeTaskRevision")==0)
															{
																erc_item_id = NULL;
																AOM_ask_value_string( TaskRevTag, "item_id", &erc_item_id);
																if(strstr(item_id_dup,erc_item_id))  // Check the respective ERC task only
																{
																	PartCnt=0;
																	k=0;
																	CALLAPI(AOM_ask_value_tags(TaskRevTag,"T5_ModuleVF_to_Change",&PartCnt,&PartTags));
																	printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
																	if (PartCnt>0)
																	{
																		GRM_find_relation_type("T5_ModuleVF_to_Change",&tsk_part_sol_rel_type);
																		for (k=0;k<PartCnt ;k++ )
																		{
																			printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
																			AssyTag=PartTags[k];
																			
																			if(AssyTag != NULLTAG && APLTaskRevTag != NULLTAG && tsk_part_sol_rel_type != NULLTAG)
																			{

																				CALLAPI(GRM_find_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type ,&Fndrelation));
																				if(Fndrelation)
																				{
																					printf("\n\t Relation Already Exist.\n" );fflush(stdout);
																				}
																				else
																				{
																					GRM_create_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type,  NULLTAG, &tsk_part_APL_rel);
																					GRM_save_relation(tsk_part_APL_rel);
																				}
																				printf("\n\n\t\t APL DML Cre:Object_PartType is :%s\n",PartTypeStr);fflush(stdout);

																			}
																		}
																	}
																} // End Check the respective ERC task only
															}
														}
														else  //Start For Other than SVR
														{
															if(strcmp(object_type,"T5_ChangeTaskRevision")==0)
															{
																erc_item_id = NULL;
																AOM_ask_value_string( TaskRevTag, "item_id", &erc_item_id);
																if(strstr(item_id_dup,erc_item_id))  // Check the respective ERC task only
																{
																	PartCnt=0;
																	CALLAPI(AOM_ask_value_tags(TaskRevTag,"CMHasSolutionItem",&PartCnt,&PartTags));
																	printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
																	if (PartCnt>0)
																	{
																		GRM_find_relation_type("CMHasSolutionItem",&tsk_part_sol_rel_type);
																		for (k=0;k<PartCnt ;k++ )
																		{
																			printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
																			AssyTag=PartTags[k];

																			CALLAPI(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
																			printf("\n\n\t\t APL DML Cre:Part_no  is :%s",Part_no);	fflush(stdout);

																			if(TCTYPE_ask_object_type(AssyTag,&objTypeTag));
																			if(TCTYPE_ask_name2(objTypeTag,&type_name1));
																			printf("\n\n\t\t APL DML Cre:AssyTag type_name1 := %s", type_name1);fflush(stdout);		
																			
																			CALLAPI(POM_class_of_instance(AssyTag,&class_id));
																			CALLAPI(POM_name_of_class(class_id,&class_name));
																			printf("\n class_name is :%s\n",class_name);fflush(stdout);

																			CALLAPI(AOM_ask_value_string(AssyTag,"object_type",&PartTypeStr));
																			printf("\n\n\t\t APL DML Cre:AssyTag PartTypeStr := %s", PartTypeStr);fflush(stdout);

																			//if (tc_strcmp(type_name1,"Design Revision")==0 &&  (AssyTag != NULLTAG && APLTaskRevTag != NULLTAG && tsk_part_sol_rel_type != NULLTAG))//Commented in TZ 1.45
																			if ( (AssyTag != NULLTAG && APLTaskRevTag != NULLTAG && tsk_part_sol_rel_type != NULLTAG))
																			{

																				CALLAPI(GRM_find_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type ,&Fndrelation));
																				if(Fndrelation)
																				{
																					printf("\n\t Relation Already Exist.\n" );fflush(stdout);
																				}
																				else
																				{
																				GRM_create_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type,  NULLTAG, &tsk_part_APL_rel);
																				GRM_save_relation(tsk_part_APL_rel);
																				}
																				printf("\n\n\t\t APL DML Cre:Object_PartType is :%s\n",PartTypeStr);fflush(stdout);

																			}
																		}
																	}
															} // End Check the respective ERC task only
															}
														
														}// End For Other than SVR Release


													}
												
												}
												else
												{
													printf("\n item_id created twice......  \n");fflush(stdout);
												}										
											}
										}

									}
								}							
							}/// Task count							
							printf("\n cntTaskD[%d]......  \n",cntTaskD);fflush(stdout);
							//Check release type == CP, if yes then make relation colour svr relation with DML,
							//proceed for colour part generation,
							//Find the clr svr from erc dml --> references relation --> colour serial
							//Generate the colour painted body module and attached it to task.
							if (DMLTag != NULLTAG && APLDMLRevTag != NULLTAG)
							{								
								//Update Design Group List on APL DML								
								if(cntTaskD>0)
								{
									CALLAPI(AOM_lock(APLDMLRevTag));
									for(iDML=0;iDML<cntTaskD;iDML++)
									{
										ITKCALL(AOM_set_value_string_at(APLDMLRevTag,"t5_crdesigngroup",iDML,TaskDesignGroupStr[iDML]));
									}
									CALLAPI( AOM_save(APLDMLRevTag) );
									CALLAPI( AOM_unlock(APLDMLRevTag) );

									//Check control object for CP DML live.
									//If control object is life, then make the relation of clt svr and dml
									//create the APL colour part and make the attachement with APL task.
									//if (tc_strcpy(erc_rel_type,"CP")==0)//
									if (tc_strcmp(erc_rel_type,"CP")==0)
									{
										printf("\nCP DML found, check for control object and proceed for APL clr part generation...!!!");fflush(stdout);
										control_number_foundPrj		=	0;
										list_of_WSO_cntrlPrj_tags	=	NULLTAG;

										if(QRY_find("Control Objects...", &qryTagCntrlPB));
										if (qryTagCntrlPB!=NULLTAG)
										{
//											qry_entryCntrlPB[0]="PlantAPLCLRPRT";
//											qry_entryCntrlPB[1]="PlantAPLCLRPRT";
//											qry_entryCntrlPB[2]="0";
//											qry_entryCntrlPB[3]="APLC";		//Deepti TZ3.52

											qry_valuesCntrlPB[0]="PlantAPLCLRPRT";
											qry_valuesCntrlPB[1]="PlantAPLCLRPRT";
											qry_valuesCntrlPB[2]="0";
											qry_valuesCntrlPB[3]="APLC";


											//if(QRY_execute(qryTagCntrl, n_entryCntrl, qry_entryCntrlPB, qry_valuesCntrlPB, &control_number_foundPrj, &list_of_WSO_cntrlPrj_tags)); //Deepti TZ3.52
											if(QRY_execute(qryTagCntrlPB, n_entryCntrlPB, qry_entryCntrlPB, qry_valuesCntrlPB, &control_number_foundPrj, &list_of_WSO_cntrlPrj_tags));
											printf("\nNo of painted body control object found  Deepti...: %d",control_number_foundPrj);fflush(stdout);

											if (control_number_foundPrj > 0)
											{
												clrSvrCnt	=	0;
												CALLAPI(AOM_ask_value_tags(DMLTag,"CMReferences",&clrSvrCnt,&clrSvrTags));
												printf("\nNo of clr svr attached with DML : %d",clrSvrCnt);fflush(stdout);
												if (clrSvrCnt > 0)
												{
													GRM_find_relation_type("CMReferences",&dml_svr_sol_rel_type);
													if (clrSvrCnt>0)
													{
														for (k=0;k<clrSvrCnt ;k++ )
														{
															//AssyTag=PartTags[k];
															AssyTag=clrSvrTags[k];

															if(AssyTag != NULLTAG && APLDMLRevTag != NULLTAG && dml_svr_sol_rel_type != NULLTAG)
															{
																Fndrelation = NULLTAG;
																CALLAPI(GRM_find_relation(APLDMLRevTag, AssyTag, dml_svr_sol_rel_type ,&Fndrelation));
																if(Fndrelation!=NULLTAG)//Need to change in main file
																{
																	printf("\n\t Relation Already Exist.\n" );fflush(stdout);
																}
																else
																{
																	GRM_create_relation(APLDMLRevTag, AssyTag, dml_svr_sol_rel_type,  NULLTAG, &dml_svr_APL_rel);
																	GRM_save_relation(dml_svr_APL_rel);
																	printf("\n\n\t\t APL DML and clr svr relation created...!!!\n");fflush(stdout);
																	if (dml_svr_APL_rel!=NULLTAG)
																	{
																		//tm_apl_colour_part_generation(APLDMLRevTag);
																		printf("\nRelation created between svr and color APL DML...!!!");fflush(stdout);
																	}
																}
																//Add in to main file
//																Fndrelation = NULLTAG;
//																CALLAPI(GRM_find_relation(APLDMLTag, AssyTag, dml_svr_sol_rel_type ,&Fndrelation));
//																if(Fndrelation!=NULLTAG)//Need to change in main file
//																{
//																	tm_apl_colour_part_generation(APLDMLRevTag);
//																}
															}
														}
														printf("\nColor Painted body Generation start...!!!");fflush(stdout);
														//tm_apl_colour_part_generation(APLDMLRevTag); need to call after unit bom query

													}
												}
												else
												{
													printf("\nColour SVR not attached with DML, skip APL Colour part generation");fflush(stdout);
												}
												//Attach color scheme to DML
												clrSvrCnt	=	0;
												clrSvrTags	=	NULLTAG;
												CALLAPI(AOM_ask_value_tags(DMLTag,"IMAN_reference",&clrSvrCnt,&clrSvrTags));
												printf("\nNo of clr cheme attached with DML : %d",clrSvrCnt);fflush(stdout);
												if (clrSvrCnt > 0)
												{
													GRM_find_relation_type("IMAN_reference",&dml_svr_sol_rel_type);
													for (k=0;k<clrSvrCnt ;k++ )
													{
														char	*svrname			=	NULL;
														tag_t	AssyTag				=	NULLTAG;
														tag_t	Fndrelation			=	NULLTAG;
														AssyTag	=	clrSvrTags[k];
														//object_name
														CALLAPI(AOM_ask_value_string(AssyTag,"object_name",&svrname));
														printf("\nsvr name : %s",svrname);fflush(stdout);

														Fndrelation = NULLTAG;
														CALLAPI(GRM_find_relation(DMLTag, AssyTag, dml_svr_sol_rel_type ,&Fndrelation));

														if(Fndrelation!=NULLTAG)//Need to change here added NULLTAG
														{
															printf("\n\t Relation Already Exist.\n" );fflush(stdout);
															//Create Relation of APL DML and clr svr
															
														}
														else
														{
															printf("\n\t Relation Not Exist.\n" );fflush(stdout);
															GRM_create_relation(DMLTag, AssyTag, dml_svr_sol_rel_type,  NULLTAG, &dml_svr_APL_rel);
															GRM_save_relation(dml_svr_APL_rel);
															ITKCALL(AOM_refresh(dml_svr_APL_rel,0));
															printf("\n\n\t\t APL DML and clr svr relation created...!!!\n");fflush(stdout);
															//tm_apl_colour_part_generation(APLDMLTag);
														}
														
													}
												}
												//Color SVR and Color scheme attachement completed
												ITKCALL(QRY_find("Control Objects...", &qryTagCntrlUnitBom));
												if (qryTagCntrlUnitBom!=NULLTAG)
												{
													qry_valuesCntrlUnitBOM[0]="UnitBOMProj";
													qry_valuesCntrlUnitBOM[1]="APLClrPart";
													qry_valuesCntrlUnitBOM[2]="0";
													qry_valuesCntrlUnitBOM[3]=Proj_Code;

													if(QRY_execute(qryTagCntrlUnitBom, n_entryCntrlUnitBOM, qry_entryCntrlUnitBOM, qry_valuesCntrlUnitBOM, &control_number_foundUnitBOM, &list_of_WSO_cntrl_tagsUnitBOM));
													printf("\n\n\t\t control_number_foundUnitBOM =:%d",control_number_foundUnitBOM);fflush(stdout);
													if(control_number_foundUnitBOM > 0)
													{
														//Below point confirm by Rahul Patil
														//Painted body is generated at ERC, so no need to generate at APL
														//tm_apl_unit_bom_colour_part_generation(APLDMLRevTag);
													}
													else
													{
														tm_apl_colour_part_generation(APLDMLRevTag); //Hemal TZ1.52
													}
												}

											}
											else
											{
												printf("\nControl object not live procceed for submission process of DML");fflush(stdout);
											}

										}
									}

									//Submit DML into Workflow
									tag_t template_tag = NULLTAG;
									tag_t test_job_a = NULLTAG;
									int attachment_types = 1;
									printf("\n Finding CM Release Process Template\n");fflush(stdout);
									printf("\n erc_rel_type1111: %s",erc_rel_type);fflush(stdout);
									
									//if dml type is WT then submit it into STD LC
									if(tc_strcmp(erc_rel_type,"WT")==0)
									{
										CALLAPI(EPM_find_template2("STDSI DML WorkFlow",0,&template_tag));
										if (template_tag!=NULLTAG)
										{
											printf("\n Submit APL DML of release type WT in to STDSI DML WorkFlow\n");fflush(stdout);
											CALLAPI(EPM_create_process("STD DML Process","",template_tag,1, &APLDMLRevTag,&attachment_types,&test_job_a));
											printf("\n Submit STD DML in to workflow process complete\n");fflush(stdout);
										}
									}
									else
									{
										
										CALLAPI(EPM_find_template2("ECN APL Workflow",0,&template_tag));
										if (template_tag!=NULLTAG)
										{
											printf("\n Submit APL DML in to workflow\n");fflush(stdout);
											CALLAPI(EPM_create_process("APL DML Process","",template_tag,1, &APLDMLRevTag,&attachment_types,&test_job_a));
											printf("\n Submit APL DML in to workflow process complete\n");fflush(stdout);
										}
									}
								}
							}
						}
						else
						{
							printf("\n******APL DML should not be created******\n");fflush(stdout);
						}

					}
					else
					{
						printf("\nNO PROCEED******\n");fflush(stdout);

					}
				}//if(apl_number_found == 0)
				else
				{
					printf("\n**********APL DML :[%s] already exist******\n",item_erc_id_dup);fflush(stdout);
				}

			//}//if((strcmp(subSyscd,Proj_Code)==0) && (delInd==0))		
		}
	  } // if(FlagGotFrmToplt!=2)
	}
	else
    {
		printf("\n No proper class class T5_APLDMLRevision......\n");fflush(stdout);
    }

	return error_code;
}


extern int ITK_user_main( int argc, char **argv )
{
	//VARIABLE DECLARATION STARTS
	int     status;
	int     ifail;
	
	char	*PlantCS				=	NULL;
	char	*item_type				=	NULL;
	char	*type_name				=	NULL;
	char	*req_item				=	NULL;
	char	*cUserName				=	NULL;
	char	*cPassWord				=	NULL;
	char	*cUserGrp				=	NULL;
	char	*ClosureRule			=	NULL;
	char	*revStr					=	NULL;
	char	*revStr1				=	NULL;
	char	*tmpStr					=	NULL;
	char	*FromDate					=	NULL;
	char	*erc_rel_type_check					=	NULL;
	char	*item_id					=	NULL;
	char	*Proj_Code					=	NULL;
	tag_t tag_query;
  	int n_entries=2;
  	int n_tags_found=0;
  	int itmRCnt=0;

	tag_t	task_tag				=	NULLTAG;
	tag_t	tsk_rev_tag				=	NULLTAG;
	tag_t	objTypeTag				=	NULLTAG;
	char *entries[2] = {"Name","date_released_after"};
		char **values =	(char **) MEM_alloc(10 * sizeof(char *));
	tag_t *tags_found = NULL;
	FILE* fperror=NULL;
	FromDate = ITK_ask_cli_argument("-i=");

	//VARIABLE DECLARATION ENDS
	ITK_CALL(ITK_auto_login());  

    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	ITK_CALL(ITK_set_journalling( TRUE ));
	
	values[0]="T5_LcsErcRlzd";
	//values[0]="ERC Released";
	values[1] = (char*)FromDate;
	
	ITKCALL(QRY_find("ERCDMLReleased",&tag_query));
	printf("Searching...\n");fflush(stdout);
	ITKCALL(QRY_execute(tag_query,n_entries, entries, values,&n_tags_found,&tags_found));
	printf("n_tags_found...%d\n",n_tags_found);fflush(stdout);
	if(n_tags_found==0)
	{
		printf("DMLfound :::%d",n_tags_found);fflush(stdout);

	}
	else
	{
		printf("n_tags_found:::%d",n_tags_found);fflush(stdout);
		for(itmRCnt=0;itmRCnt<n_tags_found;itmRCnt++)
		{
			tsk_rev_tag = tags_found[itmRCnt];
		
			if(tsk_rev_tag != NULLTAG)
			{
			
				if(TCTYPE_ask_object_type(tsk_rev_tag,&objTypeTag));
				if(TCTYPE_ask_name2(objTypeTag,&type_name));
				printf("\n type_name : %s",type_name);fflush(stdout);

				if(strcmp(type_name,"ChangeRequestRevision")==0)
				{
					
					AOM_ask_value_string( tsk_rev_tag, "t5_rlstype", &erc_rel_type_check);
					AOM_ask_value_string( tsk_rev_tag, "item_id", &item_id);
					AOM_ask_value_string( tsk_rev_tag, "t5_cprojectcode", &Proj_Code);

					if(strcmp(Proj_Code,"5442")==0 ||(strcmp(Proj_Code,"5445")==0))
					{

						if((strcmp(erc_rel_type_check,"SVR")==0 || strcmp(erc_rel_type_check,"TODR")==0  ) || strcmp(erc_rel_type_check,"SSR")==0  )   // added by Dhanashri TZ1.45 for TODR
						{
							printf("\nBefore apl_dml_signoff_checks_Func.....DML %s is getting Processed for APLDML Creation",item_id);fflush(stdout);
							apl_dml_create( tsk_rev_tag );
							//if(itmRCnt==10)break;
							printf("\nAfter apl_dml_signoff_checks_Func...");fflush(stdout);
						}
					}
				}
			
			}
		}
	
	}
	

	printf("\nCode ends...");fflush(stdout);
	return status;

}