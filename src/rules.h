

#include "sagan-ng-defs.h"

#define         VALID_RULE_OPTIONS "signature_id"

#define         MAX_RULE_SIZE                   4096
#define		MAX_RULE_DESCRIPTION		256
#define		MAX_RULE_CLASSIFICATION		32
#define         MAX_RULE_CLASSIFICATION_DESC    96
#define		MAX_RULE_REFERENCE		2048
#define		MAX_RULE_SEARCH			25

#define		SEARCH_STRING			128


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

    char search_string[MAX_RULE_SEARCH][SEARCH_STRING];
    char search_key[MAX_RULE_SEARCH][MAX_JSON_KEY];
    bool search_type[MAX_RULE_SEARCH];
    bool search_case[MAX_RULE_SEARCH];
    bool search_not[MAX_RULE_SEARCH];
    uint8_t search_string_count;


};


void Load_Ruleset( const char *ruleset );
