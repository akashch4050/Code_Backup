//==================================================
//
//  Copyright 2012 Siemens Product Lifecycle Management Software Inc. All Rights Reserved.
//
//==================================================
#include <tc/tc.h>
//#include <custom.h>
#include <epm/epm.h>
#include <ae/dataset.h>
#include <ict/ict_userservice.h>
#include <res/reservation.h>
//#include <method.h>
#include <property/prop.h>
#include <property/prop_errors.h>
#include <property/prop_msg.h>
#include <tc/tc.h>
#include <tc/emh_const.h>
#include <tc/envelope.h>
#include <tc/emh.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tccore/item_msg.h>
#include <tccore/method.h>
#include <tccore/tctype.h>
#include <tccore/workspaceobject.h>
#include <sa/tcfile.h>
//#include <tctype.h>
//#include <item_errors.h>
#include <user_exits/user_exits.h>
//#include <workspaceobject.h>
//#include <project.h>
#include <tccore/tc_msg.h>
#include <tc/iman.h>
#include <ai/sample_err.h>
#include <pom/pom/pom.h>
//#include <item.h>
//#include <aom.h>
//#include <aom_prop.h>
#include <stdio.h>
#include <stdlib.h>
#include <qry/qry.h>
#include <fclasses/iman_date.h>
#include <cfm/cfm.h>
#include "soapH.h"
#include "CreNClassifyObj.nsmap"
#include "tc/tc_startup.h"
#include "tc/tc_macros.h"
#include "string.h"
#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) { PrintErrorStack(); return ifail;}
#define ITEM_id_size_c   128
static char* default_empty_to_A(char *s)
{
    return (NULL == s ? s : ('\0' == s[0] ? "A" : s));
}
static int PrintErrorStack( void )
{
    int iNumErrs = 0;
	const int*      pSevLst = 0;
	const int*      pErrCdeLst = 0;
	const char**    pMsgLst = NULL;
	//FILE *fp1 = NULL ;
 

    register int i = 0;
	//fp1 = fopen("CreateCreNClassifyApi.txt","w");

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst );
   // fprintf(fp1, stderr, "CopyTemplates Error(s): \n");
	//fprintf(fp1,"CopyTemplates Error(s): \n");
    for ( i = 0; i < iNumErrs; i++ )
    {
       // fprintf(fp1, stderr, "\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
		//fprintf(fp1,"\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i]);
     //   fprintf(fp1, stderr, "\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
	//	fprintf(fp1,"\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i]);
    }
	//fclose ( fp1 );
    return ITK_ok;
}
int save_object (tag_t     object_tag)
{
    int ReturnCode;

    ReturnCode = AOM_save (object_tag);
    if (ReturnCode != ITK_ok)
    {
       return (ReturnCode);
    }

    ReturnCode = AOM_unlock (object_tag);
    if (ReturnCode != ITK_ok)
    {
       ;
    }

    return (ReturnCode);
}
extern int ITK_user_main(int argc, char *argv[])
{

	return soap_serve(soap_new()); /* call the request dispatcher */

}
;
int ns__CreateCreNClassifyObj ( struct soap* soap,int NoOfAttr2,char* cItemName2,char *strClassName2,char *InputIdsArr2,char *InputValuesArr2,struct ns__CharArray3 *aString_test3)
{
	int ifail =0 ;
	FILE *fp = NULL ;
	int count = 0 ;
	tag_t* tags ;
	tag_t item_rev_tags ;
	tag_t item = NULLTAG;
	tag_t rev = NULLTAG;
	tag_t datasettype_tag = NULLTAG;
	tag_t default_tool_tag;
	char * new_name1 = NULL;
	char * new_name2 = NULL;
	int n_items = 0;
	tag_t *item_tags = NULLTAG;
	tag_t tDataset = NULLTAG;
	char **list;
	tag_t imannewFileTag = NULLTAG;
	IMF_file_t filedescriptor;
	tag_t tRelationFind1 = NULLTAG;
	tag_t tRelationExist1 = NULLTAG;
	tag_t tRelationObj1 = NULLTAG;
	int index = 0;
	char *sclassid = NULL;
	WSO_search_criteria_t criteria;
	//tag_t* tags;
	char* item_revision_id = NULL;
	char revision[3]= {0};
	int iRevision = 0;
	int iLastRev = 0;
	tag_t cItemRevTagObject = NULLTAG;
	tag_t 	theClassifiedObjectTag =  NULLTAG;
	tag_t * 	theICOTag;
	tag_t 	ws_object = NULLTAG;
	tag_t   	theClassTag	 ;
	tag_t  	classification_object;

		char * 	LovStr =NULL;
		int *InputIdsArr1;
		char **InputValuesArr1=NULL;
		//int j=0;
		int c =0;
		int i =0;
		int id11 =0;
		int length =0;
		
		
		fpos_t pos;
		fgetpos(stdout, &pos);
		int fd = dup(fileno(stdout));
		freopen("CreateCreNClassifyObj123.txt", "w", stdout);
		//NoOfAttr2=16;

	//int *InputIdsArr1;
	//char **InputValuesArr1=NULL;
	//int InputIdsArr1[16] ={1149,1150,1151,1152,1153,1154,1155,1156,1157,1158,1160,1163,1165,1166,1167,1169};
	//char *InputValuesArr1[16]={"-2449","25","33",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	
	
	CALLAPI(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	CALLAPI(ITK_auto_login( ));
	CALLAPI(ITK_set_journalling( TRUE ));	
	fp = fopen("CreateCreNClassifyObj.txt","w+");
	fprintf(fp, "\nConencting to CreateCreNClassifyObj");
	aString_test3->StrRsps = (char*)malloc(50);	
	strcpy(aString_test3->StrRsps,"Classification Done");


//***
		InputIdsArr1=(int*) malloc(NoOfAttr2 * sizeof(int) );
		InputValuesArr1=(char**) malloc(NoOfAttr2 * sizeof(char*) );
	for(i=0;i<NoOfAttr2;i++)
	{
			InputValuesArr1[i]=NULL;
	}
			


	LovStr=strtok(InputIdsArr2,"#");

	while( LovStr != NULL )
	{
		id11=atoi(LovStr);
		InputIdsArr1[c]= id11;
		c++;
		LovStr = strtok( NULL, "#" );
	}

	c =0;
	if((InputValuesArr2 !=NULL) && (tc_strcmp(InputValuesArr2,"")!=0))
	{
	LovStr=strtok(InputValuesArr2,"#");

	while( LovStr != NULL )
	{
		length = strlen(LovStr);
		fprintf(fp,"\n length .. %d",length);
		InputValuesArr1[c]= malloc(length * sizeof(char) );
		if( tc_strcmp(LovStr," ")==0)
		{
		InputValuesArr1[c]=NULL;		
		}
		else
			strcpy(InputValuesArr1[c],LovStr);
		c++;
		LovStr = strtok( NULL, "#" );
	}
	}


for(i=0;i<NoOfAttr2;i++)
	{
	 fprintf(fp,"\n InputIdsArr2 .. %d    ValuesArr : %s",InputIdsArr1[i],InputValuesArr1[i]);
	}



//***


//printf("\nafter 1");



	fprintf(fp,"\n Input .. %s",strClassName2);

		if((strClassName2 !=NULL) && (tc_strcmp(strClassName2,"")!=0))
		{
			sclassid = malloc(20);	
		 if(tc_strcmp(strClassName2,"Front Spring")==0)
			{
				strcpy(sclassid,"ICM01010101");
			}
			else if(tc_strcmp(strClassName2,"CVBU")==0)
			{
				strcpy(sclassid,"ICM01040101");
			}
			else if(tc_strcmp(strClassName2,"Wheel Rim")==0)
			{
				strcpy(sclassid,"ICM01070101");
			}
			else if(tc_strcmp(strClassName2,"FRONT AXLE")==0)
			{
				strcpy(sclassid,"ICM030101");
			}
			else if(tc_strcmp(strClassName2,"Front Axle")==0)
			{
				strcpy(sclassid,"ICM01020101");
			}
			else if(tc_strcmp(strClassName2,"Fuel Tank")==0)
			{
				strcpy(sclassid,"ICM020101");
			}
			else if(tc_strcmp(strClassName2,"Bump Stopper")==0)
			{
				strcpy(sclassid,"ICM01050101");
			}
			else if(tc_strcmp(strClassName2,"Spring")==0)
			{
				strcpy(sclassid,"ICM01050102");
			}
			else if(tc_strcmp(strClassName2,"Strut")==0)
			{
				strcpy(sclassid,"ICM01050103");
			}
			else if(tc_strcmp(strClassName2,"Front ARB Bush")==0)
			{
				strcpy(sclassid,"ICM01050104");
			}
			else if(tc_strcmp(strClassName2,"Front ARB")==0)
			{
				strcpy(sclassid,"ICM01050105");
			}
			else if(tc_strcmp(strClassName2,"Spring Seat Bottom")==0)
			{
				strcpy(sclassid,"ICM01050106");
			}
			else if(tc_strcmp(strClassName2,"ARB Clamp")==0)
			{
				strcpy(sclassid,"ICM01050107");
			}
			else if(tc_strcmp(strClassName2,"ARB Ball Joint")==0)
			{
				strcpy(sclassid,"ICM01050108");
			}
			else if(tc_strcmp(strClassName2,"Spring seat Top")==0)
			{
				strcpy(sclassid,"ICM01050109");
			}
			else if(tc_strcmp(strClassName2,"Thrust Bearing")==0)
			{
				strcpy(sclassid,"ICM01050110");
			}
			else if(tc_strcmp(strClassName2,"Bearing")==0)
			{
				strcpy(sclassid,"ICM01050111");
			}
			else if(tc_strcmp(strClassName2,"Fastner")==0)
			{
				strcpy(sclassid,"ICM010601");
			}
			else if(tc_strcmp(strClassName2,"Winch")==0)
			{
				strcpy(sclassid,"ICM040101");
			}
			//******As Per New Sructure
			else if(tc_strcmp(strClassName2,"LEAF SPRING(M3210-001.10)")==0)
			{
				strcpy(sclassid,"ICM01010101");
			}
			else if(tc_strcmp(strClassName2,"BUMP STOPPER(M3210-001.5)")==0)
			{
				strcpy(sclassid,"ICM01050101");
			}
			else if(tc_strcmp(strClassName2,"COIL SPRING(M3210-001.9)")==0)
			{
				strcpy(sclassid,"ICM01050102");
			}
			else if(tc_strcmp(strClassName2,"STRUT(M3210-001.3)")==0)
			{
				strcpy(sclassid,"ICM01050103");
			}
			else if(tc_strcmp(strClassName2,"BUSH,ARB(M3280-001.2)")==0)
			{
				strcpy(sclassid,"ICM01050104");
			}
			else if(tc_strcmp(strClassName2,"ANTI ROLL BAR(M3280-001.1)")==0)
			{
				strcpy(sclassid,"ICM01050105");
			}
			else if(tc_strcmp(strClassName2,"SPRING SEAT BOTTOM(M4310-001.1)")==0)
			{
				strcpy(sclassid,"ICM01050106");
			}
			else if(tc_strcmp(strClassName2,"CLAMP,ARB(M3280-001.3)")==0)
			{
				strcpy(sclassid,"ICM01050107");
			}
			else if(tc_strcmp(strClassName2,"BALL JOINT,ARB(M4310-001.3)")==0)
			{
				strcpy(sclassid,"ICM01050108");
			}
			else if(tc_strcmp(strClassName2,"SPRING SEAT TOP(M4390-001)")==0)
			{
				strcpy(sclassid,"ICM01050109");
			}
			else if(tc_strcmp(strClassName2,"THRUST BEARING(M3320-001.4")==0)
			{
				strcpy(sclassid,"ICM01050110");
			}
			else if(tc_strcmp(strClassName2,"BEARING(M3320-001.4)")==0)
			{
				strcpy(sclassid,"SAM030102");
			}
			else if(tc_strcmp(strClassName2,"FUEL TANK")==0)
			{
				strcpy(sclassid,"ICM020101");
			}
			else if(tc_strcmp(strClassName2,"BRAKE DISC(M4210-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080401");
			}
			else if(tc_strcmp(strClassName2,"CALIPER(M4210-001.2)")==0)
			{
				strcpy(sclassid,"ICM01080402");
			}
			else if(tc_strcmp(strClassName2,"DUST SHILED(M4210-001.3)")==0)
			{
				strcpy(sclassid,"ICM01080403");
			}
			else if(tc_strcmp(strClassName2,"BRAKE DRUM(M4210-001.4.1)")==0)
			{
				strcpy(sclassid,"ICM01080404");
			}
			else if(tc_strcmp(strClassName2,"PARKING BRAKE LEVER(M4270-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080405");
			}
			else if(tc_strcmp(strClassName2,"PARKING BRAKE CABLE(M4270-001.3)")==0)
			{
				strcpy(sclassid,"ICM01080406");
			}
			else if(tc_strcmp(strClassName2,"BOOSTER(M4310-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080407");
			}
			else if(tc_strcmp(strClassName2,"BRAKE ACTUATOR(M4310-001.2)")==0)
			{
				strcpy(sclassid,"ICM01080408");
			}
			else if(tc_strcmp(strClassName2,"BRAKE CHAMBER(M4310-001.3)")==0)
			{
				strcpy(sclassid,"ICM01080409");
			}
			else if(tc_strcmp(strClassName2,"AIR TANK(M4390-001.4)")==0)
			{
				strcpy(sclassid,"ICM01080410");
			}
			else if(tc_strcmp(strClassName2,"DRYING & DISTRIBUTION UNIT(M4390-001.2)")==0)
			{
				strcpy(sclassid,"ICM01080411");
			}
			else if(tc_strcmp(strClassName2,"AIR DRYER UNIT(M4390-001.3)")==0)
			{
				strcpy(sclassid,"ICM01080412");
			}
			else if(tc_strcmp(strClassName2,"LCRV/PCRV (M4370-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080413");
			}
			else if(tc_strcmp(strClassName2,"WHEEL SPEED SENSOR(M5420-002.1)")==0)
			{
				strcpy(sclassid,"ICM01080414");
			}
			else if(tc_strcmp(strClassName2,"BRAKE ECU(M1617-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080415");
			}
			else if(tc_strcmp(strClassName2,"DRUM BRAKE(M4210-001.4)")==0)
			{
				strcpy(sclassid,"ICM01080416");
			}
			else if(tc_strcmp(strClassName2,"WHEEL RIM")==0)
			{
				strcpy(sclassid,"ICM01070101");
			}
			else if(tc_strcmp(strClassName2,"WINCH")==0)
			{
				strcpy(sclassid,"ICM040101");
			}
			else if(tc_strcmp(strClassName2,"FRONT AXLE(M3310-001.1)")==0)
			{
				strcpy(sclassid,"ICM01020101");
			}
			
				
		}

//printf("\nafter 2");
	CALLAPI(ITEM_find(cItemName2,&count,&tags));
	if ( count > 0 )
	{
		fprintf(fp,"\nAlias Object found");
	
	}
	else
	{
	//	printf("\nafter 2");
		CALLAPI(ITEM_create_item(cItemName2,cItemName2,"Item",default_empty_to_A("NR"),&item,&rev));
		CALLAPI(AOM_save(item));
		CALLAPI(AOM_unlock(item));
		fprintf(fp,"\nafter create item");

		fprintf(fp,"\n Input sclassid.. %s",sclassid);
		fprintf(fp,"\n Input NoOfAttr2.. %d",NoOfAttr2);
		fprintf(fp,"\n Input cItemName2.. %s",cItemName2);
		//fprintf(fp,"\nafter create item");
	//	printf("\nafter 4");

//CALLAPI(ICS_ico_create("543882400101",theClassifiedObjectTag,"ICM01070101",0,NULL,NULL,theICOTag));
//CALLAPI(ICS_ico_create(cItemName2,theClassifiedObjectTag,"ICM01070101",16,InputIdsArr1,InputValuesArr1,theICOTag));
//CALLAPI(ICS_ico_create(cItemName2,item,"ICM01070101",16,InputIdsArr1,InputValuesArr1,theICOTag));
//CALLAPI(ICS_ico_create(cItemName2,item,sclassid,NoOfAttr2,InputIdsArr1,InputValuesArr1,theICOTag));
//ICS_ico_create(cItemName2,item,sclassid,NoOfAttr2,InputIdsArr1,InputValuesArr1,theICOTag);
//ICS_ico_create(cItemName2,item,"ICM01070101",16,InputIdsArr1,InputValuesArr1,theICOTag);// Working
ICS_ico_create(cItemName2,item,sclassid,NoOfAttr2,InputIdsArr1,InputValuesArr1,theICOTag);// Working


//CALLAPI(ICS_class_ask_tag("ICM01070101",&theClassTag));
//CALLAPI(ICS_create_classification_object(ws_object,cItemName2,theClassTag,&classification_object));
//CALLAPI(ICS_set_values_for_classification_obj(classification_object,16,InputIdsArr1,InputValuesArr1));
//ICS_API int ICS_classify_wsobject	(	tag_t 	wsObject,
//tag_t 	classificationObject	 
//)		

		//fprintf(fp,"\n after ICS_ico_create item");
		
	}
//printf("\nafter 5");
	//fclose(fp);
	fclose(fp);
		dup2(fd, fileno(stdout));
		close(fd);
		clearerr(stdout);
		fsetpos(stdout, &pos);
		

CLEANUP :
		//if(POM_logout(false))
		

		

EXIT:
        return SOAP_OK;
		
}
