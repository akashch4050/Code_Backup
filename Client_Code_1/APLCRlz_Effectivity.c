/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File			 :   APLCRlz_Effectivity.c
*  Author		 :   Sagar Baviskar
*  Module		 :   
*                            
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes

***************************************************************************/
#include <stdio.h>
#include <tc/tc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tcinit/tcinit.h>
#include <tccore/aom.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <itk/mem.h>
#include <stdlib.h>
#include <tccore/grm.h>
#include <tc/emh.h>
#include <tccore/tctype.h>
#include <tccore/workspaceobject.h>
#include <tccore/custom.h>
#include <tccore/item_msg.h>
#include <tccore/grm_msg.h>
//#include "tm_apl_std_common_function.c"
#include <epm/epm.h>
#include <ae/dataset_msg.h>
#include <tccore/iman_msg.h>
#include <ps/ps.h>
#include <pie/pie.h>//Added by Anshul Multilevel BOM explode
#include <ps/ps_errors.h>
#include <ai/sample_err.h>
#include <bom/bom.h>
#include <ae/dataset.h>
#include <ps/ps_errors.h>
#include <sa/sa.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fclasses/tc_string.h>
#include <sa/tcfile.h>
#include <epm/releasestatus.h>
#include <res/reservation.h>
#include <tccore/custom.h>
#include <ict/ict_userservice.h>
#include <sa/imanfile.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <ss/ss_errors.h>
#include <sa/user.h>
#include <epm/cr_effectivity.h>
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


void tm_fnd_Next_revision(char	*req_item,tag_t t_currentRevision, tag_t* t_NextRevision)  //function to get next revision 
{
	int		Item_count			=	0;
	int		RevFlag				=	0;
	int		NextRevFlag 		=	0;
	int		ii					=	0;
	int		iRevLength			=	0;

	char	*Part_Rev			=	NULL;
	char	*Part_Rev_copy		=	NULL;
	char	*Part_rev_Id		=	NULL;
	char	*RevOnly			=	NULL;
	char	*NextRevOnly		=	NULL;
	char	*tmpNextRev			=	NULL;
	char	*Part_next_rev_Id	=	NULL;

	tag_t	All_item_tag		=	NULLTAG;
	tag_t	*rev_list			=	NULLTAG;

	printf("\ntm_fnd_prev_revision called : %s",req_item);fflush(stdout);

	ITKCALL(AOM_ask_value_string(t_currentRevision,"item_revision_id",&Part_Rev));
	printf("\n Part_Rev: %s\n",Part_Rev);fflush(stdout);

	ITKCALL(AOM_ask_value_string(t_currentRevision,"item_revision_id",&Part_Rev_copy));
	printf("\n Part_Rev_copy: %s\n",Part_Rev_copy);fflush(stdout);

	ITKCALL(AOM_ask_value_tags(t_currentRevision,"revision_list",&Item_count,&rev_list));
	printf("\nNo of Item : %d",Item_count);fflush(stdout);


	if(Item_count > 0)
	{
		RevFlag		=	0;
		NextRevFlag	=	0;

		for(ii=0;ii<Item_count;ii++)
		{
			RevOnly		=	NULL;
			NextRevOnly	=	NULL;

			ITKCALL(AOM_ask_value_string(rev_list[ii],"item_revision_id",&Part_rev_Id));

			printf("\n Part number : %s Part_Rev %s and Part_rev_Id: %s\n",req_item,Part_Rev,Part_rev_Id);fflush(stdout);
			if(tc_strcmp(Part_Rev,Part_rev_Id)==0)
			{
				RevFlag = 1;
			}
			printf("\n RevFlag : %d.", RevFlag);fflush(stdout);
			if(RevFlag == 1)
			{
				//if (ii < Item_count)
				{				
					RevOnly		= strtok( Part_Rev_copy, ";" );
					NextRevOnly	= strtok( Part_rev_Id, ";" );

					printf("\n RevOnly: %s",RevOnly);fflush(stdout);
					printf("\n NextRevOnly: %s",NextRevOnly);fflush(stdout);
					printf("\n Next Revision Length : %d",iRevLength);fflush(stdout);
					if(tc_strcmp(RevOnly,NextRevOnly)!=0)
					{						
						iRevLength	=	strlen(NextRevOnly);
						printf("\n Next Revision Length : %d",iRevLength);fflush(stdout);
						if(NextRevFlag ==0)
						{
							tmpNextRev	=	(char *)malloc(10);
							tc_strcpy(tmpNextRev,NextRevOnly);
							NextRevFlag =	1;
						}
						printf("\n NextRevFlag : %d, tmpNextRev : %s",NextRevFlag,tmpNextRev);fflush(stdout);
						if (tc_strcmp(tmpNextRev,NextRevOnly)==0)
						{
							printf("\n Same Revision of Part --> %s, temp rev --> %s, current rev --> %s ",req_item,tmpNextRev,Part_rev_Id);fflush(stdout);
							*t_NextRevision	=	rev_list[ii];
						}
						else
						{
							printf("\n Same Revision of Part --> %s is not same, temp rev --> %s, current rev --> %s ",req_item,tmpNextRev,Part_rev_Id);fflush(stdout);

							ITKCALL(AOM_ask_value_string(rev_list[ii-1],"item_revision_id",&Part_next_rev_Id));
							printf("\n Part_next_rev_Id: %s\n",Part_next_rev_Id);fflush(stdout);
							*t_NextRevision	=	rev_list[ii-1];
							break;
						}
					}
					else
					{
						printf("\n Next Revision Length : %d",iRevLength);fflush(stdout);
						if (ii	==	Item_count-1)
						{
							printf("\n ii : %d,Item_count-1 : %d",ii,Item_count-1);fflush(stdout);
							*t_NextRevision	=	rev_list[ii];
							break;
						}
					}
				}
			}
		}
		if(RevFlag == 1 && NextRevFlag ==0)
		{
			*t_NextRevision	=	rev_list[ii];
		}
	}
}

void get_NextDate(char ERC_Rlz_Date[100],char ReturnNxtDt[30]) //function to get next date of the passed(parameter/argument) date.
{ 
	printf("\n In getNextDate function: Given date is : %s",ERC_Rlz_Date);
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

    token = strtok(ERC_Rlz_Date, "-"); // Splitting string

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
	
	int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    day = day +1;
    if (month == 2 )
    {
        if(year % 400 == 0 || (year % 100 != 0 && year % 4 == 0))  // Leap year checking, if yes, Feb will have 29 days.
        {
            daysInMonth[1] = 29;
        }
    }

    if (day > daysInMonth[month -1])  // If day > daysInMonth then change the month.
    {
        day = 1;
        month = month +1;
        if (month > 12)               // If month > 12 then change the year.
        {
            month = 1;
            year = year +1;
        }
    }
	printf("\n Next Date as int(day-month-year) = %d-%d-%d \n",day,month,year ); // Print Next date in int	
	
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

    printf("\n Next Date as string (dd-mm-yy) = %s-%s-%s \n",dd,mm,yy); // Print Next date in string
	int size = strlen(dd) + strlen(mm) + strlen(yy) + 1;
	char *NxtDate_String = malloc(size);
	strcpy (NxtDate_String, dd);
    strcat (NxtDate_String, "-");
	strcat (NxtDate_String, mm);
	strcat (NxtDate_String, "-");
	strcat (NxtDate_String, yy);
	strcat (NxtDate_String, " ");
	strcat (NxtDate_String, "00:00");

	printf("\n Next date is(NxtDate_String) : %s \n",NxtDate_String);

	strcpy (ReturnNxtDt, NxtDate_String);
	
}

int Add_Effectivity(tag_t Obj_tag, char	**cPrevDate, FILE *fpPart_Count_NCR,FILE *fpPart_Count_CR, FILE *fpPart_Count_Eff_Add, int i) //function to check release status and effectivity on the object received.
{	
	int st_count1 = 0;
	int RlzStatLst_Length = 0;
	int count=0;
	int	ifail=0;
	int Task_Count = 0;
	int APLDML_Count = 0;
	int statusfnd=0;
	int Count = 0;
	int Flag_ERC_Rlz = 0;
	int Flag_APL_Rlz = 0;
	int Flag_STD_Rlz = 0;
	int Part_Status_Count = 0;

	tag_t class_id = NULLTAG;
	tag_t RlzStatLst_AttrID = NULLTAG;
	tag_t *RlzStatLst_Attr_tag = NULLTAG;
	tag_t Owning_Grp = NULLTAG;    
	tag_t Prt_Task_RelType_tag = NULLTAG;
    tag_t Task_DML_RelType_tag = NULLTAG;
	tag_t *Task_tag = NULLTAG;
	tag_t *APLDML_tag = NULLTAG;	
	tag_t erc_release_status = NULLTAG;
	logical	Retain_Released_Date = 0;
	tag_t  APL_Rlz_Status = NULLTAG;
	tag_t *Part_Status_List = NULLTAG;
	tag_t *status_list1 = NULLTAG;
	tag_t EffTxt_Tag = NULLTAG;
	tag_t eff_d = NULLTAG;

	logical	log1;
	logical	*is_it_null = NULL;
	logical	*is_it_empty = NULL;
	logical	retain_erc_released_date = false;
	logical stat;	
	
	char *class_name = NULL;
	char *Release_Status = NULL;
	char *Obj_ID = NULL;
	char *Obj_Type = NULL;
	char *ItemID = NULL;
    char *ItemRevID = NULL;
	char *GroupID = NULL;
	char *ERCDML_rlz_date = NULL;
	char *APLRlzStatus = NULL;
	char *PartEff = NULL;
	char *PartEff_FrmDt = NULL;
	char *PartEff_ToDt = NULL;
	char *Part_Rlz_Status = NULL;
	char *PartID = NULL;
	char *PartRevID = NULL;
	char *ERC_ClosureDate = NULL;
	char *token = NULL;	
	char *NxtRev_APL_Rlz_Date = NULL;	
	char *tmp_date	=	NULL;
	char *tmp_PartEff	=	NULL;

	ITK_CALL(POM_class_of_instance(Obj_tag,&class_id)); 
	ITK_CALL(POM_name_of_class(class_id,&class_name));
	printf("\n Class Name is : %s\n",class_name);fflush(stdout);
	ITK_CALL(POM_attr_id_of_attr("release_status_list",class_name,&RlzStatLst_AttrID));
	ITK_CALL(WSOM_ask_release_status_list(Obj_tag,&st_count1,&status_list1));
	printf("\n st_count1 is : %d \n",st_count1);fflush(stdout);
	ITK_CALL(POM_unload_instances(1,&Obj_tag));
	ITK_CALL(POM_load_instances(1,&Obj_tag,class_id,1));
	ITK_CALL(POM_is_loaded(Obj_tag,&log1));
	if(log1 == 1)
	{
		 printf(" Load Success\n " );
	}
	else
	printf(" Load Failure\n"  );
	ITK_CALL( POM_length_of_attr(Obj_tag, RlzStatLst_AttrID, &RlzStatLst_Length) );
	ITK_CALL( POM_ask_attr_tags(Obj_tag, RlzStatLst_AttrID, 0, RlzStatLst_Length, &RlzStatLst_Attr_tag, &is_it_null, &is_it_empty ));
    

	AOM_ask_value_string(Obj_tag,"item_id",&PartID);
	AOM_ask_value_string(Obj_tag,"item_revision_id",&PartRevID);
	ITK_CALL(WSOM_ask_release_status_list(Obj_tag,&Part_Status_Count,&Part_Status_List));

	Flag_APL_Rlz = 0;
	tmp_date	=	(char*)malloc(100);
	tc_strcpy(tmp_date,*cPrevDate);

	for(Count=0; Count<Part_Status_Count;Count++)
	{
		AOM_ask_name(Part_Status_List[Count], &Part_Rlz_Status);
				
		if((tc_strcmp(Part_Rlz_Status,"T5_LcsAplRlzd")==0))	
		{
			Flag_APL_Rlz++;
			printf("\n Flag_APL_Rlz (Part`s Status Flag) = %d ",Flag_APL_Rlz);fflush(stdout);

			AOM_ask_value_string(Part_Status_List[Count],"effectivity_text",&PartEff);
			printf("\n Part`s Effectivity (APLC Released) = %s ",PartEff);fflush(stdout);			
			PartEff_FrmDt = strtok(PartEff," ");			
			tmp_PartEff = strtok(NULL," ");
			printf("\n tmp_PartEff: %s",tmp_PartEff);fflush(stdout);
			tmp_PartEff = strtok(NULL," ");
			printf("\n tmp_PartEff: %s",tmp_PartEff);fflush(stdout);
			PartEff_ToDt = strtok(NULL," ");
			printf("\n tmp_PartEff: %s",PartEff_ToDt);fflush(stdout);

			printf("\n PartEff_FrmDt: [ %s ], PartEff_ToDt: [ %s ] (Actual Effective Dt on Part)\n",PartEff_FrmDt,PartEff_ToDt);fflush(stdout);
			break;
		}					
	}

	if(Flag_APL_Rlz > 0)
	{
		printf("\n Part`s status is APLC Released  \n");fflush(stdout);	
	
		GRM_find_relation_type("CMHasSolutionItem", &Prt_Task_RelType_tag);
		GRM_list_primary_objects_only(Obj_tag,Prt_Task_RelType_tag,&Task_Count,&Task_tag); //Finds primary object:Task from Part
		printf("\n Task_Count = %d",Task_Count);fflush(stdout);

		if (Task_Count > 0)
		{
			int Count = 0; 
			int APLTaskFound=0;
			for (Count=0; Count<Task_Count;Count++)
				{										
					tag_t TaskRevTag  = NULLTAG;
					TaskRevTag  = Task_tag[Count];
					tag_t TaskTypeTag = NULLTAG;
					char *Task_Type = NULL;

					TCTYPE_ask_object_type(TaskRevTag,&TaskTypeTag);
					TCTYPE_ask_name2(TaskTypeTag,&Task_Type);
					printf("\n Task Type = %s",Task_Type);fflush(stdout);									
					
					if (tc_strcmp(Task_Type,"T5_APLTaskRevision")==0) // Check for APL DML Task Revision
					{
						APLTaskFound++;
						printf("\n APLTaskFound = %d",APLTaskFound);fflush(stdout);

						GRM_find_relation_type("T5_DMLTaskRelation", &Task_DML_RelType_tag);
						GRM_list_primary_objects_only(Task_tag[Count],Task_DML_RelType_tag,&APLDML_Count,&APLDML_tag);   //Finds primary object:DML(APLDML) from Task
						printf("\n APLDML_Count = %d",APLDML_Count);fflush(stdout);
						
						if(APLDML_Count >0)
						{
							int Count1 = 0;
							for (Count1=0; Count1<APLDML_Count;Count1++)
								{
									tag_t DMLRevTag  = NULLTAG;
									DMLRevTag  = APLDML_tag[Count1];
									tag_t DMLTypeTag = NULLTAG;
									char *DML_Type = NULL;
									tag_t     eff_t =NULLTAG;

									TCTYPE_ask_object_type(DMLRevTag,&DMLTypeTag);
									TCTYPE_ask_name2(DMLTypeTag,&DML_Type);
									printf("\n DML Type = %s",DML_Type);fflush(stdout);
									
									if (tc_strcmp(DML_Type,"T5_APLDMLRevision")==0)
									{
										int Count2 = 0;
										int k=0;
										int Flag_APLDML = 0;
										int APLDML_Status_Count = 0;
										tag_t *APLDML_Status_List = NULLTAG;
										char *APLDML_Rlz_Status = NULL;
										char *APLDML_ID = NULL;
										char *APLDML_Rev_ID = NULL;
										char *Nxt_Date_Str_tmp = (char *)malloc(100);
										char *Nxt_Date_Str = NULL;
										char *Next_Dt_Str = NULL;
										char *APL_Rlz_Date = NULL;
										int Part_Stat_Cnt = 0;
										tag_t *Part_Stat_Lst = NULLTAG;
										char *Part_Rlz_Stat = NULL;
										tag_t Prt_APLC_Rlz_Status = NULLTAG;
										date_t FromDate;
										date_t FromDate_NR;
										date_t ToDate;
										date_t *start_end_date = NULL;
										tag_t eff_d = NULLTAG;
										

										AOM_ask_value_string(APLDML_tag[Count1],"item_id",&APLDML_ID);
										AOM_ask_value_string(APLDML_tag[Count1],"item_revision_id",&APLDML_Rev_ID);																		
										printf("\n APLDML_ID is : [ %s/%s ]",APLDML_ID,APLDML_Rev_ID);fflush(stdout);																																													
																												
										ITK_CALL(WSOM_ask_release_status_list(APLDML_tag[Count1],&APLDML_Status_Count,&APLDML_Status_List));
										printf("\n APLDML_Status_Count : %d\n",APLDML_Status_Count);fflush(stdout);
										
										if(APLDML_Status_Count>0)
										{
											for(Count2=0; Count2<APLDML_Status_Count;Count2++)
												{
													APLDML_Rlz_Status = NULL;
													AOM_ask_name(APLDML_Status_List[Count2], &APLDML_Rlz_Status);
													printf("\n APL DML`s Release status is : %s\n",APLDML_Rlz_Status);fflush(stdout);

													if(tc_strcmp(APLDML_Rlz_Status,"T5_LcsAplRlzd")==0)	
													{
														Flag_APLDML++;
														ITK_CALL(AOM_UIF_ask_value(APLDML_tag[Count1],"t5_APLReleaseDate",&APL_Rlz_Date));
														printf("\n APL_Rlz_Date(APLDML`s t5_APLReleaseDate) : %s\n",APL_Rlz_Date);fflush(stdout);
														
														//Storing APL_Rlz_Date for correction
														NxtRev_APL_Rlz_Date	=	NULL;
														*cPrevDate			=	NULL;
														NxtRev_APL_Rlz_Date	= (char *) MEM_alloc(tc_strlen(APL_Rlz_Date) * sizeof(char *) +5);
														*cPrevDate	= (char *) MEM_alloc(tc_strlen(APL_Rlz_Date) * sizeof(char *) +5);
														tc_strcpy(NxtRev_APL_Rlz_Date,APL_Rlz_Date);
														tc_strcpy(*cPrevDate,APL_Rlz_Date);
														printf("\n NxtRev_APL_Rlz_Date(APLDML`s t5_APLReleaseDate) : %s, cPrevDate_CC : %s\n",NxtRev_APL_Rlz_Date,cPrevDate);fflush(stdout);

														//Logic for APL_Rlz_Date + 1 = FromDate
														get_NextDate(APL_Rlz_Date,Nxt_Date_Str_tmp);    // Current Revision`s APLDML`s APL_Rlz_Date+1 (FromDate)														
														printf("\n FromDate: %s\n",Nxt_Date_Str_tmp);fflush(stdout); // This is Required FromDate
														
														//Convert Nxt_Date_Str_tmp to date_t tag --> FromDate
														ITK_CALL(ITK_string_to_date(Nxt_Date_Str_tmp,&FromDate));														

														Nxt_Date_Str	=	NULL;
														Nxt_Date_Str = (char *)malloc(100);
														tc_strcpy(Nxt_Date_Str,Nxt_Date_Str_tmp);
														Next_Dt_Str = strtok(Nxt_Date_Str_tmp," ");
														break;
													}																						
												}																				
											printf("\n Flag_APLDML (Part`s --> APLDML`s Rlz Flag) = %d \n",Flag_APLDML);fflush(stdout);
											printf("\n Parts Eff From Date(Actual): %s, Parts Eff From Date(Required) : %s\n",PartEff_FrmDt,Next_Dt_Str);fflush(stdout);

											if (tc_strcmp(PartEff_FrmDt,Next_Dt_Str)==0) // Comparing From Date and APL_Rlz_Date + 1 (Actual & Required From date)
											{
												printf("\n No Correction is required in FromDate as Actual and Required FromDate Matched\n");fflush(stdout);
												fprintf(fpPart_Count_NCR,"Part Count: [ %s/%s ]: No Correction is required in FromDate as Part`s Effective Dt.[%s] matches with it`s APLDML`s APL_Rlz_Dt+1 [%s]  \n",PartID,PartRevID,PartEff_FrmDt,Next_Dt_Str);
											}
											else
											{
												printf("\n Correction is required in FromDate as Actual and Required FromDate do not Matche\n");fflush(stdout);
												fprintf(fpPart_Count_CR,"Part Count: [ %s/%s ]: Correction is required in FromDate as Part`s Effective Dt.[%s] does not matches with it`s APLDML`s APL_Rlz_Dt+1 [%s]  \n",PartID,PartRevID,PartEff_FrmDt,Next_Dt_Str);
																		
												//FromDate = Nxt_Date_Str
												ITK_CALL(ITK_string_to_date(Nxt_Date_Str,&FromDate));		// Current Revision`s APLDML`s APL_Rlz_Date+1 (FromDate)															
											}

											printf("\n i = %d\n",i);fflush(stdout);											

											if (i != 0)
											{
												if (tc_strcmp(PartEff_ToDt,tmp_date)==0) // Compating To Date and APL_Rlz_Date + 1 (Actual & Required To date)
												{
													printf("\n No Correction is required in ToDate as Actual and Required ToDate Matched\n");fflush(stdout);
													fprintf(fpPart_Count_NCR,"Part Count: [ %s/%s ]: No Correction is required in ToDate as Part`s Effective Dt.[%s] matches with it`s Next Revision`s APLDML`s APL_Rlz_Dt [%s]  \n",PartID,PartRevID,PartEff_ToDt,tmp_date);

													ITK_CALL(ITK_string_to_date(tmp_date, &ToDate));
												}
												else
												{
													printf("\n Correction is required in ToDate as Actual and Required ToDate do not Matched\n");fflush(stdout);
													printf("\n Parts Eff To Date(Actual): %s, Parts Eff To Date(Required): %s\n",PartEff_ToDt,tmp_date);fflush(stdout);
													fprintf(fpPart_Count_CR,"Part Count: [ %s/%s ]: Correction is required in ToDate as Part`s Effective Dt.[%s] does not matches with it`s Next Revision`s APLDML`s APL_Rlz_Dt [%s]  \n",PartID,PartRevID,PartEff_ToDt,tmp_date);

													ITK_CALL(ITK_string_to_date(tmp_date, &ToDate));													
												}
											}
											else if (i == 0) // For Latest Revision ToDate = 31-Dec-9999 00:00
											{
												printf("\n For Latest Revision ToDate = 31-Dec-9999 00:00\n");
												tc_strcpy(tmp_date,"31-Dec-9999 00:00");
												ITK_CALL(ITK_string_to_date(tmp_date, &ToDate));
											}

											//If correction in FromDate or ToDate is required then update Effectivity
											if ((tc_strcmp(PartEff_FrmDt,Next_Dt_Str)!=0)  || (tc_strcmp(PartEff_ToDt,tmp_date)!=0)) 
											{
												printf("\n Updating Effectivity...\n");
												if (i == 0)
												{
													if (tc_strstr(PartRevID,"NR") != NULL)
													{
														fprintf(fpPart_Count_Eff_Add,"Part Count: [ %s/%s ]: Corrected Effective dates are: From date [ %s ], To Date [ %s ]  \n",PartID,PartRevID,NxtRev_APL_Rlz_Date,tmp_date);
													}
													else
													{
														fprintf(fpPart_Count_Eff_Add,"Part Count: [ %s/%s ]: Corrected Effective dates are: From date [ %s ], To Date [ %s ]  \n",PartID,PartRevID,Nxt_Date_Str,tmp_date);
													}
													
												}
												else
												{
													if (tc_strstr(PartRevID,"NR") != NULL)
													{
														fprintf(fpPart_Count_Eff_Add,"Part Count: [ %s/%s ]: Corrected Effective dates are: From date [ %s ], To Date [ %s ]  \n",PartID,PartRevID,NxtRev_APL_Rlz_Date,tmp_date);
													}
													else
													{
														fprintf(fpPart_Count_Eff_Add,"Part Count: [ %s/%s ]: Corrected Effective dates are: From date [ %s ], To Date [ %s ]  \n",PartID,PartRevID,Nxt_Date_Str,tmp_date);
													}
													
												}
												
												start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);
												if (tc_strstr(PartRevID,"NR") != NULL)
												{
													printf("\n This is NR Revision so setting FromDate as APL DML`s APL_Rlz_Dt. [%s]\n",NxtRev_APL_Rlz_Date);fflush(stdout);
													ITK_CALL(ITK_string_to_date(NxtRev_APL_Rlz_Date,&FromDate_NR));
													start_end_date[0] = FromDate_NR;
													start_end_date[1] = ToDate;
												}
												else
												{
													printf("\n This is not an NR Revision so setting FromDate as APL DML`s APL_Rlz_Dt+1. [%s] \n",Nxt_Date_Str);fflush(stdout);
													start_end_date[0] = FromDate;
													start_end_date[1] = ToDate;
												}											
												
												ITK_CALL(WSOM_ask_release_status_list(Obj_tag,&Part_Stat_Cnt,&Part_Stat_Lst));
												printf("Part_Stat_Cnt = %d",Part_Stat_Cnt);fflush(stdout);
												
												for(k=0;k<Part_Stat_Cnt;k++)
												{
													AOM_ask_name(Part_Stat_Lst[k], &Part_Rlz_Stat);
													printf("\n Part`s Release status is : %s\n",Part_Rlz_Stat);fflush(stdout);

													if (tc_strcmp(Part_Rlz_Stat,"T5_LcsAplRlzd")==0)
													{
														printf("\n APLC Released status found on Part for updating Effectivity\n");
														Prt_APLC_Rlz_Status = Part_Stat_Lst[k];														

														ITKCALL(WSOM_status_clear_effectivities (Prt_APLC_Rlz_Status));
														ITKCALL(WSOM_effectivity_create_with_dates(Prt_APLC_Rlz_Status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &eff_d ));
														ITKCALL(AOM_save(eff_d));
														ITKCALL(AOM_save(Prt_APLC_Rlz_Status));
														ITKCALL(AOM_refresh(Prt_APLC_Rlz_Status,0));
														printf("\n Effectivity Updated...\n");
													}
												}

											}
											
										}
										else
										{
											printf("\n No release status found on Current Revision`s APLDML \n");
										}
									}
								}				
						}	
						else
						{
							printf("\n APLDML not found for Current Revision \n");								
						}
						break;
					}												
					
				}				
		}							
						
	}
	else
	{
		printf("\n Part`s status is not APLC Released  \n");fflush(stdout);			
	}
		
	return ITK_ok;
}

int ITK_user_main(int argc,char* argv[]) // Main Function
{
   int i=0;  
   int status = 0; 
   int n_entries = 2;
   int n_found = 0;
   int	ifail=0;
   int	PartRev_count=0;
   int num_to_sort_DVC=1;

   char *keys_DVC[1] = {"creation_date"};
   int  	 orders_DVC[1]  ={2};
   char **qry_values = (char **) MEM_alloc(10 * sizeof(char *));
   char *qry_entries[2] = {"ID","Release Status"};

   char *message;
   char *loggedInUser = NULL;
   char * item_id_str = NULL;
   char *PartID = NULL;
   char *PartRevID = NULL;
   char *PartStatus = NULL;
   char *Obj_ID = NULL;
   char *Obj_Rev_ID = NULL;   
   char *ERC_Rlz_Date = NULL;
   char *DML_List_File = NULL;
   char* Part_item_id = NULL;  
   char *ReturnNxtDt_1 = (char *)malloc(100);
   char *PartType = NULL;
   char	*cPrevDate	=	NULL;
  
   FILE *fpPart_List = NULL;
   FILE *fpPart_Count_NCR = NULL;
   FILE *fpPart_Count_CR = NULL;
   FILE *fpPart_Count_Eff_Add = NULL;
   fpPart_Count_NCR = fopen("Part_Count_NoChangeReq.txt","w");
   fpPart_Count_CR = fopen("Part_Count_ChangeReq.txt","w");
   fpPart_Count_Eff_Add = fopen("Part_Count_Eff_Add.txt","w");
      
   tag_t QryTag = NULLTAG;
   tag_t *PartRev_list = NULLTAG;
   tag_t *DML_Rev_tag;
   tag_t DMLTag = NULLTAG;
   tag_t TaskTag = NULLTAG;
   tag_t Part_tag = NULLTAG;
   tag_t Owning_Grp = NULLTAG;
   tag_t PartType_tag = NULLTAG;   
   item_id_str = NULL;    
   
  ITK_initialize_text_services (0);
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
			printf(" Auto login Successfully\n");
			POM_get_user_id(&loggedInUser);
			printf(" Logged in User is : %s\n",loggedInUser);
		}

    ITK_set_journalling (TRUE);
	
	fpPart_List = fopen("Part_List_File.txt","r");	
	
	if(fpPart_List != NULL)
		{	
			printf("\n Part_List != NULL\n");fflush(stdout);
			Part_item_id=(char *) MEM_alloc(100);					
			
			 while(fgets(Part_item_id,100,fpPart_List)!=NULL)
				{
					item_id_str=strtok(Part_item_id,"^");
					printf("\n Part_item_id (item_id_str) = %s \n",item_id_str);					

					//char *qry_entries[2] = {"Item ID","Release Status"}; 
					//char *qry_values[2] = {item_id_str,"T5_LcsAplRlzd"};
					//char *qry_values[1] = {"APLC Released"};
					//char *qry_values[1] = {"T5_LcsAplRlzd"};

					qry_values[0] = item_id_str ;
					qry_values[1] = "T5_LcsAplRlzd" ;


					ITK_CALL(QRY_find("DriverVCQuerySM",&QryTag));
					ITK_CALL(QRY_execute_with_sort(QryTag,n_entries, qry_entries, qry_values,num_to_sort_DVC, keys_DVC, orders_DVC,&PartRev_count,&PartRev_list)); // Query result are sorted on the basis of creation date(Latest Rev is first in list)
					printf("\n PartRev_count(Qry Result) = %d \n",PartRev_count);
					
					for(i=0; i<PartRev_count; i++)  //Start of PartRev_count loop
					{
						Part_tag = PartRev_list[i];

						ITK_CALL(AOM_ask_value_string(Part_tag,"item_id",&PartID));
						ITK_CALL(AOM_ask_value_string(Part_tag,"item_revision_id",&PartRevID));
						printf("Part ID: [%s/%s]",PartID,PartRevID);fflush(stdout);
						Add_Effectivity(Part_tag,&cPrevDate,fpPart_Count_NCR,fpPart_Count_CR,fpPart_Count_Eff_Add,i); // Calling Function
						printf("\nCprevDate(In Main Function) : %s\n",cPrevDate);fflush(stdout);
					
					} //End of PartRev_count loop
				} //End of while loop

					fclose(fpPart_List) ;           
					printf("\n Data successfully read from file Part_List_File.txt\n");fflush(stdout); 
					printf("\n The file is now closed.\n");fflush(stdout); 
			} //End of Part_List condition
				
		else 
			{
				printf("\n DML_List = NULL");fflush(stdout);
			}
		
			 
    CLEANUP:
	ITK_CALL(ITK_exit_module(TRUE));	
	return ITK_ok;
}

// compile -DIPLIB=none APLCRlz_Effectivity.c
// linkitk -o APLCRlz_Effectivity APLCRlz_Effectivity.o
// APLCRlz_Effectivity -u=aplloader -pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba > Result.log &
// APLCRlz_Effectivity -u=infodba -p=infodba -g=dba
// scp tcuaadev@172.22.97.90:/user/tcuaadev/devgroups/sagar/APLCRlz_Effectivity/APLCRlz_Effectivity . 