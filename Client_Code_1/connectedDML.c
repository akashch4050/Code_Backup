/****************************************************************************************************************************
*  Author               :   Mohan Tayade
*  Module               :   Workflow Action Handler PO Check and attach NOPO report to DML as xls.
*  Code                 :   connectedDML.c
*  Command				:	connectedDML -u=ercpsup  -pf=XYT1ESA  -g=dba -file=input.txt
*						:	connectedDML -u=ercpsup  -pf=XYT1ESA  -g=dba -taskno=XXX
*  Created on			:   12/04/2019
*	Modification History :
*	S.No    Date         CR No		Modified By			Modification Notes
*
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

int			ifail 				= ITK_ok;
char		*sep				= "^";
int read_value_from_file(char*);

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
	char			* partnumb					= NULL;
	char			* DMLno					= NULL;
	//char			Data[100]					= "";
	//char			outputFile[200]				= "";

	//userid		= ITK_ask_cli_argument("-u=");
	//password 	= ITK_ask_cli_argument("-pf=");
	//group 		= ITK_ask_cli_argument("-g=");
	DMLno 		= ITK_ask_cli_argument("-dmlno=");

	time_t 	now;
	struct 	tm when;

	time(&now);
	when = *localtime(&now);

	//strftime(Data,100,"%d_%b_%Y_%H_%M_%S",&when);
	//sprintf(outputFile,"%s_output.txt",Data);

	//	if(tc_strlen(stripBlanks(userid)) == 0  || tc_strlen(stripBlanks(password)) == 0  || tc_strlen(stripBlanks(group)) == 0)  //9
	//	{
	//		print_requirement();
	//		return -1;
	//	}

	//ifail = ITK_auto_login();
	//ifail = ITK_init_module(userid, password, group); //9

	 ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	ITK_CALL(ITK_auto_login( ));
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Auto login .......\n");fflush(stdout);


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
	/*ifail = ITK_set_bypass(true);
    if (ifail != ITK_ok)
	{
		printf("\nUser is not Privileged Admin User \n\n");
		return -1;
	}*/
	//ifail = read_value_from_file(Filename);
	ifail = tm_connectedDMLFun(DMLno);
	printf("\nPO:....Inside PO check...\n");fflush(stdout);
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
int tm_connectedDMLFun(char* dml_no)
{
	int				ifail 					= ITK_ok;
	tag_t	objTypTag		= NULLTAG;
	char   ObjTyp[TCTYPE_name_size_c+1];
	int k=0;
	tag_t	AssyTg = NULLTAG;
	char	*Tsk_object_tpe	= NULL;
	char	*Tsk_object_type	= NULL;
	tag_t dmlTotaskRel = NULLTAG;
	tag_t tsk_HSI_rel_type = NULLTAG;
	tag_t DependDMLs = NULLTAG;
	tag_t task_to_DML = NULLTAG;
	int nopo=0;
	int DMlcunt=0;
	int taskCount=0;
	int partcnt=0;
	tag_t*	DML_tg	= NULLTAG;
	tag_t*	task_tg	= NULLTAG;
	char	*DMLSet = NULL;

	char*	usernam	=NULL; 
	char*	Dml_DR	=NULL; 
	char*	TaskDsgGrp	=NULL; 
	char*	Prtobj_type	=NULL; 
	char*	Part_nobr	=NULL; 
	char*	Dml_Type	=NULL; 
	char*	Taskno	=NULL; 
	char*	DML_class_Nm	=NULL; 
	char*	sSystPrtRevs	=NULL; 
	char*	sSystPrtRev	=NULL; 
	char*	PrtRelStus	=NULL; 
	char*	FldrName	=NULL; 
	char*	sSystPrtName	=NULL; 
	char*	Dml_num	=NULL; 
	char*	TskNm	=NULL; 
	char*	DMlnmb	=NULL; 
	char*	DmlDR	=NULL; 
	char*	PlntToPlnt	=NULL; 
	char*	Part_Revr	=NULL; 
	char*	DMltp	=NULL; 
	char*	FileDown	=NULL; 
	tag_t *PrtRelStusTg=NULLTAG;
	tag_t *Crevsn_list=NULLTAG;
	tag_t *FolderObj_tag=NULLTAG;
	int dmlcnt=0;
	int rev_count=0;
	int PrtRelStus_cnt=0;
	int fld=0;
	int tsk_cnt=0;
	int tskk=0;
	int n=0;
	int jr=0;
	int kk=0;
	int piStringCount=0;
	int FlderSize=0;
	int DDcount=0;
	int FlderObjCount=0;
	int DRmismatchFlg=0;
	tag_t All_item_tgs = NULLTAG;
	tag_t Ddml_tg = NULLTAG;
	tag_t DML_class = NULLTAG;
	tag_t *DependDMLs_tg = NULLTAG;
	tag_t item_tag = NULLTAG;
	tag_t DMLTag = NULLTAG;
	tag_t DependDML_TpTag = NULLTAG;
	tag_t DependDMLs_rel = NULLTAG;
	tag_t POclose_taggg = NULLTAG;
	tag_t Ddml_tgg = NULLTAG;
	tag_t *PartsTg = NULLTAG;
	tag_t *POclosureruleee = NULLTAG;
	tag_t *Tsk_objects = NULLTAG;
	char **	GStringList =NULL;
	char   DependDML_Type[TCTYPE_name_size_c+1];
	FILE	*fd;
	FileDown = (char *) MEM_alloc (sizeof (char) * 10000);
	DMLSet     =  (char *) MEM_alloc(100000 * sizeof(char));

	printf("\n P3:Input Item_id:%s",dml_no);fflush(stdout);
	//1] check correct relation
	//2] check for parts attached in ref rel.
	//3] check for parts in folder relation.
	tc_strcpy (DMLSet,"" );
	if(ITEM_find_item (dml_no, &item_tag)!=ITK_ok)PrintErrorStack();
	if(item_tag==NULLTAG)
	{
		printf("\n P4:item_tag is Null...\n");fflush(stdout);
	}
	else
	{
		printf("\n P5:item_tag is found...\n");fflush(stdout);
		if(ITEM_ask_latest_rev (item_tag, &DMLTag)!=ITK_ok)PrintErrorStack();
		if(DMLTag==NULLTAG)
		{
			printf("\n P6:Object not found in Teamcenter...!!\n");fflush(stdout);
			return 0;
		}
		else
		{
			printf("\n P7:Object Found...!!");fflush(stdout);
			if(TCTYPE_ask_object_type(DMLTag,&objTypTag)!=ITK_ok)PrintErrorStack();
			if(TCTYPE_ask_name(objTypTag,ObjTyp)!=ITK_ok)PrintErrorStack();
			printf("\n P8: Workfow obj type: [%s]", ObjTyp);fflush(stdout);

			if(POM_class_of_instance(DMLTag,&DML_class)!=ITK_ok)PrintErrorStack();
			if(POM_name_of_class(DML_class,&DML_class_Nm)!=ITK_ok)PrintErrorStack();
			printf("\n P9: DML_class_Nm:%s",DML_class_Nm);
			if(tc_strcmp(DML_class_Nm,"ChangeRequestRevision")==0)
			{
				if(POM_get_user_id (&usernam)!=ITK_ok)PrintErrorStack();
				printf("\n P10:Session login User1: %s\n",usernam); fflush(stdout);
				if(AOM_ask_value_string(DMLTag,"item_id",&Dml_num)!=ITK_ok)PrintErrorStack();
				if(AOM_ask_value_string(DMLTag,"t5_rlstype",&Dml_Type)!=ITK_ok)PrintErrorStack();
				if(AOM_ask_value_string(DMLTag,"t5_cDRstatus",&Dml_DR)!=ITK_ok)PrintErrorStack();
				printf("\n P11: Input Dmlno: %s Type:%s Dml_DR:%s ",Dml_num,Dml_Type,Dml_DR);fflush(stdout);
				/*if((tc_strcmp(Dml_Type,"TPL")==0) || (tc_strcmp(Dml_Type,"DFMR")==0)|| (tc_strcmp(Dml_Type,"MR")==0)|| (tc_strcmp(Dml_Type,"Veh")==0)|| (tc_strcmp(Dml_Type,"PR")==0)|| (tc_strcmp(Dml_Type,"CQ")==0) ||
				(tc_strcmp(Dml_Type,"EP")==0)|| (tc_strcmp(Dml_Type,"WTR")==0)|| (tc_strcmp(Dml_Type,"PPR")==0)|| (tc_strcmp(Dml_Type,"BEC")==0))*/
				if(tc_strcmp(Dml_Type,"TODR")==0)
				{
					if(AOM_ask_value_string(DMLTag, "t5_PlntToPlnt", &PlntToPlnt)!=ITK_ok)   PrintErrorStack();
					printf("\n DD:PlntToPlnt: %s \n",PlntToPlnt);fflush(stdout);
					if((tc_strcmp(PlntToPlnt,"DR2 to DR3")==0)||(tc_strcmp(PlntToPlnt,"AR2 to AR3")==0)||(tc_strcmp(PlntToPlnt,"DR3 to DR3P")==0)||(tc_strcmp(PlntToPlnt,"AR3 to AR3P")==0)||(tc_strcmp(PlntToPlnt,"DR3P to DR4")==0)||(tc_strcmp(PlntToPlnt,"AR3P to AR4")==0)||(tc_strcmp(PlntToPlnt,"AR3 to AR4")==0)||(tc_strcmp(PlntToPlnt,"AR4 to AR5")==0)||(tc_strcmp(PlntToPlnt,"DR3 to DR4")==0)||(tc_strcmp(PlntToPlnt,"DR4 to DR5")==0))
					{
						printf("\n DD:inside connected DML logic. \n");fflush(stdout);
						//DML to task expanssion
						if(GRM_find_relation_type("T5_DMLTaskRelation", &dmlTotaskRel)!=ITK_ok)PrintErrorStack();
						if (dmlTotaskRel!=NULLTAG)
						{
							tsk_cnt=0;
							DRmismatchFlg=0;
							//tc_strcpy(DRMismtchPrts,"");
							printf("\n P12:OK for dmlTotaskRel.");fflush(stdout);
							if(GRM_list_secondary_objects_only(DMLTag,dmlTotaskRel,&tsk_cnt,&Tsk_objects)!=ITK_ok)PrintErrorStack();
							printf("\n P13: Task count: %d",tsk_cnt);fflush(stdout);
							if (tsk_cnt>0)
							{
								for (tskk=0;tskk<tsk_cnt ;tskk++ )
								{
									if(AOM_ask_value_string(Tsk_objects[tskk],"object_type",&Tsk_object_type)!=ITK_ok)PrintErrorStack();
									printf("\n P15:Tsk_object_type is :%s\n",Tsk_object_type);fflush(stdout);

									if(strcmp(Tsk_object_type,"T5_ChangeTaskRevision")==0)
									{
										if(TaskDsgGrp) TaskDsgGrp=NULL;
										if(AOM_UIF_ask_value(Tsk_objects[tskk],"item_id",&TskNm)!=ITK_ok)PrintErrorStack();
										TaskDsgGrp=subString(TskNm,11,2);
										printf("\n P16: Task: [%s] TaskDsgGrp:[%s]\n",TskNm,TaskDsgGrp);fflush(stdout);

										if(AOM_ask_value_tags(Tsk_objects[tskk],"CMReferences",&partcnt,&PartsTg)!=ITK_ok)   PrintErrorStack();
										printf("\n P17: Now partcnt:%d",partcnt);fflush(stdout);
										if (partcnt>0)
										{
											for (k=0;k<partcnt ;k++ )
											{
												AssyTg=PartsTg[k];
												if(AOM_UIF_ask_value(AssyTg, "object_type", &Prtobj_type)!=ITK_ok)   PrintErrorStack();
												printf("\n P18: Object Type is : %s",Prtobj_type);	fflush(stdout);	
												if(tc_strcmp(Prtobj_type,"Folder")!=0)
												{
													//If part found..
													if(GRM_find_relation_type("CMHasSolutionItem", &tsk_HSI_rel_type)!=ITK_ok)PrintErrorStack();
													if(AOM_ask_value_string(AssyTg,"item_id",&Part_nobr)!=ITK_ok)   PrintErrorStack();
													if(AOM_ask_value_string(AssyTg,"item_revision_id",&Part_Revr)!=ITK_ok)   PrintErrorStack();
													printf("\n P19:.................... Part no taken : %s:%s",Part_nobr,Part_Revr);	fflush(stdout);	
													// getting DML from part
													if(GRM_list_primary_objects_only(AssyTg,tsk_HSI_rel_type,&taskCount,&task_tg)!=ITK_ok)PrintErrorStack();
													printf("\n P20:taskCount : %d",taskCount);	fflush(stdout);	
													
													if (taskCount>0)
													{
														for (kk=0;kk<taskCount ;kk++ )
														{
															if(AOM_ask_value_string(task_tg[kk],"object_type",&Tsk_object_tpe)!=ITK_ok)PrintErrorStack();
															printf("\n P21:Tsk_object_tpe is :%s\n",Tsk_object_tpe);fflush(stdout);
															if(strcmp(Tsk_object_tpe,"T5_ChangeTaskRevision")==0)
															{
																if(AOM_ask_value_string(task_tg[kk],"item_id",&Taskno)!=ITK_ok)   PrintErrorStack();
																printf("\n P22: Taskno is : %s",Taskno);	fflush(stdout);	
																if(GRM_find_relation_type("T5_DMLTaskRelation", &task_to_DML)!=ITK_ok)PrintErrorStack();
																if(GRM_list_primary_objects_only(task_tg[kk],task_to_DML,&DMlcunt,&DML_tg)!=ITK_ok)PrintErrorStack();
																printf("\n P21:DMlcunt is :%d\n",DMlcunt);fflush(stdout);
																if (DMlcunt>0)
																{
																	for (dmlcnt=0;dmlcnt<DMlcunt ;dmlcnt++ )
																	{
																		if(DMlnmb) DMlnmb=NULL;
																		if(DmlDR) DmlDR=NULL;
																		if(DMltp) DMltp=NULL;
																		if(AOM_ask_value_string(DML_tg[dmlcnt],"item_id",&DMlnmb)!=ITK_ok)PrintErrorStack();
																		if(AOM_ask_value_string(DML_tg[dmlcnt],"t5_rlstype",&DMltp)!=ITK_ok)PrintErrorStack();
																		if(AOM_ask_value_string(DML_tg[dmlcnt],"t5_cDRstatus",&DmlDR)!=ITK_ok)PrintErrorStack();
																		printf("\n P23:...........Part:%s DML:%s Type:%s DR:%s\n",Part_nobr,DMlnmb,DMltp,DmlDR);fflush(stdout);
																		if((tc_strcmp(DMltp,"TODR")!=0)&& ((tc_strcmp(DmlDR,"DR0")==0)||(tc_strcmp(DmlDR,"DR1")==0)||(tc_strcmp(DmlDR,"DR2")==0)||(tc_strcmp(DmlDR,"DR3")==0)||(tc_strcmp(DmlDR,"AR0")==0)||(tc_strcmp(DmlDR,"AR1")==0)||(tc_strcmp(DmlDR,"AR2")==0)||(tc_strcmp(DmlDR,"AR3")==0)))
																		{
																			//AllowToAttachFlg=0;
																			//logic to add in folder
																			if((DMLSet==NULL)|| (tc_strcmp(DMLSet,"")==0))
																			{
																				tc_strcpy (DMLSet,"" );
																				tc_strcat (DMLSet,DMlnmb);
																				//AllowToAttachFlg=1;
																				printf("\n P24:DMlnmb added to DMLSet: %s",DMlnmb);	fflush(stdout);	
																			}
																			else
																			{
																				if(strstr(DMLSet,DMlnmb)==NULL)
																				{
																					tc_strcat (DMLSet,"," );
																					tc_strcat (DMLSet,DMlnmb);
																					//AllowToAttachFlg=1;
																					printf("\n P25:DMlnmb added to DMLSet: %s",DMlnmb);	fflush(stdout);
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
													//If folder...
													if(AOM_ask_value_string(AssyTg,"object_name",&FldrName)!=ITK_ok)PrintErrorStack();
													printf("\n P26:FldrName:: %s\n", FldrName);fflush(stdout);
													if(tc_strcmp(FldrName,"Parts For Gate Maturation")==0)
													{
														printf("\n P27: getting parts.\n");fflush(stdout);
														if(FL_ask_size(AssyTg,&FlderSize));
														printf("\n P28:FlderSize is:   %d\n",FlderSize);fflush(stdout);
														if(FL_ask_references(AssyTg,FL_fsc_by_date_modified ,&FlderObjCount,&FolderObj_tag));
														printf("\n P29:FlderObjCount is..:   %d\n",FlderObjCount);fflush(stdout);
														if(FlderObjCount>0)
														{
															fld=0;
															for (fld=0;fld<FlderObjCount;fld++ )
															{
																if(AOM_ask_value_string(FolderObj_tag[fld],"item_id",&sSystPrtName)!=ITK_ok)PrintErrorStack();
																if(AOM_UIF_ask_value (FolderObj_tag[fld], "item_revision_id", &sSystPrtRev)!=ITK_ok)PrintErrorStack();
																printf("\n P30:........part from folder: %s: %s.", sSystPrtName,sSystPrtRev);fflush(stdout);

																/*All_item_tgs= t5GetItemRevison(sSystPrtName);
																if(All_item_tgs==NULLTAG)
																{
																	printf("\n DR:All_item_tgs is NULL..\n");fflush(stdout);
																}
																else
																{
																	if(ITEM_list_all_revs (All_item_tgs, &rev_count, &Crevsn_list)!=ITK_ok);
																	printf("\n T5PartToTaskDeleteVal:Total rev found: %d.", rev_count);fflush(stdout);
																	if(rev_count > 0)
																	{
																		ii=0;
																		for(ii=rev_count-1;ii>=0;ii--)
																		{
																			PrtRelStus_cnt=0;
																			if(WSOM_ask_release_status_list (Crevsn_list[ii], &PrtRelStus_cnt, &PrtRelStusTg)!=ITK_ok);
																			printf("\n T5PartToTaskDeleteVal:PrtRelStus_cnt is:%d.", PrtRelStus_cnt);fflush(stdout);

																			if(PrtRelStus_cnt > 0)
																			{
																				n=0;
																				for(n=0;n<PrtRelStus_cnt;n++)
																				{
																					if(AOM_ask_value_string (PrtRelStusTg[n],"object_name", &PrtRelStus)!=ITK_ok);
																					if(AOM_UIF_ask_value (Crevsn_list[ii], "item_revision_id", &sSystPrtRevs)!=ITK_ok)PrintErrorStack();
																					printf("\n T5PartToTaskDeleteVal:PrtRelStus is:%s :Part rev:%s.",PrtRelStus,sSystPrtRevs);fflush(stdout);
																					if(tc_strstr(PrtRelStus,"T5_LcsErcRlzd")!=NULL)
																					{
																						//Get the MR DML for same revision
																						if(tc_strcmp(sSystPrtRev,sSystPrtRevs)==0)
																						{
																							break;
																						}
																					}
																				}
																			}
																		}
																	}
																}*/
																// getting DML from part
																if(GRM_list_primary_objects_only(FolderObj_tag[fld],tsk_HSI_rel_type,&taskCount,&task_tg)!=ITK_ok)PrintErrorStack();
																printf("\n P31:taskCount : %d",taskCount);	fflush(stdout);	
																if (taskCount>0)
																{
																	kk=0;
																	for (kk=0;kk<taskCount ;kk++ )
																	{
																		if(AOM_ask_value_string(task_tg[kk],"object_type",&Tsk_object_tpe)!=ITK_ok)PrintErrorStack();
																		printf("\n P32:Tsk_object_tpe is :%s\n",Tsk_object_tpe);fflush(stdout);
																		if(strcmp(Tsk_object_tpe,"T5_ChangeTaskRevision")==0)
																		{
																			if(Taskno) Taskno=NULL;
																			if(AOM_ask_value_string(task_tg[kk],"item_id",&Taskno)!=ITK_ok)   PrintErrorStack();
																			printf("\n P33: Taskno is : %s",Taskno);	fflush(stdout);	
																			if(GRM_find_relation_type("T5_DMLTaskRelation", &task_to_DML)!=ITK_ok)PrintErrorStack();
																			if(GRM_list_primary_objects_only(task_tg[kk],task_to_DML,&DMlcunt,&DML_tg)!=ITK_ok)PrintErrorStack();
																			if (DMlcunt>0)
																			{
																				for (dmlcnt=0;dmlcnt<DMlcunt ;dmlcnt++ )
																				{
																					if(DMlnmb) DMlnmb=NULL;
																					if(DmlDR) DmlDR=NULL;
																					if(DMltp) DMltp=NULL;
																					if(AOM_ask_value_string(DML_tg[dmlcnt],"item_id",&DMlnmb)!=ITK_ok)PrintErrorStack();
																					if(AOM_ask_value_string(DML_tg[dmlcnt],"t5_rlstype",&DMltp)!=ITK_ok)PrintErrorStack();
																					if(AOM_ask_value_string(DML_tg[dmlcnt],"t5_cDRstatus",&DmlDR)!=ITK_ok)PrintErrorStack();
																					printf("\n P34:...........Part: %s DML:%s Type:%s DR:%s\n",sSystPrtName,DMlnmb,DMltp,DmlDR);fflush(stdout);
																					if((tc_strcmp(DMltp,"TODR")!=0)&& ((tc_strcmp(DmlDR,"DR0")==0)||(tc_strcmp(DmlDR,"DR1")==0)||(tc_strcmp(DmlDR,"DR2")==0)||(tc_strcmp(DmlDR,"DR3")==0)||(tc_strcmp(DmlDR,"AR0")==0)||(tc_strcmp(DmlDR,"AR1")==0)||(tc_strcmp(DmlDR,"AR2")==0)||(tc_strcmp(DmlDR,"AR3")==0)))
																					{
																						//AllowToAttachFlg=0;
																						//logic to add in folder
																						if((DMLSet==NULL)|| (tc_strcmp(DMLSet,"")==0))
																						{
																							tc_strcpy (DMLSet,"" );
																							tc_strcat (DMLSet,DMlnmb);
																							//AllowToAttachFlg=1;
																							printf("\n P35:DMlnmb added to DMLSet: %s",DMlnmb);	fflush(stdout);	
																						}
																						else
																						{
																							if(strstr(DMLSet,DMlnmb)==NULL)
																							{
																								tc_strcat (DMLSet,"," );
																								tc_strcat (DMLSet,DMlnmb);
																								//AllowToAttachFlg=1;
																								printf("\n P36:DMlnmb added to DMLSet: %s",DMlnmb);	fflush(stdout);
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
												}
											}
										}
									}
								}
							}
						}
						else
						{
							printf("\n AAAAAAAAAAAAAAAA\n");fflush(stdout);
						}
						if ((DMLSet==NULL)||(tc_strcmp(DMLSet,"")==0))
						{
							printf("\n P38:NO dependent DMLs fond\n");fflush(stdout);
						}
						else
						{
							//if(GRM_find_relation_type("T5_CmHasNOPoDetails", &DependDMLs)!=ITK_ok)PrintErrorStack();
							if(GRM_find_relation_type("T5_HasDependentDML", &DependDMLs)!=ITK_ok)PrintErrorStack();
							nopo=0;
							piStringCount=0;
							if(EPM__parse_string(DMLSet,",",&piStringCount,&GStringList)!=ITK_ok)PrintErrorStack();
							for (nopo=0;nopo<piStringCount ;nopo++ )
							{
								printf("\n P40:..DMLSet Part:%s \n",GStringList[nopo]);fflush(stdout);
								if(ITEM_find_item (GStringList[nopo], &Ddml_tg)!=ITK_ok)PrintErrorStack();
								if(Ddml_tg==NULLTAG)
								{
									printf("\n P41:Ddml_tg is Null...\n");fflush(stdout);
								}
								else
								{
									printf("\n P42:Ddml_tg is found...\n");fflush(stdout);
									if(ITEM_ask_latest_rev (Ddml_tg, &Ddml_tgg)!=ITK_ok)PrintErrorStack();
									if(Ddml_tgg==NULLTAG)
									{
										printf("\n P43:Ddml_tgg not found...!!\n");fflush(stdout);
										//return 0;
									}
									else
									{
										printf("\n P46:going to insert part in folder.%s \n",GStringList[nopo]);fflush(stdout);
										if(AOM_lock(DMLTag)!=ITK_ok)PrintErrorStack();
										if(AOM_lock(Ddml_tgg)!=ITK_ok)PrintErrorStack();
										if(GRM_create_relation(DMLTag,Ddml_tgg,DependDMLs,NULLTAG,&DependDMLs_rel)!=ITK_ok)PrintErrorStack();
										if(AOM_load(DependDMLs_rel)!=ITK_ok)PrintErrorStack();
										if(GRM_save_relation(DependDMLs_rel)!=ITK_ok)PrintErrorStack();
										printf("\n DD:Relation created. .....\n");fflush(stdout);
										if(AOM_save (Ddml_tgg)!=ITK_ok)PrintErrorStack();
										if(AOM_unlock(Ddml_tgg)!=ITK_ok)PrintErrorStack();
										if(AOM_refresh(Ddml_tgg,TRUE)!=ITK_ok)PrintErrorStack();
										if(AOM_save (DMLTag)!=ITK_ok)PrintErrorStack();
										if(AOM_unlock(DMLTag)!=ITK_ok)PrintErrorStack();
										if(AOM_refresh(DMLTag,TRUE)!=ITK_ok)PrintErrorStack();
										printf("\n P47:DD relation created...\n");fflush(stdout);
									}
								}
							}
						}
					}
				}
				else
				{
					printf("\n P49:DML not eligible for connected DML logic.\n");fflush(stdout);
				}
			}
		}
	}
		
	//fclose(fp);
	return ifail;
}

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
	/*if(attrs2) MEM_free(attrs2);
	if(attrss2) MEM_free(attrss2);
	if(values2) MEM_free(values2);
	if(valuess2) MEM_free(valuess2);*/

	return OutPutTag;	
}

//PO CHECK



