/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File			 :   Cntrlobj.c
*  Author		 :   Priya Kharate
*  Module		 :   Control Object create in TCUA
*                            
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes
***************************************************************************/
#include <tcinit/tcinit.h>
#include <tc/tc_startup.h>
#include <tc/emh.h>
#include <tccore/aom.h>
#include <res/res_itk.h>
#include <bom/bom.h>
#include <ctype.h>
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
#include <tccore/tctype.h>
#include <ae/dataset.h>
#include <tccore/libtccore_exports.h>
extern TCCORE_API int TCTYPE_ask_type(
        const char      *type_name,
        tag_t           *type_tag);
#include <tccore/libtccore_undef.h>
//#define TCTYPE_name_size_c 100
#define TCTYPE_name_size_c1 100
//#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
//#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return


static void handle_ifail(char *file, int line);
static void MultiExplosion (tag_t line, char *Plant, tag_t line1, int depth,FILE *fd,int reqLevel,int level);
static void MultiExplosionLinkInfo (tag_t line, char *Plant, tag_t line1, int depth,FILE *fd,char *Uidd,int reqLevel,int level);
#define HANDLE_IFAIL   handle_ifail( __FILE__, __LINE__ )
#define CHECK_IFAIL    if ( ifail != ITK_ok ) HANDLE_IFAIL;
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
			return status;													\
		}																	\
	;
	
int ITK_user_main(int argc,char* argv[])
{
int z;
int status;

                tag_t dml_type_tag                            = NULLTAG;
                tag_t object_create_input_tag                 = NULLTAG;
                tag_t new_object                              = NULLTAG;
                tag_t rev_type_tag                            = NULLTAG;
                
				FILE* fptr = NULL;
				
	char *inputfile = NULL;	
	char *inputline = NULL;
	
	char	*ControlObject			= NULL;
	char	*syscd		= NULL;
	char	*subsyscd	= NULL;
	char	*itemId	= NULL;
	
	
	char	*info1		= NULL;


	char	*info2		= NULL;

	char	*info3	= NULL;

	char	*info4		= NULL;

	int j=0;
	int flagFound=0;
	
inputfile = ITK_ask_cli_argument("-i=");
ITK_CALL(ITK_set_journalling( TRUE ));
ITK_CALL(ITK_initialize_text_services (0));
printf("\n\n\t Attempting Logging\n ");

z=ITK_CALL(ITK_auto_login());
if(z!=ITK_ok)
  {
	  printf("\n\n\t OMG Login Not Successfull\n ");
  }
  else
	{
	  printf("\n\n\t Login Successfull\n ");
	}

	
printf("\n\n\t Login Successfull\n ");

//fptr=fopen("D:\\TCUA_NewInstall_InstallDir\\input.txt","r");
fptr=fopen("//user//uaprod//Priya//input.txt","r");
	printf("\n file read .......");fflush(stdout);
	

                if(fptr!=NULL)
                {
                                inputline=(char *) MEM_alloc(2000);
                                while(fgets(inputline,2000,fptr)!=NULL)
                                {
									flagFound=0;
								printf("\n inputline .......%s",inputline);fflush(stdout);
                                fputs(inputline,stdout);
                                
                                info1=tc_strtok(inputline,"^");
                                info2=tc_strtok(NULL,"^");
                                info3=tc_strtok(NULL,"^"); 
							

                                printf("\n info ====%s,%s,%s",info1,info2,info3 );fflush(stdout);

								//outputStr = info3.Replace("\"","").Replace("'","");
							//	 printf("\n outputStr ====%s",outputStr );fflush(stdout);


//////////////////////////////////////////////

    /* va_list for LOV_ask_values_msg */
    
	//char *userinfo_value=NULL;
	char *Information1=NULL;
	char *Information2=NULL;
	char *Information3=NULL;
	//char *information1_value = NULL;
	//char final_information1_value[20];
	
	//int status;

    int
        //error_code = ITK_ok,
        n_entries = 2,
        n_found = 0,
        ii = 0;
	
    tag_t
        query = NULLTAG,
        *cntr_objects = NULL;
       

	char
        *qry_entries[2] = {"SYSCD","SUBSYSCD"},
        *qry_values[2]	= {"APNS_X4","SubDsgGrp"};


	//printf("\nTCDCcreate_dynamic_lov.c::myAskLOVStartPartValuesMethod ");fflush(stdout);


    ITK_CALL(QRY_find("ControlObjQry", &query));
	printf("\nTCDC-- ");fflush(stdout);
    ITK_CALL(QRY_execute(query, n_entries, qry_entries, qry_values, &n_found, &cntr_objects));
    printf("\nTCDC=>%d", n_found);fflush(stdout);
	
	
	for (j=0;j<n_found;j++)
	{
	
		ITK_CALL ( AOM_ask_value_string(cntr_objects[j],"t5_Userinfo1",&Information1));
		ITK_CALL ( AOM_ask_value_string(cntr_objects[j],"t5_Userinfo2",&Information2));
		ITK_CALL ( AOM_ask_value_string(cntr_objects[j],"t5_Userinfo3",&Information3));
		printf("\ninformation_value == %s,%s,%s\n",Information1,Information2,Information3 );fflush(stdout);
		printf("\n info ====>%s,%s,%s\n",info1,info2,info3 );fflush(stdout);
		if(!tc_strcmp(Information1, info1) && !tc_strcmp(Information2, info2) && !tc_strcmp(Information3, info3))
		{
			flagFound=1;
			break;		
		}
		
	 }
	 printf("\nflagFound == %d\n",flagFound);fflush(stdout);
	if(flagFound==0)
	{
			ITK_CALL(TCTYPE_find_type("T5_ControlObject", "T5_ControlObject", &dml_type_tag));   

				ITK_CALL(TCTYPE_construct_create_input(dml_type_tag, &object_create_input_tag));
				
				ITK_CALL(TCTYPE_create_object(object_create_input_tag, &new_object));
				if(new_object)
				{
				ITK_CALL(AOM_set_value_string(new_object,"object_name","APNS_X4"));
				ITK_CALL(AOM_set_value_string(new_object,"t5_Syscd","APNS_X4"));
				ITK_CALL(AOM_set_value_string(new_object,"t5_SubSyscd","SubDsgGrp"));
				ITK_CALL(AOM_set_value_string(new_object,"t5_Userinfo1",info1));
				ITK_CALL(AOM_set_value_string(new_object,"t5_Userinfo2",info2));
				ITK_CALL(AOM_set_value_string(new_object,"t5_Userinfo3",info3));
				

			   // ITK_CALL(TCTYPE_create_object(object_create_input_tag, &new_object));
   
				printf("\n t5CreateObject : object created.");
				ITK_CALL(AOM_save(new_object));
				printf("\n t5CreateObject : object saved.");
				//ITK_CALL(AOM_unlock(new_object));

				}
		}
	 
	}
                
                                               
                                }
           

	//TAG_free(cntr_objects);
    //TAG_free(userinfo_value);

CLEANUP:
	ITK_CALL(ITK_exit_module(TRUE));
	return status;
}
