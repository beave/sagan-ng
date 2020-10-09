

#include "sagan-ng-defs.h"

//#define         VALID_RULE_OPTIONS "signature_id"

#define         MAX_RULE_SIZE                   4096	/* Largest size a signature can be */
#define		MAX_RULE_DESCRIPTION		256	/* Max "description" / human readable size */
#define		MAX_RULE_CLASSIFICATION		32	/* Max short "classification" size */
#define         MAX_RULE_CLASSIFICATION_DESC    96	/* Max long "classification size */
#define		MAX_RULE_REFERENCE		2048	/* Make URL length for a reference */

#define		MAX_RULE_SEARCH			10	/* Max "search" in a rule */
#define		MAX_RULE_SEARCH_MASK		512	/* Max size of a search mask */
#define		MAX_SEARCH_STRING		128	/* Max items in a search string */
#define		MAX_SEARCH_STRING_SIZE		256	/* Max size of individual searches */


#define		SEARCH_TYPE_EXACT		0	/* Default */
#define		SEARCH_TYPE_CONTAINS		1


typedef struct _Rules _Rules;
struct _Rules
{

    uint64_t signature_id;
    uint16_t revision;
    char description[MAX_RULE_DESCRIPTION];
    char classification[MAX_RULE_CLASSIFICATION];
    char classification_desc[MAX_RULE_CLASSIFICATION_DESC];
    char normalize[MAX_JSON_KEY];
    char reference[MAX_RULE_REFERENCE];

    //char search_string[MAX_RULE_SEARCH][SEARCH_STRING];

    char search_string[MAX_RULE_SEARCH][MAX_SEARCH_STRING][MAX_SEARCH_STRING_SIZE];
    uint8_t search_count;

    char search_key[MAX_RULE_SEARCH][MAX_JSON_KEY];
    bool search_type[MAX_RULE_SEARCH];
    bool search_case[MAX_RULE_SEARCH];
    bool search_not[MAX_RULE_SEARCH];
    char search_mask[MAX_RULE_SEARCH_MASK];
    uint8_t search_string_count;


};


void Load_Ruleset( const char *ruleset );
