
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
#define Debug TRUE
#define ITK_CALL(X)																		 \
        if(Debug)																		 \
        {                                                                                \
                printf(#X);																 \
        }                                                                                \
        fflush(NULL);																	 \
        status=X;																		 \
        if (status != ITK_ok )															 \
        {																				 \
                int                             index = 0;								 \
                int                             n_ifails = 0;							 \
                const int*              severities = 0;									 \
                const int*              ifails = 0;										 \
                const char**    texts = NULL;											 \
																						 \
                EMH_ask_errors( &n_ifails, &severities, &ifails, &texts);				 \
                printf("\t%3d error(s)\n", n_ifails);                                    \
                for( index=0; index<n_ifails; index++)                                   \
                {                                                                        \
                        printf("\tError #%d, %s\n", ifails[index], texts[index]);        \
                }                                                                        \
                return status;                                                           \
        }                                                                                \
        else																			 \
        {                                                                                \
                if(Debug)																 \
                printf("\tSUCCESS\n");													 \
        }                                                                                \

#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}

static int name_attribute, seqno_attribute, parent_attribute, item_tag_attribute;
static void initialise (void);
static void initialise_attribute (char *name,  int *attribute);
static void ExpandMultiLevelBom (tag_t bom_line_tag, int depth,char *outputfile,FILE *fptr);
extern int ITK_user_main (int argc, char ** argv )
{
        int ifail;
        int status;
        char *req_item = NULL;
        char *req_item2 = NULL;
        char *inputfile=NULL;
        char *outputfile=NULL;
        tag_t *LatestRev = NULL;
        tag_t window, rule, item_tag = null_tag, top_line;
		int iStatus = 0;
        FILE *fptr;

        (void)argc;
        (void)argv;

        initialise();

        req_item = ITK_ask_cli_argument("-i=");
        req_item2 = ITK_ask_cli_argument("-o=");
		

		iStatus = ITK_auto_login();

        //if( ITK_init_module("loader" ,"loader7","dba")!=ITK_ok) ;
        ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
        ITK_CALL(ITK_set_journalling( TRUE ));
        printf("\n login .......");fflush(stdout);

        inputfile=(char *) MEM_alloc(50);
        tc_strcpy(inputfile,"BOMList_CADFiles_");
        tc_strcat(inputfile,req_item);
        tc_strcat(inputfile,".txt");

		outputfile=(char *) MEM_alloc(100);
        tc_strcpy(outputfile,req_item2);

		printf ("\n outputfile is %s\n", outputfile); fflush(stdout);
       	

        fptr=fopen(inputfile,"w");

        ifail = BOM_create_window (&window);
        CHECK_FAIL;
        ifail = CFM_find( "Latest Working", &rule );
        CHECK_FAIL;
        ifail = BOM_set_window_config_rule( window, rule );
        CHECK_FAIL;

        if ( req_item )
        {
                tag_t *tags_found = NULL;
                int n_tags_found= 0;
                int n_rev= 0;
                char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
                char **values = (char **) MEM_alloc(1 * sizeof(char *));

                attrs[0] ="item_id";
                values[0] = (char *) req_item;

                ifail = ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found);
                MEM_free(attrs);
                MEM_free(values);
                CHECK_FAIL;

                if (n_tags_found == 0)
                {
                        printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", req_item); fflush(stdout);
                        exit (0);
                }

                item_tag = tags_found[0];

                ITK_CALL(ITEM_ask_latest_rev(item_tag,&LatestRev));

                MEM_free(tags_found);
        }
        else
        {
                printf ("Pls enter input part ?? \n"); fflush(stdout);
                exit (0);
        }

        if (item_tag == null_tag)
        {
                printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", req_item); fflush(stdout);
                exit (0);
        }

        ifail = BOM_set_window_top_line (window, null_tag, LatestRev, null_tag, &top_line);
        CHECK_FAIL;

        if(fptr==NULL)
        {
                printf("Error! in File opening...%s",inputfile);  fflush(stdout);
                exit(1);
        }

        ExpandMultiLevelBom (top_line, 0,outputfile,fptr);

        ITK_exit_module(true);

        return status;
}
static void ExpandMultiLevelBom (tag_t bom_line_tag, int depth,char *outputfile, FILE *fptr)
{
        int ifail;
        int iChildItemTag;
        int status;
        int cnt=0;
        int cnt2=0;
        int referencenumberfound=0;
        char *name, *sequence_no;
        char *Item_id_par=NULL;
        char *Item_LatestRevSeq=NULL;
        char *ItemRev=NULL;
        char *ItemSeq=NULL;
        char *ItemJT=NULL;
        char *ItemCad=NULL;
        char *word=NULL;
		char *tmatrix=NULL;
		char *sPartType=NULL;
        char *DsetObjType=NULL;
        int i, n,j;
        int k = 0;
        int level=0;
		int IsSuppres=0;
		//int IsSuppres1=0;
		//logical  IsSuppres3=false;
        tag_t *children;
        tag_t item=NULLTAG;
        tag_t reva=NULLTAG;
        tag_t t_ChildItemRev;
        tag_t *tags_found = NULL;
        int n_tags_found= 0;
        tag_t *attachments = NULLTAG;
        tag_t *attachments2 = NULLTAG;
        tag_t dataset = NULLTAG;
        tag_t refobject = NULLTAG;
        tag_t relation_type, relation_type2;
        tag_t *related_occs = NULLTAG;
        tag_t *related_items =  NULLTAG;
        char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
        char **values = (char **) MEM_alloc(1 * sizeof(char *));
        char refname[AE_reference_size_c + 1];
        char orig_name[IMF_filename_size_c + 1];
        AE_reference_type_t reftype;
        char *sRelStatus = NULL;


//      int     tier = 4;
//      char * transientVolRootDir = NULL;
		int     mach_type = SS_UNIX_MACHINE;
		//int   mach_type = SS_WNT_MACHINE;
		char *pathname =NULL;
		char *relative_path = NULL;
		int     date_offset =0;
		//IMF_file_data_p_t *   file_data = NULLTAG;
		//char *        osfilename= NULL;
		char *ownername=NULL;
		char *reference_nameDS=NULL;
		reference_nameDS=(char *) MEM_alloc(10);
		char *Exfile=NULL;
		Exfile=(char *) MEM_alloc(1000);

        depth ++;

        ItemJT =(char *) MEM_alloc(100);
        ItemCad =(char *) MEM_alloc(100);
        pathname =(char *) MEM_alloc(300);

        ifail = BOM_line_ask_attribute_string (bom_line_tag, name_attribute, &name);
        CHECK_FAIL;
        word = strtok(name, "/");

        ifail = BOM_line_ask_attribute_string (bom_line_tag, seqno_attribute, &sequence_no);
        CHECK_FAIL;

        ifail = BOM_line_look_up_attribute ( ( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
        ifail = BOM_line_ask_attribute_tag(bom_line_tag, iChildItemTag, &t_ChildItemRev);
        CHECK_FAIL;


        attrs[0] ="item_id";
        values[0] = (char *)word;
        ifail = ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found);
        CHECK_FAIL;

        item = tags_found[0];
        ifail = ITEM_ask_latest_rev(item,&reva);
        CHECK_FAIL;
        if( AOM_ask_value_string(reva,"item_id",&Item_id_par)!=ITK_ok);
        if( AOM_ask_value_string(reva,"item_revision_id",&Item_LatestRevSeq)!=ITK_ok);
        if( AOM_ask_value_string(reva,"t5_PartType",&sPartType)!=ITK_ok);
		if( AOM_ask_value_int(reva,"bl_is_occ_suppressed",&IsSuppres)!=ITK_ok);
        if( AOM_UIF_ask_value(reva,"release_status_list",&sRelStatus)!=ITK_ok);
        if( AOM_ask_value_int(bom_line_tag,"bl_level_starting_0",&level)!=ITK_ok);
		if( AOM_ask_value_string(bom_line_tag,"bl_plmxml_abs_xform",&tmatrix)!=ITK_ok);
		//if( AOM_ask_value_int(bom_line_tag,"bl_is_occ_suppressed",&IsSuppres1)!=ITK_ok);
		//if( AOM_ask_value_int(bom_line_tag,"bl_is_occ_suppressed",&IsSuppres3)!=ITK_ok);
        //if( AOM_ask_value_int(bom_line_tag,"bl_plmxml_abs_xform",&matrix)!=ITK_ok);

        ItemRev=strtok(Item_LatestRevSeq,";");
        ItemSeq=strtok(NULL,";");

        tc_strcpy(ItemCad,"");

        ifail = GRM_find_relation_type("IMAN_specification",&relation_type2);
        CHECK_FAIL;
        if(relation_type2 != NULLTAG)
        {
			ifail = GRM_list_secondary_objects_only(reva, relation_type2, &cnt2, &attachments2);
			CHECK_FAIL;
                //printf("\t attches(IMAN_Rendering)= %d  ",cnt); fflush(stdout);

                if (cnt2 > 0)
                {
                   for(j=0;j<cnt2;j++)
                     {
                        dataset = attachments2[j];

                        ifail = AOM_ask_value_string(dataset,"object_type",&DsetObjType);

                        printf("\t DsetObjType [%s]\n",DsetObjType); fflush(stdout);

                        tc_strcpy(pathname," ");
                        tc_strcpy(reference_nameDS," ");

                        if ((tc_strcmp(DsetObjType,"CMI2Product")==0) || (tc_strcmp(DsetObjType,"CMI2CacheCgr")==0) || (tc_strcmp(DsetObjType,"CMI2Part")==0) || (tc_strcmp(DsetObjType,"CMI2Drawing")==0) || (tc_strcmp(DsetObjType,"CMI2AuxPart")==0))// || (tc_strcmp(DsetObjType,"ProAsm")==0) || (tc_strcmp(DsetObjType,"ProPrt")==0))
                        {
                                if(AE_ask_dataset_ref_count(dataset,&referencenumberfound));
                                if (referencenumberfound > 0)
                                {
                                        for(k=0;k<referencenumberfound;k++)
                                        {
                                                ifail = AE_find_dataset_named_ref(dataset,k,refname,&reftype,&refobject);
                                                CHECK_FAIL;

                                                if( AOM_ask_value_string(reva,"owning_user",&ownername)!=ITK_ok);
                                                printf("\t ownername [%s]\n",ownername); fflush(stdout);

                                                ifail =  IMF_ask_original_file_name(refobject,orig_name);
                                                printf("\t orig_name: in Spec [%s]\n",orig_name); fflush(stdout);

                                                //ifail =  IMF_get_transient_volume_root_dir(tier,&transientVolRootDir);
                                                //printf("\t transientVolRootDir [%s]\n",transientVolRootDir); fflush(stdout);

                                                ifail = (IMF_ask_file_pathname2(refobject,mach_type,&pathname));
                                                printf("\t pathname [%s]\n",pathname); fflush(stdout);

                                                //if ((tc_strstr(orig_name,".asm.") != NULL) || (tc_strstr(orig_name,".prt.") != NULL))
                                                //{
												tc_strcpy(ItemCad,orig_name);

												if(refobject!= NULLTAG)
													{
														strcpy(Exfile," ");
														//strcpy(Exfile,"/user/ukprod/Adi/CADFiles/");
														strcpy(Exfile,outputfile);
														strcat(Exfile,orig_name);

														if(tc_strcmp(DsetObjType,"CMI2Part")==0)
														{
														printf("\t Inside CMI2Part\n"); fflush(stdout);
														strcpy(reference_nameDS,"CATPart");
														printf("\t Inside %s\n",reference_nameDS); fflush(stdout);
														}
														else if(tc_strcmp(DsetObjType,"CMI2Product")==0)
														{
														printf("\t Inside CMI2Product\n"); fflush(stdout);
														strcpy(reference_nameDS,"CATProduct");
														printf("\t Inside %s\n",reference_nameDS); fflush(stdout);
														}
														else if(tc_strcmp(DsetObjType,"CMI2CacheCgr")==0)
														{
														printf("\t Inside CMI2CacheCgr\n"); fflush(stdout);
														strcpy(reference_nameDS,"cgr");
														printf("\t Inside %s\n",reference_nameDS); fflush(stdout);
														}
														else if(tc_strcmp(DsetObjType,"CMI2Drawing")==0)
														{
														printf("\t Inside CMI2Drawing\n"); fflush(stdout);
														strcpy(reference_nameDS,"CATDrawing");
														printf("\t Inside %s\n",reference_nameDS); fflush(stdout);
														}
														else if(tc_strcmp(DsetObjType,"CMI2AuxPart")==0)
														{
														printf("\t Inside CMI2AuxPart\n"); fflush(stdout);
														strcpy(reference_nameDS,"CATPart");
														printf("\t Inside %s\n",reference_nameDS); fflush(stdout);
														}
														AE_export_named_ref(dataset,reference_nameDS,Exfile);
													}
                                                //}
                                                //else
                                                //{
                                                        //tc_strcpy(ItemCad,"NoProeCadNamedRef");
                                                        //continue;
                                                //}
                                        }

									}
									else
									{
											tc_strcpy(ItemCad,"NoCadNamedRef");
											tc_strcpy(pathname,"NoDsetsoNoNamedRef");
									}
								}
								else
								{
										tc_strcpy(ItemCad,"NoDset");
										tc_strcpy(pathname,"NoDsetsoNoNamedRef");
								}
                        }
                }
                else
                {
                        tc_strcpy(ItemCad,"NoCadAttached");
                        tc_strcpy(pathname,"NoDsetsoNoNamedRef");
                }
        }
        else
        {
                tc_strcpy(ItemCad,"NoCadAttached");
                tc_strcpy(pathname,"NoDsetsoNoNamedRef");
        }



//      printf("%s^%s^%s^%s^%d^%s^%s^\n",Item_id_par,ItemRev,ItemSeq,sRelStatus,level,ItemCad); fflush(stdout);
//      fprintf(fptr,"%s^%s^%s^%s^%d^%s^%s^\n",Item_id_par,ItemRev,ItemSeq,sRelStatus,level,ItemCad); fflush(fptr);

	

	printf("%d^%s^%s^%s^%s^%s^%s^%d^%s^\n",level,Item_id_par,ItemRev,ItemSeq,sPartType,ItemCad,sRelStatus,IsSuppres,tmatrix); fflush(stdout);

	fprintf(fptr,"%d^%s^%s^%s^%s^%s^%s^%d^%s^\n",level,Item_id_par,ItemRev,sPartType,ItemSeq,ItemCad,sRelStatus,IsSuppres,tmatrix); fflush(fptr);

	tc_strcpy(ItemCad," ");
	tc_strcpy(sRelStatus," ");
	tc_strcpy(ItemSeq," ");
	tc_strcpy(ItemRev," ");
	tc_strcpy(Item_id_par," ");
	//tc_strcpy(pathname," ");
	//tc_strcpy(ownername," ");
	tc_strcpy(IsSuppres," ");
	MEM_free (attachments); 

	fclose(fptr);


        //for (i = 0; i < depth; i++)
        //      printf ("  "); fflush(stdout);

        ifail = BOM_line_ask_child_lines (bom_line_tag, &n, &children);
        CHECK_FAIL;
        for (i = 0; i < n; i++)
        {
                ExpandMultiLevelBom (children[i], depth,outputfile,fptr);
        }

        MEM_free (children);
        MEM_free (name);
        MEM_free (sequence_no);
        return status;
}
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
