#define TE_MAXLINELEN  128
#include <ae/dataset.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ctype.h>
#include <time.h>
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
#define ONE "1-Mtr"
#define TWO "2-Kg"
#define THREE "3-Ltr"
#define FOUR "4-Nos"
#define FIVE "5-Sq.Mtr"
#define SIX "6-Sets"
#define SEVEN "7-Tonne"
#define EIGHT "8-Cu.Mtr"
#define NINE "9-Thsnds"
#define EIGHT "8-Cu.Mtr"
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
static int my_compare_function (tag_t line_1, tag_t line_2, void *client_data);

tag_t SchmWithCmpcdRelMultlvl = NULLTAG;
tag_t window = NULLTAG;
tag_t *rev_rule = NULLTAG;
FILE* fp=NULL;
tag_t ParentBomLine = NULLTAG;
int Parentlevel = 0;
int ParentFlag = 0;
int expansionLevel = 0;
tag_t relation_typeCreoGen = NULLTAG;
tag_t relation_typeSpec = NULLTAG;
tag_t relation_typeRender = NULLTAG;
tag_t queryTag = NULLTAG;
tag_t VariqueryTag = NULLTAG;
char  *ClrSchmToVf	= NULL;
char  *ClrRevSeq = "NR;1";
char *SchDataToGov = NULL;
int item_revision_id_int =1;
tag_t ColSchmRevTag=NULLTAG;
tag_t taskRevTag = NULLTAG;
double qty = 0.00;
int qty1 = 1;
tag_t user_tag = NULLTAG;
tag_t group = NULLTAG;
FILE		*output 			= NULL;

static char *VOO_prog = (char *)"generate_variant_overlay_option_constraints";

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

static char* default_empty_to_A(char *s)
{
    return (NULL == s ? s : ('\0' == s[0] ? "A" : s));
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

int setAttributesOnDesign(tag_t* item,char * item_id,char* desc,char* UnitOfMeasureS)
{
	int status;
	char* ent_uom=NULL;
	tag_t unit_of_measure=NULLTAG;
	ent_uom=(char *) MEM_alloc(10 * sizeof(char *));
	if(strcmp(UnitOfMeasureS,"1")==0){strcpy(ent_uom,ONE);}
	else if(strcmp(UnitOfMeasureS,"2")==0){strcpy(ent_uom,TWO);}
	else if(strcmp(UnitOfMeasureS,"3")==0){strcpy(ent_uom,THREE);}
	else if(strcmp(UnitOfMeasureS,"4")==0){strcpy(ent_uom,FOUR);}
	else if(strcmp(UnitOfMeasureS,"5")==0){strcpy(ent_uom,FIVE);}
	else if(strcmp(UnitOfMeasureS,"6")==0){strcpy(ent_uom,SIX);}
	else if(strcmp(UnitOfMeasureS,"7")==0){strcpy(ent_uom,SEVEN);}
	else if(strcmp(UnitOfMeasureS,"8")==0){strcpy(ent_uom,EIGHT);}
	else {strcpy(ent_uom,"");}
	printf("\n ent_uom ... [%s]",ent_uom);
	
	printf("\n item_id--------->%s\n",item_id);
	printf("\n object_name------------>%s\n",item_id);
	printf("\n object_desc:%s\n",desc);
	printf("\n UnitOfMeasureS:%s\n",UnitOfMeasureS);	
	
	ITK_CALL(AOM_lock(*item));
	if(strcmp(UnitOfMeasureS,"4")!=0)
	{
		ITK_CALL(UOM_find_by_symbol(ent_uom,&unit_of_measure));
		ITK_CALL(ITEM_set_unit_of_measure(*item, unit_of_measure));
	}
	ITK_CALL(AOM_set_value_string(*item,"item_id",item_id));
	ITK_CALL(AOM_set_value_string(*item,"object_name",item_id));
	ITK_CALL(AOM_set_value_string(*item,"object_desc",desc));
	ITK_CALL(AOM_save(*item));
	ITK_CALL(AOM_unlock(*item));	
}

int setAttributesOnDesignRev(tag_t* rev,char* item_id,int sequence_id,char* desc,char* ColourIndS,char* t5PrtCatCodeS,char* t5ColourIDS,char* t5CoatedS,char* t5PartStatusS,char* designgroup,char* projectcode,char* mod_desc,char* DrawingIndS,char * PartTypeS,char * t5PartCodeS,char * t5AhdMakeBuyIndS,char * t5PunPCBUMakeBuyIndS,char * t5PunMakeBuyIndS,char * t5DwdMakeBuyIndS,char * NonColorPart)
{
	int status;
	double dweight=0;
	tag_t  projobj=NULLTAG;
	
	printf ( "\n Global SchDataToGov is -------> %s\n",SchDataToGov);fflush(stdout);
	ITK_CALL(AOM_lock(*rev));	
	ITK_CALL(AOM_set_value_string(*rev,"object_desc",desc));
	ITK_CALL(AOM_set_value_string(*rev,"t5_ColourInd","C"));
	ITK_CALL(AOM_set_value_string(*rev,"t5_PrtCatCode",t5PrtCatCodeS));
	ITK_CALL(AOM_set_value_string(*rev,"t5_ColourID",t5ColourIDS));
	ITK_CALL(AOM_set_value_string(*rev,"t5_Coated","N"));
	ITK_CALL(AOM_set_value_string(*rev,"t5_NcPartNo",NonColorPart));
	ITK_CALL(AOM_set_value_string(*rev,"t5_PartStatus",t5PartStatusS));
	ITK_CALL(AOM_set_value_string(*rev,"t5_DesignGrp",designgroup));
	ITK_CALL(AOM_set_value_string(*rev,"t5_ProjectCode",projectcode));
	ITK_CALL(AOM_set_value_string(*rev,"t5_DocRemarks",mod_desc));
	ITK_CALL(AOM_set_value_string(*rev,"gov_classification",SchDataToGov));

	ITK_CALL(AOM_set_value_string(*rev,"t5_DrawingInd",DrawingIndS));
	if (strstr(PartTypeS,"D")!=NULL) { ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","D")); }
	if (strstr(PartTypeS,"A")!=NULL) { ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","A")); }
	if (strstr(PartTypeS,"C")!=NULL) { ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","C")); }
	if (strstr(PartTypeS,"VC")!=NULL) { ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","VC")); }
	if (strstr(PartTypeS,"V")!=NULL) { ITK_CALL ( AOM_set_value_string(*rev,"t5_PartType","V")); }

	ITK_CALL(AOM_set_value_string(*rev,"t5_PartCode",t5PartCodeS));
	if(tc_strcmp(t5AhdMakeBuyIndS,"-")==0) { ITK_CALL(AOM_set_value_string(*rev,"t5_AhdMakeBuyIndicator","")); }else
	{
		ITK_CALL(AOM_set_value_string(*rev,"t5_AhdMakeBuyIndicator",t5AhdMakeBuyIndS));
	}

	if(tc_strcmp(t5PunPCBUMakeBuyIndS,"-")==0) { ITK_CALL(AOM_set_value_string(*rev,"t5_PunPCBUMakeBuyIndicator",""));
	}else
	{
		ITK_CALL(AOM_set_value_string(*rev,"t5_PunPCBUMakeBuyIndicator",t5PunPCBUMakeBuyIndS));
	}

	if(tc_strcmp(t5PunMakeBuyIndS,"-")==0) { ITK_CALL(AOM_set_value_string(*rev,"t5_PunMakeBuyIndicator",""));
	}else
	{
		ITK_CALL(AOM_set_value_string(*rev,"t5_PunMakeBuyIndicator",t5PunMakeBuyIndS));
	}
	
	if(tc_strcmp(t5DwdMakeBuyIndS,"-")==0) { ITK_CALL(AOM_set_value_string(*rev,"t5_DwdMakeBuyIndicator",""));
	}else
	{
		ITK_CALL(AOM_set_value_string(*rev,"t5_DwdMakeBuyIndicator",t5DwdMakeBuyIndS));
	}	
		
	ITK_CALL(AOM_save(*rev));
	ITK_CALL(AOM_unlock(*rev));
}

int CreateReleasestatus(tag_t ColourRev)
{
		tag_t   release_status =NULLTAG;
		char   *ReleaseStatus=NULL;
		char* ReviewStatus=NULL;
		int status;
		int		ifail	=0;
		int	   status_count=0;
		tag_t* status_list = NULLTAG;
		logical  retain_released_date =false;

		ReviewStatus =(char *) MEM_alloc(20 * sizeof(char *));
		tc_strcpy(ReviewStatus,"T5_LcsReview");
		printf("\n ReviewStatus --> %s\n",ReviewStatus);

		if((strcmp(ReviewStatus,"T5_LcsReview")==0))
		{
			if(ITK_ok != (ifail = WSOM_ask_release_status_list(ColourRev,&status_count,&status_list)))
			{
			   return ifail;
			}

			printf("\n REV status_count: %d\n",status_count);fflush(stdout);

			if (status_count == 0)
			{
				printf("\n No Status, so the Item is not yet Released \n");
				printf("\n *********Stamping Releasing item*******......\n");
				if(ITK_ok != (ifail = CR_create_release_status(ReviewStatus,&release_status)))	 return ifail;

				if(ITK_ok != (ifail = AOM_ask_name(release_status, &ReleaseStatus))) return ifail;
				printf("\n ******************* ReleaseStatus: %s\n",ReleaseStatus);fflush(stdout);

				printf("\n release_status \n");
				ITK_CALL( EPM_add_release_status(release_status,1,&ColourRev,retain_released_date));
			}
		}
  return status;
}

int AssignProject(tag_t  itemRev,char* projectcode)
{
	int status;
	tag_t  projobj=NULLTAG;
	char* username=NULL;
	tag_t user_tag=NULLTAG;

	printf("\n projectcode [%s]\n",projectcode);fflush(stdout);
	ITK_CALL (PROJ_find(projectcode,&projobj));
	printf("\n Assigned to Project111\n");fflush(stdout);

	if(projobj !=NULLTAG)
	{
		ITK_CALL (PROJ_assign_objects(1 ,&projobj ,1 ,&itemRev));
		printf("\n Assigned to Project\n");fflush(stdout);
	}
	else
	{
		printf("\n Project not found\n");fflush(stdout);
	}
  return status;
}

int NonClrOccursetToClrBvr(tag_t  clrbvr, char* UsesPartNo, int NClrQty3)
{	
	int          stat                = ITK_ok;
	int			 status;
	int			 ifail				 = ITK_ok;
	int n_occs;
	tag_t *occsal;
	int ChildOccNon,iy,i=0;
	tag_t	ChildItem		= NULLTAG;
	tag_t	bvrchild		= NULLTAG;
	char	*occ_item_id	= NULL;
	tag_t  *occs=NULL;
	tag_t        UseMaster           = NULLTAG;
	tag_t        Nrev                = NULLTAG;

	ITK_CALL(ITEM_find_item(UsesPartNo,&UseMaster))
	printf("\n UseMaster found------>\n");
	
	ITK_CALL(ITEM_ask_latest_rev(UseMaster, &Nrev));
	printf("\n Childrev found------>\n");

	ITK_CALL(PS_list_occurrences_of_bvr(clrbvr, &n_occs, &occsal));
	printf("\n InMain  no:of:occurances are --->%d\n",n_occs); fflush(stdout);
	ChildOccNon=0;
	if(n_occs > 0)
	{	
		 for (iy = 0; iy<n_occs; iy++)
		 {
			 ITK_CALL(PS_ask_occurrence_child(clrbvr,occsal[iy], &ChildItem, &bvrchild));
			 ITK_CALL(AOM_ask_value_string(ChildItem, "item_id", &occ_item_id));
			 printf("\n occ_item_id & child_item_id  :%s=%s\n\n",occ_item_id,UsesPartNo);fflush(stdout);
			 if(tc_strcmp(occ_item_id,UsesPartNo)==0)
			 {
				ChildOccNon=1;
				break;
			 }								 
		 }
	}
	
	printf("\n ChildOccNon values is----------------->%d\n",ChildOccNon); fflush(stdout);
	printf("\n NClrQty3----------------->%d\n",NClrQty3); fflush(stdout);
	if(ChildOccNon==0)
	{
		if(NClrQty3>0)
		{
			for (i=0; i<NClrQty3; i++)
			{	
				ITK_CALL(PS_create_occurrences(clrbvr, Nrev, NULLTAG,1, &occs ))
				printf("\n PS_create_occurrences successfully in PSE------->\n");
			}
		}else if (NClrQty3==0)
		{
			ITK_CALL(PS_create_occurrences(clrbvr, Nrev, NULLTAG,1, &occs ))
			printf("\n PS_create_occurrences successfully in PSE------->\n");
		}
		ITK_CALL(AOM_save(clrbvr))
		ITK_CALL(AOM_refresh(clrbvr,0));
	}	
	return ITK_ok;
}

int CarOptionalCSSet(tag_t ChildClrRev, char *UniqeOptionalCS, char *OptionalCSAttr)
{
	int status;
	char* ClrItemID=NULL;
	tag_t   window1=NULLTAG;
	tag_t   window=NULLTAG;
	tag_t   rule1	=NULLTAG;
	tag_t   rule	=NULLTAG;
	tag_t   top_line1=NULLTAG;
	tag_t   top_line=NULLTAG;
	int n_parents,jd =0;
	int *levels = 0;
	tag_t *parents = NULLTAG;
	tag_t ParentAssyTag = NULLTAG;
	char*	 ParentPartNum			= NULL;
	char*	 NClrItem_id			= NULL;
	int n_count,bl_occCarOptionalCS =0;
	tag_t   *line = NULL;

	ITK_CALL(BOM_create_window (&window1));		
	ITK_CALL(CFM_find("Latest Working", &rule1));	
	ITK_CALL(BOM_set_window_config_rule(window1, rule1));	
	ITK_CALL(BOM_set_window_pack_all(window1, true));
	ITK_CALL(BOM_set_window_top_line(window1, NULLTAG, ChildClrRev, NULLTAG, &top_line1));
	ITK_CALL(AOM_ask_value_string(top_line1, "bl_item_item_id", &ClrItemID));
	printf("\n ClrItemID-------------> : %s\n ",ClrItemID);fflush(stdout);
	printf("\n UniqeOptionalCS-------------> : %s\n ",UniqeOptionalCS);fflush(stdout);
	printf("\n OptionalCSAttr-------------> : %s\n ",OptionalCSAttr);fflush(stdout);
	
	ITK_CALL(PS_where_used_all(ChildClrRev,1,&n_parents,&levels,&parents));
	printf("\n\t where_used count of objects are : %d\n",n_parents); fflush(stdout);
	if(n_parents>0)
	{
	  	ParentAssyTag=parents[0];
		ITK_CALL(BOM_create_window (&window));		
		ITK_CALL(CFM_find("Latest Working", &rule));	
		ITK_CALL(BOM_set_window_config_rule(window, rule));	
		ITK_CALL(BOM_set_window_pack_all(window, true));
		ITK_CALL(BOM_set_window_top_line(window, NULLTAG, ParentAssyTag, NULLTAG, &top_line));
		ITK_CALL(BOM_line_ask_child_lines(top_line, &n_count, &line));
		printf("\nn_lines-----> %d\n\n",n_count);
		if(n_count > 0)
		{
			for (jd=0;jd<n_count ;jd++ )
			{
				ITK_CALL(AOM_ask_value_string(line[jd], "bl_item_item_id", &NClrItem_id));
				printf("\n NClrItem_id is--> : %s\n\n",NClrItem_id); fflush(stdout);
				if(tc_strcmp(ClrItemID,NClrItem_id)==0)
				{	
						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_CarOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_CarOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}

						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_DwdOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_DwdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}

						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_PunOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}

						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_PunUVOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PunUVOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}

						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_JsrOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JsrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}

						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_LkoOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_LkoOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}

						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_PnrOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_PnrOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}

						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_JdlOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_JdlOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}

						if(tc_strcmp(OptionalCSAttr,"bl_occ_t5_AhdOptionalCS")==0)
						{
							if(tc_strcmp(UniqeOptionalCS,"E99 (In-house production-Back Flush Items)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E99"));
							}else if(tc_strcmp(UniqeOptionalCS,"E50 (In-house production-Phantom Assembly)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E50"));
							}else if(tc_strcmp(UniqeOptionalCS,"E98 (In-house production-SPD items - Ph)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E98"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F (External procurement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F"));
							}else if(tc_strcmp(UniqeOptionalCS,"F30 (External procurement - Subcontracting)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F30"));
							}else if(tc_strcmp(UniqeOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F18"));
							}else if(tc_strcmp(UniqeOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F19"));
							}else if(tc_strcmp(UniqeOptionalCS,"F35 (Interplant stock transfer using 301 movement)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F35"));
							}else if(tc_strcmp(UniqeOptionalCS,"F40 (Interplant stock Transfer using STSA)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"F40"));
							}else if(tc_strcmp(UniqeOptionalCS,"E (GroupID CS)")==0)
							{
								ITK_CALL(BOM_line_look_up_attribute ("bl_occ_t5_AhdOptionalCS",&bl_occCarOptionalCS));
								ITK_CALL(BOM_line_set_attribute_string(line[jd], bl_occCarOptionalCS,"E"));
							}else
							{
								printf("\n UniqeOptionalCS---------------> : %s\n\n",UniqeOptionalCS); fflush(stdout);
							}
						}
				}
			}			
		}	  
	}
}

int OccurSetTopLineClr(tag_t  clrbvr, char* UsesPartNo, int occ_qty)
{
	int          stat                = ITK_ok;
	int			 status;
	int			 ifail				 = ITK_ok;
    tag_t        UseMaster           = NULLTAG;
	tag_t        Nrev                = NULLTAG;
	tag_t        Occ_Child;
	char		*PartNumber			 = NULL;
	char		*occ_item_id			 = NULL;
	tag_t       objNameType=NULLTAG;
	char   type_name[TCTYPE_name_size_c+1];
	tag_t  *occs=NULL;
	tag_t	ChildItem		= NULLTAG;
	tag_t	bvrchild		= NULLTAG;
	int n_occs;
	tag_t *occsal;
	int iy,i =0;
	printf("\n Uses Part Number is ------> %s\n",UsesPartNo);
    ITK_CALL(ITEM_find_item(UsesPartNo,&UseMaster))
	printf("\n UseMaster found------>\n");
	
	ITK_CALL(ITEM_ask_latest_rev(UseMaster, &Nrev));
	printf("\n Childrev found------>\n");
	
	ITK_CALL(AOM_UIF_ask_value(Nrev,"item_id",&PartNumber));
	printf("\n PartNumber : %s ..............",PartNumber);fflush(stdout);
	printf("\n InExpansion PS_ask_occurrence_qty--->%d\n",occ_qty); fflush(stdout);

	if(occ_qty > 0)
	{
		for (i = 0; i < occ_qty; i++)
		{	
			ITK_CALL(PS_create_occurrences(clrbvr, Nrev, NULLTAG,1, &occs ))
			printf("\n PS_create_occurrences successfully in PSE------->\n");
		}
	}

	ITK_CALL(AOM_save(clrbvr) )
	ITK_CALL(AOM_refresh(clrbvr,0));
	return ITK_ok;
}

int OccurSetTopLineToClr(tag_t  clrbvr, char* UsesPartNo, int occ_qty)
{
	int          stat                = ITK_ok;
	int			 status;
	int			 ifail				 = ITK_ok;
    tag_t        UseMaster           = NULLTAG;
	tag_t        Nrev                = NULLTAG;
	tag_t        Occ_Child;
	char		*PartNumber			 = NULL;
	char		*occ_item_id			 = NULL;
	tag_t       objNameType=NULLTAG;
	char   type_name[TCTYPE_name_size_c+1];
	tag_t  *occs=NULL;
	tag_t	ChildItem		= NULLTAG;
	tag_t	bvrchild		= NULLTAG;
	int n_occs;
	tag_t *occsal;
	int iy,i =0;

	printf("\n Uses Part Number is ------> %s\n",UsesPartNo);
    ITK_CALL(ITEM_find_item(UsesPartNo,&UseMaster))
	ITK_CALL(ITEM_ask_latest_rev(UseMaster, &Nrev));	
	printf("\n InExpansion PS_ask_occurrence_qty--->%d\n",occ_qty); fflush(stdout);
	if(occ_qty > 0)
	{
		for (i = 0; i < occ_qty; i++)
		{	
			ITK_CALL(PS_create_occurrences(clrbvr, Nrev, NULLTAG,1, &occs ))
			printf("\n PS_create_occurrences successfully in PSE------->\n");
		}
	}
	ITK_CALL(AOM_save(clrbvr) )
	ITK_CALL(AOM_refresh(clrbvr,0));
	return ITK_ok;
}

int ValColAndNonColBomAttrForRevise(tag_t NonClrRev,tag_t Clrrev,int ColRevRlzStatus, tag_t ColSchmRevTag)
{
	int ifail = ITK_ok;
	int no_bom_linesN, jnc=0;
	int no_bom_linesC, jc=0;
	int FlagchildPresentInBom=0;
	int FlagBomDiff=0;
	int Cmpcount2, k2, CompCodeMatchFlag=0;

	tag_t windowNC=NULLTAG;
	tag_t windowClr=NULLTAG;
	tag_t rule = NULLTAG;
	tag_t ruleClr = NULLTAG;
	tag_t top_lineNC = NULLTAG;
	tag_t top_lineClr = NULLTAG;
	tag_t CmpCodeObj2 = NULLTAG;

	tag_t *child_bom_linesNC = NULLTAG;
	tag_t *child_bom_linesC = NULLTAG;
	tag_t *CmpCodeOfClrScheme2 = NULLTAG;

	char *child_item_idN = NULL;
	char *child_item_idN2 = NULL;
	char *ChildRevSeqN = NULL;
	char *ChildColIndN = NULL;
	char *child_item_idC = NULL;
	char *ChildRevSeqC = NULL;
	char *ChildColIndC = NULL;
	char *CmpCodeCmp2 = NULL;
	char *t5ClSrl = NULL;
	char *Suffix = NULL;
	char *ColourID = NULL;
	char *ChildNCCompCode = NULL;

	child_item_idN2=(char *) MEM_alloc(50);
	
	ifail = BOM_create_window (&windowNC);
	ifail = BOM_create_window (&windowClr);

	ifail = CFM_find( "ERC release and above", &rule );	//Non-colour
	ifail = BOM_set_window_config_rule( windowNC, rule );	//Non-colour
	ifail = BOM_set_window_top_line (windowNC, null_tag, NonClrRev, null_tag, &top_lineNC);
	ifail = BOM_window_show_suppressed ( windowNC );
	ifail = BOM_line_ask_child_lines (top_lineNC, &no_bom_linesN, &child_bom_linesNC);

	//colour
	if (ColRevRlzStatus==1)
	{
		ifail = CFM_find( "Color ERC release and above", &ruleClr );//colour
	}
	else
	{
		ifail = CFM_find( "Color ERC Review and above", &ruleClr );//colour
	}
	ifail = BOM_set_window_config_rule( windowClr, ruleClr ); 

	ifail = BOM_set_window_top_line (windowClr, null_tag, Clrrev, null_tag, &top_lineClr);
	ifail = BOM_window_show_suppressed ( windowClr );
	ifail = BOM_line_ask_child_lines (top_lineClr, &no_bom_linesC, &child_bom_linesC);
	
	printf("\n\t no_bom_linesN: %d   no_bom_linesC: %d",no_bom_linesN,no_bom_linesC);fflush(stdout);
	FlagBomDiff=0;
	for (jnc = 0; jnc < no_bom_linesN; jnc++) //Non-Colour Bom loop
	{
		ifail = AOM_ask_value_string( child_bom_linesNC[jnc] ,"bl_item_item_id",&child_item_idN);
		if (strstr(child_item_idN,"_")!=NULL)
		{
			continue;
		}
		if (strcmp(subString(child_item_idN,0,1),"G")==0)
		{
			continue;
		}

		ifail = AOM_ask_value_string( child_bom_linesNC[jnc] ,"bl_rev_item_revision_id",&ChildRevSeqN);
		//ifail = AOM_ask_value_string( child_bom_linesNC[jnc] ,"bl_T5_ClrPartRevision_t5_ColourInd",&ChildColIndN);
		ifail = AOM_ask_value_string( child_bom_linesNC[jnc] ,"bl_Design Revision_t5_ColourInd",&ChildColIndN);

		printf("\n\t A--jnc: %d  \t ColItem_id: %s  ChildRevSeqC: %s   Colour_Indicator: %s  ",jnc,child_item_idN,ChildRevSeqN,ChildColIndN);fflush(stdout);

		strcpy(child_item_idN2,"");
		tc_strcpy(child_item_idN2,child_item_idN);
		CompCodeMatchFlag==0;
		if (strcmp(ChildColIndN,"Y")==0)
		{
			ifail = AOM_ask_value_string( child_bom_linesNC[jnc] ,"bl_T5_ClrPartRevision_t5_PrtCatCode",&ChildNCCompCode);
			if (SchmWithCmpcdRelMultlvl != NULLTAG)
			{
				ifail = GRM_list_secondary_objects_only(ColSchmRevTag, SchmWithCmpcdRelMultlvl, &Cmpcount2, &CmpCodeOfClrScheme2);
				printf("\n\t\t MultilevelExpantion Comp Code count from Color Scheme is : %d",Cmpcount2); fflush(stdout);
				if(Cmpcount2 > 0)
				{
					for (k2=0;k2<Cmpcount2 ;k2++ )
					{
						if(CmpCodeObj2) CmpCodeObj2=NULLTAG;
						CmpCodeObj2=CmpCodeOfClrScheme2[k2];
						ifail = AOM_ask_value_string(CmpCodeObj2, "t5_PrtCatCode", &CmpCodeCmp2 );
						printf("\n\t\t CmpCodeCmp2 & ChildNCCompCode status--> : [%s]==[%s]",CmpCodeCmp2,ChildNCCompCode); fflush(stdout);
						if(tc_strcmp(ChildNCCompCode,CmpCodeCmp2)==0)
						{
							printf(" -- Match Foun compcode "); fflush(stdout);
							CompCodeMatchFlag=1;
							ifail = AOM_ask_value_string(CmpCodeObj2, "t5_ClSrl", &t5ClSrl );
							Suffix =  strtok(t5ClSrl,";");
							ColourID  =  strtok(NULL,";");
							
							tc_strcat(child_item_idN2,Suffix);

							break;
						}
					}
				}
			}
		}
			FlagchildPresentInBom=0;
			for (jc = 0; jc < no_bom_linesC; jc++)
			{
				//ifail = AOM_ask_value_string( child_bom_linesC[jc] ,"bl_T5_ClrPartRevision_t5_ColourInd",&ChildColIndC);
				ifail = AOM_ask_value_string( child_bom_linesC[jc] ,"bl_Design Revision_t5_ColourInd",&ChildColIndC);

				//if ((strcmp(ChildColIndC,"Y")==0)||(strcmp(ChildColIndC,"C")==0))
				//{
					ifail = AOM_ask_value_string( child_bom_linesC[jc] ,"bl_item_item_id",&child_item_idC);
					if (strstr(child_item_idC,"_")!=NULL)
					{
						continue;
					}
					if (strcmp(subString(child_item_idC,0,1),"G")==0)
					{
						continue;
					}

					ifail = AOM_ask_value_string( child_bom_linesC[jc] ,"bl_rev_item_revision_id",&ChildRevSeqC);

					printf("\n\t A--jc: %d  \t ColItem_id: %s  ChildRevSeqC: %s   Colour_Indicator: %s  ",jc,child_item_idC,ChildRevSeqC,ChildColIndC);fflush(stdout);
					
					//if ((strstr(child_item_idC,child_item_idN)!=NULL) && ((strcmp(ChildColIndC,"C")==0)||(strcmp(ChildColIndC,"N")==0)||(strcmp(ChildColIndC,"Y")==0)))
					//if (strstr(child_item_idC,child_item_idN)!=NULL)
					
					printf("\n\t\t child_item_idC: %s  child_item_idN: %s   child_item_idN2: %s  ",child_item_idC,child_item_idN,child_item_idN2);fflush(stdout);
					if (strstr(child_item_idC,child_item_idN)!=NULL)
					{
						if (strcmp(child_item_idC,child_item_idN2)==0)
						{
							FlagchildPresentInBom=1;
							break;
						}
					}
				//}
			}
			if (FlagchildPresentInBom==0)
			{
				FlagBomDiff++;
			}
			printf(" -- FlagchildPresentInBom: %d  FlagBomDiff: %d",FlagchildPresentInBom,FlagBomDiff); fflush(stdout);
		//}
	}
	printf("\n\t A--FlagBomDiff: %d",FlagBomDiff);fflush(stdout);
	if (FlagBomDiff==0)
	{
		for (jc = 0; jc < no_bom_linesC; jc++) //Colour Bom loop
		{
			ifail = AOM_ask_value_string( child_bom_linesC[jc] ,"bl_T5_ClrPartRevision_t5_ColourInd",&ChildColIndC);
			ifail = AOM_ask_value_string( child_bom_linesC[jc] ,"bl_item_item_id",&child_item_idC);
			if (strstr(child_item_idC,"_")!=NULL)
			{
				continue;
			}
			if (strcmp(subString(child_item_idC,0,1),"G")==0)
			{
				continue;
			}

			ifail = AOM_ask_value_string( child_bom_linesC[jc] ,"bl_rev_item_revision_id",&ChildRevSeqC);
			printf("\n\t B--jc: %d  \t ColItem_id: %s  ChildRevSeqC: %s   Colour_Indicator: %s  ",jc,child_item_idC,ChildRevSeqC,ChildColIndC);fflush(stdout);
			
			FlagchildPresentInBom=0;
			for (jnc = 0; jnc < no_bom_linesN; jnc++)
			{
				ifail = AOM_ask_value_string( child_bom_linesNC[jnc] ,"bl_item_item_id",&child_item_idN);
				if (strstr(child_item_idN,"_")!=NULL)
				{
					continue;
				}
				if (strcmp(subString(child_item_idN,0,1),"G")==0)
				{
					continue;
				}

				ifail = AOM_ask_value_string( child_bom_linesNC[jnc] ,"bl_rev_item_revision_id",&ChildRevSeqN);
				ifail = AOM_ask_value_string( child_bom_linesNC[jnc] ,"bl_T5_ClrPartRevision_t5_ColourInd",&ChildColIndN);

				printf("\n\t B--jnc: %d  \t ColItem_id: %s  ChildRevSeqC: %s   Colour_Indicator: %s  ",jnc,child_item_idN,ChildRevSeqN,ChildColIndN);fflush(stdout);

				if (strstr(child_item_idC,child_item_idN)!=NULL)
				{
					FlagchildPresentInBom=1;
					break;
				}

			}
			if (FlagchildPresentInBom==0)
			{
				FlagBomDiff++;
			}
		}
	}
	printf("\n\t In ValColAndNonColBomAttrForRevise function,  FlagBomDiff: %d",FlagBomDiff);fflush(stdout);

	/*if (FlagBomDiff==0)
	{
		if (SchmWithCmpcdRelMultlvl != NULLTAG)
		{
			ifail = GRM_list_secondary_objects_only(ColSchmRevTag, SchmWithCmpcdRelMultlvl, &Cmpcount2, &CmpCodeOfClrScheme2);
			printf("\n\t\t MultilevelExpantion Comp Code count from Color Scheme is : %d\n\n",Cmpcount2); fflush(stdout);
			if(Cmpcount2 > 0)
			{
				for (k2=0;k2<Cmpcount2 ;k2++ )
				{
					if(CmpCodeObj2) CmpCodeObj2=NULLTAG;
					CmpCodeObj2=CmpCodeOfClrScheme2[k2];
					ifail = AOM_ask_value_string(CmpCodeObj2, "t5_PrtCatCode", &CmpCodeCmp2 );
					printf("\n CmpCodeCmp2 & t5PrtCatCd status--> : %s-%s\n",CmpCodeCmp2,t5PrtCatCd); fflush(stdout);
					//if(tc_strcmp(t5PrtCatCd,CmpCodeCmp2)==0)
					//{
					//	ClrGrpPartPresentFlag=0;
					//	ifail = AOM_ask_value_string(CmpCodeObj, "t5_ClSrl", &t5ClSrl );
					//	Suffix =  strtok(t5ClSrl,";");
					//	ColourID  =  strtok(NULL,";");
					//}
				}
			}
		}
	}*/

	return FlagBomDiff;
}

int MultilevelExpantion(tag_t Clritem, tag_t NonClrRev, tag_t ColSchmRevTag, int NonClrPartlevel,tag_t taskRevTag, tag_t user_tag, tag_t group)
{
	int		result	= 0;
	int		n_count	= 0;
	int ifail;
	tag_t   window=NULLTAG;
	tag_t   rule	=NULLTAG;	
	tag_t   top_line=NULLTAG;
	tag_t   line=NULLTAG;
	tag_t *Nlines = NULL;
	tag_t   Childobject=NULLTAG;
	tag_t	ItemTag	= NULLTAG;
	int status;
	int    bv_clrcount=0;
	int    clr_bvrcnt=0;
	int    bvrNClr=0;
	tag_t *clr_bvs=NULL;
	tag_t *clrbvrs=NULL;
	tag_t *bvrNonClr=NULL;
	tag_t  clrbv    = NULLTAG;
	tag_t  clrbvr    = NULLTAG;
	tag_t	Clrrev		= NULLTAG;
	char	*child_item_id	 = NULL;
	char	*ChildColInd	 = NULL;
	tag_t	NonClrChildRev		= NULLTAG;
	char	*t5PrtCatCd	 = NULL;
	char	*t5itemid	 = NULL;
	char	*t5NonCPrt	 = NULL;
	char	*t5revision_id	 = NULL;
	char	*t5object_desc	 = NULL;
	char	*t5_ColourInd	 = NULL;
	char	*t5_PartStatus	 = NULL;
	char	*t5_DesignGrp	 = NULL;
	char	*t5_ProjectCode	 = NULL;
	char	*t5_DocRemarks	 = NULL;
	char	*t5_DrawingInd	 = NULL;
	char	*t5_PartType	 = NULL;
	char	*t5_PartCode	 = NULL;
	char	*t5_AhdMakeBuyIndicator	 = NULL;
	char	*t5_PunPCBUMakeBuyIndicator	 = NULL;
	char	*t5_PunMakeBuyIndicator	 = NULL;
	char	*t5_DwdMakeBuyIndicator	 = NULL;
	char	*CmpCodeCmp	 = NULL;
	tag_t CmpCodeObj=NULLTAG;
	int Cmpcount,p1,k1 = 0;
	tag_t *CmpCodeOfClrScheme = NULLTAG;
	char*	t5ClSrl=NULL;
	char*	ColourID=NULL;
	char*	itemRevSeq=NULL;
	char *ClrRevSeq = "NR;1";
	char*	t5CoatedS="C";
	int item_revision_id_int =1;
	tag_t ChildClrRev=NULLTAG;
	tag_t ChildClrItem=NULLTAG;
	tag_t tRelationFind;
	tag_t gRelationFind;
	char*	Suffix=NULL;
	tag_t  *occs=NULL;
	tag_t  	   tRelationExist = NULLTAG;
	tag_t 	   Rel_task	= NULLTAG;
	tag_t	ClrPartRev		= NULLTAG;
	char*	Varformula=NULL;
	tag_t RelSolutionItem = NULLTAG;
	tag_t SolutionItemExist = NULLTAG;
	tag_t 	   reltask	= NULLTAG;
	int n_tags_found2 = 0;
	tag_t *tags_found2 = NULL;
	char	*Mitem_Rev_Seq	 = NULL;
	char	*MItem_Rev	 = NULL;
	char	*MItem_Seq	 = NULL;
	char	*itemRevSeq1	 = NULL;
	int rulefound =0;
	tag_t *closurerule = NULL;
	char* Response = NULL ;	
	tag_t close_tag;
	char **rulename = NULL;
	char **rulevalue = NULL;
	tag_t   queryds = NULLTAG;
	int ds_tags_found=0;
	tag_t	*itemrevdsclass	= NULLTAG;
	tag_t ItemTag2	= NULLTAG;
	tag_t ClrPartTag = NULLTAG;
	char **valuesClrPrt = (char **) MEM_alloc(1 * sizeof(char *));
	int n_entriesC = 1;
	char *qry_entries[1] = {"ID"};
	char *qry_entriesds[2] = {"Revision","ID"};
	char **qry_valuesds = (char **) MEM_alloc(50 * sizeof(char *));
	int n_entries = 2;	
	char* ResponseStr = NULL ;
	int TskCntMul=0;
	tag_t *TskSOMul=NULLTAG;
	int n_occs;
	tag_t *occsal;
	int iy =0;
	tag_t	ChildItem		= NULLTAG;
	tag_t	bvrchild		= NULLTAG;
	char	*occ_item_id	= NULL;
	int ChildOccNon = 0;
	int ChildOccClr = 0;
	tag_t        ReviseClrPrt           = NULLTAG;
	tag_t        ReviseClrPrtRev        = NULLTAG;

	int g_attchs,g_attchs11,h =0;
	tag_t *secondary_gobjects = NULLTAG;
	tag_t *secondary_gobjects11 = NULLTAG;
	char* Object_id		= NULL;
	int ClrGrpPartPresentFlag=0;
	int flg2 =0;

	int ColStatus_count=0;
	int ColRlzRevFlag=0;
	int cr=0;
	char *ColPrtRelSt=NULL;
	char *NewColPartRevSeq=NULL;
	char *Revise_id=NULL;
	tag_t *Col_relStatus_list = NULLTAG;
	tag_t NewColorRevTag=NULLTAG;
	int noccs,iz =0;
	tag_t *noccsal;
	tag_t	nChildItem		= NULLTAG;
	tag_t	nbvrchild		= NULLTAG;
	char	*nocc_item_id	= NULL;
	char	*clrColourID	= NULL;
	char	*Clrrev_id	= NULL;
	char	*Clrrevision_id	= NULL;
	
	char	*CarOptionalCS		= NULL;
	char	*DwdOptionalCS		= NULL;
	char	*PunOptionalCS		= NULL;
	char	*PunUVOptionalCS	= NULL;
	char	*JsrOptionalCS		= NULL;
	char	*LkoOptionalCS		= NULL;
	char	*PnrOptionalCS		= NULL;
	char	*JdlOptionalCS		= NULL;
	char	*AhdOptionalCS		= NULL;

	char	*CAROpCs	= NULL;
	tag_t TskCRBTag = NULLTAG;
	tag_t ObjTypTskCRB=NULLTAG;
	int hp=0;
	char   type_name8[TCTYPE_name_size_c+1];
	int QuantityAttrId=0;
	char   *Quantity3=NULL;
	int NClrQty3=0;
	int     bvrcnt=0;
	tag_t   *bvr=NULL;
	tag_t   bvrTag  = NULLTAG;
	int Revoccs,zz =0;
	tag_t *Revccsal;
	char* OptionalCSAttr	=	NULL;	
	int ClrReviseFlag=0;
	
	tag_t ItemTag1 = NULLTAG;
	tag_t UOM_Tag = NULLTAG;
	char  *itemUOM		= NULL;
	char  *UnitOfMeasureS		= NULL;
	int k3=0;
	char* g_ColourID	=	NULL;
	tag_t ColorID_G=NULLTAG;

	printf("\n Inside MultilevelExpantion------------------------------->\n");fflush(stdout);
	ITK_CALL(BOM_line_look_up_attribute("bl_quantity", &QuantityAttrId));
	NonClrPartlevel++;
	if(Clritem !=NULLTAG)
	{
		ifail = ITEM_ask_latest_rev(Clritem,&Clrrev);
		ifail = AOM_ask_value_string(Clrrev, "item_id", &Clrrev_id);
		ifail = AOM_ask_value_string(Clrrev, "item_revision_id", &Clrrevision_id);
		printf("\n Clrrev_id & Clrrevision_id: [%s]===[%s]\n",Clrrev_id,Clrrevision_id);fflush(stdout);
		if (Clrrev!=NULLTAG)
		{
			ifail = WSOM_ask_release_status_list(Clrrev, &ColStatus_count, &Col_relStatus_list);
			if(ColStatus_count > 0)
			{
				ColRlzRevFlag=0;
				for(cr = 0; cr < ColStatus_count; cr++)
				{
					if(AOM_ask_name(Col_relStatus_list[cr], &ColPrtRelSt)==ITK_ok)
					printf("\t ColPrtRelSt: %s ", ColPrtRelSt);fflush(stdout);
					if ((tc_strcmp(ColPrtRelSt,"T5_LcsErcRlzd")==0) || (tc_strcmp(ColPrtRelSt,"ERC Released")==0) )
					{
						ColRlzRevFlag=1;
						break;
					}
				}
			}

			int ReviseStatus = ValColAndNonColBomAttrForRevise(NonClrRev,Clrrev,ColRlzRevFlag,ColSchmRevTag);
			printf("\n\t ColRlzRevFlag:%d  ReviseStatus: %d ",ColRlzRevFlag, ReviseStatus);fflush(stdout);
			ClrReviseFlag=0;
			if ((ColRlzRevFlag==1) && (ReviseStatus>0))
			{
				printf("\t ---Revising....");fflush(stdout);
				ifail = ITEM_copy_rev(Clrrev,NULL,&NewColorRevTag);
				if (NewColorRevTag!=NULLTAG)
				{
					ClrReviseFlag=1;
					ifail = AOM_ask_value_string(NewColorRevTag,"item_id",&Revise_id);
					printf("\n\t Revise Color Part is------> %s ",Revise_id);fflush(stdout);
					//ifail = AOM_ask_value_string(NewColorRevTag,"item_revision_id",&NewColPartRevSeq);
					//printf("\n\t CP=NewColPartRevSeq: %s ",NewColPartRevSeq);fflush(stdout);					
					Clrrev=NULLTAG;
					Clrrev=NewColorRevTag;					
					if(Revise_id!=NULL)
					{
						ifail = ITEM_find_item(Revise_id,&ReviseClrPrt);
						printf("\n ReviseClrPrt found------>\n");
						
						ifail = ITEM_ask_latest_rev(ReviseClrPrt, &ReviseClrPrtRev);
						printf("\n ReviseClrPrtRev found------>\n");
						
						//REVISE PART ATTACHED TO THE DML TASK
						ifail = GRM_find_relation_type("CMHasSolutionItem",&RelSolutionItem);
						if (RelSolutionItem!=NULLTAG)
						{
							printf("\n\tGoing to create revise Color Part relation to DML task------------>\n");fflush(stdout);
							ifail = GRM_create_relation(taskRevTag,Clrrev,RelSolutionItem,NULLTAG,&reltask);
							ifail = GRM_save_relation(reltask); 
							ifail = AOM_refresh(reltask,0);
						}

						//REVISE PART CREATE RELATION WITH NON-COLOR PART				
						ifail = GRM_find_relation_type("TC_Is_Represented_By",&tRelationFind);
						if (tRelationFind!=NULLTAG)
						{	
							printf("\n\tGoing to create revise Color Part relation with non color------------>\n");fflush(stdout); //30/09/2019
							ifail = GRM_create_relation(Clrrev,NonClrRev,tRelationFind,NULLTAG,&Rel_task);
							ifail = GRM_save_relation(Rel_task);
							ifail = AOM_load(Rel_task);
							ifail = AOM_refresh(Rel_task,0);
							printf("\nRelation Created Successfully\n");fflush(stdout);							
						}
						
						printf ( "\n Global SchDataToGov is -------> %s\n",SchDataToGov);fflush(stdout);
						ifail = AOM_lock(ReviseClrPrtRev);
						ifail = AOM_set_value_string(ReviseClrPrtRev,"gov_classification",SchDataToGov);
						ifail = AOM_save(ReviseClrPrtRev);
						ifail = AOM_unlock(ReviseClrPrtRev);
					}

					//NEED TO DELETE STRUCTURE FOR REVISE PART
					if(Clrrev!=NULLTAG)
					{
						ifail = ITEM_rev_list_bom_view_revs(Clrrev,&bvrcnt, &bvr);
						printf("\n bvrcnt------->%d\n",bvrcnt);fflush(stdout);
						if (bvrcnt)
						{
							bvrTag = bvr[0];
							MEM_free(bvr);
						}
						if(bvrTag!=NULLTAG)
						{
							ifail = PS_list_occurrences_of_bvr(bvrTag, &Revoccs, &Revccsal);
							printf("\n Revoccs------->%d\n",Revoccs);fflush(stdout);
							ifail = AOM_lock(bvrTag);
							for (zz = 0; zz<Revoccs; zz++)
							{
								printf("\n Going to Call PS_delete_occurrence for Revise Part------->\n\n"); fflush(stdout);
								ifail = PS_delete_occurrence(bvrTag, Revccsal[zz]);
							}
							ifail = AOM_save(bvrTag);
							ifail = AOM_unlock(bvrTag);
						}
					}
				}				
			}else
			{
				printf("\t ---No Need to Revise....");fflush(stdout);
			}
		}

		ifail = AOM_ask_value_string(Clrrev,"item_revision_id",&NewColPartRevSeq);
		printf("\n\t CP=NewColPartRevSeq: %s ",NewColPartRevSeq);fflush(stdout);
		//if (tc_strstr(NewColPartRevSeq,"NR")==0)
		//{
		//	ClrReviseFlag=1;
		//}


		ifail =ITEM_list_bom_views(Clritem,&bv_clrcount, &clr_bvs);
		printf("\n bv_clrcnt :%d..\n",bv_clrcount);fflush(stdout);
		if (bv_clrcount)
		{
			clrbv = clr_bvs[0];
			MEM_free(clr_bvs);
		}else{
			ifail = PS_create_bom_view( NULLTAG, "", "", Clritem,&clrbv );
			ifail = AOM_save( clrbv );
			ifail = AOM_save( Clritem );
			ifail = AOM_unlock( Clritem );
			printf("\n inside PS_create_bom_view..\n");fflush(stdout);
		}

		if (Clrrev!=NULLTAG)
		{
			ifail = ITEM_rev_list_bom_view_revs(Clrrev,&clr_bvrcnt, &clrbvrs);
			printf("\n clr_bvrcnt :%d..\n",clr_bvrcnt);fflush(stdout);
			if (clr_bvrcnt)
			{
				clrbvr = clrbvrs[0];
				MEM_free(clrbvrs);
			}else{
				ifail = PS_create_bvr( clrbv, "", "", false, Clrrev,&clrbvr);
				if(clrbvr !=NULLTAG)
				{								
					ifail = AOM_save( clrbvr);
					ifail = AOM_save( Clrrev );
					ifail = AOM_unlock( Clrrev );
					printf("\n inside  PS_create_bvr..\n");fflush(stdout);
				}
			}
		}
	}
	
	ifail = AOM_ask_value_string(NonClrRev, "item_id", &t5NonCPrt);
	printf("\n t5NonCPrt for Part is --> : %s\n",t5NonCPrt); fflush(stdout);
	if(NonClrRev !=NULLTAG)
	{
		ifail = BOM_create_window(&window);
		ifail = CFM_find("ERC release and above", &rule);
		ifail = BOM_set_window_config_rule(window, rule);	
		ifail = BOM_set_window_pack_all(window, TRUE);
		ifail = BOM_set_window_top_line(window , NULLTAG, NonClrRev, NULLTAG, &top_line);
		ifail = BOM_window_show_suppressed (window);
		if(top_line!=NULLTAG)
		{
			ifail = BOM_line_ask_child_lines(top_line, &n_count, &Nlines);
			printf("\n n_lines-----> %d\n",n_count);

			if (n_count >0)
			{
				for (p1=0;p1<n_count ;p1++ )
				{
					ChildOccClr=0;
					if(Childobject) Childobject=NULLTAG;
					Childobject=Nlines[p1];
					ifail = AOM_ask_value_string(Childobject, "bl_item_item_id", &child_item_id );
					ifail = AOM_ask_value_string(Childobject, "bl_T5_ClrPartRevision_t5_ColourInd", &ChildColInd);
					printf("\n child_item_id & its t5_ColourInd is==> %s==%s\n",child_item_id,ChildColInd);
					ifail = AOM_ask_value_string(Childobject, "bl_rev_item_revision_id", &Mitem_Rev_Seq);
					MItem_Rev =  tc_strtok(Mitem_Rev_Seq,";");
					MItem_Seq = tc_strtok(NULL,";");
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_CarOptionalCS", &CarOptionalCS);
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_DwdOptionalCS", &DwdOptionalCS);
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_PunOptionalCS", &PunOptionalCS);
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_PunUVOptionalCS", &PunUVOptionalCS);
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_JsrOptionalCS", &JsrOptionalCS);
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_LkoOptionalCS", &LkoOptionalCS);
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_PnrOptionalCS", &PnrOptionalCS);
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_JdlOptionalCS", &JdlOptionalCS);
					ifail = AOM_UIF_ask_value(Childobject, "bl_occ_t5_AhdOptionalCS", &AhdOptionalCS);
					ifail = BOM_line_ask_attribute_string(Childobject,QuantityAttrId, &Quantity3);
					NClrQty3=atoi(Quantity3);
					printf("\n NClrQty3 is--> : %d\n\n",NClrQty3); fflush(stdout);
										
					if (child_item_id && MItem_Rev && MItem_Seq)
					{
						ifail = QRY_find("DesignRevision Sequence", &queryds);
						itemRevSeq1 = NULL;
						itemRevSeq1=(char *) MEM_alloc(32);
						tc_strcpy(itemRevSeq1,MItem_Rev);
						tc_strcat(itemRevSeq1,"?");
						tc_strcat(itemRevSeq1,MItem_Seq);
						printf("\n itemRevSeq1 =%s\n",itemRevSeq1);fflush(stdout);
						qry_valuesds[0] = itemRevSeq1;
						qry_valuesds[1] = child_item_id;
						ifail = QRY_execute(queryds, n_entries, qry_entriesds, qry_valuesds, &ds_tags_found, &itemrevdsclass);
						printf("\n ds_tags_found :: %d\n", ds_tags_found);fflush(stdout);
						if(ds_tags_found > 0)
						{							
							NonClrChildRev=NULLTAG;
							NonClrChildRev=itemrevdsclass[0];
						}
					}

					if(tc_strcmp(ChildColInd,"Y")!=0)
					{
						printf("\n Color Indicator for Non-Color is------------------------>N\n"); fflush(stdout);
						ifail = NonClrOccursetToClrBvr(clrbvr,child_item_id,NClrQty3);
						ifail = BOM_set_window_pack_all(window, true);
					}else
					{
						printf("\n Color Indicator for Non-Color is------------------------>Y\n"); fflush(stdout);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_PrtCatCode", &t5PrtCatCd);
						ifail = AOM_ask_value_string(NonClrChildRev, "item_id", &t5itemid);
						ifail = AOM_ask_value_string(NonClrChildRev, "current_revision_id", &t5revision_id);
						ifail = AOM_ask_value_string(NonClrChildRev, "object_desc", &t5object_desc);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_ColourInd", &t5_ColourInd);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_PartStatus", &t5_PartStatus);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_DesignGrp", &t5_DesignGrp);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_ProjectCode", &t5_ProjectCode);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_DocRemarks", &t5_DocRemarks);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_DrawingInd", &t5_DrawingInd);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_PartType", &t5_PartType);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_PartCode", &t5_PartCode);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_AhdMakeBuyIndicator", &t5_AhdMakeBuyIndicator);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_PunPCBUMakeBuyIndicator", &t5_PunPCBUMakeBuyIndicator);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_PunMakeBuyIndicator", &t5_PunMakeBuyIndicator);
						ifail = AOM_ask_value_string(NonClrChildRev, "t5_DwdMakeBuyIndicator", &t5_DwdMakeBuyIndicator);

						itemUOM = NULL;
						itemUOM=(char *) MEM_alloc(50);
						ifail = ITEM_find_item(t5itemid,&ItemTag1);
						ifail = ITEM_ask_unit_of_measure(ItemTag1, &UOM_Tag);
						tc_strcpy(itemUOM,"");

						if(UOM_Tag==NULLTAG)
						{
							tc_strcat(itemUOM,"4");								
						}else
						{
							ifail = UOM_ask_symbol(UOM_Tag, &UnitOfMeasureS);
							if(tc_strcmp(UnitOfMeasureS,"each")==0 || tc_strcmp(UnitOfMeasureS,"")==0)
							{
								tc_strcat(itemUOM,"4");
							}else
							{
								itemUOM=subString(UnitOfMeasureS, 0, 1);
							}
						}
						printf("\n final UOM Values from MULTILEVEL FUNCTION--> : %s\n",itemUOM); fflush(stdout);
						
						if (SchmWithCmpcdRelMultlvl != NULLTAG)
						{
							ifail = GRM_list_secondary_objects_only(ColSchmRevTag, SchmWithCmpcdRelMultlvl, &Cmpcount, &CmpCodeOfClrScheme);
							printf("\n\t\t MultilevelExpantion Comp Code count from Color Scheme is : %d\n\n",Cmpcount); fflush(stdout);
							int flagNonClr=0;
							if(Cmpcount > 0)
							{
								for (k1=0;k1<Cmpcount ;k1++ )
								{
									if(CmpCodeObj) CmpCodeObj=NULLTAG;
									CmpCodeObj=CmpCodeOfClrScheme[k1];
									ifail = AOM_ask_value_string(CmpCodeObj, "t5_PrtCatCode", &CmpCodeCmp );
									printf("\n CmpCodeCmp & t5PrtCatCd status--> : %s-%s\n",CmpCodeCmp,t5PrtCatCd); fflush(stdout);
									if(tc_strcmp(t5PrtCatCd,CmpCodeCmp)==0)
									{
										flagNonClr=1;
										ClrGrpPartPresentFlag=0;
										ifail = AOM_ask_value_string(CmpCodeObj, "t5_ClSrl", &t5ClSrl );
										Suffix =  strtok(t5ClSrl,";");
										ColourID  =  strtok(NULL,";");
										printf("\n t5itemid --> : %s\n",t5itemid); fflush(stdout);
										printf("\n t5ClSrl --> : %s\n",t5ClSrl); fflush(stdout);
										printf("\n Suffix --> : %s\n",Suffix); fflush(stdout);
										printf("\n ColourID --> : %s\n",ColourID); fflush(stdout);
										printf("\n t5_PartType --> : %s\n",t5_PartType); fflush(stdout);
										if(tc_strcmp(t5_PartType,"G")==0)
										{
											ifail = GRM_find_relation_type("TC_Is_Represented_By",&gRelationFind);
											if(gRelationFind!=NULLTAG)
											{
												ifail = GRM_list_primary_objects_only(NonClrChildRev,gRelationFind,&g_attchs,&secondary_gobjects);
												printf("\n\n\t\t No of DI : %d",g_attchs);fflush(stdout);

												if(g_attchs>0)
												{		
													//Need To Check Group ID logic as per priviosly created color GROUP ID
													for (k3=0;k3<g_attchs ;k3++ )
													{
														if(ColorID_G) ColorID_G=NULLTAG;
														ColorID_G=secondary_gobjects[k3];

														ifail = AOM_ask_value_string(ColorID_G,"t5_ColourID",&g_ColourID);
														printf("\n\n\t\t g_ColourID is :%s",g_ColourID);fflush(stdout);
														printf("\n ColourID from Scheme & from Color GroupID --> : %s = \n",ColourID,g_ColourID); fflush(stdout);
														
														if(tc_strcmp(ColourID,g_ColourID)==0)
														{
															ifail = AOM_ask_value_string(ColorID_G,"item_id",&Object_id);
															printf("\n\n\t\t Object_id is :%s",Object_id);fflush(stdout);
															ClrGrpPartPresentFlag=1;														
														}else {
															printf("\n\n\t\t g_ColourID is :%s",g_ColourID);fflush(stdout);
														}
													}													
													
													printf("\n Going to Check Color Part in DB============================> %s\n\n",Object_id);fflush(stdout);
													n_tags_found2=0;
													if(Object_id!=NULL)
													{
														if(QRY_find("ClrPart", &ClrPartTag));
														if (ClrPartTag) { printf("\nFound Query ClrPartTag MultilevelExpantion "); fflush(stdout); 
														}else { printf("\nNot Found Query ClrPartTag"); fflush(stdout); }
														valuesClrPrt[0] = Object_id;
														if(QRY_execute(ClrPartTag, n_entriesC, qry_entries, valuesClrPrt, &n_tags_found2, &tags_found2));
														printf("\n\n\n ClrPart count is MultilevelExpantion -------->  : %d",n_tags_found2);fflush(stdout);
													}

													if(n_tags_found2 > 0)
													{		
															ItemTag2 = tags_found2[0];
															itemRevSeq=Object_id;
															printf("\n Group ID Color Part status in DB============================> %s\n\n",itemRevSeq);fflush(stdout);

															//Add already exiting Group ID to Color BOM
															ifail = OccurSetTopLineClr(clrbvr,itemRevSeq ,NClrQty3);
															ifail = BOM_set_window_pack_all(window, true);
													}													
												}

												/////////////////////////////NewUniqGroupIDNum/////////////////////////////////////////////
												printf("\n  ClrGrpPartPresentFlag-------------------------------------->%d\n",ClrGrpPartPresentFlag); fflush(stdout);
												if(ClrGrpPartPresentFlag==0)
												{
													printf("\n  before NewUniqGroupIDNum\n"); fflush(stdout);
													ResponseStr = NewUniqGroupIDNum();
													printf("\n  after NewUniqGroupIDNum :: %s\n",ResponseStr); fflush(stdout);
													itemRevSeq = NULL;
													itemRevSeq=(char *) MEM_alloc(32);														
													strcpy(itemRevSeq,"");
													strcat(itemRevSeq,ResponseStr);
													printf("\n Colour Part for Group ID---->%s = %s\n",itemRevSeq,ClrRevSeq);
												}
											}
										}else
										{
											itemRevSeq = NULL;
											itemRevSeq=(char *) MEM_alloc(32);														
											strcpy(itemRevSeq,"");
											strcat(itemRevSeq,t5itemid);
											strcat(itemRevSeq,Suffix);														
											printf("\n Colour Part for Creation MultilevelExpantion ---->%s = %s\n",itemRevSeq,ClrRevSeq);
											if(clrbvr!=NULLTAG)
											{
												ifail = PS_list_occurrences_of_bvr(clrbvr, &noccs, &noccsal);
												printf("\n Hanuman-----Clrrev_id: %s---no:of:occurances are --->%d\n",Clrrev_id,noccs); fflush(stdout);
												if(noccs > 0)
												{
													 ifail = AOM_lock(clrbvr);
													 for (iz = 0; iz<noccs; iz++)
													 {
														 ifail = PS_ask_occurrence_child(clrbvr,noccsal[iz], &nChildItem, &nbvrchild);
														 ifail = AOM_ask_value_string(nChildItem, "item_id", &nocc_item_id);
														 printf("\n nocc_item_id & child_item_id  :%s=%s New Color is : %s\n\n",nocc_item_id,child_item_id,itemRevSeq);fflush(stdout);

														 ifail = PS_ask_occurrence_qty(clrbvr,noccsal[iz], &qty);
														 printf("\n InExpansion PS_ask_occurrence_qty --->%g\n",qty); fflush(stdout);

														 if((tc_strstr(nocc_item_id,child_item_id)!=NULL) && (tc_strstr(itemRevSeq,child_item_id)!=NULL)) //Non-Color Compared with Exiting Color ZZ with Same/ZS New Color Part
														 {
															 if(tc_strcmp(nocc_item_id,itemRevSeq)!=0)
															 {
																 ifail = PS_delete_occurrence(clrbvr, noccsal[iz]);
																 printf("\n non-color where Color Indicator Y has been removed--->%s\n",child_item_id); fflush(stdout);
															 }else
															 {
																 printf("\n BOMLine Color Childs Match found  ==> %s=%s\n\n",nocc_item_id,itemRevSeq);fflush(stdout);
															 }
														 }
													 }
													 ifail = AOM_save( clrbvr );
													 ifail = AOM_unlock( clrbvr );
												}
											}
										}

										printf("\n Going to Check Color Part in DB============================> %s\n\n",itemRevSeq);fflush(stdout);
										n_tags_found2=0;
										if(itemRevSeq!=NULL)
										{
											if(QRY_find("ClrPart", &ClrPartTag));
											if (ClrPartTag) { printf("\nFound Query ClrPartTag MultilevelExpantion "); fflush(stdout); 
											}else { printf("\nNot Found Query ClrPartTag"); fflush(stdout); }
											valuesClrPrt[0] = itemRevSeq;
											if(QRY_execute(ClrPartTag, n_entriesC, qry_entries, valuesClrPrt, &n_tags_found2, &tags_found2));
											printf("\n\n\n ClrPart count is MultilevelExpantion -------->  : %d",n_tags_found2);fflush(stdout);
										}

										if(n_tags_found2 > 0)
										{
											ItemTag2 = tags_found2[0];
											ifail = PS_list_occurrences_of_bvr(clrbvr, &n_occs, &occsal);
											printf("\n InMain  no:of:occurances are --->%d\n",n_occs); fflush(stdout);
											if(n_occs > 0)
											{
												 for (iy = 0; iy<n_occs; iy++)
												 {
													 ifail = PS_ask_occurrence_child(clrbvr,occsal[iy], &ChildItem, &bvrchild);
													 ifail = AOM_ask_value_string(ChildItem, "item_id", &occ_item_id);
													 printf("\n occ_item_id & PartNumber is  :%s=%s\n\n",occ_item_id,itemRevSeq);fflush(stdout);

													 ifail = PS_ask_occurrence_qty(clrbvr,occsal[iy], &qty);
													 printf("\n InExpansion PS_ask_occurrence_qty --->%g\n",qty); fflush(stdout);

													 if(tc_strcmp(occ_item_id,itemRevSeq)==0)
													 {
														ChildOccClr=1;
														break;
													 }
												 }												
											}
											
											if(ChildOccClr==0)
											{
												ifail = OccurSetTopLineClr(clrbvr,itemRevSeq ,NClrQty3);
												ifail = BOM_set_window_pack_all(window, true);
											}
										
											if(ItemTag2!=NULLTAG)
											{
												flg2=MultilevelExpantion(ItemTag2,NonClrChildRev,ColSchmRevTag,NonClrPartlevel,taskRevTag,user_tag,group);
												if (flg2==-1)
												{
													return flg2;
												}
											}
									  }else
									  {
										/////////////////////////////WebSerFunForClrPart/////////////////////////////////////////////
										if(ClrGrpPartPresentFlag==0)
										{	
											printf("\n Color Part is Checking in TCE---------->%s\n",itemRevSeq); fflush(stdout);
											Response = WebSerFunForClrPart(itemRevSeq);
											printf("Color Part checking in TCE :: [%s]",Response);fflush(stdout);

											if(tc_strcmp(Response,"NA")==0)
											{											
												ifail = ITEM_create_item(itemRevSeq,itemRevSeq,"T5_ClrPart",default_empty_to_A(ClrRevSeq),&ChildClrItem,&ChildClrRev);
												setAttributesOnDesign(&ChildClrItem,itemRevSeq,t5object_desc,itemUOM);					
												setAttributesOnDesignRev(&ChildClrRev,itemRevSeq,item_revision_id_int,t5object_desc,t5_ColourInd,t5PrtCatCd,ColourID,t5CoatedS,t5_PartStatus,t5_DesignGrp,t5_ProjectCode,t5_DocRemarks,t5_DrawingInd,t5_PartType,t5_PartCode,t5_AhdMakeBuyIndicator,t5_PunPCBUMakeBuyIndicator,t5_PunMakeBuyIndicator,t5_DwdMakeBuyIndicator,child_item_id);
											
												//Going to update UserOwner Name
												ifail = AOM_set_ownership(ChildClrRev,user_tag ,group);
												printf("\n  AOM_set_ownership...!!\n");fflush(stdout);

												ifail = CreateReleasestatus(ChildClrRev);
												ifail = AssignProject(ChildClrRev,t5_ProjectCode);												
												if (taskRevTag!=NULLTAG)
												{
													ifail = GRM_find_relation_type("CMHasSolutionItem",&RelSolutionItem);
													if(RelSolutionItem!=NULLTAG)
													{	
														ifail = GRM_create_relation(taskRevTag,ChildClrRev,RelSolutionItem,NULLTAG,&reltask);
														ifail = GRM_save_relation(reltask); 
														ifail = AOM_refresh(reltask,0);
														//printf("\nRelation Created Successfully MultilevelExpantion\n");fflush(stdout);	
													}
												}

												ifail = GRM_find_relation_type("TC_Is_Represented_By",&tRelationFind);
												if (tRelationFind!=NULLTAG)
												{													
													ifail = GRM_create_relation(ChildClrRev,NonClrChildRev,tRelationFind,NULLTAG,&Rel_task);
													//printf("\n Color to non-color Relation Created Successfully\n");fflush(stdout);
													ifail = GRM_save_relation(Rel_task); 
													ifail = AOM_load(Rel_task);
													ifail = OccurSetTopLineToClr(clrbvr,itemRevSeq, NClrQty3);													
												}
												
												//CS UPDATED IN COLOR PARTS AS PER NON-COLOR IN BOM GENERATION PROCESS													
												if(CarOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_CarOptionalCS");
													printf("\n CarOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, CarOptionalCS,OptionalCSAttr);
													printf("\n CarOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}

												if(DwdOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_DwdOptionalCS");
													printf("\n DwdOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, DwdOptionalCS,OptionalCSAttr);
													printf("\n DwdOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}

												if(PunOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_PunOptionalCS");
													printf("\n PunOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, PunOptionalCS,OptionalCSAttr);
													printf("\n PunOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}

												if(PunUVOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_PunUVOptionalCS");
													printf("\n PunUVOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, PunUVOptionalCS,OptionalCSAttr);
													printf("\n PunUVOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}

												if(JsrOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_JsrOptionalCS");
													printf("\n JsrOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, JsrOptionalCS,OptionalCSAttr);
													printf("\n JsrOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}

												if(LkoOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_LkoOptionalCS");
													printf("\n LkoOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, LkoOptionalCS,OptionalCSAttr);
													printf("\n LkoOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}

												if(PnrOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_PnrOptionalCS");
													printf("\n PnrOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, PnrOptionalCS,OptionalCSAttr);
													printf("\n PnrOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}

												if(JdlOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_JdlOptionalCS");
													printf("\n JdlOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, JdlOptionalCS,OptionalCSAttr);
													printf("\n JdlOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}

												if(AhdOptionalCS!=NULL)
												{
													OptionalCSAttr = NULL;
													OptionalCSAttr=(char *) MEM_alloc(50);
													tc_strcpy(OptionalCSAttr,"bl_occ_t5_AhdOptionalCS");
													printf("\n AhdOptionalCS--------->%s\n",OptionalCSAttr);
													ifail = CarOptionalCSSet(ChildClrRev, AhdOptionalCS,OptionalCSAttr);
													printf("\n AhdOptionalCS on T5_ClrPartsRevision--------->%d\n",ifail);
												}
										  }else
										  {
											  fprintf(output,"%s\n",itemRevSeq);
											  return -1;
										  }										 
									  }
									 
									  if(ChildClrItem!=NULLTAG)
									  {
										flg2=MultilevelExpantion(ChildClrItem,NonClrChildRev,ColSchmRevTag,NonClrPartlevel,taskRevTag,user_tag,group);
										if(flg2==-1)
										{
											return flg2;
										}
									  }
									  }
									}
								}
							}

							printf("\n\n\n flagNonClr-------->  : %d\n",flagNonClr);fflush(stdout);
							if(flagNonClr==0)
							{
								ifail = NonClrOccursetToClrBvr(clrbvr,t5itemid,NClrQty3);
								//printf("\n If Comp code for non-color is not matched in Scheme then add as it is to BOMLine..........>\n");fflush(stdout);
							}else {
								printf("\n\n\n flagNonClr else loop-------->");fflush(stdout);
							}
						}
					}
				}
			}
		}
	}
	return 0;
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
    char *Platform = NULL;
    char *ColPlatform = NULL;
	char *platformRev = NULL;
    char *platformSeq = NULL;
    char *SVRrule = NULL;
    char *ColSVRCotxt = NULL;
    char *expansionLvl = NULL;
    char *context = NULL;
	char* username = NULL;
	char* inputfile=NULL;
	char* child_item_id=NULL;
	char* ChildId=NULL;
	char* ChildColInd=NULL;
	char* Col_child_item_id=NULL;
	tag_t *ParentBom=NULLTAG;
	tag_t item_rev_tag = NULLTAG;
	tag_t ColItem_rev_tag = NULLTAG;
	tag_t vol=NULLTAG;
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
	int n_tags_found,n_tags_foundC,n_rev,k,k1,countV,countV1,countDep1= 0;
	int n_lines,n_lines1,n_lines2,n_lines3 = 0;
	int n_entriesV = 1;
	int resultCountVNCol = 0;
	int resultCountVCol = 0;
	int p1,p2,p3 = 0;
	char *Context_Str=NULL;
	char type_name1[TCTYPE_name_size_c+1];
	char type_name2[TCTYPE_name_size_c+1];
	char *Clrscheme	= NULL;
	char *DMLNo	= NULL;
	tag_t ERCDMLTag = NULLTAG;
	int n_entriesDML = 1;
	char *qry_entries5[1] = {"ID"};
	int resultColSchm,resultDMLNo = 0;
	tag_t *DMLNotags=NULLTAG;
	tag_t ERCDMLObjtag = NULLTAG;
	tag_t VehicleRlzRev = NULLTAG;
	tag_t ERCDMLTypeTag=NULLTAG;
	char  ERCDML_name[TCTYPE_name_size_c+1];
	tag_t relation_type;
	int   n_attchs,l = 0;
	char* object_type		= NULL;
	char* Object_id		= NULL;
	char* Itemid		= NULL;	
	tag_t SchmqueryTag = NULLTAG;
	char  *CpyClrscheme	= NULL;
	int n_entriesSchm = 1;
	char *qry_entries4[1] = {"ID"};
	tag_t *ColSchme_tags=NULLTAG;
	tag_t ColSchmTypeTag=NULLTAG;
	char* t5objectStr = NULL;
	int status_count=0;
	tag_t* relStatus_list;
	tag_t* RlzStus_list;
	int RlzRevFlag,rs=0;
	char *ProObjRelSt = NULL;		
	char *RelStatusmatch = NULL;		
	char *ClSrlName = NULL;
	char *ClSuffix=NULL;
	tag_t ColorSchemetag = NULLTAG;	
	char *SuffixCpy= NULL;	
	SuffixCpy=MEM_alloc(50);
	char *Vehicle= NULL;	
	Vehicle=MEM_alloc(100);
	char *CCVItem = NULL;
	char *NonColor = NULL;
	tag_t VCItemTag = NULLTAG;
	tag_t *VCtags_found=NULLTAG;
	int n_VCtag =0;
	char **valuesCCV = (char **) MEM_alloc(1 * sizeof(char *));
	int n_entriesCCV = 1;
	char *CCV_entries[1] = {"ID"};
	tag_t CCVItemExist = NULLTAG;	
	tag_t CCVMaster = NULLTAG;	
	tag_t CCVrev = NULLTAG;	
	tag_t CCVSolutionItem;
	tag_t CCVreltask	= NULLTAG;
	tag_t X4Item = NULLTAG;
	tag_t X4ItemRevision = NULLTAG;
	char *word=NULL;
	char *name, *sequence_no;
	int iChildItemTag;
	tag_t t_ChildItemRev=NULLTAG;
	tag_t objTypeTag=NULLTAG;
	char  typeName[TCTYPE_name_size_c+1];
	int no_bom_lines =0;
	tag_t *child_bom_lines;
	int    bv_count, bvr_count;
	tag_t *bvs, *bvrs;
	tag_t  bv,bvr = NULLTAG;
	tag_t  archObjTag = NULLTAG;
	
	char *Arcbl_Item_id = NULL;	
	char *Item_ID_str= NULL;
	int Revformula;	

	char *bl_Item_id = NULL;
	char *item_Rev_Seq = NULL;
	char *Item_Rev = NULL;
	char *Item_Seq = NULL;
	int NonClrPartlevel = 0;
	char *t5ColourInd = NULL;
	char *t5_PrtCatCd = NULL;

	tag_t queryds = NULLTAG;
	char *qry_entriesds[2] = {"Revision","ID"};
	char **qry_valuesds = (char **) MEM_alloc(50 * sizeof(char *));
	int n_entries = 2;	
	char *itemRevSeq2=NULL;
	int ds_tags_found=0;
	tag_t	*itemrevdsclass	= NULLTAG;
	tag_t	latestrev		= NULLTAG;
	int returnV =0;

	char *t5PrtCatCd = NULL;
	char *t5itemid = NULL;
	char *t5revision_id = NULL;
	char *t5object_desc = NULL;
	char *t5_ColourInd = NULL;
	char *t5_PartStatus = NULL;
	char *t5_DesignGrp = NULL;
	char *t5_ProjectCode = NULL;
	char *t5_DocRemarks = NULL;
	char *t5_DrawingInd = NULL;
	char *t5_PartType = NULL;
	char *t5_PartCode = NULL;
	char *t5_AhdMakeBuyIndicator = NULL;
	char *t5_PunPCBUMakeBuyIndicator = NULL;
	char *t5_PunMakeBuyIndicator = NULL;
	char *t5_DwdMakeBuyIndicator = NULL;
	tag_t SchmWithCmpcdRel;
	tag_t SchmWithCmpcdRel_VC;
	logical	checkout = 0;
	int Cmpcount =0;
	int CmpVcCnt =0;
	tag_t *CmpCodeOfClrScheme = NULLTAG;
	tag_t *CmpCodeOfClrSchemeVC = NULLTAG;
	tag_t CmpCodeObj=NULLTAG;
	tag_t VCCmpCodeObj=NULLTAG;
	int g1,g2 =0;
	char *CmpCodeCmp = NULL;
	char*	t5ClSrl=NULL;
	char*	t5CoatedS="C";
	char*	Suffix=NULL;
	char*	ColourID=NULL;
	char *itemRevSeq = NULL;
	tag_t	ItemTag1	= NULLTAG;
	tag_t	item	= NULLTAG;
	tag_t	rev	= NULLTAG;
	tag_t ClrPartTag = NULLTAG;
	char **valuesClrPrt = (char **) MEM_alloc(1 * sizeof(char *));
	int n_entriesC = 1;
	char *qry_entries[1] = {"ID"};
	int n_tags_found1 = 0;
	tag_t *tags_found1 = NULL;	
	
	char *SVRName = NULL;
	char *bl_ArchModule = NULL;
	char *item_rev = NULL;
	int item_revision_id_int =1;
	tag_t	occsal_tag	= NULLTAG;
	tag_t RelSolutionItem = NULLTAG;
	tag_t tRelationFind = NULLTAG;
	tag_t SolutionItemExist = NULLTAG;
	tag_t tRelationExist = NULLTAG;
	tag_t 	   reltask	= NULLTAG;
	tag_t 	   Rel_task	= NULLTAG;
	tag_t 	   CCVObject	= NULLTAG;
	tag_t  clrbv    = NULLTAG;
	tag_t  clrbvr    = NULLTAG;
	tag_t	Clrrev1		= NULLTAG;
	tag_t	ColorRev		= NULLTAG;
	tag_t  *occs=NULL;
	tag_t  *occs1=NULL;
	int n_parents,jd =0;
	int *levels = 0;
	tag_t *parents = NULL;
	tag_t archTypeTag=NULLTAG;
	tag_t VCtag=NULLTAG;
	tag_t VCItemRev=NULLTAG;
	char  archTypename[TCTYPE_name_size_c+1];	
	
	char *user_name_string = NULL;
	char *UserId = NULL;
	char *SchTok = NULL;
	
	int rulefound= 0;
	tag_t *closurerule = NULL;
	tag_t close_tag;
	char **rulename = NULL;
	char **rulevalue = NULL;
	tag_t			VeHMstr      = NULLTAG;
	tag_t			latest_rev_tag				= NULLTAG;
	tag_t CCVRelationFind;	
	tag_t NonClrBOMLine=NULLTAG;
	char *ColorVC= NULL;	
	ColorVC=MEM_alloc(100);
	tag_t NVCtag=NULLTAG;
	tag_t NVCItemRev=NULLTAG;
	int Child_VCRev_count,hd,VCRevCnt,hs =0;
	tag_t*	Child_VCrev_list = NULLTAG;
	tag_t* RlzStus_VC_list;
	char *VCRelStatusM = NULL;
	
	tag_t NVCRlzRev = NULLTAG;
	int NVCRlzRevFlag=0;
	int    bv_clrcount=0;
	int    clr_bvrcnt=0;
	tag_t *clr_bvs=NULL;
	tag_t *clrbvrs=NULL;
	tag_t  VCclrbv    = NULLTAG;
	tag_t  VCclrbvr    = NULLTAG;
	tag_t IMANReferences_type;
	//tag_t Rel_References	= NULLTAG;

	int n_occs;
	tag_t *occsal;
	int iy =0;
	tag_t	ChildItem		= NULLTAG;
	tag_t	bvrchild		= NULLTAG;
	char	*occ_item_id		= NULL;	
	tag_t ClrVariqueryTag = NULLTAG;
	tag_t CMRefExist = NULLTAG;
	int n_entriesV1 = 1;
	int ClrCnt =0;
	tag_t *ColVRule_tags=NULLTAG;
	tag_t variantule=NULLTAG;
	tag_t References_type;
	tag_t Rel_References	= NULLTAG;
	tag_t ReferencesRelExist = NULLTAG;
	char *qry_entriesClrSvr[1] = {"Name"};
	char	*UsrName		= NULL;
	tag_t *ClrSVRName = NULLTAG;
	int VariantCnt =0;
	int TskCntClr=0;
	tag_t *TskSOClr=NULLTAG;
	int flg1 =0;
	char* Response = NULL ;		
	char **valuesNonColSvr = (char **) MEM_alloc(1 * sizeof(char *));	
	char  ColSchm_name[TCTYPE_name_size_c+1];
	char **valuesColSchm = (char **) MEM_alloc(1 * sizeof(char *));
	char **valuesERCDML = (char **) MEM_alloc(1 * sizeof(char *));
	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));
	char **ValNcolPlatform = (char **) MEM_alloc(1 * sizeof(char *));
	char **valuesColSvr = (char **) MEM_alloc(1 * sizeof(char *));
	char *qry_entries3[1] = {"Name"}, *qry_values3[1]	= {"*"};
	char		*user_id			= NULL;
	logical modB=FALSE;
	logical modBCol=FALSE;
	char *FileDown = NULL;
	FileDown=(char *) MEM_alloc(100 * sizeof(char ));
    (void)argc;
    (void)argv;

    Platform     = ITK_ask_cli_argument("-i=");   
	SVRrule        = ITK_ask_cli_argument("-s=");
	Clrscheme    = ITK_ask_cli_argument("-shm=");
	DMLNo        = ITK_ask_cli_argument("-dml=");
	user_name_string    = ITK_ask_cli_argument("-Usr=");
	
	ifail = ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
	ifail = ITK_auto_login();
    ifail = ITK_set_journalling( TRUE );
	
	printf("\n Platform-------->  : %s",Platform); fflush(stdout);
	printf("\n SVRrule-------->  : %s",SVRrule); fflush(stdout);
	printf("\n Clrscheme-------->  : %s",Clrscheme); fflush(stdout);
	printf("\n DMLNo-------->  : %s",DMLNo); fflush(stdout);
	printf("\n user_name_string-------->  : %s\n\n",user_name_string);fflush(stdout);

	printf("\n SA_find_user2-------->  : %s\n\n",user_name_string);fflush(stdout);
	ITK_CALL(SA_find_user2(user_name_string, &user_tag));
	ITK_CALL(SA_find_group("ERC_Designers_Group", &group))

	tc_strcpy(FileDown,"");
	tc_strcat(FileDown,"/tmp/");
	tc_strcat(FileDown,Platform);
	tc_strcat(FileDown,"_");
	tc_strcat(FileDown,DMLNo);
	tc_strcat(FileDown,"_");
	tc_strcat(FileDown,"ClrUnitBOM.txt");
	printf("\n FileDown---------> %s\n",FileDown);fflush(stdout);
	output = fopen(FileDown, "a+");

	int Child_Rev_count,dd,RevCnt,hp =0;
	tag_t*	Child_rev_list	= NULLTAG;

	ifail = GRM_find_relation_type("T5_ShmHasClrData",&SchmWithCmpcdRelMultlvl);
	NVCRlzRevFlag=0;

	if( Platform )
	{		
		//QUERY VARIANT RULE
		if(QRY_find("VariantRule", &VariqueryTag));
		if (VariqueryTag) { printf("\n Found Query VariantRule------>\n"); fflush(stdout);
		}else {
			ifail = POM_logout(false);
			return status;
		}

		valuesNonColSvr[0] = SVRrule;
		if(QRY_execute(VariqueryTag, n_entriesV, qry_entries3, valuesNonColSvr, &resultCountVNCol, &outputVNCol_tags));
		printf("\n resultCount---------->%d\n\t",resultCountVNCol); fflush(stdout);

		if (resultCountVNCol > 0)
		{
			NonColor=tc_strtok(SVRrule,"_");
			printf ( "\n NonColor is -------> %s\n",NonColor);fflush(stdout);
		
			Item_Rev = tc_strtok(NULL,"_");
			printf("\n Item_Rev --> : %s\n",Item_Rev); fflush(stdout);			

			ITK_CALL(STRNG_replace_str(NonColor,"000R","R",&Vehicle));
			printf ( "\n Vehicle is -------> %s\n",Vehicle);fflush(stdout);

			ITK_CALL(ITEM_find_item(Vehicle, &VeHMstr));
			printf("\n VeHMstr is ------->\n");fflush(stdout);
			if(VeHMstr)
			{		
				ifail = ITEM_list_all_revs (VeHMstr, &Child_Rev_count, &Child_rev_list);
				printf("\n Child_rev_list---------------> : %d\n", Child_Rev_count);fflush(stdout);
				if(Child_Rev_count > 0)
				{
					for(dd=Child_Rev_count-1;dd>=0;dd--)
					{
						ITK_CALL(AOM_UIF_ask_value (Child_rev_list[dd], "release_status_list", &RelStatusmatch));
						if((tc_strstr(RelStatusmatch,"ERC Released")!=NULL)||(tc_strstr(RelStatusmatch,"T5_LcsErcRlzd")!=NULL))
						{
							VehicleRlzRev=Child_rev_list[dd];
							break;
						}							
					}
				}				
			}
		}else { exit (0); }
	
		//QUERY ERC DML
		if(QRY_find("ERC DML", &ERCDMLTag));
		if (ERCDMLTag) { printf("\n Found Query ERCDMLTag\n"); fflush(stdout);
		}else {
			ifail = POM_logout(false);
			return status;
		}			
		valuesERCDML[0] = DMLNo ;
		if(QRY_execute(ERCDMLTag, n_entriesDML, qry_entries5, valuesERCDML, &resultDMLNo, &DMLNotags));
		printf("\n resultDMLNo count is-------->  : %d\n",resultDMLNo);fflush(stdout);
		if (resultDMLNo > 0)
		{
			ERCDMLObjtag = DMLNotags[0];
		}else { exit (0); }

		ITK_CALL(TCTYPE_ask_object_type(ERCDMLObjtag,&ERCDMLTypeTag));
		ITK_CALL(TCTYPE_ask_name(ERCDMLTypeTag,ERCDML_name));
		printf("\n ERCDML_Typename------> %s\n",ERCDML_name);fflush(stdout);
		if(strcmp(ERCDML_name,"ChangeRequestRevision")==0)
		{
			ifail = GRM_find_relation_type("T5_DMLTaskRelation", &relation_type);
			if (relation_type!=NULLTAG)
			{
				ifail = GRM_list_secondary_objects_only(ERCDMLObjtag,relation_type,&n_attchs,&secondary_objects);
				printf("\n\n\t\t No of DI : %d",n_attchs);fflush(stdout);
				if(n_attchs>0)
				{
					for (l=0;l<n_attchs ;l++ )
					{					
						ifail = AOM_ask_value_string(secondary_objects[l],"item_id",&Object_id);
						printf("\n\n\t\t Object_id is :%s",Object_id);fflush(stdout);
						if(tc_strstr(Object_id,"_00")!=NULL)
						{	
							if(taskRevTag) taskRevTag=NULLTAG;
							taskRevTag=secondary_objects[l];
						}
					}
				}
			}
		}
		
		//QUERY COLOUR SCHEME
		if(QRY_find("ClrScheme", &SchmqueryTag));
		if (SchmqueryTag) {
			printf("\n\n Found Query SchmqueryTag"); fflush(stdout);
		}else {
			ifail = POM_logout(false);
			return status;
		}		
		CpyClrscheme =  strtok(Clrscheme,",");
		printf ("\n CpyClrscheme --> [%s]\n", CpyClrscheme); fflush(stdout);
		valuesColSchm[0] = CpyClrscheme ;
		if(QRY_execute(SchmqueryTag, n_entriesSchm, qry_entries4, valuesColSchm, &resultColSchm, &ColSchme_tags));
		printf("\n resultColSchm count is-------->  : %d\n",resultColSchm);fflush(stdout);
		if (resultColSchm > 0)
		{
			ColorSchemetag = ColSchme_tags[0];			
		}else { exit(0); }
		
		ITK_CALL(ITEM_ask_latest_rev(ColorSchemetag, &ColSchmRevTag));
		ITK_CALL(AOM_ask_value_string(ColSchmRevTag, "object_string", &t5objectStr));
		printf("\n t5objectStr is --> : %s\n\n",t5objectStr); fflush(stdout);
		
		SchTok=strtok(t5objectStr,";");
		printf ("\n Color Scheme is : %s\n", SchTok);fflush(stdout);

		ITK_CALL(STRNG_replace_str(SchTok,",","#",&SchDataToGov));
		printf ( "\n SchDataToGov is -------> %s\n",SchDataToGov);fflush(stdout);		

		ITK_CALL(AOM_ask_value_string(ColSchmRevTag, "t5_ClSrl", &t5ClSrl));
		ITK_CALL(WSOM_ask_release_status_list(ColSchmRevTag, &status_count, &relStatus_list));
		printf("\n status_count is --> : %d\n\n",status_count); fflush(stdout);
		if(status_count > 0)
		{
			RlzRevFlag=0;
			for(rs = 0; rs < status_count; rs++)
			{
				if(AOM_ask_name(relStatus_list[rs], &ProObjRelSt)==ITK_ok)
				printf("\t ColSchm Release status is:=>%s", ProObjRelSt); fflush(stdout);
				if (tc_strcmp(ProObjRelSt,"T5_LcsErcRlzd")==0)
				{
					RlzRevFlag=1;
					break;
				}
			}
		}

		ITK_CALL(GRM_find_relation_type("IMAN_reference", &IMANReferences_type));	//References folder		
		if(IMANReferences_type!=NULLTAG)
		{	
			ITK_CALL(GRM_find_relation(ERCDMLObjtag,ColSchmRevTag,IMANReferences_type,&CMRefExist));
			if(CMRefExist==NULLTAG)
			{
				ITK_CALL(GRM_create_relation(ERCDMLObjtag,ColSchmRevTag,IMANReferences_type, NULLTAG, &Rel_References));
				ITK_CALL(AOM_load(Rel_References)); 
				ITK_CALL(GRM_save_relation(Rel_References)); 
				ITK_CALL(AOM_refresh(Rel_References,0));
			}else
			printf("\n Input Scheme is already attached to DML Refernce folder-------------------->>>>\n");fflush(stdout);
		}
		
		printf ("\n NonColor is -------> %s\n",NonColor);fflush(stdout);
		printf("\n t5ClSrl is --> : %s\n\n",t5ClSrl); fflush(stdout);
		if(tc_strstr(t5ClSrl,";")!=NULL)
		{															
			Suffix =  strtok(t5ClSrl,";");
			printf("\n Suffix --> : %s\n",Suffix); fflush(stdout);

			ColourID  =  strtok(NULL,";");
			printf("\n ColourID --> : %s\n",ColourID); fflush(stdout);
		}

		ITK_CALL(ITEM_find_item(NonColor,&NVCtag))
		if(NVCtag)
		{				
			ITK_CALL(ITEM_list_all_revs (NVCtag, &Child_VCRev_count, &Child_VCrev_list));
			printf("\n Child_VCRev_count---------------> : %d\n", Child_VCRev_count);fflush(stdout);
			for(hd = 0; hd < Child_VCRev_count; hd++)
			{
				ITK_CALL(WSOM_ask_release_status_list(Child_VCrev_list[hd], &VCRevCnt, &RlzStus_VC_list));
				printf("\n VCRevCnt is --> : %d\n\n",VCRevCnt); fflush(stdout);
				if(VCRevCnt > 0)
				{
					for(hs = 0; hs < VCRevCnt; hs++)
					{
						if(AOM_ask_name(RlzStus_VC_list[hs], &VCRelStatusM)==ITK_ok)
						printf("\t Release status is:=>%s", VCRelStatusM); fflush(stdout);
						if (tc_strcmp(VCRelStatusM,"T5_LcsErcRlzd")==0)
						{
							NVCRlzRev = Child_VCrev_list[hd];
							NVCRlzRevFlag=1;
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "item_id", &t5itemid));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_PrtCatCode", &t5PrtCatCd));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "current_revision_id", &t5revision_id));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "object_desc", &t5object_desc));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_ColourInd", &t5_ColourInd));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_PartStatus", &t5_PartStatus));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_DesignGrp", &t5_DesignGrp));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_ProjectCode", &t5_ProjectCode));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_DocRemarks", &t5_DocRemarks));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_DrawingInd", &t5_DrawingInd));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_PartType", &t5_PartType));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_PartCode", &t5_PartCode));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_AhdMakeBuyIndicator", &t5_AhdMakeBuyIndicator));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_PunPCBUMakeBuyIndicator", &t5_PunPCBUMakeBuyIndicator));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_PunMakeBuyIndicator", &t5_PunMakeBuyIndicator));
							ITK_CALL(AOM_ask_value_string(NVCRlzRev, "t5_DwdMakeBuyIndicator", &t5_DwdMakeBuyIndicator));

							ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&SchmWithCmpcdRel_VC));
							ITK_CALL(GRM_list_secondary_objects_only(ColSchmRevTag, SchmWithCmpcdRel_VC, &CmpVcCnt, &CmpCodeOfClrSchemeVC));
							printf("\n Comp Code CmpVcCnt for Color Scheme : %d\n\n\n",CmpVcCnt); fflush(stdout);							
							if(tc_strcmp(t5_ColourInd,"Y")!=0)
							{
								goto END;
							}

							if(CmpVcCnt > 0)
							{
								for(g2=0;g2<CmpVcCnt;g2++ )
								{													
									if(VCCmpCodeObj) VCCmpCodeObj=NULLTAG;
									VCCmpCodeObj=CmpCodeOfClrSchemeVC[g2];
									ITK_CALL(AOM_ask_value_string(VCCmpCodeObj, "t5_PrtCatCode", &CmpCodeCmp));
									if(tc_strcmp(t5PrtCatCd,CmpCodeCmp)==0)
									{	
										printf("\n CmpCodeCmp & t5PrtCatCd is --> : %s = %s\n\n",CmpCodeCmp,t5PrtCatCd); fflush(stdout);
										ITK_CALL(AOM_ask_value_string(VCCmpCodeObj, "t5_ClSrl", &t5ClSrl ));
										printf("\n t5ClSrl --> : %s\n",t5ClSrl); fflush(stdout);
										if(tc_strstr(t5ClSrl,";")!=NULL)
										{															
											Suffix =  strtok(t5ClSrl,";");
											printf("\n Suffix --> : %s\n",Suffix); fflush(stdout);

											ColourID  =  strtok(NULL,";");
											printf("\n ColourID --> : %s\n",ColourID); fflush(stdout);
										}
										break;
									}
								}
							}
						}
					}
				} 			
			}
		}	

		printf("\n\n\n NVCRlzRevFlag is------------------------------------>%d\n", NVCRlzRevFlag); fflush(stdout);
		if(NVCRlzRevFlag==1)
		{
			strcpy(SuffixCpy,"");
			tc_strcat(SuffixCpy,"A");
			if(tc_strstr(t5ClSrl,";")!=NULL)
			{
				tc_strcat(SuffixCpy,Suffix);
			}else
			{
				tc_strcat(SuffixCpy,t5ClSrl);
			}
			printf("\n SuffixCpy => %s\n",SuffixCpy );fflush(stdout);
			ITK_CALL(STRNG_replace_str(NonColor,"000",SuffixCpy,&ColorVC));
			printf("\n ColorVC is -------> %s\n",ColorVC);fflush(stdout);

			if(QRY_find("ClrPart", &VCItemTag));
			valuesCCV[0] = ColorVC;
			if(QRY_execute(VCItemTag, n_entriesCCV, CCV_entries, valuesCCV, &n_VCtag, &VCtags_found));
			printf("\n n_VCtag count is-------->  : %d\n",n_VCtag);fflush(stdout);

			if(n_VCtag > 0)
			{
				ITK_CALL(ITEM_find_item(ColorVC,&VCtag))
				ITK_CALL(ITEM_ask_latest_rev(VCtag, &VCItemRev));
				ifail =ITEM_list_bom_views(VCtag,&bv_clrcount, &clr_bvs);
				printf("\n bv_clrcnt :%d..\n",bv_clrcount);fflush(stdout);
				if (bv_clrcount)
				{
					VCclrbv = clr_bvs[0];
					MEM_free(clr_bvs);
				}else{
					ifail = PS_create_bom_view( NULLTAG, "", "", VCtag,&VCclrbv );
					ifail = AOM_save( VCclrbv );
					ifail = AOM_save( VCtag );
					ifail = AOM_unlock( VCtag );
					printf("\n inside PS_create_bom_view..\n");fflush(stdout);
				}

				if (VCItemRev!=NULLTAG)
				{
					ifail = ITEM_rev_list_bom_view_revs(VCItemRev,&clr_bvrcnt, &clrbvrs);
					printf("\n clr_bvrcnt :%d..\n",clr_bvrcnt);fflush(stdout);
					if (clr_bvrcnt)
					{
						VCclrbvr = clrbvrs[0];
						MEM_free(clrbvrs);
					}else{
						ifail = PS_create_bvr( VCclrbv, "", "", false, VCItemRev,&VCclrbvr);
						if(VCclrbvr !=NULLTAG)
						{								
							ifail = AOM_save( VCclrbvr);
							ifail = AOM_save( VCItemRev );
							ifail = AOM_unlock( VCItemRev );
							printf("\n inside  PS_create_bvr..\n");fflush(stdout);
						}
					}
				}

				ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&CCVSolutionItem));
				if (CCVSolutionItem!=NULLTAG)
				{
					ITK_CALL(GRM_find_relation(taskRevTag,VCItemRev,CCVSolutionItem,&CCVItemExist));					
					if (CCVItemExist==NULLTAG)
					{
						ITK_CALL(GRM_create_relation(taskRevTag,VCItemRev,CCVSolutionItem,NULLTAG,&CCVreltask));
						ITK_CALL(GRM_save_relation(CCVreltask)); 
						ITK_CALL(AOM_load(CCVreltask));
						ITK_CALL(AOM_refresh(CCVreltask,0));
					}
				}
			}else
			{				
				//CREATE COLOR PART IF IS NOT AVAILABLE IN TCE
				ITK_CALL(ITEM_create_item(ColorVC,ColorVC,"T5_ClrPart",default_empty_to_A(ClrRevSeq),&item,&rev));
				setAttributesOnDesign(&item,ColorVC,t5object_desc,"4");					
				setAttributesOnDesignRev(&rev,ColorVC,item_revision_id_int,t5object_desc,t5_ColourInd,t5PrtCatCd,ColourID,t5CoatedS,t5_PartStatus,t5_DesignGrp,t5_ProjectCode,t5_DocRemarks,t5_DrawingInd,t5_PartType,t5_PartCode,t5_AhdMakeBuyIndicator,t5_PunPCBUMakeBuyIndicator,t5_PunMakeBuyIndicator,t5_DwdMakeBuyIndicator,t5itemid);
				ITK_CALL(CreateReleasestatus(rev));
				ITK_CALL(AssignProject(rev,t5_ProjectCode));
				ITK_CALL(AOM_set_ownership(rev,user_tag ,group));
				ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&CCVSolutionItem));
				if (CCVSolutionItem!=NULLTAG)
				{
					ITK_CALL(GRM_find_relation(taskRevTag,rev,CCVSolutionItem,&CCVItemExist));					
					if (CCVItemExist==NULLTAG)
					{
						ITK_CALL(GRM_create_relation(taskRevTag,rev,CCVSolutionItem,NULLTAG,&CCVreltask));
						ITK_CALL(GRM_save_relation(CCVreltask)); 
						ITK_CALL(AOM_load(CCVreltask));
						ITK_CALL(AOM_refresh(CCVreltask,0));
					}
				}

				ITK_CALL(GRM_find_relation_type("TC_Is_Represented_By",&CCVRelationFind));
				if (CCVRelationFind!=NULLTAG)
				{	
					ITK_CALL(GRM_create_relation(rev,NVCRlzRev,CCVRelationFind,NULLTAG,&CCVObject));
					ITK_CALL(GRM_save_relation(CCVObject)); 
					ITK_CALL(AOM_load(CCVObject));
					ITK_CALL(AOM_refresh(CCVObject,0));
					printf("\nRelation Created Successfully\n");fflush(stdout);							
				}

				ifail =ITEM_list_bom_views(item,&bv_clrcount, &clr_bvs);
				printf("\n bv_clrcnt :%d..\n",bv_clrcount);fflush(stdout);
				if (bv_clrcount)
				{
					VCclrbv = clr_bvs[0];
					MEM_free(clr_bvs);
				}else{
					ifail = PS_create_bom_view( NULLTAG, "", "", item,&VCclrbv );
					ifail = AOM_save( VCclrbv );
					ifail = AOM_save( item );
					ifail = AOM_unlock( item );
					printf("\n inside PS_create_bom_view..\n");fflush(stdout);
				}

				if (rev!=NULLTAG)
				{
					ifail = ITEM_rev_list_bom_view_revs(rev,&clr_bvrcnt, &clrbvrs);
					printf("\n clr_bvrcnt :%d..\n",clr_bvrcnt);fflush(stdout);
					if (clr_bvrcnt)
					{
						VCclrbvr = clrbvrs[0];
						MEM_free(clrbvrs);
					}else{
						ifail = PS_create_bvr( VCclrbv, "", "", false, rev,&VCclrbvr);
						if(VCclrbvr !=NULLTAG)
						{								
							ifail = AOM_save( VCclrbvr);
							ifail = AOM_save( rev );
							ifail = AOM_unlock( rev );
							printf("\n inside  PS_create_bvr..\n");fflush(stdout);
						}
					}
				}
			}
			printf("\n Color VC is Created------------------------------------>\n\n\n\n"); fflush(stdout);
		}else
		{
			goto END;
		}
		
		//int rulefound= 0;
		//tag_t *closurerule =NULLTAG;
		//tag_t close_tag;
		//char **rulename = NULL;
		//char **rulevalue = NULL;
		
		printf("\n\t Colour Scheme Release Status------->%d\n\n", RlzRevFlag); fflush(stdout);
		if((RlzRevFlag > 0) && (VehicleRlzRev!=NULLTAG))
		{
			ITK_CALL(BOM_create_window(&bom_window));
			ITK_CALL(CFM_find("ERC release and above", &rule));
			ITK_CALL(BOM_set_window_config_rule(bom_window, rule ));			
			/*
			ITK_CALL(PIE_find_closure_rules("BOMViewClosureRuleERC",PIE_TEAMCENTER, &rulefound, &closurerule)); //ERC release and above APLC
			if (rulefound > 0) {
				close_tag = closurerule[0];
				printf ("closure rule found \n");fflush(stdout);
			}
			*/
			ITK_CALL(BOM_window_set_closure_rule(bom_window,close_tag, 0, rulename,rulevalue));				
			ITK_CALL(BOM_set_window_pack_all(bom_window, TRUE));
			ITK_CALL(BOM_set_window_top_line(bom_window , NULLTAG, VehicleRlzRev, NULLTAG, &top_line));
			ITK_CALL(BOM_window_show_suppressed (bom_window));
			
			if(top_line!=NULLTAG)
			{
				ITK_CALL(AOM_ask_value_string(top_line, "bl_item_item_id", &bl_Item_id));
				printf("\n Item_id of First level Modules is---------------->%s\n",bl_Item_id); fflush(stdout);
				ITK_CALL(AOM_ask_value_string(top_line, "bl_rev_item_revision_id", &item_Rev_Seq));
				Item_Rev =  tc_strtok(item_Rev_Seq,";");
				Item_Seq = tc_strtok(NULL,";");
				ITK_CALL(AOM_ask_value_int(top_line, "bl_level_starting_0", &NonClrPartlevel ));
				ITK_CALL(AOM_ask_value_string(top_line, "bl_T5_ClrPartRevision_t5_PrtCatCode", &t5_PrtCatCd ));
				printf("\n NonColor Module Compcode is--> : %s\n",t5_PrtCatCd); fflush(stdout);
				ITK_CALL(AOM_ask_value_string(top_line, "bl_T5_ClrPartRevision_t5_ColourInd", &t5ColourInd ));
				printf("\n NonColor Module ColourInd is--> : %s\n",t5ColourInd); fflush(stdout);
				if((strlen(t5_PrtCatCd)>0) && (tc_strcmp(t5ColourInd,"Y")==0))
				{	
					ITK_CALL(QRY_find("DesignRevision Sequence", &queryds));
					itemRevSeq2 = NULL;
					itemRevSeq2=(char *) MEM_alloc(32);
					tc_strcpy(itemRevSeq2,Item_Rev);
					tc_strcat(itemRevSeq2,"?");
					tc_strcat(itemRevSeq2,Item_Seq);
					printf("\n itemRevSeq2 ===> %s\n",itemRevSeq2);fflush(stdout);

					qry_valuesds[0] = itemRevSeq2;
					qry_valuesds[1] = bl_Item_id;									
					ITK_CALL(QRY_execute(queryds, n_entries, qry_entriesds, qry_valuesds, &ds_tags_found, &itemrevdsclass));
					printf("\n Non-Color found in DB is :: %d\n", ds_tags_found);fflush(stdout);
					if(ds_tags_found > 0)
					{
						latestrev=NULLTAG;
						latestrev=itemrevdsclass[0];
						ITK_CALL(AOM_ask_value_string(latestrev, "item_id", &t5itemid));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_PrtCatCode", &t5PrtCatCd));
						ITK_CALL(AOM_ask_value_string(latestrev, "current_revision_id", &t5revision_id));
						ITK_CALL(AOM_ask_value_string(latestrev, "object_desc", &t5object_desc));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_ColourInd", &t5_ColourInd));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_PartStatus", &t5_PartStatus));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_DesignGrp", &t5_DesignGrp));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_ProjectCode", &t5_ProjectCode));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_DocRemarks", &t5_DocRemarks));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_DrawingInd", &t5_DrawingInd));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_PartType", &t5_PartType));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_PartCode", &t5_PartCode));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_AhdMakeBuyIndicator", &t5_AhdMakeBuyIndicator));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_PunPCBUMakeBuyIndicator", &t5_PunPCBUMakeBuyIndicator));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_PunMakeBuyIndicator", &t5_PunMakeBuyIndicator));
						ITK_CALL(AOM_ask_value_string(latestrev, "t5_DwdMakeBuyIndicator", &t5_DwdMakeBuyIndicator));								
						ITK_CALL(RES_is_checked_out(latestrev,&checkout));
						printf("\n Module Checked-out Status :=> [%d]\n",checkout);fflush(stdout);
						if(checkout==0)
						{	
							ITK_CALL(GRM_find_relation_type("T5_ShmHasClrData",&SchmWithCmpcdRel));
							if (SchmWithCmpcdRel!=NULLTAG)
							{
								ITK_CALL(GRM_list_secondary_objects_only(ColSchmRevTag, SchmWithCmpcdRel, &Cmpcount, &CmpCodeOfClrScheme));
								printf("\n Comp Code Cmpcount for Color Scheme : %d\n\n\n",Cmpcount); fflush(stdout);
								if(Cmpcount > 0)
								{
									for(g1=0;g1<Cmpcount;g1++ )
									{													
										if(CmpCodeObj) CmpCodeObj=NULLTAG;
										CmpCodeObj=CmpCodeOfClrScheme[g1];
										ITK_CALL(AOM_ask_value_string(CmpCodeObj, "t5_PrtCatCode", &CmpCodeCmp));
										printf("\n CmpCodeCmp & t5PrtCatCd is --> : %s = %s\n\n",CmpCodeCmp,t5PrtCatCd); fflush(stdout);
										if(tc_strcmp(t5PrtCatCd,CmpCodeCmp)==0)
										{
											ITK_CALL(AOM_ask_value_string(CmpCodeObj, "t5_ClSrl", &t5ClSrl ));
											printf("\n t5ClSrl --> : %s\n",t5ClSrl); fflush(stdout);
											if(tc_strstr(t5ClSrl,";")!=NULL)
											{															
												Suffix =  strtok(t5ClSrl,";");
												printf("\n Suffix --> : %s\n",Suffix); fflush(stdout);

												ColourID  =  strtok(NULL,";");
												printf("\n ColourID --> : %s\n",ColourID); fflush(stdout);
											}else{
												Suffix =  strtok(t5ClSrl,",");
												printf("\n Suffix --> : %s\n",Suffix); fflush(stdout);

												ColourID  =  strtok(NULL,",");
												printf("\n ColourID --> : %s\n",ColourID); fflush(stdout);
											}

											itemRevSeq = NULL;
											itemRevSeq=(char *) MEM_alloc(32);														
											strcpy(itemRevSeq,"");
											strcat(itemRevSeq,t5itemid);
											strcat(itemRevSeq,Suffix);														
											printf("\n Colour Part for Creation---->%s = %s\n",itemRevSeq,ClrRevSeq);

											if(QRY_find("ClrPart", &ClrPartTag));
											valuesClrPrt[0] = itemRevSeq;
											if(QRY_execute(ClrPartTag, n_entriesC, qry_entries, valuesClrPrt, &n_tags_found1, &tags_found1));
											printf("\n\n\n ClrPart count is-------->  : %d\n",n_tags_found1);fflush(stdout);
											if(ItemTag1) ItemTag1=NULLTAG;
											if(item) item=NULLTAG;
											if(rev) rev=NULLTAG;
											if(n_tags_found1 > 0)
											{																
												ItemTag1 = tags_found1[0];
												printf("\n\t available in DB---->%s\n\n",itemRevSeq);fflush(stdout);	
												if(ItemTag1!=NULLTAG)
												{
													flg1=MultilevelExpantion(ItemTag1,latestrev,ColSchmRevTag,NonClrPartlevel,taskRevTag,user_tag,group);
													if (flg1==-1) {
														goto END;
													}
												}
											}else
											{
												/////////////////////////////WebSerFunForClrPart/////////////////////////////////////////////
												printf("\n Color Part is Checking in TCE---------->%s\n",itemRevSeq); fflush(stdout);
												Response = WebSerFunForClrPart(itemRevSeq);
												printf("Color Part checking in TCE :: [%s]",Response);fflush(stdout);

												if(tc_strcmp(Response,"NA")!=0)
												{
													fprintf(output,"%s\n",itemRevSeq);
													goto END;
												}

												if(tc_strstr(itemRevSeq,"##")!=NULL)
												{
													fprintf(output,"%s\n",itemRevSeq);
													goto END;
												}

												//CREATE COLOR PART IF IS NOT AVAILABLE IN TCE
												ITK_CALL(ITEM_create_item(itemRevSeq,itemRevSeq,"T5_ClrPart",default_empty_to_A(ClrRevSeq),&item,&rev));
												setAttributesOnDesign(&item,itemRevSeq,t5object_desc,"4");					
												setAttributesOnDesignRev(&rev,itemRevSeq,item_revision_id_int,t5object_desc,t5_ColourInd,t5PrtCatCd,ColourID,t5CoatedS,t5_PartStatus,t5_DesignGrp,t5_ProjectCode,t5_DocRemarks,t5_DrawingInd,t5_PartType,t5_PartCode,t5_AhdMakeBuyIndicator,t5_PunPCBUMakeBuyIndicator,t5_PunMakeBuyIndicator,t5_DwdMakeBuyIndicator,bl_Item_id);
												ITK_CALL(CreateReleasestatus(rev));
												ITK_CALL(AssignProject(rev,t5_ProjectCode));
												ITK_CALL(AOM_set_ownership(rev,user_tag ,group));
												printf("\nproperty Value set...!!\n");fflush(stdout);

												//FIND BVR for COLOR PART REVISION
												if(item!=NULLTAG)
												{
													ITK_CALL(PS_create_bom_view(NULLTAG, "", "", item,&clrbv));
													ITK_CALL(AOM_save(clrbv));
													ITK_CALL(AOM_save(item));
													ITK_CALL(AOM_unlock(item));
													ITK_CALL(ITEM_ask_latest_rev(item,&Clrrev1));
													ITK_CALL(PS_create_bvr(clrbv, "", "", false, Clrrev1,&clrbvr));
													if(clrbvr !=NULLTAG)
													{								
														ITK_CALL(AOM_save(clrbvr));
														ITK_CALL(AOM_save(Clrrev1 ));
														ITK_CALL(AOM_unlock(Clrrev1 ));
														printf( "\n inside  PS_create_bvr..\n");fflush(stdout);
													}														
												}

												//ATTACHED COLOR PART TO THE DML 00 TASK
												ITK_CALL(GRM_find_relation_type("CMHasSolutionItem",&RelSolutionItem));
												if (RelSolutionItem!=NULLTAG)
												{
													ITK_CALL(GRM_list_primary_objects_only(rev,RelSolutionItem,&TskCntClr,&TskSOClr));
													if(TskCntClr > 0) {
														printf("\n Color Part is already attached to other task---------->%s\n",itemRevSeq); fflush(stdout);																		
													}else
													{
														ITK_CALL(GRM_create_relation(taskRevTag,rev,RelSolutionItem,NULLTAG,&reltask));
														ITK_CALL(GRM_save_relation(reltask));
														ITK_CALL(AOM_load(reltask));
														ITK_CALL(AOM_refresh(reltask,0));
														printf("\nRelation Created Successfully with CMHasSolutionItem\n");fflush(stdout);																
													}
												}
														
												//CREATE COLOR & NON-COLOR RELATION & ATTACHED COLOR PART TO TOP SVR LINE
												ITK_CALL(GRM_find_relation_type("TC_Is_Represented_By",&tRelationFind));
												if (tRelationFind!=NULLTAG)
												{
													ITK_CALL(GRM_find_relation(rev,latestrev,tRelationFind,&tRelationExist));					
													if (tRelationExist==NULLTAG)
													{
														ITK_CALL(GRM_create_relation(rev,latestrev,tRelationFind,NULLTAG,&Rel_task));
														ITK_CALL(GRM_save_relation(Rel_task)); 
														ITK_CALL(AOM_load(Rel_task));
														ITK_CALL(AOM_refresh(Rel_task,0));
														printf("\nRelation Created Successfully\n");fflush(stdout);														
													}
												}
												
												if(VCclrbvr!=NULLTAG)
												{
													ITK_CALL(AOM_lock(VCclrbvr));	
													ITK_CALL(PS_create_occurrences(VCclrbvr, rev, NULLTAG,1, &occs ));
													ITK_CALL(AOM_save(VCclrbvr));
													ITK_CALL(AOM_unlock(VCclrbvr));
													ITK_CALL(AOM_refresh(VCclrbvr,0));
													printf("\n Color Vehicle is attached to bvr of Color VC--------------->\n");fflush(stdout);											
												}
												
												//CALL MULTILEVEL EXPANSION FUNCTION
												if(item!=NULLTAG)
												{
													flg1=MultilevelExpantion(item,latestrev,ColSchmRevTag,NonClrPartlevel,taskRevTag,user_tag,group);
													if (flg1==-1) {
														goto END;
													}
												}
											}
											break;
										}
									}
								}
							}
						}
					}
				}
			}		
		}else
		{	
			printf("\n Colour Scheme OR Vehicle is not Release-------------------->\n"); fflush(stdout);
			exit(0);
		}
	}else { printf("\n PLATFROM IS INVALID-------->\n"); fflush(stdout); 
	}
END:
	fclose(output);
	ITK_exit_module(true);
	return status;
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
