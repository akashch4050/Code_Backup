/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File			 :   Check_APLDML_Status.c
*  Author		 :   Sagar Baviskar
*  Module		 :   Checks the status of APLDML and associated Tasks/Parts and  removes status if required.
*                            
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes

***************************************************************************/
#include <stdio.h>
#include<tc/tc.h>
#include <string.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tcinit/tcinit.h>
#include <tccore/aom.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <itk/mem.h>
#include <stdlib.h>
#include<tccore/grm.h>
#include <tcinit/tcinit.h>
#include <tc/emh.h>

#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) {  return ifail;}
#define ITK_errStore 91900002
#define ITK_CALL 	 


int RemoveRlzStat(tag_t Obj_tag, char* LCSToRemove)//function to remove release status of passed object
{
	tag_t class_id = NULLTAG;
	tag_t RlzStatLst_AttrID = NULLTAG;
	tag_t *RlzStatLst_Attr_tag;
	char *class_name = NULL;
	char *Release_Status = NULL;
	char *Obj_ID =NULL;
	logical	log1;
	logical	*is_it_null = NULL;
	logical	*is_it_empty = NULL;
	//int RlzStatLst_AttrID = 0;
	int RlzStatLst_Length = 0;
	int count=0;
	int	ifail=0;

		tag_t*    status_list1=NULLTAG;
	int              st_count1 = 0;

	CALLAPI(POM_class_of_instance(Obj_tag,&class_id)); 
	CALLAPI(POM_name_of_class(class_id,&class_name));
	printf("\n Class Name is :%s\n",class_name);

	CALLAPI(POM_attr_id_of_attr("release_status_list",class_name,&RlzStatLst_AttrID));
	CALLAPI(WSOM_ask_release_status_list(Obj_tag,&st_count1,&status_list1));
	printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);

	CALLAPI(POM_unload_instances(1,&Obj_tag));
	CALLAPI(POM_load_instances(1,&Obj_tag,class_id,1));
	CALLAPI(POM_is_loaded(Obj_tag,&log1));
	if(log1 == 1)
	{
		 printf(" Load Success\n " );
	}
	else
	printf("Load Failure\n"  );


/*
	CALLAPI(POM_class_of_instance(object_tag,&class_id));
	CALLAPI(POM_name_of_class(class_id,&class_name));
	printf("\n class_name is :%s\n",class_name);fflush(stdout);

	CALLAPI(POM_attr_id_of_attr("release_status_list",class_name,&tReleaseStatusList_checkin));fflush(stdout);
	CALLAPI(WSOM_ask_release_status_list(object_tag,&st_count1,&status_list1));
	printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);


	CALLAPI(POM_unload_instances (1,&object_tag));
	CALLAPI(POM_load_instances(1,&object_tag,class_id,1));
	CALLAPI(POM_is_loaded(object_tag,&log1));
	if(log1 == 1)
	{
		 printf(" Load Success\n " );
	}
	else
	printf("Load Failure\n"  );

*/
	CALLAPI( POM_length_of_attr(Obj_tag, RlzStatLst_AttrID, &RlzStatLst_Length) );
	CALLAPI( POM_ask_attr_tags(Obj_tag, RlzStatLst_AttrID, 0, RlzStatLst_Length, &RlzStatLst_Attr_tag, &is_it_null, &is_it_empty ));
	int flag = 0;
	for(count=0; count<RlzStatLst_Length;count++)
	{
		AOM_ask_value_string(Obj_tag,"item_id",&Obj_ID);		//to get object id
		printf("\n Object ID is :%s\n",Obj_ID);
		AOM_ask_name(RlzStatLst_Attr_tag[count], &Release_Status);
		printf("\n Release status is :%s\n",Release_Status);

		if( (tc_strcmp(Release_Status,LCSToRemove)==0))	
		{
			printf("\n Release status is :%s\n",Release_Status);
			
			CALLAPI(POM_remove_from_attr(1,&Obj_tag,RlzStatLst_AttrID,flag,1));  //Removes status from object
			CALLAPI(POM_save_instances(1,&Obj_tag,1));
			CALLAPI(POM_refresh_instances(1,&Obj_tag,class_id,2));
			CALLAPI(AOM_lock(Obj_tag));
			CALLAPI(AOM_save(Obj_tag));
			CALLAPI(AOM_refresh(Obj_tag,1));
			CALLAPI(AOM_unlock(Obj_tag));
			printf("\n Release Status Removed Successfully :%s\n");

		}
		flag++;
	}
	MEM_free(RlzStatLst_Attr_tag);
	return ITK_ok;
}


int ITK_user_main(int argc,char* argv[]) // Main Function
{
   char *itemID = NULL;
   char *LCSToRemove = NULL;
   char *message;
   char *loggedInUser = NULL;
   char *DMLStatus = NULL;
   char *TaskStatus = NULL;
   char *PartStatus = NULL;
  
		
   int status = 0;
   int num_found = 0;
   int task_count = 0; 
   int part_count = 0;
   int n_entries = 2;
   int n_found = 0;
   int	ifail=0;

   tag_t tag_query = NULLTAG;
   tag_t relation_type_tag1 = NULLTAG;
   tag_t relation_type_tag2= NULLTAG;
   tag_t *DML_tag;
   tag_t *task_list;
   tag_t *part_list;
   tag_t DMLTag= NULLTAG;
   tag_t TaskTag= NULLTAG;
   tag_t PartTag= NULLTAG;
      
  // printf("Enter Item ID:\n");
   itemID=ITK_ask_cli_argument("-itemID=");  
  // printf("Enter LCS to remove:\n");
   LCSToRemove=ITK_ask_cli_argument("-LCSToRemove=");  

//Checks if Item Id and LCS to remove is entered by user or not.
    if (itemID!=NULL && LCSToRemove!=NULL ) 
	{
			char *qry_entries[2] = {"Item ID","Type"};
			char *qry_values[2] = {itemID,"T5_APLDMLRevision"};
			
               ITK_initialize_text_services (0);
			   //Logging in to TC
               status = ITK_auto_login ();
               if (status != ITK_ok ) 
			   {
                       EMH_ask_error_text(status, &message);
                       printf("Error with ITK_auto_login: \"%d\", \"%s\"\n", status, message);
                       MEM_free(message);
                       return status;
               }else
			   {
				   printf("Auto login Successfully\n");
				   POM_get_user_id(&loggedInUser);
				   printf("Logged in User is :%s\n",loggedInUser);
			   }

               ITK_set_journalling (TRUE);

               //Find a query using "query name" and store it into a tag:"tag_query"
               CALLAPI(QRY_find("Item Revision...",&tag_query));
               printf("Searching...\n");

               //Execute the query by passing above tag:"tag_query" and store the result into tag:"DML_tag" 
               CALLAPI(QRY_execute(tag_query,n_entries, qry_entries, qry_values,&num_found,&DML_tag));

               printf("\nArgument passed in query is: %s\n",itemID);
			   printf("\nGiven LCS To Remove is :%s\n",LCSToRemove);
			   printf("\nNumber of objects(DML) found=%d\n",num_found);
               
			   if(num_found>0)
			    {	
					int i=0;
					int j=0;

					DMLTag=DML_tag[0];
					RemoveRlzStat(DMLTag,LCSToRemove);  // Checks the status of passed object and removes status if condition is matched.
					
						//Search Related Tasks to DML
						GRM_find_relation_type("T5_DMLTaskRelation",&relation_type_tag1); // Display name of relation T5_DMLTaskRelation = Breaks Down Into Plan Items
						GRM_list_secondary_objects_only	(DMLTag, relation_type_tag1, &task_count,&task_list); //No of Tasks found
						
						i=0;
						for(i=0; i<task_count; i++)
						{
							TaskTag=task_list[i];
							RemoveRlzStat(TaskTag,LCSToRemove);  // Checks the status of passed object and removes status if condition is matched.
							
							//Search Related Parts to Task
							GRM_find_relation_type("CMHasSolutionItem",&relation_type_tag2);  // Display name of relation CMHasSolutionItem = Solution Items
							GRM_list_secondary_objects_only(TaskTag, relation_type_tag2, &part_count,&part_list); //No of Parts found
							
							j=0;
							for(j=0; j<part_count; j++)
							{
                                PartTag=part_list[j];
								RemoveRlzStat(PartTag,LCSToRemove);  // Checks the status of passed object and removes status if condition is matched.
							}  //End of part_count loop
						}  //End of task_count loop
				}
				
			    else
			    {
				    printf("\nNo object found\n");
			    }
			   
               MEM_free(DML_tag);
			   MEM_free(task_list);
			   MEM_free(part_list);
	} 
	else 
	{
               printf("Enter both Item ID to search and LCS to remove:\n"
                       "\nExample: -itemID=19AM901001_APLC \n"
					   "\nExample: -LCSToRemove=STDSIC Working\n");
    }
	
    CLEANUP:
	ITK_CALL(ITK_exit_module(TRUE));	
return ITK_ok;
}

// compile Check_APLDML_Status.c
// linkitk -o Check_APLDML_Status Check_APLDML_Status.o
//Check_APLDML_Status -u=aplloader -pf=/user/uaprod/shells/Admin/aplpasswdfile.pwf -g=dba -itemID=17PP057108_APLC -LCSToRemove=T5_LcsSTDWrkg