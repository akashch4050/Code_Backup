/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  Author		 :   Deepti Meshram
*  Module		 :   TCUA Desing Rev Uploader
*  Code			 :   APLPartCreate.c
*  Created on	 :   March 28, 2018
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
#include <tccore/libtccore_exports.h>

#define ONE "1-Mtr"
#define TWO "2-Kg"
#define THREE "3-Ltr"
#define FOUR "4-Nos"
#define FIVE "5-Sq.Mtr"
#define SIX "6-Sets"
#define SEVEN "7-Tonne"
#define EIGHT "8-Cu.Mtr"
#define NINE "9-Thsnds"
#define EIGHT "8-Cu.Mtr"

#define ERCREVIEW	"T5_LcsReview"
#define ERCWORKING	"T5_LcsWorking"
#define ERCRELEASED "T5_LcsErcRlzd"
#define APLWORKING  "T5_LcsAPLWrkg"
#define APLRELEASED "T5_LcsAplRlzd"
#define STDWORKING  "T5_LcsSTDWrkg"
#define STDRELEASED "T5_LcsStdRlzd"

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return


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



static char* default_empty_to_A(char *s)
{
    return (NULL == s ? s : ('\0' == s[0] ? "A" : s));
}

 char* subString (char* mainStringf ,int fromCharf,int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(200);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}

/*
int findLastIndex(string &str, char x) 
{ 
    int index = -1; 
    for (int i = 0; i < str.length(); i++) 
        if (str[i] == x) 
            index = i; 
    return index; 
} 
*/


static int report_error(char *file, int line, char *call, int status, logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0;
        int const    *severities = NULL,
            *statuses = NULL;
        const char
            **messages;

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        if (n_errors > 0)
        {
            printf("\n%s\n", messages[n_errors-1]);
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            printf("\n%s\n", error_message_string);
        }

        printf("error %d at line %d in %s\n", status, line, file);
        printf("%s\n", call);

        if (exit_on_error)
        {
            printf("\nExiting program!\n");
            exit (status);
        }
    }
    return status;
}

void  getMonth( int m ,char cMonth[30])
{
      if(m==1)
	  {
		tc_strcpy(cMonth,"Jan");
	  }
	  else if(m==2)
	  {
		tc_strcpy(cMonth,"Feb");
	  }
	  else if(m==3)
	  {
		tc_strcpy(cMonth,"Mar");
	  }
	  else if(m==4)
	  {
		tc_strcpy(cMonth,"Apr");
	  }
	  else if(m==5)
	  {
		tc_strcpy(cMonth,"May");
	  }
	  else if(m==6)
	  {
		tc_strcpy(cMonth,"Jun");
	  }
	  else if(m==7)
	  {
		tc_strcpy(cMonth,"Jul");
	  }
	  else if(m==8)
	  {
		tc_strcpy(cMonth,"Aug");
	  }
	  else if(m==9)
	  {
		tc_strcpy(cMonth,"Sep");
	  }
	  else if(m==10)
	  {
		tc_strcpy(cMonth,"Oct");
	  }
	  else if(m==11)
	  {
		tc_strcpy(cMonth,"Nov");
	  }
	  else if(m==12)
	  {
		tc_strcpy(cMonth,"Dec");
	  }
}

static int validate_date ( char *date , logical *date_is_valid , date_t *the_dt )
{
    int      retcode    = ITK_ok;     /* Function return code */
    date_t   dt         = NULLDATE;   /* Date structure */
    int      month      = 0;          /* Month */
    int      day        = 0;          /* Day */
    int      year       = 0;          /* Year */
    int      hour       = 0;          /* Hour */
    int      minute     = 0;          /* Minutes */
    int      second     = 0;          /* Seconds */

    *date_is_valid = TRUE;

    /* Converts a date_t structure into the format specified  */
    retcode = DATE_string_to_date ( date , "%d-%b-%Y %H:%M:%S" , &month , &day ,
                                    &year , &hour , &minute , &second);
    if ( retcode != ITK_ok )
    {
        *date_is_valid = FALSE;
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
    }

    return retcode;
}

int convertDate(char* date,char* rDate)
{
	int status;

}


int days( int m, int y )
{
       if ( m < 1 || m > 12 ) return 0;
       switch ( m ) {
       case 1: return 31;
       case 2: return 28 + ( (y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0) );
       }
   return ( m*153 + 156 ) / 5 - ( m*153 + 3 ) / 5;
}


void getNextDate(char *DatetoConvert,char cnextAccessDate[30])
{
	int day, month, year, nd, nm, ny, ndays;
	int		ch1;
	int		Monthas;
	int		ch;
	time_t	temp;
    date_t DayTommorow ;
 	struct tm *timeptr;
	struct tm *timeptr1;
	char pAccessDate[20];
	char pAccessDate1[20];

	char*	strDay;
	char*	strMon;
	char*	strYear;
	char	DateS[20];
	char* cMonth ;
	char CCMonth[30];
	temp = time(NULL);
	tc_strcpy(pAccessDate,"");
//	timeptr = (struct tm *)localtime(&temp);
//	ch = strftime(pAccessDate,sizeof(pAccessDate)-1,"%d-%b-%Y %H:%M",timeptr);

//	day=timeptr->tm_mday;
//	month=(timeptr->tm_mon)+1;
//	year=(timeptr->tm_year+1900);

    strYear =  strtok(DatetoConvert,"/"); //1
    cMonth  =  strtok(NULL,"'/'"); //2
    strDay  =  strtok(NULL,"'/'"); //3
    Monthas =  atoi(cMonth);
	getMonth(Monthas,CCMonth);

	printf("\n strYear:%s\n",strYear);
	printf("\n CCMonth:%s\n",CCMonth);
	printf("\n strDay:%s\n",strDay);

	tc_strcpy(cnextAccessDate,strDay);
	tc_strcat(cnextAccessDate,"-");
	tc_strcat(cnextAccessDate,CCMonth);
	tc_strcat(cnextAccessDate,"-");
	tc_strcat(cnextAccessDate,strYear);
	tc_strcat(cnextAccessDate," ");
	tc_strcat(cnextAccessDate,"00:00");
	printf("\n cnextAccessDate:%s\n",cnextAccessDate);
}

char * StringTrim(char * *pointerToString)
{
    int start=0, length=0;

        // Trim.Start:
        length = strlen(*pointerToString);
        while ((*pointerToString)[start]==' ') start++;
        (*pointerToString) += start;

        if (start < length) // Required for empty (ex. "    ") input
        {
            // Trim.End:
            int end = strlen(*pointerToString)-1; // Get string length again (after Trim.Start)
            while ((*pointerToString)[end]==' ') end--;
            (*pointerToString)[end+1] = 0;
        }

    return *pointerToString;
}


int setAttributesOnTryoutRev(tag_t* rev, char *AffshopsDup, char *AggregateDup, char *BlockDup, char *PlantDup, char *AreaDup, char *AreaCountDup ,char *BatchSizeDup, char *ChnclByDup, char *ChnclcommentsDup, char *ClnclDtDup, char *CrecommentsDup, char *CycleTimeDup, char *DaviationDup,
	char *DaviationDescDup, char *DmlNo1Dup, char *DmlNo2Dup, char *DmlNo3Dup, char *DmlNo4Dup, char *DmlNo5Dup, char *DmlNo6Dup, char *DmlNo7Dup, char *DmlNo8Dup, char *DmlNo9Dup, char *DmlNo10Dup, char *FactHeadDup, char *FeedbackDup, char *FitmentDup, char *
	IntVinNo1Dup, char *IntVinNo2Dup, char *IntVinNo3Dup, char *IntVinNo4Dup, char *IntVinNo5Dup, char *IntVinNo6Dup, char *IntVinNo7Dup, char *IntVinNo8Dup, char *IntVinNo9Dup, char *IntVinNo10Dup, char *IsDMLDup, char *IsEPADup, char *LocationDup, char *
	ManReviewerDup, char *ModPartDescDup, char *ModPartDesc1Dup, char *ModPartDesc2Dup, char *ModPartDesc3Dup, char *ModPartDesc4Dup, char *ModPartDesc6Dup, char *ModPartDesc7Dup, char *ModPartDesc8Dup, char *ModPartDesc9Dup, char *
	ModPartDesc10Dup, char *ModPartNoDup, char *ModPartNo1Dup, char *ModPartNo2Dup, char *ModPartNo3Dup, char *ModPartNo4Dup, char *TOModTypeDup, char *TOModType1Dup, char *TOModType2Dup, char *TOModType3Dup, char *TOModType4Dup, char *
	TOModType5Dup, char *TOModType6Dup, char *TOModType7Dup, char *TOModType8Dup, char *TOModType9Dup, char *PartDesc1Dup, char *PartDesc2Dup, char *PartDesc3Dup, char *PartDesc4Dup, char *PartDesc5Dup, char *PartDesc6Dup, char *PartDesc7Dup, char *
	PartDesc8Dup, char *PartDesc9Dup, char *PartDesc10Dup, char *TOPartNo1Dup, char *TOPartNo2Dup, char *TOPartNo3Dup, char *TOPartNo4Dup, char *TOPartNo5Dup, char *TOPartNo6Dup, char *TOPartNo7Dup, char *TOPartNo8Dup, char *TOPartNo9Dup, char *
	TOPartNo10Dup, char *TOPartRev1Dup, char *TOPartRev2Dup, char *TOPartRev3Dup, char *TOPartRev4Dup, char *TOPartRev5Dup, char *TOPartRev6Dup, char *TOPartRev7Dup, char *TOPartRev8Dup, char *TOPartRev9Dup, char *TOPartRev10Dup,
    char *TOPartSeq1Dup, char *TOPartSeq2Dup, char *TOPartSeq3Dup, char *TOPartSeq4Dup, char *TOPartSeq5Dup, char *TOPartSeq6Dup, char *TOPartSeq7Dup, char *TOPartSeq8Dup, char *TOPartSeq9Dup, char *TOPartSeq10Dup, char *TOPhotosDup, char *TOQAHeadDup,
	char *TOQAHeadcommentsDup, char *TOQAReviewerDup, char *TOQAcommentsDup, char *TOReasonDup, char *TORefNoDup, char *TORefNo2Dup, char *TORefNo3Dup, char *TORefNo4Dup, char *TORefNo5Dup, char *TORefNo6Dup, char *TORefNo7Dup, char *TORefNo8Dup, char *
	TORefNo9Dup, char *TORefNo10Dup, char *TOSCMReviewerDup, char *TOSTDReviewerDup, char *TOSubAreaDup, char *SubAreaCountDup , char *TOTSReviewerDup, char *TOTScommentsDup, char *TOTestDetailsDup, char *TOTryOutDtDup, char *TOVDReviewerDup, char *TOVDcommentsDup, char *
	TOVQAReviewerDup, char *TOVQAcommentsDup, char *TOVehNoDup, char *VehNoCountDup ,char *TOVenLoginDup, char *TOVenNameDup, char *TOVinNoDup, char *TOVinNo1Dup, char *TOVinNo2Dup, char *TOVinNo3Dup, char *TOVinNo4Dup, char *TOhlgreqDup, char *ESLstModByDup, char *
	CreDatestr, char *LstUpdstr, char *CrtrDup, char *LCS)
 {

	int status;
	double dweight=0;
	tag_t projobj=NULLTAG;
	tag_t user_tag=NULLTAG;
	char* username=NULL;
	char FrmNextDate[20]={0};
	char FrmNextDate1[20]={0};
	char CreDate[20]={0};
	char LstUpdDate[20]={0};
	date_t Release_date;
	date_t Release_date1;
	date_t CreationDt;
	date_t LstUpdDt;

printf("\t\n AffshopsDup::[%s]",AffshopsDup);fflush(stdout);
printf("\t\n AggregateDup::[%s]",AggregateDup);fflush(stdout);
printf("\t\n BlockDup::[%s]",BlockDup);fflush(stdout);
printf("\t\n PlantDup::[%s]",PlantDup);fflush(stdout);
printf("\t\n AreaDup::[%s]",AreaDup);fflush(stdout);
printf("\t\n AreaCountDup::[%s]",AreaCountDup);fflush(stdout);
printf("\t\n BatchSizeDup::[%s]",BatchSizeDup);fflush(stdout);
printf("\t\n ChnclByDup::[%s]",ChnclByDup);fflush(stdout);
printf("\t\n ChnclcommentsDup::[%s]",ChnclcommentsDup);fflush(stdout);
printf("\t\n ClnclDtDup::[%s]",ClnclDtDup);fflush(stdout);
printf("\t\n CrecommentsDup::[%s]",CrecommentsDup);fflush(stdout);
printf("\t\n CycleTimeDup::[%s]",CycleTimeDup);fflush(stdout);
printf("\t\n DaviationDup::[%s]",DaviationDup);fflush(stdout);
printf("\t\n DaviationDescDup::[%s]",DaviationDescDup);fflush(stdout);
printf("\t\n DmlNo1Dup::[%s]",DmlNo1Dup);fflush(stdout);
printf("\t\n DmlNo2Dup::[%s]",DmlNo2Dup);fflush(stdout);
printf("\t\n DmlNo3Dup::[%s]",DmlNo3Dup);fflush(stdout);
printf("\t\n DmlNo4Dup::[%s]",DmlNo4Dup);fflush(stdout);
printf("\t\n DmlNo5Dup::[%s]",DmlNo5Dup);fflush(stdout);
printf("\t\n DmlNo6Dup::[%s]",DmlNo6Dup);fflush(stdout);
printf("\t\n DmlNo7Dup::[%s]",DmlNo7Dup);fflush(stdout);
printf("\t\n DmlNo8Dup::[%s]",DmlNo8Dup);fflush(stdout);
printf("\t\n DmlNo9Dup::[%s]",DmlNo9Dup);fflush(stdout);
printf("\t\n DmlNo10Dup::[%s]",DmlNo10Dup);fflush(stdout);
printf("\t\n FactHeadDup::[%s]",FactHeadDup);fflush(stdout);
printf("\t\n FeedbackDup::[%s]",FeedbackDup);fflush(stdout);
printf("\t\n FitmentDup::[%s]",FitmentDup);fflush(stdout);
printf("\t\n IntVinNo1Dup::[%s]",IntVinNo1Dup);fflush(stdout);
printf("\t\n IntVinNo2Dup::[%s]",IntVinNo2Dup);fflush(stdout);
printf("\t\n IntVinNo3Dup::[%s]",IntVinNo3Dup);fflush(stdout);
printf("\t\n IntVinNo4Dup::[%s]",IntVinNo4Dup);fflush(stdout);
printf("\t\n IntVinNo5Dup::[%s]",IntVinNo5Dup);fflush(stdout);
printf("\t\n IntVinNo6Dup::[%s]",IntVinNo6Dup);fflush(stdout);
printf("\t\n IntVinNo7Dup::[%s]",IntVinNo7Dup);fflush(stdout);
printf("\t\n IntVinNo8Dup::[%s]",IntVinNo8Dup);fflush(stdout);
printf("\t\n IntVinNo9Dup::[%s]",IntVinNo9Dup);fflush(stdout);
printf("\t\n IntVinNo10Dup::[%s]",IntVinNo10Dup);fflush(stdout);
printf("\t\n IsDMLDup::[%s]",IsDMLDup);fflush(stdout);
printf("\t\n IsEPADup::[%s]",IsEPADup);fflush(stdout);
printf("\t\n LocationDup::[%s]",LocationDup);fflush(stdout);
printf("\t\n ManReviewerDup::[%s]",ManReviewerDup);fflush(stdout);
printf("\t\n ModPartDescDup::[%s]",ModPartDescDup);fflush(stdout);
printf("\t\n ModPartDesc1Dup::[%s]",ModPartDesc1Dup);fflush(stdout);
printf("\t\n ModPartDesc2Dup::[%s]",ModPartDesc2Dup);fflush(stdout);
printf("\t\n ModPartDesc3Dup::[%s]",ModPartDesc3Dup);fflush(stdout);
printf("\t\n ModPartDesc4Dup::[%s]",ModPartDesc4Dup);fflush(stdout);
printf("\t\n ModPartDesc6Dup::[%s]",ModPartDesc6Dup);fflush(stdout);
printf("\t\n ModPartDesc7Dup::[%s]",ModPartDesc7Dup);fflush(stdout);
printf("\t\n ModPartDesc8Dup::[%s]",ModPartDesc8Dup);fflush(stdout);
printf("\t\n ModPartDesc9Dup::[%s]",ModPartDesc9Dup);fflush(stdout);
printf("\t\n ModPartDesc10Dup::[%s]",ModPartDesc10Dup);fflush(stdout);
printf("\t\n ModPartNoDup::[%s]",ModPartNoDup);fflush(stdout);
printf("\t\n ModPartNo1Dup::[%s]",ModPartNo1Dup);fflush(stdout);
printf("\t\n ModPartNo2Dup::[%s]",ModPartNo2Dup);fflush(stdout);
printf("\t\n ModPartNo3Dup::[%s]",ModPartNo3Dup);fflush(stdout);
printf("\t\n ModPartNo4Dup::[%s]",ModPartNo4Dup);fflush(stdout);
printf("\t\n TOModTypeDup::[%s]",TOModTypeDup);fflush(stdout);
printf("\t\n TOModType1Dup::[%s]",TOModType1Dup);fflush(stdout);
printf("\t\n TOModType2Dup::[%s]",TOModType2Dup);fflush(stdout);
printf("\t\n TOModType3Dup::[%s]",TOModType3Dup);fflush(stdout);
printf("\t\n TOModType4Dup::[%s]",TOModType4Dup);fflush(stdout);
printf("\t\n TOModType5Dup::[%s]",TOModType5Dup);fflush(stdout);
printf("\t\n TOModType6Dup::[%s]",TOModType6Dup);fflush(stdout);
printf("\t\n TOModType7Dup::[%s]",TOModType7Dup);fflush(stdout);
printf("\t\n TOModType8Dup::[%s]",TOModType8Dup);fflush(stdout);
printf("\t\n TOModType9Dup::[%s]",TOModType9Dup);fflush(stdout);
printf("\t\n PartDesc1Dup::[%s]",PartDesc1Dup);fflush(stdout);
printf("\t\n PartDesc2Dup::[%s]",PartDesc2Dup);fflush(stdout);
printf("\t\n PartDesc3Dup::[%s]",PartDesc3Dup);fflush(stdout);
printf("\t\n PartDesc4Dup::[%s]",PartDesc4Dup);fflush(stdout);
printf("\t\n PartDesc5Dup::[%s]",PartDesc5Dup);fflush(stdout);
printf("\t\n PartDesc6Dup::[%s]",PartDesc6Dup);fflush(stdout);
printf("\t\n PartDesc7Dup::[%s]",PartDesc7Dup);fflush(stdout);
printf("\t\n PartDesc8Dup::[%s]",PartDesc8Dup);fflush(stdout);
printf("\t\n PartDesc9Dup::[%s]",PartDesc9Dup);fflush(stdout);
printf("\t\n PartDesc10Dup::[%s]",PartDesc10Dup);fflush(stdout);
printf("\t\n TOPartNo1Dup::[%s]",TOPartNo1Dup);fflush(stdout);
printf("\t\n TOPartNo2Dup::[%s]",TOPartNo2Dup);fflush(stdout);
printf("\t\n TOPartNo3Dup::[%s]",TOPartNo3Dup);fflush(stdout);
printf("\t\n TOPartNo4Dup::[%s]",TOPartNo4Dup);fflush(stdout);
printf("\t\n TOPartNo5Dup::[%s]",TOPartNo5Dup);fflush(stdout);
printf("\t\n TOPartNo6Dup::[%s]",TOPartNo6Dup);fflush(stdout);
printf("\t\n TOPartNo7Dup::[%s]",TOPartNo7Dup);fflush(stdout);
printf("\t\n TOPartNo8Dup::[%s]",TOPartNo8Dup);fflush(stdout);
printf("\t\n TOPartNo9Dup::[%s]",TOPartNo9Dup);fflush(stdout);
printf("\t\n TOPartNo10Dup::[%s]",TOPartNo10Dup);fflush(stdout);
printf("\t\n TOPartRev1Dup::[%s]",TOPartRev1Dup);fflush(stdout);
printf("\t\n TOPartRev2Dup::[%s]",TOPartRev2Dup);fflush(stdout);
printf("\t\n TOPartRev3Dup::[%s]",TOPartRev3Dup);fflush(stdout);
printf("\t\n TOPartRev4Dup::[%s]",TOPartRev4Dup);fflush(stdout);
printf("\t\n TOPartRev5Dup::[%s]",TOPartRev5Dup);fflush(stdout);
printf("\t\n TOPartRev6Dup::[%s]",TOPartRev6Dup);fflush(stdout);
printf("\t\n TOPartRev7Dup::[%s]",TOPartRev7Dup);fflush(stdout);
printf("\t\n TOPartRev8Dup::[%s]",TOPartRev8Dup);fflush(stdout);
printf("\t\n TOPartRev9Dup::[%s]",TOPartRev9Dup);fflush(stdout);
printf("\t\n TOPartRev10Dup::[%s]",TOPartRev10Dup);fflush(stdout);
printf("\t\n TOPartSeq1Dup::[%s]",TOPartSeq1Dup);fflush(stdout);
printf("\t\n TOPartSeq2Dup::[%s]",TOPartSeq2Dup);fflush(stdout);
printf("\t\n TOPartSeq3Dup::[%s]",TOPartSeq3Dup);fflush(stdout);
printf("\t\n TOPartSeq4Dup::[%s]",TOPartSeq4Dup);fflush(stdout);
printf("\t\n TOPartSeq5Dup::[%s]",TOPartSeq5Dup);fflush(stdout);
printf("\t\n TOPartSeq6Dup::[%s]",TOPartSeq6Dup);fflush(stdout);
printf("\t\n TOPartSeq7Dup::[%s]",TOPartSeq7Dup);fflush(stdout);
printf("\t\n TOPartSeq8Dup::[%s]",TOPartSeq8Dup);fflush(stdout);
printf("\t\n TOPartSeq9Dup::[%s]",TOPartSeq9Dup);fflush(stdout);
printf("\t\n TOPartSeq10Dup::[%s]",TOPartSeq10Dup);fflush(stdout);
printf("\t\n TOPhotosDup::[%s]",TOPhotosDup);fflush(stdout);
printf("\t\n TOQAHeadDup::[%s]",TOQAHeadDup);fflush(stdout);
printf("\t\n TOQAHeadcommentsDup::[%s]",TOQAHeadcommentsDup);fflush(stdout);
printf("\t\n TOQAReviewerDup::[%s]",TOQAReviewerDup);fflush(stdout);
printf("\t\n TOQAcommentsDup::[%s]",TOQAcommentsDup);fflush(stdout);
printf("\t\n TOReasonDup::[%s]",TOReasonDup);fflush(stdout);
printf("\t\n TORefNoDup::[%s]",TORefNoDup);fflush(stdout);
printf("\t\n TORefNo2Dup::[%s]",TORefNo2Dup);fflush(stdout);
printf("\t\n TORefNo3Dup::[%s]",TORefNo3Dup);fflush(stdout);
printf("\t\n TORefNo4Dup::[%s]",TORefNo4Dup);fflush(stdout);
printf("\t\n TORefNo5Dup::[%s]",TORefNo5Dup);fflush(stdout);
printf("\t\n TORefNo6Dup::[%s]",TORefNo6Dup);fflush(stdout);
printf("\t\n TORefNo7Dup::[%s]",TORefNo7Dup);fflush(stdout);
printf("\t\n TORefNo8Dup::[%s]",TORefNo8Dup);fflush(stdout);
printf("\t\n TORefNo9Dup::[%s]",TORefNo9Dup);fflush(stdout);
printf("\t\n TORefNo10Dup::[%s]",TORefNo10Dup);fflush(stdout);
printf("\t\n TOSCMReviewerDup::[%s]",TOSCMReviewerDup);fflush(stdout);
printf("\t\n TOSTDReviewerDup::[%s]",TOSTDReviewerDup);fflush(stdout);
printf("\t\n TOSubAreaDup::[%s]",TOSubAreaDup);fflush(stdout);
printf("\t\n SubAreaCountDup::[%s]",SubAreaCountDup);fflush(stdout);
printf("\t\n TOTSReviewerDup::[%s]",TOTSReviewerDup);fflush(stdout);
printf("\t\n TOTScommentsDup::[%s]",TOTScommentsDup);fflush(stdout);
printf("\t\n TOTestDetailsDup::[%s]",TOTestDetailsDup);fflush(stdout);
printf("\t\n TOTryOutDtDup::[%s]",TOTryOutDtDup);fflush(stdout);
printf("\t\n TOVDReviewerDup::[%s]",TOVDReviewerDup);fflush(stdout);
printf("\t\n TOVDcommentsDup::[%s]",TOVDcommentsDup);fflush(stdout);
printf("\t\n TOVQAReviewerDup::[%s]",TOVQAReviewerDup);fflush(stdout);
printf("\t\n TOVQAcommentsDup::[%s]",TOVQAcommentsDup);fflush(stdout);
printf("\t\n TOVehNoDup::[%s]",TOVehNoDup);fflush(stdout);
printf("\t\n VehNoCountDup::[%s]",VehNoCountDup);fflush(stdout);
printf("\t\n TOVenLoginDup::[%s]",TOVenLoginDup);fflush(stdout);
printf("\t\n TOVenNameDup::[%s]",TOVenNameDup);fflush(stdout);
printf("\t\n TOVinNoDup::[%s]",TOVinNoDup);fflush(stdout);
printf("\t\n TOVinNo1Dup::[%s]",TOVinNo1Dup);fflush(stdout);
printf("\t\n TOVinNo2Dup::[%s]",TOVinNo2Dup);fflush(stdout);
printf("\t\n TOVinNo3Dup::[%s]",TOVinNo3Dup);fflush(stdout);
printf("\t\n TOVinNo4Dup::[%s]",TOVinNo4Dup);fflush(stdout);
printf("\t\n TOhlgreqDup::[%s]",TOhlgreqDup);fflush(stdout);
printf("\t\n ESLstModByDup::[%s]",ESLstModByDup);fflush(stdout);
printf("\t\n CreDatestr::[%s]",CreDatestr);fflush(stdout);
printf("\t\n LstUpdstr::[%s]",LstUpdstr);fflush(stdout);
printf("\t\n CrtrDup::[%s]",CrtrDup);fflush(stdout);
printf("\t\n LCS::[%s]",LCS);fflush(stdout);

	ITK_CALL(AOM_lock(*rev));

	if(AffshopsDup!=NULL && strcmp(AffshopsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_Affshops",AffshopsDup));		//LOV

//	if(AggregateDup!=NULL && strcmp(AggregateDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_Aggregate",AggregateDup));	//LOV

	if(AggregateDup!=NULL && strcmp(AggregateDup,"")!=0)
	{
		if (tc_strstr(AggregateDup,"EN")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Aggregate","Engine"));
		}
		if (tc_strstr(AggregateDup,"TA")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Aggregate","Transaxle"));
		}
		if (tc_strstr(AggregateDup,"CH")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Aggregate","Chassis"));
		}
		if (tc_strstr(AggregateDup,"TR")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Aggregate","Trims"));
		}
		if (tc_strstr(AggregateDup,"EL")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Aggregate","Electricals"));
		}
		if (tc_strstr(AggregateDup,"BI")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Aggregate","BIW"));
		}
	}


	if(BlockDup!=NULL && strcmp(BlockDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_Block",BlockDup));			//LOV


//	if(PlantDup!=NULL && strcmp(PlantDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant",PlantDup));
	if(PlantDup!=NULL && strcmp(PlantDup,"")!=0)
	{
		if (tc_strstr(PlantDup,"P")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","PCBU"));
		}
		if (tc_strstr(PlantDup,"C")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","CVBU PUNE"));
		}
		if (tc_strstr(PlantDup,"D")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","DHARWAD"));
		}
		if (tc_strstr(PlantDup,"S")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","SMALLCAR AHD"));
		}
		if (tc_strstr(PlantDup,"U")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","CVBU PNR"));
		}
		if (tc_strstr(PlantDup,"L")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","CVBU LKO"));
		}
		if (tc_strstr(PlantDup,"J")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","CVBU JSR"));
		}
		if (tc_strstr(PlantDup,"Z")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","CAR"));
		}
		if (tc_strstr(PlantDup,"V")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_Plant","PUVBU"));
		}
	}

	if(BatchSizeDup!=NULL && strcmp(BatchSizeDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOBatchSize",BatchSizeDup));

	if(ChnclByDup!=NULL && strcmp(ChnclByDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOChnclBy",ChnclByDup));
	if(ChnclcommentsDup!=NULL && strcmp(ChnclcommentsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOChnclcomments",ChnclcommentsDup));

	//if(ClnclDtDup!=NULL && strcmp(ClnclDtDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOClnclDt",ClnclDtDup));
		if(ClnclDtDup!=NULL && strcmp(ClnclDtDup,"")!=0)
		{
			getNextDate(ClnclDtDup,FrmNextDate);
			printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);
			ITK_CALL(ITK_string_to_date(FrmNextDate, &Release_date ));
			if(FrmNextDate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"t5_TOClnclDt",Release_date));
		}


	if(CrecommentsDup!=NULL && strcmp(CrecommentsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOCrecomments",CrecommentsDup));
	if(CycleTimeDup!=NULL && strcmp(CycleTimeDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOCycleTime",CycleTimeDup));


	//if(DaviationDup!=NULL && strcmp(DaviationDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODaviation",DaviationDup));
	if(DaviationDup!=NULL && strcmp(DaviationDup,"")!=0)
	{
		if (tc_strstr(DaviationDup,"P1")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_TODaviation","Yes"));
		}
		if (tc_strstr(DaviationDup,"P2")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_TODaviation","No"));
		}
		if (tc_strstr(DaviationDup,"P3")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_TODaviation","UnderDeviation"));
		}
	}

	if(DaviationDescDup!=NULL && strcmp(DaviationDescDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODaviationDesc",DaviationDescDup));
	if(DmlNo1Dup!=NULL && strcmp(DmlNo1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo1",DmlNo1Dup));
	if(DmlNo2Dup!=NULL && strcmp(DmlNo2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo2",DmlNo2Dup));
	if(DmlNo3Dup!=NULL && strcmp(DmlNo3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo3",DmlNo3Dup));
	if(DmlNo4Dup!=NULL && strcmp(DmlNo4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo4",DmlNo4Dup));
	if(DmlNo5Dup!=NULL && strcmp(DmlNo5Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo5",DmlNo5Dup));
	if(DmlNo6Dup!=NULL && strcmp(DmlNo6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo6",DmlNo6Dup));
	if(DmlNo7Dup!=NULL && strcmp(DmlNo7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo7",DmlNo7Dup));
	if(DmlNo8Dup!=NULL && strcmp(DmlNo8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo8",DmlNo8Dup));
	if(DmlNo9Dup!=NULL && strcmp(DmlNo9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo9",DmlNo9Dup));
	if(DmlNo10Dup!=NULL && strcmp(DmlNo10Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TODmlNo10",DmlNo10Dup));

	if(FactHeadDup!=NULL && strcmp(FactHeadDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOFactHead",FactHeadDup));

	if(FeedbackDup!=NULL && strcmp(FeedbackDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOFeedback",FeedbackDup));
	if(FitmentDup!=NULL && strcmp(FitmentDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOFitment",FitmentDup));

	if(IntVinNo1Dup!=NULL && strcmp(IntVinNo1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo1",IntVinNo1Dup));
	if(IntVinNo2Dup!=NULL && strcmp(IntVinNo2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo2",IntVinNo2Dup));
	if(IntVinNo3Dup!=NULL && strcmp(IntVinNo3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo3",IntVinNo3Dup));
	if(IntVinNo4Dup!=NULL && strcmp(IntVinNo4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo4",IntVinNo4Dup));
	if(IntVinNo5Dup!=NULL && strcmp(IntVinNo5Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo5",IntVinNo5Dup));
	if(IntVinNo6Dup!=NULL && strcmp(IntVinNo6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo6",IntVinNo6Dup));
	if(IntVinNo7Dup!=NULL && strcmp(IntVinNo7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo7",IntVinNo7Dup));
	if(IntVinNo8Dup!=NULL && strcmp(IntVinNo8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo8",IntVinNo8Dup));
	if(IntVinNo9Dup!=NULL && strcmp(IntVinNo9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo9",IntVinNo9Dup));
	if(IntVinNo10Dup!=NULL && strcmp(IntVinNo10Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIntVinNo10",IntVinNo10Dup));
	if(LocationDup!=NULL && strcmp(LocationDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOLocation",LocationDup));
	if(ManReviewerDup!=NULL && strcmp(ManReviewerDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOManReviewer",ManReviewerDup));
    if(ModPartDescDup!=NULL && strcmp(ModPartDescDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc",ModPartDescDup));
	if(ModPartDesc1Dup!=NULL && strcmp(ModPartDesc1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc1",ModPartDesc1Dup));
	if(ModPartDesc2Dup!=NULL && strcmp(ModPartDesc2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc2",ModPartDesc2Dup));
	if(ModPartDesc3Dup!=NULL && strcmp(ModPartDesc3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc3",ModPartDesc3Dup));
	if(ModPartDesc4Dup!=NULL && strcmp(ModPartDesc4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc4",ModPartDesc4Dup));
	if(ModPartDesc6Dup!=NULL && strcmp(ModPartDesc6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc6",ModPartDesc6Dup));
	if(ModPartDesc7Dup!=NULL && strcmp(ModPartDesc7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc7",ModPartDesc7Dup));
	if(ModPartDesc8Dup!=NULL && strcmp(ModPartDesc8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc8",ModPartDesc8Dup));
	if(ModPartDesc9Dup!=NULL && strcmp(ModPartDesc9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc9",ModPartDesc9Dup));
	if(ModPartDesc10Dup!=NULL && strcmp(ModPartDesc10Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartDesc10",ModPartDesc10Dup));
	if(ModPartNoDup!=NULL && strcmp(ModPartNoDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartNo",ModPartNoDup));
	if(ModPartNo1Dup!=NULL && strcmp(ModPartNo1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartNo1",ModPartNo1Dup));
	if(ModPartNo2Dup!=NULL && strcmp(ModPartNo2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartNo2",ModPartNo2Dup));
	if(ModPartNo3Dup!=NULL && strcmp(ModPartNo3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartNo3",ModPartNo3Dup));
	if(ModPartNo4Dup!=NULL && strcmp(ModPartNo4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModPartNo4",ModPartNo4Dup));

	if(TOModTypeDup!=NULL && strcmp(TOModTypeDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType",TOModTypeDup));
	if(TOModType1Dup!=NULL && strcmp(TOModType1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType1",TOModType1Dup));
	if(TOModType2Dup!=NULL && strcmp(TOModType2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType2",TOModType2Dup));
	if(TOModType3Dup!=NULL && strcmp(TOModType3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType3",TOModType3Dup));
	if(TOModType4Dup!=NULL && strcmp(TOModType4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType4",TOModType4Dup));
	if(TOModType5Dup!=NULL && strcmp(TOModType5Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType5",TOModType5Dup));
	if(TOModType6Dup!=NULL && strcmp(TOModType6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType6",TOModType6Dup));
	if(TOModType7Dup!=NULL && strcmp(TOModType7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType7",TOModType7Dup));
	if(TOModType8Dup!=NULL && strcmp(TOModType8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType8",TOModType8Dup));
	if(TOModType9Dup!=NULL && strcmp(TOModType9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOModType9",TOModType9Dup));
	if(PartDesc1Dup!=NULL && strcmp(PartDesc1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc1",PartDesc1Dup));
	if(PartDesc2Dup!=NULL && strcmp(PartDesc2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc2",PartDesc2Dup));
	if(PartDesc3Dup!=NULL && strcmp(PartDesc3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc3",PartDesc3Dup));
	if(PartDesc4Dup!=NULL && strcmp(PartDesc4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc4",PartDesc4Dup));
	if(PartDesc5Dup!=NULL && strcmp(PartDesc5Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc5",PartDesc5Dup));
	if(PartDesc6Dup!=NULL && strcmp(PartDesc6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc6",PartDesc6Dup));
	if(PartDesc7Dup!=NULL && strcmp(PartDesc7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc7",PartDesc7Dup));
	if(PartDesc8Dup!=NULL && strcmp(PartDesc8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc8",PartDesc8Dup));
	if(PartDesc9Dup!=NULL && strcmp(PartDesc9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc9",PartDesc9Dup));
	if(PartDesc10Dup!=NULL && strcmp(PartDesc10Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartDesc10",PartDesc10Dup));
	if(TOPartNo1Dup!=NULL && strcmp(TOPartNo1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo1",TOPartNo1Dup));
	if(TOPartNo2Dup!=NULL && strcmp(TOPartNo2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo2",TOPartNo2Dup));
	if(TOPartNo3Dup!=NULL && strcmp(TOPartNo3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo3",TOPartNo3Dup));
	if(TOPartNo4Dup!=NULL && strcmp(TOPartNo4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo4",TOPartNo4Dup));
	if(TOPartNo5Dup!=NULL && strcmp(TOPartNo5Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo5",TOPartNo5Dup));
	if(TOPartNo6Dup!=NULL && strcmp(TOPartNo6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo6",TOPartNo6Dup));
	if(TOPartNo7Dup!=NULL && strcmp(TOPartNo7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo7",TOPartNo7Dup));
	if(TOPartNo8Dup!=NULL && strcmp(TOPartNo8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo8",TOPartNo8Dup));
	if(TOPartNo9Dup!=NULL && strcmp(TOPartNo9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo9",TOPartNo9Dup));
	if(TOPartNo10Dup!=NULL && strcmp(TOPartNo10Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartNo10",TOPartNo10Dup));
	if(TOPartRev1Dup!=NULL && strcmp(TOPartRev1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev1",TOPartRev1Dup));
	if(TOPartRev2Dup!=NULL && strcmp(TOPartRev2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev2",TOPartRev2Dup));
	if(TOPartRev3Dup!=NULL && strcmp(TOPartRev3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev3",TOPartRev3Dup));
	if(TOPartRev4Dup!=NULL && strcmp(TOPartRev4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev4",TOPartRev4Dup));
	if(TOPartRev5Dup!=NULL && strcmp(TOPartRev5Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev5",TOPartRev5Dup));
	if(TOPartRev6Dup!=NULL && strcmp(TOPartRev6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev6",TOPartRev6Dup));
	if(TOPartRev7Dup!=NULL && strcmp(TOPartRev7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev7",TOPartRev7Dup));
	if(TOPartRev8Dup!=NULL && strcmp(TOPartRev8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev8",TOPartRev8Dup));
	if(TOPartRev9Dup!=NULL && strcmp(TOPartRev9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev9",TOPartRev9Dup));
	if(TOPartRev10Dup!=NULL && strcmp(TOPartRev10Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartRev10",TOPartRev10Dup));
	if(TOPartSeq1Dup!=NULL && strcmp(TOPartSeq1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq1",TOPartSeq1Dup));
	if(TOPartSeq2Dup!=NULL && strcmp(TOPartSeq2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq2",TOPartSeq2Dup));
	if(TOPartSeq3Dup!=NULL && strcmp(TOPartSeq3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq3",TOPartSeq3Dup));
	if(TOPartSeq4Dup!=NULL && strcmp(TOPartSeq4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq4",TOPartSeq4Dup));
	if(TOPartSeq5Dup!=NULL && strcmp(TOPartSeq5Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq5",TOPartSeq5Dup));
	if(TOPartSeq6Dup!=NULL && strcmp(TOPartSeq6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq6",TOPartSeq6Dup));
	if(TOPartSeq7Dup!=NULL && strcmp(TOPartSeq7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq7",TOPartSeq7Dup));
	if(TOPartSeq8Dup!=NULL && strcmp(TOPartSeq8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq8",TOPartSeq8Dup));
	if(TOPartSeq9Dup!=NULL && strcmp(TOPartSeq9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq9",TOPartSeq9Dup));
	if(TOPartSeq10Dup!=NULL && strcmp(TOPartSeq10Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPartSeq10",TOPartSeq10Dup));
	if(TOPhotosDup!=NULL && strcmp(TOPhotosDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOPhotos",TOPhotosDup));
	if(TOQAHeadDup!=NULL && strcmp(TOQAHeadDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOQAHead",TOQAHeadDup));
	//if(TOQAHeadcommentsDup!=NULL && strcmp(TOQAHeadcommentsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOQAHeadcomments",TOQAHeadcommentsDup));
	if(TOQAReviewerDup!=NULL && strcmp(TOQAReviewerDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOQAReviewer",TOQAReviewerDup));
	if(TOQAcommentsDup!=NULL && strcmp(TOQAcommentsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOQAcomments",TOQAcommentsDup));
	if(TOReasonDup!=NULL && strcmp(TOReasonDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOReason",TOReasonDup));
	if(TORefNoDup!=NULL && strcmp(TORefNoDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo",TORefNoDup));
	if(TORefNo2Dup!=NULL && strcmp(TORefNo2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo2",TORefNo2Dup));
	if(TORefNo3Dup!=NULL && strcmp(TORefNo3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo3",TORefNo3Dup));
	if(TORefNo4Dup!=NULL && strcmp(TORefNo4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo4",TORefNo4Dup));
	if(TORefNo5Dup!=NULL && strcmp(TORefNo5Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo5",TORefNo5Dup));
	if(TORefNo6Dup!=NULL && strcmp(TORefNo6Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo6",TORefNo6Dup));
	if(TORefNo7Dup!=NULL && strcmp(TORefNo7Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo7",TORefNo7Dup));
	if(TORefNo8Dup!=NULL && strcmp(TORefNo8Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo8",TORefNo8Dup));
	if(TORefNo9Dup!=NULL && strcmp(TORefNo9Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo9",TORefNo9Dup));
	if(TORefNo10Dup!=NULL && strcmp(TORefNo10Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TORefNo10",TORefNo10Dup));
	if(TOSCMReviewerDup!=NULL && strcmp(TOSCMReviewerDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOSCMReviewer",TOSCMReviewerDup));
	if(TOSTDReviewerDup!=NULL && strcmp(TOSTDReviewerDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOSTDReviewer",TOSTDReviewerDup));

	//if(TOSubAreaDup!=NULL && strcmp(TOSubAreaDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOSubArea",TOSubAreaDup));				//Subarea name value table
	//	//if(TOVehNoDup!=NULL && strcmp(TOVehNoDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVehNo",TOVehNoDup));					//VehNo name value table
	//	//if(AreaDup!=NULL && strcmp(AreaDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOArea",AreaDup));						//Area name value table
	
	int aa=atoi(AreaCountDup);
	int bb=atoi(SubAreaCountDup);
	int cc=atoi(VehNoCountDup);

	printf("\n aa:%d",aa);fflush(stdout);
	printf("\n bb:%d",bb);fflush(stdout);
	printf("\n cc:%d",cc);fflush(stdout);

	int araecnt=0;
	for(araecnt=0;araecnt<aa;araecnt++)
	 {
		char*  AreaStrr=NULL;
		if(araecnt==0)
		 {
			  AreaStrr=strtok(AreaDup,"#");  
			ITKCALL(AOM_set_value_string_at(*rev,"t5_TOArea",araecnt,AreaStrr));
		 }
		 else
		 {
			 AreaStrr=strtok(NULL,"#");  
			ITKCALL(AOM_set_value_string_at(*rev,"t5_TOArea",araecnt,AreaStrr));
		 }
	 }


	int subaraecnt=0;
	 for(subaraecnt=0;subaraecnt<bb;subaraecnt++)
	 {
		char*  Strr=NULL;
		if(subaraecnt==0)
		 {
			  Strr=strtok(TOSubAreaDup,"#");  
			ITKCALL(AOM_set_value_string_at(*rev,"t5_TOSubArea",subaraecnt,Strr));
		 }
		 else
		 {
		    Strr=strtok(NULL,"#");  
			ITKCALL(AOM_set_value_string_at(*rev,"t5_TOSubArea",subaraecnt,Strr));
		 }
	 }


	int vehnocnt=0;
	 for(vehnocnt=0;vehnocnt<cc;vehnocnt++)
	 {
		char*  VehnoStrr=NULL;
		if(vehnocnt==0)
		 {
			  VehnoStrr=strtok(TOVehNoDup,"#");  
			ITKCALL(AOM_set_value_string_at(*rev,"t5_TOVehNo",vehnocnt,VehnoStrr));
		 }
		 else
		 {
		    VehnoStrr=strtok(NULL,"#");  
			ITKCALL(AOM_set_value_string_at(*rev,"t5_TOVehNo",vehnocnt,VehnoStrr));
		 }
	 }


	if(TOTSReviewerDup!=NULL && strcmp(TOTSReviewerDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOTSReviewer",TOTSReviewerDup));
	if(TOTScommentsDup!=NULL && strcmp(TOTScommentsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOTScomments",TOTScommentsDup));
	if(TOTestDetailsDup!=NULL && strcmp(TOTestDetailsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOTestDetails",TOTestDetailsDup));

	//if(TOTryOutDtDup!=NULL && strcmp(TOTryOutDtDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOTryOutDt",TOTryOutDtDup));				//DATE
		if(TOTryOutDtDup!=NULL && strcmp(TOTryOutDtDup,"")!=0)
		{
			getNextDate(TOTryOutDtDup,FrmNextDate1);
			printf("\n FrmNextDate1:%s",FrmNextDate1);fflush(stdout);
			ITK_CALL(ITK_string_to_date(FrmNextDate1, &Release_date1 ));
			if(FrmNextDate1!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"t5_TOTryOutDt",Release_date1));
		}


	if(TOVDReviewerDup!=NULL && strcmp(TOVDReviewerDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVDReviewer",TOVDReviewerDup));
	if(TOVDcommentsDup!=NULL && strcmp(TOVDcommentsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVDcomments",TOVDcommentsDup));
	if(TOVQAReviewerDup!=NULL && strcmp(TOVQAReviewerDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVQAReviewer",TOVQAReviewerDup));
	if(TOVQAcommentsDup!=NULL && strcmp(TOVQAcommentsDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVQAcomments",TOVQAcommentsDup));


	if(TOVenLoginDup!=NULL && strcmp(TOVenLoginDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVenLogin",TOVenLoginDup));
	if(TOVenNameDup!=NULL && strcmp(TOVenNameDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVenName",TOVenNameDup));
	if(TOVinNoDup!=NULL && strcmp(TOVinNoDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVinNo",TOVinNoDup));
	if(TOVinNo1Dup!=NULL && strcmp(TOVinNo1Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVinNo1",TOVinNo1Dup));
	if(TOVinNo2Dup!=NULL && strcmp(TOVinNo2Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVinNo2",TOVinNo2Dup));
	if(TOVinNo3Dup!=NULL && strcmp(TOVinNo3Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVinNo3",TOVinNo3Dup));
	if(TOVinNo4Dup!=NULL && strcmp(TOVinNo4Dup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOVinNo4",TOVinNo4Dup));
	if(TOhlgreqDup!=NULL && strcmp(TOhlgreqDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOhlgreq",TOhlgreqDup));

	//if(ESLstModByDup!=NULL && strcmp(ESLstModByDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEProcessEDNNo",ESLstModByDup));

	//if(CreDatestr!=NULL && strcmp(CreDatestr,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEProcessEDNNo",CreDatestr));		// Date

	//if(LstUpdstr!=NULL && strcmp(LstUpdstr,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEProcessEDNNo",LstUpdstr));   
	//if(CrtrDup!=NULL && strcmp(CrtrDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEProcessEDNNo",CrtrDup));   


	
	if(CreDatestr!=NULL && strcmp(CreDatestr,"")!=0 )
	{
		getNextDate(CreDatestr,CreDate);
		printf("\n CreDate:%s",CreDate);fflush(stdout);
		ITK_CALL(ITK_string_to_date(CreDate, &CreationDt));
		if(CreDate!=NULL) ITK_CALL ( POM_set_creation_date(*rev, CreationDt));
	}


	logical bypass = true;
		ITK_CALL( ITK_set_bypass ( bypass ));
	if(LstUpdstr!=NULL && strcmp(LstUpdstr,"")!=0 )
	{
		getNextDate(LstUpdstr,LstUpdDate);
		printf("\n LstUpdDate:%s",LstUpdDate);fflush(stdout);
		ITK_CALL(ITK_string_to_date(LstUpdDate, &LstUpdDt ));
		if(LstUpdDate!=NULL) ITK_CALL ( POM_set_modification_date(*rev, LstUpdDt));
	}
   ITK_CALL(POM_set_env_info( POM_bypass_attr_update , false, 0, 0.0, NULLTAG, "" ) ); // added later 



//if(IsDMLDup!=NULL && strcmp(IsDMLDup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIsDML",IsDMLDup));				 //boolean
//	if(IsEPADup!=NULL && strcmp(IsEPADup,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_TOIsEPA",IsEPADup));				 //boolean


	if(IsDMLDup!=NULL && strcmp(IsDMLDup,"")!=0 ) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(IsDMLDup,"+")==0)
		{
			printf("\n test5");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_TOIsDML",true));
		}
		if (tc_strcmp(IsDMLDup,"-")==0)
		{
			printf("\n test6");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_TOIsDML",false));
		}
	}


	if(IsEPADup!=NULL && strcmp(IsEPADup,"")!=0 ) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(IsEPADup,"+")==0)
		{
			printf("\n test5");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_TOIsEPA",true));
		}
		if (tc_strcmp(IsEPADup,"-")==0)
		{
			printf("\n test6");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_TOIsEPA",false));
		}
	}



	ITK_CALL(AOM_save(*rev));
	ITK_CALL(AOM_refresh(*rev,0));
	ITK_CALL(AOM_unlock(*rev));

	//setLifeCycle(&rev,LCS);

	 char* lcsToset=NULL;
 logical retain=false;
 tag_t   status2=NULLTAG;
 char   *ReleaseStatus=NULL;

 lcsToset =(char *) MEM_alloc(20 * sizeof(char *));

		if(strcmp(LCS,"LcsApproved")==0){strcpy(lcsToset,"T5_TOLcsApproved");}
		else if(strcmp(LCS,"LcsAuthorzd")==0){strcpy(lcsToset,"T5_LcsAuthorzd");}
		else if(strcmp(LCS,"LcsTOSTDAM")==0){strcpy(lcsToset,"T5_LcsTOSTDAM");}
		//else if(strcmp(LCS,"LcsSTDWrkg")==0){strcpy(lcsToset,STDRELEASED);}
		else if(strcmp(LCS,"LcsTOChncl")==0){strcpy(lcsToset,"T5_LcsTOChncl");}
		else if(strcmp(LCS,"LcsTOCQRvw")==0){strcpy(lcsToset,"T5_LcsTOCQRvw");}
		else if(strcmp(LCS,"LcsWorking")==0){strcpy(lcsToset,"T5_TOLcsWorking");}

	ITK_CALL(CR_create_release_status(lcsToset,&status2));
	ITK_CALL(AOM_ask_name(status2, &ReleaseStatus));
	printf("\n ******************* ReleaseStatus: %s\n",ReleaseStatus);fflush(stdout);
	ITK_CALL(EPM_add_release_status(status2,1,rev,retain));


	printf("\n Tryout created and properties set on it....");fflush(stdout); 


}

int setLifeCycle(tag_t* itemRev,char* lifeCycleS)
{
	/* translate "LcsAgncWrkg" to "Feedback from Agencies" on t5TOLCStateSet ; 
      translate "LcsManWork" to "Manufacturing Working" on t5TOLCStateSet ; 
      translate "LcsQARlzd" to "TCF Final QA Released" on t5TOLCStateSet ; 
      translate "LcsQAWork" to "QA Working" on t5TOLCStateSet ; 
      translate "LcsTOAbort" to "Modfn Required" on t5TOLCStateSet ; 
      translate "LcsTOCQRvw" to "CQ Review" on t5TOLCStateSet ; 
      translate "LcsTOChncl" to "TryOut Chancel" on t5TOLCStateSet ; 
      translate "LcsTOMaFRv" to "TryOut Manufacturing Review" on t5TOLCStateSet ; 
      translate "LcsTOSTDAM" to "Approved Mail" on t5TOLCStateSet ; 
      translate "LcsTOSTDRvw" to "TryOut STD Review" on t5TOLCStateSet ; 
      translate "LcsTOVDRvw" to "TryOut VD Review" on t5TOLCStateSet ; 
      translate "LcsTOVQTSRv" to "TryOut VQA & TS Review" on t5TOLCStateSet ; 
      translate "LcsTSAppr" to "TS Approved" on t5TOLCStateSet ; 
      translate "LcsTSRej" to "TS Rejected" on t5TOLCStateSet ; 
      translate "LcsTSWork" to "TS Working" on t5TOLCStateSet ; 
      translate "LcsVQAAppr" to "VQA Approved" on t5TOLCStateSet ; 
      translate "LcsVQARej" to "VQA Rejected" on t5TOLCStateSet ; 
      translate "LcsVQAWor" to "VQA Working" on t5TOLCStateSet ; 
*/
		int status;
		tag_t   status2=NULLTAG;
		logical retain=false;
		char* lcsToset=NULL;

		printf("\n inside setLifeCycle : [%s] \n ",lifeCycleS);fflush(stdout); 

		lcsToset =(char *) MEM_alloc(20 * sizeof(char *));

		if(strcmp(lifeCycleS,"LcsApproved")==0){strcpy(lcsToset,"T5_TOLcsApproved");}
		else if(strcmp(lifeCycleS,"LcsAuthorzd")==0){strcpy(lcsToset,"T5_LcsAuthorzd");}
		else if(strcmp(lifeCycleS,"LcsTOSTDAM")==0){strcpy(lcsToset,"T5_LcsTOSTDAM");}
		//else if(strcmp(lifeCycleS,"LcsSTDWrkg")==0){strcpy(lcsToset,STDRELEASED);}
		else if(strcmp(lifeCycleS,"LcsTOChncl")==0){strcpy(lcsToset,"T5_LcsTOChncl");}
		else if(strcmp(lifeCycleS,"LcsTOCQRvw")==0){strcpy(lcsToset,"T5_LcsTOCQRvw");}
		else if(strcmp(lifeCycleS,"LcsWorking")==0){strcpy(lcsToset,"T5_TOLcsWorking");}

		ITK_CALL(CR_create_release_status(lcsToset,&status2));
		ITK_CALL(EPM_add_release_status(status2,1,itemRev,retain));

		printf("\n After LCS setting:  setLifeCycle");fflush(stdout); 

}


int CreateRelTryoutToEPA(tag_t* rev,char * EPA1,char * EPA2,char * EPA3,char * EPA4,char * EPA5,char * EPA6,char * EPA7,char * EPA8,char * EPA9,char * EPA10,FILE *fperror)
{
	int status;
	int		ifail	=0;


			printf("\n EPA1: [%s]",EPA1);fflush(stdout);
			printf("\n EPA2: [%s]",EPA2);fflush(stdout);
			printf("\n EPA3: [%s]",EPA3);fflush(stdout);
			printf("\n EPA4: [%s]",EPA4);fflush(stdout);
			printf("\n EPA5: [%s]",EPA5);fflush(stdout);
			printf("\n EPA6: [%s]",EPA6);fflush(stdout);
			printf("\n EPA7: [%s]",EPA7);fflush(stdout);
			printf("\n EPA8: [%s]",EPA8);fflush(stdout);
			printf("\n EPA9: [%s]",EPA9);fflush(stdout);
			printf("\n EPA10: [%s]",EPA10);fflush(stdout);


	if(EPA1!=NULL && strcmp(EPA1,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA1,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA1 setsize : %d", DesignTags);fflush(stdout);
	// T5_Tryout_EPA_Relation
	// T5_EPA_Tryout_Relation

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA1);
			}
	}


	if(EPA2!=NULL && strcmp(EPA2,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA2,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA2 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA2);
			}
	}


	if(EPA3!=NULL && strcmp(EPA3,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA3,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA3 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA3);
			}
	}


	if(EPA4!=NULL && strcmp(EPA4,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA4,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA4 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA4);
			}
	}


	if(EPA5!=NULL && strcmp(EPA5,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA5,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA5 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA5);
			}
	}


	if(EPA6!=NULL && strcmp(EPA6,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA6,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA6 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA6);
			}
	}


	if(EPA7!=NULL && strcmp(EPA7,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA7,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA7 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA7);
			}
	}


	if(EPA8!=NULL && strcmp(EPA8,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA8,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA8 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA8);
			}
	}


	if(EPA9!=NULL && strcmp(EPA9,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA9,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA9 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA9);
			}
	}


	if(EPA10!=NULL && strcmp(EPA10,"")!=0 )
	{
			tag_t	*designclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;

			ITK_CALL(QRY_find("Item Revision...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {EPA10,"EPA Revision"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n EPA10 setsize : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Tryout_EPA_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n tryout to EPA relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EPA_Tryout_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n EPA to Tryout relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n EPA [%s] doesn't Exists, So Relation Is Not Created.\n",EPA10);
			}
	}



}



extern int ITK_user_main (int argc, char ** argv )
{

    int status;
	int org_seq_id_int=0;
	int j=0;
	int i=0;
	int		n_strings = 1;
	int		l_strings = 80;			// Arbitrary values - Please verify proper lengths for your application!
	int		tempStrLength = 80;
	int index;
	int item_revision_id_int;
	logical isCheckOut=false;
	FILE* fp=NULL;

	char* inputline=NULL;
	char* level=NULL;
	char* item_id=NULL;
	char* item_name=NULL;
	char* item_revision_id=NULL;
	char* item_sequence_id=NULL;
	char* desc=NULL;
	char* qty=NULL;
	char* dtype=NULL;
	char* atype="L";
	char *unit_of_measure="-";

	char* ModDescriptionDupS=NULL;
	char* mod_desc=NULL;
	char* DrawingNoDupS=NULL;
	char* Materialclass=NULL;
	char* DrawingIndDupS=NULL;
	char* DrawingIndS=NULL;
	char* MaterialDupS=NULL;
	char* MaterialS=NULL;
	char* MaterialInDrwDupS=NULL;
	char* MaterialInDrwS=NULL;
	char* LeftRhDupS=NULL;
	char* LeftRhS=NULL;
	char* DeignOwnUnitDupS=NULL;
	char* DeignOwnUnitS=NULL;
	char* ModelIndDupS=NULL;
	char* ModelIndS=NULL;
	char* UnitOfMeasureDupS=NULL;
	char* UnitOfMeasureS=NULL;
	char* ColourIndDupS=NULL;
	char* ColourIndS=NULL;
	char* WeightDupS=NULL;
	char* WeightS=NULL;
	char* MaterialThickNessS=NULL;
	char* DrawingNoS=NULL;
	char* projectcode=NULL;
	char* designgroup=NULL;
	char * rel_list=NULL;
	char * lifecycleS=NULL;

	// Added By Raghavendra B T

	char * t5SpareIndS=NULL;
	char * t5SpareCriteriaS=NULL;
	char * t5ReliabilityS=NULL;
	char * t5RefPartNumberS=NULL;
	char * t5RecyclabilityS=NULL;
	char * t5RecoverableS=NULL;
	char * t5PunMakeBuyIndS=NULL;
	char * t5PunIntialAgS=NULL;
	char * t5PrtCatCodeS=NULL;
	char * t5ProductS=NULL;
	char * t5PkgStdS=NULL;
	char * t5PartStatusS=NULL;
	char * t5PartPropertyS=NULL;
	char * t5PartCodeS=NULL;
	char * t5NcPartNoS=NULL;
	char * t5ListRecSparess=NULL;
	char * t5HomologationReqdS=NULL;
	char * t5HazardousContS=NULL;
	char * t5EnvelopeDimenS=NULL;
	char * t5DismantableS=NULL;
	char * t5ConfigIDS=NULL;
	char * t5ColourIDS=NULL;
	char * t5ClassificationHazS=NULL;
	char * t5CMVRCertificationS=NULL;
	char * t5SurfPrtStdS=NULL;
	char * t5SamplesToApprS=NULL;
	char * t5AsmDisposalS=NULL;
	char * t5FinDisposalInstrS=NULL;
	char * t5RPDisposalInstrS=NULL;
	char * t5SPDisposalInstrS=NULL;
	char * t5WIPDisposalInstrS=NULL;
	char * t5LastModByS=NULL;
	char * t5VerCreatorS=NULL;
	char * t5CAEDocES=NULL;
	char * t5CoatedS=NULL;
	char * t5ConvDocS=NULL;
	char * t5AplCopyOfErcRevS=NULL;
	char * t5AppCodeS=NULL;
	char * t5RqstNumS=NULL;
	char * t5RsnCodeS=NULL;
	char * t5SurfaceAreaS=NULL;
	char * t5VolumeS=NULL;
	char * t5CarIntialAgencyS=NULL;
	char * t5CarMakeBuyIndiS=NULL;
	char * t5ErcIndNameS=NULL;
	char * t5PostRelReqS=NULL;
	char * t5ItmCategoryS=NULL;
	char * t5CopReqS=NULL;
	char * t5AplInvalidateS=NULL;
	char * t5PrtValiStatusS=NULL;
	char * t5DRSubStateS=NULL;
	char * t5KnxtDocIndS=NULL;
	char * t5SimValS=NULL;
	char * t5PerYieldS=NULL;
	char * t5PunStoreLocationS=NULL;
	char * t5AltPartNoS=NULL;
	char * t5RolledupWtS=NULL;
	char * t5PFDModReqdS=NULL;
	char * t5ToolIndentReqdS=NULL;
	char * CategoryNameS=NULL;
	char * t5DsgnDeptS=NULL;
	char * t5AplCopyOfErcSeqS=NULL;
	char *FromDate;
	char *ToDate;
	int si;
	int lat_seq_id_int;
	int lat_seq_id_int_1;
	char *itemRevSeq = NULL;
	char *t5carMakBuy = NULL;
	char *t5carStoreLoc = NULL;
	char *item_org_id = NULL;
	char *aplEffFromDate = NULL;
	char *aplEffToDate = NULL;
	char *stdEffFromDate = NULL;
	char *stdEffToDate = NULL;
	char *parttype = NULL;
	char *t5EstSheetReqdS = NULL;
	char *PartCreDateS = NULL;
	char *PartModDateS = NULL;
	char *PartCreator = NULL;
	char *testVal = NULL;
	char *testdesc = NULL;

	// Ended By Raghavendra B T

	char**	stringArray = NULL;
	tag_t newItemTag=NULLTAG;
	tag_t itemRevCreInTag=NULLTAG;
	tag_t itemRevTypeTag=NULLTAG;
	tag_t item=NULLTAG;
	tag_t rev=NULLTAG;
	tag_t *tags_found = NULL;
	char *inputfile=NULL;
	char *errorLogPath=NULL;
	int n_tags_found= 0;
	//char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	//char **values = (char **) MEM_alloc(1 * sizeof(char *));
	const char *attrs[1];
	const char *values[1];


	 int stat_count=0;
	 int stat_count2=0;
	 tag_t relPropTag=NULLTAG ;
	 tag_t relPropTag2=NULLTAG ;
	 tag_t status2=NULLTAG ;
	 tag_t reln_type=NULLTAG ;
	 tag_t reln_type2=NULLTAG ;
	 tag_t *secondary_objects=NULLTAG ;
	 tag_t *secondary_objects2=NULLTAG ;
	 tag_t *status1=NULL;
	 tag_t  attr_name_id   = NULLTAG;
	 tag_t  apl_item   = NULLTAG;
	 tag_t  primary=NULLTAG,objTypeTag=NULLTAG;
	 tag_t  primary2=NULLTAG,objTypeTag2=NULLTAG;
     char   *value         = NULL;
     char   *eff_date         = NULL;
	 tag_t class_id = NULLTAG;
	 char szClassName[100 + 1] = "";
     logical is_it_empty = FALSE;
     logical is_it_null  = FALSE;
     logical is_loaded   = FALSE;
	 char   type_name[100+1];
	 char   type_name2[100+1];
	 int release_status   = 0;
	 FILE* fperror=NULL;
	 char errLogP[200];


char*  TryOutNoDup=NULL;
char*  AffshopsDup=NULL;
char*  AggregateDup=NULL;
char*  BlockDup=NULL;
char*  PlantDup=NULL;
char*  AreaDup=NULL;
char*  BatchSizeDup=NULL;
char*  ChnclByDup=NULL;
char*  ChnclcommentsDup=NULL;
char*  ClnclDtDup=NULL;
char*  CrecommentsDup=NULL;
char*  CycleTimeDup=NULL;
char*  DaviationDup=NULL;
char*  DaviationDescDup=NULL;
char*  DmlNo1Dup=NULL;
char*  DmlNo2Dup=NULL;
char*  DmlNo3Dup=NULL;
char*  DmlNo4Dup=NULL;
char*  DmlNo5Dup=NULL;
char*  DmlNo6Dup=NULL;
char*  DmlNo7Dup=NULL;
char*  DmlNo8Dup=NULL;
char*  DmlNo9Dup=NULL;
char*  DmlNo10Dup=NULL;
char*  FactHeadDup=NULL;
char*  FeedbackDup=NULL;
char*  FitmentDup=NULL;
char*  IntVinNo1Dup=NULL;
char*  IntVinNo2Dup=NULL;
char*  IntVinNo3Dup=NULL;
char*  IntVinNo4Dup=NULL;
char*  IntVinNo5Dup=NULL;
char*  IntVinNo6Dup=NULL;
char*  IntVinNo7Dup=NULL;
char*  IntVinNo8Dup=NULL;
char*  IntVinNo9Dup=NULL;
char*  IntVinNo10Dup=NULL;
char*  IsDMLDup=NULL;
char*  IsEPADup=NULL;
char*  LocationDup=NULL;
char*  ManReviewerDup=NULL;
char*  ModPartDescDup=NULL;
char*  ModPartDesc1Dup=NULL;
char*  ModPartDesc2Dup=NULL;
char*  ModPartDesc3Dup=NULL;
char*  ModPartDesc4Dup=NULL;
char*  ModPartDesc6Dup=NULL;
char*  ModPartDesc7Dup=NULL;
char*  ModPartDesc8Dup=NULL;
char*  ModPartDesc9Dup=NULL;
char*  ModPartDesc10Dup=NULL;
char*  ModPartNoDup=NULL;
char*  ModPartNo1Dup=NULL;
char*  ModPartNo2Dup=NULL;
char*  ModPartNo3Dup=NULL;
char*  ModPartNo4Dup=NULL;
char*  TOModTypeDup=NULL;
char*  TOModType1Dup=NULL;
char*  TOModType2Dup=NULL;
char*  TOModType3Dup=NULL;
char*  TOModType4Dup=NULL;
char*  TOModType5Dup=NULL;
char*  TOModType6Dup=NULL;
char*  TOModType7Dup=NULL;
char*  TOModType8Dup=NULL;
char*  TOModType9Dup=NULL;
char*  PartDesc1Dup=NULL;
char*  PartDesc2Dup=NULL;
char*  PartDesc3Dup=NULL;
char*  PartDesc4Dup=NULL;
char*  PartDesc5Dup=NULL;
char*  PartDesc6Dup=NULL;
char*  PartDesc7Dup=NULL;
char*  PartDesc8Dup=NULL;
char*  PartDesc9Dup=NULL;
char*  PartDesc10Dup=NULL;
char*  TOPartNo1Dup=NULL;
char*  TOPartNo2Dup=NULL;
char*  TOPartNo3Dup=NULL;
char*  TOPartNo4Dup=NULL;
char*  TOPartNo5Dup=NULL;
char*  TOPartNo6Dup=NULL;
char*  TOPartNo7Dup=NULL;
char*  TOPartNo8Dup=NULL;
char*  TOPartNo9Dup=NULL;
char*  TOPartNo10Dup=NULL;
char*  TOPartRev1Dup=NULL;
char*  TOPartRev2Dup=NULL;
char*  TOPartRev3Dup=NULL;
char*  TOPartRev4Dup=NULL;
char*  TOPartRev5Dup=NULL;
char*  TOPartRev6Dup=NULL;
char*  TOPartRev7Dup=NULL;
char*  TOPartRev8Dup=NULL;
char*  TOPartRev9Dup=NULL;
char*  TOPartRev10Dup=NULL;
char*  TOPartSeq1Dup=NULL;
char*  TOPartSeq2Dup=NULL;
char*  TOPartSeq3Dup=NULL;
char*  TOPartSeq4Dup=NULL;
char*  TOPartSeq5Dup=NULL;
char*  TOPartSeq6Dup=NULL;
char*  TOPartSeq7Dup=NULL;
char*  TOPartSeq8Dup=NULL;
char*  TOPartSeq9Dup=NULL;
char*  TOPartSeq10Dup=NULL;
char*  TOPhotosDup=NULL;
char*  TOQAHeadDup=NULL;
char*  TOQAHeadcommentsDup=NULL;
char*  TOQAReviewerDup=NULL;
char*  TOQAcommentsDup=NULL;
char*  TOReasonDup=NULL;
char*  TORefNoDup=NULL;
char*  TORefNo2Dup=NULL;
char*  TORefNo3Dup=NULL;
char*  TORefNo4Dup=NULL;
char*  TORefNo5Dup=NULL;
char*  TORefNo6Dup=NULL;
char*  TORefNo7Dup=NULL;
char*  TORefNo8Dup=NULL;
char*  TORefNo9Dup=NULL;
char*  TORefNo10Dup=NULL;
char*  TOSCMReviewerDup=NULL;
char*  TOSTDReviewerDup=NULL;
char*  TOSubAreaDup=NULL;
char*  TOTSReviewerDup=NULL;
char*  TOTScommentsDup=NULL;
char*  TOTestDetailsDup=NULL;
char*  TOTryOutDtDup=NULL;
char*  TOVDReviewerDup=NULL;
char*  TOVDcommentsDup=NULL;
char*  TOVQAReviewerDup=NULL;
char*  TOVQAcommentsDup=NULL;
char*  TOVehNoDup=NULL;
char*  TOVenLoginDup=NULL;
char*  TOVenNameDup=NULL;
char*  TOVinNoDup=NULL;
char*  TOVinNo1Dup=NULL;
char*  TOVinNo2Dup=NULL;
char*  TOVinNo3Dup=NULL;
char*  TOVinNo4Dup=NULL;
char*  TOhlgreqDup=NULL;
char*  ESLstModByDup=NULL;
char*  CreDatestr=NULL;
char*  LstUpdstr=NULL;
char*  CrtrDup=NULL;
char*  LCS=NULL;
char*  AreaCountDup=NULL;
char*  SubAreaCountDup=NULL;
char*  VehNoCountDup=NULL;

int FlagERR=0;

	char *szbom_revision_status = NULL;
	tag_t   window, window2, rule, item_tag = null_tag, top_line;

	inputfile = ITK_ask_cli_argument("-i=");
	//errorLogPath = ITK_ask_cli_argument("-ep=");
	ITK_CALL(ITK_auto_login( ));
    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	
	//ITK_CALL(ITK_init_module("APLloader","abc123","APLCAR"));
	//ITK_CALL(ITK_init_module("aplloader","infodba","dba"));
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Auto login .......");fflush(stdout);

	//printf("\n Item:[%s]\nRev:[%s]\nSeq[%s]",req_item,req_rev,req_seq);fflush(stdout);

    //ITK_CALL(ITEM_find_item(req_item, &item ));

	//tc_strcpy(errLogP,errorLogPath);
	//tc_strcat(errLogP,"/error_APLPartCreation.log");
	//printf("\n errLogP %s",errLogP);fflush(stdout);

	fp=fopen(inputfile,"r");
	printf("\n File open");fflush(stdout);
	fperror=fopen("ES_Exception.log","a");
	printf("\n hello..");fflush(stdout);
	if(fp!=NULL)
	{
		printf("\n reading file ....");fflush(stdout);
		inputline=(char *) MEM_alloc(10000);
		while(fgets(inputline,10000,fp)!=NULL)
		{
			printf("\n reading file after  ....");fflush(stdout);

			fputs(inputline,stdout);
			TryOutNoDup=strtok(inputline,"^");  
			AffshopsDup=strtok(NULL,"^");  
			AggregateDup=strtok(NULL,"^");  
			BlockDup=strtok(NULL,"^");  
			PlantDup=strtok(NULL,"^");  
			AreaDup=strtok(NULL,"^");  
			AreaCountDup=strtok(NULL,"^");  
			BatchSizeDup=strtok(NULL,"^");  
			ChnclByDup=strtok(NULL,"^");  
			ChnclcommentsDup=strtok(NULL,"^");  
			ClnclDtDup=strtok(NULL,"^");  
			CrecommentsDup=strtok(NULL,"^");  
			CycleTimeDup=strtok(NULL,"^");  
			DaviationDup=strtok(NULL,"^");  
			DaviationDescDup=strtok(NULL,"^");  
			DmlNo1Dup=strtok(NULL,"^");  
			DmlNo2Dup=strtok(NULL,"^");  
			DmlNo3Dup=strtok(NULL,"^");  
			DmlNo4Dup=strtok(NULL,"^");  
			DmlNo5Dup=strtok(NULL,"^");  
			DmlNo6Dup=strtok(NULL,"^");  
			DmlNo7Dup=strtok(NULL,"^");  
			DmlNo8Dup=strtok(NULL,"^");  
			DmlNo9Dup=strtok(NULL,"^");  
			DmlNo10Dup=strtok(NULL,"^");  
			FactHeadDup=strtok(NULL,"^");  
			FeedbackDup=strtok(NULL,"^");  
			FitmentDup=strtok(NULL,"^");  
			IntVinNo1Dup=strtok(NULL,"^");  
			IntVinNo2Dup=strtok(NULL,"^");  
			IntVinNo3Dup=strtok(NULL,"^");  
			IntVinNo4Dup=strtok(NULL,"^");  
			IntVinNo5Dup=strtok(NULL,"^");  
			IntVinNo6Dup=strtok(NULL,"^");  
			IntVinNo7Dup=strtok(NULL,"^"); 
			IntVinNo8Dup=strtok(NULL,"^"); 
			IntVinNo9Dup=strtok(NULL,"^"); 
			IntVinNo10Dup=strtok(NULL,"^"); 
			IsDMLDup=strtok(NULL,"^"); 
			IsEPADup=strtok(NULL,"^"); 
			LocationDup=strtok(NULL,"^"); 
			ManReviewerDup=strtok(NULL,"^"); 
			ModPartDescDup=strtok(NULL,"^"); 
			ModPartDesc1Dup=strtok(NULL,"^"); 
			ModPartDesc2Dup=strtok(NULL,"^"); 
			ModPartDesc3Dup=strtok(NULL,"^"); 
			ModPartDesc4Dup=strtok(NULL,"^"); 
			ModPartDesc6Dup=strtok(NULL,"^"); 
			ModPartDesc7Dup=strtok(NULL,"^"); 
			ModPartDesc8Dup=strtok(NULL,"^"); 
			ModPartDesc9Dup=strtok(NULL,"^"); 
			ModPartDesc10Dup=strtok(NULL,"^"); 
			ModPartNoDup=strtok(NULL,"^"); 
			ModPartNo1Dup=strtok(NULL,"^"); 
			ModPartNo2Dup=strtok(NULL,"^"); 
			ModPartNo3Dup=strtok(NULL,"^"); 
			ModPartNo4Dup=strtok(NULL,"^"); 
			TOModTypeDup=strtok(NULL,"^"); 
			TOModType1Dup=strtok(NULL,"^"); 
			TOModType2Dup=strtok(NULL,"^"); 
			TOModType3Dup=strtok(NULL,"^"); 
			TOModType4Dup=strtok(NULL,"^"); 
			TOModType5Dup=strtok(NULL,"^"); 
			TOModType6Dup=strtok(NULL,"^"); 
			TOModType7Dup=strtok(NULL,"^"); 
			TOModType8Dup=strtok(NULL,"^"); 
			TOModType9Dup=strtok(NULL,"^"); 
			PartDesc1Dup=strtok(NULL,"^"); 
			PartDesc2Dup=strtok(NULL,"^"); 
			PartDesc3Dup=strtok(NULL,"^"); 
			PartDesc4Dup=strtok(NULL,"^"); 
			PartDesc5Dup=strtok(NULL,"^"); 
			PartDesc6Dup=strtok(NULL,"^"); 
			PartDesc7Dup=strtok(NULL,"^"); 
			PartDesc8Dup=strtok(NULL,"^"); 
			PartDesc9Dup=strtok(NULL,"^"); 
			PartDesc10Dup=strtok(NULL,"^"); 
			TOPartNo1Dup=strtok(NULL,"^"); 
			TOPartNo2Dup=strtok(NULL,"^"); 
			TOPartNo3Dup=strtok(NULL,"^"); 
			TOPartNo4Dup=strtok(NULL,"^"); 
			TOPartNo5Dup=strtok(NULL,"^"); 
			TOPartNo6Dup=strtok(NULL,"^"); 
			TOPartNo7Dup=strtok(NULL,"^"); 
			TOPartNo8Dup=strtok(NULL,"^"); 
			TOPartNo9Dup=strtok(NULL,"^"); 
			TOPartNo10Dup=strtok(NULL,"^"); 
			TOPartRev1Dup=strtok(NULL,"^"); 
			TOPartRev2Dup=strtok(NULL,"^"); 
			TOPartRev3Dup=strtok(NULL,"^"); 
			TOPartRev4Dup=strtok(NULL,"^"); 
			TOPartRev5Dup=strtok(NULL,"^"); 
			TOPartRev6Dup=strtok(NULL,"^"); 
			TOPartRev7Dup=strtok(NULL,"^"); 
			TOPartRev8Dup=strtok(NULL,"^"); 
			TOPartRev9Dup=strtok(NULL,"^"); 
			TOPartRev10Dup=strtok(NULL,"^"); 
			TOPartSeq1Dup=strtok(NULL,"^"); 
			TOPartSeq2Dup=strtok(NULL,"^"); 
			TOPartSeq3Dup=strtok(NULL,"^"); 
			TOPartSeq4Dup=strtok(NULL,"^"); 
			TOPartSeq5Dup=strtok(NULL,"^"); 
			TOPartSeq6Dup=strtok(NULL,"^"); 
			TOPartSeq7Dup=strtok(NULL,"^"); 
			TOPartSeq8Dup=strtok(NULL,"^"); 
			TOPartSeq9Dup=strtok(NULL,"^"); 
			TOPartSeq10Dup=strtok(NULL,"^"); 
			TOPhotosDup=strtok(NULL,"^"); 
			TOQAHeadDup=strtok(NULL,"^"); 
			TOQAHeadcommentsDup=strtok(NULL,"^"); 
			TOQAReviewerDup=strtok(NULL,"^"); 
			TOQAcommentsDup=strtok(NULL,"^"); 
			TOReasonDup=strtok(NULL,"^"); 
			TORefNoDup=strtok(NULL,"^"); 
			TORefNo2Dup=strtok(NULL,"^"); 
			TORefNo3Dup=strtok(NULL,"^"); 
			TORefNo4Dup=strtok(NULL,"^"); 
			TORefNo5Dup=strtok(NULL,"^"); 
			TORefNo6Dup=strtok(NULL,"^"); 
			TORefNo7Dup=strtok(NULL,"^"); 
			TORefNo8Dup=strtok(NULL,"^"); 
			TORefNo9Dup=strtok(NULL,"^"); 
			TORefNo10Dup=strtok(NULL,"^"); 
			TOSCMReviewerDup=strtok(NULL,"^"); 
			TOSTDReviewerDup=strtok(NULL,"^"); 
			TOSubAreaDup=strtok(NULL,"^"); 
			SubAreaCountDup=strtok(NULL,"^"); 
			TOTSReviewerDup=strtok(NULL,"^"); 
			TOTScommentsDup=strtok(NULL,"^"); 
			TOTestDetailsDup=strtok(NULL,"^"); 
			TOTryOutDtDup=strtok(NULL,"^"); 
			TOVDReviewerDup=strtok(NULL,"^"); 
			TOVDcommentsDup=strtok(NULL,"^"); 
			TOVQAReviewerDup=strtok(NULL,"^"); 
			TOVQAcommentsDup=strtok(NULL,"^"); 
			TOVehNoDup=strtok(NULL,"^"); 
			VehNoCountDup=strtok(NULL,"^"); 
			TOVenLoginDup=strtok(NULL,"^"); 
			TOVenNameDup=strtok(NULL,"^"); 
			TOVinNoDup=strtok(NULL,"^"); 
			TOVinNo1Dup=strtok(NULL,"^"); 
			TOVinNo2Dup=strtok(NULL,"^"); 
			TOVinNo3Dup=strtok(NULL,"^"); 
			TOVinNo4Dup=strtok(NULL,"^"); 
			TOhlgreqDup=strtok(NULL,"^"); 
			ESLstModByDup=strtok(NULL,"^"); 
			CreDatestr=strtok(NULL,"^"); 
			LstUpdstr=strtok(NULL,"^"); 
			CrtrDup=strtok(NULL,"^"); 
			LCS=strtok(NULL,"^"); 

			printf("\n BEFORE TRIM");fflush(stdout);  
				printf("\t\n TryOutNoDup::[%s]",TryOutNoDup);fflush(stdout);
				printf("\t\n AffshopsDup::[%s]",AffshopsDup);fflush(stdout);
				printf("\t\n AggregateDup::[%s]",AggregateDup);fflush(stdout);
				printf("\t\n BlockDup::[%s]",BlockDup);fflush(stdout);
				printf("\t\n PlantDup::[%s]",PlantDup);fflush(stdout);
				printf("\t\n AreaDup::[%s]",AreaDup);fflush(stdout);
				printf("\t\n AreaCountDup::[%s]",AreaCountDup);fflush(stdout);
				printf("\t\n BatchSizeDup::[%s]",BatchSizeDup);fflush(stdout);
				printf("\t\n ChnclByDup::[%s]",ChnclByDup);fflush(stdout);
				printf("\t\n ChnclcommentsDup::[%s]",ChnclcommentsDup);fflush(stdout);
				printf("\t\n ClnclDtDup::[%s]",ClnclDtDup);fflush(stdout);
				printf("\t\n CrecommentsDup::[%s]",CrecommentsDup);fflush(stdout);
				printf("\t\n CycleTimeDup::[%s]",CycleTimeDup);fflush(stdout);
				printf("\t\n DaviationDup::[%s]",DaviationDup);fflush(stdout);
				printf("\t\n DaviationDescDup::[%s]",DaviationDescDup);fflush(stdout);
				printf("\t\n DmlNo1Dup::[%s]",DmlNo1Dup);fflush(stdout);
				printf("\t\n DmlNo2Dup::[%s]",DmlNo2Dup);fflush(stdout);
				printf("\t\n DmlNo3Dup::[%s]",DmlNo3Dup);fflush(stdout);
				printf("\t\n DmlNo4Dup::[%s]",DmlNo4Dup);fflush(stdout);
				printf("\t\n DmlNo5Dup::[%s]",DmlNo5Dup);fflush(stdout);
				printf("\t\n DmlNo6Dup::[%s]",DmlNo6Dup);fflush(stdout);
				printf("\t\n DmlNo7Dup::[%s]",DmlNo7Dup);fflush(stdout);
				printf("\t\n DmlNo8Dup::[%s]",DmlNo8Dup);fflush(stdout);
				printf("\t\n DmlNo9Dup::[%s]",DmlNo9Dup);fflush(stdout);
				printf("\t\n DmlNo10Dup::[%s]",DmlNo10Dup);fflush(stdout);
				printf("\t\n FactHeadDup::[%s]",FactHeadDup);fflush(stdout);
				printf("\t\n FeedbackDup::[%s]",FeedbackDup);fflush(stdout);
				printf("\t\n FitmentDup::[%s]",FitmentDup);fflush(stdout);
				printf("\t\n IntVinNo1Dup::[%s]",IntVinNo1Dup);fflush(stdout);
				printf("\t\n IntVinNo2Dup::[%s]",IntVinNo2Dup);fflush(stdout);
				printf("\t\n IntVinNo3Dup::[%s]",IntVinNo3Dup);fflush(stdout);
				printf("\t\n IntVinNo4Dup::[%s]",IntVinNo4Dup);fflush(stdout);
				printf("\t\n IntVinNo5Dup::[%s]",IntVinNo5Dup);fflush(stdout);
				printf("\t\n IntVinNo6Dup::[%s]",IntVinNo6Dup);fflush(stdout);
				printf("\t\n IntVinNo7Dup::[%s]",IntVinNo7Dup);fflush(stdout);
				printf("\t\n IntVinNo8Dup::[%s]",IntVinNo8Dup);fflush(stdout);
				printf("\t\n IntVinNo9Dup::[%s]",IntVinNo9Dup);fflush(stdout);
				printf("\t\n IntVinNo10Dup::[%s]",IntVinNo10Dup);fflush(stdout);
				printf("\t\n IsDMLDup::[%s]",IsDMLDup);fflush(stdout);
				printf("\t\n IsEPADup::[%s]",IsEPADup);fflush(stdout);
				printf("\t\n LocationDup::[%s]",LocationDup);fflush(stdout);
				printf("\t\n ManReviewerDup::[%s]",ManReviewerDup);fflush(stdout);
				printf("\t\n ModPartDescDup::[%s]",ModPartDescDup);fflush(stdout);
				printf("\t\n ModPartDesc1Dup::[%s]",ModPartDesc1Dup);fflush(stdout);
				printf("\t\n ModPartDesc2Dup::[%s]",ModPartDesc2Dup);fflush(stdout);
				printf("\t\n ModPartDesc3Dup::[%s]",ModPartDesc3Dup);fflush(stdout);
				printf("\t\n ModPartDesc4Dup::[%s]",ModPartDesc4Dup);fflush(stdout);
				printf("\t\n ModPartDesc6Dup::[%s]",ModPartDesc6Dup);fflush(stdout);
				printf("\t\n ModPartDesc7Dup::[%s]",ModPartDesc7Dup);fflush(stdout);
				printf("\t\n ModPartDesc8Dup::[%s]",ModPartDesc8Dup);fflush(stdout);
				printf("\t\n ModPartDesc9Dup::[%s]",ModPartDesc9Dup);fflush(stdout);
				printf("\t\n ModPartDesc10Dup::[%s]",ModPartDesc10Dup);fflush(stdout);
				printf("\t\n ModPartNoDup::[%s]",ModPartNoDup);fflush(stdout);
				printf("\t\n ModPartNo1Dup::[%s]",ModPartNo1Dup);fflush(stdout);
				printf("\t\n ModPartNo2Dup::[%s]",ModPartNo2Dup);fflush(stdout);
				printf("\t\n ModPartNo3Dup::[%s]",ModPartNo3Dup);fflush(stdout);
				printf("\t\n ModPartNo4Dup::[%s]",ModPartNo4Dup);fflush(stdout);
				printf("\t\n TOModTypeDup::[%s]",TOModTypeDup);fflush(stdout);
				printf("\t\n TOModType1Dup::[%s]",TOModType1Dup);fflush(stdout);
				printf("\t\n TOModType2Dup::[%s]",TOModType2Dup);fflush(stdout);
				printf("\t\n TOModType3Dup::[%s]",TOModType3Dup);fflush(stdout);
				printf("\t\n TOModType4Dup::[%s]",TOModType4Dup);fflush(stdout);
				printf("\t\n TOModType5Dup::[%s]",TOModType5Dup);fflush(stdout);
				printf("\t\n TOModType6Dup::[%s]",TOModType6Dup);fflush(stdout);
				printf("\t\n TOModType7Dup::[%s]",TOModType7Dup);fflush(stdout);
				printf("\t\n TOModType8Dup::[%s]",TOModType8Dup);fflush(stdout);
				printf("\t\n TOModType9Dup::[%s]",TOModType9Dup);fflush(stdout);
				printf("\t\n PartDesc1Dup::[%s]",PartDesc1Dup);fflush(stdout);
				printf("\t\n PartDesc2Dup::[%s]",PartDesc2Dup);fflush(stdout);
				printf("\t\n PartDesc3Dup::[%s]",PartDesc3Dup);fflush(stdout);
				printf("\t\n PartDesc4Dup::[%s]",PartDesc4Dup);fflush(stdout);
				printf("\t\n PartDesc5Dup::[%s]",PartDesc5Dup);fflush(stdout);
				printf("\t\n PartDesc6Dup::[%s]",PartDesc6Dup);fflush(stdout);
				printf("\t\n PartDesc7Dup::[%s]",PartDesc7Dup);fflush(stdout);
				printf("\t\n PartDesc8Dup::[%s]",PartDesc8Dup);fflush(stdout);
				printf("\t\n PartDesc9Dup::[%s]",PartDesc9Dup);fflush(stdout);
				printf("\t\n PartDesc10Dup::[%s]",PartDesc10Dup);fflush(stdout);
				printf("\t\n TOPartNo1Dup::[%s]",TOPartNo1Dup);fflush(stdout);
				printf("\t\n TOPartNo2Dup::[%s]",TOPartNo2Dup);fflush(stdout);
				printf("\t\n TOPartNo3Dup::[%s]",TOPartNo3Dup);fflush(stdout);
				printf("\t\n TOPartNo4Dup::[%s]",TOPartNo4Dup);fflush(stdout);
				printf("\t\n TOPartNo5Dup::[%s]",TOPartNo5Dup);fflush(stdout);
				printf("\t\n TOPartNo6Dup::[%s]",TOPartNo6Dup);fflush(stdout);
				printf("\t\n TOPartNo7Dup::[%s]",TOPartNo7Dup);fflush(stdout);
				printf("\t\n TOPartNo8Dup::[%s]",TOPartNo8Dup);fflush(stdout);
				printf("\t\n TOPartNo9Dup::[%s]",TOPartNo9Dup);fflush(stdout);
				printf("\t\n TOPartNo10Dup::[%s]",TOPartNo10Dup);fflush(stdout);
				printf("\t\n TOPartRev1Dup::[%s]",TOPartRev1Dup);fflush(stdout);
				printf("\t\n TOPartRev2Dup::[%s]",TOPartRev2Dup);fflush(stdout);
				printf("\t\n TOPartRev3Dup::[%s]",TOPartRev3Dup);fflush(stdout);
				printf("\t\n TOPartRev4Dup::[%s]",TOPartRev4Dup);fflush(stdout);
				printf("\t\n TOPartRev5Dup::[%s]",TOPartRev5Dup);fflush(stdout);
				printf("\t\n TOPartRev6Dup::[%s]",TOPartRev6Dup);fflush(stdout);
				printf("\t\n TOPartRev7Dup::[%s]",TOPartRev7Dup);fflush(stdout);
				printf("\t\n TOPartRev8Dup::[%s]",TOPartRev8Dup);fflush(stdout);
				printf("\t\n TOPartRev9Dup::[%s]",TOPartRev9Dup);fflush(stdout);
				printf("\t\n TOPartRev10Dup::[%s]",TOPartRev10Dup);fflush(stdout);
				printf("\t\n TOPartSeq1Dup::[%s]",TOPartSeq1Dup);fflush(stdout);
				printf("\t\n TOPartSeq2Dup::[%s]",TOPartSeq2Dup);fflush(stdout);
				printf("\t\n TOPartSeq3Dup::[%s]",TOPartSeq3Dup);fflush(stdout);
				printf("\t\n TOPartSeq4Dup::[%s]",TOPartSeq4Dup);fflush(stdout);
				printf("\t\n TOPartSeq5Dup::[%s]",TOPartSeq5Dup);fflush(stdout);
				printf("\t\n TOPartSeq6Dup::[%s]",TOPartSeq6Dup);fflush(stdout);
				printf("\t\n TOPartSeq7Dup::[%s]",TOPartSeq7Dup);fflush(stdout);
				printf("\t\n TOPartSeq8Dup::[%s]",TOPartSeq8Dup);fflush(stdout);
				printf("\t\n TOPartSeq9Dup::[%s]",TOPartSeq9Dup);fflush(stdout);
				printf("\t\n TOPartSeq10Dup::[%s]",TOPartSeq10Dup);fflush(stdout);
				printf("\t\n TOPhotosDup::[%s]",TOPhotosDup);fflush(stdout);
				printf("\t\n TOQAHeadDup::[%s]",TOQAHeadDup);fflush(stdout);
				printf("\t\n TOQAHeadcommentsDup::[%s]",TOQAHeadcommentsDup);fflush(stdout);
				printf("\t\n TOQAReviewerDup::[%s]",TOQAReviewerDup);fflush(stdout);
				printf("\t\n TOQAcommentsDup::[%s]",TOQAcommentsDup);fflush(stdout);
				printf("\t\n TOReasonDup::[%s]",TOReasonDup);fflush(stdout);
				printf("\t\n TORefNoDup::[%s]",TORefNoDup);fflush(stdout);
				printf("\t\n TORefNo2Dup::[%s]",TORefNo2Dup);fflush(stdout);
				printf("\t\n TORefNo3Dup::[%s]",TORefNo3Dup);fflush(stdout);
				printf("\t\n TORefNo4Dup::[%s]",TORefNo4Dup);fflush(stdout);
				printf("\t\n TORefNo5Dup::[%s]",TORefNo5Dup);fflush(stdout);
				printf("\t\n TORefNo6Dup::[%s]",TORefNo6Dup);fflush(stdout);
				printf("\t\n TORefNo7Dup::[%s]",TORefNo7Dup);fflush(stdout);
				printf("\t\n TORefNo8Dup::[%s]",TORefNo8Dup);fflush(stdout);
				printf("\t\n TORefNo9Dup::[%s]",TORefNo9Dup);fflush(stdout);
				printf("\t\n TORefNo10Dup::[%s]",TORefNo10Dup);fflush(stdout);
				printf("\t\n TOSCMReviewerDup::[%s]",TOSCMReviewerDup);fflush(stdout);
				printf("\t\n TOSTDReviewerDup::[%s]",TOSTDReviewerDup);fflush(stdout);
				printf("\t\n TOSubAreaDup::[%s]",TOSubAreaDup);fflush(stdout);
				printf("\t\n SubAreaCountDup::[%s]",SubAreaCountDup);fflush(stdout);
				printf("\t\n TOTSReviewerDup::[%s]",TOTSReviewerDup);fflush(stdout);
				printf("\t\n TOTScommentsDup::[%s]",TOTScommentsDup);fflush(stdout);
				printf("\t\n TOTestDetailsDup::[%s]",TOTestDetailsDup);fflush(stdout);
				printf("\t\n TOTryOutDtDup::[%s]",TOTryOutDtDup);fflush(stdout);
				printf("\t\n TOVDReviewerDup::[%s]",TOVDReviewerDup);fflush(stdout);
				printf("\t\n TOVDcommentsDup::[%s]",TOVDcommentsDup);fflush(stdout);
				printf("\t\n TOVQAReviewerDup::[%s]",TOVQAReviewerDup);fflush(stdout);
				printf("\t\n TOVQAcommentsDup::[%s]",TOVQAcommentsDup);fflush(stdout);
				printf("\t\n TOVehNoDup::[%s]",TOVehNoDup);fflush(stdout);
				printf("\t\n VehNoCountDup::[%s]",VehNoCountDup);fflush(stdout);
				printf("\t\n TOVenLoginDup::[%s]",TOVenLoginDup);fflush(stdout);
				printf("\t\n TOVenNameDup::[%s]",TOVenNameDup);fflush(stdout);
				printf("\t\n TOVinNoDup::[%s]",TOVinNoDup);fflush(stdout);
				printf("\t\n TOVinNo1Dup::[%s]",TOVinNo1Dup);fflush(stdout);
				printf("\t\n TOVinNo2Dup::[%s]",TOVinNo2Dup);fflush(stdout);
				printf("\t\n TOVinNo3Dup::[%s]",TOVinNo3Dup);fflush(stdout);
				printf("\t\n TOVinNo4Dup::[%s]",TOVinNo4Dup);fflush(stdout);
				printf("\t\n TOhlgreqDup::[%s]",TOhlgreqDup);fflush(stdout);
				printf("\t\n ESLstModByDup::[%s]",ESLstModByDup);fflush(stdout);
				printf("\t\n CreDatestr::[%s]",CreDatestr);fflush(stdout);
				printf("\t\n LstUpdstr::[%s]",LstUpdstr);fflush(stdout);
				printf("\t\n CrtrDup::[%s]",CrtrDup);fflush(stdout);
				printf("\t\n LCS::[%s]",LCS);fflush(stdout);

			StringTrim(&TryOutNoDup);
			StringTrim(&AffshopsDup);
			StringTrim(&AggregateDup);
			StringTrim(&BlockDup);
			StringTrim(&PlantDup);
			StringTrim(&AreaDup);
			StringTrim(&BatchSizeDup);
			StringTrim(&ChnclByDup);
			StringTrim(&ChnclcommentsDup);
			StringTrim(&ClnclDtDup);
			StringTrim(&CrecommentsDup);
			StringTrim(&CycleTimeDup);
			StringTrim(&DaviationDup);
			StringTrim(&DaviationDescDup);
			StringTrim(&DmlNo1Dup);
			StringTrim(&DmlNo2Dup);
			StringTrim(&DmlNo3Dup);
			StringTrim(&DmlNo4Dup);
			StringTrim(&DmlNo5Dup);
			StringTrim(&DmlNo6Dup);
			StringTrim(&DmlNo7Dup);
			StringTrim(&DmlNo8Dup);
			StringTrim(&DmlNo9Dup);
			StringTrim(&DmlNo10Dup);
			StringTrim(&FactHeadDup);
			StringTrim(&FeedbackDup);
			StringTrim(&FitmentDup);
			StringTrim(&IntVinNo1Dup);
			StringTrim(&IntVinNo2Dup);
			StringTrim(&IntVinNo3Dup);
			StringTrim(&IntVinNo4Dup);
			StringTrim(&IntVinNo5Dup);
			StringTrim(&IntVinNo6Dup);
			StringTrim(&IntVinNo7Dup);
			StringTrim(&IntVinNo8Dup);
			StringTrim(&IntVinNo9Dup);
			StringTrim(&IntVinNo10Dup);
			StringTrim(&IsDMLDup);
			StringTrim(&IsEPADup);
			StringTrim(&LocationDup);
			StringTrim(&ManReviewerDup);
			StringTrim(&ModPartDescDup);
			StringTrim(&ModPartDesc1Dup);
			StringTrim(&ModPartDesc2Dup);
			StringTrim(&ModPartDesc3Dup);
			StringTrim(&ModPartDesc4Dup);
			StringTrim(&ModPartDesc6Dup);
			StringTrim(&ModPartDesc7Dup);
			StringTrim(&ModPartDesc8Dup);
			StringTrim(&ModPartDesc9Dup);
			StringTrim(&ModPartDesc10Dup);
			StringTrim(&ModPartNoDup);
			StringTrim(&ModPartNo1Dup);
			StringTrim(&ModPartNo2Dup);
			StringTrim(&ModPartNo3Dup);
			StringTrim(&ModPartNo4Dup);
			StringTrim(&TOModTypeDup);
			StringTrim(&TOModType1Dup);
			StringTrim(&TOModType2Dup);
			StringTrim(&TOModType3Dup);
			StringTrim(&TOModType4Dup);
			StringTrim(&TOModType5Dup);
			StringTrim(&TOModType6Dup);
			StringTrim(&TOModType7Dup);
			StringTrim(&TOModType8Dup);
			StringTrim(&TOModType9Dup);
			StringTrim(&PartDesc1Dup);
			StringTrim(&PartDesc2Dup);
			StringTrim(&PartDesc3Dup);
			StringTrim(&PartDesc4Dup);
			StringTrim(&PartDesc5Dup);
			StringTrim(&PartDesc6Dup);
			StringTrim(&PartDesc7Dup);
			StringTrim(&PartDesc8Dup);
			StringTrim(&PartDesc9Dup);
			StringTrim(&PartDesc10Dup);
			StringTrim(&TOPartNo1Dup);
			StringTrim(&TOPartNo2Dup);
			StringTrim(&TOPartNo3Dup);
			StringTrim(&TOPartNo4Dup);
			StringTrim(&TOPartNo5Dup);
			StringTrim(&TOPartNo6Dup);
			StringTrim(&TOPartNo7Dup);
			StringTrim(&TOPartNo8Dup);
			StringTrim(&TOPartNo9Dup);
			StringTrim(&TOPartNo10Dup);
			StringTrim(&TOPartRev1Dup);
			StringTrim(&TOPartRev2Dup);
			StringTrim(&TOPartRev3Dup);
			StringTrim(&TOPartRev4Dup);
			StringTrim(&TOPartRev5Dup);
			StringTrim(&TOPartRev6Dup);
			StringTrim(&TOPartRev7Dup);
			StringTrim(&TOPartRev8Dup);
			StringTrim(&TOPartRev9Dup);
			StringTrim(&TOPartRev10Dup);
			StringTrim(&TOPartSeq1Dup);
			StringTrim(&TOPartSeq2Dup);
			StringTrim(&TOPartSeq3Dup);
			StringTrim(&TOPartSeq4Dup);
			StringTrim(&TOPartSeq5Dup);
			StringTrim(&TOPartSeq6Dup);
			StringTrim(&TOPartSeq7Dup);
			StringTrim(&TOPartSeq8Dup);
			StringTrim(&TOPartSeq9Dup);
			StringTrim(&TOPartSeq10Dup);
			StringTrim(&TOPhotosDup);
			StringTrim(&TOQAHeadDup);
			StringTrim(&TOQAHeadcommentsDup);
			StringTrim(&TOQAReviewerDup);
			StringTrim(&TOQAcommentsDup);
			StringTrim(&TOReasonDup);
			StringTrim(&TORefNoDup);
			StringTrim(&TORefNo2Dup);
			StringTrim(&TORefNo3Dup);
			StringTrim(&TORefNo4Dup);
			StringTrim(&TORefNo5Dup);
			StringTrim(&TORefNo6Dup);
			StringTrim(&TORefNo7Dup);
			StringTrim(&TORefNo8Dup);
			StringTrim(&TORefNo9Dup);
			StringTrim(&TORefNo10Dup);
			StringTrim(&TOSCMReviewerDup);
			StringTrim(&TOSTDReviewerDup);
			StringTrim(&TOSubAreaDup);
			StringTrim(&TOTSReviewerDup);
			StringTrim(&TOTScommentsDup);
			StringTrim(&TOTestDetailsDup);
			StringTrim(&TOTryOutDtDup);
			StringTrim(&TOVDReviewerDup);
			StringTrim(&TOVDcommentsDup);
			StringTrim(&TOVQAReviewerDup);
			StringTrim(&TOVQAcommentsDup);
			StringTrim(&TOVehNoDup);
			StringTrim(&TOVenLoginDup);
			StringTrim(&TOVenNameDup);
			StringTrim(&TOVinNoDup);
			StringTrim(&TOVinNo1Dup);
			StringTrim(&TOVinNo2Dup);
			StringTrim(&TOVinNo3Dup);
			StringTrim(&TOVinNo4Dup);
			StringTrim(&TOhlgreqDup);
			StringTrim(&ESLstModByDup);
			StringTrim(&CreDatestr);
			StringTrim(&LstUpdstr);
			StringTrim(&CrtrDup);
			StringTrim(&LCS);


			printf("\n\n AFTER TRIM");fflush(stdout);  
			printf("\t\n TryOutNoDup::[%s]",TryOutNoDup);fflush(stdout);
			printf("\t\n AffshopsDup::[%s]",AffshopsDup);fflush(stdout);
			printf("\t\n AggregateDup::[%s]",AggregateDup);fflush(stdout);
			printf("\t\n BlockDup::[%s]",BlockDup);fflush(stdout);
			printf("\t\n PlantDup::[%s]",PlantDup);fflush(stdout);
			printf("\t\n AreaDup::[%s]",AreaDup);fflush(stdout);
			printf("\t\n AreaCountDup::[%s]",AreaCountDup);fflush(stdout);
			printf("\t\n BatchSizeDup::[%s]",BatchSizeDup);fflush(stdout);
			printf("\t\n ChnclByDup::[%s]",ChnclByDup);fflush(stdout);
			printf("\t\n ChnclcommentsDup::[%s]",ChnclcommentsDup);fflush(stdout);
			printf("\t\n ClnclDtDup::[%s]",ClnclDtDup);fflush(stdout);
			printf("\t\n CrecommentsDup::[%s]",CrecommentsDup);fflush(stdout);
			printf("\t\n CycleTimeDup::[%s]",CycleTimeDup);fflush(stdout);
			printf("\t\n DaviationDup::[%s]",DaviationDup);fflush(stdout);
			printf("\t\n DaviationDescDup::[%s]",DaviationDescDup);fflush(stdout);
			printf("\t\n DmlNo1Dup::[%s]",DmlNo1Dup);fflush(stdout);
			printf("\t\n DmlNo2Dup::[%s]",DmlNo2Dup);fflush(stdout);
			printf("\t\n DmlNo3Dup::[%s]",DmlNo3Dup);fflush(stdout);
			printf("\t\n DmlNo4Dup::[%s]",DmlNo4Dup);fflush(stdout);
			printf("\t\n DmlNo5Dup::[%s]",DmlNo5Dup);fflush(stdout);
			printf("\t\n DmlNo6Dup::[%s]",DmlNo6Dup);fflush(stdout);
			printf("\t\n DmlNo7Dup::[%s]",DmlNo7Dup);fflush(stdout);
			printf("\t\n DmlNo8Dup::[%s]",DmlNo8Dup);fflush(stdout);
			printf("\t\n DmlNo9Dup::[%s]",DmlNo9Dup);fflush(stdout);
			printf("\t\n DmlNo10Dup::[%s]",DmlNo10Dup);fflush(stdout);
			printf("\t\n FactHeadDup::[%s]",FactHeadDup);fflush(stdout);
			printf("\t\n FeedbackDup::[%s]",FeedbackDup);fflush(stdout);
			printf("\t\n FitmentDup::[%s]",FitmentDup);fflush(stdout);
			printf("\t\n IntVinNo1Dup::[%s]",IntVinNo1Dup);fflush(stdout);
			printf("\t\n IntVinNo2Dup::[%s]",IntVinNo2Dup);fflush(stdout);
			printf("\t\n IntVinNo3Dup::[%s]",IntVinNo3Dup);fflush(stdout);
			printf("\t\n IntVinNo4Dup::[%s]",IntVinNo4Dup);fflush(stdout);
			printf("\t\n IntVinNo5Dup::[%s]",IntVinNo5Dup);fflush(stdout);
			printf("\t\n IntVinNo6Dup::[%s]",IntVinNo6Dup);fflush(stdout);
			printf("\t\n IntVinNo7Dup::[%s]",IntVinNo7Dup);fflush(stdout);
			printf("\t\n IntVinNo8Dup::[%s]",IntVinNo8Dup);fflush(stdout);
			printf("\t\n IntVinNo9Dup::[%s]",IntVinNo9Dup);fflush(stdout);
			printf("\t\n IntVinNo10Dup::[%s]",IntVinNo10Dup);fflush(stdout);
			printf("\t\n IsDMLDup::[%s]",IsDMLDup);fflush(stdout);
			printf("\t\n IsEPADup::[%s]",IsEPADup);fflush(stdout);
			printf("\t\n LocationDup::[%s]",LocationDup);fflush(stdout);
			printf("\t\n ManReviewerDup::[%s]",ManReviewerDup);fflush(stdout);
			printf("\t\n ModPartDescDup::[%s]",ModPartDescDup);fflush(stdout);
			printf("\t\n ModPartDesc1Dup::[%s]",ModPartDesc1Dup);fflush(stdout);
			printf("\t\n ModPartDesc2Dup::[%s]",ModPartDesc2Dup);fflush(stdout);
			printf("\t\n ModPartDesc3Dup::[%s]",ModPartDesc3Dup);fflush(stdout);
			printf("\t\n ModPartDesc4Dup::[%s]",ModPartDesc4Dup);fflush(stdout);
			printf("\t\n ModPartDesc6Dup::[%s]",ModPartDesc6Dup);fflush(stdout);
			printf("\t\n ModPartDesc7Dup::[%s]",ModPartDesc7Dup);fflush(stdout);
			printf("\t\n ModPartDesc8Dup::[%s]",ModPartDesc8Dup);fflush(stdout);
			printf("\t\n ModPartDesc9Dup::[%s]",ModPartDesc9Dup);fflush(stdout);
			printf("\t\n ModPartDesc10Dup::[%s]",ModPartDesc10Dup);fflush(stdout);
			printf("\t\n ModPartNoDup::[%s]",ModPartNoDup);fflush(stdout);
			printf("\t\n ModPartNo1Dup::[%s]",ModPartNo1Dup);fflush(stdout);
			printf("\t\n ModPartNo2Dup::[%s]",ModPartNo2Dup);fflush(stdout);
			printf("\t\n ModPartNo3Dup::[%s]",ModPartNo3Dup);fflush(stdout);
			printf("\t\n ModPartNo4Dup::[%s]",ModPartNo4Dup);fflush(stdout);
			printf("\t\n TOModTypeDup::[%s]",TOModTypeDup);fflush(stdout);
			printf("\t\n TOModType1Dup::[%s]",TOModType1Dup);fflush(stdout);
			printf("\t\n TOModType2Dup::[%s]",TOModType2Dup);fflush(stdout);
			printf("\t\n TOModType3Dup::[%s]",TOModType3Dup);fflush(stdout);
			printf("\t\n TOModType4Dup::[%s]",TOModType4Dup);fflush(stdout);
			printf("\t\n TOModType5Dup::[%s]",TOModType5Dup);fflush(stdout);
			printf("\t\n TOModType6Dup::[%s]",TOModType6Dup);fflush(stdout);
			printf("\t\n TOModType7Dup::[%s]",TOModType7Dup);fflush(stdout);
			printf("\t\n TOModType8Dup::[%s]",TOModType8Dup);fflush(stdout);
			printf("\t\n TOModType9Dup::[%s]",TOModType9Dup);fflush(stdout);
			printf("\t\n PartDesc1Dup::[%s]",PartDesc1Dup);fflush(stdout);
			printf("\t\n PartDesc2Dup::[%s]",PartDesc2Dup);fflush(stdout);
			printf("\t\n PartDesc3Dup::[%s]",PartDesc3Dup);fflush(stdout);
			printf("\t\n PartDesc4Dup::[%s]",PartDesc4Dup);fflush(stdout);
			printf("\t\n PartDesc5Dup::[%s]",PartDesc5Dup);fflush(stdout);
			printf("\t\n PartDesc6Dup::[%s]",PartDesc6Dup);fflush(stdout);
			printf("\t\n PartDesc7Dup::[%s]",PartDesc7Dup);fflush(stdout);
			printf("\t\n PartDesc8Dup::[%s]",PartDesc8Dup);fflush(stdout);
			printf("\t\n PartDesc9Dup::[%s]",PartDesc9Dup);fflush(stdout);
			printf("\t\n PartDesc10Dup::[%s]",PartDesc10Dup);fflush(stdout);
			printf("\t\n TOPartNo1Dup::[%s]",TOPartNo1Dup);fflush(stdout);
			printf("\t\n TOPartNo2Dup::[%s]",TOPartNo2Dup);fflush(stdout);
			printf("\t\n TOPartNo3Dup::[%s]",TOPartNo3Dup);fflush(stdout);
			printf("\t\n TOPartNo4Dup::[%s]",TOPartNo4Dup);fflush(stdout);
			printf("\t\n TOPartNo5Dup::[%s]",TOPartNo5Dup);fflush(stdout);
			printf("\t\n TOPartNo6Dup::[%s]",TOPartNo6Dup);fflush(stdout);
			printf("\t\n TOPartNo7Dup::[%s]",TOPartNo7Dup);fflush(stdout);
			printf("\t\n TOPartNo8Dup::[%s]",TOPartNo8Dup);fflush(stdout);
			printf("\t\n TOPartNo9Dup::[%s]",TOPartNo9Dup);fflush(stdout);
			printf("\t\n TOPartNo10Dup::[%s]",TOPartNo10Dup);fflush(stdout);
			printf("\t\n TOPartRev1Dup::[%s]",TOPartRev1Dup);fflush(stdout);
			printf("\t\n TOPartRev2Dup::[%s]",TOPartRev2Dup);fflush(stdout);
			printf("\t\n TOPartRev3Dup::[%s]",TOPartRev3Dup);fflush(stdout);
			printf("\t\n TOPartRev4Dup::[%s]",TOPartRev4Dup);fflush(stdout);
			printf("\t\n TOPartRev5Dup::[%s]",TOPartRev5Dup);fflush(stdout);
			printf("\t\n TOPartRev6Dup::[%s]",TOPartRev6Dup);fflush(stdout);
			printf("\t\n TOPartRev7Dup::[%s]",TOPartRev7Dup);fflush(stdout);
			printf("\t\n TOPartRev8Dup::[%s]",TOPartRev8Dup);fflush(stdout);
			printf("\t\n TOPartRev9Dup::[%s]",TOPartRev9Dup);fflush(stdout);
			printf("\t\n TOPartRev10Dup::[%s]",TOPartRev10Dup);fflush(stdout);
			printf("\t\n TOPartSeq1Dup::[%s]",TOPartSeq1Dup);fflush(stdout);
			printf("\t\n TOPartSeq2Dup::[%s]",TOPartSeq2Dup);fflush(stdout);
			printf("\t\n TOPartSeq3Dup::[%s]",TOPartSeq3Dup);fflush(stdout);
			printf("\t\n TOPartSeq4Dup::[%s]",TOPartSeq4Dup);fflush(stdout);
			printf("\t\n TOPartSeq5Dup::[%s]",TOPartSeq5Dup);fflush(stdout);
			printf("\t\n TOPartSeq6Dup::[%s]",TOPartSeq6Dup);fflush(stdout);
			printf("\t\n TOPartSeq7Dup::[%s]",TOPartSeq7Dup);fflush(stdout);
			printf("\t\n TOPartSeq8Dup::[%s]",TOPartSeq8Dup);fflush(stdout);
			printf("\t\n TOPartSeq9Dup::[%s]",TOPartSeq9Dup);fflush(stdout);
			printf("\t\n TOPartSeq10Dup::[%s]",TOPartSeq10Dup);fflush(stdout);
			printf("\t\n TOPhotosDup::[%s]",TOPhotosDup);fflush(stdout);
			printf("\t\n TOQAHeadDup::[%s]",TOQAHeadDup);fflush(stdout);
			printf("\t\n TOQAHeadcommentsDup::[%s]",TOQAHeadcommentsDup);fflush(stdout);
			printf("\t\n TOQAReviewerDup::[%s]",TOQAReviewerDup);fflush(stdout);
			printf("\t\n TOQAcommentsDup::[%s]",TOQAcommentsDup);fflush(stdout);
			printf("\t\n TOReasonDup::[%s]",TOReasonDup);fflush(stdout);
			printf("\t\n TORefNoDup::[%s]",TORefNoDup);fflush(stdout);
			printf("\t\n TORefNo2Dup::[%s]",TORefNo2Dup);fflush(stdout);
			printf("\t\n TORefNo3Dup::[%s]",TORefNo3Dup);fflush(stdout);
			printf("\t\n TORefNo4Dup::[%s]",TORefNo4Dup);fflush(stdout);
			printf("\t\n TORefNo5Dup::[%s]",TORefNo5Dup);fflush(stdout);
			printf("\t\n TORefNo6Dup::[%s]",TORefNo6Dup);fflush(stdout);
			printf("\t\n TORefNo7Dup::[%s]",TORefNo7Dup);fflush(stdout);
			printf("\t\n TORefNo8Dup::[%s]",TORefNo8Dup);fflush(stdout);
			printf("\t\n TORefNo9Dup::[%s]",TORefNo9Dup);fflush(stdout);
			printf("\t\n TORefNo10Dup::[%s]",TORefNo10Dup);fflush(stdout);
			printf("\t\n TOSCMReviewerDup::[%s]",TOSCMReviewerDup);fflush(stdout);
			printf("\t\n TOSTDReviewerDup::[%s]",TOSTDReviewerDup);fflush(stdout);
			printf("\t\n TOSubAreaDup::[%s]",TOSubAreaDup);fflush(stdout);
			printf("\t\n SubAreaCountDup::[%s]",SubAreaCountDup);fflush(stdout);
			printf("\t\n TOTSReviewerDup::[%s]",TOTSReviewerDup);fflush(stdout);
			printf("\t\n TOTScommentsDup::[%s]",TOTScommentsDup);fflush(stdout);
			printf("\t\n TOTestDetailsDup::[%s]",TOTestDetailsDup);fflush(stdout);
			printf("\t\n TOTryOutDtDup::[%s]",TOTryOutDtDup);fflush(stdout);
			printf("\t\n TOVDReviewerDup::[%s]",TOVDReviewerDup);fflush(stdout);
			printf("\t\n TOVDcommentsDup::[%s]",TOVDcommentsDup);fflush(stdout);
			printf("\t\n TOVQAReviewerDup::[%s]",TOVQAReviewerDup);fflush(stdout);
			printf("\t\n TOVQAcommentsDup::[%s]",TOVQAcommentsDup);fflush(stdout);
			printf("\t\n TOVehNoDup::[%s]",TOVehNoDup);fflush(stdout);
			printf("\t\n VehNoCountDup::[%s]",VehNoCountDup);fflush(stdout);
			printf("\t\n TOVenLoginDup::[%s]",TOVenLoginDup);fflush(stdout);
			printf("\t\n TOVenNameDup::[%s]",TOVenNameDup);fflush(stdout);
			printf("\t\n TOVinNoDup::[%s]",TOVinNoDup);fflush(stdout);
			printf("\t\n TOVinNo1Dup::[%s]",TOVinNo1Dup);fflush(stdout);
			printf("\t\n TOVinNo2Dup::[%s]",TOVinNo2Dup);fflush(stdout);
			printf("\t\n TOVinNo3Dup::[%s]",TOVinNo3Dup);fflush(stdout);
			printf("\t\n TOVinNo4Dup::[%s]",TOVinNo4Dup);fflush(stdout);
			printf("\t\n TOhlgreqDup::[%s]",TOhlgreqDup);fflush(stdout);
			printf("\t\n ESLstModByDup::[%s]",ESLstModByDup);fflush(stdout);
			printf("\t\n CreDatestr::[%s]",CreDatestr);fflush(stdout);
			printf("\t\n LstUpdstr::[%s]",LstUpdstr);fflush(stdout);
			printf("\t\n CrtrDup::[%s]",CrtrDup);fflush(stdout);
			printf("\t\n LCS::[%s]",LCS);fflush(stdout);

			if(TryOutNoDup==NULL || strcmp(TryOutNoDup,"")==0)
			{
				fprintf(fperror,"\n No Tryout Exists so not able to create ES");
				FlagERR=1;
			}

			printf("\n FlagERR:[%d] ",FlagERR);fflush(stdout);

			if(FlagERR==0)
			{
			tag_t	*esclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=1;
			int es_tags_found=0;

			ITK_CALL(QRY_find("tm_GetLatestTryoutQry", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[4] = {"ID"},
			*qry_valueses[4] =  {TryOutNoDup};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &es_tags_found, &esclass));
			printf("\n es_tags_found %d", es_tags_found);fflush(stdout);

			if(es_tags_found==0)
			{
					printf("\n Creating new Tryout\n");fflush(stdout);

					ITK_CALL(ITEM_create_item(TryOutNoDup,TryOutNoDup,"T5_Tryout","A",&item,&rev));
					
					printf("\n after ITEM_create_item tryout\n");fflush(stdout);

					//setAttributesOnES(&item,t5PEPlantName,t5EstShop,t5PEProcessEDNNo,t5PEPartDesc);

					setAttributesOnTryoutRev(&rev,AffshopsDup, AggregateDup, BlockDup, PlantDup, AreaDup, AreaCountDup ,BatchSizeDup, ChnclByDup, ChnclcommentsDup, ClnclDtDup, CrecommentsDup, CycleTimeDup, DaviationDup,
											  DaviationDescDup, DmlNo1Dup, DmlNo2Dup, DmlNo3Dup, DmlNo4Dup, DmlNo5Dup, DmlNo6Dup, DmlNo7Dup, DmlNo8Dup, DmlNo9Dup, DmlNo10Dup, FactHeadDup, FeedbackDup, FitmentDup, 
						                      IntVinNo1Dup, IntVinNo2Dup, IntVinNo3Dup, IntVinNo4Dup, IntVinNo5Dup, IntVinNo6Dup, IntVinNo7Dup, IntVinNo8Dup, IntVinNo9Dup, IntVinNo10Dup, IsDMLDup, IsEPADup, LocationDup, 
						                      ManReviewerDup, ModPartDescDup, ModPartDesc1Dup, ModPartDesc2Dup, ModPartDesc3Dup, ModPartDesc4Dup, ModPartDesc6Dup, ModPartDesc7Dup, ModPartDesc8Dup, ModPartDesc9Dup, 
						                      ModPartDesc10Dup, ModPartNoDup, ModPartNo1Dup, ModPartNo2Dup, ModPartNo3Dup, ModPartNo4Dup, TOModTypeDup, TOModType1Dup, TOModType2Dup, TOModType3Dup, TOModType4Dup, 
						                      TOModType5Dup, TOModType6Dup, TOModType7Dup, TOModType8Dup, TOModType9Dup, PartDesc1Dup, PartDesc2Dup, PartDesc3Dup, PartDesc4Dup, PartDesc5Dup, PartDesc6Dup, PartDesc7Dup, 
						                      PartDesc8Dup, PartDesc9Dup, PartDesc10Dup, TOPartNo1Dup, TOPartNo2Dup, TOPartNo3Dup, TOPartNo4Dup, TOPartNo5Dup, TOPartNo6Dup, TOPartNo7Dup, TOPartNo8Dup, TOPartNo9Dup, 
						                      TOPartNo10Dup, TOPartRev1Dup, TOPartRev2Dup, TOPartRev3Dup, TOPartRev4Dup, TOPartRev5Dup, TOPartRev6Dup, TOPartRev7Dup, TOPartRev8Dup, TOPartRev9Dup, TOPartRev10Dup,
                                              TOPartSeq1Dup, TOPartSeq2Dup, TOPartSeq3Dup, TOPartSeq4Dup, TOPartSeq5Dup, TOPartSeq6Dup, TOPartSeq7Dup, TOPartSeq8Dup, TOPartSeq9Dup, TOPartSeq10Dup, TOPhotosDup, TOQAHeadDup,
						                      TOQAHeadcommentsDup, TOQAReviewerDup, TOQAcommentsDup, TOReasonDup, TORefNoDup, TORefNo2Dup, TORefNo3Dup, TORefNo4Dup, TORefNo5Dup, TORefNo6Dup, TORefNo7Dup, TORefNo8Dup, 
						                      TORefNo9Dup, TORefNo10Dup, TOSCMReviewerDup, TOSTDReviewerDup, TOSubAreaDup, SubAreaCountDup, TOTSReviewerDup, TOTScommentsDup, TOTestDetailsDup, TOTryOutDtDup, TOVDReviewerDup, TOVDcommentsDup, 
						                      TOVQAReviewerDup, TOVQAcommentsDup, TOVehNoDup, VehNoCountDup,TOVenLoginDup, TOVenNameDup, TOVinNoDup, TOVinNo1Dup, TOVinNo2Dup, TOVinNo3Dup, TOVinNo4Dup, TOhlgreqDup, ESLstModByDup, 
						                      CreDatestr, LstUpdstr, CrtrDup, LCS);

					CreateRelTryoutToEPA(&rev,TORefNoDup, TORefNo2Dup, TORefNo3Dup, TORefNo4Dup, TORefNo5Dup, TORefNo6Dup, TORefNo7Dup, TORefNo8Dup, TORefNo9Dup, TORefNo10Dup,fperror);
					
			}
			else
			{
				printf("\n Tryout Already exists/....\n");
				fprintf(fperror,"Tryout Already exists for :[%s]\n",TryOutNoDup);
			}
		  }
		}
		return status;
	}
	ITK_CALL(POM_logout(false));
	if(fperror)fclose(fperror);fperror=NULL;
	return status;
}


// CreateTryout -u=aplloader -pf=/home/cmitest/shells/Admin/aplloader.pwf -g=dba -i=Tryout.txt
// CreateTryout -u=aplloader -p=XYT1ESA -g=dba -i=ESData.txt  // TCUAADEV

/*
Need to add below properties in datamodel of tryout.
	if(t5LastModBy!=NULL && strcmp(t5LastModBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_LastModBy",t5LastModBy));
	if(Creator!=NULL && strcmp(Creator,"")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_Creator",Creator));
	// T5_Tryout_EPA_Relation
	// T5_EPA_Tryout_Relation


TryOutNoDup
AffshopsDup
AggregateDup
BlockDup
PlantDup
******AreaDup
BatchSizeDup
ChnclByDup
ChnclcommentsDup
ClnclDtDup
CrecommentsDup
CycleTimeDup
DaviationDup
DaviationDescDup
DmlNo1Dup
DmlNo2Dup
DmlNo3Dup
DmlNo4Dup
DmlNo5Dup
DmlNo6Dup
DmlNo7Dup
DmlNo8Dup
DmlNo9Dup
DmlNo10Dup
FactHeadDup
FeedbackDup
FitmentDup
IntVinNo1Dup
IntVinNo2Dup
IntVinNo3Dup
IntVinNo4Dup
IntVinNo5Dup
IntVinNo6Dup
IntVinNo7Dup
IntVinNo8Dup
IntVinNo9Dup
IntVinNo10Dup
IsDMLDup
IsEPADup
LocationDup
ManReviewerDup
ModPartDescDup
ModPartDesc1Dup
ModPartDesc2Dup
ModPartDesc3Dup
ModPartDesc4Dup
ModPartDesc6Dup
ModPartDesc7Dup
ModPartDesc8Dup
ModPartDesc9Dup
ModPartDesc10Dup
ModPartNoDup
ModPartNo1Dup
ModPartNo2Dup
ModPartNo3Dup
ModPartNo4Dup
TOModTypeDup
TOModType1Dup
TOModType2Dup
TOModType3Dup
TOModType4Dup
TOModType5Dup
TOModType6Dup
TOModType7Dup
TOModType8Dup
TOModType9Dup
PartDesc1Dup
PartDesc2Dup
PartDesc3Dup
PartDesc4Dup
PartDesc5Dup
PartDesc6Dup
PartDesc7Dup
PartDesc8Dup
PartDesc9Dup
PartDesc10Dup
TOPartNo1Dup
TOPartNo2Dup
TOPartNo3Dup
TOPartNo4Dup
TOPartNo5Dup
TOPartNo6Dup
TOPartNo7Dup
TOPartNo8Dup
TOPartNo9Dup
TOPartNo10Dup
TOPartRev1Dup
TOPartRev2Dup
TOPartRev3Dup
TOPartRev4Dup
TOPartRev5Dup
TOPartRev6Dup
TOPartRev7Dup
TOPartRev8Dup
TOPartRev9Dup
TOPartRev10Dup
TOPartSeq1Dup
TOPartSeq2Dup
TOPartSeq3Dup
TOPartSeq4Dup
TOPartSeq5Dup
TOPartSeq6Dup
TOPartSeq7Dup
TOPartSeq8Dup
TOPartSeq9Dup
TOPartSeq10Dup
TOPhotosDup
TOQAHeadDup
TOQAHeadcommentsDup
TOQAReviewerDup
TOQAcommentsDup
TOReasonDup
TORefNoDup
TORefNo2Dup
TORefNo3Dup
TORefNo4Dup
TORefNo5Dup
TORefNo6Dup
TORefNo7Dup
TORefNo8Dup
TORefNo9Dup
TORefNo10Dup
TOSCMReviewerDup
TOSTDReviewerDup
TOSubAreaDup
TOTSReviewerDup
TOTScommentsDup
TOTestDetailsDup
TOTryOutDtDup
TOVDReviewerDup
TOVDcommentsDup
TOVQAReviewerDup
TOVQAcommentsDup
TOVehNoDup
TOVenLoginDup
TOVenNameDup
TOVinNoDup
TOVinNo1Dup
TOVinNo2Dup
TOVinNo3Dup
TOVinNo4Dup
TOhlgreqDup
ESLstModByDup
CreDatestr
LstUpdstr
CrtrDup
*/

//Last Update
//Creator