/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
* Author		  : Dayanand Amdapure
* Created on	  : Nov 23, 2018
*  Module		 :   TCUA EPA Uploader
*  Code			 :   EPACreate.c
*
*
* Modification History :
* S.No    Date			CR No     Modified By            Modification Notes
***************************************************************************/

#include <tccore/aom.h>
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
#include <stdlib.h>
#include <ae/dataset.h>
#include <assert.h>
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
int stringsize =0;
/**
 * Remove leading whitespace characters from string
 */
void trimLeading(char * str)
{
    int index, i, j;

    index = 0;

    /* Find last index of whitespace character */
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }


    if(index != 0)
    {
        /* Shit all trailing characters to its left */
        i = 0;
        while(str[i + index] != '\0')
        {
            str[i] = str[i + index];
            i++;
        }
        str[i] = '\0'; // Make sure that string is NULL terminated
    }
}
char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    printf("  last_comma [%s]",last_comma);
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);


        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
			stringsize = stringsize +1;
    		printf("stringsize [%d]",stringsize);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    printf("  stringsize1 [%d]",stringsize);

    return result;
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

static int	validate_date ( char *date , logical *date_is_valid , date_t *the_dt )
{
    int      retcode    = ITK_ok;     /* Function return code */
    date_t   dt         = NULLDATE;   /* Date structure */
    int      month      = 0;          /* Month */
    int      day        = 0;          /* Day */
    int      year       = 0;          /* Year */
    int      hour       = 0;          /* Hour */
    int      minute     = 0;          /* Minutes */
    int      second     = 0;          /* Seconds */
	int          max_char_size = 80;
    char *    Correct_date = (char *)MEM_alloc(max_char_size * sizeof(char));

	*date_is_valid = TRUE;

    /* Converts a date_t structure into the format specified  */
//    retcode = DATE_string_to_date ( date , "%d-%b-%Y %H:%M:%S" , &month , &day , &year , &hour , &minute , &second);				// working for " 21-August-2015" / ""
//    retcode = DATE_string_to_date ( date , "%m-%d-%y-%H:%M:%S" , &month , &day , &year , &hour , &minute , &second);				// working for  "07-21-2015-09:05:58"
     if(strlen (date)>10)
	     {
			Correct_date=subString(date,0,20);
			printf("  Correct_date [%s]",Correct_date);
			retcode = DATE_string_to_date ( Correct_date , "%y/%m/%d-%H:%M:%S:" , &month , &day , &year , &hour , &minute , &second);
		 }
		 else
		{
			strcpy(Correct_date,date);
			strcat(Correct_date,"-00:00:00:");
			printf("  Correct_date [%s]",Correct_date);
			retcode = DATE_string_to_date ( Correct_date , "%y/%m/%d-%H:%M:%S:" , &month , &day , &year , &hour , &minute , &second);
		}
		printf("  month [%d]",month);
		printf("  day [%d]",day);
		printf("  year [%d]",year);
		printf("  hour [%d]",hour);
		printf("  minute [%d]",minute);
		printf("  second [%d]",second);

	if ( retcode != ITK_ok )
    {
        *date_is_valid = FALSE;
	    printf("  Date is not valid [%d]",month);
    }
    else
    {
        dt.month = month;
        dt.day   = day;
        dt.year  = year;
        dt.hour  = hour;
        dt.minute= minute;
        dt.second= second;

        *the_dt = dt;
	    printf("  Date is valid [%d]",month);

    }

	printf("  ---return from validate_date.");

    return retcode;
}
char * trim_space(char *str) {
    char *end;
    /* skip leading whitespace */
    while (isspace(*str)) {
        str = str + 1;
    }
    /* remove trailing whitespace */
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end = end - 1;
    }
    /* write null character */
    *(end+1) = '\0';
    return str;
}

extern int ITK_user_main (int argc, char ** argv )
{

    int status;
  	int          n_tags_found_epa= 0;
  	int          n_tags_found_dml= 0;
  	int          n_tags_found_task= 0;
	tag_t*   tags_found_epa = NULL;
	tag_t*   tags_found_dml = NULL;
	tag_t*   tags_found_task = NULL;
	tag_t	queryTag	= NULLTAG;
	int n_entries = 2;
	int resultCount=0;
	tag_t *rev=NULLTAG;
	int						result											= 0;
	tag_t relation_type, relation,relation_epatask_type,relation_epatask;
	tag_t epa_tag, dml_tag,epa_task_tag;
	tag_t *epa_rev_tag, *dml_rev_tag,*epa_task_rev_tag;
	tag_t Fndrelation = NULLTAG;
	tag_t Fndrelationtaskpart = NULLTAG;
	tag_t part_tag = NULLTAG;
	tag_t*   EpaTaskTag = NULL;
	tag_t*   EpaTaskTagRev = NULL;

	char *  	EPAName=NULL;
	char *  	EPAPlant=NULL;
	char *  	TypeName=NULL;
	char *  	DMLName=NULL;
	char *  	EPATaskName=NULL;
	char *  	EPAPartNumber=NULL;
	char *  	PartNumber=NULL;
	char *  	PartRevision=NULL;
	char *  	PartSequence=NULL;
	char *  	itemRevSeq=NULL;
	FILE*      fp=NULL;
	FILE*      fpout=NULL;
	char *     inputfile=NULL;
	char*       inputline=NULL;
	char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
	char **values = (char **) MEM_alloc(1 * sizeof(char *));
	char **attrsd = (char **) MEM_alloc(1 * sizeof(char *));
	char **valuesd = (char **) MEM_alloc(1 * sizeof(char *));
	char **attrst = (char **) MEM_alloc(1 * sizeof(char *));
	char **valuest = (char **) MEM_alloc(1 * sizeof(char *));
	char **qry_values = (char **) MEM_alloc(10 * sizeof(char *));

	char ***qry_entries1 = (char ***) MEM_alloc(10 * sizeof(char *));
	char ***qry_values1 = (char ***) MEM_alloc(10 * sizeof(char *));
	char *qry_entries[2] = {"Item ID","Revision"};
	int          max_char_size = 80;
	char *    EPATaskNameNew = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *    outputfile = (char *)MEM_alloc(max_char_size * sizeof(char));


	inputfile = ITK_ask_cli_argument("-i=");

    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	ITK_CALL(ITK_auto_login( ));
    //ITK_CALL(ITK_init_module("APLloader","abc123","APLCAR"));
    ITK_CALL(ITK_set_journalling( TRUE ));
	printf("\n Auto login .......");fflush(stdout);
    strcpy(outputfile,"/home/cmitest/EPAMigration/EPADMLTaskToPartException.log");

	fp=fopen(inputfile,"r");
	fpout=fopen(outputfile,"w+");
	if(fpout==NULL)
	{
	 printf("\n%s:file is not created...!!!!\n",outputfile);fflush(stdout);
	}
	fflush(fpout);


	if(fp!=NULL)
	{
		inputline=(char *) MEM_alloc(5000);
		while(fgets(inputline,5000,fp)!=NULL)
		{

			fputs(inputline,stdout);
    		printf("inputline is --->%s\n",inputline);
			TypeName=strtok(inputline,"^");  //1
			trimLeading(TypeName);
			printf("TypeName is --->%s\n",TypeName);
			EPAName=strtok(NULL,"^");  //2
			printf("EPAName is --->%s\n",EPAName);

             // Relation btween EPA and DML is not defined, once defined un-commnet the code and run

			// Start for EPA and DML relationship
			if(strcmp(TypeName,"EpaDml")==0)
			{
			attrs[0] ="item_id";
			values[0] = (char *)EPAName;
			//Querying with item id
			ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found_epa, &tags_found_epa));
			printf("n_tags_found_epa  --->[%d]\n",n_tags_found_epa);
			DMLName=strtok(NULL,"^");  //3
			printf("DMLName is --->%s\n",DMLName);
			attrsd[0] ="item_id";
			valuesd[0] = (char *)DMLName;
			ITK_CALL(ITEM_find_items_by_key_attributes(1,attrsd, valuesd, &n_tags_found_dml, &tags_found_dml));
			printf("n_tags_found_dml  --->[%d]\n",n_tags_found_dml);

						// If both the tags found EPA and DML
						if(n_tags_found_epa > 0 && n_tags_found_dml>0)
						{
							epa_tag = tags_found_epa[0];
							ITK_CALL(ITEM_ask_latest_rev(epa_tag,&epa_rev_tag));
							dml_tag = tags_found_dml[0];
							ITK_CALL(ITEM_ask_latest_rev(dml_tag,&dml_rev_tag));

							result = GRM_find_relation_type("NotDefined", &relation_type);
							printf("\n GRM_find_relation_type ---->%d",result);fflush(stdout);
							result = GRM_find_relation( epa_rev_tag, dml_rev_tag, relation_type ,&Fndrelation);
							printf("\n\t Fndrelation ---->%d",result); fflush(stdout);
							if(Fndrelation)
							{
								printf("\n\t Relation Already Exist.\n" );fflush(stdout);
							}
							else
							{
							result = GRM_create_relation(epa_rev_tag, dml_rev_tag, relation_type, NULL, &relation);
							printf("\n GRM_create_relation ---->%d",result);fflush(stdout);
							result = GRM_save_relation(relation);
							printf("\n GRM_save_relation ---->%d",result);fflush(stdout);
							}
						}
						else
						{
						printf("\n No DML found into sysytem ---->%s",DMLName);fflush(stdout);
						fprintf(fpout,"DML Not Found in System so not created the EPATask[%s] to DML[%s] Relationship \n",EPATaskName,DMLName);fflush(stdout);
						}
			}  // End for EPA and DML relationship


			//else  // Start for EPA Task  and Parts relationship
			if(strcmp(TypeName,"EpaTask")==0)
			{

						EPATaskName=strtok(NULL,"^");  //3
						EPAPartNumber=strtok(NULL,"^");  //4
						printf("EPATaskName is --->%s\n",EPATaskName);
						attrs[0] ="item_id";
						values[0] = (char *)EPAName;
						//Querying with item id
						ITK_CALL(ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found_epa, &tags_found_epa));
						printf("n_tags_found_epa  --->[%d]\n",n_tags_found_epa);
						if(n_tags_found_epa > 0)
						{
						epa_tag = tags_found_epa[0];
						ITK_CALL(ITEM_ask_latest_rev(epa_tag,&epa_rev_tag));
						ITK_CALL(AOM_ask_value_string(epa_rev_tag,"t5_EpaPlantA",&EPAPlant));
    					printf("EPAPlant is --->%s\n",EPAPlant);
						}

						if(strcmp(EPAPlant,"CAR")==0)
						{
							strcpy(EPATaskNameNew,EPATaskName);
							strcat(EPATaskNameNew,"C");
							printf("EPATaskNameNew is --->%s\n",EPATaskNameNew);
						}
						attrst[0] ="item_id";
						valuest[0] = (char *)EPATaskNameNew;
						//Querying with item id
						ITK_CALL(ITEM_find_items_by_key_attributes(1,attrst, valuest, &n_tags_found_task, &tags_found_task));
						printf("n_tags_found_task  --->[%d]\n",n_tags_found_task);
                        if(n_tags_found_task==0)
				        {
							ITK_CALL(ITEM_create_item(EPATaskNameNew,EPATaskNameNew,"T5_EPATask","A",&EpaTaskTag,&EpaTaskTagRev));
							AOM_save(EpaTaskTag);
							AOM_save(EpaTaskTagRev);
							 if(EpaTaskTagRev)
							{
					         	printf("EPA Task Created [%s]\n",EPATaskName);
								n_tags_found_task = 1;
								epa_task_rev_tag = EpaTaskTagRev;
							 }
						}
						else
							 {
								epa_task_tag = tags_found_task[0];
								ITK_CALL(ITEM_ask_latest_rev(epa_task_tag,&epa_task_rev_tag));
							}

						 // If both the tags found EPA and EPA Task
						if(n_tags_found_epa > 0 && n_tags_found_task>0)
						{

							result = GRM_find_relation_type("T5_DMLTaskRelation", &relation_type);
							printf("\n GRM_find_relation_type ---->%d",result);fflush(stdout);
							result = GRM_find_relation( epa_rev_tag, epa_task_rev_tag, relation_type ,&Fndrelation);
							printf("\n\t Fndrelation ---->%d",result); fflush(stdout);
							if(Fndrelation)
							{
								printf("\n\t Relation Already Exist.\n" );fflush(stdout);
							}
							else
							{
							result = GRM_create_relation(epa_rev_tag, epa_task_rev_tag, relation_type, NULL, &relation);
							printf("\n GRM_create_relation ---->%d",result);fflush(stdout);
							result = GRM_save_relation(relation);
							printf("\n GRM_save_relation ---->%d",result);fflush(stdout);
							}


									printf("EPAPartNumber is --->%s\n",EPAPartNumber);
									PartNumber=strtok(EPAPartNumber,",");  //4.1
									PartRevision=strtok(NULL,",");  //4.2
									PartSequence=strtok(NULL,",");  //4.3
									trim_space(PartNumber);
									trim_space(PartRevision);
									trim_space(PartSequence);
									if(QRY_find("Unique RevSeq", &queryTag));
									printf("\n After IFERR_REPORT : QRY_find \n");fflush(stdout);
									if (queryTag)
									{
										printf("2.Found Query \n");fflush(stdout);
									}
									else
									{
										printf("Not Found Query");fflush(stdout);
									}
									printf(" \n PartNumber :%s:", PartNumber); fflush(stdout);
									printf(" \n PartRevision :%s:", PartRevision); fflush(stdout);
									printf(" \n PartSequence :%s:", PartSequence); fflush(stdout);
									itemRevSeq = NULL;
									itemRevSeq=(char *) MEM_alloc(50);
									strcpy(itemRevSeq,PartRevision);
									strcat(itemRevSeq,"*");
									strcat(itemRevSeq,PartSequence);
									printf("\nitemRevSeq concated is --->%s\n\n",itemRevSeq);
									qry_values[0] = PartNumber ;
									qry_values[1] = itemRevSeq ;
									if(QRY_execute(queryTag, n_entries, qry_entries, qry_values, &resultCount, &rev));
									printf(" \n resultCount :%d:", resultCount); fflush(stdout);
							        if(resultCount>0)
									{
										part_tag = rev[0];

										result = GRM_find_relation_type("CMHasSolutionItem", &relation_epatask_type);
										printf("\n GRM_find_relation_type ---->%d",result);fflush(stdout);
										result = GRM_find_relation( epa_task_rev_tag, part_tag, relation_epatask_type ,&Fndrelationtaskpart);
										printf("\n\t Fndrelationtaskpart ---->%d",result); fflush(stdout);
										if(Fndrelationtaskpart)
										{
											printf("\n\t Relation Already Exist.\n" );fflush(stdout);
										}
										else
										{
										result = GRM_create_relation(epa_task_rev_tag, part_tag, relation_epatask_type, NULL, &relation_epatask);
										printf("\n GRM_create_relation ---->%d",result);fflush(stdout);
										result = GRM_save_relation(relation_epatask);
										printf("\n GRM_save_relation ---->%d",result);fflush(stdout);
										}
									}
									else
										{
										printf("\n No part found into sysytem ---->%s:%s",EPATaskName,PartNumber);fflush(stdout);
										//fprintf(fpout,"Part Number [%s] \n",PartNumber);fflush(stdout);
										fprintf(fpout,"Part Number Not Found in System so not created the EPATask[%s] to Part[%s;%s] Relationship \n",EPATaskName,PartNumber,itemRevSeq);fflush(stdout);
										}

						}
                     fflush(fpout);

			}   // Start for EPA Task  and Parts relationship

		}
	}

	fclose(fp);
	fclose(fpout);

	ITK_CALL(POM_logout(false));
	return status;

	ITK_CALL(POM_logout(false));
	return status;
}
