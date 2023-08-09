/***************************************************************************/
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


void reverse(char *s)
{
   int length, c;
   char *begin, *end, temp;
 
   length = tc_strlen(s);
   begin  = s;
   end    = s;
 
   for (c = 0; c < length - 1; c++)
      end++;
 
   for (c = 0; c < length/2; c++)
   {        
      temp   = *end;
      *end   = *begin;
      *begin = temp;
 
      begin++;
      end--;
   }
}

char*  GetPlantForDMLORTask( char * item_id )
{
    printf( "item_id:%s\n", item_id);

	char *item_idCpy =NULL;
	char *revStr =NULL;
	char *tmpStr =NULL;
	char *revStr1 =NULL;
	item_idCpy	=	(char *)MEM_alloc(50);

	tc_strcpy(item_idCpy,item_id);
	reverse(item_idCpy);
	revStr	=	(char *)MEM_alloc(50);
	tc_strcpy(revStr,item_idCpy);
	printf("\n revStr Find %s.......",revStr);fflush(stdout);
	tmpStr	=	strtok(revStr,"_");
	printf("\n tmpStr Find %s.......",tmpStr);fflush(stdout);
	revStr1	=	(char *)MEM_alloc(50);
	reverse(tmpStr);
	tc_strcpy(revStr1,tmpStr);
	printf("\n revStr1 Find %s.......",revStr1);fflush(stdout);
	//getPlant	=	(char *)MEM_alloc(10);
	//tc_strcpy(getPlant,revStr1);
	return revStr1;
	//printf( "getPlant:%s\n", *getPlant);
}

int createControlObject(char	*Tasknumber, char	*PlantName)
{
	int		status				=	0;
	int		n_strings			=	1;
	int		l_strings			=	500;
	int		index				=	0;
	tag_t	cntrl_obj_tag       =	NULLTAG;
	tag_t	CntrlObjCreInTag	=	NULLTAG;
	tag_t	CntrlObjTag			=	NULLTAG;

	char	**stringArrayAPLT	=	NULL;
	char	*objectName			=	NULL;

	printf("\nIndide createControlObject...!!!");fflush(stdout);

	stringArrayAPLT = (char**)malloc( n_strings * sizeof *stringArrayAPLT );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLT[index] = (char*)malloc( l_strings + 1 );
	}

	ITK_CALL(TCTYPE_find_type("T5_ControlObject", "T5_ControlObject", &cntrl_obj_tag));

	ITK_CALL( TCTYPE_construct_create_input( cntrl_obj_tag, &CntrlObjCreInTag) );

	tc_strcpy( stringArrayAPLT[0], "STDVErr");
	ITK_CALL( TCTYPE_set_create_display_value( CntrlObjCreInTag, "t5_Syscd", 1,(const char**)stringArrayAPLT) );

	tc_strcpy( stringArrayAPLT[0], "SignOffWar");
	ITK_CALL( TCTYPE_set_create_display_value( CntrlObjCreInTag, "t5_SubSyscd", 1,(const char**)stringArrayAPLT) );
	
	if (tc_strlen(Tasknumber)==10)
	{
		tc_strcpy( stringArrayAPLT[0], "NonePOPartEPAMSG");
	}
	else
	{
		tc_strcpy( stringArrayAPLT[0], "NonePOPartTaskMSG");
	}
	ITK_CALL( TCTYPE_set_create_display_value( CntrlObjCreInTag, "t5_Userinfo1", 1,(const char**)stringArrayAPLT) );

	tc_strcpy( stringArrayAPLT[0], Tasknumber);
	ITK_CALL( TCTYPE_set_create_display_value( CntrlObjCreInTag, "t5_Userinfo4", 1,(const char**)stringArrayAPLT) );

	tc_strcpy( stringArrayAPLT[0], PlantName);
	ITK_CALL( TCTYPE_set_create_display_value( CntrlObjCreInTag, "t5_Userinfo3", 1,(const char**)stringArrayAPLT) );
	

	if (!objectName) MEM_free(objectName);
	objectName=(char *) MEM_alloc(150);
	tc_strcpy(objectName,"STDVErr");
	tc_strcat(objectName,",");
	tc_strcat(objectName,"SignOffWar");
	printf("\n objectName == %s\n",objectName);fflush(stdout);
	
	tc_strcpy( stringArrayAPLT[0], objectName);
	ITK_CALL( TCTYPE_set_create_display_value( CntrlObjCreInTag, "object_name", 1,(const char**)stringArrayAPLT) );

	printf("\nTest1..\n");
	ITK_CALL( TCTYPE_create_object(CntrlObjCreInTag, &CntrlObjTag) );
	printf("\nTest2..\n");

	AOM_save(CntrlObjTag);

	return 0;
}

int ITK_user_main(int argc,char* argv[])
{
	FILE	*fpByPsCntr			=	NULL;
	
	int		status				=	0;
	int		n_tags_found		=	0;

	char	*Tasknumber			=	NULL;
	char	*PlantName			=	NULL;
	char	*FileName			=	NULL;
	char	*FilePath			=	NULL;
	char	*getPlant			=	NULL;

	tag_t	*tags_found			=	NULLTAG;
	tag_t	itemcldclass		=	NULLTAG;

	Tasknumber	= ITK_ask_cli_argument("-i=");
	PlantName	= ITK_ask_cli_argument("-pl=");
	FilePath	= ITK_ask_cli_argument("-fp=");

	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_set_journalling( TRUE ));
	ITK_CALL(ITK_auto_login( ));

	printf("Task Number : %s, PlantName : %s, tc_strlen(Tasknumber) : %d",Tasknumber,PlantName,tc_strlen(Tasknumber));fflush(stdout);

	FileName	=	(char*)malloc(tc_strlen(FilePath)+tc_strlen(Tasknumber)+tc_strlen(PlantName)+25);
	tc_strcpy(FileName,FilePath);
	tc_strcat(FileName,Tasknumber);
	tc_strcat(FileName,"_");
	tc_strcat(FileName,PlantName);
	tc_strcat(FileName,"_");
	tc_strcat(FileName,"_POByPs_");
	tc_strcat(FileName,".txt");
	printf("\nFilename : %s",FileName);fflush(stdout);

	fpByPsCntr	=	fopen(FileName,"w");

	//check if control object is already created in system or not.
	//if not created then only created in system.
	if (tc_strlen(Tasknumber)>=10)
	{
		char    *qry_entryCntrl[5]  = {"SYSCD","SUBSYSCD","Information-1","Information-3","Information-4"};	
		char	**qry_valuesCntrl= (char **) MEM_alloc(7 * sizeof(char *));

		tag_t   qryTagCntrl					=	NULLTAG;
		int		control_number_foundPrj		=	0;
		tag_t	*list_of_WSO_cntrlPrj_tags	=	NULLTAG;
		int     n_entryCntrl				=	5;


		ITKCALL(QRY_find("Control Objects...", &qryTagCntrl));
		
		if (qryTagCntrl!=NULLTAG)
		{
		
			printf("\n After find qryyy-- ");fflush(stdout);
			qry_valuesCntrl[0]="STDVErr";
			qry_valuesCntrl[1]="SignOffWar";
			qry_valuesCntrl[2]="NonePOPartTaskMSG";
			qry_valuesCntrl[3]=PlantName;
			qry_valuesCntrl[4]=Tasknumber;

			if(QRY_execute(qryTagCntrl, n_entryCntrl, qry_entryCntrl, qry_valuesCntrl, &control_number_foundPrj, &list_of_WSO_cntrlPrj_tags));
			printf("\n n_found1: %d", control_number_foundPrj);fflush(stdout);
			if (control_number_foundPrj==0)	
			{
				//if APL Task is found then go ahead with APL Task control object creation
				//else check for EPA, if EPA found the go ahead with EPA control object creation else give error message.

				//Find APL DML Master
				const char *qry_entries[2];
				const char *qry_values[2];

				qry_entries[0] ="item_id";
				qry_entries[1] ="object_type";
				qry_values[0] = Tasknumber;
				if (tc_strlen(Tasknumber)==10)
				{
					//EPA
					qry_values[1] = "T5_EPA";
				}
				else
				{	
					if (tc_strstr(Tasknumber,"STD")!=NULL)
					{
						qry_values[1] = "T5_SMDMLTask";
					}
					else
					{
						qry_values[1] = "T5_APLTask";
					}
				}
				printf("\nqry_values[0] : %s , qry_values[1] : %s",qry_values[0],qry_values[1]);fflush(stdout);
				
				
				ITK_CALL(ITEM_find_items_by_key_attributes(2, qry_entries, qry_values,&n_tags_found, &tags_found));
				printf("\n\nITEM_find_items_by_key_attributes Item_id count in DB is : %d\n",n_tags_found);fflush(stdout);

				if (n_tags_found>0)
				{
					printf("\nAPL TASK FOUND...!!!");fflush(stdout);
					itemcldclass = tags_found[0];

					/*control object creation start*/
					tag_t  	item_rev_cld_tag_orig	=	NULLTAG;
					ITKCALL(ITEM_ask_latest_rev(itemcldclass,&item_rev_cld_tag_orig));

					char	*cAPLDMLNumber		=	NULL;

					ITK_CALL(AOM_ask_value_string(item_rev_cld_tag_orig,"item_id",&cAPLDMLNumber));
					printf("\nAPL Task Number : %s",cAPLDMLNumber);fflush(stdout);

					//create bypass control object
					getPlant	=	GetPlantForDMLORTask(cAPLDMLNumber);
					printf("\nPlant Name : %s", getPlant);fflush(stdout);
					
					createControlObject(Tasknumber, PlantName);
					fprintf(fpByPsCntr,"1");fflush(fpByPsCntr);//Control object created
				}
				else
				{
					printf("\nNo Task found, check for EPA");fflush(stdout);
					fprintf(fpByPsCntr,"-1");fflush(fpByPsCntr);//Task not found
					exit(0);
				}
			}
			else
			{
				printf("\nAlready created...!!!");fflush(stdout);
				fprintf(fpByPsCntr,"0");fflush(fpByPsCntr);//Control object already exist
				exit(0);
			}
		}
		printf("\nProgram Ends...!!!");fflush(stdout);
	}
	else
	{
		fprintf(fpByPsCntr,"-1");fflush(fpByPsCntr);//Task not found
	}
}