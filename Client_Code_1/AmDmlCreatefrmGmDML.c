/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  Author		  : Vishal Awari.
*  Created on	  : 1/06/2019
*  Module		  : AMDML Create For GMDML by STD User
*  Code			  : AmDmlCreatefrmGmDML.c
*
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes
   1      1 jun 19                  vishal              1) Change Qry value for Parts. removed Design Revision of object type
														2) Added ITK_CALL( TCTYPE_set_create_display_value(APLDCreInTag, "item_id", 1,(const char**)stringArrayAPLD) );
                                                        3) chnage log Dir to /tmp/

***************************************************************************/
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
int status=0;
int static totalPartCount=0;
int static flgChildPartDuplcy=0;
logical StatusFound =  false;
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
		}										\

#define TCTYPE_name_size_c 100
int stringsize =0;


// Structure Declaration
typedef enum MAIL_to_or_cc_e
{
	MAIL_send_to,
	MAIL_send_cc
}MAIL_to_or_cc_t;

struct BomChldStrut_Impl
{
	tag_t parent_objs;//CHild
	char  chldPartItmName[30];
	tag_t parent_objs_bvr;//BVR
	int parent_objs_lvl;//LVL
}*get_BomChldStrut_Impl;


struct structre_PartTagSet
{
	tag_t PartTag;
	char  PrtItemName[30];

};

struct struct_PartPRTagSet
{
	tag_t PartTag;
	char  PartName[30];
	char  partRevision[10];
};

struct struct_PartSetSkp
{
	tag_t PartTag;
	char  PartName[30];
	char  partRevision[10];
};


//Found Object in List

int FoundObjectInList(struct struct_PartSetSkp *iList,int Counter,char *PartRevName, char * partRevisionID )
{
	int			 Found		= 0;
	//ObjectList	 * iList	= *iFinalList;
	//*varExpObjTag			= NULLTAG;

	int setIndex=0;

	printf("\n Size of struct_PartSetSkp Set  %d",Counter+1);

	for(setIndex=0;setIndex<=Counter;setIndex++)
	{

		if(strcmp(iList[setIndex].PartName,PartRevName)==0 && strcmp(iList[setIndex].partRevision,partRevisionID) )
		{
			Found	=	1;
			break;
		}
	}

	if(Found==1)
	return 1;
	else
	return 0;
}



//End of Str
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
char * trim_space(char *str) {
    char *end;
    /* skip leading whitespace */
    while (isspace(*str)) {
        str = str + 1;
    }
    /* remove trailing whitespace */
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end = end - 1;
    }
    /* write null character */
    *(end+1) = '\0';
    return str;
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


void getCurrentDateTime(char cCurrentAccessDate[20])
{

	int ch;
	time_t temp;
	struct tm *timeptr;
	char pAccessDate[20];
	char	DateS[4];
	printf("\n getCurrentDateTime calling..........\n");
	temp = time(NULL);
	tc_strcpy(pAccessDate,"");
	timeptr = (struct tm *)localtime(&temp);
	ch = strftime(pAccessDate,sizeof(pAccessDate)-1,"%d-%b-%Y %H:%M",timeptr);
	//ch = strftime(pAccessDate,sizeof(pAccessDate)-1,"%d/%m/%Y",timeptr);
	tc_strcpy(cCurrentAccessDate,pAccessDate);
	printf("\n cCurrentAccessDate:%s\n",cCurrentAccessDate);

	//sprintf(DateS,"%d",(timeptr->tm_year+1900));
	printf("Date:%d\n",(timeptr->tm_mday));
	printf("Month:%d\n",(timeptr->tm_mon)+1);
	printf("Year:%d\n",(timeptr->tm_year+1900));
	fflush(stdout);
}

//Update Life Cycle of DML to APL


void getNextDate(char cnextAccessDate[30])
{
	int day, month, year, nd, nm, ny, ndays;
	int		ch1;
	int		ch;
	time_t	temp;
    date_t DayTommorow ;
 	struct tm *timeptr;
	struct tm *timeptr1;
	char pAccessDate[20];
	char pAccessDate1[20];

	char	strDay[20];
	char	strMon[20];
	char	strYear[20];
	char	DateS[20];
	char cMonth[30];
	temp = time(NULL);
	tc_strcpy(pAccessDate,"");
	timeptr = (struct tm *)localtime(&temp);
	ch = strftime(pAccessDate,sizeof(pAccessDate)-1,"%d-%b-%Y %H:%M",timeptr);

	day=timeptr->tm_mday;
	month=(timeptr->tm_mon)+1;
	year=(timeptr->tm_year+1900);
	ndays= days( month, year );//calling days function
	ny= year;
    nm= month;
    nd= day;
	if( ++nd > ndays )
	{
	   nd= 1;
	   if ( ++nm > 12 )
	   {
		 nm= 1;
		 ++ny;
		}
	}
    printf( "Given date is %d:%d:%d\n", day, month, year );
    printf( "Next days date is %d:%d:%d\n", nd, nm, ny );
	getMonth(nm,cMonth);
	printf( "cMonth:%s\n", cMonth);
	sprintf(strDay,"%d",nd);

	sprintf(strMon,"%d",nm);
	sprintf(strYear,"%d",ny);

	tc_strcpy(cnextAccessDate,strDay);
	tc_strcat(cnextAccessDate,"-");
	tc_strcat(cnextAccessDate,cMonth);
	tc_strcat(cnextAccessDate,"-");
	tc_strcat(cnextAccessDate,strYear);
	tc_strcat(cnextAccessDate," ");
	tc_strcat(cnextAccessDate,"00:00");
	printf("\n cnextAccessDate:%s\n",cnextAccessDate);
}


static int t5UpdateLifecycleStateAPL(tag_t object_tag, char* lifecycleStateName)
{
	int n_values_checkin = 0;
	int cunt1 = 0;
	int flag = 0;
	int				ifail=0;
	date_t *start_end_date = NULL;
	tag_t eff_d = NULLTAG;
	date_t test_date_1;
	date_t test_date_2;
	tag_t    	    propEff_tag				    =NULLTAG;
    logical stat  ;
	char*			value					=NULL;
	char cNextDate[20]={0};

	char *class_name = NULL;
	char *Release_Status = NULL;

	tag_t		class_id				=     NULLTAG;
	tag_t		tReleaseStatusList_checkin=NULLTAG;
	tag_t*		attr_tags_checkin		=	NULLTAG;
	tag_t			status_rel					= NULLTAG;
	tag_t*    status_list1=NULLTAG;
	int              st_count1 = 0;

	tag_t*    status_list;
	tag_t	*rev_list				= NULL;
	int       ii=0;
	int       RevFlag=0;
	int       PreRevFlag=0;
	int       st_count=0;
	int Item_count      =  0;
	char*			PreRevOnly				= NULL;
	char*			RevOnly				= NULL;
	char*			Part_Rev				= NULL;
	char*			Part_Rev_copy				= NULL;
	char*			Part_rev_Id				= NULL;
	char*			PlantLCSFlag				= NULL;
	int       iLCS=0;
	int       cntLcs=0;
	WSOM_open_ended_status_t  	open_ended_or_stock_out;
	char cCurrentAccessDate[20]={0};
	date_t Ltest_date_2;
	date_t *Rel_start_end_values	= NULL;
	int	n_dates 	= 0;
	int	n_effectivities 		= 0;
	int	jj 		= 0;
    char	*old_to_date			= NULL;
	logical date_is_valid  ;
	date_t Ltest_date_1;
	date_t *Lstart_end_date			= NULL;
	tag_t	Leff_d				= NULLTAG;
	tag_t	*effectivities_tag		= NULL;

	logical		*is_it_null_checkin		=   NULL;
	logical		*is_it_empty_checkin	=   NULL;
	logical		log1;
	logical		checkout1;
	logical			retain							= 0;
	int	iVldStamp	=	0;//Added by Hemal

	ITK_CALL(POM_class_of_instance(object_tag,&class_id));
	ITK_CALL(POM_name_of_class(class_id,&class_name));
	printf("\n class_name is :%s\n",class_name);fflush(stdout);

	ITK_CALL(POM_attr_id_of_attr("release_status_list",class_name,&tReleaseStatusList_checkin));fflush(stdout);
	ITK_CALL(WSOM_ask_release_status_list(object_tag,&st_count1,&status_list1));
	printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);


	ITK_CALL(POM_unload_instances (1,&object_tag));
	ITK_CALL(POM_load_instances(1,&object_tag,class_id,1));
	ITK_CALL(POM_is_loaded(object_tag,&log1));
	if(log1 == 1)
	{
		 printf(" Load Success\n " );
	}
	else
	printf("Load Failure\n"  );

	ITK_CALL( POM_length_of_attr(object_tag, tReleaseStatusList_checkin, &n_values_checkin) );
	printf("\n n_values is :%d\n",n_values_checkin);fflush(stdout);
	if(n_values_checkin>0)
	{
		ITK_CALL( POM_ask_attr_tags(object_tag, tReleaseStatusList_checkin, 0, n_values_checkin, &attr_tags_checkin, &is_it_null_checkin, &is_it_empty_checkin ));
		printf("\n n_values11 is :%d\n",n_values_checkin);fflush(stdout);
        flag = 0;
		for (cunt1 =  0; cunt1 < n_values_checkin; cunt1++)
		{
			printf("\n Inside POM_save_instances is :%d\n",cunt1);fflush(stdout);
			if(AOM_ask_name(attr_tags_checkin[cunt1], &Release_Status));
			printf("\n  Release_Status changes done: for workspaceobject     %s,lifecycleStateName : %s\n", Release_Status,lifecycleStateName);fflush(stdout);
            if(tc_strcmp(lifecycleStateName,"T5_LcsAplRlzd")==0)
            {
				if(tc_strcmp(Release_Status,"T5_LcsAplReview")==0 || (tc_strcmp(Release_Status,"T5_LcsAPLWrkg")==0) || (strcmp(Release_Status,"APLC Review")==0) || (strcmp(Release_Status,"APLC Working")==0))
				{
					ITK_CALL(POM_unload_instances (1,&object_tag));
					ITK_CALL(POM_load_instances(1,&object_tag,class_id,1));
					ITK_CALL(POM_is_loaded(object_tag,&log1));

					ITK_CALL(POM_remove_from_attr(1,&object_tag,tReleaseStatusList_checkin,flag,1));
					printf("\n cunt 1 == 0 removedd is :%d\n",cunt1);fflush(stdout);

					ITK_CALL(POM_save_instances(1,&object_tag,1));

					ITK_CALL(POM_refresh_instances(1,&object_tag,class_id,2));

					ITK_CALL(AOM_lock(object_tag));

					ITK_CALL(AOM_save(object_tag));
					ITK_CALL(AOM_refresh(object_tag,1));
					ITK_CALL(AOM_unlock(object_tag));
					PlantLCSFlag = "CAR";
					iVldStamp	=	iVldStamp + 1;
				}else
				{
					flag = flag+1;
				}
			}
			else  if(tc_strcmp(lifecycleStateName,"T5_LcsAplvRlzd")==0)
            {
			    if(tc_strcmp(Release_Status,"T5_LcsAplvReview")==0 || (tc_strcmp(Release_Status,"T5_LcsAPLvWrkg")==0) || (strcmp(Release_Status,"APLV Review")==0) || (strcmp(Release_Status,"APLV Working")==0) )
			      {
					ITK_CALL(POM_unload_instances (1,&object_tag));
					ITK_CALL(POM_load_instances(1,&object_tag,class_id,1));
					ITK_CALL(POM_is_loaded(object_tag,&log1));

					ITK_CALL(POM_remove_from_attr(1,&object_tag,tReleaseStatusList_checkin,flag,1));
					printf("\n cunt 1 == 0 removedd is :%d\n",cunt1);fflush(stdout);

					ITK_CALL(POM_save_instances(1,&object_tag,1));

					ITK_CALL(POM_refresh_instances(1,&object_tag,class_id,2));

					ITK_CALL(AOM_lock(object_tag));

					ITK_CALL(AOM_save(object_tag));
					ITK_CALL(AOM_refresh(object_tag,1));
					ITK_CALL(AOM_unlock(object_tag));
                    PlantLCSFlag = "PUV";
					iVldStamp	=	iVldStamp + 1;
				  }else
				  {
					flag = flag+1;
				  }
			}

		}

	}

	printf("\niVldStamp : %d",iVldStamp);fflush(stdout);

	//Code Commented by Vishal to make compulsary stamping of APL Released Statmping
	//if(iVldStamp>0)
	//{

	printf("\n Before Stamping Life Cycle  %s",lifecycleStateName);

	ITK_CALL(CR_create_release_status(lifecycleStateName,&status_rel));

	ITK_CALL(EPM_add_release_status(status_rel,1,&object_tag,1));

	//ITK_CALL(CR_add_release_status(object_tag,status_rel));

	tag_t	releaseDateId	= NULLTAG;

	ITK_CALL(AOM_refresh(status_rel,TRUE));
	ITK_CALL(POM_attr_id_of_attr("date_released", "WorkspaceObject", &releaseDateId));
	ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
	ITK_CALL (POM_set_attr_date(1, &status_rel, releaseDateId,NULLDATE));
	//ITK_CALL(POM_set_attr_string(1,&status_rel,releaseDateId,""));
	ITK_CALL(AOM_save(status_rel));
	ITK_CALL(AOM_refresh(status_rel,TRUE));


	/*ITK_CALL(AOM_set_value_date(status_rel, "date_released", ""));

	ITK_CALL(POM_save_instances(1, &status_rel, true));

	ITK_CALL(POM_refresh_instances_any_class(1, &status_rel, POM_no_lock));

	*/

	printf("\n After Stamping Life Cycle");
	//}

	if (iVldStamp>0 && (tc_strcmp(class_name,"Design_0_Revision_alt")==0 || tc_strcmp(class_name,"Design Revision")==0  || tc_strcmp(class_name,"T5_ClrPartRevision")==0))
	{
		ITK_CALL(WSOM_ask_effectivity_mode(&stat));

	//	getCurrentDateTime(cCurrentAccessDate);
		ITK_CALL(PROP_ask_property_by_name(status_rel,"effectivity_text",&propEff_tag));
		ITK_CALL(PROP_ask_value_string(propEff_tag,&value));

		getNextDate(cNextDate);
		printf("\n cNextDate:%s",cNextDate);

		ITK_CALL(ITK_string_to_date(cNextDate, &test_date_1 ));
		ITK_CALL(ITK_string_to_date("31-Dec-9999 00:00", &test_date_2 ));

		start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);

		printf( "\n Setting release date effectivity [to date - 12-Jun-2014 00:00 and from date - 12-Jun-2014 00:00]on ReleaseStatus object  \n");

		start_end_date[0] = test_date_1;
		start_end_date[1] = test_date_2;

		ITK_CALL(WSOM_status_clear_effectivities (status_rel));
		ITK_CALL(WSOM_effectivity_create_with_dates(status_rel, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &eff_d ));
		ITK_CALL(AOM_save(eff_d));
		ITK_CALL(AOM_save(status_rel));
		ITK_CALL(AOM_refresh(status_rel,0));

         // Start Privious Rev and stamp the close date
			tag_t		class_id1				=	NULLTAG;
			char		*class_name2			=	NULL;
			ITK_CALL(POM_class_of_instance(object_tag,&class_id1));
			ITK_CALL(POM_name_of_class(class_id,&class_name2));
			printf("\nclass_name2 : %s",class_name2);fflush(stdout);
			ITK_CALL(ITEM_list_all_revs (object_tag, &Item_count, &rev_list));
			printf("\n Eff:Total rev found: %d.", Item_count);fflush(stdout);
			if(Item_count > 0)
			{
				RevFlag = 0;
				PreRevFlag = 0;
				for(ii=Item_count-1;ii>=0;ii--)
				{
				   RevOnly = NULL;
				   PreRevOnly = NULL;
				   ITK_CALL(AOM_ask_value_string(rev_list[ii],"item_revision_id",&Part_rev_Id));
					printf("\n Part_Rev: %s\n",Part_Rev);fflush(stdout);
					printf("\n Part_rev_Id: %s\n",Part_rev_Id);fflush(stdout);
					if(tc_strcmp(Part_Rev,Part_rev_Id)==0)
					{
						RevFlag = 1;
					}
					printf("\n RevFlag : %d.", RevFlag);fflush(stdout);
					if(RevFlag == 1)
					{
					   RevOnly= strtok( Part_Rev_copy, ";" );
					   PreRevOnly= strtok( Part_rev_Id, ";" );
					   printf("\n RevOnly: %s\n",RevOnly);fflush(stdout);
					   printf("\n PreRevOnly: %s\n",PreRevOnly);fflush(stdout);
					   if(tc_strcmp(RevOnly,PreRevOnly)!=0)
						{
						  PreRevFlag = 1;
						}
					}
					printf("\n PreRevFlag : %d.", PreRevFlag);fflush(stdout);
					if(PreRevFlag == 1)
					{
						ITK_CALL(WSOM_ask_release_status_list(rev_list[ii],&st_count,&status_list));
						if(st_count == 0)
						{
						printf("\n NO LCS Found for part : %d\n",st_count);fflush(stdout);
						break;
						}
						else
						{
							for (iLCS=0;iLCS<st_count ;iLCS++ )
							{
									ITK_CALL(AOM_ask_value_string(status_list[iLCS],"object_name",&class_name));
									printf("\n class_name: %s\n",class_name);fflush(stdout);
									if(tc_strcmp(PlantLCSFlag,"CAR")==0)
									{
									if(tc_strcmp(class_name,"T5_LcsAplRlzd")==0)
									{
										getCurrentDateTime(cCurrentAccessDate);
										printf("\n Eff:cCurrentAccessDate is..:[%s]",cCurrentAccessDate);fflush(stdout);
										if(ITK_ok != (ifail = ITK_string_to_date(cCurrentAccessDate, &Ltest_date_2 )))
										{
											return ifail;
										}
										if(ITK_ok != (ifail = WSOM_status_ask_effectivities (status_list[iLCS],&n_effectivities, &effectivities_tag) )) return ifail;
										printf("\n Eff:n_effectivities count:[%d] \n",n_effectivities);fflush(stdout);
										if(n_effectivities > 0)
										{
											for(jj=0;jj<n_effectivities;jj++)
											{
												if(ITK_ok != (ifail = WSOM_effectivity_ask_dates(effectivities_tag[jj],&n_dates,&Rel_start_end_values,&open_ended_or_stock_out )))return ifail;
												printf("\n Eff:no of Rel_start_end_value:%d\n", n_dates);fflush(stdout);
												if(n_dates > 0)
												{
													if(ITK_ok != (ifail = DATE_date_to_string(Rel_start_end_values[0],"%d-%b-%Y %H:%M", &old_to_date )))
													{
														return ifail;
													}
													printf("\n Eff:old_to_date:[%s] ",old_to_date);fflush(stdout);
													if(ITK_ok != (ifail = DATE_string_to_date_t(old_to_date, &date_is_valid,&Ltest_date_1 )))
													{
														return ifail;
													}
													printf(" and  DATE_string_to_date_t..\n");fflush(stdout);
													if(date_is_valid != true)
													{
														printf("\n EFF1:date_is_valid is not true .....");fflush(stdout);
													}
													else
													{
														printf("\n EFF1:date_is_valid is  true .....");fflush(stdout);
													}
												}
												else
												{
													printf("\n EFF1:NO Eff date available, so setting closure date....");fflush(stdout);
												}
											}
										}
										Lstart_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);
										Lstart_end_date[0] = Ltest_date_1;
										Lstart_end_date[1] = Ltest_date_2;

										printf("\n ERCReview--EFF:Clearing effectivities...");fflush(stdout);
										if(ITK_ok != (ifail = WSOM_status_clear_effectivities (status_list[iLCS])))
										{
											return ifail;
										}

										printf("\n ERCReview--EFF:Creating effectivities...");fflush(stdout);
										if(ITK_ok != (ifail = WSOM_effectivity_create_with_dates(status_list[iLCS], NULLTAG, 2,Lstart_end_date, EFFECTIVITY_closed, &Leff_d )))
										{
											return ifail;
										}
										printf("\n ERCReview--EFF:Saving effectivities...");fflush(stdout);
										if(ITK_ok != (ifail = AOM_save(Leff_d)))
										{
											return ifail;
										}
										printf("\n ERCReview--EFF:Saving Revision...");fflush(stdout);
										if(ITK_ok != (ifail = AOM_save(status_list[iLCS])))
										{
											return ifail;
										}
										printf("\n ERCReview--EFF:Refreshing Revision...");fflush(stdout);
										if(ITK_ok != (ifail = AOM_refresh( status_list[iLCS], 0 )))
										{
											return ifail;
										}
										break;
									}
									}  //CAR end
									else // UV start
									if(tc_strcmp(PlantLCSFlag,"PUV")==0)
									{
									if(tc_strcmp(class_name,"T5_LcsAplvRlzd")==0)
									{
										getCurrentDateTime(cCurrentAccessDate);
										printf("\n Eff:cCurrentAccessDate is..:[%s]",cCurrentAccessDate);fflush(stdout);
										if(ITK_ok != (ifail = ITK_string_to_date(cCurrentAccessDate, &Ltest_date_2 )))
										{
											return ifail;
										}
										if(ITK_ok != (ifail = WSOM_status_ask_effectivities (status_list[iLCS],&n_effectivities, &effectivities_tag) )) return ifail;
										printf("\n Eff:n_effectivities count:[%d] \n",n_effectivities);fflush(stdout);
										if(n_effectivities > 0)
										{
											for(jj=0;jj<n_effectivities;jj++)
											{
												if(ITK_ok != (ifail = WSOM_effectivity_ask_dates(effectivities_tag[jj],&n_dates,&Rel_start_end_values,&open_ended_or_stock_out )))return ifail;
												printf("\n Eff:no of Rel_start_end_value:%d\n", n_dates);fflush(stdout);
												if(n_dates > 0)
												{
													if(ITK_ok != (ifail = DATE_date_to_string(Rel_start_end_values[0],"%d-%b-%Y %H:%M", &old_to_date )))
													{
														return ifail;
													}
													printf("\n Eff:old_to_date:[%s] ",old_to_date);fflush(stdout);
													if(ITK_ok != (ifail = DATE_string_to_date_t(old_to_date, &date_is_valid,&Ltest_date_1 )))
													{
														return ifail;
													}
													printf(" and  DATE_string_to_date_t..\n");fflush(stdout);
													if(date_is_valid != true)
													{
														printf("\n EFF1:date_is_valid is not true .....");fflush(stdout);
													}
													else
													{
														printf("\n EFF1:date_is_valid is  true .....");fflush(stdout);
													}
												}
												else
												{
													printf("\n EFF1:NO Eff date available, so setting closure date....");fflush(stdout);
												}
											}
										}
										Lstart_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);
										Lstart_end_date[0] = Ltest_date_1;
										Lstart_end_date[1] = Ltest_date_2;

										printf("\n ERCReview--EFF:Clearing effectivities...");fflush(stdout);
										if(ITK_ok != (ifail = WSOM_status_clear_effectivities (status_list[iLCS])))
										{
											return ifail;
										}

										printf("\n ERCReview--EFF:Creating effectivities...");fflush(stdout);
										if(ITK_ok != (ifail = WSOM_effectivity_create_with_dates(status_list[iLCS], NULLTAG, 2,Lstart_end_date, EFFECTIVITY_closed, &Leff_d )))
										{
											return ifail;
										}
										printf("\n ERCReview--EFF:Saving effectivities...");fflush(stdout);
										if(ITK_ok != (ifail = AOM_save(Leff_d)))
										{
											return ifail;
										}
										printf("\n ERCReview--EFF:Saving Revision...");fflush(stdout);
										if(ITK_ok != (ifail = AOM_save(status_list[iLCS])))
										{
											return ifail;
										}
										printf("\n ERCReview--EFF:Refreshing Revision...");fflush(stdout);
										if(ITK_ok != (ifail = AOM_refresh( status_list[iLCS], 0 )))
										{
											return ifail;
										}
										break;
									 }
									}
							}
						printf("\n cntLcs inside : %d\n",cntLcs);fflush(stdout);
						break;
						}
					}  /// End Prev Rev  Flag Loop
				}
			} /// End Prev Rev

	}else if(iVldStamp>0 && (tc_strcmp(class_name,"VariantRule")==0))
	{
		ITK_CALL(WSOM_ask_effectivity_mode(&stat));

	//	getCurrentDateTime(cCurrentAccessDate);
		ITK_CALL(PROP_ask_property_by_name(status_rel,"effectivity_text",&propEff_tag));
		ITK_CALL(PROP_ask_value_string(propEff_tag,&value));

		getNextDate(cNextDate);
		printf("\n cNextDate:%s",cNextDate);

		ITK_CALL(ITK_string_to_date(cNextDate, &test_date_1 ));
		ITK_CALL(ITK_string_to_date("31-Dec-9999 00:00", &test_date_2 ));

		start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);

		printf( "\n Setting release date effectivity [to date - 12-Jun-2014 00:00 and from date - 12-Jun-2014 00:00]on ReleaseStatus object  \n");

		start_end_date[0] = test_date_1;
		start_end_date[1] = test_date_2;

		ITK_CALL(WSOM_status_clear_effectivities (status_rel));
		ITK_CALL(WSOM_effectivity_create_with_dates(status_rel, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &eff_d ));
		ITK_CALL(AOM_save(eff_d));
		ITK_CALL(AOM_save(status_rel));
		ITK_CALL(AOM_refresh(status_rel,0));
	}

	return 0;
}

// AMDML Creation

 int AMDMLCreateFuncTOO(char  *project,char *dsgnGrp,char  *dvrNum,char  *WbsNameMain,char *WbsDescriptionMain,char *usrNameDup,int FlagPR,char *PlantName,tag_t *AMDmlObj)
 {
	tag_t			APLDTypeTag			=  NULLTAG;
	tag_t			APLDRevTypeTag		=  NULLTAG;
	tag_t			APLDCreInTag		=  NULLTAG;
	tag_t			APLDRevCreInTag		=  NULLTAG;

	//tag_t			APLDTypeTag			= NULLTAG;
	tag_t			TaskRevTag			= NULLTAG;
	tag_t 			APLDMLRevTag		= NULLTAG;
	tag_t			APLTTypeTag			= NULLTAG;
	tag_t			APLTCreInTag		= NULLTAG;
	tag_t			APLTRevTypeTag		= NULLTAG;
	tag_t			APLTRevCreInTag		= NULLTAG;

	tag_t	        Dml_tag		    	= NULLTAG;
	tag_t			APLDMLTag			= NULLTAG;
	tag_t			TaskTagrev		    = NULLTAG;
	tag_t		    APLTaskTag			= NULLTAG;
	tag_t 			APLTaskRevTag	    = NULLTAG;
	tag_t 			DmlEcnRlzTypeAttrID = NULLTAG;
	tag_t          *results			=  NULLTAG;
	tag_t          *AmRevObjSet 		=  NULLTAG;

	tag_t         relation_type        = NULLTAG;
	tag_t  	   apltaskrelation      = NULLTAG;
	tag_t  NewDMLAttrId = NULLTAG;
	tag_t  NewDMLRevAttrId = NULLTAG;


	char*			tempString			= NULL;

	char          *DmlNoRule			= NULL;
	char 		   *apldmlno			= NULL;
	char 		   *aplTaskno			= NULL;
	char 		   *Year                = NULL;

	char          *Proj_ID=NULL;
	char*			DMLAPL				= NULL;
	char*			Suffix				= NULL;
	char*          item_id 	   		= NULL;
	char 			*DML_no 			= NULL;
	char			**DesignGroupList	= NULL;
	int max_char_size 					= 100;



	char cCurrentAccessDate[20]		={0};
	WSO_search_criteria_t  	APLTaskCriteria;
	WSO_search_criteria_t  	APLDMLCriteria;


	char *Dml_Name 	  				 = NULL;
	char *DMLSerial						 =  NULL;
	char *DMLitem_id					 =  NULL;
	char *DmlLastSerial					 =(char *)MEM_alloc(max_char_size * sizeof(char));



	 //Integer declaration
	 int apl_dml_found=0,apl_task_number_found=0,index=0,n_entries=0,n_found=0;
	 int  DMLSerialInt;
	 int				l_strings			= 500;
	 status=0;

	 tag_t 		    AmDmlFindquery		= NULLTAG;

	 char**			stringArrayAPLD		= NULL;
	 char**			stringArrayAPLT		= NULL;
	 char*			tempStringt			= NULL;

	 char *item_id_apl_dml = (char *)MEM_alloc(max_char_size * sizeof(char));
	 char *item_id_apl_dml_str = (char *)MEM_alloc(max_char_size * sizeof(char));

	 char *item_id_apl_task = (char *)MEM_alloc(max_char_size * sizeof(char));
	 char *item_id_dml = (char *)MEM_alloc(max_char_size * sizeof(char));
	   //	char *item_id_dml_Str = (char *)MEM_alloc(max_char_size * sizeof(char));

	tag_t *list_of_WSO_tags			= NULLTAG;
	tag_t  NewTaskAttrId 				= NULLTAG;

	int n_strings=1;

	int      num_to_sort  = 1;
	char    *keys[1]      = {"creation_date"};
	int  	 orders[1]     = {2};

	stringArrayAPLD = (char**)malloc( n_strings * sizeof *stringArrayAPLD );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLD[index] = (char*)malloc( l_strings + 1 );
	}
	stringArrayAPLT = (char**)malloc( n_strings * sizeof *stringArrayAPLT );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLT[index] = (char*)malloc( l_strings + 1 );
	}
	printf("\n Inside the AMDML Creation Function"); fflush(stdout);

	printf("\n Inside the AMDML Creation Function  Plan Name : %s",PlantName); fflush(stdout);

		   // printf("2.Found Query \n");fflush(stdout);
	getCurrentDateTime(cCurrentAccessDate);
	printf("\n cCurrentAccessDate:%s ",cCurrentAccessDate);
	Year=subString(cCurrentAccessDate,9,2);

	if(strcmp(PlantName,"APLJ")==0)
	{
		strcpy(item_id_apl_dml,Year);
		strcat (item_id_apl_dml,"AM80");
		strcat (item_id_apl_dml,"*");
		printf("\n item_id_apl_dml:%s",item_id_apl_dml);
		strcpy(item_id_apl_dml_str,Year);
		strcat (item_id_apl_dml_str,"AM80");
	}
	else if(strcmp(PlantName,"APLL")==0)
	{
		strcpy(item_id_apl_dml,Year);
		strcat (item_id_apl_dml,"AM70");
		strcat (item_id_apl_dml,"*");
		printf("\n item_id_apl_dml:%s",item_id_apl_dml);
		strcpy(item_id_apl_dml_str,Year);
		strcat (item_id_apl_dml_str,"AM70");
	}
	else
	{
		strcpy(item_id_apl_dml,Year);
		strcat (item_id_apl_dml,"AM90");
		strcat (item_id_apl_dml,"*");
		printf("\n item_id_apl_dml:%s",item_id_apl_dml);
		strcpy(item_id_apl_dml_str,Year);
		strcat (item_id_apl_dml_str,"AM90");
	}

			//Qry Find to Get Latest AMDML Created

	ITK_CALL(QRY_find("APLDMLQuery", &AmDmlFindquery));
	n_entries=1;
	char
		 *qry_entries[1] = {"ID"},
		 *qry_values[1] =  {item_id_apl_dml};
	printf(" Print  User  values: %s\n",qry_values[0]);fflush(stdout);
			//printf(" Print entries and values: %s\n",qry_entries[1]);fflush(stdout);


	if(AmDmlFindquery!=NULLTAG)
	{
		  QRY_execute_with_sort(AmDmlFindquery, n_entries, qry_entries, qry_values, num_to_sort, keys, orders, &n_found, &results);

	}
	printf("\n Total AMDML Found in DB %d",n_found); fflush(stdout);

			//QRY_execute(AmDmlFindquery, n_entries, qry_entries, qry_values, &n_found, &AmRevObjSet);

	if(n_found>0)
	{
		Dml_tag = results[0];
		AOM_ask_value_string( Dml_tag, "item_id", &Dml_Name);
		printf("\n Dml_Name : %s\n",Dml_Name);fflush(stdout);
		DMLSerial=subString(Dml_Name,6,4);
		printf("\n DMLSerial : %s\n",DMLSerial);fflush(stdout);
		DMLSerialInt =atoi(DMLSerial);
		DMLSerialInt = DMLSerialInt + 1 ;
		printf("\n DMLSerialInt :%d\n",DMLSerialInt);fflush(stdout);
		sprintf(DmlLastSerial,"%d",DMLSerialInt);
		printf("\n DmlLastSerial : %s\n",DmlLastSerial);fflush(stdout);
		strcat (item_id_apl_dml_str,DmlLastSerial);
		strcat (item_id_apl_dml_str,"_");
		strcat (item_id_apl_dml_str,PlantName);
		printf("\n item_id_apl_dml_str:%s",item_id_apl_dml_str);
	}
	else
	{
		strcat (item_id_apl_dml_str,"1001");
		strcat (item_id_apl_dml_str,"_");
		strcat (item_id_apl_dml_str,PlantName);
	}
		//Search Formed String of  AM DML in Db
	WSOM_clear_search_criteria(&APLDMLCriteria);
	strcpy(APLDMLCriteria.name,item_id_apl_dml_str);
	strcpy(APLDMLCriteria.class_name,"T5_APLDMLRevision");
	status	= WSOM_search(APLDMLCriteria, &apl_dml_found, &list_of_WSO_tags);
	printf("\n apl_dml_found:%d",apl_dml_found);
		//ITK_CALL(AOM_ask_value_string(latestAMDMLtag,"item_revision_id",&partRevisionID));

	if(apl_dml_found==0)
	{

		ITK_CALL( TCTYPE_find_type("T5_APLDML", NULL, &APLDTypeTag) );
		ITK_CALL( TCTYPE_construct_create_input(APLDTypeTag, &APLDCreInTag) );
		ITK_CALL( TCTYPE_find_type("T5_APLDMLRevision", NULL, &APLDRevTypeTag) );
		ITK_CALL( TCTYPE_construct_create_input(APLDRevTypeTag, &APLDRevCreInTag) );

		printf("\n 11APL_dml_name is : %s\n",WbsNameMain);fflush(stdout);
		printf("\n 11APL_dml_desc is : %s\n",WbsDescriptionMain);fflush(stdout);
		printf("\n 11APLDML is : %s\n",item_id_apl_dml_str);fflush(stdout);

//		tc_strcpy( stringArrayAPLD[0], item_id_apl_dml_str);
		tc_strcpy( stringArrayAPLD[0], "18AM905300_APLC");
		ITK_CALL( TCTYPE_set_create_display_value(APLDCreInTag, "item_id", 1,(const char**)stringArrayAPLD) );

		tc_strcpy( stringArrayAPLD[0], WbsNameMain);
		printf("\n 11APLDML Name  is : %s\n",WbsNameMain);fflush(stdout);
		ITK_CALL( TCTYPE_set_create_display_value( APLDCreInTag, "object_name", 1, (const char**)stringArrayAPLD) );
		tc_strcpy( stringArrayAPLD[0], WbsDescriptionMain);
		printf("\n 11APLDML desc is : %s\n",WbsDescriptionMain);fflush(stdout);
		ITK_CALL( TCTYPE_set_create_display_value( APLDCreInTag, "object_desc", 1, (const char**)stringArrayAPLD) );
		ITK_CALL( AOM_tag_to_string(APLDRevCreInTag, &tempString) );
		tc_strcpy( stringArrayAPLD[0], tempString);
		printf("\nTest0.D1..[%s]\n",stringArrayAPLD[0]);fflush(stdout);
		ITK_CALL( TCTYPE_set_create_display_value( APLDCreInTag, "revision", 1,(const char**)stringArrayAPLD) );

		tc_strcpy( stringArrayAPLD[0], "A");
		ITK_CALL( TCTYPE_set_create_display_value( APLDRevCreInTag, "item_revision_id", 1, (const char**)stringArrayAPLD) );

		printf("\nTest1..\n");fflush(stdout);

		ITK_CALL( TCTYPE_create_object(APLDCreInTag, &APLDMLTag) );

		printf("\nTest2..\n");fflush(stdout);

		AOM_save(APLDMLTag);
 		ITK_CALL(ITEM_ask_latest_rev(APLDMLTag,&APLDMLRevTag));

		if (APLDMLTag != NULLTAG)
		{

			if(POM_attr_id_of_attr("item_id","T5_APLDML",&NewDMLAttrId));
			if(AOM_refresh(APLDMLTag,TRUE));
			if(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
			if(POM_set_attr_string(1,&APLDMLTag,NewDMLAttrId,item_id_apl_dml_str));
			if(AOM_save(APLDMLTag));
			if(AOM_refresh(APLDMLTag,TRUE));

		 }

		if (APLDMLRevTag != NULLTAG)
		{
			if(POM_attr_id_of_attr("item_id","T5_APLDMLRevision",&NewDMLRevAttrId));
			if(AOM_refresh(APLDMLRevTag,TRUE));
			if(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
			if(POM_set_attr_string(1,&APLDMLRevTag,NewDMLRevAttrId,item_id_apl_dml_str));
			if(AOM_save(APLDMLRevTag));
			if(AOM_refresh(APLDMLRevTag,TRUE));

			char *ECNTyep="TOODMLR";
			printf("Stamping ECN Type to TOODMLR"); fflush(stdout);
						 //ITK_CALL(AOM_set_value_string(APLDMLRevTag,"t5_EcnType",ECNTyep));

			ITK_CALL(POM_attr_id_of_attr("t5_EcnType","T5_APLDMLRevision",&DmlEcnRlzTypeAttrID));
			ITK_CALL(AOM_refresh(APLDMLRevTag,TRUE));
			ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
			ITK_CALL(POM_set_attr_string(1,&APLDMLRevTag,DmlEcnRlzTypeAttrID,ECNTyep));
			ITK_CALL(AOM_save(APLDMLRevTag));
			ITK_CALL(AOM_refresh(APLDMLRevTag,TRUE));

						/*

						ITK_CALL(POM_attr_id_of_attr("date_released","T5_APLTaskRevision",&TaskClosureDtAttrId));
						ITK_CALL(AOM_refresh(APLDMLRevTag,TRUE));
						ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
						ITK_CALL(POM_set_attr_date(1,&APLDMLRevTag,TaskClosureDtAttrId,Task_Release_date));
						*/
						ITK_CALL(AOM_lock(APLDMLRevTag));


						printf("\n Set Project on AMDML %s and Design Grp : %s ",project,dsgnGrp); fflush(stdout);
						ITK_CALL(AOM_set_value_string(APLDMLRevTag,"t5_cprojectcode",project));
						ITKCALL(AOM_set_value_string(APLDMLRevTag,"t5_rlstype","TPL"));
						
						if(AOM_save(APLDMLRevTag));//Added by Hemal
						ITK_CALL( AOM_unlock(APLDMLRevTag) );//Added by Hemal
						if(AOM_refresh(APLDMLRevTag,TRUE));//Added by Hemal
						if(FlagPR==1)
						   {
							  char **prDesignGrp=NULL;
							  NewDMLRevAttrId= NULLTAG;
							  n_strings=2;

							 prDesignGrp=(char **)malloc(n_strings * sizeof *prDesignGrp);

							 for( index=0; index<n_strings; index++ )
								{
									prDesignGrp[index] = (char*)malloc( l_strings + 1 );

								}
							 prDesignGrp[0]=dsgnGrp;
							 prDesignGrp[1]="PR";
								ITK_CALL(AOM_lock(APLDMLRevTag));//Added by Hemal
							  if(POM_attr_id_of_attr("t5_crdesigngroup","T5_APLDMLRevision",&NewDMLRevAttrId));
								if(AOM_refresh(APLDMLRevTag,TRUE));
								if(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
								//if(POM_set_attr_strings(1,&APLDMLRevTag,NewDMLRevAttrId,"PR"));
								 //ITK_CALL ( AOM_set_value_string_at(APLDMLRevTag,"t5_crdesigngroup",1,prDesignGrp));
								if(POM_set_attr_strings(1,&APLDMLRevTag,NewDMLRevAttrId,0,2,prDesignGrp));
								if(AOM_save(APLDMLRevTag));//Added by Hemal
								ITK_CALL( AOM_unlock(APLDMLRevTag) );//Added by Hemal
								if(AOM_refresh(APLDMLRevTag,TRUE));//Added by Hemal
						   }
						   else
						   {
							   ITK_CALL(AOM_lock(APLDMLRevTag));//Added by Hemal
							   ITK_CALL ( AOM_set_value_string_at(APLDMLRevTag,"t5_crdesigngroup",0,dsgnGrp));
								if(AOM_save(APLDMLRevTag));//Added by Hemal
								ITK_CALL( AOM_unlock(APLDMLRevTag) );
								if(AOM_refresh(APLDMLRevTag,TRUE));//Added by Hemal
						   }

							if(tc_strstr(item_id_apl_dml_str,"AM")!=NULL)
							{

								/*
								if(tc_strcmp(aplAMLastUpDtDup,"-")==0)
								{
									printf("\n Inside if date conversion..");fflush(stdout);
								}
								else
								{
									printf("\n Inside else date conversion..");fflush(stdout);
									getNextDate(aplAMLastUpDtDup,ChangeaplAMLastUpDtDup);

									ITK_CALL(ITK_string_to_date(ChangeaplAMLastUpDtDup, &apl_last_md_dt ));

									apl_last_md_dt_cpy = (date_t *)MEM_alloc(sizeof(date_t)*2);
									apl_last_md_dt_cpy[0] = apl_last_md_dt;
									ITK_CALL(AOM_set_value_date(APLDMLRevTag,"last_mod_date",apl_last_md_dt_cpy));
								}
								 */


								printf("\n Inside AMDML data updated\n");fflush(stdout);
								if(dvrNum!=NULL)
								{
									ITK_CALL( AOM_lock(APLDMLRevTag) );	//Added by Hemal	
								  ITK_CALL(AOM_set_value_string(APLDMLRevTag,"t5_DriverVC",dvrNum));
								  if(AOM_save(APLDMLRevTag));//Added by Hemal
								  ITK_CALL( AOM_unlock(APLDMLRevTag) );//Added by Hemal
									if(AOM_refresh(APLDMLRevTag,TRUE));//Added by Hemal
								}
							 }

						//ITK_CALL( AOM_save(APLDMLRevTag) );
						//ITK_CALL( AOM_unlock(APLDMLRevTag) );
				  }
				  else{

					  printf("\n AMDML Tag Not Found . EXIT From Functn"); fflush(stdout);

					  return status;
				   }
				//Create Task & Create Relation Inbetween DML & Task


				int num=0;

				//if(ITEM_ask_latest_rev(APLDMLRevTag,&TaskTagrev));
				printf("\n inside class T5_APLDML......\n");fflush(stdout);
				AOM_ask_value_string(APLDMLRevTag, "item_id", &item_id);

				AOM_ask_value_strings(APLDMLRevTag,"t5_crdesigngroup",&num,&DesignGroupList);
				AOM_ask_value_string(APLDMLRevTag, "t5_cprojectcode", &Proj_ID);

				printf("\n item_id : %s\n",item_id);fflush(stdout);
				printf("\n DML_no : %s\n",DML_no);fflush(stdout);
				printf("\n Design Grp Count is : %d\n",num);fflush(stdout);


			  int j=0;
			  for(j=0;j<num;j++)
				{

					DMLAPL= NULL;
					DML_no= NULL;


					tc_strcpy(item_id_apl_task,"");


					AOM_ask_value_string(APLDMLRevTag, "current_id", &DML_no);
					DMLAPL = strtok (DML_no,"_");
					Suffix = strtok (NULL,"_");


					printf("\n DMLAPL is : %s\n",DMLAPL);fflush(stdout);
					printf("\n Suffix is : %s\n",Suffix);fflush(stdout);

					strcpy(item_id_apl_task,DMLAPL);
					strcat (item_id_apl_task,"_");

					if(DesignGroupList!=NULL)
					{
						printf("\n Design Grp is %s",DesignGroupList[j]);

					   if(tc_strcmp(DesignGroupList[j],"")!=0)
						  {
							strcat (item_id_apl_task,DesignGroupList[j]);
						  }
						else
						{
							printf("\n Design Grp Not Found On APL DML revision "); fflush(stdout);

							status=0;
							return status;

						}
					}
				   else
					{

						   printf("\n DesignGroupList Tag Null"); fflush(stdout);

							status=0;
							return status;
					}


					strcat (item_id_apl_task,"_");
					strcat(item_id_apl_task,Suffix);

					printf("\n item_id_apl_task to Create :%s\n",item_id_apl_task);fflush(stdout);
			   // Serch AM Task Exist in DB or Not
				WSOM_clear_search_criteria(&APLTaskCriteria);
				strcpy(APLTaskCriteria.name,item_id_apl_task);
				strcpy(APLTaskCriteria.class_name,"T5_APLTaskRevision");
				status	= WSOM_search(APLTaskCriteria, &apl_task_number_found, &list_of_WSO_tags);

				printf("\n apl_task_number_found %d\n",apl_task_number_found);fflush(stdout);


				if(apl_task_number_found==0)
					{
							ITK_CALL( TCTYPE_find_type( "T5_APLTask", NULL, &APLTTypeTag) );
							ITK_CALL( TCTYPE_construct_create_input( APLTTypeTag, &APLTCreInTag) );

							ITK_CALL( TCTYPE_find_type( "T5_APLTaskRevision", NULL, &APLTRevTypeTag) );
							ITK_CALL( TCTYPE_construct_create_input( APLTRevTypeTag, &APLTRevCreInTag) );


							tc_strcpy( stringArrayAPLT[0], item_id_apl_task);
							ITK_CALL( TCTYPE_set_create_display_value( APLTCreInTag, "item_id", 1,(const char**)stringArrayAPLT) );

							tc_strcpy( stringArrayAPLT[0], WbsNameMain);

							ITK_CALL( TCTYPE_set_create_display_value( APLTCreInTag, "object_name", 1,(const char**)stringArrayAPLT) );

							tc_strcpy( stringArrayAPLT[0], WbsDescriptionMain);
							ITK_CALL( TCTYPE_set_create_display_value( APLTCreInTag, "object_desc", 1,(const char**)stringArrayAPLT) );


							ITK_CALL( AOM_tag_to_string(APLTRevCreInTag, &tempStringt) );
							tc_strcpy( stringArrayAPLT[0], tempStringt);
							printf("\nTest0.4..[%s]\n",stringArrayAPLT[0]);fflush(stdout);
							ITK_CALL( TCTYPE_set_create_display_value( APLTCreInTag, "revision", 1,(const char**) stringArrayAPLT) );
							tc_strcpy( stringArrayAPLT[0], "A");
							ITK_CALL( TCTYPE_set_create_display_value( APLTRevCreInTag, "item_revision_id", 1,(const char**)stringArrayAPLT) );
							tc_strcpy( stringArrayAPLT[0], dsgnGrp);
							ITK_CALL( TCTYPE_set_create_display_value( APLTRevCreInTag, "t5_crdesigngroup", 1,(const char**)stringArrayAPLT) );
							tc_strcpy( stringArrayAPLT[0], project);
							ITK_CALL( TCTYPE_set_create_display_value( APLTRevCreInTag, "t5_cprojectcode", 1,(const char**)stringArrayAPLT) );

							printf("\n Value to set in apl_task_number %s:%s:%s:%s:%s:\n",item_id_apl_task,item_id_apl_dml_str,tempStringt,dsgnGrp,project);fflush(stdout);

							ITK_CALL( TCTYPE_create_object( APLTCreInTag, &APLTaskTag) );
							ITK_CALL( AOM_save(APLTaskTag) );

							ITK_CALL(ITEM_ask_latest_rev(APLTaskTag,&APLTaskRevTag));

							if (APLTaskRevTag != NULLTAG)
							{

								printf("\n Calling APLDMLLCSDateStamp for stamping the task...\n");fflush(stdout);
								//ITK_CALL(APLDMLLCSDateStamp(APLTaskRevTag,dmlNoLCS,dmlAPLRlzdDt,dmlSTDSIRlzdDt,fperror));

								//tag_t		 TaskClosureDtAttrId=NULLTAG;
								//date_t		 Task_Release_date =NULLDATE;

								/*
								ITK_CALL(AOM_lock(APLTaskRevTag));

								ITK_CALL(POM_attr_id_of_attr("date_released","T5_APLTaskRevision",&TaskClosureDtAttrId));
								ITK_CALL(AOM_refresh(APLTaskRevTag,TRUE));
								ITK_CALL(POM_set_env_info(POM_bypass_attr_update,FALSE,0,0,NULLTAG,NULL));
								ITK_CALL(POM_set_attr_date(1,&APLTaskRevTag,TaskClosureDtAttrId,Task_Release_date));
								ITK_CALL(AOM_save(APLTaskRevTag));
								ITK_CALL(AOM_unlock(APLTaskRevTag));

								ITK_CALL(APLDMLLCSDateStamp(APLTaskRevTag,dmlTaskLCS,dmlTaskAPLRlzdDt,dmlTaskSTDRlzdDt,fperror));

								*/
							}


							if ((APLDMLRevTag != NULLTAG) && (APLTaskRevTag != NULLTAG))
							{


								int FndDMLTaskrelation=0;
								printf("\n CREATING REL OF APL DML & TASK...\n");fflush(stdout);

								ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation",&relation_type));
								ITK_CALL(GRM_find_relation(APLDMLRevTag, APLTaskRevTag, relation_type ,&FndDMLTaskrelation));
								if(FndDMLTaskrelation)
								{
									printf("\n\t Relation Already Exist b/w APLDML & Task :[%s],[%s]\n",item_id_apl_dml_str,item_id_apl_task);fflush(stdout);
									//fprintf(fpexception,"\n\t Relation Already Exist b/w APLDML & Task :[%s],[%s]\n",apldmlno,aplTaskno);
								}
								else
								{
									ITK_CALL(GRM_create_relation(APLDMLRevTag, APLTaskRevTag, relation_type,  NULLTAG, &apltaskrelation));
									ITK_CALL(GRM_save_relation(apltaskrelation));
								}

							}

							else{


								printf("\n Unable to CREATING REL OF APL DML & TASK...\n");fflush(stdout);

							}

					}
				}
			}

		if(list_of_WSO_tags!=NULLTAG)
		{
			MEM_free(list_of_WSO_tags);
		}
// APL AMDML Tag
 *AMDmlObj=APLDMLRevTag;
 status=1;

 return status;
}

//End of AMDML Creation

void  GetPlantWiseRelStat( char * item_id ,char  getLCSSTDWrkg[40],char  getLCSSTDRlzd[40])
{

	char*			Dsgn_No				= NULL;
	char*			PLT_Code			= NULL;

    printf( "item_id:%s\n", item_id);
	Dsgn_No=tc_strtok(item_id,"_");
	printf("\nTest0.11..Dsgn_No:[%s],[%s]\n",Dsgn_No,item_id);
	Dsgn_No = tc_strtok(NULL,"_");
	printf("\nTest0.12..Dsgn_No:[%s],[%s]\n",Dsgn_No,item_id);
	PLT_Code = tc_strtok(NULL,"_");
	printf("\nTest0.13..PLT_Code:[%s],[%s]\n",PLT_Code,item_id);

	//PLT_CodeS=(char *) MEM_alloc(100 * sizeof(char *));
	if(tc_strcmp(PLT_Code,"APLP")==0)
    {
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDpWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStdpRlzd");
	}else
	if(tc_strcmp(PLT_Code,"APLD")==0)
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDdWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStddRlzd");
	}else
	if(tc_strcmp(PLT_Code,"APLC")==0)
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStdRlzd");
	}else
	if(tc_strcmp(PLT_Code,"APLJ")==0)
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDjWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStdjRlzd");
	}else
	if(tc_strcmp(PLT_Code,"APLL")==0)
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDlWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStdlRlzd");
	}else
	if(tc_strcmp(PLT_Code,"APLA")==0)
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDaWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStdaRlzd");
	}else
	if(tc_strcmp(PLT_Code,"APLU")==0)
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDuWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStduRlzd");
	}else
	if(tc_strcmp(PLT_Code,"APLS")==0)
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDsWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStdsRlzd");
	}else
	if(tc_strcmp(PLT_Code,"APLV")==0)
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDvWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStdvRlzd");
	}else
	{
	tc_strcpy(getLCSSTDWrkg,"T5_LcsSTDpWrkg");
	tc_strcpy(getLCSSTDRlzd,"T5_LcsStdpRlzd");
	}

	printf( "getLCSSTDWrkg:%s\n", getLCSSTDWrkg);
	printf( "getLCSSTDRlzd:%s\n", getLCSSTDRlzd);

}

void  GetPlantForDMLORTask( char * item_id ,char  getPlant[40])
{
    printf( "item_id:%s\n", item_id);

	if(tc_strstr(item_id,"APLP")!=NULL)
    {
	tc_strcpy(getPlant,"APLP");

	}else
	if(tc_strstr(item_id,"APLD")!=NULL)
	{
	tc_strcpy(getPlant,"APLD");

	}else
	if(tc_strstr(item_id,"APLC")!=NULL)
	{
	tc_strcpy(getPlant,"APLC");

	}else
	if(tc_strstr(item_id,"APLJ")!=NULL)
	{
	tc_strcpy(getPlant,"APLJ");

	}else
	if(tc_strstr(item_id,"APLL")!=NULL)
	{
	tc_strcpy(getPlant,"APLL");
	}else
	if(tc_strstr(item_id,"APLA")!=NULL)
	{
	tc_strcpy(getPlant,"APLA");

	}else
	if(tc_strstr(item_id,"APLU")!=NULL)
	{
	tc_strcpy(getPlant,"APLU");

	}else
	if(tc_strstr(item_id,"APLS")!=NULL)
	{
	tc_strcpy(getPlant,"APLS");

	}else
	if(tc_strstr(item_id,"APLV")!=NULL)
	{
	tc_strcpy(getPlant,"APLV");

	}else
	{
	tc_strcpy(getPlant,"APLP");

	}

	printf( "getPlant:%s\n", getPlant);
}

/***************************************************************************************
*FUNCTION NAME: tm_fnd_Prev_Official_Revision
*ARGUMENT: *req_item(DESIGN REVISION - PART NUMBER), t_currentRevision(CURRENT REVISION), *t_previousRevision(RETURN PREVIOUS REVISION)
**req_item - QUERY THE DESIGN REVISION(PART).
*FIND THE ALL AVAILABLE REVISION OF THE DESIGN REVISION.
*t_currentRevision - FETCH THE CURRENT REVISION OF THE DESIGN REVISION FROM t_currentRevision, WHICH WILL USED TO COMPARE THE REVISION.
*COMPARE ALL AVAIALBE REVISION WITH CURRENT DESIGN REVISION, IF MATCH FOUND SET THE FLAG RevFlag =	1.
*ONCE THE RevFlag =	1, THEN COMPARE THE PREVIOUS REVISION WITH CURRENT REVISION AND SER THE FLAG PreRevFlag =	1.
*ONCE THE PreRevFlag =	1, THEN FIND THE LATEST SEQUENCE OF THE PREVIOUS REVISION AND RETURN WITH TAG t_previousRevision.
*IF t_currentRevision IS NR, THEN IT WILL BE HANDLE WILL CALLING THE FUNCTION.
*/
void tm_fnd_Prev_Official_Revision(char	*req_item,tag_t t_currentRevision, tag_t* t_previousRevision)
{
	int		Item_count			=	0;
	int		RevFlag				=	0;
	int		PreRevFlag 			=	0;
	int		ii					=	0;
	int		n_tags_found		=	0;

	char	*Part_Rev			=	NULL;
	char	*Part_Rev_copy		=	NULL;
	char	*Part_rev_Id		=	NULL;
	char	*RevOnly			=	NULL;
	char	*PreRevOnly			=	NULL;
	char	*Part_prev_rev_Id	=	NULL;

	tag_t	All_item_tag		=	NULLTAG;
	tag_t	*rev_list			=	NULLTAG;
	tag_t	*tags_found			=	NULLTAG;

	printf("\ntm_fnd_prev_revision called : %s",req_item);fflush(stdout);

	ITK_CALL(AOM_ask_value_string(t_currentRevision,"item_revision_id",&Part_Rev));
	printf("\n Part_Rev: %s\n",Part_Rev);fflush(stdout);

	ITK_CALL(AOM_ask_value_string(t_currentRevision,"item_revision_id",&Part_Rev_copy));
	printf("\n Part_Rev_copy: %s\n",Part_Rev_copy);fflush(stdout);

	//ITK_CALL(ITEM_list_all_revs (All_item_tag, &Item_count, &rev_list));
	ITK_CALL(AOM_ask_value_tags(t_currentRevision,"revision_list",&Item_count,&rev_list));
	printf("\nNo of Item : %d",Item_count);fflush(stdout);

	if(Item_count > 0)
	{
		RevFlag		=	0;
		PreRevFlag	=	0;

		for(ii=Item_count-1;ii>=0;ii--)
		{
			RevOnly		=	NULL;
			PreRevOnly	=	NULL;

			ITK_CALL(AOM_ask_value_string(rev_list[ii],"item_revision_id",&Part_rev_Id));

			printf("\nPart_Rev %s and Part_rev_Id: %s\n",Part_Rev,Part_rev_Id);fflush(stdout);
			if(tc_strcmp(Part_Rev,Part_rev_Id)==0)
			{
				RevFlag = 1;
			}
			printf("\n RevFlag : %d.", RevFlag);fflush(stdout);
			if(RevFlag == 1)
			{
				RevOnly		= strtok( Part_Rev_copy, ";" );
				PreRevOnly	= strtok( Part_rev_Id, ";" );
				printf("\n RevOnly: %s\n",RevOnly);fflush(stdout);
				printf("\n PreRevOnly: %s\n",PreRevOnly);fflush(stdout);
				if(tc_strcmp(RevOnly,PreRevOnly)!=0)
				{
					PreRevFlag = 1;
				}
				if(PreRevFlag == 1)
				{
					printf("\n PreRevFlag : %d.", PreRevFlag);fflush(stdout);
					ITK_CALL(AOM_ask_value_string(rev_list[ii],"item_revision_id",&Part_prev_rev_Id));
					printf("\n Part_prev_rev_Id: %s\n",Part_prev_rev_Id);fflush(stdout);
					*t_previousRevision	=	rev_list[ii];
					break;
				}
			}
		}
		if(PreRevFlag == 0)
		{
			*t_previousRevision	=	t_currentRevision;
		}
	}
}


static void MM_check_status( int count, tag_t *status_list, tag_t * revstatus)
{
	int   NStatus					 = 0;
	int   ifail						 = 0;
	char  name[WSO_name_size_c+1]    = {'\0'};

	printf("\n Inside MM_check_status");fflush(stdout);
	for(NStatus=0;NStatus<count;NStatus++)
	{
		    ifail=CR_ask_release_status_type(status_list[NStatus],name);
		    printf("\n MM_check_status:: name==>[%s]",name);fflush(stdout);

		  //if(tc_strcmp(name,"T5_LcsSTDWrkg")==0|| tc_strcmp(name,"T5_LcsAplRlzd")==0 || tc_strcmp(name,"T5_LcsErcRlzd")==0 )
		  if(tc_strcmp(name,"T5_LcsSTDWrkg")==0|| tc_strcmp(name,"T5_LcsAplRlzd")==0)//Changed by Hemal

			{
			     StatusFound = true;
				*revstatus=status_list[NStatus];
			     break;
            }
	}
}



//Expand Bom Recursively
void Multi_Get_Part_BOM_Lvl_Impl(tag_t inputPart,int reqLevel,int level,tag_t closure_tag,tag_t revRule,char ViewBVR[100],tag_t *returnPartsInVc,struct BomChldStrut_Impl BomChldStrutImpl[] ,int* StructParentCnt)
{
	int ifail;
    int iChildItemTag=0;
	char * ItemName ;
	char * childItemRevName=NULL;
	char * childItemRevision=NULL;
	int k=0;
    int n=0;
	int c=0;
    int assbvr=0;
    int flagBVR=0;
	int 	n_values_bvr=0;
	tag_t	*bvr_assy_part			= NULLTAG;
	tag_t	 bvr_tag				= NULLTAG;
	tag_t   t_ChildItemRev;
	tag_t*	childrenTag				= NULLTAG;
	char *view_name					= NULL;
	char *ParetnName				= NULL;
	tag_t	window 					= NULLTAG;
	char*			partTok			= NULL;
	char*			viewTok			= NULL;
	tag_t	top_line			    = NULLTAG;
	int bvrfound=0;
	tag_t  *children				= NULLTAG;
	char* partNumber 				= NULL;
	int  	n_parents=0;
	int  	jp=0;
	int * 	levels;
	tag_t * 	parents	 			= NULLTAG;
	tag_t 	parents_tag	 			= NULLTAG;
	tag_t	childPartTag			= NULLTAG;



	//printf("\n Inside Multi_Get_Part_BOM_Lvl_Impl[%d] ...\n",*StructParentCnt);

	ITK_CALL(AOM_ask_value_string(inputPart,"item_id",&partNumber));
	printf("\n Part number===>%s\n",partNumber);fflush(stdout);

	printf("\n ViewBVR=>[%s] ...\n",ViewBVR);

	if( level >= reqLevel )
	{
		goto CLEANUP;
	}

	//ITK_CALL(PS_where_used_configured(inputPart,revRule,1,&n_parents,&levels,&parents));
	//printf("\n\n\t\t No of Assy objects are n_parents : %d\n",n_parents);fflush(stdout);

	level = level + 1;

	/*if(n_parents>0)
	{
		for(jp=0;jp<n_parents;jp++)
		{
           */
		    //parents_tag=parents[jp];

			parents_tag=inputPart;

			ITK_CALL(AOM_ask_value_string(parents_tag,"item_id",&ParetnName));
			printf("\n Fparents %s",ParetnName);fflush(stdout);
			if(parents_tag!=NULLTAG)
			{
				ITK_CALL(ITEM_rev_list_bom_view_revs(parents_tag, &n_values_bvr, &bvr_assy_part));
				printf("\n n_values_bvr=>[%d] ",n_values_bvr);fflush(stdout);

				if(n_values_bvr==0)
				{
					flagBVR=0;
				}
				else if(n_values_bvr>0)
				{
					printf("\n BVR found");fflush(stdout);
					for(assbvr=0;assbvr<n_values_bvr;assbvr++)
					{
						ITK_CALL(AOM_ask_value_string(bvr_assy_part[assbvr],"object_name",&view_name));
						printf("\n view_name %s",view_name);fflush(stdout);
						partTok = strtok (view_name,"-");
						viewTok = strtok (NULL,"-");
						printf("\n viewTok %s",viewTok);fflush(stdout);
						if(strlen(viewTok)>0)
						{
							//if(tc_strcmp(viewTok,"View")==0)
							if(tc_strcmp(viewTok,ViewBVR)==0)
							{
								flagBVR=1;
								bvr_tag=bvr_assy_part[assbvr];
								break;
							}
						}
					}
					if(flagBVR!=1)
					{
						for(assbvr=0;assbvr<n_values_bvr;assbvr++)
						{
							ITK_CALL(AOM_ask_value_string(bvr_assy_part[assbvr],"object_name",&view_name));
							printf("\n view_name %s",view_name);fflush(stdout);
							partTok = strtok (view_name,"-");
							viewTok = strtok (NULL,"-");
							printf("\n viewTok %s",viewTok);fflush(stdout);
							if(strlen(viewTok)>0)
							{
								//if(tc_strcmp(viewTok,ViewBVR)==0)
								if(tc_strcmp(viewTok,"View")==0)
								{
									flagBVR=1;
									bvr_tag=bvr_assy_part[assbvr];
									break;
								}
							}
						}
					}

				}
				printf("\n flagBVR=>[%d] ",flagBVR);fflush(stdout);

				ITK_CALL(BOM_create_window (&window));
				ITK_CALL(BOM_set_window_config_rule(window,revRule));
				ITK_CALL(BOM_window_set_closure_rule(window,closure_tag,0,NULL,NULL));

				if(flagBVR==1)
				{
					BOM_set_window_top_line(window, null_tag,parents_tag ,bvr_tag, &top_line);
					BOM_window_show_suppressed ( window );//TZ 3.42 Backend
					ITK_CALL(BOM_line_ask_child_lines (top_line, &n, &children));
					printf("\n\n\t\t No of child objects are n : %d\n",n);fflush(stdout);

					for (k = 0; k < n; k++)
					{
						BOM_line_unpack (children[k]);
						BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
						BOM_line_ask_attribute_tag(children[k], iChildItemTag, &t_ChildItemRev);
						if(t_ChildItemRev!=NULLTAG)
						{
							AOM_ask_value_string(t_ChildItemRev,"item_id",&childItemRevName);
							AOM_ask_value_string(t_ChildItemRev,"item_revision_id",&childItemRevision);

							printf("\n ChildPrtNumP=>[%s] ",childItemRevName);fflush(stdout);
							printf("\t Revision:[%s] ",childItemRevision);fflush(stdout);

							if(strcmp(childItemRevName,partNumber)!=0)
							{

							    flgChildPartDuplcy=0;

							   	for(c=0; c<=*StructParentCnt;c++)
                                   {

									   if(strcmp(BomChldStrutImpl[*StructParentCnt].chldPartItmName,childItemRevName)==0)
									   {

										   flgChildPartDuplcy=1;
										   break;
									   }

								   }

								//if Part Not Present in Set then only it will be added in Set else Skip
								if(flgChildPartDuplcy==0)
								{

									*StructParentCnt	=	*StructParentCnt+1;

									childPartTag=t_ChildItemRev;
								    BomChldStrutImpl[*StructParentCnt].parent_objs 		   = childPartTag;
									tc_strcpy(BomChldStrutImpl[*StructParentCnt].chldPartItmName,childItemRevName);

							     	//BomChldStrutImpl[*StructParentCnt].parent_objs_bvr=children[k];
									//BomChldStrutImpl[*StructParentCnt].parent_objs_lvl=level;

							       Multi_Get_Part_BOM_Lvl_Impl(childPartTag,reqLevel,level,closure_tag,revRule,ViewBVR,returnPartsInVc,BomChldStrutImpl,StructParentCnt);
								}
								else{
									printf("\n Part Already Available in Set. Part Skipped to add in Set"); fflush(stdout);
								}
							}
							else
						    {
								printf("\n*******INSIDE SAME FOUND*****\n");fflush(stdout);
								//returnPartsInVc[totalPartCount]=
								//totalPartCount++;

								/*
								*StructParentCnt	=	*StructParentCnt+1;
								BomChldStrutImpl[*StructParentCnt].parent_objs = parents_tag;
								BomChldStrutImpl[*StructParentCnt].parent_objs_bvr=children[k];
								BomChldStrutImpl[*StructParentCnt].parent_objs_lvl=level;
								*/
								break;
							}
						}
					}
				}

				//Multi_Get_Part_BOM_Lvl_Impl(parents_tag,reqLevel,level,closure_tag,revRule,ViewBVR,returnPartsInVc);

			}//ParentRev Not-NULL

		//}
	//}

	level = level - 1;
	CLEANUP:
		 printf("\n Inside Multi_Get_Part_BOM_Lvl_Impl CLEANUP");fflush(stdout);
}

int Get_Part_BOM_Lvl_Impl(tag_t partObj,int reqLevel,char*  closureRuleName,char*  revRuleName,char* ViewBVR,tag_t *returnPartsInVc,struct BomChldStrut_Impl BomChldStrutImpl[],int* StructParentCnt)
{

	int   ifail				= 0;

	tag_t revRule 			= NULLTAG;
	char* prtNumber 		= NULL;
	int j					= 0;
	PIE_scope_t scope;
	int 	n_closure_tags;
	tag_t * 	closure_tags;
	tag_t  	closure_tag;
	int level 					= 0;
	tag_t	objParent			= NULLTAG;
	tag_t	objParent_bvr		= NULLTAG;
	int Parent_lvl				= 0;
	char	*c_Qty				= NULL;
	int flagRevRule				= 0;
	int flagClosureRule			= 0;
	char * ItemName;

	printf("\nInside Get_Part_BOM_Lvl_Impl ....\n");

	if(partObj == NULLTAG)
	{
		printf("\n partObj is NULLTAG\n");fflush(stdout);
		return ifail;
	}

	ITK_CALL(AOM_ask_value_string(partObj,"item_id",&prtNumber));
	printf("\n Part number For Explode===>%s\n",prtNumber);fflush(stdout);

//	printf("\nBefore Size of StructParentCnt==>%d\n",*StructParentCnt);fflush(stdout);
	printf("\nBefore reqLevel==>%d\n",reqLevel);fflush(stdout);
	printf("\nBefore level==>%d\n",level);fflush(stdout);
	printf("\nclosureRuleName==>%s\n",closureRuleName);fflush(stdout);
	printf("\nrevRuleName==>%s\n",revRuleName);fflush(stdout);
	printf("\nViewBVR==>%s\n",ViewBVR);fflush(stdout);

	 //First insert Parent Item,First Time StructParentCnt value is -1
	*StructParentCnt	=	*StructParentCnt+1;
	BomChldStrutImpl[*StructParentCnt].parent_objs 		   = partObj;
	tc_strcpy(BomChldStrutImpl[*StructParentCnt].chldPartItmName,prtNumber);


	ITK_CALL(CFM_find(revRuleName, &revRule));
	if (revRule != NULLTAG)
	{
		printf("\nFind revRule\n");fflush(stdout);
		flagRevRule=1;
	}

	scope=PIE_TEAMCENTER;

	ITK_CALL(PIE_find_closure_rules2(closureRuleName,scope,&n_closure_tags,&closure_tags));
	printf("\n n_closure_tags:%d ..............",n_closure_tags);fflush(stdout);
	if(n_closure_tags==1)
	{
		closure_tag=closure_tags[0];
		flagClosureRule=1;
	}
	printf("\n flagClosureRule=>[%d] ",flagClosureRule);fflush(stdout);
	printf("\n flagRevRule=>[%d] ",flagRevRule);fflush(stdout);

	if((flagClosureRule==1) && (flagRevRule==1))
	{
		Multi_Get_Part_BOM_Lvl_Impl(partObj,reqLevel,level,closure_tag,revRule,ViewBVR,returnPartsInVc,BomChldStrutImpl,StructParentCnt);
	}
	else
	{
		printf("\n******Revision or Closure Rule Not Found******\n");fflush(stdout);
	}
	//Printing SetOfObject
	printf("\nAfter Size of StructParentCnt==>%d\n",*StructParentCnt+1);fflush(stdout);

	for (j=0;j<=*StructParentCnt;j++ )
	{
		objParent		= NULLTAG;
		objParent_bvr	= NULLTAG;
		c_Qty			= NULL;
		Parent_lvl       =0;

		printf("\nPrint Item ID==>%d\n",j);fflush(stdout);

		objParent=BomChldStrutImpl[j].parent_objs;
		//objParent_bvr=BomChldStrutImpl[j].parent_objs_bvr;
		//Parent_lvl=BomChldStrutImpl[j].parent_objs_lvl;

		AOM_ask_value_string(objParent,"item_id",&ItemName);
		printf("\n ItemName==>%s\n",ItemName);fflush(stdout);

		//printf("\nParent_lvl==>%d\n",Parent_lvl);fflush(stdout);

		//AOM_ask_value_string(objParent_bvr,"bl_quantity",&c_Qty);
		//printf("\nQty==>%s\n",c_Qty);fflush(stdout);

	}

	return ifail;
}

extern int ITK_user_main (int argc, char ** argv )
{

	int ifail = ITK_ok;

	int status=0;

    char*  Inpt_item_id		= NULL;
    char*  Inpt_item_rev	= NULL;


	int n_tags_found=0;
	int ii=0;


    const char* u = NULL;
    const char* p = NULL;
    const char* g = NULL;

    char* GmDmlTaskName = NULL;
    char* inputVCNo     = NULL;
	char *inputVCNoRev  = NULL;
	char* userName  = NULL;
	char *userRole  = NULL;

    const char* argstring3 = NULL;
    const char* argstring4 = NULL;
	char PlantName[40];
	char GmDmlTaskNameDup[20];
	char  getLCSSTDWrkg[40];
	char  getLCSSTDRlzd[40];

	char *GmTaskName         = NULL;
	char *ClosureDatetsk	 = NULL;

	tag_t		*t_item				= NULLTAG;
	tag_t		partrev_tag			= NULLTAG;

	int         n_entries=2;
	int     	n_found=0;
    tag_t       query 	      = NULLTAG;
	tag_t       PersonFindquery		=NULLTAG;
	tag_t 		UsrFindquery  = NULLTAG;
	tag_t	 	GmTaskObj     = NULLTAG;
	tag_t       *results=NULL;

	tag_t     *usrObjSet		= NULLTAG;
	tag_t     *PersnObjSet		= NULLTAG;
	tag_t     usrObjTag    		= NULLTAG;
	char 	  *usrEmailAddr 	= NULLTAG;
	char      *usrLocation  	= NULLTAG;

	int personFound=0;
	WSO_search_criteria_t  personQryCriteria;
	char  *personName			  = NULLTAG;
	tag_t  *list_of_person_tags   = NULLTAG;

	GmDmlTaskName=(char *) MEM_alloc(200 * sizeof(char *));
	userName=(char *) MEM_alloc(200 * sizeof(char *));
	userRole=(char *) MEM_alloc(200 * sizeof(char *));
	inputVCNo=(char *) MEM_alloc(200 * sizeof(char *));
	inputVCNoRev=(char *) MEM_alloc(200 * sizeof(char *));

	struct	BomChldStrut_Impl	VcAllChildPartSet[5000];
	struct  structre_PartTagSet PartObjectSet[5000];
	struct  struct_PartPRTagSet PRpartObjectSet[8000];
	struct  struct_PartSetSkp   UnqSetSkpPart[5000];

	tag_t		envelope				= NULLTAG;
	tag_t		*recievers				= NULLTAG;
	int countenvelop=0;
	char env_subject[300] = {'\0'};
	char env_body[90000]= {'\0'};


	//Counter Destination For Sets To -1 As Empty
	int CountPartObjectSet = -1;
	int countPrPartSet     = -1;
	int partSkpCounter	   =-1 ;
	//-1 for empty Set
	int totalChldPartCount=-1;

    u = ITK_ask_cli_argument( "-u=");
	p = ITK_ask_cli_argument( "-p=");
	g = ITK_ask_cli_argument( "-g=");

	tc_strcpy(GmDmlTaskName,ITK_ask_cli_argument("-i="));
	tc_strcpy(inputVCNo,ITK_ask_cli_argument("-part="));
	tc_strcpy(inputVCNoRev,ITK_ask_cli_argument("-rev="));
	tc_strcpy(userName,ITK_ask_cli_argument("-User="));
	tc_strcpy(userRole,ITK_ask_cli_argument("-role="));

	//argstring3 = ITK_ask_cli_argument( "-d=");
	//argstring4 = ITK_ask_cli_argument( "-v=");
	printf("\n ITK Code Execution Started For AMDML Creation for GMDML");


	printf("\n UserID  [%s]", u);fflush(stdout);
	printf("\n Password [%s] ", p);fflush(stdout);
	printf("\n Grp Name [%s] \n",g); fflush(stdout);
	printf("\n GMDML Task Name  [%s]", GmDmlTaskName);fflush(stdout);
	printf("\n Part Number for DML Creation [%s] ", inputVCNo);fflush(stdout);
	printf("\n Part Revision for DML Creation [%s] ", inputVCNoRev);fflush(stdout);

	printf("\n User Name for AMDML Creation [%s] \n",userName); fflush(stdout);
	printf("\n User Role for AMDML Creation [%s] \n",userRole); fflush(stdout);

   char *AmDmlLogFle= (char *) malloc(200*sizeof(char));
   FILE *fpAmDmlCreate=NULL;
   FILE *fpdmlPendinginUsrList=NULL;					//Deepti TZ1.52
   char *dmlPendinginUsrList= (char *) malloc(200*sizeof(char));	//Deepti TZ1.52
   char *fullPath= (char *) malloc(200*sizeof(char));	//Deepti TZ1.52
   char *dmlPendinginUsrListName= (char *) malloc(200*sizeof(char));	//Deepti TZ1.52
   tag_t datasettype_tag= NULLTAG;
	 tag_t default_tool_tag;
	 tag_t relation_type;
	 tag_t tDataset;
	 int count_dataSet= 0;
	 char **list;
	 tag_t imannewFileTag = NULLTAG;
	 tag_t Fndrelation = NULLTAG;
	 tag_t  relation = NULLTAG;		//Deepti TZ1.52
	 IMF_file_t filedescriptor;
	 tag_t   qryTagCntrl1     = NULLTAG;
	 int     n_entryCntrl1    = 2;
	 int control_number_found1=0;
	 char    *qry_entryCntrl1[2]  = {"SYSCD","Delete Indicator"};	
	char	**qry_valuesCntrl1= (char **) MEM_alloc(5 * sizeof(char *));
	 tag_t *list_of_WSO_cntrl_tags1=NULLTAG;
	 char *filrPathServer=NULL;



   int ch;
   struct tm *timeptr;
	char pAccessDate[20];
	time_t	temp;

	printf("\n getCurrentDateTime calling..........\n");
	temp = time(NULL);
	tc_strcpy(pAccessDate,"");
	timeptr = (struct tm *)localtime(&temp);
	ch = strftime(pAccessDate,sizeof(pAccessDate)-1,"%d_%b_%Y_%H_%M",timeptr);

	printf("Current Time %s",pAccessDate);

    tc_strcpy(AmDmlLogFle,"/tmp/AmDmlCrtlog_");
	tc_strcat(AmDmlLogFle,GmDmlTaskName);
	tc_strcat(AmDmlLogFle,"_");
    tc_strcat(AmDmlLogFle,inputVCNo);
	tc_strcat(AmDmlLogFle,"_");
	tc_strcat(AmDmlLogFle,pAccessDate);

	tc_strcat(AmDmlLogFle,".txt");

	     //Send Mail to userNam
		//status =ITK_init_module(u,p,g);
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    ITK_CALL(ITK_set_journalling( TRUE ));
    ITK_CALL(ITK_auto_login( ));
//		if (ITK_init_module(u,p,g)== ITK_ok )
//		{
//
//			printf("\n Login Successfully on UA "); fflush(stdout);
//		}
//		else{
//
//			printf("\n Unable to Login on TCUA please Provide correct User id and Password");fflush(stdout);
//
//			return ifail;
//
//		}

	//return ifail;//oto EXIT;

		//Qry User Get UserLocation, EMail Addres

	fpAmDmlCreate = fopen(AmDmlLogFle, "w");

	if(fpAmDmlCreate==NULL)
	{

		printf("\n Unable to Create Log File on Server %s ",AmDmlLogFle); fflush(stdout);

		return ifail;
	}

    if(tc_strstr(userRole,"DBA")!=NULL)
	{
	   printf("\n Bypass Allowed to DBA Role "); fflush(stdout);
	}

    else if(tc_strstr(userRole,"_STD")==NULL)
	{

		fprintf(fpAmDmlCreate,"\n User Role Not Set As STD Client Code return As Functionality provided to STD User Only inputeRole[%s]",userRole); fflush(stdout);
		  //fprintf(,"");
		return ifail;
	 }

	 char *Analyst_email_addr  = NULL;
	 tag_t Analyst_user_tag  = NULLTAG;
	 tag_t Analyst_person_tag  = NULLTAG;
	tag_t tc_group	=	NULLTAG;//Added by Hemal
	 ITK_CALL(SA_find_user (userName, &Analyst_user_tag));
	 ITK_CALL(SA_find_user ("STDCAR", &tc_group));
	 ITK_CALL(AOM_ask_value_tag(Analyst_user_tag,"person",&Analyst_person_tag));

	 ITK_CALL(SA_ask_person_email_address(Analyst_person_tag,&Analyst_email_addr));
	 printf("\n Person EMAIL Address =%s",Analyst_email_addr);fflush(stdout);

		//getPlantDetailsAttr(roleName,PlantCS,PlantIA,PlantStore,UserAgency);
		//Qry inpute Task by Qry Builder

	if(GmDmlTaskName!=NULL)
	{
	   tc_strcpy(GmDmlTaskNameDup,GmDmlTaskName);
	}

	ITK_CALL(QRY_find("Item Revision...", &query));

	n_entries=2;
	n_found=0;
	results=NULL;

	char
       *qry_entries[2] = {"Item ID", "Type"},
       *qry_values[2] =  {GmDmlTaskNameDup, "APL Task Revision"};
	printf(" Print entries and values: %s\n",qry_values[0]);fflush(stdout);
	printf(" Print entries and values: %s\n",qry_values[1]);fflush(stdout);

	printf("\n Execute Query...\n");fflush(stdout);
	QRY_execute(query, n_entries, qry_entries, qry_values, &n_found, &results);

	if (n_found == 1)
	{
		for (ii = 0; ii < n_found; ii++)
		{
			GmTaskObj = results[ii];
			AOM_ask_value_string(results[ii],"item_id",&GmTaskName);
			printf("\n\n item_id-->[%s]",GmTaskName);fflush(stdout);
		}
	}
	else
	{
  		printf("\n NO GMDML Task Found In DB Please Provide Correct Task Name");  fflush(stdout);
		fprintf(fpAmDmlCreate,"\n %s GMDML Task Found In DB Please Provide Correct Task Name ",GmDmlTaskNameDup);

		return ifail;
	}

        //Get Plant based on DML

	GetPlantForDMLORTask(GmTaskName,PlantName);

	printf("\n Plant Name based on GM DML TASK  is :%s",PlantName);

	fprintf(fpAmDmlCreate,"\n Plant Name based on GM DML TASK  is :%s ",PlantName);
	     // Get Input Task Type and expand to reference item.

	tag_t tsk_part_rel_type    = NULLTAG;
	tag_t *PartRevisionList	 = NULLTAG;
	tag_t  PartRevTag          = NULLTAG;
	tag_t  InPartVCExpTag      = NULLTAG;
	char *partItemName		 = NULL;
	char *partType			 = NULL;
	char *partColorInd		 = NULL;
	int partCount;
	int count=0;
	int static flgPartFound=0;

	if(GmTaskObj!=NULLTAG)
		   {
				ITK_CALL(GRM_find_relation_type("CMReferences", &tsk_part_rel_type));

				ITK_CALL(GRM_list_secondary_objects_only(GmTaskObj,tsk_part_rel_type,&count,&PartRevisionList));
				flgPartFound=1;
//				Commented by Hemal
//				for (partCount=0;partCount<count ;partCount++ )
//					{
//						PartRevTag	=	NULLTAG;
//						PartRevTag = PartRevisionList[partCount];
//						if (PartRevTag==NULLTAG)
//						{
//							printf("\n Count %d NULLTAG found, so continue");fflush(stdout);
//							continue;
//						}
//						//ITK_CALL(AOM_ask_value_string(PartRevTag,"item_id",&partItemName));
//						printf("\npartcount:%d",partCount);fflush(stdout);
//						char	*object_type	=	NULL;
//						ITK_CALL(AOM_ask_value_string(PartRevTag,"object_type",&object_type));
//						printf("111 object_type: %s",object_type);
//						ITK_CALL(AOM_UIF_ask_value(PartRevTag,"item_id",&partItemName));
//						printf("\n Reference Part Attached to  GMTask is  :%s",partItemName);fflush(stdout);
//
//						if(strcmp(partItemName,inputVCNo)==0)
//						{
//
//							// Copy the Part To Expand upto 99 level
//							//InPartVCExpTag=PartRevTag;
//							printf("\n Input Part Found in Given GMDML Task :%s  ",inputVCNo); fflush(stdout);
//
//							AOM_ask_value_string(PartRevTag,"t5_PartType",&partType);
//							printf("\n Part Type to Proceed is :%s",partType); fflush(stdout);
//							AOM_ask_value_string(PartRevTag,"t5_ColourInd",&partColorInd);
//							printf("\n Color Indicator of Part :%s is :%s",partItemName,partColorInd); fflush(stdout);
//
//							//Set Flag for Part found in the DML
//							flgPartFound=1;
//							break;
//						}
//					}
					flgPartFound=1;
		   }

		  if(flgPartFound!=1)
	      {

			 printf("\n Inpute Part Not Found In Provided GMDML AMDML Creation Program Terminate");

			 fprintf(fpAmDmlCreate,"\n  Inpute Part Not Found In Provided GMDML AMDML Creation Program Terminate :%s ",inputVCNo);

		     return ifail;

		  }

		  // Code to rolls up to APL GMDML from Task
			tag_t tsk_dml_rel_type = NULLTAG;
			tag_t *DMLRevision     = NULLTAG;
			tag_t  AplGmDmlRevTag       = NULLTAG;

			char *dmlReleaseType=NULL;
			char *DML_no= NULL;
			char *dmlprojCode= NULL;
			char *dmlDesignGrp= NULL;

			count=0;

			ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type));

			if (tsk_dml_rel_type!=NULLTAG || GmTaskObj!=NULLTAG)
			{
				ITK_CALL(GRM_list_primary_objects_only(GmTaskObj,tsk_dml_rel_type,&count,&DMLRevision));
				printf("\n\t\t APL DML Revision from Task : %d",count); fflush(stdout);
				if(count>0)
				{
					AplGmDmlRevTag = DMLRevision[0];

					ITK_CALL(AOM_ask_value_string(AplGmDmlRevTag,"t5_rlstype",&dmlReleaseType));
					printf("\n dmlReleaseType is :%s",dmlReleaseType);fflush(stdout);

					ITK_CALL(AOM_ask_value_string(AplGmDmlRevTag,"current_id",&DML_no));
					printf("\n DML_no is :%s",DML_no);fflush(stdout);

					ITK_CALL(AOM_ask_value_string(AplGmDmlRevTag,"t5_cprojectcode",&dmlprojCode));
					ITK_CALL(AOM_ask_value_string_at(AplGmDmlRevTag,"t5_crdesigngroup",0,&dmlDesignGrp));

					printf("\n dmlprojCode is :%s and Design Grp is %s",dmlprojCode,dmlDesignGrp);fflush(stdout);

				}
				else{

					printf("\n Plant APL DML NOT Found from GM TASK ");fflush(stdout);

				    fprintf(fpAmDmlCreate,"\n  Plant APL DML NOT Found from GM TASK :%s ",inputVCNo);

					return ifail;
				}
			}
		 //Qry Inpute Revision from DB

		int n_revs=0;
		n_tags_found=0;
		tag_t *rev_tags=NULLTAG;
		tag_t *iteminptVCTags= NULLTAG;

		const char *attrs[2] = {"item_id","object_type"}, *values[2]= {inputVCNo,"Design Revision"};

		//values[0] = inputVCNo;
		//values[1] = "Design Revision";
		ITK_CALL(ITEM_find_items_by_key_attributes(1, attrs, values, &n_tags_found, &iteminptVCTags));
		printf("\n Item Found by Key Attributes [%d]", n_tags_found);fflush(stdout);

		if(n_tags_found>0)
		{
			ITEM_find_revisions(iteminptVCTags[0],inputVCNoRev,&n_revs,&rev_tags);

				if(n_revs>0)
				{

					InPartVCExpTag=rev_tags[0];
					printf("\n Revision Found IN DB Part: %s Revision: %s",inputVCNo,inputVCNoRev); fflush(stdout);
				}
				else
				{
					printf("\n Revision not Found IN DB Part: %s Revision: %s",inputVCNo,inputVCNoRev); fflush(stdout);
					return ifail;
				}
		}
		else
		{
			printf("\n item tag not Found in DB %s",inputVCNo); fflush(stdout);
			return ifail;
		}

		 //Inpute Part Expansion Logic

		 int reqLvl=99;
		 tag_t *retsetOfPartinVc;
		 char *ClosureRName			=NULL;
		 char *RevisionRName		=NULL;
		 char *ViewBVR				=NULL;
		 // Start Code to Explode BOM -2 TESING FOR BOM-EXPLODE
		 ClosureRName=(char *) MEM_alloc(200 * sizeof(char *));
		 RevisionRName=(char *) MEM_alloc(200 * sizeof(char *));
		 ViewBVR=(char *) MEM_alloc(100 * sizeof(char *));

          //NeedToCheck
		 tc_strcpy(ClosureRName,"BOMViewClosureRuleERC");
		 //tc_strcpy(RevisionRName,"ERC release and above");//APLC release and above
		 tc_strcpy(RevisionRName,"APLC Release and above");//APLC release and above
		 tc_strcpy(ViewBVR,"View");

		ITK_CALL(AOM_ask_value_string(InPartVCExpTag,"item_id",&partItemName));
		printf("\npartcount:%d",partCount);fflush(stdout);
		 char	*object_type	=	NULL;
		ITK_CALL(AOM_ask_value_string(InPartVCExpTag,"object_type",&object_type));
		printf("111 object_type: %s",object_type);
		AOM_ask_value_string(InPartVCExpTag,"t5_PartType",&partType);
		printf("\n1111 Part type : %s",partType);fflush(stdout);

		 //NeedToCheck
		 if((strcmp(partType,"A")==0 || strcmp(partType,"R")==0 || strcmp(partType,"C")==0 || strcmp(partType,"M")==0 || strcmp(partType,"G")==0) && InPartVCExpTag!=NULLTAG)
		 {
			 Get_Part_BOM_Lvl_Impl(InPartVCExpTag,reqLvl,ClosureRName,RevisionRName,ViewBVR,retsetOfPartinVc,VcAllChildPartSet,&totalChldPartCount);
			 //1+ added as Set Started from '0'
			 totalChldPartCount=totalChldPartCount+1;
		 }


	 printf("\n **Total Child Part Found in VC is:%d \n",totalChldPartCount);
	 fprintf(fpAmDmlCreate,"\n  Total Child Part Found in VC is :%s  total Count %d",inputVCNo,totalChldPartCount);


	 //Code Checking All previous of Child Parts

	 char  *PartCreationDate			= NULL;
	 char  *partItemRevision		    = NULL;
	 int  *partItemRevSeq  			= NULL;
	 char  *epaPlantName				= NULL;
	 char  *epaLifeCycle				= NULL;
	 char  *TaskItemName				= NULL;
	 char  *partRevisionID				= NULL;
	 char  *partDRStatuss				= NULL;
	 char  *PartRevName					= NULL;
	 char  *taskClosureTime				= NULL;
	 char  *taskLifeCycle				= NULL;
	 char  *DmlEcnTypDup				= NULL;
	 char  *attchDmlName				= NULL;
	 char  *DmlClosureDate				= NULL;
	 char  *DmlLCSDup					= NULL;
	 char  *PartSelctdItemRevName		= NULL;
	 char  *PartSelctdItemRevision		= NULL;

	 char  RlzStatusName[40];
	 char  getTaskPlantName[40];

	 tag_t *EPATaskRevList			    = NULLTAG;
	 tag_t *EPARevList					= NULLTAG;
	 tag_t *dmlRevTagList				= NULLTAG;

	 tag_t	dmlRevTag					= NULLTAG;
	 tag_t epatask_part_rel_type	    = NULLTAG;
	 tag_t epaRevTag					= NULLTAG;
	 tag_t taskRevTag					= NULLTAG;
	 tag_t revstatus	   			    = NULLTAG;
	 char  type_class[TCTYPE_name_size_c+1];
	 tag_t	itemTypeTag_class			= NULLTAG;
	 tag_t prevBrkTsakObj				= NULLTAG;
	 tag_t prevRevPartTag				= NULLTAG;
	 tag_t t_previousRevision			= NULLTAG;
	 tag_t queryPart					= NULLTAG;

	 tag_t epatask_Rev_rel_type			= NULLTAG;
	 tag_t task_dml_rel_type			= NULLTAG;

	 tag_t *status_list					= NULL;

	 n_entries=2;
	 //index Declaration
	 int totalTaskcount=0,taskCnt=0,epaCnt=0,j=0,revCnt=0,setIndex=0,tskIndx=0,totalDmlCnt=0,NStatus=0;

	 //Flag Declaration
	 int IsEpaflg=0,PrevFlag=0,flagPartTagFound=0,flagClosure=0,flgtaskwrk=0;


	 tag_t 	*TaskRevList				= NULL;
	 tag_t   task_part_rel_type			= NULLTAG;
	 int 	totalAttchtskCnt=0;
	 int    num_to_sort =1;
	 char *keys[1] = {"creation_date"};
	 int  	 orders[1]  ={2};

	 ITK_CALL(QRY_find("Item Revision...", &queryPart));

	 //char *qry_part_entries[3] = {"Item ID", "Type","Release Status"};

    //char *qry_part_values[3] = {"*","Design Revision","T5_LcsErcRlzd"};

	 char *qry_part_entries[2] = {"Item ID","Release Status"};

     //char *qry_part_values[2] = {"*","T5_LcsErcRlzd"};//APLC Released
     char *qry_part_values[2] = {"*","T5_LcsAplRlzd"};//Hemal changed ERC Released to T5_LcsAplRlzd

	 if(totalChldPartCount>0)
	 {

		 for(j=0;j<totalChldPartCount;j++)
		 {

			  tag_t PartTag=NULLTAG;
			  n_found=0;
			  results=NULL;

			  flagClosure=0;

			  PartTag=VcAllChildPartSet[j].parent_objs;

			  ITK_CALL(AOM_ask_value_string(PartTag,"item_id",&partItemName));

			  qry_part_values[0]= partItemName;

  			  QRY_execute_with_sort(queryPart, n_entries, qry_part_entries, qry_part_values, num_to_sort, keys, orders, &n_found, &results);

			  printf("\n Total Revision found in DB above ERC Released  %s: total Revision :%d",partItemName,n_found);

			   // Checking For Any EPA Present on Latest Revision if so then select Previous Revision to Transfer in SAP/attach to AMDML

				   for(revCnt=0;revCnt<n_found;revCnt++)
				    {

					  tag_t PartrevisionTag;

					  totalTaskcount=0;
					  IsEpaflg=0;

					  PartrevisionTag=results[revCnt];

					  ITK_CALL(AOM_ask_value_string(PartrevisionTag,"item_revision_id",&partItemRevision));

					  ITK_CALL(AOM_ask_value_int(PartrevisionTag,"sequence_id",&partItemRevSeq));

					  printf("\n Item Rev :%s,%s,%d",partItemName,partItemRevision,partItemRevSeq);

					  ITK_CALL(GRM_find_relation_type("CMHasSolutionItem", &epatask_part_rel_type));

					  ITK_CALL(GRM_list_primary_objects_only(PartrevisionTag,epatask_part_rel_type,&totalTaskcount,&EPATaskRevList));

					  printf("\n task Count :%d",totalTaskcount);
					  if(totalTaskcount>0)
					  {

						for(taskCnt=0;taskCnt<totalTaskcount;taskCnt++)
						{

							taskRevTag=EPATaskRevList[taskCnt];

							ITK_CALL (TCTYPE_ask_object_type(taskRevTag,&itemTypeTag_class));
							ITK_CALL (TCTYPE_ask_name(itemTypeTag_class,type_class));

							printf("\t  type_itemRev ...%s\n", type_class);

							//NeedToCheck
							if(tc_strcmp(type_class,"T5_EPATaskRevision")==0)
							{

								ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation", &epatask_Rev_rel_type));

					            ITK_CALL(GRM_list_primary_objects_only(taskRevTag,epatask_Rev_rel_type,&epaCnt,&EPARevList));

									if(epaCnt>0)
									{

										epaRevTag=EPARevList[0];


										ITK_CALL(AOM_ask_value_string(epaRevTag,"t5_EpaPlantA",&epaPlantName));
										//ITK_CALL(AOM_ask_value_string(epaRevTag,"t5_EpaPlantA",&epaPlantName));

										ifail=WSOM_ask_release_status_list(epaRevTag,&count,&status_list);
										printf("\n Inside MM_ReleaseRevision:: release status found count-->[%d]\n",count);fflush(stdout);
										//Check this status is alreasy present in ItemRevision
										MM_check_status(count,status_list,&revstatus);

										//NeedTocheck
										if(tc_strcmp(epaPlantName,"CAR")==0 && tc_strcmp(epaLifeCycle,"T5_LcsStdRlzd")!=0)
										{
											 printf("\n EPA Found On Latest Revision in Wrking State Pls Check Previous Revision \n",IsEpaflg);fflush(stdout);
											IsEpaflg=1;
											break;

										}

									}

								}

						   }

					    }

					//NeedToCheck Condition For NR Revision Also if NR Part Also in EPA
					  if(IsEpaflg==1)
					  {
						  prevBrkTsakObj=PartrevisionTag;
					  }
					  else{

						  prevBrkTsakObj=PartrevisionTag;
						  break;
					  }

				   }//End Of All Revision Traversing



				  printf("\nIsEpaflg[%d] \n",IsEpaflg);fflush(stdout);


				 // Select latest Revision on which there is NO EPA attached or its Std Relesase for Plant

				 if(prevBrkTsakObj)
				 {

						 PartSelctdItemRevName	= NULL;
						 PartSelctdItemRevision	= NULL;
						 task_part_rel_type     = NULLTAG;
						 ITK_CALL(GRM_find_relation_type("CMHasSolutionItem", &task_part_rel_type));
					     ITK_CALL(GRM_list_primary_objects_only(prevBrkTsakObj,task_part_rel_type,&totalAttchtskCnt,&TaskRevList));

						ITK_CALL(AOM_ask_value_string(prevBrkTsakObj,"item_id",&PartSelctdItemRevName));
						ITK_CALL(AOM_ask_value_string(prevBrkTsakObj,"item_revision_id",&PartSelctdItemRevision));


						printf("\n  Part %s :%s Task Count:%d",PartSelctdItemRevName,PartSelctdItemRevision,totalAttchtskCnt);

					    if(totalAttchtskCnt>0)
						  {
							for(tskIndx=0;tskIndx<totalAttchtskCnt;tskIndx++)
							 {

									taskRevTag=TaskRevList[tskIndx];


									ITK_CALL (TCTYPE_ask_object_type(taskRevTag,&itemTypeTag_class));
									ITK_CALL (TCTYPE_ask_name(itemTypeTag_class,type_class));

									printf("\t  type_itemRev ...%s\n", type_class);

									 if(tc_strcmp(type_class,"T5_APLTaskRevision")!=0)
								     {

										printf("\n SKipp AS Object Type is Not APL TASK  Type OF Class");
										 continue;

									  }

									  ITK_CALL(AOM_ask_value_string(taskRevTag,"item_id",&TaskItemName));

									  GetPlantForDMLORTask(TaskItemName,getTaskPlantName);

									 printf("\n Plant Name based on attached TASK Type is :%s",getTaskPlantName);

                                    if(tc_strcmp(getTaskPlantName,PlantName)==0)
									{
										GetPlantWiseRelStat(TaskItemName,getLCSSTDWrkg,getLCSSTDRlzd);

										if(strstr(TaskItemName,"PP")==0 || strstr(TaskItemName,"PM")==0  || strstr(TaskItemName,"JP")==0  || strstr(TaskItemName,"JM")==0  || strstr(TaskItemName,"LP")==0  || strstr(TaskItemName,"LM")==0)
										{

											ClosureDatetsk=NULL;
											AOM_UIF_ask_value(taskRevTag,"date_released",&ClosureDatetsk);

											//commented as Date released is not required
											//if(ClosureDatetsk!=NULL)
											//{
											//	flagClosure =1;
											//	break;
											//}


											//check Life Cycle of Task

											//ITK_CALL(AOM_ask_value_string(taskRevTag,"item_id",&taskClosureTime));

											printf("\n Task ClosureTime :%s",ClosureDatetsk); fflush(stdout);

											ifail=WSOM_ask_release_status_list(taskRevTag,&count,&status_list);

											flgtaskwrk=0;

											for(NStatus=0;NStatus<count;NStatus++)
											{
												ifail=CR_ask_release_status_type(status_list[NStatus],RlzStatusName);
												printf("\n MM_check_status:: RlzStatusName==>[%s]",RlzStatusName);fflush(stdout);

											    //task in wrking for this plant so break from loop //NeedToCheck
												if(tc_strcmp(RlzStatusName,getLCSSTDWrkg)==0 || tc_strcmp(RlzStatusName,getLCSSTDRlzd)==0 )
												{

													flgtaskwrk=1;
													break;


												}
											}


											if(flgtaskwrk==1)
											{
														     // Add to Set  UnqSetSkpPart
												if(FoundObjectInList(UnqSetSkpPart,partSkpCounter,PartSelctdItemRevName,PartSelctdItemRevision)==0)
												{
																//AddParts In Set

													partSkpCounter++;
													UnqSetSkpPart[partSkpCounter].PartTag=prevBrkTsakObj;
													tc_strcpy(UnqSetSkpPart[partSkpCounter].PartName,PartSelctdItemRevName);
													tc_strcpy(UnqSetSkpPart[partSkpCounter].partRevision,PartSelctdItemRevision);

												  }
												 else{

													printf("\n Part Already present in UnqSetSkpPart Set No Need to Add"); fflush(stdout);

													}
													    flagClosure =1;
														break;

											}
									}//Closing For PP/PM type DML

									else if(strstr(TaskItemName,"AM")==0)
									{

										printf("Part Number :");fflush(stdout);

										ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation", &task_dml_rel_type));
										ITK_CALL(GRM_list_primary_objects_only(taskRevTag,task_dml_rel_type,&totalDmlCnt,&dmlRevTagList));

										if(totalDmlCnt>0)
										{

											 dmlRevTag=dmlRevTagList[0];

											 ITK_CALL (TCTYPE_ask_object_type(dmlRevTag,&itemTypeTag_class));
											 ITK_CALL (TCTYPE_ask_name(itemTypeTag_class,type_class));
											 printf("\t  DML Revision CLass ...%s\n", type_class);



														 // ITK_CALL(AOM_ask_value_string(taskRevTag,"DmlClosure",&DmlClosureDate));
														 ClosureDatetsk=NULL;
														 AOM_UIF_ask_value(taskRevTag,"date_released",&ClosureDatetsk);
														 //printf("\n DML ECN Type :%s",DmlClosureDate);


														 ifail=WSOM_ask_release_status_list(taskRevTag,&count,&status_list);
														 printf("\n Inside MM_ReleaseRevision:: release status found count-->[%d]\n",count);fflush(stdout);
														 //Check this status is alreasy present in ItemRevision
														 MM_check_status(count,status_list,&revstatus);


														if(tc_strcmp(type_class,"T5_APLDMLRevision")!=0)
														{
															 attchDmlName=NULL;

															 ITK_CALL(AOM_ask_value_string(taskRevTag,"t5_EcnType",&DmlEcnTypDup));
															 ITK_CALL(AOM_ask_value_string(taskRevTag,"item_id",&attchDmlName));
														     printf("\n DML %s ECN Type :%s",attchDmlName,DmlEcnTypDup);

															if((tc_strcmp(DmlEcnTypDup,"APLSTR") == 0) || (tc_strcmp(DmlEcnTypDup,"APLMBOMRES") == 0) || (tc_strcmp(DmlEcnTypDup,"CARRYOVER") == 0) || (tc_strcmp(DmlEcnTypDup,"AMBSTR") == 0)||(tc_strcmp(DmlEcnTypDup,"TOODMLR") == 0) || (tc_strcmp(DmlEcnTypDup,"IPBT") == 0) || (tc_strcmp(DmlEcnTypDup,"FrameGrp") == 0) || (tc_strcmp(DmlEcnTypDup,"APLRESPREL") == 0))
															{


																printf("\n Part attached with  DML %s of Type[%s] 1111Checking its  Life Cycle Status",attchDmlName,DmlEcnTypDup);fflush(stdout);
																//Check DML Life Cycle Status
																ifail=WSOM_ask_release_status_list(taskRevTag,&count,&status_list);
													            flgtaskwrk=0;

																for(NStatus=0;NStatus<count;NStatus++)
																	{
																		ifail=CR_ask_release_status_type(status_list[NStatus],RlzStatusName);
																		printf("\n MM_check_status:: RlzStatusName==>[%s]",RlzStatusName);fflush(stdout);

																		//Already Attached with AMDML so skip it//NeedToCheckCondition
																		if(tc_strcmp(RlzStatusName,getLCSSTDRlzd)==0 ||  tc_strcmp(RlzStatusName,getLCSSTDWrkg)==0)
																		{

																		    printf("\n Part attched to  DML %s of Type[%s] Life Cycle Found Released ",attchDmlName,DmlEcnTypDup); fflush(stdout);
																			flgtaskwrk=1;
																			break;

																		}
																	}


																if(flgtaskwrk==1 && ClosureDatetsk!=NULL)
																{
																	flagClosure =1;
																	break;

																}   //NeedtoCheckConditionfor AMDMLtype
																else if((tc_strcmp(DmlEcnTypDup,"TOODMLR") == 0))
																{
																	if(ClosureDatetsk!=NULL)
																	{
																		flagClosure =1;
																		break;
																	}
																	else
																	{

																		//printf("\n\n StrNotAtt PartNum is [%s]\n",PrtNumVC1Dup); fflush(stdout);
																		printf("\n Skipped Part %s :%s  attched to Task %s",PartSelctdItemRevName,PartSelctdItemRevision,attchDmlName);

																		if(FoundObjectInList(UnqSetSkpPart,partSkpCounter,PartSelctdItemRevName,PartSelctdItemRevision)==0)
																		  {
																			//AddParts In Set

																			partSkpCounter++;
																			UnqSetSkpPart[partSkpCounter].PartTag=prevBrkTsakObj;
																			tc_strcpy(UnqSetSkpPart[partSkpCounter].PartName,PartSelctdItemRevName);
																		    tc_strcpy(UnqSetSkpPart[partSkpCounter].partRevision,PartSelctdItemRevision);

																		  }
																		 else{

																			 printf("\n Part Already present in UnqSetSkpPart Set No Need to Add"); fflush(stdout);

																		   }

																	    flagClosure =1;
																		break;
																	}

																}//NeedToCheckCondition
																else if(tc_strcmp(DmlLCSDup,"LcsSTDAthrig")==0 || tc_strcmp(DmlLCSDup,"T5_LcsSTDWrkg")==0)
																{

																			//printf("\n\n StrNotAtt PartNum is [%s]\n",PrtNumVC1Dup); fflush(stdout);
																			//low_set_add_str_unique (StrNotAtt,PrtNumVC1Dup);//TZ 3.64

																			if(FoundObjectInList(UnqSetSkpPart,partSkpCounter,PartSelctdItemRevName,PartSelctdItemRevision)==0)
																			{
																				//AddParts In Set

																				partSkpCounter++;
																				UnqSetSkpPart[partSkpCounter].PartTag=prevBrkTsakObj;
																				tc_strcpy(UnqSetSkpPart[partSkpCounter].PartName,PartSelctdItemRevName);
																				tc_strcpy(UnqSetSkpPart[partSkpCounter].partRevision,PartSelctdItemRevision);


																			}
																			else{

																			 printf("\n Part Already present  in UnqSetSkpPart Set No Need to Add"); fflush(stdout);

																			}

																	flagClosure =1;
																	break;

																 }
																else
																{
																	printf("\n\n No DML/Task found which STDSI relz from same plant\n"); fflush(stdout);

																}

													        }

												        }//End of IF T5_APLDMLRevision

											        }

										        } //Endof if For DML Type 'AM'

									        }	//Check DML for Same Plant Only

								    }	// end of For loop for checking All task Attached to selected Part

						      } 	//end of if Cond for Checking Task Attahced to Parts



								//Add Part prevBrkTsakObj in PartObjectSet

						if(flagClosure==0)
						  {

									 flagPartTagFound=0;
								     PartRevName=NULL;

									  ITK_CALL(AOM_ask_value_string(prevBrkTsakObj,"item_id",&PartRevName));
									  ITK_CALL(AOM_ask_value_string(prevBrkTsakObj,"item_revision_id",&partRevisionID));

									//Check Duplicacy in Object Set
									for(setIndex=0;setIndex<CountPartObjectSet;setIndex++)
									{

									   if(strcmp(PartObjectSet[setIndex].PrtItemName,PartRevName)==0)
									   {
										   flagPartTagFound=1;
										   break;
									   }
								    }
									//Store Unique Tag Value in Set
									if(flagPartTagFound!=1)
									{
										CountPartObjectSet++;

										PartObjectSet[CountPartObjectSet].PartTag= prevBrkTsakObj;
										tc_strcpy(PartObjectSet[CountPartObjectSet].PrtItemName,PartRevName);

									}


									prevRevPartTag=prevBrkTsakObj;


									//Store All Previous Revision in Set To Attach it in PR Task skipp if latest Revision is NR Revision

									if(tc_strstr(partRevisionID,"NR")==NULL)
									{
										//PrevFlag = 0;
										do{

												 PrevFlag			= 1;
												 t_previousRevision  = NULLTAG;
												 partRevisionID=NULL;

												 //Get Previous Revision

												 tm_fnd_Prev_Official_Revision(PartRevName,prevRevPartTag, &t_previousRevision);

												 if(t_previousRevision!=NULLTAG)
												 {

													 //Store each Previous Rev Tag in SetofObject
													 prevRevPartTag=t_previousRevision;

													 ITK_CALL(AOM_ask_value_string(prevRevPartTag,"item_id",&PartRevName));
													 ITK_CALL(AOM_ask_value_string(prevRevPartTag,"item_revision_id",&partRevisionID));


													//RevOnly= strtok( partRevisionID, ";" );
													printf("\n Part Current Revision :%s",partRevisionID); fflush(stdout);

													if(tc_strstr(partRevisionID,"NR")!=NULL)
													  {
														  //if current revision is NR then no need to go back we need to break the Loop.


														  //Store NR Rev in PR Set
														  flagPartTagFound=0;
														  for(setIndex=0;setIndex<countPrPartSet;setIndex++)
															{

															   if(strcmp(PRpartObjectSet[setIndex].PartName,PartRevName)==0 && strcmp(PRpartObjectSet[setIndex].partRevision,partRevisionID) )
															   {
																   flagPartTagFound=1;
																   break;
															    }
															}
															//Store Unique Tag Value in Set
															if(flagPartTagFound!=1)
															{
																countPrPartSet++;

																PRpartObjectSet[countPrPartSet].PartTag= t_previousRevision;
																tc_strcpy(PRpartObjectSet[countPrPartSet].partRevision,partRevisionID);

															}


														  printf("flag set PrevFlag :%d",PrevFlag);

														  PrevFlag=0;
														  break;

													  }


													  //Check Duplicacy in PR Set struct_PartPRTagSet

													  flagPartTagFound=0;
													  for(setIndex=0;setIndex<countPrPartSet;setIndex++)
														{

														   if(strcmp(PRpartObjectSet[setIndex].PartName,PartRevName)==0 && strcmp(PRpartObjectSet[setIndex].partRevision,partRevisionID) )
														   {
															   flagPartTagFound=1;
															   break;
														   }
														}
														//Store Unique Tag Value in Set
														if(flagPartTagFound!=1)
														{
															countPrPartSet++;

															PRpartObjectSet[countPrPartSet].PartTag= t_previousRevision;
															tc_strcpy(PRpartObjectSet[countPrPartSet].partRevision,partRevisionID);

														}

												 }
												 else{
													 //Terminate when there is no prev Revision
													   PrevFlag=0;
												 }

										   }while(PrevFlag!=0);


									}//End of IF for NR Revision Check

								} //End of 	flagClosure if Cndn

				 }// End of prevBrkTsakObj if Loop

		   } //End of For Loop of Set Found In VC

	   } //end of If any object found in Set



		 tag_t unqPartTag 		= NULLTAG;
		 char *partName		    = NULL;

		 printf("\n *******Part Present in VC************\n ");


		 printf("\n Total Part Present in VC :%d ",CountPartObjectSet+1);

		if(CountPartObjectSet==-1)
		{

			 printf("\n No Part To Create ADML Return to Main .Total Part Present in VC :%d ",CountPartObjectSet+1);
			 goto SEND_MAIL;

		}


		 if(CountPartObjectSet>5000 || countPrPartSet>5000)
	      {


		     fprintf(fpAmDmlCreate,"\n Limit of countPrPartSet/CountPartObjectSet is More Than 5000 part Please varify Parts in Set CountPartObjectSet=%d, countPrPartSet=%d ",CountPartObjectSet,countPrPartSet);

		     goto SEND_MAIL;

		   }

		  fprintf(fpAmDmlCreate,"\n ***********Unique Parts in VC to Attach in AMDML :%s  total Count %d ********",inputVCNo,CountPartObjectSet+1);

		 for(j=0;j<=CountPartObjectSet;j++)
			{

				 unqPartTag 		=NULLTAG;
				 partName		    = NULL;
				 partRevisionID		= NULL;
				 partDRStatuss		=NULL;

				 unqPartTag=PartObjectSet[j].PartTag;

				 ITK_CALL(AOM_ask_value_string(unqPartTag,"item_id",&partName));
				 ITK_CALL(AOM_ask_value_string(unqPartTag,"item_revision_id",&partRevisionID));
				 ITK_CALL(AOM_ask_value_string(unqPartTag,"item_revision_id",&partRevisionID));

				 printf("\n  Part in VC:%s :%s",partName,partRevisionID); fflush(stdout);
				 fprintf(fpAmDmlCreate,"\n  Unique Part in VC:%s:%s",partName,partRevisionID);

			}


		  printf("\n Total Part Present in PrObjectSet :%d ",countPrPartSet+1);

		  fprintf(fpAmDmlCreate,"\n ******Unique Parts in VC to Attach  PR Task in AMDML :%s  total Count %d *********",inputVCNo,countPrPartSet+1);

		  for(j=0;j<=countPrPartSet;j++)
		  {

			 unqPartTag 		=NULLTAG;
			 partName		    = NULL;
			 partRevisionID		= NULL;

			 unqPartTag=PRpartObjectSet[j].PartTag;


			 ITK_CALL(AOM_ask_value_string(unqPartTag,"item_id",&partName));
			 ITK_CALL(AOM_ask_value_string(unqPartTag,"item_revision_id",&partRevisionID));

			 printf("\n Unique PR Part in VC:%s:%s",partName,partRevisionID); fflush(stdout);

			 fprintf(fpAmDmlCreate,"\n  Unique Part attach to PR  in VC:%s:%s",partName,partRevisionID);

		  }



		  fprintf(fpAmDmlCreate,"\n ******Unique Parts in VC UnqSetSkpPart  to Attach in AMDML :%s  total Count %d *********",inputVCNo,partSkpCounter+1);

		  printf("\n Total Part Present in PrObjectSet :%d ",partSkpCounter+1);

		  if(partSkpCounter>0)			//Deepti TZ1.52
		  {
				
				tc_strcpy(dmlPendinginUsrListName,GmDmlTaskName);	
				tc_strcat(dmlPendinginUsrListName,"_");
				tc_strcat(dmlPendinginUsrListName,inputVCNo);
				tc_strcat(dmlPendinginUsrListName,"_");
				tc_strcat(dmlPendinginUsrListName,"DML_PendingWith_STDSI_WL");
				tc_strcat(dmlPendinginUsrListName,"_");
				tc_strcat(dmlPendinginUsrListName,pAccessDate);
				tc_strcat(dmlPendinginUsrListName,".xls");

				tc_strcpy(dmlPendinginUsrList,"/tmp/");	
				tc_strcat(dmlPendinginUsrList,dmlPendinginUsrListName);	

				
				fpdmlPendinginUsrList = fopen(dmlPendinginUsrList, "w");

				if(fpdmlPendinginUsrList==NULL)
				{

					printf("\n Unable to Create Log File on Server %s ",fpdmlPendinginUsrList); fflush(stdout);

					return ifail;
				}
				else
			    {
					for(j=0;j<=partSkpCounter;j++)
					{

						unqPartTag 		=NULLTAG;
						partName		    = NULL;
						partRevisionID		= NULL;

						unqPartTag=UnqSetSkpPart[j].PartTag;


						ITK_CALL(AOM_ask_value_string(unqPartTag,"item_id",&partName));
						ITK_CALL(AOM_ask_value_string(unqPartTag,"item_revision_id",&partRevisionID));

						printf("\n Unique Part in VC To Skipp:%s:%s",partName,partRevisionID); fflush(stdout);

						fprintf(fpAmDmlCreate,"\n  Unique Part  in  UnqSetSkpPart Set :%s:%s",partName,partRevisionID);
						fprintf(fpdmlPendinginUsrList,"\n  :%s:%s",partName,partRevisionID);

					}

				}
		  
		  }
		  
		  
		  



		 // AMDML Creation Logic

		 char  		*gmTaskPrjCode      = NULL;
		 char 		*gmTaskDesignGrp 	= NULL;
		 char       *AmDmlTaskName		= NULL;

		 int 		FlagPRdmlCrt        = 0;
		 int        i=0;
		 char       Synopsis[100];

		 tag_t  	AMDmlObjTag			= NULLTAG;
		 tag_t 		APLTaskTag			= NULLTAG;
		 tag_t		APLTaskPrTag		= NULLTAG;
		 tag_t      *tskRevTagList		= NULLTAG;
		 tag_t		TaskTag				= NULLTAG;
		 tag_t      apl_task_part_relation =NULLTAG;


		 if(countPrPartSet>=0)
		 {
			 FlagPRdmlCrt=1;
		 }


		 if(CountPartObjectSet>=0 && CountPartObjectSet<5000)
		 {

			        printf("\n inside AMDML Creation Logic \n");fflush(stdout);

					tc_strcpy(Synopsis,"AMDML Created as per User Request to remove Dependency of ");
					strcat(Synopsis,GmTaskName);
					strcat(Synopsis," For Assy:");
					strcat(Synopsis,inputVCNo);

					//ITK_CALL(AOM_ask_value_string(GmTaskObj,"t5_cprojectcode",&gmTaskPrjCode));
					//printf("\n  GM TASK projCode is :%s",gmTaskPrjCode);fflush(stdout);


					//AOM_ask_value_string_at(GmTaskObj,"t5_crdesigngroup",0,&gmTaskDesignGrp);
					//ITK_CALL(AOM_ask_value_string(GmTaskObj,"t5_crdesigngroup",&gmTaskDesignGrp));
					//printf("\n  GM TASK projCode is :%s",gmTaskDesignGrp);fflush(stdout);

				AMDMLCreateFuncTOO(dmlprojCode,dmlDesignGrp,NULL,Synopsis,Synopsis,userName,FlagPRdmlCrt,PlantName,&AMDmlObjTag);

				if(AMDmlObjTag==NULLTAG)
			    {

				  fprintf(fpAmDmlCreate,"\n *********System Unable to Create AMDML********");

				}

				if(AMDmlObjTag!=NULLTAG)
				{
						//Set owing user 
						printf("\nSet owing user on DML...");fflush(stdout);
						//Added by Hmal
							ITK_CALL(AOM_lock(AMDmlObjTag));
							ITK_CALL(POM_set_owning_user(AMDmlObjTag,Analyst_user_tag));
							ITK_CALL(POM_set_owning_group(AMDmlObjTag,tc_group));
							ITK_CALL(POM_set_owners(1,&AMDmlObjTag,Analyst_user_tag,tc_group));
							ITK_CALL(AOM_save(AMDmlObjTag));
							ITK_CALL(AOM_unlock(AMDmlObjTag));
							ITK_CALL(AOM_refresh(AMDmlObjTag,TRUE));
							//Added by Hemal
					   int FndDMLTaskrelation=0;
					   totalDmlCnt=0;

					    task_dml_rel_type =NULLTAG;

						printf("\n CREATING REL OF APL TASK and Parts\n");fflush(stdout);

						ITK_CALL(GRM_find_relation_type("T5_DMLTaskRelation",&task_dml_rel_type));
						//ITK_CALL(GRM_find_relation(APLDMLRevTag, APLTaskRevTag, relation_type ,&FndDMLTaskrelation));
						ITK_CALL(GRM_list_secondary_objects_only(AMDmlObjTag,task_dml_rel_type,&totalDmlCnt,&tskRevTagList));

						for(i=0;i<totalDmlCnt;i++)
						{


							AOM_ask_value_string_at(tskRevTagList[i],"item_id",0,&AmDmlTaskName);

							if(tc_strstr(AmDmlTaskName,"_PR")!=NULL)
							{
								//Added by Hemal
								APLTaskPrTag=tskRevTagList[i];
								ITK_CALL(AOM_lock(APLTaskPrTag));
								ITK_CALL(POM_set_owning_user(APLTaskPrTag,Analyst_user_tag));
								ITK_CALL(POM_set_owning_group(APLTaskPrTag,tc_group));
								ITK_CALL(POM_set_owners(1,&APLTaskPrTag,Analyst_user_tag,tc_group));
								ITK_CALL(AOM_save(APLTaskPrTag));
								ITK_CALL(AOM_unlock(APLTaskPrTag));
								ITK_CALL(AOM_refresh(APLTaskPrTag,TRUE));
								//Added by Hemal
								
							}
							else
							{
								APLTaskTag=tskRevTagList[i];
								//Added by Hemal
								ITK_CALL(AOM_lock(APLTaskTag));
								ITK_CALL(POM_set_owning_user(APLTaskTag,Analyst_user_tag));
								ITK_CALL(POM_set_owning_group(APLTaskTag,tc_group));
								ITK_CALL(POM_set_owners(1,&APLTaskTag,Analyst_user_tag,tc_group));
								ITK_CALL(AOM_save(APLTaskTag));
								ITK_CALL(AOM_unlock(APLTaskTag));
								ITK_CALL(AOM_refresh(APLTaskTag,TRUE));
								//Added by Hemal
							}

						}

						if(APLTaskTag!=NULLTAG)
						{
							//ITK_CALL(AOM_lock(APLTaskPrTag));
							//ITK_CALL(POM_set_owning_user(APLTaskPrTag,Analyst_user_tag));
							//ITK_CALL(POM_set_owning_group(APLTaskPrTag,tc_group));
							//ITK_CALL(AOM_save(APLTaskPrTag));
							//ITK_CALL(AOM_unlock(APLTaskPrTag));
							//ITK_CALL(AOM_refresh(APLTaskPrTag,TRUE));

							task_part_rel_type=NULLTAG;

							ITK_CALL(GRM_find_relation_type("CMHasSolutionItem", &task_part_rel_type));


							printf("\n Attaching Latest Revision to AMDML TASK");

							fprintf(fpAmDmlCreate,"\n :%s",PartObjectSet[j].PrtItemName);

                           AOM_ask_value_string_at(APLTaskTag,"item_id",0,&AmDmlTaskName);
                           fprintf(fpAmDmlCreate,"\n **********List Of Parts Unable to attach AMDML Task:%s ************* \n ",AmDmlTaskName);

							for(j=0;j<=CountPartObjectSet;j++)
							{


								apl_task_part_relation=NULLTAG;

								ifail=GRM_create_relation(APLTaskTag, PartObjectSet[j].PartTag, task_part_rel_type,  NULLTAG, &apl_task_part_relation);

								if(ifail!=ITK_ok)
								{
								   fprintf(fpAmDmlCreate,"\n Unable to attach Part in AMDML Task:%s",PartObjectSet[j].PrtItemName);

								}

								ITK_CALL(GRM_save_relation(apl_task_part_relation));

							}


						}


						if(APLTaskPrTag!=NULLTAG)
						{
							task_part_rel_type=NULLTAG;

							ITK_CALL(GRM_find_relation_type("CMHasSolutionItem", &task_part_rel_type));

							AOM_ask_value_string(APLTaskPrTag,"item_id",&AmDmlTaskName);
                            fprintf(fpAmDmlCreate,"\n  ********List Of Parts Unable to attach PR Task of AMDML:%s ********** \n ",AmDmlTaskName);

							for(j=0;j<=countPrPartSet;j++)
							{
								apl_task_part_relation=NULLTAG;

								ifail=GRM_create_relation(APLTaskPrTag, PRpartObjectSet[j].PartTag, task_part_rel_type,  NULLTAG, &apl_task_part_relation);

								if(ifail!=ITK_ok)
								{
								   fprintf(fpAmDmlCreate,"\n Unable to attach Part in AMDML PR  Task:%s",PRpartObjectSet[j].PartName);

								}

								ITK_CALL(GRM_save_relation(apl_task_part_relation));

							}

						}


				 }//AMDML object not NULL


		 }//SetofObject more than 1



	 //Stamping DML & Task Life Cycle

	 char *lifeCylceStmp=NULL;


	 printf("\n Plant Name is %s",PlantName);
	 if(tc_strstr(PlantName,"APLC")!=NULL)//T5_LcsSTDWrkg
	  {
	    lifeCylceStmp="T5_LcsAplRlzd";


	  }

	  else if(tc_strstr(PlantName,"APLP")!=NULL){

	   lifeCylceStmp="T5_LcsAplpRlzd";
	  }



	 if(AMDmlObjTag!=NULLTAG && lifeCylceStmp != NULL)
	 {

       printf("\n Stamping APL Released Life Cycle on AMDML"); fflush(stdout);



      // t5UpdateLifecycleStateAPL(AMDmlObjTag,"T5_LcsAplRlzd");

	 t5UpdateLifecycleStateAPL(AMDmlObjTag,lifeCylceStmp);

	 }
     if(APLTaskTag!=NULLTAG)
	 {
	   printf("\n Stamping APL Released Life Cycle on AMDML TASK"); fflush(stdout);
	   t5UpdateLifecycleStateAPL(APLTaskTag,lifeCylceStmp);

	 }

	 if(APLTaskPrTag!=NULLTAG)
	 {

	   printf("\n Stamping APL Released Life Cycle on AMDML PR TASK"); fflush(stdout);
	   t5UpdateLifecycleStateAPL(APLTaskPrTag,lifeCylceStmp);

	 }

	 if(fpdmlPendinginUsrList!=NULL)
	 {
		 if(APLTaskTag!=NULLTAG)
		 {
			 
				
				if(QRY_find("Control Objects...", &qryTagCntrl1));
				if (qryTagCntrl1)
				{
					printf("\n Control Object Query Found for GM FIle attachemnt \n");fflush(stdout);
					qry_valuesCntrl1[0]="GMFileAtt";
					qry_valuesCntrl1[1]="0";
					
					if(QRY_execute(qryTagCntrl1, n_entryCntrl1, qry_entryCntrl1, qry_valuesCntrl1, &control_number_found1, &list_of_WSO_cntrl_tags1));
				}
				else
				{
					printf("\n Control Object Query not Found for GM DML \n");fflush(stdout);
				}	
				
				printf("\n control_number_found1 is : %d\n",control_number_found1);fflush(stdout);

				if(control_number_found1>0)
				{
					ITK_CALL(AOM_ask_value_string( list_of_WSO_cntrl_tags1[0], "t5_Userinfo1", &filrPathServer));
					printf("\n filrPathServer: %s\n",filrPathServer);fflush(stdout);

					tc_strcpy(fullPath,filrPathServer);
					tc_strcat(fullPath,dmlPendinginUsrListName);
					
					ITK_CALL(AE_find_datasettype ("MSExcel",&datasettype_tag));
					printf("\n Finding Datasettype---");fflush(stdout);
					ITK_CALL(AE_ask_datasettype_def_tool(datasettype_tag,&default_tool_tag));
					printf("\n DataSetTypeDef---");fflush(stdout);
					ITK_CALL(AE_create_dataset_with_id(datasettype_tag, dmlPendinginUsrListName,"DML_PendingWith_STDSI_WL", 0, 0, &tDataset));
					printf("\n Creating dataset---");fflush(stdout);
					ITK_CALL(AE_set_dataset_format(tDataset,"BINARY_REF"));
					printf("\n set dataset format---");fflush(stdout);
					ITK_CALL(AE_set_dataset_tool(tDataset,default_tool_tag));
					ITK_CALL(AE_ask_datasettype_refs(datasettype_tag,&count_dataSet,&list));
					printf("\n set dataset ref---");fflush(stdout);

					ITK_CALL(IMF_import_file (fullPath,NULL,SS_BINARY,&imannewFileTag,&filedescriptor));

					ITK_CALL(IMF_set_original_file_name(imannewFileTag,dmlPendinginUsrListName));
					ITK_CALL(AOM_save(imannewFileTag));
					printf("\nFile saved---");fflush(stdout);
					ITK_CALL(AE_add_dataset_named_ref(tDataset,list[0],SS_BINARY,imannewFileTag));
					printf("\nAE_add_dataset_named_ref-");fflush(stdout);
					ITK_CALL(AOM_save(tDataset));

					ITK_CALL(GRM_find_relation_type("IMAN_reference", &relation_type));
					printf("\n GRM_find_relation_type ");fflush(stdout);
					ITK_CALL(GRM_find_relation( APLTaskTag, tDataset, relation_type ,&Fndrelation));
					printf("\n\t Fndrelation "); fflush(stdout);
					if(Fndrelation)
					{
						printf("\n\t Relation Already Exist with file...\n" );fflush(stdout);
					}
					else
					{
						ITK_CALL(GRM_create_relation(APLTaskTag, tDataset, relation_type, NULLTAG, &relation));
						printf("\n GRM_create_relation ");fflush(stdout);
						ITK_CALL(GRM_save_relation(relation));
						printf("\n GRM_save_relation ");fflush(stdout);
					}
			}
		 
		 }
		 //fclose (fpdmlPendinginUsrList);
	 
	 }

	//DML Submitted in Life Cycle

   if(AMDmlObjTag!=NULLTAG)
	 {
		tag_t template_tag = NULLTAG;
		tag_t test_job_a = NULLTAG;
		int attachment_types = 1;
		printf("\n Finding CM Release Process Template\n");fflush(stdout);

		ITK_CALL(EPM_find_template2("STDSI DML WorkFlow",0,&template_tag));
		//ITK_CALL(EPM_find_template2("STDWrkflTest",0,&template_tag));
		if (template_tag!=NULLTAG)
		{
			printf("\n Submit APL DML in to workflow\n");fflush(stdout);
			//NeedToCheck
			ITK_CALL(EPM_create_process("STDWrkflTest","",template_tag,1, &AMDmlObjTag,&attachment_types,&test_job_a));
			printf("\n Submit APL DML in to workflow process complete\n");fflush(stdout);
		}

		else
		{
			fprintf(fpAmDmlCreate,"\n 'STDSI DML WorkFlow' process Not Found ");
			printf("\n 'STDSI DML WorkFlow' process Not Found \n");fflush(stdout);

		}


	 }



SEND_MAIL:

if(AMDmlObjTag!=NULLTAG)
	 {

		 AOM_ask_value_string(AMDmlObjTag,"item_id",&AmDmlTaskName);

		tc_strcpy(env_subject,"MAIL FROM PLM SYSTEM AMDML Created for TOO DML");
		//tc_strcat(env_subject,"");

		tc_strcpy(env_body,"Hi, \n\n ");
		/*tc_strcat(env_body,AmDmlTaskName);
		tc_strcat(env_body," AMDML create For GMDML ");
		tc_strcat(env_body,GmDmlTaskName);
		tc_strcat(env_body," For Assy: ");
		tc_strcat(env_body,inputVCNo);
		tc_strcat(env_body," \n Regards,\n PLM Team ");*/

		tc_strcat(env_body,"AMDML ");
		tc_strcat(env_body,"\'");
		tc_strcat(env_body,AmDmlTaskName);
		tc_strcat(env_body,"\'");
		tc_strcat(env_body," has been created for Assy ");
		tc_strcat(env_body,"\'");
		tc_strcat(env_body,inputVCNo);
		tc_strcat(env_body,"\'");
		tc_strcat(env_body," during release of GMDML ");
		tc_strcat(env_body,GmDmlTaskName);
		tc_strcat(env_body," \n Regards,\n PLM Team ");




	 }
	 else{


		 tc_strcpy(env_subject,"MAIL FROM PLM SYSTEM FAILED To Create AMDML for TOO DML");
		//tc_strcat(env_subject,"");

		/*tc_strcpy(env_body,"Hi, \n FAILED to create AMDML For GMTASK  ");
		tc_strcat(env_body, GmDmlTaskName );
		tc_strcat(env_body," For Assy: ");
		tc_strcat(env_body,inputVCNo);*/

		tc_strcpy(env_body,"Hi, \n\n ");
		tc_strcat(env_body,"AMDML ");
		tc_strcat(env_body,"creation has been failed for Assy ");
		tc_strcat(env_body,"\'");
		tc_strcat(env_body,inputVCNo);
		tc_strcat(env_body,"\'");
		tc_strcat(env_body," during release of GMDML ");
		tc_strcat(env_body,GmDmlTaskName);

		tc_strcat(env_body," \n Regards,\n PLM Team ");

	 }


	    printf(" Mail Body String :%s",env_body);
	    ITK_CALL(MAIL_create_envelope(env_subject, env_body, &envelope));

		if(envelope != NULLTAG)
		  {

		   ITK_CALL(MAIL_initialize_envelope(envelope, env_subject, env_body));
		   if(Analyst_email_addr!=NULL)
		   {
			   //ITK_CALL(MAIL_add_external_receiver(envelope, MAIL_send_to,Analyst_email_addr));

		   }
		   ITK_CALL(MAIL_add_external_receiver(envelope, MAIL_send_cc,"dhanas.ttl@tatamotors.com"));
		   ITK_CALL(MAIL_add_external_receiver(envelope, MAIL_send_cc,"hemals.ttl@tatamotors.com"));
		   ITK_CALL(MAIL_add_external_receiver(envelope, MAIL_send_cc,"deeptim.ttl@tatamotors.com"));
		   ITK_CALL(MAIL_list_envelope_receivers(envelope,&countenvelop,&recievers));
		   printf("\n countenvelop %d\n",countenvelop);fflush(stdout);
		   ITK_CALL(MAIL_send_envelope(envelope));


		   printf("\n Mail Send Successfully");
		  }
		  else
		  {


			  printf("\n Unable to  Send MAil");

		  }



   return ifail;

}
