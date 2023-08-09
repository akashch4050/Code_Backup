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

int CreateRelESRevtoDesign(tag_t* rev,char * PrtNm,char * t5PEPlantName,char * t5EstShop,char * t5PEProcessEDNNo,FILE *fperror)
{
	int status;
	int		ifail	=0;

			tag_t	*designclass	= NULLTAG;

			tag_t queryes = NULLTAG;
			int n_entrieses=2;
			int DesignTags=0;
		
			printf("\n PrtNm: [%s]",PrtNm);fflush(stdout);


			ITK_CALL(QRY_find("Item...", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char
			*qry_entrieses[2] = {"Item ID","Type"},
			*qry_valueses[2] =  {PrtNm,"Design"};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &DesignTags, &designclass));
			
			printf("\n DesignTags : %d", DesignTags);fflush(stdout);

			if(DesignTags>0)
			{
				//Create Rel
				tag_t	relation_type	= NULLTAG;
				tag_t  apltaskrelation = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Estimate_Design_Relation",&relation_type));
				ITK_CALL(GRM_create_relation(*rev, designclass[0], relation_type,  NULLTAG, &apltaskrelation));
				ITK_CALL(GRM_save_relation(apltaskrelation));
				printf("\n ES to Design relation created ...");fflush(stdout);

				tag_t	relation_type1	= NULLTAG;
				tag_t  apltaskrelation1 = NULLTAG;
				ITK_CALL(GRM_find_relation_type("T5_Design_Estimate_Relation",&relation_type1));
				ITK_CALL(GRM_create_relation(designclass[0],*rev, relation_type1,  NULLTAG, &apltaskrelation1));
				ITK_CALL(GRM_save_relation(apltaskrelation1));
				printf("\n Design to ES relation created ...");fflush(stdout);

				ITK_CALL(AOM_refresh(*rev,TRUE));
				ITK_CALL(AOM_refresh(designclass[0],TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation,TRUE));
				ITK_CALL(AOM_refresh(apltaskrelation1,TRUE));
					
			}
			else
			{
				fprintf(fperror,"\n Design [%s] doesn't Exists, So Relation With ES [%s],[%s],[%s],[%s] Is Not Created.\n",PrtNm,PrtNm,t5PEPlantName,t5EstShop,t5PEProcessEDNNo);
			}
}


int setAttributesOnES(tag_t* item,char * t5PEPlantName,char* t5EstShop,char* t5PEProcessEDNNo,char* t5PEPartDesc)
{
	int status;
	int		ifail	=0;

	ITK_CALL(AOM_lock(*item));

	if(t5PEPlantName!=NULL && strcmp(t5PEPlantName,"")!=0 ) ITK_CALL ( AOM_set_value_string(*item,"t5_ESPlantName",t5PEPlantName));
	if(t5EstShop!=NULL && strcmp(t5EstShop,"")!=0 ) ITK_CALL ( AOM_set_value_string(*item,"t5_ShopName",t5EstShop));
	if(t5PEProcessEDNNo!=NULL && strcmp(t5PEProcessEDNNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*item,"t5_ProcEdnNumber",t5PEProcessEDNNo));
	if(t5PEPartDesc!=NULL && strcmp(t5PEPartDesc,"")!=0 ) ITK_CALL ( AOM_set_value_string(*item,"object_desc",t5PEPartDesc));
	ITK_CALL(AOM_save(*item));
	ITK_CALL(AOM_refresh(*item,0));
	ITK_CALL(AOM_unlock(*item));

}


int setAttributesOnESRev(tag_t* rev,int item_sequence_id,char *ClosureTimeStamp,char *WbsID,char *t5APLCalTime,char *t5EPCOHFlg,char *t5EstActor,char *t5EstApprover,
char *t5EstShop,char *t5LastModBy,char *t5PEAgency,char *t5PECopyFrmMost,char *t5PECopyFrmPart,char *t5PEEDNRemark,char *t5PEEstCTType,char *t5PEEstCWType,char *
t5PEEstShtType,char *t5PEModfReason,char *t5PEModfRff,char *t5PEMostflag,char *t5PEMstPreparedBy,char *t5PEPlannedBy,char *t5PEPlantName,char *t5PEProcessApproveBy,
char *t5PEProcessEDNNo,char *t5PEProcessEDNType,char *t5PEShopName,char *t5PETimeOpnApl,char *t5PETimeOpnPsd,char *t5PEVehicleID,char *t5PSDCalTime,
char *t5PfdStatus,char* t5PEPartDesc,char* CreationDate,char* LastUpdate,char* Creator)
{
	int status;
	double dweight=0;
	tag_t projobj=NULLTAG;
	tag_t user_tag=NULLTAG;
	char* username=NULL;
	char FrmNextDate[20]={0};
	char CreDate[20]={0};
	char LstUpdDate[20]={0};
	date_t Release_date;
	date_t CreationDt;
	date_t LstUpdDt;

			printf("\n func item_sequence_id ::[%d]",item_sequence_id);fflush(stdout);  
			printf("\n func ClosureTimeStamp ::[%s]",ClosureTimeStamp);fflush(stdout);  
			printf("\n func WbsID ::[%s]",WbsID);fflush(stdout);  
			printf("\n func t5APLCalTime ::[%s]",t5APLCalTime);fflush(stdout);  
			printf("\n func t5EPCOHFlg ::[%s]",t5EPCOHFlg);fflush(stdout);  
			printf("\n func t5EstActor ::[%s]",t5EstActor);fflush(stdout);  
			printf("\n func t5EstApprover  ::[%s]",t5EstApprover);fflush(stdout);  
			printf("\n func t5EstShop  ::[%s]",t5EstShop);fflush(stdout);  
			printf("\n func t5LastModBy  ::[%s]",t5LastModBy);fflush(stdout);  
			printf("\n func t5PEAgency  ::[%s]",t5PEAgency);fflush(stdout);  
			printf("\n func t5PECopyFrmMost  ::[%s]",t5PECopyFrmMost);fflush(stdout);  
			printf("\n func t5PECopyFrmPart  ::[%s]",t5PECopyFrmPart);fflush(stdout);  
			printf("\n func t5PEEDNRemark  ::[%s]",t5PEEDNRemark);fflush(stdout);  
			printf("\n func t5PEEstCTType ::[%s]",t5PEEstCTType);fflush(stdout);  
			printf("\n func t5PEEstCWType  ::[%s]",t5PEEstCWType);fflush(stdout);  
			printf("\n func t5PEEstShtType  ::[%s]",t5PEEstShtType);fflush(stdout);  
			printf("\n func t5PEModfReason  ::[%s]",t5PEModfReason);fflush(stdout);  
			printf("\n func t5PEModfRff  ::[%s]",t5PEModfRff);fflush(stdout);  
			printf("\n func t5PEMostflag  ::[%s]",t5PEMostflag);fflush(stdout);  
			printf("\n func t5PEMstPreparedBy  ::[%s]",t5PEMstPreparedBy);fflush(stdout);  
			printf("\n func t5PEPartDesc  ::[%s]",t5PEPartDesc);fflush(stdout);  
			printf("\n func t5PEPlannedBy  ::[%s]",t5PEPlannedBy);fflush(stdout);  
			printf("\n func t5PEPlantName  ::[%s]",t5PEPlantName);fflush(stdout);  
			printf("\n func t5PEProcessApproveBy ::[%s]",t5PEProcessApproveBy);fflush(stdout);  
			printf("\n func t5PEProcessEDNNo  ::[%s]",t5PEProcessEDNNo);fflush(stdout);  
			printf("\n func t5PEProcessEDNType  ::[%s]",t5PEProcessEDNType);fflush(stdout);  
			printf("\n func t5PEShopName ::[%s]",t5PEShopName);fflush(stdout);  
			printf("\n func t5PETimeOpnApl ::[%s]",t5PETimeOpnApl);fflush(stdout);  
			printf("\n func t5PETimeOpnPsd ::[%s]",t5PETimeOpnPsd);fflush(stdout);  
			printf("\n func t5PEVehicleID ::[%s]",t5PEVehicleID);fflush(stdout);  
			printf("\n func t5PSDCalTime ::[%s]",t5PSDCalTime);fflush(stdout);  
			printf("\n func t5PfdStatus ::[%s]",t5PfdStatus);fflush(stdout); 
			printf("\n func CreationDate ::[%s]",CreationDate);fflush(stdout); 
			printf("\n func LastUpdate ::[%s]",LastUpdate);fflush(stdout); 
			printf("\n func Creator ::[%s]",Creator);fflush(stdout); 

	ITK_CALL(AOM_lock(*rev));
	printf("\n item_sequence_id --->%d\n",item_sequence_id);fflush(stdout);

	if(item_sequence_id!=0) ITK_CALL ( AOM_set_value_int(*rev,"sequence_id",item_sequence_id));
	if(t5PEPartDesc!=NULL && strcmp(t5PEPartDesc,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"object_desc",t5PEPartDesc));

	if(t5LastModBy!=NULL && strcmp(t5LastModBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_LastModBy",t5LastModBy));
	if(Creator!=NULL && strcmp(Creator,"")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_Creator",Creator));

	if(t5PEPlantName!=NULL && strcmp(t5PEPlantName,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEPlantName",t5PEPlantName));
	if(t5PEProcessEDNNo!=NULL && strcmp(t5PEProcessEDNNo,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEProcessEDNNo",t5PEProcessEDNNo));


		if(ClosureTimeStamp!=NULL && strcmp(ClosureTimeStamp,"")!=0)
		{
			getNextDate(ClosureTimeStamp,FrmNextDate);
			printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);
			ITK_CALL(ITK_string_to_date(FrmNextDate, &Release_date ));
			if(FrmNextDate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"t5_ActualRelDate",Release_date));
		}
	

	if(WbsID!=NULL && strcmp(WbsID,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_WbsID",WbsID));
	if(t5APLCalTime!=NULL && strcmp(t5APLCalTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_APLCalTime",t5APLCalTime));


	if(t5EPCOHFlg!=NULL && strcmp(t5EPCOHFlg,"")!=0 ) 
	{
		printf("\n test4");fflush(stdout); 
		if (tc_strcmp(t5EPCOHFlg,"+")==0)
		{
			printf("\n test5");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_EPCOHFlg",true));
		}
		if (tc_strcmp(t5EPCOHFlg,"-")==0)
		{
			printf("\n test6");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_EPCOHFlg",false));
		}

	}

	if(t5EstActor!=NULL && strcmp(t5EstActor,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EstActor",t5EstActor));
	if(t5EstApprover!=NULL && strcmp(t5EstApprover,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_EstApprover",t5EstApprover));
	if(t5PEAgency!=NULL && strcmp(t5PEAgency,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEAgency",t5PEAgency));
	if(t5PECopyFrmMost!=NULL && strcmp(t5PECopyFrmMost,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PECopyFrmMost",t5PECopyFrmMost));
	if(t5PECopyFrmPart!=NULL && strcmp(t5PECopyFrmPart,"")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PECopyFrmPart",t5PECopyFrmPart));

	if(t5PEEDNRemark!=NULL && strcmp(t5PEEDNRemark,"")!=0)
	{
		if (tc_strstr(t5PEEDNRemark,"New Release")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEDNRemark","Remark4"));
		}
		if (tc_strstr(t5PEEDNRemark,"Estimate Sheet Released for Toolings Only")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEDNRemark","Remark1"));
		}
		if (tc_strstr(t5PEEDNRemark,"Zero Time Estimate Sheet")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEDNRemark","Remark7"));
		}
		if (tc_strstr(t5PEEDNRemark,"Edition Release for Change in Material")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEDNRemark","Remark2"));
		}
		if (tc_strstr(t5PEEDNRemark,"Edition Release for Addition of Operations")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEDNRemark","Remark3"));
		}
		if (tc_strstr(t5PEEDNRemark,"Release for Change in CS")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEDNRemark","Remark5"));
		}
		if (tc_strstr(t5PEEDNRemark,"Release for Updation of Operation Time")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEDNRemark","Remark6"));
		}
		if (tc_strstr(t5PEEDNRemark,"Others")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEDNRemark","Remark8"));
			
		}
	}

	if(t5PEEstCTType!=NULL && strcmp(t5PEEstCTType,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEstCTType",t5PEEstCTType));
	if(t5PEEstCWType!=NULL && strcmp(t5PEEstCWType,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_t5PEEstCWType",t5PEEstCWType));

	/*if(t5PEEstShtType!=NULL)
	{
		if (tc_strstr(t5PEEstShtType,"A")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEstShtType","Assembly"));
			
		}
		if (tc_strstr(t5PEEstShtType,"N")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEstShtType","Normal Est.Sheet"));
			
		}
		if (tc_strstr(t5PEEstShtType,"W")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEstShtType","Weld Est.Sheet"));
			
		}

	}

	if(t5PEModfReason!=NULL)
	{
		if (tc_strstr(t5PEModfReason,"CS")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEModfReason","CS"));
			
		}
		if (tc_strstr(t5PEModfReason,"DM")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEModfReason","DM"));
			
		}
		if (tc_strstr(t5PEModfReason,"MN")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEModfReason","MN"));
			
		}
		if (tc_strstr(t5PEModfReason,"PMR")!=NULL)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PEModfReason","PMR"));
		}

	}
*/
	printf("\n *************Before t5PEEstShtType:[%s]\n",t5PEEstShtType);fflush(stdout);
	StringTrim(&t5PEEstShtType);
	printf("\n *************After t5PEEstShtType:[%s]\n",t5PEEstShtType);fflush(stdout);


	if(t5PEEstShtType!=NULL && strcmp(t5PEEstShtType,"")!=0 ) 
	{
		printf("\n Hiieeeeeee \n",t5PEEstShtType);fflush(stdout);
		ITK_CALL ( AOM_set_value_string(*rev,"t5_PEEstShtType",t5PEEstShtType));
	}
	if(t5PEModfReason!=NULL && strcmp(t5PEModfReason,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEModfReason",t5PEModfReason));
	if(t5PEModfRff!=NULL && strcmp(t5PEModfRff,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEModfRff",t5PEModfRff));

	if(t5PEMostflag!=NULL && strcmp(t5PEMostflag,"")!=0 ) 
	{
		printf("\n test1");fflush(stdout); 

		if (tc_strcmp(t5PEMostflag,"+")==0)
		{
			printf("\n test2");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PEMostflag",true));
		}
		if (tc_strcmp(t5PEMostflag,"-")==0)
		{
			printf("\n test3");fflush(stdout); 
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PEMostflag",false));
		}

	}

	if(t5PEMstPreparedBy!=NULL && strcmp(t5PEMstPreparedBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_t5PEMstPreparedBy",t5PEMstPreparedBy));
	if(t5PEPlannedBy!=NULL && strcmp(t5PEPlannedBy,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEPlannedBy",t5PEPlannedBy));
	if(t5PETimeOpnApl!=NULL && strcmp(t5PETimeOpnApl,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PETimeOpnApl",t5PETimeOpnApl));
	if(t5PETimeOpnPsd!=NULL && strcmp(t5PETimeOpnPsd,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PETimeOpnPsd",t5PETimeOpnPsd));
	if(t5PEVehicleID!=NULL && strcmp(t5PEVehicleID,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PEVehicleID",t5PEVehicleID));
	if(t5PSDCalTime!=NULL && strcmp(t5PSDCalTime,"")!=0 ) ITK_CALL ( AOM_set_value_string(*rev,"t5_PSDCalTime",t5PSDCalTime));


	if(t5PfdStatus!=NULL && strcmp(t5PfdStatus,"")!=0 ) 
	{
		if (tc_strcmp(t5PfdStatus,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PfdStatus",true));
		}
		if (tc_strcmp(t5PfdStatus,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PfdStatus",false));
		}
	}


	if(CreationDate!=NULL && strcmp(CreationDate,"")!=0 )
	{
		getNextDate(CreationDate,CreDate);
		printf("\n CreDate:%s",CreDate);fflush(stdout);
		ITK_CALL(ITK_string_to_date(CreDate, &CreationDt));
		if(CreDate!=NULL) ITK_CALL ( POM_set_creation_date(*rev, CreationDt));
	}
	//if(CreDate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"creation_date",CreationDt));
	
	logical bypass = true;
		ITK_CALL( ITK_set_bypass ( bypass ));
	if(LastUpdate!=NULL && strcmp(LastUpdate,"")!=0 )
	{
		getNextDate(LastUpdate,LstUpdDate);
		printf("\n LstUpdDate:%s",LstUpdDate);fflush(stdout);
		ITK_CALL(ITK_string_to_date(LstUpdDate, &LstUpdDt ));
		if(LstUpdDate!=NULL) ITK_CALL ( POM_set_modification_date(*rev, LstUpdDt));
	}
   ITK_CALL(POM_set_env_info( POM_bypass_attr_update , false, 0, 0.0, NULLTAG, "" ) ); // added later 


	//if(LstUpdDate!=NULL) ITK_CALL ( AOM_set_value_date(*rev,"last_mod_date",LstUpdDt));
	//ITK_CALL(POM_set_env_info( POM_bypass_attr_update , false, 0, 0.0, NULLTAG, "" ) );

	tag_t  ShopAttrId = NULLTAG;
	tag_t  DomainAttrId = NULLTAG;
	tag_t  TobeApprveByAttrId = NULLTAG;

    ITK_CALL(POM_attr_id_of_attr("t5_EstShop","T5_EstimateSheetRevision",&ShopAttrId));
    ITK_CALL(POM_attr_id_of_attr("t5_PEShopName","T5_EstimateSheetRevision",&DomainAttrId));
    ITK_CALL(POM_attr_id_of_attr("t5_PEProcessApproveBy","T5_EstimateSheetRevision",&TobeApprveByAttrId));
   // if(AOM_refresh(*rev,TRUE));
    if(t5EstShop!=NULL && strcmp(t5EstShop,"")!=0 ) ITK_CALL(POM_set_attr_string(1,rev,ShopAttrId,t5EstShop));
    if(t5PEShopName!=NULL && strcmp(t5PEShopName,"")!=0 ) ITK_CALL(POM_set_attr_string(1,rev,DomainAttrId,t5PEShopName));
    if(t5PEProcessApproveBy!=NULL && strcmp(t5PEProcessApproveBy,"")!=0 ) ITK_CALL(POM_set_attr_string(1,rev,TobeApprveByAttrId,t5PEProcessApproveBy));
  //  if(AOM_refresh(*rev,TRUE));
	 ITK_CALL(POM_set_env_info( POM_bypass_attr_update , false, 0, 0.0, NULLTAG, "" ) );


	ITK_CALL(AOM_save(*rev));
	ITK_CALL(AOM_refresh(*rev,0));
	ITK_CALL(AOM_unlock(*rev));


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
char*  CreationDate=NULL  ;
char*  LastUpdate=NULL  ;
char*  Creator=NULL  ;

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
		inputline=(char *) MEM_alloc(1000);
		while(fgets(inputline,1000,fp)!=NULL)
		{
			printf("\n reading file after  ....");fflush(stdout);

			fputs(inputline,stdout);
			t5PEPartNumber=strtok(inputline,"^");  
			Revision=strtok(NULL,"^");  
			Sequence=strtok(NULL,"^");  
			ClosureTimeStamp=strtok(NULL,"^");  
			WbsID=strtok(NULL,"^");  
			t5APLCalTime=strtok(NULL,"^");  
			t5EPCOHFlg=strtok(NULL,"^");  
			t5EstActor=strtok(NULL,"^");  
			t5EstApprover=strtok(NULL,"^");  
			t5EstShop=strtok(NULL,"^");  
			t5LastModBy=strtok(NULL,"^");  
			t5PEAgency=strtok(NULL,"^");  
			t5PECopyFrmMost=strtok(NULL,"^");  
			t5PECopyFrmPart=strtok(NULL,"^");  
			t5PEEDNRemark=strtok(NULL,"^");  
			t5PEEstCTType=strtok(NULL,"^");  
			t5PEEstCWType=strtok(NULL,"^");  
			t5PEEstShtType=strtok(NULL,"^");  
			t5PEModfReason=strtok(NULL,"^");  
			t5PEModfRff=strtok(NULL,"^");  
			t5PEMostflag=strtok(NULL,"^");  
			t5PEMstPreparedBy=strtok(NULL,"^");  
			t5PEPartDesc=strtok(NULL,"^");  
			t5PEPlannedBy=strtok(NULL,"^");  
			t5PEPlantName=strtok(NULL,"^");  
			t5PEProcessApproveBy=strtok(NULL,"^");  
			t5PEProcessEDNNo=strtok(NULL,"^");  
			t5PEProcessEDNType=strtok(NULL,"^");  
			t5PEShopName=strtok(NULL,"^");  
			t5PETimeOpnApl=strtok(NULL,"^");  
			t5PETimeOpnPsd=strtok(NULL,"^");  
			t5PEVehicleID=strtok(NULL,"^");  
			t5PSDCalTime=strtok(NULL,"^");  
			t5PfdStatus=strtok(NULL,"^"); 
			CreationDate=strtok(NULL,"^"); 
			LastUpdate=strtok(NULL,"^"); 
			Creator=strtok(NULL,"^"); 

			printf("\n BEFORE TRIM");fflush(stdout);  
			printf("\n t5PEPartNumber ::[%s]",t5PEPartNumber);fflush(stdout);  
			printf("\n Revision ::[%s]",Revision);fflush(stdout);  
			printf("\n Sequence ::[%s]",Sequence);fflush(stdout);  
			printf("\n ClosureTimeStamp ::[%s]",ClosureTimeStamp);fflush(stdout);  
			printf("\n WbsID ::[%s]",WbsID);fflush(stdout);  
			printf("\n t5APLCalTime ::[%s]",t5APLCalTime);fflush(stdout);  
			printf("\n t5EPCOHFlg ::[%s]",t5EPCOHFlg);fflush(stdout);  
			printf("\n t5EstActor ::[%s]",t5EstActor);fflush(stdout);  
			printf("\n t5EstApprover  ::[%s]",t5EstApprover);fflush(stdout);  
			printf("\n t5EstShop  ::[%s]",t5EstShop);fflush(stdout);  
			printf("\n t5LastModBy  ::[%s]",t5LastModBy);fflush(stdout);  
			printf("\n t5PEAgency  ::[%s]",t5PEAgency);fflush(stdout);  
			printf("\n t5PECopyFrmMost  ::[%s]",t5PECopyFrmMost);fflush(stdout);  
			printf("\n t5PECopyFrmPart  ::[%s]",t5PECopyFrmPart);fflush(stdout);  
			printf("\n t5PEEDNRemark  ::[%s]",t5PEEDNRemark);fflush(stdout);  
			printf("\n t5PEEstCTType ::[%s]",t5PEEstCTType);fflush(stdout);  
			printf("\n t5PEEstCWType  ::[%s]",t5PEEstCWType);fflush(stdout);  
			printf("\n t5PEEstShtType  ::[%s]",t5PEEstShtType);fflush(stdout);  
			printf("\n t5PEModfReason  ::[%s]",t5PEModfReason);fflush(stdout);  
			printf("\n t5PEModfRff  ::[%s]",t5PEModfRff);fflush(stdout);  
			printf("\n t5PEMostflag  ::[%s]",t5PEMostflag);fflush(stdout);  
			printf("\n t5PEMstPreparedBy  ::[%s]",t5PEMstPreparedBy);fflush(stdout);  
			printf("\n t5PEPartDesc  ::[%s]",t5PEPartDesc);fflush(stdout);  
			printf("\n t5PEPlannedBy  ::[%s]",t5PEPlannedBy);fflush(stdout);  
			printf("\n t5PEPlantName  ::[%s]",t5PEPlantName);fflush(stdout);  
			printf("\n t5PEProcessApproveBy ::[%s]",t5PEProcessApproveBy);fflush(stdout);  
			printf("\n t5PEProcessEDNNo  ::[%s]",t5PEProcessEDNNo);fflush(stdout);  
			printf("\n t5PEProcessEDNType  ::[%s]",t5PEProcessEDNType);fflush(stdout);  
			printf("\n t5PEShopName ::[%s]",t5PEShopName);fflush(stdout);  
			printf("\n t5PETimeOpnApl ::[%s]",t5PETimeOpnApl);fflush(stdout);  
			printf("\n t5PETimeOpnPsd ::[%s]",t5PETimeOpnPsd);fflush(stdout);  
			printf("\n t5PEVehicleID ::[%s]",t5PEVehicleID);fflush(stdout);  
			printf("\n t5PSDCalTime ::[%s]",t5PSDCalTime);fflush(stdout);  
			printf("\n t5PfdStatus ::[%s]",t5PfdStatus);fflush(stdout);  
			printf("\n CreationDate ::[%s]",CreationDate);fflush(stdout);  
			printf("\n LastUpdate ::[%s]",LastUpdate);fflush(stdout);  
			printf("\n Creator ::[%s]",Creator);fflush(stdout);  


				StringTrim(&t5PEPartNumber);
				StringTrim(&Revision);
				StringTrim(&Sequence);
				StringTrim(&ClosureTimeStamp);
				StringTrim(&WbsID);
				StringTrim(&t5APLCalTime);
				StringTrim(&t5EPCOHFlg);
				StringTrim(&t5EstActor);
				StringTrim(&t5EstApprover);
				StringTrim(&t5EstShop);
				StringTrim(&t5LastModBy);
				StringTrim(&t5PEAgency);
				StringTrim(&t5PECopyFrmMost);
				StringTrim(&t5PECopyFrmPart);
				StringTrim(&t5PEEDNRemark);
				StringTrim(&t5PEEstCTType);
				StringTrim(&t5PEEstCWType);
				StringTrim(&t5PEEstShtType);
				StringTrim(&t5PEModfReason);
				StringTrim(&t5PEModfRff);
				StringTrim(&t5PEMostflag);
				StringTrim(&t5PEMstPreparedBy);
				StringTrim(&t5PEPartDesc);
				StringTrim(&t5PEPlannedBy);
				StringTrim(&t5PEPlantName);
				StringTrim(&t5PEProcessApproveBy);
				StringTrim(&t5PEProcessEDNNo);
				StringTrim(&t5PEProcessEDNType);
				StringTrim(&t5PEShopName);
				StringTrim(&t5PETimeOpnApl);
				StringTrim(&t5PETimeOpnPsd);
				StringTrim(&t5PEVehicleID);
				StringTrim(&t5PSDCalTime);
				StringTrim(&t5PfdStatus);
				StringTrim(&CreationDate);
				StringTrim(&LastUpdate);
				StringTrim(&Creator);


			printf("\n\n AFTER TRIM");fflush(stdout);  
			printf("\n t5PEPartNumber ::[%s]",t5PEPartNumber);fflush(stdout);  
			printf("\n Revision ::[%s]",Revision);fflush(stdout);  
			printf("\n Sequence ::[%s]",Sequence);fflush(stdout);  
			printf("\n ClosureTimeStamp ::[%s]",ClosureTimeStamp);fflush(stdout);  
			printf("\n WbsID ::[%s]",WbsID);fflush(stdout);  
			printf("\n t5APLCalTime ::[%s]",t5APLCalTime);fflush(stdout);  
			printf("\n t5EPCOHFlg ::[%s]",t5EPCOHFlg);fflush(stdout);  
			printf("\n t5EstActor ::[%s]",t5EstActor);fflush(stdout);  
			printf("\n t5EstApprover  ::[%s]",t5EstApprover);fflush(stdout);  
			printf("\n t5EstShop  ::[%s]",t5EstShop);fflush(stdout);  
			printf("\n t5LastModBy  ::[%s]",t5LastModBy);fflush(stdout);  
			printf("\n t5PEAgency  ::[%s]",t5PEAgency);fflush(stdout);  
			printf("\n t5PECopyFrmMost  ::[%s]",t5PECopyFrmMost);fflush(stdout);  
			printf("\n t5PECopyFrmPart  ::[%s]",t5PECopyFrmPart);fflush(stdout);  
			printf("\n t5PEEDNRemark  ::[%s]",t5PEEDNRemark);fflush(stdout);  
			printf("\n t5PEEstCTType ::[%s]",t5PEEstCTType);fflush(stdout);  
			printf("\n t5PEEstCWType  ::[%s]",t5PEEstCWType);fflush(stdout);  
			printf("\n t5PEEstShtType  ::[%s]",t5PEEstShtType);fflush(stdout);  
			printf("\n t5PEModfReason  ::[%s]",t5PEModfReason);fflush(stdout);  
			printf("\n t5PEModfRff  ::[%s]",t5PEModfRff);fflush(stdout);  
			printf("\n t5PEMostflag  ::[%s]",t5PEMostflag);fflush(stdout);  
			printf("\n t5PEMstPreparedBy  ::[%s]",t5PEMstPreparedBy);fflush(stdout);  
			printf("\n t5PEPartDesc  ::[%s]",t5PEPartDesc);fflush(stdout);  
			printf("\n t5PEPlannedBy  ::[%s]",t5PEPlannedBy);fflush(stdout);  
			printf("\n t5PEPlantName  ::[%s]",t5PEPlantName);fflush(stdout);  
			printf("\n t5PEProcessApproveBy ::[%s]",t5PEProcessApproveBy);fflush(stdout);  
			printf("\n t5PEProcessEDNNo  ::[%s]",t5PEProcessEDNNo);fflush(stdout);  
			printf("\n t5PEProcessEDNType  ::[%s]",t5PEProcessEDNType);fflush(stdout);  
			printf("\n t5PEShopName ::[%s]",t5PEShopName);fflush(stdout);  
			printf("\n t5PETimeOpnApl ::[%s]",t5PETimeOpnApl);fflush(stdout);  
			printf("\n t5PETimeOpnPsd ::[%s]",t5PETimeOpnPsd);fflush(stdout);  
			printf("\n t5PEVehicleID ::[%s]",t5PEVehicleID);fflush(stdout);  
			printf("\n t5PSDCalTime ::[%s]",t5PSDCalTime);fflush(stdout);  
			printf("\n t5PfdStatus ::[%s]",t5PfdStatus);fflush(stdout);  
			printf("\n CreationDate ::[%s]",CreationDate);fflush(stdout);  
			printf("\n LastUpdate ::[%s]",LastUpdate);fflush(stdout);  
			printf("\n Creator ::[%s]",Creator);fflush(stdout);  

			if(t5PEPartNumber==NULL || strcmp(t5PEPartNumber,"")==0)
			{
				fprintf(fperror,"\n No Partnumber Exists so not able to create ES");
				FlagERR=1;
			}

			if(t5PEPlantName==NULL || strcmp(t5PEPlantName,"")==0)
			{
				fprintf(fperror,"\n No t5PEPlantName Exists so not able to create ES");
				FlagERR=1;
			}
			if(t5EstShop==NULL || strcmp(t5EstShop,"")==0)
			{
				fprintf(fperror,"\n No t5EstShop Exists so not able to create ES");
				FlagERR=1;
			}
			if(t5PEProcessEDNNo==NULL || strcmp(t5PEProcessEDNNo,"")==0)
			{
				fprintf(fperror,"\n No t5PEProcessEDNNo Exists so not able to create ES");
				FlagERR=1;
			}

			printf("\n FlagERR:[%d] ",FlagERR);fflush(stdout);

			if(FlagERR==0)
			{
			tag_t	*esclass	= NULLTAG;
			tag_t queryes = NULLTAG;
			int n_entrieses=1;
			int es_tags_found=0;

			ITK_CALL(QRY_find("EstimateRevPlantShopEdnQry", &queryes));
			printf("\n found queryes ");fflush(stdout);
			
			char	EstimateNumberr[100];
			tc_strcpy(EstimateNumberr,t5PEPartNumber);
			tc_strcat(EstimateNumberr,"*");
			printf("\n EstimateNumberr: %s\n",EstimateNumberr);fflush(stdout);

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
			*qry_valueses[4] =  {EstimateNumberr,t5PEPlantName,t5EstShop,t5PEProcessEDNNo};
			
			ITK_CALL(QRY_execute(queryes, n_entrieses, qry_entrieses, qry_valueses, &es_tags_found, &esclass));
			printf("\n es_tags_found %d", es_tags_found);fflush(stdout);

			if(es_tags_found==0)
			{
					printf("Creating new ES\n");fflush(stdout);

					item_revision_id_int=atoi(Sequence);

					itemRevSeq = NULL;
					itemRevSeq=(char *) MEM_alloc(32);
					strcpy(itemRevSeq,Revision);
					strcat(itemRevSeq,";");
					strcat(itemRevSeq,Sequence);

					printf("\n Category Name is ---->%s\n",itemRevSeq);fflush(stdout);
					
					//ITK_CALL(ITEM_create_item(t5PEPartNumber,t5PEPartNumber,"T5_EstimateSheet",default_empty_to_A(itemRevSeq),&item,&rev));
					ITK_CALL(ITEM_create_item(ConcatedStr,ConcatedStr,"T5_EstimateSheet",Revision,&item,&rev));

					setAttributesOnES(&item,t5PEPlantName,t5EstShop,t5PEProcessEDNNo,t5PEPartDesc);

					setAttributesOnESRev(&rev,item_revision_id_int,ClosureTimeStamp,WbsID,t5APLCalTime,t5EPCOHFlg,t5EstActor,t5EstApprover,t5EstShop,t5LastModBy,t5PEAgency,t5PECopyFrmMost,t5PECopyFrmPart,t5PEEDNRemark,t5PEEstCTType,t5PEEstCWType,
					t5PEEstShtType,t5PEModfReason,t5PEModfRff,t5PEMostflag,t5PEMstPreparedBy,t5PEPlannedBy,t5PEPlantName,t5PEProcessApproveBy,t5PEProcessEDNNo,
					t5PEProcessEDNType,t5PEShopName,t5PETimeOpnApl,t5PETimeOpnPsd,t5PEVehicleID,t5PSDCalTime,t5PfdStatus,t5PEPartDesc,CreationDate,LastUpdate,Creator);

					CreateRelESRevtoDesign(&rev,t5PEPartNumber,t5PEPlantName,t5EstShop,t5PEProcessEDNNo,fperror);
					
					//ITK_CALL(CreateEffctivity(rev,FromDate,ToDate,lifecycleS));
					//ITK_CALL(CreateEffctivity(rev,lifecycleS,aplEffFromDate,aplEffToDate,stdEffFromDate,stdEffToDate,fperror));
			}
			else
			{
				printf("\n Estimate Sheet Already exists/....\n");
				fprintf(fperror,"Estimate Sheet Already exists for  :[%s],[%s],[%s],[%s]\n",ConcatedStr,t5PEPlantName,t5EstShop,t5PEProcessEDNNo);
			}
		  }
		}
		return status;
	}
	ITK_CALL(POM_logout(false));
	if(fperror)fclose(fperror);fperror=NULL;
	return status;
}


//t5StampERCPart  -u=aplloader .pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba -i=11_sep_PartList_Data.txt  
//aplloader007

//Last Update: Last Modified Date: not getting set on cmitest ... 
//Creationdate: Date created
//Creator : Owner
//Last modify by: Last modifying user
// Need to test for last update set on ES = remaining


//CreateEstimateSheet  -u=aplloader -pf=/home/cmitest/shells/Admin/aplloader.pwf -g=dba -i=ES_9957_Thu_Sep_20_11_29_11_2018.txt

//not working on cmitest:
//Last Update: Last Modified Date: not getting set on cmitest ... 
