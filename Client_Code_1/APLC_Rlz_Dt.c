/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File			 :   APLC_Rlz_Dt.c
*  Author		 :   Sagar Baviskar
*  Module		 :   Queries the APLC Rlz status object using POM Query and corrects the date_released on it with reference to Effective From Date.
*                            
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes

ITK_CALL(AOM_lock(release_status));
ITK_CALL (POM_attr_id_of_attr("date_released", "Design_0_Revision_alt", &attr_idDR));
ITK_CALL (POM_set_attr_date(1, &release_status, attr_idDR, DRRelDate_tag));
ITK_CALL (AOM_save(release_status));
ITK_CALL(AOM_refresh(release_status,1));// there are 0 and 1 value need to check
ITK_CALL(AOM_unlock(release_status));

***************************************************************************/
#include <stdio.h>
#include <tc/tc.h>
#include <string.h>
#include <stdlib.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tcinit/tcinit.h>
#include <tccore/aom.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <itk/mem.h>
#include <stdlib.h>
#include <tccore/grm.h>
#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/tctype.h>
#include <tccore/imantype.h>
#include <tccore/workspaceobject.h>
#include <tccore/custom.h>
#include <tccore/imantype.h>
#include <tccore/grm.h>
#include <tccore/item_msg.h>
#include <tc/tc.h>
#include <tccore/grm_msg.h>
#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) {  return ifail;}
#define ITK_errStore 91900002
#define Debug TRUE
int status =0;
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

void get_PreviousDate(char Effect_Frm_Dt[100],char ReturnPrevDt[30]) //function to get next date of the passed(parameter/argument) date.
{ 
	printf("\n In get_PreviousDate function: Given date is : %s",Effect_Frm_Dt);
	int x =0;
	char DayTmp[10];
	char MothTmp[10];
	char YearTmp[10];
	int year;
	int month;
	int day;
	char *dd = (char *)malloc(100);
	char *mm = (char *)malloc(100);
	char *yy = (char *)malloc(100);
	char Day[10];
	char Year[10];
	char *token = NULL;
	//logical date_is_valid_l;
	//date_t NextDate;

    token = strtok(Effect_Frm_Dt, "-"); // Splitting string

	while (token != NULL) 
    { 
        if(x==0)
		{
			strcpy(DayTmp,token);
			day = atoi(DayTmp);        // Converting string to int
		}
		if(x==1)
		{
			strcpy(MothTmp,token);
			 month = atoi(MothTmp);
		}
		if(x==2)
		{
			strcpy(YearTmp,token);
			year = atoi(YearTmp);      // Converting string to int
		}
		token = strtok(NULL, "-");
		x++;
    } 
	printf("\n Given date as string after splitting(DD-MM-YY) = %s-%s-%s \n",DayTmp,MothTmp,YearTmp ); // Print splitted date as string	

	if (strcmp(MothTmp,"Jan")==0)
	{month = 1;}
	else if (strcmp(MothTmp,"Feb")==0)
	{month = 2;}
	else if (strcmp(MothTmp,"Mar")==0)
	{month = 3;}
	else if (strcmp(MothTmp,"Apr")==0)
	{month = 4;}
	else if (strcmp(MothTmp,"May")==0)
	{month = 5;}
	else if (strcmp(MothTmp,"Jun")==0)
	{month = 6;}
	else if (strcmp(MothTmp,"Jul")==0)
	{month = 7;}
	else if (strcmp(MothTmp,"Aug")==0)
	{month = 8;}
	else if (strcmp(MothTmp,"Sep")==0)
	{month = 9;}
	else if (strcmp(MothTmp,"Oct")==0)
	{month = 10;}
	else if (strcmp(MothTmp,"Nov")==0)
	{month = 11;}
	else if (strcmp(MothTmp,"Dec")==0)
	{month = 12;}
	printf("\n Given date as int after splitting (day-month-year) = %d-%d-%d \n",day,month,year ); // Print splitted date as int

    day = day -1;

	//Day logic, based on that month and year
	if (day ==0 )
	 {
		if (month==1)
		{
			month=12;
			year = year -1;
		}
		else
		{
			month=month-1;
		}

		if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12 )
		{
			day = 31;
		}
		else if (month == 2 )
		{
			if(year % 400 == 0 || (year % 100 != 0 && year % 4 == 0))  // Leap year checking, if yes, Feb will have 29 days.
			{
				day = 29;
			}
			else
			{
				day = 28;
			}
		}
		else
		{
			day = 30;
		}
	}   
    
	printf("\n Previous Date as int(day-month-year) = %d-%d-%d \n",day,month,year ); // Print Previous date in int	
	
	char	*ddmm	=	NULL;
	ddmm	=	(char*) malloc(5);
	sprintf(ddmm, "%d", day);
	sprintf(Year, "%d", year);
	if ( day<10)
	{
		printf("\nDay less than 10");fflush(stdout);
		strcpy(Day,"0");
		strcat(Day,ddmm);
	}
	else
	{
		strcpy(Day,ddmm);
	}
	printf("\nDay......... : %s",Day);fflush(stdout);
	//itoa(year,Year,10);
	strcpy(dd,Day);
    //strcpy(mm,itoa(month));
	strcpy(yy,Year);

	if (month == 1)
	{strcpy (mm, "Jan");}
	else if (month == 2)
	{strcpy (mm, "Feb");}
	else if (month == 3)
	{strcpy (mm, "Mar");}
	else if (month == 4)
	{strcpy (mm, "Apr");}
	else if (month == 5)
	{strcpy (mm, "May");}
	else if (month == 6)
	{strcpy (mm, "Jun");}
	else if (month == 7)
	{strcpy (mm, "Jul");}
	else if (month == 8)
	{strcpy (mm, "Aug");}
	else if (month == 9)
	{strcpy (mm, "Sep");}
	else if (month == 10)
	{strcpy (mm, "Oct");}
	else if (month == 11)
	{strcpy (mm, "Nov");}
	else if (month == 12)
	{strcpy (mm, "Dec");}

    printf("\n Previous Date as string (dd-mm-yy) = %s-%s-%s \n",dd,mm,yy); // Print Previous date in string
	int size = strlen(dd) + strlen(mm) + strlen(yy) + 1;
	char *PrevDate_String = malloc(size);
	strcpy (PrevDate_String, dd);
    strcat (PrevDate_String, "-");
	strcat (PrevDate_String, mm);
	strcat (PrevDate_String, "-");
	strcat (PrevDate_String, yy);
	//strcat (PrevDate_String, " ");
	//strcat (PrevDate_String, "00:00");

	printf("\n Previous date is(PrevDate_String) : %s \n",PrevDate_String);

	strcpy (ReturnPrevDt, PrevDate_String);
	
}

int ITK_user_main(int argc,char* argv[]) // Main Function
{
	char *message;
	char *loggedInUser = NULL;
	int  Rows = 0;
	int Columns = 0;

	char * select_attrs[]={"puid"};
	char *value = "T5_LcsAplRlzd";

	void ***Report;

	FILE *fpNoChangeReqd = NULL;
    fpNoChangeReqd = fopen("Status_No_ChangeReq.csv","w");
	FILE *fpChangeReqd = NULL;
    fpChangeReqd = fopen("Status_ChangeReq_50.csv","w");
         		
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_set_journalling( TRUE ));
	   //Auto-Logging in to TC
    status = ITK_auto_login ();
	
    if (status != ITK_ok ) 
	{
              EMH_ask_error_text(status, &message);
              printf(" Error with ITK_auto_login : \"%d\", \"%s\"\n", status, message);
              MEM_free(message);
              return status;
    }
	else
	{
			printf(" Auto login Successfully\n");fflush(stdout);
			POM_get_user_id(&loggedInUser);
			printf(" Logged in User is : %s\n",loggedInUser);fflush(stdout);
	}

    ITK_CALL(POM_enquiry_create("find_ReleaseStatus"));
	printf("\n***** find_ReleaseStatus Query created *********\n");fflush(stdout);

	ITK_CALL(POM_enquiry_add_select_attrs ("find_ReleaseStatus","ReleaseStatus",1,select_attrs));
	printf("\n***** POM_enquiry_add_select_attrs created *********\n");fflush(stdout);
	
	//ITK_CALL(POM_enquiry_set_char_value ("find_ReleaseStatus","status_name", 1 ,(const char*)value,POM_enquiry_bind_value));
	ITK_CALL(POM_enquiry_set_string_value ("find_ReleaseStatus","status_name", 1 ,&value,POM_enquiry_bind_value));

	//POM_enquiry_set_char_expr //POM_enquiry_set_attr_expr
	//ITK_CALL(POM_enquiry_set_char_expr ("find_ReleaseStatus","expr_status_name","ReleaseStatus","object_name",POM_enquiry_between,"status_name"));
	//ITK_CALL(POM_enquiry_set_string_expr ("find_ReleaseStatus","expr_status_name","ReleaseStatus","object_name",POM_enquiry_equal,"APLC Released"));
	ITK_CALL(POM_enquiry_set_attr_expr ("find_ReleaseStatus","expr_status_name","ReleaseStatus","name",POM_enquiry_equal,"status_name"));

	ITK_CALL(POM_enquiry_set_where_expr ("find_ReleaseStatus","expr_status_name"));

	ITK_CALL(POM_enquiry_add_order_attr ("find_ReleaseStatus", "ReleaseStatus", "date_released", POM_enquiry_asc_order));

	ITK_CALL(POM_enquiry_execute("find_ReleaseStatus", &Rows, &Columns, &Report));
	
	printf("\n******After Executing POM_enquiry_execute *********\n");fflush(stdout);
	ITK_CALL(POM_enquiry_delete("find_ReleaseStatus"));
	
	printf(" \n Number of Rows %d and Column %d\n",Rows,Columns);fflush(stdout);

	if (Rows>0)
	{
		printf(" \n Number of APLC Released status found = [%d]\n",Rows);fflush(stdout);

		int i = 0;
		if (Rows>0)
		{
			fprintf(fpChangeReqd,"Part No,Release Status Name,Effectivity,Dt_Rlzd,Req_Dt_Rlzd\n");fflush(fpChangeReqd);
		}
		for(i=0;i<5000;i++) // substitute Rows instead of 50
		{
			int n_instances =0;
			int *instance_levels = 0;
			int *instance_where_found = 0;
			int n_classes = 0;
			int *class_levels = 0;
			int *class_where_found = 0;

			char *RlzStatusName = NULL;
			char *Effect_dt	= NULL;
			char *Effect_dt_tmp	= (char *)malloc(100);

			tag_t  Status_tag = NULLTAG;
			tag_t  *ref_instances = NULLTAG;
			tag_t  *ref_classes = NULLTAG;

			Status_tag	=	 *(tag_t*)(Report[i][0]);

			printf("\n************************************************************************************Processing status number : [%d] \n",i);fflush(stdout);

			ITK_CALL(AOM_ask_value_string(Status_tag,"object_name",&RlzStatusName));
			printf(" \n Release status name = %s\n",RlzStatusName);fflush(stdout);
			
			ITK_CALL(AOM_ask_value_string(Status_tag,"effectivity_text",&Effect_dt));
			printf(" \n Release status Effect_dt = %s\n",Effect_dt);fflush(stdout);
			
			tc_strcpy(Effect_dt_tmp,Effect_dt);

			if (tc_strlen(Effect_dt)>0)
			{
				char *Dt_Rlzd	=	NULL;
				char *Dt_Rlzd_tmp	=	NULL;
				char *Effect_Frm_Dt	=	NULL;
				char *Req_Dt_Rlzd = (char *)malloc(100);
				char *ObjType =	NULL;
				char *PartNo =	NULL;
				char *PartRev =	NULL;

				ITK_CALL(AOM_UIF_ask_value(Status_tag,"date_released",&Dt_Rlzd));
				printf(" \n Release status Dt_Rlzd = %s\n",Dt_Rlzd);fflush(stdout);

				Dt_Rlzd_tmp = strtok(Dt_Rlzd," ");
				printf(" \n Dt_Rlzd_tmp = %s\n",Dt_Rlzd);fflush(stdout);

				Effect_Frm_Dt = strtok(Effect_dt_tmp," ");
				printf(" \n Effect_Frm_Dt = %s\n",Effect_Frm_Dt);fflush(stdout);
				
				get_PreviousDate(Effect_Frm_Dt,Req_Dt_Rlzd);
				printf(" \n Req_Dt_Rlzd = %s\n",Req_Dt_Rlzd);fflush(stdout);

				ITK_CALL(POM_referencers_of_instance(Status_tag, 1,POM_in_db_only,&n_instances,&ref_instances,&instance_levels,&instance_where_found,&n_classes,&ref_classes,&class_levels,&class_where_found));
				printf("\n  Found....n_instances = [%d] for given Released Status...n_classes : %d\n",n_instances,n_classes);
				
				if (n_instances==1)
				{
					tag_t 	class_id	=	NULLTAG;
					char*	class_name	=	NULL;
					tag_t  ref_instancesTemp	= NULLTAG;
					tag_t RlzStat_AttrID = NULLTAG;
					date_t Req_Dt_Rlzd_tag ;
					ref_instancesTemp	=	ref_instances[0];
					//ITK_CALL(POM_class_of_instance	(ref_instances[0],&class_id));	
					ITK_CALL(POM_class_of_instance	(ref_instancesTemp,&class_id));	

					ITK_CALL(POM_name_of_class	(class_id,&class_name));
					printf("\n CLASS Name of instance = %s\n",class_name);

					//ITK_CALL(POM_class_of_instance	(ref_instancesTemp,&class_id));	

					//ITK_CALL(POM_name_of_class	(class_id,&class_name));
					//printf("\n CLASS ITEM FOR ITEM REVISIONS ==%s\n",class_name);

					//ITK_CALL(AOM_UIF_ask_value(ref_instances[0],"object_type",&ObjType));
					//printf(" \n ref_instances ObjType = %s\n",ObjType);fflush(stdout);
					
					if (ref_instances[0]!=NULLTAG)
					{
					}
					else
					{
						printf("\nReference is NULLTAG...!!!");fflush(stdout);
					}
					//ITK_CALL(AOM_ask_value_string(ref_instances[0],"item_id",&PartNo));
					PartNo =	NULL;
					PartRev =	NULL;
					
					AOM_ask_value_string(ref_instancesTemp,"item_id",&PartNo);
					printf(" \n ref_instances PartNo = %s\n",PartNo);fflush(stdout);

					//ITK_CALL(AOM_ask_value_string(ref_instances[0],"item_revision_id",&PartRev));
					AOM_ask_value_string(ref_instancesTemp,"item_revision_id",&PartRev);
					printf(" \n ref_instances PartRev = %s\n",PartRev);fflush(stdout);
					//ITK_CALL(POM_class_of_instance	(ref_instances[0],&class_id));	
			   
					//ITK_CALL(POM_name_of_class	(class_id,&class_name));
					//printf("\n CLASS ITEM FOR ITEM REVISIONS ==%s\n",class_name);
					
					if (tc_strcmp(Dt_Rlzd_tmp,Req_Dt_Rlzd)==0)
					{
						printf(" \n No correction is required as Actual and Required date_released matches. \n");fflush(stdout);
					}
					else
					{
						printf(" \n Correction is required as Actual and Required date_released do not matches. \n");fflush(stdout);
						fprintf(fpChangeReqd,"%s/%s,%s,%s,%s,%s\n",PartNo,PartRev,RlzStatusName,Effect_dt,Dt_Rlzd_tmp,Req_Dt_Rlzd);fflush(fpChangeReqd);
						
						strcat (Req_Dt_Rlzd, " ");
						strcat (Req_Dt_Rlzd, "00:00");
						printf(" \n Required Released Date = %s\n",Req_Dt_Rlzd);fflush(stdout);
						ITK_CALL(ITK_string_to_date(Req_Dt_Rlzd,&Req_Dt_Rlzd_tag));

						//ITK_CALL(AOM_UIF_set_value(Status_tag,"date_released",Req_Dt_Rlzd));
						ITK_CALL(AOM_lock(Status_tag));
						ITK_CALL (POM_attr_id_of_attr("date_released", "ReleaseStatus", &RlzStat_AttrID));
						ITK_CALL (POM_set_attr_date(1, &Status_tag, RlzStat_AttrID, Req_Dt_Rlzd_tag));
						ITK_CALL (AOM_save(Status_tag));
						ITK_CALL(AOM_refresh(Status_tag,0));
						ITK_CALL(AOM_unlock(Status_tag));
					}						
				}
				else if (n_instances >1)
				{
					printf("\n Multiple POM Ref found");fflush(stdout);
				}
				else
				{
					printf("\n No POM Ref found");fflush(stdout);
				}
			}
			else
			{
				printf(" \n Effective date not found...\n");fflush(stdout);
				//ITK_CALL(POM_referencers_of_instance(Status_tag, 1,POM_in_db_only,&n_instances,&ref_instances,&instance_levels,&instance_where_found,&n_classes,&ref_classes,&class_levels,&class_where_found));
				//printf("\n  date not found....n_instances = [%d] for given Released Status...\n",n_instances);
			}
		}
		if (fpChangeReqd)
		{
			fclose(fpChangeReqd);
		}

	}
		
    CLEANUP:
	ITK_CALL(ITK_exit_module(TRUE));	
	return ITK_ok;
}




// compile -DIPLIB=none APLC_Rlz_Dt.c
// linkitk -o APLC_Rlz_Dt APLC_Rlz_Dt.o
// APLC_Rlz_Dt -u=aplloader -pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba > APLC_Result.log &
// APLC_Rlz_Dt -u=infodba -p=infodba -g=dba
// scp tcuaadev@172.22.97.90:/user/tcuaadev/devgroups/sagar/APLC_Rlz_Dt/APLC_Rlz_Dt .
// APLC_Rlz_Dt -u=aplloader -pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba
