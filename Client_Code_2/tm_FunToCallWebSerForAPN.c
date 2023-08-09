#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unidefs.h>
#include <itk/mem.h>
#include <tcinit/tcinit.h>
#include <tccore/item.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ps/ps_errors.h>
#include <tccore/item_errors.h>
#include <tc/emh.h>
#include <ae/dataset.h>
#include <tccore/tctype.h>
#include <malloc.h>
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

#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}

//char** GetClassDetails(char *,int *, int **, int **, int **, int **, int **);
char** DZ_VendorDetail(char *,int *, int **, int **, int **, int **);

static int name_attribute, seqno_attribute, parent_attribute, item_tag_attribute;
char* WebSerFunForAPN(char*);
static void initialise (void);
static void initialise_attribute (char *name,  int *attribute);
static void ExpandMultiLevelBom (tag_t bom_line_tag, int depth,char* plant,FILE *fptr);
extern int ITK_user_main (int argc, char ** argv )
{
	int ifail;
	char* Response = NULL ;	
	int status;
	char *u = NULL;
	char *p= NULL;
	char *g = NULL;
	char *ID= NULL;
	char *GID = NULL;
	FILE *fp = NULL ;
	//char *inputfile=NULL;
	//char *readresponse;

	(void)argc;
	(void)argv;


	u = ITK_ask_cli_argument("-u=");
	p = ITK_ask_cli_argument("-p=");
	g = ITK_ask_cli_argument("-g=");
	ID = ITK_ask_cli_argument("-i=");
	

	if( ITK_init_module(u,p,g)!=ITK_ok) ;
	
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_auto_login( ));
	ITK_CALL(ITK_set_journalling( TRUE ));
	//printf("\n In FunToCallWebSerForAPN: login success .......");fflush(stdout);
	//printf("\n In FunToCallWebSerForAPN: Calling WebSerFunForAPN");fflush(stdout);
	//inputfile=(char *) MEM_alloc(50);
	//tc_strcpy(inputfile,"/tmp/");
	//tc_strcat(inputfile,GID);
	//tc_strcat(inputfile,"_");
	//tc_strcat(inputfile,ID);
	//tc_strcat(inputfile,".txt");
	
	//printf("\n In FunToCallWebSerForAPN: inputfile [%s]",inputfile);fflush(stdout);
	
	Response = WebSerFunForAPN(ID);
//	printf("\n In FunToCallWebSerForAPN: Ending WebSerFunForAPN: [%s]",Response);fflush(stdout);
	printf("[%s]",Response);fflush(stdout);
	//ITK_exit_module(true);
	
	//fp=fopen(inputfile,"w");
	//fprintf(fp, "%s",Response);fflush(fp);

	return status;
}

