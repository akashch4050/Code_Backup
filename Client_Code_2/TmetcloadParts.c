/***************************************************************************
* Copyright (c) 2007 Tata Technologies Limited (TTL). All Rights Reserved.
*
* This software is the confidential and proprietary information of TTL.
* You shall not disclose such confidential information and shall use it
* only in accordance with the terms of the license agreement.
*
*  File			 :   loadParts.c
*  Author		 :   Aniket P. Kadu
*  Module		 :   TCUA Uploader
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
#include <tccore/libtccore_exports.h>
extern TCCORE_API int TCTYPE_ask_type(
        const char      *type_name,
        tag_t           *type_tag);
#include <tccore/libtccore_undef.h>


//#include <tc_ver_info.h>
#define TC_MAJOR_VERSION 2007

#include <tcinit/tcinit.h>
#if TC_MAJOR_VERSION <= 10
# define TC_master_form_rtype IMAN_master_form_rtype
#endif

/*===========================================================================*/

#define MAX_LINE_LEN 500
#define TCTYPE_name_size_c 100
#define DEFAULT_DELIMITER " "
#define DEFAULT_SUB_DELIMITER ";"

#define MEM_SAVE_THRESHOLD 10000

#define ITK_CALL(X) 							\
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
			printf("%3d error(s) with #X\n", n_ifails);						\
			for( index=0; index<n_ifails; index++)							\
			{																\
				printf("\tError #%d, %s\n", ifails[index], texts[index]);	\
			}																\
			return status;													\
		}																	\
	;


const char delim_text[] = "DELIMITER";
const char sub_delim_text[] = "SUB_DELIMITER";
const char col_text[] = "COL";

/*===========================================================================*/

/* Container for all the information for one line.
*/

typedef struct sub_array_s
{
    unsigned int no;
    char **ids;
} sub_array_t, *sub_array_p_t;

typedef struct input_line_s
{
    struct input_line_s *child_chain;
    struct input_line_s *sibling_chain;
    char *item_id;
    char *key;
    char *arch_element_id;
    char *item_name;
    char *desc;
    char *item_rev_name;
    char *seq_no;
    char *occ_name;
    int level;
    int no_of_occs;
    double qty_per_occ;
    char *unit_of_measure;
    sub_array_t sub;
    char *item_revision_id;
    char *item_type;
    char *LhRh;
    tag_t bvr;
    sub_array_t options;
    sub_array_t loadif;
} input_line_t, *input_line_p_t;

typedef enum
{
    Column_Item,
    Column_Revision,
    Column_ArchElementID,
    Column_Options,
    Column_LoadIf,
    Column_Name,
    Column_RevName,
    Column_Level,
    Column_Seq,
    Column_Occs,
    Column_Qty,
    Column_Uom,
    Column_Sub,
    Column_Type,
    Column_OccName,
    Column_Desc,
    Column_LhRh,

    Num_Columns,

    Column_None
} column_t, *column_p_t;


typedef struct {
        column_t type;
        char *label;
        char *description;
} col_item_t, *col_item_p_t;



static col_item_t col_defs[Num_Columns + 1] =
{
        Column_Item,    "item",   "Item ID",
        Column_Revision,"rev",    "Revision letter (default=System Assigned RevID)",
        Column_ArchElementID    ,"arch_element_id",  "Architecture Element Id",
        Column_Options, "option", "Option" DEFAULT_SUB_DELIMITER "Value" DEFAULT_SUB_DELIMITER "Value...",
        Column_LoadIf,  "loadif", "ItemID" DEFAULT_SUB_DELIMITER "Option ==/!= Value",
        Column_Name,    "name",   "Item name",
        Column_RevName, "revname","Item revision name",
        Column_Level,   "level",  "Determines the structural hierarchy (top = 0)",
        Column_Seq,     "seq",    "Sequence number for item in the BOM view",
        Column_Occs,    "occs",   "Number of occurrences in this item",
        Column_Qty,     "qty",    "Quantity of an item in the structure",
        Column_Uom,     "uom",    "Unit of measure (symbol, not the name)",
        Column_Sub,     "sub",    "Substitutes (default delimiter=" DEFAULT_SUB_DELIMITER ")",
        Column_Type,    "type",   "Item type",
        Column_OccName, "occname", "Occurrence path name",
        Column_Desc,    "desc",    "Description",
        Column_LhRh,    "lhrh",    "LH RH Indicator",
        Column_None,    "",       "",
};



static column_t column_array[Num_Columns + 1] =
{
    Column_Item,
    Column_ArchElementID,
    Column_Name,
    Column_Level,
    Column_Seq,
    Column_Occs,
    Column_Qty,
    Column_Uom,
    Column_Sub,
    Column_Desc,
    Column_LhRh,
    Column_None
};

/*===========================================================================*/

static void initialize( void );
static void display_help_message(void);
static void display_file_format_message(void);
static input_line_p_t read_children( input_line_p_t  parent_line,  FILE *  infile,  int *  total_lines );
static void strip_trailing_spaces( char *string );
static char *strip_space( char *str );
static char *get_next_token( char *str, const char *delimiter );
static char *token_to_string( const char *str );
static int token_to_int( const char *str );
static double token_to_double ( const char * );
static void token_to_substitutes( char *str,
                                 sub_array_t *sub,
                                 char *delimiter );
static void get_columns( column_p_t column_array, char *str );
static input_line_p_t read_line( FILE* infile );
static void print_structure( input_line_p_t line, int level );
static void print_structure_level( input_line_p_t line, int level );
static void save_structure( input_line_p_t line );
static void save_structure_level( input_line_p_t line );
static void add_to_newstuff( const input_line_p_t line );
static void build_structure( input_line_p_t  line,  const input_line_p_t  parent,  int  total_lines,
                             logical  override,  logical  mem_save_mode,  logical  verbose );
static void build_structure_level( input_line_p_t  line,  const input_line_p_t  parent,  int  total_lines,
                                   logical  override,  logical  mem_save_mode,  logical  verbose );
static void handle_ifail(char *file, int line);
static char* default_null_to_A(char *s);
static char* default_empty_to_A(char *s);
static void unload_master_forms( tag_t  obj );
static char *default_item_type="Design";    /* Default item Type. */
static char *item_class="Design";    /* Item (sub-)class to create. */
static char *my_name="loadParts";

/*===========================================================================*/

#define WHITESPACE " \t"
#define HANDLE_IFAIL   handle_ifail( __FILE__, __LINE__ )
#define CHECK_IFAIL    if ( ifail != ITK_ok ) HANDLE_IFAIL;
#define NEW_CHECK_IFAIL    if ( ifail != ITK_ok ) {MEM_free(attrs); MEM_free(values); HANDLE_IFAIL;}

/*===========================================================================*/

char *keyFromItemId(char *itemId)
{
    static const char *ITEMKEY_PREFIX = "item_id=";
    const size_t ITEMKEY_PREFIX_LENGTH = strlen(ITEMKEY_PREFIX);

    char *key = (char *) MEM_alloc(strlen(itemId) + ITEMKEY_PREFIX_LENGTH + 1);

    strcpy(key, ITEMKEY_PREFIX);
    strcat(key, itemId);
    return key;
}

extern int ITK_user_main( int argc, char **argv )
{
    int ifail;
    int  total_lines = 0;
    logical  override = true;
    logical  verbose = false;
    char *default_item_type_parameter=NULL;
    char *item_class_parameter=NULL;

    input_line_p_t top_line, dummy;

    char* argstring;
    FILE* input=NULL;

    initialize( );
 //if( ITK_init_module("loader" ,"loader7","dba")!=ITK_ok) ;
    // changes to autologin

    if(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
	printf("\n Auto login ");fflush(stdout);
	if(ITK_auto_login( ));
    if(ITK_set_journalling( TRUE ));
	printf("\n Auto login .......\n");fflush(stdout);   

    default_item_type_parameter = ITK_ask_cli_argument("-t=");
    if ( NULL != default_item_type_parameter )
        default_item_type = default_item_type_parameter;

    item_class_parameter = ITK_ask_cli_argument("-c=");
    if ( NULL != item_class_parameter )
        item_class = item_class_parameter;

    argstring = ITK_ask_cli_argument("-o=");

    if ( argstring != NULL  &&  strcmp( argstring, "off" ) == 0 )
    {
        override = false;
        printf( "Overwrite mode is off\n" );
    }

    verbose = (ITK_ask_cli_argument( "-v" ) != 0);

    argstring = ITK_ask_cli_argument( "-i=");
    if (argstring != 0)
    {
        /* user has given an input file, rather then using stdin */
        if( !(input = fopen( argstring, "rt" )))
        {
            fprintf(stderr, "Cannot open input file %s\n",argstring);
            exit( EXIT_FAILURE );
        }
        else
            printf("Using input file %s\n",argstring);
    }
    else
    {
        printf("Input from Standard Input (stdin)\n");
    }

	//printf("\n Item Class :[%s]",item_class);

    top_line = read_line( input ? input : stdin );
    printf("top_line --> item_name :%s:\n",top_line->item_name);
    printf("top_line --> LHRH :%s:\n",top_line->LhRh);
    while (top_line->level != -1)
    {
        logical  mem_save_mode;

		printf("top_line --> LHRH11 :%s:\n",top_line->LhRh);

        dummy = read_children( top_line, input ? input : stdin, &total_lines );
        if ( !strcmp( "Architecture", (item_class!=NULL?item_class:"Item") ) )
            printf( "\nread in structure1: "
                    "(id,"
                    "arch_element_id,"
                    "name, level, seq_no,no_of_occs, qty_per_occ, uom, subs,lhrh)\n" );
        else
            printf( "\n\nread in structure2: "
                    "(id,"
                    ""
                    "name, level, seq_no,no_of_occs, qty_per_occ, uom, subs,lhrh)\n\n" );fflush(stdout);
        print_structure_level( top_line, 0 );

        mem_save_mode = (total_lines >= MEM_SAVE_THRESHOLD);
        if ( verbose  &&  mem_save_mode )
        {
            printf( "Large structure (%d lines), using memory saving mode\n", total_lines );
        }

        build_structure_level( top_line, NULL, total_lines, override, mem_save_mode, verbose );

        if ( !mem_save_mode )
        {
            save_structure_level( top_line );
        }
        add_to_newstuff( top_line );

        top_line = dummy;
    }
    ifail = ITK_exit_module( true );
    CHECK_IFAIL;

    if (input)
        fclose(input);

    return EXIT_SUCCESS;
}

/*===========================================================================*/

static void initialize(void)
{
    int  ifail;

    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );


    if ( ITK_ask_cli_argument( "-h" ) != 0 )
    {
        display_help_message();
        exit( EXIT_FAILURE );
    }

    if ( ITK_ask_cli_argument( "-f" ) != 0 )
    {
        display_file_format_message();
        exit( EXIT_FAILURE );
    }

    ifail = ITK_auto_login ();
    CHECK_IFAIL;
}

/*===========================================================================*/

static void display_help_message(void)
{
    printf( "\n\nps_upload:    Creates (imprecise) product structures "
            "based on an input file");
    printf( "\n\nUSAGE:  ps_upload -u=username -p=password -g=groupname");
    printf( "\n                    [-o=][-h][-f][-c=<itemclass>][-t=<type>][-v][-i=<inputfile>]");
    printf( "\n        -u=username infodba or user with administrative privileges");
    printf( "\n                    If this argument is used without a value, the operating");
    printf( "\n                    system user name is used.");
    printf( "\n                    Note:");
    printf( "\n                    If Teamcenter Security Services single sign-on (SSO) is");
    printf( "\n                    enabled for your server, the -u and -p arguments are");
    printf( "\n                    authenticated externally through SSO rather than being");
    printf( "\n                    authenticated against the Teamcenter database.  If you do");
    printf( "\n                    not supply these arguments,the utility attempts to join an");
    printf( "\n                    existing SSO session. If no session is found, you are");
    printf( "\n                    prompted to enter a user ID and password.");
    printf( "\n        -p=password specifies the password.");
    printf( "\n                    If used without a value, the system assumes a null value.");
    printf( "\n        -g=groupname specifies the group associated with the user.");
    printf( "\n                    If used without a value, user's default group is assumed.");
    printf( "\n        -o=off turns overwrite mode off - default is to overwrite");
    printf( "\n        -h     displays this message");
    printf( "\n        -f     displays a help message about file format");
    printf( "\n        -c=<itemclass>  specifies the item class name to create");
    printf( "\n                        (Item or Architecture)");
    printf( "\n        -t=<type>       specifies the default item type to create");
    printf( "\n        -v              displays verbose information");
    printf( "\n        -i=<inputfile>  specifies the fullpath to an inputfile\n\n");
}

/*===========================================================================*/

static void display_file_format_message(void)
{
    printf( "File format:"
            "\nOne item per line, fields separated by value of #DELIMITER"
            "\n"
            "\nComments start a line with #, but so do directives"
            "\n  #DELIMITER x       (default: " DEFAULT_DELIMITER ")"
            "\n  #SUB_DELIMITER x   (default: " DEFAULT_SUB_DELIMITER ")"
            "\n  If x is omitted, space is assumed."
            "\n"
            "\n  #COL field field ...    (default:" );
    {
        int i;
        printf("%s", col_defs[column_array[0]].label);
        for ( i=1 ; column_array[i] != Column_None ; i++ )
        {
            printf( " %s",
                    col_defs[column_array[i]].label );
        }
    }
    printf( ")\nValid fields are:\n" );
    {
        col_item_p_t col;
        for ( col=col_defs; col->type != Column_None; col++ )
        {
            printf( "%10s : %s\n",
                    col->label,
                    col->description );
        }
    }
    printf("Substitutes to be separated by the value of #SUB_DELIMITER\n");
}

/*===========================================================================*/

static input_line_p_t read_children( input_line_p_t  parent_line,  FILE *  infile,  int *  total_lines )
{
    input_line_p_t last_sibling = NULL;
    input_line_p_t child_line = read_line( infile );
    (*total_lines)++;

    while (child_line->level > parent_line->level)
    {
        if (last_sibling != NULL)
        {
            last_sibling->sibling_chain = child_line;
        }
        else
        {
            parent_line->child_chain = child_line;
        }
        last_sibling = child_line;
        child_line = read_children( child_line, infile, total_lines );
    }
    return child_line;
}

/*===========================================================================*/

static void strip_trailing_spaces( char *string )
{
    int i = strlen( string ) - 1;
    while (i >= 0  &&  string[i] == ' ')
    {
        --i;
    }
    string[i + 1] = '\0';
}

/*===========================================================================*/

static char *strip_space( char *str )
{
    strip_trailing_spaces( str );

    return &str[strspn( str, " \t" )];
}


/*===========================================================================*/

static char *get_next_token( char *str, const char *set )

/* Return the next token from the input line, with leading and trailing space
** stripped. If there is no token, or just a dash, return an empty string token
*/

{
    static char *empty = "";

    char *temp = strtok( str, set );

    if (temp == NULL)
    {
        return empty;
    }
    else
    {
        temp = strip_space( temp );
        if (temp[0] == '-' && temp[1] == '\0')
        {
            return empty;
        }
        else
        {
            return temp;
        }
    }
}

/*===========================================================================*/

static char *token_to_string( const char *str )

/* Copy a string token into fresh memory.
*/

{
    char *value = (char *) MEM_alloc( (strlen( str ) + 1) * sizeof( char ) );

    strcpy( value, str );

    return value;
}

/*===========================================================================*/

static int token_to_int( const char *str )

/* Convert a string representation of a decimal number to an integer value.
** (Note that this defaults to zero on error e.g. if the string is empty).
*/

{
    return (int) strtol( str, NULL, 10);
}

/*===========================================================================*/

static double token_to_double ( const char *str )
{
    return strtod( str, NULL);
}

/*===========================================================================*/

static void token_to_substitutes( char *str,
                                 sub_array_t *sub,
                                 char *sub_delimiter )

/* Break up a string into a list of substitute ids
*/

{
    if (*str == '\0')
    {
        sub->no = 0;
    }
    else
    {
        char *search;
        int i;

        /* the number of substitutes is 1 plus the number of delimiter
           characters */
        sub->no = 1;

        for (search = str; *search != '\0'; ++search)
        {
            if (*search == *sub_delimiter)
            {
                ++sub->no;
            }
        }

        sub->ids = (char **) MEM_alloc( (sub->no) * sizeof( char * ) );

        for (i = 0; i < sub->no; ++i)
        {
            sub->ids[i] =
                token_to_string( get_next_token( str, sub_delimiter ) );
            str = NULL;
        }
    }
}
/*===========================================================================*/

static void get_columns( column_p_t column_array, char *str )

/* Get the column headings from the input line, and build the column definition
** array.
*/

{
    col_item_t *col_def;

    char *p;
    int i;
    char *heading;

    /* convert all the column headings to lower case */
    for (p = str; *p != '\0'; ++p)
        *p = tolower( *p );


	printf( "str-----> :%s: \n",str);

    /* take each word on the #COL line, and put the corresponding column_t enum
    ** value into column_array
    */
    for (i = 0, heading = strtok( str, WHITESPACE );
         heading != NULL;
         ++i, heading = strtok( NULL, WHITESPACE ))
    {
        if (i == Num_Columns)
        {
            fprintf( stderr, "INPUT ERROR: too many column headings\n" );
            exit( EXIT_FAILURE );
        }

        for (col_def = col_defs; col_def->type != Column_None; ++col_def)
        {
            /* Note that we need strncmp rather than strcmp here, because
            ** heading is not null-terminated (it is just the next part of the
            ** #COL line).
            */
            if (strlen( heading ) == strlen( col_def->label ) &&
                strncmp( col_def->label, heading, strlen( heading ) ) == 0)
            {
                column_array[i] = col_def->type;
                break;
            }
        }

        if (col_def->type == Column_None)
        {
            fprintf( stderr,
                    "INPUT ERROR: unrecognized column heading '%s'\n",
                    heading );
            exit( EXIT_FAILURE );
        }
    }

    /* default the last entry in the array as a sentinel */
    column_array[i] = Column_None;
}

/*===========================================================================*/

static input_line_p_t read_line( FILE* infile )

/*  Reads the next line from stdin.
**  Returns level -1 on EOF.
*/

{
    char input[MAX_LINE_LEN + 1];
    int input_len;
    char *line;
    char *init_quotes = "";
    column_p_t column;
    char *substitutes = NULL;
    char *options = NULL;
    char *loadif = NULL;
    static unsigned int line_number = 0;

    static char *delimiter; /* delimeter is one character so add one for termination */
    static char *sub_delimiter;
    static logical delimiter_initialised = 0;

    input_line_p_t line_ptr =
        (input_line_p_t) MEM_alloc( sizeof( input_line_t ) );
    line_ptr->child_chain = NULL;
    line_ptr->sibling_chain = NULL;

    /* Keep copies of these strings to prevent problems with */
    /* string literal->char * conversions... */
    if (delimiter_initialised == 0)
    {
        sub_delimiter = (char*)MEM_alloc( 2 * sizeof( char ) );
        delimiter = (char*)MEM_alloc( 2 * sizeof( char ) );
        strcpy( sub_delimiter, DEFAULT_SUB_DELIMITER );
        strcpy( delimiter, DEFAULT_DELIMITER );
        delimiter_initialised = 1;
    }

    /*  Initialize the char* elements of the structure line_ptr with ""
     *  to fix ps_upload crashing on SUN and SGI platforms
     *  PR# 4087018 <Yn> 11/30/99
     */

    line_ptr->item_id = init_quotes;
    line_ptr->key = init_quotes;
    line_ptr->arch_element_id = NULL;
    line_ptr->item_name = init_quotes;
    line_ptr->item_rev_name = init_quotes;
    line_ptr->seq_no = init_quotes;
    line_ptr->occ_name = init_quotes;
    line_ptr->level = -1;
    line_ptr->no_of_occs = 1;
    line_ptr->qty_per_occ = 0;
    line_ptr->sub.no = 0;
    line_ptr->sub.ids = NULL;
    line_ptr->options.no = 0;
    line_ptr->options.ids = NULL;
    line_ptr->loadif.no = 0;
    line_ptr->loadif.ids = NULL;
    line_ptr->bvr = NULLTAG;
    line_ptr->item_revision_id = NULL;
    line_ptr->item_type = init_quotes;
    line_ptr->desc = init_quotes;
    line_ptr->LhRh = init_quotes;

    /*  <PDJ>  24-Feb-1999  PR#751349
    **  Remember to initialize this one too, so we don't barf when there's no
    **  uom column in the input file.
    */
    line_ptr->unit_of_measure = init_quotes;

    /* Get the next line, ignoring comments and empty lines.
    ** Note that a line starting # may be a delimiter/column definition line.
    */
    do
    {
        if (fgets( input, MAX_LINE_LEN + 1, infile ) == NULL)
        {
            line_ptr->level = -1;    /* Got EOF. */
            return line_ptr;
        }
        ++line_number;

        input_len = strlen( input );
        if ( input_len>=1 && input[input_len-1] == '\n')
        {
            input[input_len-1] = '\0';
        }
        /* Coz PC's use 13,10 at the end of lines.... */
        if ( input_len>=2 && input[input_len-2] == '\r')
        {
            input[input_len-2] = '\0';
        }

        /* Check to see if this is a delimiter line */
        if (input[0] == '#')
        {
            /* Skip to first non-whitespace after '#' */
            char* word=input;
            do {
                ++word;
            } while (isspace(*word));

			if (strncmp( word, delim_text, strlen( delim_text ) ) == 0)
            {
                char new_delimiter;

                /* the delimiter is the next non-space character */
                char *p = word + strlen( delim_text );

                while (*++p == ' ')
                {
                    /* intentionally empty */
                }

                new_delimiter = (*p == '\0' ? ' ' : *p);

                /* Make sure the two delimiters are not equal. If they
                ** are, it is very unlikely that further parsing will
                ** produce anything except garbage, so exit.
                */
                if (new_delimiter == *sub_delimiter)
                {
                    fprintf( stderr, "INPUT ERROR: the column delimiter "
                             "cannot be set equal to '%s',the substitutes delimiter "
                             "on line %d\n", sub_delimiter, line_number );

                    exit( EXIT_FAILURE );
                }
                *delimiter = new_delimiter;
            }
            else if (strncmp( word, sub_delim_text,
                              strlen( sub_delim_text ) ) == 0)
            {
                char new_delimiter;

                /* the sub delimiter is the next non-space character */
                char *p = word + strlen( sub_delim_text );

                while (*++p == ' ')
                {
                    /* intentionally empty */
                }

                /* note that if there is no character, the delimiter
                ** is set to ' ', not to the default ';'
                */
                new_delimiter = (*p == '\0' ? ' ' : *p);

                /* Make sure the two delimiters are not equal. If they
                ** are, it is very unlikely that further parsing will
                ** produce anything except garbage, so exit. */
                if (new_delimiter == *delimiter)
                {
                    fprintf( stderr, "INPUT ERROR: the substitutes delimiter "
                             "cannot be set equal to '%s', the column delimiter "
                             "on line %d\n", delimiter, line_number );

                    exit( EXIT_FAILURE );
                }
                *sub_delimiter = new_delimiter;
            }
            else if (strncmp( word, col_text, strlen( col_text ) ) == 0)
            {
                get_columns( column_array, word + strlen( col_text ) + 1);
            }
        }
    } while ( input[0] == '#' || strspn( input, WHITESPACE ) == strlen( input ) );

    line = input;

	printf( "INPUT line :%s: \n ",line);

    for (column = column_array; *column != Column_None; ++column)
    {
        char *token = get_next_token( line, delimiter );

        if ( Column_ArchElementID == *column && strcmp( "Architecture", (item_class!=NULL?item_class:"Item") ) )
            column++; // skip the Architecture Element ID column for non Architecture Items.


	 //   printf( "INPUT line inside :%s: \n ",line);

        switch (*column)
        {
        case Column_Item :
            line_ptr->item_id = token_to_string( token );
            if (strlen( line_ptr->item_id ) == 0)
            {
                fprintf( stderr, "INPUT ERROR: "
                         "item id field is empty on line %d\n", line_number );
                exit( EXIT_FAILURE );
            }
            line_ptr->key = keyFromItemId( line_ptr->item_id );
            break;
        case Column_ArchElementID :
            line_ptr->arch_element_id = token_to_string( token );
            if (strlen( line_ptr->arch_element_id) == 0)
            {
                fprintf( stderr, "INPUT ERROR: "
                         "generic id is empty on line %d\n", line_number );
                exit( EXIT_FAILURE );
            }
            break;
        case Column_Name :
            line_ptr->item_name = token_to_string( token );
			//printf("\n Column name:[%s]",line_ptr->item_name);
            if (strlen( line_ptr->item_name ) == 0)
            {
                fprintf( stderr, "INPUT ERROR: "
                         "name field is empty on line %d\n", line_number );
                exit( EXIT_FAILURE );
            }
            break;
        case Column_Level :
            line_ptr->level = token_to_int( token );
            break;
        case Column_Seq :

            line_ptr->seq_no = token_to_string( token );
			//printf("\n Sequence Number is :[%s]",token);
            break;
        case Column_OccName :
            line_ptr->occ_name = token_to_string( token );
            break;
        case Column_RevName :
            line_ptr->item_rev_name = token_to_string( token );
			printf("\n Sequence Number is :[%s]",line_ptr->item_rev_name);
            break;
        case Column_Occs :
            line_ptr->no_of_occs = token_to_int( token );
            if ( line_ptr->no_of_occs == 0 )
                line_ptr->no_of_occs = 1;
            break;
        case Column_Qty :
            line_ptr->qty_per_occ = token_to_double( token );
            break;
        case Column_Uom :
            line_ptr->unit_of_measure = token_to_string( token );
            break;
        case Column_Sub :
            substitutes = token;
            break;
        case Column_Revision :
            line_ptr->item_revision_id = token_to_string( token );
            break;
        case Column_Options :
            options = token;
            break;
        case Column_LoadIf :
            loadif = token;
            break;
        case Column_Type :
            line_ptr->item_type = token_to_string( token );
            break;
        case Column_Desc :
            line_ptr->desc = token_to_string( token );
            break;
       case Column_LhRh :
            line_ptr->LhRh = token_to_string( token );
   			printf("\n LHRH is :[%s]",line_ptr->LhRh);
			break;

        case Column_None :
        default :
            fprintf (stderr, "INTERNAL ERROR: unknown column identifier\n" );
            break;
        }

        /* get_next_token requires line to be NULL for each subsequent call */
        line = NULL;
    }

	printf("\n Sequence Number is rbb:[%s] \n",line_ptr->item_rev_name);

    if ( strcmp(line_ptr->item_id,"NULLID") == 0 )
    {
        char *id=NULL;
        int retCode = ITK_ok;
        logical mod = TRUE;
        tag_t type_tag = NULLTAG;

		printf("\n debugging in line func 11111\n");

        // PR6099303: Pass in the input item type rather than NULLTAG
        // Default to NULLTAG if no item type specified
        if ( strcmp(line_ptr->item_type,init_quotes) != 0 )
        {
            retCode = TCTYPE_ask_type(line_ptr->item_type, &type_tag);
            if( retCode != ITK_ok)
            {
                fprintf( stderr, "Input Error: the specified item type doesn't exist.\n");
                exit( EXIT_FAILURE );
            }
        }

        retCode = USER_new_item_id(NULLTAG,type_tag,&mod,&id);
        if( retCode != ITK_ok)
        {
            fprintf( stderr, "INPUT ERROR: failed to generate item_id" );
            exit( EXIT_FAILURE );
        }
        else
        {
            //strcat(id,",");
            //strcpy(line_ptr->item_id,id);
            MEM_free(line_ptr->item_id);
            line_ptr->item_id = id;
        }
    }

    if (substitutes != NULL)
    {
        token_to_substitutes( substitutes, &line_ptr->sub, sub_delimiter );
    }

    if (options != NULL)
    {
        token_to_substitutes( options, &line_ptr->options, sub_delimiter );
    }

    if (loadif != NULL)
    {
        token_to_substitutes( loadif, &line_ptr->loadif, sub_delimiter );
    }

    return line_ptr;
}

/*===========================================================================*/

static void print_structure_level( input_line_p_t line, int level )
{
    /*  <PDJ>  29-Nov-2006  PR#5657274
    **  Loop over siblings, rather than recursing, to avoid stack overflow on very wide structures.
    */
    while ( line != NULL )
    {
        print_structure( line, level );
        line = line->sibling_chain;
    }
}

/*===========================================================================*/

static void print_structure( input_line_p_t line, int level )

/*  Debugging. Prints the structure from the specified line downwards
**  Indentation of each line indicates position in child/sibling chains.
*/
{
    if (line != NULL)
    {
        int i;
        char *open_bracket="";
        char *close_bracket="";
        char *element_id="";
        char *itemRevSeq="";

        if ( !strcmp( "Architecture", (item_class!=NULL?item_class:"Item") ) )
        {
            open_bracket = "]";
            element_id = (line->arch_element_id!=NULL?line->arch_element_id:"null");
            close_bracket = "[";
        }

        if ( (strlen( line->key ) == 0) && (strlen( line->item_id ) > 0) )
            line->key = keyFromItemId( line->item_id );

			itemRevSeq = NULL;
			itemRevSeq=(char *) MEM_alloc(32);
			strcpy(itemRevSeq,line->item_revision_id);
			strcat(itemRevSeq,";");
			strcat(itemRevSeq,line->seq_no);

        printf( "%*s%s/%s%s%s%s %s, %d, %s, %d, %lf, %s, %s, %s ",
                level * 2, "",   /* level * 2 spaces */
                line->key,(itemRevSeq!=NULL?itemRevSeq:"<System Assigned RevID>"),
                open_bracket, element_id, close_bracket,
                line->item_name,
                line->level,
                line->seq_no,
                line->no_of_occs,
                line->qty_per_occ,
                line->unit_of_measure,
                line->item_type,
                line->LhRh
            );

        for (i = 0; i < line->sub.no; ++i)
            printf( "; %s", line->sub.ids[i] );

        printf("\n");
        print_structure_level( line->child_chain, level + 1 );
    }
}

/*===========================================================================*/

static void save_structure_level( input_line_p_t line )
{
    /*  <PDJ>  29-Nov-2006  PR#5657274
    **  Loop over siblings, rather than recursing, to avoid stack overflow on very wide structures.
    */
    while ( line != NULL )
    {
        save_structure( line );
        line = line->sibling_chain;
    }
}

/*===========================================================================*/

static void save_structure( input_line_p_t line )

/*  Recurses down the tree saving all the structure we've created.
*/

{
    if (line != NULL)
    {
        if (line->bvr != NULLTAG)
        {
            int ifail = AOM_save( line->bvr );
            CHECK_IFAIL;
        }
        save_structure_level( line->child_chain );
    }
}

/*===========================================================================*/

static void add_to_newstuff( const input_line_p_t line )

/* Adds the given line's Item to the user's newstuff folder
*/
{
    int ifail = ITK_ok;
    tag_t item = NULLTAG;

    if ( (strlen( line->key ) == 0) && (strlen( line->item_id ) > 0) )
        line->key = keyFromItemId( line->item_id );

    // 008104 - MultiFieldKeys project
    {
        tag_t *tags_found = NULL;
        int n_tags_found = 0;
        ifail = ITEM_find_items_by_string(line->key, &n_tags_found, &tags_found);

        if (ifail == ITK_ok)
        {
            if (n_tags_found == 0)
            {
                ifail = ITEM_unable_to_find;
            }
            else if (n_tags_found > 1)
            {
                MEM_free(tags_found);
                ifail = ITEM_duplicate_ids_found;
                EMH_store_initial_error(EMH_severity_error,ifail);
            }
            else
            {
                item = tags_found[0];
                MEM_free(tags_found);
            }
        }
    }

    ifail = ITEM_find_item( line->item_id, &item );
    CHECK_IFAIL;
    if (item != NULLTAG)
    {
        ifail = FL_user_update_newstuff_folder( item );
        if (ifail != 6007)
        {
            CHECK_IFAIL;
        }
    }
}

/*===========================================================================*/

static char* default_null_to_A(char *s)
{
    return (NULL == s ? "A" : default_empty_to_A( s ));
}


/*===========================================================================*/

static char* default_empty_to_A(char *s)
{
    return (NULL == s ? s : ('\0' == s[0] ? "A" : s));
}

/*===========================================================================*/

static void build_structure_level( input_line_p_t  line,  const input_line_p_t  parent,  int  total_lines,
                                   logical  override,  logical  mem_save_mode,  logical  verbose )
{
    /*  <PDJ>   1-Nov-2006
    **  Loop over siblings, rather than recursing, to avoid stack overflow on very wide structures.
    */


    while ( line != NULL )
    {
		printf("\n Going for line :[%s]",line->item_id);
		printf("\n Going for lfrh :[%s]",line->LhRh);
        if ( parent != NULL )
		{
		printf("\n Going for parent :[%s]",parent->item_id);
		printf("\n Going for parent lfrh :[%s]",parent->LhRh);
        }

        build_structure( line, parent, total_lines, override, mem_save_mode, verbose );
       	printf("\n Calling after build_structure");

        line = line->sibling_chain;
    }
}

/*===========================================================================*/

static void build_structure( input_line_p_t  line,  const input_line_p_t  parent,  int  total_lines,
                             logical  override,  logical  mem_save_mode,  logical  verbose )

/*  Function to create the PS structure for this line of the input file,
**  then recurses to create its children. Items and BOMViews are created as
**  required.
*/
{

	int status;
	int org_seq_id_int=0;
	char *org_seq_id=NULL;
	double mat[4][4];
	double * mat1;
	double divInt=1000;
	FILE* mfp=NULL;
	int omat=0;
	int imat=0;
	int clean=0;
	int jmat=0;
	char matrixline[2500];
	char* tempmat1=NULL;
	int rev_seqs;
	char* tempmat=NULL;
	char* intancename=NULL;
	char* intrchanble=NULL;
	char* pattrnStamp=NULL;
	char* TlngChange=NULL;
	char* intancename2=NULL;
	char* RplcPartNumb=NULL;
	char* RplcPartNumbCopy=NULL;
	char* matfilename=NULL;
	char** stringArray=NULL;
	int length=0;
	tag_t tag_occ_name=0;
	tag_t tag_occ_name1=NULLTAG;
	tag_t queryTag = NULLTAG;
	char **qry_values = (char **) MEM_alloc(10 * sizeof(char *));
	int n_entries = 2;
	int resultCount =0;
	int resultCount5 =0;
	char *qry_entries[2] = {"Item ID","Revision"};
	tag_t *parent_rev_qry=NULLTAG;
    int    Item_ID = 0;
    int    intrchanbleint = 0;
    int    pattrnStampint = 0;
    int    TlngChangeint = 0;
    int    RplcPartNumbint = 0;
	char    *Item_ID_str =NULL;
	char type_name[TCTYPE_name_size_c+1] ;
	char * line_name;
	char * parent_name ;
    char *itemRevSeq="";
	//adk
	 tag_t top_line1;
	 tag_t bom_window=NULLTAG;
	 tag_t rule;
	//adk

	if (line != NULL)
    {
        int ifail = 0;
        tag_t item = NULLTAG;
        tag_t rev=NULLTAG;
        tag_t rev_org=NULLTAG;
        tag_t unit_of_measure=NULLTAG;

        if ( verbose )
        {
            static int  line_counter = 0;
            printf( "build_structure processing line #%d of %d\n", ++line_counter, total_lines );
        }

        /*  If the specified item does not exist then create it.
         */
        if ( (strlen( line->key ) == 0) && (strlen( line->item_id ) > 0) )
            line->key = keyFromItemId( line->item_id );

        // 008104 - MultiFieldKeys project
        {
            tag_t *tags_found = NULL;
            int n_tags_found = 0;
            ifail = ITEM_find_items_by_string(line->key, &n_tags_found, &tags_found);

            if ( ifail == ITK_ok && n_tags_found > 0 )
            {
                if (n_tags_found > 1)
                {
                    MEM_free(tags_found);
                    ifail = ITEM_duplicate_ids_found;
                    EMH_store_initial_error(EMH_severity_error,ifail);
                }
                else
                {
                    item = tags_found[0];
                    MEM_free(tags_found);
                }
            }
        }

        CHECK_IFAIL;

        /* Get a UNIT_OF_MEASURE tag */
        /*if (strlen (line->unit_of_measure) > 0)
        {
            ifail = UOM_find_by_symbol( line->unit_of_measure, &unit_of_measure );
            CHECK_IFAIL;
            if (unit_of_measure == NULLTAG)
            {
                fprintf(stderr, "ERROR: Unit of Measure %s is invalid\n",line->unit_of_measure);
                ITK_exit_module( true );
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            unit_of_measure = NULLTAG;
        }*/

        if (item == NULLTAG)
        {
            printf("Creating new item %s\n",line->item_id);
            if ( NULL == line->item_type || !strcmp( "", (line->item_type!=NULL?line->item_type:"") ) )
			{
				 line->item_type = (char*) MEM_alloc(( strlen(default_item_type) + 1) * sizeof(char));
			     strcpy(line->item_type,default_item_type);
			}

			itemRevSeq = NULL;
			itemRevSeq=(char *) MEM_alloc(32);
			strcpy(itemRevSeq,line->item_revision_id);
			strcat(itemRevSeq,";");
			strcat(itemRevSeq,line->seq_no);

            if ( !strcmp( "Architecture", (item_class!=NULL?item_class:"Item") ) )
            {
                ifail = ARCH_create_arch( line->item_id,
                                          default_empty_to_A(itemRevSeq),
                                          line->item_type,
                                          line->item_name,
                                          line->arch_element_id,
                                          my_name,
                                          &item,
                                          &rev );
            }
            else
            {
                ifail = ITEM_create_item( line->item_id,
                                          line->item_name,
                                          line->item_type,
                                          default_empty_to_A(itemRevSeq),
                                          &item,
                                          &rev );
            }
            CHECK_IFAIL;

            //ifail = ITEM_set_unit_of_measure( item, unit_of_measure);

			printf("\n Description: [%s]",line->desc);
		//	printf("\n t5_LeftRh [%s]",line->t5_LeftRh);

            //ifail =	ITEM_set_description( item, line->desc);
            ifail =	ITEM_set_rev_description( rev, line->desc);
			CHECK_IFAIL;
			//ifail = AOM_set_value_string(rev,"gov_classification",line->seq_no);
            //CHECK_IFAIL;
            /* Need to save item - else UOM will not be set */
            ifail = AOM_save(item);
            ifail = AOM_save(rev);
			printf("\n Finished creating item");
        }
        else
        {
            /*  <JB>  27-Sep-1995
            **        This Item is already in the database.
            **        Ensure that the name of the existing Item matches that
            **        of the newly read Item.
            **        If not, flag an error, and just continue.
            */

            char name[WSO_name_size_c+1];
            char type[ITEM_type_size_c+1];
            tag_t old_unit_of_measure;
            printf("Found existing item %s\n",line->item_id);
            printf("item_revision_id %s\n",line->item_revision_id);

            /* <TB> 15-Jan-2001
            ** Create revision on existing item, if nonexistent.
            */

			itemRevSeq = NULL;
			itemRevSeq=(char *) MEM_alloc(32);
			strcpy(itemRevSeq,line->item_revision_id);
			strcat(itemRevSeq,";");
			strcat(itemRevSeq,line->seq_no);
			printf("\n itemRevSeq_11 --->%s",itemRevSeq);

			ifail = ITEM_find_revision(item,default_null_to_A(itemRevSeq),&rev);
            CHECK_IFAIL;
            if ( rev == NULLTAG )
            {
				printf("\n Not Found the Item Revision as well   --->%s",itemRevSeq);
                ifail = ITEM_create_rev(item,default_empty_to_A(itemRevSeq),&rev);
				 ifail = AOM_save(rev);
				CHECK_IFAIL;
				//ifail = AOM_set_value_string(rev,"gov_classification",line->seq_no);
            }
			ifail = AOM_ask_value_int(rev,"sequence_id",&rev_seqs);
			printf("\n its revision exact sequence is --->%d",rev_seqs);


            ifail = ITEM_ask_name (item, name);
            CHECK_IFAIL;

            if (strcmp (name, line->item_name))
            {
                fprintf( stderr, "WARNING: Name clash : Item with ID '%s' "
                         "has name '%s' in DB, but now given as '%s'",
                         line->item_id, name, line->item_name );

                if (override)
                {
                    ifail = ITEM_set_name (item, line->item_name);
                    //Need to save the item otherwise item name can not set
                    ifail = AOM_save(item);
                    CHECK_IFAIL;

                    fprintf( stderr, "; changed.\n");
                }
                else
                {
                    fprintf( stderr, "; ignored.\n");
                }
            }

            if ( strlen( line->item_rev_name ) > 0 )
            {
                ifail = ITEM_ask_rev_name (rev, name);
                CHECK_IFAIL;

                if (strcmp (name, line->item_rev_name))
                {
                    fprintf( stderr, "WARNING: Name clash : Item with ID '%s' "
                             "has revision name '%s' in DB, but now given as '%s'",
                             line->item_id, name, line->item_rev_name );

                    if (override)
                    {
                        ifail = ITEM_set_rev_name (rev, line->item_rev_name);
                        //Need to save the item rev otherwise item rev name can not set
                        ifail = AOM_save(rev);
                        CHECK_IFAIL;

                        fprintf( stderr, "; changed.\n");
                    }
                    else
                    {
                        fprintf( stderr, "; ignored.\n");
                    }
                }
            }

            /*  <NKM> 19-Aug-1998
            **        Also need to check item has same unit_of_measure as existing item
            **        Same rule as above applies
            */

            /*ifail = ITEM_ask_unit_of_measure (item, &old_unit_of_measure);
            CHECK_IFAIL;
            if (unit_of_measure != old_unit_of_measure)
            {
                fprintf( stderr, "WARNING: Unit of Measure: Item with ID '%s'\n"
                         "has an existing value which is different to the one given.\n"
                         "Ignoring new unit of measure.",
                         line->item_id);

             }*/

            /* <TB> 15-Jan-2001
            ** Check type matches, if specified.
            */

            ifail = ITEM_ask_type (item, type);
            CHECK_IFAIL;
            if ( (line->item_type[0] != '\0') && strcmp (type, line->item_type) )
            {
                fprintf( stderr, "WARNING: Type clash : Item with ID '%s' "
                         "has type '%s' in DB, but input file specified '%s'.\n"
                         "Ignoring new type.\n",
                         line->item_id, type, line->item_type );
            }

        }

		printf("\n Going for structure creation");
        /* Options on this item revision (not occurrences)
         */

       		printf("\n Condition line->LhRh :%s:",line->LhRh);fflush(stdout);
			if ( !strcmp(line->LhRh, "WeldedParts"))
			{
   					 printf("\n Inside line->LhRh :%s:",line->LhRh);fflush(stdout);

					 tag_t *tags_line;
                     tag_t line_item;
                     tag_t parent_item =NULLTAG;
                     int n_tags_line = 0;

                 	printf("\n Inside line->key :%s:",line->key);fflush(stdout);

					// ifail = ITEM_find_items_by_string(line->key, &n_tags_line, &tags_line);
					 ifail = ITEM_find_items_by_string(line->key, &n_tags_line, &tags_line);

					 printf("\n n_tags_line :%d:",n_tags_line);fflush(stdout);

			         tag_t parentrev=NULLTAG;
					 tag_t *parent_tags_found= NULL;;
                     tag_t Prnt_item ;

					itemRevSeq = NULL;
					itemRevSeq=(char *) MEM_alloc(32);
					strcpy(itemRevSeq,parent->item_revision_id);
					strcat(itemRevSeq,";");
					strcat(itemRevSeq,parent->seq_no);
					printf("\n Inside itemRevSeq :%s:",itemRevSeq);fflush(stdout);
					printf("\n Inside parent->key :%s:",parent->key);fflush(stdout);

					 tag_t *tags_parent;
                     int n_tags_parent = 0;

					printf("\n Inside parent->key :%s:",parent->key);fflush(stdout);

					 ifail = ITEM_find_items_by_string(parent->key, &n_tags_parent, &tags_parent);

					Prnt_item = tags_parent[0];

					printf("\n Inside parent->key :%s:",parent->key);fflush(stdout);

					ifail = ITEM_find_revision(Prnt_item,default_null_to_A(itemRevSeq),&parent_item);
					CHECK_IFAIL;


                      line_item = tags_line[0];
			//		  parent_item = parentrev[0];

					 printf("\n n_tags_parent :%d:",n_tags_parent);fflush(stdout);
                     if(n_tags_line>0  &&    parent_item != NULLTAG )
				     {
						 int result = 0;
						 tag_t relation_type, relation;
                         tag_t Fndrelation ;

						 result = GRM_find_relation_type("T5_HasWeldParts", &relation_type);
						 printf("\n GRM_find_relation_type ---->%d",result);fflush(stdout);
						 if(relation_type)
							{
								ITK_CALL(TCTYPE_ask_name(relation_type,type_name));
								printf("\n Type Name:%s ..............",type_name);fflush(stdout);
							}
		                   if(line_item)
							{

								ITK_CALL(AOM_ask_value_string(line_item,"current_name",&line_name));
								printf("\n line_name:%s ..............",line_name);fflush(stdout);

							}

						   result = GRM_find_relation  ( parent_item, line_item, relation_type ,&Fndrelation);
                           printf("\n Fndrelation ---->%d",result);fflush(stdout);
                           if(Fndrelation == NULL)
						   {
							   printf("\n No Relations" );fflush(stdout);
							   result = GRM_create_relation(parent_item, line_item, relation_type, NULLTAG, &relation);
							   if(relation)
								 {
									 result = GRM_save_relation(relation);
									 printf("\n GRM_save_relation ---->%d",result);fflush(stdout);

								 }
						   }else
						   {
						       printf("\n There Relations" );fflush(stdout);

						   }
						 //  exit(0);
						// goto EXIT_1 ;
						return ;
						 printf("\n GRM_create_relation done ---->%d",result);fflush(stdout);

					 }
			}
		   else	if ( !strcmp(line->LhRh, "LH CATIA") || !strcmp(line->LhRh, "RH CATIA") || !strcmp(line->LhRh, "LH PROE") || !strcmp(line->LhRh, "RH PROE"))
			{
					printf("\n Inside line->LhRh :%s:",line->LhRh);fflush(stdout);

				     tag_t *tags_line;
                     tag_t line_item;
                     tag_t parent_item;
                     int n_tags_line = 0;

                 	printf("\n Inside line->key :%s:",line->key);fflush(stdout);

					// ifail = ITEM_find_items_by_string(line->key, &n_tags_line, &tags_line);
					 ifail = ITEM_find_items_by_string(line->key, &n_tags_line, &tags_line);

					 printf("\n n_tags_line :%d:",n_tags_line);fflush(stdout);

					 tag_t *tags_parent;
                     int n_tags_parent = 0;

					printf("\n Inside parent->key :%s:",parent->key);fflush(stdout);

					 ifail = ITEM_find_items_by_string(parent->key, &n_tags_parent, &tags_parent);


                      line_item = tags_line[0];
					  parent_item = tags_parent[0];

					 printf("\n n_tags_parent :%d:",n_tags_parent);fflush(stdout);
                     if(n_tags_line>0 && n_tags_parent> 0 )
				     {
						 int result = 0;
						 tag_t relation_type, relation;
                         tag_t Fndrelation ;

						 result = GRM_find_relation_type("T5_LRReln", &relation_type);
						 printf("\n GRM_find_relation_type ---->%d",result);fflush(stdout);
						 if(relation_type)
							{
								ITK_CALL(TCTYPE_ask_name(relation_type,type_name));
								printf("\n Type Name:%s ..............",type_name);fflush(stdout);
							}
		                   if(line_item)
							{

								ITK_CALL(AOM_ask_value_string(line_item,"current_name",&line_name));
								printf("\n line_name:%s ..............",line_name);fflush(stdout);

							}

							if(parent_item)
							{

								ITK_CALL(AOM_ask_value_string(parent_item,"current_name",&parent_name));
								printf("\n parent_name:%s ..............",parent_name);fflush(stdout);

								ITK_CALL ( AOM_lock(parent_item));
								printf("\n  Inside Condition line->LhRh :%s:",line->LhRh);fflush(stdout);
								ITK_CALL ( AOM_set_value_string(parent_item,"t5_LeftRh",line->LhRh));
								printf("\n  After set line->LhRh :%s:",line->LhRh);fflush(stdout);

								if(parent_item)
								 {
									 result = ITK_CALL(AOM_save(parent_item));;
									 ITK_CALL ( AOM_unlock(parent_item));
									 printf("\n GRM_save_relation123 ---->%d",result);fflush(stdout);
								 }
							}

						   result = GRM_find_relation  ( parent_item, line_item, relation_type ,&Fndrelation);
                           printf("\n Fndrelation ---->%d",result);fflush(stdout);
                           if(Fndrelation == NULL)
						   {
							   printf("\n No Relations" );fflush(stdout);
							   result = GRM_create_relation(parent_item, line_item, relation_type, NULLTAG, &relation);
							   if(relation)
								 {
									 result = GRM_save_relation(relation);
									 printf("\n GRM_save_relation ---->%d",result);fflush(stdout);

								 }
						   }else
						   {
						       printf("\n There Relations" );fflush(stdout);

						   }
						 //  exit(0);
						// goto EXIT_1 ;
						return ;
						 printf("\n GRM_create_relation done ---->%d",result);fflush(stdout);

					 }
			}
			else
			{

        if (line->options.no > 0)
        {
			printf("\n I am going inside options");
            int opt;
            for ( opt = 0 ; opt < line->options.no; ++opt )
            {
                tag_t option;
                tag_t option_rev;
                tag_t ve;
                tag_t veb;

                {
                    /* Check that the option doesn't already exist - ITK doesn't seem to do that
                    ** for you.
                    */

                    tag_t window;
                    tag_t top_bomline;
                    tag_t option;
                    tag_t option_rev;

                    ifail = BOM_create_window( &window );
                    CHECK_IFAIL;

                    /* TB::Code review - Default revision rule. Should perhaps be specified? */
                    ifail = BOM_set_window_top_line( window,
                                                     NULLTAG,
                                                     rev,
                                                     NULLTAG,
                                                     &top_bomline );
                    CHECK_IFAIL;

                    ifail = BOM_window_find_option( window,
                                                    item,
                                                    line->options.ids[opt],
                                                    &option,
                                                    &option_rev );

                    if ( ifail != ITK_ok )
                    {
                        int  i, n_errors;
                        const int *severities;
                        const int *ifails;
                        const char **messages;
                        int ignore = 0;

                        EMH_ask_errors( &n_errors, &severities, &ifails, &messages );

                        for (i = 0; i < n_errors; i++)
                        {
                            if( ifails[i] == BOM_variant_cant_find )
                            {
                                ignore = 1;
                                break;
                            }
                        }
                        if( !ignore )
                        {
                            HANDLE_IFAIL;
                        }
                        else
                        {
                            ifail = BOM_variant_cant_find;
                        }
                    }
                }

                if ( ifail == BOM_variant_cant_find )
                {
                    if (    ITK_ok != BOM_new_option( rev,
                                                      line->options.ids[opt],
                                                      "",
                                                      BOM_option_mode_enum,
                                                      &option,
                                                      &option_rev,
                                                      &ve,
                                                      &veb )
                         || ITK_ok != AOM_save(ve)
                         || ITK_ok != AOM_save(veb)
                        )
                    {
                        HANDLE_IFAIL;
                    }

                    /* Values until the first blank or end-of-list are values for this option */
                    for ( ++opt ; opt < line->options.no && ( *(line->options.ids[opt]) != '\0' ) ; ++opt )
                    {
                        int thrown_away_index;

                        ifail = BOM_add_option_rev_value( option_rev,
                                                          line->options.ids[opt],
                                                          &thrown_away_index );
                        CHECK_IFAIL;
                    }

                    {
                        logical save_required;

                        if (    ITK_ok != AOM_save(option_rev)
                                || ITK_ok != AOM_save(option)
                                || ITK_ok != POM_save_required( rev, &save_required )
                                || ( save_required
                                     && ( ITK_ok != AOM_save(rev) ) ) )
                        {
                            HANDLE_IFAIL;
                        }
                    }
                }
                else
                {
                    fprintf( stderr, "WARNING: "
                             "Option '%s' already exists on item '%s'. Ignoring this option\n",
                             line->options.ids[opt], line->item_id);
                    for ( ++opt ;
                          opt < line->options.no && ( *(line->options.ids[opt]) != '\0' ) ;
                          ++opt )
                    {
                        /* empty */
                    }
                }
            }
        }




        /*  If parent *is* NULL then this is the top line, so we wouldn't want
        **  to create an occurrence or put loadif clauses on it.
        */
        if (parent != NULL)
        {
            tag_t *occs;
            int n_occs;
	        char * parent_name ;
	        char * occ_name ;


			printf("\n Inside Going for line :[%s]",line->item_id);
			printf("\n Inside for  line lfrh :[%s]",line->LhRh);

			printf("\n Inside for parent :[%s]",parent->item_id);
			printf("\n Inside for  parent lfrh :[%s]",parent->LhRh);

			if (parent->bvr == NULLTAG)
            {
                /* <PGW> 10-Feb-1997
                   we haven't got a BOMViewRevision for the parent
                   yet, so find either find an existing bvr and use
                   that, or create a fresh one */

                tag_t  bv, parent_item, parent_rev;
                tag_t *bvs, *bvrs;
                int    bv_count, bvr_count;

                if ( (strlen( parent->key ) == 0) && (strlen( parent->item_id ) > 0) )
                    parent->key = keyFromItemId( parent->item_id );

                // 008104 - MultiFieldKeys project
                {
                    tag_t *tags_found = NULL;
                    int n_tags_found = 0;
                    ifail = ITEM_find_items_by_string(parent->key, &n_tags_found, &tags_found);

                    if (ifail == ITK_ok)
                    {
                        if (n_tags_found == 0)
                        {
                            ifail = ITEM_unable_to_find;
                        }
                        else if (n_tags_found > 1)
                        {
                            MEM_free(tags_found);
                            ifail = ITEM_duplicate_ids_found;
                            EMH_store_initial_error(EMH_severity_error,ifail);
                        }
                        else
                        {
                            parent_item = tags_found[0];
                            MEM_free(tags_found);
                        }
                    }
                }

                CHECK_IFAIL;

//				itemRevSeq = NULL;
//				itemRevSeq=(char *) MEM_alloc(32);
//				strcpy(itemRevSeq,line->item_revision_id);
//				strcat(itemRevSeq,";");
//				strcat(itemRevSeq,line->seq_no);
//				printf("\n itemRevSeq_22  --->%s",itemRevSeq);
//
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
			itemRevSeq = NULL;
			itemRevSeq=(char *) MEM_alloc(50);
			strcpy(itemRevSeq,parent->item_revision_id);
			strcat(itemRevSeq,"*");
			strcat(itemRevSeq,parent->seq_no);
			//strcat(itemRevSeq,"\0");
			//printf("\nitemRevSeq concated is --->%s\n\n",itemRevSeq);

			qry_values[0] = parent->item_id ;
			qry_values[1] = itemRevSeq ;

			printf("  2.parent->item_id :%s: \n ", parent->item_id);fflush(stdout);
			printf("  2.parent->item_revision_id [%s] \n ", itemRevSeq);fflush(stdout);

			if(QRY_execute(queryTag, n_entries, qry_entries, qry_values, &resultCount, &parent_rev_qry));

			printf(" \n resultCount :%d:", resultCount); fflush(stdout);
			parent_rev = parent_rev_qry[0];



                // 008104 - MultiFieldKeys project
     //           ifail = ITEM_find_revision( parent_item, default_null_to_A(itemRevSeq), &parent_rev );

                CHECK_IFAIL;
                if ( parent_rev == NULLTAG )
                {
                   ifail = ITEM_ask_latest_rev(parent_item,&parent_rev);
                   CHECK_IFAIL;
                }


                ifail = ITEM_list_bom_views( parent_item, &bv_count, &bvs);
                CHECK_IFAIL;

                if (bv_count)
                {
                    bv = bvs[0];
                    MEM_free(bvs);
                }
                else
                {
                    /*  <PDJ>  27-Sep-1995
                    **  Pass view type as null tag, so it will use the default
                    **  view type preference.
                    */

                    ifail = PS_create_bom_view( NULLTAG, "", "", parent_item,
                                                &bv );
                    CHECK_IFAIL;

                    ifail = AOM_save( bv );
                    CHECK_IFAIL;

                    ifail = AOM_save( parent_item );
                    CHECK_IFAIL;

                    ifail = AOM_unlock( parent_item );
                    CHECK_IFAIL;
                }

                /*  <PGW> 10-Feb-1997
                    Find existing an existing bvr and reconstruct it,
                    if necessary. Note that this code picks up the
                    first bvr in the array with out any thought to
                    viewtype etc.

                    else

                    If there is no existing bvr, create one with
                    default viewtype.
                */

                ifail = ITEM_rev_list_bom_view_revs( parent_rev, &bvr_count, &bvrs);
                CHECK_IFAIL;
				printf("\n itemRevSeq_22  --->%s",itemRevSeq);
				printf("\n bvr_count  --->[%d]",bvr_count);

                if (bvr_count)
                {
                    fprintf( stderr, "WARNING: "
                             "item %s has existing bvr; ",
                             parent->item_id );

                    if (override)
                    {
                        int i;


                        fprintf( stderr, "replacing existing BOM.\n" );
                        parent->bvr = bvrs[0];
                        MEM_free(bvrs);

                        ifail = AOM_lock( parent->bvr );
                        CHECK_IFAIL;

                        ifail = PS_list_occurrences_of_bvr( parent->bvr,
                                                            &n_occs, &occs );
                        CHECK_IFAIL;

                        for (i = 0; i<n_occs; i++)
                        {
                            ifail = PS_delete_occurrence( parent->bvr, occs[i] );
                            CHECK_IFAIL;
                        }

                        ifail = AOM_save( parent->bvr );
                        CHECK_IFAIL;

                        ifail = AOM_unlock( parent -> bvr );
                        CHECK_IFAIL;

                        MEM_free(occs);
                    }
                    else
                    {
                        fprintf( stderr, "skipping.\n" );
                        MEM_free(bvrs);
                        parent->child_chain = NULL;
                        return;
                    }
                }
                else
                {
                    ifail = PS_create_bvr( bv, "", "", false, parent_rev,
                                           &(parent->bvr) );
                    CHECK_IFAIL;

                    ifail = AOM_save( parent->bvr );
                    CHECK_IFAIL;

                    ifail = AOM_save( parent_rev );
                    CHECK_IFAIL;

                    ifail = AOM_unlock( parent_rev );
                    CHECK_IFAIL;
                }
                if ( bv != NULLTAG )
                {
                    ifail = AOM_unload( bv );
                    CHECK_IFAIL;
                }
            }

            /*  Create an occurrence to represent the data on this line.
             */

            ifail = PS_create_occurrences( parent->bvr, item, NULLTAG,
                                           line->no_of_occs, &occs );
            CHECK_IFAIL;

			printf("\n   line->no_of_occs[%d]", line->no_of_occs);

			matfilename=(char *) MEM_alloc(300);
			strcpy(matfilename,parent->item_id);
			strcat(matfilename,".");
			strcat(matfilename,line->item_id);
			strcat(matfilename,".mat.txt");
			printf("\n Printing Matrix:[%s][%s][%s]",line->item_id,parent->item_id,matfilename);
			mfp=fopen(matfilename,"r");
			omat=0;
			if(mfp==NULL)
			{
				printf("\n Could not open file");fflush(stdout);
			}else
			{
				while(fgets(matrixline,2500,mfp)!=NULL)
				{
					fputs(matrixline,stdout);
					tempmat1=strtok(matrixline,",");

                    printf("\n matrixline [%s]",tempmat1);fflush(stdout);

					for(imat=0;imat<4;imat++)
					{
						for(jmat=0;jmat<4;jmat++)
						{
							if(imat==0 && jmat==0)
							{
								mat[imat][jmat]=strtod(tempmat1,NULL);
							}else
							{
								tempmat=strtok(NULL,",");
								mat[imat][jmat]=strtod(tempmat,NULL);
							}
						}
					}

					intancename2= (char *) MEM_alloc( 200 * sizeof(char) );
					intancename=strtok(NULL,",");
					printf("\n Instance name :[%s]\n",intancename);
					for(clean=0;clean<200;clean++)
					{
						intancename2[clean]=NULL;
					}

					for(imat=0;imat<3;imat++)
					{
						mat[3][imat]=(double)(mat[3][imat]/divInt);
					}
					//mat1=&mat;
					if(omat<line->no_of_occs)
					{
						printf("\n Setting matrix \n\n");

						//comment here.....
						for(imat=0;imat<4;imat++)
						{
							for(jmat=0;jmat<4;jmat++)
							{
								printf("%10lf,",mat[imat][jmat]);
							}
							printf("\n");
						}

						ifail = PS_set_plmxml_transform(parent->bvr, occs[omat], mat);
     					length=strlen(intancename)-1;
						strncpy (intancename2,intancename,length);

						ifail = PS_set_occurrence_name(parent->bvr, occs[omat], intancename2);
						printf("\n ifail---> [%d] \n\n",ifail);
//
				//adk	
				ifail = BOM_create_window( &bom_window );
				ifail = CFM_find( "Latest Working", &rule );
				ifail = BOM_set_window_config_rule( bom_window, rule );	
				ifail = BOM_set_window_pack_all(bom_window, TRUE);
				//ITK_CALL(BOM_set_window_top_line( bom_window , item_tag, NULLTAG, NULLTAG, &top_line ));
				ifail = BOM_set_window_top_line( bom_window , NULLTAG, rev, NULLTAG, &top_line1 );
				printf( " adk BOM_set_window_top_line..\n"); fflush(stdout);

				//adk
					ifail = BOM_line_look_up_attribute ("bl_item_item_id",&Item_ID);
					ifail = BOM_line_ask_attribute_string(top_line1, Item_ID, &Item_ID_str);
					printf("\n1] adk22 Item_ID_str =%s\n",Item_ID_str);


					/*
					BOM_line_set_attribute_string	(	tag_t 	bom_line,int 	attribute,const char * 	value)	
					*/
//


					intrchanble = strtok(NULL,",");
					printf("\n adk22 intrchanble :[%s]\n",intrchanble );
					//bl_Design Revision_t5_Interchangeable
					if((strcmp(intrchanble,"X")==0)||(strcmp(intrchanble,"Y")==0)||(strcmp(intrchanble,"N")==0)||(strcmp(intrchanble,"A")==0)||(strcmp(intrchanble,"D")==0))
						{
						ifail = BOM_line_look_up_attribute ("bl_Design Revision_t5_Interchangeable",&intrchanbleint);
						ifail = BOM_line_set_attribute_string(top_line1,intrchanbleint,intrchanble);
						}


					pattrnStamp = strtok(NULL,",");
					printf("\n adk22 pattrnStamp :[%s]\n",pattrnStamp );
					//bl_Design Revision_t5_PatternStamping
					ifail = BOM_line_look_up_attribute ("bl_Design Revision_t5_PatternStamping",&pattrnStampint);
					if(strcmp(pattrnStamp,"1")==0)
						{
						ifail = BOM_line_set_attribute_string(top_line1,pattrnStampint,"YES");
						}
					else if(strcmp(pattrnStamp,"2")==0)
						{
						ifail = BOM_line_set_attribute_string(top_line1,pattrnStampint,"NO");
						}
					else if(strcmp(pattrnStamp,"3")==0)
						{
						ifail = BOM_line_set_attribute_string(top_line1,pattrnStampint,"NA");
						}

					TlngChange = strtok(NULL,",");
					printf("\n adk22 TlngChange :[%s]\n",TlngChange );
					//bl_Design Revision_t5_ToolingChange
					ifail = BOM_line_look_up_attribute ("bl_Design Revision_t5_ToolingChange",&TlngChangeint);
					//ifail = BOM_line_set_attribute_string(top_line1,TlngChangeint,TlngChange);
					if(strcmp(TlngChange,"1")==0)
						{
						ifail = BOM_line_set_attribute_string(top_line1,TlngChangeint,"YES");
						}
					else if(strcmp(TlngChange,"2")==0)
						{
						ifail = BOM_line_set_attribute_string(top_line1,TlngChangeint,"NO");
						}
					else if(strcmp(TlngChange,"3")==0)
						{
						ifail = BOM_line_set_attribute_string(top_line1,TlngChangeint,"NA");
						}

					RplcPartNumb = strtok(NULL,",");
					printf("\n adk22 RplcPartNumb :[%s]\n",RplcPartNumb);
				ifail =	STRNG_replace_str(RplcPartNumb,"\n","",&RplcPartNumbCopy);
				printf("\n adk22 RplcPartNumbCopy :[%s]\n",RplcPartNumbCopy);
					//bl_Design Revision_t5_ReplacesPartNumber
					ifail = BOM_line_look_up_attribute ("bl_Design Revision_t5_ReplacesPartNumber",&RplcPartNumbint);
					if(RplcPartNumbCopy != NULL)
						{
						ifail = BOM_line_set_attribute_string(top_line1,RplcPartNumbint,RplcPartNumbCopy);
						}
/*
						//ifail =  PS_set_occurrence_note_text(parent->bvr, occs[omat],"catiaOccurrenceName",intancename2);
						ifail =  PS_set_occurrence_note_text(parent->bvr, occs[omat],"catiaOccurrenceName",intancename2);
						printf("\n ifail---> [%d] \n\n",ifail);
						ifail = PS_find_note_type("catiaOccurrenceName",&tag_occ_name);
						printf("\n ifail---> [%d] \n\n",ifail);
						ifail = PS_set_occurrence_note_text(parent->bvr, occs[omat],tag_occ_name,intancename2);
						printf("\n ifail---> [%d] \n\n",ifail);

						//ifail = AOM_set_value_string(occs[omat],"catiaOccurrenceName", intancename);
*/
						printf("\n Omat is[%d]",omat);

						omat++;
					}else
					{
						break;
					}

					MEM_free(intancename2);intancename2=NULL;

				}
				fclose(mfp);
				mfp=NULL;
			}

		    printf("\n Out side condition \n");
            if (line->qty_per_occ > 0)
            {
                int i;
                for (i = 0; i < line->no_of_occs; i++)
                {
                    double qty = (double)(line->qty_per_occ);
					printf("\n Quantity isssss is   $$[%g]\n\n",qty);
                    ifail = PS_set_occurrence_qty( parent->bvr, occs[i], 1 );
                    CHECK_IFAIL;
                }
            }

			//printf("\n After setting qty for occ");fflush(stdout);

            if (strlen( line->seq_no ) > (size_t) 0)
            {
                int i;
                for (i = 0; i < line->no_of_occs; i++)
                {
                    ifail = PS_set_seq_no( parent->bvr, occs[i],
                                           line->seq_no );
                    CHECK_IFAIL;
                }
            }

			//printf("\n After setting qty for occ ...1...");fflush(stdout);

            if (strlen( line->occ_name ) > (size_t) 0)
            {
                int i;
                for (i = 0; i < line->no_of_occs; i++)
                {
                    ifail = PS_set_occurrence_name( parent->bvr, occs[i],
                                                    line->occ_name );
                    CHECK_IFAIL;
                }
            }

			//printf("\n After setting qty for occ....1....2");fflush(stdout);

            /* If there are any substitutes, add them for *each* occurrence.
             */
            if (line->sub.no > 0)
            {
				//printf("\n After setting qty for occ....1....2...a");fflush(stdout);
                int sub;
                char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
                char **values = (char **) MEM_alloc(1 * sizeof(char *));
                tag_t *tags_found = NULL;
                int n_tags_found= 0;
                attrs[0] ="item_id";

                for (sub = 0; sub < line->sub.no; ++sub)
                {
                    tag_t item = NULLTAG;
                    int i;

                    // 008104 - MultiFieldKeys project
                    {
                        values[0] = (char *)line->sub.ids[sub];
                        tags_found = NULL;
                        n_tags_found= 0;

                        ifail = ITEM_find_items_by_key_attributes(1, attrs, values,
                            &n_tags_found, &tags_found);

                        if (ifail == ITK_ok)
                        {
                            if (n_tags_found == 0)
                            {
                                ifail = ITEM_unable_to_find;
                            }
                            else if (n_tags_found > 1)
                            {
                                MEM_free(tags_found);
                                ifail = ITEM_duplicate_ids_found;
                                EMH_store_initial_error(EMH_severity_error,ifail);
                            }
                            else
                            {
                                item = tags_found[0];
                                MEM_free(tags_found);
                            }
                        }
                    }

                    NEW_CHECK_IFAIL;
                    if (item == NULLTAG)
                    {
                        fprintf( stderr, "WARNING: "
                                 "specified substitute '%s' does not exist\n",
                                 line->sub.ids[sub] );
                    }

                    for (i = 0; i < line->no_of_occs; ++i)
                    {
                        ifail = PS_add_substitute( parent->bvr, occs[i],
                                                  item, NULLTAG );
                        NEW_CHECK_IFAIL;
                    }
                }
                MEM_free(attrs);
                MEM_free(values);
            }

            /* If there are any loadifs, add them for *each* occurrence
             */
            /*  loadif: id, [rev. ] option, ==, value */
            if ( 4 <= line->loadif.no && line->loadif.no <= 5 )
            {
				//printf("\n After setting qty for occ....1....2....b");fflush(stdout);
                char **par = line->loadif.ids;
                int ix=0;
                char *option_item_s     = par[ix++];
                char *option_s          = par[ix++];
                char *option_eq_s       = par[ix++];
                char *option_val_s      = par[ix++];

                tag_t option;
                tag_t option_rev;
                tag_t option_item;
                tag_t option_item_rev;
                tag_t top_bomline;

                tag_t window = NULLTAG;

                /* clause_list is transient: it's the object behind the expression editor
                 *  window
                 */
                tag_t clause_list = NULLTAG;


                if ( !strcmp(option_eq_s, "==") && !strcmp(option_eq_s, "!=") )
                {
                    fprintf( stderr, "WARNING: "
                             "Equality operator '%s' unrecognised. Interpreted as '=='\n",
                             option_eq_s);
                    option_eq_s = "==";
                }

               // find the latest revision from the item whcih is used for get the option rev

                // 008104 - MultiFieldKeys project
                {
                    char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
                    char **values = (char **) MEM_alloc(1 * sizeof(char *));
                    tag_t *tags_found = NULL;
                    int n_tags_found= 0;
                    attrs[0] ="item_id";
                    values[0] = (char *)option_item_s;

                    ifail = ITEM_find_items_by_key_attributes (1, attrs, values,
                         &n_tags_found, &tags_found);
                    MEM_free(attrs);
                    MEM_free(values);

                    if (ifail == ITK_ok)
                    {
                        if (n_tags_found == 0)
                        {
                            ifail = ITEM_unable_to_find;
                        }
                        else if (n_tags_found > 1)
                        {
                            MEM_free(tags_found);
                            ifail = ITEM_duplicate_ids_found;
                            EMH_store_initial_error(EMH_severity_error,ifail);
                        }
                        else
                        {
                            option_item = tags_found[0];
                            MEM_free(tags_found);
                        }
                    }
                }
                CHECK_IFAIL;

                ifail = ITEM_ask_latest_rev(option_item, &option_item_rev );
                CHECK_IFAIL;

                /* Create loadif based on option associated with item+revision */
                if (     /* find option */
                     ITK_ok != BOM_create_window( &window )
                        /* TB::Code review - Default revision rule. Should perhaps be
                         * specified?
                         */
                     || ITK_ok != BOM_set_window_top_line( window,
                                                           NULLTAG,
                                                           option_item_rev,
                                                           NULLTAG,
                                                           &top_bomline )
                     || ITK_ok != BOM_window_find_option( window,
                                                          option_item,
                                                          option_s,
                                                          &option,
                                                          &option_rev )
                     || ITK_ok != BOM_variant_new_clause_list( window, &clause_list )
                     || ITK_ok != BOM_variant_clause_append ( clause_list,
                                                              BOM_variant_operator_and,
                                                              option,
                                                              ( ( option_eq_s[0] == '=' )
                                                                ? BOM_variant_operator_is_equal
                                                                : BOM_variant_operator_not_equal ),
                                                              option_val_s )
                    )
                {
                    HANDLE_IFAIL;
                }

                {
                    int i;
                    for (i = 0; i < line->no_of_occs; ++i)
                    {
                        tag_t cond_ve;
                        tag_t loadif_ve;
                        tag_t veb;
                        /* Create a condition from the clause_list expression editor thing. It's
                        ** possible that these could actually be shared..
                        */
                        if (    ITK_ok != BOM_variant_join_clause_list ( clause_list, &cond_ve )
                             || ITK_ok != BOM_variant_expr_load_if ( cond_ve, &loadif_ve )
                             || ITK_ok != BOM_new_variant_e_block ( &veb )
                             || ITK_ok != BOM_set_variant_e_block ( veb, 1, &loadif_ve )
                             || ITK_ok != PS_set_variant_data( parent->bvr, occs[i], veb )
                             || ITK_ok != AOM_save ( cond_ve )
                             || ITK_ok != AOM_save ( loadif_ve )
                             || ITK_ok != AOM_save ( veb )
                            )
                        {
                            HANDLE_IFAIL;
                        }
                    }
                }

                ifail = BOM_variant_delete_clause_list ( clause_list );
                CHECK_IFAIL;

                ifail = BOM_close_window( window );
                CHECK_IFAIL;
            }
            else
            {
				//printf("\n After setting qty for occ....1....2.....c");fflush(stdout);
                /*if ( line->loadif.no > 0 )
                {
                    fprintf( stderr, "WARNING: "
                             "Ignoring loadif clause with %d parameters: expecting 3 or 4\n",
                             line->loadif.no
                        );
                }*/
            }

			//printf("\n After setting qty for occ....1....2....3....");fflush(stdout);
            MEM_free( occs );
			//printf("\n After setting qty for occ....1....2....3....4....");fflush(stdout);
            ifail = AOM_save( parent->bvr );
			//printf("\n After setting qty for occ....1....2....3....4....5....");fflush(stdout);
            CHECK_IFAIL;
		 }
        }

        if ( mem_save_mode )
        {
            //  Try and unload item, rev & related stuff we know about.
            //  (Note will be inefficient for structure resuing lots of items as they may be
            //  realoaded later).
            ifail = AOM_unload( item );
            CHECK_IFAIL;

            ifail = AOM_unload( rev );
            CHECK_IFAIL;

            unload_master_forms( item );
            unload_master_forms( rev );
        }
       EXIT_1:
		  {
			  printf("\n----------Before going for :[%s]",line->item_id);fflush(stdout);
		  }

		printf("\n----------going for :[%s]",line->item_id);fflush(stdout);
        build_structure_level( line->child_chain, line, total_lines, override, mem_save_mode, verbose );

        if ( mem_save_mode  &&  line->bvr != NULLTAG )
        {
            /*
				We will have created and saved any sub-structure by now so unload this bvr.
            */
            ifail = AOM_unload( line->bvr );
            CHECK_IFAIL;
            if ( verbose )
            {
                printf( "Unloaded bvr of item %s\n", line->item_id );
            }
        }
    }else
	{
		printf("\n Boss line is null now \n");
	}
}

/*===========================================================================*/

static void unload_master_forms( tag_t  obj )
{
    int  i;
    int  n_grms;
    tag_t  rtype;
    tag_t *  grms;
    int  ifail = GRM_find_relation_type( TC_master_form_rtype, &rtype );
    CHECK_IFAIL;
    ifail = GRM_list_relations( obj, NULLTAG, rtype, NULLTAG, &n_grms, &grms );
    CHECK_IFAIL;

    for ( i = 0; i < n_grms; i++ )
    {
        logical  is_loaded;
        ifail = POM_is_loaded( grms[i], &is_loaded );
        if ( is_loaded )
        {
            tag_t  master_form;
            ifail = GRM_ask_secondary( grms[i], &master_form );
            CHECK_IFAIL;
            ifail = AOM_unload( grms[i] );
            CHECK_IFAIL;
            ifail = AOM_unload( master_form );
        }
    }
    MEM_free( grms );
}

/*===========================================================================*/

static void handle_ifail( char * file, int line )

/*  Prints out the full error stack.
**
*/
{
    int  i, n_errors;
    const int *severities;
    const int *ifails;
    const char **messages;
    EMH_ask_errors( &n_errors, &severities, &ifails, &messages );
    printf( "ERROR: (%s:%d)\n", file, line);
    for (i = 0; i < n_errors; i++)
    {
        printf( "    %6d: %s\n", ifails[i], messages[i] );
    }
    exit( EXIT_FAILURE );
}

/*===========================================================================*/
