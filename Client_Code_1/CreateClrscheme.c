//********* Main Code *******

/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  Author		 :   Ashish Walunj
*  Module		 :   Loading of color schme from TCE to TCUA
*  Code			 :   createClrscheme.c
*  Created on	 :   Oct 30th, 2019
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

char *replaceWord(const char *s, const char *oldW, const char *newW) 
{ 
    char *result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
  
    // Counting the number of times old word 
    // occur in the string 
    for (i = 0; s[i] != '\0'; i++) 
    { 
        if (strstr(&s[i], oldW) == &s[i]) 
        { 
            cnt++; 
  
            // Jumping to index after the old word. 
            i += oldWlen - 1; 
        } 
    } 
  
    // Making new string of enough length 
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) 
    { 
        // compare the substring with the result 
        if (strstr(s, oldW) == s) 
        { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
    return result; 
} 
static char* default_empty_to_A(char *s)
{
    return (NULL == s ? s : ('\0' == s[0] ? "A" : s));
}

extern int ITK_user_main (int argc, char ** argv )
{

    int status;
	logical			retain							= 0;
	tag_t			status_rel					= NULLTAG;
	tag_t			clrrevtagpre					= NULLTAG;

	char* dupintrnalscheme=NULL;
	dupintrnalscheme=(char *)MEM_alloc(100);

	char* clrsrlmain1=NULL;
	clrsrlmain1=(char *)MEM_alloc(100);

	int						result											= 0;
	int						o											= 0;
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

	char					*itemId											= NULL;
	char					*itemId1											= NULL;
	char					*clrrev									= NULL;
	char					*clrrev1									= NULL;
	clrrev1=(char *)MEM_alloc(1000);
	
	char					*clrrevmain									= NULL;
	char					*rev_name									= NULL;
	char					*clr_id									= NULL;
	char					*clr_iddash									= NULL;
	clrrevmain=(char *)MEM_alloc(1000);
	char					*clrrevmain1									= NULL;
	clrrevmain1=(char *)MEM_alloc(1000);
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
	char					result1 [ 10000 ] ;
	char					 result2 [ 10000 ] ;
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
	int  num_to_sort = 1;
	char *keys[1] = {"creation_date"};
	int  	 orders[1]  ={2};
	tag_t 	aDataset =NULLTAG;
	int ii,itemcount,j=0;
	int sPartNumberTokint=0;
	int t=0;
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
	int n_entries = 3;
	int n_entries5 = 1;
	int n_entries1 = 2;
	int n_entries6 = 3;
	tag_t *qry_output= NULLTAG;
	tag_t *qry_output5= NULLTAG;
	tag_t *qry_output6= NULLTAG;
	tag_t *qry_output1= NULLTAG;
	int n_tags_found = 0;
	tag_t query = NULLTAG;
	tag_t	LatestRev		=	NULLTAG;
	tag_t *cntr_objects = NULL;
	tag_t	*t_item1		=	NULLTAG;
	tag_t	latestrev	=	NULLTAG;
	int n_found = 1;
	int dd = 0;
	int org_seq_id_int=0;
	char *inputfile=NULL;
	inputfile=(char *)MEM_alloc(1000);
	char *inputfile1=NULL;
	inputfile1=(char *)MEM_alloc(1000);
	char *inputfile11=NULL;
	inputfile11=(char *)MEM_alloc(1000);
	char *inputfile2=NULL;
	inputfile2=(char *)MEM_alloc(1000);
	char *inputfile22=NULL;
	inputfile22=(char *)MEM_alloc(1000);
	tag_t *tags_found = NULL;
	char* inputline=NULL;
	char* inputline1=NULL;
	char* inputline2=NULL;
	char* vaiantname=NULL;
	tag_t	queryTag	= NULLTAG;
	tag_t	queryTag1	= NULLTAG;
	tag_t	queryTag2	= NULLTAG;
	tag_t	queryTag5	= NULLTAG;
	int resultCount=0;
	int resultCount5=0;
	int resultCount6=0;
	int resultCount1=0;
	int count_data=0;
	tag_t *clrdata_secondary_objects = NULLTAG;
	tag_t relation_tag = NULLTAG;
	int s=0;
	tag_t item_tag = NULLTAG;
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
	char *coated = NULL;
	char *qry_entries5[1] = {"Name"};
	char **qry_values5 = (char **) MEM_alloc(50 * sizeof(char *));
	char *colseriel = NULL;
	char *vehclass = NULL;
	char *platform = NULL;
	char *category = NULL;
	char *UserInfo6Str = NULL;
	char *seq = NULL;
	char *Desc = NULL;
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

	char* t5_ClSrlDup = NULL;
	char* t5_ClSrlDup1 = NULL;
	char* t5_ClSrlDup2 = NULL;

	int revIDInt=0;
	int revIDInt1=0;
	int secObjCount=0;
	FILE *fptr;
	FILE *fptr1;
	FILE *fptr2;
	char* line;
	int k=0;
	tag_t newRev= NULLTAG;
	tag_t clrscmtag= NULLTAG;
	tag_t clrscmltestrev= NULLTAG;
	int revCount=0;
	int Latest_count=0;
	int Latest_count123=0;
	int Lastest_corct_count123=0;
	tag_t *rev_list=NULLTAG;
	tag_t dml_rev_create_input_tag	= NULLTAG;                              
	tag_t clrdata_type_tag				= NULLTAG;
	tag_t Colordata_type_tag			= NULLTAG;
	tag_t object_create_input_tag		= NULLTAG;
	tag_t object_create_input_tag1		= NULLTAG;
	tag_t new_object						= NULLTAG;
	tag_t clrschmtag						= NULLTAG;
	tag_t new_COLDATA_object  =NULLTAG;
	tag_t rev_type_tag				= NULLTAG;
	char* clrschmname = NULL;
	char* clrschmnameDUP = NULL;
	char* clrschmnameDUP1 = NULL;
	char* clrschmnameDUP1main = NULL;
	char *name;
	tag_t clrrevtag,clrdatarevtag,clrdatarevtag1,clroldrevtag;
	char *current_revision_id;

	char					*srno									= NULL;
	char					*applicablevc									= NULL;
	char					*applicablevcstar									= NULL;
	char					*Data_category									= NULL;
	char					*compcode									= NULL;
	char					*Intrnalschme									= NULL;
	char					*t5_ClSrl									= NULL;
	char					*blank									= NULL;

	char c[] = ","; 
	char d[] = ";";
	char e[] = "/"; 
	char f[] = "-";
	tag_t ColData_relation_type = NULLTAG;
	tag_t applicablesvr_relation_type = NULLTAG;
	tag_t new_relation = NULLTAG;
	tag_t new_applicablesvr_relation = NULLTAG;
	char *clrsrlmain=NULL;
	char *item_id_ColDatamain=NULL;
	char *item_id_ColDatamain1=NULL;
	item_id_ColDatamain1=(char *)MEM_alloc(100);
	char* item_id_ColData=NULL;
	char *revId = "A";
	item_id_ColData=(char *)MEM_alloc(100);
	char *Date = NULL;
	char *UADate = NULL;
	UADate=(char *)MEM_alloc(100);
	char *month = NULL;
	char *realmonth = NULL;
	realmonth=(char *)MEM_alloc(100);
	char *Day = NULL;
	char *Year = NULL;
	char *MainYear = NULL;
	MainYear=(char *)MEM_alloc(1000);
	char *hour = NULL;
	char *min = NULL;
	date_t my_date= NULLDATE;

	inputfile = ITK_ask_cli_argument("-i=");
	char *usr = ITK_ask_cli_argument("-u=");
	char *pwd = ITK_ask_cli_argument("-pf=");
	char *grp = ITK_ask_cli_argument("-g=");

				ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ) ;
				//ITK_init_module(usr,pwd,grp);
				ITK_CALL(ITK_auto_login());
				ITK_set_bypass(true);


    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Line  ====%s \n",inputfile );fflush(stdout);
	itemId=tc_strtok(inputfile,"^");
	
	fptr=fopen(inputfile,"r");


	
	printf("\n Auto login .......");fflush(stdout);
	printf("\n inputfile id %s ",inputfile);fflush(stdout);
	printf("\n inputfile id %s ",itemId);fflush(stdout);


	if(fptr!=NULL)
	{
		inputline=(char *) MEM_alloc(1000);
		while(fgets(inputline,1000,fptr)!=NULL)
		{

		fputs(inputline,stdout);
		
		clrschmname=tc_strtok(inputline,"^");
		clrrev=tc_strtok(NULL,"^");
		seq=tc_strtok(NULL,"^");
		Desc=tc_strtok(NULL,"^");
		coated=tc_strtok(NULL,"^");
		colseriel=tc_strtok(NULL,"^");
		vehclass=tc_strtok(NULL,"^");
		platform=tc_strtok(NULL,"^");
		category=tc_strtok(NULL,"^");
		Date=tc_strtok(NULL,"^");

		clrrevmain1 = tc_strcpy(clrrevmain,"");
		clrrevmain1 = tc_strcat(clrrevmain,clrrev);		
		clrrevmain1 = tc_strcat(clrrevmain,";");		
		clrrevmain1 = tc_strcat(clrrevmain,seq);
		
//*************************************** CLRSCM-MUV/10/487^A^1^ ARIA DOMESTIC STYLE 4X4^N^GM^MUV^INDICRUZ^Exterior+Interior **************************************************

					ITK_CALL(QRY_find("Item...", &queryTag1));
					if (queryTag1)
						{
							printf("Found Query 'Item...' \n\t"); fflush(stdout);
						}
						else
						{
							printf("Not Found Query 'Item...' \n\t"); fflush(stdout);
							return status;
						}

					char *qry_entries1[2] = {"Item ID","Type"};
					char **qry_values1 = (char **) MEM_alloc(50 * sizeof(char *));

					qry_values1[0] = clrschmname ;
					qry_values1[1] = "Colour Scheme" ;

					ITK_CALL(QRY_execute(queryTag1, n_entries1, qry_entries1, qry_values1, &resultCount1, &qry_output1));
								printf("\n Count of Item... Query Values ------------------>  %d \n",resultCount1);
					if (resultCount1 == 0)
{
	printf("\n CLRSCM is Not present so going to create..........\n");
			ITK_CALL(TCTYPE_find_type("T5_clschm", NULL, &clrdata_type_tag));   

			ITK_CALL(TCTYPE_construct_create_input(clrdata_type_tag, &object_create_input_tag));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"object_name",clrschmname));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"item_id",clrschmname));

			ITK_CALL(TCTYPE_create_object(object_create_input_tag, &new_object));

			if(new_object)
			{
				printf ("Date Created ---> %s \n",Date); //	2017/06/22-04:56:21:688
				
				Year=strtok(Date,"/");
				month=strtok(NULL,"/");
				Day=strtok(NULL,"-");
				
				tc_strcpy(MainYear,Day);
				

				hour=strtok(NULL,":");
				min=strtok(NULL,":");

				printf ("month Created ---> %s \n",month);
				printf ("Day Created ---> %s \n",Day);
				printf ("Year Created ---> %s \n",Year);
				printf ("MainDay Created ---> %s \n",MainYear);
				printf ("hour Created ---> %s \n",hour);
				printf ("min Created ---> %s \n",min);

					if (tc_strcmp(month,"01") ==0)
				{
					tc_strcpy(realmonth,"Jan");
					printf ("Real month---> %s \n",realmonth);
				}
				if (tc_strcmp(month,"02") ==0)
				{
					tc_strcpy(realmonth,"Feb");
				}
				if (tc_strcmp(month,"03") ==0)
				{
					tc_strcpy(realmonth,"Mar");
				}
				if (tc_strcmp(month,"04") ==0)
				{
					tc_strcpy(realmonth,"Apr");
				}
				if (tc_strcmp(month,"05") ==0)
				{
					tc_strcpy(realmonth,"May");
				}
				if (tc_strcmp(month,"06") ==0)
				{
					tc_strcpy(realmonth,"Jun");
				}
				if (tc_strcmp(month,"07") ==0)
				{
					tc_strcpy(realmonth,"Jul");
				}
				if (tc_strcmp(month,"08") ==0)
				{
					tc_strcpy(realmonth,"Aug");
				}
				if (tc_strcmp(month,"09") ==0)
				{
					tc_strcpy(realmonth,"Sep");
				}
				if (tc_strcmp(month,"10") ==0)
				{
					tc_strcpy(realmonth,"Oct");
				}
				if (tc_strcmp(month,"11") ==0)
				{
					tc_strcpy(realmonth,"Nov");
				}
				if (tc_strcmp(month,"12") ==0)
				{
					tc_strcpy(realmonth,"Dec");
				}

				// enterprise date ----> 04/02/2018 12:58:44:322
				// UA Date --------->  20-Nov-2019 09:37

				
					tc_strcpy(UADate,"");
					tc_strcat(UADate,MainYear);
					tc_strcat(UADate,"-");
					tc_strcat(UADate,realmonth);
					tc_strcat(UADate,"-");
					tc_strcat(UADate,Year);
					tc_strcat(UADate," ");
					tc_strcat(UADate,hour);
					tc_strcat(UADate,":");
					tc_strcat(UADate,min);	
					

				printf("************** UADate ---> %s ****************\n",UADate);
				ITK_CALL(ITK_string_to_date(UADate,&my_date));
				ITK_CALL(POM_set_creation_date(new_object, my_date));
				ITK_CALL(AOM_save(new_object));
				ITK_CALL(AOM_unlock(new_object));
				printf("\n t5CreateClrschm --------------------------->  object created.");				
			}
				ITK_CALL( ITEM_ask_latest_rev (new_object,&clrrevtag) );
			if(clrrevtag)
			{
				ITK_CALL(AOM_refresh(clrrevtag,1));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_ClSchmDesc",Desc));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_Coated",coated));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_ClSrl",colseriel));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_VehClass",vehclass));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_SchmPlant",platform));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_SchmCompPlatForm",category));
				ITK_CALL(AOM_set_value_string(clrrevtag,"item_revision_id",clrrevmain1));
				ITK_CALL(AOM_save(clrrevtag));
				//ITK_CALL( ITK_set_bypass ( true ) );
				printf("************** UADate REV ---> %s ****************\n",UADate);
				ITK_CALL(POM_set_creation_date(clrrevtag, my_date));
				ITK_CALL (CR_create_release_status("T5_LcsErcRlzd",&status_rel));      
				ITK_CALL(EPM_add_release_status(status_rel,1,&clrrevtag,retain));
				ITK_CALL(AOM_refresh(clrrevtag,0));

//*************** now call file for Colour Data ***************************			
				
				clrschmnameDUP=tc_strtok(clrschmname,"-");				
				clrschmnameDUP1=tc_strtok(NULL,"-");
				clrschmnameDUP1main = replaceWord(clrschmnameDUP1, e, f);

				inputfile11=tc_strcpy(inputfile1,"");         	
				inputfile11=tc_strcat(inputfile1,"COLOR_DATA_OF_");	
				inputfile11=tc_strcat(inputfile1,clrschmnameDUP);	
				inputfile11=tc_strcat(inputfile1,"-");	
				inputfile11=tc_strcat(inputfile1,clrschmnameDUP1main);	
				inputfile11=tc_strcat(inputfile1,"__");	
				inputfile11=tc_strcat(inputfile1,clrrev);	
				inputfile11=tc_strcat(inputfile1,".txt");	

				printf("\n Line  ====%s \n",inputfile11 );fflush(stdout);  // COLOR_DATA_OF_CLRSCM-CAR-10-680.txt_test
				itemId1=tc_strtok(inputfile11,"^");
				
				fptr1=fopen(inputfile11,"r");

				if(fptr1 != NULL)
				{
					inputline1=(char *) MEM_alloc(1000);
					while(fgets(inputline1,1000,fptr1)!=NULL)
					{

					fputs(inputline1,stdout);
					
					srno=tc_strtok(inputline1,"^");
					Data_category=tc_strtok(NULL,"^");
					compcode=tc_strtok(NULL,"^");
					Intrnalschme=tc_strtok(NULL,"^");
					t5_ClSrl=tc_strtok(NULL,"^");
					blank=tc_strtok(NULL,"^");
					
//*************************************** 1^Centre stack items, Airvents, Combi swit^AIR_VENT^Y^B2,JAVA_BLACK **************************************************

					clrsrlmain = replaceWord(t5_ClSrl, c, d);

								t5_ClSrlDup=tc_strtok(t5_ClSrl,",");
								printf("\n t5_ClSrlDup --->%s ",t5_ClSrlDup);
								t5_ClSrlDup1=tc_strtok(NULL,"");
								printf("\n t5_ClSrlDup1 --->%s ",t5_ClSrlDup1);
							
								tc_strcpy(dupintrnalscheme,"");
								tc_strcat(dupintrnalscheme,t5_ClSrlDup1);
								tc_strcat(dupintrnalscheme,";");
								tc_strcat(dupintrnalscheme,t5_ClSrlDup);

					tc_strcpy(item_id_ColData,"");
					tc_strcat(item_id_ColData,compcode);
					tc_strcat(item_id_ColData,"`");
					tc_strcat(item_id_ColData,Intrnalschme);
					tc_strcat(item_id_ColData,"`");
					tc_strcat(item_id_ColData,dupintrnalscheme);

					item_id_ColDatamain = replaceWord(item_id_ColData, c, d);
					printf("\n**************** item_id_ColDatamain: %s  ***************\n",item_id_ColDatamain);fflush(stdout);					

					
					if(QRY_find("ColorSchemeDataRevision", &queryTag));
					if (queryTag)
						{
							printf("Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
						}
						else
						{
							printf("Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
							return status;
						}
					char *qry_entries4[3] = {"Comp Code","Internal Scheme","Color Serial"};
					char **qry_values = (char **) MEM_alloc(50 * sizeof(char *));
					
					printf("compcode --------> %s\n",compcode);
					printf("Intrnalschme --------> %s\n",Intrnalschme);

								tc_strcpy(clrsrlmain1,"");
								tc_strcat(clrsrlmain1,t5_ClSrlDup);
								tc_strcat(clrsrlmain1,"*");
								tc_strcat(clrsrlmain1,t5_ClSrlDup1);
					printf("clrsrlmain1 --------> %s\n",clrsrlmain1);

					qry_values[0] = compcode ;
					qry_values[1] = Intrnalschme ;
					qry_values[2] = clrsrlmain1 ;
					
					ITK_CALL(QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &qry_output));
								printf("\n Count of Query Values ------------------>  %d \n",resultCount);

						if (resultCount == 0)
							{
							ITK_CALL(TCTYPE_find_type("T5_ClrShmData", NULL, &Colordata_type_tag));   

							ITK_CALL(TCTYPE_construct_create_input(Colordata_type_tag, &object_create_input_tag1));

							ITK_CALL(AOM_set_value_string(object_create_input_tag1,"object_name",item_id_ColDatamain));
							ITK_CALL(AOM_set_value_string(object_create_input_tag1,"item_id",item_id_ColDatamain));

							ITK_CALL(TCTYPE_create_object(object_create_input_tag1, &new_COLDATA_object));

							if(new_COLDATA_object)
							{
								printf("\n t5CreateClrschm ---------------------------> new_COLDATA_object created.");
								ITK_CALL(AOM_save(new_COLDATA_object));
								ITK_CALL(AOM_unlock(new_COLDATA_object));
							}

							ITK_CALL( ITEM_ask_latest_rev (new_COLDATA_object,&clrdatarevtag) );
							if(clrdatarevtag)
							{
								ITK_CALL(AOM_refresh(clrdatarevtag,1));
								ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_PrtCatCode",compcode));
								ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_InternalSchm",Intrnalschme));
								ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_ClSrl",clrsrlmain));
								ITK_CALL(AOM_save(clrdatarevtag));
								ITK_CALL(AOM_refresh(clrdatarevtag,0));
							}

								ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
								if(ColData_relation_type != NULLTAG)
									{
										ITK_CALL(GRM_create_relation(clrrevtag,clrdatarevtag,ColData_relation_type,NULLTAG,&new_relation));
										if(new_relation != NULLTAG)
											{
											ITK_CALL(GRM_save_relation(new_relation));
											}
									}
							}								
							else
							{
								ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
								if(ColData_relation_type != NULLTAG)
									{
										//ITK_CALL( ITEM_ask_latest_rev (qry_output[0],&clrdatarevtag1) );
										clrdatarevtag1 = qry_output[0];
										ITK_CALL(GRM_create_relation(clrrevtag,clrdatarevtag1,ColData_relation_type,NULLTAG,&new_relation));
										if(new_relation != NULLTAG)
											{
											ITK_CALL(GRM_save_relation(new_relation));
											}
									}
							}
					}
				}
			}
}
else
{
								printf("\nCLRSCM is present...........\n"); fflush(stdout);
								ITEM_find_item(clrschmname,&new_object);

								ITK_CALL(QRY_find("Item Revision...", &queryTag2));

					char *qry_entries6[3] = {"Item ID","Revision","Type"};
					char **qry_values6 = (char **) MEM_alloc(50 * sizeof(char *));

					printf("clrschmname -----> %s\n",clrschmname);

								tc_strcpy(clrrev1,"");
								tc_strcat(clrrev1,clrrev);
								tc_strcat(clrrev1,"*");
					printf("clrrev1 query input -----> %s\n",clrrev1);

					qry_values6[0] = clrschmname ;
					qry_values6[1] = clrrev1 ;
					qry_values6[2] = "Colour Scheme" ;

					ITK_CALL(QRY_execute(queryTag2, n_entries6, qry_entries6, qry_values6, &resultCount6, &qry_output6));
								printf("\n Count of Item... Query Values ------------------>  %d \n",resultCount6);
					if (resultCount6 != 0)
						{     
									printf("\n Color Scheme Revision is already present in Database \n\t"); fflush(stdout);
									return status;								
						}
					else
	{
						
								clrschmtag = qry_output1[0];
								ITK_CALL( ITEM_ask_latest_rev (clrschmtag,&clrrevtagpre) );
								ITK_CALL( ITEM_copy_rev(clrrevtagpre,NULL,&clrrevtag));
								if(clrrevtag)
			{
				ITK_CALL(AOM_refresh(clrrevtag,1));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_ClSchmDesc",Desc));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_Coated",coated));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_ClSrl",colseriel));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_VehClass",vehclass));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_SchmPlant",platform));
				ITK_CALL(AOM_set_value_string(clrrevtag,"t5_SchmCompPlatForm",category));
				ITK_CALL(AOM_set_value_string(clrrevtag,"item_revision_id",clrrevmain1));
				ITK_CALL(AOM_save(clrrevtag));
			//	ITK_CALL( ITK_set_bypass ( true ) );
				printf ("Date Created ---> %s \n",Date); //	2017/06/22-04:56:21:688
				
				Year=strtok(Date,"/");
				month=strtok(NULL,"/");
				Day=strtok(NULL,"-");
				
				tc_strcpy(MainYear,Day);
				

				hour=strtok(NULL,":");
				min=strtok(NULL,":");

				printf ("month Created ---> %s \n",month);
				printf ("Day Created ---> %s \n",Day);
				printf ("Year Created ---> %s \n",Year);
				printf ("MainDay Created ---> %s \n",MainYear);
				printf ("hour Created ---> %s \n",hour);
				printf ("min Created ---> %s \n",min);

					if (tc_strcmp(month,"01") ==0)
				{
					tc_strcpy(realmonth,"Jan");
					printf ("Real month---> %s \n",realmonth);
				}
				if (tc_strcmp(month,"02") ==0)
				{
					tc_strcpy(realmonth,"Feb");
				}
				if (tc_strcmp(month,"03") ==0)
				{
					tc_strcpy(realmonth,"Mar");
				}
				if (tc_strcmp(month,"04") ==0)
				{
					tc_strcpy(realmonth,"Apr");
				}
				if (tc_strcmp(month,"05") ==0)
				{
					tc_strcpy(realmonth,"May");
				}
				if (tc_strcmp(month,"06") ==0)
				{
					tc_strcpy(realmonth,"Jun");
				}
				if (tc_strcmp(month,"07") ==0)
				{
					tc_strcpy(realmonth,"Jul");
				}
				if (tc_strcmp(month,"08") ==0)
				{
					tc_strcpy(realmonth,"Aug");
				}
				if (tc_strcmp(month,"09") ==0)
				{
					tc_strcpy(realmonth,"Sep");
				}
				if (tc_strcmp(month,"10") ==0)
				{
					tc_strcpy(realmonth,"Oct");
				}
				if (tc_strcmp(month,"11") ==0)
				{
					tc_strcpy(realmonth,"Nov");
				}
				if (tc_strcmp(month,"12") ==0)
				{
					tc_strcpy(realmonth,"Dec");
				}

				// enterprise date ----> 04/02/2018 12:58:44:322
				// UA Date --------->  20-Nov-2019 09:37

				
					tc_strcpy(UADate,"");
					tc_strcat(UADate,MainYear);
					tc_strcat(UADate,"-");
					tc_strcat(UADate,realmonth);
					tc_strcat(UADate,"-");
					tc_strcat(UADate,Year);
					tc_strcat(UADate," ");
					tc_strcat(UADate,hour);
					tc_strcat(UADate,":");
					tc_strcat(UADate,min);	
					

				printf("************** UADate ---> %s ****************\n",UADate);
				ITK_CALL(ITK_string_to_date(UADate,&my_date));
				ITK_CALL(POM_set_creation_date(clrrevtag, my_date));
				ITK_CALL (CR_create_release_status("T5_LcsErcRlzd",&status_rel));      
				ITK_CALL(EPM_add_release_status(status_rel,1,&clrrevtag,retain));
				ITK_CALL(AOM_refresh(clrrevtag,0));

				ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
								if(ColData_relation_type != NULLTAG)
									{
										ITK_CALL(GRM_list_secondary_objects_only(clrrevtag,ColData_relation_type,&count_data,&clrdata_secondary_objects));
										for(s=0;s<count_data;s++)
							{
										ITK_CALL(GRM_find_relation(clrrevtag, clrdata_secondary_objects[s], ColData_relation_type, &relation_tag));
										ITK_CALL(GRM_delete_relation(relation_tag));
							}

	//*************** now call file for Colour Data ***************************			
				
				clrschmnameDUP=tc_strtok(clrschmname,"-");				
				clrschmnameDUP1=tc_strtok(NULL,"-");
				clrschmnameDUP1main = replaceWord(clrschmnameDUP1, e, f);

				inputfile11=tc_strcpy(inputfile1,"");         	
				inputfile11=tc_strcat(inputfile1,"COLOR_DATA_OF_");	
				inputfile11=tc_strcat(inputfile1,clrschmnameDUP);	
				inputfile11=tc_strcat(inputfile1,"-");	
				inputfile11=tc_strcat(inputfile1,clrschmnameDUP1main);	
				inputfile11=tc_strcat(inputfile1,"__");	
				inputfile11=tc_strcat(inputfile1,clrrev);	
				inputfile11=tc_strcat(inputfile1,".txt");	

				printf("\n Line  ====%s \n",inputfile11 );fflush(stdout);  
				itemId1=tc_strtok(inputfile11,"^");
				
				fptr1=fopen(inputfile11,"r");

				if(fptr1 != NULL)
				{
					inputline1=(char *) MEM_alloc(1000);
					while(fgets(inputline1,1000,fptr1)!=NULL)
					{

					fputs(inputline1,stdout);
					
					srno=tc_strtok(inputline1,"^");
					Data_category=tc_strtok(NULL,"^");
					compcode=tc_strtok(NULL,"^");
					Intrnalschme=tc_strtok(NULL,"^");
					t5_ClSrl=tc_strtok(NULL,"^");
		//*********************************
					clrsrlmain = replaceWord(t5_ClSrl, c, d);


								t5_ClSrlDup=tc_strtok(t5_ClSrl,",");
								printf("\n t5_ClSrlDup --->%s ",t5_ClSrlDup);
								t5_ClSrlDup1=tc_strtok(NULL,"");
								printf("\n t5_ClSrlDup1 --->%s ",t5_ClSrlDup1);

								tc_strcpy(dupintrnalscheme,"");
								tc_strcat(dupintrnalscheme,t5_ClSrlDup1);
								tc_strcat(dupintrnalscheme,";");
								tc_strcat(dupintrnalscheme,t5_ClSrlDup);

					tc_strcpy(item_id_ColData,"");
					tc_strcat(item_id_ColData,compcode);
					tc_strcat(item_id_ColData,"`");
					tc_strcat(item_id_ColData,Intrnalschme);
					tc_strcat(item_id_ColData,"`");
					tc_strcat(item_id_ColData,dupintrnalscheme);

					printf("\n**************** item_id_ColData: %s  ***************\n",item_id_ColData);fflush(stdout);

					item_id_ColDatamain = replaceWord(item_id_ColData, c, d);
					printf("\n**************** item_id_ColDatamain: %s  ***************\n",item_id_ColDatamain);fflush(stdout);

				/*	if(QRY_find("ColorSchemeDataQuery", &queryTag));
					if (queryTag)
						{
							printf("Found Query 'ColorSchemeDataQuery' \n\t"); fflush(stdout);
						}
						else
						{
							printf("Not Found Query 'ColorSchemeDataQuery' \n\t"); fflush(stdout);
							return status;
						}
					char *qry_entries4[1] = {"ID"};
					char **qry_values = (char **) MEM_alloc(50 * sizeof(char *));
					qry_values[0] = (char *) MEM_alloc(50 * sizeof(char));

					tc_strcpy(item_id_ColDatamain1,"");
					tc_strcat(item_id_ColDatamain1,item_id_ColDatamain);
					tc_strcat(item_id_ColDatamain1,"*");

					qry_values[0] = item_id_ColDatamain1 ;

					printf("\n**************** item_id_ColDatamain1 query input:%s",item_id_ColDatamain1);fflush(stdout);
					
					ITK_CALL(QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &qry_output));
								printf("\n Count of Query Values ------------------>  %d \n",resultCount);  */

					if(QRY_find("ColorSchemeDataRevision", &queryTag));
					if (queryTag)
						{
							printf("Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
						}
						else
						{
							printf("Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
							return status;
						}
					char *qry_entries4[3] = {"Comp Code","Internal Scheme","Color Serial"};
					char **qry_values = (char **) MEM_alloc(50 * sizeof(char *));
					
					printf("compcode --------> %s\n",compcode);
					printf("Intrnalschme --------> %s\n",Intrnalschme);

								tc_strcpy(clrsrlmain1,"");
								tc_strcat(clrsrlmain1,t5_ClSrlDup);
								tc_strcat(clrsrlmain1,"*");
								tc_strcat(clrsrlmain1,t5_ClSrlDup1);
								printf("clrsrlmain1 --------> %s\n",clrsrlmain1);

					qry_values[0] = compcode ;
					qry_values[1] = Intrnalschme ;
					qry_values[2] = clrsrlmain1 ;
					
					ITK_CALL(QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &qry_output));
								printf("\n Count of Query Values ------------------>  %d \n",resultCount);
								
						if (resultCount == 0)
							{
							ITK_CALL(TCTYPE_find_type("T5_ClrShmData", NULL, &Colordata_type_tag));   

							ITK_CALL(TCTYPE_construct_create_input(Colordata_type_tag, &object_create_input_tag1));

							ITK_CALL(AOM_set_value_string(object_create_input_tag1,"object_name",item_id_ColDatamain));
							ITK_CALL(AOM_set_value_string(object_create_input_tag1,"item_id",item_id_ColDatamain));

							ITK_CALL(TCTYPE_create_object(object_create_input_tag1, &new_COLDATA_object));

							if(new_COLDATA_object)
							{
								printf("\n t5CreateClrschm ---------------------------> new_COLDATA_object created.");
								ITK_CALL(AOM_save(new_COLDATA_object));
								ITK_CALL(AOM_unlock(new_COLDATA_object));
							}
							ITK_CALL( ITEM_ask_latest_rev (new_COLDATA_object,&clrdatarevtag) );
							if(clrdatarevtag)
							{
								ITK_CALL(AOM_refresh(clrdatarevtag,1));
								ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_PrtCatCode",compcode));
								ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_InternalSchm",Intrnalschme));
								ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_ClSrl",clrsrlmain));
								ITK_CALL(AOM_save(clrdatarevtag));
								ITK_CALL(AOM_refresh(clrdatarevtag,0));
							}
								ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
								if(ColData_relation_type != NULLTAG)
									{
										ITK_CALL(GRM_create_relation(clrrevtag,clrdatarevtag,ColData_relation_type,NULLTAG,&new_relation));
										if(new_relation != NULLTAG)
											{
											ITK_CALL(GRM_save_relation(new_relation));
											}
									}
							}								
							else
							{
								ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
								if(ColData_relation_type != NULLTAG)
									{
									//	ITK_CALL( ITEM_ask_latest_rev (qry_output[0],&clrdatarevtag1) );
									clrdatarevtag1 = qry_output[0];
										ITK_CALL(GRM_create_relation(clrrevtag,clrdatarevtag1,ColData_relation_type,NULLTAG,&new_relation));
										if(new_relation != NULLTAG)
											{
											ITK_CALL(GRM_save_relation(new_relation));
											}
									}
							}
					}
				}
							}
									}
	}									
}
		}

//********************************* applicable SVR code starts **********************************


		
			ITK_CALL( ITEM_ask_latest_rev (new_object,&clrscmltestrev) );


		ITK_CALL(GRM_find_relation_type("T5_Applicable_SVR",&applicablesvr_relation_type));
				//delete relation
									int count2 =0;
									int s =0;
									tag_t  relation_tag = NULLTAG;
										ITK_CALL(GRM_list_secondary_objects_only(clrscmltestrev,applicablesvr_relation_type,&count2,&secondary_objects));
										if (count2 >0)
										{
											for(s=0;s<count2;s++)
											{
												ITK_CALL(GRM_find_relation(clrscmltestrev, secondary_objects[s], applicablesvr_relation_type, &relation_tag));
												ITK_CALL(GRM_delete_relation(relation_tag));
											}
										}
			ITK_CALL(AOM_UIF_ask_value(clrscmltestrev,"item_revision_id",&rev_name));
			printf("\n\tRev ---------------------> %s \n\t",rev_name);
			ITK_CALL(AOM_UIF_ask_value(clrscmltestrev,"item_id",&clr_id));
			clr_iddash = replaceWord(clr_id, e, f);
									inputfile22=tc_strcpy(inputfile2,"");         	
									inputfile22=tc_strcat(inputfile2,clr_iddash);	
									inputfile22=tc_strcat(inputfile2,"_Has_applicable_VC");	
									inputfile22=tc_strcat(inputfile2,".txt");


		printf("\n\t inputfile22 ---------------------> %s \n\t",inputfile22);

		fptr2=fopen(inputfile22,"r");

		if(fptr2 != NULL)
				{
					inputline2=(char *) MEM_alloc(1000);
					while(fgets(inputline2,1000,fptr2)!=NULL)
					{
					//applicablevc=inputline2;
					applicablevc=tc_strtok(inputline2,"^");
					printf("\nHi --------> I am while loop");
					printf("\napplicablevc ----------------> %s ",applicablevc);

			/*		tc_strcpy(applicablevcstar,"");.
					tc_strcat(applicablevcstar,applicablevc);
					tc_strcat(applicablevcstar,"*"); */

			//	printf("\napplicablevcstar ---------- %s ",applicablevcstar);

								ITK_CALL(QRY_find("VariantRule", &queryTag5));

								qry_values5[0] = applicablevc ;
							//	qry_values5[0] = applicablevcstar ;

								printf("\nqry_values5[0] ------------------> %s ",qry_values5[0]);
								
			//					ITK_CALL(QRY_execute(queryTag5, n_entries5, qry_entries5, qry_values5, &resultCount5, &qry_output5));
					  ITK_CALL(QRY_execute_with_sort(queryTag5, n_entries5, qry_entries5, qry_values5, num_to_sort, keys, orders, &resultCount5, &qry_output5));
					  printf("\nresultCount5 ------------------> %d ",resultCount5);
								if (resultCount5 != 0)
							{
									ITK_CALL(AOM_UIF_ask_value(qry_output5[0],"current_name",&vaiantname));	
									printf("\nvaiantname --------------> %s \n",vaiantname);
								
								if(applicablesvr_relation_type != NULLTAG)
									{									
										ITK_CALL(GRM_create_relation(clrscmltestrev,qry_output5[0],applicablesvr_relation_type,NULLTAG,&new_applicablesvr_relation));
										if(new_applicablesvr_relation != NULLTAG)
											{
											ITK_CALL(GRM_save_relation(new_applicablesvr_relation));
											}
									}
							}
					}
				}



//******************************** applicable SVR code END *********************************************

	}

	ITK_CALL(POM_logout(false));
	
		
	return status;
}