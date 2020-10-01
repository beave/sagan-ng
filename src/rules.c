/* $Id$ */
/*
** Copyright (C) 2009-2020 Quadrant Information Security <quadrantsec.com>
** Copyright (C) 2009-2020 Champ Clark III <cclark@quadrantsec.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#include <stdio.h>
#include <stdlib.h>

#include "sagan-ng-defs.h"
#include "sagan-ng.h"
#include "sagan-config.h"
#include "batch.h"
#include "util.h"
#include "rules.h"
#include "counters.h"

#include "parsers/json.h"

struct _Counters *Counters;

struct _Rules *Rules = NULL;


void Load_Ruleset( const char *ruleset )
{

    uint16_t i = 0;
    uint16_t a = 0;

    char tmpkey[MAX_JSON_KEY] = { 0 };
    uint16_t search_count = 0;

    uint16_t json_count = 0;
    uint16_t line_count = 0;

    char rulebuf[MAX_RULE_SIZE] = { 0 };

    FILE *rulesfile;

    struct _JSON_Key_String *JSON_Key_String;

    JSON_Key_String = malloc(sizeof(_JSON_Key_String) * MAX_JSON_NEST );

    if ( JSON_Key_String == NULL )
        {
            Sagan_Log(ERROR, "[%s, line %d] Failed to allocate memory for _JSON_Key_String", __FILE__, __LINE__);
        }

    if (( rulesfile = fopen(ruleset, "r" )) == NULL )
        {
            Sagan_Log(ERROR, "[%s, line %d] Cannot open rule file (%s - %s)", __FILE__, __LINE__, ruleset, strerror(errno));
        }

    /* Rule set tracking here? */

    Sagan_Log(NORMAL, "Loading %s rule file.", ruleset);

    while ( fgets(rulebuf, sizeof(rulebuf), rulesfile) != NULL )
        {

            line_count++; 	/* For error displays */


            if (rulebuf[0] == '#' || rulebuf[0] == 10 || rulebuf[0] == ';' || rulebuf[0] == 32)
                {

                    continue;

                }
            else
                {

                    /* Allocate memory for rules, but not comments */

                    Rules = (_Rules *) realloc(Rules, (Counters->rules+1) * sizeof(_Rules));

                    if ( Rules == NULL )
                        {
                            Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for _Rules. Abort!", __FILE__, __LINE__);
                        }

                    memset(&Rules[Counters->rules], 0, sizeof(struct _Rules));

                }

            Remove_Return(rulebuf);


            json_count = Parse_JSON( rulebuf, JSON_Key_String);

            if ( json_count == 1 )
                {
                    Sagan_Log(ERROR, "[%s, line %d] Failed to parse rule in %s at line %d", __FILE__, __LINE__, ruleset, line_count);
                }

            /****************************************************************
                 * Non-nested one off items ( .signature_id, .reference, etc
             ****************************************************************/

            for ( i = 0; i < json_count; i++ )
                {

                    printf("[%d] Key: %s, Value: %s\n", i, JSON_Key_String[i].key, JSON_Key_String[i].json);


                    if ( !strcmp( JSON_Key_String[i].key, ".signature_id" ) )
                        {

                            Rules[Counters->rules].signature_id = atol(JSON_Key_String[i].json);

                            if ( Rules[Counters->rules].signature_id == 0 )
                                {
                                    Sagan_Log(ERROR, "[%s, line %d] Invalid 'signature_id' in %s at line %d", __FILE__, __LINE__, ruleset, line_count);
                                }

                        }

                    else if ( !strcmp( JSON_Key_String[i].key, ".revision" ) )
                        {

                            Rules[Counters->rules].revision = atol(JSON_Key_String[i].json);

                            if ( Rules[Counters->rules].revision == 0 )
                                {
                                    Sagan_Log(ERROR, "[%s, line %d] Invalid 'revision' in %s at line %d", __FILE__, __LINE__, ruleset, line_count);
                                }

                        }

                    else if ( !strcmp( JSON_Key_String[i].key, ".description" ) )
                        {
                            strlcpy( Rules[Counters->rules].description, JSON_Key_String[i].json, MAX_RULE_DESCRIPTION);
                        }

                    else if ( !strcmp( JSON_Key_String[i].key, ".classification" ) )
                        {

                            strlcpy( Rules[Counters->rules].classification, JSON_Key_String[i].json, MAX_RULE_CLASSIFICATION);

                            if ( Classtype_Lookup( Rules[Counters->rules].classification, Rules[Counters->rules].classification_desc, MAX_RULE_CLASSIFICATION_DESC ) == -1 )
                                {
                                    Sagan_Log(ERROR, "[%s, line %d] Error: Could find classification of '%s' in %s at line %d. Abort.", __FILE__, __LINE__, Rules[Counters->rules].classification, ruleset, line_count);
                                }


                        }

                    else if ( !strcmp( JSON_Key_String[i].key, ".normalize" ) )
                        {
                            strlcpy( Rules[Counters->rules].normalize, JSON_Key_String[i].json, MAX_JSON_KEY);
                        }

                    else if ( !strcmp( JSON_Key_String[i].key, ".reference" ) )
                        {
                            strlcpy( Rules[Counters->rules].reference, JSON_Key_String[i].json, MAX_RULE_REFERENCE);
                        }

                }

            /* Sanity check! */

            if ( Rules[Counters->rules].classification[0] == '\0' )
                {
                    Sagan_Log(ERROR, "[%s, line %d] Error: No 'classification' specified in %s at line %d is invalid.  Abort.", __FILE__, __LINE__, ruleset, line_count);
                }

            if ( Rules[Counters->rules].description[0] == '\0' )
                {
                    Sagan_Log(ERROR, "[%s, line %d] Error: No 'description' specified in %s at line %d is invalid.  Abort.", __FILE__, __LINE__, ruleset, line_count);
                }


            /****************************************************************
            * Get all 'search' optinos
            ****************************************************************/

            for ( i = 0; i < json_count; i++ )
                {

                    for ( a = 0; a < MAX_RULE_SEARCH; a++ )
                        {

                            snprintf(tmpkey, MAX_JSON_KEY, ".search.%d.string", a);
                            tmpkey[ sizeof(tmpkey) - 1] = '\0';

                            if ( !strcmp( JSON_Key_String[i].key, tmpkey ) )
                                {
                                    strlcpy(Rules[Counters->rules].search_string[a], JSON_Key_String[i].json, SEARCH_STRING);
                                    Rules[Counters->rules].search_string_count++;
                                }

                            snprintf(tmpkey, MAX_JSON_KEY, ".search.%d.parse_key", a);
                            tmpkey[ sizeof(tmpkey) - 1] = '\0';

                            if ( !strcmp( JSON_Key_String[i].key, tmpkey ) )
                                {
                                    strlcpy(Rules[Counters->rules].search_key[a], JSON_Key_String[i].json, MAX_JSON_KEY);
                                }

                            snprintf(tmpkey, MAX_JSON_KEY, ".search.%d.case", a);
                            tmpkey[ sizeof(tmpkey) - 1] = '\0';

                            if ( !strcmp( JSON_Key_String[i].key, tmpkey ) )
                                {

                                    if ( !strcmp( JSON_Key_String[i].json, "true" ) )
                                        {
                                            Rules[Counters->rules].search_case[a] = true;
                                        }

                                    if ( strcmp( JSON_Key_String[i].json, "true" ) && strcmp( JSON_Key_String[i].json, "false" ) )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] Error: Expected a 'search' 'case' of 'true' or 'false'  but got '%s' in %s at line %d.  Abort.", __FILE__, __LINE__, JSON_Key_String[i].json, ruleset, line_count);
                                        }

                                }

                            snprintf(tmpkey, MAX_JSON_KEY, ".search.%d.not", a);
                            tmpkey[ sizeof(tmpkey) - 1] = '\0';

                            if ( !strcmp( JSON_Key_String[i].key, tmpkey ) )
                                {

                                    if ( !strcmp( JSON_Key_String[i].json, "true" ) )
                                        {
                                            Rules[Counters->rules].search_not[a] = true;
                                        }

                                    if ( strcmp( JSON_Key_String[i].json, "true" ) && strcmp( JSON_Key_String[i].json, "false" ) )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] Error: Expected a 'search' 'not' of 'true' or 'false'  but got '%s' in %s at line %d.  Abort.", __FILE__, __LINE__, JSON_Key_String[i].json, ruleset, line_count);
                                        }

                                }


                            snprintf(tmpkey, MAX_JSON_KEY, ".search.%d.type", a);
                            tmpkey[ sizeof(tmpkey) - 1] = '\0';

                            if ( !strcmp( JSON_Key_String[i].key, tmpkey ) )
                                {

                                    if ( !strcmp( JSON_Key_String[i].json, "exact" ) )
                                        {
                                            Rules[Counters->rules].search_type[a] = SEARCH_TYPE_EXACT;
                                        }

                                    if ( !strcmp( JSON_Key_String[i].json, "contains" ) )
                                        {
                                            Rules[Counters->rules].search_type[a] = SEARCH_TYPE_CONTAINS;
                                        }

                                    if ( strcmp( JSON_Key_String[i].json, "exact" ) && strcmp( JSON_Key_String[i].json, "contains" ) )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] Error: Expected a 'search' 'type' of 'exact' or 'contains' but got '%s' in %s at line %d.  Abort.", __FILE__, __LINE__, JSON_Key_String[i].json, ruleset, line_count);
                                        }

                                }
                        }

                }

            /* Sanity Check! */

            for ( a = 0; a < Rules->search_string_count; a++ )
                {

                    if ( Rules[Counters->rules].search_key[a][0] == '\0' )
                        {
                            Sagan_Log(ERROR, "[%s, line %d] Error: `search` option lacks a 'parse_key` option in %s at line %d. Abort.", __FILE__, __LINE__, ruleset, line_count);
                        }
                }


            __atomic_add_fetch(&Counters->rules, 1, __ATOMIC_SEQ_CST);

        } /* while ( fgets(rulebuf .... */


    free(JSON_Key_String);
    fclose(rulesfile);

}

