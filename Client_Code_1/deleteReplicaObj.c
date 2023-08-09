/**********************************************************************************************************************************************************
** Functions:- This utility takes item_id and attribute_value as input from file and updates the value of specific attribute of latest revision of perticular 

** command to run:
** update_attr -u=<username> -p=<password> -g=<group> -file=<Filename>
***********************************************************************************************************************************************************/


#include <time.h>
#include <stdio.h>
#include <tc/tc.h>
#include <tc/emh.h>
#include <bom/bom.h>
#include <tc/folder.h>
#include <tccore/aom.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <tc/tc_macros.h>
#include <bom/bom_attr.h>
#include <tc/tc_startup.h> 
#include <tcinit/tcinit.h>
#include <tc/preferences.h>
#include <tccore/aom_prop.h>
#include <fclasses/tc_string.h>
#include <tccore/workspaceobject.h>


#define CHECK_FAIL if (ifail != 0) { printf("line %d (ifail %d)\n", __LINE__, ifail); return 0;}


#define PROP_UIF_set_value_msg		"PROP_UIF_set_value" 
#define PROP_set_value_tag_msg		"PROP_set_value_tag" 
#define PROP_set_value_string_msg   "PROP_set_value_string"  

FILE 		*output 			= NULL;

char		*sep				= ",";

int read_value_from_file(char*);
int update_attr(char*, char*);
int write_info(char*, char*, char*);

void print_requirement()
{
	printf(
        "\n all the following parameters are mandatory for procedure"
        " USAGE:\n"
        " -u=<teamcenter dbUsr id> (required)\n"
        " -p=<teamcenter password> (required)\n"
        " -g=<teamcenter group> (required)\n"
        " -item_id=<filename> (required)\n"
        " -rev=<filename> (required)\n"
        );	
}

/*******************************************************************************
    Function:       ITK_user_main
    Description:    This is ITK main function. The program starts from here. 
					It checks for the proper dbUsr name and password.	
*******************************************************************************/
int ITK_user_main(int argc, char *argv[])
{
	int				ifail 						= ITK_ok;
	char			* userid 					= NULL;
	char			* password					= NULL;
	char			* group						= NULL;
	char			* item_id					= NULL;
	char			* rev						= NULL;
	char			Data[100]					= "";
	
	char 			*test						= (char*)MEM_alloc(sizeof(char) * 50);
	char			outputFile[200]				= "";
	
	userid			= ITK_ask_cli_argument("-u=");
	password 		= ITK_ask_cli_argument("-pf=");
	group 			= ITK_ask_cli_argument("-g=");
	item_id 		= ITK_ask_cli_argument("-item_id=");
	rev				= ITK_ask_cli_argument("-rev=");
	
	time_t 	now;
	struct 	tm when;
	
	time(&now);
	when = *localtime(&now);
	
	strftime(Data,100,"%d_%b_%Y_%H_%M_%S",&when);
	sprintf(outputFile,"%s_output.csv",Data);
	
	
//	if(tc_strlen(stripBlanks(userid)) == 0  || tc_strlen(stripBlanks(password)) == 0  || tc_strlen(stripBlanks(group)) == 0)
//	{
//		print_requirement();
//		return -1;
//	}
	
	
	ifail = ITK_init_module(userid, password, group);
	ifail =	 (ITK_auto_login( ));
	ifail =(ITK_set_journalling( TRUE ));
	if (ifail != ITK_ok)
	{
		printf("Error in Login to Teamcenter\n");
		return ifail;
	}
	else
		{
			printf("\nLogin Successfull.....!!\n");
			printf("\nWelcome to Teamcenter.....!!\n");
		}
//	ifail = ITK_set_bypass(true);
//    if (ifail != ITK_ok)
//	{
//		printf("\nUser is not Privileged Admin User \n\n");
//		return -1;
//	}
	
	ifail = update_attr(item_id,rev);
		
	printf("\nEnd of program.....!!\n\n");
	printf("\n*****************************\n");
	ifail = ITK_exit_module(true);
	return ifail;
}

/********************************************************************************************
    Function:       update_attr
    Description:    This function takes the item_id and attr_value as input and stamps the 
					attr_value on specific attribute of all revisions of perticular item.
********************************************************************************************/

int update_attr(char* ID, char* RevID)
{
	int				ifail 						= ITK_ok;
	int				r_count 					= 0;
	int				count 						= 0;
	int				countR	 					= 0;
	int				i		 					= 0;
	int				revloop	 					= 0;
	int				revloopR 					= 0;
		
	tag_t			item_tag 					= NULLTAG;
	tag_t			rev_tag						= NULLTAG;
	tag_t			relation_type				= NULLTAG;
	tag_t			relation_typeR				= NULLTAG;
	tag_t			*secondary_objects			= NULLTAG;
	tag_t			*secondary_objectsR			= NULLTAG;
	
	char 			*prt_type					= NULL;
	char 			*p_status					= NULL;
	char 			*type						= "Module";
	char 			*rev_id						= NULL;
	
	
	ifail = ITEM_find_rev(ID,RevID,&rev_tag);	
	if(ifail != ITK_ok)
	{
		printf("Object not found in Teamcenter...!!\n");
		fprintf(output,"%s", sep);
		fprintf(output,"Not Found\n");
		return 0;
	}
	else
	{
		printf("Object Found...!!\n");
	}

	ifail = GRM_find_relation_type("IMAN_specification",&relation_type);

	ifail = GRM_list_secondary_objects_only(rev_tag,relation_type,&count,&secondary_objects);

	if(count>0)
	{
	for (revloop=0;revloop<count; revloop++)
	{	
		ifail = AOM_refresh	(secondary_objects[revloop],0);	
		ifail = AOM_lock_for_delete(secondary_objects[revloop]);
		ifail = AOM_delete_from_parent(secondary_objects[revloop],rev_tag);	
	}
	}

	ifail = GRM_find_relation_type("IMAN_Rendering",&relation_typeR);

	ifail = GRM_list_secondary_objects_only(rev_tag,relation_typeR,&countR,&secondary_objectsR);

	if(countR>0)
	{
	for (revloopR=0;revloopR<countR; revloopR++)
	{	
		ifail = AOM_refresh	(secondary_objectsR[revloopR],0);	
		ifail = AOM_lock_for_delete(secondary_objectsR[revloopR]);
		ifail = AOM_delete_from_parent(secondary_objectsR[revloopR],rev_tag);	
	}
	}
	printf(" Related datasets deleted \n");

	ifail = AOM_refresh(rev_tag,0);	
	ifail = AOM_lock(rev_tag);	
	ifail = AOM_delete(rev_tag);

	printf(" Design revision deleted \n");

	MEM_free(rev_id);
	MEM_free(prt_type);
		
	return ifail;
}

/********************************************************************************************
    Function:       write_info
    Description:    This function takes rev_id and attr_value as input and write it into
					the file.(newly created)
********************************************************************************************/
int write_info(char* item_id, char* rev_id, char* attr_value)
{
	int			ifail 				= ITK_ok;
	
	char 		*test2 				= (char*)MEM_alloc(sizeof(char) * 50);
	char		*final2				= (char*)MEM_alloc(sizeof(char) * 50);

		tc_strcpy(test2, "");
		tc_strcpy(test2,item_id);
		tc_strcat(test2,sep);
		tc_strcat(test2,rev_id);
		tc_strcat(test2,sep);
		tc_strcat(test2,attr_value);
		
		tc_strcpy(final2,test2);
		
		fprintf(output,"%s\n",final2);
	
		MEM_free(test2);	
		MEM_free(final2);	
		return ifail;
}
