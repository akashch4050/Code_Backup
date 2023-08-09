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


void getNextDateddmmyy(char *DatetoConvert,char cnextAccessDate[30])
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

    strDay =  strtok(DatetoConvert,"/"); //1
    cMonth  =  strtok(NULL,"'/'"); //2
  strYear   =  strtok(NULL,"'/'"); //3
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

int CreateRelESOPRRevtoESRev(tag_t* rev,tag_t esclass,char * PrtNm,char * t5PEPlantName,char * t5EstShop,char * t5PEProcessEDNNo,FILE *fperror)
{
	int status;
	int		ifail	=0;

			

			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;
		
			printf("\n PrtNm: [%s]",PrtNm);fflush(stdout);

				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_OperationToEstimateRel",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, esclass, relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n ES to Design relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_EstimateOprationRelation",&relation_type1));
				ITK_CALL(GRM_create_relation(esclass,*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n Design to ES relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(esclass,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));

			
				//fprintf(fperror,"\n Design [%s] doesn't Exists, So Relation With ES [%s],[%s],[%s],[%s] Is Not Created.\n",PrtNm,PrtNm,t5PEPlantName,t5EstShop,t5PEProcessEDNNo);
			
}

int setAttributesOnESOPR(tag_t* item,char * EstimateNumberr)
{
	int status;
	int		ifail	=0;

	ITK_CALL(AOM_lock(*item));


	ITK_CALL ( AOM_set_value_string(*item,"t5_EstimateNumber",EstimateNumberr));
	ITK_CALL(AOM_save(*item));
	ITK_CALL(AOM_unlock(*item));

}

int setAttributesOnESOPRRev(tag_t* rev,char *EPOCHCostDriver,char *EPOCHNoOfCoats,char *EPOCHPEBCNo,char *EPOCHQty,
char *EPOCHUom,char *EPOCHValue,char *t5APLCalTime,char *t5APLMCDesc,char *t5BCOpnDesc,char *t5CpStatus,char *t5EqpDtl,char *t5EstLine,char *t5EstPFMEAReqd,char *t5EstStation,
char *t5EstcntrlReqd,char *t5OpnCylTm,char *t5OpnRemark,char *t5PEAllowanceOne,char *t5PEAllowanceTwo,char *t5PEAltRoute,char *t5PEAplGangSize,char *t5PEAplMcNo,
char *t5PEAplTimeType,char *t5PEBCNo,char *t5PEDPECT,char *t5PEDPECW,char *t5PEFrequency,char *t5PEGangSize,char *t5PEHandlingTime,char *t5PEInternalLabTime,char *t5PEMMAInd,
char *t5PEMachiningTime,char *t5PEMostflag,char *t5PEMstDivision,char *t5PEMstFrequency,char *t5PEMstMen,char *t5PEMstonoff,char *t5PEOpnDesc,char *t5PEOpnDrgNo,
char *t5PEOpnDrgStatus,char *t5PEOpnLDesc,char *t5PEPFMEADate,char *t5PEPFMEANO,char *t5PEPFMEARev,char *t5PEPartDesc,char *t5PEPartPerCycle,char *t5PEPlantCode,
char *t5PEPlantDesc,char *t5PEProcessEDNNoOpr,char *t5PEProsheet,char *t5PESUBOpnNo,char *t5PESetUpTime,char *t5PETactTime,char *t5PETimeOpnApl,char *t5PETimeOpnPsd,
char *t5PETradeCode,char *t5PSApprovBy,char *t5PSDCalTime,char *t5PSDocNo,char *t5PSDont,char *t5PSDos,char *t5PSIssuDate,char *t5PSModel,char *t5PSPrepBy,char *t5PSProdDate,
char *t5PSRevDate,char *t5PSRevNo,char *t5PSReviewBy,char *t5PfmeaStatus,char *t5QAApprovBy,char *t5QADate,char *t5QAPrepBy,
char *t5QAReviewBy,char *t5RawParts,char *t5SftParam,char *t5Spotplan,char *t5EstProcessReqd,char *CreDate11,char *LastUpdDate,char *Creator,char *LstModBy)
{
	int status;
	double dweight=0;
	tag_t projobj=NULLTAG;
	tag_t user_tag=NULLTAG;
	char* username=NULL;
	char FrmNextDate[20]={0};
	char QADate[20]={0};
	char PSProdDate[20]={0};
	char PSRevDate[20]={0};
	char CreDate[20]={0};
	char LstUpdDate[20]={0};
	date_t Release_date;
	date_t QADate1;
	date_t PSProdDate1;
	date_t PSRevDate1;
	date_t CreationDt;
	date_t LstUpdDt;

	ITK_CALL(AOM_lock(*rev));


	if(EPOCHCostDriver!=NULL && strcmp(EPOCHCostDriver,"")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_EPOCHCostDriver",EPOCHCostDriver));
	if(EPOCHNoOfCoats!=NULL && strcmp(EPOCHNoOfCoats,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EPOCHNoOfCoats",EPOCHNoOfCoats));
	if(EPOCHPEBCNo!=NULL && strcmp(EPOCHPEBCNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EPOCHPEBCNo",EPOCHPEBCNo));
	if(EPOCHQty!=NULL && strcmp(EPOCHQty,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EPOCHQty",EPOCHQty));
	if(EPOCHUom!=NULL && strcmp(EPOCHUom,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EPOCHUom",EPOCHUom));
	if(EPOCHValue!=NULL && strcmp(EPOCHValue,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EPOCHValue",EPOCHValue));
	if(t5APLCalTime!=NULL && strcmp(t5APLCalTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_APLCalTime",t5APLCalTime));
	if(t5APLMCDesc!=NULL && strcmp(t5APLMCDesc,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_APLMCDesc",t5APLMCDesc));
	if(t5BCOpnDesc!=NULL && strcmp(t5BCOpnDesc,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_BCOpnDesc",t5BCOpnDesc));


	if(t5CpStatus!=NULL && strcmp(t5CpStatus,"")!=0) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(t5CpStatus,"+")==0 || tc_strcmp(t5CpStatus,"Y")==0 )
		{
			printf("\n test5");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_CpStatus",true));
		}

		if (tc_strcmp(t5CpStatus,"-")==0 || tc_strcmp(t5CpStatus,"N")==0 )
		{
			printf("\n test6");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_CpStatus",false));
		}
	}
		
	if(t5EstPFMEAReqd!=NULL && strcmp(t5EstPFMEAReqd,"")!=0) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(t5EstPFMEAReqd,"+")==0 || tc_strcmp(t5EstPFMEAReqd,"Y")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_EstPFMEAReqd",true));
		}

		if (tc_strcmp(t5EstPFMEAReqd,"-")==0 || tc_strcmp(t5EstPFMEAReqd,"N")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_EstPFMEAReqd",false));
		}
	}

	if(t5EstcntrlReqd!=NULL && strcmp(t5EstcntrlReqd,"")!=0 ) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(t5EstcntrlReqd,"+")==0 || tc_strcmp(t5EstcntrlReqd,"Y")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_EstcntrlReqd",true));
		}
		if (tc_strcmp(t5EstcntrlReqd,"-")==0 || tc_strcmp(t5EstcntrlReqd,"N")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_EstcntrlReqd",false));
		}
	}


	if(t5EstProcessReqd!=NULL && strcmp(t5EstProcessReqd,"")!=0 ) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(t5EstProcessReqd,"+")==0 || tc_strcmp(t5EstProcessReqd,"Y")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_EstProcessReqd",true));
		}
		if (tc_strcmp(t5EstProcessReqd,"-")==0 || tc_strcmp(t5EstProcessReqd,"N")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_EstcntrlReqd",false));
		}
	}


	if(t5PEMostflag!=NULL && strcmp(t5PEMostflag,"")!=0 ) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(t5PEMostflag,"+")==0 || tc_strcmp(t5PEMostflag,"Y")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PEMostflag",true));
		}
		if (tc_strcmp(t5PEMostflag,"-")==0 || tc_strcmp(t5PEMostflag,"N")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PEMostflag",false));
		}
	}


	if(t5PEProsheet!=NULL && strcmp(t5PEProsheet,"")!=0) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(t5PEProsheet,"+")==0 || tc_strcmp(t5PEProsheet,"Y")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PEProsheet",true));
		}
		if (tc_strcmp(t5PEProsheet,"-")==0 || tc_strcmp(t5PEProsheet,"N")==0 )
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PEProsheet",false));
		}
	}


	if(t5PfmeaStatus!=NULL && strcmp(t5PfmeaStatus,"")!=0 ) 
		{
			printf("\n test4");fflush(stdout); 
			if (tc_strcmp(t5PfmeaStatus,"+")==0 || tc_strcmp(t5PfmeaStatus,"Y")==0 )
			{
				ITK_CALL ( AOM_set_value_logical(*rev,"t5_PfmeaStatus",true));
			}
			if (tc_strcmp(t5PfmeaStatus,"-")==0 || tc_strcmp(t5PfmeaStatus,"N")==0 )
			{
				ITK_CALL ( AOM_set_value_logical(*rev,"t5_PfmeaStatus",false));
			}
		}


	if(t5PEPFMEADate!=NULL && strcmp(t5PEPFMEADate,"")!=0)
		{
			getNextDate(t5PEPFMEADate,FrmNextDate);
			printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);
			ITK_CALL(ITK_string_to_date(FrmNextDate, &Release_date ));
			if(FrmNextDate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"t5_PEPFMEADate",Release_date));
		}
	printf("\n after set t5_PEPFMEADate");fflush(stdout);

	if(t5QADate!=NULL && strcmp(t5QADate,"")!=0)
		{
			printf("\n inside here t5QADate: [%s]",t5QADate);fflush(stdout);
			getNextDate(t5QADate,QADate);
			printf("\n QADate:%s",QADate);fflush(stdout);
			ITK_CALL(ITK_string_to_date(QADate, &QADate1 ));
			if(QADate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"t5_QADate",QADate1));

		}
	printf("\n set t5_QADate");fflush(stdout);

	if(t5PSProdDate!=NULL && strcmp(t5PSProdDate,"")!=0 )
		{
			getNextDate(t5PSProdDate,PSProdDate);
			printf("\n PSProdDate:%s",PSProdDate);fflush(stdout);
			ITK_CALL(ITK_string_to_date(PSProdDate, &PSProdDate1 ));
			if(PSProdDate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"t5_PSProdDate",PSProdDate1));
		}
	printf("\n set t5_PSProdDate");fflush(stdout);


	if(t5PSRevDate!=NULL && strcmp(t5PSRevDate,"")!=0)
		{
			getNextDateddmmyy(t5PSRevDate,PSRevDate);
			printf("\n PSRevDate:%s",PSRevDate);fflush(stdout);
			ITK_CALL(ITK_string_to_date(PSRevDate, &PSRevDate1 ));
			if(PSRevDate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"t5_PSRevDate",PSRevDate1));
		}
	printf("\n set t5_PSRevDate");fflush(stdout);

	//if(t5PSIssuDate!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSIssuDate",t5PSIssuDate)); // no fix date format in TCE

	if(t5EstStation!=NULL && strcmp(t5EstStation,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EstStation",t5EstStation));
	if(t5EqpDtl!=NULL && strcmp(t5EqpDtl,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EqpDtl",t5EqpDtl));
	if(t5EstLine!=NULL && strcmp(t5EstLine,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EstLine",t5EstLine));
	if(t5OpnCylTm!=NULL && strcmp(t5OpnCylTm,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_OpnCylTm",t5OpnCylTm));
	if(t5OpnRemark!=NULL && strcmp(t5OpnRemark,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_OpnRemark",t5OpnRemark));
	if(t5PEAllowanceOne!=NULL && strcmp(t5PEAllowanceOne,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEAllowanceOne",t5PEAllowanceOne));
	if(t5PEAllowanceTwo!=NULL && strcmp(t5PEAllowanceTwo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEAllowanceTwo",t5PEAllowanceTwo));
	if(t5PEAltRoute!=NULL && strcmp(t5PEAltRoute,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEAltRoute",t5PEAltRoute));
	if(t5PEAplGangSize!=NULL && strcmp(t5PEAplGangSize,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEAplGangSize",t5PEAplGangSize));
	if(t5PEAplMcNo!=NULL && strcmp(t5PEAplMcNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEAplMcNo",t5PEAplMcNo));
	if(t5PEAplTimeType!=NULL && strcmp(t5PEAplTimeType,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEAplTimeType",t5PEAplTimeType));
	if(t5PEBCNo!=NULL && strcmp(t5PEBCNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEBCNo",t5PEBCNo));
	if(t5PEDPECT!=NULL && strcmp(t5PEDPECT,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEDPECT",t5PEDPECT));
	if(t5PEDPECW!=NULL && strcmp(t5PEDPECW,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEDPECW",t5PEDPECW));
	if(t5PEFrequency!=NULL && strcmp(t5PEFrequency,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEFrequency",t5PEFrequency));
	if(t5PEGangSize!=NULL && strcmp(t5PEGangSize,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEGangSize",t5PEGangSize));
	if(t5PEHandlingTime!=NULL && strcmp(t5PEHandlingTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEHandlingTime",t5PEHandlingTime));
	if(t5PEInternalLabTime!=NULL && strcmp(t5PEInternalLabTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEInternalLabTime",t5PEInternalLabTime));
	if(t5PEMMAInd!=NULL && strcmp(t5PEMMAInd,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEMMAInd",t5PEMMAInd));
	if(t5PEMachiningTime!=NULL && strcmp(t5PEMachiningTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEMachiningTime",t5PEMachiningTime));
	
	if(t5PEMstDivision!=NULL && strcmp(t5PEMstDivision,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEMstDivision",t5PEMstDivision));
	if(t5PEMstFrequency!=NULL && strcmp(t5PEMstFrequency,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEMstFrequency",t5PEMstFrequency));
	if(t5PEMstMen!=NULL && strcmp(t5PEMstMen,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEMstMen",t5PEMstMen));
	if(t5PEMstonoff!=NULL && strcmp(t5PEMstonoff,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEMstonoff",t5PEMstonoff));
	if(t5PEOpnDesc!=NULL && strcmp(t5PEOpnDesc,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEOpnDesc",t5PEOpnDesc));
	if(t5PEOpnDrgNo!=NULL && strcmp(t5PEOpnDrgNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEOpnDrgNo",t5PEOpnDrgNo));
	if(t5PEOpnDrgStatus!=NULL && strcmp(t5PEOpnDrgStatus,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEOpnDrgStatus",t5PEOpnDrgStatus));
	if(t5PEOpnLDesc!=NULL && strcmp(t5PEOpnLDesc,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEOpnLDesc",t5PEOpnLDesc));
	if(t5PEPFMEANO!=NULL && strcmp(t5PEPFMEANO,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEPFMEANO",t5PEPFMEANO));
	if(t5PEPFMEARev!=NULL && strcmp(t5PEPFMEARev,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEPFMEARev",t5PEPFMEARev));
	if(t5PEPartDesc!=NULL && strcmp(t5PEPartDesc,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEPartDesc",t5PEPartDesc));
	if(t5PEPartPerCycle!=NULL && strcmp(t5PEPartPerCycle,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEPartPerCycle",t5PEPartPerCycle));
	if(t5PEPlantCode!=NULL && strcmp(t5PEPlantCode,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEPlantCode",t5PEPlantCode));
	if(t5PEPlantDesc!=NULL && strcmp(t5PEPlantDesc,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEPlantDesc",t5PEPlantDesc));
	if(t5PEProcessEDNNoOpr!=NULL && strcmp(t5PEProcessEDNNoOpr,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEProcessEDNNo",t5PEProcessEDNNoOpr));
	
	if(t5PESUBOpnNo!=NULL && strcmp(t5PESUBOpnNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PESUBOpnNo",t5PESUBOpnNo));
	if(t5PESetUpTime!=NULL && strcmp(t5PESetUpTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PESetUpTime",t5PESetUpTime));
	if(t5PETactTime!=NULL && strcmp(t5PETactTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PETactTime",t5PETactTime));
	if(t5PETimeOpnApl!=NULL && strcmp(t5PETimeOpnApl,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PETimeOpnApl",t5PETimeOpnApl));
	if(t5PETimeOpnPsd!=NULL && strcmp(t5PETimeOpnPsd,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PETimeOpnPsd",t5PETimeOpnPsd));
	if(t5PETradeCode!=NULL && strcmp(t5PETradeCode,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PETradeCode",t5PETradeCode));
	if(t5PSApprovBy!=NULL && strcmp(t5PSApprovBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSApprovBy",t5PSApprovBy));
	if(t5PSDCalTime!=NULL && strcmp(t5PSDCalTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSDCalTime",t5PSDCalTime));
	if(t5PSDocNo!=NULL && strcmp(t5PSDocNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSDocNo",t5PSDocNo));
	if(t5PSDont!=NULL && strcmp(t5PSDont,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSDont",t5PSDont));
	if(t5PSDos!=NULL && strcmp(t5PSDos,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSDos",t5PSDos));
	if(t5PSModel!=NULL && strcmp(t5PSModel,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSModel",t5PSModel));
	if(t5PSPrepBy!=NULL && strcmp(t5PSPrepBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSPrepBy",t5PSPrepBy));
	if(t5PSRevNo!=NULL && strcmp(t5PSRevNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSRevNo",t5PSRevNo));
	if(t5PSReviewBy!=NULL && strcmp(t5PSReviewBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSReviewBy",t5PSReviewBy));
	
	if(t5QAApprovBy!=NULL && strcmp(t5QAApprovBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_QAApprovBy",t5QAApprovBy));
	if(t5QAPrepBy!=NULL && strcmp(t5QAPrepBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_QAPrepBy",t5QAPrepBy));
	if(t5QAReviewBy!=NULL && strcmp(t5QAReviewBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_QAReviewBy",t5QAReviewBy));
	if(t5RawParts!=NULL && strcmp(t5RawParts,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_RawParts",t5RawParts));
	if(t5SftParam!=NULL && strcmp(t5SftParam,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_SftParam",t5SftParam));
	if(t5Spotplan!=NULL && strcmp(t5Spotplan,"")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_Spotplan",t5Spotplan));

	printf("\n After setting all props");fflush(stdout);

	if(CreDate11!=NULL && strcmp(CreDate11,"")!=0)
	{
		getNextDate(CreDate11,CreDate);
		printf("\n CreDate:%s",CreDate);fflush(stdout);
		ITK_CALL(ITK_string_to_date(CreDate, &CreationDt));
		if(CreDate!=NULL) ITK_CALL ( POM_set_creation_date(*rev, CreationDt));
	}

	printf("\n Test111");fflush(stdout);

	logical bypass = true;
		ITK_CALL( ITK_set_bypass ( bypass ));
	if(LastUpdDate!=NULL && strcmp(LastUpdDate,"")!=0)
	{
		getNextDate(LastUpdDate,LstUpdDate);
		printf("\n LstUpdDate:%s",LstUpdDate);fflush(stdout);
		ITK_CALL(ITK_string_to_date(LstUpdDate, &LstUpdDt ));
		if(LstUpdDate!=NULL) ITK_CALL ( POM_set_modification_date(*rev, LstUpdDt));
		//if(LstUpdDate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"last_mod_date",LstUpdDate));
	}
   ITK_CALL(POM_set_env_info( POM_bypass_attr_update , false, 0, 0.0, NULLTAG, "" ) );

printf("\n Test222");fflush(stdout);


	if(Creator!=NULL && strcmp(Creator,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_Creator",Creator));
	if(LstModBy!=NULL && strcmp(LstModBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_LastModBy",LstModBy));


	ITK_CALL(AOM_save(*rev));
	ITK_CALL(AOM_unlock(*rev));

printf("\n Donee");fflush(stdout);


// Release Status// t5PEProcessEDNType
 char* lcsToset=NULL;
 logical retain=false;
 tag_t   status2=NULLTAG;
 char   *ReleaseStatus=NULL;

 lcsToset =(char *) MEM_alloc(20 * sizeof(char *));
 strcpy(lcsToset,"T5_PSDFinalReleased");

	ITK_CALL(CR_create_release_status(lcsToset,&status2));
	ITK_CALL(AOM_ask_name(status2, &ReleaseStatus));
	printf("\n ******************* ReleaseStatus: %s\n",ReleaseStatus);fflush(stdout);
	ITK_CALL(EPM_add_release_status(status2,1,rev,retain));

}

int setLifeCycle(tag_t* itemRev,char* lifeCycleS)
{
		int status;
		tag_t   status2=NULLTAG;
		logical retain=false;
		char* lcsToset=NULL;

		lcsToset =(char *) MEM_alloc(20 * sizeof(char *));

		if(strcmp(lifeCycleS,"LcsReview")==0){strcpy(lcsToset,ERCREVIEW);}
		else if(strcmp(lifeCycleS,"LcsWorking")==0){strcpy(lcsToset,ERCWORKING);}
		else if(strcmp(lifeCycleS,"LcsErcRlzd")==0){strcpy(lcsToset,ERCRELEASED);}
		else if(strcmp(lifeCycleS,"LcsAPLWrkg")==0){strcpy(lcsToset,APLWORKING);}
		else if(strcmp(lifeCycleS,"LcsAplRlzd")==0){strcpy(lcsToset,APLRELEASED);}
		else if(strcmp(lifeCycleS,"LcsSTDWrkg")==0){strcpy(lcsToset,STDWORKING);}
		else if(strcmp(lifeCycleS,"LcsSTDRlzd")==0)
		{
			printf("\n This is std rel...");
			strcpy(lcsToset,STDRELEASED);
		}

		ITK_CALL(CR_create_release_status(lcsToset,&status2));
		ITK_CALL(EPM_add_release_status(status2,1,itemRev,retain));

		//ITK_CALL(AOM_save(*itemRev));
		//ITK_CALL(AOM_unlock(*itemRev));
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


char*  t5PEPartNumber=NULL  ;
char*  Revision=NULL  ;
char*  Sequence=NULL  ;
char*  ClosureTimeStamp=NULL  ;
char*  WbsID=NULL  ;
char*  t5APLCalTime=NULL  ;
char*  t5EPCOHFlg=NULL  ;
char*  t5EstActor=NULL  ;
char*  t5EstApprover=NULL  ;
char*  t5EstShop=NULL  ;
char*  t5LastModBy=NULL  ;
char*  t5PEAgency=NULL  ;
char*  t5PECopyFrmMost=NULL  ;
char*  t5PECopyFrmPart=NULL  ;
char*  t5PEEDNRemark=NULL  ;
char*  t5PEEstCTType=NULL  ;
char*  t5PEEstCWType=NULL  ;
char*  t5PEEstShtType=NULL  ;
char*  t5PEModfReason=NULL  ;
char*  t5PEModfRff=NULL  ;
char*  t5PEMostflag=NULL  ;
char*  t5PEMstPreparedBy=NULL  ;
char*  t5PEPartDesc=NULL  ;
char*  t5PEPlannedBy=NULL  ;
char*  t5PEPlantName=NULL  ;
char*  t5PEProcessApproveBy=NULL  ;
char*  t5PEProcessEDNNo=NULL  ;
char*  t5PEProcessEDNType=NULL  ;
char*  t5PEShopName=NULL  ;
char*  t5PETimeOpnApl=NULL  ;
char*  t5PETimeOpnPsd=NULL  ;
char*  t5PEVehicleID=NULL  ;
char*  t5PSDCalTime=NULL  ;
char*  t5PfdStatus=NULL  ;
char*  EPOCHCostDriver=NULL  ;
char*  EPOCHNoOfCoats=NULL  ;
char*   EPOCHPEBCNo=NULL;
char*   EPOCHQty=NULL;
char*   EPOCHUom=NULL;
char*   EPOCHValue=NULL;
char*   t5APLMCDesc=NULL;
char*   t5BCOpnDesc=NULL;
char*   t5CpStatus=NULL;
char*   t5EqpDtl=NULL;
char*   t5EstLine=NULL;
char*   t5EstPFMEAReqd=NULL;
char*   t5EstStation=NULL;
char*   t5EstcntrlReqd=NULL;
char*   t5OpnCylTm=NULL;
char*   t5OpnRemark=NULL;
char*   t5PEAllowanceOne=NULL;
char*   t5PEAllowanceTwo=NULL;
char*   t5PEAltRoute=NULL;
char*   t5PEAplGangSize=NULL;
char*   t5PEAplMcNo=NULL;
char*   t5PEAplTimeType=NULL;
char*   t5PEBCNo=NULL;
char*   t5PEDPECT=NULL;
char*   t5PEDPECW=NULL;
char*   t5PEFrequency=NULL;
char*   t5PEGangSize=NULL;
char*   t5PEHandlingTime=NULL;
char*   t5PEInternalLabTime=NULL;
char*   t5PEMMAInd=NULL;
char*   t5PEMachiningTime=NULL;
char*   t5PEMstDivision=NULL;
char*   t5PEMstFrequency=NULL;
char*   t5PEMstMen=NULL;
char*   t5PEMstonoff=NULL;
char*   t5PEOpnDesc=NULL;
char*   t5PEOpnDrgNo=NULL;
char*   t5PEOpnDrgStatus=NULL;
char*   t5PEOpnLDesc=NULL;
char*   t5PEOpnNumber=NULL;
char*   t5PEPFMEADate=NULL;
char*   t5PEPFMEANO=NULL;
char*   t5PEPFMEARev=NULL;
char*   t5PEPartNumberOpr=NULL;
char*   t5PEPartPerCycle=NULL;
char*   t5PEPlantCode=NULL;
char*   t5PEPlantDesc=NULL;
char*   t5PEProcessEDNNoOpr=NULL;
char*   t5PEProsheet=NULL;
char*   t5PESUBOpnNo=NULL;
char*   t5PESetUpTime=NULL;
char*   t5PETactTime=NULL;
char*   t5PETradeCode=NULL;
char*   t5PSApprovBy=NULL;
char*   t5PSDocNo=NULL;
char*   t5PSDont=NULL;
char*   t5PSDos=NULL;
char*   t5PSIssuDate=NULL;
char*   t5PSModel=NULL;
char*   t5PSPrepBy=NULL;
char*   t5PSProdDate=NULL;
char*   t5PSRevDate=NULL;
char*   t5PSRevNo=NULL;
char*   t5PSReviewBy=NULL;
char*   t5PfmeaStatus=NULL;
char*   t5QAApprovBy=NULL;
char*   t5QADate=NULL;
char*   t5QAPrepBy=NULL;
char*   t5QAReviewBy=NULL;
char*   t5RawParts=NULL;
char*   t5SftParam=NULL;
char*   t5Spotplan=NULL;
char*   t5EstProcessReqd=NULL;
char*   CreDate=NULL;
char*   LastUpdDate=NULL;
char*   Creator=NULL;
char*   LstModBy=NULL;
char	EstimateNumberr[100];


	char *szbom_revision_status = NULL;
	tag_t   window, window2, rule, item_tag = null_tag, top_line;

	inputfile = ITK_ask_cli_argument("-i=");
	//errorLogPath = ITK_ask_cli_argument("-ep=");

    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	ITK_CALL(ITK_auto_login( ));
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
	fperror=fopen("ES_Exception.log","a");
	if(fp!=NULL)
	{
		inputline=(char *) MEM_alloc(10000);
		while(fgets(inputline,10000,fp)!=NULL)
		{
			fputs(inputline,stdout);
			t5PEPartNumber=strtok(inputline,"^");  
			Revision=strtok(NULL,"^");  
			Sequence=strtok(NULL,"^");  
			t5EstShop=strtok(NULL,"^");  
			t5PEPlantName=strtok(NULL,"^");  
			t5PEProcessEDNNo=strtok(NULL,"^");  
			EPOCHCostDriver=strtok(NULL,"^");  
			EPOCHNoOfCoats=strtok(NULL,"^");  
			EPOCHPEBCNo=strtok(NULL,"^");  
			EPOCHQty=strtok(NULL,"^");  
			EPOCHUom=strtok(NULL,"^");  
			EPOCHValue=strtok(NULL,"^");  
			t5APLCalTime=strtok(NULL,"^");  
			t5APLMCDesc=strtok(NULL,"^");  
			t5BCOpnDesc=strtok(NULL,"^");  
			t5CpStatus=strtok(NULL,"^");  
			t5EqpDtl=strtok(NULL,"^");  
			t5EstLine=strtok(NULL,"^");  
			t5EstPFMEAReqd=strtok(NULL,"^");  
			t5EstStation=strtok(NULL,"^");  
			t5EstcntrlReqd=strtok(NULL,"^");  
			t5OpnCylTm=strtok(NULL,"^");  
			t5OpnRemark=strtok(NULL,"^");  
			t5PEAllowanceOne=strtok(NULL,"^");  
			t5PEAllowanceTwo=strtok(NULL,"^");  
			t5PEAltRoute=strtok(NULL,"^");  
			t5PEAplGangSize=strtok(NULL,"^");  
			t5PEAplMcNo=strtok(NULL,"^");  
			t5PEAplTimeType=strtok(NULL,"^");  
			t5PEBCNo=strtok(NULL,"^");  
			t5PEDPECT=strtok(NULL,"^");  
			t5PEDPECW=strtok(NULL,"^");  
			t5PEFrequency=strtok(NULL,"^");  
			t5PEGangSize=strtok(NULL,"^");  
			t5PEHandlingTime=strtok(NULL,"^");  
			t5PEInternalLabTime=strtok(NULL,"^");  
			t5PEMMAInd=strtok(NULL,"^");  
			t5PEMachiningTime=strtok(NULL,"^");  
			t5PEMostflag=strtok(NULL,"^");  
			t5PEMstDivision=strtok(NULL,"^");  
			t5PEMstFrequency=strtok(NULL,"^");  
			t5PEMstMen=strtok(NULL,"^");  
			t5PEMstonoff=strtok(NULL,"^");  
			t5PEOpnDesc=strtok(NULL,"^");  
			t5PEOpnDrgNo=strtok(NULL,"^");  
			t5PEOpnDrgStatus=strtok(NULL,"^");  
			t5PEOpnLDesc=strtok(NULL,"^");  
			t5PEOpnNumber=strtok(NULL,"^");  
			t5PEPFMEADate=strtok(NULL,"^");  
			t5PEPFMEANO=strtok(NULL,"^");  
			t5PEPFMEARev=strtok(NULL,"^");  
			t5PEPartDesc=strtok(NULL,"^");  
			t5PEPartNumberOpr=strtok(NULL,"^");  
			t5PEPartPerCycle=strtok(NULL,"^");  
			t5PEPlantCode=strtok(NULL,"^");  
			t5PEPlantDesc=strtok(NULL,"^");  
			t5PEProcessEDNNoOpr=strtok(NULL,"^");  
			t5PEProsheet=strtok(NULL,"^");  
			t5PESUBOpnNo=strtok(NULL,"^");  
			t5PESetUpTime=strtok(NULL,"^");  
			t5PETactTime=strtok(NULL,"^");  
			t5PETimeOpnApl=strtok(NULL,"^");  
			t5PETimeOpnPsd=strtok(NULL,"^");  
			t5PETradeCode=strtok(NULL,"^");  
			t5PSApprovBy=strtok(NULL,"^");  
			t5PSDCalTime=strtok(NULL,"^");  
			t5PSDocNo=strtok(NULL,"^");  
			t5PSDont=strtok(NULL,"^");  
			t5PSDos=strtok(NULL,"^");  
			t5PSIssuDate=strtok(NULL,"^");  
			t5PSModel=strtok(NULL,"^");  
			t5PSPrepBy=strtok(NULL,"^");  
			t5PSProdDate=strtok(NULL,"^");  
			t5PSRevDate=strtok(NULL,"^");  
			t5PSRevNo=strtok(NULL,"^");  
			t5PSReviewBy=strtok(NULL,"^");  
			t5PfmeaStatus=strtok(NULL,"^");  
			t5QAApprovBy=strtok(NULL,"^");  
			t5QADate=strtok(NULL,"^");  
			t5QAPrepBy=strtok(NULL,"^");  
			t5QAReviewBy=strtok(NULL,"^");  
			t5RawParts=strtok(NULL,"^");  
			t5SftParam=strtok(NULL,"^");  
			t5Spotplan=strtok(NULL,"^");  
			t5EstProcessReqd=strtok(NULL,"^");  
			CreDate=strtok(NULL,"^");  
			LastUpdDate=strtok(NULL,"^");  
			Creator=strtok(NULL,"^");  
			LstModBy=strtok(NULL,"^");  

			printf("\n BEFORE TRIM ");fflush(stdout);  
			printf("\n t5PEPartNumber=::[%s]",t5PEPartNumber);fflush(stdout);  
			printf("\n Revision=::[%s]",Revision);fflush(stdout);   
			printf("\n Sequence=::[%s]",Sequence);fflush(stdout);   
			printf("\n t5EstShop=::[%s]",t5EstShop);fflush(stdout);   
			printf("\n t5PEPlantName=::[%s]",t5PEPlantName);fflush(stdout);   
			printf("\n t5PEProcessEDNNo=::[%s]",t5PEProcessEDNNo);fflush(stdout);   
			printf("\n EPOCHCostDriver=::[%s]",EPOCHCostDriver);fflush(stdout);   
			printf("\n EPOCHNoOfCoats=::[%s]",EPOCHNoOfCoats);fflush(stdout);   
			printf("\n EPOCHPEBCNo=::[%s]",EPOCHPEBCNo);fflush(stdout);   
			printf("\n EPOCHQty=::[%s]",EPOCHQty);fflush(stdout);   
			printf("\n EPOCHUom=::[%s]",EPOCHUom);fflush(stdout);   
			printf("\n EPOCHValue=::[%s]",EPOCHValue);fflush(stdout);   
			printf("\n t5APLCalTime=::[%s]",t5APLCalTime);fflush(stdout);   
			printf("\n t5APLMCDesc=::[%s]",t5APLMCDesc);fflush(stdout);   
			printf("\n t5BCOpnDesc=::[%s]",t5BCOpnDesc);fflush(stdout);   
			printf("\n t5CpStatus=::[%s]",t5CpStatus);fflush(stdout);   
			printf("\n t5EqpDtl=::[%s]",t5EqpDtl);fflush(stdout);   
			printf("\n t5EstLine=::[%s]",t5EstLine);fflush(stdout);   
			printf("\n t5EstPFMEAReqd=::[%s]",t5EstPFMEAReqd);fflush(stdout);   
			printf("\n t5EstStation=::[%s]",t5EstStation);fflush(stdout);   
			printf("\n t5EstcntrlReqd=::[%s]",t5EstcntrlReqd);fflush(stdout);   
			printf("\n t5OpnCylTm=::[%s]",t5OpnCylTm);fflush(stdout);   
			printf("\n t5OpnRemark=::[%s]",t5OpnRemark);fflush(stdout);   
			printf("\n t5PEAllowanceOne=::[%s]",t5PEAllowanceOne);fflush(stdout);   
			printf("\n t5PEAllowanceTwo=::[%s]",t5PEAllowanceTwo);fflush(stdout);   
			printf("\n t5PEAltRoute=::[%s]",t5PEAltRoute);fflush(stdout);   
			printf("\n t5PEAplGangSize=::[%s]",t5PEAplGangSize);fflush(stdout);   
			printf("\n t5PEAplMcNo=::[%s]",t5PEAplMcNo);fflush(stdout);   
			printf("\n t5PEAplTimeType=::[%s]",t5PEAplTimeType);fflush(stdout);   
			printf("\n t5PEBCNo=::[%s]",t5PEBCNo);fflush(stdout);   
			printf("\n t5PEDPECT=::[%s]",t5PEDPECT);fflush(stdout);   
			printf("\n t5PEDPECW=::[%s]",t5PEDPECW);fflush(stdout);   
			printf("\n t5PEFrequency=::[%s]",t5PEFrequency);fflush(stdout);   
			printf("\n t5PEGangSize=::[%s]",t5PEGangSize);fflush(stdout);   
			printf("\n t5PEHandlingTime=::[%s]",t5PEHandlingTime);fflush(stdout);   
			printf("\n t5PEInternalLabTime=::[%s]",t5PEInternalLabTime);fflush(stdout);   
			printf("\n t5PEMMAInd=::[%s]",t5PEMMAInd);fflush(stdout);   
			printf("\n t5PEMachiningTime=::[%s]",t5PEMachiningTime);fflush(stdout);   
			printf("\n t5PEMostflag=::[%s]",t5PEMostflag);fflush(stdout);   
			printf("\n t5PEMstDivision=::[%s]",t5PEMstDivision);fflush(stdout);   
			printf("\n t5PEMstFrequency=::[%s]",t5PEMstFrequency);fflush(stdout);   
			printf("\n t5PEMstMen=::[%s]",t5PEMstMen);fflush(stdout);   
			printf("\n t5PEMstonoff=::[%s]",t5PEMstonoff);fflush(stdout);   
			printf("\n t5PEOpnDesc=::[%s]",t5PEOpnDesc);fflush(stdout);   
			printf("\n t5PEOpnDrgNo=::[%s]",t5PEOpnDrgNo);fflush(stdout);   
			printf("\n t5PEOpnDrgStatus=::[%s]",t5PEOpnDrgStatus);fflush(stdout);   
			printf("\n t5PEOpnLDesc=::[%s]",t5PEOpnLDesc);fflush(stdout);   
			printf("\n t5PEOpnNumber=::[%s]",t5PEOpnNumber);fflush(stdout);   
			printf("\n t5PEPFMEADate=::[%s]",t5PEPFMEADate);fflush(stdout);   
			printf("\n t5PEPFMEANO=::[%s]",t5PEPFMEANO);fflush(stdout);   
			printf("\n t5PEPFMEARev=::[%s]",t5PEPFMEARev);fflush(stdout);   
			printf("\n t5PEPartDesc=::[%s]",t5PEPartDesc);fflush(stdout);   
			printf("\n t5PEPartNumberOpr=::[%s]",t5PEPartNumberOpr);fflush(stdout);   
			printf("\n t5PEPartPerCycle=::[%s]",t5PEPartPerCycle);fflush(stdout);   
			printf("\n t5PEPlantCode=::[%s]",t5PEPlantCode);fflush(stdout);   
			printf("\n t5PEPlantDesc=::[%s]",t5PEPlantDesc);fflush(stdout);   
			printf("\n t5PEProcessEDNNoOpr=::[%s]",t5PEProcessEDNNoOpr);fflush(stdout);   
			printf("\n t5PEProsheet=::[%s]",t5PEProsheet);fflush(stdout);   
			printf("\n t5PESUBOpnNo=::[%s]",t5PESUBOpnNo);fflush(stdout);   
			printf("\n t5PESetUpTime=::[%s]",t5PESetUpTime);fflush(stdout);   
			printf("\n t5PETactTime=::[%s]",t5PETactTime);fflush(stdout);   
			printf("\n t5PETimeOpnApl=::[%s]",t5PETimeOpnApl);fflush(stdout);   
			printf("\n t5PETimeOpnPsd=::[%s]",t5PETimeOpnPsd);fflush(stdout);   
			printf("\n t5PETradeCode=::[%s]",t5PETradeCode);fflush(stdout);   
			printf("\n t5PSApprovBy=::[%s]",t5PSApprovBy);fflush(stdout);   
			printf("\n t5PSDCalTime=::[%s]",t5PSDCalTime);fflush(stdout);   
			printf("\n t5PSDocNo=::[%s]",t5PSDocNo);fflush(stdout);   
			printf("\n t5PSDont=::[%s]",t5PSDont);fflush(stdout);   
			printf("\n t5PSDos=::[%s]",t5PSDos);fflush(stdout);   
			printf("\n t5PSIssuDate=::[%s]",t5PSIssuDate);fflush(stdout);   
			printf("\n t5PSModel=::[%s]",t5PSModel);fflush(stdout);   
			printf("\n t5PSPrepBy=::[%s]",t5PSPrepBy);fflush(stdout);   
			printf("\n t5PSProdDate=::[%s]",t5PSProdDate);fflush(stdout);   
			printf("\n t5PSRevDate=::[%s]",t5PSRevDate);fflush(stdout);   
			printf("\n t5PSRevNo=::[%s]",t5PSRevNo);fflush(stdout);   
			printf("\n t5PSReviewBy=::[%s]",t5PSReviewBy);fflush(stdout);   
			printf("\n t5PfmeaStatus=::[%s]",t5PfmeaStatus);fflush(stdout);   
			printf("\n t5QAApprovBy=::[%s]",t5QAApprovBy);fflush(stdout);   
			printf("\n t5QADate=::[%s]",t5QADate);fflush(stdout);   
			printf("\n t5QAPrepBy=::[%s]",t5QAPrepBy);fflush(stdout);   
			printf("\n t5QAReviewBy=::[%s]",t5QAReviewBy);fflush(stdout);   
			printf("\n t5RawParts=::[%s]",t5RawParts);fflush(stdout);   
			printf("\n t5SftParam=::[%s]",t5SftParam);fflush(stdout);   
			printf("\n t5Spotplan=::[%s]",t5Spotplan);fflush(stdout);   
			printf("\n t5EstProcessReqd=::[%s]",t5EstProcessReqd);fflush(stdout);  
			printf("\n CreDate=::[%s]",CreDate);fflush(stdout);  
			printf("\n LastUpdDate=::[%s]",LastUpdDate);fflush(stdout);  
			printf("\n Creator=::[%s]",Creator);fflush(stdout);  
			printf("\n LstModBy=::[%s]",LstModBy);fflush(stdout);  


				StringTrim(&t5PEPartNumber);
				StringTrim(&Revision);
				StringTrim(&Sequence);
				StringTrim(&t5EstShop);
				StringTrim(&t5PEPlantName);
				StringTrim(&t5PEProcessEDNNo);
				StringTrim(&EPOCHCostDriver);
				StringTrim(&EPOCHNoOfCoats);
				StringTrim(&EPOCHPEBCNo);
				StringTrim(&EPOCHQty);
				StringTrim(&EPOCHUom);
				StringTrim(&EPOCHValue);
				StringTrim(&t5APLCalTime);
				StringTrim(&t5APLMCDesc);
				StringTrim(&t5BCOpnDesc);
				StringTrim(&t5CpStatus);
				StringTrim(&t5EqpDtl);
				StringTrim(&t5EstLine);
				StringTrim(&t5EstPFMEAReqd);
				StringTrim(&t5EstStation);
				StringTrim(&t5EstcntrlReqd);
				StringTrim(&t5OpnCylTm);
				StringTrim(&t5OpnRemark);
				StringTrim(&t5PEAllowanceOne);
				StringTrim(&t5PEAllowanceTwo);
				StringTrim(&t5PEAltRoute);
				StringTrim(&t5PEAplGangSize);
				StringTrim(&t5PEAplMcNo);
				StringTrim(&t5PEAplTimeType);
				StringTrim(&t5PEBCNo);
				StringTrim(&t5PEDPECT);
				StringTrim(&t5PEDPECW);
				StringTrim(&t5PEFrequency);
				StringTrim(&t5PEGangSize);
				StringTrim(&t5PEHandlingTime);
				StringTrim(&t5PEInternalLabTime);
				StringTrim(&t5PEMMAInd);
				StringTrim(&t5PEMachiningTime);
				StringTrim(&t5PEMostflag);
				StringTrim(&t5PEMstDivision);
				StringTrim(&t5PEMstFrequency);
				StringTrim(&t5PEMstMen);
				StringTrim(&t5PEMstonoff);
				StringTrim(&t5PEOpnDesc);
				StringTrim(&t5PEOpnDrgNo);
				StringTrim(&t5PEOpnDrgStatus);
				StringTrim(&t5PEOpnLDesc);
				StringTrim(&t5PEOpnNumber);
				StringTrim(&t5PEPFMEADate);
				StringTrim(&t5PEPFMEANO);
				StringTrim(&t5PEPFMEARev);
				StringTrim(&t5PEPartDesc);
				StringTrim(&t5PEPartNumberOpr);
				StringTrim(&t5PEPartPerCycle);
				StringTrim(&t5PEPlantCode);
				StringTrim(&t5PEPlantDesc);
				StringTrim(&t5PEProcessEDNNoOpr);
				StringTrim(&t5PEProsheet);
				StringTrim(&t5PESUBOpnNo);
				StringTrim(&t5PESetUpTime);
				StringTrim(&t5PETactTime);
				StringTrim(&t5PETimeOpnApl);
				StringTrim(&t5PETimeOpnPsd);
				StringTrim(&t5PETradeCode);
				StringTrim(&t5PSApprovBy);
				StringTrim(&t5PSDCalTime);
				StringTrim(&t5PSDocNo);
				StringTrim(&t5PSDont);
				StringTrim(&t5PSDos);
				StringTrim(&t5PSIssuDate);
				StringTrim(&t5PSModel);
				StringTrim(&t5PSPrepBy);
				StringTrim(&t5PSProdDate);
				StringTrim(&t5PSRevDate);
				StringTrim(&t5PSRevNo);
				StringTrim(&t5PSReviewBy);
				StringTrim(&t5PfmeaStatus);
				StringTrim(&t5QAApprovBy);
				StringTrim(&t5QADate);
				StringTrim(&t5QAPrepBy);
				StringTrim(&t5QAReviewBy);
				StringTrim(&t5RawParts);
				StringTrim(&t5SftParam);
				StringTrim(&t5Spotplan);
				StringTrim(&t5EstProcessReqd);


			printf("\n AFTER TRIM ");fflush(stdout);  
			printf("\n t5PEPartNumber=::[%s]",t5PEPartNumber);fflush(stdout);  
			printf("\n Revision=::[%s]",Revision);fflush(stdout);   
			printf("\n Sequence=::[%s]",Sequence);fflush(stdout);   
			printf("\n t5EstShop=::[%s]",t5EstShop);fflush(stdout);   
			printf("\n t5PEPlantName=::[%s]",t5PEPlantName);fflush(stdout);   
			printf("\n t5PEProcessEDNNo=::[%s]",t5PEProcessEDNNo);fflush(stdout);   
			printf("\n EPOCHCostDriver=::[%s]",EPOCHCostDriver);fflush(stdout);   
			printf("\n EPOCHNoOfCoats=::[%s]",EPOCHNoOfCoats);fflush(stdout);   
			printf("\n EPOCHPEBCNo=::[%s]",EPOCHPEBCNo);fflush(stdout);   
			printf("\n EPOCHQty=::[%s]",EPOCHQty);fflush(stdout);   
			printf("\n EPOCHUom=::[%s]",EPOCHUom);fflush(stdout);   
			printf("\n EPOCHValue=::[%s]",EPOCHValue);fflush(stdout);   
			printf("\n t5APLCalTime=::[%s]",t5APLCalTime);fflush(stdout);   
			printf("\n t5APLMCDesc=::[%s]",t5APLMCDesc);fflush(stdout);   
			printf("\n t5BCOpnDesc=::[%s]",t5BCOpnDesc);fflush(stdout);   
			printf("\n t5CpStatus=::[%s]",t5CpStatus);fflush(stdout);   
			printf("\n t5EqpDtl=::[%s]",t5EqpDtl);fflush(stdout);   
			printf("\n t5EstLine=::[%s]",t5EstLine);fflush(stdout);   
			printf("\n t5EstPFMEAReqd=::[%s]",t5EstPFMEAReqd);fflush(stdout);   
			printf("\n t5EstStation=::[%s]",t5EstStation);fflush(stdout);   
			printf("\n t5EstcntrlReqd=::[%s]",t5EstcntrlReqd);fflush(stdout);   
			printf("\n t5OpnCylTm=::[%s]",t5OpnCylTm);fflush(stdout);   
			printf("\n t5OpnRemark=::[%s]",t5OpnRemark);fflush(stdout);   
			printf("\n t5PEAllowanceOne=::[%s]",t5PEAllowanceOne);fflush(stdout);   
			printf("\n t5PEAllowanceTwo=::[%s]",t5PEAllowanceTwo);fflush(stdout);   
			printf("\n t5PEAltRoute=::[%s]",t5PEAltRoute);fflush(stdout);   
			printf("\n t5PEAplGangSize=::[%s]",t5PEAplGangSize);fflush(stdout);   
			printf("\n t5PEAplMcNo=::[%s]",t5PEAplMcNo);fflush(stdout);   
			printf("\n t5PEAplTimeType=::[%s]",t5PEAplTimeType);fflush(stdout);   
			printf("\n t5PEBCNo=::[%s]",t5PEBCNo);fflush(stdout);   
			printf("\n t5PEDPECT=::[%s]",t5PEDPECT);fflush(stdout);   
			printf("\n t5PEDPECW=::[%s]",t5PEDPECW);fflush(stdout);   
			printf("\n t5PEFrequency=::[%s]",t5PEFrequency);fflush(stdout);   
			printf("\n t5PEGangSize=::[%s]",t5PEGangSize);fflush(stdout);   
			printf("\n t5PEHandlingTime=::[%s]",t5PEHandlingTime);fflush(stdout);   
			printf("\n t5PEInternalLabTime=::[%s]",t5PEInternalLabTime);fflush(stdout);   
			printf("\n t5PEMMAInd=::[%s]",t5PEMMAInd);fflush(stdout);   
			printf("\n t5PEMachiningTime=::[%s]",t5PEMachiningTime);fflush(stdout);   
			printf("\n t5PEMostflag=::[%s]",t5PEMostflag);fflush(stdout);   
			printf("\n t5PEMstDivision=::[%s]",t5PEMstDivision);fflush(stdout);   
			printf("\n t5PEMstFrequency=::[%s]",t5PEMstFrequency);fflush(stdout);   
			printf("\n t5PEMstMen=::[%s]",t5PEMstMen);fflush(stdout);   
			printf("\n t5PEMstonoff=::[%s]",t5PEMstonoff);fflush(stdout);   
			printf("\n t5PEOpnDesc=::[%s]",t5PEOpnDesc);fflush(stdout);   
			printf("\n t5PEOpnDrgNo=::[%s]",t5PEOpnDrgNo);fflush(stdout);   
			printf("\n t5PEOpnDrgStatus=::[%s]",t5PEOpnDrgStatus);fflush(stdout);   
			printf("\n t5PEOpnLDesc=::[%s]",t5PEOpnLDesc);fflush(stdout);   
			printf("\n t5PEOpnNumber=::[%s]",t5PEOpnNumber);fflush(stdout);   
			printf("\n t5PEPFMEADate=::[%s]",t5PEPFMEADate);fflush(stdout);   
			printf("\n t5PEPFMEANO=::[%s]",t5PEPFMEANO);fflush(stdout);   
			printf("\n t5PEPFMEARev=::[%s]",t5PEPFMEARev);fflush(stdout);   
			printf("\n t5PEPartDesc=::[%s]",t5PEPartDesc);fflush(stdout);   
			printf("\n t5PEPartNumberOpr=::[%s]",t5PEPartNumberOpr);fflush(stdout);   
			printf("\n t5PEPartPerCycle=::[%s]",t5PEPartPerCycle);fflush(stdout);   
			printf("\n t5PEPlantCode=::[%s]",t5PEPlantCode);fflush(stdout);   
			printf("\n t5PEPlantDesc=::[%s]",t5PEPlantDesc);fflush(stdout);   
			printf("\n t5PEProcessEDNNoOpr=::[%s]",t5PEProcessEDNNoOpr);fflush(stdout);   
			printf("\n t5PEProsheet=::[%s]",t5PEProsheet);fflush(stdout);   
			printf("\n t5PESUBOpnNo=::[%s]",t5PESUBOpnNo);fflush(stdout);   
			printf("\n t5PESetUpTime=::[%s]",t5PESetUpTime);fflush(stdout);   
			printf("\n t5PETactTime=::[%s]",t5PETactTime);fflush(stdout);   
			printf("\n t5PETimeOpnApl=::[%s]",t5PETimeOpnApl);fflush(stdout);   
			printf("\n t5PETimeOpnPsd=::[%s]",t5PETimeOpnPsd);fflush(stdout);   
			printf("\n t5PETradeCode=::[%s]",t5PETradeCode);fflush(stdout);   
			printf("\n t5PSApprovBy=::[%s]",t5PSApprovBy);fflush(stdout);   
			printf("\n t5PSDCalTime=::[%s]",t5PSDCalTime);fflush(stdout);   
			printf("\n t5PSDocNo=::[%s]",t5PSDocNo);fflush(stdout);   
			printf("\n t5PSDont=::[%s]",t5PSDont);fflush(stdout);   
			printf("\n t5PSDos=::[%s]",t5PSDos);fflush(stdout);   
			printf("\n t5PSIssuDate=::[%s]",t5PSIssuDate);fflush(stdout);   
			printf("\n t5PSModel=::[%s]",t5PSModel);fflush(stdout);   
			printf("\n t5PSPrepBy=::[%s]",t5PSPrepBy);fflush(stdout);   
			printf("\n t5PSProdDate=::[%s]",t5PSProdDate);fflush(stdout);   
			printf("\n t5PSRevDate=::[%s]",t5PSRevDate);fflush(stdout);   
			printf("\n t5PSRevNo=::[%s]",t5PSRevNo);fflush(stdout);   
			printf("\n t5PSReviewBy=::[%s]",t5PSReviewBy);fflush(stdout);   
			printf("\n t5PfmeaStatus=::[%s]",t5PfmeaStatus);fflush(stdout);   
			printf("\n t5QAApprovBy=::[%s]",t5QAApprovBy);fflush(stdout);   
			printf("\n t5QADate=::[%s]",t5QADate);fflush(stdout);   
			printf("\n t5QAPrepBy=::[%s]",t5QAPrepBy);fflush(stdout);   
			printf("\n t5QAReviewBy=::[%s]",t5QAReviewBy);fflush(stdout);   
			printf("\n t5RawParts=::[%s]",t5RawParts);fflush(stdout);   
			printf("\n t5SftParam=::[%s]",t5SftParam);fflush(stdout);   
			printf("\n t5Spotplan=::[%s]",t5Spotplan);fflush(stdout);   
			printf("\n t5EstProcessReqd=::[%s]",t5EstProcessReqd);fflush(stdout);  
			printf("\n CreDate=::[%s]",CreDate);fflush(stdout);  
			printf("\n LastUpdDate=::[%s]",LastUpdDate);fflush(stdout);  
			printf("\n Creator=::[%s]",Creator);fflush(stdout);  
			printf("\n LstModBy=::[%s]",LstModBy);fflush(stdout);  


			tc_strcpy(EstimateNumberr,t5PEPartNumber);
			tc_strcat(EstimateNumberr,t5PEPlantName);
			tc_strcat(EstimateNumberr,t5EstShop);
			tc_strcat(EstimateNumberr,t5PEProcessEDNNo);
			printf("\n EstimateNumberr: %s\n",EstimateNumberr);fflush(stdout);
 
			tag_t	*esclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=1;
			int es_tags_found=0;

			ITK_CALL(QRY_find("EstimateRevPlantShopEdnQry", &queryes));
			printf("\n found queryes ");fflush(stdout);

			char *ConcatedStr = NULL;
			ConcatedStr=(char *) MEM_alloc(100);
			strcpy(ConcatedStr,t5PEPartNumber);
			strcat(ConcatedStr,"-");
			strcat(ConcatedStr,t5PEPlantName);
			strcat(ConcatedStr,"-");
			strcat(ConcatedStr,t5EstShop);
			strcat(ConcatedStr,"-");
			strcat(ConcatedStr,t5PEProcessEDNNo);
			printf("\n ConcatedStr: [%s]",ConcatedStr);fflush(stdout);
			
			char
			*qry_entrieses[4] = {"ID","Plant Name","*Shop Name","Process EDN Number"},
			*qry_valueses[4] =  {ConcatedStr,t5PEPlantName,t5EstShop,t5PEProcessEDNNo};

			//qry_valuesds[0]=DesignId;
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &es_tags_found, &esclass));
			printf("\n es_tags_found %d", es_tags_found);fflush(stdout);

			if(es_tags_found>0)
			{
				printf("Found Estimate Sheet ...\n");fflush(stdout);
				tag_t EstimateRev = NULLTAG;

				tag_t	*esoprclass	= NULLTAG;
				tag_t queryesopr = NULLTAG;
				int n_entriesesopr=2;
				int esopr_tags_found=0;

				EstimateRev=esclass[0];

				ITK_CALL(QRY_find("EstimateOperationQry", &queryesopr));
				printf("\n found queryesopr ");fflush(stdout);
				
				char
				*qry_entriesesopr[2] = {"ID","t5_EstimateNumber"},
				*qry_valuesesopr[2] =  {t5PEOpnNumber,EstimateNumberr};

				//qry_valuesds[0]=DesignId;
				
				ITK_CALL(QRY_execute(queryesopr, n_entriesesopr, qry_entriesesopr, qry_valuesesopr, &esopr_tags_found, &esoprclass));
				printf("\n hi ..esopr_tags_found %d", esopr_tags_found);fflush(stdout);

				if(esopr_tags_found==0)
				{
					printf("\n Found esopr_tags_found =0");fflush(stdout);
					itemRevSeq = NULL;
					itemRevSeq=(char *) MEM_alloc(32);
					strcpy(itemRevSeq,Revision);
					strcat(itemRevSeq,";");
					strcat(itemRevSeq,Sequence);

					ITK_CALL(ITEM_create_item(t5PEOpnNumber,t5PEOpnNumber,"T5_EstimateOprtn",default_empty_to_A(itemRevSeq),&item,&rev));

					printf("\n After Create ...");fflush(stdout);

					setAttributesOnESOPR(&item,EstimateNumberr);

					printf("\n After Setting Properties on OPR ...");fflush(stdout);

					setAttributesOnESOPRRev(&rev,EPOCHCostDriver,EPOCHNoOfCoats,EPOCHPEBCNo,EPOCHQty,
					EPOCHUom,EPOCHValue,t5APLCalTime,t5APLMCDesc,t5BCOpnDesc,t5CpStatus,t5EqpDtl,t5EstLine,t5EstPFMEAReqd,t5EstStation,t5EstcntrlReqd,t5OpnCylTm,t5OpnRemark,
					t5PEAllowanceOne,t5PEAllowanceTwo,t5PEAltRoute,t5PEAplGangSize,t5PEAplMcNo,t5PEAplTimeType,t5PEBCNo,t5PEDPECT,t5PEDPECW,t5PEFrequency,t5PEGangSize,
					t5PEHandlingTime,t5PEInternalLabTime,t5PEMMAInd,t5PEMachiningTime,t5PEMostflag,t5PEMstDivision,t5PEMstFrequency,t5PEMstMen,t5PEMstonoff,t5PEOpnDesc,
					t5PEOpnDrgNo,t5PEOpnDrgStatus,t5PEOpnLDesc,t5PEPFMEADate,t5PEPFMEANO,t5PEPFMEARev,t5PEPartDesc,t5PEPartPerCycle,t5PEPlantCode,
					t5PEPlantDesc,t5PEProcessEDNNoOpr,t5PEProsheet,t5PESUBOpnNo,t5PESetUpTime,t5PETactTime,t5PETimeOpnApl,t5PETimeOpnPsd,t5PETradeCode,t5PSApprovBy,t5PSDCalTime,
					t5PSDocNo,t5PSDont,t5PSDos,t5PSIssuDate,t5PSModel,t5PSPrepBy,t5PSProdDate,t5PSRevDate,t5PSRevNo,t5PSReviewBy,t5PfmeaStatus,t5QAApprovBy,t5QADate,t5QAPrepBy,
					t5QAReviewBy,t5RawParts,t5SftParam,t5Spotplan,t5EstProcessReqd,CreDate,LastUpdDate,Creator,LstModBy);	

					printf("\n After Setting Properties on OPR REV...");fflush(stdout);

					CreateRelESOPRRevtoESRev(&rev,EstimateRev,t5PEPartNumber,t5PEPlantName,t5EstShop,t5PEProcessEDNNo,fperror);

					printf("\n After REL Creation");fflush(stdout);
				}
				else
				{
					printf("\n Relation of Estimate Sheet to Opr already exists...\n");
					fprintf(fperror,"Estimate Sheet to Operation already exists..:[%s],[%s],[%s],[%s],[%s]\n",t5PEPartNumber,t5PEPlantName,t5EstShop,t5PEProcessEDNNo,t5PEOpnNumber);
				}
			}
			else
			{
				printf("\n Estimate Sheet Doesn't exists/....\n");
				fprintf(fperror,"Estimate Sheet Doesn't exists for  :[%s],[%s],[%s],[%s]\n",t5PEPartNumber,t5PEPlantName,t5EstShop,t5PEProcessEDNNo);

			}

			
			
			
		}
		return status;
	}
	ITK_CALL(POM_logout(false));
	if(fperror)fclose(fperror);fperror=NULL;
	return status;
}


//t5StampERCPart  -u=aplloader .pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba -i=11_sep_PartList_Data.txt  
//CreateEstimateOperation  -u=aplloader -pf=/home/cmitest/shells/Admin/aplloader.pwf -g=dba -i=ES_OPR_9957_Fri_Oct_19_16_50_32_2018.txt

//Remaining: 
//if(t5PSIssuDate!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSIssuDate",t5PSIssuDate)); // no fix date format in TCE
// Query Operations