#define _CLMAIN_DEFNS
#include <user_exits/user_exits.h>
#include <ss/ss_const.h>
#include <ae/ae.h>
#include <tccore/aom_prop.h>
#include <epm/epm_errors.h>
#include <sys/stat.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tccore/custom.h>
#include <ae/dataset.h>
#include <tccore/grm.h>
#include <ict/ict_userservice.h>
#include <itk/mem.h>
#include <tccore/method.h>
#include <ps/ps_errors.h>
#include <tccore/aom.h>
#include <res/reservation.h>
#include <sa/sa.h>
#include <fclasses/tc_stdarg.h>
#include <fclasses/tc_stdio.h>
#include <fclasses/tc_stdlib.h>
#include <fclasses/tc_string.h>
#include <tc/tc.h>
#include <tc/emh.h>
#include <fclasses/tc_string.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <sa/tcfile.h>
#include <tcinit/tcinit.h>
#include <tccore/tctype.h>
#include <fclasses/tc_time.h>
#include <unidefs.h>
#include <tccore/workspaceobject.h>
#include <tccore/item_msg.h>
#include <tccore/tc_msg.h>
#include <tccore/iman_msg.h>
#include <epm/epm.h>
#include <ae/dataset_msg.h>
#include <tccore/grmtype.h>
#include <pom/pom/pom.h>
#include <ai/sample_err.h>
#include <dispatcher/dispatcher_itk.h>
#include <unistd.h>
#include <tcinit/tcinit.h>
#include <tccore/item.h>
#include <tc/folder.h>
#include <tccore/grm.h>
#include <form/form.h>
#include <tccore/aom.h>
#include <tccore/workspaceobject.h>
#include <tccore/aom_prop.h>
#include <sa/tcfile.h>
#include <tc/preferences.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <ae/datasettype.h>
#include <time.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ps/ps.h>
#include <mechatronics/psconnection.h>
#include <tccoreext/gde.h>
#include <mechatronics/gdelink.h>
#include <tccore/tctype.h>
#include <ecm/ecm.h>
#include <ae/nxsm.h>
#include <tccore/grm.h>
#include <me/me.h>
#include <math.h>
#include <fclasses/tc_date.h>
#include <itk/mem.h>
#include <tccore/item_errors.h>
#include "string.h"

//#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) { PrintErrorStack(); return ifail;}

FILE	*DmlExecutionLog;
FILE	*NoPOResult;

#define ITK_CALL(X) 							\
		iStatus=X; 								\
		if (iStatus != ITK_ok ) 					\
		{										\
			int				index = 0;			\
			int				n_ifails = 0;		\
			const int*		severities = 0;		\
			const int*		ifails = 0;			\
			const char**	texts = NULL;		\
												\
			EMH_ask_errors( &n_ifails, &severities, &ifails, &texts);		\
			for( index=0; index<n_ifails; index++)							\
			{																\
			}																\
			return iStatus;													\
		}																	\
	;

char* subString(char* mainStringf ,int fromCharf,int toCharf);
char* subString
(
	char* mainStringf ,
	int fromCharf     ,
	int toCharf
)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(toCharf+1);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}
int ITK_user_main(int argc, char *argv[])
{
	int			iStatus				 = 0;
	
	char		*message			 = NULL;

	char *argString  = ITK_ask_cli_argument("-dml=");
	printf("argstring ---> %s\n",argString);

	iStatus = ITK_auto_login();	

	if(iStatus == ITK_ok)
	{
		printf("\n ** Login Successful ** \n"); fflush(stdout);
		
		if (argString != 0)
		{
			printf("input argument string is  :: %s\n",argString); fflush(stdout);

			//// Calling function to get details of ERC DML in TCUA for Web Service..

			iStatus = getERCDMLforWebService(argString);
			
			if(iStatus == 0)
			{	printf("\n Successful.."); fflush(stdout);
				fprintf(DmlExecutionLog,"\n Successful..\n"); fflush(DmlExecutionLog);
			}
			else
			{
				printf("\n UnSuccessful...\n"); fflush(stdout);
				fprintf(DmlExecutionLog,"\n UnSuccessful...\n"); fflush(DmlExecutionLog);
			}

		}
		else
		{
			printf("input argument string is blank, kindly provide valide input\n"); fflush(stdout);
		}
	}
	else
	{
		printf("\n\n\t Login UnSuccessful\n"); 
	}	
	
	fclose(NoPOResult);
	fclose(DmlExecutionLog);

	return iStatus;
}
char *replaceWord(const char *s, const char *oldW, 
                                 const char *newW) 
{ 
    char *result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
  
    // Counting the number of times old word 
    // occur in the string 
    for (i = 0; s[i] != '\0'; i++) 
    { 
        if (strstr(&s[i], oldW) == &s[i]) 
        { 
            cnt++; 
  
            // Jumping to index after the old word. 
            i += oldWlen - 1; 
        } 
    } 
  
    // Making new string of enough length 
    result = (char *)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) 
    { 
        // compare the substring with the result 
        if (strstr(s, oldW) == s) 
        { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
    return result; 
}
int getERCDMLforWebService(char * dml_no)
{	
	int				iStatus			= 0;
	int				secObj_count	= 0;
	int				ind_found		= 0;
	int				stat_count		= 0;
	int				dsgn_stat_count	= 0;
	int				cnt,i,m			= 0;
	char			*ExeLogFile		= NULL;
	char			*ResLogFile		= NULL;

	tag_t dml_tag = NULLTAG;
	tag_t DMLRevTag = NULLTAG;
	tag_t RevTypTag = NULLTAG;
	tag_t			uaindObj		= NULLTAG;
	tag_t			*indresults		= NULLTAG;
	tag_t			*status_list	= NULLTAG;
	tag_t			*desgn_status_list	= NULLTAG;
	tag_t           usrTag			= NULLTAG;
	tag_t           persontag		= NULLTAG;
	tag_t           relation_type	= NULLTAG;
	tag_t           *secondary_list	= NULLTAG;
	tag_t           designObj		= NULLTAG;
	tag_t           relationObj		= NULLTAG;

	char *erc_dml = NULL;
	char *dmlNumber = NULL;
	char *RevTyp = NULL;
	char *DMLRevId = NULL;
	char *DMLtype = NULL;
	char *DMLProj = NULL;
	char *Dml_TP = NULL;
	char *Dml_TPStr = NULL;
	char *DmlRlzStatus = NULL;
	char *TargetPlantMakeBuyNm = NULL;
	char *TargetPlantCS = NULL;
	char *DmlDesgnGrp = NULL;
	char *sNOPOFldr = NULL;
	char *PrtNo = NULL;
	char *PrtRevSeq = NULL;
	char *PrtRev = NULL;
	char *PrtSeq = NULL;
	char *PrtDescTmp = NULL;
	char *PrtDesc = NULL;
	char *PrtHomo = NULL;
	char *PrtCmvrReqStr = NULL;
	char *DmlCreator = NULL;
	char *DmlCreatorTmp = NULL;
	char *DmlCreatorTmp2 = NULL;
	char *DmlCreatorTmp3 = NULL;
	char *DmlAuthBy = NULL;
	char *PORelType = NULL;

	logical PrtCmvrReq;

	char Reftype_name[TCTYPE_name_size_c+1];


	tag_t NOPO_dml_Ref_Rel = NULLTAG;
	tag_t Fold_Cont_Rel = NULLTAG;
	tag_t RefTypeTag = NULLTAG;
	tag_t *RefDoc_tag = NULLTAG;
	tag_t *FoldContents_tag = NULLTAG;
	int Refcount=0;
	int jr,k=0;
	int FldContentCnt=0;
	int ReqFdrSize=0;
	
	ExeLogFile = (char	*)MEM_alloc(100);
	ResLogFile = (char	*)MEM_alloc(100);
	erc_dml = (char	*)MEM_alloc(100);
	PORelType = (char	*)MEM_alloc(20);

	tc_strcpy(erc_dml,"");
   	tc_strcat(erc_dml,dml_no);
	printf("\n ERC DML No :: %s\n",erc_dml);

	tc_strcpy(ExeLogFile,"");
   	tc_strcat(ExeLogFile,"/user/rrr05503/Programs/ERC_DML_VCMS/");
   	tc_strcat(ExeLogFile,erc_dml);
   	tc_strcat(ExeLogFile,"_Log.log");
	printf("\n ERC DML Execution Log file is :: %s\n",ExeLogFile);
	DmlExecutionLog = fopen(ExeLogFile,"w");
		
	tc_strcpy(ResLogFile,"");
	tc_strcat(ResLogFile,"/user/rrr05503/Programs/ERC_DML_VCMS/");
   	tc_strcat(ResLogFile,erc_dml);
   	tc_strcat(ResLogFile,"_TCUA_VCMS_NOPOList.txt");
	NoPOResult = fopen(ResLogFile,"w");

	PrtCmvrReqStr =(char *) MEM_alloc(2);
	Dml_TPStr =(char *) MEM_alloc(5);
	TargetPlantMakeBuyNm =(char *) MEM_alloc(20);

	
	/// find Query ERC DML tag here 
	ITK_CALL(ITEM_find_item(erc_dml, &dml_tag));

	if(dml_tag != NULLTAG)
	{
		if(ITEM_ask_latest_rev(dml_tag, &DMLRevTag));
		if(TCTYPE_ask_object_type(DMLRevTag, &RevTypTag));
		if(TCTYPE_ask_name2(RevTypTag,&RevTyp));
		printf("\n RevTyp type: [%s]\n", RevTyp);fflush(stdout);
		fprintf(DmlExecutionLog,"DML RevTyp: %s\n",RevTyp); fflush(DmlExecutionLog);

		if(tc_strcmp(RevTyp,"ChangeRequestRevision")==0)
		{
			if (AOM_ask_value_string(DMLRevTag,"item_id",&dmlNumber));
			fprintf(DmlExecutionLog,"dmlNumber: %s ",dmlNumber); fflush(DmlExecutionLog);

			ITK_CALL(AOM_ask_value_string(DMLRevTag,"item_revision_id",&DMLRevId));
			fprintf(DmlExecutionLog," DMLRevId: [%s]", DMLRevId);fflush(DmlExecutionLog);

			ITK_CALL(AOM_ask_value_string(DMLRevTag,"t5_rlstype",&DMLtype));
			ITK_CALL(AOM_ask_value_string(DMLRevTag,"t5_cprojectcode",&DMLProj));
			ITK_CALL(AOM_ask_value_string(DMLRevTag,"t5_TargetPlant",&Dml_TP));
			ITK_CALL(AOM_UIF_ask_value(DMLRevTag,"owning_user",&DmlCreatorTmp));
			DmlCreatorTmp2=strtok(DmlCreatorTmp,")");
			DmlCreatorTmp3=strtok(DmlCreatorTmp2,"(");
			DmlCreator=strtok(NULL,"(");

			ITK_CALL(AOM_UIF_ask_value(DMLRevTag,"release_status_list",&DmlRlzStatus));

			if (tc_strcmp(DmlRlzStatus,"In Manager Review")==0)
			{
				DmlRlzStatus =(char *) MEM_alloc(5);
				tc_strcpy(DmlRlzStatus,"");
				tc_strcpy(DmlRlzStatus,"CcfIncorprtd");  //TCE DML LifeCycleState allow for VCMS only
			}

			fprintf(DmlExecutionLog," DMLtype: [%s]   DMLProj: [%s]   Dml_TP:[%s]   DmlRlzStatus: [%s]  DmlCreator: [%s]",DMLtype,DMLProj,Dml_TP,DmlRlzStatus,DmlCreator);
			fflush(DmlExecutionLog);
			
			ITK_CALL(GRM_find_relation_type("T5_CmHasNOPoDetails", &NOPO_dml_Ref_Rel));
			ITK_CALL(GRM_list_secondary_objects_only(DMLRevTag,NOPO_dml_Ref_Rel,&Refcount,&RefDoc_tag));
			fprintf(DmlExecutionLog,"\n Refcount Value :   %d",Refcount);fflush(stdout);
			if(Refcount>0)
			{
				for (jr=0;jr<Refcount;jr++ )
				{
					if(TCTYPE_ask_object_type(RefDoc_tag[jr],&RefTypeTag));
					if(TCTYPE_ask_name(RefTypeTag,Reftype_name));
					fprintf(DmlExecutionLog,"\n\n NOPO:Type: [%s]", Reftype_name);fflush(stdout);

					if((tc_strcmp(Reftype_name,"T5_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"NOPO Parts") ==0) || (tc_strcmp(Reftype_name,"T5_PORevMismatch_Parts") ==0) || (tc_strcmp(Reftype_name,"PO Rev Mismatch Parts") ==0) || (tc_strcmp(Reftype_name,"T5_New_NOPO_Parts") ==0) || (tc_strcmp(Reftype_name,"New NOPO Parts") ==0)|| (tc_strcmp(Reftype_name,"T5_PSC_PORevMismatch_Parts") ==0) || (tc_strcmp(Reftype_name,"P&SC PORevMismatch Parts") ==0))
					{
						tc_strcpy(PORelType,"");
						if((tc_strcmp(Reftype_name,"T5_NOPO_Parts") ==0)|| (tc_strcmp(Reftype_name,"NOPO Parts") ==0))
						{
							tc_strcpy(PORelType,"NOPO");
						}
						else if ((tc_strcmp(Reftype_name,"T5_PORevMismatch_Parts") ==0) || (tc_strcmp(Reftype_name,"PO Rev Mismatch Parts") ==0))
						{
							tc_strcpy(PORelType,"REVPOMISMATCH");
						}
						else if ((tc_strcmp(Reftype_name,"T5_New_NOPO_Parts") ==0) || (tc_strcmp(Reftype_name,"New NOPO Parts") ==0))
						{
							tc_strcpy(PORelType,"NwNoRvPrtNotAllow");
						}
						else if ((tc_strcmp(Reftype_name,"T5_PSC_PORevMismatch_Parts") ==0) || (tc_strcmp(Reftype_name,"P&SC PORevMismatch Parts") ==0))
						{
							tc_strcpy(PORelType,"PSCNoRvPrtNotAllow");
						}

						ITK_CALL(AOM_ask_value_string(RefDoc_tag[jr],"object_name",&sNOPOFldr));
						fprintf(DmlExecutionLog,"\t name: [%s]", sNOPOFldr);fflush(DmlExecutionLog);

						if(FL_ask_size(RefDoc_tag[jr],&ReqFdrSize));
						fprintf(DmlExecutionLog,"\t Size: [%d]",ReqFdrSize);fflush(stdout);
						if(FL_ask_references(RefDoc_tag[jr],FL_fsc_by_date_modified ,&FldContentCnt,&FoldContents_tag));
						fprintf(DmlExecutionLog,"\t FldContentCnt: [%d]",FldContentCnt);fflush(stdout);

						if(FldContentCnt>0)
						{
							k=0;
							for (k=0;k<FldContentCnt;k++ )
							{
								ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"item_id",&PrtNo));
								ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"item_revision_id",&PrtRevSeq));

								PrtRev=strtok(PrtRevSeq,";");
								PrtSeq=strtok(NULL,";");

								ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"object_desc",&PrtDescTmp));
								PrtDesc = replaceWord(PrtDescTmp, ",", " ");

								ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_HomologationReqd",&PrtHomo));

								ITK_CALL(AOM_ask_value_logical(FoldContents_tag[k],"t5_CMVRCertificationReqd",&PrtCmvrReq));
								tc_strcpy(PrtCmvrReqStr,"");
								if (PrtCmvrReq==0)
								{
									tc_strcpy(PrtCmvrReqStr,"-");
								}
								else
								{
									tc_strcpy(PrtCmvrReqStr,"+");
								}

								//ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],TargetPlantMakeBuyNm,&TargetPlantCS));
								tc_strcpy(Dml_TPStr,"");
								tc_strcpy(TargetPlantMakeBuyNm,"");
								if (tc_strcmp(Dml_TP,"PCVBU")==0) //PCVBU
								{
									tc_strcpy(Dml_TPStr,"1001");
									tc_strcpy(TargetPlantMakeBuyNm,"t5_PunMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_PunMakeBuyIndicator",&TargetPlantCS));
								}
								else if ((tc_strcmp(Dml_TP,"CARPLT")==0)||(tc_strcmp(Dml_TP,"CARPLANT")==0)) //CARPLANT
								{
									tc_strcpy(Dml_TPStr,"1100");
									tc_strcpy(TargetPlantMakeBuyNm,"t5_CarMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_CarMakeBuyIndicator",&TargetPlantCS));
								}
								else if (tc_strcmp(Dml_TP,"DWD")==0) //DHARWAD
								{
									tc_strcpy(Dml_TPStr,"1500");
									tc_strcpy(TargetPlantMakeBuyNm,"t5_DwdMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_DwdMakeBuyIndicator",&TargetPlantCS));
								}
								else if (tc_strcmp(Dml_TP,"PNR")==0) //PANTNAGAR
								{
									tc_strcpy(Dml_TPStr,"3100");
									tc_strcpy(TargetPlantMakeBuyNm,"t5_PnrMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_PnrMakeBuyIndicator",&TargetPlantCS));
								}
								else if (tc_strcmp(Dml_TP,"AHD")==0) //AHMEDABAD
								{
									tc_strcpy(Dml_TPStr,"7501");
									tc_strcpy(TargetPlantMakeBuyNm,"t5_AhdMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_AhdMakeBuyIndicator",&TargetPlantCS));
								}
								else if (tc_strcmp(Dml_TP,"JSR")==0) //JAMSHEDPUR
								{
									tc_strcpy(Dml_TPStr,"2001");
									tc_strcpy(TargetPlantMakeBuyNm,"t5_JsrMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_JsrMakeBuyIndicator",&TargetPlantCS));
								}
								else if (tc_strcmp(Dml_TP,"LKO")==0) //LUCKNOW
								{
									tc_strcpy(Dml_TPStr,"3001");
									tc_strcpy(TargetPlantMakeBuyNm,"t5_LkoMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_LkoMakeBuyIndicator",&TargetPlantCS));
								}
								else if (tc_strcmp(Dml_TP,"JDL")==0) //JSR DRIVELINES
								{
									tc_strcpy(TargetPlantMakeBuyNm,"t5_JdlMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_JdlMakeBuyIndicator",&TargetPlantCS));

									ITK_CALL(AOM_ask_value_string(DMLRevTag,"t5_DesignGroup",&DmlDesgnGrp));

									if ((tc_strcmp(DmlDesgnGrp,"32")==0) || (tc_strcmp(DmlDesgnGrp,"33")==0) || (tc_strcmp(DmlDesgnGrp,"35")==0) || (tc_strcmp(DmlDesgnGrp,"37")==0) || (tc_strcmp(DmlDesgnGrp,"39")==0) || (tc_strcmp(DmlDesgnGrp,"42")==0) || (tc_strcmp(DmlDesgnGrp,"43")==0))
									{
										tc_strcpy(Dml_TPStr,"2210");
									}
									else if ((tc_strcmp(DmlDesgnGrp,"26")==0) || (tc_strcmp(DmlDesgnGrp,"27")==0) || (tc_strcmp(DmlDesgnGrp,"28")==0))
									{
										tc_strcpy(Dml_TPStr,"2220");
									}
									else
									{
										tc_strcpy(Dml_TPStr,"");
									}
								}
								else if (tc_strcmp(Dml_TP,"PUVBU")==0) //UV Plant
								{
									tc_strcpy(Dml_TPStr,"1140");
									tc_strcpy(TargetPlantMakeBuyNm,"t5_PunUVMakeBuyIndicator");
									ITK_CALL(AOM_ask_value_string(FoldContents_tag[k],"t5_PunUVMakeBuyIndicator",&TargetPlantCS));
								}

								if (tc_strlen(TargetPlantCS)==0)
								{
									TargetPlantCS =(char *) MEM_alloc(2);
									tc_strcpy(TargetPlantCS,"");
									tc_strcpy(TargetPlantCS,"-");
								}
								fprintf(DmlExecutionLog,"\n\t prtcnt: %d	 NoPOPrtNo: %s,%s,%s,[%s],%s,%s,%s,%s,%s,%s,%s,%s,%s,",k,PrtNo,PrtRev,PrtSeq,PrtDesc,PrtHomo,PrtCmvrReqStr,DMLProj,DmlCreator,/*DmlAuthBy*/DmlCreator,Dml_TPStr,TargetPlantCS,PORelType,DmlRlzStatus);fflush(DmlExecutionLog);
								fprintf(NoPOResult,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n",PrtNo,PrtRev,PrtSeq,PrtDesc,PrtHomo,PrtCmvrReqStr,DMLProj,DmlCreator,/*DmlAuthBy*/DmlCreator,Dml_TPStr,TargetPlantCS,PORelType,DmlRlzStatus); fflush(NoPOResult);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		fprintf(DmlExecutionLog,"\nDML tag not found....Pl check input.."); fflush(DmlExecutionLog);
	}
	fprintf(DmlExecutionLog,"\n"); fflush(DmlExecutionLog);
	
	return iStatus;
}
