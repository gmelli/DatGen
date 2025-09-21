
#define	DATE       "2000/02/21"
#define	VERSION    "3.1.1"
#define	SUPPORT    "support@datasetgenerator.com"

/*****************************************************************
** DatGen: A program that creates structured data.              **
**                                                              **
** Copyright (c) 2000, Gabor Melli                              **
**                                                              **
** Please fell free to make use of this code for personal use.  **
** Written consent from the author is required to use           **
** this program in a commercial setting.                        **
**                                                              **
** Comments and suggestions are welcome.                        **
**                                                              **
** for further information please refer to                      **
** http://www.datasetgenerator.com (http://www.datgen.com)      **
**                                                              **
******************************************************************
**                                                              **
** EXIT STATUS:                                                 **
**  0 - successful                                              **
**  1 - domain possibly overconstrained                         **
**  2 - parameter problem                                       **
**  3 - other error                                             **
**                                                              **
** NEW                                                          **
**                                                              **
** In 3.1                                                       **
**  - Introduced continuous datatype                            **
**  - Onesided numerical tests. T(wosided) is now an -X option  **
**  - Numerical tests now test on boundaries.                   **
**  - Rename DataGen to DatGen because the domain name was open **
**                                                              **
** In 3.0                                                       **
**  - Introduced default rule + its invocation ratio -F%        **
**  - Introduced -f param to save rules to a file               **
**  - Support for eXplicit ordinal attributes                   **
**	- Support for ordinal attr term ratio% see last example   **
**  - Support for eXplicit min/max attribute domain             **
**  - Support for eXplicit min/max attr. rule term disjunction  **
**  - Allow nominal attrs. to be displayed as strings           **
**  - Removed limbo rule effect (see s/limbo effect/)           **
**  - Maximum number of attributes is configurable: curr 256    **
**  - Improved debugging and documentation                      **
**                                                              **
** TO DO:                                                       **
**  - Support no rules -R0 (implies no Target)                  **
**  - Review values returned by exit() calls                    **
**  - Allow for guassian bias in irrelevant columns             **
**  - Read in data dictionary configuration file                **
**  - Read-in a rule-base; likely for repeatable exprmnts       **
**  - Set disallowed attribute-value combinations               **
**  - Introduce structured datatypes with hierarchies           **
**  - Upgrade knowledge rep. to capab. of decision lists        **
**  - Create rules which map to balanced decision tree          **
**	  - i.e. Murthy, Salzberg 95                              **
**   - Generate rules with more than one attribute class        **
**	  - e.g. Association rules (Agrawal et. al)               **
**                                                              ** 
** QUESTIONS:                                                   **
**  - http://www.datgen.com/questions.html	                **
**                                                              **
** BUGS:                                                        **
**  - please report to mailto:support@datgen.com                **
**                                                              **
*****************************************************************/

/*****************************************************************
** PROGRAM STRUCTURE                                            **
**                                                              **
** INCLUDE FILES                                                **
** DEFINED DEFAULTS                                             **
** SYNTAX MESSAGE                                               **
** GLOBAL STRUCTURES                                            **
** GLOBAL VARIABLES                                             **
** FORWARD DECLARATION                                          **
** MAIN                                                         **
**  - PROCESS THE PROGRAM'S PARAMETERS                          **
**  - INITIALIZE RANDOMNESS                                     **
**  - CREATE THE DATA DICTIONARY                                **
**  - TEST THE SELECTED PARAMETER SETTINGS                      **
**  - REPORT OF THE VARIABLE SETTINGS (when verbose)            **
**  - Create the CNF Rule Base                                  **
**  - CREATE EACH RULE                                          **
**  - select the number of terms in this rule                   **
**  - select a particular set of attributes for this rule       **
**  - set the size of each term                                 **
**  - set the values of each term                               **
**  - CREATE THE OBJECTS                                        **
**  - Display Rules (when verbose)                              **
**                                                              **
** SUPPORT PROCEDURES                                           **
** compare_rule_freq()                                          **
** compare_int()                                                **
** n_rand()                                                     **
** int_rand()                                                   **
** sn_rand()                                                    **
*****************************************************************/


/*****************************************************************
******************************************************************
** INCLUDE FILES                                                **
******************************************************************
*****************************************************************/

#include	<math.h>	/* log() */
/* malloc.h is not portable - stdlib.h provides malloc/calloc on all platforms */
#ifdef __linux__
#include	<malloc.h>	/* calloc() - Linux only */
#endif
#include 	<stdio.h>	/* f____() */
#include 	<string.h>	/* strtok() */
#include	<time.h>	/* time() */
#include	<stdlib.h>  /* qsort(), calloc() on macOS/BSD */


/*****************************************************************
******************************************************************
** DEFINED DEFAULTS                                             **
******************************************************************
*****************************************************************/
#define	MAX_ATTRIBUTES        256	/* plenty of dimensions */
#define	FAILURES_PER_RULE     20	/* heuristic from tests */
#define	FAILURES_PER_OBJECT   12	/* heuristic from tests */

#define	CLASS_NAME            "Class" /* default class name */

#define	DEBUG                 1
#define	PSEUDORANDOM          1
#define	VERBOSE               1

#define	UNIFORM_DISTRIBUTION  0
#define	RANDOM_DISTRIBUTION   1
#define	NORMAL_DISTRIBUTION   2

#define	NODATATYPE            0
#define	NOMINAL               1
#define	ORDINAL               2
#define	CONTINUOUS            3

#define	SET                   0
#define	ONESIDED              1
#define	TWOSIDED              2

/* characters used to divide eXplicit col definitions */
#define	XTOKENSEP             ":"
#define	XSUBTOKENSEP          ","
#define	XSUBSUBTOKENSEP       "/"

#define SMALLINTEGER          -999999999
#define BIGINTEGER             999999999

#define MISSINGVAL            88888888
#define MISSINGVALCHAR        "?"




/*****************************************************************
******************************************************************
** SYNTAX MESSAGE
******************************************************************
*****************************************************************/
#define  USAGE \
fprintf(stderr, "\nVersion %s\t%s\t%s\n", VERSION, SUPPORT, DATE) ; \
fprintf(stderr, "\nSYNTAX: %s [-hvpc] [-AefgIMmPRrO value] [-DCTd value[,value]] [-X string]\n\n", program_name) ; \
fprintf(stderr, "\tmore @ www.datasetgenerator.com/parameters.html\n") ; \
fprintf(stderr, "\th:\thelp (this report) [default value]\n") ; \
fprintf(stderr, "\tv:\tverbose report [false]\n")	; \
fprintf(stderr, "\tp:\tpseudo randomness [false]\n")	; \
fprintf(stderr, "\tc:\tplain column banner [false]\n")	; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "\te:\tProportion of erroneously entered attribute-values\n") ; \
fprintf(stderr, "\tf:\tFile path to hold rules [stdout]\n") ; \
fprintf(stderr, "\tF:\tDefault rule ratio: 1.0 to 100.0%% or 0.0 to 0.99.\n") ; \
fprintf(stderr, "\tg:\tProportion of erroneously entered class-values\n") ; \
fprintf(stderr, "\tM:\tNumber of masked relevant attributes\n") ; \
fprintf(stderr, "\tm:\tProportion of missing attribute-values\n") ; \
fprintf(stderr, "\tP:\tName of predicted attribute [%s]\n", class_name ) ; \
fprintf(stderr, "\tR:\tNumber of DNF rules\n")	; \
fprintf(stderr, "\tr:\tRule distribution 0=uniform,1=random,2=standard normal [1]\n") ; \
fprintf(stderr, "\tO:\tNumber of objects\n") ; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "\tRanges (min,max)\n") ; \
fprintf(stderr, "\tD:\tDisjunctions per rule\n") ; \
fprintf(stderr, "\tC:\tConjunctions per CNF rule component\n") ; \
fprintf(stderr, "\tT:\tDisjunctions per attribute term. e.g. B=(2,5)\n") ; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "Automated data dictionary definition.\n") ; \
fprintf(stderr, "\tA:\tNumber of relevant attributes\n") ; \
fprintf(stderr, "\td:\tDomain of all predicting attributes (min,max)\n") ; \
fprintf(stderr, "\tI:\tNumber of irrelevant attributes\n") ; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "Explicit data set definition.\n") ; \
fprintf(stderr, "\tX:\tDOM#,DISJ#,[R,I],[N,O,C],[V,M] \n") ; \
fprintf(stderr, "\t\tDOM#: Domain size. Single or Min#/Max#. Only single for Nominal.\n") ; \
fprintf(stderr, "\t\tDISJ#: Disjunctions. Single or Min#/Max#.\n") ; \
fprintf(stderr, "\t\t\tNumber of disjunctions normally assoc. with this attr.\n") ; \
fprintf(stderr, "\t\tR: Relevant, I: Irrelevant\n") ; \
fprintf(stderr, "\t\tN: Nominal, O: Ordinal, C: Continuous\n") ; \
fprintf(stderr, "\t\tV: Visible, M: Masked\n") ; \
fprintf(stderr, "\t\tmore on -X @ http://www.datasetgenerator.com/parameters.html#PAED\n") ; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "Examples\n") ; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "1\tA verbose report for 5 objects based on 3 unary rules\n\tfrom 2 predicting attributes with domain of 5\n") ; \
fprintf(stderr, "\t%% %s -O5 -R3 -d5 -A1 -v \n", program_name) ; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "2\t100 objects which abide by a balanced binary tree.\n\tEach leaf has a separate class\n") ; \
fprintf(stderr, "\t%% %s -O100 -R2 -A5 -d2 -C1 -T0 -r0 -v\n", program_name) ; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "3\tA simple use of the -X eXplicit attribute definition\n") ; \
fprintf(stderr, "\t%% %s -O13 -R2 -C0 -D0 -X10,2,O -v\n", program_name) ; \
fprintf(stderr, "\n") ; \
fprintf(stderr, "4\tThe works.\n") ; \
fprintf(stderr, "\t%% %s -O13 -R2 -C0/2 -D0/1 -X5/10,2/3,O:6,1/2,O,M:11,2,N:12,I,N:-1/1,C -F0.25 -e0.15 -m0.15 -g0.15 -f rules.txt\n", program_name) ; \
fprintf(stderr, "\n") ;


/*********************************************************************
**********************************************************************
** GLOBAL STRUCTURES
**********************************************************************
*********************************************************************/


typedef float	object[MAX_ATTRIBUTES] ;
	/* A Fully Conj. Rule definition can't be */
	/* greater in size than the attribute_map */


/***************************************
** Structures to define the Data Set. **
***************************************/

struct Attribute_def {
  char   name[3]  ;   /* Attribute's given name.                             */
  char   datatype ;   /* Nominal, ordinal ,or continuous attribute.          */
  float  dom_min  ;   /* Min num associated with this attr. = 1 if NOMINAL  */
  float  dom_max  ;   /* Max num associated with this attr  >= 1 if NOMINAL  */
  char   masked   ;   /* Is this attribute masked (1) or visible (0)         */

  /* Data to configure the rule behavior over this attribute */
  char   irrelevant ; /* Is this an irrelevant (1) or relevant (0) attr.     */
  char   testtype ;   /* ONESIDE or TWOSIDED for numeric attributes          */
  float  term_min ;   /* min set size or interval size.						*/
  float  term_max ;   /* max set size or interval size.						*/
					 /* A value in (0.0,1.0) is for a percent of the domain */
} ;



/************************************
** Structures to define the Rules. **
************************************/

struct Terms {
  /* look at Attr. defn to determine this term's datatype and testtype */
  int    attribute ;     /* keep memory of referenced attribute */
  int    setsize  ;      /* number of values-1 in the set for this term <= attribute domain */
  int    *nominal ;      /* values for terms over a nominal attribute */
  float  interval ;
  int    ordinal[2] ;    /* min/max */
  float  continuous[2] ; /* min/max */
  char   lessthan ;      /* used to visually display the rule as one sided*/

  struct Terms	
            *next_term ; /* linked list to next attribute with a term */
} ;


struct CNF_Rule {
  /* This structure contains a single CNF Rule */
  int    conjuncts ;	/* number of terms selected for this rule */
  int    attribute_map[MAX_ATTRIBUTES] ; /* bit map of attrs used by this rule */
  int    objects ;	/* number of objects instantiated using this rule */
  int    tail ;		/* class selected for this rule */
  int    default_rule ;	/* is this the default rule */
  struct Terms	*body ;	/* compact CNF description */

} *Rules ;




/*********************************************************************
**********************************************************************
** GLOBAL VARIABLES                                                 **
**********************************************************************
*********************************************************************/
int   debug    = ! DEBUG ;   /* display progress information */
int   verbose  = ! VERBOSE ; /* report supplementary information */
char  program_name[40] ;     /* kept for friendly syntax report */
char  class_name[40] ;       /* Customized class name */



/*********************************************************************
**********************************************************************
** FORWARD DECLARATION                                              **
**********************************************************************
*********************************************************************/

int     compare_rule_freq() ;
int     compare_int() ;
double  n_rand() ;
int     int_rand() ;
float   flt_rand() ;
double  sn_rand() ;
int     num2str() ;

extern double   drand48() ;
extern double   pow() ;
extern int      getopt(int argc, char **argv, char *optstring);
extern void     srand48(long seedval);

 

/*****************************************************************
******************************************************************
** MAIN                                                         **
******************************************************************
*****************************************************************/

int main(int argc, char *argv[])
{

    struct Attribute_def *Data_Dictionary=0 ;	/* Table information */

    char	rule_file[256] ;	/* file name for rule deposit */
    FILE	*rule_fd=NULL ;		/* rule file handle */
    int     i, j=0, k, l, m, n ;	/* for loop indeces */
    int     Relevant[MAX_ATTRIBUTES] ;		
    int     attributes=0 ;		/* Total number of pred. attribs */

    float   attrib_error     = 0.0 ;
    float   class_error      = 0.0 ;
    int     classes          = 0 ;
    int     column_banner    = 0 ;
    int     cnf_rules        = 0 ;
    int     cnf_min          = 0 ;
    int     cnf_max          = 0 ;
    int     customized       = 0 ;	/* flag */
    float   default_rule     = 0 ;
    int     dnf_min          = 0 ;
    int     dnf_max          = 0 ;
    float   domain_min       = 0 ;
    float   domain_max       = 1 ;
    int     irrelevant       = 0 ;
    int     masked           = 0 ;
    float   miss_ratio       = 0.0 ;	/* from 0.0 to 1.0 */
    int     random_style     = ! PSEUDORANDOM ;
    int     relevant         = 0 ;
    int     objects          = 0 ;
    int     object_failures  = 0 ;	/* Retrials counter */
    int     rule_distr       = RANDOM_DISTRIBUTION ;
    int     rule_failures    = 0 ;		/* Retrials counter */
    float   term_min         = 1 ;
    float   term_max         = 1 ;



    /* capture the program name to friendlify the error reports */
    strcpy(program_name, argv[0]) ;

    /* default name for predicted attribute */
    strcpy(class_name, CLASS_NAME) ;


    /* Clearly too few parameters */
    if (argc <= 1) {
	fprintf(stderr, "ERROR: Too few parameters. See %s -help.\n", program_name) ;
	exit(2) ;
    }


    /*********************************************************************
    ** PROCESS THE PROGRAM'S PARAMETERS                                 **
    *********************************************************************/

    { 
	char c ;        	/* contains the current parameter flag */
	extern char *optarg ;

	/* defaults */
	verbose=0 ;


	while ((c = getopt(argc, argv, "hvpzcA:e:f:g:I:M:m:P:R:r:O:D:C:T:d:F:X:")) != -1) {

         if (debug) fprintf(stderr,"debug: getopt switch (%c).\n", c) ;

	   switch(c)
	   {
		   case 'h': /* Simple help message */
			USAGE ;
			exit(2) ;

		   case 'v': /* Verbose */
			verbose=VERBOSE ;
			break ;

		   case 'p': /* Randomness Style */
			random_style=PSEUDORANDOM ;
			break ;

		   case 'z': /* Debug */
			debug=DEBUG ;
			verbose=VERBOSE ;
			break ;

		   case 'c': /* Column Banner */
			column_banner=1 ;
			break ;

		   case 'A':  /* Number of relevant attributes*/
			if (sscanf(optarg, "%d", &relevant) != 1) {
				fprintf(stderr, "ERROR: parameter -A [%s]\n", optarg) ;
				exit(2) ;
			}
			break ;

		   case 'e':  /* Proportion of erroneous attribute entries */
			if (sscanf(optarg, "%f", &attrib_error) != 1) {
				fprintf(stderr, "ERROR: parameter -e [%s]\n", optarg) ;
				exit(2) ;
			}

			break ;

		   case 'g':  /* Proportion of erroneous class entries */
			if (sscanf(optarg, "%f", &class_error) != 1) {
				fprintf(stderr, "ERROR: parameter -g [%s]\n", optarg) ;
				exit(2) ;
			}

                   break ;

		   case 'f':  /* Rule file path */
			if (sscanf(optarg, "%s", rule_file) != 1) {
				fprintf(stderr, "ERROR: parameter -f [%s]\n", optarg) ;
				exit(2) ;
			}

			if ( ! (rule_fd = fopen(rule_file, "w")) ) {
				fprintf(stderr, "ERROR: could not open file '%s'\n", rule_file) ;
				exit(3) ;
			}

			break ;

		   case 'I':  /* Number of irrelevant attributes */
			if (sscanf(optarg, "%d", &irrelevant) != 1) {
				fprintf(stderr, "ERROR: parameter -d [%s]\n", optarg) ;
				exit(2) ;
			}

                   break ;

		   case 'M':  /* Number of masked predicting relevant */
			if (sscanf(optarg, "%d", &masked) != 1) {
				fprintf(stderr, "ERROR: parameter -M [%s]\n", optarg) ;
				exit(2) ;
			}

                   break ;

		   case 'm':  /* Proportion of missing attribute-values */
			if (sscanf(optarg, "%f", &miss_ratio) != 1) {
				fprintf(stderr, "ERROR: parameter -m [%s]\n", optarg) ;
				exit(2) ;
			}

                   break ;

		   case 'P':  /* Name of predicted-attribute */
			if (sscanf(optarg, "%s", class_name) != 1) {
				fprintf(stderr, "ERROR: parameter -P [%s]\n", optarg) ;
				exit(2) ;
			}

                   break ;

		   case 'R':  /* Number of DNF rules */
			if (sscanf(optarg, "%d", &classes) != 1) {
				fprintf(stderr, "ERROR: parameter -R [%s]\n", optarg) ;
				exit(2) ;
			}

			break ;

		   case 'r': /* Rule Distribution Style */
			if (sscanf(optarg, "%d", &rule_distr) != 1) {
				fprintf(stderr, "ERROR: parameter -A [%s]\n", optarg) ;
				exit(2) ;
			}

			break ;

		   case 'O':  /* Number of objects */
			if (sscanf(optarg, "%d", &objects) != 1) {
				fprintf(stderr, "ERROR: parameter -O [%s]\n", optarg) ;
				exit(2) ;
			}
			break ;

		   case 'F':  /* Rule default existence and usage ratio */
			if (sscanf(optarg, "%f", &default_rule) != 1) {
				fprintf(stderr, "ERROR: parameter -F [%s]\n", optarg) ;
				exit(2) ;
			}
			if (debug) fprintf(stderr,"DEBUG: default_rule set to %g.\n", default_rule) ;
			if (default_rule >= 1.0) {
				fprintf(stderr, "ERROR: parameter -F [%s] must be set between [0.0, 1.0) in place of [0%%,100%%)\n",
					optarg) ;
				exit(2) ;
			}

			break ;

		   case 'D':  /* CNF rules per class */
                   { char	expression[200], *token ;

                     if (sscanf(optarg, "%s", expression) != 1) {
			fprintf(stderr, "ERROR: parameter -D [%s]\n", optarg) ;
			exit(2) ;
                     }
		     /* Get the first token */
		     token=strtok(expression, "/") ;

		     if (sscanf(token, "%d", &dnf_min) != 1) {
			fprintf(stderr, "ERROR: parameter -D [%s]\n", token) ;
			exit(2) ;
                     }

		     /* Get the next token (max) */
		     token=strtok(NULL, "/") ;
		     if (token == NULL)
			/* assume max = first token */
			dnf_max = dnf_min ;
		     else
			if (sscanf(token, "%d", &dnf_max) != 1) {
			  fprintf(stderr, "ERROR: parameter -D [%s]\n", token) ;
			exit(2) ;
                        }

		     /* Test whether the given min is less than the given max */
		     if (dnf_min > dnf_max) {
			fprintf(stderr, "PARAMETER ERROR in -D. %d > %d\n",
				dnf_min, dnf_max) ;
			exit(2) ;
		     }
		   } /* -D */

                   break ;

		   case 'C':  /* Conjuncts per CNF rule component */
                   { char	expression[200], *token ;

                     if (sscanf(optarg, "%s", expression) != 1) {
			fprintf(stderr, "ERROR: parameter -C [%s]\n", optarg) ;
			exit(2) ;
                     }

		     /* Get the first token */
		     token=strtok(expression, "/") ;

		     if (sscanf(token, "%d", &cnf_min) != 1) {
			fprintf(stderr, "ERROR: parameter -C [%s]\n", token) ;
			exit(2) ;
                     }

		     /* Get the next token (max) */
		     token=strtok(NULL, "/") ;
		     if (token == NULL)
			/* assume max = first token */
			cnf_max = cnf_min ;
		     else
			if (sscanf(token, "%d", &cnf_max) != 1) {
			  fprintf(stderr, "ERROR: parameter -C [%s]\n", token) ;
			exit(2) ;
                        }

		     /* Test whether the given min is less than the given max */
		     if (cnf_min > cnf_max) {
			fprintf(stderr, "PARAMETER ERROR in -C. %d > %d\n",
				cnf_min, cnf_max) ;
			exit(2) ;
		     }
		   } /* -C */
				   
				   break ;


		   case 'T':  /* Size for a term's range over the domain */
			   { char	expression[200], *token ;

                 if (sscanf(optarg, "%s", expression) != 1) {
					fprintf(stderr, "ERROR: parameter -T [%s]\n", optarg) ;
					exit(2) ;
                 }

                 /* Get the first token */
                 token=strtok(expression, "/") ;

                 if (sscanf(token, "%f", &term_min) != 1) {
					fprintf(stderr, "ERROR: parameter -T [%s]\n", token) ;
					exit(2) ;
                 }

                 /* Get the next token (max) */
                 token=strtok(NULL, "/") ;

                 if (token == NULL)
					/* assume first token is also max */
					term_max = term_min ;

                 else if (sscanf(token, "%f", &term_max) != 1) {
					fprintf(stderr, "ERROR: parameter -T [%s]\n", token) ;
                    exit(2) ;
                 }

                 /* Test that a positive number is used*/
                 if (term_min<0) {
					fprintf(stderr,
                       "PARAMETER ERROR -T must be greater than zero. Not %g.\n",
						term_min) ;
					exit(2) ;
                 }

                 /* Test whether both are fixed or both are domain-ratio */
                 if ((term_min>0 && term_min<1 && term_max>=1)) {
					fprintf(stderr,
						"PARAMETER ERROR with -T %f,%f\n\tboth must be either both integers > 0 or in (0.0,1.0)\n",
						term_min, term_max) ;
					exit(2) ;
                 }

                 /* Test whether the given min is less than the given max */
                 if (term_min > term_max) {
					fprintf(stderr, "PARAMETER ERROR in -T. %2.3f > %2.3f\n",
						term_min, term_max) ;
					exit(2) ;
                 }
			   } /* -T */
			   
			   break ;



		   case 'd':  /* Range of each predicting attribute */
			   { char	expression[200], *token ;

                 sscanf(optarg, "%s", expression) ;
                 token=strtok(expression, "/") ;

                 sscanf(token, "%f", &domain_min)  ;
                 token=strtok(NULL, "/") ;

                 /* One domain value or min/max */
                 if (token == NULL)
						domain_max = domain_min ;
                 else
						sscanf(token, "%f", &domain_max)  ;

                 if (domain_min > domain_max) {
						fprintf(stderr,
							"PARAMETER ERROR in -d. %f > %f\n",
							domain_min, domain_max) ;
						exit(2) ;
				 }

			   } /* -d */
			   
			   break ;









		/**************************************/
		/* -X E'X'PLICIT DATA SET DESCRIPTION */
		/**************************************/
		   case 'X': { 
		     char    expression[200], Xtokens[MAX_ATTRIBUTES][20] ;
		     char    *token, *subtoken, character=0 ;
		     int     domain, disjnct ;
		     float   rational ;	/* use to test presence of real num. */
		     int     i ;

		     /****************************************************
		     ** FORMAT	-X param                                **
		     **	                                                **
		     **	param    = dom|disjflag                         **
		     **	dom      = domain | domain_min/domain_max       **
		     **	disjflag = disj &| flag                         **
		     **	disj     = dsjncts | dsjncts_min/dsjncts_max	**
		     **	flag     = flag & flag                          **
		     **	flag     = flag &| (N | O | C)                  **
		     **	flag     = flag &| (V | M)                      **
		     **	flag     = flag &| (R | I)                      **
		     **	flag     =                                      **
		     ****************************************************/

		     customized = 1;

		     if (debug) {fprintf(stderr, "debug: decompose -X [%s]\n", optarg);}

		     /* Copy the parameter contents into a variable */
			 if (sscanf(optarg, "%s", expression) != 1) {
				/* if other than one string then error */
			   fprintf(stderr, "ERROR: parameter -X [%s]\n", optarg) ;
			   exit(2) ;
			 }

		     /* Find out the number of specified attributes to */
		     /*		preallocate the data structure */
		     attributes=0 ;
		     for ( token=strtok(expression, XTOKENSEP) ;
			   token != NULL ;
			   token=strtok(NULL, XTOKENSEP) ) {

			   if (debug) {fprintf(stderr, "token [%s]\n", token); }
			   if (attributes >= MAX_ATTRIBUTES) {
				fprintf(stderr,
					"ERROR: There is a limit of MAX_ATTRIBUTES on the number of attributes.\n") ;
				exit(2) ;
			   }
			   strcpy(Xtokens[attributes], token) ;
			   attributes++ ;
		     }
		     if (debug) {fprintf(stderr,
			"debug: %d tokens in -X\n", attributes); }


		     /* Allocate the space for the Data_Dictionary */
		     Data_Dictionary = (struct Attribute_def *)calloc(attributes, sizeof(struct Attribute_def)) ;
			 
			 
			 /* clear out the Data_Dictionary by hand */
			 /* memset and bzero() are competing Unix calls */
			 /* bzero(Data_Dictionary, attributes*sizeof(struct Attribute_def) ) ; */
			 {  char *c ; int i ;
			    c = (char *)Data_Dictionary ;
				for(i=0 ; i<sizeof(Data_Dictionary); i++) {
					*c = (char)0 ;
					c++ ;
				}
			 }


		     /* Add each attribute definition incrementally */
		     for ( i=0; i<attributes; i++) {

			int visible=0, masked=0 ;
			int relevant=0, irrelevant=0 ;
			int nominal=0, ordinal=0, continuous=0 ;
			float	rationalmin, rationalmax ;

			if (debug)
			   {fprintf(stderr,
			      "debug: process -X token [%s]\n", Xtokens[i]); }

			domain=0 ;
			disjnct=0 ;

			/* initialize structure */
			Data_Dictionary[i].datatype = NODATATYPE ;
			Data_Dictionary[i].masked   = 0 ;
			Data_Dictionary[i].dom_min  = (float)SMALLINTEGER ;
			Data_Dictionary[i].dom_max  = (float)BIGINTEGER ;
			Data_Dictionary[i].irrelevant = 0 ;
			Data_Dictionary[i].testtype = ONESIDED ; /* so that a T switches it to twosided */
			Data_Dictionary[i].term_min = (float)SMALLINTEGER ;
			Data_Dictionary[i].term_max = (float)BIGINTEGER ;

			/* process each attribute definition dimension - subtoken */
			/* three options one number, two numbers or a character */
			for (	subtoken=strtok(Xtokens[i], XSUBTOKENSEP);
			  subtoken != NULL;
			  subtoken=strtok(NULL, XSUBTOKENSEP) ) {

			
			  rationalmin = (float)SMALLINTEGER ;
			  rationalmax = (float)BIGINTEGER ;
			  rational    = (float)BIGINTEGER ;


			  if (debug) {fprintf(stderr, "debug: -X subtoken [%s]\n", subtoken); }

			  /* test whether the subtoken is two numbers */
			  if ( (sscanf(subtoken, "%f/%f", &rationalmin, &rationalmax) == 2)
					&& (rationalmax != (float)BIGINTEGER) ) {
			     if (rationalmin > rationalmax) {
				fprintf(stderr, 
				  "ERROR in parameter -X with subtoken [%s]: %f > %f\n",
				  subtoken, rationalmin, rationalmax) ;
				exit(2) ;
			     }
			  }

			  /* test whether the subtoken is a single number */
			  else if ( (sscanf(subtoken, "%f", &rational) == 1)
					&& (rational != (float)BIGINTEGER) ) {
			     if (rational <= 0) {
				fprintf(stderr,
				  "ERROR in parameter -X with subtoken [%s] is negative [%f]\n",
				  subtoken, rational) ;
				 exit(2) ;
	 		     }
			  }

			  /* test whether the subtoken is a single character */
			  else if (strlen(subtoken) == 1) {
				character = subtoken[0] ; 
			  }

			  else {
				fprintf(stderr, 
					"ERROR in parameter -X with subtoken [%s] neither number nor single character\n",
					subtoken) ;
				exit(2) ;
			  }



			  /* two numbers were provided */
			  /* the first is domain, the second disjuncts, no others */
			  if ((rationalmin < rationalmax) && (rationalmax!=(float)BIGINTEGER)) {
				if (debug)
					fprintf(stderr,"debug: Two numbers were provided [%g,%g].\n",
						rationalmin, rationalmax) ;

				/* test whether the domain has already been set */
				if (Data_Dictionary[i].dom_min == SMALLINTEGER) {
					Data_Dictionary[i].dom_min = rationalmin ;
					Data_Dictionary[i].dom_max = rationalmax ;

					if (debug) fprintf(stderr,
						"debug: dom_min = [%g] dom_max = [%g]\n",
						Data_Dictionary[i].dom_min, Data_Dictionary[i].dom_max) ;
				}

				/* test whether term sizing has already been set */
				else if (Data_Dictionary[i].term_min == SMALLINTEGER) {
					Data_Dictionary[i].term_min = rationalmin ;
					Data_Dictionary[i].term_max = rationalmax ;

					if (debug) fprintf(stderr,
						"debug: term_min = [%g] term_max = [%g]\n",
						Data_Dictionary[i].term_min, Data_Dictionary[i].term_max) ;

					/* Test that a positive number is used*/
					if (rationalmin<=0) {
						fprintf(stderr,
							"PARAMETER ERROR -T must be greater than zero. Not [%g].\n",
							term_min) ;
						exit(2) ;
	                 }
					 
					 /* Test whether both are fixed or both are domain-ratio */
					 if ((term_min>0 && term_min<1 && term_max>=1)) {
						fprintf(stderr,
							"PARAMETER ERROR with -T %f,%f\n\tboth must be either both integers > 0 or in (0.0,1.0)\n",
							term_min, term_max) ;
						exit(2) ;
					 }
					 
					 /* Test whether the given min is less than the given max */
					 if (term_min > term_max) {
						fprintf(stderr, "PARAMETER ERROR in -T. %2.3f > %2.3f\n",
							term_min, term_max) ;
						exit(2) ;
				     }

				}

				else {
				  fprintf(stderr,
					"ERROR in parameter -X with subtoken [%s]: too many numbers\n",
					subtoken) ;
				  exit(2) ;
				}
			  } /* processed dual numbers */


			  /* just one number was provided */
			  else if ((rational > 0) && (rational!=(float)BIGINTEGER)) {

				if (debug)
					fprintf(stderr,"debug: One number was provided [%g].\n",
						rational) ;

				/* test if the domain has already been set */
				if (Data_Dictionary[i].dom_min == SMALLINTEGER ) {
					/* min value depends on attr's datatype so place a flag to mark this requirement */
					Data_Dictionary[i].dom_min = MISSINGVAL ;
					Data_Dictionary[i].dom_max = rational ;

					if (debug) fprintf(stderr,
						"debug: dom_min = [%g] dom_max = [%g]\n",
						Data_Dictionary[i].dom_min, Data_Dictionary[i].dom_max) ;
				}

				/* test whether term sizing has already been set */
				else if (Data_Dictionary[i].term_min == SMALLINTEGER) {
					Data_Dictionary[i].term_min = rational ;
					Data_Dictionary[i].term_max = rational ;
					if (debug) fprintf(stderr,
						"debug: term_min = [%g] term_max = [%g]\n",
						Data_Dictionary[i].term_min, Data_Dictionary[i].term_max) ;

					/* Test that a positive number is used*/
					if (rational<=0) { fprintf(stderr,
			               "PARAMETER ERROR -T must be greater than zero. Not [%4.2f].\n",
							rational) ;
						exit(2) ;
					}

				}

				else {
				   fprintf(stderr,
					   "ERROR in parameter -X with subtoken [%s]: too many numbers\n",
					   subtoken) ;
				   exit(2) ;
				}

			  } /* processed single number */


			  /* process character based attribute definition flags */
			  else {
					if (character == 'V') {
					  visible++ ;
					  Data_Dictionary[i].masked = 0 ; }
					else if (character == 'M') {
					  masked++ ;
					  Data_Dictionary[i].masked = 1 ; }
					else if (character == 'R') {
					  relevant++ ;
					  Data_Dictionary[i].irrelevant = 0 ; }
					else if (character == 'I') {
					  irrelevant++ ;
					  Data_Dictionary[i].irrelevant = 1 ; }
					else if (character == 'N') {
					  nominal++ ;
					  Data_Dictionary[i].datatype = NOMINAL ; }
					else if (character == 'O') {
					  ordinal++ ;
					  Data_Dictionary[i].datatype = ORDINAL ; }
					else if (character == 'C') {
					  continuous++ ;
					  Data_Dictionary[i].datatype = CONTINUOUS ; }
					else if (character == 'T') {
					  Data_Dictionary[i].testtype = TWOSIDED ; }
					else {
					  fprintf(stderr, 
						  "ERROR in parameter -X with subtoken [%c]: undefined specialization\n", 
						  character) ;
					  exit(2) ;
					}
			  } /* character handler */

			} /* foreach subtoken */

			/* completed processing the next subtoken */


			/* set dom-min if it was flaged to wait for datatype */
			if (Data_Dictionary[i].dom_min == MISSINGVAL) {
			   if (Data_Dictionary[i].datatype == NOMINAL)
				Data_Dictionary[i].dom_min = 1 ;
			   else if (Data_Dictionary[i].datatype == ORDINAL)
				Data_Dictionary[i].dom_min = 1 ;
			   else
				Data_Dictionary[i].dom_min = 0 ;
			}


			if (debug) { fprintf(stderr,
					"dom_min[%g] dom_max[%g] mask[%d] irrel[%d] dtype[%d] term_min[%g] term_max[%g]\n",
					Data_Dictionary[i].dom_min,
					Data_Dictionary[i].dom_max,
					Data_Dictionary[i].masked,
					Data_Dictionary[i].irrelevant,
					Data_Dictionary[i].datatype,
					Data_Dictionary[i].term_min,
					Data_Dictionary[i].term_max					
					) ;
			}

			if (visible & masked) { fprintf(stderr,
					"ERROR in parameter -X: cannot have both V and M in the same token [%s]\n",
					subtoken) ;
				exit(2) ;
			}

			if (relevant & irrelevant) { fprintf(stderr,
					"ERROR in parameter -X: cannot have both R and I in the same token [%s]\n",
					subtoken) ;
				exit(2) ;
			}

			if (continuous + ordinal + nominal > 1) { fprintf(stderr,
					"ERROR in parameter -X: cannot have more than one of O, N or C in the same token [%s]\n",
					subtoken) ;
				exit(2) ;
			}

		    /* Enhancement: test for other invalid combos like nominal and fraction domain */


		   } /* foreach -X token */

		 } /* -X case switch */
         break ;

	   } /* switch (c) */

       } /* while getopt() */

	} /* process parameters segment */



   /* Enhancement: Test that -X was not combined w/others like -A */



/*********************************************************************
** INITIALIZE RANDOMNESS
*********************************************************************/
    if (debug) fprintf(stderr, "debug: INITIALIZE RANDOMNESS\n");

    /* if not pseudo random stir the pot with time */
    if (! random_style) {

	time_t	localTime ;

	time(&localTime) ; /* time returns a (long) */

	if (debug) fprintf(stderr, "debug: time=%ld\n", (long)localTime) ;

	srand48((long)localTime) ;
    }





/*********************************************************************
** CREATE THE DATA DICTIONARY
**	- if custom then copy in information
**	- if automated then construct based on settings
*********************************************************************/


    if (debug) fprintf(stderr, "debug: CREATE THE DATA DICTIONARY\n");

    /* prepare the data dictionary */
    if (customized) {
	/* some of data dict already specified in -X param */
	/* see: -X E'X'PLICIT ... */

	  masked=0 ; /* reset counters */
	  irrelevant=0 ;
	  relevant=0 ;

	  for (i=0; i<attributes; i++) {

	    /* Set domains which went unspecified */
	    if (Data_Dictionary[i].dom_min == SMALLINTEGER) {
			if (Data_Dictionary[i].datatype == NOMINAL) {
				int domain = (int)domain_min + int_rand((int)(domain_max-domain_min)) ;

				Data_Dictionary[i].dom_min = 1.0 ;
				Data_Dictionary[i].dom_max = (float)domain ;
				if (debug) fprintf(stderr, "debug: domain[%d] = [%d,%d]\n",
					i, (int)Data_Dictionary[i].dom_min, (int)Data_Dictionary[i].dom_max);
			}
			else if (Data_Dictionary[i].datatype == ORDINAL) {
				int domain = (int)domain_min + int_rand((int)(1+domain_max-domain_min)) ;

				Data_Dictionary[i].dom_min = 1.0 ;
				Data_Dictionary[i].dom_max = (float)domain ;
				if (debug) fprintf(stderr, "debug: domain[%d] = [%d,%d]\n",
					i, (int)Data_Dictionary[i].dom_min, (int)Data_Dictionary[i].dom_max);
			}
			else /* CONTINUOUS */ {
				float domain = domain_min + (float)n_rand(1.0)*(domain_max-domain_min) ;

				Data_Dictionary[i].dom_min = 0.0 ;
				Data_Dictionary[i].dom_max = (float)domain ;
				if (debug) fprintf(stderr, "debug: domain[%d] = [%g,%g]\n",
					i, Data_Dictionary[i].dom_min, Data_Dictionary[i].dom_max);
			}

	    }

	    /* Set term sizes which went unspecified */
	    if (Data_Dictionary[i].term_min == SMALLINTEGER) {
			Data_Dictionary[i].term_min = term_min ;
			Data_Dictionary[i].term_max = term_max ;
	    }

	    /* Update the attributes relevancy constructs */
	    if (! Data_Dictionary[i].irrelevant) {
			if (debug) fprintf(stderr, "debug: setting relevant %d\n", i ) ;	

			Relevant[i]=1 ;
			relevant++ ;
	    }
		else {
			if (debug) fprintf(stderr, "debug: setting irrelevant %d\n", i ) ;	

			Relevant[i]=0 ;
			irrelevant++ ;	/* update the count */
		}

	    /* Update the masked constructs */
	    if (Data_Dictionary[i].masked)
			masked++ ;

	    /* Set the datatype */
	    if (Data_Dictionary[i].datatype == NODATATYPE)
			Data_Dictionary[i].datatype = ORDINAL ;

	  }
    }  /* Completed customized data dictionary definition */





    /* Automated data dictionary generation */
    else {

	  if (debug) fprintf(stderr,"debug: AUTOMATED DATA DICTIONARY GENERATION\n") ;

	  attributes = relevant + irrelevant ;
	  
	  if (attributes >= MAX_ATTRIBUTES) { fprintf(stderr,
				"ERROR: There is a limit of MAX_ATTRIBUTES on the number of attributes.\n") ;
			exit(2) ;
	  }
	  
	  if (debug) fprintf(stderr,"debug: attributes %d = relevant %d + irrelevant %d\n",
		attributes, relevant, irrelevant) ;

	  /* Can only have as many masked relevant attributes as there are relevant attributes */
	  if (masked > relevant) masked = relevant ;


	  /* Instantiate the data structure */
	  Data_Dictionary = (struct Attribute_def *)calloc(attributes, sizeof(struct Attribute_def)) ;

	  /* clear out the Data_Dictionary by hand */
	  /* memset and bzero() are competing Unix calls */
	  /* bzero(Data_Dictionary, attributes*sizeof(struct Attribute_def) ) ; */
	  {  char *c ; int i ;
		c = (char *)Data_Dictionary ;
		for(i=0 ; i<sizeof(Data_Dictionary); i++) {
			*c = (char)0 ;
			c++ ;
		}
	  }


	  /* Set all attr. to irrlev. The next section sets the relevant ones */
	  for (i=0; i<attributes; i++) {	
		  Relevant[i]=0 ;
		  Data_Dictionary[i].irrelevant = 1 ;
	  }

	  /* Specify datatypes for the attributes */
	  /* Enhancement: allow for automated distribution of datatypes. Currently all NOMINAL. */
	  for(i=0; i<attributes; i++) {
		  Data_Dictionary[i].datatype = NOMINAL ;
		  Data_Dictionary[i].testtype = SET ;
	  }


      /* Set the relevant attributes */
	  for (i=0, j=0; i<relevant; i++) {
	    int	offset ;

	    /* Randomly locate a distinct attribute */
		/* ENHANCE: catch infinite loop */
	    for (offset=int_rand(attributes);
			Relevant[offset]==1 ;
			offset=int_rand(attributes) )
			
			{

			if (debug) fprintf(stderr,
				"debug: irrelev repeat %d %d\n", offset, Relevant[offset] ) ;
			
			}

		if (debug) fprintf(stderr,
			"debug: Selection %d for relevant attribute is %d. %d to go.\n",
			i, offset, relevant-i+1) ;

	    Relevant[offset]=1 ;
	    Data_Dictionary[offset].irrelevant = 0 ;

	    /* The first few necessary randomly chosen relevant attributes will be masked */
	    if (j++ < masked)
	        Data_Dictionary[offset].masked = 1 ;

	  }


	  /* Specify domains for the attributes */
	  /* We are assuming a nominal datatype */
	  for(i=0; i<attributes; i++) {
		  int domain = (int)domain_min + int_rand((int)(1+domain_max-domain_min)) ;
		  Data_Dictionary[i].dom_min = 1.0 ;
		  Data_Dictionary[i].dom_max = (float)domain ;
		  if (debug) fprintf(stderr, "debug: domain[%d] = [%d,%d]\n",
			  i, Data_Dictionary[i].dom_min, Data_Dictionary[i].dom_max);
	  }


    } /* for each attribute automatically set data dict */



    /* Give the attributes a unique human readable name */
    /* Start with value 1 */
    for (i=0; i<attributes; i++) {
	Data_Dictionary[i].name[0] = 'A'+i ;
	Data_Dictionary[i].name[1] = 0 ;
    	if (debug) fprintf(stderr,"debug: Attribute [%d] received name [%s]\n",
		i+1, Data_Dictionary[i].name) ;
    }

    /* Normalize the attributes with terms */
    for (i=0; i<attributes; i++) {
	if (Data_Dictionary[i].term_min>0 && Data_Dictionary[i].term_max<1) {
	   float dom_range ;

	   dom_range = Data_Dictionary[i].dom_max - Data_Dictionary[i].dom_min ;

	   Data_Dictionary[i].term_min *= dom_range ;
	   Data_Dictionary[i].term_max *= dom_range ;
	}
    }



    /* The data dictionary is now completed. All columns are described */
	/* for both customized and autogenerated dictionaries. */


    if (debug) { /* Print a data dictionary report */

	fprintf(stderr, "DEBUG: Data Dictionary Report\n") ;
	fprintf(stderr, "\n  Name:\t") ;
	for (i=0; i<attributes; i++) fprintf(stderr, "%2s\t", Data_Dictionary[i].name) ;

	fprintf(stderr, "\n  Dom:\t") ;
	for (i=0; i<attributes; i++) fprintf(stderr, "%g/%g\t",
		Data_Dictionary[i].dom_min, Data_Dictionary[i].dom_max) ;

	fprintf(stderr, "\n  Term:\t") ;
	for (i=0; i<attributes; i++) fprintf(stderr, "%g/%g\t",
		Data_Dictionary[i].term_min, Data_Dictionary[i].term_max) ;

	fprintf(stderr, "\n  Dtyp:\t") ;
	for (i=0; i<attributes; i++) {
	    if (Data_Dictionary[i].datatype == NOMINAL) fprintf(stderr, "N\t") ;
	    else if (Data_Dictionary[i].datatype == ORDINAL) fprintf(stderr, "O\t") ;
	    else if (Data_Dictionary[i].datatype == CONTINUOUS) fprintf(stderr, "C\t") ;
	    else fprintf(stderr, "?\t") ;
	}


	fprintf(stderr, "\n  Mask:\t") ;
	for (i=0; i<attributes; i++) fprintf(stderr, " %d\t", Data_Dictionary[i].masked);

	fprintf(stderr, "\n  Irre:\t") ;
	for (i=0; i<attributes; i++) {
	    if (Data_Dictionary[i].irrelevant) fprintf(stderr, " 1\t");
	    else fprintf(stderr, " 0\t") ;
	}

	fprintf(stderr, "\n  Side:\t") ;
	for (i=0; i<attributes; i++) {
	    if (Data_Dictionary[i].testtype == ONESIDED) fprintf(stderr, " 1\t");
	    else fprintf(stderr, " 2\t") ;
	}
	fprintf(stderr, "\n\n") ;

    }



    /*********************************************************************
    ** TEST THE SELECTED PARAMETER SETTINGS
    *********************************************************************/

    /* we are using a bitmap so we are limited by the int size */
    if (attributes > MAX_ATTRIBUTES) {
		fprintf(stderr,
			"\nERROR: The number of predicting attributes %d\nis greater than current max (const)%d\n",
			attributes, MAX_ATTRIBUTES) ;
		fprintf(stderr,
			"\tYou may update MAX_ATTRIBUTES and recompile.\nSorry for the inconvenience.\n\n") ;
		exit(2) ;
    }

    if (attributes <= 0) {
		fprintf(stderr, "\nERROR: 0 Predicting Attributes\n") ;
		exit(1) ;
    }


    /* Test that that there are enough attributes to support -D */
    if (dnf_max >= relevant) {
	fprintf(stderr,
	   "ERROR in parameter -D: the maximum number of rule disjunctions (%d) requires at least %d relevant attributes. Not the specified %d.\n",
	   dnf_max, dnf_max+1, relevant) ;
	exit(1) ;
    }



    /*********************************************************************
    ** REPORT OF THE VARIABLE SETTINGS (when verbose)
    *********************************************************************/
    if (verbose) { 
	fprintf(stdout, "VERSION: %s\n\n", VERSION);
        fprintf(stdout, "VARIABLES\n\n");
	if (random_style)
	   fprintf(stdout, "     pseudo:\tRandomness\n") ;
	else
	   fprintf(stdout, "       full:\tRandomness\n") ;

	if (rule_distr==0)
	   fprintf(stdout, "       unif:\t%s\n"  , "Rule distribution") ;
	else if (rule_distr==1)
	   fprintf(stdout, "       rand:\t%s\n"  , "Rule distribution") ;
	else
	   fprintf(stdout, "      stdno:\t%s\n"  , "Rule distribution") ;

	fprintf(stdout, "\n") ;
	fprintf(stdout, "      %5d:\t%s\n"  , objects, "Objects") ;
	fprintf(stdout, "      %5d:\t%s\n"  , classes, "Classes (non-default)") ;
	fprintf(stdout, "      %5d:\t%s\n"  , relevant, "Relevant Attributes") ;
	fprintf(stdout, "      %5d:\t%s\n"  , irrelevant, "Irrelevant Attributes") ;
	fprintf(stdout, "      %5d:\t%s\n"  , masked, "Masked Relevant Attributes") ;
	fprintf(stdout, "      %0.3f:\t%s\n", default_rule, "Proportion of default rules") ;
	fprintf(stdout, "      %0.3f:\t%s\n", attrib_error, "Proportion of erroneously entered predicting attribute-values") ;
	fprintf(stdout, "      %0.3f:\t%s\n", class_error, "Proportion of erroneously entered class values") ;
	fprintf(stdout, "      %0.3f:\t%s\n", miss_ratio, "Proportion of missing values") ;
	fprintf(stdout, "\n") ;
	if (customized != 1) {
	  fprintf(stdout, "    %2f,%-4f:\t%s\n"  , domain_min, domain_max, "Avg. Attribute domains") ;
	  fprintf(stdout, "    %2d,%-4d:\t%s\n"  , dnf_min, dnf_max, "Avg. Disjuncts per rule") ;
	  fprintf(stdout, "    %2d,%-4d:\t%s\n"  , cnf_min, cnf_max, "Avg. Conjuncts per rule component") ;
	  if (term_min<1)
	    fprintf(stdout, "    %2.2f,%-2.2f:\t%s\n"  , term_min, term_max, "Avg. Disjuncts per rule term") ;
	  else
	    fprintf(stdout, "    %2.0f,%-4.0f:\t%s\n"  , term_min, term_max, "Avg. Disjuncts per rule term (min>1)") ;
	}

	fprintf(stdout, "\n\n") ;
   } /* end of report */






   /*********************************************************************
   ** Create the CNF Rule Base
   *********************************************************************/
   if (debug) fprintf(stderr, "\nDEBUG: Create the Rules\n");

   {
      int *components ; /* Array with the # of CNFs for each rule */
      int i, j, k ;


      /* CNF rule components for each class (DNF rule) */
      components = (int *)calloc(classes, sizeof(int)) ;


      /* Determine how many CNF rules need to be created */
      cnf_rules=0 ;
      for (i=0; i<classes; i++) {
		  components[i] = 1 + dnf_min + int_rand(1+dnf_max-dnf_min) ;
		  cnf_rules += components[i] ;
		  if (debug) fprintf(stderr,
			  "Class %d has %d CNF components\n", i, components[i]) ;
      }

      if (debug) fprintf(stderr,
		  "Rule base will have %d CNF rules.\n", cnf_rules) ;

      /* Allocate the space for the CNF rule base */
      /* Add one (+1) because of the default rule */
      Rules = (struct CNF_Rule *)calloc(cnf_rules+1, sizeof(struct CNF_Rule)) ;

      /* Set the class and default status of the default_rule */
      Rules[0].tail = 0 ;
      Rules[0].default_rule = 1 ;

      /* Set the class and default status of each rule k. */
      /* Start with class i=1. */
      for (k=1, i=1; i<=classes; i++) {
		for (j=0; j<components[i-1]; j++) {
	         Rules[k].tail = i ;
	         Rules[k].default_rule = 0 ;
	         if (debug) fprintf(stderr,
				"debug: cnf rule [%d] is of class/tail = [%d]\n",
				k, Rules[k].tail) ;
		   k++;
		}
	 }

   }


   /*******************************************************
   ** CREATE ONE SUBRULE AT A TIME                       **
   ** halt if unable to squeeze another in               **
   *******************************************************/
   rule_failures = 0 ;
   for (i=1; i<=cnf_rules; i++) {
	int            attribute_map[MAX_ATTRIBUTES] ;
	int            offset, conjuncts ;
	struct Terms   *Term=NULL ;
	struct Terms   *Term_prev=NULL ;
	struct Terms   *body=NULL ;
	int            f_c_rules ;	/* number of fully conjunctive rules for this CNF rule */
	int            New_rule_ok ;

	/*******************************************************
	** select the number of terms/attributes in this rule **
	*******************************************************/
	conjuncts = cnf_min + int_rand(1+cnf_max-cnf_min) ;
	if (conjuncts > relevant-1) conjuncts = relevant-1 ;
	if (debug) fprintf(stderr, "\nDEBUG: Rule %d has %d conjuncts => %d terms\n",
			i, conjuncts, conjuncts+1 ) ;


	/* clean the map */
	{  int i ;
	   for(i=0; i<MAX_ATTRIBUTES; i++) attribute_map[i]=0 ;
	}

	/* select a particular set of attributes for this rule */
	for (j=0; j<=conjuncts; j++) {
	   int loopcount = 0 ;

	   if (debug) fprintf(stderr, " Term %d ", j+1 ) ;

	   /** randomly locate a distinct relevant attribute **/
	   /** test possibilities until an open offset is found **/
	   for ( offset=int_rand(attributes) ;
		 (attribute_map[offset]==1) || (Relevant[offset]==0) ;
		 offset=int_rand(attributes), loopcount++ ) {

	      if (debug)
		   fprintf(stderr, "[%s failed] ",
			Data_Dictionary[offset].name) ;

		  if (loopcount > 256) {
			  fprintf(stderr, "ERROR: infinite loop condition 507204720\n") ;
			  exit(2) ;
		  }
	   }

	   attribute_map[offset] = 1 ; /* make the assignment */
   
	   if (debug) fprintf(stderr, " attr[%s] ",
			Data_Dictionary[offset].name) ;

	} /* the attribute_map contains the term's attributes */

	if (debug) fprintf(stderr, "\n") ;


	/*********************************************************
	** Foreach term, e.g. A in {} or A in [,] define its    **
	** dimensions and create its data structure.            **
	*********************************************************/
	/* create the head node of the list */
	body = Term = (struct Terms *)malloc(sizeof(struct Terms)) ;
	f_c_rules = 1 ; 

	/* act on each term sequentially */
	for (j=0; j<attributes; j++) if (attribute_map[j]==1) {

	   Term->attribute = j ;

	   /* NOMINAL */
	   if (Data_Dictionary[j].datatype == NOMINAL) {
			float term_max=Data_Dictionary[j].term_max ;
			float term_min=Data_Dictionary[j].term_min ;
			float setsize=0 ;

			if (debug) fprintf(stderr,
				"debug: determine setsize for attribute [%s] with term min/max [%g/%g].\n",
				Data_Dictionary[j].name, term_min, term_max) ;

			setsize = 1 + term_max - term_min ;
			setsize *= ceil(n_rand(1.0)) ;
			setsize += Data_Dictionary[j].term_min ;
			
			/* keep disjuncts within bounds */
			if (setsize < 1) {
				fprintf(stderr,
					"ERROR: setsize for attribute [%s] is less than one [%g].\n",
					Data_Dictionary[j].name, setsize) ;
				exit(3) ;
			}
			if (setsize > Data_Dictionary[j].dom_max) {
				fprintf(stderr,
					"ERROR: setsize for attribute [%s] is bigger than the domain [%g > %g].\n",
					Data_Dictionary[j].name, setsize, Data_Dictionary[j].dom_max) ;
				exit(3) ;
			}

			/* Set the number of disjuncts for this term */
			Term->setsize = (int)setsize ;

			if (debug)
				fprintf(stderr,
					"debug: Attribute [%d] ->setsize [%d].\n",
					j, Term->setsize ) ;
	
			/* create the space to hold the values for this term */
			Term->nominal = (int *)calloc(1+Term->setsize, sizeof(int)) ;


		} /* NOMINAL */

		/* ORDINAL  */
		if (Data_Dictionary[j].datatype == ORDINAL) {
			int interval = (int)(Data_Dictionary[j].term_min
					+ int_rand((int)(1 + Data_Dictionary[j].term_max
								- Data_Dictionary[j].term_min) ) );

			/* keep disjuncts within bounds */
			if (interval < 1) {
			   fprintf(stderr, "ERROR: Ordinal interval calculated to be < 1 [%d].\n",
				interval) ;
			   exit(3) ;
			}
			if (interval > Data_Dictionary[j].dom_max - Data_Dictionary[j].dom_min) {
			   fprintf(stderr, "ERROR: Ordinal interval calculated to be < 1 [%g].\n",
				(int)(Data_Dictionary[j].dom_max - Data_Dictionary[j].dom_min) ) ;
			   exit(3) ;
			}

			Term->setsize = (int)interval ;

			if (debug)
				fprintf(stderr, "debug: Attribute %d interval [%d].\n", j, (int)interval ) ;

		} /* ORDINAL */


		/* CONTINUOUS */
		if (Data_Dictionary[j].datatype == CONTINUOUS) {
			float interval = Data_Dictionary[j].term_min
					+ (float)n_rand(1.0)*(Data_Dictionary[j].term_max
								- Data_Dictionary[j].term_min) ;

			/* keep disjuncts within bounds */
			if (interval < 0) {
				fprintf(stderr,
					"\nERROR: term interval on continuous attribute [%s] is smaller than zero [%f].\n",
					Data_Dictionary[j].name, interval) ;
				exit(3) ;
			}
			if (interval > Data_Dictionary[j].dom_max - Data_Dictionary[j].dom_min) {
				fprintf(stderr,
					"\nERROR: term interval on continuous attribute [%s] -> [%f] is larger than the attribute's domain [%f].\n",
					Data_Dictionary[j].name, interval,
					Data_Dictionary[j].dom_max - Data_Dictionary[j].dom_min) ;
				exit(3) ;
			}

			Term->interval = interval ;


			if (debug)
				fprintf(stderr, "debug: Attribute [%s] interval [%f].\n",
					Data_Dictionary[j].name, interval ) ;

		} /* CONTINUOUS */



	    /* add a node in the linked list for the next term */
	    Term->next_term = (struct Terms *)malloc(sizeof(struct Terms)) ;
	    Term_prev = Term ;
	    Term = Term->next_term ;
	}

	/* The algorithm creates one too many nodes. */
	free(Term) ; Term_prev->next_term = NULL ;


	/***********************************************
	** set the values of each term in the subrule **
	***********************************************/
	Term = body ;
	for (j=0; j<attributes; j++) if (attribute_map[j]==1) {
	    
	    if (Data_Dictionary[j].datatype == CONTINUOUS) {

			if (Data_Dictionary[j].testtype == TWOSIDED) {
				float lower  ;
				float higher ;

				/* set the lower and higher bounds of the range */

				/* calculate how much room there is on either size of the interval */
				lower = Data_Dictionary[j].dom_max - Data_Dictionary[j].dom_min - Term->interval ;
				/* randomize the size of this limit */
				lower = (float)n_rand(1.0) * lower ;
				/* start the lower at the domainmin plus the random value */
				lower += Data_Dictionary[j].dom_min ;

				higher = lower + Term->interval ;


				if (debug) fprintf(stderr,
					"debug: Continuous Attribute %s TWOsided interval [%g,%g]\n",
					Data_Dictionary[j].name, Term->continuous[0], Term->continuous[1]) ;

				/* Assign the range */
				Term->continuous[0] = lower ; 
				Term->continuous[1] = higher ; 
			}

			else { /* assume ONESIDED test */
				 /* flip coin to determine lessthan */
				if (n_rand(1.0) >= 0.50) {
					Term->continuous[0] = Data_Dictionary[j].dom_min ;
					Term->continuous[1] = Term->continuous[0] + Term->interval ;

					Term->lessthan = 1 ;
				}
				else {
					Term->continuous[1] = Data_Dictionary[j].dom_max ;
					Term->continuous[0] = Term->continuous[1] - Term->interval ;

					Term->lessthan = 0 ;
				}

				if (debug) fprintf(stderr,
					"debug: Continuous Attribute %s ONEsided interval [%g,%g]\n",
					Data_Dictionary[j].name, Term->continuous[0], Term->continuous[1]) ;

			}

		} /* CONTINUOUS */


		/*************************/
		/* ORDINAL Term contents */
		/*************************/
		else if (Data_Dictionary[j].datatype == ORDINAL) {

		   if (Data_Dictionary[j].testtype == TWOSIDED) {
			int lower  ;
			int higher ;

			/* set the lower and higher bounds of the range */

			/* calculate how much room there is on either size of the interval */
			lower = (int)(Data_Dictionary[j].dom_max - Data_Dictionary[j].dom_min - Term->setsize) ;
			/* randomize the size of this limit */
			lower = int_rand(lower+2) ;
			/* start the lower at the domainmin plus the random value */
			lower += (int)Data_Dictionary[j].dom_min ;

			higher = lower + Term->setsize - 1 ; /* minus 1 to achieve setsize */


			if (debug) fprintf(stderr,
				"debug: Ordinal Attribute %s interval [%d,%d]\n",
				Data_Dictionary[j].name, lower, higher) ;

			/* Assign the range */
			Term->ordinal[0] = lower ; 
			Term->ordinal[1] = higher ; 
		   }

		   else { /* assume ONESIDED test */
				 /* flip coin to determine lessthan */
			if (n_rand(1.0) >= 0.50) {
					Term->ordinal[0] = (int)Data_Dictionary[j].dom_min ;
					Term->ordinal[1] = Term->ordinal[0] + Term->setsize - 1 ;

					Term->lessthan = 1 ;
		   	}
		   	else {
					Term->ordinal[1] = (int)Data_Dictionary[j].dom_max ;
					Term->ordinal[0] = Term->ordinal[1] - Term->setsize + 1 ;

					Term->lessthan = 0 ;
			}

		   }

		} /* ORDINAL */




	    else if (Data_Dictionary[j].datatype == NOMINAL) {
		/*
		** Nominal attributes have randomly selected values from the domain.
		** ENHANCE: test for infinite loop
		*/
		  for (k=0; k<Term->setsize; k++) {
		    int seed ;

		    seed = int_rand( (int)Data_Dictionary[j].dom_max ) ;
		    seed += 1 ; /* don't want value 0 */
		    if (debug) 
				fprintf(stderr, " DEBUG: value %d added to disjunct %d\n", seed, k+1) ;

		    /* Let's commit to this value.
		    ** and then check if it is a repeat
		    */
		    Term->nominal[k] = seed ;
		    for (l=0; l<k; l++)
				if (Term->nominal[l] == seed) {
					if (debug) fprintf(stderr, "WARN: %d is a repeat value\n", seed) ;
					k-- ; /* let's try again */
				}	        
		
		  }

		  /* Sort the list of values so the are human readable */
		  qsort(Term->nominal, Term->setsize, sizeof(int), compare_int) ;

		  if (debug) { /* print out the values in this term */
			int base ;
			fprintf(stderr, "debug: sorted term disjunction [") ;		
			for (base=0; base<Term->setsize; base++) {		
				fprintf(stderr, "%d ", (int)Term->nominal[base]) ;
			}		
			fprintf(stderr, "]\n") ;
		  }

		} /* NOMINAL */


	    /* Neither continous, ordinal or nominal */
	    else {
			fprintf(stderr, "ERROR in assigning a rule term's value\n") ;
			fprintf(stderr, "neither continous, ordinal or nominal\n") ;
			exit(3) ;
	    }
	

	    Term = Term->next_term ;

	} /* For each term in the sub-rule define/create */





	/***************************************************************
	** DEBUG: List out the "rule" for this attempt.               **
	***************************************************************/
	if (debug) {
	   struct Terms *Term ;
	   int j, k ;
	   int firstA, firstB ;
	   FILE *stream = stderr ;

	   fprintf(stderr,"debug: RULE == ") ;

	   Term = body ;

		/* for each attribute   used by the rule */
	   for (j=0, firstA=1; j<attributes; j++) if (attribute_map[j]==1) {

			/* time for an ampersand & */
			if (firstA) firstA=!firstA ;
		    else fprintf(stream, " & ") ;

			/* print the attribute's name */
		    fprintf(stream, "%s", Data_Dictionary[j].name) ;

		    /* show that this was a masked attribute */
		    if (Data_Dictionary[j].masked) fprintf(stream, "*" ) ;

			if (Data_Dictionary[j].datatype == NOMINAL) {
				char buffer[256] ;
				
				fprintf(stream, "=") ;

				if (debug) fprintf(stream, "|%d|", Term->setsize);

			    /* add parentheses if this is a disjuntive term */
			    if (Term->setsize > 1) fprintf(stream, "(") ;

				/* for each value in the term's set */
			    for (k=0,firstB=1; k<Term->setsize; k++) {

					if (firstB) firstB=!firstB ;
					else fprintf(stream, "," ) ;

					num2str(Term->nominal[k], buffer) ;
					fprintf(stream,"%s", buffer ) ;
				}
				/* conclude the set if there were several values */
			    if (Term->setsize > 1) fprintf(stream, ")" ) ;
			}
			
			else if (Data_Dictionary[j].testtype == TWOSIDED) {

				if (Data_Dictionary[j].datatype == ORDINAL)
						fprintf(stream, "=[%d,%d]", Term->ordinal[0], Term->ordinal[1] ) ;
				else
						fprintf(stream, "=[%g,%g]", Term->continuous[0], Term->continuous[1] ) ;
			}

			else if (Data_Dictionary[j].testtype == ONESIDED) {

				if (Term->lessthan) {
					fprintf(stream, "<=") ;
					if (Data_Dictionary[j].datatype == ORDINAL)
						fprintf(stream, "%d", Term->ordinal[1] ) ;
					else
						fprintf(stream, "%f", Term->continuous[1] ) ;
				}
				else {
					fprintf(stream, ">=") ;
					if (Data_Dictionary[j].datatype == ORDINAL)
						fprintf(stream, "%d", Term->ordinal[0] ) ;
					else
						fprintf(stream, "%f", Term->continuous[0] ) ;
				}

			}

			else { /* ERROR */
					fprintf(stderr, "ERROR: unknown condition 84792740 [%d][%d].\n",
						Data_Dictionary[j].datatype, Data_Dictionary[j].testtype) ;
					exit(3) ;
				}
			
			
			Term=Term->next_term ;

	   } /* for every term */

	   fprintf(stream, "\n" ) ;
		
	} /* if debug print rule */




	/************************************************
	** Test this new rule against all current rules **
	************************************************/
	New_rule_ok = 1 ;

	for (k=1; k<i; k++) { /* for each commited rule */
	   /* let's count the number of matches and mismatches for each rule */
	   struct Terms *Term2 ;
	   int matches = 0 ;
	   int mismatches = 0 ;

	   Term=body ;
	   Term2=Rules[k].body ;

	   if (debug) fprintf(stderr, "debug: test against commited rule k=%d\n", k) ;

	   /* test the two rules one term at a time*/
	   for (j=0; j<attributes; j++) {
		/* If both rules reference this dimension
		   then test the specific values */

		if (debug) fprintf(stderr, "debug: attr j=%d\n", j) ;

		/* do both terms reference this attribute? */
		if ((attribute_map[j]==1) && (Rules[k].attribute_map[j]==1)) {
			if (Data_Dictionary[j].datatype == NOMINAL) {
				int i, j ;
				int term_matches = 0 ;

				/* brute force test of all combinations */
				for (i=0; i<Term->setsize; i++)
				for (j=0; j<Term2->setsize; j++) {

				   if (debug) {
					fprintf(stderr, "debug: test %d == %d? ",
						Term->nominal[i], Term2->nominal[j]) ;
				   }

				   if (Term->nominal[i] == Term2->nominal[j]) {
					term_matches++ ;
					if (debug) fprintf(stderr,"YES") ;
				   }
				   else
					if (debug) fprintf(stderr,"NO") ;

				   if (debug) fprintf(stderr,"\n") ;


				} /* for all combinations */

				if (term_matches>0) matches++ ; 
				else mismatches++ ;
			}

			else if (Data_Dictionary[j].datatype == ORDINAL) {

					  if (debug) {
						fprintf(stderr, "debug: test [%d,%d] within [%d,%d]?\n",
							Term->ordinal[0], Term->ordinal[1],
							Term2->ordinal[0], Term2->ordinal[1]) ;
					  }

					  /* test both ends of the range */
					  if ((Term->ordinal[0]>=Term2->ordinal[0])
							&& (Term->ordinal[0]<=Term2->ordinal[1]))
						matches++ ; 
					  else if  ((Term->ordinal[1]>=Term2->ordinal[0])
							&& (Term->ordinal[1]<=Term2->ordinal[1]))
						matches++ ; 
					  else
						mismatches++ ;
			}

			else if (Data_Dictionary[j].datatype == CONTINUOUS) {

					  if (debug) {
						fprintf(stderr, "debug: test [%g,%g] within [%g,%g]?\n",
							Term->continuous[0], Term->continuous[1],
							Term2->continuous[0], Term2->continuous[1]) ;
					  }

					  /* test both ends of the range */
					  if ((Term->continuous[0]>=Term2->continuous[0])
							&& (Term->continuous[0]<=Term2->continuous[1]))
						matches++ ; 
					  else if  ((Term->continuous[1]>=Term2->continuous[0])
							&& (Term->continuous[1]<=Term2->continuous[1]))
						matches++ ; 
					  else
						mismatches++ ;
			}

			if (debug) fprintf(stderr,"j[%d] ", j) ;

		} /* both terms reference the attribute */

		/* Else if only one of the rules references this dimension
		   then the other is free over the dimension and there is a
		   conflict (limbo effect) e.g. IF A=1 THEN C1 IF B=1 THEN C2 */
			
		else if ((Rules[k].attribute_map[j]==1)
					|| (attribute_map[j]==1)) {

			matches++ ; 

			if (debug && (Rules[k].attribute_map[j]==1))
				fprintf(stderr,"debug: only the commited rule references this attribute\n") ;
			else if (debug)
				fprintf(stderr,"debug: only the new rule references this attribute\n") ;
		}

		/* advance to the next term */
		if (attribute_map[j]==1) {
			Term = Term->next_term ;
			if (debug) fprintf(stderr,"debug: next term [%p] in new rule\n", Term) ;
		}
		if (Rules[k].attribute_map[j]==1) {
			Term2 = Term2->next_term ;
			if (debug) fprintf(stderr,"debug: next term [%p] in commited rule [%d]\n",
				Term2, k) ;
		}

	   } /* move onto the next term */


	   /* If there were matches and no mismatches
		   then there is an overlap so we need to try again. */
	   if (matches && ! mismatches) {
		New_rule_ok = 0 ; /* FALSE */
		if (debug) fprintf(stderr,
			"\nWARN: New subrule %d conflicts with subrule %d.\n", i+1, k+1) ;

		/* terminate the three loops */
		k=i-1 ;
	   }
	   else {
		if(debug)
			fprintf(stderr, " matches=%d, mismatches=%d\n",matches, mismatches) ;
	   }

	} /* move onto the next rule */


	if ( New_rule_ok ) {
	    /* Accept this new rule */
	    Rules[i].body = body ;
		{ int j ;
		  for (j=0;j<MAX_ATTRIBUTES;j++)
			  Rules[i].attribute_map[j]=attribute_map[j] ;
		}
	    Rules[i].conjuncts = conjuncts ;
	    Rules[i].objects = 0 ;
	}

	else {
	    /* FAIL: Rule creation has occurred too often */
	    if (rule_failures++ > FAILURES_PER_RULE * cnf_rules) {
		fprintf(stderr, "\nEXCEPTION:\n\tFailed to create a RULE base.\n") ;
		fprintf(stderr, "\tThis domain appears to be too constrained!\n\n") ;
		fprintf(stderr, "\tIncrease the sizes of your attribute domains.\n\n") ;
		exit(1) ;
	    }

	    /*
	    ** This attempt failed free up the resources and setup to try again.
	    */
	    i-- ;

	    Term = body ;
	    while(Term != NULL) {
		Term_prev = Term ;
		Term = Term_prev->next_term ;
		free(Term_prev) ;
	    }
	}

    } /* for each i cnf_rule */







    /***********************************************/
    /***********************************************/
    /** WE NOW HAVE A VALID SYNTHETIC RULE BASE!! **/
    /***********************************************/
    /***********************************************/



    /*************************************************************
    ** CREATE THE OBJECTS					**
    *************************************************************/
    if (debug) fprintf(stderr, "\nDEBUG: CREATE THE OBJECTS.\n") ; 


    if (verbose) {
	/*************************************************
	** A descriptive attribute banner was requested **
	*************************************************/

	  char	suffix[3] ;

	  if (debug) fprintf(stderr, 
			"\nDEBUG: A descriptive attribute banner was requested.\n");
	
	  fprintf(stdout, "\nOBJECTS\n\n");
	  fprintf(stdout, "Object#\t\t") ;

	  for (k=0; k<attributes; k++) {
            
		if (debug) fprintf(stderr, "DEBUG: attribute %d.\n", k);
	    
		l=0 ;

	    if (Data_Dictionary[k].irrelevant)
			suffix[l++]='I' ;
	    else
			suffix[l++]='R' ;

	    if (Data_Dictionary[k].masked) suffix[l++]='M' ;

	    /* add datatype letter */
	    if (Data_Dictionary[k].datatype==NOMINAL)
			suffix[l++]='N' ;
	    else if (Data_Dictionary[k].datatype==ORDINAL)
			suffix[l++]='O' ;
	    else if (Data_Dictionary[k].datatype==CONTINUOUS)
			suffix[l++]='C' ;
	    else suffix[l++]='?' ;

		
		suffix[l] = 0 ; /* terminate string */

	    printf("%s-%g%s\t", Data_Dictionary[k].name,
			1+Data_Dictionary[k].dom_max-Data_Dictionary[k].dom_min, suffix ) ;
	  }	  

	printf("   %s\n", class_name) ;

    } /* verbose banner presented */

    else if (column_banner) {
	/********************************************
	** A plain attribute banner is the default **
	********************************************/
	for (k=0, l=0; k<attributes; k++, l++)
	    if ( !(Data_Dictionary[k].masked) )
		printf("%s\t", Data_Dictionary[k].name ) ;

	printf("%s\n", class_name) ;
    }
  
    fflush(stdout) ;



    /***************************************************************
    ** Create objects one at a time.                              **
    ** ensure that only one rule could have created this object.  **
    ***************************************************************/
    object_failures = 0 ;
    for (i=0; i<objects; i++) {
		struct Terms	*Term ;
		object			new_object ;
		int				New_object_ok=0 ; /* assume not okay */


	/******************
	** Select a Rule **
	******************/

	/* First, is there a default rule? */
	if (n_rand(1.0) < default_rule) {
	   j = 0 ; /* 0 is the default */
	   if(debug)
		   fprintf(stderr, "DEBUG: use default rule [%d].\n", j) ;
	}
	/* select a rule from the rule base*/
	else {
	   if (rule_distr == UNIFORM_DISTRIBUTION ) {
	        if(debug) fprintf(stderr, "DEBUG: rule [%d] (uniform distribution).\n", j) ;
			j =  1 + (i % cnf_rules) ;
		}

	   else if (rule_distr == RANDOM_DISTRIBUTION ) {
		/* Select a random rule */
		   j= 1 + int_rand(cnf_rules) ;
		   if(debug) fprintf(stderr, "DEBUG: rule [%d] (rand distribution).\n", j) ;
		}

	   else /* Select a random rule with bias*/ {
		   j= 1 + (int)(cnf_rules*sn_rand()) ;
		   if(debug) fprintf(stderr, "DEBUG: rule [%d] (biased rand distribution).\n", j) ;
		}
	}

	/* while a valid object for this rules has not been created */
	while (New_object_ok==0) {


	/************************************************
	** Create an object which abides by this rule. **
	************************************************/
	Term=Rules[j].body ;
	for (k=0; k<attributes; k++) {

	    /* Is this attribute represented in this rule? */
	    if (Rules[j].attribute_map[k]==1) { /* YES */
            /* Pick a value for this attribute */

		if (Data_Dictionary[k].datatype == NOMINAL) {
			new_object[k] = (float)Term->nominal[int_rand(Term->setsize )] ;
		}
		else if (Data_Dictionary[k].datatype == ORDINAL) {
			new_object[k] = (float)(Term->ordinal[0] + 
				int_rand(1 + Term->ordinal[1] - Term->ordinal[0])) ;
				/* added +1 to include the max value */
		}
		else if (Data_Dictionary[k].datatype == CONTINUOUS) {
			new_object[k] = (float)(Term->continuous[0] + 
				(n_rand()*(Term->continuous[1]-Term->continuous[0])) ) ;
		}
		else {
		   fprintf(stderr, "\nERROR 19274494.\n") ;
		   exit(3) ;
		}

		/* Go to the next term in the rule */
		Term=Term->next_term ;
	    }

	    else { /* Not covered by a rule so randomly choose a value */

		if (debug) fprintf(stderr, " randval[%d] ", k) ;

		if (Data_Dictionary[k].datatype == NOMINAL) {
			int random_nominal = 1 + int_rand((int)Data_Dictionary[k].dom_max) ;

			new_object[k] = (float)random_nominal ;
			if (debug) fprintf(stderr, " nom[%d] ", random_nominal) ;

		}
		else if (Data_Dictionary[k].datatype == ORDINAL) {
			int random_ordinal ;

			/* add +1 to include the dom_max value */
			random_ordinal = int_rand((int)(1 + Data_Dictionary[k].dom_max-Data_Dictionary[k].dom_min)) ;
			random_ordinal += (int)Data_Dictionary[k].dom_min ;

			new_object[k] = (float)random_ordinal ;
			if (debug) fprintf(stderr, " ord[%d] ", random_ordinal) ;
		}
		else if (Data_Dictionary[k].datatype == CONTINUOUS) {

			new_object[k] =
				(float)Data_Dictionary[k].dom_min + (float)n_rand(1.0) *
				(Data_Dictionary[k].dom_max - Data_Dictionary[k].dom_min) ;

		}
		else {
			   fprintf(stderr, "\nERROR 294489473.\n") ;
			   exit(3) ;
		}

	    } /* open attribute for this rule */

	    /* This may be a missing attribute-value */
		/* If so then reset value to MISSINGVAL */
	    if ( miss_ratio > n_rand() ) {
			new_object[k] = MISSINGVAL ;
	    }

	} /* create each object's attribute */


	/*******************************************************************
	** Test that this new tuple could not be created by another rule. **
	*******************************************************************/
	New_object_ok = 1 ;
	for (n=1; n<=cnf_rules; n++) { /* for each subrule n */
	  if (n != j) { /* avoid self */

		int matches = 0 ; /* count the mis/matches for this rule */
		int mismatches = 0 ;
		struct Terms	*Term ;

		Term = Rules[n].body ;
				
		/* Could the object be created by this other rule? */
		/* Test each term in the subrule */
		for (m=0; m<attributes; m++) {
			/* Does the rule have a term on this attribute? */
			if (Rules[n].attribute_map[m]==1) {
				int k ;
			
				if (Data_Dictionary[m].datatype == NOMINAL) {
				    /* test whether this value is contained in the set */
				    int nominal_match=0 ;
				    for (k=0;k<=Term->setsize;k++) {
					if ((int)new_object[m] == Term->nominal[k])
						nominal_match++ ;
				    }
				    if (nominal_match) matches++ ;
				    else mismatches++ ;
				}

				else if (Data_Dictionary[m].datatype == ORDINAL) {
					if ((new_object[m] >= (float)Term->ordinal[0])
							&& (new_object[m] <= (float)Term->ordinal[1]))
						matches++ ;
					else 
						mismatches++ ;
				}

				else if (Data_Dictionary[m].datatype == CONTINUOUS) {
					if ((new_object[m] >= Term->continuous[0])
							&& (new_object[m] <= Term->continuous[1]))
						matches++ ;
					else 
						mismatches++ ;
				}

			Term = Term->next_term ;
			}

		} /* for each attribute */


		if (debug) fprintf(stderr,
			"\ndebug: matches [%d] && ! mismatches [%d] = %d\n",
			matches, mismatches, matches && ! mismatches) ;

		if (matches && ! mismatches) {
		    New_object_ok = 0 ; /* FALSE */
		    if (debug) fprintf(stderr,
				"WARN: Bad new object #%d matches rule %d!\n", i+1, k+1) ;
			/* terminate the loops */
		    n=cnf_rules ;		
		}
	    
	  } /* avoid self */

	} /* test each rule k */


	if (debug) {
		int l ;
		for (l=0;l<attributes;l++)
			fprintf(stderr,"%d[%g] ", l, new_object[l]) ;
		fprintf(stderr,"\n") ;
	}
		


	/**************************************************************
	** If the new object is OK then PRINT IT otherwise try again **
	**************************************************************/
	if (New_object_ok) {
	  /* update the number of objects for this rule */
	  Rules[j].objects++ ;

	  /* Display the object id */
	  if(verbose) fprintf(stdout, "%5d:\t\t", i+1) ;

	  /* Cycle through each attribute */
	  for (k=0; k<attributes; k++) {

	    /* Hold string NOMINAL values */
        char buffer[256] ;

		if (debug) printf("%d|", k) ;

		/* This attribute is masked */
		if (Data_Dictionary[k].masked) {
		    if (verbose) printf("   *\t" ) ;
		}

		/* supply a rule-independent (erroneously entered) attribute-value */
		else if ( attrib_error > n_rand( 1.0 ) ) {
		  if (debug) printf("er[%d] ", 
			  Data_Dictionary[k].datatype) ;

		  if (Data_Dictionary[k].datatype == NOMINAL) {

			int rand_val = 1 + int_rand((int)Data_Dictionary[k].dom_max) ;

		    num2str(rand_val, buffer) ;

			if (verbose)	printf("%4s*\t", buffer ) ;
			else			printf("%s\t", buffer ) ;
		  }
/*sss*/
		  else if (Data_Dictionary[k].datatype == ORDINAL) {
			int rand_val = (int)Data_Dictionary[k].dom_min
						+ int_rand((int)Data_Dictionary[k].dom_max - (int)Data_Dictionary[k].dom_min) ;

			if (verbose)	printf("%4d*\t", rand_val ) ;
			else			printf("%d\t" , rand_val ) ;
		  }

	
		  else if (Data_Dictionary[k].datatype == CONTINUOUS) {
			float rand_val = Data_Dictionary[k].dom_min + (float)(n_rand(1.0) *
				(Data_Dictionary[k].dom_max - Data_Dictionary[k].dom_min)) ;

			if (verbose)	printf("%5.2g*\t", rand_val ) ;
			else			printf("%5.2g\t" , rand_val ) ;
		  }

		  
		  else {
			fprintf(stderr, "\nERROR 294489473.\n") ;
			exit(2) ;
		  }

		
		} /* an erroneously entered value */


		/* missing attribute-value */
		else if (new_object[k] == MISSINGVAL) {
		  if (debug) printf("mi[%d] ", 
			  Data_Dictionary[k].datatype) ;

		  if (verbose)
			printf("%4s\t", MISSINGVALCHAR ) ;
		  
		  else
			printf("%s\t", MISSINGVALCHAR) ;
		}

		/* all hurdles were passed */
		else {

			if (debug) printf("ok[%d] ",
				Data_Dictionary[k].datatype) ;

			if (Data_Dictionary[k].datatype == NOMINAL) {
				if (verbose) {
					num2str((int)new_object[k], buffer) ;
					printf("%4s\t", buffer ) ;
				}

				else {
					num2str((int)new_object[k], buffer) ;
					printf("%s\t", buffer ) ;
			    }
			}

		
			else if (Data_Dictionary[k].datatype == ORDINAL) {	
				if (verbose) {
					printf("%4d\t", (int)new_object[k] ) ;
				}
				else {
					printf("%d\t", (int)new_object[k] ) ;
				}
			}
	    
			else if (Data_Dictionary[k].datatype == CONTINUOUS) {	
				if (verbose) {
					printf("%5.2g\t", new_object[k] ) ;
				}
				else {
					printf("%g\t", new_object[k] ) ;
				}
			}
			else { /* ERROR */
					fprintf(stderr, "ERROR: unknown condition 9359732 [%d].\n",
						Data_Dictionary[j].datatype) ;
					exit(3) ;
			}

	    
		} /* Printed this attribute's value */

		} /* Cycled through the attributes */


	    /* Finally, report the class value */
	    {   int class = Rules[j].tail ;

			/* erroneously entered class */
			if ( class_error > n_rand() ) {
			    int rand_val = int_rand(classes+1);
			    class=rand_val ; /* overwrite the correct class */
			}
	
			if (verbose)
				printf("%5s%d\n", "c", class ) ;
			else	
				printf("c%d\n", class ) ;
		}
	}

	else { /* This object could have been created by another rule */

	    if (object_failures++ > FAILURES_PER_OBJECT * objects) {
	        /* FAIL: Recreation of this object has occurred too often */
			fprintf(stderr, 
				"\nEXCEPTION:\n\tFailed to create all the requested objects.\n") ;
			fprintf(stderr, 
				"\tThis domain appears to be too constrained!\n\n") ;
			exit(1) ;
	    }
	} /* object create by another rule */

    } /* while an object for this rule has not been created */

    } /* for each object i */




   /*********************************************************************
   ** Display Rules (when verbose)
   *********************************************************************/
    
   if (verbose || rule_fd) {
	float r ;
	int firstA, firstB ;
	FILE	*stream ;

	if (debug) fprintf(stderr, "\nDEBUG: Display Rules {\n");

	if (rule_fd)
		stream = rule_fd ;
	else
		stream = stdout ;

	/* Sort the rules based on their data set representation */
	qsort(Rules, cnf_rules, sizeof(struct CNF_Rule), compare_rule_freq) ;

	if (verbose) fprintf(stream,
		"\n\nRULES\n\t(activation%%) class <- class description\n\n");



	

	
	
	
		/* Report each rule's composition and performance */
	for (i=0; i<=cnf_rules; i++) {
	    struct Terms	*Term ;

	  /* if this is not the default rule or if it is the 
		default rule that a default rule was specified */
	  if (Rules[i].default_rule==0 || default_rule>0.0) {

        /* print rule id */
	    if (debug) fprintf(stderr, "  Rule %d\n", i);

		/* calculate the rule's activation frequency */
		r=(float)Rules[i].objects ;
		r/=objects ;

	    if (verbose) fprintf(stream, "\t") ;
	    
		/* (percent) class <- */
	    fprintf(stream, "(%02.1f%%) c%d <- ",
			r*100, Rules[i].tail) ;

	    if (Rules[i].default_rule==1)
			fprintf(stream, " default ") ;

		/* for each attribute   used by the rule */
	    Term=Rules[i].body ;


	    for (j=0, firstA=1; j<attributes; j++) {
		  if (Rules[i].attribute_map[j]==1) {

			/* time for an ampersand & */
			if (firstA) firstA=!firstA ;
		    else fprintf(stream, " & ") ;

			/* print the attribute's name */
		    fprintf(stream, "%s", Data_Dictionary[j].name) ;

		    /* show that this was a masked attribute */
		    if (Data_Dictionary[j].masked) fprintf(stream, "*" ) ;

			if (Data_Dictionary[j].datatype == NOMINAL) {
				char buffer[256] ;
				
				fprintf(stream, "=") ;

				if (debug) fprintf(stream, "%d", Term->setsize);

			    /* add parentheses if this is a disjuntive term */
			    if (Term->setsize > 1) fprintf(stream, "{") ;

				/* for each value in the term's set */
			    for (k=0,firstB=1; k<Term->setsize; k++) {

					if (firstB) firstB=!firstB ;
					else fprintf(stream, "," ) ;

					num2str(Term->nominal[k], buffer) ;
					fprintf(stream,"%s", buffer ) ;
				}
				/* conclude the set if there were several values */
			    if (Term->setsize > 1) fprintf(stream, "}" ) ;
			}
			
			else if (Data_Dictionary[j].testtype == TWOSIDED) {

			   if (Data_Dictionary[j].datatype == ORDINAL) {
				if (Term->setsize == 1)
					fprintf(stream, "=%d", Term->ordinal[0]) ;
				else
					fprintf(stream, "=[%d,%d]", Term->ordinal[0], Term->ordinal[1] ) ;
			   }
			   else
						fprintf(stream, "=[%g,%g]", Term->continuous[0], Term->continuous[1] ) ;
			}

			else if (Data_Dictionary[j].testtype == ONESIDED) {

				if (Term->lessthan) {
					fprintf(stream, "<=") ;
					if (Data_Dictionary[j].datatype == ORDINAL)
						fprintf(stream, "%d", Term->ordinal[1] ) ;
					else
						fprintf(stream, "%f", Term->continuous[1] ) ;
				}
				else {
					fprintf(stream, ">=") ;
					if (Data_Dictionary[j].datatype == ORDINAL)
						fprintf(stream, "%d", Term->ordinal[0] ) ;
					else
						fprintf(stream, "%f", Term->continuous[0] ) ;
				}

			}

			else { /* ERROR */
					fprintf(stderr, "ERROR: unknown condition 84792740 [%d][%d].\n",
						Data_Dictionary[j].datatype, Data_Dictionary[j].testtype) ;
					exit(3) ;
				}
			
			
			Term=Term->next_term ;
		  }

		} /* for every term */





	    fprintf(stream, "\n" ) ;
	  }
	} /* foreach rule */






	
	/* Conclude printin of rule base */
	if (verbose) fprintf(stream, "\n\n") ;
	fflush(stream) ;
	if (rule_fd) fclose(rule_fd) ;
 	if (debug) fprintf(stderr, "} Finished Rule Display %d\n", i);

   } /* print rule base */


   if (debug) fprintf(stderr, "\nAbout to exit\n", i);




   exit(0) ;

} /* end of main() */










/*****************************************************************
******************************************************************
** SUPPORT PROCEDURES 						**
******************************************************************
*****************************************************************/

/*************************************
** Compare the number of objects of two rules.
*************************************/
int compare_rule_freq(struct CNF_Rule *i, struct CNF_Rule *j)
{
        return (j->objects - i->objects);
}



/*************************************
** Compare two integers.
*************************************/
int compare_int(int *i, int *j)
{
        return (*i - *j);
}


/*************************************
** Compare two real numbers.
*************************************/
float compare_float(float *i, float *j)
{
        return (*i - *j);
}


/*************************************
** Return a random real over interval (0.0, 1.0)
*************************************/
double n_rand() {
	/* srand()/drand() are for UNIX System V. */
	return(drand48()) ;
}


/***************************************
** Return a random integer y in [0,x) **
***************************************/
int int_rand(int x) {
	double y=0 ;

  /* Function drand48() return double-precision floating-point
     values uniformly distributed over the interval [0.0, 1.0). */
	y = drand48() ;
	y *= x ;
	return((int)y) ;
}


/***************************************
** Return a random float y from [0,x) **
***************************************/
float flt_rand(int x) {
	double y=0 ;

  /* Function drand48() return double-precision floating-point
     values uniformly distributed over the interval [0.0, 1.0). */
	y = drand48() ;
	y *= x ;
	return((float)y) ;
}


/*****************************************************************************
** sn_rand()
**
** Return a random real > 0 and < 1 with a standard normal (gaussian) distribution.
** Based on GASDEV() from "Numerical Recipes" (1986) p.203
*****************************************************************************/
double sn_rand() {
   double	v1, v2 ;
   double	r, fac, val ;

   v1 = fabs(2*drand48() - 1) ;
   v2 = fabs(2*drand48() - 1) ;
   r =  pow(v1,2.0) + pow(v2,2.0) ;
   while (r >= 1.0) {
	v1=fabs(2*drand48() - 1) ;
	v2=fabs(2*drand48() - 1) ;
	r=pow(v1,2.0)+pow(v2,2.0) ;
   }

   fac = sqrt(-2 * log(r)/r) ;
   val = v1 * fac / 2.5 ;
   val = val - floor(val) ;
   /* if (debug) fprintf(stderr, "sn_rand: %f * %03.3f / 3 = %f\n",v1, fac, val) ; */
   return(val) ;

}
	

/*****************************************************************************
** num2str()
**
** Convert a number to a string using the mapping below
**
** BUG: tokens with more than 5 characters will not follow the sequence.
**
*****************************************************************************/
/*
1	a
2	b
...
25	y
26	z
27	aa
28	ab
...
51	ay
52	az
53	ba
54	bb
...
701	zy
702	zz
703	aaa
704	aab
...
*/

int num2str(int number, char buffer[256]) {
   int  	i		; /* digit counter */
   int  	digit		; /* value 0 is digit #1 */
   float	digitorder	; /* order of digit. in decimal the 3rd digit has order 100 */
   int  	digitvalue	;
   int  	workingnum=number	;
   float	fdigits	;
   int  	adj1,adj2,adj3,adj4 ;

   /* if (debug) fprintf(stderr, "debug: num2str(%d,%d)\n", number, buffer) ; */

   /* Adjusts for the transition from z to aa */
   /* Similar to the transition from 9 to 11 instead of 9 to 10 */
   /* Do this by adding the right amount as the number given gets bigger */
   /* BUG: Only 4 adjustments are made. */
   adj1 = (int)(number-1)/(26) ;
   adj2 = 27*(int)((number-1-26)/(26*26)) ;
   if (adj2<0) adj2=0 ;
   adj3 = 27*27*(int)((number-1-26-26*26)/(26*26*26)) ;
   if (adj3<0) adj3=0 ;
   adj4 = 27*27*27*(int)((number-1-26-26*26-26*26*26)/(26*26*26*26)) ;
   if (adj4<0) adj4=0 ;

   /* debug printf("adj1=%d adj2=%d adj3=%d adj4=%d\n", adj1, adj2, adj3, adj4) ; */

   workingnum = number + adj1 + adj2 + adj3 + adj4 ;

   fdigits = (float)(log(workingnum)/log('z'-'a'+2)) ; /* how many digits */

   digit = (int)fdigits ; /* point to the first digit */

   /* process each digit */
   i=0 ;
   while(i<=fdigits) {

	digitorder = (float)pow(27,digit) ;

	/* calculate the value of this digit */
	digitvalue = (int)(workingnum/digitorder) ;

      /* 
	if (debug) printf("working %d digit %d, order %3.1f, value %d i %d\n",
		workingnum, digit, digitorder, digitvalue, i) ;
      */

	buffer[i]='a'+(digitvalue-1) ;

	/* calculate the remainder */
	workingnum = workingnum - (int)(digitvalue*digitorder) ;

	/* move on to the next digit */
	digit-- ;
	i++ ;

   } /* for each digit */


   /* terminate the string */
   buffer[i]=(char)0 ;

   return(0) ;
}

