#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <pom/pom/pom.h>
#define Debug TRUE
#define ITK_CALL(X) 						\
	if(Debug)								\
	{										\
		printf(#X);							\
	}										\
	fflush(NULL);							\
	status=X; 							    \ 
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

#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}
#define MAX_LEN 100
static int name_attribute, seqno_attribute, parent_attribute, item_tag_attribute;
static void initialise (void);
static void initialise_attribute (char *name,  int *attribute);
tag_t FunToReturnLatestRevision(char *Item_ID);
static void ExpandMultiLevelBom (tag_t bom_line_tag, int depth,FILE *fptr,FILE *fptrP,FILE *fptrC,FILE *fptrA,char* sbl_item_id,char* sbl_rev_id,char* sParentCompCode,char* sParentColID);

tag_t	LatestRev = NULLTAG;
tag_t	NewClr = NULLTAG;
tag_t 	class_to_load_as;
int 	lock_type = POM_no_lock;
// Defining comparator function as per the requirement 
static int myCompare(const void* a, const void* b) 
{ 
  
    // setting up rules for comparison 
    return strcmp(*(const char**)a, *(const char**)b); 
} 
  
// Function to sort the array 
void sort(const char* arr[], int n) 
{ 
    // calling qsort function to sort the array 
    // with the help of Comparator 
    qsort(arr, n, sizeof(const char*), myCompare); 
} 
//char* subString (char* mainStringf, int fromCharf, int toCharf)
//{
//	int i;
//	char *retStringf;
//	retStringf = (char*) malloc(3);
//	for(i=0; i < toCharf; i++ )
//              *(retStringf+i) = *(mainStringf+i+fromCharf);
//	*(retStringf+i) = '\0';
//	return retStringf;
//}
void selectionSort(char name[][MAX_LEN], int n) 
{    
	int i=0,j=0;
	char* temp=NULL;
	temp=(char *) MEM_alloc(100);

	printf("\n in selectionSort %d \n",n);fflush(stdout);
	
	for (i = 0; i < n - 1 ; i++)
	{
	for (j = i + 1; j < n; j++)
	{
	if (strcmp(name[i], name[j]) > 0) 
	{
	strcpy(temp, name[i]);
	strcpy(name[i], name[j]);
	strcpy(name[j], temp);
	}
	}
	}

	 for (i = 0; i < n; i++) 
     {
         printf("\t%s\n", name[i]);
     }


	printf("\n end selectionSort \n");fflush(stdout);
} 
char *replaceWord(const char *s, const char *oldW, const char *newW) 
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
// fun To Find String is present in array.
void setFindStr(char **str_list,int count,char *str,int *found)
{

	int k=0;
	*found=0;
	for(k=0;k<count;k++)
	{

		//printf("\n[%d]tc_strcmp(%s,%s)",k,str_list[k],str);fflush(stdout);
		if(tc_strcmp(str_list[k],str)==0)
		{
			*found=1;
			printf("\n str found in str_list \n");
			break;
		}

	}
}
// fun to find array values contains strstr some string 
void setGetStrStr(char **str_list,int count,char *str,char *str1,int *found)
{

	int k=0;
	*found=0;
	for(k=0;k<count;k++)
	{

		//printf("\n[%d]tc_strcmp(%s,%s)",k,str_list[k],str);fflush(stdout);
		if((tc_strstr(str_list[k],str)!=NULL) && (tc_strstr(str_list[k],str1)!=NULL))
		{
			*found=1;
			break;
		}

	}
}
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
	// printf("\n setAddStr===%s",(*strset)[*count-1]);fflush(stdout);

}
void setAddStr1(int *count,char ***strset,char* str)
{

	*count=*count+1;
	printf("\n setAddStr1 %d",*count);fflush(stdout);
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
	 printf("\n setAddStr1===%s",(*strset)[*count-1]);fflush(stdout);

}
 int returnmin(int a[],int n)
 {
 	int min,max,i;
 	min=max=a[0];
    for(i=1; i<n; i++)
    {
         if(min>a[i])
		  min=a[i];   
		   if(max<a[i])
		    max=a[i];       
    }
    
    printf("minimum of array is : %d",min);
    printf("\nmaximum of array is : %d",max);
	return min;
 }
extern int ITK_user_main (int argc, char ** argv )
{
	int ifail;
	int status;
	char *inputfile = NULL;
	char *itemId = NULL;
	char *nwitemId = NULL;
	char *outputfile=NULL;
	char *Parentfile=NULL;
	char *Childfile=NULL;
	char *AllFiles=NULL;
	char *input=NULL;
	char *sParentCompCode=NULL;
	char *sParentColID=NULL;
	char *sbl_item_id=NULL;  
	char *sbl_rev_id=NULL;  
	char *sNewColorSrl=NULL;  
	//tag_t *LatestRev = NULL;
	tag_t SchmHasClrData_RelTag = NULLTAG;
	tag_t window, rule, item_tag = null_tag, top_line;
	FILE *fptr;
	FILE *fptr1;
	FILE *fptrP;
	FILE *fptrC;
	FILE *fptrA;
	char* line;
	char *sitem_id =NULL;
	char *sSVRNo   =NULL;
	char *sPlatform   =NULL;
	char *sDtCmpCd   =NULL;
	char *sDtIntScm   =NULL;
	//char *sDtCmpCd   =NULL;
	char *sDtClrSrl   =NULL;

	int DOstatus =0;
	int	DelStatus	=0;

	char *sPrtCompCode		= NULL;
	char *sClrSerial		= NULL;
	char *sDataPartNumber	= NULL;
	char *sInternalScheme	= NULL;
	tag_t LatestRev_tag		= NULLTAG;

	printf("find values of color scheme data.\n");fflush(stdout);
	int Rstatus =0;
	char	*sFileName				= NULL;

	char	*sCompCodeF				= NULL;
	char	*sInteSch				= NULL;
	char	*sCategory				= NULL;
	char	*sPartNum				= NULL;
	FILE* fp = NULL;
	char	temp[200];
	sFileName=(char *) MEM_alloc(200);

	(void)argc;
	(void)argv;

	initialise();

	//req_item = ITK_ask_cli_argument("-i=");

	itemId = ITK_ask_cli_argument("-old=");
	nwitemId = ITK_ask_cli_argument("-nw=");
	

	if(itemId== NULL)
	{
		printf("\n Please use exe as RefreshColorScheme -i=$ColorSchemeNumber");fflush(stdout);
		(POM_logout(false));
		return status;
	}

	if( ITK_init_module("ercpsup" ,"XYT1ESA","dba")!=ITK_ok) ;
	
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	//ITK_CALL(ITK_auto_login( ));
	ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n login .......");fflush(stdout);

	if ( itemId )
	{
		LatestRev = FunToReturnLatestRevision(itemId);
		NewClr = FunToReturnLatestRevision(nwitemId);
		
		ITK_CALL(AOM_ask_value_string(LatestRev,"item_id",&sitem_id));
		ITK_CALL(AOM_ask_value_string(NewClr,"t5_VehNO",&sSVRNo));
		ITK_CALL(AOM_ask_value_string(NewClr,"t5_SchmPlant",&sPlatform));

		ITK_CALL(AOM_ask_value_string(NewClr,"t5_ClSrl",&sNewColorSrl));
	
		//printf("\nColor scheme Name: [%s] SVR number: [%s]\n",sitem_id,sSVRNo);fflush(stdout);
	}
	else
	{
		printf ("Pls enter input part ?? \n"); fflush(stdout);
		exit (0);
	}
	
	printf ("file name \n"); fflush(stdout);

	sFileName	=	tc_strcpy(sFileName,"");
	sFileName	=	tc_strcpy(sFileName,"/tmp/");
	sFileName	=	tc_strcat(sFileName,sPlatform);
	sFileName	=	tc_strcat(sFileName,"_");
	sFileName	=	tc_strcat(sFileName,sSVRNo);
	sFileName	=	tc_strcat(sFileName,"_");
	sFileName	=	tc_strcat(sFileName,"Copy");
	sFileName	=	tc_strcat(sFileName,"_");
	sFileName	=	tc_strcat(sFileName,"CompCode.txt");

	printf("\n Input sFileName:[%s] \n",sFileName);

	fp = fopen(sFileName, "r");
	

	if (fp != NULL)
	{
		
		while (fgets(temp, 400, fp)!= NULL)
		{
			tc_strcpy(temp,stripBlanks(temp));
			

			if (tc_strlen(temp) > 0 && tc_strcmp(temp, NULL) != 0)
			{
				sCompCodeF = strtok(temp, "^");//CCA_AIR_VENT
				sInteSch = strtok(NULL, "^");//N Y T
				
				if(strlen(sCompCodeF)>0)
				{
				printf("\n sCompCodeF[%s] \n",sCompCodeF);
				if(tc_strcmp(sInteSch,"N")==0)
				{
				NinternalScheme(NewClr,sCompCodeF,sInteSch,sNewColorSrl);	
				}
				else if(tc_strcmp(sInteSch,"Y")==0)
				{
				YinternalScheme(NewClr,sCompCodeF,sInteSch,sNewColorSrl);
				}
				else if(tc_strcmp(sInteSch,"T")==0)
				{
				FunToFindDummyClrSrlNCreRel(NewClr,sCompCodeF);
				}
				else
				{
				printf("\n in else ");					
				}
					
				}
			 }
			}
		}
		else
		{
			printf("\n file not found\n");
		}
		fclose(fp);
		
//	}
//	
//	if(SchemeDataCnt>0)
//	{
//	for(n=0;n<SchemeDataCnt;n++)
//	{
//	ITK_CALL(AOM_ask_value_string(ClrSchemeData_tag[n],"t5_PrtCatCode",&sDtCmpCd));
//	setFindStr(FileCompCodeSet,IFlCmp,sDtCmpCd,&DelFlag);// if DelFlag=0,not found;if DelFlag=1, found
//	printf("\t\n DelFlag: [%d] sDtCmpCd %s \n",DelFlag,sDtCmpCd);
//	if(DelFlag==0)
//	{
//	setAddStr(&InDel,&DelCompCodeSet,sDtCmpCd);// create array of child color suffix.
//	sort(DelCompCodeSet,InDel); // sort
//	// create relation of comp code data with color scheme				
//	}
//	}
//	}
//	printf("\t\n InNw: [%d] InDel: [%d]\n",InNw,InDel);
//
//	for (p=0;p<InNw;p++)
//	{
//	printf("\n new comp codes to be add NwCompCodeSet:  %s\n",NwCompCodeSet[p]);
//	}
//	for (q=0;q<InDel;q++)
//	{
//	printf("\n comp codes to be deleted DelCompCodeSet:  %s\n",DelCompCodeSet[q]);
//	}
//
//	for (k=0;k<InNw;k++ )
//	{
//		//// create relation of comp code data with color scheme
//		printf("\t\n create relation of comp code data with color scheme for  NwCompCodeSet[k]: [%s]\n",NwCompCodeSet[k]);
//		FunToFindDummyClrSrlNCreRel(LatestRev,NwCompCodeSet[k]);	
//	}
//
//	for (m=0;m<InDel;m++ )
//	{
//		//// delete = relation of comp code data with color scheme
//		printf("\t\n delete = relation of comp code data with color scheme DelCompCodeSet[m] : [%s]\n",DelCompCodeSet[m]);
//		FunToFindDummyClrSrlNDelRel(LatestRev,DelCompCodeSet[m]);	
//	}
	
	ITK_exit_module(true);	
	
	return status;
}
int YinternalScheme(tag_t NwClrSchmTag,char* YsDtCmpCd,char* YsDtIntScm,char* YsDtClrSrl)
{
	int		status						=	0;	

	int		n_entriesF					=	3;
	tag_t	queryTagF					=	NULLTAG;
	char*	clrsrlmain1					=	NULL;
	int		resultCountF				=	0;
	tag_t	*qry_outputF				=	NULLTAG;
	char *item_id_ColDatamain			=	NULL;
	char* dupintrnalscheme				=	NULL;
	char* sDtClrSrl						=	NULL;
	char* sDtIntScm					=	NULL;
	char* sDtCmpCd					=	NULL;
	tag_t SchmHasClrData_RelTag			=	NULLTAG;
	tag_t del_relation					=	NULLTAG;
	tag_t new_relation					=	NULLTAG;
	tag_t Colordata_type_tag			=	NULLTAG;
	int i = 0,SchemeDataCnt=0;

	tag_t object_create_input_tag1		=	NULLTAG;
	//tag_t Colordata_type_tag			=	NULLTAG;
	tag_t new_COLDATA_object			=	NULLTAG;	
	tag_t clrdatarevtag					=	NULLTAG;	
	tag_t clrdatarevtag1				=	NULLTAG;	
	tag_t* ClrSchemeData_tag			=	NULLTAG;
	//tag_t del_relation					=	NULLTAG;
	//tag_t new_relation					=	NULLTAG;

	
//	item_id_ColDatamain=(char *)MEM_alloc(100);
//
//	printf(" \n Inside YinternalScheme \n\t"); fflush(stdout);
//
//	strcpy(item_id_ColDatamain,"");
//	strcat(item_id_ColDatamain,YsDtCmpCd);
//	strcat(item_id_ColDatamain,"`");
//	strcat(item_id_ColDatamain,YsDtIntScm);
//	strcat(item_id_ColDatamain,"`");
//	strcat(item_id_ColDatamain,YsDtClrSrl);

	(GRM_find_relation_type	("T5_ShmHasClrData",&SchmHasClrData_RelTag));
		
	(GRM_list_secondary_objects_only(LatestRev,SchmHasClrData_RelTag,&SchemeDataCnt,&ClrSchemeData_tag));
		
		printf("\n Color scheme data count SchemeDataCnt: %d\n",SchemeDataCnt);

	if(SchemeDataCnt>0)
	{
	for(i=0;i<SchemeDataCnt;i++)
	{
	(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_PrtCatCode",&sDtCmpCd));
	(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_InternalSchm",&sDtIntScm));
	(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_ClSrl",&sDtClrSrl));

	if((tc_strcmp(sDtIntScm,"Y")==0) && (tc_strcmp(YsDtCmpCd,sDtCmpCd)==0))
	{

	ITK_CALL(GRM_create_relation(NwClrSchmTag,ClrSchemeData_tag[i],SchmHasClrData_RelTag,NULLTAG,&new_relation));
	if(new_relation != NULLTAG)
	{
	ITK_CALL(GRM_save_relation(new_relation));
	}
	}
	}
	ITK_CALL(AOM_refresh(NwClrSchmTag,0));
	ITK_CALL(AOM_lock(NwClrSchmTag));
	ITK_CALL(AOM_save(NwClrSchmTag));
	//ITK_CALL(AOM_unlock(LatestRev));
	ITK_CALL(AOM_refresh(NwClrSchmTag,1));
	ITK_CALL(AOM_load(NwClrSchmTag));

	ITK_CALL(POM_refresh_instances(1,&NwClrSchmTag,class_to_load_as,lock_type));

	}
	
	

	printf(" \n closing YinternalScheme \n\t"); fflush(stdout);
	return status;
}
int NinternalScheme(tag_t NwClrSchmTag,char* YsDtCmpCd,char* YsDtIntScm,char* YsDtClrSrl)
{
	int		status						=	0;	

	int		n_entriesF					=	3;
	tag_t	queryTagF					=	NULLTAG;
	char*	clrsrlmain1					=	NULL;
	int		resultCountF				=	0;
	tag_t	*qry_outputF				=	NULLTAG;
	char *item_id_ColDatamain			=	NULL;
	char* dupintrnalscheme				=	NULL;
	char* ClrsrlCat						=	NULL;
	char* sObjectNameDt					=	NULL;
	char* clrsrlmain2					=	NULL;
	tag_t SchmHasClrData_RelTag			=	NULLTAG;
	tag_t del_relation					=	NULLTAG;
	tag_t new_relation					=	NULLTAG;
	tag_t Colordata_type_tag			=	NULLTAG;
	int m = 0;

	tag_t object_create_input_tag1		=	NULLTAG;
	//tag_t Colordata_type_tag			=	NULLTAG;
	tag_t new_COLDATA_object			=	NULLTAG;	
	tag_t clrdatarevtag					=	NULLTAG;	
	tag_t clrdatarevtag1				=	NULLTAG;	
	tag_t ColData_relation_type			=	NULLTAG;
	//tag_t del_relation					=	NULLTAG;
	//tag_t new_relation					=	NULLTAG;

	
	item_id_ColDatamain=(char *)MEM_alloc(100);

	printf(" \n Inside NinternalScheme \n\t"); fflush(stdout);

	strcpy(item_id_ColDatamain,"");
	strcat(item_id_ColDatamain,YsDtCmpCd);
	strcat(item_id_ColDatamain,"`");
	strcat(item_id_ColDatamain,YsDtIntScm);
	strcat(item_id_ColDatamain,"`");
	strcat(item_id_ColDatamain,YsDtClrSrl);

	ITK_CALL(GRM_find_relation_type	("T5_ShmHasClrData",&SchmHasClrData_RelTag));
		
	//	 Find ##;DUMMY_COLSRL comp code
		
	if(QRY_find("ColorSchemeDataRevision", &queryTagF));
	if (queryTagF)
	{
	printf("Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
	}
	else
	{
	printf("Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
	return status;
	}
	char *qry_entriesF[3] = {"Comp Code","Internal Scheme","Color Serial"};
	char **qry_valuesF = (char **) MEM_alloc(50 * sizeof(char *));

	

	qry_valuesF[0] = YsDtCmpCd;
	qry_valuesF[1] = YsDtIntScm;
	qry_valuesF[2] = YsDtClrSrl;

	(QRY_execute(queryTagF, n_entriesF, qry_entriesF, qry_valuesF, &resultCountF, &qry_outputF));
	printf("\n Count of Query Values resultCountF ------------------>  %d \n",resultCountF);
	if(resultCountF>0)
	{
		for (m=0;m<resultCountF ;m++ )
		{
			(AOM_UIF_ask_value(qry_outputF[m],"object_name",&sObjectNameDt));	
			
			if(tc_strcmp(item_id_ColDatamain,sObjectNameDt)==0)
			{
			ITK_CALL(GRM_find_relation(NwClrSchmTag,qry_outputF[m],SchmHasClrData_RelTag,&del_relation));
			if(del_relation==NULLTAG)
			{
			printf(" \n Inside FunToFindDummyClrSrlNCreRel: found relation, now deleting it\n\t"); fflush(stdout);
			//ITK_CALL(GRM_delete_relation(del_relation));
			ITK_CALL(GRM_create_relation(NwClrSchmTag,qry_outputF[m],SchmHasClrData_RelTag,NULLTAG,&new_relation));
			if(new_relation != NULLTAG)
			{
			ITK_CALL(GRM_save_relation(new_relation));
			}
			}
			}
			ITK_CALL(AOM_refresh(NwClrSchmTag,0));
			ITK_CALL(AOM_lock(NwClrSchmTag));
			ITK_CALL(AOM_save(NwClrSchmTag));
			//ITK_CALL(AOM_unlock(LatestRev));
			ITK_CALL(AOM_refresh(NwClrSchmTag,1));
			ITK_CALL(AOM_load(NwClrSchmTag));
			
			ITK_CALL(POM_refresh_instances(1,&NwClrSchmTag,class_to_load_as,lock_type));
			//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

		}
	}
	else
	{
		// create new color scheme data and create relation

		strcpy(item_id_ColDatamain,"");
		strcat(item_id_ColDatamain,YsDtCmpCd);
		strcat(item_id_ColDatamain,"`");
		strcat(item_id_ColDatamain,YsDtIntScm);
		strcat(item_id_ColDatamain,"`");
		strcat(item_id_ColDatamain,YsDtClrSrl);
		
		ITK_CALL(TCTYPE_find_type("T5_ClrShmData", NULL, &Colordata_type_tag));   

		ITK_CALL(TCTYPE_construct_create_input(Colordata_type_tag, &object_create_input_tag1));

		ITK_CALL(AOM_set_value_string(object_create_input_tag1,"object_name",item_id_ColDatamain));
		ITK_CALL(AOM_set_value_string(object_create_input_tag1,"item_id",item_id_ColDatamain));

		ITK_CALL(TCTYPE_create_object(object_create_input_tag1, &new_COLDATA_object));

		if(new_COLDATA_object)
		{
		printf("\n t5CreateClrschm ---------------------------> new_COLDATA_object created.");
		ITK_CALL(AOM_save(new_COLDATA_object));
		//ITK_CALL(AOM_unlock(new_COLDATA_object));
		}
		ITK_CALL( ITEM_ask_latest_rev (new_COLDATA_object,&clrdatarevtag) );
		if(clrdatarevtag)
		{
		ITK_CALL(AOM_refresh(clrdatarevtag,1));
		ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_PrtCatCode",YsDtCmpCd));
		ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_InternalSchm",YsDtIntScm));
		ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_ClSrl",YsDtClrSrl));
		ITK_CALL(AOM_save(clrdatarevtag));
		ITK_CALL(AOM_refresh(clrdatarevtag,0));
		}
		ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
		if(ColData_relation_type != NULLTAG)
		{
		ITK_CALL(GRM_create_relation(NwClrSchmTag,clrdatarevtag,ColData_relation_type,NULLTAG,&new_relation));
		if(new_relation != NULLTAG)
		{
		ITK_CALL(GRM_save_relation(new_relation));
		}
		}

		ITK_CALL(AOM_refresh(NwClrSchmTag,0));
		ITK_CALL(AOM_lock(NwClrSchmTag));
		ITK_CALL(AOM_save(NwClrSchmTag));
		//ITK_CALL(AOM_unlock(LatestRev));
		ITK_CALL(AOM_refresh(NwClrSchmTag,1));
		ITK_CALL(AOM_load(NwClrSchmTag));

		ITK_CALL(POM_refresh_instances(1,&NwClrSchmTag,class_to_load_as,lock_type));
	}


	printf(" \n closing YinternalScheme \n\t"); fflush(stdout);
	return status;
}
int FunToFindDummyClrSrlNCreRel(tag_t ClrSchmTag,char* sCompCodeFT)
{
	int		status						=	0;	
	int		n_entriesF					=	3;
	tag_t	queryTagF					=	NULLTAG;
	char*	clrsrlmain1					=	NULL;
	int		resultCountF				=	0;
	tag_t	*qry_outputF				=	NULLTAG;
	char *item_id_ColDatamain			=	NULL;
	char* dupintrnalscheme				=	NULL;
	char* ClrsrlCat						=	NULL;
	char* sObjectNameDt					=	NULL;
	char* clrsrlmain2					=	NULL;
	tag_t SchmHasClrData_RelTag			=	NULLTAG;
	tag_t del_relation					=	NULLTAG;
	tag_t new_relation					=	NULLTAG;
	tag_t Colordata_type_tag			=	NULLTAG;
	int m = 0;

	tag_t object_create_input_tag1		=	NULLTAG;
	//tag_t Colordata_type_tag			=	NULLTAG;
	tag_t new_COLDATA_object			=	NULLTAG;	
	tag_t clrdatarevtag					=	NULLTAG;	
	tag_t clrdatarevtag1				=	NULLTAG;	
	tag_t ColData_relation_type			=	NULLTAG;
	//tag_t del_relation					=	NULLTAG;
	//tag_t new_relation					=	NULLTAG;

	clrsrlmain2=(char *)MEM_alloc(100);
	item_id_ColDatamain=(char *)MEM_alloc(100);

	printf(" \n Inside FunToFindDummyClrSrlNCreRel \n\t"); fflush(stdout);

	ITK_CALL(GRM_find_relation_type	("T5_ShmHasClrData",&SchmHasClrData_RelTag));
		
	//	 Find ##;DUMMY_COLSRL comp code
		
	if(QRY_find("ColorSchemeDataRevision", &queryTagF));
	if (queryTagF)
	{
	printf("Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
	}
	else
	{
	printf("Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
	return status;
	}
	char *qry_entriesF[3] = {"Comp Code","Internal Scheme","Color Serial"};
	char **qry_valuesF = (char **) MEM_alloc(50 * sizeof(char *));

	tc_strcpy(clrsrlmain2,"");
	tc_strcat(clrsrlmain2,"##");
	tc_strcat(clrsrlmain2,"*");
	tc_strcat(clrsrlmain2,"DUMMY_COLSRL");
	printf("clrsrlmain2 --------> %s\n",clrsrlmain2);

	qry_valuesF[0] = sCompCodeFT;
	qry_valuesF[1] = "T";
	qry_valuesF[2] = clrsrlmain2;

	ITK_CALL(QRY_execute(queryTagF, n_entriesF, qry_entriesF, qry_valuesF, &resultCountF, &qry_outputF));
	printf("\n Count of Query Values resultCountF ------------------>  %d \n",resultCountF);
	if(resultCountF>0)
	{
		for (m=0;m<resultCountF ;m++ )
		{
			ITK_CALL(AOM_UIF_ask_value(qry_outputF[m],"object_name",&sObjectNameDt));			
		
			ITK_CALL(GRM_find_relation(ClrSchmTag,qry_outputF[m],SchmHasClrData_RelTag,&del_relation));
			if(del_relation==NULLTAG)
			{
			printf(" \n Inside FunToFindDummyClrSrlNCreRel: found relation, now deleting it\n\t"); fflush(stdout);
			//ITK_CALL(GRM_delete_relation(del_relation));
			ITK_CALL(GRM_create_relation(ClrSchmTag,qry_outputF[m],SchmHasClrData_RelTag,NULLTAG,&new_relation));
			if(new_relation != NULLTAG)
			{
			ITK_CALL(GRM_save_relation(new_relation));
			}
			}
			ITK_CALL(AOM_refresh(ClrSchmTag,0));
			ITK_CALL(AOM_lock(ClrSchmTag));
			ITK_CALL(AOM_save(ClrSchmTag));
			//ITK_CALL(AOM_unlock(ClrSchmTag));
			ITK_CALL(AOM_refresh(ClrSchmTag,1));
			ITK_CALL(AOM_load(ClrSchmTag));
			
			ITK_CALL(POM_refresh_instances(1,&ClrSchmTag,class_to_load_as,lock_type));
			//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

		}
	}
	else
	{
		// create new color scheme data and create relation

		strcpy(item_id_ColDatamain,"");
		strcat(item_id_ColDatamain,sCompCodeFT);
		strcat(item_id_ColDatamain,"`");
		strcat(item_id_ColDatamain,"T");
		strcat(item_id_ColDatamain,"`");
		strcat(item_id_ColDatamain,"##;DUMMY_COLSRL");
		
		ITK_CALL(TCTYPE_find_type("T5_ClrShmData", NULL, &Colordata_type_tag));   

		ITK_CALL(TCTYPE_construct_create_input(Colordata_type_tag, &object_create_input_tag1));

		ITK_CALL(AOM_set_value_string(object_create_input_tag1,"object_name",item_id_ColDatamain));
		ITK_CALL(AOM_set_value_string(object_create_input_tag1,"item_id",item_id_ColDatamain));

		ITK_CALL(TCTYPE_create_object(object_create_input_tag1, &new_COLDATA_object));

		if(new_COLDATA_object)
		{
		printf("\n t5CreateClrschm ---------------------------> new_COLDATA_object created.");
		ITK_CALL(AOM_save(new_COLDATA_object));
		//ITK_CALL(AOM_unlock(new_COLDATA_object));
		}
		ITK_CALL( ITEM_ask_latest_rev (new_COLDATA_object,&clrdatarevtag) );
		if(clrdatarevtag)
		{
		ITK_CALL(AOM_refresh(clrdatarevtag,1));
		ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_PrtCatCode",sCompCodeFT));
		ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_InternalSchm","T"));
		ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_ClSrl","##;DUMMY_COLSRL"));
		ITK_CALL(AOM_save(clrdatarevtag));
		ITK_CALL(AOM_refresh(clrdatarevtag,0));
		}
		ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
		if(ColData_relation_type != NULLTAG)
		{
		ITK_CALL(GRM_create_relation(LatestRev,clrdatarevtag,ColData_relation_type,NULLTAG,&new_relation));
		if(new_relation != NULLTAG)
		{
		ITK_CALL(GRM_save_relation(new_relation));
		}
		}
		ITK_CALL(AOM_refresh(ClrSchmTag,0));
		ITK_CALL(AOM_lock(ClrSchmTag));
		ITK_CALL(AOM_save(ClrSchmTag));
		//ITK_CALL(AOM_unlock(ClrSchmTag));
		ITK_CALL(AOM_refresh(ClrSchmTag,1));
		ITK_CALL(AOM_load(ClrSchmTag));

		ITK_CALL(POM_refresh_instances(1,&ClrSchmTag,class_to_load_as,lock_type));
	}
	printf(" \n closing FunToFindDummyClrSrlNCreRel \n\t"); fflush(stdout);
	return status;
}
int FunToFindDummyClrSrlNDelRel(tag_t ClrSchmTag, char* PrtCmpCodeDel)
{
	int		status						=	0;	
	int		n_entriesF					=	0;
	tag_t	queryTagF					=	NULLTAG;
	char*	clrsrlmain1					=	NULL;
	int		resultCountF				=	0;
	tag_t	*ClrSchemeData_tag			=	NULLTAG;
	char *item_id_ColDatamain			=	NULL;
	char* dupintrnalscheme				=	NULL;
	char* ClrsrlCat						=	NULL;
	char* sObjectNameDt					=	NULL;
	char* sDtCmpCd					=	NULL;
	tag_t SchmHasClrData_RelTag			=	NULLTAG;
	tag_t del_relation					=	NULLTAG;
	int m = 0;

	//clrsrlmain2=(char *)MEM_alloc(100);

	printf(" \n Inside FunToFindDummyClrSrlNDelRel \n\t"); fflush(stdout);

	
	ITK_CALL(GRM_find_relation_type	("T5_ShmHasClrData",&SchmHasClrData_RelTag));
	
		
	ITK_CALL(GRM_list_secondary_objects_only(LatestRev,SchmHasClrData_RelTag,&n_entriesF,&ClrSchemeData_tag));

	printf("\n Color scheme data count n_entriesF: %d\n",n_entriesF);

	if(n_entriesF>0)
	{
	for(m=0;m<n_entriesF;m++)
	{
	(AOM_ask_value_string(ClrSchemeData_tag[m],"t5_PrtCatCode",&sDtCmpCd));
	
	if(strcmp(PrtCmpCodeDel,sDtCmpCd)==0)
	{
		ITK_CALL(GRM_find_relation(ClrSchmTag,ClrSchemeData_tag[m],SchmHasClrData_RelTag,&del_relation));
		if(del_relation!=NULLTAG)
		{
		printf(" \n Inside FunToFindDummyClrSrlNDelRel: found relation, now deleting it\n\t"); fflush(stdout);
		ITK_CALL(GRM_delete_relation(del_relation));
		}
		ITK_CALL(AOM_refresh(LatestRev,0));
		ITK_CALL(AOM_lock(LatestRev));
		ITK_CALL(AOM_save(LatestRev));
		//ITK_CALL(AOM_unlock(LatestRev));
		ITK_CALL(AOM_refresh(LatestRev,1));
		ITK_CALL(AOM_load(LatestRev));
		
		ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));

	}
	}
	}
	
	printf(" \n closing FunToFindDummyClrSrlNDelRel \n\t"); fflush(stdout);


	return status;
}

int FunReadPrtNmCompCodeFrmFile(char *sPlatform,char *sSVRNum,char **sCompCode,char **sDataPartNumber)
{
	int		status					= 0;
	char	*itemid					= NULL;
	char	*attr_value				= NULL;
	char	*Clrscm					= NULL;
	char	*sFileName				= NULL;
	
	char	*sCompCodeF				= NULL;
	char	*sIntSchemF				= NULL;
	char	*sCategory				= NULL;
	char	*sPartNum				= NULL;
	
	char	temp[200];	
	FILE* fp = NULL;
	sFileName=(char *) MEM_alloc(100);
	*sCompCode		= malloc(10*sizeof(char));
	*sDataPartNumber = malloc(10*sizeof(char));

	printf("\n [%s] [%s] [%s] \n",sPlatform,sSVRNum,sCompCode);//CLRSCM-X4/18/0001


	sFileName	=	strcpy(sFileName,"");
	sFileName	=	strcpy(sFileName,"/tmp/");
	sFileName	=	strcat(sFileName,sPlatform);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,sSVRNum);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,"CompCode.txt");

	printf("\n Input sFileName:[%s] \n",sFileName);

	fp = fopen(sFileName, "r");
	
	if (fp != NULL)
	{
		while (fgets(temp, 400, fp)!= NULL)
		{
			tc_strcpy(temp,stripBlanks(temp));

			if (tc_strlen(temp) > 0 && tc_strcmp(temp, NULL) != 0)
			{
				sCompCodeF = strtok(temp, "^");
				//if(strcmp(sCompCodeF,sCompCode)==0)
				//{
				sIntSchemF = strtok(NULL, "^");
				sCategory  = strtok(NULL, "^");
				sPartNum   = strtok(NULL, "^");
				printf("\n\n\t sPartNum:[%s]\n\n",sPartNum);
				if(strlen(sPartNum)>0)
				{
				strcpy(*sCompCode,sIntSchemF);
				strcpy(*sDataPartNumber,sPartNum);
				break;
				}
				else
				{
				strcpy(*sDataPartNumber,"NA");
				break;
				}
				//}
			}			
			tc_strcpy(temp, "");
		}
	}
	else 
	{
		printf("File Unable to Open...!!");
	}
	 if (feof(fp)) 
	{
     printf("\n End of file reached."); 
	 strcpy(*sDataPartNumber,"EOF");
	}

	return status;
}
int FunGetPrtNmFrmFile(char *sPlatform,char *sSVRNum,char *sCompCode,char **sDataPartNumber)
{
	int		status=0;
	char	*itemid					= NULL;
	char	*attr_value				= NULL;
	char	*Clrscm					= NULL;
	char	*sFileName				= NULL;
	
	char	*sCompCodeF				= NULL;
	char	*sIntSchemF				= NULL;
	char	*sCategory				= NULL;
	char	*sPartNum				= NULL;
	
	char	temp[200];	
	FILE* fp = NULL;
	sFileName=(char *) MEM_alloc(100);
	*sDataPartNumber = malloc(10*sizeof(char));

	printf("\n [%s] [%s] [%s] \n",sPlatform,sSVRNum,sCompCode);//CLRSCM-X4/18/0001


	sFileName	=	strcpy(sFileName,"");
	sFileName	=	strcpy(sFileName,"/tmp/");
	sFileName	=	strcat(sFileName,sPlatform);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,sSVRNum);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,"CompCode.txt");

	printf("\n Input sFileName:[%s] \n",sFileName);

	fp = fopen(sFileName, "r");
	
	if (fp != NULL)
	{
		while (fgets(temp, 400, fp)!= NULL)
		{
			tc_strcpy(temp,stripBlanks(temp));

			if (tc_strlen(temp) > 0 && tc_strcmp(temp, NULL) != 0)
			{
				sCompCodeF = strtok(temp, "^");
				if(strcmp(sCompCodeF,sCompCode)==0)
				{
				sIntSchemF = strtok(NULL, "^");
				sCategory  = strtok(NULL, "^");
				sPartNum   = strtok(NULL, "^");
				printf("\n\n\t sPartNum:[%s]\n\n",sPartNum);
				if(strlen(sPartNum)>0)
				{
				strcpy(*sDataPartNumber,sPartNum);
				break;
				}
				else
				{
				strcpy(*sDataPartNumber,"NA");
				break;
				}
				}
			}			
			tc_strcpy(temp, "");
		}
	}
	else 
	{
		printf("File Unable to Open...!!");
	}
	 if (feof(fp)) 
	{
     printf("\n End of file reached."); 
	 strcpy(*sDataPartNumber,"EOF");
	}

	return status;

}

tag_t FunToReturnLatestRevision(char *Item_ID)
{
	int status =0;
	tag_t *tags_found = NULL;
	int n_tags_found= 0;
	int n_rev= 0;
	tag_t item_tag =NULLTAG;
	char *sitem_id=NULL;
	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));
	tag_t LatestRevision;
	attrs[0] ="item_id";
	values[0] = (char *) Item_ID;		


	ITK_CALL(ITEM_find_item(Item_ID,&item_tag));	

	//item_tag = tags_found[0];

	ITK_CALL(ITEM_ask_latest_rev(item_tag,&LatestRevision));

	ITK_CALL(AOM_ask_value_string(LatestRevision,"item_id",&sitem_id));
	
	printf("\n sitem_id: %s \n\n",sitem_id);	

	MEM_free(tags_found);

	return LatestRevision;

}

// function to replace CCA comp code with new color serial
int FunToReplaceColorSchemeData(char* ClrSufixName,char* OrgcompcodeStr)
{
	
	printf("\t Please proceed with parent color suffix: [%s] OrgcompcodeStr [%s]\n",ClrSufixName,OrgcompcodeStr);
	//ClrSufixName: CCA_CENTER_AIR_VENT~CLRPRT_ASY_1
	//ClrSufixName: CCA_CENTER_AIR_VENT
	//CCA_GLOVE_BOX~CLRPRT_ASY_3
	//CCA_STEERING_WHEEL~CLRPRT_ASY_1
	char *sCompCode = NULL;
	char *sColorID	= NULL;		
	char* c = ","; 
	char* d	= ";";
	
	printf("\n 77ClrSufixName [%s]\n",ClrSufixName);fflush(stdout);
	
	if(strstr(ClrSufixName,"~")!=NULL)
	{
		sCompCode =	tc_strtok(ClrSufixName,"~");
		sColorID = tc_strtok(NULL,"");
	
	if(strstr(ClrSufixName,"~")!=NULL)
	{
		printf("\n AAClrSufixName [%s]\n",ClrSufixName);fflush(stdout);
		sColorID = strtok(NULL,"~");
		printf("\n CCsColorID [%s]\n",sColorID);fflush(stdout);
	}
	else
	{
		printf("\n 1111ClrSufixName [%s]\n",ClrSufixName);fflush(stdout);
		printf("\n 1111sColorID [%s]\n",sColorID);fflush(stdout);
		//sColorID = strtok(NULL,"");
		//sColorID=(char *) MEM_alloc(50);
		//tc_strcpy(sColorID,ClrSufixName);
	}	
	}
	else
	{
		printf("\n BBClrSufixName [%s]\n",ClrSufixName);fflush(stdout);
		sCompCode =	tc_strtok(ClrSufixName,"~");
		sColorID =	tc_strtok(NULL,"");
		//sColorID=(char *) MEM_alloc(50);
		//tc_strcpy(sColorID,ClrSufixName);
	}

	printf("\n 77sColorID [%s]\n",sColorID);fflush(stdout);

	int		status						=	0;
	tag_t   Cntl_obj_Ctag				=	NULLTAG;
	int     n_entry_C					=	1;
	char    *qry_sys_entryC[1]			=	{"Colour ID"};
	char    **qry_sys_valuesC			=  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCountC					=	0;
	int		IsValidFlag					=	0;
	tag_t   *ClrSuf_tag					=	NULLTAG;
	char	*sClrSerSuf					=	NULL;
	char	*sClrID						=	NULL;
	char	*CID						=	NULL;
	tag_t object_create_input_tag1		=	NULLTAG;
	tag_t Colordata_type_tag			=	NULLTAG;
	tag_t new_COLDATA_object			=	NULLTAG;	
	tag_t clrdatarevtag					=	NULLTAG;	
	tag_t clrdatarevtag1				=	NULLTAG;	
	tag_t ColData_relation_type			=	NULLTAG;
	tag_t new_relation					=	NULLTAG;
	tag_t del_relation					=	NULLTAG;

	
	int		n_entries					=	3;
	tag_t	queryTag					=	NULLTAG;
	char*	clrsrlmain1					=	NULL;
	int		resultCount					=	0;
	tag_t	*qry_output					=	NULLTAG;
	char *item_id_ColDatamain			=	NULL;
	char* dupintrnalscheme				=	NULL;
	char* ClrsrlCat						=	NULL;

	dupintrnalscheme=(char *)MEM_alloc(100);
	ClrsrlCat=(char *)MEM_alloc(100);

	char* item_id_ColData=NULL;
	item_id_ColData=(char *)MEM_alloc(100);

	clrsrlmain1=(char *)MEM_alloc(100);
	CID=(char *) MEM_alloc(50);
	
			
	printf("\n  ");fflush(stdout);
	strcpy(CID,sColorID);
	strcat(CID,"*");
	printf("\n inside FunToReplaceColorSchemeData 1: CID:[%s]",CID);fflush(stdout);

	ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
	
	// Find ##;DUMMY_COLSRL comp code

	
	ITK_CALL(AOM_load(LatestRev));

	ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
	//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

	printf("\n  ");fflush(stdout);
	printf("\n inside FunToReplaceColorSchemeData  2");fflush(stdout);

	ITK_CALL(QRY_find("Colour Master", &Cntl_obj_Ctag));
	if (Cntl_obj_Ctag)
	{
		printf("\n 11 Found Query Colour Master... \n");fflush(stdout);
	}
	else
	{
		printf("\n Not Found Query Colour Master... \n");fflush(stdout);
	}

	qry_sys_valuesC[0] = CID;
	ITK_CALL(QRY_execute(Cntl_obj_Ctag, n_entry_C, qry_sys_entryC, qry_sys_valuesC, &IntCountC, &ClrSuf_tag));
	printf("11 IntCountC: [%d] \n",IntCountC); fflush(stdout);

		printf(" \n "); fflush(stdout);
		if(IntCountC>0)
		{
			ITK_CALL(AOM_UIF_ask_value(ClrSuf_tag[0],"object_name",&sClrSerSuf));
			ITK_CALL(AOM_UIF_ask_value(ClrSuf_tag[0],"t5_ColourId",&sClrID));		
			
			printf("\n sClrSerSuf[%s] sClrID[%s]\n",sClrSerSuf,sClrID);fflush(stdout);
			// Function to Create Color Scheme Data and Add it in Color Scheme.
			//FunToCreClrSchData(sCompCodeS,sClrSerSuf,sClrID);
			//FunToCreClrSchData(char *sCompCodeS,char *sClrSerSuffix,char* sClrID)	

	/***********************************************************************************************/
			
			// Query Is color scheme data available for give inputs.

			printf("\n sClrSerSuf[%s] sClrID[%s]\n",sClrSerSuf,sClrID);fflush(stdout);
			printf("sCompCode --------> %s\n",sCompCode);
					
			if(QRY_find("ColorSchemeDataRevision", &queryTag));
			if (queryTag)
			{
			printf("Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
			}
			else
			{
			printf("Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
			return status;
			}
			char *qry_entries4[3] = {"Comp Code","Internal Scheme","Color Serial"};
			char **qry_values = (char **) MEM_alloc(50 * sizeof(char *));

			tc_strcpy(clrsrlmain1,"");
			tc_strcat(clrsrlmain1,sClrSerSuf);
			tc_strcat(clrsrlmain1,"*");
			tc_strcat(clrsrlmain1,sClrID);
			printf("clrsrlmain1 --------> %s\n",clrsrlmain1);

			qry_values[0] = sCompCode ;
			qry_values[1] = "Y" ;
			qry_values[2] = clrsrlmain1 ;

			ITK_CALL(QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &qry_output));
			printf("\n Count of Query Values ------------------>  %d \n",resultCount);
			
			if (resultCount == 0)
			{			
				tc_strcpy(dupintrnalscheme,"");
				tc_strcat(dupintrnalscheme,sClrID);
				tc_strcat(dupintrnalscheme,";");
				tc_strcat(dupintrnalscheme,sClrSerSuf);

				tc_strcpy(ClrsrlCat,"");
				tc_strcat(ClrsrlCat,sClrSerSuf);
				tc_strcat(ClrsrlCat,";");
				tc_strcat(ClrsrlCat,sClrID);

				tc_strcpy(item_id_ColData,"");
				tc_strcat(item_id_ColData,sCompCode);
				tc_strcat(item_id_ColData,"`");
				tc_strcat(item_id_ColData,"Y");
				tc_strcat(item_id_ColData,"`");
				tc_strcat(item_id_ColData,dupintrnalscheme);

				item_id_ColDatamain = replaceWord(item_id_ColData, c, d);
				printf("\n**************** item_id_ColDatamain: %s  ***************\n",item_id_ColDatamain);fflush(stdout);

				ITK_CALL(TCTYPE_find_type("T5_ClrShmData", NULL, &Colordata_type_tag));   

				ITK_CALL(TCTYPE_construct_create_input(Colordata_type_tag, &object_create_input_tag1));

				ITK_CALL(AOM_set_value_string(object_create_input_tag1,"object_name",item_id_ColDatamain));
				ITK_CALL(AOM_set_value_string(object_create_input_tag1,"item_id",item_id_ColDatamain));

				ITK_CALL(TCTYPE_create_object(object_create_input_tag1, &new_COLDATA_object));

				if(new_COLDATA_object)
				{
				printf("\n t5CreateClrschm ---------------------------> new_COLDATA_object created.");
				ITK_CALL(AOM_save(new_COLDATA_object));					
				ITK_CALL(AOM_refresh(new_COLDATA_object,0));
				}

				ITK_CALL( ITEM_ask_latest_rev (new_COLDATA_object,&clrdatarevtag) );
				if(clrdatarevtag)
				{
				ITK_CALL(AOM_refresh(clrdatarevtag,1));
				ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_PrtCatCode",sCompCode));
				ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_InternalSchm","Y"));
				ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_ClSrl",ClrsrlCat));
				ITK_CALL(AOM_save(clrdatarevtag));
				ITK_CALL(AOM_refresh(clrdatarevtag,0));
				}

				
				if(ColData_relation_type != NULLTAG)
				{
					ITK_CALL(GRM_create_relation(LatestRev,clrdatarevtag,ColData_relation_type,NULLTAG,&new_relation));
					if(new_relation != NULLTAG)
						{
						ITK_CALL(GRM_save_relation(new_relation));
						ITK_CALL(AOM_load(LatestRev));
						ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
						//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));
						}
				}
				
			}
			else
				{
					printf("\n Inside else of relation creation if not available \n");fflush(stdout);
					//ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
					if(ColData_relation_type != NULLTAG)
						{				
						clrdatarevtag1 = qry_output[0];
						ITK_CALL(GRM_find_relation(LatestRev,clrdatarevtag1,ColData_relation_type,&del_relation));
						if(del_relation!=NULLTAG)
							{
								printf("\n relation already available. \n");fflush(stdout);
							}
							else
							{
							ITK_CALL(GRM_create_relation(LatestRev,clrdatarevtag1,ColData_relation_type,NULLTAG,&new_relation));
							if(new_relation != NULLTAG)
							{
							ITK_CALL(GRM_save_relation(new_relation));
							}
							}
						}
				}
			}
			ITK_CALL(AOM_load(LatestRev));

			ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
			//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

}
static void initialise (void)
{
	int ifail;
	int status;

	/* <kc> pr#397778 July2595 exit if autologin() fail */
	(ITK_auto_login());
	printf("Login fail !!: Error code = %d \n\n",ifail); fflush(stdout);
	

	/* these tokens come from bom_attr.h */
	initialise_attribute (bomAttr_lineName, &name_attribute);
	initialise_attribute (bomAttr_occSeqNo, &seqno_attribute);
	(BOM_line_look_up_attribute (bomAttr_lineParentTag, &parent_attribute));
	
	(BOM_line_look_up_attribute (bomAttr_lineItemTag, &item_tag_attribute));
	
}
static void initialise_attribute (char *name,  int *attribute)
{
	int ifail, mode;
	int status;

	(BOM_line_look_up_attribute (name, attribute));
	
	(BOM_line_ask_attribute_mode (*attribute, &mode));
	
	if (mode != BOM_attribute_mode_string)
	{
		printf ("Help,  attribute %s has mode %d,  I want a string\n", name, mode); fflush(stdout);
		exit(0);
	}
}
