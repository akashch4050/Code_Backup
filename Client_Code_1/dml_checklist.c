/***********************************************************************************************************************************************************
**
** SOURCE FILE NAME: dml_checklist.c
**
** Functions:- 	This utility queries ERC_DML CheckList into teamcenter and get it's attribute values and then creates form with class T5_ERCDMLChecklist
				and stamp those attribute values and then creates relation(IMAN_manifestation) between form and ERC DML's latest_rev found from input file.
				
** 
**
**	Date							Author								Modification
**	26-April-2018					Kalpesh Gondhali					Code creation
**
** command to run:
** dml_checklist -u=<username> -p=<password> -g=<group> -file=<input_file>
************************************************************************************************************************************************************/

#include <time.h>
#include <stdio.h>
#include <tc/tc.h>
#include <ps/ps.h>
#include <tc/emh.h>
#include <bom/bom.h>
#include <qry/qry.h>
#include <tc/folder.h>
#include <tccore/aom.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <tc/tc_macros.h>
#include <bom/bom_attr.h>
#include <tc/tc_startup.h> 
#include <tccore/tctype.h>
#include <tcinit/tcinit.h>
#include <tc/preferences.h>
#include <tccore/aom_prop.h>
#include <fclasses/tc_string.h>
#include <tccore/workspaceobject.h>
#include <user_exits/epm_toolkit_utils.h>

#define PROP_UIF_ask_value_msg   "PROP_UIF_ask_value"
#define CHECK_FAIL if (ifail != 0) { printf("line %d (ifail %d)\n", __LINE__, ifail); return 0;}

int			ifail 				= ITK_ok;

int read_value_from_file(char*);
int get_parts(char*);

void print_requirement()
{
	printf(
        "\n all the following parameters are mandatory for procedure"
        " USAGE:\n"
        " -u=<teamcenter dbUsr id> (required)\n"
        " -p=<teamcenter password> (required)\n"
        " -g=<teamcenter group> (required)\n"
        " -file=<Filename> (required)\n"
        );	
}

/*******************************************************************************
    Function:       ITK_user_main
    Description:    This is ITK main function. The program starts from here. 
					It checks for the proper dbUsr name and password.	
*******************************************************************************/
int ITK_user_main(int argc, char *argv[])
{
	char			* userid 					= NULL;
	char			* password					= NULL;
	char			* group						= NULL;
	char			* prj_code					= NULL;
	char			* Filename					= NULL;
	
	userid			= ITK_ask_cli_argument("-u=");
	password 		= ITK_ask_cli_argument("-p=");
	group 			= ITK_ask_cli_argument("-g=");
	Filename		= ITK_ask_cli_argument("-file=");
	
	if(tc_strlen(stripBlanks(userid)) == 0  || tc_strlen(stripBlanks(password)) == 0  || tc_strlen(stripBlanks(group)) == 0 || tc_strlen(stripBlanks(Filename)) == 0)
	{
		print_requirement();
		return -1;
	}
	
	ifail = ITK_init_module(userid, password, group);
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
	ifail = ITK_set_bypass(true);
    if (ifail != ITK_ok)
	{
		printf("\nUser is not Privileged Admin User \n\n");
		return -1;
	}	
	
		ifail = read_value_from_file(Filename);
		CHECK_FAIL;

	printf("\nEnd of program.....!!\n");
	printf("\n*********************\n");
	ifail = ITK_exit_module(true);
	return ifail;
}

/*********************************************************************************************
    Function:       read_value_from_file
    Description:    This function reads input value from file and performs action accordingly.	
**********************************************************************************************/
int read_value_from_file(char* Filename)
{	
	char 		*itemid					= NULL;
	char 		*obj_id					= NULL;
	char 		temp[200];
	
	FILE		* fp 					= NULL;
	
	fp = fopen(Filename, "r");
	if (fp != NULL)
	{
		while (fgets(temp, 200, fp)!= NULL)
		{
			tc_strcpy(temp,stripBlanks(temp));
		
			if (tc_strlen(temp) > 0 && tc_strcmp(temp, NULL) != 0)
			{
				itemid = strtok(temp, "^");
				printf("\nInput Item_id:%s\n",itemid);

				
				ifail = get_parts(itemid);
			}
			printf("\n***************************************\n");
			tc_strcpy(temp, "");
		}
	}
	else
	{
		printf("File Unable to Open...!!");
	}
	fclose(fp);
	return ifail;
}


/********************************************************************************************
    Function:       get_parts
    Description:    This function takes the item_tag and object_id as input and writes 
					item's information into a file.
********************************************************************************************/
int get_parts(char* item_id)
{
	int				i 							= 0;
	int				results 					= 0;
	
	tag_t			object						= NULLTAG;
	tag_t			new_object					= NULLTAG;
	tag_t			dml_tag						= NULLTAG;
	tag_t			query_tag					= NULLTAG;
	tag_t			latest_rev					= NULLTAG;
	tag_t			relation_type				= NULLTAG;
	tag_t			new_relation				= NULLTAG;
	tag_t			item_type_tag				= NULLTAG;
	tag_t			item_create_input_tag		= NULLTAG;
	tag_t			*item_tags					= NULL;
	
	char			* value1					= NULL;
	char			* value2					= NULL;
	char			* value3					= NULL;
	char			* value4					= NULL;
	char			* value5					= NULL;
	char			* value6					= NULL;
	char			* value7					= NULL;
	char			* value8					= NULL;
	char			* value9					= NULL;
	char			* value10					= NULL;
	char			* value11					= NULL;
	char			* value12					= NULL;
	char			* value13					= NULL;
	char			* value14					= NULL;
	char			* value15					= NULL;
	char			* value16					= NULL;
	char			* value17					= NULL;
	char			* value18					= NULL;
	char			* value19					= NULL;
	char			* value20					= NULL;
	char			* value21					= NULL;
	char			* value22					= NULL;
	char			* value23					= NULL;
	char			* value24					= NULL;
	char			* value25					= NULL;
	char			* value26					= NULL;
	char			* value27					= NULL;
	char			* value28					= NULL;
	char			* value29					= NULL;
	char			* value30					= NULL;
	char			* value31					= NULL;
	char			* value32					= NULL;
	char			* value33					= NULL;
	char			* value34					= NULL;
	char			* value35					= NULL;
	char			* value36					= NULL;
	char			* value37					= NULL;
	char			* value38					= NULL;
	char			* value39					= NULL;
	char			* value40					= NULL;
	char			* value41					= NULL;
	char			* value42					= NULL;
	char			* value43					= NULL;
	char			* value44					= NULL;
	char			* value45					= NULL;
	char			* value46					= NULL;
	char			* value47					= NULL;
	char			* value48					= NULL;
	char			* value49					= NULL;
	char			* value50					= NULL;
	char			* value51					= NULL;
	char			* value52					= NULL;
	char			* value53					= NULL;
	char			* value54					= NULL;
	char			* value55					= NULL;
	char			* value56					= NULL;
	char			* value57					= NULL;
	char			* value58					= NULL;
	char			* value59					= NULL;
	char			* value60					= NULL;
	char			* value61					= NULL;
	char			* value62					= NULL;
	char			* value63					= NULL;
	char			* value64					= NULL;
	char			* value65					= NULL;
	char			* value66					= NULL;
	char			* value67					= NULL;
	char			* value68					= NULL;
	char			* value69					= NULL;
	char			* value70					= NULL;
	char			* value71					= NULL;
	char			* value72					= NULL;
	char			* value73					= NULL;

	char 			*entry0						= "Name";
	char 			*entry1						= "Type";
	char			*type						= "ERC DML CheckList";
	
	//char			*checklist					= NULL;
	char**  		entries						= NULL;
	char**  		values						= NULL;
		
	entries		= (char**) MEM_alloc (sizeof(char*) * 10);
	values		= (char**) MEM_alloc (sizeof(char*) * 10);
		
	/*tc_strcpy(checklist, "");
	tc_strcpy(checklist, item_id);
	printf("%s",checklist);*/
	
	entries[0] 	= (char*) MEM_alloc (sizeof(char) * (tc_strlen(entry0) + 20));
	values[0]  	= (char*) MEM_alloc (sizeof(char) * (tc_strlen(item_id) + 20));		
	
	entries[1] 	= (char*) MEM_alloc (sizeof(char) * (tc_strlen(entry1) + 20));
	values[1]  	= (char*) MEM_alloc (sizeof(char) * (tc_strlen(type) + 20));	
	
	tc_strcpy(entries[0], "");	
	tc_strcpy(entries[0], entry0);
	tc_strcpy(values[0], "");	
	tc_strcpy(values[0], item_id);
	
	tc_strcpy(entries[1], "");	
	tc_strcpy(entries[1], entry1);
	tc_strcpy(values[1], "");	
	tc_strcpy(values[1], type);
		
	ifail = QRY_ignore_case_on_search(TRUE);
	ifail = QRY_find2("get_checklist", &query_tag);
		
	if(query_tag == NULLTAG)
	{
		printf("\nQuery Not Found in TC....!!\n");
		return 0;
	}
	ifail = QRY_execute(query_tag,2, entries, values, &results, &item_tags);
	printf("Total found object:%d\n", results);
	if(results == 1)
	{
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_ActComplDFMEA", &value1);         
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_AddToDF", &value2 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_AffDesGrpIden", &value3 );        
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_attachdmlDFMEA", &value4 );       
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_ColPrtInd", &value5 );            
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_CorrAppPrtDes", &value6 );        
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_DesRevRec", &value7 );            
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_DispIns", &value8 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_DMLRsnCorrt", &value9 );          
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_DMLSubCorrt", &value10 );          
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_DMRLnkdToDML", &value11 );         
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_DrawClassInd", &value12 );         
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_DrwShtNoCor", &value13 );          
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_EffTstFlrMod", &value14 );         
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_EnvDimProp", &value15 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_ERCDept", &value16 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_FailDigLevGen", &value17 );        
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_GrpWsData", &value18 );            
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_HazMatCont", &value19 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_HomLogImp", &value20 );            
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_IntrChnDef", &value21 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_IsDesgnPrt", &value22 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_IsDsgnNvl", &value23 );            
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_IsDsgnPrtctd", &value24 );         
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_IsRskanalysisDFMEA", &value25 );   
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_MatClsDef", &value26 );            
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_ModAssyTPLStruc", &value27 );      
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_ModCompPrt", &value28 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_ModDetStatCor", &value29 );        
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_PrtyPrtNob", &value30 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_PrtyPrtNoLnk", &value31 );         
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason1", &value32 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason10", &value33 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason11", &value34 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason12", &value35 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason13", &value36 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason14", &value37 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason15", &value38 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason16", &value39 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason17", &value40 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason18", &value41 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason19", &value42 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason2", &value43 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason20", &value44 );
		
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason21", &value60 );

		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason22", &value45 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason23", &value46 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason24", &value47 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason25", &value48 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason26", &value49 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason27", &value50 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason28", &value51 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason29", &value52 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason3", &value53 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason30", &value54 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason31", &value55 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason32", &value56 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason33", &value57 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason34", &value58 );             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason35", &value59 );                          
		             
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason4", &value61 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason5", &value62 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason6", &value63 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason7", &value64 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason8", &value65 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_Reason9", &value66 );              
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_ReqQanSpec", &value67 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_RootCsAnal", &value68 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_SixCompTst", &value69 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_SprCrtrDef", &value70 );           
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_SprIndDef", &value71 );            
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_t5IsDsgnPrctnIntd", &value72 );    
		ifail = AOM_UIF_ask_value (item_tags[0], "t5_WgtDef", &value73 );               

		ifail = TCTYPE_find_type("T5_ERCDMLChecklist", "Form", &item_type_tag);
		ifail = TCTYPE_construct_create_input(item_type_tag, &item_create_input_tag);
		ifail = AOM_set_value_string(item_create_input_tag, "object_name", item_id);
		ifail = TCTYPE_create_object (item_create_input_tag,&object);
		
		ifail = AOM_refresh (object, 1);
		
		ifail = AOM_UIF_set_value (object, "t5_ActComplDFMEA", value1);
		ifail = AOM_UIF_set_value (object, "t5_AddToDF", value2 );
		ifail = AOM_UIF_set_value (object, "t5_AffDesGrpIden", value3 );
		ifail = AOM_UIF_set_value (object, "t5_attachdmlDFMEA", value4 );
		ifail = AOM_UIF_set_value (object, "t5_ColPrtInd", value5 );
		ifail = AOM_UIF_set_value (object, "t5_CorrAppPrtDes", value6 );
		ifail = AOM_UIF_set_value (object, "t5_DesRevRec", value7 );
		ifail = AOM_UIF_set_value (object, "t5_DispIns", value8 );
		ifail = AOM_UIF_set_value (object, "t5_DMLRsnCorrt", value9 );
		ifail = AOM_UIF_set_value (object, "t5_DMLSubCorrt", value10 );
		ifail = AOM_UIF_set_value (object, "t5_DMRLnkdToDML", value11 );
		ifail = AOM_UIF_set_value (object, "t5_DrawClassInd", value12 );
		ifail = AOM_UIF_set_value (object, "t5_DrwShtNoCor", value13 );
		ifail = AOM_UIF_set_value (object, "t5_EffTstFlrMod", value14 );
		ifail = AOM_UIF_set_value (object, "t5_EnvDimProp", value15 );
		ifail = AOM_UIF_set_value (object, "t5_ERCDept", value16 );
		ifail = AOM_UIF_set_value (object, "t5_FailDigLevGen", value17 );
		ifail = AOM_UIF_set_value (object, "t5_GrpWsData", value18 );
		ifail = AOM_UIF_set_value (object, "t5_HazMatCont", value19 );
		ifail = AOM_UIF_set_value (object, "t5_HomLogImp", value20 );
		ifail = AOM_UIF_set_value (object, "t5_IntrChnDef", value21 );
		ifail = AOM_UIF_set_value (object, "t5_IsDesgnPrt", value22 );
		ifail = AOM_UIF_set_value (object, "t5_IsDsgnNvl", value23 );
		ifail = AOM_UIF_set_value (object, "t5_IsDsgnPrtctd", value24 );
		ifail = AOM_UIF_set_value (object, "t5_IsRskanalysisDFMEA", value25 );
		ifail = AOM_UIF_set_value (object, "t5_MatClsDef", value26 );
		ifail = AOM_UIF_set_value (object, "t5_ModAssyTPLStruc", value27 );
		ifail = AOM_UIF_set_value (object, "t5_ModCompPrt", value28 );
		ifail = AOM_UIF_set_value (object, "t5_ModDetStatCor", value29 );
		ifail = AOM_UIF_set_value (object, "t5_PrtyPrtNob", value30 );
		ifail = AOM_UIF_set_value (object, "t5_PrtyPrtNoLnk", value31 );
		ifail = AOM_UIF_set_value (object, "t5_Reason1", value32 );
		ifail = AOM_UIF_set_value (object, "t5_Reason10", value33 );
		ifail = AOM_UIF_set_value (object, "t5_Reason11", value34 );
		ifail = AOM_UIF_set_value (object, "t5_Reason12", value35 );
		ifail = AOM_UIF_set_value (object, "t5_Reason13", value36 );
		ifail = AOM_UIF_set_value (object, "t5_Reason14", value37 );
		ifail = AOM_UIF_set_value (object, "t5_Reason15", value38 );
		ifail = AOM_UIF_set_value (object, "t5_Reason16", value39 );
		ifail = AOM_UIF_set_value (object, "t5_Reason17", value40 );
		ifail = AOM_UIF_set_value (object, "t5_Reason18", value41 );
		ifail = AOM_UIF_set_value (object, "t5_Reason19", value42 );
		ifail = AOM_UIF_set_value (object, "t5_Reason2", value43 );
		ifail = AOM_UIF_set_value (object, "t5_Reason20", value44 );

		ifail = AOM_UIF_set_value (object, "t5_Reason21", value60 );

		ifail = AOM_UIF_set_value (object, "t5_Reason22", value45 );
		ifail = AOM_UIF_set_value (object, "t5_Reason23", value46 );
		ifail = AOM_UIF_set_value (object, "t5_Reason24", value47 );
		ifail = AOM_UIF_set_value (object, "t5_Reason25", value48 );
		ifail = AOM_UIF_set_value (object, "t5_Reason26", value49 );
		ifail = AOM_UIF_set_value (object, "t5_Reason27", value50 );
		ifail = AOM_UIF_set_value (object, "t5_Reason28", value51 );
		ifail = AOM_UIF_set_value (object, "t5_Reason29", value52 );
		ifail = AOM_UIF_set_value (object, "t5_Reason3", value53 );
		ifail = AOM_UIF_set_value (object, "t5_Reason30", value54 );
		ifail = AOM_UIF_set_value (object, "t5_Reason31", value55 );
		ifail = AOM_UIF_set_value (object, "t5_Reason32", value56 );
		ifail = AOM_UIF_set_value (object, "t5_Reason33", value57 );
		ifail = AOM_UIF_set_value (object, "t5_Reason34", value58 );
		ifail = AOM_UIF_set_value (object, "t5_Reason35", value59 );
		
		ifail = AOM_UIF_set_value (object, "t5_Reason4", value61 );
		ifail = AOM_UIF_set_value (object, "t5_Reason5", value62 );
		ifail = AOM_UIF_set_value (object, "t5_Reason6", value63 );
		ifail = AOM_UIF_set_value (object, "t5_Reason7", value64 );
		ifail = AOM_UIF_set_value (object, "t5_Reason8", value65 );
		ifail = AOM_UIF_set_value (object, "t5_Reason9", value66 );
		ifail = AOM_UIF_set_value (object, "t5_ReqQanSpec", value67 );
		ifail = AOM_UIF_set_value (object, "t5_RootCsAnal", value68 );
		ifail = AOM_UIF_set_value (object, "t5_SixCompTst", value69 );
		ifail = AOM_UIF_set_value (object, "t5_SprCrtrDef", value70 );
		ifail = AOM_UIF_set_value (object, "t5_SprIndDef", value71 );
		ifail = AOM_UIF_set_value (object, "t5_t5IsDsgnPrctnIntd", value72 );
		ifail = AOM_UIF_set_value (object, "t5_WgtDef", value73 );

		ifail = AOM_save (object);
		ifail = AOM_refresh (object, 0);
		
		ifail = ITEM_find_item (item_id, &new_object);
		ifail = ITEM_ask_latest_rev (new_object, &latest_rev);
		
		ifail = GRM_find_relation_type ("IMAN_manifestation", &relation_type);
		ifail = GRM_create_relation (latest_rev, object, relation_type, NULLTAG, &new_relation);
		ifail = GRM_save_relation (new_relation);
		if(ifail == ITK_ok)
		{
			printf("\nRelation created...!!");
		}
	}
	else
	{
		printf("\nNot found\n");
		return 0;
	}

	MEM_free(item_tags);
						
	return ifail;
}
