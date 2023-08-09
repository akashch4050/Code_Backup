/**********************************************************************************************************************
* 
* Copyright (c) 2004 Tata Technologies Limited (TTL). All Rights
* Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
* Program Name : TCUA_DML_Downloader.c
* Author : Hemal Sukhanandi
* Created On : 16/08/2018
* Project : DML DOwnload for TCE APL
*	54420024510005
* 
* Purpose : Download DML Metadata, Task Metadata, Attched Parts, First Level structure
************************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sa/sa.h>
#include <sys/stat.h>
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
#include <stdlib.h>
#include <string.h>
#include <fclasses/tc_string.h>
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
#include <itk/mem.h>
#include <pom/pom/pom_tokens.h>
#include <unidefs.h>
#include <property/prop.h>
#include <property/propdesc.h>
#include <tccore/tc_msg.h>
#include <lov/liblov_exports.h>
#include <lov/liblov_undef.h>
#include <ug_va_copy.h>
#include <tccore/grm.h>
#define CHECK_FAIL if (ifail != 0) { printf("line %d (ifail %d)\n", __LINE__, ifail); return 0;}
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

extern int ITK_user_main (int argc, char ** argv )
{

	int			status;

	char		*req_item		=	NULL;
	char		*FilePath		=	NULL;
	char		*File_Dml		=	NULL;

	req_item = ITK_ask_cli_argument("-i=");
	FilePath = ITK_ask_cli_argument("-Path=");

	//Initialize Texe Service
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_auto_login( ));
    ITK_CALL(ITK_set_journalling( TRUE ));
	


	//File_Dml=(char *) MEM_alloc(200 * sizeof(char ));


	//strcpy(File_Dml,FilePath);
	//strcat(File_Dml,req_item);
	//strcat(File_Dml,"/");
	//strcat(File_Dml,req_item);
	//strcat(File_Dml,"_Dml.txt");

	
	if (req_item!=NULL)
	{
		Complete_APLDML_Download(req_item,FilePath);
	}


	printf("\n MAIN FUNCTION : : FINISH\n");
	return status;
}

int Complete_APLDML_Download(char* DmlIdS, char* FilePath)
{
	int		status					= 0;

	FILE	*DML_Download_fp		=	NULL;
	
	char	*DMLFile				= NULL;
	char	*AllDmlAttrS			= NULL;
	char	*AllTaskAttrS			= NULL;
	char	*AllTaskAttrDupS		= NULL;

	char	*item_type				= NULL;
	char	*DML_Number				= NULL;
	char	*DML_Desc				= NULL;
	char	*DML_Name				= NULL;
	char	*DML_DescR				= NULL;
	char	*DML_Project			= NULL;
	char	*object_descS			= NULL;
	char	*CMDispositionS			= NULL;
	char	*DmlReview				= NULL;
	char	*requestor_user_idS		= NULL;
	char	*analyst_user_idS		= NULL;
	char	*CMClosureS				= NULL;
	char	*DMLtype				= NULL;
	char	*DRStatus				= NULL;
	char	*ClosureTmStmp			= NULL;
	char	*CMClosureDateS			= NULL;
	char	*ClosureTmStmpFinal		= NULL;
	char	*DMLClosureStmp			= NULL;

	char	*TaskItemIdS			= NULL;
	char	*TaskObjectDescS		= NULL;
	char	*DesignGroupS			= NULL;
	char	*TaskDespositionS		= NULL;
	char	*RequestorS				= NULL;
	char	*TaskAnalystUserIdS		= NULL;

	char	*PartNumberS			= NULL;
	char	*RevisionS				= NULL;
	char	*PrtRev					= NULL;
	char	*SequenceS				= NULL;
	char	*PartTypeS				= NULL;
	char	*PartSTDPrtIndS			= NULL;
	char	*PartWeightS			= NULL;
	char	*PartDRStS				= NULL;
	char	*PartDrawIndS			= NULL;
	char	*PartOwnrGrpS			= NULL;
	char	*PartDsgnDptS			= NULL;
	char	*PartDsgnOwnS			= NULL;
	char	*PartCARIntAgncyS		= NULL;
	char	*PartCARMakeByIndS		= NULL;
	char	*PartCARSlocS			= NULL;

	char	*ReviewName				= NULL;
	char	*ReviewId				= NULL;
	char	*Creator				= NULL;
	char	*CreatorName			= NULL;
	char	*CreatorId				= NULL;
	
	int		n_tasks				= 0;
	int		n_parts				= 0;
	int		st_count1			= 0;
	int		iTsk				= 0;
	int		iPrt				= 0;
	int		ists				= 0;
	int     CTSmonth			= 0;          /* Month */
    int     CTSday				= 0;          /* Day */
    int     CTSyear				= 0;          /* Year */
    int     CTShour				= 0;          /* Hour */
    int     CTSminute			= 0;          /* Minutes */
    int     CTSsecond			= 0;          /* Seconds */
	int      month				= 0;          /* Month */
    int      day				= 0;          /* Day */
    int      year				= 0;          /* Year */
    int      hour				= 0;          /* Hour */
    int      minute				= 0;          /* Minutes */
    int      second				= 0;          /* Seconds */

	tag_t	dml_tag					=	NULLTAG;
	tag_t	dml_rev_tag				=	NULLTAG;
	tag_t	reln_type_tag			=	NULLTAG;
	tag_t	*task_obj_tag			=	NULLTAG;
	tag_t	*part_obj_tag			=	NULLTAG;
	tag_t	*status_list1			=	NULLTAG;
	tag_t	task_rev_tag			=	NULLTAG;
	tag_t	part_rev_tag			=	NULLTAG;

	printf("\nInside Complete_APLDML_Download...!!!");fflush(stdout);

	DMLFile=(char *) MEM_alloc(200 * sizeof(char ));

	AllDmlAttrS = ( char * ) MEM_alloc(600);
	
	DMLClosureStmp = ( char * ) MEM_alloc(30);
	ClosureTmStmpFinal = ( char * ) MEM_alloc(30);

	ReviewName = (char *) MEM_alloc( 200 * sizeof(char) );
	ReviewId = (char *) MEM_alloc( 200 * sizeof(char) );

	printf("\nFile Name : %s",FilePath);fflush(stdout);

	//MAKE FOLDER
	//ITK_CALL(system("mkdir %s",FilePath));
	if((mkdir(FilePath,00777))==-1) {
           printf("\nDir Not Created successfully...!!!");fflush(stdout);
     }
     else
     {
           printf("\nDir Created successfully...!!!");fflush(stdout);
     }

	
	AllDmlAttrS=strcpy(AllDmlAttrS,"");

	ITK_CALL(ITEM_find_item(DmlIdS,&dml_tag));

	if (dml_tag != NULLTAG)
	{
		printf("\n\n Complete_APLDML_Download : DML found ! ! ! \n\n");  fflush(stdout);
		printf("\n\n Complete_APLDML_Download : [%s] \n\n",AllDmlAttrS);  fflush(stdout);

		//Find Item Type
		ITK_CALL(ITEM_ask_type2(dml_tag,&item_type));
		printf("\n Complete_APLDML_Download : item_typeS [%s] \n",item_type);  fflush(stdout);
	
		
		//ITK_CALL(ITEM_find_revision(dml_tag,"A",&dml_rev_tag)) ;
		
		if (dml_tag  != NULLTAG)
		{
			//"Design Revision"
			ITK_CALL(ITEM_ask_latest_rev(dml_tag,&dml_rev_tag));
			item_type				= NULL;
			ITK_CALL(ITEM_ask_rev_type2(dml_rev_tag,&item_type));
			printf("\n Complete_APLDML_Download : item_typeS 12 [%s] \n",item_type);  fflush(stdout);
			if (tc_strcmp(item_type,"Design Revision")==0)
			{
				strcpy(DMLFile,FilePath);
				//strcat(File_Dml,req_item);
				strcat(DMLFile,"/");
				strcat(DMLFile,DmlIdS);
				strcat(DMLFile,"_APLPART.txt");

				DML_Download_fp=fopen(DMLFile,"w");
				if(DML_Download_fp==NULL)
				{
					printf("\nError in opening the file \n");
				}
				fflush(DML_Download_fp);
				printf("\n");fflush(stdout);

				part_rev_tag	=	dml_rev_tag;
				if (part_rev_tag!=NULLTAG)
				{
					APLPart_Download(part_rev_tag,FilePath,DmlIdS);
				}
				else
				{
					printf("\nPart not found, please enter valid part number");fflush(stdout);
				}
				
			}
			else if(tc_strcmp(item_type,"T5_APLTaskRevision")==0)
			{
				//Find Item revision
				ITK_CALL(ITEM_find_revision(dml_tag,"NR",&dml_rev_tag)) ;

				strcpy(DMLFile,FilePath);
				//strcat(File_Dml,req_item);
				strcat(DMLFile,"/");
				strcat(DMLFile,DmlIdS);
				strcat(DMLFile,"_Dml.txt");

				DML_Download_fp=fopen(DMLFile,"w");
				if(DML_Download_fp==NULL)
				{
					printf("\nError in opening the file \n");
				}
				fflush(DML_Download_fp);
				//T5_APLTaskRevision
				printf("\n Complete_APLDML_Download : item_typeS [%s] \n",item_type);  fflush(stdout);

				//1	DML NUMBER - item_id
				//AOM_ask_value_string --> return value of single property 
				ITK_CALL(AOM_ask_value_string(dml_rev_tag,"item_id",&DML_Number));
				printf("\nDML NUMBER : [%s]",DML_Number);fflush(stdout);
				if (DML_Number!=NULL)
					tc_strcat(AllDmlAttrS,DML_Number);
				tc_strcat(AllDmlAttrS,",");

				//2	DML NAME
				ITK_CALL(AOM_ask_value_string(dml_rev_tag,"object_name",&DML_Name));
				printf("\n Complete_APLDML_Download : DML_Name : %s \n",DML_Name); fflush(stdout);

				STRNG_replace_str(DML_Name,",",":",&DML_DescR);
				printf("\n Complete_APLDML_Download : DML_DescR : %s \n",DML_DescR); fflush(stdout);

				if (DML_DescR)
					tc_strcat(AllDmlAttrS,DML_DescR);
				tc_strcat(AllDmlAttrS,",");

				//3	Project Code
				ITK_CALL(AOM_ask_value_string(dml_rev_tag,"t5_cprojectcode",&DML_Project));
				if (DML_Project)
					tc_strcat(AllDmlAttrS,DML_Project);
				tc_strcat(AllDmlAttrS,",");
				
				//Design Group
				char *DesignGroupList=NULL;
				//int num =0;
				ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"t5_crdesigngroup",&DesignGroupList));
				printf("\n Complete_APLDML_Download : t5designgroup : %s , %d\n",DesignGroupList); fflush(stdout);

				if (DesignGroupList) 
					strcat(AllDmlAttrS,DesignGroupList);
				strcat(AllDmlAttrS,",");

				//5 Desc
				ITK_CALL(AOM_ask_value_string(dml_rev_tag,"object_desc",&object_descS));
				printf("\n Complete_APLDML_Download : object_desc : %s\n",object_descS); fflush(stdout);
				if (object_descS)
					tc_strcat(AllDmlAttrS,object_descS);
				tc_strcat(AllDmlAttrS,",");

				//Disposition
				ITK_CALL(AOM_ask_value_string(dml_rev_tag,"CMDisposition",&CMDispositionS));
				printf("\n Complete_APLDML_Download : CMDispositionS : %s\n",CMDispositionS); fflush(stdout);
				if (CMDispositionS)
					tc_strcat(AllDmlAttrS,CMDispositionS);
				tc_strcat(AllDmlAttrS,",");

				//7 Authorization By is not Present in UA
				tc_strcat(AllDmlAttrS,"APLAna1");
				tc_strcat(AllDmlAttrS,",");

				//8 Change Review Board in TCUA
				ITK_CALL (AOM_UIF_ask_value(dml_rev_tag,"ChangeReviewBoard",&DmlReview));
				printf("\n Complete_DML_Download : DmlReview : %s \n",DmlReview); fflush(stdout);
				
				ReviewName = strtok(DmlReview,"(");
				ReviewId= strtok(NULL,")");
				if (ReviewId)
					tc_strcat(AllDmlAttrS,ReviewId);
				tc_strcat(AllDmlAttrS,",");

				//9
				tc_strcat(AllDmlAttrS,"");
				tc_strcat(AllDmlAttrS,",");

				//10 DML Owner tmp

				tc_strcat(AllDmlAttrS,"APLAna1");	//temp value
				tc_strcat(AllDmlAttrS,",");

				//11 Creator not present in TCUA
				ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"owning_user",&Creator));
				printf("\n Complete_DML_Download : Creator : %s \n",Creator); fflush(stdout);
				CreatorName = strtok(Creator,"(");
				CreatorId= strtok(NULL,")");
				if (CreatorId)
					tc_strcat(AllDmlAttrS,CreatorId);
				tc_strcat(AllDmlAttrS,",");

				//12 DML Requestor
				ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"requestor_user_id",&requestor_user_idS));
				printf("\n Complete_DML_Download : requestor_user_id : %s \n",requestor_user_idS); fflush(stdout);
				if (requestor_user_idS)               
					tc_strcat(AllDmlAttrS,requestor_user_idS);
				tc_strcat(AllDmlAttrS,",");

				//13 DML Analyst
				ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"analyst_user_id",&analyst_user_idS));
				printf("\n Complete_DML_Download : analyst_user_id : %s \n",analyst_user_idS); fflush(stdout);
				if (analyst_user_idS)               
					tc_strcat(AllDmlAttrS,analyst_user_idS);
				tc_strcat(AllDmlAttrS,",");

				//14 Administrator not present in TCUA

				tc_strcat(AllDmlAttrS,"");
				tc_strcat(AllDmlAttrS,",");

				//15 Planning Status not present in TCUA
				tc_strcat(AllDmlAttrS,"");
				tc_strcat(AllDmlAttrS,",");

				//16 DML closure
				ITK_CALL(AOM_ask_value_string(dml_rev_tag,"CMClosure",&CMClosureS)!=ITK_ok);
				printf("\n Complete_DML_Download : CMClosure : %s \n",CMClosureS); fflush(stdout);
				if (CMClosureS)               
					tc_strcat(AllDmlAttrS,CMClosureS);//CcfPlanClosed in TCE,Closed in TCUA
				tc_strcat(AllDmlAttrS,",");

				//17 DML Reason
				tc_strcat(AllDmlAttrS,"");
				tc_strcat(AllDmlAttrS,",");
				
				//18 DML Type ,object_type confirm once ???????
				if (tc_strstr(DML_Number,"AM")!=NULL)
				{
					ITK_CALL(AOM_ask_value_string(dml_rev_tag,"t5_EcnType",&DMLtype));
					printf("\n Complete_DML_Download : object_type : %s \n",DMLtype); fflush(stdout);
				}
				else
				{
					ITK_CALL(AOM_ask_value_string(dml_rev_tag,"t5_rlstype",&DMLtype));
					printf("\n Complete_DML_Download : object_type : %s \n",DMLtype); fflush(stdout);
				}
				if (DMLtype)tc_strcat(AllDmlAttrS,DMLtype);
				tc_strcat(AllDmlAttrS,",");

				//19 DML Release Letter Ref
				tc_strcat(AllDmlAttrS,"");
				tc_strcat(AllDmlAttrS,",");

				//20 DML Release notes
					tc_strcat(AllDmlAttrS,"");
				tc_strcat(AllDmlAttrS,",");

				//21 DML Addenda
				//if (AOM_ask_value_string(dml_rev_tag,"t5_basicdml",&t5_basicdmlNum)!=ITK_ok)   PrintErrorStack();
				//printf("\n Complete_DML_Download : t5_basicdml : %s \n",t5_basicdmlNum); fflush(stdout);
				tc_strcat(AllDmlAttrS,"");
				tc_strcat(AllDmlAttrS,",");

				//22 DML Part status (DR status)
				ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"t5_cDRstatus",&DRStatus));
				printf("\n Complete_DML_Download : DRStatus : %s \n",DRStatus); fflush(stdout);
				if(DRStatus)
					tc_strcat(AllDmlAttrS,DRStatus);
				tc_strcat(AllDmlAttrS,",");

				//23 DML Closure Date
				ITK_CALL(AOM_UIF_ask_value(dml_rev_tag,"date_released",&ClosureTmStmp));
				printf("\n Complete_DML_Download : date_released : %s \n",ClosureTmStmp); fflush(stdout);

				if (ClosureTmStmp!=NULL)
				{
					DATE_string_to_date(ClosureTmStmp , "%d-%b-%Y %H:%M:%S", &CTSmonth , &CTSday ,&CTSyear , &CTShour , &CTSminute , &CTSsecond);
					printf("\nClosureTmStmpFinal----><%s> %d-%d-%d:%d:%d:%d\n",ClosureTmStmpFinal,CTSday,CTSmonth,CTSyear,CTShour,CTSminute,CTSsecond);
				}

				CMClosureDateS = ( char * ) MEM_alloc(30);
				sprintf(CMClosureDateS,"%d/%d/%d",CTSyear,CTSmonth,CTSday);
				printf("\n CMClosureDateS : %s \n",CMClosureDateS); fflush(stdout);
				tc_strcat(AllDmlAttrS,CMClosureDateS);

				tc_strcat(AllDmlAttrS,",");

				//24 Change review Board in TCUA 

				ITK_CALL (AOM_UIF_ask_value(dml_rev_tag,"ChangeReviewBoard",&DmlReview)!=ITK_ok);
				printf("\n Complete_DML_Download : DmlReview : %s \n",DmlReview); fflush(stdout);
				ReviewName = strtok(DmlReview,"(");
				printf("\n ReviewName : %s \n",ReviewName); fflush(stdout);
				ReviewId= strtok(NULL,")");
				printf("\n ReviewId : %s \n",ReviewId); fflush(stdout);
				if (ReviewId)               
					tc_strcat(AllDmlAttrS,ReviewId);
				else
					tc_strcat(AllDmlAttrS,"");
				tc_strcat(AllDmlAttrS,",");

				//25 Closure time stamp
				//TCE : 2018/02/26-12:35:43:503

				sprintf(DMLClosureStmp,"%d/%d/%d-%d:%d:00:000",CTSyear,CTSmonth,CTSday,CTShour,CTSminute);
				printf("\n kk.DMLClosureStmp: %s \n",DMLClosureStmp); fflush(stdout);
				if (DMLClosureStmp)     
					tc_strcat(AllDmlAttrS,DMLClosureStmp);
				tc_strcat(AllDmlAttrS,",");

				printf("\n Complete_DML_Download : AllDmlAttrS : [%s] \n",AllDmlAttrS); fflush(stdout);

				//DML Details Complete

				// Expand the DML-Revision using relation "T5_DMLTaskRelation" to get Task tag.
				ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation",&reln_type_tag));

				if (reln_type_tag	!=	NULLTAG)
				{
					ITK_CALL(GRM_list_secondary_objects_only( dml_rev_tag, reln_type_tag, &n_tasks, &task_obj_tag ))
					printf("\nNo of Task Found : %d",n_tasks);fflush(stdout);

					if (n_tasks>0)
					{
						for (iTsk=0;iTsk<n_tasks ;iTsk++ )
						{
							task_rev_tag	=	task_obj_tag[iTsk];

							AllTaskAttrS = (  char * )MEM_alloc(600);

							AllTaskAttrS=strcpy(AllTaskAttrS,AllDmlAttrS);
							tc_strcat(AllTaskAttrS,"#");

							// 1 task number
							ITK_CALL(AOM_ask_value_string(task_rev_tag,"item_id",&TaskItemIdS));
							printf("\n Complete_APLDML_Download : Task-Rev item_id : %s \n",TaskItemIdS); fflush(stdout);
							if (TaskItemIdS)               
								tc_strcat(AllTaskAttrS,TaskItemIdS);
							tc_strcat(AllTaskAttrS,"#");
							
							//2 task desc
							ITK_CALL(AOM_UIF_ask_value(task_rev_tag,"object_desc",&TaskObjectDescS));
							printf("\n Complete_APLDML_Download : Task-Rev TaskObjectDescS : %s \n",TaskObjectDescS); fflush(stdout);
							if (TaskObjectDescS == NULL)               
								tc_strcat(AllTaskAttrS,TaskObjectDescS);
							tc_strcat(AllTaskAttrS,"#");
							
							// 3 task design grp
							ITK_CALL(AOM_UIF_ask_value(task_rev_tag,"t5designgroup",&DesignGroupS));
							printf("\n Complete_APLDML_Download : Task-Rev DesignGroupS : %s \n",DesignGroupS); fflush(stdout);

							if (DesignGroupS)               
								strcat(AllTaskAttrS,DesignGroupS);
							strcat(AllTaskAttrS,"#");

							//4 LCS of Task sepreated by - 
							//Function call
							
							strcat(AllTaskAttrS,"#");
							
							// 5 TaskStatus is not present in TCUA.
							strcat(AllTaskAttrS,"");
							strcat(AllTaskAttrS,"#");

							// 6 task disposition
							ITK_CALL(AOM_ask_value_string(task_rev_tag,"CMDisposition",&TaskDespositionS));
							printf("\n Complete_DML_Download : Task-Rev TaskDespositionS : %s \n",TaskDespositionS); fflush(stdout);
							if (TaskDespositionS)               
								strcat(AllTaskAttrS,TaskDespositionS);
							strcat(AllTaskAttrS,"#");

							// 7 Creator is not present in TCUA.


							strcat(AllTaskAttrS,CMClosureDateS);
							strcat(AllTaskAttrS,"#");

							// 8 task requestor
							ITK_CALL(AOM_ask_value_string(task_rev_tag,"requestor_user_id",&RequestorS));
							printf("\n Complete_DML_Download : Task-Rev RequestorS : %s \n",RequestorS); fflush(stdout);
							if (RequestorS)               
								strcat(AllTaskAttrS,RequestorS);
							strcat(AllTaskAttrS,"#");

							// 9 task analyst
							ITK_CALL(AOM_ask_value_string(task_rev_tag,"analyst_user_id",&TaskAnalystUserIdS));
							printf("\n Complete_DML_Download : Task-Rev TaskAnalystUserIdS : %s \n",TaskAnalystUserIdS); fflush(stdout);
							if (TaskAnalystUserIdS)               
								strcat(AllTaskAttrS,TaskAnalystUserIdS);
							strcat(AllTaskAttrS,"#");
							
							//10 task admin
								strcat(AllTaskAttrS,"");
							strcat(AllTaskAttrS,"#");
						
							AllTaskAttrDupS = strdup(AllTaskAttrS);

							printf("\n Complete_DML_Download : AllTaskAttrS : %s \n",AllTaskAttrS); fflush(stdout);
							printf("\n Complete_DML_Download : AllTaskAttrDupS : %s \n",AllTaskAttrDupS); fflush(stdout);

							// Expand the Task-Revision using CMHasSolutionItem to get the TC Part.

							reln_type_tag = NULLTAG;
							ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&reln_type_tag));

							if (reln_type_tag	!=	NULLTAG)
							{
								n_parts			=	0;
								part_obj_tag	=	NULLTAG;
								ITK_CALL(GRM_list_secondary_objects_only( task_rev_tag, reln_type_tag, &n_parts, &part_obj_tag ));

								printf("\nNo of Part Found : %d",n_parts);fflush(stdout);

								if (n_parts>0)
								{
									for (iPrt=0;iPrt<n_parts ;iPrt++ )
									{
										part_rev_tag	=	part_obj_tag[iPrt];
										printf("\nNo of Part Found : %d",n_parts);fflush(stdout);
										fflush(DML_Download_fp);
										printf("\nNo of Part Found : %d",n_parts);fflush(stdout);
										fprintf(DML_Download_fp,AllTaskAttrDupS);fflush(DML_Download_fp);
										//1 Part Number
																			
										fprintf(DML_Download_fp,"$");
										PartNumberS	=	NULL;
										ITK_CALL(AOM_ask_value_string(part_rev_tag,"item_id",&PartNumberS));
										printf("\nPart Number : %s",PartNumberS);fflush(stdout);
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,PartNumberS);

										//2 Revision
										RevisionS	=	NULL;
										ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"item_revision_id",&RevisionS));
										if(RevisionS!=NULL) PrtRev=strtok(RevisionS,";");
										printf("\nPart PrtRev : %s",PrtRev);fflush(stdout);
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,PrtRev);

										//3 Sequence
										SequenceS	=	NULL;
										ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"sequence_id",&SequenceS));
										printf("\nPart SequenceS : %s",SequenceS);fflush(stdout);
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,SequenceS);

	//									//4 Part Type - t5_PartType
	//									PartTypeS	=	NULL;
	//									ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_PartType",&PartTypeS));
	//									printf("\nPart PartTypeS : %s",PartTypeS);fflush(stdout);
	//									fprintf(DML_Download_fp,"$");
	//									fprintf(DML_Download_fp,PartTypeS);
	//
	//									//5 STD Part Indicator - t5_StdPartIndicator
	//									PartSTDPrtIndS	=	NULL;
	//									ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_StdPartIndicator",&PartSTDPrtIndS));
	//									printf("\nPart PartSTDPrtIndS : %s",PartSTDPrtIndS);fflush(stdout);
	//									fprintf(DML_Download_fp,"$");
	//									fprintf(DML_Download_fp,PartSTDPrtIndS);
	//
	//									//6 Weight - t5_Weight
	//									PartWeightS	=	NULL;
	//									ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_Weight",&PartWeightS));
	//									printf("\nPart PartWeightS : %s",PartWeightS);fflush(stdout);
	//									fprintf(DML_Download_fp,"$");
	//									fprintf(DML_Download_fp,PartWeightS);
	//
	//									//DR Status - t5_PartStatus
	//									PartDRStS	=	NULL;									
	//									ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_PartStatus",&PartDRStS));
	//									printf("\nPart PartDRStS : %s",PartDRStS);fflush(stdout);
	//									fprintf(DML_Download_fp,"$");
	//									fprintf(DML_Download_fp,PartDRStS);
	//
	//									//Drawing Ind - t5_DrawingInd
	//									PartDrawIndS	=	NULL;
	//									ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_DrawingInd",&PartDrawIndS));
	//									printf("\nPart PartDrawIndS : %s",PartDrawIndS);fflush(stdout);
	//									fprintf(DML_Download_fp,"$");
	//									fprintf(DML_Download_fp,PartDrawIndS);
	//
	//									//Owner Design Dept - t5_DsgnDept
	//									PartDsgnDptS	=	NULL;
	//									ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_DsgnDept",&PartDsgnDptS));
	//									printf("\nPart PartDsgnDptS : %s",PartDsgnDptS);fflush(stdout);
	//									fprintf(DML_Download_fp,"$");
	//									fprintf(DML_Download_fp,PartDsgnDptS);
	//
	//									//Design Owner - t5_DsgnOwn
	//									PartDsgnOwnS	=	NULL;
	//									ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_DsgnOwn",&PartDsgnOwnS));
	//									printf("\nPart PartDsgnOwnS : %s",PartDsgnOwnS);fflush(stdout);
	//									fprintf(DML_Download_fp,"$");
	//									fprintf(DML_Download_fp,PartDsgnOwnS);

										//t5_CarIntialAgency
										PartCARIntAgncyS	=	NULL;
										ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_CarIntialAgency",&PartCARIntAgncyS));
										printf("\nPart t5_CarIntialAgency : %s",PartCARIntAgncyS);fflush(stdout);
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,PartCARIntAgncyS);

										//t5_CarMakeBuyIndicator
										PartCARMakeByIndS	=	NULL;
										ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_CarMakeBuyIndicator",&PartCARMakeByIndS));
										printf("\nPart PartCARMakeByIndS : %s",PartCARMakeByIndS);fflush(stdout);
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,PartCARMakeByIndS);


										//t5_CarStoreLocation
										PartCARSlocS	=	NULL;
										ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_CarStoreLocation",&PartCARSlocS));
										printf("\nPart PartCARSlocS : %s",PartCARSlocS);fflush(stdout);
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,PartCARSlocS);
										
										//LCS & EFFECTIVITY DATE
										st_count1	=	0;
										status_list1=	NULLTAG;
										ITK_CALL(WSOM_ask_release_status_list(part_rev_tag,&st_count1,&status_list1));
										printf("\nStatus Count : %d",st_count1);fflush(stdout);
										if (st_count1 > 0)
										{
											//if apl released
											char	*WSO_Name	=	NULL;
											int			stsFnd		=	0;

											tag_t		t_status	=	NULLTAG;
											for (ists=0;ists<st_count1 ;ists++ )
											{
												t_status	=	NULLTAG;
												t_status	=	status_list1[ists];

												ITK_CALL(AOM_ask_name(t_status,&WSO_Name));
												printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);

												if (tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0 || tc_strcmp(WSO_Name,"APL Released")==0)
												{
													printf("\nAPL Released Found",WSO_Name);fflush(stdout);
													stsFnd++;
													break;
												}
											}
											if (stsFnd>0)
											{
												//find the effectivity
												char* effectivityStr	=	NULL;
												ITK_CALL(AOM_ask_value_string(t_status,"effectivity_text",&effectivityStr));
												if (effectivityStr!=NULL)
												{
													char	*effectivityStrFinal	=	NULL;
													effectivityStrFinal = ( char * ) MEM_alloc(30);
													effectivityStrFinal = strtok(effectivityStr," ");
													DATE_string_to_date ( effectivityStrFinal , "%d-%b-%Y", &month , &day ,&year , &hour , &minute , &second);
													printf("\effectivityStrFinal----><%s> %d-%d-%d\n",effectivityStrFinal,day,month,year);
													month++;
													fprintf(DML_Download_fp,"%d/%d/%d",month,day,year);
													fprintf(DML_Download_fp,"$");
													fprintf(DML_Download_fp,"");
													fprintf(DML_Download_fp,"$");
													fprintf(DML_Download_fp,"Released");
													fprintf(DML_Download_fp,"$");
													MEM_free(effectivityStrFinal);

												}
												else
												{
													fprintf(DML_Download_fp,"");
													fprintf(DML_Download_fp,"$");
													fprintf(DML_Download_fp,"");
													fprintf(DML_Download_fp,"$");
													fprintf(DML_Download_fp,"Released");
													fprintf(DML_Download_fp,"$");
												}
											}
											else
											{
												//APL Released status not found
												fprintf(DML_Download_fp,"");
												fprintf(DML_Download_fp,"$");
												fprintf(DML_Download_fp,"");
												fprintf(DML_Download_fp,"$");
												fprintf(DML_Download_fp,"WIP");
												fprintf(DML_Download_fp,"$");
											}
										}
										else
										{
											printf("\n Release status count no release status found \n"); fflush(stdout);
											fprintf(DML_Download_fp,"");
											fprintf(DML_Download_fp,"$");
											fprintf(DML_Download_fp,"");
											fprintf(DML_Download_fp,"$");
											fprintf(DML_Download_fp,"");
											fprintf(DML_Download_fp,"$");
										}
										//



										//owning_group
										PartOwnrGrpS	=	NULL;
										ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"owning_group",&PartOwnrGrpS));
										printf("\nPart PartOwnrGrpS : %s",PartOwnrGrpS);fflush(stdout);
										fprintf(DML_Download_fp,"$");
										fprintf(DML_Download_fp,PartOwnrGrpS);

										fprintf(DML_Download_fp,"\n");

										//if (tc_strstr(PartOwnrGrpS,"APL")!=NULL && part_rev_tag!=NULLTAG)
										{
											//Call function for Part Details
											APLPart_Download(part_rev_tag,FilePath,DmlIdS);
										}

									}
								}
								else
								{
									printf("\nNo Part attached with Task...!!!");fflush(stdout);
								}
							}
							else
							{
								printf("\nTask to Part releation is not found...!!!");fflush(stdout);
							}

						}
					}
					else
					{
						printf("\nZero Task Found under Task");fflush(stdout);
					}
				}
			}
			else
			{
				printf("\nPlease Enter either Part Number or DML Number");fflush(stdout);
			}
			
		}
	}
	printf("End Function Complete_APLDML_Download...!!!");fflush(stdout);
	if (DML_Download_fp)
	{
		fclose(DML_Download_fp);
	}
	
}

int print_DesignRevisionData(tag_t itemrev,FILE *fdf)
{
	int				status 						= ITK_ok;
	int				r_count 					= 0;
	
	tag_t			item_tag 					= NULLTAG;
	tag_t			rev_tag						= NULLTAG;
	tag_t			*rev_list					= NULLTAG;
	
	char 			*rev_id						= NULL;

	char   *mcreator= NULL;
	char   *mcreatorName= NULL;
	char   *VCreator= NULL;
	char   *VCreatorName= NULL;
	char   *VerCreator= NULL;
	char   *Creator= NULL;
	char   *moddesc= NULL;
	char   *DrSts= NULL;
	char   *moddescDup= NULL;
	
	mcreator = (char *) MEM_alloc( 200 * sizeof(char) );
	mcreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	VCreatorName = (char *) MEM_alloc( 200 * sizeof(char) );
	VerCreator = (char *) MEM_alloc( 200 * sizeof(char) );
	
		
	ITK_CALL(AOM_UIF_ask_value(itemrev,"owning_user",&Creator)); //Creator
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_VerCreator",&VCreator));  //Version Creator
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_DocRemarks",&moddesc));  //modification desc 
	ITK_CALL(AOM_ask_value_string(itemrev,"t5_PartStatus",&DrSts));  //modification desc 
	mcreatorName = strtok(Creator,"(");
	mcreator = strtok(NULL,")");
	VCreatorName = strtok(VCreator,"(");
	VerCreator = strtok(NULL,")");

	printf("\n creator------->%s\n",moddesc);
	STRNG_replace_str(moddesc,"\n"," ",&moddescDup);
	printf("\n creator------->%s\n",moddescDup);

	printf("\n mcreatorName------->%s\n",mcreatorName);
	printf("\n creator------->%s\n",mcreator);
	printf("\n VerCreator------->%s\n",VerCreator);
	printf("\n DR status------->%s\n",DrSts);

	fprintf(fdf,mcreator);
	fprintf(fdf,"~");
	fprintf(fdf,VerCreator);
	fprintf(fdf,"~");
	fprintf(fdf,moddescDup);
	fprintf(fdf,"~");
	fprintf(fdf,DrSts);
	fprintf(fdf,"~");
	fprintf(fdf,"\n");
	
	return status;
}

int APLPart_Download(tag_t part_rev_tag, char* FilePath,char	*req_item)
{
	int		status					= 0;

	FILE	*Part_Download_fp		=	NULL;
	
	char	*item_type				=	NULL;
	char	*PartFile				=	NULL;
	char	*PartNumberS			=	NULL;
	char	*RevisionS				=	NULL;
	char	*PrtRev					=	NULL;
	char	*SequenceS				=	NULL;
	char	*DsgnGrpS				=	NULL;
	char	*DRStatS				=	NULL;
	char	*DescS					=	NULL;
	char	*DescSDup				=	NULL;
	char	*ProjCodeS				=	NULL;
	char	*PartOwnrGrpS			=	NULL;
	char	*PrtTypeS				=	NULL;
	char	*CoatedS				=	NULL;
	char	*ClrIndS				=	NULL;
	char	*DrgIndS				=	NULL;
	char	*EnvDiaS				=	NULL;
	char	*StdIndS				=	NULL;
	char	*UomS					=	NULL;
	char	*WeightS				=	NULL;
	char	*class_name				=   NULL;

	tag_t		class_id			=   NULLTAG;

	PartFile=(char *) MEM_alloc(200 * sizeof(char ));

	printf("\nInside APLPart_Download...!!!");fflush(stdout);
	

	ITK_CALL(ITEM_ask_rev_type2(part_rev_tag, &item_type));//find the revision type
	printf("\n Main item_typeS [%s] \n",item_type);  fflush(stdout);

	if (tc_strcmp(item_type,"Design Revision")==0)
	{
	
		ITK_CALL(AOM_ask_value_string(part_rev_tag,"item_id",&PartNumberS));
		printf("\nPart Number : %s",PartNumberS);fflush(stdout);
		
		tag_t	NewDsgnAttrId	=	NULLTAG;
		logical item_id_dml_Str	=	true;

		ITK_CALL(POM_class_of_instance(part_rev_tag,&class_id));
		ITK_CALL(POM_name_of_class(class_id,&class_name));
		printf("\n class_name is :%s\n",class_name);fflush(stdout);
		ITK_CALL(POM_attr_id_of_attr("t5_DenisPart","Design_0_Revision_alt",&NewDsgnAttrId));
		if (NewDsgnAttrId!=NULLTAG)
		{
			printf("\n Update thru POM");fflush(stdout);
			ITK_CALL(AOM_refresh(part_rev_tag,1));
			ITK_CALL(POM_set_env_info(POM_bypass_attr_update,TRUE,0,0,NULLTAG,NULL));
			ITK_CALL(POM_set_attr_logical(1,&part_rev_tag,NewDsgnAttrId,true));
			//ITK_CALL(AOM_set_value_logical(part_rev_tag,"t5_DenisPart",true));
			ITK_CALL(AOM_lock(part_rev_tag));
			ITK_CALL(AOM_save(part_rev_tag));
			ITK_CALL(AOM_unlock(part_rev_tag));
			ITK_CALL(AOM_refresh(part_rev_tag,1));
		}
		else
		{
			printf("\n Update thru AOM");fflush(stdout);
			//ITK_CALL(POM_attr_id_of_attr("t5_DenisPart","Design_0_Revision_alt",&NewDsgnAttrId));
			ITK_CALL(AOM_refresh(part_rev_tag,1));
			ITK_CALL(POM_set_env_info(POM_bypass_attr_update,TRUE,0,0,NULLTAG,NULL));
			//ITK_CALL(POM_set_attr_logical(1,&part_rev_tag,NewDsgnAttrId,true));
			ITK_CALL(AOM_set_value_logical(part_rev_tag,"t5_DenisPart",true));
			ITK_CALL(AOM_lock(part_rev_tag));
			ITK_CALL(AOM_save(part_rev_tag));
			ITK_CALL(AOM_unlock(part_rev_tag));
			ITK_CALL(AOM_refresh(part_rev_tag,1));
		}
		
		
		printf("\nPart locked for check out");fflush(stdout);

		strcpy(PartFile,FilePath);
		//strcat(File_Dml,req_item);
		strcat(PartFile,"/");
		strcat(PartFile,PartNumberS);
		strcat(PartFile,".txt");

		Part_Download_fp=fopen(PartFile,"w");

		fprintf(Part_Download_fp,PartNumberS);
		//fprintf(DML_Download_fp,"~");

		//2 Revision
		RevisionS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"item_revision_id",&RevisionS));
		if(RevisionS!=NULL) PrtRev=strtok(RevisionS,";");
		printf("\nPart PrtRev : %s",PrtRev);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,PrtRev);

		//3 Sequence
		SequenceS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"sequence_id",&SequenceS));
		printf("\nPart SequenceS : %s",SequenceS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,SequenceS);

		//4 Design Group
		DsgnGrpS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_DesignGrp",&DsgnGrpS));
		printf("\nPart DsgnGrpS : %s",DsgnGrpS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,DsgnGrpS);

		//5 Part Type
		PrtTypeS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_PartType",&PrtTypeS));
		printf("\nPart PrtTypeS : %s",PrtTypeS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,PrtTypeS);

		//6 DR Status
		DRStatS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_PartStatus",&DRStatS));
		printf("\nPart DRStatS : %s",DRStatS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,DRStatS);

		//7 Description
		DescS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"object_desc",&DescS));
		printf("\nPart DescS : %s",DescS);fflush(stdout);
		STRNG_replace_str(DescS,"","",&DescSDup);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,DescSDup);

		//8 Project Code
		ProjCodeS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_ProjectCode",&ProjCodeS));
		printf("\nPart ProjCodeS : %s",ProjCodeS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,ProjCodeS);

		//9 Owner Design Unit

		//10 Application Owner

		//11 Coated 
		CoatedS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_Coated",&CoatedS));
		printf("\nPart CoatedS : %s",CoatedS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,CoatedS);

		//12 Colour indicator
		ClrIndS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_ColourInd",&ClrIndS));
		printf("\nPart ClrIndS : %s",ClrIndS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,ClrIndS);

		//13 Drawing Ind
		DrgIndS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_ColourInd",&DrgIndS));
		printf("\nPart DrgIndS : %s",DrgIndS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,DrgIndS);

		//14 Env Diamension
		EnvDiaS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_EnvelopeDimensions",&EnvDiaS));
		printf("\nPart EnvDiaS : %s",EnvDiaS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,EnvDiaS);

		//15 Standard Part Indicator
		StdIndS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_StdPartIndicator",&StdIndS));
		printf("\nPart StdIndS : %s",StdIndS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,StdIndS);

		//16 Unit Of Measure
		UomS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_uom",&UomS));
		printf("\nPart UomS : %s",UomS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,UomS);

		//17 Weight
		WeightS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"t5_Weight",&WeightS));
		printf("\nPart WeightS : %s",WeightS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,WeightS);


		PartOwnrGrpS	=	NULL;
		ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"owning_group",&PartOwnrGrpS));
		printf("\nPart PartOwnrGrpS : %s",PartOwnrGrpS);fflush(stdout);
		fprintf(Part_Download_fp,"~");
		fprintf(Part_Download_fp,PartOwnrGrpS);

		print_DesignRevisionData(part_rev_tag,Part_Download_fp);

		APLDataSetDownload(part_rev_tag, FilePath, req_item);

		//BOM Download based on Context
		if (Part_Download_fp)
		{
			fclose(Part_Download_fp);
		}

	}
	
	printf("\nAPLPart_Download End...!!!");fflush(stdout);
}

int APLDataSetDownload(tag_t part_rev_tag, char* FilePath, char	*req_item)
{
	int		status					=	0;
	int		j						=	0;
	int		n_attchs				=	0;
	int		n_attchs_JT				=	0;
	int		iAttch					=	0;
	int		iPdf					=	0;
	int		jPdf					=	0;
	int		iJT						=	0;
	int		jJT						=	0;
	int		CadCount				=	0;
	int		int_ent_sequence		=	0;
	int		ctr						=	0;
	int		referencenumberfound	=	0;
	int		referencenumberfoundPdf	=	0;
	int		referencenumberfoundJT	=	0;

	FILE	*fdf;

	char	*PartFile				=	NULL;
	char	*PartNumberS			=	NULL;
	char	*item_type				=	NULL;
	char	*type_name				=	NULL;
	char	*type_name_rel			=	NULL;
	char	*AppOwner				=	NULL;
	char	*DataSetID				=	NULL;
	char	*DataSetRev				=	NULL;
	char	*enterprise_sequence	=	NULL;
	char	*ITemRevSeq				=	NULL;
	char	*tokenDI				=	NULL;
	char	*tokenJT				=	NULL;
	char	*tokenPdf				=	NULL;
	char	*Item_ID_str			=	NULL;
	char	*PrtType				=	NULL;
	char	*projectcode			=	NULL;
	char	*desgngrp				=	NULL;
	char	*DescS					=	NULL;
	char	*RevisionS				=	NULL;
	char	*Item_Revision_str_O	=	NULL;
	//char	*refname				=	NULL;
	char	*type_name_Pdf			=	NULL;
	char	*type_name_JT			=	NULL;

	char	refname[AE_reference_size_c + 1];
	char	refnamePdf[AE_reference_size_c + 1];
	char	refnameJT[AE_reference_size_c + 1];

	char	orig_name_Tmp[IMF_filename_size_c + 1];
	char	orig_name[IMF_filename_size_c + 1];
	char	orig_namePdf[IMF_filename_size_c + 1];
	char	orig_namePdf_Tmp[IMF_filename_size_c + 1];
	char	orig_nameJT_Tmp[IMF_filename_size_c + 1];
	char	orig_nameJT[IMF_filename_size_c + 1];

	char	pathname[SS_MAXPATHLEN + 1];
	char	pathnamePdf[SS_MAXPATHLEN + 1];
	char	pathnameJT[SS_MAXPATHLEN + 1];

	tag_t	reln_type				=	NULLTAG;
	tag_t	reln_type_Render		=	NULLTAG;
	tag_t	primary					=	NULLTAG;
	tag_t	relationstr				=	NULLTAG;
	tag_t	objTypeTag				=	NULLTAG;
	tag_t	refobject				=	NULLTAG;
	tag_t	refTag					=	NULLTAG;
	tag_t	primaryJT				=	NULLTAG;
	tag_t	primaryPdf				=	NULLTAG;
	tag_t	objTypeTagJT			=	NULLTAG;
	tag_t	objTypeTagPdf			=	NULLTAG;
	tag_t	refobjectPdf			=	NULLTAG;
	tag_t	refobjectJT				=	NULLTAG;
	tag_t	objTypeTag_rel			=	NULLTAG;

	AE_reference_type_t		refType;
	AE_reference_type_t     reftypePdf;
	AE_reference_type_t     reftypeJT;

	GRM_relation_t *rellist;
	GRM_relation_t *rellist_JT;

	PartFile=(char *) MEM_alloc(200 * sizeof(char ));

	printf("\nInside APLPart_Download...!!!");fflush(stdout);

	
	
	ITK_CALL(ITEM_ask_rev_type2(part_rev_tag, &item_type));//find the revision type
	printf("\n Main item_typeS [%s] \n",item_type);  fflush(stdout);

	ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_ApplicationOwner",&AppOwner));
	printf("\n Main AppOwner [%s] \n",AppOwner);  fflush(stdout);

	//ITK_CALL(AOM_ask_value_int(part_rev_tag,"sequence_id",&sequence_id_c));
	//printf("\n Main sequence_id_c [%s] \n",sequence_id_c);  fflush(stdout);
	ITemRevSeq = (char *) MEM_alloc( 5 * sizeof(char) );
	tokenDI = (char *) MEM_alloc( 200 * sizeof(char) );
	tokenJT = (char *) MEM_alloc( 200 * sizeof(char) );
	//sprintf(ITemRevSeq,"%d",sequence_id_c);

	ITK_CALL(AOM_ask_value_string(part_rev_tag,"item_id",&Item_ID_str));
	printf("\nPart Number : %s",Item_ID_str);fflush(stdout);

	ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_PartType",&PrtType));
	printf("\nPart Number : %s",Item_ID_str);fflush(stdout);
	ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_ProjectCode",&projectcode));
	printf("\nPart Number : %s",Item_ID_str);fflush(stdout);
	ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_DesignGrp",&desgngrp));
	printf("\nPart Number : %s",Item_ID_str);fflush(stdout);

	ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"item_revision_id",&RevisionS));
	printf("\nPart RevisionS : %s",RevisionS);fflush(stdout);
	STRNG_replace_str(RevisionS,";","~",&Item_Revision_str_O);

	DescS	=	NULL;
	ITK_CALL(AOM_UIF_ask_value(part_rev_tag,"object_desc",&DescS));
	printf("\nPart DescS : %s",DescS);fflush(stdout);

	ITK_CALL(AOM_ask_value_string(part_rev_tag,"gov_classification",&enterprise_sequence));
    printf("\n -----------------------enterprise_sequence -->%s\n",enterprise_sequence);

	int_ent_sequence=atoi(enterprise_sequence);

	if (strlen(AppOwner)==0)
	{
		AppOwner = ( char * ) MEM_alloc(10);
		strcpy(AppOwner,"ProCat");
	}

	if (tc_strcmp(item_type,"Design Revision")==0)
	{
		ITK_CALL(AOM_ask_value_string(part_rev_tag,"item_id",&PartNumberS));
		printf("\nPart Number : %s",PartNumberS);fflush(stdout);

		strcpy(PartFile,FilePath);
		//strcat(File_Dml,req_item);
		strcat(PartFile,"/");
		strcat(PartFile,PartNumberS);
		strcat(PartFile,"_DATASET");
		strcat(PartFile,".txt");

		fdf	=	fopen(PartFile,"w");

		//LH RH Catia - Need to confirm

		//Find attached Item with Parts
		ITK_CALL(GRM_list_secondary_objects(part_rev_tag,reln_type,&n_attchs,&rellist));
		
		if (n_attchs > 0)
		{
			ITK_CALL(GRM_find_relation_type("IMAN_Rendering",&reln_type_Render));
			ITK_CALL(GRM_list_secondary_objects(part_rev_tag,reln_type_Render,&n_attchs_JT,&rellist_JT));

			for (iAttch= 0; iAttch < n_attchs; iAttch++)
			{
				printf("\n Inside loop ..............");fflush(stdout);
				primary=rellist[iAttch].secondary;
				relationstr=rellist[iAttch].the_relation;
		
				ITK_CALL(TCTYPE_ask_object_type(primary,&objTypeTag));
				ITK_CALL(TCTYPE_ask_name2(objTypeTag,&type_name));

				printf("\ntype_name : %s",type_name);fflush(stdout);

				if( (tc_strcmp(type_name,"CATProduct")==0 ||tc_strcmp(type_name,"CMI2Product")==0 || tc_strcmp(type_name,"t5Alias")==0 || tc_strcmp(type_name,"CATPart")==0 || tc_strcmp(type_name,"CMI2Part")==0 || tc_strcmp(type_name,"CMI2AuxPart")==0) && strstr(AppOwner,"Cat"))
				{
					CadCount++;
					printf("\n--------inside if of n attaches Catia Product CAt part---------------------\n");fflush(stdout);

					//FIND THE DATASET ID AND REVISION
					DataSetID	=	NULL;
					DataSetRev	=	NULL;
					ITK_CALL(AE_ask_dataset_id_rev(primary,&DataSetID, &DataSetRev));
					printf("\nDataset Id : %s, Dataset Rev : %s",DataSetID,DataSetRev);fflush(stdout);
					
					//FIND DATASET COUNT
					ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
					printf("\nreferencenumberfound : %d",referencenumberfound);fflush(stdout);
					if (referencenumberfound>0)
					{
						for(j=0;j<referencenumberfound;j++)
						{
							printf("\n -------- inside referencenumberfound CAT PRT--------------------- \n");
							

							//Finds named references in datasets.
							//ITK_CALL(AE_find_dataset_named_ref2(primary,j,&refName,&refType,&refTag));
							ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&refType,&refobject));
							printf("\n ---name ref type CAT PRD PRT----- :%s\n",refname);
							if(tc_strcmp(refname,"CMIAuxFileInfo")!=0 && tc_strcmp(refname,"CMI2BBInfo")!=0 && tc_strcmp(refname,"CMI2TypeInfo")!=0)
							{
								ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
								ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));

								printf("\n orig_name is :%s\n",orig_name);
								
								if (orig_name!=NULL)
								{
									if(strstr(orig_name,"jt")==NULL)
									{
										printf("\nenterprise %d \n",int_ent_sequence);
										//printf("\nua %d\n",sequence_id_c);
										tokenDI = strtok(orig_name,".");
										printf("\ntokenDI %s\n",tokenDI);
										ctr	=	int_ent_sequence+1;
										printf("\nNot in spec loop %s\n",Item_ID_str);	
										fprintf(fdf,Item_ID_str);						//1] Part Number
										fprintf(fdf,"~");
										fprintf(fdf,projectcode);						//2] Project Code
										fprintf(fdf,"~");
										fprintf(fdf,desgngrp);							//3] Design Group
										fprintf(fdf,"~");
										fprintf(fdf,DescS);
										fprintf(fdf,"~");								//4] Description
										fprintf(fdf,orig_name_Tmp);						//5] Cad data if no cad data then NULL is stamped 
										fprintf(fdf,"~");
										fprintf(fdf,"NULL");							//6] ???
										fprintf(fdf,"~");				
										fprintf(fdf,Item_Revision_str_O);				//7]Revision and Sequence
										fprintf(fdf,"~");
										fprintf(fdf,PrtType);							//8] Part type of part
										fprintf(fdf,"~");
										fprintf(fdf,"Released");						//9] Released status
										fprintf(fdf,"~");
										fprintf(fdf,orig_name_Tmp);						//10] Cad data if no cad data then NULL is stamped
										fprintf(fdf,"~");
										fprintf(fdf,Item_Revision_str_O);				//11]Revision and Sequence
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//12] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//13] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//14] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//15] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//16] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"NULL");							//17] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"0");								//18] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"Y");								//19] bom INDICATOR                               
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"-");								//20] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"-");								//21] ???????                                     
										fprintf(fdf,"~");								                                                    
										fprintf(fdf,"reptest#");						//22] ???????                                     
										fprintf(fdf,"NULL#");
										fprintf(fdf,"NULL#");
										fprintf(fdf,"NULL");
										fprintf(fdf,"~");

										printf("\norig_name issssssss 11---->%s\n",orig_name);
										//JT DOWNLOADING PENDING - Need to discuss
										if (!strstr(orig_name,"Spec"))
										{
											for (iJT= 0; iJT < n_attchs_JT; iJT++)
											{
												printf("\n Inside loop JT .............. %d \n",iJT);fflush(stdout);
												primaryJT=rellist_JT[iJT].secondary;

												ITK_CALL(TCTYPE_ask_object_type(primaryJT,&objTypeTagJT));
												ITK_CALL(TCTYPE_ask_name2(objTypeTagJT,&type_name_JT));
												if(tc_strcmp(type_name_JT,"DirectModel")==0)
												{
													ITK_CALL(AE_ask_dataset_ref_count(primaryJT,&referencenumberfoundJT));
													printf("\n JT found......%d",referencenumberfoundJT);fflush(stdout);
													for(jJT=0;jJT<referencenumberfoundJT;jJT++)
													{
														ITK_CALL(AE_find_dataset_named_ref(primaryJT,jJT,refnameJT,&reftypeJT,&refobjectJT));
														ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT));
														ITK_CALL( IMF_ask_original_file_name(refobjectJT,orig_nameJT_Tmp));
														printf("\n orig_nameJT is :%s  jJT Conut %s\n",orig_nameJT,orig_nameJT_Tmp);
														printf("\n orig_nameJT is :%s  jJT Conut %d\n",orig_nameJT,jJT);
														tokenJT = strtok(orig_nameJT,".jt");
														printf("\n tokenJT %s tokenDI%s\n",tokenJT,tokenDI);
														printf("\n tFilePath :%s\n",FilePath);
														printf("\n tFilePath :%s/%s\n",FilePath,orig_nameJT_Tmp);
														if(tc_strcmp(tokenJT,tokenDI)==0)
														{
															fprintf(fdf,orig_nameJT_Tmp);
															//sprintf(pathnameJT,"%s%s/%s",FilePath,req_item,orig_nameJT_Tmp );
															sprintf(pathnameJT,"%s/%s",FilePath,orig_nameJT_Tmp );
															printf("\n pathnameJT=%s\n",pathnameJT);
															IMF_export_file(refobjectJT,pathnameJT);
															
														}
													}
												}
											}
										}

										fprintf(fdf,"~");
										print_DesignRevisionData(part_rev_tag,fdf);
										//sprintf(pathname,"%s%s/%s",FilePath,req_item,orig_name_Tmp );
										sprintf(pathname,"%s/%s",FilePath,orig_name_Tmp );
										printf("\n pathname=%s\n",pathname);
										IMF_export_file(refobject,pathname);
									}
								}
								
							}
						}
					}
				}
				else if((tc_strcmp(type_name,"CATDrawing")==0||tc_strcmp(type_name,"CMI2Drawing")==0) && strstr(AppOwner,"Cat"))
				{
					CadCount++;
					
					ITK_CALL(TCTYPE_ask_object_type(relationstr,&objTypeTag_rel));
					ITK_CALL(TCTYPE_ask_name2(objTypeTag_rel,&type_name_rel));

					printf("\n--------inside if of n Drawing attaches---------------------\n");fflush(stdout);
				
					DataSetID	=	NULL;
					DataSetRev	=	NULL;
					ITK_CALL(AE_ask_dataset_id_rev(primary,&DataSetID, &DataSetRev));
					printf("\nDataset Id : %s, Dataset Rev : %s",DataSetID,DataSetRev);fflush(stdout);
					
					//FIND DATASET COUNT
					ITK_CALL(AE_ask_dataset_ref_count(primary,&referencenumberfound));
					printf("\nreferencenumberfound : %d",referencenumberfound);fflush(stdout);

					printf("\nreferencenumberfound =%d\n",referencenumberfound);
					if (referencenumberfound>0)
					{
						for(j=0;j<referencenumberfound;j++)
						{
							printf("\n--------inside referencenumberfound CAT DRW---------------------\n");
							
							ITK_CALL(AE_find_dataset_named_ref(primary,j,refname,&refType,&refobject));
							printf("\n ---name ref type CAT DRW PRT----- :%s\n",refname);

							if(tc_strcmp(refname,"CATDrawing")==0)
							{
								printf("\nCAD Drawing found...!!!");fflush(stdout);

								ITK_CALL( IMF_ask_original_file_name(refobject,orig_name_Tmp));
								ITK_CALL( IMF_ask_original_file_name(refobject,orig_name));
								printf("\n orig_name is Cat :%s %s\n",orig_name,orig_name_Tmp);

								tokenDI = strtok(orig_name,".");
								printf("\ntokenDI Drawing Loop %s\n",tokenDI);

								if(tc_strcmp(type_name_rel,"IMAN_reference")!=0)
								{
									if(strstr(orig_name_Tmp,"pdf")==NULL)
									{
										ctr=int_ent_sequence+1;

										printf("\nNot in spec loop  %s\n",Item_ID_str);

										fprintf(fdf,Item_ID_str);					//1]  Part nos                                    
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,projectcode);					//2] projectcode                                  
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,desgngrp);						//3] desgngrp                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,DescS);							//4] Description                                  
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,orig_name_Tmp);					//5] Cad data if no cad data then NULL is stamped 
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//6] ???????                                      
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,Item_Revision_str_O);			//7] Rev and sequence of design revison           
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,PrtType);						//8] Part type of part                            
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"Released");					//9] Part type of part                            
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,orig_name_Tmp);					//10] Cad data if no cad data then NULL is stamped
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,Item_Revision_str_O);			//11] Rev and sequence of design revison          
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//12] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//13] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//14] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//15] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//16] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"NULL");						//17] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"0");							//18] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"Y");							//19] bom INDICATOR                               
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"-");							//20] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"-");							//21] ???????                                     
										fprintf(fdf,"~");							                                                    
										fprintf(fdf,"reptest#");					//22] ???????                                     
										

										if(tc_strcmp(type_name_rel,"IMAN_reference")==0)
										{
											fprintf(fdf,Item_ID_str);
											fprintf(fdf,"#");
											fprintf(fdf,Item_Revision_str_O);

										}
										else
										{
											fprintf(fdf,"NULL#");
											fprintf(fdf,"NULL#");
											fprintf(fdf,"NULL");
										}

										fprintf(fdf,"~");

										for (iPdf= 0; iPdf < n_attchs; iPdf++)
										{
											printf("\n Inside loop Pdf .............. %d \n",iPdf);fflush(stdout);
											primaryPdf=rellist[iPdf].secondary;

											ITK_CALL(TCTYPE_ask_object_type(primaryPdf,&objTypeTagPdf));
											ITK_CALL(TCTYPE_ask_name2(objTypeTagPdf,&type_name_Pdf));
											if(tc_strcmp(type_name_Pdf,"PDF")==0)
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
													if(tc_strcmp(tokenPdf,tokenDI)==0)
													{
														fprintf(fdf,orig_namePdf_Tmp);    //23] JT/PDF             
														//sprintf(pathnamePdf,"%s%s/%s",FilePath,req_item,orig_namePdf_Tmp );
														sprintf(pathname,"%s/%s",FilePath,orig_namePdf_Tmp );
														printf("\n pathnamePdf=%s\n",pathnamePdf);
														IMF_export_file(refobjectPdf,pathnamePdf);
													}
												}
											}
										}
										fprintf(fdf,"~");
										print_DesignRevisionData(part_rev_tag,fdf);
										sprintf(pathname,"%s/%s",FilePath,orig_name_Tmp );
										printf("\n pathname=%s\n",pathname);
										IMF_export_file(refobject,pathname);

									}
								}
							}
						}
					}

				}
			}
		}
		else
		{
			printf("\nThere are no data item attached to it \n");
			
		}
		APLBOMDownLoad(part_rev_tag, FilePath, req_item);

	}

}

//PARENT PART^CHILDAPRT^BL_OCCURS^QTY^MAINSERIAL^TMATRIX
int APLBOMDownLoad(tag_t part_rev_tag, char* FilePath, char	*req_item)
{
	FILE	*fChild;
	int		status					=	0;
	int		nClhd					=	0;
	int		iChildItemTag			=	0;
	int		iChld					=	0;
	int		iChildViewMask			=	0;
	int		iQuantityAttrId			=	0;
	int		iTransforationMatrixId	=	0;
	int		iOccuranceId			=	0;

	char	*PartFile				=	NULL;
	char	*c_Item_VM				=	NULL;
	char	*item_type				=	NULL;
	char	*PrtType				=	NULL;
	char	*projectcode			=	NULL;
	char	*RevisionS				=	NULL;
	char	*desgngrp				=	NULL;
	char	*PrtNumber				=	NULL;
	char	*revRuleName			=	NULL;
	char	*ChildPuneCS			=	NULL;
	char	*ChldRev				=	NULL;
	char	*ChldName				=	NULL;
	char	*cQuantity				=	NULL;
	char	*cTransforationMatrix	=	NULL;
	char	*parentrev_O			=	NULL;
	char	*childrev_O				=	NULL;
	char	*cTransforationMatrix_O	=	NULL;
	char	*PartOwnrGrpS			=	NULL;
	char	*cPrtType				=	NULL;
	char	*cOccurance				=	NULL;
	

	tag_t		window				=	NULLTAG;
	tag_t		rule				=	NULLTAG;
	tag_t		top_line			=	NULLTAG;
	tag_t		*DgnChld			=	NULLTAG;
	tag_t		t_ChldItemRev		=	NULLTAG;
	tag_t		revRule 			= NULLTAG;

	printf("\nInside APL BOM Download...!!!");fflush(stdout);

	PartFile=(char *) MEM_alloc(200 * sizeof(char ));

	revRuleName	=	(char *)MEM_alloc(250 * sizeof(char));
	tc_strcpy(revRuleName,"ERC release and above");

	ITK_CALL(ITEM_ask_rev_type2(part_rev_tag, &item_type));//find the revision type
	printf("\n Main item_typeS [%s] \n",item_type);  fflush(stdout);
	
	ITK_CALL(AOM_ask_value_string(part_rev_tag,"item_id",&PrtNumber));
	printf("\nPart Number : %s",PrtNumber);fflush(stdout);

	ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_PartType",&PrtType));
	printf("\nPart Type : %s",PrtType);fflush(stdout);
	ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_ProjectCode",&projectcode));
	printf("\nPart Project : %s",projectcode);fflush(stdout);
	ITK_CALL(AOM_ask_value_string(part_rev_tag,"t5_DesignGrp",&desgngrp));
	printf("\nPart Design Group : %s",desgngrp);fflush(stdout);

	ITK_CALL(AOM_ask_value_string(part_rev_tag,"item_revision_id",&RevisionS));
	
	STRNG_replace_str(RevisionS,";","^",&parentrev_O);
	printf("\nPart RevisionS : %s",parentrev_O);fflush(stdout);
	//BOM Details
	ITKCALL(BOM_create_window (&window));
	ITKCALL(CFM_find(revRuleName, &rule));
	if (revRule != NULLTAG)
	{
		printf("\nFind revRule\n");fflush(stdout);
		ITKCALL(BOM_set_window_config_rule(window,revRule));
	}

	ITKCALL(BOM_set_window_top_line(window, null_tag,part_rev_tag , null_tag, &top_line));
	ITK_CALL(BOM_set_window_pack_all (window, true));
	ITK_CALL( BOM_window_show_suppressed ( window ));
	ITKCALL(BOM_line_ask_child_lines (top_line, &nClhd, &DgnChld));
	printf("\nNo of child found %d under %s",nClhd,PrtNumber);fflush(stdout);
	
	if (nClhd>0)
	{
		strcpy(PartFile,FilePath);
		//strcat(File_Dml,req_item);
		strcat(PartFile,"/");
		strcat(PartFile,PrtNumber);
		strcat(PartFile,"_USESPART");
		strcat(PartFile,".txt");
		fChild	=	fopen(PartFile,"w");

		for (iChld=0;iChld<nClhd ;iChld++ )
		{
			//ITKCALL(BOM_line_unpack (DgnChld[iChld]));
			
			ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
			ITKCALL(BOM_line_ask_attribute_tag(DgnChld[iChld], iChildItemTag, &t_ChldItemRev));
			
			fprintf(fChild,"%s",PrtNumber);fflush(fChild);	//1] Part Number
			fprintf(fChild,"^");fflush(fChild);				
			fprintf(fChild,"%s",PrtType);fflush(fChild);	//2] Part Type
			fprintf(fChild,"^");fflush(fChild);				
			fprintf(fChild,"%s",parentrev_O);fflush(fChild);//3] Part Rev, Seq
			fprintf(fChild,"^");fflush(fChild);


			ITKCALL(AOM_ask_value_string(t_ChldItemRev,"item_id",&ChldName));
			printf("\n child ChldName:%s ..............",ChldName);fflush(stdout);
			fprintf(fChild,"%s",ChldName);fflush(fChild);
			fprintf(fChild,"^");fflush(fChild);

			ITKCALL(AOM_ask_value_string(t_ChldItemRev,"item_revision_id",&ChldRev));
			printf("\n child ChldRev:%s ..............",ChldRev);fflush(stdout);
			STRNG_replace_str(ChldRev,";","^",&childrev_O);
			//fprintf(fChild,"%s",childrev_O);fflush(fChild);
			//fprintf(fChild,"^");fflush(fChild);

			ITK_CALL(AOM_ask_value_string(t_ChldItemRev,"t5_PartType",&cPrtType));
			printf("\nPart cPrtType : %s",cPrtType);fflush(stdout);

			//fprintf(fChild,"%s",cPrtType);fflush(fChild);
			//fprintf(fChild,"^");fflush(fChild);

			//ITKCALL(AOM_ask_value_string(t_ChldItemRev,"t5_CarMakeBuyIndicator",&ChildPuneCS));
			//printf("\n child ChildPuneCS:%s ..............",ChildPuneCS);fflush(stdout);

			//t5CurrentViewMaskC
			ITKCALL(BOM_line_look_up_attribute("bl_occ_t5_CurrentViewMaskC",&iChildViewMask));
			ITKCALL( BOM_line_ask_attribute_string(DgnChld[iChld], iChildViewMask, &c_Item_VM));
			//ITKCALL(AOM_ask_value_string(DgnChld[iChld],"t5CurrentViewMaskC",&ChildViewMask));
			printf("\n child ChildViewMask:%s ..............",c_Item_VM);fflush(stdout);
			if (tc_strcmp(c_Item_VM,"NULL")!=0 && tc_strlen(c_Item_VM)>0)
			{
				fprintf(fChild,"%s",c_Item_VM);fflush(fChild);
			}
			else
			{
				//c_Item_VM	=	NULL;
				//c_Item_VM	=	(char *)MEM_alloc(5);
				//tc_strcpy(c_Item_VM," ");
				fprintf(fChild," ");fflush(fChild);
			}
			fprintf(fChild,"^");fflush(fChild);

			ITK_CALL(BOM_line_look_up_attribute("bl_quantity", &iQuantityAttrId));
			ITK_CALL(  BOM_line_ask_attribute_string(DgnChld[iChld],iQuantityAttrId, &cQuantity));
			printf("\nChild Qnatity : %s",cQuantity);fflush(stdout);
			fprintf(fChild,"%s",cQuantity);fflush(fChild);
			fprintf(fChild,"^");fflush(fChild);

			//bl_occurrence_name --Occurance for Multi quantity
			ITK_CALL(BOM_line_look_up_attribute("bl_occurrence_name", &iOccuranceId));
			ITK_CALL(  BOM_line_ask_attribute_string(DgnChld[iChld],iOccuranceId, &cOccurance));
			printf("\nChild Qnatity : %s",cOccurance);fflush(stdout);
			fprintf(fChild,"%s",cOccurance);fflush(fChild);
			fprintf(fChild,"^");fflush(fChild);

			ITK_CALL(BOM_line_look_up_attribute("bl_plmxml_abs_xform", &iTransforationMatrixId));
			ITK_CALL(BOM_line_ask_attribute_string(DgnChld[iChld],iTransforationMatrixId, &cTransforationMatrix));
			STRNG_replace_str(cTransforationMatrix," ","^",&cTransforationMatrix_O);
			printf("\nTransformation Matrix : %s",cTransforationMatrix_O);fflush(stdout);
			fprintf(fChild,"%s",cTransforationMatrix_O);fflush(fChild);
			
			

			//PartOwnrGrpS	=	NULL;
			ITK_CALL(AOM_UIF_ask_value(t_ChldItemRev,"owning_group",&PartOwnrGrpS));
			printf("\nPart PartOwnrGrpS : %s",PartOwnrGrpS);fflush(stdout);
			fprintf(fChild,"^");
			fprintf(fChild,PartOwnrGrpS);
			fprintf(fChild,"\n");fflush(fChild);


		}
		if (fChild)
		{
			fclose(fChild);
		}
	}
	

//	Set Closure Rule
//	CALLAPI(PIE_find_closure_rules2(closureRuleName,scope,&n_closure_tags,&closure_tags));
//	printf("\n n_closure_tags:%d ..............",n_closure_tags);fflush(stdout);
//	if(n_closure_tags==1)
//	{
//		closure_tag=closure_tags[0];
//		CALLAPI(BOM_window_set_closure_rule(window,closure_tag,0,NULL,NULL));
//	}
}