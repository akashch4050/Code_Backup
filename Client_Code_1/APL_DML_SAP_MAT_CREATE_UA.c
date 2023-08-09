#define TE_MAXLINELEN  128

#define _CLMAIN_DEFNS

#include <ae/dataset.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ctype.h>
#include <fclasses/tc_string.h>
#include <itk/mem.h>
#include <pom/pom/pom.h>
#include <ps/ps.h>
#include <ps/ps_errors.h>
#include <rdv/arch.h>
#include <res/res_itk.h>
#include <sa/sa.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tccore/libtccore_exports.h>
#include <tccore/libtccore_undef.h>
#include <tccore/tctype.h>
#include <tccore/uom.h>
#include <tccore/workspaceobject.h>
#include <tcinit/tcinit.h>
#include <textsrv/textserver.h>
#include <unidefs.h>
#include <string.h>
#include <user_exits/user_exits.h>
//#include <librfc.a>
//#include <ProdSap_functions.a>
//#include "VrCre.h"
#include "wmhelpe.h"
//#include "ppe.h"
#include "bapi.h"


int FetchPlantSpecificData(char* DMLNumber,char* ProjCode);
void displaying_objects(void);
void cll_zbapi_material_savedata_mrp(void);
RFC_RC cll_ccap_ecn_create(RFC_HANDLE);
RFC_RC ccap_ecn_create(RFC_HANDLE hRfc,AENR_API01 *,CSDATA_XFELD *,CSDATA_XFELD *,CSDATA_XFELD *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AENV_API01 *,AEEF_API01 *,AENRB_AENNR *,ITAB_H,ITAB_H,ITAB_H,ITAB_H,ITAB_H,char *);
RFC_RC zbapi_material_savedata_mrp(RFC_HANDLE hRfc,BAPIMATHEAD *,BAPI_MARA *,BAPI_MARAX *,BAPI_MARC *,BAPI_MARCX *,BAPI_MPGD *,BAPI_MPGDX *,BAPI_MARD *,BAPI_MARDX *,BAPI_MBEW *,BAPI_MBEWX *,RMMG1_AENNR *,BAPIRET2 *,ITAB_H ,ITAB_H ,ITAB_H,ITAB_H ,ITAB_H ,char *);
RFC_RC zbapi_material_savedata_mrpCreate(RFC_HANDLE hRfc,BAPIMATHEAD *eBapiMatHead,BAPI_MARA	*eBasicView,BAPI_MARAX	*eBasicViewx,BAPI_MARC	*eMrpView,BAPI_MARCX	*eMrpViewx,BAPI_MPGD	*eBapi_Mpgd,BAPI_MPGDX	*eBapi_Mpgdx,BAPI_MARD	*eBapi_Mard,BAPI_MARDX	*eBapi_Mardx,BAPI_MBEW	*eAccView,BAPI_MBEWX	*eAccViewx,RMMG1_AENNR *eRmmg1_Aennr,BAPIRET2 *eBapiret2,ITAB_H thBapi_Makt,ITAB_H thBapi_Marm,ITAB_H thBapi_Marmx,ITAB_H thBAPIE1PAREX3,ITAB_H thBAPIE1PAREXX3,char *xException);
RFC_RC allocate_insp_type(void);
RFC_RC allocate_insp_type_vc(void);
RFC_RC cll_zrfc_insptype_alloc(RFC_HANDLE);
RFC_RC vc_zrfc_insptype_alloc(RFC_HANDLE );

FILE* fsuccess;
FILE* fp=NULL;
FILE* finsp;
FILE* fval;

char fsuccess_name[200];
char fisa_name[200];
char fvac_name[200];

WERKS_D eWerks;
static MATNR eMatnr;

tag_t		*TaskRevision		= NULLTAG;
tag_t		TaskRevTag			= NULLTAG;
tag_t		*PartTags			= NULLTAG;

int tcount=0,pcount=0,TaskCnt=0,PartCnt=0;

int apl_dml_flag = 0;
int car_dml = 0;
int go_for_rfc = 0;
int flag_vc_v_tpl = 0;
int flag = 0;
int acc_flag = 0;

char *part_noDupDes=NULL;
char *part_noDupDesx=NULL;
char *doc_no=NULL;
char *basic_division=NULL;
char *unit_wt=NULL;
char *volume=NULL;
char *vol_unit=NULL;
char *material_group=NULL;
char *doc_noDup=NULL;
char *dwg_typeDup=NULL;
char *tmpDrwNum1=NULL;
char *tmpDrwNum=NULL;
char *tmpDrwRev=NULL;
char *tmpDrwSeq=NULL;
char *dwg_revDup=NULL;
char *dwg_rev=NULL;
char *sizeDup=NULL;
char *sheet_noDup=NULL;
char *OldMatNoDup=NULL;
char *net_wt=NULL;
char *net_wtDup=NULL;
char *gross_wt=NULL;
char *part_noDup=NULL;
char *PartMakeBuyInd=NULL;
char *make_buy_indDup=NULL;
char *make_buy_ind=NULL;
char *mat_type=NULL;
char *store_loc=NULL;
char *store_locDup=NULL;
char *mrp_grp=NULL;
char *mrp_type=NULL;
char *abc_ind=NULL;
char *series=NULL;
char *odstatus=NULL;
char *num;
char *drstatus = NULL;
char *drstatusDup = NULL;
char *mm_pp_status = NULL;
char *partClrIndDup = NULL;
char *partClrInd = NULL;
char *reord_pt = NULL;
char *quota_arrangement_usage = NULL;
char *mrp_controller = NULL;
char *lot_size_key = NULL;
char *fixed_lot_size = NULL;
char *max_stlvl = NULL;
char *blk_ind = NULL;
char *sched_mar_key = NULL;
char *req_grp = NULL;
char *period_ind = NULL;
char *mixedmrp = NULL;
char *cs1_val = NULL;
char *alternativb = NULL;
char *avail_chk = NULL;
char *splan_mat = NULL;
char *splan_plant = NULL;
char *splan_conv_fact = NULL;
char *ind_collect = NULL;
char *rep_mfg_in = NULL;
char *rep_mfg = NULL;
char *stock_det_group = NULL;
char *uoissue = NULL;
char *qua_ins_ind = NULL;
char *doc_req = NULL;
char *grptime = NULL;
char *certificate_type = NULL;
char *qm_proc_ind = NULL;
char *control_key = NULL;
char *catalog_prof = NULL;
char *valuation_cat = NULL;
char *price_con = NULL;
char *std_price = NULL;
char *moving_avg_price = NULL;
char *val_class = NULL;
char *mat_origin = NULL;
char *cost_lot_size = NULL;
char *variance_key = NULL;
char *with_quantity_structure = NULL;
char *costing_split_valuation = NULL;
char *costing_val_class = NULL;
char *MatCrChFlag = NULL;
char *PrtRevDup = NULL;
char *OrgIDDup = NULL;
char *plan_calendar = NULL;
char *profit_centre_sap = NULL;
char *overhd_grp = NULL;
char *myzeroDup3 = NULL;
char *myzeroDup1 = NULL;
char *myzeroDup2 = NULL;
char *myzeroDup4 = NULL;
char *sap_msg = NULL;
char *p;


char *plantcode=NULL;
char *Plant_MakeBuy=NULL;
char *Plant_StoreLoc=NULL;
char *profit_centre2=NULL;
char *plan_calendar2=NULL;
char *overhd_grp2=NULL;
char *origin_group2=NULL;
char *origin_group=NULL;
char *meas_unit=NULL;
char *desc=NULL;
char *descDup=NULL;
char *DMLDescription=NULL;
char *apl_release_date=NULL;
char *drg_ind=NULL;
char *drg_indDup=NULL;
char *docClass=NULL;
char *spl_proc_key=NULL;
char *OwnerNameDup=NULL;
char *OwnerName=NULL;
char *proc_type;

char *inputDml=NULL;
char *iPlantCode=NULL;


char *sap_proc_type=NULL;
char *sap_spproc_type=NULL;
char *SAPpstat=NULL;
char *MRPpstat=NULL;
char pstat;
char pstat_mrp;
char pstat_acc;
char proc_Key;
char spl_Key;
char group;

char dml_no_arg[14]={0};
char dml_numAP1[14]={0};
//drwnum = (char *)malloc(500 * sizeof(char));


#define GETCHAR(var,str) sprintf(str,"%.*s",(int)sizeof(var),var)
#define GETNUM(var,str) sprintf(str,"%.*s",sizeof(var),var);
#define SETDATE(var,str)memcpy(var,str,__min(strlen(str),sizeof(var)));if(sizeof(var)>strlen(str))memset(var+strlen(str),'0',sizeof(var)-strlen(str))
#define CONNECT_FAIL (EMH_USER_error_base + 2)
void rfc_error(char *);
//#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) {  return ifail;}


#define ITK_CALL(X) 							\
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
			printf("%3d error(s) with #X\n", n_ifails);						\
			for( index=0; index<n_ifails; index++)							\
			{																\
				printf("\tError #%d, %s\n", ifails[index], texts[index]);	\
			}																\
			return status;													\
		}																	\
	;

//void F_OUTK(const char *text,const unsigned pos,const unsigned len,char*str)
//{
//  printf("%*.0s",pos,text);fflush(stdout); printf("%-*s : %s\n",len,text,str);fflush(stdout);
//  fprintf(fsuccess,"%*.0s",pos,text); fprintf(fsuccess,"%-*s : %s\n",len,text,str);
//  return;
//}

char* subString(char* mainStringf ,int fromCharf ,int toCharf)
{
      int i;
      char *retStringf;
      retStringf = (char*) malloc(toCharf+1);
      for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
      *(retStringf+i) = '\0';
      return retStringf;
}

void displaying_objects(void)
{
	static RFC_HANDLE hRfc;
	static RFC_RC RfcRc;

	hRfc = BapiLogon();
	printf("\ncalling the change no creation function");
	RfcRc = cll_ccap_ecn_create(hRfc);
	RfcClose(hRfc);
}

RFC_RC cll_ccap_ecn_create(RFC_HANDLE hRfc)
{
	static RFC_RC RfcRc;
	int status;
	int Cnt=0,pCnt=0;
	char *PartNumC=NULL;
	char *part_typeC=NULL;

	tag_t PartTag1= NULLTAG;

	AENR_API01 eChangeHeader;
	CSDATA_XFELD eFlAle;
	CSDATA_XFELD eFlCommitAndWait;
	CSDATA_XFELD eFlNoCommitWork;
	AENV_API01 eObjectBom;
	AENV_API01 eObjectBomCus;
	AENV_API01 eObjectBomDoc;
	AENV_API01 eObjectBomEqui;
	AENV_API01 eObjectBomLoc;
	AENV_API01 eObjectBomMat;
	AENV_API01 eObjectBomPsp;
	AENV_API01 eObjectBomStd;
	AENV_API01 eObjectChar;
	AENV_API01 eObjectCls;
	AENV_API01 eObjectClsMaint;
	AENV_API01 eObjectConfProf;
	AENV_API01 eObjectDep;
	AENV_API01 eObjectDoc;
	AENV_API01 eObjectHazmat;
	AENV_API01 eObjectMat;
	AENV_API01 eObjectPhrase;
	AENV_API01 eObjectPvs;
	AENV_API01 eObjectPvsAlt;
	AENV_API01 eObjectPvsRel;
	AENV_API01 eObjectPvsVar;
	AENV_API01 eObjectSubstance;
	AENV_API01 eObjectTlist;
	AENV_API01 eObjectTlist2;
	AENV_API01 eObjectTlistA;
	AENV_API01 eObjectTlistE;
	AENV_API01 eObjectTlistM;
	AENV_API01 eObjectTlistN;
	AENV_API01 eObjectTlistQ;
	AENV_API01 eObjectTlistR;
	AENV_API01 eObjectTlistS;
	AENV_API01 eObjectTlistT;
	AENV_API01 eObjectValidMatvers;
	AENV_API01 eObjectVarTab;
	AEEF_API01 eValueAssign;
	AENRB_AENNR iChangeNo;
	ITAB_H thAltDates = ITAB_NULL;
	ITAB_H thEffectivity = ITAB_NULL;
	ITAB_H thObjmgrec = ITAB_NULL;
	ITAB_H thTextheader = ITAB_NULL;
	ITAB_H thTextlines = ITAB_NULL;
	char xException[256];

	AEOI_API01 *tObjmgrec;

	//tc_strdup("29.09.2018",&apl_release_date); //Temp Date

	printf("\nChangeNo : %s ",dml_no_arg);
	printf("\nDMLDescription : %s",DMLDescription);
	printf("\nERC_RELEASE_DATE : %s",apl_release_date);
	
	SETCHAR(eChangeHeader.ChangeNo,dml_no_arg);
	SETNUM(eChangeHeader.Status,"01");
	SETCHAR(eChangeHeader.AuthGroup,"");

	SETCHAR(eChangeHeader.ValidFrom,apl_release_date);
	SETCHAR(eChangeHeader.Descript,DMLDescription);
	SETCHAR(eChangeHeader.ReasonChg,"");
	SETCHAR(eChangeHeader.DeletionMark,"");
	SETCHAR(eChangeHeader.IndateRule,"");
	SETCHAR(eChangeHeader.OutdateRule,"");
	SETCHAR(eChangeHeader.Function,"");
	SETCHAR(eChangeHeader.ChangeLeader,"");
	SETCHAR(eChangeHeader.EffectivityType,"");
	SETCHAR(eChangeHeader.OverridingMark,"");
	SETNUM(eChangeHeader.Rank,"");
	SETNUM(eChangeHeader.ReleaseKey,"");
	SETCHAR(eChangeHeader.StatusProfile,"");
	SETCHAR(eChangeHeader.TechRel,"");
	SETCHAR(eChangeHeader.BasicChange,"");
	SETCHAR(eFlAle,"");
	SETCHAR(eFlCommitAndWait,"X");
	SETCHAR(eFlNoCommitWork,"");
	SETCHAR(eObjectBom.Active,"");
	SETCHAR(eObjectBom.Locked,"");
	SETCHAR(eObjectBom.ObjRequ,"");
	SETCHAR(eObjectBom.MgtrecGen,"");
	SETCHAR(eObjectBom.GenNew,"");
	SETCHAR(eObjectBom.GenDialog,"");
	SETCHAR(eObjectBomCus.Active,"");
	SETCHAR(eObjectBomCus.Locked,"");
	SETCHAR(eObjectBomCus.ObjRequ,"");
	SETCHAR(eObjectBomCus.MgtrecGen,"");
	SETCHAR(eObjectBomCus.GenNew,"");
	SETCHAR(eObjectBomCus.GenDialog,"");
	SETCHAR(eObjectBomDoc.Active,"");
	SETCHAR(eObjectBomDoc.Locked,"");
	SETCHAR(eObjectBomDoc.ObjRequ,"");
	SETCHAR(eObjectBomDoc.MgtrecGen,"");
	SETCHAR(eObjectBomDoc.GenNew,"");
	SETCHAR(eObjectBomDoc.GenDialog,"");
	SETCHAR(eObjectBomEqui.Active,"");
	SETCHAR(eObjectBomEqui.Locked,"");
	SETCHAR(eObjectBomEqui.ObjRequ,"");
	SETCHAR(eObjectBomEqui.MgtrecGen,"");
	SETCHAR(eObjectBomEqui.GenNew,"");
	SETCHAR(eObjectBomEqui.GenDialog,"");
	SETCHAR(eObjectBomLoc.Active,"");
	SETCHAR(eObjectBomLoc.Locked,"");
	SETCHAR(eObjectBomLoc.ObjRequ,"");
	SETCHAR(eObjectBomLoc.MgtrecGen,"");
	SETCHAR(eObjectBomLoc.GenNew,"");
	SETCHAR(eObjectBomLoc.GenDialog,"");
	SETCHAR(eObjectBomMat.Active,"X");
	SETCHAR(eObjectBomMat.Locked,"");
	SETCHAR(eObjectBomMat.ObjRequ,"X");
	SETCHAR(eObjectBomMat.MgtrecGen,"X");
	SETCHAR(eObjectBomMat.GenNew,"");
	SETCHAR(eObjectBomMat.GenDialog,"");
	SETCHAR(eObjectBomPsp.Active,"");
	SETCHAR(eObjectBomPsp.Locked,"");
	SETCHAR(eObjectBomPsp.ObjRequ,"");
	SETCHAR(eObjectBomPsp.MgtrecGen,"");
	SETCHAR(eObjectBomPsp.GenNew,"");
	SETCHAR(eObjectBomPsp.GenDialog,"");
	SETCHAR(eObjectBomStd.Active,"");
	SETCHAR(eObjectBomStd.Locked,"");
	SETCHAR(eObjectBomStd.ObjRequ,"");
	SETCHAR(eObjectBomStd.MgtrecGen,"");
	SETCHAR(eObjectBomStd.GenNew,"");
	SETCHAR(eObjectBomStd.GenDialog,"");
	SETCHAR(eObjectChar.Active,"");
	SETCHAR(eObjectChar.Locked,"");
	SETCHAR(eObjectChar.ObjRequ,"");
	SETCHAR(eObjectChar.MgtrecGen,"");
	SETCHAR(eObjectChar.GenNew,"");
	SETCHAR(eObjectChar.GenDialog,"");
	SETCHAR(eObjectCls.Active,"");
	SETCHAR(eObjectCls.Locked,"");
	SETCHAR(eObjectCls.ObjRequ,"");
	SETCHAR(eObjectCls.MgtrecGen,"");
	SETCHAR(eObjectCls.GenNew,"");
	SETCHAR(eObjectCls.GenDialog,"");
	SETCHAR(eObjectClsMaint.Active,"");
	SETCHAR(eObjectClsMaint.Locked,"");
	SETCHAR(eObjectClsMaint.ObjRequ,"");
	SETCHAR(eObjectClsMaint.MgtrecGen,"");
	SETCHAR(eObjectClsMaint.GenNew,"");
	SETCHAR(eObjectClsMaint.GenDialog,"");
	SETCHAR(eObjectConfProf.Active,"");
	SETCHAR(eObjectConfProf.Locked,"");
	SETCHAR(eObjectConfProf.ObjRequ,"");
	SETCHAR(eObjectConfProf.MgtrecGen,"");
	SETCHAR(eObjectConfProf.GenNew,"");
	SETCHAR(eObjectConfProf.GenDialog,"");
	SETCHAR(eObjectDep.Active,"");
	SETCHAR(eObjectDep.Locked,"");
	SETCHAR(eObjectDep.ObjRequ,"");
	SETCHAR(eObjectDep.MgtrecGen,"");
	SETCHAR(eObjectDep.GenNew,"");
	SETCHAR(eObjectDep.GenDialog,"");
	SETCHAR(eObjectDoc.Active,"X");
	SETCHAR(eObjectDoc.Locked,"");
	SETCHAR(eObjectDoc.ObjRequ,"X");
	SETCHAR(eObjectDoc.MgtrecGen,"X");
	SETCHAR(eObjectDoc.GenNew,"");
	SETCHAR(eObjectDoc.GenDialog,"");
	SETCHAR(eObjectHazmat.Active,"");
	SETCHAR(eObjectHazmat.Locked,"");
	SETCHAR(eObjectHazmat.ObjRequ,"");
	SETCHAR(eObjectHazmat.MgtrecGen,"");
	SETCHAR(eObjectHazmat.GenNew,"");
	SETCHAR(eObjectHazmat.GenDialog,"");
	SETCHAR(eObjectMat.Active,"X");
	SETCHAR(eObjectMat.Locked,"");
	SETCHAR(eObjectMat.ObjRequ,"X");
	SETCHAR(eObjectMat.MgtrecGen,"X");
	SETCHAR(eObjectMat.GenNew,"");
	SETCHAR(eObjectMat.GenDialog,"");
	SETCHAR(eObjectPhrase.Active,"");
	SETCHAR(eObjectPhrase.Locked,"");
	SETCHAR(eObjectPhrase.ObjRequ,"");
	SETCHAR(eObjectPhrase.MgtrecGen,"");
	SETCHAR(eObjectPhrase.GenNew,"");
	SETCHAR(eObjectPhrase.GenDialog,"");
	SETCHAR(eObjectPvs.Active,"");
	SETCHAR(eObjectPvs.Locked,"");
	SETCHAR(eObjectPvs.ObjRequ,"");
	SETCHAR(eObjectPvs.MgtrecGen,"");
	SETCHAR(eObjectPvs.GenNew,"");
	SETCHAR(eObjectPvs.GenDialog,"");
	SETCHAR(eObjectPvsAlt.Active,"");
	SETCHAR(eObjectPvsAlt.Locked,"");
	SETCHAR(eObjectPvsAlt.ObjRequ,"");
	SETCHAR(eObjectPvsAlt.MgtrecGen,"");
	SETCHAR(eObjectPvsAlt.GenNew,"");
	SETCHAR(eObjectPvsAlt.GenDialog,"");
	SETCHAR(eObjectPvsRel.Active,"");
	SETCHAR(eObjectPvsRel.Locked,"");
	SETCHAR(eObjectPvsRel.ObjRequ,"");
	SETCHAR(eObjectPvsRel.MgtrecGen,"");
	SETCHAR(eObjectPvsRel.GenNew,"");
	SETCHAR(eObjectPvsRel.GenDialog,"");
	SETCHAR(eObjectPvsVar.Active,"");
	SETCHAR(eObjectPvsVar.Locked,"");
	SETCHAR(eObjectPvsVar.ObjRequ,"");
	SETCHAR(eObjectPvsVar.MgtrecGen,"");
	SETCHAR(eObjectPvsVar.GenNew,"");
	SETCHAR(eObjectPvsVar.GenDialog,"");
	SETCHAR(eObjectSubstance.Active,"");
	SETCHAR(eObjectSubstance.Locked,"");
	SETCHAR(eObjectSubstance.ObjRequ,"");
	SETCHAR(eObjectSubstance.MgtrecGen,"");
	SETCHAR(eObjectSubstance.GenNew,"");
	SETCHAR(eObjectSubstance.GenDialog,"");
	SETCHAR(eObjectTlist.Active,"");
	SETCHAR(eObjectTlist.Locked,"");
	SETCHAR(eObjectTlist.ObjRequ,"");
	SETCHAR(eObjectTlist.MgtrecGen,"");
	SETCHAR(eObjectTlist.GenNew,"");
	SETCHAR(eObjectTlist.GenDialog,"");
	SETCHAR(eObjectTlist2.Active,"");
	SETCHAR(eObjectTlist2.Locked,"");
	SETCHAR(eObjectTlist2.ObjRequ,"");
	SETCHAR(eObjectTlist2.MgtrecGen,"");
	SETCHAR(eObjectTlist2.GenNew,"");
	SETCHAR(eObjectTlist2.GenDialog,"");
	SETCHAR(eObjectTlistA.Active,"");
	SETCHAR(eObjectTlistA.Locked,"");
	SETCHAR(eObjectTlistA.ObjRequ,"");
	SETCHAR(eObjectTlistA.MgtrecGen,"");
	SETCHAR(eObjectTlistA.GenNew,"");
	SETCHAR(eObjectTlistA.GenDialog,"");
	SETCHAR(eObjectTlistE.Active,"");
	SETCHAR(eObjectTlistE.Locked,"");
	SETCHAR(eObjectTlistE.ObjRequ,"");
	SETCHAR(eObjectTlistE.MgtrecGen,"");
	SETCHAR(eObjectTlistE.GenNew,"");
	SETCHAR(eObjectTlistE.GenDialog,"");
	SETCHAR(eObjectTlistM.Active,"");
	SETCHAR(eObjectTlistM.Locked,"");
	SETCHAR(eObjectTlistM.ObjRequ,"");
	SETCHAR(eObjectTlistM.MgtrecGen,"");
	SETCHAR(eObjectTlistM.GenNew,"");
	SETCHAR(eObjectTlistM.GenDialog,"");
	SETCHAR(eObjectTlistN.Active,"");
	SETCHAR(eObjectTlistN.Locked,"");
	SETCHAR(eObjectTlistN.ObjRequ,"");
	SETCHAR(eObjectTlistN.MgtrecGen,"");
	SETCHAR(eObjectTlistN.GenNew,"");
	SETCHAR(eObjectTlistN.GenDialog,"");
	SETCHAR(eObjectTlistQ.Active,"");
	SETCHAR(eObjectTlistQ.Locked,"");
	SETCHAR(eObjectTlistQ.ObjRequ,"");
	SETCHAR(eObjectTlistQ.MgtrecGen,"");
	SETCHAR(eObjectTlistQ.GenNew,"");
	SETCHAR(eObjectTlistQ.GenDialog,"");
	SETCHAR(eObjectTlistR.Active,"");
	SETCHAR(eObjectTlistR.Locked,"");
	SETCHAR(eObjectTlistR.ObjRequ,"");
	SETCHAR(eObjectTlistR.MgtrecGen,"");
	SETCHAR(eObjectTlistR.GenNew,"");
	SETCHAR(eObjectTlistR.GenDialog,"");
	SETCHAR(eObjectTlistS.Active,"");
	SETCHAR(eObjectTlistS.Locked,"");
	SETCHAR(eObjectTlistS.ObjRequ,"");
	SETCHAR(eObjectTlistS.MgtrecGen,"");
	SETCHAR(eObjectTlistS.GenNew,"");
	SETCHAR(eObjectTlistS.GenDialog,"");
	SETCHAR(eObjectTlistT.Active,"");
	SETCHAR(eObjectTlistT.Locked,"");
	SETCHAR(eObjectTlistT.ObjRequ,"");
	SETCHAR(eObjectTlistT.MgtrecGen,"");
	SETCHAR(eObjectTlistT.GenNew,"");
	SETCHAR(eObjectTlistT.GenDialog,"");
	SETCHAR(eObjectValidMatvers.Active,"");
	SETCHAR(eObjectValidMatvers.Locked,"");
	SETCHAR(eObjectValidMatvers.ObjRequ,"");
	SETCHAR(eObjectValidMatvers.MgtrecGen,"");
	SETCHAR(eObjectValidMatvers.GenNew,"");
	SETCHAR(eObjectValidMatvers.GenDialog,"");
	SETCHAR(eObjectVarTab.Active,"");
	SETCHAR(eObjectVarTab.Locked,"");
	SETCHAR(eObjectVarTab.ObjRequ,"");
	SETCHAR(eObjectVarTab.MgtrecGen,"");
	SETCHAR(eObjectVarTab.GenNew,"");
	SETCHAR(eObjectVarTab.GenDialog,"");
	SETCHAR(eValueAssign.ValidFrom,"");
	SETCHAR(eValueAssign.ValidTo,"");
	SETCHAR(eValueAssign.DateMark,"");
	SETCHAR(eValueAssign.Material,"");
	SETCHAR(eValueAssign.SerialnrLow,"");
	SETCHAR(eValueAssign.SerialnrHigh,"");
	SETCHAR(eValueAssign.Class,"");
	SETCHAR(eValueAssign.Classty,"");
	SETCHAR(eValueAssign.Startup,"");
	SETCHAR(eValueAssign.Plant,"");
	SETCHAR(eValueAssign.SernrOi,"");
	SETCHAR(eValueAssign.FlDelete,"");


    if (thAltDates==ITAB_NULL)
	{
		thAltDates = ItCreate("ALT_DATES", sizeof(AEDT_API01), 0, 0);
		if (thAltDates == ITAB_NULL)
			printf("\nItCreate ALT_DATES");
	}
    else if (ItFree(thAltDates) != 0)
		printf("\nItFree ALT_DATES");

    if (thEffectivity==ITAB_NULL)
	{
		thEffectivity = ItCreate("EFFECTIVITY", sizeof(AEEF_API01), 0, 0);
		if (thEffectivity == ITAB_NULL)
			printf("\nItCreate EFFECTIVITY");
	}
    else if (ItFree(thEffectivity) != 0)
		printf("\nItFree EFFECTIVITY");

	if (thObjmgrec==ITAB_NULL)
	{
		thObjmgrec = ItCreate("OBJMGREC", sizeof(AEOI_API01), 0, 0);
		if (thObjmgrec==ITAB_NULL)
			printf("\nItCreate OBJMGREC");
	}
    else if (ItFree(thObjmgrec) != 0)
		printf("\nItFree OBJMGREC");

    if (thTextheader==ITAB_NULL)
	{
		thTextheader = ItCreate("TEXTHEADER", sizeof(CCTHEAD), 0, 0);
		if (thTextheader==ITAB_NULL)
			printf("\nItCreate TEXTHEADER");
	}
    else if (ItFree(thTextheader) != 0)
		printf("\nItFree TEXTHEADER");

    if (thTextlines==ITAB_NULL)
	{
		thTextlines = ItCreate("TEXTLINES", sizeof(CCTLINE), 0, 0);
		if (thTextlines==ITAB_NULL)
			printf("\nItCreate TEXTLINES");
	}
    else if (ItFree(thTextlines) != 0)
		printf("\nItFree TEXTLINES");

	printf("\nTaskCnt in Displaying Object %d",tcount);
	//fprintf(fsuccess,"\nTaskCnt in Displaying Object %d",tcount);

	for (Cnt=0;Cnt<tcount ;Cnt++ )
	{
		TaskRevTag = TaskRevision[Cnt];
		pCnt=0;
		ITK_CALL(AOM_ask_value_tags(TaskRevTag,"CMHasSolutionItem",&pCnt,&PartTags));
		printf("\nTask To Part Count : %d",pCnt);fflush(stdout);
			
		if(pCnt>0)
		{
			for(PartCnt=0;PartCnt<pCnt;PartCnt++)
			{
				PartTag1=PartTags[PartCnt];
				ITK_CALL(AOM_ask_value_string(PartTag1,"item_id",&PartNumC));
				ITK_CALL(AOM_ask_value_string(PartTag1,"t5_PartType",&part_typeC));
			
				if(strcmp(part_typeC,"D")==0)	//Skipping Dummy Parts
				{	continue;	}

				printf("\nAdding partno As Part is %s %s", PartNumC,part_typeC);
				fprintf(fsuccess,"\nAdding partno As Part is %s %s", PartNumC,part_typeC);

				tObjmgrec = ItAppLine(thObjmgrec);
				if (tObjmgrec == NULL)
					printf("\nItAppLine OBJMGREC");

				SETCHAR(tObjmgrec->AltDate,"");
				SETNUM(tObjmgrec->ChgObjtyp,"");
				SETNUM(tObjmgrec->ChgObjtyp,"4");
				SETCHAR(tObjmgrec->BomCat,"");
				SETCHAR(tObjmgrec->BomStdObject,"");
				SETCHAR(tObjmgrec->BomUsage,"");
				SETNUM(tObjmgrec->Chgtypeobj,"");
				SETCHAR(tObjmgrec->DescrObj,"");
				SETCHAR(tObjmgrec->DocType,"");
				SETCHAR(tObjmgrec->DocNumber,"");
				SETCHAR(tObjmgrec->DocVers,"");
				SETCHAR(tObjmgrec->DocPart,"");
				SETCHAR(tObjmgrec->Equipment,"");
				SETCHAR(tObjmgrec->FuncLoc,"");
				SETCHAR(tObjmgrec->Material,PartNumC);
				SETCHAR(tObjmgrec->Plant,"");
				SETCHAR(tObjmgrec->PspElement,"");
				SETCHAR(tObjmgrec->PvsType,"");
				SETCHAR(tObjmgrec->PvsNode,"");
				SETCHAR(tObjmgrec->PvsClassNumber,"");
				SETCHAR(tObjmgrec->PvsClassType,"");
				SETCHAR(tObjmgrec->PvsVariant,"");
				SETCHAR(tObjmgrec->SdOrder,"");
				SETNUM(tObjmgrec->SdOrderI,"");
				SETNUM(tObjmgrec->Textkey,"");
				SETCHAR(tObjmgrec->TlistType,"");
				SETCHAR(tObjmgrec->TlistGrp,"");
				SETCHAR(tObjmgrec->ObjChglock,"");
				SETCHAR(tObjmgrec->StatusProfObj,"");
				SETCHAR(tObjmgrec->FlDelete,"");
			}
		}
	}
	RfcRc = ccap_ecn_create(hRfc,&eChangeHeader,&eFlAle,&eFlCommitAndWait,&eFlNoCommitWork,&eObjectBom,&eObjectBomCus,&eObjectBomDoc,&eObjectBomEqui,&eObjectBomLoc,&eObjectBomMat,&eObjectBomPsp,&eObjectBomStd,&eObjectChar,&eObjectCls,&eObjectClsMaint,&eObjectConfProf,&eObjectDep,&eObjectDoc,&eObjectHazmat,&eObjectMat,&eObjectPhrase,&eObjectPvs,&eObjectPvsAlt,&eObjectPvsRel,&eObjectPvsVar,&eObjectSubstance,&eObjectTlist,&eObjectTlist2,&eObjectTlistA,&eObjectTlistE,&eObjectTlistM,&eObjectTlistN,&eObjectTlistQ,&eObjectTlistR,&eObjectTlistS,&eObjectTlistT,&eObjectValidMatvers,&eObjectVarTab,&eValueAssign,&iChangeNo,thAltDates,thEffectivity,thObjmgrec,thTextheader,thTextlines,xException);
	switch (RfcRc)
	{
		case RFC_OK:
			printf("\necn create: %u",RFC_OK);
			fprintf(fsuccess,"\necn create: %u",RFC_OK);
		break;
		case RFC_EXCEPTION :
			printf("\nRFC Exception: %s",xException);
			fprintf(fsuccess,"\nRFC Exception: %s",xException);
		break;
		default:;
	}
	EXIT:
	printf("exit from bapi");
	return RfcRc;
}

FetchPlantSpecificData(char* DMLNumber,char* ProjCode)
{
	char	*DMLNo			= NULL;
	char	*PlantSuffix	= NULL;
	char	*SAPLivFlg		= NULL;
	
	int     n_entry			= 3;
	int     p_entry			= 2;
	int		ContObjCnt		= 0;

	tag_t   PlantCodeQryTag     = NULLTAG;
	tag_t   MakeBuyQryTag		= NULLTAG;
	tag_t   PlantDataQryTag     = NULLTAG;
	tag_t   *ContorlObjTag     = NULLTAG;

	char    *qry_entryApl[3]  = {"SYSCD","SUBSYSCD","Information-1"};
	char    *qry_entryStd[3]  = {"SYSCD","SUBSYSCD","Information-2"};

	char    *qry_entryMake[2] = {"SYSCD","SUBSYSCD"};

	DMLNo = strtok(DMLNumber, "_" );
	PlantSuffix  = strtok ( NULL, "_" );
	
	tc_strcpy(dml_no_arg,DMLNo);
	tc_strcpy(dml_numAP1,DMLNo);

	printf("\nDMLNo [%s]	PlantSuffix [%s]	Project Code [%s]", DMLNo,PlantSuffix,ProjCode);

	char	*qry_value[3] = {"PlantDML",ProjCode,PlantSuffix};
	
	if(QRY_find("Control Objects...", &PlantCodeQryTag));

	if(PlantCodeQryTag)
	{
		printf("\nFound Query : Control Objects...\n"); fflush(stdout);

		if(QRY_execute(PlantCodeQryTag,n_entry,qry_entryApl,qry_value,&ContObjCnt,&ContorlObjTag));

		if (ContObjCnt==0)
		{
			if(QRY_execute(PlantCodeQryTag,n_entry,qry_entryStd,qry_value,&ContObjCnt,&ContorlObjTag));
		}

		if(ContObjCnt >0)
		{
			AOM_ask_value_string(ContorlObjTag[0],"t5_Userinfo5",&plantcode);
			AOM_ask_value_string(ContorlObjTag[0],"t5_Userinfo6",&SAPLivFlg);
			printf("\nPlantSuffix [%s]	Plant Code [%s]",PlantSuffix,plantcode);
		}
		else
		{
			printf("\nProjectCode [%s] do not have entry in Control Object PlantDML for Suffix [%s] hence exiting!",ProjCode,PlantSuffix); fflush(stdout);
			goto CLEANUP;
		}

		char    *qry_MakeValue[2]  = {"MAKEBUY",plantcode};
		
		if(QRY_execute(PlantCodeQryTag,p_entry,qry_entryMake,qry_MakeValue,&ContObjCnt,&ContorlObjTag));

		if(ContObjCnt >0)
		{
			AOM_ask_value_string(ContorlObjTag[0],"t5_Userinfo1",&Plant_StoreLoc);
			AOM_ask_value_string(ContorlObjTag[0],"t5_Userinfo2",&Plant_MakeBuy);
			printf("\nPlant Code [%s]	Plant_MakeBuy [%s]	Plant_StoreLoc [%s]",plantcode,Plant_MakeBuy,Plant_StoreLoc);
		}
		else
		{
			printf("\nPlantcode [%s] do not have entry in Control Object MAKEBUY hence exiting!",plantcode); fflush(stdout);
			goto CLEANUP;
		}



		char    *qry_PlantValue[2]  = {"PLANTDATA",plantcode};

		if(QRY_execute(PlantCodeQryTag,p_entry,qry_entryMake,qry_PlantValue,&ContObjCnt,&ContorlObjTag));

		if(ContObjCnt >0)
		{
			AOM_ask_value_string(ContorlObjTag[0],"t5_Userinfo1",&profit_centre2);
			AOM_ask_value_string(ContorlObjTag[0],"t5_Userinfo2",&plan_calendar2);
			AOM_ask_value_string(ContorlObjTag[0],"t5_Userinfo3",&overhd_grp2);
			AOM_ask_value_string(ContorlObjTag[0],"t5_Userinfo4",&origin_group2);

			tc_strcpy(profit_centre_sap,"000");
			tc_strcat(profit_centre_sap,profit_centre2);
			tc_strdup(plan_calendar2,&plan_calendar);
			tc_strdup(overhd_grp2,&overhd_grp);
			tc_strdup(origin_group2,&origin_group);

			printf("\nplantcode      :[%s]",plantcode);
			printf("\nprofit_centre  :[%s]",profit_centre_sap);
			printf("\nplan_calendar  :[%s]",plan_calendar);
			printf("\noverhd_grp     :[%s]",overhd_grp);
			printf("\norigin_group   :[%s]",origin_group);
		}
		
		if (tc_strlen(PlantSuffix)>3)
		{
			dml_no_arg[10] = PlantSuffix[3];
			dml_no_arg[11] = 'P';
			dml_no_arg[12] = '\0';

			dml_numAP1[10] = PlantSuffix[3];
			dml_numAP1[11] = 'P';
			dml_numAP1[12] = '\0';
		}
		else
		{
			tc_strcat(dml_no_arg,"AP");
			tc_strcat(dml_numAP1,"AP");
		}
	}

	CLEANUP:
	return ContObjCnt;
}

RFC_RC allocate_insp_type(void)
{
	static RFC_HANDLE hRfc;
	static RFC_RC RfcRc;

	hRfc = BapiLogon();
	printf("\nReturned value = %u", hRfc);
	if (hRfc == RFC_HANDLE_NULL)
		printf("\nRfcOpen");
	if (hRfc == 0)
	{
		printf("\nRFC connection is not present.");
	}
	else
		RfcRc = cll_zrfc_insptype_alloc(hRfc);
	//fprintf(fsuccess,"\n******************************************************************");
	RfcClose(hRfc);
	printf("\nRFC connection is closed");
	return RfcRc;
}


//zrfc_insptype_alloc(RFC_HANDLE hRfc,MATNR *eMatnr,QPART *eQpart,WERKS_D *eWerks,MESSAGEINF *iMessg,SYST_SUBRC *iRetval,char *xException)
//{
//	RFC_PARAMETER Exporting[4];
//	RFC_PARAMETER Importing[3];
//	RFC_TABLE Tables[1];
//	RFC_RC RfcRc;
//	char *RfcException = NULL;
//
//	Exporting[0].name = "MATNR";
//	Exporting[0].nlen = 5;
//	Exporting[0].type = handleOfMATNR;
//	Exporting[0].leng = sizeof(MATNR);
//	Exporting[0].addr = eMatnr;
//
//	Exporting[1].name = "QPART";
//	Exporting[1].nlen = 5;
//	Exporting[1].type = handleOfQPART;
//	Exporting[1].leng = sizeof(QPART);
//	Exporting[1].addr = eQpart;
//
//	Exporting[2].name = "WERKS";
//	Exporting[2].nlen = 5;
//	Exporting[2].type = handleOfWERKS_D;
//	Exporting[2].leng = sizeof(WERKS_D);
//	Exporting[2].addr = eWerks;
//
//	Exporting[3].name = NULL;
//
//	Tables[0].name = NULL;
//
//	RfcRc = RfcCall(hRfc,"ZRFC_INSPTYPE_ALLOC",Exporting,Tables);
//
//	switch (RfcRc)
//	{
//	  	case RFC_OK :
//			Importing[0].name = "MESSG";
//			Importing[0].nlen = 5;
//			Importing[0].type = handleOfMESSAGEINF;
//			Importing[0].leng = sizeof(MESSAGEINF);
//			Importing[0].addr = iMessg;
//
//			Importing[1].name = "RETVAL";
//			Importing[1].nlen = 6;
//			Importing[1].type = TYPINT;
//			Importing[1].leng = sizeof(SYST_SUBRC);
//			Importing[1].addr = iRetval;
//
//			Importing[2].name = NULL;
//			RfcRc = RfcReceive(hRfc,Importing,Tables,&RfcException);
//			switch (RfcRc)
//			{
//	  			case RFC_SYS_EXCEPTION:
//					strcpy(xException,RfcException);
//					break;
//				case RFC_EXCEPTION:
//					strcpy(xException,RfcException);
//					break;
//			}
//	}
//	return RfcRc;
//}

void get_origingroup(void)
{
	char *dw;
	int partlen;

	partlen = strcspn(part_noDup, " ");
	dw = subString(part_noDup,0,2);


	if (tc_strcmp(make_buy_indDup, "F  ") == 0)
	{
		if (car_dml == 1)
		{
			strcpy(origin_group,origin_group2);
		}
		if (car_dml == 0)
		{
			strcpy(origin_group,origin_group2);

		}
		if ((partlen == 11) && tc_strcmp(dw, "55") == 0)
		{
			strcpy(origin_group,origin_group2);
		}

		if (tc_strcmp(mat_type, "ROH") == 0)
		{
			strcpy(origin_group,origin_group2);
		}
	}
	else
	{
		strcpy(origin_group,origin_group2);
	}
}


RFC_RC export_CSpastat(RFC_HANDLE hRfc,MATNR *eMatnr,WERKS_D* eWerks,PLANTDATA* iMrpView, CLIENTDATA* iView, char *xException)
{
	RFC_RC RfcRc;
	RFC_PARAMETER Exporting[3];
	RFC_PARAMETER Importing[3];
	RFC_TABLE Tables[1];
	char *RfcException = NULL;

	Exporting[0].name = "MATERIAL";
	Exporting[0].nlen = 8;
	Exporting[0].type = handleOfMATNR;
	Exporting[0].leng = sizeof(MATNR);
	Exporting[0].addr = eMatnr;

	Exporting[1].name = "PLANT";
	Exporting[1].nlen = 5;
	Exporting[1].type = handleOfWERKS_D;
	Exporting[1].leng = sizeof(WERKS_D);
	Exporting[1].addr = eWerks;

	Exporting[2].name = NULL;


	Tables[0].name = NULL;

	RfcRc = RfcCall(hRfc,"BAPI_MATERIAL_GETALL",Exporting,Tables);

	switch (RfcRc)
	{
	  	case RFC_OK :

			Importing[0].name = "CLIENTDATA";
			Importing[0].nlen = 10;
			Importing[0].type = handleOfCLIENTDATA;
			Importing[0].leng = sizeof(CLIENTDATA);
			Importing[0].addr = iView;

			Importing[1].name = "PLANTDATA";
			Importing[1].nlen = 9;
			Importing[1].type = handleOfPLANTDATA;
			Importing[1].leng = sizeof(PLANTDATA);
			Importing[1].addr = iMrpView;

			Importing[2].name = NULL;

			RfcRc = RfcReceive(hRfc,Importing,Tables,&RfcException);

			switch (RfcRc)
			{
	  			case RFC_SYS_EXCEPTION:
					strcpy(xException,RfcException);
					break;
				case RFC_EXCEPTION:
					strcpy(xException,RfcException);
					break;
				default: ;
			}
		default: ;
	}
	return RfcRc;
}

int GetCSPstat(/*char cMaterial[15]*/void)
{
char *xException=NULL;
RFC_HANDLE hRfc;
RFC_RC RfcRc;
MATNR eMatnr;
WERKS_D eWerks;
PLANTDATA iMrpView;
CLIENTDATA iView;

printf("\nProcessing GetCSPstat:[%s,%s]",part_noDup,plantcode);

hRfc = BapiLogon();

//strcpy(sap_proc_type,"");
//strcpy(sap_spproc_type,"");
strcpy(SAPpstat,"");
strcpy(MRPpstat,"");

//SETCHAR(eMatnr.Matnr,cMaterial);
SETCHAR(eMatnr.Matnr,part_noDup);
SETCHAR(eWerks.WerksD,plantcode);


RfcRc = export_CSpastat(hRfc
				,&eMatnr
				,&eWerks
				,&iMrpView
				,&iView
				,xException);

switch (RfcRc)
{
	case RFC_OK:
		//printf("\n In RFC_OK case...\n");	fflush(stdout);
		//GETCHAR(iMrpView.PROC_TYPE, proc_type1);		OUTS("PROC_TYPE",10,30,proc_type1);
		//GETCHAR(iMrpView.SPPROCTYPE, spproc_type);		OUTS("SPPROCTYPE",10,30,spproc_type);

		GETCHAR(iMrpView.PROC_TYPE, sap_proc_type);		//OUTS("PROC_TYPE",10,30,sap_proc_type);
		GETCHAR(iMrpView.SPPROCTYPE, sap_spproc_type);	//OUTS("SPPROCTYPE",10,30,sap_spproc_type);

		GETCHAR(iView.MAINT_STAT, SAPpstat);
		SAPpstat[14] = '\0';
		//OUTS("SAPpstat",10,30,SAPpstat);

		GETCHAR(iMrpView.MAINT_STAT, MRPpstat);
		MRPpstat[14] = '\0';
		//OUTS("MRPpstat",10,30,MRPpstat);

		//printf("\n...SAPpstat:[%s]\n\n",SAPpstat);
		//printf("\n...Material[%s]; proc_type1:[%s]; spproc_type:[%s]; SAPpstat:[%s]\n\n",cMaterial,proc_type1,spproc_type,SAPpstat);

	break;
	case RFC_EXCEPTION:
		printf("\nRFC Exception : %s",xException);
		exit(0);
	break;
	case RFC_SYS_EXCEPTION:
		printf("\nSystem Exception Raised!!!");
		exit(0);
	break;
	case RFC_FAILURE:
		printf("\nFailure!!!");
		exit(0);
	break;
	default:
		printf("\nOther Failure!");
		exit(0);
}

RfcClose(hRfc);
return 0;
}

int pstat_basicFun(void)
{
	//printf("\n In pstat_basicFun function..[%s]\n",sappstat); fflush(stdout);
	if(tc_strlen(tc_strstr(SAPpstat,"K"))>0)
	{
		pstat = 'K';
	}
	else
	{
		pstat = '0';
	}

	
   if(tc_strlen(tc_strstr(MRPpstat,"D"))>0)
	{
		pstat_mrp = 'D';
	}
	else
	{
		pstat_mrp = '0';
	}

	
  if(tc_strlen(tc_strstr(MRPpstat,"B"))>0)
	{
		pstat_acc = 'B';
	}
	else
	{
		pstat_acc = '0';
	}
	//printf("\n..%c:%c:%c..\n",pstat,pstat_mrp,pstat_acc); fflush(stdout);
	return 0;
}


void mat_create()
{
	printf("\nmmat_create");
	//cll_BAPI_MATERIAL_SAVEREPLICA();
	cll_zbapi_material_savedata_mrp();
	go_for_rfc = 1;
	//fprintf(fsuccess,"\n******************************************************************");
	if (tc_strcmp(mat_type, "FERT") == 0 && apl_dml_flag == 2)
	{
		printf("\nmat_type = FERT and apl_dml_flag = 2");
	}
	else
	{

		go_for_rfc = 1;
	}
}

void cll_zbapi_material_savedata_mrp()
{
	static RFC_RC RfcRc;
	static RFC_HANDLE hRfc;
	char xException[256];

	BAPIMATHEAD eBapiMatHead;
	BAPI_MARA eBasicView;
	BAPI_MARAX eBasicViewx;
	BAPI_MARC eMrpView;
	BAPI_MARCX eMrpViewx;
	BAPI_MPGD eBapi_Mpgd;
	BAPI_MPGDX eBapi_Mpgdx;
	BAPI_MARD eBapi_Mard;
	BAPI_MARDX eBapi_Mardx;
	BAPI_MBEW eAccView;
	BAPI_MBEWX eAccViewx;
	RMMG1_AENNR eRmmg1_Aennr;
	BAPIRET2 eBapiret2;

	ITAB_H thBapi_Makt = ITAB_NULL;
	ITAB_H thBapi_Marm = ITAB_NULL;
	ITAB_H thBapi_Marmx = ITAB_NULL;
	BAPI_MAKT *tBapi_Makt;
	BAPI_MARM *tBapi_Marm;
	BAPI_MARMX *tBapi_Marmx;

	ITAB_H thBAPIE1PAREX3 = ITAB_NULL;
	ITAB_H thBAPIE1PAREXX3 = ITAB_NULL;
	BAPIE1PAREX3 *tBAPIE1PAREX3;
	BAPIE1PAREXX3 *tBAPIE1PAREXX3;

	if (flag == 0)
	{
		printf("\npstat: %c %c %c", pstat, pstat_acc, pstat_mrp); fflush(stdout);
		if (pstat_mrp=='D' || pstat_acc== 'B')
		{
			printf("\nMesage for Material Create: Material %s already exists for plant %s", part_noDup, plantcode); fflush(stdout);
			fprintf(fsuccess, "\nMSG RCVD:Mesage for Material Create: Material %s already exists for plant %s", part_noDup, plantcode); fflush(fsuccess);
			acc_flag = 1;
		}
		else
		{
			acc_flag = 0;
			printf("\nGoing for MATERIAL CREATION..."); fflush(stdout);

			if (apl_dml_flag != 2 || apl_dml_flag == 0) /*only other views to be created as pstat = K*/
			{
				printf("\nOnly other Views will be extended"); fflush(stdout);
				hRfc = BapiLogon();
				thBapi_Makt = ITAB_NULL;
				thBapi_Marm = ITAB_NULL;
				thBapi_Marmx = ITAB_NULL;

				thBAPIE1PAREX3 = ITAB_NULL;
				thBAPIE1PAREXX3 = ITAB_NULL;

				if (thBapi_Makt==ITAB_NULL)
				{
					thBapi_Makt = ItCreate("MATERIALDESCRIPTION",sizeof(BAPI_MAKT),0,0);
					if (thBapi_Makt==ITAB_NULL) rfc_error("ItCreate MATERIALDESCRIPTION");
				}
				else if (ItFree(thBapi_Makt) != 0) rfc_error("ItFree MATERIALDESCRIPTION");

				if (thBapi_Marm==ITAB_NULL)
				{
					 thBapi_Marm = ItCreate("UNITSOFMEASURE",sizeof(BAPI_MARM),0,0);
					 if (thBapi_Marm==ITAB_NULL) rfc_error("ItCreate UNITSOFMEASURE");
				}
				else if (ItFree(thBapi_Marm)!= 0) rfc_error("ItFree UNITSOFMEASURE");

				if (thBapi_Marmx==ITAB_NULL)
				{
					thBapi_Marmx = ItCreate("UNITSOFMEASUREX",sizeof(BAPI_MARMX),0,0);
					if (thBapi_Marmx==ITAB_NULL) rfc_error("ItCreate UNITSOFMEASUREX");
				}
				else if (ItFree(thBapi_Marmx)!= 0) rfc_error("ItFree UNITSOFMEASUREX");

				if (thBAPIE1PAREX3==ITAB_NULL)
				{
					thBAPIE1PAREX3 = ItCreate("EXTENSIONIN",sizeof(BAPIE1PAREX3),0,0);
					if (thBAPIE1PAREX3==ITAB_NULL)
						rfc_error("ItCreate EXTENSIONIN");
				}
				else
				{
					if (ItFree(thBAPIE1PAREX3)!= 0)
						rfc_error("ItFree EXTENSIONIN");
				}

				if (thBAPIE1PAREXX3==ITAB_NULL)
				{
					thBAPIE1PAREXX3 = ItCreate("EXTENSIONINX",sizeof(BAPIE1PAREXX3),0,0);
					if (thBAPIE1PAREXX3==ITAB_NULL)
						rfc_error("ItCreate EXTENSIONINX");
				}
				else
				{
					if (ItFree(thBAPIE1PAREXX3)!= 0)
						rfc_error("ItFree EXTENSIONINX");
				}

				tBapi_Makt = ItAppLine (thBapi_Makt) ;
				tBapi_Marm  = ItAppLine (thBapi_Marm) ;
				tBapi_Marmx = ItAppLine (thBapi_Marmx) ;
				tBAPIE1PAREX3 = ItAppLine(thBAPIE1PAREX3) ;
				tBAPIE1PAREXX3 = ItAppLine(thBAPIE1PAREXX3) ;

				if (tBapi_Makt == NULL)	rfc_error("ItAppLine BAPI_MAKT");
				if (tBapi_Marm == NULL)	rfc_error("ItAppLine BAPI_MARM second table");
				if (tBapi_Marmx == NULL) rfc_error("ItAppLine BAPI_MARMX");
				if (tBAPIE1PAREX3 == NULL) rfc_error("ItAppLine BAPIE1PAREX3");
				if (tBAPIE1PAREXX3 == NULL) rfc_error("ItAppLine BAPIE1PAREXX3");

				printf("\neBapiMatHead.Material:		%s",part_noDup); fflush(stdout);
				printf("\neBapiMatHead.Matl_Type:		%s",mat_type);
				printf("\neBapiMatHead.Ind_Sector:	%s","M");
				printf("\neBapiMatHead.Basic_View:		X");		/*basic view*/
				printf("\neBasicView.Base_Uom:		%s",meas_unit);
				printf("\neBasicView.Base_Uom_Iso:	%s",meas_unit);
				printf("\ntBapi_Marm->Alt_Unit:		%s",meas_unit);
				printf("\ntBapi_Marm->Alt_Unit_Iso:	%s",meas_unit);
				printf("\ntBapi_Marmx->Alt_Unit:	%s",meas_unit);
				printf("\ntBapi_Marmx->Alt_Unit_Iso:	%s",meas_unit);
				printf("\ntBapi_Makt->Matl_Desc:	%s",descDup);
				printf("\neBasicView.Document:		%s",doc_noDup);
				printf("\neBasicView.Doc_Type:		%s",dwg_typeDup);
				printf("\neBasicView.Doc_Vers:		%s",dwg_revDup);
				printf("\neBasicView.No_Sheets:		%s",sheet_noDup);
				printf("\neBasicView.Old_Mat_No:	%s",OldMatNoDup);
				printf("\neBasicView.Net_Weight:	%s",net_wtDup);
				printf("\ntBapi_Marm->Gross_Wt:		%s",gross_wt);
				printf("\neBasicView.Unit_Of_Wt:	%s",unit_wt);
				printf("\neBasicView.Unit_Of_Wt_Iso:	%s",unit_wt);
				printf("\ntBapi_Marm->Unit_Of_Wt:	%s",unit_wt);
				printf("\ntBapi_Marm->Unit_Of_Wt_Iso:	%s",unit_wt);
				printf("\ntBapi_Marm->Volume:		%s",volume);
				printf("\ntBapi_Marm->VolumeUnit:	%s",vol_unit);
				printf("\ntBapi_Marm->VolumeUnit_Iso:	%s",vol_unit);
				printf("\neBasicView.Size_Dim:		%s",sizeDup);
				printf("\neBasicView.Matl_Group:	%s",material_group);
				printf("\neBasicView.Division:		%s",basic_division);
				printf("\neBapiMatHead.Mrp_View:		X");		/*mrp view*/
				printf("\neMrpView.Mrp_Group:		%s",mrp_grp);
				printf("\neMrpView.Abc_Id:		%s",abc_ind);
				printf("\neMrpView.Pur_Status:		%s",mm_pp_status);
				printf("\neMrpView.Mrp_Type:		%s",mrp_type);
				printf("\neMrpView.Reorder_Pt:		%s",reord_pt);
				printf("\neMrpView.Quotausage:		%s",quota_arrangement_usage);
				printf("\neMrpView.Mrp_Ctrler:		%s",mrp_controller);
				printf("\neMrpView.Lotsizekey:		%s",lot_size_key);
				printf("\neMrpView.Fixed_Lot:		%s",fixed_lot_size);
				printf("\neMrpView.Max_Stock:		%s",max_stlvl);
				printf("\neMrpView.Proc_Type:		%s",proc_type);
				printf("\neMrpView.Spproctype:		%s",spl_proc_key);
				printf("\neMrpView.Bulk_Mat:		%s",blk_ind);
				printf("\neMrpView.Sm_Key:		%s",sched_mar_key);
				printf("\neMrpView.Ppc_Pl_Cal:		%s",plan_calendar);
				printf("\neMrpView.Grp_Reqmts:		%s",req_grp);
				printf("\neMrpView.Period_Ind:		%s",period_ind);
				printf("\neMrpView.Mixed_Mrp:		%s",mixedmrp);
				printf("\neMrpView.Availcheck:		%s",avail_chk);
				printf("\neBapi_Mpgd.Plng_Matl:		%s",splan_mat);
				printf("\neBapi_Mpgd.Plng_Plant:	%s",splan_plant);
				printf("\neBapi_Mpgd.Convfactor:	%s",splan_conv_fact);
				printf("\neMrpView.Alternative_Bom:	%s",alternativb);
				printf("\neMrpView.Dep_Req_Id:		%s",ind_collect);
				printf("\neMrpView.Rep_Manuf:		%s",rep_mfg_in);
				printf("\neMrpView.Repmanprof:		%s",rep_mfg);
				printf("\neMrpView.Determ_Grp:		%s",stock_det_group);
				printf("\neBapiMatHead.Storage_View:	X");			/*store view*/
				printf("\neMrpView.Issue_Unit:		%s",uoissue);
				printf("\neMrpView.Issue_Unit_Iso:	%s",uoissue);
				printf("\neMrpView.Profit_Ctr:		%s",profit_centre_sap);		/*profit_centre=0001111010*/
				printf("\neBapiMatHead.Quality_View:		X");			/*quality view*/
				printf("\neMrpView.Ind_Post_To_Insp_Stock:%s",qua_ins_ind);
				printf("\neMrpView.Doc_Reqd:		%s",doc_req);
				printf("\neMrpView.Gr_Pr_Time:		%s",grptime);
				printf("\neBasicView.Catprofile:	%s",catalog_prof);
				printf("\neBasicView.Qm_Procmnt:	%s",qm_proc_ind);
				printf("\neMrpView.Ctrl_Key:		%s",control_key);
				printf("\neMrpView.Cert_Type:		%s",certificate_type);
				printf("\neBapiMatHead.Account_View:		X"); 			/*accounting view */
				printf("\neAccView.Val_Cat:		%s",valuation_cat);
				printf("\neAccView.Val_Class:		%s",val_class);
				printf("\neAccView.Price_Ctrl:		%s",price_con);
				printf("\neAccView.Std_Price:		%s",std_price);
				printf("\neAccView.Moving_Pr:		%s",moving_avg_price);
				printf("\neBapiMatHead.Cost_View:		X");		/*lvl cost view */
				printf("\neAccView.Mat_Origin:		%s",mat_origin);
				printf("\neAccView.Orig_Group:		%s",origin_group);
				printf("\neMrpView.Lot_Size:		%s",cost_lot_size);
				printf("\neAccView.Overhead_Grp:	%s",overhd_grp);
				printf("\neMrpView.Variance_Key:	%s",variance_key);
				printf("\neAccView.Qty_Struct:		%s",with_quantity_structure);
				printf("\neMrpView.Split_Ind:		%s",costing_split_valuation);
				printf("\neMrpViewx.Split_Ind:		X");
				printf("\neRmmg1_Aennr.Aennr:		%s",dml_no_arg);
				printf("\npart_noDupDes:		%s",part_noDupDes);
				printf("\npart_noDupDes:		%s",part_noDupDesx); fflush(stdout);

				fprintf(fsuccess,"\neBapiMatHead.Material:	%s",part_noDup); fflush(fsuccess);
				fprintf(fsuccess,"\neBapiMatHead.Matl_Type:	%s",mat_type);
				fprintf(fsuccess,"\neBapiMatHead.Ind_Sector:		M");
				fprintf(fsuccess,"\neBapiMatHead.Basic_View:		X");		/*basic view*/
				fprintf(fsuccess,"\neBasicView.Base_Uom:	%s",meas_unit);
				fprintf(fsuccess,"\neBasicView.Base_Uom_Iso:	%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Marm->Alt_Unit:	%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Marm->Alt_Unit_Iso:	%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Marmx->Alt_Unit:	%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Marmx->Alt_Unit_Iso:	%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Makt->Matl_Desc:	%s",descDup);
				fprintf(fsuccess,"\neBasicView.Document:	%s",doc_noDup);
				fprintf(fsuccess,"\neBasicView.Doc_Type:	%s",dwg_typeDup);
				fprintf(fsuccess,"\neBasicView.Doc_Vers:	%s",dwg_revDup);
				fprintf(fsuccess,"\neBasicView.No_Sheets:	%s",sheet_noDup);
				fprintf(fsuccess,"\neBasicView.Old_Mat_No:	%s",OldMatNoDup);
				fprintf(fsuccess,"\neBasicView.Net_Weight:	%s",net_wtDup);
				fprintf(fsuccess,"\ntBapi_Marm->Gross_Wt:	%s",gross_wt);
				fprintf(fsuccess,"\neBasicView.Unit_Of_Wt:	%s",unit_wt);
				fprintf(fsuccess,"\neBasicView.Unit_Of_Wt_Iso:	%s",unit_wt);
				fprintf(fsuccess,"\ntBapi_Marm->Unit_Of_Wt:	%s",unit_wt);
				fprintf(fsuccess,"\ntBapi_Marm->Unit_Of_Wt_Iso:	%s",unit_wt);
				fprintf(fsuccess,"\ntBapi_Marm->Volume:		%s",volume);
				fprintf(fsuccess,"\ntBapi_Marm->VolumeUnit:	%s",vol_unit);
				fprintf(fsuccess,"\ntBapi_Marm->VolumeUnit_Iso:	%s",vol_unit);
				fprintf(fsuccess,"\neBasicView.Size_Dim:	%s",sizeDup);
				fprintf(fsuccess,"\neBasicView.Matl_Group:	%s",material_group);
				fprintf(fsuccess,"\neBasicView.Division:	%s",basic_division);
				fprintf(fsuccess,"\neBapiMatHead.Mrp_View:		X");		/*mrp view*/
				fprintf(fsuccess,"\neMrpView.Mrp_Group:		%s",mrp_grp);
				fprintf(fsuccess,"\neMrpView.Abc_Id:		%s",abc_ind);
				fprintf(fsuccess,"\neMrpView.Pur_Status:	%s",mm_pp_status);
				fprintf(fsuccess,"\neMrpView.Mrp_Type:		%s",mrp_type);
				fprintf(fsuccess,"\neMrpView.Reorder_Pt:	%s",reord_pt);
				fprintf(fsuccess,"\neMrpView.Quotausage:	%s",quota_arrangement_usage);
				fprintf(fsuccess,"\neMrpView.Mrp_Ctrler:	%s",mrp_controller);
				fprintf(fsuccess,"\neMrpView.Lotsizekey:	%s",lot_size_key);
				fprintf(fsuccess,"\neMrpView.Fixed_Lot:		%s",fixed_lot_size);
				fprintf(fsuccess,"\neMrpView.Max_Stock:		%s",max_stlvl);
				fprintf(fsuccess,"\neMrpView.Proc_Type:		%s",proc_type);
				fprintf(fsuccess,"\neMrpView.Spproctype:	%s",spl_proc_key);
				fprintf(fsuccess,"\neMrpView.Bulk_Mat:		%s",blk_ind);
				fprintf(fsuccess,"\neMrpView.Sm_Key:		%s",sched_mar_key);
				fprintf(fsuccess,"\neMrpView.Ppc_Pl_Cal:	%s",plan_calendar);
				fprintf(fsuccess,"\neMrpView.Grp_Reqmts:	%s",req_grp);
				fprintf(fsuccess,"\neMrpView.Period_Ind:	%s",period_ind);
				fprintf(fsuccess,"\neMrpView.Mixed_Mrp:		%s",mixedmrp);
				fprintf(fsuccess,"\neMrpView.Availcheck:	%s",avail_chk);
				fprintf(fsuccess,"\neBapi_Mpgd.Plng_Matl:	%s",splan_mat);
				fprintf(fsuccess,"\neBapi_Mpgd.Plng_Plant:	%s",splan_plant);
				fprintf(fsuccess,"\neBapi_Mpgd.Convfactor:	%s",splan_conv_fact);
				fprintf(fsuccess,"\neMrpView.Alternative_Bom:	%s",alternativb);
				fprintf(fsuccess,"\neMrpView.Dep_Req_Id:	%s",ind_collect);
				fprintf(fsuccess,"\neMrpView.Rep_Manuf:		%s",rep_mfg_in);
				fprintf(fsuccess,"\neMrpView.Repmanprof:	%s",rep_mfg);
				fprintf(fsuccess,"\neMrpView.Determ_Grp:	%s",stock_det_group);
				fprintf(fsuccess,"\neBapiMatHead.Storage_View:		X");		/*store view*/
				fprintf(fsuccess,"\neMrpView.Issue_Unit:	%s",uoissue);
				fprintf(fsuccess,"\neMrpView.Issue_Unit_Iso:	%s",uoissue);
				fprintf(fsuccess,"\neMrpView.Profit_Ctr:	%s",profit_centre_sap);		/*profit_centre=0001111010*/
				fprintf(fsuccess,"\neBapiMatHead.Quality_View:		X");		/*quality view*/
				fprintf(fsuccess,"\neMrpView.Ind_Post_To_Insp_Stock:%s",qua_ins_ind);
				fprintf(fsuccess,"\neMrpView.Doc_Reqd:		%s",doc_req);
				fprintf(fsuccess,"\neMrpView.Gr_Pr_Time:	%s",grptime);
				fprintf(fsuccess,"\neBasicView.Catprofile:	%s",catalog_prof);
				fprintf(fsuccess,"\neBasicView.Qm_Procmnt:	%s",qm_proc_ind);
				fprintf(fsuccess,"\neMrpView.Ctrl_Key:		%s",control_key);
				fprintf(fsuccess,"\neMrpView.Cert_Type:		%s",certificate_type);
				fprintf(fsuccess,"\neBapiMatHead.Account_View:		X"); 		/*accounting view */
				fprintf(fsuccess,"\neAccView.Val_Cat:		%s",valuation_cat);
				fprintf(fsuccess,"\neAccView.Val_Class:		%s",val_class);
				fprintf(fsuccess,"\neAccView.Price_Ctrl:	%s",price_con);
				fprintf(fsuccess,"\neAccView.Std_Price:		%s",std_price);
				fprintf(fsuccess,"\neAccView.Moving_Pr:		%s",moving_avg_price);
				fprintf(fsuccess,"\neBapiMatHead.Cost_View:		X");		/*lvl cost view */
				fprintf(fsuccess,"\neAccView.Mat_Origin:	%s",mat_origin);
				fprintf(fsuccess,"\neAccView.Orig_Group:	%s",origin_group);
				fprintf(fsuccess,"\neMrpView.Lot_Size:		%s",cost_lot_size);
				fprintf(fsuccess,"\neAccView.Overhead_Grp:	%s",overhd_grp);
				fprintf(fsuccess,"\neMrpView.Variance_Key:	%s",variance_key);
				fprintf(fsuccess,"\neAccView.Qty_Struct:	%s",with_quantity_structure);
				fprintf(fsuccess,"\neMrpView.Split_Ind:		%s",costing_split_valuation);
				fprintf(fsuccess,"\neMrpViewx.Split_Ind:		X");	/*Please make it as X*/
				fprintf(fsuccess,"\neRmmg1_Aennr.Aennr:		%s",dml_no_arg);
				fprintf(fsuccess,"\npart_noDupDes:		%s",part_noDupDes);
				fprintf(fsuccess,"\npart_noDupDes:		%s",part_noDupDesx); fflush(fsuccess);

				SETCHAR(eBapiMatHead.Material,part_noDup);
				SETCHAR(eBapiMatHead.Matl_Type,mat_type);
				SETCHAR(eBapiMatHead.Ind_Sector,"M");
				SETCHAR(eBapiMatHead.Basic_View,"");
				SETCHAR(eBasicView.Base_Uom,meas_unit);
				SETCHAR(eBasicView.Base_Uom_Iso,meas_unit);
				SETCHAR(eBasicViewx.Base_Uom,"X");
				SETCHAR(eBasicViewx.Base_Uom_Iso,"X");
				SETCHAR(tBapi_Marm->Alt_Unit,meas_unit);
				SETCHAR(tBapi_Marm->Alt_Unit_Iso,meas_unit);
				SETCHAR(tBapi_Marmx->Alt_Unit,meas_unit);
				SETCHAR(tBapi_Marmx->Alt_Unit_Iso,meas_unit);
				SETCHAR(tBapi_Makt->Matl_Desc,descDup);
				
				if(tc_strlen(doc_noDup)==0)
				{
					SETCHAR(eBasicView.Document,"");
					SETCHAR(eBasicViewx.Document,"");
				}
				else
				{
					SETCHAR(eBasicView.Document,doc_noDup);
					SETCHAR(eBasicViewx.Document,"X");
				}
				if(tc_strlen(dwg_typeDup)==0)
				{
						SETCHAR(eBasicView.Doc_Type,"");
						SETCHAR(eBasicViewx.Doc_Type,"");
				}
				else
				{
					SETCHAR(eBasicView.Doc_Type,dwg_typeDup);
					SETCHAR(eBasicViewx.Doc_Type,"X");
				}
				if(tc_strlen(dwg_revDup)==0)
				{
						SETCHAR(eBasicView.Doc_Vers,"");
						SETCHAR(eBasicViewx.Doc_Vers,"");
				}
				else
				{
					SETCHAR(eBasicView.Doc_Vers,dwg_revDup);
					SETCHAR(eBasicViewx.Doc_Vers,"X");
				}
				if(tc_strlen(sheet_noDup)==0)
				{
						SETCHAR(eBasicView.No_Sheets,"");
						SETCHAR(eBasicViewx.No_Sheets,"");
				}
				else
				{
					SETNUM(eBasicView.No_Sheets,sheet_noDup);
					SETCHAR(eBasicViewx.No_Sheets,"X");
				}
				printf("sheet_noDup:%s",sheet_noDup); fflush(stdout);
				fprintf(fsuccess,"sheet_noDup:%s",sheet_noDup); fflush(fsuccess);

				SETCHAR(eBasicView.Old_Mat_No,OldMatNoDup);
				SETCHAR(eBasicViewx.Old_Mat_No,"");
				SETBCD(eBasicView.Net_Weight,myzeroDup3,3);
				SETCHAR(eBasicViewx.Net_Weight,"");
				SETBCD(tBapi_Marm->Gross_Wt,gross_wt,3);
				SETCHAR(tBapi_Marmx->Gross_Wt,"");
				SETCHAR(eBasicView.Unit_Of_Wt,unit_wt);
				SETCHAR(eBasicView.Unit_Of_Wt_Iso,unit_wt);
				SETCHAR(eBasicViewx.Unit_Of_Wt,"");
				SETCHAR(eBasicViewx.Unit_Of_Wt_Iso,"");
				SETCHAR(tBapi_Marm->Unit_Of_Wt,unit_wt);
				SETCHAR(tBapi_Marm->Unit_Of_Wt_Iso,unit_wt);
				SETCHAR(tBapi_Marmx->Unit_Of_Wt,"");
				SETCHAR(tBapi_Marmx->Unit_Of_Wt_Iso,"");
				SETBCD(tBapi_Marm->Volume,myzeroDup3,3);
				SETCHAR(tBapi_Marm->VolumeUnit,vol_unit);
				SETCHAR(tBapi_Marm->VolumeUnit_Iso,vol_unit);
				SETCHAR(tBapi_Marmx->Volume,"");
				SETCHAR(tBapi_Marmx->VolumeUnit,"");
				SETCHAR(tBapi_Marmx->VolumeUnit_Iso,"");
				if(tc_strlen(sizeDup)==0)
				{
					SETCHAR(eBasicView.Size_Dim,"");
					SETCHAR(eBasicViewx.Size_Dim,"");
				}
				else
				{
					SETCHAR(eBasicView.Size_Dim,sizeDup);/*sizeDup*/
					SETCHAR(eBasicViewx.Size_Dim,"X");
				}

				if(tc_strlen(store_locDup)>0)
				{
					//string strtoupper ( string store_locDup );
					//store_locDup1 = strtoupper(store_locDup);

					printf ("Before conversion: %s\n", store_locDup);
					for (p = store_locDup; *p != '\0'; ++p)
					{
						*p = toupper(*p);
					}
					printf ("After conversion: %s\n", store_locDup);

					//printf("\nstore location converted to capital letters, if in small letters  : %s",store_locDup1);
					SETCHAR(eMrpView.Iss_St_Loc,store_locDup);
					SETCHAR(eMrpView.Sloc_Exprc,store_locDup);
					SETCHAR(eMrpViewx.Iss_St_Loc,"X");
					SETCHAR(eMrpViewx.Sloc_Exprc,"X");
					printf("\nstore location is : %s",store_locDup);
					fprintf(fsuccess,"\nstore location is : %s",store_locDup);
				}
				else
				{
					SETCHAR(eMrpView.Iss_St_Loc,"");
					SETCHAR(eMrpView.Sloc_Exprc,"");
					SETCHAR(eMrpViewx.Iss_St_Loc,"");
					SETCHAR(eMrpViewx.Sloc_Exprc,"");

					//printf("\nstore location is : %s",store_locDup); fflush(stdout);
					//fprintf(fsuccess,"\nstore location is : %s",store_locDup); fflush(fsuccess);
					printf("\nstore location is null"); 
				}
				
				SETCHAR(eBasicView.Matl_Group,material_group);
				SETCHAR(eBasicViewx.Matl_Group,"");
				SETCHAR(eBasicView.Division,basic_division);
				SETCHAR(eBasicViewx.Division,"");
				SETCHAR(eBapiMatHead.Mrp_View,"X");	/*mrp view*/
				
				SETCHAR(eMrpView.Mrp_Group,mrp_grp);
				SETCHAR(eMrpViewx.Mrp_Group,"X");
				SETCHAR(eMrpView.Abc_Id,abc_ind);
				SETCHAR(eMrpViewx.Abc_Id,"X");

				if(tc_strlen(mm_pp_status)==0)
				{
					SETCHAR(eMrpView.Pur_Status," ");
					SETCHAR(eMrpViewx.Pur_Status,"X");
					printf("\nMaterial  status is........ABC : %s",mm_pp_status);
					fprintf(fsuccess,"\nMaterial  status is........ABC : %s",mm_pp_status);
				}
				else
				{
					SETCHAR(eMrpView.Pur_Status,mm_pp_status);
					SETCHAR(eMrpViewx.Pur_Status,"X");
					printf("\nMaterial  status is........ABC1 : %s",mm_pp_status);
					fprintf(fsuccess,"\nMaterial  status is........ABC1 : %s",mm_pp_status);
				}
				
				//SETCHAR(eMrpView.Pur_Status,mm_pp_status);
				//SETCHAR(eMrpViewx.Pur_Status,"X");
				SETCHAR(eMrpView.Mrp_Type,mrp_type);
				SETCHAR(eMrpViewx.Mrp_Type,"X");
				SETBCD(eMrpView.Reorder_Pt,reord_pt,3);
				SETCHAR(eMrpViewx.Reorder_Pt,"X");

				SETCHAR(eMrpView.Quotausage,quota_arrangement_usage);
				SETCHAR(eMrpViewx.Quotausage,"X");
				SETCHAR(eMrpView.Mrp_Ctrler,mrp_controller);
				SETCHAR(eMrpViewx.Mrp_Ctrler,"X");
				SETCHAR(eMrpView.Lotsizekey,lot_size_key);
				SETCHAR(eMrpViewx.Lotsizekey,"X");
				SETBCD(eMrpView.Fixed_Lot,fixed_lot_size,3);
				SETCHAR(eMrpViewx.Fixed_Lot,"X");
				SETBCD(eMrpView.Max_Stock,max_stlvl,3);
				SETCHAR(eMrpViewx.Max_Stock,"X");
				SETCHAR(eMrpView.Proc_Type,proc_type);
				SETCHAR(eMrpViewx.Proc_Type,"X");
				SETCHAR(eMrpView.Spproctype,spl_proc_key);
				SETCHAR(eMrpViewx.Spproctype,"X");
				SETCHAR(eMrpView.Bulk_Mat,blk_ind);
				SETCHAR(eMrpViewx.Bulk_Mat,"X");
				SETCHAR(eMrpView.Sm_Key,sched_mar_key);
				SETCHAR(eMrpViewx.Sm_Key,"X");
				SETCHAR(eMrpView.Ppc_Pl_Cal,plan_calendar);
				SETCHAR(eMrpViewx.Ppc_Pl_Cal,"X");
				SETCHAR(eMrpView.Grp_Reqmts,req_grp);
				SETCHAR(eMrpViewx.Grp_Reqmts,"X");
				SETCHAR(eMrpView.Period_Ind,period_ind);
				SETCHAR(eMrpViewx.Period_Ind,"X");
				SETCHAR(eMrpView.Mixed_Mrp,mixedmrp);
				SETCHAR(eMrpViewx.Mixed_Mrp,"X");
				SETCHAR(eMrpView.Availcheck,avail_chk);
				SETCHAR(eMrpViewx.Availcheck,"X");
				SETCHAR(eBapi_Mpgd.Plng_Matl,splan_mat);
				SETCHAR(eBapi_Mpgdx.Plng_Matl,"X");
				SETCHAR(eBapi_Mpgd.Plng_Plant,splan_plant);
				SETCHAR(eBapi_Mpgdx.Plng_Plant,"X");
				SETCHAR(eBapi_Mpgd.Convfactor,splan_conv_fact);
				SETCHAR(eBapi_Mpgdx.Convfactor,"X");
				SETCHAR(eMrpView.Alternative_Bom,"");
				SETCHAR(eMrpViewx.Alternative_Bom,"");
				SETCHAR(eMrpView.Alt_Bom_Id,alternativb);
				SETCHAR(eMrpViewx.Alt_Bom_Id,"X");
				SETCHAR(eMrpView.Dep_Req_Id,ind_collect);
				SETCHAR(eMrpViewx.Dep_Req_Id,"X");
				
				

				//SETCHAR(eMrpView.Rep_Manuf,"");/*rep_mfg_in*/
				//SETCHAR(eMrpViewx.Rep_Manuf,"");/*X*/
				//SETCHAR(eMrpView.Repmanprof,"");/*rep_mfg*/
				//SETCHAR(eMrpViewx.Repmanprof,"");/*X*/

				if(tc_strlen(rep_mfg)>0)		//rep_mfg modified on 14.02.2017 for TE02
				{
					SETCHAR(eMrpView.Rep_Manuf,"X");/*rep_mfg_in*/
					SETCHAR(eMrpViewx.Rep_Manuf,"X");/*X*/
					SETCHAR(eMrpView.Repmanprof,rep_mfg);/*rep_mfg*/
					SETCHAR(eMrpViewx.Repmanprof,"X");/*X*/
				}
				else
				{
					SETCHAR(eMrpView.Rep_Manuf,"");/*rep_mfg_in*/
					SETCHAR(eMrpViewx.Rep_Manuf,"X");/*X*/
					SETCHAR(eMrpView.Repmanprof,"");/*rep_mfg*/
					SETCHAR(eMrpViewx.Repmanprof,"X");/*X*/
				}
				
				SETCHAR(eMrpView.Determ_Grp,stock_det_group);
				SETCHAR(eMrpViewx.Determ_Grp,"X");
				SETCHAR(eBapiMatHead.Storage_View,"X");	/*store view*/
				SETCHAR(eMrpView.Issue_Unit,uoissue);
				SETCHAR(eMrpView.Issue_Unit_Iso,uoissue);
				SETCHAR(eMrpViewx.Issue_Unit,"");	/*these values they are sending as blank hence removed*/
				SETCHAR(eMrpViewx.Issue_Unit_Iso,"");
				SETCHAR(eMrpView.Profit_Ctr,profit_centre_sap);		/*profit_centre=0001111010*/
				SETCHAR(eMrpViewx.Profit_Ctr,"X");
				printf("\n3"); fflush(stdout);
				if(tc_strcmp(mat_type,"FERT") != 0)
				{
					SETCHAR(eBapiMatHead.Quality_View,"X");	/*quality view*/
					SETCHAR(eMrpView.Ind_Post_To_Insp_Stock,qua_ins_ind);
					SETCHAR(eMrpViewx.Ind_Post_To_Insp_Stock,"X");
					SETCHAR(eMrpView.Doc_Reqd,doc_req);
					SETCHAR(eMrpViewx.Doc_Reqd,"X");
					SETBCD(eMrpView.Gr_Pr_Time,grptime,0);
					SETCHAR(eMrpViewx.Gr_Pr_Time,"X");
					SETCHAR(eBasicView.Catprofile,catalog_prof);
					SETCHAR(eBasicViewx.Catprofile,"X");
					SETCHAR(eBasicView.Qm_Procmnt,qm_proc_ind);
					SETCHAR(eBasicViewx.Qm_Procmnt,"X");
					SETCHAR(eMrpView.Ctrl_Key,control_key);
					SETCHAR(eMrpViewx.Ctrl_Key,"X");
					SETCHAR(eMrpView.Cert_Type,certificate_type);
					SETCHAR(eMrpViewx.Cert_Type,"X");

					printf("\ncertificate_type:%s",certificate_type); fflush(stdout);
				}
				else if(tc_strcmp(mat_type,"FERT") == 0)	//Anuja,Swati Tendulkar - QM View for VC
				{
					printf("\nQM View For VC");fflush(stdout);
					printf("\n[%s,%s,%s,%s,%s]",qua_ins_ind,doc_req,grptime,catalog_prof,qm_proc_ind);fflush(stdout);

					SETCHAR(eBapiMatHead.Quality_View,"X");	/*quality view*/
					SETCHAR(eMrpView.Ind_Post_To_Insp_Stock,"");
					SETCHAR(eMrpViewx.Ind_Post_To_Insp_Stock,"");
					SETCHAR(eMrpView.Doc_Reqd,"");
					SETCHAR(eMrpViewx.Doc_Reqd,"");
					SETBCD(eMrpView.Gr_Pr_Time,grptime,0);
					SETCHAR(eMrpViewx.Gr_Pr_Time,"X");
					SETCHAR(eBasicView.Catprofile,catalog_prof);
					SETCHAR(eBasicViewx.Catprofile,"X");
					SETCHAR(eBasicView.Qm_Procmnt,"");
					SETCHAR(eBasicViewx.Qm_Procmnt,"");
					SETCHAR(eMrpView.Ctrl_Key,control_key);
					SETCHAR(eMrpViewx.Ctrl_Key,"X");
					SETCHAR(eMrpView.Cert_Type,certificate_type);
					SETCHAR(eMrpViewx.Cert_Type,"X");
				}
				else    /* as per req. on 1/8 */
				{
					SETCHAR(eBapiMatHead.Quality_View,"");	/*quality view*/
					SETCHAR(eMrpView.Ind_Post_To_Insp_Stock,"");
					SETCHAR(eMrpViewx.Ind_Post_To_Insp_Stock,"");
					SETCHAR(eMrpView.Doc_Reqd,"");
					SETCHAR(eMrpViewx.Doc_Reqd,"");
					SETBCD(eMrpView.Gr_Pr_Time,"",0);
					SETCHAR(eMrpViewx.Gr_Pr_Time,"");
					SETCHAR(eBasicView.Catprofile,"");
					SETCHAR(eBasicViewx.Catprofile,"");
					SETCHAR(eBasicView.Qm_Procmnt,"");
					SETCHAR(eBasicViewx.Qm_Procmnt,"");
					SETCHAR(eMrpView.Ctrl_Key,"");
					SETCHAR(eMrpViewx.Ctrl_Key,"");
					SETCHAR(eMrpView.Cert_Type,"");
					SETCHAR(eMrpViewx.Cert_Type,"");
				}
				SETCHAR(eBapiMatHead.Account_View,"X"); /*accounting view */
				SETCHAR(eAccView.Val_Cat,valuation_cat);
				SETCHAR(eAccViewx.Val_Cat,"X");
				SETCHAR(eAccView.Val_Class,val_class);
				SETCHAR(eAccViewx.Val_Class,"X");
				SETCHAR(eAccView.Price_Ctrl,price_con);
				SETCHAR(eAccViewx.Price_Ctrl,"X");
				SETBCD(eAccView.Std_Price,std_price,4);
				SETCHAR(eAccViewx.Std_Price,"X");
				SETBCD(eAccView.Moving_Pr,moving_avg_price,4);
				SETCHAR(eAccViewx.Moving_Pr,"X");

				SETCHAR(eBapiMatHead.Cost_View,"X");/*lvl cost view */
				SETCHAR(eAccView.Mat_Origin,mat_origin);
				SETCHAR(eAccViewx.Mat_Origin,"X");
				SETCHAR(eAccView.Orig_Group,origin_group);
				SETCHAR(eAccViewx.Orig_Group,"X");
				SETBCD(eMrpView.Lot_Size,cost_lot_size,3);
				SETCHAR(eMrpViewx.Lot_Size,"X");
				SETCHAR(eAccView.Overhead_Grp,overhd_grp);
				SETCHAR(eAccViewx.Overhead_Grp,"X");
				SETCHAR(eMrpView.Variance_Key,variance_key);
				SETCHAR(eMrpViewx.Variance_Key,"X");
				SETCHAR(eAccView.Qty_Struct,with_quantity_structure);
				SETCHAR(eAccViewx.Qty_Struct,"X");
				SETCHAR(eMrpView.Split_Ind,costing_split_valuation);
				SETCHAR(eMrpViewx.Split_Ind,"");/*Please make it as X*/
				SETCHAR(eRmmg1_Aennr.Aennr,dml_no_arg);

				SETCHAR(eBapiMatHead.Sales_View,"");
				SETCHAR(eBapiMatHead.Purchase_View,"");
				SETCHAR(eBapiMatHead.Forecast_View,"");
				SETCHAR(eBapiMatHead.Work_Sched_View,"");
				SETCHAR(eBapiMatHead.Prt_View,"");
				SETCHAR(eBapiMatHead.Warehouse_View,"");
				SETCHAR(eBapiMatHead.Inp_Fld_Check,"");
				SETCHAR(eBapiMatHead.Material_External,"");
				SETCHAR(eBapiMatHead.MateriaL_Guid,"");
				SETCHAR(eBapiMatHead.Material_Version,"");

				SETCHAR(eBasicView.Del_Flag,"");
				SETCHAR(eBasicView.Po_Unit,"");
				SETCHAR(eBasicView.Po_Unit_Iso,"");
				SETCHAR(eBasicView.Doc_Format,"");
				SETCHAR(eBasicView.Doc_Chg_No,"");
				SETCHAR(eBasicView.Page_No,"");
				SETCHAR(eBasicView.Prod_Memo,"");
				SETCHAR(eBasicView.Pageformat,"");
				SETCHAR(eBasicView.Basic_Matl,"");
				SETCHAR(eBasicView.Std_Descr,"");
				SETCHAR(eBasicView.Dsn_Office,"");
				SETCHAR(eBasicView.Pur_Valkey,"");
				SETCHAR(eBasicView.Container,"");
				SETCHAR(eBasicView.Stor_Conds,"");
				SETCHAR(eBasicView.Temp_Conds,"");
				SETCHAR(eBasicView.Trans_Grp,"");
				SETCHAR(eBasicView.Haz_Mat_No,"");
				SETCHAR(eBasicView.Competitor,"");
				SETBCD(eBasicView.Qty_Gr_Gi,myzeroDup3,3);
				SETCHAR(eBasicView.Proc_Rule,"");
				SETCHAR(eBasicView.Sup_Source,"");
				SETCHAR(eBasicView.Season,"");
				SETCHAR(eBasicView.Label_Type,"");
				SETCHAR(eBasicView.Label_Form,"");
				SETCHAR(eBasicView.Prod_Hier,"");
				SETCHAR(eBasicView.Cad_Id,"X");
				SETBCD(eBasicView.Allowed_Wt,myzeroDup3,3);
				SETCHAR(eBasicView.Pack_Wt_Un,"");
				SETCHAR(eBasicView.Pack_Wt_Un_Iso,"");
				SETBCD(eBasicView.Allwd_Vol,myzeroDup3,3);
				SETCHAR(eBasicView.Pack_Vo_Un,"");
				SETCHAR(eBasicView.Pack_Vo_Un_Iso,"");
				SETBCD(eBasicView.Wt_Tol_Lt,myzeroDup1,1);
				SETBCD(eBasicView.Vol_Tol_Lt,myzeroDup1,1);
				SETCHAR(eBasicView.Var_Ord_Un,"");
				SETCHAR(eBasicView.Batch_Mgmt,"");
				SETCHAR(eBasicView.Sh_Mat_Typ,"");
				SETBCD(eBasicView.Fill_Level,"0",0);
				SETINT2(&eBasicView.Stack_Fact,"0");
				SETCHAR(eBasicView.Mat_Grp_Sm,"");
				SETCHAR(eBasicView.Authoritygroup,"");
				SETBCD(eBasicView.Minremlife,"0",0);
				SETBCD(eBasicView.Shelf_Life,"0",0);
				SETBCD(eBasicView.Stor_Pct,"0",0);
				SETCHAR(eBasicView.Pur_Status,"");
				SETCHAR(eBasicView.Sal_Status,"");
				SETDATE(eBasicView.Pvalidfrom,"");
				SETDATE(eBasicView.Svalidfrom,"");
				SETCHAR(eBasicView.Envt_Rlvt,"");
				SETCHAR(eBasicView.Prod_Alloc,"");
				SETCHAR(eBasicView.Qual_Dik,"");
				SETCHAR(eBasicView.Manu_Mat,"");
				SETCHAR(eBasicView.Mfr_No,"");
				SETCHAR(eBasicView.Inv_Mat_No,"");
				SETCHAR(eBasicView.Manuf_Prof,"");
				SETCHAR(eBasicView.Hazmatprof,"");
				SETCHAR(eBasicView.High_Visc,"");
				SETCHAR(eBasicView.Looseorliq,"");
				SETCHAR(eBasicView.Closed_Box,"");
				SETCHAR(eBasicView.Appd_B_Rec,"");
				SETNUM(eBasicView.Matcmpllvl,"00");
				SETCHAR(eBasicView.Par_Eff,"");
				SETCHAR(eBasicView.Round_Up_Rule_Expiration_Date,"");
				SETCHAR(eBasicView.Period_Ind_Expiration_Date,"D");
				SETCHAR(eBasicView.Prod_Composition_On_Packaging,"");
				SETCHAR(eBasicView.Item_Cat,"");
				SETCHAR(eBasicView.Haz_Mat_No_External,"");
				SETCHAR(eBasicView.Haz_Mat_No_Guid,"");
				SETCHAR(eBasicView.Haz_Mat_No_Version,"");
				SETCHAR(eBasicView.Inv_Mat_No_External,"");
				SETCHAR(eBasicView.Inv_Mat_No_Guid,"");
				SETCHAR(eBasicView.Inv_Mat_No_Version,"");
				SETCHAR(eBasicView.Material_Fixed,"");
				SETCHAR(eBasicView.Cm_Relevance_Flag,"");
				SETCHAR(eBasicView.Sled_Bbd,"");
				SETCHAR(eBasicView.Gtin_Variant,"");
				SETCHAR(eBasicView.Serialization_Level,"");
				SETCHAR(eBasicView.Pl_Ref_Mat,"");
				SETCHAR(eBasicView.Extmatlgrp,"");
				SETCHAR(eBasicView.Uomusage,"");
				SETCHAR(eBasicView.Pl_Ref_Mat_External,"");
				SETCHAR(eBasicView.Pl_Ref_Mat_Guid,"");
				SETCHAR(eBasicView.Pl_Ref_Mat_Version,"");

				SETCHAR(eBasicViewx.Del_Flag,"");
				SETCHAR(eBasicViewx.Po_Unit,"");
				SETCHAR(eBasicViewx.Po_Unit_Iso,"");
				SETCHAR(eBasicViewx.Doc_Format,"");
				SETCHAR(eBasicViewx.Doc_Chg_No,"");
				SETCHAR(eBasicViewx.Page_No,"");
				SETCHAR(eBasicViewx.Prod_Memo,"");
				SETCHAR(eBasicViewx.Pageformat,"");
				SETCHAR(eBasicViewx.Basic_Matl,"");
				SETCHAR(eBasicViewx.Std_Descr,"");
				SETCHAR(eBasicViewx.Dsn_Office,"");
				SETCHAR(eBasicViewx.Pur_Valkey,"");
				SETCHAR(eBasicViewx.Container,"");
				SETCHAR(eBasicViewx.Stor_Conds,"");
				SETCHAR(eBasicViewx.Temp_Conds,"");
				SETCHAR(eBasicViewx.Trans_Grp,"");
				SETCHAR(eBasicViewx.Haz_Mat_No,"");
				SETCHAR(eBasicViewx.Competitor,"");
				SETCHAR(eBasicViewx.Qty_Gr_Gi,"");
				SETCHAR(eBasicViewx.Proc_Rule,"");
				SETCHAR(eBasicViewx.Sup_Source,"");
				SETCHAR(eBasicViewx.Season,"");
				SETCHAR(eBasicViewx.Label_Type,"");
				SETCHAR(eBasicViewx.Label_Form,"");
				SETCHAR(eBasicViewx.Prod_Hier,"");
				SETCHAR(eBasicViewx.Cad_Id,"X");
				SETCHAR(eBasicViewx.Allowed_Wt,"");
				SETCHAR(eBasicViewx.Pack_Wt_Un,"");
				SETCHAR(eBasicViewx.Pack_Wt_Un_Iso,"");
				SETCHAR(eBasicViewx.Allwd_Vol,"");
				SETCHAR(eBasicViewx.Pack_Vo_Un,"");
				SETCHAR(eBasicViewx.Pack_Vo_Un_Iso,"");
				SETCHAR(eBasicViewx.Wt_Tol_Lt,"");
				SETCHAR(eBasicViewx.Vol_Tol_Lt,"");
				SETCHAR(eBasicViewx.Var_Ord_Un,"");
				SETCHAR(eBasicViewx.Batch_Mgmt,"");
				SETCHAR(eBasicViewx.Sh_Mat_Typ,"");
				SETCHAR(eBasicViewx.Fill_Level,"");
				SETCHAR(eBasicViewx.Stack_Fact,"");
				SETCHAR(eBasicViewx.Mat_Grp_Sm,"");
				SETCHAR(eBasicViewx.Authoritygroup,"");
				SETCHAR(eBasicViewx.Minremlife,"");
				SETCHAR(eBasicViewx.Shelf_Life,"");
				SETCHAR(eBasicViewx.Stor_Pct,"");
				SETCHAR(eBasicViewx.Pur_Status,"");
				SETCHAR(eBasicViewx.Sal_Status,"");
				SETCHAR(eBasicViewx.Pvalidfrom,"");
				SETCHAR(eBasicViewx.Svalidfrom,"");
				SETCHAR(eBasicViewx.Envt_Rlvt,"");
				SETCHAR(eBasicViewx.Prod_Alloc,"");
				SETCHAR(eBasicViewx.Qual_Dik,"");
				SETCHAR(eBasicViewx.Manu_Mat,"");
				SETCHAR(eBasicViewx.Mfr_No,"");
				SETCHAR(eBasicViewx.Inv_Mat_No,"");
				SETCHAR(eBasicViewx.Manuf_Prof,"");
				SETCHAR(eBasicViewx.Hazmatprof,"");
				SETCHAR(eBasicViewx.High_Visc,"");
				SETCHAR(eBasicViewx.Looseorliq,"");
				SETCHAR(eBasicViewx.Closed_Box,"");
				SETCHAR(eBasicViewx.Appd_B_Rec,"");
				SETCHAR(eBasicViewx.Matcmpllvl,"");
				SETCHAR(eBasicViewx.Par_Eff,"");
				SETCHAR(eBasicViewx.Round_Up_Rule_Expiration_Date,"");
				SETCHAR(eBasicViewx.Period_Ind_Expiration_Date,"");
				SETCHAR(eBasicViewx.Prod_Composition_On_Packaging,"");
				SETCHAR(eBasicViewx.Item_Cat,"");
				SETCHAR(eBasicViewx.Haz_Mat_No_External,"");
				SETCHAR(eBasicViewx.Haz_Mat_No_Guid,"");
				SETCHAR(eBasicViewx.Haz_Mat_No_Version,"");
				SETCHAR(eBasicViewx.Inv_Mat_No_External,"");
				SETCHAR(eBasicViewx.Inv_Mat_No_Guid,"");
				SETCHAR(eBasicViewx.Inv_Mat_No_Version,"");
				SETCHAR(eBasicViewx.Material_Fixed,"");
				SETCHAR(eBasicViewx.Cm_Relevance_Flag,"");
				SETCHAR(eBasicViewx.Sled_Bbd,"");
				SETCHAR(eBasicViewx.Gtin_Variant,"");
				SETCHAR(eBasicViewx.Serialization_Level,"");
				SETCHAR(eBasicViewx.Pl_Ref_Mat,"");
				SETCHAR(eBasicViewx.Extmatlgrp,"");
				SETCHAR(eBasicViewx.Uomusage,"");
				SETCHAR(eBasicViewx.Pl_Ref_Mat_External,"");
				SETCHAR(eBasicViewx.Pl_Ref_Mat_Guid,"");
				SETCHAR(eBasicViewx.Pl_Ref_Mat_Version,"");
				SETCHAR(eMrpView.Plant,plantcode);
				SETCHAR(eMrpView.Del_Flag,"");
				SETCHAR(eMrpView.Crit_Part,"");
				SETCHAR(eMrpView.Pur_Group,"");
				SETCHAR(eMrpView.Mrpprofile,"");
				SETCHAR(eMrpView.Plnd_Delry,"0");
				SETBCD(eMrpView.Assy_Scrap,myzeroDup2,2);
				SETBCD(eMrpView.Safety_Stk,myzeroDup3,3);
				SETBCD(eMrpView.Minlotsize,myzeroDup3,3);
				SETBCD(eMrpView.Maxlotsize,myzeroDup3,3);
				SETBCD(eMrpView.Round_Val,myzeroDup3,3);
				SETBCD(eMrpView.Ord_Costs,myzeroDup4,4);
				SETCHAR(eMrpView.Stor_Costs,"");

				SETCHAR(eMrpView.Discontinu,"");
				SETCHAR(eMrpView.Eff_O_Day,"");
				SETCHAR(eMrpView.Follow_Up,"");
				SETCHAR(eMrpView.Backflush,"");
				SETCHAR(eMrpView.Production_Scheduler,"");
				SETBCD(eMrpView.Proc_Time,myzeroDup2,2);
				SETBCD(eMrpView.Setuptime,myzeroDup2,2);
				SETBCD(eMrpView.Interop,myzeroDup2,2);
				SETBCD(eMrpView.Base_Qty,myzeroDup3,3);
				SETBCD(eMrpView.Inhseprodt,"0",0);
				SETBCD(eMrpView.Stgeperiod,"0",0);
				SETCHAR(eMrpView.Stge_Pd_Un,"");
				SETCHAR(eMrpView.Stge_Pd_Un_Iso,"");
				SETBCD(eMrpView.Over_Tol,myzeroDup1,1);
				SETCHAR(eMrpView.Unlimited,"");
				SETBCD(eMrpView.Under_Tol,myzeroDup1,1);
				SETBCD(eMrpView.Replentime,"0",0);
				SETCHAR(eMrpView.Replace_Pt,"");
				SETCHAR(eMrpView.Loadinggrp,"");
				SETCHAR(eMrpView.Batch_Mgmt,"");
				SETBCD(eMrpView.Serv_Level,myzeroDup1,1);
				SETCHAR(eMrpView.Fy_Variant,"");
				SETCHAR(eMrpView.Corr_Fact,"");
				SETBCD(eMrpView.Setup_Time,myzeroDup2,2);
				SETBCD(eMrpView.Base_Qty_Plan,myzeroDup3,3);
				SETBCD(eMrpView.Ship_Proc_Time,myzeroDup2,2);
				SETCHAR(eMrpView.Sup_Source,"");
				SETCHAR(eMrpView.Auto_P_Ord,"");
				SETCHAR(eMrpView.Sourcelist,"");
				SETCHAR(eMrpView.Comm_Code,"");
				SETCHAR(eMrpView.Countryori,"");
				SETCHAR(eMrpView.Countryori_Iso,"");
				SETCHAR(eMrpView.Regionorig,"");
				SETCHAR(eMrpView.Comm_Co_Un,"");
				SETCHAR(eMrpView.Comm_Co_Un_Iso,"");
				SETCHAR(eMrpView.Expimpgrp,"");
				SETNUM(eMrpView.Pl_Ti_Fnce,"000");
				SETCHAR(eMrpView.Consummode,"");
				SETNUM(eMrpView.Bwd_Cons,"000");
				SETNUM(eMrpView.Fwd_Cons,"000");
				SETCHAR(eMrpView.Bom_Usage,"");
				SETCHAR(eMrpView.Planlistgrp,"");
				SETCHAR(eMrpView.Planlistcnt,"");
				SETCHAR(eMrpView.Specprocty,"");
				SETCHAR(eMrpView.Prod_Unit,"");
				SETCHAR(eMrpView.Prod_Unit_Iso,"");
				//SETCHAR(eMrpView.Iss_St_Loc,store_locDup);
				//SETCHAR(eMrpView.Iss_St_Loc,"");
				SETBCD(eMrpView.Comp_Scrap,myzeroDup2,2);
				SETBCD(eMrpView.Cycle_Time,"0",0);
				SETCHAR(eMrpView.Covprofile,"");
				SETCHAR(eMrpView.Cc_Ph_Inv,"");
				SETCHAR(eMrpView.Serno_Prof,"");
				SETCHAR(eMrpView.Neg_Stocks,"");
				SETCHAR(eMrpView.Qm_Rgmts,"");
				SETCHAR(eMrpView.Plng_Cycle,"");
				SETCHAR(eMrpView.Round_Prof,"");
				SETCHAR(eMrpView.Refmatcons,"");
				SETCHAR(eMrpView.D_To_Ref_M,"");
				SETBCD(eMrpView.Mult_Ref_M,myzeroDup2,2);
				SETCHAR(eMrpView.Auto_Reset,"");
				SETCHAR(eMrpView.Ex_Cert_Id,"");
				SETCHAR(eMrpView.Ex_Cert_No_New,"");
				SETCHAR(eMrpView.Ex_Cert_Dt,"");
				SETCHAR(eMrpView.Milit_Id,"");
				SETBCD(eMrpView.Insp_Int,"0",0);
				SETCHAR(eMrpView.Co_Product,"");
				SETCHAR(eMrpView.Plan_Strgp,"");
				//SETCHAR(eMrpView.Sloc_Exprc,store_locDup);
				//SETCHAR(eMrpView.Sloc_Exprc,"");
				SETCHAR(eMrpView.Cc_Fixed,"");
				SETCHAR(eMrpView.Qm_Authgrp,"");
				SETCHAR(eMrpView.Task_List_Type,"");
				SETCHAR(eMrpView.Prodprof,"");
				SETCHAR(eMrpView.Safty_T_Id,"");
				SETNUM(eMrpView.Safetytime,"00");
				SETCHAR(eMrpView.Plord_Ctrl,"");
				SETCHAR(eMrpView.Batchentry,"");
				SETDATE(eMrpView.Pvalidfrom,"");
				SETCHAR(eMrpView.Matfrgtgrp,"");
				SETCHAR(eMrpView.Prodverscs,"");
				SETCHAR(eMrpView.Mat_Cfop,"");
				SETCHAR(eMrpView.Eu_List_No,"");
				SETCHAR(eMrpView.Eu_Mat_Grp,"");
				SETCHAR(eMrpView.Cas_No,"");
				SETCHAR(eMrpView.Prodcom_No,"");
				SETCHAR(eMrpView.Ctrl_Code,"");
				SETCHAR(eMrpView.Jit_Relvt,"");
				SETCHAR(eMrpView.Mat_Grp_Trans,"");
				SETCHAR(eMrpView.Handlg_Grp,"");
				SETCHAR(eMrpView.Supply_Area,"");
				SETCHAR(eMrpView.Fair_Share_Rule,"");
				SETCHAR(eMrpView.Push_Distrib,"");
				SETBCD(eMrpView.Deploy_Horiz,"0",0);
				SETBCD(eMrpView.Min_Lot_Size,myzeroDup3,3);
				SETBCD(eMrpView.Max_Lot_Size,myzeroDup3,3);
				SETBCD(eMrpView.Fix_Lot_Size,myzeroDup3,3);
				SETBCD(eMrpView.Lot_Increment,myzeroDup3,3);
				SETCHAR(eMrpView.Prod_Conv_Type,"");
				SETCHAR(eMrpView.Distr_Prof,"");
				SETCHAR(eMrpView.Period_Profile_Safety_Time,"");
				SETCHAR(eMrpView.Fxd_Price,"");
				SETCHAR(eMrpView.Avail_Check_All_Proj_Segments,"");
				SETCHAR(eMrpView.Overallprf,"");
				SETCHAR(eMrpView.Mrp_Relevancy_Dep_Requirements,"");
				SETBCD(eMrpView.Min_Safety_Stk,myzeroDup2,2);
				SETCHAR(eMrpView.No_Costing,"");
				SETCHAR(eMrpView.Unit_Group,"");
				SETCHAR(eMrpView.Follow_Up_External,"");
				SETCHAR(eMrpView.Follow_Up_Guid,"");
				SETCHAR(eMrpView.Follow_Up_Version,"");
				SETCHAR(eMrpView.Refmatcons_External,"");
				SETCHAR(eMrpView.Refmatcons_Guid,"");
				SETCHAR(eMrpView.Refmatcons_Version,"");
				SETCHAR(eMrpView.Rotation_Date,"");
				SETCHAR(eMrpView.Original_Batch_Flag,"");
				SETCHAR(eMrpView.Original_Batch_Ref_Material,"");
				SETCHAR(eMrpView.Original_Batch_Ref_Material_E,"");
				SETCHAR(eMrpView.Original_Batch_Ref_Material_V,"");
				SETCHAR(eMrpView.Original_Batch_Ref_Material_G,"");

				SETCHAR(eMrpViewx.Plant,plantcode);
				SETCHAR(eMrpViewx.Del_Flag,"");
				SETCHAR(eMrpViewx.Crit_Part,"");
				SETCHAR(eMrpViewx.Pur_Group,"");
				SETCHAR(eMrpViewx.Mrpprofile,"");
				SETCHAR(eMrpViewx.Plnd_Delry,"");
				SETCHAR(eMrpViewx.Assy_Scrap,"");
				SETCHAR(eMrpViewx.Safety_Stk,"");
				SETCHAR(eMrpViewx.Minlotsize,"");
				SETCHAR(eMrpViewx.Maxlotsize,"");
				SETCHAR(eMrpViewx.Round_Val,"");
				SETCHAR(eMrpViewx.Ord_Costs,"");
				SETCHAR(eMrpViewx.Stor_Costs,"");

				SETCHAR(eMrpViewx.Discontinu,"");
				SETCHAR(eMrpViewx.Eff_O_Day,"");
				SETCHAR(eMrpViewx.Follow_Up,"");
				SETCHAR(eMrpViewx.Backflush,"");
				SETCHAR(eMrpViewx.Production_Scheduler,"");
				SETCHAR(eMrpViewx.Proc_Time,"");
				SETCHAR(eMrpViewx.Setuptime,"");
				SETCHAR(eMrpViewx.Interop,"");
				SETCHAR(eMrpViewx.Base_Qty,"");
				SETCHAR(eMrpViewx.Inhseprodt,"");
				SETCHAR(eMrpViewx.Stgeperiod,"");
				SETCHAR(eMrpViewx.Stge_Pd_Un,"");
				SETCHAR(eMrpViewx.Stge_Pd_Un_Iso,"");
				SETCHAR(eMrpViewx.Over_Tol,"");
				SETCHAR(eMrpViewx.Unlimited,"");
				SETCHAR(eMrpViewx.Under_Tol,"");
				SETCHAR(eMrpViewx.Replentime,"");
				SETCHAR(eMrpViewx.Replace_Pt,"");
				SETCHAR(eMrpViewx.Loadinggrp,"");
				SETCHAR(eMrpViewx.Batch_Mgmt,"");
				SETCHAR(eMrpViewx.Serv_Level,"");
				SETCHAR(eMrpViewx.Fy_Variant,"");
				SETCHAR(eMrpViewx.Corr_Fact,"");
				SETCHAR(eMrpViewx.Setup_Time,"");
				SETCHAR(eMrpViewx.Base_Qty_Plan,"");
				SETCHAR(eMrpViewx.Ship_Proc_Time,"");
				SETCHAR(eMrpViewx.Sup_Source,"");
				SETCHAR(eMrpViewx.Auto_P_Ord,"");
				SETCHAR(eMrpViewx.Sourcelist,"");
				SETCHAR(eMrpViewx.Comm_Code,"");
				SETCHAR(eMrpViewx.Countryori,"");
				SETCHAR(eMrpViewx.Countryori_Iso,"");
				SETCHAR(eMrpViewx.Regionorig,"");
				SETCHAR(eMrpViewx.Comm_Co_Un,"");
				SETCHAR(eMrpViewx.Comm_Co_Un_Iso,"");
				SETCHAR(eMrpViewx.Expimpgrp,"");
				SETCHAR(eMrpViewx.Pl_Ti_Fncve,"");
				SETCHAR(eMrpViewx.Consummode,"");
				SETCHAR(eMrpViewx.Bwd_Cons,"");
				SETCHAR(eMrpViewx.Fwd_Cons,"");
				SETCHAR(eMrpViewx.Bom_Usage,"");
				SETCHAR(eMrpViewx.Planlistgrp,"");
				SETCHAR(eMrpViewx.Planlistcnt,"");
				SETCHAR(eMrpViewx.Specprocty,"");
				SETCHAR(eMrpViewx.Prod_Unit,"");
				SETCHAR(eMrpViewx.Prod_Unit_Iso,"");
				//SETCHAR(eMrpViewx.Iss_St_Loc,"X");
				//SETCHAR(eMrpViewx.Iss_St_Loc,"");
				SETCHAR(eMrpViewx.Comp_Scrap,"");
				SETCHAR(eMrpViewx.Cycle_Time,"");
				SETCHAR(eMrpViewx.Covprofile,"");
				SETCHAR(eMrpViewx.Cc_Ph_Inv,"");
				SETCHAR(eMrpViewx.Serno_Prof,"");
				SETCHAR(eMrpViewx.Neg_Stocks,"");
				SETCHAR(eMrpViewx.Qm_Rgmts,"");
				SETCHAR(eMrpViewx.Plng_Cycle,"");
				SETCHAR(eMrpViewx.Round_Prof,"");
				SETCHAR(eMrpViewx.Refmatcons,"");
				SETCHAR(eMrpViewx.D_To_Ref_M,"");
				SETCHAR(eMrpViewx.Mult_Ref_M,"");
				SETCHAR(eMrpViewx.Auto_Reset,"");
				SETCHAR(eMrpViewx.Ex_Cert_Id,"");
				SETCHAR(eMrpViewx.Ex_Cert_No_New,"");
				SETCHAR(eMrpViewx.Ex_Cert_Dt,"");
				SETCHAR(eMrpViewx.Milit_Id,"");
				SETCHAR(eMrpViewx.Insp_Int,"");
				SETCHAR(eMrpViewx.Co_Product,"");
				SETCHAR(eMrpViewx.Plan_Strgp,"");
				//SETCHAR(eMrpViewx.Sloc_Exprc,"X");
				//SETCHAR(eMrpViewx.Sloc_Exprc,"");
				SETCHAR(eMrpViewx.Cc_Fixed,"");
				SETCHAR(eMrpViewx.Qm_Authgrp,"");
				SETCHAR(eMrpViewx.Task_List_Type,"");
				SETCHAR(eMrpViewx.Prodprof,"");
				SETCHAR(eMrpViewx.Safty_T_Id,"");
				SETCHAR(eMrpViewx.Safetytime,"");
				SETCHAR(eMrpViewx.Plord_Ctrl,"");
				SETCHAR(eMrpViewx.Batchentry,"");
				SETCHAR(eMrpViewx.Pvalidfrom,"");
				SETCHAR(eMrpViewx.Matfrgtgrp,"");
				SETCHAR(eMrpViewx.Prodverscs,"");
				SETCHAR(eMrpViewx.Mat_Cfop,"");
				SETCHAR(eMrpViewx.Eu_List_No,"");
				SETCHAR(eMrpViewx.Eu_Mat_Grp,"");
				SETCHAR(eMrpViewx.Cas_No,"");
				SETCHAR(eMrpViewx.Prodcom_No,"");
				SETCHAR(eMrpViewx.Ctrl_Code,"");
				SETCHAR(eMrpViewx.Jit_Relvt,"");
				SETCHAR(eMrpViewx.Mat_Grp_Trans,"");
				SETCHAR(eMrpViewx.Handlg_Grp,"");
				SETCHAR(eMrpViewx.Supply_Area,"");
				SETCHAR(eMrpViewx.Fair_Share_Rule,"");
				SETCHAR(eMrpViewx.Push_Distrib,"");
				SETCHAR(eMrpViewx.Deploy_Horiz,"");
				SETCHAR(eMrpViewx.Min_Lot_Size,"");
				SETCHAR(eMrpViewx.Max_Lot_Size,"");
				SETCHAR(eMrpViewx.Fix_Lot_Size,"");
				SETCHAR(eMrpViewx.Lot_Increment,"");
				SETCHAR(eMrpViewx.Prod_Conv_Type,"");
				SETCHAR(eMrpViewx.Distr_Prof,"");
				SETCHAR(eMrpViewx.Period_Profile_Safety_Time,"");
				SETCHAR(eMrpViewx.Fxd_Price,"");
				SETCHAR(eMrpViewx.Avail_Check_All_Proj_Segments,"");
				SETCHAR(eMrpViewx.Overallprf,"");
				SETCHAR(eMrpViewx.Mrp_Relevancy_Dep_Requirements,"");
				SETCHAR(eMrpViewx.Min_Safety_Stk,"");
				SETCHAR(eMrpViewx.No_Costing,"");
				SETCHAR(eMrpViewx.Unit_Group,"");
				SETCHAR(eMrpViewx.Follow_Up_External,"");
				SETCHAR(eMrpViewx.Follow_Up_Guid,"");
				SETCHAR(eMrpViewx.Follow_Up_Version,"");
				SETCHAR(eMrpViewx.Refmatcons_External,"");
				SETCHAR(eMrpViewx.Refmatcons_Guid,"");
				SETCHAR(eMrpViewx.Refmatcons_Version,"");
				SETCHAR(eMrpViewx.Rotation_Date,"");
				SETCHAR(eMrpViewx.Original_Batch_Flag,"");
				SETCHAR(eMrpViewx.Original_Batch_Ref_Material,"");
				SETCHAR(eMrpViewx.Original_Batch_Ref_Material_E,"");
				SETCHAR(eMrpViewx.Original_Batch_Ref_Material_V,"");
				SETCHAR(eMrpViewx.Original_Batch_Ref_Material_G,"");

				SETCHAR(eBapi_Mpgd.Plant,plantcode);
				SETCHAR(eBapi_Mpgd.Plng_Matl_External,"");
				SETCHAR(eBapi_Mpgd.Plng_Matl_Guid,"");
				SETCHAR(eBapi_Mpgd.Plng_Matl_Version,"");
				SETCHAR(eBapi_Mpgdx.Plant,plantcode);
				SETCHAR(eBapi_Mpgdx.Plng_Matl_External,"");
				SETCHAR(eBapi_Mpgdx.Plng_Matl_Guid,"");
				SETCHAR(eBapi_Mpgdx.Plng_Matl_Version,"");

				SETCHAR(eBapi_Mard.Plant,plantcode);
				SETCHAR(eBapi_Mard.Stge_Loc,"9005");
				SETCHAR(eBapi_Mard.Del_Flag,"");
				SETCHAR(eBapi_Mard.Mrp_Ind,"");
				SETCHAR(eBapi_Mard.Spec_Proc,"");
				SETBCD(eBapi_Mard.Reorder_Pt,myzeroDup3,3);
				SETBCD(eBapi_Mard.Repl_Qty,myzeroDup3,3);
				SETCHAR(eBapi_Mard.Stge_Bin,"");
				SETCHAR(eBapi_Mard.Pickg_Area,"");
				SETFLOAT(&eBapi_Mard.Inv_Corr_Fac,"");

				SETCHAR(eBapi_Mardx.Plant,plantcode);
				SETCHAR(eBapi_Mardx.Stge_Loc,"9005");
				SETCHAR(eBapi_Mardx.Del_Flag,"");
				SETCHAR(eBapi_Mardx.Mrp_Ind,"");
				SETCHAR(eBapi_Mardx.Spec_Proc,"");
				SETCHAR(eBapi_Mardx.Reorder_Pt,"");
				SETCHAR(eBapi_Mardx.Repl_Qty,"");
				SETCHAR(eBapi_Mardx.Stge_Bin,"");
				SETCHAR(eBapi_Mardx.Pickg_Area,"");
				SETCHAR(eBapi_Mardx.Inv_Corr_Fac,"");

				SETCHAR(eAccView.Val_Area,plantcode);
				SETCHAR(eAccView.Val_Type,"");
				SETCHAR(eAccView.Del_Flag,"");
				SETBCD(eAccView.Price_Unit,"",0);
				SETCHAR(eAccView.Pr_Ctrl_Pp,"");
				SETBCD(eAccView.Mov_Pr_Pp,myzeroDup4,4);
				SETBCD(eAccView.Std_Pr_Pp,myzeroDup4,4);
				SETBCD(eAccView.Pr_Unit_Pp,"0",1);
				SETCHAR(eAccView.Vclass_Pp,"");
				SETCHAR(eAccView.Pr_Ctrl_Py,"");
				SETBCD(eAccView.Mov_Pr_Py,myzeroDup4,4);
				SETBCD(eAccView.Std_Pr_Py,myzeroDup4,4);
				SETCHAR(eAccView.Vclass_Py,"");
				SETBCD(eAccView.Pr_Unit_Py,"0",0);
				SETBCD(eAccView.Future_Pr,myzeroDup4,4);
				SETDATE(eAccView.Valid_From,"");
				SETBCD(eAccView.Taxprice_1,myzeroDup4,4);
				SETBCD(eAccView.Commprice1,myzeroDup4,4);
				SETBCD(eAccView.Taxprice_3,myzeroDup4,4);
				SETBCD(eAccView.Commprice3,myzeroDup4,4);
				SETBCD(eAccView.Plnd_Price,myzeroDup4,4);
				SETBCD(eAccView.Plndprice1,myzeroDup4,4);
				SETBCD(eAccView.Plndprice2,myzeroDup4,4);
				SETBCD(eAccView.Plndprice3,myzeroDup4,4);
				SETDATE(eAccView.Plndprdate1,"");
				SETDATE(eAccView.Plndprdate2,"");
				SETDATE(eAccView.Plndprdate3,"");
				SETCHAR(eAccView.Lifo_Fifo,"");
				SETCHAR(eAccView.Poolnumber,"");
				SETBCD(eAccView.Taxprice_2,myzeroDup4,4);
				SETBCD(eAccView.Commprice2,myzeroDup4,4);
				SETNUM(eAccView.Deval_Ind,"00");
				SETCHAR(eAccView.Ml_Active,"");
				SETCHAR(eAccView.Ml_Settle,"");
				SETCHAR(eAccView.Orig_Mat,"X");
				SETCHAR(eAccView.Vm_So_Stk,"");
				SETCHAR(eAccView.Vm_P_Stock,"");
				SETCHAR(eAccView.Matl_Usage,"");
				SETCHAR(eAccView.In_House,"");
				SETBCD(eAccView.Tax_Cml_Un,"0",0);

				SETCHAR(eAccViewx.Val_Area,plantcode);
				SETCHAR(eAccViewx.Val_Type,"");
				SETCHAR(eAccViewx.Del_Flag,"");
				SETCHAR(eAccViewx.Price_Unit,"");
				SETCHAR(eAccViewx.Pr_Ctrl_Pp,"");
				SETCHAR(eAccViewx.Mov_Pr_Pp,"");
				SETCHAR(eAccViewx.Std_Pr_Pp,"");
				SETCHAR(eAccViewx.Pr_Unit_Pp,"");
				SETCHAR(eAccViewx.Vclass_Pp,"");
				SETCHAR(eAccViewx.Pr_Ctrl_Py,"");
				SETCHAR(eAccViewx.Mov_Pr_Py,"");
				SETCHAR(eAccViewx.Std_Pr_Py,"");
				SETCHAR(eAccViewx.Vclass_Py,"");
				SETCHAR(eAccViewx.Pr_Unit_Py,"");
				SETCHAR(eAccViewx.Future_Pr,"");
				SETCHAR(eAccViewx.Valid_From,"");
				SETCHAR(eAccViewx.Taxprice_1,"");
				SETCHAR(eAccViewx.Commprice1,"");
				SETCHAR(eAccViewx.Taxprice_3,"");
				SETCHAR(eAccViewx.Commprice3,"");
				SETCHAR(eAccViewx.Plnd_Price,"");
				SETCHAR(eAccViewx.Plndprice1,"");
				SETCHAR(eAccViewx.Plndprice2,"");
				SETCHAR(eAccViewx.Plndprice3,"");
				SETCHAR(eAccViewx.Plndprdate1,"");
				SETCHAR(eAccViewx.Plndprdate2,"");
				SETCHAR(eAccViewx.Plndprdate3,"");
				SETCHAR(eAccViewx.Lifo_Fifo,"");
				SETCHAR(eAccViewx.Poolnumber,"");
				SETCHAR(eAccViewx.Taxprice_2,"");
				SETCHAR(eAccViewx.Commprice2,"");
				SETCHAR(eAccViewx.Deval_Ind,"");
				SETCHAR(eAccViewx.Ml_Active,"");
				SETCHAR(eAccViewx.Ml_Settle,"");
				SETCHAR(eAccViewx.Orig_Mat,"X");
				SETCHAR(eAccViewx.Vm_So_Stk,"");
				SETCHAR(eAccViewx.Vm_P_Stock,"");
				SETCHAR(eAccViewx.Matl_Usage,"");
				SETCHAR(eAccViewx.In_House,"");
				SETCHAR(eAccViewx.Tax_Cml_Un,"");

				SETCHAR(tBapi_Makt->Langu,"EN");
				SETCHAR(tBapi_Makt->Langu_Iso,"EN");
				SETCHAR(tBapi_Makt->Del_Flag,"");

				SETBCD(tBapi_Marm->Numerator,"1",0);
				SETBCD(tBapi_Marm->Denominatr,"1",0);
				SETCHAR(tBapi_Marm->Ean_Upc,"");
				SETCHAR(tBapi_Marm->Ean_Cat,"");
				SETCHAR(tBapi_Marm->Length,"");
				SETCHAR(tBapi_Marm->Width,"");
				SETCHAR(tBapi_Marm->Height,"");
				SETCHAR(tBapi_Marm->Unit_Dim,"");
				SETCHAR(tBapi_Marm->Unit_Dim_Iso,"");
				SETCHAR(tBapi_Marm->Del_Flag,"");
				SETCHAR(tBapi_Marm->Sub_Uom,"");
				SETCHAR(tBapi_Marm->Sub_Uom_Iso,"");
				SETCHAR(tBapi_Marm->Gtin_Variant,"");

				SETCHAR(tBapi_Marmx->Numerator,"X");
				SETCHAR(tBapi_Marmx->Denominatr,"X");
				SETCHAR(tBapi_Marmx->Ean_Upc,"");
				SETCHAR(tBapi_Marmx->Ean_Cat,"");
				SETCHAR(tBapi_Marmx->Length,"");
				SETCHAR(tBapi_Marmx->Width,"");
				SETCHAR(tBapi_Marmx->Height,"");
				SETCHAR(tBapi_Marmx->Unit_Dim,"");
				SETCHAR(tBapi_Marmx->Unit_Dim_Iso,"");
				SETCHAR(tBapi_Marmx->Sub_Uom,"");
				SETCHAR(tBapi_Marmx->Sub_Uom_Iso,"");
				SETCHAR(tBapi_Marmx->Gtin_Variant,"");


				SETCHAR(tBAPIE1PAREX3->STRUCTURE,"");//BAPI_TE_MARA
				SETCHAR(tBAPIE1PAREX3->VALUEPART1,"");//part_noDupDes
				SETCHAR(tBAPIE1PAREX3->VALUEPART2,"");
				SETCHAR(tBAPIE1PAREX3->VALUEPART3,"");
				SETCHAR(tBAPIE1PAREX3->VALUEPART4,"");

				SETCHAR(tBAPIE1PAREXX3->STRUCTURE,"");//BAPI_TE_MARAX
				SETCHAR(tBAPIE1PAREXX3->VALUEPART1,"");//part_noDupDesx
				SETCHAR(tBAPIE1PAREXX3->VALUEPART2,"");
				SETCHAR(tBAPIE1PAREXX3->VALUEPART3,"");
				SETCHAR(tBAPIE1PAREXX3->VALUEPART4,"");

				RfcRc = zbapi_material_savedata_mrp(hRfc,&eBapiMatHead,&eBasicView,&eBasicViewx,&eMrpView,&eMrpViewx,&eBapi_Mpgd,&eBapi_Mpgdx,&eBapi_Mard,&eBapi_Mardx,&eAccView,&eAccViewx,&eRmmg1_Aennr,&eBapiret2,thBapi_Makt,thBapi_Marm,thBapi_Marmx,thBAPIE1PAREX3,thBAPIE1PAREXX3,xException);

				switch (RfcRc)
				{
					case RFC_OK:
						//sprintf(sap_msg,"%.*s:",sizeof(sap_msg),eBapiret2.Message);
						//GETCHAR(eBapiret2.Message,sap_msg);

						printf("\nMessage for Material %s create Other view extend:%s",part_noDup,eBapiret2.Message); fflush(stdout);
						fprintf(fsuccess,"\nMessage for Material %s create Other view extend:%s",part_noDup,eBapiret2.Message); fflush(fsuccess);
						break;
					case RFC_EXCEPTION:
						printf("\nRFC EXCEPTION: %s",xException); fflush(stdout);
						break;
					case RFC_SYS_EXCEPTION:
						printf("\nSystem Exception Raised!!!"); fflush(stdout);
						break;
					case RFC_FAILURE:
						printf("\nFailure!!!"); fflush(stdout);
						break;
					default:
						printf("\nOther Failure!"); fflush(stdout);
						break;
				}
				if(ItDelete(thBapi_Makt) != 0) rfc_error("ItDelete thBapi_Makt");
				if(ItDelete(thBapi_Marm) != 0) rfc_error("ItDelete tBapi_Marm");
				if(ItDelete(thBapi_Marmx) != 0) rfc_error("ItDelete thBapi_Marmx");
				if(ItDelete(thBAPIE1PAREX3) != 0) rfc_error("ItDelete thBAPIE1PAREX3");
				if(ItDelete(thBAPIE1PAREXX3) != 0)	rfc_error("ItDelete thBAPIE1PAREXX3");
				/*sleep(5);*/
				RfcClose(hRfc);

			}
			if (apl_dml_flag == 2) /*all view will created*/
			{
				printf("\neBapiMatHead.Material:		%s",part_noDup); fflush(stdout);
				printf("\neBapiMatHead.Matl_Type:		%s",mat_type);
				printf("\neBapiMatHead.Ind_Sector:	%s","M");
				printf("\neBapiMatHead.Basic_View:	%s","X");		/*basic view*/
				printf("\neBasicView.Base_Uom:		%s",meas_unit);
				printf("\neBasicView.Base_Uom_Iso:	%s",meas_unit);
				printf("\ntBapi_Marm->Alt_Unit:		%s",meas_unit);
				printf("\ntBapi_Marm->Alt_Unit_Iso:	%s",meas_unit);
				printf("\ntBapi_Marmx->Alt_Unit:		%s",meas_unit);
				printf("\ntBapi_Marmx->Alt_Unit_Iso:	%s",meas_unit);
				printf("\ntBapi_Makt->Matl_Desc:		%s",descDup);
				printf("\neBasicView.Document:		%s",doc_noDup);
				printf("\neBasicView.Doc_Type:		%s",dwg_typeDup);
				printf("\neBasicView.Doc_Vers:		%s",dwg_revDup);
				printf("\neBasicView.No_Sheets:		%s",sheet_noDup);
				printf("\neBasicView.Old_Mat_No:		%s",OldMatNoDup);
				printf("\neBasicView.Net_Weight:		%s",net_wtDup);
				printf("\ntBapi_Marm->Gross_Wt:		%s",gross_wt);
				printf("\neBasicView.Unit_Of_Wt:		%s",unit_wt);
				printf("\neBasicView.Unit_Of_Wt_Iso:	%s",unit_wt);
				printf("\ntBapi_Marm->Unit_Of_Wt:		%s",unit_wt);
				printf("\ntBapi_Marm->Unit_Of_Wt_Iso:	%s",unit_wt);
				printf("\ntBapi_Marm->Volume:			%s",volume);
				printf("\ntBapi_Marm->VolumeUnit:		%s",vol_unit);
				printf("\ntBapi_Marm->VolumeUnit_Iso:	%s",vol_unit);
				printf("\neBasicView.Size_Dim:		%s",sizeDup);
				printf("\neBasicView.Matl_Group:		%s",material_group);
				printf("\neBasicView.Division:		%s",basic_division);
				printf("\neBapiMatHead.Mrp_View:		%s","X");	/*mrp view*/
				printf("\neMrpView.Mrp_Group:			%s",mrp_grp);
				printf("\neMrpView.Abc_Id:			%s",abc_ind);
				printf("\neMrpView.Pur_Status:		%s",mm_pp_status);
				printf("\neMrpView.Mrp_Type:			%s",mrp_type);
				printf("\neMrpView.Reorder_Pt:		%s",reord_pt);
				printf("\neMrpView.Quotausage:		%s",quota_arrangement_usage);
				printf("\neMrpView.Mrp_Ctrler:		%s",mrp_controller);
				printf("\neMrpView.Lotsizekey:		%s",lot_size_key);
				printf("\neMrpView.Fixed_Lot:			%s",fixed_lot_size);
				printf("\neMrpView.Max_Stock:			%s",max_stlvl);
				printf("\neMrpView.Proc_Type:			%s",proc_type);
				printf("\neMrpView.Spproctype:		%s",spl_proc_key);
				printf("\neMrpView.Bulk_Mat:			%s",blk_ind);
				printf("\neMrpView.Sm_Key:			%s",sched_mar_key);
				printf("\neMrpView.Ppc_Pl_Cal:		%s",plan_calendar);
				printf("\neMrpView.Grp_Reqmts:		%s",req_grp);
				printf("\neMrpView.Period_Ind:		%s",period_ind);
				printf("\neMrpView.Mixed_Mrp:			%s",mixedmrp);
				printf("\neMrpView.Availcheck:		%s",avail_chk);
				printf("\neBapi_Mpgd.Plng_Matl:		%s",splan_mat);
				printf("\neBapi_Mpgd.Plng_Plant:		%s",splan_plant);
				printf("\neBapi_Mpgd.Convfactor:		%s",splan_conv_fact);
				printf("\neMrpView.Alternative_Bom:	%s",alternativb);
				printf("\neMrpView.Dep_Req_Id:		%s",ind_collect);
				printf("\neMrpView.Rep_Manuf:			%s",rep_mfg_in);
				printf("\neMrpView.Repmanprof:		%s",rep_mfg);
				printf("\neMrpView.Determ_Grp:		%s",stock_det_group);
				printf("\neBapiMatHead.Storage_View:	%s","X");	/*store view*/
				printf("\neMrpView.Issue_Unit:		%s",uoissue);
				printf("\neMrpView.Issue_Unit_Iso:	%s",uoissue);
				printf("\neMrpView.Profit_Ctr:		%s",profit_centre_sap);		/*profit_centre=0001111010*/
				printf("\neBapiMatHead.Quality_View:	%s","X");	/*quality view*/
				printf("\neMrpView.Ind_Post_To_Insp_Stock:%s",qua_ins_ind);
				printf("\neMrpView.Doc_Reqd:			%s",doc_req);
				printf("\neMrpView.Gr_Pr_Time:		%s",grptime);
				printf("\neBasicView.Catprofile:		%s",catalog_prof);
				printf("\neBasicView.Qm_Procmnt:		%s",qm_proc_ind);
				printf("\neMrpView.Ctrl_Key:			%s",control_key);
				printf("\neMrpView.Cert_Type:			%s",certificate_type);
				printf("\neBapiMatHead.Account_View:	%s","X"); /*accounting view */
				printf("\neAccView.Val_Cat:			%s",valuation_cat);
				printf("\neAccView.Val_Class:			%s",val_class);
				printf("\neAccView.Price_Ctrl:		%s",price_con);
				printf("\neAccView.Std_Price:			%s",std_price);
				printf("\neAccView.Moving_Pr:			%s",moving_avg_price);
				printf("\neBapiMatHead.Cost_View:		%s","X");/*lvl cost view */
				printf("\neAccView.Mat_Origin:		%s",mat_origin);
				printf("\neAccView.Orig_Group:		%s",origin_group);
				printf("\neMrpView.Lot_Size:			%s",cost_lot_size);
				printf("\neAccView.Overhead_Grp:		%s",overhd_grp);
				printf("\neMrpView.Variance_Key:		%s",variance_key);
				printf("\neAccView.Qty_Struct:		%s",with_quantity_structure);
				printf("\neMrpView.Split_Ind:			%s",costing_split_valuation);
				printf("\neMrpViewx.Split_Ind:		%s","");/*Please make it as X*/
				printf("\neRmmg1_Aennr.Aennr:			%s",dml_no_arg);
				printf("\npart_noDupDes:			%s",part_noDupDes);
				printf("\npart_noDupDes:			%s",part_noDupDesx); fflush(stdout);
				/*log file */

				fprintf(fsuccess,"\neBapiMatHead.Material:		%s",part_noDup); fflush(fsuccess);
				fprintf(fsuccess,"\neBapiMatHead.Matl_Type:		%s",mat_type);
				fprintf(fsuccess,"\neBapiMatHead.Ind_Sector:	%s","M");
				fprintf(fsuccess,"\neBapiMatHead.Basic_View:	%s","X");		/*basic view*/
				fprintf(fsuccess,"\neBasicView.Base_Uom:		%s",meas_unit);
				fprintf(fsuccess,"\neBasicView.Base_Uom_Iso:	%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Marm->Alt_Unit:		%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Marm->Alt_Unit_Iso:	%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Marmx->Alt_Unit:		%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Marmx->Alt_Unit_Iso:	%s",meas_unit);
				fprintf(fsuccess,"\ntBapi_Makt->Matl_Desc:		%s",descDup);
				fprintf(fsuccess,"\neBasicView.Document:		%s",doc_noDup);
				fprintf(fsuccess,"\neBasicView.Doc_Type:		%s",dwg_typeDup);
				fprintf(fsuccess,"\neBasicView.Doc_Vers:		%s",dwg_revDup);
				fprintf(fsuccess,"\neBasicView.No_Sheets:		%s",sheet_noDup);
				fprintf(fsuccess,"\neBasicView.Old_Mat_No:		%s",OldMatNoDup);
				fprintf(fsuccess,"\neBasicView.Net_Weight:		%s",net_wtDup);
				fprintf(fsuccess,"\ntBapi_Marm->Gross_Wt:		%s",gross_wt);
				fprintf(fsuccess,"\neBasicView.Unit_Of_Wt:		%s",unit_wt);
				fprintf(fsuccess,"\neBasicView.Unit_Of_Wt_Iso:	%s",unit_wt);
				fprintf(fsuccess,"\ntBapi_Marm->Unit_Of_Wt:		%s",unit_wt);
				fprintf(fsuccess,"\ntBapi_Marm->Unit_Of_Wt_Iso:	%s",unit_wt);
				fprintf(fsuccess,"\ntBapi_Marm->Volume:			%s",volume);
				fprintf(fsuccess,"\ntBapi_Marm->VolumeUnit:		%s",vol_unit);
				fprintf(fsuccess,"\ntBapi_Marm->VolumeUnit_Iso:	%s",vol_unit);
				fprintf(fsuccess,"\neBasicView.Size_Dim:		%s",sizeDup);
				fprintf(fsuccess,"\neBasicView.Matl_Group:		%s",material_group);
				fprintf(fsuccess,"\neBasicView.Division:		%s",basic_division);
				fprintf(fsuccess,"\neBapiMatHead.Mrp_View:		%s","X");	/*mrp view*/
				fprintf(fsuccess,"\neMrpView.Mrp_Group:			%s",mrp_grp);
				fprintf(fsuccess,"\neMrpView.Abc_Id:			%s",abc_ind);
				fprintf(fsuccess,"\neMrpView.Pur_Status:		%s",mm_pp_status);
				fprintf(fsuccess,"\neMrpView.Mrp_Type:			%s",mrp_type);
				fprintf(fsuccess,"\neMrpView.Reorder_Pt:		%s",reord_pt);
				fprintf(fsuccess,"\neMrpView.Quotausage:		%s",quota_arrangement_usage);
				fprintf(fsuccess,"\neMrpView.Mrp_Ctrler:		%s",mrp_controller);
				fprintf(fsuccess,"\neMrpView.Lotsizekey:		%s",lot_size_key);
				fprintf(fsuccess,"\neMrpView.Fixed_Lot:			%s",fixed_lot_size);
				fprintf(fsuccess,"\neMrpView.Max_Stock:			%s",max_stlvl);
				fprintf(fsuccess,"\neMrpView.Proc_Type:			%s",proc_type);
				fprintf(fsuccess,"\neMrpView.Spproctype:		%s",spl_proc_key);
				fprintf(fsuccess,"\neMrpView.Bulk_Mat:			%s",blk_ind);
				fprintf(fsuccess,"\neMrpView.Sm_Key:			%s",sched_mar_key);
				fprintf(fsuccess,"\neMrpView.Ppc_Pl_Cal:		%s",plan_calendar);
				fprintf(fsuccess,"\neMrpView.Grp_Reqmts:		%s",req_grp);
				fprintf(fsuccess,"\neMrpView.Period_Ind:		%s",period_ind);
				fprintf(fsuccess,"\neMrpView.Mixed_Mrp:			%s",mixedmrp);
				fprintf(fsuccess,"\neMrpView.Availcheck:		%s",avail_chk);
				fprintf(fsuccess,"\neBapi_Mpgd.Plng_Matl:		%s",splan_mat);
				fprintf(fsuccess,"\neBapi_Mpgd.Plng_Plant:		%s",splan_plant);
				fprintf(fsuccess,"\neBapi_Mpgd.Convfactor:		%s",splan_conv_fact);
				fprintf(fsuccess,"\neMrpView.Alternative_Bom:	%s",alternativb);
				fprintf(fsuccess,"\neMrpView.Dep_Req_Id:		%s",ind_collect);
				fprintf(fsuccess,"\neMrpView.Rep_Manuf:			%s",rep_mfg_in);
				fprintf(fsuccess,"\neMrpView.Repmanprof:		%s",rep_mfg);
				fprintf(fsuccess,"\neMrpView.Determ_Grp:		%s",stock_det_group);
				fprintf(fsuccess,"\neBapiMatHead.Storage_View:	%s","X");	/*store view*/
				fprintf(fsuccess,"\neMrpView.Issue_Unit:		%s",uoissue);
				fprintf(fsuccess,"\neMrpView.Issue_Unit_Iso:	%s",uoissue);
				fprintf(fsuccess,"\neMrpView.Profit_Ctr:		%s",profit_centre_sap);		/*profit_centre=0001111010*/
				fprintf(fsuccess,"\neBapiMatHead.Quality_View:	%s","X");	/*quality view*/
				fprintf(fsuccess,"\neMrpView.Ind_Post_To_Insp_Stock:%s",qua_ins_ind);
				fprintf(fsuccess,"\neMrpView.Doc_Reqd:			%s",doc_req);
				fprintf(fsuccess,"\neMrpView.Gr_Pr_Time:		%s",grptime);
				fprintf(fsuccess,"\neBasicView.Catprofile:		%s",catalog_prof);
				fprintf(fsuccess,"\neBasicView.Qm_Procmnt:		%s",qm_proc_ind);
				fprintf(fsuccess,"\neMrpView.Ctrl_Key:			%s",control_key);
				fprintf(fsuccess,"\neMrpView.Cert_Type:			%s",certificate_type);
				fprintf(fsuccess,"\neBapiMatHead.Account_View:	%s","X"); /*accounting view */
				fprintf(fsuccess,"\neAccView.Val_Cat:			%s",valuation_cat);
				fprintf(fsuccess,"\neAccView.Val_Class:			%s",val_class);
				fprintf(fsuccess,"\neAccView.Price_Ctrl:		%s",price_con);
				fprintf(fsuccess,"\neAccView.Std_Price:			%s",std_price);
				fprintf(fsuccess,"\neAccView.Moving_Pr:			%s",moving_avg_price);
				fprintf(fsuccess,"\neBapiMatHead.Cost_View:		%s","X");/*lvl cost view */
				fprintf(fsuccess,"\neAccView.Mat_Origin:		%s",mat_origin);
				fprintf(fsuccess,"\neAccView.Orig_Group:		%s",origin_group);
				fprintf(fsuccess,"\neMrpView.Lot_Size:			%s",cost_lot_size);
				fprintf(fsuccess,"\neAccView.Overhead_Grp:		%s",overhd_grp);
				fprintf(fsuccess,"\neMrpView.Variance_Key:		%s",variance_key);
				fprintf(fsuccess,"\neAccView.Qty_Struct:		%s",with_quantity_structure);
				fprintf(fsuccess,"\neMrpView.Split_Ind:			%s",costing_split_valuation);
				fprintf(fsuccess,"\neMrpViewx.Split_Ind:		%s","");/*Please make it as X*/
				fprintf(fsuccess,"\neRmmg1_Aennr.Aennr:			%s",dml_no_arg);
				fprintf(fsuccess,"\npart_noDupDes:			%s",part_noDupDes);
				fprintf(fsuccess,"\npart_noDupDes:			%s",part_noDupDesx); fflush(fsuccess);

				printf("\nCreating ALL Views..."); fflush(stdout);

				hRfc = BapiLogon();
				thBapi_Makt = ITAB_NULL;
				thBapi_Marm = ITAB_NULL;
				thBapi_Marmx = ITAB_NULL;
				thBAPIE1PAREX3 = ITAB_NULL;
				thBAPIE1PAREXX3 = ITAB_NULL;

				if (thBapi_Makt==ITAB_NULL)
				{
					thBapi_Makt = ItCreate("MATERIALDESCRIPTION",sizeof(BAPI_MAKT),0,0);
					if (thBapi_Makt==ITAB_NULL) rfc_error("ItCreate MATERIALDESCRIPTION");
				}
				else if (ItFree(thBapi_Makt) != 0) rfc_error("ItFree MATERIALDESCRIPTION");

				if (thBapi_Marm==ITAB_NULL)
				{
					 thBapi_Marm = ItCreate("UNITSOFMEASURE",sizeof(BAPI_MARM),0,0);
					 if (thBapi_Marm==ITAB_NULL) rfc_error("ItCreate UNITSOFMEASURE");
				}
				else if (ItFree(thBapi_Marm)!= 0) rfc_error("ItFree UNITSOFMEASURE");

				if (thBapi_Marmx==ITAB_NULL)
				{
					 thBapi_Marmx = ItCreate("UNITSOFMEASUREX",sizeof(BAPI_MARMX),0,0);
					if (thBapi_Marmx==ITAB_NULL) rfc_error("ItCreate UNITSOFMEASUREX");
				}
				else if (ItFree(thBapi_Marmx)!= 0) rfc_error("ItFree UNITSOFMEASUREX");

				if (thBAPIE1PAREX3==ITAB_NULL)
				{
					thBAPIE1PAREX3 = ItCreate("EXTENSIONIN",sizeof(BAPIE1PAREX3),0,0);
					if (thBAPIE1PAREX3==ITAB_NULL)
						rfc_error("ItCreate EXTENSIONIN");
				}
				else
				{
					if (ItFree(thBAPIE1PAREX3)!= 0)
						rfc_error("ItFree EXTENSIONIN");
				}

				if (thBAPIE1PAREXX3==ITAB_NULL)
				{
					thBAPIE1PAREXX3 = ItCreate("EXTENSIONINX",sizeof(BAPIE1PAREXX3),0,0);
					if (thBAPIE1PAREXX3==ITAB_NULL)
						rfc_error("ItCreate EXTENSIONINX");
				}
				else
				{
					if (ItFree(thBAPIE1PAREXX3)!= 0)
						rfc_error("ItFree EXTENSIONINX");
				}

				tBapi_Makt = ItAppLine (thBapi_Makt) ;
				tBapi_Marm  = ItAppLine (thBapi_Marm) ;
				tBapi_Marmx = ItAppLine (thBapi_Marmx) ;
				tBAPIE1PAREX3 = ItAppLine(thBAPIE1PAREX3) ;
				tBAPIE1PAREXX3 = ItAppLine(thBAPIE1PAREXX3) ;


				if (tBapi_Makt == NULL)
				rfc_error("ItAppLine BAPI_MAKT");
				if (tBapi_Marm == NULL)
				rfc_error("ItAppLine BAPI_MARM second table");
				if (tBapi_Marmx == NULL)
				rfc_error("ItAppLine BAPI_MARMX");
				if (tBAPIE1PAREX3 == NULL) rfc_error("ItAppLine BAPIE1PAREX3");
				if (tBAPIE1PAREXX3 == NULL) rfc_error("ItAppLine BAPIE1PAREXX3");

				SETCHAR(eBapiMatHead.Material,part_noDup);
				SETCHAR(eBapiMatHead.Matl_Type,mat_type);
				SETCHAR(eBapiMatHead.Ind_Sector,"M");
				SETCHAR(eBapiMatHead.Basic_View,"X");		/*basic view*/
				SETCHAR(eBasicView.Base_Uom,meas_unit);
				SETCHAR(eBasicView.Base_Uom_Iso,meas_unit);
				SETCHAR(eBasicViewx.Base_Uom,"X");
				SETCHAR(eBasicViewx.Base_Uom_Iso,"X");
				SETCHAR(tBapi_Marm->Alt_Unit,meas_unit);
				SETCHAR(tBapi_Marm->Alt_Unit_Iso,meas_unit);
				SETCHAR(tBapi_Marmx->Alt_Unit,meas_unit);
				SETCHAR(tBapi_Marmx->Alt_Unit_Iso,meas_unit);
				SETCHAR(tBapi_Makt->Matl_Desc,descDup);
				if(tc_strlen(doc_noDup)==0)
				{
						SETCHAR(eBasicView.Document,"");
						SETCHAR(eBasicViewx.Document,"");
				}
				else
				{
						SETCHAR(eBasicView.Document,doc_noDup);
						SETCHAR(eBasicViewx.Document,"X");
				}
				if(tc_strlen(dwg_typeDup)==0)
				{
						SETCHAR(eBasicView.Doc_Type,"");
						SETCHAR(eBasicViewx.Doc_Type,"");
				}
				else
				{
						SETCHAR(eBasicView.Doc_Type,dwg_typeDup);
						SETCHAR(eBasicViewx.Doc_Type,"X");
				}
				if(tc_strlen(dwg_revDup)==0)
				{
						SETCHAR(eBasicView.Doc_Vers,"");
						SETCHAR(eBasicViewx.Doc_Vers,"");
				}
				else
				{
						SETCHAR(eBasicView.Doc_Vers,dwg_revDup);
						SETCHAR(eBasicViewx.Doc_Vers,"X");
				}

				if(tc_strlen(sheet_noDup)==0)
				{
						SETCHAR(eBasicView.No_Sheets,"");
						SETCHAR(eBasicViewx.No_Sheets,"");
				}
				else
				{
						SETNUM(eBasicView.No_Sheets,sheet_noDup);
						SETCHAR(eBasicViewx.No_Sheets,"X");
				}

				SETCHAR(eBasicView.Old_Mat_No,OldMatNoDup);
				SETCHAR(eBasicViewx.Old_Mat_No,"X");

				SETBCD(eBasicView.Net_Weight,myzeroDup3,3);
				SETCHAR(eBasicViewx.Net_Weight,"X");

				SETBCD(tBapi_Marm->Gross_Wt,gross_wt,3);
				SETCHAR(tBapi_Marmx->Gross_Wt,"X");

				SETCHAR(eBasicView.Unit_Of_Wt,unit_wt);
				SETCHAR(eBasicView.Unit_Of_Wt_Iso,unit_wt);
				SETCHAR(eBasicViewx.Unit_Of_Wt,"X");
				SETCHAR(eBasicViewx.Unit_Of_Wt_Iso,"X");

				SETCHAR(tBapi_Marm->Unit_Of_Wt,unit_wt);
				SETCHAR(tBapi_Marm->Unit_Of_Wt_Iso,unit_wt);
				SETCHAR(tBapi_Marmx->Unit_Of_Wt,"X");
				SETCHAR(tBapi_Marmx->Unit_Of_Wt_Iso,"X");

				SETBCD(tBapi_Marm->Volume,myzeroDup3,3);
				SETCHAR(tBapi_Marm->VolumeUnit,vol_unit);
				SETCHAR(tBapi_Marm->VolumeUnit_Iso,vol_unit);
				SETCHAR(tBapi_Marmx->Volume,"X");
				SETCHAR(tBapi_Marmx->VolumeUnit,"X");
				SETCHAR(tBapi_Marmx->VolumeUnit_Iso,"X");

				if(tc_strlen(sizeDup)==0)
				{
						SETCHAR(eBasicView.Size_Dim,"");
						SETCHAR(eBasicViewx.Size_Dim,"");
				}
				else
				{
					SETCHAR(eBasicView.Size_Dim,sizeDup);/*sizeDup*/
					SETCHAR(eBasicViewx.Size_Dim,"X");
				}

				if(tc_strlen(store_locDup)>0)
				{
					//string strtoupper ( string store_locDup );
					//store_locDup1 = strtoupper(store_locDup);

					printf ("Before conversion: %s\n", store_locDup);
					for (p = store_locDup; *p != '\0'; ++p)
					{
						*p = toupper(*p);
					}
					printf ("After conversion: %s\n", store_locDup);

					//printf("\nstore location converted to capital letters, if in small letters  : %s",store_locDup1);
					SETCHAR(eMrpView.Iss_St_Loc,store_locDup);
					SETCHAR(eMrpView.Sloc_Exprc,store_locDup);
					SETCHAR(eMrpViewx.Iss_St_Loc,"X");
					SETCHAR(eMrpViewx.Sloc_Exprc,"X");
					printf("\nstore location is : %s",store_locDup);
					fprintf(fsuccess,"\nstore location is : %s",store_locDup);
				}
				else
				{
					SETCHAR(eMrpView.Iss_St_Loc,"");
					SETCHAR(eMrpView.Sloc_Exprc,"");
					SETCHAR(eMrpViewx.Iss_St_Loc,"");
					SETCHAR(eMrpViewx.Sloc_Exprc,"");

					//printf("\nstore location is : %s",store_locDup); fflush(stdout);
					//fprintf(fsuccess,"\nstore location is : %s",store_locDup); fflush(fsuccess);
					printf("\nstore location is null"); 
				}

				SETCHAR(eBasicView.Matl_Group,material_group);
				SETCHAR(eBasicViewx.Matl_Group,"X");

				SETCHAR(eBasicView.Division,basic_division);
				SETCHAR(eBasicViewx.Division,"X");

				SETCHAR(eBapiMatHead.Mrp_View,"X");	/*mrp view*/
				SETCHAR(eMrpView.Mrp_Group,mrp_grp);
				SETCHAR(eMrpViewx.Mrp_Group,"X");
				SETCHAR(eMrpView.Abc_Id,abc_ind);
				SETCHAR(eMrpViewx.Abc_Id,"X");
				if(tc_strlen(mm_pp_status)==0)
				{
					SETCHAR(eMrpView.Pur_Status," ");
					SETCHAR(eMrpViewx.Pur_Status,"X");
					printf("\nMaterial  status is........ABC : %s",mm_pp_status);
					fprintf(fsuccess,"\nMaterial  status is........ABC : %s",mm_pp_status);
				}
				else
				{
					SETCHAR(eMrpView.Pur_Status,mm_pp_status);
					SETCHAR(eMrpViewx.Pur_Status,"X");
					printf("\nMaterial  status is........ABC1 : %s",mm_pp_status);
					fprintf(fsuccess,"\nMaterial  status is........ABC1 : %s",mm_pp_status);
				}
				//SETCHAR(eMrpView.Pur_Status,mm_pp_status);
				//SETCHAR(eMrpViewx.Pur_Status,"X");
				SETCHAR(eMrpView.Mrp_Type,mrp_type);
				SETCHAR(eMrpViewx.Mrp_Type,"X");
				SETBCD(eMrpView.Reorder_Pt,reord_pt,3);
				SETCHAR(eMrpViewx.Reorder_Pt,"X");
				SETCHAR(eMrpView.Quotausage,quota_arrangement_usage);
				SETCHAR(eMrpViewx.Quotausage,"X");
				SETCHAR(eMrpView.Mrp_Ctrler,mrp_controller);
				SETCHAR(eMrpViewx.Mrp_Ctrler,"X");
				SETCHAR(eMrpView.Lotsizekey,lot_size_key);
				SETCHAR(eMrpViewx.Lotsizekey,"X");
				SETBCD(eMrpView.Fixed_Lot,fixed_lot_size,3);
				SETCHAR(eMrpViewx.Fixed_Lot,"X");
				SETBCD(eMrpView.Max_Stock,max_stlvl,3);
				SETCHAR(eMrpViewx.Max_Stock,"X");
				SETCHAR(eMrpView.Proc_Type,proc_type);
				SETCHAR(eMrpViewx.Proc_Type,"X");
				SETCHAR(eMrpView.Spproctype,spl_proc_key);
				SETCHAR(eMrpViewx.Spproctype,"X");
				SETCHAR(eMrpView.Bulk_Mat,blk_ind);
				SETCHAR(eMrpViewx.Bulk_Mat,"X");
				SETCHAR(eMrpView.Sm_Key,sched_mar_key);
				SETCHAR(eMrpViewx.Sm_Key,"X");
				SETCHAR(eMrpView.Ppc_Pl_Cal,plan_calendar);

				SETCHAR(eMrpViewx.Ppc_Pl_Cal,"X");
				SETCHAR(eMrpView.Grp_Reqmts,req_grp);
				SETCHAR(eMrpViewx.Grp_Reqmts,"X");
				SETCHAR(eMrpView.Period_Ind,period_ind);
				SETCHAR(eMrpViewx.Period_Ind,"X");
				SETCHAR(eMrpView.Mixed_Mrp,mixedmrp);
				SETCHAR(eMrpViewx.Mixed_Mrp,"X");
				SETCHAR(eMrpView.Availcheck,avail_chk);
				SETCHAR(eMrpViewx.Availcheck,"X");
				SETCHAR(eBapi_Mpgd.Plng_Matl,splan_mat);
				SETCHAR(eBapi_Mpgdx.Plng_Matl,"X");
				SETCHAR(eBapi_Mpgd.Plng_Plant,splan_plant);
				SETCHAR(eBapi_Mpgdx.Plng_Plant,"X");
				SETCHAR(eBapi_Mpgd.Convfactor,splan_conv_fact);
				SETCHAR(eBapi_Mpgdx.Convfactor,"X");
				SETCHAR(eMrpView.Alternative_Bom,"");
				SETCHAR(eMrpViewx.Alternative_Bom,"");
				SETCHAR(eMrpView.Alt_Bom_Id,alternativb);
				SETCHAR(eMrpViewx.Alt_Bom_Id,"X");
				SETCHAR(eMrpView.Dep_Req_Id,ind_collect);
				SETCHAR(eMrpViewx.Dep_Req_Id,"X");

				//SETCHAR(eMrpView.Rep_Manuf,"");/*rep_mfg_in*/
				//SETCHAR(eMrpViewx.Rep_Manuf,"");/*X*/
				//SETCHAR(eMrpView.Repmanprof,"");/*rep_mfg*/
				//SETCHAR(eMrpViewx.Repmanprof,"");/*X*/

				if(tc_strlen(rep_mfg)>0)		//rep_mfg modified on 14.02.2017 for TE02
				{
					SETCHAR(eMrpView.Rep_Manuf,"X");/*rep_mfg_in*/
					SETCHAR(eMrpViewx.Rep_Manuf,"X");/*X*/
					SETCHAR(eMrpView.Repmanprof,rep_mfg);/*rep_mfg*/
					SETCHAR(eMrpViewx.Repmanprof,"X");/*X*/
				}
				else
				{
					SETCHAR(eMrpView.Rep_Manuf,"");/*rep_mfg_in*/
					SETCHAR(eMrpViewx.Rep_Manuf,"X");/*X*/
					SETCHAR(eMrpView.Repmanprof,"");/*rep_mfg*/
					SETCHAR(eMrpViewx.Repmanprof,"X");/*X*/
				}

				SETCHAR(eMrpView.Determ_Grp,stock_det_group);

				SETCHAR(eMrpViewx.Determ_Grp,"X");
				SETCHAR(eBapiMatHead.Storage_View,"X");	/*store view*/
				SETCHAR(eMrpView.Issue_Unit,uoissue);
				SETCHAR(eMrpView.Issue_Unit_Iso,uoissue);
				SETCHAR(eMrpViewx.Issue_Unit,"");	/*these value they are sending as blank hence removed*/
				SETCHAR(eMrpViewx.Issue_Unit_Iso,"");
				SETCHAR(eMrpView.Profit_Ctr,profit_centre_sap);		/*profit_centre=0001111010*/
				SETCHAR(eMrpViewx.Profit_Ctr,"X");

				if(tc_strcmp(mat_type,"FERT") != 0)
				{
					//SETCHAR(eMrpView.Rep_Manuf,"");/*X*/	/*for vc it is checked in mrp4*/
					//SETCHAR(eMrpViewx.Rep_Manuf,"");/*X*/
					//SETCHAR(eMrpView.Repmanprof,"");/*rep_mfg*/
					//SETCHAR(eMrpViewx.Repmanprof,"");/*X*/

					if(tc_strlen(rep_mfg)>0)		//rep_mfg modified on 14.02.2017 for TE02
					{
						SETCHAR(eMrpView.Rep_Manuf,"X");/*rep_mfg_in*/
						SETCHAR(eMrpViewx.Rep_Manuf,"X");/*X*/
						SETCHAR(eMrpView.Repmanprof,rep_mfg);/*rep_mfg*/
						SETCHAR(eMrpViewx.Repmanprof,"X");/*X*/
					}
					else
					{
						SETCHAR(eMrpView.Rep_Manuf,"");/*rep_mfg_in*/
						SETCHAR(eMrpViewx.Rep_Manuf,"X");/*X*/
						SETCHAR(eMrpView.Repmanprof,"");/*rep_mfg*/
						SETCHAR(eMrpViewx.Repmanprof,"X");/*X*/
					}

					SETCHAR(eBapiMatHead.Quality_View,"X");	/*quality view*/
					SETCHAR(eMrpView.Ind_Post_To_Insp_Stock,qua_ins_ind);
					SETCHAR(eMrpViewx.Ind_Post_To_Insp_Stock,"X");

					SETCHAR(eMrpView.Doc_Reqd,doc_req);
					SETCHAR(eMrpViewx.Doc_Reqd,"X");
					SETBCD(eMrpView.Gr_Pr_Time,grptime,0);
					SETCHAR(eMrpViewx.Gr_Pr_Time,"X");
					SETCHAR(eBasicView.Catprofile,catalog_prof);
					SETCHAR(eBasicViewx.Catprofile,"X");
					SETCHAR(eBasicView.Qm_Procmnt,qm_proc_ind);
					SETCHAR(eBasicViewx.Qm_Procmnt,"X");

					//control_key = "";
					SETCHAR(eMrpView.Ctrl_Key,control_key);
					SETCHAR(eMrpViewx.Ctrl_Key,"X");

					SETCHAR(eMrpView.Cert_Type,certificate_type);
					SETCHAR(eMrpViewx.Cert_Type,"X");
				}
				else if(tc_strcmp(mat_type,"FERT") == 0)	//Anuja,Swati Tendulkar - QM View for VC
				{
					printf("\nQM View For VC");fflush(stdout);
					printf("\n[%s,%s,%s,%s,%s]",qua_ins_ind,doc_req,grptime,catalog_prof,qm_proc_ind);fflush(stdout);

					SETCHAR(eBapiMatHead.Quality_View,"X");	/*quality view*/
					SETCHAR(eMrpView.Ind_Post_To_Insp_Stock,"");
					SETCHAR(eMrpViewx.Ind_Post_To_Insp_Stock,"");
					SETCHAR(eMrpView.Doc_Reqd,"");
					SETCHAR(eMrpViewx.Doc_Reqd,"");
					SETBCD(eMrpView.Gr_Pr_Time,grptime,0);
					SETCHAR(eMrpViewx.Gr_Pr_Time,"X");
					SETCHAR(eBasicView.Catprofile,catalog_prof);
					SETCHAR(eBasicViewx.Catprofile,"X");
					SETCHAR(eBasicView.Qm_Procmnt,"");
					SETCHAR(eBasicViewx.Qm_Procmnt,"");
					SETCHAR(eMrpView.Ctrl_Key,control_key);
					SETCHAR(eMrpViewx.Ctrl_Key,"X");
					SETCHAR(eMrpView.Cert_Type,certificate_type);
					SETCHAR(eMrpViewx.Cert_Type,"X");
				}
				else    /* as per req. on 1/8 */
				{
					SETCHAR(eBapiMatHead.Quality_View,"");	/*quality view*/
					SETCHAR(eMrpView.Ind_Post_To_Insp_Stock,"");
					SETCHAR(eMrpViewx.Ind_Post_To_Insp_Stock,"");
					SETCHAR(eMrpView.Doc_Reqd,"");
					SETCHAR(eMrpViewx.Doc_Reqd,"");
					SETBCD(eMrpView.Gr_Pr_Time,"",0);
					SETCHAR(eMrpViewx.Gr_Pr_Time,"");
					SETCHAR(eBasicView.Catprofile,"");
					SETCHAR(eBasicViewx.Catprofile,"");
					SETCHAR(eBasicView.Qm_Procmnt,"");
					SETCHAR(eBasicViewx.Qm_Procmnt,"");
					SETCHAR(eMrpView.Ctrl_Key,"");
					SETCHAR(eMrpViewx.Ctrl_Key,"");
					SETCHAR(eMrpView.Cert_Type,"");
					SETCHAR(eMrpViewx.Cert_Type,"");
				}

				SETCHAR(eBapiMatHead.Account_View,"X"); /*accounting view */
				SETCHAR(eAccView.Val_Cat,valuation_cat);
				SETCHAR(eAccViewx.Val_Cat,"X");
				SETCHAR(eAccView.Val_Class,val_class);
				SETCHAR(eAccViewx.Val_Class,"X");
				SETCHAR(eAccView.Price_Ctrl,price_con);
				SETCHAR(eAccViewx.Price_Ctrl,"X");
				SETBCD(eAccView.Std_Price,std_price,4);
				SETCHAR(eAccViewx.Std_Price,"X");
				SETBCD(eAccView.Moving_Pr,moving_avg_price,4);
				SETCHAR(eAccViewx.Moving_Pr,"X");

				SETCHAR(eBapiMatHead.Cost_View,"X");/*lvl cost view */
				SETCHAR(eAccView.Mat_Origin,mat_origin);
				SETCHAR(eAccViewx.Mat_Origin,"X");
				SETCHAR(eAccView.Orig_Group,origin_group);
				SETCHAR(eAccViewx.Orig_Group,"X");
				SETBCD(eMrpView.Lot_Size,cost_lot_size,3);
				SETCHAR(eMrpViewx.Lot_Size,"X");
				SETCHAR(eAccView.Overhead_Grp,overhd_grp);
				SETCHAR(eAccViewx.Overhead_Grp,"X");
				SETCHAR(eMrpView.Variance_Key,variance_key);
				SETCHAR(eMrpViewx.Variance_Key,"X");
				SETCHAR(eAccView.Qty_Struct,with_quantity_structure);
				SETCHAR(eAccViewx.Qty_Struct,"X");
				SETCHAR(eMrpView.Split_Ind,costing_split_valuation);
				SETCHAR(eMrpViewx.Split_Ind,"");	/*Please make it as X*/
				SETCHAR(eRmmg1_Aennr.Aennr,dml_no_arg);

				SETCHAR(eBapiMatHead.Sales_View,"");
				SETCHAR(eBapiMatHead.Purchase_View,"");
				SETCHAR(eBapiMatHead.Forecast_View,"");
				SETCHAR(eBapiMatHead.Work_Sched_View,"");
				SETCHAR(eBapiMatHead.Prt_View,"");
				SETCHAR(eBapiMatHead.Warehouse_View,"");
				SETCHAR(eBapiMatHead.Inp_Fld_Check,"");
				SETCHAR(eBapiMatHead.Material_External,"");
				SETCHAR(eBapiMatHead.MateriaL_Guid,"");
				SETCHAR(eBapiMatHead.Material_Version,"");

				SETCHAR(eBasicView.Del_Flag,"");
				SETCHAR(eBasicView.Po_Unit,"");
				SETCHAR(eBasicView.Po_Unit_Iso,"");
				SETCHAR(eBasicView.Doc_Format,"");
				SETCHAR(eBasicView.Doc_Chg_No,"");
				SETCHAR(eBasicView.Page_No,"");
				SETCHAR(eBasicView.Prod_Memo,"");
				SETCHAR(eBasicView.Pageformat,"");
				SETCHAR(eBasicView.Basic_Matl,"");
				SETCHAR(eBasicView.Std_Descr,"");
				SETCHAR(eBasicView.Dsn_Office,"");
				SETCHAR(eBasicView.Pur_Valkey,"");
				SETCHAR(eBasicView.Container,"");
				SETCHAR(eBasicView.Stor_Conds,"");
				SETCHAR(eBasicView.Temp_Conds,"");
				SETCHAR(eBasicView.Trans_Grp,"");
				SETCHAR(eBasicView.Haz_Mat_No,"");
				SETCHAR(eBasicView.Competitor,"");
				SETBCD(eBasicView.Qty_Gr_Gi,myzeroDup3,3);
				SETCHAR(eBasicView.Proc_Rule,"");
				SETCHAR(eBasicView.Sup_Source,"");
				SETCHAR(eBasicView.Season,"");
				SETCHAR(eBasicView.Label_Type,"");
				SETCHAR(eBasicView.Label_Form,"");
				SETCHAR(eBasicView.Prod_Hier,"");
				SETCHAR(eBasicView.Cad_Id,"X");
				SETBCD(eBasicView.Allowed_Wt,myzeroDup3,3);
				SETCHAR(eBasicView.Pack_Wt_Un,"");
				SETCHAR(eBasicView.Pack_Wt_Un_Iso,"");
				SETBCD(eBasicView.Allwd_Vol,myzeroDup3,3);
				SETCHAR(eBasicView.Pack_Vo_Un,"");
				SETCHAR(eBasicView.Pack_Vo_Un_Iso,"");
				SETBCD(eBasicView.Wt_Tol_Lt,myzeroDup1,1);
				SETBCD(eBasicView.Vol_Tol_Lt,myzeroDup1,1);
				SETCHAR(eBasicView.Var_Ord_Un,"");
				SETCHAR(eBasicView.Batch_Mgmt,"");
				SETCHAR(eBasicView.Sh_Mat_Typ,"");
				SETBCD(eBasicView.Fill_Level,"0",0);
				SETINT2(&eBasicView.Stack_Fact,"0");
				SETCHAR(eBasicView.Mat_Grp_Sm,"");
				SETCHAR(eBasicView.Authoritygroup,"");
				SETBCD(eBasicView.Minremlife,"0",0);
				SETBCD(eBasicView.Shelf_Life,"0",0);
				SETBCD(eBasicView.Stor_Pct,"0",0);
				SETCHAR(eBasicView.Pur_Status,"");
				SETCHAR(eBasicView.Sal_Status,"");
				SETDATE(eBasicView.Pvalidfrom,"");
				SETDATE(eBasicView.Svalidfrom,"");
				SETCHAR(eBasicView.Envt_Rlvt,"");
				SETCHAR(eBasicView.Prod_Alloc,"");
				SETCHAR(eBasicView.Qual_Dik,"");
				SETCHAR(eBasicView.Manu_Mat,"");
				SETCHAR(eBasicView.Mfr_No,"");
				SETCHAR(eBasicView.Inv_Mat_No,"");
				SETCHAR(eBasicView.Manuf_Prof,"");
				SETCHAR(eBasicView.Hazmatprof,"");
				SETCHAR(eBasicView.High_Visc,"");
				SETCHAR(eBasicView.Looseorliq,"");
				SETCHAR(eBasicView.Closed_Box,"");
				SETCHAR(eBasicView.Appd_B_Rec,"");
				SETNUM(eBasicView.Matcmpllvl,"00");
				SETCHAR(eBasicView.Par_Eff,"");
				SETCHAR(eBasicView.Round_Up_Rule_Expiration_Date,"");
				SETCHAR(eBasicView.Period_Ind_Expiration_Date,"D");
				SETCHAR(eBasicView.Prod_Composition_On_Packaging,"");
				SETCHAR(eBasicView.Item_Cat,"");
				SETCHAR(eBasicView.Haz_Mat_No_External,"");
				SETCHAR(eBasicView.Haz_Mat_No_Guid,"");
				SETCHAR(eBasicView.Haz_Mat_No_Version,"");
				SETCHAR(eBasicView.Inv_Mat_No_External,"");
				SETCHAR(eBasicView.Inv_Mat_No_Guid,"");
				SETCHAR(eBasicView.Inv_Mat_No_Version,"");
				SETCHAR(eBasicView.Material_Fixed,"");
				SETCHAR(eBasicView.Cm_Relevance_Flag,"");
				SETCHAR(eBasicView.Sled_Bbd,"");
				SETCHAR(eBasicView.Gtin_Variant,"");
				SETCHAR(eBasicView.Serialization_Level,"");
				SETCHAR(eBasicView.Pl_Ref_Mat,"");
				SETCHAR(eBasicView.Extmatlgrp,"");
				SETCHAR(eBasicView.Uomusage,"");
				SETCHAR(eBasicView.Pl_Ref_Mat_External,"");
				SETCHAR(eBasicView.Pl_Ref_Mat_Guid,"");
				SETCHAR(eBasicView.Pl_Ref_Mat_Version,"");

				SETCHAR(eBasicViewx.Del_Flag,"");
				SETCHAR(eBasicViewx.Po_Unit,"");
				SETCHAR(eBasicViewx.Po_Unit_Iso,"");
				SETCHAR(eBasicViewx.Doc_Format,"");
				SETCHAR(eBasicViewx.Doc_Chg_No,"");
				SETCHAR(eBasicViewx.Page_No,"");
				SETCHAR(eBasicViewx.Prod_Memo,"");
				SETCHAR(eBasicViewx.Pageformat,"");
				SETCHAR(eBasicViewx.Basic_Matl,"");
				SETCHAR(eBasicViewx.Std_Descr,"");
				SETCHAR(eBasicViewx.Dsn_Office,"");
				SETCHAR(eBasicViewx.Pur_Valkey,"");
				SETCHAR(eBasicViewx.Container,"");
				SETCHAR(eBasicViewx.Stor_Conds,"");
				SETCHAR(eBasicViewx.Temp_Conds,"");
				SETCHAR(eBasicViewx.Trans_Grp,"");
				SETCHAR(eBasicViewx.Haz_Mat_No,"");
				SETCHAR(eBasicViewx.Competitor,"");
				SETCHAR(eBasicViewx.Qty_Gr_Gi,"");
				SETCHAR(eBasicViewx.Proc_Rule,"");
				SETCHAR(eBasicViewx.Sup_Source,"");
				SETCHAR(eBasicViewx.Season,"");
				SETCHAR(eBasicViewx.Label_Type,"");
				SETCHAR(eBasicViewx.Label_Form,"");
				SETCHAR(eBasicViewx.Prod_Hier,"");
				SETCHAR(eBasicViewx.Cad_Id,"X");
				SETCHAR(eBasicViewx.Allowed_Wt,"");
				SETCHAR(eBasicViewx.Pack_Wt_Un,"");
				SETCHAR(eBasicViewx.Pack_Wt_Un_Iso,"");
				SETCHAR(eBasicViewx.Allwd_Vol,"");
				SETCHAR(eBasicViewx.Pack_Vo_Un,"");
				SETCHAR(eBasicViewx.Pack_Vo_Un_Iso,"");
				SETCHAR(eBasicViewx.Wt_Tol_Lt,"");
				SETCHAR(eBasicViewx.Vol_Tol_Lt,"");
				SETCHAR(eBasicViewx.Var_Ord_Un,"");
				SETCHAR(eBasicViewx.Batch_Mgmt,"");
				SETCHAR(eBasicViewx.Sh_Mat_Typ,"");
				SETCHAR(eBasicViewx.Fill_Level,"");
				SETCHAR(eBasicViewx.Stack_Fact,"");
				SETCHAR(eBasicViewx.Mat_Grp_Sm,"");
				SETCHAR(eBasicViewx.Authoritygroup,"");
				SETCHAR(eBasicViewx.Minremlife,"");
				SETCHAR(eBasicViewx.Shelf_Life,"");
				SETCHAR(eBasicViewx.Stor_Pct,"");
				SETCHAR(eBasicViewx.Pur_Status,"");
				SETCHAR(eBasicViewx.Sal_Status,"");
				SETCHAR(eBasicViewx.Pvalidfrom,"");
				SETCHAR(eBasicViewx.Svalidfrom,"");
				SETCHAR(eBasicViewx.Envt_Rlvt,"");
				SETCHAR(eBasicViewx.Prod_Alloc,"");
				SETCHAR(eBasicViewx.Qual_Dik,"");
				SETCHAR(eBasicViewx.Manu_Mat,"");
				SETCHAR(eBasicViewx.Mfr_No,"");
				SETCHAR(eBasicViewx.Inv_Mat_No,"");
				SETCHAR(eBasicViewx.Manuf_Prof,"");
				SETCHAR(eBasicViewx.Hazmatprof,"");
				SETCHAR(eBasicViewx.High_Visc,"");
				SETCHAR(eBasicViewx.Looseorliq,"");
				SETCHAR(eBasicViewx.Closed_Box,"");
				SETCHAR(eBasicViewx.Appd_B_Rec,"");
				SETCHAR(eBasicViewx.Matcmpllvl,"");
				SETCHAR(eBasicViewx.Par_Eff,"");
				SETCHAR(eBasicViewx.Round_Up_Rule_Expiration_Date,"");
				SETCHAR(eBasicViewx.Period_Ind_Expiration_Date,"");
				SETCHAR(eBasicViewx.Prod_Composition_On_Packaging,"");
				SETCHAR(eBasicViewx.Item_Cat,"");
				SETCHAR(eBasicViewx.Haz_Mat_No_External,"");
				SETCHAR(eBasicViewx.Haz_Mat_No_Guid,"");
				SETCHAR(eBasicViewx.Haz_Mat_No_Version,"");
				SETCHAR(eBasicViewx.Inv_Mat_No_External,"");
				SETCHAR(eBasicViewx.Inv_Mat_No_Guid,"");
				SETCHAR(eBasicViewx.Inv_Mat_No_Version,"");
				SETCHAR(eBasicViewx.Material_Fixed,"");
				SETCHAR(eBasicViewx.Cm_Relevance_Flag,"");
				SETCHAR(eBasicViewx.Sled_Bbd,"");
				SETCHAR(eBasicViewx.Gtin_Variant,"");
				SETCHAR(eBasicViewx.Serialization_Level,"");
				SETCHAR(eBasicViewx.Pl_Ref_Mat,"");
				SETCHAR(eBasicViewx.Extmatlgrp,"");
				SETCHAR(eBasicViewx.Uomusage,"");
				SETCHAR(eBasicViewx.Pl_Ref_Mat_External,"");
				SETCHAR(eBasicViewx.Pl_Ref_Mat_Guid,"");
				SETCHAR(eBasicViewx.Pl_Ref_Mat_Version,"");

				SETCHAR(eMrpView.Plant,plantcode);
				SETCHAR(eMrpView.Del_Flag,"");
				SETCHAR(eMrpView.Crit_Part,"");
				SETCHAR(eMrpView.Pur_Group,"");
				SETCHAR(eMrpView.Mrpprofile,"");
				SETCHAR(eMrpView.Plnd_Delry,"0");
				SETBCD(eMrpView.Assy_Scrap,myzeroDup2,2);
				SETBCD(eMrpView.Safety_Stk,myzeroDup3,3);
				SETBCD(eMrpView.Minlotsize,myzeroDup3,3);
				SETBCD(eMrpView.Maxlotsize,myzeroDup3,3);
				SETBCD(eMrpView.Round_Val,myzeroDup3,3);
				SETBCD(eMrpView.Ord_Costs,myzeroDup4,4);
				SETCHAR(eMrpView.Stor_Costs,"");

				SETCHAR(eMrpView.Discontinu,"");
				SETCHAR(eMrpView.Eff_O_Day,"");
				SETCHAR(eMrpView.Follow_Up,"");
				SETCHAR(eMrpView.Backflush,"");
				SETCHAR(eMrpView.Production_Scheduler,"");
				SETBCD(eMrpView.Proc_Time,myzeroDup2,2);
				SETBCD(eMrpView.Setuptime,myzeroDup2,2);
				SETBCD(eMrpView.Interop,myzeroDup2,2);
				SETBCD(eMrpView.Base_Qty,myzeroDup3,3);
				SETBCD(eMrpView.Inhseprodt,"0",0);
				SETBCD(eMrpView.Stgeperiod,"0",0);
				SETCHAR(eMrpView.Stge_Pd_Un,"");
				SETCHAR(eMrpView.Stge_Pd_Un_Iso,"");
				SETBCD(eMrpView.Over_Tol,myzeroDup1,1);
				SETCHAR(eMrpView.Unlimited,"");
				SETBCD(eMrpView.Under_Tol,myzeroDup1,1);
				SETBCD(eMrpView.Replentime,"0",0);
				SETCHAR(eMrpView.Replace_Pt,"");
				SETCHAR(eMrpView.Loadinggrp,"");
				SETCHAR(eMrpView.Batch_Mgmt,"");
				SETBCD(eMrpView.Serv_Level,myzeroDup1,1);
				SETCHAR(eMrpView.Fy_Variant,"");
				SETCHAR(eMrpView.Corr_Fact,"");
				SETBCD(eMrpView.Setup_Time,myzeroDup2,2);
				SETBCD(eMrpView.Base_Qty_Plan,myzeroDup3,3);
				SETBCD(eMrpView.Ship_Proc_Time,myzeroDup2,2);
				SETCHAR(eMrpView.Sup_Source,"");
				SETCHAR(eMrpView.Auto_P_Ord,"");
				SETCHAR(eMrpView.Sourcelist,"");
				SETCHAR(eMrpView.Comm_Code,"");
				SETCHAR(eMrpView.Countryori,"");
				SETCHAR(eMrpView.Countryori_Iso,"");
				SETCHAR(eMrpView.Regionorig,"");
				SETCHAR(eMrpView.Comm_Co_Un,"");
				SETCHAR(eMrpView.Comm_Co_Un_Iso,"");
				SETCHAR(eMrpView.Expimpgrp,"");
				SETNUM(eMrpView.Pl_Ti_Fnce,"000");
				SETCHAR(eMrpView.Consummode,"");
				SETNUM(eMrpView.Bwd_Cons,"000");
				SETNUM(eMrpView.Fwd_Cons,"000");
				SETCHAR(eMrpView.Bom_Usage,"");
				SETCHAR(eMrpView.Planlistgrp,"");
				SETCHAR(eMrpView.Planlistcnt,"");
				SETCHAR(eMrpView.Specprocty,"");
				SETCHAR(eMrpView.Prod_Unit,"");
				SETCHAR(eMrpView.Prod_Unit_Iso,"");
				//SETCHAR(eMrpView.Iss_St_Loc,store_locDup);
				//SETCHAR(eMrpView.Iss_St_Loc,"");
				SETBCD(eMrpView.Comp_Scrap,myzeroDup2,2);
				SETBCD(eMrpView.Cycle_Time,"0",0);
				SETCHAR(eMrpView.Covprofile,"");
				SETCHAR(eMrpView.Cc_Ph_Inv,"");
				SETCHAR(eMrpView.Serno_Prof,"");
				SETCHAR(eMrpView.Neg_Stocks,"");
				SETCHAR(eMrpView.Qm_Rgmts,"");
				SETCHAR(eMrpView.Plng_Cycle,"");
				SETCHAR(eMrpView.Round_Prof,"");
				SETCHAR(eMrpView.Refmatcons,"");
				SETCHAR(eMrpView.D_To_Ref_M,"");
				SETBCD(eMrpView.Mult_Ref_M,myzeroDup2,2);
				SETCHAR(eMrpView.Auto_Reset,"");
				SETCHAR(eMrpView.Ex_Cert_Id,"");
				SETCHAR(eMrpView.Ex_Cert_No_New,"");
				SETCHAR(eMrpView.Ex_Cert_Dt,"");
				SETCHAR(eMrpView.Milit_Id,"");
				SETBCD(eMrpView.Insp_Int,"0",0);
				SETCHAR(eMrpView.Co_Product,"");
				SETCHAR(eMrpView.Plan_Strgp,"");
				//SETCHAR(eMrpView.Sloc_Exprc,store_locDup);
				//SETCHAR(eMrpView.Sloc_Exprc,"");
				SETCHAR(eMrpView.Cc_Fixed,"");
				SETCHAR(eMrpView.Qm_Authgrp,"");
				SETCHAR(eMrpView.Task_List_Type,"");
				SETCHAR(eMrpView.Prodprof,"");
				SETCHAR(eMrpView.Safty_T_Id,"");
				SETNUM(eMrpView.Safetytime,"00");
				SETCHAR(eMrpView.Plord_Ctrl,"");
				SETCHAR(eMrpView.Batchentry,"");
				SETDATE(eMrpView.Pvalidfrom,"");
				SETCHAR(eMrpView.Matfrgtgrp,"");
				SETCHAR(eMrpView.Prodverscs,"");
				SETCHAR(eMrpView.Mat_Cfop,"");
				SETCHAR(eMrpView.Eu_List_No,"");
				SETCHAR(eMrpView.Eu_Mat_Grp,"");
				SETCHAR(eMrpView.Cas_No,"");
				SETCHAR(eMrpView.Prodcom_No,"");
				SETCHAR(eMrpView.Ctrl_Code,"");
				SETCHAR(eMrpView.Jit_Relvt,"");
				SETCHAR(eMrpView.Mat_Grp_Trans,"");
				SETCHAR(eMrpView.Handlg_Grp,"");
				SETCHAR(eMrpView.Supply_Area,"");
				SETCHAR(eMrpView.Fair_Share_Rule,"");
				SETCHAR(eMrpView.Push_Distrib,"");
				SETBCD(eMrpView.Deploy_Horiz,"0",0);
				SETBCD(eMrpView.Min_Lot_Size,myzeroDup3,3);
				SETBCD(eMrpView.Max_Lot_Size,myzeroDup3,3);
				SETBCD(eMrpView.Fix_Lot_Size,myzeroDup3,3);
				SETBCD(eMrpView.Lot_Increment,myzeroDup3,3);
				SETCHAR(eMrpView.Prod_Conv_Type,"");
				SETCHAR(eMrpView.Distr_Prof,"");
				SETCHAR(eMrpView.Period_Profile_Safety_Time,"");
				SETCHAR(eMrpView.Fxd_Price,"");
				SETCHAR(eMrpView.Avail_Check_All_Proj_Segments,"");
				SETCHAR(eMrpView.Overallprf,"");
				SETCHAR(eMrpView.Mrp_Relevancy_Dep_Requirements,"");
				SETBCD(eMrpView.Min_Safety_Stk,myzeroDup2,2);
				SETCHAR(eMrpView.No_Costing,"");
				SETCHAR(eMrpView.Unit_Group,"");
				SETCHAR(eMrpView.Follow_Up_External,"");
				SETCHAR(eMrpView.Follow_Up_Guid,"");
				SETCHAR(eMrpView.Follow_Up_Version,"");
				SETCHAR(eMrpView.Refmatcons_External,"");
				SETCHAR(eMrpView.Refmatcons_Guid,"");
				SETCHAR(eMrpView.Refmatcons_Version,"");
				SETCHAR(eMrpView.Rotation_Date,"");
				SETCHAR(eMrpView.Original_Batch_Flag,"");
				SETCHAR(eMrpView.Original_Batch_Ref_Material,"");
				SETCHAR(eMrpView.Original_Batch_Ref_Material_E,"");
				SETCHAR(eMrpView.Original_Batch_Ref_Material_V,"");
				SETCHAR(eMrpView.Original_Batch_Ref_Material_G,"");

				SETCHAR(eMrpViewx.Plant,plantcode);
				SETCHAR(eMrpViewx.Del_Flag,"");
				SETCHAR(eMrpViewx.Crit_Part,"");
				SETCHAR(eMrpViewx.Pur_Group,"");
				SETCHAR(eMrpViewx.Mrpprofile,"");
				SETCHAR(eMrpViewx.Plnd_Delry,"");
				SETCHAR(eMrpViewx.Assy_Scrap,"");
				SETCHAR(eMrpViewx.Safety_Stk,"");
				SETCHAR(eMrpViewx.Minlotsize,"");
				SETCHAR(eMrpViewx.Maxlotsize,"");
				SETCHAR(eMrpViewx.Round_Val,"");
				SETCHAR(eMrpViewx.Ord_Costs,"");
				SETCHAR(eMrpViewx.Stor_Costs,"");

				SETCHAR(eMrpViewx.Discontinu,"");
				SETCHAR(eMrpViewx.Eff_O_Day,"");
				SETCHAR(eMrpViewx.Follow_Up,"");
				SETCHAR(eMrpViewx.Backflush,"");
				SETCHAR(eMrpViewx.Production_Scheduler,"");
				SETCHAR(eMrpViewx.Proc_Time,"");
				SETCHAR(eMrpViewx.Setuptime,"");
				SETCHAR(eMrpViewx.Interop,"");
				SETCHAR(eMrpViewx.Base_Qty,"");
				SETCHAR(eMrpViewx.Inhseprodt,"");
				SETCHAR(eMrpViewx.Stgeperiod,"");
				SETCHAR(eMrpViewx.Stge_Pd_Un,"");
				SETCHAR(eMrpViewx.Stge_Pd_Un_Iso,"");
				SETCHAR(eMrpViewx.Over_Tol,"");
				SETCHAR(eMrpViewx.Unlimited,"");
				SETCHAR(eMrpViewx.Under_Tol,"");
				SETCHAR(eMrpViewx.Replentime,"");
				SETCHAR(eMrpViewx.Replace_Pt,"");
				SETCHAR(eMrpViewx.Loadinggrp,"");
				SETCHAR(eMrpViewx.Batch_Mgmt,"");
				SETCHAR(eMrpViewx.Serv_Level,"");
				SETCHAR(eMrpViewx.Fy_Variant,"");
				SETCHAR(eMrpViewx.Corr_Fact,"");
				SETCHAR(eMrpViewx.Setup_Time,"");
				SETCHAR(eMrpViewx.Base_Qty_Plan,"");
				SETCHAR(eMrpViewx.Ship_Proc_Time,"");
				SETCHAR(eMrpViewx.Sup_Source,"");
				SETCHAR(eMrpViewx.Auto_P_Ord,"");
				SETCHAR(eMrpViewx.Sourcelist,"");
				SETCHAR(eMrpViewx.Comm_Code,"");
				SETCHAR(eMrpViewx.Countryori,"");
				SETCHAR(eMrpViewx.Countryori_Iso,"");
				SETCHAR(eMrpViewx.Regionorig,"");
				SETCHAR(eMrpViewx.Comm_Co_Un,"");
				SETCHAR(eMrpViewx.Comm_Co_Un_Iso,"");
				SETCHAR(eMrpViewx.Expimpgrp,"");
				SETCHAR(eMrpViewx.Pl_Ti_Fncve,"");
				SETCHAR(eMrpViewx.Consummode,"");
				SETCHAR(eMrpViewx.Bwd_Cons,"");
				SETCHAR(eMrpViewx.Fwd_Cons,"");
				SETCHAR(eMrpViewx.Bom_Usage,"");
				SETCHAR(eMrpViewx.Planlistgrp,"");
				SETCHAR(eMrpViewx.Planlistcnt,"");
				SETCHAR(eMrpViewx.Specprocty,"");
				SETCHAR(eMrpViewx.Prod_Unit,"");
				SETCHAR(eMrpViewx.Prod_Unit_Iso,"");
				//SETCHAR(eMrpViewx.Iss_St_Loc,"X");
				//SETCHAR(eMrpViewx.Iss_St_Loc,"");
				SETCHAR(eMrpViewx.Comp_Scrap,"");
				SETCHAR(eMrpViewx.Cycle_Time,"");
				SETCHAR(eMrpViewx.Covprofile,"");
				SETCHAR(eMrpViewx.Cc_Ph_Inv,"");
				SETCHAR(eMrpViewx.Serno_Prof,"");
				SETCHAR(eMrpViewx.Neg_Stocks,"");
				SETCHAR(eMrpViewx.Qm_Rgmts,"");
				SETCHAR(eMrpViewx.Plng_Cycle,"");
				SETCHAR(eMrpViewx.Round_Prof,"");
				SETCHAR(eMrpViewx.Refmatcons,"");
				SETCHAR(eMrpViewx.D_To_Ref_M,"");
				SETCHAR(eMrpViewx.Mult_Ref_M,"");
				SETCHAR(eMrpViewx.Auto_Reset,"");
				SETCHAR(eMrpViewx.Ex_Cert_Id,"");
				SETCHAR(eMrpViewx.Ex_Cert_No_New,"");
				SETCHAR(eMrpViewx.Ex_Cert_Dt,"");
				SETCHAR(eMrpViewx.Milit_Id,"");
				SETCHAR(eMrpViewx.Insp_Int,"");
				SETCHAR(eMrpViewx.Co_Product,"");
				SETCHAR(eMrpViewx.Plan_Strgp,"");
				//SETCHAR(eMrpViewx.Sloc_Exprc,"X");
				//SETCHAR(eMrpViewx.Sloc_Exprc,"");
				SETCHAR(eMrpViewx.Cc_Fixed,"");
				SETCHAR(eMrpViewx.Qm_Authgrp,"");
				SETCHAR(eMrpViewx.Task_List_Type,"");
				SETCHAR(eMrpViewx.Prodprof,"");
				SETCHAR(eMrpViewx.Safty_T_Id,"");
				SETCHAR(eMrpViewx.Safetytime,"");
				SETCHAR(eMrpViewx.Plord_Ctrl,"");
				SETCHAR(eMrpViewx.Batchentry,"");
				SETCHAR(eMrpViewx.Pvalidfrom,"");
				SETCHAR(eMrpViewx.Matfrgtgrp,"");
				SETCHAR(eMrpViewx.Prodverscs,"");
				SETCHAR(eMrpViewx.Mat_Cfop,"");
				SETCHAR(eMrpViewx.Eu_List_No,"");
				SETCHAR(eMrpViewx.Eu_Mat_Grp,"");
				SETCHAR(eMrpViewx.Cas_No,"");
				SETCHAR(eMrpViewx.Prodcom_No,"");
				SETCHAR(eMrpViewx.Ctrl_Code,"");
				SETCHAR(eMrpViewx.Jit_Relvt,"");
				SETCHAR(eMrpViewx.Mat_Grp_Trans,"");
				SETCHAR(eMrpViewx.Handlg_Grp,"");
				SETCHAR(eMrpViewx.Supply_Area,"");
				SETCHAR(eMrpViewx.Fair_Share_Rule,"");
				SETCHAR(eMrpViewx.Push_Distrib,"");
				SETCHAR(eMrpViewx.Deploy_Horiz,"");
				SETCHAR(eMrpViewx.Min_Lot_Size,"");
				SETCHAR(eMrpViewx.Max_Lot_Size,"");
				SETCHAR(eMrpViewx.Fix_Lot_Size,"");
				SETCHAR(eMrpViewx.Lot_Increment,"");
				SETCHAR(eMrpViewx.Prod_Conv_Type,"");
				SETCHAR(eMrpViewx.Distr_Prof,"");
				SETCHAR(eMrpViewx.Period_Profile_Safety_Time,"");
				SETCHAR(eMrpViewx.Fxd_Price,"");
				SETCHAR(eMrpViewx.Avail_Check_All_Proj_Segments,"");
				SETCHAR(eMrpViewx.Overallprf,"");
				SETCHAR(eMrpViewx.Mrp_Relevancy_Dep_Requirements,"");
				SETCHAR(eMrpViewx.Min_Safety_Stk,"");
				SETCHAR(eMrpViewx.No_Costing,"");
				SETCHAR(eMrpViewx.Unit_Group,"");
				SETCHAR(eMrpViewx.Follow_Up_External,"");
				SETCHAR(eMrpViewx.Follow_Up_Guid,"");
				SETCHAR(eMrpViewx.Follow_Up_Version,"");
				SETCHAR(eMrpViewx.Refmatcons_External,"");
				SETCHAR(eMrpViewx.Refmatcons_Guid,"");
				SETCHAR(eMrpViewx.Refmatcons_Version,"");
				SETCHAR(eMrpViewx.Rotation_Date,"");
				SETCHAR(eMrpViewx.Original_Batch_Flag,"");
				SETCHAR(eMrpViewx.Original_Batch_Ref_Material,"");
				SETCHAR(eMrpViewx.Original_Batch_Ref_Material_E,"");
				SETCHAR(eMrpViewx.Original_Batch_Ref_Material_V,"");
				SETCHAR(eMrpViewx.Original_Batch_Ref_Material_G,"");

				SETCHAR(eBapi_Mpgd.Plant,plantcode);
				SETCHAR(eBapi_Mpgd.Plng_Matl_External,"");
				SETCHAR(eBapi_Mpgd.Plng_Matl_Guid,"");
				SETCHAR(eBapi_Mpgd.Plng_Matl_Version,"");
				SETCHAR(eBapi_Mpgdx.Plant,plantcode);
				SETCHAR(eBapi_Mpgdx.Plng_Matl_External,"");
				SETCHAR(eBapi_Mpgdx.Plng_Matl_Guid,"");
				SETCHAR(eBapi_Mpgdx.Plng_Matl_Version,"");

				SETCHAR(eBapi_Mard.Plant,plantcode);
				SETCHAR(eBapi_Mard.Stge_Loc,"9005");
				SETCHAR(eBapi_Mard.Del_Flag,"");
				SETCHAR(eBapi_Mard.Mrp_Ind,"");
				SETCHAR(eBapi_Mard.Spec_Proc,"");
				SETBCD(eBapi_Mard.Reorder_Pt,myzeroDup3,3);
				SETBCD(eBapi_Mard.Repl_Qty,myzeroDup3,3);
				SETCHAR(eBapi_Mard.Stge_Bin,"");
				SETCHAR(eBapi_Mard.Pickg_Area,"");
				SETFLOAT(&eBapi_Mard.Inv_Corr_Fac,"");

				SETCHAR(eBapi_Mardx.Plant,plantcode);
				SETCHAR(eBapi_Mardx.Stge_Loc,"9005");
				SETCHAR(eBapi_Mardx.Del_Flag,"");
				SETCHAR(eBapi_Mardx.Mrp_Ind,"");
				SETCHAR(eBapi_Mardx.Spec_Proc,"");
				SETCHAR(eBapi_Mardx.Reorder_Pt,"");
				SETCHAR(eBapi_Mardx.Repl_Qty,"");
				SETCHAR(eBapi_Mardx.Stge_Bin,"");
				SETCHAR(eBapi_Mardx.Pickg_Area,"");
				SETCHAR(eBapi_Mardx.Inv_Corr_Fac,"");

				SETCHAR(eAccView.Val_Area,plantcode);
				SETCHAR(eAccView.Val_Type,"");
				SETCHAR(eAccView.Del_Flag,"");
				SETBCD(eAccView.Price_Unit,"",0);
				SETCHAR(eAccView.Pr_Ctrl_Pp,"");
				SETBCD(eAccView.Mov_Pr_Pp,myzeroDup4,4);
				SETBCD(eAccView.Std_Pr_Pp,myzeroDup4,4);
				SETBCD(eAccView.Pr_Unit_Pp,"0",1);
				SETCHAR(eAccView.Vclass_Pp,"");
				SETCHAR(eAccView.Pr_Ctrl_Py,"");
				SETBCD(eAccView.Mov_Pr_Py,myzeroDup4,4);
				SETBCD(eAccView.Std_Pr_Py,myzeroDup4,4);
				SETCHAR(eAccView.Vclass_Py,"");
				SETBCD(eAccView.Pr_Unit_Py,"0",0);
				SETBCD(eAccView.Future_Pr,myzeroDup4,4);
				SETDATE(eAccView.Valid_From,"");
				SETBCD(eAccView.Taxprice_1,myzeroDup4,4);
				SETBCD(eAccView.Commprice1,myzeroDup4,4);
				SETBCD(eAccView.Taxprice_3,myzeroDup4,4);
				SETBCD(eAccView.Commprice3,myzeroDup4,4);
				SETBCD(eAccView.Plnd_Price,myzeroDup4,4);
				SETBCD(eAccView.Plndprice1,myzeroDup4,4);
				SETBCD(eAccView.Plndprice2,myzeroDup4,4);
				SETBCD(eAccView.Plndprice3,myzeroDup4,4);
				SETDATE(eAccView.Plndprdate1,"");
				SETDATE(eAccView.Plndprdate2,"");
				SETDATE(eAccView.Plndprdate3,"");
				SETCHAR(eAccView.Lifo_Fifo,"");
				SETCHAR(eAccView.Poolnumber,"");
				SETBCD(eAccView.Taxprice_2,myzeroDup4,4);
				SETBCD(eAccView.Commprice2,myzeroDup4,4);
				SETNUM(eAccView.Deval_Ind,"00");
				SETCHAR(eAccView.Ml_Active,"");
				SETCHAR(eAccView.Ml_Settle,"");
				SETCHAR(eAccView.Orig_Mat,"X");
				SETCHAR(eAccView.Vm_So_Stk,"");
				SETCHAR(eAccView.Vm_P_Stock,"");
				SETCHAR(eAccView.Matl_Usage,"");
				SETCHAR(eAccView.In_House,"");
				SETBCD(eAccView.Tax_Cml_Un,"0",0);

				SETCHAR(eAccViewx.Val_Area,plantcode);
				SETCHAR(eAccViewx.Val_Type,"");
				SETCHAR(eAccViewx.Del_Flag,"");
				SETCHAR(eAccViewx.Price_Unit,"");
				SETCHAR(eAccViewx.Pr_Ctrl_Pp,"");
				SETCHAR(eAccViewx.Mov_Pr_Pp,"");
				SETCHAR(eAccViewx.Std_Pr_Pp,"");
				SETCHAR(eAccViewx.Pr_Unit_Pp,"");
				SETCHAR(eAccViewx.Vclass_Pp,"");
				SETCHAR(eAccViewx.Pr_Ctrl_Py,"");
				SETCHAR(eAccViewx.Mov_Pr_Py,"");
				SETCHAR(eAccViewx.Std_Pr_Py,"");
				SETCHAR(eAccViewx.Vclass_Py,"");
				SETCHAR(eAccViewx.Pr_Unit_Py,"");
				SETCHAR(eAccViewx.Future_Pr,"");
				SETCHAR(eAccViewx.Valid_From,"");
				SETCHAR(eAccViewx.Taxprice_1,"");
				SETCHAR(eAccViewx.Commprice1,"");
				SETCHAR(eAccViewx.Taxprice_3,"");
				SETCHAR(eAccViewx.Commprice3,"");
				SETCHAR(eAccViewx.Plnd_Price,"");
				SETCHAR(eAccViewx.Plndprice1,"");
				SETCHAR(eAccViewx.Plndprice2,"");
				SETCHAR(eAccViewx.Plndprice3,"");
				SETCHAR(eAccViewx.Plndprdate1,"");
				SETCHAR(eAccViewx.Plndprdate2,"");
				SETCHAR(eAccViewx.Plndprdate3,"");
				SETCHAR(eAccViewx.Lifo_Fifo,"");
				SETCHAR(eAccViewx.Poolnumber,"");
				SETCHAR(eAccViewx.Taxprice_2,"");
				SETCHAR(eAccViewx.Commprice2,"");
				SETCHAR(eAccViewx.Deval_Ind,"");
				SETCHAR(eAccViewx.Ml_Active,"");
				SETCHAR(eAccViewx.Ml_Settle,"");
				SETCHAR(eAccViewx.Orig_Mat,"X");
				SETCHAR(eAccViewx.Vm_So_Stk,"");
				SETCHAR(eAccViewx.Vm_P_Stock,"");
				SETCHAR(eAccViewx.Matl_Usage,"");
				SETCHAR(eAccViewx.In_House,"");
				SETCHAR(eAccViewx.Tax_Cml_Un,"");

				SETCHAR(tBapi_Makt->Langu,"EN");
				SETCHAR(tBapi_Makt->Langu_Iso,"EN");
				SETCHAR(tBapi_Makt->Del_Flag,"");

				SETBCD(tBapi_Marm->Numerator,"1",0);
				SETBCD(tBapi_Marm->Denominatr,"1",0);
				SETCHAR(tBapi_Marm->Ean_Upc,"");
				SETCHAR(tBapi_Marm->Ean_Cat,"");
				SETCHAR(tBapi_Marm->Length,"");
				SETCHAR(tBapi_Marm->Width,"");
				SETCHAR(tBapi_Marm->Height,"");
				SETCHAR(tBapi_Marm->Unit_Dim,"");
				SETCHAR(tBapi_Marm->Unit_Dim_Iso,"");
				SETCHAR(tBapi_Marm->Del_Flag,"");
				SETCHAR(tBapi_Marm->Sub_Uom,"");
				SETCHAR(tBapi_Marm->Sub_Uom_Iso,"");
				SETCHAR(tBapi_Marm->Gtin_Variant,"");

				SETCHAR(tBapi_Marmx->Numerator,"X");
				SETCHAR(tBapi_Marmx->Denominatr,"X");
				SETCHAR(tBapi_Marmx->Ean_Upc,"");
				SETCHAR(tBapi_Marmx->Ean_Cat,"");
				SETCHAR(tBapi_Marmx->Length,"");
				SETCHAR(tBapi_Marmx->Width,"");
				SETCHAR(tBapi_Marmx->Height,"");
				SETCHAR(tBapi_Marmx->Unit_Dim,"");
				SETCHAR(tBapi_Marmx->Unit_Dim_Iso,"");
				SETCHAR(tBapi_Marmx->Sub_Uom,"");
				SETCHAR(tBapi_Marmx->Sub_Uom_Iso,"");
				SETCHAR(tBapi_Marmx->Gtin_Variant,"");

				SETCHAR(tBAPIE1PAREX3->STRUCTURE,"BAPI_TE_MARA");
				SETCHAR(tBAPIE1PAREX3->VALUEPART1,part_noDupDes);
				SETCHAR(tBAPIE1PAREX3->VALUEPART2,"");
				SETCHAR(tBAPIE1PAREX3->VALUEPART3,"");
				SETCHAR(tBAPIE1PAREX3->VALUEPART4,"");

				SETCHAR(tBAPIE1PAREXX3->STRUCTURE,"BAPI_TE_MARAX");
				SETCHAR(tBAPIE1PAREXX3->VALUEPART1,part_noDupDesx);
				SETCHAR(tBAPIE1PAREXX3->VALUEPART2,"");
				SETCHAR(tBAPIE1PAREXX3->VALUEPART3,"");
				SETCHAR(tBAPIE1PAREXX3->VALUEPART4,"");

				RfcRc = zbapi_material_savedata_mrpCreate(hRfc,&eBapiMatHead,&eBasicView,&eBasicViewx,&eMrpView,&eMrpViewx,&eBapi_Mpgd,&eBapi_Mpgdx,&eBapi_Mard,&eBapi_Mardx,&eAccView,&eAccViewx,&eRmmg1_Aennr,&eBapiret2,thBapi_Makt,thBapi_Marm,thBapi_Marmx,thBAPIE1PAREX3,thBAPIE1PAREXX3,xException);

				switch (RfcRc)
				{
					case RFC_OK:
						//sprintf(sap_msg,"%.*s:",sizeof(sap_msg),eBapiret2.Message);
						//GETCHAR(eBapiret2.Message,sap_msg);

						printf("\nMessage for Material %s Create with basic view: %s",part_noDup,eBapiret2.Message); fflush(stdout);
						fprintf(fsuccess,"\nMessage for Material %s Create with basic view: %s",part_noDup,eBapiret2.Message); fflush(fsuccess);
						break;
					case RFC_EXCEPTION:
						printf("\nRFC EXCEPTION: %s",xException); fflush(stdout);
						break;
					case RFC_SYS_EXCEPTION:
						printf("\nSystem Exception Raised!!!"); fflush(stdout);
						break;
					case RFC_FAILURE:
						printf("\nFailure!!!"); fflush(stdout);
						break;
					default:
						printf("\nOther Failure!"); fflush(stdout);
					break;
				}
				if(ItDelete(thBapi_Makt) != 0)	rfc_error("ItDelete thBapi_Makt");
				if(ItDelete(thBapi_Marm) != 0)	rfc_error("ItDelete tBapi_Marm");
				if(ItDelete(thBapi_Marmx) != 0) rfc_error("ItDelete thBapi_Marmx");
				if(ItDelete(thBAPIE1PAREX3) != 0) rfc_error("ItDelete thBAPIE1PAREX3");
				if(ItDelete(thBAPIE1PAREXX3) != 0) rfc_error("ItDelete thBAPIE1PAREXX3");
				/*sleep(5);*/
				RfcClose(hRfc);
			}
		}
	}
	if (flag == 1)
	{
		printf("\nGoing for MATERIAL CHANGE..."); fflush(stdout);
		printf("\nMaterial change string : %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",part_noDup,plantcode,meas_unit,dml_numAP1,descDup,doc_noDup,dwg_typeDup,dwg_revDup,mrp_type,lot_size_key,proc_type,spl_proc_key,mrp_grp,reord_pt,max_stlvl,val_class,overhd_grp,origin_group,dml_no_arg); fflush(stdout);
		fprintf(fsuccess,"\nMaterial change string : %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",part_noDup,plantcode,meas_unit,dml_numAP1,descDup,doc_noDup,dwg_typeDup,dwg_revDup,mrp_type,lot_size_key,proc_type,spl_proc_key,mrp_grp,reord_pt,max_stlvl,val_class,overhd_grp,origin_group,dml_no_arg); fflush(fsuccess);
		fprintf(fsuccess,"\npart_noDupDes:%s",part_noDupDes);
		fprintf(fsuccess,"\npart_noDupDes:%s",part_noDupDesx); fflush(fsuccess);

		hRfc = BapiLogon();
		thBapi_Makt = ITAB_NULL;
		thBapi_Marm = ITAB_NULL;
		thBapi_Marmx = ITAB_NULL;
		thBAPIE1PAREX3 = ITAB_NULL;
		thBAPIE1PAREXX3 = ITAB_NULL;

		if (thBapi_Makt==ITAB_NULL)
		{
			thBapi_Makt = ItCreate("MATERIALDESCRIPTION",sizeof(BAPI_MAKT),0,0);
			if (thBapi_Makt==ITAB_NULL) rfc_error("ItCreate MATERIALDESCRIPTION");
		}
		else if (ItFree(thBapi_Makt) != 0) rfc_error("ItFree MATERIALDESCRIPTION");

		if (thBapi_Marm==ITAB_NULL)
		{
			 thBapi_Marm = ItCreate("UNITSOFMEASURE",sizeof(BAPI_MARM),0,0);
			 if (thBapi_Marm==ITAB_NULL) rfc_error("ItCreate UNITSOFMEASURE");
		}
		else if (ItFree(thBapi_Marm)!= 0) rfc_error("ItFree UNITSOFMEASURE");

		if (thBapi_Marmx==ITAB_NULL)
		{
			 thBapi_Marmx = ItCreate("UNITSOFMEASUREX",sizeof(BAPI_MARMX),0,0);
			if (thBapi_Marmx==ITAB_NULL) rfc_error("ItCreate UNITSOFMEASUREX");
		}
		else if (ItFree(thBapi_Marmx)!= 0) rfc_error("ItFree UNITSOFMEASUREX");

		if (thBAPIE1PAREX3==ITAB_NULL)
		{
			thBAPIE1PAREX3 = ItCreate("EXTENSIONIN",sizeof(BAPIE1PAREX3),0,0);
			if (thBAPIE1PAREX3==ITAB_NULL)
				rfc_error("ItCreate EXTENSIONIN");
		}
		else
		{
			if (ItFree(thBAPIE1PAREX3)!= 0)
				rfc_error("ItFree EXTENSIONIN");
		}

		if (thBAPIE1PAREXX3==ITAB_NULL)
		{
			thBAPIE1PAREXX3 = ItCreate("EXTENSIONINX",sizeof(BAPIE1PAREXX3),0,0);
			if (thBAPIE1PAREXX3==ITAB_NULL)
				rfc_error("ItCreate EXTENSIONINX");
		}
		else
		{
			if (ItFree(thBAPIE1PAREXX3)!= 0)
				rfc_error("ItFree EXTENSIONINX");
		}

		tBapi_Makt = ItAppLine (thBapi_Makt);
		tBapi_Marm = ItAppLine (thBapi_Marm);
		tBapi_Marmx = ItAppLine (thBapi_Marmx);
		tBAPIE1PAREX3 = ItAppLine(thBAPIE1PAREX3) ;
		tBAPIE1PAREXX3 = ItAppLine(thBAPIE1PAREXX3) ;

		if (tBapi_Makt == NULL)	rfc_error("ItAppLine BAPI_MAKT");
		if (tBapi_Marm == NULL)	rfc_error("ItAppLine BAPI_MARM second table");
		if (tBapi_Marmx == NULL) rfc_error("ItAppLine BAPI_MARMX");
		if (tBAPIE1PAREX3 == NULL) rfc_error("ItAppLine BAPIE1PAREX3");
		if (tBAPIE1PAREXX3 == NULL) rfc_error("ItAppLine BAPIE1PAREXX3");

		SETCHAR(eBapiMatHead.Material,part_noDup);
		SETCHAR(eMrpView.Plant,plantcode);
		SETCHAR(eMrpViewx.Plant,plantcode);
		SETCHAR(eBapi_Mpgd.Plant,plantcode);
		SETCHAR(eBapi_Mpgdx.Plant,plantcode);
		SETCHAR(eBapi_Mard.Plant,plantcode);
		SETCHAR(eBapi_Mardx.Plant,plantcode);
		SETCHAR(eAccView.Val_Area,plantcode);
		SETCHAR(eAccViewx.Val_Area,plantcode);
		SETCHAR(eBasicView.Base_Uom,meas_unit);
		SETCHAR(eBasicView.Base_Uom_Iso,meas_unit);
		SETCHAR(eBasicViewx.Base_Uom,"X");
		SETCHAR(eBasicViewx.Base_Uom_Iso,"X");

		SETCHAR(tBapi_Marm->Alt_Unit,meas_unit);
		SETCHAR(tBapi_Marm->Alt_Unit_Iso,meas_unit);
		SETCHAR(tBapi_Marmx->Alt_Unit,meas_unit);
		SETCHAR(tBapi_Marmx->Alt_Unit_Iso,meas_unit);
		SETCHAR(eRmmg1_Aennr.Aennr,dml_no_arg);
		if (tc_strcmp(mat_type,"FERT") != 0)
		{
			printf("\npart is not a VC"); fflush(stdout);
			SETCHAR(eBapiMatHead.Quality_View,"X");//qm view
		}
		else if(tc_strcmp(mat_type,"FERT") == 0)	//Anuja,Swati Tendulkar - QM View for VC
		{
			printf("\nQM View For VC");fflush(stdout);
			printf("\n[%s,%s,%s,%s,%s]",qua_ins_ind,doc_req,grptime,catalog_prof,qm_proc_ind);fflush(stdout);

			SETCHAR(eBapiMatHead.Quality_View,"X");	/*quality view*/
			SETCHAR(eMrpView.Ind_Post_To_Insp_Stock,"");
			SETCHAR(eMrpViewx.Ind_Post_To_Insp_Stock,"");
			SETCHAR(eMrpView.Doc_Reqd,"");
			SETCHAR(eMrpViewx.Doc_Reqd,"");
			SETBCD(eMrpView.Gr_Pr_Time,grptime,0);
			SETCHAR(eMrpViewx.Gr_Pr_Time,"X");
			SETCHAR(eBasicView.Catprofile,catalog_prof);
			SETCHAR(eBasicViewx.Catprofile,"X");
			SETCHAR(eBasicView.Qm_Procmnt,"");
			SETCHAR(eBasicViewx.Qm_Procmnt,"");
			SETCHAR(eMrpView.Ctrl_Key,control_key);
			SETCHAR(eMrpViewx.Ctrl_Key,"X");
			SETCHAR(eMrpView.Cert_Type,certificate_type);
			SETCHAR(eMrpViewx.Cert_Type,"X");
		}
		else SETCHAR(eBapiMatHead.Quality_View,"");     //qm view
		// construct string for the mtchmdu string
		SETCHAR(eBapiMatHead.Basic_View,"X");		//basic view
		SETCHAR(tBapi_Makt->Matl_Desc,descDup);
		if(tc_strlen(doc_noDup)==0)
		{
				SETCHAR(eBasicView.Document,"");
				SETCHAR(eBasicViewx.Document,"");
		}
		else
		{
			SETCHAR(eBasicView.Document,doc_noDup);
			SETCHAR(eBasicViewx.Document,"X");
		}
		if(tc_strlen(dwg_typeDup)==0)
		{
				SETCHAR(eBasicView.Doc_Type,"");
				SETCHAR(eBasicViewx.Doc_Type,"");
		}
		else
		{
			SETCHAR(eBasicView.Doc_Type,dwg_typeDup);
			SETCHAR(eBasicViewx.Doc_Type,"X");
		}
		if(tc_strlen(dwg_revDup)==0)
		{
				SETCHAR(eBasicView.Doc_Vers,"");
				SETCHAR(eBasicViewx.Doc_Vers,"");
		}
		else
		{
			SETCHAR(eBasicView.Doc_Vers,dwg_revDup);
			SETCHAR(eBasicViewx.Doc_Vers,"X");
		}
		if(tc_strlen(sheet_noDup)==0)
		{
				SETCHAR(eBasicView.No_Sheets,"");
				SETCHAR(eBasicViewx.No_Sheets,"");
		}
		else
		{
			SETNUM(eBasicView.No_Sheets,sheet_noDup);
			SETCHAR(eBasicViewx.No_Sheets,"X");
		}
		SETCHAR(eBapiMatHead.Mrp_View,"X");	//mrp view
		SETCHAR(eMrpView.Mrp_Type,"");//mrp_type X
		SETCHAR(eMrpViewx.Mrp_Type,"");
		SETCHAR(eMrpView.Lotsizekey,"");//lot_size_key X
		SETCHAR(eMrpViewx.Lotsizekey,"");
		tc_strdup("S",&price_con);
		//if(car_dml==1)
		//{
		//	SETCHAR(eMrpView.Proc_Type,proc_type);
		//	SETCHAR(eMrpViewx.Proc_Type,"");
		//	SETCHAR(eMrpView.Spproctype,spl_proc_key);
		//	SETCHAR(eMrpViewx.Spproctype,"");
		//}
		//else
		//{
		//As per new request manisha madam 28.12.2007 car dml cs changes also trasffered to sap
			//SETCHAR(eMrpView.Proc_Type,proc_type);
			//SETCHAR(eMrpViewx.Proc_Type,"X");
			//SETCHAR(eMrpView.Spproctype,spl_proc_key);
			//SETCHAR(eMrpViewx.Spproctype,"X");
		//}

       
	    //SCO-7079-Rajendra-G part-CS change stop from PLM -13 Nov 2017

		if(*part_noDup=='G')
		{
			printf("\nGrp Part...... \n");fflush(stdout);
			SETCHAR(eMrpView.Proc_Type,proc_type);
			SETCHAR(eMrpViewx.Proc_Type,"");
			SETCHAR(eMrpView.Spproctype,spl_proc_key);
			SETCHAR(eMrpViewx.Spproctype,"");
		}
        else
		{
			//As per new request manisha madam 28.12.2007 car dml cs changes also trasffered to sap
			SETCHAR(eMrpView.Proc_Type,proc_type);
			SETCHAR(eMrpViewx.Proc_Type,"X");
			SETCHAR(eMrpView.Spproctype,spl_proc_key);
			SETCHAR(eMrpViewx.Spproctype,"X");
		}

		SETCHAR(eMrpView.Mrp_Group,"");//mrp_grp X
		SETCHAR(eMrpViewx.Mrp_Group,"");
		SETBCD(eMrpView.Reorder_Pt,myzeroDup3,3);//reord_pt X
		SETCHAR(eMrpViewx.Reorder_Pt,"");
		SETBCD(eMrpView.Max_Stock,max_stlvl,3);
		SETCHAR(eMrpViewx.Max_Stock,"X");
		SETCHAR(eBapiMatHead.Account_View,"X"); //accounting view
		SETCHAR(eAccView.Val_Class,val_class);
		SETCHAR(eAccViewx.Val_Class,"X");
		SETCHAR(eAccView.Overhead_Grp,overhd_grp);
		SETCHAR(eAccViewx.Overhead_Grp,"X");
		SETCHAR(eAccView.Orig_Group,origin_group);
		SETCHAR(eAccViewx.Orig_Group,"X");

		SETCHAR(eBapiMatHead.Ind_Sector,"M");
		SETCHAR(eBapiMatHead.Matl_Type,mat_type);
		SETCHAR(eBapiMatHead.Sales_View,"");
		SETCHAR(eBapiMatHead.Purchase_View,"");
		SETCHAR(eBapiMatHead.Forecast_View,"");
		SETCHAR(eBapiMatHead.Work_Sched_View,"");
		SETCHAR(eBapiMatHead.Prt_View,"");
		SETCHAR(eBapiMatHead.Storage_View,"");
		SETCHAR(eBapiMatHead.Warehouse_View,"");
		SETCHAR(eBapiMatHead.Cost_View,"");
		SETCHAR(eBapiMatHead.Inp_Fld_Check,"");
		SETCHAR(eBapiMatHead.Material_External,"");
		SETCHAR(eBapiMatHead.MateriaL_Guid,"");
		SETCHAR(eBapiMatHead.Material_Version,"");

		SETCHAR(eBasicView.Del_Flag,"");
		SETCHAR(eBasicView.Matl_Group,"");
		SETCHAR(eBasicView.Old_Mat_No,OldMatNoDup);
		SETCHAR(eBasicView.Po_Unit,"");
		SETCHAR(eBasicView.Po_Unit_Iso,"");
		SETCHAR(eBasicView.Doc_Format,"");
		SETCHAR(eBasicView.Doc_Chg_No,"");
		SETCHAR(eBasicView.Page_No,"");
		SETNUM(eBasicView.No_Sheets,"");
		SETCHAR(eBasicView.Prod_Memo,"");
		SETCHAR(eBasicView.Pageformat,"");
		SETCHAR(eBasicView.Size_Dim,"");
		SETCHAR(eBasicView.Basic_Matl,"");
		SETCHAR(eBasicView.Std_Descr,"");
		SETCHAR(eBasicView.Dsn_Office,"");
		SETCHAR(eBasicView.Pur_Valkey,"");
		SETBCD(eBasicView.Net_Weight,myzeroDup3,3);
		SETCHAR(eBasicView.Unit_Of_Wt,"");
		SETCHAR(eBasicView.Unit_Of_Wt_Iso,"");
		SETCHAR(eBasicView.Container,"");
		SETCHAR(eBasicView.Stor_Conds,"");
		SETCHAR(eBasicView.Temp_Conds,"");
		SETCHAR(eBasicView.Trans_Grp,"");
		SETCHAR(eBasicView.Haz_Mat_No,"");
		SETCHAR(eBasicView.Division,"");
		SETCHAR(eBasicView.Competitor,"");
		SETBCD(eBasicView.Qty_Gr_Gi,myzeroDup3,3);
		SETCHAR(eBasicView.Proc_Rule,"");
		SETCHAR(eBasicView.Sup_Source,"");
		SETCHAR(eBasicView.Season,"");
		SETCHAR(eBasicView.Label_Type,"");
		SETCHAR(eBasicView.Label_Form,"");
		SETCHAR(eBasicView.Prod_Hier,"");
		SETCHAR(eBasicView.Cad_Id,"X");
		SETBCD(eBasicView.Allowed_Wt,myzeroDup3,3);
		SETCHAR(eBasicView.Pack_Wt_Un,"");
		SETCHAR(eBasicView.Pack_Wt_Un_Iso,"");
		SETBCD(eBasicView.Allwd_Vol,myzeroDup3,3);
		SETCHAR(eBasicView.Pack_Vo_Un,"");
		SETCHAR(eBasicView.Pack_Vo_Un_Iso,"");
		SETBCD(eBasicView.Wt_Tol_Lt,myzeroDup1,1);
		SETBCD(eBasicView.Vol_Tol_Lt,myzeroDup1,1);
		SETCHAR(eBasicView.Var_Ord_Un,"");
		SETCHAR(eBasicView.Batch_Mgmt,"");
		SETCHAR(eBasicView.Sh_Mat_Typ,"");
		SETBCD(eBasicView.Fill_Level,"0",0);
		SETINT2(&eBasicView.Stack_Fact,"0");
		SETCHAR(eBasicView.Mat_Grp_Sm,"");
		SETCHAR(eBasicView.Authoritygroup,"");
		SETCHAR(eBasicView.Qm_Procmnt,"");
		SETCHAR(eBasicView.Catprofile,"");
		SETBCD(eBasicView.Minremlife,"0",0);
		SETBCD(eBasicView.Shelf_Life,"0",0);
		SETBCD(eBasicView.Stor_Pct,"0",0);
		SETCHAR(eBasicView.Pur_Status,"");
		SETCHAR(eBasicView.Sal_Status,"");
		SETDATE(eBasicView.Pvalidfrom,"");
		SETDATE(eBasicView.Svalidfrom,"");
		SETCHAR(eBasicView.Envt_Rlvt,"");
		SETCHAR(eBasicView.Prod_Alloc,"");
		SETCHAR(eBasicView.Qual_Dik,"");
		SETCHAR(eBasicView.Manu_Mat,"");
		SETCHAR(eBasicView.Mfr_No,"");
		SETCHAR(eBasicView.Inv_Mat_No,"");
		SETCHAR(eBasicView.Manuf_Prof,"");
		SETCHAR(eBasicView.Hazmatprof,"");
		SETCHAR(eBasicView.High_Visc,"");
		SETCHAR(eBasicView.Looseorliq,"");
		SETCHAR(eBasicView.Closed_Box,"");
		SETCHAR(eBasicView.Appd_B_Rec,"");
		SETNUM(eBasicView.Matcmpllvl,"00");
		SETCHAR(eBasicView.Par_Eff,"");
		SETCHAR(eBasicView.Round_Up_Rule_Expiration_Date,"");
		SETCHAR(eBasicView.Period_Ind_Expiration_Date,"D");
		SETCHAR(eBasicView.Prod_Composition_On_Packaging,"");
		SETCHAR(eBasicView.Item_Cat,"");
		SETCHAR(eBasicView.Haz_Mat_No_External,"");
		SETCHAR(eBasicView.Haz_Mat_No_Guid,"");
		SETCHAR(eBasicView.Haz_Mat_No_Version,"");
		SETCHAR(eBasicView.Inv_Mat_No_External,"");
		SETCHAR(eBasicView.Inv_Mat_No_Guid,"");
		SETCHAR(eBasicView.Inv_Mat_No_Version,"");
		SETCHAR(eBasicView.Material_Fixed,"");
		SETCHAR(eBasicView.Cm_Relevance_Flag,"");
		SETCHAR(eBasicView.Sled_Bbd,"");
		SETCHAR(eBasicView.Gtin_Variant,"");
		SETCHAR(eBasicView.Serialization_Level,"");
		SETCHAR(eBasicView.Pl_Ref_Mat,"");
		SETCHAR(eBasicView.Extmatlgrp,"");
		SETCHAR(eBasicView.Uomusage,"");
		SETCHAR(eBasicView.Pl_Ref_Mat_External,"");
		SETCHAR(eBasicView.Pl_Ref_Mat_Guid,"");
		SETCHAR(eBasicView.Pl_Ref_Mat_Version,"");

		SETCHAR(eBasicViewx.Del_Flag,"");
		SETCHAR(eBasicViewx.Matl_Group,"");
		SETCHAR(eBasicViewx.Old_Mat_No,"");
		SETCHAR(eBasicViewx.Po_Unit,"");
		SETCHAR(eBasicViewx.Po_Unit_Iso,"");
		SETCHAR(eBasicViewx.Doc_Format,"");
		SETCHAR(eBasicViewx.Doc_Chg_No,"");
		SETCHAR(eBasicViewx.Page_No,"");
		SETCHAR(eBasicViewx.No_Sheets,"");
		SETCHAR(eBasicViewx.Prod_Memo,"");
		SETCHAR(eBasicViewx.Pageformat,"");
		SETCHAR(eBasicViewx.Size_Dim,"");
		SETCHAR(eBasicViewx.Basic_Matl,"");
		SETCHAR(eBasicViewx.Std_Descr,"");
		SETCHAR(eBasicViewx.Dsn_Office,"");
		SETCHAR(eBasicViewx.Pur_Valkey,"");
		SETCHAR(eBasicViewx.Net_Weight,"");
		SETCHAR(eBasicViewx.Unit_Of_Wt,"");
		SETCHAR(eBasicViewx.Unit_Of_Wt_Iso,"");
		SETCHAR(eBasicViewx.Container,"");
		SETCHAR(eBasicViewx.Stor_Conds,"");
		SETCHAR(eBasicViewx.Temp_Conds,"");
		SETCHAR(eBasicViewx.Trans_Grp,"");
		SETCHAR(eBasicViewx.Haz_Mat_No,"");
		SETCHAR(eBasicViewx.Division,"");
		SETCHAR(eBasicViewx.Competitor,"");
		SETCHAR(eBasicViewx.Qty_Gr_Gi,"");
		SETCHAR(eBasicViewx.Proc_Rule,"");
		SETCHAR(eBasicViewx.Sup_Source,"");
		SETCHAR(eBasicViewx.Season,"");
		SETCHAR(eBasicViewx.Label_Type,"");
		SETCHAR(eBasicViewx.Label_Form,"");
		SETCHAR(eBasicViewx.Prod_Hier,"");
		SETCHAR(eBasicViewx.Cad_Id,"X");
		SETCHAR(eBasicViewx.Allowed_Wt,"");
		SETCHAR(eBasicViewx.Pack_Wt_Un,"");
		SETCHAR(eBasicViewx.Pack_Wt_Un_Iso,"");
		SETCHAR(eBasicViewx.Allwd_Vol,"");
		SETCHAR(eBasicViewx.Pack_Vo_Un,"");
		SETCHAR(eBasicViewx.Pack_Vo_Un_Iso,"");
		SETCHAR(eBasicViewx.Wt_Tol_Lt,"");
		SETCHAR(eBasicViewx.Vol_Tol_Lt,"");
		SETCHAR(eBasicViewx.Var_Ord_Un,"");
		SETCHAR(eBasicViewx.Batch_Mgmt,"");
		SETCHAR(eBasicViewx.Sh_Mat_Typ,"");
		SETCHAR(eBasicViewx.Fill_Level,"");
		SETCHAR(eBasicViewx.Stack_Fact,"");
		SETCHAR(eBasicViewx.Mat_Grp_Sm,"");
		SETCHAR(eBasicViewx.Authoritygroup,"");
		SETCHAR(eBasicViewx.Qm_Procmnt,"");
		SETCHAR(eBasicViewx.Catprofile,"");
		SETCHAR(eBasicViewx.Minremlife,"");
		SETCHAR(eBasicViewx.Shelf_Life,"");
		SETCHAR(eBasicViewx.Stor_Pct,"");
		SETCHAR(eBasicViewx.Pur_Status,"");
		SETCHAR(eBasicViewx.Sal_Status,"");
		SETCHAR(eBasicViewx.Pvalidfrom,"");
		SETCHAR(eBasicViewx.Svalidfrom,"");
		SETCHAR(eBasicViewx.Envt_Rlvt,"");
		SETCHAR(eBasicViewx.Prod_Alloc,"");
		SETCHAR(eBasicViewx.Qual_Dik,"");
		SETCHAR(eBasicViewx.Manu_Mat,"");
		SETCHAR(eBasicViewx.Mfr_No,"");
		SETCHAR(eBasicViewx.Inv_Mat_No,"");
		SETCHAR(eBasicViewx.Manuf_Prof,"");
		SETCHAR(eBasicViewx.Hazmatprof,"");
		SETCHAR(eBasicViewx.High_Visc,"");
		SETCHAR(eBasicViewx.Looseorliq,"");
		SETCHAR(eBasicViewx.Closed_Box,"");
		SETCHAR(eBasicViewx.Appd_B_Rec,"");
		SETCHAR(eBasicViewx.Matcmpllvl,"");
		SETCHAR(eBasicViewx.Par_Eff,"");
		SETCHAR(eBasicViewx.Round_Up_Rule_Expiration_Date,"");
		SETCHAR(eBasicViewx.Period_Ind_Expiration_Date,"");
		SETCHAR(eBasicViewx.Prod_Composition_On_Packaging,"");
		SETCHAR(eBasicViewx.Item_Cat,"");
		SETCHAR(eBasicViewx.Haz_Mat_No_External,"");
		SETCHAR(eBasicViewx.Haz_Mat_No_Guid,"");
		SETCHAR(eBasicViewx.Haz_Mat_No_Version,"");
		SETCHAR(eBasicViewx.Inv_Mat_No_External,"");
		SETCHAR(eBasicViewx.Inv_Mat_No_Guid,"");
		SETCHAR(eBasicViewx.Inv_Mat_No_Version,"");
		SETCHAR(eBasicViewx.Material_Fixed,"");
		SETCHAR(eBasicViewx.Cm_Relevance_Flag,"");
		SETCHAR(eBasicViewx.Sled_Bbd,"");
		SETCHAR(eBasicViewx.Gtin_Variant,"");
		SETCHAR(eBasicViewx.Serialization_Level,"");
		SETCHAR(eBasicViewx.Pl_Ref_Mat,"");
		SETCHAR(eBasicViewx.Extmatlgrp,"");
		SETCHAR(eBasicViewx.Uomusage,"");
		SETCHAR(eBasicViewx.Pl_Ref_Mat_External,"");
		SETCHAR(eBasicViewx.Pl_Ref_Mat_Guid,"");
		SETCHAR(eBasicViewx.Pl_Ref_Mat_Version,"");

		SETCHAR(eMrpView.Del_Flag,"");
		SETCHAR(eMrpView.Abc_Id,"");
		SETCHAR(eMrpView.Crit_Part,"");
		SETCHAR(eMrpView.Pur_Group,"");
		SETCHAR(eMrpView.Issue_Unit,"");
		SETCHAR(eMrpView.Issue_Unit_Iso,"");
		SETCHAR(eMrpView.Mrpprofile,"");
		SETCHAR(eMrpView.Mrp_Ctrler,"");
		SETCHAR(eMrpView.Plnd_Delry,"0");
		SETBCD(eMrpView.Gr_Pr_Time,"0",0);
		SETCHAR(eMrpView.Period_Ind,"");
		SETBCD(eMrpView.Assy_Scrap,myzeroDup2,2);
		SETBCD(eMrpView.Safety_Stk,myzeroDup3,3);
		SETBCD(eMrpView.Minlotsize,myzeroDup3,3);
		SETBCD(eMrpView.Maxlotsize,myzeroDup3,3);
		SETBCD(eMrpView.Fixed_Lot,myzeroDup3,3);
		SETBCD(eMrpView.Round_Val,myzeroDup3,3);
		SETBCD(eMrpView.Ord_Costs,myzeroDup4,4);
		SETCHAR(eMrpView.Dep_Req_Id,"");
		SETCHAR(eMrpView.Stor_Costs,"");
		SETCHAR(eMrpView.Alt_Bom_Id,"");
		SETCHAR(eMrpView.Discontinu,"");
		SETCHAR(eMrpView.Eff_O_Day,"");
		SETCHAR(eMrpView.Follow_Up,"");
		SETCHAR(eMrpView.Grp_Reqmts,"");
		SETCHAR(eMrpView.Mixed_Mrp,"");
		SETCHAR(eMrpView.Backflush,"");
		SETCHAR(eMrpView.Production_Scheduler,"");
		SETBCD(eMrpView.Proc_Time,myzeroDup2,2);
		SETBCD(eMrpView.Setuptime,myzeroDup2,2);
		SETBCD(eMrpView.Interop,myzeroDup2,2);
		SETBCD(eMrpView.Base_Qty,myzeroDup3,3);
		SETBCD(eMrpView.Inhseprodt,"0",0);
		SETBCD(eMrpView.Stgeperiod,"0",0);
		SETCHAR(eMrpView.Stge_Pd_Un,"");
		SETCHAR(eMrpView.Stge_Pd_Un_Iso,"");
		SETBCD(eMrpView.Over_Tol,myzeroDup1,1);
		SETCHAR(eMrpView.Unlimited,"");
		SETBCD(eMrpView.Under_Tol,myzeroDup1,1);
		SETBCD(eMrpView.Replentime,"0",0);
		SETCHAR(eMrpView.Replace_Pt,"");
		SETCHAR(eMrpView.Ind_Post_To_Insp_Stock,"");
		SETCHAR(eMrpView.Doc_Reqd,"");
		SETCHAR(eMrpView.Loadinggrp,"");
		SETCHAR(eMrpView.Batch_Mgmt,"");
		SETCHAR(eMrpView.Quotausage,"");
		SETBCD(eMrpView.Serv_Level,myzeroDup1,1);
		SETCHAR(eMrpView.Split_Ind,"");
		SETCHAR(eMrpView.Availcheck,"");
		SETCHAR(eMrpView.Fy_Variant,"");
		SETCHAR(eMrpView.Corr_Fact,"");
		SETBCD(eMrpView.Setup_Time,myzeroDup2,2);
		SETBCD(eMrpView.Base_Qty_Plan,myzeroDup3,3);
		SETBCD(eMrpView.Ship_Proc_Time,myzeroDup2,2);
		SETCHAR(eMrpView.Sup_Source,"");
		SETCHAR(eMrpView.Auto_P_Ord,"");
		SETCHAR(eMrpView.Sourcelist,"");
		SETCHAR(eMrpView.Comm_Code,"");
		SETCHAR(eMrpView.Countryori,"");
		SETCHAR(eMrpView.Countryori_Iso,"");
		SETCHAR(eMrpView.Regionorig,"");
		SETCHAR(eMrpView.Comm_Co_Un,"");
		SETCHAR(eMrpView.Comm_Co_Un_Iso,"");
		SETCHAR(eMrpView.Expimpgrp,"");
		SETCHAR(eMrpView.Profit_Ctr,"");
		SETCHAR(eMrpView.Ppc_Pl_Cal,"");
		//SETCHAR(eMrpView.Rep_Manuf,"");
		SETNUM(eMrpView.Pl_Ti_Fnce,"000");
		SETCHAR(eMrpView.Consummode,"");
		SETNUM(eMrpView.Bwd_Cons,"000");
		SETNUM(eMrpView.Fwd_Cons,"000");
		SETCHAR(eMrpView.Alternative_Bom,"");
		SETCHAR(eMrpView.Bom_Usage,"");
		SETCHAR(eMrpView.Planlistgrp,"");
		SETCHAR(eMrpView.Planlistcnt,"");
		SETBCD(eMrpView.Lot_Size,myzeroDup3,3);
		SETCHAR(eMrpView.Specprocty,"");
		SETCHAR(eMrpView.Prod_Unit,"");
		SETCHAR(eMrpView.Prod_Unit_Iso,"");
		SETCHAR(eMrpView.Iss_St_Loc,"");
		SETBCD(eMrpView.Comp_Scrap,myzeroDup2,2);
		SETBCD(eMrpView.Cycle_Time,"0",0);
		SETCHAR(eMrpView.Covprofile,"");
		SETCHAR(eMrpView.Cc_Ph_Inv,"");
		SETCHAR(eMrpView.Variance_Key,"");
		SETCHAR(eMrpView.Serno_Prof,"");
		//SETCHAR(eMrpView.Repmanprof,"");
		SETCHAR(eMrpView.Neg_Stocks,"");
		SETCHAR(eMrpView.Qm_Rgmts,"");
		SETCHAR(eMrpView.Plng_Cycle,"");
		SETCHAR(eMrpView.Round_Prof,"");
		SETCHAR(eMrpView.Refmatcons,"");
		SETCHAR(eMrpView.D_To_Ref_M,"");
		SETBCD(eMrpView.Mult_Ref_M,myzeroDup2,2);
		SETCHAR(eMrpView.Auto_Reset,"");
		SETCHAR(eMrpView.Ex_Cert_Id,"");
		SETCHAR(eMrpView.Ex_Cert_No_New,"");
		SETCHAR(eMrpView.Ex_Cert_Dt,"");
		SETCHAR(eMrpView.Milit_Id,"");
		SETBCD(eMrpView.Insp_Int,"0",0);
		SETCHAR(eMrpView.Co_Product,"");
		SETCHAR(eMrpView.Plan_Strgp,"");
		SETCHAR(eMrpView.Sloc_Exprc,"");
		SETCHAR(eMrpView.Bulk_Mat,"");
		SETCHAR(eMrpView.Cc_Fixed,"");
		SETCHAR(eMrpView.Determ_Grp,"");
		SETCHAR(eMrpView.Qm_Authgrp,"");
		SETCHAR(eMrpView.Task_List_Type,"");
		SETCHAR(eMrpView.Pur_Status,"");
		SETCHAR(eMrpView.Prodprof,"");
		SETCHAR(eMrpView.Safty_T_Id,"");
		SETNUM(eMrpView.Safetytime,"00");
		SETCHAR(eMrpView.Plord_Ctrl,"");
		SETCHAR(eMrpView.Batchentry,"");
		SETDATE(eMrpView.Pvalidfrom,"");
		SETCHAR(eMrpView.Matfrgtgrp,"");
		SETCHAR(eMrpView.Prodverscs,"");
		SETCHAR(eMrpView.Mat_Cfop,"");
		SETCHAR(eMrpView.Eu_List_No,"");
		SETCHAR(eMrpView.Eu_Mat_Grp,"");
		SETCHAR(eMrpView.Cas_No,"");
		SETCHAR(eMrpView.Prodcom_No,"");
		SETCHAR(eMrpView.Ctrl_Code,"");
		SETCHAR(eMrpView.Jit_Relvt,"");
		SETCHAR(eMrpView.Mat_Grp_Trans,"");
		SETCHAR(eMrpView.Handlg_Grp,"");
		SETCHAR(eMrpView.Supply_Area,"");
		SETCHAR(eMrpView.Fair_Share_Rule,"");
		SETCHAR(eMrpView.Push_Distrib,"");
		SETBCD(eMrpView.Deploy_Horiz,"0",0);
		SETBCD(eMrpView.Min_Lot_Size,myzeroDup3,3);
		SETBCD(eMrpView.Max_Lot_Size,myzeroDup3,3);
		SETBCD(eMrpView.Fix_Lot_Size,myzeroDup3,3);
		SETBCD(eMrpView.Lot_Increment,myzeroDup3,3);
		SETCHAR(eMrpView.Prod_Conv_Type,"");
		SETCHAR(eMrpView.Distr_Prof,"");
		SETCHAR(eMrpView.Period_Profile_Safety_Time,"");
		SETCHAR(eMrpView.Fxd_Price,"");
		SETCHAR(eMrpView.Avail_Check_All_Proj_Segments,"");
		SETCHAR(eMrpView.Overallprf,"");
		SETCHAR(eMrpView.Mrp_Relevancy_Dep_Requirements,"");
		SETBCD(eMrpView.Min_Safety_Stk,myzeroDup2,2);
		SETCHAR(eMrpView.No_Costing,"");
		SETCHAR(eMrpView.Unit_Group,"");
		SETCHAR(eMrpView.Follow_Up_External,"");
		SETCHAR(eMrpView.Follow_Up_Guid,"");
		SETCHAR(eMrpView.Follow_Up_Version,"");
		SETCHAR(eMrpView.Refmatcons_External,"");
		SETCHAR(eMrpView.Refmatcons_Guid,"");
		SETCHAR(eMrpView.Refmatcons_Version,"");
		SETCHAR(eMrpView.Rotation_Date,"");
		SETCHAR(eMrpView.Original_Batch_Flag,"");
		SETCHAR(eMrpView.Original_Batch_Ref_Material,"");
		SETCHAR(eMrpView.Original_Batch_Ref_Material_E,"");
		SETCHAR(eMrpView.Original_Batch_Ref_Material_V,"");
		SETCHAR(eMrpView.Original_Batch_Ref_Material_G,"");

		SETCHAR(eMrpViewx.Del_Flag,"");
		SETCHAR(eMrpViewx.Abc_Id,"");
		SETCHAR(eMrpViewx.Crit_Part,"");
		SETCHAR(eMrpViewx.Pur_Group,"");
		SETCHAR(eMrpViewx.Issue_Unit,"");
		SETCHAR(eMrpViewx.Issue_Unit_Iso,"");
		SETCHAR(eMrpViewx.Mrpprofile,"");
		SETCHAR(eMrpViewx.Mrp_Ctrler,"");
		SETCHAR(eMrpViewx.Plnd_Delry,"");
		SETCHAR(eMrpViewx.Gr_Pr_Time,"");
		SETCHAR(eMrpViewx.Period_Ind,"");
		SETCHAR(eMrpViewx.Assy_Scrap,"");
		SETCHAR(eMrpViewx.Safety_Stk,"");
		SETCHAR(eMrpViewx.Minlotsize,"");
		SETCHAR(eMrpViewx.Maxlotsize,"");
		SETCHAR(eMrpViewx.Fixed_Lot,"");
		SETCHAR(eMrpViewx.Round_Val,"");
		SETCHAR(eMrpViewx.Ord_Costs,"");
		SETCHAR(eMrpViewx.Dep_Req_Id,"");
		SETCHAR(eMrpViewx.Stor_Costs,"");
		SETCHAR(eMrpViewx.Alt_Bom_Id,"");
		SETCHAR(eMrpViewx.Discontinu,"");
		SETCHAR(eMrpViewx.Eff_O_Day,"");
		SETCHAR(eMrpViewx.Follow_Up,"");
		SETCHAR(eMrpViewx.Grp_Reqmts,"");
		SETCHAR(eMrpViewx.Mixed_Mrp,"");
		SETCHAR(eMrpViewx.Backflush,"");
		SETCHAR(eMrpViewx.Production_Scheduler,"");
		SETCHAR(eMrpViewx.Proc_Time,"");
		SETCHAR(eMrpViewx.Setuptime,"");
		SETCHAR(eMrpViewx.Interop,"");
		SETCHAR(eMrpViewx.Base_Qty,"");
		SETCHAR(eMrpViewx.Inhseprodt,"");
		SETCHAR(eMrpViewx.Stgeperiod,"");
		SETCHAR(eMrpViewx.Stge_Pd_Un,"");
		SETCHAR(eMrpViewx.Stge_Pd_Un_Iso,"");
		SETCHAR(eMrpViewx.Over_Tol,"");
		SETCHAR(eMrpViewx.Unlimited,"");
		SETCHAR(eMrpViewx.Under_Tol,"");
		SETCHAR(eMrpViewx.Replentime,"");
		SETCHAR(eMrpViewx.Replace_Pt,"");
		SETCHAR(eMrpViewx.Ind_Post_To_Insp_Stock,"");
		SETCHAR(eMrpViewx.Doc_Reqd,"");
		SETCHAR(eMrpViewx.Loadinggrp,"");
		SETCHAR(eMrpViewx.Batch_Mgmt,"");
		SETCHAR(eMrpViewx.Quotausage,"");
		SETCHAR(eMrpViewx.Serv_Level,"");
		SETCHAR(eMrpViewx.Split_Ind,"");
		SETCHAR(eMrpViewx.Availcheck,"");
		SETCHAR(eMrpViewx.Fy_Variant,"");
		SETCHAR(eMrpViewx.Corr_Fact,"");
		SETCHAR(eMrpViewx.Setup_Time,"");
		SETCHAR(eMrpViewx.Base_Qty_Plan,"");
		SETCHAR(eMrpViewx.Ship_Proc_Time,"");
		SETCHAR(eMrpViewx.Sup_Source,"");
		SETCHAR(eMrpViewx.Auto_P_Ord,"");
		SETCHAR(eMrpViewx.Sourcelist,"");
		SETCHAR(eMrpViewx.Comm_Code,"");
		SETCHAR(eMrpViewx.Countryori,"");
		SETCHAR(eMrpViewx.Countryori_Iso,"");
		SETCHAR(eMrpViewx.Regionorig,"");
		SETCHAR(eMrpViewx.Comm_Co_Un,"");
		SETCHAR(eMrpViewx.Comm_Co_Un_Iso,"");
		SETCHAR(eMrpViewx.Expimpgrp,"");
		SETCHAR(eMrpViewx.Profit_Ctr,"");
		SETCHAR(eMrpViewx.Ppc_Pl_Cal,"");
		//SETCHAR(eMrpViewx.Rep_Manuf,"");
		SETCHAR(eMrpViewx.Pl_Ti_Fncve,"");
		SETCHAR(eMrpViewx.Consummode,"");
		SETCHAR(eMrpViewx.Bwd_Cons,"");
		SETCHAR(eMrpViewx.Fwd_Cons,"");
		SETCHAR(eMrpViewx.Alternative_Bom,"");
		SETCHAR(eMrpViewx.Bom_Usage,"");
		SETCHAR(eMrpViewx.Planlistgrp,"");
		SETCHAR(eMrpViewx.Planlistcnt,"");
		SETCHAR(eMrpViewx.Lot_Size,"");
		SETCHAR(eMrpViewx.Specprocty,"");
		SETCHAR(eMrpViewx.Prod_Unit,"");
		SETCHAR(eMrpViewx.Prod_Unit_Iso,"");
		SETCHAR(eMrpViewx.Iss_St_Loc,"");
		SETCHAR(eMrpViewx.Comp_Scrap,"");
		SETCHAR(eMrpViewx.Cycle_Time,"");
		SETCHAR(eMrpViewx.Covprofile,"");
		SETCHAR(eMrpViewx.Cc_Ph_Inv,"");
		SETCHAR(eMrpViewx.Variance_Key,"");
		SETCHAR(eMrpViewx.Serno_Prof,"");
		//SETCHAR(eMrpViewx.Repmanprof,"");
		SETCHAR(eMrpViewx.Neg_Stocks,"");
		SETCHAR(eMrpViewx.Qm_Rgmts,"");
		SETCHAR(eMrpViewx.Plng_Cycle,"");
		SETCHAR(eMrpViewx.Round_Prof,"");
		SETCHAR(eMrpViewx.Refmatcons,"");
		SETCHAR(eMrpViewx.D_To_Ref_M,"");
		SETCHAR(eMrpViewx.Mult_Ref_M,"");
		SETCHAR(eMrpViewx.Auto_Reset,"");
		SETCHAR(eMrpViewx.Ex_Cert_Id,"");
		SETCHAR(eMrpViewx.Ex_Cert_No_New,"");
		SETCHAR(eMrpViewx.Ex_Cert_Dt,"");
		SETCHAR(eMrpViewx.Milit_Id,"");
		SETCHAR(eMrpViewx.Insp_Int,"");
		SETCHAR(eMrpViewx.Co_Product,"");
		SETCHAR(eMrpViewx.Plan_Strgp,"");
		SETCHAR(eMrpViewx.Sloc_Exprc,"");
		SETCHAR(eMrpViewx.Bulk_Mat,"");
		SETCHAR(eMrpViewx.Cc_Fixed,"");
		SETCHAR(eMrpViewx.Determ_Grp,"");
		SETCHAR(eMrpViewx.Qm_Authgrp,"");
		SETCHAR(eMrpViewx.Task_List_Type,"");
		SETCHAR(eMrpViewx.Pur_Status,"");
		SETCHAR(eMrpViewx.Prodprof,"");
		SETCHAR(eMrpViewx.Safty_T_Id,"");
		SETCHAR(eMrpViewx.Safetytime,"");
		SETCHAR(eMrpViewx.Plord_Ctrl,"");
		SETCHAR(eMrpViewx.Batchentry,"");
		SETCHAR(eMrpViewx.Pvalidfrom,"");
		SETCHAR(eMrpViewx.Matfrgtgrp,"");
		SETCHAR(eMrpViewx.Prodverscs,"");
		SETCHAR(eMrpViewx.Mat_Cfop,"");
		SETCHAR(eMrpViewx.Eu_List_No,"");
		SETCHAR(eMrpViewx.Eu_Mat_Grp,"");
		SETCHAR(eMrpViewx.Cas_No,"");
		SETCHAR(eMrpViewx.Prodcom_No,"");
		SETCHAR(eMrpViewx.Ctrl_Code,"");
		SETCHAR(eMrpViewx.Jit_Relvt,"");
		SETCHAR(eMrpViewx.Mat_Grp_Trans,"");
		SETCHAR(eMrpViewx.Handlg_Grp,"");
		SETCHAR(eMrpViewx.Supply_Area,"");
		SETCHAR(eMrpViewx.Fair_Share_Rule,"");
		SETCHAR(eMrpViewx.Push_Distrib,"");
		SETCHAR(eMrpViewx.Deploy_Horiz,"");
		SETCHAR(eMrpViewx.Min_Lot_Size,"");
		SETCHAR(eMrpViewx.Max_Lot_Size,"");
		SETCHAR(eMrpViewx.Fix_Lot_Size,"");
		SETCHAR(eMrpViewx.Lot_Increment,"");
		SETCHAR(eMrpViewx.Prod_Conv_Type,"");
		SETCHAR(eMrpViewx.Distr_Prof,"");
		SETCHAR(eMrpViewx.Period_Profile_Safety_Time,"");
		SETCHAR(eMrpViewx.Fxd_Price,"");
		SETCHAR(eMrpViewx.Avail_Check_All_Proj_Segments,"");
		SETCHAR(eMrpViewx.Overallprf,"");
		SETCHAR(eMrpViewx.Mrp_Relevancy_Dep_Requirements,"");
		SETCHAR(eMrpViewx.Min_Safety_Stk,"");
		SETCHAR(eMrpViewx.No_Costing,"");
		SETCHAR(eMrpViewx.Unit_Group,"");
		SETCHAR(eMrpViewx.Follow_Up_External,"");
		SETCHAR(eMrpViewx.Follow_Up_Guid,"");
		SETCHAR(eMrpViewx.Follow_Up_Version,"");
		SETCHAR(eMrpViewx.Refmatcons_External,"");
		SETCHAR(eMrpViewx.Refmatcons_Guid,"");
		SETCHAR(eMrpViewx.Refmatcons_Version,"");
		SETCHAR(eMrpViewx.Rotation_Date,"");
		SETCHAR(eMrpViewx.Original_Batch_Flag,"");
		SETCHAR(eMrpViewx.Original_Batch_Ref_Material,"");
		SETCHAR(eMrpViewx.Original_Batch_Ref_Material_E,"");
		SETCHAR(eMrpViewx.Original_Batch_Ref_Material_V,"");
		SETCHAR(eMrpViewx.Original_Batch_Ref_Material_G,"");

		if(tc_strlen(rep_mfg)>0)		//rep_mfg modified on 14.02.2017 for TE02
		{
			SETCHAR(eMrpView.Rep_Manuf,"X");/*rep_mfg_in*/
			SETCHAR(eMrpViewx.Rep_Manuf,"X");/*X*/
			SETCHAR(eMrpView.Repmanprof,rep_mfg);/*rep_mfg*/
			SETCHAR(eMrpViewx.Repmanprof,"X");/*X*/
		}
		else
		{
			SETCHAR(eMrpView.Rep_Manuf,"");/*rep_mfg_in*/
			SETCHAR(eMrpViewx.Rep_Manuf,"X");/*X*/
			SETCHAR(eMrpView.Repmanprof,"");/*rep_mfg*/
			SETCHAR(eMrpViewx.Repmanprof,"X");/*X*/
		}

		SETCHAR(eBapi_Mpgd.Plng_Matl,"");
		SETCHAR(eBapi_Mpgd.Plng_Plant,"");
		SETCHAR(eBapi_Mpgd.Convfactor,"");
		SETCHAR(eBapi_Mpgd.Plng_Matl_External,"");
		SETCHAR(eBapi_Mpgd.Plng_Matl_Guid,"");
		SETCHAR(eBapi_Mpgd.Plng_Matl_Version,"");

		SETCHAR(eBapi_Mpgdx.Plng_Matl,"");
		SETCHAR(eBapi_Mpgdx.Plng_Plant,"");
		SETCHAR(eBapi_Mpgdx.Convfactor,"");
		SETCHAR(eBapi_Mpgdx.Plng_Matl_External,"");
		SETCHAR(eBapi_Mpgdx.Plng_Matl_Guid,"");
		SETCHAR(eBapi_Mpgdx.Plng_Matl_Version,"");

		SETCHAR(eBapi_Mard.Stge_Loc,"9005");
		SETCHAR(eBapi_Mard.Del_Flag,"");
		SETCHAR(eBapi_Mard.Mrp_Ind,"");
		SETCHAR(eBapi_Mard.Spec_Proc,"");
		SETBCD(eBapi_Mard.Reorder_Pt,myzeroDup3,3);
		SETBCD(eBapi_Mard.Repl_Qty,myzeroDup3,3);
		SETCHAR(eBapi_Mard.Stge_Bin,"");
		SETCHAR(eBapi_Mard.Pickg_Area,"");
		SETFLOAT(&eBapi_Mard.Inv_Corr_Fac,"");

		SETCHAR(eBapi_Mardx.Stge_Loc,"9005");
		SETCHAR(eBapi_Mardx.Del_Flag,"");
		SETCHAR(eBapi_Mardx.Mrp_Ind,"");
		SETCHAR(eBapi_Mardx.Spec_Proc,"");
		SETCHAR(eBapi_Mardx.Reorder_Pt,"");
		SETCHAR(eBapi_Mardx.Repl_Qty,"");
		SETCHAR(eBapi_Mardx.Stge_Bin,"");
		SETCHAR(eBapi_Mardx.Pickg_Area,"");
		SETCHAR(eBapi_Mardx.Inv_Corr_Fac,"");

		SETCHAR(eAccView.Val_Type,"");
		SETCHAR(eAccView.Del_Flag,"");
		SETCHAR(eAccView.Price_Ctrl,"");
		SETBCD(eAccView.Moving_Pr,myzeroDup4,4);
		SETBCD(eAccView.Std_Price,myzeroDup4,4);
		SETBCD(eAccView.Price_Unit,"",0);
		SETCHAR(eAccView.Pr_Ctrl_Pp,"");
		SETBCD(eAccView.Mov_Pr_Pp,myzeroDup4,4);
		SETBCD(eAccView.Std_Pr_Pp,myzeroDup4,4);
		SETBCD(eAccView.Pr_Unit_Pp,"0",1);
		SETCHAR(eAccView.Vclass_Pp,"");
		SETCHAR(eAccView.Pr_Ctrl_Py,"");
		SETBCD(eAccView.Mov_Pr_Py,myzeroDup4,4);
		SETBCD(eAccView.Std_Pr_Py,myzeroDup4,4);
		SETCHAR(eAccView.Vclass_Py,"");
		SETBCD(eAccView.Pr_Unit_Py,"0",0);
		SETCHAR(eAccView.Val_Cat,"");
		SETBCD(eAccView.Future_Pr,myzeroDup4,4);
		SETDATE(eAccView.Valid_From,"");
		SETBCD(eAccView.Taxprice_1,myzeroDup4,4);
		SETBCD(eAccView.Commprice1,myzeroDup4,4);
		SETBCD(eAccView.Taxprice_3,myzeroDup4,4);
		SETBCD(eAccView.Commprice3,myzeroDup4,4);
		SETBCD(eAccView.Plnd_Price,myzeroDup4,4);
		SETBCD(eAccView.Plndprice1,myzeroDup4,4);
		SETBCD(eAccView.Plndprice2,myzeroDup4,4);
		SETBCD(eAccView.Plndprice3,myzeroDup4,4);
		SETDATE(eAccView.Plndprdate1,"");
		SETDATE(eAccView.Plndprdate2,"");
		SETDATE(eAccView.Plndprdate3,"");
		SETCHAR(eAccView.Lifo_Fifo,"");
		SETCHAR(eAccView.Poolnumber,"");
		SETBCD(eAccView.Taxprice_2,myzeroDup4,4);
		SETBCD(eAccView.Commprice2,myzeroDup4,4);
		SETNUM(eAccView.Deval_Ind,"00");
		SETCHAR(eAccView.Qty_Struct,"");
		SETCHAR(eAccView.Ml_Active,"");
		SETCHAR(eAccView.Ml_Settle,"");
		SETCHAR(eAccView.Orig_Mat,"X");
		SETCHAR(eAccView.Vm_So_Stk,"");
		SETCHAR(eAccView.Vm_P_Stock,"");
		SETCHAR(eAccView.Matl_Usage,"");
		SETCHAR(eAccView.Mat_Origin,"");
		SETCHAR(eAccView.In_House,"");
		SETBCD(eAccView.Tax_Cml_Un,"0",0);

		SETCHAR(eAccViewx.Val_Type,"");
		SETCHAR(eAccViewx.Del_Flag,"");
		SETCHAR(eAccViewx.Price_Ctrl,"");
		SETCHAR(eAccViewx.Moving_Pr,"");
		SETCHAR(eAccViewx.Std_Price,"");
		SETCHAR(eAccViewx.Price_Unit,"");
		SETCHAR(eAccViewx.Pr_Ctrl_Pp,"");
		SETCHAR(eAccViewx.Mov_Pr_Pp,"");
		SETCHAR(eAccViewx.Std_Pr_Pp,"");
		SETCHAR(eAccViewx.Pr_Unit_Pp,"");
		SETCHAR(eAccViewx.Vclass_Pp,"");
		SETCHAR(eAccViewx.Pr_Ctrl_Py,"");
		SETCHAR(eAccViewx.Mov_Pr_Py,"");
		SETCHAR(eAccViewx.Std_Pr_Py,"");
		SETCHAR(eAccViewx.Vclass_Py,"");
		SETCHAR(eAccViewx.Pr_Unit_Py,"");
		SETCHAR(eAccViewx.Val_Cat,"");
		SETCHAR(eAccViewx.Future_Pr,"");
		SETCHAR(eAccViewx.Valid_From,"");
		SETCHAR(eAccViewx.Taxprice_1,"");
		SETCHAR(eAccViewx.Commprice1,"");
		SETCHAR(eAccViewx.Taxprice_3,"");
		SETCHAR(eAccViewx.Commprice3,"");
		SETCHAR(eAccViewx.Plnd_Price,"");
		SETCHAR(eAccViewx.Plndprice1,"");
		SETCHAR(eAccViewx.Plndprice2,"");
		SETCHAR(eAccViewx.Plndprice3,"");
		SETCHAR(eAccViewx.Plndprdate1,"");
		SETCHAR(eAccViewx.Plndprdate2,"");
		SETCHAR(eAccViewx.Plndprdate3,"");
		SETCHAR(eAccViewx.Lifo_Fifo,"");
		SETCHAR(eAccViewx.Poolnumber,"");
		SETCHAR(eAccViewx.Taxprice_2,"");
		SETCHAR(eAccViewx.Commprice2,"");
		SETCHAR(eAccViewx.Deval_Ind,"");
		SETCHAR(eAccViewx.Qty_Struct,"");
		SETCHAR(eAccViewx.Ml_Active,"");
		SETCHAR(eAccViewx.Ml_Settle,"");
		SETCHAR(eAccViewx.Orig_Mat,"X");
		SETCHAR(eAccViewx.Vm_So_Stk,"");
		SETCHAR(eAccViewx.Vm_P_Stock,"");
		SETCHAR(eAccViewx.Matl_Usage,"");
		SETCHAR(eAccViewx.Mat_Origin,"");
		SETCHAR(eAccViewx.In_House,"");
		SETCHAR(eAccViewx.Tax_Cml_Un,"");

		SETCHAR(tBapi_Makt->Langu,"EN");
		SETCHAR(tBapi_Makt->Langu_Iso,"EN");
		SETCHAR(tBapi_Makt->Del_Flag,"");

		SETBCD(tBapi_Marm->Numerator,"1",0);
		SETBCD(tBapi_Marm->Denominatr,"1",0);
		SETCHAR(tBapi_Marm->Ean_Upc,"");
		SETCHAR(tBapi_Marm->Ean_Cat,"");
		SETCHAR(tBapi_Marm->Length,"");
		SETCHAR(tBapi_Marm->Width,"");
		SETCHAR(tBapi_Marm->Height,"");
		SETCHAR(tBapi_Marm->Unit_Dim,"");
		SETCHAR(tBapi_Marm->Unit_Dim_Iso,"");
		SETBCD(tBapi_Marm->Volume,myzeroDup3,3);
		SETCHAR(tBapi_Marm->VolumeUnit,"");
		SETCHAR(tBapi_Marm->VolumeUnit_Iso,"");
		SETBCD(tBapi_Marm->Gross_Wt,gross_wt,3);
		SETCHAR(tBapi_Marm->Unit_Of_Wt,"");
		SETCHAR(tBapi_Marm->Unit_Of_Wt_Iso,"");
		SETCHAR(tBapi_Marm->Del_Flag,"");
		SETCHAR(tBapi_Marm->Sub_Uom,"");
		SETCHAR(tBapi_Marm->Sub_Uom_Iso,"");
		SETCHAR(tBapi_Marm->Gtin_Variant,"");

		SETCHAR(tBapi_Marmx->Numerator,"X");
		SETCHAR(tBapi_Marmx->Denominatr,"X");
		SETCHAR(tBapi_Marmx->Ean_Upc,"");
		SETCHAR(tBapi_Marmx->Ean_Cat,"");
		SETCHAR(tBapi_Marmx->Length,"");
		SETCHAR(tBapi_Marmx->Width,"");
		SETCHAR(tBapi_Marmx->Height,"");
		SETCHAR(tBapi_Marmx->Unit_Dim,"");
		SETCHAR(tBapi_Marmx->Unit_Dim_Iso,"");
		SETCHAR(tBapi_Marmx->Volume,"");
		SETCHAR(tBapi_Marmx->VolumeUnit,"");
		SETCHAR(tBapi_Marmx->VolumeUnit_Iso,"");
		SETCHAR(tBapi_Marmx->Gross_Wt,"");
		SETCHAR(tBapi_Marmx->Unit_Of_Wt,"");
		SETCHAR(tBapi_Marmx->Unit_Of_Wt_Iso,"");
		SETCHAR(tBapi_Marmx->Sub_Uom,"");
		SETCHAR(tBapi_Marmx->Sub_Uom_Iso,"");
		SETCHAR(tBapi_Marmx->Gtin_Variant,"");
		if (tc_strcmp(mat_type,"FERT")==0)
		{
			SETCHAR(eMrpView.Sm_Key,"000");//schedk
			SETCHAR(eMrpViewx.Sm_Key,"");
			SETCHAR(eMrpView.Ctrl_Key,control_key);
			SETCHAR(eMrpViewx.Ctrl_Key,"");
			SETCHAR(eMrpView.Cert_Type,certificate_type);
			SETCHAR(eMrpViewx.Cert_Type,"");
		}
		else
		{
			SETCHAR(eMrpView.Sm_Key,"000");//schedk
			SETCHAR(eMrpViewx.Sm_Key,"X");
			SETCHAR(eMrpView.Ctrl_Key,control_key);
			SETCHAR(eMrpViewx.Ctrl_Key,"X");
			SETCHAR(eMrpView.Cert_Type,certificate_type);
			SETCHAR(eMrpViewx.Cert_Type,"X");
		}

		SETCHAR(tBAPIE1PAREX3->STRUCTURE,"");//BAPI_TE_MARA
		SETCHAR(tBAPIE1PAREX3->VALUEPART1,"");//part_noDupDes
		SETCHAR(tBAPIE1PAREX3->VALUEPART2,"");
		SETCHAR(tBAPIE1PAREX3->VALUEPART3,"");
		SETCHAR(tBAPIE1PAREX3->VALUEPART4,"");

		SETCHAR(tBAPIE1PAREXX3->STRUCTURE,"");//BAPI_TE_MARAX
		SETCHAR(tBAPIE1PAREXX3->VALUEPART1,"");//part_noDupDesx
		SETCHAR(tBAPIE1PAREXX3->VALUEPART2,"");
		SETCHAR(tBAPIE1PAREXX3->VALUEPART3,"");
		SETCHAR(tBAPIE1PAREXX3->VALUEPART4,"");

		RfcRc = zbapi_material_savedata_mrp(hRfc,&eBapiMatHead,&eBasicView,&eBasicViewx,&eMrpView,&eMrpViewx,&eBapi_Mpgd,&eBapi_Mpgdx,&eBapi_Mard,&eBapi_Mardx,&eAccView,&eAccViewx,&eRmmg1_Aennr,&eBapiret2,thBapi_Makt,thBapi_Marm,thBapi_Marmx,thBAPIE1PAREX3,thBAPIE1PAREXX3,xException);

		switch (RfcRc)
		{
			case RFC_OK:
				//sprintf(sap_msg,"%.*s:",sizeof(sap_msg),eBapiret2.Message);
				//GETCHAR(eBapiret2.Message,sap_msg);
				printf("\nMessage for Material %s change: %s",part_noDup,eBapiret2.Message); fflush(stdout);
				fprintf(fsuccess,"\nMessage for Material %s change: %s",part_noDup,eBapiret2.Message); fflush(fsuccess);
				break;
			case RFC_EXCEPTION:
				printf("\nRFC EXCEPTION: %s",xException); fflush(stdout);
				break;
			case RFC_SYS_EXCEPTION:
				printf("\nSYSTEM EXCEPTION RAISED!!!"); fflush(stdout);
				break;
			case RFC_FAILURE:
				printf("\nFAILURE!!!"); fflush(stdout);
				break;
			default:
				printf("\nOTHER FAILURE!"); fflush(stdout);
				break;
		}

		if(ItDelete(thBapi_Makt) != 0)	rfc_error("ItDelete thBapi_Makt");
		if(ItDelete(thBapi_Marm) != 0)	rfc_error("ItDelete tBapi_Marm");
		if(ItDelete(thBapi_Marmx) != 0) rfc_error("ItDelete thBapi_Marmx");
		if(ItDelete(thBAPIE1PAREX3) != 0) rfc_error("ItDelete thBAPIE1PAREX3");
		if(ItDelete(thBAPIE1PAREXX3) != 0) rfc_error("ItDelete thBAPIE1PAREXX3");
		//sleep(5);
		RfcClose(hRfc);
	}
	/*finished setting values for mat create and change*/
}
RFC_RC zbapi_material_savedata_mrp(RFC_HANDLE hRfc,BAPIMATHEAD *eBapiMatHead,BAPI_MARA	*eBasicView,BAPI_MARAX	*eBasicViewx,BAPI_MARC	*eMrpView,BAPI_MARCX	*eMrpViewx,BAPI_MPGD	*eBapi_Mpgd,BAPI_MPGDX	*eBapi_Mpgdx,BAPI_MARD	*eBapi_Mard,BAPI_MARDX	*eBapi_Mardx,BAPI_MBEW	*eAccView,BAPI_MBEWX	*eAccViewx,RMMG1_AENNR *eRmmg1_Aennr,BAPIRET2 *eBapiret2,ITAB_H thBapi_Makt,ITAB_H thBapi_Marm,ITAB_H thBapi_Marmx,ITAB_H thBAPIE1PAREX3,ITAB_H thBAPIE1PAREXX3,char *xException)
{
	RFC_PARAMETER Exporting[13];
	RFC_PARAMETER Importing[2];
	RFC_TABLE Tables[4];
	RFC_RC RfcRc;
	char *RfcException = NULL;

	Exporting[0].name = "HEADDATA";
	Exporting[0].nlen = 8;
	Exporting[0].type = handleOfBAPIMATHEAD;
	Exporting[0].leng = sizeof(BAPIMATHEAD);
	Exporting[0].addr = eBapiMatHead;

	Exporting[1].name = "CLIENTDATA";
	Exporting[1].nlen = 10;
	Exporting[1].type = handleOfBAPI_MARA;
	Exporting[1].leng = sizeof(BAPI_MARA);
	Exporting[1].addr = eBasicView;

	Exporting[2].name = "CLIENTDATAX";
	Exporting[2].nlen = 11;
	Exporting[2].type = handleOfBAPI_MARAX;
	Exporting[2].leng = sizeof(BAPI_MARAX);
	Exporting[2].addr = eBasicViewx;

	Exporting[3].name = "PLANTDATA";
	Exporting[3].nlen = 9;
	Exporting[3].type = handleOfBAPI_MARC;
	Exporting[3].leng = sizeof(BAPI_MARC);
	Exporting[3].addr = eMrpView;

	Exporting[4].name = "PLANTDATAX";
	Exporting[4].nlen = 10;
	Exporting[4].type = handleOfBAPI_MARCX;
	Exporting[4].leng = sizeof(BAPI_MARCX);
	Exporting[4].addr = eMrpViewx;

	Exporting[5].name = "PLANNINGDATA";
	Exporting[5].nlen = 12;
	Exporting[5].type = handleOfBAPI_MPGD;
	Exporting[5].leng = sizeof(BAPI_MPGD);
	Exporting[5].addr = eBapi_Mpgd;

	Exporting[6].name = "PLANNINGDATAX";
	Exporting[6].nlen = 13;
	Exporting[6].type = handleOfBAPI_MPGDX;
	Exporting[6].leng = sizeof(BAPI_MPGDX);
	Exporting[6].addr = eBapi_Mpgdx;

	Exporting[7].name = "STORAGELOCATIONDATA";
	Exporting[7].nlen = 19;
	Exporting[7].type = handleOfBAPI_MARD;
	Exporting[7].leng = sizeof(BAPI_MARD);
	Exporting[7].addr = eBapi_Mard;

	Exporting[8].name = "STORAGELOCATIONDATAX";
	Exporting[8].nlen = 20;
	Exporting[8].type = handleOfBAPI_MARDX;
	Exporting[8].leng = sizeof(BAPI_MARDX);
	Exporting[8].addr = eBapi_Mardx;

	Exporting[9].name = "VALUATIONDATA";
	Exporting[9].nlen = 13;
	Exporting[9].type = handleOfBAPI_MBEW;
	Exporting[9].leng = sizeof(BAPI_MBEW);
	Exporting[9].addr = eAccView;

	Exporting[10].name = "VALUATIONDATAX";
	Exporting[10].nlen = 14;
	Exporting[10].type = handleOfBAPI_MBEWX;
	Exporting[10].leng = sizeof(BAPI_MBEWX);
	Exporting[10].addr = eAccViewx;

	Exporting[11].name = "CHANGE_NUMBER";
	Exporting[11].nlen = 13;
	Exporting[11].type = handleOfRMMG1_AENNR;
	Exporting[11].leng = sizeof(RMMG1_AENNR);
	Exporting[11].addr = eRmmg1_Aennr;

	Exporting[12].name = NULL;


	Tables[0].name     = "MATERIALDESCRIPTION";
	Tables[0].nlen     = 19;
	Tables[0].type     = handleOfBAPI_MAKT;
	Tables[0].ithandle = thBapi_Makt;

	Tables[1].name     = "UNITSOFMEASURE";
	Tables[1].nlen     = 14;
	Tables[1].type     = handleOfBAPI_MARM;
	Tables[1].ithandle = thBapi_Marm;

	Tables[2].name     = "UNITSOFMEASUREX";
	Tables[2].nlen     = 15;
	Tables[2].type     = handleOfBAPI_MARMX;
	Tables[2].ithandle = thBapi_Marmx;

	/*Tables[3].name     = "EXTENSIONIN";
	Tables[3].nlen     = 11;
	Tables[3].type     = handleOfBAPIE1PAREX3;
	Tables[3].ithandle = thBAPIE1PAREX3;

	Tables[4].name     = "EXTENSIONINX";
	Tables[4].nlen     = 12;
	Tables[4].type     = handleOfBAPIE1PAREXX3;
	Tables[4].ithandle = thBAPIE1PAREXX3;*/

	Tables[3].name = NULL;

	RfcRc = RfcCall(hRfc,"ZBAPI_MATERIAL_SAVEDATA",Exporting,Tables);

	switch (RfcRc)
	{
		case RFC_OK:
			Importing[0].name = "RETURN";
			Importing[0].nlen = 6;
			Importing[0].type = TYPC;
			Importing[0].leng = sizeof(BAPIRET2);
			Importing[0].addr = eBapiret2;

			Importing[1].name = NULL;

			RfcRc = RfcReceive(hRfc,Importing,Tables,&RfcException);
			switch (RfcRc)
			{
				case RFC_SYS_EXCEPTION:
					strcpy(xException,RfcException);
				break;
				case RFC_EXCEPTION:
					strcpy(xException,RfcException);
				break;
				default:;
			}
			break;
		default:
			printf("\nNOT RFC OK"); fflush(stdout);
		break;
	}
	return RfcRc;
}
RFC_RC zbapi_material_savedata_mrpCreate(RFC_HANDLE hRfc,BAPIMATHEAD *eBapiMatHead,BAPI_MARA	*eBasicView,BAPI_MARAX	*eBasicViewx,BAPI_MARC	*eMrpView,BAPI_MARCX	*eMrpViewx,BAPI_MPGD	*eBapi_Mpgd,BAPI_MPGDX	*eBapi_Mpgdx,BAPI_MARD	*eBapi_Mard,BAPI_MARDX	*eBapi_Mardx,BAPI_MBEW	*eAccView,BAPI_MBEWX	*eAccViewx,RMMG1_AENNR *eRmmg1_Aennr,BAPIRET2 *eBapiret2,ITAB_H thBapi_Makt,ITAB_H thBapi_Marm,ITAB_H thBapi_Marmx,ITAB_H thBAPIE1PAREX3,ITAB_H thBAPIE1PAREXX3,char *xException)
{
	RFC_PARAMETER Exporting[13];
	RFC_PARAMETER Importing[2];
	RFC_TABLE Tables[6];
	RFC_RC RfcRc;
	char *RfcException = NULL;

	Exporting[0].name = "HEADDATA";
	Exporting[0].nlen = 8;
	Exporting[0].type = handleOfBAPIMATHEAD;
	Exporting[0].leng = sizeof(BAPIMATHEAD);
	Exporting[0].addr = eBapiMatHead;

	Exporting[1].name = "CLIENTDATA";
	Exporting[1].nlen = 10;
	Exporting[1].type = handleOfBAPI_MARA;
	Exporting[1].leng = sizeof(BAPI_MARA);
	Exporting[1].addr = eBasicView;

	Exporting[2].name = "CLIENTDATAX";
	Exporting[2].nlen = 11;
	Exporting[2].type = handleOfBAPI_MARAX;
	Exporting[2].leng = sizeof(BAPI_MARAX);
	Exporting[2].addr = eBasicViewx;

	Exporting[3].name = "PLANTDATA";
	Exporting[3].nlen = 9;
	Exporting[3].type = handleOfBAPI_MARC;
	Exporting[3].leng = sizeof(BAPI_MARC);
	Exporting[3].addr = eMrpView;

	Exporting[4].name = "PLANTDATAX";
	Exporting[4].nlen = 10;
	Exporting[4].type = handleOfBAPI_MARCX;
	Exporting[4].leng = sizeof(BAPI_MARCX);
	Exporting[4].addr = eMrpViewx;

	Exporting[5].name = "PLANNINGDATA";
	Exporting[5].nlen = 12;
	Exporting[5].type = handleOfBAPI_MPGD;
	Exporting[5].leng = sizeof(BAPI_MPGD);
	Exporting[5].addr = eBapi_Mpgd;

	Exporting[6].name = "PLANNINGDATAX";
	Exporting[6].nlen = 13;
	Exporting[6].type = handleOfBAPI_MPGDX;
	Exporting[6].leng = sizeof(BAPI_MPGDX);
	Exporting[6].addr = eBapi_Mpgdx;

	Exporting[7].name = "STORAGELOCATIONDATA";
	Exporting[7].nlen = 19;
	Exporting[7].type = handleOfBAPI_MARD;
	Exporting[7].leng = sizeof(BAPI_MARD);
	Exporting[7].addr = eBapi_Mard;

	Exporting[8].name = "STORAGELOCATIONDATAX";
	Exporting[8].nlen = 20;
	Exporting[8].type = handleOfBAPI_MARDX;
	Exporting[8].leng = sizeof(BAPI_MARDX);
	Exporting[8].addr = eBapi_Mardx;

	Exporting[9].name = "VALUATIONDATA";
	Exporting[9].nlen = 13;
	Exporting[9].type = handleOfBAPI_MBEW;
	Exporting[9].leng = sizeof(BAPI_MBEW);
	Exporting[9].addr = eAccView;

	Exporting[10].name = "VALUATIONDATAX";
	Exporting[10].nlen = 14;
	Exporting[10].type = handleOfBAPI_MBEWX;
	Exporting[10].leng = sizeof(BAPI_MBEWX);
	Exporting[10].addr = eAccViewx;

	Exporting[11].name = "CHANGE_NUMBER";
	Exporting[11].nlen = 13;
	Exporting[11].type = handleOfRMMG1_AENNR;
	Exporting[11].leng = sizeof(RMMG1_AENNR);
	Exporting[11].addr = eRmmg1_Aennr;

	Exporting[12].name = NULL;


	Tables[0].name     = "MATERIALDESCRIPTION";
	Tables[0].nlen     = 19;
	Tables[0].type     = handleOfBAPI_MAKT;
	Tables[0].ithandle = thBapi_Makt;

	Tables[1].name     = "UNITSOFMEASURE";
	Tables[1].nlen     = 14;
	Tables[1].type     = handleOfBAPI_MARM;
	Tables[1].ithandle = thBapi_Marm;

	Tables[2].name     = "UNITSOFMEASUREX";
	Tables[2].nlen     = 15;
	Tables[2].type     = handleOfBAPI_MARMX;
	Tables[2].ithandle = thBapi_Marmx;

	Tables[3].name     = "EXTENSIONIN";
	Tables[3].nlen     = 11;
	Tables[3].type     = handleOfBAPIE1PAREX3;
	Tables[3].ithandle = thBAPIE1PAREX3;

	Tables[4].name     = "EXTENSIONINX";
	Tables[4].nlen     = 12;
	Tables[4].type     = handleOfBAPIE1PAREXX3;
	Tables[4].ithandle = thBAPIE1PAREXX3;

	Tables[5].name = NULL;

	RfcRc = RfcCall(hRfc,"ZBAPI_MATERIAL_SAVEDATA",Exporting,Tables);

	switch (RfcRc)
	{
		case RFC_OK:
			Importing[0].name = "RETURN";
			Importing[0].nlen = 6;
			Importing[0].type = TYPC;
			Importing[0].leng = sizeof(BAPIRET2);
			Importing[0].addr = eBapiret2;

			Importing[1].name = NULL;

			RfcRc = RfcReceive(hRfc,Importing,Tables,&RfcException);
			switch (RfcRc)
			{
				case RFC_SYS_EXCEPTION:
					strcpy(xException,RfcException);
				break;
				case RFC_EXCEPTION:
					strcpy(xException,RfcException);
				break;
				default:;
			}
			break;
		default:
			printf("\nNOT RFC OK"); fflush(stdout);
		break;
	}
	return RfcRc;
}

RFC_RC allocate_insp_type_vc(void)
{
	static RFC_HANDLE hRfc;
	static RFC_RC RfcRc;

	hRfc = BapiLogon();
	printf("\nReturned value = %u", hRfc);
	if (hRfc == RFC_HANDLE_NULL)
		printf("\nRfcOpen");
	if (hRfc == 0)
	{
		printf("\nRFC connection is not present.");
	}
	else
		RfcRc = vc_zrfc_insptype_alloc(hRfc);
	fprintf(fsuccess,"\n******************************************************************");
	RfcClose(hRfc);
	printf("\nRFC connection is closed");
	return RfcRc;
}
RFC_RC  cll_zrfc_insptype_alloc(RFC_HANDLE hRfc)
{
	int count = 0;
	static RFC_RC RfcRc;
	static QPART eQpart;
	MESSAGEINF iMessg;
	SYST_SUBRC iRetval;
	char xException[256];
	char xMessage[80]={0};
	WERKS_D eWerks;
	static  MATNR eMatnr;


	SETCHAR(eMatnr.Matnr,"");
	SETCHAR(eQpart.Qpart,"");
	SETCHAR(eWerks.WerksD,"");
	SETCHAR(eMatnr.Matnr,part_noDup);

	printf("\nInspection alloc %s %s",part_noDup,plantcode); fflush(stdout);

	SETCHAR(eWerks.WerksD,plantcode);

	for (count = 0; count < 4; count++)
	{
		SETCHAR(eQpart.Qpart,"");
		if (count == 0) SETCHAR(eQpart.Qpart,"01");
		if (count == 1) SETCHAR(eQpart.Qpart,"0101");
		if (count == 2) SETCHAR(eQpart.Qpart,"0102");
		if (count == 3) SETCHAR(eQpart.Qpart,"8901");

		RfcRc = zrfc_insptype_alloc(hRfc,&eMatnr,&eQpart,&eWerks,&iMessg,&iRetval,xException);

		switch (RfcRc)
		{
			case RFC_OK:
				strcpy(xMessage,"");
				tc_strncpy(xMessage,iMessg.Msgtx,60);
				xMessage[61]='\0';
				printf("\ninsptype_alloc[%s,%s] :[%s]",part_noDup,plantcode,xMessage); fflush(stdout);
				fprintf(fsuccess,"\ninsptype_alloc[%s,%s] :[%s]",part_noDup,plantcode,xMessage); fflush(fsuccess);

				if(tc_strstr(xMessage,"batch input data for screen SAPLMGMM 4000")!=NULL)
				{
				      fprintf(finsp,"%s,%s\n",part_noDup,plantcode);
                }
			break;
			case RFC_EXCEPTION:
				printf("\nRFC Exception : %s",xException); fflush(stdout);
			break;
			case RFC_SYS_EXCEPTION:
				printf("\nSystem Exception Raised!!!"); fflush(stdout);
			break;
			case RFC_FAILURE:
				printf("\nFailure!!!"); fflush(stdout);
			break;
			default:
				printf("\nOther Failure!"); fflush(stdout);
		}
		sleep(2);
	}
	return RfcRc;
}
RFC_RC  vc_zrfc_insptype_alloc(RFC_HANDLE hRfc)
{
	static RFC_RC RfcRc;
	static QPART eQpart;
	MESSAGEINF iMessg;
	SYST_SUBRC iRetval;
	char xException[256];
	WERKS_D eWerks;
	static  MATNR eMatnr;
	printf("\nInspection Alloc 08 for VC QM View %s %s",part_noDup,plantcode);
	fprintf(fsuccess,"\nInspection Alloc 08 for VC QM View %s %s",part_noDup,plantcode);

	/*SETCHAR(eMatnr.Matnr,"");
	SETCHAR(eQpart.Qpart,"");
	SETCHAR(eWerks.WerksD,"");*/
	
	SETCHAR(eMatnr.Matnr,part_noDup);/*rfc_partno*/
	SETCHAR(eWerks.WerksD,plantcode);
	SETCHAR(eQpart.Qpart,"08");

	RfcRc = zrfc_insptype_alloc(hRfc,&eMatnr,&eQpart,&eWerks,&iMessg,&iRetval,xException);

	switch (RfcRc)
	{
		case RFC_OK:
			/*printf("\nReturned value from zrfc_insptype_alloc = %u",RFC_OK);*/
			printf("\ninsptype_alloc = %s",iMessg.Msgty);
			fprintf(fsuccess,"\ninsptype_alloc = %s",iMessg.Msgty);
			/*printf("\nMsgid = %s",iMessg.Msgid);
			printf("\nMsgno = %s",iMessg.Msgno);
			printf("\nMsspc = %s",iMessg.Msspc);
			printf("\nMsgtx = %s",iMessg.Msgtx);*/

			break;
		case RFC_EXCEPTION:
			printf("\nRFC Exception = %s",xException);
			break;
		case RFC_SYS_EXCEPTION:
			printf("\nSystem Exception Raised!!!");
			break;
		case RFC_FAILURE:
			printf("\nFailure!!!");
			break;
		default:
			printf("\nOther Failure!");
	}
	return RfcRc;
}
void cll_zrfc_ac_view_create()
{
	int count = 1;
	int call_function = 0;
	char xException[256] = {0};
	char xMessage[80] = {0};
	static RFC_HANDLE hRfc;
	static RFC_RC RfcRc;
	WERKS_D eWerks;
	static  MATNR eMatnr;

	BKLAS eBklas;
	BWTAR_D eBwtar;
	MESSAGEINF iMessg;
	SYST_SUBRC iRetval;

	hRfc = BapiLogon();

	SETCHAR(eMatnr.Matnr,"");
	SETCHAR(eWerks.WerksD,"");
	SETCHAR(eMatnr.Matnr,part_noDup);
	SETCHAR(eWerks.WerksD,plantcode);

	printf("\nBatches Create:%s %s",part_noDup,plantcode); fflush(stdout);
	for (count = 1; count < 6; count++)
	{
		call_function = 0;
		if (count == 1)
		{
			SETCHAR(eBklas.Bklas,"");
			SETCHAR(eBwtar.BwtarD,"");
			SETCHAR(eBklas.Bklas,"0110");
			SETCHAR(eBwtar.BwtarD,"1");
			call_function = 1;
		}
		if (count == 2)
		{
			SETCHAR(eBklas.Bklas,"");
			SETCHAR(eBwtar.BwtarD,"");
			SETCHAR(eBklas.Bklas,"0120");
			SETCHAR(eBwtar.BwtarD,"2");
			call_function = 1;
		}
		if (count == 3)
		{
			SETCHAR(eBklas.Bklas,"");
			SETCHAR(eBwtar.BwtarD,"");
			SETCHAR(eBklas.Bklas,"0230");
			SETCHAR(eBwtar.BwtarD,"3");
			call_function = 1;
		}
		if (count == 4)
		{
			SETCHAR(eBklas.Bklas,"");
			SETCHAR(eBwtar.BwtarD,"");
			SETCHAR(eBklas.Bklas,"0240");
			SETCHAR(eBwtar.BwtarD,"4");
			call_function = 1;
		}
		if (count == 5)
		{
			SETCHAR(eBklas.Bklas,"");
			SETCHAR(eBwtar.BwtarD,"");
			SETCHAR(eBklas.Bklas,"0250");
			SETCHAR(eBwtar.BwtarD,"5");
			call_function = 1;
		}
		if (call_function == 1)
		{
			RfcRc = zrfc_ac_view_create(hRfc,&eBklas,&eBwtar,&eMatnr,&eWerks,&iMessg,&iRetval,xException);
			switch (RfcRc)
			{
				case RFC_OK:
					strcpy(xMessage,"");
					strncpy(xMessage,iMessg.Msgtx,78);
					xMessage[79] = '\0';
					printf("\nMessage for %s,%s Batches Create : [%s]",part_noDup,plantcode,xMessage); fflush(stdout);
					fprintf(fsuccess,"\nMessage for %s,%s Batches Create : [%s]",part_noDup,plantcode,xMessage); fflush(fsuccess);

					/*if((!nlsIsStrNull(nlsStrStr(xMessage,"batch input data for screen SAPLMGMM"))) || (!nlsIsStrNull(nlsStrStr(xMessage,"the account is locked"))) || (!nlsIsStrNull(nlsStrStr(xMessage,"locked by DENIS"))))
					{
					     fprintf(fval,"%s,%s\n",part_noDup,plantcode); fflush(fval);
					}*/
					sleep(2);
					break;
				case RFC_EXCEPTION:
					printf("\nRFC Exception %s",xException); fflush(stdout);
					break;
				case RFC_SYS_EXCEPTION:
					printf("\nSYSTEM EXCEPTION RAISED"); fflush(stdout);
					break;
				case RFC_FAILURE:
					printf("\nFAILURE"); fflush(stdout);
					break;
				default:
					printf("\nOTHER FAILURE"); fflush(stdout);
					break;
			}
			sleep(2);
		}
	}
	RfcClose(hRfc);
}
int BapiRevcr(char sap_dml_no[13],char sap_part_no[15],char sap_rev_sheet_status[3])
{
	static RFC_HANDLE hRfc;
	static RFC_RC RfcRc;
	AENNR eAennr;
	CCMATNR eMatnr;
	CC_REVLV eRevlv;

	MESSAGEINF iMessg;
	SYST_SUBRC iRetval;
	char xException[256];



	hRfc = BapiLogon();

	printf("\nValues received: \nDml No.: %s\nPart No.: %s\nRev_Sheet_status: %s",sap_dml_no,sap_part_no,sap_rev_sheet_status); fflush(stdout);
	fprintf(fsuccess,"\nValues received: \nDml No.: %s\nPart No.: %s\nRev_Sheet_status: %s",sap_dml_no,sap_part_no,sap_rev_sheet_status); fflush(fsuccess);

	SETCHAR(eAennr.Aennr,sap_dml_no);
	SETCHAR(eMatnr.Ccmatnr,sap_part_no);
	SETCHAR(eRevlv.CcRevlv,sap_rev_sheet_status);


	RfcRc = zpprfc_revisionnum_change(hRfc,&eAennr,&eMatnr,&eRevlv,&iMessg,&iRetval,xException);
	switch (RfcRc)
	{
		case RFC_OK:
			printf("\nRevision Creation Message :%s",iMessg.Msgtx); fflush(stdout);
			fprintf(fsuccess,"\nRevision Creation Message :%s",iMessg.Msgtx); fflush(fsuccess);
			break;
		case RFC_EXCEPTION:
			printf("\nRFC_EXCEPTION raised"); fflush(stdout);
			fprintf(fsuccess,"\nRFC_EXCEPTION raised"); fflush(fsuccess);
			break;
		case RFC_SYS_EXCEPTION:
			printf("\nsystem exception raised"); fflush(stdout);
			fprintf(fsuccess,"\nsystem exception raised"); fflush(fsuccess);
			break;
		case RFC_FAILURE:
			printf("\nfailure"); fflush(stdout);
			break;
		default:
			printf("\nother failure"); fflush(stdout);
	}

	RfcClose(hRfc);
	return 0;
}
BapiLogon()
{
	static RFC_OPTIONS RfcOptions;

	static RFC_CONNOPT_R3ONLY RfcConnoptR3only;
	static RFC_ENV RfcEnv;
	static RFC_HANDLE hRfc;

	/*RfcOptions.destination ="PP8";
	RfcOptions.client = "500";
	RfcOptions.user = "DENIS";
	RfcOptions.language = "EN";
	RfcOptions.password = "CARINIT1";
	RfcOptions.trace = 0;
	RfcConnoptR3only.hostname="p690";
	RfcConnoptR3only.gateway_host="p690";
	RfcConnoptR3only.gateway_service="sapgw00";
	RfcOptions.connopt = &RfcConnoptR3only;*/

	/*RfcOptions.destination ="PD8";
	RfcOptions.client = "250";
	RfcOptions.user = "DENIS";
	RfcOptions.language = "EN";
	RfcOptions.password = "CARINIT1";
	RfcOptions.trace = 1;
	RfcConnoptR3only.hostname="6m252";
	RfcConnoptR3only.gateway_host="6m252";
	RfcConnoptR3only.gateway_service="sapgw00";
	RfcOptions.connopt = &RfcConnoptR3only;*/

	RfcOptions.destination ="PQ8";
	RfcOptions.client = "500";
	RfcOptions.user = "DENIS";
	RfcOptions.language = "EN";
	RfcOptions.password = "CARINIT1";
	RfcOptions.trace = 1;
	RfcConnoptR3only.hostname="172.24.29.12";
	RfcConnoptR3only.gateway_host="172.24.29.12";	//tmleccqa
	RfcConnoptR3only.gateway_service="sapgw00";
	RfcOptions.connopt = &RfcConnoptR3only;

	/*	
	printf("\nDESTINATION    :%-50s",RfcOptions.destination);
	printf("\nCLIENT         :%-50s",RfcOptions.client);
	printf("\nUSER           :%-50s",RfcOptions.user);
	printf("\nPASSWORD       :%-50s",RfcOptions.password);
	printf("\nLAGUAGE        :%-50s",RfcOptions.language);
	printf("\nTRACE          :%-50d",RfcOptions.trace);
	printf("\nHOST NAME      :%-50s",RfcConnoptR3only.hostname);
	printf("\nGATEWAY HOST   :%-50s",RfcConnoptR3only.gateway_host);
	printf("\nGATEWAY SERVICE:%-50s",RfcConnoptR3only.gateway_service);
	printf("\nCONNOPT        :%-50u",RfcOptions.connopt);*/

	printf("\nConnecting to :%s %s %s",RfcOptions.destination,RfcOptions.client,RfcConnoptR3only.hostname);
	hRfc = RfcOpen(&RfcOptions);
	if (hRfc == RFC_HANDLE_NULL)
	{
		printf("\nERROR RECEIVED CPIC-ERROR");
		exit(0);
	}
	else
	{
		printf("\nGot the connection!!!");
		RfcEnvironment(&RfcEnv);
	}
	return hRfc;
}


extern int ITK_user_main (int argc, char ** argv )
{
    int status;
	int kk=0;
	int is=0;
	int sflag=0;
	int ii=2;
	int doccount=0;
	int *ip=&ii;
	int len=0;
	int Refcount=0;
	int resultCount =0;
	int n_entries = 1; //no. of query input arg

	tag_t		queryTag			= NULLTAG;
	tag_t		docOPtr				= NULLTAG;
	tag_t		*outTag				= NULLTAG;
	tag_t		DMLTag				= NULLTAG;
	tag_t		objTypeTag			= NULLTAG;
	tag_t		PartMasterTag		= NULLTAG;
	tag_t		resultOutputTag		= NULLTAG;
	tag_t		LatestRev			= NULLTAG;
    tag_t		docRelObjs			= NULLTAG;
    tag_t		*docObjs			= NULLTAG;
    tag_t		*refdocObjs			= NULLTAG;
    tag_t		doctag				= NULLTAG;
    tag_t		docRelObj			= NULLTAG;
    tag_t		refdocRelObj		= NULLTAG;
	tag_t		relation_type		= NULLTAG;

	char* sUserName	= NULL;
	char* sPassword = NULL;

	char* DMLNum = NULL;
	char* DMLNumDup = NULL;
	char* DMLDesc = NULL;
	char* DMLRelStatus = NULL;
	char* DMLProjCode = NULL;
	char* DMLProjCodeDup = NULL;
	char* DMLRelType = NULL;
	char* object_type = NULL;
	char* taskId = NULL;
	char* PartRelStatus = NULL;
	char* DateStr = NULL;

	char *LatRevName=NULL;
	char *part_type=NULL;
	char *unit=NULL;
	char *chrp=NULL;
	char *part_typeDup=NULL;
	char *unitDup=NULL;
	char *doc_no=NULL;
	char  type_name[TCTYPE_name_size_c+1];
	static RFC_RC RfcRc;

	date_t APLRelDate;
	date_t STDRelDate;

	char* APLRelDateStr = NULL;
	char* STDRelDateStr = NULL;

    plantcode=(char *) malloc (500 * sizeof(char));
	apl_release_date=(char *) MEM_alloc(40 * sizeof(char));
	DMLDescription=(char *) MEM_alloc(40 * sizeof(char));
	profit_centre2 = (char *)malloc(500 * sizeof(char));
	plan_calendar2 = (char *)malloc(500 * sizeof(char));
	overhd_grp2 = (char *)malloc(500 * sizeof(char));
	origin_group2 = (char *)malloc(500 * sizeof(char));
	origin_group = (char *)malloc(500 * sizeof(char));
	iPlantCode=(char *) malloc( 500 * sizeof(char));
	mat_type=(char *) malloc(500 * sizeof(char));
	meas_unit=(char *) malloc(500 * sizeof(char));

	tmpDrwNum = (char *)malloc(500 * sizeof(char));
	tmpDrwNum1 = (char *)malloc(500 * sizeof(char));
	tmpDrwRev = (char *)malloc(500 * sizeof(char));
	tmpDrwSeq = (char *)malloc(500 * sizeof(char));

	sap_proc_type = (char *)malloc(500 * sizeof(char));
	sap_spproc_type = (char *)malloc(500 * sizeof(char));
	SAPpstat = (char *)malloc(500 * sizeof(char));
	MRPpstat = (char *)malloc(500 * sizeof(char));
	myzeroDup1 = (char *)malloc(500 * sizeof(char));
	myzeroDup2 = (char *)malloc(500 * sizeof(char));
	myzeroDup3 = (char *)malloc(500 * sizeof(char));
	myzeroDup4 = (char *)malloc(500 * sizeof(char));
	std_price = (char *)malloc(500 * sizeof(char));
	moving_avg_price = (char *)malloc(500 * sizeof(char));
	cost_lot_size = (char *)malloc(500 * sizeof(char));
	mat_type = (char *)malloc(500 * sizeof(char));
	descDup = (char *)malloc(500 * sizeof(char));
	reord_pt = (char *)malloc(500 * sizeof(char));
	profit_centre_sap = (char *)malloc(500 * sizeof(char));

	part_noDupDes = (char *)malloc(500 * sizeof(char));
	part_noDupDesx = (char *)malloc(500 * sizeof(char));



	//char *qry_entries[1] = {"Name"};
	//char *qry_entries[1] = {"item_id"};
	char *qry_entries[1] = {"ID"};
	char **qry_values = (char **) MEM_alloc(10 * sizeof(char *));

	printf("\n 111111"); fflush(stdout);

	//ITK_CALL(ITK_init_module("loader" ,"abc","dba")!=ITK_ok) ;
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_auto_login( ));
	ITK_CALL(ITK_set_journalling( TRUE ));

	sUserName = ITK_ask_cli_argument("-u=");
	sPassword = ITK_ask_cli_argument("-p=");
	inputDml = ITK_ask_cli_argument("-d=");

	if(tc_strcmp(sUserName,"")==0 || tc_strcmp(sPassword,"")==0 || tc_strcmp(inputDml,"")==0)
	{
		printf("\nPartMatCreChange -u=userid -p=password -d=inputDml\n");fflush(stdout);
		goto CLEANUP;
	}

	//sprintf(fsuccess_name,"/user/plmsap/PLMSAP/PLM_SAP_APL_LOG/APL_Sap_Mat_%s.log",inputDML);
	sprintf(fsuccess_name,"APL_Sap_Mat_%s.log",inputDml);
	fsuccess = fopen(fsuccess_name,"a");

	sprintf(fisa_name,"INSPECTION_ALLOC_%s.log",inputDml);
	finsp = fopen(fisa_name,"a");

	sprintf(fvac_name,"VALUATIONCR_%s.log",inputDml);
	fval = fopen(fvac_name,"a");

	//if(QRY_find("APLDML_Query", &queryTag));
	if(QRY_find("APLDMLRevisionQry", &queryTag));
	if (queryTag)
	{
		printf("\nQuery Found : APLDMLRevisionQry\n");fflush(stdout);
	}
	else
	{
		printf("\nQuery NotFound : APLDMLRevisionQry\n");fflush(stdout);
		goto CLEANUP;
	}
	printf("\nAfter QRY_find : APLDMLRevisionQry\n");fflush(stdout);
    printf("\nInput APL DML : %s\n ", inputDml);fflush(stdout);

    qry_values[0] = inputDml;

	if(QRY_execute(queryTag, n_entries, qry_entries, qry_values, &resultCount, &outTag));

    printf("\nResultCount :%d:\n", resultCount);fflush(stdout);

	if(resultCount>0)
	{
		DMLTag=outTag[0];
		if(TCTYPE_ask_object_type(DMLTag,&objTypeTag));
    	if(TCTYPE_ask_name(objTypeTag,type_name));
	    printf("\nInput APL DML Object Type : %s\n",type_name);fflush(stdout);
		printf("\nQuery Executed...\n");fflush(stdout);

		ITK_CALL(AOM_UIF_ask_value(DMLTag,"current_id",&DMLNum));
		ITK_CALL(AOM_UIF_ask_value(DMLTag,"t5_cprojectcode",&DMLProjCode));
		ITK_CALL(AOM_UIF_ask_value(DMLTag,"t5_rlstype",&DMLRelType));
		ITK_CALL(AOM_UIF_ask_value(DMLTag,"current_desc",&DMLDesc));
		ITK_CALL(AOM_UIF_ask_value(DMLTag,"release_status_list",&DMLRelStatus));

		ITK_CALL(AOM_ask_value_date(DMLTag,"t5_APLReleaseDate",&APLRelDate));
		ITK_CALL(DATE_date_to_string(APLRelDate,"%d/%m/%Y",&APLRelDateStr));
		
		ITK_CALL(AOM_ask_value_date(DMLTag,"date_released",&STDRelDate));
		ITK_CALL(DATE_date_to_string(STDRelDate,"%d/%m/%Y",&STDRelDateStr));
		
		ITK_CALL(ITK_date_to_string(APLRelDate,&DateStr));

		if (tc_strlen(DateStr)==0)
		{
			printf("\nDML Number [%s] APL Release Date is NULL..Exiting!\n",DMLNum,tc_strlen(DateStr));
			//goto CLEANUP;
		}

		tc_strdup(DMLNum,&DMLNumDup);
		tc_strdup(DMLProjCode,&DMLProjCodeDup);

		if(tc_strstr(DMLRelStatus,"Released")!=NULL && (tc_strstr(DMLRelStatus,"APL")!=NULL || tc_strstr(DMLRelStatus,"STD")!=NULL))
		{
			printf("\nDML Number [%s] Release Type [%s] Project Code [%s]\n",DMLNum,DMLRelType,DMLProjCode);fflush(stdout);
			printf("\nRelease Status [%s] APLReleaseDate [%s] STD Released Date [%s]\n",DMLRelStatus,APLRelDateStr,STDRelDateStr);fflush(stdout);
			printf("\nDML Desc :%s\n",DMLDesc);fflush(stdout);
			printf("\nDMLNumDup :%s	DMLProjCodeDup : %s\n",DMLNumDup,DMLProjCodeDup);fflush(stdout);
		}
		else
		{
			printf("\nDML Number [%s] Release Status [%s] Not Found APL Released or Above...\n",DMLNum,DMLRelStatus);fflush(stdout);
			goto CLEANUP;
		}


		if(FetchPlantSpecificData(DMLNumDup,DMLProjCodeDup)>0)
		{
			printf("\nGot Plant Specific Data...\n");fflush(stdout);
		}
		else
		{
			printf("\nError in Fetching Plant Specific Data...\n");fflush(stdout);
			goto CLEANUP;
		}
		
		if(tc_strcmp(plantcode,"")==0)
		{
			printf("\nPlant code is null hence exit"); fflush(stdout);
			fprintf(fsuccess,"\nPlant code is null hence exit"); fflush(fsuccess);
			goto CLEANUP;
		}

		strcpy(myzeroDup1,"0.0");
		strcpy(myzeroDup2,"0.00");
		strcpy(myzeroDup3,"0.000");
		strcpy(myzeroDup4,"0.0000");

		if(tc_strlen(DMLDesc)<=5)
		{
			tc_strcpy(DMLDescription,DMLNum);
		}
		else
		{
			tc_strncpy(DMLDescription,DMLDesc,80);
		}
		//tc_strdup(DMLDesc,&DMLDescription);

		//DmlReleasedDate [11/04/2018]
		strcpy(apl_release_date,"");
		strcpy(apl_release_date,strtok(APLRelDateStr,"/"));
		strcat(apl_release_date,".");
		strcat(apl_release_date,strtok(NULL,"/"));
		strcat(apl_release_date,".");
		strcat(apl_release_date,strtok(NULL,"/"));

		printf("\ndml_no_arg :%s",dml_no_arg);fflush(stdout);
		printf("\napl_release_date :%s",apl_release_date);fflush(stdout);
		printf("\nDMLDescription :%s",DMLDescription);fflush(stdout);

		/// Solution Item start
		GRM_find_relation_type("T5_DMLTaskRelation",&relation_type);
		ITK_CALL(GRM_list_secondary_objects_only(DMLTag,relation_type,&tcount,&TaskRevision));
		printf("\nAPL DML Revision to Task Count : %d",tcount);fflush(stdout);
		for (TaskCnt=0;TaskCnt<tcount ;TaskCnt++ )
		{
			TaskRevTag = TaskRevision[TaskCnt];
			ITK_CALL(AOM_ask_value_string(TaskRevTag,"object_type",&object_type));
			ITK_CALL(AOM_ask_value_string(TaskRevTag,"item_id",&taskId));
			printf("\nTask ID [%s]	object_type : %s",taskId,object_type);fflush(stdout);
			
			pcount=0;
			ITK_CALL(AOM_ask_value_tags(TaskRevTag,"CMHasSolutionItem",&pcount,&PartTags));
			printf("\nTask To Part Count : %d",pcount);fflush(stdout);
			
			if (pcount>0 && TaskCnt==0)
			{
				displaying_objects();
			}

			for (PartCnt=0;PartCnt<pcount;PartCnt++)
			{
				LatestRev=PartTags[PartCnt];

				//ITK_CALL(ITEM_ask_latest_rev(resultOutputTag,&LatestRev));	
				// ITK_CALL(AOM_ask_value_string(LatestRev,"IMAN_master_form_rev",&LatRevName));

				ITK_CALL(AOM_UIF_ask_value(LatestRev,"object_string",&LatRevName));
				ITK_CALL(AOM_UIF_ask_value(LatestRev,"owning_user",&OwnerName));
				tc_strdup(OwnerName,&OwnerNameDup);
				ITK_CALL(AOM_UIF_ask_value(LatestRev,"t5_PartType",&part_type));
				ITK_CALL(AOM_UIF_ask_value(LatestRev,"item_id",&part_noDup));
				ITK_CALL(AOM_UIF_ask_value(LatestRev,"release_status_list",&PartRelStatus));

				printf("\nLatRevName:%s \n",LatRevName);fflush(stdout);
				printf("\nPart Type: %s\n",part_type);
				printf("\npart_noDup: %s\n",part_noDup);
				printf("\nOwnerName: %s\n",OwnerNameDup);

				if(tc_strstr(PartRelStatus,"Released")!=NULL && (tc_strstr(PartRelStatus,"APL")!=NULL || tc_strstr(PartRelStatus,"STD")!=NULL))
				{
					printf("\nPartRelStatus: %s\n",PartRelStatus);
				}
				else
				{
					printf("\nPart %s PartRelStatus: %s Part LCS not APL Released or Above...\n",LatRevName,PartRelStatus);
					continue;
				}

				len = tc_strlen(part_noDup);
            
				if(len>0)
				{
					if(tc_strstr(part_noDup,"98R"))
					{
						tc_strdup("T",&part_type);
						tc_strdup("T",&part_typeDup);
					}
					else if(*part_noDup == 'G')
					{
						 tc_strdup("C",&part_type);
						 tc_strdup("C",&part_typeDup);
					}

				}

				strcpy(part_noDupDes,part_noDup);
				
				strcpy(part_noDupDesx,part_noDup);

				if(tc_strlen(part_type)>0)
				{
					 tc_strdup(part_type,&part_typeDup);
					/*if (tc_strcmp(part_typeDup, "C") == 0) mat_type = "HALB";
					else if (tc_strcmp(part_typeDup, "V") == 0) mat_type = "HALB";
					else if (tc_strcmp(part_typeDup, "VC") == 0) mat_type = "FERT";
					else if (tc_strcmp(part_typeDup, "T") == 0) mat_type = "HALB";
					else if (len == 12 || len == 14) mat_type = "HALB";
					else if (len == 10 || len == 11) mat_type = "ROH";*/
					if (tc_strcmp(part_typeDup, "C") == 0)
					{
						tc_strcpy(mat_type,"HALB");
					}
					else if (tc_strcmp(part_typeDup, "M") == 0)
					{
						tc_strcpy(mat_type,"HALB");
					}
					else if (tc_strcmp(part_typeDup, "V") == 0)
					{
							tc_strcpy(mat_type,"HALB");
					}
					else if ((tc_strcmp(part_typeDup, "VC") == 0) || (tc_strcmp(part_typeDup, "CKDVC") == 0) || (tc_strcmp(part_typeDup, "SKDVC") == 0))
					{
							tc_strcpy(mat_type,"FERT");
					}
					else if (tc_strcmp(part_typeDup, "VCCR") == 0)
					{
							tc_strcpy(mat_type,"FERT");
					}
					else if (tc_strcmp(part_typeDup, "T") == 0)
					{
							tc_strcpy(mat_type,"HALB");
					}
					else if (tc_strcmp(part_typeDup,"DTPL")==0)
					{
						tc_strcpy(mat_type, "HALB");
					}
					else if (tc_strcmp(part_typeDup,"SA")==0)  // added on 17.03.2016 for 16 digit stage assembly of 60R TPL disintegration 
					{
						tc_strcpy(mat_type, "HALB");
					}
					else if (len == 12 || len == 14)
					{
							tc_strcpy(mat_type,"HALB");
					}
					else if (len == 10 || len == 11)
					{
							tc_strcpy(mat_type,"ROH");
					}
					else if (len == 14 || len == 16)	/*cabin kit logic by Ashish on 19.11.2013*/
					{
						if((*(part_noDup+10)=='C') && (*(part_noDup+11)=='K'))
						{
							tc_strcpy(mat_type,"HALB");
						}
					}

					printf("\nMaterial Type: %s", mat_type); /*part_type = acr_ind*/
					printf("\nI M HERE AFTER MATERIAL TYPE 2nd time");
				}
				else
				{
					printf("\nPart Type for the part %s is NULL",part_noDup);
					//fprintf(fsuccess,"\nMSG RCVD:Part Type for the part %s is NULL",part_noDup);
					continue;
				}

                 chrp=tc_strstr(part_noDup,"99");
				 printf("\nchrp:%s",chrp);

				//if((tc_strcmp(part_typeDup, "D")==0) && (tc_str_contains_wildcard_at(part_noDup,"99",ip)!=0))
				if(tc_strcmp(part_typeDup, "D")==0)
				if((tc_strcmp(part_typeDup, "D")==0) && (tc_strcmp(chrp,"9")==0))
				{
					printf("\nPart %s is DUMMY! AND HENCE CAN NOT CREATE MATERIAL IN SAP",part_noDup);
					//fprintf(fsuccess,"\nMSG RCVD:Part %s is DUMMY! AND HENCE CAN NOT CREATE MATERIAL IN SAP",part_noDup);
					//fprintf(fsuccess,"\n******************************************************************");
					continue;
				}

				ITK_CALL(ITEM_find_item(part_noDup, &PartMasterTag));
				if (PartMasterTag != NULLTAG)
				{
					ITK_CALL(AOM_UIF_ask_value(PartMasterTag,"uom_tag",&unit));
					//ITK_CALL(AOM_UIF_ask_value(PartMasterTag,"t5_EngineType",&PartFamily));
					//tc_strdup(PartFamily,&PartFamilyDup);
				}
				printf("\nUnit Of Masure : %s",unit);
		
			   if(tc_strlen(unit)>0)
				{
					tc_strdup(unit,&unitDup);
					if (tc_strcmp(unitDup,"1") == 0) tc_strcpy(meas_unit,"M ");
					else if (tc_strcmp(unitDup,"2") == 0) tc_strcpy(meas_unit,"KG");
					else if (tc_strcmp(unitDup,"3") == 0) tc_strcpy(meas_unit,"L ");
					else if (tc_strcmp(unitDup,"4-Nos") == 0) tc_strcpy(meas_unit,"EA");
					else if (tc_strcmp(unitDup,"5") == 0) tc_strcpy(meas_unit,"M2");
					else if (tc_strcmp(unitDup,"each") == 0) tc_strcpy(meas_unit,"EA");
					else if (tc_strcmp(unitDup,"7") == 0) tc_strcpy(meas_unit,"TO");
					else if (tc_strcmp(unitDup,"8") == 0) tc_strcpy(meas_unit,"M3");
					else if (tc_strcmp(unitDup,"9") == 0) tc_strcpy(meas_unit,"TS");
					else tc_strdup("EA",&unitDup);
				}
				else tc_strcpy(meas_unit,"EA");
				/*if ((tc_strcmp(mat_type, "ROH") == 0)&&(tc_strcmp(prjcodeDup,"H800")!=0))
				{
					strcpy(meas_unit,"KG");
				}*/

				//strcpy(meas_unit,"SET");
				printf("\nUnit Of Measure: %s", meas_unit);

				ITK_CALL(AOM_UIF_ask_value(LatestRev,"object_desc",&desc));

				/*Description = Nomenclature*/

				if(tc_strlen(desc)>0)
				{
					tc_strdup(desc,&descDup);
					printf("\nDescription: %s", descDup);
				}
				else printf("\nDescription is NULL!");

				/*if Drawing Indicator = D, only then go for Documents*/
				ITK_CALL(AOM_UIF_ask_value(LatestRev,"t5_DrawingInd",&drg_ind));
				
				if(tc_strlen(drg_ind)>0)
				{
					tc_strdup(drg_ind,&drg_indDup);
					printf("\nDrawing Indicator: %s\n", drg_indDup);
				}
				else
				{
					tc_strdup("",&drg_indDup);
					printf("\nDrawing Indicator is NULL!\n");
				}

                if(tc_strcmp(part_type,"D")!=0)
				{
					if(tc_strcmp(drg_indDup,"D")==0)
					{
						ITK_CALL(GRM_find_relation_type("IMAN_specification", &docRelObj));
			
						if(docRelObj)
						{
							ITK_CALL(GRM_list_secondary_objects_only(LatestRev,docRelObj,&doccount,&docObjs));
							printf("\n Speccount Value :  %d\n",doccount);fflush(stdout);

							for (is=0;is<doccount;is++ )
							{	
								doctag = docObjs[is];
								
								if(TCTYPE_ask_object_type(doctag,&docOPtr));
								if(TCTYPE_ask_name2(docOPtr,&docClass));
								printf("\nT5TaskToPartCreateVal RefDoc_tag  docClass :: %s\n", docClass);fflush(stdout);
							
								if(!strcmp(docClass,"ProDrw"))
								{
									printf("\n ProDrw \n");fflush(stdout);
									//intDrgCnt++;
									sflag=1;
									break;
								}
								else if(!strcmp(docClass,"CMI2Drawing"))
								{
									printf("\n CMI2Drawing \n");fflush(stdout);
									//intDrgCnt++;
									sflag=1;
									break;
								}
							}
						}
						else
						{
							ITK_CALL(GRM_find_relation_type("IMAN_reference", &refdocRelObj));
							if(refdocRelObj)
							{
								ITK_CALL(GRM_list_secondary_objects_only(LatestRev,refdocRelObj,&Refcount,&refdocObjs));
								printf("\n Refcount Value :-------------------->>>> Rajendra   %d\n",Refcount);fflush(stdout);

								for (is=0;is<Refcount;is++ )
								{	
									doctag = refdocObjs[is];
									if(TCTYPE_ask_object_type(doctag,&docOPtr));
									if(TCTYPE_ask_name(docOPtr,docClass));
									printf("\n RefDoc_tag  docClass :: %s\n", docClass);fflush(stdout);
								
									if(!strcmp(docClass,"ProDrw"))
									{
										printf("\n Ref ProDrw \n");fflush(stdout);
										//intDrgCnt++;
										sflag=1;
										break;
									}
									else if(!strcmp(docClass,"CMI2Drawing"))
									{
										printf("\n Ref CMI2Drawing \n");fflush(stdout);
										//intDrgCnt++;
										sflag=1;
										break;
									}
								}
							}
						}

						if(sflag>0)
						{
						  
							 printf("\n<Drawing Documents Found!>");fflush(stdout);

							 ITK_CALL(AOM_UIF_ask_value(doctag,"object_name",&doc_no));
							 tc_strdup(doc_no,&doc_noDup);

							printf("\nDocument Number: %s",doc_noDup);fflush(stdout);

							if(tc_strlen(docClass)>0)
							{
								 tc_strdup(docClass,&dwg_typeDup);
								printf("\nDocument Type: %s", dwg_typeDup);
							}
							else
								printf("\nDocument Type is NULL!");fflush(stdout);
					
							strcpy(tmpDrwNum,doc_noDup);
							tmpDrwNum1=strtok(tmpDrwNum,"/");
							tmpDrwRev=strtok(NULL,";");
							tmpDrwSeq=strtok(NULL,";");

							printf("\nDocument Number: %s",tmpDrwNum1);fflush(stdout);
							printf("\nDocument Rev: %s",tmpDrwRev);fflush(stdout);
							printf("\nDocument Seq: %s",tmpDrwSeq);fflush(stdout);

							if(tc_strlen(tmpDrwRev)>0)
							{
								tc_strdup(tmpDrwRev,&dwg_revDup);
								printf("\nDocument Version: %s", dwg_revDup);fflush(stdout);
								//fprintf(fsuccess,"\nDocument Version: %s", dwg_revDup);
							}
							else
							{
								tc_strdup("NR",&dwg_revDup);
								printf("\nDocument Version: %s", dwg_revDup);fflush(stdout);
								//fprintf(fsuccess,"\nDocument Version: %s", dwg_revDup);
							}

							tc_strdup("zero",&sizeDup);// ZERO as no Sheet available in UA
						}
					}
					else
					{
						printf("Getting NR revision here");fflush(stdout);
						tc_strdup("NR",&dwg_revDup);
						tc_strdup("",&sheet_noDup);
						tc_strdup("",&sizeDup);
					}

					tc_strdup("",&OldMatNoDup);

					printf("\nOld Material No.: %s", OldMatNoDup);fflush(stdout);

					if ((tc_strcmp(part_typeDup, "VC") == 0) || (tc_strcmp(part_typeDup, "VCCR") == 0)|| (tc_strcmp(part_typeDup, "CKDVC") == 0)|| (tc_strcmp(part_typeDup, "SKDVC") == 0))
					{
						printf("\nPlease Create Classification View for VC - %s",part_noDup);fflush(stdout);
						//fprintf(fsuccess,"\nPlease Create Classification View for VC - %s",part_noDup);
						ITK_CALL(AOM_UIF_ask_value(LatestRev,"t5_Weight",&net_wt));
						
						if(tc_strlen(net_wt)>0)
						{
							tc_strdup(net_wt,&net_wtDup);
						}
						else
						{
							tc_strdup("0.000",&net_wtDup);
							
						}
					}
					else
					{
						tc_strdup("0.000",&net_wtDup);
					}


					tc_strdup("0.000",&gross_wt);
					printf("\nNet Weight: %s", net_wtDup);fflush(stdout);
					printf("\nGross Weight: %s", gross_wt);fflush(stdout);
				}

               
			     /*Weight Unit = KG*/
				tc_strdup("KG",&unit_wt);
				printf("\nWeight Unit: %s", unit_wt);

				/*Volume = 0.000*/
				tc_strdup("0.000",&volume);
				printf("\nVolume: %s", volume);

				/*Volume Unit = M3*/
				tc_strdup("M3",&vol_unit);
				printf("\nVolume Unit: %s", vol_unit);

				/*Page Format of Production Memo = ""*/
				printf("\nPage Format of Production Memo: ''");

				/*Material Group = ZZZZZZ*/
				tc_strdup("ZZZZZZ",&material_group);
				printf("\nMaterial Group: %s", material_group);

				/*Division = 01*/
				tc_strdup("01",&basic_division);
				printf("\nBasic Division: %s", basic_division);

				/********************** add logic for pstat and apl_dml_flag here *******************/
				//group = subString(part_noDup, 0, 1);
				group = *part_noDup;
				printf("\nGroup: %c", group);


				//Uncomment later to check APL DML

			  	printf("\ndml_noDup=%s",dml_numAP1);
				series = subString(dml_numAP1, 2, 2);
				printf("\nDML Series: %s", series);

				if ((tc_strcmp(series, "AM") == 0) || (tc_strcmp(series, "MC") == 0)) apl_dml_flag = 2;
				else apl_dml_flag = 0;
				printf("\nAPL_DML_FLAG: %d", apl_dml_flag);

				/*strcpy(part_no_chk,part_noDup);*/

				/*pstat = pstat_basic("283429200111", "7001", 1, "G"); testing*/

				printf("\nim here:");

				strcpy(sap_proc_type,"");
				strcpy(sap_spproc_type,"");
				strcpy(SAPpstat,"");
				strcpy(MRPpstat,"");

				printf("\nim here:");

				GetCSPstat(/*part_noDup*/);

				pstat='0';
				pstat_mrp='0';
				pstat_acc='0';

				/*pstat = pstat_basic(part_noDup, plantcode, 1, group);
				pstat_mrp = pstat_basic(part_noDup, plantcode, 2, group);
				pstat_acc = pstat_basic(part_noDup, plantcode, 3, group);*/

				pstat_basicFun();

				//pstat_mrp='0';
				//pstat_acc='0';

				//printf("\n... pstat Values are :%c:%c:%c",pstat,pstat_mrp,pstat_acc);fflush(stdout);
				//fprintf(fsuccess,"\n... pstat Values are :%c:%c:%c",pstat,pstat_mrp,pstat_acc);fflush(stdout);

				printf("\nPStat for Basic View: %c",pstat);
				printf("\nPStat for MRP View: %c",pstat_mrp);
				printf("\nPStat for Accounting View: %c",pstat_acc);

				//fprintf(fsuccess,"\nPStat %c %c %c",pstat,pstat_mrp,pstat_acc);

				//pstat_mrp='0';
				//pstat_acc='0';

				//pstat='0';
				//pstat_mrp='0';
				//pstat_acc='0';
				printf("\nPStat for Basic View: %c",pstat);
				printf("\nPStat for MRP View: %c",pstat_mrp);
				printf("\nPStat for Accounting View: %c",pstat_acc);

				if (pstat == 'K')
				{
					apl_dml_flag = 0;
				}
				else
				{
					apl_dml_flag = 2;
				}
				printf("\nAPL_DML_FLAG after PStat :: %d", apl_dml_flag);

				printf("\nPlant Code Is :: %s",plantcode);

				/*t5CheckDstat(objGetAttribute(PartOPtr, "t5PunMakeBuyIndicator", &make_buy_ind));*/

				ITK_CALL(AOM_UIF_ask_value(LatestRev,Plant_MakeBuy,&PartMakeBuyInd));
				ITK_CALL(AOM_UIF_ask_value(LatestRev,Plant_StoreLoc,&store_loc));
				printf("\nPart Make Buy Ind : %s",PartMakeBuyInd);
				if (tc_strlen(PartMakeBuyInd)==0)
				{
					printf("\nSkiping Part %s from MM Creation as Make Buy Ind is NULL!!",part_noDup);
					continue;
				}
				make_buy_ind = strtok(PartMakeBuyInd," " );
				printf("\nMake_buy_ind : %s		store_loc : %s ",make_buy_ind,store_loc);

				tc_strdup(store_loc,&store_locDup);

				if(tc_strcmp(make_buy_ind,"F19")==0)
				{
					strcpy(make_buy_ind,"F30");
				}

				if(tc_strcmp(make_buy_ind,"NA")==0)
				{
					printf("\nMakebuy indicator is NA hence can not create material");
					goto CLEANUP;
				}

				/***********************************************************************************/

				/*make_buy_ind = nlsStrDup("E99");
				make_buy_indDup = nlsStrDup("E99");*/

				if(tc_strcmp(plantcode,"1100")==0)
				{
					if(tc_strcmp(sap_proc_type,"E")==0)
					{
						//strcpy(proc_Key,sap_proc_type);
						proc_Key = 'E';
					}
					else
					{
						//strcpy(proc_Key,"0");
						proc_Key = '0';
					}

					if(tc_strcmp(sap_spproc_type,"99")==0)
					{
						//strcpy(spl_Key,"Y");
						spl_Key = 'Y';
					}
					else
					{
						//strcpy(spl_Key,"0");
						spl_Key = '0';
					}

					printf("\n Plant 1100...proc_Key:[%c] spl_Key:[%c]",proc_Key,spl_Key) ; fflush(stdout);
					//fprintf(fsuccess,"\n Plant 1100...proc_Key:[%c] spl_Key:[%c]",proc_Key,spl_Key) ; fflush(stdout);
				}

				if(tc_strcmp(plantcode,"1100")==0)
				{
					if(tc_strlen(make_buy_ind)>0)
					{

						tc_strdup(make_buy_ind,&make_buy_indDup);
						printf("\nInSide GET CS Proc");
						//fprintf(fsuccess,"\nInSide GET CS Proc");
						//proc_Key = GetCSSAP(part_noDup,1);
						//spl_Key = GetCSSAP(part_noDup,2);
						if(proc_Key == 'E' && spl_Key == 'Y')
						{
							printf("\nTaking Value from CS Proc");
							//fprintf(fsuccess,"\nTaking Value from CS Proc");
							printf("\nMake/Buy Indicator in PLM:%s:", make_buy_indDup);
							//fprintf(fsuccess,"\nPune Make/Buy Indicator in PLM:%s:", make_buy_indDup);
							tc_strdup("E",&proc_type);
							printf("\nProcurement Type: %s", proc_type);
							//fprintf(fsuccess,"\nProcurement Type: %s", proc_type);
							tc_strdup("99",&spl_proc_key);
							
							printf("\nSpecial Procurement Key: %s", spl_proc_key);
							//fprintf(fsuccess,"\nSpecial Procurement Key: %s", spl_proc_key);
						}
						else
						{
							printf("\nTaking Value from PLM");
							//fprintf(fsuccess,"\nTaking Value from PLM");
							printf("\nMake/Buy Indicator:%s:", make_buy_indDup);
							proc_type = subString(make_buy_indDup, 0, 1);
							printf("\nProcurement Type: %s", proc_type);
							spl_proc_key = subString(make_buy_indDup, 1, 2);
							printf("\nSpecial Procurement Key: %s", spl_proc_key);
						}
					}
					else
					{
						printf("\nPune Make/Buy Indicator is NULL!");
						printf("\nProcurement Type is NULL!");
						printf("\nSpecial Procurement Key is NULL!");
						printf("\nPart %s is found in Release Vault with Life Cycle State as APL Released but CS is NULL",part_noDup);
						//fprintf(fsuccess,"\nMSG RCVD:Part %s is found in Release Vault with Life Cycle State as APL Released but CS is NULL",DispNameDup);
						continue;
					}
				}
				else
				{
					if(tc_strlen(make_buy_ind)>0)
					{
                       	tc_strdup(make_buy_ind,&make_buy_indDup);

					
						printf("\nMake/Buy Indicator:%s:", make_buy_indDup);
						proc_type = subString(make_buy_indDup, 0, 1);
						printf("\nProcurement Type: %s", proc_type);
						spl_proc_key = subString(make_buy_indDup, 1, 2);
						printf("\nSpecial Procurement Key: %s", spl_proc_key);

					}
					else
					{
						printf("\nPune Make/Buy Indicator is NULL!");
						printf("\nProcurement Type is NULL!");
						printf("\nSpecial Procurement Key is NULL!");
						printf("\nPart %s is found in Release Vault with Life Cycle State as APL Released but CS is NULL",part_noDup);
						//fprintf(fsuccess,"\nMSG RCVD:Part %s is found in Release Vault with Life Cycle State as APL Released but CS is NULL",DispNameDup);
						continue;
					}


				}

				//if(tc_strcmp(sap_proc_type,"F")==0)
				

				/**************************MRP 1 VIEW***********************************************/

				/*mrp_grp*/
				/*mrp_type-> car_dml => if plantcode == 1100, car_dml=1 else car_dml=0
				car_dml = 0 for plant 7001*/
				/*strcpy(mrp_grp, "");*/

				if(tc_strcmp(plantcode,"1100")==0)
				{
					car_dml=1;
				}
				else
				{
					car_dml=0;
				}

				printf("\ncar_dml value is :: %d",car_dml);
				//fprintf(fsuccess,"\ncar_dml value is :: %d",car_dml);


/*MRP type logic changed by Prasad as per request from Kartik Dhond. mail dated 29.04.2016 */

                  tc_strdup("",&mrp_grp);
                  tc_strdup("PD",&mrp_type);
			 
			
				if (tc_strcmp(part_typeDup, "D") != 0)
			   {
				printf("\nMaterial type %s",mat_type);

				if (tc_strcmp(mat_type, "FERT") == 0)
				{
					tc_strdup("A1",&mrp_grp);
					
				}
				else if (tc_strcmp(mat_type, "HALB") == 0 || tc_strcmp(mat_type, "ROH") == 0)
				{
					tc_strdup("A2",&mrp_grp);
					
				}
				if (tc_strcmp(proc_type, "F") == 0)
				{
					tc_strdup("PD",&mrp_type);
					
				}
				if (tc_strcmp(make_buy_indDup, "F18") == 0) 
				{
					tc_strdup("PD",&mrp_type);
					
				}
				if (tc_strcmp(make_buy_indDup, "F30") == 0) 
				{
					tc_strdup("PD",&mrp_type);
					
				}
				if (tc_strcmp(subString(part_noDup,0,1),"G") == 0)
				{
					if ((tc_strcmp(make_buy_indDup, "E") == 0) || (tc_strcmp(make_buy_indDup, "E50") == 0) )
					{
						tc_strdup("PD",&mrp_type);
						
					}
				}
			}

/*MRP logic change end*/

				printf("\nMRP Group: %s", mrp_grp);
				printf("\nMRP Type: %s", mrp_type);

				/*abc_ind = "B"*/
				tc_strdup("B",&abc_ind);
				printf("\nABC Indicator: %s", abc_ind);

				/*mm_pp_status-> odstatus = ??
				EXEC SQL select	'1' into :odstatus from	amden_part where part_no = :comp
				and	substr(part_no,9,2) = '16' and description like '%OFFER%';*/
				num = subString(part_noDup, 8, 2);
				/*if ((strcmp(num, "16")==0) && (!nlsIsStrNull(nlsStrStr(descDup, "OFFER")))) */
				tc_strdup("",&odstatus);

				if ((tc_strcmp(num, "16")==0))
				{
					//fprintf(fsuccess,"\nnum= %s.........",num);
					if((tc_strstr(descDup, "OFFER"))!=NULL)
					{
						tc_strdup("1",&odstatus);
						
					}
				}
				else
				{
					tc_strdup("",&odstatus);
					
				}
				
			

				ITK_CALL(AOM_UIF_ask_value(LatestRev,"t5_PartStatus",&drstatus));

				
				if(tc_strlen(drstatus)>0)
				{
					tc_strdup(drstatus,&drstatusDup);
				}
			
				//fprintf(fsuccess,"\nPart Number: %s    DR/AR Status:%s", part_noDup,drstatusDup); fflush(fsuccess);

			  ITK_CALL(AOM_UIF_ask_value(LatestRev,"t5_ColourInd",&partClrInd));

				
				if(tc_strlen(partClrInd)>0)
				{
					tc_strdup(partClrInd,&partClrIndDup);
				} 

				printf("\nPart Number: %s    DR/AR Status:%s   Color Ind:%s ", part_noDup,drstatusDup,partClrIndDup); fflush(stdout);

				tc_strdup("",&mm_pp_status);

				if ((tc_strcmp(make_buy_indDup, "F18") == 0)  || (tc_strcmp(make_buy_indDup, "F") == 0)  || (tc_strcmp(make_buy_indDup, "F19") == 0) || (tc_strcmp(make_buy_indDup, "F30") == 0)) 
				{
				
                  tc_strdup("30",&mm_pp_status);

			    	printf("\nmmpp status is set to 30 for part with proc F"); fflush(stdout);
			
				}



			   if ((tc_strcmp(partClrIndDup, "Y") == 0))
				{

				   tc_strdup("PB",&mm_pp_status);
					printf("\nmmpp status is set to PB for colour ind Y part"); fflush(stdout);
					//fprintf(fsuccess,"\nmmpp status is set to PB for colour ind Y part"); fflush(stdout);
				}
				else
				{

					//As per new request from Rohan Ghate below logic is added on date 06.12.2016 for materail status mm_pp_status
					if((tc_strcmp(part_typeDup, "VC") == 0) || (tc_strcmp(part_typeDup, "VCCR") == 0))
					{
	//					if(dstat = oiSqlCreateSelect(&mstrlSql)) goto CLEANUP;
	//					if(dstat = oiSqlWhereBegParen(mstrlSql)) goto CLEANUP;
	//					if(dstat = oiSqlWhereEQ(mstrlSql,"t5Syscd","MMPPST")) goto CLEANUP;
	//					if(dstat = oiSqlWhereAND(mstrlSql)) goto CLEANUP;
	//					if(dstat = oiSqlWhereEQ(mstrlSql,"t5SubSyscd",plantcode)) goto CLEANUP;
	//					if(dstat = oiSqlWhereEndParen(mstrlSql)) goto CLEANUP;
	//					if(dstat = QueryWhere("t5Cntrol",mstrlSql,&MMPPControlObjs,&mfail)) goto CLEANUP;
						
						//if(setSize(MMPPControlObjs)>0)
						//{
							if (tc_strcmp(OwnerNameDup, "Obsolete Vault") == 0 )
							{
								tc_strdup("AR",&mm_pp_status);
					
							} 
							else if((tc_strcmp(drstatusDup,"DR0")==0)||(tc_strcmp(drstatusDup,"DR1")==0)||(tc_strcmp(drstatusDup,"DR2")==0)||
								(tc_strcmp(drstatusDup,"DR3")==0)||(tc_strcmp(drstatusDup,"AR0")==0)||(tc_strcmp(drstatusDup,"AR1")==0)||
								(tc_strcmp(drstatusDup,"AR2")==0)||(tc_strcmp(drstatusDup,"AR3")==0))
							{
								tc_strdup("PB",&mm_pp_status);
								
								//printf("\nVC..mm_pp_status is set to 'PB' for DR status %s  < DR4",drstatusDup); fflush(stdout);
								//fprintf(fsuccess,"\nVC..mm_pp_status is set to 'PB' for DR status %s  < DR4",drstatusDup); fflush(fsuccess);
							}
							else
							{
								tc_strdup("",&mm_pp_status);

								printf("\nVC..mm_pp_status is set to NULL for DR status %s > DR3",drstatusDup); fflush(stdout);
								//fprintf(fsuccess,"\nVC..mm_pp_status is set to NULL for DR status %s > DR3",drstatusDup); fflush(fsuccess);
							}
						//}
						//else	// else condition for non-control object plant present which having part type VC/VCCR
						//{
						//		tc_strdup("",&mm_pp_status);

						//	printf("\nVC..Plantcode %s is not live to set mm_pp_status hence updating to NULL...",plantcode); fflush(stdout);
						//	//fprintf(fsuccess,"\nVC..Plantcode %s is not live to set mm_pp_status hence updating to NULL...",plantcode); fflush(fsuccess);
						//}
					}
					
					else
					{
						if (tc_strcmp(odstatus, "1") == 0)
						{
							tc_strdup("A7",&mm_pp_status);
							
						}
						else
						{
							printf("\nmmpp status inot changed"); fflush(stdout);
							fprintf(fsuccess,"\nmmpp status inot changed"); fflush(stdout);
						}

						printf("\nPartType: [%s]  odstatus:[%s] and mm_pp_status: [%s] \n  ",part_typeDup,odstatus,mm_pp_status); fflush(stdout);
						fprintf(fsuccess,"\nPartType: [%s]  odstatus:[%s] and mm_pp_status: [%s]  ",part_typeDup,odstatus,mm_pp_status); fflush(fsuccess);
					}
				}
				printf("\n%s Part_noDup: %s    part_typeDup: %s    drstatusDup: %s    mm_pp_status: %s\n",plantcode,part_noDup, part_typeDup,drstatusDup,mm_pp_status); fflush(stdout);
				fprintf(fsuccess,"\n%s Part_noDup: %s    part_typeDup: %s    drstatusDup: %s    mm_pp_status: %s",plantcode,part_noDup, part_typeDup,drstatusDup,mm_pp_status); fflush(fsuccess);

				printf("\nodstatus: %s", odstatus);
				printf("\nnum: %s", num);
				printf("\nMM/PP Status: %s", mm_pp_status);
				fprintf(fsuccess,"\nMM/PP Status: %s %s %s %s", mm_pp_status,num,odstatus,descDup);


				tc_strdup("0.000",&reord_pt);

				printf("\nReorder Point: %s", reord_pt);

				tc_strdup("Z",&quota_arrangement_usage);
				printf("\nQuota Arrangement Usage: %s", quota_arrangement_usage);

                tc_strdup("ZZZ",&mrp_controller);
			
				printf("\nMRP Controller: %s", mrp_controller);

		        tc_strdup("",&lot_size_key);
				tc_strdup("",&fixed_lot_size);
				
				if (tc_strcmp(part_typeDup, "D") != 0)
				{
					tc_strdup("EX",&lot_size_key);
					
				}
				printf("\nLot Size: %s\nFixed Lot Size: %s", lot_size_key, fixed_lot_size);

               tc_strdup("",&max_stlvl);
			    printf("\nMaximum stock level: %s", max_stlvl);
                
				tc_strdup("",&blk_ind);
				printf("\nBulk Indicator: %s", blk_ind);

				tc_strdup("000",&sched_mar_key);
				printf("\nScheduling Margin Key: %s", sched_mar_key);

				tc_strdup("T",&req_grp);
				printf("\nRequirements Grouping: %s", req_grp);

				/*******************************MRP 2 VIEW***********************************/

				tc_strdup("M",&period_ind);
				printf("\nPeriod Indicator: %s", period_ind);

                tc_strdup("",&mixedmrp);
				tc_strdup("S",&cs1_val);
				tc_strdup("",&alternativb);
		
				if (tc_strcmp(part_typeDup, "D") != 0)
				{
					if (tc_strcmp(mat_type, "FERT") == 0 && car_dml != 1)
					{
						tc_strdup("2",&mixedmrp);
						
					}
					if (tc_strcmp(cs1_val,"S") == 0)
					{
						tc_strdup("",&alternativb);
					}
				}
				printf("\nMixed MRP: %s", mixedmrp);
				printf("\nMethod for Selecting Alternative Bills: %s", alternativb);

				tc_strdup("02",&avail_chk);
				printf("\nChecking Group for Availability Check: %s", avail_chk);

				tc_strdup("",&splan_mat);
				printf("\nPlanning Material: %s", splan_mat);

				tc_strdup("",&splan_plant);
				printf("\nPlanning Plant: %s", splan_plant);

				tc_strdup("",&splan_conv_fact);
				printf("\nConversion Factor for Planning Material: %s", splan_conv_fact);

				tc_strdup("2",&ind_collect);
				printf("\nDependent requirements ind. for individual: %s", ind_collect);

				tc_strdup("",&rep_mfg_in);
				printf("\nRepetitive Mfg. Allowed: %s", rep_mfg_in);

				tc_strdup("",&rep_mfg);
				printf("\nRepetitive Manufacturing Profile: %s", rep_mfg);

				tc_strdup("0001",&stock_det_group);
				printf("\nStock determination group: %s", stock_det_group);

				/**************************STORAGE VIEW***********************************/
				tc_strdup("",&uoissue);
				printf("\nUnit of Issue: %s", uoissue);

				/*qua_ins_ind, doc_req, grptime, qm_proc_ind, certificate_type, control_key*/

				tc_strdup("",&qua_ins_ind);
				tc_strdup("",&doc_req);
				tc_strdup("1",&grptime);
				tc_strdup("",&certificate_type);
				tc_strdup("",&qm_proc_ind);

				if (tc_strcmp(part_typeDup, "D") != 0)
				{
					if (tc_strcmp(proc_type,"F") == 0 && tc_strcmp(spl_proc_key,"40") != 0)
					{  

						tc_strdup("X",&qua_ins_ind);
						tc_strdup("X",&doc_req);
						tc_strdup("1",&grptime);
						tc_strdup("0005",&certificate_type);
						tc_strdup("0008",&control_key);
						tc_strdup("X",&qm_proc_ind);

					}
					else if (tc_strcmp(proc_type,"E") == 0 || tc_strcmp(spl_proc_key,"40") == 0)
					{

                        tc_strdup("",&qua_ins_ind);
						tc_strdup("X",&doc_req);
						tc_strdup("",&grptime);
						tc_strdup("",&certificate_type);
						tc_strdup("0000",&control_key);
						tc_strdup("X",&qm_proc_ind);

					}
				}

				if (tc_strcmp(proc_type,"F") == 0 && (tc_strcmp(spl_proc_key,"40") != 0))
				{
					    tc_strdup("X",&qua_ins_ind);
						tc_strdup("X",&doc_req);
						tc_strdup("1",&grptime);
						tc_strdup("0005",&certificate_type);
						tc_strdup("0008",&control_key);
						tc_strdup("X",&qm_proc_ind);

					
				}

				if(tc_strcmp(mat_type,"FERT") == 0)	//Anuja,Swati Tendulkar Requirement QM View for VC 
				{

					tc_strdup("X",&qua_ins_ind);
					tc_strdup("",&doc_req);
					tc_strdup("",&grptime);
					tc_strdup("0005",&certificate_type);
					tc_strdup("0008",&control_key);
					tc_strdup("X",&qm_proc_ind);
				}
				
				printf("\nPost to Inspection Stock: %s", qua_ins_ind);
				printf("\nDocumentation Required Indicator: %s", doc_req);
				printf("\nGoods Receipt Processing Time in Days: %s", grptime);
				printf("\nQM in Procurement is Active: %s", qm_proc_ind);
				printf("\nControl Key for Quality Management: %s", control_key);
				printf("\nCertificate Type: %s", certificate_type);


               tc_strdup("",&catalog_prof);
				printf("\nCatalog Profile: %s", catalog_prof);

				tc_strdup("",&valuation_cat);
				tc_strdup("",&price_con);
				tc_strdup("",&std_price);
				tc_strdup("",&moving_avg_price);

				printf("\nmoving_avg_price: %s", moving_avg_price);


				if (tc_strcmp(part_typeDup, "D") != 0)
				{
                    if ((tc_strcmp(part_typeDup, "VC") != 0) && (tc_strcmp(part_typeDup, "VCCR") != 0) && (tc_strcmp(part_typeDup, "CKDVC") != 0) && (tc_strcmp(part_typeDup, "SKDVC") != 0))
					
					{
						printf("\nIm in If .......");
						flag_vc_v_tpl = 1;
						tc_strdup("S",&valuation_cat);
						tc_strdup("V",&price_con);
						tc_strdup("0.0100",&moving_avg_price);
					}
					else
					{
						printf("\nIm in else ......."); 
						tc_strdup("",&valuation_cat);
						tc_strdup("S",&price_con);
						tc_strdup("0.01",&std_price);
					}
				}
				printf("\nValuation Category: %s", valuation_cat);
				printf("\nPrice control indicator: %s", price_con);
				printf("\nStandard Price: %s", std_price);
				printf("\nMoving Average Price: %s", moving_avg_price);


               //tc_strdup("0.01",&val_class);

				if(tc_strcmp(part_typeDup,"D") != 0)
				{
					if (tc_strcmp(mat_type, "FERT") == 0)
					{
					   tc_strdup("0530",&val_class);
						
					}
					else
					{
						if (tc_strcmp(make_buy_indDup, "F") == 0 || tc_strcmp(make_buy_indDup, "F01") == 0)
						{
							tc_strdup("0110",&val_class);
							
						}
						if (tc_strcmp(make_buy_indDup, "F98") == 0)
						{
							tc_strdup("0110",&val_class);
						}
						if (tc_strcmp(make_buy_indDup, "F18") == 0)
						{
							tc_strdup("0110",&val_class);
						}
						if (tc_strcmp(make_buy_indDup, "F30") == 0)
						{
						
							tc_strdup("0240",&val_class);
						}
						if (tc_strcmp(make_buy_indDup, "F19") == 0)
						{
							tc_strdup("0240",&val_class);
						}
						if (tc_strcmp(proc_type, "E") == 0)
						{
							tc_strdup("0230",&val_class);
						}
					}

					printf("\nGoing in valuation class loop................%s %s :%s:",mat_type,val_class,make_buy_indDup);
					printf("\nValuation Class: %s", val_class);
			}

			if(tc_strcmp(val_class,"")==0)
			{
				printf("\n**Valuation Class is NULL, skipping from material transfer\n\n");
				//fprintf(fsuccess,"\n**Valuation Class is NULL, skipping from material transfer\n\n");
				//goto CLEANUP;
			}

			tc_strdup("X",&mat_origin);

			printf("\nMaterial-related origin: %s", mat_origin);

			strcpy(origin_group,origin_group2);
			get_origingroup();
			printf("\nOrigin Group: %s", origin_group);

			if (group != 'G')
			{
				tc_strdup("1",&cost_lot_size);
				
			}
			else if (group == 'G')
			{
				tc_strdup("100",&cost_lot_size);
				
			}
			printf("\nLot Size for Product Costing: %s", cost_lot_size);


			tc_strdup("PP1",&variance_key);
			printf("\nVariance Key: %s", variance_key);

			tc_strdup("X",&with_quantity_structure);
			printf("\nMaterial Is Costed with Quantity Structure: %s", with_quantity_structure);
		  
			tc_strdup("",&costing_split_valuation);
			tc_strdup("",&costing_val_class);


			if (tc_strcmp(part_typeDup, "D") != 0)
			{
				if (tc_strcmp(mat_type, "FERT") == 0)
				{
					tc_strdup("",&costing_split_valuation);
					tc_strdup("0530",&costing_val_class);
					
				}
				if (tc_strcmp(mat_type, "HALB") == 0 || tc_strcmp(mat_type, "ROH") == 0)
					  tc_strdup("S",&costing_split_valuation);
			}
			printf("\nCosting Valuation Category: %s", costing_split_valuation);
			printf("\nCosting Valuation Class: %s", costing_val_class);

			if (flag == 0)
			{
				mat_create();
				if (go_for_rfc == 1 && flag_vc_v_tpl == 1 && acc_flag !=1)
				{
					printf("\nAccounting View and go_for_rfc flag = 1");
					if ((tc_strcmp(part_typeDup, "VC") != 0) && (tc_strcmp(part_typeDup, "VCCR") != 0) && (tc_strcmp(part_typeDup, "CKDVC") != 0) && (tc_strcmp(part_typeDup, "SKDVC") != 0))
					{
						printf("\nPart is not VC");
						if((tc_strcmp(proc_type, "F") == 0) && (tc_strcmp(spl_proc_key, "40") != 0))
						{
							RfcRc = allocate_insp_type();
						}
					}
					/*In material change it will not go for batches create*/
					if (tc_strcmp(valuation_cat, "S") == 0 && flag != 1)
					{
						//extend_accounting_view();
					}
					go_for_rfc = 0;  /* reseting for next material  */
				}

				if (tc_strcmp(mat_type,"FERT") == 0)	//Anuja,Swati Tendulkar Request for QM View for VC
				{
					//allocate_insp_type_vc();
				}
			}
				printf("\nI am here series........................%s",subString(dml_numAP1, 2, 2));
				printf("\nI am here partrev ........................%s",PrtRevDup);
				printf("\nI am here organization........................%s",OrgIDDup);
				
				if ((tc_strcmp(plantcode,"1001") != 0) && (tc_strcmp(plantcode,"1020" )!= 0) && (tc_strcmp(plantcode,"1500") != 0) && (tc_strcmp(plantcode,"3100") != 0))
				{
					if((tc_strcmp(subString(dml_numAP1, 2, 2),"PP")==0||tc_strcmp(subString(dml_numAP1, 2, 2),"PM")==0) && tc_strcmp(PrtRevDup,"NR")!=0)
					{	flag = 1;}

					if(tc_strcmp(subString(dml_numAP1, 2, 2),"AM")==0 && tc_strcmp(OrgIDDup,"ERC")==0)
					{	flag = 1;}

					if(tc_strcmp(subString(dml_numAP1, 2, 2),"AM")==0 && tc_strcmp(OrgIDDup,"APL")==0 && tc_strcmp(PrtRevDup,"NR")!=0)
					{
						flag = 1;
					}

					if (flag == 1)
					{
						printf("\nGoing for MATERIAL CHANGE");
						mat_create();
						if(tc_strcmp(proc_type, "F") == 0 && tc_strcmp(mat_type, "FERT") != 0)
						{
							printf("\nCreating insp. type during Material Change as proc = F");
							RfcRc = allocate_insp_type();
						}

						if (tc_strcmp(mat_type,"FERT") == 0)	//Anuja,Swati Tendulkar Request for QM View for VC
						{
							//allocate_insp_type_vc();
						}
					}
				}
				else
				{
					printf("\n\n**** plantcode: [%s] Skipping from material change  %s",plantcode,part_noDup); fflush(stdout);
					fprintf(fsuccess,"\n\n**** plantcode: [%s] Skipping from material change  %s",plantcode,part_noDup); fflush(fsuccess);
				}
			}//Part loop
		}//Task Loop
	}//DML Revision Found
	else
	{
		printf("\n\nDML [%s] Not Found in TCUA...Exiting!!",inputDml); fflush(stdout);
		fprintf(fsuccess,"\n\nDML [%s] Not Found in TCUA...Exiting!!",inputDml); fflush(fsuccess);		
	}

	ITK_CALL(POM_logout(false));
	return status;

	CLEANUP:
	ITK_CALL(POM_logout(false));
	printf("\nCLEANUP..."); fflush(stdout);
}
;