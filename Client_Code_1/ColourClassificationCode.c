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

	int						result											= 0;
	int						count											= 0;
	int						iCounter1										= 0;
	int						i												= 0;
	char					*value											= NULL;
	char					*argstring1										= NULL;
	char					*argstring2										= NULL;
	char					*argstring3										= NULL;

	char					*user										= NULL;
	char					*pass										= NULL;
	char					*group										= NULL;
	char					*RefNo										= NULL;

	char					*VendorID									= NULL;
	char					*VendorName									= NULL;
	char					*VendorAddr									= NULL;
	char					*VendorPhone								= NULL;
	char					*itemId											= NULL;
	char					*SubSyscdStr									= NULL;
	char					*revisionID										= NULL;
	char					*ItmSeqID										= NULL;
	char					 JTFile[256];
	char					*JTFileDesc										= NULL;
	char					*JTFileSeq										= NULL;
	char					*CreatedDate									= NULL;
	char					*ModifyDate										= NULL;
	char					*fullPath										= NULL;
	char					*VaultLocation									= NULL;
	char					*HostName										= NULL;
	char					*Owner											= NULL;
	char					*ProjectCode									= NULL;
	char					*vault											= NULL;
	char					*InDatabase										= NULL;
	char					*Randomized										= NULL;
	char					*Superseded										= NULL;
	char					*Frozen											= NULL;
	char					*LifeCycleState									= NULL;
	char					*FileStatus										= NULL;
	char					*Externallymanaged								= NULL;
	char					*FileRegisteredBy								= NULL;
	char					*ShadowCopy										= NULL;
	char					*Categoryname									= NULL;
	char					*Sitename										= NULL;
	char					*WorkingFileName								= NULL;
	char					*WorkingpathFormat								= NULL;
	char					*BulkdataLocation								= NULL;

	char					*Bulkdata										= NULL;
	char					*DataBasename									= NULL;
	char					*PartSequence									= NULL;

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
	int						ref_count;
	tag_t					datasettype										= NULLTAG;
	tag_t					Newdataset										= NULLTAG;
	tag_t					tool											= NULLTAG;
	tag_t					filetag											= NULLTAG;
	char					**ref_list;
	char					pathbuff[1000];
    char					*creation_date									= NULL;
	char					*text											= NULL;
	tag_t					datasetAvl										= NULLTAG;
	IMF_file_t				fileDescriptor;
	AE_reference_type_t		tagRefType										= 1;
	int						iCountSecondary									= 0;
	tag_t					*secondaryObjects								= NULL;

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


	int ii,itemcount,j=0,p=0;
	int sPartNumberTokint=0;
	int c,t=0;
	tag_t item=NULLTAG;
	tag_t *rev=NULLTAG;
	tag_t revTag=NULLTAG;
	char *format=NULL;
	char *StrWeightArrayMain1=NULL;
	tag_t relation_type, relation;

	char **attrs = (char **) MEM_alloc(2 * sizeof(char *));
    char **values = (char **) MEM_alloc(2 * sizeof(char *));

	char **qry_values = (char **) MEM_alloc(10 * sizeof(char *));

	char ***qry_entries1 = (char ***) MEM_alloc(10 * sizeof(char *));
	char ***qry_values1 = (char ***) MEM_alloc(10 * sizeof(char *));

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

	char *drawingnumber;
	FILE* fp=NULL;
	char *drawingtype;
	char *relationstr;
	char *user_data_1;
	//char *type_name;
	char  type_name[TCTYPE_name_size_c+1];
    char object_name[TCTYPE_name_size_c+1] ;
	tag_t *namRefObject_d = NULL;

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
	char *UserInfo1Str = NULL;
	char *UserInfo2Str = NULL;
	char *UserInfo3Str = NULL;
	char *UserInfo4Str = NULL;
	char *RecordTypeStr = NULL;
	char *Obj_name = NULL;
	char *revIDCheck = NULL;
	char *revIDCheck1 = NULL;
	char *revIDCheck2 = NULL;

	int n_count_item=0;
	int newSeqID1=0;
	int n_rev_cnt=0;
	tag_t *all_rev;
	tag_t *secObj;
	tag_t Relatn_type;
	int revloop=0;
	int revIDInt=0;
	int revIDInt1=0;
	int secObjCount=0;
	FILE *fptr;
	char* line;
	char					*VendorEmail								= NULL;
	char					*VendorWeb								= NULL;
	char					*VendorContactName								= NULL;
	
	int k=0;
	tag_t newRev= NULLTAG;
	const char *LatestRevArray[18]={"NR", "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q"};
	const char *LatestRevArray1[9]={"1","2","3","4","5","6","7","8","9"};
	int RevSeq=1;
	int revCount=0;
	int Latest_count=0;
	int Latest_count123=0;
	int Lastest_corct_count123=0;
	tag_t *rev_list=NULLTAG;
	tag_t dml_rev_create_input_tag	= NULLTAG;                              
	tag_t dml_type_tag				= NULLTAG;
	tag_t object_create_input_tag		= NULLTAG;
	tag_t new_object						= NULLTAG;
	tag_t rev_type_tag				= NULLTAG;
	char* ModuleSyscdStr = NULL;

	int  	theCount =0;
		int * 	theIds;
		char ** 	theNames;
		char ** 	theShortNames;
		char ** 	theAnnotations;
		int * 	theArraySize;
		int * 	theFormat;
		char ** 	theUnit;
		char ** 	theMinValues;
		char ** 	theMaxValues;
		char ** 	theDefaultValues;
		char ** 	theDescriptions;
		int * 	theOptions1 ;

		int *InputIdsArr1;
		char **InputValuesArr1=NULL;
		char **StrConcatenatedMaster=NULL;
		char * 	StrConcatenatedinput=NULL;

		tag_t   	view;
		tag_t   	theClassTag	; 
		int   	nInstances99=0;
		tag_t *  	instances99;
		char * 	atrValue=NULL;
		char * 	atrValueReturn=NULL;

	//int InputIdsArr2[]={1002,1005,1003,1004};
	char *InputValues[]={"GLOVE_BOX_LATCH/C7","CCA_DASHBOARD_SKIN/ZY","INSTRUMENT_PANEL/C9"};
	int  	NumberOfInput =3;
	int  	chkflag =0;

	//inputfile = ITK_ask_cli_argument("-i=");
	
	//ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	//printf("\n Auto login ");fflush(stdout);
	ITK_CALL(ITK_auto_login());	
	//if( ITK_init_module("loader","loader7","dba")!=ITK_ok) ;
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
    ITK_CALL(ITK_set_journalling( TRUE ));



	//CALLAPI(ICS_class_describe("SAM0101",&theParent,&theType,&theName,&theShortName,&theDescription,&theUserData1,&theUserData2,&theOptions,&theInstanceCount,&theChildrenCount,&theViewCount));

	ITK_CALL(ICS_class_describe_attributes("ICM0201",
	&theCount,
	&theIds,
	&theNames,
	&theShortNames,
	&theAnnotations,
	&theArraySize,
	&theFormat,
	&theUnit,
	&theMinValues,
	&theMaxValues,
	&theDefaultValues,
	&theDescriptions,
	&theOptions1)	)

printf("\n After ICS_class_describe_attributes..\n");fflush(stdout);

	InputIdsArr1=(int*) malloc(theCount * sizeof(int) );
	InputValuesArr1=(char**) malloc(theCount * sizeof(char*) );	
	StrConcatenatedinput= malloc(1500 * sizeof(char) );
	for(i=0;i<theCount;i++)
	{
		InputValuesArr1[i]= malloc(99 * sizeof(char) );;
	}
		for(i=0;i<theCount;i++)
	{
		if (i==0)
		{
			strcpy(InputValuesArr1[i],"CCA_DASHBOARD_PANEL");
		}
		else
		{
			strcpy(InputValuesArr1[i],"");
		}
	}


ITK_CALL(  ICS_view_ask_tag ( "defaultView",&view ) );
ITK_CALL(ICS_class_ask_tag 	("ICM0201",&theClassTag));
printf("\n after  ICS_class_ask_tag.......");

//for(i=0;i<2;i++)
//	{
//		for(j=0;j<theCount;j++)
//		{
//			strcpy(InputValuesArr1[j],InputValues[i]);
//		}
//
//	}


for(j=0;j<theCount;j++)
		{
			printf("\n ID : %d",theIds[j]);fflush(stdout);
		}
	ITK_CALL(ICS_search_instances (theClassTag,
		view,
		theCount,
		theIds,
		InputValuesArr1,
		&nInstances99,
		&instances99	 
	));

printf("\n after  nInstances99 by ICS_search_instances .......%d\n",nInstances99);

if(nInstances99>0)
	{
		StrConcatenatedMaster=(char**) malloc(nInstances99 * sizeof(char*) );
	}

for(k=0;k<nInstances99;k++)
	{ 
		chkflag=0;
		StrConcatenatedMaster[k]= malloc(1500 * sizeof(char) );		
		strcpy(StrConcatenatedMaster[k],"");
		strcpy(StrConcatenatedinput,"");
		for (j=0;j<theCount ;j++ )
		{
			atrValue=NULL;
			ICS_ask_attribute_value	((instances99[k]),theNames[j],&atrValue );
			printf("\ncount %d  \t AttrName :%s \t atrValue : %s\n",j,theNames[j],atrValue);fflush(stdout);
					
			if (j>1)
			{
				strcat(StrConcatenatedMaster[k],atrValue);
			}		
		}

		printf("\nStrConcatenatedMaster :%s\n",StrConcatenatedMaster[k]);

		for(p=0;p<NumberOfInput;p++)
		{
			strcat(StrConcatenatedinput,InputValues[p]);
			if(tc_strstr(StrConcatenatedMaster[k],InputValues[p])==NULL)
			{
				chkflag=1;
			}					
			
		}
		if (chkflag==0 && (tc_strlen(StrConcatenatedMaster[k])==tc_strlen(StrConcatenatedinput)))
		{
			atrValueReturn=NULL;
			ICS_ask_attribute_value	((instances99[k]),theNames[1],&atrValueReturn );
			printf("\nFinal Output colour code is :%s\n",atrValueReturn);
			break;
		}




	}



	ITK_CALL(POM_logout(false));
	return status;
}

