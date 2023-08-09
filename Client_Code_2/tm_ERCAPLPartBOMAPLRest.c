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

#include <tccore/aom.h>
#include <res/res_itk.h>
#include <bom/bom.h>
#include <ctype.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <itk/mem.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ps/ps.h>
#include <tccore/uom.h>
#include <user_exits/user_exits.h>
#include <rdv/arch.h>
#include <stdlib.h>
#include <string.h>
#include <textsrv/textserver.h>
#include <tccore/item_errors.h>
#include <stdlib.h>
#include <pie/pie.h>

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

int validateBOMPart(char *inputfile,FILE *fperror1)
{
	int status;

	FILE *fp1=NULL;
	char *inputline1=NULL;
	char *mod_Part1=NULL;
	char *prent_Orgid1=NULL;
	char *parent_item_id1=NULL;
	char *parent_item_revision_id1=NULL;
	char *parent_item_sequence_id1=NULL;
	char *child_item_id1=NULL;
	char *child_item_revision_id1=NULL;
	char *child_item_sequence_id1=NULL;
	char *childitemRevSeq=NULL;
	const char *attrs1[1];
	const char *values1[1];
	int n_tags_found1= 0;
	tag_t *tags_found1 = NULL;
	tag_t child_item=NULLTAG;
	tag_t child_rev=NULLTAG;
	
	printf("\n INSIDE BOM VALIDATE FUNCTION.. %s \n",inputfile);
	
	fp1=fopen(inputfile,"r");
	if(fp1!=NULL)
	{
		inputline1=(char *) MEM_alloc(1000);
		while(fgets(inputline1,1000,fp1)!=NULL)
		{
			fputs(inputline1,stdout);


			mod_Part1=NULL;
			prent_Orgid1=NULL;
			parent_item_id1=NULL;
			parent_item_revision_id1=NULL;
			parent_item_sequence_id1=NULL;
			child_item_id1=NULL;
			child_item_revision_id1=NULL;
			child_item_sequence_id1=NULL;

			mod_Part1=strtok(inputline1,"^");  //1
			prent_Orgid1=strtok(NULL,"^");  //2
			parent_item_id1=strtok(NULL,"^"); //3
			parent_item_revision_id1=strtok(NULL,"^"); //4
			parent_item_sequence_id1=strtok(NULL,"^"); //4
			child_item_id1=strtok(NULL,"^"); //2
			child_item_revision_id1=strtok(NULL,"^"); //3
			child_item_sequence_id1=strtok(NULL,"^"); //4


			printf("\n 11 mod_Part .......%s,%s,%s,%s,%s,%s,%s",mod_Part1,parent_item_id1,parent_item_revision_id1,parent_item_sequence_id1,child_item_id1,child_item_revision_id1,
			child_item_sequence_id1);fflush(stdout);

			attrs1[0] ="item_id";
			values1[0] = (char *)child_item_id1;
			ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs1, values1, &n_tags_found1, &tags_found1));
			if(n_tags_found1==0)
			{
				fprintf(fperror1,"Child Parent :[%s],[%s],[%s] of Parent Part not found  :[%s],[%s],[%s]\n",child_item_id1,child_item_revision_id1,child_item_sequence_id1,
				parent_item_id1,parent_item_revision_id1,parent_item_sequence_id1);
				
				printf("11 Child Parent :[%s],[%s],[%s] of Parent Part not found  :[%s],[%s],[%s]\n",child_item_id1,child_item_revision_id1,child_item_sequence_id1,
				parent_item_id1,parent_item_revision_id1,parent_item_sequence_id1);fflush(stdout);

			}
//			else
//			{
//				
//					child_item = tags_found1[0];
//					printf(" 111 Item already exists\n");fflush(stdout);
//
//					childitemRevSeq = NULL;
//					childitemRevSeq=(char *) MEM_alloc(32);
//					strcpy(childitemRevSeq,child_item_revision_id1);
//					strcat(childitemRevSeq,";");
//					strcat(childitemRevSeq,child_item_sequence_id1);
//					printf(" 1111childitemRevSeq [%s]\n",childitemRevSeq);fflush(stdout);	
//					
//					ITK_CALL(ITEM_find_revision(child_item,childitemRevSeq,&child_rev));
//					if(child_rev != NULLTAG)
//					{
//						printf(" 11 Child Parent :[%s],[%s],[%s] of Parent Part found......  :[%s],[%s],[%s]\n",child_item_id1,child_item_revision_id1,child_item_sequence_id1,
//						parent_item_id1,parent_item_revision_id1,parent_item_sequence_id1);fflush(stdout);
//
//					}
//					else
//					{
//						fprintf(fperror1,"Child Parent :[%s],[%s],[%s] of Parent Part not found  :[%s],[%s],[%s]\n",child_item_id1,child_item_revision_id1,child_item_sequence_id1,
//						parent_item_id1,parent_item_revision_id1,parent_item_sequence_id1);
//						
//						printf("111Child Parent :[%s],[%s],[%s] of Parent Part not found  :[%s],[%s],[%s]\n",child_item_id1,child_item_revision_id1,child_item_sequence_id1,
//						parent_item_id1,parent_item_revision_id1,parent_item_sequence_id1);fflush(stdout);
//
//					}
//			}
		}
	}

	
	if(fp1)fclose(fp1);fp1=NULL;
	if(fperror1)fclose(fperror1);fperror1=NULL;


}

extern int ITK_user_main (int argc, char ** argv )
{

    int status;
	char *inputfile=NULL;
	char *PlantName="PlantName1";
	char *mod_Part=NULL;
	char *prent_Orgid=NULL;
	char *parent_item_id=NULL;
	char *parent_item_revision_id=NULL;
	char *parent_item_sequence_id=NULL;
	char *child_item_id=NULL;
	char *child_item_revision_id=NULL;
	char *child_item_sequence_id=NULL;
	char *child_item_Qty=NULL;
	char *parent_itemRevSeq=NULL;
	char *view_name=NULL;
	char *new_view_name=NULL;
	char *newViewName=NULL;
	char *partName=NULL;
	char *new_partName=NULL;
	char *childItemName=NULL;
	char *childItemFlag=NULL;
	char *childItemQty=NULL;
	char *new_AchildItemName=NULL;
	char *new_AchildItemQty=NULL;
	char *view_type_name=NULL;
	char *new_AViewName=NULL;
	char *newItemName=NULL;
	char *parentitemRevSeq=NULL;
	char	*bs_view_type_name		= NULL;
	char	*getPlantViewName		= NULL;
	char	*getPlantView		= NULL;
	char	*getPlantCurntMaskFlag		= NULL;
	FILE *fp=NULL;
	FILE *fperror=NULL;
	FILE *fperror1=NULL;
	FILE *fpexception=NULL;
	FILE *fperror_file=NULL;
	char* inputline=NULL;
	const char *attrs[1];
	const char *values[1];
	const char *attrs1[1];
	const char *values1[1];
	int n_tags_found= 0;
	int n_tags_found_prt= 0;
	int aplFlag_attribute= 0;
	int newitemAlreadyFnd= 0;
	int Ccnt= 0;
	int n= 0;
	int k1= 0;
	tag_t *tags_found = NULL;
	tag_t *tags_found_prt = NULL;
	tag_t parent_item=NULLTAG;
	tag_t item_part=NULLTAG;
	tag_t parent_rev=NULLTAG;
	tag_t	window 				= NULLTAG;
	tag_t	newwindow 				= NULLTAG;
	tag_t revRule 			= NULLTAG;
	tag_t newrevRule 			= NULLTAG;
	int 	n_closure_tags=0;
	PIE_scope_t scope;
	tag_t * 	closure_tags;
	tag_t  	closure_tag;
	tag_t			top_line							= NULLTAG;
	tag_t			new_A_top_line						= NULLTAG;
	tag_t			new_top_line						= NULLTAG;
	tag_t			view_type							= NULLTAG;
	tag_t			Bomline_tag							= NULLTAG;
	tag_t			new_Bomline_tag						= NULLTAG;
	tag_t  *children1=NULLTAG;
	tag_t  *new_add_children1=NULLTAG;
	tag_t  *newchildren=NULLTAG;
	int iChildItemTag=0;
	int newChildItemTag=0;
	int new_AChildItemTag=0;
	int ercViewFound=0;
	int newercViewFound=0;
	int assbvr=0;
	int newassbvr=0;
	tag_t   t_ChildItemRev;
	tag_t   new_tChildItemRev;
	tag_t   new_AChildItemRev;
	int 	n_values_bvr_bs=0;
	int 	new_values_bvr_bs=0;
	int 	itemDeleted=0;
	int 	itemAlreadyFnd=0;
	int 	newCnt=0;
	int 	an=0;
	int 	ak=0;
	double 	consumableQtyD;
	tag_t			*bvr_assy_part= NULLTAG;
	tag_t			*new_bvr_assy_part= NULLTAG;
	tag_t			view_type_tag					= NULLTAG;
	tag_t			new_parent_bvr					= NULLTAG;
	tag_t			bom_view_ercassyPart			= NULLTAG;
	tag_t	*occurrences						= NULLTAG;
	tag_t			*bvr_aplassy_part= NULLTAG;
		tag_t	bom_view_item			= NULLTAG;
	int bvr_count_aplpartRev=0;
	tag_t			*bvrs_aplpartRev= NULLTAG;
	tag_t			bvrs_aplpartRev1= NULLTAG;
	int 	aplcViewFound=0;
	int bvr_count_BSCRev=0;
	tag_t			*bvrs_BSCRev= NULLTAG;
	int 	bvrCnt=0;
	int 	aplChildPartF=0;
	char *view_name_assy=NULL;
	char *item_Qty=NULL;
	char *view_name_item=NULL;
	int childCnt=0;
	int kC=0;
	char *ItemName = NULL;
	const char *type_name[1];
	char* tempmat1=NULL;
	int imat=0;
	int jmat=0;
	double mat[4][4];
	char* tempmat=NULL;
	tag_t tag_occ_name=0;
	int length=0;
	int omat=0;
	int clean=0;
	char* intancename=NULL;
	char* getPlantCR=NULL;
	char* intancename2;
	long int res=0;
	int newitembvr=0;
	int itembvr_count=0;
	int aplcViewItmFound=0;
	tag_t* item_bv_list=NULLTAG;
										

	inputfile = ITK_ask_cli_argument("-i=");
	//PlantName = ITK_ask_cli_argument("-plname=");

	if((tc_strcmp(PlantName,"PlantName1")==0) || (tc_strcmp(PlantName,"PlantName4")==0))
	{
		getPlantCR=NULL;
		getPlantCR=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLC");

		getPlantView=NULL;
		getPlantView=(char *) MEM_alloc(100);
		tc_strcpy(getPlantView,"APLC");

		getPlantViewName=NULL;
		getPlantViewName=(char *) MEM_alloc(100);
		tc_strcpy(getPlantViewName,"T5_APLCView");

		getPlantCurntMaskFlag=NULL;
		getPlantCurntMaskFlag=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCurntMaskFlag,"bl_occ_t5_CurrentViewMaskC");	
		

		printf("OPCS Name After Conversion ==> [%s]:[%s]:[%s]\n",getPlantCurntMaskFlag,getPlantCR,getPlantView);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName13")==0)
	{
		getPlantCR=NULL;
		getPlantCR=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLV");

		getPlantView=NULL;
		getPlantView=(char *) MEM_alloc(100);
		tc_strcpy(getPlantView,"APLV");

		getPlantViewName=NULL;
		getPlantViewName=(char *) MEM_alloc(100);
		tc_strcpy(getPlantViewName,"T5_APLVView");

		getPlantCurntMaskFlag=NULL;
		getPlantCurntMaskFlag=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCurntMaskFlag,"bl_occ_t5_CurrentViewMaskV");

		printf("OPCS Name After Conversion ==> [%s]:[%s]:[%s]\n",getPlantCurntMaskFlag,getPlantCR,getPlantView);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName2")==0)
	{
		getPlantCR=NULL;
		getPlantCR=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLU");

		getPlantView=NULL;
		getPlantView=(char *) MEM_alloc(100);
		tc_strcpy(getPlantView,"APLU");

		getPlantViewName=NULL;
		getPlantViewName=(char *) MEM_alloc(100);
		tc_strcpy(getPlantViewName,"T5_APLUView");

		getPlantCurntMaskFlag=NULL;
		getPlantCurntMaskFlag=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCurntMaskFlag,"bl_occ_t5_CurrentViewMaskU");

		printf("OPCS Name After Conversion ==> [%s]:[%s]:[%s]\n",getPlantCurntMaskFlag,getPlantCR,getPlantView);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName3")==0)
	{
		getPlantCR=NULL;
		getPlantCR=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLP");

		getPlantView=NULL;
		getPlantView=(char *) MEM_alloc(100);
		tc_strcpy(getPlantView,"APLP");

		getPlantViewName=NULL;
		getPlantViewName=(char *) MEM_alloc(100);
		tc_strcpy(getPlantViewName,"T5_APLPView");

		getPlantCurntMaskFlag=NULL;
		getPlantCurntMaskFlag=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCurntMaskFlag,"bl_occ_t5_CurrentViewMaskP");

		printf("OPCS Name After Conversion ==> [%s]:[%s]:[%s]\n",getPlantCurntMaskFlag,getPlantCR,getPlantView);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName6")==0)
	{
		getPlantCR=NULL;
		getPlantCR=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLA");

		getPlantView=NULL;
		getPlantView=(char *) MEM_alloc(100);
		tc_strcpy(getPlantView,"APLA");

		getPlantViewName=NULL;
		getPlantViewName=(char *) MEM_alloc(100);
		tc_strcpy(getPlantViewName,"T5_APLAView");

		getPlantCurntMaskFlag=NULL;
		getPlantCurntMaskFlag=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCurntMaskFlag,"bl_occ_t5_CurrentViewMaskA");

		printf("OPCS Name After Conversion ==> [%s]:[%s]:[%s]\n",getPlantCurntMaskFlag,getPlantCR,getPlantView);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName8")==0)
	{
		getPlantCR=NULL;
		getPlantCR=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLJ");

		getPlantView=NULL;
		getPlantView=(char *) MEM_alloc(100);
		tc_strcpy(getPlantView,"APLJ");

		getPlantViewName=NULL;
		getPlantViewName=(char *) MEM_alloc(100);
		tc_strcpy(getPlantViewName,"T5_APLJView");

		getPlantCurntMaskFlag=NULL;
		getPlantCurntMaskFlag=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCurntMaskFlag,"bl_occ_t5_CurrentViewMaskJ");

		printf("OPCS Name After Conversion ==> [%s]:[%s]:[%s]\n",getPlantCurntMaskFlag,getPlantCR,getPlantView);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName9")==0)
	{
		getPlantCR=NULL;
		getPlantCR=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLL");

		getPlantView=NULL;
		getPlantView=(char *) MEM_alloc(100);
		tc_strcpy(getPlantView,"APLL");

		getPlantViewName=NULL;
		getPlantViewName=(char *) MEM_alloc(100);
		tc_strcpy(getPlantViewName,"T5_APLLView");

		getPlantCurntMaskFlag=NULL;
		getPlantCurntMaskFlag=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCurntMaskFlag,"bl_occ_t5_CurrentViewMaskL");

		printf("OPCS Name After Conversion ==> [%s]:[%s]:[%s]\n",getPlantCurntMaskFlag,getPlantCR,getPlantView);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName11")==0)
	{
		getPlantCR=NULL;
		getPlantCR=(char *) MEM_alloc(100);

		tc_strcpy(getPlantCR,"BOMViewClosureRuleAPLD");

		getPlantView=NULL;
		getPlantView=(char *) MEM_alloc(100);
		tc_strcpy(getPlantView,"APLD");

		getPlantViewName=NULL;
		getPlantViewName=(char *) MEM_alloc(100);
		tc_strcpy(getPlantViewName,"T5_APLDView");

		getPlantCurntMaskFlag=NULL;
		getPlantCurntMaskFlag=(char *) MEM_alloc(100);
		tc_strcpy(getPlantCurntMaskFlag,"bl_occ_t5_CurrentViewMaskD");


		printf("OPCS Name After Conversion ==> [%s]:[%s]:[%s]\n",getPlantCurntMaskFlag,getPlantCR,getPlantView);fflush(stdout);
	}
	else
	{
		printf("\nSYNTEX ERROR :: \n");fflush(stdout);
		printf("\n PLEASE ENTER CORRECT PLANTNAME(EX:PlantName1,PlantName13,PlantName2 etc..)\n");fflush(stdout);
		printf("\nTRY AGAIN !!!\n");fflush(stdout);
		return status;
	}

	ITK_CALL(ITK_auto_login( ));  
    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	//ITK_CALL(ITK_init_module("hrb293377","XYT1ESA","APLCAR"));
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Auto login .......");fflush(stdout);

	fperror1=fopen("error_ERCBOMCreation.log","a+");
	printf("\n CALLING BOM VALIDATE FUNCTION.. \n");fflush(stdout);
	//validateBOMPart(inputfile,fperror1);
	//fperror_file=fopen("error_ERCBOMCreation.log","r");

//	if(fperror_file==NULL)
//	{
//		printf("\n fperror_file is NULL \n");fflush(stdout);
//
//	}
//	else
//	{
//		printf("\n fperror_file is NOT NULL \n");fflush(stdout);
//		res = ftell(fperror_file);
//		printf("Size of the file is %ld bytes \n", res); 
//
//	}



	
	if(res>0)
	{
		printf("Error log is not null...Hence not going for structure creation..\n");fflush(stdout);
	
	}
	else
	{	
		printf("Error log is null...going for structure creation..\n");fflush(stdout);

		fp=fopen(inputfile,"r");
		fpexception=fopen("Exception_ERCAPLBOMCreation.log","a+");
		if(fp!=NULL)
		{
			
			fperror=fopen("error_ERCAPLBOMCreation.log","a+");

			inputline=(char *) MEM_alloc(1000);
			while(fgets(inputline,1000,fp)!=NULL)
			{
				fputs(inputline,stdout);


				mod_Part=NULL;
				prent_Orgid=NULL;
				parent_item_id=NULL;
				parent_item_revision_id=NULL;
				parent_item_sequence_id=NULL;
				child_item_id=NULL;
				child_item_revision_id=NULL;
				child_item_sequence_id=NULL;
				child_item_Qty=NULL;
				newitemAlreadyFnd=0;

				mod_Part=strtok(inputline,"^");  //1
				prent_Orgid=strtok(NULL,"^");  //2
				parent_item_id=strtok(NULL,"^"); //3
				parent_item_revision_id=strtok(NULL,"^"); //4
				parent_item_sequence_id=strtok(NULL,"^"); //4
				child_item_id=strtok(NULL,"^"); //2
				child_item_revision_id=strtok(NULL,"^"); //3
				child_item_sequence_id=strtok(NULL,"^"); //4
				child_item_Qty=strtok(NULL,"^"); //4


				printf("\n mod_Part .......%s,%s,%s,%s,%s,%s,%s,%s",mod_Part,parent_item_id,parent_item_revision_id,parent_item_sequence_id,child_item_id,child_item_revision_id,
				child_item_sequence_id,child_item_Qty);fflush(stdout);

				attrs[0] ="item_id";
				values[0] = (char *)parent_item_id;
				ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
				if(n_tags_found==0)
				{
					if((tc_strcmp(prent_Orgid,"ERC")==0))
					{				
						fprintf(fperror,"ERC Parent Part not found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);
						printf("ERC Parent Part not found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);
						continue;
					}
					else
					{
						fprintf(fperror,"APL Parent Part not found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);
						printf("APL Parent Part not found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);
						continue;				
					
					}

				}
				else
				{
					
						parent_item = tags_found[0];
						printf("Item already exists\n");fflush(stdout);

						parentitemRevSeq = NULL;
						parentitemRevSeq=(char *) MEM_alloc(32);
						strcpy(parentitemRevSeq,parent_item_revision_id);
						strcat(parentitemRevSeq,";");
						strcat(parentitemRevSeq,parent_item_sequence_id);
						printf(" parentitemRevSeq [%s]\n",parentitemRevSeq);fflush(stdout);	
						
						ITK_CALL(ITEM_find_revision(parent_item,parentitemRevSeq,&parent_rev));
						if(parent_rev != NULLTAG)
						{
											
							if((tc_strcmp(mod_Part,"DELETED")==0) && (tc_strcmp(prent_Orgid,"ERC")==0))
							{
								ITKCALL(BOM_create_window (&window));
								ITKCALL(CFM_find("ERC release and above APLC", &revRule));
								if (revRule != NULLTAG)
								{
									printf("\nFind revRule\n");fflush(stdout);
									ITKCALL(BOM_set_window_config_rule(window,revRule));
								}

								scope=PIE_TEAMCENTER;
								ITKCALL(PIE_find_closure_rules2("BOMViewClosureRuleERC",scope,&n_closure_tags,&closure_tags));
								printf("\n n_closure_tags:%d ..............",n_closure_tags);fflush(stdout);
								if(n_closure_tags==1)
								{
									closure_tag=closure_tags[0];
									ITKCALL(BOM_window_set_closure_rule(window,closure_tag,0,NULL,NULL));
								}

								ITKCALL(ITEM_rev_list_bom_view_revs(parent_rev, &n_values_bvr_bs, &bvr_assy_part));
								printf("\n n_values_bvr_bs %d ",n_values_bvr_bs);fflush(stdout);
								for(assbvr=0;assbvr<n_values_bvr_bs;assbvr++)
								{
									ITKCALL(AOM_ask_value_string(bvr_assy_part[assbvr],"object_name",&view_name));
									printf("\n view_name %s",view_name);fflush(stdout);
									partName=tc_strtok(view_name,"-");
									newViewName=tc_strtok(NULL,"-");
									printf("\n newViewName %s,partName %s",newViewName,partName);fflush(stdout);
									
									if(tc_strcmp(newViewName,"View")==0)
									{
										ercViewFound++;			
										break;
									}
									
									
								}

								if(ercViewFound>0)
								{
									
									ITKCALL(BOM_set_window_top_line(window,parent_item,parent_rev,bvr_assy_part[assbvr],&top_line));
									ITKCALL(BOM_line_ask_child_lines (top_line, &n, &children1));
									printf("\n\n\t\t No of child objects are n : %d\n",n);fflush(stdout);
								}
								else
								{
									fprintf(fperror,"ERC BVR(View) Not Found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);
									continue;
								}	
								
								for (k1 = 0; k1 < n; k1++)
								{
									ITKCALL(BOM_line_unpack (children1[k1]));

									ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
									ITKCALL(BOM_line_ask_attribute_tag(children1[k1], iChildItemTag, &t_ChildItemRev));
									if(t_ChildItemRev!=NULLTAG)
									{

										ITKCALL(AOM_ask_value_string(t_ChildItemRev,"item_id",&childItemName))
										printf("\n D.child ItemName:%s ..............",childItemName);fflush(stdout);
										
										if(tc_strcmp(childItemName,child_item_id)==0)
										{
											int ercFlag_attribute;

											ITKCALL(BOM_line_look_up_attribute (getPlantCurntMaskFlag,&ercFlag_attribute));
											Bomline_tag = children1[k1];

											ITKCALL(BOM_line_ask_attribute_string(Bomline_tag,ercFlag_attribute,&childItemFlag));
											printf("\n D.childItemFlag:%s ..............",childItemFlag);fflush(stdout);

											itemDeleted++;

											if(tc_strcmp(childItemFlag,"")==0)
											{
												ITKCALL(BOM_line_set_attribute_string(Bomline_tag, ercFlag_attribute, "0--"));
												printf("\n D.FLAG SET 0 to child_item_id:%s ..............",child_item_id);fflush(stdout);
												
											}
											else
											{
												fprintf(fpexception,"Child Part :[%s],[%s],[%s] already presnt 0 Flag in Parent Part  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
												child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);

												if(tc_strcmp(childItemFlag,"0")==0)
												{
													ITKCALL(BOM_line_set_attribute_string(Bomline_tag, ercFlag_attribute, "0--"));
													printf("\n D.FLAG SET 0-- to child_item_id:%s ..............",child_item_id);fflush(stdout);
												}
												
												printf("Child Part :[%s],[%s],[%s] already presnt 0 Flag in Parent Part  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
												child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);
											}
											break;

										}
									}
									else
									{
										printf("\n D.t_ChildItemRev is NULLTAG..");fflush(stdout);
									}

									
								}
								ITKCALL(BOM_close_window(window));

								if(itemDeleted==0)
								{
									fprintf(fperror,"Child Part :[%s],[%s],[%s] not presnt in Parent Part  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
									child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);
									
									printf("Child Part :[%s],[%s],[%s] not presnt in Parent Part  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
									child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);

									
								}

							
							}
							else if((tc_strcmp(mod_Part,"ADDED")==0) && (tc_strcmp(prent_Orgid,"ERC")==0))
							{
							
								ITKCALL(BOM_create_window (&newwindow));
								ITKCALL(CFM_find("ERC release and above APLC", &newrevRule));
								if (newrevRule != NULLTAG)
								{
									printf("\nFind newrevRule\n");fflush(stdout);
									ITKCALL(BOM_set_window_config_rule(newwindow,newrevRule));
								}
								ITKCALL(ITEM_rev_list_bom_view_revs(parent_rev, &new_values_bvr_bs, &new_bvr_assy_part));
								printf("\n new_values_bvr_bs %d ",new_values_bvr_bs);fflush(stdout);
								
								for(newassbvr=0;newassbvr<new_values_bvr_bs;newassbvr++)
								{
									ITKCALL(AOM_ask_value_string(new_bvr_assy_part[newassbvr],"object_name",&new_view_name));
									printf("\n new_view_name %s",new_view_name);fflush(stdout);
									new_partName=tc_strtok(new_view_name,"-");
									new_AViewName=tc_strtok(NULL,"-");
									printf("\n new_AViewName %s,new_partName %s",new_AViewName,new_partName);fflush(stdout);
									
									if(tc_strcmp(new_AViewName,"View")==0)
									{
										newercViewFound++;			
										break;
									}
									
									
								}

								if(newercViewFound>0)
								{
									
									ITKCALL(BOM_set_window_top_line(newwindow,parent_item,parent_rev,new_bvr_assy_part[assbvr],&new_A_top_line));
									ITKCALL(BOM_line_ask_child_lines (new_A_top_line, &an, &new_add_children1));
									printf("\n\n\t\t No of child objects are an : %d\n",an);fflush(stdout);
								}
								else
								{
									//fprintf(fperror,"ERC BVR(View) Not Found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);
									//continue;

									ITKCALL(PS_find_view_type(getPlantViewName,&view_type_tag ));
									if(view_type_tag==NULLTAG)
									{
										printf("\n ERC PART APL VIEW CHECK view_type_tag is null");fflush(stdout);
									}
									else
									{
										printf("\n ERC PART APL VIEW CHECK  view_type_tag is not null ");fflush(stdout);
									}


									if(new_values_bvr_bs==0)
									{
										
										ITKCALL(ITEM_list_bom_views(parent_item,&itembvr_count,&item_bv_list));
										if(itembvr_count==0)
										{
											ITKCALL(PS_create_bom_view (view_type_tag,"","",parent_item,&bom_view_item));
											ITKCALL(AOM_save(bom_view_item));
											ITKCALL(AOM_save(parent_item));
											ITKCALL(AOM_unlock(parent_item));
										}
										else
										{
											for(newitembvr=0;newitembvr<itembvr_count;newitembvr++)
											{
												ITKCALL(AOM_ask_value_string(item_bv_list[newitembvr],"object_name",&view_name_item));
												printf("\n ERC PART APL VIEW CHECK view_name_item %s",view_name_item);fflush(stdout);
												if(tc_strstr(view_name_item,getPlantView)!=NULL)
												{
													aplcViewItmFound++;
													bom_view_item=item_bv_list[newitembvr];
													break;
												}
												else
												{
													ITKCALL(PS_create_bom_view (view_type_tag,"","",parent_item,&bom_view_item));
													ITKCALL(AOM_save(bom_view_item));
													ITKCALL(AOM_save(parent_item));
													ITKCALL(AOM_unlock(parent_item));
												
												}
											}	
										
										}
										
										ITKCALL(PS_create_bvr (bom_view_item,"","",false,parent_rev,&bvrs_aplpartRev1));
										ITKCALL(AOM_save(bvrs_aplpartRev1));
										ITKCALL(AOM_save(parent_rev));
										ITKCALL(AOM_unlock(parent_rev));
										aplcViewFound++;

									}
									else
									{
										printf("\n ERC PART APL VIEW CHECK  111...BVR already found");fflush(stdout);
										newassbvr=0;
										for(newassbvr=0;newassbvr<new_values_bvr_bs;newassbvr++)
										{
											ITKCALL(AOM_ask_value_string(new_bvr_assy_part[newassbvr],"object_name",&view_name));
											printf("\n ERC PART APL VIEW CHECK  view_name %s",view_name);fflush(stdout);
											if(tc_strstr(view_name,getPlantView)!=NULL)
											{
												aplcViewFound++;
												break;
											}
											
											
										}
									
									}

									printf("\n ERC PART APL VIEW CHECK  111aplcViewFound %d ",aplcViewFound);fflush(stdout);

									if(aplcViewFound==0)
									{
										ITKCALL(ITEM_list_bom_views(parent_item,&itembvr_count,&item_bv_list));
										if(itembvr_count==0)
										{
											ITKCALL(PS_create_bom_view (view_type_tag,"","",parent_item,&bom_view_item));
											ITKCALL(AOM_save(bom_view_item));
											ITKCALL(AOM_save(parent_item));
											ITKCALL(AOM_unlock(parent_item));
										}
										else
										{
											for(newitembvr=0;newitembvr<itembvr_count;newitembvr++)
											{
												ITKCALL(AOM_ask_value_string(item_bv_list[newitembvr],"object_name",&view_name_item));
												printf("\n ERC PART APL VIEW CHECK view_name_item %s",view_name_item);fflush(stdout);
												if(tc_strstr(view_name_item,getPlantView)!=NULL)
												{
													bom_view_item=item_bv_list[newitembvr];
													aplcViewItmFound++;
													break;
												}
											}	
										
										}
										
										ITKCALL(ITEM_rev_list_bom_view_revs( parent_rev, &bvr_count_aplpartRev, &bvrs_aplpartRev));
										printf("\n ERC PART APL VIEW CHECK  111 bvr_count_aplpartRev %d ",bvr_count_aplpartRev);fflush(stdout);

										ITKCALL(PS_create_bvr (bom_view_item,"","",false,parent_rev,&bvrs_aplpartRev1));
										ITKCALL(AOM_save(bvrs_aplpartRev1));
										ITKCALL(AOM_save(parent_rev));
										ITKCALL(AOM_unlock(parent_rev));					
									
									
									}

										ITKCALL(ITEM_rev_list_bom_view_revs( parent_rev, &bvr_count_BSCRev, &new_bvr_assy_part));
										printf("\n ERC PART APL VIEW CHECK  bvr_count_BSCRev %d ",bvr_count_BSCRev);fflush(stdout);

										
										assbvr=0;

										for (assbvr=0;assbvr<bvr_count_BSCRev;assbvr++)
										{
											
											printf("\n ERC PART APL VIEW CHECK  Inside for loop ..");fflush(stdout);
											ITKCALL(AOM_ask_value_string(new_bvr_assy_part[assbvr],"object_name",&view_name_assy));
											printf("\n ERC PART APL VIEW CHECK  view_name_assy %s",view_name_assy);fflush(stdout);
											if(tc_strstr(view_name_assy,getPlantView)!=NULL)
											{
												//printf("\n Creating relation .....");fflush(stdout);
												printf("\n ERC PART APL VIEW CHECK  111Checking for relation .....");fflush(stdout);
												//bvr_tag=bvrs_BSCRev[assbvr];

												
												ITKCALL(BOM_set_window_top_line(newwindow,parent_item,parent_rev,new_bvr_assy_part[assbvr],&new_A_top_line));
												ITKCALL(BOM_save_window(newwindow));
												ITKCALL(BOM_line_ask_child_lines (new_A_top_line, &an, &new_add_children1));
												printf("\n\n\t\t ERC PART APL VIEW CHECK  111 No of child objects are an : %d\n",an);fflush(stdout);
								
												break;

											}
										}
									
								}
								
								ak = 0;
								for (ak = 0; ak < an; ak++)
								{
									ITKCALL(BOM_line_unpack (new_add_children1[ak]));
									new_Bomline_tag = new_add_children1[ak];

									ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &new_AChildItemTag));
									ITKCALL(BOM_line_ask_attribute_tag(new_Bomline_tag, new_AChildItemTag, &new_AChildItemRev));
									if(new_AChildItemRev!=NULLTAG)
									{
										ITKCALL(AOM_ask_value_string(new_AChildItemRev,"item_id",&new_AchildItemName))
										printf("\n A. child new_AchildItemName:%s ..............",new_AchildItemName);fflush(stdout);

										//ITKCALL(AOM_ask_value_string(new_A_top_line,"bl_quantity",&new_AchildItemQty));
										//printf("\n A. new_AchildItemQty:%s ..............",new_AchildItemQty);fflush(stdout);

										if(tc_strcmp(new_AchildItemName,child_item_id)==0)
										{
											
											int ercFlag_attribute;

											ITKCALL(BOM_line_look_up_attribute (getPlantCurntMaskFlag,&ercFlag_attribute));
											
											ITKCALL(BOM_line_ask_attribute_string(new_Bomline_tag,ercFlag_attribute,&childItemFlag));
											printf("\n D.childItemFlag:%s ..............",childItemFlag);fflush(stdout);

											
											if(tc_strcmp(childItemFlag,"1")==0)
											{
												ITKCALL(BOM_line_set_attribute_string(new_Bomline_tag, ercFlag_attribute, "-12"));
												printf("\n D.FLAG SET -12 to child_item_id:%s ..............",child_item_id);fflush(stdout);
												
											}
											fprintf(fpexception,"Child Part :[%s],[%s],[%s] already present in Parent Part  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
											child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);
											printf("A.Child Part :[%s],[%s],[%s] already present in Parent Part  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
											child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);
											newitemAlreadyFnd++;
											break;
										}
									}
									else
									{
										printf("\n A.Adding Child Part Code t_ChildItemRev is NULLTAG..");fflush(stdout);
									}
								
								}
								printf("\n newitemAlreadyFnd %d",newitemAlreadyFnd);fflush(stdout);
								if(newitemAlreadyFnd==0)
								{
									
									attrs1[0] ="item_id";
									values1[0] = (char *)child_item_id;
									ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs1, values1, &n_tags_found_prt, &tags_found_prt));
									printf("\n n_tags_found_prt %d",n_tags_found_prt);fflush(stdout);
									if(n_tags_found_prt==0)
									{
										fprintf(fperror,"Child Part :[%s],[%s],[%s] not Found.So cannot create realtion with ERC Part  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
										child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);
										printf("A.Child Part :[%s],[%s],[%s] not Found.So cannot create realtion with ERC Part  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
										child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);

										
									}
									else
									{
										item_part = tags_found_prt[0];
										consumableQtyD=atof(child_item_Qty);
										ITKCALL(PS_create_occurrences(new_bvr_assy_part[assbvr],item_part,NULLTAG,1,&occurrences));
										ITKCALL(PS_set_occurrence_qty(new_bvr_assy_part[assbvr], occurrences[0], consumableQtyD ));
										ITKCALL(AOM_save(new_bvr_assy_part[assbvr]));
										printf("\n A.FLAG SET 1 to child_item_id:%s ..............",child_item_id);fflush(stdout);

										ITKCALL(AOM_refresh(parent_rev,0));

										ITKCALL(BOM_set_window_top_line(newwindow,parent_item,parent_rev,new_bvr_assy_part[assbvr],&new_top_line));
										ITKCALL(BOM_line_ask_child_lines (new_top_line, &newCnt, &newchildren));
										printf("\n\n\t\t A.No of child objects are n : %d:%d\n",newCnt,Ccnt);fflush(stdout);
										Ccnt = 0;
										for (Ccnt = 0; Ccnt < newCnt; Ccnt++)
										{
											BOM_line_unpack (newchildren[Ccnt]);
											new_Bomline_tag = newchildren[Ccnt];

											ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &newChildItemTag));
											ITKCALL(BOM_line_ask_attribute_tag(new_Bomline_tag, newChildItemTag, &new_tChildItemRev));
											if(new_tChildItemRev!=NULLTAG)
											{
												ITKCALL(AOM_ask_value_string(new_tChildItemRev,"item_id",&newItemName));
												printf("\n\n\t\t A.Part Compare : %s:%s \n",newItemName,child_item_id);fflush(stdout);
												
												if(tc_strcmp(newItemName,child_item_id)==0)
												{
													ITKCALL(BOM_line_look_up_attribute (getPlantCurntMaskFlag,&aplFlag_attribute));										
													ITKCALL(BOM_line_set_attribute_string(new_Bomline_tag, aplFlag_attribute, "-12"));
													break;
												}
											}
											else
											{
												printf("\n TAG IS NULL...new_tChildItemRev\n");fflush(stdout);	
											}
										}
									}						
								
								}
								ITKCALL(BOM_close_window(newwindow));

							
							}
							else if((tc_strcmp(mod_Part,"ADDED")==0) && (tc_strstr(prent_Orgid,"APL")!=NULL))
							{
									tempmat1=strtok(NULL,",");

									printf("\n matrixline [%s]",tempmat1);fflush(stdout);
									
									ITK_CALL(BOM_create_window (&window));
									ITK_CALL(CFM_find("ERC release and above APLC", &revRule));
									if (revRule != NULLTAG)
									{
										printf("\nFind revRule\n");fflush(stdout);
										ITK_CALL(BOM_set_window_config_rule(window,revRule));
									}

									scope=PIE_TEAMCENTER;
									ITK_CALL(PIE_find_closure_rules2(getPlantCR,scope,&n_closure_tags,&closure_tags));
									printf("\n n_closure_tags:%d ..............",n_closure_tags);fflush(stdout);
									if(n_closure_tags==1)
									{
										closure_tag=closure_tags[0];
										ITK_CALL(BOM_window_set_closure_rule(window,closure_tag,0,NULL,NULL));
									}

									aplChildPartF=0;

									ITKCALL(ITEM_rev_list_bom_view_revs(parent_rev, &n_values_bvr_bs, &bvr_aplassy_part));
									printf("\n n_values_bvr_bs %d ",n_values_bvr_bs);fflush(stdout);

									ITKCALL(PS_find_view_type(getPlantViewName,&view_type_tag ));
									if(view_type_tag==NULLTAG)
									{
										printf("\n view_type_tag is null");fflush(stdout);
									}
									else
									{
										printf("\n view_type_tag is not null ");fflush(stdout);
									}

									aplcViewFound=0;
									if(n_values_bvr_bs==0)
									{
										
										ITKCALL(ITEM_list_bom_views(parent_item,&itembvr_count,&item_bv_list));
										if(itembvr_count==0)
										{
											ITKCALL(PS_create_bom_view (view_type_tag,"","",parent_item,&bom_view_item));
											ITKCALL(AOM_save(bom_view_item));
											ITKCALL(AOM_save(parent_item));
											ITKCALL(AOM_unlock(parent_item));
										}
										else
										{
											for(newitembvr=0;newitembvr<itembvr_count;newitembvr++)
											{
												ITKCALL(AOM_ask_value_string(item_bv_list[newitembvr],"object_name",&view_name_item));
												printf("\n ERC PART APL VIEW CHECK view_name_item %s",view_name_item);fflush(stdout);
												if(tc_strstr(view_name_item,getPlantView)!=NULL)
												{
													bom_view_item=item_bv_list[newitembvr];
													aplcViewItmFound++;
													break;
												}
											}	
										
										}
										

										//ITKCALL(PS_create_bom_view (view_type_tag,"","",parent_item,&bom_view_item));
										//ITKCALL(AOM_save(bom_view_item));
										//ITKCALL(AOM_save(parent_item));
										//ITKCALL(AOM_unlock(parent_item));

										ITKCALL(ITEM_rev_list_bom_view_revs( parent_rev, &bvr_count_aplpartRev, &bvrs_aplpartRev));
										printf("\n bvr_count_aplpartRev %d ",bvr_count_aplpartRev);fflush(stdout);


										ITKCALL(PS_create_bvr (bom_view_item,"","",false,parent_rev,&bvrs_aplpartRev1));
										ITKCALL(AOM_save(bvrs_aplpartRev1));
										ITKCALL(AOM_save(parent_rev));
										ITKCALL(AOM_unlock(parent_rev));
										aplcViewFound++;

									}
									else
									{
										printf("\n BVR already found");fflush(stdout);
										for(assbvr=0;assbvr<n_values_bvr_bs;assbvr++)
										{
											ITKCALL(AOM_ask_value_string(bvr_aplassy_part[assbvr],"object_name",&view_name));
											printf("\n view_name %s",view_name);fflush(stdout);
											if(tc_strstr(view_name,getPlantView)!=NULL)
											{
												aplcViewFound++;
												break;
											}
											
											
										}
									
									}

									printf("\n aplcViewFound %d ",aplcViewFound);fflush(stdout);
									if(aplcViewFound==0)
									{
										ITKCALL(ITEM_list_bom_views(parent_item,&itembvr_count,&item_bv_list));
										if(itembvr_count==0)
										{
											ITKCALL(PS_create_bom_view (view_type_tag,"","",parent_item,&bom_view_item));
											ITKCALL(AOM_save(bom_view_item));
											ITKCALL(AOM_save(parent_item));
											ITKCALL(AOM_unlock(parent_item));
										}
										else
										{
											for(newitembvr=0;newitembvr<itembvr_count;newitembvr++)
											{
												ITKCALL(AOM_ask_value_string(item_bv_list[newitembvr],"object_name",&view_name_item));
												printf("\n ERC PART APL VIEW CHECK view_name_item %s",view_name_item);fflush(stdout);
											}	
										
										}
										
										//ITKCALL(PS_create_bom_view (view_type_tag,"","",parent_item,&bom_view_item));
										//ITKCALL(AOM_save(bom_view_item));
										//ITKCALL(AOM_save(parent_item));
										//ITKCALL(AOM_unlock(parent_item));

										ITKCALL(ITEM_rev_list_bom_view_revs( parent_rev, &bvr_count_aplpartRev, &bvrs_aplpartRev));
										printf("\n 111 bvr_count_aplpartRev %d ",bvr_count_aplpartRev);fflush(stdout);

										ITKCALL(PS_create_bvr (bom_view_item,"","",false,parent_rev,&bvrs_aplpartRev1));
										ITKCALL(AOM_save(bvrs_aplpartRev1));
										ITKCALL(AOM_save(parent_rev));
										ITKCALL(AOM_unlock(parent_rev));								
									
									
									}


									attrs1[0] ="item_id";
									values1[0] = (char *)child_item_id;
									ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs1, values1, &n_tags_found_prt, &tags_found_prt));
									if(n_tags_found_prt==0)
									{
										fprintf(fperror,"Child Part :[%s],[%s],[%s] not Found.So cannot create realtion with parent  :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
										child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);
										printf("\n A.Child Part :[%s],[%s],[%s] not Found.So cannot create realtion with parent :[%s],[%s],[%s]\n",child_item_id,child_item_revision_id,
										child_item_sequence_id,parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);
										
										
										continue;
									}
									else
									{
										item_part = tags_found_prt[0];
										bvr_count_BSCRev=0;
										printf("\n Inside Creating relation Process.....");fflush(stdout);
										ITKCALL(ITEM_rev_list_bom_view_revs( parent_rev, &bvr_count_BSCRev, &bvrs_BSCRev));
										printf("\n bvr_count_BSCRev %d ",bvr_count_BSCRev);fflush(stdout);

										
										bvrCnt=0;

										for (bvrCnt=0;bvrCnt<bvr_count_BSCRev;bvrCnt++)
										{
											
											printf("\n Inside for loop ..");fflush(stdout);
											ITKCALL(AOM_ask_value_string(bvrs_BSCRev[bvrCnt],"object_name",&view_name_assy));
											printf("\n view_name_assy %s",view_name_assy);fflush(stdout);
											if(tc_strstr(view_name_assy,getPlantView)!=NULL)
											{
												//printf("\n Creating relation .....");fflush(stdout);
												printf("\n Checking for relation .....");fflush(stdout);
												//bvr_tag=bvrs_BSCRev[bvrCnt];

												ITKCALL(BOM_set_window_top_line(window,parent_item,parent_rev,bvrs_BSCRev[bvrCnt],&top_line));
												ITKCALL(BOM_save_window(window));
												ITKCALL(BOM_line_ask_child_lines(top_line, &childCnt, &children1));
												printf("\n\n\t\t No of child objects are n : %d\n",childCnt);fflush(stdout);
												for (kC = 0; kC < childCnt; kC++)
												{
													BOM_line_unpack (children1[kC]);
													Bomline_tag = children1[kC];

													ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
													ITKCALL(BOM_line_ask_attribute_tag(Bomline_tag, iChildItemTag, &t_ChildItemRev));
													if(t_ChildItemRev!=NULLTAG)
													{
														ITKCALL(AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemName));
														printf("\n\n\t\t Part Compare : %s: %s\n",ItemName,child_item_id);fflush(stdout);
													
														if(tc_strcmp(ItemName,child_item_id)==0)
														{
															int ercFlag_attribute;

															ITKCALL(BOM_line_look_up_attribute (getPlantCurntMaskFlag,&ercFlag_attribute));															

															ITKCALL(BOM_line_ask_attribute_string(Bomline_tag,ercFlag_attribute,&childItemFlag));
															printf("\n D.childItemFlag:%s ..............",childItemFlag);fflush(stdout);

															
															if(tc_strcmp(childItemFlag,"1")==0)
															{
																ITKCALL(BOM_line_set_attribute_string(Bomline_tag, ercFlag_attribute, "-12"));
																printf("\n D.1FLAG SET -12 to child_item_id:%s ..............",child_item_id);fflush(stdout);
																
															}
															printf("\n PART ALREADY PRESENT.....");fflush(stdout);
															aplChildPartF++;
															break;
														}
													}
													
												}
												
												if(aplChildPartF==0)
												{
													printf("\n CREATING RELATION OF CHILD & PARENT.....");fflush(stdout);
													ITKCALL(PS_create_occurrences(bvrs_BSCRev[bvrCnt],item_part,NULLTAG,1,&occurrences));
													
													//tempmat1=strtok(NULL,"^");
													printf("\n tempmat1 ::111 %s,",tempmat1);fflush(stdout);

													for(imat=0;imat<4;imat++)
													{
														printf("\n For loop Instance name \n");fflush(stdout);
														for(jmat=0;jmat<4;jmat++)
														{
															
															//printf("\n tempmat %s \n",tempmat);fflush(stdout);
															if(imat==0 && jmat==0)
															{
																tempmat=strtok(tempmat1,"^");
																mat[imat][jmat]=strtod(tempmat,NULL);
																printf("\n Data : tempmat %s",tempmat);fflush(stdout);
																printf("\n Data1 ::111 %f,",mat[imat][jmat]);fflush(stdout);
															}else
															{
																//printf("\n tempmat1 %s \n",tempmat1);fflush(stdout);
																tempmat=strtok(NULL,"^");
																printf("\n tempmat %s \n",tempmat);fflush(stdout);
																mat[imat][jmat]=strtod(tempmat,NULL);
																printf("\n Data22::111 %f,",mat[imat][jmat]);fflush(stdout);
															}
														}
													}

													intancename2= (char *) MEM_alloc( 200 * sizeof(char) );
													intancename=strtok(NULL,"^");
													printf("\n Instance name :[%s]\n",intancename);fflush(stdout);
													for(clean=0;clean<200;clean++)
													{
														intancename2[clean]=NULL;
													}

													if(omat<1)
													{
														printf("\n Setting matrix \n\n");fflush(stdout);

														for(imat=0;imat<4;imat++)
														{
															for(jmat=0;jmat<4;jmat++)
															{
																printf("111 %10lf,",mat[imat][jmat]);fflush(stdout);
																printf("123 %f,",mat[imat][jmat]);fflush(stdout);
															}
															printf("\n");fflush(stdout);
														}

														ITKCALL(PS_set_plmxml_transform(bvrs_BSCRev[bvrCnt],occurrences[omat],mat));
														length=strlen(intancename)-1;
														strncpy (intancename2,intancename,length);

														ITKCALL(PS_set_occurrence_name(bvrs_BSCRev[bvrCnt], occurrences[omat], intancename2));
														ITKCALL(PS_find_note_type("catiaOccurrenceName",&tag_occ_name));
														ITKCALL(PS_set_occurrence_note_text(bvrs_BSCRev[bvrCnt], occurrences[omat],tag_occ_name,intancename2));

														ITKCALL(PS_set_seq_no(bvrs_BSCRev[bvrCnt], occurrences[omat],child_item_sequence_id));

														printf("\n Omat is[%d]",omat);fflush(stdout);

														omat++;
													}

													MEM_free(intancename2);intancename2=NULL;

													consumableQtyD=atof(child_item_Qty);
													ITKCALL(PS_set_occurrence_qty(bvrs_BSCRev[bvrCnt], occurrences[0], consumableQtyD ));							
													ITKCALL(AOM_save(bvrs_BSCRev[bvrCnt]));
													ITKCALL(AOM_refresh(parent_rev,0));

													//ITKCALL(BOM_set_window_top_line(window,parent_item,parent_rev,NULLTAG,&top_line));

													

													ITKCALL(BOM_set_window_top_line(window,parent_item,parent_rev,bvrs_BSCRev[bvrCnt],&top_line));
													ITKCALL(BOM_save_window(window));
													ITKCALL(BOM_line_ask_child_lines(top_line, &childCnt, &children1));
													printf("\n\n\t\t No of child objects are n : %d\n",childCnt);fflush(stdout);
													for (kC = 0; kC < childCnt; kC++)
													{
														BOM_line_unpack (children1[kC]);
														Bomline_tag = children1[kC];

														ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
														ITKCALL(BOM_line_ask_attribute_tag(Bomline_tag, iChildItemTag, &t_ChildItemRev));
														if(t_ChildItemRev!=NULLTAG)
														{
															ITKCALL(AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemName));
															printf("\n\n\t\t Part Compare : %s: %s\n",ItemName,child_item_id);fflush(stdout);

															if(tc_strcmp(ItemName,child_item_id)==0)
															{
																ITKCALL(BOM_line_look_up_attribute (getPlantCurntMaskFlag,&aplFlag_attribute));										
																ITKCALL(BOM_line_set_attribute_string(Bomline_tag, aplFlag_attribute, "-12"));
																printf("\n Updating CAR Flag");fflush(stdout);
																break;
															}
														}
														
													}
												}
												
												
												break;
											}
										}


									}
							
								ITKCALL(BOM_close_window(window));
							}
						
						}
						else
						{
							if((tc_strcmp(prent_Orgid,"ERC")==0))
							{				
								fprintf(fperror,"ERC Parent Part REV not found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);
								printf("ERC Parent Part REV not found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);
								continue;
							}
							else
							{
								fprintf(fperror,"APL Parent Part REV not found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);
								printf("APL Parent Part REV not found  :[%s],[%s],[%s]\n",parent_item_id,parent_item_revision_id,parent_item_sequence_id);fflush(stdout);
								continue;				

							}
						
						}		

				}


			}

			printf("-----------PART LIST COMPLETED-----------------------------------------\n");fflush(stdout);	
		}

	}
	

	//ITK_CALL(POM_logout(false));
	
	//if(fp)fclose(fp);fp=NULL;
	//if(fperror)fclose(fperror);fperror=NULL;
	//if(fpexception)fclose(fpexception);fpexception=NULL;
	//if(fperror_file)fclose(fperror_file);fperror_file=NULL;
	
	printf("-----------PART LOADING SUCCESS-----------------------------------------\n");fflush(stdout);
	return status;

}