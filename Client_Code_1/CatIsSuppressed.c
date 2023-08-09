
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
#include <time.h>

#include <tcinit/tcinit.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <tccore/workspaceobject.h>
#include <ps/ps.h>
#include <bom/bom.h>
#include <bom/bom_tokens.h>
#include <pom/pom/pom.h>
#include <tccore/aom.h>
#include <cfm/cfm_item.h>
#include <cfm/cfm_tokens.h>
#include <ps/ps_errors.h>
#include <ps/vrule.h>
#include <tccore/grm.h>
#include <tc/emh_errors.h>
#include <tc/tc_errors.h>
#include <ss/ss_errors.h>
#include <fclasses/tc_string.h>
#include <property/prop.h>
#include <error.h>

#include <dispatcher/dispatcher_itk.h>
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
#include <sys/dir.h>
#include <sys/stat.h>
#include <fclasses/tc_string.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <sa/tcfile.h>
#include <tcinit/tcinit.h>
#include <tccore/tctype.h>
#include <res/reservation.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/iman.h>
#include <tccore/imantype.h>
#include <sa/imanfile.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <itk/mem.h>
#include <sa/user.h>
#include <unistd.h>
#include <sys/types.h>
#define Debug TRUE
#define ITK_CALL(X)																		\
        if(Debug)																		\
        {                                                                               \
                printf(#X);																\
        }                                                                               \
        fflush(NULL);																	\
        status=X;																		\
        if (status != ITK_ok )															\
        {                                                                               \
                int                     index		= 0;								\
                int                     n_ifails	= 0;								\
                const int*              severities	= 0;								\
                const int*              ifails		= 0;								\
                const char**			texts		= NULL;								\
                                                                                        \
                EMH_ask_errors( &n_ifails, &severities, &ifails, &texts);               \
                printf("\t%3d error(s)\n", n_ifails);                                   \
                for( index=0; index<n_ifails; index++)                                  \
                {                                                                       \
                        printf("\tError #%d, %s\n", ifails[index], texts[index]);       \
                }                                                                       \
                return status;                                                          \
        }                                                                               \
        else																			\
        {                                                                               \
                if(Debug)																\
                printf("\tSUCCESS\n");													\
        }                                                                               \

#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}

//char* subString (char* mainStringf ,int fromCharf,int toCharf);
//
//char* subString (char* mainStringf ,int fromCharf,int toCharf)
//{
//	int i;
//	char *retStringf;
//	retStringf = (char*) malloc(3);
//	for(i=0; i < toCharf; i++ )
//    *(retStringf+i) = *(mainStringf+i+fromCharf);o
//	*(retStringf+i) = '\0';
//	return retStringf;
//}
//
#define CHECK_FAIL if (ifail != 0) { printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}


static int PrintErrorStack( void )
{
    int iNumErrs = 0;
    const int *pSevLst;
    const int *pErrCdeLst;
    const char **pMsgLst;
    register int i = 0;

    EMH_ask_errors( &iNumErrs, &pSevLst, &pErrCdeLst, &pMsgLst ); 

    for ( i = 0; i < iNumErrs; i++ )
    {
		fprintf( stderr, "Error(PrintErrorStack): \n");
        fprintf( stderr, "\t%6d: %s\n", pErrCdeLst[i], pMsgLst[i] );
    }
    return ITK_ok;
}


extern int ITK_user_main (int argc, char ** argv )
{
	int							ifail;
	int							status;
	int							count;
	int							i;
	logical						IsSuppres;
	char *partnumber		=	NULL;
	tag_t LatestRev			=	NULLTAG;
	tag_t item_tag			=	null_tag;
	tag_t top_line;
	tag_t*children;
	tag_t window;
	char *inputfile			=	NULL;
	char *CadFile			=	NULL;
	char *suppressed		=	NULL;
	char *inputline			=	NULL;
	char *Item_id_par		=	NULL;
	FILE* fp=NULL;
	tag_t *tags_found		= NULL;
	int n_tags_found		= 0;
	int n_rev				= 0;
	char **attrs			= (char **) MEM_alloc(1 * sizeof(char *));
	char **values			= (char **) MEM_alloc(1 * sizeof(char *));

	(void)argc;
	(void)argv;

	partnumber = ITK_ask_cli_argument("-p=");
	inputfile = ITK_ask_cli_argument("-i=");

	if( ITK_init_module("loader" ,"loader7","dba")!=ITK_ok) ;
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n LOGGING IN .......");fflush(stdout);

	if ( partnumber )
	{
		attrs[0] ="item_id";
		values[0] = (char *) partnumber;

		ifail = ITEM_find_item(partnumber,&item_tag);
		CHECK_FAIL;

		ITK_CALL(ITEM_ask_latest_rev(item_tag,&LatestRev));

		//MEM_free(tags_found);
	}
	else
	{
		printf ("Pls enter input part ?? \n"); fflush(stdout);
		exit (0);
	}

	fp=fopen(inputfile,"r");
	if(fp!=NULL)
	{

		ifail = BOM_create_window (&window);
        CHECK_FAIL;

		ifail = BOM_set_window_top_line (window, null_tag, LatestRev, null_tag, &top_line);
		CHECK_FAIL;

		count=0;
		status = BOM_line_ask_child_lines (top_line, &count, &children);

		inputline=(char *) MEM_alloc(500);
		while(fgets(inputline,500,fp)!=NULL)
		{			
			CadFile=strtok(inputline,":");
			suppressed=strtok(NULL," "); 
		
			printf("CadFile is --->%s\n",CadFile);fflush(stdout);
			printf("suppressed is --->%s\n",suppressed);fflush(stdout);
		}
			
	
		for (i=0;i<count;i++)
		  {
			//if( AOM_ask_value_string(children,"item_id",&Item_id_par)!=ITK_ok);
			//if( AOM_ask_value_string(children,"bl_line_name",&Item_id_par)!=ITK_ok);
			if( AOM_ask_value_string(children[i],"bl_item_item_id",&Item_id_par)!=ITK_ok);

			printf("\n Item_id_par is ---> %s \n",Item_id_par);fflush(stdout);

			if(strcmp(suppressed,"S")==0) //SHOW
			  {
				  AOM_set_value_logical(children[i], "bl_is_occ_suppressed", TRUE);
			  }
			else if (strcmp(suppressed,"H")==0) //HIDE
			  {
				  AOM_set_value_logical(children[i], "bl_is_occ_suppressed", FALSE);
			  }	
			  
			printf("\n after setting suppressed value on %s \n",Item_id_par);fflush(stdout);

		 }
	 	 // }

		 	  	//Saving the BOM Structurels
				BOM_save_window(window);

				//Closing the BOM Window
				BOM_close_window(window);

				//MEM_free(item_revisions_tag);
				MEM_free(children);
	  }
	  

  }
