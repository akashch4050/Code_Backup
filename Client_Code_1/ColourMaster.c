#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <res/res_itk.h>
#include <bom/bom.h> 
#include <ctype.h>
#include <tc/emh.h>
#include <tc/folder.h> 
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <itk/mem.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ps/ps.h>
#include <tccore/uom.h>
#include <user_exits/user_exits.h>
#include <rdv/arch.h>
#include <stdlib.h>
#include <string.h>
#include <textsrv/textserver.h>
#include <tccore/item_errors.h>
#include <ae/dataset.h>
#include <stdlib.h>
#include <tccore/libtccore_exports.h>
#define Debug TRUE
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

#define TCTYPE_name_size_c 100

extern int ITK_user_main (int argc, char ** argv )
{
	int status;	

	int n_tags_found = 0;
	tag_t query = NULLTAG;
	tag_t	LatestRev		=	NULLTAG;
	tag_t *cntr_objects = NULL;
	tag_t	*t_item1		=	NULLTAG;
	tag_t	latestrev	=	NULLTAG;
	int n_entries = 1;
	//int n_found = 1;
	int dd = 0;
	int org_seq_id_int=0;
	char *inputfile=NULL;
	tag_t *tags_found = NULL;
	char* inputline=NULL;
	tag_t	queryTag	= NULLTAG;
	int resultCount=0;
	tag_t item_tag = NULLTAG;

	char *Item_id = NULL;
	char *object_name = NULL;
	char *object_desc = NULL;
	char *t5_ClTatNo = NULL;
	char *t5_IntScheme = NULL;
	char *t5_Finish = NULL;
	char *t5_ValidatedBy = NULL;
	char *t5_Status = NULL;
	char *t5_PartNumber = NULL;
	char *t5_GtCode = NULL;
	char *t5_GtName = NULL;
	char *t5_Mtrl = NULL;
	char *t5_Gtarget = NULL;
	char *t5_DefaultQuantity = NULL;
	char *t5_ClId = NULL;
	char *t5_NoOfCoates = NULL;
	char *t5_DefaultUnitOfMeasure = NULL;
	char *t5_BaseRed = NULL;
	char *t5_BaseGreen = NULL;
	char *t5_BaseBlue = NULL;
	char *t5_AmbientRed = NULL;
	char *t5_AmbientGreen = NULL;
	char *t5_AmbientBlue = NULL;
	char *t5_DiffuseRed = NULL;
	char *t5_DiffuseGreen = NULL;
	char *t5_DiffuseBlue = NULL;
	char *t5_SpecularRed = NULL;
	char *t5_SpecularGreen = NULL;
	char *t5_SpecularBlue = NULL;
	char *t5_EmissiveRed = NULL;
	char *t5_EmissiveGreen = NULL;
	char *t5_EmissiveBlue = NULL;
	char *t5_Transparency = NULL;
	char *t5_Shininess = NULL;
	char *t5_DualTonePartNo = NULL;
	char *t5_ClMstrAliasStr = NULL;

	char *s;
	
	tag_t boTypeTag= NULLTAG;
	tag_t boTypeTag_r= NULLTAG;
	int n_found = 0;
	tag_t boTag= NULLTAG;
//	tag_t boRevTag= NULLTAG;
	tag_t creInputTag= NULLTAG;
	tag_t creInputTag_r= NULLTAG;
	
	char* Itemid=NULL;
	Itemid=(char *)MEM_alloc(100);
	
	tag_t* foundItemsMtag;
	tag_t rev;

	FILE *fptr;
	
	inputfile = ITK_ask_cli_argument("-i=");
	
	//ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	//printf("\n Auto login ");fflush(stdout);
	//ITK_CALL(ITK_auto_login( ));	
	
	//if( ITK_init_module("loader","loader7","dba")!=ITK_ok) ;
	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_auto_login( ));
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Line  ====%s ",inputfile );fflush(stdout);
	
	fptr=fopen(inputfile,"r");

	if(fptr!=NULL)
	{
		inputline=(char *) MEM_alloc(1000);
		while(fgets(inputline,1000,fptr)!=NULL)
		{	
			fputs(inputline,stdout);			
			Item_id=tc_strtok(inputline,"^");
			printf("\n Colour ID => %s ",Item_id );fflush(stdout);

			object_name=tc_strtok(NULL,"^");
			printf("\n Color serial => %s ",object_name );fflush(stdout);

			object_desc=tc_strtok(NULL,"^");
			printf("\n Color Description => %s ",object_desc );fflush(stdout);

			t5_ClTatNo=tc_strtok(NULL,"^");
			printf("\n t5_ClTatNo => %s ",t5_ClTatNo );fflush(stdout);

			t5_IntScheme=tc_strtok(NULL,"^");
			printf("\n t5_IntScheme => %s ",t5_IntScheme );fflush(stdout);

			t5_Finish=tc_strtok(NULL,"^");
			printf("\n t5_Finish => %s ",t5_Finish );fflush(stdout);

			t5_ValidatedBy=tc_strtok(NULL,"^");
			printf("\n t5_ValidatedBy => %s ",t5_ValidatedBy );fflush(stdout);

			t5_Status=tc_strtok(NULL,"^");
			printf("\n t5_Status => %s ",t5_Status );fflush(stdout);

			t5_PartNumber=tc_strtok(NULL,"^");
			printf("\n t5_PartNumber => %s ",t5_PartNumber );fflush(stdout);

			t5_GtCode=tc_strtok(NULL,"^");
			printf("\n t5_GtCode => %s ",t5_GtCode );fflush(stdout);

			t5_GtName=tc_strtok(NULL,"^");
			printf("\n t5_GtName => %s ",t5_GtName );fflush(stdout);

			t5_Mtrl=tc_strtok(NULL,"^");
			printf("\n Pre-Coated Paint Number => %s ",t5_Mtrl );fflush(stdout);

			t5_Gtarget=tc_strtok(NULL,"^");
			printf("\n Gloss Target => %s ",t5_Gtarget );fflush(stdout);

			t5_DefaultQuantity=tc_strtok(NULL,"^");
			printf("\n t5_DefaultQuantity => %s ",t5_DefaultQuantity );fflush(stdout);

			t5_ClId=tc_strtok(NULL,"^");
			printf("\n Qty Per Coat => %s ",t5_ClId );fflush(stdout);

			t5_NoOfCoates=tc_strtok(NULL,"^");
			printf("\n Coated => %s ",t5_NoOfCoates );fflush(stdout);

			t5_DefaultUnitOfMeasure=tc_strtok(NULL,"^");
			printf("\n Unit Of Measure => %s ",t5_DefaultUnitOfMeasure );fflush(stdout);
						
			//Base Red Color
			t5_BaseRed=tc_strtok(NULL,"^");
			printf("\n t5_BaseRed => %s ",t5_BaseRed );fflush(stdout);

			t5_BaseGreen=tc_strtok(NULL,"^");
			printf("\n t5_BaseGreen => %s ",t5_BaseGreen );fflush(stdout);

			t5_BaseBlue=tc_strtok(NULL,"^");
			printf("\n t5_BaseBlue => %s ",t5_BaseBlue );fflush(stdout);
			
			//Ambient Red Color
			t5_AmbientRed=tc_strtok(NULL,"^");
			printf("\n t5_AmbientRed => %s ",t5_AmbientRed );fflush(stdout);

			t5_AmbientGreen=tc_strtok(NULL,"^");
			printf("\n t5_AmbientGreen => %s ",t5_AmbientGreen );fflush(stdout);

			t5_AmbientBlue=tc_strtok(NULL,"^");
			printf("\n t5_AmbientBlue => %s ",t5_AmbientBlue );fflush(stdout);

			//Diffuse Red Color
			t5_DiffuseRed=tc_strtok(NULL,"^");
			printf("\n t5_DiffuseRed => %s ",t5_DiffuseRed );fflush(stdout);

			t5_DiffuseGreen=tc_strtok(NULL,"^");
			printf("\n t5_DiffuseGreen => %s ",t5_DiffuseGreen );fflush(stdout);

			t5_DiffuseBlue=tc_strtok(NULL,"^");
			printf("\n t5_DiffuseBlue => %s ",t5_DiffuseBlue );fflush(stdout);

			//Specular Red Color
			t5_SpecularRed=tc_strtok(NULL,"^");
			printf("\n t5_SpecularRed => %s ",t5_SpecularRed );fflush(stdout);

			t5_SpecularGreen=tc_strtok(NULL,"^");
			printf("\n t5_SpecularGreen => %s ",t5_SpecularGreen );fflush(stdout);

			t5_SpecularBlue=tc_strtok(NULL,"^");
			printf("\n t5_SpecularBlue => %s ",t5_SpecularBlue );fflush(stdout);
									
			//Emissive Red Color
			t5_EmissiveRed=tc_strtok(NULL,"^");
			printf("\n t5_EmissiveRed => %s ",t5_EmissiveRed );fflush(stdout);

			t5_EmissiveGreen=tc_strtok(NULL,"^");
			printf("\n t5_EmissiveGreen => %s ",t5_EmissiveGreen );fflush(stdout);

			t5_EmissiveBlue=tc_strtok(NULL,"^");
			printf("\n t5_EmissiveBlue => %s ",t5_EmissiveBlue );fflush(stdout);

			t5_Transparency=tc_strtok(NULL,"^");
			printf("\n t5_Transparency => %s ",t5_Transparency );fflush(stdout);

			t5_Shininess=tc_strtok(NULL,"^");
			printf("\n t5_Shininess => %s ",t5_Shininess );fflush(stdout);

			t5_DualTonePartNo=tc_strtok(NULL,"^");
			printf("\n t5_DualTonePartNo => %s ",t5_DualTonePartNo );fflush(stdout);

			t5_ClMstrAliasStr=tc_strtok(NULL,"^");
			printf("\n t5_ClMstrAliasStr => %s ",t5_ClMstrAliasStr );fflush(stdout);

			TCTYPE_find_type("T5_ClMas", NULL, &boTypeTag);
			TCTYPE_find_type("T5_ClMasRevision", NULL, &boTypeTag_r);

			if(boTypeTag != NULLTAG) 
			printf("\nType Tag Found. \n", n_found);fflush(stdout);

			if(boTypeTag_r != NULLTAG) 
			printf("\n Revision Type Tag Found. \n", n_found);fflush(stdout);
			
			TCTYPE_construct_create_input (boTypeTag, &creInputTag);
			if(creInputTag != NULLTAG) 
			printf("\nInput Tag Created for item id. \n", n_found);fflush(stdout);
			boTypeTag = NULLTAG;
									
			TCTYPE_construct_create_input (boTypeTag_r, &creInputTag_r);
			if(creInputTag_r != NULLTAG) 
			printf("\nInput Tag Created for item id. \n", n_found);fflush(stdout);
			boTypeTag_r = NULLTAG;

			printf("\n Setting revision attributes......\n");fflush(stdout);
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_ColourId", Item_id));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_ClTatNo", t5_ClTatNo));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_IntScheme", t5_IntScheme));
			//ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_Finish", t5_Finish));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_ValidatedBy", t5_ValidatedBy));
			//ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_Status", t5_Status));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_PartNumber", t5_PartNumber));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_GtCode", t5_GtCode));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_GtName", t5_GtName));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_Mtrl", t5_Mtrl));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_Gtarget", t5_Gtarget));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_DefaultQuantity", t5_DefaultQuantity));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_ClId", t5_ClId));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_NoOfCoates", t5_NoOfCoates));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_DefaultUnitOfMeasure", t5_DefaultUnitOfMeasure));
			ITK_CALL(AOM_set_value_int(creInputTag_r,"t5_BaseRed",atoi(t5_BaseRed)));			
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_BaseGreen", atoi(t5_BaseGreen)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_BaseBlue", atoi(t5_BaseBlue)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_AmbientRed", atoi(t5_AmbientRed)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_AmbientGreen", atoi(t5_AmbientGreen)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_AmbientBlue", atoi(t5_AmbientBlue)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_DiffuseRed", atoi(t5_DiffuseRed)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_DiffuseGreen", atoi(t5_DiffuseGreen)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_DiffuseBlue", atoi(t5_DiffuseBlue)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_SpecularRed", atoi(t5_SpecularRed)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_SpecularGreen", atoi(t5_SpecularGreen)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_SpecularBlue", atoi(t5_SpecularBlue)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_EmissiveRed", atoi(t5_EmissiveRed)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_EmissiveGreen", atoi(t5_EmissiveGreen)));
			ITK_CALL(AOM_set_value_int(creInputTag_r, "t5_EmissiveBlue", atoi(t5_EmissiveBlue)));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_Transparency", t5_Transparency));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_Shininess", t5_Shininess));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_DualTonePartNo", t5_DualTonePartNo));
			ITK_CALL(AOM_set_value_string(creInputTag_r, "t5_ClMstrAliasStr", t5_ClMstrAliasStr));			

			AOM_set_value_tag(creInputTag, "revision",creInputTag_r);
			printf("\n Color serial => %s ",object_name );fflush(stdout);
			printf("\n Color Description => %s\n\n",object_desc );fflush(stdout);
			
			tc_strcpy(Itemid,"");
			tc_strcat(Itemid,Item_id);
			tc_strcat(Itemid,"/");
			tc_strcat(Itemid,object_name);
			tc_strcat(Itemid,"/");
			tc_strcat(Itemid,t5_IntScheme);
			printf(" \n Itemid : %s\n", Itemid); fflush(stdout);
				
			ITK_CALL(AOM_set_value_string(creInputTag, "item_id", Itemid));
			ITK_CALL(AOM_set_value_string(creInputTag, "object_name", object_name));						
			ITK_CALL(AOM_set_value_string(creInputTag, "object_desc", object_desc));

			//Creating Object
			printf("\n Creating Color Master ......................... \n");fflush(stdout);
			ITK_CALL(TCTYPE_create_object(creInputTag, &boTag));
			if(boTag)
			{
				printf("\n t5CreateObject : object created.\n");
				ITK_CALL(AOM_save(boTag));
				ITK_CALL(AOM_unlock(boTag));
			}
			
			/*
			printf("\n Start Comp Code Master creation process..........\n");
			ITK_CALL(TCTYPE_find_type("T5_CompCdMst", NULL, &dml_type_tag));  
			ITK_CALL(TCTYPE_construct_create_input(dml_type_tag, &object_create_input_tag));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"object_name",CompCdMaster));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"t5_InternalSchme",t5InternalSchm));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"t5_CcodePlatform",t5CcdPlatform));
			ITK_CALL(AOM_set_value_string(object_create_input_tag,"t5_VehClassNVTable",t5VehClassNVTable));
			ITK_CALL(TCTYPE_create_object(object_create_input_tag, &new_object));
			if(new_object)
			{
				printf("\n t5CreateObject : object created.\n");
				ITK_CALL(AOM_save(new_object));
				ITK_CALL(AOM_unlock(new_object));
			}
			
			
			ITK_CALL( FL_user_update_newstuff_folder(newItemTag) );
			ITK_CALL(GRM_find_relation_type  ("T5_DMLTaskRelation",&tRelationFind));
			if (tRelationFind!=NULLTAG)
			{
				//Now Creating Relation Between Requirement and DVM Object
				GRM_find_relation(dmlrevisiontag,task_tag,tRelationFind,&tRelationExist);  
				if (tRelationExist==NULLTAG)
				{
					//###  Creating Relation between DML and Task  ###
					printf("\n Now Creating Relation Between DML and DML Task Object \n");fflush(stdout);
					ITK_CALL(GRM_create_relation(dmlrevisiontag,task_tag,tRelationFind,NULLTAG,&Rel_task));
					ITK_CALL(GRM_save_relation  (Rel_task)); 
					printf("\nRelation Created Successfully\n");fflush(stdout);		
					
				}
			}
			*/
			

		}
	}

	ITK_CALL(POM_logout(false));
	return status;
}

