/****************************************************************************************************
*  File            :   t5ListEmptyVisPdfDesRev.c
*  Created By      :   Anu Nair
*  Created On      :   01 Mar 2020
*  Purpose         :   BUG in JT/PDF Translation, Dataset DirectModel and PDF have no named references. CC List the DesignRevision
*
*****************************************************************************************************/
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
#include <pie/pie.h>
#include <ae/dataset.h>
#include <tccore/tctype.h>
#define Debug TRUE


#define CONNECT_FAIL (EMH_USER_error_base + 2)

#define WRONG_USAGE 100001
char* subString (char* mainStringf ,int fromCharf,int toCharf);

void dousage()
{
   printf("\nUSAGE: t5ListEmptyVisPdfDesRev -fromDate= -toDate= -sType= -outFile= \n");
   printf("\E.g: t5ListEmptyVisPdfDesRev -fromDate=01-May-2018 -toDate=02-May-2018 -sType=CMI2Part -outFile=out.txt \n");
   return;
}

void lower_string(char s[]) 
{
	int c = 0;

	while (s[c] != '\0') 
	{
		if (s[c] >= 'A' && s[c] <= 'Z') 
		{
			s[c] = s[c] + 32;
		}
		c++;
	}
}

char* subString (char* mainStringf ,int fromCharf,int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(3);
	for(i=0; i < toCharf; i++ )
    *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}

static int PrintErrorStack( void )
/*
*
* PURPOSE : Function to dump the ITK error stack
*
* RETURN : causes program termination. If you made it here
*          you're not coming back modified for cust.c to not call exit()
*          but to just print the error stack
*
* NOTES : This version will always return ITK_ok, which is quite strange
*           actually. But if the error reporting was "OK" then that makes
*           sense
*
*/
{
    int iNumErrs = 0;
    const int *pSevLst = NULL;
    const int *pErrCdeLst = NULL;
    const char **pMsgLst = NULL;
    register int i = 0;

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst );
    fprintf( stderr, "Error(PrintErrorStack): \n");
    for ( i = 0; i < iNumErrs; i++ )
    {
        fprintf( stderr, "\t%6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
    }
    return ITK_ok;
}

typedef struct  
{
    char partnumbers[100]; 
	char* sequences[4];
} InputFile;

int ITK_user_main(int argc, char* argv[])
{
	int status;
	int i = 0;
	int j = 0;
	int prints = 0;
	int resultCount = 0;
	int secObjCount = 0;
	int n_entries = 4;
	
	char *sDsetObj;
	char *rType;
	char *sType1;
	char *sItemId=NULL;
	char *LatestRevID=NULL;
	char *owningUserS=NULL;
	char *s;
	//char searchEntry [4][30] = {"start_date","end_date","DatasetType","JTType"};
	char searchEntry [4][30] = {"Start Date","End Date","Dataset Type","JTType"};

	//char *qry_entries[3] = {"start_last_mod_date","end_last_mod_date","specification_object_type"};
	char ** qry_entries = (char **) MEM_alloc(n_entries * sizeof(char *));
	char **qry_values = (char **) MEM_alloc(n_entries * sizeof(char *));
	//char **qry_values = NULL;
	
	FILE *fp = NULL;
	tag_t queryTag;
	tag_t* rev;
	tag_t* secObj;

	char type_name2[TCTYPE_name_size_c+1];

	if ((status = ITK_auto_login()) != ITK_ok)
    {
		printf("Login fail !!: Error code = %d \n\n",status);
		return 1;
	}
   
	//Getting Input Arguments
	char* usr = ITK_ask_cli_argument("-u=");/* gets user */	
	char* upw = ITK_ask_cli_argument("-p=");/* gets the password */
	if(!upw) 
	{
		char* upf = ITK_ask_cli_argument("-pf=");/* gets the password */
	}
	char* ugp = ITK_ask_cli_argument("-g=");/* what the group is */
	char* fromDate = ITK_ask_cli_argument("-fromDate=");/* what is the PartNumber */
	char* toDate = ITK_ask_cli_argument("-toDate=");/* what the REV is */
	char* sType = ITK_ask_cli_argument("-cadType=");/* what the SEQ is */
	//char* rType = ITK_ask_cli_argument("-rType=");/* what the SEQ is */
	char* outFile = ITK_ask_cli_argument("-outFile=");/* Input file name is */

	printf("\nStarting Program -- t5ListEmptyVisPdfDesRev :: %s %s %s %s\n", fromDate , toDate, sType, outFile);fflush(stdout);
	//If input is missing prompt at command line the usage. 
	/*if (( !usr) || (!upw) || (!ugp) || (!fromDate) || (!toDate) || (!sType) || (!rType) || (!outFile))	    
	{
		dousage();
		return WRONG_USAGE ;
	}*/
	if ((!fromDate) || (!toDate) || (!sType) ||  (!outFile))	    
	{
		dousage();
		return WRONG_USAGE ;
	}

	if(status =ITK_ok)
	{
	   printf("\n LOGIN SUCCESSFUL\n");fflush(stdout);
	}

	if (status != ITK_ok)
	{
	  EMH_ask_error_text( status, &s);
	  printf("\n*** Error with ITK_init_module: %s ***\n",s);fflush(stdout);
	  MEM_free(s);
	  return status;
	}

	printf("\n From Date : %s ***\n",fromDate);
	printf("\n To Date : %s ***\n",toDate);
	printf("\n Dataset Type : %s ***\n",sType);
	if(tc_strcmp(sType,"Component") == 0)
	{
		rType ="DirectModel";
		sType1 ="CMI2Part";
		sDsetObj="NOJT";
	}

	if(tc_strcmp(sType,"Assembly") ==0 )
	{
		rType ="DirectModel";
		sType1 ="CMI2Product";
		sDsetObj="NOJT";
	}

	if(tc_strcmp(sType,"Drawing") == 0 ) 
	{
		rType ="PDF";
		sType1 ="CMI2Drawing";
		sDsetObj="NOPDF";
	}
	//printf("\n Visualization Type : %s ***\n",rType);
	printf("\n Output FileName : %s ***\n",outFile);fflush(stdout);
	
	//Find QUery
	status = QRY_find("__tm_QueryEmptyNamedRefInRendBasedOnSpecRelation", &queryTag);
	if (status != ITK_ok)
	{
	  EMH_ask_error_text( status, &s);
	  printf("\n*** Error with Finding Query ***\n");fflush(stdout);
	  MEM_free(s);
	  return status;
	}	

	if (queryTag)
	{
		printf("Found Query \n"); fflush(stdout);
	}
	else
	{
		printf("*** Query Not Found *** \n"); fflush(stdout);
		return status;
	}

	//qry_values[0] = User_id;
	//qry_values[0] = "24-Jan-2018 12:50";
	//qry_values[0] = fromDate;
	//qry_values[1] = "24-Aug-2018 02:50";
	//qry_values[1] = toDate;
	//qry_values[2] = "CMI2Product";
	qry_entries[0] = (char *)MEM_alloc(strlen(searchEntry[0] ) + 1);
	qry_entries[1] = (char *)MEM_alloc(strlen(searchEntry[0] ) + 1);
	qry_entries[2] = (char *)MEM_alloc(strlen(searchEntry[0] ) + 1);
	qry_entries[3] = (char *)MEM_alloc(strlen(searchEntry[0] ) + 1);
	strcpy(qry_entries[0], searchEntry[0]);
	strcpy(qry_entries[1], searchEntry[1]);
	strcpy(qry_entries[2], searchEntry[2]);
	strcpy(qry_entries[3], searchEntry[3]);

	qry_values[0] = (char *)MEM_alloc(strlen(fromDate ) + 1);
	qry_values[1] = (char *)MEM_alloc(strlen(toDate ) + 1);
	qry_values[2] = (char *)MEM_alloc(strlen(sType ) + 1);
	qry_values[3] = (char *)MEM_alloc(strlen(sType ) + 1);
	tc_strcpy(qry_values[0], fromDate);
	tc_strcpy(qry_values[1], toDate);
	tc_strcpy(qry_values[2], sType1);
	tc_strcpy(qry_values[3], rType);
	
	
	//printf("\nNo. of entries in query are -- %d\n", n_entries);fflush(stdout);	
	QRY_set_name_mode(queryTag,false);
	//Execute Query
	status = QRY_execute(queryTag, n_entries, qry_entries, qry_values, &resultCount, &rev);
	
	printf("\nNo. of Design Revision Object Found are -- %d\n", resultCount);fflush(stdout);

	if(resultCount > 0)
	{		
		if(!fp) fp = fopen(outFile,"w");
		//Looping
		for (i=0;i<resultCount ; i++)
		{
			AOM_ask_value_string(rev[i],"item_id",&sItemId);
			AOM_ask_value_string(rev[i],"item_revision_id",&LatestRevID);
			AOM_ask_value_string(rev[i],"item_revision_id",&LatestRevID);
			AOM_ask_value_string(rev[i],"owning_user",&owningUserS);
			fprintf(fp,"%s,%s,%s,%s,\n",sItemId,LatestRevID,sDsetObj,owningUserS);fflush(fp);				
			//printf("%s,%s,%s\n",sItemId,LatestRevID,sDsetObj);fflush(stdout);				
		}
		
		if(fp) fclose(fp);
		MEM_free(rev);	
	}
	
	printf("\nExiting Program -- t5ListEmptyVisPdfDesRev\n");fflush(stdout);
	return status;		 
}