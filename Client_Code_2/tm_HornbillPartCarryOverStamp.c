/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  Author		 :   Deepti Meshram
*  Module		 :   TCUA Desing Rev Data Stamping (CS,STORELOC,LCS,APL-STD EFF DATE) as per TCE
*  Code			 :   t5StampERCPart.c
*  Created on	 :   August 25, 2018
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

#define ERCREVIEW	"T5_LcsReview"
#define ERCWORKING	"T5_LcsWorking"
#define ERCRELEASED "T5_LcsErcRlzd"
#define APLWORKING  "T5_LcsAPLWrkg"
#define APLRELEASED "T5_LcsAplRlzd"
#define STDWORKING  "T5_LcsSTDWrkg"
#define STDRELEASED "T5_LcsStdRlzd"
#define APLREVIEW	"T5_LcsAplReview"

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

int setAttributesOnDesign(tag_t* item,char * desc,char* UnitOfMeasureS,char* LeftRhS)
{
	int status;
	int		ifail	=0;

	char* ent_uom=NULL;
	tag_t unit_of_measure=NULLTAG;

	ent_uom=(char *) MEM_alloc(10 * sizeof(char *));

	ITK_CALL(AOM_lock(*item));

	if(strcmp(UnitOfMeasureS,"1")==0){strcpy(ent_uom,ONE);}
	else if(strcmp(UnitOfMeasureS,"2")==0){strcpy(ent_uom,TWO);}
	else if(strcmp(UnitOfMeasureS,"3")==0){strcpy(ent_uom,THREE);}
	else if(strcmp(UnitOfMeasureS,"4")==0){strcpy(ent_uom,FOUR);}
	else if(strcmp(UnitOfMeasureS,"5")==0){strcpy(ent_uom,FIVE);}
	else if(strcmp(UnitOfMeasureS,"6")==0){strcpy(ent_uom,SIX);}
	else if(strcmp(UnitOfMeasureS,"7")==0){strcpy(ent_uom,SEVEN);}
	else if(strcmp(UnitOfMeasureS,"8")==0){strcpy(ent_uom,EIGHT);}
	else {strcpy(ent_uom,"-");}

	ITK_CALL(UOM_find_by_symbol(ent_uom,&unit_of_measure));
	if(ITK_ok != (ifail =ITEM_set_unit_of_measure(*item, unit_of_measure)))
	{
		return ifail;
	}
	ITK_CALL ( AOM_set_value_string(*item,"t5_LeftRh",LeftRhS));

	//ITK_CALL ( AOM_set_value_string(*item,"uom_tag",UnitOfMeasureS));
	ITK_CALL ( AOM_set_value_string(*item,"object_desc",desc));
	ITK_CALL(AOM_save(*item));
	ITK_CALL(AOM_unlock(*item));

	

}

int setAttributesOnDesignRev(tag_t* rev,char* t5CarMakeBuyIndiS,char* t5CarStoreLoc,char* GenPlantCS,char* getPlantStore,FILE *fperror)
{

	char* item_id_upd=NULL;
	char* item_revision_id_upd=NULL;
	int* item_sequence_id_upd=NULL;
	char* rev_PlantStrLoc=NULL;
	char* rev_PlantCS=NULL;
    int status;

	ITK_CALL ( AOM_ask_value_string(*rev,GenPlantCS,&rev_PlantCS));
	ITK_CALL ( AOM_ask_value_string(*rev,getPlantStore,&rev_PlantStrLoc));

	ITK_CALL ( AOM_ask_value_string(*rev,"item_id",&item_id_upd));
	ITK_CALL ( AOM_ask_value_string(*rev,"item_revision_id",&item_revision_id_upd));
	ITK_CALL ( AOM_ask_value_int(*rev,"sequence_id",&item_sequence_id_upd));

	printf("Exist value..[%s]:[%s]",rev_PlantCS,rev_PlantStrLoc);fflush(stdout);

	
	ITK_CALL(AOM_lock(*rev));

	printf("inside make buy indicator update..[%s]:[%s]",GenPlantCS,getPlantStore);fflush(stdout);
	if((tc_strcmp(rev_PlantCS,"")!=0))
	{
		printf("make buy indicator update is not blank ..so updating as per value[%s]:[%s]:[%d]",item_id_upd,item_revision_id_upd,item_sequence_id_upd);fflush(stdout);	
		if(strcmp(t5CarMakeBuyIndiS," ")!=0)
		{
			if((tc_strcmp(rev_PlantCS,t5CarMakeBuyIndiS)!=0))
			{
				ITK_CALL ( AOM_set_value_string(*rev,GenPlantCS,t5CarMakeBuyIndiS));
				printf("Updating the MAKE BUY...[%s]:[%s]:[%d]",item_id_upd,item_revision_id_upd,item_sequence_id_upd);fflush(stdout);
			}
			else
			{
				printf("make buy indicator update is not blank ..so updating as per value[%s]:[%s]:[%d]",item_id_upd,item_revision_id_upd,item_sequence_id_upd);fflush(stdout);	
		
			}
		}

	}
	else
	{
		printf("Setting the value for MAKE BUY...");fflush(stdout);
		//if((tc_strcmp(t5CarMakeBuyIndiS," ")!=0) || (tc_strcmp(t5CarMakeBuyIndiS,"")!=0))
		if(strcmp(t5CarMakeBuyIndiS," ")!=0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,GenPlantCS,t5CarMakeBuyIndiS));
		}
		
	}

	if((tc_strcmp(rev_PlantStrLoc,"")!=0))
	{
		printf("Store Location is not blank ..so updating as per value..[%s]:[%s]:[%d]",item_id_upd,item_revision_id_upd,item_sequence_id_upd);fflush(stdout);	
		if(strcmp(t5CarStoreLoc," ")!=0)
		{
			if((tc_strcmp(rev_PlantStrLoc,t5CarStoreLoc)!=0))
			{
				ITK_CALL ( AOM_set_value_string(*rev,getPlantStore,t5CarStoreLoc));
				printf("Updating the STORE LOCATION..[%s]:[%s]:[%d]",item_id_upd,item_revision_id_upd,item_sequence_id_upd);fflush(stdout);
			}
		}

	}
	else
	{
		printf("Setting the value for STORELOCA...");fflush(stdout);
		if(strcmp(t5CarStoreLoc," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,getPlantStore,t5CarStoreLoc));

	}
	

	ITK_CALL(AOM_save(*rev));
	ITK_CALL(AOM_unlock(*rev));
	
	return status;


}




int convertDate(char* date,char* rDate)
{
	int status;

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

	printf("\n strYear:%s\n",strYear);fflush(stdout);
	printf("\n CCMonth:%s\n",CCMonth);fflush(stdout);
	printf("\n strDay:%s\n",strDay);fflush(stdout);

	tc_strcpy(cnextAccessDate,strDay);
	tc_strcat(cnextAccessDate,"-");
	tc_strcat(cnextAccessDate,CCMonth);
	tc_strcat(cnextAccessDate,"-");
	tc_strcat(cnextAccessDate,strYear);
	tc_strcat(cnextAccessDate," ");
	tc_strcat(cnextAccessDate,"00:00");
	printf("\n cnextAccessDate:%s\n",cnextAccessDate);fflush(stdout);
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
/*FUCNTION TO REMOVE EXISTING STATUS AS PER APL PART OR ERC PART*/
int removeReleaseStatus(tag_t itemrev,char *lifeCycleS,int ercStatusFlg,int aplWrkStatusFlg,int aplRlzdStatusFlg,int stdWrkStatusFlg,int stdRlzdStatusFlg,char *OrgIDflag)
{
		
		int			st_relList_count		=0;
		int			status		=0;
		int			Rel_cnt					=0; 
		tag_t*		relstatus_list			=NULLTAG; 
		char		*WSO_Name_RelVal		=NULL ;
		tag_t		tReleaseStatusList_checkin=NULLTAG; 
		int			n_values_checkin			=0; 
		int			cunt1					=	0; 
		tag_t*		attr_tags_checkin		=	NULLTAG; 
		logical		*is_it_null_checkin		=   NULL; 
		logical		*is_it_empty_checkin	=   NULL; 
		tag_t		class_id				=     NULLTAG;
		char		*class_name				=    NULL;
		char		*gov_classification		=NULL;

		logical		log1;
		
		printf("\n Passed Parameters :%s,%d,%d,%d,%d,%s\n",lifeCycleS,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,OrgIDflag);fflush(stdout);
		
		ITK_CALL(POM_class_of_instance(itemrev,&class_id));
		ITK_CALL(POM_name_of_class(class_id,&class_name));
		printf("\n class_name is :%s\n",class_name);fflush(stdout);

		ITK_CALL(POM_attr_id_of_attr("release_status_list",class_name,&tReleaseStatusList_checkin));fflush(stdout);

		ITK_CALL(POM_unload_instances (1,&itemrev));
		ITK_CALL(POM_load_instances(1,&itemrev,class_id,1));
		ITK_CALL(POM_is_loaded(itemrev,&log1));
		if(log1 == 1)
		{
			 printf(" Load Success\n " );
		}
		else
		printf("Load Failure\n"  );

		ITK_CALL( POM_length_of_attr(itemrev, tReleaseStatusList_checkin, &n_values_checkin) );
		printf("\n n_values is :%d\n",n_values_checkin);fflush(stdout);
		if(n_values_checkin>0)
		{
			ITK_CALL( POM_ask_attr_tags(itemrev, tReleaseStatusList_checkin, 0, n_values_checkin, &attr_tags_checkin, &is_it_null_checkin, &is_it_empty_checkin ));
			printf("\n n_values11 is :%d\n",n_values_checkin);fflush(stdout);

			
			for (cunt1 =  0; cunt1 < n_values_checkin; cunt1++)
			{
				printf("\n Inside POM_save_instances is :%d\n",cunt1);fflush(stdout);


				if(tc_strcmp(OrgIDflag,"ERC")==0)
				{
					if(aplWrkStatusFlg>0)
					{
						ITK_CALL(POM_remove_from_attr(1,&itemrev,tReleaseStatusList_checkin,1,1));
						printf("\n For ERC cunt1 == 0 removedd is :%d\n",cunt1);fflush(stdout);
					}

					if(stdWrkStatusFlg>0)
					{
						ITK_CALL(POM_remove_from_attr(1,&itemrev,tReleaseStatusList_checkin,2,1));
						printf("\n For ERC cunt1 == 0 removedd is :%d\n",cunt1);fflush(stdout);
					}
				}
				else
				{
					if(aplWrkStatusFlg>0)
					{
						ITK_CALL(POM_remove_from_attr(1,&itemrev,tReleaseStatusList_checkin,0,1));
						printf("\n For APL cunt1 == 0 removedd is :%d\n",cunt1);fflush(stdout);
					}

					if(stdWrkStatusFlg>0)
					{
						ITK_CALL(POM_remove_from_attr(1,&itemrev,tReleaseStatusList_checkin,1,1));
						printf("\n For APL cunt1 == 0 removedd is :%d\n",cunt1);fflush(stdout);
					}
				
				
				}


				ITK_CALL(POM_save_instances(1,&itemrev,1));

				ITK_CALL(POM_refresh_instances(1,&itemrev,class_id,2));

				ITK_CALL(AOM_lock(itemrev));

				ITK_CALL(AOM_save(itemrev));
				ITK_CALL(AOM_refresh(itemrev,1));
				break;
			}

			ITK_CALL(AOM_unlock(itemrev));
			
		}

		return status;
}


/*******FUNCTION TO CREATE EFF,STAMP LCS FOR APL PARTS...************/

int CreateEffctivityAPL(tag_t itemrev,char *lifeCycleS,char *APLEffFromDate,char *APLEffToDate,char *STDEffFromDate,char *STDEffToDate,FILE *fperror,FILE *fp_Efferror)
{

		int status;
		char* lcsToset=NULL;
		char* erclcsToset=NULL;
		char* aplWrklcsToset=NULL;
		char* aplRlzdlcsToset=NULL;
		char* stdWrklcsToset=NULL;
		char* stdRlzdlcsToset=NULL;
		char* aplReviewlcsToset=NULL;
		char* flagSetAPL=NULL;
		char* item_seq_upd=NULL;
		char* item_revision_id_upd=NULL;
		char* item_id_upd=NULL;
		char* class_name=NULL;
    	int	   status_count=0;
    	int	   StatusFlg=0;
    	int	   ss=0;
    	int	   ercStatusFlg=0;
    	int	   aplWrkStatusFlg=0;
    	int	   aplRlzdStatusFlg=0;
    	int	   stdWrkStatusFlg=0;
    	int	   stdRlzdStatusFlg=0;
    	int	   apleffStamp=0;
		tag_t* status_list = NULLTAG;
		tag_t apl_release_status = NULLTAG;
		tag_t std_release_status = NULLTAG;
		logical retain_apl_released_date=false;
		logical retain_std_released_date=false;
		tag_t   status_rel                                      = NULLTAG;
		date_t test_date_1;
		date_t test_date_2;
		tag_t apl_eff_d = NULLTAG;
		char FrmNextDate[20]={0};
		char ToNextDate[20]={0};
		date_t *start_end_date = NULL;
		date_t test_date_11;
		date_t test_date_21;
		tag_t std_eff_d = NULLTAG;
		char FrmNextDate1[20]={0};
		char ToNextDate1[20]={0};
		date_t *start_end_date1 = NULL;



		lcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		erclcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		aplWrklcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		aplRlzdlcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		stdWrklcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		stdRlzdlcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		aplReviewlcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		flagSetAPL =(char *) MEM_alloc(20 * sizeof(char *));

		if(strcmp(lifeCycleS,"LcsAPLWrkg")==0)
		{
			printf("\n This is apl wrk...");fflush(stdout);
			strcpy(lcsToset,APLWORKING);
		}
		else if(strcmp(lifeCycleS,"LcsAplRlzd")==0)
		{
			printf("\n This is apl rel...");fflush(stdout);
			strcpy(lcsToset,APLRELEASED);
		}
		else if(strcmp(lifeCycleS,"LcsSTDWrkg")==0)
		{
			printf("\n This is std wrk...");fflush(stdout);
			strcpy(lcsToset,STDWORKING);
		}
		else if(strcmp(lifeCycleS,"LcsSTDRlzd")==0)
		{
			printf("\n This is std rel...");fflush(stdout);
			strcpy(lcsToset,STDRELEASED);
		}

		strcpy(erclcsToset,ERCRELEASED);
		strcpy(aplWrklcsToset,APLWORKING);
		strcpy(aplRlzdlcsToset,APLRELEASED);
		strcpy(stdWrklcsToset,STDWORKING);
		strcpy(stdRlzdlcsToset,STDRELEASED);
		strcpy(aplReviewlcsToset,APLREVIEW);
		strcpy(flagSetAPL,"APL");

		//ITK_CALL(AOM_ask_value_int(itemrev,"sequence_id",&item_seq_upd));
		//printf("\n Ckd Out seq is -->%d\n",item_seq_upd); fflush(stdout);

		ITK_CALL(AOM_ask_value_string(itemrev,"item_revision_id",&item_revision_id_upd));
		printf("\n Ckd Out Rev is -->%s\n",item_revision_id_upd); fflush(stdout);

		ITK_CALL(AOM_ask_value_string(itemrev,"item_id",&item_id_upd));

		
		ITK_CALL( WSOM_ask_release_status_list(itemrev,&status_count,&status_list));
		printf("\n REV status_count: %d\n",status_count);fflush(stdout);
		if (status_count == 0)  /* No Status, so the Item is not yet Released */
		{
			printf("\n No Status, so stamping the PART LCS as per TCE \n");fflush(stdout);

			//fprintf(*fperror,"No Status found on Part:[%s],[%s],[%s]\n",item_id_upd,item_revision_id_upd,item_seq_upd);
			fprintf(fperror,"No Status, so stamping the PART LCS as per TCE  :[%s],[%s]\n",item_id_upd,item_revision_id_upd);
			StatusFlg ++;

			if((strcmp(lcsToset,"T5_LcsAPLWrkg")==0))
				{
					ITK_CALL(CR_create_release_status(lcsToset,&apl_release_status));
					ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));

				}
				if((strcmp(lcsToset,"T5_LcsAplRlzd")==0))
				{
					
					if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
					{
						ITK_CALL(CR_create_release_status(lcsToset,&apl_release_status));
						ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));
					
						getNextDate(APLEffFromDate,FrmNextDate);
						printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);

						getNextDate(APLEffToDate,ToNextDate);
						printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

						ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
						ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
						
						start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


						start_end_date[0] = test_date_1;
						start_end_date[1] = test_date_2;

						ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
						ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
						ITK_CALL(AOM_save(apl_eff_d));
						ITK_CALL(AOM_save(apl_release_status));
						ITK_CALL(AOM_refresh(apl_release_status, 0 ));
						printf("\n Eff and Release status stamped ..");fflush(stdout);
					}
					else
					{
						fprintf(fp_Efferror,"\n [%s],[%s],APL\n",item_id_upd,item_revision_id_upd);
					}
			

				}
				if((strcmp(lcsToset,"T5_LcsSTDWrkg")==0))
				{
					
					printf("\n aplRlzdStatusFlg ..%d",aplRlzdStatusFlg);fflush(stdout);
					if(aplRlzdStatusFlg==0)
					{
						if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
						{
							ITK_CALL(CR_create_release_status("T5_LcsAplRlzd",&apl_release_status));
							ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));
						
							getNextDate(APLEffFromDate,FrmNextDate);
							printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);

							getNextDate(APLEffToDate,ToNextDate);
							printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

							ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
							ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
							
							start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


							start_end_date[0] = test_date_1;
							start_end_date[1] = test_date_2;

							ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
							ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
							ITK_CALL(AOM_save(apl_eff_d));
							ITK_CALL(AOM_save(apl_release_status));
							ITK_CALL(AOM_refresh(apl_release_status, 0 ));
							printf("\n APL Eff and Release status stamped ..");fflush(stdout);

							ITK_CALL(CR_create_release_status(lcsToset,&std_release_status));
							ITK_CALL(EPM_add_release_status(std_release_status,1,&itemrev,retain_std_released_date));
						}
						else
						{
							fprintf(fp_Efferror,"\n [%s],[%s],APL\n",item_id_upd,item_revision_id_upd);
						}
					}

					

					
				}
				if((strcmp(lcsToset,"T5_LcsStdRlzd")==0))
				{
					printf("\n Inside STDRLZD Cond aplRlzdStatusFlg ..%d",aplRlzdStatusFlg);fflush(stdout);
					if(aplRlzdStatusFlg==0)
					{
							
						if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
						{
							ITK_CALL(CR_create_release_status("T5_LcsAplRlzd",&apl_release_status));
							ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));
						
							getNextDate(APLEffFromDate,FrmNextDate);
							getNextDate(APLEffToDate,ToNextDate);
							printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);
							printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

							ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
							ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
							
							start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


							start_end_date[0] = test_date_1;
							start_end_date[1] = test_date_2;

							ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
							ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
							ITK_CALL(AOM_save(apl_eff_d));
							ITK_CALL(AOM_save(apl_release_status));
							ITK_CALL(AOM_refresh(apl_release_status, 0 ));
							printf("\n 11 APL Eff and Release status stamped ..");fflush(stdout);
							apleffStamp++;
						}
						else
						{
							fprintf(fp_Efferror,"\n [%s],[%s],APL\n",item_id_upd,item_revision_id_upd);
						}
					}


					if(apleffStamp>0)
					{

						if((tc_strcmp(STDEffFromDate,"-")!=0) && (tc_strcmp(STDEffToDate,"-")!=0))
						{
							ITK_CALL(CR_create_release_status(lcsToset,&std_release_status));
							ITK_CALL(EPM_add_release_status(std_release_status,1,&itemrev,retain_std_released_date));

							getNextDate(STDEffFromDate,FrmNextDate1);
							getNextDate(STDEffToDate,ToNextDate1);
							printf("\n FrmNextDate1:%s",FrmNextDate1);fflush(stdout);
							printf("\n ToNextDate1:%s",ToNextDate1);fflush(stdout);

							ITK_CALL(ITK_string_to_date(FrmNextDate1, &test_date_11 ));
							ITK_CALL(ITK_string_to_date(ToNextDate1, &test_date_21 ));
							
							start_end_date1 = (date_t *)MEM_alloc(sizeof(date_t)*2);


							start_end_date1[0] = test_date_11;
							start_end_date1[1] = test_date_21;

							ITK_CALL(WSOM_status_clear_effectivities ( std_release_status));
							ITK_CALL(WSOM_effectivity_create_with_dates(std_release_status, NULLTAG, 2,start_end_date1, EFFECTIVITY_closed, &std_eff_d ));
							ITK_CALL(AOM_save(std_eff_d));
							ITK_CALL(AOM_save(std_release_status));
							ITK_CALL(AOM_refresh(std_release_status, 0 ));
							printf("\n STD Eff and Release status stamped ..");fflush(stdout);
						}
						else
						{
							fprintf(fp_Efferror,"\n [%s],[%s],STD\n",item_id_upd,item_revision_id_upd);
						}
					}

					

				}

		}
		else
		{
			for(ss=0; ss<status_count ; ss++)
			{
				ITK_CALL(AOM_ask_value_string(status_list[ss],"object_name",&class_name));
				printf("\n class_name: %s\n",class_name);fflush(stdout);
				if(strcmp(class_name,lcsToset)==0)
				{
					StatusFlg ++;
				}
				else if(strcmp(class_name,erclcsToset)==0)
				{
					ercStatusFlg ++;
				}
				else if(strcmp(class_name,aplWrklcsToset)==0)
				{
					aplWrkStatusFlg ++;
				}
				else if(strcmp(class_name,aplRlzdlcsToset)==0)
				{
					aplRlzdStatusFlg ++;
				}
				else if(strcmp(class_name,stdWrklcsToset)==0)
				{
					stdWrkStatusFlg ++;
				}
				else if(strcmp(class_name,stdRlzdlcsToset)==0)
				{
					stdRlzdStatusFlg ++;
				}
				else if(strcmp(class_name,aplReviewlcsToset)==0)
				{
					aplWrkStatusFlg ++;
				}

			}
		}
		printf("\n StatusFlg: %d\n",StatusFlg);fflush(stdout);
		if(StatusFlg > 0)
		{
			printf("\n 22 StatusFlg: %d\n",StatusFlg);fflush(stdout);
			return status;
		}
		else
		{
			printf("\n11 StatusFlg: %d\n",StatusFlg);fflush(stdout);
			//if(ercStatusFlg >0)
			//{
				if((strcmp(lcsToset,"T5_LcsAPLWrkg")==0))
				{
					ITK_CALL(removeReleaseStatus(itemrev,lcsToset,ercStatusFlg,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,flagSetAPL));
					ITK_CALL(CR_create_release_status(lcsToset,&apl_release_status));
					ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));

				

				}
				if((strcmp(lcsToset,"T5_LcsAplRlzd")==0))
				{
					
					ITK_CALL(removeReleaseStatus(itemrev,lcsToset,ercStatusFlg,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,flagSetAPL));

					ITK_CALL(CR_create_release_status(lcsToset,&apl_release_status));
					ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));


					if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
					{
						getNextDate(APLEffFromDate,FrmNextDate);
						printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);

						getNextDate(APLEffToDate,ToNextDate);
						printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

						ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
						ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
						
						start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


						start_end_date[0] = test_date_1;
						start_end_date[1] = test_date_2;

						ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
						ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
						ITK_CALL(AOM_save(apl_eff_d));
						ITK_CALL(AOM_save(apl_release_status));
						ITK_CALL(AOM_refresh(apl_release_status, 0 ));
						printf("\n Eff and Release status stamped ..");fflush(stdout);
					}
			

				}
				if((strcmp(lcsToset,"T5_LcsSTDWrkg")==0))
				{
					
					printf("\n aplRlzdStatusFlg ..%d",aplRlzdStatusFlg);fflush(stdout);
					if(aplRlzdStatusFlg==0)
					{
						ITK_CALL(removeReleaseStatus(itemrev,lcsToset,ercStatusFlg,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,flagSetAPL));

						ITK_CALL(CR_create_release_status("T5_LcsAplRlzd",&apl_release_status));
						ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));

						if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
						{
							getNextDate(APLEffFromDate,FrmNextDate);
							printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);

							getNextDate(APLEffToDate,ToNextDate);
							printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

							ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
							ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
							
							start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


							start_end_date[0] = test_date_1;
							start_end_date[1] = test_date_2;

							ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
							ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
							ITK_CALL(AOM_save(apl_eff_d));
							ITK_CALL(AOM_save(apl_release_status));
							ITK_CALL(AOM_refresh(apl_release_status, 0 ));
							printf("\n APL Eff and Release status stamped ..");fflush(stdout);
						}
					}

					ITK_CALL(CR_create_release_status(lcsToset,&std_release_status));
					ITK_CALL(EPM_add_release_status(std_release_status,1,&itemrev,retain_std_released_date));

					
				}
				if((strcmp(lcsToset,"T5_LcsStdRlzd")==0))
				{
					ITK_CALL(removeReleaseStatus(itemrev,lcsToset,ercStatusFlg,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,flagSetAPL));
					
					printf("\n Inside STDRLZD Cond aplRlzdStatusFlg ..%d",aplRlzdStatusFlg);fflush(stdout);
					if(aplRlzdStatusFlg==0)
					{
						ITK_CALL(CR_create_release_status("T5_LcsAplRlzd",&apl_release_status));
						ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));

						if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
						{
							getNextDate(APLEffFromDate,FrmNextDate);
							getNextDate(APLEffToDate,ToNextDate);
							printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);
							printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

							ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
							ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
							
							start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


							start_end_date[0] = test_date_1;
							start_end_date[1] = test_date_2;

							ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
							ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
							ITK_CALL(AOM_save(apl_eff_d));
							ITK_CALL(AOM_save(apl_release_status));
							ITK_CALL(AOM_refresh(apl_release_status, 0 ));
							printf("\n 11 APL Eff and Release status stamped ..");fflush(stdout);
						}
					}


					ITK_CALL(CR_create_release_status(lcsToset,&std_release_status));
					ITK_CALL(EPM_add_release_status(std_release_status,1,&itemrev,retain_std_released_date));

					if((tc_strcmp(STDEffFromDate,"-")!=0) && (tc_strcmp(STDEffToDate,"-")!=0))
					{
						getNextDate(STDEffFromDate,FrmNextDate1);
						getNextDate(STDEffToDate,ToNextDate1);
						printf("\n FrmNextDate1:%s",FrmNextDate1);fflush(stdout);
						printf("\n ToNextDate1:%s",ToNextDate1);fflush(stdout);

						ITK_CALL(ITK_string_to_date(FrmNextDate1, &test_date_11 ));
						ITK_CALL(ITK_string_to_date(ToNextDate1, &test_date_21 ));
						
						start_end_date1 = (date_t *)MEM_alloc(sizeof(date_t)*2);


						start_end_date1[0] = test_date_11;
						start_end_date1[1] = test_date_21;

						ITK_CALL(WSOM_status_clear_effectivities ( std_release_status));
						ITK_CALL(WSOM_effectivity_create_with_dates(std_release_status, NULLTAG, 2,start_end_date1, EFFECTIVITY_closed, &std_eff_d ));
						ITK_CALL(AOM_save(std_eff_d));
						ITK_CALL(AOM_save(std_release_status));
						ITK_CALL(AOM_refresh(std_release_status, 0 ));
						printf("\n STD Eff and Release status stamped ..");fflush(stdout);
					}

					

				}

		
		
		}
		
  return status;
}

/*******FUNCTION TO Update Data on new created APL PART...************/

int setAttributesOnDesignRevAPL(tag_t* rev,int item_sequence_id,char *t5carMakBuy,char *t5carStoreLoc,char *desc,char *
parttype,char *projectcode,char *designgroup,char *mod_desc,char *DrawingNoS,char *DrawingIndS,char *Materialclass,char *MaterialInDrwS,char *MaterialThickNessS,char *DeignOwnUnitS,char *ModelIndS,char *
ColourIndS,char *WeightS,char *t5CMVRCertificationS,char *t5ClassificationHazS,char *t5ColourIDS,char *t5ConfigIDS,char *t5DismantableS,char *t5EnvelopeDimenS,char *t5HazardousContS,char *t5HomologationReqdS,char *
t5ListRecSparess,char *t5NcPartNoS,char *t5PartCodeS,char *t5PartPropertyS,char *t5PartStatusS,char *t5PkgStdS,char *t5ProductS,char *t5PrtCatCodeS,char *t5RecoverableS,char *t5RecyclabilityS,char *t5RefPartNumberS,char *t5ReliabilityS,char *
t5SpareCriteriaS,char *t5SpareIndS,char *t5SurfPrtStdS,char *t5SamplesToApprS,char *t5AsmDisposalS,char *t5FinDisposalInstrS,char *t5RPDisposalInstrS,char *t5SPDisposalInstrS,char *t5WIPDisposalInstrS,char *t5LastModByS,char *
t5VerCreatorS,char *t5CAEDocES,char *t5CoatedS,char *t5ConvDocS,char *t5AplCopyOfErcRevS,char *t5AplCopyOfErcSeqS,char *t5AppCodeS,char *t5RqstNumS,char *t5RsnCodeS,char *t5SurfaceAreaS,char *t5VolumeS,char *t5ErcIndNameS,char *t5PostRelReqS,char *
t5ItmCategoryS,char *t5CopReqS,char *t5AplInvalidateS,char *t5PrtValiStatusS,char *t5DRSubStateS,char *t5KnxtDocIndS,char *t5SimValS,char *t5PerYieldS,char *t5AltPartNoS,char *t5RolledupWtS,char *t5EstSheetReqdS,char *t5PFDModReqdS,char *
t5ToolIndentReqdS,char *CategoryNameS,char *GenPlantCS,char *getPlantStore)
{

	int status;
	double dweight=0;
	double d5ReliabilityS=0;
	double d5SurfaceAreaS=0;
	double d5VolumeS=0;
	tag_t projobj=NULLTAG;
	tag_t user_tag=NULLTAG;
	char* username=NULL;

	ITK_CALL(AOM_lock(*rev));
	printf("\n item_sequence_id --->%d\n",item_sequence_id);fflush(stdout);
	printf("\n t5carMakBuy --->%s\n",t5carMakBuy);fflush(stdout);
	printf("\n t5carStoreLoc --->%s\n",t5carStoreLoc);fflush(stdout);
	printf("\n desc --->%s\n",desc);fflush(stdout);
	printf("\n parttype --->%s\n",parttype);fflush(stdout);
	printf("\n GenPlantCS --->%s\n",GenPlantCS);fflush(stdout);
	printf("\n getPlantStore --->%s\n",getPlantStore);fflush(stdout);

	ITK_CALL ( AOM_set_value_int(*rev,"sequence_id",item_sequence_id));
	if((t5carMakBuy!=NULL) ||(tc_strcmp(t5carMakBuy," ")!=0)) ITK_CALL ( AOM_set_value_string(*rev,GenPlantCS,t5carMakBuy));
	if(t5carStoreLoc!=NULL) ITK_CALL ( AOM_set_value_string(*rev,getPlantStore,t5carStoreLoc));
	if(desc!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"object_desc",desc));
	if(parttype!=NULL)
	{
		if (tc_strcmp(parttype,"D")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","D"));
		}
		if (tc_strcmp(parttype,"A")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","A"));
		}
		if (tc_strcmp(parttype,"C")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","C"));
		}
		if (tc_strcmp(parttype,"VC")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","VC"));
		}
		if (tc_strcmp(parttype,"V")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","T"));
		}
		if (tc_strcmp(parttype,"T")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","V"));
		}
		if (tc_strcmp(parttype,"VCCR")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","VCCR"));
		}
		if (tc_strcmp(parttype,"G")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","G"));
		}
		if (tc_strcmp(parttype,"R")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","R"));
		}
		if (tc_strcmp(parttype,"SA")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","SA"));
		}
		if (tc_strcmp(parttype,"SP")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","SP"));
		}
		if (tc_strcmp(parttype,"PE")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","PE"));
		}
		if (tc_strcmp(parttype,"P")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","P"));
		}
		if (tc_strcmp(parttype,"M")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","M"));
		}
		if (tc_strcmp(parttype,"CM")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","CM"));
		}
		if (tc_strcmp(parttype,"DC")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","DC"));
		}
		if (tc_strcmp(parttype,"DA")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","DA"));
		}
		if (tc_strcmp(parttype,"IFD")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","IFD"));
		}
		if (tc_strcmp(parttype,"CR TPL")==0)
		{
			ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","CR TPL"));
		}
	}


	printf("\n projectcode --->%s\n",projectcode);fflush(stdout);
	printf("\n designgroup --->%s\n",designgroup);fflush(stdout);
	printf("\n mod_desc --->%s\n",mod_desc);fflush(stdout);
	printf("\n DrawingNoS --->%s\n",DrawingNoS);fflush(stdout);
	printf("\n DrawingIndS --->%s\n",DrawingIndS);fflush(stdout);
	printf("\n Materialclass --->%s\n",Materialclass);fflush(stdout);
	printf("\n MaterialInDrwS --->%s\n",MaterialInDrwS);fflush(stdout);
	printf("\n MaterialThickNessS --->%s\n",MaterialThickNessS);fflush(stdout);
	printf("\n DeignOwnUnitS --->%s\n",DeignOwnUnitS);fflush(stdout);


	if(projectcode!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_ProjectCode",projectcode));
	if(designgroup!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_DesignGrp",designgroup));
	if(mod_desc!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_DocRemarks",mod_desc));
	if(DrawingNoS!=NULL)ITK_CALL ( AOM_set_value_string(*rev,"t5_DrawingNo",DrawingNoS));
	if(DrawingIndS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_DrawingInd",DrawingIndS));
	//if((Materialclass!=NULL) ||(strcmp(Materialclass," ")!=0)) ITK_CALL ( AOM_set_value_string(*rev,"t5_MatlClass",Materialclass));
	if((MaterialInDrwS!=NULL)||(strcmp(MaterialInDrwS," ")!=0)) ITK_CALL ( AOM_set_value_string(*rev,"t5_Material",MaterialInDrwS));
	if(MaterialThickNessS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_MThickness",MaterialThickNessS));
	if(DeignOwnUnitS!=NULL)
	{
		ITK_CALL ( AOM_set_value_string(*rev,"t5_DsgnOwn",DeignOwnUnitS));
	}
	else
	{

	}
	printf("\n ModelIndS --->%s\n",ModelIndS);fflush(stdout);
	printf("\n ColourIndS --->%s\n",ColourIndS);fflush(stdout);

	if(ModelIndS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_ModelIndicator",ModelIndS));
	if(ColourIndS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_ColourInd",ColourIndS));
	printf("\n dweight --->%f\n",dweight);fflush(stdout);

	if(WeightS!=NULL)
	{
		dweight=atof(WeightS);
		ITK_CALL ( AOM_set_value_double(*rev,"t5_Weight",dweight));
	}


	printf("\n t5CMVRCertificationS --->%s\n",t5CMVRCertificationS);fflush(stdout);
	printf("\n t5ClassificationHazS --->%s\n",t5ClassificationHazS);fflush(stdout);
	printf("\n t5ColourIDS --->%s\n",t5ColourIDS);fflush(stdout);
	printf("\n t5ConfigIDS --->%s\n",t5ConfigIDS);fflush(stdout);
	printf("\n t5DismantableS --->%s\n",t5DismantableS);fflush(stdout);
	printf("\n t5EnvelopeDimenS --->%s\n",t5EnvelopeDimenS);fflush(stdout);
	printf("\n t5HazardousContS --->%s\n",t5HazardousContS);fflush(stdout);
	printf("\n t5HomologationReqdS --->%s\n",t5HomologationReqdS);fflush(stdout);
	printf("\n t5ListRecSparess --->%s\n",t5ListRecSparess);fflush(stdout);
	printf("\n t5NcPartNoS --->%s\n",t5NcPartNoS);fflush(stdout);
	printf("\n t5PartCodeS --->%s\n",t5PartCodeS);fflush(stdout);
	printf("\n t5PartPropertyS --->%s\n",t5PartPropertyS);fflush(stdout);

	
	if(strcmp(t5ClassificationHazS," ")==0)
	{
		if(strcmp(t5CMVRCertificationS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_CMVRCertificationReqd",false));
		}
		else if(strcmp(t5CMVRCertificationS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_CMVRCertificationReqd",true));
		}
	}
	if(strcmp(t5ClassificationHazS," ")==0)
	{
		printf("\n t5ClassificationHazS is blank");fflush(stdout);	
	}
	else
	{
		printf("\n t5ClassificationHazS is not blank");fflush(stdout);
		ITK_CALL ( AOM_set_value_string(*rev,"t5_ClassificationHazardous",t5ClassificationHazS));
	}
	if(strcmp(t5ColourIDS," ")!=0)ITK_CALL ( AOM_set_value_string(*rev,"t5_ColourID",t5ColourIDS));
	if(strcmp(t5ConfigIDS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_ConfigID",t5ConfigIDS));
	if(strcmp(t5DismantableS," ")!=0)
	{
		if(strcmp(t5DismantableS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_Dismantable",false));
		}
		else if(strcmp(t5DismantableS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_Dismantable",true));
		}
	}
	if(strcmp(t5EnvelopeDimenS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_EnvelopeDimensions",t5EnvelopeDimenS));
	if(strcmp(t5HazardousContS," ")!=0)
	{
		if(strcmp(t5HazardousContS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_HazardousContents",false));
		}
		else if(strcmp(t5HazardousContS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_HazardousContents",true));
		}
	}
	if(strcmp(t5HomologationReqdS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_HomologationReqd",t5HomologationReqdS));
	if(strcmp(t5ListRecSparess," ")!=0)
	{
		if(strcmp(t5ListRecSparess,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_ListRecSpares",false));
		}
		else if(strcmp(t5ListRecSparess,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_ListRecSpares",true));
		}
	}
	if(strcmp(t5NcPartNoS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_NcPartNo",t5NcPartNoS));
	if(strcmp(t5PartCodeS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PartCode",t5PartCodeS));
	if(strcmp(t5PartPropertyS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PartProperty",t5PartPropertyS));
	if(strcmp(t5PartStatusS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PartStatus",t5PartStatusS));
	if(strcmp(t5PkgStdS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PkgStd",t5PkgStdS));
	if(strcmp(t5ProductS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_Product",t5ProductS));
	//if(strcmp(t5PrtCatCodeS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PrtCatCode",t5PrtCatCodeS));
	if(strcmp(t5RecoverableS," ")!=0)
	{
		if(strcmp(t5RecyclabilityS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_Recoverable",false));
		}
		else if(strcmp(t5RecyclabilityS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_Recoverable",true));
		}
	}
	if(strcmp(t5RecyclabilityS," ")!=0)
	{
		if(strcmp(t5RecyclabilityS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_Recyclability",false));
		}
		else if(strcmp(t5RecyclabilityS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_Recyclability",true));
		}
	}
	if(strcmp(t5RefPartNumberS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_RefPartNumber",t5RefPartNumberS));
	if(strcmp(t5ReliabilityS," ")!=0)
	{
		d5ReliabilityS=atof(t5ReliabilityS);
		ITK_CALL ( AOM_set_value_double (*rev,"t5_Reliability",d5ReliabilityS));
	}
	//if(strcmp(t5SpareCriteriaS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_SpareCriteria",t5SpareCriteriaS));
	if(strcmp(t5SpareIndS," ")!=0)
	{
		if(strcmp(t5SpareIndS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_SpareInd",false));
		}
		else if(strcmp(t5SpareIndS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_SpareInd",true));
		}
	}
	if(strcmp(t5SurfPrtStdS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_SurfPrtStd",t5SurfPrtStdS));
	if(strcmp(t5SamplesToApprS," ")!=0)
	{
		if(strcmp(t5SamplesToApprS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_SamplesToAppr",false));
		}
		else if(strcmp(t5SamplesToApprS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_SamplesToAppr",true));
		}
	}
	if(strcmp(t5AsmDisposalS," ")==0)
	{
		printf("\n t5AsmDisposalS is blank");fflush(stdout);	
	}
	else
	{
		printf("\n t5AsmDisposalS is not blank");fflush(stdout);
		ITK_CALL ( AOM_set_value_string(*rev,"t5_AsmDisposalInstr",t5ClassificationHazS));
	}
	//if(t5AsmDisposalS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_AsmDisposalInstr",t5AsmDisposalS));
	
	if(strcmp(t5FinDisposalInstrS," ")==0)
	{
		printf("\n t5FinDisposalInstrS is blank");fflush(stdout);	
	}
	else
	{
		printf("\n t5FinDisposalInstrS is not blank");fflush(stdout);
		ITK_CALL ( AOM_set_value_string(*rev,"t5_FinDisposalInstr",t5FinDisposalInstrS));
	}
	//if(t5FinDisposalInstrS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_FinDisposalInstr",t5FinDisposalInstrS));
	
	//if(t5RPDisposalInstrS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_RPDisposalInstr",t5RPDisposalInstrS));
	//if(t5SPDisposalInstrS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_SPDisposalInstr",t5SPDisposalInstrS));
	//if(t5WIPDisposalInstrS!=NULL) ITK_CALL ( AOM_set_value_string(*rev,"t5_WIPDisposalInstr",t5WIPDisposalInstrS));
	
	if(strcmp(t5RPDisposalInstrS," ")==0)
	{
		printf("\n t5RPDisposalInstrS is blank");fflush(stdout);	
	}
	else
	{
		printf("\n t5RPDisposalInstrS is not blank");fflush(stdout);
		ITK_CALL ( AOM_set_value_string(*rev,"t5_RPDisposalInstr",t5RPDisposalInstrS));
	}

	if(strcmp(t5SPDisposalInstrS," ")==0)
	{
		printf("\n t5SPDisposalInstrS is blank");fflush(stdout);	
	}
	else
	{
		printf("\n t5SPDisposalInstrS is not blank");fflush(stdout);
		ITK_CALL ( AOM_set_value_string(*rev,"t5_SPDisposalInstr",t5SPDisposalInstrS));
	}

	if(strcmp(t5WIPDisposalInstrS," ")==0)
	{
		printf("\n t5WIPDisposalInstrS is blank");fflush(stdout);	
	}
	else
	{
		printf("\n t5WIPDisposalInstrS is not blank");fflush(stdout);
		ITK_CALL ( AOM_set_value_string(*rev,"t5_WIPDisposalInstr",t5WIPDisposalInstrS));
	}

	if(strcmp(t5VerCreatorS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_VerCreator",t5VerCreatorS));//Create it
	if(strcmp(t5CAEDocES," ")!=0)
	{
		if(strcmp(t5CAEDocES,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_CAEDocE",false));
		}
		else if(strcmp(t5CAEDocES,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_CAEDocE",true));
		}
	}
	if(strcmp(t5CoatedS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_Coated",t5CoatedS));
	if(strcmp(t5ConvDocS," ")!=0)
	{
		if(strcmp(t5ConvDocS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_ConvDoc",false));
		}
		else if(strcmp(t5ConvDocS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_ConvDoc",true));
		}
	}

	if(strcmp(t5AplCopyOfErcRevS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_AplCopyOfErcRev",t5AplCopyOfErcRevS));
	if(strcmp(t5AplCopyOfErcSeqS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_AplCopyOfErcSeq",t5AplCopyOfErcSeqS));
	if(strcmp(t5AppCodeS," ")==0)
	{
		ITK_CALL ( AOM_set_value_string(*rev,"t5_AppCode","NA"));
	}
	else
	{
		ITK_CALL ( AOM_set_value_string(*rev,"t5_AppCode",t5AppCodeS));	
	}
	if(strcmp(t5RqstNumS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_RqstNum",t5RqstNumS));
	if(strcmp(t5RsnCodeS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_RsnCode",t5RsnCodeS));
	if(strcmp(t5SurfaceAreaS," ")!=0)
	{
		d5SurfaceAreaS=atof(t5SurfaceAreaS);
		ITK_CALL ( AOM_set_value_double(*rev,"t5_SurfaceArea",d5SurfaceAreaS));
	}
	if(strcmp(t5VolumeS," ")!=0)
	{
		d5VolumeS=atof(t5VolumeS);
		ITK_CALL ( AOM_set_value_double(*rev,"t5_Volume",d5VolumeS));
	}
	if(strcmp(t5ErcIndNameS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_ErcIndName",t5ErcIndNameS));
	if(strcmp(t5PostRelReqS," ")!=0)
	{
		if(strcmp(t5PostRelReqS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PostRelReq",false));
		}
		else if(strcmp(t5PostRelReqS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_PostRelReq",true));
		}
	}
	if(strcmp(t5ItmCategoryS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_ItmCategory",t5ItmCategoryS));
	if(strcmp(t5CopReqS," ")!=0)
	{
		if(strcmp(t5CopReqS,"-")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_CopReq",false));
		}
		else if(strcmp(t5CopReqS,"+")==0)
		{
			ITK_CALL ( AOM_set_value_logical(*rev,"t5_CopReq",true));	
		}
	}
	if(strcmp(t5AplInvalidateS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_AplInvalidate",t5AplInvalidateS));//create it
	if(strcmp(t5PrtValiStatusS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PrtValiStatus",t5PrtValiStatusS));//create it
	if(strcmp(t5DRSubStateS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_DRSubState",t5DRSubStateS));//create it
	if(strcmp(t5KnxtDocIndS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_KnxtDocInd",t5KnxtDocIndS));//create it
	if(strcmp(t5SimValS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_SimVal",t5SimValS));//create it
	if(strcmp(t5PerYieldS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PerYield",t5PerYieldS));//create it
	if(strcmp(t5AltPartNoS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_AltPartNo",t5AltPartNoS));//create it
	if(strcmp(t5RolledupWtS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_RolledupWt",t5RolledupWtS));//create it
	if(strcmp(t5EstSheetReqdS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_EstSheetReqd",t5EstSheetReqdS));//create it
	if(strcmp(t5PFDModReqdS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_PFDModReqd",t5PFDModReqdS));//create it
	if(strcmp(t5ToolIndentReqdS," ")!=0) ITK_CALL ( AOM_set_value_string(*rev,"t5_ToolIndentReqd",t5ToolIndentReqdS));//create it

	ITK_CALL(AOM_save(*rev));
	ITK_CALL(AOM_unlock(*rev));


}

/*******FUNCTION TO CREATE EFF,STAMP LCS FOR ERC PARTS...************/

int CreateEffctivityERC(tag_t itemrev,char *lifeCycleS,char *APLEffFromDate,char *APLEffToDate,char *STDEffFromDate,char *STDEffToDate,FILE *fperror,FILE *fp_Efferror)
{

		int status;
		char* lcsToset=NULL;
		char* erclcsToset=NULL;
		char* aplWrklcsToset=NULL;
		char* aplRlzdlcsToset=NULL;
		char* stdWrklcsToset=NULL;
		char* stdRlzdlcsToset=NULL;
		char* aplReviewlcsToset=NULL;
		char* ercFlag=NULL;
		char* item_seq_upd=NULL;
		char* item_revision_id_upd=NULL;
		char* item_id_upd=NULL;
		char* class_name=NULL;
    	int	   status_count=0;
    	int	   StatusFlg=0;
    	int	   ss=0;
    	int	   ercStatusFlg=0;
    	int	   aplWrkStatusFlg=0;
    	int	   aplRlzdStatusFlg=0;
    	int	   stdWrkStatusFlg=0;
    	int	   stdRlzdStatusFlg=0;
    	int	   aplEffStamp=0;
		tag_t* status_list = NULLTAG;
		tag_t apl_release_status = NULLTAG;
		tag_t std_release_status = NULLTAG;
		logical retain_apl_released_date=false;
		logical retain_std_released_date=false;
		tag_t   status_rel                                      = NULLTAG;
		date_t test_date_1;
		date_t test_date_2;
		tag_t apl_eff_d = NULLTAG;
		char FrmNextDate[20]={0};
		char ToNextDate[20]={0};
		date_t *start_end_date = NULL;
		date_t test_date_11;
		date_t test_date_21;
		tag_t std_eff_d = NULLTAG;
		char FrmNextDate1[20]={0};
		char ToNextDate1[20]={0};
		date_t *start_end_date1 = NULL;



		lcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		erclcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		aplWrklcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		aplRlzdlcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		stdWrklcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		stdRlzdlcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		aplReviewlcsToset =(char *) MEM_alloc(20 * sizeof(char *));
		ercFlag =(char *) MEM_alloc(20 * sizeof(char *));

		if(strcmp(lifeCycleS,"LcsAPLWrkg")==0)
		{
			printf("\n This is apl wrk...");fflush(stdout);
			strcpy(lcsToset,APLWORKING);
		}
		else if(strcmp(lifeCycleS,"LcsAplRlzd")==0)
		{
			printf("\n This is apl rel...");fflush(stdout);
			strcpy(lcsToset,APLRELEASED);
		}
		else if(strcmp(lifeCycleS,"LcsSTDWrkg")==0)
		{
			printf("\n This is std wrk...");fflush(stdout);
			strcpy(lcsToset,STDWORKING);
		}
		else if(strcmp(lifeCycleS,"LcsSTDRlzd")==0)
		{
			printf("\n This is std rel...");fflush(stdout);
			strcpy(lcsToset,STDRELEASED);
		}

		strcpy(erclcsToset,ERCRELEASED);
		strcpy(aplWrklcsToset,APLWORKING);
		strcpy(aplRlzdlcsToset,APLRELEASED);
		strcpy(stdWrklcsToset,STDWORKING);
		strcpy(stdRlzdlcsToset,STDRELEASED);
		strcpy(aplReviewlcsToset,APLREVIEW);


		strcpy(ercFlag,"ERC");

		//ITK_CALL(AOM_ask_value_int(itemrev,"sequence_id",&item_seq_upd));
		//printf("\n Ckd Out seq is -->%d\n",item_seq_upd); fflush(stdout);

		ITK_CALL(AOM_ask_value_string(itemrev,"item_revision_id",&item_revision_id_upd));
		printf("\n Ckd Out Rev is -->%s\n",item_revision_id_upd); fflush(stdout);

		ITK_CALL(AOM_ask_value_string(itemrev,"item_id",&item_id_upd));

		
		ITK_CALL( WSOM_ask_release_status_list(itemrev,&status_count,&status_list));
		printf("\n REV status_count: %d\n",status_count);fflush(stdout);
		if (status_count == 0)  /* No Status, so the Item is not yet Released */
		{
			printf("\n No Status, so the Item is not yet Released \n");fflush(stdout);

			//fprintf(*fperror,"No Status found on Part:[%s],[%s],[%s]\n",item_id_upd,item_revision_id_upd,item_seq_upd);
			fprintf(fperror,"No Status found on Part :[%s],[%s]\n",item_id_upd,item_revision_id_upd);
			StatusFlg ++;

		}
		else
		{
			for(ss=0; ss<status_count ; ss++)
			{
				ITK_CALL(AOM_ask_value_string(status_list[ss],"object_name",&class_name));
				printf("\n class_name: %s\n",class_name);fflush(stdout);
				if(strcmp(class_name,lcsToset)==0)
				{
					StatusFlg ++;
				}
				else if(strcmp(class_name,erclcsToset)==0)
				{
					ercStatusFlg ++;
				}
				else if(strcmp(class_name,aplWrklcsToset)==0)
				{
					aplWrkStatusFlg ++;
				}
				else if(strcmp(class_name,aplRlzdlcsToset)==0)
				{
					aplRlzdStatusFlg ++;
				}
				else if(strcmp(class_name,stdWrklcsToset)==0)
				{
					stdWrkStatusFlg ++;
				}
				else if(strcmp(class_name,stdRlzdlcsToset)==0)
				{
					stdRlzdStatusFlg ++;
				}
				else if(strcmp(class_name,aplReviewlcsToset)==0)
				{
					aplWrkStatusFlg ++;
				}

			}
		}
		printf("\n StatusFlg: %d\n",StatusFlg);fflush(stdout);
		if(StatusFlg > 0)
		{
			printf("\n 22 StatusFlg: %d\n",StatusFlg);fflush(stdout);
			return status;
		}
		else
		{
			printf("\n11 StatusFlg: %d\n",StatusFlg);fflush(stdout);
			if(ercStatusFlg >0)
			{
				if((strcmp(lcsToset,"T5_LcsAPLWrkg")==0))
				{
					ITK_CALL(removeReleaseStatus(itemrev,lcsToset,ercStatusFlg,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,ercFlag));
					ITK_CALL(CR_create_release_status(lcsToset,&apl_release_status));
					ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));
					

				}
				if((strcmp(lcsToset,"T5_LcsAplRlzd")==0))
				{
										
					if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
					{
						
						ITK_CALL(removeReleaseStatus(itemrev,lcsToset,ercStatusFlg,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,ercFlag));

						ITK_CALL(CR_create_release_status(lcsToset,&apl_release_status));
						ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));


						getNextDate(APLEffFromDate,FrmNextDate);
						printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);

						getNextDate(APLEffToDate,ToNextDate);
						printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

						ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
						ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
						
						start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


						start_end_date[0] = test_date_1;
						start_end_date[1] = test_date_2;

						ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
						ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
						ITK_CALL(AOM_save(apl_eff_d));
						ITK_CALL(AOM_save(apl_release_status));
						ITK_CALL(AOM_refresh(apl_release_status, 0 ));
					}
					else
					{
						fprintf(fp_Efferror,"\n [%s],[%s],APL\n",item_id_upd,item_revision_id_upd);

					}
					
					printf("\n Eff and Release status stamped ..");fflush(stdout);
			

				}
				if((strcmp(lcsToset,"T5_LcsSTDWrkg")==0))
				{
					
					printf("\n aplRlzdStatusFlg ..%d",aplRlzdStatusFlg);fflush(stdout);
					if(aplRlzdStatusFlg==0)
					{
						
						if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
						{
							
							ITK_CALL(removeReleaseStatus(itemrev,lcsToset,ercStatusFlg,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,ercFlag));

							ITK_CALL(CR_create_release_status("T5_LcsAplRlzd",&apl_release_status));
							ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));

							getNextDate(APLEffFromDate,FrmNextDate);
							printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);

							getNextDate(APLEffToDate,ToNextDate);
							printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

							ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
							ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
							
							start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


							start_end_date[0] = test_date_1;
							start_end_date[1] = test_date_2;

							ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
							ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
							ITK_CALL(AOM_save(apl_eff_d));
							ITK_CALL(AOM_save(apl_release_status));
							ITK_CALL(AOM_refresh(apl_release_status, 0 ));

							ITK_CALL(CR_create_release_status(lcsToset,&std_release_status));
							ITK_CALL(EPM_add_release_status(std_release_status,1,&itemrev,retain_std_released_date));
						}
						else
						{
							fprintf(fp_Efferror,"\n [%s],[%s],APL\n",item_id_upd,item_revision_id_upd);

						}
						
						printf("\n APL Eff and Release status stamped ..");fflush(stdout);
					}
					else
					{
						ITK_CALL(CR_create_release_status(lcsToset,&std_release_status));
						ITK_CALL(EPM_add_release_status(std_release_status,1,&itemrev,retain_std_released_date));
						printf("\n STD Release status stamped ..");fflush(stdout);
					}

					

					
				}
				if((strcmp(lcsToset,"T5_LcsStdRlzd")==0))
				{
					ITK_CALL(removeReleaseStatus(itemrev,lcsToset,ercStatusFlg,aplWrkStatusFlg,aplRlzdStatusFlg,stdWrkStatusFlg,stdRlzdStatusFlg,ercFlag));
					
					printf("\n Inside STDRLZD Cond aplRlzdStatusFlg ..%d",aplRlzdStatusFlg);fflush(stdout);
					if(aplRlzdStatusFlg==0)
					{
						//printf("\n APL EFF DT IS NULL...length %d",tc_strlen(APLEffFromDate));fflush(stdout);

						if((tc_strcmp(APLEffFromDate,"-")!=0) && (tc_strcmp(APLEffToDate,"-")!=0))
						{
								ITK_CALL(CR_create_release_status("T5_LcsAplRlzd",&apl_release_status));
								ITK_CALL(EPM_add_release_status(apl_release_status,1,&itemrev,retain_apl_released_date));
						
								getNextDate(APLEffFromDate,FrmNextDate);
								getNextDate(APLEffToDate,ToNextDate);
								printf("\n FrmNextDate:%s",FrmNextDate);fflush(stdout);
								printf("\n ToNextDate:%s",ToNextDate);fflush(stdout);

								ITK_CALL(ITK_string_to_date(FrmNextDate, &test_date_1 ));
								ITK_CALL(ITK_string_to_date(ToNextDate, &test_date_2 ));
								
								start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);


								start_end_date[0] = test_date_1;
								start_end_date[1] = test_date_2;

								ITK_CALL(WSOM_status_clear_effectivities ( apl_release_status));
								ITK_CALL(WSOM_effectivity_create_with_dates(apl_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &apl_eff_d ));
								ITK_CALL(AOM_save(apl_eff_d));
								ITK_CALL(AOM_save(apl_release_status));
								ITK_CALL(AOM_refresh(apl_release_status, 0 ));
								aplEffStamp++;
							//}
						}
						else
						{
							fprintf(fp_Efferror,"\n [%s],[%s],APL\n",item_id_upd,item_revision_id_upd);

						}
						
						printf("\n 11 APL Eff and Release status stamped ..");fflush(stdout);
					}
				


					if((aplEffStamp>0)||(aplRlzdStatusFlg>0))
					{

						//if((tc_strcmp(STDEffFromDate,"-")!=0) && (tc_strcmp(STDEffToDate,"-")!=0))
						if((strcmp(STDEffFromDate,"-")!=0) && (strcmp(STDEffToDate,"-")!=0))
						{
							ITK_CALL(CR_create_release_status(lcsToset,&std_release_status));
							ITK_CALL(EPM_add_release_status(std_release_status,1,&itemrev,retain_std_released_date));
						
							getNextDate(STDEffFromDate,FrmNextDate1);
							getNextDate(STDEffToDate,ToNextDate1);
							printf("\n FrmNextDate1:%s",FrmNextDate1);fflush(stdout);
							printf("\n ToNextDate1:%s",ToNextDate1);fflush(stdout);

							ITK_CALL(ITK_string_to_date(FrmNextDate1, &test_date_11 ));
							ITK_CALL(ITK_string_to_date(ToNextDate1, &test_date_21 ));
							
							start_end_date1 = (date_t *)MEM_alloc(sizeof(date_t)*2);


							start_end_date1[0] = test_date_11;
							start_end_date1[1] = test_date_21;

							ITK_CALL(WSOM_status_clear_effectivities ( std_release_status));
							ITK_CALL(WSOM_effectivity_create_with_dates(std_release_status, NULLTAG, 2,start_end_date1, EFFECTIVITY_closed, &std_eff_d ));
							ITK_CALL(AOM_save(std_eff_d));
							ITK_CALL(AOM_save(std_release_status));
							ITK_CALL(AOM_refresh(std_release_status, 0 ));
						}
						else
						{
							fprintf(fp_Efferror,"\n [%s],[%s],STD\n",item_id_upd,item_revision_id_upd);

						}
					}
					
					printf("\n STD Eff and Release status stamped ..");fflush(stdout);

				}
			}

				
			
			else
			{
				fprintf(fperror,"ERC Release Status not found on Part :[%s],[%s]\n",item_id_upd,item_revision_id_upd);
				//printf("ERC Release Status not found on Part :[%s],[%s],[%s]\n",item_id_upd,item_revision_id_upd,item_seq_upd);
				printf("ERC Release Status not found on Part :[%s],[%s]",item_id_upd,item_revision_id_upd);fflush(stdout);
			}
		
		
		
		}
		

		

		
  return status;
}

/*******MAIN FUNCTION************/

extern int ITK_user_main (int argc, char ** argv )
{

   
	logical isCheckOut=false;
	FILE* fp=NULL;
	FILE* fperror=NULL;
	FILE* fp_partStampingList=NULL;
	FILE* fp_Efferror=NULL;

	char* inputline=NULL;
	char* inputpartline=NULL;
	char* level=NULL;
	char* item_id=NULL;
	char* item_revision_id=NULL;
	char* item_sequence_id=NULL;
	char* t5CarMakeBuyIndiS=NULL;
	char* t5CarStoreLoc=NULL;
	char* lifecycleS=NULL;
	char* APLEffFromDate=NULL;
	char* APLEffToDate=NULL;
	char* STDEffFromDate=NULL;
	char* STDEffToDate=NULL;
	char* partOrgID=NULL;
	char *itemRevSeq = NULL;
	char *partStampingFile=NULL;
	char *DmlListFile=NULL;
	char *PlantName=NULL;
	char *projectCode=NULL;
	char *timestamp=NULL;
	char *dml_no=NULL;
	char *dml_cnt=NULL;
	char *PartListName = NULL;
	char *desc=NULL;
	char *testVal=NULL;
	char *testdesc=NULL;
	char *parttype=NULL;
	char *projectcode=NULL;
	char *designgroup=NULL;
	char *mod_desc=NULL;
	char *DrawingNoS=NULL;
	char *DrawingIndS=NULL;
	char *Materialclass=NULL;
	char *MaterialInDrwS=NULL;
	char *MaterialThickNessS=NULL;
	char *LeftRhS=NULL;
	char *DeignOwnUnitS=NULL;
	char *ModelIndS=NULL;
	char *UnitOfMeasureS=NULL;
	char *ColourIndS=NULL;
	char *WeightS=NULL;
	char *t5CMVRCertificationS=NULL;
	char *t5ClassificationHazS=NULL;
	char *t5ColourIDS=NULL;
	char *t5ConfigIDS=NULL;
	char *t5DismantableS=NULL;
	char *t5DsgnDeptS=NULL;
	char *t5EnvelopeDimenS=NULL;
	char *t5HazardousContS=NULL;
	char *t5HomologationReqdS=NULL;
	char *t5ListRecSparess=NULL;
	char *t5NcPartNoS=NULL;
	char *t5PartCodeS=NULL;
	char *t5PartPropertyS=NULL;
	char *t5PartStatusS=NULL;
	char *t5PkgStdS=NULL;
	char *t5ProductS=NULL;
	char *t5PrtCatCodeS=NULL;
	char *t5RecoverableS=NULL;
	char *t5RecyclabilityS=NULL;
	char *t5RefPartNumberS=NULL;
	char *t5ReliabilityS=NULL;
	char *t5SpareCriteriaS=NULL;
	char *t5SpareIndS=NULL;
	char *t5SurfPrtStdS=NULL;
	char *t5SamplesToApprS=NULL;
	char *t5AsmDisposalS=NULL;
	char *t5FinDisposalInstrS=NULL;
	char *t5RPDisposalInstrS=NULL;
	char *t5SPDisposalInstrS=NULL;
	char *t5WIPDisposalInstrS=NULL;
	char *t5LastModByS=NULL;
	char *t5VerCreatorS=NULL;
	char *t5CAEDocES=NULL;
	char *t5CoatedS=NULL;
	char *t5ConvDocS=NULL;
	char *t5AplCopyOfErcRevS=NULL;
	char *t5AplCopyOfErcSeqS=NULL;
	char *t5AppCodeS=NULL;
	char *t5RqstNumS=NULL;
	char *t5RsnCodeS=NULL;
	char *t5SurfaceAreaS=NULL;
	char *t5VolumeS=NULL;
	char *t5ErcIndNameS=NULL;
	char *t5PostRelReqS=NULL;
	char *t5ItmCategoryS=NULL;
	char *t5CopReqS=NULL;
	char *t5AplInvalidateS=NULL;
	char *t5PrtValiStatusS=NULL;
	char *t5DRSubStateS=NULL;
	char *t5KnxtDocIndS=NULL;
	char *t5SimValS=NULL;
	char *t5PerYieldS=NULL;
	char *t5AltPartNoS=NULL;
	char *t5RolledupWtS=NULL;
	char *t5EstSheetReqdS=NULL;
	char *t5PFDModReqdS=NULL;
	char *t5ToolIndentReqdS=NULL;
	char *CategoryNameS=NULL;
	char *PartCreDateS=NULL;
	char *PartModDateS=NULL;
	char *PartCreator=NULL;
	char *GenPlantCS=NULL;
	char *getPlantStore=NULL;
	
	char * rel_list=NULL;
	int item_revision_id_int;
	const char *attrs[1];
	const char *values[1];

	int n_tags_found= 0;
	tag_t *tags_found = NULL;
	tag_t item=NULLTAG;
	tag_t rev=NULLTAG;
	tag_t aplitem=NULLTAG;
	tag_t aplrev=NULLTAG;
    int status;
	

	DmlListFile = ITK_ask_cli_argument("-i=");
	PlantName = ITK_ask_cli_argument("-plname=");
	

	ITK_CALL(ITK_auto_login( ));  

    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	ITK_CALL(ITK_set_journalling( TRUE ));
	
	printf("\n Auto login .......");fflush(stdout);

	if((tc_strcmp(PlantName,"PlantName1")==0) || (tc_strcmp(PlantName,"PlantName4")==0))
	{
		GenPlantCS=NULL;
		GenPlantCS=(char *) MEM_alloc(100);
		tc_strcpy(GenPlantCS,"t5_CarMakeBuyIndicator");

		getPlantStore=NULL;
		getPlantStore=(char *) MEM_alloc(100);
		tc_strcpy(getPlantStore,"t5_CarStoreLocation");
		printf("OPCS Name After Conversion ==> [%s]\n",GenPlantCS);fflush(stdout);

		
	}
	else if(tc_strcmp(PlantName,"PlantName13")==0)
	{
		GenPlantCS=NULL;
		GenPlantCS=(char *) MEM_alloc(100);
		tc_strcpy(GenPlantCS,"t5_PunUVMakeBuyIndicator");

		getPlantStore=NULL;
		getPlantStore=(char *) MEM_alloc(100);
		tc_strcpy(getPlantStore,"t5_PunUVStoreLocation");
		printf("OPCS Name After Conversion ==> [%s]\n",GenPlantCS);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName2")==0)
	{
		GenPlantCS=NULL;
		GenPlantCS=(char *) MEM_alloc(100);
		tc_strcpy(GenPlantCS,"t5_PnrMakeBuyIndicator");

		getPlantStore=NULL;
		getPlantStore=(char *) MEM_alloc(100);
		tc_strcpy(getPlantStore,"t5_PnrStoreLocation");
		printf("OPCS Name After Conversion ==> [%s]\n",GenPlantCS);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName3")==0)
	{
		GenPlantCS=NULL;
		GenPlantCS=(char *) MEM_alloc(100);
		tc_strcpy(GenPlantCS,"t5_PunMakeBuyIndicator");

		getPlantStore=NULL;
		getPlantStore=(char *) MEM_alloc(100);
		tc_strcpy(getPlantStore,"t5_PunStoreLocation");
		printf("OPCS Name After Conversion ==> [%s]\n",GenPlantCS);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName6")==0)
	{
		GenPlantCS=NULL;
		GenPlantCS=(char *) MEM_alloc(100);
		tc_strcpy(GenPlantCS,"t5_AhdMakeBuyIndicator");

		getPlantStore=NULL;
		getPlantStore=(char *) MEM_alloc(100);
		tc_strcpy(getPlantStore,"t5_AhdStoreLocation");
		printf("OPCS Name After Conversion ==> [%s]\n",GenPlantCS);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName8")==0)
	{
		GenPlantCS=NULL;
		GenPlantCS=(char *) MEM_alloc(100);
		tc_strcpy(GenPlantCS,"t5_JsrMakeBuyIndicator");

		getPlantStore=NULL;
		getPlantStore=(char *) MEM_alloc(100);
		tc_strcpy(getPlantStore,"t5_JsrStoreLocation");
		printf("OPCS Name After Conversion ==> [%s]\n",GenPlantCS);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName9")==0)
	{
		GenPlantCS=NULL;
		GenPlantCS=(char *) MEM_alloc(100);
		tc_strcpy(GenPlantCS,"t5_LkoMakeBuyIndicator");

		getPlantStore=NULL;
		getPlantStore=(char *) MEM_alloc(100);
		tc_strcpy(getPlantStore,"t5_LkoStoreLocation");
		printf("OPCS Name After Conversion ==> [%s]\n",GenPlantCS);fflush(stdout);
	}
	else if(tc_strcmp(PlantName,"PlantName11")==0)
	{
		GenPlantCS=NULL;
		GenPlantCS=(char *) MEM_alloc(100);
		tc_strcpy(GenPlantCS,"t5_DwdMakeBuyIndicator");

		getPlantStore=NULL;
		getPlantStore=(char *) MEM_alloc(100);
		tc_strcpy(getPlantStore,"t5_DwdStoreLocation");
		printf("OPCS Name After Conversion ==> [%s]\n",GenPlantCS);fflush(stdout);
	}
	else
	{
		printf("\nSYNTEX ERROR :: \n");
		printf("\n PLEASE ENTER CORRECT PLANTNAME(EX:PlantName1,PlantName13,PlantName2 etc..)\n");
		printf("\nTRY AGAIN !!!\n");
		return status;
	}

	fp=fopen(DmlListFile,"r");
	fperror=fopen("ERC_Part_Stamping_error.log","a");
	fp_Efferror=fopen("Effectivity_error.log","a");
	if(fp!=NULL)
	{
		inputline=(char *) MEM_alloc(1000);
		while(fgets(inputline,1000,fp)!=NULL)
		{
			fputs(inputline,stdout);		
					
			level=NULL;
			item_id=NULL;
			item_revision_id=NULL;
			item_sequence_id=NULL;
			t5CarMakeBuyIndiS=NULL;
			t5CarStoreLoc=NULL;
			lifecycleS=NULL;
			APLEffFromDate=NULL;
			APLEffToDate=NULL;
			STDEffFromDate=NULL;
			STDEffToDate=NULL;
			partOrgID=NULL;

			level=strtok(inputline,"^");  //1
			item_id=strtok(NULL,"^"); //2
			item_revision_id=strtok(NULL,"^"); //3
			item_sequence_id=strtok(NULL,"^"); //4
			t5CarMakeBuyIndiS=strtok(NULL,"^");//5
			t5CarStoreLoc=strtok(NULL,"^");//6
			lifecycleS=strtok(NULL,"^");//7			
			APLEffFromDate=strtok(NULL,"^");//8
			APLEffToDate=strtok(NULL,"^");//9
			STDEffFromDate=strtok(NULL,"^");//10
			STDEffToDate=strtok(NULL,"^");//11				
			partOrgID=strtok(NULL,"^");//5

			
			//0^2790210200R^B^1^ERC^E50^ ^LcsSTDRlzd^2002/06/26^2007/11/28^2007/12/05^9999/12/31^

			printf("level [%s]\n",level);fflush(stdout);
			printf("item_id [%s]\n",item_id);fflush(stdout);
			printf("item_revision_id [%s]\n",item_revision_id);fflush(stdout);
			printf("item_sequence_id [%s]\n",item_sequence_id);fflush(stdout);
			printf("t5CarMakeBuyIndiS [%s]\n",t5CarMakeBuyIndiS);fflush(stdout);
			printf("t5CarStoreLoc [%s]\n",t5CarStoreLoc);fflush(stdout);
			printf("lifecycleS [%s]\n",lifecycleS);fflush(stdout);
			printf("APLEffFromDate [%s]\n",APLEffFromDate);fflush(stdout);
			printf("APLEffToDate [%s]\n",APLEffToDate);fflush(stdout);
			printf("STDEffFromDate [%s]\n",STDEffFromDate);fflush(stdout);
			printf("STDEffToDate [%s]\n",STDEffToDate);fflush(stdout);
			printf("partOrgID [%s]\n",partOrgID);fflush(stdout);

			if((tc_strcmp(partOrgID,"ERC")!=0))
			{
				desc=strtok(NULL,"^"); //13
				testVal=strtok(NULL,"^"); //14
				testdesc=strtok(NULL,"^"); //15
				parttype=strtok(NULL,"^"); //16
				projectcode=strtok(NULL,"^"); //17
				designgroup=strtok(NULL,"^"); //18
				mod_desc=strtok(NULL,"^");//19
				DrawingNoS=strtok(NULL,"^");//20
				DrawingIndS=strtok(NULL,"^");//21
				Materialclass=strtok(NULL,"^");//22
				MaterialInDrwS=strtok(NULL,"^");//23
				MaterialThickNessS=strtok(NULL,"^");//24
				LeftRhS=strtok(NULL,"^");//25
				DeignOwnUnitS=strtok(NULL,"^");//26
				ModelIndS=strtok(NULL,"^");//27
				UnitOfMeasureS=strtok(NULL,"^");//28
				ColourIndS=strtok(NULL,"^");//29
				WeightS=strtok(NULL,"^");//30
				t5CMVRCertificationS=strtok(NULL,"^");//31
				t5ClassificationHazS=strtok(NULL,"^");//32
				t5ColourIDS=strtok(NULL,"^");//33
				t5ConfigIDS=strtok(NULL,"^");//34
				t5DismantableS=strtok(NULL,"^");//35
				t5DsgnDeptS=strtok(NULL,"^");//34
				t5EnvelopeDimenS=strtok(NULL,"^");//35
				t5HazardousContS=strtok(NULL,"^");//36
				t5HomologationReqdS=strtok(NULL,"^");//37
				t5ListRecSparess=strtok(NULL,"^");//38
				t5NcPartNoS=strtok(NULL,"^");//39
				t5PartCodeS=strtok(NULL,"^");//40
				t5PartPropertyS=strtok(NULL,"^");//41
				t5PartStatusS=strtok(NULL,"^");//42
				t5PkgStdS=strtok(NULL,"^");//43
				t5ProductS=strtok(NULL,"^");//44
				t5PrtCatCodeS=strtok(NULL,"^");//45
				t5RecoverableS=strtok(NULL,"^");//46
				t5RecyclabilityS=strtok(NULL,"^");//47
				t5RefPartNumberS=strtok(NULL,"^");//48
				t5ReliabilityS=strtok(NULL,"^");//49
				t5SpareCriteriaS=strtok(NULL,"^");//50
				t5SpareIndS=strtok(NULL,"^");//51
				t5SurfPrtStdS=strtok(NULL,"^");//52
				t5SamplesToApprS=strtok(NULL,"^");//53
				t5AsmDisposalS=strtok(NULL,"^");//54
				t5FinDisposalInstrS=strtok(NULL,"^");//55
				t5RPDisposalInstrS=strtok(NULL,"^");//56
				t5SPDisposalInstrS=strtok(NULL,"^");//57
				t5WIPDisposalInstrS=strtok(NULL,"^");//58
				t5LastModByS=strtok(NULL,"^");//59
				t5VerCreatorS=strtok(NULL,"^");//60
				t5CAEDocES=strtok(NULL,"^");//61
				t5CoatedS=strtok(NULL,"^");//62
				t5ConvDocS=strtok(NULL,"^");//63
				t5AplCopyOfErcRevS=strtok(NULL,"^");//64
				t5AplCopyOfErcSeqS=strtok(NULL,"^");//65
				t5AppCodeS=strtok(NULL,"^");//66
				t5RqstNumS=strtok(NULL,"^");//67
				t5RsnCodeS=strtok(NULL,"^");//68
				t5SurfaceAreaS=strtok(NULL,"^");//69
				t5VolumeS=strtok(NULL,"^");//70
				t5ErcIndNameS=strtok(NULL,"^");//71
				t5PostRelReqS=strtok(NULL,"^");//72
				t5ItmCategoryS=strtok(NULL,"^");//73
				t5CopReqS=strtok(NULL,"^");//74
				t5AplInvalidateS=strtok(NULL,"^");//75
				t5PrtValiStatusS=strtok(NULL,"^");//76
				t5DRSubStateS=strtok(NULL,"^");//77
				t5KnxtDocIndS=strtok(NULL,"^");//78
				t5SimValS=strtok(NULL,"^");//79
				t5PerYieldS=strtok(NULL,"^");//80
				t5AltPartNoS=strtok(NULL,"^");//81
				t5RolledupWtS=strtok(NULL,"^");//82
				t5EstSheetReqdS=strtok(NULL,"^");//83
				t5PFDModReqdS=strtok(NULL,"^");//84
				t5ToolIndentReqdS=strtok(NULL,"^");//85
				CategoryNameS=strtok(NULL,"^");//86
				PartCreDateS=strtok(NULL,"^");//87
				PartModDateS=strtok(NULL,"^");//88
				PartCreator=strtok(NULL,"^");//89
			}

			item_revision_id_int=atoi(item_sequence_id);
			attrs[0] ="item_id";
			values[0] = (char *)item_id;
			//Querying with item id
			ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));

			if(n_tags_found==0)
			{
					if((tc_strcmp(partOrgID,"ERC")==0))
					{
						printf("ERC PART NOT FOUND\n");fflush(stdout);
						fprintf(fperror,"Part number not found in TCUA :[%s],[%s],[%s],[%s]\n",item_id,item_revision_id,item_sequence_id,partOrgID);
					}
					else
					{
						printf("APL PART NOT FOUND.SO Calling APL PART Creation Code..\n");fflush(stdout);
						
						itemRevSeq = NULL;
						itemRevSeq=(char *) MEM_alloc(32);
						strcpy(itemRevSeq,item_revision_id);
						strcat(itemRevSeq,";");
						strcat(itemRevSeq,item_sequence_id);								


						ITK_CALL(ITEM_create_item(item_id,item_id,"Design",default_empty_to_A(itemRevSeq),&aplitem,&aplrev));
						
						ITK_CALL(AOM_save(aplrev));
						ITK_CALL(AOM_unlock(aplrev));

						//setAttributesOnDesign(&aplitem,desc,UnitOfMeasureS,LeftRhS);
						
						setAttributesOnDesignRevAPL(&aplrev,item_revision_id_int,t5CarMakeBuyIndiS,t5CarStoreLoc,desc,
						parttype,projectcode,designgroup,mod_desc,DrawingNoS,DrawingIndS,Materialclass,MaterialInDrwS,MaterialThickNessS,DeignOwnUnitS,ModelIndS,
						ColourIndS,WeightS,t5CMVRCertificationS,t5ClassificationHazS,t5ColourIDS,t5ConfigIDS,t5DismantableS,t5EnvelopeDimenS,t5HazardousContS,t5HomologationReqdS,
						t5ListRecSparess,t5NcPartNoS,t5PartCodeS,t5PartPropertyS,t5PartStatusS,t5PkgStdS,t5ProductS,t5PrtCatCodeS,t5RecoverableS,t5RecyclabilityS,t5RefPartNumberS,t5ReliabilityS,
						t5SpareCriteriaS,t5SpareIndS,t5SurfPrtStdS,t5SamplesToApprS,t5AsmDisposalS,t5FinDisposalInstrS,t5RPDisposalInstrS,t5SPDisposalInstrS,t5WIPDisposalInstrS,t5LastModByS,
						t5VerCreatorS,t5CAEDocES,t5CoatedS,t5ConvDocS,t5AplCopyOfErcRevS,t5AplCopyOfErcSeqS,t5AppCodeS,t5RqstNumS,t5RsnCodeS,t5SurfaceAreaS,t5VolumeS,t5ErcIndNameS,t5PostRelReqS,
						t5ItmCategoryS,t5CopReqS,t5AplInvalidateS,t5PrtValiStatusS,t5DRSubStateS,t5KnxtDocIndS,t5SimValS,t5PerYieldS,t5AltPartNoS,t5RolledupWtS,t5EstSheetReqdS,t5PFDModReqdS,
						t5ToolIndentReqdS,CategoryNameS,GenPlantCS,getPlantStore);

						setAttributesOnDesign(&aplitem,desc,UnitOfMeasureS,LeftRhS);

						
						ITK_CALL(CreateEffctivityAPL(aplrev,lifecycleS,APLEffFromDate,APLEffToDate,STDEffFromDate,STDEffToDate,fperror,fp_Efferror));

					}
					
					

			}else
			{
				item = tags_found[0];
				printf("Item already exists\n");fflush(stdout);

				itemRevSeq = NULL;
				itemRevSeq=(char *) MEM_alloc(32);
				strcpy(itemRevSeq,item_revision_id);
				strcat(itemRevSeq,";");
				strcat(itemRevSeq,item_sequence_id);

				printf(" itemRevSeq [%s]\n",itemRevSeq);fflush(stdout);

				ITK_CALL(ITEM_find_revision(item,itemRevSeq,&rev));
				if(rev != NULLTAG)
				{
					ITK_CALL(RES_is_checked_out(rev,&isCheckOut));
					if(isCheckOut)
					{
						fprintf(fperror,"Part number is Checked Out:[%s],[%s],[%s],[%s]\n",item_id,item_revision_id,item_sequence_id,partOrgID);
					}
					else
					{
						ITK_CALL(setAttributesOnDesignRev(&rev,t5CarMakeBuyIndiS,t5CarStoreLoc,GenPlantCS,getPlantStore,fperror));
					
						if((tc_strcmp(partOrgID,"ERC")==0))
						{
							printf(" Calling code for partOrgID [%s]\n",partOrgID);fflush(stdout);
							ITK_CALL(CreateEffctivityERC(rev,lifecycleS,APLEffFromDate,APLEffToDate,STDEffFromDate,STDEffToDate,fperror,fp_Efferror));
						}
						else
						{
							printf(" Calling code for partOrgID [%s]\n",partOrgID);fflush(stdout);
							ITK_CALL(CreateEffctivityAPL(rev,lifecycleS,APLEffFromDate,APLEffToDate,STDEffFromDate,STDEffToDate,fperror,fp_Efferror));
						}
					}

				}else
				{
							
					if((tc_strcmp(partOrgID,"ERC")==0))
					{
						printf("Item Revision not found\n");fflush(stdout);
						fprintf(fperror,"Part number Rev not found in TCUA:[%s],[%s],[%s],[%s] \n",item_id,item_revision_id,item_sequence_id,partOrgID);
					}
					else
					{
						printf("\n Create APL Part Rev... ");fflush(stdout);

						ITK_CALL(ITEM_create_rev(item,default_empty_to_A(itemRevSeq),&aplrev));
						ITK_CALL(AOM_save(aplrev));
						ITK_CALL(AOM_unlock(aplrev));
						setAttributesOnDesign(&item,desc,UnitOfMeasureS,LeftRhS);

						setAttributesOnDesignRevAPL(&aplrev,item_revision_id_int,t5CarMakeBuyIndiS,t5CarStoreLoc,desc,
						parttype,projectcode,designgroup,mod_desc,DrawingNoS,DrawingIndS,Materialclass,MaterialInDrwS,MaterialThickNessS,DeignOwnUnitS,ModelIndS,
						ColourIndS,WeightS,t5CMVRCertificationS,t5ClassificationHazS,t5ColourIDS,t5ConfigIDS,t5DismantableS,t5EnvelopeDimenS,t5HazardousContS,t5HomologationReqdS,
						t5ListRecSparess,t5NcPartNoS,t5PartCodeS,t5PartPropertyS,t5PartStatusS,t5PkgStdS,t5ProductS,t5PrtCatCodeS,t5RecoverableS,t5RecyclabilityS,t5RefPartNumberS,t5ReliabilityS,
						t5SpareCriteriaS,t5SpareIndS,t5SurfPrtStdS,t5SamplesToApprS,t5AsmDisposalS,t5FinDisposalInstrS,t5RPDisposalInstrS,t5SPDisposalInstrS,t5WIPDisposalInstrS,t5LastModByS,
						t5VerCreatorS,t5CAEDocES,t5CoatedS,t5ConvDocS,t5AplCopyOfErcRevS,t5AplCopyOfErcSeqS,t5AppCodeS,t5RqstNumS,t5RsnCodeS,t5SurfaceAreaS,t5VolumeS,t5ErcIndNameS,t5PostRelReqS,
						t5ItmCategoryS,t5CopReqS,t5AplInvalidateS,t5PrtValiStatusS,t5DRSubStateS,t5KnxtDocIndS,t5SimValS,t5PerYieldS,t5AltPartNoS,t5RolledupWtS,t5EstSheetReqdS,t5PFDModReqdS,
						t5ToolIndentReqdS,CategoryNameS,GenPlantCS,getPlantStore);

						

						ITK_CALL(CreateEffctivityAPL(aplrev,lifecycleS,APLEffFromDate,APLEffToDate,STDEffFromDate,STDEffToDate,fperror,fp_Efferror));
					
					}									
				}
			}

		}
		printf("----------------END OF PART-----------------------------------\n");fflush(stdout);

	}
	else
	{
		printf("PartListName [%s] is NULL..\n",PartListName);fflush(stdout);				
	}

	printf("-----------PART LOADING SUCCESS-----------------------------------------\n");fflush(stdout);
		
	
	//ITK_CALL(POM_logout(false));

	/*if (inputline)MEM_free(inputline);
	if (level)MEM_free(level);
	if(item_id)MEM_free(item_id);
	if(item_revision_id)MEM_free(item_revision_id);
	if(item_sequence_id)MEM_free(item_sequence_id);
	if(t5CarMakeBuyIndiS)MEM_free(t5CarMakeBuyIndiS);
	if(t5CarStoreLoc)MEM_free(t5CarStoreLoc);t5CarStoreLoc=NULL;
	if(lifecycleS)MEM_free(lifecycleS);lifecycleS=NULL;
	if(APLEffFromDate)MEM_free(APLEffFromDate);APLEffFromDate=NULL;
	if(APLEffToDate)MEM_free(APLEffToDate);APLEffToDate=NULL;
	if(STDEffFromDate)MEM_free(STDEffFromDate);STDEffFromDate=NULL;
	if(STDEffToDate)MEM_free(STDEffToDate);STDEffToDate=NULL;
	if(itemRevSeq)MEM_free(itemRevSeq);itemRevSeq=NULL;
	if(rel_list)MEM_free(rel_list);rel_list=NULL;
	if(fperror)fclose(fperror);fperror=NULL;*/

	
	return status;

   
}
 
