#define TE_MAXLINELEN  128
#include <ae/dataset.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ctype.h>
#include <fclasses/tc_string.h>
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
#include <tccore/imantype.h>
#include <tccore/tctype.h>
#include <tccore/uom.h>
#include <tccore/workspaceobject.h>
#include <tcinit/tcinit.h>
#include <textsrv/textserver.h>
#include <unidefs.h>
#include <user_exits/user_exits.h>
#include <sys/stat.h>
#include <pie/pie.h>
#define Debug TRUE
#define MAX_LINE_SIZE         256 /* The maximum size of the strings   */
#define ITK_CALL(X) 							\
		if(Debug)								\
		{										\
			printf(#X);							\
		}										\
		fflush(NULL);							\
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

/* this sequence is so common */
#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}

static int name_attribute, seqno_attribute, parent_attribute, item_tag_attribute , qunt_attribute , refjt_attribute;

static void ExpandMultiLevelBom (tag_t line, tag_t line1, int depth , tag_t ParentBomLine, tag_t *parent_bom , int noOfChildinParent);
static void GetColourSchemeData(tag_t platformItem_tag);

tag_t window = NULLTAG;
tag_t *rev_rule = NULLTAG;
FILE* fp=NULL;
FILE* fp2=NULL;
FILE* fpClrSchm=NULL;
tag_t ParentBomLine = NULLTAG;
int Parentlevel = 0;
int ParentFlag = 0;
int expansionLevel = 0;
tag_t queryTag = NULLTAG;
tag_t VariqueryTag = NULLTAG;
char *parentCompCodeBodyShell = NULL;
int ParentBodyShellLevel = 0;

static char *VOO_prog = (char *)"generate_variant_overlay_option_constraints";


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
static char *VOO_time_stamp()
{
    time_t now;
    static char time_stamp[512];

    time( &now );
    sprintf( time_stamp, "%s", ctime( &now ) );
    time_stamp[tc_strlen(time_stamp)-1] = '\0'; /* strip off new line */
    return time_stamp;
}
static int VOO_show_wso ( tag_t wso ) 
{
    int                  stat=ITK_ok;
    tag_t                instance_class=NULLTAG;
    static tag_t         wso_class=NULLTAG;
    static tag_t         ve_class=NULLTAG;
    static int           not_initialized=TRUE;
    logical              is_wso=FALSE;
	int status;

    // fprintf( stderr, "inside function VOO_show_wso");

	if ( not_initialized ) {
        ITK_CALL ( POM_class_id_of_class ( "WorkspaceObject", &wso_class ) )
        ITK_CALL ( POM_class_id_of_class ( "VariantExpression", &ve_class ) )
        not_initialized = FALSE;
    }

    if ( NULLTAG == wso ) {
        fprintf( stderr, "VOO_show_wso: Can't show info for a NULLTAG\n" );
        return stat;
    }

    ITK_CALL ( POM_class_of_instance ( wso, &instance_class ) )
    ITK_CALL ( POM_is_descendant ( wso_class, instance_class, &is_wso ) )
    if ( ! is_wso ) {
        logical is_ve=FALSE;

        ITK_CALL ( POM_is_descendant ( ve_class, instance_class, &is_ve ) )
        if ( is_wso ) {
            char *t=NULL;

            ITK_CALL ( BOM_variant_expression_as_text( wso, &t ) )
            fprintf( stderr, "%s %s: VariantExpression: %s: ", VOO_prog, VOO_time_stamp(), (NULL==t?"null":t) );
            MEM_free( t );
        }
        else
        {
            char    *class_name=NULL;

            ITK_CALL ( POM_name_of_class ( instance_class, &class_name ) )
            fprintf( stderr, "%s", (NULL==class_name?"null":class_name) );
        }
    }
    else
    {
        char              *obj_id=NULL;
        WSO_description_t  wso_desc;

        ITK_CALL ( WSOM_describe ( wso, &wso_desc ) )
        ITK_CALL ( WSOM_ask_object_id_string ( wso, &obj_id ) )
        if ( stat == ITK_ok ) {
            fprintf( stderr, "obj_id=%s", (NULL==obj_id?"null":obj_id) );
            MEM_free( obj_id );
	/*            fprintf( stderr, ", name=%s", wso_desc.object_name );                             */
	/*            fprintf( stderr, ", description=%s", wso_desc.description );                      */
				fprintf( stderr, ", type=%s", wso_desc.object_type );
	/*            fprintf( stderr, ", owner=%s", wso_desc.owners_name );                            */
	/*            fprintf( stderr, ", owning_group=%s", wso_desc.owning_group_name );               */
	/*            fprintf( stderr, ", appl=%s", wso_desc.application );                             */
	/*            fprintf( stderr, ", created=%s", wso_desc.date_created );                         */
	/*            fprintf( stderr, ", modified=%s", wso_desc.date_modified );                       */
	/*            fprintf( stderr, ", last_modifying_user=%s", wso_desc.last_modifying_user_name ); */
	/*            fprintf( stderr, ", release_date=%s", wso_desc.date_released );                   */
	/*            fprintf( stderr, ", release_status=%s", wso_desc.released_for );                  */
	/*            fprintf( stderr, ", id=%s", wso_desc.id_string );                                 */
	/*            fprintf( stderr, ", revision=%d", wso_desc.revision_number );                     */
	/*            fprintf( stderr, ", revision_limit=%d", wso_desc.revision_limit );                */
	/*            fprintf( stderr, ", archive_date=%s", wso_desc.archive_date );                    */
	/*            fprintf( stderr, ", backup_date=%s", wso_desc.backup_date );                      */
	/*            fprintf( stderr, ", is_frozen=%d", wso_desc.is_frozen );                          */
	/*            fprintf( stderr, ", is_reserved=%d", wso_desc.is_reserved );                      */
	/*            fprintf( stderr, ", revision_id=%s", wso_desc.revision_id );                      */
	/*            fprintf( stderr, ", owning_site=%s", wso_desc.owning_site_name );                 */
        }
    }
    fprintf( stderr, "\n" );
    return stat;
}
static void initialise_attribute (char *name,  int *attribute)
{
    int ifail, mode;

    ifail = BOM_line_look_up_attribute (name, attribute);
    CHECK_FAIL;
    ifail = BOM_line_ask_attribute_mode (*attribute, &mode);
    CHECK_FAIL;
    if (mode != BOM_attribute_mode_string)
      { printf ("Help,  attribute %s has mode %d,  I want a string\n", name, mode);
        exit(0);
      }
}
static void initialise (void)
{
    int ifail;

	printf("\nAuto Login--\n\n",ifail);

    /* <kc> pr#397778 July2595 exit if autologin() fail */
    if ((ifail = ITK_auto_login()) != ITK_ok)
       fprintf(stderr,"Login fail !!: Error code = %d \n\n",ifail);
    CHECK_FAIL;

    /* these tokens come from bom_attr.h */
    initialise_attribute (bomAttr_lineName, &name_attribute);
    initialise_attribute (bomAttr_occSeqNo, &seqno_attribute);
    //initialise_attribute (bomAttr_occQty, &qunt_attribute);
    //initialise_attribute (bomAttr_JTDatasetTag , &refjt_attribute);
    ifail = BOM_line_look_up_attribute (bomAttr_lineParentTag, &parent_attribute);
    CHECK_FAIL;
    ifail = BOM_line_look_up_attribute (bomAttr_lineItemTag, &item_tag_attribute);
    CHECK_FAIL;
        ifail = BOM_line_look_up_attribute (bomAttr_JTDatasetTag, &refjt_attribute);
    CHECK_FAIL;
}
extern int ITK_user_main (int argc, char ** argv )
{
    int ifail;
	int status;
    int h=0;
    int org_seq_id_int = 0;
	int currseq=0;
	int kk=0;
	int numBVRs = 0;
	int rulefound= 0;

	char **rulename = NULL;
    char **rulevalue = NULL;

	tag_t close_tag = NULL;
	tag_t *closurerule = NULL;

    char *platformNm = NULL;
    char *ColPlatformNm = NULL;
	char *platformRev = NULL;
    char *platformSeq = NULL;
    char *SVRCotxt = NULL;
    char *ColSVRCotxt = NULL;
    char *expansionLvl = NULL;
    char *context = NULL;
	char* username = NULL;
	char* inputfile=NULL;
	char* inputfile2=NULL;
	char* child_item_id=NULL;
	char* ChildId=NULL;
	char* ChildColInd=NULL;
	char* Col_child_item_id=NULL;

	tag_t *ParentBom=NULLTAG;

	tag_t platformItem_tag = NULLTAG;
	tag_t platform_rev_tag = NULLTAG;
	tag_t ColItem_rev_tag = NULLTAG;
	tag_t vol=NULLTAG;
	tag_t user_tag = NULLTAG;
	tag_t ParentBomLineTag = NULLTAG;
	tag_t bom_view = NULLTAG;

    tag_t rule, topline_tag = null_tag;
	tag_t itemCol_tag = null_tag;
	char refname[AE_reference_size_c + 1];
	char pathnamee[SS_MAXPATHLEN + 1];
	char orig_name[IMF_filename_size_c + 1];
	tag_t refobject = NULLTAG;
	AE_reference_type_t reftype;

	tag_t* bvr_tags = NULL;


	tag_t *tags_found = NULL;
	tag_t *tags_foundC = NULL;
	tag_t *rev1 = NULL;
	tag_t *secondary_objects = NULLTAG;
	tag_t *secondary_objects1 = NULLTAG;
	tag_t *lines = NULL;
	tag_t *lines2 = NULL;
	tag_t *ColChildLines = NULL;
	tag_t *bom_variant_config=NULL;
	tag_t *bom_variant_config_Col=NULL;
	tag_t *outputVNCol_tags=NULLTAG;
	tag_t *outputVCol_tags=NULLTAG;

	tag_t primary=NULLTAG;
	tag_t primary1=NULLTAG;
	tag_t objTypeTagDi=NULLTAG;
	tag_t objTypeTagDi1=NULLTAG;
	tag_t bom_window=NULLTAG;
	tag_t bom_windowCol=NULLTAG;
	tag_t top_line=NULLTAG;
	tag_t top_lineCol=NULLTAG;
	tag_t VarientRuletag = NULLTAG;
	tag_t VarientRuleColtag = NULLTAG;
	tag_t Childobject=NULLTAG;
	tag_t Childobject2=NULLTAG;
	tag_t ColChildobject=NULLTAG;

	int n_tags_found= 0;
	int n_tags_foundC= 0;
	int n_rev= 0;
	int k= 0;
	int k1= 0;
	int countV= 0;
	int countV1= 0;
	int countDep1= 0;
	int n_lines,n_lines1,n_lines2,n_lines3 = 0;
	int n_entriesV = 1;
	int resultCountVNCol = 0;
	int resultCountVCol = 0;
	int p1 = 0;
	int p2 = 0;
	int p3 = 0;

	char *Context_Str=NULL;

	char type_name1[TCTYPE_name_size_c+1];
	char type_name2[TCTYPE_name_size_c+1];

	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));
	char **ValNcolPlatform = (char **) MEM_alloc(1 * sizeof(char *));
	char **valuesNonColSvr = (char **) MEM_alloc(1 * sizeof(char *));
	char **valuesColSvr = (char **) MEM_alloc(1 * sizeof(char *));

	char *qry_entries3[1] = {"Name"},
		 *qry_values3[1]	= {"*"};

	logical modB=FALSE;
	logical modBCol=FALSE;


    (void)argc;
    (void)argv;


    platformNm = ITK_ask_cli_argument("-i=");
    platformRev = ITK_ask_cli_argument("-j=");
	platformSeq = ITK_ask_cli_argument("-k=");
	SVRCotxt = ITK_ask_cli_argument("-s=");
	expansionLvl = ITK_ask_cli_argument("-lvl=");

	initialise();

	if (expansionLvl)
	{
		expansionLevel=atoi(expansionLvl);
	}

	printf("\nClrCxtJT=Input parameters-- platformNm: %s  platformRev: %s  expansionLvl: %s expansionLevel: %d \n",platformNm,platformRev, expansionLvl, expansionLevel);fflush(stdout);

	if ( platformNm )
	{
		POM_get_user(&username,&user_tag);
		printf("\n ClrCxtJT=Session User:[%s]\n",username); fflush(stdout);

		inputfile =(char *) MEM_alloc(100);
		tc_strcpy(inputfile,"t5MulExpBOMDetails_");
		tc_strcat(inputfile,platformNm);
		tc_strcat(inputfile,"_");
		tc_strcat(inputfile,SVRCotxt);
		tc_strcat(inputfile,".txt");

		fp = fopen(inputfile,"w");
		if(fp == NULL)
		{
			printf ("\n ClrCxtJT=Could not open inputfile file : %s\n", inputfile); fflush(stdout);
			exit( EXIT_FAILURE );
		}

		inputfile2 =(char *) MEM_alloc(100);
		tc_strcpy(inputfile2,"t5MulExpBOMJTFile_");
		tc_strcat(inputfile2,platformNm);
		tc_strcat(inputfile2,"_");
		tc_strcat(inputfile2,SVRCotxt);
		tc_strcat(inputfile2,".txt");

		fp2 = fopen(inputfile2,"w");
		if(fp2 == NULL)
		{
			printf ("\n ClrCxtJT=Could not open inputfile2 file : %s\n", inputfile2); fflush(stdout);
			exit( EXIT_FAILURE );
		}

		if(QRY_find("VariantRule", &VariqueryTag));
		printf("\n\t ClrCxtJT=After IFERR_REPORT : QRY_find .... "); fflush(stdout);

		if (VariqueryTag)
		{
			printf("Found Query 'VariantRule' \n\t"); fflush(stdout);
		}
		else
		{
			printf("Not Found Query 'VariantRule' \n\t"); fflush(stdout);

			ITK_CALL(POM_logout(false));
			return status;
		}

		valuesNonColSvr[0] = SVRCotxt ;

		if(QRY_execute(VariqueryTag, n_entriesV, qry_entries3, valuesNonColSvr, &resultCountVNCol, &outputVNCol_tags));

		printf(" [%s]  n_entriesV:%d   resultCount:%d \n\t",SVRCotxt,n_entriesV,resultCountVNCol); fflush(stdout);

		if (resultCountVNCol > 0)
		{
			VarientRuletag = outputVNCol_tags[0];
		}
		else
		{
			printf ("\n ClrCxtJT=NonColour-SVR-VarientRule not present  [%s]\n", SVRCotxt); fflush(stdout);
			exit (0);
		}


		attrs[0] ="item_id";
		values[0] = (char *)platformNm;
		ifail = ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found);
		//MEM_free(attrs);
		MEM_free(values);
		CHECK_FAIL;
		if (n_tags_found == 0)
		{
			printf ("ClrCxtJT=ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", platformNm); fflush(stdout);
			exit (0);
		}
		else if (n_tags_found > 1)
		{
			MEM_free(tags_found);
			EMH_store_initial_error(EMH_severity_error,ITEM_multiple_items_returned);
			printf ( "More than one items matched with id %s\n", platformNm);fflush(stdout);
			exit (0);
		}
		platformItem_tag = tags_found[0];

		if (platformItem_tag == null_tag)
		{
			printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", platformNm); fflush(stdout);
			exit (0);
		}	

		ifail = ITEM_find_revisions(platformItem_tag,platformRev,&n_rev,&rev1);
		printf("\n\n\n\t n_rev: %d \n", n_rev); fflush(stdout);
		if (n_rev == 0)
		{
			ifail = ITEM_ask_latest_rev(platformItem_tag , &platform_rev_tag);
			CHECK_FAIL;
		}
		else if (n_rev == 1)
		{
			platform_rev_tag = rev1[0];
		}
		else
		{
			for(h=0; h<n_rev; h++)
			{
				ifail = AOM_ask_value_int(rev1[h],"sequence_id",&org_seq_id_int);
				CHECK_FAIL;
				currseq = atoi(platformSeq);
				printf("\n\t org_seq_id_int: %d",org_seq_id_int); fflush(stdout);
				printf("\n\t platformSeq: %s",platformSeq); fflush(stdout);
				printf("\n\t seq id is1 :%d",currseq); fflush(stdout);
				//platform_rev_tag = rev1[h];

				if(org_seq_id_int == currseq)
				{
					platform_rev_tag = rev1[h];
					printf("\t.. inside tag assign \n"); fflush(stdout);

					break;
				}
			}
		}
		MEM_free(tags_found);
	}  //If end


	ifail = ITEM_rev_list_bom_view_revs(platform_rev_tag, &numBVRs, &bvr_tags);

	if ( numBVRs == 0 )
	{
		printf ( " Error : No BOM View Revision Exists \n\n " ); fflush(stdout);
		printf ( " Exiting the program \n " ); fflush(stdout);
		exit(0);
	}
	ifail = PS_ask_bom_view_of_bvr(bvr_tags[0],&bom_view) ;

	ITK_CALL ( BOM_create_window( &bom_window ));
	//ITK_CALL(CFM_find( "Latest Working", &rule ));
	ITK_CALL(CFM_find( "ERC release and above", &rule ));
	//ITK_CALL(CFM_find( "ERC release and above dd", &rule ));
	ITK_CALL(BOM_set_window_config_rule( bom_window, rule ));

	ITK_CALL(PIE_find_closure_rules( "BOMViewClosureRuleAPLC",PIE_TEAMCENTER, &rulefound, &closurerule ));
	printf ("\n Closure Rule count: %d \n",rulefound);fflush(stdout);
	if (rulefound > 0)
	{
		close_tag = closurerule[0];
		printf ("closure rule found \n");fflush(stdout);
		MEM_free(closurerule);
	}
	ITK_CALL(BOM_window_set_closure_rule( bom_window,close_tag, 0, rulename,rulevalue ));

	ITK_CALL(BOM_set_window_pack_all(bom_window, TRUE));
	//ITK_CALL(BOM_set_window_top_line( bom_window , platformItem_tag, NULLTAG, NULLTAG, &top_line ));
	ITK_CALL(BOM_set_window_top_line( bom_window , NULLTAG, platform_rev_tag, NULLTAG, &top_line ));
	printf( " ClrCxtJT=BOM_set_window_top_line..\n");

	ITK_CALL(BOM_line_ask_child_lines(top_line, &n_lines1, &lines));
	printf("\n before hiide LLlast Before setting option n_lines1: %d \n", n_lines1);
	if(bom_window != NULLTAG)
	{
		printf("\n before inside bom_window-----\n"); fflush(stdout);
		
		ITK_CALL(BOM_window_hide_unconfigured(bom_window));
		ITK_CALL(BOM_window_show_variants(bom_window));
		ITK_CALL(BOM_create_window_variant_config(bom_window,1,&bom_variant_config));
		ITK_CALL(BOM_variant_config_apply (bom_variant_config));

		printf("\n After inside bom_window-----\n"); fflush(stdout);
		ITK_CALL(BOM_window_apply_variant_configuration(bom_window,1,&VarientRuletag));
		printf("\n After BOM_window_apply_variant_configuration-----\n"); fflush(stdout);
		ITK_CALL(BOM_window_hide_variants(bom_window));
		ITK_CALL(BOM_window_ask_is_modified(bom_window,&modB));
		if(modB)
		{
			printf( "\n modified bom window:..\n");
		}
		else
		{
			printf( "\n not modfiifed ..\n");
		}

		if(BOM_line_ask_child_lines(top_line, &n_lines1, &lines));
		printf("\n 2nd time LLlast Before setting option n_lines1: %d \n", n_lines1); fflush(stdout);
	}

	printf("\n ClrCxtJT=Before ExpandMultiLevelBom Function----------\n"); fflush(stdout);

	printf("\nPartnumber,Rev,Seq,OwnerName,CompCode,MaterialClass,ReleaseStatus,ColourInd,"); fflush(stdout);
	//fprintf(fp,"Partnumber,Rev,Seq,OwnerName,CompCode,MaterialClass,ReleaseStatus,ColourInd,\n"); fflush(fp);
	ParentFlag = 0;
	parentCompCodeBodyShell = NULL;
	parentCompCodeBodyShell=(char *) MEM_alloc(10);

	ExpandMultiLevelBom (top_line, NULLTAG, 0 , ParentBomLineTag , ParentBom , 0);
	fclose(fp);
	fclose(fp2);

	GetColourSchemeData(platformItem_tag);

	printf ("\n================================================\n");


    ITK_exit_module(true);

	return status;
}
static void GetColourSchemeData(tag_t platformItem_tag)
{
	int ifail;
	int resultColSchm, co = 0;
	int status_count, rs, RlzRevFlag = 0;
	int SchmRevsCnt, c2 = 0;
	int Cmpcount, g1 = 0;
	int countDep, k = 0;

	char type_name2[TCTYPE_name_size_c+1];

	char *qry_entries4[1] = {"ID"};
	char **valuesColSchm = (char **) MEM_alloc(1 * sizeof(char *));

	char *qry_entries5[1] = {"Colour ID"};
	char **valuesColID = (char **) MEM_alloc(1 * sizeof(char *));

	char *CpyClrscheme = NULL;
	char *ColSchm_name = NULL;
	char *ColSchmRlzSt = NULL;
	char *t5objectStr = NULL;
	char *SchmCmpCode = NULL;
	char *t5ClSrl = NULL;
	char *Suffix = NULL;
	char *ColourID = NULL;
	char *SchmDesc = NULL;
	char *SchmClrSrl = NULL;
	char *ClrShcmFileNm = NULL;
	char *SchmSuffix = NULL;
	char *SchmColourID = NULL;

	tag_t *secondary_objects = NULLTAG;
	tag_t *ColSchme_tags = NULLTAG;
	tag_t *SchmRevisions = NULLTAG;
	tag_t *relStatus_list = NULLTAG;
	tag_t *CmpCodeOfClrScheme = NULLTAG;

	tag_t context_tag = NULLTAG;
	tag_t objTypeTagDi = NULLTAG;
	tag_t SchmqueryTag = NULLTAG;
	tag_t ClrIdMstrQryTag = NULLTAG;
	tag_t ColorSchemetag = NULLTAG;
	tag_t ColSchmRevTag = NULLTAG;
	tag_t ColSchmTypeTag = NULLTAG;
	tag_t SchmWithCmpcdRel = NULLTAG;
	tag_t CmpCodeObjTag = NULLTAG;

	/*if(platformItem_tag!=NULLTAG)
	{
		 ifail = GRM_list_secondary_objects_only(platformItem_tag,NULLTAG,&countDep,&secondary_objects);
		 printf("\n\t countDep-------->  : %d\n",countDep);fflush(stdout);
		 if (countDep >0)
		 {
			for (k=0;k<countDep ;k++ )
			{
				context_tag=secondary_objects[k];
				ifail = TCTYPE_ask_object_type(context_tag,&objTypeTagDi);
				ifail = TCTYPE_ask_name(objTypeTagDi,type_name2);
				printf("\n\t type_name2-------->  : %s\n\n",type_name2);fflush(stdout);

				if (tc_strcmp(type_name2,"Cfg0ProductItem")==0)
				{
					ifail = AOM_ask_value_string(context_tag,"object_string",&SmVCContext);
					printf("\n\t SmVCContext Object String: %s",SmVCContext);fflush(stdout);

					ifail = AOM_ask_value_string(context_tag,"current_id",&SmCrIDContext);
					printf("\n\t SmCrIDContext ID: %s",SmCrIDContext);fflush(stdout);

					//Query Option Family from Configuration ID
					if(QRY_find("Cfg0OptionFamiliesFromIDs", &queryTag));
					printf("\n\t After Cfg0OptionFamiliesFromIDs : QRY_find");fflush(stdout);
					if (queryTag) { printf("\n\t Found Query");fflush(stdout);
					}else { printf("\n\t Not Found Query");fflush(stdout); }
					qry_values[0] = SmCrIDContext;
					if(QRY_execute(queryTag, n_entries, qry_entries, qry_values, &resultCount, &Optfmly_Set));
					printf("\n\t resultCount : %d\n", resultCount); fflush(stdout);

					if(resultCount > 0)
					{
						for (Optcnt=0;Optcnt<resultCount;Optcnt++ )
						{
							Optfmly_tag = Optfmly_Set[Optcnt];
							ifail = AOM_ask_value_string(Optfmly_tag,"object_name",&OptfmlyStr);
							printf("\n\t OptfmlyStr is :%s\n\n",OptfmlyStr); fflush(stdout);

							if (tc_strcmp(OptfmlyStr,"SCHEME-NO")==0)
							{
								//Query Option Values from Option Family
								if(QRY_find("__Cfg0OptionValuesFromOptionFamilyIDs", &queryTagOptFamVal));
								printf("\n   After IFERR_REPORT : QRY_find \n");fflush(stdout);
								if (queryTagOptFamVal) {
									printf("\n   1.Found Query \n");fflush(stdout);
								}else {
									printf("\n   Not Found Query");fflush(stdout);
								}
								qry_valuesOptFamVal[0]= OptfmlyStr;
								qry_valuesOptFamVal[1]= SmCrIDContext;
								if(QRY_execute(queryTagOptFamVal, n_entriesOptFamVal, qry_entriesOptFamVal, qry_valuesOptFamVal, &resultCountOptFamVal, &revOptFamVal));
								printf("\n\t resultCountOptFamVal : %d\n", resultCountOptFamVal); fflush(stdout);	
								
								printf("\n\t selected Scheme values is =>%s\n",SchemeName);fflush(stdout);
								if(resultCountOptFamVal > 0)
								{
									for (i=0;i<resultCountOptFamVal;i++ )
									{
										OptfmlyVal_tag = revOptFamVal[i];
										ifail = AOM_ask_value_string(OptfmlyVal_tag,"object_name",&OptValName);
										printf("\n\t Option Values from Family is=>%s\n",OptValName);fflush(stdout);
										if(tc_strcmp(SchemeName,OptValName)==0)
										{
											CheckResult=1;
											printf("\n\t CheckResult Values is=>%d\n",CheckResult);fflush(stdout);
											break;
										}													
									}
								}
								
								printf("\n\t\t CheckResult : %d\n",CheckResult);fflush(stdout);
								if(CheckResult==0)
								{
									//Creating Option Values
									ifail = TCTYPE_find_types_for_class("Cfg0LiteralOptionValue",&number_of_types1,&type_tag3);
									ifail = TCTYPE_construct_create_input(type_tag3[0], &object_create_input_tag3);
									ifail = AOM_set_value_string(object_create_input_tag3,"cfg0ObjectId",SchemeName);
									ifail = AOM_set_value_string(object_create_input_tag3,"object_name",SchemeName);
									if(Optfmly_tag != NULLTAG)
									{
										ifail = AOM_get_value_tag(Optfmly_tag,"wso_thread", &thread_tag);
										if(thread_tag != NULLTAG) 
										{ 
											printf("\n\t go thread.......\n\n"); fflush(stdout); 
										}else { 
										printf("\n\t thread error.......\n\n"); fflush(stdout); 
										}
									}else { printf("\n\t family error.......\n\n"); fflush(stdout); }	
									ifail = AOM_set_value_tag(object_create_input_tag3,"cfg0OptionFamilyThread",thread_tag);
									ifail = AOM_set_value_tag_at(object_create_input_tag3,"cfg0ProductItems",0,context_tag);
									ifail = AOM_set_value_int(object_create_input_tag3,"cfg0Sequence",0);
									ifail = TCTYPE_create_object(object_create_input_tag3, &value_tag);
									if(value_tag) 
									{
										ifail = AOM_save(value_tag);
										ifail = AOM_refresh(value_tag,0);
										ifail = AOM_unlock(value_tag);
										printf("\n Option Value is created......\n"); fflush(stdout);
									}

									//Added Option Values to Option family
									ifail = TCTYPE_find_types_for_class("Cfg0Allocation",&number_of_types,&type_tag4);
									ifail = TCTYPE_construct_create_input(type_tag4[0], &object_create_input_tag);
									ifail = AOM_set_value_string(object_create_input_tag,"object_name",SchemeName);
									ifail = AOM_set_value_string(object_create_input_tag,"cfg0ObjectId",SchemeName);
									ifail = AOM_set_value_tag(object_create_input_tag,"cfg0Target",value_tag);
									ifail = AOM_set_value_tag(object_create_input_tag,"cfg0ProductItem",context_tag);
									ifail = TCTYPE_create_object(object_create_input_tag, &new_object);
									if(new_object) 
									{
										ifail = AOM_save(new_object);
										ifail = AOM_refresh(new_object,0);
										ifail = AOM_unlock(new_object);
										printf("\n t5CreateObject : object created.\n"); fflush(stdout);
									}
								}											
								break;
							}
						}
					}
				}
			}
		 }
	}*/
	if(QRY_find("ClrScheme", &SchmqueryTag));
	if (SchmqueryTag)
	{
		printf("\n ClrCxtJT=Found Query SchmqueryTag"); fflush(stdout);

		if(QRY_find("Colour Master", &ClrIdMstrQryTag));
		if (ClrIdMstrQryTag)
		{
			printf("\n ClrCxtJT=Found Query ClrIdMstrQryTag\n");fflush(stdout);
		}
		else
		{
			printf("\n ClrCxtJT=NOT Found Query ClrIdMstrQryTag\n");fflush(stdout);
		}

		//CpyClrscheme =  "CLRSCM-X4/19/0004" ;	//uadev
		//CpyClrscheme =  "CLRSCM-X4/19/0001" ; //UAPROD
		CpyClrscheme =  "CLRSCM-*" ;

		valuesColSchm[0] = CpyClrscheme ;
		if(QRY_execute(SchmqueryTag, 1, qry_entries4, valuesColSchm, &resultColSchm, &ColSchme_tags));
		printf("\n resultColSchm count is-------->  : %d",resultColSchm);fflush(stdout);
		if (resultColSchm > 0)
		{
			if(GRM_find_relation_type("T5_ShmHasClrData",&SchmWithCmpcdRel)!=ITK_ok);

			for(co = 0; co < resultColSchm; co++)
			{
				printf("\n co: %d ",co);fflush(stdout);

				ColorSchemetag = ColSchme_tags[co];

				//if(TCTYPE_ask_object_type(ColorSchemetag,&ColSchmTypeTag)!=ITK_ok);
				//if(TCTYPE_ask_name(ColSchmTypeTag,ColSchm_name)!=ITK_ok);
				//printf("\t ColSchm_name name------> %s",ColSchm_name);fflush(stdout);

				if(ITEM_ask_latest_rev(ColorSchemetag , &ColSchmRevTag)!=ITK_ok);
				if(AOM_ask_value_string(ColSchmRevTag, "object_string", &t5objectStr)!=ITK_ok);
				printf("\t t5objectStr: %s",t5objectStr); fflush(stdout);
				
				int vschmFlag=0;
				if ((strstr(t5objectStr,"CLRSCM-X4/19/0001")!=NULL) || (strstr(t5objectStr,"CLRSCM-X4/19/0002")!=NULL))
				{
					vschmFlag=1;
				}
				if (vschmFlag==0)
				{
					continue;
				}

				if(WSOM_ask_release_status_list(ColSchmRevTag, &status_count, &relStatus_list)!=ITK_ok);
				printf("\t status_count:: %d",status_count); fflush(stdout);
				if(status_count > 0)
				{
					printf("\t ColSchm Release status:");fflush(stdout);
					for(rs = 0; rs < status_count; rs++)
					{
						if(AOM_ask_name(relStatus_list[rs], &ColSchmRlzSt)==ITK_ok)
						printf("   %s ", ColSchmRlzSt);fflush(stdout);
						if ((tc_strcmp(ColSchmRlzSt,"T5_LcsErcRlzd")==0) || (tc_strcmp(ColSchmRlzSt,"ERC Released")==0) )
						{
							RlzRevFlag=1;
							break;
						}
						else
						{
							if(ITEM_list_all_revs (ColorSchemetag, &SchmRevsCnt, &SchmRevisions));
						}
					}
				}
				else
				{
					printf("\t ----No Release status on ColSchm, hence skipping ...");fflush(stdout);
					continue;
				}

				if (SchmWithCmpcdRel!=NULLTAG)
				{
					if(GRM_list_secondary_objects_only(ColSchmRevTag, SchmWithCmpcdRel, &Cmpcount, &CmpCodeOfClrScheme)!=ITK_ok);
					printf("\n\n Comp Code Cmpcount for Color Scheme: %d \n",Cmpcount); fflush(stdout);

					if(Cmpcount <= 0)
					{
						printf("\n\t\t CM=Comp Code are not attached to Color Scheme : %s , hence skipping ??? \n\n",CpyClrscheme); fflush(stdout);
						//return ifail;
						continue;
					}

					ClrShcmFileNm=(char *) MEM_alloc(100);

					if(AOM_ask_value_string(ColSchmRevTag, "t5_ClSchmDesc", &SchmDesc)!=ITK_ok);
					if(AOM_ask_value_string(ColSchmRevTag, "t5_ClSrl", &SchmClrSrl)!=ITK_ok);

					if(tc_strstr(SchmClrSrl,";")!=NULL)
					{															
						SchmSuffix =  strtok(SchmClrSrl,";");
						SchmColourID  =  strtok(NULL,";");
					}
					else
					{
						SchmSuffix =  strtok(SchmClrSrl,",");
						SchmColourID  =  strtok(NULL,",");
					}

					if (strlen(SchmColourID)>0)
					{
						tc_strcpy(ClrShcmFileNm,"");
						tc_strcat(ClrShcmFileNm,SchmColourID);
						tc_strcat(ClrShcmFileNm,"_");
						tc_strcat(ClrShcmFileNm,SchmDesc);

						STRNG_replace_str(ClrShcmFileNm,".","_",&ClrShcmFileNm);
						STRNG_replace_str(ClrShcmFileNm,",","_",&ClrShcmFileNm);
						STRNG_replace_str(ClrShcmFileNm,";","_",&ClrShcmFileNm);
						STRNG_replace_str(ClrShcmFileNm,"-","_",&ClrShcmFileNm);
						STRNG_replace_str(ClrShcmFileNm,"^","_",&ClrShcmFileNm);
						STRNG_replace_str(ClrShcmFileNm,"+","",&ClrShcmFileNm);
						STRNG_replace_str(ClrShcmFileNm," ","_",&ClrShcmFileNm);

						tc_strcat(ClrShcmFileNm,".clrschlog");

						fpClrSchm = fopen(ClrShcmFileNm,"w");
						if(fpClrSchm == NULL)
						{
							printf ("\n Could not open ClrShcmFileNm file : %s\n", ClrShcmFileNm); fflush(stdout);
							exit( EXIT_FAILURE );
						}

						for(g1=0;g1<Cmpcount;g1++ )
						{
							if(CmpCodeObjTag) CmpCodeObjTag=NULLTAG;

							CmpCodeObjTag=CmpCodeOfClrScheme[g1];

							if(AOM_ask_value_string(CmpCodeObjTag, "t5_PrtCatCode", &SchmCmpCode )!=ITK_ok);

							if (tc_strcmp(subString(SchmCmpCode,0,3),"CCA")==0)
							{
								continue;
							}

							if(AOM_ask_value_string(CmpCodeObjTag, "t5_ClSrl", &t5ClSrl )!=ITK_ok);
							
							if(tc_strstr(t5ClSrl,";")!=NULL)
							{															
								Suffix =  strtok(t5ClSrl,";");
								ColourID  =  strtok(NULL,";");
								//printf("\n\t\t CM=Suffix --> : %s",Suffix); fflush(stdout);
								//printf("\n\t\t CM=ColourID --> : %s",ColourID); fflush(stdout);
							}
							else
							{
								Suffix =  strtok(t5ClSrl,",");
								ColourID  =  strtok(NULL,",");
								//printf("\n\t\t CM=Suffix --> : %s",Suffix); fflush(stdout);
								//printf("\n\t\t CM=ColourID --> : %s",ColourID); fflush(stdout);
							}

							printf("\n%s:%s,%s",SchmCmpCode,Suffix,ColourID); fflush(stdout);
							fprintf(fpClrSchm,"%s:%s,%s\n",SchmCmpCode,Suffix,ColourID); fflush(fpClrSchm);
						}

						fclose(fpClrSchm);
					} //End of if (strlen(SchmColourID)>0)
				}
			}
		}
	}
	else
	{
		printf("\n Not Found Query SchmqueryTag"); fflush(stdout);
		if(POM_logout(false)!=ITK_ok);
	}
}
static void ExpandMultiLevelBom (tag_t bom_line_tag, tag_t line1, int depth , tag_t ParentBomLine , tag_t *parent_bom , int noOfChildinParent)
{
	int ifail;
	int i, no_bom_lines , j ,jr, k=0;
	int n_tags_found= 0;
	int level0=0;
	int m = 0;
	int status_count=0;
	int n_entries = 2;
	int resultCount = 2;

	char *name;
	char *word=NULL;
	char *Item_id_par=NULL;
	char *ItemRevSeq=NULL;
	char *ItemRevSeqTemp=NULL;
	char *ItemRevStr=NULL;
	char *ItemRev=NULL;
	char *itemRevSeqStr=NULL;
	char *ItemSeq=NULL;
	char *ColourInd=NULL;
	char *PartType=NULL;
	char *Coated = NULL;
	char *CompCode=NULL;
	char *MatlClassTmp=NULL;
	char *MatlClass=NULL;
	char *ReleaseStatus=NULL;
	char *blLineName=NULL;
	char *Parent_name=NULL;
	char *t5InternalSchme=NULL;
	char *Category=NULL;

	char *qry_entries4[2] = {"Revision","ID"},
		 *qry_values4[2]	= {"*","*"};

	tag_t *child_bom_lines;
	tag_t *tags_found = NULL;
	tag_t *rev=NULLTAG;
	tag_t *compCodeCtrl_tags=NULLTAG;
	
	tag_t item=NULLTAG;
	tag_t reva=NULLTAG;
	tag_t dataset = NULLTAG;
	tag_t refobject = NULLTAG;
	tag_t refobjectG = NULLTAG;
	tag_t datasetG = NULLTAG;
	tag_t CreoGenRev = NULLTAG;
	tag_t child_tag = NULLTAG;
	tag_t CompCodeQryTag = NULLTAG;

	char **attrs = (char **) MEM_alloc(2 * sizeof(char *));
	char **values = (char **) MEM_alloc(2 * sizeof(char *));

	char **valuesCompCode = (char **) MEM_alloc(1 * sizeof(char *));

	//char *qry_entries[1] = {"Colour ID"},	//uadev
	char *qry_entries[1] = {"Comp Code"},	//UAPROD
		 *qry_values[1]	= {"*"};

	CompCode =(char *) MEM_alloc(100);
	MatlClass =(char *) MEM_alloc(3);

	depth ++;

	//printf("   In ExpandMultiLevelBom Function....................\n");

	ifail = BOM_line_ask_attribute_string (bom_line_tag, name_attribute, &name);
	CHECK_FAIL;
	word = strtok(name, "/");
	//printf("\n word: [%s]",word); fflush(stdout);
	/* note that I know name is always defined,  but sometimes sequence number is unset.
	If that happens it returns NULL,  not an error.
	*/

	if (tc_strcmp(word,"<<REMOTE OBJECT>>")!=0)
	{
		attrs[0] ="item_id";
		values[0] = (char *)word;
		ifail = ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found);
		CHECK_FAIL;

		item = tags_found[0];
		//printf("Item Found\n");

		//ifail = ITEM_ask_latest_rev(item,&reva);
		//CHECK_FAIL;

		if (ParentBomLine != NULLTAG)
		{
			if( AOM_ask_value_string(ParentBomLine,"bl_item_item_id",&Parent_name)!=ITK_ok);
		}
		else
		{
			Parent_name =(char *) MEM_alloc(10);
			tc_strcpy(Parent_name,"-");
		}
		//printf("\t Parent_name:[%s] \n",Parent_name); fflush(stdout);

		if( AOM_ask_value_int(bom_line_tag,"bl_level_starting_0",&level0)!=ITK_ok);
		if( AOM_ask_value_string(bom_line_tag,"bl_item_item_id",&Item_id_par)!=ITK_ok);
		if( AOM_ask_value_string(bom_line_tag,"bl_rev_item_revision_id",&ItemRevSeq)!=ITK_ok);
		if( AOM_ask_value_string(bom_line_tag,"bl_rev_item_revision_id",&ItemRevSeqTemp)!=ITK_ok);
		if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_PartType",&PartType)!=ITK_ok);
		if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_Coated",&Coated)!=ITK_ok);
		
		//if( AOM_ask_value_string(bom_line_tag,"bl_T5_ClrPartRevision_t5_ColourInd",&ColourInd)!=ITK_ok);  //uadev
		if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_ColourInd",&ColourInd)!=ITK_ok);		//UAPROD

		tc_strcpy(CompCode,"");
		//if( AOM_ask_value_string(bom_line_tag,"bl_T5_ClrPartRevision_t5_PrtCatCode",&CompCode)!=ITK_ok);  //uadev
		if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_PrtCatCode",&CompCode)!=ITK_ok);		//UAPROD

		printf("\n\n Item_id_par:[%s]    ItemRevSeq:[%s]    PartType:[%s]",Item_id_par, ItemRevSeqTemp, PartType); fflush(stdout);

		if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_MatlClass",&MatlClassTmp)!=ITK_ok);		//--UAPROD

		//if( AOM_ask_value_string(bom_line_tag,"bl_rev_release_status_list",&ReleaseStatus)!=ITK_ok);
		if( AOM_ask_value_string(bom_line_tag,"bl_rev_last_release_status",&ReleaseStatus)!=ITK_ok);


		if( AOM_ask_value_string(bom_line_tag,"bl_line_name",&blLineName)!=ITK_ok);

		if (ItemRevSeqTemp != NULL)
		{
			ItemRev = strtok(ItemRevSeqTemp,";");
			ItemSeq = strtok(NULL,"';'");

			if (ItemRev == NULL)
			{
				ItemRev =(char *) MEM_alloc(3);
				tc_strcpy(ItemRev,"NR");
			}
			if (ItemSeq == NULL)
			{
				ItemSeq =(char *) MEM_alloc(3);
				tc_strcpy(ItemSeq,"1");
			}
		}
		else
		{
			ItemRev =(char *) MEM_alloc(3);
			ItemSeq =(char *) MEM_alloc(3);
			tc_strcpy(ItemRev,"");
			tc_strcpy(ItemSeq,"");
		}

		printf("    strlen(MatlClassTmp): [%d]   MatlClassTmp: [%s] ",strlen(MatlClassTmp),MatlClassTmp); fflush(stdout);
		//printf("\n---1----"); fflush(stdout);

		if (strlen(MatlClassTmp)>0)
		{	
			if (tc_strstr(MatlClassTmp,"ABS")!=NULL)
			{
				tc_strcpy(MatlClass,"AB");
			}
			else if (tc_strstr(MatlClassTmp,"Acrylic")!=NULL)
			{
				tc_strcpy(MatlClass,"AC");
			}
			else if (tc_strstr(MatlClassTmp,"Aluminiun Castings")!=NULL)
			{
				tc_strcpy(MatlClass,"AC");
			}
			else if (tc_strstr(MatlClassTmp,"Adhesives/sealants")!=NULL)
			{
				tc_strcpy(MatlClass,"AD");
			}
			else if (tc_strstr(MatlClassTmp,"Aluminium Extrusion")!=NULL)
			{
				tc_strcpy(MatlClass,"AE");
			}
			else if (tc_strstr(MatlClassTmp,"AC Refrigerants")!=NULL)
			{
				tc_strcpy(MatlClass,"AR");
			}
			else if (tc_strstr(MatlClassTmp,"Asbestos/ceramics")!=NULL)
			{
				tc_strcpy(MatlClass,"AS");
			}
			else if (tc_strstr(MatlClassTmp,"NA")!=NULL)
			{
				tc_strcpy(MatlClass,"AS");
			}
			else if (tc_strstr(MatlClassTmp,"11")!=NULL)
			{
				tc_strcpy(MatlClass,"AS");
			}
			else if (tc_strstr(MatlClassTmp,"Aluminium Sheet")!=NULL)
			{
				tc_strcpy(MatlClass,"AS");
			}
			else if (tc_strstr(MatlClassTmp,"Aluminium Tubes")!=NULL)
			{
				tc_strcpy(MatlClass,"AT");
			}
			else if (tc_strstr(MatlClassTmp,"Aluminium Wrought Bar")!=NULL)
			{
				tc_strcpy(MatlClass,"AW");
			}
			else if (tc_strstr(MatlClassTmp,"Battery Acid")!=NULL)
			{
				tc_strcpy(MatlClass,"BA");
			}
			else if (tc_strstr(MatlClassTmp,"Brass Components")!=NULL)
			{
				tc_strcpy(MatlClass,"BC");
			}
			else if (tc_strstr(MatlClassTmp,"Brake fluid")!=NULL)
			{
				tc_strcpy(MatlClass,"BR");
			}
			else if (tc_strstr(MatlClassTmp,"Bronze Components")!=NULL)
			{
				tc_strcpy(MatlClass,"BR");
			}
			else if (tc_strstr(MatlClassTmp,"C1 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"C1");
			}
			else if (tc_strstr(MatlClassTmp,"C2 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"C2");
			}
			else if (tc_strstr(MatlClassTmp,"Cast Iron Casting")!=NULL)
			{
				tc_strcpy(MatlClass,"CI");
			}
			else if (tc_strstr(MatlClassTmp,"Coolant")!=NULL)
			{
				tc_strcpy(MatlClass,"CO");
			}
			else if (tc_strstr(MatlClassTmp,"Cross linked PE foams")!=NULL)
			{
				tc_strcpy(MatlClass,"CR");
			}
			else if (tc_strstr(MatlClassTmp,"Steel Cold Rolled Sheets")!=NULL)
			{
				tc_strcpy(MatlClass,"CS");
			}
			else if (tc_strstr(MatlClassTmp,"Damper Oil")!=NULL)
			{
				tc_strcpy(MatlClass,"DO");
			}
			else if (tc_strstr(MatlClassTmp,"Electronc components like PCBs/chips etc")!=NULL)
			{
				tc_strcpy(MatlClass,"EC");
			}
			else if (tc_strstr(MatlClassTmp,"Engine Oil")!=NULL)
			{
				tc_strcpy(MatlClass,"EO");
			}
			else if (tc_strstr(MatlClassTmp,"Felt/insulation/fabric")!=NULL)
			{
				tc_strcpy(MatlClass,"FI");
			}
			else if (tc_strstr(MatlClassTmp,"Ferrous metal & alloys")!=NULL)
			{
				tc_strcpy(MatlClass,"FM");
			}
			else if (tc_strstr(MatlClassTmp,"Ferrous Powder Metallurgy Components")!=NULL)
			{
				tc_strcpy(MatlClass,"FP");
			}
			else if (tc_strstr(MatlClassTmp,"Fuels")!=NULL)
			{
				tc_strcpy(MatlClass,"FU");
			}
			else if (tc_strstr(MatlClassTmp,"Glass")!=NULL)
			{
				tc_strcpy(MatlClass,"GL");
			}
			else if (tc_strstr(MatlClassTmp,"Gearbox Oil")!=NULL)
			{
				tc_strcpy(MatlClass,"GO");
			}
			else if (tc_strstr(MatlClassTmp,"Grease")!=NULL)
			{
				tc_strcpy(MatlClass,"GR");
			}
			else if (tc_strstr(MatlClassTmp,"HC1 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"H1");
			}
			else if (tc_strstr(MatlClassTmp,"HC2 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"H2");
			}
			else if (tc_strstr(MatlClassTmp,"Steel Hot Rolled Bars")!=NULL)
			{
				tc_strcpy(MatlClass,"HB");
			}
			else if (tc_strstr(MatlClassTmp,"Steel Hot Rolled Sheets")!=NULL)
			{
				tc_strcpy(MatlClass,"HS");
			}
			else if (tc_strstr(MatlClassTmp,"L1 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"L1");
			}
			else if (tc_strstr(MatlClassTmp,"L2 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"L2");
			}
			else if (tc_strstr(MatlClassTmp,"L3 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"L3");
			}
			else if (tc_strstr(MatlClassTmp,"L4 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"L4");
			}
			else if (tc_strstr(MatlClassTmp,"L5 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"L5");
			}
			else if (tc_strstr(MatlClassTmp,"L6 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"L6");
			}
			else if (tc_strstr(MatlClassTmp,"L7 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"L7");
			}
			else if (tc_strstr(MatlClassTmp,"L8 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"L8");
			}
			else if (tc_strstr(MatlClassTmp,"Leather")!=NULL)
			{
				tc_strcpy(MatlClass,"LR");
			}
			else if (tc_strstr(MatlClassTmp,"M1 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"M1");
			}
			else if (tc_strstr(MatlClassTmp,"M2 -Refer Annex-I of TS 11414")!=NULL)
			{
				tc_strcpy(MatlClass,"M2");
			}
			else if (tc_strstr(MatlClassTmp,"MON (Modified Organic Naturals) like cotton, jute, namada etc")!=NULL)
			{
				tc_strcpy(MatlClass,"MO");
			}
			else if (tc_strstr(MatlClassTmp,"Nylon/PA")!=NULL)
			{
				tc_strcpy(MatlClass,"NB");
			}
			else if (tc_strstr(MatlClassTmp,"Nn-ferrous metal & alloys")!=NULL)
			{
				tc_strcpy(MatlClass,"NF");
			}
			else if (tc_strstr(MatlClassTmp,"PPO+PA")!=NULL)
			{
				tc_strcpy(MatlClass,"NR");
			}
			else if (tc_strstr(MatlClassTmp,"Oils")!=NULL)
			{
				tc_strcpy(MatlClass,"OI");
			}
			else if (tc_strstr(MatlClassTmp,"Other thermoplastics")!=NULL)
			{
				tc_strcpy(MatlClass,"OT");
			}
			else if (tc_strstr(MatlClassTmp,"PBT/PET")!=NULL)
			{
				tc_strcpy(MatlClass,"PB");
			}
			else if (tc_strstr(MatlClassTmp,"PC+ABS")!=NULL)
			{
				tc_strcpy(MatlClass,"PC");
			}
			else if (tc_strstr(MatlClassTmp,"Polyethylene")!=NULL)
			{
				tc_strcpy(MatlClass,"PE");
			}
			else if (tc_strstr(MatlClassTmp,"Polyacetal")!=NULL)
			{
				tc_strcpy(MatlClass,"PL");
			}
			else if (tc_strstr(MatlClassTmp,"Paints")!=NULL)
			{
				tc_strcpy(MatlClass,"PN");
			}
			else if (tc_strstr(MatlClassTmp,"Polycarbonate")!=NULL)
			{
				tc_strcpy(MatlClass,"PO");
			}
			else if (tc_strstr(MatlClassTmp,"Paper")!=NULL)
			{
				tc_strcpy(MatlClass,"PP");
			}
			else if (tc_strstr(MatlClassTmp,"Polypropylene based")!=NULL)
			{
				tc_strcpy(MatlClass,"PR");
			}
			else if (tc_strstr(MatlClassTmp,"PTFE/teflon")!=NULL)
			{
				tc_strcpy(MatlClass,"PT");
			}
			else if (tc_strstr(MatlClassTmp,"PU foams")!=NULL)
			{
				tc_strcpy(MatlClass,"PU");
			}
			else if (tc_strstr(MatlClassTmp,"PVC")!=NULL)
			{
				tc_strcpy(MatlClass,"PV");
			}
			else if (tc_strstr(MatlClassTmp,"Polyether-ester based TPE")!=NULL)
			{
				tc_strcpy(MatlClass,"PY");
			}
			else if (tc_strstr(MatlClassTmp,"Rubbers (except tyres)")!=NULL)
			{
				tc_strcpy(MatlClass,"RB");
			}
			else if (tc_strstr(MatlClassTmp,"Rubber Tyres")!=NULL)
			{
				tc_strcpy(MatlClass,"RT");
			}
			else if (tc_strstr(MatlClassTmp,"Steel Cold Drawn Bars")!=NULL)
			{
				tc_strcpy(MatlClass,"SB");
			}
			else if (tc_strstr(MatlClassTmp,"Steel Castings")!=NULL)
			{
				tc_strcpy(MatlClass,"SC");
			}
			else if (tc_strstr(MatlClassTmp,"Structural foams")!=NULL)
			{
				tc_strcpy(MatlClass,"SF");
			}
			else if (tc_strstr(MatlClassTmp,"SMC/FRP/BMC")!=NULL)
			{
				tc_strcpy(MatlClass,"SM");
			}
			else if (tc_strstr(MatlClassTmp,"Steering Oil")!=NULL)
			{
				tc_strcpy(MatlClass,"SO");
			}
			else if (tc_strstr(MatlClassTmp,"Steel Tubes")!=NULL)
			{
				tc_strcpy(MatlClass,"ST");
			}
			else if (tc_strstr(MatlClassTmp,"Steel Wires")!=NULL)
			{
				tc_strcpy(MatlClass,"SW");
			}
			else if (tc_strstr(MatlClassTmp,"TIMBER/WOOD")!=NULL)
			{
				tc_strcpy(MatlClass,"WD");
			}
			else if (tc_strstr(MatlClassTmp,"Wash Fluid")!=NULL)
			{
				tc_strcpy(MatlClass,"WF");
			}
			else
			{
				tc_strcpy(MatlClass,"NoMatClass");
				//MatlClass =(char *) MEM_alloc(30);
				//tc_strcpy(MatlClass,MatlClassTmp);
			}
		}
		else
		{
			MatlClass =(char *) MEM_alloc(2);
			tc_strcpy(MatlClass,"NoMatClass");
		}

		printf("    MatlClass::: [%s] ",MatlClass); fflush(stdout);

		if ((tc_strcmp(CompCode,"")==0) || (strlen(CompCode)<=0))
		{
			CompCode =(char *) MEM_alloc(11);
			tc_strcpy(CompCode,"NoCompCode");
		}

		if ((tc_strcmp(ColourInd,"")==0) || (strlen(ColourInd)==0))
		{
			ColourInd =(char *) MEM_alloc(10);
			tc_strcpy(ColourInd," ");
		}
		if ((tc_strcmp(PartType,"")==0) || (strlen(PartType)==0))
		{
			PartType =(char *) MEM_alloc(10);
			tc_strcpy(PartType," ");
		}
		if ((tc_strcmp(Coated,"")==0) || (strlen(Coated)==0))
		{
			Coated =(char *) MEM_alloc(10);
			tc_strcpy(Coated," ");
		}
		if ((tc_strcmp(ReleaseStatus,"")==0) || (strlen(ReleaseStatus)==0))
		{
			ReleaseStatus =(char *) MEM_alloc(10);
			tc_strcpy(ReleaseStatus," ");
		}

		itemRevSeqStr = NULL;
		itemRevSeqStr=(char *) MEM_alloc(10);
		tc_strcpy(itemRevSeqStr,ItemRev);
		tc_strcat(itemRevSeqStr,"*");
		tc_strcat(itemRevSeqStr,ItemSeq);
		
		if ((level0>ParentBodyShellLevel) && (level0>1)&&(tc_strcmp(parentCompCodeBodyShell,"BODY_SHELL")==0))
		{
			printf("\n%s,%s,%s, ,%s,%s,%s,%s,",Item_id_par,ItemRev,ItemSeq,parentCompCodeBodyShell,MatlClass,ReleaseStatus,ColourInd); fflush(stdout);
			fprintf(fp,"%s,%s,%s, ,%s,%s,%s,%s,\n",Item_id_par,ItemRev,ItemSeq,parentCompCodeBodyShell,MatlClass,ReleaseStatus,ColourInd); fflush(fp);
			fprintf(fp2,"%s~%s~%s~ ~%s~%s~%s~%s~\n",blLineName,ItemRev,ItemSeq,parentCompCodeBodyShell,MatlClass,ReleaseStatus,ColourInd); fflush(fp2);
		}
		else
		{
			////Part,Rev,Seq,OwnerName/vault,Compcode,MatClass,RelzStatus,ColourInd,
			printf("\n%s,%s,%s, ,%s,%s,%s,%s,",Item_id_par,ItemRev,ItemSeq,CompCode,MatlClass,ReleaseStatus,ColourInd); fflush(stdout);
			fprintf(fp,"%s,%s,%s, ,%s,%s,%s,%s,\n",Item_id_par,ItemRev,ItemSeq,CompCode,MatlClass,ReleaseStatus,ColourInd); fflush(fp);
			fprintf(fp2,"%s~%s~%s~ ~%s~%s~%s~%s~\n",blLineName,ItemRev,ItemSeq,CompCode,MatlClass,ReleaseStatus,ColourInd); fflush(fp2);
		}
		//54456000PB1002_SE20221_56506002961/NR;2~G~2~ ~BODY_SHELL~FM~STDSIC Working~N~
	}
	//for (i = 0; i < depth; i++)
	//{	printf ("  "); }

	if (strcmp(Item_id_par,"544216211203")!=0)
	{
	ifail = BOM_line_ask_child_lines (bom_line_tag, &no_bom_lines, &child_bom_lines);
	CHECK_FAIL;
	}

	//printf("\n	no_bom_lines: %d ",no_bom_lines);
	fflush(stdout);

	if (no_bom_lines > 0)
	{
		Parentlevel = level0;
		ParentBomLine = bom_line_tag;

		parent_bom = child_bom_lines;
		noOfChildinParent = no_bom_lines;

		if (tc_strcmp(CompCode,"BODY_SHELL")==0)
		{
			strcpy(parentCompCodeBodyShell,"BODY_SHELL");
			ParentBodyShellLevel=level0;
		}
		printf("\nParent--1--Item_id_par::> %d,%s,%s,%s,%d,",level0,Item_id_par,CompCode,parentCompCodeBodyShell,ParentBodyShellLevel);

		if ((level0<=ParentBodyShellLevel)&&(tc_strcmp(CompCode,"BODY_SHELL")!=0))
		{
			strcpy(parentCompCodeBodyShell,"NoCompCode");
			ParentBodyShellLevel=-1;
		}

		printf("\nParent--2--Item_id_par::> %d,%s,%s,%s,%d,",level0,Item_id_par,CompCode,parentCompCodeBodyShell,ParentBodyShellLevel);
		fflush(stdout);
	}

	if (depth <= expansionLevel)
	{
		for (j = 0; j < no_bom_lines; j++)
		{
			ExpandMultiLevelBom (child_bom_lines[j], bom_line_tag, depth,ParentBomLine,parent_bom,noOfChildinParent);
		}

		if (no_bom_lines > 0)
		{
			MEM_free (child_bom_lines);
		}
	}

	MEM_free (name);
}
