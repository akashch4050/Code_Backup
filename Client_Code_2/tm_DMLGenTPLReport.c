/**********************************************************************************************************************************************************
**
** SOURCE FILE NAME: DML_GenTPL_Report.c
**
** Functions:- 	This utility queries DML into teamcenter and get it's revision's closure date, if found null then stamps released date as a closure date 
				on DML Revision and it's associated tasks and writes required information into the file.
** 
**
**	Date							Author								Modification
**	02-Aug-2018						Kalpesh Gondhali					Code creation
**
** command to run:
** DML_GenTPL_Report fileNameIntput
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
#include <tccore/tctype.h>
#include <tc/preferences.h>
#include <tccore/aom_prop.h>
#include <fclasses/tc_string.h>
#include <tccore/workspaceobject.h>
#include <user_exits/epm_toolkit_utils.h>
#define PROP_UIF_ask_value_msg   "PROP_UIF_ask_value"
#define CHECK_FAIL if (ifail != 0) { printf("line %d (ifail %d)\n", __LINE__, ifail); return 0;}

#define ITK_CALL(X) (report_error( __FILE__, __LINE__, #X, (X)))

static int report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_msg_string;

        EMH_get_error_string (NULLTAG, return_code, &error_msg_string);
        printf("ERROR: %d ERROR MSG: %s.\n", return_code, error_msg_string);
        TC_write_syslog("ERROR: %d ERROR MSG: %s.\n", return_code, error_msg_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);    
        TC_write_syslog("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        if(error_msg_string) MEM_free(error_msg_string);
		
    }
	return return_code;
}

FILE		*output 			= NULL;
int			ifail 				= ITK_ok;
char		*sep				= "^";
int read_value_from_file(char*);
int DML_GenTPL_Report(char*);

void print_requirement()
{
	printf(
        "\n all the following parameters are mandatory for procedure"
        " USAGE:\n"
        " -u=<teamcenter dbUsr id> (required)\n"
        " -p=<teamcenter password> (required)\n"
        " -g=<teamcenter group> (required)\n"
        " -file=<file name> (required)\n"
        );	
}


static int PrintErrorStack( void )
/*
*
* PURPOSE : Function to dump the ITK error stack
*
* RETURN : causes program termination. If you made it here
*          you're not coming back modified for cust.c to not call exit()
*          but to just print the error stack
*
* NOTES : This version will always return ITK_ok, which is quite strange
*           actually. But if the error reporting was "OK" then that makes
*           sense
*
*/
{
    int iNumErrs = 0;
    const int *pSevLst;
    const int *pErrCdeLst;
    const char **pMsgLst;
    register int i = 0;

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst ); 
	//fprintf( stderr, "in PrintErrorStack iNumErrs :%d \n",iNumErrs);
    for ( i = 0; i < iNumErrs; i++ )
    {
		fprintf( stderr, "Error(PrintErrorStack): \n");
        fprintf( stderr, "\t%6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
    }
    return ITK_ok;
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
	char			* dml_no						= NULL;
	char			Data[100]					= "";
	char			outputFile[200]				= "";
	char			*test						= (char*)MEM_alloc(sizeof (char) * 200);

	userid			= ITK_ask_cli_argument("-u=");
	password 		= ITK_ask_cli_argument("-p=");
	group 			= ITK_ask_cli_argument("-g=");
	dml_no 			= ITK_ask_cli_argument("-dmlno=");
	
	time_t 	now;
	struct 	tm when;
	
	time(&now);
	when = *localtime(&now);
	//for output file with date.
	strftime(Data,100,"%d_%b_%Y_%H_%M_%S",&when);
	sprintf(outputFile,"%s_output.txt",Data);

	//to remove extra sapce.
	if(tc_strlen(stripBlanks(userid)) == 0  || tc_strlen(stripBlanks(password)) == 0  || tc_strlen(stripBlanks(group)) == 0 || tc_strlen(stripBlanks(dml_no)) == 0)
	{
		print_requirement();
		return -1;
	}
	
	//Auto login
	//ifail=ITK_auto_login();
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_init_module( userid, password, group ));
	/*if (ifail != ITK_ok)
	{
		printf("Error in Login to Teamcenter\n");
		return ifail;
	}
	else
	{
		printf("\nLogin Successfull.....!!\n");
		printf("\nWelcome to Teamcenter.....!!\n");
	}*/
	//provide access.
	ITK_CALL(ITK_set_bypass(true));
	//ifail=ITK_set_bypass(true);
   /* if (ifail != ITK_ok)
	{
		printf("\nUser is not Privileged Admin User \n\n");
		return -1;
	}	*/
	//File open here
	output = fopen(outputFile, "a+");
	if (output == NULL)
	{
		//printf("ERROR: Cannot open File\n");
		return -1;
	}
	else
	{
		//printf("File opened successfully.\n");
	}
	
		tc_strcpy(test, "");
		tc_strcpy(test,"dml_no");
		tc_strcat(test,sep);
		tc_strcat(test,"task_id");
		tc_strcat(test,sep);
		tc_strcat(test,"part_id");
		tc_strcat(test,sep);
		tc_strcat(test,"Remark/status");

		fprintf(output,"%s\n",test);
		MEM_free(test);
			
		//ifail = read_value_from_file(file);
		ITK_CALL(DML_GenTPL_Report(dml_no));
		CHECK_FAIL;
	fclose(output);
	//printf("\n*********************");
	//printf("\nEnd of program.....!!");
	//printf("\n*********************\n");
	//ifail =  ITK_exit_module(true);
	return ifail;
}

/*********************************************************************************************
    Function:       read_value_from_file
    Description:    This function reads input value from file and performs action accordingly.	
**********************************************************************************************/
/*int read_value_from_file(char* Filename)
{
	int				count	 	= 0;
	char 			*dml_no		= NULL;
	char 			temp[200];
	
	FILE* fp = NULL;
	fp = fopen(Filename, "r");
	if (fp != NULL)
	{
		while (fgets(temp, 200, fp)!= NULL)
		{
			tc_strcpy(temp,stripBlanks(temp));

			if (tc_strlen(temp) > 0 && tc_strcmp(temp, NULL) != 0)
			{
				dml_no = strtok(temp, " ");
				printf("\nInput dml_no:%s\n",dml_no);fflush(stdout);
				ifail =  DML_GenTPL_Report(dml_no);
			}
			tc_strcpy(temp, "");
		}
	}
	else 
	{
		printf("File Unable to Open...!!");
	}
	fclose(fp);
	return ifail;
}*/


/**********************************************************************************************
    Function:       DML_GenTPL_Report
    Description:    This function queries the dml into teamcenter and expand it through task 
					and it's parts and then checks for release status and effectivity date.
**********************************************************************************************/
int DML_GenTPL_Report(char* dml_no)
{
	FILE 			*InputFile;
	int		i	 			= 0;
	int		j	 			= 0;
	int		transfermode1Cnt	= 0;
	int		transfermodeCnt	= 0;
	int		eff_count		= 0;
	int		task_count		= 0;
	int		part_count		= 0;
	int		status_count	= 0;
	char	*task_id		= NULL;
	char	*part_id		= NULL;
	char	*RevTyp			= NULL;
	char	*status_name	= NULL;
	char	*PrtRevTyp			= NULL;
	char	*prt_id			= NULL;
	char	*revId			= NULL;
	char	*part_Rev			= NULL;
	char	*dmlNumber		= NULL;
	char	output[500];
	char	Doutput[500];
	char	DoutputExl[500];
	char	mainFile1[50];
	char	mainFile[50];
	char	command[512];
	char	docFile[50];

	tag_t	dml_task_rel	= NULLTAG;
	tag_t 	tagRule 			= NULLTAG;
	tag_t	task_part_rel	= NULLTAG;
	tag_t	Prev_tag			= NULLTAG;
	tag_t	rev_tag			= NULLTAG;
	tag_t	part_t			= NULLTAG;
	tag_t	dml_tag			= NULLTAG;
	tag_t	*task_tag		= NULL;
	tag_t	*part_tag		= NULL;
	tag_t	*status_list	= NULL;
	tag_t	*effectivities	= NULL;
	tag_t	*transfermodes1	= NULLTAG;
	tag_t	*transfermodes	= NULLTAG;
	tag_t	RevPrtTypTag		= NULLTAG;
	tag_t	RevTypTag		= NULLTAG;
	tag_t	PLMSession		= NULLTAG;
	tag_t	PLMSession1		= NULLTAG;
	tag_t	msWordType=NULLTAG;
	tag_t	wordDataset=NULLTAG;
	tag_t	wordLatestDat_t=NULLTAG;
	tag_t specRelationType_t=NULLTAG;
	tag_t specificationRel_t=NULLTAG;
		


	ITK_CALL(ITEM_find_item(dml_no, &dml_tag));
	/*if(ifail != ITK_ok)
	{
		//printf("\nDML not found...!!");fflush(stdout);
		TC_write_syslog("\nDML not found in TC...!!\n");fflush(stdout);
	}*/
	ITK_CALL(ITEM_ask_latest_rev(dml_tag, &rev_tag));

	if(TCTYPE_ask_object_type(rev_tag, &RevTypTag));
	if(TCTYPE_ask_name2(RevTypTag,&RevTyp));
	printf("\n MT: RevTyp type: [%s]\n", RevTyp);fflush(stdout);

	TC_write_syslog("\n\n\n\n\n Rev Type ChangeRequestRevision........  \n\n\n\n\n");

	if(tc_strcmp(RevTyp,"ChangeRequestRevision")==0)
	{
		//DML DETAILS.................
		if(AOM_ask_value_string(rev_tag,"item_id",&dmlNumber)!=ITK_ok)PrintErrorStack();
		TC_write_syslog("\n MT:DML Number: [%s]", dmlNumber);fflush(stdout);
		if(AOM_ask_value_string(rev_tag,"item_revision_id",&revId)!=ITK_ok)PrintErrorStack();
		TC_write_syslog(" and revId: [%s]", revId);fflush(stdout);

		//DML XML FILE CREATION.................

		tc_strcpy(Doutput,"");
		tc_strcpy(Doutput,"/tmp/");
		tc_strcat(Doutput,dmlNumber);
		tc_strcat(Doutput,"_DMLOutput");
		tc_strcat(Doutput,".xml");
		TC_write_syslog("\n MT:Doutput.: [%s]", Doutput);fflush(stdout);
		tc_strcpy(DoutputExl,"");
		tc_strcpy(DoutputExl,"/tmp/");
		tc_strcat(DoutputExl,dmlNumber);
		tc_strcat(DoutputExl,"_DMLOutput");
		tc_strcat(DoutputExl,".xls");
		TC_write_syslog("\n MT:DoutputExl.: [%s]", DoutputExl);fflush(stdout);
		tc_strcpy(output,"");
		tc_strcpy(output,"/tmp/");
		tc_strcat(output,dmlNumber);
		tc_strcat(output,"_InputFile");
		tc_strcat(output,".txt");
		TC_write_syslog("\n MT:output.: [%s]", output);fflush(stdout);
		InputFile = fopen(output, "w");

		tc_strcpy(mainFile,"");
		tc_strcpy(mainFile,"/tmp/");
		tc_strcat(mainFile,dmlNumber);
		tc_strcat(mainFile,".xml");
		TC_write_syslog("\n MT:mainFile.: [%s]", mainFile);fflush(stdout);
		fprintf(InputFile,"%s\n",mainFile);

		/*tc_strcpy(mainFile1,"");
		tc_strcat(mainFile1,dmlNumber);
		tc_strcat(mainFile1,"_");
		tc_strcat(mainFile1,revId);
		tc_strcat(mainFile1,".xml");
		printf("\n MT:mainFile1.: [%s]", mainFile1);fflush(stdout);*/
		

		/*//REPORT DOC FILE CREATION.............
		tc_strcpy(docFile,"");
		tc_strcpy(docFile,"/tmp/dmlprint_report_");
		tc_strcat(docFile,dmlNumber);
		tc_strcat(docFile,"_");
		tc_strcat(docFile,revId);
		tc_strcat(docFile,".doc");
		TC_write_syslog("Doc File = %s\n",docFile);
		//printf("\n MT:docFile: [%s]", docFile);fflush(stdout);*/

		//DML DATA EXPORT to XML...............
		ITK_CALL(PIE_create_session(&PLMSession));
		ITK_CALL(PIE_session_set_file(PLMSession,mainFile));
		ITK_CALL(PIE_find_transfer_mode2("T5_DML_Print_Report_TM","DEFAULT_PIE_CONTEXT_STRING",&transfermodeCnt,&transfermodes));
		TC_write_syslog("\n DML TrasnferModes = %d\n",transfermodeCnt); fflush(stdout);
		ITK_CALL(PIE_session_set_transfer_mode(PLMSession,transfermodes[0]));
		ITK_CALL(PIE_session_export_objects(PLMSession,1,&rev_tag));

		TC_write_syslog("\n\n\n\n\n PIE_session_export_objects........  \n\n\n\n\n");

		/*//sprintf(command,"java -classpath /home/uadev/TC_ROOT/bin/tml_tools/xalan.jar org.apache.xalan.xslt.Process -IN %s -OUT /tmp/%s_dml_print_report -XSL /home/uadev/TC_ROOT/bin/tml_tools/DML_print_report_template.xsl >/tmp/wfout.log",mainFile,dmlNumber);
		sprintf(command,"/home/uadev/TC_ROOT/bin/tml_tools/apply.sh %s %s",mainFile,docFile);
		TC_write_syslog("Command = %s\n",command);
		//system("java -cp \".;/home/uadevTC_ROOT/bin/tml_tools\" org.apache.xalan.xslt.Process -IN %s -OUT /tmp/%s_dml_print_report -XSL /home/uadevTC_ROOT/bin/tml_tools/DML_print_report_template.xsl",mainFile,dmlNumber);
		system(command);*/


		//DML TO TASK.................
		ITK_CALL( GRM_find_relation_type("T5_DMLTaskRelation", &dml_task_rel));
		ITK_CALL( GRM_list_secondary_objects_only(rev_tag, dml_task_rel, &task_count, &task_tag));
		//printf("\nTotal %d task found...!!", task_count);fflush(stdout);
		TC_write_syslog("\n\n\n\n\n GRM_list_secondary_objects_only........  \n\n\n\n\n");
		for(i=0;i<task_count;i++)
		{
			TC_write_syslog("\nInside Task[%d]",i);
			ITK_CALL( AOM_UIF_ask_value(task_tag[i], "item_id", &task_id));
			//printf("\nTask name: %s ", task_id);fflush(stdout);

			//TASK TO PART.................
			ITK_CALL(GRM_find_relation_type("CMHasSolutionItem", &task_part_rel));
			ITK_CALL(GRM_list_secondary_objects_only(task_tag[i], task_part_rel, &part_count, &part_tag));
			printf("\n\tTotal %d part found...!!", part_count);fflush(stdout);
			for(j=0;j<part_count;j++)
			{
				part_Rev	= NULL;
				part_id		= NULL;
				ITK_CALL(AOM_UIF_ask_value(part_tag[j], "item_id", &part_id));
				ITK_CALL(AOM_UIF_ask_value(part_tag[j], "item_revision_id", &part_Rev));
				printf("\n Part name: %s part_Rev: %s", part_id,part_Rev);fflush(stdout);

				tc_strcpy(mainFile,"");
				tc_strcpy(mainFile,"/tmp/");
				tc_strcat(mainFile,part_id);
				tc_strcat(mainFile,".xml");
				
				printf("\n MT:mainFile: [%s]", mainFile);fflush(stdout);
				printf("\n MT:mainFile1: [%s]", mainFile1);fflush(stdout);
				fprintf(InputFile,"%s\n",mainFile);

				//PART GENTPL DATA EXPORT.............
				ITK_CALL(PIE_create_session(&PLMSession));
				ITK_CALL( CFM_find("Latest Working", &tagRule));
				ITK_CALL(PIE_find_transfer_mode2("T5_GENTPL_TM","DEFAULT_PIE_CONTEXT_STRING",&transfermode1Cnt,&transfermodes1));
				//printf("\n GENTPL TrasnferModes = %d\n",transfermode1Cnt);fflush(stdout);
				ITK_CALL(PIE_session_set_transfer_mode(PLMSession,transfermodes1[0]));
				ITK_CALL(PIE_session_set_revision_rule(PLMSession,tagRule));
				ITK_CALL(PIE_session_set_file(PLMSession,mainFile));
				ITK_CALL(PIE_session_export_objects(PLMSession,1,&part_tag[j]));
				TC_write_syslog("\n\n\n\n\n PIE_session_export_objects........  \n\n\n\n\n");

			}
		}
			// Read input.txt file and get all list of xmls. 
			// Replace Header by Header-PLMXML
			// Incremental IDs for shell
			// Merge all XMLs in to main.xml file.

				//sprintf(command,"/home/uadev/TC_ROOT/bin/tml_tools/reportsDMLGenTPL.sh %s %s",mainFile,docFile);
				//sprintf(command,"/home/uadev/TC_ROOT/bin/tml_tools/reportsDMLGenTPL.sh");
				//sprintf(command,"/home/uadev/TC_ROOT/bin/tml_tools/reportsDMLGenTPL.sh dml_gentpl_report -reportxml /tmp/DMLOutput.xml -inputfile %s",output);
				sprintf(command,"/user/uaprod/TC_ROOT_10/bin/tml_tools/reportsDMLGenTPL.sh %s",dmlNumber);
				TC_write_syslog("\n\n\n\nCommand = %s\n\n\n\n\n",command);
				//system("java -cp \".;/home/uadevTC_ROOT/bin/tml_tools\" org.apache.xalan.xslt.Process -IN %s -OUT /tmp/%s_dml_print_report -XSL /home/uadevTC_ROOT/bin/tml_tools/DML_print_report_template.xsl",mainFile,dmlNumber);
				fclose(InputFile);
				system(command);

				//sprintf(command,"java -classpath /home/uadev/TC_ROOT/bin/tml_tools/xalan.jar org.apache.xalan.xslt.Process -IN %s -OUT /tmp/%s_dml_print_report -XSL /home/uadev/TC_ROOT/bin/tml_tools/DML_print_report_template.xsl >/tmp/wfout.log",mainFile,dmlNumber);
				//sprintf(command,"/home/uadev/TC_ROOT/bin/tml_tools/apply.sh %s %s",mainFile,docFile);
				sprintf(command,"/user/uaprod/TC_ROOT_10/bin/tml_tools/apply.sh  %s %s",Doutput,DoutputExl);
				TC_write_syslog("Command = %s\n",command);
				//system("java -cp \".;/home/uadevTC_ROOT/bin/tml_tools\" org.apache.xalan.xslt.Process -IN %s -OUT /tmp/%s_dml_print_report -XSL /home/uadevTC_ROOT/bin/tml_tools/DML_print_report_template.xsl",mainFile,dmlNumber);
				system(command);

				// create dataset for output excel

			
				TC_write_syslog("Doc file created successfully\n");
				ITK_CALL(AE_find_datasettype2("MSExcel",&msWordType));
/*				if(msWordType == NULLTAG)
				{
					TC_write_syslog("Could not find Dataset Type MSExcel. Please check data model\n");
					
				}	*/			
				ITK_CALL(AE_create_dataset_with_id(msWordType,dmlNumber,"DML GenTPL Report","","",&wordDataset));
				ITK_CALL(AE_save_myself(wordDataset));
				ITK_CALL(GRM_find_relation_type("IMAN_reference",&specRelationType_t));
				ITK_CALL(GRM_create_relation(rev_tag,wordDataset,specRelationType_t,NULLTAG,&specificationRel_t));
				ITK_CALL(AOM_load(specificationRel_t));
				ITK_CALL(GRM_save_relation(specificationRel_t));
				ITK_CALL(AE_ask_dataset_latest_rev(wordDataset,&wordLatestDat_t));
				ITK_CALL(AOM_refresh(wordLatestDat_t,TRUE));
				ITK_CALL(AE_import_named_ref(wordLatestDat_t,"excel",DoutputExl,NULL,SS_BINARY));
				ITK_CALL(AE_save_myself(wordLatestDat_t));
				//remove(DoutputExl);
				



	}

	MEM_free(part_tag);
	MEM_free(task_tag);
	
	
	//return ifail;	
	return ITK_ok;	
}
