#define _CRT_SECURE_NO_DEPRECATE
#define NUM_ENTRIES 1
#define TE_MAXLINELEN  128
#include <epm/epm.h>
#include <ae/dataset_msg.h>
#include <tccore/iman_msg.h>
#include <ps/ps.h>
#include <pie/pie.h>//Added by Anshul Multilevel BOM explode
#include <ps/ps_errors.h>
#include <time.h>
#include <ai/sample_err.h>
#include <tc/tc.h>
#include <tccore/grm_msg.h>
#include <tccore/workspaceobject.h>
#include <bom/bom.h>
#include <ae/dataset.h>
#include <ps/ps_errors.h>
#include <sa/sa.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fclasses/tc_string.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <sa/tcfile.h>
#include <epm/releasestatus.h>
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
#include <tccore/grm.h>
#include <tccore/item_msg.h>
#include <string.h>
#include <epm/cr_effectivity.h>
#define ITK_err 919002
#define ITK_errStore1 (EMH_USER_error_base + 5)
#define ITK_Prjerr (EMH_USER_error_base + 8)

#define CONNECT_FAIL (EMH_USER_error_base + 2)
#define CALLAPI(expr)ITKCALL(ifail = expr); if(ifail != ITK_ok) {  return ifail;}
#define ITK_errStore 91900002
int cnt = 0;
int cntFlag = 0;


#define ITK_CALL 	 \
status=X; 	 \
if (status != ITK_ok)  	 \
{	 \
int	 index = 0;	 \
int	 n_ifails = 0;	 \
const int*	 severities = 0;	 \
const int*	 ifails = 0;	 \
const char**	texts = NULL;	 \
\
EMH_ask_errors( &n_ifails, &severities, &ifails, &texts); \
printf("%3d error with #X\n", n_ifails);	 \
for( index=0; index<n_ifails; index++)	 \
{	 \
printf("\tError #%d, %s\n", ifails[index], texts[index]);	\
}	 \
return status;	 \
}	 \
;
//Start Code to Explode BOM -4
struct BomChldStrut
{
	tag_t child_objs;
}get_BomChldStrut[6000];
//End Code to Explode BOM -4
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
;
void getCurrentDateTime(char cCurrentAccessDate[20])
{

	int ch;
	time_t temp;
	struct tm *timeptr;
	char pAccessDate[20];
	char	DateS[4];
	printf("\n getCurrentDateTime calling..........\n");
	temp = time(NULL);
	tc_strcpy(pAccessDate,"");
	timeptr = (struct tm *)localtime(&temp);
	ch = strftime(pAccessDate,sizeof(pAccessDate)-1,"%d-%b-%Y %H:%M",timeptr);
	//ch = strftime(pAccessDate,sizeof(pAccessDate)-1,"%d/%m/%Y",timeptr);
	tc_strcpy(cCurrentAccessDate,pAccessDate);
	printf("\n cCurrentAccessDate:%s\n",cCurrentAccessDate);

	//sprintf(DateS,"%d",(timeptr->tm_year+1900));
	printf("Date:%d\n",(timeptr->tm_mday));
	printf("Month:%d\n",(timeptr->tm_mon)+1);
	printf("Year:%d\n",(timeptr->tm_year+1900));
	fflush(stdout);
}
int days( int m, int y )
{
       if ( m < 1 || m > 12 ) return 0;
       switch ( m ) {
       case 1: return 31;
       case 2: return 28 + ( (y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0) );
       }
   return ( m*153 + 156 ) / 5 - ( m*153 + 3 ) / 5;
}
void  getMonth( int m ,char cMonth[30])
{
      if(m==1)
	  {
		tc_strcpy(cMonth,"Jan");
	  }
	  else if(m==2)
	  {
		tc_strcpy(cMonth,"Feb");
	  }
	  else if(m==3)
	  {
		tc_strcpy(cMonth,"Mar");
	  }
	  else if(m==4)
	  {
		tc_strcpy(cMonth,"Apr");
	  }
	  else if(m==5)
	  {
		tc_strcpy(cMonth,"May");
	  }
	  else if(m==6)
	  {
		tc_strcpy(cMonth,"Jun");
	  }
	  else if(m==7)
	  {
		tc_strcpy(cMonth,"Jul");
	  }
	  else if(m==8)
	  {
		tc_strcpy(cMonth,"Aug");
	  }
	  else if(m==9)
	  {
		tc_strcpy(cMonth,"Sep");
	  }
	  else if(m==10)
	  {
		tc_strcpy(cMonth,"Oct");
	  }
	  else if(m==11)
	  {
		tc_strcpy(cMonth,"Nov");
	  }
	  else if(m==12)
	  {
		tc_strcpy(cMonth,"Dec");
	  }
}
void  getPlantDetailsAttr( char * RoleName ,char  getPlantCS[40],char  getPlantOptCS[40],char  getPlantIA[40],char  getPlantStore[40],char  getUserAgency[40])
{
     char   *PlantName=NULL;
	  printf( "RoleName:%s\n", RoleName);
      PlantName=subString(RoleName,3,4);
      printf( "PlantName:%s\n", PlantName);
	  if(strcmp(RoleName,"APLD")==0)
	  {
		tc_strcpy(getPlantCS,"t5_DwdMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"bl_occ_t5_DwdOptionalCS");
		tc_strcpy(getPlantIA,"t5_DwdIntialAgency");
		tc_strcpy(getPlantStore,"t5_DwdStoreLocation");
		tc_strcpy(getUserAgency,"DWD");
	  }else  if(strcmp(PlantName,"APLP")==0)
	  {
		tc_strcpy(getPlantCS,"t5_PunMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"bl_occ_t5_PunOptionalCS");
		tc_strcpy(getPlantIA,"t5_PunIntialAgency");
		tc_strcpy(getPlantStore,"t5_PunStoreLocation");
		tc_strcpy(getUserAgency,"PUN");
	  }else  if(strcmp(PlantName,"APLC")==0)
	  {
		tc_strcpy(getPlantCS,"t5_CarMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"bl_occ_t5_CarOptionalCS");
		tc_strcpy(getPlantIA,"t5_CarIntialAgency");
		tc_strcpy(getPlantStore,"t5_CarStoreLocation");
		tc_strcpy(getUserAgency,"CAR");
	  }else  if(strcmp(PlantName,"APLJ")==0)
	  {
		tc_strcpy(getPlantCS,"t5_JsrMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"bl_occ_t5_JsrOptionalCS");
		tc_strcpy(getPlantIA,"t5_JsrIntialAgency");
		tc_strcpy(getPlantStore,"t5_JsrStoreLocation");
		tc_strcpy(getUserAgency,"JSR");
	  }else  if(strcmp(PlantName,"APLL")==0)
	  {
		tc_strcpy(getPlantCS,"t5_LkoMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"bl_occ_t5_LkoOptionalCS");
		tc_strcpy(getPlantIA,"t5_LkoIntialAgency");
		tc_strcpy(getPlantStore,"t5_LkoStoreLocation");
		tc_strcpy(getUserAgency,"LKO");
	  }else  if(strcmp(PlantName,"APLA")==0)
	  {
		tc_strcpy(getPlantCS,"t5_AhdMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"t5_AhdOptionalCS");
		tc_strcpy(getPlantIA,"t5_AhdIntialAgency");
		tc_strcpy(getPlantStore,"t5_AhdStoreLocation");
		tc_strcpy(getUserAgency,"AHD");
	  }else  if(strcmp(PlantName,"APLU")==0)
	  {
		tc_strcpy(getPlantCS,"t5_PnrMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"bl_occ_t5_PnrOptionalCS");
		tc_strcpy(getPlantIA,"t5_PnrIntialAgency");
		tc_strcpy(getPlantStore,"t5_PnrStoreLocation");
		tc_strcpy(getUserAgency,"PNR");
	  }else  if(strcmp(PlantName,"APLS")==0)
	  {
		tc_strcpy(getPlantCS,"t5_JdlMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"bl_occ_t5_JdlOptionalCS");
		tc_strcpy(getPlantIA,"t5_JdlIntialAgency");
		tc_strcpy(getPlantStore,"t5_JdlStoreLocation");
		tc_strcpy(getUserAgency,"JDL");
	  }else
	 {
		tc_strcpy(getPlantCS,"t5_PunMakeBuyIndicator");
		tc_strcpy(getPlantOptCS,"bl_occ_t5_PunOptionalCS");
		tc_strcpy(getPlantIA,"t5_PunIntialAgency");
		tc_strcpy(getPlantStore,"t5_PunStoreLocation");
		tc_strcpy(getUserAgency,"PUN");
	  }
}

void getNextDate(char cnextAccessDate[30])
{
	int day, month, year, nd, nm, ny, ndays;
	int		ch1;
	int		ch;
	time_t	temp;
    date_t DayTommorow ;
 	struct tm *timeptr;
	struct tm *timeptr1;
	char pAccessDate[20];
	char pAccessDate1[20];

	char	strDay[20];
	char	strMon[20];
	char	strYear[20];
	char	DateS[20];
	char cMonth[30];
	temp = time(NULL);
	tc_strcpy(pAccessDate,"");
	timeptr = (struct tm *)localtime(&temp);
	ch = strftime(pAccessDate,sizeof(pAccessDate)-1,"%d-%b-%Y %H:%M",timeptr);

	day=timeptr->tm_mday;
	month=(timeptr->tm_mon)+1;
	year=(timeptr->tm_year+1900);
	ndays= days( month, year );//calling days function
	ny= year;
    nm= month;
    nd= day;
	if( ++nd > ndays )
	{
	   nd= 1;
	   if ( ++nm > 12 )
	   {
		 nm= 1;
		 ++ny;
		}
	}
    printf( "Given date is %d:%d:%d\n", day, month, year );
    printf( "Next days date is %d:%d:%d\n", nd, nm, ny );
	getMonth(nm,cMonth);
	printf( "cMonth:%s\n", cMonth);
	sprintf(strDay,"%d",nd);

	sprintf(strMon,"%d",nm);
	sprintf(strYear,"%d",ny);

	tc_strcpy(cnextAccessDate,strDay);
	tc_strcat(cnextAccessDate,"-");
	tc_strcat(cnextAccessDate,cMonth);
	tc_strcat(cnextAccessDate,"-");
	tc_strcat(cnextAccessDate,strYear);
	tc_strcat(cnextAccessDate," ");
	tc_strcat(cnextAccessDate,"00:00");
	printf("\n cnextAccessDate:%s\n",cnextAccessDate);
}




int setEffectivity(tag_t* itemRev,char* date)
{

	int status;
	logical   date_is_valid   = FALSE;
	tag_t	  st_date_id =NULLTAG;
	tag_t     end_date_id =NULLTAG;
	tag_t     eff_t =NULLTAG;
	date_t	  st_date;
	date_t    end_date;
	int       st_count=0;
	tag_t*    status_list;
	tag_t class_id=NULLTAG;
	char* class_name=NULL;
	logical ans=false;
	int   ifail				= 0;


	CALLAPI(WSOM_ask_release_status_list(*itemRev,&st_count,&status_list));

	printf("\n No. of status found:[%d]",st_count);
	if(st_count>0)
	{
		CALLAPI(POM_class_of_instance(status_list[0],&class_id));
		CALLAPI(POM_name_of_class(class_id,&class_name));
		CALLAPI(POM_unload_instances (st_count,status_list));
		CALLAPI(POM_load_instances(st_count,status_list,class_id,1));
		CALLAPI(POM_is_loaded(status_list[0],&ans));
		if(ans==1)
		{
			//CALLAPI(WSOM_effectivity_create_with_dates(status_list[0],NULLTAG,1,&range_date,EFFECTIVITY_closed,&eff_t));
			//CALLAPI(WSOM_eff_set_date_range(status_list[0]));
			//WSOM_status_ask_effectivitie
			//CALLAPI(WSOM_effectivity_create_with_text(status_list[0],NULLTAG,"05-Jul-2011 00:00 to 13-Jul-2011 00:00",&eff_t));
			CALLAPI(WSOM_effectivity_create_with_text(status_list[0],NULLTAG,"11-MAR-2014 00:00 to UP",&eff_t));
			CALLAPI(AOM_save(status_list[0]));
			CALLAPI(POM_save_instances(st_count,status_list,1));

		}else
		{
			printf("\n Cannot load this......");
		}
		//CALLAPI(AOM_save(*itemRev));
		//CALLAPI(AOM_unlock(*itemRev));
	}
	  return ITK_ok;

}

static void MultiExplosion (tag_t line,tag_t line1, int depth,char *taskPrtNo,int reqLevel,int level,char partFndStatus[40])
  {
    int ifail;
	int Item_Revision=0;
	int Item_Revision1=0;
	int Item_ID=0;
	int Item_ID1=0;
    int iChildItemTag;
    int iChildItemTag1;
    char *Item_ID_str, *Item_Revision_str;
    char *Item_ID_str1, *Item_Revision_str1;
	char * ItemName ;
	char * ItemRev ;
    int k=0;
    int n=0;

	tag_t  *children1=NULLTAG;
	tag_t   t_ChildItemRev;
	tag_t   t_ChildItem;

	depth ++;


	printf("\n taskPrtNo:%s ..............",taskPrtNo);fflush(stdout);

	BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID);
	BOM_line_ask_attribute_string(line, Item_ID, &Item_ID_str);
	printf("\n Item_ID_str =%s\n",Item_ID_str);fflush(stdout);

	BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
	BOM_line_ask_attribute_tag(line, iChildItemTag, &t_ChildItemRev);

    BOM_line_look_up_attribute (( char * ) bomAttr_lineItemTag , &iChildItemTag1);
	BOM_line_ask_attribute_tag(line, iChildItemTag1, &t_ChildItem);

	AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemName);
	printf("\n child ItemName:%s ..............",ItemName);fflush(stdout);

	if(tc_strcmp(taskPrtNo,ItemName)==0)
	{
		//tc_strcpy(partFndStatus,"Y");
		tc_strcpy(partFndStatus,"Y");

		goto CLEANUP;
		return ;
	}

	if( level >= reqLevel )
	{
		goto CLEANUP;
	}

	BOM_line_ask_child_lines (line, &n, &children1);
	printf("\n\n\t\t No of child objects for [%s]:[%d]\n",Item_ID_str,n);fflush(stdout);

	level = level + 1;
	for (k = 0; k < n; k++)
	{
		BOM_line_unpack (children1[k]);
		MultiExplosion(children1[k],line, 0,taskPrtNo,reqLevel,level,partFndStatus);
	}
	level = level - 1;
    MEM_free (children1);

	CLEANUP:
		 printf("\n Inside CLEANUP");fflush(stdout);

  }

int createAplCR_Task( METHOD_message_t *msg, va_list  args )
{

    tag_t	TaskTag		= va_arg(args, const tag_t);
    tag_t			TaskTagrev		= NULLTAG;
    int*	n_values	= va_arg(args, int*);
    char	***values	= va_arg(args, char***);
	char	*item_sequence_id;
	char	*DmlAnalyst_name=NULL;
	char	*DmlChangeSpecialist=NULL;
	int		status;
	int max_char_size = 80;
	char*			DMLAPL					=NULL;
	char*			Suffix					=NULL;

	int n_tags_found = 0;


	int
	error_code	= ITK_ok,
	n_entries	= 2,
	n_found		= 0,
	num=0,
	i=0,
	ii			= 0;
	tag_t
	query			= NULLTAG,
	*cntr_objects	= NULL,
	objTypeTag=NULLTAG,
	item=NULLTAG,
	rev=NULLTAG,
	user			= NULLTAG;
	char *item_id 	   = NULL;
	char *Proj_ID 	   = NULL;
	tag_t *tags_found = NULL;

	//char *item_id_dup 	   = NULL;
	int  number_found;
	tag_t *list_of_WSO_tags=NULLTAG;

	char *item_name   = NULL;
	char *Design_group = NULL;
	char *RlsType = NULL;
	char *DML_no = NULL;
	char **DesignGroupList;
	char   type_name[TCTYPE_name_size_c+1];
	tag_t	relation_type,relation	,propTag	= NULLTAG;
	WSO_search_criteria_t  	criteria;

	char *item_id_dup = (char *)MEM_alloc(max_char_size * sizeof(char));
    printf("\n **************inside createCR_Task***************\n ");fflush(stdout);

	if(TCTYPE_ask_object_type(TaskTag,&objTypeTag));
	if(TCTYPE_ask_name(objTypeTag,type_name));
	printf("\n     type_name changes done: for workspaceobject     %s\n", type_name);fflush(stdout);

	if(strcmp(type_name,"T5_APLDML")==0)
	{
        	if(ITEM_ask_latest_rev(TaskTag,&TaskTagrev));
		    printf("\n inside class T5_APLDML......\n");fflush(stdout);
			AOM_ask_value_string( TaskTagrev, "item_id", &item_id);
			AOM_ask_value_string( TaskTagrev, "current_id", &DML_no);
			AOM_ask_value_strings(TaskTagrev,"t5_crdesigngroup",&num,&DesignGroupList);
            AOM_ask_value_string( TaskTagrev, "t5_cprojectcode", &Proj_ID);

			printf("\n item_id : %s\n",item_id);fflush(stdout);
			printf("\n DML_no : %s\n",DML_no);fflush(stdout);
			printf("\n num is : %d\n",num);fflush(stdout);
			for(i=0;i<num;i++)
			{
				 DMLAPL = strtok (DML_no,"_");
				 Suffix = strtok (NULL,"_");

				printf("\n DMLAPL is : %s\n",DMLAPL);fflush(stdout);
    			printf("\n Suffix is : %s\n",Suffix);fflush(stdout);

				strcpy(item_id_dup,DMLAPL);
				strcat (item_id_dup,"_");
				strcat (item_id_dup,DesignGroupList[i]);
				strcat (item_id_dup,"_");
				strcat(item_id_dup,Suffix);


    			printf("\n item_id_dup AFTER CAT is by hanuman :%s\n",item_id_dup);fflush(stdout);

				WSOM_clear_search_criteria(&criteria);
				strcpy(criteria.name,item_id_dup);
				strcpy(criteria.class_name,"T5_APLTaskRevision");
				status	= WSOM_search(criteria, &number_found, &list_of_WSO_tags);

				//ITEM_find_items_by_key_attributes(1, "item_id", item_id_dup, &n_tags_found, &tags_found);
				printf("\n\n\t\t Item_id count in DB is : %d\n",number_found);fflush(stdout);

				if(number_found == 0)
				 {
					printf("\n item_id :%s and DML_no :%s\n ",item_id,DML_no);fflush(stdout);
					ITEM_create_item(item_id_dup,item_id_dup,"T5_APLTask","A",&item,&rev);
					printf("\n Item created first time only with item_id \n");fflush(stdout);

					AOM_save(item);
					AOM_save(rev);
					GRM_find_relation_type("T5_DMLTaskRelation",&relation_type);
					GRM_create_relation(TaskTagrev, rev, relation_type,  NULLTAG, &relation);
					GRM_save_relation(relation);
					if(AssignAPLPlanner(rev,Proj_ID));
					if(AssignAPLReviewer(rev,Proj_ID));

				 }else
				 {
					printf("\n item_id created twice......  \n");fflush(stdout);
				 }

				 MEM_free(list_of_WSO_tags);
			 }
	}

    return error_code;

}

int apl_dml_assign( EPM_action_message_t msg )
{

		int   ifail				= 0;
		int	  noAttachment,t,n_attchs,i,m,noTaskAttachment,l,cnt		=0;
		int  attype = 1;
		tag_t			parent_task				=NULLTAG;
		tag_t			rootTask				=NULLTAG;
		tag_t			*attachments			=NULLTAG;
		tag_t			*Taskattachments			=NULLTAG;
		tag_t			item					=NULLTAG;
		tag_t			*Dml_Task_Tag					=NULLTAG;

		tag_t			*objTypeTagTask			= NULLTAG;

		//GRM_relation_t			*secondary_objects	;
		tag_t			primary					=NULLTAG;
		tag_t			*secondary_objects					=NULLTAG;
		tag_t			remove					= NULLTAG;
		tag_t			user_tag				=NULLTAG;
		tag_t			job						= NULLTAG;
		tag_t		   Analystusertag			= NULLTAG;
		char		*job_name					= NULL;
		char		*job_name_new				= NULL;


		char*			parent_name				=NULL;
		char*			object_type				=NULL;
		char*			Analyst_name			=NULL;
		char*			DmlAnalyst_name			=NULL;

		char*			Dml_no					=NULL;
		char*			CurrentTask				=NULL;
		char*			TaskName				=NULL;
		char*			username				=NULL;
		char*			ImplbyTaskNo			=NULL;
		char*			token					=NULL;
		char*			Tasktoken					=NULL;



		POM_get_user(&username,&user_tag);
		printf("\nStarting for username :%s....\n",username);fflush(stdout);
		CALLAPI(EPM_ask_root_task(msg.task,&rootTask));
		printf("\n apl_dml_assign \n"); fflush(stdout);
		CALLAPI(AOM_ask_value_string(msg.task,"object_name",&CurrentTask));
	   printf("\nCurrentTask:%s\n",CurrentTask);
		CALLAPI(AOM_ask_value_string(rootTask,"object_name",&parent_name));
		printf("\nParent Name:%s\n",parent_name);fflush(stdout);

		if(strstr(parent_name,"Work Break Down - CN")!=NULL)
		{
		  if(strstr(CurrentTask,"Add Solution item")!=NULL)
		  {
			CALLAPI(EPM_ask_attachments(rootTask,EPM_target_attachment,&noAttachment,&attachments));
			printf("Target Attachment:%d\n",noAttachment);fflush(stdout);
			 if(noAttachment > 0)
			{
			   for(t=0;t<noAttachment;t++)
			   {
				  AOM_ask_value_string(attachments[t],"object_type",&object_type);
				  printf("\n object_type is :%s\n",object_type);fflush(stdout);


				  if(strcmp(object_type,"ChangeNoticeRevision")==0)
				  {
					CALLAPI(AOM_ask_value_string(attachments[t],"current_id",&Dml_no));
					printf("\n Dml_no is :%s\n",Dml_no);	fflush(stdout);


				//	CALLAPI(ITEM_find_rev(Dml_no,"A", &item ));


					if(Dml_no)
					{
							printf("\n found part \n");fflush(stdout);
							CALLAPI(AOM_UIF_ask_value(attachments[t],"Analyst",&DmlAnalyst_name));
							printf("\n Analyst_name  of DML  is %s\n", DmlAnalyst_name);

					}
				  }

		      }

			 }
			}
	  }
		return ITK_ok;
}


int apl_dml_release( EPM_action_message_t msg )
{
	int		status;
	int max_char_size = 80;
	tag_t			rootTask				=NULLTAG;
	char		*CurrentTask					= NULL;
	char       *parent_name				=NULL;
	char       *Proj_Code				=NULL;
	char*			Part_no				= NULL;
	char*			PartTypeStr			= NULL;
	char*			type_name1			= NULL;

	int n_tags_found = 0;
	tag_t			*attachments			=NULLTAG;
	int   ifail				= 0;
    char* lcsToset=NULL;
	tag_t   status2=NULLTAG;
	logical retain=false;
     logical stat  ;


	int
	error_code	= ITK_ok,
	n_entries	= 2,
	n_found		= 0,
	num=0,
	i=0,
	j=0,
	ii			= 0;
	tag_t
	query			= NULLTAG,
	*cntr_objects	= NULL,
	objTypeTag=NULLTAG,
	item=NULLTAG,
	rev=NULLTAG,
	user			= NULLTAG;
	char *item_id 	   = NULL;
	char *PlantName 	   = NULL;
	tag_t *tags_found = NULL;
	int	  noAttachment,noTaskAttachment 	= 0;
	tag_t DMLTag = NULLTAG;

	tag_t			item_apl					=NULLTAG;
	tag_t			rev_apl					=NULLTAG;
	tag_t			APLDRevTypeTag		= NULLTAG;
	tag_t			APLDRevCreInTag		= NULLTAG;
	tag_t*			TaskRevision		= NULLTAG;
	tag_t*			PartTags			= NULLTAG;

	char**			stringArrayAPLD		= NULL;
	char**			stringArrayAPLT		= NULL;
	int			    	n_strings			= 1;
	char*			object_type			= NULL;
	 char FrmNextDate[20]="2016/10/12";
	 char ToNextDate[20]="9999/12/31";
	char*			value					=NULL;


	//char *item_id_dup 	   = NULL;
	int   apl_task_number_found;
	int  apl_number_found;
	int * erc_number_found;
	int  control_number_found;

	tag_t *list_of_WSO_tags=NULLTAG;
	tag_t *list_of_WSO_erc_tags=NULLTAG;
	tag_t *list_of_WSO_cntrl_tags=NULLTAG;
	tag_t			APLDTypeTag			= NULLTAG;
	tag_t			APLDCreInTag		= NULLTAG;
	tag_t			APLDMLTag			= NULLTAG;
	tag_t			TaskRevTag			= NULLTAG;
	tag_t			tsk_part_sol_rel_type= NULLTAG;
	tag_t			AssyTag				= NULLTAG;
	tag_t			tsk_part_APL_rel	= NULLTAG;
	date_t *start_end_date = NULL;
	tag_t eff_d = NULLTAG;
	date_t test_date_1;
	date_t test_date_2;
	date_t Release_date;
	tag_t    	    propEff_tag				    =NULLTAG;

	int				index				= 0;
	int				l_strings			= 80;
	char*			tempStringt			= NULL;
	char*			APL_Task_No			= NULL;
	int				count				= 0;
	int				TaskCnt				= 0;
	int				PartCnt				= 0;
	int				AttCnt				= 0;
	int				k					= 0;
    int 	n_instances =1;
	tag_t			APLDMLRevTag		= NULLTAG;
	tag_t			APLTTypeTag			= NULLTAG;
	tag_t			APLTCreInTag		= NULLTAG;
	tag_t			APLTRevTypeTag		= NULLTAG;
	tag_t			APLTRevCreInTag		= NULLTAG;
	tag_t			APLTaskTag			= NULLTAG;
	tag_t			APLTaskRevTag		= NULLTAG;
	tag_t         Fndrelation = NULLTAG;
     tag_t 	  *ReleaseDateTgs = NULLTAG;
     tag_t 	  ReleaseDateTg = NULLTAG;
	char *item_erc_id_dup = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *item_erc_id_dup1 = (char *)MEM_alloc(max_char_size * sizeof(char));
	char*			tempString			= NULL;
     logical * 	date_is_valid  = false;
	char cNextDate[20]={0};
	char cCurrentAccessDate[20]={0};
	char   *ReleaseStatus=NULL;
	char *item_name   = NULL;
	char *Design_group = NULL;
	char *RlsType = NULL;
	char *DML_no = NULL;
	char **DesignGroupList;
	char   type_name[TCTYPE_name_size_c+1];
	tag_t	relation_type,relation	,propTag	= NULLTAG;
	tag_t  aplrelation = NULLTAG;
	tag_t  apltaskrelation = NULLTAG;
	WSO_search_criteria_t  	criteria;
	WSO_search_criteria_t  	criteria_erc;
	WSO_search_criteria_t  	criteria_control;

	lcsToset =(char *) MEM_alloc(20 * sizeof(char *));

    printf("\n **************inside apl_dml_release***************\n ");fflush(stdout);
   strcpy(lcsToset,"T5_LcsAplRlzd");

	stringArrayAPLD = (char**)malloc( n_strings * sizeof *stringArrayAPLD );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLD[index] = (char*)malloc( l_strings + 1 );
	}
	stringArrayAPLT = (char**)malloc( n_strings * sizeof *stringArrayAPLT );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLT[index] = (char*)malloc( l_strings + 1 );
	}


	if(EPM_ask_root_task(msg.task,&rootTask));
	printf("\n apl_dml_release \n"); fflush(stdout);

	if(AOM_ask_value_string(msg.task,"object_name",&CurrentTask));
   printf("\nCurrentTask:%s\n",CurrentTask);
	if(AOM_ask_value_string(rootTask,"object_name",&parent_name));
	printf("\nParent Name:%s\n",parent_name);fflush(stdout);
	if(EPM_ask_attachments(rootTask,EPM_target_attachment,&noAttachment,&attachments));
	printf("Target Attachment:%d\n",noAttachment);fflush(stdout);
	 if(noAttachment > 0)
	{
		DMLTag = attachments[0];
		if(TCTYPE_ask_object_type(DMLTag,&objTypeTag));
    	if(TCTYPE_ask_name(objTypeTag,type_name));
	    printf("\n     type_name changes done: for workspaceobject     %s\n", type_name);fflush(stdout);
	}

	if(strcmp(type_name,"T5_APLDMLRevision")==0)
	{
		    printf("\n inside class T5_APLDMLRevision......\n");fflush(stdout);
			AOM_ask_value_string( DMLTag, "item_id", &item_id);
			AOM_ask_value_string( DMLTag, "current_id", &DML_no);
			AOM_ask_value_strings(DMLTag,"t5_crdesigngroup",&num,&DesignGroupList);
			//AOM_ask_value_string(DMLTag,"t5_rlstype",&RlsType);
			AOM_ask_value_string( DMLTag, "t5_cprojectcode", &Proj_Code);

			getCurrentDateTime(cCurrentAccessDate);
			printf("\n cCurrentAccessDate:%s",cCurrentAccessDate);
		   CALLAPI(ITK_string_to_date(cCurrentAccessDate, &Release_date ));
     	  // CALLAPI(ITK_set_bypass(TRUE));
		   CALLAPI(AOM_lock(DMLTag));
           CALLAPI(AOM_set_value_date	(DMLTag,"t5_APLReleaseDate",Release_date));
			CALLAPI(AOM_save(DMLTag));
		    CALLAPI(AOM_unlock(DMLTag));

			printf("\n Proj_Code : %s\n",Proj_Code);fflush(stdout);
			printf("\n item_id : %s\n",item_id);fflush(stdout);
			printf("\n DML_no : %s\n",DML_no);fflush(stdout);
			printf("\n RlsType : %s\n",RlsType);fflush(stdout);
			printf("\n num is : %d\n",num);fflush(stdout);
        				printf("\n Item created first time only with item_id \n");fflush(stdout);
                        if (DMLTag != NULLTAG)
						 {
							GRM_find_relation_type("T5_DMLTaskRelation",&relation_type);
							 /// Solution Item start

							CALLAPI(GRM_list_secondary_objects_only(DMLTag,relation_type,&count,&TaskRevision));
							printf("\n\n\t\t APL DML Cre:ERC DML to Task : %d",count);fflush(stdout);
							for (TaskCnt=0;TaskCnt<count ;TaskCnt++ )
							{
							TaskRevTag = TaskRevision[TaskCnt];

//						   CALLAPI(ITK_set_bypass(TRUE));
						   CALLAPI(AOM_lock(TaskRevTag));
						   CALLAPI(AOM_set_value_date	(TaskRevTag,"t5_APLReleaseDate",Release_date));
							CALLAPI(AOM_save(TaskRevTag));
							CALLAPI(AOM_unlock(TaskRevTag));

							CALLAPI(AOM_ask_value_string(TaskRevTag,"object_type",&object_type));
							printf("\n\n\t\t object_type is :%s",object_type);fflush(stdout);
							if(strcmp(object_type,"T5_APLTaskRevision")==0)
							{
								PartCnt=0;

								CALLAPI(CR_create_release_status(lcsToset,&status2));
      							CALLAPI(AOM_ask_name(status2, &ReleaseStatus));
								printf("\n ******************* ReleaseStatus: %s\n",ReleaseStatus);fflush(stdout);
								CALLAPI(EPM_add_release_status(status2,1,&TaskRevTag,retain));


								CALLAPI(AOM_ask_value_tags(TaskRevTag,"CMHasSolutionItem",&PartCnt,&PartTags));
								printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
								if (PartCnt>0)
								{
									GRM_find_relation_type("CMHasSolutionItem",&tsk_part_sol_rel_type);
									for (k=0;k<PartCnt ;k++ )
									{
										printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
										AssyTag=PartTags[k];

										CALLAPI(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
										printf("\n\n\t\t APL DML Cre:Part_no  is :%s",Part_no);	fflush(stdout);

										if(TCTYPE_ask_object_type(AssyTag,&objTypeTag));
										if(TCTYPE_ask_name2(objTypeTag,&type_name1));
										printf("\n\n\t\t APL DML Cre:AssyTag type_name1 := %s", type_name1);fflush(stdout);

										CALLAPI(AOM_ask_value_string(AssyTag,"object_type",&PartTypeStr));
										printf("\n\n\t\t APL DML Cre:AssyTag PartTypeStr := %s", PartTypeStr);fflush(stdout);

										if (tc_strcmp(type_name1,"Design Revision")==0 || tc_strcmp(type_name1,"Part Revision")==0 )
										{
								           CALLAPI(EPM_add_release_status(status2,1,&AssyTag,retain));
											if((strcmp(ReleaseStatus,"T5_LcsAplRlzd")==0) )
											{
												CALLAPI(WSOM_ask_effectivity_mode(&stat));

											//	getCurrentDateTime(cCurrentAccessDate);
												CALLAPI(PROP_ask_property_by_name(status2,"effectivity_text",&propEff_tag));
												CALLAPI(PROP_ask_value_string(propEff_tag,&value));

												getNextDate(cNextDate);
												printf("\n cNextDate:%s",cNextDate);

												CALLAPI(ITK_string_to_date(cNextDate, &test_date_1 ));
												CALLAPI(ITK_string_to_date("31-Dec-9999 00:00", &test_date_2 ));

												start_end_date = (date_t *)MEM_alloc(sizeof(date_t)*2);

												printf( "\n Setting release date effectivity [to date - 12-Jun-2014 00:00 and from date - 12-Jun-2014 00:00]on ReleaseStatus object  \n");

												start_end_date[0] = test_date_1;
												start_end_date[1] = test_date_2;

												CALLAPI(WSOM_status_clear_effectivities (status2));
												CALLAPI(WSOM_effectivity_create_with_dates(status2, NULLTAG, 2,start_end_date, EFFECTIVITY_closed, &eff_d ));
												CALLAPI(AOM_save(eff_d));
												CALLAPI(AOM_save(status2));
												CALLAPI(AOM_refresh(status2,0));
											}
										}
									}
								}
							}
						  }  /// Solution Item end
					 }

	}else
	   {
	     	printf("\n No proper class class T5_APLDMLRevision......\n");fflush(stdout);
	   }

	return error_code;
}

int apl_dml_claim( EPM_action_message_t msg )
{
	int		status;
	int max_char_size = 80;
	tag_t			rootTask				=NULLTAG;
	char		*CurrentTask					= NULL;
	char       *parent_name				=NULL;
	char       *Proj_Code				=NULL;
	char*			Part_no				= NULL;
	char*			PartTypeStr			= NULL;
	char*			type_name1			= NULL;

	int n_tags_found = 0;
	tag_t			*attachments			=NULLTAG;
	int   ifail				= 0;
    char* lcsToset=NULL;
	tag_t   status2=NULLTAG;
	logical retain=false;
     logical stat  ;


	int
	error_code	= ITK_ok,
	n_entries	= 2,
	n_found		= 0,
	num=0,
	i=0,
	j=0,
	ii			= 0;
	tag_t
	query			= NULLTAG,
	*cntr_objects	= NULL,
	objTypeTag=NULLTAG,
	item=NULLTAG,
	rev=NULLTAG,
	user			= NULLTAG;
	char *item_id 	   = NULL;
	char *PlantName 	   = NULL;
	tag_t *tags_found = NULL;
	int	  noAttachment,noTaskAttachment 	= 0;
	tag_t DMLTag = NULLTAG;

	tag_t			item_apl					=NULLTAG;
	tag_t			rev_apl					=NULLTAG;
	tag_t			APLDRevTypeTag		= NULLTAG;
	tag_t			APLDRevCreInTag		= NULLTAG;
	tag_t*			TaskRevision		= NULLTAG;
	tag_t*			PartTags			= NULLTAG;

	char**			stringArrayAPLD		= NULL;
	char**			stringArrayAPLT		= NULL;
	int			    	n_strings			= 1;
	char*			object_type			= NULL;
	 char FrmNextDate[20]="2016/10/12";
	 char ToNextDate[20]="9999/12/31";
	char*			value					=NULL;

	char PlantCS[40];
	char PlantOptCS[40];
	char PlantIA[40];
	char PlantStore[40];
	char UserAgency[40];
	//char *item_id_dup 	   = NULL;
	int   apl_task_number_found;
	int  apl_number_found;
	int * erc_number_found;
	int  control_number_found;

	tag_t *list_of_WSO_tags=NULLTAG;
	tag_t *list_of_WSO_erc_tags=NULLTAG;
	tag_t *list_of_WSO_cntrl_tags=NULLTAG;
	tag_t			APLDTypeTag			= NULLTAG;
	tag_t			APLDCreInTag		= NULLTAG;
	tag_t			APLDMLTag			= NULLTAG;
	tag_t			TaskRevTag			= NULLTAG;
	tag_t			tsk_part_sol_rel_type= NULLTAG;
	tag_t			AssyTag				= NULLTAG;
	tag_t			tsk_part_APL_rel	= NULLTAG;
	date_t *start_end_date = NULL;
	tag_t eff_d = NULLTAG;
	date_t test_date_1;
	date_t test_date_2;
	tag_t    	    propEff_tag				    =NULLTAG;

	int				index				= 0;
	int				l_strings			= 80;
	char*			tempStringt			= NULL;
	char*			APL_Task_No			= NULL;
	int				count				= 0;
	int				TaskCnt				= 0;
	int				PartCnt				= 0;
	int				AttCnt				= 0;
	int				k					= 0;
	int              st_count1 = 0;
	int              cnt = 0;
	int              DMLFlag = 0;
	int              PartFlag = 0;

	tag_t			APLDMLRevTag		= NULLTAG;
	tag_t			APLTTypeTag			= NULLTAG;
	tag_t			APLTCreInTag		= NULLTAG;
	tag_t			APLTRevTypeTag		= NULLTAG;
	tag_t			APLTRevCreInTag		= NULLTAG;
	tag_t			APLTaskTag			= NULLTAG;
	tag_t			APLTaskRevTag		= NULLTAG;
	tag_t         Fndrelation = NULLTAG;

	char *item_erc_id_dup = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *item_erc_id_dup1 = (char *)MEM_alloc(max_char_size * sizeof(char));
	char*			tempString			= NULL;
	tag_t  CurrentRoleTag = NULLTAG;
	char       roleName[SA_name_size_c+1]  ;
	tag_t*    status_list1=NULLTAG;

	char cNextDate[20]={0};
	char cCurrentAccessDate[20]={0};
	char   *ReleaseStatus=NULL;
	char *item_name   = NULL;
	char *Design_group = NULL;
	char *RlsType = NULL;
	char *DML_no = NULL;
	char *Part_Coated = NULL;
	char *Part_ColourInd = NULL;
	char *Part_Type = NULL;
	char **DesignGroupList;
	char *WSO_Name = NULL;
	char   type_name[TCTYPE_name_size_c+1];
	tag_t	relation_type,relation	,propTag	= NULLTAG;
	tag_t  aplrelation = NULLTAG;
	tag_t  apltaskrelation = NULLTAG;
	WSO_search_criteria_t  	criteria;
	WSO_search_criteria_t  	criteria_erc;
	WSO_search_criteria_t  	criteria_control;

	lcsToset =(char *) MEM_alloc(20 * sizeof(char *));

    printf("\n **************inside apl_dml_claim***************\n ");fflush(stdout);
   strcpy(lcsToset,"T5_LcsAPLWrkg");

	stringArrayAPLD = (char**)malloc( n_strings * sizeof *stringArrayAPLD );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLD[index] = (char*)malloc( l_strings + 1 );
	}
	stringArrayAPLT = (char**)malloc( n_strings * sizeof *stringArrayAPLT );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLT[index] = (char*)malloc( l_strings + 1 );
	}


	if(EPM_ask_root_task(msg.task,&rootTask));
	printf("\n apl_dml_claim \n"); fflush(stdout);

	if(AOM_ask_value_string(msg.task,"object_name",&CurrentTask));
   printf("\nCurrentTask:%s\n",CurrentTask);
	if(AOM_ask_value_string(rootTask,"object_name",&parent_name));
	printf("\nParent Name:%s\n",parent_name);fflush(stdout);
	if(EPM_ask_attachments(rootTask,EPM_target_attachment,&noAttachment,&attachments));
	printf("Target Attachment:%d\n",noAttachment);fflush(stdout);
	 if(noAttachment > 0)
	{
		DMLTag = attachments[0];
		if(TCTYPE_ask_object_type(DMLTag,&objTypeTag));
    	if(TCTYPE_ask_name(objTypeTag,type_name));
	    printf("\n     type_name changes done: for workspaceobject     %s\n", type_name);fflush(stdout);
	}

	if(strcmp(type_name,"T5_APLTaskRevision")==0 || strcmp(CurrentTask,"Work On DML")==0)
	{
		    printf("\n inside class T5_APLTaskRevision......\n");fflush(stdout);
			AOM_ask_value_string( DMLTag, "item_id", &item_id);
			AOM_ask_value_string( DMLTag, "current_id", &DML_no);
			AOM_ask_value_strings(DMLTag,"t5_crdesigngroup",&num,&DesignGroupList);
			//AOM_ask_value_string(DMLTag,"t5_rlstype",&RlsType);
			AOM_ask_value_string( DMLTag, "t5_crdesigngroup", &Proj_Code);


			ITKCALL(WSOM_ask_release_status_list(DMLTag,&st_count1,&status_list1));
			printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);

			if(st_count1>0)
			{
				for (cnt=0;cnt< st_count1;cnt++ )
				{
					WSO_Name=NULL;
					ITKCALL(AOM_ask_name(status_list1[cnt],&WSO_Name));
					printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);
					if(tc_strcmp(WSO_Name,"")!=0)
						{
							if (tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0)
							{
								DMLFlag = 1;
							}
						}
				}
			}

			if(DMLFlag == 0)
			{
				CALLAPI(CR_create_release_status(lcsToset,&status2));
				CALLAPI(AOM_ask_name(status2, &ReleaseStatus));
				printf("\n ******************* ReleaseStatus: %s\n",ReleaseStatus);fflush(stdout);
				CALLAPI(EPM_add_release_status(status2,1,&DMLTag,retain));
			}

			ITKCALL(SA_ask_current_role(&CurrentRoleTag));
			ITKCALL(SA_ask_role_name(CurrentRoleTag,roleName))
			printf("\n\n  roleName : %s\n",roleName); fflush(stdout);

			printf("\n Proj_Code : %s\n",Proj_Code);fflush(stdout);
			printf("\n item_id : %s\n",item_id);fflush(stdout);
			printf("\n DML_no : %s\n",DML_no);fflush(stdout);
			printf("\n RlsType : %s\n",RlsType);fflush(stdout);
			printf("\n num is : %d\n",num);fflush(stdout);
			printf("\n Item created first time only with item_id \n");fflush(stdout);
			if (DMLTag != NULLTAG)
			 {
					PartCnt=0;
					getPlantDetailsAttr(roleName,PlantCS,PlantOptCS,PlantIA,PlantStore,UserAgency);
					printf("\n PlantCS %s \n",PlantCS);
					printf("\n PlantOptCS %s \n",PlantOptCS);
					printf("\n PlantAgency %s \n",PlantIA);
					printf("\n PlantStore %s \n",PlantStore);
					printf("\n UserAgency %s \n",UserAgency);

					CALLAPI(AOM_ask_value_tags(DMLTag,"CMHasSolutionItem",&PartCnt,&PartTags));
					printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
					if (PartCnt>0)
					{
						for (k=0;k<PartCnt ;k++ )
						{
							printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
							AssyTag=PartTags[k];

							PartFlag = 0;
			            	ITKCALL(WSOM_ask_release_status_list(AssyTag,&st_count1,&status_list1));
							printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);

							if(st_count1>0)
							{
								for (cnt=0;cnt< st_count1;cnt++ )
								{
									WSO_Name=NULL;
									ITKCALL(AOM_ask_name(status_list1[cnt],&WSO_Name));
									printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);
									if(tc_strcmp(WSO_Name,"")!=0)
										{
											if (tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0)
											{
												PartFlag = 1;
											}
										}
								}
							}

							if(PartFlag == 0)
							{
								CALLAPI(CR_create_release_status(lcsToset,&status2));
								CALLAPI(AOM_ask_name(status2, &ReleaseStatus));
								printf("\n ******************* ReleaseStatus: %s\n",ReleaseStatus);fflush(stdout);
								CALLAPI(EPM_add_release_status(status2,1,&AssyTag,retain));
							}


							CALLAPI(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
							printf("\n\n\t\t APL DML Cre:Part_no  is :%s",Part_no);	fflush(stdout);

							CALLAPI(AOM_ask_value_string(AssyTag,"t5_Coated",&Part_Coated));
							printf("\n\n\t\t APL DML Cre:Part_Coated  is :%s",Part_Coated);	fflush(stdout);
							CALLAPI(AOM_ask_value_string(AssyTag,"t5_ColourInd",&Part_ColourInd));
							printf("\n\n\t\t APL DML Cre:Part_ColourInd  is :%s",Part_ColourInd);	fflush(stdout);
							CALLAPI(AOM_ask_value_string(AssyTag,"t5_PartType",&Part_Type));
							printf("\n\n\t\t APL DML Cre:Part_Type  is :%s",Part_Type);	fflush(stdout);

							if(TCTYPE_ask_object_type(AssyTag,&objTypeTag));
							if(TCTYPE_ask_name2(objTypeTag,&type_name1));
							printf("\n\n\t\t APL DML Cre:AssyTag type_name1 := %s", type_name1);fflush(stdout);

							CALLAPI(AOM_ask_value_string(AssyTag,"object_type",&PartTypeStr));
							printf("\n\n\t\t APL DML Cre:AssyTag PartTypeStr := %s", PartTypeStr);fflush(stdout);

							if (tc_strcmp(type_name1,"Design Revision")==0 || tc_strcmp(type_name1,"Part Revision")==0 )
							{
								if((strcmp(Part_ColourInd,"C") ==0) || (strcmp(Part_Coated,"C")==0))
								{
								 printf("\n \t\t Colour part co continue No Update of CS  ");fflush(stdout);
								 continue;
								}
								if((strcmp(Part_Type,"V")==0))
								{
								 printf("\n\n\t\t APL DML Cre:Part_Type  is :%s",Part_Type);	fflush(stdout);
								CALLAPI(AOM_lock(AssyTag));
								 CALLAPI(AOM_set_value_string(AssyTag,PlantCS,"E50"));
								CALLAPI( AOM_save(AssyTag) );
								CALLAPI( AOM_unlock(AssyTag) );
								}else if(strcmp(Part_Type,"T")==0 ||strcmp(Part_Type,"M")==0 )
								{
								 printf("\n\n\t\t APL DML Cre:Part_Type  is :%s",Part_Type);	fflush(stdout);
								CALLAPI(AOM_lock(AssyTag));
								 CALLAPI(AOM_set_value_string(AssyTag,PlantCS,"E50"));
								 CALLAPI(AOM_set_value_string(AssyTag,"t5_EstSheetReqd","NA"));
								 CALLAPI(AOM_set_value_string(AssyTag,"t5_ToolIndentReqd","NA"));
								 CALLAPI(AOM_set_value_string(AssyTag,"t5_PFDModReqd","NA"));
								CALLAPI( AOM_save(AssyTag) );
								CALLAPI( AOM_unlock(AssyTag) );
								}else if(strcmp(Part_Type,"G")==0)
								{
								 printf("\n\n\t\t APL DML Cre:Part_Type  is :%s",Part_Type);	fflush(stdout);
								CALLAPI(AOM_lock(AssyTag));
								 CALLAPI(AOM_set_value_string(AssyTag,"t5_EstSheetReqd","NA"));
								 CALLAPI(AOM_set_value_string(AssyTag,"t5_ToolIndentReqd","NA"));
								 CALLAPI(AOM_set_value_string(AssyTag,"t5_PFDModReqd","NA"));
								CALLAPI( AOM_save(AssyTag) );
								CALLAPI( AOM_unlock(AssyTag) );
								}
								else if((strcmp(Part_Type,"VC")==0))
								{
								 printf("\n\n\t\t APL DML Cre:Part_Type  is :%s",Part_Type);	fflush(stdout);
								 CALLAPI(AOM_lock(AssyTag));
								 CALLAPI(AOM_set_value_string(AssyTag,PlantCS,"E99"));
								CALLAPI( AOM_save(AssyTag) );
								CALLAPI( AOM_unlock(AssyTag) );
								}
							}
						}
					}
				}

	}if(strcmp(type_name,"T5_APLTaskRevision")==0 || strcmp(CurrentTask,"Please Review The Changes Done By APL Planner")==0)
	{
   /********************** Gss 2.49 For Generating 35-36 Parts at reviewer Claim of APL Reviewer *****************/
   /********************** This need to  be implemented if required now it is moved to ERC *****************/
	}
	else
	   {
	     	printf("\n No proper class class T5_APLDMLRevision......\n");fflush(stdout);
	   }

	return error_code;
}
//Start Code to Explode BOM -3
void Multi_Get_Part_BOM_Lvl(tag_t line,tag_t line1,tag_t revRule,int reqLevel,int level,int* StructChldCnt)
{
	int ifail;
    int iChildItemTag=0;
	char * ItemName ;
	int k=0;
    int n1=0;
	tag_t   t_ChildItemRev;
	tag_t*	childrenTag	= NULLTAG;

	printf("\n Inside Multi_Get_Part_BOM_Lvl[%d] ...\n",*StructChldCnt);

	if( level >= reqLevel )
	{
		goto CLEANUP;
	}

	ITKCALL(BOM_line_ask_child_lines (line, &n1, &childrenTag));
	printf("\n\n\t\t No of child objects for n1 : %d\n",n1);fflush(stdout);

	level = level + 1;
	for (k = 0; k < n1; k++)
	{
		BOM_line_unpack (childrenTag[k]);
		BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
		BOM_line_ask_attribute_tag(childrenTag[k], iChildItemTag, &t_ChildItemRev);
		AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemName);
		*StructChldCnt	=	*StructChldCnt+1;
		get_BomChldStrut[*StructChldCnt].child_objs = childrenTag[k];

		Multi_Get_Part_BOM_Lvl(childrenTag[k],line,revRule,reqLevel,level,StructChldCnt);
	}
	level = level - 1;
	MEM_free (childrenTag);

	CLEANUP:
		 printf("\n Inside Multi_Get_Part_BOM_Lvl CLEANUP");fflush(stdout);
}
int Get_Part_BOM_Lvl(tag_t VehicleObj,int reqLevel,char  View[40],char  Context[200],char  Plant[40],int* StructChldCnt)
{
	int   ifail				= 0;

	tag_t revRule 			= NULLTAG;
	char* vehicleNumber 	= NULL;
	int j					= 0;
	PIE_scope_t scope;
	int 	n_closure_tags;
	tag_t * 	closure_tags;
	tag_t  	closure_tag;
	int k1=0;
	int n=0;
	int level 				= 0;
	tag_t	window 				= NULLTAG;
	tag_t	top_line			= NULLTAG;
	tag_t	objChild			= NULLTAG;
	char	*c_Qty						=	NULL;
	tag_t  *children1=NULLTAG;
	int iChildItemTag=0;
	tag_t   t_ChildItemRev;
	char * ItemName ;
	char closureRuleName[300] ;
	char *revRuleName;

	printf("\nInside Get_Part_BOM_Lvl ....\n");

	if(VehicleObj == NULLTAG)
	{
		printf("\n VehicleObj is NULLTAG\n");fflush(stdout);
		return ifail;
	}
	CALLAPI(AOM_ask_value_string(VehicleObj,"item_id",&vehicleNumber));
	printf("\n Part number===>%s\n",vehicleNumber);fflush(stdout);

	//Start Closure Rule
	if(tc_strcmp(View,"ERC")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleERC");
	}
	else if(tc_strcmp(View,"APLC")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleAPLC");
	}
	else if(tc_strcmp(View,"STDC")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleSTDC");
	}
	else if(tc_strcmp(View,"APLP")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleAPLP");
	}
	else if(tc_strcmp(View,"STDP")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleSTDP");
	}
	else if(tc_strcmp(View,"APLD")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleAPLD");
	}
	else if(tc_strcmp(View,"STDD")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleSTDD");
	}
	else if(tc_strcmp(View,"APLJ")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleAPLJ");
	}
	else if(tc_strcmp(View,"STDJ")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleSTDJ");
	}
	else if(tc_strcmp(View,"APLL")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleAPLL");
	}
	else if(tc_strcmp(View,"STDL")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleSTDL");
	}
	else if(tc_strcmp(View,"APLA")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleAPLA");
	}
	else if(tc_strcmp(View,"STDA")==0)
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleSTDA");
	}
	else
	{
		tc_strcpy(closureRuleName,"BOMViewClosureRuleERC");
	}
	//End Closure Rule

	//Start Revision Rule


	if((tc_strcmp(Context,"All States with Last Revision")==0))
	{
		revRuleName="Latest Working";
	}
	else if((tc_strcmp(Context,"ERC Review and above")==0))
	{
		revRuleName="ERC Review And Above";
	}
	else if((tc_strcmp(Context,"ERC Released data only")==0))
	{
		revRuleName="ERC release and above";
	}
	else if((tc_strcmp(Context,"APL Released data only")==0) && (tc_strcmp(Plant,"CAR")==0))
	{
		revRuleName="APLC Released and Above";
	}
	else if((tc_strcmp(Context,"STDSI Released data only")==0) && (tc_strcmp(Plant,"CAR")==0))
	{
		revRuleName="STDC Released and Above";
	}
	else if((tc_strcmp(Context,"APL Released data only")==0) && (tc_strcmp(Plant,"CVBU Pune")==0))
	{
		revRuleName="APLP Released and Above";
	}
	else if((tc_strcmp(Context,"STDSI Released data only")==0) && (tc_strcmp(Plant,"CVBU Pune")==0))
	{
		revRuleName="STDP Released and Above";
	}
	else if((tc_strcmp(Context,"APL Released data only")==0) && (tc_strcmp(Plant,"CVBU JSR")==0))
	{
		revRuleName="APLJ Released and Above";
	}
	else if((tc_strcmp(Context,"STDSI Released data only")==0) && (tc_strcmp(Plant,"CVBU JSR")==0))
	{
		revRuleName="STDJ Released and Above";
	}
	else if((tc_strcmp(Context,"APL Released data only")==0) && (tc_strcmp(Plant,"CVBU LKO")==0))
	{
		revRuleName="APLL Released and Above";
	}
	else if((tc_strcmp(Context,"STDSI Released data only")==0) && (tc_strcmp(Plant,"CVBU LKO")==0))
	{
		revRuleName="STDL Released and Above";
	}
	else if((tc_strcmp(Context,"APL Released data only")==0) && (tc_strcmp(Plant,"CVBU PNR")==0))
	{
		revRuleName="APLU Released and Above";
	}
	else if((tc_strcmp(Context,"STDSI Released data only")==0) && (tc_strcmp(Plant,"CVBU PNR")==0))
	{
		revRuleName="STDU Released and Above";
	}
	else if((tc_strcmp(Context,"APL Released data only")==0) && (tc_strcmp(Plant,"SMALLCAR AHD")==0))
	{
		revRuleName="APLA Released and Above";
	}
	else if((tc_strcmp(Context,"STDSI Released data only")==0) && (tc_strcmp(Plant,"SMALLCAR AHD")==0))
	{
		revRuleName="STDA Released and Above";
	}
	else if((tc_strcmp(Context,"APL Released data only")==0) && (tc_strcmp(Plant,"DHARWAD")==0))
	{
		revRuleName="APLD Released and Above";
	}
	else if((tc_strcmp(Context,"STDSI Released data only")==0) && (tc_strcmp(Plant,"DHARWAD")==0))
	{
		revRuleName="STDD Released and Above";
	}
	else if((tc_strcmp(Context,"APL Released data only")==0) && (tc_strcmp(Plant,"PUVBU")==0))
	{
		revRuleName="APLV Released and Above";
	}
	else if((tc_strcmp(Context,"STDSI Released data only")==0) && (tc_strcmp(Plant,"PUVBU")==0))
	{
		revRuleName="STDV Released and Above";
	}
	else
	{
		revRuleName="Latest Working";
	}

	//End Revision Rule
	/*if((tc_strcmp(View,"ERC")==0) && (tc_strcmp(Context,"ERC Released data only")==0) && (tc_strcmp(Plant,"CVBU Pune")==0))
	{
		//Based on condition
		tc_strcpy(closureRuleName,"BOMExpandSkipByPartSource");
		revRuleName="Latest Working";
	}
	else
	{
		tc_strcpy(closureRuleName,"BOMExpandSkipByPartSource");
		revRuleName="Latest Working";
	}*/

	printf("\nBefore Size of StructChldCnt==>%d\n",*StructChldCnt);fflush(stdout);
	printf("\nBefore reqLevel==>%d\n",reqLevel);fflush(stdout);
	printf("\nBefore level==>%d\n",level);fflush(stdout);
	printf("\nclosureRuleName==>%s\n",closureRuleName);fflush(stdout);
	printf("\nrevRuleName==>%s\n",revRuleName);fflush(stdout);

	scope=PIE_TEAMCENTER;

	CALLAPI(BOM_create_window (&window));
	CALLAPI(CFM_find(revRuleName, &revRule));
	if (revRule != NULLTAG)
	{
		printf("\nFind revRule\n");fflush(stdout);
		CALLAPI(BOM_set_window_config_rule(window,revRule));
	}

	CALLAPI(PIE_find_closure_rules2(closureRuleName,scope,&n_closure_tags,&closure_tags));
	printf("\n n_closure_tags:%d ..............",n_closure_tags);fflush(stdout);
	if(n_closure_tags==1)
	{
		closure_tag=closure_tags[0];
		CALLAPI(BOM_window_set_closure_rule(window,closure_tag,0,NULL,NULL));
	}

	BOM_set_window_top_line(window, null_tag,VehicleObj ,null_tag, &top_line);
	ITKCALL(BOM_line_ask_child_lines (top_line, &n, &children1));
	printf("\n\n\t\t No of child objects are n : %d\n",n);fflush(stdout);

	for (k1 = 0; k1 < n; k1++)
	{
		BOM_line_unpack (children1[k1]);

		BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
		BOM_line_ask_attribute_tag(children1[k1], iChildItemTag, &t_ChildItemRev);
		AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemName);
		printf("\n child ItemName:%s ..............",ItemName);fflush(stdout);
		printf("\n StructChldCnt:%d ..............",*StructChldCnt);fflush(stdout);

		*StructChldCnt	=	*StructChldCnt+1;

		get_BomChldStrut[*StructChldCnt].child_objs = children1[k1];
		printf("\n StructChldCnt:%d ..............",*StructChldCnt);fflush(stdout);
		Multi_Get_Part_BOM_Lvl(children1[k1],top_line,revRule,reqLevel,level,StructChldCnt);
	}

	printf("\nAfter Size of StructChldCnt==>%d\n",*StructChldCnt);fflush(stdout);

	for (j=1;j<= *StructChldCnt;j++ )
	{
		objChild		= NULLTAG;
		iChildItemTag	= 0;
		t_ChildItemRev	= NULLTAG;
		c_Qty			= NULL;

		//printf("\nPrint Item ID==>%d\n",j);fflush(stdout);

		objChild=get_BomChldStrut[j].child_objs;
		BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag);
		BOM_line_ask_attribute_tag(objChild, iChildItemTag, &t_ChildItemRev);
		AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemName);

		AOM_ask_value_string(objChild,"bl_quantity",&c_Qty);

		printf("\nItemName==>%s\n",ItemName);fflush(stdout);
		printf("\nQty==>%s\n",c_Qty);fflush(stdout);
	}
	return ifail;
}
//End Code to Explode BOM -3

//Start of Function apl_dml_create_appl_desgrp
int apl_dml_create( EPM_action_message_t msg )
{
	char	*item_sequence_id;
	char	*DmlAnalyst_name=NULL;
	char	*DmlChangeSpecialist=NULL;
	int		status;
	int max_char_size = 80;
	char*			DMLAPL					=NULL;
	char*			Suffix					=NULL;
	tag_t			rootTask				=NULLTAG;
	char		*CurrentTask					= NULL;
	char       *parent_name				=NULL;
	char       *Proj_Code				=NULL;
	char*			Part_no				= NULL;
	char*			PartTypeStr			= NULL;
	char*			type_name1			= NULL;

	int n_tags_found = 0;
	tag_t			*attachments			=NULLTAG;
	int   ifail				= 0;


	int
	error_code	= ITK_ok,
	n_entries	= 2,
	n_found		= 0,
	num=0,
	i=0,
	j=0,
	ii			= 0;
	tag_t
	query			= NULLTAG,
	*cntr_objects	= NULL,
	objTypeTag=NULLTAG,
	item=NULLTAG,
	rev=NULLTAG,
	user			= NULLTAG;
	char *item_id 	   = NULL;
	char *erc_item_id 	   = NULL;
	char *PlantName 	   = NULL;
	char *subSyscd 	   = NULL;//Start Plant Aggregate
	char *DesGrpVal 	   = NULL;//Start Plant Aggregate
	char *DesGrpTmp 	   = NULL;//Start Plant Aggregate
	tag_t *tags_found = NULL;
	int	  noAttachment,noTaskAttachment 	= 0;
	int	  iDes 	= 0;//Start Plant Aggregate
	tag_t DMLTag = NULLTAG;

	tag_t			item_apl					=NULLTAG;
	tag_t			rev_apl					=NULLTAG;
	tag_t			APLDRevTypeTag		= NULLTAG;
	tag_t			APLDRevCreInTag		= NULLTAG;
	tag_t*			TaskRevision		= NULLTAG;
	tag_t*			PartTags			= NULLTAG;

	char**			stringArrayAPLD		= NULL;
	char**			stringArrayAPLT		= NULL;
	int			    	n_strings			= 1;
	int			    	n_strings_dsg		= 99;//Start Plant Aggregate
	int			    	cnt		= 0;//Start Plant Aggregate
	int			    	cnt1		= 0;//Start Plant Aggregate
	int			    	FlagFound		= 0;//Start Plant Aggregate
	char*			object_type			= NULL;


	//char *item_id_dup 	   = NULL;
	int   apl_task_number_found;
	int  apl_number_found;
	int * erc_number_found;
	int  control_number_found;

	tag_t *list_of_WSO_tags=NULLTAG;
	tag_t *list_of_WSO_erc_tags=NULLTAG;
	tag_t *list_of_WSO_cntrl_tags=NULLTAG;
	tag_t			APLDTypeTag			= NULLTAG;
	tag_t			APLDCreInTag		= NULLTAG;
	tag_t			APLDMLTag			= NULLTAG;
	tag_t			TaskRevTag			= NULLTAG;
	tag_t			tsk_part_sol_rel_type= NULLTAG;
	tag_t			AssyTag				= NULLTAG;
	tag_t			tsk_part_APL_rel	= NULLTAG;

	int				index				= 0;
	int				l_strings			= 80;
	char*			tempStringt			= NULL;
	char*			APL_Task_No			= NULL;
	int				count				= 0;
	int				TaskCnt				= 0;
	int				PartCnt				= 0;
	int				AttCnt				= 0;
	int				k					= 0;

	tag_t			APLDMLRevTag		= NULLTAG;
	tag_t			APLTTypeTag			= NULLTAG;
	tag_t			APLTCreInTag		= NULLTAG;
	tag_t			APLTRevTypeTag		= NULLTAG;
	tag_t			APLTRevCreInTag		= NULLTAG;
	tag_t			APLTaskTag			= NULLTAG;
	tag_t			APLTaskRevTag		= NULLTAG;
	tag_t         Fndrelation = NULLTAG;

	char *item_erc_id_dup = (char *)MEM_alloc(max_char_size * sizeof(char));
	char *item_erc_id_dup1 = (char *)MEM_alloc(max_char_size * sizeof(char));
	char*			tempString			= NULL;

     char *erc_dml_name  = NULL;
	 char *erc_dml_desc  = NULL;
	char *item_name   = NULL;
	char *Design_group = NULL;
	char *RlsType = NULL;
	char *DML_no = NULL;
	char **DesignGroupList;
	char **DesignGroupStr= NULL;// Plant Aggregate
	char **DesignGroupStr1= NULL;// Plant Aggregate
	char *DesGrpFrst= NULL;// Plant Aggregate
	char *DesGrpFrstTmp= NULL;// Plant Aggregate
	int	DesGrpFrstTmp1;// Plant Aggregate
	char	DesGrpFrstTmp2[20];// Plant Aggregate
	char *DesGrpLst= NULL;// Plant Aggregate
	char   type_name[TCTYPE_name_size_c+1];
	tag_t	relation_type,relation	,propTag	= NULLTAG;
	tag_t  aplrelation = NULLTAG;
	tag_t  apltaskrelation = NULLTAG;
	WSO_search_criteria_t  	criteria;
	WSO_search_criteria_t  	criteria_erc;
	WSO_search_criteria_t  	criteria_control;
	logical			flgFnd			= false;

	//Start Code to Explode BOM -1
	//int     reqLvl=99;
	//char	View[40];
	//char	Context[200];
	//char	Plant[40];
	//int		StructChldCnt = 0;

	//End Code to Explode BOM -1

	char *item_id_dup = (char *)MEM_alloc(max_char_size * sizeof(char));
    printf("\n **************inside apl_dml_create***************\n ");fflush(stdout);

	stringArrayAPLD = (char**)malloc( n_strings * sizeof *stringArrayAPLD );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLD[index] = (char*)malloc( l_strings + 1 );
	}
	stringArrayAPLT = (char**)malloc( n_strings * sizeof *stringArrayAPLT );
	for( index=0; index<n_strings; index++ )
	{
		stringArrayAPLT[index] = (char*)malloc( l_strings + 1 );
	}
	//Start Plant Aggregate
	DesignGroupStr = (char**)malloc( n_strings_dsg * sizeof *DesignGroupStr );
	for( index=0; index<n_strings_dsg; index++ )
	{
		DesignGroupStr[index] = (char*)malloc( l_strings + 1 );
	}

	DesignGroupStr1 = (char**)malloc( n_strings_dsg * sizeof *DesignGroupStr1 );
	for( index=0; index<n_strings_dsg; index++ )
	{
		DesignGroupStr1[index] = (char*)malloc( l_strings + 1 );
	}

	// DesGrpTmp = (char *)MEM_alloc(max_char_size * sizeof(char));
	//END Plant Aggregate

	if(EPM_ask_root_task(msg.task,&rootTask));
	printf("\n apl_dml_create \n"); fflush(stdout);

	if(AOM_ask_value_string(msg.task,"object_name",&CurrentTask));
   printf("\nCurrentTask:%s\n",CurrentTask);
	if(AOM_ask_value_string(rootTask,"object_name",&parent_name));
	printf("\nParent Name:%s\n",parent_name);fflush(stdout);
	if(EPM_ask_attachments(rootTask,EPM_target_attachment,&noAttachment,&attachments));
	printf("Target Attachment:%d\n",noAttachment);fflush(stdout);
	 if(noAttachment > 0)
	{
		DMLTag = attachments[0];
		if(TCTYPE_ask_object_type(DMLTag,&objTypeTag));
    	if(TCTYPE_ask_name(objTypeTag,type_name));
	    printf("\n     type_name changes done: for workspaceobject     %s\n", type_name);fflush(stdout);
	}

	//Start Code to Explode BOM -2
	//Get_Part_BOM_Lvl(DMLTag,reqLvl,View,Context,Plant,&StructChldCnt);
	//printf("\n********After Get_Part_BOM_Lvl *********");fflush(stdout);
	//End Code to Explode BOM -2

	if(strcmp(type_name,"ChangeRequestRevision")==0)
	{
		    printf("\n inside class ChangeRequestRevision......\n");fflush(stdout);
			AOM_ask_value_string( DMLTag, "item_id", &item_id);
			AOM_ask_value_string( DMLTag, "current_id", &DML_no);
			AOM_ask_value_strings(DMLTag,"t5_crdesigngroup",&num,&DesignGroupList);
			//AOM_ask_value_string(DMLTag,"t5_rlstype",&RlsType);
			AOM_ask_value_string( DMLTag, "t5_cprojectcode", &Proj_Code);
			AOM_ask_value_string( DMLTag, "object_name", &erc_dml_name);
			AOM_ask_value_string( DMLTag, "object_desc", &erc_dml_desc);

			printf("\n Proj_Code : %s\n",Proj_Code);fflush(stdout);
			printf("\n item_id : %s\n",item_id);fflush(stdout);
			printf("\n DML_no : %s\n",DML_no);fflush(stdout);
			printf("\n RlsType : %s\n",RlsType);fflush(stdout);
			printf("\n num is : %d\n",num);fflush(stdout);
			printf("\n erc_dml_name is : %s\n",erc_dml_name);fflush(stdout);
			printf("\n erc_dml_desc is : %s\n",erc_dml_desc);fflush(stdout);

			WSOM_clear_search_criteria(&criteria_control);
			strcpy(criteria_control.name,"PlantDML");
			strcpy(criteria_control.class_name,"T5_ControlObject");
			status	= WSOM_search(criteria_control, &control_number_found, &list_of_WSO_cntrl_tags);

			printf("\n control_number_found is : %d\n",control_number_found);fflush(stdout);
			for(j=0;j<control_number_found;j++)
			{

				AOM_ask_value_string( list_of_WSO_cntrl_tags[j], "t5_SubSyscd", &subSyscd);
	            printf("\n subSyscd: %s\n",subSyscd);fflush(stdout);

				if(strcmp(subSyscd,Proj_Code)==0)
				{
				printf("\n j++++++++++++++++++[%d]\n",j);fflush(stdout);

				item_erc_id_dup = NULL;
				item_erc_id_dup =	(char *)MEM_alloc(max_char_size * sizeof(char));

				strcpy(item_erc_id_dup,item_id);
				AOM_ask_value_string( list_of_WSO_cntrl_tags[j], "t5_Userinfo1", &PlantName);
				printf("\n PlantName11: %s\n",PlantName);fflush(stdout);

				AOM_ask_value_string( list_of_WSO_cntrl_tags[j], "t5_Userinfo3", &DesGrpVal);
			//	AOM_ask_value_string( list_of_WSO_cntrl_tags[j], "t5_Userinfo3", &DesGrpVal);
				 printf("\n DesGrpVal: %s\n",DesGrpVal);fflush(stdout);


				strcat (item_erc_id_dup,"_");
				strcat (item_erc_id_dup,PlantName);

				printf("\n item_erc_id_dup: %s\n",item_erc_id_dup);fflush(stdout);
				WSOM_clear_search_criteria(&criteria_erc);
				strcpy(criteria_erc.name,item_erc_id_dup);
				strcpy(criteria_erc.class_name,"T5_APLDMLRevision");
				status	= WSOM_search(criteria_erc, &apl_number_found, &list_of_WSO_erc_tags);

				printf("\n apl_number_found is000 :[%d]\n",apl_number_found);fflush(stdout);
					 if(apl_number_found == 0)
					{
						 //Start Plant Aggregate

	//					 AOM_ask_value_string( list_of_WSO_cntrl_tags[j], "t5_Userinfo4", &DesGrpVal);
	//					 printf("\n DesGrpVal: %s\n",DesGrpVal);fflush(stdout);

						 //TOKEN BASED on ,

						 cnt =0;
						 flgFnd			= false;

						if((tc_strcmp(DesGrpVal,"")!=0) && (tc_strcmp(DesGrpVal,"00-99")!=0))
						{
								if(tc_strcmp(DesGrpVal,"")!=0)
								{
									DesGrpTmp = strtok(DesGrpVal, ", ");
									 while (DesGrpTmp != NULL)
									 {
											printf("VAULE:%s\n", DesGrpTmp);
											tc_strcpy( DesignGroupStr[cnt],DesGrpTmp);
											cnt++;
											DesGrpTmp = strtok(NULL, ",");
											printf("VAULE DesGrpTmp:%s\n", DesGrpTmp);
									 }
								 }

								 printf("\ncnt=[%d]\n",cnt);fflush(stdout);

								 //TOKEN BASED on -
								DesGrpTmp = NULL;
								 cnt1 =0;
								 for(iDes=0;iDes<cnt;iDes++)
								 {
									 DesGrpTmp = NULL;
									 printf("\niDes Value=[%s]\n",DesignGroupStr[iDes]);fflush(stdout);

									 if(tc_strcmp(DesignGroupStr[iDes],"")!=0)
									{
										 if(tc_strstr(DesignGroupStr[iDes],"-")!=NULL)
										{
											DesGrpFrst	=strtok(DesignGroupStr[iDes],"-");
											DesGrpLst	=strtok(NULL,"-");

											printf("\nDesGrpFrst=[%s]\n",DesGrpFrst);fflush(stdout);

										//	tc_strcpy( DesignGroupStr1[cnt1],DesGrpFrst);
										//	cnt1++;

											DesGrpFrstTmp1=atoi(DesGrpFrst);

											 while (DesGrpFrstTmp1!=atoi(DesGrpLst))
											 {
												   sprintf(DesGrpFrstTmp2,"%d",DesGrpFrstTmp1);
												     if(tc_strcmp(DesGrpFrstTmp2,"0")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"00");
														printf("\nInside DesGrpFrst********\n");fflush(stdout);
												   }
												 else  if(tc_strcmp(DesGrpFrstTmp2,"1")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"01");
												   }
												   else  if(tc_strcmp(DesGrpFrstTmp2,"2")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"02");
												   }
													 else  if(tc_strcmp(DesGrpFrstTmp2,"3")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"03");
												   }
													 else  if(tc_strcmp(DesGrpFrstTmp2,"4")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"04");
												   }
													 else  if(tc_strcmp(DesGrpFrstTmp2,"5")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"05");
												   }
													 else  if(tc_strcmp(DesGrpFrstTmp2,"6")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"06");
												   }
													 else  if(tc_strcmp(DesGrpFrstTmp2,"7")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"07");
												   }
													 else  if(tc_strcmp(DesGrpFrstTmp2,"8")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"08");
												   }
													 else  if(tc_strcmp(DesGrpFrstTmp2,"9")==0)
												  {
														tc_strcpy(DesignGroupStr1[cnt1],"09");
												   }
												   else
													{
													   printf("\nDesGrpFrstTmp2=[%s]\n",DesGrpFrstTmp2);fflush(stdout);
														tc_strcpy(DesignGroupStr1[cnt1],DesGrpFrstTmp2);
													}
													cnt1++;
													 DesGrpFrstTmp1=atoi(DesGrpFrstTmp2);
													 DesGrpFrstTmp1++;
											 }
											 printf("\nDesGrpLst=[%s]\n",DesGrpLst);fflush(stdout);
											 tc_strcpy( DesignGroupStr1[cnt1],DesGrpLst);
											 cnt1++;
										}
										else
										{
											printf("\nCOPIED=[%s]\n",DesignGroupStr[iDes]);fflush(stdout);
											tc_strcpy( DesignGroupStr1[cnt1],DesignGroupStr[iDes]);
											cnt1++;
										}
									 }
								  }
								 printf("\nAfter All Token cnt1=[%d]\n",cnt1);fflush(stdout);

								  for(iDes=0;iDes<cnt1;iDes++)
								 {
									 printf("\nDesignGroupStr1[iDes] Value=[%s]\n",DesignGroupStr1[iDes]);fflush(stdout);
								 }

								FlagFound =0;
								 for(i=0;i<num;i++)
								{
									printf("\nDesignGroupList[i] Value*********=[%s]\n",DesignGroupList[i]);fflush(stdout);

									 for(iDes=0;iDes<cnt1;iDes++)
									 {
										  if(tc_strcmp(DesignGroupList[i],DesignGroupStr1[iDes])==0)
										  {
											  FlagFound=1;
											  break;
										  }
									}
								}
								printf("\nFlagFound Value for DML=[%d]\n",FlagFound);fflush(stdout);
								 if(FlagFound==0)
									 continue;

								 //END Plant Aggregate

						}
						printf("\n apl_number_found is111 : %d\n",apl_number_found);fflush(stdout);

						CALLAPI( TCTYPE_find_type( "T5_APLDML", NULL, &APLDTypeTag) );
						CALLAPI( TCTYPE_construct_create_input( APLDTypeTag, &APLDCreInTag) );

						CALLAPI( TCTYPE_find_type( "T5_APLDMLRevision", NULL, &APLDRevTypeTag) );
						CALLAPI( TCTYPE_construct_create_input( APLDRevTypeTag, &APLDRevCreInTag) );

						//Build the APL DML Input Structure

						tc_strcpy( stringArrayAPLD[0], item_erc_id_dup);
						//strncpy( stringArrayAPLD[0], item_erc_id_dup, l_strings );
						CALLAPI( TCTYPE_set_create_display_value( APLDCreInTag, "item_id", 1, (const char**)stringArrayAPLD) );
						tc_strcpy( stringArrayAPLD[0], erc_dml_name);
						CALLAPI( TCTYPE_set_create_display_value( APLDCreInTag, "object_name", 1, (const char**)stringArrayAPLD) );
						tc_strcpy( stringArrayAPLD[0], erc_dml_desc);
						CALLAPI( TCTYPE_set_create_display_value( APLDCreInTag, "object_desc", 1, (const char**)stringArrayAPLD) );

						CALLAPI( AOM_tag_to_string(APLDRevCreInTag, &tempString) );
						tc_strcpy( stringArrayAPLD[0], tempString);
						printf("\nTest0.D1..[%s]\n",stringArrayAPLD[0]);
						CALLAPI( TCTYPE_set_create_display_value( APLDCreInTag, "revision", 1,(const char**)stringArrayAPLD) );

						tc_strcpy( stringArrayAPLD[0], "A");
						CALLAPI( TCTYPE_set_create_display_value( APLDRevCreInTag, "item_revision_id", 1, (const char**)stringArrayAPLD) );

						printf("\nTest1..\n");
						CALLAPI( TCTYPE_create_object(APLDCreInTag, &APLDMLTag) );
						printf("\nTest2..\n");

						AOM_save(APLDMLTag);

						//CALLAPI( FL_user_update_newstuff_folder(APLDMLTag) );

						CALLAPI(ITEM_ask_latest_rev(APLDMLTag,&APLDMLRevTag));

						if (DMLTag != NULLTAG && APLDMLRevTag != NULLTAG)
						{
						GRM_find_relation_type("T5_DMLTaskRelation",&relation_type);
						GRM_create_relation(DMLTag, APLDMLRevTag, relation_type,  NULLTAG, &aplrelation);
						GRM_save_relation(aplrelation);
						printf("\n APL DML Created for ERC DML  : %s\n",item_erc_id_dup);fflush(stdout);
						//Update Project code on APL DML
						CALLAPI(AOM_lock(APLDMLRevTag));
						CALLAPI(AOM_set_value_string(APLDMLRevTag,"t5_cprojectcode",Proj_Code));
						CALLAPI( AOM_save(APLDMLRevTag) );
						CALLAPI( AOM_unlock(APLDMLRevTag) );
					  }

					FlagFound =0;
					for(i=0;i<num;i++)
					{
						if((tc_strcmp(DesGrpVal,"")!=0) && (tc_strcmp(DesGrpVal,"00-99")!=0))
						{
							printf("\nDesignGroupList[i] Value=[%s]\n",DesignGroupList[i]);fflush(stdout);
							//START Plant Aggregate
							 for(iDes=0;iDes<cnt1;iDes++)
							 {
								  if(tc_strcmp(DesignGroupList[i],DesignGroupStr1[iDes])==0)
								  {
									  FlagFound=1;
									  break;
								  }
							 }
							 printf("\nFlagFound Value for TASK=[%d]\n",FlagFound);fflush(stdout);
							 if(FlagFound==0)
								 continue;

							 //END Plant Aggregate
					  }

						printf("\n item_erc_id_dup is : %s\n",item_erc_id_dup);fflush(stdout);
						strcpy(item_erc_id_dup1,item_erc_id_dup);

						 DMLAPL = strtok (item_erc_id_dup,"_");
						 Suffix = strtok (NULL,"_");

						item_erc_id_dup = NULL;
						item_erc_id_dup =	(char *)MEM_alloc(max_char_size * sizeof(char));
						strcpy(item_erc_id_dup,item_erc_id_dup1);

						printf("\n DMLAPL is : %s\n",DMLAPL);fflush(stdout);
						printf("\n Suffix is : %s\n",Suffix);fflush(stdout);

						strcpy(item_id_dup,DMLAPL);
						strcat (item_id_dup,"_");
						strcat (item_id_dup,DesignGroupList[i]);
						strcat (item_id_dup,"_");
						strcat(item_id_dup,Suffix);

						printf("\n item_id_dup AFTER CAT is by hanuman :%s\n",item_id_dup);fflush(stdout);

						WSOM_clear_search_criteria(&criteria);
						strcpy(criteria.name,item_id_dup);
						strcpy(criteria.class_name,"T5_APLTaskRevision");
						status	= WSOM_search(criteria, &apl_task_number_found, &list_of_WSO_tags);

						//ITEM_find_items_by_key_attributes(1, "item_id", item_id_dup, &n_tags_found, &tags_found);
						printf("\n\n\t\t apl_task_number_found count in DB is : %d\n",apl_task_number_found);fflush(stdout);

						  if(apl_task_number_found == 0)
						  {
							printf("\n item_id_dup :%s and Design_group :%s and DML_no :%s \n ",item_id_dup,DesignGroupList[i],DML_no);fflush(stdout);

							CALLAPI( TCTYPE_find_type( "T5_APLTask", NULL, &APLTTypeTag) );
							CALLAPI( TCTYPE_construct_create_input( APLTTypeTag, &APLTCreInTag) );

							CALLAPI( TCTYPE_find_type( "T5_APLTaskRevision", NULL, &APLTRevTypeTag) );
							CALLAPI( TCTYPE_construct_create_input( APLTRevTypeTag, &APLTRevCreInTag) );


							tc_strcpy( stringArrayAPLT[0], item_id_dup);
							CALLAPI( TCTYPE_set_create_display_value( APLTCreInTag, "item_id", 1,(const char**)stringArrayAPLT) );
							tc_strcpy( stringArrayAPLT[0], erc_dml_name);
							CALLAPI( TCTYPE_set_create_display_value( APLTCreInTag, "object_name", 1,(const char**)stringArrayAPLT) );

							CALLAPI( AOM_tag_to_string(APLTRevCreInTag, &tempStringt) );
							tc_strcpy( stringArrayAPLT[0], tempStringt);
							printf("\nTest0.4..[%s]\n",stringArrayAPLT[0]);
							CALLAPI( TCTYPE_set_create_display_value( APLTCreInTag, "revision", 1,(const char**) stringArrayAPLT) );
							tc_strcpy( stringArrayAPLT[0], "A");
							CALLAPI( TCTYPE_set_create_display_value( APLTRevCreInTag, "item_revision_id", 1,(const char**)stringArrayAPLT) );
							tc_strcpy( stringArrayAPLT[0], DesignGroupList[i]);
							CALLAPI( TCTYPE_set_create_display_value( APLTRevCreInTag, "t5_crdesigngroup", 1,(const char**)stringArrayAPLT) );
							tc_strcpy( stringArrayAPLT[0], Proj_Code);
							CALLAPI( TCTYPE_set_create_display_value( APLTRevCreInTag, "t5_cprojectcode", 1,(const char**)stringArrayAPLT) );

							CALLAPI( TCTYPE_create_object( APLTCreInTag, &APLTaskTag) );
							CALLAPI( AOM_save(APLTaskTag) );
							CALLAPI( AOM_unlock(APLTaskTag) );

							CALLAPI(ITEM_ask_latest_rev(APLTaskTag,&APLTaskRevTag));

							printf("\n Item created first time only with item_id \n");fflush(stdout);
							if (APLDMLRevTag != NULLTAG && APLTaskRevTag != NULLTAG)
							 {
								GRM_find_relation_type("T5_DMLTaskRelation",&relation_type);
								GRM_create_relation(APLDMLRevTag, APLTaskRevTag, relation_type,  NULLTAG, &apltaskrelation);
								GRM_save_relation(apltaskrelation);
								printf("\n APL DML Created for ERC DML  : %s\n",item_erc_id_dup);fflush(stdout);
							   if(AssignAPLPlanner(APLTaskRevTag,Proj_Code));
							   if(AssignAPLReviewer(APLTaskRevTag,Proj_Code));

								 /// Solution Item start

								CALLAPI(GRM_list_secondary_objects_only(DMLTag,relation_type,&count,&TaskRevision));
								printf("\n\n\t\t APL DML Cre:ERC DML to Task : %d",count);fflush(stdout);
								for (TaskCnt=0;TaskCnt<count ;TaskCnt++ )
								{
								TaskRevTag = TaskRevision[TaskCnt];
								CALLAPI(AOM_ask_value_string(TaskRevTag,"object_type",&object_type));
								printf("\n\n\t\t object_type is :%s",object_type);fflush(stdout);
								if(strcmp(object_type,"T5_ChangeTaskRevision")==0)
								{
									erc_item_id = NULL;
									AOM_ask_value_string( TaskRevTag, "item_id", &erc_item_id);
                                    if(strstr(item_id_dup,erc_item_id))  // Check the respective ERC task only
									{
										PartCnt=0;
										CALLAPI(AOM_ask_value_tags(TaskRevTag,"CMHasSolutionItem",&PartCnt,&PartTags));
										printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
										if (PartCnt>0)
										{
											GRM_find_relation_type("CMHasSolutionItem",&tsk_part_sol_rel_type);
											for (k=0;k<PartCnt ;k++ )
											{
												printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
												AssyTag=PartTags[k];

												CALLAPI(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
												printf("\n\n\t\t APL DML Cre:Part_no  is :%s",Part_no);	fflush(stdout);

												if(TCTYPE_ask_object_type(AssyTag,&objTypeTag));
												if(TCTYPE_ask_name2(objTypeTag,&type_name1));
												printf("\n\n\t\t APL DML Cre:AssyTag type_name1 := %s", type_name1);fflush(stdout);

												CALLAPI(AOM_ask_value_string(AssyTag,"object_type",&PartTypeStr));
												printf("\n\n\t\t APL DML Cre:AssyTag PartTypeStr := %s", PartTypeStr);fflush(stdout);

												if (tc_strcmp(type_name1,"Design Revision")==0 &&  (AssyTag != NULLTAG && APLTaskRevTag != NULLTAG && tsk_part_sol_rel_type != NULLTAG))
												{

													CALLAPI(GRM_find_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type ,&Fndrelation));
													if(Fndrelation)
													{
														printf("\n\t Relation Already Exist.\n" );fflush(stdout);
													}
													else
													{
													GRM_create_relation(APLTaskRevTag, AssyTag, tsk_part_sol_rel_type,  NULLTAG, &tsk_part_APL_rel);
													GRM_save_relation(tsk_part_APL_rel);
													}
													printf("\n\n\t\t APL DML Cre:Object_PartType is :%s\n",PartTypeStr);fflush(stdout);

												}
											}
										}
								} // End Check the respective ERC task only
								}
							  }  /// Solution Item end

						 }
						 }else
						 {
							printf("\n item_id created twice......  \n");fflush(stdout);
						 }

				   }
			   }
			}//Plant Aggregate
		}
	}else
	   {
	     	printf("\n No proper class class T5_APLDMLRevision......\n");fflush(stdout);
	   }

	return error_code;
}
//End of Function apl_dml_create_appl_desgrp

int AssignAPLPlanner(tag_t APLTaskRevT,char*		Proj_Code)
{
	char*			Task_no				= NULL;
	char*			Dsgn_Grp			= NULL;
	char*			Dsgn_No				= NULL;
	char*			PLT_Code			= NULL;
	char*			APL_Plnr_Grp		= NULL;
	//char*			rolename			= NULL;
	//char*			userid				= NULL;
	char			rolename[SA_name_size_c+1];
	char				userid[SA_user_size_c+1];
	int   ifail				= 0;
	char*			PLT_CodeS		= NULL;


	tag_t			group_tag			= NULLTAG;
	tag_t			user_tag			= NULLTAG;
	tag_t			relation			= NULLTAG;
	tag_t			participant_type	= NULLTAG;
	tag_t			participant_tag		= NULLTAG;
	tag_t			relation_type		= NULLTAG;
	tag_t			role_tag		= NULLTAG;

	tag_t*			role_tags			= NULLTAG;
	tag_t*			member_tags			= NULLTAG;

	int				num_of_roles		= 0;
	int				num_of_members		= 0;
	int				i					= 0;
	int				j					= 0;

	logical			flgRoleFnd			= false;

	printf("\n Inside AssignAPLPlanner \n");

	CALLAPI(AOM_ask_value_string(APLTaskRevT,"item_id",&Task_no));
	CALLAPI(AOM_ask_value_string(APLTaskRevT,"t5_crdesigngroup",&Dsgn_Grp));
	//CALLAPI(AOM_ask_value_string(APLTaskRevT,"t5_cprojectcode",&Proj_Code));
	printf("\n\n\t\t APL DML Cre:AssignAPLPlanner:Task_no  is :%s",Task_no);	fflush(stdout);
	printf("\n\n\t\t APL DML Cre:AssignAPLPlanner:Dsgn_Grp  is :%s",Dsgn_Grp);	fflush(stdout);
	printf("\n\n\t\t APL DML Cre:AssignAPLPlanner:Proj_Code  is :%s",Proj_Code);	fflush(stdout);

	Dsgn_No=tc_strtok(Task_no,"_");
	printf("\nTest0.11..Dsgn_No:[%s],[%s]\n",Dsgn_No,Task_no);
	Dsgn_No = tc_strtok(NULL,"_");
	printf("\nTest0.12..Dsgn_No:[%s],[%s]\n",Dsgn_No,Task_no);
	PLT_Code = tc_strtok(NULL,"_");
	printf("\nTest0.13..PLT_Code:[%s],[%s]\n",PLT_Code,Task_no);

	PLT_CodeS=(char *) MEM_alloc(100 * sizeof(char *));
	if(tc_strcmp(PLT_Code,"APLP")==0)
    {
	tc_strcpy(PLT_CodeS,"PUNE");
	}else
	if(tc_strcmp(PLT_Code,"APLD")==0)
	{
	tc_strcpy(PLT_CodeS,"DWD");
	}else
	if(tc_strcmp(PLT_Code,"APLC")==0)
	{
	tc_strcpy(PLT_CodeS,"CAR");
	}else
	if(tc_strcmp(PLT_Code,"APLJ")==0)
	{
	tc_strcpy(PLT_CodeS,"JSR");
	}else
	if(tc_strcmp(PLT_Code,"APLL")==0)
	{
	tc_strcpy(PLT_CodeS,"LKO");
	}else
	if(tc_strcmp(PLT_Code,"APLA")==0)
	{
	tc_strcpy(PLT_CodeS,"AHD");
	}else
	if(tc_strcmp(PLT_Code,"APLU")==0)
	{
	tc_strcpy(PLT_CodeS,"UTK");
	}else
	if(tc_strcmp(PLT_Code,"APLS")==0)
	{
	tc_strcpy(PLT_CodeS,"JDL");
	}else
	{
	tc_strcpy(PLT_CodeS,"PUNE");
	}

	APL_Plnr_Grp=(char *) MEM_alloc(100 * sizeof(char *));
	tc_strcpy(APL_Plnr_Grp,"APL");
	tc_strcat(APL_Plnr_Grp,PLT_CodeS);
	printf("\nTest0.14..Planner Group Name:[%s]\n",APL_Plnr_Grp);


	CALLAPI(SA_find_group(APL_Plnr_Grp,&group_tag));
	 if (group_tag != NULLTAG)
	CALLAPI(SA_ask_roles_from_group(group_tag,&num_of_roles,&role_tags  ));

	printf("\nNo of Role found in Group are :%d\n",num_of_roles);
	flgRoleFnd=false;
	if(num_of_roles>0)
	{
		for (i=0;i<num_of_roles ;i++ )
		{
				CALLAPI(SA_ask_role_name(role_tags[i],rolename));
				printf("\nRol Name :[%d][%s]\n",i,rolename);
				printf("\n Dsgn_Grp [%s]\n",Dsgn_Grp);
				if(strstr(rolename,"APL") && strstr(rolename,Dsgn_Grp) && strstr(rolename,"Planner"))
				{
					printf("\nMatch Found\n");
					flgRoleFnd=true;
			     	if (flgRoleFnd==true)
					{
							role_tag = role_tags[i];
							CALLAPI(SA_find_groupmember_by_role(role_tag,group_tag,&num_of_members,&member_tags));
							printf("\nNo of Group Members found in Group/Role are :%d\n",num_of_members);
							if(num_of_members>0)
							{
								for (j=0;j<num_of_members ;j++  )
								{
									CALLAPI(SA_ask_groupmember_user(member_tags[j],&user_tag));
									CALLAPI(SA_ask_user_identifier(user_tag,userid));
									printf("\nUser ID Name :[%s]\n",userid);
									TCTYPE_find_type("Analyst", "Analyst", &participant_type);
									EPM_create_participant(member_tags[j], participant_type, &participant_tag);
									GRM_find_relation_type("HasParticipant", &relation_type);
									GRM_create_relation(APLTaskRevT, participant_tag, relation_type,  NULLTAG, &relation);
									GRM_save_relation(relation);
									printf("\n TestParticipent Assigned..DoneXXXX\n");
								}
							}else  //default group
							{
							APL_Plnr_Grp = NULL;
							group_tag = NULLTAG;
							num_of_roles = 0;
							APL_Plnr_Grp=(char *) MEM_alloc(100 * sizeof(char *));
							tc_strcpy(APL_Plnr_Grp,"APL");
							tc_strcat(APL_Plnr_Grp,"_");
							tc_strcat(APL_Plnr_Grp,PLT_CodeS);
							tc_strcat(APL_Plnr_Grp,"_");
							tc_strcat(APL_Plnr_Grp,"Default");
							printf("\nTest0.14..Planner Group Name:[%s]\n",APL_Plnr_Grp);

							CALLAPI(SA_find_group(APL_Plnr_Grp,&group_tag));

							 if (group_tag != NULLTAG)
							CALLAPI(SA_ask_roles_from_group(group_tag,&num_of_roles,&role_tags  ));
							printf("\nNo of Role found in Group are :%d\n",num_of_roles);
							flgRoleFnd=false;
							if(num_of_roles>0)
							{
								for (i=0;i<num_of_roles ;i++ )
								{
										CALLAPI(SA_ask_role_name(role_tags[i],rolename));
										printf("\nRol Name :[%d][%s]\n",i,rolename);
										if(strstr(rolename,"APL") )
										{
											printf("\nMatch Found\n");
											flgRoleFnd=true;
											break;
										}
								}

								if (flgRoleFnd==true)
								{
									CALLAPI(SA_find_groupmember_by_role(role_tags[i],group_tag,&num_of_members,&member_tags));
									printf("\nNo of Group Members found in Group/Role are :%d\n",num_of_roles);
									if(num_of_members>0)
									{
										for (j=0;j<num_of_members ;j++  )
										{
											CALLAPI(SA_ask_groupmember_user(member_tags[j],&user_tag));
											CALLAPI(SA_ask_user_identifier(user_tag,userid));
											printf("\nUser ID Name :[%s]\n",userid);
											TCTYPE_find_type("Analyst", "Analyst", &participant_type);
											EPM_create_participant(member_tags[j], participant_type, &participant_tag);
											GRM_find_relation_type("HasParticipant", &relation_type);
											GRM_create_relation(APLTaskRevT, participant_tag, relation_type,  NULLTAG, &relation);
											GRM_save_relation(relation);
											printf("\n TestParticipent Assigned..DoneXXXX\n");
										}
									}
								}
							}
						}
					}
					break;
				}  // Role matches
		}

	}else //default group
	{
			APL_Plnr_Grp = NULL;
			group_tag = NULLTAG;
			num_of_roles = 0;
			APL_Plnr_Grp=(char *) MEM_alloc(100 * sizeof(char *));
			tc_strcpy(APL_Plnr_Grp,"APL");
			tc_strcat(APL_Plnr_Grp,"_");
			tc_strcat(APL_Plnr_Grp,PLT_CodeS);
			tc_strcat(APL_Plnr_Grp,"_");
			tc_strcat(APL_Plnr_Grp,"Default");
			printf("\nTest0.14..Planner Group Name:[%s]\n",APL_Plnr_Grp);

			CALLAPI(SA_find_group(APL_Plnr_Grp,&group_tag));

			 if (group_tag != NULLTAG)
			CALLAPI(SA_ask_roles_from_group(group_tag,&num_of_roles,&role_tags  ));
			printf("\nNo of Role found in Group are :%d\n",num_of_roles);
			flgRoleFnd=false;
			if(num_of_roles>0)
			{
				for (i=0;i<num_of_roles ;i++ )
				{
						CALLAPI(SA_ask_role_name(role_tags[i],rolename));
						printf("\nRol Name :[%d][%s]\n",i,rolename);
			         	if(strstr(rolename,"APL"))
						{
							printf("\nMatch Found\n");
							flgRoleFnd=true;
							break;
						}
				}

				if (flgRoleFnd==true)
				{
					CALLAPI(SA_find_groupmember_by_role(role_tags[i],group_tag,&num_of_members,&member_tags));
					printf("\nNo of Group Members found in Group/Role are :%d\n",num_of_roles);
					if(num_of_members>0)
					{
						for (j=0;j<num_of_members ;j++  )
						{
							CALLAPI(SA_ask_groupmember_user(member_tags[j],&user_tag));
							CALLAPI(SA_ask_user_identifier(user_tag,userid));
							printf("\nUser ID Name :[%s]\n",userid);
							TCTYPE_find_type("Analyst", "Analyst", &participant_type);
							EPM_create_participant(member_tags[j], participant_type, &participant_tag);
							GRM_find_relation_type("HasParticipant", &relation_type);
							GRM_create_relation(APLTaskRevT, participant_tag, relation_type,  NULLTAG, &relation);
							GRM_save_relation(relation);
							printf("\n TestParticipent Assigned..DoneXXXX\n");
						}
					}
				}
	        }
	}
	return ITK_ok;
}

int AssignAPLReviewer(tag_t APLTaskRevT,char*		Proj_Code)
{
	char*			Task_no				= NULL;
	char*			Dsgn_Grp			= NULL;
//	char*			Proj_Code			= NULL;
	char*			Dsgn_No				= NULL;
	char*			PLT_Code			= NULL;
	char*			APL_Plnr_Grp		= NULL;
	//char*			rolename			= NULL;
	//char*			userid				= NULL;
	char			rolename[SA_name_size_c+1];
	char				userid[SA_user_size_c+1];
	int   ifail				= 0;
	char*			PLT_CodeS		= NULL;


	tag_t			group_tag			= NULLTAG;
	tag_t			user_tag			= NULLTAG;
	tag_t			relation			= NULLTAG;
	tag_t			participant_type	= NULLTAG;
	tag_t			participant_tag		= NULLTAG;
	tag_t			relation_type		= NULLTAG;

	tag_t*			role_tags			= NULLTAG;
	tag_t			role_tag			= NULLTAG;
	tag_t*			member_tags			= NULLTAG;

	int				num_of_roles		= 0;
	int				num_of_members		= 0;
	int				i					= 0;
	int				j					= 0;

	logical			flgRoleFnd			= false;

	printf("\n Inside AssignAPLReviewer  \n");

	CALLAPI(AOM_ask_value_string(APLTaskRevT,"item_id",&Task_no));
	CALLAPI(AOM_ask_value_string(APLTaskRevT,"t5_crdesigngroup",&Dsgn_Grp));
	//CALLAPI(AOM_ask_value_string(APLTaskRevT,"t5_cprojectcode",&Proj_Code));
	printf("\n\n\t\t APL DML Cre:AssignAPLReviewer:Task_no  is :%s",Task_no);	fflush(stdout);
	printf("\n\n\t\t APL DML Cre:AssignAPLReviewer:Dsgn_Grp  is :%s",Dsgn_Grp);	fflush(stdout);
	printf("\n\n\t\t APL DML Cre:AssignAPLReviewer:Proj_Code  is :%s",Proj_Code);	fflush(stdout);

	Dsgn_No=tc_strtok(Task_no,"_");
	printf("\nTest0.11..Dsgn_No:[%s],[%s]\n",Dsgn_No,Task_no);
	Dsgn_No = tc_strtok(NULL,"_");
	printf("\nTest0.12..Dsgn_No:[%s],[%s]\n",Dsgn_No,Task_no);
	PLT_Code = tc_strtok(NULL,"_");
	printf("\nTest0.13..PLT_Code:[%s],[%s]\n",PLT_Code,Task_no);

	PLT_CodeS=(char *) MEM_alloc(100 * sizeof(char *));
	if(tc_strcmp(PLT_Code,"APLP")==0)
    {
	tc_strcpy(PLT_CodeS,"PUNE");
	}else
	if(tc_strcmp(PLT_Code,"APLD")==0)
	{
	tc_strcpy(PLT_CodeS,"DWD");
	}else
	if(tc_strcmp(PLT_Code,"APLC")==0)
	{
	tc_strcpy(PLT_CodeS,"CAR");
	}else
	if(tc_strcmp(PLT_Code,"APLJ")==0)
	{
	tc_strcpy(PLT_CodeS,"JSR");
	}else
	if(tc_strcmp(PLT_Code,"APLL")==0)
	{
	tc_strcpy(PLT_CodeS,"LKO");
	}else
	if(tc_strcmp(PLT_Code,"APLA")==0)
	{
	tc_strcpy(PLT_CodeS,"AHD");
	}else
	if(tc_strcmp(PLT_Code,"APLU")==0)
	{
	tc_strcpy(PLT_CodeS,"UTK");
	}else
	if(tc_strcmp(PLT_Code,"APLS")==0)
	{
	tc_strcpy(PLT_CodeS,"JDL");
	}else
	{
	tc_strcpy(PLT_CodeS,"PUNE");
	}

	APL_Plnr_Grp=(char *) MEM_alloc(100 * sizeof(char *));
	tc_strcpy(APL_Plnr_Grp,"APL");
	tc_strcat(APL_Plnr_Grp,PLT_CodeS);
	printf("\nTest0.14..Planner Group Name:[%s]\n",APL_Plnr_Grp);


	CALLAPI(SA_find_group(APL_Plnr_Grp,&group_tag));

	 if (group_tag != NULLTAG)
	CALLAPI(SA_ask_roles_from_group(group_tag,&num_of_roles,&role_tags  ));

	printf("\nNo of Role found in Group are :%d\n",num_of_roles);
	flgRoleFnd=false;
	if(num_of_roles>0)
	{
		for (i=0;i<num_of_roles ;i++ )
		{
				CALLAPI(SA_ask_role_name(role_tags[i],rolename));
				printf("\nRol Name :[%d][%s]\n",i,rolename);
				printf("\n Dsgn_Grp [%s]\n",Dsgn_Grp);
				if(strstr(rolename,"APL") && strstr(rolename,Dsgn_Grp) && strstr(rolename,"Reviewer"))
				{
					printf("\nMatch Found\n");
					flgRoleFnd=true;
					if (flgRoleFnd==true)
						{
							role_tag = role_tags[i];
							CALLAPI(SA_find_groupmember_by_role(role_tag,group_tag,&num_of_members,&member_tags));
							printf("\nNo of Group Members found in Group/Role are :%d\n",num_of_members);
							if(num_of_members>0)
							{
								for (j=0;j<num_of_members ;j++  )
								{
									CALLAPI(SA_ask_groupmember_user(member_tags[j],&user_tag));
									CALLAPI(SA_ask_user_identifier(user_tag,userid));
									printf("\nUser ID Name :[%s]\n",userid);
									TCTYPE_find_type("ChangeSpecialist1", "ChangeSpecialist1", &participant_type);
									EPM_create_participant(member_tags[j], participant_type, &participant_tag);
									GRM_find_relation_type("HasParticipant", &relation_type);
									GRM_create_relation(APLTaskRevT, participant_tag, relation_type,  NULLTAG, &relation);
									GRM_save_relation(relation);
									printf("\n TestParticipent Assigned..DoneXXXX\n");
								}
							}else
							{
									APL_Plnr_Grp = NULL;
									group_tag = NULLTAG;
									num_of_roles = 0;
									APL_Plnr_Grp=(char *) MEM_alloc(100 * sizeof(char *));
									tc_strcpy(APL_Plnr_Grp,"APL");
									tc_strcat(APL_Plnr_Grp,"_");
									tc_strcat(APL_Plnr_Grp,PLT_CodeS);
									tc_strcat(APL_Plnr_Grp,"_");
									tc_strcat(APL_Plnr_Grp,"Default");
									printf("\nTest0.14..Planner Group Name:[%s]\n",APL_Plnr_Grp);

									CALLAPI(SA_find_group(APL_Plnr_Grp,&group_tag));

									 if (group_tag != NULLTAG)
									CALLAPI(SA_ask_roles_from_group(group_tag,&num_of_roles,&role_tags  ));
									printf("\nNo of Role found in Group are :%d\n",num_of_roles);
									flgRoleFnd=false;
									if(num_of_roles>0)
									{
										for (i=0;i<num_of_roles ;i++ )
										{
												CALLAPI(SA_ask_role_name(role_tags[i],rolename));
												printf("\nRol Name :[%d][%s]\n",i,rolename);
												if(strstr(rolename,"APL"))
												{
													printf("\nMatch Found\n");
													flgRoleFnd=true;
													break;
												}
										}

										if (flgRoleFnd==true)
										{
											CALLAPI(SA_find_groupmember_by_role(role_tags[i],group_tag,&num_of_members,&member_tags));
											printf("\nNo of Group Members found in Group/Role are :%d\n",num_of_roles);
											if(num_of_members>0)
											{
												for (j=0;j<num_of_members ;j++  )
												{
													CALLAPI(SA_ask_groupmember_user(member_tags[j],&user_tag));
													CALLAPI(SA_ask_user_identifier(user_tag,userid));
													printf("\nUser ID Name :[%s]\n",userid);
													TCTYPE_find_type("ChangeSpecialist1", "ChangeSpecialist1", &participant_type);
													EPM_create_participant(member_tags[j], participant_type, &participant_tag);
													GRM_find_relation_type("HasParticipant", &relation_type);
													GRM_create_relation(APLTaskRevT, participant_tag, relation_type,  NULLTAG, &relation);
													GRM_save_relation(relation);
													printf("\n TestParticipent Assigned..DoneXXXX\n");
												}
											}
									}
							  }
							}
						}
					break;
				}  // Roll matches
		}

	}else
	{
			APL_Plnr_Grp = NULL;
			group_tag = NULLTAG;
			num_of_roles = 0;
			APL_Plnr_Grp=(char *) MEM_alloc(100 * sizeof(char *));
			tc_strcpy(APL_Plnr_Grp,"APL");
			tc_strcat(APL_Plnr_Grp,"_");
			tc_strcat(APL_Plnr_Grp,PLT_CodeS);
			tc_strcat(APL_Plnr_Grp,"_");
			tc_strcat(APL_Plnr_Grp,"Default");
			printf("\nTest0.14..Planner Group Name:[%s]\n",APL_Plnr_Grp);

			CALLAPI(SA_find_group(APL_Plnr_Grp,&group_tag));

			 if (group_tag != NULLTAG)
			CALLAPI(SA_ask_roles_from_group(group_tag,&num_of_roles,&role_tags  ));
			printf("\nNo of Role found in Group are :%d\n",num_of_roles);
			flgRoleFnd=false;
			if(num_of_roles>0)
			{
				for (i=0;i<num_of_roles ;i++ )
				{
						CALLAPI(SA_ask_role_name(role_tags[i],rolename));
						printf("\nRol Name :[%d][%s]\n",i,rolename);
				        if(strstr(rolename,"APL"))
						{
							printf("\nMatch Found\n");
							flgRoleFnd=true;
							break;
						}
				}

				if (flgRoleFnd==true)
				{
					CALLAPI(SA_find_groupmember_by_role(role_tags[i],group_tag,&num_of_members,&member_tags));
					printf("\nNo of Group Members found in Group/Role are :%d\n",num_of_roles);
					if(num_of_members>0)
					{
						for (j=0;j<num_of_members ;j++  )
						{
							CALLAPI(SA_ask_groupmember_user(member_tags[j],&user_tag));
							CALLAPI(SA_ask_user_identifier(user_tag,userid));
							printf("\nUser ID Name :[%s]\n",userid);
							TCTYPE_find_type("ChangeSpecialist1", "ChangeSpecialist1", &participant_type);
							EPM_create_participant(member_tags[j], participant_type, &participant_tag);
							GRM_find_relation_type("HasParticipant", &relation_type);
							GRM_create_relation(APLTaskRevT, participant_tag, relation_type,  NULLTAG, &relation);
							GRM_save_relation(relation);
							printf("\n TestParticipent Assigned..DoneXXXX\n");
						}
					}
	        }
	  }
	}
	return ITK_ok;

	//printf("\n Inside AssignAPLReviewer \n");

	///return 0;
}

extern int DLLAPI apl_dml_DriverVc_attach( METHOD_message_t *msg, va_list  args )
{
    tag_t	TaskTag		= va_arg(args, const tag_t);
	tag_t objTypeTag=NULLTAG;
	char   type_name[TCTYPE_name_size_c+1];

	char *item_id   = NULL;
	char *DML_no = NULL;
	char *EcnType = NULL;
	char *DriverVC = NULL;
	char *ReleaseStatus = NULL;
	char *part_type = NULL;
	int n_tags_found = 0;
	int revCnt = 0;
	int stlst = 0;
	int rlzFlag = 0;
	int status_count = 0;
	tag_t *tags_found = NULL;
	tag_t *status_list;
	tag_t  CurrentRoleTag = NULLTAG;
	char roleName[SA_name_size_c+1];
	char PlantCS[40];
	char PlantIA[40];
	char PlantStore[40];
	char UserAgency[40];
	char PlantName[40];
	char PlantOptCS[40];

	tag_t item = NULLTAG;
	tag_t	item_rev_tag							= NULLTAG;
	int error_code	= ITK_ok;
	tag_t	itemcldclass							= NULLTAG;
	tag_t  	item_rev_cld_tag;
    int status;

	if(TCTYPE_ask_object_type(TaskTag,&objTypeTag));
	if(TCTYPE_ask_name(objTypeTag,type_name));
	printf("\n     type_name for apl dml object %s\n", type_name);fflush(stdout);


	ITKCALL(SA_ask_current_role(&CurrentRoleTag));
	ITKCALL(SA_ask_role_name(CurrentRoleTag,roleName))
	printf("\n\n  roleName : %s\n",roleName); fflush(stdout);

	//UserAgency=subString(roleName,0,4);
    getPlantDetailsAttr(roleName,PlantCS,PlantOptCS,PlantIA,PlantStore,UserAgency);
	//PlantName=subString(roleName,0,4);

	printf("\n PlantCS %s \n",PlantCS);
	printf("\n PlantAgency %s \n",PlantIA);
	printf("\n PlantStore %s \n",PlantStore);
	printf("\n UserAgency %s \n",UserAgency);

	if(strcmp(type_name,"T5_APLDMLRevision")==0)
	{
		printf("\n inside class ChangeRequestRevision......\n");fflush(stdout);
		AOM_ask_value_string( TaskTag, "item_id", &item_id);
		AOM_ask_value_string( TaskTag, "current_id", &DML_no);
		AOM_ask_value_string( TaskTag,"t5_EcnType",&EcnType);
		AOM_ask_value_string( TaskTag,"t5_DriverVC",&DriverVC);

		printf("\n item_id : %s\n",item_id);fflush(stdout);
		printf("\n DML_no : %s\n",DML_no);fflush(stdout);
		printf("\n EcnType : %s\n",EcnType);fflush(stdout);
		printf("\n DriverVC : %s\n",DriverVC);fflush(stdout);

		if(tc_strcmp(EcnType,"")!=0)
		{
				printf("\n EcnType is not null...\n");
				if(tc_strstr(DML_no,"AM")!=NULL)
				{
					printf("\n Inside AM DML..\n");
					if(tc_strstr(roleName,"APL")!=NULL)
					{
						if((tc_strcmp(EcnType,"APLSTR")==0) || (tc_strcmp(EcnType,"APLMBOMRES")==0) )
						{
							if(tc_strcmp(DriverVC,"")==0)
							{
								printf("\n Driver VC Value is null..\n");
								EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Please enter the Driver VC Number for APL-Restructuring DML.");
								return ITK_errStore1;
							}
							else
							{
									const char *qry_entries[1];
									const char *qry_values[1];

									qry_entries[0] ="item_id";
									qry_values[0] = DriverVC;

									ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found, &tags_found);
									printf("\n\n\t\t Item_id count in DB is : %d\n",n_tags_found);fflush(stdout);

									if(n_tags_found==0)
									{
										EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Please enter valid Driver VC Number.");
										return ITK_errStore1;

									}
									else
									{
										itemcldclass = tags_found[0];
										ITKCALL(ITEM_ask_latest_rev(itemcldclass,&item_rev_cld_tag));

										//for(revCnt=0;revCnt<n_tags_found;revCnt++)
										//{
											status=WSOM_ask_release_status_list(item_rev_cld_tag,&status_count,&status_list);
											printf("\n number of statuses: %d \n",  status_count);
											for (stlst = 0; stlst < status_count; stlst++)
											{
												AOM_ask_name(status_list[stlst], &ReleaseStatus);
												printf("\n ReleaseStatus %s \n",  ReleaseStatus);
												if (tc_strcmp(ReleaseStatus,"NULL")!=0)
												{
													if((tc_strcmp(ReleaseStatus,"T5_LcsAplRlzd")==0)||(tc_strcmp(ReleaseStatus,"T5_LcsSTDWrkg")==0)||(tc_strcmp(ReleaseStatus,"T5_LcsStdRlzd")==0))
													{
														rlzFlag++;
														//break;

													}
												}
												if(rlzFlag>0)
												{
													break;
												}
											}

											ITKCALL(AOM_ask_value_string(item_rev_cld_tag, "t5_PartType", &part_type));
										//}
										if(rlzFlag==0)
										{
											EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Invalid Driver VC Number, Check for Life Cycle.Lifecyle should be APL Released/STDSI Working/STDSI Released.");
											return ITK_errStore1;
										}

									 }

									if(rlzFlag>0)
									{
										//item = status_list[0];

										//ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag));
										if(tc_strcmp(UserAgency,"JDL")==0)
										{
											if ((tc_strcmp(part_type,"T")==0) ||(tc_strcmp(part_type,"M")==0)||(tc_strcmp(part_type,"A")==0))
											{
												printf("\n Driver VC Part Type: %s \n",  part_type);

											}
											else
											{
												EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Driver VC is not TPL Type.");
												return ITK_errStore1;

											}

										}
										else
										{
											if ((tc_strcmp(part_type,"V")==0)||(tc_strcmp(part_type,"VC")==0)||(tc_strcmp(part_type,"VCCR")==0))
											{
												printf("\n Driver VC Part Type: %s \n",  part_type);

											}
											else
											{
												EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Driver VC Type is not VC/Vehicle/Vehicle Combination CR.");
												return ITK_errStore1;

											}


										}


									}


							}

						}

				}
				else
				{
					EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"APL User are allowed to create AMDML.");
					return ITK_errStore1;
				}
			}
		}

	}
	return error_code;
}

extern int DLLAPI apl_dml_part_attach( METHOD_message_t *msg, va_list  args )
{

		tag_t objTypeTag=NULLTAG;
		tag_t objTypeTag2=NULLTAG;
		tag_t tsk_dml_rel_type=NULLTAG;
		tag_t tsk_dml_rel_type_1=NULLTAG;
		tag_t *DMLRevision = NULLTAG;
		tag_t *DMLRevision_Prt = NULLTAG;
		tag_t DMLRevTag = NULLTAG;
		tag_t DMLRevTag_Prt = NULLTAG;
		tag_t class_id1=NULLTAG;
		tag_t*    status_list1=NULLTAG;
		tag_t    user_tag=NULLTAG;
		tag_t*    member_tags=NULLTAG;
		tag_t*    status_list_nextRev=NULLTAG;
		tag_t	latestrev	=	NULLTAG;
		tag_t  	master =	NULLTAG;
		tag_t*    status_list;
		tag_t*    dml_status_list;
		tag_t group_tag = NULLTAG;
		tag_t group_tag1 = NULLTAG;
		tag_t role_tag = NULLTAG;
		tag_t			tRole=NULLTAG;

		int error_code	= ITK_ok;

		char   type_name[TCTYPE_name_size_c+1];
		char   type_name2[TCTYPE_name_size_c+1];
		char   relation_name[TCTYPE_name_size_c+1];

		char *part_DRStatus		=NULL;
		char *part_name		=NULL;
		char *part_type		=NULL;
		char *owning_group_val		=NULL;
		char *part_organization		=NULL;
		char *part_DesignGrp		=NULL;
		char *class_name		=NULL;
		char *part_MakeBuy		=NULL;
		char *Desc_obj		=NULL;
		char*	 DMLtype		= NULL;
		char*	 DMLDRStatus		= NULL;
		char*	 DMLNumber		= NULL;
		char*	 Desc_obj_drop_prt		= NULL;
		char*	 group_name		= NULL;
		char			roleName[SA_name_size_c+1];
		char*	 part_creator		= NULL;
		char *WSO_Name = NULL;
		char *WSO_Name_next_Rev = NULL;
		tag_t  CurrentRoleTag = NULLTAG;
        char       roleName_crt[SA_name_size_c+1];
		char PlantCS[40];
		char PlantIA[40];
		char PlantStore[40];
		char UserAgency[40];
		char OrgId[40];
		char dmlnumberCpy[20];
		char dmlnumberCpy_val[20];
		char ConndmlnumberCpy_val[20];
		char		   *DMLNo =NULL;
		char		   *DMLNoCpy =NULL;
		char		   *ConntDMLNoCpy =NULL;
		char		   *PlantName =NULL;
		char		   *PlantNameCpy =NULL;
		char		   *ConntPlantNameCpy =NULL;
		char		   *partDmlType =NULL;
		char *  ErrMssg = NULL;
			char*	DmlId;
				char *  AMDMLErr = NULL;
				logical is_latest_dml;



	       char PlantOptCS[40];

		char *class_name1= NULL;

		int count = 0;
		int st_count1 = 0;
		int st_count_next_rev = 0;
		int cnt_next = 0;
		int j = 0;
		int cnt = 0;
		int errorFlag = 0;
		int NRCheCkBPS = 0;
		int count_dml = 0;
		int PlantDMLActiveFlg = 0;
		int dml = 0;
		int num_of_members = 0;
		int IsAPLStrDMLRlzErr = 0;
		int st_count = 0;
		int st_count_dml = 0;
		int j_mem = 0;
		int ercUsrFnd = 0;
		int aplUsrFnd = 0;
		int dmlstlst = 0;
		int count_dml_1 = 0;
		int t5AplPrtwithAPLWorkFlag=0;
		int samePlantPrtNotFound = 0;
		int samePlantPrtFound = 0;
		int validERCLCSFound=0;
		int	validAPLLCSFound=0;
		int	validSTDLCSFound=0;
		tag_t	relation_type_task = NULLTAG;
		tag_t*			PartTaskTags			= NULLTAG;
		tag_t			PartDmlTag				= NULLTAG;
		tag_t	itemTypeTag_class							= NULLTAG;
		char  type_class[TCTYPE_name_size_c+1];
		char		   *creatorlogin =NULL;
		char		   *ReleaseStatusDML =NULL;

		char *taskTempLT = NULL; //ADDED BY DEEPTI TZ1.34
		char *username= NULL;//ADDED BY DEEPTI TZ1.34
		char *TaskAnlyst= NULL;//ADDED BY DEEPTI TZ1.34
		char *TaskSpec= NULL;//ADDED BY DEEPTI TZ1.34

		//char   *UserAgency=NULL;
		tag_t  primary_object = va_arg(args, tag_t);
		tag_t  secondary_object = va_arg(args, tag_t);
		tag_t  relation_type = va_arg(args, tag_t);

		ITKCALL(SA_ask_current_role(&CurrentRoleTag));
		ITKCALL(SA_ask_role_name(CurrentRoleTag,roleName))
		printf("\n\n  roleName : %s\n",roleName); fflush(stdout);

		//UserAgency=subString(roleName,0,4);
		getPlantDetailsAttr(roleName,PlantCS,PlantOptCS,PlantIA,PlantStore,UserAgency);

		printf("\n PlantCS %s \n",PlantCS);fflush(stdout);
		printf("\n PlantAgency %s \n",PlantIA);fflush(stdout);
		printf("\n PlantStore %s \n",PlantStore);fflush(stdout);
		printf("\n UserAgency %s \n",UserAgency);fflush(stdout);
		//printf("\n OrgId %s \n",OrgId);


		printf("\n\t Calling A9_ValidationAtAMDML running.\n"); fflush(stdout);

		ITKCALL(TCTYPE_ask_object_type(primary_object,&objTypeTag));
		ITKCALL(TCTYPE_ask_name(objTypeTag,type_name));
		printf("\n     A9_ValidationAtAMDML primary_object  type_name :: %s\n", type_name); fflush(stdout);

		ITKCALL(TCTYPE_ask_object_type(secondary_object,&objTypeTag2));
		ITKCALL(TCTYPE_ask_name(objTypeTag2,type_name2));
		printf("\n     A9_ValidationAtAMDML secondary_object  type_name2 :: %s\n", type_name2); fflush(stdout);


			if(tc_strcmp(type_name,"T5_APLTaskRevision")==0)
			{


				ITKCALL(GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type));
				if (tsk_dml_rel_type!=NULLTAG)
				{
					ITKCALL(GRM_list_primary_objects_only(primary_object,tsk_dml_rel_type,&count,&DMLRevision));
					printf("\n\t\t APL DML Revision from Task : %d",count); fflush(stdout);
				}


				for (j=0;j<count;j++ )
				{
					DMLRevTag = DMLRevision[j];

					ITKCALL(POM_class_of_instance(DMLRevTag,&class_id1));
					ITKCALL(POM_name_of_class(class_id1,&class_name1));
					printf("\n\t class_name found1 :%s",class_name1); fflush(stdout);

					if(tc_strcmp(class_name1,"T5_APLDMLRevision")==0)
					{
						ITKCALL(AOM_ask_value_string(DMLRevTag,"t5_EcnType",&DMLtype));
						printf("\n\t APLDMLtype is :%s",DMLtype); fflush(stdout);

						ITKCALL(AOM_ask_value_string(DMLRevTag,"t5_cDRstatus",&DMLDRStatus));
						printf("\n\t DMLDRStatus is :%s",DMLDRStatus); fflush(stdout);

						ITKCALL(AOM_ask_value_string(DMLRevTag,"item_id",&DMLNumber));
						printf("\n\t DMLNumber is :%s",DMLNumber); fflush(stdout);

						break;
					}


				}


				if(relation_type != NULLTAG)
				{
					ITKCALL(TCTYPE_ask_name( relation_type, relation_name));
				}
				printf("\n   relation_name  A9_ValidationAtAMDML CMHasSolutionItem:%s \n",relation_name); fflush(stdout);

				if(tc_strcmp(relation_name,"CMHasSolutionItem")==0)
				{
					printf("\n     Inside attaching part to APL-AM DML task \n"); fflush(stdout);
					if(tc_strcmp(type_name2,"Design Revision")==0)
					{

						if( AOM_ask_value_string(secondary_object,"current_id",&part_name)==ITK_ok)
						printf("\n part_name Value :--------------------   %s\n",part_name); fflush(stdout);

						if( AOM_ask_value_string(secondary_object,"t5_PartStatus",&part_DRStatus)==ITK_ok)
						printf("\n part_DRStatus Value :--------------------   %s\n",part_DRStatus); fflush(stdout);


						if( AOM_ask_value_string(secondary_object,"t5_PartType",&part_type)==ITK_ok)
						printf("\n 111 part_type Value :--------------------   %s\n",part_type); fflush(stdout);


						if( AOM_UIF_ask_value(secondary_object,"owning_group",&owning_group_val)==ITK_ok)
						printf("\n after owning_group_val Value :--------------------   %s\n",owning_group_val); fflush(stdout);

						if( AOM_ask_value_string(secondary_object,"t5_DesignGrp",&part_DesignGrp)==ITK_ok)
						printf("\n part_DesignGrp Value :--------------------   %s\n",part_DesignGrp); fflush(stdout);

						if (AOM_ask_value_string(secondary_object,"current_revision_id",&Desc_obj_drop_prt)==ITK_ok)
						printf("\n Desc_obj_drop_prt Value :--------------------   %s\n",Desc_obj_drop_prt); fflush(stdout);


						if(ITEM_ask_item_of_rev  ( secondary_object,&master) );
						if(ITEM_ask_latest_rev(master,&latestrev));

						cnt=0;
						WSO_Name=NULL;

						ITKCALL(WSOM_ask_release_status_list(secondary_object,&st_count1,&status_list1));
						printf("\n 111..st_count1 :%d is\n",st_count1);fflush(stdout);




					}

					if(tc_strcmp(DMLtype,"")!=0)
					{
						printf("\n DMLtype is not null...\n"); fflush(stdout);
						if(tc_strstr(roleName,"APL")!=NULL)
						{
							if(tc_strstr(DMLNumber,"AM")!=NULL)
							{
								printf("\n DMLNumber is AM...\n");fflush(stdout);

								if( AOM_UIF_ask_value(primary_object,"fnd0ActuatedInteractiveTsks",&taskTempLT)==ITK_ok); //ADDED BY DEEPTI 1.34
								printf("\n primary taskTempLT is .. Add Part: %s\n",taskTempLT);fflush(stdout);

								if( AOM_UIF_ask_value(primary_object,"Analyst",&TaskAnlyst)==ITK_ok); //ADDED BY DEEPTI 1.34
								printf("\n TaskAnlyst: %s\n",TaskAnlyst);fflush(stdout);

								if( AOM_UIF_ask_value(primary_object,"ChangeSpecialist1",&TaskSpec)==ITK_ok); //ADDED BY DEEPTI 1.34
								printf("\n TaskSpec: %s\n",TaskSpec);fflush(stdout);

								if(POM_get_user_id (&username)==ITK_ok);
								printf("\n\n  username : %s\n",username); fflush(stdout);

								if( (tc_strcmp(taskTempLT,"")!=0)&& (tc_strstr(taskTempLT,"Work On DML")!=NULL)) //ADDED BY DEEPTI 1.34
								{

									if(tc_strstr(TaskAnlyst,username) ==NULL)
									{

										EMH_clear_errors();
										EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Session User is Not Task Analyst..Only Analyst Can Add Part To APL DML Task.");

										return ITK_errStore1;

									}
									if(tc_strcmp(DMLDRStatus,"")!=0)
									{
									/*************Start of DR Validation********************/
									if( (tc_strcmp(DMLDRStatus,"DR4")==0)|| (tc_strcmp(DMLDRStatus,"DR5")==0) || (tc_strcmp(DMLDRStatus,"AR4")==0) || (tc_strcmp(DMLDRStatus,"AR5")==0))
									{
										if( (tc_strcmp(part_DRStatus,"DR4")==0)|| (tc_strcmp(part_DRStatus,"DR5")==0) || (tc_strcmp(part_DRStatus,"AR4")==0) || (tc_strcmp(part_DRStatus,"AR5")==0))
										{
											printf("Correct...In APL AMDML with Status DR4/DR5/AR4/AR5 only Design Revision with DR Status DR4/DR5/AR4/AR5 allowed."); fflush(stdout);

										}
										else
										{

											printf("In APL AMDML with Status DR4/DR5/AR4/AR5 only Design Revision with DR Status DR4/DR5/AR4/AR5 allowed."); fflush(stdout);
											if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
											AMDMLErr=(char *)MEM_alloc(500);
											tc_strcpy(AMDMLErr,"Part ");
											tc_strcat(AMDMLErr,part_name);
											tc_strcat(AMDMLErr," is with DR ");
											tc_strcat(AMDMLErr,part_DRStatus);
											//tc_strcat(AMDMLErr," status Not Allowed in APL Restructuring DML.\n");
											tc_strcat(AMDMLErr," status Not Allowed in AM DML of EcnType:");
											tc_strcat(AMDMLErr,DMLtype);
											tc_strcat(AMDMLErr,".\n");
											tc_strcat(AMDMLErr,"In APL DML with Status DR4/DR5/AR4/AR5 only Design Revision with DR Status DR4/DR5/AR4/AR5 allowed");

											EMH_clear_errors();
											EMH_store_error_s1(EMH_severity_error,ITK_errStore1,AMDMLErr);
											return ITK_errStore1;
										}

									}

									if( (tc_strcmp(DMLDRStatus,"DR0")==0)|| (tc_strcmp(DMLDRStatus,"DR1")==0) || (tc_strcmp(DMLDRStatus,"DR2")==0)|| (tc_strcmp(DMLDRStatus,"DR3")==0)|| (tc_strcmp(DMLDRStatus,"AR0")==0) || (tc_strcmp(DMLDRStatus,"AR1")==0)|| (tc_strcmp(DMLDRStatus,"AR2")==0)|| (tc_strcmp(DMLDRStatus,"AR3")==0))
									{
										if( (tc_strcmp(part_DRStatus,"DR0")==0)|| (tc_strcmp(part_DRStatus,"DR1")==0) || (tc_strcmp(part_DRStatus,"DR2")==0)|| (tc_strcmp(part_DRStatus,"DR3")==0)|| (tc_strcmp(part_DRStatus,"AR0")==0) || (tc_strcmp(part_DRStatus,"AR1")==0)|| (tc_strcmp(part_DRStatus,"AR2")==0)|| (tc_strcmp(part_DRStatus,"AR3")==0))
										{
											printf("Correct..In APL AMDML with Status DR0/DR1/DR2/DR3/AR0/AR1/AR2/AR3  only Design Revision with DR Status DR0/DR1/DR2/DR3/AR0/AR1/AR2/AR3  allowed."); fflush(stdout);

										}
										else
										{

											printf("In APL DML with Status DR0/DR1/DR2/DR3/AR0/AR1/AR2/AR3  only Design Revision with DR Status DR0/DR1/DR2/DR3/AR0/AR1/AR2/AR3  allowed."); fflush(stdout);
											if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
											AMDMLErr=(char *)MEM_alloc(500);
											tc_strcpy(AMDMLErr,"Part ");
											tc_strcat(AMDMLErr,part_name);
											tc_strcat(AMDMLErr," is with DR ");
											tc_strcat(AMDMLErr,part_DRStatus);
											//tc_strcat(AMDMLErr," status Not Allowed in APL Restructuring DML.\n");
											tc_strcat(AMDMLErr," status Not Allowed in AM DML of EcnType:");
											tc_strcat(AMDMLErr,DMLtype);
											tc_strcat(AMDMLErr,".\n");
											tc_strcat(AMDMLErr,"In APL DML with Status DR0/DR1/DR2/DR3/AR0/AR1/AR2/AR3  only Design Revision with DR Status DR0/DR1/DR2/DR3/AR0/AR1/AR2/AR3  allowed.");

											EMH_clear_errors();
											EMH_store_error_s1(EMH_severity_error,ITK_errStore1,AMDMLErr);
											return ITK_errStore1;


										}

									}
									/*************End of DR Validation********************/

								}

								/*************Start of Resource Part Validation********************/
								if(tc_strcmp(DMLtype,"APLRESPREL")==0)
								{
									printf("Inside APLRESPREL validation...\n"); fflush(stdout);
									if (tc_strcmp(part_type,"APL Resource Part")==0)
									{
										printf("part type is APL Resource Part..\n"); fflush(stdout);

									}
									else
									{
										printf("Design Revision Should be 'APL Resource Parts' For DML having ECN Type 'APL Resource Part Release'"); fflush(stdout);
										EMH_clear_errors();
										EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Design Revision Should be 'APL Resource Parts' for DML having ECN Type 'APL Resource Part Release'");
										return ITK_errStore1;

									}

								}
								/*************End of Resource Part Validation********************/

								/*************Start of Frame Grp Validation********************/
								if(tc_strcmp(DMLtype,"FrameGrp")==0)
								{
									printf("Inside FrameGrp validation...\n"); fflush(stdout);
									if ((tc_strcmp(part_type,"A")!=0)|| ((tc_strcmp(part_DesignGrp,"31")!=0)))
									{
										printf("Design Revision Should be Assembly and of group 31 for DML having ECN Type 'Frame Grp Re-Structuring'"); fflush(stdout);
										EMH_clear_errors();
										EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Design Revision Should be Assembly and of group 31 for DML having ECN Type 'Frame Grp Re-Structuring'");
										return ITK_errStore1;

									}

								}
								/*************End of Frame Grp Validation********************/

								/*************Start of Task and Part belong to same Plant Validation********************/


								tc_strcpy(dmlnumberCpy,DMLNumber);

								DMLNo = strtok( dmlnumberCpy, "_" );
								PlantName  = strtok ( NULL, "_" );

								printf("\n DMLNo %s:PlantName %s",DMLNo,PlantName); fflush(stdout);

								/*************COMMENTED BELOW CODE OF ORGANIZATION ID.WILL DO AFTER MANAGEMENT DECISION::: REWORK TZ TEST********************/

								
								if( (((tc_strcmp(PlantName,"APLP")==0) || (tc_strcmp(PlantName,"STDP")==0)) && ( (tc_strstr(owning_group_val,"APLP")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL) || (tc_strstr(owning_group_val,"ERC")!=NULL) )) ||
								(((tc_strcmp(PlantName,"APLC")==0) || (tc_strcmp(PlantName,"STDC")==0)) && ( (tc_strstr(owning_group_val,"APLC")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL) || (tc_strstr(owning_group_val,"ERC")!=NULL) )) ||
								(((tc_strcmp(PlantName,"APLV")==0) || (tc_strcmp(PlantName,"STDV")==0)) && ( (tc_strstr(owning_group_val,"APLV")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL) || (tc_strstr(owning_group_val,"ERC")!=NULL) )) ||								
								(((tc_strcmp(PlantName,"APLS")==0) || (tc_strcmp(PlantName,"STDS")==0)) && ( (tc_strstr(owning_group_val,"APLS")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL) || (tc_strstr(owning_group_val,"ERC")!=NULL)  )  ) ||
								(((tc_strcmp(PlantName,"APLL")==0) || (tc_strcmp(PlantName,"STDL")==0)) && ( (tc_strstr(owning_group_val,"APLL")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL) || (tc_strstr(owning_group_val,"ERC")!=NULL)  )) ||
								(((tc_strcmp(PlantName,"APLJ")==0) || (tc_strcmp(PlantName,"STDJ")==0)) && ( (tc_strstr(owning_group_val,"APLJ")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL) || (tc_strstr(owning_group_val,"ERC")!=NULL) ) ) ||
								(((tc_strcmp(PlantName,"APLU")==0) || (tc_strcmp(PlantName,"STDU")==0)) && ( (tc_strstr(owning_group_val,"APLU")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL) || (tc_strstr(owning_group_val,"ERC")!=NULL)  )) ||
								(((tc_strcmp(PlantName,"APLD")==0) || (tc_strcmp(PlantName,"STDD")==0)) && ( (tc_strstr(owning_group_val,"APLD")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL) || (tc_strstr(owning_group_val,"ERC")!=NULL) ) ) )
								{

									printf("No error of Task and Design Revision Should Belong To Same Plant.");fflush(stdout);
									//EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Task and Design Revision Should Belong To Same Plant.");
									//return ITK_errStore1;

								}
								else
								{
									printf("Task and Design Revision Should Belong To Same Plant.");fflush(stdout);
									EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Task and Design Revision Should Belong To Same Plant.");
									return ITK_errStore1;
								
								}

								/*************END of Task and Part belong to same Plant Validation********************/

								/*************Start of APL MBOM Re-Structuring Validation********************/
								/*************COMMENTED BELOW CODE OF ORGANIZATION ID.WILL DO AFTER MANAGEMENT DECISION:::REWORK TZ TEST********************/

								/*if (tc_strcmp(DMLtype,"APLMBOMRES")==0)
								{

									printf("Inside APLMBOMRES validation...\n"); fflush(stdout);

									if((tc_strstr(owning_group_val,"ERC")!=NULL) || (tc_strstr(owning_group_val,"dba")!=NULL))
									{
										ITKCALL(WSOM_ask_release_status_list(secondary_object,&st_count1,&status_list1));
										printf("\n st_count1 :%d is\n",st_count1);fflush(stdout);

										if(st_count1>0)
										{
											for (cnt=0;cnt< st_count1;cnt++ )
											{
												WSO_Name=NULL;
												ITKCALL(AOM_ask_name(status_list1[cnt],&WSO_Name));
												printf("\n ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);

												if(tc_strcmp(WSO_Name,"")!=0)
												{
													if (tc_strcmp(WSO_Name,"T5_LcsStdRlzd")!=0)
													{
															printf("Design Revision should be STDSI Released for DML having ECN Type 'APL MBOM Re-Structuring'");fflush(stdout);
															if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
															AMDMLErr=(char *)MEM_alloc(500);
															tc_strcpy(AMDMLErr,"Part ");
															tc_strcat(AMDMLErr,part_name);
															tc_strcat(AMDMLErr," should be STDSI Released for DML having ECN Type 'APL MBOM Re-Structuring'.\n");

															EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Design Revision should be STDSI Released for DML having ECN Type 'APL MBOM Re-Structuring'");
															return ITK_errStore1;
													}else
													{
														printf("\n inside else loop........\n");fflush(stdout);

													}
												}
												else
												{
													printf("\n No Release Status Found...\n");fflush(stdout);
												}
											}
										}
									}

									if (tc_strcmp(part_type,"SMA")!=0)
									{
										printf("For DML with ECN Type 'APL MBOM Re-Structuring' 14 digit Design Revision Type Should be 'Stage APL MBOM Assembly'"); fflush(stdout);

										if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
										AMDMLErr=(char *)MEM_alloc(500);
										tc_strcpy(AMDMLErr,"Design Revision ");
										tc_strcat(AMDMLErr,part_name);
										tc_strcat(AMDMLErr," with Part Type ");
										tc_strcat(AMDMLErr,part_type);
										tc_strcat(AMDMLErr," not allowed under APL Restructuring DML ");
										tc_strcat(AMDMLErr,DMLNumber );
										tc_strcat(AMDMLErr," with ECN Type ");
										tc_strcat(AMDMLErr,DMLtype);
										tc_strcat(AMDMLErr,".\n For DML with ECN Type 'APL MBOM Re-Structuring' ,Design Revision Type Should be 'Stage APL MBOM Assembly'");

										EMH_clear_errors();
										EMH_store_error_s1(EMH_severity_error,ITK_errStore1,AMDMLErr);
										return ITK_errStore1;

									}


								}*/

								/*************End of APL MBOM Re-Structuring Validation********************/

								/*************Start of APL MBOM Re-Structuring Validation********************/

								/*************COMMENTED BELOW CODE OF ORGANIZATION ID.WILL DO AFTER MANAGEMENT DECISION:::REWORK TZ TEST********************/
								if(tc_strstr(owning_group_val,"APL")!=NULL)
								{
									printf("\n owning_group_val is APL...");fflush(stdout);	
								}
								else
								{
									if( AOM_ask_value_string(secondary_object,PlantCS,&part_MakeBuy)==ITK_ok)
									printf("\n part_MakeBuy Value :--------------------   %s\n",part_MakeBuy);fflush(stdout);

									if(tc_strcmp(part_MakeBuy,"")!=0)
									{
										//if ( (tc_strcmp(part_MakeBuy,"E50 (In-house production-Phantom Assembly)")==0) || (tc_strcmp(part_MakeBuy,"F30 (External procurement - Subcontracting)")==0) || (tc_strcmp(part_MakeBuy,"E99 (In-house production-Back Flush Items)")==0) || (tc_strcmp(part_MakeBuy,"F40 (Interplant stock Transfer using STSA)")==0))
										if ( (tc_strcmp(part_MakeBuy,"E50")==0) || (tc_strcmp(part_MakeBuy,"F30")==0) || (tc_strcmp(part_MakeBuy,"E99")==0) || (tc_strcmp(part_MakeBuy,"F40")==0))
										{
											printf("\n APL working Design Rev with CS E50/E99/F30/F49 is allowed to attach in AM DML \n");fflush(stdout);
										}

										else
										{

											count_dml=0;
											dml=0;
											cnt=0;
											st_count1=0;
											PlantDMLActiveFlg=0;
											PartTaskTags=NULLTAG;
											PartDmlTag=NULLTAG;
											relation_type_task=NULLTAG;
											DmlId=NULL;
											GRM_find_relation_type("CMHasSolutionItem",&relation_type_task);
											GRM_list_primary_objects_only(secondary_object,relation_type_task,&count_dml,&PartTaskTags);
											printf("\t  count_dml ...%d\n", count_dml);fflush(stdout);
											if(count_dml >0)
											{
												for (dml=0;dml<count_dml ;dml++ )
												{
													PartDmlTag=PartTaskTags[dml];
													ITKCALL (TCTYPE_ask_object_type(PartDmlTag,&itemTypeTag_class));
													ITKCALL (TCTYPE_ask_name(itemTypeTag_class,type_class));

													printf("\t  type_itemRev ...%s\n", type_class);fflush(stdout);
													if(tc_strcmp(type_class,"T5_APLTaskRevision")==0)
													{
														ITKCALL(AOM_ask_value_string(PartDmlTag, "item_id",&DmlId));
														printf("\n DmlId %s.....PlantName:%s\n",DmlId,PlantName);fflush(stdout);

														if(tc_strstr(DmlId,PlantName)!=NULL)
														{

															st_count1=0;
															ITKCALL(WSOM_ask_release_status_list(PartDmlTag,&st_count1,&status_list1));
															printf("\n 111..st_count1 :%d is\n",st_count1);fflush(stdout);

															if(st_count1>0)
															{
																for (cnt=0;cnt< st_count1;cnt++ )
																{
																	WSO_Name=NULL;
																	ITKCALL(AOM_ask_name(status_list1[cnt],&WSO_Name));
																	printf("\n 111..ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);

																	if(tc_strcmp(WSO_Name,"")!=0)
																	{
																		if ((tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0))
																		{
																			PlantDMLActiveFlg++;
																		}
																	}
																}
															}
														}
													}
												}
												if(PlantDMLActiveFlg>0)
												{
													printf("Design Revision is attached to some Other DML and is in APL-Working State.");fflush(stdout);
													EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Design Revision is attached to some Other DML and is in APL-Working State.");
													return ITK_errStore1;													
												}
											}
											
										}
										
								  }
								  else
								  {
										printf("\n part_MakeBuy Value :--------------------   %s is NULL\n",part_MakeBuy);fflush(stdout);
								  }
								}
								/*************End of APL MBOM Re-Structuring Validation********************/

								/*************Start of AMDML Validation********************/
								if(tc_strcmp(DMLtype,"AMDML")==0)
								{

									printf("Inside AMDML validation...\n"); fflush(stdout);
									if(latestrev != NULLTAG)
									{
										if (AOM_ask_value_string(latestrev,"current_revision_id",&Desc_obj)==ITK_ok)
										printf("\n Desc_obj Value :--------------------   %s\n",Desc_obj); fflush(stdout);

										if(tc_strcmp(Desc_obj_drop_prt,Desc_obj)==0)
										{
											printf("Dropped object is latest apl released.."); fflush(stdout);
										}
										else
										{

											ITKCALL(WSOM_ask_release_status_list(latestrev,&st_count_next_rev,&status_list_nextRev));
											printf("\n st_count_next_rev :%d is\n",st_count_next_rev);fflush(stdout);

											if(st_count_next_rev>0)
											{
												for (cnt_next=0;cnt_next< st_count_next_rev;cnt_next++ )
												{
													WSO_Name_next_Rev=NULL;
													ITKCALL(AOM_ask_name(status_list_nextRev[cnt_next],&WSO_Name_next_Rev));
													printf("\n WSO_Name_next_Rev is :%s\n",WSO_Name_next_Rev);fflush(stdout);

												}
											}

											if(tc_strcmp(WSO_Name_next_Rev,"")!=0)
											{
												if ((tc_strcmp(WSO_Name_next_Rev,"T5_LcsAplRlzd")==0)||(tc_strcmp(WSO_Name_next_Rev,"T5_LcsSTDWrkg")==0) || (tc_strcmp(WSO_Name_next_Rev,"T5_LcsStdRlzd")==0))
												{
													printf("Drag & Drop Of Design Revision Not Allowed As The Next Revision Of Part is already APL Released"); fflush(stdout);

													if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
													AMDMLErr=(char *)MEM_alloc(500);
													tc_strcpy(AMDMLErr,"Drag & Drop Of Part Not Allowed As The Next Revision Of Part ");
													tc_strcat(AMDMLErr,part_name);
													tc_strcat(AMDMLErr," Is Already APL Released.");

													EMH_clear_errors();
													EMH_store_error_s1(EMH_severity_error,ITK_errStore1,AMDMLErr);
													return ITK_errStore1;
												}
											}

										}

									}
								}
								/*************End of AMDML Validation********************/

								/*************Start of AMBSTR Validation********************/
								if (tc_strcmp(DMLtype,"AMBSTR")==0)
								{
									printf("Inside AMBSTR validation...\n"); fflush(stdout);
									if (tc_strcmp(part_type,"VC")==0)
									{
										printf("part type is VC..\n");

									}
									else
									{
										printf("\n For DML with ECN Type 'Ambulance Restructuring', Dropped Design Revision must be 'VC'."); fflush(stdout);

										if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
										AMDMLErr=(char *)MEM_alloc(500);
										tc_strcpy(AMDMLErr,"Part ");
										tc_strcat(AMDMLErr,part_name);
										tc_strcat(AMDMLErr," with Part Type ");
										tc_strcat(AMDMLErr,part_type);
										tc_strcat(AMDMLErr," not allowed under Ambulance Restructuring DML ");
										tc_strcat(AMDMLErr,DMLNumber);
										tc_strcat(AMDMLErr,".\n For DML with ECN Type 'Ambulance Restructuring', Dropped Design Revision must be 'VC'.");

										EMH_clear_errors();
										EMH_store_error_s1(EMH_severity_error,ITK_errStore1,AMDMLErr);
										return ITK_errStore1;

									}
								}
								/*************Start of AMBSTR Validation********************/
								//if((tc_strcmp(DMLtype,"APLSTR")==0) || (tc_strcmp(DMLtype,"APLMBOMRES")==0) || (tc_strcmp(DMLtype,"AMBSTR")==0) || (tc_strcmp(DMLtype,"FrameGrp")==0))
								if((tc_strcmp(DMLtype,"APLSTR")==0) || (tc_strcmp(DMLtype,"AMBSTR")==0) || (tc_strcmp(DMLtype,"FrameGrp")==0))
								{

									printf("\n Inside 12-14 digiti validation.."); fflush(stdout);
									//if(tc_strlen(part_name)==14)
									if(tc_strstr(owning_group_val,"APL")!=NULL)
									{

										/*************COMMENTED BELOW CODE OF ORGANIZATION ID.WILL DO AFTER MANAGEMENT DECISION:REWORK.BELOW CHECK IS NOT REQUIRED********************/
										/*if(tc_strstr(organization_id,"APL")!=NULL)
										{
											printf("\n UserAgency is APL");fflush(stdout);
										}
										else
										{
											printf("For Any 14 digit Design Revision Organization ID should be APLP/APLA/APLJ/APLL/APLD.");

											if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
											AMDMLErr=(char *)MEM_alloc(500);
											tc_strcpy(AMDMLErr,"Design Revision ");
											tc_strcat(AMDMLErr,part_name);
											tc_strcat(AMDMLErr," is 14 digit Part with Organization ID as ");
											tc_strcat(AMDMLErr,organization_id);
											tc_strcat(AMDMLErr,".\n For Any 14 digit Design Revision Organization ID should be APLP/APLA/APLJ/APLL/APLD");

											EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"For Any 14 digit Design Revision Organization ID should be APLP/APLA/APLJ/APLL/APLD.");
											return ITK_errStore1;

										}*/


										t5AplPrtwithAPLWorkFlag=0;
										cnt=0;
										if(st_count1>0)
										{
											for (cnt=0;cnt< st_count1;cnt++ )
											{
												WSO_Name=NULL;
												ITKCALL(AOM_ask_name(status_list1[cnt],&WSO_Name));
												printf("\n 111..ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);

												if(tc_strcmp(WSO_Name,"")!=0)
												{
													if(tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0)
													{
														t5AplPrtwithAPLWorkFlag++;	
													}
												}
											}
											if(t5AplPrtwithAPLWorkFlag==0)
											{
												printf("For any APL created Design Revision, Release Status should be APL Working."); fflush(stdout);
												if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
												AMDMLErr=(char *)MEM_alloc(500);
												tc_strcpy(AMDMLErr,"APL created Design Revision ");
												tc_strcat(AMDMLErr,part_name);
												tc_strcat(AMDMLErr," with Life Cycle State 'APL Working'");
												tc_strcat(AMDMLErr," are allowed under APL Restructuring DML "); // DEEPTI ADDDED TZ1.34
												tc_strcat(AMDMLErr,DMLNumber);
												tc_strcat(AMDMLErr,".\n For any APL created Design Revision, Release Status should be APL Working.");

												EMH_clear_errors();
												EMH_store_error_s1(EMH_severity_error,ITK_errStore1,AMDMLErr);
												return ITK_errStore1;
											
											}
										}

									}
									else
									{
										if(st_count1>0)
										{
											cnt=0;
											validERCLCSFound=0;
											validAPLLCSFound=0;
											validSTDLCSFound=0;
											for (cnt=0;cnt< st_count1;cnt++ )
											{
												WSO_Name=NULL;
												ITKCALL(AOM_ask_name(status_list1[cnt],&WSO_Name));
												printf("\n ....111..ppWSO_Name is :%s\n",WSO_Name);fflush(stdout);

												if(tc_strcmp(WSO_Name,"")!=0)
												{
													if((tc_strcmp(WSO_Name,"T5_LcsErcRlzd")==0) ||(tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")==0) ||(tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0)||(tc_strcmp(WSO_Name,"T5_LcsSTDWrkg")==0)||(tc_strcmp(WSO_Name,"T5_LcsStdRlzd")==0))
													{
														validERCLCSFound++;
													}
													if((tc_strcmp(WSO_Name,"T5_LcsAplRlzd")==0)||(tc_strcmp(WSO_Name,"T5_LcsSTDWrkg")==0)||(tc_strcmp(WSO_Name,"T5_LcsStdRlzd")==0))
													{
														validAPLLCSFound++;
													}
													if((tc_strcmp(WSO_Name,"T5_LcsStdRlzd")==0))
													{
														validSTDLCSFound++;
													}
												}
											}
											printf("\n ....111..ppWSO_Name is :%s:%d:%d:%d\n",owning_group_val,validERCLCSFound,validAPLLCSFound,validSTDLCSFound);fflush(stdout);
											/*************COMMENTED BELOW CODE OF ORGANIZATION ID.WILL DO AFTER MANAGEMENT DECISION:REWORK POINT TZ********************/

											if(( ((tc_strstr(owning_group_val,"ERC")!=NULL)||(tc_strstr(owning_group_val,"dba")!=NULL) )&&   (validERCLCSFound>0)) || ( ( tc_strstr(owning_group_val,"APL")!=NULL ) &&   (validAPLLCSFound>0)))
											{
												printf("\n Inside checking NR for Frame Grp %s:%s",DMLtype,Desc_obj_drop_prt);fflush(stdout);
												if(validSTDLCSFound==0)
												{
													if((tc_strcmp(DMLtype,"FrameGrp")==0) && (tc_strstr(Desc_obj_drop_prt,"NR")!=NULL))
													{
														NRCheCkBPS=1;
													}

													if(NRCheCkBPS!=1)
													{
														printf("ERC Released and above Parts With ERC Orgnization and APL Working and above Parts with APL Organisation are allowed to add in Restructuring DML only.");
														EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"ERC Released and above Parts With ERC Orgnization and APL Working and above Parts with APL Organisation are allowed to add in Restructuring DML only.");
														return ITK_errStore1;
													}
												}
											}
											else if(( ((tc_strstr(owning_group_val,"ERC")!=NULL)||(tc_strstr(owning_group_val,"dba")!=NULL)) &&   ((tc_strcmp(WSO_Name,"T5_LcsErcRlzd")!=0) ||(tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")!=0) ||(tc_strcmp(WSO_Name,"T5_LcsAplRlzd")!=0)||(tc_strcmp(WSO_Name,"T5_LcsSTDWrkg")!=0)||(tc_strcmp(WSO_Name,"T5_LcsStdRlzd")!=0))) || ((tc_strstr(owning_group_val,"APL")!=NULL) &&   ((tc_strcmp(WSO_Name,"T5_LcsAPLWrkg")!=0) ||(tc_strcmp(WSO_Name,"T5_LcsAplRlzd")!=0)||(tc_strcmp(WSO_Name,"T5_LcsSTDWrkg")!=0)||(tc_strcmp(WSO_Name,"T5_LcsStdRlzd")!=0))))
											{
													printf("11..ERC Released and above Parts With ERC Orgnization and APL Working and above Parts with APL Organisation are allowed to add in Restructuring DML only.");
													EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"ERC Released and above Parts With ERC Orgnization and APL Working and above Parts with APL Organisation are allowed to add in Restructuring DML only.");
													return ITK_errStore1;


											}
											else
											{
												tc_strcpy(dmlnumberCpy_val,DMLNumber);
												IsAPLStrDMLRlzErr=0;

												DMLNoCpy = strtok( dmlnumberCpy_val, "_" );
												PlantNameCpy  = strtok ( NULL, "_" );

												GRM_find_relation_type("CMHasSolutionItem",&relation_type_task);
												GRM_list_primary_objects_only(secondary_object,relation_type_task,&count_dml,&PartTaskTags);
												if(count_dml >0)
												{
													for (dml=0;dml<count_dml ;dml++ )
													{
														PartDmlTag=PartTaskTags[dml];
														ITKCALL (TCTYPE_ask_object_type(PartDmlTag,&itemTypeTag_class));
														ITKCALL (TCTYPE_ask_name(itemTypeTag_class,type_class));

														printf("\t  type_itemRev ...%s\n", type_class); fflush(stdout);
														if(tc_strcmp(type_class,"T5_APLTaskRevision")==0)
														{
															ITKCALL(AOM_ask_value_string(PartDmlTag, "item_id",&DmlId));
															printf("\n DmlId %s.....\n",DmlId);fflush(stdout);

															tc_strcpy(ConndmlnumberCpy_val,DmlId);

															ConntPlantNameCpy=subString(ConndmlnumberCpy_val,14,tc_strlen(ConndmlnumberCpy_val));


															printf("\n PlantNameCpy %s:ConntPlantNameCpy %s.....\n",PlantNameCpy,ConntPlantNameCpy); fflush(stdout);
															if(tc_strcmp(PlantNameCpy,ConntPlantNameCpy)==0)
															{
																if(tc_strstr(DmlId,"AM")!=NULL)
																{

																	ITKCALL(GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type_1));
																	if (tsk_dml_rel_type_1!=NULLTAG)
																	{
																		ITKCALL(GRM_list_primary_objects_only(PartDmlTag,tsk_dml_rel_type_1,&count_dml_1,&DMLRevision_Prt));
																		printf("\n\n\t\t DML Revision from Task : %d",count_dml_1);fflush(stdout);

																		for (j=0;j<count_dml_1 ;j++ )
																		{
																			ITKCALL(ITEM_rev_sequence_is_latest(DMLRevision_Prt[j],&is_latest_dml));
																			printf("\n\n\t\t is_latest_dml : %d\n",is_latest_dml);fflush(stdout);

																			if(is_latest_dml != true)
																			{
																				printf("\n\n\t\t is_latest_dml is not true .....\n");fflush(stdout);
																			}else
																			{
																				DMLRevTag_Prt = DMLRevision_Prt[j];

																				ITKCALL(AOM_ask_value_string(DMLRevTag_Prt,"t5_EcnType",&partDmlType));
																				printf("\n\n\t\t partDmlType is :%s",partDmlType);fflush(stdout);
																			}
																		}
																	}



																	//if ((tc_strcmp(partDmlType,"APLSTR")==0) || (tc_strcmp(partDmlType,"APLMBOMRES")==0) || (tc_strcmp(partDmlType,"AMBSTR")==0)|| (tc_strcmp(partDmlType,"CARRYOVER")==0)|| (tc_strcmp(partDmlType,"TOODMLR")==0)|| (tc_strcmp(partDmlType,"K")==0))
																	if ((tc_strcmp(partDmlType,"APLSTR")==0) || (tc_strcmp(partDmlType,"AMBSTR")==0)|| (tc_strcmp(partDmlType,"CARRYOVER")==0)|| (tc_strcmp(partDmlType,"TOODMLR")==0)|| (tc_strcmp(partDmlType,"K")==0))
																	{
																		ITKCALL(WSOM_ask_release_status_list(DMLRevTag_Prt,&st_count_dml,&dml_status_list));
																		for (dmlstlst = 0; dmlstlst < st_count_dml; dmlstlst++)
																		{
																			AOM_ask_name(dml_status_list[dmlstlst], &ReleaseStatusDML);
																			printf("\n ReleaseStatusDML %s \n",  ReleaseStatusDML);fflush(stdout);
																			if(tc_strcmp(ReleaseStatusDML,"T5_LcsStdRlzd")==0)//DEEPTI ADDED 1.34
																			{
																				IsAPLStrDMLRlzErr++;
																			}
																		}
																	}

																}
																else
																{
																	ITKCALL(WSOM_ask_release_status_list(PartDmlTag,&st_count,&status_list));
																	if (st_count == 0)
																	{
																		printf("\n No Status in DML Task, so the Item is not yet Released \n");fflush(stdout);

																	}
																	else
																	{
																		dmlstlst = 0;
																		for (dmlstlst = 0; dmlstlst < st_count; dmlstlst++)
																		{
																			ITKCALL(AOM_ask_value_string(status_list[dmlstlst],"object_name",&class_name));
																			printf("\n class_name: %s\n",class_name);fflush(stdout);
																			if(tc_strcmp(class_name,"T5_LcsStdRlzd")==0)//DEEPTI ADDED 1.34
																			{
																				IsAPLStrDMLRlzErr++;

																			}
																		}
																	}


																}

															}

														}
													}
												}

											}												
											
										}
										printf("IsAPLStrDMLRlzErr %d",IsAPLStrDMLRlzErr);fflush(stdout);
										if(IsAPLStrDMLRlzErr==0)
										{

											printf("Latest STDSI Released Part from your Plant will be allow to attached in APL Restructuring DML .");fflush(stdout);

											if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
											AMDMLErr=(char *)MEM_alloc(500);
											//tc_strcpy(AMDMLErr,"DML attached to  ");
											tc_strcpy(AMDMLErr,"Part  ");
											tc_strcat(AMDMLErr,part_name);
											tc_strcat(AMDMLErr,",");
											tc_strcat(AMDMLErr,Desc_obj_drop_prt);
											tc_strcat(AMDMLErr," is not STDSI Released from APLDML of your plant.\n");
											tc_strcat(AMDMLErr,"Latest STDSI Released Part from your Plant will be allow to attached in APL Restructuring DML . \n");


											EMH_clear_errors();
											EMH_store_error_s1(EMH_severity_error,ITK_errStore1,AMDMLErr);
											return ITK_errStore1;

										}


									}
								}

							}
							else //ADDED BY DEEPTI 1.34
							{
									printf("DML Is not on WORK ON DML...");fflush(stdout);

									if (tc_strcmp(AMDMLErr,"NULL") !=0) MEM_free(AMDMLErr);
									AMDMLErr=(char *)MEM_alloc(500);
									tc_strcpy(AMDMLErr,"Task ");
									tc_strcat(AMDMLErr,"is not for assignment of ");
									tc_strcat(AMDMLErr,"'Work On DML'.");
									tc_strcat(AMDMLErr,"You cannot add parts to SolutionItem of the task.");


									EMH_clear_errors();
									EMH_store_error_s1(EMH_severity_error,ITK_errStore1,AMDMLErr);
									return ITK_errStore1;


							}

							}

					}
					else
					{
						/*************Start of ERC Cannot Attach Part to APL DML********************/
						if((tc_strstr(DMLNumber,"APL")!=NULL)&&(tc_strstr(DMLNumber,"AM")!=NULL))
						{
							if ((tc_strcmp(DMLtype,"AMBSTR")!=0))
							{
									printf("ERC cannot attach a Design Revision to APL DML."); fflush(stdout);
									EMH_clear_errors();
									EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"ERC cannot attach a Design Revision to APL DML.");
									return ITK_errStore1;

							}


						}
						/*************END of ERC Cannot Attach Part to APL DML********************/

					}

				}
//				else
//				{
//					printf("ECN Type is NULL.");
//					EMH_clear_errors();
//					EMH_store_error_s1(EMH_severity_error,ITK_errStore1,"Please update the ECN Type for AM-DML");//1.34
//					return ITK_errStore1;
//				}
			}

		}
		if(DMLRevision)
					MEM_free(DMLRevision);

		return error_code;
}

int NA_CS_Bypass_Checks(int PartCnt, tag_t AssyTag,char* PlantCS)
{
        		EPM_decision_t decision;
	            int k = 0;
	            char*			Part_no				= NULL;
	             tag_t	item_rev_tag_p						= NULLTAG;
	            char*		Part_type			=	NULL;
             	char*			Part_CS				= NULL;

				//Start for NA CS Bypass check

					const char *qry_entries[1];
					const char *qry_values[1];
					int n_tags_found=0;
					tag_t	*itemclassp	= NULLTAG;
					 tag_t item = NULLTAG;
					printf("\n\n\t\t for k =:%d",k);fflush(stdout);

					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
					printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);
					 qry_entries[0] ="item_id";
					 qry_values[0] = Part_no;
					ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found, &itemclassp);
					//ITKCALL(ITEM_find_item(Part_no,&itemclassp));
					item = itemclassp[0];
					if(item != NULLTAG )
					ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag_p));

					ITKCALL(AOM_ask_value_string(AssyTag,"t5_PartType",&Part_type));
					printf("\n\n\t\t Part_type  is :%s",Part_type);	fflush(stdout);

					ITKCALL(AOM_ask_value_string(AssyTag,PlantCS,&Part_CS));
					printf("\n Parent Part_CS:%s ..............",Part_CS);fflush(stdout);

					if( (strstr(Part_CS,"NA")) || (strcmp(Part_type,"D")==0) )
					{
						printf("\n\n\t\t Validation Bypass");	fflush(stdout);
						return 1;
					}
					else
					{
						printf("\n\n\t\t Validation Calling");	fflush(stdout);
                        return 0;
					}

	return 0;

}

int CS_Successor_Update(int PartCnt, tag_t* PartTags,char* PlantCS)
{
        		EPM_decision_t decision;
	            int k = 0;
	            int count1 = 0;
	            int Flag = 0;
	            int ii = 0;
	            tag_t			AssyTag				= NULLTAG;
	            char*			Part_no				= NULL;
	             tag_t	item_rev_tag_p						= NULLTAG;
	             tag_t	user_tag						= NULLTAG;
	             tag_t	DesignTag						= NULLTAG;
	            char*		Part_type			=	NULL;
             	char*			Part_CS				= NULL;
             	char*			Item_prev				= NULL;
		         tag_t *  rev_list;
		         char  	rev_id[ITEM_id_size_c+1];
		         char  	rev_id1[ITEM_id_size_c+1];
	         	char*			username				="infodba";

				printf("\n Inside Successor Update..............");fflush(stdout);


				//Start for Successor Update
				for (k=0;k<PartCnt ;k++ )
				{
					const char *qry_entries[1];
					const char *qry_values[1];
					int n_tags_found=0;
					tag_t	*itemclassp	= NULLTAG;
					 tag_t item = NULLTAG;
					printf("\n\n\t\t for k =:%d",k);fflush(stdout);
					AssyTag=PartTags[k];

					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
					printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);
					 qry_entries[0] ="item_id";
					 qry_values[0] = Part_no;
					ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found, &itemclassp);
					//ITKCALL(ITEM_find_item(Part_no,&itemclassp));
					item = itemclassp[0];
					if(item != NULLTAG )
					ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag_p));

					AOM_UIF_ask_value(AssyTag,"item_revision_id",&Item_prev);
					printf("\n\n\t  Part String is Item_prev--->%s",Item_prev);

					ITKCALL(AOM_ask_value_string(AssyTag,PlantCS,&Part_CS));
					printf("\n Parent Part_CS:{%s}",Part_CS);fflush(stdout);

					ITEM_list_all_revs  (item, &count1,  &rev_list );
					printf("\n\n\t\tWSOM count1:- %d\n\n",count1);
					Flag = 0;
					for (ii = 0; ii < count1; ii++)
					{
						ITEM_ask_rev_id 	( rev_list[ii], rev_id1);
						printf("\n\n\t  Part String is rev_id1--->%s",rev_id1);
                         if(Flag ==1)
						   {
			 				ITKCALL(POM_get_user(&username,&user_tag));
            				printf("\n Starting for username :%s....\n",username);fflush(stdout);
        			        DesignTag	=	rev_list[ii];

							ITKCALL(AOM_lock(DesignTag));
							ITKCALL ( AOM_set_value_string(DesignTag,PlantCS,Part_CS));
							ITKCALL(AOM_save(DesignTag));
							ITKCALL(AOM_unlock(DesignTag));
							printf("\n rev_id1[%s] CS[%s] Updated :..............",rev_id1,Part_CS);fflush(stdout);
						   }

						  if (strcmp(rev_id1,Item_prev)==0)
						  {
	  						printf("\n \t  Both revisions match");
							  Flag = 1;
						  }else
						 {
						  	printf("\n \t  Both revisions not-match");
						    Flag = 0;
					     }
					  }

				}//End for Successor Update

	return 0;


}


int CS_Parent_Child_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS)
{
        		EPM_decision_t decision;
	            int k = 0;
	            int n = 0;
	            int j = 0;
	            tag_t			AssyTag				= NULLTAG;
	            char*			Part_no				= NULL;
	             tag_t	item_rev_tag_p						= NULLTAG;
	            char*		Part_type			=	NULL;
             	char*			Part_CS				= NULL;
             	char*			Item_prev				= NULL;
		        char *  CSMainErr = NULL;
		        char *  CSMainErr1 = NULL;
				tag_t	window								= NULLTAG;
				tag_t	rule								= NULLTAG;
				tag_t	top_line							= NULLTAG;
				tag_t	Childtop_line							= NULLTAG;
				tag_t  *children;
         	    tag_t  	item_rev_cld_tag;
         	    tag_t	itemcldclass							= NULLTAG;
		    	char *Item_rev = NULL;
	            int   attribute_act_tak = 0;
	           char *Item_id = NULL;

			    printf("\n Inside Main  CS Parent/Child  check ..............");fflush(stdout);
				//Start for Main  CS Parent/Child  check
				for (k=0;k<PartCnt ;k++ )
				{
					const char *qry_entries[1];
					const char *qry_values[1];
					int n_tags_found=0;
					int BypassNACSFlag=0;
					tag_t	*itemclassp	= NULLTAG;
					tag_t item = NULLTAG;

					if (tc_strcmp(CSMainErr,"NULL") !=0) MEM_free(CSMainErr);
					CSMainErr=(char *)MEM_alloc(100);
					tc_strcpy(CSMainErr," ");

					printf("\n\n\t\t for k =:%d",k);fflush(stdout);
					AssyTag=PartTags[k];
					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }

					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
					printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);
					 qry_entries[0] ="item_id";
					 qry_values[0] = Part_no;
					ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found, &itemclassp);
					//ITKCALL(ITEM_find_item(Part_no,&itemclassp));
					item = itemclassp[0];
					if(item != NULLTAG )
					ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag_p));

					AOM_UIF_ask_value(AssyTag,"item_revision_id",&Item_prev);
					printf("\n\n\t Child Part String is Item_prev--->%s",Item_prev);

					ITKCALL(AOM_ask_value_string(AssyTag,"t5_PartType",&Part_type));
					printf("\n\n\t\t Part_type  is :%s",Part_type);	fflush(stdout);

					ITKCALL(AOM_ask_value_string(AssyTag,PlantCS,&Part_CS));
					printf("\n Parent Part_CS:%s ..............",Part_CS);fflush(stdout);

					if( (strcmp(Part_type,"G") == 0) || (strcmp(Part_type,"D")==0) )
					{
						printf("\n\n\t\t Validation Bypass");	fflush(stdout);
						continue;
					}

					if( (strcmp(Part_CS,"") == 0) || Part_CS == NULL )
					{
						 if(cnt==0)
						{
							cnt = 1;
							tc_strcat(CSMainErr,"Part ");
							tc_strcat(CSMainErr,Part_no);
							tc_strcat(CSMainErr,",");
							tc_strcat(CSMainErr,Item_prev);
							tc_strcat(CSMainErr," is not having CS, Please  give CS.\n");
							 printf("\n CSMainErr11: %s\n",CSMainErr);fflush(stdout);
						}
						else
						{
							tc_strcat(CSMainErr,"Part ");
							tc_strcat(CSMainErr,Part_no);
							tc_strcat(CSMainErr,",");
							tc_strcat(CSMainErr,Item_prev);
							tc_strcat(CSMainErr," is not having CS, Please  give CS.\n");
							cnt = cnt+1;
							 printf("\n CSMainErr12: %s\n",CSMainErr);fflush(stdout);

						}

						if(cnt==1)
						{
							tc_strcpy(SetChildRelErr,CSMainErr);
							cntFlag = 1;
						}
						else
						{
							tc_strcat(SetChildRelErr,CSMainErr);
							cntFlag = 1;
						}
					}


				if (tc_strcmp(CSMainErr1,"NULL") !=0) MEM_free(CSMainErr1);
				CSMainErr1=(char *)MEM_alloc(100);
				tc_strcpy(CSMainErr1," ");

				BOM_create_window(&window);
				BOM_set_window_top_line(window,item,null_tag,null_tag,&top_line);
				BOM_line_ask_all_child_lines(top_line,&n,&children);
				printf("\n\n\t no:of Chidl parts  -->%d",n);
 			    BOM_line_look_up_attribute 	("bl_revision",&attribute_act_tak);
				for ( j = 0; j < n; j++)
				{
					const char *qry_entries[1];
					const char *qry_values[1];
					int n_tags_found2=0;
		            char*		ChildPart_type			=	NULL;
                   	char*			ChildPart_CS				= NULL;
			        tag_t  	value_act_Tak;
					tag_t	*itemclass2							= NULLTAG;
		    	    char *Child_Item_rev = NULL;

					BOM_line_ask_attribute_tag 	(children[j],attribute_act_tak,&value_act_Tak);
					AOM_UIF_ask_value(value_act_Tak,"item_id",&Item_id);
					printf("\n\n\t Child Part String is Item_id-->%s",Item_id);
					AOM_UIF_ask_value(value_act_Tak,"item_revision_id",&Child_Item_rev);
					printf("\n\n\t Child Part String is Child_Item_rev--->%s",Child_Item_rev);
					ITKCALL(AOM_ask_value_string(value_act_Tak,"t5_PartType",&ChildPart_type));
					printf("\n\n\t\t ChildPart_type  is :%s",ChildPart_type);	fflush(stdout);
				    ITKCALL(AOM_ask_value_string(value_act_Tak,PlantCS,&ChildPart_CS));
					printf("\n Parent ChildPart_CS:%s ..............",ChildPart_CS);fflush(stdout);
					if( (strcmp(ChildPart_type,"D")==0) )
					{
						printf("\n\n\t\t Validation Bypass");	fflush(stdout);
						continue;
					}

					if( (strcmp(ChildPart_CS,"") == 0) || ChildPart_CS == NULL )
					{
						 if(cnt==0)
						{
							cnt = 1;
							tc_strcat(CSMainErr1,"Parent ");
							tc_strcat(CSMainErr1,Part_no);
							tc_strcat(CSMainErr1,",");
							tc_strcat(CSMainErr1,Item_prev);
							tc_strcat(CSMainErr1,"  having Child ");
							tc_strcat(CSMainErr1,Item_id);
							tc_strcat(CSMainErr1,",");
							tc_strcat(CSMainErr1,Child_Item_rev);
							tc_strcat(CSMainErr1," is not having CS, Please  give CS.\n");
							 printf("\n CSMainErr21: %s\n",CSMainErr1);fflush(stdout);
						}
						else
						{
							tc_strcat(CSMainErr1,"Parent ");
							tc_strcat(CSMainErr1,Part_no);
							tc_strcat(CSMainErr1,",");
							tc_strcat(CSMainErr1,Item_prev);
    						tc_strcat(CSMainErr1,"  having Child ");
							tc_strcat(CSMainErr1,Item_id);
							tc_strcat(CSMainErr1,",");
							tc_strcat(CSMainErr1,Child_Item_rev);
							tc_strcat(CSMainErr1," is not having CS, Please  give CS.\n");
							cnt = cnt+1;
							 printf("\n CSMainErr22: %s\n",CSMainErr1);fflush(stdout);
						}
						if(cnt==1)
						{
							tc_strcpy(SetChildRelErr,CSMainErr1);
							cntFlag = 1;
						}
						else
						{
							tc_strcat(SetChildRelErr,CSMainErr1);
							cntFlag = 1;
						}
					}

			  }

		}//End for CS Parent/Child  check

	return 0;

}

int Child_Part_Rev_Checks(tag_t	task_rev_tag,int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS)
{
			EPM_decision_t decision;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			char*			Task_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
	        int count1 = 0;
	        int Flag = 0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag_orig;
			tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	       char* class_name=NULL;
    	   int	cntTask=0,iTask=0;
		   tag_t*  	prtTasks			=	NULLTAG;
		   tag_t  		prtTask				=	NULLTAG;
		   tag_t  		prtTask_Class		=	NULLTAG;
		   char*		prtTask_type		=	NULL;
		   char*		prtTaskName			=	NULL;
	        tag_t	relation_type = NULLTAG;
	        int count = 0;
			 int ii = 0;
	        tag_t *  rev_list;
			size_t	prtTaskLen	=	0;

			 //Start for Child check
			for (k=0;k<PartCnt ;k++ )
			{
			const char *qry_entries[1];
			const char *qry_values[1];

			int n_tags_found1=0;
			int BypassNACSFlag=0;
			 tag_t	*itemclass1							= NULLTAG;
			 tag_t	itemclassp1							= NULLTAG;

			printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
			AssyTag=PartTags[k];
			BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
			printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
			 if(BypassNACSFlag == 1)
			{
				printf("\t  Bypass Validations .\n");
				continue;
			 }
			 else
			{
				printf("\t  Call Validations .\n");
			 }

	   	   ITKCALL(AOM_ask_value_string(task_rev_tag,"item_id",&Task_no));
		   printf("\t  Task_no ...%s\n", Task_no);fflush(stdout);


			ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
			printf("\n\n\t\t APL DML Cre:Part_no  is :%s",Part_no);	fflush(stdout);
			qry_entries[0] ="item_id";
			qry_values[0] =  Part_no;

			ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found1, &itemclass1);
			 itemclassp1 = itemclass1[0];

			BOM_create_window(&window);
			BOM_set_window_top_line(window,itemclassp1,null_tag,null_tag,&top_line);
			BOM_line_ask_all_child_lines(top_line,&n,&children);
			printf("\n\n\t no:of action taken are -->%d",n);
			BOM_line_look_up_attribute 	("bl_revision",&attribute_act_tak);

			printf("\n\n\t Inside if of count_act_tak ");
			for ( j = 0; j < n; j++)
			{
				const char *qry_entries[1];
				const char *qry_values[1];
				int n_tags_found2=0;
				tag_t	*itemclass2							= NULLTAG;
                cntLcs	=	0;

				BOM_line_ask_attribute_tag 	(children[j],attribute_act_tak,&value_act_Tak);
				AOM_UIF_ask_value(value_act_Tak,"item_id",&Item_id);
				printf("\n\n\t Child Part String is Item_id-->%s",Item_id);
				qry_entries[0] ="item_id";
				qry_values[0] = Item_id;

				ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found2, &itemclass2);
				 itemcldclass = itemclass2[0];

				AOM_UIF_ask_value(value_act_Tak,"item_revision_id",&Item_rev);
				printf("\n\n\t Child Part String is Item_rev--->%s",Item_rev);
				ITKCALL(ITEM_ask_latest_rev(itemcldclass,&item_rev_cld_tag_orig));


				ITKCALL(GRM_find_relation_type("CMHasSolutionItem",&relation_type));
				ITKCALL( GRM_list_primary_objects_only(item_rev_cld_tag_orig,relation_type,&cntTask,&prtTasks));
				printf("\nNo Of Task Found : %d",cntTask);fflush(stdout);
				if (cntTask>0)
				{
					for (iTask=0;iTask<cntTask;iTask++)
					{
						prtTask	=	prtTasks[iTask];
						ITKCALL (TCTYPE_ask_object_type(prtTask,&prtTask_Class));
						ITKCALL (TCTYPE_ask_name2(prtTask_Class,&prtTask_type));

						printf("\nprtTask_type : %s",prtTask_type);fflush(stdout);

						//if(tc_strcmp(type_class,"A9_APLTaskRevision")==0)
						if(tc_strcmp(prtTask_type,"T5_APLTaskRevision")==0)
						{
							prtTaskName	=	NULL;
							ITKCALL( AOM_ask_value_string(prtTask,"item_id",&prtTaskName));
							printf("\nprtTaskName : %s",prtTaskName);fflush(stdout);
							if (tc_strcmp(prtTaskName,Task_no)==0)
							{
								printf("\n Part attached to same task...!!!");fflush(stdout);
								cntLcs ++;
								break;

							}
							prtTaskLen	=	tc_strlen(prtTaskName);
							if (prtTaskLen>0)
							{
								MEM_free(prtTaskName);
							}
						}
					}
					if (cntTask>0)
					{
						MEM_free(prtTasks);
					}
				}

							ITEM_list_all_revs  (itemcldclass, &count1,  &rev_list );
							Flag = 0;
							printf("\n count1:%d \n",count1);
							for (ii = 0; ii < count1; ii++)
							{
							item_rev_cld_tag = rev_list[ii];
							ITKCALL(WSOM_ask_release_status_list(item_rev_cld_tag,&st_count,&status_list));
							printf("\n st_count:%d \n",st_count);
							if (tc_strcmp(ChildRelErr,"NULL") !=0) MEM_free(ChildRelErr);
							ChildRelErr=(char *)MEM_alloc(100);
							tc_strcpy(ChildRelErr," ");
							if (st_count == 0)  /* No Status, so the Item is not yet Released */
							{
								printf("\n No Status, so the Item is not yet Released \n");

							}else
								{

										for (iLCS=0;iLCS<st_count ;iLCS++ )
										{
											ITKCALL(AOM_ask_value_string(status_list[iLCS],"object_name",&class_name));
											printf("\n class_name: %s\n",class_name);fflush(stdout);
											if(tc_strcmp(class_name,"T5_LcsAplRlzd")==0)
											{
												cntLcs++;
												break;
											}
										}
								}
							}

							printf("\n cntLcs: %d\n",cntLcs);fflush(stdout);
							if (cntLcs<=0)
							{
								if(cnt==0)
								{
									cnt = 1;
									tc_strcat(ChildRelErr,"Child Part ");
									tc_strcat(ChildRelErr,Item_id);
									tc_strcat(ChildRelErr,",");
									tc_strcat(ChildRelErr,Item_rev);
									tc_strcat(ChildRelErr,"  Of Parent Part ");
									tc_strcat(ChildRelErr,Part_no);
									tc_strcat(ChildRelErr," is not APL Released.\n");
									 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
								}
								else
								{
									tc_strcat(ChildRelErr,"Child Part ");
									tc_strcat(ChildRelErr,Item_id);
									tc_strcat(ChildRelErr,",");
									tc_strcat(ChildRelErr,Item_rev);
									tc_strcat(ChildRelErr,"  Of Parent Part ");
									tc_strcat(ChildRelErr,Part_no);
									tc_strcat(ChildRelErr," is not APL Released.\n");
									cnt = cnt+1;
									 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

								}
							if(cnt==1)
							{
								tc_strcpy(SetChildRelErr,ChildRelErr);
								cntFlag = 1;
							}
							else
							{
								tc_strcat(SetChildRelErr,ChildRelErr);
								cntFlag = 1;
							}
						}

				}
			} //End for Child check

	return 0;
}

int Prev_Part_Rev_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS)
{
			EPM_decision_t decision;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        tag_t	relation_type = NULLTAG;
	        int count = 0;
			tag_t*			PartPreTags			= NULLTAG;
			char*	itemidpre;
 			char*	itemidprerev;
	        tag_t			AssyPreTag				= NULLTAG;

 //Start for Previous check
				for (k=0;k<PartCnt ;k++ )
				{
					int BypassNACSFlag =0;
					printf("\n\n\t\t APL DML Cre:for k_1 =:%d",k);fflush(stdout);
					AssyTag=PartTags[k];
					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }
					GRM_find_relation_type("IMAN_based_on",&relation_type);
					GRM_list_secondary_objects_only(AssyTag,relation_type,&count,&PartPreTags);
					printf("\n\n\t\t APL DML Cre:ERC DML to Task : %d",count);fflush(stdout);
					if(count >0)
					{
					AssyPreTag=PartPreTags[0];
					ITKCALL(AOM_ask_value_string(AssyPreTag, "item_id",&itemidpre));
					printf("\n itemidpre %s.....\n",itemidpre);
					ITKCALL(AOM_ask_value_string(AssyPreTag, "item_revision_id",&itemidprerev));
					printf("\n itemidprerev %s.....\n",itemidprerev);

					ITKCALL(WSOM_ask_release_status_list(AssyPreTag,&st_count,&status_list));
					if (tc_strcmp(ChildRelErr,"NULL") !=0) MEM_free(ChildRelErr);
					ChildRelErr=(char *)MEM_alloc(100);
					tc_strcpy(ChildRelErr," ");
					if (st_count == 0)  /* No Status, so the Item is not yet Released */
					{
						printf("\n No Status, so the Item is not yet Released \n");
					}else
						{
							for (iLCS=0;iLCS<st_count ;iLCS++ )
								{
									ITKCALL(AOM_ask_value_string(status_list[iLCS],"object_name",&class_name));
									printf("\n class_name: %s\n",class_name);fflush(stdout);
									if(tc_strcmp(class_name,"T5_LcsAplRlzd")==0)
									{
										cntLcs++;
										break;
									}
								}

								printf("\n cntLcs: %d\n",cntLcs);fflush(stdout);
								if (cntLcs<=0)
								{
									if(cnt==0)
									{
										cnt = 1;
										tc_strcat(ChildRelErr,"Previous Revision Part ");
										tc_strcat(ChildRelErr,itemidpre);
										tc_strcat(ChildRelErr,",");
										tc_strcat(ChildRelErr,itemidprerev);
										tc_strcat(ChildRelErr," is not APL Released.\n");
										 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
									}
									else
									{
										tc_strcat(ChildRelErr,"Previous Revision Part ");
										tc_strcat(ChildRelErr,itemidpre);
										tc_strcat(ChildRelErr,",");
										tc_strcat(ChildRelErr,itemidprerev);
										tc_strcat(ChildRelErr," is not APL Released.\n");
										 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
										cnt = cnt+1;
										 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);
									}
								if(cnt==1)
								{
									tc_strcpy(SetChildRelErr,ChildRelErr);
									cntFlag = 1;
								}
								else
								{
									tc_strcat(SetChildRelErr,ChildRelErr);
									cntFlag = 1;
								}
						  }
						}
					}
				}  //End for Previous check

	return 0;
}

//DEEPTI TZ1.36 GRP98 CREATION AND VALIDATION ONLY FRO CAR PLANT //
int Creation_Grp98Part_Checks(tag_t	sTaskTag,char* SetChildRelErr)
{
	EPM_decision_t				decision;
	int							status;
	tag_t						rootTask					= NULLTAG;
	char						*CurrentTask				= NULL;
	char						*parent_name				= NULL;
	tag_t						ItemTypeTag					= NULLTAG;
	tag_t						*pTagAttch;
	int							iNumAttch					= 0;
	int							i							= 0;
	char*						itemid =NULL;
	char*						design_grp_no=NULL;
	tag_t						item_rev_tag					= NULLTAG;
	char						type_name[TCTYPE_name_size_c+1];
	tag_t						itemTypeTag_cdss				= NULLTAG;
	char						type_itemRev[TCTYPE_name_size_c+1];
	char apldmlnumberCpy[20];
	char newercdmlnumber[20];
	char		   *ERCDMLNo				=NULL;
	WSO_search_criteria_t  	ercdml_search_criteria;
	WSO_search_criteria_t  	grp98_searchcriteria;
	WSO_search_criteria_t  	grp98_searchcriteria1;
	int  ercdml_number_found;
	int  number_found=0;
	int  number_found1=0;
	int  jRev=0;
	tag_t *list_of_ercdml_tags			=NULLTAG;
	tag_t *list_of_grp98_tags			=NULLTAG;
	tag_t *list_of_grp98Rev_tags		=NULLTAG;
	int j=0;
	char *erc_dmlrelztype 	   = NULL;
	char *erc_dmlprojCode 	   = NULL;
	char *  Grp98Err = NULL;
	tag_t	Project_tag		= NULLTAG;
	//tag_t	erc_dml_tag		= NULLTAG;
	tag_t	Item_98Grp_tag		= NULLTAG;
	tag_t	ObjTypProj	= NULLTAG;
	tag_t	ERCObjTypProj	= NULLTAG;
	char   type_Proj[TCTYPE_name_size_c+1];
	char   dml_type[TCTYPE_name_size_c+1];
	char	*Proj_VehicleClass = NULL;
	char	*Proj_BusUnit = NULL;
	logical PcbuCvbuFlag =false;
	int				PartCnt				= 0;
	tag_t*			PartTags			= NULLTAG;
	int k=0;
	int k1=0;
	int consmTempCnt=0;
	int consmTempPartCnt=0;
	int num=0;
	int conTpCnt=0;
	int conTpPartCnt=0;
	tag_t			AssyTag				= NULLTAG;
	tag_t			AssyTag_SerBody		= NULLTAG;
	tag_t			list_of_grp98		= NULLTAG;
	char*			Part_no				= NULL;
	char*			Part_type				= NULL;
	char*			NewPart_no				= NULL;
	char*			Part_ColorIndicator				= NULL;
	char*			AssyPart_type				= NULL;
	char*			AssyPart_ColorIndicator				= NULL;
	char*			AssyPart_ColorID				= NULL;
	tag_t			bs_bvr_strc_revList=NULLTAG;
	char*			item_id_grp98				= NULL;
	char*			Part_ColorID				= NULL;
	char*			Part_ProjID				= NULL;
	char*			Part_DesgnGrp				= NULL;
	char*			AssyPart_Number				= NULL;
	char*			AssyPart_CatCode				= NULL;
	char*			grp98Name				= NULL;
	char*			template_id				= NULL;
	char*			part_id				= NULL;
	char*			consumableqty				= NULL;
	char*			consumableClass				= NULL;
	char*			child_Item_id				= NULL;
	char*			AssyPart_revision				= NULL;
	tag_t			newGrp98item=  NULLTAG;
	tag_t			newGrp98rev=  NULLTAG;
	tag_t*			consmTempTags			= NULLTAG;
	tag_t*			consmTempPartTags			= NULLTAG;
	tag_t*			consmTempPartrelation_tags			= NULLTAG;
	tag_t			TemplateTag				= NULLTAG;
	tag_t			PartTag				= NULLTAG;
	tag_t			PartTagrev				= NULLTAG;
	tag_t			PartRelTag				= NULLTAG;
	const char		*erc_dml_qry_entries[1];
	const char		*erc_dml_qry_values[1];
	const char		*project_qry_entries[1];
	const char		*project_qry_values[1];
	char			erc_dml_rev_id[TCTYPE_name_size_c+1]="NR";
	//char			erc_dml_rev_id[TCTYPE_name_size_c+1]="A";
	int				ercdml_tags_found	= 0;
	int				project_tags_found	= 0;
	int				flagGen98Grp	= 0;
	tag_t			*erc_dml_tag	= NULLTAG;
	tag_t			erc_dml_revision  = NULLTAG;
	tag_t			project_item  = NULLTAG;
	tag_t			erc_dml_rev_tag  = NULLTAG;
	tag_t			*projectclass  = NULLTAG;
	tag_t			window								= NULLTAG;
	tag_t			window_BS								= NULLTAG;
	tag_t			rule_BS								= NULLTAG;
	tag_t			rule								= NULLTAG;
	tag_t			top_line							= NULLTAG;
	tag_t			top_line_BS							= NULLTAG;
	char			*view_type_name=NULL;
	tag_t			view_type_tag							= NULLTAG;
	char			*bs_view_type_name							= NULL;
	int				bvr_count_grp98Rev=0;
	int				bvr_count_bsRev=0;
	int				bvr_count_BSCRev=0;
	int				bvrCnt=0;
	tag_t			*bvrs_grp98Rev= NULLTAG;
	tag_t			*bvrs_BSRev= NULLTAG;
	tag_t			*view_type= NULLTAG;
	tag_t			bom_view_grp98					= NULLTAG;
	tag_t			bom_view_bs					= NULLTAG;
	tag_t			bvr_grp98Rev1						= NULLTAG;
	tag_t			bvr_BSRev1						= NULLTAG;
	tag_t			*bvrs_BSCRev						= NULLTAG;
	tag_t	*occurrences						= NULLTAG;
	double consumableQtyD;
	const char *qry_entries[1];
	const char *qry_values[1];

	int n_tags_found1=0;
	tag_t	*itemclass1	= NULLTAG;
	tag_t	itemclassp	= NULLTAG;
	int		Grp98Alreadyattach	= 0;
	int		cnt	= 0;
	int		childCntBS	= 0;
	tag_t  *children;
	int   attribute_act_tak=0;
	int   childBS=0;
	int   grp98AttachToDMLFlag=0;
	int   Fndrelation=0;
	int   n_values_bvr_bs=0;
	tag_t  	value_act_Tak;
	tag_t			tsk_part_rel_type= NULLTAG;
	tag_t	tsk_part_APL_rel	= NULLTAG;
	tag_t			status_rel					= NULLTAG;
	tag_t			finalitem_bdySh_childprt					= NULLTAG;
	logical			retain							= 0;
	const char *qry_entries_BS[1];
	const char *qry_values_BS[1];

	int n_tags_found_BS=0;
	tag_t	*itemclass_BS							= NULLTAG;
	tag_t item_BS = NULLTAG;
	tag_t item_BSrev_tag = NULLTAG;
	char *  ChildRelErr = NULL;

	int	bdyshpart_revcount1	= 0;
	int	StructCntBdyShProdPlan	= 0;
	int	bsii	= 0;
	tag_t *  bdyshrev_partlist;
	char  	bdysh_part_rev_id1[ITEM_id_size_c+1];
	tag_t  CurrentRoleTag = NULLTAG;
	char roleName[SA_name_size_c+1];
	char *PlantName=NULL;
	char *ItemName_BS=NULL;
	char ContexPlantVal[40];
	tag_t objChild_BS		= NULLTAG;
	int	 iChildItemTag_BS	= 0;
	tag_t t_ChildItemRev_BS	= NULLTAG;


	ITKCALL (TCTYPE_ask_object_type(sTaskTag,&itemTypeTag_cdss));
	ITKCALL (TCTYPE_ask_name(itemTypeTag_cdss,type_itemRev));
	if(tc_strcmp(type_itemRev,"T5_APLTaskRevision" )==0)
	{

		ITKCALL(AOM_ask_value_string(sTaskTag,"t5_crdesigngroup",&design_grp_no));
		printf("\t  design_grp_no ...%s\n", design_grp_no);fflush(stdout);

		ITKCALL(AOM_ask_value_string(sTaskTag,"item_id",&itemid));
		printf("\t  itemid ...%s\n", itemid);fflush(stdout);



		if(((tc_strcmp(design_grp_no,"00")==0) || (tc_strcmp(design_grp_no,"98")==0)) &&(tc_strstr(itemid,"AM")==NULL))
		{
			tc_strcpy(apldmlnumberCpy,itemid);
			ERCDMLNo = strtok( apldmlnumberCpy, "_" );
			printf("DMLNo %s:",ERCDMLNo);fflush(stdout);

			//tc_strcpy(newercdmlnumber,ERCDMLNo);
			//tc_strcat(newercdmlnumber,"/");

			//printf("newercdmlnumber %s:",newercdmlnumber);fflush(stdout);

			/*WSOM_clear_search_criteria(&ercdml_search_criteria);
			strcpy(ercdml_search_criteria.name,newercdmlnumber);
			strcpy(ercdml_search_criteria.class_name,"ChangeRequestRevision");
			//strcpy(ercdml_search_criteria.class_name,"ChangeRequestRevision");
			status	= WSOM_search(ercdml_search_criteria, &ercdml_number_found, &list_of_ercdml_tags);*/

			//ITKCALL(ITEM_find_item(ERCDMLNo,&erc_dml_tag));

			erc_dml_qry_entries[0] ="item_id";
			erc_dml_qry_values[0] = ERCDMLNo;
			//erc_dml_rev_id= "A";
			ITKCALL(ITEM_find_item_revs_by_key_attributes(1,erc_dml_qry_entries,erc_dml_qry_values,erc_dml_rev_id,&ercdml_tags_found,&erc_dml_tag));

			printf("\n ercdml_tags_found is : %d\n",ercdml_tags_found);fflush(stdout);

			erc_dml_revision = erc_dml_tag[0];
			if(erc_dml_revision != NULLTAG )
			//ITKCALL(ITEM_ask_latest_rev(erc_dml_revision,&erc_dml_rev_tag));

			if (erc_dml_revision!=NULLTAG)
			{
				//for(j=0;j<ercdml_number_found;j++)
				//{

					ITKCALL(TCTYPE_ask_object_type(erc_dml_revision,&ERCObjTypProj));
					ITKCALL(TCTYPE_ask_name(ERCObjTypProj,dml_type));
					printf("\n dml_type :: %s\n", dml_type);fflush(stdout);
					if (tc_strcmp(dml_type,"ChangeRequestRevision")==0)
					{

						ITKCALL(AOM_ask_value_string( erc_dml_revision, "t5_rlstype", &erc_dmlrelztype));
						printf("\n erc_dmlrelztype: %s\n",erc_dmlrelztype);fflush(stdout);

						AOM_ask_value_string( erc_dml_revision, "t5_cprojectcode", &erc_dmlprojCode);
						printf("\n erc_dmlprojCode: %s\n",erc_dmlprojCode);fflush(stdout);

						project_qry_entries[0] ="item_id";
						project_qry_values[0] = erc_dmlprojCode;

						ITKCALL(ITEM_find_items_by_key_attributes(1, project_qry_entries, project_qry_values,&project_tags_found, &projectclass));
						project_item = projectclass[0];
						//if(project_item != NULLTAG )
						//ITKCALL(ITEM_ask_latest_rev(project_item,&sTaskTag));

						//ITKCALL(ITEM_find_item(erc_dmlprojCode,&Project_tag));
						if (project_item!=NULLTAG)
						{
							ITKCALL(TCTYPE_ask_object_type(project_item,&ObjTypProj));
							ITKCALL(TCTYPE_ask_name(ObjTypProj,type_Proj));
							printf("\n MT:DML: type_Proj :: %s\n", type_Proj);fflush(stdout);
							if (tc_strcmp(type_Proj,"T5_Project")==0)
							{
								ITKCALL(AOM_ask_value_string(project_item,"t5_VehicleClass",&Proj_VehicleClass));
								printf("\n Proj_VehicleClass :: %s\n", Proj_VehicleClass);fflush(stdout);
								if(tc_strcmp(Proj_VehicleClass,"")==0)
								{
									if (tc_strcmp(Grp98Err,"NULL") !=0) MEM_free(Grp98Err);
									Grp98Err=(char *)MEM_alloc(500);
									tc_strcpy(Grp98Err,"\n Vehicle Class Not Found for project  ");
									tc_strcat(Grp98Err,erc_dmlprojCode);
									tc_strcat(Grp98Err,"\n");
									cnt++;

									//EMH_clear_errors();
									//EMH_store_error_s1(EMH_severity_error,ITK_errStore1,Grp98Err);
									//return ITK_errStore1;

									if(tc_strcmp(SetChildRelErr,"NULL") ==0)
									{
										tc_strcpy(SetChildRelErr,Grp98Err);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,Grp98Err);
										cntFlag = 1;
									}


								}

							}

						}
					}



				//}
			}


		}
		flagGen98Grp=0;

		if( ((tc_strcmp(design_grp_no,"00")==0)&&(tc_strcmp(erc_dmlrelztype,"Veh")==0))   ||  ((tc_strcmp(design_grp_no,"98")==0)&&(tc_strcmp(erc_dmlrelztype,"CP")==0))
			||  ((tc_strcmp(design_grp_no,"00")==0)&&(tc_strcmp(erc_dmlrelztype,"CP")==0)) ||  ((tc_strcmp(design_grp_no,"00")==0)&&(tc_strcmp(erc_dmlrelztype,"CM")==0)) )
		{

			if (project_item!=NULLTAG)
			{
				ITKCALL(AOM_ask_value_string(project_item,"t5_BussUnitType",&Proj_BusUnit));
				printf("\n Proj_BusUnit :: %s\n", Proj_BusUnit);fflush(stdout);
				if(tc_strcmp(Proj_BusUnit,"")==0)
				{
					if (tc_strcmp(Grp98Err,"NULL") !=0) MEM_free(Grp98Err);
					Grp98Err=(char *)MEM_alloc(500);
					tc_strcpy(Grp98Err,"\n Business Unit Not Found for project  ");
					tc_strcat(Grp98Err,erc_dmlprojCode);
					tc_strcat(Grp98Err,"\n");
					cnt++;

					//EMH_clear_errors();
					//EMH_store_error_s1(EMH_severity_error,ITK_errStore1,Grp98Err);
					//return ITK_errStore1;
					//if(cnt==1)
					if(tc_strcmp(SetChildRelErr,"NULL") ==0)
					{
						tc_strcpy(SetChildRelErr,Grp98Err);
						cntFlag = 1;
					}
					else
					{
						tc_strcat(SetChildRelErr,Grp98Err);
						cntFlag = 1;
					}


				}
				else
				{
					if(tc_strcmp(Proj_BusUnit,"PCBU and CVBU Both")==0)
					{
						if((tc_strcmp(erc_dmlrelztype,"CP")!=0) || (tc_strcmp(erc_dmlrelztype,"CM")!=0))
						{

							PcbuCvbuFlag = true;

						}

					}

					if (PcbuCvbuFlag == true || (tc_strcmp(Proj_BusUnit,"CVBU")==0) )
					{
						printf("\n INSIDE CVBU 98 GRP");fflush(stdout);


					}
					else if (PcbuCvbuFlag == false || (tc_strcmp(Proj_BusUnit,"PCBU")==0))
					{

							printf("\n INSIDE NON_CAR PCBU 98 GRP");fflush(stdout);
							if(tc_strcmp(Proj_VehicleClass,"CAR")==0)
							{
								printf("\n INSIDE CAR 98 GRP");fflush(stdout);
								ITKCALL(AOM_ask_value_tags(sTaskTag,"CMHasSolutionItem",&PartCnt,&PartTags));
								if (PartCnt>0)
								{
									
									
									ITKCALL(SA_ask_current_role(&CurrentRoleTag));
									ITKCALL(SA_ask_role_name(CurrentRoleTag,roleName))
									printf("\n\n  roleName : %s\n",roleName); fflush(stdout);
									PlantName=subString(roleName,3,4);
									printf( "PlantName:%s\n", PlantName);fflush(stdout);
									if(tc_strcmp(PlantName,"APLD")==0)
									{
										tc_strcpy(ContexPlantVal,"DHARWAD");
									}
									else  if(tc_strcmp(PlantName,"APLP")==0)
									{
										tc_strcpy(ContexPlantVal,"CVBU Pune");								
									}
									else  if(tc_strcmp(PlantName,"APLC")==0)
									{								
										tc_strcpy(ContexPlantVal,"CAR");
									}
									else  if(tc_strcmp(PlantName,"APLJ")==0)
									{
										tc_strcpy(ContexPlantVal,"CVBU JSR");								
									}
									else  if(tc_strcmp(PlantName,"APLL")==0)
									{
										tc_strcpy(ContexPlantVal,"CVBU LKO");								
									}
									else  if(tc_strcmp(PlantName,"APLA")==0)
									{
										tc_strcpy(ContexPlantVal,"SMALLCAR AHD");								
									}
									else  if(tc_strcmp(PlantName,"APLU")==0)
									{
										tc_strcpy(ContexPlantVal,"CVBU PNR");								
									}
									else  if(tc_strcmp(PlantName,"APLS")==0)
									{
										tc_strcpy(ContexPlantVal,"CVBU JSR");								
									}
									else
									{
										tc_strcpy(ContexPlantVal,"CVBU Pune");								
									}
								
									 printf( "ContexPlantVal:%s\n", ContexPlantVal);fflush(stdout);
									
									for (k=0;k<PartCnt ;k++ )
									{
										printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
										AssyTag=PartTags[k];

										ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
										printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);

										NewPart_no=subString(Part_no,0,8);
										printf("\n\n\t\t NewPart_no  is :%s",NewPart_no);	fflush(stdout);

										ITKCALL(AOM_ask_value_string(AssyTag,"t5_PartType",&Part_type));
										printf("\n\n\t\t Part_type  is :%s",Part_type);	fflush(stdout);

										ITKCALL(AOM_ask_value_string(AssyTag,"t5_ColourInd",&Part_ColorIndicator));
										printf("\n\n\t\t Part_ColorIndicator  is :%s",Part_ColorIndicator);	fflush(stdout);

										ITKCALL(AOM_ask_value_string(AssyTag,"t5_ColourID",&Part_ColorID));
										printf("\n\n\t\t Part_ColorID  is :%s",Part_ColorID);	fflush(stdout);

										ITKCALL(AOM_ask_value_string(AssyTag,"t5_ProjectCode",&Part_ProjID));
										printf("\n\n\t\t Part_ProjID  is :%s",Part_ProjID);	fflush(stdout);

										ITKCALL(AOM_ask_value_string(AssyTag,"t5_DesignGrp",&Part_DesgnGrp));
										printf("\n\n\t\t Part_DesgnGrp  is :%s",Part_DesgnGrp);	fflush(stdout);

										if((tc_strcmp(Part_type,"VC")==0) && (tc_strcmp(Part_ColorIndicator,"C")==0))
										{

											if(tc_strcmp(Part_ColorID,"")==0)
											{
												if (tc_strcmp(Grp98Err,"NULL") !=0) MEM_free(Grp98Err);
												Grp98Err=(char *)MEM_alloc(500);
												tc_strcpy(Grp98Err,"\n Colour ID is NULL for Vehicle Combination  ");
												tc_strcat(Grp98Err,Part_no);
												tc_strcat(Grp98Err,"\n");
												cnt++;

												//EMH_clear_errors();
												//EMH_store_error_s1(EMH_severity_error,ITK_errStore1,Grp98Err);
												//return ITK_errStore1;
												//if(cnt==1)
												if(tc_strcmp(SetChildRelErr,"NULL") ==0)
												{
													tc_strcpy(SetChildRelErr,Grp98Err);
													cntFlag = 1;
												}
												else
												{
													tc_strcat(SetChildRelErr,Grp98Err);
													cntFlag = 1;
												}


											}
											for (k1=0;k1<PartCnt ;k1++ )
											{
												printf("\n\n\t\t 11APL DML Cre:for k =:%d",k1);fflush(stdout);
												AssyTag_SerBody=PartTags[k1];

												ITKCALL(AOM_ask_value_string(AssyTag_SerBody,"item_id",&AssyPart_Number));
												printf("\n\n\t\t AssyPart_Number  is :%s",AssyPart_Number);	fflush(stdout);

												ITKCALL(AOM_ask_value_string(AssyTag_SerBody,"t5_PartType",&AssyPart_type));
												printf("\n\n\t\t AssyPart_type  is :%s",AssyPart_type);	fflush(stdout);

												ITKCALL(AOM_ask_value_string(AssyTag_SerBody,"t5_ColourInd",&AssyPart_ColorIndicator));
												printf("\n\n\t\t AssyPart_ColorIndicator  is :%s",AssyPart_ColorIndicator);	fflush(stdout);

												ITKCALL(AOM_ask_value_string(AssyTag_SerBody,"t5_PrtCatCode",&AssyPart_CatCode));
												printf("\n\n\t\t AssyPart_CatCode  is :%s",AssyPart_CatCode);	fflush(stdout);

												ITKCALL(AOM_ask_value_string(AssyTag_SerBody,"t5_ColourID",&AssyPart_ColorID));
												printf("\n\n\t\t AssyPart_ColorID  is :%s",AssyPart_ColorID);	fflush(stdout);

												ITKCALL(AOM_ask_value_string(AssyTag_SerBody,"item_revision_id",&AssyPart_revision));
												printf("\n\n\t\t AssyPart_revision  is :%s",AssyPart_revision);	fflush(stdout);

												ITKCALL(POM_attr_id_of_attr("structure_revisions","Design_0_Revision_alt",&bs_bvr_strc_revList));fflush(stdout);
												ITKCALL(POM_length_of_attr(AssyTag_SerBody, bs_bvr_strc_revList, &n_values_bvr_bs) );
												printf("\n n_values_bvr_bs is :%d\n",n_values_bvr_bs);fflush(stdout);




												if((tc_strcmp(AssyPart_type,"A")==0) && (tc_strcmp(AssyPart_CatCode,"BODY_SHELL")==0) && (tc_strcmp(AssyPart_ColorID,Part_ColorID)==0))
												{

													//COLOUR SR LOGIC NEED TO BE IMPLEMENTED AFTER COLOUR MODULE LIVE //ERRROR PCBU0169 PCBU003 ClAttNull
													flagGen98Grp++;
													ITKCALL(AOM_ask_value_tags(AssyTag,"T5_VCConsumableTemplateRel",&consmTempCnt,&consmTempTags));


													if (tc_strcmp(grp98Name,"NULL") !=0) MEM_free(grp98Name);
													grp98Name=(char *)MEM_alloc(500);
													tc_strcpy(grp98Name,NewPart_no);
													tc_strcat(grp98Name,"98R"); // HARDCODED
													tc_strcat(grp98Name,"BL_TEST"); // HARDCODED

													ITKCALL(WSOM_clear_search_criteria(&grp98_searchcriteria));
													strcpy(grp98_searchcriteria.name,grp98Name);
													strcpy(grp98_searchcriteria.class_name,"Design Revision");
													status	= WSOM_search(grp98_searchcriteria, &number_found, &list_of_grp98_tags);

													printf("\n\n\t\t number_found is : %d,consmTempCnt %d\n",number_found,consmTempCnt);fflush(stdout);
													if((consmTempCnt==0) && (number_found==0))
													{
														printf("\n\n\t\t Inside error of COnsumable Template not assigned..");fflush(stdout);

														if (tc_strcmp(Grp98Err,"NULL") !=0) MEM_free(Grp98Err);
														Grp98Err=(char *)MEM_alloc(500);
														tc_strcpy(Grp98Err,"\n Please Assign Consumable Template to VC  ");
														tc_strcat(Grp98Err,Part_no);
														tc_strcat(Grp98Err," using option APL Menu-->Grp98-->Assign Consumable Template");
														tc_strcat(Grp98Err,"\n");
														cnt++;

														//EMH_clear_errors();
														//EMH_store_error_s1(EMH_severity_error,ITK_errStore1,Grp98Err);
														//return ITK_errStore1;
														//if(cnt==1)
														if(tc_strcmp(SetChildRelErr,"NULL") ==0)
														{
															tc_strcpy(SetChildRelErr,Grp98Err);
															cntFlag = 1;
														}
														else
														{
															tc_strcat(SetChildRelErr,Grp98Err);
															cntFlag = 1;
														}
													}
													if(number_found==0)
													{
														ITKCALL(ITEM_create_item(grp98Name,grp98Name,"Design","NR;1",&newGrp98item,&newGrp98rev));
														printf("\n Item created first time only with item_id %s\n",grp98Name);fflush(stdout);

														ITKCALL(AOM_save(newGrp98item));
														ITKCALL(AOM_save(newGrp98rev));
														//ITKCALL(AOM_unlock(newGrp98item));
														//ITKCALL(AOM_unlock(newGrp98rev));

														ITKCALL(WSOM_clear_search_criteria(&grp98_searchcriteria1));
														strcpy(grp98_searchcriteria1.name,grp98Name);
														strcpy(grp98_searchcriteria1.class_name,"Design Revision");
														status	= WSOM_search(grp98_searchcriteria1, &number_found1, &list_of_grp98Rev_tags);

														printf("\n\n\t\t number_found1 is : %d\n",number_found1);fflush(stdout);
														if(number_found1>0)
														{
															for(jRev=0;jRev<number_found1;jRev++)
															{
																printf("\n Setting the values for grp 98..");fflush(stdout);

																list_of_grp98=list_of_grp98Rev_tags[jRev];

																ITKCALL(AOM_ask_value_string( list_of_grp98, "item_id", &item_id_grp98));
																printf("\n\n\t\t item_id_grp98 is : %s\n",item_id_grp98);fflush(stdout);

																ITKCALL(AOM_lock(list_of_grp98));

																ITKCALL(AOM_set_value_string(list_of_grp98,"object_desc","TPL GR98-TPL FOR DEFINING CONSUMABLES BELOW BODY_SHELL"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_PartCode","Z"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_PartType","T"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_DrawingInd","N"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_PunPCBUMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_PnrMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_LkoMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_PunMakeBuyIndicator","E50"));
																//ITKCALL(AOM_set_value_string(list_of_grp98,"t5_PunUVMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_JsrMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_CarMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_AhdMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_SgrMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_DwdMakeBuyIndicator","E50"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_DsgnOwn","TELPUN"));
																//CALLAPI(AOM_set_value_string(list_of_grp98,"t5_ColourID","TELPUN"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_ColourInd","C"));
																ITKCALL(AOM_set_value_logical(list_of_grp98,"t5_CMVRCertificationReqd",false));
																ITKCALL(AOM_set_value_logical(list_of_grp98,"t5_Dismantable",false));
																ITKCALL(AOM_set_value_logical(list_of_grp98,"t5_ListRecSpares",false));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_EnvelopeDimensions","1234"));
																ITKCALL(AOM_set_value_double(list_of_grp98,"t5_Weight","0.0"));
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_ProjectCode",Part_ProjID)); //ADDED FOR TZ1.38
																ITKCALL(AOM_set_value_string(list_of_grp98,"t5_DesignGrp","98"));

																//ITKCALL(CR_create_release_status("T5_LcsAPLWrkg",&status_rel));
																//ITKCALL(EPM_add_release_status(status_rel,1,&list_of_grp98,retain));

																ITKCALL( AOM_save(list_of_grp98) );
																ITKCALL( AOM_unlock(list_of_grp98) );

																ITKCALL(CR_create_release_status("T5_LcsAPLWrkg",&status_rel));
																ITKCALL(EPM_add_release_status(status_rel,1,&list_of_grp98,retain));


															}
															printf("\n\n Adding Release Status");fflush(stdout);

															/*ITKCALL(AOM_unlock(newGrp98item));
															ITKCALL(AOM_lock(newGrp98item));
															ITKCALL(CR_create_release_status("T5_LcsAPLWrkg",&status_rel));
															ITKCALL(EPM_add_release_status(status_rel,1,&newGrp98item,retain));

															ITKCALL( AOM_save(newGrp98item) );
															ITKCALL( AOM_unlock(newGrp98item) );*/

															ITKCALL(AOM_ask_value_tags(AssyTag,"T5_VCConsumableTemplateRel",&consmTempCnt,&consmTempTags));
															if(consmTempCnt>0)
															{
																for (conTpCnt=0;conTpCnt<consmTempCnt ;conTpCnt++ )
																{
																	printf("\n\n Inside Consumable template relation=:%d",conTpCnt);fflush(stdout);
																	TemplateTag=consmTempTags[conTpCnt];

																	if (TemplateTag!=NULLTAG)
																	{
																		ITKCALL(AOM_ask_value_string(TemplateTag,"item_id",&template_id));
																		printf("\n\n\t\t template_id  is :%s",template_id);	fflush(stdout);

																		ITKCALL(AOM_ask_value_tags(TemplateTag,"T5_ConsumableTempPartRelatn",&consmTempPartCnt,&consmTempPartTags));
																		ITKCALL(AOM_ask_relations(TemplateTag,"T5_ConsumableTempPartRelatn",&num,&consmTempPartrelation_tags));
																		if(consmTempPartCnt>0)
																		{

																			ITKCALL(BOM_create_window (&window));
																			//ITKCALL(CFM_find( "Latest Working", &rule )); //ADDED FOR TZ1.38
																			ITKCALL(CFM_find( "ERC release and above", &rule ));
																			ITKCALL(BOM_set_window_config_rule( window, rule ));
																			ITKCALL(BOM_set_window_pack_all (window, true));
																			ITKCALL(BOM_set_window_top_line (window, newGrp98item, null_tag, null_tag, &top_line));
																			ITKCALL(BOM_save_window (window));


																			ITKCALL(PS_ask_default_view_type (&view_type_tag));
																			if(view_type_tag==NULLTAG)
																			{
																				printf("\n view_type_tag is null");fflush(stdout);
																			}
																			else
																			{
																				printf("\n view_type_tag is not null ");fflush(stdout);
																			}


																			ITKCALL(PS_ask_view_type_name (view_type_tag,&bs_view_type_name));
																			printf("\n bs_view_type_name %s ",bs_view_type_name);fflush(stdout);


																			ITKCALL(PS_create_bom_view (view_type_tag,"","",newGrp98item,&bom_view_grp98));
																			ITKCALL(AOM_save(bom_view_grp98));
																			ITKCALL(AOM_save(newGrp98item));
																			ITKCALL(AOM_unlock(newGrp98item));

																			ITKCALL(ITEM_rev_list_bom_view_revs( newGrp98rev, &bvr_count_grp98Rev, &bvrs_grp98Rev));
																			printf("\n bvr_count_grp98Rev %d ",bvr_count_grp98Rev);fflush(stdout);


																			ITKCALL(PS_create_bvr (bom_view_grp98,"","",false,newGrp98rev,&bvr_grp98Rev1));
																			ITKCALL(AOM_save(bvr_grp98Rev1));
																			ITKCALL(AOM_save(newGrp98rev));
																			ITKCALL(AOM_unlock(newGrp98rev));

																			for (conTpPartCnt=0;conTpPartCnt<consmTempPartCnt;conTpPartCnt++ )
																			{

																				printf("\n\n Inside Consumable template part relation=:%d,RelationCnt %d:",conTpPartCnt,num);fflush(stdout);
																				PartTag=consmTempPartTags[conTpPartCnt];
																				PartRelTag=consmTempPartrelation_tags[conTpPartCnt];

																				ITKCALL(AOM_ask_value_string(PartTag,"item_id",&part_id));
																				printf("\n\n\t\t part_id  is :%s",part_id);fflush(stdout);

																				ITKCALL(ITEM_ask_latest_rev(PartTag,&PartTagrev));

																				ITKCALL(AOM_ask_value_string(PartRelTag,"t5_ConsumableClass",&consumableClass));
																				printf("\n\n\t\t consumableClass  is :%s",consumableClass);fflush(stdout);

																				ITKCALL(AOM_ask_value_string(PartRelTag,"t5_ConsumableQty",&consumableqty));
																				printf("\n\n\t\t consumableqty  is :%s",consumableqty);fflush(stdout);

																				consumableQtyD=atof(consumableqty);

																				//COLOUR SR LOGIC NEED TO BE IMPLEMENTED AFTER COLOUR MODULE LIVE...
																				if(tc_strstr(part_id,"G")!=NULL)
																				{

																					printf("\n\n\t\t Inside GRP Part");fflush(stdout);
																					ITKCALL(PS_create_occurrences(bvr_grp98Rev1,PartTagrev,NULLTAG,1,&occurrences));
																					if(tc_strcmp(consumableqty,"AR")==0)
																					{
																						consumableQtyD=1;
																						ITKCALL(PS_set_occurrence_qty( bvr_grp98Rev1, occurrences[0], consumableQtyD ));
																						//ITKCALL(PS_set_occurrence_qty( bvr_grp98Rev1, bl_occ_t5_CurrentViewMaskC[0], 1 ));
																					}
																					else
																					{
																						ITKCALL(PS_set_occurrence_qty( bvr_grp98Rev1, occurrences[0], consumableQtyD ));
																					}
																					ITKCALL(AOM_save(bvr_grp98Rev1));




																				}
																				else
																				{
																					printf("\n\n\t\t Inside Other than GRP Part");	fflush(stdout);
																					ITKCALL(PS_create_occurrences(bvr_grp98Rev1,PartTagrev,NULLTAG,1,&occurrences));
																					ITKCALL(PS_set_occurrence_qty( bvr_grp98Rev1, occurrences[0], consumableQtyD ));
																					ITKCALL(AOM_save(bvr_grp98Rev1));
																					printf("\n\n\t\tBOM Created...");fflush(stdout);



																				}





																			}
																			ITKCALL(BOM_close_window(window));
																			printf("\n\n\t\tClosing BOM WIndow");fflush(stdout);


																		}


																	}




																}



															}


															//SEARCH FOR GRP98 //

															printf("\n\n\t\t Searching for BODY SHells");fflush(stdout);
															n_tags_found1=0;
															// char *qry_entries_1[1];
															//tag_t	*itemclass1							= NULLTAG;
															qry_entries[0] ="item_id";
															qry_values[0] =  AssyPart_Number;
															printf("\n\n\t\t 11 Searching for BODY SHells");fflush(stdout);
															ITKCALL(ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found1, &itemclass1));
															itemclassp = itemclass1[0];
															printf("\n\n\t\t 22 Searching for BODY SHells");fflush(stdout);
															cnt = 0;


															
															//ADDED FOR TZ1.38
															/*
															ITKCALL(BOM_create_window(&window));
															ITKCALL(BOM_set_window_top_line(window,itemclassp,null_tag,null_tag,&top_line));
															ITKCALL(BOM_line_ask_all_child_lines(top_line,&childCntBS,&children));
															printf("\n\n\t in grp 98 no:of action taken are -->%d",childCntBS);fflush(stdout);
															ITKCALL(BOM_line_look_up_attribute 	("bl_revision",&attribute_act_tak));

															for ( childBS = 0; childBS < childCntBS; childBS++)
															{
																//const char *qry_entries[1];
																//const char *qry_values[1];
																//int n_tags_found2=0;
																//tag_t	*itemclass2							= NULLTAG;

																ITKCALL(BOM_line_ask_attribute_tag 	(children[childBS],attribute_act_tak,&value_act_Tak));
																ITKCALL(AOM_UIF_ask_value(value_act_Tak,"item_id",&child_Item_id));
																printf("\n\n\t Child Part String is Item_id-->%s",child_Item_id);fflush(stdout);
																if(tc_strcmp(child_Item_id,grp98Name)==0)
																{
																	Grp98Alreadyattach = 1;
																	break;
																}


															}*/

															ITKCALL(ITEM_list_all_revs (itemclassp, &bdyshpart_revcount1,  &bdyshrev_partlist ));
															for (bsii = 0; bsii < bdyshpart_revcount1; bsii++)
															{

																ITKCALL(ITEM_ask_rev_id( bdyshrev_partlist[bsii], bdysh_part_rev_id1));

																printf("\n bdysh_part_rev_id1 is  ---->%s\n", bdysh_part_rev_id1);fflush(stdout);//diplay the previous revision
																if(tc_strcmp(bdysh_part_rev_id1,AssyPart_revision)==0)
																{
																	printf("\n Matched rev id is in Grp98 ---->%s,%s\n", bdysh_part_rev_id1,AssyPart_revision);fflush(stdout);
																	finalitem_bdySh_childprt=bdyshrev_partlist[bsii];
																	break;
																
																}
															}


															ITKCALL(Get_Part_BOM_Lvl(finalitem_bdySh_childprt,1,PlantName,"ERC release and above",ContexPlantVal,&StructCntBdyShProdPlan));//Get Multilevel BOM for Product Plan Items
															printf("\n\t\t StructCntBdyShProdPlan:%d",StructCntBdyShProdPlan);fflush(stdout);

															childBS = 0;
															
															for (childBS = 1; childBS <= StructCntBdyShProdPlan; childBS++)
															{

																objChild_BS		= NULLTAG;
																
																iChildItemTag_BS	= 0;
																t_ChildItemRev_BS	= NULLTAG;

																printf("\n111 Print Item ID==>%d,%d\n",childBS,StructCntBdyShProdPlan);fflush(stdout);

																objChild_BS=get_BomChldStrut[childBS].child_objs;
																ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag_BS));
																ITKCALL(BOM_line_ask_attribute_tag(objChild_BS, iChildItemTag_BS, &t_ChildItemRev_BS));
																ITKCALL(AOM_ask_value_string(t_ChildItemRev_BS,"item_id",&ItemName_BS));
																printf("\n\n\t ItemName_BS-->%s",ItemName_BS);fflush(stdout);
																
																if(tc_strcmp(ItemName_BS,grp98Name)==0)
																{
																	Grp98Alreadyattach = 1;
																	break;
																}
															}
															
															if(Grp98Alreadyattach>0)
															{

																ITKCALL(GRM_find_relation_type("CMHasSolutionItem", &tsk_part_rel_type));
																if (tsk_part_rel_type!=NULLTAG)
																{

																	ITKCALL(GRM_find_relation(sTaskTag, list_of_grp98, tsk_part_rel_type ,&Fndrelation));
																	if(Fndrelation>0)
																	{
																		printf("\n\t Relation of task and grp98 Already Exist.\n" );fflush(stdout);
																	}
																	else
																	{
																		printf("\n\t Creating Relation of task and grp98 ....\n" );fflush(stdout);
																		ITKCALL(GRM_create_relation(sTaskTag, list_of_grp98, tsk_part_rel_type,  NULLTAG, &tsk_part_APL_rel));
																		ITKCALL(GRM_save_relation(tsk_part_APL_rel));
																	}


																}


															}
															else
															{

																const char *qry_entries_BS[1];
																const char *qry_values_BS[1];

																int n_tags_found_BS=0;
																tag_t	*itemclass_BS							= NULLTAG;
																tag_t item_BS = NULLTAG;

																qry_entries_BS[0] ="item_id";
																qry_values_BS[0] = AssyPart_Number;

																ITKCALL(ITEM_find_items_by_key_attributes(1, qry_entries_BS, qry_values_BS,&n_tags_found_BS, &itemclass_BS));
																item_BS = itemclass_BS[0];
																//if(item != NULLTAG )
																//ITKCALL(ITEM_ask_latest_rev(item,&sTaskTag));

																ITKCALL(BOM_create_window (&window_BS));
																//ITKCALL(CFM_find( "Latest Working", &rule_BS )); //ADDED FOR TZ1.38
																ITKCALL(CFM_find( "ERC release and above", &rule_BS ));
																ITKCALL(BOM_set_window_config_rule( window_BS, rule_BS ));
																ITKCALL(BOM_set_window_pack_all (window_BS, true));
																ITKCALL(BOM_set_window_top_line (window_BS, item_BS, null_tag, null_tag, &top_line_BS));
																ITKCALL(BOM_save_window (window_BS));


																ITKCALL(PS_ask_default_view_type (&view_type_tag));
																if(view_type_tag==NULLTAG)
																{
																	printf("\n view_type_tag is null");fflush(stdout);
																}
																else
																{
																	printf("\n view_type_tag is not null ");fflush(stdout);
																}


																ITKCALL(PS_ask_view_type_name (view_type_tag,&view_type_name));
																printf("\n view_type_name %s ",view_type_name);fflush(stdout);

																/*ITKCALL(PS_ask_bom_view_type(bom_view_bs,&view_type));
																if(view_type==NULLTAG)
																{
																	printf("\n view_type is null");
																}
																else
																{
																	printf("\n view_type is not null ");
																}*/

																if(n_values_bvr_bs==0)
																{
																	ITKCALL(PS_create_bom_view (view_type_tag,"","",item_BS,&bom_view_bs));
																	ITKCALL(AOM_save(bom_view_bs));
																	ITKCALL(AOM_save(item_BS));
																	ITKCALL(AOM_unlock(item_BS));

																	ITKCALL(ITEM_rev_list_bom_view_revs( AssyTag_SerBody, &bvr_count_bsRev, &bvrs_BSRev));
																	printf("\n bvr_count_bsRev %d ",bvr_count_bsRev);fflush(stdout);

																	ITKCALL(PS_create_bvr (bom_view_bs,"","",false,AssyTag_SerBody,&bvr_BSRev1));
																	ITKCALL(AOM_save(bvr_BSRev1));
																	ITKCALL(AOM_save(AssyTag_SerBody));
																	ITKCALL(AOM_unlock(AssyTag_SerBody));

																	consumableQtyD=1;

																	ITKCALL(PS_create_occurrences(bvr_BSRev1,list_of_grp98,NULLTAG,1,&occurrences));
																	ITKCALL(PS_set_occurrence_qty( bvr_BSRev1, occurrences[0], consumableQtyD ));
																	ITKCALL(AOM_save(bvr_BSRev1));


																}
																else
																{
																	ITKCALL(ITEM_rev_list_bom_view_revs( AssyTag_SerBody, &bvr_count_BSCRev, &bvrs_BSCRev));
																	printf("\n bvr_count_BSCRev %d ",bvr_count_BSCRev);fflush(stdout);

																	consumableQtyD=1;

																	for ( bvrCnt = 0; bvrCnt < bvr_count_BSCRev; bvrCnt++)
																	{
																		ITKCALL(PS_create_occurrences(bvrs_BSCRev[bvrCnt],list_of_grp98,NULLTAG,1,&occurrences));
																		ITKCALL(PS_set_occurrence_qty( bvrs_BSCRev[bvrCnt], occurrences[0], consumableQtyD ));
																		ITKCALL(AOM_save(bvrs_BSCRev[bvrCnt]));
																	}



																}
																ITKCALL(BOM_close_window(window_BS));


																ITKCALL(GRM_find_relation_type("CMHasSolutionItem", &tsk_part_rel_type));
																if (tsk_part_rel_type!=NULLTAG)
																{

																	ITKCALL(GRM_find_relation(sTaskTag, list_of_grp98, tsk_part_rel_type ,&Fndrelation));
																	if(Fndrelation>0)
																	{
																		printf("\n\t 11 Relation of task and grp98 Already Exist.\n" );fflush(stdout);
																	}
																	else
																	{
																		printf("\n\t 11Creating Relation of task and grp98 ....\n" );fflush(stdout);
																		ITKCALL(GRM_create_relation(sTaskTag, list_of_grp98, tsk_part_rel_type,  NULLTAG, &tsk_part_APL_rel));
																		ITKCALL(GRM_save_relation(tsk_part_APL_rel));
																	}


																}






															}



														}



													}
													else
													{

															const char *qry_entries[1];
															const char *qry_values[1];
															int n_tags_found1=0;
															tag_t	*itemclass1							= NULLTAG;

															qry_entries[0] ="item_id";
															qry_values[0] =  AssyPart_Number;
															ITKCALL(ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found1, &itemclass1));
															itemclassp = itemclass1[0];

															cnt = 0;

															/*ITKCALL(BOM_create_window(&window));
															ITKCALL(BOM_set_window_top_line(window,itemclassp,null_tag,null_tag,&top_line));
															ITKCALL(BOM_line_ask_all_child_lines(top_line,&childCntBS,&children));
															printf("\n\n\t in grp 98 no:of action taken are -->%d",childCntBS);fflush(stdout);
															ITKCALL(BOM_line_look_up_attribute 	("bl_revision",&attribute_act_tak));

															for ( childBS = 0; childBS < childCntBS; childBS++)
															{
																const char *qry_entries[1];
																const char *qry_values[1];
																int n_tags_found2=0;
																tag_t	*itemclass2							= NULLTAG;

																ITKCALL(BOM_line_ask_attribute_tag 	(children[childBS],attribute_act_tak,&value_act_Tak));
																ITKCALL(AOM_UIF_ask_value(value_act_Tak,"item_id",&child_Item_id));
																printf("\n\n\t Child Part String is Item_id-->%s",child_Item_id);fflush(stdout);
																if(tc_strcmp(child_Item_id,grp98Name)==0)
																{
																	Grp98Alreadyattach = 1;
																	break;
																}


															}*/

															
															ITKCALL(ITEM_list_all_revs (itemclassp, &bdyshpart_revcount1,  &bdyshrev_partlist ));
															for (bsii = 0; bsii < bdyshpart_revcount1; bsii++)
															{

																ITKCALL(ITEM_ask_rev_id( bdyshrev_partlist[bsii], bdysh_part_rev_id1));

																printf("\n 11 bdysh_part_rev_id1 is  ---->%s\n", bdysh_part_rev_id1);fflush(stdout);//diplay the previous revision
																if(tc_strcmp(bdysh_part_rev_id1,AssyPart_revision)==0)
																{
																	printf("\n 11 Matched rev id is in Grp98 ---->%s,%s\n", bdysh_part_rev_id1,AssyPart_revision);fflush(stdout);
																	finalitem_bdySh_childprt=bdyshrev_partlist[bsii];
																	break;
																
																}
															}

															ITKCALL(Get_Part_BOM_Lvl(finalitem_bdySh_childprt,1,PlantName,"ERC release and above",ContexPlantVal,&StructCntBdyShProdPlan));//Get Multilevel BOM for Product Plan Items
															printf("\n\t\t 11 StructCntBdyShProdPlan:%d",StructCntBdyShProdPlan);fflush(stdout);

															childBS = 0;
															
															for (childBS = 1; childBS <= StructCntBdyShProdPlan; childBS++)
															{

																objChild_BS		= NULLTAG;
																
																iChildItemTag_BS	= 0;
																t_ChildItemRev_BS	= NULLTAG;

																printf("\n111 Print Item ID==>%d,%d\n",childBS,StructCntBdyShProdPlan);fflush(stdout);

																objChild_BS=get_BomChldStrut[childBS].child_objs;
																ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag_BS));
																ITKCALL(BOM_line_ask_attribute_tag(objChild_BS, iChildItemTag_BS, &t_ChildItemRev_BS));
																ITKCALL(AOM_ask_value_string(t_ChildItemRev_BS,"item_id",&ItemName_BS));
																printf("\n\n\t 11 ItemName_BS-->%s",ItemName_BS);fflush(stdout);

																if(tc_strcmp(ItemName_BS,grp98Name)==0)
																{
																	Grp98Alreadyattach = 1;
																	break;
																}
															}
															if(Grp98Alreadyattach>0)
															{

																ITKCALL(GRM_find_relation_type("CMHasSolutionItem", &tsk_part_rel_type));
																if (tsk_part_rel_type!=NULLTAG)
																{

																	ITKCALL(GRM_find_relation(sTaskTag, list_of_grp98_tags[0], tsk_part_rel_type ,&Fndrelation));
																	if(Fndrelation>0)
																	{
																		printf("\n\t Relation of task and grp98 Already Exist.\n" );fflush(stdout);
																	}
																	else
																	{
																		printf("\n\t Creating Relation of task and grp98 ....\n" );fflush(stdout);
																		ITKCALL(GRM_create_relation(sTaskTag, list_of_grp98_tags[0], tsk_part_rel_type,  NULLTAG, &tsk_part_APL_rel));
																		ITKCALL(GRM_save_relation(tsk_part_APL_rel));
																	}


																}


															}
															else
															{

																const char *qry_entries_BS[1];
																const char *qry_values_BS[1];
																int n_tags_found_BS=0;
																tag_t	*itemclass_BS							= NULLTAG;

																qry_entries_BS[0] ="item_id";
																qry_values_BS[0] = AssyPart_Number;

																ITKCALL(ITEM_find_items_by_key_attributes(1, qry_entries_BS, qry_values_BS,&n_tags_found_BS, &itemclass_BS));
																item_BS = itemclass_BS[0];


																ITKCALL(BOM_create_window (&window_BS));
																//ITKCALL(CFM_find( "Latest Working", &rule_BS ));
																ITKCALL(CFM_find( "ERC release and above", &rule_BS ));
																ITKCALL(BOM_set_window_config_rule( window_BS, rule_BS ));
																ITKCALL(BOM_set_window_pack_all (window_BS, true));
																ITKCALL(BOM_set_window_top_line (window_BS, item_BS, null_tag, null_tag, &top_line_BS));
																ITKCALL(BOM_save_window (window_BS));


																ITKCALL(PS_ask_default_view_type (&view_type_tag));
																if(view_type_tag==NULLTAG)
																{
																	printf("\n view_type_tag is null");fflush(stdout);
																}
																else
																{
																	printf("\n view_type_tag is not null ");fflush(stdout);
																}


																ITKCALL(PS_ask_view_type_name (view_type_tag,&view_type_name));
																printf("\n view_type_name %s ",view_type_name);fflush(stdout);

																/*ITKCALL(PS_ask_bom_view_type(bom_view_bs,&view_type));
																if(view_type==NULLTAG)
																{
																	printf("\n view_type is null");
																}
																else
																{
																	printf("\n view_type is not null ");
																}*/

																if(n_values_bvr_bs==0)
																{
																	ITKCALL(PS_create_bom_view (view_type_tag,"","",item_BS,&bom_view_bs));
																	ITKCALL(AOM_save(bom_view_bs));
																	ITKCALL(AOM_save(item_BS));
																	ITKCALL(AOM_unlock(item_BS));

																	ITKCALL(ITEM_rev_list_bom_view_revs( AssyTag_SerBody, &bvr_count_bsRev, &bvrs_BSRev));
																	printf("\n bvr_count_bsRev %d ",bvr_count_bsRev);fflush(stdout);

																	ITKCALL(PS_create_bvr (bom_view_bs,"","",false,AssyTag_SerBody,&bvr_BSRev1));
																	ITKCALL(AOM_save(bvr_BSRev1));
																	ITKCALL(AOM_save(AssyTag_SerBody));
																	ITKCALL(AOM_unlock(AssyTag_SerBody));

																	consumableQtyD=1;

																	ITKCALL(PS_create_occurrences(bvr_BSRev1,list_of_grp98_tags[0],NULLTAG,1,&occurrences));
																	ITKCALL(PS_set_occurrence_qty( bvr_BSRev1, occurrences[0], consumableQtyD ));
																	ITKCALL(AOM_save(bvr_BSRev1));


																}
																else
																{
																	ITKCALL(ITEM_rev_list_bom_view_revs( AssyTag_SerBody, &bvr_count_BSCRev, &bvrs_BSCRev));
																	printf("\n bvr_count_BSCRev %d ",bvr_count_BSCRev);fflush(stdout);

																	consumableQtyD=1;
																	bvrCnt=0;

																	for ( bvrCnt = 0; bvrCnt < bvr_count_BSCRev; bvrCnt++)
																	{
																		ITKCALL(PS_create_occurrences(bvrs_BSCRev[bvrCnt],list_of_grp98_tags[0],NULLTAG,1,&occurrences));
																		ITKCALL(PS_set_occurrence_qty( bvrs_BSCRev[bvrCnt], occurrences[0], consumableQtyD ));
																		ITKCALL(AOM_save(bvrs_BSCRev[bvrCnt]));
																	}



																}
																ITKCALL(BOM_close_window(window_BS));


																ITKCALL(GRM_find_relation_type("CMHasSolutionItem", &tsk_part_rel_type));
																if (tsk_part_rel_type!=NULLTAG)
																{

																	ITKCALL(GRM_find_relation(sTaskTag, list_of_grp98_tags[0], tsk_part_rel_type ,&Fndrelation));
																	printf("\n\t Fndrelation ::%d\n",Fndrelation );fflush(stdout);
																	if(Fndrelation)
																	{
																		printf("\n\t 11 Relation of task and grp98 Already Exist.\n" );fflush(stdout);
																	}
																	else
																	{
																		printf("\n\t 11Creating Relation of task and grp98 ....\n" );fflush(stdout);
																		ITKCALL(GRM_create_relation(sTaskTag, list_of_grp98_tags[0], tsk_part_rel_type,  NULLTAG, &tsk_part_APL_rel));
																		ITKCALL(GRM_save_relation(tsk_part_APL_rel));
																	}


																}






															}





													}



												}
												if(flagGen98Grp>0)
												{
													break;
												}

											}


										}
										if(flagGen98Grp>0)
										{
											break;
										}


									}
								}




							}






					}





				}

			}


		}
		else
		{
			printf("This is not Vehicle DML.Bypassing t5Group98CheckNew\n");fflush(stdout);

		}




	}

	return 0;
}

int ERC_APL_DML_Checks(int PartCnt,tag_t	sTaskTag, tag_t* PartTags,char* SetChildRelErr,char* PlantCS)
{
			EPM_decision_t decision;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0,inp_len=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        tag_t	relation_type = NULLTAG;
        	int count = 0;
	        tag_t*			PartDmlTags			= NULLTAG;
         	int dml = 0;
	        tag_t			PartDmlTag				= NULLTAG;
			tag_t	itemTypeTag_class							= NULLTAG;
			char  type_class[TCTYPE_name_size_c+1];
	        char*	DmlId;
			char*			inp_task_no			= NULL;
			char			*inp_Plant			= NULL;
			char			*task_Plant			= NULL;
			char			*dml_type			= NULL;
			int task_len=0;
			int stCnt=0;

			ITKCALL(AOM_ask_value_string(sTaskTag,"item_id",&inp_task_no));
			printf("\n\n\t\t t5StdAMDMLValidation_Check_Func :inp_task_no  is :%s",inp_task_no);	fflush(stdout);

			if(strstr(inp_task_no,"AM"))
			{
				inp_len=0;
				inp_len=strlen(inp_task_no);
				if(strstr(inp_task_no,"NONERC"))
				{
					inp_Plant=subString(inp_task_no,18,inp_len);
				}
				else
				{
					inp_Plant=subString(inp_task_no,14,inp_len);
				}


                 // Start for ERC DML/AM DML check
				for (k=0;k<PartCnt ;k++ )
				{
					 int BypassNACSFlag =0;

					printf("\n\n\t\t APL DML Cre:for k_2 =:%d",k);fflush(stdout);
					AssyTag=PartTags[k];
					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }
					GRM_find_relation_type("CMHasSolutionItem",&relation_type);
					GRM_list_primary_objects_only(AssyTag,relation_type,&count,&PartDmlTags);
					//GRM_list_all_related_objects_only(AssyTag,&count,&PartDmlTags);

					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
					printf("\n\n\t\t STD DML Cre:Part_no  is :%s",Part_no);	fflush(stdout);

					printf("\n\n\t\t Part to ERC DML to Task : %d",count);fflush(stdout);
					if(count >0)
					{
				   for (dml=0;dml<count ;dml++ )
				   {
					PartDmlTag=PartDmlTags[dml];
					ITKCALL (TCTYPE_ask_object_type(PartDmlTag,&itemTypeTag_class));
					ITKCALL (TCTYPE_ask_name(itemTypeTag_class,type_class));

					printf("\t  type_itemRev ...%s\n", type_class);
					if(tc_strcmp(type_class,"T5_APLTaskRevision")!=0)
						continue;

				ITKCALL(AOM_ask_value_string(PartDmlTag, "item_id",&DmlId));
				printf("\n DmlId %s.....\n",DmlId);

				dml_type=subString(DmlId,2,2);
				printf("\ndml_type:%s",dml_type);fflush(stdout);

				task_len=0;
				task_len=strlen(DmlId);
				if(strstr(DmlId,"NONERC"))
				{
					task_Plant=subString(DmlId,18,task_len);
				}
				else
				{
					task_Plant=subString(DmlId,14,task_len);
				}
				printf("\n task_Plant:%s",task_Plant);fflush(stdout);
				printf("\n inp_Plant:%s",inp_Plant);fflush(stdout);
//
//					if(strstr(DmlId,"AM"))
//						continue;

					ITKCALL(WSOM_ask_release_status_list(PartDmlTag,&st_count,&status_list));
					printf("\n st_count:%d",st_count);fflush(stdout);

					//if (tc_strcmp(ChildRelErr,"NULL") !=0) MEM_free(ChildRelErr);
					ChildRelErr=NULL;
					ChildRelErr=(char *)MEM_alloc(1000);
					tc_strcpy(ChildRelErr," ");

					if(tc_strcmp(inp_Plant,task_Plant)==0)
					{
						if(tc_strcmp(dml_type,"PP")==0 || tc_strcmp(dml_type,"PM")==0)
							{

								if (st_count == 0)  /* No Status, so the Item is not yet Released */
								{
									printf("\n No Status, so the Item is not yet Released \n");

									tc_strcat(ChildRelErr,"PP/PM Series DML  ");
									tc_strcat(ChildRelErr,DmlId);
									tc_strcat(ChildRelErr,",");
									tc_strcat(ChildRelErr,"A");
									tc_strcat(ChildRelErr," for part ");
									tc_strcat(ChildRelErr,Part_no);
									tc_strcat(ChildRelErr," is not APL Released.");
									tc_strcat(ChildRelErr,"Please Release the PP/PM Series DML from APL first and try respective DML \n");
									printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
									cnt=cnt+1;


								}else
									{
									for(stCnt=0;stCnt<st_count;stCnt++)
										{
											ITKCALL(AOM_ask_value_string(status_list[stCnt],"object_name",&class_name));
											printf("\n class_name: %s\n",class_name);fflush(stdout);
											if( (tc_strcmp(class_name,"T5_LcsAplRlzd")!=0) && (stCnt==st_count-1) )
														{

															tc_strcat(ChildRelErr,"PP/PM Series DML ");
															tc_strcat(ChildRelErr,DmlId);
															tc_strcat(ChildRelErr,",");
															tc_strcat(ChildRelErr,"A");
															tc_strcat(ChildRelErr," for part ");
															tc_strcat(ChildRelErr,Part_no);
															tc_strcat(ChildRelErr," is not APL Released.");
															tc_strcat(ChildRelErr,"Please Release the PP/PM Series DML from APL first and try respective DML \n");
															printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
															cnt = cnt+1;
														}
										}
									}
							}
					}


									printf("\n strlen(ChildRelErr) :%d",strlen(ChildRelErr));fflush(stdout);
									if(strlen(ChildRelErr)>10)
									{
									tc_strcat(SetChildRelErr,ChildRelErr);
									cntFlag = 1;
									}
					}
				  }
				}// End for ERC DML/AM DML check
		}

	return 0;
}

int Hanging_Part_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS)
{
			EPM_decision_t decision;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        char * ItemName1 ;
			char * ItemRev ;
			char * ItemPType ;
			int  	n_parents=0;
			int * 	levels;
			tag_t * 	parents	 =NULLTAG;
			char *  HangingPrtErr = NULL;

			// Start for Hanging Part check
			for (k=0;k<PartCnt ;k++ )
				{
					int BypassNACSFlag =0;

					printf("\n\n\t\t APL DML Cre:for k_3 =:%d",k);fflush(stdout);
					ItemName1 = NULL;
					ItemRev = NULL;
					ItemPType = NULL;
					AssyTag=PartTags[k];

					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }

					// Other check related colour,std part,ECU part will be added as per colour module introduction at ERC
					ITKCALL(AOM_ask_value_string(AssyTag,"t5_PartType",&ItemPType));
					printf("\n child ItemPType:%s ..............",ItemPType);fflush(stdout);
					if(tc_strcmp(ItemPType,"V" )==0 || tc_strcmp(ItemPType,"VC" )==0 ||  tc_strcmp(ItemPType,"VCCR" )==0 ||tc_strcmp(ItemPType,"T" )==0 || tc_strcmp(ItemPType,"M" )==0  || tc_strcmp(ItemPType,"D" )==0|| tc_strcmp(ItemPType,"SA" )==0 ||  tc_strcmp(ItemPType,"SP" )==0 ||  tc_strcmp(ItemPType,"G" )==0)
					{
					printf("\n Part Type is V/VC/VCCR/T/M/D/SP/SA/G[[%s]]--->So continue No hanging part check..............",ItemPType);fflush(stdout);
					continue;
					}
					ITKCALL(PS_where_used_all(AssyTag,1,&n_parents,&levels,&parents));
					printf("\n\n\t\t No of Assy objects are n_parents : %d\n",n_parents);fflush(stdout);
					//GRM_list_all_related_objects_only(AssyTag,&count,&PartDmlTags);
					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&ItemName1));
					printf("\n child ItemName1:%s ..............",ItemName1);fflush(stdout);
					ITKCALL(AOM_ask_value_string(AssyTag,"item_revision_id",&ItemRev));
					printf("\n child ItemRev:%s ..............",ItemRev);fflush(stdout);

					if (tc_strcmp(HangingPrtErr,"NULL") !=0) MEM_free(HangingPrtErr);
					HangingPrtErr=(char *)MEM_alloc(500);
					tc_strcpy(HangingPrtErr," ");


					printf("\n\n\t\t Part to ERC DML to Task : %d",n_parents);fflush(stdout);
					if(n_parents<=0)
					{
							if(cnt==0)
							{
								cnt = 1;
								tc_strcat(HangingPrtErr,"PartNumber[");
								tc_strcat(HangingPrtErr,ItemName1);
								tc_strcat(HangingPrtErr,",");
								tc_strcat(HangingPrtErr,ItemRev);
								tc_strcat(HangingPrtErr,"]is Hanging.This part should have under some Assembly");
								 printf("\n ChildRelErr10: %s\n",HangingPrtErr);fflush(stdout);
							}
							else
							{
								tc_strcat(HangingPrtErr,"PartNumber[");
								tc_strcat(HangingPrtErr,ItemName1);
								tc_strcat(HangingPrtErr,",");
								tc_strcat(HangingPrtErr,ItemRev);
								tc_strcat(HangingPrtErr,"]is Hanging.This part should have under some Assembly");
								 printf("\n ChildRelErr11: %s\n",HangingPrtErr);fflush(stdout);
								cnt = cnt+1;
								 printf("\n ChildRelEr22r: %s\n",HangingPrtErr);fflush(stdout);
							}
						if(cnt==1)
						{
							tc_strcpy(SetChildRelErr,HangingPrtErr);
							cntFlag = 1;
						}
						else if(cnt>1)
						{
							tc_strcat(SetChildRelErr,HangingPrtErr);
							cntFlag = 1;
						}
					}
				}// End for Hanging Part check

	return 0;
}

int CS_Mismatch_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS,char* PlantOptCS)
{
			EPM_decision_t decision;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0,nG=0;
			tag_t	window					= NULLTAG;
			tag_t	rule							= NULLTAG;
			tag_t	top_line					= NULLTAG;
			tag_t	windowG					= NULLTAG;
			tag_t	ruleG							= NULLTAG;
			tag_t	top_lineG				= NULLTAG;
			tag_t  *childrenG            =NULLTAG;

			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Optional_cs = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
		    int Optional_CS =0;
	        char* class_name=NULL;
	        char * ItemName1 ;
			char * ItemRev ;
			char * ItemPType ;
			int  	n_parents=0;
			int * 	levels;
			tag_t * 	parents	 =NULLTAG;
			char *  HangingPrtErr = NULL;
			char   *PuneCS=NULL;
			char   *ParentPart=NULL;
			char * ItemName ;
			char * ItemNameG ;
			char * ItemType ;
			char * t_ChildIOptionalCS =NULL;
			char * t_ChildIOptionalCSg=NULL;
			char * ChildPuneCS ;
			tag_t  *children1=NULLTAG;
			tag_t   t_ChildItemRev;
			tag_t   t_ChildItemRevG;
	        int cl = 0;
	        int clg = 0;
	        int iChildItemTag;
	        int iChildItemTagG;
			tag_t *bvs, *bvrs;
			int    bv_count, bvr_count;
            tag_t *occs;
            int n_occs;
			tag_t   BvrPtr;
			tag_t   OccPtr;
        	logical * 	is_it_null = false;
            logical * 	is_it_empty	= false;
            int n_tags_line = 0;
		    tag_t *tags_line=NULLTAG;
            tag_t line_item;
          	tag_t attr_id ;
	        logical log1;

				// Start for Cs Mismatch check
				for (k=0;k<PartCnt ;k++ )
				{
					int BypassNACSFlag =0;

					printf("\n\n\t\t APL DML Cre:for k_4 =:%d",k);fflush(stdout);
					AssyTag=PartTags[k];

					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }

					ITKCALL(AOM_ask_value_string(AssyTag,PlantCS,&PuneCS));
					printf("\n Parent PuneCS:%s ..............",PuneCS);fflush(stdout);

					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&ParentPart));
					printf("\n child ParentPart:%s ..............",ParentPart);fflush(stdout);

					ITKCALL(BOM_create_window (&window));
					ITKCALL(CFM_find( "Latest Working", &rule));
					ITKCALL(BOM_set_window_config_rule( window, rule ));
					//ifail = BOM_set_window_pack_all( window, false );
					//CHECK_IFAIL;
					ITKCALL(BOM_set_window_top_line(window, null_tag,AssyTag , null_tag, &top_line));
					ITKCALL(BOM_line_ask_child_lines (top_line, &n, &children1));
					printf("\n\n\t\t No of child objects are n : %d\n",n);fflush(stdout);
			        ITKCALL(ITEM_rev_list_bom_view_revs( AssyTag, &bvr_count, &bvrs));
					printf("\n bvr_count  --->[%d]",bvr_count);

					for (cl = 0; cl < n; cl++)
					{

								ITKCALL(BOM_line_unpack (children1[cl]));
								ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
								ITKCALL(BOM_line_ask_attribute_tag(children1[cl], iChildItemTag, &t_ChildItemRev));

								ITKCALL(AOM_ask_value_string(t_ChildItemRev,"t5_PartType",&ItemType));
								printf("\n child ItemType:%s ..............",ItemType);fflush(stdout);

							   if((strcmp(ItemType,"G")!=0))  // Start for other than group
						       {
								BOM_line_look_up_attribute (PlantOptCS,&Optional_CS);
								printf("\n Optional_CS:%d: ..............",Optional_CS);fflush(stdout);
								BOM_line_ask_attribute_string(children1[cl], Optional_CS, &t_ChildIOptionalCS);
								printf("\n cItem_Optional_cs:%s ..............",t_ChildIOptionalCS);fflush(stdout);

								ITKCALL(AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemName));
								printf("\n child ItemName:%s ..............",ItemName);fflush(stdout);

								ITKCALL(AOM_ask_value_string(t_ChildItemRev,"item_revision_id",&ItemRev));
								printf("\n child ItemRev:%s ..............",ItemRev);fflush(stdout);

								ITKCALL(AOM_ask_value_string(t_ChildItemRev,PlantCS,&ChildPuneCS));
								printf("\n child ChildPuneCS:%s ..............",ChildPuneCS);fflush(stdout);
								if (tc_strcmp(ChildRelErr,"NULL") !=0) MEM_free(ChildRelErr);
								ChildRelErr=(char *)MEM_alloc(500);
								tc_strcpy(ChildRelErr," ");
								 // Parent as F CS and child part CS checks
								 if((strcmp(PuneCS,"F")==0) && (strcmp(ChildPuneCS,"E99")==0 || strcmp(ChildPuneCS,"E50")==0
																																				  || strcmp(ChildPuneCS,"E98")==0  || strcmp(ChildPuneCS,"F")==0
																																				  || strcmp(ChildPuneCS,"F30")==0  || strcmp(ChildPuneCS,"F19")==0  ))
								{
									printf("\n F under F not allowed");fflush(stdout);
										 if((strcmp(PuneCS,"F")==0) && (strcmp(t_ChildIOptionalCS,"E99 (In-house production-Back Flush Items)")==0 || strcmp(t_ChildIOptionalCS,"E50 (In-house production-Phantom Assembly)")==0
																											  || strcmp(t_ChildIOptionalCS,"E98 (In-house production-SPD items - Ph)")==0  || strcmp(t_ChildIOptionalCS,"F (External procurement)")==0
																											  || strcmp(t_ChildIOptionalCS,"F30 (External procurement - Subcontracting)")==0  || strcmp(t_ChildIOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
                                                                     										  || strcmp(t_ChildIOptionalCS,"")==0))
									{

									printf("\n F under F not allowed1");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as E50 CS and child part CS checks
							  if((strcmp(PuneCS,"E50")==0) && (strcmp(ChildPuneCS,"F18")==0
																																				   || strcmp(ChildPuneCS,"F19")==0  ))
								{
										  if((strcmp(PuneCS,"E50")==0) && (strcmp(t_ChildIOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0
																													   || strcmp(t_ChildIOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
											                                                                           || strcmp(t_ChildIOptionalCS,"")==0))
									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as E99 CS and child part CS checks
							  if((strcmp(PuneCS,"E99")==0) && (strcmp(ChildPuneCS,"F")==0 || strcmp(ChildPuneCS,"F30")==0
																																				   || strcmp(ChildPuneCS,"F18")==0  || strcmp(ChildPuneCS,"F19")==0))
								{
										  if((strcmp(PuneCS,"E99")==0) && (strcmp(t_ChildIOptionalCS,"F (External procurement)")==0 || strcmp(t_ChildIOptionalCS,"F30 (External procurement - Subcontracting)")==0
																													   || strcmp(t_ChildIOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0  || strcmp(t_ChildIOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
											                                                                           || strcmp(t_ChildIOptionalCS,"")==0))
									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as E98 CS and child part CS checks
							  if((strcmp(PuneCS,"E98")==0) && (strcmp(ChildPuneCS,"F")==0 || strcmp(ChildPuneCS,"F30")==0
																																				   || strcmp(ChildPuneCS,"F18")==0  || strcmp(ChildPuneCS,"F19")==0))
								{
									  if((strcmp(PuneCS,"E98")==0) && (strcmp(t_ChildIOptionalCS,"F (External procurement)")==0 || strcmp(t_ChildIOptionalCS,"F30 (External procurement - Subcontracting)")==0
																													   || strcmp(t_ChildIOptionalCS,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0  || strcmp(t_ChildIOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
										                                                                               || strcmp(t_ChildIOptionalCS,"")==0))
									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as F18 CS and child part CS checks
								 if((strcmp(PuneCS,"F18")==0)  && (strcmp(ChildPuneCS,"E99")==0 || strcmp(ChildPuneCS,"E50")==0
																																				  || strcmp(ChildPuneCS,"E98")==0  || strcmp(ChildPuneCS,"F")==0
																																				  || strcmp(ChildPuneCS,"F30")==0  || strcmp(ChildPuneCS,"F19")==0  ))
								  {
										 if((strcmp(PuneCS,"F18")==0)  && (strcmp(t_ChildIOptionalCS,"E99 (In-house production-Back Flush Items)")==0 || strcmp(t_ChildIOptionalCS,"E50 (In-house production-Phantom Assembly)")==0
																											  || strcmp(t_ChildIOptionalCS,"E98 (In-house production-SPD items - Ph)")==0  || strcmp(t_ChildIOptionalCS,"F (External procurement)")==0
																											  || strcmp(t_ChildIOptionalCS,"F30 (External procurement - Subcontracting)")==0  || strcmp(t_ChildIOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
											                                                                   || strcmp(t_ChildIOptionalCS,"")==0))
									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as F18 CS and child part CS checks
								 if((strcmp(PuneCS,"F19")==0)  && (strcmp(ChildPuneCS,"F30")==0  || strcmp(ChildPuneCS,"F19")==0  ))
								{
										 if((strcmp(PuneCS,"F19")==0)  && (strcmp(t_ChildIOptionalCS,"F30 (External procurement - Subcontracting)")==0  || strcmp(t_ChildIOptionalCS,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0  || strcmp(t_ChildIOptionalCS,"")==0))

									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child ");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							    }
							 } // End for other than group
							 else if((strcmp(ItemType,"G")==0))  // Check for group
						      {
							    printf("\n Inside Group Part check .");fflush(stdout);
								ITKCALL(AOM_ask_value_string(t_ChildItemRev,"item_id",&ItemNameG));
								printf("\n child ItemNameG:%s ..............",ItemNameG);fflush(stdout);

								ITKCALL(BOM_create_window (&windowG));
								ITKCALL(CFM_find( "Latest Working", &ruleG));
								ITKCALL(BOM_set_window_config_rule( windowG, ruleG ));
								//ifail = BOM_set_window_pack_all( window, false );
								//CHECK_IFAIL;
								ITKCALL(BOM_set_window_top_line(windowG, null_tag,t_ChildItemRev , null_tag, &top_lineG));
								ITKCALL(BOM_line_ask_child_lines (top_lineG, &nG, &childrenG));
								printf("\n\n\t\t No of child objects are n : %d\n",nG);fflush(stdout);
								for (clg = 0; clg < nG; clg++)
								{

								ITKCALL(BOM_line_unpack (childrenG[clg]));
								ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTagG));
								ITKCALL(BOM_line_ask_attribute_tag(childrenG[clg], iChildItemTagG, &t_ChildItemRevG));

   							    BOM_line_look_up_attribute (PlantOptCS,&Optional_CS);
								printf("\n Optional_CS:%d: ..............",Optional_CS);fflush(stdout);
								BOM_line_ask_attribute_string(childrenG[cl], Optional_CS, &t_ChildIOptionalCSg);
								printf("\n cItem_Optional_cs:%s ..............",t_ChildIOptionalCSg);fflush(stdout);
                                 if(t_ChildIOptionalCSg==NULL)
									{
							         t_ChildIOptionalCSg = "";
									printf("\n cItem_Optional_cs1:%s ..............",t_ChildIOptionalCSg);fflush(stdout);
									}

								ITKCALL(AOM_ask_value_string(t_ChildItemRevG,"item_id",&ItemName));
								printf("\n child ItemName:%s ..............",ItemName);fflush(stdout);

								ITKCALL(AOM_ask_value_string(t_ChildItemRevG,"item_revision_id",&ItemRev));
								printf("\n child ItemRev:%s ..............",ItemRev);fflush(stdout);

								ITKCALL(AOM_ask_value_string(t_ChildItemRevG,PlantCS,&ChildPuneCS));
								printf("\n child ChildPuneCS:%s ..............",ChildPuneCS);fflush(stdout);
								if (tc_strcmp(ChildRelErr,"NULL") !=0) MEM_free(ChildRelErr);
								ChildRelErr=(char *)MEM_alloc(500);
								tc_strcpy(ChildRelErr," ");
								 // Parent as F CS and child part CS checks
								 if((strcmp(PuneCS,"F")==0) && (strcmp(ChildPuneCS,"E99")==0 || strcmp(ChildPuneCS,"E50")==0
																																				  || strcmp(ChildPuneCS,"E98")==0  || strcmp(ChildPuneCS,"F")==0
																																				  || strcmp(ChildPuneCS,"F30")==0  || strcmp(ChildPuneCS,"F19")==0  ))
								{
									printf("\n F under F not allowed");fflush(stdout);
										 if((strcmp(PuneCS,"F")==0) && (strcmp(t_ChildIOptionalCSg,"E99 (In-house production-Back Flush Items)")==0 || strcmp(t_ChildIOptionalCSg,"E50 (In-house production-Phantom Assembly)")==0
																											  || strcmp(t_ChildIOptionalCSg,"E98 (In-house production-SPD items - Ph)")==0  || strcmp(t_ChildIOptionalCSg,"F (External procurement)")==0
																											  || strcmp(t_ChildIOptionalCSg,"F30 (External procurement - Subcontracting)")==0  || strcmp(t_ChildIOptionalCSg,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
                                                                     										  || strcmp(t_ChildIOptionalCSg,"")==0 ))
									{

									printf("\n F under F not allowed1");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as E50 CS and child part CS checks
							  if((strcmp(PuneCS,"E50")==0) && (strcmp(ChildPuneCS,"F18")==0
																																				   || strcmp(ChildPuneCS,"F19")==0  ))
								{
										  if((strcmp(PuneCS,"E50")==0) && (strcmp(t_ChildIOptionalCSg,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0
																													   || strcmp(t_ChildIOptionalCSg,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
											                                                                           || strcmp(t_ChildIOptionalCSg,"")==0 ))
									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as E99 CS and child part CS checks
							  if((strcmp(PuneCS,"E99")==0) && (strcmp(ChildPuneCS,"F")==0 || strcmp(ChildPuneCS,"F30")==0
																																				   || strcmp(ChildPuneCS,"F18")==0  || strcmp(ChildPuneCS,"F19")==0))
								{
										  if((strcmp(PuneCS,"E99")==0) && (strcmp(t_ChildIOptionalCSg,"F (External procurement)")==0 || strcmp(t_ChildIOptionalCSg,"F30 (External procurement - Subcontracting)")==0
																													   || strcmp(t_ChildIOptionalCSg,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0  || strcmp(t_ChildIOptionalCSg,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
											                                                                           || strcmp(t_ChildIOptionalCSg,"")==0))
									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as E98 CS and child part CS checks
							  if((strcmp(PuneCS,"E98")==0) && (strcmp(ChildPuneCS,"F")==0 || strcmp(ChildPuneCS,"F30")==0
																																				   || strcmp(ChildPuneCS,"F18")==0  || strcmp(ChildPuneCS,"F19")==0))
								{
									  if((strcmp(PuneCS,"E98")==0) && (strcmp(t_ChildIOptionalCSg,"F (External procurement)")==0 || strcmp(t_ChildIOptionalCSg,"F30 (External procurement - Subcontracting)")==0
																													   || strcmp(t_ChildIOptionalCSg,"F18 (External procurement-Assy/Comp recd in assembled condition with Party's matl)")==0  || strcmp(t_ChildIOptionalCSg,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
										                                                                               || strcmp(t_ChildIOptionalCSg,"")==0))
									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as F18 CS and child part CS checks
								 if((strcmp(PuneCS,"F18")==0)  && (strcmp(ChildPuneCS,"E99")==0 || strcmp(ChildPuneCS,"E50")==0
																																				  || strcmp(ChildPuneCS,"E98")==0  || strcmp(ChildPuneCS,"F")==0
																																				  || strcmp(ChildPuneCS,"F30")==0  || strcmp(ChildPuneCS,"F19")==0  ))
								  {
										 if((strcmp(PuneCS,"F18")==0)  && (strcmp(t_ChildIOptionalCSg,"E99 (In-house production-Back Flush Items)")==0 || strcmp(t_ChildIOptionalCSg,"E50 (In-house production-Phantom Assembly)")==0
																											  || strcmp(t_ChildIOptionalCSg,"E98 (In-house production-SPD items - Ph)")==0  || strcmp(t_ChildIOptionalCSg,"F (External procurement)")==0
																											  || strcmp(t_ChildIOptionalCSg,"F30 (External procurement - Subcontracting)")==0  || strcmp(t_ChildIOptionalCSg,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0
											                                                                   || strcmp(t_ChildIOptionalCSg,"")==0))
									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							  }else   // Parent as F18 CS and child part CS checks
								 if((strcmp(PuneCS,"F19")==0)  && (strcmp(ChildPuneCS,"F30")==0  || strcmp(ChildPuneCS,"F19")==0  ))
								{
										 if((strcmp(PuneCS,"F19")==0)  && (strcmp(t_ChildIOptionalCSg,"F30 (External procurement - Subcontracting)")==0  || strcmp(t_ChildIOptionalCSg,"F19 (External procurement-In-house machining of Assy/Comp(recd from outside party W/o TML Matl))")==0  || strcmp(t_ChildIOptionalCSg,"")==0))

									{
									printf("\n F18/F19 under E50 not allowd");fflush(stdout);
										if(cnt==0)
											{
												cnt = 1;
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
											}
										else
										{
												tc_strcat(ChildRelErr,"\n Parent Part ");
												tc_strcat(ChildRelErr,ParentPart);
												tc_strcat(ChildRelErr," Having CS ");
												tc_strcat(ChildRelErr,PuneCS);
												tc_strcat(ChildRelErr," is not allowed as Child Of Group ");
												tc_strcat(ChildRelErr,ItemNameG);
												tc_strcat(ChildRelErr,"/");
												tc_strcat(ChildRelErr,ItemName);
												tc_strcat(ChildRelErr," With CS  ");
												tc_strcat(ChildRelErr,ChildPuneCS);
												 printf("\n ChildRelErr11: %s\n",ChildRelErr);fflush(stdout);
												 cnt = cnt+1;
											 printf("\n ChildRelEr22r: %s\n",ChildRelErr);fflush(stdout);

										}
									if(cnt==1)
									{
										tc_strcpy(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									else
									{
										tc_strcat(SetChildRelErr,ChildRelErr);
										cntFlag = 1;
									}
									}
							    }
							   } //End of Loop
							  }// End check for group
						} // End of FOR loop
					}  // End for Cs Mismatch check

	return 0;
}

int Two_Rev_Same_Day_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* itemid,char* PlantCS)
{
			EPM_decision_t decision;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        char * ItemName1 ;
			char * ItemRev ;
			char * ItemPType ;
			int  	n_parents=0;
			int * 	levels;
			tag_t * 	parents	 =NULLTAG;
			int				task_len=0;
			int				inp_len=0;
			char			*inp_Plant			= NULL;
			char			*task_Plant			= NULL;
	        tag_t item = NULLTAG;
			tag_t  	objTypeTag_eff;
			char  	rev_id[ITEM_id_size_c+1];
		    tag_t *  rev_list;
		    tag_t *DMLRevision = NULLTAG;
			char  type_itemRev[TCTYPE_name_size_c+1];
			char  ChildRevtype_name[TCTYPE_name_size_c+1];
			tag_t	view_type							= NULLTAG;
			tag_t	item_rev_tag							= NULLTAG;
			tag_t	itemTypeTag_cdss							= NULLTAG;
			tag_t	*itemclassp	= NULLTAG;
			tag_t	value_child_details							= NULLTAG;
			char  req_id[ITEM_id_size_c+1];
			char*	 current_name		= NULL;
			char*	 creationdate		= NULL;
			char*	 ClosureDate		= NULL;
			char*	 ClosureDateDup		= NULL;
			char   type_eff[TCTYPE_name_size_c+1];
			int	   ii=0;
			int	   jj=0;
		    char  	rev_id1[ITEM_id_size_c+1];
			tag_t	itemclass							= NULLTAG;
			//tag_t	itemclassp							= NULLTAG;
			tag_t	itemclass_task						= NULLTAG;
			tag_t	objTypeRefTag						= NULLTAG;
			char            *desid				= NULL;
			char			*dml_type			= NULL;
			char			type_name_ref[TCTYPE_name_size_c+1];
			tag_t	item_rev_task						= NULLTAG;
			tag_t	reln_type_tag						= NULLTAG;
			tag_t tsk_dml_rel_type;
        	tag_t	relation_type = NULLTAG;
	        int count = 0;
	       tag_t*			PartDmlTags			= NULLTAG;
           tag_t			PartDmlTag				= NULLTAG;
           logical is_latest;
       	  char*	DmlId;
          tag_t DMLRevTag = NULLTAG;
		  char pAccessDate[20];
		  char cCurrentAccessDate[20]={0};
       	  char * pAccessDateDup 	= NULL;
		  char *  IsPPPPMRlzErr1 = NULL;
		  char *  SetIsPPPPMRlzErr1 = NULL;
	      int				cntErr=0;

		// Start for two rev. on same day check
				task_len=0;
				task_len=strlen(itemid);
				printf("\t  task_len ...%d\n", task_len);// length task
				inp_Plant=subString(itemid,14,task_len);
				printf("\t  inp_Plant after  ...%s\n", inp_Plant);//task

				for (k=0;k<PartCnt ;k++ )
				{
					const char *attrs[1];
					const char *values[1];
					int n_tags_found=0;
	                int				count1				= 0;
	                int				BypassNACSFlag				= 0;

					printf("\n\n\t\t for k_same rev =:%d",k);fflush(stdout);
					AssyTag=PartTags[k];

					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }

					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
					printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);
					 attrs[0] ="item_id";
					 values[0] = (char *)Part_no;
					ITKCALL(ITEM_find_items_by_key_attributes(1, attrs, values,&n_tags_found, &itemclassp));
					item = itemclassp[0];
					if(item != NULLTAG )
					ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag_p));

					ITKCALL (TCTYPE_ask_object_type(item_rev_tag_p,&objTypeTag_eff));
					ITKCALL (TCTYPE_ask_name(objTypeTag_eff,type_eff));

					printf("\n type_eff class is  ---->%s\n", type_eff);fflush(stdout);

					ITEM_ask_rev_id 	( item_rev_tag_p, rev_id);

					printf("\n rev_id is  ---->%s\n", rev_id);fflush(stdout);
					ITEM_list_all_revs  (item, &count1,  &rev_list );
					printf("\n\n\t\tWSOM count1:- %d\n\n",count1);
					for (ii = 0; ii < count1; ii++)
					{
						ITEM_ask_rev_id 	( rev_list[ii], rev_id1);

						printf("\n rev_id1 is  ---->%s\n", rev_id1);fflush(stdout);//diplay the previous revision

						printf("\n\n\t\t ii:%d:\n",ii);


					GRM_find_relation_type("CMHasSolutionItem",&relation_type);
					GRM_list_primary_objects_only(rev_list[ii],relation_type,&count,&PartDmlTags);

					printf("\n\n\t\t Part to ERC DML to Task : %d",count);fflush(stdout);
					if(count >0)
					{
				   for (k=0;k<count ;k++ )
				   {
					PartDmlTag=PartDmlTags[k];//task pointer
					printf("\n\n\t\t INSIDE: ");fflush(stdout);



						ITKCALL(TCTYPE_ask_object_type(PartDmlTag,&objTypeRefTag));//task pointer
						ITKCALL(TCTYPE_ask_name(objTypeRefTag,type_name_ref));
						printf("\nTCDCtype_name_ref isss :%s\n",type_name_ref);fflush(stdout);
			//

						  //if (strcmp(type_name_ref,"A9_APLTaskRevision")==0)
						  if (strcmp(type_name_ref,"T5_APLTaskRevision")==0)
							{
								ITKCALL(GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type));
								if (tsk_dml_rel_type!=NULLTAG)
								 {

									ITKCALL(GRM_list_primary_objects_only(PartDmlTag,tsk_dml_rel_type,&count1,&DMLRevision));
									printf("\n\n\t\t DML Revision from Task : %d",count1);fflush(stdout);		//task to dml

									for (j=0;j<count1 ;j++ )
									{
										ITKCALL(ITEM_rev_sequence_is_latest(DMLRevision[j],&is_latest));
										printf("\n\n\t\t is_latest : %d\n",is_latest);fflush(stdout);

										if(is_latest != true)
										{
											printf("\n\n\t\t is_latest is not true .....\n");fflush(stdout);
										}else
										{
											ITKCALL(AOM_ask_value_string(PartDmlTag, "item_id",&DmlId));
											printf("\n DmlId %s.....\n",DmlId);//previous revision's task
											DMLRevTag = DMLRevision[j];//DML
											ITKCALL(AOM_ask_value_string(DMLRevTag,"current_name",&current_name));
											printf("\n\n\t\t dml is :%s\n",current_name);fflush(stdout);
											task_len=0;
											task_len=strlen(DmlId);
											task_Plant=subString(DmlId,14,task_len);
											printf("\n task_Plant is : %s\n",task_Plant);fflush(stdout);

											if(tc_strcmp(inp_Plant,task_Plant)==0)
											{
												printf("\n inside plant check is :\n");fflush(stdout);

												ITKCALL(AOM_UIF_ask_value(DMLRevTag,"date_released",&ClosureDate));
												printf("\n ClosureDate : %s\n",ClosureDate);fflush(stdout);//28-Oct-2017 14:12

												ClosureDateDup = subString(ClosureDate,0,11);
												printf("\n ClosureDateDup : %s\n",ClosureDateDup);fflush(stdout);
												getCurrentDateTime(cCurrentAccessDate);
												printf("\n  todays date111 is: %s\n",cCurrentAccessDate);
												pAccessDateDup = subString(cCurrentAccessDate,0,11);
												printf("\n  todays date is: %s\n",pAccessDateDup);

												if (tc_strcmp(IsPPPPMRlzErr1,"NULL")==0) MEM_free(IsPPPPMRlzErr1);
												IsPPPPMRlzErr1=(char *)MEM_alloc(100);
												tc_strcpy(IsPPPPMRlzErr1,"");
												if(tc_strcmp(ClosureDateDup,pAccessDateDup)==0)
												{

													if(cnt==0)
													{
														cnt = 1;

														tc_strcat(IsPPPPMRlzErr1,"\n Two revision of part ");
														tc_strcat(IsPPPPMRlzErr1,Part_no);
														tc_strcat(IsPPPPMRlzErr1," cannot be released on same day.\n");
														printf("\n IsPPPPMRlzErr: %s\n",IsPPPPMRlzErr1);fflush(stdout);

													}
													else
													{
														tc_strcat(IsPPPPMRlzErr1,"\n Two revision of part ");
														tc_strcat(IsPPPPMRlzErr1,Part_no);
														tc_strcat(IsPPPPMRlzErr1," cannot be released on same day..\n");
														cnt=cnt+1;
														printf("\n IsPPPPMRlzErr: %s\n",IsPPPPMRlzErr1);fflush(stdout);
													}
	                                                printf("\n cnt: %d\n",cnt);fflush(stdout);

													if(cnt==1)
													{
														tc_strcat(SetChildRelErr,IsPPPPMRlzErr1);
														cntFlag = 1;
													}
													else
													{
														tc_strcat(SetChildRelErr,IsPPPPMRlzErr1);
														cntFlag = 1;
													}
												  }
											}

										}

								}
							  }

						  }


			  }
			}
			}
			}// End for two rev. on same day check

	return 0;
}

int BOM_Parts_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS,char* UserAgency)
{
			EPM_decision_t decision;
			int			status			=	0;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        char * ItemName1 ;
			char * ItemRev ;
			char * ItemPType ;
			int  	n_parents=0;
			int * 	levels;
			tag_t * 	parents	 =NULLTAG;
			int				task_len=0;
			int				inp_len=0;
			char			*inp_Plant			= NULL;
			char			*task_Plant			= NULL;
	        tag_t item = NULLTAG;
			tag_t  	objTypeTag_eff;
			char  	rev_id[ITEM_id_size_c+1];
		    tag_t *  rev_list;
		    tag_t *DMLRevision = NULLTAG;
			char  type_itemRev[TCTYPE_name_size_c+1];
			char  ChildRevtype_name[TCTYPE_name_size_c+1];
			tag_t	view_type							= NULLTAG;
			tag_t	item_rev_tag							= NULLTAG;
			tag_t	itemTypeTag_cdss							= NULLTAG;
			tag_t	*itemclassp	= NULLTAG;
			tag_t	value_child_details							= NULLTAG;
			char  req_id[ITEM_id_size_c+1];
			char*	 current_name		= NULL;
			char*	 creationdate		= NULL;
			char*	 ClosureDate		= NULL;
			char*	 ClosureDateDup		= NULL;
			char   type_eff[TCTYPE_name_size_c+1];
			int	   ii=0;
			int	   jj=0;
		    char  	rev_id1[ITEM_id_size_c+1];
			tag_t	itemclass							= NULLTAG;
			//tag_t	itemclassp							= NULLTAG;
			tag_t	itemclass_task						= NULLTAG;
			tag_t	objTypeRefTag						= NULLTAG;
			char            *desid				= NULL;
			char			*dml_type			= NULL;
			char			type_name_ref[TCTYPE_name_size_c+1];
			tag_t	item_rev_task						= NULLTAG;
			tag_t	reln_type_tag						= NULLTAG;
			tag_t tsk_dml_rel_type;
        	tag_t	relation_type = NULLTAG;
	       tag_t*			PartDmlTags			= NULLTAG;
           tag_t			PartDmlTag				= NULLTAG;
           logical is_latest;
       	  char*	DmlId;
          tag_t DMLRevTag = NULLTAG;
		  char pAccessDate[20];
		  char cCurrentAccessDate[20]={0};
       	  char * pAccessDateDup 	= NULL;
		  char *  IsPPPPMRlzErr1 = NULL;
		  char *  SetIsPPPPMRlzErr1 = NULL;
	      int				cntErr=0;
	     char*		FullBomErrString	=	NULL;
         tag_t		DesignTag		=	NULLTAG;
		tag_t		objTypeTag		=	NULLTAG;
		tag_t		CurrentRoleTag	=	NULLTAG;
		tag_t*		list_of_WSO_cntrl_tags=NULLTAG;
    	tag_t		t_GChldItemRev;
        char*		type_name1			=	NULL;
		char*		Part_Type			=	NULL;
		char*		Part_Proj			=	NULL;
		char*		SubSyscd 			=	NULL;
		WSO_search_criteria_t  	criteria_CARDML;
		int control_number_found;
    	int	count=0,iPrt=0,nClhd=0,FlagGrpNA = 0,iCntl=0,cntCntrl=0,stampingdone=0,iGChld=0,cntTask=0,iTask=0;
        char*		PlantName 		=	NULL;
	    char*		prtPlantCS 			=	NULL;
	    char*		prtCoatedInd		=	NULL;
	    char*		prtClrInd			=	NULL;
	    tag_t*		DgnChld				=	NULLTAG;
		tag_t*  	prtTasks			=	NULLTAG;
		tag_t  		prtTask				=	NULLTAG;
		tag_t  		prtTask_Class		=	NULLTAG;
		char*		prtTask_type		=	NULL;
		char*		prtTaskName			=	NULL;
		size_t	prtTaskLen	=	0;
	    char*		AssyNoBOMErr		=	NULL;

		   // Start for E50/E99/E98/F30 BOM Check
		FullBomErrString = (char *)MEM_alloc(2500);
		tc_strcpy(FullBomErrString,"PART_BOM_ERROR_MISMATCH :: ");

				for (k=0;k<PartCnt ;k++ )
				{
					int BypassNACSFlag=0;
					printf("\n%d",k);fflush(stdout);
					DesignTag	=	PartTags[k];

					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,DesignTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }

					if(TCTYPE_ask_object_type(DesignTag,&objTypeTag));
					if(TCTYPE_ask_name2(objTypeTag,&type_name1));
					printf("\n\n\t\t AssyTag type_name1 := %s", type_name1);fflush(stdout);

					ITKCALL(AOM_ask_value_string(DesignTag,"item_id",&Part_no));
					printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);

					ITKCALL(AOM_ask_value_string(DesignTag,"t5_PartType",&Part_Type));
					printf("\n\n\t\t Part_no  is :%s",Part_Type);	fflush(stdout);

					if (tc_strcmp(type_name1,"Design Revision")==0)
					{
						//t5_ProjectCode
						ITKCALL(AOM_ask_value_string(DesignTag,"t5_ProjectCode",&Part_Proj));
						printf("\n\n\t\t Part_Proj  is :%s",Part_Proj);	fflush(stdout);

						//QUERY THE CARDML CONTROL OBJECT BASED ON PROJECT CODE
						//IF FOUND BYPASS THE BOM CHECK FOR THAT PART
						WSOM_clear_search_criteria(&criteria_CARDML);
						tc_strcpy(criteria_CARDML.name,"CARDML");
						tc_strcpy(criteria_CARDML.class_name,"T5_ControlObject");
						status	= WSOM_search(criteria_CARDML, &control_number_found, &list_of_WSO_cntrl_tags);
						printf("\ncontrol_number_found : %d",control_number_found);fflush(stdout);
						if (control_number_found>0)
						{
							cntCntrl	=	0;
							for (iCntl=0;iCntl<control_number_found;iCntl++ )
							{
								//Find the subsysd and plantname
								AOM_ask_value_string( list_of_WSO_cntrl_tags[iCntl], "t5_SubSyscd", &SubSyscd);
								AOM_ask_value_string( list_of_WSO_cntrl_tags[iCntl], "t5_Userinfo1", &PlantName);
								printf("\n\nSubSyscd : %s, Plant Name : %s, UserAgency : %s",SubSyscd,PlantName,UserAgency);fflush(stdout);
								if ((tc_strcmp(SubSyscd,Part_Proj)==0) && (tc_strcmp(PlantName,"X1")==0))
								{
									cntCntrl++;
									break;
								}
							}
						}
						if (cntCntrl>0)
						{
							printf("\n\n cntCntrl : %d, Bypass the CS E50/E99/E98 check for Car Plant.\n",cntCntrl);fflush(stdout);
							continue;
						}
						//t5_PartType
						//if (tc_strcmp(Part_Type,"t5SpKit")!=0) need to check this condition for class. as in TCE check class t5SpKit
						{
							ITKCALL(AOM_ask_value_string(DesignTag,PlantCS,&prtPlantCS));
							printf("\n Parent prtPlantCS:%s ..............",prtPlantCS);fflush(stdout);

							if ((tc_strcmp(prtPlantCS,"E50")==0 || tc_strcmp(prtPlantCS,"E99")==0 || tc_strcmp(prtPlantCS,"E98")==0 || tc_strcmp(prtPlantCS,"F30")==0) && (tc_strcmp(Part_Type,"A")==0 || tc_strcmp(Part_Type,"C")==0))
							{
								printf("\n CS E50, E99 ,E98 or F30 part found...\n");fflush(stdout);
								if (tc_strcmp(prtPlantCS,"F30")==0 && tc_strcmp(UserAgency,"JDL")==0)
								{
									printf("\nCS F30 and Location JDL, Bypass check...!!!");fflush(stdout);
									continue;
								}
							//}

								ITKCALL(AOM_ask_value_string(DesignTag,"t5_Coated",&prtCoatedInd));
								printf("\n Parent prtCoatedInd:%s ..............",prtCoatedInd);fflush(stdout);

								ITKCALL(AOM_ask_value_string(DesignTag,"t5_ColourInd",&prtClrInd));
								printf("\n Parent prtClrInd:%s ..............",prtClrInd);fflush(stdout);

								if (tc_strcmp(prtCoatedInd,"C")!=0 && tc_strcmp(prtClrInd,"C")!=0)
								{
									nClhd	=	0;
									ITKCALL(BOM_create_window (&window));
									ITKCALL(CFM_find( "Latest Working", &rule));
									ITKCALL(BOM_set_window_config_rule( window, rule ));
									ITKCALL(BOM_set_window_top_line(window, null_tag,DesignTag , null_tag, &top_line));
									ITKCALL(BOM_line_ask_child_lines (top_line, &nClhd, &DgnChld));
									printf("\nNo of Child Found : %d",nClhd);fflush(stdout);
									if (nClhd<=0)
									{
										//If the part is componet, then
										//Query the supersed part based on orgnazaion ID(APL%) and explode the BOM
										//If BOM found, the set the counter = 1
										//Find the DML, if APL STR DML set counter = 1
										ITKCALL(GRM_find_relation_type("CMHasSolutionItem",&relation_type));
										ITKCALL( GRM_list_primary_objects_only(DesignTag,relation_type,&cntTask,&prtTasks));
										printf("\nNo Of Task Found : %d",cntTask);fflush(stdout);
										if (cntTask>0)
										{
											for (iTask=0;iTask<cntTask;iTask++)
											{
												prtTask	=	prtTasks[iTask];
												ITKCALL (TCTYPE_ask_object_type(prtTask,&prtTask_Class));
												ITKCALL (TCTYPE_ask_name2(prtTask_Class,&prtTask_type));

												printf("\nprtTask_type : %s",prtTask_type);fflush(stdout);

												//if(tc_strcmp(type_class,"A9_APLTaskRevision")==0)
												if(tc_strcmp(prtTask_type,"T5_APLTaskRevision")==0)
												{
													prtTaskName	=	NULL;
													ITKCALL( AOM_ask_value_string(prtTask,"item_id",&prtTaskName));
													printf("\nprtTaskName : %s",prtTaskName);fflush(stdout);
													if (tc_strstr(prtTaskName,"AM")!=NULL)
													{
														printf("\nAM TASK FOUND...!!!");fflush(stdout);
														//NOW CHECK FOR DML

													}
													prtTaskLen	=	tc_strlen(prtTaskName);
													if (prtTaskLen>0)
													{
														MEM_free(prtTaskName);
													}
												}
											}
											if (cntTask>0)
											{
												MEM_free(prtTasks);
											}
										}

										//Free outut tags
										//Find the raw part, attached with part
										//check the plant, if plant match set the RPFoundFlg=1
										//if above all condition failed, give error msg

										AssyNoBOMErr=(char *)MEM_alloc(500);
										//tc_strcpy(AssyNoBOMErr,"");
										sprintf(AssyNoBOMErr,"\nAssy [%s] Has CS As E50/E99/E98/F30, But It Neither Has Any BOM Under It Nor Has Raw Parts.\n", Part_no);
										printf("\nAssy [%s] Has CS As E50/E99/E98/F30, But It Neither Has Any BOM Under It Nor Has Raw Parts.\n", Part_no);
										tc_strcat(FullBomErrString,AssyNoBOMErr);
										tc_strcat(SetChildRelErr,FullBomErrString);
										cntFlag	=	1;
									}

								}

							}
						}
					}
				} // End for E50/E99/E98/F30 BOM Check

	return 0;
}

int Drg_Part_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char*	itemid,char* PlantCS)
{
			EPM_decision_t decision;
			int			status			=	0;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        char * ItemName1 ;
			char * ItemRev ;
			char * ItemPType ;
			int  	n_parents=0;
			int * 	levels;
			tag_t * 	parents	 =NULLTAG;
			int				task_len=0;
			int				inp_len=0;
			char			*inp_Plant			= NULL;
			char			*task_Plant			= NULL;
	        tag_t item = NULLTAG;
			tag_t  	objTypeTag_eff;
			char  	rev_id[ITEM_id_size_c+1];
		    tag_t *  rev_list;
		    tag_t *DMLRevision = NULLTAG;
			char  type_itemRev[TCTYPE_name_size_c+1];
			char  ChildRevtype_name[TCTYPE_name_size_c+1];
			tag_t	view_type							= NULLTAG;
			tag_t	item_rev_tag							= NULLTAG;
			tag_t	itemTypeTag_cdss							= NULLTAG;
			tag_t	*itemclassp	= NULLTAG;
			tag_t	value_child_details							= NULLTAG;
			char  req_id[ITEM_id_size_c+1];
			char*	 current_name		= NULL;
			char*	 creationdate		= NULL;
			char*	 ClosureDate		= NULL;
			char*	 ClosureDateDup		= NULL;
			char   type_eff[TCTYPE_name_size_c+1];
			int	   ii=0;
			int	   jj=0;
		    char  	rev_id1[ITEM_id_size_c+1];
			tag_t	itemclass							= NULLTAG;
			//tag_t	itemclassp							= NULLTAG;
			tag_t	itemclass_task						= NULLTAG;
			tag_t	objTypeRefTag						= NULLTAG;
			char            *desid				= NULL;
			char			*dml_type			= NULL;
			char			type_name_ref[TCTYPE_name_size_c+1];
			tag_t	item_rev_task						= NULLTAG;
			tag_t	reln_type_tag						= NULLTAG;
			tag_t tsk_dml_rel_type;
        	tag_t	relation_type = NULLTAG;
	       tag_t*			PartDmlTags			= NULLTAG;
           tag_t			PartDmlTag				= NULLTAG;
           logical is_latest;
       	  char*	DmlId;
          tag_t DMLRevTag = NULLTAG;
		  char pAccessDate[20];
		  char cCurrentAccessDate[20]={0};
       	  char * pAccessDateDup 	= NULL;
		  char *  IsPPPPMRlzErr1 = NULL;
		  char *  SetIsPPPPMRlzErr1 = NULL;
	      int				cntErr=0;
	     char*		FullBomErrString	=	NULL;
         tag_t		DesignTag		=	NULLTAG;
		tag_t		objTypeTag		=	NULLTAG;
		tag_t		CurrentRoleTag	=	NULLTAG;
		tag_t*		list_of_WSO_cntrl_tags=NULLTAG;
    	tag_t		t_GChldItemRev;
        char*		type_name1			=	NULL;
		char*		Part_Type			=	NULL;
		char*		Part_Proj			=	NULL;
		char*		SubSyscd 			=	NULL;
		WSO_search_criteria_t  	criteria_CARDML;
		int control_number_found;
    	int	count=0,iPrt=0,nClhd=0,FlagGrpNA = 0,iCntl=0,cntCntrl=0,stampingdone=0,iGChld=0,cntTask=0,iTask=0;
        char*		PlantName 		=	NULL;
	    char*		prtPlantCS 			=	NULL;
	    char*		prtCoatedInd		=	NULL;
	    char*		prtClrInd			=	NULL;
	    tag_t*		DgnChld				=	NULLTAG;
		tag_t*  	prtTasks			=	NULLTAG;
		tag_t  		prtTask				=	NULLTAG;
		tag_t  		prtTask_Class		=	NULLTAG;
		char*		prtTask_type		=	NULL;
		char*		prtTaskName			=	NULL;
		size_t	prtTaskLen	=	0;
	    char*		AssyNoBOMErr		=	NULL;
	    int Prt_len=0;
	    char*			Part_DrgNum				= NULL;
		char*			Part_CmpCode				= NULL;
		char*			Part_Creator				= NULL;
	    char*			SpareCriteriaDup				= NULL;

		int AMDrwcntErr = 0;
		int PrtcntErr = 0;
		int DrwcntErr = 0;
	    char *  SetAmDrgChk = NULL;
	    tag_t  reln_type = NULLTAG;
		int    n_attchs =0;
		GRM_relation_t *rellist;
		logical isCheckOut=false;
		logical isDrwCheckOut=false;
	    char *  SetPrtCheckOut = NULL;
	    char  type_name[TCTYPE_name_size_c+1];
	    tag_t  datasettype	= NULLTAG;
	    int result=0;
	    char       *parent_name				=NULL;


			if (!SetPrtCheckOut) MEM_free(SetPrtCheckOut);
			SetPrtCheckOut=(char *)MEM_alloc(1000);
			tc_strcpy(SetPrtCheckOut,"");

		   // Start for E50/E99/E98/F30 BOM Check
			for (k=0;k<PartCnt ;k++ )
			{
//				 char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
//				 char **values = (char **) MEM_alloc(1 * sizeof(char *));
				const char *attrs[1];
				const char *values[1];

				int n_tags_found=0;
				int BypassNACSFlag=0;
				tag_t	*itemclassp	= NULLTAG;
				 tag_t item = NULLTAG;
				printf("\n\n\t\t for k =:%d",k);fflush(stdout);
				AssyTag=PartTags[k];

				BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
				printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
				 if(BypassNACSFlag == 1)
				{
					printf("\t  Bypass Validations .\n");
					continue;
				 }
				 else
				{
					printf("\t  Call Validations .\n");
				 }
				ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
				printf("\n\n\t\t drgindapl_Check_Func Part_no  is :%s",Part_no);fflush(stdout);
				Prt_len=0;
				Prt_len=strlen(Part_no);
				printf("\n\n\t\t Prt_len is :%d",Prt_len);fflush(stdout);

				 attrs[0] ="item_id";
				 values[0] = (char *)Part_no;
				ITKCALL(ITEM_find_items_by_key_attributes(1, attrs, values,&n_tags_found, &itemclassp));
				//ITKCALL(ITEM_find_item(Part_no,&itemclassp));
				item = itemclassp[0];
				if(item != NULLTAG )
				ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag_p));


		//	if(nlsStrLen(PartNumberSS)==14 && (nlsStrCmp(OrganizationIDDup,"APLPUNE")==0 || nlsStrCmp(OrganizationIDDup,"APLAHD")==0 || nlsStrCmp(OrganizationIDDup,"APLJSR")==0 || nlsStrCmp(OrganizationIDDup,"APLLKO")==0 || nlsStrCmp(OrganizationIDDup,"APLPNR")==0 || nlsStrCmp(OrganizationIDDup,"APLDWD")==0))
			if( Prt_len==16) // && (nlsStrCmp(OrganizationIDDup,"APLPUNE")==0 || nlsStrCmp(OrganizationIDDup,"APLAHD")==0 || nlsStrCmp(OrganizationIDDup,"APLJSR")==0 || nlsStrCmp(OrganizationIDDup,"APLLKO")==0 || nlsStrCmp(OrganizationIDDup,"APLPNR")==0 || nlsStrCmp(OrganizationIDDup,"APLDWD")==0))
		//	if( Prt_len==6) // && (nlsStrCmp(OrganizationIDDup,"APLPUNE")==0 || nlsStrCmp(OrganizationIDDup,"APLAHD")==0 || nlsStrCmp(OrganizationIDDup,"APLJSR")==0 || nlsStrCmp(OrganizationIDDup,"APLLKO")==0 || nlsStrCmp(OrganizationIDDup,"APLPNR")==0 || nlsStrCmp(OrganizationIDDup,"APLDWD")==0))
			{
				ITKCALL(AOM_ask_value_string(AssyTag,"t5_PartType",&Part_type));
				printf("\n\n\t\t drgindapl_Check_Func Part_type  is :%s",Part_type);fflush(stdout);

				ITKCALL(AOM_ask_value_string(AssyTag,"t5_DrawingInd",&Part_DrgNum));
				printf("\n\n\t\t drgindapl_Check_Func Part_DrgNum  is :%s",Part_DrgNum);fflush(stdout);

				ITKCALL(AOM_ask_value_string(AssyTag,"t5_PrtCatCode",&Part_CmpCode));
				printf("\n\n\t\t drgindapl_Check_Func Part_CmpCode  is :%s",Part_CmpCode);fflush(stdout);

				//ITKCALL(AOM_ask_value_string(AssyTag,"owning_user",&Part_Creator));
				ITKCALL(AOM_UIF_ask_value(AssyTag,"owning_user",&Part_Creator));
				printf("\n\n\t\t drgindapl_Check_Func Part_Creator  is :%s",Part_Creator);fflush(stdout);

				ITKCALL(AOM_UIF_ask_value(AssyTag,"t5_SpareCriteria",&SpareCriteriaDup));
				printf("\n\n\t\t drgindapl_Check_Func SpareCriteriaDup  is :%s",SpareCriteriaDup);fflush(stdout);


				//if( (nlsStrCmp(Part_type,"SA") == 0 && nlsStrCmp(SpareCriteriaDup,"PHN") == 0 ) ||  (nlsStrCmp(PrtCatCodeDup,"VC") == 0 &&  (nlsStrCmp(PrtCreatorDup,"Loader") == 0 ||   nlsStrCmp(PrtCreatorDup,"super user") == 0)  ) || (nlsStrCmp(PrtClassDup,"t5SpKit") == 0 ) )
				if( (strcmp(Part_type,"SA") == 0 && strcmp(SpareCriteriaDup,"PHN") == 0 )  ||  ( strcmp(Part_CmpCode,"VC") == 0 &&  (strstr(Part_Creator,"loader")) ) )
				{
					printf("\n For system generated 14 digit parts like 35/36 CED coated parts Parts ,Drawing should not be com:"); fflush(stdout);
					continue ;
				}

				if(strcmp(Part_DrgNum,"D")!=0 && strstr(itemid,"AM")!=NULL)
				{

					printf("\n Task Number [%s] is : ",itemid);fflush(stdout);
					if(AMDrwcntErr==0)
					{
						AMDrwcntErr=1;
						tc_strcat(SetChildRelErr,"Below Part is 16 digit part and must have Drawing attachment and Drawing Indicator 'D'");
						tc_strcat(SetChildRelErr,"\n");
						tc_strcat(SetChildRelErr,Part_no);
	                    cntFlag = 1;
					}
					else
					{
						tc_strcat(SetChildRelErr,"\n");
						tc_strcat(SetChildRelErr,Part_no);
						AMDrwcntErr=AMDrwcntErr+1;
						printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
						cntFlag = 1;
					}

				}

				if( strcmp(Part_DrgNum,"D")==0)
				{
					//FlagDrg=0;
					printf("\n\n\t\t Idrgindapl_Check_Func iside Drawing Number D");fflush(stdout);
					ITKCALL(GRM_find_relation_type("IMAN_specification",&reln_type));
					if(reln_type != NULLTAG)
					{
							//int cnt = 0;
							int atchmt = 0;
							tag_t  primary,objTypeTag=NULLTAG;

							//tag_t	*attachments				= NULLTAG;
							//tag_t	dataset						= NULLTAG;
							//tag_t	refobject						= NULLTAG;
							//tag_t secObjTypeTag	= NULLTAG;

							//char   refname[AE_reference_size_c + 1];
							//AE_reference_type_t     reftype;
							//char   path_name1[SS_MAXPATHLEN]={""};
							//char type_name2[TCTYPE_name_size_c+1];

						//ITK_CALL(GRM_list_secondary_objects_only(AssyTag,reln_type,&cnt,&attachments));
						//printf("\nTotal no of attches =%d\n",cnt);fflush(stdout);

						ITKCALL(GRM_list_secondary_objects(AssyTag,reln_type,&n_attchs,&rellist));
						printf("\n Total n attches =%d\n",n_attchs);fflush(stdout);

						if(n_attchs>0)
						{
							isCheckOut=false;
							ITKCALL(RES_is_checked_out(AssyTag,&isCheckOut));
							if(isCheckOut)
							{
								printf("\n Part [%s] is  check out....",Part_no);fflush(stdout);
								if(PrtcntErr==0)
								{
									PrtcntErr=1;
									tc_strcat(SetChildRelErr,"Drwing Indicator for Below Part is D which is in Checkout State, Please Checkin Part before Release and then proceed");
									tc_strcat(SetChildRelErr,"\n");
									tc_strcat(SetChildRelErr,Part_no);
	                                cntFlag = 1;
								}
								else
								{
									tc_strcat(SetChildRelErr,"\n");
									tc_strcat(SetChildRelErr,Part_no);
									PrtcntErr=PrtcntErr+1;
									printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
									cntFlag = 1;
								}

							}

							for (atchmt= 0; atchmt < n_attchs; atchmt++)
							{
								printf("\n Inside loop ..............");fflush(stdout);
								//primary=secondary_objects[atchmt];
								primary=rellist[atchmt].secondary;
								//relationstr=rellist[atchmt].the_relation;

								ITKCALL(TCTYPE_ask_object_type(primary,&objTypeTag));
								ITKCALL(TCTYPE_ask_name(objTypeTag,type_name));
								printf("\n Type Name:%s ..............",type_name);fflush(stdout);
								if(strcmp(type_name,"ImanFile")==0)
								continue;

								result = AE_find_datasettype("CMI2Drawing", &datasettype);
								printf("\nType found---->%d",result);fflush(stdout);

								ITKCALL(AOM_ask_value_string(primary,"object_name",&parent_name));
								printf("\n parent_name:%s ..............",parent_name);fflush(stdout);

								if( strcmp(type_name,"CMI2Drawing") ==0 )
								{

									isDrwCheckOut=false;
									ITKCALL(RES_is_checked_out(primary,&isDrwCheckOut));
									if(isDrwCheckOut)
									{
										if(DrwcntErr==0)
										{
											DrwcntErr=1;
											tc_strcat(SetChildRelErr,"\n Drwing attached to below Part is in Checkout State, Please Checkin Drawing before Release and then proceed");
											tc_strcat(SetChildRelErr,"\n");
											tc_strcat(SetChildRelErr,Part_no);
	                                       cntFlag = 1;
										}
										else
										{
											tc_strcat(SetChildRelErr,"\n");
											tc_strcat(SetChildRelErr,Part_no);
											DrwcntErr=DrwcntErr+1;
											printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
											cntFlag = 1;
										}

									}


								}

								if( strcmp(type_name,"ProDrw") ==0 ) // Creo drawing
								{
									isDrwCheckOut=false;
									ITKCALL(RES_is_checked_out(primary,&isDrwCheckOut));
									if(isDrwCheckOut)
									{
										if(DrwcntErr==0)
										{
											DrwcntErr=1;
											tc_strcat(SetChildRelErr,"\n Drwing attached to below Part is in Checkout State, Please Checkin Drawing before Release and then proceed");
											tc_strcat(SetChildRelErr,"\n");
											tc_strcat(SetChildRelErr,Part_no);
											cntFlag = 1;

										}
										else
										{
											tc_strcat(SetChildRelErr,"\n");
											tc_strcat(SetChildRelErr,Part_no);
											DrwcntErr=DrwcntErr+1;
											printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
											cntFlag = 1;
										}

									}
								}


								if( strcmp(type_name,"PDF") ==0 )
								{
									isDrwCheckOut=false;
									ITKCALL(RES_is_checked_out(primary,&isDrwCheckOut));
									if(isDrwCheckOut)
									{
										if(DrwcntErr==0)
										{
											DrwcntErr=1;
											tc_strcat(SetChildRelErr,"\n Drwing attached to below Part is in Checkout State, Please Checkin Drawing before Release and then proceed");
											tc_strcat(SetChildRelErr,"\n");
											tc_strcat(SetChildRelErr,Part_no);
                                        	cntFlag = 1;
										}
										else
										{
											tc_strcat(SetChildRelErr,"\n");
											tc_strcat(SetChildRelErr,Part_no);
											DrwcntErr=DrwcntErr+1;
											printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
										   cntFlag = 1;
										}

									}
								}

							}
						}
						else
						{
							printf("\n No of Attaches are zeroooo");fflush(stdout);

							if(cntErr==0)
							{
								cntErr=1;
								tc_strcat(SetChildRelErr,"\n Drwing Indicator for Below Part is D, Please attached Drawing to it and then Proceed:");
								tc_strcat(SetChildRelErr,"\n");
								tc_strcat(SetChildRelErr,Part_no);
	                           cntFlag = 1;
							}
							else
							{
								tc_strcat(SetChildRelErr,"\n");
								tc_strcat(SetChildRelErr,Part_no);
								cntErr=cntErr+1;
								printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
								cntFlag = 1;
							}

						}
					}

				}
			}
		}

	return 0;
}

int Group_Parts_Stamping(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS,char* UserAgency)
{
			EPM_decision_t decision;
			int			status			=	0;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  ChildRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        char * ItemName1 ;
			char * ItemRev ;
			char * ItemPType ;
			int  	n_parents=0;
			int * 	levels;
			tag_t * 	parents	 =NULLTAG;
			int				task_len=0;
			int				inp_len=0;
			char			*inp_Plant			= NULL;
			char			*task_Plant			= NULL;
	        tag_t item = NULLTAG;
			tag_t  	objTypeTag_eff;
			char  	rev_id[ITEM_id_size_c+1];
		    tag_t *  rev_list;
		    tag_t *DMLRevision = NULLTAG;
			char  type_itemRev[TCTYPE_name_size_c+1];
			char  ChildRevtype_name[TCTYPE_name_size_c+1];
			tag_t	view_type							= NULLTAG;
			tag_t	item_rev_tag							= NULLTAG;
			tag_t	itemTypeTag_cdss							= NULLTAG;
			tag_t	*itemclassp	= NULLTAG;
			tag_t	value_child_details							= NULLTAG;
			char  req_id[ITEM_id_size_c+1];
			char*	 current_name		= NULL;
			char*	 creationdate		= NULL;
			char*	 ClosureDate		= NULL;
			char*	 ClosureDateDup		= NULL;
			char   type_eff[TCTYPE_name_size_c+1];
			int	   ii=0;
			int	   jj=0;
		    char  	rev_id1[ITEM_id_size_c+1];
			tag_t	itemclass							= NULLTAG;
			//tag_t	itemclassp							= NULLTAG;
			tag_t	itemclass_task						= NULLTAG;
			tag_t	objTypeRefTag						= NULLTAG;
			char            *desid				= NULL;
			char			*dml_type			= NULL;
			char			type_name_ref[TCTYPE_name_size_c+1];
			tag_t	item_rev_task						= NULLTAG;
			tag_t	reln_type_tag						= NULLTAG;
			tag_t tsk_dml_rel_type;
        	tag_t	relation_type = NULLTAG;
	       tag_t*			PartDmlTags			= NULLTAG;
           tag_t			PartDmlTag				= NULLTAG;
           logical is_latest;
       	  char*	DmlId;
		 tag_t*		GrpChld			=	NULLTAG;
		 char*		GChldName ;
		 char*		GChldRev ;
		 char*		GChildPuneCS ;
		 WSO_search_criteria_t  	criteria_Gcontrol;

		 tag_t		DesignTag		=	NULLTAG;
		tag_t		objTypeTag		=	NULLTAG;
		tag_t		CurrentRoleTag	=	NULLTAG;
		tag_t*		list_of_WSO_cntrl_tags=NULLTAG;
    	tag_t		t_GChldItemRev;
        char*		type_name1			=	NULL;
		char*		Part_Type			=	NULL;
		char*		Part_Proj			=	NULL;
		char*		SubSyscd 			=	NULL;
		WSO_search_criteria_t  	criteria_CARDML;
		int control_number_found;
    	int	count=0,iPrt=0,nClhd=0,FlagGrpNA = 0,iCntl=0,cntCntrl=0,stampingdone=0,iGChld=0,cntTask=0,iTask=0;
        char*		PlantName 		=	NULL;
	    char*		prtPlantCS 			=	NULL;
	    char*		prtCoatedInd		=	NULL;
	    char*		prtClrInd			=	NULL;
	    tag_t*		DgnChld				=	NULLTAG;
		tag_t*  	prtTasks			=	NULLTAG;
		tag_t  		prtTask				=	NULLTAG;
		tag_t  		prtTask_Class		=	NULLTAG;
		char*		prtTask_type		=	NULL;
		char*		prtTaskName			=	NULL;
		size_t	prtTaskLen	=	0;
        char*		type_name2			=	NULL;
     	int iChildItemTag;

		printf("\n\n\t\t  Inside Group Part CS Stamp ");fflush(stdout);
		// Start for Group Part CS Stamp
				for (k=0;k<PartCnt ;k++ )
				{
					int BypassNACSFlag =0;

					printf("\n%d",k);fflush(stdout);
					DesignTag	=	PartTags[k];
					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,DesignTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }

					if(TCTYPE_ask_object_type(DesignTag,&objTypeTag));
					if(TCTYPE_ask_name2(objTypeTag,&type_name2));
					printf("\n\n\t\t AssyTag type_name2 := %s", type_name2);fflush(stdout);

					ITKCALL(AOM_ask_value_string(DesignTag,"item_id",&Part_no));
					printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);

					ITKCALL(AOM_ask_value_string(DesignTag,"t5_PartType",&Part_Type));
					printf("\n\n\t\t Part_no  is :%s",Part_Type);	fflush(stdout);

					if((tc_strcmp(type_name2,"Design Revision")==0) && (tc_strcmp(Part_Type,"G")==0))
					{

						//Get the child of Group Part
						if(BOM_create_window (&window)!=ITK_ok);
						if(BOM_set_window_pack_all (window, true)!=ITK_ok);//Pack the BOM Line
						if(BOM_set_window_top_line (window, null_tag,DesignTag , null_tag, &top_line)!=ITK_ok);//Find the child of the BOM line

						if(BOM_line_ask_child_lines (top_line, &nClhd, &GrpChld));
						printf("\nNo of Child Found : %d",nClhd);fflush(stdout);
						if (nClhd>0)
						{
							//Check the CS of Child Parts, If all child having CS as 'NA' then stamp Group Part CS as 'NA'
							FlagGrpNA	=	0;
							for (iGChld=0;iGChld<nClhd ;iGChld++)
							{
								ITKCALL(BOM_line_unpack (GrpChld[iGChld]));
								ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
								ITKCALL(BOM_line_ask_attribute_tag(GrpChld[iGChld], iChildItemTag, &t_GChldItemRev));

								ITKCALL(AOM_ask_value_string(t_GChldItemRev,"item_id",&GChldName));
								printf("\n child GChldName:%s ..............",GChldName);fflush(stdout);

								ITKCALL(AOM_ask_value_string(t_GChldItemRev,"item_revision_id",&GChldRev));
								printf("\n child GChldRev:%s ..............",GChldRev);fflush(stdout);

								ITKCALL(AOM_ask_value_string(t_GChldItemRev,PlantCS,&GChildPuneCS));
								printf("\n child GChildPuneCS:%s ..............",GChildPuneCS);fflush(stdout);

								if(tc_strcmp(GChildPuneCS,"NA (Not Applicable For plant)" )==0)
								{
									FlagGrpNA++;
								}
							}
							printf("\n\nFlagGrpNA : %d, chilcnt : %d",FlagGrpNA,nClhd);fflush(stdout);
							if (FlagGrpNA==nClhd)
							{
								//Update the Group Part CS as "NA"
								printf("\n\nAll child having NA CS,so Group CS is NA.\n");fflush(stdout);
								ITKCALL(AOM_lock(DesignTag));//Locks an object against modification by another process.
								ITKCALL(AOM_set_value_string(DesignTag,PlantCS,"NA (Not Applicable For plant)"));//Sets value on a single-valued property.
								ITKCALL(AOM_save(DesignTag));//Saves an application object to the database.
								ITKCALL(AOM_refresh(DesignTag,0));//Reload the object in Database
								ITKCALL(AOM_unlock(DesignTag));
							}
							else
							{
								cntCntrl	=	0;
								//Query the control object, if found stamped Group part CS as "E50"
								printf("\nNot all part having NA CS.");fflush(stdout);
								WSOM_clear_search_criteria(&criteria_Gcontrol);
								strcpy(criteria_Gcontrol.name,"AutoStmGrp");
								strcpy(criteria_Gcontrol.class_name,"T5_ControlObject");
								status	= WSOM_search(criteria_Gcontrol, &control_number_found, &list_of_WSO_cntrl_tags);
								printf("\ncontrol_number_found : %d",control_number_found);fflush(stdout);
								if (control_number_found>0)
								{
									cntCntrl	=	0;
									for (iCntl=0;iCntl<control_number_found;iCntl++ )
									{
										//Find the subsysd and plantname
										AOM_ask_value_string( list_of_WSO_cntrl_tags[iCntl], "t5_SubSyscd", &SubSyscd);
										AOM_ask_value_string( list_of_WSO_cntrl_tags[iCntl], "t5_Userinfo1", &PlantName);
										printf("\n\nSubSyscd : %s, Plant Name : %s, UserAgency : %s",SubSyscd,PlantName,UserAgency);fflush(stdout);
										if ((tc_strcmp(SubSyscd,"Live")==0) && (tc_strcmp(PlantName,UserAgency)==0))
										{
											cntCntrl++;
											MEM_free(list_of_WSO_cntrl_tags);
											break;
										}
									}
								}
								printf("\n\ncntCntrl : %d",cntCntrl);fflush(stdout);
								if (cntCntrl>0)
								{
									printf("\ncontrol object found,So update the CS as E50.");fflush(stdout);
									//Update the Group Part CS as "E50"
									ITKCALL(AOM_lock(DesignTag));//Locks an object against modification by another process.
									ITKCALL(AOM_set_value_string(DesignTag,PlantCS,"E50"));//Sets value on a single-valued property.
									ITKCALL(AOM_save(DesignTag));//Saves an application object to the database.
									ITKCALL(AOM_refresh(DesignTag,0));//Reload the object in Database
									ITKCALL(AOM_unlock(DesignTag));
								}
								else
								{
									//Update the CS base on Child Part and User location
									printf("\nNo control object found.");fflush(stdout);
									stampingdone=0;
									for (iGChld=0;iGChld<nClhd ;iGChld++)
									{
										ITKCALL(BOM_line_unpack (GrpChld[iGChld]));
										ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag));
										ITKCALL(BOM_line_ask_attribute_tag(GrpChld[iGChld], iChildItemTag, &t_GChldItemRev));
										ITKCALL(AOM_ask_value_string(t_GChldItemRev,"item_id",&GChldName));
										printf("\n child GChldName:%s ..............",GChldName);fflush(stdout);

										ITKCALL(AOM_ask_value_string(t_GChldItemRev,"item_revision_id",&GChldRev));
										printf("\n child GChldRev:%s ..............",GChldRev);fflush(stdout);

										ITKCALL(AOM_ask_value_string(t_GChldItemRev,PlantCS,&GChildPuneCS));
										printf("\n child GChildPuneCS:%s ..............",GChildPuneCS);fflush(stdout);

										//Check CS as E50 and Location as JSR/JDL/PNR
										if ((tc_strcmp(GChildPuneCS,"E50")==0) && ((tc_strcmp(UserAgency,"JSR")==0) || (tc_strcmp(UserAgency,"JDL")==0)|| (tc_strcmp(UserAgency,"PNR")==0)))
										{
											printf("\nInside %s(JSR/JDL/PNR) CS update as E50 for Grp part\n",UserAgency);fflush(stdout);
											//Update the Group Part CS as "E50"
											ITKCALL(AOM_lock(DesignTag));//Locks an object against modification by another process.
											ITKCALL(AOM_set_value_string(DesignTag,PlantCS,"E50"));//Sets value on a single-valued property.
											ITKCALL(AOM_save(DesignTag));//Saves an application object to the database.
											ITKCALL(AOM_refresh(DesignTag,0));//Reload the object in Database
											ITKCALL(AOM_unlock(DesignTag));
											break;
										}
										else if (((tc_strcmp(GChildPuneCS,"E50")==0) || (tc_strcmp(GChildPuneCS,"E99")==0) ) && ((tc_strcmp(UserAgency,"JSR")!=0) || (tc_strcmp(UserAgency,"JDL")!=0)|| (tc_strcmp(UserAgency,"PNR")!=0)))
										{
											//Update the Group Part CS as "E50"
											printf("\nInside %s(not JSR/JDL/PNR) CS update as E50 for Grp part\n",UserAgency);fflush(stdout);
											ITKCALL(AOM_lock(DesignTag));//Locks an object against modification by another process.
											ITKCALL(AOM_set_value_string(DesignTag,PlantCS,"E50"));//Sets value on a single-valued property.
											ITKCALL(AOM_save(DesignTag));//Saves an application object to the database.
											ITKCALL(AOM_refresh(DesignTag,0));//Reload the object in Database
											ITKCALL(AOM_unlock(DesignTag));
											break;
										}
										else
										{
											stampingdone++;
											printf("\nGroup Id Will Update as E [%d]\n",  stampingdone);fflush(stdout);
											if (stampingdone==1)
											{
												ITKCALL(AOM_lock(DesignTag));//Locks an object against modification by another process.
												ITKCALL(AOM_set_value_string(DesignTag,PlantCS,"E"));//Sets value on a single-valued property.
												ITKCALL(AOM_save(DesignTag));//Saves an application object to the database.
												ITKCALL(AOM_refresh(DesignTag,0));//Reload the object in Database
												ITKCALL(AOM_unlock(DesignTag));
											}

										}

									}
								}
								//Free list_of_WSO_cntrl_tags

							}

						}
						else
						{
						 tc_strcat(SetChildRelErr,"\n No child found under Group Part:");
						tc_strcat(SetChildRelErr,Part_no);
					    cntFlag = 1;
						}

					}
				}// End for Group Part CS Stamp

	return 0;
}

int CS_IA_Mismatch_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS,char* PlantIA,char* UserAgency)
{
			EPM_decision_t decision;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  CSIARelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        char * ItemName1 ;
			char * ItemRev ;
			char * ItemPType ;
			int  	n_parents=0;
			int * 	levels;
			char*			Part_CS_Input		= NULL;
			char*			PuneInAg			= NULL;
			char*			PuneInAg_Input		= NULL;
		   char * PuneStoreLoc ;
		   char * revision ;
		   char * Object_ColorInd ;
		   char * sDrgInd ;
		   char * Object_Coated ;
	       char  type_name[TCTYPE_name_size_c+1];

				if (tc_strcmp(CSIARelErr,"NULL") !=0) MEM_free(CSIARelErr);
				CSIARelErr=(char *)MEM_alloc(500);
				tc_strcpy(CSIARelErr," ");
				printf("\n\n\t\t  Inside CS_IA Mismatch Part check ");fflush(stdout);
			// Start for CS_IA Mismatch Part check
				for (k=0;k<PartCnt ;k++ )
				{
//					char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
//					char **values = (char **) MEM_alloc(1 * sizeof(char *));
					const char *attrs[1];
		      		const char *values[1];

					int n_tags_found=0;
					int BypassNACSFlag=0;

					tag_t	*itemclassp	= NULLTAG;
					 tag_t item = NULLTAG;
					printf("\n\n\t\t for k =:%d",k);fflush(stdout);
					AssyTag=PartTags[k];
					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }

					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
					printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);
					 attrs[0] ="item_id";
					 values[0] = (char *)Part_no;
					ITEM_find_items_by_key_attributes(1, attrs, values,&n_tags_found, &itemclassp);
					//ITKCALL(ITEM_find_item(Part_no,&itemclassp));
					item = itemclassp[0];
					if(item != NULLTAG )
					ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag_p));

					ITKCALL(AOM_ask_value_string(AssyTag,"t5_PartType",&Part_type));
					printf("\n\n\t\t Part_type  is :%s",Part_type);	fflush(stdout);

					ITKCALL(AOM_ask_value_string(AssyTag,PlantCS,&Part_CS));
					printf("\n\n\t\t Part_CS  is :%s",Part_CS);	fflush(stdout);

					ITKCALL(AOM_ask_value_string(AssyTag,PlantIA,&PuneInAg));
					printf("\n\n\t\t PuneInAg  is :%s",PuneInAg);	fflush(stdout);

					if(AOM_ask_value_string(AssyTag,"t5_Coated",&Object_Coated));
					printf("\n\n\t\t Object_Coated is :%s\n",Object_Coated);fflush(stdout);

					if(AOM_ask_value_string(AssyTag,"t5_ColourInd",&Object_ColorInd));
					printf("\n\n\t\t Object_ColorInd is :%s\n",Object_ColorInd);fflush(stdout);

					if(AOM_ask_value_string(AssyTag,"t5_DrawingInd",&sDrgInd));//Drwing Ind
					printf("\n\n\t\t sDrgInd is :%s\n",sDrgInd);fflush(stdout);

					if( AOM_ask_value_string(AssyTag,"item_revision_id",&revision)!=ITK_ok);//Need to Check
					printf("\n saveslinmodel_method::Design Revision revision :: [%s]\n", revision);fflush(stdout);


					if(strcmp(Part_type,"D")==0 || strcmp(Part_type,"CKDVC")==0 || strcmp(Part_type,"G")==0 )
				   {
						printf("\n Inside Part_type Dummy,Group and CKDVC\n");fflush(stdout);
						continue;
				   }

					 if(((strcmp(sDrgInd,"N" )==0) || (strcmp(sDrgInd,"A" )==0)) && (strcmp(Part_type,"D" )==0))
				   {
						printf("\n Inside Other than Drawing \n");fflush(stdout);
						continue;
				   }

				   if(strcmp(Part_CS,"NA")==0)
				   {
						printf("\n Bypass for NA CS\n");fflush(stdout);
						continue;
				   }

					if((strcmp(Part_CS,"F")==0) || (strcmp(Part_CS,"F18")==0)|| (strcmp(Part_CS,"F30")==0)|| (strcmp(Part_CS,"F19")==0))
					{
						printf("\n F/F30/F18/F19");fflush(stdout);

						if((strcmp(PuneInAg,"A")==0) || (strcmp(PuneInAg,"B")==0)|| (strcmp(PuneInAg,"C")==0)|| (strcmp(PuneInAg,"E")==0)
						|| (strcmp(PuneInAg,"E")==0)|| (strcmp(PuneInAg,"F")==0)|| (strcmp(PuneInAg,"G")==0)|| (strcmp(PuneInAg,"J")==0)
						|| (strcmp(PuneInAg,"K")==0)|| (strcmp(PuneInAg,"L")==0)|| (strcmp(PuneInAg,"S")==0)|| (strcmp(PuneInAg,"")==0)
						|| (strcmp(PuneInAg,"CT")==0)|| (strcmp(PuneInAg,"X")==0)|| (strcmp(PuneInAg,"J1")==0)
						|| (strcmp(PuneInAg,"J2")==0)|| (strcmp(PuneInAg,"E")==0)|| (strcmp(PuneInAg,"M")==0))
						{
						}
						else
						{
						printf("\n Other F/F30/F18/F19");fflush(stdout);

						  if(cnt==0)
						{
							  	cnt = 1;
								tc_strcat(CSIARelErr,"\n Part ");
								tc_strcat(CSIARelErr,Part_no);
								tc_strcat(CSIARelErr,"Error in CS and Initial-Agency comparision for assembly");

						}else
							{
								printf("\n Other_1 F/F30/F18/F19");fflush(stdout);

								tc_strcat(CSIARelErr,"\n Part ");
								tc_strcat(CSIARelErr,Part_no);
								tc_strcat(CSIARelErr,"Error in CS and Initial-Agency comparision for assembly");
                               cnt = cnt+1;
						   }
						   if(cnt==1)
							{
								tc_strcpy(SetChildRelErr,CSIARelErr);
								cntFlag = 1;
							}
							else
							{
								tc_strcat(SetChildRelErr,CSIARelErr);
								cntFlag = 1;
							}
						}

					}
					else
					{
						if((strcmp(PuneInAg,"D")==0) || (strcmp(PuneInAg,"N")==0)|| (strcmp(PuneInAg,"R")==0)|| (strcmp(PuneInAg,"W")==0)
						|| (strcmp(PuneInAg,"Y")==0)|| (strcmp(PuneInAg,"H")==0)|| (strcmp(PuneInAg,"P")==0)|| (strcmp(PuneInAg,"")==0)
						||  (strcmp(PuneInAg,"L")==0)|| (strcmp(PuneInAg,"X")==0)|| (strcmp(PuneInAg,"J1")==0)
						|| (strcmp(PuneInAg,"J2")==0)|| (strcmp(PuneInAg,"M")==0))
						{
						}
						else
						{
						  if(cnt==0)
						{
							  	cnt = 1;
								tc_strcat(CSIARelErr,"\n Part ");
								tc_strcat(CSIARelErr,Part_no);
								tc_strcat(CSIARelErr,"Error in CS and Initial-Agency comparision for assembly");

						}else
							{
							     tc_strcat(CSIARelErr,"\n Part ");
								tc_strcat(CSIARelErr,Part_no);
								tc_strcat(CSIARelErr,"Error in CS and Initial-Agency comparision for assembly");
                               cnt = cnt+1;
						   }
						   if(cnt==1)
							{
								tc_strcpy(SetChildRelErr,CSIARelErr);
								cntFlag = 1;
							}
							else
							{
								tc_strcat(SetChildRelErr,CSIARelErr);
								cntFlag = 1;
							}
						}

					}

				}// End for CS_IA Mismatch Part check

	return 0;
}

int CS_StoreLoc_Mismatch_Checks(int PartCnt, tag_t* PartTags,char* SetChildRelErr,char* PlantCS,char* PlantStore,char* UserAgency)
{
			EPM_decision_t decision;
			tag_t			AssyTag				= NULLTAG;
			char*			Part_no				= NULL;
			 tag_t	item_rev_tag_p						= NULLTAG;
			char*		Part_type			=	NULL;
			char*			Part_CS				= NULL;
	        int n=0,j=0;
			tag_t	window								= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	Childtop_line							= NULLTAG;
			tag_t  *children;
			tag_t  *totalchildren;
	        int   attribute_act_tak;
	        tag_t  	value_act_Tak;
	        int k = 0,Item_ID=0,childCnt=0;
	        char *Item_id = NULL;
         	tag_t	itemcldclass							= NULLTAG;
			char *Item_rev = NULL;
			char *Item_LCS = NULL;
			char *Item_Lcs_str = NULL;
         	tag_t  	item_rev_cld_tag;
	        int       st_count=0;
	        tag_t*    status_list;
	        char *  CSSLRelErr = NULL;
			int       iLCS=0;
			int       cntLcs=0;
	        char* class_name=NULL;
	        char * ItemName1 ;
			char * ItemRev ;
			char * ItemPType ;
			int  	n_parents=0;
			int * 	levels;
			char*			Part_CS_Input		= NULL;
			char*			PuneInAg			= NULL;
			char*			PuneInAg_Input		= NULL;
		   char * PuneStoreLoc ;
		   char * revision ;
		   char * Object_ColorInd ;
		   char * sDrgInd ;
		   char * Object_Coated ;
	       char  type_name[TCTYPE_name_size_c+1];

				if (tc_strcmp(CSSLRelErr,"NULL") !=0) MEM_free(CSSLRelErr);
				CSSLRelErr=(char *)MEM_alloc(500);
				tc_strcpy(CSSLRelErr," ");
				printf("\n\n\t\t  Inside  CS_Store Mismatch Part check ");fflush(stdout);
			// Start for CS_Store Mismatch Part check
				for (k=0;k<PartCnt ;k++ )
				{
//					char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
//					char **values = (char **) MEM_alloc(1 * sizeof(char *));

					const char *attrs[1];
				    const char *values[1];

					int n_tags_found=0;
					int BypassNACSFlag=0;
					tag_t	*itemclassp	= NULLTAG;
					 tag_t item = NULLTAG;
					printf("\n\n\t\t for k =:%d",k);fflush(stdout);
					AssyTag=PartTags[k];
					BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
					printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);
					 if(BypassNACSFlag == 1)
					{
						printf("\t  Bypass Validations .\n");
						continue;
					 }
					 else
					{
						printf("\t  Call Validations .\n");
					 }

					ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&Part_no));
					printf("\n\n\t\t Part_no  is :%s",Part_no);	fflush(stdout);
					 attrs[0] ="item_id";
					 values[0] = (char *)Part_no;
					ITEM_find_items_by_key_attributes(1, attrs, values,&n_tags_found, &itemclassp);
					//ITKCALL(ITEM_find_item(Part_no,&itemclassp));
					item = itemclassp[0];
					if(item != NULLTAG )
					ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag_p));

					ITKCALL(AOM_ask_value_string(AssyTag,"t5_PartType",&Part_type));
					printf("\n\n\t\t Part_type  is :%s",Part_type);	fflush(stdout);

					ITKCALL(AOM_ask_value_string(AssyTag,PlantCS,&Part_CS));
					printf("\n\n\t\t Part_CS  is :%s",Part_CS);	fflush(stdout);

					if( AOM_ask_value_string(AssyTag,PlantStore,&PuneStoreLoc)!=ITK_ok);
					printf("\n\n\t\t Part_PuneStoreLoc :: [%s]", PuneStoreLoc);fflush(stdout);

					printf("\n strlen(PuneStoreLoc)=%d",strlen(PuneStoreLoc));fflush(stdout);

					if(AOM_ask_value_string(AssyTag,"t5_Coated",&Object_Coated));
					printf("\n\n\t\t Object_Coated is :%s",Object_Coated);fflush(stdout);

					if(AOM_ask_value_string(AssyTag,"t5_ColourInd",&Object_ColorInd));
					printf("\n\n\t\t Object_ColorInd is :%s",Object_ColorInd);fflush(stdout);

					if( AOM_ask_value_string(AssyTag,"item_revision_id",&revision)!=ITK_ok);//Need to Check
					printf("\n\n\t\t saveslinmodel_method::Design Revision revision :: [%s]", revision);fflush(stdout);



					if(strcmp(Object_Coated,"C")==0 && strcmp(Object_ColorInd,"C")==0)
				   {
						printf("\n Inside Object_ColorInd and Object_ColorIndi\n");fflush(stdout);
				   }
				   else
				   {
					   if(strcmp(Part_type,"D")!=0)
					   {
						   printf("\n *******Part_type is not D***\n");fflush(stdout);
						if((strcmp(Part_CS,"F")==0) || (strcmp(Part_CS,"F30")==0))
						{
							printf("\n F/F30");fflush(stdout);


						//	if((strcmp(PuneStoreLoc,"")!=0) || (strcmp(PuneStoreLoc,NULL)!=0))
							if(strlen(PuneStoreLoc)>0)
							{
								printf("\n NOT NULL SLOC");fflush(stdout);
								if((strcmp(PuneStoreLoc,"NA")==0))
								{
									printf("\n PuneStoreLoc is NA",PuneStoreLoc);fflush(stdout);
										  if(cnt==0)
										{
												cnt = 1;
												tc_strcat(CSSLRelErr,"\n Part ");
												tc_strcat(CSSLRelErr,Part_no);
												tc_strcat(CSSLRelErr,"CS (F/F30) is Having NULL/NA Store location for Assembly");

										}else
											{
												 tc_strcat(CSSLRelErr,"\n Part ");
												tc_strcat(CSSLRelErr,Part_no);
												tc_strcat(CSSLRelErr,"CS (F/F30) is Having NULL/NA Store location for Assembly");
											   cnt = cnt+1;
										   }
										   if(cnt==1)
											{
												tc_strcpy(SetChildRelErr,CSSLRelErr);
												cntFlag = 1;
											}
											else
											{
												tc_strcat(SetChildRelErr,CSSLRelErr);
												cntFlag = 1;
											}
								}
							}
							else
							{

								printf("\n Revision");fflush(stdout);
								//if((strcmp(revision,"NR")==0))
								//{
									  if(cnt==0)
									{
											cnt = 1;
											tc_strcat(CSSLRelErr,"\n Part ");
											tc_strcat(CSSLRelErr,Part_no);
											tc_strcat(CSSLRelErr,"CS (F/F30) is Having NULL/NA Store location for Assembly");

									}else
										{
											 tc_strcat(CSSLRelErr,"\n Part ");
											tc_strcat(CSSLRelErr,Part_no);
											tc_strcat(CSSLRelErr,"CS (F/F30) is Having NULL/NA Store location for Assembly");
										   cnt = cnt+1;
									   }
									   if(cnt==1)
										{
											tc_strcpy(SetChildRelErr,CSSLRelErr);
											cntFlag = 1;
										}
										else
										{
											tc_strcat(SetChildRelErr,CSSLRelErr);
											cntFlag = 1;
										}
								//}
							}
							printf("\n F....");fflush(stdout);

						}
						else
						   {
							printf("\n *******CS is not F/F30***\n");fflush(stdout);
						}
					  }
				  }

				}// End for CS_Store Mismatch Part check

	return 0;
}

int Driver_VC_Check(tag_t DMLLcmTag ,char* SetChildRelErr,char* PlantCS)
{
			EPM_decision_t decision;
			int count= 0;
			tag_t *attachments = NULLTAG;
			char* t5current_name		= NULL;
			char *s;
			tag_t class_id=NULLTAG;
			char *class_name;
			tag_t tsk_dml_rel_type;
			tag_t *DMLRevision = NULLTAG;
			logical is_latest;
			tag_t DMLRevTag = NULLTAG;
			int n = 0;
			int n_prt = 0;
			int j = 0;
			int ds = 0;
			int k = 0;
			int j_dVC = 0;
			char *EcnType = NULL;
			char *DML_no = NULL;
			char *DriverVC = NULL;
			int n_tags_found1=0;
			int n_tags_found_childprt=0;
			int partfoundinDRVC=0;
			int childpartfoundinDRVC=0;
			int partCntFnd=0;
			int childpartCntFnd=0;
			int reqLevel=99;
			int level=0;
			tag_t	*itemclass1	= NULLTAG;
			tag_t	itemclassp	= NULLTAG;
			tag_t tag_query;
			char *entries[2] = {"Item ID","Release Status"};
			char **values =	(char **) MEM_alloc(10 * sizeof(char *));
  	       int n_entries=1;
			//char searchEntry[1][16]={"ItemID","ReleaseStatus"};
			//char searchEntry[2][16]={"ReleaseStatus"};




			tag_t	erc_view_type	= NULLTAG;
			tag_t	erc_bom_view	= NULLTAG;
			tag_t	itemclassp_childprt	= NULLTAG;
			tag_t	finalitemclassp_childprt	= NULLTAG;
			tag_t	window								= NULLTAG;
			tag_t	top_line							= NULLTAG;
			tag_t	top_line_childprt							= NULLTAG;
			tag_t	rule								= NULLTAG;
			tag_t  *children = NULLTAG;
			tag_t  *prt_children = NULLTAG;
			int   attribute_act_tak;
			int   attribute_act_tak_childprt;
			tag_t  	value_act_Tak;
			char *Item_id = NULL;
			char *Item_id_DV = NULL;
			char		*CurrentTask					= NULL;
			char       *parent_name				=NULL;
			tag_t	item_rev_tag							= NULLTAG;
			int				PartCnt				= 0;
			tag_t*			PartTags			= NULLTAG;
			tag_t			AssyTag				= NULLTAG;
			char   *taskAttPrtNo=NULL;
			char   *taskowning_groupVal=NULL;
			char   *owning_group_val=NULL;
			char   *t5Part_type=NULL;
			char   *t5Part_revision=NULL;
			char   *ItemName_ProdtPlan=NULL;
			char   *t5Part_no=NULL;
			char   *t5design_grp=NULL;
			char *  DriverVCErr = NULL;
			char *  DriverVCChildErr = NULL;
			char *DriverVCLCS=NULL ;

			const char *qry_entries[1];
			const char *qry_values[1];
			int n_tags_found=0;
			tag_t	*itemclass	= NULLTAG;
			tag_t	*itemclass_childprt	= NULLTAG;
			tag_t item = NULLTAG;
			char*	itemid;
			const char *qry_entries_vc[1];
			char erc_view_type_name[TCTYPE_name_size_c+1]="view";
			const char *qry_values_vc[1];
			char			qry_values_vc_rev_id[TCTYPE_name_size_c+1]="NR";
			char			qry_values_childprt_rev_id[TCTYPE_name_size_c+1];
			const char *qry_entries_childprt[1];
			const char *qry_values_childprt[1];
			int iChildItemTag=0;
			tag_t   t_ChildItemRev;
			tag_t   t_ChildItemRev_DVC;
			char * ItemName ;
			char partFoundStatus[40];
			//char partFoundStatus1[10];
			char partFoundStatus1[40];
			tag_t  CurrentRoleTag = NULLTAG;
			char roleName[SA_name_size_c+1];
			char *PlantName=NULL;
			char ContexPlantVal[40];
			int StructChldCnt=0;
			int StructCntProdPlan=0;
			tag_t	objChild			= NULLTAG;
			int iChildItemTag_DVC=0;
			int	childpart_revcount1	= 0;
			int	pii	= 0;
			tag_t objChild_ProdtPlan		= NULLTAG;
			int	 iChildItemTag_ProdtPlan	= 0;
			tag_t t_ChildItemRev_ProdtPlan	= NULLTAG;
		    tag_t *  rev_partlist;
			char  	part_rev_id1[ITEM_id_size_c+1];
			tag_t			view_type_tag_prdPln							= NULLTAG;
			char			*bs_view_type_prdplnname							= NULL;
			int num_found=0;


			//entries = (char **) MEM_alloc(n_entries * sizeof(char *));
			//entries[2]={"ItemID","ReleaseStatus"};
			//values = (char **) MEM_alloc(n_entries * sizeof(char *));

            // Start  for Driver VC Part check
			ITKCALL(GRM_find_relation_type("T5_DMLTaskRelation", &tsk_dml_rel_type));
			if (tsk_dml_rel_type!=NULLTAG)
			{
				ITKCALL(GRM_list_primary_objects_only(DMLLcmTag,tsk_dml_rel_type,&count,&DMLRevision));
				printf("\n\n\t\t DML Revision from Task : %d",count);fflush(stdout);

				for (j=0;j<count ;j++ )
				{
					ITKCALL(ITEM_rev_sequence_is_latest(DMLRevision[j],&is_latest));
					printf("\n\n\t\t is_latest : %d\n",is_latest);fflush(stdout);

					if(is_latest != true)
					{
						printf("\n\n\t\t is_latest is not true .....\n");fflush(stdout);
					}else
					{
						DMLRevTag = DMLRevision[j];

						ITKCALL(AOM_ask_value_string(DMLRevTag,"t5_EcnType",&EcnType));
						printf("\n\n\t\t EcnType is :%s",EcnType);fflush(stdout);

						ITKCALL(AOM_ask_value_string(DMLRevTag,"current_id",&DML_no));
						printf("\n\n\t\t DML_no is :%s",DML_no);fflush(stdout);

						if(tc_strstr(DML_no,"AM")!=NULL)
						{
							printf("\n Inside AM DML..\n");
							//if((tc_strcmp(EcnType,"APLSTR")==0) || (tc_strcmp(EcnType,"APLMBOMRES")==0) )
							if((tc_strcmp(EcnType,"APLSTR")==0) )
							{
								ITKCALL(AOM_ask_value_string(DMLRevTag,"t5_DriverVC",&DriverVC));
								printf("\n\n\t\t DriverVC is :%s",DriverVC);fflush(stdout);

								ITKCALL(SA_ask_current_role(&CurrentRoleTag));
								ITKCALL(SA_ask_role_name(CurrentRoleTag,roleName))
								printf("\n\n  roleName : %s\n",roleName); fflush(stdout);
								PlantName=subString(roleName,3,4);
								printf( "PlantName:%s\n", PlantName);fflush(stdout);
								if(tc_strcmp(PlantName,"APLD")==0)
								{
									tc_strcpy(ContexPlantVal,"DHARWAD");
								}
								else  if(tc_strcmp(PlantName,"APLP")==0)
								{
									tc_strcpy(ContexPlantVal,"CVBU Pune");								
								}
								else  if(tc_strcmp(PlantName,"APLC")==0)
								{								
									tc_strcpy(ContexPlantVal,"CAR");
								}
								else  if(tc_strcmp(PlantName,"APLJ")==0)
								{
									tc_strcpy(ContexPlantVal,"CVBU JSR");								
								}
								else  if(tc_strcmp(PlantName,"APLL")==0)
								{
									tc_strcpy(ContexPlantVal,"CVBU LKO");								
								}
								else  if(tc_strcmp(PlantName,"APLA")==0)
								{
									tc_strcpy(ContexPlantVal,"SMALLCAR AHD");								
								}
								else  if(tc_strcmp(PlantName,"APLU")==0)
								{
									tc_strcpy(ContexPlantVal,"CVBU PNR");								
								}
								else  if(tc_strcmp(PlantName,"APLS")==0)
								{
									tc_strcpy(ContexPlantVal,"CVBU JSR");								
								}
								else
								{
									tc_strcpy(ContexPlantVal,"CVBU Pune");								
								}
							
								 printf( "ContexPlantVal:%s\n", ContexPlantVal);fflush(stdout);

								

							   ITKCALL(QRY_find("Driver VC Query",&tag_query));
							   printf("Searching...\n");fflush(stdout);

							   DriverVCLCS=NULL;
							   DriverVCLCS=(char *)MEM_alloc(1000);


							  tc_strcpy(DriverVCLCS,"APL Released;STDSI Working;STDSI Released");							 

							  printf("nArgument passed in is: %s...\n",DriverVC);fflush(stdout);
							  printf("nArgument passed in is: %s...\n",DriverVCLCS);fflush(stdout);


							   //entries[0] = (char *)MEM_alloc(strlen(searchEntry[0] ) + 1);
							   //entries[1] = (char *)MEM_alloc(strlen(searchEntry[1] ) + 1);
							   //strcpy(entries[0], searchEntry[0]);
							   //strcpy(entries[1], searchEntry[1]);

							   //values[0] = (char *)MEM_alloc(strlen(DriverVC ) + 1);
							   //values[1] = (char *)MEM_alloc(strlen(DriverVCLCS ) + 1);
							   //strcpy(values[0], DriverVC);
							  // strcpy(values[1], DriverVCLCS);
							
							  //**values		=	(char **) MEM_alloc(10 * sizeof(char *));
							  
							  values[0]=DriverVC;
							  values[1]=DriverVCLCS;

							   CALL(QRY_execute(tag_query,n_entries, entries, values,&num_found,&itemclass1));
							   itemclassp = itemclass1[0];

							   printf("\nNumber of objects found=%d\n",num_found);fflush(stdout);

								ITKCALL(Get_Part_BOM_Lvl(itemclassp,99,"ERC","APL Released data only",ContexPlantVal,&StructChldCnt));//Get VC Multilevel BOM
								printf("\n\t\t StructChldCnt:%d",StructChldCnt);fflush(stdout);
								ITKCALL(AOM_ask_value_tags(DMLLcmTag,"CMHasSolutionItem",&PartCnt,&PartTags));  //Task attached parts
								printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
								if (PartCnt>0)
								{
									for (k=0;k<PartCnt ;k++ )
									{
										int BypassNACSFlag =0;
										printf("\n\n\t\t APL DML Cre:for k =:%d",k);fflush(stdout);
										AssyTag=PartTags[k];
										partfoundinDRVC=0;

										BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
										printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);fflush(stdout);
										 if(BypassNACSFlag == 1)
										{
											printf("\t  Bypass Validations .\n");fflush(stdout);
											continue;
										 }
										 else
										{
											printf("\t  Call Validations .\n");fflush(stdout);
										 }

										ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&taskAttPrtNo));
										printf("\n  taskAttPrtNo:%s ..............",taskAttPrtNo);fflush(stdout);

										/* CHECK FOR ORGANIZATION_ID.IF ORD_ID IS APL OR STD ETC
										   THEN CONTINUE THE LOOP.COMMENTED BELOW CODE OF ORGANIZATION ID.WILL DO AFTER MANAGEMENT DECISION
											:REWORK POINT TZ
										*/

										ITKCALL(AOM_UIF_ask_value(AssyTag,"owning_group",&taskowning_groupVal));
										printf("\n  taskowning_groupVal:%s ..............",taskowning_groupVal);fflush(stdout);

										if((tc_strstr(taskowning_groupVal,"APL")!=NULL)|| (tc_strstr(taskowning_groupVal,"STD")!=NULL) )
										{
											continue;
										}



										/*for ( j = 0; j < n; j++)
										{
											partfoundinDRVC=0;
											BOM_line_ask_attribute_tag 	(children[j],attribute_act_tak,&value_act_Tak);
											AOM_UIF_ask_value(value_act_Tak,"item_id",&Item_id);
											printf("\n\n\t Child Part String is Item_id-->%s",Item_id);fflush(stdout);

											if(tc_strcmp(taskAttPrtNo,Item_id)==0)
											{
												partfoundinDRVC++;
												break;
											}
											else
											{
												tc_strcpy(partFoundStatus,"N");
												MultiExplosion(children[j], top_line, 0,taskAttPrtNo,reqLevel,level,partFoundStatus);
												printf("\n\n\t partFoundStatus-->%s",partFoundStatus);fflush(stdout);
												if(tc_strcmp(partFoundStatus,"Y")==0)
												{
													break;
												}
											}
										}*/

										j = 0;
										printf("\nPrint Item ID==>%d:%d\n",j,StructChldCnt);fflush(stdout);
										for ( j = 1; j <= StructChldCnt; j++)
										{
											
											objChild		= NULLTAG;
											iChildItemTag_DVC	= 0;
											t_ChildItemRev_DVC	= NULLTAG;

											printf("\nPrint Item ID==>%d\n",j);fflush(stdout);

											objChild=get_BomChldStrut[j].child_objs;
											ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag_DVC));
											ITKCALL(BOM_line_ask_attribute_tag(objChild, iChildItemTag_DVC, &t_ChildItemRev_DVC));
											ITKCALL(AOM_ask_value_string(t_ChildItemRev_DVC,"item_id",&ItemName));
											printf("\n\n\t ItemName-->%s",ItemName);fflush(stdout);

											if(tc_strcmp(taskAttPrtNo,ItemName)==0)
											{
												partfoundinDRVC++;
												tc_strcpy(partFoundStatus,"Y");
												break;
											}
											else
											{
												tc_strcpy(partFoundStatus,"N");	
											
											}
										}
										

										printf("\n\n\t partfoundinDRVC-->%d,partFoundStatus %s",partfoundinDRVC,partFoundStatus);fflush(stdout);

										if((partfoundinDRVC==0) && (tc_strcmp(partFoundStatus,"Y")!=0))
										{
											printf("\n\n\t Inside error ");fflush(stdout);
											partCntFnd++;
											printf("\n\n\t Inside error %d",partCntFnd);fflush(stdout);
										}


										if((partfoundinDRVC==0) && (tc_strcmp(partFoundStatus,"Y")!=0))
										{
											printf("\n\n\t Inside error %d",partCntFnd);fflush(stdout);
											if(partCntFnd==1)
											{
												if (tc_strcmp(DriverVCErr,"NULL") !=0) MEM_free(DriverVCErr);
												DriverVCErr=(char *)MEM_alloc(500);
												tc_strcpy(DriverVCErr,"\n Below Mentioned Product Plan Parts not found in Driver VC:\n");
												tc_strcat(DriverVCErr,taskAttPrtNo);
												cnt=cnt+1;
											}
											else
											{
												tc_strcat(DriverVCErr,"\n");
												tc_strcat(DriverVCErr,taskAttPrtNo);
												cnt=cnt+1;
											}
										 if(cnt==1)
											{
												tc_strcpy(SetChildRelErr,DriverVCErr);
												cntFlag = 1;
											}
											else
											{
												tc_strcat(SetChildRelErr,DriverVCErr);
												cntFlag = 1;
											}
										}

									}

//									if(cnt==1)
//									{
//										tc_strcpy(SetChildRelErr,DriverVCErr);
//										cntFlag = 1;
//									}
//									else
//									{
//										tc_strcat(SetChildRelErr,DriverVCErr);
//										cntFlag = 1;
//									}
								printf("\n\n\t cntFlag -->%d",cntFlag);fflush(stdout);

									/* NEED TO ADD LOGIC OF DRIVER VC .CHILD PART OF PRODUCT PLAN ITEM
										SHOULD BE IN DRIVER VC.IT IS BASED ON CONFIGURATION CONTEXT AND
										HENCE IMPLEMENTING THE LOGIC CURRENTLY BASED ON LATEST WORKING.
										WILL MODIFY THE CODE AFTER MANAGEMENT DISCUSSION .ALSO NEED TO
										ADD THE LOGIC OF GRP 98.
									*/
									k=0;
									j=0;
									for (k=0;k<PartCnt ;k++ )
									{
										int BypassNACSFlag=0;
										AssyTag=PartTags[k];

										BypassNACSFlag = NA_CS_Bypass_Checks(PartCnt,AssyTag,PlantCS);
										printf("\t  BypassNACSFlag ...%d\n", BypassNACSFlag);fflush(stdout);
										 if(BypassNACSFlag == 1)
										{
											printf("\t  Bypass Validations .\n");fflush(stdout);
											continue;
										 }
										 else
										{
											printf("\t  Call Validations .\n");fflush(stdout);
										 }

										ITKCALL(AOM_ask_value_string(AssyTag,"item_id",&t5Part_no));
										printf("\n  t5Part_no:%s ..............",t5Part_no);fflush(stdout);

										ITKCALL(AOM_ask_value_string(AssyTag,"t5_DesignGrp",&t5design_grp));
										printf("\n  t5design_grp:%s ..............",t5design_grp);fflush(stdout);

										ITKCALL(AOM_ask_value_string(AssyTag,"t5_PartType",&t5Part_type));
										printf("\n  t5Part_type:%s ..............",t5Part_type);fflush(stdout);

										ITKCALL(AOM_ask_value_string(AssyTag,"item_revision_id",&t5Part_revision));
										printf("\n  t5Part_revision:%s ..............",t5Part_revision);fflush(stdout);

										//qry_values_childprt_rev_id[TCTYPE_name_size_c+1]=t5Part_revision;

										if((tc_strcmp(t5design_grp,"98")==0) && (tc_strcmp(t5Part_type,"T")==0 || tc_strcmp(t5Part_type,"M")==0 ))
										{
											continue;
										}

										qry_entries_childprt[0] ="item_id";
										qry_values_childprt[0] =  t5Part_no;
										

										ITKCALL(ITEM_find_items_by_key_attributes(1, qry_entries_childprt, qry_values_childprt,&n_tags_found_childprt, &itemclass_childprt));
										//ITKCALL(ITEM_find_item_revs_by_key_attributes(1,qry_entries_childprt,qry_values_childprt,qry_values_childprt_rev_id,&n_tags_found_childprt,&itemclass_childprt));
										itemclassp_childprt = itemclass_childprt[0];

										ITKCALL(ITEM_list_all_revs  (itemclassp_childprt, &childpart_revcount1,  &rev_partlist ));
										for (pii = 0; pii < childpart_revcount1; pii++)
										{

											ITKCALL(ITEM_ask_rev_id( rev_partlist[pii], part_rev_id1));

											printf("\n part_rev_id1 is  ---->%s\n", part_rev_id1);fflush(stdout);//diplay the previous revision
											if(tc_strcmp(part_rev_id1,t5Part_revision)==0)
											{
												printf("\n Matched rev id is  ---->%s,%s\n", part_rev_id1,t5Part_revision);fflush(stdout);
												finalitemclassp_childprt=rev_partlist[pii];
												break;
											
											}
										}

										//BOM_create_window(&window);

										//ITKCALL(CFM_find( "Latest Working", &rule_childprt));
										//ITKCALL(BOM_set_window_config_rule( window1, rule1 ));

										/*BOM_set_window_top_line(window,itemclassp_childprt,null_tag,null_tag,&top_line_childprt);
										BOM_line_ask_child_lines(top_line_childprt,&n_prt,&prt_children);
										printf("\n\n\t no:of action taken are -->%d",n_prt);fflush(stdout);
										BOM_line_look_up_attribute("bl_revision",&attribute_act_tak_childprt);

										BOM_create_window(&window);

										ITKCALL(CFM_find( "Latest Working", &rule));
										ITKCALL(BOM_set_window_config_rule( window, rule ));

										n=0;
										BOM_set_window_top_line(window,itemclassp,null_tag,null_tag,&top_line);
										ITKCALL(BOM_line_ask_child_lines (top_line, &n, &children));
										printf("\n\n\t no:of action taken are -->%d",n);fflush(stdout);
										BOM_line_look_up_attribute 	("bl_revision",&attribute_act_tak);*/


										ITKCALL(Get_Part_BOM_Lvl(finalitemclassp_childprt,1,PlantName,"APL Released data only",ContexPlantVal,&StructCntProdPlan));//Get Multilevel BOM for Product Plan Items
										printf("\n\t\t StructCntProdPlan:%d",StructCntProdPlan);fflush(stdout);


										//for (ds = 0; ds < n_prt; ds++)
										ds = 0;
										for (ds = 1; ds <= StructCntProdPlan; ds++)
										{

											childpartfoundinDRVC=0;

											objChild_ProdtPlan		= NULLTAG;
											iChildItemTag_ProdtPlan	= 0;
											t_ChildItemRev_ProdtPlan	= NULLTAG;

											printf("\n111 Print Item ID==>%d,%d\n",ds,StructCntProdPlan);fflush(stdout);

											objChild_ProdtPlan=get_BomChldStrut[ds].child_objs;
											ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag_ProdtPlan));
											ITKCALL(BOM_line_ask_attribute_tag(objChild_ProdtPlan, iChildItemTag_ProdtPlan, &t_ChildItemRev_ProdtPlan));
											ITKCALL(AOM_ask_value_string(t_ChildItemRev_ProdtPlan,"item_id",&ItemName_ProdtPlan));
											printf("\n\n\t ItemName_ProdtPlan-->%s",ItemName_ProdtPlan);fflush(stdout);

											if( AOM_UIF_ask_value(t_ChildItemRev_ProdtPlan,"owning_group",&owning_group_val)==ITK_ok)
											printf("\n after owning_group_val in Driver VC Value :--------------------   %s\n",owning_group_val); fflush(stdout);

											if (tc_strstr(owning_group_val,"APL")!=NULL || tc_strstr(owning_group_val,"STD")!=NULL)
											{
												continue;
											}


											
											/*COMMENTED BELOW CODE OF ORGANIZATION ID.WILL DO AFTER MANAGEMENT DECISION :REWORK POINT TZ*/

											j=0;
											for ( j = 1; j <= StructChldCnt; j++)
											{
												objChild		= NULLTAG;
												iChildItemTag_DVC	= 0;
												t_ChildItemRev_DVC	= NULLTAG;

												printf("\n11134 Print Item ID==>%d,%d\n",j,StructChldCnt);fflush(stdout);

												objChild=get_BomChldStrut[j].child_objs;
												ITKCALL(BOM_line_look_up_attribute (( char * ) bomAttr_lineItemRevTag , &iChildItemTag_DVC));
												ITKCALL(BOM_line_ask_attribute_tag(objChild, iChildItemTag_DVC, &t_ChildItemRev_DVC));
												ITKCALL(AOM_ask_value_string(t_ChildItemRev_DVC,"item_id",&ItemName));
												printf("\n\n\t ItemName-->%s",ItemName);fflush(stdout);

												childpartfoundinDRVC=0;
												if(tc_strcmp(ItemName_ProdtPlan,ItemName)==0)
												{
													childpartfoundinDRVC++;
													tc_strcpy(partFoundStatus1,"Y");
													break;

												}
												else
												{
													tc_strcpy(partFoundStatus1,"N");
													
												}
											}

											printf("\n\n\t childpartfoundinDRVC-->%d,partFoundStatus1 %s",childpartfoundinDRVC,partFoundStatus1);fflush(stdout);

											if((childpartfoundinDRVC==0) && (tc_strcmp(partFoundStatus1,"Y")!=0))
											{
												printf("inside error calling");fflush(stdout);
												childpartCntFnd++;
											}

											if((childpartfoundinDRVC==0)&& (tc_strcmp(partFoundStatus1,"Y")!=0))
											{
												printf("inside error calling1");fflush(stdout);
												if(childpartCntFnd==1)
												{
													if (tc_strcmp(DriverVCChildErr,"NULL") !=0) MEM_free(DriverVCChildErr);
													DriverVCChildErr=(char *)MEM_alloc(500);
													tc_strcpy(DriverVCChildErr,"\n Below Mentioned Child Parts of Product Plan item are not found in Driver VC: \n");
													tc_strcat(DriverVCChildErr,ItemName);
													cnt = cnt+1;
												}
												else
												{
													tc_strcat(DriverVCChildErr,"\n");
													tc_strcat(DriverVCChildErr,ItemName);
													cnt = cnt+1;
												}

												if(cnt==1)
												{
													tc_strcpy(SetChildRelErr,DriverVCChildErr);
													cntFlag = 1;
												}
												else
												{
													tc_strcat(SetChildRelErr,DriverVCChildErr);
													cntFlag = 1;
												}
											}

										}

									}

											

								}

								if((partCntFnd>0)|| (childpartCntFnd>0))
								{
									decision = EPM_nogo;
									return ITK_errStore1;
								}
							}
						}


					}
				}
			}
			// End for Driver VC Part check

	return 0;
}

extern EPM_decision_t DLLAPI apl_dml_signoff_checks_Func(EPM_rule_message_t msg)
{
	EPM_decision_t decision;
	int status;
	tag_t ItemTypeTag = NULLTAG;
	int iNumAttch = 0;
	int count = 0;
	int k = 0,Item_ID=0,childCnt=0;
	int count_child = 0;
	int   attribute_act_tak;
	tag_t			rootTask				=NULLTAG;
	char		*CurrentTask					= NULL;
	char       *parent_name				=NULL;
	char *Item_id = NULL;
	char *Item_rev = NULL;
	char *Item_LCS = NULL;
	char *Item_Lcs_str = NULL;
	tag_t * pTagAttch;
	int i = 0;
	int j = 0;
	int n = 0;
	int stampingdone=0;
	int iGChld = 0;
	char*	itemid;
	char *  ChildRelErr = NULL;
	char *  SetChildRelErr = NULL;
	char*	childPartNo;
	char*	revid;
	char *viewtype_name=NULL;
	char  type_name[TCTYPE_name_size_c+1];

	char  type_itemRev[TCTYPE_name_size_c+1];
	char  ChildRevtype_name[TCTYPE_name_size_c+1];
	tag_t	window								= NULLTAG;
	tag_t	rule								= NULLTAG;
	tag_t	top_line							= NULLTAG;
	tag_t	Childtop_line							= NULLTAG;
	tag_t	view_type							= NULLTAG;
	tag_t	item_rev_tag							= NULLTAG;
	tag_t	itemTypeTag_cdss							= NULLTAG;
	tag_t	*itemclassp	= NULLTAG;
	tag_t	itemcldclass							= NULLTAG;
	tag_t	value_child_details							= NULLTAG;
	char  req_id[ITEM_id_size_c+1];
	tag_t  *children;
	tag_t  *totalchildren;
	tag_t  	value_act_Tak;
	tag_t  	item_rev_cld_tag;
	tag_t*			PartTags			= NULLTAG;
	int				PartCnt				= 0;
	int				AttCnt				= 0;
	int				ii				= 0;
	char*			value					=NULL;
	tag_t			tsk_part_sol_rel_type= NULLTAG;
	tag_t			AssyTag				= NULLTAG;
	char*			Part_no				= NULL;
	int       st_count=0;
	int       iLCS=0;
	int       cntLcs=0;
	int				BypassNACSFlag=0;
	char PlantCS[40];
	char PlantOptCS[40];
	char PlantIA[40];
	char PlantStore[40];
	char UserAgency[40];
	tag_t  CurrentRoleTag = NULLTAG;
	char       roleName[SA_name_size_c+1]  ;
    cnt = 0;
	cntFlag = 0;


	printf("\n Calling apl_dml_signoff_checks_Func %d.....\n",iNumAttch);

	ITKCALL(EPM_ask_root_task(msg.task,&rootTask));
	printf("\n apl_dml_signoff_checks_Func \n"); fflush(stdout);

	ITKCALL(AOM_ask_value_string(msg.task,"object_name",&CurrentTask));
   printf("\nCurrentTask:%s\n",CurrentTask);
	ITKCALL(AOM_ask_value_string(rootTask,"object_name",&parent_name));
	printf("\nParent Name:%s\n",parent_name);fflush(stdout);
	ITKCALL( EPM_ask_attachments( rootTask, EPM_target_attachment, &iNumAttch, &pTagAttch) );

	ITKCALL(SA_ask_current_role(&CurrentRoleTag));
	ITKCALL(SA_ask_role_name(CurrentRoleTag,roleName))
	printf("\n\n  roleName : %s\n",roleName); fflush(stdout);


	getPlantDetailsAttr(roleName,PlantCS,PlantOptCS,PlantIA,PlantStore,UserAgency);
	printf("\n PlantCS %s \n",PlantCS);
	printf("\n PlantOptCS %s \n",PlantOptCS);
	printf("\n PlantAgency %s \n",PlantIA);
	printf("\n PlantStore %s \n",PlantStore);
	printf("\n UserAgency %s \n",UserAgency);


     if(tc_strcmp(CurrentTask,"Work On DML" )==0)
	{
 	ITKCALL( TCTYPE_find_type("T5_APLTaskRevision", NULL, &ItemTypeTag));

	printf("\n iNumAttch %d.....\n",iNumAttch);
	for (i=0; i < iNumAttch; i++)
	{
		tag_t objTypeTag = NULLTAG;
		tag_t objChildRevTag = NULLTAG;

		const char *qry_entries[1];
		const char *qry_values[1];

//		char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
//		char **values = (char **) MEM_alloc(1 * sizeof(char *));

		int n_tags_found=0;
		 tag_t	*itemclass							= NULLTAG;
		  tag_t item = NULLTAG;

		ITKCALL(AOM_ask_value_string(pTagAttch[i], "item_id",&itemid));
		printf("\n itemid %s.....\n",itemid);
		qry_entries[0] ="item_id";
		qry_values[0] = itemid;

		ITEM_find_items_by_key_attributes(1, qry_entries, qry_values,&n_tags_found, &itemclass);
		 item = itemclass[0];
		if(item != NULLTAG )
		ITKCALL(ITEM_ask_latest_rev(item,&item_rev_tag));

		printf("\n itemid %s \n",itemid);
		ITKCALL( TCTYPE_ask_object_type (pTagAttch[i], &objTypeTag));
		ITKCALL( TCTYPE_ask_name( objTypeTag, type_name) );
		printf("\t  type_name ...%s\n", type_name);
		if( objTypeTag == ItemTypeTag )
		{
			printf("\t \n objTypeTag == ItemTypeTag \n");

			ITKCALL (TCTYPE_ask_object_type(item_rev_tag,&itemTypeTag_cdss));
			ITKCALL (TCTYPE_ask_name(itemTypeTag_cdss,type_itemRev));

			printf("\t  type_itemRev ...%s\n", type_itemRev);
			if (!SetChildRelErr) MEM_free(SetChildRelErr);
			SetChildRelErr=(char *)MEM_alloc(5000);
            tc_strcpy(SetChildRelErr,"");

			if(tc_strcmp(type_itemRev,"T5_APLTaskRevision" )==0)
			{

    		ITKCALL(AOM_ask_value_tags(item_rev_tag,"CMHasSolutionItem",&PartCnt,&PartTags));
			printf("\n\n\t\t APL DML Cre:Now PartCnt:%d",PartCnt);fflush(stdout);
			if (PartCnt>0)
			{

				GRM_find_relation_type("CMHasSolutionItem",&tsk_part_sol_rel_type);
				printf("\n\n\t\t for k =:%d",k);fflush(stdout);

				printf("\n cntFlag_1: %d",cntFlag);fflush(stdout);
				CS_Successor_Update(PartCnt,PartTags,PlantCS);
		        printf("\n cntFlag_2: %d",cntFlag);fflush(stdout);

				Creation_Grp98Part_Checks(item_rev_tag,SetChildRelErr); //DEEPTI TZ1.36
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
    		    printf("\n cntFlag_16: %d",cntFlag);fflush(stdout);

				CS_Parent_Child_Checks(PartCnt,PartTags,SetChildRelErr,PlantCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
				printf("\n cntFlag_3: %d",cntFlag);fflush(stdout);

				Child_Part_Rev_Checks(item_rev_tag,PartCnt,PartTags,SetChildRelErr,PlantCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
      			 printf("\n cntFlag_4: %d",cntFlag);fflush(stdout);

                Prev_Part_Rev_Checks(PartCnt,PartTags,SetChildRelErr,PlantCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
      			 printf("\n cntFlag_5: %d",cntFlag);fflush(stdout);

				ERC_APL_DML_Checks(PartCnt,item_rev_tag,PartTags,SetChildRelErr,PlantCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
    			printf("\n cntFlag_6: %d",cntFlag);fflush(stdout);

				Hanging_Part_Checks(PartCnt,PartTags,SetChildRelErr,PlantCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
    		    printf("\n cntFlag_7: %d",cntFlag);fflush(stdout);

				CS_Mismatch_Checks(PartCnt,PartTags,SetChildRelErr,PlantCS,PlantOptCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
				 printf("\n cntFlag_8: %d",cntFlag);fflush(stdout);

                Two_Rev_Same_Day_Checks(PartCnt,PartTags,SetChildRelErr,itemid,PlantCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
                 printf("\n cntFlag_9: %d",cntFlag);fflush(stdout);

                BOM_Parts_Checks(PartCnt,PartTags,SetChildRelErr,PlantCS,UserAgency);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
                printf("\n cntFlag_10: %d",cntFlag);fflush(stdout);

                Drg_Part_Checks(PartCnt,PartTags,SetChildRelErr,itemid,PlantCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
                 printf("\n cntFlag_11: %d",cntFlag);fflush(stdout);

                Group_Parts_Stamping(PartCnt,PartTags,SetChildRelErr,PlantCS,UserAgency);
                printf("\n cntFlag_12: %d",cntFlag);fflush(stdout);

				CS_IA_Mismatch_Checks(PartCnt,PartTags,SetChildRelErr,PlantCS,PlantIA,UserAgency);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
                 printf("\n cntFlag_13: %d",cntFlag);fflush(stdout);

				CS_StoreLoc_Mismatch_Checks(PartCnt,PartTags,SetChildRelErr,PlantCS,PlantStore,UserAgency);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
                printf("\n cntFlag_14: %d",cntFlag);fflush(stdout);

				Driver_VC_Check(item_rev_tag,SetChildRelErr,PlantCS);
		        printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
                 printf("\n cntFlag_15: %d",cntFlag);fflush(stdout);
			} //End for Part

		   printf("\n SetChildRelErr: %s\n",SetChildRelErr);fflush(stdout);
			//if (tc_strcmp(SetChildRelErr,"NULL")!=0)
			if(cntFlag==1)
			{
				printf("\n All Child Part is not TCM Released  %s \n",SetChildRelErr);
				EMH_clear_errors();
				status=EMH_store_error_s1(EMH_severity_error,ITK_Prjerr,SetChildRelErr);
				//printf("\n All Child Part is not TCM Released  %s \n",SetChildRelErr);
				//status=EMH_store_error_s2(EMH_severity_error,ITK_errStore,SetChildRelErr ,"Child Part Release Error");
				decision = EPM_nogo;
				return ITK_errStore;
			}

	  }
    }
  }
 }
	decision = EPM_go;
	return decision;
}

extern int apl_user_assign_register_method(int *decision, va_list args)
{
    METHOD_id_t  method1;
    METHOD_id_t  method2;
    int ifail=0;
   // *decision = ALL_CUSTOMIZATIONS ;
    char   type_name[TCTYPE_name_size_c+1];
    char   type_name1[TCTYPE_name_size_c+1];
    tag_t objTag = va_arg(args, tag_t);
    tag_t objTypeTag=NULLTAG;

   if( ITK_ok == EPM_register_action_handler("apl_dml_assign", "", apl_dml_assign))
   {
		printf("\t\n Registered Action Handler apl_user_assign_register_method\n");fflush(stdout);
   }else
   {
		printf("\t FAILED to register Action Handler apl_user_assign_register_method\n");fflush(stdout);
   }

   if( ITK_ok == EPM_register_action_handler("apl_dml_create", "", apl_dml_create))
   {
		printf("\t\n Registered Action Handler apl_dml_create_register_method\n");fflush(stdout);
   }else
   {
		printf("\t FAILED to register Action Handler apl_dml_create_register_method\n");fflush(stdout);
   }

	if( ITK_ok == EPM_register_action_handler("apl_dml_release", "", apl_dml_release))
   {
		printf("\t\n Registered Action Handler apl_dml_release_register_method\n");fflush(stdout);
   }else
   {
		printf("\t FAILED to register Action Handler apl_dml_release_register_method\n");fflush(stdout);
   }

	if( ITK_ok == EPM_register_action_handler("apl_dml_claim", "", apl_dml_claim))
   {
		printf("\t\n Registered Action Handler apl_dml_claim_register_method\n");fflush(stdout);
   }else
   {
		printf("\t FAILED to register Action Handler apl_dml_claim_register_method\n");fflush(stdout);
   }

   ifail = METHOD_find_method("T5_APLDMLRevision", IMAN_save_msg, &method1);
	if( ifail == ITK_ok )
	{
		printf("\n Before register method for T5_APLDMLRevision \n");
		if (method1.id != NULLTAG)
		{
			//if( METHOD_add_action(method1, METHOD_post_action_type, (METHOD_function_t) apl_dml_DriverVc_attach, NULL)!=ITK_ok)//DEEPTI ADDED1.34
			if( METHOD_add_action(method1, METHOD_pre_action_type, (METHOD_function_t) apl_dml_DriverVc_attach, NULL)!=ITK_ok)
			printf("\n rrrrrrrrRegistered as Post-Action for T5_APLDMLRevision Creation\n");fflush(stdout);
		}else
		{
			printf("\n Method not found for T5_APLDMLRevision !TC_save_msg\n");
		}
	}

	ifail = METHOD_find_method("CMHasSolutionItem", GRM_create_msg, &method2);
	if( ifail == ITK_ok )
	{
		printf("\n Before register method for GRM_create_msg \n");
		if (method2.id != NULLTAG)
		{
			if( METHOD_add_action(method2, METHOD_pre_action_type, (METHOD_function_t) apl_dml_part_attach, NULL)!=ITK_ok)
			printf("Registered as METHOD_pre_action_type for A9_ValidationAtAMDML\n");
		}
		else
		printf("Method not found!GRM_create_msg\n");
	}

	if( ITK_ok == EPM_register_rule_handler ("apl_dml_signoff_checks_Func","This Handler is used to display message",(EPM_rule_handler_t)apl_dml_signoff_checks_Func))
   {
		printf("\t\n Registered Rule Handler apl_dml_signoff_checks_Func\n");fflush(stdout);
   }else
   {
		printf("\t FAILED to register Rule Handler apl_dml_signoff_checks_Func\n");fflush(stdout);
   }


   ifail = METHOD_find_method("T5_APLDMLRevision", ITEM_create_msg, &method1);
   if (method1.id != NULLTAG)
   {
			ifail = METHOD_add_action(method1, METHOD_post_action_type, (METHOD_function_t) createAplCR_Task, NULL)!=ITK_ok;
			//PrintErrorStack();
			printf("\nrrrrrrrrRegistered as Post-Action for ChangeRequestRevision Creation\n");fflush(stdout);
   }else
		{
			printf("\nMethod not found!TC_save_msg\n");fflush(stdout);
		}

    return ITK_ok;
}

extern DLLAPI int apl_dml_assign_register_callbacks()
{
	 CUSTOM_register_exit("apl_dml_assign", "USER_init_module", (CUSTOM_EXIT_ftn_t)apl_user_assign_register_method);

     printf("\n registered function apl_dml_assign\n");	fflush(stdout);

	 return ( ITK_ok );
}
