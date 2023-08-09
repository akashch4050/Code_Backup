#include "soapH.h"
#include "classificationInfo.nsmap"
#include "tc/tc_startup.h"
#include "tc/tc_macros.h"
#include "string.h"
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
#include <ae/dataset.h>
#include <tccore/libtccore_exports.h>
#define Debug TRUE

#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) { PrintErrorStack(); return ifail;}
#define ITEM_id_size_c   128
static int PrintErrorStack( void )
{
    int iNumErrs = 0;
	const int*      pSevLst = 0;
	const int*      pErrCdeLst = 0;
	const char**    pMsgLst = NULL;
	FILE *errfile =NULL;
 

    register int i = 0;
	errfile =fopen("classificationInfoError.txt", "a");
    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst );

    fprintf(errfile,"CopyTemplates Error(s): \n");

    for ( i = 0; i < iNumErrs; i++ )
    {
        fprintf(errfile,"\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );		
       fprintf(errfile,"\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );

    }
	fclose(errfile);
    return ITK_ok;
}


extern int ITK_user_main(int argc, char *argv[])
{

	return soap_serve(soap_new()); /* call the request dispatcher */

}
;

int ns__GetDetailsOfClassification(struct soap* soap,char *Item,struct ns__StructOutput *aStructObjToReturn)
{
	int status;
	int ifail =0;
	//char	*PartId	= "282960911204_D/NR;1";
	 tag_t *tags=NULLTAG;
	 tag_t 	classificationObject=NULLTAG;
	 int count = 0 ;
	 tag_t view=NULLTAG;
	 int  	nAttributes=0;
	int * 	unctNumbers;
	int * 	unctFormats;
	char ** 	attributeNames;
	char ** 	attributeValues;
	char ** 	units;
	char * 	objectId;
	int  	theCount,j,i;
	tag_t * 	theInstanceTags ;
	char * 	theClassId=NULL;
	FILE *fp =NULL;
	

char * 	theParent;
int  	theType;
char * 	theName;
char * 	theShortName;
char * 	theDescription;
char * 	theUserData1;
char * 	theUserData2;
int theOptions;
int theInstanceCount;
int theChildrenCount;
int theViewCount;



		fpos_t pos;
		fgetpos(stdout, &pos);
		int fd = dup(fileno(stdout));
		freopen("classificationInfostd.txt", "a", stdout);
		fp =fopen("classificationInfolog.txt", "a");
		fprintf(fp,"\n Calling ITK_initialize_text_services..");


		fprintf(fp,"\n Calling ITK_initialize_text_services..");
		CALLAPI(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
		//if( ITK_init_module("infodba" ,"infodba","dba")!=ITK_ok) ;
		//CALLAPI(ITK_init_module("infodba" ,"infodba","dba")) ;
		CALLAPI( ITK_auto_login( )) ;
		CALLAPI(ITK_set_journalling( TRUE ));
		fprintf(fp,"\n Login Success....");fflush(stdout);

//printf("\n After Login"); fflush(stdout);
//ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
//ITK_CALL(ITK_auto_login());
//ITK_auto_login();
CALLAPI(ICS_ask_instances_by_id(Item,&theCount,&theInstanceTags));
	if ( theCount> 0 )
	{
		CALLAPI(ICS_ask_classification_object_id ( theInstanceTags[0],&objectId));
		//ITK_CALL(ICS_ico_ask_class	(theInstanceTags[0],&theClassId ));
		CALLAPI(ICS_ico_ask_class	(theInstanceTags[0],&theClassId ));
		//ITK_CALL(ICS_class_describe(theClassId,&theParent,&theType,&theName,&theShortName,&theDescription,&theUserData1,&theUserData2,&theOptions,&theInstanceCount,&theChildrenCount,&theViewCount));
		CALLAPI(ICS_class_describe(theClassId,&theParent,&theType,&theName,&theShortName,&theDescription,&theUserData1,&theUserData2,&theOptions,&theInstanceCount,&theChildrenCount,&theViewCount));
		//ITK_CALL(ICS_describe_classification_object(theInstanceTags[0],&view,&nAttributes,&unctNumbers,&unctFormats,&attributeNames,&attributeValues,&units));
		CALLAPI(ICS_describe_classification_object(theInstanceTags[0],&view,&nAttributes,&unctNumbers,&unctFormats,&attributeNames,&attributeValues,&units));
		
		aStructObjToReturn-> __size = nAttributes;
		aStructObjToReturn->__ptrItem = malloc( (aStructObjToReturn->__size + 1) * sizeof(*aStructObjToReturn->__ptrItem) );
		for(i=0;i< nAttributes;i++)
		{
		aStructObjToReturn->__ptrItem[i].AtrNames = malloc(150 * sizeof(char *) );
		aStructObjToReturn->__ptrItem[i].AtrValuesStr = malloc(100 * sizeof(char *) );
		aStructObjToReturn->__ptrItem[i].ClassName = malloc(100 * sizeof(char *) );
		strcpy(aStructObjToReturn->__ptrItem[i].AtrNames,attributeNames[i]);
		strcpy(aStructObjToReturn->__ptrItem[i].AtrValuesStr,attributeValues[i]);
		strcpy(aStructObjToReturn->__ptrItem[i].ClassName,theName);
		}
		
	}

	fprintf(fp,"\nDone .\n");
		fclose(fp);
		dup2(fd, fileno(stdout));
		close(fd);
		clearerr(stdout);
		fsetpos(stdout, &pos);

	return SOAP_OK;



}
;