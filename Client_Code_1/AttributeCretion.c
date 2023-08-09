/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  Author		 :   Raghavendra B T
*  Module		 :   TCUA DataSet Uploader
*  Code			 :   DataSetLoad.c
*  Created on	 :   March 25th, 2015
*
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes
***************************************************************************/

#include <tccore/aom.h>
#include <tccore/aom_prop.h>
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
#include <ae/dataset.h>
#include <stdlib.h>
#include <tccore/libtccore_exports.h>
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

#define TCTYPE_name_size_c 100



extern int ITK_user_main (int argc, char ** argv )
{

    int status;

	int						i												= 0;

	char					*user										= NULL;
	char					*pass										= NULL;
	char					*group										= NULL;
	char					*RefNo										= NULL;

	char					*itemId											= NULL;
	char					*revId											= NULL;
	char					*revisionID										= NULL;

	char					*Status											= NULL;
	char					*relation_input									= NULL;
	char					*file_type										= NULL;

	char					*error_Text										= NULL;
	tag_t					JTDataset										= NULLTAG;
	tag_t					relation_JTDataset								= NULLTAG;

//	char  					line[500]										= {'\0'};
	FILE					*input											= NULL;
	char					*relation_form									= NULL;
	tag_t					t_item											= NULLTAG;
	tag_t					revision										= NULLTAG;
	tag_t					*status_list									= NULLTAG;
	tag_t					release_status									= NULLTAG;
	tag_t					revstatus										= NULLTAG;
	int						x												= 0;

	date_t				    test											= NULLDATE;
	date_t					test1										    = NULLDATE;
	char					*o											    = NULL;
	char					result1 [ 10000 ] ;
	char					 result2 [ 10000 ] ;
	char					*f												=NULL;

	char					*test11											=NULL;
	char					*test12											=NULL;
	char					*test13											=NULL;
	char					*JTid											=NULL;
	char					*JTid1											=NULL;
	char					*itemId12											=NULL;
	char					*Creator										=NULL;
	char					*aDatasetId										=NULL;
	char					*aDatasetRev										=NULL;
	char * parent_name ;
	char * dataset_name ;
	char * parent_revision_id ;
	int parent_sequence_id ;
	int ItmSeqID_int ;

	tag_t 	aDataset =NULLTAG;


	int ii,itemcount,j=0;
	int sPartNumberTokint=0;
	int c,t=0;
	tag_t item=NULLTAG;
	tag_t *rev=NULLTAG;
	tag_t revTag=NULLTAG;
	char *format=NULL;
	char *StrWeightArrayMain1=NULL;
	tag_t relation_type, relation;

	tag_t  reln_type = NULLTAG;
	int    n_attchs =0;
	int    Flag_1 =0;
	GRM_relation_t *rellist;
	tag_t  *secondary_objects,primary,objTypeTag,refobject=NULLTAG;
	tag_t Fndrelation = NULLTAG;
	int nameRef_cnt;
	tag_t *namRefObject = NULL;
    int  * nFound;
    int   nlFound;
    int   ds;
    int   ass = 0;
    tag_t **  datasets;
	int ref_count_d=0;


	int n_tags_found = 0;
	tag_t query = NULLTAG;
	tag_t	LatestRev		=	NULLTAG;
	tag_t *cntr_objects = NULL;
	tag_t	*t_item1		=	NULLTAG;
	tag_t	latestrev	=	NULLTAG;
	int n_entries = 1;
	int n_found = 1;
	int dd = 0;
	int org_seq_id_int=0;
	char *inputfile=NULL;
	tag_t *tags_found = NULL;
	char* inputline=NULL;
	tag_t	queryTag	= NULLTAG;
	int resultCount=0;
	tag_t item_tag = NULLTAG;
	char
		*qry_entries4[2] = {"Revision","ID"},
        *qry_values4[2]	= {"*","*"};
	char *itemRevSeq = NULL;

	FILE* fp=NULL;

	//char *type_name;
	char  type_name[TCTYPE_name_size_c+1];
    char object_name[TCTYPE_name_size_c+1] ;




	char *DataSetNm2 = NULL;
	char *DataSetNm = NULL;
	const char *qry_entries[1] = {"item_id"};
	const char *qry_values123[1];
	char *rev_id123 = NULL;
	char *checkout = NULL;
	char *last_mod_date = NULL;
	char *currentRevID = NULL;
	char *IsDataset = NULL;
	char *currentRevID2 = NULL;
	char *currentRevID3 = NULL;
	char *LatestRevID = NULL;
	char *object_type = NULL;
	char *owning_user1 = NULL;
	char *checked_out_user1 = NULL;
	char *newSeqID = NULL;
	char *newRevID = NULL;
	char *newRevIDRec = NULL;
	char *revIDCheck = NULL;
	char *revIDCheck1 = NULL;
	char *revIDCheck2 = NULL;

	//RPS
	char *strPropertyName = NULL;
	char *strPropertyName1 = NULL;
	char *strPropertyValue = NULL;
	char *strPropertyValue1 = NULL;
	char *strPropertyValue2 = NULL;
	char *strAttrbuteName = NULL;
	char *strFoundAttrbuteName = NULL;
	char *strSearchValue = NULL;
	int straAttrName = 0;
	int  iFoundID = 0;

	int iFlagAttrFound = 0;

	int n_count_item=0;
	int newSeqID1=0;
	int n_rev_cnt=0;
	tag_t *all_rev;
	tag_t *secObj;
	tag_t Relatn_type;
	int revloop=0;
	int revIDInt=0;
	int revIDInt1=0;
	int Myid=0;
	int secObjCount=0;
	FILE *fptr;
	FILE *NewAttrfptr;
	char* line;
	int k=0;
	int NewId=0;
	int NewlyId=0;
	int NewStrLength=0;
	int NewFormat=0;
	tag_t newRev= NULLTAG;
	const char *LatestRevArray[18]={"NR", "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q"};
	const char *LatestRevArray1[9]={"1","2","3","4","5","6","7","8","9"};
	int RevSeq=1;
	int revCount=0;
	int Latest_count=0;
	int Latest_count123=0;
	int Lastest_corct_count123=0;
	int iAttributeCount = 0;
	int iAttachAtt = 1;
	
	int iSearchCount = 0;
	int *iIDCount=NULL;
	char **strAttrName;
	tag_t *rev_list=NULLTAG;
	//int i=0;
	//int j=0;
	//int k=0;


	tag_t dml_rev_create_input_tag	= NULLTAG;                              
	tag_t dml_type_tag				= NULLTAG;
	tag_t object_create_input_tag		= NULLTAG;
	tag_t new_object						= NULLTAG;
	tag_t rev_type_tag				= NULLTAG;


	inputfile = ITK_ask_cli_argument("-i=");
	

   // ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
//	printf("\n Auto login ");fflush(stdout);
	//ITK_CALL(ITK_auto_login( ));
	              if( ITK_init_module("uaprod" ,"adminUA2020","dba")!=ITK_ok) ;
				   ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
					printf("\n Auto login ");fflush(stdout);
					 ITK_CALL(ITK_auto_login( ));


    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Line  ====%s ",inputfile );fflush(stdout);
	//itemId=tc_strtok(inputfile,"^");
	
	fptr=fopen("partlist.txt","r");
	NewAttrfptr=fopen("NewAttribute.txt","w");
	if(NewAttrfptr!=NULL)
		;
	else
		printf("\n Unable to open/create file NewAttribute.txt");fflush(stdout);


	
	printf("\n Auto login .......");fflush(stdout);
	//printf("\n inputfile id %s ",inputfile);fflush(stdout);
	//printf("\n inputfile id %s ",itemId);fflush(stdout);
	
	//iIDCount=(int *) MEM_alloc(100 * sizeof(int *));
	/*for(i=0;i<100;i++)
	{
		iIDCount[i]=(int *) MEM_alloc(240);
		//char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	}*/

	/*strAttrName= (char **) MEM_alloc(100 * sizeof(char *));
	for(i=0;i<100;i++)
	{		
		strAttrName[i]=(char *) MEM_alloc(100);		
		//char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	}*/

	/*for(i=0;i<100;i++)
	{
		for(j=0;j<100;j++)
		{
			for(k=0;k<100;k++)
				strAttrName[i][j][k]=(char *) MEM_alloc(240);
		}
		//char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	}*/
	//temp1=(char **) MEM_alloc(1000);

	//strPropertyName1 = (char *) MEM_alloc(1000);
	strPropertyName = (char *) MEM_alloc(100);
	strPropertyName1 =(char *) MEM_alloc(100);
	strFoundAttrbuteName = (char *) MEM_alloc(100);
	strAttrbuteName = (char *) MEM_alloc(100);
	strSearchValue = (char *) MEM_alloc(100);
	if(fptr!=NULL)
	{
		inputline=(char *) MEM_alloc(1000);
		while(fgets(inputline,1000,fptr)!=NULL)
		{
			iFlagAttrFound = 0;
			iAttachAtt = 1;
			strcpy(strFoundAttrbuteName,"");
			iFoundID = 0;
			fputs(inputline,stdout);
			//itemId=tc_strtok(inputline,"^");
			//printf("\n item id %s ######## \n",itemId);fflush(stdout);
			//Test1^20208^UNIT1^mm^
			newRevIDRec=tc_strtok(inputline,"^");
			newRevID=tc_strtok(NULL,"^");
			strPropertyName=tc_strtok(NULL,"^");
			strPropertyValue=tc_strtok(NULL,"^");
			//strAttrbuteName=tc_strtok(NULL,"^");

			printf("\n Attribute ID from file  @@@@@@@@@@ (%s) \n",newRevIDRec);fflush(stdout);
			printf("\n NAME of attribute  *************  (%s) \n",newRevID);fflush(stdout);
			printf("\n Format of attribute  *************  (%s) \n",strPropertyName);fflush(stdout);
			printf("\nUnit of attribute  *************  (%s) \n",strPropertyValue);fflush(stdout);

			NewStrLength=atoi(newRevIDRec); 
			printf("\n NewStrLength Attribute type *************  (%d) \n",NewStrLength);fflush(stdout);

			NewFormat=atoi(strPropertyName); 
			printf("\n NewStrLength Attribute type *************  (%d) \n",NewFormat);fflush(stdout);
		
			
			
			
			//strcpy(strAttrbuteName,newRevIDRec);
			//sprintf(strSearchValue,"-401 = %s", strAttrbuteName);


//			printf("\n NewId  @@@@@@@@@@ (%d) \n",NewId);fflush(stdout);
//			printf("\n strSearchValue Search Value  @@@@@@@@@@ (%s) \n",strSearchValue);fflush(stdout);
//			printf("\n NewStrLength Attribute type *************  (%d) \n",NewStrLength);fflush(stdout);
//			printf("\n strPropertyName *************  (%s) \n",strPropertyName);fflush(stdout);
//			printf("\n strPropertyValue *************  (%s) \n",strPropertyValue);fflush(stdout);
//			printf("\n strAttrbuteName *************  (%s) \n",strAttrbuteName);fflush(stdout);
//			
			//ITK_CALL( ICS_attribute_search(strSearchValue,0,0,&iSearchCount,&status_list,&iIDCount,&strAttrName));
		//	printf("\n No of attribute found ==>(%d) \n",iSearchCount);fflush(stdout);
			if(strcmp(argv[1],"1")==0)
			{
				iSearchCount = 0;
				printf("\n !!! Not searching for existing, creating new Atttribute !!! \n");fflush(stdout);
			}
//			for(i=0;i<iSearchCount;i++)
//			{
//				printf("\n Attribute ID ====>(%d) \n",iIDCount[i]);fflush(stdout);
//				printf("\n Attribute Value ====>(%s) \n",strAttrName[i]);fflush(stdout);
//				ITK_CALL( ICS_attribute_ask_property(iIDCount[i],strPropertyName,&strPropertyValue1));
//				printf("\n Attribute Unit ====>(%s) \n",strPropertyValue1);fflush(stdout);
//				strcpy(strPropertyName1,"FORMAT1");	
//				ITK_CALL( ICS_attribute_ask_property(iIDCount[i],strPropertyName1,&strPropertyValue2));
//				printf("\n Attribute FORMAT1 ====>(%s) \n",strPropertyValue2);fflush(stdout);
//					
//				
//			}
			
			if(iFlagAttrFound==0)
			{			
				//ITK_CALL(ICS_attribute_create_unique(strAttrbuteName,NewStrLength,&NewlyId));
				ITK_CALL(ICS_attribute_create(NewStrLength,newRevID,NewFormat));
				
				printf("\n ##### Id of newly created attribute  %d \n",NewStrLength );fflush(stdout);

				 if (tc_strcmp(strPropertyValue,"NA")!=0)
				{
						printf("\n set unit to attribute created  %s \n",strPropertyValue );fflush(stdout);
						ITK_CALL( ICS_attribute_set_property(NewStrLength,"UNIT1",strPropertyValue));
			
						straAttrName = NewStrLength;
				}
				else
				{
					printf("\n Else no NEED to set unit to attribute created  %s \n",strPropertyValue );fflush(stdout);
					straAttrName = NewStrLength;
				}
				
			
			}
		

			if(argv[2]!=NULL)
			{
				printf("\n 444444444444444444444  111111111111111 classe %s  \n",argv[2] );fflush(stdout);
				ITK_CALL(ICS_class_ask_attributes(argv[2],&iAttributeCount,&status_list,&iIDCount));	
				printf("\n ##### No of attributes attached to classe %s is %d \n",argv[2],iAttributeCount );fflush(stdout);
				
				if(iAttachAtt == 1)
				{
					ITK_CALL(ICS_class_add_attributes(argv[2],1,&straAttrName));
				}
			}
					


		}

	}
	printf("\n 12121212121212  \n");fflush(stdout);
	if(inputline!=NULL)
		MEM_free(inputline);
	printf("\n aaaaaaaaaaa   \n");fflush(stdout);

	printf("\n bbbbbbbbbbb  \n");fflush(stdout);
	if(strPropertyName1!=NULL)
		MEM_free(strPropertyName1);
	printf("\n cccccccccc  \n");fflush(stdout);
	if(strFoundAttrbuteName!=NULL)
		MEM_free(strFoundAttrbuteName);
printf("\n ddddddddddddd  \n");fflush(stdout);
	
	ITK_CALL(POM_logout(false));
	printf("\n 88888888888888  \n");fflush(stdout);
	return status;
}
