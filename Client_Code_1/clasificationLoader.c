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
	FILE	*file			=NULL;
	FILE	*fileNotLoaded			=NULL;
	char *	str				=NULL;
	char *	inputline				=NULL;	
	char *	inputClassId				=NULL;	
	int count =0;
	tag_t* tags ;

	//describe class
		int  	theCount =0;
		int * 	theIds;
		char ** 	theNames;
		char ** 	theShortNames;
		char ** 	theAnnotations;
		int * 	theArraySize;
		int * 	theFormat;
		char ** 	theUnit;
		char ** 	theMinValues;
		char ** 	theMaxValues;
		char ** 	theDefaultValues;
		char ** 	theDescriptions;
		int * 	theOptions1 ;
		tag_t * 	theICOTag;
	//describe class end

	char * 	PartNumber =NULL;
	//char * 	inputClassId =NULL;
	char * 	LovStr =NULL;
	char **InputValuesArr1=NULL;
	int i=0;
	
	 str=malloc(500);
	 inputline=malloc(500);
	// inputClassId=malloc(50);
	 int c =0;
	 int length =0;


	 	

	ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	ITK_CALL(ITK_auto_login( ));
	ITK_CALL(ITK_set_journalling( TRUE ));

//	if(argv[1]!=NULL)
//	{
//	 strcpy(inputClassId,argv[1]);
//	 printf("\n inputClassId  :%s\n",inputClassId);fflush(stdout);
//	}
//	else
//	{
//		printf("\n inputClassId is null\n");fflush(stdout);
//	}
	


//		ITK_CALL(ICS_class_describe_attributes(inputClassId,
//										&theCount,
//										&theIds,
//										&theNames,
//										&theShortNames,
//										&theAnnotations,
//										&theArraySize,
//										&theFormat,
//										&theUnit,
//										&theMinValues,
//										&theMaxValues,
//										&theDefaultValues,
//										&theDescriptions,
//										&theOptions1)	)
//										
//				printf("\n After ICS_class_describe_attributes. theCount =%d.\n", theCount);fflush(stdout);
//				InputValuesArr1=(char**) malloc(theCount * sizeof(char*) );
//				for(i=0;i<theCount;i++)
//					{
//						InputValuesArr1[i]= malloc(90 * sizeof(char) );
//					}
				



	file=fopen("upload.txt","r");
	fileNotLoaded=fopen("NotLoaded.txt","w");
	if ( file != NULL )
	{
		while ( fgets ( str,500, file ) != NULL ) /* read a line */
		{
			//nlsStrTrimTrailWhiteSpace(str);
			
			strcpy(inputline,str);
			printf("\n Line from file  :%s\n",inputline);fflush(stdout);

		
//				for(i=0;i<theCount;i++)
//					{
//						InputValuesArr1[i]=NULL;
//					}
					inputClassId=strtok(inputline,"$");
					printf("\n inputClassId .. %s",inputClassId);
					ITK_CALL(ICS_class_describe_attributes(inputClassId,
										&theCount,
										&theIds,
										&theNames,
										&theShortNames,
										&theAnnotations,
										&theArraySize,
										&theFormat,
										&theUnit,
										&theMinValues,
										&theMaxValues,
										&theDefaultValues,
										&theDescriptions,
										&theOptions1)	)
										
				printf("\n After ICS_class_describe_attributes. theCount =%d.\n", theCount);fflush(stdout);
				InputValuesArr1=(char**) malloc(theCount * sizeof(char*) );
				for(i=0;i<theCount;i++)
					{
						InputValuesArr1[i]= malloc(90 * sizeof(char) );
					}
					PartNumber=strtok(NULL,"$");
					printf("\n PartNumber .. %s",PartNumber);
					LovStr=strtok(NULL,"$");	
					c=0;

				while( (LovStr != NULL ) && (c< theCount ))
				{
					length = strlen(LovStr);
					printf("\n LovStr .. %s",LovStr);
					//printf("\n length .. %d",length);
					//InputValuesArr1[c]= malloc(length * sizeof(char) );
					if( tc_strcmp(LovStr," ")==0)
					{
					//InputValuesArr1[c]=NULL;
					strcpy(InputValuesArr1[c]," ");
					}
					else
						strcpy(InputValuesArr1[c],LovStr);
					c++;
					LovStr = strtok( NULL, "$" );
				}

				for(i=0;i<theCount;i++)
				{
				 printf("\n InputIdsArr .. %d    ValuesArr : %s",theIds[i],InputValuesArr1[i]);
				}
						
			printf("\n");

				ITK_CALL(ITEM_find(PartNumber,&count,&tags));
					if ( count > 0 )
					{
						printf("\nPartNumber Object found count = %d",count);
						ICS_ico_create(PartNumber,tags[0],inputClassId,theCount,theIds,InputValuesArr1,theICOTag);
						printf("\n\n Done \n");
					}
					else
					{
						printf("\nNot found  = %s\n",PartNumber);
						fprintf(fileNotLoaded,"%s",str);fflush(stdout);
						
					}

					for(i=0;i<theCount;i++)
					{
						free(InputValuesArr1[i]);
					}
					free(InputValuesArr1);
					InputValuesArr1=NULL;

		}
	}

	fclose(file);
	fclose(fileNotLoaded);




	ITK_CALL(POM_logout(false));
	return status;
}