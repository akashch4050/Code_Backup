/*
/home/trlprod/shells/cad/CMI/OtherShells/CatiaGeneric -u=infodba -p=infodba -i=541260700107_ABFPanelNo.txt -o=541260700107_ABFPanelNo.txt.test -m=miss.txt.test
CatiaGeneric -u=infodba -p=infodba -i=541260700107_ABFPanelNo.txt -o=541260700107_ABFPanelNo.txt.test -m=miss.txt.test

scp infodba@tmvlph1tu7:/home/infodba/dbk/clientcodes/CatiaGeneric .
*/
// new attribute compare requested by Mr. Tare on 30.07.2016
//CatiaGeneric -i=u1.txt -o=panelno.txt -m=miss.txt
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


#define Debug TRUE
struct PartList
{
	char PartNumber[50];
	struct PartList *next;
};
//static void get_bom (tag_t line,tag_t Parent, int depth,char *Item_idP,char *Env, char *EnvOrignal);
//int getTcPartDetails(tag_t* objTag);
int QueryDesignRevision(char* sPartNumber, struct PartList *start,FILE* fp,FILE* fp_missing, char* sString3, char* Allow);

char* subString (char* mainStringf ,int fromCharf,int toCharf);

struct PartList* CreateNode(char PartNumber[20])
{
	struct PartList* p = NULL;
	p = (struct PartList*)malloc(sizeof(struct PartList));
	strcpy(p->PartNumber,PartNumber);
	p->next = NULL;
	return p;
}
void my_free(struct PartList* start)
{
	struct PartList* prev;
	struct PartList* ptr;
	printf("\nCatiaGenericErazing Linklist Started...\n");fflush(stdout);
	for(prev = start, ptr = start; ptr != NULL, prev = ptr;ptr = ptr->next)
	{
		printf("#");
		free(prev);
	}
	start = NULL;
	printf("\nCatiaGenericErazing Linklist Completed.\n");fflush(stdout);
}

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
/*
getTcPartDetails(tag_t* objTag)
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

}

static void get_bom (tag_t line,tag_t Parent, int depth,char *Item_idP,char *Env, char *EnvOrg)
{
    int    status;
	int    k, n,int_ent_sequence;
	int    Item_ID,Item_Revision,xform_matrix,parent_item_seq=0,occ_xform_matrix=0,User=0;
	int    n_attchs,i,referencenumberfound,j;
	int    numBoundingBoxes,ctr;
	int    Parentsequence_id=0;
	int    dum=0;

	double *  boundingBoxes;

	char   *name, *Item_ID_str,*Item_Revision_str,*Item_Description_str,*xform_matrix_str,*occ_xform_matrix_str,*User_str=NULL;
	char   *dumy_Item_ID_str=NULL;
	char   *catiafileName=NULL;
	char   *projectcode=NULL;
	char   *desgngrp=NULL;
	char   *ret_id,*ret_rev;
	char   type_name[TCTYPE_name_size_c+1];
	char   refname[AE_reference_size_c + 1];
	char   orig_name[IMF_filename_size_c + 1];
	char   *enterprise_sequence;
	char   pathname[SS_MAXPATHLEN + 1];
	char   path_name1[SS_MAXPATHLEN];
	char   *ent_seq_str;
	char   *parentrev=NULL;
	char   *parent=NULL;
	char   *parentseq=NULL;
	char   *parentdesc=NULL;
	char   *PatSeq= NULL;
	char   *ITemRevSeq,*Desc_obj,*EnvTemp= NULL;
	char command[100];
	tag_t  *secondary_objects,primary,objTypeTag,refobject=NULLTAG;
	tag_t  *children,itemrev,sequence_id_c;
	tag_t  reln_type =NULLTAG;
	AE_reference_type_t     reftype;

	depth ++;
	if( BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
	Item_ID_str =malloc(100);
	if(BOM_line_ask_attribute_string(line, Item_ID, &Item_ID_str));
	if( BOM_line_look_up_attribute ("bl_rev_item_revision_id",&Item_Revision));
	if(  BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
	if(  BOM_line_ask_attribute_string(Parent, Item_Revision, &parentrev));
	if(  BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &parent_item_seq));
	if(parent) parent=NULL;
	parent =malloc(100);
	if(BOM_line_ask_attribute_string(Parent, Item_ID, &parent));
	printf("\nChild :%25s and parent :%25s",Item_ID_str,parent);
	if(BOM_line_ask_child_lines (line, &n, &children));
	printf("\tchildren count is :%d \n",n);
	for (k = 0; k < n; k++)
    get_bom (children[k],line,depth,Item_idP,Env,EnvOrg);

}*/
void fDeleteCharAllOccurance ( char* sReadLine, char cDelChar )
{
int iStringLength = 0;
int iCounter = 0;
int iCounter1 = 0;
char ch1;
char* sReadLineCpy = NULL;
sReadLineCpy = ( char* ) malloc (500 *  sizeof ( char* ));
iStringLength = strlen ( sReadLine );
printf ( "\nCatiaGenericiStringLength:%d", iStringLength );fflush(stdout);
for ( iCounter = 0; iCounter < iStringLength; iCounter++ )
{
	if ( sReadLine[iCounter] != cDelChar )
	{
		ch1 = sReadLine[iCounter];
		sReadLineCpy[iCounter1] = ch1;
		iCounter1++;
	}
}
sReadLineCpy[iCounter1] = '\0';
printf ( "\nCatiaGenericNew String[%s]", sReadLineCpy );fflush(stdout);
strcpy(sReadLine,sReadLineCpy);

}
GetDIFromTCPart(tag_t* item_rev_tags, FILE* fp, FILE* fp_missing,char* sString3)
{
	tag_t reln_type = NULLTAG;
	int n_attchs;
	tag_t *secondary_objects = NULLTAG;
	tag_t primary = NULLTAG;
	tag_t objTypeTag = NULLTAG;
	char a_type_name[250][TCTYPE_name_size_c+1]= { 0 };
	char type_name[TCTYPE_name_size_c+1];
	int i;
	char *ret_id;
	char *ret_rev;
	int referencenumberfound = 0;
	int j = 0 ;
	char refname[AE_reference_size_c + 1];
	AE_reference_type_t reftype;
	tag_t refobject = NULLTAG;
	char orig_name[IMF_filename_size_c + 1];
	char path_name1[SS_MAXPATHLEN];
	char* Item_id = NULL;
	char* checked_out_user = NULL;
	char* sRevision = NULL;
	char* sSequence = NULL;
	WSO_description_t desc;
	int iSequence = 0;
	char* childITemRevSeq = NULL;
	tag_t owning_user = NULLTAG;
	char userid[SA_user_size_c+1];
	int CatProductFlag = 1;
	logical isCheckOut = false;
	char cDescRevision[10] = { "" };
	char* cRevSeq = NULL ;
	char *newPatRev = NULL;
	char *newPatSeq = NULL;
	char *Item_ID_str ;
	
	if ( AOM_ask_value_string ( *item_rev_tags, "item_id", &Item_ID_str ) != ITK_ok ) PrintErrorStack() ;

	if ( AOM_UIF_ask_value( *item_rev_tags,"item_revision_id",&cRevSeq ) ) PrintErrorStack() ;
	printf("TCDC in ListWTMSelf AOM_UIF_ask_value cRevSeq:     %s\n", cRevSeq);fflush(stdout);

	newPatRev = strtok ( cRevSeq, ";" );
	newPatSeq = strtok ( NULL, ";" );

	printf("\nChecking di for part :%s %s %s",Item_ID_str,newPatRev,newPatSeq);

	if(GRM_find_relation_type("IMAN_specification",&reln_type));//new
	printf("\n tm_ECURefSnapshot after finding reln_type \n");fflush(stdout);//new
	

	//if(GRM_list_secondary_objects_only(*item_rev_tags,reln_type,&n_attchs,&secondary_objects));
	if(GRM_list_secondary_objects_only(*item_rev_tags,reln_type,&n_attchs,&secondary_objects));
	printf("\nCatiaGenericn_attchs:%d",n_attchs);fflush(stdout);
	if( n_attchs > 0 )
	{
		for (i= 0; i < n_attchs; i++)
		{

			primary = secondary_objects[i];
			if(TCTYPE_ask_object_type(primary,&objTypeTag));
			if(TCTYPE_ask_name(objTypeTag,type_name));
			tc_strcpy(a_type_name[i],type_name);
			printf("\n%s",a_type_name[i]);
			if((tc_strcmp(a_type_name[i],"CMI2Product")==0)||(tc_strcmp(a_type_name[i],"CMI2Part")==0))
			{
				CatProductFlag = 0;
				break;
			}
			else
			{
				CatProductFlag = 1;
			}
		}
		printf("CatProductFlag:%d",CatProductFlag);
		if( CatProductFlag == 0 )
		{
			for (i= 0; i < n_attchs; i++)
			{

				primary = secondary_objects[i];
				if(TCTYPE_ask_object_type(primary,&objTypeTag));
				if(TCTYPE_ask_name(objTypeTag,type_name));
				printf("\nCatiaGenerictype_name:%s",type_name);fflush(stdout);
				tc_strcpy(a_type_name[i],type_name);
				if((tc_strcmp(a_type_name[i],"CMI2Product")==0)||(tc_strcmp(a_type_name[i],"CMI2Part")==0))
				{
					if(AE_ask_dataset_id_rev(primary,&ret_id,&ret_rev));
					if(AE_ask_dataset_ref_count(primary,&referencenumberfound));
					for(j=0;j<referencenumberfound;j++)
					{
						if(AE_find_dataset_named_ref(primary,j,refname,&reftype,&refobject));
						//printf("\nrefname:%s",refname);
						if((strcmp(refname,"CATPart")==0) || (strcmp(refname,"CATProduct")==0))
						{
							ERROR_CALL( AOM_ask_value_string(*item_rev_tags,"item_id",&Item_id) );
							ERROR_CALL( WSOM_describe(*item_rev_tags, &desc) );
							ERROR_CALL( AOM_ask_value_int(*item_rev_tags,"sequence_id",&iSequence) );
							childITemRevSeq = (char *) MEM_alloc( 5 * sizeof(char) );
							sprintf(childITemRevSeq,"%d",iSequence);
							//ERROR_CALL( AOM_ask_owner( *item_rev_tags, &owning_user ));
							//ERROR_CALL( SA_ask_user_identifier(owning_user,userid) );

							ERROR_CALL( AOM_UIF_ask_value(primary,"checked_out_user",&checked_out_user) );
							printf("CatiaGenericchecked_out_user%s,",checked_out_user);
							printf("CatiaGeneric%s,", Item_id);fflush(stdout);

							tc_strcpy ( cDescRevision, desc.revision_id );
							newPatRev = strtok ( cDescRevision, ";" );
							newPatSeq = strtok ( NULL, ";" );


							//printf("CatiaGeneric%s,", desc.revision_id);fflush(stdout);
							printf("CatiaGeneric%s,", newPatRev);fflush(stdout);
							//printf("CatiaGeneric%s,", childITemRevSeq);fflush(stdout);
							printf("CatiaGeneric%s,", newPatSeq);fflush(stdout);
							printf("CatiaGeneric%s,", userid);fflush(stdout);
							if( IMF_ask_file_pathname(refobject,SS_UNIX_MACHINE,path_name1)!=ITK_ok)PrintErrorStack();
							printf("CatiaGeneric%s,",path_name1);fflush(stdout);
							if( IMF_ask_original_file_name(refobject,orig_name));
							printf("CatiaGeneric%s\n",orig_name);//RelativeStringSfflush(stdout);

							ERROR_CALL(RES_is_checked_out(*item_rev_tags,&isCheckOut));
							if(isCheckOut == false)
							{
								//fprintf( fp, "%s,%s,%s,%s,%s,%s,N\n", Item_id, desc.revision_id, childITemRevSeq, userid, path_name1, orig_name );
								fprintf( fp, "%s,%s,%s,%s,%s,%s,N\n", Item_id, newPatRev, newPatSeq, checked_out_user, path_name1, orig_name );
							}
							else
							{
								//fprintf( fp, "%s,%s,%s,%s,%s,%s,Y\n", Item_id, desc.revision_id, childITemRevSeq, userid, path_name1, orig_name );
								fprintf( fp, "%s,%s,%s,%s,%s,%s,Y\n", Item_id, newPatRev, newPatSeq, checked_out_user, path_name1, orig_name );
							}


						}
					}
				}
			}
		}
		else
		{
			printf("\nCatiaGeneric1Missing String print here:[%s]",sString3);fflush(stdout);
			fprintf( fp_missing, "%s--DI Not attached to Design Revision", sString3 );
		}
	}
	else
	{
		printf("\nCatiaGeneric2Missing String print here:[%s]",sString3);fflush(stdout);
		fprintf( fp_missing, "%s--No DI Found in Relationship", sString3 );
	}

}
void ExpanRecursively( char* sPartNumber, tag_t* item_rev_tags, struct PartList *start, tag_t Parent, FILE* fp, FILE* fp_missing, char* sString3, char* Allow )
{
	tag_t window = NULLTAG;
	tag_t rule = NULLTAG;
	tag_t top_line = NULLTAG;

	int Item_ID;
	char *Item_ID_str;
	int Item_Revision;
	char *parent = NULL;
	char *parentrev = NULL;
	int parent_item_seq;
	int n;
	tag_t *children;
	char *Item_ID_strW2;
	char *Item_ID_strW3;
	int Item_Type1;
	int Item_Type2;
	tag_t Childobject1;
	int k = 0;
	char CompPartNumber[50] = { 0 };
	int InstanceFound = 0;
	struct PartList *q = NULL;



	if(tc_strcmp(Allow,"Y")==0)
	{
		printf("\nCatiaGenericAllow:%s",Allow);fflush(stdout);
		GetDIFromTCPart(item_rev_tags, fp, fp_missing, sString3);
	}
	if( start == NULL )
	{
		printf("\nCatiaGenericList completed ....");fflush(stdout);
		return;
	}

	printf("\nCatiaGenericstart -> PartNumber:[%s]",start -> PartNumber);fflush(stdout);
	strcpy(CompPartNumber, start -> PartNumber );


	if( *CompPartNumber == '\n' )
	{
		printf("\nCatiaGenericNew line found");fflush(stdout);
		return;
	}
	printf("\nCatiaGenericGet Part bom");fflush(stdout);
	if(BOM_create_window (&window)!=ITK_ok)PrintErrorStack();
	if(CFM_find( "Latest Working", &rule )!=ITK_ok)PrintErrorStack();
	if(BOM_set_window_config_rule( window, rule )!=ITK_ok)PrintErrorStack();
	if(BOM_set_window_pack_all (window, false)!=ITK_ok)PrintErrorStack();
	if(BOM_set_window_top_line (window, null_tag, *item_rev_tags , null_tag, &top_line)!=ITK_ok)PrintErrorStack();
	if( BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
	Item_ID_str = malloc(100);
	if( BOM_line_ask_attribute_string(top_line, Item_ID, &Item_ID_str));
	if( BOM_line_look_up_attribute ("bl_rev_item_revision_id",&Item_Revision));
	if( BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID));
	if( BOM_line_ask_attribute_string(Parent, Item_Revision, &parentrev));
	if( BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &parent_item_seq));
	if(parent) parent = NULL;
	parent = malloc(100);
	if( BOM_line_ask_attribute_string(Parent, Item_ID, &parent) );
	printf ( "\nCatiaGenericChild :%25s and parent :%25s" ,Item_ID_str ,parent);fflush(stdout);
	if( BOM_line_ask_child_lines ( top_line, &n, &children ) );
	printf("\nCatiaGenericNumber of child found :[%d]" ,n);fflush(stdout);
	for ( k = 0; k < n; k++ )
	{
		Childobject1 = children[k];
		Item_Type1 = 0;
		Item_Type2 = 0;
		//if( BOM_line_look_up_attribute ( "bl_item_item_id" ,&Item_Type1) );
		// new attribute compare requested by Mr. Tare on 30.07.2016
		if( BOM_line_look_up_attribute ( "bl_occurrence_name" ,&Item_Type1) );
		if( BOM_line_look_up_attribute ( "bl_item_item_id" ,&Item_Type2) );
		if ( Item_ID_strW2 )
		{
			Item_ID_strW2 = NULL;
		}
		Item_ID_strW2 = malloc(50);



		if ( BOM_line_ask_attribute_string ( Childobject1, Item_Type1, &Item_ID_strW2 ));
		printf("\nCatiaGeneric%s compare with %s", CompPartNumber, Item_ID_strW2 );fflush(stdout);
		// new attribute compare requested by Mr. Tare on 30.07.2016
		if ( Item_ID_strW3 )
		{
			Item_ID_strW3 = NULL;
		}
		Item_ID_strW3 = malloc(50);
		if ( BOM_line_ask_attribute_string ( Childobject1, Item_Type2, &Item_ID_strW3 ));
		printf("\nCatiaGeneric%s compare with %s", CompPartNumber, Item_ID_strW2 );fflush(stdout);
		//if ( tc_strcmp ( Item_ID_strW2, CompPartNumber) == 0 )
		if ( tc_strstr ( CompPartNumber, Item_ID_strW2 ) != NULL )
		{
			printf("\nCatiaGenerictc_strstr ( Item_ID_strW2, CompPartNumber):%s",tc_strstr ( Item_ID_strW2, CompPartNumber));fflush(stdout);
			if( start != NULL )
			{
				printf("\nCatiaGenericRemoving Parent from List started1:[%s]",start -> PartNumber);fflush(stdout);
				q = start;
				start  = start -> next;
				q -> next = NULL;
				free ( q );
				printf("\nCatiaGenericRemoving Parent from List end1:[%s]" ,start -> PartNumber);fflush(stdout);

				QueryDesignRevision ( Item_ID_strW3, start, fp, fp_missing, sString3, "Y" );
				printf("\nCatiaGenericObject Found hence exist");fflush(stdout);
				InstanceFound = 1;
				break;

			}
			else
			{
				printf("\nCatiaGenericList empty2");fflush(stdout);
			}
		}
	}
	if( n != 0 )// if no child
	{
		if ( InstanceFound == 0 )
		{
			printf("\nCatiaGeneric3Missing String print here:[%s]",sString3);fflush(stdout);
			fprintf( fp_missing, "%s--USES Part Not Found", sString3);
		}
	}
}
int QueryDesignRevision ( char* sPartNumber, struct PartList *start, FILE* fp, FILE* fp_missing, char* sString3, char* Allow )
{
	int status;
	WSO_search_criteria_t criteria;
	int count = 0;
	tag_t* tags;
	tag_t item_rev_tags;
	int i =0;
	char* Item_id = NULL;
	int PartLength = 0;

	PartLength = strlen ( sPartNumber );
	printf("\nCatiaGenericbeforeremove:%s:",sPartNumber);fflush(stdout);
	if (sPartNumber[PartLength - 1] == '\r' || sPartNumber[PartLength - 1] == '\n')
	{
		sPartNumber[PartLength - 1] = '\0';         // delete it
		PartLength--;
	}

	printf("\nCatiaGenericQuerying:%s:",sPartNumber);fflush(stdout);
	ERROR_CALL(ITEM_find(sPartNumber,&count,&tags));
	//printf("\nCatiaGenericQueryinghard coded values:%s:",sPartNumber);fflush(stdout);
	//ERROR_CALL(ITEM_find("543861800123",&count,&tags));

	/*ERROR_CALL(WSOM_clear_search_criteria(&criteria));
	tc_strcpy(criteria.class_name,"Design");
	tc_strcpy(criteria.name,sPartNumber);
	ERROR_CALL(WSOM_search(criteria,&count,&tags));*/
	printf("\nCatiaGenericno of records found:[%d]",count);fflush(stdout);

	if( count >0 )
	{
		printf("\nCatiaGenericpart found");fflush(stdout);
		ITEM_ask_latest_rev(tags[0],&item_rev_tags);
		ExpanRecursively ( sPartNumber, &item_rev_tags, start, NULLTAG, fp, fp_missing, sString3, Allow );

	}
	else
	{
		printf("\nCatiaGeneric4Missing String print here:[%s]",sString3);fflush(stdout);
		fprintf( fp_missing, "%s--TC Part Not Found", sString3);
	}
	return ITK_ok;
}

extern int ITK_user_main (int argc, char ** argv )
{
	FILE* fpCatiaGeneric = NULL;
	FILE* fp = NULL;
	FILE* fp_missing = NULL;
	char* sReadLine = NULL;
	char* sResult = NULL;
	char* sAssembly = NULL;
	int iCount = 0;
	char cColon = ':';
	char* sString1 = NULL;
	char* sString2 = NULL;
	char* sString3 = NULL;
	struct PartList *start;
	struct PartList *p;
	struct PartList *q;
	char PartNumber[50] = { 0 };
	/*char inputfile[500] = { 0 };
	char outputfile[500] = { 0 };
	char missing[500] = { 0 };*/
	char tempFileName[500] = { 0 };

	char* inputfile = NULL;
	char* outputfile = NULL;
	char* missing = NULL;
	int sret = 0;
	int LineLength = 0;

	int count = 0;
	tag_t* tags;
	char* sUserName	= NULL;
	char* sPassword = NULL;


	if(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_auto_login( );
	ITK_set_journalling( TRUE );

	sUserName = ITK_ask_cli_argument("-u=");
	sPassword = ITK_ask_cli_argument("-p=");
	inputfile = ITK_ask_cli_argument("-i=");
	outputfile = ITK_ask_cli_argument("-o=");
	missing = ITK_ask_cli_argument("-m=");


	printf("\nCatiaGenericinputfile:[%s]",inputfile);fflush(stdout);
	printf("\nCatiaGenericoutputfile:[%s]",outputfile);fflush(stdout);
	printf("\nCatiaGenericmissing:[%s]",missing);fflush(stdout);
	printf("\nCatiaGenericRemoving ^M char");fflush(stdout);

	/*printf("\nCatiaGeneric:%s",getenv("JAVA_HOME"));
	printf("\nCatiaGeneric:%s",getenv("JRE_HOME"));
	printf("\nCatiaGeneric:%s",getenv("FMS_HOME"));
	printf("\nCatiaGeneric:%s",getenv("TC_ROOT"));
	printf("\nCatiaGeneric:%s",getenv("TCDC_LOGPATH"));
	printf("\nCatiaGeneric:%s",getenv("TCDC_SHELL_PATH"));
	printf("\nCatiaGeneric:%s",getenv("TCDC_LOGPATH"));
	system(". /home/cmitest/TC_DATA_10/tc_profilevars");*/

	if(fp) fclose(fp);
	if(fp_missing) fclose(fp_missing);
	sprintf(tempFileName,"rm -fr %s %s",outputfile, missing);
	printf("\nCatiaGenericDeleting file :[%s]\n",tempFileName );fflush(stdout);
	sret = system(tempFileName);

	fpCatiaGeneric = fopen(inputfile,"r");
	fp = fopen(outputfile,"a");
	fp_missing = fopen(missing,"a");

	if ( fpCatiaGeneric != NULL )
	{
		sReadLine = (char*) malloc( 500 * sizeof ( char* ));
		while ( fgets ( sReadLine, 500, fpCatiaGeneric ) != NULL )
		{

			LineLength = strlen ( sReadLine );
			while (LineLength > 0)                        // while string not empty
			{                                      // if last char is \r or \n
				if (sReadLine[LineLength - 1] == '\r' || sReadLine[LineLength - 1] == '\n')
				{
					printf("\nCatiaGenericFound ^M hence removing");fflush(stdout);
					sReadLine[LineLength - 1] = '\0';         // delete it
					LineLength--;
				}
				else                               // otherwise we found the last "real" character
					break;
			}
			start = NULL;
			p = NULL;
			q = NULL;
			sString1 = (char*) malloc( 500 * sizeof ( char* ));
			sString2 = (char*) malloc( 500 * sizeof ( char* ));
			sString3 = (char*) malloc( 500 * sizeof ( char* ));
			strcpy ( sString1, sReadLine );
			strcpy ( sString2, sReadLine );
			strcpy ( sString3, sReadLine );
			sAssembly = strtok ( sString1, ":" );
			sResult = strtok ( sString2, "::" );
			printf ( "\nCatiaGenericsString3:%s", sString3 );fflush(stdout);
			printf ( "\nCatiaGenericsAssembly:%s", sAssembly );fflush(stdout);
			printf ( "\tCatiaGenericsResult:%s", sResult );fflush(stdout);
			while ( sResult != NULL )
			{
				if ( iCount == 0 )
				{
					fDeleteCharAllOccurance ( sResult, cColon );
					strcpy(PartNumber,sResult);
					printf("\nCatiaGenericTesting1 [%s][%s]",sResult,PartNumber);fflush(stdout);
					if( p == NULL )
					{
						p = CreateNode ( PartNumber );
						start = p;
					}
					else
					{
						q = start ;
						while ( q -> next != NULL)
						{
							q = q -> next;
						}
						q -> next = CreateNode ( PartNumber );
					}

				}
				else
				{
					fDeleteCharAllOccurance ( sResult, cColon );
					strcpy(PartNumber,sResult);
					printf("\nCatiaGenericTesting2 [%s][%s]",sResult,PartNumber);fflush(stdout);
					/*if( (*PartNumber == '\n') && (*( PartNumber + 1 )  == '\0') )
					{
						printf ( "\nNew line hence skipping" );
						break;
					}*/
					if( p == NULL )
					{
						strcpy(PartNumber,sResult);
						p = CreateNode ( PartNumber );
						start = p;
					}
					else
					{
						strcpy(PartNumber,sResult);
						q = start ;
						while ( q -> next != NULL)
						{
							q = q -> next;
						}
						q -> next = CreateNode ( PartNumber );
					}
				}
				if ( ( sResult == NULL ) || ( *sResult == '\0' ) || ( *sResult == '\n' ) )
				{
					break;
				}
				printf ( "\nCatiaGenericComponent:%s", sResult );fflush(stdout);
				sResult = strtok ( NULL, "!" );
				iCount++;
			}
			if ( start != NULL )
			{
				printf("\nCatiaGenericRemoving Parent from List started2:[%s]",start -> PartNumber);fflush(stdout);
				q = start;
				start  = start -> next;
				q -> next = NULL;
				free ( q );
				printf("\nCatiaGenericRemoving Parent from List end2:[%s]" ,start -> PartNumber);fflush(stdout);
				printf("\nCatiaGenericPart Number List to be processed");fflush(stdout);
				for(p = start; p != NULL; p = p -> next)
				{
					printf ( "\n%s" , p -> PartNumber );fflush(stdout);
				}

				printf("\nCatiaGenericQuery design item");fflush(stdout);
				QueryDesignRevision( sAssembly, start, fp, fp_missing, sString3, "N" );
				/*for(p = start; p != NULL; p = p -> next)
				{
					printf ( "\n%s" , p -> PartNumber );
				}*/
				//my_free ( start );
			}
			else
			{
				printf("\nCatiaGenericList is Empty check input file");fflush(stdout);
			}
		}
	}
}
