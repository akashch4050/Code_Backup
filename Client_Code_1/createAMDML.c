 //#include<iostream>
#include <tccore/aom.h>
#include <res/res_itk.h>
#include <bom/bom.h>
#include <ctype.h>
#include <epm/cr.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <itk/mem.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ps/ps.h>
#include <time.h>
#include <pie/pie.h>        //Added by Vishal to expand BOM
#include <tccore/uom.h>
#include <user_exits/user_exits.h>
#include <rdv/arch.h>
#include <stdlib.h>
#include <string.h>
#include <textsrv/textserver.h>
#include <tccore/item_errors.h>
#include <stdlib.h>
#include <ae/dataset.h>
#include <assert.h>
//#include <envelope.h>
#include <tccore/libtccore_exports.h>
//#include <itkVectorContainer.h>
#define Debug TRUE
#define ITK_CALL(X) 							\
		if(Debug)								\
		{										\
			printf("\n");							\
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
			/*return status; */													\
		}																	\
		else									\
		{										\
			if(Debug)							\
			printf("\tSUCCESS\n");				\
		}
int status=0;
int static totalPartCount=0;
int static flgChildPartDuplcy=0;
logical StatusFound =  false;
 //int AMDMLCreateFuncTOO(char  *project,char *dsgnGrp,char  *dvrNum,char  *WbsNameMain,char *WbsDescriptionMain,char *usrNameDup,int FlagPR,char *PlantName,tag_t *AMDmlObj)

extern int ITK_user_main (int argc, char ** argv )
{

	//int     status;
	int     ifail;
	
	//char	*cUserName		=	NULL;
	//char	*cPassWord		=	NULL;
	//char	*cUserGrp		=	NULL;
	char	*DMLNumber		=	NULL;
	char	*project		=	NULL;
	char	*usergrp		=	NULL;
	char	*dsggrp			=	NULL;
	char	*ecntype		=	NULL;
	char	*Response		=	NULL;

	tag_t	AMDmlObj		=	NULLTAG;
	
	//cUserName		=	ITK_ask_cli_argument("-UN=");
	//cPassWord		=	ITK_ask_cli_argument("-UP=");
	//cUserGrp		=	ITK_ask_cli_argument("-UG=");
	project			=	ITK_ask_cli_argument("-AP=");
	usergrp			=	ITK_ask_cli_argument("-AU=");
	dsggrp			=	ITK_ask_cli_argument("-AD=");
	ecntype			=	ITK_ask_cli_argument("-AE=");

	//printf("\nUsername : %s, Group : %s",cUserName,cUserGrp);fflush(stdout);

	printf("\nProject : %s, User : %s, design group : %s, ecn type : %s ",project,usergrp,dsggrp,ecntype);fflush(stdout);

	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_set_journalling(TRUE));
	//ITK_CALL(ITK_init_module(cUserName,cPassWord,cUserGrp));
	ITK_CALL(ITK_auto_login( ));
	
	//response	=	creAMDMLInTCE("APLCAR","5442","TCUA APL DML", "00", "MCNote");
	Response	=	creAMDMLInTCE(usergrp,project,"SYSTEM GENERATED APL DML FOR TCUA", dsggrp, ecntype);
	printf("\n%s",Response);fflush(stdout);
	
	return status;
}



//createAMDML -u=infodba -p=APLinfo2020 -g=DBA -AP=5442 -AU=APLCAR -AD=00 -AE=MCNote