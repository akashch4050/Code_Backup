/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File			 :   Check_And_Add_DML_Status.c
*  Author		 :   Sagar Baviskar
*  Module		 :   Checks the status of DML and associated Tasks/Parts and  adds status and effectivity if required.
*                            
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes

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
	 


void get_NextDate(char ERC_Rlz_Date[100],char *ReturnNxtDt) //function to get next date of the passed(parameter/argument) date.
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

	sprintf(Day, "%d", day);
	sprintf(Year, "%d", year);
	//itoa(day,Day,10);
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
	strcat (NxtDate_String, " 00:00");

	printf("\n Next date is(NxtDate_String) : %s \n",NxtDate_String);

	strcpy (ReturnNxtDt, NxtDate_String);
	
}

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

void AddEffectivity(tag_t Obj_tag) //function to add effectivity
{ 
	tag_t EffTxt_Tag = NULLTAG;
	logical stat;
	char *value = NULL;
	char *ERC_Rlz_Date = NULL;
	char *ERC_Rlz_Date_2 = NULL;
	char *Nxt_Date_Str = (char *)malloc(100);
	logical date_is_valid_l;
	date_t FromDate;
	date_t ToDate;
	date_t *start_end_date = NULL;
	tag_t t_NextRevision = NULLTAG;
	char *PartRevID_1 = NULL;
	char *PartRevID = NULL;
	int Status_Count = 0;
	tag_t *Status_List = NULLTAG;
	char *Rlz_Status = NULL;
	char *Part_Rlz_Status = NULL;
	int Flag_ErcRlzd = 0;
	int jj = 0;
	int Count = 0;
	int Flag_ERC_Rlz = 0;
	int Part_Status_Count = 0;
	tag_t *Part_Status_List = NULLTAG;
	tag_t eff_d = NULLTAG;
	char *PartID = NULL;
	int Flag_ERCDML_2 = 0;
	tag_t Prt_Task_RelType_tag_1 = NULLTAG;
	tag_t Task_DML_RelType_tag_1 = NULLTAG;
	int ERCDML_Count_1 = 0;
	int ERCDML_Task_Count_1 = 0;
	tag_t *ERC_Task_tag_1 = NULLTAG;
	tag_t *ERCDML_tag_1 = NULLTAG;
	
	ITKCALL(WSOM_ask_release_status_list(Obj_tag,&Part_Status_Count,&Part_Status_List));

			for(Count=0; Count<Part_Status_Count;Count++)
				{
					AOM_ask_name(Part_Status_List[Count], &Part_Rlz_Status);

					if((tc_strcmp(Part_Rlz_Status,"T5_LcsErcRlzd")==0))	
						{
							Flag_ERC_Rlz++;
							printf("\n Flag_ERC_Rlz = %d ",Flag_ERC_Rlz);
							printf("\n Parts status is ERC Released \n");
							//Effectivity Start
							ITKCALL(WSOM_ask_effectivity_mode(&stat));
							ITKCALL(PROP_ask_property_by_name(Part_Status_List[Count],"effectivity_text",&EffTxt_Tag));
							ITKCALL(PROP_ask_value_string(EffTxt_Tag,&value));							
							
							GRM_find_relation_type("CMHasSolutionItem", &Prt_Task_RelType_tag_1);
							GRM_list_primary_objects_only(Obj_tag,Prt_Task_RelType_tag_1,&ERCDML_Task_Count_1,&ERC_Task_tag_1); //Finds primary object:Task from Part
							printf("\n Task_Count = %d",ERCDML_Task_Count_1);

							if (ERCDML_Task_Count_1 > 0)
								{
									int Count = 0; 
									int ercTaskFound=0;
									for (Count=0; Count<ERCDML_Task_Count_1;Count++)
										{											
											tag_t TaskRevTag_1  = NULLTAG;
											TaskRevTag_1  = ERC_Task_tag_1[Count];
											tag_t TaskTypeTag_1 = NULLTAG;
											char *Task_Type_1 = NULL;
											TCTYPE_ask_object_type(TaskRevTag_1,&TaskTypeTag_1);
											TCTYPE_ask_name2(TaskTypeTag_1,&Task_Type_1);
											printf("\n Task Type = %s",Task_Type_1);											

											if (tc_strcmp(Task_Type_1,"T5_ChangeTaskRevision")==0) // Check for ERC DML Task Revision
												{
													ercTaskFound++;
													GRM_find_relation_type("T5_DMLTaskRelation", &Task_DML_RelType_tag_1);
													GRM_list_primary_objects_only(ERC_Task_tag_1[Count],Task_DML_RelType_tag_1,&ERCDML_Count_1,&ERCDML_tag_1);   //Finds primary object:DML(ERCDML) from Task
													printf("\n ERCDML_Count_1 = %d",ERCDML_Count_1);
													
													if(ERCDML_Count_1 >0)
														{
															int Count1 = 0;
															for (Count1=0; Count1<ERCDML_Count_1;Count1++)
																{
																	tag_t DMLRevTag_1  = NULLTAG;
																	DMLRevTag_1  = ERCDML_tag_1[Count1];
																	tag_t DMLTypeTag_1 = NULLTAG;
																	char *DML_Type_1 = NULL;
																	tag_t     eff_t =NULLTAG;
																	TCTYPE_ask_object_type(DMLRevTag_1,&DMLTypeTag_1);
																	TCTYPE_ask_name2(DMLTypeTag_1,&DML_Type_1);
																	printf("\n DML Type = %s",DML_Type_1);
																	
																	if (tc_strcmp(DML_Type_1,"ChangeRequestRevision")==0)
																	{
																		int Count2 = 0;
																		int Flag_ERCDML_1 = 0;
																		int ERCDML_Status_Count_1 = 0;
																		tag_t *ERCDML_Status_List_1 = NULLTAG;
																		char *ERCDML_Rlz_Status_1 = NULL;
																		char *ERCDML_ID_1 = NULL;
																		char *ERCDML_Rev_ID_1 = NULL;
																		char *ERC_Rlz_Date_1 = NULL;

																		AOM_ask_value_string(ERCDML_tag_1[Count1],"item_id",&ERCDML_ID_1);
																		AOM_ask_value_string(ERCDML_tag_1[Count1],"item_revision_id",&ERCDML_Rev_ID_1);
																		printf("\n ERCDML_ID_1 is : %s",ERCDML_ID_1);
																		printf("\n ERCDML_Rev_ID_1 is : %s",ERCDML_Rev_ID_1);																	    
																		
																		ITKCALL(WSOM_ask_release_status_list(ERCDML_tag_1[Count1],&ERCDML_Status_Count_1,&ERCDML_Status_List_1));
																		printf("\n ERCDML_Status_Count_1 : %d\n",ERCDML_Status_Count_1);
																		
																		if(ERCDML_Status_Count_1>0)
																			{
																				for(Count2=0; Count2<ERCDML_Status_Count_1;Count2++)
																					{
																						ERCDML_Rlz_Status_1 = NULL;
																						AOM_ask_name(ERCDML_Status_List_1[Count2], &ERCDML_Rlz_Status_1);
																						printf("\n ERC DML Release status is : %s",ERCDML_Rlz_Status_1);
																						if(tc_strcmp(ERCDML_Rlz_Status_1,"T5_LcsErcRlzd")==0)	
																							{
																								Flag_ERCDML_1++;
																								ITKCALL(AOM_UIF_ask_value(ERCDML_tag_1[Count1],"date_released",&ERC_Rlz_Date_1));
																								printf("\n ERC_Rlz_Date_1 : %s\n",ERC_Rlz_Date_1);fflush(stdout);
																								//Logic for ERC_Rlz_Date_1 + 1 = FromDate
																								get_NextDate(ERC_Rlz_Date_1,Nxt_Date_Str);
																								break;
																							}																						
																					}																				
																				printf("\n Flag_ERCDML_1 = %d \n",Flag_ERCDML_1);																				
																			}
																		else
																			{
																				printf("\n No release status found on ERC DML \n");
																			}
																	}

																}										
														}	
													else
														{
															printf("\n ERCDML not found for this part \n");								
														}
													break;
												}								
										}										
								}						   
																												
							//FromDate = Nxt_Date_Str
							ITKCALL(ITK_string_to_date(Nxt_Date_Str, &FromDate));
													
							start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);
							start_end_date[0] = FromDate;
							//start_end_date[1] = ToDate;
							
							// Check next revision release status for ToDate
							AOM_ask_value_string(Obj_tag,"item_id",&PartID);
							printf("\n Before tm_fnd_Next_revision\n");fflush(stdout);
							tm_fnd_Next_revision(PartID,Obj_tag,&t_NextRevision);
							printf("\n After tm_fnd_Next_revision\n");fflush(stdout);
							if (t_NextRevision != NULLTAG)
							{								
								AOM_ask_value_string(Obj_tag,"item_revision_id",&PartRevID);
								AOM_ask_value_string(t_NextRevision,"item_revision_id",&PartRevID_1);
								printf("\n Next revision of Part found : [%s],[%s] ",PartID,PartRevID_1);
								if (strcmp(PartRevID,PartRevID_1)==0)
								{
									printf("\n In PartRevID = PartRevID_1");
									ITKCALL(ITK_string_to_date("31-Dec-9999 00:00", &ToDate ));
									printf("\n ToDate = 31-Dec-9999 00:00 \n");
								}
								else
								{
									ITKCALL(WSOM_ask_release_status_list(t_NextRevision,&Status_Count,&Status_List));
									printf("\n Status_Count = %d",Status_Count);fflush(stdout);
									if (Status_Count>0)
									{
										for(jj=0; jj<Status_Count;jj++)
										{
											AOM_ask_name(Status_List[jj], &Rlz_Status);
											if((tc_strcmp(Rlz_Status,"T5_LcsErcRlzd")==0))	
											{																												
												Flag_ErcRlzd ++;
												int ERCDML_Task_Count_2 =0;								
												tag_t Prt_Task_RelType_tag_2 = NULLTAG;
												tag_t Task_DML_RelType_tag_2 = NULLTAG;
												int ERCDML_Count_2 = 0;												
												tag_t *ERC_Task_tag_2 = NULLTAG;
												tag_t *ERCDML_tag_2 = NULLTAG;
												printf("\n T5_LcsErcRlzd status found on Next Part Revision : [%s],[%s] ",PartID,PartRevID_1);
	
												//Logic for next revision`s -->ERC DML`s-->date_released
												GRM_find_relation_type("CMHasSolutionItem", &Prt_Task_RelType_tag_2);
												GRM_list_primary_objects_only(t_NextRevision,Prt_Task_RelType_tag_2,&ERCDML_Task_Count_2,&ERC_Task_tag_2); //Finds primary object:Task from Part
												printf("\n Task_Count = %d",ERCDML_Task_Count_2);

												if (ERCDML_Task_Count_2 > 0)
												{
													int Count = 0; 
													int ercTaskFound=0;
													for (Count=0; Count<ERCDML_Task_Count_2;Count++)
														{											
															tag_t TaskRevTag_2  = NULLTAG;
															TaskRevTag_2  = ERC_Task_tag_2[Count];
															tag_t TaskTypeTag_2 = NULLTAG;
															char *Task_Type_2 = NULL;
															TCTYPE_ask_object_type(TaskRevTag_2,&TaskTypeTag_2);
															TCTYPE_ask_name2(TaskTypeTag_2,&Task_Type_2);
															printf("\n Task Type = %s",Task_Type_2);											

															if (tc_strcmp(Task_Type_2,"T5_ChangeTaskRevision")==0) // Check for ERC DML Task Revision
																{
																	ercTaskFound++;
																	GRM_find_relation_type("T5_DMLTaskRelation", &Task_DML_RelType_tag_2);
																	GRM_list_primary_objects_only(ERC_Task_tag_2[Count],Task_DML_RelType_tag_2,&ERCDML_Count_2,&ERCDML_tag_2);   //Finds primary object:DML(ERCDML) from Task
																	printf("\n ERCDML_Count_2 = %d",ERCDML_Count_2);
																	
																	if(ERCDML_Count_2 >0)
																		{
																			int Count1 = 0;
																			for (Count1=0; Count1<ERCDML_Count_2;Count1++)
																				{
																					tag_t DMLRevTag_2  = NULLTAG;
																					DMLRevTag_2  = ERCDML_tag_2[Count1];
																					tag_t DMLTypeTag_2 = NULLTAG;
																					char *DML_Type_2 = NULL;
																					tag_t     eff_t =NULLTAG;
																					TCTYPE_ask_object_type(DMLRevTag_2,&DMLTypeTag_2);
																					TCTYPE_ask_name2(DMLTypeTag_2,&DML_Type_2);
																					printf("\n DML Type = %s",DML_Type_2);
																					
																					if (tc_strcmp(DML_Type_2,"ChangeRequestRevision")==0)
																					{
																						int Count2 = 0;
																						Flag_ERCDML_2 = 0;
																						int ERCDML_Status_Count_2 = 0;
																						tag_t *ERCDML_Status_List_2 = NULLTAG;
																						char *ERCDML_Rlz_Status_2 = NULL;
																						char *ERCDML_ID_2 = NULL;
																						char *ERCDML_Rev_ID_2 = NULL;
																						char *ERC_Rlz_Date_2 = NULL;

																						AOM_ask_value_string(ERCDML_tag_2[Count1],"item_id",&ERCDML_ID_2);
																						AOM_ask_value_string(ERCDML_tag_2[Count1],"item_revision_id",&ERCDML_Rev_ID_2);
																						printf("\n ERCDML_ID_2 is : %s",ERCDML_ID_2);
																						printf("\n ERCDML_Rev_ID_2 is : %s",ERCDML_Rev_ID_2);
																					    																							
																						ITKCALL(WSOM_ask_release_status_list(ERCDML_tag_2[Count1],&ERCDML_Status_Count_2,&ERCDML_Status_List_2));
																						printf("\n ERCDML_Status_Count_2 : %d\n",ERCDML_Status_Count_2);
																						
																						if(ERCDML_Status_Count_2>0)
																						{
																							for(Count2=0; Count2<ERCDML_Status_Count_2;Count2++)
																							{
																								ERCDML_Rlz_Status_2 = NULL;
																								AOM_ask_name(ERCDML_Status_List_2[Count2], &ERCDML_Rlz_Status_2);
																								printf("\n ERC DML Release status is : %s",ERCDML_Rlz_Status_2);
																								if(tc_strcmp(ERCDML_Rlz_Status_2,"T5_LcsErcRlzd")==0)	
																								{
																									Flag_ERCDML_2++;
																									ITKCALL(AOM_UIF_ask_value(ERCDML_tag_2[Count1],"date_released",&ERC_Rlz_Date_2));
																									printf("\n ERC_Rlz_Date_2/ToDate : %s\n",ERC_Rlz_Date_2);fflush(stdout);
																									//Logic for ERC_Rlz_Date_2 = ToDate
																									ITKCALL(ITK_string_to_date(ERC_Rlz_Date_2, &ToDate ));																									
																									break;
																								}
																								
																								
																							}																								
																							printf("\n Flag_ERCDML_2 = %d \n",Flag_ERCDML_2);
																							
																						}
																						
																						break;
																					}

																				}
														
																		}	

																	break;	
																}
												
														}
														
												}

												if(Flag_ERCDML_2==0)
												{
													ITKCALL(ITK_string_to_date("31-Dec-9999 00:00", &ToDate ));
													printf("\n ToDate = 31-Dec-9999 00:00 \n");
												}
												
												break;
											}
										
										}
										
									}
									else
									{
										printf("\n In else of (Status_Count>0)");
										ITKCALL(ITK_string_to_date("31-Dec-9999 00:00", &ToDate ));
										printf("\n ToDate = 31-Dec-9999 00:00 \n");
									}

								}
								

							}
							else
							{
								printf("\n In else of (t_NextRevision != NULLTAG)");
								ITKCALL(ITK_string_to_date("31-Dec-9999 00:00", &ToDate ));
								printf("\n ToDate = 31-Dec-9999 00:00 \n");
							}

							start_end_date[1] = ToDate;

  							ITKCALL(WSOM_status_clear_effectivities (Part_Status_List[Count]));
							ITKCALL(WSOM_effectivity_create_with_dates(Part_Status_List[Count], NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &eff_d ));
							ITKCALL(AOM_save(eff_d));
							ITKCALL(AOM_save(Part_Status_List[Count]));
							ITKCALL(AOM_refresh(Part_Status_List[Count],0));
							printf("\n Effectivity Added...\n");
							//Effectivity End
						}
					
				}
	

}

int Add_RlzStat(tag_t Obj_tag, char *StatusToAdd,FILE *fp,FILE *fpNo_ERC_Rlz) //function to check release status on the object received.
{	
	tag_t class_id = NULLTAG;
	tag_t RlzStatLst_AttrID = NULLTAG;
	tag_t *RlzStatLst_Attr_tag = NULLTAG;
	char *class_name = NULL;
	char *Release_Status = NULL;
	char *Obj_ID = NULL;
	char *Obj_Type = NULL;
	logical	log1;
	logical	*is_it_null = NULL;
	logical	*is_it_empty = NULL;
	logical	retain_erc_released_date = false;
	int RlzStatLst_Length = 0;
	int count=0;
	int	ifail=0;
	tag_t *status_list1 = NULLTAG;
	int st_count1 = 0;
	char *ItemID = NULL;
    char *ItemRevID = NULL;

	tag_t Owning_Grp = NULLTAG;
    char *GroupID = NULL;
	tag_t Prt_Task_RelType_tag = NULLTAG;
    tag_t Task_DML_RelType_tag = NULLTAG;
	tag_t *ERC_Task_tag = NULLTAG;
	tag_t *ERCDML_tag = NULLTAG;
	int ERCDML_Task_Count = 0;
	int ERCDML_Count = 0;
	char *ERCDML_rlz_date = NULL;
	tag_t erc_release_status = NULLTAG;
	logical	Retain_Released_Date = 0;
	tag_t  APL_Rlz_Status = NULLTAG;
	char *APLRlzStatus = NULL;

	CALLAPI(POM_class_of_instance(Obj_tag,&class_id)); 
	CALLAPI(POM_name_of_class(class_id,&class_name));
	printf("\n Class Name is : %s\n",class_name);
	CALLAPI(POM_attr_id_of_attr("release_status_list",class_name,&RlzStatLst_AttrID));
	CALLAPI(WSOM_ask_release_status_list(Obj_tag,&st_count1,&status_list1));
	printf("\n st_count1 is : %d \n",st_count1);fflush(stdout);
	CALLAPI(POM_unload_instances(1,&Obj_tag));
	CALLAPI(POM_load_instances(1,&Obj_tag,class_id,1));
	CALLAPI(POM_is_loaded(Obj_tag,&log1));
	if(log1 == 1)
	{
		 printf(" Load Success\n " );
	}
	else
	printf(" Load Failure\n"  );
	CALLAPI( POM_length_of_attr(Obj_tag, RlzStatLst_AttrID, &RlzStatLst_Length) );
	CALLAPI( POM_ask_attr_tags(Obj_tag, RlzStatLst_AttrID, 0, RlzStatLst_Length, &RlzStatLst_Attr_tag, &is_it_null, &is_it_empty ));
    int statusfnd=0;

	AOM_ask_value_string(Obj_tag,"item_id",&ItemID);
	AOM_ask_value_string(Obj_tag,"item_revision_id",&ItemRevID);
	AOM_ask_value_string(Obj_tag,"object_type",&Obj_Type);

	for(count=0; count<RlzStatLst_Length;count++)
		{
			AOM_ask_name(RlzStatLst_Attr_tag[count], &Release_Status);
			printf("\n Release status is : %s\n",Release_Status);

			if((tc_strcmp(Release_Status,StatusToAdd)==0))	
				{
					statusfnd++; 
					break;
				}
		}
	printf("\n Flag statusfnd = %d \n",statusfnd);

	if(statusfnd==0)
		{	
			printf("\n In statusfnd==0 loop ....");
			printf("\n Object Type: %s, Item ID: %s, Item Revision ID: %s",Obj_Type,ItemID,ItemRevID);

			if(tc_strcmp(Obj_Type,"T5_APLDMLRevision")==0)
				{
					 printf("\n Printing DML info in File \n");
					 fprintf(fp,"APLDML Revision Count : [%s],[%s] (Adding APL Rlz Status on DML Revision)\n",ItemID,ItemRevID);
					 
				     	 //Adding APL Released Status on DML Revision
					 printf("\n Adding T5_LcsAplRlzd status... \n");
					 ITK_CALL(CR_create_release_status("T5_LcsAplRlzd",&APL_Rlz_Status));
					 ITK_CALL(AOM_ask_name(APL_Rlz_Status, &APLRlzStatus));
					 printf("\n Created %s status\n",APLRlzStatus);
					 ITK_CALL(EPM_add_release_status(APL_Rlz_Status,1,&Obj_tag,Retain_Released_Date));
					 printf("\n T5_LcsAplRlzd status added successfully \n");
					 
					 
				}

			if(tc_strcmp(Obj_Type,"T5_APLTaskRevision")==0)
				{					 
					 if((tc_strstr(ItemID,"_PR") == NULL))
					 {
						printf("\n Printing Task info in File \n");
						fprintf(fp,"APL Task Count : [%s],[%s] (Adding APL Rlz Status on Task)\n",ItemID,ItemRevID);
					 
						  //Adding APL Released Status on APL Task
						printf("\n Adding T5_LcsAplRlzd status... \n");
						ITK_CALL(CR_create_release_status("T5_LcsAplRlzd",&APL_Rlz_Status));
						ITK_CALL(AOM_ask_name(APL_Rlz_Status, &APLRlzStatus));
						printf("\n Created %s status\n",APLRlzStatus);
						ITK_CALL(EPM_add_release_status(APL_Rlz_Status,1,&Obj_tag,Retain_Released_Date));
						printf("\n T5_LcsAplRlzd status added successfully \n");
						
					 }

					 
				}

			if(tc_strcmp(Obj_Type,"Design Revision")==0)
				{        
					
					AOM_ask_group(Obj_tag,&Owning_Grp);
					AOM_ask_name(Owning_Grp,&GroupID);
					printf("\n Group ID is : %s",GroupID);

					if((tc_strstr(GroupID,"ERC")!=NULL))	
						{
							printf("\n Printing Part info in File \n");
							fprintf(fp,"Part Count : [%s],[%s], GroupID = ERC*, Part is not ERC Released \n ",ItemID,ItemRevID);
						}

					else if((tc_strstr(GroupID,"APL")!=NULL))	
						{
							printf("\n Printing Part info in File \n");
							fprintf(fp,"Part Count : [%s],[%s], GroupID =APL*, Part is not APL Released \n",ItemID,ItemRevID);						
						}
				
				}

 		}

	if(tc_strcmp(Obj_Type,"Design Revision")==0)
		{   
			printf("\n In ERC DMl Loop.... \n");
			int Count = 0;
			int Flag_ERC_Rlz = 0;
			int Flag_APL_Rlz = 0;
			int Flag_STD_Rlz = 0;
			int Part_Status_Count = 0;
			tag_t *Part_Status_List = NULLTAG;
			char *Part_Rlz_Status = NULL;
			char *PartID = NULL;
			char *PartRevID = NULL;
			char *ERC_ClosureDate = NULL;
			tag_t EffTxt_Tag = NULLTAG;
			logical stat;
			char *token = NULL;
			tag_t eff_d = NULLTAG;

			AOM_ask_value_string(Obj_tag,"item_id",&PartID);
			AOM_ask_value_string(Obj_tag,"item_revision_id",&PartRevID);
			CALLAPI(WSOM_ask_release_status_list(Obj_tag,&Part_Status_Count,&Part_Status_List));

			for(Count=0; Count<Part_Status_Count;Count++)
				{
					AOM_ask_name(Part_Status_List[Count], &Part_Rlz_Status);

					if((tc_strcmp(Part_Rlz_Status,"T5_LcsErcRlzd")==0))	
						{
							Flag_ERC_Rlz++;
							printf("\n Flag_ERC_Rlz = %d ",Flag_ERC_Rlz);
						}
					if((tc_strcmp(Part_Rlz_Status,"T5_LcsAplRlzd")==0))	
						{
							Flag_APL_Rlz++;
							printf("\n Flag_APL_Rlz = %d ",Flag_APL_Rlz);
						}
					if((tc_strcmp(Part_Rlz_Status,"T5_LcsStdRlzd")==0))	
						{
							Flag_STD_Rlz++;
							printf("\n Flag_STD_Rlz = %d ",Flag_STD_Rlz);
						}		
				}

			if(Flag_ERC_Rlz == 0)
				{
					printf("\n Part`s status != T5_LcsErcRlzd");
					if(Flag_APL_Rlz > 0 || Flag_STD_Rlz > 0)
						{
							printf("\n Part`s status = T5_LcsAplRlzd/T5_LcsStdRlzd");
							GRM_find_relation_type("CMHasSolutionItem", &Prt_Task_RelType_tag);
							GRM_list_primary_objects_only(Obj_tag,Prt_Task_RelType_tag,&ERCDML_Task_Count,&ERC_Task_tag); //Finds primary object:Task from Part
							printf("\n Task_Count = %d",ERCDML_Task_Count);

							if (ERCDML_Task_Count > 0)
								{
									int Count = 0; 
									int ercTaskFound=0;
									for (Count=0; Count<ERCDML_Task_Count;Count++)
										{										
											tag_t TaskRevTag  = NULLTAG;
											TaskRevTag  = ERC_Task_tag[Count];
											tag_t TaskTypeTag = NULLTAG;
											char *Task_Type = NULL;
											TCTYPE_ask_object_type(TaskRevTag,&TaskTypeTag);
											TCTYPE_ask_name2(TaskTypeTag,&Task_Type);
											printf("\n Task Type = %s",Task_Type);									
											
											if (tc_strcmp(Task_Type,"T5_ChangeTaskRevision")==0) // Check for ERC DML Task Revision
												{
													ercTaskFound++;
													GRM_find_relation_type("T5_DMLTaskRelation", &Task_DML_RelType_tag);
													GRM_list_primary_objects_only(ERC_Task_tag[Count],Task_DML_RelType_tag,&ERCDML_Count,&ERCDML_tag);   //Finds primary object:DML(ERCDML) from Task
													printf("\n ERCDML_Count = %d",ERCDML_Count);
													
													if(ERCDML_Count >0)
														{
															int Count1 = 0;
															for (Count1=0; Count1<ERCDML_Count;Count1++)
																{
																	tag_t DMLRevTag  = NULLTAG;
																	DMLRevTag  = ERCDML_tag[Count1];
																	tag_t DMLTypeTag = NULLTAG;
																	char *DML_Type = NULL;
																	tag_t     eff_t =NULLTAG;
																	TCTYPE_ask_object_type(DMLRevTag,&DMLTypeTag);
																	TCTYPE_ask_name2(DMLTypeTag,&DML_Type);
																	printf("\n DML Type = %s",DML_Type);
																	
																	if (tc_strcmp(DML_Type,"ChangeRequestRevision")==0)
																	{
																		int Count2 = 0;
																		int Flag_ERCDML = 0;
																		int ERCDML_Status_Count = 0;
																		tag_t *ERCDML_Status_List = NULLTAG;
																		char *ERCDML_Rlz_Status = NULL;
																		char *ERCDML_ID = NULL;
																		char *ERCDML_Rev_ID = NULL;
																		char *Nxt_Date_Str = (char *)malloc(100);
																		char *ERC_Rlz_Date = NULL;																		

																		AOM_ask_value_string(ERCDML_tag[Count1],"item_id",&ERCDML_ID);
																		AOM_ask_value_string(ERCDML_tag[Count1],"item_revision_id",&ERCDML_Rev_ID);																		
																		printf("\n ERCDML_ID is : %s",ERCDML_ID);
																		printf("\n ERCDML_Rev_ID is : %s",ERCDML_Rev_ID);																																		
																																				
																		CALLAPI(WSOM_ask_release_status_list(ERCDML_tag[Count1],&ERCDML_Status_Count,&ERCDML_Status_List));
																		printf("\n ERCDML_Status_Count : %d\n",ERCDML_Status_Count);
																		
																		if(ERCDML_Status_Count>0)
																			{
																				for(Count2=0; Count2<ERCDML_Status_Count;Count2++)
																					{
																						ERCDML_Rlz_Status = NULL;
																						AOM_ask_name(ERCDML_Status_List[Count2], &ERCDML_Rlz_Status);
																						printf("\n ERC DML Release status is : %s",ERCDML_Rlz_Status);
																						if(tc_strcmp(ERCDML_Rlz_Status,"T5_LcsErcRlzd")==0)	
																							{
																								Flag_ERCDML++;
																								ITKCALL(AOM_UIF_ask_value(ERCDML_tag[Count1],"date_released",&ERC_Rlz_Date));
																								printf("\n ERC_Rlz_Date(ERC DML Rev) : %s\n",ERC_Rlz_Date);fflush(stdout);																								
																								//Logic for ERC_Rlz_Date + 1 = FromDate
																								get_NextDate(ERC_Rlz_Date,Nxt_Date_Str);
																								break;
																							}																						
																					}																				
																				printf("\n Flag_ERCDML = %d \n",Flag_ERCDML);
																				if(Flag_ERCDML>0)
																				{
																					tag_t tag_query = NULLTAG;
																					tag_t *PartRev_list = NULLTAG;
																					int n_entries = 3;
																					int num_found = 0;
																					int ii =0;
																					int jj =0;
																					tag_t *Status_List = NULLTAG;
																					int Status_Count =0;
																					char *ERC_Rlz_Date = NULL;
																					char *ERC_Rlz_Date_2 = NULL;
																					char *Rlz_Status = NULL;
																					tag_t t_NextRevision = NULLTAG;
																					int Flag_ErcRlzd_3 = 0;
																					date_t FromDate;
																					date_t ToDate;
																					date_t *start_end_date = NULL;
																					date_t NextDate;
																					char *ToDate_Str = NULL;
																					char *FromDate_Str = NULL;
																					char *value = NULL;
																					char *PartRevID_1 = NULL;
																					int Flag_ERCDML_3 = 0;
																					
																					logical date_is_valid_l;

																					printf("\n Part Count : [%s],[%s]: T5_LcsErcRlzd status found on ERCDML but part is not ERC Released \n",PartID,PartRevID);
																					fprintf(fpNo_ERC_Rlz,"Part Count : [%s],[%s]: T5_LcsErcRlzd status found on ERCDML but part is not ERC Released (Adding ERC Rlz Status and Effectivity on Part) \n",PartID,PartRevID);
																					
																					CALLAPI(CR_create_release_status("T5_LcsErcRlzd",&erc_release_status));
																					CALLAPI(EPM_add_release_status(erc_release_status,1,&Obj_tag,retain_erc_released_date));
																					printf("\n T5_LcsErcRlzd status added successfully \n");																			
																				
																					//Effectivity Start
																						
																					CALLAPI(WSOM_ask_effectivity_mode(&stat));
																					CALLAPI(PROP_ask_property_by_name(erc_release_status,"effectivity_text",&EffTxt_Tag));
																					CALLAPI(PROP_ask_value_string(EffTxt_Tag,&value));
																					ITKCALL(AOM_UIF_ask_value(erc_release_status,"date_released",&ERC_Rlz_Date));
																					printf("\n ERC_Rlz_Date(Part/Design Rev) : %s\n",ERC_Rlz_Date);fflush(stdout);																				
																																																												
																					//FromDate = Nxt_Date_Str
																					ITKCALL(ITK_string_to_date(Nxt_Date_Str, &FromDate));																					
																					
																					start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);
																					start_end_date[0] = FromDate;
																					//start_end_date[1] = ToDate;																					
																			
																					printf("\n Before tm_fnd_Next_revision\n");fflush(stdout);
																					// Check next revision release status for ToDate
																					tm_fnd_Next_revision(PartID,Obj_tag,&t_NextRevision);
																					printf("\n After tm_fnd_Next_revision\n");fflush(stdout);
																					if (t_NextRevision != NULLTAG)
																					{
																						AOM_ask_value_string(t_NextRevision,"item_revision_id",&PartRevID_1);
																						printf("Next revision of Part found : [%s],[%s] ",PartID,PartRevID_1);
																						if (strcmp(PartRevID,PartRevID_1)==0)
																						{
																							printf("\n In PartRevID = PartRevID_1");
																							ITKCALL(ITK_string_to_date("31-Dec-9999 00:00", &ToDate ));
																							printf("\n ToDate = 31-Dec-9999 00:00 \n");
																						}
																						else
																						{
																							CALLAPI(WSOM_ask_release_status_list(t_NextRevision,&Status_Count,&Status_List));
																							printf("\n Status_Count = %d",Status_Count);fflush(stdout);
																							if (Status_Count>0)
																							{
																								for(jj=0; jj<Status_Count;jj++)
																								{
																									AOM_ask_name(Status_List[jj], &Rlz_Status);
																									if((tc_strcmp(Rlz_Status,"T5_LcsErcRlzd")==0))	
																									{ 																												
																										Flag_ErcRlzd_3 ++;
																										int ERCDML_Task_Count_3 =0;								
																										tag_t Prt_Task_RelType_tag_3 = NULLTAG;
																										tag_t Task_DML_RelType_tag_3 = NULLTAG;
																										int ERCDML_Count_3 = 0;																										
																										tag_t *ERC_Task_tag_3 = NULLTAG;
																										tag_t *ERCDML_tag_3 = NULLTAG;
																										printf("\n T5_LcsErcRlzd status found on Next Part Revision : [%s],[%s] ",PartID,PartRevID_1);
																										
																										//Logic for next revision`s -->ERC DML`s-->date_released
																										GRM_find_relation_type("CMHasSolutionItem", &Prt_Task_RelType_tag_3);
																										GRM_list_primary_objects_only(t_NextRevision,Prt_Task_RelType_tag_3,&ERCDML_Task_Count_3,&ERC_Task_tag_3); //Finds primary object:Task from Part
																										printf("\n Task_Count = %d",ERCDML_Task_Count_3);

																										if (ERCDML_Task_Count_3 > 0)
																										{
																											int Count = 0; 
																											int ercTaskFound=0;
																											for (Count=0; Count<ERCDML_Task_Count_3;Count++)
																												{											
																													tag_t TaskRevTag_3  = NULLTAG;
																													TaskRevTag_3  = ERC_Task_tag_3[Count];
																													tag_t TaskTypeTag_3 = NULLTAG;
																													char *Task_Type_3 = NULL;
																													TCTYPE_ask_object_type(TaskRevTag_3,&TaskTypeTag_3);
																													TCTYPE_ask_name2(TaskTypeTag_3,&Task_Type_3);
																													printf("\n Task Type = %s",Task_Type_3);											

																													if (tc_strcmp(Task_Type_3,"T5_ChangeTaskRevision")==0) // Check for ERC DML Task Revision
																														{
																															ercTaskFound++;
																															GRM_find_relation_type("T5_DMLTaskRelation", &Task_DML_RelType_tag_3);
																															GRM_list_primary_objects_only(ERC_Task_tag_3[Count],Task_DML_RelType_tag_3,&ERCDML_Count_3,&ERCDML_tag_3);   //Finds primary object:DML(ERCDML) from Task
																															printf("\n ERCDML_Count_3 = %d",ERCDML_Count_3);
																															
																															if(ERCDML_Count_3 >0)
																																{
																																	int Count1 = 0;
																																	for (Count1=0; Count1<ERCDML_Count_3;Count1++)
																																		{
																																			tag_t DMLRevTag_3  = NULLTAG;
																																			DMLRevTag_3  = ERCDML_tag_3[Count1];
																																			tag_t DMLTypeTag_3 = NULLTAG;
																																			char *DML_Type_3 = NULL;
																																			tag_t     eff_t =NULLTAG;
																																			TCTYPE_ask_object_type(DMLRevTag_3,&DMLTypeTag_3);
																																			TCTYPE_ask_name2(DMLTypeTag_3,&DML_Type_3);
																																			printf("\n DML Type = %s",DML_Type_3);
																																			
																																			if (tc_strcmp(DML_Type_3,"ChangeRequestRevision")==0)
																																			{
																																				int Count2 = 0;
																																				Flag_ERCDML_3 = 0;
																																				int ERCDML_Status_Count_3 = 0;
																																				tag_t *ERCDML_Status_List_3 = NULLTAG;
																																				char *ERCDML_Rlz_Status_3 = NULL;
																																				char *ERCDML_ID_3 = NULL;
																																				char *ERCDML_Rev_ID_3 = NULL;
																																				char *ERC_Rlz_Date_3 = NULL;

																																				AOM_ask_value_string(ERCDML_tag_3[Count1],"item_id",&ERCDML_ID_3);
																																				AOM_ask_value_string(ERCDML_tag_3[Count1],"item_revision_id",&ERCDML_Rev_ID_3);
																																				printf("\n ERCDML_ID_3 is : %s",ERCDML_ID_3);
																																				printf("\n ERCDML_Rev_ID_3 is : %s",ERCDML_Rev_ID_3);
																																			    																							
																																				ITKCALL(WSOM_ask_release_status_list(ERCDML_tag_3[Count1],&ERCDML_Status_Count_3,&ERCDML_Status_List_3));
																																				printf("\n ERCDML_Status_Count_3 : %d\n",ERCDML_Status_Count_3);
																																				
																																				if(ERCDML_Status_Count_3>0)
																																				{
																																					for(Count2=0; Count2<ERCDML_Status_Count_3;Count2++)
																																					{
																																						ERCDML_Rlz_Status_3 = NULL;
																																						AOM_ask_name(ERCDML_Status_List_3[Count2], &ERCDML_Rlz_Status_3);
																																						printf("\n ERC DML Release status is : %s",ERCDML_Rlz_Status_3);
																																						if(tc_strcmp(ERCDML_Rlz_Status_3,"T5_LcsErcRlzd")==0)	
																																						{
																																							Flag_ERCDML_3++;
																																							ITKCALL(AOM_UIF_ask_value(ERCDML_tag_3[Count1],"date_released",&ERC_Rlz_Date_3));
																																							printf("\n ERC_Rlz_Date_3/ToDate : %s\n",ERC_Rlz_Date_3);fflush(stdout);
																																							//Logic for ERC_Rlz_Date_3 = ToDate
																																							ITKCALL(ITK_string_to_date(ERC_Rlz_Date_3, &ToDate ));																																							
																																							break;
																																						}
																																						
																																					}																								
																																					printf("\n Flag_ERCDML_3 = %d \n",Flag_ERCDML_3);
																																					
																																				}

																																				break;
																																			}

																																		}
																												
																																}	

																															break;
																														}
																													
																												}
																												
																										}

																										if(Flag_ERCDML_3==0)
																										{
																											ITKCALL(ITK_string_to_date("31-Dec-9999 00:00", &ToDate ));
																											printf("\n ToDate = 31-Dec-9999 00:00 \n");
																										}																																					
																										break;
																									}																									
																								}																								
																							}

																						}																					
																					}

																					start_end_date[1] = ToDate;															
																															
																					ITKCALL(WSOM_status_clear_effectivities (erc_release_status));
																					ITKCALL(WSOM_effectivity_create_with_dates(erc_release_status, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &eff_d ));
																					ITKCALL(AOM_save(eff_d));
																					ITKCALL(AOM_save(erc_release_status));
																					ITKCALL(AOM_refresh(erc_release_status,0));
																					printf("\n Effectivity Added...\n");																		

																					//Effectivity End																					
																					break;
																				
																				}
																				else
																				{																				
																					printf("\n Part Count : [%s],[%s]: T5_LcsErcRlzd status Not found on ERCDML and part is not ERC Released \n",PartID,PartRevID);
																					fprintf(fpNo_ERC_Rlz,"Part Count : [%s],[%s]: T5_LcsErcRlzd status Not found on ERCDML and part is not ERC Released \n",PartID,PartRevID);
																					break;
																				}
																			}
																		else
																			{
																				printf("\n No release status found on ERC DML \n");
																			}

																	}

																}
										
														}	
													else
														{
															printf("\n ERCDML not found for this part \n");
															fprintf(fpNo_ERC_Rlz,"Part Count : [%s],[%s]: ERCDML not found for this part \n",PartID,PartRevID);
														}
													break;
												}												
											
										}
										if(ercTaskFound==0)
										{
											fprintf(fpNo_ERC_Rlz,"Part Count : [%s],[%s]: T5_LcsErcRlzd status not found on part and ERC DML not present.. \n",PartID,PartRevID);																					
										
										}
								}							
						}
					else if(Flag_APL_Rlz == 0 && Flag_STD_Rlz == 0)
						{
							printf("\n Part`s status != T5_LcsErcRlzd/T5_LcsAplRlzd/T5_LcsStdRlzd  \n");
							fprintf(fpNo_ERC_Rlz,"Part Count : [%s],[%s]: Part`s status != T5_LcsErcRlzd/T5_LcsAplRlzd/T5_LcsStdRlzd \n",PartID,PartRevID);
						}
				}
			else
				{
					printf("\n Part`s status = T5_LcsErcRlzd  \n");
					//printf("Adding Effectivity...");
					//AddEffectivity(Obj_tag);
				}
		}
	return ITK_ok;
}

int ITK_user_main(int argc,char* argv[]) // Main Function
{
   char *GroupID = NULL;
   char *message;
   char *loggedInUser = NULL;
   char *DMLStatus = NULL;
   char *TaskStatus = NULL;
   char *PartStatus = NULL;
   char *Obj_ID = NULL;
   char *Obj_Rev_ID = NULL;
   char * item_id_str = NULL;
   char *ERC_Rlz_Date = NULL;
   FILE *fpDML = NULL;
   FILE *fpTask = NULL;
   FILE *fpPart = NULL;
   FILE *fpError = NULL;
   FILE *fpDML_List = NULL;
   FILE *fpNo_ERC_Rlz = NULL;
   //FILE *fpAddEff_List = NULL;
   //FILE *fpNo_DML = NULL;
   int i=0;
   int j=0;
   int k=0;	
   int status = 0;
   int dml_count = 0;
   int DML_Count = 0;
   int task_count = 0; 
   int part_count = 0;
   int n_entries = 3;
   int n_found = 0;
   int	ifail=0;

   tag_t tag_query = NULLTAG;
   tag_t relation_type_tag1 = NULLTAG;
   tag_t relation_type_tag2 = NULLTAG;
   tag_t *dml_list = NULLTAG;
   tag_t *task_list = NULLTAG;
   tag_t *part_list = NULLTAG;
   tag_t *DML_Rev_tag;
   tag_t DMLTag = NULLTAG;
   tag_t TaskTag = NULLTAG;
   tag_t PartTag = NULLTAG;
   tag_t Owning_Grp = NULLTAG;
   char *DML_List_File = NULL;
   char* DML_item_id = NULL;
   const char *attrs[1];
   const char *values[1];
   char *ReturnNxtDt_1 = (char *)malloc(100);
   int DML_Rev_Count = 0;
   item_id_str = NULL;
   char *PartType = NULL;
   tag_t PartType_tag = NULLTAG;
			
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

		fpDML=fopen("DML_Count_Log.txt","w");
		fpTask=fopen("Task_Count_Log.txt","w");
		fpPart=fopen("Part_Count_Log.txt","w");
		fpError=fopen("Error_Log.txt","w");
		fpDML_List=fopen("DML_List_File.txt","r");
		fpNo_ERC_Rlz=fopen("Part_Status_Log.txt","w");
		//fpAddEff_List=fopen("AddEff_List.txt","r");
		//fpNo_DML=fopen("Part_With_No_DML_Log.txt","w");

		if(fpDML_List != NULL)
			{	
				printf("\n DML_List != NULL\n");fflush(stdout);
				DML_item_id=(char *) MEM_alloc(100);
					
				
				 while(fgets(DML_item_id,100,fpDML_List)!=NULL)
					{
						item_id_str=strtok(DML_item_id,"^");
						printf("\n DML_item_id = %s \n",item_id_str);
						attrs[0] ="item_id";
			            values[0] = (char *)item_id_str;
						ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs,values, &dml_count, &dml_list));
					
						for(i=0; i<dml_count; i++)  //Start of dml_count loop
						{
						
                            AOM_ask_value_tags(dml_list[i],"revision_list",&DML_Rev_Count, &DML_Rev_tag);
                            printf("\n DML_Rev_Count : %d\n", DML_Rev_Count);fflush(stdout);

							if(DML_Rev_tag[0]==NULLTAG)
								{
									printf("\n DML_Rev_tag is NULLTAG \n");fflush(stdout);
								}
							else
								{
									printf("\n DML_Rev_tag is NOT a NULLTAG \n");fflush(stdout);
								}

							ITK_CALL(AOM_ask_value_string(DML_Rev_tag[0],"item_id",&Obj_ID));		//to get object id
							printf("\n DML ID is : %s",Obj_ID);
							ITK_CALL(AOM_ask_value_string(DML_Rev_tag[0],"item_revision_id",&Obj_Rev_ID));		//to get object rev id
							printf("\n DML Revision ID is : %s",Obj_Rev_ID);

							Add_RlzStat(DML_Rev_tag[0],"T5_LcsAplRlzd",fpDML,fpNo_ERC_Rlz);  
					
							//Search Related Tasks to DML
							GRM_find_relation_type("T5_DMLTaskRelation",&relation_type_tag1); // Display name of relation T5_DMLTaskRelation = Breaks Down Into Plan Items
							GRM_list_secondary_objects_only	(DML_Rev_tag[0], relation_type_tag1, &task_count,&task_list); //No of Tasks found
							printf("\n task_count : %d\n", task_count);fflush(stdout);
						
							j=0;
								for(j=0; j<task_count; j++)  //Start of task_count loop
									{

										Obj_ID=NULL;

										part_list=NULLTAG;
										relation_type_tag2=NULLTAG;
										Obj_Rev_ID=NULL;

										printf("\n In task_count loop\n");fflush(stdout);
										TaskTag=task_list[j];

										ITK_CALL(AOM_ask_value_string(TaskTag,"item_id",&Obj_ID));		//to get object id
										printf("\n Task ID is : %s",Obj_ID);
										ITK_CALL(AOM_ask_value_string(TaskTag,"item_revision_id",&Obj_Rev_ID));		//to get object rev id
										printf("\n Task Revision ID is : %s",Obj_Rev_ID);

										Add_RlzStat(TaskTag,"T5_LcsAplRlzd",fpTask,fpNo_ERC_Rlz);  
							
										//Search Related Parts to Task
										GRM_find_relation_type("CMHasSolutionItem",&relation_type_tag2);  // Display name of relation CMHasSolutionItem = Solution Items
										GRM_list_secondary_objects_only(TaskTag, relation_type_tag2, &part_count,&part_list); //No of Parts found
										printf("\n part_count : %d\n", part_count);fflush(stdout);
										if (part_count > 0)
										{											
											k=0;
											for(k=0; k<part_count; k++) //Start of part_count loop
												{	
													Obj_ID = NULL;
													PartType = NULL;
													PartTag = NULLTAG;
													PartType_tag = NULLTAG;

													PartTag=part_list[k];

													if (PartTag == NULLTAG)
													{
														printf("\n PartTag = NULLTAG\n");
													}
													else
													{
														printf("\n PartTag != NULLTAG\n");
														TCTYPE_ask_object_type(PartTag,&PartType_tag);fflush(stdout);
														TCTYPE_ask_name2(PartType_tag,&PartType);
														printf("\n PartType %d : %s\n ",k,PartType);fflush(stdout);

														if(PartType==NULL)
														{
														  printf("\n PartType = NULL\n");
														  continue;
														}
													}
													
													ITK_CALL(AOM_ask_value_string(part_list[k],"item_id",&Obj_ID));fflush(stdout);

													printf("\n Part ID is : %s\n",Obj_ID);

													if(tc_strcmp(Obj_ID,"544588500107")==0)
													{
														continue;
													}

													ITK_CALL(AOM_ask_value_string(PartTag,"item_revision_id",&Obj_Rev_ID));		//to get object rev id
													printf("\n Part Revision ID is : %s",Obj_Rev_ID);
													ITKCALL(AOM_UIF_ask_value(PartTag,"owning_group",&GroupID));		
													printf("\n Group ID is : %s",GroupID);

													if((tc_strstr(GroupID,"ERC")!=NULL))	
														{
															printf("\n Owning Group is ERC: Checking ERC Released Status");
															Add_RlzStat(PartTag,"T5_LcsErcRlzd",fpPart,fpNo_ERC_Rlz);															
														}

													else if((tc_strstr(GroupID,"APL")!=NULL))	
														{
															printf("\n Owning Group is APL: Checking APL Released Status");
															//Add_RlzStat(PartTag,"T5_LcsAplRlzd",fpPart,fpNo_ERC_Rlz);  							
														}
													else
														{
															printf("\n Owning Group ID does not belong to APL*/ERC*");
															fprintf(fpError,"Part Count : [%s],[%s], Owning Group ID does not belong to APL*/ERC*\n",Obj_ID,Obj_Rev_ID);
														}
												}  //End of part_count loop
											printf("\n End of part_count loop\n");
										}  
										
									}  //End of task_count loop
					    } //End of dml_count loop
					} //End of while loop

					fclose(fpDML_List) ;           
					printf("\n Data successfully read from file DML_List_File.txt\n"); 
					printf("\n The file is now closed.\n"); 
			} //End of DML_List condition
				
		else 
			{
				printf("\n DML_List = NULL");fflush(stdout);
			}
		
			 
    CLEANUP:
	ITK_CALL(ITK_exit_module(TRUE));	
	return ITK_ok;
}

// compile Check_And_Add_DML_Status.c
// linkitk -o Check_And_Add_DML_Status Check_And_Add_DML_Status.o
// Check_And_Add_DML_Status -u=aplloader -pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba > Result.log &
// Check_And_Add_DML_Status -u=infodba -p=infodba -g=dba
// scp tcuaadev@172.22.97.90:/user/tcuaadev/devgroups/sagar/Check_And_Add_DML_Status/Check_And_Add_DML_Status . 