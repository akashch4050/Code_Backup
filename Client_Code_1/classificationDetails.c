//==================================================
//
//  Copyright 2012 Siemens Product Lifecycle Management Software Inc. All Rights Reserved.
//
//==================================================
#include "soapH.h"
#include "classificationDetails.nsmap"
#include "tc/tc_startup.h"
#include "tc/tc_macros.h"
#include "string.h"
#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) { PrintErrorStack(); return ifail;}
#define ITEM_id_size_c   128

static int PrintErrorStack( void )
{
    int iNumErrs = 0;
	const int*      pSevLst = 0;
	const int*      pErrCdeLst = 0;
	const char**    pMsgLst = NULL;
 

    register int i = 0;

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst );
   // ffprintf(fp, stderr, "CopyTemplates Error(s): \n");
	//fprintf(fp,"CopyTemplates Error(s): \n");
    for ( i = 0; i < iNumErrs; i++ )
    {
//        ffprintf(fp, stderr, "\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
//		fprintf(fp,"\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i]);
//        ffprintf(fp, stderr, "\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
//		fprintf(fp,"\t %6d: %s\n", pErrCdeLst[i], pMsgLst[i]);
    }
    return ITK_ok;
}

extern int ITK_user_main(int argc, char *argv[])
{

	return soap_serve(soap_new()); /* call the request dispatcher */

}
;

int ns__getItemData(struct soap* soap,char *Item,struct ns__CharArray1 *aString_test)
{
        int ifail =0;
		int i=0;
		tag_t item	=NULLTAG;
		int itemRevisionCount	=0;
		int revCnt	=0;
		tag_t *revisions;
		//char itemRevisionID[ITEM_id_size_c + 1] = "";
		char *itemRevisionID;
		int* itemRev_seqId;
		char* itemRev_name;
		char* itemRev_owningUser;
		date_t last_mod_date;
		date_t creation_date;
		int 	n_revs;
		char *itemSeq;
		char *last_mod_str = NULL;
		char *creation_date_str = NULL;
		char *itemRev_desc = NULL;
		//char *subclass_name = NULL;


		//for class
		char *sclassid = NULL;
		char * 	theParent;
		int 		theType;
		char * 	theName;
		char * 	theShortName;
		char * 	theDescription;
		char *	theUserData1;
		char * 	theUserData2;
		int  		theOptions;
		int  		theInstanceCount;
		int  		theChildrenCount;
		int  		theViewCount;

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


		tag_t 	classificationObject;
		tag_t  theClass;
		char * 	id;
		char * 	name;
		int 	n_items = 0;
		FILE *fp =NULL;
		tag_t  	*itags;
		//char *Input;

		//end class api 
		fpos_t pos;
		fgetpos(stdout, &pos);
		int fd = dup(fileno(stdout));
		freopen("log1.txt", "w", stdout);
		fp =fopen("log7.txt", "w");

		fprintf(fp,"\n Calling ITK_initialize_text_services..");fflush(stdout);


		CALLAPI(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
		CALLAPI(ITK_auto_login( ));
		CALLAPI(ITK_set_journalling( TRUE ));
		fprintf(fp,"\n Login Success....");fflush(stdout);

//		Input = malloc(20);
//		strcpy(Input,"item_id=????");
//		strcat(Input,Item);
//		strcat(Input,"*");
//		fprintf(fp,"\n Input .. %s",Input);fflush(stdout);		
//		CALLAPI(ITEM_find_items_by_string	(Input,&n_items,&itags));			
	
//if(itags != NULLTAG)
//		{
//			fprintf(fp,"\n n_items .. %d",n_items);
//
//			for(i=0;i<n_items;i++)
//			{
//			CALLAPI(ITEM_list_all_revs(itags[i], &itemRevisionCount, &revisions));
//			fprintf(fp,"\n Calling Code..");
//
//			if(revisions != NULLTAG)
//			{
//				for(revCnt=0;revCnt<itemRevisionCount;revCnt++)
//				{
//					subclass_name=NULL;
//					CALLAPI(AOM_ask_value_string(revisions[revCnt], "ics_subclass_name", &subclass_name));
//					if((subclass_name != NULL ) && (tc_strcmp(subclass_name,"")!=0))
//					{
//						fprintf(fp,"\n ics_subclass_name : %s",subclass_name);
//						break;
//					}				
//
//					
//				}
//
//
//			}
//			if((subclass_name != NULL ) && (tc_strcmp(subclass_name,"")!=0))
//					{
//						fprintf(fp,"\n ics_subclass_name : %s",subclass_name);
//						break;
//					}		
//			}
//
//	}


fprintf(fp,"\n Input .. %s",Item);fflush(stdout);	
		if((Item !=NULL) && (tc_strcmp(Item,"")!=0))
		{
			sclassid = malloc(20);	
		 if(tc_strcmp(Item,"Front Spring")==0)
			{
				strcpy(sclassid,"ICM01010101");
			}
			else if(tc_strcmp(Item,"CVBU")==0)
			{
				strcpy(sclassid,"ICM01040101");
			}
			else if(tc_strcmp(Item,"Wheel Rim")==0)
			{
				strcpy(sclassid,"ICM01070101");
			}
			else if(tc_strcmp(Item,"FRONT AXLE")==0)
			{
				strcpy(sclassid,"ICM030101");
			}
			else if(tc_strcmp(Item,"Front Axle")==0)
			{
				strcpy(sclassid,"ICM01020101");
			}
			else if(tc_strcmp(Item,"Fuel Tank")==0)
			{
				strcpy(sclassid,"ICM020101");
			}
			else if(tc_strcmp(Item,"Bump Stopper")==0)
			{
				strcpy(sclassid,"ICM01050101");
			}
			else if(tc_strcmp(Item,"Spring")==0)
			{
				strcpy(sclassid,"ICM01050102");
			}
			else if(tc_strcmp(Item,"Strut")==0)
			{
				strcpy(sclassid,"ICM01050103");
			}
			else if(tc_strcmp(Item,"Front ARB Bush")==0)
			{
				strcpy(sclassid,"ICM01050104");
			}
			else if(tc_strcmp(Item,"Front ARB")==0)
			{
				strcpy(sclassid,"ICM01050105");
			}
			else if(tc_strcmp(Item,"Spring Seat Bottom")==0)
			{
				strcpy(sclassid,"ICM01050106");
			}
			else if(tc_strcmp(Item,"ARB Clamp")==0)
			{
				strcpy(sclassid,"ICM01050107");
			}
			else if(tc_strcmp(Item,"ARB Ball Joint")==0)
			{
				strcpy(sclassid,"ICM01050108");
			}
			else if(tc_strcmp(Item,"Spring seat Top")==0)
			{
				strcpy(sclassid,"ICM01050109");
			}
			else if(tc_strcmp(Item,"Thrust Bearing")==0)
			{
				strcpy(sclassid,"ICM01050110");
			}
			else if(tc_strcmp(Item,"Bearing")==0)
			{
				strcpy(sclassid,"ICM01050111");
			}
			else if(tc_strcmp(Item,"Fastner")==0)
			{
				strcpy(sclassid,"ICM010601");
			}
			else if(tc_strcmp(Item,"Winch")==0)
			{
				strcpy(sclassid,"ICM040101");
			}
			//******As Per New Sructure
			else if(tc_strcmp(Item,"LEAF SPRING(M3210-001.10)")==0)
			{
				strcpy(sclassid,"ICM01010101");
			}
			else if(tc_strcmp(Item,"BUMP STOPPER(M3210-001.5)")==0)
			{
				strcpy(sclassid,"ICM01050101");
			}
			else if(tc_strcmp(Item,"COIL SPRING(M3210-001.9)")==0)
			{
				strcpy(sclassid,"ICM01050102");
			}
			else if(tc_strcmp(Item,"STRUT(M3210-001.3)")==0)
			{
				strcpy(sclassid,"ICM01050103");
			}
			else if(tc_strcmp(Item,"BUSH,ARB(M3280-001.2)")==0)
			{
				strcpy(sclassid,"ICM01050104");
			}
			else if(tc_strcmp(Item,"ANTI ROLL BAR(M3280-001.1)")==0)
			{
				strcpy(sclassid,"ICM01050105");
			}
			else if(tc_strcmp(Item,"SPRING SEAT BOTTOM(M4310-001.1)")==0)
			{
				strcpy(sclassid,"ICM01050106");
			}
			else if(tc_strcmp(Item,"CLAMP,ARB(M3280-001.3)")==0)
			{
				strcpy(sclassid,"ICM01050107");
			}
			else if(tc_strcmp(Item,"BALL JOINT,ARB(M4310-001.3)")==0)
			{
				strcpy(sclassid,"ICM01050108");
			}
			else if(tc_strcmp(Item,"SPRING SEAT TOP(M4390-001)")==0)
			{
				strcpy(sclassid,"ICM01050109");
			}
			else if(tc_strcmp(Item,"THRUST BEARING(M3320-001.4")==0)
			{
				strcpy(sclassid,"ICM01050110");
			}
			else if(tc_strcmp(Item,"BEARING(M3320-001.4)")==0)
			{
				strcpy(sclassid,"SAM030102");
			}
			else if(tc_strcmp(Item,"FUEL TANK")==0)
			{
				strcpy(sclassid,"ICM020101");
			}
			else if(tc_strcmp(Item,"BRAKE DISC(M4210-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080401");
			}
			else if(tc_strcmp(Item,"CALIPER(M4210-001.2)")==0)
			{
				strcpy(sclassid,"ICM01080402");
			}
			else if(tc_strcmp(Item,"DUST SHILED(M4210-001.3)")==0)
			{
				strcpy(sclassid,"ICM01080403");
			}
			else if(tc_strcmp(Item,"BRAKE DRUM(M4210-001.4.1)")==0)
			{
				strcpy(sclassid,"ICM01080404");
			}
			else if(tc_strcmp(Item,"PARKING BRAKE LEVER(M4270-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080405");
			}
			else if(tc_strcmp(Item,"PARKING BRAKE CABLE(M4270-001.3)")==0)
			{
				strcpy(sclassid,"ICM01080406");
			}
			else if(tc_strcmp(Item,"BOOSTER(M4310-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080407");
			}
			else if(tc_strcmp(Item,"BRAKE ACTUATOR(M4310-001.2)")==0)
			{
				strcpy(sclassid,"ICM01080408");
			}
			else if(tc_strcmp(Item,"BRAKE CHAMBER(M4310-001.3)")==0)
			{
				strcpy(sclassid,"ICM01080409");
			}
			else if(tc_strcmp(Item,"AIR TANK(M4390-001.4)")==0)
			{
				strcpy(sclassid,"ICM01080410");
			}
			else if(tc_strcmp(Item,"DRYING & DISTRIBUTION UNIT(M4390-001.2)")==0)
			{
				strcpy(sclassid,"ICM01080411");
			}
			else if(tc_strcmp(Item,"AIR DRYER UNIT(M4390-001.3)")==0)
			{
				strcpy(sclassid,"ICM01080412");
			}
			else if(tc_strcmp(Item,"LCRV/PCRV (M4370-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080413");
			}
			else if(tc_strcmp(Item,"WHEEL SPEED SENSOR(M5420-002.1)")==0)
			{
				strcpy(sclassid,"ICM01080414");
			}
			else if(tc_strcmp(Item,"BRAKE ECU(M1617-001.1)")==0)
			{
				strcpy(sclassid,"ICM01080415");
			}
			else if(tc_strcmp(Item,"DRUM BRAKE(M4210-001.4)")==0)
			{
				strcpy(sclassid,"ICM01080416");
			}
			else if(tc_strcmp(Item,"WHEEL RIM")==0)
			{
				strcpy(sclassid,"ICM01070101");
			}
			else if(tc_strcmp(Item,"WINCH")==0)
			{
				strcpy(sclassid,"ICM040101");
			}
			else if(tc_strcmp(Item,"FRONT AXLE(M3310-001.1)")==0)
			{
				strcpy(sclassid,"ICM01020101");
			}
				
		}


fprintf(fp,"\n Calling ICS_class_describe..");fflush(stdout);
//CALLAPI( ICS_class_describe("ICM01010101",&theParent,&theType,&theName,&theShortName,&theDescription,&theUserData1,&theUserData2,&theOptions,&theInstanceCount,&theChildrenCount,&theViewCount));
//if (Item!=NULL)
if (tc_strcmp(Item,"NULL")!=0 || tc_strcmp(Item,"")!=0)
	{
CALLAPI( ICS_class_describe(sclassid,&theParent,&theType,&theName,&theShortName,&theDescription,&theUserData1,&theUserData2,&theOptions,&theInstanceCount,&theChildrenCount,&theViewCount));
fprintf(fp,"\n After ICS_class_describe..\n\n");fflush(stdout);

fprintf(fp,"\n\n theParent : %s ",theParent);fflush(stdout);
fprintf(fp,"\n theType : %d ",theType);fflush(stdout);
fprintf(fp,"\n theName : %s ",theName);fflush(stdout);
fprintf(fp,"\n theShortName : %s ",theShortName);fflush(stdout);
fprintf(fp,"\n theDescription : %s ",theDescription);fflush(stdout);
fprintf(fp,"\n theUserData1 : %s ",theUserData1);fflush(stdout);
fprintf(fp,"\n theUserData2 : %s ",theUserData2);fflush(stdout);
fprintf(fp,"\n theOptions : %d ",theOptions);fflush(stdout);
fprintf(fp,"\n theInstanceCount : %d ",theInstanceCount);fflush(stdout);
fprintf(fp,"\n theChildrenCount : %d ",theChildrenCount);fflush(stdout);
fprintf(fp,"\n theViewCount : %d \n\n",theViewCount);fflush(stdout);




fprintf(fp,"\n Calling ICS_class_describe_attributes..");fflush(stdout);
//CALLAPI(ICS_class_describe_attributes("ICM01010101",
CALLAPI(ICS_class_describe_attributes(sclassid,
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
fprintf(fp,"\n After ICS_class_describe_attributes..\n");fflush(stdout);

fprintf(fp,"\n\n theCount : %d \n\n ",theCount);fflush(stdout);

			aString_test-> __size =theCount;
			aString_test->__ptrItem = malloc( (aString_test->__size + 1) * sizeof(*aString_test->__ptrItem) );
	
for(i=0;i<theCount;i++)
	{
	//fprintf(fp,"theIds %d : %d \t theNames : %s \t theShortNames : %s \t theAnnotations : %s\n",i,theIds[i],theNames[i],theShortNames[i],theAnnotations[i]);fflush(stdout);
			fprintf(fp,"count %d  \t theIds : %d  \t theNames : %s theArraySize :%d , theDefaultValues : %s\n",i,theIds[i],theNames[i],theArraySize[i],theDefaultValues[i]);fflush(stdout);

					
					aString_test->__ptrItem[i].AttrName = malloc(30 * sizeof(char *) );
					fprintf(fp,"\nAfter AttrName mem allocate t..\n");fflush(stdout);
					
						aString_test->__ptrItem[i].AttrID = theIds[i];
						fprintf(fp,"\nAfter AttrID assingment t..\n");fflush(stdout);
						strcpy(aString_test->__ptrItem[i].AttrName,theNames[i]);
						fprintf(fp,"\nDone .\n");fflush(stdout);
	}
	}
	else
	{
		fprintf(fp,"\nI am in else t..\n");fflush(stdout);

		aString_test-> __size =1;
		aString_test->__ptrItem = malloc( (aString_test->__size + 1) * sizeof(*aString_test->__ptrItem) );
		fprintf(fp,"\nI am after 1 malloc t..\n");fflush(stdout);
	

			//fprintf(fp,"count %d  \t theIds : %d  \t theNames : %s\n",i,theIds[i],theNames[i]);fflush(stdout);

					
					//aString_test->__ptrItem[0].AttrID = malloc(1*sizeof(int) );
					fprintf(fp,"\nI am after 2 malloc t..\n");fflush(stdout);
					aString_test->__ptrItem[0].AttrName = malloc(30 * sizeof(char *) );
					fprintf(fp,"\nI am after 3 malloc t..\n");fflush(stdout);
					fprintf(fp,"\nAfter AttrName mem allocate t..\n");fflush(stdout);
					
						//aString_test->__ptrItem[i].AttrID = theIds[i];
						aString_test->__ptrItem[0].AttrID = 254;
						fprintf(fp,"\nAfter AttrID assingment t..: %d\n",aString_test->__ptrItem[0].AttrID);fflush(stdout);
						//strcpy(aString_test->__ptrItem[i].AttrName,theNames[i]);
						strcpy(aString_test->__ptrItem[0].AttrName,"Amol");
						fprintf(fp,"\nAfter AttrID name t. :%s.\n",aString_test->__ptrItem[0].AttrName);fflush(stdout);
						fprintf(fp,"\nDone .\n");fflush(stdout);
	
	}


		fflush(stdout);
		dup2(fd, fileno(stdout));
		close(fd);
		clearerr(stdout);
		fsetpos(stdout, &pos);

		return SOAP_OK;

 CLEANUP :
                return SOAP_OK;

EXIT:
                return SOAP_OK;
}



;
