#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unidefs.h>
#include <itk/mem.h>
#include <tcinit/tcinit.h>
#include <tccore/item.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ps/ps_errors.h>
#include <tccore/item_errors.h>
#include <tc/emh.h>
#include <ae/dataset.h>
#include <tccore/tctype.h>
#include <bom/bom_tokens.h>
#include <cfm/cfm_item.h>
#include <cfm/cfm_tokens.h>
#include <dispatcher/dispatcher_itk.h>
#include <ps/ps.h>
#include <ai/sample_err.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <bom/bom.h>
#include <sa/sa.h>
#include <stdarg.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <fclasses/tc_string.h>
#include <sa/tcfile.h>
#include <res/reservation.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <sa/imanfile.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <sa/user.h>
#include <unistd.h>
#include <sys/types.h>
#include <property/prop.h>
#include <epm/epm.h>
#include <ae/dataset_msg.h>
#include <tccore/iman_msg.h>
#include <time.h>
#include <tccore/grm_msg.h>
#include <stdlib.h>
#include <string.h>
#include <epm/releasestatus.h>
#include <tccore/aom_prop.h>
#include <tccore/aom.h>
#include <ss/ss_errors.h>
#include <tccore/grm.h>
#include <epm/cr_effectivity.h>
#include<ics/ics.h>
#include<ics/iman_ics.h>
//#include <tcfile_cache.h>

#define Debug TRUE
#define ITK_CALL(X)														\
	if(Debug)															\
	{																	\
		printf(#X);														\
	}																	\
	fflush(NULL);														\
	status=X;															\
	if (status != ITK_ok )												\
	{																	\
		int				index = 0;										\
		int				n_ifails = 0;									\
		const int*		severities = 0;									\
		const int*		ifails = 0;										\
		const char**	texts = NULL;									\
																		\
		EMH_ask_errors( &n_ifails, &severities, &ifails, &texts);		\
		printf("\t%3d error(s)\n", n_ifails);							\
		for( index=0; index<n_ifails; index++)							\
		{																\
			printf("\tError #%d, %s\n", ifails[index], texts[index]);	\
		}																\
		return status;													\
	}																	\
	else																\
	{																	\
		if(Debug)														\
		printf("\tSUCCESS\n");											\
	}																	\

#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}

char* Con_Part_List=NULL;

static int name_attribute, seqno_attribute, parent_attribute, item_tag_attribute;
//static void initialise (void);
static void initialise_attribute (char *name,  int *attribute);
//static void ExpandMultiLevelBom (tag_t bom_line_tag, int depth,FILE *fptr);

void setAddStr(int *count,char ***strset,char* str)
{

	*count=*count+1;
	//printf("\n setAddStr %d",*count);fflush(stdout);
	if(*count==1)
	{
		(*strset) = (char **)malloc((*count ) * sizeof(char *));
	}
	else
	{
		(*strset) = (char **)realloc((*strset),(*count ) * sizeof(char *));
	}
	(*strset)[*count-1] = malloc((strlen(str)+1) * sizeof(char));
	 tc_strcpy((*strset)[*count-1],str);
	 printf("\n setAddStr===%s",(*strset)[*count-1]);fflush(stdout);

}

int setFindStr1(int *count,char ***strset,char* str)
 {
	int j   =0;

	//tmp = sizeof(*strset); 

	for(j=0;j<*count;j++) 
	{
		printf("\n inside setFindStr1 %d \n",*count);fflush(stdout);
		//printf("\n setFindStr1 value is ===%s",(*strset)[*count-1]);fflush(stdout);
		printf("\n setFindStr1 value is ===%s",(*strset)[j]);fflush(stdout);

		//if(tc_strcmp((*strset)[*count-1],str)==0) //check this
		if(tc_strcmp((*strset)[j],str)==0) //check this
		//if(tc_strcmp(*strset[j],str)==0)

		return j;

	}
	return -1;
 }

struct BomChldStrut_EE
{
	tag_t child_objs;//CHild
	tag_t child_objs_bvr;//BVR
	int child_objs_lvl;//LVL
}*get_BomChldStrut_EE;

void Multi_Get_Part_BOM_Lvl_Col(tag_t top_line,int reqLevel,int level,tag_t SnapshotRevTag ,struct BomChldStrut_EE BomChldStrut[],int* StructChldCnt)
{
	int ifail;
    int iChildItemTag=0;
	char * ItemName ;
	int k=0;
    int n_Cnt=0;
    int assbvr=0;
    int flagBVR=0;
	int 	n_values_bvr=0;
	tag_t			*bvr_assy_part= NULLTAG;
	tag_t			bvr_tag= NULLTAG;
	tag_t   t_ChildItemRev;
	tag_t*	childrenTag	= NULLTAG;
	char *view_name=NULL;
	tag_t	window 				= NULLTAG;

	char*			partColInd					=NULL;
	//tag_t	top_line			= NULLTAG;
	int bvrfound=0;
	tag_t  *children=NULLTAG;
	char* partNumber 	= NULL;
	char* PrtCmpCode 	= NULL;

	int iCON =0;											 //18 SEPT
	int y =0;												 //18 SEPT
	char	**CONDetails		  =	NULL;					 //18 SEPT
	CONDetails = (char**)MEM_alloc( 1 * sizeof *CONDetails );//18 SEPT
	char * PartType = NULL;									 //18 SEPT
	char * child_item_id = NULL;							 //18 SEPT

	printf("\n Inside Multi_Get_Part_BOM_Lvl[%d] ...\n",*StructChldCnt);

	if( level >= reqLevel )
	{
		goto CLEANUP;
	}
	
	if (level==0)
	{

	tag_t Modl_rev_tags = NULLTAG;
	ITKCALL(AOM_ask_value_tag(SnapshotRevTag,"topLine",&Modl_rev_tags));
				BomChldStrut[*StructChldCnt].child_objs = top_line;
				BomChldStrut[*StructChldCnt].child_objs_bvr=Modl_rev_tags;
				BomChldStrut[*StructChldCnt].child_objs_lvl=level;

		ITKCALL(BOM_create_window (&window));
		BOM_create_window_from_snapshot(SnapshotRevTag,&window);
		ITKCALL(BOM_set_window_pack_all (window, true));

		BOM_set_window_top_line(window, null_tag,Modl_rev_tags ,null_tag, &top_line);
		BOM_window_show_suppressed(window);//TZ 3.42 Backend

	}

		tag_t objTypeTag = NULLTAG;
	char * partNumberRev =NULL;

	char * SnapNumber =NULL;

	

	ITKCALL(AOM_ask_value_string(SnapshotRevTag,"object_name",&SnapNumber));
	printf("\n Snapshot Name===>%s\n",SnapNumber);fflush(stdout);

	  ITKCALL(AOM_ask_value_string(top_line,"bl_rev_item_revision_id",&partNumberRev));
	printf("\n partNumberRevr===>%s\n",partNumberRev);fflush(stdout);

	ITKCALL(AOM_ask_value_string(top_line,"bl_item_item_id",&partNumber));
    printf("\n Part number===>%s\n",partNumber);fflush(stdout);


		ITKCALL(BOM_line_ask_child_lines (top_line, &n_Cnt, &children));
		printf("\n\n\t\t No of child objects are n : %d\n",n_Cnt);fflush(stdout);

		level = level + 1;
		for (k = 0; k < n_Cnt; k++)
		{
			int MatchFlag=0;																			   //18 SEPT
		
			BOM_line_unpack (children[k]);
			BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
			BOM_line_ask_attribute_tag(children[k], iChildItemTag, &t_ChildItemRev);
			if(t_ChildItemRev!=NULLTAG)
			{
               ITKCALL( AOM_ask_value_string(t_ChildItemRev,"t5_SwPartType",&PartType));                   //18 SEPT
		       printf("\n t_ChildItemRev tag found ===>%s\n",PartType);fflush(stdout);					   //18 SEPT

			   ITKCALL(AOM_ask_value_string(t_ChildItemRev, "item_id", &child_item_id ));				   //18 SEPT
			   printf("\n t_ChildItemRev tag found  child_item_id===>%s\n",child_item_id);fflush(stdout);  //18 SEPT
		       if(tc_strcmp(PartType,"CON")==0)                                                            //18 SEPT
				{
					//int ww=0;
					char* containerMatch=NULL;
					for (y=0;y<*StructChldCnt+1;y++)													   //18 SEPT
					{
						printf("\n..IN Expansion_TPL_16 structure.. :%d\n",y);fflush(stdout);			   //18 SEPT
						containerMatch=NULL;															   //18 SEPT
						
						tag_t ChildRevBomLine=NULLTAG;
				
						if( AOM_ask_value_string(BomChldStrut[y].child_objs,"item_id",&containerMatch)!=ITK_ok);//18 SEPT
						if (tc_strcmp(containerMatch,child_item_id)==0)											//18 SEPT
						{
							printf("\n..CONTAINER MATCH FOUND  :\n");fflush(stdout);
							MatchFlag=1;																	    //18 SEPT
							break;
						
						}
					}																						    //18 SEPT
																					
				}
				if (MatchFlag==0)																			    //18 SEPT
				{
					printf("\n..match not found.hence proceeding \n");fflush(stdout);
					*StructChldCnt	=	*StructChldCnt+1;
					//get_BomChldStrut[*StructChldCnt].child_objs = childrenTag[k];
					BomChldStrut[*StructChldCnt].child_objs = t_ChildItemRev;
					BomChldStrut[*StructChldCnt].child_objs_bvr=children[k];
					BomChldStrut[*StructChldCnt].child_objs_lvl=level;

					Multi_Get_Part_BOM_Lvl_Col(children[k],reqLevel,level,SnapshotRevTag,BomChldStrut,StructChldCnt);
				}

			}
		}
		level = level - 1;
	

	MEM_free (childrenTag);

	CLEANUP:
		 printf("\n Inside Multi_Get_Part_BOM_Lvl CLEANUP");fflush(stdout);
}



FILE *fptr;
FILE *fptr1;
FILE *fptr2;
FILE *fptr3;
FILE *fptr4;
FILE *fptr5;
FILE *fptr6;
FILE *fptr7;
FILE *fptr8;
FILE *fptr9;

char	**HWCList			=	NULL;
char	**CALList			=	NULL;
char	**PBLList			=	NULL;
char	**SBLList			=	NULL;
char	**BASList			=	NULL;
char	**APPList			=	NULL;
char	**STKList			=	NULL;
char	**VCIList			=	NULL;
char	**CFGList			=	NULL;
char	**PRMList			=	NULL;
char	**CONList			=	NULL;
char	**ParaList			=	NULL;
char	**ConatainerTypeListX=	NULL;
char	**ConatainerList	=   NULL;
char	**ConatainerRev		=   NULL;	
char	*DML_name			=   NULL;
char	* VCnumber			=   NULL;
char	* VCnumberDup		=   NULL;
char	* req_item			=   NULL;
char	*	t5CountryValDup =   NULL;
char	*	t5MajorModelDup =   NULL;
char	*	t5NoEcuDup		=   NULL;
char	*	t5ABSValDup		=   NULL;
char	*	Noval			=  "0";
int     iWriteRPar			=   0;
int     iWriteRCon			=   0;
int     iWriteRConType		=   0;
int     iWriteRConrev		=   0;
int		supfile				=   0;
int		calfilecnt			=   0;
tag_t	DMLRevTag			= NULLTAG;
char *req_item2				= NULL;
char *req_item3				= NULL;
 tag_t	Mod_rev_tag				= NULLTAG;
 tag_t	LatestModRevTag				= NULLTAG;



static void initialise (void);
//static void initialise_attribute (char *name,  int *attribute);



void getCurrentDateTimeA(char* CurrentDate)
{ 
    time_t temp ; 
    struct tm *timeptr ; 
    char pAccessDate[30]; 

	CurrentDate		=(char *) MEM_alloc(30);
    time( &temp ); 
      
    tc_strcpy(pAccessDate,"");
    timeptr = localtime( &temp ); 

    strftime(pAccessDate, sizeof(pAccessDate), "%x - %I:%M%p", timeptr); 
      
    //printf("Formatted date & time : %s\n", pAccessDate ); 
	tc_strcpy(CurrentDate,pAccessDate);
	printf("\n CurrentDate AAAAAAA :%s\n",CurrentDate);
   // return(0); 
} 




char* subString (char* mainStringf ,int fromCharf,int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(200);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}

void strip(char *s) 
{ 
	char *p2 = s; 
	while(*s != '\0')
	{
		if(*s != '\t' && *s != '\n'  && *s != '\r') 
		{
			*p2++ = *s++; 
		} else
		{
			++s; 
		} 
	} 
	*p2 = '\0';
} 

extern int ITK_user_main (int argc, char ** argv )
{
	int ifail;
	int status;
	int iStatus=0;

	int		count_tsk				= 0;
	int		pp						= 0;
	int		rr						= 0;
	int		aa						= 0;
	int  	n_parents				= 0;
	int  	count_DML				= 0;
	int  	*levels				    = 0;
	int  	VCcnt				    = 0;
	int  	Modcnt				    = 0;

	char *req_item2Dup				= NULL;
	char *req_item2DupS				= NULL;
	char *inputfile					= NULL;
	char *outputfile				= NULL;
	char *partnumDup				= NULL;				
	char *object_type				= NULL;
	char *RevIDNo					= NULL;
	char *PartNo					= NULL;
	char *type_name					= NULL;
	char *task_name					= NULL;
	char *type_dml_name				= NULL;
	char *Report_Name				= NULL;
	char *Proj_Details				= NULL;
	char *VehicleC_Details			= NULL;
	char *Report_Para				= NULL;
	char *Report_Struct				= NULL;
	char *File_List					= NULL;
	char *Support_Master			= NULL;
	char *User_File					= NULL;
	char *VC_Details				= NULL;
	char *Assy_obj					= NULL;
	char *Assy_Rev					= NULL;
	char *Objdesc					= NULL;
	//char *VCnumber				= NULL;
	char *VCnumberRev				= NULL;
	char *VCnumberRevDup			= NULL;
	char *ProjDup					= NULL;
	char *Proj						= NULL;
	char *PartTypeM					= NULL;
	char *Techspecobjectdesc		= NULL;
	char *ModuleNoDup				= NULL;
	char *Modnumber					= NULL;
	char *vcDrStat					= NULL;
	char *vcDrStatDup				= NULL;
	char *vcProj					= NULL;
	char *vcProjDup					= NULL;
	char *MRevIDNo					= NULL;
	char *VCVal						= "NA";
	char *nwRev						= NULL;
	char*   NoObjDes				= "";

	tag_t window, rule, item_tag = null_tag, top_line;

	tag_t   LatestRev				= NULLTAG;
	tag_t   LatestModRevTag			= NULLTAG;
    tag_t	tsk_part_sol_rel_type	= NULLTAG;
    tag_t	techspec_rel_type		= NULLTAG;
    tag_t	Design_rev_tag			= NULLTAG;
    tag_t	Main_CCID_rev_tag		= NULLTAG;
    tag_t	LatestRevTag			= NULLTAG;
	tag_t	objTaskTypeTag			= NULLTAG;
	tag_t	objDMLTypeTag			= NULLTAG;
	tag_t	TaskRevTag				= NULLTAG;
	tag_t	AssyTag					= NULLTAG;
	tag_t	dml_task_rel_type		= NULLTAG;
	tag_t	CCVCrelation_type       = NULLTAG;
	tag_t   *VCattachments			= NULLTAG;
	tag_t   *Modattachments			= NULLTAG;
	tag_t Mod_rev_tagP;
	tag_t	ReferencesCCVTag		= NULLTAG;
	tag_t	ProQryCCIDObj			= NULLTAG;

	tag_t	*TaskRevision			= NULLTAG;
	tag_t   *DMLRevision            = NULLTAG;
	tag_t	*parents				= NULLTAG;
	tag_t	*CCIDObj				= NULL;

	int		n_Input					= 3;
	int		CCIDCount				= 0;
	int		n_tags_found			= 0;
	int		n_tags_found1			= 0;
	int		desccnt					= 0;
	int		Objdesclength		    = 0;
	int		ccv						= 0;
	char	*cRplStr				= NULL;
	char	*ObjDescFinal				= NULL;
	char	*qry_Input[3]			= {"Item ID", "Type", "Revision"};
	char	*values[3];
	tag_t   CCIDRev					= NULLTAG;
	tag_t	*tags_found				= NULL;
	tag_t	*tags_foundX			= NULL;


	(void)argc;
	(void)argv;

	initialise();

	req_item  = ITK_ask_cli_argument("-i=");// Baseline number  -i=5442E4Z01016_01 -r='A'
	req_item2 = ITK_ask_cli_argument("-r=");// revision 'A' LATER ON 'A;1'
	//req_item3 = ITK_ask_cli_argument("-s=");// sequence 1

	iStatus = ITK_auto_login();

	//if( ITK_init_module("ercpsup" ,"ERCpsup2019","Engineering")!=ITK_ok) ;
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_set_journalling( TRUE ));


	printf("\n BASELINE NUMBER & REVISION ------> [%s][%s]  \n",req_item,req_item2);fflush(stdout);

	inputfile=(char *) MEM_alloc(50);

	Report_Name		=(char *) MEM_alloc(150);
	Report_Para		=(char *) MEM_alloc(150);
	Report_Struct	=(char *) MEM_alloc(150);
	File_List		=(char *) MEM_alloc(150);
	Support_Master	=(char *) MEM_alloc(150);
	User_File		=(char *) MEM_alloc(150);
	VC_Details		=(char *) MEM_alloc(150);
	Proj_Details	=(char *) MEM_alloc(150);
	VehicleC_Details	=(char *) MEM_alloc(150);

	t5CountryValDup	=(char *) MEM_alloc(150);
	t5MajorModelDup	=(char *) MEM_alloc(150);
	t5NoEcuDup		=(char *) MEM_alloc(150);
	t5ABSValDup		=(char *) MEM_alloc(150);
	nwRev		=(char *) MEM_alloc(10);

	ParaList			= (char**)MEM_alloc( 1 * sizeof *ParaList );
	ConatainerTypeListX = (char**)MEM_alloc( 5 * sizeof *ConatainerTypeListX );
	ConatainerList		= (char**)MEM_alloc( 1 * sizeof *ConatainerList );
	ConatainerRev		= (char**)MEM_alloc( 1 * sizeof *ConatainerRev );

//fprintf(fptr6,"\n Vc No,Desc ,Part_No,Part_Rev_Seq,Cal_Part,Cal File Name,Cal_Rev,Cal_Seq,ECU Type,HW_Part,HW_Rev,HW_Seq,VEH_Part,VEH_Rev,VEH_Seq,CFG_Part,CFG_Rev,CFG_Seq,APP_Part,APP_Rev,APP_Seq,PBL_Part,PBL_Rev,PBL_Seq,SBL_Part,SBL_Rev,SBL_Seq,No_Of_ECU\n");
	req_item2Dup=(char *) MEM_alloc(20);
	tc_strcpy(req_item2Dup,req_item2);//A;1
	printf("\n req_item2Dup== %s", req_item2Dup);fflush(stdout);

	req_item2DupS = tc_strtok(req_item2Dup,";");//A
	req_item3 =tc_strtok(NULL,"");//1


	printf("\n req_item2DupS== %s", req_item2DupS);fflush(stdout);
	printf("\n req_item3== %s", req_item3);fflush(stdout);

	tc_strcpy(nwRev,"");
	//strcat(nwRev,req_item2Dup);
	tc_strcat(nwRev,req_item2DupS);
	tc_strcat(nwRev,"*");
	tc_strcat(nwRev,req_item3);
	printf("\nAAAAAAAAA  nwRev:::::::: = %s\n", nwRev);fflush(stdout);

	if ( req_item )
	 {
		printf("\n nwRev:::::::: = %s", nwRev);fflush(stdout);
		values[0] = req_item;
		values[1] = "CCID Revision";
		values[2] = nwRev;

		if(QRY_find2("Item Revision...", &ProQryCCIDObj));
		if(ProQryCCIDObj)
		{
			 if(QRY_execute(ProQryCCIDObj, n_Input, qry_Input, values, &CCIDCount, &CCIDObj));
			 printf("\n qUERY for CCID count== %d", CCIDCount);fflush(stdout);
		}

		if (CCIDCount == 0)
		{
			printf ("\n XXXXXXX CCIDCount ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", req_item); fflush(stdout);
			exit (0);
		}

		if(CCIDCount>0)
		{			
			Main_CCID_rev_tag = CCIDObj[0];

			//item_tag = tags_found[0];
			//ITK_CALL(ITEM_ask_latest_rev(Main_CCID_rev_tag,&LatestRevTag));

			//ITK_CALL(ITEM_find_rev(req_item,req_item2,&CCIDRev));

			//printf("\n  CCIDRev is ------------- [%s]\n",CCIDRev);fflush(stdout);

			Proj=subString(req_item,0,4); 
			printf("\n  Proj is ------------- [%s]\n",Proj);fflush(stdout);
		
			ProjDup=(char *) MEM_alloc(10);
			tc_strcpy(ProjDup,Proj);
			printf("\n  ProjDup ------------- [%s]\n",ProjDup); fflush(stdout);	
			 		   
			ITK_CALL(GRM_find_relation_type("T5_ReferencesCCV",&ReferencesCCVTag));
			printf("\n After ReferencesCCVTag\n");fflush(stdout);

			ITK_CALL(GRM_list_secondary_objects_only(Main_CCID_rev_tag,ReferencesCCVTag,&VCcnt,&VCattachments));
			printf("\n VCcnt is ============= [%d]  \n",VCcnt); fflush(stdout);	

			
			Report_Name		=(char *) MEM_alloc(150);
			Report_Para		=(char *) MEM_alloc(150);
			Report_Struct	=(char *) MEM_alloc(150);
			File_List		=(char *) MEM_alloc(150);
			Support_Master	=(char *) MEM_alloc(150);
			User_File		=(char *) MEM_alloc(150);
			VC_Details		=(char *) MEM_alloc(150);
			Proj_Details	=(char *) MEM_alloc(150);
			VehicleC_Details	=(char *) MEM_alloc(150);

			if (VCcnt > 0)
			  {
				tc_strcpy(VehicleC_Details,"/tmp/");
				tc_strcat(VehicleC_Details,req_item);
				//tc_strcat(VehicleC_Details,VCnumberDup);
				tc_strcat(VehicleC_Details,"_VCCRList.csv");

				printf("\n after VCCRList.csv  \n");fflush(stdout);
				fptr9 = fopen(VehicleC_Details,"w"); // to handle multiple VC's
	
				for (ccv=0;ccv<VCcnt;ccv++ )
				{
				 //Design_rev_tag = VCattachments[0];
				 Design_rev_tag = VCattachments[ccv];
				 VCnumberDup=(char *) MEM_alloc(50);
				 vcProjDup=(char *) MEM_alloc(10);
				 vcDrStatDup=(char *) MEM_alloc(10);

				 ITK_CALL(ITEM_ask_latest_rev(Design_rev_tag,&LatestRev));
				 if( AOM_ask_value_string(LatestRev,"item_id",&VCnumber)!=ITK_ok);
				 tc_strcpy(VCnumberDup,VCnumber);
				 printf("\n VCnumberDup is [%s]  \n",VCnumberDup); fflush(stdout);

				 if( AOM_ask_value_string(LatestRev,"t5_PartStatus",&vcDrStat)!=ITK_ok);
				 tc_strcpy(vcDrStatDup,vcDrStat);
				 printf("\n vcDrStatDup is [%s]  \n",vcDrStatDup); fflush(stdout);

				 if( AOM_ask_value_string(LatestRev,"t5_ProjectCode",&vcProj)!=ITK_ok);
				 tc_strcpy(vcProjDup,vcProj);
				 printf("\n vcProjDup is [%s]  \n",vcProjDup); fflush(stdout);

				 ITK_CALL(AOM_ask_value_string(LatestRev,"object_desc",&Objdesc));
				 printf("\n Object_desc = [%s]\n",object_type);fflush(stdout);

				 ITK_CALL(AOM_ask_value_string(LatestRev,"item_revision_id",&RevIDNo));
				 printf("\n RevIDNo is  = [%s]\n", RevIDNo);fflush(stdout);


				if(VCnumberDup)
				  {
					printf("\n CREATING CSV  \n");fflush(stdout);
					tc_strcpy(Report_Name,"/tmp/");
					tc_strcat(Report_Name,VCnumberDup);
					tc_strcat(Report_Name,"_VC.csv");//not for grpid

					printf("\n CREATING para CSV  \n");fflush(stdout);

					tc_strcpy(Report_Para,"/tmp/");
					tc_strcat(Report_Para,VCnumberDup);
					tc_strcat(Report_Para,"_Para.csv");

					tc_strcpy(Report_Struct,"/tmp/");
					tc_strcat(Report_Struct,VCnumberDup);
					tc_strcat(Report_Struct,"_Struct.csv");

					tc_strcpy(File_List,"/tmp/");
					tc_strcat(File_List,VCnumberDup);
					tc_strcat(File_List,"_Files.csv");

					tc_strcpy(Support_Master,"/tmp/");
					tc_strcat(Support_Master,VCnumberDup);
					tc_strcat(Support_Master,"_SuppMstr.csv");

					tc_strcpy(User_File,"/tmp/");
					tc_strcat(User_File,VCnumberDup);
					tc_strcat(User_File,"_Summary.csv");

					tc_strcpy(VC_Details,"/tmp/");
					tc_strcat(VC_Details,VCnumberDup);
					tc_strcat(VC_Details,"_VCList.csv");

					printf("\n CREATING projlist CSV  \n");fflush(stdout);

					tc_strcpy(Proj_Details,"/tmp/");
					tc_strcat(Proj_Details,VCnumberDup);
					tc_strcat(Proj_Details,"_ProjList.csv");



				
					fptr1 = fopen(Report_Name,"w");
					fptr2 = fopen(Report_Para,"w");
					fptr3 = fopen(Report_Struct,"w");
					fptr4 = fopen(File_List,"w");
					fptr5 = fopen(Support_Master,"w");
					fptr6 = fopen(User_File,"w");
					fptr7 = fopen(VC_Details,"w");
					fptr8 = fopen(Proj_Details,"w");


					printf("\n after fptr9  \n");fflush(stdout);
				  }

				  printf("\n before VCnumberDup  fptr9  \n");fflush(stdout); 
				  fprintf(fptr9,"%s\n",VCnumberDup);fflush(fptr9);
	
				 fprintf(fptr8,"%s\n",ProjDup);fflush(fptr8);

//				 printf("\n before VCnumberDup  fptr9  \n");fflush(stdout); 
//				 fprintf(fptr9,"%s\n",VCnumberDup);fflush(fptr9);

				 tag_t	SnapshotRevTag	= NULLTAG;
				 tag_t	HasSnapshot_type	= NULLTAG;
				 tag_t*	Snapshotattachments	= NULL;
				 int Snapcnt=0;

				 ifail = GRM_find_relation_type("T5_CCIDHasSnapshot",&HasSnapshot_type); 
				 printf("\n  Expansion_TPL_16 after snapshot rel \n");fflush(stdout);

				 ifail = GRM_list_secondary_objects_only(Main_CCID_rev_tag,HasSnapshot_type,&Snapcnt,&Snapshotattachments);
				 printf("\n  Snapcnt [%d] -------> \n",Snapcnt);fflush(stdout);
		

				 if (Snapcnt > 0)
				 {
					//Mod_rev_tag = Modattachments[0];
					//ModuleNoDup=(char *) MEM_alloc(50);
					//if( AOM_ask_value_string(Mod_rev_tag,"item_id",&Modnumber)!=ITK_ok);

					//ITEM_ask_item_of_rev(Mod_rev_tag,&Mod_rev_tagP); 	
					//ITEM_ask_item_of_rev(Mod_rev_tag,&LatestModRevTag); 

					SnapshotRevTag=Snapshotattachments[0];
					//Mod_rev_tag = Modattachments[0];
					 ITKCALL(AOM_ask_value_tag(SnapshotRevTag,"topLine",&LatestModRevTag));// THIS is the module based on which SS was create
									
					ITK_CALL(AOM_ask_value_string(LatestModRevTag,"item_id",&partnumDup));
					printf("\n partnumDup is   = [%s]\n", partnumDup);fflush(stdout);

					ITK_CALL(AOM_ask_value_string(LatestModRevTag,"item_revision_id",&MRevIDNo));
					printf("\n MRevIDNo is  = [%s]\n", MRevIDNo);fflush(stdout);
					 
					ITK_CALL(PS_where_used_all(LatestModRevTag,1,&n_parents,&levels,&parents));
					printf("\n Part attached to  count : %d\n",n_parents); fflush(stdout);

					if(n_parents>0)
					  {
						for (aa=0;aa<n_parents;aa++ )
						  {
							if(AssyTag) AssyTag=NULLTAG;
							AssyTag=parents[aa];

							ITK_CALL(AOM_ask_value_string(AssyTag,"item_id",&Assy_obj));
							printf("\n\t parent ID  is :%s",Assy_obj);fflush(stdout);

							ITK_CALL(AOM_ask_value_string(AssyTag,"item_revision_id",&Assy_Rev));
							printf("\n\t parent REV ID  is :%s",Assy_Rev);fflush(stdout);

//							char	*strToBereplaced;  //14aug changes
//							char	*strToBereplaced1;
//							char	*strToBereplaced2;
//							char	*strToBeUsedInstead;
//			
//							strToBereplaced		=	(char*)malloc(5);
//							tc_strcpy(strToBereplaced,",");
//							//strToBereplaced1	=	(char*)malloc(5);
//							//tc_strcpy(strToBereplaced1,"\n");
//						    //strToBereplaced2	=	(char*)malloc(5);
//						    //tc_strcpy(strToBereplaced2,"'");
//  
//							strToBeUsedInstead	=	(char*)malloc(5);
//							tc_strcpy(strToBeUsedInstead," ");
//
//		//					Objdesc=low_strssra(Objdesc,","," "); 
//		//					Objdesc=low_strssra(Objdesc,"\n"," "); 
//		//					Objdesc=low_strssra(Objdesc,"'"," ");  
//
//							ITK_CALL(STRNG_replace_str (Objdesc,strToBereplaced,strToBeUsedInstead,&ObjDescFinal));
//							//ITK_CALL(STRNG_replace_str (Objdesc,strToBereplaced1,strToBeUsedInstead,&ObjDescFinal));
//							//ITK_CALL(STRNG_replace_str (Objdesc,strToBereplaced2,strToBeUsedInstead,&ObjDescFinal));	 //14aug changes

							if(tc_strcmp(Objdesc,"")!=0) //14aug changes
							{
								Objdesclength = tc_strlen(Objdesc);
								for(desccnt=0;desccnt<Objdesclength;desccnt++)
								{
									if(Objdesc[desccnt]=='\n')
										Objdesc[desccnt] =' ';

									if(Objdesc[desccnt]=='`')
										Objdesc[desccnt] =' ';

									if(Objdesc[desccnt]=='~')
										Objdesc[desccnt] =' ';

									if(Objdesc[desccnt]=='|')
										Objdesc[desccnt] =' ';

									if(Objdesc[desccnt]=='&')
										Objdesc[desccnt] ='-';
//
//									if(Objdesc[desccnt]=='#')
//										Objdesc[desccnt] =' ';
//
//									if(Objdesc[desccnt]=='@')
//										Objdesc[desccnt] =' ';

									if(Objdesc[desccnt]=='^')
										Objdesc[desccnt] =' ';

									if(Objdesc[desccnt]=='!')
										Objdesc[desccnt] =' ';

									if(Objdesc[desccnt]==',')
										Objdesc[desccnt] =' ';
									printf("\n [%c]\n",Objdesc[desccnt]);fflush(stdout);
								}
								//val=(char **)MEM_alloc(sizeof(char **));
								ObjDescFinal = (char *)MEM_alloc (Objdesclength * sizeof(char));
								tc_strcpy(ObjDescFinal,Objdesc);
								printf("\n ObjDescFinal is----------------- = [%s]\n",ObjDescFinal);fflush(stdout);
							}//14aug changes
							else
							{
								  ObjDescFinal=(char *) MEM_alloc(100);
								  tc_strcpy(ObjDescFinal,NoObjDes);
								  printf("\n else else ObjDescFinal is --------- [%s]\n",ObjDescFinal); fflush(stdout);
							}

						   } 
				  }

	//				============================= TECH SPEC HEADER =============================

//						tc_strcpy(t5CountryValDup,"NA");	 // 152  //8152
//						tc_strcpy(t5MajorModelDup,"X445");   // 1    //8001
//						tc_strcpy(t5NoEcuDup,"8");			 //226   //8226
//						tc_strcpy(t5ABSValDup,"DAB+PAB+ABS");// 69   //8069					

							char *AttrKeyValue=NULL;
							char *AttrKeyValue1=NULL;
							char *AttrKeyValueX=NULL;
							char *TechspecID=NULL;
							tag_t objTypeTag=NULLTAG;
							tag_t IcsClsTag=NULLTAG;
							char   Ptype_name[TCTYPE_name_size_c+1];
							int TScnt=0;
							int CLASScnt=0;
							tag_t*	TSattachments	= NULL;
							tag_t*	ClsObjTag	= NULLTAG;
							tag_t	techspec_rel_type	= NULLTAG;
							char *clsKeyAtrrId= NULL;
							char 			*ctrObjAttrid			="8069";
							int j=0,k=0,p=0,i=0,ic=0,id=0,ie=0;
							int  	chkflag =0,Flag=0;
							int status;


							int *InputIdsArr1;
							char **InputValuesArr1=NULL;
							char **StrConcatenatedMaster=NULL;
							char * 	StrConcatenatedinput=NULL;


							char * 	theClassId =NULL;
							int  	theAttributeCount;
							int * 	theAttributeIds;
							int * 	theAttributeValCounts;
							char ** 	theAttributeValues;
							printf("\n  input clsKeyAtrrId[%s]",clsKeyAtrrId); fflush(stdout);
							char keyPrefix[10];
							tc_strcpy(keyPrefix,"-");
							tc_strcat(keyPrefix,clsKeyAtrrId);
							printf("\n  input keyPrefix[%s]",keyPrefix); fflush(stdout);
							const char * 	key_lov_id=keyPrefix; //cls attr id for ChassisTypeNo_TYP_APPRVL_NO
							char * 	key_lov_name;
							char * 	key_lov_name1;
							char * 	key_lov_name2;
							char * 	key_lov_name3;
							int  	options;
							int  	options1;
							int  	options2;
							int  	options3;
							//int i;
							int keyfound=0;
							int  	n_lov_entries;
							int  	n_lov_entries1;
							int  	n_lov_entries2;
							int  	n_lov_entries3;
							char ** 	lov_keys;
							char ** 	lov_keys1;
							char ** 	lov_keys3;
							char ** 	lov_keys2;
							char ** 	lov_values;
							char ** 	lov_values1;
							char ** 	lov_values2;
							char ** 	lov_values3;
							logical * 	deprecated_staus;
							logical * 	deprecated_staus1;
							logical * 	deprecated_staus2;
							logical * 	deprecated_staus3;
							char * 	owning_site;
							char * 	owning_site1;
							char * 	owning_site2;
							char * 	owning_site3;
							char *keyLovOfCCVNo=(char*) MEM_alloc( 1 * sizeof(lov_keys) );
							int  	n_shared_sites;
							int  	n_shared_sites1;
							int  	n_shared_sites2;
							int  	n_shared_sites3;
							char ** 	shared_sites ;
							char ** 	shared_sites1 ;
							char ** 	shared_sites2 ;
							char ** 	shared_sites3 ;
							//theICOTag=IcsClsTag;

							tag_t Design_rev_cls_tag = NULLTAG;
							tag_t Design_rev_cls_tag1 = NULLTAG;
							tag_t Design_rev_cls_tag2 = NULLTAG;
							tag_t Design_rev_cls_tag3 = NULLTAG;
							 char* attributeValue=NULL;
							 char* attributeValue1=NULL;
							 char* attributeValue2=NULL;
							 char* attributeValue3=NULL;


							tag_t TagQryContObj = NULLTAG,*cntr_objects = NULL;
							//int	n_entries = 2;
							//int n_found = 0;
							//char* CCA_ParentCompCode =NULL;
							printf("\n  classification \n");fflush(stdout);

					        ITKCALL(AOM_ask_value_tags(Design_rev_tag,"IMAN_classification",&CLASScnt,&ClsObjTag));

						    printf("\n techspec found \n ");fflush(stdout);
  													 
							//ITKCALL(TCTYPE_ask_object_type(IcsClsTag,&objTypeTag)); //T5_TSCompRevision
							//ITKCALL(TCTYPE_ask_name(objTypeTag,Ptype_name));
							//printf("\n  IcsClsTag  Ptype_name :: %s\n", Ptype_name); fflush(stdout);

							ITKCALL(ICS_ask_attribute_value(*ClsObjTag,ctrObjAttrid,&AttrKeyValue));
							printf("\n AttrKeyValue MAJOR MODEL  IS =%s ",AttrKeyValue); fflush(stdout);

							//ITKCALL(ICS_ask_attribute_value(*ClsObjTag,ctrObjAttrid1,&AttrKeyValueX));
							//printf("\n  NO OF ECU IS =%s ",AttrKeyValue1); fflush(stdout);

							// ITKCALL(ICS_ask_attribute_value(*ClsObjTag,"[VC]MAJORMODEL",&AttrKeyValue1));
							// printf("\n AttrKeyValue MAJOR MODEL 222 =%s ",AttrKeyValueX); fflush(stdout);

							//printf("\n AttrKeyValue=%s key_lov_id=%s",AttrKeyValue,key_lov_id); fflush(stdout);
							//if(AttrKeyValue)
							//{
							
							 ITK_CALL(ICS_keylov_get_keylov("-8226",&key_lov_name,&options,&n_lov_entries,&lov_keys,&lov_values,&deprecated_staus,&owning_site,&n_shared_sites,&shared_sites));
							 //CALLAPI(ICS_keylov_get_keylov(key_lov_id,&key_lov_name,&options,&n_lov_entries,&lov_keys,&lov_values,&deprecated_staus,&owning_site,&n_shared_sites,&shared_sites));
							 printf("\n key_lov_name=%s n_lov_entries=%d options=%d",key_lov_name,n_lov_entries,options); fflush(stdout);
							 for(i=0;i<n_lov_entries;i++)								
								{
								  //printf("\n lov_keys=%s lov_values=%s",lov_keys[i],lov_values[i]); fflush(stdout);

								    (ICS_ask_classification_object(Design_rev_tag,&Design_rev_cls_tag));

									// ITK_CALL( ICS_ask_attribute_value	(Design_rev_cls_tag,"[VC]MAJORMODEL",&attributeValue));
								    (ICS_ask_attribute_value	(Design_rev_cls_tag,"8226",&attributeValue));

									//printf("\n attributeValue: %s ",attributeValue); fflush(stdout);
									
									if(tc_strcmp(attributeValue,lov_keys[i])==0)
									{
										printf("\n Value of classified object: \n Mascot lov_keys=%s lov_values=%s",lov_keys[i],lov_values[i]); fflush(stdout);

										t5NoEcuDup		   =(char *) MEM_alloc(20);
										tc_strcpy(t5NoEcuDup,lov_values[i]);

										printf("\n ECU COUNT IS  =%s",t5NoEcuDup); fflush(stdout);
										
									}									
								}

							ITK_CALL(ICS_keylov_get_keylov("-8001",&key_lov_name1,&options1,&n_lov_entries1,&lov_keys1,&lov_values1,&deprecated_staus1,&owning_site1,&n_shared_sites1,&shared_sites1));
							 //CALLAPI(ICS_keylov_get_keylov(key_lov_id,&key_lov_name,&options,&n_lov_entries,&lov_keys,&lov_values,&deprecated_staus,&owning_site,&n_shared_sites,&shared_sites));
							 printf("\n key_lov_name1=%s n_lov_entries1=%d options1=%d",key_lov_name1,n_lov_entries1,options1); fflush(stdout);
							 for(ic=0;ic<n_lov_entries1;ic++)								
								{
								    (ICS_ask_classification_object(Design_rev_tag,&Design_rev_cls_tag1));

								    (ICS_ask_attribute_value	(Design_rev_cls_tag1,"8001",&attributeValue1));
									
									if(tc_strcmp(attributeValue1,lov_keys1[ic])==0)
									{
										printf("\n Value of classified object: \n Mascot lov_keys=%s lov_values=%s",lov_keys1[ic],lov_values1[ic]); fflush(stdout);

										t5MajorModelDup		   =(char *) MEM_alloc(25);
										tc_strcpy(t5MajorModelDup,lov_values1[ic]);

										printf("\n t5MajorModelDup IS  =%s",t5MajorModelDup); fflush(stdout);
										
									}									
								}

								
							ITK_CALL(ICS_keylov_get_keylov("-8069",&key_lov_name2,&options2,&n_lov_entries2,&lov_keys2,&lov_values2,&deprecated_staus2,&owning_site2,&n_shared_sites2,&shared_sites2));
							 //CALLAPI(ICS_keylov_get_keylov(key_lov_id,&key_lov_name,&options,&n_lov_entries,&lov_keys,&lov_values,&deprecated_staus,&owning_site,&n_shared_sites,&shared_sites));
							 printf("\n key_lov_name2=%s n_lov_entries2=%d options2=%d",key_lov_name2,n_lov_entries2,options2); fflush(stdout);
							 for(id=0;id<n_lov_entries2;id++)								
								{
								    (ICS_ask_classification_object(Design_rev_tag,&Design_rev_cls_tag2));

								    (ICS_ask_attribute_value	(Design_rev_cls_tag,"8069",&attributeValue2));

									if(tc_strcmp(attributeValue2,lov_keys2[id])==0)
									{
										printf("\n Value of classified object: \n Mascot lov_keys=%s lov_values2=%s",lov_keys2[id],lov_values2[id]); fflush(stdout);

										t5ABSValDup		   =(char *) MEM_alloc(25);
										tc_strcpy(t5ABSValDup,lov_values2[id]);

										printf("\n t5ABSValDup COUNT IS  =%s",t5ABSValDup); fflush(stdout);										
									}									
								}

								
							ITK_CALL(ICS_keylov_get_keylov("-8152",&key_lov_name3,&options3,&n_lov_entries3,&lov_keys3,&lov_values3,&deprecated_staus3,&owning_site3,&n_shared_sites3,&shared_sites3));
							 //CALLAPI(ICS_keylov_get_keylov(key_lov_id,&key_lov_name,&options,&n_lov_entries,&lov_keys,&lov_values,&deprecated_staus,&owning_site,&n_shared_sites,&shared_sites));
							 printf("\n key_lov_name3=%s n_lov_entries3=%d options3=%d",key_lov_name3,n_lov_entries3,options3); fflush(stdout);
							 for(ie=0;ie<n_lov_entries3;ie++)								
								{
								    (ICS_ask_classification_object(Design_rev_tag,&Design_rev_cls_tag3));

									// ITK_CALL( ICS_ask_attribute_value	(Design_rev_cls_tag,"[VC]MAJORMODEL",&attributeValue));
								    (ICS_ask_attribute_value	(Design_rev_cls_tag,"8152",&attributeValue3));
									
									if(tc_strcmp(attributeValue3,lov_keys3[ie])==0)
									{
										printf("\n Value of classified object: \n Mascot lov_keys=%s lov_values=%s",lov_keys3[ie],lov_values3[ie]); fflush(stdout);

										t5CountryValDup		   =(char *) MEM_alloc(25);
										tc_strcpy(t5CountryValDup,lov_values3[ie]);

										printf("\n t5CountryValDup COUNT IS  =%s",t5CountryValDup); fflush(stdout);
										
									}									
								}
							//}


//						tc_strcpy(t5CountryValDup,"NA");
//						tc_strcpy(t5MajorModelDup,"X445");
//						tc_strcpy(t5NoEcuDup,"11");
//						tc_strcpy(t5ABSValDup,"DAB+PAB+ABS");
	

	//				============================= TECH SPEC HEADER ==============================
			
					ITKCALL(GRM_find_relation_type("CMHasSolutionItem",&tsk_part_sol_rel_type));  

					if(tsk_part_sol_rel_type!=NULLTAG)
					  {
						 printf("\n task found \n ");fflush(stdout);

						 //GRM_list_primary_objects_only(Design_rev_tag,tsk_part_sol_rel_type,&count_tsk,&TaskRevision);
						 GRM_list_primary_objects_only(LatestModRevTag,tsk_part_sol_rel_type,&count_tsk,&TaskRevision);
						 printf("\n  NO OF TASKS FOUND ----------- %d",count_tsk);fflush(stdout);

						 if(count_tsk>0)
						  {
							 for(pp=0;pp<count_tsk;pp++)
							  {

								TaskRevTag	=	NULLTAG;
								TaskRevTag	=	TaskRevision[pp];

								if(TCTYPE_ask_object_type(TaskRevTag,&objTaskTypeTag));
								if(TCTYPE_ask_name2(objTaskTypeTag,&type_name));
								printf("\n     type_name for object     %s \n", type_name);fflush(stdout);
								
								if(tc_strcmp(type_name,"T5_ChangeTaskRevision")==0)
								 { 
									//printf("\n\t object_type is :%s",task_name);fflush(stdout);
									printf("\n     pp for object     %d  \n", pp);fflush(stdout);
									//ITKCALL(AOM_ask_value_string(TaskRevTag,"item_id",&task_name));
									AOM_ask_value_string(TaskRevTag,"object_name",&task_name);
									printf("\n\t object_type is :%s",task_name);fflush(stdout);

									if(tc_strstr(task_name,"PP")!=NULL || tc_strstr(task_name,"PM")!=NULL)
									  {
										 printf("\n PP or PM Task found ");fflush(stdout);

										 ITKCALL(GRM_find_relation_type("T5_DMLTaskRelation",&dml_task_rel_type));
										 if(dml_task_rel_type!=NULLTAG)
										  {
											 ITKCALL(GRM_list_primary_objects_only(TaskRevTag,dml_task_rel_type,&count_DML,&DMLRevision));
											 printf("\n\t No OF DML FOUND  : %d",count_DML);fflush(stdout);

											 if(count_DML>0)
											  {
												for(rr=0;rr<count_DML;rr++)
												 {
													DMLRevTag	=	NULLTAG;
													DMLRevTag	=	DMLRevision[rr];
													
													if(TCTYPE_ask_object_type(DMLRevTag,&objDMLTypeTag));
													if(TCTYPE_ask_name2(objDMLTypeTag,&type_dml_name));
													printf("\n type_dml_name : %s\n",type_dml_name);fflush(stdout);

													if (tc_strcmp(type_dml_name,"ChangeRequestRevision")==0)
													{
														ITKCALL(AOM_ask_value_string(DMLRevTag,"item_id",&DML_name));
														printf("\n DML_name is ----------- %s\n",DML_name);fflush(stdout);

														break;
													}
												 }
											  }
											  else
											  {
												printf("\n No DML found...!!!\n");fflush(stdout);

											  }
										  }								
									  }
								  }
							  }
						  }
						  else
						  {
							printf("\n No TASK found...!!!\n");fflush(stdout);

						  }
					  }
				  }
//			  }
//			  }
//			  else
//			  {
//				 VCnumberDup=(char *) MEM_alloc(50);
//
//				 tc_strcpy(VCnumberDup,VCVal);
//				 printf("\n else VCnumberDup ============= [%s]  \n",VCnumberDup); fflush(stdout);
//			  }
//
//		  }
//
//	MEM_free(tags_found);
//	}
//	else
//	{
//		printf ("Pls enter input part ?? \n"); fflush(stdout);
//		exit (0);
//	}
//
//	if (Design_rev_tag == null_tag)
//	{
//		printf ("\n references CCV rel not found -Design_rev_tag ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", req_item); fflush(stdout);
//		exit (0);
//	}

	//ifail = BOM_create_window (&window);
	//CHECK_FAIL;

	//ifail = CFM_find( "ERC release and above", &rule );

	//printf ("\n after rev rule \n"); fflush(stdout);

	//ifail = BOM_set_window_config_rule( window, rule );
	//CHECK_FAIL;
	
//	tag_t	SnapshotRevTag	= NULLTAG;
//	tag_t	HasSnapshot_type	= NULLTAG;
//	tag_t*	Snapshotattachments	= NULL;
//	int Snapcnt=0;
//
//
//
//	ifail = GRM_find_relation_type("T5_CCIDHasSnapshot",&HasSnapshot_type); 
//	printf("\n  Expansion_TPL_16 after snapshot rel \n");fflush(stdout);
//
//	ifail = GRM_list_secondary_objects_only(Main_CCID_rev_tag,HasSnapshot_type,&Snapcnt,&Snapshotattachments);
//	printf("\n  Snapcnt [%d] -------> \n",Snapcnt);fflush(stdout);
//
//	if (Snapcnt ==1)
//	{
//		SnapshotRevTag=Snapshotattachments[0];
//	}
//	else
//	{
//		printf("More than 1 or 0 snapshot foundg...");  fflush(stdout);
//		exit(1);
//	}



	tag_t Modl_rev_tags = NULLTAG;
	ITKCALL(AOM_ask_value_tag(SnapshotRevTag,"topLine",&Modl_rev_tags));



	ifail =BOM_create_window_from_snapshot(SnapshotRevTag,&window);
	
	printf("\n  After BOM_create_window------- \n");fflush(stdout);

	//ifail = BOM_set_window_top_line (window, null_tag, SnapshotRevTag, null_tag, &top_lineS);

	//ifail = BOM_set_window_top_line (window, null_tag, Design_rev_tag, null_tag, &top_line);
	ifail = BOM_set_window_top_line (window, null_tag, Modl_rev_tags, null_tag, &top_line);
	CHECK_FAIL;

	ifail = BOM_window_show_suppressed ( window );
	CHECK_FAIL;

	ITK_CALL(BOM_set_window_pack_all(window, FALSE));

	if(fptr1==NULL)
	{
		printf("Error! in File opening...%s",Report_Name);  fflush(stdout);
		exit(1);
	}
	if(fptr2==NULL)
	{
		printf("Error! in File opening...%s",Report_Para);  fflush(stdout);
		exit(1);
	}
	if(fptr3==NULL)
	{
		printf("Error! in File opening...%s",Report_Struct);  fflush(stdout);
		exit(1);
	}
	if(fptr4==NULL)
	{
		printf("Error! in File opening...%s",File_List);  fflush(stdout);
		exit(1);
	}
	if(fptr5==NULL)
	{
		printf("Error! in File opening...%s",Support_Master);  fflush(stdout);
		exit(1);
	}
	if(fptr6==NULL)
	{
		printf("Error! in File opening...%s",User_File);  fflush(stdout);
		exit(1);
	}
	if(fptr7==NULL)
	{
		printf("Error! in File opening...%s",VC_Details);  fflush(stdout);
		exit(1);
	}
   if(fptr8==NULL)
	{
		printf("Error! in File opening...%s",Proj_Details);  fflush(stdout);
		exit(1);
	}


	printf("\n Calling all functions now...\n");  fflush(stdout);

	//Container_List (top_line,rule,VCnumberDup,RevIDNo,Objdesc);
	Container_List (top_line,VCnumberDup,RevIDNo,ObjDescFinal);

	printf("\n END OF Container_List AAAA..\n");  fflush(stdout);

	//Expansion_TPL_16 (LatestModRevTag,top_line,rule,VCnumberDup,RevIDNo,Objdesc,vcDrStatDup,vcProjDup,partnumDup);
	Expansion_TPL_16 (LatestModRevTag,top_line,rule,VCnumberDup,RevIDNo,ObjDescFinal,vcDrStatDup,vcProjDup,partnumDup);

	}

  }
else
{  
	 VCnumberDup=(char *) MEM_alloc(50);

	 tc_strcpy(VCnumberDup,VCVal);
	 printf("\n else VCnumberDup ============= [%s]  \n",VCnumberDup); fflush(stdout);
}

}

	MEM_free(tags_found);
	}
	else
	{
		printf ("Pls enter input part ?? \n"); fflush(stdout);
		exit (0);
	}

	if (Design_rev_tag == null_tag)
	{
		printf ("\n references CCV rel not found -Design_rev_tag ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", req_item); fflush(stdout);
		exit (0);
	}

	printf("\n END OF  Expansion_TPL_16 AAAA..\n");  fflush(stdout);

	printf("\n END OF CODE Adi..\n");  fflush(stdout);


	ITK_exit_module(true);

	return status;
}

    			

//int  Expansion_TPL_16(tag_t top_line,tag_t rule,char * PartNo,char * Assy_Rev,char * Objdesc,char * vcDrStatDup,char * vcProjDup,char * PartNo)
//
void trimLeading(char * str)
{
    int index, i, j;

    index = 0;

    /* Find last index of whitespace character */
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }

    if(index != 0)
    {
        /* Shift all trailing characters to its left */
        i = 0;
        while(str[i + index] != '\0')
        {
            str[i] = str[i + index];
            i++;
        }
        str[i] = '\0'; // Make sure that string is NULL terminated
    }
}
//

int  Expansion_TPL_16(tag_t LatestModRevTag ,tag_t top_line,tag_t rule,char * VCnumberDup,char * RevIDNo,char * ObjDescFinal,char * vcDrStatDup,char * vcProjDup,char * partnumDup)
{

	int		statusZ;
	int		statusZZ;
	int		ww            = 0;
	int		yy            = 0;
	int		n_tags_found4 = 0;
	int		n_tags_found6 = 0;
	int		jj			  = 0;
	int		cnt			  = 0;
	int		cntX		  = 0;
	int		countZ		  = 0;
	int		countZZ		  = 0;
	int		calfiletest   = 0;
	int		CalFlileReq   = 0;
	int		kp			  = 0;
	int		bb		      = 0;
	int     flag		  = 0;
	int     flagX		  = 0;
	int     iWrite		  = 0;
	int     iRead		  = 0;
	int     iRead2		  = 0;
	int     m			  = 0;
	int     mm			  = 0;
	int     index		  = 0;
	int     indexcon	  = 0;
	int     wx			  = 0;
	int     wy			  = 0;
	int     PartCnt		  = 0;
	int     num			  = 0;
	int     pCnt			  = 0;
	int     ms			  = 0;
	int     mt			  = 0;
	int     ss			  = 0;

tag_t top_lineS;
	tag_t  *tags_found4   = NULL;
	tag_t  *tags_found6   = NULL;
	tag_t   ChildRev      = NULLTAG;
	tag_t   ContChildRev  = NULLTAG;

	tag_t   SpkitTag	  = NULLTAG;
	tag_t   Sparekit      = NULLTAG;
	tag_t sp_kit_rel_type = NULLTAG;
	tag_t Parameter_type  = NULLTAG;
	tag_t	namedrefobject     = NULLTAG;
	tag_t	ParaPtr       = NULLTAG;
	

	char* container		  = NULL;
	char* containerrev	  = NULL;
	char* ConProject	  = NULL;
	char* EEPartType	  = NULL;
	char* PartType		  = NULL;
	char* TPLEOL		  = NULL;
	char* ContEOL		  = NULL;
	char* Cnt_EcuType	  = NULL;
	char* Sp_ItemID		  = NULL;
	char* Sp_ItemREV	  = NULL;
	char* contpartrev	  = NULL;
	char* contpart		  = NULL;
	char* ProjectC		  = NULL;
	char* ChdPartType	  = NULL;
	char* Desg			  = NULL;
	char* ownername		  = NULL;

	char orig_name[IMF_filename_size_c + 1];
	char *pathname		  = NULL;
	char *relative_path		  = NULL;

	char * test_file      = NULL;
	char * test_file2      = NULL;
	char * test_file3     = NULL;
	char * test_fileX      = NULL;
	char *cRplStr		  =	NULL;
	char *cRplStrdup		  =	NULL;


	char	*contpartrevdup		= NULL;
	char	*ContainerRevValDup	= NULL;

	char*	EOLDup			=	NULL;
	char*	EOL_Val			=	NULL;
	//char*	EOL_Val_Both	=	"BOTH";
	//char*	EOL_Val_N		=	"N";
	char*	EOL				=	NULL;
	char*	EOLNA			=	"NA";
	char*	ValBoth			=	"BOTH";
	char*	WriteVal		=	"WRITE";
	char*	ReadVal			=	"READ";
	char*	ServVal			=	"SERVICE";
	char*	NoValStr		=	"NA";
	char*	EOL_Val_Both	=	"BOTH";
	char*	EOL_Val_N		=	"N";
	char*   objtype			=	NULL;
	char*   AtttachFileNames  = NULL;
	char*   Sticker			  = NULL;
	char*   Pr_Boot_Loader_Sm = NULL;
	char*   Pr_Boot_Loader	  = NULL;
	char*   BasicSw			  = NULL;
	char*   BASIC			  = "NA";
	char*   SLV				  = "NA";
	char*   Se_Boot_Loader    = NULL;
	char*   Se_Boot_Loader_Sm = NULL;
	char*   AppSw_Sm	      = NULL;
	char*   AppSw		      = NULL;
	char*   CalSw		      = NULL;
	char*   CalSw_Sm	      = NULL;
	char*   CalSw_SmDup	      = NULL;
	char*   CfgSw_Sm	      = NULL;
	char*   CfgSw		      = NULL;
	char*   VehCal		      = NULL;
	char*   VehCal_Sm	      = NULL;
	char*   HwPart_Sm	      = NULL;
	char*   HwPart		      = NULL;
	char*   CalSwDup	      = NULL;
	char*   CalFileName	      = NULL;
	char*   NoParaDes	      = "";

	char *CurrentDate		  = NULL;;
	char * DateDup		      = NULL;
	char * ItemId2		      = NULL;
	char * ItemId1		      = NULL;

	char	**SWDetails		  =	NULL;
	char	**HWDetails	      =	NULL;
	char * PRM_EcuType		  = NULL;
	char * paraecutype		  = NULL;
	char * paradesc			  = NULL;
	char * sVehicleType		  = NULL;
	char * sMVehicleType	  = NULL;
	char * sSVehicleType	  = NULL;
	char * ParaApp			  = NULL;
	char * ParaRead			  = NULL;
	char * DefValue			  = NULL;
	char * ValSer			  = "SERVICE";


	char * paraunit		     = NULL;
	char * paraeptype		 = NULL;
	char * paraeptypedup	 = NULL;
	char * paralen			 = NULL;
	char * paramin			 = NULL;
	char * paramax			 = NULL;
	char * parastep			 = NULL;
	char * ParaDIDVal		 = NULL;
	char * paradoc			 = NULL;
	char * 	 Noval			 = "0";
	char * paradocdupS		 = NULL;
	char * paravalid		 = NULL;
	char * paravallist		 = NULL;
	char * CreDate			 = NULL;
	char * ParaAppdup_Val    = NULL;
	char * ParaReaddup_Val   = NULL;
	char * conitemid	     = NULL;
	char * conitemrevid	     = NULL;

	char * ChildRevitemid	 = NULL;
	char * ChildRevitemrevid = NULL;
	char * slavecontainer    = NULL;
	char * mastercontainer   = NULL;
	char * containerslv      = NULL;
	char * containermst      = NULL;
	char * byte1             = NULL;
	char * byte_size         = NULL;
	char * name              = NULL;
	char * DRStatus          = NULL;
	char * CalFileNameDup    = NULL;
	char * BitValue          = NULL;

	char * CCIDNumberS       = NULL;
	char * CCIDNumberDupS    = NULL;
	char * Cciditem_rev      = NULL;
	char * CCIDRevDupS       = NULL;
	//char * CcidSeq           = NULL;
	//char * CCIDSeqDupS       = NULL;
	int CcidSeq;
	char * CcidAppDupS       = NULL;
	char * CcidAppS          = NULL;

	char * Cciditem_revS     = NULL;
	char * MltCciditem_rev   = NULL;
	char * MltCCIDNumberS    = NULL;
	char * MltCCIDSeqDupS    = NULL;
	char * MltCCIDRevDupS    = NULL;
	char * MltCCIDNumberDupS = NULL;
	char * CCIDRevDupS1 = NULL;
	//long		byte1			= 0;

	int ifail;
	int status;
	int iStatus=0;
	int referencenumberfound =0;
	int cntEE =0;
	int ee =0;
	int ic =0;
	int ValFlag =0;
	int Slvcnt =0;
	int Mstrcnt =0;
	int Msflag =0;
	int Mflag =0;
	int Sflag =0;
	int paradesccnt =0;
	int paradesclength =0;
	int paraeptypelength =0;
	int paraccnt =0;
	int 	mach_type = SS_UNIX_MACHINE;

	tag_t window, item_tag = null_tag,top_line2;
	tag_t window2               = NULLTAG;
	tag_t *childrenZ;
	tag_t *childrenZZ;
	tag_t *attachments			= NULLTAG;
	tag_t *attachmentsX			= NULLTAG;
	tag_t *attachmentsEE		= NULLTAG;
	tag_t *Slvattachments		= NULLTAG;
	tag_t *Mstrattachments		= NULLTAG;
	tag_t dataset				= NULLTAG;
	tag_t SlaveNo				= NULLTAG;
	tag_t MstrNo				= NULLTAG;
	tag_t ParaMaster			= NULLTAG;
	tag_t relation_type         = NULLTAG;
	tag_t EErelation_type       = NULLTAG;
	tag_t ConPararelation_type  = NULLTAG;
	tag_t MstrSlvrelation_type  = NULLTAG;
	char	 *NewDate			= NULL;
	char *type					= NULL;
	//char *MltCcidSeq					= NULL;
	int MltCcidSeq;

	tag_t*	PartTags			= NULLTAG;
	tag_t	relationprop		= NULLTAG;
	tag_t*	Prm_Reltags			= NULLTAG;
	tag_t	PrmRelTag			= NULLTAG;
	tag_t	HasNewCCID_type		= NULLTAG;
	tag_t	HasSnapshot_type	= NULLTAG;
	tag_t  *CCIDattachments		= NULLTAG;
	tag_t  *Snapshotattachments	= NULLTAG;
	int		CCIDcnt				= 0;
	int		Snapcnt				= 0;
	int		cc					= 0;
	tag_t 	CCIDRevTag			= NULLTAG;
	tag_t 	CCIDMstRevTag			= NULLTAG;
	tag_t 	SnapshotRevTag			= NULLTAG;

//char * ParaReaddup_Val =NULL;
char * parastepdup   =NULL;
char * paradocdup   =NULL;
char* containermstrev =NULL;
char* mastercontainerrev =NULL;


	//int length	=0;	  
	//int byte1int	=0;	  
	long byte1int	=0;	  

	char *reference_nameDS=NULL;	
	reference_nameDS=(char *) MEM_alloc(15);

	char *Exfile=NULL;	
	Exfile=(char *) MEM_alloc(300);

	AE_reference_type_t reftype;
	char refname[AE_reference_size_c + 1];
	//pathname		 =(char *) MEM_alloc(300);
	AtttachFileNames =(char *) MEM_alloc(4000);

    SWDetails = (char**)MEM_alloc( 1 * sizeof *SWDetails );
    HWDetails = (char**)MEM_alloc( 1 * sizeof *HWDetails );

	time_t temp ; 
    struct tm *timeptr ; 
    char pAccessDate[30]; 

	CurrentDate		=(char *) MEM_alloc(30);
    time( &temp ); 
      
    tc_strcpy(pAccessDate,"");
    timeptr = localtime( &temp ); 

    strftime(pAccessDate, sizeof(pAccessDate), "%x - %I:%M%p", timeptr); 
      
    printf("Formatted date & time  Expansion_TPL_16 : %s\n", pAccessDate ); 
	tc_strcpy(CurrentDate,pAccessDate);
	printf("\n CurrentDate AAAAAAA Expansion_TPL_16 :%s\n",CurrentDate);

	char * CurrentDateDup =NULL;
	char * CurrentDateDupNew =NULL;
	CurrentDateDup=(char *) MEM_alloc(50);
	CurrentDateDupNew=(char *) MEM_alloc(50);

	char	*strToBereplaceddate;
	char	*strToBeUsedInsteaddate;

	strToBereplaceddate		=	(char*)malloc(5);
	tc_strcpy(strToBereplaceddate,"/");

	strToBeUsedInsteaddate	=	(char*)malloc(5);
	tc_strcpy(strToBeUsedInsteaddate,"-");

	ifail =STRNG_replace_str (CurrentDate,strToBereplaceddate, strToBeUsedInsteaddate,&CurrentDateDup); 
	printf("\n..CurrentDateDup [%s].. \n",CurrentDateDup);fflush(stdout);//02-13-2020 04:33PM

    tc_strcpy(CurrentDateDupNew,CurrentDateDup); 
																														
    NewDate=subString(CurrentDateDupNew,0,10); 
    printf("\n NewDate is ------------  = [%s]\n", NewDate);fflush(stdout);

	ifail = GRM_find_relation_type("T5_IsMemberOfCCID",&HasNewCCID_type); 
	printf("\n  Expansion_TPL_16 after HasNewCCID_type ======== \n");fflush(stdout);

	ifail = GRM_list_secondary_objects_only(LatestModRevTag,HasNewCCID_type,&CCIDcnt,&CCIDattachments);
	
	//ITK_CALL(ITEM_ask_latest_rev(LatestModRevTag,&CCIDRevTag));

   printf("\n CCIDcnt is============= [%d]  \n",CCIDcnt); fflush(stdout);	
	if (CCIDcnt > 0)
	 {
		if (CCIDcnt > 1)
		 {
		   CCIDNumberDupS     = NULL;
		   CCIDRevDupS		   = NULL;
		   CCIDNumberDupS	   =(char *) MEM_alloc(50);
		   CCIDRevDupS		   =(char *) MEM_alloc(5);
		   CCIDRevDupS1		   =(char *) MEM_alloc(5);
		   CcidAppDupS		   =(char *) MEM_alloc(10);
		   for (cc=0;cc<CCIDcnt;cc++ )
			 {
				 CCIDRevTag=CCIDattachments[cc];
				// ITK_CALL(ITEM_ask_latest_rev(CCIDMstRevTag,&CCIDRevTag));
//				 MltCCIDNumberDupS     = NULL;
//				 MltCCIDRevDupS		   = NULL;
//				 MltCCIDSeqDupS		   = NULL;
//				 MltCCIDNumberDupS	   =(char *) MEM_alloc(50);
//				 MltCCIDRevDupS		   =(char *) MEM_alloc(5);
//				 MltCCIDSeqDupS		   =(char *) MEM_alloc(5);
//
//				 if(AOM_ask_value_string(CCIDRevTag,"item_id",&MltCCIDNumberS)==ITK_ok);
//				 tc_strcpy(MltCCIDNumberDupS,MltCCIDNumberS);			  
//				 printf("\n MltCCIDNumberDupS is  = [%s]\n", MltCCIDNumberDupS);fflush(stdout);
//				  
//				 if(AOM_ask_value_string(CCIDRevTag,"item_revision_id",&MltCciditem_rev)==ITK_ok);
//				 tc_strcpy(MltCCIDRevDupS,MltCciditem_rev);
//				 printf("\n MltCCIDRevDupS IS --------->[%s]\n",MltCCIDRevDupS);fflush(stdout);
//
//				 //if(AOM_ask_value_string(CCIDRevTag,"sequence_id",&MltCcidSeq)==ITK_ok);
//				 if(AOM_ask_value_int(CCIDRevTag,"sequence_id",&MltCcidSeq)==ITK_ok);
//				// tc_strcpy(MltCCIDSeqDupS,MltCcidSeq);
//				// printf("\n MltCCIDSeqDupS IS --------->[%s]\n",MltCCIDSeqDupS);fflush(stdout);
//
//				 if((tc_strcmp(MltCCIDNumberDupS,req_item)==0) && (tc_strcmp(MltCCIDRevDupS,req_item2)==0))
//				 {
//					 printf("\n match found \n");fflush(stdout);
//					 break;
//				 }



				 if(AOM_ask_value_string(CCIDRevTag,"item_id",&CCIDNumberS)==ITK_ok);
				 tc_strcpy(CCIDNumberDupS,CCIDNumberS);			  
				 printf("\n CCIDNumberDupS is  = [%s]\n", CCIDNumberDupS);fflush(stdout);
				  
				 if(AOM_ask_value_string(CCIDRevTag,"item_revision_id",&Cciditem_rev)==ITK_ok);
				 tc_strcpy(CCIDRevDupS1,Cciditem_rev);
				 printf("\n CCIDRevDupS1 IS --------->[%s]\n",CCIDRevDupS1);fflush(stdout);

				 if((tc_strcmp(CCIDNumberDupS,req_item)==0) && (tc_strcmp(CCIDRevDupS1,req_item2)==0))
				 {
					 printf("\n match found \n");fflush(stdout);
					 break;
				 }
			 }

			 if((CCIDNumberDupS)!=NULL && tc_strlen(CCIDNumberDupS)>0)
			 {
			   printf("\n CCIDNumberDupS is  = [%s]\n", CCIDNumberDupS);fflush(stdout);							
			 }
			 else
			 {
				tc_strcpy(CCIDNumberDupS,"NA");
				printf("\n CCIDNumberDupS is  = [%s]\n", CCIDNumberDupS);fflush(stdout);	
			 }

			 char	*strToBereplR=NULL;
			 char	*strToBeUsedR=NULL;

			 strToBereplR		=	(char*)malloc(5);
			 tc_strcpy(strToBereplR,";");

			 strToBeUsedR	=	(char*)malloc(5);
			 tc_strcpy(strToBeUsedR,"");

			 ifail =(STRNG_replace_str (Cciditem_rev,strToBereplR, strToBeUsedR,&Cciditem_revS));
			 printf("\n Cciditem_revS is  = [%s]\n", Cciditem_revS);fflush(stdout);

			 tc_strcpy(CCIDRevDupS,Cciditem_revS);

			 if((CCIDRevDupS)!=NULL && tc_strlen(CCIDRevDupS)>0)
			 {
			    printf("\n CCIDRevDupS is  = [%s]\n", CCIDRevDupS);fflush(stdout);
			 }
			 else
			 {
				tc_strcpy(CCIDRevDupS,"NA");
				printf("\n CCIDRevDupS is  = [%s]\n", CCIDRevDupS);fflush(stdout);	
			 }

			 if(AOM_ask_value_string(CCIDRevTag,"t5_CCIDApplicability",&CcidAppS)==ITK_ok);
			 tc_strcpy(CcidAppDupS,CcidAppS);
			 if((CcidAppDupS)!=NULL && tc_strlen(CcidAppDupS)>0)
			 {
			   printf("\n CcidAppDupS is  = [%s]\n", CcidAppDupS);fflush(stdout);							
			 }
			 else
			 {
				tc_strcpy(CcidAppDupS,"NA");
				printf("\n CcidAppDupS is  = [%s]\n", CcidAppDupS);fflush(stdout);	
			 }

		 }
		 else
		 {
			 
			 CCIDRevTag=CCIDattachments[0];
			 //ITK_CALL(ITEM_ask_latest_rev(CCIDMstRevTag,&CCIDRevTag));

			 CCIDNumberDupS		   =(char *) MEM_alloc(50);
			 CCIDRevDupS		   =(char *) MEM_alloc(5);
			 //CCIDSeqDupS		   =(char *) MEM_alloc(5);
			 CcidAppDupS		   =(char *) MEM_alloc(10);

			 if(AOM_ask_value_string(CCIDRevTag,"item_id",&CCIDNumberS)==ITK_ok);
			 tc_strcpy(CCIDNumberDupS,CCIDNumberS);			  

			 if((CCIDNumberDupS)!=NULL && tc_strlen(CCIDNumberDupS)>0)
			 {
			   printf("\n CCIDNumberDupS is  = [%s]\n", CCIDNumberDupS);fflush(stdout);							
			 }
			 else
			 {
				tc_strcpy(CCIDNumberDupS,"NA");
				printf("\n CCIDNumberDupS is  = [%s]\n", CCIDNumberDupS);fflush(stdout);	
			 }

			 if(AOM_ask_value_string(CCIDRevTag,"item_revision_id",&Cciditem_rev)==ITK_ok);
			 printf("\n Cciditem_rev is  = [%s]\n", Cciditem_rev);fflush(stdout);	
			
			 char	*strToBereplR=NULL;
			 char	*strToBeUsedR=NULL;

			 strToBereplR		=	(char*)malloc(5);
			 tc_strcpy(strToBereplR,";");

			 strToBeUsedR	=	(char*)malloc(5);
			 tc_strcpy(strToBeUsedR,"");

			 ifail =(STRNG_replace_str (Cciditem_rev,strToBereplR, strToBeUsedR,&Cciditem_revS));
			 printf("\n Cciditem_revS is  = [%s]\n", Cciditem_revS);fflush(stdout);

			 tc_strcpy(CCIDRevDupS,Cciditem_revS);

			 if((CCIDRevDupS)!=NULL && tc_strlen(CCIDRevDupS)>0)
			 {
			    printf("\n CCIDRevDupS is  = [%s]\n", CCIDRevDupS);fflush(stdout);
			 }
			 else
			 {
				tc_strcpy(CCIDRevDupS,"NA");
				printf("\n CCIDRevDupS is  = [%s]\n", CCIDRevDupS);fflush(stdout);	
			 }
	

			 //if(AOM_ask_value_string(CCIDRevTag,"sequence_id",&CcidSeq)==ITK_ok); 
			 if(AOM_ask_value_int(CCIDRevTag,"sequence_id",&CcidSeq)==ITK_ok); 
			 //tc_strcpy(CCIDSeqDupS,CcidSeq);
			 printf("\n CcidSeq is AA = [%d]\n", CcidSeq);fflush(stdout);
			 //if((CcidSeq)!=NULL && tc_strlen(CcidSeq)>0)
			 if((CcidSeq)>0)
			 {
			   printf("\n CcidSeq is  = [%d]\n", CcidSeq);fflush(stdout);							
			 }
			 else
			 {
				//tc_strcpy(CcidSeq,"NA");
				printf("\n else CcidSeq is  = [%d]\n", CcidSeq);fflush(stdout);	
			 }

			 if(AOM_ask_value_string(CCIDRevTag,"t5_CCIDApplicability",&CcidAppS)==ITK_ok);
			 tc_strcpy(CcidAppDupS,CcidAppS);
			 if((CcidAppDupS)!=NULL && tc_strlen(CcidAppDupS)>0)
			 {
			   printf("\n CcidAppDupS is  = [%s]\n", CcidAppDupS);fflush(stdout);							
			 }
			 else
			 {
				tc_strcpy(CcidAppDupS,"NA");
				printf("\n CcidAppDupS is  = [%s]\n", CcidAppDupS);fflush(stdout);	
			 }
		 }
	 }
	else
	 { 
		  printf("\n nothing found in new ccid relation \n"); fflush(stdout);	
	 }

	//fprintf(fptr1," \n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s \n",Cnt_EcuTypeDup,container,containerrevdup,VCnumberDup,Objdesc,DML_name,t5NoEcuDup,vcDrStatDup,vcProjDup);fflush(fptr1);
	//fprintf(fptr1," \n %s,%s%s,%s,%s,%s,%s,%s,%s,%s \n",CCIDNumberDupS,CCIDRevDupS,req_item3,CcidAppDupS,VCnumberDup,ObjDescFinal,DML_name,t5NoEcuDup,vcDrStatDup,vcProjDup);fflush(fptr1);
	fprintf(fptr1," \n %s,%s,%s,%s,%s,%s,%s,%s,%s \n",CCIDNumberDupS,CCIDRevDupS,CcidAppDupS,VCnumberDup,ObjDescFinal,DML_name,t5NoEcuDup,vcDrStatDup,vcProjDup);fflush(fptr1);
	//fprintf(fp1,"\n%s,%s%s,%s,%s,%s,%s,%s,%s,%s,\n",BaseLineNumberDupS,BaseLineRevDupS,BaseLineSeqDupS,BaseLineAppDupS,vcnoDup,vcdescDup,DmlNo,t5NoEcuDup,vcDrStatDup,vcProjDup);fflush(fp1);

	ifail = GRM_find_relation_type("T5_CCIDHasSnapshot",&HasSnapshot_type); 
	printf("\n  Expansion_TPL_16 after snapshot rel \n");fflush(stdout);

	ifail = GRM_list_secondary_objects_only(CCIDRevTag,HasSnapshot_type,&Snapcnt,&Snapshotattachments);
	printf("\n  Snapcnt [%d] -------> \n",Snapcnt);fflush(stdout);
    
	if(Snapcnt >0)													//18 SEPT	
	{																//18 SEPT	
      SnapshotRevTag=Snapshotattachments[0];		                //18 SEPT				
	  printf("\n  After SnapshotRevTag------- \n");fflush(stdout);	//18 SEPT	
	
//	 for (ss=0;ss<Snapcnt;ss++ )												    //18 SEPT	
//			 {																		//18 SEPT	
//				 SnapshotRevTag=Snapshotattachments[0];								//18 SEPT	
//				 printf("\n  After SnapshotRevTag------- \n");fflush(stdout);       //18 SEPT	
//			 }                                                                      //18 SEPT	

	//ifail = BOM_create_window (&window);
	//ifail =BOM_create_window_from_snapshot(SnapshotRevTag,&window);
	
	//printf("\n  After BOM_create_window------- \n");fflush(stdout);

	//ifail = BOM_set_window_top_line (window, null_tag, SnapshotRevTag, null_tag, &top_lineS);
	
	//printf("\n  After top_lineS------- \n");fflush(stdout);

	////ifail = BOM_window_show_suppressed ( window );
	

	////ITK_CALL(BOM_set_window_pack_all(window, FALSE));


	//countZ=0;
	//ITK_CALL(AOM_load(SnapshotRevTag));


	//printf("\n  After AOM_load \n");fflush(stdout);

	//statusZ = BOM_line_ask_child_lines (top_line, &countZ, &childrenZ);
	//printf("\n  countZ   -------> [%d] \n",countZ);fflush(stdout);


	int level 				= 0;				
	int StructChldCnt   =    0;
	int www   =    0;
	int arr   =    0;
	char *containerID =NULL;
	char *containerrevID =NULL;
	char *mastercontainerrevID =NULL;
	struct			BomChldStrut_EE	ChldStrutBdySh[5000];

	Multi_Get_Part_BOM_Lvl_Col(LatestModRevTag,99,level,SnapshotRevTag,ChldStrutBdySh,&StructChldCnt);
	printf ("\n Child Count count %d \n",StructChldCnt);fflush(stdout);

	//ITK_CALL(AOM_ask_value_tags(SnapshotRevTag,"contents",&countZ,&childrenZ)); //////// 
    //printf("\n  countZ  contents -------> [%d] \n",countZ);fflush(stdout);

	if(StructChldCnt >0)
	{
	   char* totalArray  [StructChldCnt+1];
	   //const char **totalArray  = (const char **) MEM_alloc(2 * sizeof(char *));
	   for (www=0;www<StructChldCnt+1;www++)
		{
			tag_t ChildRevBomLineXX=NULLTAG;
			if( AOM_ask_value_string(ChldStrutBdySh[www].child_objs,"item_id",&containerID)!=ITK_ok);

			if( AOM_ask_value_string(ChldStrutBdySh[www].child_objs,"item_revision_id",&containerrevID)!=ITK_ok);

//			    =ChldStrutBdySh[ww].child_objs_bvr;
			totalArray[www] = MEM_alloc(50);
			
			tc_strcpy(totalArray[www],containerID);
			tc_strcat(totalArray[www],"_");
			tc_strcat(totalArray[www],containerrevID);

			printf("\n [%d]  totalArray -- %s\n",www,totalArray[www] );fflush(stdout);
		}


		//for (ww=0;ww<countZ;ww++)
		for (ww=0;ww<StructChldCnt+1;ww++)
		 {
			printf("\n..IN Expansion_TPL_16 structure.. :%d\n",ww);fflush(stdout);
			container=NULL;
			containerrev=NULL;
			tag_t ChildRevBomLine=NULLTAG;
			//if( AOM_ask_value_string(childrenZ[ww],"bl_item_item_id",&container)!=ITK_ok);
			//if( AOM_ask_value_string(childrenZ[ww],"item_id",&container)!=ITK_ok);
			if( AOM_ask_value_string(ChldStrutBdySh[ww].child_objs,"item_id",&container)!=ITK_ok);

			//if( AOM_ask_value_string(childrenZ[ww],"bl_rev_item_revision_id",&containerrev)!=ITK_ok);
			//if( AOM_ask_value_string(childrenZ[ww],"item_revision_id",&containerrev)!=ITK_ok);
			if( AOM_ask_value_string(ChldStrutBdySh[ww].child_objs,"item_revision_id",&containerrev)!=ITK_ok);

			ChildRevBomLine=ChldStrutBdySh[ww].child_objs_bvr;

   
			//if( AOM_ask_value_string(childrenZ[ww],"bl_Design Revision_t5_ProjectCode ",&ConProject)!=ITK_ok);
			printf("\n inside Expansion_TPL_16 structure  container is [%s] -----> & containerrev is ---->[%s] \n",container,containerrev);fflush(stdout);

		if((container)&&(containerrev))
		 {
			// if((tc_strcmp(container,req_item)==0) && (tc_strcmp(containerrev,req_item2)==0))

			const char **attrs3  = (const char **) MEM_alloc(2 * sizeof(char *));
			const char **values3 = (const char **) MEM_alloc(2 * sizeof(char *));
			attrs3[0]	   ="item_id";
			//attrs3[1]      ="object_type";
			values3[0]     = (char *)container;
			//values3[1]     = "Design";
			//ifail = ITEM_find_items_by_key_attributes(2,attrs, values, &n_tags_found, &tags_found);
			ifail = ITEM_find_item_revs_by_key_attributes(1,attrs3, values3,containerrev, &n_tags_found4, &tags_found4);
			MEM_free(attrs3);
			MEM_free(values3);

			if (n_tags_found4>0)
			{
			   ChildRev= tags_found4[0];
			}

			if( AOM_ask_value_string(ChildRev,"t5_ProjectCode",&ConProject)!=ITK_ok);

			if( AOM_ask_value_string(ChildRev,"t5_SwPartType",&PartType)!=ITK_ok);

			if( AOM_ask_value_string(ChildRev,"t5_EE_PartType",&EEPartType)!=ITK_ok);

			if( AOM_ask_value_string(ChildRev,"t5_AppForEOL",&TPLEOL)!=ITK_ok);

			if( AOM_ask_value_string(ChildRev,"t5_AppForEOL",&ContEOL)!=ITK_ok);
			if( AOM_ask_value_string(ChildRev,"t5_PartStatus",&DRStatus)!=ITK_ok);
			printf("\n Expansion_TPL_16 structure   ConProject is ---> [%s]  PartType is ---> [%s]    EEPartType is ---> [%s]  TPLEOL is ---> [%s]  ContEOL is ---> [%s] DRStatus is ---> [%s] \n",ConProject,PartType,EEPartType,TPLEOL,ContEOL,DRStatus); fflush(stdout);

			char	*strToBerepl1R;
			char	*strToBeUsed1R;
			char	*containerrevdup;

			strToBerepl1R		=	(char*)malloc(5);
			tc_strcpy(strToBerepl1R,";");

			strToBeUsed1R	=	(char*)malloc(5);
			tc_strcpy(strToBeUsed1R,"_");

			ifail =(STRNG_replace_str (containerrev,strToBerepl1R, strToBeUsed1R,&containerrevdup));
			printf("\n containerrevdup containerrevdup    ConProject is ---> [%s]\n",containerrevdup);fflush(stdout);

			char * ContEOLDup =NULL;
			ContEOLDup=(char *) MEM_alloc(100);
			char * ContEOLDup_Val =NULL;
			ContEOLDup_Val=(char *) MEM_alloc(100);

			if((ContEOL))
			 {			   
				tc_strcpy(ContEOLDup,ContEOL); 
			 }
			else
			 {
				tc_strcpy(ContEOLDup,EOLNA);
			 }
		
			if(ContEOLDup)
			 {

				if(tc_strstr(ContEOLDup,"BothES")!=NULL) 
				{					
					tc_strcpy(ContEOLDup_Val,ValBoth);
				}
				else if (tc_strstr(ContEOLDup,"NotApp")!=NULL)
				{					
					tc_strcpy(ContEOLDup_Val,ValBoth);
				}
				else if (tc_strstr(ContEOLDup,"Service")!=NULL)
				{					
					tc_strcpy(ContEOLDup_Val,ServVal);
				}
				else
				{
					tc_strcpy(ContEOLDup_Val,ContEOLDup);
				}
			 }
			 
			char * TPLEOLDUP =NULL;
			TPLEOLDUP=(char *) MEM_alloc(100);
			char * TPLEOLVAL =NULL;
			TPLEOLVAL=(char *) MEM_alloc(100);

			if((TPLEOL))
			{
				tc_strcpy(TPLEOLDUP,TPLEOL);
			}
			if (tc_strstr(TPLEOLDUP,"BothES")!=0)
			{
				tc_strcpy(TPLEOLVAL,EOL_Val_Both);
			}
			else if (tc_strstr(TPLEOLDUP,"NotApp")!=0)
			{
				tc_strcpy(TPLEOLVAL,EOL_Val_N);
			}
			else
			{
				tc_strcpy(TPLEOLVAL,TPLEOLDUP);
			}

//			if(container)
//			 {
				//if(tc_strstr(container,"16R")==0  &&  tc_strstr(container,"16L")==0 )
				//{
				//	printf("\n..IN Expansion_TPL_16 structure 16R FOUND.. \n");fflush(stdout);
				//	Expansion_TPL_16 (childrenZ[ww],rule,PartNo,Assy_Rev,Objdesc,container);
				
				//}

				if(tc_strcmp(EEPartType,"G")==0)
				{
					printf("\t IN Expansion_TPL_16 GROUP id FOUND  \n"); fflush(stdout);


					//Expansion_GrpId( ChildRev,rule,PartNo,Objdesc); 
					//Expansion_GrpId( ChildRev,ChildRevBomLine,VCnumberDup,ObjDescFinal); //uncomment later
				}
				else if((tc_strcmp(PartType,"CON")==0)||(tc_strcmp(PartType,"Container")==0) && (tc_strcmp(EEPartType,"G")!=0))
				{
					printf("\t INSIDE CONTAINER AND NOT GRP ID CONDITION =========> [%s]\n",container); fflush(stdout);

					supfile=0;
					//calfiletest=0;
					CalFlileReq=0;
					char * totalstr = NULL;

					char * mastercontainerMstr = NULL;
										
					//ADD MASTER SLAVE CODES HERE

					sVehicleType=NULL;	
					
					ifail = GRM_find_relation_type("T5_ContHasSlave",&MstrSlvrelation_type); 
					{
						sVehicleType = NULL;
					  if(MstrSlvrelation_type != NULLTAG)
					  {
						  int Flag_Master=0;
						printf("\n Expansion_TPL_16 MstrSlvrelation_type relation found......");	fflush(stdout);	
						//sVehicleType=(char *) MEM_alloc(30);								
					    
						GRM_list_primary_objects_only(ChildRev,MstrSlvrelation_type,&Mstrcnt,&Mstrattachments);
						printf("\n  NO OF Mstrcnt ----------- %d",Mstrcnt);fflush(stdout);

						 if(Mstrcnt>0)
						  {							 
							 for(mt=0;mt<Mstrcnt;mt++)
							  {
								printf("\n AFTER MstrSlvrelation_type =========> \n"); fflush(stdout);
								MstrNo = Mstrattachments[mt];
									
								if( AOM_ask_value_string(ChildRev,"item_id",&mastercontainer)!=ITK_ok);
								printf("\n IN Expansion_TPL_16 mastercontainer ---------  [%s]\n",mastercontainer); fflush(stdout);

								if( AOM_ask_value_string(MstrNo,"item_id",&mastercontainerMstr)!=ITK_ok);
								printf("\n IN Expansion_TPL_16 mastercontainer ---------  [%s]\n",mastercontainerMstr); fflush(stdout);

								if( AOM_ask_value_string(MstrNo,"item_revision_id",&mastercontainerrevID)!=ITK_ok);
								printf("\n IN Expansion_TPL_16 mastercontainerrevID ---------  [%s]\n",mastercontainerrevID); fflush(stdout);


								ITKCALL(GRM_list_secondary_objects_only(MstrNo,MstrSlvrelation_type,&Slvcnt,&Slvattachments));
								CHECK_FAIL; //slave has container
							

								for (ms=0;ms<Slvcnt;ms++)
								{
									if( AOM_ask_value_string(Slvattachments[ms],"item_id",&containermst)!=ITK_ok);
									printf("\n IN Expansion_TPL_16 containermst ---------  [%s]\n",containermst); fflush(stdout);
									
									if(tc_strcmp(mastercontainer,containermst)==0)
									{
										
										if( AOM_ask_value_string(ChildRev,"item_revision_id",&mastercontainerrev)!=ITK_ok);//primary
										printf("\n IN Expansion_TPL_16 mastercontainer ---------  [%s]\n",mastercontainerrev); fflush(stdout);	
										
										if( AOM_ask_value_string(Slvattachments[ms],"item_revision_id",&containermstrev)!=ITK_ok);//secondary
										printf("\n IN Expansion_TPL_16 containermst ---------  [%s]\n",containermstrev); fflush(stdout)	;


										if(tc_strcmp(containermstrev,mastercontainerrev) == 0)
										{
											totalstr = NULL;
											totalstr = MEM_alloc(50);

											tc_strcpy(totalstr,mastercontainerMstr);
											tc_strcat(totalstr,"_");
											tc_strcat(totalstr,mastercontainerrevID);	

											for(arr=0;arr<StructChldCnt;arr++)
											{
												if(tc_strcmp(totalArray[arr], totalstr) == 0)
												{
													sVehicleType=NULL;	
												sVehicleType=(char *) MEM_alloc(50);
												tc_strcpy(sVehicleType,"S_"); 
												tc_strcat(sVehicleType,mastercontainerMstr);
												printf("\n sVehicleType issssssssss. mastercontainer...[%s]..\n",sVehicleType);	fflush(stdout);
												break;
												}
											}
											printf("\n match found mastercontainer \n"); 
											Flag_Master=1;												
										    break;
										}

										
									}
								 }
							  }
						   }
			                             printf("\n match found mastercontainer \n");	
							  if (Flag_Master==0)
							  {

                                ITKCALL(GRM_list_secondary_objects_only(ChildRev,MstrSlvrelation_type,&Slvcnt,&Slvattachments));
								CHECK_FAIL; //slave has container
								printf("\n  NO OF Slvcnt ----------- %d",Slvcnt);fflush(stdout);

								 if(Slvcnt>0)
								  {							 
									 for(mt=0;mt<Slvcnt;mt++)
									  {
										printf("\n AFTER MstrSlvrelation_type =========> \n"); fflush(stdout);
										MstrNo = Slvattachments[mt];
									
										if( AOM_ask_value_string(ChildRev,"item_id",&mastercontainer)!=ITK_ok);
										printf("\n IN Expansion_TPL_16 mastercontainer ---------  [%s]\n",mastercontainer); fflush(stdout);

										if( AOM_ask_value_string(MstrNo,"item_id",&mastercontainerMstr)!=ITK_ok);
										printf("\n IN Expansion_TPL_16 mastercontainer ---------  [%s]\n",mastercontainerMstr); fflush(stdout);
										if( AOM_ask_value_string(MstrNo,"item_revision_id",&mastercontainerrevID)!=ITK_ok);
										printf("\n IN Expansion_TPL_16 mastercontainerrevID ---------  [%s]\n",mastercontainerrevID); fflush(stdout);
		


										GRM_list_primary_objects_only(MstrNo,MstrSlvrelation_type,&Mstrcnt,&Mstrattachments);
										printf("\n  NO OF Mstrcnt ----------- %d",Mstrcnt);fflush(stdout);

										for (ms=0;ms<Mstrcnt;ms++)
										{
											if( AOM_ask_value_string(Mstrattachments[ms],"item_id",&containermst)!=ITK_ok);
											printf("\n IN Expansion_TPL_16 containermst ---------  [%s]\n",containermst); fflush(stdout)	;
											
											if(tc_strcmp(mastercontainer,containermst)==0)
											{
												
												if( AOM_ask_value_string(ChildRev,"item_revision_id",&mastercontainerrev)!=ITK_ok);//primary
												printf("\n IN Expansion_TPL_16 mastercontainer ---------  [%s]\n",mastercontainerrev); fflush(stdout);	
												
												if( AOM_ask_value_string(Mstrattachments[ms],"item_revision_id",&containermstrev)!=ITK_ok);//secondary
												printf("\n IN Expansion_TPL_16 containermst ---------  [%s]\n",containermstrev); fflush(stdout)	;

												if(tc_strcmp(containermstrev,mastercontainerrev) == 0)
												{

													totalstr = NULL;
													totalstr = MEM_alloc(50);

													tc_strcpy(totalstr,mastercontainerMstr);
													tc_strcat(totalstr,"_");
													tc_strcat(totalstr,mastercontainerrevID);	

													for(arr=0;arr<StructChldCnt;arr++)
													{
														if(tc_strcmp(totalArray[arr], totalstr) == 0)
														{
															sVehicleType=NULL;	
														sVehicleType=(char *) MEM_alloc(50);
														tc_strcpy(sVehicleType,"M_"); 
														tc_strcat(sVehicleType,mastercontainerMstr);
														printf("\n sVehicleType issssssssss. mastercontainer...[%s]..\n",sVehicleType);	fflush(stdout);
														break;
														}
													}
													printf("\n match found mastercontainer \n"); fflush(stdout);
													Flag_Master=1;												
													break;
												}

		
											}
										 }
									  }
								   }														

						      }
								
							  if(!sVehicleType)
							   {
								  sVehicleType=(char *) MEM_alloc(50);
								  tc_strcpy(sVehicleType,"NA");
							  }
					  }
				  } 

			

printf("\n sVehicleType issssssssss final ....[%s]..\n",sVehicleType);	fflush(stdout);
					if( AOM_ask_value_string(ChildRev,"t5_EcuType",&Cnt_EcuType)!=ITK_ok);
					printf("\t ChildRev t5ECUType   is -----> [%s]\n",Cnt_EcuType); fflush(stdout);

					char * Cnt_EcuTypeDup =NULL;
					Cnt_EcuTypeDup=(char *) MEM_alloc(100);

					if((Cnt_EcuType)!= NULL)
					{
						printf("\n Cnt_EcuType is NOT NULL-----> [%s]\n",Cnt_EcuType); fflush(stdout);

						if(tc_strstr(Cnt_EcuType,"Passive Entry Passive Start")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"PEPS");
						else if(tc_strstr(Cnt_EcuType,"Engine Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ECM");
						else if (tc_strstr(Cnt_EcuType,"Antilock Braking System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ABS");
						else if (tc_strstr(Cnt_EcuType,"Antilock/Electronic Braking System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ABS");
						else if (tc_strstr(Cnt_EcuType,"Electronic Stability Program")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ESP");
						else if (tc_strstr(Cnt_EcuType,"Restraints")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"RCM");
						else if (tc_strstr(Cnt_EcuType,"Body Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"BCM");
						else if (tc_strstr(Cnt_EcuType,"Heating, Ventilation, and Air Conditioning")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"HVA");
						else if (tc_strstr(Cnt_EcuType,"Fully Automated Temperature Control")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"FATC");
						else if (tc_strstr(Cnt_EcuType,"Infotainment Head Unit")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"IHU");
						else if (tc_strstr(Cnt_EcuType,"Vehicle Immobilizer Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"VICM");
						else if (tc_strstr(Cnt_EcuType,"Park Distance Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"PACM");
						else if (tc_strstr(Cnt_EcuType,"Power Steering Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"PSCM");
						else if (tc_strstr(Cnt_EcuType,"Tire Pressure Monitor System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TPMS");
						else if (tc_strstr(Cnt_EcuType,"Transmission Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TCM");
						else if (tc_strstr(Cnt_EcuType,"Drive Mode Controller/Switch")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"DMC");
						else if (tc_strstr(Cnt_EcuType,"Transmission control unit")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TCU");
						else if (tc_strstr(Cnt_EcuType,"Instrument Panel Cluster")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"IPC");
						else if (tc_strstr(Cnt_EcuType,"Torque On Demand Controller")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TOD");
						else if (tc_strstr(Cnt_EcuType,"Window Winding Auto Down Controller")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"WWAD");
						else if (tc_strstr(Cnt_EcuType,"Retarder")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"RTD");
						else if (tc_strstr(Cnt_EcuType,"Tachograph")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TCG");
						else if (tc_strstr(Cnt_EcuType,"Telematics")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TMC");
						else if (tc_strstr(Cnt_EcuType,"Multiplexing")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"MPX");
						else if (tc_strstr(Cnt_EcuType,"Hybrid ECU")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"HCU");
						else if (tc_strstr(Cnt_EcuType,"Auto Transmission System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ATS");
						else if (tc_strstr(Cnt_EcuType,"Electronically Controlled  Air Suspension")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ECAS");
						else if (tc_strstr(Cnt_EcuType,"Emergency Braking System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"EBS");
						else if (tc_strstr(Cnt_EcuType,"Exhaust Gas Recirculation")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"EGR");
						else if (tc_strstr(Cnt_EcuType,"Selective Catalytic Reduction")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"SCR");
						else if (tc_strstr(Cnt_EcuType,"Electronic Steering Column Lock")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ESCL");
						else if (tc_strstr(Cnt_EcuType,"Unique Identification")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"UI");
						else if (tc_strstr(Cnt_EcuType,"DC-DC Converter")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"DDC");
						else if (tc_strstr(Cnt_EcuType,"Motor Controller Unit")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"MCU");
						else if (tc_strstr(Cnt_EcuType,"Battery Management System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"BMS");
						else if (tc_strstr(Cnt_EcuType,"Gear Shift Advisor")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"GSA");
						else if (tc_strstr(Cnt_EcuType,"Advanced Driver Assistance System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ADAS");
						else if (tc_strstr(Cnt_EcuType,"On Board Charger")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"OBC");
						else if (tc_strstr(Cnt_EcuType,"Wireless Power Charger")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"WPC");
						else if (tc_strstr(Cnt_EcuType,"Dosing Control Unit")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"DCU");
						else if (tc_strstr(Cnt_EcuType,"Battery Cooling System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"BCS");
						else
						tc_strcpy(Cnt_EcuTypeDup,"NA");
					}

					ITKCALL(GRM_find_relation_type("T5_HasSpareKit", &sp_kit_rel_type));

					if(sp_kit_rel_type != NULLTAG)
					{
						printf("\n\t Expansion_TPL_16 SPARE KIT relation found......");	fflush(stdout);	
																												 
						ITKCALL(GRM_list_secondary_objects_only(SpkitTag,sp_kit_rel_type,&cnt,&attachments));
						CHECK_FAIL;

						if (cnt > 0)
						{
						  for(jj=0;jj<cnt;jj++)
						  {
							Sparekit = attachments[jj];

							if( AOM_ask_value_string(Sparekit,"item_id",&Sp_ItemID)!=ITK_ok);
							printf("\t Expansion_TPL_16 Sp_ItemID NUMBER  is [%s]\n",Sp_ItemID); fflush(stdout);

							if(AOM_ask_value_string(Sparekit,"item_revision_id",&Sp_ItemREV)!=ITK_ok);
							 printf("\t Expansion_TPL_16 Sp_ItemREV NUMBER  is [%s]\n",Sp_ItemREV); fflush(stdout);

							char * HwPart =NULL;
							HwPart=(char *) MEM_alloc(100);
							
							char * HwPart_Sm =NULL;
							HwPart_Sm=(char *) MEM_alloc(100);

							char * ContEOLDup_Val =NULL;
							ContEOLDup_Val=(char *) MEM_alloc(100);

							 if(ContEOL)
							 {
								
								if(tc_strstr(ContEOL,"BothES")!=NULL) 
								{					
									tc_strcpy(ContEOLDup,ValBoth);
								}
								else if (tc_strstr(ContEOLDup,"NotApp")!=NULL)
								{					
									tc_strcpy(ContEOLDup,ValBoth);
								}
								else if (tc_strstr(ContEOLDup,"Service")!=NULL)
								{					
									tc_strcpy(ContEOLDup,ServVal);
								}
								else
								{
									tc_strcpy(ContEOLDup_Val,ContEOLDup);
								}
							 }
								tc_strcpy(HwPart,Sp_ItemID);
								tc_strcat(HwPart,",");
								tc_strcat(HwPart,"NA");
								tc_strcat(HwPart,",");
								tc_strcat(HwPart,Sp_ItemREV);
								tc_strcat(HwPart,"_");
								tc_strcat(HwPart,ContEOLDup_Val);


								tc_strcpy(HwPart_Sm,Sp_ItemID);
								tc_strcat(HwPart_Sm,",");
								tc_strcat(HwPart_Sm,Sp_ItemREV);

							}
						}
					}

					printf ("\n Expansion_TPL_16 fptr11111111111111111 ---> \n"); fflush(stdout);

					//fprintf(fptr1," \n %s,%s,%s,%s,%s \n",Cnt_EcuTypeDup,container,containerrevdup,DML_name,req_item);fflush(fptr1);
					//fprintf(fp1,  "\ n %s,%s %s,%s,%s,%s,%s,%s,%s,%s,\n",BaseLineNumberDupS,BaseLineRevDupS,BaseLineSeqDupS,BaseLineAppDupS,vcnoDup,vcdescDup,DML_name,t5NoEcuDup,vcDrStatDup,vcProjDup);fflush(fp1);

                   //ChildRev

				   	//ifail = BOM_create_window (&window2);
					//CHECK_FAIL;

					//ifail = CFM_find( "Latest Released", &rule );
					//ifail = CFM_find( "ERC release and above", &rule );

					//printf ("\n after rev rule window2 \n"); fflush(stdout);

					//ifail = BOM_set_window_config_rule( window2, rule );
					//CHECK_FAIL;

					//ifail = BOM_set_window_top_line (window2, null_tag, ChildRev, null_tag, &top_line2);
					//CHECK_FAIL;

					//ifail = BOM_window_show_suppressed ( window2 );
					//CHECK_FAIL;

					//ifail = (BOM_set_window_pack_all(window2, FALSE));

					countZZ=0;
					statusZZ = BOM_line_ask_child_lines (ChildRevBomLine, &countZZ, &childrenZZ);

					printf ("\n line 1120 IN Expansion_TPL_16 countZZ is --->%d \n",countZZ); fflush(stdout);

					if(countZZ >0)
					 {
						//printf("\n..line 1124  Expansion_TPL_16 \n");fflush(stdout);
						// CalFileNameDup=NULL;
						 test_file2=NULL;
						 //CalFileNameDup			 =(char *) MEM_alloc(300);
						 //test_file2			 =(char *) MEM_alloc(300);
						 for(yy=0;yy<countZZ;yy++)
						    {
							   // CalFileNameDup=NULL;
						      //  CalFileNameDup			 =(char *) MEM_alloc(300);
							 // tc_strcpy(CalFileNameDup,"");
							  printf("---------------------------------------------------------------------------------------------------------------------------------------------------");fflush(stdout);

							  
								if(AOM_ask_value_string(childrenZZ[yy],"bl_item_item_id",&contpart)!=ITK_ok);
													
								if( AOM_ask_value_string(childrenZZ[yy],"bl_rev_item_revision_id",&contpartrev)!=ITK_ok);
								//printf("\n contpartrev ID--->[%s] \n",contpartrev);fflush(stdout);

								//if( AOM_ask_value_string(childrenZZ[yy],"bl_Design Revision_t5_ProjectCode",&ProjectC)!=ITK_ok);
								printf("\n  1135  Expansion_TPL_16  contpart ID  is---> [%s] ontpartrev ID--->[%s] \n",contpart,contpartrev);fflush(stdout);

								/////adiiiiiiiiii adddddddd condition here
								if((contpartrev)!=NULL && tc_strlen(contpartrev)>0)
								{

								const char **attrs6  = (const char **) MEM_alloc(2 * sizeof(char *));
								const char **values6 = (const char **) MEM_alloc(2 * sizeof(char *));
								attrs6[0]	   = "item_id";
								//attrs6[1]      = "object_type";
								values6[0]     = (char *)contpart;
								//values6[1]     = "Design";
								//ifail = ITEM_find_items_by_key_attributes(2,attrs, values, &n_tags_found, &tags_found);
								ifail = ITEM_find_item_revs_by_key_attributes(1,attrs6, values6,contpartrev, &n_tags_found6, &tags_found6);
								CHECK_FAIL;
								MEM_free(attrs6);
								MEM_free(values6);

								if (n_tags_found6>0)
								{
								   ContChildRev= tags_found6[0];
								   printf("\n ContChildRev tags founddd \n");fflush(stdout);
								}

								if( AOM_ask_value_string(ContChildRev,"item_id",&conitemid)!=ITK_ok);

								printf("\n Expansion_TPL_16 conitemid  is----> [%s]\n",conitemid);

								if( AOM_ask_value_string(ContChildRev,"item_revision_id",&conitemrevid)!=ITK_ok);//A;1

								printf("\n Expansion_TPL_16 conitemrevid  is----> [%s]\n",conitemrevid);

								char	*strToBerepl1RR =NULL;
								char	*strToBeUsed1RR =NULL;
								char	*conitemreviddup=NULL;//A;1

								strToBerepl1RR		=	(char*)malloc(5);
								tc_strcpy(strToBerepl1RR,";");

								strToBeUsed1RR	=	(char*)malloc(5);
								tc_strcpy(strToBeUsed1RR,"_");

								ifail =(STRNG_replace_str (conitemrevid,strToBerepl1R, strToBeUsed1R,&conitemreviddup));
								printf("\n conitemreviddup    ConProject is ---> [%s]\n",conitemreviddup);fflush(stdout);//A_1
								
								char	*conitemreviddupS		=	NULL;
								char	*conitemrevidduptokS	=	NULL;
								conitemreviddupS		=	(char*)malloc(10);
								tc_strcpy(conitemreviddupS,conitemreviddup);
								printf("\n Expansion_TPL_16 conitemreviddupS  is----> [%s]\n",conitemreviddupS);
								printf("\n Expansion_TPL_16 conitemreviddup AAAA  is----> [%s]\n",conitemreviddup);

								conitemrevidduptokS = tc_strtok (conitemreviddupS,"_");
								conitemreviddupS =tc_strtok(NULL,"");
								printf("\n Expansion_TPL_16 conitemrevidduptokS  is----> [%s]\n",conitemrevidduptokS);//A
								printf("\n Expansion_TPL_16 conitemreviddupS AFTER TOK is----> [%s]\n",conitemreviddupS);//1


								if( AOM_ask_value_string(ContChildRev,"t5_ProjectCode",&ProjectC)!=ITK_ok);

								if( AOM_ask_value_string(ContChildRev,"t5_AppForEOL",&EOL)!=ITK_ok);

								if( AOM_ask_value_string(ContChildRev,"t5_SwPartType",&ChdPartType)!=ITK_ok);

								if( AOM_ask_value_string(ContChildRev,"t5_DesignGrp",&Desg)!=ITK_ok);

								printf("\n Expansion_TPL_16 Desg GROUP  is----> [%s], ProjectC is --->[%s],EOL  is----> [%s],ChdPartType is ---->[%s]\n",Desg,ProjectC,EOL,ChdPartType); fflush(stdout);

								char	*strToBereplR=NULL;
								char	*strToBeUsedR=NULL;
				
								strToBereplR		=	(char*)malloc(5);
								tc_strcpy(strToBereplR,";");

								strToBeUsedR	=	(char*)malloc(5);
								tc_strcpy(strToBeUsedR,"_");

								ifail =(STRNG_replace_str (contpartrev,strToBereplR, strToBeUsedR,&contpartrevdup));



								char	*strToBereplRQ=NULL;
								char	*strToBeUsedRQ=NULL;
								char	*contpartrevdupSS11=NULL;
								char	*contpartrevdupSS11copy=NULL;
								char	*contpartrevdupSS=NULL;
								contpartrevdupSS=	(char*)malloc(30);
								contpartrevdupSS11copy=	(char*)malloc(30);
								char	*contpartrevdupSS11tokS		=	NULL;
								char	*contpartseqdupSS11	=	NULL;
					

								tc_strcpy(contpartrevdupSS,contpartrevdup);
								printf("\n Expansion_TPL_16 contpartrevdupSS ------------>[%s] \n",contpartrevdupSS);fflush(stdout);//G_2

								
								strToBereplRQ		=	(char*)malloc(5);
								tc_strcpy(strToBereplRQ,"_");
	
								strToBeUsedRQ	=	(char*)malloc(5);
								tc_strcpy(strToBeUsedRQ,",");

								ifail =(STRNG_replace_str (contpartrevdupSS,strToBereplRQ, strToBeUsedRQ,&contpartrevdupSS11));//g,2
								tc_strcpy(contpartrevdupSS11copy,contpartrevdupSS11);

				
								//contpartrevdupSS11tokS = tc_strtok (contpartrevdupSS11copy,"_");
								//contpartseqdupSS11 =tc_strtok(NULL,"");

								printf("\n Expansion_TPL_16 contpartrevdupSS11copy  is----> [%s]\n",contpartrevdupSS11copy);//G,2   G
								//printf("\n Expansion_TPL_16 contpartseqdupSS11 AFTER TOK is----> [%s]\n",contpartseqdupSS11);//NULL   2


									
								char * EOLDup =NULL;
								EOLDup=(char *) MEM_alloc(100);
								if((EOL)!=NULL && tc_strlen(EOL)>0)
								{
									
								   if(tc_strstr(EOL,"BothES")!=NULL) 
									{
										tc_strcpy(EOLDup,ValBoth);
										printf("\n..AFTER ValBoth COPY EOLDUP VALUE IS------>[%s] \n",EOLDup);fflush(stdout);
									}
								   else if (tc_strstr(EOL,"NotApp")!=NULL)
									{
										tc_strcpy(EOLDup,EOLNA);
										printf("\n..AFTER EOLNA COPY EOLDUP VALUE IS------>[%s] \n",EOLDup);fflush(stdout);
									}
								   else if (tc_strstr(EOL,"Service")!=NULL)
									{
										tc_strcpy(EOLDup,ServVal);
										printf("\n..AFTER ServVal COPY EOLDUP VALUE IS------>[%s] \n",EOLDup);fflush(stdout);
									}
								}
								else 
								{ 
									tc_strcpy(EOLDup,NoValStr);
									printf("\n..AFTER EOL COPY EOLDUP VALUE IS------>[%s] \n",EOLDup);fflush(stdout);
								}
								
								if((EOLDup)!=NULL && tc_strlen(EOLDup)>0)  
								{
								   	printf("\n..AAAAAA Expansion_TPL_16 EOLDup value is ------>[%s] \n",EOLDup);fflush(stdout);

								}
								else
								{
									//printf("..Expansion_TPL_16 in else loop of contpart %s",contpart);fflush(stdout);
									tc_strcpy(EOLDup,NoValStr);
									printf("\n..BBBBBB Expansion_TPL_16 EOLDup value is ------>[%s] \n",EOLDup);fflush(stdout);
								}

								//printf("\n Expansion_TPL_16 ContChildRev  is----> [%s] [%s]\n",conitemid ,conitemrevid);fflush(stdout);

								ifail = GRM_find_relation_type("IMAN_specification",&relation_type); 
			
								ifail = GRM_list_secondary_objects_only(ContChildRev,relation_type,&cntX,&attachmentsX);
								printf("\n attachmentsX ============= %d \n",cntX); fflush(stdout);	
									if (cntX > 0)
									  {	
										tc_strcpy(test_file2,"");
										test_file =NULL;
											test_file2 =NULL;
											test_fileX =NULL;
											test_file3 =NULL;
											pathname =NULL;
											test_file=(char *) MEM_alloc(300);
											test_file2=(char *) MEM_alloc(300);
											test_fileX=(char *) MEM_alloc(300);
											test_file3=(char *) MEM_alloc(300);
										for(bb=0;bb<cntX;bb++)
										{	

											dataset = attachmentsX[bb];
											if( AOM_ask_value_string(dataset,"object_type",&objtype)!=ITK_ok);
											printf("\n  Expansion_TPL_16 objtype is ======== %s \n",objtype);fflush(stdout);
											

											if((tc_strstr(objtype,"CMI2")!=NULL)||(tc_strstr(objtype,"CAT")!=NULL)||(tc_strstr(objtype,"Creo")!=NULL)||(tc_strstr(objtype,"ProPrt")!=NULL)||(tc_strstr(objtype,"ProDrw")!=NULL))
											 {
												printf("\n Expansion_TPL_16  proe OR catia datasets SO CONTINUE \n"); fflush(stdout);
												
												//tc_strcpy(test_file,NoValStr);

												tc_strcpy(test_file,"NA");
												printf("\n else test_file not ok ------ %s\n",test_file); fflush(stdout);
												
												tc_strcpy(test_file2,"");
												tc_strcat(test_file2,"NA");
												printf("\n else test_file2 not ok ------ %s\n",test_file2); fflush(stdout);

											 }
											 else
											 {

												printf("\n  objtype not creo or catia \n");fflush(stdout);
												ifail = AE_find_dataset_named_ref(dataset,0,refname,&reftype,&namedrefobject);
												//ifail = AE_ask_dataset_named_ref2(dataset,"T5_abm",&reftype,&namedrefobject);
												//CHECK_FAIL;
												printf("\n after AE_find_dataset_named_ref \n");fflush(stdout);
												//ask_property_value_by_name(objtype, "Type", &type);

												//printf("\n type ======== \n",&type);fflush(stdout);

												if(namedrefobject)
												{
													//if(tc_strcmp(objtype,"T5_IndepBin")==0)
												    if((tc_strcmp(objtype,"T5_IndepBin")==0)||(tc_strcmp(objtype,"MSWordX")==0)||(tc_strcmp(objtype,"MSWord")==0)
														||(tc_strcmp(objtype,"MSWordTemplate")==0)||(tc_strcmp(objtype,"MSWordTemplateX")==0))
													{
														printf("\t namedrefobject found \n"); fflush(stdout);
														ifail =(IMF_ask_original_file_name(namedrefobject,orig_name));
														printf("\t orig_name: in Rendering [%s]\n",orig_name); fflush(stdout);

														ifail = IMF_ask_file_pathname2(namedrefobject,mach_type,&pathname);	
														//ifail =(IMF_ask_file_pathname(namedrefobject,mach_type,pathname));	
														printf("\t pathname [%s]\n",pathname); fflush(stdout);

														//ifail = (IMF_ask_relative_path2(namedrefobject,&relative_path));
														//printf("\t relative_path [%s]\n",relative_path); fflush(stdout);

														ifail =( AOM_UIF_ask_value(namedrefobject,"file_size",&byte1));
														printf("\n byte1 size is --------- [%s]\n",byte1); fflush(stdout);

														ifail =( AOM_UIF_ask_value(namedrefobject,"file_name",&name));
														printf("\n name name is --------- [%s]\n",name); fflush(stdout);

														ifail =( AOM_ask_value_string(namedrefobject,"byte_size",&byte_size));
														printf("\n byte_sizeis --------- [%s]\n",byte_size); fflush(stdout);
																												
														//byte1int = atoi(byte1);
														byte1int = atol(byte1);
														printf("\n byte1int size is --------- [%d]\n",byte1int); fflush(stdout);
	

														tc_strcpy(test_file,pathname);
														printf("\n test_file [%s]\n",test_file); fflush(stdout);

														tc_strcpy(test_fileX,"/tmp");
														tc_strcat(test_fileX,"/");
														tc_strcat(test_fileX,orig_name);
														printf("\n test_fileX [%s]\n",test_fileX); fflush(stdout);
														
														tc_strcpy(test_file2,"");
														tc_strcat(test_file2,orig_name);
														printf("\n test_file2 [%s]\n",test_file2); fflush(stdout);////

//														tc_strcpy(test_file3,test_file2);
//														printf("\n test_file3 new [%s]\n",test_file3); fflush(stdout);
//
//														ItemId1 = tc_strtok(test_file3, ".");
//														ItemId2 = tc_strtok(NULL, ".");//.s //.swp
//
//														printf("\n test_file2 after after  [%s]\n",test_file2); fflush(stdout);
//														printf("\n ItemId1 after after -------- [%s]\n",ItemId1); fflush(stdout);
//														printf("\n ItemId2 after after -------- [%s]\n",ItemId2); fflush(stdout);
//
														//T5_IndepBin
//														printf("\t Inside reference_nameDS %s\n",reference_nameDS); fflush(stdout);
														tc_strcpy(Exfile," ");
														//strcpy(Exfile,"/home/testcmi/Adi/DIGIBUCK/BOMListJT/");
														//tc_strcpy(Exfile,"/user/bsd05818/Adi/");
														tc_strcpy(Exfile,"/tmp/");
														tc_strcat(Exfile,orig_name);
														printf("\t exporting now\n"); fflush(stdout);
														//AE_export_named_ref(dataset,reference_nameDS,Exfile);
														IMF_export_file(namedrefobject,Exfile);
														printf("\t Exfile is ---- [%s]\n",Exfile); fflush(stdout);
														strcpy(Exfile," ");
														flagX=0;
													}
//														else
//														{
//															tc_strcpy(test_file2,"");
//															tc_strcat(test_file2,"NA");
//															printf("\n AAAAA test_file2 AAAAAA [%s]\n",test_file2); fflush(stdout);
//														}
													}
													else
													{
														printf("\n problem with named reference of tpl \n"); fflush(stdout);
														flagX=1;
														tc_strcpy(test_file2,"");
														tc_strcat(test_file2,"NA");
														printf("\n AAAAA test_file2 AAAAAA [%s]\n",test_file2); fflush(stdout);
													}

															supfile=1;
															//if  ((nlsStrCmp(ChdPartTypeDup,"CAL") == 0 || nlsStrCmp(ChdPartTypeDup,"HCL") == 0) && (nlsStrStr(ContEOLDup,"Both")!=NULL || nlsStrCmp(ContEOLDup,"EOL")==0))

															if  (((tc_strcmp(ChdPartType,"CAL") == 0)|| tc_strcmp(ChdPartType,"HCL") == 0) && ((tc_strstr(ContEOLDup,"Both")!=NULL) || tc_strcmp(ContEOLDup,"EOL")==0))
															{
																printf("\n..Container having Applicability ......%s",ContEOLDup);fflush(stdout);
																CalFlileReq=1;
															}
														//}
												   //}
											    }											

											//CalFileName=(char *) MEM_alloc(100);
											

											if (!(tc_strcmp(ChdPartType,"CAL") == 0 || tc_strcmp(ChdPartType,"HCL") == 0))
											{
												printf("\n Part is not calibaration SW");fflush(stdout);
												//calfiletest=0;
												
											}
											else
											{										
												//tc_strcpy(CalFileName,relative_path);
												if ((tc_strstr(TPLEOLVAL,"BOTH")!=0) || (tc_strstr(TPLEOLVAL,"EOL")!=0))
												{
													calfiletest=1;
												}
												else
												{
													calfiletest=0;
												}
											}

										//if((tc_strcmp(test_file,"NA") != 0 ) && (flagX!=1))
										if((tc_strcmp(test_file,"NA") != 0 ) && (flagX==0))
											{
												printf("\n EOLDup is  [%s]   &  ContEOLDup is  [%s]  ContEOLDup_Val is  [%s]============ \n",EOLDup,ContEOLDup,ContEOLDup_Val);fflush(stdout);
												//fprintf(fptr5,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%ld \n",req_item,container,conitemid,test_file2,contpartrevdup,conitemrevidduptokS,conitemreviddupS,DML_name,EOLDup,byte1int);fflush(fptr5); 
												//fprintf(fptr5,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%ld \n",VCnumberDup,container,conitemid,test_file2,contpartrevdup,conitemrevidduptokS,conitemreviddupS,DML_name,EOLDup,byte1int);fflush(fptr5); 
												fprintf(fptr5,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%ld \n",VCnumberDup,container,conitemid,test_file2,containerrevdup,conitemrevidduptokS,conitemreviddupS,DML_name,EOLDup,byte1int);fflush(fptr5); 
												//fprintf(fp5,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%ld,\n",vcnoDup,containerDup,contpartDup,test_file,containerrevdup,ChdPartRevDup,ChdPartSeqDup,DmlNo,EOLDup,byte1);fflush(fp5);

																																			//contpartrevdup dont change , conitemreviddupS dont change
												//fprintf(fp5,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%ld,\n",vcnoDup,containerDup,contpartDup,test_file,containerrevdup,ChdPartRevDup,ChdPartSeqDup,DmlNo,EOLDup,byte1);fflush(fp5);

												printf("\n NEWWWWWWWWWWWWWWWW fptr4444444444444444  ============ \n");fflush(stdout);
												
												//fprintf(fptr4,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",ConProject,Desg,test_fileX,conitemid,contpartrevdupSS11tokS,NewDate,container,containerrevdup,DML_name,EOLDup,test_file2);fflush(fptr4);
												fprintf(fptr4,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",vcProjDup,Desg,test_fileX,conitemid,contpartrevdupSS11copy,NewDate,container,containerrevdup,DML_name,EOLDup,test_file2);fflush(fptr4);
												//replacing container project by vc project

											}
										}
									}

						    iWrite  =0;	
						   
							printf("\n Expansion_TPL_16 contpartrevdup ------------>[%s] \n",contpartrevdup);fflush(stdout);//G_2
							printf("\n Expansion_TPL_16 ChdPartType ------------>[%s] \n",ChdPartType);fflush(stdout);//G_2
							printf("\n Expansion_TPL_16 test_file2 ------------>[%s] \n",test_file2);fflush(stdout);

//							char	*strToBereplRQ=NULL;
//							char	*strToBeUsedRQ=NULL;
//							char	*contpartrevdupSS11=NULL;
//							char	*contpartrevdupSS=NULL;
//							contpartrevdupSS=	(char*)malloc(30);
//				
//
//							tc_strcpy(contpartrevdupSS,contpartrevdup);
//							printf("\n Expansion_TPL_16 contpartrevdupSS ------------>[%s] \n",contpartrevdupSS);fflush(stdout);//G_2
//
//								
//								strToBereplRQ		=	(char*)malloc(5);
//								tc_strcpy(strToBereplRQ,"_");
//	
//								strToBeUsedRQ	=	(char*)malloc(5);
//								tc_strcpy(strToBeUsedRQ,",");
//
//								ifail =(STRNG_replace_str (contpartrevdupSS,strToBereplRQ, strToBeUsedRQ,&contpartrevdupSS11));

						  
						   if (tc_strcmp(ChdPartType, "STK") == 0)
							{							  
								flag=1;
								Sticker	=(char *) MEM_alloc(tc_strlen(contpart)+tc_strlen(contpartrevdupSS11)+tc_strlen(EOLDup)+25);
								tc_strcpy(Sticker,contpart);
								tc_strcat(Sticker,",");
								tc_strcat(Sticker,contpartrevdupSS11);
								tc_strcat(Sticker,"_");
								tc_strcat(Sticker,EOLDup);
								printf("\n Expansion_TPL_16 Sticker ------------>[%s] \n",Sticker);fflush(stdout);
							}
							else if (tc_strcmp(ChdPartType, "PBL") == 0)
							{
								Pr_Boot_Loader_Sm=(char *) MEM_alloc(200);
								Pr_Boot_Loader	 =(char *) MEM_alloc(100);

								tc_strcpy(Pr_Boot_Loader_Sm,contpart);
								tc_strcat(Pr_Boot_Loader_Sm,",");
								tc_strcat(Pr_Boot_Loader_Sm,contpartrevdupSS11);

								printf("\n Expansion_TPL_16 PBL Pr_Boot_Loader_Sm ------------>[%s] \n",Pr_Boot_Loader_Sm);fflush(stdout);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart);//?????
								setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(Pr_Boot_Loader,contpart);
								tc_strcat(Pr_Boot_Loader,",");
								tc_strcat(Pr_Boot_Loader,contpartrevdupSS11);
								tc_strcat(Pr_Boot_Loader,"_");
								tc_strcat(Pr_Boot_Loader,EOLDup);
								printf("\n Expansion_TPL_16 PBL Pr_Boot_Loader ------------>[%s] \n",Pr_Boot_Loader);fflush(stdout);

							}
							else if (tc_strcmp(ChdPartType, "SBL") == 0)
							{
								Se_Boot_Loader_Sm =(char *) MEM_alloc(200);
								Se_Boot_Loader	 =(char *) MEM_alloc(100);

								tc_strcpy(Se_Boot_Loader_Sm,contpart);
								tc_strcat(Se_Boot_Loader_Sm,",");
								tc_strcat(Se_Boot_Loader_Sm,contpartrevdupSS11);

								printf("\n Expansion_TPL_16 SBL Se_Boot_Loader_Sm ------------>[%s] \n",Se_Boot_Loader_Sm);fflush(stdout);

								
								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart); //?????
								setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(Se_Boot_Loader,contpart);
								tc_strcat(Se_Boot_Loader,",");
								tc_strcat(Se_Boot_Loader,contpartrevdupSS11);
								tc_strcat(Se_Boot_Loader,"_");
								tc_strcat(Se_Boot_Loader,EOLDup);

								printf("\n Expansion_TPL_16 SBL Se_Boot_Loader ------------>[%s] \n",Se_Boot_Loader);fflush(stdout);


							}
							else if (tc_strcmp(ChdPartType, "BAS") == 0)
							{
								BasicSw			 =(char *) MEM_alloc(100);

								//low_set_add_str(SWDetails,contpart); //?????
								tc_strcpy(BasicSw,contpart);
								tc_strcat(BasicSw,"_");
								tc_strcat(BasicSw,contpartrevdup);
								tc_strcat(BasicSw,"_");
								tc_strcat(BasicSw,EOLDup);

								printf("\n Expansion_TPL_16 BAS BasicSw ------------>[%s] \n",BasicSw);fflush(stdout);

							}
							else if (tc_strcmp(ChdPartType, "APP") == 0)
							{
								AppSw			 =(char *) MEM_alloc(200);
								AppSw_Sm	     =(char *) MEM_alloc(200);
								tc_strcpy(AppSw_Sm,contpart);
								tc_strcat(AppSw_Sm,",");
								tc_strcat(AppSw_Sm,contpartrevdupSS11);

								printf("\n Expansion_TPL_16 APP AppSw_Sm ------------>[%s] \n",AppSw_Sm);fflush(stdout);
								
								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart);//>????
								setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(AppSw,contpart);
								tc_strcat(AppSw,",");
								tc_strcat(AppSw,contpartrevdupSS11);
								tc_strcat(AppSw,"_");
								tc_strcat(AppSw,EOLDup);

								

							}
							else if (tc_strcmp(ChdPartType,"CAL") == 0)
							{
								CalFileNameDup = NULL;

								printf("\n Expansion_TPL_16 test_file2 ------------>[%s] \n",test_file2);fflush(stdout);
																
								CalSw_Sm	     =(char *) MEM_alloc(100);
								CalSw			 =(char *) MEM_alloc(100);
								CalFileNameDup			 =(char *) MEM_alloc(100);
								tc_strcpy(CalSw_Sm,"");
								tc_strcat(CalSw_Sm,contpart);
								tc_strcat(CalSw_Sm,",");
								//tc_strcat(CalSw_Sm,CalFileName);
								tc_strcat(CalSw_Sm,test_file2);
								tc_strcat(CalSw_Sm,",");
								tc_strcat(CalSw_Sm,contpartrevdupSS11);
								printf("\n Expansion_TPL_16 CAL CalSw_Sm ------------>[%s] \n",CalSw_Sm);fflush(stdout);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

							//low_set_add_str(SWDetails,contpart);//????
							   setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(CalSw,contpart);
								tc_strcat(CalSw,",");
								tc_strcat(CalSw,contpartrevdupSS11);
								tc_strcat(CalSw,"_");
								tc_strcat(CalSw,EOLDup);

								//tc_strcpy(CalSwDup,contpartrev);
								//tc_strcat(CalSwDup,"_");
								//tc_strcat(CalSwDup,EOLDup);

								printf("\n Expansion_TPL_16 CAL CalSw ------------>[%s] \n",CalSw);fflush(stdout);

								tc_strcpy(CalFileNameDup,"");
								tc_strcat(CalFileNameDup,test_file2);
								printf("\n CalFileNameDup CAL CalSw isssss  ------------>[%s] \n",CalFileNameDup);fflush(stdout);
								
							if(CalFileNameDup)
								 {
									printf("\n	value in CALFILENAME so continue %s  \n",CalFileNameDup);fflush(stdout);
									//break;
								 }
								else
								 {	
									tc_strcpy(CalFileNameDup,"");
									tc_strcat(CalFileNameDup,"NA");
									printf("\n cal file is null so setting to  %s\n",CalFileNameDup);fflush(stdout);
								 }

								printf("\n 2121221212221211111 CalFileNameDup CAL CalSw isssss  ------------>[%s] \n",CalFileNameDup);fflush(stdout);

							}
							else if (tc_strcmp(ChdPartType, "CFG") == 0)
							{
								CfgSw_Sm		 =(char *) MEM_alloc(200);
								CfgSw		     =(char *) MEM_alloc(100);
								tc_strcpy(CfgSw_Sm,contpart);
								tc_strcat(CfgSw_Sm,",");
								tc_strcat(CfgSw_Sm,contpartrevdupSS11);

								printf("\n Expansion_TPL_16 CFG CfgSw_Sm ------------>[%s] \n",CfgSw_Sm);fflush(stdout);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart);//????
								setAddStr(&iWrite,&SWDetails,contpart);
								tc_strcpy(CfgSw,contpart);
								tc_strcat(CfgSw,",");
								tc_strcat(CfgSw,contpartrevdupSS11);
								tc_strcat(CfgSw,"_");
								tc_strcat(CfgSw,EOLDup);

								printf("\n Expansion_TPL_16 CFG CfgSw ------------>[%s] \n",CfgSw);fflush(stdout);

							}
							else if (tc_strcmp(ChdPartType, "VCI") == 0)
							{
								VehCal			 =(char *) MEM_alloc(100);
								VehCal_Sm		 =(char *) MEM_alloc(200);

								tc_strcpy(VehCal_Sm,contpart);
								tc_strcat(VehCal_Sm,",");
								tc_strcat(VehCal_Sm,contpartrevdupSS11);

								printf("\n Expansion_TPL_16  VCI VehCal_Sm ------------>[%s] \n",VehCal_Sm);fflush(stdout);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart);//????
								setAddStr(&iWrite,&SWDetails,contpart);
								tc_strcpy(VehCal,contpart);
								tc_strcat(VehCal,",");
								tc_strcat(VehCal,contpartrevdupSS11);
								tc_strcat(VehCal,"_");
								tc_strcat(VehCal,EOLDup);

								printf("\n Expansion_TPL_16 VCI VehCal ------------>[%s] \n",VehCal);fflush(stdout);

							}
							else if (tc_strcmp(ChdPartType,"HWC") == 0)
							{
								HwPart			 =(char *) MEM_alloc(100);
								HwPart_Sm		 =(char *) MEM_alloc(200);

								tc_strcpy(HwPart_Sm,contpart);
								tc_strcat(HwPart_Sm,",");
								tc_strcat(HwPart_Sm,contpartrevdupSS11);
								printf("\n Expansion_TPL_16 HWC HwPart_Sm ------------>[%s] \n",HwPart_Sm);fflush(stdout);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(HWDetails,contpart);//?????
								setAddStr(&iWrite,&HWDetails,contpart);
								tc_strcpy(HwPart,contpart);
								tc_strcat(HwPart,",");
								tc_strcat(HwPart,contpartrevdupSS11);
								//tc_strcat(HwPart,"_");
								//tc_strcat(HwPart,EOLDup);
								printf("\n Expansion_TPL_16 HWC HwPart ------------>[%s] \n",HwPart);fflush(stdout);

								//if(HwPart)//just added.
								 //{
									//printf("\n some value in  HwPart so continue %s  \n",HwPart);fflush(stdout);
									//break;
								 //}/
							}
							else if (tc_strcmp(ChdPartType,"HCL") == 0)
							{
								CalSw		 =(char *) MEM_alloc(200);
								CalSwDup	 =(char *) MEM_alloc(200);
								VehCal	     =(char *) MEM_alloc(100);
								CalSw_Sm	 =(char *) MEM_alloc(200);
								VehCal_Sm	 =(char *) MEM_alloc(100);
								//CalFileNameDup			 =(char *) MEM_alloc(100);
								//Cal code
								tc_strcpy(CalSw_Sm,contpart);
								tc_strcat(CalSw_Sm,",");
								//tc_strcat(CalSw_Sm,CalFileName);
								tc_strcat(CalSw_Sm,test_file2);
								tc_strcat(CalSw_Sm,",");
								tc_strcat(CalSw_Sm,contpartrevdupSS11);
								printf("\n Expansion_TPL_16 HCL CalSw_Sm ------------>[%s] \n",CalSw_Sm);fflush(stdout);
								
								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");
								
								setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(CalSw,contpart);
								tc_strcat(CalSw,",");
								tc_strcat(CalSw,contpartrevdupSS11);
								tc_strcat(CalSw,"_");
								tc_strcat(CalSw,EOLDup);
								printf("\n Expansion_TPL_16 HCL CalSw ------------>[%s] \n",CalSw);fflush(stdout);


								tc_strcpy(CalSwDup,contpartrevdupSS11);
								tc_strcat(CalSwDup,"_");
								tc_strcat(CalSwDup,EOLDup);

								tc_strcpy(VehCal_Sm,contpart);
								tc_strcat(VehCal_Sm,",");
								tc_strcat(VehCal_Sm,contpartrevdupSS11);
								printf("\n Expansion_TPL_16 HCL VehCal_Sm ------------>[%s] \n",VehCal_Sm);fflush(stdout);
								
								setAddStr(&iWrite,&SWDetails,contpart);
								tc_strcpy(VehCal,contpart);
								tc_strcat(VehCal,",");
								tc_strcat(VehCal,contpartrevdupSS11);
								tc_strcat(VehCal,"_");
								tc_strcat(VehCal,EOLDup);
								printf("\n Expansion_TPL_16 HCL VehCal ------------>[%s] \n",VehCal);fflush(stdout);
								
								tc_strcpy(CalFileNameDup,"");
								tc_strcat(CalFileNameDup,test_file2);
								
								printf("\n CalFileNameDup HCL isssss  ------------>[%s] \n",CalFileNameDup);fflush(stdout);

								if(CalFileNameDup)
								 {
									printf("\n some value in  HCL CALFILENAME so continue %s  \n",CalFileNameDup);fflush(stdout);
									//break;
								 }
								else
								 {	
									tc_strcpy(CalFileNameDup,"");
									tc_strcat(CalFileNameDup,"NA");
									printf("\n  HCL cal file is null so setting to  %s\n",CalFileNameDup);fflush(stdout);
								 }
								
							}
							printf("\n CalFileNameDup CAL CalSw isssss  ------------>[%s]---------------------------------------%s \n",CalFileNameDup, ChdPartType);fflush(stdout);

							printf("---------------------------------------------------------------------------------------------------------------------------------------------------");fflush(stdout);
							}//23 sept
						} //NEW REMOVED
						printf("\n HwPart is AAAAAAAAA ------------>[%s] \n",HwPart);fflush(stdout);

						printf("\n 3333333333333333333333333333333333333333333333333333333333333333CalFileNameDup CAL CalSw isssss  ------------>[%s] \n",CalFileNameDup);fflush(stdout);
						if((HwPart)!=NULL && tc_strlen(HwPart)>0)
						{
							printf("\n HwPart %s\n", HwPart);fflush(stdout);						
						}
						else
						{
							HwPart			 =(char *) MEM_alloc(100);
                            tc_strcpy(HwPart,"NA,-,-,-");
							printf("\n else HwPart %s\n", HwPart);fflush(stdout);
						}

						if((HwPart_Sm)!=NULL && tc_strlen(HwPart_Sm)>0)
						{
							printf("\n HwPart_Sm %s \n",HwPart_Sm);fflush(stdout);							
						}
						else
						{
							 HwPart_Sm			 =(char *) MEM_alloc(100);
                             tc_strcpy(HwPart_Sm,"-,-,-");
							 printf("\n else HwPart_Sm %s \n",HwPart_Sm);fflush(stdout);
						}

						if((VehCal)!=NULL && tc_strlen(VehCal)>0)
						{
							printf("\n VehCal is %s\n",VehCal);fflush(stdout);							
						}
						else
						{
							VehCal			 =(char *) MEM_alloc(100);
                            tc_strcpy(VehCal,"NA,-,-,-");
							printf("\n else VehCal %s\n",VehCal);fflush(stdout);	
						}

						if((VehCal_Sm)!=NULL && tc_strlen(VehCal_Sm)>0)
						{
							printf("\n VehCal_Sm is  %s\n",VehCal_Sm);fflush(stdout);
						}
						else
						{
							VehCal_Sm			 =(char *) MEM_alloc(100);
                            tc_strcpy(VehCal_Sm,"-,-,-");
							printf("\n VehCal_Sm %s\n",VehCal_Sm);fflush(stdout);
						}

						if((CfgSw)!=NULL && tc_strlen(CfgSw)>0)
						{
							printf("\n CfgSw %s\n",CfgSw);fflush(stdout);
						}
						else
						{
							CfgSw			 =(char *) MEM_alloc(100);
                            tc_strcpy(CfgSw,"NA,-,-,-");
							printf("\n else  CfgSw %s\n",CfgSw);fflush(stdout);
						}

						if((CfgSw_Sm)!=NULL && tc_strlen(CfgSw_Sm)>0)
						{
							printf("\n CfgSw_Sm is %s\n",CfgSw_Sm);fflush(stdout);						
						}
						else
						{
							CfgSw_Sm			 =(char *) MEM_alloc(100);
                            tc_strcpy(CfgSw_Sm,"-,-,-");
							printf("\n else CfgSw_Sm %s \n",CfgSw_Sm);fflush(stdout);	
						}

						

						if((CalSw)!=NULL && tc_strlen(CalSw)>0)
						{
							printf("\n CalSw is %s\n",CalSw);fflush(stdout);
						}
						else
						{
							CalSw			 =(char *) MEM_alloc(100);
                            tc_strcpy(CalSw,"NA,-,-,-");
							printf("\n else CalSw %s\n",CalSw);fflush(stdout);
						}

						printf("\n 444444444444444444444444444444444444444444444444444444444444444444444 CalFileNameDup CAL CalSw isssss  ------------>[%s] \n",CalFileNameDup);fflush(stdout);

						if((CalSw_Sm)!=NULL && tc_strlen(CalSw_Sm)>0)
						{
							printf("\n CalSw_Sm is %s\n",CalSw_Sm);fflush(stdout);	
							
						}
						else
						{
							CalSw_Sm			 =(char *) MEM_alloc(100);
                            tc_strcpy(CalSw_Sm,"-,-,-,-");
							printf("\n else CalSw_Sm is %s\n",CalSw_Sm);fflush(stdout);

						}

						printf("\n 55555555555555555555555555555555555555555555555555555555555555555555555555555555555555 CalFileNameDup CAL CalSw isssss  ------------>[%s] \n",CalFileNameDup);fflush(stdout);
						printf("\n TEST1111 \n");fflush(stdout);
						if((AppSw)!=NULL && tc_strlen(AppSw)>0)
						{
							printf("\n AppSw\n");fflush(stdout);
						}
						else
						{
							AppSw			 =(char *) MEM_alloc(100);
                            tc_strcpy(AppSw,"NA,-,-,-");
						}

						if((AppSw_Sm)!=NULL && tc_strlen(AppSw_Sm)>0)
						{
							printf("\n AppSw_Sm\n");fflush(stdout);
						}
						else
						{
							AppSw_Sm			 =(char *) MEM_alloc(100);
                            tc_strcpy(AppSw_Sm,"-,-,-");
						}
	
						if((BasicSw)!=NULL && tc_strlen(BasicSw)>0)
						{
							printf("\n BasicSw\n");fflush(stdout);							
						}
						else
						{
							BasicSw			 =(char *) MEM_alloc(100);
                            tc_strcpy(BasicSw,"NA");
						}

						if((Se_Boot_Loader)!=NULL && tc_strlen(Se_Boot_Loader)>0)
						{
							printf("\n Se_Boot_Loader\n");fflush(stdout);
						}
						else
						{
							Se_Boot_Loader			 =(char *) MEM_alloc(100);
                            tc_strcpy(Se_Boot_Loader,"NA,-,-,-");
						}

						if((Se_Boot_Loader_Sm)!=NULL && tc_strlen(Se_Boot_Loader_Sm)>0)
						{
							printf("\n Se_Boot_Loader_Sm\n");fflush(stdout);
						}
						else
						{
							Se_Boot_Loader_Sm			 =(char *) MEM_alloc(100);
                            tc_strcpy(Se_Boot_Loader_Sm,"-,-,-");
						}

						if((Pr_Boot_Loader)!=NULL && tc_strlen(Pr_Boot_Loader)>0)
						{
							printf("\n Pr_Boot_Loader\n");fflush(stdout);
						}
						else
						{   
							Pr_Boot_Loader			 =(char *) MEM_alloc(100);
							tc_strcpy(Pr_Boot_Loader,"NA,-,-,-");
						}

						if((Pr_Boot_Loader_Sm)!=NULL && tc_strlen(Pr_Boot_Loader_Sm)>0)
						{
							printf("\n Pr_Boot_Loader_Sm\n");fflush(stdout);
						}
						else
						{
							Pr_Boot_Loader_Sm			 =(char *) MEM_alloc(100);
                            tc_strcpy(Pr_Boot_Loader_Sm,"-,-,-");
						}
	
						if((Sticker)!=NULL && tc_strlen(Sticker)>0)
						{
							 printf("\n Sticker not null \n");fflush(stdout);
						}
						else
						{
							Sticker			 =(char *) MEM_alloc(100);
                          //  tc_strcpy(Sticker,"-,-,-,");//changed
                            tc_strcpy(Sticker,"-,-,-");//changed
						}
						if(flag==0)
						{
							Sticker			 =(char *) MEM_alloc(100);
							tc_strcpy(Sticker,"-,-,-");
						}
						printf("\n TEST2222 \n");fflush(stdout);
					// }//new added

						 printf("\n CalFileNameDup is $$$$$$$$$$$$$$$$$$$$$$ %s  \n",CalFileNameDup);fflush(stdout);

/*
						  if((tc_strstr(CalFileNameDup,".crc")!=NULL)||(tc_strstr(CalFileNameDup,".rec")!=NULL)||(tc_strstr(CalFileNameDup,".s")!=NULL)||(tc_strstr(CalFileNameDup,".n07")!=NULL)
							||(tc_strstr(CalFileNameDup,".s07")!=NULL)||(tc_strstr(CalFileNameDup,".N07")!=NULL)||(tc_strstr(CalFileNameDup,".S07")!=NULL)||(tc_strstr(CalFileNameDup,".S19")!=NULL)
							||(tc_strstr(CalFileNameDup,".hex")!=NULL)||(tc_strstr(CalFileNameDup,".ecs")!=NULL)||(tc_strstr(CalFileNameDup,".par")!=NULL)||(tc_strstr(CalFileNameDup,".sol")!=NULL)
							||(tc_strstr(CalFileNameDup,".mhx")!=NULL)||(tc_strstr(CalFileNameDup,".ulp")!=NULL)||(tc_strstr(CalFileNameDup,".swl")!=NULL)||(tc_strstr(CalFileNameDup,".bin")!=NULL)
							||(tc_strstr(CalFileNameDup,".abm")!=NULL)||(tc_strstr(CalFileNameDup,".iso")!=NULL)||(tc_strstr(CalFileNameDup,".sal")!=NULL)||(tc_strstr(CalFileNameDup,".idx")!=NULL)
							||(tc_strstr(CalFileNameDup,".sqfs")!=NULL)||(tc_strstr(CalFileNameDup,".hxk")!=NULL)||(tc_strstr(CalFileNameDup,".docx")!=NULL))*/
						//if(CalFileNameDup)
//						 {
//							//printf("\n indep binary file found  %s  \n",CalFileNameDup);fflush(stdout);
//							CalFlileReq=1;
//							printf("\n CalFlileReq value is  %d  \n",CalFlileReq);fflush(stdout);
//						 }
						if((CalFileNameDup)!=NULL && tc_strlen(CalFileNameDup)>0)
						 {
							if(tc_strcmp(CalFileNameDup,"NA")!=0)
							 {
								//printf("\n indep binary file found  %s  \n",CalFileNameDup);fflush(stdout);
								CalFlileReq=1;
								printf("\n CalFlileReq value is  %d  \n",CalFlileReq);fflush(stdout);
							 }
						 }
						else
						 {	
							CalFileNameDup			 =(char *) MEM_alloc(100);
							tc_strcpy(CalFileNameDup,"NA");
							printf("\n cal file is null so setting to  %s\n",CalFileNameDup);fflush(stdout);
						 }

						 printf("\n TEST3333 \n");fflush(stdout);
					// }//new adityaaa

						   
						//if(tc_strstr(container,"16R")==0  &&  tc_strstr(container,"16L")==0 )
						//{
							
							if((Sticker))
							{
								printf("\n Expansion_TPL_16  fptr33333333333333333333 %s \n",req_item);fflush(stdout);
								//fprintf(fptr3,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s\n",req_item,Objdesc,container,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,Sticker,containerrevdup,ConProject,"NA",DML_name,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,sVehicleType,DRStatus);fflush(fptr3);//M_ASAS
								fprintf(fptr3,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s\n",VCnumberDup,ObjDescFinal,container,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,Sticker,containerrevdup,ConProject,"NA",DML_name,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,sVehicleType);fflush(fptr3);
							//  fprintf(fp3,"  \n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s,\n",vcnoDup,vcdescDup,containerDup,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,",",StickerDup,containerrevdup,ConProjectDup,"NA",DmlNo,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,"_");fflush(fp3);
							
							}
							else
							{
								printf("\n Expansion_TPL_16 else else fptr33333333333333333333 -------------- \n");fflush(stdout);
								//fprintf(fptr3,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s\n",req_item,Objdesc,container,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,",",Sticker,containerrevdup,ConProject,"NA",DML_name,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,sVehicleType,DRStatus);fflush(fptr3);
								fprintf(fptr3,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s\n",VCnumberDup,ObjDescFinal,container,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,",",Sticker,containerrevdup,ConProject,"NA",DML_name,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,sVehicleType);fflush(fptr3);

																																//54455424000R,X445 PETROL BS6 MT SEAT FABRIC JALI BLUE TECHY,544516261204,NA,-,-,-,IPC,NA,544516262003,NA,NR,2,NA,-,-,-,544516301132,NA,NR,2_BOTH,544516301130,NA,_BOTH,544216301141,NA,A,2_BOTH,544216301142,NA,F,2_BOTH,-,-,-,A_2,5445,NA,16PP251702,0,1,BOTH,NA,X445,NA,11,DAB+PAB+ABS,NA
						       	//fprintf(fp3,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s,\n",vcnoDup,vcdescDup,containerDup,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,StickerDup,containerrevdup,ConProjectDup,"NA",DmlNo,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,"_");fflush(fp3);

						    }
						//}
						//if(tc_strstr(container,"16R")==0  &&  tc_strstr(container,"16L")==0 )
						//{
							printf("\n Expansion_TPL_16 fptr6666666666666666 \n");fflush(stdout);
							//fprintf(fptr6,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",req_item,Objdesc,container,containerrev,CalSw_Sm,Cnt_EcuTypeDup,HwPart_Sm,VehCal_Sm,CfgSw_Sm,AppSw_Sm,Pr_Boot_Loader_Sm,Se_Boot_Loader_Sm,t5NoEcuDup);fflush(fptr6);
							fprintf(fptr6,"\n'%s,%s,'%s,%s,'%s,%s,'%s,'%s,'%s,'%s,'%s,'%s,%s\n",VCnumberDup,ObjDescFinal,container,containerrev,CalSw_Sm,Cnt_EcuTypeDup,HwPart_Sm,VehCal_Sm,CfgSw_Sm,AppSw_Sm,Pr_Boot_Loader_Sm,Se_Boot_Loader_Sm,t5NoEcuDup);fflush(fptr6);
						//	fprintf(fp6,  "\n'%s,%s,'%s,%s,'%s,%s,'%s,'%s,'%s,'%s,'%s,'%s,%s,\n",vcnoDup,vcdescDup,containerDup,containerrevdup,CalSw_Sm,Cnt_EcuTypeDup,HwPart_Sm,VehCal_Sm,CfgSw_Sm,AppSw_Sm,Pr_Boot_Loader_Sm,Se_Boot_Loader_Sm,t5NoEcuDup);fflush(fp6);
						CalSw_Sm="";
						VehCal_Sm="";
						Se_Boot_Loader_Sm="";
						AppSw_Sm="";
						CfgSw_Sm="";
						Pr_Boot_Loader_Sm="";
						HwPart_Sm="";
						CalFileNameDup="";
						test_file2="";
						//}
						

						flag=0;
				

						if((HwPart))
						{
							//MEM_free (HwPart);
							HwPart=NULL;
						}
						printf("\n..VehCal Expansion_TPL_16 going to file.. %s\n",VehCal);fflush(stdout);
						if((VehCal)!=NULL)
						{
							VehCal=NULL;
							printf("\n..inside mem freee VehCal \n");fflush(stdout);
							//MEM_free (VehCal); 							
						}
						printf("\n.CfgSw Expansion_TPL_16 .befor going to file..%s\n",CfgSw);fflush(stdout);
						if((CfgSw)!=NULL)
						{
							//MEM_free (CfgSw); 
							printf("\n..inside mem freee CfgSw \n");fflush(stdout);
							CfgSw=NULL;
						}
						printf("\n.CalSw Expansion_TPL_16 .befor going to file..%s\n",CalSw);fflush(stdout);
						if((CalSw)!=NULL)
						{
							//MEM_free (CalSw); 
							CalSw=NULL;
							printf("\n..inside mem freee CalSw \n");fflush(stdout);
						}
						printf("\n.AppSw Expansion_TPL_16 .befor going to file..%s\n",AppSw);fflush(stdout);
						if((AppSw)!=NULL)
						{
							AppSw=NULL;
							//MEM_free (AppSw);
							printf("\n..inside mem freee AppSw \n");fflush(stdout);
						}
						printf("\n.BasicSw Expansion_TPL_16 .befor going to file..%s\n",BasicSw);fflush(stdout);
						if((BasicSw)!=NULL)
						{
							//MEM_free (BasicSw); 
							BasicSw=NULL;
							printf("\n..inside mem freee BasicSw \n");fflush(stdout);
						}
						printf("\n.Se_Boot_Loader Expansion_TPL_16 .befor going to file..%s\n",Se_Boot_Loader);fflush(stdout);
						if((Se_Boot_Loader)!=NULL)
						{
							//MEM_free (Se_Boot_Loader);
							Se_Boot_Loader=NULL;
							printf("\n..inside mem freee Se_Boot_Loader \n");fflush(stdout);
						}
						printf("\n..Pr_Boot_Loader Expansion_TPL_16 befor going to file..%s\n",Pr_Boot_Loader);fflush(stdout);
						if((Pr_Boot_Loader)!=NULL)
						{
							//MEM_free (Pr_Boot_Loader); 	
							Pr_Boot_Loader=NULL;
						}
						printf("\n..Sticker Expansion_TPL_16 befor going to file..%s\n",Sticker);fflush(stdout);
						if((Sticker)!=NULL)
						{
							//MEM_free (Sticker);
							Sticker=NULL;
						}
						//}//NEW ADDED
					}
				}
				else
				{ 

					if ((tc_strcmp(PartType,"PRM") == 0) && (tc_strcmp(EEPartType,"G")!=0))
					{
						if( AOM_ask_value_string(ChildRev,"t5_EcuType",&PRM_EcuType)!=ITK_ok);
						printf("\n Expansion_TPL_16 inside pArt type  PRM_EcuType --------> [%s]\n",PRM_EcuType); fflush(stdout);


						//if( AOM_ask_value_string(ChildRev,"item_revision_id",&PRM_EcuRevType)!=ITK_ok);
						//printf("\n Expansion_TPL_16 PRM_EcuRevType --------> [%s]\n",PRM_EcuRevType); fflush(stdout);

						char * PRM_EcuTypeDup =NULL;
						PRM_EcuTypeDup=(char *) MEM_alloc(100);

						if((PRM_EcuType)!=NULL)
						{
							printf("\n prm value found --------> [%s]\n",PRM_EcuType); fflush(stdout);

							if(tc_strstr(PRM_EcuType,"Passive Entry Passive Start")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"PEPS");
							else if(tc_strstr(PRM_EcuType,"Engine Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ECM");
							else if (tc_strstr(PRM_EcuType,"Antilock Braking System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ABS");
							else if (tc_strstr(PRM_EcuType,"Antilock/Electronic Braking System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ABS");
							else if (tc_strstr(PRM_EcuType,"Electronic Stability Program")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ESP");
							else if (tc_strstr(PRM_EcuType,"Restraints Control Module (Airbag)")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"RCM");
							else if (tc_strstr(PRM_EcuType,"Body Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"BCM");
							else if (tc_strstr(PRM_EcuType,"Heating, Ventilation, and Air Conditioning")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"HVA");
							else if (tc_strstr(PRM_EcuType,"Fully Automated Temperature Control")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"FATC");
							else if (tc_strstr(PRM_EcuType,"Infotainment Head Unit")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"IHU");
							else if (tc_strstr(PRM_EcuType,"Vehicle Immobilizer Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"VICM");
							else if (tc_strstr(PRM_EcuType,"Park Distance Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"PACM");
							else if (tc_strstr(PRM_EcuType,"Power Steering Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"PSCM");
							else if (tc_strstr(PRM_EcuType,"Tire Pressure Monitor System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TPMS");
							else if (tc_strstr(PRM_EcuType,"Transmission Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TCM");
							else if (tc_strstr(PRM_EcuType,"Drive Mode Controller/Switch")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"DMC");
							else if (tc_strstr(PRM_EcuType,"Transmission control unit")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TCU");
							else if (tc_strstr(PRM_EcuType,"Instrument Panel Cluster")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"IPC");
							else if (tc_strstr(PRM_EcuType,"Torque On Demand Controller")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TOD");
							else if (tc_strstr(PRM_EcuType,"Window Winding Auto Down Controller")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"WWAD");
							else if (tc_strstr(PRM_EcuType,"Retarder")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"RTD");
							else if (tc_strstr(PRM_EcuType,"Tachograph")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TCG");
							else if (tc_strstr(PRM_EcuType,"Telematics")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TMC");
							else if (tc_strstr(PRM_EcuType,"Multiplexing")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"MPX");
							else if (tc_strstr(PRM_EcuType,"Hybrid ECU")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"HCU");
							else if (tc_strstr(PRM_EcuType,"Auto Transmission System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ATS");
							else if (tc_strstr(PRM_EcuType,"Electronically Controlled  Air Suspension")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ECAS");
							else if (tc_strstr(PRM_EcuType,"Emergency Braking System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"EBS");
							else if (tc_strstr(PRM_EcuType,"Exhaust Gas Recirculation")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"EGR");
							else if (tc_strstr(PRM_EcuType,"Selective Catalytic Reduction")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"SCR");
							else if (tc_strstr(PRM_EcuType,"Electronic Steering Column Lock")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ESCL");
							else if (tc_strstr(PRM_EcuType,"Unique Identification")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"UI");
							else if (tc_strstr(PRM_EcuType,"DC-DC Converter")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"DCC");
							else if (tc_strstr(PRM_EcuType,"Motor Controller Unit")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"MCU");
							else if (tc_strstr(PRM_EcuType,"Battery Management System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"BMS");
							else if (tc_strstr(PRM_EcuType,"Gear Shift Advisor")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"GSA");
							else if (tc_strstr(PRM_EcuType,"Advanced Driver Assistance System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ADAS");
							else if (tc_strstr(PRM_EcuType,"On Board Charger")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"OBC");
							else if (tc_strstr(Cnt_EcuType,"Wireless Power Charger")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"WPC");
							else if (tc_strstr(Cnt_EcuType,"Dosing Control Unit")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"DCU");
							else if (tc_strstr(Cnt_EcuType,"Battery Cooling System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"BCS");
							else 
							tc_strcpy(PRM_EcuTypeDup,"NA");
						}
						else 
						{
							printf("\n ECU value NOT found SO SETTING value as NA-------->\n"); fflush(stdout);
							tc_strcpy(PRM_EcuTypeDup,"NA");
						}

						char * ContainerRevVal =NULL;
						ContainerRevVal=(char *) MEM_alloc(100);
						//char * ContainerRevValDuptk =NULL;					
						//ContainerRevValDuptk=(char *) MEM_alloc(100);
						
						char * ContainerRevValDup =NULL;
						ContainerRevValDup=(char *) MEM_alloc(100);
						//ContainerRevValDup1=(char *) MEM_alloc(100);

						char* ContainerListVal	=	NULL;
						ContainerListVal=(char *) MEM_alloc(100);

						char	*strToBereplacedre;
						char	*strToBeUsedInsteadre;
		
						strToBereplacedre		=	(char*)malloc(5);
						tc_strcpy(strToBereplacedre,";");

						strToBeUsedInsteadre	=	(char*)malloc(5);
						tc_strcpy(strToBeUsedInsteadre,"_");//k_2


						

						printf("\n PRM_EcuTypeDup VALUE IS  --------> [%s]\n",PRM_EcuTypeDup); fflush(stdout);
						//printf("\n ContainerRevValDup VALUE IS  --------> [%s]\n",ContainerRevValDup); fflush(stdout);
					    
						//if (tc_strstr(PRM_EcuTypeDup,"NA")==0) //value should not be NA
						if (tc_strstr(PRM_EcuTypeDup,"NA")==NULL) //value should not be NA
						{															
//							index =-1;							
//							
//							int length  = tc_strlen(*ParaList);
//							printf("\n ParaList length  ------------- %d \n",length);fflush(stdout);
//							for(m=0;m<length;m++)
//							{	
//								printf("\n container value  ------------- %s\n",container);fflush(stdout);
//								if(tc_strstr(ParaList[m],container)!=NULL)
//								{
//									index = m;
//									printf("\n ParaList index value is  :-------------- %d \n",index);fflush(stdout);
//									break;
//								}
//							}
//							if (index >=0)
//							{
//
//								printf("\n INSIDE index>=0 ContainerListVal is %s:::::: \n",ContainerListVal);fflush(stdout);
//								
//								tc_strcpy(ContainerListVal,ConatainerList[index]);
//								//ContainerListVal=low_set_get_str(ConatainerList,index);
//							
//								//tc_strcpy(ContainerRevValDuptk,ConatainerRev[index]); HH
//								tc_strcpy(ContainerRevVal,ConatainerRev[index]);
//
//								//ContainerRevValDup = tc_strtok (ContainerRevValDuptk,";");
//
//								ifail =STRNG_replace_str (ContainerRevVal,strToBereplacedre, strToBeUsedInsteadre,&ContainerRevValDup);
//
//								printf("\n..111111 Parameter matches with container is :%s having revision %s and ECUTYPE %s",ContainerListVal,ContainerRevValDup,PRM_EcuTypeDup);fflush(stdout);
//							}
//							else
//							{	
								//printf("\n INSIDE else of index>=0 m ======> [%d] \n",m);fflush(stdout);
								//printf("\n ConatainerTypeListX [mm] ======> [%s] \n",ConatainerTypeListX[mm]);fflush(stdout);

								//int Conlength		  =	tc_strlen(*ConatainerTypeListX);
							    //printf("\n Conlength  ------------- %d \n",Conlength);fflush(stdout);
								index	=-1;
								//for(mm=0;mm<=iWriteRConType;mm++)
								//{
								//	printf("\n ConatainerTypeListX[mm] : %s\n",ConatainerTypeListX[mm]);fflush(stdout);
								//}

								//Steps 
								//define new set
								//char**
								//loop out container 
								int containtercount =0;
								int conn_tags_found3 =0;
								int newSlvcnt =0;
									tag_t  *contags_found3   = NULL;
									tag_t MstrSlvrelation_type2  = NULLTAG;
									tag_t  MstrChildrev   = NULLTAG;
									tag_t * newSlvattachments   = NULLTAG;
									char* MScontain=NULL;
								for(mm=0;mm<iWriteRConType;mm++) // changed from <= to < because of exceptional handler error
								{
									printf("\n inside snapshot transfer ConatainerTypeListX[mm]TEst123 : %s, PRM_EcuTypeDup : %s, ConatainerList : %s, Container id == %s\n",ConatainerTypeListX[mm],PRM_EcuTypeDup,ConatainerList[mm],container);fflush(stdout);
									if(tc_strstr(ConatainerTypeListX[mm],PRM_EcuTypeDup)!=NULL)

									{
										containtercount = containtercount + 1;
										
									}
								}

								printf("\nContainer Count == %d", containtercount);fflush(stdout);

								//setsize > 1 look for master slave relationship
								if(containtercount>1)
								{ 
									for(mm=0;mm<iWriteRConType;mm++) // changed from <= to < because of exceptional handler error
									{
										printf("\n inside snapshot transfer ConatainerTypeListX[mm] : %s, PRM_EcuTypeDup : %s, ConatainerList : %s\n",ConatainerTypeListX[mm],PRM_EcuTypeDup,ConatainerList[mm]);fflush(stdout);
										if(tc_strstr(ConatainerTypeListX[mm],PRM_EcuTypeDup)!=NULL)

										{
											index = mm;
											printf("\n ELSE index value is  :-------------- %d \n",index);fflush(stdout);
										

											const char **conattrs3 = (const char **) MEM_alloc(2 * sizeof(char *));
											const char **convalues3 = (const char **) MEM_alloc(2 * sizeof(char *));
											conattrs3[0] ="item_id";
										
											 convalues3[0] = ConatainerList[index];
								
											ifail =(ITEM_find_item_revs_by_key_attributes(1,conattrs3, convalues3,ConatainerRev[index], &conn_tags_found3, &contags_found3));
											CHECK_FAIL;
											MEM_free(conattrs3);
											MEM_free(convalues3);

											if (conn_tags_found3>0)
											{
											   MstrChildrev= contags_found3[0];
											   //printf("\n INSIDE TPL_BOM_List  Childrev \n");fflush(stdout);

											   if( AOM_ask_value_string(MstrChildrev,"item_id",&MScontain)!=ITK_ok);

												ifail = GRM_find_relation_type("T5_ContHasSlave",&MstrSlvrelation_type2); 
												{																		
													if(MstrSlvrelation_type2 != NULLTAG)
													 {
														 ITKCALL(GRM_list_secondary_objects_only(MstrChildrev,MstrSlvrelation_type2,&newSlvcnt,&newSlvattachments));

														 if (newSlvcnt>0)
														 {
															 break;
														 }
														 
													 }
												}
											}																																
										}
									}
								}
								else
								{
									for(mm=0;mm<iWriteRConType;mm++) // changed from <= to < because of exceptional handler error
									{
										printf("\n inside snapshot transfer ConatainerTypeListX[mm] : %s, PRM_EcuTypeDup : %s, ConatainerList : %s\n",ConatainerTypeListX[mm],PRM_EcuTypeDup,ConatainerList[mm]);fflush(stdout);
										if(tc_strstr(ConatainerTypeListX[mm],PRM_EcuTypeDup)!=NULL)

										{
											index = mm;
											printf("\n ELSE index value is  :-------------- %d \n",index);fflush(stdout);
											break;
										}
									}
								}
								if(index >= 0)
								{							
									tc_strcpy(ContainerListVal,ConatainerList[index]);
									//ContainerListVal=low_set_get_str(ConatainerList,index);								
									tc_strcpy(ContainerRevVal,ConatainerRev[index]);

									//ContainerRevValDup = tc_strtok (ContainerRevValDuptk,";"); HH

									ifail =STRNG_replace_str (ContainerRevVal,strToBereplacedre, strToBeUsedInsteadre,&ContainerRevValDup);


									printf("\n..Expansion_TPL_16 222222 Parameter matches with container is :%s having revision %s and ECUTYPE %s",ContainerListVal,ContainerRevValDup,PRM_EcuTypeDup);fflush(stdout);

								}
								else
								{
									printf("\n..Expansion_TPL_16 No match for parameter .");fflush(stdout);
									tc_strcpy(ContainerListVal,NoValStr);
									tc_strcpy(ContainerRevValDup,NoValStr);;
								}
//							}
						}
						else
						{
							printf("\n..Expansion_TPL_16 No type for comparison .%s,%s",PRM_EcuTypeDup,container);fflush(stdout);

							tc_strcpy(ContainerListVal,NoValStr);
							tc_strcpy(ContainerRevValDup,NoValStr);
					
						}
						if(index>=0)
						{
					      if( AOM_ask_value_string(ChildRev,"item_id",&ChildRevitemid)!=ITK_ok);
						  printf("\n ChildRevitemid     --------> [%s]\n",ChildRevitemid); fflush(stdout);

						  if( AOM_ask_value_string(ChildRev,"item_revision_id",&ChildRevitemrevid)!=ITK_ok);
						  printf("\n ChildRevitemrevid     --------> [%s]\n",ChildRevitemrevid); fflush(stdout);

						  ifail = GRM_find_relation_type("T5_EEPrm",&EErelation_type); 
			
						  if(EErelation_type != NULLTAG)
						    {
							  ifail = GRM_list_secondary_objects_only(ChildRev,EErelation_type,&cntEE,&attachmentsEE);
							  printf("\n Expansion_TPL_16  T5_EEPrm AFTER GRM_list_secondary cntEE  ---------- %d\n",cntEE); fflush(stdout);
							  if (cntEE > 0)
							    {						
								 for(ee=0;ee<cntEE;ee++)
								    {
									  printf("\n para TESTTTTTT --------- %d\n",ee);fflush(stdout);
									  ParaPtr = attachmentsEE[ee];
								
									  //if(ITEM_ask_item_of_rev  ( ParaPtr,&ParaMaster) );
									  

									  ifail = AOM_ask_value_string(ParaPtr,"t5_ECUType",&paraecutype);
									  printf("\n para master ecutype   is ---------- [%s]\n",paraecutype); fflush(stdout);								

									  ifail = AOM_ask_value_string(ParaPtr,"t5_t5EPDesc",&paradesc); 
									  printf("\n parameter desc   is --------- [%s]\n",paradesc); fflush(stdout);

									 								
										ifail = GRM_find_relation(ChildRev,ParaPtr,EErelation_type,&relationprop);

									    ifail = AOM_ask_value_string(relationprop,"t5_BitValue",&DefValue);
										printf("\n DefValue is --------- [%s]\n",DefValue); fflush(stdout);
										
										char * DefValuedup   =NULL;
										char * printValueS   =NULL;
										printValueS=(char *) MEM_alloc(1000);
																				
										//ifail =(STRNG_replace_str (DefValue,"\n","\0",&DefValuedup1));
									//	tc_strdup(DefValue,&DefValuedup);
										ifail =(STRNG_replace_str (DefValue, "'", " ",&DefValuedup));
										printf("\n DefValuedup qqqqqqqqqqqqqqqqqqqqqqqqq is --------- [%s]\n",DefValuedup); fflush(stdout);
										strip(DefValuedup);																					//uncomment for handling \n

										printf("\n DefValuedup is --------- [%s]\n",DefValuedup); fflush(stdout);										
										
										


									  
							
									  //if((paradesc))
									 if(tc_strcmp(paradesc,"")!=0)
										{
											paradesclength = tc_strlen(paradesc);
											for(paradesccnt=0;paradesccnt<paradesclength;paradesccnt++)
												{
													if(paradesc[paradesccnt]=='\n')
														paradesc[paradesccnt] =' ';

//													if(paradesc[paradesccnt]=='`')
//														paradesc[paradesccnt] =' ';
//
//													if(paradesc[paradesccnt]=='~')
//														paradesc[paradesccnt] =' ';
//
//													if(paradesc[paradesccnt]=='|')
//														paradesc[paradesccnt] =' ';

													if(paradesc[paradesccnt]=='&')
														paradesc[paradesccnt] ='-';

													if(paradesc[paradesccnt]==',')
														paradesc[paradesccnt] =' ';

													printf("\n [%c]\n",paradesc[paradesccnt]);fflush(stdout);
												}
											cRplStr = (char *)MEM_alloc (paradesclength * sizeof(char));
											cRplStrdup=(char *) MEM_alloc(250);
											tc_strcpy(cRplStr,paradesc);
											printf("\n parameter desc newwwwwwww   is --------- [%s]\n",cRplStr); fflush(stdout);

											
											if(cRplStr)
											{
												tc_strcpy(cRplStrdup,cRplStr);
												tc_strcat(cRplStrdup,";");
												tc_strcat(cRplStrdup,DefValuedup);
											}
										
										}	
										else
										{
											  cRplStrdup=(char *) MEM_alloc(50);
                                              tc_strcpy(cRplStrdup,NoParaDes);											  																						
											  //tc_strcpy(cRplStrdup,"NA");
											  tc_strcat(cRplStrdup,";");
											  tc_strcat(cRplStrdup,DefValuedup);
											  printf("\n NoParaDes   is --------- [%s]\n",cRplStrdup); fflush(stdout);	
										}


										AOM_ask_value_string(ParaPtr,"t5_EPApplicable",&ParaApp); //changed from ifail to itkcall
										printf("\t Expansion_TPL_16 parameter applicable val   is --------- [%s]\n",ParaApp); fflush(stdout);
										printf("\n testing 0000 here  --------- \n");fflush(stdout);
										 //if(ParaApp)
										 
											ParaAppdup_Val=NULL;
											ParaAppdup_Val=(char *) MEM_alloc(100);
										if((ParaApp)!=NULL && tc_strlen(ParaApp)>0)
										 {
											
											if(tc_strcmp(ParaApp,"BothES")==0) 
											{											
												tc_strcpy(ParaAppdup_Val,ValBoth);												
											}
											else if(tc_strcmp(ParaApp,"Both")==0) 
											{											
												tc_strcpy(ParaAppdup_Val,ValBoth);												
											}
										    else if(tc_strcmp(ParaApp,"EOL")==0) 
											{
												tc_strcpy(ParaAppdup_Val,"EOL");
											}
											//else if (tc_strstr(ParaApp,"NotApp")!=NULL)
											else if(tc_strcmp(ParaApp,"NotApp")==0) 
											{	
												tc_strcpy(ParaAppdup_Val,"NA");
											}
											//else if (tc_strstr(ParaApp,"Service")!=NULL)
											else if(tc_strcmp(ParaApp,"Service")==0) 
											{	
												tc_strcpy(ParaAppdup_Val,ValSer);
											}
											else if(tc_strcmp(ParaApp,"FOTA")==0) 
											{
												tc_strcpy(ParaAppdup_Val,"FOTA");
											}
											else if(tc_strcmp(ParaApp,"t5all")==0) 
											{
												tc_strcpy(ParaAppdup_Val,"ALL");
											}
										}
										else
										{
											printf("\n testing BEFORE 7777 here  --------- \n");fflush(stdout);
											//ParaAppdup_Val=(char *) MEM_alloc(100);
											tc_strcpy(ParaAppdup_Val,NoValStr);
										}

										printf("\n testing 9999 here  ---------%s \n",ParaAppdup_Val);fflush(stdout);

//											char*	EOLNA			=	"NA";
//									char*	ValBoth			=	"BOTH";
//									char*	WriteVal		=	"WRITE";
//									char*	ReadVal			=	"READ";
//									char*	ServVal			=	"SERVICE";
//									char*	NoValStr		=	"NA";
//									char*	EOL_Val_Both	=	"BOTH";
//									char*	EOL_Val_N		=	"N";

										//ITK_CALL(AOM_ask_value_string(ParaPtr,"t5_EPReadable",&ParaRead));
										AOM_ask_value_string(ParaPtr,"t5_EPReadable",&ParaRead);
										printf("\t Expansion_TPL_16 parameter READ val   is --------- [%s]\n",ParaRead); fflush(stdout);

										ParaReaddup_Val=NULL;
										ParaReaddup_Val=(char *) MEM_alloc(100);
										 
										// if(ParaRead)
										if((ParaRead)!=NULL && tc_strlen(ParaRead)>0)
										 {												
										    
											//if(tc_strstr(ParaApp,"BothRW")!=NULL) 
											//if(tc_strstr(ParaRead,"Both")!=NULL) 
											if(tc_strcmp(ParaRead,"Both")==0) 
											{					
												tc_strcpy(ParaReaddup_Val,ValBoth);
											}
										    else if(tc_strcmp(ParaRead,"BothRW")==0) 
											{					
												tc_strcpy(ParaReaddup_Val,ValBoth);
											}									
											//else if (tc_strstr(ParaRead,"Write")!=NULL)
											else if(tc_strcmp(ParaRead,"Write1")==0) 
											{					
												tc_strcpy(ParaReaddup_Val,WriteVal);
											}
											//else if (tc_strstr(ParaRead,"Read Only")!=NULL)
											else if(tc_strcmp(ParaRead,"Read Only")==0) 
											{					
												tc_strcpy(ParaReaddup_Val,ReadVal);
											}
											else
											{
												tc_strcpy(ParaReaddup_Val,EOLNA);
											}
											printf("\n ParaReaddup_Val    is --------- [%s]\n",ParaReaddup_Val); fflush(stdout);
										}
										else
										{
											//ParaReaddup_Val=(char *) MEM_alloc(100);
											tc_strcpy(ParaReaddup_Val,NoValStr);
											printf("\n ParaReaddup_Val  elseeeee  is --------- [%s]\n",ParaReaddup_Val); fflush(stdout);
										}

										ITK_CALL( AOM_ask_value_string(ParaPtr,"t5_EPUnit",&paraunit));
										printf("\t Expansion_TPL_16 paraunit val   is --------- [%s]\n",paraunit); fflush(stdout);

										ITK_CALL( AOM_ask_value_string(ParaPtr,"t5_EPType",&paraeptype));
										printf("\t Expansion_TPL_16 paraeptype   is --------- [%s]\n",paraeptype); fflush(stdout);


										if((paraeptype))
										{
										    printf("\t paraeptype not null \n",paraeptype); fflush(stdout);
											
											paraeptypelength = tc_strlen(paraeptype);
											for(paraccnt=0;paraccnt<paraeptypelength;paraccnt++)
											{
												if(paraeptype[paraccnt]=='\n')
													paraeptype[paraccnt] =' ';

												if(paraeptype[paraccnt]==',')
													paraeptype[paraccnt] =' ';

												if(paraeptype[paraccnt]=='&')
													paraeptype[paraccnt] ='-';

												printf("\n [%c]\n",paraeptype[paraccnt]);fflush(stdout);
											}
											
											//NewDesc = (char *)MEM_alloc (desclength * sizeof(char));
											//tc_strcpy(NewDesc,Desc);
											tc_strdup(paraeptype,&paraeptypedup);
											
											//tc_strdup(paraeptype,&paraeptypedup);
											printf("\n paraeptypedupppppppppppppppppp is --------------- [%s]\n",paraeptypedup); fflush(stdout);
											strip(paraeptypedup);																					
											printf("\n paraeptypedup is --------- [%s]\n",paraeptypedup); fflush(stdout);								
										}
										else
										{	 paraeptypedup=NULL;
										     paraeptypedup=(char *) MEM_alloc(100);
											 tc_strcpy(paraeptypedup,NoValStr);
											 printf("\n in else paraeptypedup issssssss--------- [%s]\n",paraeptypedup); fflush(stdout);
										}


										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPLen",&paralen));
										printf("\t paralen is --------- [%s]\n",paralen); fflush(stdout);

										if((paralen))
										{
										    printf("\t paralen not null \n",paralen); fflush(stdout);
										}
										else
										{    
											 paralen=NULL;
											 paralen=(char *) MEM_alloc(25);
											 tc_strcpy(paralen,Noval);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPMin",&paramin));//value will be null
										printf("\n Expansion_TPL_16 paramin is --------- [%s]\n",paramin); fflush(stdout);

										if((paramin) && tc_strlen(paramin)>0)
										{
										    printf("\n paramin not null [%s] \n",paramin); fflush(stdout);
										}
										else
										{
											 paramin=NULL;
											 paramin=(char *) MEM_alloc(25);
											 tc_strcpy(paramin,Noval);
											 printf("\n paramin ------- [%s] \n",paramin); fflush(stdout);
										}

										ifail = AOM_ask_value_string(ParaPtr,"t5_EPMax",&paramax);//value will be null
										printf("\n Expansion_TPL_16 paramax is --------- [%s]\n",paramax); fflush(stdout);

										if((paramax)&& tc_strlen(paramax)>0)
										{
										    printf("\n paramax not null [%s]\n",paramax); fflush(stdout);
										}
										else
										{
											paramax=NULL;
											 paramax=(char *) MEM_alloc(25);
											 tc_strcpy(paramax,Noval);
											 printf("\n paramax ------- [%s] \n",paramax); fflush(stdout);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPStep",&parastep));//?????check if attribute name is on master or rev
										printf("\n Expansion_TPL_16 parastep is --------- [%s]\n",parastep); fflush(stdout);

										
										
										if((parastep)&& tc_strlen(parastep)>0)
										{
											parastepdup=NULL;
											parastepdup=(char *) MEM_alloc(50);
											tc_strcpy(parastepdup,parastep);
											printf("\n Expansion_TPL_16 parastepdup is --------- [%s]\n",parastepdup); fflush(stdout);
										}
										else
										{
											parastepdup=NULL;
											parastepdup=(char *) MEM_alloc(50);
											tc_strcpy(parastepdup,Noval);
											printf("\n else Expansion_TPL_16 parastepdup is --------- [%s]\n",parastepdup); fflush(stdout);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPDidValue",&ParaDIDVal));
										printf("\n Expansion_TPL_16 ParaDIDVal is --------- [%s]\n",ParaDIDVal); fflush(stdout);
										
										if((ParaDIDVal))
										{
										    printf("\n ParaDIDVal not null \n",ParaDIDVal); fflush(stdout);
										}
										else
										{
											 ParaDIDVal=NULL;
											 ParaDIDVal=(char *) MEM_alloc(50);
											 tc_strcpy(ParaDIDVal,NoValStr);
											 printf("\n ParaDIDVal value is [%s] \n",ParaDIDVal); fflush(stdout);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"object_name",&paradoc));
										printf("\n Expansion_TPL_16 paradoc is --------- [%s]\n",paradoc); fflush(stdout);

										
										

//										if(paradoc)
//										{
//											tc_strcpy(paradocdup,paradoc);
//											tc_strcat(paradocdup,";");
//											tc_strcat(paradocdup,ParaDIDVal);
//										}
//										else
//										{
//											tc_strcpy(paradocdup,"NA");
//											tc_strcat(paradocdup,";");
//											tc_strcat(paradocdup,ParaDIDVal);
//										}

										if(paradoc)
										{
											paradocdup=NULL;
											paradocdup=(char *) MEM_alloc(75);
											tc_strcpy(paradocdup,paradoc);
									
										}
										else
										{
											paradocdup=NULL;
											paradocdup=(char *) MEM_alloc(75);
											tc_strcpy(paradocdup,"NA");
										}

										printf("\n Expansion_TPL_16 paradocdup  paradocdup is --------- [%s]\n",paradocdup); fflush(stdout);

										int paradocccnt=0;
										int paradocduplength=0;

										if(tc_strcmp(paradocdup,"")!=0)
										{
											paradocduplength = tc_strlen(paradocdup);
											for(paradocccnt=0;paradocccnt<paradocduplength;paradocccnt++)
											{
												if(paradocdup[paradocccnt]=='\n')
													paradocdup[paradocccnt] =' ';

												if(paradocdup[paradocccnt]==',')
													paradocdup[paradocccnt] =' ';

												if(paradocdup[paradocccnt]=='&')
													paradocdup[paradocccnt] ='-';

												printf("\n [%c]\n",paradocdup[paradocccnt]);fflush(stdout);
											}
										}
										tc_strdup(paradocdup,&paradocdupS);
																				
										printf("\n Expansion_TPL_16 paradocdupS is --------- [%s]\n",paradocdupS); fflush(stdout);

										//ifail =( AOM_ask_value_string(ParaMaster,"t5_EPValid",&paravalid));//?????check if attribute name is on master or rev
										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPValid",&paravalid));//?????check if attribute name is on master or rev
										printf("\n Expansion_TPL_16  paravalid is --------- [%s]\n",paravalid); fflush(stdout);

										if((paravalid))
										{
										 if (tc_strcmp(paravalid,"T1") == 0)
											{
												ValFlag=1;
											}
											else
											{
												ValFlag=0;
											}
										}
										else
										{
											ValFlag=0;
										}

										//ifail =( AOM_ask_value_string(ParaMaster,"t5_ParamValue",&DefValue));//?????check if attribute name is on master or rev
										//printf("\n only DefValue is --------- [%s]\n",DefValue); fflush(stdout);
//								
//										ifail = GRM_find_relation(ChildRev,ParaPtr,EErelation_type,&relationprop);
//
//									    ifail = AOM_ask_value_string(relationprop,"t5_BitValue",&DefValue);
//										printf("\n DefValue is --------- [%s]\n",DefValue); fflush(stdout);//[
//										
//										char * DefValuedup   =NULL;
//										char * printValueS   =NULL;
//										printValueS=(char *) MEM_alloc(1000);
//																				
//										//ifail =(STRNG_replace_str (DefValue,"\n","\0",&DefValuedup1));
//										tc_strdup(DefValue,&DefValuedup);
//										printf("\n DefValuedup qqqqqqqqqqqqqqqqqqqqqqqqq is --------- [%s]\n",DefValuedup); fflush(stdout);
//										strip(DefValuedup);																					//uncomment for handling \n
//
//										printf("\n DefValuedup is --------- [%s]\n",DefValuedup); fflush(stdout);

/*
										char * DefValuedup   =NULL;
										DefValuedup=(char *) MEM_alloc(100);

										char * DefValueduptemp   =NULL;
											
									    char	*replx;
										char	*replx2;
										char	*replx3;
										char	*ToBeUsedx;

									     replx3=(char *) MEM_alloc(50);
									     replx=(char *) MEM_alloc(10);
									     replx2=(char *) MEM_alloc(10);
										//replx		=	(char*)malloc(5);
										//tc_strcpy(replx,",");
										//replx2	=	(char*)malloc(5);
										//tc_strcpy(replx2,"'");
										replx3	=	(char*)malloc(5);
										tc_strcpy(replx3,"\n");

										ToBeUsedx	=	(char*)malloc(5);
										tc_strcpy(ToBeUsedx,"");

										if((DefValue))
										{
											char *temp	=	NULL;
											temp	=	(char *) MEM_alloc(100);

								    	 ifail =(STRNG_replace_str (DefValue,replx3,ToBeUsedx,&DefValueduptemp));
										 //ifail =(STRNG_replace_str (DefValue,replx, ToBeUsedx,&DefValuedup));
										 // ifail =(STRNG_replace_str (DefValue,replx2,ToBeUsedx,&DefValuedup));
										 // ifail =(STRNG_replace_str (DefValue,replx3,ToBeUsedx,&DefValuedup));
										 //ifail =(STRNG_replace_str (DefValuedup,replx3,ToBeUsedx,&temp));

								//		  printf("\n DefValuedup is --------- [%s]\n",DefValueduptemp); fflush(stdout);

										  //tc_strcpy(DefValuedup,DefValueduptemp);
										  tc_strdup(DefValuedup,&DefValueduptemp);
//										  if (DefValuedup[tc_strlen(DefValuedup-1)]=='\n')
//										  {
//											  DefValuedup[tc_strlen&DefValuedup-1)]='\0';
//										  }
//										  printf("\n DefValuedup222 is temp--------- {%s}",DefValuedup); fflush(stdout);
										 
										//  trimLeading(DefValuedup);

										   printf("\n DefValuedup 11111 is --------- [%s]\n",DefValuedup); fflush(stdout);

										//  temp = tc_strtok (DefValuedup,"\n");//NEW REMOVED
										//  DefValuedup	=	NULL;
										//  DefValuedup	=	(char	*)MEM_alloc(tc_strlen(temp)+15);
										//  tc_strcpy(DefValuedup,temp);
										//  tc_strcpy(ToBeUsedx,"\0");


										}
										else
										{
										  tc_strcpy(DefValuedup,Noval);
										}*/
											///////////////////////////////////////////////////////////////////////////////////////
										CreDate=NULL;
										//ifail =( AOM_ask_value_string(ParaPtr,"creation_date",&CreDate));
										ifail =( AOM_UIF_ask_value(ParaPtr,"creation_date",&CreDate));
										printf("\n CreDate is --------- [%s]\n",CreDate); fflush(stdout);
										
										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPValueList",&paravallist));
										printf("\n paravallist is --------- [%s]\n",paravallist); fflush(stdout);

										char * paravallistdup   =NULL;
										paravallistdup=(char *) MEM_alloc(50);

										if((paravallist)!=NULL  && tc_strlen(paravallist)>0)
										{
										   tc_strcpy(paravallistdup,paravallist);
										   printf("\n 1111 paravallistdup is --------- [%s]\n",paravallistdup); fflush(stdout);
										}
									    else
										{
										   tc_strcpy(paravallistdup,NoValStr);
										   printf("\n 2222 paravallistdup is --------- [%s]\n",paravallistdup); fflush(stdout);
										}

										printf("\n ParaReaddup_Val QQQQQQQQQQQQQQQQQQQQQQQQ    is --------- [%s]\n",ParaReaddup_Val); fflush(stdout);
										printf("\n Expansion_TPL_16 fptr22222222222222222 \n");fflush(stdout);

										//printf(fptr2,"\n %s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",container,paradocdupS,cRplStr,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevValDup,DML_name,req_item,ParaAppdup_Val,ParaReaddup_Val,cntEE);fflush(fptr2);
										printf("%s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%s,%s\n",container,paradocdupS,cRplStrdup,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevValDup,DML_name,VCnumberDup,ParaAppdup_Val,ParaReaddup_Val,cntEE,ParaDIDVal,containerrevdup);fflush(fptr2);
										fprintf(fptr2,"\n %s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%s,%s\n",container,paradocdupS,cRplStrdup,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevValDup,DML_name,VCnumberDup,ParaAppdup_Val,ParaReaddup_Val,cntEE,ParaDIDVal,containerrevdup);fflush(fptr2);
										//sprintf(printValueS,"%s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%s,%s",container,paradocdupS,cRplStr,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevValDup,DML_name,VCnumberDup,ParaAppdup_Val,ParaReaddup_Val,cntEE,ParaDIDVal,containerrevdup);
										//fprintf(fptr2,"%s\n",printValueS);fflush(fptr2);																													
								    }
							    }
						    }
						}
					}
			    }

			//}	
		 }
		}
	}
	} //18SEPT

	if(fptr2) fclose(fptr2);
}
						   
						

static void TPL_BOM_List(tag_t top_line)
 {

	tag_t window,windowW, Rrule, item_tag = null_tag,top_lineW;
	tag_t *children;
	tag_t *childrenY;
	tag_t *childrenW;
	tag_t *tags_found3  = NULL;
	tag_t *tags_found5  = NULL;
	tag_t Childrev      = NULLTAG;
	tag_t ChildW        = NULLTAG;
	

	int countX			= 0;
	int countW			= 0;
	int i				= 0;
	int kk				= 0;
	int iw				= 0;
	int n_tags_found3   = 0;
	int n_tags_found5   = 0;
	int statusX;
	int statusW;
	char* container		= NULL;
	char* PartType		= NULL;
	char* containerrev	= NULL;
	char* contain		= NULL;
	char* Cnt_EcuType	= NULL;
	char* ChildPart		= NULL;
	char* ChildPartType = NULL;
	char* ChildPartrev  = NULL;

	FILE *fptr;

	int ifail;
	int status;

	int depth  =0;
	int iWriteX  =0;

	HWCList = (char**)MEM_alloc( 1 * sizeof *HWCList );
	CALList = (char**)MEM_alloc( 1 * sizeof *CALList );
	PBLList = (char**)MEM_alloc( 1 * sizeof *PBLList );
	SBLList = (char**)MEM_alloc( 1 * sizeof *SBLList );
	BASList = (char**)MEM_alloc( 1 * sizeof *BASList );
	APPList = (char**)MEM_alloc( 1 * sizeof *APPList );
	STKList = (char**)MEM_alloc( 1 * sizeof *STKList );
	VCIList = (char**)MEM_alloc( 1 * sizeof *VCIList );
	CFGList = (char**)MEM_alloc( 1 * sizeof *CFGList );
	PRMList = (char**)MEM_alloc( 1 * sizeof *PRMList );
	CONList = (char**)MEM_alloc( 1 * sizeof *CONList );

	printf("\n inside TPL BOM list \n");fflush(stdout);

//	ifail = BOM_create_window (&window);
//	CHECK_FAIL;
//	printf("\n after bom create window  \n");fflush(stdout);
//
//	//ifail = BOM_set_window_config_rule( window, rule );
//	//CHECK_FAIL;
//	//printf("\n after BOM_set_window_config_rule \n");fflush(stdout);
//
//	ifail = BOM_set_window_top_line (window, null_tag, Design_rev_tag, null_tag, &top_line); 
//	CHECK_FAIL;
//	printf("\n after BOM_set_window_top_line \n");fflush(stdout);
//
//	ifail = BOM_window_show_suppressed ( window );
//	CHECK_FAIL;
//
//	ifail =(BOM_set_window_pack_all(window, FALSE));
//	CHECK_FAIL;

	countX=0;
	statusX = BOM_line_ask_child_lines (top_line, &countX, &children);

	printf ("\n INSIDE TPL_BOM_List countX is --->%d \n",countX); fflush(stdout);

	if(countX >0)
		{
		   //printf("\n..16R has a structure.. \n");fflush(stdout);
		   for (i=0;i<countX;i++)
			{
				if( AOM_ask_value_string(children[i],"bl_item_item_id",&container)!=ITK_ok);
				//printf("\n  TPL_BOM_List container is --->[%s] \n",container);fflush(stdout);

				if( AOM_ask_value_string(children[i],"bl_rev_item_revision_id",&containerrev)!=ITK_ok);
				printf("\t TPL_BOM_List container is --->[%s] & containerrev is ----> [%s]\n",container,containerrev); fflush(stdout);

				//if( AOM_ask_value_string(children[i],"t5_SwPartType",&PartType)!=ITK_ok);//???????
				//printf("\t PartType [%s]\n",PartType); fflush(stdout);

				const char **attrs3 = (const char **) MEM_alloc(2 * sizeof(char *));
				const char **values3 = (const char **) MEM_alloc(2 * sizeof(char *));
				attrs3[0] ="item_id";
				//attrs3[1] ="object_type";
				values3[0] = (char *)container;
				//values3[1] = "Design";
				//ifail = ITEM_find_items_by_key_attributes(2,attrs, values, &n_tags_found, &tags_found);
				ifail =(ITEM_find_item_revs_by_key_attributes(1,attrs3, values3,containerrev, &n_tags_found3, &tags_found3));
				CHECK_FAIL;
				MEM_free(attrs3);
				MEM_free(values3);

				if (n_tags_found3>0)
				{
				   Childrev= tags_found3[0];
				   //printf("\n INSIDE TPL_BOM_List  Childrev \n");fflush(stdout);
				}

				if( AOM_ask_value_string(Childrev,"item_id",&contain)!=ITK_ok);
				//printf("\n  TPL_BOM_List contain  --->[%s] \n",contain);fflush(stdout);

				if( AOM_ask_value_string(Childrev,"t5_SwPartType",&PartType)!=ITK_ok);
				printf("\n PartType  TPL_BOM_List ... %s \n",PartType);fflush(stdout);

				iWriteX  =0;
				//if(container)
				//{
				   if (tc_strcmp(PartType, "CON") == 0)
					{
					   printf("\n TPL_BOM_List Container found with part type CON... %s \n",container);fflush(stdout);

						if( AOM_ask_value_string(Childrev,"t5_EcuType",&Cnt_EcuType)!=ITK_ok);
						
						if((Cnt_EcuType))
						{
							printf("\t TPL_BOM_List Cnt_EcuType [%s]\n",Cnt_EcuType); fflush(stdout);
						}
						else
						{
							Cnt_EcuType=(char *) MEM_alloc(10);
							tc_strcpy(Cnt_EcuType,"NA"); 
							printf("\t TPL_BOM_List Cnt_EcuType  %s \n",Cnt_EcuType); fflush(stdout);
						}

						//tc_strcpy(CONList,container); 

						setAddStr(&iWriteX,&CONList,container);

						//ifail = BOM_create_window (&windowW);
						//CHECK_FAIL;
						////ifail = CFM_find( "Latest Released", &rule );
						//ifail = CFM_find( "ERC release and above", &rule );

						//printf ("\n after rev rule \n"); fflush(stdout);

						//ifail = BOM_set_window_config_rule( windowW, rule );
						//CHECK_FAIL;

						//ifail = BOM_set_window_top_line (windowW, null_tag, Childrev, null_tag, &top_lineW); 
						//CHECK_FAIL;

						//ifail = BOM_window_show_suppressed ( windowW );
						//CHECK_FAIL;

						//ifail =(BOM_set_window_pack_all(windowW, FALSE));
						///CHECK_FAIL;

						countW=0;
						statusW = BOM_line_ask_child_lines (children[i], &countW, &childrenW);

						printf ("\n   TPL_BOM_List countW is --->%d \n",countW); fflush(stdout);

						if(countW >0)
						{
						   printf("\n.TPL_BOM_List .INSIDE . childrenW. \n");fflush(stdout);

						   for (iw=0;iw<countW;iw++)
							 {
								if( AOM_ask_value_string(childrenW[iw],"bl_item_item_id",&ChildPart)!=ITK_ok);
								//printf("\n TPL_BOM_List ChildPart is  XXXXX--->[%s] \n",ChildPart);fflush(stdout);

								if( AOM_ask_value_string(childrenW[iw],"bl_rev_item_revision_id",&ChildPartrev)!=ITK_ok);
								printf("\t TPL_BOM_List ChildPart is ---->[%s] & ChildPartrev is -----> [%s]\n",ChildPart,ChildPartrev); fflush(stdout);

							    const char **attrs5  = (const char **) MEM_alloc(2 * sizeof(char *));
								const char **values5 = (const char **) MEM_alloc(2 * sizeof(char *));
								attrs5[0]	   ="item_id";
								attrs5[1]      ="object_type";
								values5[0]     = (char *)ChildPart;
								values5[1]     = "Design";
								//ifail = ITEM_find_items_by_key_attributes(2,attrs, values, &n_tags_found, &tags_found);
								ifail =(ITEM_find_item_revs_by_key_attributes(2,attrs5, values5,ChildPartrev, &n_tags_found5, &tags_found5));
								CHECK_FAIL;
								MEM_free(attrs5);
								MEM_free(values5);

								if (n_tags_found5>0)
								{
								   ChildW= tags_found5[0];
								}

								if( AOM_ask_value_string(ChildW,"t5_SwPartType",&ChildPartType)!=ITK_ok);
								printf("\t TPL_BOM_List ChildPartType [%s]\n",ChildPartType); fflush(stdout);

								if (tc_strcmp(ChildPartType, "HWC") == 0)
								{
									setAddStr(&iWriteX,&HWCList,ChildPart);
								}
								else if (tc_strcmp(ChildPartType, "CAL") == 0 || tc_strcmp(ChildPartType, "HCL") == 0)
								{
									setAddStr(&iWriteX,&CALList,ChildPart);
								}
								else if (tc_strcmp(ChildPartType, "PBL") == 0)
								{
									setAddStr(&iWriteX,&PBLList,ChildPart);
								}
								else if (tc_strcmp(ChildPartType, "SBL") == 0)
								{
									setAddStr(&iWriteX,&SBLList,ChildPart);
								}
								else if (tc_strcmp(ChildPartType, "BAS") == 0)
								{
									setAddStr(&iWriteX,&BASList,ChildPart);
								}
								else if (tc_strcmp(ChildPartType, "APP") == 0)
								{
									setAddStr(&iWriteX,&APPList,ChildPart);
								}
								else if (tc_strcmp(ChildPartType, "STK") == 0)
								{
									setAddStr(&iWriteX,&STKList,ChildPart);
								}
								else if (tc_strcmp(ChildPartType, "VCI") == 0 || tc_strcmp(ChildPartType, "HCL") == 0)
								{
									setAddStr(&iWriteX,&VCIList,ChildPart);
								}
								else if (tc_strcmp(ChildPartType, "CFG") == 0)
								{
									setAddStr(&iWriteX,&CFGList,ChildPart);
								}
							}
						}						
					}
					else if (tc_strcmp(PartType, "STK") == 0)
					{
						printf("\n TPL_BOM_List STK found number is ---->: %s",container);
						//low_set_add_str(STKList,container); 
						setAddStr(&iWriteX,&STKList,container);
					}
					else if (tc_strcmp(PartType, "PRM") == 0)
					{
						printf("\n TPL_BOM_List parameter found number is ---->: %s",container);
						//low_set_add_str(PRMList,container); 
						setAddStr(&iWriteX,&PRMList,container);
						printf("\n after PRMList\n");
					}
				//}
			}
		}			
    }

//========================= END OF TPL_BOM_List =============================================== 
int Container_List(tag_t top_line,char * PartNo,char * Assy_Rev,char * Objdesc)
 {

	char * CurrentDate			= NULL;
	char * DateDupS				= NULL;
	char * containerl			= NULL;
	char * containerrevl		= NULL;
	char * PartTypeS			= NULL;
	char * parapart				= NULL;
	char * Cnt_EcuType			= NULL;
	char * Cnt_EE_PartType			= NULL;
	char ** SWDetails			= NULL;
	char ** HWDetails			= NULL;
	char * Cnt_EcuTypeDup		= NULL;
	char * contain				= NULL;
	char * paraID				= NULL;
	char * pararev				= NULL;
	char * tmpcon				= "TEMP";
	int	   nn					= 0;
	int	   mi					= 0;
	int	   pq					= 0;
	int	   cnt					= 0;
	int	   cntt					= 0;
	int	   paracnt				= 0;
	int	   cntEE				= 0;
	int	   ifail;
	int	   statusS;
	int	   parastatusS;
	int	   FATC_flag			= 0;
    int	   FATC_S_flag			= 0;
	int    iReadR				= 0;
	int    ind					= 0;
	int    SlvcntS				= 0;
	int    pc					= 0;

	tag_t  asmtop_line;
	tag_t  asmwindow;
	tag_t  item_tag				 = null_tag;
	tag_t  ChildRevS			 = NULLTAG;
	tag_t  PararevS				 = NULLTAG;
	tag_t  ConHasParaPtr		 = NULLTAG;
	tag_t  ConPararelation_type  = NULLTAG;
	tag_t  EEPrmrelation_type    = NULLTAG;
	tag_t MstrSlvrelation_typeS  = NULLTAG;

	tag_t *Paraattachments		 = NULLTAG;
	tag_t *SlvattachmentsS		 = NULLTAG;

	//boolean  bIsConParFound	= FALSE;
	tag_t	*tags_found9		= NULL;
	tag_t	*tags_found10		= NULL;
	int		n_tags_found9		= 0;
	int		n_tags_found10		= 0;

	tag_t *childprtS;
	tag_t *parachildprt;

    CurrentDate	= (char *) MEM_alloc(30);
    SWDetails   = (char**)MEM_alloc( 1 * sizeof *SWDetails );
    HWDetails   = (char**)MEM_alloc( 1 * sizeof *HWDetails );

	time_t temp ; 
    struct tm *timeptr ; 
    char pAccessDate[30]; 

	CurrentDate		=(char *) MEM_alloc(30);
    time( &temp ); 
      
    tc_strcpy(pAccessDate,"");
    timeptr = localtime( &temp ); 

    strftime(pAccessDate, sizeof(pAccessDate), "%x - %I:%M%p", timeptr); 
      
    //printf("Formatted date & time : %s\n", pAccessDate ); 
	tc_strcpy(CurrentDate,pAccessDate);
	printf("\n CurrentDate Container_List  :%s\n",CurrentDate);

	char * CurrentDateCDup =NULL;
	CurrentDateCDup=(char *) MEM_alloc(100);

	char	*strToBereplaceddateC;
	char	*strToBeUsedInsteaddateC;

	strToBereplaceddateC		=	(char*)malloc(5);
	tc_strcpy(strToBereplaceddateC,"-");

	strToBeUsedInsteaddateC	=	(char*)malloc(5);
	tc_strcpy(strToBeUsedInsteaddateC,"");

	ifail =STRNG_replace_str (CurrentDate,strToBereplaceddateC, strToBeUsedInsteaddateC,&CurrentDateCDup);


	//getCurrentDateTimeA(CurrentDate);
	//printf("\n inside Container_List Today's date is  ---------- %s\n",CurrentDate);

	//DateDupS = subString(CurrentDate,0,16);
	//printf("\n inside Container_List Today's date is  ---------- %s\n",DateDupS);

	TPL_BOM_List(top_line); //rule to be removed

	printf("\n AFTER TPL_BOM_List IN Container_List ---------- \n");

	cnt=0;
	statusS = BOM_line_ask_child_lines (top_line, &cnt, &childprtS);

	printf ("\n Container_List cnt is --->%d \n",cnt); fflush(stdout);

	if(cnt >0)
	 {
		for (nn=0;nn<cnt;nn++)
		 {
			containerl=NULL;
			containerrevl=NULL;
			if( AOM_ask_value_string(childprtS[nn],"bl_item_item_id",&containerl)!=ITK_ok);
			//printf("\n ccontainerl is --->[%s] \n",containerl);fflush(stdout);

			if( AOM_ask_value_string(childprtS[nn],"bl_rev_item_revision_id",&containerrevl)!=ITK_ok);
			printf("\n ccontainerl is --->[%s] & containerrevl is --->[%s]\n ",containerl,containerrevl);fflush(stdout);

			if(!containerrevl) continue;

			const char **attrs9  = (const char **) MEM_alloc(2 * sizeof(char *));
			const char **values9 = (const char **) MEM_alloc(2 * sizeof(char *));
			attrs9[0]	   ="item_id";
			values9[0]     = (char *)containerl;
			ifail = ITEM_find_item_revs_by_key_attributes(1,attrs9, values9,containerrevl, &n_tags_found9, &tags_found9);
			MEM_free(attrs9);
			MEM_free(values9);

			if (n_tags_found9>0)
			{
			   ChildRevS= tags_found9[0];
			}

			if( AOM_ask_value_string(ChildRevS,"item_id",&contain)!=ITK_ok);

			if( AOM_ask_value_string(ChildRevS,"t5_SwPartType",&PartTypeS)!=ITK_ok); 
			printf("\t  item_id is [%s]     &    t5_SwPartType is [%s]\n",contain,PartTypeS); fflush(stdout);

			if( AOM_ask_value_string(ChildRevS,"t5_EcuType",&Cnt_EcuType)!=ITK_ok);
			printf("\n cont list Cnt_EcuType [%s]\n",Cnt_EcuType); fflush(stdout);

			if( AOM_ask_value_string(ChildRevS,"t5_EE_PartType",&Cnt_EE_PartType)!=ITK_ok);
			printf("\n cont list Cnt_EE_PartType [%s]\n",Cnt_EE_PartType); fflush(stdout);


			//if(tc_strstr(containerl,"16R")==0  &&  tc_strstr(containerl,"16L")==0 )
			//{
			//	printf("\n..containerl 16R present in 16R structure... %s",containerl);fflush(stdout);
			//	Container_List (childprtS[nn],rule,PartNo,Assy_Rev,Objdesc);
			//}

		if((tc_strstr(PartTypeS,"CON") != NULL) || (tc_strcmp(Cnt_EE_PartType,"G") == 0)) // PART TYPE IS CON commented adiiiii
		  {
			  paracnt=0;
			  parastatusS = BOM_line_ask_child_lines (childprtS[nn], &paracnt, &parachildprt);
			 
			  printf ("\n para _List cnt is --->%d \n",paracnt); fflush(stdout);

			  if(paracnt >0)
				{
				  for (pc=0;pc<paracnt;pc++)
					{
					   if( AOM_ask_value_string(parachildprt[pc],"bl_item_item_id",&paraID)!=ITK_ok);
					   printf("\n  paraID  is  --------------------- %s\n",paraID);fflush(stdout);

					   if( AOM_ask_value_string(parachildprt[pc],"bl_rev_item_revision_id",&pararev)!=ITK_ok);
					   printf("\n  pararev  is  --------------------- %s\n",pararev);fflush(stdout);

					   // ifail = GRM_find_relation_type("T5_ContHasPara",&ConPararelation_type);
					   ifail = GRM_find_relation_type("T5_ContHasPara",&ConPararelation_type);  

					   if(ConPararelation_type != NULLTAG)
						{
						    printf("\n  parameter found FOR %s......",paraID);	fflush(stdout);
						  
						  	const char **attrs10  = (const char **) MEM_alloc(2 * sizeof(char *));
							const char **values10 = (const char **) MEM_alloc(2 * sizeof(char *));
							attrs10[0]	   ="item_id";
							values10[0]     = (char *)paraID;
							//ifail = ITEM_find_items_by_key_attributes(1,attrs10, values10, &n_tags_found10, &tags_found10);
							//if(ITEM_find_items_by_key_attributes(1,&cTemp, &item_name, &n_tags_found, &tags_found) !=ITK_ok);PrintErrorStack();
							ifail = ITEM_find_item_revs_by_key_attributes(1,attrs10, values10,pararev, &n_tags_found10, &tags_found10);
							MEM_free(attrs10);
							MEM_free(values10);

							printf("\n n_tags_found10 is  --------------------- %d\n",n_tags_found10);fflush(stdout);

							if (n_tags_found10 >0)
							{
							   PararevS= tags_found10[0];
							}
					 
							 (GRM_list_secondary_objects_only(PararevS,ConPararelation_type,&cntt,&Paraattachments));
							

							 printf("\n  cntt value is  --------------------- %d\n",cntt);fflush(stdout);

							 if (cntt > 0)
							  {				 	
								for(pq=0;pq<cntt;pq++)
								 {
									ConHasParaPtr = Paraattachments[pq];
									printf("\n ConHasParaPtr after attachments\n ");	fflush(stdout);	
						
								   //iWriteR=0;
									if( AOM_ask_value_string(ConHasParaPtr,"item_id",&parapart)!=ITK_ok);
									printf("\t parapart   is -----------[%s]\n",parapart); fflush(stdout);	
									
	//								ind=-1;
	//								printf("\n XXXXXXXX %d : %s",mi,PRMList[mi]);fflush(stdout);
	//								int length		  =	tc_strlen(PRMList[mi]+5);				
	//
	//								for(mi=0;mi<length;mi++)
	//								 {	
	//									//char *str		  = NULL;
	//									//str=(char *) MEM_alloc(100);						
	//									tc_strcpy(PRMList[mi],parapart)!=NULL;
	//									//printf("\n 2726 inside length  ------------- [%s]   [%s] \n",parapart,str);fflush(stdout);
	//									///if(tc_strcmp(str,parapart)==0)
	//									{
	//										ind = mi;
	//										printf("\n index value is  :-------------- %d \n",ind);fflush(stdout);
	//										break;
	//									}
	//								 }
	//
	//								if(ind>=0)
	//								 {	
										printf("\nAdded in TEMP");fflush(stdout);
										setAddStr(&iWriteRCon,&ConatainerList,containerl);
										setAddStr(&iWriteRConType,&ConatainerTypeListX,"TEMP");
										setAddStr(&iWriteRConrev,&ConatainerRev,containerrevl);
										setAddStr(&iWriteRPar,&ParaList,parapart);
										//bIsConParFound = TRUE;
	//								 }
								 }
							 }
						 }
					 }				
				 }
		
				  if(Cnt_EcuType)
				  {	
					printf("\n Cnt_EcuType not null \n"); fflush(stdout);
					Cnt_EcuTypeDup	=	NULL;
					Cnt_EcuTypeDup=(char *) MEM_alloc(100);

					if(tc_strstr(Cnt_EcuType,"Passive Entry Passive Start")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"PEPS");
					  }
					else if(tc_strstr(Cnt_EcuType,"Engine Control Module")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"ECM");
					  }
					else if (tc_strstr(Cnt_EcuType,"Antilock Braking System")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"ABS");
					  }
					else if (tc_strstr(Cnt_EcuType,"Antilock/Electronic Braking System")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"ABS");
					  }
					else if (tc_strstr(Cnt_EcuType,"Electronic Stability Program")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"ESP");
					  }
					else if (tc_strstr(Cnt_EcuType,"Restraints")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"RCM");
					  }
					else if (tc_strstr(Cnt_EcuType,"Body Control Module")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"BCM");
					  }
					else if (tc_strstr(Cnt_EcuType,"Heating, Ventilation, and Air Conditioning")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"HVA");
					  }
					else if (tc_strstr(Cnt_EcuType,"Infotainment Head Unit")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"IHU");
					  }
					else if (tc_strstr(Cnt_EcuType,"Vehicle Immobilizer Control Module")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"VICM");
					  }
					else if (tc_strstr(Cnt_EcuType,"Park Distance Control Module")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"PACM");
					  }
					else if (tc_strstr(Cnt_EcuType,"Power Steering Control Module")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"PSCM");
					  }
					else if (tc_strstr(Cnt_EcuType,"Tire Pressure Monitor System")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"TPMS");
					  }
					else if (tc_strstr(Cnt_EcuType,"Transmission Control Module")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"TCM");
					  }
					else if (tc_strstr(Cnt_EcuType,"Drive Mode Controller/Switch")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"DMC");
					  }
					else if (tc_strstr(Cnt_EcuType,"Transmission control unit")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"TCU");
					  }
					else if (tc_strstr(Cnt_EcuType,"Instrument Panel Cluster")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"IPC");
					  }
					else if (tc_strstr(Cnt_EcuType,"Torque On Demand Controller")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"TOD");
					  }
					else if (tc_strstr(Cnt_EcuType,"Window Winding Auto Down Controller")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"WWAD");
					  }
					else if (tc_strstr(Cnt_EcuType,"Retarder")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"RTD");
					  }
					else if (tc_strstr(Cnt_EcuType,"Tachograph")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"TCG");
					  }
					else if (tc_strstr(Cnt_EcuType,"Telematics")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"TMC");
					  }
					else if (tc_strstr(Cnt_EcuType,"Multiplexing")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"MPX");
					  }
					else if (tc_strstr(Cnt_EcuType,"Hybrid ECU")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"HCU");
					  }
					else if (tc_strstr(Cnt_EcuType,"Auto Transmission System")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"ATS");
					  }
					else if (tc_strstr(Cnt_EcuType,"Electronically Controlled  Air Suspension")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"ECAS");
					  }
					else if (tc_strstr(Cnt_EcuType,"Emergency Braking System")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"EBS");
					  }
					else if (tc_strstr(Cnt_EcuType,"Exhaust Gas Recirculation")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"EGR");
					  }
					else if (tc_strstr(Cnt_EcuType,"Selective Catalytic Reduction")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"SCR");
					  }
					else if (tc_strstr(Cnt_EcuType,"Electronic Steering Column Lock")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"ESCL");
					  }
					else if (tc_strstr(Cnt_EcuType,"Unique Identification")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"UI");
					  }
					else if (tc_strstr(Cnt_EcuType,"DC-DC Converter")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"DCC");
					  }
					else if (tc_strstr(Cnt_EcuType,"Motor Controller Unit")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"MCU");
					  }
					else if (tc_strstr(Cnt_EcuType,"Battery Management System")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"BMS");
					  }
					else if (tc_strstr(Cnt_EcuType,"Gear Shift Advisor")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"GSA");
					  }
					else if (tc_strstr(Cnt_EcuType,"Advanced Driver Assistance System")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"ADAS");
					  }
					else if (tc_strstr(Cnt_EcuType,"On Board Charger")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"OBC");
					  }
				    else if (tc_strstr(Cnt_EcuType,"Wireless Power Charger")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"WPC");
					  }
					else if (tc_strstr(Cnt_EcuType,"Dosing Control Unit")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"DCU");
					  }
					else if (tc_strstr(Cnt_EcuType,"Battery Cooling System")!=NULL) 
					  {
						tc_strcpy(Cnt_EcuTypeDup,"BCS");
					  }
				    else if (tc_strstr(Cnt_EcuType,"Fully Automated Temperature Control")!=NULL)
					  {
						tc_strcpy(Cnt_EcuTypeDup,"FATC");
						//add code here.......

						FATC_flag = 0;  
						ifail = GRM_find_relation_type("T5_ContHasSlave",&MstrSlvrelation_typeS); 
						{
							if(MstrSlvrelation_typeS != NULLTAG)
							{
								printf("\n Expansion_TPL_16 MstrSlvrelation_typeSSSSSSSSSSSSSSSS relation found......\n");	fflush(stdout);	
								
								ITKCALL(GRM_list_secondary_objects_only(ChildRevS,MstrSlvrelation_typeS,&SlvcntS,&SlvattachmentsS));
								CHECK_FAIL;

								if (SlvcntS > 0)
								{	
									printf("\nAdded in SlvcntS");fflush(stdout);
									setAddStr(&iWriteRCon,&ConatainerList,containerl);
									setAddStr(&iWriteRConType,&ConatainerTypeListX,Cnt_EcuTypeDup);
									setAddStr(&iWriteRConrev,&ConatainerRev,containerrevl);
									setAddStr(&iWriteRPar,&ParaList,tmpcon);
									FATC_flag = 1;
									continue;

								}
	//							else if(setSize(soMastrECUs)>0)
	//						    {
	//						  	  FATC_flag = 1;
	//						    }
								else
								{
									FATC_S_flag = 1;
								}
							}
						}
					 }
					else 
					 {
						tc_strcpy(Cnt_EcuTypeDup,"NA");
					 }
					printf("\n ZZZZZZXXXX Cnt_EcuTypeDup value is  :-------------- %s \n",Cnt_EcuTypeDup);fflush(stdout);
					setAddStr(&iWriteRCon,&ConatainerList,containerl);
					setAddStr(&iWriteRConType,&ConatainerTypeListX,Cnt_EcuTypeDup);
					setAddStr(&iWriteRConrev,&ConatainerRev,containerrevl);
					setAddStr(&iWriteRPar,&ParaList,tmpcon); 

				   }
				  else
				   {
					  tc_strcpy(Cnt_EcuTypeDup,"NA");
					  printf("\n test Cnt_EcuTypeDup value is  :-------------- %s \n",Cnt_EcuTypeDup);fflush(stdout);
				   }
				 // if(tc_strcmp(Cnt_EcuTypeDup,"FATC")!= 0)  /*|| (nlsStrCmp(Cnt_EcuTypeDup,"FATC")== 0 && FATC_flag==0 && FATC_S_flag == 1))*/
				  //if((tc_strcmp(Cnt_EcuTypeDup,"FATC")!= 0)  || (tc_strcmp(Cnt_EcuTypeDup,"FATC")== 0 && FATC_S_flag == 1))
//					  {
//						printf("\n XXXX Cnt_EcuTypeDup value is  :-------------- %s \n",Cnt_EcuTypeDup);fflush(stdout);
//						setAddStr(&iWriteRCon,&ConatainerList,containerl);
//						setAddStr(&iWriteRConType,&ConatainerTypeListX,Cnt_EcuTypeDup);
//						setAddStr(&iWriteRConrev,&ConatainerRev,containerrevl);
//						setAddStr(&iWriteRPar,&ParaList,tmpcon); 
						

						//int	mm=0;
						//for(mm=0;mm<=tc_strlen(*ConatainerTypeListX);mm++)
						printf("\n iWriteRConType :%d\n",iWriteRConType);fflush(stdout);
						
//					  }												
								
			 }

		   }
		}		
	 } 

//(=================AFTER CONTAINER LIST ===================================================)

int Expansion_GrpId(tag_t Childrev,tag_t ChildRevBomLine,char * PartNo,char * Objdesc) //not used
//int  Expansion_GrpId(tag_t top_line,tag_t rule,char * PartNo,char * Assy_Rev,char * Objdesc,char * PartTypeM)
{

	int countC = 0;
	int ww	   = 0;

	//char CurrentDate[20]={0};
	char * DateDup 	= NULL;
	tag_t *childrenC;
	tag_t *childrenZZ;

	tag_t   top_lineC	=NULLTAG;
	tag_t   top_line2	=NULLTAG;
	tag_t   window		=NULLTAG;
	tag_t   window2		=NULLTAG;

	int	   ifail;
	int	   statusC;

	int		n_tags_found4 = 0;
	int		n_tags_found6 = 0;
	int		jj			  = 0;
	int		cnt			  = 0;
	int		cntX		  = 0;
	int		countZ		  = 0;
	int		CNT			  = 0;
	int		calfiletest   = 0;
	int		CalFlileReq   = 0;
	int		kp			  = 0;
	int		bb		      = 0;
	int     flag		  = 0;
	int     flagY		  = 0;
	int     iWrite		  = 0;
	int     iRead		  = 0;
	int     iRead2		  = 0;
	int     m			  = 0;
	int     mm			  = 0;
	int     yy			  = 0;
	int     ee			  = 0;
	int    	statusZZ	  = 0;
	int     index         = 0;
	int     cntEE         = 0;

	int ValFlag			  = 0;
	int 	mach_type = SS_UNIX_MACHINE;

	int referencenumberfound =0;

	tag_t  *tags_found4   = NULL;
	tag_t  *tags_found6   = NULL;
	tag_t   ChildRev      = NULLTAG;
	tag_t   ChildRevG     = NULLTAG;
	tag_t   ContChildRev  = NULLTAG;

	tag_t   SpkitTag	  = NULLTAG;
	tag_t   Sparekit      = NULLTAG;
	tag_t sp_kit_rel_type = NULLTAG;
	tag_t Parameter_type  = NULLTAG;
	tag_t	namedrefobject     = NULLTAG;
	tag_t	ParaPtr       = NULLTAG;
	

	char* container		  = NULL;
	char* containerrev	  = NULL;
	char* ConProject	  = NULL;
	char* EEPartType	  = NULL;
	char* PartType		  = NULL;
	char* TPLEOL		  = NULL;
	char* ContEOL		  = NULL;
	char* Cnt_EcuType	  = NULL;
	char* Sp_ItemID		  = NULL;
	char* Sp_ItemREV	  = NULL;
	char* contpartrev	  = NULL;
	char* contpart		  = NULL;
	char* ProjectC		  = NULL;
	char* ChdPartType	  = NULL;
	char* Desg			  = NULL;
	char* ownername		  = NULL;
	char* orig_name		  = NULL;
	char *pathname		  = NULL;
	char *relative_path	  = NULL;
	char * test_file      = NULL;
	char * test_file2      = NULL;
	char * test_fileX      = NULL;
	char *cRplStr		 =	NULL;

	char * byte1 =NULL;



	char*	EOLDup		  =	NULL;
	char*	EOL_Val		  =	NULL;

	char*	EOL			  =	NULL;
	char*	EOLNA		  =	"NA";
	char*	ValBoth		  =	"BOTH";
	char*	WriteVal	  =	"WRITE";
	char*	ReadVal		  =	"READ";
	char*	ServVal		  =	"SERVICE";
	char*	NoValStr	  =	"NA";
	char*	EOL_Val_Both  =	"BOTH";
	char*	EOL_Val_N	  =	"N";
	char*   objtype       = NULL;
	char*   AtttachFileNames  = NULL;
	char*   Sticker       = NULL;
	char*   Pr_Boot_Loader_Sm = NULL;
	char*   Pr_Boot_Loader = NULL;
	char*   BasicSw		   = NULL;
	char*   Se_Boot_Loader = NULL;
	char*   Se_Boot_Loader_Sm = NULL;
	char*   AppSw_Sm	   = NULL;
	char*   AppSw		   = NULL;
	char*   CalSw		   = NULL;
	char*   CalSw_Sm	   = NULL;
	char*   CalSw_SmDup	   = NULL;
	char*   CfgSw_Sm	   = NULL;
	char*   CfgSw		   = NULL;
	char*   VehCal		   = NULL;
	char*   VehCal_Sm	   = NULL;
	char*   HwPart_Sm	   = NULL;
	char*   HwPart		   = NULL;
	char*   CalSwDup	   = NULL;
	char*   CalFileName	   = NULL;
	char*   NoParaDes	   = "";

	char * PRM_EcuType  =   NULL;
	char * paraecutype  =   NULL;
	char * paradesc		=   NULL;
	char * sVehicleType	=   NULL;
	char * ParaApp		=   NULL;
	char * ParaRead		=   NULL;
	char * DefValue		=   NULL;
	char * ValSer		=   "SERVICE";


	char * paraunit		=   NULL;
	char * paraeptype	=   NULL;
	char * paraeptypedup=   NULL;
	char * paralen		=   NULL;
	char * paramin		=   NULL;
	char * paramax		=   NULL;
	char * parastep		=   NULL;
	char * ParaDIDVal	=   NULL;
	char * paradoc      =   NULL;
	char * 	 Noval	    =  "0";
	char * paradocdupS  =   NULL;
	char * paravalid    =   NULL;
	char * paravallist  =   NULL;
	char * CreDate      =   NULL;
	char * ParaAppdup_Val  =NULL;
	char * ParaReaddup_Val =NULL;
	char * CurrentDate		=NULL;
	char * conitemid		=NULL;
	char * conitemrevid		=NULL;
	char * DRStatus			=NULL;
	char * CalFileNameDup   =NULL;
	char * BitValue   =NULL;

	
	char	*contpartrevdup		= NULL;

	tag_t *attachments			= NULLTAG;
	tag_t *attachmentsX			= NULLTAG;
	tag_t *attachmentsEE		= NULLTAG;
	tag_t dataset				= NULLTAG;
	tag_t ParaMaster			= NULLTAG;
	tag_t relation_type         = NULLTAG;
	tag_t EErelation_type       = NULLTAG;
	tag_t ConPararelation_type  = NULLTAG;

		int length	=0;	  
	//int byte1int	=0;	  
	long byte1int	=0;	  

	char *reference_nameDS=NULL;	
	reference_nameDS=(char *) MEM_alloc(15);

	char *Exfile=NULL;	
	Exfile=(char *) MEM_alloc(300);

		char	*strToBerepl1R=NULL;
			char	*strToBeUsed1R=NULL;
			char	*containerrevdup =NULL;


	char	 *NewDateG				 = NULL;
	AE_reference_type_t reftype;
	char refname[AE_reference_size_c + 1];

	char	**SWDetails	=	NULL;
	char	**HWDetails	=	NULL;
//
//	getCurrentDateTimeA(CurrentDate);
//	printf("\n  Expansion_TPL_16 Today's date is  ---------- %s\n",CurrentDate);
//
//	DateDup = subString(CurrentDate,0,11);
//	printf("\n  Expansion_TPL_16 Today's date is  ----------- %s\n",DateDup);

	time_t temp ; 
    struct tm *timeptr ; 
    char pAccessDate[30]; 

	CurrentDate		=(char *) MEM_alloc(30);
    time( &temp ); 
      
    tc_strcpy(pAccessDate,"");
    timeptr = localtime( &temp ); 

    strftime(pAccessDate, sizeof(pAccessDate), "%x - %I:%M%p", timeptr); 
      
    printf("Formatted date & time : %s\n", pAccessDate ); 
	tc_strcpy(CurrentDate,pAccessDate);
	printf("\n CurrentDate AAAAAAA :%s\n",CurrentDate);

	char * CurrentDateGDup =NULL;
	CurrentDateGDup=(char *) MEM_alloc(100);

	char	*strToBereplaceddateG;
	char	*strToBeUsedInsteaddateG;

	char * CurrentDateGDupNew =NULL;
	CurrentDateGDupNew=(char *) MEM_alloc(100);

	strToBereplaceddateG		=	(char*)malloc(5);
	tc_strcpy(strToBereplaceddateG,"/");

	strToBeUsedInsteaddateG	=	(char*)malloc(5);
	tc_strcpy(strToBeUsedInsteaddateG,"-");

	ifail =STRNG_replace_str (CurrentDate,strToBereplaceddateG, strToBeUsedInsteaddateG,&CurrentDateGDup);

	tc_strcpy(CurrentDateGDupNew,CurrentDateGDup); // 5445T3A1816001
																														
    NewDateG=subString(CurrentDateGDupNew,0,10); //5445T3A18
    printf("\n NewDateG is ------------  = [%s]\n", NewDateG);fflush(stdout);


	//ifail = BOM_create_window (&window);
	//CHECK_FAIL;

	//printf ("\n in Expansion_TPL_16 after rev rule \n"); fflush(stdout);

	//ifail = BOM_set_window_config_rule( window, rule );
	//CHECK_FAIL;

	//ifail = BOM_set_window_top_line (window, null_tag, Childrev, null_tag, &top_lineC);
	//CHECK_FAIL;

	//ifail = BOM_window_show_suppressed ( window );
	//CHECK_FAIL;

	//ifail= BOM_set_window_pack_all(window, FALSE);
	//CHECK_FAIL;

//		for (ww=0;ww<StructChldCnt+1;ww++) ///test
//		 {
//			printf("\n..IN Expansion_TPL_16 structure.. :%d\n",ww);fflush(stdout);
//			container=NULL;
//			containerrev=NULL;
//			tag_t ChildRevBomLine=NULLTAG;
//			//if( AOM_ask_value_string(childrenZ[ww],"bl_item_item_id",&container)!=ITK_ok);
//			//if( AOM_ask_value_string(childrenZ[ww],"item_id",&container)!=ITK_ok);
//			if( AOM_ask_value_string(ChldStrutBdySh[ww].child_objs,"item_id",&container)!=ITK_ok);
//
//			//if( AOM_ask_value_string(childrenZ[ww],"bl_rev_item_revision_id",&containerrev)!=ITK_ok);
//			//if( AOM_ask_value_string(childrenZ[ww],"item_revision_id",&containerrev)!=ITK_ok);
//			if( AOM_ask_value_string(ChldStrutBdySh[ww].child_objs,"item_revision_id",&containerrev)!=ITK_ok);
//
//			ChildRevBomLine=ChldStrutBdySh[ww].child_objs_bvr;
//		 }

	countC=0;
	statusC = BOM_line_ask_child_lines (ChildRevBomLine, &countC, &childrenC);

	printf ("\n IN Expansion_GrpId countC is --->%d \n",countC); fflush(stdout);

	if((countC)>0)
	{
	    printf("\n..IN Expansion_GrpId structure.. \n");fflush(stdout);

	  for (ww=0;ww<countC;ww++)
		{

		 if( AOM_ask_value_string(childrenC[ww],"bl_item_item_id",&container)!=ITK_ok);
		 //printf("\n container Expansion_TPL_16 structure is --->[%s] \n",container);fflush(stdout);

		 if( AOM_ask_value_string(childrenC[ww],"bl_rev_item_revision_id",&containerrev)!=ITK_ok);
		 //printf("\n container Expansion_TPL_16 structure  containerrev is --->[%s] \n",containerrev);fflush(stdout);

		 //if( AOM_ask_value_string(childrenC[ww],"bl_Design Revision_t5_ProjectCode",&ConProject)!=ITK_ok);
		 printf("\n  Expansion_GrpId structure  container is ---->[%s]    containerrev is ---> [%s] \n",container,containerrev);fflush(stdout);

			
         if((container)&&(containerrev)) //check below AAAAAAAAAA
			{
			 printf("\n  in Expansion_GrpId structure  container && containerrev \n");fflush(stdout);

			const char **attrs3  = (const char **) MEM_alloc(2 * sizeof(char *));
			const char **values3 = (const char **) MEM_alloc(2 * sizeof(char *));
			attrs3[0]	   ="item_id";
			//attrs3[1]      ="object_type";
			values3[0]     = (char *)container;
			//values3[1]     = "Design";
			//ifail = ITEM_find_items_by_key_attributes(2,attrs, values, &n_tags_found, &tags_found);
			ifail = ITEM_find_item_revs_by_key_attributes(1,attrs3, values3,containerrev, &n_tags_found4, &tags_found4);
			MEM_free(attrs3);
			MEM_free(values3);

			if (n_tags_found4>0)
			{
			   ChildRevG= tags_found4[0];
			   printf("\n  Expansion_GrpId structure  ChildRevG found \n");fflush(stdout);
			}

			//if( AOM_ask_value_string(ChildRevG,"bl_Design Revfision_t5_ProjectCode",&ConProject)!=ITK_ok);
			//printf("\n container Expansion_TPL_16 structure  ConProject is --->[%s] \n",ConProject);fflush(stdout);

			if( AOM_ask_value_string(ChildRevG,"t5_ProjectCode",&ConProject)!=ITK_ok);

			if( AOM_ask_value_string(ChildRevG,"t5_SwPartType",&PartType)!=ITK_ok);
			//printf("\t PartType ontainer Expansion_TPL_16 structure is [%s]\n",PartType); fflush(stdout);

			if( AOM_ask_value_string(ChildRevG,"t5_EE_PartType",&EEPartType)!=ITK_ok);
			//printf("\t EEPartType ontainer Expansion_TPL_16 structure   is [%s]\n",EEPartType); fflush(stdout);

			if( AOM_ask_value_string(ChildRevG,"t5_AppForEOL",&TPLEOL)!=ITK_ok);
			//printf("\t TPLEOL ontainer Expansion_TPL_16 structure   is [%s]\n",TPLEOL); fflush(stdout);

			if( AOM_ask_value_string(ChildRevG,"t5_AppForEOL",&ContEOL)!=ITK_ok);
			if( AOM_ask_value_string(ChildRevG,"t5_PartStatus",&DRStatus)!=ITK_ok);
			printf("\t GRPID structure - ConProject is ---> [%s]   PartType  is--->[%s]   EEPartType is ----> [%s]  TPLEOL is ----> [%s] ContEOL is ----->[%s],DRStatus is ----->[%s]\n",PartType,EEPartType,TPLEOL,ContEOL,DRStatus); fflush(stdout);

		

			strToBerepl1R		=	(char*)malloc(5);
			tc_strcpy(strToBerepl1R,";");

			strToBeUsed1R	=	(char*)malloc(5);
			tc_strcpy(strToBeUsed1R,"_");

			ifail =(STRNG_replace_str (containerrev,strToBerepl1R, strToBeUsed1R,&containerrevdup));
			printf("\n Expansion_GrpId containerrevdup containerrevdup    ConProject is ---> [%s]\n",containerrevdup);fflush(stdout);


			char * ContEOLDup =NULL;
			ContEOLDup=(char *) MEM_alloc(100);
			char * ContEOLDup_Val =NULL;
			ContEOLDup_Val=(char *) MEM_alloc(100);

			printf("\n  ADI1111--->\n");fflush(stdout);


			if((ContEOL))
			 {			   
				tc_strcpy(ContEOLDup,ContEOL); 
			 }
			else
			 {
				tc_strcpy(ContEOLDup,EOLNA);
			 }
		
			if(ContEOLDup)
			 {

				if(tc_strstr(ContEOLDup,"BothES")!=NULL) 
				{					
					tc_strcpy(ContEOLDup_Val,ValBoth);
				}
				else if (tc_strstr(ContEOLDup,"NotApp")!=NULL)
				{					
					tc_strcpy(ContEOLDup_Val,ValBoth);
				}
				else if (tc_strstr(ContEOLDup,"Service")!=NULL)
				{					
					tc_strcpy(ContEOLDup_Val,ServVal);
				}
				else
				{
					tc_strcpy(ContEOLDup_Val,ContEOLDup);
				}
			 }

			 printf("\n  ADI2222--->\n");fflush(stdout);
			 
			char * TPLEOLDUP =NULL;
			TPLEOLDUP=(char *) MEM_alloc(100);
			char * TPLEOLVAL =NULL;
			TPLEOLVAL=(char *) MEM_alloc(100);

			printf("\n  ADI3333--->\n");fflush(stdout);

			if((TPLEOL))
			{
				tc_strcpy(TPLEOLDUP,TPLEOL);
				printf("\n  ADI444-->\n");fflush(stdout);
			//}
				if (tc_strstr(TPLEOLDUP,"BothES")!=0)
				{
					tc_strcpy(TPLEOLVAL,EOL_Val_Both);
					printf("\n  ADI555-->\n");fflush(stdout);
				}
				else if (tc_strstr(TPLEOLDUP,"NotApp")!=0)
				{
					tc_strcpy(TPLEOLVAL,EOL_Val_N);
					printf("\n  ADI666-->\n");fflush(stdout);
				}
				else
				{
					tc_strcpy(TPLEOLVAL,TPLEOLDUP);
					printf("\n  ADI777->\n");fflush(stdout);
				}

			}

				if(tc_strcmp(PartType,"CON") == 0)
				{
					supfile=0;
					//calfiletest=0;
					CalFlileReq=0;
					printf("\t INSIDE CONTAINER AND GRP ID CONDITION  \n"); fflush(stdout);

					//sVehicleType=(char *) MEM_alloc(100);
					//tc_strcpy(sVehicleType,"NA");

					//ADD MASTER SLAVE CODES HERE //newly added check this
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					tag_t MstrSlvrelation_type  = NULLTAG;
					tag_t SlaveNo				= NULLTAG;
					tag_t MstrNo				= NULLTAG;
					int Mstrcnt=0;
					int mt=0;
					int Slvcnt=0;
					int wx=0;
					tag_t *Mstrattachments		= NULLTAG;
					tag_t *Slvattachments		= NULLTAG;
					char * mastercontainer      = NULL;
					char * slavecontainer       = NULL;
				

					ifail = GRM_find_relation_type("T5_ContHasSlave",&MstrSlvrelation_type); 
					{
					  if(MstrSlvrelation_type != NULLTAG)
					  {
						  int Flag_Master=0;
						printf("\n group iD MstrSlvrelation_type relation found......");	fflush(stdout);	
						sVehicleType=(char *) MEM_alloc(30);								
					    
						GRM_list_primary_objects_only(ChildRevG,MstrSlvrelation_type,&Mstrcnt,&Mstrattachments);
						printf("\n  NO OF Mstrcnt group iD----------- %d",Mstrcnt);fflush(stdout);

						 if(Mstrcnt>0)
						  {							 
							 for(mt=0;mt<Mstrcnt;mt++)
							  {
								printf("\n AFTER MstrSlvrelation_type =========> \n"); fflush(stdout);
								MstrNo = Mstrattachments[mt];
									
								if( AOM_ask_value_string(MstrNo,"item_id",&mastercontainer)!=ITK_ok);
								printf("\n IN group iD mastercontainer ---------  [%s]\n",mastercontainer); fflush(stdout);
							
								//for (ms=0;ms<countZ;ms++)
								//{
								//	if( AOM_ask_value_string(childrenZ[ms],"bl_item_item_id",&containermst)!=ITK_ok);
								//	printf("\n IN Expansion_TPL_16 containermst ---------  [%s]\n",containermst); fflush(stdout);

								//	if(tc_strcmp(mastercontainer,containermst)==0)
								//	{
								//		printf("\n match found mastercontainer \n"); fflush(stdout);
										//sVehicleType=NULL;	
										//sVehicleType=(char *) MEM_alloc(50);
										tc_strcpy(sVehicleType,"S_"); 
										tc_strcat(sVehicleType,mastercontainer);
										printf("\n group iDs  VehicleType issssssssss. mastercontainer...[%s]..\n",sVehicleType);	fflush(stdout);

										Flag_Master=1;
								//		break;
								//	}
								// }
							  }
						   }
						   else
						  {
							   tc_strcpy(sVehicleType,"NA");
						  }

						  if (Flag_Master==0)
						  {
						  
							
						 ITKCALL(GRM_list_secondary_objects_only(ChildRevG,MstrSlvrelation_type,&Slvcnt,&Slvattachments));
						 CHECK_FAIL; //slave has container

							if (Slvcnt > 0)
							{	
							  for (wx=0;wx<Slvcnt;wx++)
								{									
									printf("\n AFTER MstrSlvrelation_type group iD =========> \n"); fflush(stdout);
									SlaveNo = Slvattachments[wx];
									
									if( AOM_ask_value_string(SlaveNo,"item_id",&slavecontainer)!=ITK_ok);
									printf("\n IN group iD slavecontainer ---------  [%s]\n",slavecontainer); fflush(stdout);

									//for (wy=0;wy<countZ;wy++)
									//{
									//	if( AOM_ask_value_string(childrenZ[wy],"bl_item_item_id",&containerslv)!=ITK_ok);
									//	printf("\n IN Expansion_TPL_16 containerslv ---------  [%s]\n",containerslv); fflush(stdout);

									//	if(tc_strcmp(slavecontainer,containerslv)==0)
									 //   {
									//		printf("\n match found \n"); fflush(stdout);
											//sVehicleType=NULL;	
											//sVehicleType=(char *) MEM_alloc(50);
											tc_strcpy(sVehicleType,"M_");
											tc_strcat(sVehicleType,slavecontainer);
											printf("\n group iD sVehicleType issssssssss....[%s]..\n",sVehicleType);	fflush(stdout);
									//		break;
									//	}
								//	}
								}
							}
							else 
						  {
							   tc_strcpy(sVehicleType,"NA");
						  }
					  }
						}
					}
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if( AOM_ask_value_string(ChildRevG,"t5ECUType",&Cnt_EcuType)!=ITK_ok);
					printf("\t t5ECUType   is [%s]\n",Cnt_EcuType); fflush(stdout);

					char * Cnt_EcuTypeDup =NULL;
					Cnt_EcuTypeDup=(char *) MEM_alloc(100);

					if((Cnt_EcuType))
					{
						if(tc_strstr(Cnt_EcuType,"Passive Entry Passive Start")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"PEPS");
						else if(tc_strstr(Cnt_EcuType,"Engine Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ECM");
						else if (tc_strstr(Cnt_EcuType,"Antilock Braking System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ABS");
						else if (tc_strstr(Cnt_EcuType,"Antilock/Electronic Braking System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ABS");
						else if (tc_strstr(Cnt_EcuType,"Electronic Stability Program")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ESP");
						//else if (tc_strstr(Cnt_EcuType,"Restraints Control Module (Airbag)")!=NULL) 
						else if (tc_strstr(Cnt_EcuType,"Restraints")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"RCM");
						else if (tc_strstr(Cnt_EcuType,"Body Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"BCM");
						else if (tc_strstr(Cnt_EcuType,"Heating, Ventilation, and Air Conditioning")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"HVA");
						else if (tc_strstr(Cnt_EcuType,"Fully Automated Temperature Control")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"FATC");
						else if (tc_strstr(Cnt_EcuType,"Infotainment Head Unit")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"IHU");
						else if (tc_strstr(Cnt_EcuType,"Vehicle Immobilizer Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"VICM");
						else if (tc_strstr(Cnt_EcuType,"Park Distance Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"PACM");
						else if (tc_strstr(Cnt_EcuType,"Power Steering Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"PSCM");
						else if (tc_strstr(Cnt_EcuType,"Tire Pressure Monitor System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TPMS");
						else if (tc_strstr(Cnt_EcuType,"Transmission Control Module")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TCM");
						else if (tc_strstr(Cnt_EcuType,"Drive Mode Controller/Switch")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"DMC");
						else if (tc_strstr(Cnt_EcuType,"Transmission control unit")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TCU");
						else if (tc_strstr(Cnt_EcuType,"Instrument Panel Cluster")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"IPC");
						else if (tc_strstr(Cnt_EcuType,"Torque On Demand Controller")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TOD");
						else if (tc_strstr(Cnt_EcuType,"Window Winding Auto Down Controller")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"WWAD");
						else if (tc_strstr(Cnt_EcuType,"Retarder")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"RTD");
						else if (tc_strstr(Cnt_EcuType,"Tachograph")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TCG");
						else if (tc_strstr(Cnt_EcuType,"Telematics")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"TMC");
						else if (tc_strstr(Cnt_EcuType,"Multiplexing")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"MPX");
						else if (tc_strstr(Cnt_EcuType,"Hybrid ECU")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"HCU");
						else if (tc_strstr(Cnt_EcuType,"Auto Transmission System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ATS");
						else if (tc_strstr(Cnt_EcuType,"Electronically Controlled  Air Suspension")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ECAS");
						else if (tc_strstr(Cnt_EcuType,"Emergency Braking System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"EBS");
						else if (tc_strstr(Cnt_EcuType,"Exhaust Gas Recirculation")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"EGR");
						else if (tc_strstr(Cnt_EcuType,"Selective Catalytic Reduction")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"SCR");
						else if (tc_strstr(Cnt_EcuType,"Electronic Steering Column Lock")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ESCL");
						else if (tc_strstr(Cnt_EcuType,"Unique Identification")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"UI");
						else if (tc_strstr(Cnt_EcuType,"DC-DC Converter")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"DDC");
						else if (tc_strstr(Cnt_EcuType,"Motor Controller Unit")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"MCU");
						else if (tc_strstr(Cnt_EcuType,"Battery Management System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"BMS");
						else if (tc_strstr(Cnt_EcuType,"Gear Shift Advisor")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"GSA");
						else if (tc_strstr(Cnt_EcuType,"Advanced Driver Assistance System")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"ADAS");
						else if (tc_strstr(Cnt_EcuType,"On Board Charger")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"OBC");
						else if (tc_strstr(Cnt_EcuType,"Wireless Power Charger")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"WPC");
						else if (tc_strstr(Cnt_EcuType,"Dosing Control Unit")!=NULL) 
						tc_strcpy(Cnt_EcuTypeDup,"DCU");
						else 
						tc_strcpy(Cnt_EcuTypeDup,"NA");
					}

					ITKCALL(GRM_find_relation_type("T5_HasSpareKit", &sp_kit_rel_type));

					if(sp_kit_rel_type != NULLTAG)
					{
						printf("\n\t in group ID  SPARE KIT relation found......");	fflush(stdout);	
																											 
						ITKCALL(GRM_list_secondary_objects_only(SpkitTag,sp_kit_rel_type,&cnt,&attachments));
						CHECK_FAIL;

						if (cnt > 0)
						{
						  for(jj=0;jj<cnt;jj++)
						  {
							Sparekit = attachments[jj];

							if( AOM_ask_value_string(Sparekit,"item_id",&Sp_ItemID)!=ITK_ok);
							printf("\t GRP ID Sp_ItemID NUMBER  is [%s]\n",Sp_ItemID); fflush(stdout);

							if(AOM_ask_value_string(Sparekit,"item_revision_id",&Sp_ItemREV)!=ITK_ok);
							 printf("\t GRP ID Sp_ItemREV NUMBER  is [%s]\n",Sp_ItemREV); fflush(stdout);

							char * HwPart =NULL;
							HwPart=(char *) MEM_alloc(100);
							
							char * HwPart_Sm =NULL;
							HwPart_Sm=(char *) MEM_alloc(100);

							char * ContEOLDup_Val =NULL;
							ContEOLDup_Val=(char *) MEM_alloc(100);

							 if(ContEOL)
							 {
								
								if(tc_strstr(ContEOL,"BothES")!=NULL) 
								{					
									tc_strcpy(ContEOLDup,ValBoth);
								}
								else if (tc_strstr(ContEOLDup,"NotApp")!=NULL)
								{					
									tc_strcpy(ContEOLDup,ValBoth);
								}
								else if (tc_strstr(ContEOLDup,"Service")!=NULL)
								{					
									tc_strcpy(ContEOLDup,ServVal);
								}
								else
								{
									tc_strcpy(ContEOLDup_Val,ContEOLDup);
								}
							 }
								tc_strcpy(HwPart,Sp_ItemID);
								tc_strcat(HwPart,",");
								tc_strcat(HwPart,"NA");
								tc_strcat(HwPart,",");
								tc_strcat(HwPart,Sp_ItemREV);
								tc_strcat(HwPart,"_");
								tc_strcat(HwPart,ContEOLDup_Val);


								tc_strcpy(HwPart_Sm,Sp_ItemID);
								tc_strcat(HwPart_Sm,",");
								tc_strcat(HwPart_Sm,Sp_ItemREV);

							}
						}
					}

//					char    *containerrevDup =NULL;
//					char	*strToBereplacedcre;
//					char	*strToBeUsedInsteadcre;
//	
//					strToBereplacedcre		=	(char*)malloc(5);
//					tc_strcpy(strToBereplacedcre,";");
//
//					strToBeUsedInsteadcre	=	(char*)malloc(5);
//					tc_strcpy(strToBeUsedInsteadcre,",");
//
//					containerrevDup=(char *) MEM_alloc(100);

				    //if(tc_strstr(container,"16R")==0  &&  tc_strstr(container,"16L")==0 )
					//{
					//	printf ("\n grp id --- fptr11111111111111111111111111111   \n"); fflush(stdout);

					//	ifail =STRNG_replace_str (containerrev,strToBereplacedcre, strToBeUsedInsteadcre,&containerrevDup);

						//fprintf(fptr1," \n %s,%s,%s,%s \n",Cnt_EcuTypeDup,container,containerrev,DML_name,PartNo);fflush(fptr1);
						//fprintf(fptr1," \n %s,%s,%s,%s \n",Cnt_EcuTypeDup,container,containerrev,DML_name,VCnumberDup);fflush(fptr1);
						//fprintf(fptr1,"\n %s,%s,%s,%s %s \n",Cnt_EcuTypeDup,container,containerrevdup,DML_name,req_item);fflush(fptr1);
						///fprintf(fptr1," \n %s,%s%s,%s,%s,%s,%s,%s,%s,%s \n",CCIDNumberDupS,CCIDRevDupS,req_item3,CcidAppDupS,VCnumberDup,ObjDescFinal,DML_name,t5NoEcuDup,vcDrStatDup,vcProjDup);fflush(fptr1);


					//}
                   //ChildRev

				   //	ifail = BOM_create_window (&window2);
					//CHECK_FAIL;

					//printf ("\n 222 in  ChildRevG Expansion_TPL_16  \n"); fflush(stdout);

					//ifail = BOM_set_window_config_rule( window2, rule );
					//CHECK_FAIL;

					//ifail = BOM_set_window_top_line (window2, null_tag, ChildRevG, null_tag, &top_line2);
					//CHECK_FAIL;

					//ifail = BOM_window_show_suppressed ( window2 );
					//CHECK_FAIL;

					//ifail = (BOM_set_window_pack_all(window2, FALSE));

					CNT=0;
					statusZZ = BOM_line_ask_child_lines ( childrenC[ww], &CNT, &childrenZZ);

					//printf ("\n 2222222222222222 IN Expansion_TPL_16 CNT is --->%d \n",CNT); fflush(stdout);

					if(CNT >0)
					 {
						printf("\n..INSIDE 2222222 GROUPS  ID   CNT \n");fflush(stdout);
						//CalFileNameDup=NULL;
					   // CalFileNameDup			 =(char *) MEM_alloc(100);
					   CalFileNameDup=NULL;
					   CalFileNameDup			 =(char *) MEM_alloc(200);
						 for(yy=0;yy<CNT;yy++)
						    {
								tc_strcpy(CalFileNameDup,"");
								if(AOM_ask_value_string(childrenZZ[yy],"bl_item_item_id",&contpart)!=ITK_ok);
								//printf("\t contpart ID  is [%s]\n",contpart); fflush(stdout);
								if( AOM_ask_value_string(childrenZZ[yy],"bl_rev_item_revision_id",&contpartrev)!=ITK_ok);
								//printf("\n contpartrev ID--->[%s] \n",contpartrev);fflush(stdout);
								if( AOM_ask_value_string(childrenZZ[yy],"bl_Design Revision_t5_ProjectCode ",&ProjectC)!=ITK_ok);
								printf("\n   contpart ID  is ---->[%s]  contpartrev ID-->[%s]  ProjectC is --->[%s] \n",contpart,contpartrev,ProjectC);fflush(stdout);

								const char **attrs6  = (const char **) MEM_alloc(2 * sizeof(char *));
								const char **values6 = (const char **) MEM_alloc(2 * sizeof(char *));
								attrs6[0]	   = "item_id";
								//attrs6[1]      = "object_type";
								values6[0]     = (char *)contpart;
								//values6[1]     = "Design";
								//ifail = ITEM_find_items_by_key_attributes(2,attrs, values, &n_tags_found, &tags_found);
								ifail = ITEM_find_item_revs_by_key_attributes(1,attrs6, values6,contpartrev, &n_tags_found6, &tags_found6);
								CHECK_FAIL;
								MEM_free(attrs6);
								MEM_free(values6);

								if (n_tags_found6>0)
								{
								   ContChildRev= tags_found6[0];
								}

								
								if( AOM_ask_value_string(ContChildRev,"item_id",&conitemid)!=ITK_ok);

								printf("\n GROUP ID conitemid  is----> [%s]\n",conitemid);

								if( AOM_ask_value_string(ContChildRev,"item_revision_id",&conitemrevid)!=ITK_ok);

								printf("\n GROUP ID conitemrevid  is----> [%s]\n",conitemrevid);

								char	*strToBerepl1RR =NULL;
								char	*strToBeUsed1RR =NULL;
								char	*conitemreviddup=NULL;//A;1

								strToBerepl1RR		=	(char*)malloc(5);
								tc_strcpy(strToBerepl1RR,";");

								strToBeUsed1RR	=	(char*)malloc(5);
								tc_strcpy(strToBeUsed1RR,"_");

								ifail =(STRNG_replace_str (conitemrevid,strToBerepl1R, strToBeUsed1R,&conitemreviddup));
								printf("\n conitemreviddup    ConProject is ---> [%s]\n",conitemreviddup);fflush(stdout);//A_1
								
								char	*conitemreviddupS		=	NULL;
								char	*conitemrevidduptokS	=	NULL;
								conitemreviddupS		=	(char*)malloc(10);
								tc_strcpy(conitemreviddupS,conitemreviddup);
								printf("\n GROUP ID conitemreviddupS  is----> [%s]\n",conitemreviddupS);
								printf("\n GROUP ID conitemreviddup AAAA  is----> [%s]\n",conitemreviddup);

								conitemrevidduptokS = tc_strtok (conitemreviddupS,"_");
								conitemreviddupS =tc_strtok(NULL,"");
								printf("\n GROUP ID conitemrevidduptokS  is----> [%s]\n",conitemrevidduptokS);//A
								printf("\n GROUP ID conitemreviddupS AFTER TOK is----> [%s]\n",conitemreviddupS);//1

								if( AOM_ask_value_string(ContChildRev,"t5_AppForEOL",&EOL)!=ITK_ok);
								printf("\t GROUP ID EOL Container structure   is [%s]\n",EOL); fflush(stdout);

								if( AOM_ask_value_string(ContChildRev,"t5_SwPartType",&ChdPartType)!=ITK_ok);
								printf("\t  GROUP IDChdPartType Container structure   is [%s]\n",ChdPartType); fflush(stdout);

								if( AOM_ask_value_string(ContChildRev,"t5_DesignGrp",&Desg)!=ITK_ok);
								printf("\t GROUP ID Desg GROUP   is [%s]\n",Desg); fflush(stdout);

									char	*strToBereplR=NULL;
								char	*strToBeUsedR=NULL;
				
								strToBereplR		=	(char*)malloc(5);
								tc_strcpy(strToBereplR,";");

								strToBeUsedR	=	(char*)malloc(5);
								tc_strcpy(strToBeUsedR,"_");

								ifail =(STRNG_replace_str (contpartrev,strToBereplR, strToBeUsedR,&contpartrevdup));


								char	*strToBereplRQ=NULL;
								char	*strToBeUsedRQ=NULL;
								char	*contpartrevdupSS11=NULL;
								char	*contpartrevdupSS=NULL;
								contpartrevdupSS=	(char*)malloc(30);
					

								tc_strcpy(contpartrevdupSS,contpartrevdup);
								printf("\n GROUP ID contpartrevdupSS ------------>[%s] \n",contpartrevdupSS);fflush(stdout);//G_2

								
								strToBereplRQ		=	(char*)malloc(5);
								tc_strcpy(strToBereplRQ,"_");
	
								strToBeUsedRQ	=	(char*)malloc(5);
								tc_strcpy(strToBeUsedRQ,",");

								ifail =(STRNG_replace_str (contpartrevdupSS,strToBereplRQ, strToBeUsedRQ,&contpartrevdupSS11));

								char	*contpartrevdupSS11tokS		=	NULL;
								char	*contpartseqdupSS11	=	NULL;


								contpartrevdupSS11tokS = tc_strtok (contpartrevdupSS11,"_");
								contpartseqdupSS11 =tc_strtok(NULL,"");

								printf("\n GROUP ID contpartrevdupSS11tokS  is----> [%s]\n",contpartrevdupSS11tokS);//A
								printf("\n GROUP ID conitemreviddupS AFTER TOK is----> [%s]\n",contpartseqdupSS11);//1


								char * EOLDup =NULL;
								EOLDup=(char *) MEM_alloc(100);
								if((EOL)!=NULL && tc_strlen(EOL)>0)
								{
									
								   if(tc_strstr(EOL,"BothES")!=NULL) 
									{
										tc_strcpy(EOLDup,ValBoth);
									}
								   else if (tc_strstr(EOL,"NotApp")!=NULL)
									{
										tc_strcpy(EOLDup,EOLNA);
									}
								   else if (tc_strstr(EOL,"Service")!=NULL)
									{
										tc_strcpy(EOLDup,ServVal);
									}
								   else
									{ 
										tc_strcpy(EOLDup,EOL);
									}
								}
								else 
								{ 
									tc_strcpy(EOLDup,NoValStr);
									printf("\n..GRP ID AFTER EOL COPY EOLDUP VALUE IS------>[%s] \n",EOLDup);fflush(stdout);
								}
								if((EOLDup)!=NULL && tc_strlen(EOLDup)>0) 
								{
									printf("\n..grp id EOLDup has some value [%s] \n",EOLDup);fflush(stdout);
								
								}
								else
								{
									printf("grp id Expansion_TPL_16 EOLDup value is ------>[%s] \n",EOLDup);fflush(stdout);
									tc_strcpy(EOLDup,NoValStr);
								}

//								char    *contpartrevDup =NULL;
//								char	*strToBereplacedccre;
//								char	*strToBeUsedInsteadccre;
//				
//								strToBereplacedccre		=	(char*)malloc(5);
//								tc_strcpy(strToBereplacedccre,";");
//
//								strToBeUsedInsteadccre	=	(char*)malloc(5);
//								tc_strcpy(strToBeUsedInsteadccre,",");
//
//								contpartrevDup=(char *) MEM_alloc(100);
//								ifail =STRNG_replace_str (contpartrev,strToBereplacedccre, strToBeUsedInsteadccre,&contpartrevDup);

								ifail = GRM_find_relation_type("IMAN_specification",&relation_type); 
				
								//if(relation_type != NULLTAG)
								//  {
									 ifail = GRM_list_secondary_objects_only(ContChildRev,relation_type,&cntX,&attachmentsX);
									  printf("\t attches(IMAN_Rendering)= %d  ",cntX); fflush(stdout);	
									 if (cntX > 0)
									  {						
										for(bb=0;bb<cntX;bb++)
										{
											test_file =NULL;
											test_file2 =NULL;
											test_fileX =NULL;
											pathname =NULL;
											test_file=(char *) MEM_alloc(300);
											test_file2=(char *) MEM_alloc(300);
											test_fileX=(char *) MEM_alloc(300);
											pathname=(char *) MEM_alloc(300);
											dataset = attachmentsX[bb];

											if( AOM_ask_value_string(dataset,"object_type",&objtype)!=ITK_ok);

											//if((tc_strstr(objtype,"CMI2")==0)||tc_strstr(objtype,"Creo")==0)
											if((tc_strstr(objtype,"CMI2")!=NULL)||(tc_strstr(objtype,"CAT")!=NULL)||(tc_strstr(objtype,"Creo")!=NULL)||(tc_strstr(objtype,"ProPrt")!=NULL)||(tc_strstr(objtype,"ProDrw")!=NULL))
											 {
												printf("\n  GROUP ID proe and catia datasets SO CONTINUE \n"); fflush(stdout);

												//test_file =NULL;
												//test_file=(char *) MEM_alloc(20);
												tc_strcpy(test_file,NoValStr);
												printf("\n GROUP ID else test_file not ok ------ %s\n",test_file); fflush(stdout);

												//pathname =NULL;
												//pathname=(char *) MEM_alloc(20);
												tc_strcpy(test_file2,NoValStr);
												printf("\n GROUP ID else pathname not ok ------ %s\n",test_file2); fflush(stdout);
											 }
											 else
											 {
												//if(AE_ask_dataset_ref_count(dataset,&referencenumberfound));
												//printf("\n  referencenumberfound=%d\n", referencenumberfound);fflush(stdout);

												//if (referencenumberfound > 0)
												   //{
													 //for(kp=0;kp<referencenumberfound;kp++)
														 //{
													if(AE_find_dataset_named_ref(dataset,0,refname,&reftype,&namedrefobject)!=ITK_ok);
													//CHECK_FAIL;

													if(namedrefobject)
													{

														if( AOM_ask_value_string(ContChildRev,"owning_user",&ownername)!=ITK_ok);

														ifail =  IMF_ask_original_file_name(namedrefobject,orig_name);
														printf("\tGROUP ID  orig_name: in Rendering [%s]\n",orig_name); fflush(stdout);

														ifail = (IMF_ask_file_pathname2(namedrefobject,mach_type,&pathname));	
														printf("\t GROUP ID pathname [%s]\n",pathname); fflush(stdout);

														//ifail = (IMF_ask_relative_path2(namedrefobject,&relative_path));
														//printf("\t relative_path [%s]\n",relative_path); fflush(stdout);

														
														ifail =( AOM_UIF_ask_value(namedrefobject,"file_size",&byte1));
														printf("\n GROUP IDbyte1 size is --------- [%s]\n",byte1); fflush(stdout);

														//byte1int = atoi(byte1);
														byte1int = atol(byte1);
														printf("\n  GROUP ID byte1int size is --------- [%d]\n",byte1int); fflush(stdout);

														//char * test_file =NULL;
														tc_strcpy(test_file,pathname);
														printf("\n  GROUP ID test_file [%s]\n",test_file); fflush(stdout);

														tc_strcpy(test_fileX,"/tmp");
														tc_strcat(test_fileX,"/");
														tc_strcat(test_fileX,orig_name);
														printf("\n GROUP ID test_fileX [%s]\n",test_fileX); fflush(stdout);

														tc_strcpy(test_file2,orig_name);
														printf("\n GROUP ID test_file2 [%s]\n",test_file2); fflush(stdout);

								///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//														tc_strcpy(test_file3,test_file2);
//														printf("\n test_file3 new [%s]\n",test_file3); fflush(stdout);
//
//														ItemId1 = tc_strtok(test_file3, ".");
//														ItemId2 = tc_strtok(NULL, ".");//.s //.swp
//
//														printf("\n test_file2 after after  [%s]\n",test_file2); fflush(stdout);
//														printf("\n ItemId1 after after -------- [%s]\n",ItemId1); fflush(stdout);
//														printf("\n ItemId2 after after -------- [%s]\n",ItemId2); fflush(stdout);
//

														
//T5_IndepBin
//															if((tc_strstr(test_file2,".abm")!=NULL) || (tc_strstr(test_file2,".sqfs")!=NULL))
//															{
//																tc_strcpy(reference_nameDS,"T5_abm");
//															}
//															else if(tc_strstr(test_file2,".s19")!=NULL) 
//															{
//																tc_strcpy(reference_nameDS,"T5_s19");
//															}
//															else if(tc_strstr(test_file2,".S19")!=NULL) 
//															{
//																tc_strcpy(reference_nameDS,"T5_s19");
//															}
//															else if(tc_strstr(test_file2,".ecs")!=NULL) 
//															{
//																tc_strcpy(reference_nameDS,"T5_ecs");
//															}
//															else if(tc_strstr(test_file2,".hxk")!=NULL) 
//															{
//																tc_strcpy(reference_nameDS,"T5_hxk");
//															}
//															else if(tc_strstr(test_file2,".hex")!=NULL) 
//															{
//																tc_strcpy(reference_nameDS,"T5_hex");
//															}
//															else if(tc_strstr(test_file2,".sol")!=NULL) 
//															{
//																tc_strcpy(reference_nameDS,"T5_sol");
//															}
//															else if(tc_strstr(test_file2,".par")!=NULL) 
//															{
//																tc_strcpy(reference_nameDS,"T5_par");
//															}
//															else if(tc_strstr(test_file2,".ecu")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_ecu");
//															}
//															else if(tc_strstr(test_file2,".mhx")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_mhx");
//															}
//															else if(tc_strstr(test_file2,".iso")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_iso");
//															}
//															else if(tc_strstr(test_file2,".prm")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_prm");
//															}
//															else if(tc_strstr(test_file2,".ulp")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_ulp");
//															}
//														    else if(tc_strstr(test_file2,".swl")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_swl");
//															}
//															else if(tc_strstr(test_file2,".bin")!=NULL)
//															{
//																 tc_strcpy(reference_nameDS,"T5_bin");
//															}
//															else if(tc_strstr(test_file2,".idx")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_idx");
//															}
//															else if(tc_strstr(test_file2,".sal")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_sal");
//															}
//															else if(tc_strstr(test_file2,".crc")!=NULL)
//															{			
//																tc_strcpy(reference_nameDS,"T5_crc");
//															}
//															else if((tc_strcmp(ItemId2,"s")==0) ||(tc_strcmp(ItemId2,"S")==0))
//															{
//																tc_strcpy(reference_nameDS,"T5_s");
//															}
//															else if(tc_strstr(test_file2,".n07")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_n07");
//															}
//															else if(tc_strstr(test_file2,".N07")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_n07");
//															}
//															else if((tc_strstr(test_file2,".s07")!=NULL) || (tc_strstr(test_file2,".S07")!=NULL))
//															{
//																tc_strcpy(reference_nameDS,"T5_s07");
//															}														
//															else if(tc_strstr(test_file2,".rec")!=NULL)
//															{
//																tc_strcpy(reference_nameDS,"T5_rec");
//															}
															//else if(tc_strstr(test_file2,".sqfs")!=NULL)
															//{
															//	tc_strcpy(reference_nameDS,"T5_sqfs");
															//}
															//else if(tc_strstr(test_file2,".srec")!=NULL)
															//{
															//	tc_strcpy(reference_nameDS,"T5_srec");
															//}


//															printf("\t Inside reference_nameDS %s\n",reference_nameDS); fflush(stdout);
															tc_strcpy(Exfile," ");
															//strcpy(Exfile,"/home/testcmi/Adi/DIGIBUCK/BOMListJT/");
															//tc_strcpy(Exfile,"/user/bsd05818/Adi/");
															tc_strcpy(Exfile,"/tmp/");
															tc_strcat(Exfile,orig_name);
															printf("\t exporting now\n"); fflush(stdout);
															//AE_export_named_ref(dataset,reference_nameDS,Exfile);
															IMF_export_file(namedrefobject,Exfile);
															printf("\t GROUP ID Exfile is ---- [%s]\n",Exfile); fflush(stdout);
															strcpy(Exfile," ");
															flagY=0;

													}
													else
													{
														printf("\n problem with named reference GRPID \n"); fflush(stdout);
														flagY=1;
														tc_strcpy(test_file2,"");
														tc_strcat(test_file2,"NA");
														printf("\n AAAAA test_file2 AAAAAA [%s]\n",test_file2); fflush(stdout);
													}

															supfile=1;
															if  (((tc_strcmp(ChdPartType,"CAL") == 0)|| tc_strcmp(ChdPartType,"HCL") == 0) && ((tc_strstr(ContEOLDup,"BOTH")!=NULL) || tc_strcmp(ContEOLDup,"EOL")==0))
															{
																printf("\n..GRP ID Container having Applicability ......%s",ContEOLDup);fflush(stdout);
																CalFlileReq=1;

															}
														//}
												    //}
											    }
											//CalFileName=(char *) MEM_alloc(100);
											//tc_strcpy(CalFileName,NoValStr);

											//fprintf(fptr5,"\n%s,%s,%s,%s,%s,%s,%s,%s,%ld,\n",Assy_obj,container,contpart,test_file,containerrev,contpartrev,DML_name,EOLDup,byte1);fflush(fp5); //check for byte 1 - rest ok
											//printf("\n GRP ID fptr555555555555555555555555 ");fflush(stdout);

											//fprintf(fptr5,"\n%s,%s,%s,%s,%s,%s,%s,%s,\n",PartNo,container,contpart,test_file,containerrev,contpartrev,DML_name,EOLDup);fflush(fptr5); 
											//fprintf(fptr5,"\n%s,%s,%s,%s,%s,%s,%s,%s,\n",VCnumberDup,container,contpart,test_file,containerrev,contpartrev,DML_name,EOLDup);fflush(fptr5); 

											if (!(tc_strcmp(ChdPartType,"CAL") == 0 || tc_strcmp(ChdPartType,"HCL") == 0))
											{
												printf("\n Part is not calibaration SW");fflush(stdout);
												//calfiletest=0;
												
											}
											else
											{										
												//tc_strcpy(CalFileName,relative_path);
												 if ((tc_strstr(TPLEOLVAL,"BOTH")!=0) || (tc_strstr(TPLEOLVAL,"EOL")!=0))
												{
													calfiletest=1;
												}
												else
												{
													calfiletest=0;
												}
											}
											//tc_strcpy(AtttachFileNames,test_file);
											//tc_strcat(AtttachFileNames,";");
										//	printf("\n GRP ID fptr444444444444444444444444 ");fflush(stdout);
										//   fprintf(fptr4,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n",ProjectC,Desg,pathname,contpart,contpartrevDup,CurrentDateGDup,container,containerrevDup,DML_name,EOLDup,test_file);fflush(fptr4);
										}
									}

								if((flagY==1) && (tc_strcmp(test_file,"NA") != 0 ))
								{
								printf("\n GRP ID fptr555555555555555555555555 ");fflush(stdout);

								//fprintf(fptr5,"\n%s,%s,%s,%s,%s,%s,%s,%s,\n",req_item,container,contpart,test_file,containerrevDup,contpartrevDup,DML_name,EOLDup);fflush(fptr5); 
								//fprintf(fptr5,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%ld \n",req_item,container,conitemid,test_file2,contpartrevdup,conitemrevidduptokS,conitemreviddupS,DML_name,EOLDup,byte1int);fflush(fptr5); //change byte size above later on
								fprintf(fptr5,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%ld \n",VCnumberDup,container,conitemid,test_file2,containerrevdup,conitemrevidduptokS,conitemreviddupS,DML_name,EOLDup,byte1int);fflush(fptr5); 



								printf("\n GRP ID fptr444444444444444444444444 ");fflush(stdout);
								//fprintf(fptr4,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n",ProjectC,Desg,pathname,contpart,contpartrevDup,CurrentDateGDup,container,containerrevDup,DML_name,EOLDup,test_file);fflush(fptr4);
								//fprintf(fptr4,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",ProjectC,Desg,test_fileX,container,contpartrevdupSS11tokS,NewDateG,conitemid,conitemreviddup,DML_name,EOLDup,test_file2);fflush(fptr4);
								fprintf(fptr4,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",ConProject,Desg,test_fileX,conitemid,contpartrevdupSS11tokS,NewDateG,container,containerrevdup,DML_name,EOLDup,test_file2);fflush(fptr4);
								

								}
							//	 }																									
							//char * test_file_new =NULL;
							//test_file_new=(char *) MEM_alloc(3000);
							//tc_strcpy(test_file_new,test_file);
							iWrite  =0;

						   Sticker			 =(char *) MEM_alloc(50);
						   if (tc_strcmp(ChdPartType, "STK") == 0)
							{
								flag=1;
								tc_strcpy(Sticker,contpart);
								tc_strcat(Sticker,",");
								tc_strcat(Sticker,contpartrevdup);
								tc_strcat(Sticker,"_");
								tc_strcat(Sticker,EOLDup);
							}
							else if (tc_strcmp(ChdPartType, "PBL") == 0)
							{
								Pr_Boot_Loader_Sm=(char *) MEM_alloc(200);
								Pr_Boot_Loader	 =(char *) MEM_alloc(50);

								tc_strcpy(Pr_Boot_Loader_Sm,contpart);
								tc_strcat(Pr_Boot_Loader_Sm,",");
								tc_strcat(Pr_Boot_Loader_Sm,contpartrevdup);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart);//?????
								setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(Pr_Boot_Loader,contpart);
								tc_strcat(Pr_Boot_Loader,",");
								tc_strcat(Pr_Boot_Loader,contpartrevdup);
								tc_strcat(Pr_Boot_Loader,"_");
								tc_strcat(Pr_Boot_Loader,EOLDup);

							}
							else if (tc_strcmp(ChdPartType, "SBL") == 0)
							{
								Se_Boot_Loader_Sm =(char *) MEM_alloc(200);
								Se_Boot_Loader	 =(char *) MEM_alloc(200);

								tc_strcpy(Se_Boot_Loader_Sm,contpart);
								tc_strcat(Se_Boot_Loader_Sm,",");
								tc_strcat(Se_Boot_Loader_Sm,contpartrevdup);
								
								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart); //?????
								setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(Se_Boot_Loader,contpart);
								tc_strcat(Se_Boot_Loader,",");
								tc_strcat(Se_Boot_Loader,contpartrevdup);
								tc_strcat(Se_Boot_Loader,"_");
								tc_strcat(Se_Boot_Loader,EOLDup);

							}
							else if (tc_strcmp(ChdPartType, "BAS") == 0)
							{
								BasicSw			 =(char *) MEM_alloc(100);

								//low_set_add_str(SWDetails,contpart); //?????
								tc_strcpy(BasicSw,contpart);
								tc_strcat(BasicSw,"_");
								tc_strcat(BasicSw,contpartrevdup);
								tc_strcat(BasicSw,"_");
								tc_strcat(BasicSw,EOLDup);
							}
							else if (tc_strcmp(ChdPartType, "APP") == 0)
							{
								AppSw			 =(char *) MEM_alloc(100);
								AppSw_Sm	     =(char *) MEM_alloc(200);
								tc_strcpy(AppSw_Sm,contpart);
								tc_strcat(AppSw_Sm,",");
								tc_strcat(AppSw_Sm,contpartrevdup);
								
								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart);//>????
								setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(AppSw,contpart);
								tc_strcat(AppSw,",");
								tc_strcat(AppSw,contpartrevdup);
								tc_strcat(AppSw,"_");
								tc_strcat(AppSw,EOLDup);
							}
							else if (tc_strcmp(ChdPartType,"CAL") == 0)
							{
								
								CalSw_Sm	     =(char *) MEM_alloc(100);
								CalSw			 =(char *) MEM_alloc(100);
								tc_strcpy(CalSw_Sm,contpart);
								tc_strcat(CalSw_Sm,",");
								//tc_strcat(CalSw_Sm,CalFileName);
								tc_strcat(CalSw_Sm,test_file2);
								tc_strcat(CalSw_Sm,",");
								tc_strcat(CalSw_Sm,contpartrevdup);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

							//low_set_add_str(SWDetails,contpart);//????
							   setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(CalSw,contpart);
								tc_strcat(CalSw,",");
								tc_strcat(CalSw,contpartrevdup);
								tc_strcat(CalSw,"_");
								tc_strcat(CalSw,EOLDup);

								tc_strcat(CalSwDup,contpartrevdup);
								tc_strcat(CalSwDup,"_");
								tc_strcat(CalSwDup,EOLDup);

								
								tc_strcpy(CalFileNameDup,test_file2);
								printf("\n group id CalFileNameDup CAL isssss 4 GRPIDDDDD ------------>[%s] \n",CalFileNameDup);fflush(stdout);

								if(CalFileNameDup)
								 {
									printf("\n GRP ID some value in  CAL CALFILENAME so continue %s  \n",CalFileNameDup);fflush(stdout);
								 }
								else
								 {	
									tc_strcpy(CalFileNameDup,"NA");
									printf("\n  GRP ID CAL cal file is null so setting to  %s\n",CalFileNameDup);fflush(stdout);
								 }

							}
							else if (tc_strcmp(ChdPartType, "CFG") == 0)
							{
								CfgSw_Sm		 =(char *) MEM_alloc(200);
								CfgSw		     =(char *) MEM_alloc(200);
								tc_strcpy(CfgSw_Sm,contpart);
								tc_strcat(CfgSw_Sm,",");
								tc_strcat(CfgSw_Sm,contpartrevdup);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart);//????
								setAddStr(&iWrite,&SWDetails,contpart);
								tc_strcpy(CfgSw,contpart);
								tc_strcat(CfgSw,",");
								tc_strcat(CfgSw,contpartrevdup);
								tc_strcat(CfgSw,"_");
								tc_strcat(CfgSw,EOLDup);
							}
							else if (tc_strcmp(ChdPartType, "VCI") == 0)
							{
								VehCal			 =(char *) MEM_alloc(50);
								VehCal_Sm		 =(char *) MEM_alloc(200);
								tc_strcpy(VehCal_Sm,contpart);
								tc_strcat(VehCal_Sm,",");
								tc_strcat(VehCal_Sm,contpartrevdup);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(SWDetails,contpart);//????
								setAddStr(&iWrite,&SWDetails,contpart);
								tc_strcpy(VehCal,contpart);
								tc_strcat(VehCal,",");
								tc_strcat(VehCal,contpartrevdup);
								tc_strcat(VehCal,"_");
								tc_strcat(VehCal,EOLDup);

							}
							else if (tc_strcmp(ChdPartType,"HWC") == 0)
							{
								HwPart			 =(char *) MEM_alloc(200);
								HwPart_Sm		 =(char *) MEM_alloc(200);

								tc_strcpy(HwPart_Sm,contpart);
								tc_strcat(HwPart_Sm,",");
								tc_strcat(HwPart_Sm,contpartrevdup);

								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");

								//low_set_add_str(HWDetails,contpart);//?????
								setAddStr(&iWrite,&HWDetails,contpart);
								tc_strcpy(HwPart,contpart);
								tc_strcat(HwPart,",");
								tc_strcat(HwPart,contpartrevdup);
								tc_strcat(HwPart,"_");
								tc_strcat(HwPart,EOLDup);
							}
							else if (tc_strcmp(ChdPartType,"HCL") == 0)
							{
								CalSw		 =(char *) MEM_alloc(200);
								CalSwDup	 =(char *) MEM_alloc(200);
								VehCal	     =(char *) MEM_alloc(100);
								CalSw_Sm	     =(char *) MEM_alloc(100);
							
								VehCal_Sm	     =(char *) MEM_alloc(100);
								//Cal code
								tc_strcpy(CalSw_Sm,contpart);
								tc_strcat(CalSw_Sm,",");
								//tc_strcat(CalSw_Sm,CalFileName);
								tc_strcat(CalSw_Sm,test_file2);
								tc_strcat(CalSw_Sm,",");
								tc_strcat(CalSw_Sm,contpartrevdup);
								
								tc_strcat(contpart,",");
								tc_strcat(contpart,"NA");
								//low_set_add_str(SWDetails,contpart);//????
								setAddStr(&iWrite,&SWDetails,contpart);

								tc_strcpy(CalSw,contpart);
								tc_strcat(CalSw,",");
								tc_strcat(CalSw,contpartrevdup);
								tc_strcat(CalSw,"_");
								tc_strcat(CalSw,EOLDup);

								tc_strcat(CalSwDup,contpartrevdup);
								tc_strcat(CalSwDup,"_");
								tc_strcat(CalSwDup,EOLDup);

								tc_strcpy(VehCal_Sm,contpart);
								tc_strcat(VehCal_Sm,",");
								tc_strcat(VehCal_Sm,contpartrevdup);

								//low_set_add_str(SWDetails,contpart);//????
								setAddStr(&iWrite,&SWDetails,contpart);
								tc_strcpy(VehCal,contpart);
								tc_strcat(VehCal,",");
								tc_strcat(VehCal,contpartrevdup);
								tc_strcat(VehCal,"_");
								tc_strcat(VehCal,EOLDup);

								tc_strcpy(CalFileNameDup,test_file2);
								printf("\n CalFileNameDup HCL isssss FOR GRPIDDDDD ------------>[%s] \n",CalFileNameDup);fflush(stdout);

								if(CalFileNameDup)
								 {
									printf("\n some value in  HCL CALFILENAME so continue %s  \n",CalFileNameDup);fflush(stdout);
								 }
								else
								 {	
									tc_strcpy(CalFileNameDup,"NA");
							
									printf("\n  HCL cal file is null so setting to  %s\n",CalFileNameDup);fflush(stdout);
								 }
							}
						}
						if((HwPart)&& tc_strlen(HwPart)>0)
						{
							printf("\n  grp id HwPart------- %s\n",HwPart);fflush(stdout);						
						}
						else
						{
							HwPart	     =(char *) MEM_alloc(100);
                            tc_strcpy(HwPart,"NA,-,-,-");
							
							printf("\n  else grp id HwPart ------%s\n",HwPart);fflush(stdout);	
						}
						if((HwPart_Sm)&& tc_strlen(HwPart_Sm)>0)
						{
							printf("\n  grp id HwPart_Sm -----%s \n",HwPart_Sm);fflush(stdout);							
						}
						else
						{
							HwPart_Sm	     =(char *) MEM_alloc(100);
                             tc_strcpy(HwPart_Sm,"-,-,-");
							 printf("\n  else grp id HwPart_Sm ------%s \n",HwPart_Sm);fflush(stdout);
						}
						if((VehCal)&& tc_strlen(VehCal)>0)
						{
							printf("\n  grp id VehCal----- %s \n",VehCal);fflush(stdout);							
						}
						else
						{
							VehCal	     =(char *) MEM_alloc(100);
                            tc_strcpy(VehCal,"NA,-,-,-");
							printf("\n  else grp id VehCal -----%s  \n",VehCal);fflush(stdout);
						}

						if((VehCal_Sm)&& tc_strlen(VehCal_Sm)>0)
						{
							printf("\n  grp id VehCal_Sm---- %s \n",VehCal_Sm);fflush(stdout);							
						}
						else
						{
							VehCal_Sm	     =(char *) MEM_alloc(100);
                            tc_strcpy(VehCal_Sm,"-,-,-");
							printf("\n else grp id VehCal_Sm -----%s \n",VehCal_Sm);fflush(stdout);
						}
						if((CfgSw)&& tc_strlen(CfgSw)>0)
						{
							printf("\n  grp id CfgSw -----%s \n",CfgSw);fflush(stdout);							
						}
						else
						{
							CfgSw	     =(char *) MEM_alloc(100);
                            tc_strcpy(CfgSw,"NA,-,-,-");
							printf("\n  else grp id CfgSw----- %s \n",CfgSw);fflush(stdout);
						}
						if((CfgSw_Sm)&& tc_strlen(CfgSw_Sm)>0)
						{
							printf("\n  grp id CfgSw_Sm ----- %s \n",CfgSw_Sm);fflush(stdout);						
						}
						else
						{
							CfgSw_Sm	     =(char *) MEM_alloc(100);
                            tc_strcpy(CfgSw_Sm,"-,-,-");
							printf("\n  else grp id CfgSw_Sm ----- %s \n",CfgSw_Sm);fflush(stdout);
						}
						if((CalSw)&& tc_strlen(CalSw)>0)
						{
							printf("\n  grp id CalSw %s \n",CalSw);fflush(stdout);
						}
						else
						{
							CalSw	     =(char *) MEM_alloc(100);
                            tc_strcpy(CalSw,"NA,-,-,-");
							printf("\n  else grp id CalSw----- %s \n",CalSw);fflush(stdout);
						}
						if((CalSw_Sm)&& tc_strlen(CalSw_Sm)>0)
						{
							printf("\n  grp id CalSw_Sm ----%s \n",CalSw_Sm);fflush(stdout);							
						}
						else
						{
							CalSw_Sm	     =(char *) MEM_alloc(100);
                            tc_strcpy(CalSw_Sm,"-,-,-,-");
							printf("\n  else grp id CalSw_Sm----- %s \n",CalSw_Sm);fflush(stdout);
						}
						if((AppSw)&& tc_strlen(AppSw)>0)
						{
							printf("\n  grp id AppSw -----%s \n",AppSw);fflush(stdout);
						}
						else
						{
							AppSw	     =(char *) MEM_alloc(100);
                            tc_strcpy(AppSw,"NA,-,-,-");
							printf("\n  else grp id AppSw -----%s \n",AppSw);fflush(stdout);
						}

						if((AppSw_Sm)&& tc_strlen(AppSw_Sm)>0)
						{
							printf("\n  grp id AppSw_Sm----- %s \n",AppSw_Sm);fflush(stdout);
						}
						else
						{
							AppSw_Sm	     =(char *) MEM_alloc(100);
                            tc_strcpy(AppSw_Sm,"-,-,-");
							printf("\n else grp id AppSw_Sm ------%s \n",AppSw_Sm);fflush(stdout);
						}
						if((BasicSw)&& tc_strlen(BasicSw)>0)
						{
							printf("\n  grp id BasicSw----- %s \n",BasicSw);fflush(stdout);							
						}
						else
						{
							BasicSw	     =(char *) MEM_alloc(100);
                            tc_strcpy(BasicSw,"NA");
							printf("\n else grp id BasicSw -----%s \n",BasicSw);fflush(stdout);
						}
						if((Se_Boot_Loader)&& tc_strlen(Se_Boot_Loader)>0)
						{
							printf("\n  grp id Se_Boot_Loader -----%s \n",Se_Boot_Loader);fflush(stdout);
						}
						else
						{
							Se_Boot_Loader	     =(char *) MEM_alloc(100);
                            tc_strcpy(Se_Boot_Loader,"NA,-,-,-");
							printf("\n else  grp id Se_Boot_Loader %s \n",Se_Boot_Loader);fflush(stdout);
						}
						if((Se_Boot_Loader_Sm)&& tc_strlen(Se_Boot_Loader_Sm)>0)
						{
							printf("\n  grp id Se_Boot_Loader_Sm -----%s \n",Se_Boot_Loader_Sm);fflush(stdout);
						}
						else
						{
							Se_Boot_Loader_Sm	     =(char *) MEM_alloc(100);
                            tc_strcpy(Se_Boot_Loader_Sm,"-,-,-");
							printf("\n else  grp id Se_Boot_Loader_Sm  -----%s \n",Se_Boot_Loader_Sm);fflush(stdout);
						}
						if((Pr_Boot_Loader)&& tc_strlen(Pr_Boot_Loader)>0)
						{
							printf("\n grp id Pr_Boot_Loader------- %s \n",Pr_Boot_Loader);fflush(stdout);
						}
						else
						{  
							Pr_Boot_Loader	     =(char *) MEM_alloc(100);
							tc_strcpy(Pr_Boot_Loader,"NA,-,-,-");
							printf("\n else grp id Pr_Boot_Loader------- %s \n",Pr_Boot_Loader);fflush(stdout);
						}
						if((Pr_Boot_Loader_Sm)&& tc_strlen(Pr_Boot_Loader_Sm)>0)
						{
							printf("\n Pr_Boot_Loader_Sm ----- %s\n",Pr_Boot_Loader_Sm);fflush(stdout);
						}
						else
						{
							Pr_Boot_Loader_Sm	     =(char *) MEM_alloc(100);
                             tc_strcpy(Pr_Boot_Loader_Sm,"-,-,-");
							 printf("\n else Pr_Boot_Loader_Sm ----- %s\n",Pr_Boot_Loader_Sm);fflush(stdout);
						}
						if((Sticker)&& tc_strlen(Sticker)>0)
						{
							 printf("\n Sticker not null ------ %s\n",Sticker);fflush(stdout);
						}
						else
						{
							Sticker	     =(char *) MEM_alloc(100);
                             tc_strcpy(Sticker,"-,-,-");
							 printf("\n else Sticker not null ------ %s\n",Sticker);fflush(stdout);
						}
						if(flag==0)
						{
							tc_strcpy(Sticker,"-,-,-");
							printf("\n flag Sticker  ------ %s\n",Sticker);fflush(stdout);
						}

						printf("\n CalFileNameDup group ID is $$$$$$$$$$$$$$$$$$$$$$ %s  \n",CalFileNameDup);fflush(stdout);

						  if((tc_strstr(CalFileNameDup,".crc")!=NULL)||(tc_strstr(CalFileNameDup,".rec")!=NULL)||(tc_strstr(CalFileNameDup,".s")!=NULL)||(tc_strstr(CalFileNameDup,".n07")!=NULL)
							||(tc_strstr(CalFileNameDup,".s07")!=NULL)||(tc_strstr(CalFileNameDup,".N07")!=NULL)||(tc_strstr(CalFileNameDup,".S07")!=NULL)||(tc_strstr(CalFileNameDup,".S19")!=NULL)
							||(tc_strstr(CalFileNameDup,".hex")!=NULL)||(tc_strstr(CalFileNameDup,".ecs")!=NULL)||(tc_strstr(CalFileNameDup,".par")!=NULL)||(tc_strstr(CalFileNameDup,".sol")!=NULL)
							||(tc_strstr(CalFileNameDup,".mhx")!=NULL)||(tc_strstr(CalFileNameDup,".ulp")!=NULL)||(tc_strstr(CalFileNameDup,".swl")!=NULL)||(tc_strstr(CalFileNameDup,".bin")!=NULL)
							||(tc_strstr(CalFileNameDup,".abm")!=NULL)||(tc_strstr(CalFileNameDup,".iso")!=NULL)||(tc_strstr(CalFileNameDup,".sal")!=NULL)||(tc_strstr(CalFileNameDup,".idx")!=NULL)
							||(tc_strstr(CalFileNameDup,".sqfs")!=NULL)||(tc_strstr(CalFileNameDup,".hxk")!=NULL))
						 {
							printf("\n indep binary file found  %s  \n",CalFileNameDup);fflush(stdout);
							CalFlileReq=1;
						 }
						else
						 {	
							tc_strcpy(CalFileNameDup,"NA");
							printf("\n cal file is null so setting to  %s\n",CalFileNameDup);fflush(stdout);
						 }

//						 if((tc_strstr(CalFileNameDup,"NA")!=NULL)||(tc_strlen(CalFileNameDup)==0)
//						 {
//							 CalFlileReq=1;
//						 }

						///if(tc_strstr(container,"16R")==0  &&  tc_strstr(container,"16L")==0 )
						//{
							if((Sticker))
							{
								printf("\n..grp id fptr333333333333333333333 >>>>>>>>>>>>>>.. \n");fflush(stdout);
								fprintf(fptr3,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s\n",req_item,Objdesc,container,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,Sticker,containerrevdup,ConProject,"NA",DML_name,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,sVehicleType,DRStatus);fflush(fptr3);
								//fprintf(fp3,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s,\n",vcnoDup,vcdescDup,containerDup,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,",",StickerDup,containerrevdup,ConProjectDup,"NA",DmlNo,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,"_");fflush(fp3);


							}
							else
							{	
								printf("\n..grp id fptr333333333333333333333 >>>>>>>>>>>>>>.. \n");fflush(stdout);							
								fprintf(fptr3,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s\n",req_item,Objdesc,container,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,",",Sticker,containerrevdup,ConProject,"NA",DML_name,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,sVehicleType,DRStatus);fflush(fptr3);
								//fprintf(fp3,"\n%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%s,%s,%s,\n",vcnoDup,vcdescDup,containerDup,CalSw,Cnt_EcuTypeDup,CalFileNameDup,HwPart,VehCal,CfgSw,AppSw,Pr_Boot_Loader,Se_Boot_Loader,StickerDup,containerrevdup,ConProjectDup,"NA",DmlNo,CalFlileReq,supfile,ContEOLDup_Val,t5CountryValDup,t5MajorModelDup,BasicSw,t5NoEcuDup,t5ABSValDup,"_");fflush(fp3);

							}
						//}
						//if(tc_strstr(container,"16R")==0  &&  tc_strstr(container,"16L")==0 )
						//{
							printf("\n..grp id fptr666666666666666666666 >>>>>>>>>>>>>>.. \n");fflush(stdout);
							//fprintf(fptr6,"\n'%s,%s,'%s,%s,'%s,%s,'%s,'%s,'%s,'%s,'%s,'%s,%s,\n",PartNo,Objdesc,container,containerrev,CalSw_Sm,Cnt_EcuTypeDup,HwPart_Sm,VehCal_Sm,CfgSw_Sm,AppSw_Sm,Pr_Boot_Loader_Sm,Se_Boot_Loader_Sm,t5NoEcuDup);fflush(fptr6);
							//fprintf(fptr6,"\n'%s,%s,'%s,%s,'%s,%s,'%s,'%s,'%s,'%s,'%s,'%s,%s,\n",VCnumberDup,Objdesc,container,containerrev,CalSw_Sm,Cnt_EcuTypeDup,HwPart_Sm,VehCal_Sm,CfgSw_Sm,AppSw_Sm,Pr_Boot_Loader_Sm,Se_Boot_Loader_Sm,t5NoEcuDup);fflush(fptr6);
							//fprintf(fptr6,"\n'%s,%s,'%s,%s,'%s,%s,'%s,'%s,'%s,'%s,'%s,'%s,%s,\n",req_item,Objdesc,container,containerrevDup,CalSw_Sm,Cnt_EcuTypeDup,HwPart_Sm,VehCal_Sm,CfgSw_Sm,AppSw_Sm,Pr_Boot_Loader_Sm,Se_Boot_Loader_Sm,t5NoEcuDup);fflush(fptr6);
							fprintf(fptr6,"\n %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",req_item,Objdesc,container,containerrev,CalSw_Sm,Cnt_EcuTypeDup,HwPart_Sm,VehCal_Sm,CfgSw_Sm,AppSw_Sm,Pr_Boot_Loader_Sm,Se_Boot_Loader_Sm,t5NoEcuDup);fflush(fptr6);

						//}
						CalSw_Sm="";
						VehCal_Sm="";
						Se_Boot_Loader_Sm="";
						AppSw_Sm="";
						CfgSw_Sm="";
						Pr_Boot_Loader_Sm="";
						HwPart_Sm="";
						CalFileNameDup="";

						flag=0;

						if((HwPart))
						{
							//MEM_free (HwPart);
							HwPart=NULL;
						}
						printf("\n..VehCal grp id going to file.. %s\n",VehCal);fflush(stdout);
						if((VehCal)!=NULL)
						{
							VehCal=NULL;
							printf("\n..inside  grp id mem freee VehCal \n");fflush(stdout);
							//MEM_free (VehCal); 							
						}
						printf("\n.CfgSw grp id .befor going to file..%s\n",CfgSw);fflush(stdout);
						if((CfgSw)!=NULL)
						{
							//MEM_free (CfgSw); 
							printf("\n..inside  grp id mem freee CfgSw \n");fflush(stdout);
							CfgSw=NULL;
						}
						printf("\n.CalSw grp id .befor going to file..%s\n",CalSw);fflush(stdout);
						if((CalSw)!=NULL)
						{
							//MEM_free (CalSw); 
							CalSw=NULL;
							printf("\n..inside  grp id mem freee CalSw \n");fflush(stdout);
						}
						printf("\n.AppSw grp id .befor going to file..%s\n",AppSw);fflush(stdout);
						if((AppSw)!=NULL)
						{
							AppSw=NULL;
							//MEM_free (AppSw);
							printf("\n..inside  grp id mem freee AppSw \n");fflush(stdout);
						}
						printf("\n.BasicSw grp id .befor going to file..%s\n",BasicSw);fflush(stdout);
						if((BasicSw)!=NULL)
						{
							//MEM_free (BasicSw); 
							BasicSw=NULL;
							printf("\n..inside  grp id mem freee BasicSw \n");fflush(stdout);
						}
						printf("\n.Se_Boot_Loader grp id .befor going to file..%s\n",Se_Boot_Loader);fflush(stdout);
						if((Se_Boot_Loader)!=NULL)
						{
							//MEM_free (Se_Boot_Loader);
							Se_Boot_Loader=NULL;
							printf("\n..inside  grp id mem freee Se_Boot_Loader \n");fflush(stdout);
						}
						printf("\n..Pr_Boot_Loader grp id befor going to file..%s\n",Pr_Boot_Loader);fflush(stdout);
						if((Pr_Boot_Loader)!=NULL)
						{
							//MEM_free (Pr_Boot_Loader); 	
							Pr_Boot_Loader=NULL;
						}
						printf("\n..Sticker grp id befor going to file..%s\n",Sticker);fflush(stdout);
						if((Sticker)!=NULL)
						{
							//MEM_free (Sticker);
							Sticker=NULL;
						}
					}
				}
				else
				{ 
						char * PRM_EcuTypeDup =NULL;
						PRM_EcuTypeDup=(char *) MEM_alloc(100);

						char * ContainerListVal =NULL;
						ContainerListVal=(char *) MEM_alloc(100);

						char * ContainerRevValDuptk =NULL;
						ContainerRevValDuptk=(char *) MEM_alloc(100);

						char * ContainerRevVal =NULL;
						ContainerRevVal=(char *) MEM_alloc(50);

						char    *ContainerRevValDup =NULL;
						ContainerRevValDup=(char *) MEM_alloc(50);

						char	*strToBereplacedconcre;
						char	*strToBeUsedInsteadconcre;
		
						strToBereplacedconcre		=	(char*)malloc(5);
						tc_strcpy(strToBereplacedconcre,";");

						strToBeUsedInsteadconcre	=	(char*)malloc(5);
						tc_strcpy(strToBeUsedInsteadconcre,",");

						//ifail =STRNG_replace_str (ContainerRevVal,strToBereplacedconcre, strToBeUsedInsteadconcre,&ContainerRevValDup);

					if (tc_strcmp(PartType,"PRM") == 0  && tc_strcmp(EEPartType,"G")!=0)
					{
						if( AOM_ask_value_string(ChildRevG,"t5ECUType",&PRM_EcuType)!=ITK_ok);//check container has parameter
						printf("\t PRM_EcuType   is [%s]\n",PRM_EcuType); fflush(stdout);

						if((PRM_EcuType))
						{
							if(tc_strstr(PRM_EcuType,"Passive Entry Passive Start")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"PEPS");
							else if(tc_strstr(PRM_EcuType,"Engine Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ECM");
							else if (tc_strstr(PRM_EcuType,"Antilock Braking System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ABS");
							else if (tc_strstr(PRM_EcuType,"Antilock/Electronic Braking System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ABS");
							else if (tc_strstr(PRM_EcuType,"Electronic Stability Program")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ESP");
							else if (tc_strstr(PRM_EcuType,"Restraints")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"RCM");
							else if (tc_strstr(PRM_EcuType,"Body Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"BCM");
							else if (tc_strstr(PRM_EcuType,"Heating, Ventilation, and Air Conditioning")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"HVA");
							else if (tc_strstr(PRM_EcuType,"Fully Automated Temperature Control")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"FATC");
							else if (tc_strstr(PRM_EcuType,"Infotainment Head Unit")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"IHU");
							else if (tc_strstr(PRM_EcuType,"Vehicle Immobilizer Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"VICM");
							else if (tc_strstr(PRM_EcuType,"Park Distance Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"PACM");
							else if (tc_strstr(PRM_EcuType,"Power Steering Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"PSCM");
							else if (tc_strstr(PRM_EcuType,"Tire Pressure Monitor System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TPMS");
							else if (tc_strstr(PRM_EcuType,"Transmission Control Module")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TCM");
							else if (tc_strstr(PRM_EcuType,"Drive Mode Controller/Switch")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"DMC");
							else if (tc_strstr(PRM_EcuType,"Transmission control unit")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TCU");
							else if (tc_strstr(PRM_EcuType,"Instrument Panel Cluster")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"IPC");
							else if (tc_strstr(PRM_EcuType,"Torque On Demand Controller")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TOD");
							else if (tc_strstr(PRM_EcuType,"Window Winding Auto Down Controller")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"WWAD");
							else if (tc_strstr(PRM_EcuType,"Retarder")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"RTD");
							else if (tc_strstr(PRM_EcuType,"Tachograph")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TCG");
							else if (tc_strstr(PRM_EcuType,"Telematics")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"TMC");
							else if (tc_strstr(PRM_EcuType,"Multiplexing")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"MPX");
							else if (tc_strstr(PRM_EcuType,"Hybrid ECU")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"HCU");
							else if (tc_strstr(PRM_EcuType,"Auto Transmission System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ATS");
							else if (tc_strstr(PRM_EcuType,"Electronically Controlled  Air Suspension")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ECAS");
							else if (tc_strstr(PRM_EcuType,"Emergency Braking System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"EBS");
							else if (tc_strstr(PRM_EcuType,"Exhaust Gas Recirculation")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"EGR");
							else if (tc_strstr(PRM_EcuType,"Selective Catalytic Reduction")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"SCR");
							else if (tc_strstr(PRM_EcuType,"Electronic Steering Column Lock")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ESCL");
							else if (tc_strstr(PRM_EcuType,"Unique Identification")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"UI");
							else if (tc_strstr(PRM_EcuType,"DC-DC Converter")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"DCC");
							else if (tc_strstr(PRM_EcuType,"Motor Controller Unit")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"MCU");
							else if (tc_strstr(PRM_EcuType,"Battery Management System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"BMS");
							else if (tc_strstr(PRM_EcuType,"Gear Shift Advisor")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"GSA");
							else if (tc_strstr(PRM_EcuType,"Advanced Driver Assistance System")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"ADAS");
							else if (tc_strstr(PRM_EcuType,"On Board Charger")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"OBC");
							else if (tc_strstr(Cnt_EcuType,"Wireless Power Charger")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"WPC");
							else if (tc_strstr(Cnt_EcuType,"Dosing Control Unit")!=NULL) 
							tc_strcpy(PRM_EcuTypeDup,"DCU");
							else 
							tc_strcpy(PRM_EcuTypeDup,"NA");
						}
						else 
						{
							tc_strcpy(PRM_EcuTypeDup,"NA");
						}

						if (tc_strstr(PRM_EcuTypeDup,"NA")==0)
							{		
								index =-1;							
								//int length		  =	tc_strlen(*ParaList);
								//printf("\n ParaList length  ------------- %d \n",length);fflush(stdout);
								for(m=0;m<iWriteRPar;m++)
								{							
									if(tc_strstr(ParaList[m],container)!=NULL)
									{
										index = m;
										printf("\n ParaList index value is  :-------------- %d \n",index);fflush(stdout);
										break;
									}
								}								
							}
							if (index>=0)
							{
								tc_strcpy(ContainerListVal,ConatainerList[index]);
								//ContainerListVal=low_set_get_str(ConatainerList,index);
							
								//tc_strcpy(ContainerRevValDuptk,ConatainerRev[index]);

								//ContainerRevValDup = tc_strtok (ContainerRevValDuptk,";");
								//tc_strcpy(ContainerRevVal,ConatainerRev[index]);
								//ifail =STRNG_replace_str (ContainerRevVal,strToBereplacedre, strToBeUsedInsteadre,&ContainerRevValDup);
								ifail =STRNG_replace_str (ContainerRevVal,strToBereplacedconcre, strToBeUsedInsteadconcre,&ContainerRevValDup);


								printf("\n..GRP ID 111111 Parameter matches with container is :%s having revision %s and ECUTYPE %s",ContainerListVal,ContainerRevVal,PRM_EcuTypeDup);fflush(stdout);
							}
							else
							{	
								
								printf("\n grp id INSIDE else of index>=0 m ======> [%d] ConatainerTypeListX [mm] ======> [%s] \n",m,ConatainerTypeListX[mm]);fflush(stdout);

								//int Conlength		  =	tc_strlen(*ConatainerTypeListX);
							   // printf("\n Conlength  ------------- %d \n",Conlength);fflush(stdout);

								//for(mm=0;mm<Conlength;mm++) 
								for(mm=0;mm<iWriteRConType;mm++) 
								{
									if(tc_strstr(ConatainerTypeListX[mm],PRM_EcuTypeDup)!=NULL)

									{
										index = mm;
										printf("\n ELSE index value is  :-------------- %d \n",index);fflush(stdout);
										break;
									}
								}

							if(index >= 0)
								{
									printf("\n grp id INSIDE index>=0 ContainerListVal is %s:::::: \n",ContainerListVal);fflush(stdout);
									
									tc_strcpy(ContainerListVal,ConatainerList[index]);
									//ContainerListVal=low_set_get_str(ConatainerList,index);

									tc_strcpy(ContainerRevVal,ConatainerRev[index]);
								
									//tc_strcpy(ContainerRevValDuptk,ConatainerRev[index]);

									//ContainerRevValDup = tc_strtok (ContainerRevValDuptk,";");
									//ifail =STRNG_replace_str (ContainerRevVal,strToBereplacedre, strToBeUsedInsteadre,&ContainerRevValDup);
									ifail =STRNG_replace_str (ContainerRevVal,strToBereplacedconcre, strToBeUsedInsteadconcre,&ContainerRevValDup);


									printf("\n..GRP ID 222222 Parameter matches with container is :%s having revision %s and ECUTYPE %s",ContainerListVal,ContainerRevVal,PRM_EcuTypeDup);fflush(stdout);
								}
								else
								{
									printf("\n..GRP ID No match for parameter .");fflush(stdout);
									tc_strcpy(ContainerListVal,NoValStr);
									tc_strcpy(ContainerRevValDup,NoValStr);;
								}
							}
						}
						else
						{
							//printf("\n..GRP ID No type for comparison .%s,%s",PRM_EcuTypeDup,container);fflush(stdout);

							tc_strcpy(ContainerListVal,NoValStr);
							tc_strcpy(ContainerRevValDup,NoValStr);
					
						}
						if(index>=0)
						{
						  ifail = GRM_find_relation_type("T5_EEPrm",&EErelation_type); 
			
						  if(EErelation_type != NULLTAG)
						    {
							  ifail = GRM_list_secondary_objects_only(ContChildRev,EErelation_type,&cntEE,&attachmentsEE);
							   if (cntEE > 0)
							    {						
								   for(ee=0;ee<cntEE;ee++)
								    {
									  ParaPtr = attachmentsEE[ee];

									 // if(ITEM_ask_item_of_rev  ( ParaPtr,&ParaMaster) );
									 ifail = AOM_ask_value_string(ParaPtr,"t5_ECUType",&paraecutype);
									  printf("\n GRP ID para master ecutype   is ---------- [%s]\n",paraecutype); fflush(stdout);

									  // FIND t5_BitValue which is on relationship class ???

									 ifail = AOM_ask_value_string(ParaPtr,"t5_t5EPDesc",&paradesc);
									  printf("\n GRP ID parameter desc master   is --------- [%s]\n",paradesc); fflush(stdout);

									  if((paradesc))
										{
											char	*strToBereplaced;
											char	*strToBereplaced1;
											char	*strToBereplaced2;
											char	*strToBeUsedInstead;
							
											strToBereplaced		=	(char*)malloc(5);
											tc_strcpy(strToBereplaced,",");
											strToBereplaced1	=	(char*)malloc(5);
											tc_strcpy(strToBereplaced1,"\n");
											strToBereplaced2	=	(char*)malloc(5);
											tc_strcpy(strToBereplaced2,"'");

											strToBeUsedInstead	=	(char*)malloc(5);
											tc_strcpy(strToBeUsedInstead," ");

											ifail =STRNG_replace_str (paradesc,strToBereplaced, strToBeUsedInstead,&cRplStr);
											ifail =STRNG_replace_str (paradesc,strToBereplaced1,strToBeUsedInstead,&cRplStr);
											ifail =STRNG_replace_str (paradesc,strToBereplaced2,strToBeUsedInstead,&cRplStr);
										}	
										else
										{
											  paradesc=(char *) MEM_alloc(50);
                                              tc_strcpy(paradesc,NoParaDes);
										}

										// code for t5_BitValue 

										 ifail = AOM_ask_value_string(ParaPtr,"t5_BitValue",&BitValue);
										 printf("\n GRP ID BitValue   is --------- [%s]\n",BitValue); fflush(stdout);

										 ifail =( AOM_ask_value_string(ParaPtr,"t5_EPApplicable",&ParaApp));
										 printf("\n GRP ID parameter applicable val   is --------- [%s]\n",ParaApp); fflush(stdout);

										 if((ParaApp))
										 {
											
											ParaAppdup_Val=(char *) MEM_alloc(100);
											if(tc_strstr(ParaApp,"BothES")!=NULL) 
											{					
												tc_strcpy(ParaAppdup_Val,ValBoth);
											}
											else if (tc_strstr(ParaApp,"NotApp")!=NULL)
											{					
												tc_strcpy(ParaAppdup_Val,ValBoth);
											}
											else if (tc_strstr(ParaApp,"Service")!=NULL)
											{					
												tc_strcpy(ParaAppdup_Val,ValSer);
											}
											else
											{
												tc_strcpy(ParaAppdup_Val,ParaApp);
											}
										}
										else
										{
											tc_strcpy(ParaApp,NoValStr);
										}

										ifail = AOM_ask_value_string(ParaPtr,"t5_EPReadable",&ParaRead);
										printf("\n GRP ID parameter READ val   is --------- [%s]\n",ParaRead); fflush(stdout);

										 if((ParaRead))
										 {
											char * ParaReaddup_Val =NULL;
											ParaReaddup_Val=(char *) MEM_alloc(100);
											if(tc_strstr(ParaApp,"BothRW")!=NULL) 
											{					
												tc_strcpy(ParaReaddup_Val,ValBoth);
											}
											else if (tc_strstr(ParaApp,"Write1")!=NULL)
											{					
												tc_strcpy(ParaReaddup_Val,WriteVal);
											}
											else if (tc_strstr(ParaApp,"Read Only")!=NULL)
											{					
												tc_strcpy(ParaReaddup_Val,ReadVal);
											}
											else
											{
												tc_strcpy(ParaReaddup_Val,EOLNA);
											}
										}
										else
										{
											tc_strcpy(ParaRead,NoValStr);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPUnit",&paraunit));
										printf("\n GRP ID paraunit val   is --------- [%s]\n",paraunit); fflush(stdout);

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPType",&paraeptype));
										printf("\n GRP ID paraeptype   is --------- [%s]\n",paraeptype); fflush(stdout);

										if((paraeptype))
										{
										    printf("\t paraeptype not null \n",paraeptype); fflush(stdout);
										}
										else
										{
											 tc_strcpy(paraeptype,NoValStr);
										}

										char	*strToBerepl;
										char	*strToBerepl1;
										char	*strToBerepl2;
										char	*strToBeUsed;
						
										strToBerepl		=	(char*)malloc(5);
										tc_strcpy(strToBerepl,",");
										strToBerepl1	=	(char*)malloc(5);
										tc_strcpy(strToBerepl1,"\n");
										strToBerepl2	=	(char*)malloc(5);
										tc_strcpy(strToBerepl2,"'");

										strToBeUsed	=	(char*)malloc(5);
										tc_strcpy(strToBeUsed," ");

										ifail =(STRNG_replace_str (paraeptype,strToBerepl, strToBeUsed,&paraeptypedup));
										ifail =(STRNG_replace_str (paraeptype,strToBerepl1,strToBeUsed,&paraeptypedup));
										ifail =(STRNG_replace_str (paraeptype,strToBerepl2,strToBeUsed,&paraeptypedup));

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPLen",&paralen));
										printf("\t paralen is --------- [%s]\n",paralen); fflush(stdout);

										if((paralen))
										{
										    printf("\t paralen not null \n",paralen); fflush(stdout);
										}
										else
										{
											 tc_strcpy(paralen,Noval);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPMin",&paramin));//VALUE WILL BE  0 
										printf("\t paramin is --------- [%s]\n",paramin); fflush(stdout);

										if((paramin))
										{
										    printf("\t GRP ID paramin not null \n",paramin); fflush(stdout);
										}
										else
										{
											 tc_strcpy(paramin,Noval);
										}

										ifail = AOM_ask_value_string(ParaPtr,"t5_EPMax",&paramax);//VALUE WILL BE  0 
										printf("\t paramax is --------- [%s]\n",paramax); fflush(stdout);

										if((paramax))
										{
										    printf("\t paramax not null \n",paramax); fflush(stdout);
										}
										else
										{
											 tc_strcpy(paramax,Noval);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPStep",&parastep));//?????check if attribute name is on master or rev
										printf("\t GRP ID parastep is --------- [%s]\n",paramax); fflush(stdout);

										char * parastepdup   =NULL;
										parastepdup=(char *) MEM_alloc(50);
										if((parastep))
										{
											tc_strcpy(parastepdup,parastep);
										}
										else
										{
											tc_strcpy(parastepdup,Noval);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPDidValue",&ParaDIDVal));//?????check if attribute name is on master or rev
										printf("\t GRP ID ParaDIDVal is --------- [%s]\n",ParaDIDVal); fflush(stdout);
										
										if((ParaDIDVal))
										{
										    printf("\n GRP ID ParaDIDVal not null [%s]\n",ParaDIDVal); fflush(stdout);
										}
										else
										{
											 tc_strcpy(ParaDIDVal,NoValStr);
											 printf("\n GRP ID ParaDIDVal value is [%s]\n",ParaDIDVal); fflush(stdout);
										}

										char * paradocdup   =NULL;
										paradocdup=(char *) MEM_alloc(50);

										if((paradoc))
										{
											tc_strcpy(paradocdup,paradoc);
											tc_strcat(paradocdup,";");
											tc_strcat(paradocdup,ParaDIDVal);
										}
										else
										{
											tc_strcpy(paradocdup,"NA");
											tc_strcat(paradocdup,";");
											tc_strcat(paradocdup,ParaDIDVal);
										}
										char	*repl;
										char	*repl1;
										char	*repl2;
										char	*ToBeUsed;

										repl		=	(char*)malloc(5);
										tc_strcpy(repl,",");
										repl1	=	(char*)malloc(5);
										tc_strcpy(repl1,"\n");
										repl2	=	(char*)malloc(5);
										tc_strcpy(repl2,"'");

										ToBeUsed	=	(char*)malloc(5);
										tc_strcpy(ToBeUsed," ");

										ifail =(STRNG_replace_str (paradocdup,repl, ToBeUsed,&paradocdupS));
										ifail =(STRNG_replace_str (paradocdup,repl1,ToBeUsed,&paradocdupS));
										ifail =(STRNG_replace_str (paradocdup,repl2,ToBeUsed,&paradocdupS));

										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPValid",&paravalid));//?????check if attribute name is on master or rev
										printf("\n GRP ID paravalid is --------- [%s]\n",paravalid); fflush(stdout);

										if((paravalid))
										{
										 if (tc_strcmp(paravalid,"T1") == 0)
											{
												ValFlag=1;
											}
											else
											{
												ValFlag=0;
											}
										}
										else
										{
											ValFlag=0;
										}

										ifail =( AOM_ask_value_string(ParaPtr,"t5_ParamValue",&DefValue));//?????check if attribute name is on master or rev
										printf("\t group id DefValue is --------- [%s]\n",DefValue); fflush(stdout);

										char * DefValuedup   =NULL;
										DefValuedup=(char *) MEM_alloc(100);

										char	*replx;
										char	*replx2;
										char	*ToBeUsedx;

										replx		=	(char*)malloc(5);
										tc_strcpy(replx,",");
										replx2	=	(char*)malloc(5);
										tc_strcpy(replx2,"'");

										ToBeUsedx	=	(char*)malloc(5);
										tc_strcpy(ToBeUsedx," ");

										if((DefValue))
										{
										  ifail =(STRNG_replace_str (DefValue,replx, ToBeUsedx,&DefValuedup));
										  ifail =(STRNG_replace_str (DefValue,replx2,ToBeUsedx,&DefValuedup));

										  DefValuedup = tc_strtok (DefValuedup,"\n");
										}
										else
										{
										  tc_strcpy(DefValuedup,Noval);
										}

										ifail =( AOM_ask_value_string(ParaPtr,"creation_date",&CreDate));//?????check if attribute name is on master or rev
										printf("\t grp id CreDate is --------- [%s]\n",CreDate); fflush(stdout);
										
										ifail =( AOM_ask_value_string(ParaPtr,"t5_EPValueList",&paravallist));//?????check if attribute name is on master or rev
										printf("\t grp id paravallist is --------- [%s]\n",paravallist); fflush(stdout);

										char * paravallistdup   =NULL;
										paravallistdup=(char *) MEM_alloc(50);

										if((paravallist)&& tc_strlen(paravallist)>0)
										{
										   tc_strcpy(paravallistdup,paravallist);
										   printf("\t grp id 11111 paravallist is --------- [%s]\n",paravallistdup); fflush(stdout);
										}
									    else
										{
										   tc_strcpy(paravallistdup,NoValStr);
										   printf("\t grp id 22222 paravallist is --------- [%s]\n",paravallistdup); fflush(stdout);
										}
										printf("\t GRP ID - fptr2222222222222222222 \n"); fflush(stdout);
										//fprintf(fptr2,"\n %s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",container,paradocdupS,cRplStr,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevVal,DML_name,PartNo,ParaAppdup_Val,ParaReaddup_Val);fflush(fptr2);
										//fprintf(fptr2,"\n %s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",container,paradocdupS,cRplStr,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevVal,DML_name,VCnumberDup,ParaAppdup_Val,ParaReaddup_Val);fflush(fptr2);
										//fprintf(fptr2,"\n %s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",container,paradocdupS,cRplStr,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevValDup,DML_name,req_item,ParaAppdup_Val,ParaReaddup_Val,cntEE);fflush(fptr2);
										//fprintf(fptr2,"\n %s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n",container,paradocdupS,cRplStr,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevValDup,DML_name,req_item,ParaAppdup_Val,ParaReaddup_Val,cntEE);fflush(fptr2);
										fprintf(fptr2,"\n %s,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%s,%s\n",container,paradocdupS,cRplStr,paraeptypedup,ValFlag,paramin,paramax,parastepdup,paralen,paravallistdup,paraunit,"NA",PRM_EcuTypeDup,DefValuedup,ContainerListVal,ContainerRevValDup,DML_name,VCnumberDup,ParaAppdup_Val,ParaReaddup_Val,cntEE,ParaDIDVal,containerrevdup);fflush(fptr2);


								    }
							    }
						    }
						}		
					}
			    }
			}		//}																			
		}
	}



/*static void ExpandMultiLevelBom (tag_t bom_line_tag, int depth, FILE *fptr)

{
	int ifail;
	int iChildItemTag;
	int status;
	char *Item_id_par		=NULL;
	char *Item_LatestRevSeq =NULL;
	char *ChildPart			=NULL;
	char *ChildPartType		=NULL;


	int i, n;

	tag_t *children;

	tag_t t_ChildItemRev = NULLTAG;
	int n_tags_found4 = 0;
	tag_t *tags_found4 = NULL;

	depth ++;


	if( AOM_ask_value_string(bom_line_tag,"bl_item_item_id",&Item_id_par)!=ITK_ok);
	printf("\t Item_id_par [%s]\n",Item_id_par); fflush(stdout);

	char **attrs5  = (char **) MEM_alloc(2 * sizeof(char *));
	char **values5 = (char **) MEM_alloc(2 * sizeof(char *));
	attrs5[0]	   ="item_id";
	attrs5[1]      ="object_type";
	values5[0]     = (char *)Item_id_par;
	values5[1]     = "Design";
	//ifail = ITEM_find_items_by_key_attributes(2,attrs, values, &n_tags_found, &tags_found);
	ITK_CALL(ITEM_find_item_revs_by_key_attributes(2,attrs5, values5,Item_LatestRevSeq, &n_tags_found4, &tags_found4));
	MEM_free(attrs5);
	MEM_free(values5);

	if (n_tags_found4>0)
	{
	   t_ChildItemRev= tags_found4[0];
	}


	if( AOM_ask_value_string(bom_line_tag,"bl_rev_item_revision_id",&Item_LatestRevSeq)!=ITK_ok);
	printf("\t Item_LatestRevSeq [%s]\n",Item_LatestRevSeq); fflush(stdout);

	if( AOM_ask_value_string(t_ChildItemRev,"bl_item_item_id",&ChildPart)!=ITK_ok);
	printf("\t ChildPart [%s]\n",ChildPart); fflush(stdout);

	if( AOM_ask_value_string(t_ChildItemRev,"t5_SwPartType",&ChildPartType)!=ITK_ok);
	printf("\t ChildPartType [%s]\n",ChildPartType); fflush(stdout);

	if (tc_strcmp(ChildPartType, "HWC") == 0)
	{
		tc_strcpy(HWCList,ChildPart);
	}
	else if (tc_strcmp(ChildPartType, "CAL") == 0 || tc_strcmp(ChildPartType, "HCL") == 0)
	{
		tc_strcpy(CALList,ChildPart);
	}
	else if (tc_strcmp(ChildPartType, "PBL") == 0)
	{
		tc_strcpy(PBLList,ChildPart);
	}
	else if (tc_strcmp(ChildPartType, "SBL") == 0)
	{
		tc_strcpy(SBLList,ChildPart);
	}
	else if (tc_strcmp(ChildPartType, "BAS") == 0)
	{
		tc_strcpy(BASList,ChildPart);
	}
	else if (tc_strcmp(ChildPartType, "APP") == 0)
	{
		tc_strcpy(APPList,ChildPart);
	}
	else if (tc_strcmp(ChildPartType, "STK") == 0)
	{
		tc_strcpy(STKList,ChildPart);
	}
	else if (tc_strcmp(ChildPartType, "VCI") == 0 || tc_strcmp(ChildPartType, "HCL") == 0)
	{
		tc_strcpy(VCIList,ChildPart);
	}
	else if (tc_strcmp(ChildPartType, "CFG") == 0)
	{
		tc_strcpy(CFGList,ChildPart);
	}


	printf("%s^%s^%s ^%s ^%s ^%s ^%s ^%s ^%s ^%s ^%s ^%s   \n",ChildPart,ChildPartType,HWCList,CALList,PBLList,SBLList,BASList,APPList,STKList,VCIList,CFGList); fflush(stdout);
	fprintf(fptr,"%s^%s ^%s ^%s ^%s ^%s ^%s ^%s ^%s ^%s ^%s ^%s \n",ChildPart,ChildPartType, HWCList,CALList,PBLList,SBLList,BASList,APPList,STKList,VCIList,CFGList); fflush(fptr);


		ifail = BOM_line_ask_child_lines (bom_line_tag, &n, &children);
	CHECK_FAIL;
	for (i = 0; i < n; i++)
	{
		ExpandMultiLevelBom (children[i], depth,fptr);
	}

	return status;
}*/

static void initialise (void)
{
	int ifail;

	/* <kc> pr#397778 July2595 exit if autologin() fail */
	if ((ifail = ITK_auto_login()) != ITK_ok)
	   printf("Login fail !!: Error code = %d \n\n",ifail); fflush(stdout);
	CHECK_FAIL;

	/* these tokens come from bom_attr.h */
	initialise_attribute (bomAttr_lineName, &name_attribute);
	initialise_attribute (bomAttr_occSeqNo, &seqno_attribute);
	ifail = BOM_line_look_up_attribute (bomAttr_lineParentTag, &parent_attribute);
	CHECK_FAIL;
	ifail = BOM_line_look_up_attribute (bomAttr_lineItemTag, &item_tag_attribute);
	CHECK_FAIL;
}
static void initialise_attribute (char *name,  int *attribute)
{
	int ifail, mode;

	ifail = BOM_line_look_up_attribute (name, attribute);
	CHECK_FAIL;
	ifail = BOM_line_ask_attribute_mode (*attribute, &mode);
	CHECK_FAIL;
	if (mode != BOM_attribute_mode_string)
	{
		printf ("Help,  attribute %s has mode %d,  I want a string\n", name, mode); fflush(stdout);
		exit(0);
	}
}







// 54248824R_VCList.csv
// 54248824R_VC.csv
// 54248824R_SuppMstr.csv
// 54248824R_Summary.csv
// 54248824R_Struct.csv
// 54248824R_Files.csv
// 54248824R_Para.csv3



///t5ConHasParam - container has parameter

//t5HasSlv - has slave