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
tag_t window = NULLTAG;
tag_t *rev_rule = NULLTAG;
FILE* fp=NULL;
FILE* fpClrSchm=NULL;
FILE* fpRGB=NULL;
tag_t ParentBomLine = NULLTAG;
int Parentlevel = 0;
int ParentFlag = 0;
int expansionLevel = 0;
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
/*
SPECKLD_SIVR:RGB,240,228,226:Ambient,0,0,0:Diffuse,240,228,226:Specular,232,228,188:Emissive,0,0,0:Transparency,0:Shininess,.1
SchmColourID:
RGB:t5_BaseRed,t5_BaseGreen,t5_BaseBlue
:Ambient,t5_AmbientRed,t5_AmbientGreen,t5_AmbientBlue
:Diffuse,t5_DiffuseRed,t5_DiffuseGreen,t5_DiffuseBlue
:Specular,t5_SpecularRed,t5_SpecularGreen,t5_SpecularBlue
:Emissive,t5_EmissiveRed,t5_EmissiveGreen,t5_EmissiveBlue
:Transparency,t5_Transparency
:Shininess,t5_Shininess
*/
extern int ITK_user_main (int argc, char ** argv )
{
    int ifail;
	int status;
	int resultColID = 0;

	char *inputFile = NULL;
	char *outputFile = NULL;
	char *buffer = NULL;
	char *colourIDTmp = NULL;
	char *colourID = NULL;
	int BaseRed = 0;
	int BaseGreen = 0;
	int BaseBlue = 0;
	int AmbientRed = 0;
	int AmbientGreen = 0;
	int AmbientBlue = 0;
	int DiffuseRed = 0;
	int DiffuseGreen = 0;
	int DiffuseBlue = 0;
	int SpecularRed = 0;
	int SpecularGreen = 0;
	int SpecularBlue = 0;
	int EmissiveRed = 0;
	int EmissiveGreen = 0;
	int EmissiveBlue = 0;
	char *Transparency = NULL;
	char *Shininess= NULL;

	char *qry_entries5[1] = {"Colour ID"};
	char **valuesColID = (char **) MEM_alloc(1 * sizeof(char *));

	FILE* fp=NULL;
	FILE* fpOut=NULL;

	tag_t *ColID_tags = NULLTAG;
	
	tag_t ClrIdMstrQryTag = NULLTAG;
	tag_t ColorIDtag = NULLTAG;

    (void)argc;
    (void)argv;


    inputFile = ITK_ask_cli_argument("-i=");
    outputFile = ITK_ask_cli_argument("-o=");

	initialise();

	printf("\ninputFile: %s      outputFile: %s \n",inputFile,outputFile);fflush(stdout);

	if ( inputFile )
	{
		if(QRY_find("Colour Master", &ClrIdMstrQryTag));
		if (ClrIdMstrQryTag)
		{
			printf("\n ClrCxtJT=Found Query ClrIdMstrQryTag\n");fflush(stdout);
		}
		else
		{
			printf("\n ClrCxtJT=NOT Found Query ClrIdMstrQryTag\n");fflush(stdout);
		}

		fp=fopen(inputFile,"r");
		fpOut=fopen(outputFile,"a");

		if(fp!=NULL)
		{
			buffer=(char *) MEM_alloc(100);
			while(fgets(buffer,100,fp)!=NULL)
			{
				colourIDTmp = (char *) malloc(strlen(buffer));
				strncpy(colourIDTmp, buffer, strlen(buffer) - 1);
				colourIDTmp[strlen(buffer)-1] = '\0';

				if (ClrIdMstrQryTag)
				{
					colourID = (char *) malloc(strlen(buffer)+1);
					strcpy(colourID,colourIDTmp);
					strcat(colourID,"*");

					printf("\n colourID: [%s]",colourID);fflush(stdout);

					valuesColID[0] = colourID;
					if(QRY_execute(ClrIdMstrQryTag, 1, qry_entries5, valuesColID, &resultColID, &ColID_tags));
					printf(" resultColID: %d",resultColID);fflush(stdout);
					
					if (resultColID > 0)
					{
						ColorIDtag = ColID_tags[0];

						if(AOM_ask_value_int(ColorIDtag, "t5_BaseRed",   &BaseRed )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_BaseGreen", &BaseGreen )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_BaseBlue",  &BaseBlue )!=ITK_ok);

						if(AOM_ask_value_int(ColorIDtag, "t5_AmbientRed",   &AmbientRed )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_AmbientGreen", &AmbientGreen )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_AmbientBlue",  &AmbientBlue )!=ITK_ok);
						
						if(AOM_ask_value_int(ColorIDtag, "t5_DiffuseRed",   &DiffuseRed )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_DiffuseGreen", &DiffuseGreen )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_DiffuseBlue",  &DiffuseBlue )!=ITK_ok);

						if(AOM_ask_value_int(ColorIDtag, "t5_SpecularRed",  &SpecularRed )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_SpecularGreen",&SpecularGreen )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_SpecularBlue", &SpecularBlue )!=ITK_ok);

						if(AOM_ask_value_int(ColorIDtag, "t5_EmissiveRed",  &EmissiveRed )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_EmissiveGreen",&EmissiveGreen )!=ITK_ok);
						if(AOM_ask_value_int(ColorIDtag, "t5_EmissiveBlue", &EmissiveBlue )!=ITK_ok);

						if(AOM_ask_value_string(ColorIDtag, "t5_Transparency", &Transparency )!=ITK_ok);
						if(AOM_ask_value_string(ColorIDtag, "t5_Shininess",    &Shininess )!=ITK_ok);

						printf("\n%s:RGB,%d,%d,%d:Ambient,%d,%d,%d:Diffuse,%d,%d,%d:Specular,%d,%d,%d:Emissive,%d,%d,%d:Transparency,%s:Shininess,%s",colourIDTmp,BaseRed,BaseGreen,BaseBlue,AmbientRed,AmbientGreen,AmbientBlue,DiffuseRed,DiffuseGreen,DiffuseBlue,SpecularRed,SpecularGreen,SpecularBlue,EmissiveRed,EmissiveGreen,EmissiveBlue,Transparency,Shininess); fflush(stdout);
						fprintf(fpOut,"%s:RGB,%d,%d,%d:Ambient,%d,%d,%d:Diffuse,%d,%d,%d:Specular,%d,%d,%d:Emissive,%d,%d,%d:Transparency,%s:Shininess,%s\n",colourIDTmp,BaseRed,BaseGreen,BaseBlue,AmbientRed,AmbientGreen,AmbientBlue,DiffuseRed,DiffuseGreen,DiffuseBlue,SpecularRed,SpecularGreen,SpecularBlue,EmissiveRed,EmissiveGreen,EmissiveBlue,Transparency,Shininess); fflush(stdout);
						//SPECKLD_SIVR:RGB,240,228,226:Ambient,0,0,0:Diffuse,240,228,226:Specular,232,228,188:Emissive,0,0,0:Transparency,0:Shininess,.1
					}
				}
			} // for loop end
		}
	}  //If end

	printf ("\n================================================\n");

    ITK_exit_module(true);

	return status;
}
