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
#define ITK_CALL(X) 							\
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

tag_t LatestRev = NULLTAG;
tag_t 	class_to_load_as;
FILE* fplog = NULL;
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
char* subString (char* mainStringf, int fromCharf, int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(3);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}
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
	printf("\n setAddStr %d",*count);fflush(stdout);
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
	char *outputfile=NULL;
	char *Parentfile=NULL;
	char *Childfile=NULL;
	char *AllFiles=NULL;
	char *input=NULL;
	char *sParentCompCode=NULL;
	char *sParentColID=NULL;
	char *sbl_item_id=NULL;  
	char *sbl_rev_id=NULL;  
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

	int DOstatus =0;
	int	DelStatus	=0;

	(void)argc;
	(void)argv;

	fpos_t pos;
	fgetpos(stdout, &pos);
	int fd = dup(fileno(stdout));
	//freopen("/user/bsd05818/Ashish/SuffixSelection/test.txt", "a", stdout);
	

	initialise();

	//req_item = ITK_ask_cli_argument("-i=");

	itemId = ITK_ask_cli_argument("-i=");
	

	if(itemId== NULL)
	{
		printf("\n Please use exe as ColorSchemeClrSrlSetValue -i=$ColorSchemeNumber");fflush(stdout);
		(POM_logout(false));
		return status;
	}

	if( ITK_init_module("ercpsup" ,"XYT1ESA","dba")!=ITK_ok) ;
	
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	//ITK_CALL(ITK_auto_login( ));
	ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n login .......");fflush(stdout);
//	outputfile=(char *) MEM_alloc(50);
//	tc_strcpy(outputfile,"ColorSuffixDetails_Data.txt");
//	fptr=fopen(outputfile,"w");
//	printf("\n outputfile id %s ",outputfile);fflush(stdout);
//
//	Parentfile=(char *) MEM_alloc(50);
//	tc_strcpy(Parentfile,"Parent_ColorSuffixDetails.txt");
//	fptrP=fopen(Parentfile,"w");
//	printf("\n Parentfile id %s ",Parentfile);fflush(stdout);
//
//	Childfile=(char *) MEM_alloc(50);
//	tc_strcpy(Childfile,"Child_ColorSuffixDetails.txt");
//	fptrC=fopen(Childfile,"w");
//	printf("\n Childfile id %s ",Childfile);fflush(stdout);
//
//	AllFiles=(char *) MEM_alloc(50);
//	tc_strcpy(AllFiles,"AllColorSuffixDetails.txt");
//	fptrA=fopen(AllFiles,"w");
//	printf("\n AllFiles id %s ",AllFiles);fflush(stdout);
//
//	fprintf(fptrA,"PartNumber,Revision,ParentCompCode,ParentColorID,ChildCompCode,ChildColorID,ColorSerial,\n"); fflush(fptrA);

	if ( itemId )
	{
//		tag_t *tags_found = NULL;
//		int n_tags_found= 0;
//		int n_rev= 0;
//		char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
//		char **values = (char **) MEM_alloc(1 * sizeof(char *));
//
//		attrs[0] ="item_id";
//		values[0] = (char *) itemId;		
//
//		ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
//		MEM_free(attrs);
//		MEM_free(values);		
//
//		if (n_tags_found == 0)
//		{
//			printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", itemId); fflush(stdout);
//			exit (0);
//		}
//
//		item_tag = tags_found[0];
//
//		ITK_CALL(ITEM_ask_latest_rev(item_tag,&LatestRev));
//
//		MEM_free(tags_found);

		LatestRev = FunToReturnLatestRevision(itemId);
		
		ITK_CALL(AOM_ask_value_string(LatestRev,"item_id",&sitem_id));
		ITK_CALL(AOM_ask_value_string(LatestRev,"t5_VehNO",&sSVRNo));
		ITK_CALL(AOM_ask_value_string(LatestRev,"t5_SchmPlant",&sPlatform));
	
		printf("\nColor scheme Name: [%s] SVR number: [%s]\n",sitem_id,sSVRNo);
	}
	else
	{
		printf ("Pls enter input part ?? \n"); fflush(stdout);
		exit (0);
	}

	ITK_CALL(GRM_find_relation_type	("T5_ShmHasClrData",&SchmHasClrData_RelTag));

	int		i					 =	0;
	int		SchemeDataCnt		 =	0;
	int		IntRem				 =	0;
	int		IntRem1				 =	0;
	int		IntRem4				 =	0;
	int		IntRem5				 =	0;
	int		YNFlag11			 =	0;
	int		YNFlag22			 =	0;
	int		rm					 =	0;
	tag_t	*ClrSchemeData_tag	 =	NULLTAG;
	tag_t   ColData_relation_type	 =	NULLTAG;
	tag_t   ClrToClrDatarel_tag	 =	NULLTAG;
	char **sPartNumberSet = (char **) MEM_alloc(1 * sizeof(char *));
	char **sPartNumberSet4 = (char **) MEM_alloc(1 * sizeof(char *));
	char **sPrtCompCodeSet = (char **) MEM_alloc(1 * sizeof(char *));
	char **sPrtCompCodeSet5 = (char **) MEM_alloc(1 * sizeof(char *));
		


	ITK_CALL(GRM_list_secondary_objects_only(LatestRev,SchmHasClrData_RelTag,&SchemeDataCnt,&ClrSchemeData_tag));
//	
	printf("\n Color scheme data count SchemeDataCnt: %d\n",SchemeDataCnt);
//
//	if(SchemeDataCnt>0)
//	{
//	for (i=0;i<SchemeDataCnt;i++)
//	{
		char *sPrtCompCode		= NULL;
		char *sClrSerial		= NULL;
		char *sDataPartNumber	= NULL;
		char *sInternalScheme	= NULL;
		tag_t LatestRev_tag		= NULLTAG;
	
		printf("find values of color scheme data.\n");

		//ITK_CALL(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_PrtCatCode",&sPrtCompCode));
		//ITK_CALL(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_ClSrl",&sClrSerial));
		//comment below , try to use comp code in function
		//ITK_CALL(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_SchmPrtC",&sDataPartNumber));
		//ITK_CALL(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_InternalSchm",&sInternalScheme));

		//printf("\n sInternalScheme[%s] sClrSerial [%s] \n",sInternalScheme,sClrSerial);

		//if(tc_strstr(sClrSerial,"##")!=NULL && tc_strcmp(sInternalScheme,"T")==0)
		//{
			// comp code CCA_AIR_VENT having Color serail ## and internal scheme T
			//printf("\n Color scheme Part number whose color serial ## and Internal Scheme T sDataPartNumber: %s \n",sDataPartNumber);
			int Rstatus =0;
			

			// Make new function to pass compcode and colour scheme and return partnuumber(this will read data from a file created in /tmp), use returnvalue from function in below .
			//ITK_CALL(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_PrtCatCode",&sPrtCompCode));


			// Code to read file and pass comp code and part numbers to FunExpPartToGetChildCompCodes

			// File write to take log4

	inputfile =(char *) MEM_alloc(100);
	tc_strcpy(inputfile,sPlatform);
	tc_strcat(inputfile,"_");
	tc_strcat(inputfile,sSVRNo);
	tc_strcat(inputfile,"Suffix_Selection.log");
	fplog = fopen(inputfile,"w");
	if(fplog == NULL)
	{
	printf ("\n Could not open inputfile file : %s\n", inputfile); fflush(stdout);
	exit( EXIT_FAILURE );
	}

			// file write end

	char	*sFileName				= NULL;
	char	*sFileName1				= NULL;

	char	*sCompCodeF				= NULL;
	char	*sIntSchemF				= NULL;
	char	*sCategory				= NULL;
	char	*sPartNum				= NULL;
	int Isallow=0;

	char	temp[200];	
	FILE* fp = NULL;
	sFileName1=(char *) MEM_alloc(100);

	sFileName1	=	strcpy(sFileName1,"");
	sFileName1	=	strcpy(sFileName1,"/user/bsd05818/Ashish/Color/");
	sFileName1	=	strcat(sFileName1,sPlatform);
	sFileName1	=	strcat(sFileName1,"_");
	sFileName1	=	strcat(sFileName1,sSVRNo);
	sFileName1	=	strcat(sFileName1,"_");
	sFileName1	=	strcat(sFileName1,"Suf");
	sFileName1	=	strcat(sFileName1,"_");
	sFileName1	=	strcat(sFileName1,"CompCode.log");

	printf("\n Input sFileName1:[%s] \n",sFileName1);
	fprintf(fplog," Input sFileName1:[%s] \n",sFileName1); fflush(fplog);
	
	freopen(sFileName1, "a", stdout);

	sFileName=(char *) MEM_alloc(100);

	sFileName	=	strcpy(sFileName,"");
	sFileName	=	strcpy(sFileName,"/tmp/");
	sFileName	=	strcat(sFileName,sPlatform);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,sSVRNo);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,"Suf");
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,"CompCode.txt");

	printf("\n Input sFileName:[%s] \n",sFileName);
	fprintf(fplog," Input sFileName:[%s] \n",sFileName); fflush(fplog);

	fp = fopen(sFileName, "r");
	
	if (fp != NULL)
	{
		while (fgets(temp, 400, fp)!= NULL)
		{
			tc_strcpy(temp,stripBlanks(temp));

			if (tc_strlen(temp) > 0 && tc_strcmp(temp, NULL) != 0)
			{
				sCompCodeF = strtok(temp, "^");//CCA_AIR_VENT
				//if(strcmp(sCompCodeF,sCompCode)==0)
				//{
				sIntSchemF = strtok(NULL, "^");//T
				sCategory  = strtok(NULL, "^");//Interior
				sPartNum   = strtok(NULL, "^");//544583200651
				printf("\n\n\t sPartNum:[%s]\n\n",sPartNum);
				fprintf(fplog,"\n\t sPartNum:[%s]\n\n",sPartNum); fflush(fplog);
				if((strlen(sPartNum)>0) &&(strcmp(sIntSchemF,"T")==0))
				{

				printf("\n sPrtCompCode[%s] \n",sCompCodeF);
				fprintf(fplog,"sPrtCompCode[%s] \n",sCompCodeF); fflush(fplog);

				printf("\n Inside function: FunGetPrtNmFrmFile \n");
				fprintf(fplog," Inside function: FunGetPrtNmFrmFile  \n"); fflush(fplog);

				//FunGetPrtNmFrmFile(sPlatform,sSVRNo,sPrtCompCode,&sDataPartNumber);
				//FunReadPrtNmCompCodeFrmFile(sPlatform,sSVRNo,&sPrtCompCode,&sDataPartNumber);

				Isallow = FunToFindDmyClRelWithColScm(LatestRev,sCompCodeF);

				printf("\n Return from function: sPartNum: [%s] \n Isallow %d\n",sPartNum,Isallow);
				fprintf(fplog,"  Return from function: sPartNum: [%s] \n Isallow %d\n",sPartNum,Isallow); fflush(fplog);
			
				if((tc_strlen(sPartNum)>0)&& (Isallow==1))
				{
				Rstatus = FunExpPartToGetChildCompCodes(sPartNum,sCompCodeF,LatestRev,IntRem);	
				printf("\n Return from function: FunExpPartToGetChildCompCodes Rstatus %d \n",Rstatus);
				fprintf(fplog," Return from function: FunExpPartToGetChildCompCodes Rstatus %d \n",Rstatus); fflush(fplog);
				if(Rstatus==0)
				{
				printf("\n Color scheme and color scheme data relation deletion Rstatus: %d \n",Rstatus);
				fprintf(fplog," Color scheme and color scheme data relation deletion Rstatus: %d \n",Rstatus); fflush(fplog);
				//ITK_CALL(GRM_find_relation(LatestRev,ClrSchemeData_tag[i],SchmHasClrData_RelTag,&ClrToClrDatarel_tag));	
				//ITK_CALL(GRM_delete_relation(ClrToClrDatarel_tag));

				DelStatus = FunToFindDummyClrSrlNDelRel(LatestRev,sCompCodeF);
				
				printf("\n *************** Refreshing Color scheme **************************\n");
				fprintf(fplog," ***** Refreshing Color scheme *** \n"); fflush(fplog);

				ITK_CALL(AOM_refresh(LatestRev,0));
				ITK_CALL(AOM_lock(LatestRev));
				ITK_CALL(AOM_save(LatestRev));
				//ITK_CALL(AOM_unlock(LatestRev));
				ITK_CALL(AOM_refresh(LatestRev,1));
				//ITK_CALL(AOM_load(LatestRev));

				printf("\n POM_refresh_instances \n");
				fprintf(fplog," POM_refresh_instances \n"); fflush(fplog);

				ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
				//ITK_CALL(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

				printf("\n *********************D END OF PROGRAM***********************************\n");
				fprintf(fplog," *******D END OF PROGRAM**** \n"); fflush(fplog);

				}
				else if(Rstatus==1)
				{
				
				printf("\n *************** Refreshing Color scheme without updating color scheme **************************\n");
				fprintf(fplog," ** Refreshing Color scheme without updating color scheme *** \n"); fflush(fplog);fflush(fplog);

				ITK_CALL(AOM_refresh(LatestRev,0));
				ITK_CALL(AOM_lock(LatestRev));
				ITK_CALL(AOM_save(LatestRev));
				ITK_CALL(AOM_refresh(LatestRev,1));
				ITK_CALL(AOM_load(LatestRev));
			
				
				ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
				//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));


				fprintf(fplog,"\n closing program without updating color scheme data\n");fflush(fplog);
				fprintf(fplog,"\n **********************************C END OF PROGRAM***************************\n");fflush(fplog);
				}
				else if(Rstatus==2)
				{
					fprintf(fplog,"\n *************** comp code children contains CCA and ##, so process later **************************\n");fflush(fplog);	
					fprintf(fplog,"\n ******** sPartNum: %s  sCompCodeF %s\n",sPartNum,sCompCodeF);fflush(fplog);	
					
					setAddStr(&IntRem,&sPartNumberSet,sPartNum);
					setAddStr(&IntRem1,&sPrtCompCodeSet,sCompCodeF);				
				}
				else
				{
					fprintf(fplog,"\n *************** LIVE LOVE LOUGH**************************\n");
				}
				}

				fprintf(fplog,"\n IntRem: [%d] , proceed to update all remaining color scheme data.\n",IntRem);fflush(fplog);
				
				if(i == (SchemeDataCnt-1))
				{
					for(rm=0;rm<IntRem;rm++)
						{
						fprintf(fplog,"\n *********rm: %d************** \n",rm);fflush(fplog);
						//DOstatus = FunExpPartToGetChildCompCodes(sPartNumberSet[rm],sPrtCompCodeSet[rm],LatestRev,IntRem);
						fprintf(fplog,"\n *********sPartNumberSet[rm]: %s sPrtCompCodeSet[rm] %s ************** \n",sPartNumberSet[rm],sPrtCompCodeSet[rm]);fflush(fplog);
						}


					do
					{
						fprintf(fplog,"\n *********IntRem: %d************** \n",IntRem);fflush(fplog);
						//for(rm=0;rm<IntRem;rm++)
						for(rm=(IntRem-1);rm>=0;rm--)
						{
						fprintf(fplog,"\n *********rm: %d************** \n",rm);fflush(fplog);
						ITK_CALL(AOM_refresh(LatestRev,0));
						ITK_CALL(AOM_lock(LatestRev));
						ITK_CALL(AOM_save(LatestRev));
						ITK_CALL(AOM_refresh(LatestRev,1));
						ITK_CALL(AOM_load(LatestRev));
						
						ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
						//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

						DOstatus = FunExpPartToGetChildCompCodes(sPartNumberSet[rm],sPrtCompCodeSet[rm],LatestRev,IntRem);
						fprintf(fplog,"\n *********DOstatus: %d************** \n",DOstatus);fflush(fplog);
						if(DOstatus==0)
						{
						DelStatus = FunToFindDummyClrSrlNDelRel(LatestRev,sPrtCompCodeSet[rm]);
						fprintf(fplog,"\n *********DelStatus: %d************** \n",DelStatus);fflush(fplog);
						}
						}
					}while(rm==0);

					fprintf(fplog,"\n **********Out of while loop ************** \n");fflush(fplog);
				

				}

//			if(IntRem>0)
//			{
//				if(IntRem1>0)
//				{
//				for (rm=0;rm<IntRem;rm++)
//				{
//					Rstatus = FunExpPartToGetChildCompCodes(sPartNumberSet[rm],sPrtCompCodeSet[rm],LatestRev,IntRem);
//					if(Rstatus==0)
//					{
//					printf("\n 11 Color scheme and color scheme data relation deletion Rstatus: %d \n",Rstatus);
//					ITK_CALL(GRM_find_relation(LatestRev,ClrSchemeData_tag[i],SchmHasClrData_RelTag,&ClrToClrDatarel_tag));	
//					if(ClrToClrDatarel_tag)
//						{
//					ITK_CALL(GRM_delete_relation(ClrToClrDatarel_tag));
//						}
//
//					printf("\n ***************11 D Refreshing Color scheme **************************\n");
//
//					ITK_CALL(AOM_refresh(LatestRev,0));
//					ITK_CALL(AOM_lock(LatestRev));
//					ITK_CALL(AOM_save(LatestRev));
//					//ITK_CALL(AOM_unlock(LatestRev));
//					ITK_CALL(AOM_refresh(LatestRev,1));
//					ITK_CALL(AOM_load(LatestRev));
//					printf("\n *********************11 END OF PROGRAM***********************************\n");
//
//					}
//					else if(Rstatus==1)
//					{
//
//					printf("\n ***************11 Refreshing Color scheme without updating color scheme **************************\n");
//
//					ITK_CALL(AOM_refresh(LatestRev,0));
//					ITK_CALL(AOM_lock(LatestRev));
//					ITK_CALL(AOM_save(LatestRev));
//					ITK_CALL(AOM_refresh(LatestRev,1));
//					ITK_CALL(AOM_load(LatestRev));
//
//					printf("\n 11 closing program without updating color scheme data\n");
//					printf("\n **********************************11 C END OF PROGRAM***************************\n");
//					}
//					else if(Rstatus==2)
//					{
//					printf("\n ***************11 comp code children contains CCA and ##, so process later **************************\n");				
//					setFindStr(sPartNumberSet,IntRem,sDataPartNumber,&YNFlag11);
//					setFindStr(sPrtCompCodeSet,IntRem1,sPrtCompCode,&YNFlag22);
//					if((YNFlag11==0) && (YNFlag22==0))
//					{					
//					setAddStr(&IntRem,&sPartNumberSet,sDataPartNumber);
//					setAddStr(&IntRem1,&sPrtCompCodeSet,sPrtCompCode);
//					}
//					}
//					else
//					{
//					printf("\n ***************11 LIVE LOVE LOUGH**************************\n");
//					}
//				}
//				}
//			}
//		}
//		else
//		{
//			printf("\n No ## and T combination found\n");
//		}
//	}
//	}

			}
			}
		}
		fclose(fp);
	}
	
	
	// try to update...




	for(rm=0;rm<IntRem;rm++)
	{
	fprintf(fplog,"\n *********rm: %d************** \n",rm);fflush(fplog);
	//DOstatus = FunExpPartToGetChildCompCodes(sPartNumberSet[rm],sPrtCompCodeSet[rm],LatestRev,IntRem);
	fprintf(fplog,"\n *********sPartNumberSet[rm]: %s sPrtCompCodeSet[rm] %s ************** \n",sPartNumberSet[rm],sPrtCompCodeSet[rm]);fflush(fplog);
	}

	
	int remaining=IntRem;
	do
	{
	fprintf(fplog,"\n *********IntRem: %d************** \n",IntRem);fflush(fplog);
	//for(rm=0;rm<IntRem;rm++)
	for(rm=(IntRem-1);rm>=0;rm--)
	{
	ITK_CALL(AOM_refresh(LatestRev,0));
	ITK_CALL(AOM_lock(LatestRev));
	ITK_CALL(AOM_save(LatestRev));
	ITK_CALL(AOM_refresh(LatestRev,1));
	ITK_CALL(AOM_load(LatestRev));
		
	Isallow=0;
	
	Isallow = FunToFindDmyClRelWithColScm(LatestRev,sPrtCompCodeSet[rm]);
	
	fprintf(fplog,"\n *******Isallow: %d************** \n",Isallow);fflush(fplog);

	if(Isallow==1)
	{
	ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
	//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));
	
	fprintf(fplog,"\n ********remaining: %d************** \n",remaining);fflush(fplog);


	fprintf(fplog,"\n *********rm: %d************** \n",rm);fflush(fplog);
	DOstatus = FunExpPartToGetChildCompCodes(sPartNumberSet[rm],sPrtCompCodeSet[rm],LatestRev,IntRem);
	fprintf(fplog,"\n *********DOstatus: %d************** \n",DOstatus);fflush(fplog);
	if(DOstatus==0)
	{
	DelStatus = FunToFindDummyClrSrlNDelRel(LatestRev,sPrtCompCodeSet[rm]);
	fprintf(fplog,"\n in do while loop.. deleted ## comp codes \n");fflush(fplog);
	fprintf(fplog,"\n *******DelStatus: %d************** \n",DelStatus);fflush(fplog);
	}
	else
	{
	fprintf(fplog,"\n in do while loop ... in else of not delete ##  %d\n",DOstatus);fflush(fplog);
	//remaining++;
	setAddStr(&IntRem4,&sPartNumberSet4,sPartNumberSet[rm]);
	setAddStr(&IntRem5,&sPrtCompCodeSet5,sPrtCompCodeSet[rm]);
	}
	}
	else
	{
	fprintf(fplog,"\n **********not allow to update ************** \n");fflush(fplog);
	}
	}
	}while(rm==0);

	fprintf(fplog,"\n **********Out of while loop ************** \n");fflush(fplog);

	fprintf(fplog,"\n rechecking remaining comp codes SchemeDataCnt: %d\n",SchemeDataCnt);fflush(fplog);
	int m=0;
	fp = fopen(sFileName, "r");
	if (fp != NULL)
	{
	if(SchemeDataCnt>0)
	{
		for (m=0;m<SchemeDataCnt;m++ )
		{
			(AOM_ask_value_string(ClrSchemeData_tag[m],"t5_PrtCatCode",&sPrtCompCode));
			(AOM_ask_value_string(ClrSchemeData_tag[m],"t5_ClSrl",&sClrSerial));
			(AOM_ask_value_string(ClrSchemeData_tag[m],"t5_InternalSchm",&sInternalScheme));

			if (strcmp(sInternalScheme,"T")==0)
			{	
				while (fgets(temp, 400, fp)!= NULL)
				{
				tc_strcpy(temp,stripBlanks(temp));

				if (tc_strlen(temp) > 0 && tc_strcmp(temp, NULL) != 0)
				{
					sCompCodeF = strtok(temp, "^");//CCA_AIR_VENT
					//if(strcmp(sCompCodeF,sCompCode)==0)
					//{
					sIntSchemF = strtok(NULL, "^");//T
					sCategory  = strtok(NULL, "^");//Interior
					sPartNum   = strtok(NULL, "^");//544583200651
					fprintf(fplog,"\n\n\t sPartNum:[%s]\n\n",sPartNum);fflush(fplog);

					if(strcmp(sCompCodeF,sPrtCompCode)==0)
					{
						Rstatus = FunExpPartToGetChildCompCodes(sPartNum,sCompCodeF,LatestRev,IntRem);	
						fprintf(fplog,"\n Return from function: FunExpPartToGetChildCompCodes Rstatus %d \n",Rstatus);fflush(fplog);
						if(Rstatus==0)
						{
						fprintf(fplog,"\n Color scheme and color scheme data relation deletion Rstatus: %d \n",Rstatus);fflush(fplog);
						//ITK_CALL(GRM_find_relation(LatestRev,ClrSchemeData_tag[i],SchmHasClrData_RelTag,&ClrToClrDatarel_tag));	
						//ITK_CALL(GRM_delete_relation(ClrToClrDatarel_tag));

						DelStatus = FunToFindDummyClrSrlNDelRel(LatestRev,sCompCodeF);

						fprintf(fplog,"\n *************** Refreshing Color scheme **************************\n");fflush(fplog);

						ITK_CALL(AOM_refresh(LatestRev,0));
						ITK_CALL(AOM_lock(LatestRev));
						ITK_CALL(AOM_save(LatestRev));
						//ITK_CALL(AOM_unlock(LatestRev));
						ITK_CALL(AOM_refresh(LatestRev,1));
						//ITK_CALL(AOM_load(LatestRev));

						fprintf(fplog,"\n POM_refresh_instances \n");fflush(fplog);

						ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
						//ITK_CALL(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

						fprintf(fplog,"\n *********************D END OF PROGRAM***********************************\n");fflush(fplog);

					}
						
					}
				}
				}			
			}
		}
	}
	}

	ITK_exit_module(true);	
	fclose(fplog);
	fsetpos(stdout, &pos);
	return status;
}
int FunToFindDmyClRelWithColScm(tag_t ClrSchmTag,char* compCodeStrInList)
{
	int		status						=	0;	
	int		flagallow					=	0;	
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
	char* sPrtCompCode					=	NULL;
	char* sClrSerial					=	NULL;
	char* sInternalScheme				=	NULL;
	tag_t SchmHasClrData_RelTag			=	NULLTAG;
	tag_t del_relation					=	NULLTAG;
	int m = 0;
	int SchemeDataCnt = 0;
	tag_t* ClrSchemeData_tag = NULLTAG;
	clrsrlmain2=(char *)MEM_alloc(100);

	fprintf(fplog," \n Inside FunToFindDmyClRelWithColScm \n\t"); fflush(stdout); fflush(fplog);
	
	(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));

	(GRM_find_relation_type	("T5_ShmHasClrData",&SchmHasClrData_RelTag));

	
	(GRM_list_secondary_objects_only(LatestRev,SchmHasClrData_RelTag,&SchemeDataCnt,&ClrSchemeData_tag));
	
	fprintf(fplog,"\n Color scheme data count SchemeDataCnt: %d\n",SchemeDataCnt); fflush(fplog);
	
	if(SchemeDataCnt>0)
	{
		for (m=0;m<SchemeDataCnt;m++ )
		{
			(AOM_ask_value_string(ClrSchemeData_tag[m],"t5_PrtCatCode",&sPrtCompCode));
			(AOM_ask_value_string(ClrSchemeData_tag[m],"t5_ClSrl",&sClrSerial));
			(AOM_ask_value_string(ClrSchemeData_tag[m],"t5_InternalSchm",&sInternalScheme));
			
			if(tc_strcmp(compCodeStrInList,sPrtCompCode)==0)
			{
				//if(tc_strcmp(sInternalScheme,"T")==0)
				if((tc_strcmp(sClrSerial,"##;DUMMY_COLSRL")==0) && (tc_strcmp(sInternalScheme,"T")==0))
				{
					flagallow=1;
					break;
				}
				else
				{
					flagallow=0;
				}
			}
		}
		
	fprintf(fplog,"\n closing FunToFindDmyClRelWithColScm flagallow: %d\n",flagallow); fflush(fplog);

	}

		
//	//	 Find ##;DUMMY_COLSRL comp code
//		
//	if(QRY_find("ColorSchemeDataRevision", &queryTagF));
//	if (queryTagF)
//	{
//	printf("find Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
//	}
//	else
//	{
//	printf("Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
//	return status;
//	}
//	char *qry_entriesF[3] = {"Comp Code","Internal Scheme","Color Serial"};
//	char **qry_valuesF = (char **) MEM_alloc(50 * sizeof(char *));
//
//	tc_strcpy(clrsrlmain2,"");
//	tc_strcat(clrsrlmain2,"##");
//	tc_strcat(clrsrlmain2,"*");
//	tc_strcat(clrsrlmain2,"DUMMY_COLSRL");
//	printf("find clrsrlmain2 --------> %s\n",clrsrlmain2);
//
//	qry_valuesF[0] = compCodeStrInList;
//	qry_valuesF[1] = "T";
//	qry_valuesF[2] = clrsrlmain2;
//
//	ITK_CALL(QRY_execute(queryTagF, n_entriesF, qry_entriesF, qry_valuesF, &resultCountF, &qry_outputF));
//	printf("\n find Count of Query Values resultCountF ------------------>  %d \n",resultCountF);
//	if(resultCountF>0)
//	{
//		for (m=0;m<resultCountF ;m++ )
//		{
//			ITK_CALL(AOM_UIF_ask_value(qry_outputF[m],"object_name",&sObjectNameDt));			
//		
//			ITK_CALL(GRM_find_relation(ClrSchmTag,qry_outputF[m],SchmHasClrData_RelTag,&del_relation));
//			if(del_relation!=NULLTAG)
//			{
//			printf(" \n Inside FunToFindDmyClRelWithColScm found relation, allow to update\n\t"); fflush(stdout);
//			
//			flagallow=1;
//			break;
//			}
//			else
//			{
//				flagallow=0;
//			}
//		
//			ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
//			//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));
//
//		}
//	}
	fprintf(fplog,"\n closed FunToFindDmyClRelWithColScm \n\t"); fflush(stdout); fflush(fplog);


	return flagallow;
}
int FunToFindDummyClrSrlNDelRel(tag_t ClrSchmTag, char* PrtCmpCodeDel)
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
	int m = 0;

	clrsrlmain2=(char *)MEM_alloc(100);

	fprintf(fplog," \n Inside FunToFindDummyClrSrlNDelRel \n\t"); fflush(stdout); fflush(fplog);

	ITK_CALL(GRM_find_relation_type	("T5_ShmHasClrData",&SchmHasClrData_RelTag));
		
	//	 Find ##;DUMMY_COLSRL comp code
		
	if(QRY_find("ColorSchemeDataRevision", &queryTagF));
	if (queryTagF)
	{
	fprintf(fplog,"Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout); fflush(fplog);
	}
	else
	{
	fprintf(fplog,"Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout); fflush(fplog);
	return status;
	}
	char *qry_entriesF[3] = {"Comp Code","Internal Scheme","Color Serial"};
	char **qry_valuesF = (char **) MEM_alloc(50 * sizeof(char *));

	tc_strcpy(clrsrlmain2,"");
	tc_strcat(clrsrlmain2,"##");
	tc_strcat(clrsrlmain2,"*");
	tc_strcat(clrsrlmain2,"DUMMY_COLSRL");
	fprintf(fplog,"clrsrlmain2 --------> %s\n",clrsrlmain2); fflush(fplog);

	qry_valuesF[0] = PrtCmpCodeDel;
	qry_valuesF[1] = "T";
	qry_valuesF[2] = clrsrlmain2;

	ITK_CALL(QRY_execute(queryTagF, n_entriesF, qry_entriesF, qry_valuesF, &resultCountF, &qry_outputF));
	fprintf(fplog,"\n Count of Query Values resultCountF ------------------>  %d \n",resultCountF); fflush(fplog);
	if(resultCountF>0)
	{
		for (m=0;m<resultCountF ;m++ )
		{
			ITK_CALL(AOM_UIF_ask_value(qry_outputF[m],"object_name",&sObjectNameDt));			
		
			ITK_CALL(GRM_find_relation(ClrSchmTag,qry_outputF[m],SchmHasClrData_RelTag,&del_relation));
			if(del_relation!=NULLTAG)
			{
			fprintf(fplog," \n Inside FunToFindDummyClrSrlNDelRel: found relation, now deleting it\n\t"); fflush(stdout);
			ITK_CALL(GRM_delete_relation(del_relation));
			}
			ITK_CALL(AOM_refresh(LatestRev,0));
			ITK_CALL(AOM_lock(LatestRev));
			ITK_CALL(AOM_save(LatestRev));
			//ITK_CALL(AOM_unlock(LatestRev));
			ITK_CALL(AOM_refresh(LatestRev,1));
			ITK_CALL(AOM_load(LatestRev));
			
			ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
			//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

		}
	}
	fprintf(fplog," \n closing FunToFindDummyClrSrlNDelRel \n\t"); fflush(stdout); fflush(fplog);
//	 end

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

	fprintf(fplog," \n [%s] [%s] [%s] \n",sPlatform,sSVRNum,sCompCode);fflush(fplog);//CLRSCM-X4/18/0001


	sFileName	=	strcpy(sFileName,"");
	sFileName	=	strcpy(sFileName,"/tmp/");
	sFileName	=	strcat(sFileName,sPlatform);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,sSVRNum);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,"CompCode.txt");

	fprintf(fplog," \n Input sFileName:[%s] \n",sFileName);fflush(fplog);

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
				fprintf(fplog,"\n\n\t sPartNum:[%s]\n\n",sPartNum);fflush(fplog);
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
		fprintf(fplog," File Unable to Open...!!");fflush(fplog);
	}
	 if (feof(fp)) 
	{
     fprintf(fplog," \n End of file reached."); fflush(fplog);
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

	fprintf(fplog,"\n [%s] [%s] [%s] \n",sPlatform,sSVRNum,sCompCode); fflush(fplog);//CLRSCM-X4/18/0001


	sFileName	=	strcpy(sFileName,"");
	sFileName	=	strcpy(sFileName,"/tmp/");
	sFileName	=	strcat(sFileName,sPlatform);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,sSVRNum);
	sFileName	=	strcat(sFileName,"_");
	sFileName	=	strcat(sFileName,"CompCode.txt");

	fprintf(fplog,"\n Input sFileName:[%s] \n",sFileName); fflush(fplog);

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
				fprintf(fplog,"\n\n\t sPartNum:[%s]\n\n",sPartNum); fflush(fplog);
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
		fprintf(fplog,"File Unable to Open...!!"); fflush(fplog);
	}
	 if (feof(fp)) 
	{
     fprintf(fplog,"\n End of file reached.");  fflush(fplog);
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

//	ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
//	MEM_free(attrs);
//	MEM_free(values);		
//
//	if (n_tags_found == 0)
//	{
//		printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", Item_ID); fflush(stdout);
//		exit (0);
//	}
	
	ITK_CALL(ITEM_find_item(Item_ID,&item_tag));	

	//item_tag = tags_found[0];

	ITK_CALL(ITEM_ask_latest_rev(item_tag,&LatestRevision));

	ITK_CALL(AOM_ask_value_string(LatestRevision,"item_id",&sitem_id));
	
	//fprintf(fplog,"\n sitem_id: %s \n\n",sitem_id);	 fflush(fplog);

	MEM_free(tags_found);

	return LatestRevision;

}
// This function returns color ID when input is as CCA_CENTER_AIR_VENT~CLRPRT_ASY_4
// input Colour ID as CLRPRT_ASY_4*
char* FunToGetColorID(char *ColorIDStr)
{
	int status =0;
	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int     n_entry				=	1;
	char    *qry_sys_entry[1]	=	{"Colour ID"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCount			=	0;
	int		IsValidFlag			=	0;
	tag_t   *ClrSuf_tag			=	NULLTAG;
	char	*sClrSerSuf			=	NULL;
	char	*sClrID				=	NULL;
	char	*compcode1			=	NULL;
	char	*colorid1			=	NULL;
	char	*clrid				=	NULL;

	clrid=(char *)MEM_alloc(100);
	//ColorIDStr= CCA_CENTER_AIR_VENT~CLRPRT_ASY_4
	printf("\n  ");fflush(fplog);
	fprintf(fplog,"\n inside FunToGetColorID ColorIDStr [%s] ",ColorIDStr);fflush(fplog);

 	(QRY_find("Colour Master", &Cntl_obj_Qtag));
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n Found Query Colour Master... \n");fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query Colour Master... \n");fflush(fplog);
	}
	
	compcode1 =	strtok(ColorIDStr,"~");
	colorid1 =	strtok(NULL,"");

	strcpy(clrid,"");
	strcpy(clrid,colorid1);
	strcpy(clrid,"*");

	qry_sys_values[0] = clrid;
	(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &ClrSuf_tag));
	printf(" \n "); fflush(stdout);
	if(IntCount>0)
	{
		(AOM_UIF_ask_value(ClrSuf_tag[0],"object_name",&sClrSerSuf));
		(AOM_UIF_ask_value(ClrSuf_tag[0],"t5_ColourId",&sClrID));		
		
		fprintf(fplog,"\n sClrSerSuf[%s] sClrID[%s]\n",sClrSerSuf,sClrID);fflush(fplog);
		// Function to Create Color Scheme Data and Add it in Color Scheme.
		//FunToCreClrSchData(sCompCodeS,sClrSerSuf,sClrID);
	}

	return sClrSerSuf;
}
char* funToGetPlaceHolderSuffix(char* compcodeStr)
{
	// query parent fix table data

	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int		l					=	0;
	int     n_entry				=	1;
	char    *qry_sys_entry[1]	=	{"Colour ID"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCountI			=	0;
	tag_t   *PrntClrSuf_tag		=	NULLTAG;
	char*	Obj_name			=	NULL;
	char*	sPlcHoldSuf			=	NULL;	
	char*	compcode			=	NULL;
	char*	d="~";
	char*	e="*";

	compcode=(char *)MEM_alloc(100);
	
	fprintf(fplog,"\n inside funToGetPlaceHolderSuffix...compcodeStr %s \n",compcodeStr); fflush(fplog);

	(QRY_find("Colour Master", &Cntl_obj_Qtag));
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n Get Colour Master... \n"); fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n No Colour Master... \n"); fflush(fplog);
	}

	//compcode = replaceWord(compcodeStr, d, e);

	strcpy(compcode,"");
	strcpy(compcode,compcodeStr);
	strcat(compcode,"*");

	fprintf(fplog,"\n No Colour Master... compcode %s \n",compcode); fflush(fplog);

	qry_sys_values[0] = compcode;
	//qry_sys_values[1] = StrSuf;
	(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCountI, &PrntClrSuf_tag));
	fprintf(fplog," \n inside funToGetPlaceHolderSuffix Parent color suffix count IntCountI: [%d]\n",IntCountI);  fflush(fplog);
	if(IntCountI>0)
	{
		AOM_ask_value_string(PrntClrSuf_tag[0],"object_name",&Obj_name);
		//AOM_ask_value_string(PrntClrSuf_tag[0],"t5_PlaceHolderSuffix",&sPlcHoldSuf);

	}
	
	return Obj_name;
}
//char* funToGetPlaceHolderSuffix(char* compcodeStr)
//{
//	// query parent fix table data
//
//	tag_t   Cntl_obj_Qtag		=	NULLTAG;
//	int		l					=	0;
//	int     n_entry				=	1;
//	char    *qry_sys_entry[1]	=	{"Name"};
//	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
//	int		IntCount			=	0;
//	tag_t   *PrntClrSuf_tag		=	NULLTAG;
//	char*	Obj_name			=	NULL;
//	char*	sPlcHoldSuf			=	NULL;	
//	char*	compcode			=	NULL;
//	char*	d="~";
//	char*	e="*";
//	
//	printf("\n inside funToGetPlaceHolderSuffix...compcodeStr %s \n",compcodeStr);fflush(stdout);
//
//	(QRY_find("Color_Suffix_Query", &Cntl_obj_Qtag));
//	if (Cntl_obj_Qtag)
//	{
//		printf("\n -- Found Query Color_Suffix_Query... \n");fflush(stdout);
//	}
//	else
//	{
//		printf("\n Not Found Query Color_Suffix_Query... \n");fflush(stdout);
//	}
//
//	compcode = replaceWord(compcodeStr, d, e);
//
//	qry_sys_values[0] = compcode;
//	//qry_sys_values[1] = StrSuf;
//	(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &PrntClrSuf_tag));
//	printf(" \n inside funToGetPlaceHolderSuffix Parent color suffix count IntCount: [%d]\n",IntCount); fflush(stdout);
//	if(IntCount>0)
//	{
//		AOM_ask_value_string(PrntClrSuf_tag[0],"object_name",&Obj_name);
//		AOM_ask_value_string(PrntClrSuf_tag[0],"t5_PlaceHolderSuffix",&sPlcHoldSuf);
//
//	}
//	
//	return sPlcHoldSuf;
//}
int FunToGetNewObjName(char* sOldObjName,char **sObjNm)
{
	
	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int		l					=	0;
	int     n_entry				=	1;
	int     flag_i				=	0;
	int     Iseq				=	0;
	int     INum				=	0;
	int     IClrID				=	0;

	char    *qry_sys_entry[1]	=	{"Name"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCount			=	0;
	tag_t   *PrntClrSuf_tag		=	NULLTAG;
	
	
	char*	compcode			=	NULL;
	char*	SplcHoldr			=	NULL;
	char*	SClrID				=	NULL;
	char*	Sseq				=	NULL;
	char*	Num					=	NULL;
	char*	Num1				=	NULL;
	char*	sNewObjName			=	NULL;
	char*	CLRPRT				=	NULL;
	char*	ASY					=	NULL;
	int		IntCldCmp			=	0;
	char	SnwSeq[50];
	char	SNumS[50];
	char	*SclrSuf_Nm			=	NULL;
	SclrSuf_Nm=(char *) MEM_alloc(250);
	Num=(char *) MEM_alloc(15);
	*sObjNm=(char *) MEM_alloc(250);

		
	fprintf(fplog,"\n inside FunToGetNewObjName...sOldObjName %s\n",sOldObjName);fflush(fplog);//CCA_CENTER_AIR_VENT~CLRPRT_ASY_1

	if(QRY_find("Color_Suffix_Query", &Cntl_obj_Qtag)!=ITK_ok);
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n -- Found Query Color_Suffix_Query... \n");fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query Color_Suffix_Query... \n");fflush(fplog);
	}
	
	qry_sys_values[0] = sOldObjName;//CCA_AIR_VENT~CLRPART_ASSY~1
	//CCA_CENTER_AIR_VENT~CLRPRT_ASY_1
	(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &PrntClrSuf_tag));
	fprintf(fplog," \n inside FunToGetNewObjName Parent color suffix count IntCount: [%d]\n",IntCount);fflush(fplog);
	if(IntCount>0)
	{
		for (l=0;l<IntCount;l++)
		{
			flag_i=1;
			
			SplcHoldr = strtok(sOldObjName,"~");
			if(tc_strstr(sOldObjName,"~")!=NULL)
			{
				fprintf(fplog,"\n tt \n");fflush(fplog);
			SClrID	  = strtok(NULL,"~");			
			Sseq	  = strtok(NULL,"");			
			}
			else
			{
			fprintf(fplog,"\n mm \n");fflush(fplog);
			SClrID	  = strtok(NULL,"");						
			}
			fprintf(fplog,"\n SClrID:[%s] \n",SClrID);fflush(fplog);
			if(tc_strlen(Sseq)>0)
			{
			Iseq = atoi(Sseq);
			fprintf(fplog,"\n :: Iseq is :: %d\n",Iseq);fflush(fplog);
			Iseq = Iseq+1;
			fprintf(fplog,"\n :: Iseq after append :: %d\n",Iseq);fflush(fplog);		
			sprintf(SnwSeq, "%d",Iseq);
			
			fprintf(fplog,"\n :: SnwSeq after append :: %s\n",SnwSeq);fflush(fplog);	

			strcpy(SclrSuf_Nm,"");
			strcat(SclrSuf_Nm,SplcHoldr);
			strcat(SclrSuf_Nm,"~");
			strcat(SclrSuf_Nm,SClrID);
			strcat(SclrSuf_Nm,"~");
			strcat(SclrSuf_Nm,SnwSeq);
			}			
			else
			{
			fprintf(fplog,"\n else cat CLRPRT_ASY_NUM");fflush(fplog);//CLRPART_ASSY~2 CLRPART_ASY_2 
			fprintf(fplog,"\n 3333SClrID:  %s",SClrID);fflush(fplog);//CLRPART_ASSY~2 CLRPART_ASY_2 
			//CLRPRT_ASY10

			Num1 = subString(SClrID,10,12);
			fprintf(fplog,"\n substring Num1:  %s",Num1);fflush(fplog);

			CLRPRT = tc_strtok(SClrID,"_");
			
			fprintf(fplog,"\n 4444CLRPRT:  %s",CLRPRT);fflush(fplog);

			if(tc_strcmp(CLRPRT,"CLRPART")==0)
				{
					strcpy(SclrSuf_Nm,"");			
					strcat(SclrSuf_Nm,SplcHoldr);
					strcat(SclrSuf_Nm,"~");
					strcat(SclrSuf_Nm,"CLRPRT_ASY_1");					
				}
				else
				{
				fprintf(fplog,"\n SClrID in else:  %s",SClrID);fflush(fplog);
				if(strstr(SClrID,"~")!=NULL)
				{
				ASY = tc_strtok(NULL,"~");
				Num = tc_strtok(NULL,"~");
				}
				else if(atoi(Num1)>9)
				{
				strcpy(Num,"");
				strcpy(Num,Num1);
				}
				else
				{
				ASY = tc_strtok(NULL,"_");
				Num= tc_strtok(NULL,"_");
				}				
				INum = atoi(Num);//CLRPRT_ASY_1
				fprintf(fplog,"\n :: INum is :: %d\n",INum);fflush(fplog);
				INum = INum+1;
				fprintf(fplog,"\n :: INum after append :: %d\n",INum);fflush(fplog);			
				sprintf(SNumS, "%d",INum);
				fprintf(fplog,"\n :: SplcHoldr: %s \n CLRPRT: %s \n ASY: %s \n INum  :: %d\n",SplcHoldr,CLRPRT,ASY,INum);fflush(fplog);
				strcpy(SclrSuf_Nm,"");
				strcat(SclrSuf_Nm,SplcHoldr);
				strcat(SclrSuf_Nm,"~");
				strcat(SclrSuf_Nm,CLRPRT);
				if(INum>9)
				{
				strcat(SclrSuf_Nm,"_");
				strcat(SclrSuf_Nm,"ASY");
				}
				else
				{
				strcat(SclrSuf_Nm,"_");
				strcat(SclrSuf_Nm,ASY);
				}
				// if value is more than 9 then concat directly.
				fprintf(fplog,"\n :: check INum  :: %d\n",INum);fflush(fplog);
				if(INum<=9)
				{
				strcat(SclrSuf_Nm,"_");
				strcat(SclrSuf_Nm,SNumS);
				}
				else if(INum>9)
				{
				strcat(SclrSuf_Nm,SNumS);
				}
				}
			}
			FunToGetNewObjName(SclrSuf_Nm,&sNewObjName);

			strcpy(*sObjNm,"");
			tc_strcat(*sObjNm,sNewObjName);
			//tc_strcat(*sObjNm,SclrSuf_Nm);
			break;
		}
	}
	else
	{
	flag_i=0;
	strcpy(*sObjNm,"");
	tc_strcat(*sObjNm,sOldObjName);
	}
	return flag_i;
}
// function to new entry in fix table as given input CCA_CENTER_AIR_VENT~CLRPRT_ASY_4
int funToNewEntryInFixTable(char* compcodeStr,char** CompCodeSet,int CompCodeInt,char **LtstCompCode)
{
	fprintf(fplog,"\n inside funToNewEntryInFixTable...compcodeStr %s\n",compcodeStr); fflush(fplog);
	
	// query parent fix table data
	int status			=	0;
	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int		l					=	0;
	int		m					=	0;
	char* d	= "~";
	char* e  = "*"; 
	int     n_entry				=	2;
	int     Nflag				=	0;
	int     IntCmCode			=	0;
	char    *qry_sys_entry[2]	=	{"Name","Place Holder"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCount			=	0;
	tag_t   *PrntClrSuf_tag		=	NULLTAG;
	char*	Obj_name			=	NULL;
	char*	ChildCompCode		=	NULL;
	char*	compcode			=	NULL;
	int		IntCldCmp			=	0;
	char	**sChldClrSufixvalues  =	NULL;
	char	*ChldCompCode		  =	NULL;
	char	*ChldCompCodeCpy		  =	NULL;
	char	*SplcHoldr			  =	NULL;
	char	*ScatSuf		  =	NULL;
	char	*StrSufStr		  =	NULL;
	char	*placeholderS		  =	NULL;
	char	*clrSerialS		  =	NULL;
	char	*plchlS		  =	NULL;

	tag_t	Clr_type_tag				= NULLTAG;
	tag_t	object_create_input_tag		= NULLTAG;
	tag_t	Chd_type_tag				= NULLTAG;
	tag_t	object_create_chd_tag		= NULLTAG;
	tag_t	ColSufTag					= NULLTAG;
	tag_t	ChdColSufTag				= NULLTAG;
	tag_t   Cntl_obj_tag				=	NULLTAG;
	int     n					=	2;
	char    *qry_entry[2]		=	{"Name","Type"};
	char    **qry_values		=  (char **) MEM_alloc(10 * sizeof(char *));
	int		Count				=	0;
	tag_t   *ChildClrSuf_tag    =	NULLTAG;
	

	ChldCompCodeCpy= (char *) MEM_alloc (250);
	ChldCompCodeCpy= (char *) MEM_alloc (250);
	char **ChldCompCodeSet = (char **) MEM_alloc(1 * sizeof(char *));
	compcode= (char *) MEM_alloc (100);
	ScatSuf= (char *) MEM_alloc (100);
	plchlS= (char *) MEM_alloc (100);
	*LtstCompCode=(char *) MEM_alloc(250);


	strcpy(*LtstCompCode,"");
	
	fprintf(fplog,"\n inside funToNewEntryInFixTable...compcodeStr %s\n",compcodeStr); fflush(fplog);//CLRPRT_ASY_1

	(QRY_find("Color_Suffix_Query", &Cntl_obj_Qtag));
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n -- Found Query Color_Suffix_Query... \n"); fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query Color_Suffix_Query... \n"); fflush(fplog);
	}
	
	strcpy(plchlS,"");
	strcpy(plchlS,compcodeStr);
	placeholderS = strtok(plchlS,"~");
	clrSerialS = strtok(NULL,"");
	compcode = replaceWord(compcodeStr, d, e);
	
	fprintf(fplog,"\n  placeholderS %s\n clrSerialS %s",placeholderS,clrSerialS); fflush(fplog);

	qry_sys_values[0] = compcode;
	qry_sys_values[1] = placeholderS;
	(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &PrntClrSuf_tag));
	fprintf(fplog," \n inside funToNewEntryInFixTable Parent color suffix count IntCount: [%d] \n",IntCount);  fflush(fplog);
	if(IntCount==0)
	{
		
		ITK_CALL(TCTYPE_find_type("T5_ColorSuffix", NULL, &Clr_type_tag));   
		ITK_CALL(TCTYPE_construct_create_input(Clr_type_tag, &object_create_input_tag));

		ITK_CALL(AOM_set_value_string(object_create_input_tag,"object_name",compcodeStr));
		ITK_CALL(TCTYPE_create_object(object_create_input_tag, &ColSufTag));

		if(ColSufTag)
		{
		fprintf(fplog,"\n t5CreateObject : object created."); fflush(fplog);
		ITK_CALL(AOM_save(ColSufTag));
		ITK_CALL(AOM_refresh(ColSufTag,0));

		ITK_CALL(AOM_refresh(ColSufTag,1));
		
		if(strstr(compcodeStr,"~")!=NULL)
			{
		SplcHoldr = strtok(compcodeStr,"~");
		if(strstr(compcodeStr,"~")!=NULL)
			{
			ScatSuf = strtok(NULL,"~");
			}
			else
			{
			ScatSuf = strtok(NULL,"");
			}
			}
			else
			{
				strcpy(ScatSuf,"");
				strcat(ScatSuf,compcodeStr);
			}
		
		StrSufStr = funToGetPlaceHolderSuffix(ScatSuf);

		ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolder",SplcHoldr));		
		ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolderSuffix",StrSufStr));

		ITK_CALL(AOM_save(ColSufTag));
		ITK_CALL(AOM_refresh(ColSufTag,1));

		fprintf(fplog,"\n CompCodeInt:[%d]\n",CompCodeInt); fflush(fplog);

			if(CompCodeInt>0)
			{
			
			fprintf(fplog,"\n CompCodeInt:[%d]\n",CompCodeInt); fflush(fplog);

			for(l=0;l<CompCodeInt;l++)
				{
				fprintf(fplog,"\n suffixInt22:[%d]\n",CompCodeInt); fflush(fplog);
				fprintf(fplog,"\n Child Color Suffix: [%s]\n",CompCodeSet[l]); fflush(fplog);
				
				ITK_CALL(QRY_find("General...", &Cntl_obj_tag));
				if (Cntl_obj_tag)
				{
					fprintf(fplog,"\n -- Found Query General... \n"); fflush(fplog);
				}
				else
				{
					fprintf(fplog,"\n Not Found Query General... \n"); fflush(fplog);
				}
				fprintf(fplog,"\n CompCodeSet [%s] \n",CompCodeSet[l]); fflush(fplog);
				
				ChildCompCode = replaceWord(CompCodeSet[l], d, e);
								
				qry_values[0] = ChildCompCode;
				qry_values[1] = "Child Color Suffix" ;
				ITK_CALL(QRY_execute(Cntl_obj_tag, n, qry_entry, qry_values, &Count, &ChildClrSuf_tag));
				fprintf(fplog," \n Child Color Suffix Count:[%d] \n",Count);  fflush(fplog);
				if(Count>0)
				{
					// create relation between color suffix and child color suffix.
					fprintf(fplog,"\n create relation between color suffix and child color suffix. \n"); fflush(fplog);
					if(FL_insert(ColSufTag,ChildClrSuf_tag[0],99));
					if(AOM_save(ColSufTag));

				}
				else
				{
				// create child color suffix
				ITK_CALL(TCTYPE_find_type("T5_Child_Color_Suffix", NULL, &Chd_type_tag));   
				ITK_CALL(TCTYPE_construct_create_input(Chd_type_tag, &object_create_chd_tag));
				ITK_CALL(AOM_set_value_string(object_create_chd_tag,"object_name",CompCodeSet[l]));					
				ITK_CALL(TCTYPE_create_object(object_create_chd_tag, &ChdColSufTag));
				if(ChdColSufTag)
				{
					fprintf(fplog,"\n t5CreateObject : object created."); fflush(fplog);
					ITK_CALL(AOM_save(ChdColSufTag));
					ITK_CALL(AOM_refresh(ChdColSufTag,0));


				}
				//CombCompCodeArr[l] = ChdColSufTag;

				if(FL_insert(ColSufTag,ChdColSufTag,99));
				if(AOM_save(ColSufTag));	

				}
				// Create relation betwn color suffix and child color suffix.
			}
			}
		}
	}
}

// function to entry as CLRPART_ASSY as new entry in fix table as it is not available.
int funToEntryInFixTable(char* compcodeStr,char** CompCodeSet,int CompCodeInt,char **LtstCompCode)
{
		// query parent fix table data
	int status			=	0;
	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int		l					=	0;
	int		m					=	0;
	char* d	= "~";
	char* e  = "*"; 
	int     n_entry				=	1;
	int     Nflag				=	0;
	int     IntCmCode			=	0;
	char    *qry_sys_entry[1]	=	{"Name"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCount			=	0;
	tag_t   *PrntClrSuf_tag		=	NULLTAG;
	char*	Obj_name			=	NULL;
	char*	ChildCompCode		=	NULL;
	char*	compcode			=	NULL;
	int		IntCldCmp			=	0;
	char	**sChldClrSufixvalues  =	NULL;
	char	*ChldCompCode		  =	NULL;
	char	*ChldCompCodeCpy		  =	NULL;
	char	*SplcHoldr			  =	NULL;
	char	*ScatSuf		  =	NULL;
	char	*StrSufStr		  =	NULL;

	tag_t	Clr_type_tag				= NULLTAG;
	tag_t	object_create_input_tag		= NULLTAG;
	tag_t	Chd_type_tag				= NULLTAG;
	tag_t	object_create_chd_tag		= NULLTAG;
	tag_t	ColSufTag					= NULLTAG;
	tag_t	ChdColSufTag				= NULLTAG;
	tag_t   Cntl_obj_tag				=	NULLTAG;
	int     n					=	2;
	char    *qry_entry[2]		=	{"Name","Type"};
	char    **qry_values		=  (char **) MEM_alloc(10 * sizeof(char *));
	int		Count				=	0;
	tag_t   *ChildClrSuf_tag    =	NULLTAG;
	

	ChldCompCodeCpy= (char *) MEM_alloc (250);
	char **ChldCompCodeSet = (char **) MEM_alloc(1 * sizeof(char *));
	compcode= (char *) MEM_alloc (100);
	*LtstCompCode=(char *) MEM_alloc(250);


	strcpy(*LtstCompCode,"");
	
	fprintf(fplog,"\n inside funToEntryInFixTable...compcodeStr %s\n",compcodeStr); fflush(fplog);

	(QRY_find("Color_Suffix_Query", &Cntl_obj_Qtag));
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n -- Found Query Color_Suffix_Query... \n"); fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query Color_Suffix_Query... \n"); fflush(fplog);
	}
	
	strcpy(compcode,"");
	strcpy(compcode,compcodeStr);
	strcpy(compcode,"~");
	strcpy(compcode,"CLRPART_ASSY");
	compcode = replaceWord(compcodeStr, d, e);
	
	qry_sys_values[0] = compcode;
	//qry_sys_values[1] = StrSuf;
	(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &PrntClrSuf_tag));
	fprintf(fplog," \n inside funToEntryInFixTable Parent color suffix count IntCount: [%d]\n",IntCount);  fflush(fplog);
	if(IntCount==0)
	{
		
		ITK_CALL(TCTYPE_find_type("T5_ColorSuffix", NULL, &Clr_type_tag));   
		ITK_CALL(TCTYPE_construct_create_input(Clr_type_tag, &object_create_input_tag));

		ITK_CALL(AOM_set_value_string(object_create_input_tag,"object_name",compcodeStr));
		ITK_CALL(TCTYPE_create_object(object_create_input_tag, &ColSufTag));

		if(ColSufTag)
		{
		fprintf(fplog,"\n t5CreateObject : object created."); fflush(fplog);
		ITK_CALL(AOM_save(ColSufTag));
		ITK_CALL(AOM_refresh(ColSufTag,0));

		ITK_CALL(AOM_refresh(ColSufTag,1));

		SplcHoldr = strtok(compcodeStr,"~");
		if(strstr(compcodeStr,"~")!=NULL)
			{
			ScatSuf = strtok(NULL,"~");
			}
			else
			{
			ScatSuf = strtok(NULL,"");
			}
		
		StrSufStr = funToGetPlaceHolderSuffix(ScatSuf);
		
		fprintf(fplog,"\n StrSufStr:: %s",StrSufStr); fflush(fplog);

		ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolder",SplcHoldr));		
		ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolderSuffix",StrSufStr));

		ITK_CALL(AOM_save(ColSufTag));
		ITK_CALL(AOM_refresh(ColSufTag,1));

		fprintf(fplog,"\n CompCodeInt:[%d]\n",CompCodeInt); fflush(fplog);

			if(CompCodeInt>0)
			{
			
			fprintf(fplog,"\n CompCodeInt:[%d]\n",CompCodeInt); fflush(fplog);

			for(l=0;l<CompCodeInt;l++)
				{
				fprintf(fplog,"\n suffixInt22:[%d]\n",CompCodeInt); fflush(fplog);
				fprintf(fplog,"\n Child Color Suffix: [%s]\n",CompCodeSet[l]); fflush(fplog);
				
				ITK_CALL(QRY_find("General...", &Cntl_obj_tag));
				if (Cntl_obj_tag)
				{
					fprintf(fplog,"\n -- Found Query General... \n"); fflush(fplog);
				}
				else
				{
					fprintf(fplog,"\n Not Found Query General... \n"); fflush(fplog);
				}
				fprintf(fplog,"\n CompCodeSet [%s] \n",CompCodeSet[l]); fflush(fplog);
				
				ChildCompCode = replaceWord(CompCodeSet[l], d, e);
								
				qry_values[0] = ChildCompCode;
				qry_values[1] = "Child Color Suffix" ;
				ITK_CALL(QRY_execute(Cntl_obj_tag, n, qry_entry, qry_values, &Count, &ChildClrSuf_tag));
				fprintf(fplog," \n Child Color Suffix Count:[%d] \n",Count);  fflush(fplog);
				if(Count>0)
				{
					// create relation between color suffix and child color suffix.
					fprintf(fplog,"\n create relation between color suffix and child color suffix. \n"); fflush(fplog);
					if(FL_insert(ColSufTag,ChildClrSuf_tag[0],99));
					if(AOM_save(ColSufTag));

				}
				else
				{
				// create child color suffix
				ITK_CALL(TCTYPE_find_type("T5_Child_Color_Suffix", NULL, &Chd_type_tag));   
				ITK_CALL(TCTYPE_construct_create_input(Chd_type_tag, &object_create_chd_tag));
				ITK_CALL(AOM_set_value_string(object_create_chd_tag,"object_name",CompCodeSet[l]));					
				ITK_CALL(TCTYPE_create_object(object_create_chd_tag, &ChdColSufTag));
				if(ChdColSufTag)
				{
					fprintf(fplog,"\n t5CreateObject : object created."); fflush(fplog);
					ITK_CALL(AOM_save(ChdColSufTag));
					ITK_CALL(AOM_refresh(ChdColSufTag,0));


				}
				//CombCompCodeArr[l] = ChdColSufTag;

				if(FL_insert(ColSufTag,ChdColSufTag,99));
				if(AOM_save(ColSufTag));	

				}
				// Create relation betwn color suffix and child color suffix.
			}
			}
		}
	}
	
	return status;
}
int funToGetLatClrSufData(char* compcodeStr,char** CompCodeSet,int CompCodeInt,char **LtstCompCode)
{
	// query parent fix table data

	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int		l					=	0;
	int		m,n					=	0;
	int     n_entry				=	1;
	int     Nflag				=	0;
	int     IntCmCode			=	0;
	int     YNFlag				=	0;
	int     flag_j				=	0;
	char    *qry_sys_entry[1]	=	{"Place Holder"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCount			=	0;
	tag_t   *PrntClrSuf_tag		=	NULLTAG;
	char*	Obj_name			=	NULL;
	char*	ChildCompCode		=	NULL;
	char*	compcode			=	NULL;
	int		IntCldCmp			=	0;
	char	**sChldClrSufixvalues  =	NULL;
	char	*ChldCompCode		  =	NULL;
	char	*ChldCompCodeCpy		  =	NULL;

	ChldCompCodeCpy= (char *) MEM_alloc (250);
	char **ChldCompCodeSet = (char **) MEM_alloc(1 * sizeof(char *));
	compcode= (char *) MEM_alloc (100);
	*LtstCompCode=(char *) MEM_alloc(250);


	strcpy(*LtstCompCode,"");
	
	fprintf(fplog,"\n inside funToGetLatClrSufData...compcodeStr %s\n",compcodeStr); fflush(fplog);

	(QRY_find("Color_Suffix_Query", &Cntl_obj_Qtag));
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n -- Found Query Color_Suffix_Query... \n"); fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query Color_Suffix_Query... \n"); fflush(fplog);
	}
	
	strcpy(compcode,"");
	strcpy(compcode,compcodeStr);
	//strcat(compcode,"*");

	qry_sys_values[0] = compcode; 
	//qry_sys_values[1] = StrSuf;
	(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &PrntClrSuf_tag));
	fprintf(fplog," \n  inside funToGetLatClrSufData. Parent color suffix count IntCount: [%d]\n",IntCount); fflush(fplog);
	if(IntCount>0)
	{
		for (l=0;l<IntCount;l++)
		{
			Nflag=0;
			AOM_ask_value_string(PrntClrSuf_tag[l],"object_name",&Obj_name);
			AOM_UIF_ask_values(PrntClrSuf_tag[l],"contents",&IntCldCmp,&sChldClrSufixvalues);
			
			fprintf(fplog," \n checking for Obj_name: [%s]\n",Obj_name); fflush(stdout); fflush(fplog);
			fprintf(fplog," \n IntCldCmp [%d] CompCodeInt [%d]\n",IntCldCmp,CompCodeInt); fflush(fplog);
			if(CompCodeInt>0)
			{
			if(IntCldCmp==CompCodeInt)// if count of comp codes and child color suffix is same.
			{
				IntCmCode=0;
				YNFlag=0;
				for(m=0;m<IntCldCmp;m++)
				{			
				fprintf(fplog,"\n sChldClrSufixvalues[m]: %s\n",sChldClrSufixvalues[m]); fflush(fplog);	
				//ChldCompCode = tc_strtok(sChldClrSufixvalues[m],"/");
				//printf("\n ChldCompCode: %s\n",ChldCompCode);	
				if (ChldCompCodeCpy)tc_strcpy(ChldCompCodeCpy,"");
				strcat(ChldCompCodeCpy,sChldClrSufixvalues[m]);				
				setFindStr(ChldCompCodeSet,IntCmCode,ChldCompCodeCpy,&YNFlag);
				if(YNFlag==0)
				{
				setAddStr(&IntCmCode,&ChldCompCodeSet,ChldCompCodeCpy);// create array of child color suffix.
				sort(ChldCompCodeSet,IntCmCode); // sort
				}
				}
				// compare set of comp code and child color suffix.

				for (n=0;n<IntCldCmp;n++)
				{
				fprintf(fplog,"\n ChldCompCodeSet: [%s] CompCodeSet: [%s]\n",ChldCompCodeSet[n],CompCodeSet[n]); fflush(fplog);
				
				setFindStr(ChldCompCodeSet,IntCmCode,CompCodeSet[n],&YNFlag);// if YNFlag=0,not found;if YNFlag=1, found
				fprintf(fplog,"\t\n YNFlag: [%d] \n",YNFlag);
				if(YNFlag==0)
				{
					Nflag=1;
					fprintf(fplog,"\t\n ChldCompCodeSet does not contain: [%s]\n",CompCodeSet[n]); fflush(fplog);
					break;
				}
				if(n==(IntCldCmp-1) && Nflag==0)
				{
					flag_j=1;
					fprintf(fplog,"\t\n for loop complete, arrays are equal.\n"); fflush(fplog);
					break;
				}		
			    }			
			}
			else
			{
				Nflag=1;
				fprintf(fplog,"\t IntCldCmp!=CompCodeInt, so setting Obj_name: [%s] Nflag [%d]\n",Obj_name,Nflag); fflush(fplog);				
			}
			
			}
			else
			{
				Nflag=3;
				fprintf(fplog,"\t CompCodeInt is zero, so setting Obj_name: [%s] Nflag [%d]\n",Obj_name,Nflag);										
			}
		
			if(flag_j==1)
			{
				fprintf(fplog,"\n \t All the values are equal. Please proceed with parent color suffix: [%s]\n",Obj_name); fflush(fplog);
				break;
			}		
		}			
	}
	else
	{
		fprintf(fplog,"\t inside else funToGetLatClrSufData...IntCount is: %d,No data found so Exit function",IntCount); fflush(fplog);
		tc_strcat(*LtstCompCode,"NOTAVAIL");
	}
	if(flag_j==1)
	{
	tc_strcat(*LtstCompCode,Obj_name);
	fprintf(fplog,"\t exiting funToGetLatClrSufData...Nflag: %d ,Exit",Nflag); fflush(fplog);
	}
	else if((Nflag==1))
	{
	tc_strcat(*LtstCompCode,Obj_name);
	fprintf(fplog,"\t exiting funToGetLatClrSufData...Nflag: %d ,Exit",Nflag); fflush(fplog);
	}


	return Nflag;
}
int FunExpPartToGetChildCompCodes(char *sClrDataPartNumber,char* PrtCompCodeStr,tag_t ClrScheme_tag,int IntRemClr)
{	
	int status =0;
	tag_t* LatestRevision_tag = NULLTAG;
	
	char* c = ","; 
	char* d	= ";";
	char* e  = "/"; 
	char* f  = "-";
	char* h  = "*";
	int numBVRs =0;
	int j=0;
	int p=0;
	int suffixCnt=0;
	int suffixCnt1=0;
	tag_t* bvr_tags = NULLTAG; 
	tag_t bom_view = NULLTAG;
	tag_t bom_window=NULLTAG;
	tag_t rule, item_tag = null_tag, topline_tag;
	tag_t top_line=NULLTAG;
	int n_lines,n_lines1 = 0;
	tag_t *lines = NULL;
	tag_t *bom_variant_config=NULLTAG;
	tag_t VarientRuletag = NULLTAG;
	logical modB=FALSE;
	char *sChildCompCode =NULL;
	char *sClrInd		  =NULL;
	char *sitem_id		  =NULL;
	int flag =0,i=0;
	tag_t   ColData_relation_type=	NULLTAG;
	char* sPrntCompCode=NULL;
	char **CompCodeData = (char **) MEM_alloc(1 * sizeof(char *));
	char **AllCompCodes = (char **) MEM_alloc(1 * sizeof(char *));
	char *RplccombCompCodeClrID =NULL;
	char *combCompCodeClrID =NULL;
	char *combCompCode =NULL;
	char *sParentCompCode =NULL;
	char *sCombCompCodeClrID =NULL;

	int 	num_to_sort			=	1;
	int 	r					=	0;
	char	*keys[1]			=	{"creation_date"};
	int		orders				=	1;

	char	*sParentClrSufName	=	NULL;
	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int     n_entry				=	1;
	char    *qry_sys_entry[1]	=	{"Place Holder"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCount			=	0;
	tag_t   *PrntClrSuf_tag		=	NULLTAG;
	int		num_values			=	0;
	char	**sChldClrSufixvalues	=	NULL;
	int		cnt					=	0;
	int		YNFlag				=	0;
	int		YNFlag1				=	0;
	int		ccaincca			=	0;
	char	*ClrSchmDt			=	NULL;



	//int		i					 =	0;
	int		ICnt				 =	0;
	int		SchemeDataCnt		 =	0;
	tag_t	*ClrSchemeData_tag	 =	NULLTAG;			
	tag_t   ClrToClrDatarel_tag	 =	NULLTAG;
	char	*sPrtCompCode		 = NULL;
	char	*sClrSerial			 = NULL;
	char	*sInternalScheme	 = NULL;
	char	*sClrSufIn			 = NULL;
	char	*sClrSufName		 = NULL;



	combCompCodeClrID= (char *) MEM_alloc (250);
	combCompCode= (char *) MEM_alloc (250);

	char	**sListOfChildSuffix = (char **) MEM_alloc(500 * sizeof(char *));
	ClrSchmDt= (char *) MEM_alloc (100);
	sCombCompCodeClrID= (char *) MEM_alloc (100);
	char **ClrSchemDataList = (char **) MEM_alloc(500 * sizeof(char *));
		

	ITK_CALL(GRM_find_relation_type	("T5_ShmHasClrData",&ColData_relation_type));

	ITK_CALL(AOM_load(ClrScheme_tag));

	
	ITK_CALL(POM_refresh_instances(1,&ClrScheme_tag,class_to_load_as,lock_type));
	//(POM_load_instances(1,&ClrScheme_tag,class_to_load_as,lock_type));


	fprintf(fplog,"Design Revision ID sitem_id: %s",sitem_id);fflush(fplog);	

	// new logic

	// BOM Comp Code Logic

	LatestRevision_tag = FunToReturnLatestRevision(sClrDataPartNumber);

	ITK_CALL(AOM_ask_value_string(LatestRevision_tag,"item_id",&sitem_id));

	ITK_CALL(ITEM_rev_list_bom_view_revs(LatestRevision_tag, &numBVRs, &bvr_tags));

	if ( numBVRs == 0 )
	{
		fprintf(fplog, " Error : No BOM View Revision Exists \n\n " );fflush(fplog);
		fprintf(fplog, " Exiting the program \n " );fflush(fplog);
		//exit(0);
		//continue1;
		status=1;
		return status;

	}

	ITK_CALL(PS_ask_bom_view_of_bvr(bvr_tags[0],&bom_view));
	ITK_CALL ( BOM_create_window( &bom_window ));
	ITK_CALL(CFM_find( "ERC released and above", &rule ));
	ITK_CALL(BOM_set_window_config_rule( bom_window, rule ));	
	ITK_CALL(BOM_set_window_pack_all(bom_window, TRUE));
	ITK_CALL(BOM_set_window_top_line( bom_window , NULLTAG, LatestRevision_tag, NULLTAG, &top_line ));
	fprintf(fplog," BOM_set_window_top_line..\n");fflush(fplog);

	if(bom_window != NULLTAG)
	{
		fprintf(fplog,"\n before inside bom_window-----\n");fflush(fplog);
		
		ITK_CALL(BOM_window_hide_unconfigured(bom_window));
		ITK_CALL(BOM_window_show_variants(bom_window));
		ITK_CALL(BOM_create_window_variant_config(bom_window,1,&bom_variant_config));
		ITK_CALL(BOM_variant_config_apply (bom_variant_config));

		fprintf(fplog,"\n After inside bom_window-----\n"); fflush(fplog);
		//ITK_CALL(BOM_window_apply_variant_configuration(bom_window,1,&VarientRuletag));
		fprintf(fplog,"\n After BOM_window_apply_variant_configuration-----\n");fflush(fplog);
		ITK_CALL(BOM_window_hide_variants(bom_window));
		ITK_CALL(BOM_window_ask_is_modified(bom_window,&modB));
		if(modB)
		{
			fprintf(fplog, "\n modified bom window:..\n");fflush(fplog);
		}
		else
		{
			fprintf(fplog, "\n not modfiifed ..\n");fflush(fplog);
		}

		ITK_CALL(BOM_line_ask_child_lines(top_line, &n_lines1, &lines));
		fprintf(fplog,"\n bom line count: %d \n", n_lines1);fflush(fplog);
		
		// code to check CCA under CCA

		if(n_lines1>0)
		{
		for(p=0;p<n_lines1;p++)
		{			
			(AOM_UIF_ask_value(lines[p],"bl_Design Revision_t5_PrtCatCode",&sParentCompCode));//ARM_REST		
			(AOM_UIF_ask_value(lines[p],"bl_Design Revision_t5_ColourInd",&sClrInd));	

			if((tc_strlen(sParentCompCode)>0) && (tc_strcmp(sClrInd,"Y")==0))
			{
				if(tc_strstr(sParentCompCode,"CCA_")!=NULL) 
				{
					(GRM_list_secondary_objects_only(ClrScheme_tag,ColData_relation_type,&SchemeDataCnt,&ClrSchemeData_tag));
					////ASHISH		
					fprintf(fplog,"\n Color scheme data count of given color scheme SchemeDataCnt: %d\n",SchemeDataCnt);fflush(fplog);

					if(SchemeDataCnt>0)
					{
					for (i=0;i<SchemeDataCnt;i++)
					{	
					(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_PrtCatCode",&sPrtCompCode));
					(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_ClSrl",&sClrSerial));
					
					if((tc_strcmp(sParentCompCode,sPrtCompCode)==0) && (tc_strstr(sClrSerial,"##")!=NULL))
					{
					ccaincca=1;
					break;
					}
					}
					}
				}
			}
		}
		if(ccaincca==1)
		{
		status=2;
		fprintf(fplog,"\n CCA contains CCA ccaincca %d \n",ccaincca);fflush(fplog);
		return status;
		}
		else
		{
			ccaincca=0;
			fprintf(fplog,"\n CCA not contains CCA ccaincca %d \n",ccaincca);fflush(fplog);
		}
		}
		if(ccaincca==0)
		{
		if(n_lines1>0)
		{
		for(j=0;j<n_lines1;j++)
		{			
			(AOM_UIF_ask_value(lines[j],"bl_Design Revision_t5_PrtCatCode",&sParentCompCode));//ARM_REST		
			(AOM_UIF_ask_value(lines[j],"bl_Design Revision_t5_ColourInd",&sClrInd));		
					
			 //printf("sParentCompCode: %s^%s\n",sParentCompCode,sColorID);	
			 			
			//printf( "\n  find out comp codes whose and color indicator Y in BOM Line \n");
			if((tc_strlen(sParentCompCode)>0) && (tc_strcmp(sClrInd,"Y")==0))
			{
			ITK_CALL(AOM_load(ClrScheme_tag));

			ITK_CALL(POM_refresh_instances(1,&ClrScheme_tag,class_to_load_as,lock_type));
			//(POM_load_instances(1,&ClrScheme_tag,class_to_load_as,lock_type));
			
			(GRM_list_secondary_objects_only(ClrScheme_tag,ColData_relation_type,&SchemeDataCnt,&ClrSchemeData_tag));
	////ASHISH		
			fprintf(fplog,"\n Color scheme data count of given color scheme SchemeDataCnt: %d\n",SchemeDataCnt);fflush(fplog);
			
			
			

			if(SchemeDataCnt>0)
			{
			for (i=0;i<SchemeDataCnt;i++)
			{	
				(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_PrtCatCode",&sPrtCompCode));
				(AOM_ask_value_string(ClrSchemeData_tag[i],"t5_ClSrl",&sClrSerial));
				
				//printf("\n sParentCompCode:[%s] sPrtCompCode:[%s]\n",sParentCompCode,sPrtCompCode);

				if(tc_strcmp(sParentCompCode,sPrtCompCode)==0)
				{
					fprintf(fplog,"\n sParentCompCode:[%s] sPrtCompCode:[%s]\n",sParentCompCode,sPrtCompCode);fflush(fplog);
					fprintf(fplog,"\n sClrSerial:[%s]\n",sClrSerial);fflush(fplog);
					

					// find if string array contains CCA as child of color suffix

					if((tc_strstr(sParentCompCode,"CCA_")!=NULL) && (tc_strstr(sClrSerial,"##")!=NULL))
					{
					fprintf(fplog,"\n : Parent comp code contains CCA child comp codes, return to function and process further. \n"); fflush(fplog);
//					if (IntRemClr>0)
//					{
//						printf("\n try once \n");
//					}
//					else
//					{
					
					status=2;
					return status;
					//}
					}
					// query parent fix table data

					sClrSufIn = tc_strtok(sClrSerial,";");
					sClrSufName = tc_strtok(NULL,"");
					fprintf(fplog,"\n sClrSufName: %s \n", sClrSufName);fflush(fplog); 
					if (combCompCodeClrID)tc_strcpy(combCompCodeClrID,"");
					tc_strcat(combCompCodeClrID,sPrtCompCode);
					tc_strcat(combCompCodeClrID,"/");
					tc_strcat(combCompCodeClrID,sClrSufName);
//					CompCodeData[suffixCnt]=(char *) MEM_alloc(tc_strlen(combCompCodeClrID)* sizeof(char ));
//					tc_strcpy(CompCodeData[suffixCnt],combCompCodeClrID);
//					//printf("\n CompCodeData: compcode and colsrl concat after comparing with clrscheme data: [%s]\n",CompCodeData[suffixCnt]);	
//					printf("\t [%s]\n",CompCodeData[suffixCnt]);	
				//	suffixCnt = suffixCnt+1;		
					if (combCompCode)tc_strcpy(combCompCode,"");
					tc_strcat(combCompCode,sPrtCompCode);
					
					setFindStr(CompCodeData,suffixCnt,combCompCodeClrID,&YNFlag);
					if(YNFlag==0)
					{
					setAddStr(&suffixCnt,&CompCodeData,combCompCodeClrID);
					sort(CompCodeData,suffixCnt); 
					}					
					setFindStr(AllCompCodes,suffixCnt1,combCompCode,&YNFlag1);
					if(YNFlag1==0)
					{
					setAddStr(&suffixCnt1,&AllCompCodes,combCompCode);
					sort(AllCompCodes,suffixCnt1); 
					}	
					
				}
			}		
			}			
			}
		}
		
	}
	}

	
	}
	fprintf(fplog,"\n count after comparing bomline comp codes and clrschemdata comp codes suffixCnt [%d]\n",suffixCnt); fflush(fplog);
	int l=0;
	int flag_i=0;
	int Eqflag=0;
	(QRY_find("Color_Suffix_Query", &Cntl_obj_Qtag));
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n -- Found Query General... \n"); fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query General... \n"); fflush(fplog);
	}

	qry_sys_values[0] = PrtCompCodeStr;
	QRY_execute_with_sort(Cntl_obj_Qtag,n_entry,qry_sys_entry,qry_sys_values,num_to_sort,keys,&orders,&IntCount,&PrntClrSuf_tag);
	fprintf(fplog," \n QRY_execute_with_sort Parent color suffix count IntCount: [%d]\n",IntCount); fflush(fplog);
	int	ListCount =0;
	
	if(IntCount==0)
	{
		fprintf(fplog," \n ERROR: ENTRY NOT FOUND IN FIX TABLE, ADD LOGIC TO CREATE FIX TABLE ENTRY."); fflush(fplog);
		FunToEntryDataInFIXTable(PrtCompCodeStr,CompCodeData,AllCompCodes,&suffixCnt);	
		
	}
	
	QRY_execute_with_sort(Cntl_obj_Qtag,n_entry,qry_sys_entry,qry_sys_values,num_to_sort,keys,&orders,&IntCount,&PrntClrSuf_tag);
	fprintf(fplog," \n 1111--Parent color suffix count IntCount: [%d]\n",IntCount); fflush(fplog);
//	if(IntCount==1)
//	{
//		for(i=0;i<IntCount;i++)
//		{
//			(AOM_UIF_ask_value(PrntClrSuf_tag[i],"object_name",&sParentClrSufName));
//			printf(" \n Parent color suffix found: [%s] \n",sParentClrSufName); fflush(stdout);
//			
//			(AOM_UIF_ask_values(PrntClrSuf_tag[i],"contents",&num_values,&sChldClrSufixvalues));	
//			printf(" \n child color suffix count [%d] for Parent color suffix: [%s] \n",num_values,sParentClrSufName); fflush(stdout);
//
//			if(num_values==suffixCnt)
//			{
//			printf(" \n number of child suffix and comp codes in bomline are same \n"); fflush(stdout);
//			for (cnt=0;cnt<num_values;cnt++)
//			{
//				//printf("\n count is[%d]:sChldClrSufixvalues[%s]\n",cnt,sChldClrSufixvalues[cnt]);fflush(stdout);
//				sListOfChildSuffix[cnt]=(char *) MEM_alloc(tc_strlen(sChldClrSufixvalues[cnt])* sizeof(char ));
//				tc_strcpy(sListOfChildSuffix[cnt],"");
//				tc_strcpy(sListOfChildSuffix[cnt],sChldClrSufixvalues[cnt]);
//				printf("\t [%s]\n",sListOfChildSuffix[cnt]);
//				ListCount = ListCount +1;	
//				
//			}
//			sort(sListOfChildSuffix,ListCount); // sort
//			for (l=0;l<suffixCnt;l++)
//			{
//				flag_i=0;
//				printf("\n CompCodeData: [%s] sListOfChildSuffix: [%s]\n",CompCodeData[l],sListOfChildSuffix[l]);
//				if(strcmp(CompCodeData[l],sListOfChildSuffix[l])==0)
//				{
//					printf("\tEQUAL\n");						
//				}
//				else
//				{
//					printf("\tNOT EQUAL\n");
//					flag_i=2;
//					break;
//				}
//			}
//			}
//			else
//			{
//				printf(" \n COLOR SUFFIX [%s] FOUND IN FIX TABLE, BUT AS IT'S CHILD COLOR SUFFIX IS NOT SAME.\n",sParentClrSufName); fflush(stdout);
//				printf(" \n CREATE COLOR SUFFIX WITH INCREMENTAL VALUES AND ADD CHILD COLOR SUFFIXES.\n"); fflush(stdout);
//				FunToEntryDataInFIXTable(PrtCompCodeStr,CompCodeData,AllCompCodes,&suffixCnt);
//				flag_i=2;
//			}
//		}	
//	}
	if(IntCount>0)
	{
		fprintf(fplog," \n MORE THAN 0 ENTRY FOUND IN FIX TABLE."); fflush(fplog);
		int cnt11=0;
		int cnt2=0;
		int IntClr=0;
		char **CldClrSufSet = (char **) MEM_alloc(1 * sizeof(char *));
				
		for(i=0;i<IntCount;i++)
		{
			(AOM_UIF_ask_value(PrntClrSuf_tag[i],"object_name",&sParentClrSufName));
			fprintf(fplog," \n Parent color suffix found: [%s] \n",sParentClrSufName); fflush(fplog);
	
			(AOM_UIF_ask_values(PrntClrSuf_tag[i],"contents",&num_values,&sChldClrSufixvalues));	
			fprintf(fplog," \n child color suffix count [%d] suffixCnt [%d] for Parent color suffix: [%s] \n",num_values,suffixCnt,sParentClrSufName); fflush(fplog);

			if(num_values==suffixCnt)
			{
			ListCount=0;
			flag_i=0;
			fprintf(fplog," \n number of child suffix and comp codes in bomline are same \n");  fflush(fplog);
			for (cnt=0;cnt<num_values;cnt++)
			{
				//printf("\n count is[%d]:sChldClrSufixvalues[%s]\n",cnt,sChldClrSufixvalues[cnt]);fflush(stdout);
				sListOfChildSuffix[cnt]=(char *) MEM_alloc(tc_strlen(sChldClrSufixvalues[cnt])* sizeof(char ));
				tc_strcpy(sListOfChildSuffix[cnt],"");
				tc_strcpy(sListOfChildSuffix[cnt],sChldClrSufixvalues[cnt]);
				fprintf(fplog,"\t\n\n sListOfChildSuffix[cnt]>>>> [%s]\n\n",sListOfChildSuffix[cnt]); fflush(fplog);			
				ListCount = ListCount +1;	
				// Ashish
				//setAddStr1(&IntClr,&CldClrSufSet,sListOfChildSuffix[cnt]);// create array of child color suffix.
				
			}
			sort(sListOfChildSuffix,ListCount);// sort
			
			for (cnt11=0;cnt11<cnt;cnt11++)
			{
				fprintf(fplog,"\t\n selectionSort >>> [%s]\n",sListOfChildSuffix[cnt11]); fflush(fplog);			
			}
			for (cnt2=0;cnt2<cnt;cnt2++)
			{
				fprintf(fplog,"\t\n CompCodeData[cnt2] >>> [%s]",CompCodeData[cnt2]); fflush(fplog);			
			}
			
			for (l=0;l<suffixCnt;l++)
			{				
				Eqflag=0;
				fprintf(fplog,"\n CompCodeData: [%s] sListOfChildSuffix: [%s]\n",CompCodeData[l],sListOfChildSuffix[l]); fflush(fplog);
				
				fprintf(fplog,"\t\n\n Finding the string in array using setFindStr \n\n"); fflush(fplog);		

				setFindStr(sListOfChildSuffix,ListCount,CompCodeData[l],&YNFlag);// if YNFlag=0,not found;if YNFlag=1, found
				fprintf(fplog,"\t\n YNFlag: [%d]\n",YNFlag); fflush(fplog);	
				if(YNFlag==0)//not found
				{
					flag_i=1;
					fprintf(fplog,"\t\n flag_i: [%d], break the loop as data not found.\n",flag_i); fflush(fplog);
					break;//data not found, then break.
				}
				fprintf(fplog,"\t\n flag_i: [%d] \n",flag_i); fflush(fplog);
				if((l==(suffixCnt-1)) && (flag_i==0))//for loop complete and flag_i=0, data is same.
				{
					Eqflag=1;
					fprintf(fplog,"\t\n flag_i: [%d] and for loop completed. setting Eqflag:[%d], as data is same. \n",flag_i,Eqflag); fflush(fplog);
					break;
				}
			}
			} 
			else
			{
				fprintf(fplog," \n number of child suffix and comp codes in bomline are not same \n"); fflush(fplog);
				flag_i=3;
			}
			fprintf(fplog," \n nnnnnnnflag_i: %d Eqflag : %d\n",flag_i,Eqflag); fflush(stdout);
			
			if(Eqflag==1)
			{
				fprintf(fplog,"\n \t All the values are equal. Please proceed with parent color suffix: [%s]\n",sParentClrSufName); fflush(fplog);
				break;
			}
		}	
	}
	
	fprintf(fplog,"\t flag_i: [%d] Eqflag: [%d]\n",flag_i,Eqflag); fflush(fplog);
	//if Eqflag=1,all the values are same, use same sParentClrSufName for updating color serial for CCA comp code.
	//if Eqflag=0,values are not same, create new entry in fix table.
	
	//if(flag_i==3 && Eqflag!=4)
	if(Eqflag==0)
	{
	
		fprintf(fplog,"\n\t flag_i [3], please create new entry in FIX Table, as no child clr suffix available as of BOM Comp Codes. \n"); fflush(fplog);
		status = FunToEntryDataInFIXTable(PrtCompCodeStr,CompCodeData,AllCompCodes,&suffixCnt);
		QRY_execute_with_sort(Cntl_obj_Qtag,n_entry,qry_sys_entry,qry_sys_values,num_to_sort,keys,&orders,&IntCount,&PrntClrSuf_tag);

		(AOM_UIF_ask_value(PrntClrSuf_tag[IntCount-1],"object_name",&sParentClrSufName));
		fprintf(fplog," \n After creating new color suffix: sParentClrSufName: [%s] status {%d} \n",sParentClrSufName,status);  fflush(fplog);
		fprintf(fplog," \n New entry added in fix table. setting Eqflag=1 to change color scheme data in color scheme.\n");  fflush(fplog);
		Eqflag=1;
	
	}
	if((Eqflag==1) && (status!=1))
	{
		fprintf(fplog,"\t Please proceed with parent color suffix: [%s]\n",sParentClrSufName); fflush(fplog);

		char *sCompCode = NULL;
		char *sColorID	= NULL;			
		fprintf(fplog,"\n sParentClrSufName [%s]\n",sParentClrSufName); fflush(fplog);
		sCompCode =	tc_strtok(sParentClrSufName,"~");
		sColorID = tc_strtok(NULL,"~");			

		fprintf(fplog,"\n sColorID [%s]\n",sColorID); fflush(fplog);

		//int status =0;
		tag_t   Cntl_obj_Ctag		=	NULLTAG;
		int     n_entry_C			=	1;
		char    *qry_sys_entryC[1]	=	{"Colour ID"};
		char    **qry_sys_valuesC   =  (char **) MEM_alloc(10 * sizeof(char *));
		int		IntCountC			=	0;
		int		IsValidFlag			=	0;
		tag_t   *ClrSuf_tag			=	NULLTAG;
		char	*sClrSerSuf			=	NULL;
		char	*sClrID				=	NULL;
		char	*CID				=	NULL;
		CID=(char *) MEM_alloc(50);
		
				
		fprintf(fplog,"\n  "); fflush(fplog);
		strcpy(CID,sColorID);
		strcat(CID,"*");
		fprintf(fplog,"\n CID:[%s]",CID); fflush(fplog);

		
		fprintf(fplog,"\n  "); fflush(fplog);
		fprintf(fplog,"\n "); fflush(fplog);

		ITK_CALL(QRY_find("Colour Master", &Cntl_obj_Ctag));
		if (Cntl_obj_Ctag)
		{
			fprintf(fplog,"\n Found Query Colour Master... \n"); fflush(fplog);
		}
		else
		{
			fprintf(fplog,"\n Not Found Query Colour Master... \n"); fflush(fplog);
		}

		qry_sys_valuesC[0] = CID;
		ITK_CALL(QRY_execute(Cntl_obj_Ctag, n_entry_C, qry_sys_entryC, qry_sys_valuesC, &IntCountC, &ClrSuf_tag));
		fprintf(fplog,"IntCountC: [%d] \n",IntCountC);  fflush(fplog);

			fprintf(fplog," \n ");  fflush(fplog);
			if(IntCountC>0)
			{
				ITK_CALL(AOM_UIF_ask_value(ClrSuf_tag[0],"object_name",&sClrSerSuf));
				ITK_CALL(AOM_UIF_ask_value(ClrSuf_tag[0],"t5_ColourId",&sClrID));		
				
				fprintf(fplog,"\n sClrSerSuf[%s] sClrID[%s]\n",sClrSerSuf,sClrID); fflush(fplog);
				// Function to Create Color Scheme Data and Add it in Color Scheme.
				//FunToCreClrSchData(sCompCodeS,sClrSerSuf,sClrID);
				//FunToCreClrSchData(char *sCompCodeS,char *sClrSerSuffix,char* sClrID)

			

		/***********************************************************************************************/
				//int		status		=	0;
				int		n_entries	=	3;
				tag_t	queryTag	=	NULLTAG;
				char*	clrsrlmain1	=	NULL;
				int		resultCount	=	0;
				tag_t	*qry_output	=	NULLTAG;
				char *item_id_ColDatamain=NULL;
				char* dupintrnalscheme=NULL;
				char* ClrsrlCat=NULL;

				dupintrnalscheme=(char *)MEM_alloc(100);
				ClrsrlCat=(char *)MEM_alloc(100);

				char* item_id_ColData=NULL;
				item_id_ColData=(char *)MEM_alloc(100);

				clrsrlmain1=(char *)MEM_alloc(100);
				// Query Is color scheme data available for give inputs.

				fprintf(fplog,"\n sClrSerSuf[%s] sClrID[%s]\n",sClrSerSuf,sClrID); fflush(fplog);
				fprintf(fplog,"sCompCode --------> %s\n",sCompCode); fflush(fplog);
						
				if(QRY_find("ColorSchemeDataRevision", &queryTag));
				if (queryTag)
				{
				fprintf(fplog,"Found Query 'ColorSchemeDataRevision' \n\t"); fflush(fplog);
				}
				else
				{
				fprintf(fplog,"Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(fplog);
				return status;
				}
				char *qry_entries4[3] = {"Comp Code","Internal Scheme","Color Serial"};
				char **qry_values = (char **) MEM_alloc(50 * sizeof(char *));

				tc_strcpy(clrsrlmain1,"");
				tc_strcat(clrsrlmain1,sClrSerSuf);
				tc_strcat(clrsrlmain1,"*");
				tc_strcat(clrsrlmain1,sClrID);
				fprintf(fplog,"clrsrlmain1 --------> %s\n",clrsrlmain1); fflush(fplog);

				qry_values[0] = sCompCode ;
				qry_values[1] = "T" ;
				qry_values[2] = clrsrlmain1 ;

				ITK_CALL(QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &qry_output));
				fprintf(fplog,"\n Final Count of Query Values ------------------>  %d \n",resultCount); fflush(fplog);
				tag_t object_create_input_tag1		=	NULLTAG;
				tag_t Colordata_type_tag			=	NULLTAG;
				tag_t new_COLDATA_object			=	NULLTAG;	
				tag_t clrdatarevtag					=	NULLTAG;	
				tag_t clrdatarevtag1				=	NULLTAG;	
				tag_t ColData_relation_type			=	NULLTAG;
				tag_t del_relation					=	NULLTAG;
				tag_t new_relation					=	NULLTAG;
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
					tc_strcat(item_id_ColData,"T");
					tc_strcat(item_id_ColData,"`");
					tc_strcat(item_id_ColData,dupintrnalscheme);

					item_id_ColDatamain = replaceWord(item_id_ColData, c, d);
					fprintf(fplog,"\n**************** item_id_ColDatamain: %s  ***************\n",item_id_ColDatamain); fflush(fplog);

					ITK_CALL(TCTYPE_find_type("T5_ClrShmData", NULL, &Colordata_type_tag));   

					ITK_CALL(TCTYPE_construct_create_input(Colordata_type_tag, &object_create_input_tag1));

					ITK_CALL(AOM_set_value_string(object_create_input_tag1,"object_name",item_id_ColDatamain));
					ITK_CALL(AOM_set_value_string(object_create_input_tag1,"item_id",item_id_ColDatamain));

					ITK_CALL(TCTYPE_create_object(object_create_input_tag1, &new_COLDATA_object));

					if(new_COLDATA_object)
					{
					fprintf(fplog,"\n t5CreateClrschm ---------------------------> new_COLDATA_object created."); fflush(fplog);
					ITK_CALL(AOM_save(new_COLDATA_object));					
					ITK_CALL(AOM_refresh(new_COLDATA_object,0));
					}
 
					ITK_CALL( ITEM_ask_latest_rev (new_COLDATA_object,&clrdatarevtag) );
					if(clrdatarevtag)
					{
					ITK_CALL(AOM_refresh(clrdatarevtag,1));
					ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_PrtCatCode",sCompCode));
					ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_InternalSchm","T"));
					ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_ClSrl",ClrsrlCat));
					ITK_CALL(AOM_save(clrdatarevtag));
					ITK_CALL(AOM_refresh(clrdatarevtag,0));
					}

					ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
					if(ColData_relation_type != NULLTAG)
					{
						ITK_CALL(GRM_create_relation(ClrScheme_tag,clrdatarevtag,ColData_relation_type,NULLTAG,&new_relation));
						if(new_relation != NULLTAG)
							{
							ITK_CALL(GRM_save_relation(new_relation));
							}
					}
					
				}
				else
					{
						ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
						if(ColData_relation_type != NULLTAG)
							{				
								clrdatarevtag1 = qry_output[0];
								ITK_CALL(GRM_find_relation(ClrScheme_tag,clrdatarevtag1,ColData_relation_type,&del_relation));
								if(del_relation!=NULLTAG)
								{
								fprintf(fplog,"\n  FunExpPartToGetChildCompCodes relation already available. \n"); fflush(fplog);

								ITK_CALL(AOM_refresh(del_relation,0));
								ITK_CALL(AOM_lock(del_relation));
								ITK_CALL(GRM_save_relation(del_relation));
								ITK_CALL(AOM_refresh(del_relation,1));
								ITK_CALL(AOM_load(del_relation));	
								
								status=0;
								}
								else
								{

								ITK_CALL(GRM_create_relation(ClrScheme_tag,clrdatarevtag1,ColData_relation_type,NULLTAG,&new_relation));
								if(new_relation != NULLTAG)
								{
								ITK_CALL(GRM_save_relation(new_relation));
								}
								}
							}
					}
			}
	}

	// new logic

	return status;	
}
// function to replace CCA comp code with new color serial
int FunToReplaceColorSchemeData(char* ClrSufixName,char* OrgcompcodeStr)
{
	
	fprintf(fplog,"\t Please proceed with parent color suffix: [%s] OrgcompcodeStr [%s]\n",ClrSufixName,OrgcompcodeStr);fflush(fplog);
	//ClrSufixName: CCA_CENTER_AIR_VENT~CLRPRT_ASY_1
	//ClrSufixName: CCA_CENTER_AIR_VENT
	//CCA_GLOVE_BOX~CLRPRT_ASY_3
	//CCA_STEERING_WHEEL~CLRPRT_ASY_1
	char *sCompCode = NULL;
	char *sColorID	= NULL;		
	char* c = ","; 
	char* d	= ";";
	
	fprintf(fplog,"\n 77ClrSufixName [%s]\n",ClrSufixName);fflush(fplog);
	
	if(strstr(ClrSufixName,"~")!=NULL)
	{
		sCompCode =	tc_strtok(ClrSufixName,"~");
		sColorID = tc_strtok(NULL,"");
	
	if(strstr(ClrSufixName,"~")!=NULL)
	{
		fprintf(fplog,"\n AAClrSufixName [%s]\n",ClrSufixName);fflush(fplog);
		sColorID = strtok(NULL,"~");
		fprintf(fplog,"\n CCsColorID [%s]\n",sColorID);fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n 1111ClrSufixName [%s]\n",ClrSufixName);fflush(fplog);
		fprintf(fplog,"\n 1111sColorID [%s]\n",sColorID);fflush(fplog);
		//sColorID = strtok(NULL,"");
		//sColorID=(char *) MEM_alloc(50);
		//tc_strcpy(sColorID,ClrSufixName);
	}	
	}
	else
	{
		fprintf(fplog,"\n BBClrSufixName [%s]\n",ClrSufixName);fflush(fplog);
		sCompCode =	tc_strtok(ClrSufixName,"~");
		sColorID =	tc_strtok(NULL,"");
		//sColorID=(char *) MEM_alloc(50);
		//tc_strcpy(sColorID,ClrSufixName);
	}

	fprintf(fplog,"\n 77sColorID [%s]\n",sColorID);fflush(fplog);

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
	
			
	fprintf(fplog,"\n  ");fflush(fplog);
	strcpy(CID,sColorID);
	strcat(CID,"*");
	fprintf(fplog,"\n inside FunToReplaceColorSchemeData 1: CID:[%s]",CID);fflush(fplog);

	ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
	
	// Find ##;DUMMY_COLSRL comp code
		
//	if(QRY_find("ColorSchemeDataRevision", &queryTagF));
//	if (queryTagF)
//	{
//	printf("Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
//	}
//	else
//	{
//	printf("Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(stdout);
//	return status;
//	}
//	char *qry_entriesF[3] = {"Comp Code","Internal Scheme","Color Serial"};
//	char **qry_valuesF = (char **) MEM_alloc(50 * sizeof(char *));
//
//	tc_strcpy(clrsrlmain2,"");
//	tc_strcat(clrsrlmain2,"##");
//	tc_strcat(clrsrlmain2,"*");
//	tc_strcat(clrsrlmain2,"DUMMY_COLSRL");
//	printf("clrsrlmain2 --------> %s\n",clrsrlmain2);
//
//	qry_valuesF[0] = sCompCode;
//	qry_valuesF[1] = "T";
//	qry_valuesF[2] = clrsrlmain2;
//
//	ITK_CALL(QRY_execute(queryTagF, n_entriesF, qry_entriesF, qry_valuesF, &resultCountF, &qry_outputF));
//	printf("\n Count of Query Values resultCountF ------------------>  %d \n",resultCountF);
//	if(resultCountF>0)
//	{
//		ITK_CALL(GRM_find_relation(LatestRev,qry_outputF[0],ColData_relation_type,&del_relation));
//		ITK_CALL(GRM_delete_relation(del_relation));
//		ITK_CALL(AOM_load(LatestRev));	
//	}

	// end
	
	ITK_CALL(AOM_load(LatestRev));

	ITK_CALL(POM_refresh_instances(1,&LatestRev,class_to_load_as,lock_type));
	//(POM_load_instances(1,&LatestRev,class_to_load_as,lock_type));

	fprintf(fplog,"\n  ");fflush(fplog);
	fprintf(fplog,"\n inside FunToReplaceColorSchemeData  2");fflush(fplog);

	ITK_CALL(QRY_find("Colour Master", &Cntl_obj_Ctag));
	if (Cntl_obj_Ctag)
	{
		fprintf(fplog,"\n 11 Found Query Colour Master... \n");fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query Colour Master... \n");fflush(fplog);
	}

	qry_sys_valuesC[0] = CID;
	ITK_CALL(QRY_execute(Cntl_obj_Ctag, n_entry_C, qry_sys_entryC, qry_sys_valuesC, &IntCountC, &ClrSuf_tag));
	fprintf(fplog,"11 IntCountC: [%d] \n",IntCountC);fflush(fplog);

		fprintf(fplog," \n "); fflush(fplog);
		if(IntCountC>0)
		{
			ITK_CALL(AOM_UIF_ask_value(ClrSuf_tag[0],"object_name",&sClrSerSuf));
			ITK_CALL(AOM_UIF_ask_value(ClrSuf_tag[0],"t5_ColourId",&sClrID));		
			
			fprintf(fplog,"\n sClrSerSuf[%s] sClrID[%s]\n",sClrSerSuf,sClrID);fflush(fplog);
			// Function to Create Color Scheme Data and Add it in Color Scheme.
			//FunToCreClrSchData(sCompCodeS,sClrSerSuf,sClrID);
			//FunToCreClrSchData(char *sCompCodeS,char *sClrSerSuffix,char* sClrID)	

	/***********************************************************************************************/
			
			// Query Is color scheme data available for give inputs.

			fprintf(fplog,"\n sClrSerSuf[%s] sClrID[%s]\n",sClrSerSuf,sClrID);fflush(fplog);
			fprintf(fplog,"sCompCode --------> %s\n",sCompCode);fflush(fplog);
					
			if(QRY_find("ColorSchemeDataRevision", &queryTag));
			if (queryTag)
			{
			fprintf(fplog,"Found Query 'ColorSchemeDataRevision' \n\t");fflush(fplog);
			}
			else
			{
			fprintf(fplog,"Not Found Query 'ColorSchemeDataRevision' \n\t"); fflush(fplog);
			return status;
			}
			char *qry_entries4[3] = {"Comp Code","Internal Scheme","Color Serial"};
			char **qry_values = (char **) MEM_alloc(50 * sizeof(char *));

			tc_strcpy(clrsrlmain1,"");
			tc_strcat(clrsrlmain1,sClrSerSuf);
			tc_strcat(clrsrlmain1,"*");
			tc_strcat(clrsrlmain1,sClrID);
			fprintf(fplog,"clrsrlmain1 --------> %s\n",clrsrlmain1);fflush(fplog);

			qry_values[0] = sCompCode ;
			qry_values[1] = "T" ;
			qry_values[2] = clrsrlmain1 ;

			ITK_CALL(QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &qry_output));
			fprintf(fplog,"\n Count of Query Values ------------------>  %d \n",resultCount);fflush(fplog);
			
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
				tc_strcat(item_id_ColData,"T");
				tc_strcat(item_id_ColData,"`");
				tc_strcat(item_id_ColData,dupintrnalscheme);

				item_id_ColDatamain = replaceWord(item_id_ColData, c, d);
				fprintf(fplog,"\n**************** item_id_ColDatamain: %s  ***************\n",item_id_ColDatamain);fflush(fplog);

				ITK_CALL(TCTYPE_find_type("T5_ClrShmData", NULL, &Colordata_type_tag));   

				ITK_CALL(TCTYPE_construct_create_input(Colordata_type_tag, &object_create_input_tag1));

				ITK_CALL(AOM_set_value_string(object_create_input_tag1,"object_name",item_id_ColDatamain));
				ITK_CALL(AOM_set_value_string(object_create_input_tag1,"item_id",item_id_ColDatamain));

				ITK_CALL(TCTYPE_create_object(object_create_input_tag1, &new_COLDATA_object));

				if(new_COLDATA_object)
				{
				fprintf(fplog,"\n t5CreateClrschm ---------------------------> new_COLDATA_object created.");fflush(fplog);
				ITK_CALL(AOM_save(new_COLDATA_object));					
				ITK_CALL(AOM_refresh(new_COLDATA_object,0));
				}

				ITK_CALL( ITEM_ask_latest_rev (new_COLDATA_object,&clrdatarevtag) );
				if(clrdatarevtag)
				{
				ITK_CALL(AOM_refresh(clrdatarevtag,1));
				ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_PrtCatCode",sCompCode));
				ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_InternalSchm","T"));
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
					fprintf(fplog,"\n Inside else of relation creation if not available \n");fflush(fplog);
					//ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
					if(ColData_relation_type != NULLTAG)
						{				
						clrdatarevtag1 = qry_output[0];
						ITK_CALL(GRM_find_relation(LatestRev,clrdatarevtag1,ColData_relation_type,&del_relation));
						if(del_relation!=NULLTAG)
							{
								fprintf(fplog,"\n relation already available. \n");fflush(fplog);
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

int FunToEntryDataInFIXTable(char *compcodeStr, char** CombCompCodeArr,char** AllCompCodes,int* suffixInt)
{
	int status=0;
	
	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int		l					=	0;
	int     n_entry				=	1;
	char    *qry_sys_entry[1]	=	{"Place Holder"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCount			=	0;
	tag_t   *PrntClrSuf_tag		=	NULLTAG;
	char*	Obj_name			=	NULL;
	char*	ChildCompCode			=	NULL;
	char* d	= ";";
	char* e  = "*"; 

	tag_t	Clr_type_tag				= NULLTAG;
	tag_t	object_create_input_tag		= NULLTAG;
	tag_t	Chd_type_tag				= NULLTAG;
	tag_t	object_create_chd_tag		= NULLTAG;
	tag_t	ColSufTag					= NULLTAG;
	tag_t	ChdColSufTag				= NULLTAG;
	tag_t	rev_type_tag				= NULLTAG;
	char*	ModuleSyscdStr				= NULL;
	tag_t   Cntl_obj_tag		=	NULLTAG;
	int     n					=	2;
	char    *qry_entry[2]		=	{"Name","Type"};
	char    **qry_values		=  (char **) MEM_alloc(10 * sizeof(char *));
	int		Count				=	0;
	tag_t   *ChildClrSuf_tag    =	NULLTAG;

	int 	num_to_sort			=	1;
	char	*keys[1]			=	{"creation_date"};
	int		orders				=	1;
	int		Count4				=	0;
	tag_t	*qry_tag_sort		=	NULLTAG;
	int		ii					=	0;
	char*	SclrSuf_Name		=	NULL;
	char*	SplcHolder			=	NULL;

	char	*SplcHoldr			=	NULL;
	char	*SClrID				=	NULL;
	char	*subColID			=	NULL;		
	int		IntsubColID			=	0;
	char	SsubColID[50];
	char	*SclrSuf_Nm			=	NULL;
	char	*subSuf				=	NULL;
	char	*ScatSuf			=	NULL;
	char	*ObjName			=	NULL;
	char	*SplcHolSuf			=	NULL;


	Obj_name=(char *) MEM_alloc(100);
	ChildCompCode=(char *) MEM_alloc(100);
	SclrSuf_Nm=(char *) MEM_alloc(100);
	ObjName=(char *) MEM_alloc(100);
	ScatSuf=(char *) MEM_alloc(100);

	fprintf(fplog,"\n Inside Function FunToEntryDataInFIXTable \n");fflush(fplog);

	fprintf(fplog,"\n compcodeStr:[%s] suffixInt:[%d]\n",compcodeStr,*suffixInt);fflush(fplog);

	// query parent fix table data
	
	(QRY_find("Color_Suffix_Query", &Cntl_obj_Qtag));
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n -- Found Query General... \n");fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query General... \n");fflush(fplog);
	}

	qry_sys_values[0] = compcodeStr;
	(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &PrntClrSuf_tag));
	fprintf(fplog," \n Inside Function FunToEntryDataInFIXTable Parent color suffix count IntCount: [%d]\n",IntCount);fflush(fplog);
	if(IntCount==0)
	{
		// Create Color Suffix with below details if it is first entry:
		// object_name: compcodeStr~CLRPRT_ASY
		// t5_PlaceHolder: compcodeStr
		// t5_PlaceHolderSuffix: ZZ

		strcpy(Obj_name,"");
		strcat(Obj_name,compcodeStr);
		strcat(Obj_name,"~");
		strcat(Obj_name,"CLRPART_ASSY");
		ITK_CALL(TCTYPE_find_type("T5_ColorSuffix", NULL, &Clr_type_tag));   
		ITK_CALL(TCTYPE_construct_create_input(Clr_type_tag, &object_create_input_tag));

		ITK_CALL(AOM_set_value_string(object_create_input_tag,"object_name",Obj_name));
		ITK_CALL(TCTYPE_create_object(object_create_input_tag, &ColSufTag));

		if(ColSufTag)
		{
			fprintf(fplog,"\n t5CreateObject : object created.");fflush(fplog);
			ITK_CALL(AOM_save(ColSufTag));
			ITK_CALL(AOM_refresh(ColSufTag,0));

			ITK_CALL(AOM_refresh(ColSufTag,1));

			ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolder",compcodeStr));		
			ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolderSuffix","ZZ"));

			ITK_CALL(AOM_save(ColSufTag));
			ITK_CALL(AOM_refresh(ColSufTag,1));

		
			fprintf(fplog,"\n suffixInt:[%d]\n",*suffixInt);fflush(fplog);
		
			if(*suffixInt>0)
			{
			
			fprintf(fplog,"\n suffixInt11:[%d]\n",*suffixInt);fflush(fplog);

			for(l=0;l<*suffixInt;l++)
				{
				fprintf(fplog,"\n suffixInt22:[%d]\n",*suffixInt);fflush(fplog);
				fprintf(fplog,"\n Child Color Suffix: [%s]\n",CombCompCodeArr[l]);fflush(fplog);
				
				ITK_CALL(QRY_find("General...", &Cntl_obj_tag));
				if (Cntl_obj_tag)
				{
					fprintf(fplog,"\n -- Found Query General... \n");fflush(fplog);
				}
				else
				{
					fprintf(fplog,"\n Not Found Query General... \n");fflush(fplog);
				}
				fprintf(fplog,"\n CombCompCodeArr [%s] \n",CombCompCodeArr[l]);fflush(fplog);
				
				ChildCompCode = replaceWord(CombCompCodeArr[l], d, e);
								
				qry_values[0] = ChildCompCode;
				qry_values[1] = "Child Color Suffix" ;
				ITK_CALL(QRY_execute(Cntl_obj_tag, n, qry_entry, qry_values, &Count, &ChildClrSuf_tag));
				fprintf(fplog," \n Child Color Suffix Count:[%d] \n",Count); fflush(fplog);
				if(Count>0)
				{
					// create relation between color suffix and child color suffix.
					fprintf(fplog,"\n create relation between color suffix and child color suffix. \n");fflush(fplog);
					if(FL_insert(ColSufTag,ChildClrSuf_tag[0],99));
					if(AOM_save(ColSufTag));

				}
				else
				{
					// create child color suffix
					ITK_CALL(TCTYPE_find_type("T5_Child_Color_Suffix", NULL, &Chd_type_tag));   
					ITK_CALL(TCTYPE_construct_create_input(Chd_type_tag, &object_create_chd_tag));
					ITK_CALL(AOM_set_value_string(object_create_chd_tag,"object_name",CombCompCodeArr[l]));					
					ITK_CALL(TCTYPE_create_object(object_create_chd_tag, &ChdColSufTag));
					if(ChdColSufTag)
					{
						fprintf(fplog,"\n t5CreateObject : object created.");fflush(fplog);
						ITK_CALL(AOM_save(ChdColSufTag));
						ITK_CALL(AOM_refresh(ChdColSufTag,0));


					}
					//CombCompCodeArr[l] = ChdColSufTag;
					
					if(FL_insert(ColSufTag,ChdColSufTag,99));
					if(AOM_save(ColSufTag));	

				}

				// Create relation betwn color suffix and child color suffix.

			}
		}
		else
		{
		fprintf(fplog,"\n ELSE \n");fflush(fplog);
		}
		}
	}
//	else if(IntCount==1)
//	{
//		// Create Color Suffix with below details if it is first entry:
//		// object_name: compcodeStr~'CLRPRT_ASY_1'
//		// t5_PlaceHolder: compcodeStr
//		// t5_PlaceHolderSuffix: 'ZY'
//		
//		
//		strcpy(Obj_name,"");
//		strcat(Obj_name,compcodeStr);
//		strcat(Obj_name,"~");
//		strcat(Obj_name,"CLRPRT_ASY_1");
//		ITK_CALL(TCTYPE_find_type("T5_ColorSuffix", NULL, &Clr_type_tag));   
//		ITK_CALL(TCTYPE_construct_create_input(Clr_type_tag, &object_create_input_tag));
//
//		ITK_CALL(AOM_set_value_string(object_create_input_tag,"object_name",Obj_name));
//		ITK_CALL(TCTYPE_create_object(object_create_input_tag, &ColSufTag));
//
//		if(ColSufTag)
//		{
//			printf("\n t5CreateObject : object created.");fflush(stdout);
//			ITK_CALL(AOM_save(ColSufTag));
//			ITK_CALL(AOM_refresh(ColSufTag,0));
//
//			ITK_CALL(AOM_refresh(ColSufTag,1));
//
//			ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolder",compcodeStr));		
//			ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolderSuffix","ZY"));
//
//			ITK_CALL(AOM_save(ColSufTag));
//			ITK_CALL(AOM_refresh(ColSufTag,1));
//
//		
//			printf("\n suffixInt:[%d]\n",*suffixInt);fflush(stdout);
//		
//			if(*suffixInt>0)
//			{
//			
//			printf("\n suffixInt11:[%d]\n",*suffixInt);fflush(stdout);
//
//			for(l=0;l<*suffixInt;l++)
//				{
//				printf("\n suffixInt22:[%d]\n",*suffixInt);fflush(stdout);
//				printf("\n Child Color Suffix: [%s]\n",CombCompCodeArr[l]);fflush(stdout);
//				
//				ITK_CALL(QRY_find("General...", &Cntl_obj_tag));
//				if (Cntl_obj_tag)
//				{
//					printf("\n -- Found Query General... \n");fflush(stdout);
//				}
//				else
//				{
//					printf("\n Not Found Query General... \n");fflush(stdout);
//				}
//				printf("\n CombCompCodeArr [%s] \n",CombCompCodeArr[l]);fflush(stdout);
//				
//				ChildCompCode = replaceWord(CombCompCodeArr[l], d, e);
//								
//				qry_values[0] = ChildCompCode;
//				qry_values[1] = "Child Color Suffix" ;
//				ITK_CALL(QRY_execute(Cntl_obj_tag, n, qry_entry, qry_values, &Count, &ChildClrSuf_tag));
//				printf(" \n Child Color Suffix Count:[%d] \n",Count); fflush(stdout);
//				if(Count>0)
//				{
//					// create relation between color suffix and child color suffix.
//					printf("\n create relation between color suffix and child color suffix. \n");fflush(stdout);
//					if(FL_insert(ColSufTag,ChildClrSuf_tag[0],99));
//					if(AOM_save(ColSufTag));
//
//				}
//				else
//				{
//					// create child color suffix
//					ITK_CALL(TCTYPE_find_type("T5_Child_Color_Suffix", NULL, &Chd_type_tag));   
//					ITK_CALL(TCTYPE_construct_create_input(Chd_type_tag, &object_create_chd_tag));
//					ITK_CALL(AOM_set_value_string(object_create_chd_tag,"object_name",CombCompCodeArr[l]));					
//					ITK_CALL(TCTYPE_create_object(object_create_chd_tag, &ChdColSufTag));
//					if(ChdColSufTag)
//					{
//						printf("\n t5CreateObject : object created.");
//						ITK_CALL(AOM_save(ChdColSufTag));
//						ITK_CALL(AOM_refresh(ChdColSufTag,0));
//
//
//					}
//					//CombCompCodeArr[l] = ChdColSufTag;					
//					if(FL_insert(ColSufTag,ChdColSufTag,99));
//					if(AOM_save(ColSufTag));	
//				}
//			}
//		}
//		else
//		{
//		printf("\n ELSE \n");fflush(stdout);
//		}
//		}
//
//	}
	else if(IntCount>=1)
	{
		// Create Color Suffix with below details if it is first entry:
		// object_name: compcodeStr~'Incremental_CLRPRT'
		// t5_PlaceHolder: compcodeStr
		// t5_PlaceHolderSuffix: 'Incremental_suffix'
		int IntSub[1000];
		int min=0;
		int flag_n=0;
		char *StrSub=NULL;
		char *StrSuf = NULL;
		char *LtstCompCode = NULL;
		char *LtstCompCodeData = NULL;
		char *NewLtstCompCodeData = NULL;
		char *sNewObjName = NULL;
		StrSuf=(char *)MEM_alloc(100);
		char *StrSufStr = NULL;
		StrSufStr=(char *)MEM_alloc(100);
		NewLtstCompCodeData=(char *)MEM_alloc(200);
		QRY_execute_with_sort(Cntl_obj_Qtag,n_entry,qry_sys_entry,qry_sys_values,num_to_sort,keys,&orders,&Count4,&qry_tag_sort);
		
		for (min=0;min<Count4;min++)
		{		
		AOM_ask_value_string(qry_tag_sort[min],"object_name",&SclrSuf_Name);
		fprintf(fplog,"\n :: SclrSuf_Name is :: %s\n",SclrSuf_Name);fflush(fplog);
		AOM_ask_value_string(qry_tag_sort[min],"t5_PlaceHolder",&SplcHolder);
		fprintf(fplog,"\n :: SplcHolder is :: %s\n",SplcHolder);fflush(fplog);
		AOM_ask_value_string(qry_tag_sort[min],"t5_PlaceHolderSuffix",&SplcHolSuf);
		fprintf(fplog,"\n :: SplcHolSuf is :: %s\n",SplcHolSuf);fflush(fplog);
		//CLRPRT_ASY_1
//		StrSub=subString(SplcHolSuf,1,1);
//		printf("\n :: StrSub : [%c]\n",*StrSub);fflush(stdout);
//		IntSub[min] = *StrSub;		
		
		}
//		int intSuf = returnmin(IntSub,Count4);
//		printf("\n :: intSuf is :: %d\n",intSuf);fflush(stdout);//		
//		sprintf(StrSuf, "%c",intSuf);//		
//		printf("\n :: StrSuf is :: %c\n",StrSuf);fflush(stdout);//
//		strcpy(StrSufStr,"");
//		strcpy(StrSufStr,"Z");
//		strcat(StrSufStr,StrSuf);
		int NFlag =0;
		NFlag = funToGetLatClrSufData(compcodeStr,CombCompCodeArr,*suffixInt,&LtstCompCode);
		//if LtstCompCode = CCA_CENTER_AIR_VENT~CLRPRT_ASY_1
		//if NFlag ==0 then arrays are equal and return LtstCompCode value.		
		//if NFlag ==1 then count are not same or arrays are not equal, 
		// Increment value of color serial with ~+1 as its child color suffix having same comp code as of color scheme.

		fprintf(fplog,"\n :: LtstCompCode is :: %s\n",LtstCompCode);fflush(fplog);

		if(NFlag==0)
		{
			fprintf(fplog,"\n NFlag is: %d , so return status as arrays are equal and return LtstCompCode value.	\n");fflush(fplog);
			FunToReplaceColorSchemeData(LtstCompCode,compcodeStr);			
			status=1;
			fprintf(fplog,"\n :: closed function when NFlag=0 FunToReplaceColorSchemeData with status %d \n",status);fflush(fplog);
			return status;
		}
		else if((NFlag==1) && tc_strcmp(LtstCompCode,"NOTAVAIL")==0)
		{
			// count is not equal, so add entry in fix table for new color suffix and child color suffix.
			funToEntryInFixTable(compcodeStr,CombCompCodeArr,*suffixInt,&LtstCompCode);
			StrSufStr = funToGetPlaceHolderSuffix(LtstCompCode);
			SplcHoldr = tc_strtok(LtstCompCode,"~");
			SClrID	  = tc_strtok(NULL,"");
		}
		else if(NFlag==3)
		{
			fprintf(fplog,"\n  Child BOMline does not have comp code \n");fflush(fplog);
			status=3;
			return status;
		}
		else
		{
			FunToGetNewObjName(LtstCompCode,&LtstCompCodeData);
			fprintf(fplog,"\n :: LtstCompCodeData is :: %s\n",LtstCompCodeData);fflush(fplog);
			strcpy(NewLtstCompCodeData,LtstCompCodeData);
			funToNewEntryInFixTable(NewLtstCompCodeData,CombCompCodeArr,*suffixInt,&LtstCompCode);
			fprintf(fplog,"\n ::LtstCompCodeData [%s] LtstCompCode is :: %s\n",LtstCompCodeData,LtstCompCode);fflush(fplog);
			FunToReplaceColorSchemeData(LtstCompCodeData,compcodeStr);
			//StrSufStr = FunToGetColorID(LtstCompCodeData);
			status=2;
			fprintf(fplog,"\n :: closed function FunToReplaceColorSchemeData when NFlag is other than 0,1,3 with status %d \n",status);fflush(fplog);
			//printf("\n :: _StrSufStr is :: %s  LtstCompCodeData  %s \n",StrSufStr,LtstCompCodeData);fflush(stdout);
		// if StrSufStr=ZX Decrement in place holder suffix [ex. if ZX then it should be ZW] 
			//SplcHoldr = tc_strtok(LtstCompCodeData,"~");
			//SClrID	  = tc_strtok(NULL,"");
			NFlag =1; 

		}
		fprintf(fplog,"\n closing function FunToEntryDataInFIXTable \n");fflush(fplog);

		
		// try to compare values of color suffix and its child present with comp codes only and create entries accorrdingly...

		//printf("\n SclrSuf_Name [%s]\n SplcHoldr [%s]\n SClrID is [%s] \n strlen(SClrID)[%d]",SclrSuf_Name,SplcHoldr,SClrID,strlen(SClrID));fflush(stdout);
		
//		if(strlen(SClrID)==12)
//		{
//			if(NFlag ==2)
//			{
//			subColID=subString(SClrID,11,1);//CLRPRT_ASY_1
//			printf("\n :: subColID is :: %s\n",subColID);fflush(stdout);
//			IntsubColID = atoi(subColID);
//			printf("\n :: IntsubColID is :: %d\n",IntsubColID);fflush(stdout);
//			IntsubColID = IntsubColID+1;
//			printf("\n :: IntsubColID after append :: %d\n",IntsubColID);fflush(stdout);			
//			sprintf(SsubColID, "%d",IntsubColID);
//			strcpy(SclrSuf_Nm,"");
//			strcat(SclrSuf_Nm,"CLRPRT_ASY");
//			strcat(SclrSuf_Nm,"_");
//			strcat(SclrSuf_Nm,SsubColID);
//			printf("\n :: SclrSuf_Nm : [%s]\n",SclrSuf_Nm);fflush(stdout);
//			strcpy(ObjName,"");
//			strcat(ObjName,SplcHoldr);
//			strcat(ObjName,"~");
//			strcat(ObjName,SclrSuf_Nm);
//
//			printf("\n :: ObjName : [%s] SplcHoldr [%s]\n",ObjName,SplcHoldr);fflush(stdout);
//
//			// place holder suffix
//
//			subSuf=subString(StrSufStr,1,1);
//			printf("\n :: subSuf : [%c]\n",*subSuf);fflush(stdout);
//			*subSuf = *subSuf -1;			
//			printf("\n :: subSuf : [%c]\n",*subSuf);fflush(stdout);			
//			strcpy(ScatSuf,"");
//			strcat(ScatSuf,"Z");
//			strcat(ScatSuf,subSuf);
//			printf("\n :: ScatSuf : [%s]\n",ScatSuf);fflush(stdout);
//
//			}
//			else if(NFlag ==0)
//			{
//			//subColID=subString(SClrID,11,1);//CLRPRT_ASY_1
//			//printf("\n :: subColID is :: %s\n",subColID);fflush(stdout);
//			//IntsubColID = atoi(subColID);
//			//printf("\n :: IntsubColID is :: %d\n",IntsubColID);fflush(stdout);
//			//IntsubColID = IntsubColID+1;
//			//printf("\n :: IntsubColID after append :: %d\n",IntsubColID);fflush(stdout);			
//			//sprintf(SsubColID, "%d",IntsubColID);
//			strcpy(SclrSuf_Nm,"");
//			strcat(SclrSuf_Nm,SClrID);
//			strcat(SclrSuf_Nm,"~");
//			strcat(SclrSuf_Nm,"1");// increment here and check if it is present then re increment it.
//			printf("\n :: SclrSuf_Nm : [%s]\n",SclrSuf_Nm);fflush(stdout);//CLRPRT_ASY_1~1
//			strcpy(ObjName,"");
//			strcat(ObjName,SplcHoldr);
//			strcat(ObjName,"~");
//			strcat(ObjName,SclrSuf_Nm);
//			
//			printf("\n :: calling FunToGetNewObjName: ObjName : [%s]\n",ObjName);fflush(stdout);
//			
//			flag_n = FunToGetNewObjName(ObjName,&sNewObjName);
//
//			printf("\n ::flag_n[%d] sNewObjName : [%s] SplcHoldr [%s]\n",flag_n,sNewObjName,SplcHoldr);fflush(stdout);
////			
////			switch(flag_n)
////				{
////				case 1:
////				flag_n = FunToGetNewObjName(ObjName,&sNewObjName);
////				if(flag_n==1)
////					{
////					break;
////					}
////				}
//			// place holder suffix		
//			strcpy(ScatSuf,"");			
//			strcat(ScatSuf,StrSufStr);
//			printf("\n :: ScatSuf : [%s]\n",ScatSuf);fflush(stdout);
//
//			}		
//
//			// Create Color Suffix in FIX Table with ObjName and ScatSuf
//			
//			ITK_CALL(TCTYPE_find_type("T5_ColorSuffix", NULL, &Clr_type_tag));   
//			ITK_CALL(TCTYPE_construct_create_input(Clr_type_tag, &object_create_input_tag));
//
//			ITK_CALL(AOM_set_value_string(object_create_input_tag,"object_name",sNewObjName));
//			ITK_CALL(TCTYPE_create_object(object_create_input_tag, &ColSufTag));
//
//			if(ColSufTag)
//			{
//			printf("\n t5CreateObject : object created.");fflush(stdout);
//			ITK_CALL(AOM_save(ColSufTag));
//			ITK_CALL(AOM_refresh(ColSufTag,0));
//
//			ITK_CALL(AOM_refresh(ColSufTag,1));
//
//			ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolder",SplcHoldr));		
//			ITK_CALL(AOM_set_value_string(ColSufTag,"t5_PlaceHolderSuffix",ScatSuf));
//
//			ITK_CALL(AOM_save(ColSufTag));
//			ITK_CALL(AOM_refresh(ColSufTag,1));
//
//			printf("\n suffixInt:[%d]\n",*suffixInt);fflush(stdout);
//		
//			if(*suffixInt>0)
//			{
//			
//			printf("\n suffixInt11:[%d]\n",*suffixInt);fflush(stdout);
//
//			for(l=0;l<*suffixInt;l++)
//				{
//				printf("\n suffixInt22:[%d]\n",*suffixInt);fflush(stdout);
//				printf("\n Child Color Suffix: [%s]\n",CombCompCodeArr[l]);fflush(stdout);
//				
//				ITK_CALL(QRY_find("General...", &Cntl_obj_tag));
//				if (Cntl_obj_tag)
//				{
//					printf("\n -- Found Query General... \n");fflush(stdout);
//				}
//				else
//				{
//					printf("\n Not Found Query General... \n");fflush(stdout);
//				}
//				printf("\n CombCompCodeArr [%s] \n",CombCompCodeArr[l]);fflush(stdout);
//				
//				ChildCompCode = replaceWord(CombCompCodeArr[l], d, e);
//								
//				qry_values[0] = ChildCompCode;
//				qry_values[1] = "Child Color Suffix" ;
//				ITK_CALL(QRY_execute(Cntl_obj_tag, n, qry_entry, qry_values, &Count, &ChildClrSuf_tag));
//				printf(" \n Child Color Suffix Count:[%d] \n",Count); fflush(stdout);
//				if(Count>0)
//				{
//					// create relation between color suffix and child color suffix.
//					printf("\n create relation between color suffix and child color suffix. \n");fflush(stdout);
//					if(FL_insert(ColSufTag,ChildClrSuf_tag[0],99));
//					if(AOM_save(ColSufTag));
//
//				}
//				else
//				{
//				// create child color suffix
//				ITK_CALL(TCTYPE_find_type("T5_Child_Color_Suffix", NULL, &Chd_type_tag));   
//				ITK_CALL(TCTYPE_construct_create_input(Chd_type_tag, &object_create_chd_tag));
//				ITK_CALL(AOM_set_value_string(object_create_chd_tag,"object_name",CombCompCodeArr[l]));					
//				ITK_CALL(TCTYPE_create_object(object_create_chd_tag, &ChdColSufTag));
//				if(ChdColSufTag)
//				{
//					printf("\n t5CreateObject : object created.");
//					ITK_CALL(AOM_save(ChdColSufTag));
//					ITK_CALL(AOM_refresh(ChdColSufTag,0));
//
//
//				}
//				//CombCompCodeArr[l] = ChdColSufTag;
//
//				if(FL_insert(ColSufTag,ChdColSufTag,99));
//				if(AOM_save(ColSufTag));	
//
//				}
//				// Create relation betwn color suffix and child color suffix.
//			}
//			}
//			}
//		}
//		else
//		{
//			printf("\n SClrID is null");
//		}
	}

	return status;
}

int FunToGetParentCompCode(char *sCombCompCodeClrID, char** Psuffix)
{
	int status =0;

	tag_t   Cntl_obj_Qtag		=	NULLTAG;
	int     n_entry				=	2;
	char    *qry_sys_entry[2]	=	{"Name","Type"};
	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
	int		IntCount			=	0;
	int		IsValidFlag			=	0;
	tag_t   *ChildClrSuf_tag    =	NULLTAG;

	int		n_refs				=	0;
	int		j=0,cnt=0;
	int*	levels;
	tag_t* 	referencers			=	NULLTAG;
	char ** relations			=	NULL;
	tag_t	objTypeRefTag		=	NULLTAG;
	char sParentClrSuffix[TCTYPE_name_size_c+1];

	char *sCompCode				=	NULL;
	char *sColorID				=	NULL;
	char *sParentClrSufName		=	NULL;
	char **sChldClrSufixvalues	=	NULL;
	*Psuffix = malloc(10*sizeof(char));
	char **sListOfChildSuffix = (char **) MEM_alloc(500 * sizeof(char *));
	
	fprintf(fplog,"\n ");fflush(fplog);

	ITK_CALL(QRY_find("General...", &Cntl_obj_Qtag));
	if (Cntl_obj_Qtag)
	{
		fprintf(fplog,"\n -- Found Query General... \n");fflush(fplog);
	}
	else
	{
		fprintf(fplog,"\n Not Found Query General... \n");fflush(fplog);
	}
	fprintf(fplog,"\n sCombCompCodeClrID [%s] \n",sCombCompCodeClrID);fflush(fplog);
	
	qry_sys_values[0] = sCombCompCodeClrID;
	qry_sys_values[1] = "Child Color Suffix" ;
	ITK_CALL(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &ChildClrSuf_tag));
	fprintf(fplog," \n "); fflush(stdout);
	if(IntCount>0)
	{
		fprintf(fplog,"IntCount: %d",IntCount);fflush(fplog);		

		ITK_CALL(WSOM_where_referenced(ChildClrSuf_tag[0],1,&n_refs,&levels,&referencers,&relations));
		
		int	num_values =0;
		char **	values = NULL;

		if(n_refs==1)
		{
			//object_name
			ITK_CALL(AOM_UIF_ask_value(referencers[0],"object_name",&sParentClrSufName));
		
			//sCompCode =	tc_strtok(sParentClrSufName,"~");
			//sColorID = tc_strtok(NULL,"~");
			
			fprintf(fplog,"\n sParentClrSufName [%s]\n",sParentClrSufName);fflush(fplog);
			//printf("\n sColorID [%s]\n",sColorID);fflush(stdout);

			tc_strcpy(*Psuffix,sParentClrSufName);

			// Function to find color master using color ID
			//FunToGetColorID(sColorID);
			
//			ITK_CALL(AOM_UIF_ask_values(referencers[0],"contents",&num_values,&sChldClrSufixvalues));	
//
//			printf("num_values: [%d]\n",num_values);
//			for (cnt=0;cnt<num_values;cnt++)
//			{
//				//printf("\n count is[%d]:sChldClrSufixvalues[%s]\n",cnt,sChldClrSufixvalues[cnt]);fflush(stdout);
//				sListOfChildSuffix[cnt]=(char *) MEM_alloc(tc_strlen(sChldClrSufixvalues[cnt])* sizeof(char ));
//				tc_strcpy(sListOfChildSuffix[cnt],sChldClrSufixvalues[cnt]);
//				printf("%s\n",sListOfChildSuffix[cnt]);
//
//			}
		}
		else if(n_refs>1)
		{
		for (j=0;j<n_refs;j++)
		{
			ITK_CALL(TCTYPE_ask_object_type(referencers[j],&objTypeRefTag));
			ITK_CALL(TCTYPE_ask_name(objTypeRefTag,sParentClrSuffix));
			fprintf(fplog,"\n sParentClrSuffix :%s\n",sParentClrSuffix);fflush(fplog);
		}
		}				
	}
	

	return status;
}
//int FunToCheckChilds(char *sPrntCompCode,char **ChldSuffix,int* ChldsuffixCnt)
//{
//	int status =0;
//	int j=0;
//	tag_t   Cntl_obj_Qtag		=	NULLTAG;
//	int     n_entry				=	2;
//	char    *qry_sys_entry[2]	=	{"Name","Type"};
//	char    **qry_sys_values    =  (char **) MEM_alloc(10 * sizeof(char *));
//	int		IntCount			=	0;
//	int		IsValidFlag			=	0;
//	tag_t   *ChildClrSuf_tag    =	NULLTAG;
//	int		cnt					=	0;
//	int		num_values			=	0;
//	char **sChldClrSufixvalues	=	NULL;
//	**ChldSuffix = (char **) MEM_alloc(500 * sizeof(char *));
//	//	printf("\n sPrntCompCode: [%s] ChilDCompCodeArrayCnt:[%d]\n",sPrntCompCode,ChilDCompCodeArrayCnt);
//		
//	//	for (j=0;j<ChilDCompCodeArrayCnt;j++)
//	//	{
//	//		printf("\n ChilDCompCodeArray: [%s]\n",ChilDCompCodeArray[j]);
//	//	}
//	printf("\n ");fflush(stdout);
//
//	ITK_CALL(QRY_find("General...", &Cntl_obj_Qtag));
//	if (Cntl_obj_Qtag)
//	{
//	printf("\n 111Found Query General... \n");fflush(stdout);
//	}
//	else
//	{
//	printf("\n 111Not Found Query General... \n");fflush(stdout);
//	}
//
//	qry_sys_values[0] = sPrntCompCode;
//	qry_sys_values[1] = "Color Suffix" ;
//	ITK_CALL(QRY_execute(Cntl_obj_Qtag, n_entry, qry_sys_entry, qry_sys_values, &IntCount, &ChildClrSuf_tag));
//	printf(" \n "); fflush(stdout);
//	if(IntCount>0)
//	{
//	ITK_CALL(AOM_UIF_ask_values(ChildClrSuf_tag[0],"contents",&num_values,&sChldClrSufixvalues));	
//
//	printf("num_values: [%d]\n",num_values);
//	for (cnt=0;cnt<num_values;cnt++)
//	{
//	*ChldsuffixCnt  = *ChldsuffixCnt+1;
//	printf("\n count is[%d]:sChldClrSufixvalues[%s]\n",cnt,sChldClrSufixvalues[cnt]);fflush(stdout);
//	ChldSuffix[cnt]=(char *) MEM_alloc(tc_strlen(sChldClrSufixvalues[cnt])* sizeof(char ));
//	tc_strcpy(ChldSuffix[cnt],sChldClrSufixvalues[cnt]);
//	printf("%s\n",ChldSuffix[cnt]);
//
//	}
//	}
//
//
//	return status;
//}

/*int FunToCreClrSchData(char *sCompCodeS,char *sClrSerSuffix,char* sClrID)
{
	int		status		=	0;
	int		n_entries	=	3;
	tag_t	queryTag	=	NULLTAG;
	char*	clrsrlmain1	=	NULL;
	int		resultCount	=	0;
	tag_t	*qry_output	=	NULLTAG;
	char *item_id_ColDatamain=NULL;
	char* dupintrnalscheme=NULL;
	char* c = ","; 
	char* d	= ";";
	char* e  = "/"; 
	char* f  = "-";
	dupintrnalscheme=(char *)MEM_alloc(100);

	char* item_id_ColData=NULL;
	item_id_ColData=(char *)MEM_alloc(100);

	clrsrlmain1=(char *)MEM_alloc(100);
	// Query Is color scheme data available for give inputs.

	printf("\n sClrSerSuffix[%s] sClrID[%s]\n",sClrSerSuffix,sClrID);fflush(stdout);
	printf("sCompCodeS --------> %s\n",sCompCodeS);
			
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
		tc_strcat(clrsrlmain1,sClrSerSuffix);
		tc_strcat(clrsrlmain1,"*");
		tc_strcat(clrsrlmain1,sClrID);
		printf("clrsrlmain1 --------> %s\n",clrsrlmain1);

		qry_values[0] = sCompCodeS ;
		qry_values[1] = "Y" ;
		qry_values[2] = clrsrlmain1 ;

		ITK_CALL(QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &qry_output));
		printf("\n Count of Query Values ------------------>  %d \n",resultCount);

		if (resultCount == 0)
		{
			tag_t object_create_input_tag1		=	NULLTAG;
			tag_t Colordata_type_tag			=	NULLTAG;
			tag_t new_COLDATA_object			=	NULLTAG;	
			tag_t clrdatarevtag					=	NULLTAG;	
			tag_t ColData_relation_type			=	NULLTAG;
			tag_t new_relation					=	NULLTAG;

			tc_strcpy(dupintrnalscheme,"");
			tc_strcat(dupintrnalscheme,sClrID);
			tc_strcat(dupintrnalscheme,";");
			tc_strcat(dupintrnalscheme,sClrSerSuffix);

			tc_strcpy(item_id_ColData,"");
			tc_strcat(item_id_ColData,sCompCodeS);
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
			ITK_CALL(AOM_unlock(new_COLDATA_object));
			}

			ITK_CALL( ITEM_ask_latest_rev (new_COLDATA_object,&clrdatarevtag) );
			if(clrdatarevtag)
			{
			ITK_CALL(AOM_refresh(clrdatarevtag,1));
			ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_PrtCatCode",sCompCodeS));
			ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_InternalSchm","Y"));
			ITK_CALL(AOM_set_value_string(clrdatarevtag,"t5_ClSrl",dupintrnalscheme));
			ITK_CALL(AOM_save(clrdatarevtag));
			ITK_CALL(AOM_refresh(clrdatarevtag,0));
			}

			ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
			if(ColData_relation_type != NULLTAG)
			{
				ITK_CALL(GRM_create_relation(clrrevtag,clrdatarevtag,ColData_relation_type,NULLTAG,&new_relation));
				if(new_relation != NULLTAG)
					{
					ITK_CALL(GRM_save_relation(new_relation));
					}
			}
				
		}
		else
		{
			ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&ColData_relation_type));
			if(ColData_relation_type != NULLTAG)
				{				
				clrdatarevtag1 = qry_output[0];
				ITK_CALL(GRM_create_relation(clrrevtag,clrdatarevtag1,ColData_relation_type,NULLTAG,&new_relation));
				if(new_relation != NULLTAG)
					{
					ITK_CALL(GRM_save_relation(new_relation));
					}
				}
		}

	// Create color scheme data with inputs as comp code,internal scheme,Color serial
		
	
	return status;
}*/
//static void ExpandMultiLevelBom (tag_t bom_line_tag, int depth, FILE *fptr,FILE *fptrP,FILE *fptrC,FILE *fptrA,char* sbl_item_id,char* sbl_rev_id,char* sParentCompCode,char* sParentColID)
//{
//	int ifail;
//	int iChildItemTag;
//	int status;
//	int cnt=0;
//	int cnt2=0;
//	int referencenumberfound=0;
//	char *name, *sequence_no;
//	char *Item_id_par=NULL;
//	char *Item_LatestRevSeq=NULL;
//	char *ItemRev=NULL;
//	char *ItemSeq=NULL;
//	char *ItemJT=NULL;
//	char *ItemCad=NULL;
//	char *word=NULL;
//	char *DsetObjType=NULL;
//	int i, n,j;
//	int k = 0;
//	int level=0;
//	tag_t *children;
//	tag_t item=NULLTAG;
//	tag_t reva=NULLTAG;
//	tag_t t_ChildItemRev;
//	tag_t *tags_found = NULL;
//	int n_tags_found= 0;
//	tag_t *attachments = NULLTAG;
//	tag_t *attachments2 = NULLTAG;
//	tag_t dataset = NULLTAG;
//	tag_t refobject = NULLTAG;
//	tag_t relation_type, relation_type2;
//	tag_t *related_occs = NULLTAG;
//	tag_t *related_items =  NULLTAG;
//	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
//	char **values = (char **) MEM_alloc(1 * sizeof(char *));
//	char refname[AE_reference_size_c + 1];
//	char orig_name[IMF_filename_size_c + 1];
//	AE_reference_type_t reftype;
//	char *sRelStatus	= NULL;
//	char *sCompCode		= NULL;
//	char *sColID		= NULL;
//	char *sColInd		= NULL;
//	char *sColSerial	= NULL;
//
//	depth ++;
//
//	ItemJT =(char *) MEM_alloc(100);
//	ItemCad =(char *) MEM_alloc(100);
//
//	(BOM_line_ask_attribute_string (bom_line_tag, name_attribute, &name));
//	
//	word = strtok(name, "/");
//
//	(BOM_line_ask_attribute_string (bom_line_tag, seqno_attribute, &sequence_no));
//	
//
//	(BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
//	(BOM_line_ask_attribute_tag(bom_line_tag, iChildItemTag, &t_ChildItemRev));
//	
//
//
//	attrs[0] ="item_id";
//	values[0] = (char *)word;
//	(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found));
//	
//
//	item = tags_found[0];
//	(ITEM_ask_latest_rev(item,&reva));
//	
//	if( AOM_ask_value_string(reva,"item_id",&Item_id_par)!=ITK_ok);
//	if( AOM_ask_value_string(reva,"item_revision_id",&Item_LatestRevSeq)!=ITK_ok);
//	if( AOM_UIF_ask_value(reva,"release_status_list",&sRelStatus)!=ITK_ok);
//
//	if((BOM_line_ask_child_lines (bom_line_tag, &n, &children)!=ITK_ok));
//	
//	printf("\n bom line count: %d \n",n);fflush(stdout);
//
//	if(n>0)
//	{
//		for (i = 0; i < n; i++)
//		{
//		
//		if( AOM_UIF_ask_value(children[i],"bl_Design Revision_t5_PrtCatCode",&sCompCode)!=ITK_ok);//CCA_GEAR_SHIFT_KNOB
//		if( AOM_UIF_ask_value(children[i],"bl_Design Revision_t5_ColourID",&sColID)!=ITK_ok);//CLRPRT_ASY_1
//		if( AOM_UIF_ask_value(children[i],"bl_Design Revision_t5_ColourInd",&sColInd)!=ITK_ok);//C/Y/N
//		if( AOM_ask_value_int(bom_line_tag,"bl_level_starting_0",&level)!=ITK_ok);
//
//		int		ValCntT				=	0;
//		tag_t*	ValCntrlObjectTagT	=	NULLTAG;
//		tag_t	qryTagT				=	NULLTAG;
//		char    **qry_valuesT		=	(char **) MEM_alloc(10 * sizeof(char *));
//		int		n_entryT			=	1;
//		char    *qry_entryT[1]		=	{"Colour ID"};
//
//
//		if(QRY_find("Colour Master", &qryTagT));
//		if (qryTagT)
//		{
//		//printf("\n Found Query Colour Master \n");fflush(stdout);
//		}
//		else
//		{
//		//printf("\n Not Found Query Colour Master \n");fflush(stdout);
//		}
//
//		tc_strcpy(ItemCad,"");
//		tc_strcat(ItemCad,sColID);
//		tc_strcat(ItemCad,"*");
//
//		qry_valuesT[0] = ItemCad;
//		if(QRY_execute(qryTagT, n_entryT, qry_entryT, qry_valuesT, &ValCntT, &ValCntrlObjectTagT));
//		//printf(" \n :ValCnt :%d:", ValCntT); fflush(stdout);
//
//		if(ValCntT>0)
//		{
//			if(tc_strcmp(sColInd,"C")==0)
//				{
//				if (AOM_ask_value_string(ValCntrlObjectTagT[0],"object_name",&sColSerial)!=ITK_ok);//ZY
//
//				if(level==0)
//					{
//						// parentinput - CCA_PARKING_BRAKE~CLRPRT_ASY_1`CCA_PARKING_BRAKE`ZZ
//						//	output:		 CCA_PARKING_BRAKE~CLRPRT_ASY_1`CCA_PARKING_BRAKE`ZY`
//						printf("%s~%s`%s`%s`\n",sParentCompCode,sParentColID,sParentCompCode,sColSerial); fflush(stdout);
//						fprintf(fptrP,"%s~%s`%s`%s`\n",sParentCompCode,sParentColID,sParentCompCode,sColSerial); fflush(fptrP);
//					}	
//					
//					//CCA_GEAR_SHIFT_KNOB~CLRPRT_ASY_1Y`CCA_GEAR_SHIFT_KNOB`ZY
//					//CCA_PARKING_BRAKE~CLRPRT_ASY_1`HAND_BRAKE_COVER`C7
//					// relation - CCA_GEAR_SHIFT_KNOB~CLRPRT_ASY_1`CCA_GEAR_SHIFT_KNOB/CLRPRT_ASY_1`
//					printf("%s~%s`%s/%s`\n",sParentCompCode,sParentColID,sCompCode,sColID); fflush(stdout);
//					fprintf(fptr,"%s~%s`%s/%s`\n",sParentCompCode,sParentColID,sCompCode,sColID); fflush(fptr);
//					
//					// childinput - HAND_BRAKE_COVER/EMERLD_GREEN`
//					// output		HAND_BRAKE_COVER/GRANIT_BLACK`
//					printf("%s/%s`\n",sCompCode,sColID); fflush(stdout);
//					fprintf(fptrC,"%s/%s`\n",sCompCode,sColID); fflush(fptrC);
//					
//					printf("%s,%s,%s,%s,%s,%s,%s\n",sbl_item_id,sbl_rev_id,sParentCompCode,sParentColID,sCompCode,sColID,sColSerial); fflush(stdout);
//					fprintf(fptrA,"%s,%s,%s,%s,%s,%s,%s\n",sbl_item_id,sbl_rev_id,sParentCompCode,sParentColID,sCompCode,sColID,sColSerial); fflush(fptrA);
//					
//				}
//		
//		}	
//		}
//	}
//
//	tc_strcpy(ItemCad,"");
//	tc_strcpy(ItemJT," ");
//	tc_strcpy(sRelStatus," ");
//	tc_strcpy(ItemSeq," ");
//	tc_strcpy(ItemRev," ");
//	tc_strcpy(Item_id_par," ");
//	MEM_free (attachments); 
//
//
//	//for (i = 0; i < depth; i++)
//	//	printf ("  "); fflush(stdout);
//
//	//(BOM_line_ask_child_lines (bom_line_tag, &n, &children));
//	
//	//for (i = 0; i < n; i++)
//	//{
//	//	ExpandMultiLevelBom (children[0], depth,fptr,fptrP,fptrC,sParentCompCode,sParentColID);
//	//}
//
//	MEM_free (children);
//	MEM_free (name);
//	MEM_free (sequence_no);
//	return status; 
//}
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
