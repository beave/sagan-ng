
#define         VALID_RULE_OPTIONS "signature_id"


#define         MAX_RULE_SIZE                   4096
#define		MAX_RULE_DESCRIPTION		256


typedef struct _Rules _Rules;
struct _Rules
{

    uint64_t signature_id;
    uint16_t revision;
    char description[MAX_RULE_DESCRIPTION];

};


void Load_Ruleset( const char *ruleset );
