/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
* Author		  : Dayanand Amdapure
* Created on	  : Nov 23, 2018
*  Module		 :   TCUA EPA Uploader
*  Code			 :   EPACreate.c
*
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes
***************************************************************************/

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
#include <assert.h>
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
int stringsize =0;
/**
 * Remove leading whitespace characters from string
 */
void trimLeading(char * str)
{
    int index, i, j;

    index = 0;

    /* Find last index of whitespace character */
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }


    if(index != 0)
    {
        /* Shit all trailing characters to its left */
        i = 0;
        while(str[i + index] != '\0')
        {
            str[i] = str[i + index];
            i++;
        }
        str[i] = '\0'; // Make sure that string is NULL terminated
    }
}
char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    printf("  last_comma [%s]",last_comma);
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);


        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
			stringsize = stringsize +1;
    		printf("stringsize [%d]",stringsize);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    printf("  stringsize1 [%d]",stringsize);

    return result;
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

static int	validate_date ( char *date , logical *date_is_valid , date_t *the_dt )
{
    int      retcode    = ITK_ok;     /* Function return code */
    date_t   dt         = NULLDATE;   /* Date structure */
    int      month      = 0;          /* Month */
    int      day        = 0;          /* Day */
    int      year       = 0;          /* Year */
    int      hour       = 0;          /* Hour */
    int      minute     = 0;          /* Minutes */
    int      second     = 0;          /* Seconds */
	int          max_char_size = 80;
    char *    Correct_date = (char *)MEM_alloc(max_char_size * sizeof(char));

	*date_is_valid = TRUE;

    /* Converts a date_t structure into the format specified  */
//    retcode = DATE_string_to_date ( date , "%d-%b-%Y %H:%M:%S" , &month , &day , &year , &hour , &minute , &second);				// working for " 21-August-2015" / ""
//    retcode = DATE_string_to_date ( date , "%m-%d-%y-%H:%M:%S" , &month , &day , &year , &hour , &minute , &second);				// working for  "07-21-2015-09:05:58"
     if(strlen (date)>10)
	     {
			Correct_date=subString(date,0,20);
			printf("  Correct_date [%s]",Correct_date);
			retcode = DATE_string_to_date ( Correct_date , "%y/%m/%d-%H:%M:%S:" , &month , &day , &year , &hour , &minute , &second);
		 }
		 else
		{
			strcpy(Correct_date,date);
			strcat(Correct_date,"-00:00:00:");
			printf("  Correct_date [%s]",Correct_date);
			retcode = DATE_string_to_date ( Correct_date , "%y/%m/%d-%H:%M:%S:" , &month , &day , &year , &hour , &minute , &second);
		}
		printf("  month [%d]",month);
		printf("  day [%d]",day);
		printf("  year [%d]",year);
		printf("  hour [%d]",hour);
		printf("  minute [%d]",minute);
		printf("  second [%d]",second);

	if ( retcode != ITK_ok )
    {
        *date_is_valid = FALSE;
	    printf("  Date is not valid [%d]",month);
    }
    else
    {
        dt.month = month;
        dt.day   = day;
        dt.year  = year;
        dt.hour  = hour;
        dt.minute= minute;
        dt.second= second;

        *the_dt = dt;
	    printf("  Date is valid [%d]",month);

    }

	printf("  ---return from validate_date.");

    return retcode;
}


extern int ITK_user_main (int argc, char ** argv )
{

    int status;

	char *  	EPAName=NULL;
	char *     AQEngineer=NULL;
	char *     AQGroupLeader=NULL;
	char *     ActionByAgency=NULL;
	char *     ActionTobeTaken=NULL;
	char *     ClosureComments=NULL;
	char *     Customer=NULL;
	char *     DateOfIntroInVC2=NULL;
	char *     DateOfIntroInVC3=NULL;
	char *     DateOfIntroInVC4=NULL;
	char *     DateOfIntroInVCE2=NULL;
	char *     DateOfIntroInVCE3=NULL;
	char *     EmailAdd=NULL;
	char *     EpaMeetinDate=NULL;
	char *    EpaPlantA=NULL;
	char *    InterchangeCodes=NULL;
	char *    MEPA_NO=NULL;
	char *    MbpaCreDate=NULL;
	char *    MbpaCretor=NULL;
	char *    MepaCreDate=NULL;
	char *    MepaCretor=NULL;
	char *    PlanningStatus=NULL;
	char *    WbsPriority=NULL;
	char *    t5AggreCutOffNo=NULL;
	char *    t5AggreCutOffNo1=NULL;
	char *    t5AppModelList=NULL;
	char * *   t5AppModelListArry=NULL;
	char * *   t5ModelListArry=NULL;
	char *    t5BomComu=NULL;
	char *    t5BreakPointDate=NULL;
	char *    t5BtchCode=NULL;
	char *    t5CTChange=NULL;
	char *    t5Category_code1=NULL;
	char * *   t5Category_code1Arry=NULL;
	char *    t5Category_code2=NULL;
	char *    t5Category_code3=NULL;
	char *    t5Category_code4=NULL;
	char * *   t5Category_code4Arry=NULL;
	char *    t5Category_code5=NULL;
	char *    t5Category_code6=NULL;
	char *    t5ChasisNo=NULL;
	char *    t5ChasisNo2=NULL;
	char *    t5ChasisNo3=NULL;
	char *    t5ChasisNo4=NULL;
	char *    t5ChasisNoE=NULL;
	char *    t5ChasisNoE2=NULL;
	char *    t5ChasisNoE3=NULL;
	char *    t5ChasisNoE4=NULL;
	char *    t5ChasisTypeList=NULL;
	char **    t5ChasisTypeListArry=NULL;
	char *    t5CommnMepaStr=NULL;
	char *    t5CostReduction=NULL;
	char *    t5DesignGroup=NULL;
	char *    t5DisposalAction1=NULL;
	char *    t5DisposalAction2=NULL;
	char *    t5DisposalAction3=NULL;
	char *    t5DisposalAction4=NULL;
	char *    t5DisposalAction5=NULL;
	char *    t5DumCompln=NULL;
	char *    t5DumEpNo=NULL;
	char *    t5ECNTstRepNo=NULL;
	char *    t5EPACategory_code1=NULL;
	char *    t5EPAClosureDate=NULL;
	char *     t5EpaClass=NULL;
	char *     t5EpaClassDate=NULL;
	char *     t5EpaFolUpInd=NULL;
	char *     t5EpaSet=NULL;
	char *     t5EpaStatus=NULL;
	char *     t5EpaTskCrDate=NULL;
	char *     t5EpaType=NULL;
	char **    t5EpaTypeArry=NULL;
	char *     t5EpaValidity=NULL;
	char *     t5FialReq=NULL;
	char *     t5Introduction_type=NULL;
	char *     t5IsActionReqbySer=NULL;
	char *     t5IsDumEp=NULL;
	char *     t5IsSubAggReq=NULL;
	char *     t5IsToolReq=NULL;
	char *     t5IsTryOutReq=NULL;
	char *     t5LogisticsRemarks=NULL;
	char *     t5LstPODate=NULL;
	char *     t5LstSmplDate=NULL;
	char *     t5MBPARemarks=NULL;
	char *     t5MEPAQADate=NULL;
	char *     t5MailAlert=NULL;
	char *     t5MbpaDesc=NULL;
	char *     t5MbpaNo=NULL;
	char *     t5MepaReason=NULL;
	char *     t5ModOfIntrod=NULL;
	char *     t5ModelList=NULL;
	char *     t5POCheckStatus=NULL;
	char *     t5POLockDate=NULL;
	char *     t5PPMRemarks=NULL;
	char *     t5PartYesNo=NULL;
	char *     t5PartialReason=NULL;
	char *     t5QACRemarks=NULL;
	char *     t5ReadinessDate=NULL;
	char *     t5ReleaseNotes=NULL;
	char *     t5RemarkAppModel=NULL;
	char *     t5RemarksBP=NULL;
	char *     t5RemarksEPAClosure=NULL;
	char *     t5RemarksMEPAQA=NULL;
	char *     t5RemarksPOLock=NULL;
	char *     t5RemarksReadiness=NULL;
	char *     t5RemarksStocks=NULL;
	char *     t5RemarksTryout=NULL;
	char *     t5RevRemarks=NULL;
	char *     t5RjVECNo=NULL;
	char *     t5RygSts=NULL;
	char *     t5SORSendDate=NULL;
	char *     t5ServAction=NULL;
	char *     t5ServActionAgg=NULL;
	char *     t5StocksSCMDate=NULL;
	char *     t5TSRemarks=NULL;
	char *     t5TargetDtIntro=NULL;
	char *     t5TargetDtIntroHis=NULL;
	char *     t5TmpEcnNo=NULL;
	char *     t5TrgtDtIntRem=NULL;
	char *     t5TryoutDate=NULL;
	char *     t5VDRemarks=NULL;
	char *     t5VQARemarks=NULL;
	char *     t5tcfAggregate=NULL;
	char *     t5tcfAggregate1=NULL;
	char **     t5tcfAggregateArry=NULL;
	char *     tentativeDateLogistic=NULL;
	char *     tentativeDateSCM=NULL;
	char *     tx0Addressee=NULL;
	char *     tx0DateReceived=NULL;
	char *     tx0DocCreateDate=NULL;
	char *     inputfile=NULL;
	char*       inputline=NULL;
	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));

  	int          n_tags_found= 0;
	int          max_char_size = 80;
	char * 	t5IsStructEPA=NULL;
	char *    EpaPlantAc = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *    InterchangeCodesC = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *    WbsPriorityC = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *    t5MailAlertC = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *    t5AppModelsC = (char *)MEM_alloc(max_char_size * sizeof(char));
	tag_t*   tags_found = NULL;
	tag_t*   EpaTag = NULL;
	tag_t*   EpaTagRev = NULL;
	FILE*      fp=NULL;


	inputfile = ITK_ask_cli_argument("-i=");

    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	ITK_CALL(ITK_auto_login( ));
   // ITK_CALL(ITK_init_module("APLloader","abc123","APLCAR"));
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Auto login .......");fflush(stdout);


	fp=fopen(inputfile,"r");
	if(fp!=NULL)
	{
		inputline=(char *) MEM_alloc(5000);
		while(fgets(inputline,5000,fp)!=NULL)
		{

			fputs(inputline,stdout);
    		printf("inputline is --->%s\n",inputline);
			EPAName=strtok(inputline,"^");  //1
			trimLeading(EPAName);
			printf("EPAName is --->%s\n",EPAName);
			attrs[0] ="item_id";
			values[0] = (char *)EPAName;
			//Querying with item id
			ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
			printf("n_tags_found  --->[%d]\n",n_tags_found);
			if(n_tags_found==0)
			{
				AQEngineer=strtok(NULL,"^");  //2
				printf("AQEngineer is --->%s\n",AQEngineer);
				AQGroupLeader=strtok(NULL,"^"); //3
				printf("AQGroupLeader is --->%s\n",AQGroupLeader);
				ActionByAgency=strtok(NULL,"^"); //4
				printf("ActionByAgency is --->%s\n",ActionByAgency);
				ActionTobeTaken=strtok(NULL,"^"); //5
				printf("ActionTobeTaken is --->%s\n",ActionTobeTaken);
				ClosureComments=strtok(NULL,"^"); //6
				printf("ClosureComments is --->%s\n",ClosureComments);
				Customer=strtok(NULL,"^"); //7
				printf("Customer is --->%s\n",Customer);
				DateOfIntroInVC2=strtok(NULL,"^"); //8
				printf("DateOfIntroInVC2 is --->%s\n",DateOfIntroInVC2);
				DateOfIntroInVC3=strtok(NULL,"^"); //9
				printf("DateOfIntroInVC3 is --->%s\n",DateOfIntroInVC3);
				DateOfIntroInVC4=strtok(NULL,"^"); //10
				printf("DateOfIntroInVC4 is --->%s\n",DateOfIntroInVC4);
				DateOfIntroInVCE2=strtok(NULL,"^"); //11
				printf("DateOfIntroInVCE2 is --->%s\n",DateOfIntroInVCE2);
				DateOfIntroInVCE3=strtok(NULL,"^"); //12
				printf("DateOfIntroInVCE3 is --->%s\n",DateOfIntroInVCE3);
				EmailAdd=strtok(NULL,"^"); //13
				printf("EmailAdd is --->%s\n",EmailAdd);
				EpaMeetinDate=strtok(NULL,"^"); //14
				printf("EpaMeetinDate is --->%s\n",EpaMeetinDate);
				EpaPlantA=strtok(NULL,"^"); //15

				printf("EpaPlantA is --->%s\n",EpaPlantA);
				if(strcmp(EpaPlantA,"P")==0)
				{
					strcpy(EpaPlantAc,"CAR");

				}else if(strcmp(EpaPlantA,"C")==0)
				{
					strcpy(EpaPlantAc,"CVBU PUNE");

				}else if(strcmp(EpaPlantA,"S")==0)
				{
					strcpy(EpaPlantAc,"SMALLCAR AHD");

				}else if(strcmp(EpaPlantA,"U")==0)
				{
					strcpy(EpaPlantAc,"SMALLCAR PNR");

				}else if(strcmp(EpaPlantA,"A")==0)
				{
					strcpy(EpaPlantAc,"PCBU and CVBU");

				}else if(strcmp(EpaPlantA,"B")==0)
				{
					strcpy(EpaPlantAc,"CVBU PNR");

				}else if(strcmp(EpaPlantA,"L")==0)
				{
					strcpy(EpaPlantAc,"PCBU LKO");

				}else if(strcmp(EpaPlantA,"G")==0)
				{
					strcpy(EpaPlantAc,"PCBU");

				}else if(strcmp(EpaPlantA,"J")==0)
				{
					strcpy(EpaPlantAc,"CVBU JSR");

				}else if(strcmp(EpaPlantA,"l")==0)
				{
					strcpy(EpaPlantAc,"CVBU LKO");

				}else if(strcmp(EpaPlantA,"D")==0)
				{
					strcpy(EpaPlantAc,"DHARWAD");
				}else
				{
					strcpy(EpaPlantAc,"TMLDL JSR");
           			}


				InterchangeCodes=strtok(NULL,"^"); //16
				printf("InterchangeCodes is --->%s\n",InterchangeCodes);
				if(strcmp(InterchangeCodes,"Non Interchangeable")==0)
				{
					strcpy(InterchangeCodesC,"N");

				}else if(strcmp(InterchangeCodes,"Two Way")==0)
				{
					strcpy(InterchangeCodesC,"X");

				}else if(strcmp(InterchangeCodes,"One Way")==0)
				{
					strcpy(InterchangeCodesC,"Y");

				}else if(strcmp(InterchangeCodes,"X")==0)
				{
					strcpy(InterchangeCodesC,"D");
				}else
				{
				strcpy(InterchangeCodesC,"A");
				}

				MEPA_NO=strtok(NULL,"^"); //17
				printf("MEPA_NO is --->%s\n",MEPA_NO);
				MbpaCreDate=strtok(NULL,"^"); //18
				printf("MbpaCreDate is --->%s\n",MbpaCreDate);
				MbpaCretor=strtok(NULL,"^"); //19
				printf("MbpaCretor is --->%s\n",MbpaCretor);
				MepaCreDate=strtok(NULL,"^"); //20
				printf("MepaCreDate is --->%s\n",MepaCreDate);
				MepaCretor=strtok(NULL,"^"); //21
				printf("MepaCretor is --->%s\n",MepaCretor);
				PlanningStatus=strtok(NULL,"^"); //22
				printf("PlanningStatus is --->%s\n",PlanningStatus);
				WbsPriority=strtok(NULL,"^"); //23
				printf("WbsPriority is --->%s\n",WbsPriority);
				if(strcmp(WbsPriority,"CcfPriority1")==0)
				{
					strcpy(WbsPriorityC,"Mid");

				}else if(strcmp(WbsPriority,"CcfPriority2")==0)
				{
					strcpy(WbsPriorityC,"Low");

				}else if(strcmp(WbsPriority,"CcfPriority3")==0)
				{
					strcpy(WbsPriorityC,"High");

				}else if(strcmp(WbsPriority,"CcfPriority4")==0)
				{
					strcpy(WbsPriorityC,"NA");
				}

				t5AggreCutOffNo=strtok(NULL,"^"); //24
				printf("t5AggreCutOffNo is --->%s\n",t5AggreCutOffNo);
				t5AggreCutOffNo1=strtok(NULL,"^"); //25
				printf("t5AggreCutOffNo1 is --->%s\n",t5AggreCutOffNo1);
				t5AppModelList=strtok(NULL,"^"); //26
				printf("t5AppModelList is --->%s\n",t5AppModelList);
				t5BomComu=strtok(NULL,"^"); //27
				printf("t5BomComu is --->%s\n",t5BomComu);
				t5BreakPointDate=strtok(NULL,"^"); //28
				printf("t5BreakPointDate is --->%s\n",t5BreakPointDate);
				t5BtchCode=strtok(NULL,"^"); //29
				printf("t5BtchCode is --->%s\n",t5BtchCode);
				t5CTChange=strtok(NULL,"^"); //30
				printf("t5CTChange is --->%s\n",t5CTChange);
				t5Category_code1=strtok(NULL,"^"); //31
				printf("t5Category_code1 is --->%s\n",t5Category_code1);
				t5Category_code2=strtok(NULL,"^"); //32
				printf("t5Category_code2 is --->%s\n",t5Category_code2);
				t5Category_code3=strtok(NULL,"^"); //33
				printf("t5Category_code3 is --->%s\n",t5Category_code3);
				t5Category_code4=strtok(NULL,"^"); //34
				printf("t5Category_code4 is --->%s\n",t5Category_code4);
				t5Category_code5=strtok(NULL,"^"); //35
				printf("t5Category_code5 is --->%s\n",t5Category_code5);
				t5Category_code6=strtok(NULL,"^"); //36
				printf("t5Category_code6 is --->%s\n",t5Category_code6);
				t5ChasisNo=strtok(NULL,"^"); //37
				printf("t5ChasisNo is --->%s\n",t5ChasisNo);
				t5ChasisNo2=strtok(NULL,"^"); //38
				printf("t5ChasisNo2 is --->%s\n",t5ChasisNo2);
				t5ChasisNo3=strtok(NULL,"^"); //39
				printf("t5ChasisNo3 is --->%s\n",t5ChasisNo3);
				t5ChasisNo4=strtok(NULL,"^"); //40
				printf("t5ChasisNo4 is --->%s\n",t5ChasisNo4);
				t5ChasisNoE=strtok(NULL,"^"); //41
				printf("t5ChasisNoE is --->%s\n",t5ChasisNoE);
				t5ChasisNoE2=strtok(NULL,"^"); //42
				printf("t5ChasisNoE2 is --->%s\n",t5ChasisNoE2);
				t5ChasisNoE3=strtok(NULL,"^"); //43
				printf("t5ChasisNoE3 is --->%s\n",t5ChasisNoE3);
				t5ChasisNoE4=strtok(NULL,"^"); //44
				printf("t5ChasisNoE4 is --->%s\n",t5ChasisNoE4);
				t5ChasisTypeList=strtok(NULL,"^"); //45
				printf("t5ChasisNoE4 is --->%s\n",t5ChasisTypeList);
				t5CommnMepaStr=strtok(NULL,"^"); //46
				printf("t5CommnMepaStr is --->%s\n",t5CommnMepaStr);
				t5CostReduction=strtok(NULL,"^"); //47
				printf("t5CostReduction is --->%s\n",t5CostReduction);
				t5DesignGroup=strtok(NULL,"^"); //48
				printf("t5DesignGroup is --->%s\n",t5DesignGroup);
				t5DisposalAction1=strtok(NULL,"^"); //49
				printf("t5DisposalAction1 is --->%s\n",t5DisposalAction1);
				t5DisposalAction2=strtok(NULL,"^"); //50
				printf("t5DisposalAction2 is --->%s\n",t5DisposalAction2);
				t5DisposalAction3=strtok(NULL,"^"); //51
				printf("t5DisposalAction3 is --->%s\n",t5DisposalAction3);
				t5DisposalAction4=strtok(NULL,"^"); //52
				printf("t5DisposalAction4 is --->%s\n",t5DisposalAction4);
				t5DisposalAction5=strtok(NULL,"^"); //53
				printf("t5DisposalAction4 is --->%s\n",t5DisposalAction5);
				t5DumCompln=strtok(NULL,"^"); //54
				printf("t5DisposalAction4 is --->%s\n",t5DumCompln);
				t5DumEpNo=strtok(NULL,"^"); //55
				printf("t5DumEpNo is --->%s\n",t5DumEpNo);
				t5ECNTstRepNo=strtok(NULL,"^"); //56
				printf("t5ECNTstRepNo is --->%s\n",t5ECNTstRepNo);
				t5EPACategory_code1=strtok(NULL,"^"); //57
				printf("t5EPACategory_code1 is --->%s\n",t5EPACategory_code1);
				t5EPAClosureDate=strtok(NULL,"^"); //58
				printf("t5EPAClosureDate is --->%s\n",t5EPAClosureDate);
				t5EpaClass=strtok(NULL,"^"); //59
				printf("t5EpaClass is --->%s\n",t5EpaClass);
				t5EpaClassDate=strtok(NULL,"^"); //60
				printf("t5EpaClassDate is --->%s\n",t5EpaClassDate);
				t5EpaFolUpInd=strtok(NULL,"^"); //61
				printf("t5EpaFolUpInd is --->%s\n",t5EpaFolUpInd);
				t5EpaSet=strtok(NULL,"^"); //62
				printf("t5EpaSet is --->%s\n",t5EpaSet);
				t5EpaStatus=strtok(NULL,"^"); //63
				printf("t5EpaStatus is --->%s\n",t5EpaStatus);
				t5EpaTskCrDate=strtok(NULL,"^"); //64
				printf("t5EpaTskCrDate is --->%s\n",t5EpaTskCrDate);
				t5EpaType=strtok(NULL,"^"); //65
				printf("t5EpaType is --->%s\n",t5EpaType);
				t5EpaValidity=strtok(NULL,"^"); //66
				printf("t5EpaValidity is --->%s\n",t5EpaValidity);
				t5FialReq=strtok(NULL,"^"); //67
				printf("t5FialReq is --->%s\n",t5FialReq);
				t5Introduction_type=strtok(NULL,"^"); //68
				printf("t5Introduction_type is --->%s\n",t5Introduction_type);
				t5IsActionReqbySer=strtok(NULL,"^"); //69
				printf("t5IsActionReqbySer is --->%s\n",t5IsActionReqbySer);
				t5IsDumEp=strtok(NULL,"^"); //70
				printf("t5IsDumEp is --->%s\n",t5IsDumEp);
				t5IsStructEPA=strtok(NULL,"^"); //71
				printf("t5IsStructEPA is --->%s\n",t5IsStructEPA);
				t5IsSubAggReq=strtok(NULL,"^"); //72
				printf("t5IsSubAggReq is --->%s\n",t5IsSubAggReq);
				t5IsToolReq=strtok(NULL,"^"); //73
				printf("t5IsToolReq is --->%s\n",t5IsToolReq);
				t5IsTryOutReq=strtok(NULL,"^"); //74
				printf("t5IsTryOutReq is --->%s\n",t5IsTryOutReq);
				t5LogisticsRemarks=strtok(NULL,"^"); //75
				printf("t5LogisticsRemarks is --->%s\n",t5LogisticsRemarks);
				t5LstPODate=strtok(NULL,"^"); //76
				printf("t5LstPODate is --->%s\n",t5LstPODate);
				t5LstSmplDate=strtok(NULL,"^"); //77
				printf("t5LstSmplDate is --->%s\n",t5LstSmplDate);
				t5MBPARemarks=strtok(NULL,"^"); //78
				printf("t5MBPARemarks is --->%s\n",t5MBPARemarks);
				t5MEPAQADate=strtok(NULL,"^"); //79
				printf("t5MEPAQADate is --->%s\n",t5MEPAQADate);
				t5MailAlert=strtok(NULL,"^"); //80
				printf("t5MailAlert is --->%s\n",t5MailAlert);
				if(strcmp(t5MailAlert,"YES")==0)
				{
					strcpy(t5MailAlertC,"Y");

				}else
				{
					strcpy(t5MailAlertC,"N");
				}
				t5MbpaDesc=strtok(NULL,"^"); //81
				printf("t5MbpaDesc is --->%s\n",t5MbpaDesc);
				t5MbpaNo=strtok(NULL,"^"); //82
				printf("t5MbpaNo is --->%s\n",t5MbpaNo);
				t5MepaReason=strtok(NULL,"^"); //83
				printf("t5MepaReason is --->%s\n",t5MepaReason);
				t5ModOfIntrod=strtok(NULL,"^"); //84
				printf("t5ModOfIntrod is --->%s\n",t5ModOfIntrod);
				t5ModelList=strtok(NULL,"^"); //85
				printf("t5ModelList is --->%s\n",t5ModelList);
				t5POCheckStatus=strtok(NULL,"^"); //86
				printf("t5POCheckStatus is --->%s\n",t5POCheckStatus);
				t5POLockDate=strtok(NULL,"^"); //87
				printf("t5POLockDate is --->%s\n",t5POLockDate);
				t5PPMRemarks=strtok(NULL,"^"); //88
				printf("t5PPMRemarks is --->%s\n",t5PPMRemarks);
				t5PartYesNo=strtok(NULL,"^"); //89
				printf("t5PartYesNo is --->%s\n",t5PartYesNo);
				t5PartialReason=strtok(NULL,"^"); //90
				printf("t5PartialReason is --->%s\n",t5PartialReason);
				t5QACRemarks=strtok(NULL,"^"); //91
				printf("t5QACRemarks is --->%s\n",t5QACRemarks);
				t5ReadinessDate=strtok(NULL,"^"); //92
				printf("t5ReadinessDate is --->%s\n",t5ReadinessDate);
				t5ReleaseNotes=strtok(NULL,"^"); //93
				printf("t5ReleaseNotes is --->%s\n",t5ReleaseNotes);
				t5RemarkAppModel=strtok(NULL,"^"); //94
				printf("t5RemarkAppModel is --->%s\n",t5RemarkAppModel);
				t5RemarksBP=strtok(NULL,"^"); //95
				printf("t5RemarksBP is --->%s\n",t5RemarksBP);
				t5RemarksEPAClosure=strtok(NULL,"^"); //96
				printf("t5RemarksEPAClosure is --->%s\n",t5RemarksEPAClosure);
				t5RemarksMEPAQA=strtok(NULL,"^"); //97
				printf("t5RemarksMEPAQA is --->%s\n",t5RemarksMEPAQA);
				t5RemarksPOLock=strtok(NULL,"^"); //98
				printf("t5RemarksPOLock is --->%s\n",t5RemarksPOLock);
				t5RemarksReadiness=strtok(NULL,"^"); //99
				printf("t5RemarksReadiness is --->%s\n",t5RemarksReadiness);
				t5RemarksStocks=strtok(NULL,"^"); //100
				printf("t5RemarksStocks is --->%s\n",t5RemarksStocks);
				t5RemarksTryout=strtok(NULL,"^"); //101
				printf("t5RemarksTryout is --->%s\n",t5RemarksTryout);
				t5RevRemarks=strtok(NULL,"^"); //102
				printf("t5RevRemarks is --->%s\n",t5RevRemarks);
				t5RjVECNo=strtok(NULL,"^"); //103
				printf("t5RjVECNo is --->%s\n",t5RjVECNo);
				t5RygSts=strtok(NULL,"^"); //104
				printf("t5RygSts is --->%s\n",t5RygSts);
				t5SORSendDate=strtok(NULL,"^"); //105
				printf("t5SORSendDate is --->%s\n",t5SORSendDate);
				t5ServAction=strtok(NULL,"^"); //106
				printf("t5ServAction is --->%s\n",t5ServAction);
				t5ServActionAgg=strtok(NULL,"^"); //107
				printf("t5ServActionAgg is --->%s\n",t5ServActionAgg);
				t5StocksSCMDate=strtok(NULL,"^"); //108
				printf("t5StocksSCMDate is --->%s\n",t5StocksSCMDate);
				t5TSRemarks=strtok(NULL,"^"); //109
				printf("t5TSRemarks is --->%s\n",t5TSRemarks);
				t5TargetDtIntro=strtok(NULL,"^"); //110
				printf("t5TargetDtIntro is --->%s\n",t5TargetDtIntro);
				t5TargetDtIntroHis=strtok(NULL,"^"); //111
				printf("t5TargetDtIntroHis is --->%s\n",t5TargetDtIntroHis);
				t5TmpEcnNo=strtok(NULL,"^"); //112
				printf("t5TmpEcnNo is --->%s\n",t5TmpEcnNo);
				t5TrgtDtIntRem=strtok(NULL,"^"); //113
				printf("t5TrgtDtIntRem is --->%s\n",t5TrgtDtIntRem);
				t5TryoutDate=strtok(NULL,"^"); //114
				printf("t5TryoutDate is --->%s\n",t5TryoutDate);
				t5VDRemarks=strtok(NULL,"^"); //115
				printf("t5VDRemarks is --->%s\n",t5VDRemarks);
				t5VQARemarks=strtok(NULL,"^"); //116
				printf("t5VQARemarks is --->%s\n",t5VQARemarks);
				t5tcfAggregate=strtok(NULL,"^"); //117
				printf("t5tcfAggregate is --->%s\n",t5tcfAggregate);
				t5tcfAggregate1=strtok(NULL,"^"); //118
				printf("t5tcfAggregate1 is --->%s\n",t5tcfAggregate1);
				tentativeDateLogistic=strtok(NULL,"^"); //119
				printf("tentativeDateLogistic is --->%s\n",tentativeDateLogistic);
				tentativeDateSCM=strtok(NULL,"^"); //120
				printf("tentativeDateSCM is --->%s\n",tentativeDateSCM);
				tx0Addressee=strtok(NULL,"^"); //121
				printf("tx0Addressee is --->%s\n",tx0Addressee);
				tx0DateReceived=strtok(NULL,"^"); //122
				printf("tx0DateReceived is --->%s\n",tx0DateReceived);
				tx0DocCreateDate=strtok(NULL,"^"); //123
				printf("tx0DocCreateDate is --->%s\n",tx0DocCreateDate);

    			ITK_CALL(ITEM_create_item(EPAName,EPAName,"T5_EPA","A",&EpaTag,&EpaTagRev));
				AOM_save(EpaTagRev);
	 			AOM_save(EpaTagRev);
                 if(EpaTagRev)
				{
				 	date_t DateOfIntroInVC2_date;
				 	date_t DateOfIntroInVC4_date;
				 	date_t DateOfIntroInVCE2_date;
				 	date_t DateOfIntroInVCE3_date;
				 	date_t EpaMeetinDate_date;
				 	date_t MbpaCreDate_date;
				 	date_t MepaCreDate_date;
				 	date_t BreakPointDate_date;
				 	date_t EPAClosureDate_date;
				 	date_t EpaTskCrDate_date;
				 	date_t EpaClassDate_date;
				 	date_t LstPODate_date;
				 	date_t LstSmplDate_date;
				 	date_t MEPAQADate_date;
				 	date_t POLockDate_date;
				 	date_t ReadinessDate_date;
				 	date_t SORSendDate_date;
				 	date_t StocksSCMDate_date;
				 	date_t TargetDtIntro_date;
				 	date_t TryoutDate_date;
				 	date_t tentativeDateLogistic_date;
				 	date_t tentativeDateSCM_date;
				 	date_t tx0DateReceived_date;
				 	date_t tx0DocCreateDate_date;
                    logical date_is_valid = FALSE;
	                int retcode = ITK_ok;
					double CostReduction;


				 	ITK_CALL(AOM_lock(EpaTagRev));
					 if(strcmp(AQEngineer," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_AQEngineer",AQEngineer));
					}
					if(strcmp(AQGroupLeader," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_AQGroupLeader",AQGroupLeader));
					}
					if(strcmp(ActionByAgency," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ActionByAgency",ActionByAgency));
					}
					if(strcmp(ActionTobeTaken," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ActionTobeTaken",ActionTobeTaken));
					}
					if(strcmp(ClosureComments," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ClosureComments",ClosureComments));
					}
					if(strcmp(Customer," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_Customer",Customer));
					}
                   if(strcmp(DateOfIntroInVC2," ")!=0)
					{
						retcode = validate_date( DateOfIntroInVC2 , &date_is_valid , &DateOfIntroInVC2_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
						ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_DateOfIntroInVC2",DateOfIntroInVC2_date));
					}

                   if(strcmp(DateOfIntroInVC4," ")!=0)
					{
						retcode = validate_date( DateOfIntroInVC4 , &date_is_valid , &DateOfIntroInVC4_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_DateOfIntroInVC4",DateOfIntroInVC4_date));
					}

                   if(strcmp(DateOfIntroInVCE2," ")!=0)
					{
						retcode = validate_date( DateOfIntroInVCE2 , &date_is_valid , &DateOfIntroInVCE2_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_DateOfIntroInVCE2",DateOfIntroInVCE2_date));
					}

					if(strcmp(DateOfIntroInVCE3," ")!=0)
					{
						retcode = validate_date( DateOfIntroInVCE3 , &date_is_valid , &DateOfIntroInVCE3_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_DateOfIntroInVCE3",DateOfIntroInVCE3_date));
					}

					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_EmailAdd",EmailAdd));

					if(strcmp(EpaMeetinDate," ")!=0)
					{
						retcode = validate_date( EpaMeetinDate , &date_is_valid , &EpaMeetinDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_EpaMeetinDate",EpaMeetinDate_date));
					}

	                if(strcmp(EpaPlantA," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_EpaPlantA",EpaPlantAc));
			     	}

					if(strcmp(InterchangeCodes," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_InterchangeCodes",InterchangeCodesC));
				    }
					if(strcmp(MEPA_NO," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_Mepa_no",MEPA_NO));
					}

					if(strcmp(MbpaCreDate," ")!=0)
					{
						retcode = validate_date( MbpaCreDate , &date_is_valid , &MbpaCreDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_MbpaCreDate",MbpaCreDate_date));
					}

					if(strcmp(MbpaCretor," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_MbpaCretor",MbpaCretor));
					}

					if(strcmp(MepaCreDate," ")!=0)
					{
						retcode = validate_date( MepaCreDate , &date_is_valid , &MepaCreDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_MepaCreDate",MepaCreDate_date));
					}

					if(strcmp(MepaCretor," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_MepaCretor",MepaCretor));
					}
      				if(strcmp(PlanningStatus," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_PlanningStatus",PlanningStatus));
					}
      				if(strcmp(WbsPriority," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_WbsPriority",WbsPriorityC));
					}
      				if(strcmp(t5AggreCutOffNo," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_AggreCutOffNo",t5AggreCutOffNo));
					}
      				if(strcmp(t5AggreCutOffNo1," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_AggreCutOffNo1",t5AggreCutOffNo1));
					}
      				if(strcmp(t5AppModelList," ")!=0)
					{
					stringsize = 0;
					printf("t5AppModelList --->%s\n",t5AppModelList);
					if(strcmp(t5AppModelList,"All,")==0)
					{
					strcpy(t5AppModelsC,"Nano LX,Nano CX,Nano,AQUA QUADRAJET,AQUA SAFIRE,AQUA TDI,AURA (ABS) QUADRAJET,AURA (ABS) SAFIRE,AURA + QUADRAJET,AURA + SAFIRE,AURA QUADRAJET,AURA SAFIRE,AURA TDI,MANZA DOM LOW,MANZA DOM MID,MANZA DOM HIGH,MANZA EXPERT EURO,MANZA EXPORT NON EURO,VISTA AQUA QUADRAJET,VISTA AQUA SAFIRE,VISTA AQUA TDI,VISTA AURA 1.2 SAFIRE,VISTA AURA 1.3 QUADRAJET,VISTA AURA 1.4 SAFIRE,VISTA AURA ABS 1.2 SAFIRE,VISTA AURA ABS 1.3 QUADRAJET,VISTA AURA PLUS 1.2 SAFIRE,VISTA AURA PLUS 1.3 QUADRAJE,VISTA AURA QUADRAJET,VISTA AURA SAFIRE,VISTA AURA TDI,VISTA HIGH 1.3 QUADRAJET,VISTA HIGH 1.4 SAFIRE,VISTA IGNIS 1.3 QUADRAJET,VISTA IGNIS 1.4 SAFIRE,VISTA INI 1.4 SAFIRE,VISTA LUX 1.3 QUADRAJET,VISTA LUX 1.4 SAFIRE,VISTA MID 1.4 SAFIRE,VISTA SAFIRE,VISTA TERRA QUADRAJET,VISTA TERRA SAFIRE,VISTA TERRA TDI,");
					t5AppModelListArry = str_split(t5AppModelsC,',');
					printf("stringsize --->%d \n",stringsize);
					printf("t5AppModelList_11 --->%s\n",*t5AppModelListArry);
					ITK_CALL ( AOM_set_value_strings(EpaTagRev,"t5_AppModelList",stringsize,t5AppModelListArry));
					}else
					{
					t5AppModelListArry = str_split(t5AppModelList,',');
					printf("stringsize --->%d \n",stringsize);
					printf("t5AppModelList_12 --->%s\n",*t5AppModelListArry);
					ITK_CALL ( AOM_set_value_strings(EpaTagRev,"t5_AppModelList",stringsize,t5AppModelListArry));
					}
					}
      				if(strcmp(t5BomComu," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_BomComu",t5BomComu));
					}

					if(strcmp(t5BreakPointDate," ")!=0)
					{
						retcode = validate_date( t5BreakPointDate , &date_is_valid , &BreakPointDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_BreakPointDate",BreakPointDate_date));
					}
      				if(strcmp(t5BtchCode," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_BtchCode",t5BtchCode));
					}
					if(strcmp(t5CTChange," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_CTChange",t5CTChange));
					}
					 if(strcmp(t5Category_code1," ")!=0)
					{
					stringsize = 0;
					printf("t5Category_code1 is --->%s\n",t5Category_code1);
					t5Category_code1Arry = str_split(t5Category_code1,',');
					printf("stringsize --->%d \n",stringsize);
					printf("t5Category_code1 --->%s\n",*t5Category_code1Arry);
					ITK_CALL ( AOM_set_value_strings(EpaTagRev,"t5_Reason_of_EPA_List",stringsize,t5Category_code1Arry));
					}
					if(strcmp(t5Category_code2," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_Category_code2",t5Category_code2));
					}

					if(strcmp(t5Category_code3," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_Category_code3",t5Category_code3));
					}
					 if(strcmp(t5Category_code4," ")!=0)
					{
					stringsize = 0;
					printf("t5Category_code4 is --->%s\n",t5Category_code4);
					t5Category_code4Arry = str_split(t5Category_code4,',');
					printf("stringsize --->%d \n",stringsize);
					printf("t5Category_code4 --->%s\n",*t5Category_code4Arry);
					ITK_CALL ( AOM_set_value_strings(EpaTagRev,"t5_Factory_code1_list",stringsize,t5Category_code4Arry));
					}
					if(strcmp(t5Category_code5," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_Category_code5",t5Category_code5));
					}
					if(strcmp(t5Category_code6," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_Category_code6",t5Category_code6));
					}
					if(strcmp(t5ChasisNo," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ChasisNo",t5ChasisNo));
					}
					if(strcmp(t5ChasisNo2," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ChasisNo2",t5ChasisNo2));
					}
					if(strcmp(t5ChasisNo3," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ChasisNo3",t5ChasisNo3));
					}
					if(strcmp(t5ChasisNo4," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ChasisNo4",t5ChasisNo4));
					}
					if(strcmp(t5ChasisNoE," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ChasisNoE",t5ChasisNoE));
					}
					if(strcmp(t5ChasisNoE2," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ChasisNoE2",t5ChasisNoE2));
					}
					if(strcmp(t5ChasisNoE3," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ChasisNoE3",t5ChasisNoE3));
					}
					if(strcmp(t5ChasisNoE4," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ChasisNoE4",t5ChasisNoE4));
					}
                  	if(strcmp(t5ChasisTypeList," ")!=0)
					{
					printf("t5ChasisTypeList --->%s\n",t5ChasisTypeList);
					stringsize = 0;
					t5ChasisTypeListArry = str_split(t5ChasisTypeList,',');
					printf("stringsize --->%d \n",stringsize);
					printf("t5ChasisTypeListArry --->%s\n",*t5ChasisTypeListArry);
					ITK_CALL ( AOM_set_value_strings(EpaTagRev,"t5_ChasisTypeList",stringsize,t5ChasisTypeListArry));
					}

					if(strcmp(t5CommnMepaStr," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_CommnMepaStr",t5CommnMepaStr));
					}
   			    	if(strcmp(t5CostReduction," ")!=0)
					{
                    CostReduction = atof(t5CostReduction);
					ITK_CALL ( AOM_set_value_double(EpaTagRev,"t5_CostReduction",CostReduction));
					}
					if(strcmp(t5DesignGroup," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_DesignGroup",t5DesignGroup));
					}
					if(strcmp(t5DisposalAction1," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_DisposalAction1",t5DisposalAction1));
					}
					if(strcmp(t5DisposalAction2," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_DisposalAction2",t5DisposalAction2));
					}
					if(strcmp(t5DisposalAction3," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_DisposalAction3",t5DisposalAction3));
					}
					if(strcmp(t5DisposalAction4," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_DisposalAction4",t5DisposalAction4));
					}
					if(strcmp(t5DisposalAction5," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_DisposalAction5",t5DisposalAction5));
					}
					if(strcmp(t5DumCompln," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_DumCompln",t5DumCompln));
					}
					if(strcmp(t5DumEpNo," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_DumEpNo",t5DumEpNo));
					}
					if(strcmp(t5ECNTstRepNo," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ECNTstRepNo",t5ECNTstRepNo));
					}
					if(strcmp(t5EPACategory_code1," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_EPACategory_code1",t5EPACategory_code1));
					}
					if(strcmp(t5EPAClosureDate," ")!=0)
					{
						retcode = validate_date( t5EPAClosureDate , &date_is_valid , &EPAClosureDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_EPAClosureDate",EPAClosureDate_date));
					}
					if(strcmp(t5EpaClass," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_EpaClass",t5EpaClass));
					}
					if(strcmp(t5EpaClassDate," ")!=0)
					{
						retcode = validate_date( t5EpaClassDate , &date_is_valid , &EpaClassDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_EpaClassDate",EpaClassDate_date));
					}
					if(strcmp(t5EpaFolUpInd," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_EpaFolUpInd",t5EpaFolUpInd));
					}
					if(strcmp(t5EpaSet," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_EpaSet",t5EpaSet));
					}
					if(strcmp(t5EpaStatus," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_EpaStatus",t5EpaStatus));
					}
					if(strcmp(t5EpaTskCrDate," ")!=0)
					{
						retcode = validate_date( t5EpaTskCrDate , &date_is_valid , &EpaTskCrDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_EpaTskCrDate",EpaTskCrDate_date));
					}
					 if(strcmp(t5EpaType," ")!=0)
					{
   					stringsize = 0;
					printf("t5EpaType --->%s\n",t5EpaType);
					t5EpaTypeArry = str_split(t5EpaType,',');
					printf("stringsize --->%d \n",stringsize);
					printf("t5EpaTypeArry --->%s\n",*t5EpaTypeArry);
					ITK_CALL ( AOM_set_value_strings(EpaTagRev,"t5_EpaType_List",stringsize,t5EpaTypeArry));
					}
					if(strcmp(t5EpaValidity," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_EpaValidity",t5EpaValidity));
					}
					if(strcmp(t5FialReq," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_FialReq",t5FialReq));
					}
					if(strcmp(t5Introduction_type," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_Introduction_type",t5Introduction_type));
					}
					if(strcmp(t5IsActionReqbySer," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_IsActionReqbySer",t5IsActionReqbySer));
					}
					if(strcmp(t5IsDumEp," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_IsDumEpa",t5IsDumEp));
					}
					if(strcmp(t5IsStructEPA," ")!=0)
					{
					ITK_CALL ( AOM_set_value_logical(EpaTagRev,"t5_IsStructEPA",t5IsStructEPA));
					}
					if(strcmp(t5IsSubAggReq," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_IsSubAggReq",t5IsSubAggReq));
					}
					if(strcmp(t5IsToolReq," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_IsToolReq",t5IsToolReq));
					}
					if(strcmp(t5IsTryOutReq," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_IsTryOutReq",t5IsTryOutReq));
					}
					if(strcmp(t5LogisticsRemarks," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_LogisticsRemarks",t5LogisticsRemarks));
					}
					if(strcmp(t5LstPODate," ")!=0)
					{
						retcode = validate_date( t5LstPODate , &date_is_valid , &LstPODate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_LstPODate",LstPODate_date));
					}
					if(strcmp(t5LstSmplDate," ")!=0)
					{
						retcode = validate_date( t5LstSmplDate , &date_is_valid , &LstSmplDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_LstSmplDate",LstSmplDate_date));
					}
					if(strcmp(t5MBPARemarks," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_MBPARemarks",t5MBPARemarks));
					}
					if(strcmp(t5MEPAQADate," ")!=0)
					{
						retcode = validate_date( t5MEPAQADate , &date_is_valid , &MEPAQADate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_MEPAQADate",MEPAQADate_date));
					}
					if(strcmp(t5MailAlert," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_MailAlert",t5MailAlertC));
					}
					if(strcmp(t5MbpaDesc," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_MbpaDesc",t5MbpaDesc));
					}
					if(strcmp(t5MbpaNo," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_MbpaNo",t5MbpaNo));
					}
					if(strcmp(t5MepaReason," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_MepaReason",t5MepaReason));
					}
					if(strcmp(t5ModOfIntrod," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ModOfIntrod",t5ModOfIntrod));
					}
					 if(strcmp(t5ModelList," ")!=0)
					{
   					stringsize = 0;
					printf("t5ModelList --->%s\n",t5ModelList);
					t5ModelListArry = str_split(t5ModelList,',');
					printf("stringsize --->%d \n",stringsize);
					printf("t5ModelListArry --->%s\n",*t5ModelListArry);
					ITK_CALL ( AOM_set_value_strings(EpaTagRev,"t5_ModelList",stringsize,t5ModelListArry));
					}

					if(strcmp(t5POCheckStatus," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_POCheckStatus",t5POCheckStatus));
					}
					if(strcmp(t5POLockDate," ")!=0)
					{
						retcode = validate_date( t5POLockDate , &date_is_valid , &POLockDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_POLockDate",POLockDate_date));
					}
					if(strcmp(t5PartYesNo," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_PartYesNo",t5PartYesNo));
					}
					if(strcmp(t5PartialReason," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_PartialReason",t5PartialReason));
					}
					if(strcmp(t5QACRemarks," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_QACRemarks",t5QACRemarks));
					}
					if(strcmp(t5ReadinessDate," ")!=0)
					{
						retcode = validate_date( t5ReadinessDate , &date_is_valid , &ReadinessDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_ReadinessDate",ReadinessDate_date));
					}
					if(strcmp(t5ReleaseNotes," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ReleaseNotes",t5ReleaseNotes));
					}
					if(strcmp(t5RemarkAppModel," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RemarkAppModel",t5RemarkAppModel));
					}
					if(strcmp(t5RemarksEPAClosure," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RemarksEPAClosure",t5RemarksEPAClosure));
					}
					if(strcmp(t5RemarksMEPAQA," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RemarksMEPAQA",t5RemarksMEPAQA));
					}
					if(strcmp(t5RemarksPOLock," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RemarksPOLock",t5RemarksPOLock));
					}
					if(strcmp(t5RemarksReadiness," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RemarksReadiness",t5RemarksReadiness));
					}
					if(strcmp(t5RemarksStocks," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RemarksStocks",t5RemarksStocks));
					}
					if(strcmp(t5RemarksTryout," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RemarksTryout",t5RemarksTryout));
					}
					if(strcmp(t5RemarksTryout," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RemarksTryout",t5RemarksTryout));
					}
					if(strcmp(t5RevRemarks," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RevRemarks",t5RevRemarks));
					}
					if(strcmp(t5RjVECNo," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RjVECNo",t5RjVECNo));
					}
					if(strcmp(t5RygSts," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_RygSts",t5RygSts));
					}
					if(strcmp(t5SORSendDate," ")!=0)
					{
						retcode = validate_date( t5SORSendDate , &date_is_valid , &SORSendDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_SORSendDate",SORSendDate_date));
					}
					if(strcmp(t5ServAction," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ServAction",t5ServAction));
					}
					if(strcmp(t5ServActionAgg," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_ServActionAgg",t5ServActionAgg));
					}
					if(strcmp(t5StocksSCMDate," ")!=0)
					{
						retcode = validate_date( t5StocksSCMDate , &date_is_valid , &StocksSCMDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_StocksSCMDate",StocksSCMDate_date));
					}
					if(strcmp(t5TSRemarks," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_TSRemarks",t5TSRemarks));
					}
					if(strcmp(t5TSRemarks," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_TSRemarks",t5TSRemarks));
					}
					if(strcmp(t5TargetDtIntro," ")!=0)
					{
						retcode = validate_date( t5TargetDtIntro , &date_is_valid , &TargetDtIntro_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_TargetDtIntro",TargetDtIntro_date));
					}
					if(strcmp(t5TargetDtIntroHis," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_TargetDtIntroHis",t5TargetDtIntroHis));
					}
					if(strcmp(t5TmpEcnNo," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_TmpEcnNo",t5TmpEcnNo));
					}
					if(strcmp(t5TrgtDtIntRem," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_TrgtDtIntRem",t5TrgtDtIntRem));
					}
					if(strcmp(t5TryoutDate," ")!=0)
					{
						retcode = validate_date( t5TryoutDate , &date_is_valid , &TryoutDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_TryoutDate",TryoutDate_date));
					}
					if(strcmp(t5VQARemarks," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_VQARemarks",t5VQARemarks));
					}
					if(strcmp(t5tcfAggregate," ")!=0)
					{
   					stringsize = 0;
					printf("t5tcfAggregate --->%s\n",t5tcfAggregate);
					t5tcfAggregateArry = str_split(t5tcfAggregate,',');
					printf("stringsize --->%d \n",stringsize);
					printf("t5ModelListArry --->%s\n",*t5tcfAggregateArry);
					ITK_CALL ( AOM_set_value_strings(EpaTagRev,"t5_Aggregate_List",stringsize,t5tcfAggregateArry));
					}
					if(strcmp(t5tcfAggregate1," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_tcfAggregate1",t5tcfAggregate1));
					}
					if(strcmp(tentativeDateLogistic," ")!=0)
					{
						retcode = validate_date( tentativeDateLogistic , &date_is_valid , &tentativeDateLogistic_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_tentativeDateLogistic",tentativeDateLogistic_date));
					}
					if(strcmp(tentativeDateSCM," ")!=0)
					{
						retcode = validate_date( tentativeDateSCM , &date_is_valid , &tentativeDateSCM_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_tentativeDateSCM",tentativeDateSCM_date));
					}
					if(strcmp(tx0Addressee," ")!=0)
					{
					ITK_CALL ( AOM_set_value_string(EpaTagRev,"t5_tx0Addressee",tx0Addressee));
					}
	 				printf("tx0DateReceived  --->%s\n",tx0DateReceived);
					if(strcmp(tx0DateReceived," ")!=0)
					{
						retcode = validate_date( tx0DateReceived , &date_is_valid , &tx0DateReceived_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_tx0DateReceived",tx0DateReceived_date));
					}
					if(strcmp(tx0DocCreateDate," ")!=0)
					{
						retcode = validate_date( tx0DocCreateDate , &date_is_valid , &tx0DocCreateDate_date );
						if ( retcode != ITK_ok )
						{
							printf ( " Creation Date--error code %d\n\n", retcode); fflush(stdout);
							return EXIT_FAILURE;
						}
					ITK_CALL(AOM_set_value_date	(EpaTagRev,"t5_tx0DocCreateDate",tx0DocCreateDate_date));
					}

	 				printf("EPA  is created  --->%s\n",EPAName);
				 	ITK_CALL(AOM_save(EpaTagRev));
				 	ITK_CALL(AOM_unlock(EpaTagRev));
				}
			}
		}
	}

	ITK_CALL(POM_logout(false));
	return status;

	ITK_CALL(POM_logout(false));
	return status;
}
