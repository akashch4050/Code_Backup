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

static void initialise (void);
static void initialise_attribute (char *name,  int *attribute);
static void ExpandMultiLevelBom (tag_t line, tag_t line1, int depth , tag_t ParentBomLine, tag_t *parent_bom , int noOfChildinParent,char* sSVRsub);
static int my_compare_function (tag_t line_1, tag_t line_2, void *client_data);

tag_t window = NULLTAG;
tag_t *rev_rule = NULLTAG;
FILE* fp=NULL;
FILE* fp1=NULL;
FILE* fptr=NULL;
FILE* fptrC=NULL;
tag_t ParentBomLine = NULLTAG;
int Parentlevel = 0;
int ParentFlag = 0;
int expansionLevel = 0;
tag_t relation_typeCreoGen = NULLTAG;
tag_t relation_typeSpec = NULLTAG;
tag_t relation_typeRender = NULLTAG;
tag_t queryTag = NULLTAG;
tag_t VariqueryTag = NULLTAG;

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
static int VOO_show_wso ( tag_t wso ) {
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
int ValColourableChildUnderParent(tag_t bomline_tag)
{
	int FlagColourablePart = 0;
	int no_bomlines,mn = 0;
	char *ColourInd=NULL;
	char *CompCode=NULL;
	char *PartType=NULL;
	char *Coated = NULL;
	tag_t *child_bomlines = NULLTAG;
	tag_t child_bl_tag = NULLTAG;

	if(BOM_line_ask_child_lines (bomline_tag, &no_bomlines, &child_bomlines)!=ITK_ok);

	if (no_bomlines > 0)
	{
		for (mn = 0; mn < no_bomlines; mn++)
		{
			child_bl_tag=child_bomlines[mn];

			//if( AOM_ask_value_string(child_bl_tag,"bl_T5_ClrPartRevision_t5_ColourInd",&ColourInd)!=ITK_ok);  //uadev
			if( AOM_ask_value_string(child_bl_tag,"bl_Design Revision_t5_ColourInd",&ColourInd)!=ITK_ok);		//UAPROD

			tc_strcpy(CompCode,"");
			//if( AOM_ask_value_string(child_bl_tag,"bl_T5_ClrPartRevision_t5_PrtCatCode",&CompCode)!=ITK_ok);  //uadev
			if( AOM_ask_value_string(child_bl_tag,"bl_Design Revision_t5_PrtCatCode",&CompCode)!=ITK_ok);		//UAPROD

			if( AOM_ask_value_string(child_bl_tag,"bl_Design Revision_t5_Coated",&Coated)!=ITK_ok);

			if (tc_strcmp(ColourInd,"Y")==0)
			{
				FlagColourablePart=FlagColourablePart+1;
				break;
			}
		}
		if (no_bomlines > 0)
		{
			MEM_free (child_bomlines);
		}
	}

	return FlagColourablePart;
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
    char *SVRContxtSub = NULL;
    char *ColSVRCotxt = NULL;
    char *expansionLvl = NULL;
    char *context = NULL;
	char* username = NULL;
	char* inputfile=NULL;
	char* inputfile1=NULL;
	char* inputfile2=NULL;
	char* inputfileC=NULL;
	char* child_item_id=NULL;
	char* ChildId=NULL;
	char* ChildColInd=NULL;
	char* Col_child_item_id=NULL;
	char* sSVRInput=NULL;
	char* sVeh=NULL;
	char *sVehNum=NULL;

	tag_t *ParentBom=NULLTAG;

	tag_t item_rev_tag = NULLTAG;
	tag_t ColItem_rev_tag = NULLTAG;
	tag_t vol=NULLTAG;
	tag_t user_tag = NULLTAG;
	tag_t ParentBomLineTag = NULLTAG;
	tag_t bom_view = NULLTAG;

    tag_t rule, item_tag = null_tag, topline_tag;
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

	printf("\nInput parameters-- platformNm: %s  platformRev: %s  expansionLvl: %s expansionLevel: %d \n",platformNm,platformRev, expansionLvl, expansionLevel);fflush(stdout);

	if ( platformNm )
	{
		POM_get_user(&username,&user_tag);
		printf("\n Session User:[%s]\n",username); fflush(stdout);

		inputfile =(char *) MEM_alloc(100);
		tc_strcpy(inputfile,platformNm);
		tc_strcat(inputfile,"_");
		tc_strcat(inputfile,SVRCotxt);
		tc_strcat(inputfile,"_CompCode.txt");
		fp = fopen(inputfile,"w");
		if(fp == NULL)
		{
			printf ("\n Could not open inputfile file : %s\n", inputfile); fflush(stdout);
			exit( EXIT_FAILURE );
		}

		inputfile2 =(char *) MEM_alloc(100);
		tc_strcpy(inputfile2,platformNm);
		tc_strcat(inputfile2,"_");
		tc_strcat(inputfile2,SVRCotxt);
		tc_strcat(inputfile2,"_Suf_CompCode.txt");
		fp1 = fopen(inputfile2,"w");
		if(fp1 == NULL)
		{
			printf ("\n Could not open inputfile2 file : %s\n", inputfile2); fflush(stdout);
			exit( EXIT_FAILURE );
		}

		inputfile1 =(char *) MEM_alloc(100);
		tc_strcpy(inputfile1,platformNm);
		tc_strcat(inputfile1,"_");
		tc_strcat(inputfile1,SVRCotxt);
		tc_strcat(inputfile1,"_Refresh_CompCode.txt");
		fptr = fopen(inputfile1,"w");
		if(fptr == NULL)
		{
			printf ("\n Could not open inputfile1 file : %s\n", inputfile1); fflush(stdout);
			exit( EXIT_FAILURE );
		}

		inputfileC =(char *) MEM_alloc(100);
		tc_strcpy(inputfileC,platformNm);
		tc_strcat(inputfileC,"_");
		tc_strcat(inputfileC,SVRCotxt);
		tc_strcat(inputfileC,"_Copy_CompCode.txt");
		fptrC = fopen(inputfileC,"w");
		if(fptrC == NULL)
		{
			printf ("\n Could not open inputfileC file : %s\n", inputfileC); fflush(stdout);
			exit( EXIT_FAILURE );
		}

		if(QRY_find("VariantRule", &VariqueryTag));
		printf("\n\t After IFERR_REPORT : QRY_find .... "); fflush(stdout);

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
			printf ("\n NonColour-SVR-VarientRule not present  [%s]\n", SVRCotxt); fflush(stdout);
			exit (0);
		}

		// skip svr applied and bom expansion of X4, instead do expansion of vehicle.
		// Get vehicle nummber from SVR. 54423524000R_A
		
		sSVRInput = subString(SVRCotxt,0,4);
	
		if(strcmp(sSVRInput,"5442")==0)
		{
		sVeh = subString(SVRCotxt,0,8);

		printf ("\n sVehNum [%s]\n", sVeh); fflush(stdout);
		
		sVehNum =(char *) MEM_alloc(100);

		strcpy(sVehNum,"");
		strcat(sVehNum,sVeh);
		strcat(sVehNum,"R");
		
		printf(" \n sVehNum: [%s]",sVehNum);
		}
		
		attrs[0] ="item_id";

		if(strcmp(sSVRInput,"5442")==0)
		{
		values[0] = (char *)sVehNum;
		}
		else
		{
		values[0] = (char *)platformNm;
		}
		ifail = ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found);
		//MEM_free(attrs);
		MEM_free(values);
		CHECK_FAIL;
		if (n_tags_found == 0)
		{
			printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", platformNm); fflush(stdout);
			exit (0);
		}
		else if (n_tags_found > 1)
		{
			MEM_free(tags_found);
			EMH_store_initial_error(EMH_severity_error,ITEM_multiple_items_returned);
			printf ( "More than one items matched with id %s\n", platformNm);fflush(stdout);
			exit (0);
		}
		item_tag = tags_found[0];

		if (item_tag == null_tag)
		{
			printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", platformNm); fflush(stdout);
			exit (0);
		}

		/*
		printf("\n--------------------------------Start: Colour Platform and SVR ------------------------------------------------------------------\n");fflush(stdout);
			
			ColPlatformNm =(char *) MEM_alloc(100);
			ColSVRCotxt =(char *) MEM_alloc(100);
			strcpy(ColPlatformNm,"001443");
			strcpy(ColSVRCotxt,"54426745ABFR_NR");

			if (VariqueryTag !=NULLTAG)
			{
				valuesColSvr[0] = ColSVRCotxt ;

				if(QRY_execute(VariqueryTag, n_entriesV, qry_entries3, valuesColSvr, &resultCountVCol, &outputVCol_tags));

				printf(" Colour-[%s]  n_entriesV:%d   resultCountVCol:%d \n\t",ColSVRCotxt,n_entriesV,resultCountVCol); fflush(stdout);

				if (resultCountVCol > 0)
				{
					VarientRuleColtag = outputVCol_tags[0];
				}
				else
				{
					printf ("\n Colour-SVR-VarientRule not present  [%s]\n", ColSVRCotxt); fflush(stdout);
					exit (0);
				}
			}
			else
			{
				printf("Not Found Query 'VariantRule' \n\t"); fflush(stdout);

				ITK_CALL(POM_logout(false));
				return status;
			}

			printf(" Before ColPlatformNm----- \n"); fflush(stdout);

			attrs[0] ="item_id";
			ValNcolPlatform[0] = (char *)ColPlatformNm;
			ifail = ITEM_find_items_by_key_attributes(1,attrs, ValNcolPlatform, &n_tags_foundC, &tags_foundC);
			printf(" Colour-n_tags_foundC:[%d] \n",n_tags_foundC); fflush(stdout);

			MEM_free(attrs);
			MEM_free(ValNcolPlatform);
			CHECK_FAIL;


			if (n_tags_foundC == 0)
			{
				printf ("Colour-ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", ColPlatformNm); fflush(stdout);
				exit (0);
			}
			else if (n_tags_foundC > 1)
			{
				MEM_free(tags_foundC);
				EMH_store_initial_error(EMH_severity_error,ITEM_multiple_items_returned);
				printf ( "Colour-More than one items matched with id %s\n", ColPlatformNm);fflush(stdout);
				exit (0);
			}
			itemCol_tag = tags_foundC[0];

			if (itemCol_tag == null_tag)
			{
				printf ("Colour-ITEM_find_items_by_key_attributes returns success,  but didn't find  %s\n", ColPlatformNm); fflush(stdout);
				exit (0);
			}

			//ifail = ITEM_find_revisions(itemCol_tag,platformRev,&n_rev,&rev1);
			//printf("\n\n\n\t n_rev: %d \n", n_rev); fflush(stdout);
			//if (n_rev == 0)
			//{
				ifail = ITEM_ask_latest_rev(itemCol_tag , &ColItem_rev_tag);
				CHECK_FAIL;
			//}
			//else if (n_rev == 1)
			//{
			//	item_rev_tag = rev1[0];
			//}
			//else
			//{
			//	for(h=0; h<n_rev; h++)
			//	{
			//		ifail = AOM_ask_value_int(rev1[h],"sequence_id",&org_seq_id_int);
			//		CHECK_FAIL;
			//		currseq = atoi(platformSeq);
			//		printf("\n\t org_seq_id_int: %d",org_seq_id_int); fflush(stdout);
			//		printf("\n\t platformSeq: %s",platformSeq); fflush(stdout);
			//		printf("\n\t seq id is1 :%d",currseq); fflush(stdout);
			//		//item_rev_tag = rev1[h];
			//		if(org_seq_id_int == currseq)
			//		{
			//			item_rev_tag = rev1[h];
			//			printf("\t.. inside tag assign \n"); fflush(stdout);
			//			break;
			//		}
			//	}
			//}
			MEM_free(tags_foundC);

			ITK_CALL ( BOM_create_window( &bom_windowCol ));
			ITK_CALL(CFM_find( "Latest Working", &rule ))
			ITK_CALL(BOM_set_window_config_rule( bom_windowCol, rule ));	
			ITK_CALL(BOM_set_window_pack_all(bom_windowCol, TRUE));
			ITK_CALL(BOM_set_window_top_line( bom_windowCol , NULLTAG, ColItem_rev_tag, NULLTAG, &top_lineCol ));
			printf( " Colour-BOM_set_window_top_line..\n");

			ITK_CALL(BOM_line_ask_child_lines(top_lineCol, &n_lines3, &ColChildLines));
			printf("\n Colour-before hiide LLlast Before setting option n_lines3: %d \n", n_lines3);
			if(bom_windowCol != NULLTAG)
			{
				printf("\n Colour-before inside bom_windowCol-----\n"); fflush(stdout);
				
				ITK_CALL(BOM_window_hide_unconfigured(bom_windowCol));
				ITK_CALL(BOM_window_show_variants(bom_windowCol));
				ITK_CALL(BOM_create_window_variant_config(bom_windowCol,1,&bom_variant_config_Col));
				ITK_CALL(BOM_variant_config_apply (bom_variant_config_Col));

				printf("\n Colour-After inside bom_windowCol-----VarientRuleColtag \n"); fflush(stdout);
				ITK_CALL(BOM_window_apply_variant_configuration(bom_windowCol,1,&VarientRuleColtag));
				printf("\n Colour-After BOM_window_apply_variant_configuration-----\n"); fflush(stdout);
				ITK_CALL(BOM_window_hide_variants(bom_windowCol));
				ITK_CALL(BOM_window_ask_is_modified(bom_windowCol,&modBCol));
				if(modBCol)
				{
					printf( "\n Colour-modified bom window:..\n");
				}
				else
				{
					printf( "\n Colour-not modfiifed ..\n");
				}

				if(BOM_line_ask_child_lines(top_lineCol, &n_lines3, &ColChildLines));
				printf("\n Colour-2nd time LLlast Before setting option n_lines3: %d \n", n_lines3); fflush(stdout);

				if (n_lines3 >0)
				{
					for (p1=0; p1 < n_lines3; p1++ )
					{
						if(ColChildobject) ColChildobject=NULLTAG;
						ColChildobject=ColChildLines[p1];
						ITK_CALL(AOM_ask_value_string(ColChildobject, "bl_item_item_id", &Col_child_item_id ));
						printf(  "\n Colour-Col_child_item_id ..: %s ",Col_child_item_id);
					}
				}
			}
		printf("\n--------------------------------End: Colour Platform and SVR --------------------------------------------------------------------\n");fflush(stdout);
		*/


/*		ITK_CALL(GRM_list_secondary_objects_only(item_tag,NULLTAG,&countV,&secondary_objects));
		printf("\n Smc0HasVariantConfigContext countV :%d ..\n",countV); fflush(stdout);

		if (countV >0)
		{
			for (k=0;k<countV ;k++ )
			{
				primary=secondary_objects[k];

				ITK_CALL(TCTYPE_ask_object_type(primary,&objTypeTagDi));
				ITK_CALL(TCTYPE_ask_name(objTypeTagDi,type_name2));
				printf("\n type_name2 :%s ..\n",type_name2); fflush(stdout);

				ITK_CALL ( VOO_show_wso( primary ) )
				if (strcmp(type_name2,"Cfg0ProductItem")==0)
				{
					ITK_CALL(GRM_list_secondary_objects_only(primary,NULLTAG,&countV1,&secondary_objects1));
					if (countV1 >0)
					{
						for (k1=0;k1<countV1 ;k1++ )
						{
							primary1=secondary_objects1[k1];
							ITK_CALL(TCTYPE_ask_object_type(primary1,&objTypeTagDi1));
							if(TCTYPE_ask_name(objTypeTagDi1,type_name1));
							printf( "\n Config relation type_name111 :%s ..\n",type_name1);
							if (strcmp(type_name1,"VariantRule")==0)
							{
								ITK_CALL( AOM_ask_value_string(primary1,"object_name",&Context_Str));
								printf( "\n Context_Str Vraint rule is :  %s \n",Context_Str);

								//if (strcmp(Context_Str,"SVR_test_1")==0)
								if (strcmp(SVRCotxt,Context_Str)==0)
								{
									ITK_CALL ( BOM_create_window( &bom_window ));
									ITK_CALL(CFM_find( "Latest Working", &rule ))
									ITK_CALL(BOM_set_window_config_rule( bom_window, rule ));	
									ITK_CALL(BOM_set_window_pack_all(bom_window, TRUE));
									ITK_CALL(BOM_set_window_top_line( bom_window , item_tag, NULLTAG, NULLTAG, &top_line ));
									printf( " BOM_set_window_top_line..\n");
									//ITK_CALL ( VOO_show_wso( bom_window ));
									printf( " BOM_set_window_bom_window..\n");

									ITK_CALL(BOM_line_ask_child_lines(top_line, &n_lines1, &lines));
									printf("\n before hiide LLlast Before setting option n_lines1: %d \n", n_lines1);
									if(bom_window != NULLTAG)
									{
										printf("\n before inside bom_window-----\n"); fflush(stdout);
										
										ITK_CALL(BOM_window_hide_unconfigured(bom_window));
										ITK_CALL(BOM_window_show_variants(bom_window));
										ITK_CALL(BOM_create_window_variant_config(bom_window,1,&bom_variant_config));
										ITK_CALL(BOM_variant_config_apply (bom_variant_config));

										//ITK_CALL(BOM_variant_rule_apply(primary1));
										printf("\n After inside bom_window-----\n"); fflush(stdout);
										ITK_CALL(BOM_window_apply_variant_configuration(bom_window,1,&primary1));
										printf("\n After BOM_window_apply_variant_configuration-----\n"); fflush(stdout);

										//ITK_CALL(BOM_window_apply_overlay_variant_rules(bom_window,1,primary1, "SVRACTION_OVERLAY_ADD_RULE", &count, &variant_rule_list));
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
									else
									{
										printf( "\n no bom window found here \n");
									}

									break;
								}
							}
						}
					}
				}
			}
		}
*/

		ifail = ITEM_find_revisions(item_tag,platformRev,&n_rev,&rev1);
		printf("\n\n\n\t n_rev: %d \n", n_rev); fflush(stdout);
		if (n_rev == 0)
		{
			ifail = ITEM_ask_latest_rev(item_tag , &item_rev_tag);
			CHECK_FAIL;
		}
		else if (n_rev == 1)
		{
			item_rev_tag = rev1[0];
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
				//item_rev_tag = rev1[h];

				if(org_seq_id_int == currseq)
				{
					item_rev_tag = rev1[h];
					printf("\t.. inside tag assign \n"); fflush(stdout);

					break;
				}
			}
		}
		MEM_free(tags_found);
	}  //If end


	ifail = ITEM_rev_list_bom_view_revs(item_rev_tag, &numBVRs, &bvr_tags);

	if ( numBVRs == 0 )
	{
		printf ( " Error : No BOM View Revision Exists \n\n " ); fflush(stdout);
		printf ( " Exiting the program \n " ); fflush(stdout);
		exit(0);
	}
	ifail = PS_ask_bom_view_of_bvr(bvr_tags[0],&bom_view) ;

	ITK_CALL ( BOM_create_window( &bom_window ));
	//ITK_CALL(CFM_find( "Latest Working", &rule ));
	if(strcmp(sSVRInput,"5442")==0)
	{
	ITK_CALL(CFM_find( "APLC Release and above", &rule ));
	//ITK_CALL(CFM_find( "APL Release And Above-data download", &rule ));
	//ITK_CALL(CFM_find( "Latest Working", &rule ));
	}
	else
	{
	ITK_CALL(CFM_find( "ERC release and above", &rule ));
	}
	ITK_CALL(BOM_set_window_config_rule( bom_window, rule ));	
	ITK_CALL(BOM_set_window_pack_all(bom_window, TRUE));
	//ITK_CALL(BOM_set_window_top_line( bom_window , item_tag, NULLTAG, NULLTAG, &top_line ));
	ITK_CALL(BOM_set_window_top_line( bom_window , NULLTAG, item_rev_tag, NULLTAG, &top_line ));
	printf( " BOM_set_window_top_line..\n");

	ITK_CALL(BOM_line_ask_child_lines(top_line, &n_lines1, &lines));
	printf("\n before hiide LLlast Before setting option n_lines1: %d \n", n_lines1);
	if(bom_window != NULLTAG)
	{
		printf("\n before inside bom_window-----\n"); fflush(stdout);
		
		SVRContxtSub = subString(SVRCotxt,0,4);
	
		printf("\n SVRContxtSub %s \n",SVRContxtSub); fflush(stdout);
		
		if(strcmp(SVRContxtSub,"5442")==0)
		{
			printf("\n SVRContxtSub %s , so do not apply svr on bom window. \n",SVRContxtSub); fflush(stdout);
		}
		else
		{
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
		}

		if(BOM_line_ask_child_lines(top_line, &n_lines1, &lines));
		printf("\n 2nd time LLlast Before setting option n_lines1: %d \n", n_lines1); fflush(stdout);


	}


	printf("\n Before ExpandMultiLevelBom Function----------\n"); fflush(stdout);

	printf("\nPartnumber,Rev,Seq,CompCode,InternalSchme,ColourInd,"); fflush(stdout);
	//fprintf(fp,"Partnumber,Rev,Seq,CompCode,InternalSchme,ColourInd,\n"); fflush(fp);
	ParentFlag = 0;

//	ExpandMultiLevelBom (topline_tag, NULLTAG, 0 , ParentBomLineTag , ParentBom , 0);
	ExpandMultiLevelBom (top_line, NULLTAG, 0 , ParentBomLineTag , ParentBom , 0,SVRContxtSub);
	
	if(strcmp(SVRContxtSub,"5442")==0)
	{		
	fprintf(fp,"VC^T^Exterior+Interior^\n"); fflush(fp);
	fprintf(fptr,"VC^\n"); fflush(fptr);
	}
	
	fclose(fp);
	fclose(fp1);
	fclose(fptr);
	fclose(fptrC);

	printf ("\n================================================\n");


    ITK_exit_module(true);

	return status;
}
static void ExpandMultiLevelBom (tag_t bom_line_tag, tag_t line1, int depth , tag_t ParentBomLine , tag_t *parent_bom , int noOfChildinParent,char* sSVRsub)
{
	int ifail;
	int i, no_bom_lines , j ,jr, k=0;
	int n_tags_found= 0;
	int iChildItemTag;
	int level0=0;
	int m = 0;
	int group_child_flag = 0;
	int status_count=0;
	int n_entries = 2;
	int resultCount = 2;
	int CompCodeCtrlCount = 0;

	char *name, *sequence_no;
	char *word=NULL;
	char *Item_id_par=NULL;
	char *ItemRevSeq=NULL;
	char *ItemRevSeqTemp=NULL;
	char *ItemRevStr=NULL;
	char *ItemRev=NULL;
	char *itemRevSeqStr=NULL;
	char *ItemSeq=NULL;
	//char *ProjectCode=NULL;
	//char *DesignGrp=NULL;
	char *ColourInd=NULL;
	char *CompCode=NULL;
	char *PartType=NULL;
	char *Coated = NULL;
	char *Parent_name=NULL;
	char *t5InternalSchme=NULL;
	char *Category=NULL;
	char *sObjType=NULL;

	char *qry_entries4[2] = {"Revision","ID"},
		 *qry_values4[2]	= {"*","*"};

	tag_t *child_bom_lines;
	tag_t *tags_found = NULL;
	tag_t *rev=NULLTAG;
	tag_t *compCodeCtrl_tags=NULLTAG;
	
	tag_t item=NULLTAG;
	tag_t reva=NULLTAG;
	tag_t t_ChildItemRev=NULLTAG;
	tag_t dataset = NULLTAG;
	tag_t refobject = NULLTAG;
	tag_t refobjectG = NULLTAG;
	tag_t datasetG = NULLTAG;
	tag_t CreoGenRev = NULLTAG;
	tag_t child_tag = NULLTAG;
	tag_t CompCodeQryTag = NULLTAG;

	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));

	char **valuesCompCode = (char **) MEM_alloc(1 * sizeof(char *));

	//char *qry_entries[1] = {"Colour ID"},	//uadev
	char *qry_entries[1] = {"Comp Code"},	//UAPROD
		 *qry_values[1]	= {"*"};



	CompCode =(char *) MEM_alloc(100);

	depth ++;

	//printf("   In ExpandMultiLevelBom Function....................\n");

	ifail = BOM_line_ask_attribute_string (bom_line_tag, name_attribute, &name);
	CHECK_FAIL;
	word = strtok(name, "/");
	//printf("\n word: [%s]",word); fflush(stdout);
	/* note that I know name is always defined,  but sometimes sequence number is unset.
	If that happens it returns NULL,  not an error.
	*/
	ifail = BOM_line_ask_attribute_string (bom_line_tag, seqno_attribute, &sequence_no);
	CHECK_FAIL;

	ifail = BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
	ifail = BOM_line_ask_attribute_tag(bom_line_tag, iChildItemTag, &t_ChildItemRev);
	CHECK_FAIL;

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
	//if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_ProjectCode",&ProjectCode)!=ITK_ok);
	if( AOM_ask_value_string(bom_line_tag,"bl_item_object_type",&sObjType)!=ITK_ok);//Architecture Module
	if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_PartType",&PartType)!=ITK_ok);
	if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_Coated",&Coated)!=ITK_ok);
	
	//if( AOM_ask_value_string(bom_line_tag,"bl_T5_ClrPartRevision_t5_ColourInd",&ColourInd)!=ITK_ok);  //uadev
	if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_ColourInd",&ColourInd)!=ITK_ok);		//UAPROD

	tc_strcpy(CompCode,"");
	//if( AOM_ask_value_string(bom_line_tag,"bl_T5_ClrPartRevision_t5_PrtCatCode",&CompCode)!=ITK_ok);  //uadev
	if( AOM_ask_value_string(bom_line_tag,"bl_Design Revision_t5_PrtCatCode",&CompCode)!=ITK_ok);		//UAPROD

	printf("\n Item_id_par:[%s]    ColourInd:[%s]    CompCode:[%s] sObjType:[%s]",Item_id_par, ColourInd, CompCode,sObjType); fflush(stdout);

	ItemRev = strtok(ItemRevSeqTemp,";");
    ItemSeq = strtok(NULL,"';'");

	//if (tc_strcmp(ProjectCode,"")==0)
	//{
	//	ProjectCode =(char *) MEM_alloc(10);
	//	tc_strcpy(ProjectCode," ");
	//}
	//if (tc_strcmp(DesignGrp,"")==0)
	//{
	//	DesignGrp =(char *) MEM_alloc(10);
	//	tc_strcpy(DesignGrp," ");
	//}
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

	itemRevSeqStr = NULL;
	itemRevSeqStr=(char *) MEM_alloc(10);
	tc_strcpy(itemRevSeqStr,ItemRev);
	tc_strcat(itemRevSeqStr,"*");
	tc_strcat(itemRevSeqStr,ItemSeq);

	//Querying Bomline revision to extract CAD/JT named reference
//	qry_values[0] = itemRevSeqStr ;
//	qry_values[1] = Item_id_par;

//	if(QRY_execute(queryTag, n_entries, qry_entries4, qry_values, &resultCount, &rev));
//	printf(" [%s---%s]  n_entries:%d   resultCount:%d \n\t",itemRevSeqStr, Item_id_par, n_entries, resultCount); fflush(stdout);
//	if(resultCount>0)
//	{
//		printf("\n....1....\n"); fflush(stdout);

		/*for (jr=0; jr<resultCount; jr++ )
		{
			printf("\n...2.....\n"); fflush(stdout);
			reva = rev[jr];
			printf("\n...3.....\n"); fflush(stdout);

			ifail = AOM_ask_value_string(reva,"item_revision_id",&ItemRevStr);
			CHECK_FAIL;

			if (tc_strcmp(ItemRevSeq,ItemRevStr)==0)
			{
				printf("\n...ItemRevision is matching ....."); fflush(stdout);
				break;
			}
		}*/
//		reva = rev[0];
//	}

	//if((strcmp(sSVRsub,"5442")==0) && (tc_strcmp(CompCode,"BODY_SHELL")==0))
	//	{
	//		printf(" \n SVR is 5442 and comp code is BODY_SHELL, so skip to print in file \n");fflush(stdout);
	//	}
	//else
		//{
		//printf(" \n Regular process.. SVR is not 5442 \n");fflush(stdout);

		if ( ((strlen(CompCode)>0) && (tc_strcmp(ColourInd,"Y")==0)) || (tc_strcmp(sObjType,"T5_ArchModule")==0) || (tc_strcmp(sObjType,"T5_Platform")==0))
		{
			int ColourablePartFlag = 0;

			if(QRY_find("CompCode", &CompCodeQryTag));
			printf("\n\n\t After IFERR_REPORT : QRY_find .... "); fflush(stdout);

			if (CompCodeQryTag)
			{
				printf("\n\t Found Query 'CompCode' "); fflush(stdout);
				valuesCompCode[0] = CompCode ;

				if(QRY_execute(CompCodeQryTag, 1, qry_entries, valuesCompCode, &CompCodeCtrlCount, &compCodeCtrl_tags));

				printf("[%s]   CompCodeCtrlCount:%d ",CompCode,CompCodeCtrlCount); fflush(stdout);

				if (CompCodeCtrlCount > 0)
				{
					if( AOM_ask_value_string(compCodeCtrl_tags[0],"t5_InternalSchme",&t5InternalSchme)!=ITK_ok);
					if( AOM_ask_value_string(compCodeCtrl_tags[0],"t5_CcodePlatform",&Category)!=ITK_ok);
				}
				else
				{
					printf(" CompCodeNotFound ????"); fflush(stdout);
				}

				if (tc_strcmp(t5InternalSchme,"")==0)
				{
					t5InternalSchme = (char*) malloc(3);
					strcpy(t5InternalSchme,"-");
				}
				if (tc_strcmp(Category,"")==0)
				{
					Category = (char*) malloc(3);
					strcpy(Category,"-");
				}

				//printf("\n\t "); fflush(stdout);

				printf("\n%s,%s,%s,%s,%s,%s,t5InternalSchme: [%s,%s]",Item_id_par,ItemRev,ItemSeq,CompCode,t5InternalSchme,ColourInd,t5InternalSchme,Category); fflush(stdout);
				
				if(tc_strlen(CompCode)>0)
				{
				if (tc_strstr(CompCode,"CCA_")!=NULL)
				{
					ColourablePartFlag=ValColourableChildUnderParent(bom_line_tag);
					printf("---ColourablePartFlag:%d",ColourablePartFlag);fflush(stdout);

					if (ColourablePartFlag>0)
					{
						fprintf(fp,"%s^%s^%s^\n",CompCode,t5InternalSchme,Category); fflush(fp);
						fprintf(fptrC,"%s^%s^\n",CompCode,t5InternalSchme); fflush(fptrC);
						fprintf(fp1,"%s^%s^%s^%s^\n",CompCode,t5InternalSchme,Category,Item_id_par); fflush(fp1);
						fprintf(fptr,"%s^\n",CompCode); fflush(fptr);
					}
				}
				else
				{
					fprintf(fp,"%s^%s^%s^\n",CompCode,t5InternalSchme,Category); fflush(fp);
					fprintf(fptrC,"%s^%s^\n",CompCode,t5InternalSchme); fflush(fptrC);
					fprintf(fp1,"%s^%s^%s^%s^\n",CompCode,t5InternalSchme,Category,Item_id_par); fflush(fp1);
					fprintf(fptr,"%s^\n",CompCode); fflush(fptr);
				}
				}
				else
				{
					printf("\n comp code length is 0 \n"); fflush(stdout);
				}
			}
			else
			{
				printf("\n Not Found Query 'CompCode' [%s-%s-%s-%s]",Item_id_par,ItemRev,ItemSeq,CompCode); fflush(stdout);
			}
	
	//for (i = 0; i < depth; i++)
	//{	printf ("  "); }
	

	ifail = BOM_line_ask_child_lines (bom_line_tag, &no_bom_lines, &child_bom_lines);
	CHECK_FAIL;

	printf("\n	no_bom_lines: %d ",no_bom_lines);
	fflush(stdout);

	if (no_bom_lines > 0)
	{
		Parentlevel = level0;
		ParentBomLine = bom_line_tag;

		parent_bom = child_bom_lines;
		noOfChildinParent = no_bom_lines;

		//printf("\n	Parent Item_id_par: %s ",Item_id_par);
		fflush(stdout);
	}

	if (depth <= expansionLevel)
	{
		for (j = 0; j < no_bom_lines; j++)
		{
			ExpandMultiLevelBom (child_bom_lines[j], bom_line_tag, depth,ParentBomLine,parent_bom,noOfChildinParent,sSVRsub);
		}

		if (no_bom_lines > 0)
		{
			MEM_free (child_bom_lines);
		}
	}
		}
		//}
	}

	MEM_free (name);
	MEM_free (sequence_no);
}
static int my_compare_function (tag_t line_1, tag_t line_2, void * client_data)
{
	/* returns strcmp style -1/0/+1 according to whether line_1 and line_2 sort <, = or > */
	char *seq1, *seq2;
	int ifail, result;
	(void)client_data;
	ifail = BOM_line_ask_attribute_string (line_1, seqno_attribute, &seq1);
	CHECK_FAIL;
	ifail = BOM_line_ask_attribute_string (line_2, seqno_attribute, &seq2);
	CHECK_FAIL;
	if (seq1 == NULL || seq2 == NULL)
	result = 0;
	else
	result = strcmp (seq2, seq1);  /* we are being silly and doing a reverse sort */
	/* note:  the default sort function compares Item names if the sequence numbers sort equal
	but this is just an example for fun,  so we won't bother with that bit
	*/
	MEM_free (seq1);
	MEM_free (seq2);
	return result;
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
