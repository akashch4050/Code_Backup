/****************************************************************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  Author               :   Mohan Tayade
*  Module               :   Workflow Action Handler PO Check and attach NOPO report to DML as xls.
*  Code                 :   tm_DML_Part_NOPOReport.c
*  Command				:	tm_DML_Part_NOPOReport -u=ercpsup  -pf=XYT1ESA  -g=dba -file=input.txt
*						:	tm_DML_Part_NOPOReport -u=ercpsup  -pf=XYT1ESA  -g=dba -taskno=XXX
*  Created on			:   12/04/2019
*	Modification History :
*	S.No    Date         CR No		Modified By			Modification Notes
*	1.		12/04/2019				Mohan Tayade		Workflow Action Handler PO Check and attach NOPO report to DML as xls.
*	2.		08/05/2019				Mohan Tayade		Handle DR3P/AR3P.
*	3.		24/05/2019				Mohan Tayade		Bulk-stock-transaction-PO/Stop at DR4/new created parts/Unmaturated parts.
*	4.		02/06/2020				Mohan Tayade		PO check logic sync,CS-NA parts consider for PO check, UOM 4 parts only consider.
*****************************************************************************************************************************/

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
#include <pie/pie.h>
#include <tccore/tctype.h>
#include <tc/preferences.h>
#include <tccore/aom_prop.h>
#include <fclasses/tc_string.h>
#include <tccore/workspaceobject.h>
#include <user_exits/epm_toolkit_utils.h>
#define ITK_err 919002
#define PROP_UIF_ask_value_msg   "PROP_UIF_ask_value"
#define CHECK_FAIL if (ifail != 0) { printf("line %d (ifail %d)\n", __LINE__, ifail); return 0;}
#define ITK_CALL(X) (report_error( __FILE__, __LINE__, #X, (X)))

char	*NOPOPartsSet = NULL;
char	*TaskAtchedPartSet = NULL;
char	*UnMatPartSet = NULL;
char	*NwNOPOPartsSet = NULL;
char	*PORevNOPOPartsSet = NULL;
char	*PSCPORevNOPOPartsSet = NULL;
char	*BypassPartSet = NULL;
char	*POAvailPartSet = NULL;
char	*PORevAvailPartSet = NULL;
char	*SkippedPartSet = NULL;
char	*sPOCHKInfo3 = NULL;
char	*RelDRStatus = NULL;
char	*sPlantMakeBuy = NULL;
char	*FromTOStus = NULL;
char	*PARTTYPEs = NULL;

char* subString (char* mainStringf ,int fromCharf,int toCharf)
{
	int i;
	char *retStringf;
	//printf("\n count found %s ",mainStringf);fflush(stdout);
	retStringf = (char*) MEM_alloc(200);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	//printf("\n return string found %s ",retStringf);fflush(stdout);
	return retStringf;
}
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
int tm_get_plantmakebuy(char*);

void print_requirement()
{
	printf(
	"\nHELP:\n"
	"POcheckatERC -u=<username> -pf=<password file path> -g=<group> -file=<file_name>\n"
	"------------------------------------------------------------------------------------------\n\n"
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
int ITK_user_main(int argc, char *argv[])
{
	int				ifail 						= ITK_ok;
	char			* userid 					= NULL;
	char			* password					= NULL;
	char			* group						= NULL;
	char			* Input						= NULL;
	char			* TPlant					= NULL;
	char			* Filename					= NULL;
	//char			Data[100]					= "";
	//char			outputFile[200]				= "";

	//userid		= ITK_ask_cli_argument("-u=");
	//password 	= ITK_ask_cli_argument("-p=");
	//group 		= ITK_ask_cli_argument("-g=");
	Input 		= ITK_ask_cli_argument("-Object_Name=");
	TPlant 		= ITK_ask_cli_argument("-Target_Plant=");

	time_t 	now;
	struct 	tm when;

	time(&now);
	when = *localtime(&now);

	//strftime(Data,100,"%d_%b_%Y_%H_%M_%S",&when);
	//sprintf(outputFile,"%s_output.txt",Data);

	/*if(tc_strlen(stripBlanks(userid)) == 0  || tc_strlen(stripBlanks(password)) == 0  || tc_strlen(stripBlanks(group)) == 0)
	{
		print_requirement();
		return -1;
	}*/

	//ifail = ITK_auto_login();
	ifail = ITK_init_module("ercpsup", "ERCpsup2019", "Engineering");
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
	//ifail = ITK_set_bypass(true);
    if (ifail != ITK_ok)
	{
		printf("\nUser is not Privileged Admin User \n\n");
		return -1;
	}
	//ifail = read_value_from_file(Filename);
	//ifail = tm_ChkVCMSforNOPOFun(dmlno);
	ifail = tm_POCheckStopAtFun(Input,TPlant);
	CHECK_FAIL;

	printf("\n*********************");
	printf("\nEnd of program.....!!");
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
	int				ifail 					= ITK_ok;
	int				count	 				= 0;

	char 			*itemid					= NULL;
	char 			*t_plant				= NULL;
	char 			*design_group			= NULL;
	char 			*dr_status				= NULL;
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
				itemid = strtok(temp, "");
				printf("\nInput Item_id:%s",itemid);


		//ifail = tm_DesignRevBOM(itemid,"13PP254028","DR2");
				ifail = tm_POCheckStopAtFun(itemid,t_plant);
			}
			tc_strcpy(temp, "");
		}
	}
	else
	{
		printf("File Unable to Open...!!");
	}
	fclose(fp);
	fclose(output);
	return ifail;
}

///CODE START HERE
int t5GetItemRevison(char* InputPart)
{
	//MT start
	tag_t   OutPutTag     = NULLTAG;
	int n_tags_found2=0;
	tag_t *tags_found2 = NULL;
	
	//char **attrs2 = (char **) MEM_alloc(200 * sizeof(char *));
	//char **values2 = (char **) MEM_alloc(200 * sizeof(char *));
	int n_tags_foundd2=0;
	int n_tags_foundd3=0;
	tag_t *tags_foundd3 = NULL;
	tag_t *tags_foundd2 = NULL;
	//char **attrss2 = (char **) MEM_alloc(200 * sizeof(char *));
	//char **valuess2 = (char **) MEM_alloc(200 * sizeof(char *));
	const char *attrss2[2];
	const char *valuess2[2];
	const char *attrss3[2];
	const char *valuess3[2];
	//MT end
	//printf("\n FUN:part no :%s.", InputPart);fflush(stdout);
	//GETTING RELEASED REVISION
	const char *attrs2[2];
	const char *values2[2];
	attrs2[0] ="item_id";
	attrs2[1] ="object_type";
	values2[0] = (char *)InputPart;
	values2[1] = "Design";
	if(ITEM_find_items_by_key_attributes(2,attrs2, values2, &n_tags_found2, &tags_found2)!=ITK_ok)PrintErrorStack();
	//printf("\n FUN:count n_tags_found2 :%d.", n_tags_found2);fflush(stdout);

	if (n_tags_found2>0)
	{
		OutPutTag= tags_found2[0];
	}
	else
	{
		///printf("\n FUN:Inside T5_EE_Part:%s.", InputPart);fflush(stdout);
		attrss2[0] ="item_id";
		attrss2[1] ="object_type";
		valuess2[0] = (char *)InputPart;
		valuess2[1] = "T5_EE_Part";
		if(ITEM_find_items_by_key_attributes(2,attrss2, valuess2, &n_tags_foundd2, &tags_foundd2)!=ITK_ok)PrintErrorStack();
		//printf("\n FUN:EE part count :%d.", n_tags_foundd2);fflush(stdout);
		if (n_tags_foundd2>0)
		{
			OutPutTag= tags_foundd2[0];
		}
		else
		{
			//printf("\n FUN:Inside T5_ClrPart:%s.", InputPart);fflush(stdout);
			attrss3[0] ="item_id";
			attrss3[1] ="object_type";
			valuess3[0] = (char *)InputPart;
			valuess3[1] = "T5_ClrPart";
			if(ITEM_find_items_by_key_attributes(2,attrss3, valuess3, &n_tags_foundd3, &tags_foundd3)!=ITK_ok)PrintErrorStack();
			//printf("\n FUN:T5_ClrPart part count :%d.", n_tags_foundd3);fflush(stdout);
			if (n_tags_foundd3>0)
			{
				OutPutTag= tags_foundd3[0];
			}
			else
			{
				//printf("\n FUN:Inside T5_ArchModule:%s.", InputPart);fflush(stdout);
				attrss3[0] ="item_id";
				attrss3[1] ="object_type";
				valuess3[0] = (char *)InputPart;
				valuess3[1] = "T5_ArchModule";
				if(ITEM_find_items_by_key_attributes(2,attrss3, valuess3, &n_tags_foundd3, &tags_foundd3)!=ITK_ok)PrintErrorStack();
				//printf("\n FUN:T5_ArchModule part count :%d.", n_tags_foundd3);fflush(stdout);
				if (n_tags_foundd3>0)
				{
					OutPutTag= tags_foundd3[0];
				}
			}
		}
	}
	if(OutPutTag==NULLTAG)
	{
		printf("\n FUN: NULL tag for part no:%s.", InputPart);fflush(stdout);
	}
	else
	{
		printf("\n FUN: tag found for part no:%s.", InputPart);fflush(stdout);
	}

	return OutPutTag;	
}
int t5GetItemRevisonForRelRev(char* InputPart,char* IsCCVC)
{
	//MT start
	int Frel_status_cunt =0;
	int funii =0;
	int Item_count =0;
	int fcrr =0;
	int FDRChkFlag =0;
	tag_t   OutPutTagg     = NULLTAG;
	tag_t   OutPutTag     = NULLTAG;
	int n_tags_found2=0;
	tag_t *tags_found2 = NULL;
	char*	 sPartno = NULL;
	char*	 FunParentProj = NULL;
	char*	 FunPrtDRst = NULL;
	char*	 FunPartno = NULL;
	char*	 FunPrnt_Type = NULL;
	char*	 Funrev_idd = NULL;
	char*	 Funrel_status = NULL;
	char*	 FChld_Rel_Stus = NULL;
	tag_t	*Funrel_status_lst		= NULL;
	tag_t	*Child_sttus_lst		= NULL;
	tag_t	*Funrev_list		= NULL;
	int n_tags_foundd2=0;
	int n_tags_foundd3=0;
	tag_t *tags_foundd3 = NULL;
	tag_t *tags_foundd2 = NULL;
	const char *attrss2[2];
	const char *valuess2[2];
	const char *attrss3[2];
	const char *valuess3[2];
	//GETTING RELEASED REVISION
	const char *attrs2[2];
	const char *values2[2];
	attrs2[0] ="item_id";
	attrs2[1] ="object_type";
	values2[0] = (char *)InputPart;
	values2[1] = "Design";
	if(ITEM_find_items_by_key_attributes(2,attrs2, values2, &n_tags_found2, &tags_found2)!=ITK_ok)PrintErrorStack();
	//printf("\n FUN:count n_tags_found2 :%d.", n_tags_found2);fflush(stdout);

	if (n_tags_found2>0)
	{
		OutPutTag= tags_found2[0];
	}
	else
	{
		///printf("\n FUN:Inside T5_EE_Part:%s.", InputPart);fflush(stdout);
		attrss2[0] ="item_id";
		attrss2[1] ="object_type";
		valuess2[0] = (char *)InputPart;
		valuess2[1] = "T5_EE_Part";
		if(ITEM_find_items_by_key_attributes(2,attrss2, valuess2, &n_tags_foundd2, &tags_foundd2)!=ITK_ok)PrintErrorStack();
		//printf("\n FUN:EE part count :%d.", n_tags_foundd2);fflush(stdout);
		if (n_tags_foundd2>0)
		{
			OutPutTag= tags_foundd2[0];
		}
		else
		{
			//printf("\n FUN:Inside T5_ClrPart:%s.", InputPart);fflush(stdout);
			attrss3[0] ="item_id";
			attrss3[1] ="object_type";
			valuess3[0] = (char *)InputPart;
			valuess3[1] = "T5_ClrPart";
			if(ITEM_find_items_by_key_attributes(2,attrss3, valuess3, &n_tags_foundd3, &tags_foundd3)!=ITK_ok)PrintErrorStack();
			//printf("\n FUN:T5_ClrPart part count :%d.", n_tags_foundd3);fflush(stdout);
			if (n_tags_foundd3>0)
			{
				OutPutTag= tags_foundd3[0];
			}
			else
			{
				//printf("\n FUN:Inside T5_ArchModule:%s.", InputPart);fflush(stdout);
				attrss3[0] ="item_id";
				attrss3[1] ="object_type";
				valuess3[0] = (char *)InputPart;
				valuess3[1] = "T5_ArchModule";
				if(ITEM_find_items_by_key_attributes(2,attrss3, valuess3, &n_tags_foundd3, &tags_foundd3)!=ITK_ok)PrintErrorStack();
				//printf("\n FUN:T5_ArchModule part count :%d.", n_tags_foundd3);fflush(stdout);
				if (n_tags_foundd3>0)
				{
					OutPutTag= tags_foundd3[0];
				}
			}
		}
	}
	if(OutPutTag==NULLTAG)
	{
		printf("\n F1: NULL tag for part no:%s.", InputPart);fflush(stdout);
	}
	else
	{
		printf("\n F2: tag found for part no:%s. now getting latest released rev tag", InputPart);fflush(stdout);
		if(ITEM_list_all_revs(OutPutTag, &Item_count, &Funrev_list)!=ITK_ok)PrintErrorStack();
		printf("\n F3:Total rev found: %d.", Item_count);fflush(stdout);
		if(Item_count > 0)
		{
			funii=0;
			for(funii=Item_count-1;funii>=0;funii--)
			{
				Frel_status_cunt=0;
				if(AOM_UIF_ask_value(Funrev_list[funii], "item_id", &FunPartno)!=ITK_ok)PrintErrorStack();
				if(AOM_UIF_ask_value(Funrev_list[funii], "item_revision_id", &Funrev_idd)!=ITK_ok)PrintErrorStack();
				if(AOM_UIF_ask_value(Funrev_list[funii], "release_status_list", &Funrel_status)!=ITK_ok)PrintErrorStack();
				if(AOM_ask_value_string(Funrev_list[funii],"t5_PartStatus",&FunPrtDRst)!=ITK_ok)PrintErrorStack();
				if(AOM_ask_value_string(Funrev_list[funii],"t5_PartType",&FunPrnt_Type)!=ITK_ok)PrintErrorStack();
				if( AOM_ask_value_string(Funrev_list[funii],"t5_ProjectCode",&FunParentProj)==ITK_ok)  PrintErrorStack();
				printf("\n F4:Part %s rev:%s and Rel Status:%s PrtDRst:%s Prnt_Type:%s ParentProj:%s", FunPartno,Funrev_idd,Funrel_status,FunPrtDRst,FunPrnt_Type,FunParentProj);fflush(stdout);
				if(tc_strcmp(FunParentProj, "1111")!=0)
				{
					//Release status.
					if(WSOM_ask_release_status_list (Funrev_list[funii], &Frel_status_cunt, &Funrel_status_lst)!=ITK_ok)PrintErrorStack();
					printf("\n F5:Frel_status_cunt is:%d.", Frel_status_cunt);fflush(stdout);
					if(Frel_status_cunt > 0)
					{
						fcrr=0;
						FDRChkFlag=0;
						for(fcrr=0;fcrr<Frel_status_cunt;fcrr++)
						{
							FDRChkFlag=0;
							if(AOM_ask_value_string (Funrel_status_lst[fcrr], "object_name", &FChld_Rel_Stus)!=ITK_ok)PrintErrorStack();
							printf("\n F6: FChld_Rel_Stus:%s", FChld_Rel_Stus);fflush(stdout);
							//check here DmlFromDR
							if((tc_strstr(Funrel_status,"ERC Released")!=NULL) ||(tc_strstr(FChld_Rel_Stus,"Rlzd")!=NULL)||(tc_strstr(FChld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)||(tc_strstr(FChld_Rel_Stus,"ERC Released")!=NULL) ||(tc_strstr(Funrel_status,"T5_LcsErcRlzd")!=NULL))
							{
								if((tc_strcmp(FromTOStus,"AR3 to AR3P")==0)||(tc_strcmp(FromTOStus,"DR3 to DR3P")==0)||(tc_strcmp(FromTOStus,"DR3P")==0)||(tc_strcmp(FromTOStus,"AR3P")==0))
								{
									if(tc_strcmp(IsCCVC,"CCVC")==0)
									{
										if((tc_strcmp(FunPrtDRst,"AR3P")==0)||(tc_strcmp(FunPrtDRst,"DR3P")==0)||(tc_strcmp(FunPrtDRst,"AR4")==0)||(tc_strcmp(FunPrtDRst,"DR4")==0)||(tc_strcmp(FunPrtDRst,"AR5")==0)||(tc_strcmp(FunPrtDRst,"DR5")==0))
										{
											printf("\n C1:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
											OutPutTag= Funrev_list[funii];
											FDRChkFlag=1;
											break;
										}
									}
									else
									{
										if((tc_strcmp(FunPrtDRst,"AR3")==0)||(tc_strcmp(FunPrtDRst,"DR3")==0)||(tc_strcmp(FunPrtDRst,"AR3P")==0)||(tc_strcmp(FunPrtDRst,"DR3P")==0)||(tc_strcmp(FunPrtDRst,"AR4")==0)||(tc_strcmp(FunPrtDRst,"DR4")==0)||(tc_strcmp(FunPrtDRst,"AR5")==0)||(tc_strcmp(FunPrtDRst,"DR5")==0))
										{
											printf("\n P1:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
											OutPutTag= Funrev_list[funii];
											FDRChkFlag=1;
											break;
										}
									}
								}
								else if((tc_strcmp(FromTOStus,"AR3 to AR4")==0)||(tc_strcmp(FromTOStus,"DR3 to DR4")==0)||(tc_strcmp(FromTOStus,"AR4")==0)||(tc_strcmp(FromTOStus,"DR4")==0))
								{
									if(tc_strcmp(IsCCVC,"CCVC")==0)
									{
										if((tc_strcmp(FunPrtDRst,"AR4")==0)||(tc_strcmp(FunPrtDRst,"DR4")==0)||(tc_strcmp(FunPrtDRst,"AR5")==0)||(tc_strcmp(FunPrtDRst,"DR5")==0))
										{
											printf("\n C2:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
											OutPutTag= Funrev_list[funii];
											FDRChkFlag=1;
											break;
										}
									}
									else
									{
										if((tc_strcmp(FunPrtDRst,"AR3")==0)||(tc_strcmp(FunPrtDRst,"DR3")==0)||(tc_strcmp(FunPrtDRst,"AR3P")==0)||(tc_strcmp(FunPrtDRst,"DR3P")==0)||(tc_strcmp(FunPrtDRst,"AR4")==0)||(tc_strcmp(FunPrtDRst,"DR4")==0)||(tc_strcmp(FunPrtDRst,"AR5")==0)||(tc_strcmp(FunPrtDRst,"DR5")==0))
										{
											printf("\n P2:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
											OutPutTag= Funrev_list[funii];
											FDRChkFlag=1;
											break;
										}
									}
								}
								else if((tc_strcmp(FromTOStus,"AR3P to AR4")==0)||(tc_strcmp(FromTOStus,"DR3P to DR4")==0)||(tc_strcmp(FromTOStus,"AR4")==0)||(tc_strcmp(FromTOStus,"DR4")==0))
								{
									if(tc_strcmp(IsCCVC,"CCVC")==0)
									{
										if((tc_strcmp(FunPrtDRst,"AR4")==0)||(tc_strcmp(FunPrtDRst,"DR4")==0)||(tc_strcmp(FunPrtDRst,"AR5")==0)||(tc_strcmp(FunPrtDRst,"DR5")==0))
										{
											printf("\n C3:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
											OutPutTag= Funrev_list[funii];
											FDRChkFlag=1;
											break;
										}
									}
									else
									{
										if((tc_strcmp(FunPrtDRst,"AR3P")==0)||(tc_strcmp(FunPrtDRst,"DR3P")==0)||(tc_strcmp(FunPrtDRst,"AR4")==0)||(tc_strcmp(FunPrtDRst,"DR4")==0)||(tc_strcmp(FunPrtDRst,"AR5")==0)||(tc_strcmp(FunPrtDRst,"DR5")==0))
										{
											printf("\n P3:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
											OutPutTag= Funrev_list[funii];
											FDRChkFlag=1;
											break;
										}
									}
								}
								else if((tc_strcmp(FromTOStus,"AR4 to AR5")==0)||(tc_strcmp(FromTOStus,"DR4 to DR5")==0)||(tc_strcmp(FromTOStus,"AR5")==0)||(tc_strcmp(FromTOStus,"DR5")==0))
								{
									if(tc_strcmp(IsCCVC,"CCVC")==0)
									{
										if((tc_strcmp(FunPrtDRst,"AR5")==0)||(tc_strcmp(FunPrtDRst,"DR5")==0))
										{
											printf("\n C4:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
											OutPutTag= Funrev_list[funii];
											FDRChkFlag=1;
											break;
										}
									}
									else
									{
										if((tc_strcmp(FunPrtDRst,"AR4")==0)||(tc_strcmp(FunPrtDRst,"DR4")==0)||(tc_strcmp(FunPrtDRst,"AR5")==0)||(tc_strcmp(FunPrtDRst,"DR5")==0))
										{
											printf("\n P4:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
											OutPutTag= Funrev_list[funii];
											FDRChkFlag=1;
											break;
										}
									}
								}
								else
								{
									printf("\n P6:No Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
									OutPutTag= Funrev_list[funii];
								}
							}
						}
						if (FDRChkFlag >0)
						{
							printf("\n P5:Valid DR-status part tag found:%s.", InputPart);fflush(stdout);
							break;
						}
					}
				}
			}
		}
	}
	printf("\n P7:FDRChkFlag:%d.", FDRChkFlag);fflush(stdout);
	if (FDRChkFlag ==0)
	{
		printf(" Tag for part no:%s.", InputPart);fflush(stdout);
		if(ITEM_ask_latest_rev (OutPutTag, &OutPutTagg)!=ITK_ok)PrintErrorStack();
		if(OutPutTagg==NULLTAG)
		{
			printf("\n P8:OutPutTagg not found in Teamcenter...!!\n");fflush(stdout);
		}
		else
		{
			printf("\n P8:OutPutTagg Objectfound for ");fflush(stdout);
			OutPutTag= OutPutTagg;
			if(AOM_UIF_ask_value(OutPutTag, "item_id", &sPartno)!=ITK_ok)PrintErrorStack();
			printf("sPartno:%s.", sPartno);fflush(stdout);
		}
	}

	return OutPutTag;
}

//PO CHECK
int tm_CheckPOAvalability(tag_t PartObj)
{
	int POAvailblFlag =0;
	char*	 sPartPO = NULL;
	char*	 sPartStk = NULL;
	char*	 sPartTrz = NULL;
	char*	 sPartBlk = NULL;

	if (POAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO1",&sPartPO)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Stock1",&sPartStk)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Transaction1",&sPartTrz)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_BulkMaterial1",&sPartBlk)!=ITK_ok)   PrintErrorStack();
		//printf("\n P1:Plant1:CARPLANT:1100:sPartPO:%s sPartStk:%s sPartTrz:%s sPartBlk:%s",sPartPO,sPartStk,sPartTrz,sPartBlk);	fflush(stdout);	
		//printf("\n P2:Plant1:CARPLANT:1100:P1:");fflush(stdout);	
		if((strlen(sPartBlk) <= 0) || (sPartBlk==NULL)|| (tc_strcmp(sPartBlk,"")==0))
		{
			//printf(" No Bulk.");fflush(stdout);
			if(strlen(sPartStk) < 2)
			{
				//printf(" No stock.");fflush(stdout);
				if((strlen(sPartTrz) <= 0)|| (sPartTrz==NULL)|| (tc_strcmp(sPartTrz,"")==0))
				{
					//printf(" No transaction.");fflush(stdout);
					if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
					{
						//printf(" No PO.\n");fflush(stdout);
					}
					else
					{
						//printf(" PO avail.\n");fflush(stdout);
						POAvailblFlag=1;
					}
				}
				else
				{
					//printf(" transaction avail.\n");fflush(stdout);
					POAvailblFlag=1;
				}
			}
			else
			{
				//printf(" stock avail.\n");fflush(stdout);
				POAvailblFlag=1;
			}
		}
		else
		{
			//printf(" Bulk avail.\n");fflush(stdout);
			POAvailblFlag=1;
		}
	}
	if (POAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO8",&sPartPO)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Stock8",&sPartStk)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Transaction8",&sPartTrz)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_BulkMaterial8",&sPartBlk)!=ITK_ok)   PrintErrorStack();
		//printf("\n P3:Plant8:JAMSHEDPUR:2001:sPartPO:%s sPartStk:%s sPartTrz:%s sPartBlk:%s",sPartPO,sPartStk,sPartTrz,sPartBlk);	fflush(stdout);	
		//printf("\n P4:Plant8:JAMSHEDPUR:2001:P1:");fflush(stdout);	
		if((strlen(sPartBlk) <= 0) || (sPartBlk==NULL)|| (tc_strcmp(sPartBlk,"")==0))
		{
			//printf(" No Bulk.");fflush(stdout);
			if(strlen(sPartStk) < 2)
			{
				//printf(" No stock.");fflush(stdout);
				if((strlen(sPartTrz) <= 0)|| (sPartTrz==NULL)|| (tc_strcmp(sPartTrz,"")==0))
				{
					//printf(" No transaction.");fflush(stdout);
					if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
					{
						//printf(" No PO.\n");fflush(stdout);
					}
					else
					{
						//printf(" PO avail.\n");fflush(stdout);
						POAvailblFlag=1;
					}
				}
				else
				{
					//printf(" transaction avail.\n");fflush(stdout);
					POAvailblFlag=1;
				}
			}
			else
			{
				//printf(" stock avail.\n");fflush(stdout);
				POAvailblFlag=1;
			}
		}
		else
		{
			//printf(" Bulk avail.\n");fflush(stdout);
			POAvailblFlag=1;
		}
	}
	if (POAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO9",&sPartPO)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Stock9",&sPartStk)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Transaction9",&sPartTrz)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_BulkMaterial9",&sPartBlk)!=ITK_ok)   PrintErrorStack();
		//printf("\n P5:Plant9:LUCKNOW:3001:sPartPO:%s sPartStk:%s sPartTrz:%s sPartBlk:%s",sPartPO,sPartStk,sPartTrz,sPartBlk);	fflush(stdout);	
		//printf("\n P6:Plant9:LUCKNOW:3001:P9:");fflush(stdout);	
		if((strlen(sPartBlk) <= 0) || (sPartBlk==NULL)|| (tc_strcmp(sPartBlk,"")==0))
		{
			//printf(" No Bulk.");fflush(stdout);
			if(strlen(sPartStk) < 2)
			{
				//printf(" No stock.");fflush(stdout);
				if((strlen(sPartTrz) <= 0)|| (sPartTrz==NULL)|| (tc_strcmp(sPartTrz,"")==0))
				{
					//printf(" No transaction.");fflush(stdout);
					if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
					{
						//printf(" No PO.\n");fflush(stdout);
					}
					else
					{
						//printf(" PO avail.\n");fflush(stdout);
						POAvailblFlag=1;
					}
				}
				else
				{
					//printf(" transaction avail.\n");fflush(stdout);
					POAvailblFlag=1;
				}
			}
			else
			{
				//printf(" stock avail.\n");fflush(stdout);
				POAvailblFlag=1;
			}
		}
		else
		{
			//printf(" Bulk avail.\n");fflush(stdout);
			POAvailblFlag=1;
		}
	}
	if (POAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO6",&sPartPO)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Stock6",&sPartStk)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Transaction6",&sPartTrz)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_BulkMaterial6",&sPartBlk)!=ITK_ok)   PrintErrorStack();
		//printf("\n P7:Plant6:DHARWAD:1500:sPartPO:%s sPartStk:%s sPartTrz:%s sPartBlk:%s",sPartPO,sPartStk,sPartTrz,sPartBlk);	fflush(stdout);	
			//printf("\n P8:Plant6:DHARWAD:1500:P6:");fflush(stdout);	
		if((strlen(sPartBlk) <= 0) || (sPartBlk==NULL)|| (tc_strcmp(sPartBlk,"")==0))
		{
			//printf(" No Bulk.");fflush(stdout);
			if(strlen(sPartStk) < 2)
			{
				//printf(" No stock.");fflush(stdout);
				if((strlen(sPartTrz) <= 0)|| (sPartTrz==NULL)|| (tc_strcmp(sPartTrz,"")==0))
				{
					//printf(" No transaction.");fflush(stdout);
					if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
					{
						//printf(" No PO.\n");fflush(stdout);
					}
					else
					{
						//printf(" PO avail.\n");fflush(stdout);
						POAvailblFlag=1;
					}
				}
				else
				{
					//printf(" transaction avail.\n");fflush(stdout);
					POAvailblFlag=1;
				}
			}
			else
			{
				//printf(" stock avail.\n");fflush(stdout);
				POAvailblFlag=1;
			}
		}
		else
		{
			//printf(" Bulk avail.\n");fflush(stdout);
			POAvailblFlag=1;
		}
	}
	if (POAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO5",&sPartPO)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Stock5",&sPartStk)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Transaction5",&sPartTrz)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_BulkMaterial5",&sPartBlk)!=ITK_ok)   PrintErrorStack();
		//printf("\n P9:Plant4:PANTNAGAR:sPartPO:%s sPartStk:%s sPartTrz:%s sPartBlk:%s",sPartPO,sPartStk,sPartTrz,sPartBlk);	fflush(stdout);	
			//printf("\n P10:Plant4:PANTNAGAR:P4:");fflush(stdout);	
		if((strlen(sPartBlk) <= 0) || (sPartBlk==NULL)|| (tc_strcmp(sPartBlk,"")==0))
		{
			//printf(" No Bulk.");fflush(stdout);
			if(strlen(sPartStk) < 2)
			{
				//printf(" No stock.");fflush(stdout);
				if((strlen(sPartTrz) <= 0)|| (sPartTrz==NULL)|| (tc_strcmp(sPartTrz,"")==0))
				{
					//printf(" No transaction.");fflush(stdout);
					if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
					{
						//printf(" No PO.\n");fflush(stdout);
					}
					else
					{
						//printf(" PO avail.\n");fflush(stdout);
						POAvailblFlag=1;
					}
				}
				else
				{
					//printf(" transaction avail.\n");fflush(stdout);
					POAvailblFlag=1;
				}
			}
			else
			{
				//printf(" stock avail.\n");fflush(stdout);
				POAvailblFlag=1;
			}
		}
		else
		{
			//printf(" Bulk avail.\n");fflush(stdout);
			POAvailblFlag=1;
		}
	}
	if (POAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO3",&sPartPO)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Stock3",&sPartStk)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Transaction3",&sPartTrz)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_BulkMaterial3",&sPartBlk)!=ITK_ok)   PrintErrorStack();
		//printf("\n P11:Plant3:AHMEDABAD:7501:sPartPO:%s sPartStk:%s sPartTrz:%s sPartBlk:%s",sPartPO,sPartStk,sPartTrz,sPartBlk);	fflush(stdout);
		//printf("\n P12:Plant3:AHMEDABAD:7501:P3:");fflush(stdout);	
		if((strlen(sPartBlk) <= 0) || (sPartBlk==NULL)|| (tc_strcmp(sPartBlk,"")==0))
		{
			//printf(" No Bulk.");fflush(stdout);
			if(strlen(sPartStk) < 2)
			{
				//printf(" No stock.");fflush(stdout);
				if((strlen(sPartTrz) <= 0)|| (sPartTrz==NULL)|| (tc_strcmp(sPartTrz,"")==0))
				{
					//printf(" No transaction.");fflush(stdout);
					if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
					{
						//printf(" No PO.\n");fflush(stdout);
					}
					else
					{
						//printf(" PO avail.\n");fflush(stdout);
						POAvailblFlag=1;
					}
				}
				else
				{
					//printf(" transaction avail.\n");fflush(stdout);
					POAvailblFlag=1;
				}
			}
			else
			{
				//printf(" stock avail.\n");fflush(stdout);
				POAvailblFlag=1;
			}
		}
		else
		{
			//printf(" Bulk avail.\n");fflush(stdout);
			POAvailblFlag=1;
		}
	}
	if (POAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO2",&sPartPO)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Stock2",&sPartStk)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Transaction2",&sPartTrz)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_BulkMaterial2",&sPartBlk)!=ITK_ok)   PrintErrorStack();
		//printf("\n P13:Plant2:PCVBU:1001:sPartPO:%s sPartStk:%s sPartTrz:%s sPartBlk:%s",sPartPO,sPartStk,sPartTrz,sPartBlk);	fflush(stdout);	
		//printf("\n P14:Plant2:PCVBU:1001:P2:");fflush(stdout);	
		if((strlen(sPartBlk) <= 0) || (sPartBlk==NULL)|| (tc_strcmp(sPartBlk,"")==0))
		{
			//printf(" No Bulk.");fflush(stdout);
			if(strlen(sPartStk) < 2)
			{
				//printf(" No stock.");fflush(stdout);
				if((strlen(sPartTrz) <= 0)|| (sPartTrz==NULL)|| (tc_strcmp(sPartTrz,"")==0))
				{
					//printf(" No transaction.");fflush(stdout);
					if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
					{
						//printf(" No PO.\n");fflush(stdout);
					}
					else
					{
						//printf(" PO avail.\n");fflush(stdout);
						POAvailblFlag=1;
					}
				}
				else
				{
					//printf(" transaction avail.\n");fflush(stdout);
					POAvailblFlag=1;
				}
			}
			else
			{
				//printf(" stock avail.\n");fflush(stdout);
				POAvailblFlag=1;
			}
		}
		else
		{
			//printf(" Bulk avail.\n");fflush(stdout);
			POAvailblFlag=1;
		}
	}
	if (POAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO11",&sPartPO)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Stock11",&sPartStk)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_Transaction11",&sPartTrz)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(PartObj,"t5_BulkMaterial11",&sPartBlk)!=ITK_ok)   PrintErrorStack();
		//printf("\n P16:Plant11:PUVBU:1140:sPartPO:%s sPartStk:%s sPartTrz:%s sPartBlk:%s ",sPartPO,sPartStk,sPartTrz,sPartBlk);	fflush(stdout);	
		//printf("\n P17:Plant11:PUVBU:1140:P11:");fflush(stdout);	
		if((strlen(sPartBlk) <= 0) || (sPartBlk==NULL)|| (tc_strcmp(sPartBlk,"")==0))
		{
			//printf(" No Bulk.");fflush(stdout);
			if(strlen(sPartStk) < 2)
			{
				//printf(" No stock.");fflush(stdout);
				if((strlen(sPartTrz) <= 0)|| (sPartTrz==NULL)|| (tc_strcmp(sPartTrz,"")==0))
				{
					//printf(" No transaction.");fflush(stdout);
					if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
					{
						//printf(" No PO.\n");fflush(stdout);
					}
					else
					{
						//printf(" PO avail.\n");fflush(stdout);
						POAvailblFlag=1;
					}
				}
				else
				{
					//printf(" transaction avail.\n");fflush(stdout);
					POAvailblFlag=1;
				}
			}
			else
			{
				//printf(" stock avail.\n");fflush(stdout);
				POAvailblFlag=1;
			}
		}
		else
		{
			//printf(" Bulk avail.\n");fflush(stdout);
			POAvailblFlag=1;
		}
	}

	if (POAvailblFlag >0)
	{
		printf("\n PO:PO check PASSED for part.  \n");fflush(stdout);
		return 1;
	}
	else
	{
		printf("\n P18:PO check FAILED for part.  \n");fflush(stdout);
		return 0;
	}

	return 0;	
}

//Check colour part PO.
int tm_CheckColPrtPO(char* nonColPartno)
{
	int     cl      =  0;
	int     jjclr      =  0;
	int     clr      =  0;
	int     ii      =  0;
	int     col      =  0;
	int     colRev_count      =  0;
	int     ColPOAvail      =  0;
	int     ColPOAvailFlag      =  0;
	int     Clr_Rel_Stus_Cunt      =  0;
	int     Colsec_count      =  0;
	int     Colcount      =  0;
	char*	 Colsec_obj_type = NULL;
	char*	 Col_Rel_Stus = NULL;
	char*	 Col_Prtno = NULL;
	char*	 NonColPrt = NULL;
	char*	 NonColDR = NULL;
	char*	 NonColPrttp = NULL;
	char*	 NonColRev = NULL;
	tag_t   ColPartObjm     = NULLTAG;
	tag_t   ColPP_tag     = NULLTAG;
	tag_t   ColPartObjj     = NULLTAG;
	tag_t   ColP_tag     = NULLTAG;
	tag_t   ColPrt_tagg     = NULLTAG;
	tag_t   *ColPrt_tag     = NULLTAG;
	tag_t   Col_Prt_rel_type     = NULLTAG;
	tag_t   *Colsec_obj     = NULLTAG;
	tag_t   *Col_status_lstt     = NULLTAG;
	tag_t   *NRnopoPrtTag     = NULLTAG;
	tag_t	*ColPart_tag		= NULL;

	printf("\n Col1:tm_CheckColPrtPO for part:%s\n",nonColPartno);fflush(stdout);
	//ColPP_tag= t5GetItemRevison(nonColPartno);
	ColPrt_tagg= t5GetItemRevisonForRelRev(nonColPartno,PARTTYPEs);
	if(ColPrt_tagg==NULLTAG)
	{
		printf("\n Col2:ColPP_tag is NULL.\n");fflush(stdout);
	}
	else
	{
		printf("\n Col4:..");fflush(stdout);
		if(AOM_ask_value_string(ColPrt_tagg,"item_id",&NonColPrt)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(ColPrt_tagg,"t5_PartType",&NonColPrttp)!=ITK_ok)   PrintErrorStack();
		if(AOM_UIF_ask_value(ColPrt_tagg, "item_revision_id", &NonColRev)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(ColPrt_tagg,"t5_PartStatus",&NonColDR)!=ITK_ok)PrintErrorStack();
		printf("non col:[%s] type:[%s] Rev:[%s] DR:[%s]",NonColPrt,NonColPrttp,NonColRev,NonColDR);	fflush(stdout);	
		//go to colour part and check if any of the colour part having PO.
		if(GRM_find_relation_type("TC_Is_Represented_By", &Col_Prt_rel_type)!=ITK_ok)PrintErrorStack();
		if (Col_Prt_rel_type!=NULLTAG)
		{
			///if(GRM_list_secondary_objects_only(ColPrt_tagg,Col_Prt_rel_type,&Colcount,&ColPart_tag)!=ITK_ok)PrintErrorStack();
			if(GRM_list_primary_objects_only(ColPrt_tagg,Col_Prt_rel_type,&Colcount,&ColPart_tag)!=ITK_ok)PrintErrorStack();
			printf("\n Col5:Colcount: %d",Colcount);	fflush(stdout);	
			ColPOAvail =0;
			for(col=0; col<Colcount; col++)
			{
				if(ColPartObjm) ColPartObjm=NULLTAG;
				ColPartObjm=ColPart_tag[col];
				if(AOM_ask_value_string(ColPartObjm,"item_id",&Col_Prtno)!=ITK_ok)   PrintErrorStack();
				printf("\n Col6:Col_Prtno:%s\n",Col_Prtno);fflush(stdout);
				ColP_tag= t5GetItemRevison(Col_Prtno);
				if(ColP_tag==NULLTAG)
				{
					printf("\n Col7:ColP_tag is NULL.\n");fflush(stdout);
				}
				else
				{
					//if(ITEM_ask_latest_rev (ColP_tag, &ColPrt_tag)!=ITK_ok)PrintErrorStack();
					if(ITEM_list_all_revs (ColP_tag, &colRev_count, &ColPrt_tag)!=ITK_ok)   PrintErrorStack();
					printf("\n Col3:colRev_count.: %d.", colRev_count);fflush(stdout);
					if(colRev_count > 0)
					{
						for(cl=colRev_count-1;cl>=0;cl--)
						{
							Clr_Rel_Stus_Cunt=0;
							if(WSOM_ask_release_status_list (ColPrt_tag[cl], &Clr_Rel_Stus_Cunt, &Col_status_lstt)!=ITK_ok)PrintErrorStack();
							printf("\n Col4:Clr_Rel_Stus_Cunt is:%d.", Clr_Rel_Stus_Cunt);fflush(stdout);
							if(Clr_Rel_Stus_Cunt > 0)
							{
								clr=0;
								for(clr=0;clr<Clr_Rel_Stus_Cunt;clr++)
								{
									//POAvailFlag=0;
									//Checking PO availability on any revision.
									if(AOM_ask_value_string (Col_status_lstt[clr], "object_name", &Col_Rel_Stus)!=ITK_ok)PrintErrorStack();
									printf("\n Col4:Col_Rel_Stus is:%s.", Col_Rel_Stus);fflush(stdout);
									if(tc_strstr(Col_Rel_Stus,"T5_LcsErcRlzd")!=NULL)
									{
										Colsec_count=0;
										if(GRM_list_secondary_objects_only(ColPrt_tag[cl], NULLTAG, &Colsec_count, &Colsec_obj)!=ITK_ok)   PrintErrorStack();
										printf("\n Col5:Colsec_count: %d",Colsec_count);	fflush(stdout);	
										for(jjclr=0; jjclr<Colsec_count; jjclr++)
										{
											//POAvailFlag=0;
											if(ColPartObjj) ColPartObjj=NULLTAG;
											ColPartObjj=Colsec_obj[jjclr];
											if(AOM_UIF_ask_value(ColPartObjj, "object_type", &Colsec_obj_type)!=ITK_ok)   PrintErrorStack();
											//printf("\n PO:...Colsec_obj_type:%s", Colsec_obj_type);fflush(stdout);
											if((tc_strcmp(Colsec_obj_type, "Design Revision Master") == 0)||(tc_strcmp(Colsec_obj_type, "EE Part Revision Master") == 0)||(tc_strcmp(Colsec_obj_type, "Clr Part Revision Master") == 0))
											{
												printf("\n Col6:...Colsec_obj_type:%s", Colsec_obj_type);fflush(stdout);
												ColPOAvailFlag =0;
												ColPOAvailFlag = tm_CheckPOAvalability(ColPartObjj);
												printf("\n Col:ColPOAvailFlag.: %d",ColPOAvailFlag);fflush(stdout);
												if(ColPOAvailFlag >0)
												{
													//PO Available so break.
													if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
													{
														tc_strcpy (POAvailPartSet,"" );
														tc_strcat (POAvailPartSet,Col_Prtno);
														printf("\n Col:Part added to POAvailPartSet: %s",Col_Prtno);	fflush(stdout);	
													}
													else
													{
														if(strstr(POAvailPartSet,Col_Prtno)==NULL)
														{
															tc_strcat (POAvailPartSet,"," );
															tc_strcat (POAvailPartSet,Col_Prtno);
															printf("\n Col:Part added to POAvailPartSet: %s",Col_Prtno);	fflush(stdout);
														}
													}
													printf("\n Col:PO availale for part:%s ",Col_Prtno);	fflush(stdout);
													ColPOAvail =1;
													return 1;
												} 
											}
										}
									}
								}
							}
						}
						printf("\n Col:Objectfound!!\n");fflush(stdout);
					}
				} 
			}
			if (ColPOAvail >0)
			{
				return 1;
			}
		}
		
	}

	return 0;	
}


//REVISION LEVEL PO CHECK
int tm_CheckPORevAvalability(tag_t PartObj)
{
	int PORevAvailblFlag =0;
	char*	 sPartPO = NULL;

	if (PORevAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO1",&sPartPO)!=ITK_ok)   PrintErrorStack();
		printf("\n PORev:Plant1:CARPLANT:1100:sPartPO:%s",sPartPO); fflush(stdout);	
		if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
		{
			printf(" No PORev.\n");fflush(stdout);
		}
		else
		{
			printf(" PORev avail.\n");fflush(stdout);
			PORevAvailblFlag=1;
		}
	}
	if (PORevAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO8",&sPartPO)!=ITK_ok)   PrintErrorStack();
		printf("\n PORev:Plant8:JAMSHEDPUR:2001:sPartPO:%s",sPartPO); fflush(stdout);	
		if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
		{
			printf(" No PORev.\n");fflush(stdout);
		}
		else
		{
			printf(" PORev avail.\n");fflush(stdout);
			PORevAvailblFlag=1;
		}
	}
	if (PORevAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO9",&sPartPO)!=ITK_ok)   PrintErrorStack();
		printf("\n PORev:Plant9:LUCKNOW:3001:sPartPO:%s",sPartPO);	fflush(stdout);	
		if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
		{
			printf(" No PORev.\n");fflush(stdout);
		}
		else
		{
			printf(" PORev avail.\n");fflush(stdout);
			PORevAvailblFlag=1;
		}
	}
	if (PORevAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO6",&sPartPO)!=ITK_ok)   PrintErrorStack();
		printf("\n PORev:Plant6:DHARWAD:1500:sPartPO:%s ",sPartPO);	fflush(stdout);	
		if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
		{
			printf(" No PORev.\n");fflush(stdout);
		}
		else
		{
			printf(" PORev avail.\n");fflush(stdout);
			PORevAvailblFlag=1;
		}
	}
	if (PORevAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO5",&sPartPO)!=ITK_ok)   PrintErrorStack();
		printf("\n PORev:Plant4:PANTNAGAR:sPartPO:%s ",sPartPO);	fflush(stdout);	
		if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
		{
			printf(" No PORev.\n");fflush(stdout);
		}
		else
		{
			printf(" PORev avail.\n");fflush(stdout);
			PORevAvailblFlag=1;
		}
	}
	if (PORevAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO3",&sPartPO)!=ITK_ok)   PrintErrorStack();
		printf("\n PORev:Plant3:AHMEDABAD:7501:sPartPO:%s ",sPartPO);	fflush(stdout);
		if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
		{
			printf(" No PORev.\n");fflush(stdout);
		}
		else
		{
			printf(" PORev avail.\n");fflush(stdout);
			PORevAvailblFlag=1;
		}
	}
	if (PORevAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO2",&sPartPO)!=ITK_ok)   PrintErrorStack();
		printf("\n PORev:Plant2:PCVBU:1001:sPartPO:%s ",sPartPO); fflush(stdout);	
		if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
		{
			printf(" No PORev.\n");fflush(stdout);
		}
		else
		{
			printf(" PORev avail.\n");fflush(stdout);
			PORevAvailblFlag=1;
		}
	}
	if (PORevAvailblFlag ==0)
	{
		if(AOM_ask_value_string(PartObj,"t5_PO11",&sPartPO)!=ITK_ok)   PrintErrorStack();
		printf("\n PORev:Plant11:PUVBU:1140:sPartPO:%s ",sPartPO);	fflush(stdout);	
		if((strlen(sPartPO) <= 0)||(sPartPO==NULL)|| (tc_strcmp(sPartPO,"")==0))
		{
			printf(" No PORev.\n");fflush(stdout);
		}
		else
		{
			printf(" PORev avail.\n");fflush(stdout);
			PORevAvailblFlag=1;
		}
	}

	if (PORevAvailblFlag >0)
	{
		printf("\n PORev:PO check PASSED for part.  \n");fflush(stdout);
		return 1;
	}
	else
	{
		printf("\n PORev:PO check FAILED for part.  \n");fflush(stdout);
		return 0;
	}

	return 0;	
}

int tm_CheckNewCretNOPO(char* nopoPartno,char* PartRev)
{
	int sdayI=0;
	int sMonthI=0;
	int sYearI=0;
	int sHurI=0;
	int sMinI=0;
	int iip=0;
	int part_count=0;
	char	*sday		= NULL;
	char	*sMonth		= NULL;
	char	*sYear		= NULL;
	char	*sHur		= NULL;
	char	*sMin		= NULL;
	char	*Pnob		= NULL;
	char	*CreDtItem		= NULL;
	char	*P_rev_idd		= NULL;
	tag_t	Part_tgs	= NULLTAG;
	int     PO_rsltCount      =  0;
	tag_t   POqryTag     = NULLTAG;
	tag_t   *PartRev_list     = NULLTAG;

	printf("\n P101:Part taken for checking creation date:%s:%s\n",nopoPartno,PartRev);fflush(stdout);

	Part_tgs= t5GetItemRevison(nopoPartno);
	if(Part_tgs==NULLTAG)
	{
		printf("\n P108:Part_tgs is NULL..\n");fflush(stdout);
	}
	else
	{
		printf("\n P109:Part_tgs is found..\n");fflush(stdout);
		if(ITEM_list_all_revs (Part_tgs, &part_count, &PartRev_list)!=ITK_ok)   PrintErrorStack();
		printf("\n P110:Total rev found.: %d.", part_count);fflush(stdout);
		if(part_count > 0)
		{
			iip=0;
			for(iip=part_count-1;iip>=0;iip--)
			{
				if(AOM_ask_value_string(PartRev_list[iip],"item_id",&Pnob)!=ITK_ok)   PrintErrorStack();
				if(AOM_UIF_ask_value (PartRev_list[iip], "item_revision_id", &P_rev_idd)!=ITK_ok)PrintErrorStack();
				printf("\n P111:Pnob:%s item_revision_id:%s.",Pnob,P_rev_idd);fflush(stdout);
				if (tc_strcmp(PartRev,P_rev_idd)==0)
				{
					printf("\n P112:Rev found..\n");fflush(stdout);
					if(AOM_UIF_ask_value(PartRev_list[iip],"creation_date",&CreDtItem)!=ITK_ok)   PrintErrorStack();
					printf("\n P113:CreDtItem:%s",CreDtItem);fflush(stdout);
					//18-Dec-2019 11:23
					sday = strtok(CreDtItem,"-");
					sMonth = strtok(NULL,"-");
					sYear = strtok(NULL," ");
					sHur = strtok(NULL,":");
					sMin = strtok(NULL,"");
					printf("\n P104:%s :%s :%s :%s :%s  \n",sday,sMonth,sYear,sHur,sMin);fflush(stdout);
					sdayI = atoi(sday);
					//sMonthI = atoi(sMonth);
					sYearI = atoi(sYear);
					sHurI = atoi(sHur);
					sMinI = atoi(sMin);
					printf("\n P105:%d :%d :%d :%d :%d  \n",sdayI,sMonthI,sYearI,sHurI,sMinI);fflush(stdout);
					//if (tc_strcmp(sYear,"2019")==0)
					if(sYearI < 2019)
					{
						printf("\n P103:Part created before 1 jan 2019:%s \n",nopoPartno);fflush(stdout);
						return 0;
					}
					else if ((sYearI == 2019)||(sYearI > 2019))
					{
						printf("\n P102:Part created after 1 jan 2019:%s \n",nopoPartno);fflush(stdout);
						return 1;
					}
					else
					{
						printf("\n P106:Part created after 1 jan 2019:%s \n",nopoPartno);fflush(stdout);
						return 1;
					}
					break;
				}
			}
		}
	}

	return 0;	
}

//Check Umnatured part:
int tm_CheckUnMatPart(char* PartDR_stus)
{
	printf("\n U1:Inside tm_CheckUnMatPart.");	fflush(stdout);
	if((PartDR_stus==NULL)|| (tc_strcmp(PartDR_stus,"")==0)||(RelDRStatus==NULL)|| (tc_strcmp(RelDRStatus,"")==0))
	{
		printf("\n U1:ERROR: DR-status of part or DML is NULL.");	fflush(stdout);
		return 1;
	}
	else
	{
		printf("\n U1:PartDR_stus:%s RelDRStatus:%s", PartDR_stus,RelDRStatus);fflush(stdout);
		if ((tc_strcmp(RelDRStatus,"DR0")==0) ||(tc_strcmp(RelDRStatus,"AR0")==0))
		{
			if ((tc_strcmp(PartDR_stus,"DR0")==0) ||(tc_strcmp(PartDR_stus,"AR0")==0)|| (tc_strcmp(PartDR_stus,"DR1")==0) ||(tc_strcmp(PartDR_stus,"AR1")==0) || (tc_strcmp(PartDR_stus,"DR2")==0) ||(tc_strcmp(PartDR_stus,"AR2")==0)
				||(tc_strcmp(PartDR_stus,"DR3")==0) ||(tc_strcmp(PartDR_stus,"AR3")==0)||(tc_strcmp(PartDR_stus,"DR3P")==0) ||(tc_strcmp(PartDR_stus,"AR3P")==0)|| (tc_strcmp(PartDR_stus,"DR4")==0) ||(tc_strcmp(PartDR_stus,"AR4")==0)
				||(tc_strcmp(PartDR_stus,"DR5")==0) ||(tc_strcmp(PartDR_stus,"AR5")==0))
			{
				printf("\n U1:DR0:Maturated Part.");	fflush(stdout);
				return 0;
			}
			else
			{
				printf("\n U1:DR0:UnMaturated Part.");	fflush(stdout);
				return 1;
			}
		}
		else if ((tc_strcmp(RelDRStatus,"DR1")==0) ||(tc_strcmp(RelDRStatus,"AR1")==0))
		{
			if ((tc_strcmp(PartDR_stus,"DR1")==0) ||(tc_strcmp(PartDR_stus,"AR1")==0) || (tc_strcmp(PartDR_stus,"DR2")==0) ||(tc_strcmp(PartDR_stus,"AR2")==0)
				||(tc_strcmp(PartDR_stus,"DR3")==0) ||(tc_strcmp(PartDR_stus,"AR3")==0)||(tc_strcmp(PartDR_stus,"DR3P")==0) ||(tc_strcmp(PartDR_stus,"AR3P")==0)|| (tc_strcmp(PartDR_stus,"DR4")==0) ||(tc_strcmp(PartDR_stus,"AR4")==0)
				||(tc_strcmp(PartDR_stus,"DR5")==0) ||(tc_strcmp(PartDR_stus,"AR5")==0))
			{
				printf("\n U1:DR1:Maturated Part.");	fflush(stdout);
				return 0;
			}
			else
			{
				printf("\n U1:DR1:UnMaturated Part.");	fflush(stdout);
				return 1;
			}
		}
		else if ((tc_strcmp(RelDRStatus,"DR2")==0) ||(tc_strcmp(RelDRStatus,"AR2")==0))
		{
			if ((tc_strcmp(PartDR_stus,"DR2")==0) ||(tc_strcmp(PartDR_stus,"AR2")==0)
				||(tc_strcmp(PartDR_stus,"DR3")==0) ||(tc_strcmp(PartDR_stus,"AR3")==0)||(tc_strcmp(PartDR_stus,"DR3P")==0) ||(tc_strcmp(PartDR_stus,"AR3P")==0)|| (tc_strcmp(PartDR_stus,"DR4")==0) ||(tc_strcmp(PartDR_stus,"AR4")==0)
				||(tc_strcmp(PartDR_stus,"DR5")==0) ||(tc_strcmp(PartDR_stus,"AR5")==0))
			{
				printf("\n U1:DR2:Maturated Part.");	fflush(stdout);
				return 0;
			}
			else
			{
				printf("\n U1:DR2:UnMaturated Part.");	fflush(stdout);
				return 1;
			}
		}
		else if ((tc_strcmp(RelDRStatus,"DR3")==0) ||(tc_strcmp(RelDRStatus,"AR3")==0))
		{
			if ((tc_strcmp(PartDR_stus,"DR3")==0) ||(tc_strcmp(PartDR_stus,"AR3")==0)||(tc_strcmp(PartDR_stus,"DR3P")==0) ||(tc_strcmp(PartDR_stus,"AR3P")==0)|| (tc_strcmp(PartDR_stus,"DR4")==0) ||(tc_strcmp(PartDR_stus,"AR4")==0)
				||(tc_strcmp(PartDR_stus,"DR5")==0) ||(tc_strcmp(PartDR_stus,"AR5")==0))
			{
				printf("\n U1:DR3:Maturated Part.");	fflush(stdout);
				return 0;
			}
			else
			{
				printf("\n U1:DR3:UnMaturated Part.");	fflush(stdout);
				return 1;
			}
		}
		else if ((tc_strcmp(RelDRStatus,"DR3P")==0) ||(tc_strcmp(RelDRStatus,"AR3P")==0))
		{
			if ((tc_strcmp(PartDR_stus,"DR3P")==0) ||(tc_strcmp(PartDR_stus,"AR3P")==0)|| (tc_strcmp(PartDR_stus,"DR4")==0) ||(tc_strcmp(PartDR_stus,"AR4")==0) ||(tc_strcmp(PartDR_stus,"DR5")==0) ||(tc_strcmp(PartDR_stus,"AR5")==0))
			{
				printf("\n U1:DR3P:Maturated Part.");	fflush(stdout);
				return 0;
			}
			else
			{
				printf("\n U1:DR3P:UnMaturated Part.");	fflush(stdout);
				return 1;
			}
		}
		else if ((tc_strcmp(RelDRStatus,"DR4")==0) ||(tc_strcmp(RelDRStatus,"AR4")==0))
		{
			if ((tc_strcmp(PartDR_stus,"DR4")==0) ||(tc_strcmp(PartDR_stus,"AR4")==0) ||(tc_strcmp(PartDR_stus,"DR5")==0) ||(tc_strcmp(PartDR_stus,"AR5")==0))
			{
				printf("\n U1:DR4:Maturated Part.");	fflush(stdout);
				return 0;
			}
			else
			{
				printf("\n U1:DR4:UnMaturated Part.");	fflush(stdout);
				return 1;
			}
		}
		else if ((tc_strcmp(RelDRStatus,"DR5")==0) ||(tc_strcmp(RelDRStatus,"AR5")==0))
		{
			if ((tc_strcmp(PartDR_stus,"DR5")==0) ||(tc_strcmp(PartDR_stus,"AR5")==0))
			{
				printf("\n U1:DR5:Maturated Part.");	fflush(stdout);
				return 0;
			}
			else
			{
				printf("\n U1:DR5:UnMaturated Part.");	fflush(stdout);
				return 1;
			}
		}
		else
		{
			printf("\n U1:ERROR: Other status.");	fflush(stdout);
		}
	}

	return 0;	
}

int tm_POCheckStopAtFBOMline (tag_t Child_line_tag,char* sDml_TP,char* sEESWPrtType,char* ParnPrt,char* ParnRev)
{
	int ii=0;
	int crrr=0;
	int jj=0;
	int iii=0;
	int iCountB=0;
	int iCnt=0;
	int POfoundFlg=0;
	int ColPrtFlag=0;
	int sec_count=0;
	int rev_count=0;
	int CondiPassFlag=0;
	int FunOutflag=0;
	int DummyFlg=0;
	int RelRevFlag=0;
	int POAvailFlag=0;
	int GrpFunflag=0;
	int CSFoundFlag=0;
	int EEPartByPass=0;
	int F18CSFoundFlag=0;
	int EXPANSSIONFlag=0;
	int childCountBomLine=0;
	int NwNOPOPartFlag=0;
	int PORevAvailFlag=0;
	int MatFlag=0;
	int NwNOPOPrtFlg=0;
	int PartRelFlag=0;
	int UomFlag=0;
	int UmMatrFlag=0;
	int flgPartAtachedToTsk=0;
	int Chld_Rel_Stus_Cunt=0;
	int status = ITK_ok;
	char	*Child_Prt_Uom		= NULL;
	char	*sec_obj_type		= NULL;
	char	*Child_rev_idd		= NULL;
	char	*sNOPO		= NULL;
	char	*cPartCS		= NULL;
	char	*cPart_nob		= NULL;
	char	*cPart_DR		= NULL;
	char	*cPart_typ		= NULL;
	char	*cPartProj		= NULL;
	char	*cPartColInd		= NULL;
	char	*cpartDesgGrp		= NULL;
	char	*Chld_Rel_Stus		= NULL;
	char	*cChld_type		= NULL;
	char	*cpartDrwind		= NULL;
	char	*sPrtCod		= NULL;
	char	*sEEswTyp		= NULL;
	char	*cPartCls		= NULL;
	char	*cpartRev		= NULL;
	char	*PrtDRstt		= NULL;
	char	*rev_iddd		= NULL;
	char	*P_Typee		= NULL;
	char	*Part_nobrr		= NULL;
	char	*Child_Prt_DR		= NULL;
	char	*Child_Prt		= NULL;
	tag_t	tagBOMlineB 	= NULLTAG;
	tag_t	*tagTopBLchildLineB = NULLTAG;
	tag_t  	EEitem_tag =	NULLTAG;
	tag_t  	PartObjj =	NULLTAG;
	tag_t  	EEpart_tag =	NULLTAG;
	tag_t  	cClstag =	NULLTAG;
	tag_t	All_item_tgs	= NULLTAG;
	tag_t	Prt_tag	= NULLTAG;
	tag_t	P_tag	= NULLTAG;
	tag_t	*sec_obj		= NULL;
	tag_t	*Crevsn_list		= NULL;
	tag_t	*Child_status_lstt		= NULL;
	tag_t 	*childBoMLine 	= NULL;
	char **POrulename = NULL;
	char **POrulevalue = NULL;
	int		rulefound	= 0;
	tag_t	tagRuleB 	= NULLTAG;
	tag_t	tagBOMwindowB 	= NULLTAG;
	int		childCountTopBomLineB	= 0;
	tag_t *POclosurerule = NULLTAG;
	tag_t POclose_tag = NULLTAG;

	printf("\n P25:Inside CS check function:TP:%s\n",sDml_TP);fflush(stdout);
	if(AOM_ask_value_string(Child_line_tag,"bl_item_item_id",&cPart_nob)!=ITK_ok)   PrintErrorStack();
	if(AOM_ask_value_string(Child_line_tag,"bl_Design Revision_t5_PartStatus",&cPart_DR)!=ITK_ok)   PrintErrorStack();
	if(AOM_ask_value_string(Child_line_tag,"bl_Design Revision_t5_PartType",&cPart_typ)!=ITK_ok)   PrintErrorStack();
	if(AOM_ask_value_string(Child_line_tag,"bl_Design Revision_t5_ProjectCode",&cPartProj)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(Child_line_tag,"bl_Design Revision_t5_ColourInd",&cPartColInd)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(Child_line_tag,"bl_Design Revision_t5_DesignGrp",&cpartDesgGrp)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(Child_line_tag,"fnd0bl_line_object_type",&cChld_type)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(Child_line_tag,"bl_Design Revision_t5_DrawingInd",&cpartDrwind)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(Child_line_tag,"bl_rev_item_revision_id",&cpartRev)!=ITK_ok)PrintErrorStack();
	printf("\n P27:Parent part:[%s] Parent Rev:[%s] Child Part:[%s] cPart_DR:[%s] cPart_typ:[%s] cPartProj:[%s] cPartColInd:[%s] cpartDesgGrp:[%s] cChld_type:[%s] cpartDrwind:[%s] cpartRev:[%s]",ParnPrt,ParnRev,cPart_nob,cPart_DR,cPart_typ,cPartProj,cPartColInd,cpartDesgGrp,cChld_type,cpartDrwind,cpartRev);fflush(stdout);	

	//Query part and take class.
	printf("\n P26:t5GetItemRevison.. fun\n",cPart_nob,PARTTYPEs);fflush(stdout);
	Prt_tag= t5GetItemRevisonForRelRev(cPart_nob,PARTTYPEs);
	if(Prt_tag==NULLTAG)
	{
		printf("\n P26:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
		return 0;
	}
	else
	{
		printf("\n P26:EE:Objectfound!!\n");fflush(stdout);
		if(POM_class_of_instance(Prt_tag,&cClstag)!=ITK_ok)   PrintErrorStack();
		if(POM_name_of_class(cClstag,&cPartCls)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(Prt_tag,"item_id",&Part_nobrr)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(Prt_tag,"t5_PartType",&P_Typee)!=ITK_ok)   PrintErrorStack();
		if(AOM_UIF_ask_value(Prt_tag, "item_revision_id", &rev_iddd)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(Prt_tag,"t5_PartStatus",&PrtDRstt)!=ITK_ok)PrintErrorStack();
		printf("\n PO: Part_nobrr is : %s P_Typee:%s rev_iddd:%s PrtDRstt:%s cPartCls:%s ",Part_nobrr,P_Typee,rev_iddd,PrtDRstt,cPartCls);	fflush(stdout);	
	}
	
	//Skipping Dummy parts and spare kit
	if(tc_strcmp(cPart_typ,"D")==0 || tc_strcmp(cPart_typ,"DA")==0 ||tc_strcmp(cPart_typ,"DC")==0 || tc_strcmp(cPart_typ,"IFD")==0|| tc_strcmp(cPart_typ,"IM" )==0 ||(tc_strcmp(cPartCls,"t5SpKit")==0))
	{
		printf("\n P28:DUMMY part skipped for PO: %s",cPart_nob);	fflush(stdout);	
	}
	else
	{
		if (tc_strcmp(sDml_TP,"CARPLANT")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_CarMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P29:Plant1:CARPLANT:1100:cPartCS: %s",cPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_JsrMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P30:Plant8:JAMSHEDPUR:2001:cPartCS: %s",cPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_LkoMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P31:Plant9:LUCKNOW:3001:cPartCS: %s",cPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_JdlMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P32:DRIVELINES:cPartCS: %s",cPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_DwdMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P33:Plant6:DHARWAD:1500:cPartCS: %s",cPartCS);	fflush(stdout);		
		}
		else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_PnrMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P34:Plant4:PANTNAGAR:cPartCS: %s",cPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_AhdMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P35:Plant3:AHMEDABAD:7501:cPartCS: %s",cPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"PCVBU")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_PunMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P36:Plant2:PCVBU:1001:cPartCS: %s",cPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"PUVBU")==0)
		{
			if(AOM_ask_value_string(Prt_tag,"t5_PunUVMakeBuyIndicator",&cPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n P37:Plant11:PUVBU:1140:cPartCS: %s",cPartCS);	fflush(stdout);	
		}
		else
		{
			printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
		}
		if((cPartCS==NULL)|| (tc_strcmp(cPartCS,"")==0)||(tc_strcmp(cPartCS,"NA")==0))
		{
			printf("\n PO:target Plant CS is NULL so considered as NOPO: %s",cPart_nob);	fflush(stdout);	
			if((NOPOPartsSet==NULL)|| (tc_strcmp(NOPOPartsSet,"")==0))
			{
				tc_strcpy (NOPOPartsSet,"" );
				tc_strcat (NOPOPartsSet,cPart_nob);
				printf("\n P38:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);	
			}
			else
			{
				if(strstr(NOPOPartsSet,cPart_nob)==NULL)
				{
					tc_strcat (NOPOPartsSet,"," );
					tc_strcat (NOPOPartsSet,cPart_nob);
					printf("\n P39:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);
				}
			}
		}
		else
		{
			if(tc_strcmp(cPartCS,"F")==0 || tc_strcmp(cPartCS,"F30")==0|| tc_strcmp(cPartCS,"F35")==0 || tc_strcmp(cPartCS,"F40")==0)
			{
				printf("\n P41:F CS fonud.\n");fflush(stdout);
				CSFoundFlag=1;
			}
			else if (tc_strcmp(cPartCS,"F18")==0 || tc_strcmp(cPartCS,"F19")==0)
			{
				printf("\n P40:F18 CS fonud.\n");fflush(stdout);
				F18CSFoundFlag=1;
			}
			else
			{
				printf(" P42:Found Other CS.\n");fflush(stdout);
				CSFoundFlag=0;
				F18CSFoundFlag=0;
			}
			if (F18CSFoundFlag >0)
			{
				EXPANSSIONFlag=0;
				if((BypassPartSet==NULL)|| (tc_strcmp(BypassPartSet,"")==0))
				{
					tc_strcpy (BypassPartSet,"" );
					tc_strcat (BypassPartSet,cPart_nob);
					printf("\n P43:F18:Part added to bypassset: %s",cPart_nob);	fflush(stdout);	
				}
				else
				{
					if(strstr(BypassPartSet,cPart_nob)==NULL)
					{
						tc_strcat (BypassPartSet,"," );
						tc_strcat (BypassPartSet,cPart_nob);
						printf("\n P44:F18:Part added to bypassset: %s",cPart_nob);	fflush(stdout);
					}
				}
			}
			else if (CSFoundFlag >0)
			{
				DummyFlg=0;
				CondiPassFlag=0;
				EXPANSSIONFlag=0;
				printf("\n P45:.Checking conditions..:[%s]\n",cPart_nob);fflush(stdout);

				if(tc_strcmp(cPart_typ,"D")==0 || tc_strcmp(cPart_typ,"DA")==0 ||tc_strcmp(cPart_typ,"DC")==0 || tc_strcmp(cPart_typ,"IFD")==0|| tc_strcmp(cPart_typ,"IM" )==0 ||(tc_strcmp(cPartCls,"t5SpKit")==0))
				{
					printf("\n P46:.Obsolete/Dummy part skipped....:[%s]\n",cPart_nob);fflush(stdout);
					DummyFlg=1;				
				}
				else if((tc_strcmp(cPartProj,"1111")==0)||(tc_strcmp(cPartCls,"T5_EE_PartRevision")==0)||(tc_strcmp(cPart_typ,"G")==0)|| (tc_strcmp(cPartCls,"t5GrpID")==0)||((strlen(cPart_nob)!=11)&& ((tc_strcmp(cPartColInd,"Y")==0)||(tc_strcmp(cPartColInd,"N")==0)||(tc_strcmp(cPartColInd,"C")==0))&&((tc_strcmp(cPart_typ,"A")==0)||(tc_strcmp(cPart_typ,"C")==0)||(tc_strcmp(cPart_typ,"M")==0))))
				{
					printf("\n P47:Part eligible for PO check..:[%s]\n",cPart_nob);fflush(stdout);
					CondiPassFlag=1;
				}
				else
				{
					printf("\n P48:NO expanssion for DR3 and below CS-F parts:[%s]\n",cPart_nob);fflush(stdout);
					EXPANSSIONFlag=1;
				}
				printf("\n P49:DummyFlg:%d CondiPassFlag:%d EXPANSSIONFlag:%d\n",DummyFlg,CondiPassFlag,EXPANSSIONFlag);fflush(stdout);
				if(DummyFlg >0)
				{
					EXPANSSIONFlag=0;
					if((SkippedPartSet==NULL)|| (tc_strcmp(SkippedPartSet,"")==0))
					{
						tc_strcpy (SkippedPartSet,"" );
						tc_strcat (SkippedPartSet,cPart_nob);
						printf("\n P50:Dmy:Part added to SkippedPartSet: %s",cPart_nob);	fflush(stdout);	
					}
					else
					{
						if(strstr(SkippedPartSet,cPart_nob)==NULL)
						{
							tc_strcat (SkippedPartSet,"," );
							tc_strcat (SkippedPartSet,cPart_nob);
							printf("\n P51:Dmy:Part added to SkippedPartSet: %s",cPart_nob);	fflush(stdout);
						}
					}
				}
				else if (CondiPassFlag >0)
				{
					if ((tc_strcmp(cPart_typ,"G")==0)|| (tc_strcmp(cPartCls,"t5GrpID")==0))
					{
						EXPANSSIONFlag=0;
						GrpFunflag=0;
						//GrpFunflag = tm_POCheckStopAtFGrpBOMline(Child_line_tag,sDml_TP,sEESWPrtType);
						GrpFunflag = tm_POCheckStopAtFGrpBOMline(Prt_tag,sDml_TP,sEESWPrtType);
						printf("\n P52:GrpFunflag: %d",GrpFunflag);fflush(stdout);
						if(GrpFunflag >0)
						{
							printf("\n P54:Grp: PO failed.: %s cpartRev:%s",cPart_nob,cpartRev);	fflush(stdout);
							NwNOPOPrtFlg =0;
							NwNOPOPrtFlg = tm_CheckNewCretNOPO(cPart_nob,cpartRev);
							printf("\n P53:Grp:NwNOPOPrtFlg.: %d",NwNOPOPrtFlg);fflush(stdout);
							if(NwNOPOPrtFlg >0)
							{
								if((NwNOPOPartsSet==NULL)|| (tc_strcmp(NwNOPOPartsSet,"")==0))
								{
									tc_strcpy (NwNOPOPartsSet,"" );
									tc_strcat (NwNOPOPartsSet,cPart_nob);
									printf("\n P55:NwNOPOPartsSet. %s",cPart_nob);	fflush(stdout);	
								}
								else
								{
									if(strstr(NwNOPOPartsSet,cPart_nob)==NULL)
									{
										tc_strcat (NwNOPOPartsSet,"," );
										tc_strcat (NwNOPOPartsSet,cPart_nob);
										printf("\n P56:NwNOPOPartsSet.. %s",cPart_nob);	fflush(stdout);
									}
								}
							}
							else
							{
								if((NOPOPartsSet==NULL)|| (tc_strcmp(NOPOPartsSet,"")==0))
								{
									tc_strcpy (NOPOPartsSet,"" );
									tc_strcat (NOPOPartsSet,cPart_nob);
									printf("\n P57:Part added to NOPOPartsSet: %s",cPart_nob);	fflush(stdout);	
								}
								else
								{
									if(strstr(NOPOPartsSet,cPart_nob)==NULL)
									{
										tc_strcat (NOPOPartsSet,"," );
										tc_strcat (NOPOPartsSet,cPart_nob);
										printf("\n P58:Part added to NOPOPartsSet: %s",cPart_nob);	fflush(stdout);
									}
								}
							}
						}
						else
						{
							printf("\n P59:PO success.: %s",cPart_nob);	fflush(stdout);
							if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
							{
								tc_strcpy (POAvailPartSet,"" );
								tc_strcat (POAvailPartSet,cPart_nob);
								printf("\n P60:Part added to POAvailPartSet: %s",cPart_nob);	fflush(stdout);	
							}
							else
							{
								if(strstr(POAvailPartSet,cPart_nob)==NULL)
								{
									tc_strcat (POAvailPartSet,"," );
									tc_strcat (POAvailPartSet,cPart_nob);
									printf("\n P61:Part added to POAvailPartSet: %s",cPart_nob);	fflush(stdout);
								}
							}
						}
					}
					else
					{
						EEPartByPass=0;
						if((tc_strcmp(cPartCls,"T5_EE_PartRevision")==0)||(tc_strcmp(cChld_type,"T5_EE_PartRevision")==0)||(tc_strcmp(cChld_type,"EE Part Revision")==0))
						{
							//Query EE part and take Swft part type.
							EEpart_tag= t5GetItemRevisonForRelRev(cPart_nob,PARTTYPEs);
							if(EEpart_tag==NULLTAG)
							{
								printf("\n P62:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
								return 0;
							}
							else
							{
								if(AOM_ask_value_string(EEpart_tag,"t5_SwPartType",&sEEswTyp)!=ITK_ok)PrintErrorStack();
								printf("\n P63:EE:sEEswTyp:%s\n",sEEswTyp);fflush(stdout);
								if(strstr(sEESWPrtType,sEEswTyp)!=NULL)
								{
									printf("\n P64: Bypassing EE part for PO.\n");fflush(stdout);
									EEPartByPass=1;
									if((SkippedPartSet==NULL)|| (tc_strcmp(SkippedPartSet,"")==0))
									{
										tc_strcpy (SkippedPartSet,"" );
										tc_strcat (SkippedPartSet,cPart_nob);
										printf("\n P65:EE:Part added to SkippedPartSet: %s",cPart_nob);	fflush(stdout);	
									}
									else
									{
										if(strstr(SkippedPartSet,cPart_nob)==NULL)
										{
											tc_strcat (SkippedPartSet,"," );
											tc_strcat (SkippedPartSet,cPart_nob);
											printf("\n P66:EE:Part added to SkippedPartSet: %s",cPart_nob);	fflush(stdout);
										}
									}
								}
								else
								{
									EEPartByPass=0;
								}
							}
						}
						printf("\n P50:EE:EEPartByPass: %d",EEPartByPass);	fflush(stdout);	
						if (EEPartByPass ==0)
						{
							//bypass colour parts.
							if((strlen(cPart_nob)==14) && (tc_strcmp(cPartColInd,"C")==0))
							{
								printf("\n P65: Bypass 14 digit parts for PO:[%s].\n",cPart_nob);fflush(stdout);
							}
							else
							{
								//Owners Manual.
								printf("\n P66:Checkinging for Owners Manual:[%s : %s :%s].\n",cPart_nob,cpartDesgGrp,cPart_DR);fflush(stdout);
								/*if((tc_strcmp(cpartDesgGrp,"58")==0)&& ((tc_strcmp(cPart_DR,"DR4")==0)|| (tc_strcmp(cPart_DR,"AR4")==0)))
								{
									//MOHAN: Need to check for detailed logic.
									printf("\n PO: Owners Manual bypassed:[%s].\n",cPart_nob);fflush(stdout);
								}
								else
								{*/
									//Going to check PO details.
									//if(ITEM_find_item (cPart_nob, &All_item_tgs)!=ITK_ok)   PrintErrorStack();
									All_item_tgs= t5GetItemRevison(cPart_nob);
									if(All_item_tgs==NULLTAG)
									{
										printf("\n PO:All_item_tgs is NULL..\n");fflush(stdout);
									}
									else
									{
										printf("\n PO:All_item_tgs is found..\n");fflush(stdout);
										if(ITEM_list_all_revs (All_item_tgs, &rev_count, &Crevsn_list)!=ITK_ok)   PrintErrorStack();
										printf("\n P67:Total rev found.: %d.", rev_count);fflush(stdout);
										if(rev_count > 0)
										{
											MatFlag=0;
											//Checking for unmaturated part.A
											ii=0;
											PartRelFlag=0;
											for(ii=rev_count-1;ii>=0;ii--)
											{
												MatFlag=0;
												if(Child_Prt_DR) Child_Prt_DR=NULL;
												if(Child_Prt) Child_Prt=NULL;
												if(Child_rev_idd) Child_rev_idd=NULL;
												if(AOM_ask_value_string(Crevsn_list[ii],"item_id",&Child_Prt)!=ITK_ok)   PrintErrorStack();
												if(AOM_ask_value_string(Crevsn_list[ii],"t5_PartStatus",&Child_Prt_DR)!=ITK_ok)   PrintErrorStack();
												if(AOM_ask_value_string(Crevsn_list[ii],"t5_ProjectCode",&sPrtCod)!=ITK_ok)PrintErrorStack();
												if(AOM_UIF_ask_value (Crevsn_list[ii], "item_revision_id", &Child_rev_idd)!=ITK_ok)PrintErrorStack();
												//printf("\n P68:Child_Prt:%s Child_Prt_DR:%s Child_rev_idd:%s :%s.", Child_Prt,Child_Prt_DR,Child_rev_idd,sPrtCod);fflush(stdout);
												//if(AOM_ask_value_string(Crevsn_list[ii],"t5_ProjectCode",&sPrtCod)!=ITK_ok)PrintErrorStack();
												if (tc_strcmp(sPrtCod,"1111")==0)
												{
													MatFlag=1;
													PartRelFlag=1;
													printf("\n P682:Standard part skipped for unmat check:%s: %d :%d.", Child_Prt,MatFlag,PartRelFlag);fflush(stdout);
													break;
												}
												else if(strstr(TaskAtchedPartSet,Child_Prt)!=NULL)
												{
													MatFlag=1;
													PartRelFlag=1;
													printf("\n P681:Part is directly attached to DML.:%s: %d :%d.", Child_Prt,MatFlag,PartRelFlag);fflush(stdout);
													break;
												}
												else
												{
													Chld_Rel_Stus_Cunt=0;
													if(WSOM_ask_release_status_list (Crevsn_list[ii], &Chld_Rel_Stus_Cunt, &Child_status_lstt)!=ITK_ok)PrintErrorStack();
													printf("\n P69:Chld_Rel_Stus_Cunt is:%d.", Chld_Rel_Stus_Cunt);fflush(stdout);
													if(Chld_Rel_Stus_Cunt > 0)
													{
														crrr=0;
														for(crrr=0;crrr<Chld_Rel_Stus_Cunt;crrr++)
														{
															POAvailFlag=0;
															//Checking PO availability on any revision.
															if(AOM_ask_value_string (Child_status_lstt[crrr], "object_name", &Chld_Rel_Stus)!=ITK_ok)PrintErrorStack();
															if(tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)
															{
																PartRelFlag ++;
																printf("\n P68:Child_Prt:%s Child_Prt_DR:%s Child_rev_idd:%s :%s.", Child_Prt,Child_Prt_DR,Child_rev_idd,sPrtCod);fflush(stdout);
																printf("\n P70:P1:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																//if ((tc_strcmp(Child_Prt_DR,"DR3P")==0)|| (tc_strcmp(Child_Prt_DR,"DR4")==0)||(tc_strcmp(Child_Prt_DR,"DR5")==0)|| (tc_strcmp(Child_Prt_DR,"AR3P")==0)|| (tc_strcmp(Child_Prt_DR,"AR4")==0)||(tc_strcmp(Child_Prt_DR,"AR5")==0))
																UmMatrFlag =0;
																UmMatrFlag = tm_CheckUnMatPart(Child_Prt_DR);
																printf("\n P70:UnMat:UmMatrFlag.: %d",UmMatrFlag);fflush(stdout);
																if (UmMatrFlag ==0)
																{
																	MatFlag=1;
																	printf("\n P71:MatFlag flag set to 1:%d.", MatFlag);fflush(stdout);
																}
																break;
															}
														}
														if (MatFlag >0)
														{
															break;
														}
													}
												}
											}
											printf("\n P72:MatFlag:%d PartRelFlag:%d", MatFlag,PartRelFlag);fflush(stdout);
											//Checking for unmaturated part.B cpartRev 
											if((MatFlag >0)&&(PartRelFlag >0))
											{
												ii=0;
												for(ii=rev_count-1;ii>=0;ii--)
												{
													UomFlag =0;
													POAvailFlag=0;
													if(Child_Prt_DR) Child_Prt_DR=NULL;
													if(AOM_ask_value_string(Crevsn_list[ii],"item_id",&Child_Prt)!=ITK_ok)   PrintErrorStack();
													if(AOM_ask_value_string(Crevsn_list[ii],"t5_PartStatus",&Child_Prt_DR)!=ITK_ok)   PrintErrorStack();
													if(tc_strcmp(cPartCls,"T5_ClrPartRevision")!=0)
													{
														if(AOM_UIF_ask_value(Crevsn_list[ii],"t5_uom",&Child_Prt_Uom)!=ITK_ok)   PrintErrorStack();
													}
													if(AOM_UIF_ask_value (Crevsn_list[ii], "item_revision_id", &Child_rev_idd)!=ITK_ok)PrintErrorStack();
													printf("\n P73:Child_Prt:%s Child_Prt_DR:%s Child_rev_idd:%s UOM:%s.", Child_Prt,Child_Prt_DR,Child_rev_idd,Child_Prt_Uom);fflush(stdout);
													if((tc_strcmp(Child_Prt_Uom,"")==0)||(tc_strcmp(Child_Prt_Uom,"each")==0)||(tc_strcmp(Child_Prt_Uom,"4-Nos")==0))
													{
														Chld_Rel_Stus_Cunt=0;
														if(WSOM_ask_release_status_list (Crevsn_list[ii], &Chld_Rel_Stus_Cunt, &Child_status_lstt)!=ITK_ok)PrintErrorStack();
														//printf("\n P74:Chld_Rel_Stus_Cunt is:%d.", Chld_Rel_Stus_Cunt);fflush(stdout);
														if(Chld_Rel_Stus_Cunt > 0)
														{
															crrr=0;
															for(crrr=0;crrr<Chld_Rel_Stus_Cunt;crrr++)
															{
																POAvailFlag=0;
																//Checking PO availability on any revision.
																if(AOM_ask_value_string (Child_status_lstt[crrr], "object_name", &Chld_Rel_Stus)!=ITK_ok)PrintErrorStack();
																if(tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)
																{
																	printf("\n P75:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																	sec_count=0;
																	if(GRM_list_secondary_objects_only(Crevsn_list[ii], NULLTAG, &sec_count, &sec_obj)!=ITK_ok)   PrintErrorStack();
																	printf("\n P76:sec_count: %d",sec_count);	fflush(stdout);	
																	for(jj=0; jj<sec_count; jj++)
																	{
																		POAvailFlag=0;
																		if(PartObjj) PartObjj=NULLTAG;
																		PartObjj=sec_obj[jj];
																		if(AOM_UIF_ask_value(PartObjj, "object_type", &sec_obj_type)!=ITK_ok)   PrintErrorStack();
																		//printf("\n PO:...sec_obj_type:%s", sec_obj_type);fflush(stdout);
																		if((tc_strcmp(sec_obj_type, "Design Revision Master") == 0)||(tc_strcmp(sec_obj_type, "EE Part Revision Master") == 0)||(tc_strcmp(sec_obj_type, "Clr Part Revision Master") == 0))
																		{
																			printf("\n P77:...sec_obj_type:%s", sec_obj_type);fflush(stdout);
																			//PO availability Fun call
																			POAvailFlag =0;
																			POAvailFlag = tm_CheckPOAvalability(PartObjj);
																			printf("\n P78:POAvailFlag.: %d",POAvailFlag);fflush(stdout);
																			if(POAvailFlag >0)
																			{
																				//PO Available so break.
																				if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
																				{
																					tc_strcpy (POAvailPartSet,"" );
																					tc_strcat (POAvailPartSet,cPart_nob);
																					printf("\n P79:Part added to POAvailPartSet: %s",cPart_nob);	fflush(stdout);	
																				}
																				else
																				{
																					if(strstr(POAvailPartSet,cPart_nob)==NULL)
																					{
																						tc_strcat (POAvailPartSet,"," );
																						tc_strcat (POAvailPartSet,cPart_nob);
																						printf("\n P80:Part added to POAvailPartSet: %s",cPart_nob);	fflush(stdout);
																					}
																				}
																				printf("\n P81:PO availale for part:%s ",cPart_nob);	fflush(stdout);
																			}

																			//below break for Desg Master form.
																			break;
																		}
																	}
																	if (POAvailFlag >0)
																	{
																		break;
																	}
																}
															}
															if (POAvailFlag >0)
															{
																break;
															}
														}
													}
													else
													{
														printf("\n P811:UOM is not 4 for part:%s ",cPart_nob);	fflush(stdout);
														UomFlag =1;
														break;
													}
												}
												printf("\n P811:UomFlag:%d ",UomFlag);	fflush(stdout);
												if(UomFlag ==0)
												{
													//IF PO available then Go for PO revision mismatch check, PO check for latest released revision.
													if (POAvailFlag >0)
													{
														PORevAvailFlag=0;
														for(iii=rev_count-1;iii>=0;iii--)
														{
															RelRevFlag=0;
															PORevAvailFlag=0;
															if(AOM_UIF_ask_value (Crevsn_list[iii], "item_revision_id", &Child_rev_idd)!=ITK_ok)PrintErrorStack();
															printf("\n P82:Latest released rev is:%s.", Child_rev_idd);fflush(stdout);
															Chld_Rel_Stus_Cunt=0;
															if(WSOM_ask_release_status_list (Crevsn_list[iii], &Chld_Rel_Stus_Cunt, &Child_status_lstt)!=ITK_ok)PrintErrorStack();
															//printf("\n P83:Chld_Rel_Stus_Cunt is:%d.", Chld_Rel_Stus_Cunt);fflush(stdout);
															if(Chld_Rel_Stus_Cunt > 0)
															{
																crrr=0;
																for(crrr=0;crrr<Chld_Rel_Stus_Cunt;crrr++)
																{
																	RelRevFlag=0;
																	PORevAvailFlag=0;
																	//Checking PO availability on any revision.
																	if(AOM_ask_value_string (Child_status_lstt[crrr], "object_name", &Chld_Rel_Stus)!=ITK_ok)PrintErrorStack();
																	//printf("\n PO:P2:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																	//if((tc_strstr(Chld_Rel_Stus,"Released")!=NULL) || (tc_strstr(Chld_Rel_Stus,"Rlzd")!=NULL)||(tc_strstr(Chld_Rel_Stus,"ERC Released")!=NULL) || (tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL))
																	if((tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)|| (tc_strstr(Chld_Rel_Stus,"ERC Released")!=NULL))
																	{
																		printf("\n P84:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																		RelRevFlag=1;
																		if(GRM_list_secondary_objects_only(Crevsn_list[iii], NULLTAG, &sec_count, &sec_obj)!=ITK_ok)   PrintErrorStack();
																		printf("\n P85:sec_count: %d",sec_count);	fflush(stdout);	
																		jj=0;
																		for(jj=0; jj<sec_count; jj++)
																		{
																			PORevAvailFlag=0;
																			if(AOM_UIF_ask_value(sec_obj[jj], "object_type", &sec_obj_type)!=ITK_ok)   PrintErrorStack();
																			//printf("\n PO:......sec_obj_type:%s", sec_obj_type);fflush(stdout);
																			if((tc_strcmp(sec_obj_type, "Design Revision Master") == 0)||(tc_strcmp(sec_obj_type, "EE Part Revision Master") == 0) ||(tc_strcmp(sec_obj_type, "Clr Part Revision Master") == 0))
																			{
																				printf("\n P86:Asec_obj_type:%s", sec_obj_type);fflush(stdout);
																				if(PartObjj) PartObjj=NULLTAG;
																				PartObjj=sec_obj[jj];
																				printf("\n P87:PORev availability Fun call: %s",cPart_nob);fflush(stdout);
																				PORevAvailFlag=0;
																				PORevAvailFlag = tm_CheckPORevAvalability(PartObjj);
																				printf("\n P88:PORevAvailFlag.: %d",PORevAvailFlag);fflush(stdout);
																				printf("\n P89:PO availability for latset part rev:%s : PORevAvailFlag: %d",cPart_nob,PORevAvailFlag);	fflush(stdout);
																				if (PORevAvailFlag >0)
																				{
																					//ERROR: Add part number in PO Rev mismatch set.
																					printf("\n P90:Rev lvl PO Available for part:%s \n",cPart_nob);
																					if((PORevAvailPartSet==NULL)|| (tc_strcmp(PORevAvailPartSet,"")==0))
																					{
																						tc_strcpy (PORevAvailPartSet,"" );
																						//tc_strcpy (PORevAvailPartSet,"," );
																						tc_strcat (PORevAvailPartSet,cPart_nob);
																						printf("\n P91:Part added to PORevAvailPartSet: %s",cPart_nob);	fflush(stdout);	
																					}
																					else
																					{
																						if(strstr(PORevAvailPartSet,cPart_nob)==NULL)
																						{
																							tc_strcat (PORevAvailPartSet,"," );
																							tc_strcat (PORevAvailPartSet,cPart_nob);
																							printf("\n P92:Part added to PORevAvailPartSet: %s",cPart_nob);	fflush(stdout);
																						}
																					}
																				}
																				else
																				{
																					flgPartAtachedToTsk=0;
																					printf("\n P96:Rev lvl PO not Available for part:%s Child_rev_idd:%s \n",cPart_nob,Child_rev_idd);
																					//Check if same part is attached to task or if modified after 1 jan 2019:PSCPORevNOPOPartsSet
																					if(strstr(TaskAtchedPartSet,cPart_nob)!=NULL)
																					{
																						flgPartAtachedToTsk=1;
																					}
																					NwNOPOPartFlag =0;
																					NwNOPOPartFlag = tm_CheckNewCretNOPO(cPart_nob,Child_rev_idd);
																					printf("\n P97:NwNOPOPartFlag.: %d flgPartAtachedToTsk:%d",NwNOPOPartFlag,flgPartAtachedToTsk);fflush(stdout);
																					//if((NwNOPOPartFlag >0)&& (flgPartAtachedToTsk==0))
																					if((NwNOPOPartFlag >0)&& (flgPartAtachedToTsk==1))
																					{
																						printf("\n P93:NEW CRE/MODI NOPO Parts.:%s \n",cPart_nob);
																						if((PSCPORevNOPOPartsSet==NULL)|| (tc_strcmp(PSCPORevNOPOPartsSet,"")==0))
																						{
																							tc_strcpy (PSCPORevNOPOPartsSet,"" );
																							tc_strcat (PSCPORevNOPOPartsSet,cPart_nob);
																							printf("\n P94:Part added to PSCPORevNOPOPartsSet: %s",cPart_nob);	fflush(stdout);	
																						}
																						else
																						{
																							if(strstr(PSCPORevNOPOPartsSet,cPart_nob)==NULL)
																							{
																								tc_strcat (PSCPORevNOPOPartsSet,"," );
																								tc_strcat (PSCPORevNOPOPartsSet,cPart_nob);
																								printf("\n P95:Part added to PSCPORevNOPOPartsSet: %s",cPart_nob);	fflush(stdout);
																							}
																						}
																					}
																					else
																					{
																						printf("\n P951:SKIP:Legacy PO revision mismatch part: %s",cPart_nob);	fflush(stdout);
																						if((PORevNOPOPartsSet==NULL)|| (tc_strcmp(PORevNOPOPartsSet,"")==0))
																						{
																							tc_strcpy (PORevNOPOPartsSet,"" );
																							//tc_strcpy (PORevNOPOPartsSet,"," );
																							tc_strcat (PORevNOPOPartsSet,cPart_nob);
																							printf("\n P94:Part added to PORevNOPOPartsSet: %s",cPart_nob);	fflush(stdout);	
																						}
																						else
																						{
																							if(strstr(PORevNOPOPartsSet,cPart_nob)==NULL)
																							{
																								tc_strcat (PORevNOPOPartsSet,"," );
																								tc_strcat (PORevNOPOPartsSet,cPart_nob);
																								printf("\n P95:Part added to PORevNOPOPartsSet: %s",cPart_nob);	fflush(stdout);
																							}
																						}
																					}
																				}
																				//below break for Desg Master form.
																				break;
																			}
																		}
																		//below break to stop other released revision
																		break;
																	}
																}
																if (RelRevFlag >0)
																{
																	//if got released revision.
																	break;
																}
															}
														}														
													}
													else
													{
														// If t5_ColourInd Y for NOPO part then check PO on colour part.......
														printf("\n P96:part:%s cPartColInd:%s\n",cPart_nob,cPartColInd);fflush(stdout);
														POfoundFlg=0;
														if (tc_strcmp(cPartColInd,"Y")==0)
														{
															ColPrtFlag = 0;
															ColPrtFlag = tm_CheckColPrtPO(cPart_nob);
															printf("\n Col2:PO check for part:%s ColPrtFlag:%d\n",cPart_nob,ColPrtFlag);fflush(stdout);
															if(ColPrtFlag >0)
															{
																POfoundFlg=1;
															}
														}
														if(POfoundFlg ==0)
														{
															NwNOPOPartFlag =0;
															printf("\n tm_CheckNewCretNOPO:cPart_nob:%s cpartRev:%s",cPart_nob,cpartRev);fflush(stdout);
															NwNOPOPartFlag = tm_CheckNewCretNOPO(cPart_nob,cpartRev);
															printf("\n P97:NwNOPOPartFlag.: %d",NwNOPOPartFlag);fflush(stdout);
															if(NwNOPOPartFlag >0)
															{
																//NEW CRE/MODI NOPO Parts.
																if((NwNOPOPartsSet==NULL)|| (tc_strcmp(NwNOPOPartsSet,"")==0))
																{
																	tc_strcpy (NwNOPOPartsSet,"" );
																	tc_strcat (NwNOPOPartsSet,cPart_nob);
																	printf("\n P98:F:NwNOPOPartsSet..: %s",cPart_nob);	fflush(stdout);	
																}
																else
																{
																	if(strstr(NwNOPOPartsSet,cPart_nob)==NULL)
																	{
																		tc_strcat (NwNOPOPartsSet,"," );
																		tc_strcat (NwNOPOPartsSet,cPart_nob);
																		printf("\n P99:F:NwNOPOPartsSet.: %s",cPart_nob);	fflush(stdout);
																	}
																}
															}
															else
															{
																//LEGACY NOPO Parts
																if((NOPOPartsSet==NULL)|| (tc_strcmp(NOPOPartsSet,"")==0))
																{
																	tc_strcpy (NOPOPartsSet,"" );
																	//tc_strcpy (NOPOPartsSet,"," );
																	tc_strcat (NOPOPartsSet,cPart_nob);
																	printf("\n P100:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);	
																}
																else
																{
																	if(strstr(NOPOPartsSet,cPart_nob)==NULL)
																	{
																		tc_strcat (NOPOPartsSet,"," );
																		tc_strcat (NOPOPartsSet,cPart_nob);
																		printf("\n P101:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);
																	}
																}
															}
														}
														else
														{
															//PO available on color parts.
															if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
															{
																tc_strcpy (POAvailPartSet,"" );
																//tc_strcpy (POAvailPartSet,"," );
																tc_strcat (POAvailPartSet,cPart_nob);
																printf("\n Col1:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);	
															}
															else
															{
																if(strstr(POAvailPartSet,cPart_nob)==NULL)
																{
																	tc_strcat (POAvailPartSet,"," );
																	tc_strcat (POAvailPartSet,cPart_nob);
																	printf("\n Col2:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);
																}
															}
														}
													}

												}
											}
											else
											{
												printf("\n P102:checking unmat part:%s %s.", cPart_nob,cpartRev);fflush(stdout);
												//Part is unmaturated but shoudl be released once.
												if(PartRelFlag >0)
												{
													if((UnMatPartSet==NULL)|| (tc_strcmp(UnMatPartSet,"")==0))
													{
														tc_strcpy (UnMatPartSet,"" );
														tc_strcat (UnMatPartSet,cPart_nob);
														printf("\n P103:Part added to UnMatPartSet: %s",cPart_nob);	fflush(stdout);	
													}
													else
													{
														if(strstr(UnMatPartSet,cPart_nob)==NULL)
														{
															tc_strcat (UnMatPartSet,"," );
															tc_strcat (UnMatPartSet,cPart_nob);
															printf("\n P104:Part added to UnMatPartSet: %s",cPart_nob);	fflush(stdout);
														}
													}
												}
											}
										}
									}
								//}
							}
						}
						else
						{
							EXPANSSIONFlag=0;
						}
					}
				}
			}
			else
			{
				//only expanssion.
				if ((tc_strcmp(cPart_typ,"G")==0)|| (tc_strcmp(cPartCls,"t5GrpID")==0))
				{
					EXPANSSIONFlag=0;
					GrpFunflag=0;
					GrpFunflag = tm_POCheckStopAtFGrpBOMline(Child_line_tag,sDml_TP,sEESWPrtType);
					printf("\n P105:GrpFunflag: %d",GrpFunflag);fflush(stdout);
					if(GrpFunflag >0)
					{
						printf("\n P106: PO failed.: %s cpartRev:%s",cPart_nob,cpartRev);	fflush(stdout);
						NwNOPOPrtFlg =0;
						NwNOPOPrtFlg = tm_CheckNewCretNOPO(cPart_nob,cpartRev);
						printf("\n P107:NwNOPOPrtFlg.: %d",NwNOPOPrtFlg);fflush(stdout);
						if(NwNOPOPrtFlg >0)
						{
							if((NwNOPOPartsSet==NULL)|| (tc_strcmp(NwNOPOPartsSet,"")==0))
							{
								tc_strcpy (NwNOPOPartsSet,"" );
								tc_strcat (NwNOPOPartsSet,cPart_nob);
								printf("\n P108:NwNOPOPartsSet. %s",cPart_nob);	fflush(stdout);	
							}
							else
							{
								if(strstr(NwNOPOPartsSet,cPart_nob)==NULL)
								{
									tc_strcat (NwNOPOPartsSet,"," );
									tc_strcat (NwNOPOPartsSet,cPart_nob);
									printf("\n P109:NwNOPOPartsSet.. %s",cPart_nob);	fflush(stdout);
								}
							}
						}
						else
						{
							if((NOPOPartsSet==NULL)|| (tc_strcmp(NOPOPartsSet,"")==0))
							{
								tc_strcpy (NOPOPartsSet,"" );
								tc_strcat (NOPOPartsSet,cPart_nob);
								printf("\n P110:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);	
							}
							else
							{
								if(strstr(NOPOPartsSet,cPart_nob)==NULL)
								{
								tc_strcat (NOPOPartsSet,"," );
								tc_strcat (NOPOPartsSet,cPart_nob);
								printf("\n P111:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);
								}
							}
						}
					}
					else
					{
						printf("\n P112:PO success.: %s",cPart_nob);	fflush(stdout);
						if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
						{
							tc_strcpy (POAvailPartSet,"" );
							tc_strcat (POAvailPartSet,cPart_nob);
							printf("\n P113:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);	
						}
						else
						{
							if(strstr(POAvailPartSet,cPart_nob)==NULL)
							{
							tc_strcat (POAvailPartSet,"," );
							tc_strcat (POAvailPartSet,cPart_nob);
							printf("\n P114:Part added to NOPOset: %s",cPart_nob);	fflush(stdout);
							}
						}
					}
				}
				else
				{
					printf("\n P115:..CS other than F so checking EXPANSSION:[%s]  \n",cPart_nob);fflush(stdout);
					EXPANSSIONFlag=1;
					printf("\n P116:..EXPANSSIONFlag:[%d]\n",EXPANSSIONFlag);fflush(stdout);
				}
			}
		}
		//Expanding E50/E99 parts.
	}
	
	if (EXPANSSIONFlag >0)
	{
		printf("\n P129: BOM Window setting for parent part: %s",cPart_nob);fflush(stdout);
		if(BOM_create_window(&tagBOMwindowB)!=ITK_ok)   PrintErrorStack();
		if(CFM_find("ERC release and above", &tagRuleB)!=ITK_ok)   PrintErrorStack(); 
		if(BOM_set_window_config_rule( tagBOMwindowB, tagRuleB )!=ITK_ok)   PrintErrorStack();
		//0-qty skip
		if(PIE_find_closure_rules("TMLBOMExpandByQty",PIE_TEAMCENTER, &rulefound, &POclosurerule )!=ITK_ok)   PrintErrorStack();
		//printf ("P129:rule count %d \n",rulefound);fflush(stdout);
		if (rulefound > 0)
		{
			POclose_tag = POclosurerule[0];
			//printf ("P129:closure rule found \n");fflush(stdout);
		}
		if(BOM_window_set_closure_rule( tagBOMwindowB,POclose_tag, 0, POrulename,POrulevalue )!=ITK_ok)   PrintErrorStack();
		//0-qty skip
		if(BOM_set_window_top_line(tagBOMwindowB, NULLTAG, Prt_tag, NULLTAG, &tagBOMlineB)!=ITK_ok)   PrintErrorStack();
		if(BOM_line_ask_child_lines(tagBOMlineB, &childCountTopBomLineB, &tagTopBLchildLineB)!=ITK_ok)   PrintErrorStack();
		printf("\n P118:SLP:Partent Part:%s total child count: %d\n",cPart_nob,childCountTopBomLineB);fflush(stdout);
		for(iCountB =0 ; iCountB < childCountTopBomLineB; iCountB++)
		{
			printf("\n P119:SLP:Partent Part:%s taking child part: %d\n",cPart_nob,iCountB);fflush(stdout);
			FunOutflag=0;
			FunOutflag= tm_POCheckStopAtFBOMline(tagTopBLchildLineB[iCountB],sDml_TP,sEESWPrtType,cPart_nob,cpartRev);
			printf("\n P132:FunOutflag: %s :%d",cPart_nob,FunOutflag);fflush(stdout);
		}
		
	}
	
	return status;
}

// For E50 parts under group id.
int tm_POCheckStopAtFE50BOMline(tag_t GrpE50childBoMLine,char* sDml_TP,char* sEESWPrtType)
{
	int jj =0;
	int iCCnt =0;
	int E50BranchFailed=0;
	int CSFoundFlag=0;
	int F18CSFoundFlag=0;
	int EXPANSSIONFlag=0;
	int GE50chldLineCunt=0;
	int DummyFlg=0;
	int EEPartByPass=0;
	int rev_count=0;
	int CondiPassFlag=0;
	int ii=0;
	int crr=0;
	int eChld_Rel_Stus_Cunt=0;
	int sec_count=0;
	int GrpE50Fun=0;
	int GrpE50GrpFun=0;
	int POAvailFlag=0;
	char*	 EChild_Prt_Uom = NULL;
	char*	 sPartCS = NULL;
	char*	 eChld_Rel_Stus = NULL;
	char*	 eChild_rev_idd = NULL;
	char*	 EPart_nob = NULL;
	char*	 ePart_class = NULL;
	char*	 ePart_nob = NULL;
	char*	 ePart_DR = NULL;
	char*	 ePart_typ = NULL;
	char*	 ePartProj = NULL;
	char*	 ePartColInd = NULL;
	char*	 epartDesgGrp = NULL;
	char*	 eChld_type = NULL;
	char*	 eEEswTyp = NULL;
	char*	 gPart_class = NULL;
	char*	 gChld_type = NULL;
	char*	 esec_obj_type = NULL;
	tag_t PartObjm = NULLTAG;
	tag_t eAll_item_tgs = NULLTAG;
	tag_t EEpart_tag = NULLTAG;
	tag_t EEitem_tag = NULLTAG;
	tag_t PPrt_tag = NULLTAG;
	tag_t PP_tag = NULLTAG;
	tag_t echldtg = NULLTAG;
	tag_t *Erevsn_list=NULLTAG;
	tag_t *eChild_status_lst=NULLTAG;
	tag_t *GE50childBoMLine=NULLTAG;
	tag_t	*esec_obj		= NULL;

	printf("\n P137:Inside CS check function:TP:%s\n",sDml_TP);fflush(stdout);
	if(AOM_ask_value_string(GrpE50childBoMLine,"bl_item_item_id",&EPart_nob)!=ITK_ok)   PrintErrorStack();
	if(BOM_line_ask_child_lines(GrpE50childBoMLine, &GE50chldLineCunt, &GE50childBoMLine)!=ITK_ok)   PrintErrorStack();
	printf("\n P138:E50:SLP:No of child objects are for Parent Part:%s: %d\n",EPart_nob,GE50chldLineCunt);fflush(stdout);
	for(iCCnt =0 ; iCCnt < GE50chldLineCunt; iCCnt++)
	{
		CSFoundFlag=0;
		F18CSFoundFlag=0;
		if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_item_item_id",&ePart_nob)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_PartStatus",&ePart_DR)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_PartType",&ePart_typ)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_ProjectCode",&ePartProj)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_ColourInd",&ePartColInd)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_DesignGrp",&epartDesgGrp)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"fnd0bl_line_object_type",&eChld_type)!=ITK_ok)PrintErrorStack();
		//Query part and take class.
		//if(ITEM_find_item (ePart_nob, &PP_tag)!=ITK_ok)PrintErrorStack();
		PP_tag= t5GetItemRevison(ePart_nob);
		if(PP_tag==NULLTAG)
		{
			printf("\n P138:PP_tag is NULL...\n");fflush(stdout);
		}
		else
		{
			printf("\n P138:PP_tag is found...\n");fflush(stdout);
			if(ITEM_ask_latest_rev (PP_tag, &PPrt_tag)!=ITK_ok)PrintErrorStack();
			if(PPrt_tag==NULLTAG)
			{
				printf("\n P139:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
				return 0;
			}
			else
			{
				if(POM_class_of_instance(PPrt_tag,&echldtg)!=ITK_ok)   PrintErrorStack();
				if(POM_name_of_class(echldtg,&ePart_class)!=ITK_ok)   PrintErrorStack();
			}
		}
		printf("\n P140:E50:Part no:%s ePart_class:%s ePart_typ:%s ePartProj:%s ePartColInd:%s epartDesgGrp:%s eChld_type:%s",ePart_nob,ePart_class,ePart_typ,ePartProj,ePartColInd,epartDesgGrp,eChld_type);fflush(stdout);	
		//Skipping Dummy parts and spare kit
		if(tc_strcmp(ePart_typ,"D")==0 || tc_strcmp(ePart_typ,"DA")==0 ||tc_strcmp(ePart_typ,"DC")==0 || tc_strcmp(ePart_typ,"IFD")==0|| tc_strcmp(ePart_typ,"IM" )==0 ||(tc_strcmp(ePart_class,"t5SpKit")==0))
		{
			printf("\n P141:E50:Dummy Parts skip for PO: %s",ePart_nob);	fflush(stdout);	
		}
		else
		{
			if (tc_strcmp(sDml_TP,"CARPLANT")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P142:E50:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P143:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P144:E50:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P145:E50:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P146:E50:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
			}
			else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P147:E50:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P148:E50:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"PCVBU")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P149:E50:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"PUVBU")==0)
			{
				if(AOM_ask_value_string(GE50childBoMLine[iCCnt],"bl_Design Revision_t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n P150:E50:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else
			{
				printf("\n P151:E50:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
			}
			if((sPartCS==NULL)|| (tc_strcmp(sPartCS,"")==0)|| (tc_strcmp(sPartCS,"NA")==0))
			{
				printf("\n P152:E50:target Plant CS is NULL so considered as NOPO: %s",ePart_nob);	fflush(stdout);	
				//E50 branch got failed.
				E50BranchFailed=1;
				break;
			}
			else
			{
				if(tc_strcmp(sPartCS,"F")==0 || tc_strcmp(sPartCS,"F30")==0|| tc_strcmp(sPartCS,"F35")==0 || tc_strcmp(sPartCS,"F40")==0)
				{
					printf("\n P153:F CS fonud.\n");fflush(stdout);
					CSFoundFlag=1;
				}
				else if (tc_strcmp(sPartCS,"F18")==0 || tc_strcmp(sPartCS,"F19")==0)
				{
					printf("\n P154:F18 CS fonud.\n");fflush(stdout);
					F18CSFoundFlag=1;
				}
				else
				{
					printf(" P155:Found Other CS.\n");fflush(stdout);
					CSFoundFlag=0;
					F18CSFoundFlag=0;
				}
				if (F18CSFoundFlag >0)
				{
					EXPANSSIONFlag=0;
					if((BypassPartSet==NULL)|| (tc_strcmp(BypassPartSet,"")==0))
					{
						tc_strcpy (BypassPartSet,"" );
						//tc_strcpy (BypassPartSet,"," );
						tc_strcat (BypassPartSet,ePart_nob);
						printf("\n P156:Part added to BypassPartSet: %s",ePart_nob);	fflush(stdout);	
					}
					else
					{
						if(strstr(BypassPartSet,ePart_nob)==NULL)
						{
						tc_strcat (BypassPartSet,"," );
						tc_strcat (BypassPartSet,ePart_nob);
						printf("\n P157:Part added to BypassPartSet: %s",ePart_nob);	fflush(stdout);
						}
					}
				}
				else if (CSFoundFlag >0)
				{
					DummyFlg=0;
					CondiPassFlag=0;
					EXPANSSIONFlag=0;
					printf("\n P158:.Checking conditions..:[%s]\n",ePart_nob);fflush(stdout);

					if(tc_strcmp(ePart_typ,"D")==0 || tc_strcmp(ePart_typ,"DA")==0 ||tc_strcmp(ePart_typ,"DC")==0 || tc_strcmp(ePart_typ,"IFD")==0|| tc_strcmp(ePart_typ,"IM" )==0 ||(tc_strcmp(ePart_class,"t5SpKit")==0))
					{
						printf("\n P159:.Obsolete/Dummy part skipped....:[%s]\n",ePart_nob);fflush(stdout);
						DummyFlg=1;				
					}
					else if((tc_strcmp(ePartProj,"1111")==0)||(tc_strcmp(ePart_class,"T5_EE_PartRevision")==0)||(tc_strcmp(ePart_typ,"G")==0)|| (tc_strcmp(ePart_class,"t5GrpID")==0)||((strlen(ePart_nob)!=11)&& ((tc_strcmp(ePartColInd,"Y")==0)||(tc_strcmp(ePartColInd,"N")==0)||(tc_strcmp(ePartColInd,"C")==0))&&((tc_strcmp(ePart_typ,"A")==0)||(tc_strcmp(ePart_typ,"C")==0)||(tc_strcmp(ePart_typ,"M")==0))))
					{
						printf("\n P160:Part eligible for PO check:[%s]\n",ePart_nob);fflush(stdout);
						CondiPassFlag=1;
					}
					else
					{
						printf("\n P161:NO expanssion for DR3 and below CS-F parts:[%s]\n",ePart_nob);fflush(stdout);
						EXPANSSIONFlag=1;
					}
					printf("\n P162:DummyFlg:%d CondiPassFlag:%d EXPANSSIONFlag:%d\n",DummyFlg,CondiPassFlag,EXPANSSIONFlag);fflush(stdout);
					if(DummyFlg >0)
					{
						EXPANSSIONFlag=0;
						if((SkippedPartSet==NULL)|| (tc_strcmp(SkippedPartSet,"")==0))
						{
							tc_strcpy (SkippedPartSet,"" );
							//tc_strcpy (SkippedPartSet,"," );
							tc_strcat (SkippedPartSet,ePart_nob);
							printf("\n P167:Dmy:Part added to NOPOset: %s",ePart_nob);	fflush(stdout);	
						}
						else
						{
							if(strstr(SkippedPartSet,ePart_nob)==NULL)
							{
							tc_strcat (SkippedPartSet,"," );
							tc_strcat (SkippedPartSet,ePart_nob);
							printf("\n P168:Dmy:Part added to NOPOset: %s",ePart_nob);	fflush(stdout);
							}
						}
					}
					else if (CondiPassFlag >0)
					{
						if ((tc_strcmp(ePart_typ,"G")==0)|| (tc_strcmp(ePart_class,"t5GrpID")==0))
						{
							EXPANSSIONFlag=0;
							GrpE50GrpFun=0;
							GrpE50GrpFun = tm_POCheckStopAtFGrpBOMline(GE50childBoMLine[iCCnt],sDml_TP,sEESWPrtType);
							printf("\n P169:GrpE50GrpFun: %d",GrpE50GrpFun);fflush(stdout);
							if(GrpE50GrpFun >0)
							{
								E50BranchFailed=1;
								break;
							}
						}
						else
						{
							EEPartByPass=0;
							if((tc_strcmp(gPart_class,"T5_EE_PartRevision")==0)||(tc_strcmp(gChld_type,"T5_EE_PartRevision")==0)||(tc_strcmp(gChld_type,"EE Part Revision")==0))
							{
								//Query EE part and take Swft part type.
								//if(ITEM_find_item (ePart_nob, &EEitem_tag)!=ITK_ok)PrintErrorStack();
								EEitem_tag= t5GetItemRevison(ePart_nob);
								if(EEitem_tag==NULLTAG)
								{
									printf("\n P169:EEitem_tag is Nnull...\n");fflush(stdout);
								}
								else
								{
									printf("\n P169:EEitem_tag is fond...\n");fflush(stdout);
									if(ITEM_ask_latest_rev (EEitem_tag, &EEpart_tag)!=ITK_ok)PrintErrorStack();
									if(EEpart_tag==NULLTAG)
									{
										printf("\n P170:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
										return 0;
									}
									else
									{
										if(AOM_ask_value_string(EEpart_tag,"t5_SwPartType",&eEEswTyp)!=ITK_ok)PrintErrorStack();
										printf("\n P171:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
										if(strstr(sEESWPrtType,eEEswTyp)!=NULL)
										{
											printf("\n PO172: Bypassing EE part for PO.\n");fflush(stdout);
											EEPartByPass=1;
											if((SkippedPartSet==NULL)|| (tc_strcmp(SkippedPartSet,"")==0))
											{
												tc_strcpy (SkippedPartSet,"" );
												//tc_strcpy (SkippedPartSet,"," );
												tc_strcat (SkippedPartSet,ePart_nob);
												printf("\n P167:Dmy:Part added to NOPOset: %s",ePart_nob);	fflush(stdout);	
											}
											else
											{
												if(strstr(SkippedPartSet,ePart_nob)==NULL)
												{
												tc_strcat (SkippedPartSet,"," );
												tc_strcat (SkippedPartSet,ePart_nob);
												printf("\n P168:Dmy:Part added to NOPOset: %s",ePart_nob);	fflush(stdout);
												}
											}
										}
										else
										{
											EEPartByPass=0;
										}
									}
								}
							}
							if (EEPartByPass ==0)
							{
								//bypass colour parts.
								if((strlen(ePart_nob)==14) && ((tc_strcmp(ePartColInd,"N")==0)|| (tc_strcmp(ePartColInd,"Y")==0)))
								{
									printf("\n P173: Bypass 14 digit parts for PO:[%s].\n",ePart_nob);fflush(stdout);
								}
								else
								{
									//Owners Manual.
									printf("\n PO:P2:Checkinging for Owners Manual:[%s : %s :%s].\n",ePart_nob,epartDesgGrp,ePart_DR);fflush(stdout);
									/*if((tc_strcmp(epartDesgGrp,"58")==0)&& ((tc_strcmp(ePart_DR,"DR4")==0)|| (tc_strcmp(ePart_DR,"AR4")==0)))
									{
										//MOHAN: Need to check for detailed logic.
										printf("\n PO: Owners Manual bypassed:[%s].\n",ePart_nob);fflush(stdout);
									}
									else
									{*/
										//Going to check PO details.
										//if(ITEM_find_item (ePart_nob, &eAll_item_tgs)!=ITK_ok)   PrintErrorStack();
										eAll_item_tgs= t5GetItemRevison(ePart_nob);
										if(eAll_item_tgs==NULLTAG)
										{
											printf("\n P2:eAll_item_tgs is Nnull...\n");fflush(stdout);
										}
										else
										{
											printf("\n P2:eAll_item_tgs is fond...\n");fflush(stdout);
											if(ITEM_list_all_revs (eAll_item_tgs, &rev_count, &Erevsn_list)!=ITK_ok)   PrintErrorStack();
											printf("\n PO:.Total rev found: %d.", rev_count);fflush(stdout);
											if(rev_count > 0)
											{
												POAvailFlag=0;
												for(ii=rev_count-1;ii>=0;ii--)
												{
													POAvailFlag=0;
													if(AOM_UIF_ask_value (Erevsn_list[ii], "item_revision_id", &eChild_rev_idd)!=ITK_ok)PrintErrorStack();
													if(tc_strcmp(gPart_class,"T5_ClrPartRevision")!=0)
													{
													if(AOM_UIF_ask_value(Erevsn_list[ii],"t5_uom",&EChild_Prt_Uom)!=ITK_ok)   PrintErrorStack();
													}
													printf("\n PO:Latest released rev is:%s EChild_Prt_Uom:%s", eChild_rev_idd,EChild_Prt_Uom);fflush(stdout);
													if((tc_strcmp(EChild_Prt_Uom,"")==0)||(tc_strcmp(EChild_Prt_Uom,"each")==0)||(tc_strcmp(EChild_Prt_Uom,"4-Nos")==0))
													{
														if(WSOM_ask_release_status_list (Erevsn_list[ii], &eChld_Rel_Stus_Cunt, &eChild_status_lst)!=ITK_ok)PrintErrorStack();
														//printf("\n PO:eChld_Rel_Stus_Cunt is:%d.", eChld_Rel_Stus_Cunt);fflush(stdout);
														if(eChld_Rel_Stus_Cunt > 0)
														{
															crr=0;
															for(crr=0;crr<eChld_Rel_Stus_Cunt;crr++)
															{
																POAvailFlag=0;
																//Checking PO availability on any revision.
																if(AOM_ask_value_string (eChild_status_lst[crr], "object_name", &eChld_Rel_Stus)!=ITK_ok)PrintErrorStack();
																//printf("\n PO: eChld_Rel_Stus:%s", eChld_Rel_Stus);fflush(stdout);
																//if((tc_strstr(eChld_Rel_Stus,"Released")!=NULL) || (tc_strstr(eChld_Rel_Stus,"Rlzd")!=NULL)||(tc_strstr(eChld_Rel_Stus,"ERC Released")!=NULL) || (tc_strstr(eChld_Rel_Stus,"T5_LcsErcRlzd")!=NULL))
																if(tc_strstr(eChld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)
																{
																	printf("\n PO:Latest released rev is:%s EChild_Prt_Uom:%s", eChild_rev_idd,EChild_Prt_Uom);fflush(stdout);
																	printf("\n PO: eChld_Rel_Stus:%s", eChld_Rel_Stus);fflush(stdout);
																	if(GRM_list_secondary_objects_only(Erevsn_list[ii], NULLTAG, &sec_count, &esec_obj)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:sec_count: %d",sec_count);	fflush(stdout);	
																	for(jj=0; jj<sec_count; jj++)
																	{
																		POAvailFlag=0;
																		if(PartObjm) PartObjm=NULLTAG;
																		PartObjm=esec_obj[jj];
																		if(AOM_UIF_ask_value(PartObjm, "object_type", &esec_obj_type)!=ITK_ok)   PrintErrorStack();
																		//printf("\n PO:esec_obj_type.:%s", esec_obj_type);fflush(stdout);
																		if((tc_strcmp(esec_obj_type, "Design Revision Master") == 0) ||(tc_strcmp(esec_obj_type, "EE Part Revision Master") == 0) ||(tc_strcmp(esec_obj_type, "Clr Part Revision Master") == 0))
																		{
																			printf("\n PO:esec_obj_type.:%s", esec_obj_type);fflush(stdout);
																			//PO availability Fun call
																			POAvailFlag=0;
																			POAvailFlag = tm_CheckPOAvalability(PartObjm);
																			printf("\n P2:SIGNOFF:POAvailFlag.: %d",POAvailFlag);fflush(stdout);
																			if(POAvailFlag >0)
																			{
																				//PO Available so break.
																				if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
																				{
																					tc_strcpy (POAvailPartSet,"" );
																					//tc_strcpy (POAvailPartSet,"," );
																					tc_strcat (POAvailPartSet,ePart_nob);
																					printf("\n PO:F:P32:Part added to POAvailPartSet: %s",ePart_nob);	fflush(stdout);	
																				}
																				else
																				{
																					if(strstr(POAvailPartSet,ePart_nob)==NULL)
																					{
																					tc_strcat (POAvailPartSet,"," );
																					tc_strcat (POAvailPartSet,ePart_nob);
																					printf("\n PO:F:P22:Part added to POAvailPartSet: %s",ePart_nob);	fflush(stdout);
																					}
																				}
																				printf("\n PO:P2:PO availale for part:%s",ePart_nob);	fflush(stdout);
																			}
																			//below break for Desg Master form.
																			break;
																		}
																	}
																	if (POAvailFlag >0)
																	{
																		break;
																	}
																}
															}
															if (POAvailFlag >0)
															{
																break;
															}
														}
													}
													else
													{
														printf("\n PO:E50:UOM not 4 for part:%s \n",EChild_Prt_Uom);fflush(stdout);
														POAvailFlag=1;
														break;
													}
												}

												//IF PO available then Go for PO revision mismatch check, PO check for latest released revision.
												if (POAvailFlag >0)
												{
													printf("\n PO:E50:PO available for part:%s \n",ePart_nob);fflush(stdout);
												}
												else
												{
													printf("\n PO:E50:PO not available for part:%s \n",ePart_nob);fflush(stdout);
													E50BranchFailed=1;
													break;
												}
											}
										}
									//}
								}
							}
							else
							{
								printf("\n PO: Bypassing EE part for PO....\n");fflush(stdout);
							}
						}
					}
				}
				else
				{
					//only expanssion.
					if((tc_strcmp(ePart_typ,"G")==0)|| (tc_strcmp(ePart_class,"t5GrpID")==0))
					{
						GrpE50GrpFun=0;
						GrpE50GrpFun = tm_POCheckStopAtFGrpBOMline(GE50childBoMLine[iCCnt],sDml_TP,sEESWPrtType);
						printf("\n PO:GrpE50GrpFun: %d",GrpE50GrpFun);fflush(stdout);
						if(GrpE50GrpFun >0)
						{
							E50BranchFailed=1;
							break;
						}
					}
					else
					{
						//tm_POCheckStopAtFE50BOMline
						//printf("\n PO:CS other than F so checking EXPANSSION:[%s] \n",ePart_nob);fflush(stdout);
						printf("\n PO:Grp E50 part expanssion.:%s\n",ePart_nob);fflush(stdout);
						GrpE50Fun=0;
						GrpE50Fun = tm_POCheckStopAtFE50BOMline(GE50childBoMLine[iCCnt],sDml_TP,sEESWPrtType);
						printf("\n PO:GrpE50Fun: %d",GrpE50Fun);fflush(stdout);
						if(GrpE50Fun >0)
						{
							E50BranchFailed=1;
							break;
						}
					}
				}
			}
			//Expanding E50/E99 parts.
		}
	}
	if (E50BranchFailed >0)
	{
		return 1;
	}
	//if(TskObjSO) MEM_free(TskObjSO);
	
	return 0;
}
//for Group id
int tm_POCheckStopAtFGrpBOMline(tag_t GrpChild_line_tag,char* sDml_TP,char* sEESWPrtType)
{
	int ii=0;
	int jj=0;
	int crr=0;
	int rev_count=0;
	int sec_count=0;
	int iCnt=0;
	int GrpE50Fun=0;
	int AllF18CSFlag=0;
	int CondiPassFlag=0;
	int CSFoundFlag=0;
	int GchldLineCunt=0;
	int EEPartByPass=0;
	int EXPANSSIONFlag=0;
	int Chld_Rel_Stus_Cunt=0;
	int DummyFlg=0;
	int GrpGrpFun=0;
	int F18CSFoundFlag=0;
	int UomFlag=0;
	int POAvailFlag=0;
	int GrpIdSuccessFlag=0;
	char*	 Chld_Rel_Stus = NULL;
	char*	 grPart_nob = NULL;
	char*	 gPart_nob = NULL;
	char*	 sPartCS = NULL;
	char*	 gPart_class = NULL;
	char*	 gPart_DR = NULL;
	char*	 gPart_typ = NULL;
	char*	 gPartProj = NULL;
	char*	 gPartColInd = NULL;
	char*	 gpartDesgGrp = NULL;
	char*	 sEEswTyp = NULL;
	char*	 sec_obj_type = NULL;
	char*	 gChld_type = NULL;
	char*	 GChild_Prt_Uom = NULL;
	char*	 Child_rev_idd = NULL;
	tag_t All_item_tgs = NULLTAG;
	tag_t EEpart_tag = NULLTAG;
	tag_t PartObjk = NULLTAG;
	tag_t EEitem_tag = NULLTAG;
	tag_t gClsTg = NULLTAG;
	tag_t GP_tag = NULLTAG;
	tag_t GPrt_tag = NULLTAG;
	tag_t *GchildBoMLine=NULLTAG;
	tag_t	*sec_obj		= NULL;
	tag_t	*Grevsn_list		= NULL;
	tag_t	*Child_status_lst		= NULL;

	printf("\n PO:Grp:Inside CS check function:TP:%s\n",sDml_TP);fflush(stdout);
	if(AOM_ask_value_string(GrpChild_line_tag,"bl_item_item_id",&grPart_nob)!=ITK_ok)   PrintErrorStack();

	if(BOM_line_ask_child_lines(GrpChild_line_tag, &GchldLineCunt, &GchildBoMLine)!=ITK_ok)   PrintErrorStack();
	printf("\n PO:SLP:No of child objects are for Parent Part:%s: %d\n",grPart_nob,GchldLineCunt);fflush(stdout);
	//Checking F18 Case
	for(iCnt =0 ; iCnt < GchldLineCunt; iCnt++)
	{
		AllF18CSFlag=0;
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_item_item_id",&gPart_nob)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PartStatus",&gPart_DR)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PartType",&gPart_typ)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_ProjectCode",&gPartProj)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_ColourInd",&gPartColInd)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_DesignGrp",&gpartDesgGrp)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"fnd0bl_line_object_type",&gChld_type)!=ITK_ok)PrintErrorStack();
		//Query part and take class.
		//if(ITEM_find_item (gPart_nob, &GP_tag)!=ITK_ok)PrintErrorStack();
		GP_tag= t5GetItemRevison(gPart_nob);
		if(GP_tag==NULLTAG)
		{
			printf("\n P2:GP_tag is Nnull...\n");fflush(stdout);
		}
		else
		{
			printf("\n P2:GP_tag is fond...\n");fflush(stdout);
			if(ITEM_ask_latest_rev (GP_tag, &GPrt_tag)!=ITK_ok)PrintErrorStack();
			if(GPrt_tag==NULLTAG)
			{
				printf("\n PO:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
				return 0;
			}
			else
			{
				if(POM_class_of_instance(GPrt_tag,&gClsTg)!=ITK_ok)   PrintErrorStack();
				if(POM_name_of_class(gClsTg,&gPart_class)!=ITK_ok)   PrintErrorStack();
			}
		}
		printf("\n PO:Grp: Part no:%s gPart_class:%s gPart_typ:%s gPartProj:%s gPartColInd:%s gpartDesgGrp:%s gChld_type:%s",gPart_nob,gPart_class,gPart_typ,gPartProj,gPartColInd,gpartDesgGrp,gChld_type);fflush(stdout);	

		//Skipping Dummy parts and spare kit
		if(tc_strcmp(gPart_typ,"D")==0 || tc_strcmp(gPart_typ,"DA")==0 ||tc_strcmp(gPart_typ,"DC")==0 || tc_strcmp(gPart_typ,"IFD")==0|| tc_strcmp(gPart_typ,"IM" )==0 ||(tc_strcmp(gPart_class,"t5SpKit")==0))
		{
			printf("\n PO:Grp:Part skipped for PO check: %s",gPart_nob);	fflush(stdout);	
		}
		else
		{
			if (tc_strcmp(sDml_TP,"CARPLANT")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);		
			}
			else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
			}
			else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"PCVBU")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"PUVBU")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO3:Grp:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else
			{
				printf("\n PO3:Grp:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
			}
			if((sPartCS==NULL)|| (tc_strcmp(sPartCS,"")==0)|| (tc_strcmp(sPartCS,"NA")==0))
			{
				AllF18CSFlag=1;
				printf("\n PO3:Grp:target Plant CS is NULL so considered as NOPO: %s",gPart_nob);	fflush(stdout);	
				break;
			}
			else
			{
				if (tc_strcmp(sPartCS,"F18")!=0 && tc_strcmp(sPartCS,"F19")!=0)
				{
					AllF18CSFlag=1;
					printf("\n PO3:.Grp:other than F18 CS fonud.\n");fflush(stdout);
					break;
				}
			}
		}
	}
	if (AllF18CSFlag >0)
	{
		iCnt =0 ;
		for(iCnt =0 ; iCnt < GchldLineCunt; iCnt++)
		{
			CSFoundFlag=0;
			F18CSFoundFlag=0;
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_item_item_id",&gPart_nob)!=ITK_ok)   PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PartStatus",&gPart_DR)!=ITK_ok)   PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PartType",&gPart_typ)!=ITK_ok)   PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_ProjectCode",&gPartProj)!=ITK_ok)PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_ColourInd",&gPartColInd)!=ITK_ok)PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_DesignGrp",&gpartDesgGrp)!=ITK_ok)PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"fnd0bl_line_object_type",&gChld_type)!=ITK_ok)PrintErrorStack();
			printf("\n PO2:Grp:checking for prt %s",gPart_nob);	fflush(stdout);
			//Query part and take class.
			//if(ITEM_find_item (gPart_nob, &GP_tag)!=ITK_ok)PrintErrorStack();
			GP_tag= t5GetItemRevison(gPart_nob);
			if(GP_tag==NULLTAG)
			{
				printf("\n P2:GP_tag is Nnull...\n");fflush(stdout);
			}
			else
			{
				printf("\n P2:GP_tag is fond...\n");fflush(stdout);
				if(ITEM_ask_latest_rev (GP_tag, &GPrt_tag)!=ITK_ok)PrintErrorStack();
				if(GPrt_tag==NULLTAG)
				{
					printf("\n PO:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
					return 0;
				}
				else
				{
					if(POM_class_of_instance(GPrt_tag,&gClsTg)!=ITK_ok)   PrintErrorStack();
					if(POM_name_of_class(gClsTg,&gPart_class)!=ITK_ok)   PrintErrorStack();
				}
			}
			printf("\n PO:Grp: Part no:%s gPart_class:%s gPart_typ:%s gPartProj:%s gPartColInd:%s gpartDesgGrp:%s gChld_type:%s",gPart_nob,gPart_class,gPart_typ,gPartProj,gPartColInd,gpartDesgGrp,gChld_type);fflush(stdout);	

			//Skipping Dummy parts and spare kit
			if(tc_strcmp(gPart_typ,"D")==0 || tc_strcmp(gPart_typ,"DA")==0 ||tc_strcmp(gPart_typ,"DC")==0 || tc_strcmp(gPart_typ,"IFD")==0|| tc_strcmp(gPart_typ,"IM" )==0 ||(tc_strcmp(gPart_class,"t5SpKit")==0))
			{
				printf("\n PO:Grp:Part typ eDummy skipp for PO %s",gPart_nob);	fflush(stdout);
			}
			else
			{
				if (tc_strcmp(sDml_TP,"CARPLANT")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO4:Grp:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO:Grp:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);
				}
				else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO:Grp:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO:Grp:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO:Grp:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
				}
				else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO:Grp:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO:Grp:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"PCVBU")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO:Grp:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"PUVBU")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO:Grp:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else
				{
					printf("\n PO:Grp:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
				}
				if((sPartCS==NULL)|| (tc_strcmp(sPartCS,"")==0) || (tc_strcmp(sPartCS,"NA")==0))
				{
					printf("\n PO:Grp:target Plant CS is NULL so considered as NOPO: %s",gPart_nob);	fflush(stdout);	
				}
				else
				{
					if(tc_strcmp(sPartCS,"F")==0 || tc_strcmp(sPartCS,"F30")==0|| tc_strcmp(sPartCS,"F35")==0 || tc_strcmp(sPartCS,"F40")==0)
					{
						printf("\n PO:Grp:F CS fonud.\n");fflush(stdout);
						CSFoundFlag=1;
					}
					else if (tc_strcmp(sPartCS,"F18")==0 || tc_strcmp(sPartCS,"F19")==0)
					{
						printf("\n PO:Grp:F18 CS fonud.\n");fflush(stdout);
						F18CSFoundFlag=1;
					}
					else
					{
						printf("\n PO:Grp:Found Other CS.\n");fflush(stdout);
						CSFoundFlag=0;
						F18CSFoundFlag=0;
					}
					if (F18CSFoundFlag >0)
					{
						EXPANSSIONFlag=0;
					}
					else if (CSFoundFlag >0)
					{
						DummyFlg=0;
						CondiPassFlag=0;
						EXPANSSIONFlag=0;
						printf("\n PO:Grp:.Checking conditions..:[%s] \n",gPart_nob);fflush(stdout);

						if(tc_strcmp(gPart_typ,"D")==0 || tc_strcmp(gPart_typ,"DA")==0 ||tc_strcmp(gPart_typ,"DC")==0 || tc_strcmp(gPart_typ,"IFD")==0|| tc_strcmp(gPart_typ,"IM" )==0 ||(tc_strcmp(gPart_class,"t5SpKit")==0))
						{
							printf("\n PO:A:.Obsolete/Dummy part skipped....:[%s]\n",gPart_nob);fflush(stdout);
							DummyFlg=1;				
						}
						else if((tc_strcmp(gPartProj,"1111")==0)||(tc_strcmp(gPart_class,"T5_EE_PartRevision")==0)||(tc_strcmp(gPart_typ,"G")==0)|| (tc_strcmp(gPart_class,"t5GrpID")==0)||((strlen(gPart_nob)!=11)&& ((tc_strcmp(gPartColInd,"Y")==0)||(tc_strcmp(gPartColInd,"N")==0)||(tc_strcmp(gPartColInd,"C")==0))&&((tc_strcmp(gPart_typ,"A")==0)||(tc_strcmp(gPart_typ,"C")==0)||(tc_strcmp(gPart_typ,"M")==0))))
						{
							printf("\n PO:.Part eligible for PO check:[%s]\n",gPart_nob);fflush(stdout);
							CondiPassFlag=1;
						}
						else
						{
							printf("\n PO:P3:NO expanssion for DR3 and below CS-F parts:[%s]\n",gPart_nob);fflush(stdout);
							EXPANSSIONFlag=1;
						}
						printf("\n PO:DummyFlg:%d CondiPassFlag:%d EXPANSSIONFlag:%d\n",DummyFlg,CondiPassFlag,EXPANSSIONFlag);fflush(stdout);
						if(DummyFlg >0)
						{
							EXPANSSIONFlag=0;
						}
						else if (CondiPassFlag >0)
						{
							if ((tc_strcmp(gPart_typ,"G")==0)|| (tc_strcmp(gPart_class,"t5GrpID")==0))
							{
								EXPANSSIONFlag=0;
								GrpGrpFun=0;
								GrpGrpFun = tm_POCheckStopAtFGrpBOMline(GchildBoMLine[iCnt],sDml_TP,sEESWPrtType);
								printf("\n PO:GrpGrpFun: %d",GrpGrpFun);fflush(stdout);
								if(GrpGrpFun >0)
								{
									printf("\n PO:Grp:P1:first grp child part Failed: %s",gPart_nob);	fflush(stdout);
								}
								else
								{
									GrpIdSuccessFlag=1;
									break;
								}
								
							}
							else
							{
								EEPartByPass=0;
								if((tc_strcmp(gPart_class,"T5_EE_PartRevision")==0)||(tc_strcmp(gChld_type,"T5_EE_PartRevision")==0)||(tc_strcmp(gChld_type,"EE Part Revision")==0))
								{
									//Query EE part and take Swft part type.
									//if(ITEM_find_item (gPart_nob, &EEitem_tag)!=ITK_ok)PrintErrorStack();
									EEitem_tag= t5GetItemRevison(gPart_nob);
									if(EEitem_tag==NULLTAG)
									{
										printf("\n P2:EEitem_tag is Nnull...\n");fflush(stdout);
									}
									else
									{
										printf("\n P2:EEitem_tag is fond...\n");fflush(stdout);
										if(ITEM_ask_latest_rev (EEitem_tag, &EEpart_tag)!=ITK_ok)PrintErrorStack();
										if(EEpart_tag==NULLTAG)
										{
											printf("\n PO1:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
											return 0;
										}
										else
										{
											if(AOM_ask_value_string(EEpart_tag,"t5_SwPartType",&sEEswTyp)!=ITK_ok)PrintErrorStack();
											printf("\n PO1:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
											if(strstr(sEESWPrtType,sEEswTyp)!=NULL)
											{
												printf("\n PO1: Bypassing EE part for PO.\n");fflush(stdout);
												EEPartByPass=1;
											}
										}
									}
								}
								if (EEPartByPass ==0)
								{
									//bypass colour parts.
									if((strlen(gPart_nob)==14) && ((tc_strcmp(gPartColInd,"N")==0)|| (tc_strcmp(gPartColInd,"Y")==0)))
									{
										printf("\n PO: Bypass 14 digit parts for PO:[%s].\n",gPart_nob);fflush(stdout);
									}
									else
									{
										//Owners Manual.
										printf("\n PO:P3:Checkinging for Owners Manual:[%s : %s :%s].\n",gPart_nob,gpartDesgGrp,gPart_DR);fflush(stdout);
										/*if((tc_strcmp(gpartDesgGrp,"58")==0)&& ((tc_strcmp(gPart_DR,"DR4")==0)|| (tc_strcmp(gPart_DR,"AR4")==0)))
										{
											//MOHAN: Need to check for detailed logic.
											printf("\n PO: Owners Manual bypassed:[%s].\n",gPart_nob);fflush(stdout);
										}
										else
										{*/
											//Going to check PO details.
											//if(ITEM_find_item (gPart_nob, &All_item_tgs)!=ITK_ok)   PrintErrorStack();
											All_item_tgs= t5GetItemRevison(gPart_nob);
											if(All_item_tgs==NULLTAG)
											{
												printf("\n P25:All_item_tgs is Nnull...\n");fflush(stdout);
											}
											else
											{
												printf("\n P25:All_item_tgs is fond...\n");fflush(stdout);
												if(ITEM_list_all_revs (All_item_tgs, &rev_count, &Grevsn_list)!=ITK_ok)   PrintErrorStack();
												printf("\n PO:..Total rev found: %d.", rev_count);fflush(stdout);
												if(rev_count > 0)
												{
													POAvailFlag=0;
													for(ii=rev_count-1;ii>=0;ii--)
													{
														UomFlag=0;
														POAvailFlag=0;
														if(AOM_UIF_ask_value (Grevsn_list[ii], "item_revision_id", &Child_rev_idd)!=ITK_ok)PrintErrorStack();
														if(tc_strcmp(gPart_class,"T5_ClrPartRevision")!=0)
														{
														if(AOM_UIF_ask_value(Grevsn_list[ii],"t5_uom",&GChild_Prt_Uom)!=ITK_ok)   PrintErrorStack();
														}
														printf("\n PO:Latest released rev is:%s GChild_Prt_Uom:%s", Child_rev_idd,GChild_Prt_Uom);fflush(stdout);
														if((tc_strcmp(GChild_Prt_Uom,"")==0)||(tc_strcmp(GChild_Prt_Uom,"each")==0)||(tc_strcmp(GChild_Prt_Uom,"4-Nos")==0))
														{
															if(WSOM_ask_release_status_list (Grevsn_list[ii], &Chld_Rel_Stus_Cunt, &Child_status_lst)!=ITK_ok)PrintErrorStack();
															//printf("\n PO:Chld_Rel_Stus_Cunt is:%d.", Chld_Rel_Stus_Cunt);fflush(stdout);
															if(Chld_Rel_Stus_Cunt > 0)
															{
																crr=0;
																for(crr=0;crr<Chld_Rel_Stus_Cunt;crr++)
																{
																	POAvailFlag=0;
																	//Checking PO availability on any revision.
																	if(AOM_ask_value_string (Child_status_lst[crr], "object_name", &Chld_Rel_Stus)!=ITK_ok)PrintErrorStack();
																	//printf("\n PO:P3:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																	//if((tc_strstr(Chld_Rel_Stus,"Released")!=NULL) || (tc_strstr(Chld_Rel_Stus,"Rlzd")!=NULL)||(tc_strstr(Chld_Rel_Stus,"ERC Released")!=NULL) || (tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL))
																	if(tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)
																	{
																		printf("\n PO:Latest released rev is:%s GChild_Prt_Uom:%s", Child_rev_idd,GChild_Prt_Uom);fflush(stdout);
																		printf("\n PO:P3:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																		if(GRM_list_secondary_objects_only(Grevsn_list[ii], NULLTAG, &sec_count, &sec_obj)!=ITK_ok)   PrintErrorStack();
																		printf("\n PO:sec_count: %d",sec_count);	fflush(stdout);	
																		for(jj=0; jj<sec_count; jj++)
																		{
																			POAvailFlag=0;
																			if(PartObjk) PartObjk=NULLTAG;
																			PartObjk=sec_obj[jj];
																			if(AOM_UIF_ask_value(PartObjk, "object_type", &sec_obj_type)!=ITK_ok)   PrintErrorStack();
																			//printf("\n PO:.sec_obj_type..:%s", sec_obj_type);fflush(stdout);
																			if((tc_strcmp(sec_obj_type, "Design Revision Master") == 0) ||(tc_strcmp(sec_obj_type, "EE Part Revision Master") == 0) ||(tc_strcmp(sec_obj_type, "Clr Part Revision Master") == 0))
																			{
																				printf("\n PO:.sec_obj_type..:%s", sec_obj_type);fflush(stdout);
																				//PO availability Fun call
																				POAvailFlag=0;
																				POAvailFlag = tm_CheckPOAvalability(PartObjk);
																				printf("\n P3:SIGNOFF:POAvailFlag.: %d",POAvailFlag);fflush(stdout);
																				if(POAvailFlag >0)
																				{
																					//PO Available so break.
																					if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
																					{
																						tc_strcpy (POAvailPartSet,"" );
																						tc_strcat (POAvailPartSet,gPart_nob);
																						printf("\n PO:F:P43:Part added to POAvailPartSet: %s",gPart_nob);	fflush(stdout);	
																					}
																					else
																					{
																						if(strstr(POAvailPartSet,gPart_nob)==NULL)
																						{
																						tc_strcat (POAvailPartSet,"," );
																						tc_strcat (POAvailPartSet,gPart_nob);
																						printf("\n PO:F:P23:Part added to POAvailPartSet: %s",gPart_nob);	fflush(stdout);
																						}
																					}
																					printf("\n PO:P33:PO availale for part:%s ",gPart_nob);	fflush(stdout);
																				}
																				//below break for Desg Master form.
																				break;
																			}
																		}
																		if (POAvailFlag >0)
																		{
																			break;
																		}
																	}
																}
																if (POAvailFlag >0)
																{
																	break;
																}
															}
														}
														else
														{
															printf("\n PO:P33:Uom is not 4 for part:%s ",gPart_nob);	fflush(stdout);
															POAvailFlag=0;
															UomFlag=1;
															break;
														}
													}
												}
												if(UomFlag==0)
												{
													if (POAvailFlag>0)
													{
														GrpIdSuccessFlag=1;
														break;
													}
												}
											}
										//}
									}
								}
								else
								{
									printf("\n PO:Grp: Grp child part skipped as sw part type b ypassed.: %s",gPart_nob);	fflush(stdout);
									//GrpIdSuccessFlag=1;
									//break;
								}
							}
						}
					}
					else
					{
						//only expanssion.
						if((tc_strcmp(gPart_typ,"G")==0)|| (tc_strcmp(gPart_class,"t5GrpID")==0))
						{
							GrpGrpFun=0;
							GrpGrpFun = tm_POCheckStopAtFGrpBOMline(GchildBoMLine[iCnt],sDml_TP,sEESWPrtType);
							printf("\n PO:GrpGrpFun: %d",GrpGrpFun);fflush(stdout);
							if(GrpGrpFun >0)
							{
								printf("\n PO:Grp: first grp child part Failed.: %s",gPart_nob);	fflush(stdout);
							}
							else
							{
								GrpIdSuccessFlag=1;
								break;
							}
						}
						else
						{
							printf("\n PO:Grp:E50 Checking.: %s",gPart_nob);	fflush(stdout);
							GrpE50Fun=0;
							GrpE50Fun = tm_POCheckStopAtFE50BOMline(GchildBoMLine[iCnt],sDml_TP,sEESWPrtType);
							printf("\n PO:GrpE50Fun: %d",GrpE50Fun);fflush(stdout);
							if(GrpE50Fun >0)
							{
								printf("\n PO:Grp:E50 first child part Failed.: %s",gPart_nob);	fflush(stdout);
							}
							else
							{
								printf("\n PO:Grp:E50 first child part PASS.: %s",gPart_nob);	fflush(stdout);
								GrpIdSuccessFlag=1;
								break;
							}
						}
					}
				}
				//Expanding E50/E99 parts.
			}
		}
	}
	else
	{
		GrpIdSuccessFlag=1;
		printf("\n PO:Grp:ALLF18:P1:Grp id added to BypassPartSet: %s",grPart_nob);	fflush(stdout);
		if((BypassPartSet==NULL)|| (tc_strcmp(BypassPartSet,"")==0))
		{
			tc_strcpy (BypassPartSet,"" );
			//tc_strcpy (BypassPartSet,"," );
			tc_strcat (BypassPartSet,grPart_nob);
			printf("\n PO:Grp:F18:P1:Part added to NOPOset: %s",grPart_nob);	fflush(stdout);	
		}
		else
		{
			if(strstr(BypassPartSet,grPart_nob)==NULL)
			{
			tc_strcat (BypassPartSet,"," );
			tc_strcat (BypassPartSet,grPart_nob);
			printf("\n PO:Grp:F18:P2:Part added to NOPOset: %s",grPart_nob);	fflush(stdout);
			}
		}
	}
	if (GrpIdSuccessFlag >0)
	{
		printf("\n PO:Grp:success Grp case: %s",grPart_nob);	fflush(stdout);
	}
	else
	{
		printf("\n PO:Grp:failed Grp case: %s",grPart_nob);	fflush(stdout);
		return 1;
	}
	//if(TskObjSO) MEM_free(TskObjSO);
	
	return 0;
}
//for grp id direct attached.
int tm_POCheckStopAtFGrp(tag_t Grp_tag,char* sDml_TP,char* sEESWPrtType)
{
	int ii=0;
	int jj=0;
	int crr=0;
	int rev_count=0;
	int sec_count=0;
	int iCnt=0;
	int GrpE50Fun=0;
	int AllF18CSFlag=0;
	int CondiPassFlag=0;
	int CSFoundFlag=0;
	int GchldLineCunt=0;
	int EEPartByPass=0;
	int EXPANSSIONFlag=0;
	int Chld_Rel_Stus_Cunt=0;
	int DummyFlg=0;
	int GrpGrpFun=0;
	int F18CSFoundFlag=0;
	int POAvailFlag=0;
	int GrpIdSuccessFlag=0;
	char*	 Chld_Rel_Stus = NULL;
	char*	 grPart_nob = NULL;
	char*	 gPart_nob = NULL;
	char*	 sPartCS = NULL;
	char*	 gPart_class = NULL;
	char*	 gPart_DR = NULL;
	char*	 gPart_typ = NULL;
	char*	 gPartProj = NULL;
	char*	 gPartColInd = NULL;
	char*	 gpartDesgGrp = NULL;
	char*	 sEEswTyp = NULL;
	char*	 sec_obj_type = NULL;
	char*	 gChld_type = NULL;
	char*	 Child_rev_idd = NULL;
	char*	 GChild_Prt_Uom = NULL;
	tag_t All_item_tgs = NULLTAG;
	tag_t EEpart_tag = NULLTAG;
	tag_t PartObjk = NULLTAG;
	tag_t EEitem_tag = NULLTAG;
	tag_t gClsTg = NULLTAG;
	tag_t GP_tag = NULLTAG;
	tag_t GPrt_tag = NULLTAG;
	tag_t *GchildBoMLine=NULLTAG;
	tag_t	*sec_obj		= NULL;
	tag_t	*Grevsn_list		= NULL;
	tag_t	*Child_status_lst		= NULL;
	tag_t	tagRuleGrp	= NULLTAG;
	tag_t	tagBOMwindowGrp 	= NULLTAG;
	tag_t	tagBOMlineGrp 	= NULLTAG;
	int		childCountTopBomLineGrp	= 0;
	int		rulefoundd	= 0;
	char **POrulenamee = NULL;
	char **POrulevaluee = NULL;
	tag_t POclose_tagg = NULLTAG;
	tag_t *POclosurerulee = NULLTAG;

	printf("\n PO:Grp:Inside CS check function:TP:%s\n",sDml_TP);fflush(stdout);
	if(AOM_ask_value_string(Grp_tag,"item_id",&grPart_nob)!=ITK_ok)   PrintErrorStack();
	printf("\n PO:Grp..: grPart_nob2:%s\n",grPart_nob);fflush(stdout);

	if(BOM_create_window(&tagBOMwindowGrp)!=ITK_ok)   PrintErrorStack();
	if(CFM_find("ERC release and above", &tagRuleGrp)!=ITK_ok)   PrintErrorStack(); 
	if(BOM_set_window_config_rule( tagBOMwindowGrp, tagRuleGrp )!=ITK_ok)   PrintErrorStack();
	//0-qty skip
	if(PIE_find_closure_rules("TMLBOMExpandByQty",PIE_TEAMCENTER, &rulefoundd, &POclosurerulee )!=ITK_ok)   PrintErrorStack();
	printf ("PO:rulefoundd count %d \n",rulefoundd);fflush(stdout);
	if (rulefoundd > 0)
	{
		POclose_tagg = POclosurerulee[0];
		printf ("PO:closure rule found \n");fflush(stdout);
	}
	if(BOM_window_set_closure_rule( tagBOMwindowGrp,POclose_tagg, 0, POrulenamee,POrulevaluee )!=ITK_ok)   PrintErrorStack();
	//0-qty skip

	if(BOM_set_window_top_line(tagBOMwindowGrp, NULLTAG, Grp_tag, NULLTAG, &tagBOMlineGrp)!=ITK_ok)   PrintErrorStack();

	//	if(BOM_line_ask_child_lines(tagBOMlineGrp, &childCountTopBomLineGrp, &tagTopBLchildLineGrp)!=ITK_ok)   PrintErrorStack();
	//	printf("\n PO:FLP:No of child objects are n : %d\n",childCountTopBomLineGrp);fflush(stdout);
	//	for(cntGrp =0 ; cntGrp < childCountTopBomLineGrp; cntGrp++)
	//	{
	//		printf("\n PO:MR:No of child taken : %d Part_no:%s\n",cntGrp,Part_no);fflush(stdout);
	//		
	//		FunOutflag=0;
	//		FunOutflag= tm_POCheckStopAtFBOMline(tagTopBLchildLineGrp[cntGrp],sDml_TP,sEESWPrtType);
	//		printf("\n PO:P1:FunOutflag: %s :%d",Part_no,FunOutflag);fflush(stdout);
	//	}

	if(BOM_line_ask_child_lines(tagBOMlineGrp, &GchldLineCunt, &GchildBoMLine)!=ITK_ok)   PrintErrorStack();
	printf("\n PO:SLP11:No of child objects are for Parent Part:%s: %d\n",grPart_nob,GchldLineCunt);fflush(stdout);
	//Checking F18 Case
	for(iCnt =0 ; iCnt < GchldLineCunt; iCnt++)
	{
		AllF18CSFlag=0;
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_item_item_id",&gPart_nob)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PartStatus",&gPart_DR)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PartType",&gPart_typ)!=ITK_ok)   PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_ProjectCode",&gPartProj)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_ColourInd",&gPartColInd)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_DesignGrp",&gpartDesgGrp)!=ITK_ok)PrintErrorStack();
		if(AOM_ask_value_string(GchildBoMLine[iCnt],"fnd0bl_line_object_type",&gChld_type)!=ITK_ok)PrintErrorStack();
		//Query part and take class.
		//if(ITEM_find_item (gPart_nob, &GP_tag)!=ITK_ok)PrintErrorStack();
		GP_tag= t5GetItemRevison(gPart_nob);
		if(GP_tag==NULLTAG)
		{
			printf("\n P25:GP_tag is Nnull...\n");fflush(stdout);
		}
		else
		{
			printf("\n P25:GP_tag is fond...\n");fflush(stdout);
			if(ITEM_ask_latest_rev (GP_tag, &GPrt_tag)!=ITK_ok)PrintErrorStack();
			if(GPrt_tag==NULLTAG)
			{
				printf("\n PO2:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
				return 0;
			}
			else
			{
				if(POM_class_of_instance(GPrt_tag,&gClsTg)!=ITK_ok)   PrintErrorStack();
				if(POM_name_of_class(gClsTg,&gPart_class)!=ITK_ok)   PrintErrorStack();
			}
		}
		printf("\n PO1:Grp: Part no:%s gPart_class:%s gPart_typ:%s gPartProj:%s gPartColInd:%s gpartDesgGrp:%s gChld_type:%s",gPart_nob,gPart_class,gPart_typ,gPartProj,gPartColInd,gpartDesgGrp,gChld_type);fflush(stdout);	

		//Skipping Dummy parts and spare kit
		if(tc_strcmp(gPart_typ,"D")==0 || tc_strcmp(gPart_typ,"DA")==0 ||tc_strcmp(gPart_typ,"DC")==0 || tc_strcmp(gPart_typ,"IFD")==0|| tc_strcmp(gPart_typ,"IM" )==0||(tc_strcmp(gPart_class,"t5SpKit")==0))
		{
			printf("\n PO:Grp:Part skipped for PO check: %s",gPart_nob);	fflush(stdout);	
		}
		else
		{
			if (tc_strcmp(sDml_TP,"CARPLANT")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO1:Grp:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO:Grp:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);		
			}
			else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO:Grp:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO:Grp:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO:Grp:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
			}
			else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO:Grp:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO:Grp:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"PCVBU")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO:Grp:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else if (tc_strcmp(sDml_TP,"PUVBU")==0)
			{
				if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
				printf("\n PO:Grp:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
			}
			else
			{
				printf("\n PO:Grp:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
			}
			if((sPartCS==NULL)|| (tc_strcmp(sPartCS,"")==0)|| (tc_strcmp(sPartCS,"NA")==0))
			{
				AllF18CSFlag=1;
				printf("\n PO:Grp:target Plant CS is NULL so considered as NOPO: %s",gPart_nob);	fflush(stdout);	
				break;
			}
			else
			{
				if (tc_strcmp(sPartCS,"F18")!=0 && tc_strcmp(sPartCS,"F19")!=0)
				{
					AllF18CSFlag=1;
					printf("\n PO1:Grp:other than F18 CS fonud.\n");fflush(stdout);
					break;
				}
			}
		}
	}
	if (AllF18CSFlag >0)
	{
		iCnt =0 ;
		for(iCnt =0 ; iCnt < GchldLineCunt; iCnt++)
		{
			CSFoundFlag=0;
			F18CSFoundFlag=0;
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_item_item_id",&gPart_nob)!=ITK_ok)   PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PartStatus",&gPart_DR)!=ITK_ok)   PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PartType",&gPart_typ)!=ITK_ok)   PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_ProjectCode",&gPartProj)!=ITK_ok)PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_ColourInd",&gPartColInd)!=ITK_ok)PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_DesignGrp",&gpartDesgGrp)!=ITK_ok)PrintErrorStack();
			if(AOM_ask_value_string(GchildBoMLine[iCnt],"fnd0bl_line_object_type",&gChld_type)!=ITK_ok)PrintErrorStack();
			printf("\n PO:Grp:gPart_nob.. %s",gPart_nob);	fflush(stdout);
			//Query part and take class.
			//if(ITEM_find_item (gPart_nob, &GP_tag)!=ITK_ok)PrintErrorStack();
			GP_tag= t5GetItemRevison(gPart_nob);
			if(GP_tag==NULLTAG)
			{
				printf("\n P26:GP_tag is Nnull...\n");fflush(stdout);
			}
			else
			{
				printf("\n P26:GP_tag is fond...\n");fflush(stdout);
				if(ITEM_ask_latest_rev (GP_tag, &GPrt_tag)!=ITK_ok)PrintErrorStack();
				if(GPrt_tag==NULLTAG)
				{
					printf("\n PO4:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
					return 0;
				}
				else
				{
					if(POM_class_of_instance(GPrt_tag,&gClsTg)!=ITK_ok)   PrintErrorStack();
					if(POM_name_of_class(gClsTg,&gPart_class)!=ITK_ok)   PrintErrorStack();
				}
			}
			printf("\n PO4:Grp: Part no:%s gPart_class:%s gPart_typ:%s gPartProj:%s gPartColInd:%s gpartDesgGrp:%s gChld_type:%s",gPart_nob,gPart_class,gPart_typ,gPartProj,gPartColInd,gpartDesgGrp,gChld_type);fflush(stdout);	

			//Skipping Dummy parts and spare kit
			if(tc_strcmp(gPart_typ,"D")==0 || tc_strcmp(gPart_typ,"DA")==0 ||tc_strcmp(gPart_typ,"DC")==0 || tc_strcmp(gPart_typ,"IFD")==0|| tc_strcmp(gPart_typ,"IM" )==0 ||(tc_strcmp(gPart_class,"t5SpKit")==0))
			{
				printf("\n PO4:Grp:Part typ eDummy skipp for PO %s",gPart_nob);	fflush(stdout);
			}
			else
			{
				if (tc_strcmp(sDml_TP,"CARPLANT")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);
				}
				else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
				}
				else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"PCVBU")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else if (tc_strcmp(sDml_TP,"PUVBU")==0)
				{
					if(AOM_ask_value_string(GchildBoMLine[iCnt],"bl_Design Revision_t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
					printf("\n PO2:Grp:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
				}
				else
				{
					printf("\n PO2:Grp:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
				}
				if((sPartCS==NULL)|| (tc_strcmp(sPartCS,"")==0)|| (tc_strcmp(sPartCS,"NA")==0))
				{
					printf("\n PO2:Grp:target Plant CS is NULL so considered as NOPO: %s",gPart_nob);	fflush(stdout);	
				}
				else
				{
					if(tc_strcmp(sPartCS,"F")==0 || tc_strcmp(sPartCS,"F30")==0|| tc_strcmp(sPartCS,"F35")==0 || tc_strcmp(sPartCS,"F40")==0)
					{
						printf("\n PO2:Grp:F CS fonud.\n");fflush(stdout);
						CSFoundFlag=1;
						F18CSFoundFlag=0;
					}
					else if (tc_strcmp(sPartCS,"F18")==0 || tc_strcmp(sPartCS,"F19")==0)
					{
						printf("\n PO2:Grp:F18 CS fonud.\n");fflush(stdout);
						F18CSFoundFlag=1;
					}
					else
					{
						printf("\n PO2:Grp:Found Other CS.\n");fflush(stdout);
						CSFoundFlag=0;
						F18CSFoundFlag=0;
					}

					if (F18CSFoundFlag >0)
					{
						EXPANSSIONFlag=0;
					}
					else if (CSFoundFlag >0)
					{
						DummyFlg=0;
						CondiPassFlag=0;
						EXPANSSIONFlag=0;
						printf("\n PO2:Grp:.Checking conditions..:[%s] \n",gPart_nob);fflush(stdout);

						if(tc_strcmp(gPart_typ,"D")==0 || tc_strcmp(gPart_typ,"DA")==0 ||tc_strcmp(gPart_typ,"DC")==0 || tc_strcmp(gPart_typ,"IFD")==0|| tc_strcmp(gPart_typ,"IM" )==0 ||(tc_strcmp(gPart_class,"t5SpKit")==0))
						{
							printf("\n PO2:A:.Obsolete/Dummy part skipped....:[%s]\n",gPart_nob);fflush(stdout);
							DummyFlg=1;				
						}
						else if((tc_strcmp(gPartProj,"1111")==0)||(tc_strcmp(gPart_class,"T5_EE_PartRevision")==0)||(tc_strcmp(gPart_typ,"G")==0)|| (tc_strcmp(gPart_class,"t5GrpID")==0)||((strlen(gPart_nob)!=11)&& ((tc_strcmp(gPartColInd,"Y")==0)||(tc_strcmp(gPartColInd,"N")==0)||(tc_strcmp(gPartColInd,"C")==0))&&((tc_strcmp(gPart_typ,"A")==0)||(tc_strcmp(gPart_typ,"C")==0)||(tc_strcmp(gPart_typ,"M")==0))))
						{
							printf("\n PO2:.Part eligible for PO check:[%s]\n",gPart_nob);fflush(stdout);
							CondiPassFlag=1;
						}
						else
						{
							printf("\n PO2:P3:NO expanssion for DR3 and below CS-F parts:[%s]\n",gPart_nob);fflush(stdout);
							EXPANSSIONFlag=1;
						}
						printf("\n PO2:DummyFlg:%d CondiPassFlag:%d EXPANSSIONFlag:%d\n",DummyFlg,CondiPassFlag,EXPANSSIONFlag);fflush(stdout);
						if(DummyFlg >0)
						{
							EXPANSSIONFlag=0;
						}
						else if (CondiPassFlag >0)
						{
							if ((tc_strcmp(gPart_typ,"G")==0)|| (tc_strcmp(gPart_class,"t5GrpID")==0))
							{
								EXPANSSIONFlag=0;
								GrpGrpFun=0;
								GrpGrpFun = tm_POCheckStopAtFGrpBOMline(GchildBoMLine[iCnt],sDml_TP,sEESWPrtType);
								printf("\n PO2:GrpGrpFun: %d",GrpGrpFun);fflush(stdout);
								if(GrpGrpFun >0)
								{
									printf("\n PO2:Grp:P1:first grp child part Failed: %s",gPart_nob);	fflush(stdout);
								}
								else
								{
									GrpIdSuccessFlag=1;
									break;
								}
								
							}
							else
							{
								EEPartByPass=0;
								if((tc_strcmp(gPart_class,"T5_EE_PartRevision")==0)||(tc_strcmp(gChld_type,"T5_EE_PartRevision")==0)||(tc_strcmp(gChld_type,"EE Part Revision")==0))
								{
									//Query EE part and take Swft part type.
									//if(ITEM_find_item (gPart_nob, &EEitem_tag)!=ITK_ok)PrintErrorStack();
									EEitem_tag= t5GetItemRevison(gPart_nob);
									if(EEitem_tag==NULLTAG)
									{
										printf("\n P27:EEitem_tag is Nnull...\n");fflush(stdout);
									}
									else
									{
										printf("\n P27:EEitem_tag is fond...\n");fflush(stdout);
										if(ITEM_ask_latest_rev (EEitem_tag, &EEpart_tag)!=ITK_ok)PrintErrorStack();
										if(EEpart_tag==NULLTAG)
										{
											printf("\n PO5:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
											return 0;
										}
										else
										{
											if(AOM_ask_value_string(EEpart_tag,"t5_SwPartType",&sEEswTyp)!=ITK_ok)PrintErrorStack();
											printf("\n PO5:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
											if(strstr(sEESWPrtType,sEEswTyp)!=NULL)
											{
												printf("\n PO5: Bypassing EE part for PO.\n");fflush(stdout);
												EEPartByPass=1;
											}
										}
									}
								}
								if (EEPartByPass ==0)
								{
									//bypass colour parts.
									if((strlen(gPart_nob)==14) && ((tc_strcmp(gPartColInd,"N")==0)|| (tc_strcmp(gPartColInd,"Y")==0)))
									{
										printf("\n PO5: Bypass 14 digit parts for PO:[%s].\n",gPart_nob);fflush(stdout);
									}
									else
									{
										//Owners Manual.
										printf("\n PO5:P3:Checkinging for Owners Manual:[%s : %s :%s].\n",gPart_nob,gpartDesgGrp,gPart_DR);fflush(stdout);
										/*if((tc_strcmp(gpartDesgGrp,"58")==0)&& ((tc_strcmp(gPart_DR,"DR4")==0)|| (tc_strcmp(gPart_DR,"AR4")==0)))
										{
											//MOHAN: Need to check for detailed logic.
											printf("\n PO: Owners Manual bypassed:[%s].\n",gPart_nob);fflush(stdout);
										}
										else
										{*/
											//Going to check PO details.
											//if(ITEM_find_item (gPart_nob, &All_item_tgs)!=ITK_ok)   PrintErrorStack();
											All_item_tgs= t5GetItemRevison(gPart_nob);
											if(All_item_tgs==NULLTAG)
											{
												printf("\n P27:All_item_tgs is Nnull...\n");fflush(stdout);
											}
											else
											{
												printf("\n P27:All_item_tgs is fond...\n");fflush(stdout);
												if(ITEM_list_all_revs (All_item_tgs, &rev_count, &Grevsn_list)!=ITK_ok)   PrintErrorStack();
												printf("\n PO:..Total rev found: %d.", rev_count);fflush(stdout);
												if(rev_count > 0)
												{
													POAvailFlag=0;
													for(ii=rev_count-1;ii>=0;ii--)
													{
														POAvailFlag=0;
														if(AOM_UIF_ask_value (Grevsn_list[ii], "item_revision_id", &Child_rev_idd)!=ITK_ok)PrintErrorStack();
														if(tc_strcmp(gPart_class,"T5_ClrPartRevision")!=0)
														{
														if(AOM_UIF_ask_value(Grevsn_list[ii],"t5_uom",&GChild_Prt_Uom)!=ITK_ok)   PrintErrorStack();
														}
														//printf("\n PO4:Latest released rev is:%s GChild_Prt_Uom:%s.", Child_rev_idd,GChild_Prt_Uom);fflush(stdout);
														if((tc_strcmp(GChild_Prt_Uom,"")==0)||(tc_strcmp(GChild_Prt_Uom,"each")==0)||(tc_strcmp(GChild_Prt_Uom,"4-Nos")==0))
														{
															if(WSOM_ask_release_status_list (Grevsn_list[ii], &Chld_Rel_Stus_Cunt, &Child_status_lst)!=ITK_ok)PrintErrorStack();
															//printf("\n PO:Chld_Rel_Stus_Cunt is:%d.", Chld_Rel_Stus_Cunt);fflush(stdout);
															if(Chld_Rel_Stus_Cunt > 0)
															{
																crr=0;
																for(crr=0;crr<Chld_Rel_Stus_Cunt;crr++)
																{
																	POAvailFlag=0;
																	//Checking PO availability on any revision.
																	if(AOM_ask_value_string (Child_status_lst[crr], "object_name", &Chld_Rel_Stus)!=ITK_ok)PrintErrorStack();
																	//printf("\n PO:P3:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																	//if((tc_strstr(Chld_Rel_Stus,"Released")!=NULL) || (tc_strstr(Chld_Rel_Stus,"Rlzd")!=NULL)||(tc_strstr(Chld_Rel_Stus,"ERC Released")!=NULL) || (tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL))
																	if(tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)
																	{
																		printf("\n PO4:Latest released rev is:%s GChild_Prt_Uom:%s.", Child_rev_idd,GChild_Prt_Uom);fflush(stdout);
																		printf("\n PO:P3:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																		if(GRM_list_secondary_objects_only(Grevsn_list[ii], NULLTAG, &sec_count, &sec_obj)!=ITK_ok)   PrintErrorStack();
																		printf("\n PO:sec_count: %d",sec_count);	fflush(stdout);	
																		for(jj=0; jj<sec_count; jj++)
																		{
																			POAvailFlag=0;
																			if(PartObjk) PartObjk=NULLTAG;
																			PartObjk=sec_obj[jj];
																			if(AOM_UIF_ask_value(PartObjk, "object_type", &sec_obj_type)!=ITK_ok)   PrintErrorStack();
																			//printf("\n PO:.sec_obj_type..:%s", sec_obj_type);fflush(stdout);
																			if((tc_strcmp(sec_obj_type, "Design Revision Master") == 0) ||(tc_strcmp(sec_obj_type, "EE Part Revision Master") == 0) ||(tc_strcmp(sec_obj_type, "Clr Part Revision Master") == 0))
																			{
																				printf("\n PO:.sec_obj_type..:%s", sec_obj_type);fflush(stdout);
																				//PO availability Fun call
																				POAvailFlag=0;
																				POAvailFlag = tm_CheckPOAvalability(PartObjk);
																				printf("\n P3:SIGNOFF:POAvailFlag.: %d",POAvailFlag);fflush(stdout);
																				if(POAvailFlag >0)
																				{
																					//PO Available so break.
																					if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
																					{
																						tc_strcpy (POAvailPartSet,"" );
																						tc_strcat (POAvailPartSet,gPart_nob);
																						printf("\n PO:F:P43:Part added to POAvailPartSet: %s",gPart_nob);	fflush(stdout);	
																					}
																					else
																					{
																						if(strstr(POAvailPartSet,gPart_nob)==NULL)
																						{
																						tc_strcat (POAvailPartSet,"," );
																						tc_strcat (POAvailPartSet,gPart_nob);
																						printf("\n PO:F:P23:Part added to POAvailPartSet: %s",gPart_nob);	fflush(stdout);
																						}
																					}
																					printf("\n PO:P33:PO availale for part:%s ",gPart_nob);	fflush(stdout);
																				}
																				//below break for Desg Master form.
																				break;
																			}
																		}
																		if (POAvailFlag >0)
																		{
																			break;
																		}
																	}
																}
																if (POAvailFlag >0)
																{
																	break;
																}
															}
														}
													}	
												}
												if (POAvailFlag>0)
												{
													GrpIdSuccessFlag=1;
													break;
												}
											}
										//}
									}
								}
								else
								{
									printf("\n PO:Grp: Grp child part skipped as sw part type b ypassed.: %s",gPart_nob);	fflush(stdout);
									//GrpIdSuccessFlag=1;
									//break;
								}
							}
						}
					}
					else
					{
						//only expanssion.
						if((tc_strcmp(gPart_typ,"G")==0)|| (tc_strcmp(gPart_class,"t5GrpID")==0))
						{
							GrpGrpFun=0;
							GrpGrpFun = tm_POCheckStopAtFGrpBOMline(GchildBoMLine[iCnt],sDml_TP,sEESWPrtType);
							printf("\n PO:GrpGrpFun: %d",GrpGrpFun);fflush(stdout);
							if(GrpGrpFun >0)
							{
								printf("\n PO:Grp: first grp child part Failed.: %s",gPart_nob);	fflush(stdout);
							}
							else
							{
								GrpIdSuccessFlag=1;
								break;
							}
						}
						else
						{
							printf("\n PO:Grp:E50 Checking.: %s",gPart_nob);	fflush(stdout);
							GrpE50Fun=0;
							GrpE50Fun = tm_POCheckStopAtFE50BOMline(GchildBoMLine[iCnt],sDml_TP,sEESWPrtType);
							printf("\n PO:GrpE50Fun: %d",GrpE50Fun);fflush(stdout);
							if(GrpE50Fun >0)
							{
								printf("\n PO:Grp:E50 first child part Failed.: %s",gPart_nob);	fflush(stdout);
							}
							else
							{
								printf("\n PO:Grp:E50 first child part PASS.: %s",gPart_nob);	fflush(stdout);
								GrpIdSuccessFlag=1;
								break;
							}
						}
					}
				}
				//Expanding E50/E99 parts.
			}
		}
	}
	else
	{
		GrpIdSuccessFlag=1;
		printf("\n PO:Grp:ALLF18:P1:Grp id added to BypassPartSet: %s",grPart_nob);	fflush(stdout);
		if((BypassPartSet==NULL)|| (tc_strcmp(BypassPartSet,"")==0))
		{
			tc_strcpy (BypassPartSet,"" );
			//tc_strcpy (BypassPartSet,"," );
			tc_strcat (BypassPartSet,grPart_nob);
			printf("\n PO:Grp:F18:P1:Part added to NOPOset: %s",grPart_nob);	fflush(stdout);	
		}
		else
		{
			if(strstr(BypassPartSet,grPart_nob)==NULL)
			{
			tc_strcat (BypassPartSet,"," );
			tc_strcat (BypassPartSet,grPart_nob);
			printf("\n PO:Grp:F18:P2:Part added to NOPOset: %s",grPart_nob);	fflush(stdout);
			}
		}
	}
	if (GrpIdSuccessFlag >0)
	{
		printf("\n PO:Grp:success Grp case: %s",grPart_nob);	fflush(stdout);
	}
	else
	{
		printf("\n PO1:Grp:failed Grp case: %s",grPart_nob);	fflush(stdout);
		return 1;
	}
	//if(TskObjSO) MEM_free(TskObjSO);
	
	return 0;
}

//For part attached to task.
int tm_POCheckStopAtFFF(tag_t AssyTg,char* sDml_TP,char* sEESWPrtType)
{
	int DRstusFlag = 0;
	int	CSFoundFlag=0;
	int	CondiPassFlag=0;
	int	F18CSFoundFlag=0;
	int	EEPartByPass=0;
	int	POAvailFlag=0;
	int	NwNOPOPrtFlg=0;
	int	EXPANSSIONFlag=0;
	int	DummyFlg=0;
	int	UomFlag=0;
	int	GrpFunflag=0;
	int	rev_count=0;
	int crr=0;
	int ii=0;
	int iii=0;
	int jj=0;
	int sec_count=0;
	int RelRevFlag=0;
	int flgPartAtachedToTsk=0;
	int PORevAvailFlag=0;
	int ColPrtFlag=0;
	int POfoundFlg=0;
	int Chld_Rel_Stus_Cunt=0;
	char	*sEEswTyp		= NULL;
	char	*sec_obj_type		= NULL;
	char	*Chld_Rel_Stus		= NULL;
	char*	 fPart_nob = NULL;
	char*	 fPart_DR = NULL;
	char*	 fPart_typ = NULL;
	char*	 fPartProj = NULL;
	char*	 fPartColInd = NULL;
	char*	 fpartDesgGrp = NULL;
	char*	 fpartDrwind = NULL;
	char*	 fPart_class = NULL;
	char*	 Child_rev_idd = NULL;
	char*	 Chld_Prt_Uom = NULL;
	char*	 fPartObjTp = NULL;
	char*	 fPrtRevv = NULL;
	char*	 sPartCS = NULL;
	tag_t	EEpart_tag	= NULLTAG;
	tag_t	PartObj	= NULLTAG;
	tag_t	EEitem_tag	= NULLTAG;
	tag_t	All_item_tgs	= NULLTAG;
	tag_t	*Child_status_lst		= NULLTAG;
	tag_t fClsId = NULLTAG;
	tag_t	*sec_obj		= NULLTAG;
	tag_t	*revsn_list		= NULLTAG;

	printf("\n PO:Inside CS check function:TP:%s\n",sDml_TP);fflush(stdout);
	if(AOM_ask_value_string(AssyTg,"item_id",&fPart_nob)!=ITK_ok)   PrintErrorStack();
	if(AOM_ask_value_string(AssyTg,"t5_PartStatus",&fPart_DR)!=ITK_ok)   PrintErrorStack();
	if(AOM_ask_value_string(AssyTg,"t5_PartType",&fPart_typ)!=ITK_ok)   PrintErrorStack();
	if(AOM_ask_value_string(AssyTg,"t5_ProjectCode",&fPartProj)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(AssyTg,"t5_ColourInd",&fPartColInd)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(AssyTg,"t5_DesignGrp",&fpartDesgGrp)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(AssyTg,"t5_DrawingInd",&fpartDrwind)!=ITK_ok)PrintErrorStack();
	if(AOM_ask_value_string(AssyTg,"object_type",&fPartObjTp)!=ITK_ok)PrintErrorStack();
	if(AOM_UIF_ask_value (AssyTg, "item_revision_id", &fPrtRevv)!=ITK_ok)PrintErrorStack();
	if(POM_class_of_instance(AssyTg,&fClsId)!=ITK_ok)   PrintErrorStack();
	if(POM_name_of_class(fClsId,&fPart_class)!=ITK_ok)   PrintErrorStack();
	printf("\n PO:P41:Part no:%s fPart_class:%s fPart_typ:%s fPartProj:%s fPartColInd:%s fpartDesgGrp:%s fpartDrwind:%s :fPrtRevv:%s",fPart_nob,fPart_class,fPart_typ,fPartProj,fPartColInd,fpartDesgGrp,fpartDrwind,fPrtRevv);	fflush(stdout);	
	//Skipping Dummy parts and spare kit
	if(tc_strcmp(fPart_typ,"D")==0 || tc_strcmp(fPart_typ,"DA")==0 ||tc_strcmp(fPart_typ,"DC")==0 || tc_strcmp(fPart_typ,"IFD")==0|| tc_strcmp(fPart_typ,"IM" )==0 ||(tc_strcmp(fPart_class,"t5SpKit")==0))
	{
		printf("\n PO:P41:Dummy part skipped. %s",fPart_nob);	fflush(stdout);
	}
	else
	{
		if (tc_strcmp(sDml_TP,"CARPLANT")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
		}
		else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
		}
		else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"PCVBU")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
		}
		else if (tc_strcmp(sDml_TP,"PUVBU")==0)
		{
			if(AOM_ask_value_string(AssyTg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
			printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
		}
		else
		{
			printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
		}
		if((sPartCS==NULL)|| (tc_strcmp(sPartCS,"")==0)|| (tc_strcmp(sPartCS,"NA")==0))
		{
			printf("\n PO:target Plant CS is NULL so considered as NOPO: %s",fPart_nob);	fflush(stdout);	
			//ERROR: to add in NOPO set
			if((NOPOPartsSet==NULL)|| (tc_strcmp(NOPOPartsSet,"")==0))
			{
				tc_strcpy (NOPOPartsSet,"" );
				//tc_strcpy (NOPOPartsSet,"," );
				tc_strcat (NOPOPartsSet,fPart_nob);
				printf("\n PO:P1:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
			}
			else
			{
				if(strstr(NOPOPartsSet,fPart_nob)==NULL)
				{
				tc_strcat (NOPOPartsSet,"," );
				tc_strcat (NOPOPartsSet,fPart_nob);
				printf("\n PO:P22:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
				}
			}
		}
		else
		{
			if(tc_strcmp(sPartCS,"F")==0 || tc_strcmp(sPartCS,"F30")==0|| tc_strcmp(sPartCS,"F35")==0 || tc_strcmp(sPartCS,"F40")==0)
			{
				printf("\n F CS fonud.\n");fflush(stdout);
				CSFoundFlag=1;
			}
			else if (tc_strcmp(sPartCS,"F18")==0 || tc_strcmp(sPartCS,"F19")==0)
			{
				printf("\n F18 CS fonud.\n");fflush(stdout);
				F18CSFoundFlag=1;
			}
			else
			{
				printf(" Found Other CS.\n");fflush(stdout);
				CSFoundFlag=0;
				F18CSFoundFlag=0;
			}
			if (F18CSFoundFlag >0)
			{
				EXPANSSIONFlag=0;
				if((BypassPartSet==NULL)|| (tc_strcmp(BypassPartSet,"")==0))
				{
					tc_strcpy (BypassPartSet,"" );
					tc_strcat (BypassPartSet,fPart_nob);
					printf("\n PO:F18:P41::Part added to BypassPartSet: %s",fPart_nob);	fflush(stdout);	
				}
				else
				{
					if(strstr(BypassPartSet,fPart_nob)==NULL)
					{
						tc_strcat (BypassPartSet,"," );
						tc_strcat (BypassPartSet,fPart_nob);
						printf("\n PO:F18:P41:Part added to BypassPartSet: %s",fPart_nob);	fflush(stdout);
					}
				}
			}
			else if (CSFoundFlag >0)
			{
				DummyFlg=0;
				CondiPassFlag=0;
				EXPANSSIONFlag=0;
				printf("\n POCHCK:A:Checking conditions...:[%s] \n",fPart_nob);fflush(stdout);

				if(tc_strcmp(fPart_typ,"D")==0 || tc_strcmp(fPart_typ,"DA")==0 ||tc_strcmp(fPart_typ,"DC")==0 || tc_strcmp(fPart_typ,"IFD")==0|| tc_strcmp(fPart_typ,"IM" )==0 ||(tc_strcmp(fPart_class,"t5SpKit")==0))
				{
					printf("\n POCHCK:A:.Obsolete/Dummy part skipped....:[%s]\n",fPart_nob);fflush(stdout);
					DummyFlg=1;				
				}
				else if((tc_strcmp(fPartProj,"1111")==0)||(tc_strcmp(fPart_class,"T5_EE_PartRevision")==0)||(tc_strcmp(fPart_typ,"G")==0)|| (tc_strcmp(fPart_class,"t5GrpID")==0)||((strlen(fPart_nob)!=11)&& ((tc_strcmp(fPartColInd,"Y")==0)||(tc_strcmp(fPartColInd,"N")==0)||(tc_strcmp(fPartColInd,"C")==0))&&((tc_strcmp(fPart_typ,"A")==0)||(tc_strcmp(fPart_typ,"C")==0)||(tc_strcmp(fPart_typ,"M")==0))))
				{
					printf("\n PO:Part eligible for PO check.:[%s]\n",fPart_nob);fflush(stdout);
					CondiPassFlag=1;
				}
				else
				{
					printf("\n PO:P41::NO expanssion for DR3 and below CS-F parts:[%s]\n",fPart_nob);fflush(stdout);
					EXPANSSIONFlag=1;
				}
				printf("\n PO:DummyFlg:%d CondiPassFlag:%d EXPANSSIONFlag:%d\n",DummyFlg,CondiPassFlag,EXPANSSIONFlag);fflush(stdout);
				if(DummyFlg >0)
				{
					EXPANSSIONFlag=0;
					if((SkippedPartSet==NULL)|| (tc_strcmp(SkippedPartSet,"")==0))
					{
						tc_strcpy (SkippedPartSet,"" );
						tc_strcat (SkippedPartSet,fPart_nob);
						printf("\n PO:Dmy:P1:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
					}
					else
					{
						if(strstr(SkippedPartSet,fPart_nob)==NULL)
						{
							tc_strcat (SkippedPartSet,"," );
							tc_strcat (SkippedPartSet,fPart_nob);
							printf("\n PO:Dmy:P2:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
						}
					}
				}
				else if (CondiPassFlag >0)
				{
					if ((tc_strcmp(fPart_typ,"G")==0)|| (tc_strcmp(fPart_class,"t5GrpID")==0))
					{
						EXPANSSIONFlag=0;
						GrpFunflag=0;
						GrpFunflag = tm_POCheckStopAtFGrp(AssyTg,sDml_TP,sEESWPrtType);
						printf("\n PO:GrpFunflag: %d",GrpFunflag);fflush(stdout);
						if(GrpFunflag >0)
						{
							printf("\n PO3:Grp: PO failed.: %s:%s",fPart_nob,fPrtRevv);	fflush(stdout);
							NwNOPOPrtFlg =0;
							NwNOPOPrtFlg = tm_CheckNewCretNOPO(fPart_nob,fPrtRevv);
							printf("\n PO:Grp4:NwNOPOPrtFlg.: %d",NwNOPOPrtFlg);fflush(stdout);
							if(NwNOPOPrtFlg >0)
							{
								if((NwNOPOPartsSet==NULL)|| (tc_strcmp(NwNOPOPartsSet,"")==0))
								{
									tc_strcpy (NwNOPOPartsSet,"" );
									tc_strcat (NwNOPOPartsSet,fPart_nob);
									printf("\n PO:P4:NwNOPOPartsSet. %s",fPart_nob);	fflush(stdout);	
								}
								else
								{
									if(strstr(NwNOPOPartsSet,fPart_nob)==NULL)
									{
										tc_strcat (NwNOPOPartsSet,"," );
										tc_strcat (NwNOPOPartsSet,fPart_nob);
										printf("\n PO:P4:NwNOPOPartsSet.. %s",fPart_nob);	fflush(stdout);
									}
								}
							}
							else
							{
								if((NOPOPartsSet==NULL)|| (tc_strcmp(NOPOPartsSet,"")==0))
								{
									tc_strcpy (NOPOPartsSet,"" );
									//tc_strcpy (NOPOPartsSet,"," );
									tc_strcat (NOPOPartsSet,fPart_nob);
									printf("\n PO:grp:P3:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
								}
								else
								{
									if(strstr(NOPOPartsSet,fPart_nob)==NULL)
									{
										tc_strcat (NOPOPartsSet,"," );
										tc_strcat (NOPOPartsSet,fPart_nob);
										printf("\n PO:grp:P4:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
									}
								}
							}
						}
						else
						{
							printf("\n PO:Grp:PO success.: %s",fPart_nob);	fflush(stdout);
							if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
							{
								tc_strcpy (POAvailPartSet,"" );
								tc_strcat (POAvailPartSet,fPart_nob);
								printf("\n PO:F:P7:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
							}
							else
							{
								if(strstr(POAvailPartSet,fPart_nob)==NULL)
								{
									tc_strcat (POAvailPartSet,"," );
									tc_strcat (POAvailPartSet,fPart_nob);
									printf("\n PO:F:P8:Part added to NOPOset: %s",fPart_nob); fflush(stdout);
								}
							}
						}
					}
					else
					{
						EEPartByPass=0;
						if((tc_strcmp(fPart_class,"T5_EE_PartRevision")==0)||(tc_strcmp(fPartObjTp,"T5_EE_PartRevision")==0)||(tc_strcmp(fPartObjTp,"EE Part Revision")==0))
						{
							//Query EE part and take Swft part type.
							//if(ITEM_find_item (fPart_nob, &EEitem_tag)!=ITK_ok)PrintErrorStack();
							EEitem_tag= t5GetItemRevison(fPart_nob);
							if(EEitem_tag==NULLTAG)
							{
								printf("\n P29:EEitem_tag is Null...\n");fflush(stdout);
							}
							else
							{
								printf("\n P29:EEitem_tag is found...\n");fflush(stdout);
								if(ITEM_ask_latest_rev (EEitem_tag, &EEpart_tag)!=ITK_ok)PrintErrorStack();
								if(EEpart_tag==NULLTAG)
								{
									printf("\n PO:EE:Object not found in Teamcenter...!!\n");fflush(stdout);
									return 0;
								}
								else
								{
									if(AOM_ask_value_string(EEpart_tag,"t5_SwPartType",&sEEswTyp)!=ITK_ok)PrintErrorStack();
									printf("\n PO:EE:Object found sEEswTyp:%s\n",sEEswTyp);fflush(stdout);
									if(strstr(sEESWPrtType,sEEswTyp)!=NULL)
									{
										printf("\n PO: Bypassing EE part for PO.\n");fflush(stdout);
										EEPartByPass=1;
										if((SkippedPartSet==NULL)|| (tc_strcmp(SkippedPartSet,"")==0))
										{
											tc_strcpy (SkippedPartSet,"" );
											tc_strcat (SkippedPartSet,fPart_nob);
											printf("\n PO:Dmy:P1:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
										}
										else
										{
											if(strstr(SkippedPartSet,fPart_nob)==NULL)
											{
												tc_strcat (SkippedPartSet,"," );
												tc_strcat (SkippedPartSet,fPart_nob);
												printf("\n PO:Dmy:P2:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
											}
										}
									}
									else
									{
										EEPartByPass=0;
									}
								}
							}
						}
						if (EEPartByPass ==0)
						{
							//bypass colour parts.
							if((strlen(fPart_nob)==14) && ((tc_strcmp(fPartColInd,"N")==0)|| (tc_strcmp(fPartColInd,"Y")==0)))
							{
								printf("\n PO: Bypass 14 digit parts for PO:[%s].\n",fPart_nob);fflush(stdout);
							}
							else
							{
								printf("\n PO:P4:Checkinging for Owners Manual:[%s : %s :%s].\n",fPart_nob,fpartDesgGrp,fPart_DR);fflush(stdout);
								All_item_tgs= t5GetItemRevison(fPart_nob);
								if(All_item_tgs==NULLTAG)
								{
									printf("\n P30:All_item_tgs is Null...\n");fflush(stdout);
								}
								else
								{
									printf("\n P30:All_item_tgs is found...\n");fflush(stdout);
									if(ITEM_list_all_revs (All_item_tgs, &rev_count, &revsn_list)!=ITK_ok)   PrintErrorStack();
									printf("\n PO:...Total rev found: %d.", rev_count);fflush(stdout);
									if(rev_count > 0)
									{
										POAvailFlag=0;
										for(ii=rev_count-1;ii>=0;ii--)
										{
											POAvailFlag=0;
											UomFlag=0;
											if(AOM_UIF_ask_value (revsn_list[ii], "item_revision_id", &Child_rev_idd)!=ITK_ok)PrintErrorStack();
											if(tc_strcmp(fPart_class,"T5_ClrPartRevision")!=0)
											{
												if(AOM_UIF_ask_value(revsn_list[ii],"t5_uom",&Chld_Prt_Uom)!=ITK_ok)   PrintErrorStack();
											}
											printf("\n PO:P7:Latest released rev is:%s Chld_Prt_Uom:%s", Child_rev_idd,Chld_Prt_Uom);fflush(stdout);

											if((tc_strcmp(Chld_Prt_Uom,"")==0)||(tc_strcmp(Chld_Prt_Uom,"each")==0)||(tc_strcmp(Chld_Prt_Uom,"4-Nos")==0))
											{
												if(WSOM_ask_release_status_list (revsn_list[ii], &Chld_Rel_Stus_Cunt, &Child_status_lst)!=ITK_ok)PrintErrorStack();
												//printf("\n PO:Chld_Rel_Stus_Cunt is:%d.", Chld_Rel_Stus_Cunt);fflush(stdout);
												if(Chld_Rel_Stus_Cunt > 0)
												{
													crr=0;
													for(crr=0;crr<Chld_Rel_Stus_Cunt;crr++)
													{
														POAvailFlag=0;
														//Checking PO availability on any revision.
														if(AOM_ask_value_string (Child_status_lst[crr], "object_name", &Chld_Rel_Stus)!=ITK_ok)PrintErrorStack();
														if((tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)||(tc_strstr(Chld_Rel_Stus,"ERC Released")!=NULL))
														{
															printf("\n PO:P4:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
															if(GRM_list_secondary_objects_only(revsn_list[ii], NULLTAG, &sec_count, &sec_obj)!=ITK_ok)   PrintErrorStack();
															printf("\n PO:.sec_count: %d",sec_count);	fflush(stdout);	
															for(jj=0; jj<sec_count; jj++)
															{
																POAvailFlag=0;
																if(PartObj) PartObj=NULLTAG;
																PartObj=sec_obj[jj];
																if(AOM_UIF_ask_value(PartObj, "object_type", &sec_obj_type)!=ITK_ok)   PrintErrorStack();
																//printf("\n PO:.sec_obj_type:%s", sec_obj_type);fflush(stdout);;;
																if((tc_strcmp(sec_obj_type, "Design Revision Master") == 0)||(tc_strcmp(sec_obj_type, "EE Part Revision Master") == 0) ||(tc_strcmp(sec_obj_type, "Clr Part Revision Master") == 0))
																{
																	printf("\n PO:.sec_obj_type:%s sDml_TP:%s", sec_obj_type,sDml_TP);fflush(stdout);
																	
																	//PO availability Fun call
																	printf("\n PO availability Fun call: %s",fPart_nob);fflush(stdout);
																	POAvailFlag=0;
																	POAvailFlag = tm_CheckPOAvalability(PartObj);
																	printf("\n P1:SIGNOFF:POAvailFlag.: %d",POAvailFlag);fflush(stdout);
																	if (POAvailFlag >0)
																	{
																		//PO Available so break.
																		if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
																		{
																			tc_strcpy (POAvailPartSet,"" );
																			tc_strcat (POAvailPartSet,fPart_nob);
																			printf("\n PO:P57:Part added to POAvailPartSet: %s",fPart_nob);	fflush(stdout);	
																		}
																		else
																		{
																			if(strstr(POAvailPartSet,fPart_nob)==NULL)
																			{
																			tc_strcat (POAvailPartSet,"," );
																			tc_strcat (POAvailPartSet,fPart_nob);
																			printf("\n PO:F:P24:Part added to POAvailPartSet: %s",fPart_nob);	fflush(stdout);
																			}
																		}
																		printf("\n PO:P41.:PO availale for part:%s ",fPart_nob);	fflush(stdout);
																	}
																	
																	break;
																}
															}
															if (POAvailFlag >0)
															{
																break;
															}
														}
													}
													if (POAvailFlag >0)
													{
														break;
													}
												}
											}
											else
											{
												POAvailFlag=0;
												UomFlag=1;
											}
										}
										if(UomFlag==0)
										{
											//IF PO available then Go for PO revision mismatch check, PO check for latest released revision.
											if (POAvailFlag >0)
											{
												PORevAvailFlag=0;
												for(iii=rev_count-1;iii>=0;iii--)
												{
													RelRevFlag=0;
													PORevAvailFlag=0;
													if(AOM_UIF_ask_value (revsn_list[iii], "item_revision_id", &Child_rev_idd)!=ITK_ok)PrintErrorStack();
													printf("\n PO:P5:Latest released rev is:%s.", Child_rev_idd);fflush(stdout);

													if(WSOM_ask_release_status_list (revsn_list[iii], &Chld_Rel_Stus_Cunt, &Child_status_lst)!=ITK_ok)PrintErrorStack();
													//printf("\n PO:Chld_Rel_Stus_Cunt is:%d.", Chld_Rel_Stus_Cunt);fflush(stdout);
													if(Chld_Rel_Stus_Cunt > 0)
													{
														crr=0;
														for(crr=0;crr<Chld_Rel_Stus_Cunt;crr++)
														{
															RelRevFlag=0;
															PORevAvailFlag=0;
															//Checking PO availability on any revision.
															if(AOM_ask_value_string (Child_status_lst[crr], "object_name", &Chld_Rel_Stus)!=ITK_ok)PrintErrorStack();
															//printf("\n PO:P6:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
															//if((tc_strstr(Chld_Rel_Stus,"Released")!=NULL) || (tc_strstr(Chld_Rel_Stus,"Rlzd")!=NULL)||(tc_strstr(Chld_Rel_Stus,"ERC Released")!=NULL) || (tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL))
															if(tc_strstr(Chld_Rel_Stus,"T5_LcsErcRlzd")!=NULL)
															{
																printf("\n PO:P6:Chld_Rel_Stus:%s", Chld_Rel_Stus);fflush(stdout);
																RelRevFlag=1;
																if(GRM_list_secondary_objects_only(revsn_list[iii], NULLTAG, &sec_count, &sec_obj)!=ITK_ok)   PrintErrorStack();
																printf("\n PO:sec_count: %d",sec_count);	fflush(stdout);	
																jj=0;
																for(jj=0; jj<sec_count; jj++)
																{
																	PORevAvailFlag=0;
																	if(PartObj) PartObj=NULLTAG;
																	PartObj=sec_obj[jj];
																	if(AOM_UIF_ask_value(PartObj, "object_type", &sec_obj_type)!=ITK_ok)   PrintErrorStack();
																	//printf("\n PO:..sec_obj_type:%s", sec_obj_type);fflush(stdout);
																	if((tc_strcmp(sec_obj_type, "Design Revision Master") == 0)||(tc_strcmp(sec_obj_type, "EE Part Revision Master") == 0) ||(tc_strcmp(sec_obj_type, "Clr Part Revision Master") == 0))
																	{
																		printf("\n PORev:..sec_obj_type:%s", sec_obj_type);fflush(stdout);
																		printf("\n PORev availability Fun call: %s",fPart_nob);fflush(stdout);
																		PORevAvailFlag=0;
																		PORevAvailFlag = tm_CheckPORevAvalability(PartObj);
																		printf("\n PO:PO availability for latset part rev:%s : PORevAvailFlag: %d",fPart_nob,PORevAvailFlag);	fflush(stdout);
																		if (PORevAvailFlag >0)
																		{
																			//ERROR: Add part number in PO Rev mismatch set.
																			printf("\n PO:Rev lvl PO Available for part:%s \n",fPart_nob);
																			if((PORevAvailPartSet==NULL)|| (tc_strcmp(PORevAvailPartSet,"")==0))
																			{
																				tc_strcpy (PORevAvailPartSet,"" );
																				tc_strcat (PORevAvailPartSet,fPart_nob);
																				printf("\n PO:F:P7:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
																			}
																			else
																			{
																				if(strstr(PORevAvailPartSet,fPart_nob)==NULL)
																				{
																					tc_strcat (PORevAvailPartSet,"," );
																					tc_strcat (PORevAvailPartSet,fPart_nob);
																					printf("\n PO:F:P8:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
																				}
																			}
																		}
																		else
																		{
																			flgPartAtachedToTsk=0;
																			printf("\n PO2:Rev lvl PO not avail for part:%s :%s \n",fPart_nob,Child_rev_idd);
																			if(strstr(TaskAtchedPartSet,fPart_nob)!=NULL)
																			{
																				flgPartAtachedToTsk=1;
																			}
																			NwNOPOPrtFlg =0;
																			NwNOPOPrtFlg = tm_CheckNewCretNOPO(fPart_nob,Child_rev_idd);
																			printf("\n PO:P12:NwNOPOPrtFlg...: %d",NwNOPOPrtFlg);fflush(stdout);
																			if((NwNOPOPrtFlg >0)&&(flgPartAtachedToTsk==1))
																			{
																				printf("\n PO:New Created PO Mis Part:%s \n",fPart_nob);
																				if((PSCPORevNOPOPartsSet==NULL)|| (tc_strcmp(PSCPORevNOPOPartsSet,"")==0))
																				{
																					tc_strcpy (PSCPORevNOPOPartsSet,"" );
																					tc_strcat (PSCPORevNOPOPartsSet,fPart_nob);
																					printf("\n PO:F:P6:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
																				}
																				else
																				{
																					if(strstr(PSCPORevNOPOPartsSet,fPart_nob)==NULL)
																					{
																					tc_strcat (PSCPORevNOPOPartsSet,"," );
																					tc_strcat (PSCPORevNOPOPartsSet,fPart_nob);
																					printf("\n PO:F:P5:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
																					}
																				}
																			}
																			else
																			{
																				printf("\n PO:Legacy Rev lvl PO mismtch part..:%s \n",fPart_nob);
																				if((PORevNOPOPartsSet==NULL)|| (tc_strcmp(PORevNOPOPartsSet,"")==0))
																				{
																					tc_strcpy (PORevNOPOPartsSet,"" );
																					tc_strcat (PORevNOPOPartsSet,fPart_nob);
																					printf("\n PO:F:P6:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
																				}
																				else
																				{
																					if(strstr(PORevNOPOPartsSet,fPart_nob)==NULL)
																					{
																					tc_strcat (PORevNOPOPartsSet,"," );
																					tc_strcat (PORevNOPOPartsSet,fPart_nob);
																					printf("\n PO:F:P5:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
																					}
																				}
																			}
																		}
																		//below break for Desg Master form.
																		break;
																	}
																}
																//below break to stop other released revision
																break;
															}
														}
														if (RelRevFlag >0)
														{
															//if got released revision.
															break;
														}
													}
												}													
											}
											else
											{
												printf("\n PO:PO not Available for part:%s fPartColInd:%s\n",fPart_nob,fPartColInd); fflush(stdout);
												POfoundFlg=0;
												if (tc_strcmp(fPartColInd,"Y")==0)
												{
													ColPrtFlag = 0;
													ColPrtFlag = tm_CheckColPrtPO(fPart_nob);
													printf("\n Col4:PO check for part:%s ColPrtFlag:%d\n",fPart_nob,ColPrtFlag);fflush(stdout);
													if(ColPrtFlag >0)
													{
														POfoundFlg=1;
													}
												}
												if(POfoundFlg ==0)
												{
													NwNOPOPrtFlg =0;
													printf("\n Col:PO3:fPart_nob:%s fPrtRevv:%s\n",fPart_nob,fPrtRevv);fflush(stdout);
													NwNOPOPrtFlg = tm_CheckNewCretNOPO(fPart_nob,fPrtRevv);
													printf("\n PO:P1:NwNOPOPrtFlg.: %d",NwNOPOPrtFlg);fflush(stdout);
													if(NwNOPOPrtFlg >0)
													{
														if((NwNOPOPartsSet==NULL)|| (tc_strcmp(NwNOPOPartsSet,"")==0))
														{
															tc_strcpy (NwNOPOPartsSet,"" );
															tc_strcat (NwNOPOPartsSet,fPart_nob);
															printf("\n PO:F:NwNOPOPartsSet:: %s",fPart_nob);	fflush(stdout);	
														}
														else
														{
															if(strstr(NwNOPOPartsSet,fPart_nob)==NULL)
															{
																tc_strcat (NwNOPOPartsSet,"," );
																tc_strcat (NwNOPOPartsSet,fPart_nob);
																printf("\n PO:F:NwNOPOPartsSet:. %s",fPart_nob);	fflush(stdout);
															}
														}
													}
													else
													{
														if((NOPOPartsSet==NULL)|| (tc_strcmp(NOPOPartsSet,"")==0))
														{
															tc_strcpy (NOPOPartsSet,"" );
															tc_strcat (NOPOPartsSet,fPart_nob);
															printf("\n PO:F:P3:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
														}
														else
														{
															if(strstr(NOPOPartsSet,fPart_nob)==NULL)
															{
															tc_strcat (NOPOPartsSet,"," );
															tc_strcat (NOPOPartsSet,fPart_nob);
															printf("\n PO:F:P4:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
						else
						{
							printf("\n PO: Bypassing EE part NO expanssion.\n");fflush(stdout);
							EXPANSSIONFlag=0;
						}
					}
				}
			}
			else
			{
				//only expanssion.
				if((tc_strcmp(fPart_typ,"G")==0)|| (tc_strcmp(fPart_class,"t5GrpID")==0))
				{
					EXPANSSIONFlag=0;
					GrpFunflag=0;
					GrpFunflag = tm_POCheckStopAtFGrp(AssyTg,sDml_TP,sEESWPrtType);
					printf("\n PO:GrpFunflag: %d",GrpFunflag);fflush(stdout);
					if(GrpFunflag >0)
					{
						printf("\n PO4:Grp:PO failed-: %s :fPrtRevv:%s",fPart_nob,fPrtRevv);	fflush(stdout);
						NwNOPOPrtFlg =0;
						NwNOPOPrtFlg = tm_CheckNewCretNOPO(fPart_nob,fPrtRevv);
						printf("\n PO:Grp1:NwNOPOPrtFlg.: %d",NwNOPOPrtFlg);fflush(stdout);
						if(NwNOPOPrtFlg >0)
						{
							if((NwNOPOPartsSet==NULL)|| (tc_strcmp(NwNOPOPartsSet,"")==0))
							{
								tc_strcpy (NwNOPOPartsSet,"" );
								tc_strcat (NwNOPOPartsSet,fPart_nob);
								printf("\n PO:NwNOPOPartsSet. %s",fPart_nob);	fflush(stdout);	
							}
							else
							{
								if(strstr(NwNOPOPartsSet,fPart_nob)==NULL)
								{
									tc_strcat (NwNOPOPartsSet,"," );
									tc_strcat (NwNOPOPartsSet,fPart_nob);
									printf("\n PO:NwNOPOPartsSet.. %s",fPart_nob);	fflush(stdout);
								}
							}
						}
						else
						{
							if((NOPOPartsSet==NULL)|| (tc_strcmp(NOPOPartsSet,"")==0))
							{
								tc_strcpy (NOPOPartsSet,"" );
								//tc_strcpy (NOPOPartsSet,"," );
								tc_strcat (NOPOPartsSet,fPart_nob);
								printf("\n PO:grp:P3:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
							}
							else
							{
								if(strstr(NOPOPartsSet,fPart_nob)==NULL)
								{
									tc_strcat (NOPOPartsSet,"," );
									tc_strcat (NOPOPartsSet,fPart_nob);
									printf("\n PO:grp:P4:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
								}
							}
						}
					}
					else
					{
						printf("\n PO:Grp:PO success.: %s",fPart_nob);	fflush(stdout);
						if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
						{
							tc_strcpy (POAvailPartSet,"" );
							//tc_strcpy (POAvailPartSet,"," );
							tc_strcat (POAvailPartSet,fPart_nob);
							printf("\n PO:F:P7:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);	
						}
						else
						{
							if(strstr(POAvailPartSet,fPart_nob)==NULL)
							{
							tc_strcat (POAvailPartSet,"," );
							tc_strcat (POAvailPartSet,fPart_nob);
							printf("\n PO:F:P8:Part added to NOPOset: %s",fPart_nob);	fflush(stdout);
							}
						}
					}
				}
				else
				{
					printf("\n PO:CS other than F so checking EXPANSSION:[%s]  \n",fPart_nob);fflush(stdout);
					EXPANSSIONFlag=1;
					printf("\n PO:.EXPANSSIONFlag:[%d]\n",EXPANSSIONFlag);fflush(stdout);
				}
			}
		}
	}
	if (EXPANSSIONFlag >0)
	{
		return 1;
	}
	
	return 0;
	//return ITK_ok;
}

int tm_POCheckStopAtFun(char* item_id,char* tplant)
{
	FILE	*fd;
	int		ifail 		= ITK_ok;
	int		k	=  0;
	int		j	=  0;
	int		inopo	=  0;
	int		iCount	= 0;
	int		GMDMLFun = 0;
	int		MRDMLFun = 0;
	int		count = 0;
	int		FunOutflag	= 0;
	int		Tskcnt	= 0;
	int		childCountTopBomLine	= 0;
	int		FirstCheckOK	= 0;
	int		partcnt	= 0;
	int		tsk	= 0;
	int     DR_n_entry    = 1;
	int     DR_rsltCount      =  0;
	char	*Tsk_object_type = NULL;
	char	*usernam		= NULL;
	char	*sNOPO		= NULL;
	char	*PlntToPlnt		= NULL;
	char	*class_name1	= NULL;
	char	*tsknm		= NULL;
	char	*DMLProj		= NULL;
	char	*sDml_DR		= NULL;
	char	*sDml_TP		= NULL;
	char	*Part_nobr		= NULL;
	char	*sPartCS		= NULL;
	char	*POinfo1		= NULL;
	char	*NOPOlist		= NULL;
	char	*Part_no		= NULL;
	char	*sDMLno			= NULL;
	char	*sEESWPrtType	= NULL;
	char	*DMLtype		= NULL;
	char    *DR_qry_entry[1]  = {"SYSCD"};
	char    **DR_qry_values2     =  (char **) MEM_alloc(10 * sizeof(char *));
	char   ObjTyp[TCTYPE_name_size_c+1];
	char **	AStringList =NULL;
	char **	BStringList =NULL;
	char **	CStringList =NULL;
	char **	DStringList =NULL;
	char **	EStringList =NULL;
	char **	FStringList =NULL;
	char **	GStringList =NULL;
	tag_t	objTypTag		= NULLTAG;
	tag_t	Lat_Item_Tag 	= NULLTAG;
	tag_t	AssyTg 	= NULLTAG;
	tag_t	item_tag 	= NULLTAG;
	tag_t	tagRule 	= NULLTAG;
	tag_t	tagBOMwindow 	= NULLTAG;
	tag_t	tagBOMline 	= NULLTAG;
	tag_t	*PartsTg	= NULLTAG;
	tag_t	*Dml_tasks	= NULLTAG;
	tag_t   *DRCntrlObjectTag	= NULLTAG;
	tag_t   DRqryTag     = NULLTAG;
	tag_t	class_id1	=NULLTAG;
	tag_t	*tagTopBLchildLine = NULLTAG;
	tag_t	*DMLRevision = NULLTAG;
	tag_t	Prt_tag	= NULLTAG;
	tag_t	PrtRev_tg	= NULLTAG;
	tag_t	Pitem_tag	= NULLTAG;
	tag_t	DMLRevTg	= NULLTAG;
	tag_t	NOPOpart_tg	= NULLTAG;
	tag_t	NOPOprt_tg	= NULLTAG;
	tag_t	tsk_dml_rel_type	= NULLTAG;
	int jr=0;
	int jrr=0;
	int nopo=0;
	int Errorflag=0;
	int Refcount=0;
	int piStringCount=0;
	int NOPORevFldFlag=0;
	int FldObjCount=0;
	int ReqFdrSize=0;
	int NOPOFldFlag=0;
	int PrtRev_count=0;
	int pii=0;
	int n=0;
	int PrtRelStus_cnt=0;
	int mbFlag = 0;
	logical is_latest;
	char*	sPrtName	=NULL; 
	char*	FileDown	=NULL; 
	char*	 nPrtNO = NULL;
	char*	 nPrtRev = NULL;
	char*	 nPart_DR = NULL;
	char*	 PrtRelStus = NULL;
	char*	 nPartProj = NULL;
	char*	 nPart_typ = NULL;
	char*	 nPartColInd = NULL;
	char*	 npartDrwind = NULL;
	char*	 npartDesgGrp = NULL;
	char*	 NOPOFolder = NULL;
	//char*	 Reftype_name = NULL;
	char   Reftype_name[TCTYPE_name_size_c+1];
	char*	 sNOPOFldr = NULL;
	char*	 nPartObjTp = NULL;
	char*	 sPartNumber = NULL;
	char*	 cRevSeq = NULL;
	char*	 newPatRev = NULL;
	char*	 newPatSeq = NULL;
	tag_t msWordType=NULLTAG;
	tag_t wordDataset=NULLTAG;
	tag_t wordLatestDat_t=NULLTAG;
	tag_t Iman_Ref_Rel_Tg	= NULLTAG;
	tag_t dml_Ref_Rel_t	= NULLTAG;
	tag_t Part_Ref_Rel_t = NULLTAG;
	tag_t item_create_input_tag	= NULLTAG;
	tag_t item_type_tag	= NULLTAG;
	tag_t Fold_Cont_Rel_t	= NULLTAG;
	tag_t Fold_Cont_Rel	= NULLTAG;
	tag_t NOPO_dml_Ref_Rel	= NULLTAG;
	tag_t NOPOdml_Ref_Rel_t	= NULLTAG;
	tag_t NOPOobject	= NULLTAG;
	tag_t object	= NULLTAG;
	tag_t RefTypeTag	= NULLTAG;
	tag_t *PrtRev_list=NULLTAG;
	tag_t *PrtRelStusTg=NULLTAG;
	tag_t *FoldObj_tag=NULLTAG;
	tag_t *RefDoc_tag=NULLTAG;
	tag_t RefDoc_t=NULLTAG;
	char	*Toplnt		=  NULL;
	char	*FromPlnt		=  NULL;
	int rulefounddd=0;
	int rulefoundXO=0;
	char **POrulenameee = NULL;
	char **POrulevalueee = NULL;
	tag_t POclose_taggg = NULLTAG;
	tag_t *POclosureruleee = NULLTAG;
	char **POrulenameXO = NULL;
	char **POrulevalueXO = NULL;
	tag_t POclose_tagXO = NULLTAG;
	tag_t *POclosureruleXO = NULLTAG;
	tag_t	TaskTag 		= NULLTAG;
	char*	 Prtobj_type = NULL;
	char*	 FldrName = NULL;
	int FlderSize=0;
	int FlderObjCount=0;
	tag_t *FolderObj_tag=NULLTAG;
	int fld=0;
	char*	 P_Rev = NULL;
	char*	 Part_nobrrr = NULL;
	char*	 P_Typee = NULL;
	char*	 P_Typeer = NULL;
	char*	 rev_idddr = NULL;
	char*	 PrtDRstt = NULL;
	char*	 PrtDRsttr = NULL;
	char*	 rev_iddd = NULL;
	char*	 MoldChild = NULL;
	char*	 nPrtnoo = NULL;
	char*	 nPrtRevv = NULL;
	char*	 sSystPrtName = NULL;
	char*	 Partobj_type = NULL;
	char*	 Part_nobrr = NULL;
	char*	 P_Type = NULL;
	tag_t	CCVtoSnapShot_Rel_Type	= NULLTAG;
	int     RelRevFlag      =  0;
	int     mdl      =  0;
	int     SnpShtcount      =  0;
	tag_t	*SnpShtTags 	= NULLTAG;
	tag_t	SnpShtTag 	= NULLTAG;
	tag_t	SnpObjTypeTag 	= NULLTAG;
	char   SnpObjTypeNm[TCTYPE_name_size_c+1];
	char   *SnpShtNo =NULL;
	tag_t	SnpBOMline 	= NULLTAG;
	tag_t Modl_rev_tags = NULLTAG;
	tag_t	Snpwindow 	= NULLTAG;
	int     childCountSnpBOMline      =  0;
	tag_t	*SnpBOMlineTags 	= NULLTAG;
	tag_t	SnpBOMlineTag 	= NULLTAG;
	char   *ArcMdlNo =NULL;
	tag_t	ArcMdlNo_tg 	= NULLTAG;
	tag_t	ArcMdlNo_tag 	= NULLTAG;
	tag_t	MdlBOMwindow 	= NULLTAG;
	tag_t	MdltagRule 	= NULLTAG;
	tag_t	*MdlTopBLchildLine 	= NULLTAG;
	int     MdlCount      =  0;
	tag_t	*POclosRul 	= NULLTAG;
	tag_t	POcloseRul_tag 	= NULLTAG;
	char **POrulNm = NULL;
	char **Mdlrulevl = NULL;
	tag_t	MdlBOMline 	= NULLTAG;
	int     mdll      =  0;
	int     mdladd      =  0;
	int     MdlchildCount      =  0;
	char   *MdlNo =NULL;
	char   *Mdl_DR =NULL;
	tag_t	MdlLstRev_tag	= NULLTAG;
	tag_t	Mdl_tag	= NULLTAG;
	char   *ERCMdlRelErrStrng =NULL;
	tag_t dml_Ref_Rel	= NULLTAG;
	FileDown = (char *) MEM_alloc (sizeof (char) * 10000);

	NOPOPartsSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	TaskAtchedPartSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	UnMatPartSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	NwNOPOPartsSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	PORevNOPOPartsSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	PSCPORevNOPOPartsSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	BypassPartSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	POAvailPartSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	PORevAvailPartSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	SkippedPartSet     =  (char *) MEM_alloc(100000 * sizeof(char));
	RelDRStatus     =  (char *) MEM_alloc(500 * sizeof(char));
	FromTOStus     =  (char *) MEM_alloc(500 * sizeof(char));
	PARTTYPEs     =  (char *) MEM_alloc(500 * sizeof(char));
	printf("\nPO:....Inside PO check...\n");fflush(stdout);
	tc_strcpy (NOPOPartsSet,"" );
	tc_strcpy (TaskAtchedPartSet,"" );
	tc_strcpy (UnMatPartSet,"" );
	tc_strcpy (NwNOPOPartsSet,"" );
	tc_strcpy (PORevNOPOPartsSet,"" );
	tc_strcpy (PSCPORevNOPOPartsSet,"" );
	tc_strcpy (BypassPartSet,"" );
	tc_strcpy (POAvailPartSet,"" );
	tc_strcpy (PORevAvailPartSet,"" );
	tc_strcpy (SkippedPartSet,"" );
	tc_strcpy (RelDRStatus,"" );
	tc_strcpy (FromTOStus,"" );

	printf("\n PO:PO check started....%s:%s \n",item_id,tplant);fflush(stdout);
	//PO check bypass logic:
	if(QRY_find("ControlObjects", &DRqryTag));
	if (DRqryTag)
	{
		printf("\n PO:Found Query ControlObjects \n");fflush(stdout);
	}
	else
	{
		printf("\n PO:Not Found Query ControlObjects \n");fflush(stdout);
	}

	DR_qry_values2[0] = "POCHCK";
	//DR_qry_values2[0] = "CREVali";
	if(QRY_execute(DRqryTag, DR_n_entry, DR_qry_entry, DR_qry_values2, &DR_rsltCount, &DRCntrlObjectTag));
	printf(" \n PO:DR_rsltCount :%d:", DR_rsltCount); fflush(stdout);
	if(DR_rsltCount > 0)
	{
		//PO bypass
		if (AOM_ask_value_string(DRCntrlObjectTag[0],"t5_Userinfo1",&POinfo1)!=ITK_ok)   PrintErrorStack();
		if (AOM_ask_value_string(DRCntrlObjectTag[0],"t5_Userinfo2",&sEESWPrtType)!=ITK_ok)   PrintErrorStack();
		if (AOM_ask_value_string(DRCntrlObjectTag[0],"t5_Userinfo3",&sPOCHKInfo3)!=ITK_ok)   PrintErrorStack();
		printf("\n PO: POinfo1:%s\n sEESWPrtType:%s sPOCHKInfo3:%s\n", POinfo1,sEESWPrtType,sPOCHKInfo3);fflush(stdout);
	
		if(strstr(sPOCHKInfo3,"POSTART")==NULL)
		{
			if(ITEM_find_item (item_id, &item_tag)!=ITK_ok)PrintErrorStack();
			//item_tag= t5GetItemRevison(item_id);
			if(item_tag==NULLTAG)
			{
				printf("\n P30:item_tag is Null...\n");fflush(stdout);
			}
			else
			{
				printf("\n P30:item_tag is found...\n");fflush(stdout);
				if(ITEM_ask_latest_rev (item_tag, &Lat_Item_Tag)!=ITK_ok)PrintErrorStack();
				if(Lat_Item_Tag==NULLTAG)
				{
					printf("\n PO:Object not found in Teamcenter...!!\n");fflush(stdout);
					return 0;
				}
				else
				{
					printf("\n PO:Object Found...!!");fflush(stdout);
					if(TCTYPE_ask_object_type(Lat_Item_Tag,&objTypTag)!=ITK_ok)PrintErrorStack();
					if(TCTYPE_ask_name(objTypTag,ObjTyp)!=ITK_ok)PrintErrorStack();
					printf("\n PO: Workfow obj type: [%s]", ObjTyp);fflush(stdout);
					if(tc_strcmp(ObjTyp,"T5_ChangeTaskRevision")==0)
					{
						if(POM_get_user_id (&usernam)!=ITK_ok)PrintErrorStack();
						printf("\n PO:Session login User1 for MR : %s\n",usernam); fflush(stdout);

						//if(tc_strcmp(usernam,"infodba") !=0 && tc_strcmp(usernam,"loader") !=0)
						//{
							if(GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type)!=ITK_ok)PrintErrorStack();
							if (tsk_dml_rel_type!=NULLTAG)
							{
								if(GRM_list_primary_objects_only(Lat_Item_Tag,tsk_dml_rel_type,&count,&DMLRevision)!=ITK_ok)PrintErrorStack();
								printf("\n PO:DML Revision from Task for MR : %d",count);fflush(stdout);
								for (j=0;j<count ;j++ )
								{
									if(ITEM_rev_sequence_is_latest(DMLRevision[j],&is_latest)!=ITK_ok)PrintErrorStack();
									printf("\n PO: is_latest MR is : %d\n",is_latest);fflush(stdout);

									if(is_latest != true)
									{
										printf("\n PO:is_latest is not true .....\n");fflush(stdout);
									}
									else
									{
										DMLRevTg = DMLRevision[j];
										if(POM_class_of_instance(DMLRevTg,&class_id1)!=ITK_ok)PrintErrorStack();
										if(POM_name_of_class(class_id1,&class_name1)!=ITK_ok)PrintErrorStack();
										printf("\n PO: class_name is:%s",class_name1);
										if(AOM_ask_value_string(DMLRevTg,"item_id",&sDMLno)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(DMLRevTg,"t5_rlstype",&DMLtype)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(DMLRevTg,"t5_cprojectcode",&DMLProj)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(DMLRevTg,"t5_PlntToPlnt",&PlntToPlnt)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(DMLRevTg,"t5_cDRstatus",&sDml_DR)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(DMLRevTg,"t5_TargetPlant",&sDml_TP)!=ITK_ok)PrintErrorStack();
										printf("\n PO: sDMLno:%s DMLtype:%s DMLProj:%s PlntToPlnt:%s sDml_TP:%s sDml_DR:%s",sDMLno,DMLtype,DMLProj,PlntToPlnt,sDml_TP,sDml_DR);fflush(stdout);

										//DML DR-status should not be NULL.
										if((sDml_DR==NULL)|| (tc_strcmp(sDml_DR,"")==0))
										{
											printf("\n PO:ERROR: DR-status should not be NULL for DML.\n");fflush(stdout);
											//EMH_store_error_s2(EMH_severity_error,ITK_err,"DML not having valid DR-status, please updated it and then sign off task.\n",sDMLno);	
											//return 1;
										}
										else
										{
											if((tc_strcmp(DMLtype,"MR")==0)||(tc_strcmp(DMLtype,"DFMR")==0)||(tc_strcmp(DMLtype,"TPL")==0)||(tc_strcmp(DMLtype,"Veh")==0))
											{
												if((strstr(sDml_DR,"AR")!=NULL)||(strstr(sDml_DR,"DR")!=NULL))
												{
													printf("\n PO:MR:valid DR-status available.\n");fflush(stdout);
													tc_strcpy(RelDRStatus,"");
													tc_strcpy(RelDRStatus,sDml_DR);	
													tc_strcpy(FromTOStus,"");
													tc_strcpy(FromTOStus,sDml_DR);	
													printf("\n PO:MR:valid DR-status RelDRStatus:%s FromTOStus:%s\n",RelDRStatus,FromTOStus);fflush(stdout);
												}
												else
												{
													printf("\n PO:ERROR:MR: Valid DR-status should be for DML.\n");fflush(stdout);
													//EMH_store_error_s2(EMH_severity_error,ITK_err,"DML not having valid DR-status, please updated it and then sign off task.\n",sDMLno);	
													//return 1;
												}
											}
											else if(tc_strcmp(DMLtype,"TODR")==0)
											{
												//DML DR-status
												if(tc_strcmp(sDml_DR,"NA")==0)
												{
													printf("\n PO:GM:NA-status available.\n");fflush(stdout);
												}
												else
												{
													printf("\n PO:ERROR:Gate Maturation DML should have NA as DR-status.\n");fflush(stdout);
													//EMH_store_error_s2(EMH_severity_error,ITK_err,"Gate Maturation DML should have NA as DR-status.\n",sDMLno);	
													//return 1;
												}
												//FROMTO Gate change is mandatroy.
												if((PlntToPlnt==NULL)|| (tc_strcmp(PlntToPlnt,"")==0))
												{
													printf("\n PO:ERROR: Target plant should not be NULL for DML.\n");fflush(stdout);
													//EMH_store_error_s2(EMH_severity_error,ITK_err,"DML not having valid FROM-TO DR-Gate value updated. Please update valid value and then sign off DML.\n",sDMLno);	
													//return 1;
												}
												else
												{
													if ((tc_strcmp(PlntToPlnt,"AR3P to AR4")==0)|| (tc_strcmp(PlntToPlnt,"DR3P to DR4")==0))
													{
														FromPlnt=subString(PlntToPlnt, 0, 4);
														Toplnt=subString(PlntToPlnt, 8, 3);
													}
													else if ((tc_strcmp(PlntToPlnt,"AR3 to AR3P")==0)|| (tc_strcmp(PlntToPlnt,"DR3 to DR3P")==0))
													{
														FromPlnt=subString(PlntToPlnt, 0, 3);
														Toplnt=subString(PlntToPlnt, 7, 4);
													}
													else
													{
														FromPlnt=subString(PlntToPlnt, 0, 3);
														Toplnt=subString(PlntToPlnt, 7, 3);
													}
													printf("\n GMD: FromPlnt:[%s] RelDRStatus:[%s]\n", FromPlnt,Toplnt);fflush(stdout);
													tc_strcpy(RelDRStatus,"");
													tc_strcpy(RelDRStatus,Toplnt);
													tc_strcpy(FromTOStus,"");
													tc_strcpy(FromTOStus,PlntToPlnt);	
													printf("\n GMD: RelDRStatus:[%s] FromTOStus:[%s]\n", RelDRStatus,FromTOStus);fflush(stdout);
												}
											}
										}
										//Checking PO check eligibility DML
										FirstCheckOK=0;
										if(((tc_strcmp(DMLtype,"MR")==0)||(tc_strcmp(DMLtype,"DFMR")==0)||(tc_strcmp(DMLtype,"TPL")==0)||(tc_strcmp(DMLtype,"Veh")==0))&& ((tc_strcmp(sDml_DR,"AR3P")==0)||(tc_strcmp(sDml_DR,"DR3P")==0)||(tc_strcmp(sDml_DR,"DR4")==0)|| (tc_strcmp(sDml_DR,"DR5")==0)||(tc_strcmp(sDml_DR,"AR4")==0)|| (tc_strcmp(sDml_DR,"AR5")==0)))
										{
											printf("\n PO:MR: PO FirstCheckOK for DML:%s \n",sDMLno);fflush(stdout);
											FirstCheckOK=1;
										}
										else if((tc_strcmp(DMLtype,"TODR")==0) && ((tc_strcmp(PlntToPlnt,"AR3P to AR4")==0)|| (tc_strcmp(PlntToPlnt,"DR3P to DR4")==0)||(tc_strcmp(PlntToPlnt,"AR3 to AR3P")==0)|| (tc_strcmp(PlntToPlnt,"DR3 to DR3P")==0)||(tc_strcmp(PlntToPlnt,"AR3 to AR4")==0)||(tc_strcmp(PlntToPlnt,"AR4 to AR5")==0)||(tc_strcmp(PlntToPlnt,"DR3 to DR4")==0)||(tc_strcmp(PlntToPlnt,"DR4 to DR5")==0)))
										{
											printf("\n PO:GM: PO FirstCheckOK for DML:%s \n",sDMLno);fflush(stdout);
											FirstCheckOK=1;
										}
										else
										{
											printf("\n PO:GM: Not eligible DML:%s \n",sDMLno);fflush(stdout);
											FirstCheckOK=0;
										}
										if(sDMLno!=NULL)
										{
											if(strstr(POinfo1,sDMLno)!=NULL)
											{
												FirstCheckOK=0;
											}
										}
										
										printf("\n PO:FirstCheckOK:%d  \n",FirstCheckOK);fflush(stdout);
										if (FirstCheckOK >0)
										{
											tc_strcpy(FileDown,"/tmp/");
											tc_strcat(FileDown,sDMLno);
											tc_strcat(FileDown,"_DML_NOPO_Report.xls");
											TC_write_syslog("Main File = %s\n",FileDown);
											
											fd=fopen(FileDown,"w");
											if(fd==NULL)
											{
												printf("\nPO:Error in opening the file \n");fflush(stdout);
											}
											fprintf(fd,"\n\t -- DML NOPO Report --\n\n");
											fprintf(fd,"\nDML DETAILS:");
											//fprintf(fd,"\nATTRIBUTES\tVALUES");
											fprintf(fd,"\nDML Number\t%s",sDMLno);
											fprintf(fd,"\nTarget Plant\t%s",sDml_TP);
											fprintf(fd,"\nDML DR-status\t%s",sDml_DR);
											if(strcmp(DMLtype,"MR")==0)
											{
												fprintf(fd,"\nDML Releasetype\t Module Release");
											}
											else if(strcmp(DMLtype,"Veh")==0)
											{
												fprintf(fd,"\nDML Releasetype\t Vehicle Release");
											}
											else if(strcmp(DMLtype,"TODR")==0)
											{
												fprintf(fd,"\nDML Releasetype\t Gate Maturation");
											}
											else
											{
												fprintf(fd,"\nDML Releasetype\t%s",DMLtype);
											}
											fprintf(fd,"\nFROM-TO Gate Change\t%s",PlntToPlnt);
											fprintf(fd,"\n TC PART NO.\t REVISION \t PART CS \t PART TYPE \t PART DR-STATUS \t PROJECT \t DESIGN GRP \t DRW_INDICATOR  \t COLOUR INDICATOR \t PART DESCRIPTION \t \n");


											//Target plant should not be NULL.
											if((sDml_TP==NULL)|| (tc_strcmp(sDml_TP,"")==0))
											{
												printf("\n PO:ERROR: Target plant should not be NULL for DML.\n");fflush(stdout);
												//EMH_store_error_s2(EMH_severity_error,ITK_err,"DML not having valid target plant. Please update valid target plant and then sign off DML.\n",sDMLno);	
												//return 1;
											}

											Tskcnt=0;
											printf("\n PO:Target Plant:%s  \n",sDml_TP);fflush(stdout);
											if(AOM_ask_value_tags(DMLRevTg,"T5_DMLTaskRelation",&Tskcnt,&Dml_tasks)!=ITK_ok)PrintErrorStack();
											printf("\n Now Tskcnt:%d\n",Tskcnt);fflush(stdout);
											if (Tskcnt>0)
											{
												Errorflag=0;
												//creation set of parts atached to task for updation.
												for (tsk=0;tsk<Tskcnt ;tsk++ )
												{
													if(AOM_ask_value_string(Dml_tasks[tsk],"item_id",&tsknm)!=ITK_ok)PrintErrorStack();
													if(AOM_ask_value_string(Dml_tasks[tsk],"object_type",&Tsk_object_type)!=ITK_ok)PrintErrorStack();
													printf("\n Task:%s ObjType :%s\n",tsknm,Tsk_object_type);fflush(stdout);

													if(strcmp(Tsk_object_type,"T5_ChangeTaskRevision")==0)
													{
														//Task expanssion till parts.
														if((tc_strcmp(DMLtype,"MR")==0)||(tc_strcmp(DMLtype,"DFMR")==0)||(tc_strcmp(DMLtype,"TPL")==0)||(tc_strcmp(DMLtype,"Veh")==0))
														{
															if(AOM_ask_value_tags(Dml_tasks[tsk],"CMHasSolutionItem",&partcnt,&PartsTg)!=ITK_ok)   PrintErrorStack();
															printf("\n PO: Now partcnt:%d",partcnt);fflush(stdout);
															if (partcnt>0)
															{
																for (k=0;k<partcnt ;k++ )
																{
																	AssyTg=PartsTg[k];
																	if(AOM_ask_value_string(AssyTg,"item_id",&Part_no)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:task attached Part_no: %s",Part_no);	fflush(stdout);	
																	if((TaskAtchedPartSet==NULL)|| (tc_strcmp(TaskAtchedPartSet,"")==0))
																	{
																		tc_strcpy (TaskAtchedPartSet,"" );
																		tc_strcat (TaskAtchedPartSet,Part_no);
																		printf("\n P:Part added to TaskAtchedPartSet: %s",Part_no);	fflush(stdout);	
																	}
																	else
																	{
																		if(strstr(TaskAtchedPartSet,Part_no)==NULL)
																		{
																			tc_strcat (TaskAtchedPartSet,"," );
																			tc_strcat (TaskAtchedPartSet,Part_no);
																			printf("\n P:Part added to TaskAtchedPartSet: %s",Part_no);	fflush(stdout);
																		}
																	}
																}
															}
														}
														else if(tc_strcmp(DMLtype,"TODR")==0)
														{
															if(AOM_ask_value_tags(Dml_tasks[tsk],"CMReferences",&partcnt,&PartsTg)!=ITK_ok)   PrintErrorStack();
															printf("\n PO: Now partcnt:%d",partcnt);fflush(stdout);
															if (partcnt>0)
															{
																for (k=0;k<partcnt ;k++ )
																{
																	AssyTg=PartsTg[k];
																	if(AOM_UIF_ask_value(AssyTg, "object_type", &Prtobj_type)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO: Object Type is : %s",Prtobj_type);	fflush(stdout);	
																	if(tc_strcmp(Prtobj_type,"Folder")!=0)
																	{
																		if(AOM_ask_value_string(AssyTg,"item_id",&Part_nobr)!=ITK_ok)   PrintErrorStack();
																		printf("\n PO: Part_nobr is : %s",Part_nobr);	fflush(stdout);	
																		if((TaskAtchedPartSet==NULL)|| (tc_strcmp(TaskAtchedPartSet,"")==0))
																		{
																			tc_strcpy (TaskAtchedPartSet,"" );
																			tc_strcat (TaskAtchedPartSet,Part_nobr);
																			printf("\n P4:Part added to TaskAtchedPartSet: %s",Part_nobr);	fflush(stdout);	
																		}
																		else
																		{
																			if(strstr(TaskAtchedPartSet,Part_nobr)==NULL)
																			{
																				tc_strcat (TaskAtchedPartSet,"," );
																				tc_strcat (TaskAtchedPartSet,Part_nobr);
																				printf("\n P3:Part added to TaskAtchedPartSet: %s",Part_nobr);	fflush(stdout);
																			}
																		}
																	}
																	else
																	{
																		//If folder then copy all parts to TaskAtchedPartSet.
																		if(AOM_ask_value_string(AssyTg,"object_name",&FldrName)!=ITK_ok)PrintErrorStack();
																		printf("\n GMD:FldrName:: %s\n", FldrName);fflush(stdout);
																		if(tc_strcmp(FldrName,"Parts For Gate Maturation")==0)
																		{
																			printf("\n GMD: getting parts.\n");fflush(stdout);
																			if(FL_ask_size(AssyTg,&FlderSize));
																			printf("\n GMD:FlderSize is:   %d\n",FlderSize);fflush(stdout);
																			if(FL_ask_references(AssyTg,FL_fsc_by_date_modified ,&FlderObjCount,&FolderObj_tag));
																			printf("\n GMD:FlderObjCount is..:   %d\n",FlderObjCount);fflush(stdout);
																			if(FlderObjCount>0)
																			{
																				fld=0;
																				for (fld=0;fld<FlderObjCount;fld++ )
																				{
																					if(AOM_ask_value_string(FolderObj_tag[fld],"item_id",&sSystPrtName)!=ITK_ok)PrintErrorStack();
																					printf("\n GMD: part from folder: %s.", sSystPrtName);fflush(stdout);
																					if((TaskAtchedPartSet==NULL)|| (tc_strcmp(TaskAtchedPartSet,"")==0))
																					{
																						tc_strcpy (TaskAtchedPartSet,"" );
																						tc_strcat (TaskAtchedPartSet,sSystPrtName);
																						printf("\n P1:Part added to TaskAtchedPartSet: %s",sSystPrtName);	fflush(stdout);	
																					}
																					else
																					{
																						if(strstr(TaskAtchedPartSet,sSystPrtName)==NULL)
																						{
																							tc_strcat (TaskAtchedPartSet,"," );
																							tc_strcat (TaskAtchedPartSet,sSystPrtName);
																							printf("\n P2:Part added to TaskAtchedPartSet: %s",sSystPrtName);	fflush(stdout);
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
												//creation set End.
												tsk=0;
												for (tsk=0;tsk<Tskcnt ;tsk++ )
												{
													if(AOM_ask_value_string(Dml_tasks[tsk],"item_id",&tsknm)!=ITK_ok)PrintErrorStack();
													if(AOM_ask_value_string(Dml_tasks[tsk],"object_type",&Tsk_object_type)!=ITK_ok)PrintErrorStack();
													printf("\n %d:Task:%s ObjType :%s\n",tsk,tsknm,Tsk_object_type);fflush(stdout);

													if(strcmp(Tsk_object_type,"T5_ChangeTaskRevision")==0)
													{
														//Task expanssion till parts.
														if((tc_strcmp(DMLtype,"MR")==0)||(tc_strcmp(DMLtype,"DFMR")==0)||(tc_strcmp(DMLtype,"TPL")==0)||(tc_strcmp(DMLtype,"Veh")==0))
														{
															if(AOM_ask_value_tags(Dml_tasks[tsk],"CMHasSolutionItem",&partcnt,&PartsTg)!=ITK_ok)   PrintErrorStack();
															printf("\n PO: Now partcnt:%d",partcnt);fflush(stdout);
															if (partcnt>0)
															{
																k=0;
																for (k=0;k<partcnt ;k++ )
																{
																	AssyTg=PartsTg[k];
																	if(AOM_ask_value_string(AssyTg,"item_id",&Part_no)!=ITK_ok)   PrintErrorStack();
																	if(AOM_ask_value_string(AssyTg,"item_revision_id",&P_Rev)!=ITK_ok)   PrintErrorStack();
																	if(AOM_ask_value_string(AssyTg,"t5_PartType",&P_Type)!=ITK_ok)   PrintErrorStack();
																	printf("\n A1: Function calling parent part : %s:%s:%s",Part_no,P_Rev,P_Type);	fflush(stdout);	
																	if((tc_strcmp(P_Type,"CCVC")==0)||(tc_strcmp(P_Type,"VC")==0)||(tc_strcmp(P_Type,"Vehicle Combination")==0)||(tc_strcmp(P_Type,"Configurable VC")==0)||(tc_strcmp(P_Type,"V")==0)||(tc_strcmp(P_Type,"Vehicle")==0))
																	{
																		//Consiere only TO Gate child parts.
																		tc_strcpy(PARTTYPEs,"");
																		tc_strcpy(PARTTYPEs,"CCVC");	
																		printf("\n A8: PARTTYPEs:[%s]\n", PARTTYPEs);fflush(stdout);
																	}
																	else
																	{
																		tc_strcpy(PARTTYPEs,"");
																		tc_strcpy(PARTTYPEs,"MODULE");	
																		printf("\n A7: PARTTYPEs:[%s]\n", PARTTYPEs);fflush(stdout);
																	}
																	//First level parts.
																	MRDMLFun=0;
																	MRDMLFun = tm_POCheckStopAtFFF(AssyTg,sDml_TP,sEESWPrtType);
																	printf("\n A2: Function done for parent part:%s MRDMLFun: %d",Part_no,MRDMLFun);	fflush(stdout);	
																	if (MRDMLFun >0)
																	{
																		//Expanssion start
																		printf("\n A3: BOM Window setting.");fflush(stdout);
																		if(BOM_create_window(&tagBOMwindow)!=ITK_ok)   PrintErrorStack();
																		if(CFM_find("ERC release and above", &tagRule)!=ITK_ok)   PrintErrorStack(); 
																		if(BOM_set_window_config_rule( tagBOMwindow, tagRule )!=ITK_ok)   PrintErrorStack();
																		//0-qty skip
																		if(PIE_find_closure_rules("TMLBOMExpandByQty",PIE_TEAMCENTER, &rulefounddd, &POclosureruleee )!=ITK_ok)   PrintErrorStack();
																		//printf ("PO3:rulefounddd count %d \n",rulefounddd);fflush(stdout);
																		if (rulefounddd > 0)
																		{
																			POclose_taggg = POclosureruleee[0];
																			//printf ("PO3: POclose_taggg closure rule found \n");fflush(stdout);
																		}
																		if(BOM_window_set_closure_rule( tagBOMwindow,POclose_taggg, 0, POrulenameee,POrulevalueee )!=ITK_ok)   PrintErrorStack();
																		//0-qty skip

																		if(BOM_set_window_top_line(tagBOMwindow, NULLTAG, AssyTg, NULLTAG, &tagBOMline)!=ITK_ok)   PrintErrorStack();
																		if(BOM_line_ask_child_lines(tagBOMline, &childCountTopBomLine, &tagTopBLchildLine)!=ITK_ok)   PrintErrorStack();
																		printf("\n A4:FLP:No of child objects are n : %d\n",childCountTopBomLine);fflush(stdout);
																		for(iCount =0 ; iCount < childCountTopBomLine; iCount++)
																		{
																			printf("\n A5:MR:Exp start Parent Part_no:[%s] No of child taken :[%d] \n",Part_no,iCount);fflush(stdout);
																			
																			FunOutflag=0;
																			FunOutflag= tm_POCheckStopAtFBOMline(tagTopBLchildLine[iCount],sDml_TP,sEESWPrtType,Part_no,P_Rev);
																			printf("\n A6:MR:Exp start Parent Part_no:[%s] FunOutflag:[%d]",Part_no,FunOutflag);fflush(stdout);
																		}
																	}
																}
															}
														}
														if(tc_strcmp(DMLtype,"TODR")==0)
														{
															if(AOM_ask_value_tags(Dml_tasks[tsk],"CMReferences",&partcnt,&PartsTg)!=ITK_ok)   PrintErrorStack();
															printf("\n T1: Now partcnt:%d",partcnt);fflush(stdout);
															if (partcnt>0)
															{
																for (k=0;k<partcnt ;k++ )
																{
																	AssyTg=PartsTg[k];
																	if(AOM_UIF_ask_value(AssyTg, "object_type", &Partobj_type)!=ITK_ok)   PrintErrorStack();
																	printf("\n T2: Partobj_type is : %s",Partobj_type);	fflush(stdout);	
																	if(tc_strcmp(Partobj_type,"Folder")!=0)
																	{
																		if(AOM_ask_value_string(AssyTg,"item_id",&Part_nobr)!=ITK_ok)   PrintErrorStack();
																		if(AOM_ask_value_string(AssyTg,"t5_PartType",&P_Type)!=ITK_ok)   PrintErrorStack();
																		printf("\n T3: Part_nobr is : %s P_Type:%s",Part_nobr,P_Type);	fflush(stdout);
																		if((tc_strcmp(P_Type,"CCVC")==0)||(tc_strcmp(P_Type,"VC")==0)||(tc_strcmp(P_Type,"Vehicle Combination")==0)||(tc_strcmp(P_Type,"Configurable VC")==0)||(tc_strcmp(P_Type,"V")==0)||(tc_strcmp(P_Type,"Vehicle")==0))
																		{
																			//Consiere only TO Gate child parts.
																			tc_strcpy(PARTTYPEs,"");
																			tc_strcpy(PARTTYPEs,"CCVC");	
																			printf("\n T4: PARTTYPEs:[%s]\n", PARTTYPEs);fflush(stdout);
																		}
																		else
																		{
																			tc_strcpy(PARTTYPEs,"");
																			tc_strcpy(PARTTYPEs,"MODULE");	
																			printf("\n T5: PARTTYPEs:[%s]\n", PARTTYPEs);fflush(stdout);
																		}
																		
																		printf("\n T6:Function calling for parent part:%s\n",Part_nobr);fflush(stdout);
																		Prt_tag= t5GetItemRevisonForRelRev(Part_nobr,PARTTYPEs);
																		if(Prt_tag==NULLTAG)
																		{
																			printf("\n T7:Prt_tag is Null...\n");fflush(stdout);
																		}
																		else
																		{
																			printf("\n T8:Prt_tag is found...\n");fflush(stdout);
																			if(AOM_ask_value_string(Prt_tag,"item_id",&Part_nobrr)!=ITK_ok)   PrintErrorStack();
																			if(AOM_ask_value_string(Prt_tag,"t5_PartType",&P_Typee)!=ITK_ok)   PrintErrorStack();
																			if(AOM_UIF_ask_value(Prt_tag, "item_revision_id", &rev_iddd)!=ITK_ok)PrintErrorStack();
																			if(AOM_ask_value_string(Prt_tag,"t5_PartStatus",&PrtDRstt)!=ITK_ok)PrintErrorStack();
																			printf("\n T9: Parent part: %s P_Typee:%s rev_iddd:%s PrtDRstt:%s",Part_nobrr,P_Typee,rev_iddd,PrtDRstt);	fflush(stdout);	
																			
																			if((tc_strcmp(P_Type,"CCVC")==0)||(tc_strcmp(P_Type,"VC")==0)||(tc_strcmp(P_Type,"Vehicle Combination")==0)||(tc_strcmp(P_Type,"Configurable VC")==0))
																			{
																				//expand CCV to snapshot.
																				if(GRM_find_relation_type("T5_PartHasSnapShot", &CCVtoSnapShot_Rel_Type)!=ITK_ok)PrintErrorStack();
																				if (CCVtoSnapShot_Rel_Type!=NULLTAG)
																				{
																					SnpShtcount=0;
																					if(GRM_list_secondary_objects_only(Prt_tag,CCVtoSnapShot_Rel_Type,&SnpShtcount,&SnpShtTags)!=ITK_ok)PrintErrorStack();
																					printf("\n C1:SnpShtcount: %d",SnpShtcount);fflush(stdout);
																					if(SnpShtcount > 0)
																					{
																						SnpShtTag = SnpShtTags[0];
																						if(TCTYPE_ask_object_type(SnpShtTag,&SnpObjTypeTag)!=ITK_ok)PrintErrorStack();
																						if(TCTYPE_ask_name(SnpObjTypeTag,SnpObjTypeNm)!=ITK_ok)PrintErrorStack();
																						printf("\n C2:SnpObjTypeNm:: [%s].",SnpObjTypeNm); fflush(stdout);

																						if(strcmp(SnpObjTypeNm,"Snapshot")==0)
																						{
																							if(AOM_ask_value_string(SnpShtTag,"object_name",&SnpShtNo)!=ITK_ok)   PrintErrorStack();
																								printf("\n C3:SnapshotName:%s ......",SnpShtNo);fflush(stdout);

																							if(AOM_ask_value_tag(SnpShtTag,"topLine",&Modl_rev_tags)!=ITK_ok)PrintErrorStack();

																							printf("\n C4:BOM_create_window_from_snapshot...");fflush(stdout);
																							if(BOM_create_window_from_snapshot(SnpShtTag,&Snpwindow)!=ITK_ok)PrintErrorStack();
																							//printf("\n CCV:BOM_set_window_top_line...\n");fflush(stdout);
																							if(BOM_set_window_top_line(Snpwindow, NULLTAG, Modl_rev_tags, NULLTAG, &SnpBOMline)!=ITK_ok)   PrintErrorStack();
																							//printf("\n CCV:BOM_line_ask_child_lines...\n");fflush(stdout);
																							if(BOM_line_ask_child_lines(SnpBOMline, &childCountSnpBOMline, &SnpBOMlineTags)!=ITK_ok)   PrintErrorStack();
																							printf("\n C5:ArcNodCount:%d: ",childCountSnpBOMline);fflush(stdout);
																							for(mdl =0 ; mdl < childCountSnpBOMline; mdl++)
																							{
																								SnpBOMlineTag = SnpBOMlineTags[mdl];
																								if(AOM_ask_value_string(SnpBOMlineTags[mdl],"bl_item_item_id",&ArcMdlNo)!=ITK_ok)   PrintErrorStack();
																								printf("\n C6:Arc_Module:[%s] :total Arc_module:[%d] Taken Arc_module:[%d]",ArcMdlNo,childCountSnpBOMline,mdl);fflush(stdout);
																								ArcMdlNo_tag= t5GetItemRevison(ArcMdlNo);
																								if(ArcMdlNo_tag==NULLTAG)
																								{
																									printf("\n C7:ArcMdlNo_tag is NULL.\n");fflush(stdout);
																									//return 0;
																								}
																								else
																								{
																									//printf("\n CCV:ArcMdlNo_tag found.\n");fflush(stdout);
																									if(ITEM_ask_latest_rev (ArcMdlNo_tag, &ArcMdlNo_tg)!=ITK_ok)PrintErrorStack();
																									if(ArcMdlNo_tg==NULLTAG)
																									{
																										printf("\n C8:Object not found ArcMdlNo_tg...!!\n");fflush(stdout);
																										//return 0;
																									}
																									else
																									{
																										//Expanssion start
																										//printf("\n CCV: BOM Window setting.");fflush(stdout);
																										if(BOM_create_window(&MdlBOMwindow)!=ITK_ok)   PrintErrorStack();
																										if(CFM_find("ERC release and above", &MdltagRule)!=ITK_ok)   PrintErrorStack(); 
																										if(BOM_set_window_config_rule( MdlBOMwindow, MdltagRule )!=ITK_ok)   PrintErrorStack();
																										//0-qty skip
																										//if(PIE_find_closure_rules("BOMLineSkipZeroQtyMaskUnconfig",PIE_TEAMCENTER, &MdlCount, &POclosRul )!=ITK_ok)   PrintErrorStack();
																										//if(PIE_find_closure_rules("BOMLineskipforunconfigured",PIE_TEAMCENTER, &MdlCount, &POclosRul )!=ITK_ok)   PrintErrorStack();
																										if(PIE_find_closure_rules("BOMLineSkipZeroQtyMaskUnconfig",PIE_TEAMCENTER, &MdlCount, &POclosRul )!=ITK_ok)   PrintErrorStack();
																										//printf ("CCV:Closure Rule count %d \n",MdlCount);fflush(stdout);
																										if (MdlCount > 0)
																										{
																											POcloseRul_tag = POclosRul[0];
																											//printf ("CCV: POcloseRul_tag closure rule found \n");fflush(stdout);
																										}
																										if(BOM_window_set_closure_rule(MdlBOMwindow,POcloseRul_tag, 0, POrulNm,Mdlrulevl )!=ITK_ok)   PrintErrorStack();
																										if(BOM_set_window_top_line(MdlBOMwindow, NULLTAG, ArcMdlNo_tg, NULLTAG, &MdlBOMline)!=ITK_ok)   PrintErrorStack();
																										//printf ("CCV:BBBBBBBBBB \n");fflush(stdout);
																										MdlchildCount=0;
																										if(BOM_line_ask_child_lines(MdlBOMline, &MdlchildCount, &MdlTopBLchildLine)!=ITK_ok)   PrintErrorStack();
																										printf("\n C9:Arc_Module:[%s] :total Arc_module:[%d] Taken Arc_module:[%d] Total Modules:[%d]",ArcMdlNo,childCountSnpBOMline,mdl,MdlchildCount);fflush(stdout);
																										for(mdll =0 ; mdll < MdlchildCount; mdll++)
																										{
																											if(AOM_ask_value_string(MdlTopBLchildLine[mdll],"bl_item_item_id",&MdlNo)!=ITK_ok)   PrintErrorStack();
																											if(AOM_ask_value_string(MdlTopBLchildLine[mdll],"bl_Design Revision_t5_PartStatus",&Mdl_DR)!=ITK_ok)   PrintErrorStack();
																											printf("\n C10:Arc_Module:[%s] Total Arc_module:[%d] Taken Arc_module:[%d] Total Modules:[%d] Taken Module:[%s] Module no:[%s] Module DR:[%s]",ArcMdlNo,childCountSnpBOMline,mdl,MdlchildCount,mdll,MdlNo,Mdl_DR);fflush(stdout);
																											//AAAAAAAAAAAAAA
																											MdlLstRev_tag= t5GetItemRevisonForRelRev(MdlNo,PARTTYPEs);
																											if(MdlLstRev_tag==NULLTAG)
																											{
																												printf("\n C10:MdlLstRev_tag is NULL.\n");fflush(stdout);
																												//return 0;
																											}
																											else
																											{
																												printf("\n C11:MdlLstRev_tag found.\n");fflush(stdout);
																												if(AOM_ask_value_string(MdlLstRev_tag,"item_id",&Part_nobrrr)!=ITK_ok)   PrintErrorStack();
																												if(AOM_ask_value_string(MdlLstRev_tag,"t5_PartType",&P_Typeer)!=ITK_ok)   PrintErrorStack();
																												if(AOM_UIF_ask_value(MdlLstRev_tag, "item_revision_id", &rev_idddr)!=ITK_ok)PrintErrorStack();
																												if(AOM_ask_value_string(MdlLstRev_tag,"t5_PartStatus",&PrtDRsttr)!=ITK_ok)PrintErrorStack();
																												printf("\n C12:Call Module:[%s] Module type:[%s] Module Rev:[%s] Module DR:[%s]",Part_nobrrr,P_Typeer,rev_idddr,PrtDRsttr);	fflush(stdout);	
																												GMDMLFun=0;
																												GMDMLFun = tm_POCheckStopAtFFF(MdlLstRev_tag,sDml_TP,sEESWPrtType);
																												printf("\n C13:fun done for Module:[%s] GMDMLFun..: [%d]",Part_nobrrr,GMDMLFun);fflush(stdout);
																												if (GMDMLFun >0)
																												{
																													//Expanssion start
																													printf("\n C14: BOM Window setting.");fflush(stdout);
																													if(BOM_create_window(&tagBOMwindow)!=ITK_ok)   PrintErrorStack();
																													if(CFM_find("ERC release and above", &tagRule)!=ITK_ok)   PrintErrorStack(); 
																													if(BOM_set_window_config_rule( tagBOMwindow, tagRule )!=ITK_ok)   PrintErrorStack();
																													//0-qty skip
																													if(PIE_find_closure_rules("TMLBOMExpandByQty",PIE_TEAMCENTER, &rulefoundXO, &POclosureruleXO )!=ITK_ok)   PrintErrorStack();
																													//printf ("PO3:rulefounddd count %d \n",rulefoundXO);fflush(stdout);
																													if (rulefoundXO > 0)
																													{
																														POclose_tagXO = POclosureruleXO[0];
																														//printf ("PO3: POclose_taggg closure rule found \n");fflush(stdout);
																													}
																													if(BOM_window_set_closure_rule( tagBOMwindow,POclose_tagXO, 0, POrulenameXO,POrulevalueXO )!=ITK_ok)   PrintErrorStack();
																													//0-qty skip

																													if(BOM_set_window_top_line(tagBOMwindow, NULLTAG, MdlLstRev_tag, NULLTAG, &tagBOMline)!=ITK_ok)   PrintErrorStack();
																													if(BOM_line_ask_child_lines(tagBOMline, &childCountTopBomLine, &tagTopBLchildLine)!=ITK_ok)   PrintErrorStack();
																													printf("\n C15:Exp parent Module:%s with revision:%s having no of child Parts: %d\n",Part_nobrrr,rev_idddr,childCountTopBomLine);fflush(stdout);
																													for(iCount =0 ; iCount < childCountTopBomLine; iCount++)
																													{
																														if(AOM_ask_value_string(tagTopBLchildLine[iCount],"bl_item_item_id",&MoldChild)!=ITK_ok)   PrintErrorStack();
																														printf("\n C16:Exp start for parent Module:[%s] taken child part :[%d] child Part: [%s] \n",Part_nobrrr,iCount,MoldChild);fflush(stdout);
																														FunOutflag=0;
																														FunOutflag= tm_POCheckStopAtFBOMline(tagTopBLchildLine[iCount],sDml_TP,sEESWPrtType,Part_nobrrr,rev_idddr);
																														printf("\n C17:Exp done for parent Module:[%s] taken child part :[%d] child Part: [%s] FunOutflag:[%d]\n",Part_nobrrr,iCount,MoldChild,FunOutflag);fflush(stdout);
																													}
																												}
																											}
																										}
																									}
																								}
																							}
																						}
																					}
																					else
																					{
																						printf("\n C16:ERROR:Snapshot not available:%s:\n",Part_nobr);fflush(stdout);	
																						if((NwNOPOPartsSet==NULL)|| (tc_strcmp(NwNOPOPartsSet,"")==0))
																						{
																							tc_strcpy (NwNOPOPartsSet,"" );
																							tc_strcat (NwNOPOPartsSet,Part_nobr);
																							printf("\n C18:Snapshot not available:NwNOPOPartsSet. %s",Part_nobr);	fflush(stdout);	
																						}
																						else
																						{
																							if(strstr(NwNOPOPartsSet,Part_nobr)==NULL)
																							{
																								tc_strcat (NwNOPOPartsSet,"," );
																								tc_strcat (NwNOPOPartsSet,Part_nobr);
																								printf("\n C19:Snapshot not available:NwNOPOPartsSet.. %s",Part_nobr);	fflush(stdout);
																							}
																						}
																					}
																				}
																			}
																			else
																			{
																				GMDMLFun=0;
																				GMDMLFun = tm_POCheckStopAtFFF(Prt_tag,sDml_TP,sEESWPrtType);
																				printf("\n T10:Fun1 done for Parent part:[%s] GMDMLFun:[%d]",Part_nobrr,GMDMLFun);fflush(stdout);
																				if (GMDMLFun >0)
																				{
																					//Expanssion start
																					printf("\n T11: BOM Window setting.");fflush(stdout);
																					if(BOM_create_window(&tagBOMwindow)!=ITK_ok)   PrintErrorStack();
																					if(CFM_find("ERC release and above", &tagRule)!=ITK_ok)   PrintErrorStack(); 
																					if(BOM_set_window_config_rule( tagBOMwindow, tagRule )!=ITK_ok)   PrintErrorStack();
																					//0-qty skip
																					if(PIE_find_closure_rules("TMLBOMExpandByQty",PIE_TEAMCENTER, &rulefoundXO, &POclosureruleXO )!=ITK_ok)   PrintErrorStack();
																					//printf ("PO3:rulefounddd count %d \n",rulefoundXO);fflush(stdout);
																					if (rulefoundXO > 0)
																					{
																						POclose_tagXO = POclosureruleXO[0];
																						//printf ("PO3: POclose_taggg closure rule found \n");fflush(stdout);
																					}
																					if(BOM_window_set_closure_rule( tagBOMwindow,POclose_tagXO, 0, POrulenameXO,POrulevalueXO )!=ITK_ok)   PrintErrorStack();
																					//0-qty skip

																					if(BOM_set_window_top_line(tagBOMwindow, NULLTAG, Prt_tag, NULLTAG, &tagBOMline)!=ITK_ok)   PrintErrorStack();
																					if(BOM_line_ask_child_lines(tagBOMline, &childCountTopBomLine, &tagTopBLchildLine)!=ITK_ok)   PrintErrorStack();
																					printf("\n T12:Parent Part:%s with revision:%s No of child parts: %d\n",Part_nobrr,rev_iddd,childCountTopBomLine);fflush(stdout);
																					for(iCount =0 ; iCount < childCountTopBomLine; iCount++)
																					{
																						printf("\n T13:Exp start Parent P1:%s No of child taken: %d \n",Part_nobr,iCount);fflush(stdout);
																						FunOutflag=0;
																						FunOutflag= tm_POCheckStopAtFBOMline(tagTopBLchildLine[iCount],sDml_TP,sEESWPrtType,Part_nobrr,rev_iddd);
																						printf("\n T14:Exp done Parent P1:[%s] FunOutflag:[%d]",Part_nobr,FunOutflag);fflush(stdout);
																					}
																				}
																			}
																			
																		}
																	}
																	else
																	{
																		printf("\n Bypassing Folder for PO check..\n");fflush(stdout);
																	}
																}
															}
														}
													}
												}
												printf("\n Removing parts from Folder.\n");fflush(stdout);
												//if(GRM_find_relation_type("IMAN_reference", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
												if(GRM_find_relation_type("T5_CmHasNOPoDetails", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
												if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
												printf("\n Refcount Value :   %d\n",Refcount);fflush(stdout);
												if(Refcount>0)
												{
													for (jr=0;jr<Refcount;jr++ )
													{
														if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
														if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
														printf("\n NOPO:Reftype_name :: %s\n", Reftype_name);fflush(stdout);
														if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
														printf("\n NOPO:folder name :: %s\n", sNOPOFldr);fflush(stdout);
														//if(tc_strcmp(Reftype_name,"Folder") ==0)
														if((tc_strcmp(Reftype_name,"T5_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"NOPO Parts") ==0))
														{
															
															//if(tc_strcmp(sNOPOFldr,"NOPO_Parts") ==0)
															if((tc_strcmp(sNOPOFldr,"T5_NOPO_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"NOPO Parts") ==0))
															{
																NOPOFldFlag=1;
																printf("\n NOPO Folder already available. Remove earlier parts in available.\n");fflush(stdout);
																if(FL_ask_size(RefDoc_tag[jr],&ReqFdrSize));
																printf("\n NOPO:ReqFdrSize is:   %d\n",ReqFdrSize);fflush(stdout);
																if(FL_ask_references(RefDoc_tag[jr],FL_fsc_by_date_modified ,&FldObjCount,&FoldObj_tag));
																printf("\n NOPO:FldObjCount is..:   %d\n",FldObjCount);fflush(stdout);
																/*if(GRM_find_relation_type("contents", &Fold_Cont_Rel)!=ITK_ok)PrintErrorStack();
																if(GRM_list_secondary_objects_only(RefDoc_tag[jr],Fold_Cont_Rel,&FldObjCount,&FoldObj_tag)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:FldObjCount is.:   %d\n",FldObjCount);fflush(stdout);*/
																if(FldObjCount>0)
																{
																	jrr=0;
																	for (jrr=0;jrr<FldObjCount;jrr++ )
																	{
																		if(AOM_ask_value_string(FoldObj_tag[jrr],"item_id",&sPrtName)!=ITK_ok)PrintErrorStack();
																		printf("\n NOPO:removing part from folder: %s.", sPrtName);fflush(stdout);
																		if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(FL_remove(RefDoc_tag[jr],FoldObj_tag[jrr])!=ITK_ok)PrintErrorStack();
																		if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	}
																	if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
																}
																else
																{
																	printf("\n NOPO:Folder already NULL.\n");fflush(stdout);
																}
																//break;
															}
														}
														//PO revision mismatch hard check
														if((tc_strcmp(Reftype_name,"T5_PSC_PORevMismatch_Parts") ==0)|| (tc_strcmp(Reftype_name,"P&SC PO Rev Mismatch Parts") ==0))
														{
															//if(tc_strcmp(sNOPOFldr,"PORevMismatch_Parts") ==0)
															if((tc_strcmp(sNOPOFldr,"T5_PSC_PORevMismatch_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"P&SC PO Rev Mismatch Parts") ==0))
															{
																NOPOFldFlag=1;
																printf("\n NOPOREV: Folder already available. Remove earlier parts in available.\n");fflush(stdout);
																if(FL_ask_size(RefDoc_tag[jr],&ReqFdrSize));
																printf("\n NOPOREV:ReqFdrSize is:   %d\n",ReqFdrSize);fflush(stdout);
																if(FL_ask_references(RefDoc_tag[jr],FL_fsc_by_date_modified ,&FldObjCount,&FoldObj_tag));
																printf("\n NOPOREV:FldObjCount is..:   %d\n",FldObjCount);fflush(stdout);
																/*if(GRM_find_relation_type("contents", &Fold_Cont_Rel)!=ITK_ok)PrintErrorStack();
																if(GRM_list_secondary_objects_only(RefDoc_tag[jr],Fold_Cont_Rel,&FldObjCount,&FoldObj_tag)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:FldObjCount is.:   %d\n",FldObjCount);fflush(stdout);*/
																if(FldObjCount>0)
																{
																	jrr=0;
																	for (jrr=0;jrr<FldObjCount;jrr++ )
																	{
																		if(AOM_ask_value_string(FoldObj_tag[jrr],"item_id",&sPrtName)!=ITK_ok)PrintErrorStack();
																		printf("\n NOPOREV:removing part from folder: %s.", sPrtName);fflush(stdout);
																		if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(FL_remove(RefDoc_tag[jr],FoldObj_tag[jrr])!=ITK_ok)PrintErrorStack();
																		if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	}
																	if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
																}
																else
																{
																	printf("\n NOPOREV:Folder already NULL.\n");fflush(stdout);
																}
																//break;
															}
														}
														//PO revision mismatch parts
														if((tc_strcmp(Reftype_name,"T5_PORevMismatch_Parts") ==0)|| (tc_strcmp(Reftype_name,"PO Rev Mismatch Parts") ==0))
														{
															//if(tc_strcmp(sNOPOFldr,"PORevMismatch_Parts") ==0)
															if((tc_strcmp(sNOPOFldr,"T5_PORevMismatch_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"PO Rev Mismatch Parts") ==0))
															{
																NOPOFldFlag=1;
																printf("\n NOPOREV: Folder already available. Remove earlier parts in available.\n");fflush(stdout);
																if(FL_ask_size(RefDoc_tag[jr],&ReqFdrSize));
																printf("\n NOPOREV:ReqFdrSize is:   %d\n",ReqFdrSize);fflush(stdout);
																if(FL_ask_references(RefDoc_tag[jr],FL_fsc_by_date_modified ,&FldObjCount,&FoldObj_tag));
																printf("\n NOPOREV:FldObjCount is..:   %d\n",FldObjCount);fflush(stdout);
																/*if(GRM_find_relation_type("contents", &Fold_Cont_Rel)!=ITK_ok)PrintErrorStack();
																if(GRM_list_secondary_objects_only(RefDoc_tag[jr],Fold_Cont_Rel,&FldObjCount,&FoldObj_tag)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:FldObjCount is.:   %d\n",FldObjCount);fflush(stdout);*/
																if(FldObjCount>0)
																{
																	jrr=0;
																	for (jrr=0;jrr<FldObjCount;jrr++ )
																	{
																		if(AOM_ask_value_string(FoldObj_tag[jrr],"item_id",&sPrtName)!=ITK_ok)PrintErrorStack();
																		printf("\n NOPOREV:removing part from folder: %s.", sPrtName);fflush(stdout);
																		if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(FL_remove(RefDoc_tag[jr],FoldObj_tag[jrr])!=ITK_ok)PrintErrorStack();
																		if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	}
																	if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
																}
																else
																{
																	printf("\n NOPOREV:Folder already NULL.\n");fflush(stdout);
																}
																//break;
															}
														}
														//New created NOPO partssss
														if((tc_strcmp(Reftype_name,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"New NOPO Parts") ==0))
														{
															//if(tc_strcmp(sNOPOFldr,"PORevMismatch_Parts") ==0)
															if((tc_strcmp(sNOPOFldr,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"New NOPO Parts") ==0))
															{
																NOPOFldFlag=1;
																printf("\n NwNOPO: Folder already available. Remove earlier parts in available.\n");fflush(stdout);
																if(FL_ask_size(RefDoc_tag[jr],&ReqFdrSize));
																printf("\n NwNOPO:ReqFdrSize is:   %d\n",ReqFdrSize);fflush(stdout);
																if(FL_ask_references(RefDoc_tag[jr],FL_fsc_by_date_modified ,&FldObjCount,&FoldObj_tag));
																printf("\n NwNOPO:FldObjCount is..:   %d\n",FldObjCount);fflush(stdout);
																if(FldObjCount>0)
																{
																	jrr=0;
																	for (jrr=0;jrr<FldObjCount;jrr++ )
																	{
																		if(AOM_ask_value_string(FoldObj_tag[jrr],"item_id",&sPrtName)!=ITK_ok)PrintErrorStack();
																		printf("\n NwNOPO:removing part from folder: %s.", sPrtName);fflush(stdout);
																		if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(FL_remove(RefDoc_tag[jr],FoldObj_tag[jrr])!=ITK_ok)PrintErrorStack();
																		if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	}
																	if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
																}
																else
																{
																	printf("\n NwNOPO:Folder already NULL.\n");fflush(stdout);
																}
																//break;
															}
														}
														if(tc_strcmp(Reftype_name,"Folder") ==0)
														{
															if(tc_strcmp(sNOPOFldr,"UnMaturated DR Status Parts") ==0)
															{
																NOPOFldFlag=1;
																printf("\n UnMat: Folder already available. Remove UnMat parts in available.\n");fflush(stdout);
																if(FL_ask_size(RefDoc_tag[jr],&ReqFdrSize));
																printf("\n UnMat:ReqFdrSize is:   %d\n",ReqFdrSize);fflush(stdout);
																if(FL_ask_references(RefDoc_tag[jr],FL_fsc_by_date_modified ,&FldObjCount,&FoldObj_tag));
																printf("\n UnMat:FldObjCount is..:   %d\n",FldObjCount);fflush(stdout);
																if(FldObjCount>0)
																{
																	jrr=0;
																	for (jrr=0;jrr<FldObjCount;jrr++ )
																	{
																		if(AOM_ask_value_string(FoldObj_tag[jrr],"item_id",&sPrtName)!=ITK_ok)PrintErrorStack();
																		printf("\n UnMat:removing part from folder: %s.", sPrtName);fflush(stdout);
																		if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(FL_remove(RefDoc_tag[jr],FoldObj_tag[jrr])!=ITK_ok)PrintErrorStack();
																		if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																		if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	}
																	if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																	if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
																}
																else
																{
																	printf("\n UnMat:Folder already NULL.\n");fflush(stdout);
																}
																//break;
															}
														}
													}
												}
												printf("\n Folder creation.\n");fflush(stdout);
												//NOPO Parts
												if ((NOPOPartsSet==NULL)||(tc_strcmp(NOPOPartsSet,"")==0))
												{
													fprintf(fd,"\n SET OF NOPO PARTS ARE NOT FOUND. \n");
													printf("\n PO:NOPOPartsSet is NULL.\n");fflush(stdout);
												}
												else
												{
													Errorflag=1;
													//NOPO Folder creation
													printf("\n NOPO Folder creation.\n");fflush(stdout);
													NOPOFldFlag=0;
													//if(GRM_find_relation_type("IMAN_reference", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_find_relation_type("T5_CmHasNOPoDetails", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n Refcount Value :   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															NOPOFldFlag=0;
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPO:Reftype_name :: %s\n", Reftype_name);fflush(stdout);

															//if(tc_strcmp(Reftype_name,"Folder") ==0)
															if((tc_strcmp(Reftype_name,"T5_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"NOPO Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:folder name :: %s\n", sNOPOFldr);fflush(stdout);
																//if(tc_strcmp(sNOPOFldr,"NOPO_Parts") ==0)
																if((tc_strcmp(sNOPOFldr,"T5_NOPO_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"NOPO Parts") ==0))
																{
																	NOPOFldFlag=1;
																	printf("\n NOPO Folder already available.\n");fflush(stdout);
																	break;
																}
															}
														}
													}
													if(NOPOFldFlag ==0)
													{
														printf("\n NOPO Folder not available.\n");fflush(stdout);
														//NOPO Folder creation
														if(TCTYPE_find_type("T5_NOPO_Parts", "Folder", &item_type_tag)!=ITK_ok)PrintErrorStack();
														if(TCTYPE_construct_create_input(item_type_tag, &item_create_input_tag)!=ITK_ok)PrintErrorStack();
														if(AOM_set_value_string(item_create_input_tag, "object_name", "NOPO Parts")!=ITK_ok)PrintErrorStack();
														if(TCTYPE_create_object (item_create_input_tag, &NOPOobject)!=ITK_ok)PrintErrorStack();
														//if(AE_save_myself(wordLatestDat_t)!=ITK_ok)PrintErrorStack();
														if(AOM_save (NOPOobject)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(NOPOobject,TRUE)!=ITK_ok)PrintErrorStack();

														if(AOM_lock( DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(GRM_create_relation(DMLRevTg,NOPOobject,NOPO_dml_Ref_Rel,NULLTAG,&NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(AOM_load(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(GRM_save_relation(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														printf("\n NOPO:Folder created. .....\n");fflush(stdout);
														if(AOM_save (NOPOobject)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(NOPOobject,TRUE)!=ITK_ok)PrintErrorStack();
														if(AOM_save (DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_unlock(DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
													}
													//Going to add NOPO parts
													jr=0;
													Refcount=0;
													printf("\n Going to add NOPO parts in folder.\n");fflush(stdout);
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n NOPO:.Refcount Value :   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPO:.Reftype_name:%s\n", Reftype_name);fflush(stdout);

															//if(tc_strcmp(Reftype_name,"Folder") ==0)
															if((tc_strcmp(Reftype_name,"T5_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"NOPO Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:.folder name: %s\n", sNOPOFldr);fflush(stdout);
																//if(tc_strcmp(sNOPOFldr,"NOPO_Parts") ==0)
																if((tc_strcmp(sNOPOFldr,"T5_NOPO_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"NOPO Parts") ==0))
																{
																	//if(GRM_find_relation_type("Contents", &Fold_Cont_Rel)!=ITK_ok)PrintErrorStack();
																	printf("\n NOPO addtition start.\n");fflush(stdout);
																	fprintf(fd,"\n SET OF NOPO PARTS ARE BELOW.\n[PO to be placed or VCMS allowed]");
																	nopo=0;
																	piStringCount=0;
																	if(EPM__parse_string(NOPOPartsSet,",",&piStringCount,&GStringList)!=ITK_ok)PrintErrorStack();
																	for (nopo=0;nopo<piStringCount ;nopo++ )
																	{
																		printf("\n PO:..NOPOPartsSet Part:%s \n",GStringList[nopo]);fflush(stdout);
																		//if(ITEM_find_item (GStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
																		NOPOpart_tg= t5GetItemRevison(GStringList[nopo]);
																		if(NOPOpart_tg==NULLTAG)
																		{
																			printf("\n UnMat:P9:Object not found in Teamcenter.\n");fflush(stdout);
																		}
																		else
																		{
																			PrtRev_count=0;
																			if(ITEM_list_all_revs (NOPOpart_tg, &PrtRev_count, &PrtRev_list)!=ITK_ok)   PrintErrorStack();
																			printf("\n P67:Total rev found: %d.", PrtRev_count);fflush(stdout);
																			if(PrtRev_count > 0)
																			{
																				pii=0;
																				for(pii=PrtRev_count-1;pii>=0;pii--)
																				{
																					PrtRelStus_cnt=0;
																					PrtRev_tg=PrtRev_list[pii];
																					if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtnoo)!=ITK_ok)   PrintErrorStack();
																					if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRevv)!=ITK_ok)   PrintErrorStack();
																					printf("\n P68:part no:%s revision:%s.",nPrtnoo,nPrtRevv);fflush(stdout);
																					if(WSOM_ask_release_status_list (PrtRev_tg, &PrtRelStus_cnt, &PrtRelStusTg)!=ITK_ok)PrintErrorStack();
																					printf("\n P69:PrtRelStus_cnt is:%d.", PrtRelStus_cnt);fflush(stdout);
																					if(PrtRelStus_cnt > 0)
																					{
																						n=0;
																						for(n=0;n<PrtRelStus_cnt;n++)
																						{
																							RelRevFlag=0;
																							//Checking PO availability on any revision.
																							if(AOM_ask_value_string (PrtRelStusTg[n], "object_name", &PrtRelStus)!=ITK_ok)PrintErrorStack();
																							printf("\n P70:Latest released status is:%s.",PrtRelStus);fflush(stdout);
																							if(tc_strstr(PrtRelStus,"T5_LcsErcRlzd")!=NULL)
																							{
																								RelRevFlag=1;
																								printf("\n PO:P1:going to insert part in folder.%s \n",GStringList[nopo]);fflush(stdout);
																								if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								//if(FL_insert(RefDoc_tag[jr],NOPOprt_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(PrtRev_tg,TRUE)!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
																								printf("\n PO:P1:part details in report..\n");fflush(stdout);
																								if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																								if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																								}
																								else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																								}
																								else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else
																								{
																									printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																								}
																								printf("\n PO:P1:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																								printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																								fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
																								break;
																							}
																						}
																						if(RelRevFlag>0)
																						{
																							break;
																						}
																					}
																				}
																			}
																		}
																	}
																	fprintf(fd,"\n ");
																	break;
																}
															}
														}
													}
													//NOPO Folder creation end
											
													printf("\n PO:ERROR:NO PO Parts available for DML:\n%s\n",NOPOPartsSet);fflush(stdout);
													//EMH_store_error_s2(EMH_severity_error,ITK_err,"As per SAP informantion PO is not available for below mantioned parts.\n Please get the PO available in SAP or raise VCMS request.\n",NOPOlist);	
													//return 1;
												}
												
												//NEW CREATED PO REVISION MISMATCH PARTS
												if ((NwNOPOPartsSet==NULL)||(tc_strcmp(NwNOPOPartsSet,"")==0))
												{
													fprintf(fd,"\n SET OF NOPO PARTS CREATED AFTER JAN 2019 ARE NOT FOUND. \n");
													printf("\n PO:NwNOPOPartsSet is NULL.\n");fflush(stdout);
												}
												else
												{
													Errorflag=1;
													//NOPO Folder creation
													printf("\n NwNOPO Folder creation.\n");fflush(stdout);
													NOPOFldFlag=0;
													//if(GRM_find_relation_type("IMAN_reference", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_find_relation_type("T5_CmHasNOPoDetails", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n Refcount Value :   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															NOPOFldFlag=0;
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPO:Reftype_name :: %s\n", Reftype_name);fflush(stdout);

															//if(tc_strcmp(Reftype_name,"Folder") ==0)
															if((tc_strcmp(Reftype_name,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"New NOPO Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:folder name :: %s\n", sNOPOFldr);fflush(stdout);
																//if(tc_strcmp(sNOPOFldr,"NOPO_Parts") ==0)
																if((tc_strcmp(sNOPOFldr,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"New NOPO Parts") ==0))
																{
																	NOPOFldFlag=1;
																	printf("\n NOPO Folder already available.\n");fflush(stdout);
																	break;
																}
															}
														}
													}
													if(NOPOFldFlag ==0)
													{
														printf("\n NOPO Folder not available.\n");fflush(stdout);
														//NOPO Folder creation
														if(TCTYPE_find_type("T5_New_NOPO_Parts", "Folder", &item_type_tag)!=ITK_ok)PrintErrorStack();
														if(TCTYPE_construct_create_input(item_type_tag, &item_create_input_tag)!=ITK_ok)PrintErrorStack();
														if(AOM_set_value_string(item_create_input_tag, "object_name", "New NOPO Parts")!=ITK_ok)PrintErrorStack();
														if(TCTYPE_create_object (item_create_input_tag, &NOPOobject)!=ITK_ok)PrintErrorStack();
														//if(AE_save_myself(wordLatestDat_t)!=ITK_ok)PrintErrorStack();
														if(AOM_save (NOPOobject)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(NOPOobject,TRUE)!=ITK_ok)PrintErrorStack();

														if(AOM_lock( DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(GRM_create_relation(DMLRevTg,NOPOobject,NOPO_dml_Ref_Rel,NULLTAG,&NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(AOM_load(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(GRM_save_relation(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														printf("\n NOPO:Folder created. .....\n");fflush(stdout);
														if(AOM_save (NOPOobject)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(NOPOobject,TRUE)!=ITK_ok)PrintErrorStack();
														if(AOM_save (DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_unlock(DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
													}
													//Going to add NOPO parts
													jr=0;
													Refcount=0;
													printf("\n Going to add new NOPO parts in folder.\n");fflush(stdout);
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n NOPO:.Refcount Value :   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPO:.Reftype_name:%s\n", Reftype_name);fflush(stdout);

															//if(tc_strcmp(Reftype_name,"Folder") ==0)
															if((tc_strcmp(Reftype_name,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"New NOPO Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:.folder name: %s\n", sNOPOFldr);fflush(stdout);
																//if(tc_strcmp(sNOPOFldr,"NOPO_Parts") ==0)
																if((tc_strcmp(sNOPOFldr,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"New NOPO Parts") ==0))
																{
																	//if(GRM_find_relation_type("Contents", &Fold_Cont_Rel)!=ITK_ok)PrintErrorStack();
																	printf("\n NOPO addtition start.\n");fflush(stdout);
																	fprintf(fd,"\n SET OF NOPO PARTS CREATED AFTER JAN 2019 ARE BELOW.\n[PO is mandatory on latest released revision. VCMS not allowed.]");
																	nopo=0;
																	piStringCount=0;
																	if(EPM__parse_string(NwNOPOPartsSet,",",&piStringCount,&GStringList)!=ITK_ok)PrintErrorStack();
																	for (nopo=0;nopo<piStringCount ;nopo++ )
																	{
																		printf("\n PO:NwNOPOPartsSet Part:%s \n",GStringList[nopo]);fflush(stdout);
																		//if(ITEM_find_item (GStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
																		NOPOpart_tg= t5GetItemRevison(GStringList[nopo]);
																		if(NOPOpart_tg==NULLTAG)
																		{
																			printf("\n UnMat:P9:Object not found in Teamcenter.\n");fflush(stdout);
																		}
																		else
																		{
																			if(ITEM_list_all_revs (NOPOpart_tg, &PrtRev_count, &PrtRev_list)!=ITK_ok)   PrintErrorStack();
																			printf("\n P67:Total rev found: %d.", PrtRev_count);fflush(stdout);
																			if(PrtRev_count > 0)
																			{
																				pii=0;
																				for(pii=PrtRev_count-1;pii>=0;pii--)
																				{
																					PrtRelStus_cnt=0;
																					PrtRev_tg=PrtRev_list[pii];
																					if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtnoo)!=ITK_ok)   PrintErrorStack();
																					if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRevv)!=ITK_ok)   PrintErrorStack();
																					printf("\n C68:part no:%s revision:%s.",nPrtnoo,nPrtRevv);fflush(stdout);
																					if(WSOM_ask_release_status_list (PrtRev_tg, &PrtRelStus_cnt, &PrtRelStusTg)!=ITK_ok)PrintErrorStack();
																					printf("\n P69:PrtRelStus_cnt is:%d.", PrtRelStus_cnt);fflush(stdout);
																					if(PrtRelStus_cnt > 0)
																					{
																						n=0;
																						for(n=0;n<PrtRelStus_cnt;n++)
																						{
																							RelRevFlag=0;
																							//Checking PO availability on any revision.
																							if(AOM_ask_value_string (PrtRelStusTg[n], "object_name", &PrtRelStus)!=ITK_ok)PrintErrorStack();
																							printf("\n P69:PrtRelStus is:%s.", PrtRelStus);fflush(stdout);
																							if(tc_strstr(PrtRelStus,"T5_LcsErcRlzd")!=NULL)
																							{
																								RelRevFlag=1;
																								printf("\n PO:P9:going to insert part in folder.%s \n",GStringList[nopo]);fflush(stdout);
																								if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								//if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(PrtRev_tg,TRUE)!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
																								printf("\n PO:P9:part details in report..\n");fflush(stdout);
																								if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																								if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																								}
																								else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																								}
																								else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else
																								{
																									printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																								}
																								printf("\n NwPO:P1:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																								printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																								fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
																								break;
																							}
																						}
																						if(RelRevFlag>0)
																						{
																							break;
																						}
																					}
																				}
																			}
																		}
																	}
																	fprintf(fd,"\n ");
																	break;
																}
															}
														}
													}
													printf("\n PO:ERROR:NwNOPOPartsSet:\n%s\n",NwNOPOPartsSet);fflush(stdout);
												}
												//Checking NOPO Rev Parts
												if((PSCPORevNOPOPartsSet==NULL)|| (tc_strcmp(PSCPORevNOPOPartsSet,"")==0))
												{
													fprintf(fd,"\n SET OF PO REV MISMATCH FOR PARTS ARE NOT FOUND. (Created/Modified after 1 Jan 2019.)\n");
													printf("\n PO:PSCPORevNOPOPartsSet is NULL.\n");fflush(stdout);
												}
												else
												{
													Errorflag=1;
													printf("\n PO REV MISMATCH Folder creation.\n");fflush(stdout);
													NOPORevFldFlag=0;
													//if(GRM_find_relation_type("IMAN_reference", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_find_relation_type("T5_CmHasNOPoDetails", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n NOPORev:Refcount Value :   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															NOPORevFldFlag=0;
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPORev:Reftype_name :: %s\n", Reftype_name);fflush(stdout);

															//if(tc_strcmp(Reftype_name,"Folder") ==0)
															if((tc_strcmp(Reftype_name,"T5_PSC_PORevMismatch_Parts") ==0)|| (tc_strcmp(Reftype_name,"P&SC PO Rev Mismatch Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n NOPORev:folder name :: %s\n", sNOPOFldr);fflush(stdout);
																//if(tc_strcmp(sNOPOFldr,"PORevMismatch_Parts") ==0)
																if((tc_strcmp(sNOPOFldr,"T5_PSC_PORevMismatch_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"P&SC PO Rev Mismatch Parts") ==0))
																{
																	NOPORevFldFlag=1;
																	printf("\n NOPORev Rev Folder already availableRemove earlier parts in available.\n");fflush(stdout);
																	break;
																}
															}
														}
													}
													if(NOPORevFldFlag ==0)
													{
														printf("\n PORevMismatch_Parts Folder not available.\n");fflush(stdout);
														//NOPO Folder creation
														if(TCTYPE_find_type("T5_PSC_PORevMismatch_Parts", "Folder", &item_type_tag)!=ITK_ok)PrintErrorStack();
														if(TCTYPE_construct_create_input(item_type_tag, &item_create_input_tag)!=ITK_ok)PrintErrorStack();
														//if(AOM_set_value_string(item_create_input_tag, "object_name", "PORevMismatch_Parts")!=ITK_ok)PrintErrorStack();
														if(AOM_set_value_string(item_create_input_tag, "object_name", "P&SC PO Rev Mismatch Parts")!=ITK_ok)PrintErrorStack();
														if(TCTYPE_create_object (item_create_input_tag, &object)!=ITK_ok)PrintErrorStack();
														//if(AE_save_myself(wordLatestDat_t)!=ITK_ok)PrintErrorStack();
														if(AOM_save (object)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(object,TRUE)!=ITK_ok)PrintErrorStack();

														if(AOM_lock(DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(GRM_create_relation(DMLRevTg,object,NOPO_dml_Ref_Rel,NULLTAG,&NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(AOM_load(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(GRM_save_relation(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														printf("\n PO:Folder created. .....\n");fflush(stdout);
														if(AOM_save (object)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(object,TRUE)!=ITK_ok)PrintErrorStack();
														if(AOM_save (DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_unlock(DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
													}
													printf("\n PO:Going for PORev NOPO part addtion in folder.\n");fflush(stdout);
													jr=0;
													Refcount=0;
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n Refcount Value-:   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															//NOPORevFldFlag=0;
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPO:Reftype_name.: %s\n", Reftype_name);fflush(stdout);

															//if(tc_strcmp(Reftype_name,"Folder") ==0)
															if((tc_strcmp(Reftype_name,"T5_PSC_PORevMismatch_Parts") ==0)|| (tc_strcmp(Reftype_name,"P&SC PO Rev Mismatch Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:folder name.: %s\n", sNOPOFldr);fflush(stdout);
																//if(tc_strcmp(sNOPOFldr,"PORevMismatch_Parts") ==0)
																if((tc_strcmp(sNOPOFldr,"T5_PSC_PORevMismatch_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"P&SC PO Rev Mismatch Parts") ==0))
																{
																	//NOPORevFldFlag=1;
																	printf("\n PO REV NOPO part adition start..\n");fflush(stdout);
																	fprintf(fd,"\n SET OF PO REV MISMATCH FOR PARTS ARE BELOW. (Created/Modified after 1 Jan 2019.)\n[PO should be available on latest released revision,VCMS not allowed]  ");
																	piStringCount=0;
																	nopo=0;
																	if(EPM__parse_string(PSCPORevNOPOPartsSet,",",&piStringCount,&FStringList)!=ITK_ok)PrintErrorStack();
																	for (nopo=0;nopo<piStringCount ;nopo++ )
																	{
																		printf("\n PO:PSCPORevNOPOPartsSet Part:%s \n",FStringList[nopo]);fflush(stdout);
																		//if(ITEM_find_item (FStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
																		NOPOpart_tg= t5GetItemRevison(FStringList[nopo]);
																		if(NOPOpart_tg==NULLTAG)
																		{
																			printf("\n UnMat:P9:Object not found in Teamcenter.\n");fflush(stdout);
																		}
																		else
																		{
																			if(ITEM_list_all_revs (NOPOpart_tg, &PrtRev_count, &PrtRev_list)!=ITK_ok)   PrintErrorStack();
																			printf("\n P67:Total rev found: %d.", PrtRev_count);fflush(stdout);
																			if(PrtRev_count > 0)
																			{
																				pii=0;
																				for(pii=PrtRev_count-1;pii>=0;pii--)
																				{
																					PrtRelStus_cnt=0;
																					PrtRev_tg=PrtRev_list[pii];
																					if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtnoo)!=ITK_ok)   PrintErrorStack();
																					if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRevv)!=ITK_ok)   PrintErrorStack();
																					printf("\n D68:part no:%s revision:%s.",nPrtnoo,nPrtRevv);fflush(stdout);
																					if(WSOM_ask_release_status_list (PrtRev_tg, &PrtRelStus_cnt, &PrtRelStusTg)!=ITK_ok)PrintErrorStack();
																					printf("\n P69:PrtRelStus_cnt is:%d.", PrtRelStus_cnt);fflush(stdout);
																					if(PrtRelStus_cnt > 0)
																					{
																						n=0;
																						for(n=0;n<PrtRelStus_cnt;n++)
																						{
																							RelRevFlag=0;
																							//Checking PO availability on any revision.
																							if(AOM_ask_value_string (PrtRelStusTg[n], "object_name", &PrtRelStus)!=ITK_ok)PrintErrorStack();
																							printf("\n D69:PrtRelStus is:%s.", PrtRelStus);fflush(stdout);
																							if(tc_strstr(PrtRelStus,"T5_LcsErcRlzd")!=NULL)
																							{
																								RelRevFlag=1;
																								printf("\n PO:P2:going to insert part in folder.%s \n",FStringList[nopo]);fflush(stdout);
																								if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								//if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																								printf("\n PO:P2:part details in report..\n");fflush(stdout);
																								if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																								if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																								}
																								else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																								}
																								else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else
																								{
																									printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																								}
																								printf("\n PO:P2:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																								printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																								fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
																								break;
																							}
																						}
																						if (RelRevFlag>0)
																						{
																							break;
																						}
																					}
																				}
																			}
																		}
																	}
																	fprintf(fd,"\n ");
																	break;
																}
															}
														}
													}
													
													printf("\n PO:ERROR:PSCPORevNOPOPartsSet for DML\n %s\n",PSCPORevNOPOPartsSet);fflush(stdout);
													//EMH_store_error_s2(EMH_severity_error,ITK_err,"As per SAP informantion PO is not available on latest released revision for below mantioned parts.\n Please get the PO available in SAP or raise VCMS request.\n",PSCPORevNOPOPartsSet);	
													//return 1;
												}
												//PO revision mismatch parts
												if((PORevNOPOPartsSet==NULL)|| (tc_strcmp(PORevNOPOPartsSet,"")==0))
												{
													fprintf(fd,"\n SET OF PO REV MISMATCH FOR PARTS BEFORE 1 Jan 2019 ARE NOT FOUND. \n");
													printf("\n PO:PORevNOPOPartsSet is NULL.\n");fflush(stdout);
												}
												else
												{
													Errorflag=1;
													printf("\n PO REV MISMATCH Folder creation.\n");fflush(stdout);
													NOPORevFldFlag=0;
													//if(GRM_find_relation_type("IMAN_reference", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_find_relation_type("T5_CmHasNOPoDetails", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n NOPORev:Refcount Value :   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															NOPORevFldFlag=0;
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPORev:Reftype_name :: %s\n", Reftype_name);fflush(stdout);

															//if(tc_strcmp(Reftype_name,"Folder") ==0)
															if((tc_strcmp(Reftype_name,"T5_PORevMismatch_Parts") ==0)|| (tc_strcmp(Reftype_name,"PO Rev Mismatch Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n NOPORev:folder name :: %s\n", sNOPOFldr);fflush(stdout);
																//if(tc_strcmp(sNOPOFldr,"PORevMismatch_Parts") ==0)
																if((tc_strcmp(sNOPOFldr,"T5_PORevMismatch_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"PO Rev Mismatch Parts") ==0))
																{
																	NOPORevFldFlag=1;
																	printf("\n NOPORev Rev Folder already available Remove earlier parts in available.\n");fflush(stdout);
																	break;
																}
															}
														}
													}
													if(NOPORevFldFlag ==0)
													{
														printf("\n PORevMismatch_Parts Folder not available.\n");fflush(stdout);
														//NOPO Folder creation
														if(TCTYPE_find_type("T5_PORevMismatch_Parts", "Folder", &item_type_tag)!=ITK_ok)PrintErrorStack();
														if(TCTYPE_construct_create_input(item_type_tag, &item_create_input_tag)!=ITK_ok)PrintErrorStack();
														//if(AOM_set_value_string(item_create_input_tag, "object_name", "PORevMismatch_Parts")!=ITK_ok)PrintErrorStack();
														if(AOM_set_value_string(item_create_input_tag, "object_name", "PO Rev Mismatch Parts")!=ITK_ok)PrintErrorStack();
														if(TCTYPE_create_object (item_create_input_tag, &object)!=ITK_ok)PrintErrorStack();
														//if(AE_save_myself(wordLatestDat_t)!=ITK_ok)PrintErrorStack();
														if(AOM_save (object)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(object,TRUE)!=ITK_ok)PrintErrorStack();

														if(AOM_lock(DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(GRM_create_relation(DMLRevTg,object,NOPO_dml_Ref_Rel,NULLTAG,&NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(AOM_load(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(GRM_save_relation(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														printf("\n PO:Folder created. .....\n");fflush(stdout);
														if(AOM_save (object)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(object,TRUE)!=ITK_ok)PrintErrorStack();
														if(AOM_save (DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_unlock(DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
													}
													printf("\n PO:Going for PORev NOPO part addtion in folder.\n");fflush(stdout);
													jr=0;
													Refcount=0;
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n Refcount Value-:   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															//NOPORevFldFlag=0;
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPO:Reftype_name.: %s\n", Reftype_name);fflush(stdout);

															//if(tc_strcmp(Reftype_name,"Folder") ==0)
															if((tc_strcmp(Reftype_name,"T5_PORevMismatch_Parts") ==0)|| (tc_strcmp(Reftype_name,"PO Rev Mismatch Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n NOPO:folder name.: %s\n", sNOPOFldr);fflush(stdout);
																//if(tc_strcmp(sNOPOFldr,"PORevMismatch_Parts") ==0)
																if((tc_strcmp(sNOPOFldr,"T5_PORevMismatch_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"PO Rev Mismatch Parts") ==0))
																{
																	//NOPORevFldFlag=1;
																	printf("\n PO REV NOPO part adition start..\n");fflush(stdout);
																	fprintf(fd,"\n SET OF PO REV MISMATCH FOR PARTS ARE BELOW (Created before 1 Jan 2019).\n[PO should be available on latest released revision or VCMS allowed]  ");
																	piStringCount=0;
																	nopo=0;
																	if(EPM__parse_string(PORevNOPOPartsSet,",",&piStringCount,&FStringList)!=ITK_ok)PrintErrorStack();
																	for (nopo=0;nopo<piStringCount ;nopo++ )
																	{
																		printf("\n PO:PORevNOPOPartsSet Part:%s \n",FStringList[nopo]);fflush(stdout);
																		//if(ITEM_find_item (FStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
																		NOPOpart_tg= t5GetItemRevison(FStringList[nopo]);
																		if(NOPOpart_tg==NULLTAG)
																		{
																			printf("\n UnMat:P9:Object not found in Teamcenter.\n");fflush(stdout);
																		}
																		else
																		{
																			if(ITEM_list_all_revs (NOPOpart_tg, &PrtRev_count, &PrtRev_list)!=ITK_ok)   PrintErrorStack();
																			printf("\n P67:Total rev found: %d.", PrtRev_count);fflush(stdout);
																			if(PrtRev_count > 0)
																			{
																				pii=0;
																				for(pii=PrtRev_count-1;pii>=0;pii--)
																				{
																					PrtRelStus_cnt=0;
																					PrtRev_tg=PrtRev_list[pii];
																					if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtnoo)!=ITK_ok)   PrintErrorStack();
																					if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRevv)!=ITK_ok)   PrintErrorStack();
																					printf("\n E68:part no:%s revision:%s.",nPrtnoo,nPrtRevv);fflush(stdout);
																					if(WSOM_ask_release_status_list (PrtRev_tg, &PrtRelStus_cnt, &PrtRelStusTg)!=ITK_ok)PrintErrorStack();
																					printf("\n P69:PrtRelStus_cnt is:%d.", PrtRelStus_cnt);fflush(stdout);
																					if(PrtRelStus_cnt > 0)
																					{
																						n=0;
																						for(n=0;n<PrtRelStus_cnt;n++)
																						{
																							RelRevFlag=0;
																							//Checking PO availability on any revision.
																							if(AOM_ask_value_string (PrtRelStusTg[n], "object_name", &PrtRelStus)!=ITK_ok)PrintErrorStack();
																							printf("\n E69:PrtRelStus is:%s.", PrtRelStus);fflush(stdout);
																							if(tc_strstr(PrtRelStus,"T5_LcsErcRlzd")!=NULL)
																							{
																								RelRevFlag=1;
																								printf("\n PO:P2:going to insert part in folder.%s \n",FStringList[nopo]);fflush(stdout);
																								/*if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								//if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();*/
																								printf("\n PO:P2:part details in report..\n");fflush(stdout);
																								if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																								if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																								}
																								else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																								}
																								else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else
																								{
																									printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																								}
																								printf("\n PO:P2:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																								printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																								fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
																								break;
																							}
																						}
																						if(RelRevFlag>0)
																						{
																							break;
																						}
																					}
																				}
																			}
																		}
																	}
																	fprintf(fd,"\n ");
																	break;
																}
															}
														}
													}
													
													printf("\n PO:ERROR:PORevNOPOPartsSet for DML\n %s\n",PORevNOPOPartsSet);fflush(stdout);
													//EMH_store_error_s2(EMH_severity_error,ITK_err,"As per SAP informantion PO is not available on latest released revision for below mantioned parts.\n Please get the PO available in SAP or raise VCMS request.\n",PORevNOPOPartsSet);	
													//return 1;
												}

												//UNMATURATED Parts
												if ((UnMatPartSet==NULL)||(tc_strcmp(UnMatPartSet,"")==0))
												{
													fprintf(fd,"\n SET OF UNMATURATED DR-STATUS PARTS ARE NOT FOUND. \n");
													printf("\n UnMat:UnMatPartSet is NULL.\n");fflush(stdout);
												}
												else
												{
													Errorflag=1;
													//NOPO Folder creation
													printf("\n UnMat Folder creation.\n");fflush(stdout);
													NOPOFldFlag=0;
													//if(GRM_find_relation_type("IMAN_reference", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_find_relation_type("T5_CmHasNOPoDetails", &NOPO_dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n Refcount Value :   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															NOPOFldFlag=0;
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n UnMat:Reftype_name :: %s\n", Reftype_name);fflush(stdout);

															if(tc_strcmp(Reftype_name,"Folder") ==0)
															//if((tc_strcmp(Reftype_name,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"New NOPO Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n UnMat:folder name :: %s\n", sNOPOFldr);fflush(stdout);
																if(tc_strcmp(sNOPOFldr,"UnMaturated DR Status Parts") ==0)
																//if((tc_strcmp(sNOPOFldr,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"New NOPO Parts") ==0))
																{
																	NOPOFldFlag=1;
																	printf("\n UnMat Folder already available.\n");fflush(stdout);
																	break;
																}
															}
														}
													}
													if(NOPOFldFlag ==0)
													{
														printf("\n UnMat Folder not available.\n");fflush(stdout);
														//NOPO Folder creation
														if(TCTYPE_find_type("Folder", "Folder", &item_type_tag)!=ITK_ok)PrintErrorStack();
														if(TCTYPE_construct_create_input(item_type_tag, &item_create_input_tag)!=ITK_ok)PrintErrorStack();
														if(AOM_set_value_string(item_create_input_tag, "object_name", "UnMaturated DR Status Parts")!=ITK_ok)PrintErrorStack();
														if(TCTYPE_create_object (item_create_input_tag, &NOPOobject)!=ITK_ok)PrintErrorStack();
														//if(AE_save_myself(wordLatestDat_t)!=ITK_ok)PrintErrorStack();
														if(AOM_save (NOPOobject)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(NOPOobject,TRUE)!=ITK_ok)PrintErrorStack();

														if(AOM_lock( DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(GRM_create_relation(DMLRevTg,NOPOobject,NOPO_dml_Ref_Rel,NULLTAG,&NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(AOM_load(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														if(GRM_save_relation(NOPOdml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
														printf("\n UnMat:Folder created. .....\n");fflush(stdout);
														if(AOM_save (NOPOobject)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(NOPOobject,TRUE)!=ITK_ok)PrintErrorStack();
														if(AOM_save (DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_unlock(DMLRevTg)!=ITK_ok)PrintErrorStack();
														if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
													}
													//Going to add NOPO parts
													jr=0;
													Refcount=0;
													printf("\n Going to add new NOPO parts in folder.\n");fflush(stdout);
													if(GRM_list_secondary_objects_only(DMLRevTg,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag)!=ITK_ok)PrintErrorStack();
													printf("\n UnMat:.Refcount Value :   %d\n",Refcount);fflush(stdout);
													if(Refcount>0)
													{
														for (jr=0;jr<Refcount;jr++ )
														{
															if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
															if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
															printf("\n NOPO:.Reftype_name:%s\n", Reftype_name);fflush(stdout);

															if(tc_strcmp(Reftype_name,"Folder") ==0)
															//if((tc_strcmp(Reftype_name,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"New NOPO Parts") ==0))
															{
																if(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr)!=ITK_ok)PrintErrorStack();
																printf("\n UnMat:.folder name: %s\n", sNOPOFldr);fflush(stdout);
																if(tc_strcmp(sNOPOFldr,"UnMaturated DR Status Parts") ==0)
																//if((tc_strcmp(sNOPOFldr,"T5_New_NOPO_Parts") ==0)|| (tc_strcmp(sNOPOFldr,"New NOPO Parts") ==0))
																{
																	//if(GRM_find_relation_type("Contents", &Fold_Cont_Rel)!=ITK_ok)PrintErrorStack();
																	printf("\n UnMat addtition start.\n");fflush(stdout);
																	fprintf(fd,"\n SET OF UNMATURATED DR-STATUS PARTS ARE BELOW.\n[Parts should be updated to higher DR-status with Gate Maturation DML.]");
																	nopo=0;
																	piStringCount=0;
																	if(EPM__parse_string(UnMatPartSet,",",&piStringCount,&GStringList)!=ITK_ok)PrintErrorStack();
																	for (nopo=0;nopo<piStringCount ;nopo++ )
																	{
																		printf("\n UnMat:UnMatPartSet Part:%s \n",GStringList[nopo]);fflush(stdout);
																		//if(ITEM_find_item (GStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
																		NOPOpart_tg= t5GetItemRevison(GStringList[nopo]);
																		if(NOPOpart_tg==NULLTAG)
																		{
																			printf("\n UnMat:P9:Object not found in Teamcenter.\n");fflush(stdout);
																		}
																		else
																		{
																			if(ITEM_list_all_revs (NOPOpart_tg, &PrtRev_count, &PrtRev_list)!=ITK_ok)   PrintErrorStack();
																			printf("\n P67:Total rev found: %d.", PrtRev_count);fflush(stdout);
																			if(PrtRev_count > 0)
																			{
																				pii=0;
																				for(pii=PrtRev_count-1;pii>=0;pii--)
																				{
																					PrtRelStus_cnt=0;
																					PrtRev_tg=PrtRev_list[pii];
																					if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtnoo)!=ITK_ok)   PrintErrorStack();
																					if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRevv)!=ITK_ok)   PrintErrorStack();
																					printf("\n F68:part no:%s revision:%s.",nPrtnoo,nPrtRevv);fflush(stdout);
																					if(WSOM_ask_release_status_list (PrtRev_tg, &PrtRelStus_cnt, &PrtRelStusTg)!=ITK_ok)PrintErrorStack();
																					printf("\n P69:PrtRelStus_cnt is:%d.", PrtRelStus_cnt);fflush(stdout);
																					if(PrtRelStus_cnt > 0)
																					{
																						n=0;
																						RelRevFlag=0;
																						for(n=0;n<PrtRelStus_cnt;n++)
																						{
																							RelRevFlag=0;
																							//Checking PO availability on any revision.
																							if(AOM_ask_value_string (PrtRelStusTg[n], "object_name", &PrtRelStus)!=ITK_ok)PrintErrorStack();
																							printf("\n F69:PrtRelStus:%s.",PrtRelStus);fflush(stdout);
																							if(tc_strstr(PrtRelStus,"T5_LcsErcRlzd")!=NULL)
																							{
																								RelRevFlag=1;
																								printf("\n UnMat:P9:going to insert part in folder.%s \n",GStringList[nopo]);fflush(stdout);
																								if(AOM_lock( RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								//if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(FL_insert(RefDoc_tag[jr],PrtRev_tg,nopo)!=ITK_ok)PrintErrorStack();
																								if(AOM_save (RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_unlock(RefDoc_tag[jr])!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(RefDoc_tag[jr],TRUE)!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(PrtRev_tg,TRUE)!=ITK_ok)PrintErrorStack();
																								if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
																								printf("\n UnMat:P9:part details in report..\n");fflush(stdout);
																								if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																								if(AOM_ask_value_string(PrtRev_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																								if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																								}
																								else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																								}
																								else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																								{
																									if(AOM_ask_value_string(PrtRev_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																									printf("\n UnMat:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																								}
																								else
																								{
																									printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																								}
																								printf("\n UnMat:P1:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																								printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																								fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
																								break;
																							}
																						}
																						if(RelRevFlag>0)
																						{
																							break;
																						}
																					}
																				}
																			}
																		}
																	}
																	fprintf(fd,"\n ");
																	break;
																}
															}
														}
													}
													printf("\n PO:ERROR:UnMatPartSet:\n%s\n",UnMatPartSet);fflush(stdout);
												}
												//CHECK BYPASS PARTS
												if((BypassPartSet==NULL)|| (tc_strcmp(BypassPartSet,"")==0))
												{
													fprintf(fd,"\n SET OF NO PO CHECK BYPASS PARTS NOT FOUND. \n");
													printf("\n PO:BypassPartSet is NULL.\n");fflush(stdout);
												}
												else
												{
													fprintf(fd,"\n SET OF NO PO CHECK BYPASS PARTS ARE BELOW. ");
													nopo=0;
													piStringCount=0;
													if(EPM__parse_string(BypassPartSet,",",&piStringCount,&DStringList)!=ITK_ok)PrintErrorStack();
													for (nopo=0;nopo<piStringCount ;nopo++ )
													{
														printf("\n PO:BypassPartSet Part:%s \n",DStringList[nopo]);fflush(stdout);
														//if(ITEM_find_item (DStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
														NOPOpart_tg= t5GetItemRevison(DStringList[nopo]);
														if(NOPOpart_tg==NULLTAG)
														{
															printf("\n PO1:NOPOpart_tg is NULL.\n");fflush(stdout);
														}
														else
														{
															printf("\n PO1:NOPOpart_tg is found.\n");fflush(stdout);
															if(ITEM_ask_latest_rev (NOPOpart_tg, &NOPOprt_tg)!=ITK_ok)PrintErrorStack();
															if(NOPOprt_tg==NULLTAG)
															{
																printf("\n PO:P4:Object not found in Teamcenter.\n");fflush(stdout);
															}
															else
															{
																if(AOM_ask_value_string(NOPOprt_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																}
																else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																}
																else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else
																{
																	printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																}
																printf("\n PO:P4:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
															}
														}
													}
													fprintf(fd,"\n ");
													printf("\n PO:ERROR:BypassPartSet for DML\n %s\n",BypassPartSet);fflush(stdout);
												}

												//CHECK BYPASS PARTS
												if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
												{
													fprintf(fd,"\n SET OF PO AVAILABLE FOR PARTS NOT FOUND. \n");
													printf("\n PO:POAvailPartSet is NULL.\n");fflush(stdout);
												}
												else
												{
													fprintf(fd,"\n SET OF PO AVAILABLE FOR PARTS ARE BELOW. ");
													nopo=0;
													piStringCount=0;
													if(EPM__parse_string(POAvailPartSet,",",&piStringCount,&CStringList)!=ITK_ok)PrintErrorStack();
													for (nopo=0;nopo<piStringCount ;nopo++ )
													{
														printf("\n PO:POAvailPartSet Part:%s \n",CStringList[nopo]);fflush(stdout);
														//if(ITEM_find_item (CStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
														NOPOpart_tg= t5GetItemRevison(CStringList[nopo]);
														if(NOPOpart_tg==NULLTAG)
														{
															printf("\n PO3:NOPOpart_tg is NULL.\n");fflush(stdout);
														}
														else
														{
															printf("\n PO2:NOPOpart_tg is found.\n");fflush(stdout);
															if(ITEM_ask_latest_rev (NOPOpart_tg, &NOPOprt_tg)!=ITK_ok)PrintErrorStack();
															if(NOPOprt_tg==NULLTAG)
															{
																printf("\n PO:P5:Object not found in Teamcenter.\n");fflush(stdout);
															}
															else
															{
																if(AOM_ask_value_string(NOPOprt_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																}
																else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																}
																else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else
																{
																	printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																}
																printf("\n PO:P5:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
															}
														}
													}
													fprintf(fd,"\n ");
													printf("\n PO:ERROR:POAvailPartSet for DML\n %s\n",POAvailPartSet);fflush(stdout);
												}
												//CHECK BYPASS PARTS
												if((PORevAvailPartSet==NULL)|| (tc_strcmp(PORevAvailPartSet,"")==0))
												{
													fprintf(fd,"\n SET OF PO REV AVAILABLE FOR PARTS ARE NOT FOUND. \n");
													printf("\n PO:PORevAvailPartSet is NULL.\n");fflush(stdout);
												}
												else
												{
													fprintf(fd,"\n SET OF PO REV AVAILABLE FOR PARTS ARE BELOW. ");
													nopo=0;
													piStringCount=0;
													if(EPM__parse_string(PORevAvailPartSet,",",&piStringCount,&BStringList)!=ITK_ok)PrintErrorStack();
													for (nopo=0;nopo<piStringCount ;nopo++ )
													{
														printf("\n PO:PORevAvailPartSet Part:%s \n",BStringList[nopo]);fflush(stdout);
														//if(ITEM_find_item (BStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
														NOPOpart_tg= t5GetItemRevison(BStringList[nopo]);
														if(NOPOpart_tg==NULLTAG)
														{
															printf("\n PO4:NOPOpart_tg is NULL.\n");fflush(stdout);
														}
														else
														{
															printf("\n PO5:NOPOpart_tg is found.\n");fflush(stdout);
															if(ITEM_ask_latest_rev (NOPOpart_tg, &NOPOprt_tg)!=ITK_ok)PrintErrorStack();
															if(NOPOprt_tg==NULLTAG)
															{
																printf("\n PO:P6:Object not found in Teamcenter.\n");fflush(stdout);
															}
															else
															{
																if(AOM_ask_value_string(NOPOprt_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																}
																else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																}
																else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else
																{
																	printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																}
																printf("\n PO:P6:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
															}
														}
													}
													fprintf(fd,"\n ");
													printf("\n PO:ERROR:PORevAvailPartSet for DML\n %s\n",PORevAvailPartSet);fflush(stdout);
												}
												//SKIPPED PARTS
												if((SkippedPartSet==NULL)|| (tc_strcmp(SkippedPartSet,"")==0))
												{
													fprintf(fd,"\n SET OF PO CHECK SKIPPED FOR PARTS NOT FOUND. \n");
													printf("\n PO:SkippedPartSet is NULL.\n");fflush(stdout);
												}
												else
												{
													fprintf(fd,"\n SET OF PO CHECK SKIPPED FOR PARTS ARE BELOW. ");
													nopo=0;
													piStringCount=0;
													if(EPM__parse_string(SkippedPartSet,",",&piStringCount,&AStringList)!=ITK_ok)PrintErrorStack();
													for (nopo=0;nopo<piStringCount ;nopo++ )
													{
														printf("\n PO:SkippedPartSet Part:%s \n",AStringList[nopo]);fflush(stdout);
														//if(ITEM_find_item (AStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
														NOPOpart_tg= t5GetItemRevison(AStringList[nopo]);
														if(NOPOpart_tg==NULLTAG)
														{
															printf("\n PO4:NOPOpart_tg is NULL.\n");fflush(stdout);
														}
														else
														{
															printf("\n PO5:NOPOpart_tg is found.\n");fflush(stdout);
															if(ITEM_ask_latest_rev (NOPOpart_tg, &NOPOprt_tg)!=ITK_ok)PrintErrorStack();
															if(NOPOprt_tg==NULLTAG)
															{
																printf("\n PO:P7:Object not found in Teamcenter.\n");fflush(stdout);
															}
															else
															{
																if(AOM_ask_value_string(NOPOprt_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
																if(AOM_ask_value_string(NOPOprt_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
																if (tc_strcmp(sDml_TP,"CARPLANT")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_CarMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant1:CARPLANT:1100:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_JsrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant8:JAMSHEDPUR:2001:sPartCS: %s",sPartCS);	fflush(stdout);	;	
																}
																else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_LkoMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant9:LUCKNOW:3001:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_JdlMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:DRIVELINES:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_DwdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant6:DHARWAD:1500:sPartCS: %s",sPartCS);	fflush(stdout);		
																}
																else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PnrMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant4:PANTNAGAR:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_AhdMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant3:AHMEDABAD:7501:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"PCVBU")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PunMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant2:PCVBU:1001:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else if (tc_strcmp(sDml_TP,"PUVBU")==0)
																{
																	if(AOM_ask_value_string(NOPOprt_tg,"t5_PunUVMakeBuyIndicator",&sPartCS)!=ITK_ok)   PrintErrorStack();
																	printf("\n PO:Plant11:PUVBU:1140:sPartCS: %s",sPartCS);	fflush(stdout);	
																}
																else
																{
																	printf("\n PO:Other Target Plant:%s  \n",sDml_TP);fflush(stdout);
																}
																printf("\n PO:P7:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
																printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
																fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
															}
														}
													}
													fprintf(fd,"\n ");
													printf("\n PO:ERROR:SkippedPartSet for DML\n %s\n",SkippedPartSet);fflush(stdout);
												}
											}
											
											fclose(fd);

											printf("\n going for dataset\n");fflush(stdout);
											if(GRM_find_relation_type("IMAN_reference", &dml_Ref_Rel)!=ITK_ok)PrintErrorStack();
											TC_write_syslog("Doc file created successfully\n");
											if(AE_find_datasettype2("MSExcel",&msWordType)!=ITK_ok)PrintErrorStack();
											if(msWordType == NULLTAG)
											{
												printf("\n Could not find Dataset\n");fflush(stdout);
												TC_write_syslog("Could not find Dataset Type Text. Please check data model\n");
												//continue;
											}

											if(AE_create_dataset_with_id(msWordType,sDMLno,"NOPO Report for DML","","",&wordDataset)!=ITK_ok)PrintErrorStack();
											if(AE_save_myself(wordDataset));
											if(GRM_create_relation(DMLRevTg,wordDataset,dml_Ref_Rel,NULLTAG,&dml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
											if(AOM_load(dml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
											if(GRM_save_relation(dml_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
											if(AE_ask_dataset_latest_rev(wordDataset,&wordLatestDat_t));
											if(AOM_refresh(wordLatestDat_t,TRUE)!=ITK_ok)PrintErrorStack();
											//if(AE_import_named_ref(wordLatestDat_t,"word",FileDown,NULL,SS_BINARY)!=ITK_ok)PrintErrorStack();
											if(AE_import_named_ref(wordLatestDat_t,"excel",FileDown,NULL,SS_BINARY)!=ITK_ok)PrintErrorStack();
											//if(AE_import_named_ref(wordLatestDat_t,"Text",FileDown,NULL,SS_TEXT)!=ITK_ok)PrintErrorStack();
											if(AE_save_myself(wordLatestDat_t)!=ITK_ok)PrintErrorStack();
											//if(AOM_unlock(DMLRevTg)!=ITK_ok)PrintErrorStack();
											if(AOM_refresh(DMLRevTg,TRUE)!=ITK_ok)PrintErrorStack();
											remove(FileDown);
											if(FileDown) MEM_free(FileDown);

											if (Errorflag >0)
											{
												printf("\n PO:final ERROR FOR NOPO..\n");fflush(stdout);
												//return 1;
											}
										}
									}
								}
							}
						//}
					}
					else if(tc_strcmp(ObjTyp,"Design Revision")==0 || tc_strcmp(ObjTyp,"T5_ClrPartRevision")==0)
					{
						FirstCheckOK = 1;
						if(POM_get_user_id (&usernam)!=ITK_ok)PrintErrorStack();
						printf("\nPO Check For Part :Session login User1 for : %s\n",usernam); fflush(stdout);
						
						ITK_CALL(AOM_UIF_ask_value(Lat_Item_Tag,"item_id",&sPartNumber));
						//ITK_CALL(AOM_ask_value_string(Lat_Item_Tag,"object_desc",&sDescription));
						ITK_CALL(AOM_UIF_ask_value(Lat_Item_Tag,"item_revision_id",&cRevSeq));

						newPatRev = strtok ( cRevSeq, ";" );
						newPatSeq = strtok ( NULL, ";" );

						printf("\nPO Check For Part : %s,%s,%s\n",sPartNumber,newPatRev,newPatSeq); fflush(stdout);

						if (FirstCheckOK >0)
						{
							tc_strcpy(FileDown,"/tmp/");
							tc_strcat(FileDown,sPartNumber);
							tc_strcat(FileDown,"_NOPO_Report.xls");
							TC_write_syslog("Main File = %s\n",FileDown);
							
							fd=fopen(FileDown,"w");
							if(fd==NULL)
							{
								printf("\nPO:Error in opening the file \n");fflush(stdout);
							}
							fprintf(fd,"\n\t -- Part NOPO Report --\n\n");
							fprintf(fd,"\nReport DETAILS:");

							fprintf(fd,"\nPart Number\t%s %s %s",sPartNumber,newPatRev,newPatSeq);
							fprintf(fd,"\nTarget Plant\t%s",tplant);
							fprintf(fd,"\n TC PART NO.\t REVISION \t PART CS \t PART TYPE \t PART DR-STATUS \t PROJECT \t DESIGN GRP \t DRW_INDICATOR  \t COLOUR INDICATOR \t PART DESCRIPTION \t \n");
						}

						mbFlag = tm_get_plantmakebuy(tplant);
						printf("\n PO:Input Target plant : %s MakeBuy	: %s",tplant,sPlantMakeBuy);fflush(stdout);

						/*MRDMLFun=0;
						MRDMLFun = tm_POCheckStopAtFFF(Lat_Item_Tag,tplant,sEESWPrtType);
						printf("\n PO:MRDMLFun: %d",MRDMLFun);fflush(stdout);
						if (MRDMLFun >0)
						{
							//Expanssion start
							printf("\n PO3: BOM Window setting.");fflush(stdout);
							if(BOM_create_window(&tagBOMwindow)!=ITK_ok)   PrintErrorStack();
							if(CFM_find("ERC release and above", &tagRule)!=ITK_ok)   PrintErrorStack(); 
							if(BOM_set_window_config_rule( tagBOMwindow, tagRule )!=ITK_ok)   PrintErrorStack();
							//0-qty skip
							if(PIE_find_closure_rules("TMLBOMExpandByQty",PIE_TEAMCENTER, &rulefounddd, &POclosureruleee )!=ITK_ok)   PrintErrorStack();
							printf ("PO3:rulefounddd count %d \n",rulefounddd);fflush(stdout);
							if (rulefounddd > 0)
							{
								POclose_taggg = POclosureruleee[0];
								printf ("PO3: POclose_taggg closure rule found \n");fflush(stdout);
							}
							if(BOM_window_set_closure_rule( tagBOMwindow,POclose_taggg, 0, POrulenameee,POrulevalueee )!=ITK_ok)   PrintErrorStack();
							//0-qty skip

							if(BOM_set_window_top_line(tagBOMwindow, NULLTAG, Lat_Item_Tag, NULLTAG, &tagBOMline)!=ITK_ok)   PrintErrorStack();
							if(BOM_line_ask_child_lines(tagBOMline, &childCountTopBomLine, &tagTopBLchildLine)!=ITK_ok)   PrintErrorStack();
							printf("\n PO3:FLP:No of child objects are n : %d\n",childCountTopBomLine);fflush(stdout);
							for(iCount =0 ; iCount < childCountTopBomLine; iCount++)
							{
								printf("\n PO:MR:No of child taken : %d Part_no:%s\n",iCount,Part_no);fflush(stdout);
								
								FunOutflag=0;
								FunOutflag= tm_POCheckStopAtFBOMline(tagTopBLchildLine[iCount],tplant,sEESWPrtType);
								printf("\n PO:P1:FunOutflag: %s :%d",Part_no,FunOutflag);fflush(stdout);
							}
						}*/
						//MOhan
						printf("\n T8:Lat_Item_Tag is found...\n");fflush(stdout);
						if(AOM_ask_value_string(Lat_Item_Tag,"item_id",&Part_nobrr)!=ITK_ok)   PrintErrorStack();
						if(AOM_ask_value_string(Lat_Item_Tag,"t5_PartType",&P_Type)!=ITK_ok)   PrintErrorStack();
						if(AOM_UIF_ask_value(Lat_Item_Tag, "item_revision_id", &rev_iddd)!=ITK_ok)PrintErrorStack();
						if(AOM_ask_value_string(Lat_Item_Tag,"t5_PartStatus",&PrtDRstt)!=ITK_ok)PrintErrorStack();
						printf("\n T9: Parent part: %s P_Type:%s rev_iddd:%s PrtDRstt:%s",Part_nobrr,P_Type,rev_iddd,PrtDRstt);	fflush(stdout);
						if((tc_strcmp(P_Type,"CCVC")==0)||(tc_strcmp(P_Type,"VC")==0)||(tc_strcmp(P_Type,"Vehicle Combination")==0)||(tc_strcmp(P_Type,"Configurable VC")==0)||(tc_strcmp(P_Type,"V")==0)||(tc_strcmp(P_Type,"Vehicle")==0))
						{
							//Consiere only TO Gate child parts.
							tc_strcpy(PARTTYPEs,"");
							tc_strcpy(PARTTYPEs,"CCVC");	
							printf("\n T4: PARTTYPEs:[%s]\n", PARTTYPEs);fflush(stdout);
						}
						else
						{
							tc_strcpy(PARTTYPEs,"");
							tc_strcpy(PARTTYPEs,"MODULE");	
							printf("\n T5: PARTTYPEs:[%s]\n", PARTTYPEs);fflush(stdout);
						}
						
						if((tc_strcmp(P_Type,"CCVC")==0)||(tc_strcmp(P_Type,"VC")==0)||(tc_strcmp(P_Type,"Vehicle Combination")==0)||(tc_strcmp(P_Type,"Configurable VC")==0))
						{
							//expand CCV to snapshot.
							if(GRM_find_relation_type("T5_PartHasSnapShot", &CCVtoSnapShot_Rel_Type)!=ITK_ok)PrintErrorStack();
							if (CCVtoSnapShot_Rel_Type!=NULLTAG)
							{
								SnpShtcount=0;
								if(GRM_list_secondary_objects_only(Lat_Item_Tag,CCVtoSnapShot_Rel_Type,&SnpShtcount,&SnpShtTags)!=ITK_ok)PrintErrorStack();
								printf("\n C1:SnpShtcount: %d",SnpShtcount);fflush(stdout);
								if(SnpShtcount > 0)
								{
									SnpShtTag = SnpShtTags[0];
									if(TCTYPE_ask_object_type(SnpShtTag,&SnpObjTypeTag)!=ITK_ok)PrintErrorStack();
									if(TCTYPE_ask_name(SnpObjTypeTag,SnpObjTypeNm)!=ITK_ok)PrintErrorStack();
									printf("\n C2:SnpObjTypeNm:: [%s].",SnpObjTypeNm); fflush(stdout);

									if(strcmp(SnpObjTypeNm,"Snapshot")==0)
									{
										if(AOM_ask_value_string(SnpShtTag,"object_name",&SnpShtNo)!=ITK_ok)   PrintErrorStack();
											printf("\n C3:SnapshotName:%s ......",SnpShtNo);fflush(stdout);

										if(AOM_ask_value_tag(SnpShtTag,"topLine",&Modl_rev_tags)!=ITK_ok)PrintErrorStack();

										printf("\n C4:BOM_create_window_from_snapshot...");fflush(stdout);
										if(BOM_create_window_from_snapshot(SnpShtTag,&Snpwindow)!=ITK_ok)PrintErrorStack();
										//printf("\n CCV:BOM_set_window_top_line...\n");fflush(stdout);
										if(BOM_set_window_top_line(Snpwindow, NULLTAG, Modl_rev_tags, NULLTAG, &SnpBOMline)!=ITK_ok)   PrintErrorStack();
										//printf("\n CCV:BOM_line_ask_child_lines...\n");fflush(stdout);
										if(BOM_line_ask_child_lines(SnpBOMline, &childCountSnpBOMline, &SnpBOMlineTags)!=ITK_ok)   PrintErrorStack();
										printf("\n C5:ArcNodCount:%d: ",childCountSnpBOMline);fflush(stdout);
										for(mdl =0 ; mdl < childCountSnpBOMline; mdl++)
										{
											SnpBOMlineTag = SnpBOMlineTags[mdl];
											if(AOM_ask_value_string(SnpBOMlineTags[mdl],"bl_item_item_id",&ArcMdlNo)!=ITK_ok)   PrintErrorStack();
											printf("\n C6:Arc_Module:[%s] :total Arc_module:[%d] Taken Arc_module:[%d]",ArcMdlNo,childCountSnpBOMline,mdl);fflush(stdout);
											ArcMdlNo_tag= t5GetItemRevison(ArcMdlNo);
											if(ArcMdlNo_tag==NULLTAG)
											{
												printf("\n C7:ArcMdlNo_tag is NULL.\n");fflush(stdout);
												//return 0;
											}
											else
											{
												//printf("\n CCV:ArcMdlNo_tag found.\n");fflush(stdout);
												if(ITEM_ask_latest_rev (ArcMdlNo_tag, &ArcMdlNo_tg)!=ITK_ok)PrintErrorStack();
												if(ArcMdlNo_tg==NULLTAG)
												{
													printf("\n C8:Object not found ArcMdlNo_tg...!!\n");fflush(stdout);
													//return 0;
												}
												else
												{
													//Expanssion start
													//printf("\n CCV: BOM Window setting.");fflush(stdout);
													if(BOM_create_window(&MdlBOMwindow)!=ITK_ok)   PrintErrorStack();
													if(CFM_find("ERC release and above", &MdltagRule)!=ITK_ok)   PrintErrorStack(); 
													if(BOM_set_window_config_rule( MdlBOMwindow, MdltagRule )!=ITK_ok)   PrintErrorStack();
													//0-qty skip
													//if(PIE_find_closure_rules("BOMLineSkipZeroQtyMaskUnconfig",PIE_TEAMCENTER, &MdlCount, &POclosRul )!=ITK_ok)   PrintErrorStack();
													//if(PIE_find_closure_rules("BOMLineskipforunconfigured",PIE_TEAMCENTER, &MdlCount, &POclosRul )!=ITK_ok)   PrintErrorStack();
													if(PIE_find_closure_rules("BOMLineSkipZeroQtyMaskUnconfig",PIE_TEAMCENTER, &MdlCount, &POclosRul )!=ITK_ok)   PrintErrorStack();
													//printf ("CCV:Closure Rule count %d \n",MdlCount);fflush(stdout);
													if (MdlCount > 0)
													{
														POcloseRul_tag = POclosRul[0];
														//printf ("CCV: POcloseRul_tag closure rule found \n");fflush(stdout);
													}
													if(BOM_window_set_closure_rule(MdlBOMwindow,POcloseRul_tag, 0, POrulNm,Mdlrulevl )!=ITK_ok)   PrintErrorStack();
													if(BOM_set_window_top_line(MdlBOMwindow, NULLTAG, ArcMdlNo_tg, NULLTAG, &MdlBOMline)!=ITK_ok)   PrintErrorStack();
													//printf ("CCV:BBBBBBBBBB \n");fflush(stdout);
													MdlchildCount=0;
													if(BOM_line_ask_child_lines(MdlBOMline, &MdlchildCount, &MdlTopBLchildLine)!=ITK_ok)   PrintErrorStack();
													printf("\n C9:Arc_Module:[%s] :total Arc_module:[%d] Taken Arc_module:[%d] Total Modules:[%d]",ArcMdlNo,childCountSnpBOMline,mdl,MdlchildCount);fflush(stdout);
													for(mdll =0 ; mdll < MdlchildCount; mdll++)
													{
														if(AOM_ask_value_string(MdlTopBLchildLine[mdll],"bl_item_item_id",&MdlNo)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(MdlTopBLchildLine[mdll],"bl_Design Revision_t5_PartStatus",&Mdl_DR)!=ITK_ok)   PrintErrorStack();
														printf("\n C10:Arc_Module:[%s] Total Arc_module:[%d] Taken Arc_module:[%d] Total Modules:[%d] Taken Module:[%s] Module no:[%s] Module DR:[%s]",ArcMdlNo,childCountSnpBOMline,mdl,MdlchildCount,mdll,MdlNo,Mdl_DR);fflush(stdout);
														//AAAAAAAAAAAAAA
														MdlLstRev_tag= t5GetItemRevisonForRelRev(MdlNo,PARTTYPEs);
														if(MdlLstRev_tag==NULLTAG)
														{
															printf("\n C10:MdlLstRev_tag is NULL.\n");fflush(stdout);
															//return 0;
														}
														else
														{
															printf("\n C11:MdlLstRev_tag found.\n");fflush(stdout);
															if(AOM_ask_value_string(MdlLstRev_tag,"item_id",&Part_nobrrr)!=ITK_ok)   PrintErrorStack();
															if(AOM_ask_value_string(MdlLstRev_tag,"t5_PartType",&P_Typeer)!=ITK_ok)   PrintErrorStack();
															if(AOM_UIF_ask_value(MdlLstRev_tag, "item_revision_id", &rev_idddr)!=ITK_ok)PrintErrorStack();
															if(AOM_ask_value_string(MdlLstRev_tag,"t5_PartStatus",&PrtDRsttr)!=ITK_ok)PrintErrorStack();
															printf("\n C12:Call Module:[%s] Module type:[%s] Module Rev:[%s] Module DR:[%s]",Part_nobrrr,P_Typeer,rev_idddr,PrtDRsttr);	fflush(stdout);	
															GMDMLFun=0;
															GMDMLFun = tm_POCheckStopAtFFF(MdlLstRev_tag,tplant,sEESWPrtType);
															printf("\n C13:fun done for Module:[%s] GMDMLFun..: [%d]",Part_nobrrr,GMDMLFun);fflush(stdout);
															if (GMDMLFun >0)
															{
																//Expanssion start
																printf("\n C14: BOM Window setting.");fflush(stdout);
																if(BOM_create_window(&tagBOMwindow)!=ITK_ok)   PrintErrorStack();
																if(CFM_find("ERC release and above", &tagRule)!=ITK_ok)   PrintErrorStack(); 
																if(BOM_set_window_config_rule( tagBOMwindow, tagRule )!=ITK_ok)   PrintErrorStack();
																//0-qty skip
																if(PIE_find_closure_rules("TMLBOMExpandByQty",PIE_TEAMCENTER, &rulefoundXO, &POclosureruleXO )!=ITK_ok)   PrintErrorStack();
																//printf ("PO3:rulefounddd count %d \n",rulefoundXO);fflush(stdout);
																if (rulefoundXO > 0)
																{
																	POclose_tagXO = POclosureruleXO[0];
																	//printf ("PO3: POclose_taggg closure rule found \n");fflush(stdout);
																}
																if(BOM_window_set_closure_rule( tagBOMwindow,POclose_tagXO, 0, POrulenameXO,POrulevalueXO )!=ITK_ok)   PrintErrorStack();
																//0-qty skip

																if(BOM_set_window_top_line(tagBOMwindow, NULLTAG, MdlLstRev_tag, NULLTAG, &tagBOMline)!=ITK_ok)   PrintErrorStack();
																if(BOM_line_ask_child_lines(tagBOMline, &childCountTopBomLine, &tagTopBLchildLine)!=ITK_ok)   PrintErrorStack();
																printf("\n C15:Exp parent Module:%s with revision:%s having no of child Parts: %d\n",Part_nobrrr,rev_idddr,childCountTopBomLine);fflush(stdout);
																for(iCount =0 ; iCount < childCountTopBomLine; iCount++)
																{
																	if(AOM_ask_value_string(tagTopBLchildLine[iCount],"bl_item_item_id",&MoldChild)!=ITK_ok)   PrintErrorStack();
																	printf("\n C16:Exp start for parent Module:[%s] taken child part :[%d] child Part: [%s] \n",Part_nobrrr,iCount,MoldChild);fflush(stdout);
																	FunOutflag=0;
																	FunOutflag= tm_POCheckStopAtFBOMline(tagTopBLchildLine[iCount],tplant,sEESWPrtType,Part_nobrrr,rev_idddr);
																	printf("\n C17:Exp done for parent Module:[%s] taken child part :[%d] child Part: [%s] FunOutflag:[%d]\n",Part_nobrrr,iCount,MoldChild,FunOutflag);fflush(stdout);
																}
															}
														}
													}
												}
											}
										}
									}
								}
								else
								{
									printf("\n C16:ERROR:Snapshot not available:%s:\n",Part_nobr);fflush(stdout);	
									if((NwNOPOPartsSet==NULL)|| (tc_strcmp(NwNOPOPartsSet,"")==0))
									{
										tc_strcpy (NwNOPOPartsSet,"" );
										tc_strcat (NwNOPOPartsSet,Part_nobr);
										printf("\n C18:Snapshot not available:NwNOPOPartsSet. %s",Part_nobr);	fflush(stdout);	
									}
									else
									{
										if(strstr(NwNOPOPartsSet,Part_nobr)==NULL)
										{
											tc_strcat (NwNOPOPartsSet,"," );
											tc_strcat (NwNOPOPartsSet,Part_nobr);
											printf("\n C19:Snapshot not available:NwNOPOPartsSet.. %s",Part_nobr);	fflush(stdout);
										}
									}
								}
							}
						}
						else
						{
							GMDMLFun=0;
							GMDMLFun = tm_POCheckStopAtFFF(Lat_Item_Tag,tplant,sEESWPrtType);
							printf("\n T10:Fun1 done for Parent part:[%s] GMDMLFun:[%d]",Part_nobrr,GMDMLFun);fflush(stdout);
							if (GMDMLFun >0)
							{
								//Expanssion start
								printf("\n T11: BOM Window setting.");fflush(stdout);
								if(BOM_create_window(&tagBOMwindow)!=ITK_ok)   PrintErrorStack();
								if(CFM_find("ERC release and above", &tagRule)!=ITK_ok)   PrintErrorStack(); 
								if(BOM_set_window_config_rule( tagBOMwindow, tagRule )!=ITK_ok)   PrintErrorStack();
								//0-qty skip
								if(PIE_find_closure_rules("TMLBOMExpandByQty",PIE_TEAMCENTER, &rulefoundXO, &POclosureruleXO )!=ITK_ok)   PrintErrorStack();
								//printf ("PO3:rulefounddd count %d \n",rulefoundXO);fflush(stdout);
								if (rulefoundXO > 0)
								{
									POclose_tagXO = POclosureruleXO[0];
									//printf ("PO3: POclose_taggg closure rule found \n");fflush(stdout);
								}
								if(BOM_window_set_closure_rule( tagBOMwindow,POclose_tagXO, 0, POrulenameXO,POrulevalueXO )!=ITK_ok)   PrintErrorStack();
								//0-qty skip

								if(BOM_set_window_top_line(tagBOMwindow, NULLTAG, Lat_Item_Tag, NULLTAG, &tagBOMline)!=ITK_ok)   PrintErrorStack();
								if(BOM_line_ask_child_lines(tagBOMline, &childCountTopBomLine, &tagTopBLchildLine)!=ITK_ok)   PrintErrorStack();
								printf("\n T12:Parent Part:%s with revision:%s No of child parts: %d\n",Part_nobrr,rev_iddd,childCountTopBomLine);fflush(stdout);
								for(iCount =0 ; iCount < childCountTopBomLine; iCount++)
								{
									printf("\n T13:Exp start Parent P1:%s No of child taken: %d \n",Part_nobr,iCount);fflush(stdout);
									FunOutflag=0;
									FunOutflag= tm_POCheckStopAtFBOMline(tagTopBLchildLine[iCount],tplant,sEESWPrtType,Part_nobrr,rev_iddd);
									printf("\n T14:Exp done Parent P1:[%s] FunOutflag:[%d]",Part_nobr,FunOutflag);fflush(stdout);
								}
							}
						}
						//MOhan

						if ((NOPOPartsSet==NULL)||(tc_strcmp(NOPOPartsSet,"")==0))
						{
							fprintf(fd,"\n SET OF NOPO PARTS ARE NOT FOUND. \n");
							printf("\n PO:NOPOPartsSet is NULL.\n");fflush(stdout);
						}
						else
						{
							//if(GRM_find_relation_type("Contents", &Fold_Cont_Rel)!=ITK_ok)PrintErrorStack();
							printf("\n NOPO addtition start.\n");fflush(stdout);
							fprintf(fd,"\n SET OF NOPO PARTS ARE BELOW.\n[PO to be placed or VCMS allowed]");
							nopo=0;
							piStringCount=0;
							if(EPM__parse_string(NOPOPartsSet,",",&piStringCount,&GStringList)!=ITK_ok)PrintErrorStack();
							for (nopo=0;nopo<piStringCount ;nopo++ )
							{
								printf("\n PO:..NOPOPartsSet Part:%s \n",GStringList[nopo]);fflush(stdout);
								//if(ITEM_find_item (GStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
								NOPOpart_tg= t5GetItemRevison(GStringList[nopo]);
								if(NOPOpart_tg==NULLTAG)
								{
									printf("\n UnMat:P9:Object not found in Teamcenter.\n");fflush(stdout);
								}
								else
								{
									PrtRev_count=0;
									if(ITEM_list_all_revs (NOPOpart_tg, &PrtRev_count, &PrtRev_list)!=ITK_ok)   PrintErrorStack();
									printf("\n P67:Total rev found: %d.", PrtRev_count);fflush(stdout);
									if(PrtRev_count > 0)
									{
										pii=0;
										for(pii=PrtRev_count-1;pii>=0;pii--)
										{
											PrtRelStus_cnt=0;
											PrtRev_tg=PrtRev_list[pii];
											if(WSOM_ask_release_status_list (PrtRev_tg, &PrtRelStus_cnt, &PrtRelStusTg)!=ITK_ok)PrintErrorStack();
											printf("\n P69:PrtRelStus_cnt is:%d.", PrtRelStus_cnt);fflush(stdout);
											if(PrtRelStus_cnt > 0)
											{
												n=0;
												for(n=0;n<PrtRelStus_cnt;n++)
												{
													//POAvailFlag=0;
													//Checking PO availability on any revision.
													if(AOM_ask_value_string (PrtRelStusTg[n], "object_name", &PrtRelStus)!=ITK_ok)PrintErrorStack();
													if(tc_strstr(PrtRelStus,"T5_LcsErcRlzd")!=NULL)
													{
														printf("\n PO:P1:part details in report..\n");fflush(stdout);
														if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,sPlantMakeBuy,&sPartCS)!=ITK_ok)   PrintErrorStack();
														printf("\n PO:P1:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
														printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
														fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
														break;
													}
												}
												break;
											}
										}
									}
								}
							}
							fprintf(fd,"\n ");
						}

						//UNMATURATED Parts
						if ((UnMatPartSet==NULL)||(tc_strcmp(UnMatPartSet,"")==0))
						{
							fprintf(fd,"\nSET OF UNMATURATED DR-STATUS PARTS ARE NOT FOUND. \n");
							printf("\n UnMat:UnMatPartSet is NULL.\n");fflush(stdout);
						}
						else
						{
							//if(GRM_find_relation_type("Contents", &Fold_Cont_Rel)!=ITK_ok)PrintErrorStack();
							printf("\n UnMat addtition start.\n");fflush(stdout);
							fprintf(fd,"\n SET OF UNMATURATED DR-STATUS PARTS ARE BELOW.\n[Parts should be updated to higher DR-status with Gate Maturation DML.]");
							nopo=0;
							piStringCount=0;
							if(EPM__parse_string(UnMatPartSet,",",&piStringCount,&GStringList)!=ITK_ok)PrintErrorStack();
							for (nopo=0;nopo<piStringCount ;nopo++ )
							{
								printf("\n UnMat:UnMatPartSet Part:%s \n",GStringList[nopo]);fflush(stdout);
								//if(ITEM_find_item (GStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
								NOPOpart_tg= t5GetItemRevison(GStringList[nopo]);
								if(NOPOpart_tg==NULLTAG)
								{
									printf("\n UnMat:P9:Object not found in Teamcenter.\n");fflush(stdout);
								}
								else
								{
									if(ITEM_list_all_revs (NOPOpart_tg, &PrtRev_count, &PrtRev_list)!=ITK_ok)   PrintErrorStack();
									printf("\n P67:Total rev found: %d.", PrtRev_count);fflush(stdout);
									if(PrtRev_count > 0)
									{
										pii=0;
										for(pii=PrtRev_count-1;pii>=0;pii--)
										{
											PrtRelStus_cnt=0;
											PrtRev_tg=PrtRev_list[pii];
											if(WSOM_ask_release_status_list (PrtRev_tg, &PrtRelStus_cnt, &PrtRelStusTg)!=ITK_ok)PrintErrorStack();
											printf("\n P69:PrtRelStus_cnt is:%d.", PrtRelStus_cnt);fflush(stdout);
											if(PrtRelStus_cnt > 0)
											{
												n=0;
												for(n=0;n<PrtRelStus_cnt;n++)
												{
													//POAvailFlag=0;
													//Checking PO availability on any revision.
													if(AOM_ask_value_string (PrtRelStusTg[n], "object_name", &PrtRelStus)!=ITK_ok)PrintErrorStack();
													if(tc_strstr(PrtRelStus,"T5_LcsErcRlzd")!=NULL)
													{
														printf("\n UnMat:P9:part details in report..\n");fflush(stdout);
														if(AOM_ask_value_string(PrtRev_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
														if(AOM_ask_value_string(PrtRev_tg,sPlantMakeBuy,&sPartCS)!=ITK_ok)   PrintErrorStack();
														printf("\n UnMat:P1:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
														printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
														fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
														break;
													}
												}
												break;
											}
										}
									}
								}
							}
							fprintf(fd,"\n ");
						}

						//CHECK BYPASS PARTS
						if((BypassPartSet==NULL)|| (tc_strcmp(BypassPartSet,"")==0))
						{
							fprintf(fd,"\n SET OF NO PO CHECK BYPASS PARTS NOT FOUND. \n");
							printf("\n PO:BypassPartSet is NULL.\n");fflush(stdout);
						}
						else
						{
							fprintf(fd,"\n SET OF NO PO CHECK BYPASS PARTS ARE BELOW. ");
							nopo=0;
							piStringCount=0;
							if(EPM__parse_string(BypassPartSet,",",&piStringCount,&DStringList)!=ITK_ok)PrintErrorStack();
							for (nopo=0;nopo<piStringCount ;nopo++ )
							{
								printf("\n PO:BypassPartSet Part:%s \n",DStringList[nopo]);fflush(stdout);
								//if(ITEM_find_item (DStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
								NOPOpart_tg= t5GetItemRevison(DStringList[nopo]);
								if(NOPOpart_tg==NULLTAG)
								{
									printf("\n PO1:NOPOpart_tg is NULL.\n");fflush(stdout);
								}
								else
								{
									printf("\n PO1:NOPOpart_tg is found.\n");fflush(stdout);
									if(ITEM_ask_latest_rev (NOPOpart_tg, &NOPOprt_tg)!=ITK_ok)PrintErrorStack();
									if(NOPOprt_tg==NULLTAG)
									{
										printf("\n PO:P4:Object not found in Teamcenter.\n");fflush(stdout);
									}
									else
									{
										if(AOM_ask_value_string(NOPOprt_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,sPlantMakeBuy,&sPartCS)!=ITK_ok)   PrintErrorStack();
										printf("\n PO:P4:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
										printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
										fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
									}
								}
							}
							fprintf(fd,"\n ");
							printf("\n PO:ERROR:BypassPartSet for DML\n %s\n",BypassPartSet);fflush(stdout);
						}

						//CHECK BYPASS PARTS
						if((POAvailPartSet==NULL)|| (tc_strcmp(POAvailPartSet,"")==0))
						{
							fprintf(fd,"\n SET OF PO AVAILABLE FOR PARTS NOT FOUND. \n");
							printf("\n PO:POAvailPartSet is NULL.\n");fflush(stdout);
						}
						else
						{
							fprintf(fd,"\n SET OF PO AVAILABLE FOR PARTS ARE BELOW. ");
							nopo=0;
							piStringCount=0;
							if(EPM__parse_string(POAvailPartSet,",",&piStringCount,&CStringList)!=ITK_ok)PrintErrorStack();
							for (nopo=0;nopo<piStringCount ;nopo++ )
							{
								printf("\n PO:POAvailPartSet Part:%s \n",CStringList[nopo]);fflush(stdout);
								//if(ITEM_find_item (CStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
								NOPOpart_tg= t5GetItemRevison(CStringList[nopo]);
								if(NOPOpart_tg==NULLTAG)
								{
									printf("\n PO3:NOPOpart_tg is NULL.\n");fflush(stdout);
								}
								else
								{
									printf("\n PO2:NOPOpart_tg is found.\n");fflush(stdout);
									if(ITEM_ask_latest_rev (NOPOpart_tg, &NOPOprt_tg)!=ITK_ok)PrintErrorStack();
									if(NOPOprt_tg==NULLTAG)
									{
										printf("\n PO:P5:Object not found in Teamcenter.\n");fflush(stdout);
									}
									else
									{
										if(AOM_ask_value_string(NOPOprt_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,sPlantMakeBuy,&sPartCS)!=ITK_ok)   PrintErrorStack();
										printf("\n PO:P5:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
										printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
										fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
									}
								}
							}
							fprintf(fd,"\n ");
							printf("\n PO:ERROR:POAvailPartSet for DML\n %s\n",POAvailPartSet);fflush(stdout);
						}
						//CHECK BYPASS PARTS
						if((PORevAvailPartSet==NULL)|| (tc_strcmp(PORevAvailPartSet,"")==0))
						{
							fprintf(fd,"\n SET OF PO REV AVAILABLE FOR PARTS ARE NOT FOUND. \n");
							printf("\n PO:PORevAvailPartSet is NULL.\n");fflush(stdout);
						}
						else
						{
							fprintf(fd,"\n SET OF PO REV AVAILABLE FOR PARTS ARE BELOW. ");
							nopo=0;
							piStringCount=0;
							if(EPM__parse_string(PORevAvailPartSet,",",&piStringCount,&BStringList)!=ITK_ok)PrintErrorStack();
							for (nopo=0;nopo<piStringCount ;nopo++ )
							{
								printf("\n PO:PORevAvailPartSet Part:%s \n",BStringList[nopo]);fflush(stdout);
								//if(ITEM_find_item (BStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
								NOPOpart_tg= t5GetItemRevison(BStringList[nopo]);
								if(NOPOpart_tg==NULLTAG)
								{
									printf("\n PO4:NOPOpart_tg is NULL.\n");fflush(stdout);
								}
								else
								{
									printf("\n PO5:NOPOpart_tg is found.\n");fflush(stdout);
									if(ITEM_ask_latest_rev (NOPOpart_tg, &NOPOprt_tg)!=ITK_ok)PrintErrorStack();
									if(NOPOprt_tg==NULLTAG)
									{
										printf("\n PO:P6:Object not found in Teamcenter.\n");fflush(stdout);
									}
									else
									{
										if(AOM_ask_value_string(NOPOprt_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,sPlantMakeBuy,&sPartCS)!=ITK_ok)   PrintErrorStack();
										printf("\n PO:P6:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
										printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
										fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
									}
								}
							}
							fprintf(fd,"\n ");
							printf("\n PO:ERROR:PORevAvailPartSet for DML\n %s\n",PORevAvailPartSet);fflush(stdout);
						}
						//SKIPPED PARTS
						if((SkippedPartSet==NULL)|| (tc_strcmp(SkippedPartSet,"")==0))
						{
							fprintf(fd,"\n SET OF PO CHECK SKIPPED FOR PARTS NOT FOUND. \n");
							printf("\n PO:SkippedPartSet is NULL.\n");fflush(stdout);
						}
						else
						{
							fprintf(fd,"\n SET OF PO CHECK SKIPPED FOR PARTS ARE BELOW. ");
							nopo=0;
							piStringCount=0;
							if(EPM__parse_string(SkippedPartSet,",",&piStringCount,&AStringList)!=ITK_ok)PrintErrorStack();
							for (nopo=0;nopo<piStringCount ;nopo++ )
							{
								printf("\n PO:SkippedPartSet Part:%s \n",AStringList[nopo]);fflush(stdout);
								//if(ITEM_find_item (AStringList[nopo], &NOPOpart_tg)!=ITK_ok)PrintErrorStack();
								NOPOpart_tg= t5GetItemRevison(AStringList[nopo]);
								if(NOPOpart_tg==NULLTAG)
								{
									printf("\n PO4:NOPOpart_tg is NULL.\n");fflush(stdout);
								}
								else
								{
									printf("\n PO5:NOPOpart_tg is found.\n");fflush(stdout);
									if(ITEM_ask_latest_rev (NOPOpart_tg, &NOPOprt_tg)!=ITK_ok)PrintErrorStack();
									if(NOPOprt_tg==NULLTAG)
									{
										printf("\n PO:P7:Object not found in Teamcenter.\n");fflush(stdout);
									}
									else
									{
										if(AOM_ask_value_string(NOPOprt_tg,"item_id",&nPrtNO)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"item_revision_id",&nPrtRev)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_PartStatus",&nPart_DR)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_PartType",&nPart_typ)!=ITK_ok)   PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_ProjectCode",&nPartProj)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_ColourInd",&nPartColInd)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_DesignGrp",&npartDesgGrp)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"t5_DrawingInd",&npartDrwind)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,"object_desc",&nPartObjTp)!=ITK_ok)PrintErrorStack();
										if(AOM_ask_value_string(NOPOprt_tg,sPlantMakeBuy,&sPartCS)!=ITK_ok)   PrintErrorStack();
										printf("\n PO:P7:nPrtNO:%s nPrtRev:%s  nPart_DR:%s  nPart_typ:%s  nPartProj:%s ",nPrtNO,nPrtRev,nPart_DR,nPart_typ,nPartProj);fflush(stdout);
										printf(" nPartColInd:%s npartDesgGrp:%s  npartDrwind:%s nPartObjTp:%s\n",nPartColInd,npartDesgGrp,npartDrwind,nPartObjTp);fflush(stdout);
										fprintf(fd,"\n`%s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t %s\t",nPrtNO,nPrtRev,sPartCS,nPart_typ,nPart_DR,nPartProj,npartDesgGrp,npartDrwind,nPartColInd,nPartObjTp);
									}
								}
							}
							fprintf(fd,"\n ");
							printf("\n PO:ERROR:SkippedPartSet for DML\n %s\n",SkippedPartSet);fflush(stdout);
						}
						fclose(fd);

						printf("\nCreating Report dataset\n");fflush(stdout);
						if(GRM_find_relation_type("IMAN_reference", &Iman_Ref_Rel_Tg)!=ITK_ok)PrintErrorStack();
						if(AE_find_datasettype2("MSExcel",&msWordType)!=ITK_ok)PrintErrorStack();
						if(msWordType == NULLTAG)
						{
							printf("\n Could not find Dataset\n");fflush(stdout);
							TC_write_syslog("Could not find Dataset Type Text. Please check data model\n");
							//continue;
						}

						if(AE_create_dataset_with_id(msWordType,sPartNumber,"NOPO Report for Part","","",&wordDataset)!=ITK_ok)PrintErrorStack();
						if(AE_save_myself(wordDataset));
						if(GRM_create_relation(Lat_Item_Tag,wordDataset,Iman_Ref_Rel_Tg,NULLTAG,&Part_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
						if(AOM_load(Part_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
						if(GRM_save_relation(Part_Ref_Rel_t)!=ITK_ok)PrintErrorStack();
						if(AE_ask_dataset_latest_rev(wordDataset,&wordLatestDat_t));
						if(AOM_refresh(wordLatestDat_t,TRUE)!=ITK_ok)PrintErrorStack();
						//if(AE_import_named_ref(wordLatestDat_t,"word",FileDown,NULL,SS_BINARY)!=ITK_ok)PrintErrorStack();
						if(AE_import_named_ref(wordLatestDat_t,"excel",FileDown,NULL,SS_BINARY)!=ITK_ok)PrintErrorStack();
						//if(AE_import_named_ref(wordLatestDat_t,"Text",FileDown,NULL,SS_TEXT)!=ITK_ok)PrintErrorStack();
						if(AE_save_myself(wordLatestDat_t)!=ITK_ok)PrintErrorStack();
						//if(AOM_unlock(DMLRevTg)!=ITK_ok)PrintErrorStack();
						if(AOM_refresh(Lat_Item_Tag,TRUE)!=ITK_ok)PrintErrorStack();
						remove(FileDown);
						if(FileDown) MEM_free(FileDown);
					}
				}
			}
		}
	}

	return 0;
}


int tm_get_plantmakebuy(char* sDml_TP)
{
	if (tc_strcmp(sDml_TP,"CARPLANT")==0)
	{
		tc_strdup("t5_CarMakeBuyIndicator",&sPlantMakeBuy);
		printf("\nPO:Plant1:CARPLANT:1100:: %s",sPlantMakeBuy);	fflush(stdout);	
	}
	else if (tc_strcmp(sDml_TP,"JAMSHEDPUR")==0)
	{
		tc_strdup("t5_JsrMakeBuyIndicator",&sPlantMakeBuy);
		printf("\n PO:Plant8:JAMSHEDPUR:2001:: %s",sPlantMakeBuy);	fflush(stdout);	;	
	}
	else if (tc_strcmp(sDml_TP,"LUCKNOW")==0)
	{
		tc_strdup("t5_LkoMakeBuyIndicator",&sPlantMakeBuy);
		printf("\n PO:Plant9:LUCKNOW:3001:: %s",sPlantMakeBuy);	fflush(stdout);	
	}
	else if (tc_strcmp(sDml_TP,"JSR DRIVELINES")==0)
	{
		tc_strdup("t5_JdlMakeBuyIndicator",&sPlantMakeBuy);
		printf("\n PO:JSR DRIVELINES:: %s",sPlantMakeBuy);	fflush(stdout);	
	}
	else if (tc_strcmp(sDml_TP,"DHARWAD")==0)
	{
		tc_strdup("t5_DwdMakeBuyIndicator",&sPlantMakeBuy);
		printf("\n PO:Plant6:DHARWAD:1500:: %s",sPlantMakeBuy);	fflush(stdout);		
	}
	else if (tc_strcmp(sDml_TP,"PANTNAGAR")==0)
	{
		tc_strdup("t5_PnrMakeBuyIndicator",&sPlantMakeBuy);
		printf("\n PO:Plant4:PANTNAGAR:: %s",sPlantMakeBuy);	fflush(stdout);	
	}
	else if (tc_strcmp(sDml_TP,"AHMEDABAD")==0)
	{
		tc_strdup("t5_AhdMakeBuyIndicator",&sPlantMakeBuy);
		printf("\n PO:Plant3:AHMEDABAD:7501:: %s",sPlantMakeBuy);	fflush(stdout);	
	}
	else if (tc_strcmp(sDml_TP,"PCVBU")==0)
	{
		tc_strdup("t5_PunMakeBuyIndicator",&sPlantMakeBuy);
		printf("\n PO:Plant2:PCVBU:1001: %s",sPlantMakeBuy);	fflush(stdout);	
	}
	else if (tc_strcmp(sDml_TP,"PUVBU")==0)
	{
		tc_strdup("t5_PunUVMakeBuyIndicator",&sPlantMakeBuy);
		printf("\n PO:Plant11:PUVBU:1140:: %s",sPlantMakeBuy);	fflush(stdout);	
	}
	else
	{
		printf("\nPO:Not Valid Target Plant:%s  \n",sDml_TP);fflush(stdout);
		return 0;
	}

	return 1;
}