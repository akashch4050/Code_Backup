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

	char					*CompCdMaster									= NULL;
	char					*t5InternalSchm									= NULL;
	char					*t5CcdPlatform									= NULL;
	char					*t5VehClassNVTable								= NULL;
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


	int ii,itemcount,j=0;
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

	inputfile = ITK_ask_cli_argument("-i=");
	
	//ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	//printf("\n Auto login ");fflush(stdout);
	//ITK_CALL(ITK_auto_login( ));
	
	//if( ITK_init_module("loader","loader7","dba")!=ITK_ok) ;
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	
	ITK_CALL(ITK_auto_login( ));
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Line  ====%s ",inputfile );fflush(stdout);
	//itemId=tc_strtok(inputfile,"^");
	
	fptr=fopen(inputfile,"r");

	if(fptr!=NULL)
	{
		inputline=(char *) MEM_alloc(1000);
		while(fgets(inputline,1000,fptr)!=NULL)
		{
			fputs(inputline,stdout);			
			//CCA_DECORATIVE_PIECE,T,Exterior+Interior,BUS-CAR-HCV-LCV-LMV-MCV-MUV-PRD-UV_VAN

			CompCdMaster=tc_strtok(inputline,",");
			printf("\n CompCdMaster => %s ",CompCdMaster );fflush(stdout);

			t5InternalSchm=tc_strtok(NULL,",");
			printf("\n t5InternalSchm => %s ",t5InternalSchm );fflush(stdout);

			t5CcdPlatform=tc_strtok(NULL,",");
			printf("\n t5CcdPlatform => %s ",t5CcdPlatform );fflush(stdout);

			t5VehClassNVTable=tc_strtok(NULL,",");	
			printf("\n t5VehClassNVTable => %s \n\n",t5VehClassNVTable );fflush(stdout);
						
			printf("\n Start Comp Code Master creation process..........\n");
			ITK_CALL(TCTYPE_find_type("T5_CompCdMst", NULL, &dml_type_tag));  
			ITK_CALL(TCTYPE_construct_create_input(dml_type_tag, &object_create_input_tag));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"object_name",CompCdMaster));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"t5_InternalSchme",t5InternalSchm));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"t5_CcodePlatform",t5CcdPlatform));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"t5_VehClassNVTable",t5VehClassNVTable));
			ITK_CALL(TCTYPE_create_object(object_create_input_tag, &new_object));
			if(new_object)
			{
				printf("\n t5CreateObject : object created.\n");
				ITK_CALL(AOM_save(new_object));
				ITK_CALL(AOM_unlock(new_object));
			}
			
			/*
			ITK_CALL( FL_user_update_newstuff_folder(newItemTag) );
			ITK_CALL(GRM_find_relation_type  ("T5_DMLTaskRelation",&tRelationFind));
			if (tRelationFind!=NULLTAG)
			{
				//Now Creating Relation Between Requirement and DVM Object
				GRM_find_relation(dmlrevisiontag,task_tag,tRelationFind,&tRelationExist);  
				if (tRelationExist==NULLTAG)
				{
					//###  Creating Relation between DML and Task  ###
					printf("\n Now Creating Relation Between DML and DML Task Object \n");fflush(stdout);
					ITK_CALL(GRM_create_relation(dmlrevisiontag,task_tag,tRelationFind,NULLTAG,&Rel_task));
					ITK_CALL(GRM_save_relation  (Rel_task)); 
					printf("\nRelation Created Successfully\n");fflush(stdout);		
					
				}
			}
			*/
			

		}
	}

	ITK_CALL(POM_logout(false));
	return status;
}

