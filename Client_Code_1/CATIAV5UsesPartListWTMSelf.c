///user/uaprod/shells/cad/CMI/OtherShells

//-u=cmitest -p=cmitest123 -i=287073200105 -r=NR -s=1 -c=CXT -o=287073200105-TCPartList.txt
//CATIAV5UsesPartListWTMSelf -u=infodba -p=infodba -i=277560000124 -r=C -s=2 -c=CXT -o=277560000124-TCPartList.txt
//CATIAV5UsesPartListWTMSelf -u=infodba -p=infodba -i=281854107001 -r=NR -s=1 -c=CXT -o=281854107001.txt
//CATIAV5UsesPartListWTMSelf -u=trlxcmi -p=trlxcmi1 -i=282960210700 -r=NR -s=2 -c=CXT -o=TCPartList.txt
//**********************************************************************************************
//compile CATIAV5UsesPartListWTMSelf.c
//linkitk -o CATIAV5UsesPartListWTMSelf CATIAV5UsesPartListWTMSelf.o
//CATIAV5UsesPartListWTMSelf "-u=infodba" "-p=infodba" "-i=541288100101" "-r=A" "-s=2" "-c=CXT" "-q=/proj/plmdownload/CATProductMismatch/541288100101_A_2_2015_test" "-o=/proj/plmdownload/CATProductMismatch/541288100101_A_2_2015_test/541288100101"-TCPartList.txt
//please check all property from cmitest are deployed on trilix
//CATIAV5UsesPartListWTMSelf "-u=ercpsup" "-p=XYT1ESA" "-i=5445B2B0362001" "-r=F" "-s=3" "-c=ERCReview" "-q=/proj/plmdownload/CATProductMismatch/5445B2B0362001_F_3_2019-06-21-08_09_09" "-o=/proj/plmdownload/CATProductMismatch/5445B2B0362001_F_3_2019-06-21-08_09_09/5445B2B0362001"-TCPartList.txt

#include <ps/ps.h>
#include <ps/ps_errors.h>
#include <ai/sample_err.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <bom/bom.h>
#include <ae/dataset.h>
#include <ps/ps_errors.h>
#include <sa/sa.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <fclasses/tc_string.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <sa/tcfile.h>
#include <tcinit/tcinit.h>
#include <tccore/tctype.h>
#include <res/reservation.h>
#include <tccore/aom.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <sa/imanfile.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <itk/mem.h>
#include <ss/ss_errors.h>
#include <sa/user.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#define TE_MAXLINELEN  128 
#define Debug TRUE
#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}

static int name_attribute, seqno_attribute, parent_attribute, item_tag_attribute;
static void get_bom (tag_t line,tag_t Parent, int depth,char *Item_idP,char *Env, char *EnvOrignal);
//int getTcPartDetails(tag_t* objTag);
int QueryDesignRevision(char* sPartNumber);
char* subString (char* mainStringf ,int fromCharf,int toCharf);

#define ERROR_CALL(x) {               \
  int stat;                     \
  char *err_string = NULL;             \
  if( (stat = (x)) != ITK_ok)   \
  {                             \
    EMH_ask_error_text (stat, &err_string);              \
    if(err_string != NULL) {\
	printf ("ERROR: %d ERROR MSG: %s \n", stat, err_string);        \
    printf ("Function: %s FILE: %s LINE: %d \n",#x, __FILE__, __LINE__);             \
	TC_write_syslog("Arcelik Workflow ERROR[%d]: %s\n\t(FILE: %s, LINE:%d)\n", stat, err_string, __FILE__, __LINE__);\
	MEM_free (err_string);\
	err_string=NULL;\
	}\
    return (stat);          \
  }                         \
 }                                                                              \

static char* default_empty_to_A(char *s)
{
    return (NULL == s ? s : ('\0' == s[0] ? "A" : s));
}
char* subString (char* mainStringf ,int fromCharf,int toCharf)
{
	int i;
	char *retStringf;
	retStringf = (char*) malloc(3);
	for(i=0; i < toCharf; i++ )
              *(retStringf+i) = *(mainStringf+i+fromCharf);
	*(retStringf+i) = '\0';
	return retStringf;
}

static int PrintErrorStack( void )
{
    int iNumErrs = 0;
    int *pSevLst = NULL;
    int *pErrCdeLst = NULL;
    char **pMsgLst = NULL;
    register int i = 0;

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst );
    fprintf( stderr, "Error(PrintErrorStack): \n");
    for ( i = 0; i < iNumErrs; i++ )
    {
        fprintf( stderr, "\t%6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
    }
    return ITK_ok;
}
/*getTcPartDetails(tag_t* objTag)
{
char name[WSO_name_size_c +1];
char description[WSO_desc_size_c +1];
char* Item_id = NULL;
char* sRevision = NULL;
char* sSequence = NULL;
char* sDescription = NULL;
char* sPartType = NULL;
char* sModelDescription = NULL;
char* sKeywordDescription = NULL;
char* sAddonDescription = NULL;
char* sProjectCode = NULL;
char* sDesignGroup = NULL;
char* sMaterialInDrawing = NULL;
char* sMaterialThickness = NULL;
char* sMaterialClass = NULL;
char* sYield = NULL;
char* sEnteredWeight = NULL;
char* sRollupWeight = NULL;
char* sSurfaceArea = NULL;
char* sVolume = NULL;
char* sEnvelopeDimensions = NULL;
char* sPartCategory = NULL;

int iSequence = 0;
double dEnteredWeight = 0.0;
double dRollupWeight = 0.0;
double dSurfaceArea = 0.0;
double dVolume = 0.0;
WSO_description_t desc;
		printf("\nGet Part Details");fflush(stdout);


		Item_id = malloc (50 * sizeof (char *) );
		sRevision = malloc(4 * sizeof (char *) );
		sSequence = malloc(4 * sizeof (char *) );
		sDescription = malloc(400 * sizeof (char *) );
		sPartType = malloc(25 * sizeof (char *) );
		sModelDescription = malloc(400 * sizeof (char *) );
		sKeywordDescription = malloc(400 * sizeof (char *) );
		sAddonDescription = malloc(400 * sizeof (char *) );
		sProjectCode = malloc(10 * sizeof (char *) );
		sDesignGroup = malloc(10 * sizeof (char *) );
		sMaterialInDrawing = malloc(50 * sizeof (char *) );
		sMaterialThickness = malloc(50 * sizeof (char *) );
		sMaterialClass = malloc(50 * sizeof (char *) );
		sYield = malloc(50 * sizeof (char *) );
		sEnteredWeight = malloc(50 * sizeof (char *) );
		sRollupWeight = malloc(50 * sizeof (char *) );
		sSurfaceArea = malloc(50 * sizeof (char *) );
		sVolume = malloc(50 * sizeof (char *) );
		sEnvelopeDimensions = malloc(50 * sizeof (char *) );
		sPartCategory = malloc(50 * sizeof (char *) );

		ERROR_CALL( AOM_ask_value_string(*objTag,"item_id",&Item_id));
		ERROR_CALL( WSOM_describe(*objTag, &desc));
		ERROR_CALL( AOM_ask_value_int(*objTag,"sequence_id",&iSequence));
		ERROR_CALL( AOM_ask_value_string(*objTag,"object_desc",&sDescription));
		if(tc_strcmp(sDescription,"")==0)
		{
			tc_strcpy(sDescription,"NULL");
		}
		ERROR_CALL( AOM_ask_value_string(*objTag,"t5_PartType",&sPartType));
		if(tc_strcmp(sPartType,"")==0)
		{
			tc_strcpy(sPartType,"NULL");
		}


		//if( strcmp(desc.revision_id,"NR")!=0 )
		//{
			ERROR_CALL( AOM_ask_value_string(*objTag,"t5_DocRemarks",&sModelDescription));
			if(tc_strcmp(sModelDescription,"")==0)
			{
				tc_strcpy(sModelDescription,"NULL");
			}
		//}
		tc_strcpy(sKeywordDescription,"NULL");
		tc_strcpy(sAddonDescription,"NULL");
		ERROR_CALL( AOM_ask_value_string(*objTag,"t5_ProjectCode",&sProjectCode));
		if(tc_strcmp(sProjectCode,"")==0)
		{
			tc_strcpy(sProjectCode,"NULL");
		}
		ERROR_CALL( AOM_ask_value_string(*objTag,"t5_DesignGrp",&sDesignGroup));
		if(tc_strcmp(sDesignGroup,"")==0)
		{
			tc_strcpy(sDesignGroup,"NULL");
		}
		ERROR_CALL( AOM_ask_value_string(*objTag,"t5_Material",&sMaterialInDrawing));
		if(tc_strcmp(sMaterialInDrawing,"")==0)
		{
			tc_strcpy(sMaterialInDrawing,"NULL");
		}
		ERROR_CALL( AOM_ask_value_string(*objTag,"t5_MThickness",&sMaterialThickness));
		if(tc_strcmp(sMaterialThickness,"")==0)
		{
			tc_strcpy(sMaterialThickness,"NULL");
		}
		tc_strcpy(sMaterialClass,"NULL");
		tc_strcpy(sYield,"NULL");
		ERROR_CALL( AOM_ask_value_double(*objTag,"t5_Weight",&dEnteredWeight));
		if(dEnteredWeight==0)
		{
			dEnteredWeight = 0;
		}
		ERROR_CALL( AOM_ask_value_double(*objTag,"t5_SurfaceArea",&dSurfaceArea));
		if(dSurfaceArea==0)
		{
			dSurfaceArea = 0;
		}
		ERROR_CALL( AOM_ask_value_double(*objTag,"t5_Volume",&dVolume));
		if(dVolume==0)
		{
			dVolume = 0;
		}
		ERROR_CALL( AOM_ask_value_string(*objTag,"t5_EnvelopeDimensions",&sEnvelopeDimensions));
		if(tc_strcmp(sEnvelopeDimensions,"")==0)
		{
			tc_strcpy(sEnvelopeDimensions,"NULL");
		}
		tc_strcpy(sPartCategory,"NULL");


		printf("\n\nPartNumber=%s\n",Item_id);
		//printf("     Object Typee:     %s\n", desc.object_type);fflush(stdout);
		//printf("     Revision IDD:     %s\n", desc.revision_id);fflush(stdout);
		tc_strcpy(sRevision,desc.revision_id);
		printf("Revision=%s\n",sRevision);
		printf("Sequence=%d\n",iSequence);
		printf("Description=%s\n",sDescription);
		printf("Part Type=%s\n",sPartType);
		printf("Model Description=%s\n",sModelDescription);
		printf("Keyword Description=%s\n",sKeywordDescription);	//?
		printf("Addon Description=%s\n",sAddonDescription);		//?
		printf("Project Code=%s\n",sProjectCode);
		printf("Design Group=%s\n",sDesignGroup);
		printf("Material In Drawing=%s\n",sMaterialInDrawing);
		printf("Material Thickness=%s\n",sMaterialThickness);
		printf("Material Class=%s\n",sMaterialClass);			//?		t5_MatlClass
		printf("Yield=%s\n",sYield);					//?
		printf("Entered Weight=%f\n",dEnteredWeight);
		printf("Rollup Weight=%f\n",dRollupWeight);			//?
		printf("Surface Area=%f\n",dSurfaceArea);
		printf("Volume=%f\n",dVolume);
		printf("Envelope Dimensions=%s\n",sEnvelopeDimensions);
		printf("Part Category=%s\n",sPartCategory);			//?		t5_ItmCategory

}*/
void getJTDetails(tag_t* childitemrev, FILE* fp)
{
	tag_t reln_type =NULLTAG;
	int n_attchs = 0;
	tag_t *secondary_objects;
	tag_t primary;
	tag_t objTypeTag;
	tag_t refobject = NULLTAG;
	int i = 0;
	int j = 0;
	char *ret_id;
	char *ret_rev;
	char type_name[TCTYPE_name_size_c + 1];
	char refname[AE_reference_size_c + 1];
	AE_reference_type_t reftype;
	char orig_name[IMF_filename_size_c + 1];
	char path_name1[SS_MAXPATHLEN];
	int referencenumberfound = 0;
	int iFlagTypeName = 0;

	if ( GRM_list_secondary_objects_only ( *childitemrev, reln_type, &n_attchs, &secondary_objects ) );
	printf("\nn_attchs:%d",n_attchs);fflush(stdout);
	if ( n_attchs > 0 )
	{
		iFlagTypeName = 0;
		for ( i = 0; i < n_attchs; i++ )
		{
			primary = secondary_objects[i];
			if ( TCTYPE_ask_object_type ( primary, &objTypeTag ) );
			if ( TCTYPE_ask_name ( objTypeTag, type_name ) );
			printf("\ntype_name:%s",type_name);fflush(stdout);
			if( strcmp ( type_name, "DirectModel" )== 0 )
			{
				if ( AE_ask_dataset_id_rev ( primary, &ret_id, &ret_rev ) );
				if ( AE_ask_dataset_ref_count ( primary, &referencenumberfound ) );
				for ( j = 0; j < referencenumberfound; j++)
				{
					if ( AE_find_dataset_named_ref ( primary, j, refname, &reftype, &refobject ) );
					printf("\nrefname:%s",refname);fflush(stdout);
					if ( ( strcmp ( refname, "JTPART") == 0 ) )
					{
						if ( IMF_ask_original_file_name ( refobject,orig_name ) );
						printf ( ",%s", orig_name );fflush(stdout);
						fprintf ( fp, ",%s",orig_name );

						if ( IMF_ask_file_pathname ( refobject,SS_UNIX_MACHINE,path_name1 ) != ITK_ok ) PrintErrorStack();
						printf ( ",%s",path_name1 );fflush(stdout);
						fprintf ( fp, ",%s", path_name1 );
						
						
					}
				}
				iFlagTypeName = 0;
				break;
			}
			else
			{
				iFlagTypeName = 1;
			}
			
		}
		if ( iFlagTypeName == 1 )
		{
			printf ( "\nNO JT TST********,NULL,NULL" );fflush(stdout);
			fprintf ( fp, ",NULL,NULL" );
		}
		
	}
	
}

int GetBomAttributeFromTc ( const char * attrName, tag_t wotag, char  **outstr )
{
   int        a_tag_attr=0;
   int        ifail = ITK_ok, mode = -1 ;
   char       *name = NULL ;
   char tmpstr[TE_MAXLINELEN] = "";
   ifail = BOM_line_look_up_attribute ( (char * ) attrName, &a_tag_attr);
   if ( ifail )
       return ifail ;
   else
   {
       ifail = BOM_line_ask_attribute_mode (a_tag_attr, &mode);
       if ( ifail )
           return ifail ;
   }
   *outstr = ( char * ) MEM_alloc ( sizeof ( char ) * TE_MAXLINELEN );

   switch ( mode )
   {
       case BOM_attribute_mode_string :
       {
           ifail = BOM_line_ask_attribute_string (wotag, a_tag_attr, &name);
           sprintf ( *outstr, "%s",  name == NULL ? "" : name );
           break;
       }
       case BOM_attribute_mode_string_intl :
       {
           sprintf ( *outstr, "%s",  "NOT SUPPORTED YET" );
           break;
       }
       case BOM_attribute_mode_logical :
       {
           logical yesno;
           ifail = BOM_line_ask_attribute_logical (wotag, a_tag_attr, &yesno);

           yesno ? sprintf ( *outstr , "%s" , "y" ) : sprintf (
*outstr , "%s", "n" );
           break;
       }
       case BOM_attribute_mode_int :
       {
           int anint = 0;
           ifail = BOM_line_ask_attribute_int (wotag, a_tag_attr, &anint);
           sprintf ( *outstr, "%d",  anint );
           break;
       }
       case BOM_attribute_mode_tag :
       {
           tag_t atag = null_tag ;
           char * tagstr = NULL;
           ifail = BOM_line_ask_attribute_tag (wotag, a_tag_attr, &atag);
           if ( atag != null_tag )
               ifail = PROP_tag_to_string (atag, &tagstr );
           sprintf ( *outstr, "%s",  tagstr == NULL ? "" : tagstr );
           break;
       }
       case BOM_attribute_mode_double :
       {
           double adouble = 0.0;

           ifail = BOM_line_ask_attribute_double (wotag, a_tag_attr, &adouble);
           sprintf ( *outstr, "%f",  adouble );
           break;
       }
       default :
       {
           sprintf ( *outstr, "%s",  "" );
       }
   }

   MEM_free ( name );

   return ifail;
}
void ListWTMSelf(tag_t item_rev_tags, FILE *fp)
{
	tag_t reln_type = NULLTAG ;
	int n_attchs = 0 ;
	int i = 0 ;
	tag_t *secondary_objects = NULLTAG ;
	tag_t primary = NULLTAG ;
	tag_t objTypeTag = NULLTAG ;
	char a_type_name[250][TCTYPE_name_size_c+1]= { 0 } ;
	char type_name[TCTYPE_name_size_c+1] ;
	char *Item_ID_str ;
	char* cRevSeq = NULL ;
	char *newPatRev = NULL ;
	char *newPatSeq = NULL ;
	char *ret_id ;
	char *ret_rev ;
	int referencenumberfound = 0 ;
	char *childDesc_obj = NULL ;
	char *childPartType = NULL ;
	char *childPartType1 = NULL ;
	tag_t childitemrev = NULLTAG ;
	char *childUser_str = NULL ;
	char path_name1[SS_MAXPATHLEN] ;
	char orig_name[IMF_filename_size_c + 1] ;
	char *Item_ID_strW2 = NULL ;
	char *childbl_Qty_str = NULL ;
	tag_t refobject = NULLTAG ;
	AE_reference_type_t reftype ;
	char refname[AE_reference_size_c + 1] ;
	int j = 0 ;
	char *childCatiaOccName_str=NULL ;
	char *childocc_xform_matrix_str = NULL ;
	int CatiaIndex = -1 ;
	char *PartType;
	//char *Item_ID_strW2 = NULL;
	char* ccRevSeq = NULL ;
	char *newcPatRev = NULL ;
	char *newcPatSeq = NULL ;

	if ( AOM_ask_value_string ( item_rev_tags, "item_id", &Item_ID_str ) != ITK_ok ) PrintErrorStack() ;

	if ( AOM_UIF_ask_value( item_rev_tags,"item_revision_id",&cRevSeq ) ) PrintErrorStack() ;
	printf("TCDC in ListWTMSelf AOM_UIF_ask_value cRevSeq:     %s\n", cRevSeq);fflush(stdout);

	newPatRev = strtok ( cRevSeq, ";" );
	newPatSeq = strtok ( NULL, ";" );

	printf("TCDCRevision=%s\n",newPatRev);fflush(stdout);
	printf("TCDCSequence=%s\n",newPatSeq);fflush(stdout);
	
	if ( AOM_UIF_ask_value( item_rev_tags,"owning_user",&childUser_str ) ) PrintErrorStack() ;
	printf("TCDC in ListWTMSelf childUser_str:%s\n", childUser_str ) ; fflush(stdout);
	
	if ( AOM_UIF_ask_value( item_rev_tags,"owning_user",&childUser_str ) ) PrintErrorStack() ;
	printf("TCDC in ListWTMSelf childUser_str:%s\n", childUser_str ); fflush(stdout);
	//object_desc
	if ( AOM_UIF_ask_value( item_rev_tags,"object_desc",&childDesc_obj ) ) PrintErrorStack() ;
	printf("TCDC in ListWTMSelf childDesc_obj:%s\n", childDesc_obj ) ; fflush(stdout);
	
	childPartType = NULL ;
	childPartType = malloc(50) ;
	childPartType1 = NULL ;
	childPartType1 = malloc(50) ;
	if ( AOM_ask_value_string ( item_rev_tags, "t5_PartType", &childPartType ) != ITK_ok )   PrintErrorStack();
	if ( strcmp( childPartType, "C" ) ==0 )
	{
		strcpy( childPartType1, "Cmponent" );
	}
	if ( strcmp( childPartType, "A" ) ==0 )
	{
		strcpy( childPartType1, "Assembly" );
	}
	if ( strcmp( childPartType, "G" ) ==0 )
	{
		strcpy( childPartType1, "Group Id" );
	}
	if ( strcmp( childPartType, "T" ) ==0 )
	{
		strcpy( childPartType1, "Technical Part List" );
	}
	if ( strcmp( childPartType, "V" ) ==0 )
	{
		strcpy( childPartType1, "Vehicle" );
	}
	if ( strcmp( childPartType, "" ) ==0 )
	{
		strcpy( childPartType1, "NULL" );
	}
	
	printf("TCDC in ListWTMSelf PartType1:%s\n", childPartType1 ) ; fflush(stdout);

	if ( GRM_list_secondary_objects_only(item_rev_tags,reln_type,&n_attchs,&secondary_objects ) ) ;
	printf ( "\nself n_attchs:%d",n_attchs ) ;
	if ( n_attchs > 0 )
	{
		for ( i = 0; i < n_attchs; i++)
		{

			primary = secondary_objects[i];
			if ( TCTYPE_ask_object_type ( primary, &objTypeTag ) );
			if ( TCTYPE_ask_name ( objTypeTag, type_name ) );
			tc_strcpy ( a_type_name[i], type_name );
			printf("\nSelf type_name:%s",type_name);
		}
		for (i= 0; i < n_attchs; i++)
		{
			if((tc_strcmp(a_type_name[i],"CMI2Product")==0)||(tc_strcmp(a_type_name[i],"CMI2Part")==0))
			{
				CatiaIndex = i;
				break;
			}
			else
			{
				CatiaIndex = -1;
			}
		}

		if( CatiaIndex > -1 )
		{
			for ( i = 0; i < n_attchs; i++)
			{

				primary = secondary_objects[i];
				if ( TCTYPE_ask_object_type ( primary, &objTypeTag ) );
				if ( TCTYPE_ask_name ( objTypeTag, type_name ) );
				tc_strcpy ( a_type_name[i], type_name );
				printf("\nSelf type_name:%s",type_name);

				
				if(strcmp(type_name,"CMI2Product")==0 ||  strcmp(type_name,"CMI2Part")==0 || strcmp(type_name,"CMI2Drawing")==0 || strcmp(type_name,"CMI2AuxPart")==0/*|| strcmp(type_name,"DirectModel")==0*/)
				{
					
					if ( AE_ask_dataset_id_rev ( primary, &ret_id,&ret_rev ) ) ;
					if ( AE_ask_dataset_ref_count ( primary, &referencenumberfound ) ) ;
					for ( j = 0; j < referencenumberfound; j++ )
					{
						if ( AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
						printf("\nself refname:%s",refname);
						if ( strcmp ( refname, "JTPART" ) == 0 )
						{
							if ( AOM_ask_value_string ( primary, "object_name", &Item_ID_strW2 ) != ITK_ok ) PrintErrorStack() ;
							printf("TCDCItem_ID_strW2=%s\n",Item_ID_strW2);fflush(stdout);
				


							printf("\nSELF:%s,%s,%s,,%s,%s,%s,,"
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,Item_ID_strW2
							,newPatRev
							,newPatSeq);fflush(stdout);

							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");

							fflush(stdout);

							fprintf(fp, "%s,%s,%s,,%s,%s,%s,,"
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,Item_ID_strW2
							,newPatRev
							,newPatSeq);
							
						    //fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							
							printf(",%s",Item_ID_strW2);//DocumentNameS is same as childpart
							fflush(stdout);
							fprintf(fp, ",%s",Item_ID_strW2);//DocumentNameS is same as childpart

							if( IMF_ask_original_file_name(refobject,orig_name));
							printf(",%s",orig_name);//RelativeStringS
							fflush(stdout);
							fprintf(fp, ",%s",orig_name);//RelativeStringS

							if(IMF_ask_file_pathname(refobject,SS_UNIX_MACHINE,path_name1)!=ITK_ok)PrintErrorStack();
							printf(",%s",path_name1);//WorkingRelativeStringS
							fprintf(fp, ",%s",path_name1);//WorkingRelativeStringS

							printf(",%s",newPatSeq);//DISequenceS
							fflush(stdout);
							fprintf(fp, ",%s",newPatSeq);//DISequenceS

							printf(",%s",childUser_str);//DataItemownerNameS
							fflush(stdout);
							fprintf(fp, ",%s",childUser_str);//DataItemownerNameS

							getJTDetails(&item_rev_tags, fp);

							printf(",NULL");//part type
							fflush(stdout);
							fprintf(fp, ",NULL");//part type

							printf(",NULL");//ItemownerNameS
							fflush(stdout);
							fprintf(fp, ",NULL");//ItemownerNameS

							printf(",%s",childDesc_obj);
							fflush(stdout);
							fprintf(fp, ",%s\n",childDesc_obj);

						}
						if ( ( strcmp ( refname, "CATPart" ) == 0 ) || ( strcmp ( refname, "CATProduct" ) == 0 ) )
						{
							if ( AOM_ask_value_string ( primary, "object_name", &Item_ID_strW2 ) != ITK_ok ) PrintErrorStack() ;
							printf("TCDCItem_ID_strW2=%s\n",Item_ID_strW2);fflush(stdout);
			

							
							/*printf("\nSELF:%s,%s,%s,,%s,%s,%s,,%s,%s,%s"
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,childCatiaOccName_str
							,childocc_xform_matrix_str
							,childbl_Qty_str);fflush(stdout);

							fprintf(fp, "%s,%s,%s,,%s,%s,%s,,%s,%s,%s"
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,childCatiaOccName_str
							,childocc_xform_matrix_str
							,childbl_Qty_str);*/

							printf("\nSELF:%s,%s,%s,,%s,%s,%s,,"
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,Item_ID_strW2
							,newPatRev
							,newPatSeq);fflush(stdout);

							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");
							printf("NULL");
							printf(",");

							fflush(stdout);

							fprintf(fp, "%s,%s,%s,,%s,%s,%s,,"
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,Item_ID_strW2
							,newPatRev
							,newPatSeq);
							
						    //fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							fprintf(fp,",");
							fprintf(fp,"NULL");
							
							printf(",%s",Item_ID_strW2);//DocumentNameS is same as childpart
							fflush(stdout);
							fprintf(fp, ",%s",Item_ID_strW2);//DocumentNameS is same as childpart

							if( IMF_ask_original_file_name(refobject,orig_name));
							printf(",%s",orig_name);//RelativeStringS
							fflush(stdout);
							fprintf(fp, ",%s",orig_name);//RelativeStringS

							if(IMF_ask_file_pathname(refobject,SS_UNIX_MACHINE,path_name1)!=ITK_ok)PrintErrorStack();
							printf(",%s",path_name1);//WorkingRelativeStringS
							fprintf(fp, ",%s",path_name1);//WorkingRelativeStringS

							printf(",%s",newPatSeq);//DISequenceS
							fflush(stdout);
							fprintf(fp, ",%s",newPatSeq);//DISequenceS

							printf(",%s",childUser_str);//DataItemownerNameS
							fflush(stdout);
							fprintf(fp, ",%s",childUser_str);//DataItemownerNameS

							getJTDetails(&item_rev_tags, fp);

							printf(",%s",childPartType1);
							fflush(stdout);
							fprintf(fp, ",%s",childPartType1);

							printf(",%s",childUser_str);//ItemownerNameS
							fflush(stdout);
							fprintf(fp, ",%s",childUser_str);//ItemownerNameS

							printf(",%s",childDesc_obj);
							fflush(stdout);
							fprintf(fp, ",%s\n",childDesc_obj);
						}
					}
				}
			}
		}
		else //if(strcmp(refname,"CATDrawing")==0)
		{
			//if ( AOM_ask_value_string ( primary, "object_name", &Item_ID_strW2 ) != ITK_ok ) PrintErrorStack() ;
			//printf("TCDCItem_ID_strW2=%s\n",Item_ID_strW2);fflush(stdout);
			
			printf ( "\n%s,%s,%s,,%s,%s,%s,"
			,Item_ID_str
			,newPatRev
			,newPatSeq
			,Item_ID_strW2
			,newPatRev
			,newPatSeq);
			fflush(stdout);
			fprintf(fp, "%s,%s,%s,,%s,%s,%s,\n"
			,Item_ID_str
			,newPatRev
			,newPatSeq
			,Item_ID_strW2
			,newPatRev
			,newPatSeq );
			printf ( "Document type is %s", refname );
			fflush(stdout);
		}
	}
}

static void print_bom2 (tag_t line,tag_t Parent, int depth,char *Item_idP,char *Item_idC,int qty,int TmpQty,FILE *fp, char* sQtyFileName,char* sRevision,char* sSequence)
{
    int status;
	int k;
	int n;
	int int_ent_sequence;
	int q = 0;
	int Item_ID;
	int Item_Revision;
	int Item_Description;
	int xform_matrix,parent_item_seq = 0;
	int occ_xform_matrix = 0;
	int User = 0;
	int bl_Qty = 0;
	int childUser = 0;
	int child_item_seq = 0;
	int childocc_xform_matrix = 0;
	int childbl_Qty = 0;
	int childbl_LCS = 0;
	int n_attchs;
	int i;
	int referencenumberfound;
	int j;
	int jtreferencenumberfound = 0;
	int jtk = 0;
	int numBoundingBoxes;
	int ctr;
	int Parentsequence_id = 0;
	int CatiaOccName = 0;
	int Item_Type1;
	int Item_Type2 = 0;
	char *Item_ID_strW2 = NULL;
	char *Item_ID_strW3 = NULL;
	int dum=0;
	double *  boundingBoxes;
	int childItem_Revision = 0;
	char *name;
	char *Item_ID_str;
	char *Item_Revision_str;
	char *Item_Description_str;
	char *xform_matrix_str;
	char *occ_xform_matrix_str;
	char *User_str = NULL;
	char *bl_Qty_str = NULL;
	char *childUser_str = NULL;
	char *childbl_Qty_str = NULL;
	char *childbl_LCS_str = NULL;
	char *childocc_xform_matrix_str = NULL;
	char *ChildItem_Revision_str = NULL;
	char *dumy_Item_ID_str=NULL;
	char *catiafileName=NULL;
	char *CatiaOccName_str=NULL;
	char *projectcode=NULL;
	char *desgngrp=NULL;
	char *ret_id,*ret_rev;
	char type_name[TCTYPE_name_size_c+1];
	char a_type_name[250][TCTYPE_name_size_c+1]= { 0 };
	char refname[AE_reference_size_c + 1];
	char jtrefname[AE_reference_size_c + 1];
	char orig_name[IMF_filename_size_c + 1];
	char jtorig_name[IMF_filename_size_c + 1];
	char *enterprise_sequence;
	char pathname[SS_MAXPATHLEN + 1];
	char path_name1[SS_MAXPATHLEN];
	char jtpath_name1[SS_MAXPATHLEN];
	char *ent_seq_str;
	char *parentrev = NULL;
	char *parent = NULL;
	char *parentseq = NULL;
	char *parentdesc = NULL;
	char *PatSeq = NULL;
	char *ITemRevSeq;
	char *Desc_obj;
	char *PartType;
	char *PartType1 = NULL;
	char *childDesc_obj = NULL;
	char *childPartType;
	char *childPartType1 = NULL;
	char* childITemRevSeq = NULL;
	char* qtyfilename=NULL;
	char* assyChildQty=NULL;
	char* assyChild=NULL;

	tag_t *occs;
	int n_occs;
	int io = 0;
	int Flag = 0;
	int qtyCnt1;
	int k1 = 0;
	int mulQtyIndex;
	int cLen = 0;
	int p = 0;
	char tmpchar[10];
	char tmpchar1[10];
	//char *6;char *7;char *8;char *9;char *10;

	tag_t ParentItemRevTag;
	tag_t Childobject1;
	tag_t Childobject2 = NULLTAG;
	tag_t *secondary_objects;
	tag_t primary;
	tag_t objTypeTag;
	tag_t refobject = NULLTAG;
	tag_t jtrefobject = NULLTAG;
	tag_t *children;
	tag_t itemrev;
	tag_t sequence_id_c;
	tag_t childsequence_id_c;
	tag_t childitemrev;
	tag_t reln_type =NULLTAG;
	AE_reference_type_t reftype;
	AE_reference_type_t jtreftype;
	char *childcatiafileName=NULL;
	char *childCatiaOccName_str=NULL;
	int childCatiaOccName = 0;
	int iProcessedFlag = 0;
	int CatiaIndex = -1;
	int JtIndex = -1;
	char *datasetowning_user = NULL;
	int DataitemUser = 0;
	char *DataItemUser_str = NULL;
	char matrixline[1000]="";
	char* tmpParent=NULL;
	char* tmpChild=NULL;
	char* tmpQty=NULL;
	char* childParentId_str=NULL;
	char* cParent_id=NULL;
	char* childRevId_str=NULL;

	int childParentId=0,childRevId=0;
	char *newPatRev = NULL ;
	char *newPatSeq = NULL ;

	char *newcPatRev = NULL ;
	char *newcPatSeq = NULL ;


	FILE* qfp=NULL;

	datasetowning_user = malloc (50 * sizeof (char *) );
	depth ++;
	

    if ( BOM_line_look_up_attribute ( "bl_item_item_id", &Item_ID ) );
	if ( BOM_line_ask_attribute_string ( line, Item_ID, &Item_ID_str ) );

    if ( BOM_line_look_up_attribute ( "bl_abs_xform_matrix", &xform_matrix ) );
	if ( BOM_line_ask_attribute_string ( line, xform_matrix, &xform_matrix_str ) );

	printf ( "\n1] Item_ID_str =%s and xform_matrix_str :%s and Item_idC(child taken for qty ) :%s", Item_ID_str, xform_matrix_str, Item_idC );
	fflush(stdout);

	if ( BOM_line_look_up_attribute ("bl_rev_item_revision_id", &Item_Revision ) );
	if ( BOM_line_ask_attribute_string (line, Item_Revision, &Item_Revision_str ) );

	printf ( "\n2] Item_Revision_str =%s", Item_Revision_str );
	fflush(stdout);

	

	if ( BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag, &parent_item_seq ) );
	if ( BOM_line_ask_attribute_tag ( line, parent_item_seq, &itemrev ) );
    if ( AOM_ask_value_int ( itemrev, "sequence_id", &sequence_id_c ) );
	printf ( "\n3] Sequence =%d", sequence_id_c );
	fflush(stdout);
	ITemRevSeq = (char *) MEM_alloc( 5 * sizeof(char) );
	sprintf ( ITemRevSeq, "%d", sequence_id_c );


	newPatRev = strtok ( Item_Revision_str, ";" );
	newPatSeq = strtok ( NULL, ";" );

	printf("TCDCRevision=%s\n",newPatRev);fflush(stdout);
	printf("TCDCSequence=%s\n",newPatSeq);fflush(stdout);


	if ( BOM_line_look_up_attribute ( "bl_rev_object_desc", &Item_Description ) );
	if ( BOM_line_ask_attribute_string ( line, Item_Description, &Item_Description_str ) );
	printf ( "\n4] Item_Description_str =%s", Item_Description_str );
	fflush(stdout);
	if ( BOM_line_look_up_attribute ( "bl_rev_owning_user", &User ) );
	if ( BOM_line_ask_attribute_string ( line, User, &User_str ) );
	printf ( "\n User_str =%s\n", User_str );
	fflush(stdout);
	occ_xform_matrix = 0;
	if ( BOM_line_look_up_attribute ( "bl_plmxml_def_occ_xform", &occ_xform_matrix ) );
	if ( occ_xform_matrix_str ) occ_xform_matrix_str = NULL; occ_xform_matrix_str = malloc ( 500 );
	if (BOM_line_ask_attribute_string ( line, occ_xform_matrix, &occ_xform_matrix_str ) );
	printf ( "\n1");
	fflush(stdout);
	CatiaOccName = 0;
	//if ( BOM_line_look_up_attribute ( "catiaOccurrenceName", &CatiaOccName ) );
	if ( BOM_line_look_up_attribute ( "bl_occurrence_name", &CatiaOccName ) );
	printf ( "\n1a");
	fflush(stdout);
	if ( CatiaOccName_str) CatiaOccName_str = NULL; CatiaOccName_str = malloc ( 200 );
	printf ( "\n1b");
	fflush(stdout);
	if ( BOM_line_ask_attribute_string ( line, CatiaOccName, &CatiaOccName_str ) );
	printf ( "\n12:-> %s",CatiaOccName_str);
	fflush(stdout);

	if ( AOM_ask_value_string ( itemrev, "gov_classification", &enterprise_sequence ) );
	printf ( "\n14");
	fflush(stdout);
	if ( AOM_ask_value_string ( itemrev, "current_desc", &Desc_obj ) != ITK_ok ) PrintErrorStack();
	printf ( "\n15");
	fflush(stdout);

	PartType = NULL; PartType = malloc ( 50 );
	PartType1 = NULL; PartType1 = malloc ( 50 );
	if ( AOM_ask_value_string ( itemrev, "t5_PartType", &PartType) != ITK_ok ) PrintErrorStack();
	if ( strcmp ( PartType, "C" ) == 0 )
	{
		strcpy (PartType1, "Cmponent" );
	}
	if ( strcmp (PartType, "A" ) ==0 )
	{
		strcpy (PartType1, "Assembly" );
	}
	if ( strcmp (PartType, "G") == 0 )
	{
		strcpy (PartType1, "Group Id" );
	}
	if ( strcmp (PartType, "T") == 0 )
	{
		strcpy (PartType1, "Technical Part List" );
	}
	if ( strcmp (PartType, "V" ) ==0 )
	{
		strcpy (PartType1, "Vehicle" );
	}
	if ( strcmp (PartType, "" ) == 0 )
	{
		strcpy (PartType1, "NULL" );
	}
	printf ( "\n16");
	fflush(stdout);
	int_ent_sequence = atoi ( enterprise_sequence );
	printf ( "\n17");
	fflush(stdout);
	dumy_Item_ID_str = MEM_string_copy ( Item_ID_str ) ;
	projectcode = subString ( dumy_Item_ID_str, 0, 4 );
	desgngrp = subString ( dumy_Item_ID_str, 4, 2);
	printf ( "\n18");
	fflush(stdout);
	if(  BOM_line_look_up_attribute ( "bl_rev_item_revision_id", &Item_Revision ) );
	if(  BOM_line_look_up_attribute ( "bl_item_item_id", &Item_ID ) );
	if(  BOM_line_ask_attribute_string( Parent, Item_Revision, &parentrev ) );
	if(  BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &parent_item_seq ) );
	if(  BOM_line_look_up_attribute ( "bl_rev_object_desc", &Item_Description ) );
	printf ( "\n19");
	fflush(stdout);
	if ( parent ) parent = NULL;
	if ( BOM_line_ask_attribute_string ( Parent, Item_ID, &parent ) );
	if ( BOM_line_ask_attribute_tag ( Parent, parent_item_seq, &ParentItemRevTag ) );
	if ( AOM_ask_value_int ( ParentItemRevTag, "sequence_id", &Parentsequence_id ) );
	PatSeq = malloc ( 3 );
	sprintf( PatSeq,"%d", Parentsequence_id );
	if ( BOM_line_ask_attribute_string ( Parent, Item_Description, &parentdesc ) );

	if ( BOM_line_ask_child_lines (line, &n, &children ) );

	TmpQty = 0;
	p = 0;

	printf("\n ******Child Found....n:%d",n);fflush(stdout);

	qtyfilename=(char *) MEM_alloc(200);
	printf("\n************sQtyFileName:%s",sQtyFileName);fflush(stdout);
	printf("\n************sRevision:%s",sRevision);fflush(stdout);
	printf("\n************sSequence:%s",sSequence);fflush(stdout);
	strcpy(qtyfilename,sQtyFileName);
	strcat(qtyfilename,"/AssyChildQty_");
	strcat(qtyfilename,Item_ID_str);
	strcat(qtyfilename,"_");
	//strcat(qtyfilename,Item_Revision_str);
	strcat(qtyfilename,sRevision);
	strcat(qtyfilename,"_");
	//strcat(qtyfilename,ITemRevSeq);
	strcat(qtyfilename,sSequence);
	strcat(qtyfilename,".txt");
	printf("\nqtyfilename:[%s]",qtyfilename);fflush(stdout);
	
	//qfp=fopen(qtyfilename,"r");

	printf ("\n20");fflush(stdout);

   for ( k = 0; k < n; k++ )
    {
		qtyCnt1 = 0;
		if ( Childobject1 ) Childobject1 = NULLTAG;
		Childobject1 = children[k];

		Item_Type1 = 0;
		if ( BOM_line_look_up_attribute ( "bl_item_item_id", &Item_Type1 ) ) ;

		if ( Item_ID_strW2 )
		{
			Item_ID_strW2 = NULL;
		}
		Item_ID_strW2 = malloc ( 50 );

		if ( BOM_line_ask_attribute_string ( Childobject1, Item_Type1, &Item_ID_strW2 ) );

		if( BOM_line_look_up_attribute ( "bl_rev_item_revision_id",&childItem_Revision ) );
		if( BOM_line_ask_attribute_string (Childobject1, childItem_Revision, &ChildItem_Revision_str ) );

		if ( BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &child_item_seq ) );
		if ( BOM_line_ask_attribute_tag ( Childobject1, child_item_seq, &childitemrev ) );
		if ( AOM_ask_value_int ( childitemrev, "sequence_id", &childsequence_id_c ) );
		childITemRevSeq = (char *) MEM_alloc( 5 * sizeof ( char ) );
		sprintf ( childITemRevSeq, "%d", childsequence_id_c );

		newcPatRev = strtok ( ChildItem_Revision_str, ";" );
		newcPatSeq = strtok ( NULL, ";" );

		printf("TCDCRevision=%s\n",newcPatRev);fflush(stdout);
		printf("TCDCSequence=%s\n",newcPatSeq);fflush(stdout);


		childCatiaOccName = 0;
		//if( BOM_line_look_up_attribute ( "catiaOccurrenceName", &childCatiaOccName ) );
		if( BOM_line_look_up_attribute ( "bl_occurrence_name", &childCatiaOccName ) );
		if ( childCatiaOccName_str )
			childCatiaOccName_str = NULL;
		childCatiaOccName_str = malloc ( 200 );
		if ( BOM_line_ask_attribute_string( Childobject1, childCatiaOccName, &childCatiaOccName_str ) );

		childocc_xform_matrix = 0;
		if( BOM_line_look_up_attribute ( "bl_plmxml_def_occ_xform", &childocc_xform_matrix ) );
		if( childocc_xform_matrix_str )
			childocc_xform_matrix_str = NULL;
		childocc_xform_matrix_str = malloc ( 500 );
		if ( BOM_line_ask_attribute_string ( Childobject1, childocc_xform_matrix, &childocc_xform_matrix_str ) );

		if( BOM_line_look_up_attribute ("bl_formatted_parent_name",&childParentId));
		if(childParentId_str) childParentId_str=NULL;childParentId_str=malloc(100);
		if(BOM_line_ask_attribute_string(Childobject1, childParentId, &childParentId_str));
		//cParent_id=strtok(childParentId_str,"/");
		//printf("\n parent_id:[%s]",cParent_id);

		if( BOM_line_look_up_attribute ("bl_item_item_id",&childRevId));
		if(childRevId_str) childRevId_str=NULL;childRevId_str=malloc(100);
		if(BOM_line_ask_attribute_string(Childobject1, childRevId, &childRevId_str));
		//printf("\n childRevId_str:[%s]\n",childRevId_str);

		assyChildQty=(char *) MEM_alloc(100);

		strcpy(assyChildQty,strtok(childParentId_str,"/"));
		strcat(assyChildQty,",");
		strcat(assyChildQty,childRevId_str);
		//printf("\n assyChildQty:[%s]\n",assyChildQty);
		printf ("\n21");fflush(stdout);

		qfp=fopen(qtyfilename,"r");
		//printf("\n 2..qtyfilename:[%s]",qtyfilename);
		if(qfp==NULL)
		{
			printf("\n Could not open file qfp \n");fflush(stdout);
		}
		else
		{
			printf ("\n22");fflush(stdout);
			//printf("\n Could open file  matrixline \n");fflush(stdout);
			while(fgets(matrixline,1500,qfp)!=NULL)
			{
				fputs(matrixline,stdout);
				tmpParent=strtok(matrixline,",");
				tmpChild=strtok(NULL,",");
				//tmpQty=strtok(NULL,",");
				childbl_Qty_str=strtok(NULL,",");

				if (strcmp(childbl_Qty_str,"-") ==0)
				{
					childbl_Qty_str=(char *) MEM_alloc(5);
					strcpy(childbl_Qty_str,"1");
				}

				assyChild=(char *) MEM_alloc(100);
				strcpy(assyChild,tmpParent);
				strcat(assyChild,",");
				strcat(assyChild,tmpChild);

				if (strstr(assyChildQty,assyChild))
				{
					printf("\n%s-%s Match found\n",assyChildQty,assyChild); fflush(stdout);
					break;
				}

				//printf("\n%s,%s,%s",tmpParent,tmpChild,tmpQty); fflush(stdout);



			}
			printf ("\n23");fflush(stdout);
			fclose(qfp);
			qfp=NULL;
		}
		printf ("\n24");fflush(stdout);
		//childbl_Qty = 0;
		//if ( BOM_line_look_up_attribute ( "bl_quantity", &childbl_Qty ) );
		//if ( childbl_Qty_str )
		//	childbl_Qty_str = NULL;
		//childbl_Qty_str = malloc ( 30 );
		//if ( BOM_line_ask_attribute_string (Childobject1, childbl_Qty, &childbl_Qty_str ) );
		//printf("\n 111....childbl_Qty_str::%s",childbl_Qty_str);

		//childbl_LCS = 0;
		//if ( BOM_line_look_up_attribute ( "object_string", &childbl_LCS ) );
		//if ( childbl_LCS_str )  childbl_LCS_str = NULL;
		//childbl_LCS_str = malloc ( 100 );
		//if ( BOM_line_ask_attribute_string (Childobject1, childbl_LCS, &childbl_LCS_str ) );
		//printf("\n 111....childbl_LCS_str::%s",childbl_LCS_str);
		//
		//double bomline_qty;
		//if ( AOM_ask_value_double(Childobject1, "bl_real_quantity", &bomline_qty));
		//printf("\n bomline_qty :%f",bomline_qty);
		//
		//char *strQty = NULL;
		//if (GetBomAttributeFromTc ("bl_quantity", line, &strQty ));
		//printf("\nstrQty is %s \n ",strQty);

		childPartType = NULL;
		childPartType = malloc(50);
		childPartType1 = NULL;
		childPartType1 = malloc(50);
		if ( AOM_ask_value_string ( childitemrev, "t5_PartType", &childPartType ) != ITK_ok )   PrintErrorStack();
		if ( strcmp( childPartType, "C" ) ==0 )
		{
			strcpy( childPartType1, "Cmponent" );
		}
		if ( strcmp( childPartType, "A" ) ==0 )
		{
			strcpy( childPartType1, "Assembly" );
		}
		if ( strcmp( childPartType, "G" ) ==0 )
		{
			strcpy( childPartType1, "Group Id" );
		}
		if ( strcmp( childPartType, "T" ) ==0 )
		{
			strcpy( childPartType1, "Technical Part List" );
		}
		if ( strcmp( childPartType, "V" ) ==0 )
		{
			strcpy( childPartType1, "Vehicle" );
		}
		if ( strcmp( childPartType, "" ) ==0 )
		{
			strcpy( childPartType1, "NULL" );
		}

		if ( BOM_line_look_up_attribute ( "bl_rev_owning_user", &childUser ) );
		if ( BOM_line_ask_attribute_string ( Childobject1, childUser, &childUser_str ) );

		if (AOM_ask_value_string(Childobject1,"bl_rev_object_desc",&childDesc_obj)!=ITK_ok)   PrintErrorStack();

		if ( GRM_list_secondary_objects_only(childitemrev,reln_type,&n_attchs,&secondary_objects ) );
		if ( n_attchs > 0 )
		{
			CatiaIndex = -1;
			JtIndex = -1;
			for (i= 0; i < n_attchs; i++)
			{

				primary = secondary_objects[i];
				if( TCTYPE_ask_object_type ( primary, &objTypeTag ) );
				if( TCTYPE_ask_name ( objTypeTag, type_name ) );
				tc_strcpy ( a_type_name[i], type_name );
			}
			for (i= 0; i < n_attchs; i++)
			{
				if((tc_strcmp(a_type_name[i],"CMI2Product")==0)||(tc_strcmp(a_type_name[i],"CMI2Part")==0))
				{
					CatiaIndex = i;
					break;
				}
				else
				{
					CatiaIndex = -1;
				}
			}

			if( CatiaIndex > -1 )
			{
				primary = secondary_objects[CatiaIndex];
				if ( TCTYPE_ask_object_type(primary,&objTypeTag ) );
				if ( TCTYPE_ask_name(objTypeTag,type_name ) );
				//printf("\ntype_name:%s",type_name);

				//if( AOM_ask_value_string(primary,"owning_user",&datasetowning_user)!=ITK_ok)   PrintErrorStack();
				//printf("\ndatasetowning_user:%s",datasetowning_user);


				//if( BOM_line_look_up_attribute ("owning_user",&DataitemUser));
				//if(BOM_line_ask_attribute_string(primary, DataitemUser, &DataItemUser_str));
				//printf("\nDataItemUser_str:%s",DataItemUser_str);

				if(strcmp(type_name,"CMI2Product")==0 ||  strcmp(type_name,"CMI2Part")==0 || strcmp(type_name,"CMI2Drawing")==0 /*|| strcmp(type_name,"DirectModel")==0*/)
				{
					if(AE_ask_dataset_id_rev(primary,&ret_id,&ret_rev));
					if(AE_ask_dataset_ref_count(primary,&referencenumberfound));
					for(j=0;j<referencenumberfound;j++)
					{
						if(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
						if((strcmp(refname,"CATPart")==0) || (strcmp(refname,"CATProduct")==0))
						{
							printf("\nDBK TST:%s,%s,%s,,%s,%s,%s,,%s,%s,%s"
							,Item_ID_str
							//,Item_Revision_str
							,newPatRev
							//,ITemRevSeq
							,newPatSeq
							,Item_ID_strW2
							//,ChildItem_Revision_str
							,newcPatRev
							//,childITemRevSeq
							,newcPatSeq
							,childCatiaOccName_str
							,childocc_xform_matrix_str
							,childbl_Qty_str);fflush(stdout);

							fprintf(fp, "%s,%s,%s,,%s,%s,%s,,%s,%s,%s"
							,Item_ID_str
							,newPatRev
							,newPatSeq
							,Item_ID_strW2
							,newcPatRev
							,newcPatSeq
							,childCatiaOccName_str
							,childocc_xform_matrix_str
							,childbl_Qty_str);

							/*printf(",0");
							fprintf(fp, ",0");*/

							printf(",%s",Item_ID_strW2);//DocumentNameS is same as childpart
							fflush(stdout);
							fprintf(fp, ",%s",Item_ID_strW2);//DocumentNameS is same as childpart

							if( IMF_ask_original_file_name(refobject,orig_name));
							printf(",%s",orig_name);//RelativeStringS
							fflush(stdout);
							fprintf(fp, ",%s",orig_name);//RelativeStringS

							if(IMF_ask_file_pathname(refobject,SS_UNIX_MACHINE,path_name1)!=ITK_ok)PrintErrorStack();
							printf(",%s",path_name1);//WorkingRelativeStringS
							fprintf(fp, ",%s",path_name1);//WorkingRelativeStringS

							printf(",%s",childITemRevSeq);//DISequenceS
							fflush(stdout);
							fprintf(fp, ",%s",childITemRevSeq);//DISequenceS

							printf(",%s",childUser_str);//DataItemownerNameS
							fflush(stdout);
							fprintf(fp, ",%s",childUser_str);//DataItemownerNameS

							getJTDetails(&childitemrev, fp);

							printf(",%s",childPartType1);
							fflush(stdout);
							fprintf(fp, ",%s",childPartType1);

							printf(",%s",childUser_str);//ItemownerNameS
							fflush(stdout);
							fprintf(fp, ",%s",childUser_str);//ItemownerNameS

							printf(",%s",childDesc_obj);
							fflush(stdout);
							fprintf(fp, ",%s\n",childDesc_obj);
						}
					}
				}
			}
			else //if(strcmp(refname,"CATDrawing")==0)
			{
				printf ( "\n%s,%s,%s,,%s,%s,%s,"
				,Item_ID_str
				,Item_Revision_str
				,ITemRevSeq
				,Item_ID_strW2
				,ChildItem_Revision_str
				,childITemRevSeq);
				fflush(stdout);
				fprintf(fp, "%s,%s,%s,,%s,%s,%s,\n"
				,Item_ID_str
				,Item_Revision_str
				,ITemRevSeq
				,Item_ID_strW2
				,ChildItem_Revision_str
				,childITemRevSeq );
				printf ( "Document type is %s", refname );
				fflush(stdout);
			}
		}
   }
}
extern int ITK_user_main (int argc, char ** argv )
{
	FILE* fp = NULL;
	FILE* fpQty = NULL;
	//FILE* fp2 = NULL;

	char *Item_id = NULL;
	//char last_mod_user[SA_user_size_c+1];
	//tag_t a_mod_user = NULLTAG;
	char* sReadLine = NULL;
	char* sResult = NULL;
	char* sAssembly = NULL;
	char cColon = ':';
	char* sString1 = NULL;
	char* sString2 = NULL;
	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));
	char* TempFile = NULL;
	char* sUserName	= NULL;
	char* sPassword = NULL;
	char* sPartNumber = NULL;
	char* sRevision = NULL;
	char* sSequence = NULL;
	char* sContext = NULL;
	char* sOutFileName = NULL;
	char* sQtyFileName = NULL;
	char* sOutQtyFileNm = NULL;
	char ca_ChildType_Name[TCTYPE_name_size_c+1];
	char *cp_ItemType_str=NULL;
	char *cp_ItemID_str=NULL;
	char *cp_ItemRev_str=NULL;
	char *bl_Qty_str=NULL;
	char *iParentId_str=NULL;
	char* parent_id=NULL;

	int iCount = 0;
	int status;
	int count = 0;
	int n_items = 0;
	int i_ChildCount= 0,iChldPrts = 0;
	int iItemRevId=0,iItemId=0,bl_Qty=0,iParentId=0;

	tag_t* tags;
	tag_t item_rev_tags;
	tag_t window;
	tag_t window2;
	tag_t rule,rule1;
	tag_t item_tag = null_tag;
	tag_t top_line;
	tag_t top_line1;
	tag_t *t_PartChildren;
	tag_t t_Childobject=NULLTAG,t_ChildObjType;

	WSO_search_criteria_t criteria;

	//--------------------------------
	char *parent = NULL;
	int Item_ID = 0;
	int parent_item_seq = 0;
	tag_t ParentItemRevTag =NULLTAG;
	int Parentsequence_id=0;
	char *PatSeq= NULL;
	tag_t itemrev;
	tag_t sequence_id_c;
	char *ITemRevSeq = NULL;
	//--------------------------------

	sOutQtyFileNm = malloc(150);


	printf("\nTCDC Client code started");fflush(stdout);
	if(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_auto_login( );
	ITK_set_journalling( TRUE );

	sUserName = ITK_ask_cli_argument("-u=");
	sPassword = ITK_ask_cli_argument("-p=");
	sPartNumber = ITK_ask_cli_argument("-i=");
	sRevision = ITK_ask_cli_argument("-r=");
	sSequence = ITK_ask_cli_argument("-s=");
	sContext = ITK_ask_cli_argument("-c=");
	sQtyFileName = ITK_ask_cli_argument("-q=");
	sOutFileName = ITK_ask_cli_argument("-o=");
	
	printf("\nTCDC Client code started1");fflush(stdout);

	/*ERROR_CALL(WSOM_clear_search_criteria(&criteria));
	tc_strcpy(criteria.class_name,"Design");
	//tc_strcpy(criteria.name,"287162600150");
	//tc_strcpy(criteria.name,"543861800123");
	//tc_strcpy(criteria.name,sPartNumber);
	ERROR_CALL(WSOM_search(criteria,&count,&tags));
	*/
	ERROR_CALL(ITEM_find(sPartNumber,&n_items,&tags));
	printf("\nTCDC Client code started2[%d]",n_items);fflush(stdout);
	if(n_items > 0 )
	{
		ERROR_CALL(ITEM_ask_latest_rev(tags[0],&item_rev_tags));
		printf("\nTCDC Client code started21");fflush(stdout);
		attrs[0] ="item_id";
		printf("\nTCDC Client code started22");fflush(stdout);
		values[0] = (char *)Item_id;
		//printf("\nvalues[0] :%s\n",values[0]);
		printf("\nTCDC Client code started3");fflush(stdout);
		
		printf("\nTempFile TCPartList:%s \n",sOutFileName);fflush(stdout);

		fp = fopen(sOutFileName,"w");
		if(fp==NULL)
		{
			printf("\nError in opening the file fp TCPartList\n");fflush(stdout);
		}
		
		ListWTMSelf ( item_rev_tags, fp ) ;

		if( AOM_ask_value_string(item_rev_tags,"item_id",&Item_id)!=ITK_ok)PrintErrorStack();
		printf("\nItem_id:%s",Item_id);fflush(stdout);

		//AOM_ask_last_modifier(item_rev_tags,&a_mod_user);
		//ERROR_CALL( SA_ask_user_identifier(a_mod_user,last_mod_user) );
		//printf("\nlast_mod_user%s",last_mod_user);fflush(stdout);

		if(BOM_create_window (&window)!=ITK_ok)PrintErrorStack();
		if(CFM_find( "Latest Working", &rule )!=ITK_ok)PrintErrorStack();
		if(BOM_set_window_config_rule( window, rule )!=ITK_ok)PrintErrorStack();
		if(BOM_set_window_pack_all (window, false)!=ITK_ok)PrintErrorStack();
		if(BOM_set_window_top_line (window, null_tag,item_rev_tags , null_tag, &top_line)!=ITK_ok)PrintErrorStack();;
		//-----------------------------------
		printf("\na1");fflush(stdout);
		printf("\na2");fflush(stdout);
		if(parent) parent=NULL;
		parent =malloc(100);
		printf("\na3");fflush(stdout);
		if( BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
		printf("\na31");fflush(stdout);
		if(BOM_line_ask_attribute_string(top_line, Item_ID, &parent));
		printf("\na4");fflush(stdout);
		if(BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &parent_item_seq));
		if(BOM_line_ask_attribute_tag(top_line, parent_item_seq, &itemrev));
		if(AOM_ask_value_int(itemrev,"sequence_id",&sequence_id_c));
		ITemRevSeq = (char *) MEM_alloc( 5 * sizeof(char) );
		sprintf(ITemRevSeq,"%d",sequence_id_c);
		printf("\nITemRevSeq:%s",ITemRevSeq);
		//--------------------------
		if(BOM_line_ask_child_lines (top_line, &i_ChildCount, &t_PartChildren)!=ITK_ok) PrintErrorStack();
		printf("\nTotal Child Parts Found in BomLine are [%d]\n",i_ChildCount);fflush(stdout);

		if (i_ChildCount > 0)
		{
			sprintf(sOutQtyFileNm,"%s/AssyChildQty_%s_%s_%s.txt",sQtyFileName,sPartNumber,sRevision,sSequence);
			printf("\nsOutQtyFileNm:%s \n",sOutQtyFileNm);fflush(stdout);

			fpQty = fopen(sOutQtyFileNm,"w");
			if(fpQty==NULL)
			{
				printf("\nError in opening the file fpQty \n");fflush(stdout);
			}

			for (iChldPrts = 0; iChldPrts < i_ChildCount; iChldPrts++)
			{
				if(t_Childobject) t_Childobject=NULLTAG;
				t_Childobject=t_PartChildren[iChldPrts];
				if (TCTYPE_ask_object_type(t_Childobject,&t_ChildObjType)!=ITK_ok)PrintErrorStack();
				if (TCTYPE_ask_name(t_ChildObjType,ca_ChildType_Name)!=ITK_ok)PrintErrorStack();
				printf("\n[%d] ChildType is [%s]",iChldPrts+1,ca_ChildType_Name);fflush(stdout);

				if( BOM_line_look_up_attribute ("bl_rev_item_revision_id",&iItemRevId));
				if( BOM_line_look_up_attribute ("bl_item_item_id",&iItemId));
				if(BOM_line_ask_attribute_string(t_Childobject, iItemId, &cp_ItemID_str));
				if(BOM_line_ask_attribute_string(t_Childobject, iItemRevId, &cp_ItemRev_str));
				printf("\t ItemID is: [%s] and ItemRevision is [%s] ",cp_ItemID_str,cp_ItemRev_str);fflush(stdout);

				bl_Qty=0;
				if( BOM_line_look_up_attribute ("bl_quantity",&bl_Qty));
				if(bl_Qty_str) bl_Qty_str=NULL;bl_Qty_str=malloc(20);
				if(BOM_line_ask_attribute_string(t_Childobject, bl_Qty, &bl_Qty_str));
				printf("\t bl_Qty:[%s]",bl_Qty_str);fflush(stdout);

				if( BOM_line_look_up_attribute ("bl_formatted_parent_name",&iParentId));
				if(iParentId_str) iParentId_str=NULL;iParentId_str=malloc(100);
				if(BOM_line_ask_attribute_string(t_Childobject, iParentId, &iParentId_str));
				parent_id=strtok(iParentId_str,"/");
				printf("\t parent_id:[%s]",parent_id);fflush(stdout);

				if (strcmp(bl_Qty_str,"") ==0)
				{
					bl_Qty_str=(char *) MEM_alloc(5);
					strcpy(bl_Qty_str,"-");
				}
				

				fprintf(fpQty,"%s,",parent_id);
				fprintf(fpQty,"%s,",cp_ItemID_str);
				fprintf(fpQty,"%s,\n",bl_Qty_str);

				printf("\nparent_id:%s,",parent_id);fflush(stdout);
				printf("\tcp_ItemID_str:%s,",cp_ItemID_str);fflush(stdout);
				printf("\tbl_Qty_str:%s,\n",bl_Qty_str);fflush(stdout);
			}

			fclose(fpQty);

			printf("\n");fflush(stdout);
		}

		if(BOM_create_window (&window)!=ITK_ok)PrintErrorStack();
		if(CFM_find( "Latest Working", &rule )!=ITK_ok)PrintErrorStack();
		if(BOM_set_window_config_rule( window, rule )!=ITK_ok)PrintErrorStack();
		if(BOM_set_window_pack_all (window, false)!=ITK_ok)PrintErrorStack();
		if(BOM_set_window_top_line (window, null_tag,item_rev_tags , null_tag, &top_line)!=ITK_ok)PrintErrorStack();;
		
		print_bom2 (top_line,NULLTAG, 0,Item_id,Item_id,1,1,fp,sQtyFileName,sRevision,sSequence);

		fclose(fp);
	}
	else
	{
		printf("\nPart Not found");fflush(stdout);
	}
	printf("\nReturn ITK_user_main of CATIAV5UsesPartListWTMSelf\n");fflush(stdout);
}
