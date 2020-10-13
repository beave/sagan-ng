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
#include <stdint.h>
#include <pthread.h>

#ifdef HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#include "sagan-ng-defs.h"
#include "util.h"
#include "rules.h"
#include "counters.h"
#include "parsers/json.h"
#include "parsers/search.h"

#include "processors/engine.h"

struct _Rules *Rules;
struct _Counters *Counters;


bool Search( uint32_t rule_position, uint16_t json_count, struct _JSON_Key_String *JSON_Key_String)
{

    uint8_t i = 0;
    uint8_t k = 0;
    uint16_t a = 0;
    uint8_t total_hits = 0;

    bool flag = false;

    /* Total "search" items rules */

    for ( i = 0; i < Rules[rule_position].search_string_count; i++ )
        {

//            if ( Rules[rule_position].search_not[i] == false )
//                {
            /* Go through JSON */

            for ( a = 0; a < json_count; a++ )
                {
                    /* Look for target key */

                    if ( !strcmp(JSON_Key_String[a].key, Rules[rule_position].search_key[i]) )
                        {
                            /* Is search "contacts" or "exact" ? */

                            if ( Rules[rule_position].search_type[i] == SEARCH_TYPE_CONTAINS )
                                {
                                    /* Cycle through the # of searches (could be a list) */

                                    for ( k = 0; k < Rules[rule_position].search_count[i]; k++ )
                                        {

                                            if ( Rules[rule_position].search_case[i] == false )
                                                {
                                                    /* Case insensitive */

                                                    if ( Sagan_stristr( JSON_Key_String[a].json, Rules[rule_position].search_string[i][k], true ) )
                                                        {

                                                            if ( Rules[rule_position].search_not[i] == false )
                                                                {
                                                                    total_hits++;
                                                                    break;
                                                                }
                                                            else
                                                                {
                                                                    return(false);
                                                                }

                                                        }

                                                }
                                            else
                                                {
                                                    /* Case sensitive */

                                                    if ( Sagan_strstr( JSON_Key_String[a].json, Rules[rule_position].search_string[i][k], true ) )
                                                        {
                                                            if ( Rules[rule_position].search_not[i] == false )
                                                                {
                                                                    total_hits++;
                                                                    break;
                                                                }
                                                            else
                                                                {
                                                                    return(false);
                                                                }
                                                        }
                                                }

                                        } /* for ( k = 0; k < Rules[rule_position].search_count[i] */

                                    /* If exclude and not found, its a "hit" */

                                    if ( Rules[rule_position].search_not[i] == true )
                                        {
                                            total_hits++;
                                        }

                                }
                            else
                                {

                                    /* Exact match */

                                    for ( k = 0; k < Rules[rule_position].search_count[i]; k++ )
                                        {

                                            if ( Rules[rule_position].search_case[i] == false )
                                                {
                                                    /* Case insensitive */

                                                    if ( !strcasecmp( JSON_Key_String[a].json, Rules[rule_position].search_string[i][k]) )
                                                        {

                                                            if ( Rules[rule_position].search_not[i] == false )
                                                                {
                                                                    total_hits++;
                                                                    break;
                                                                }
                                                            else
                                                                {
                                                                    return(false);
                                                                }
                                                        }
                                                }
                                            else
                                                {
                                                    /* Case sensitive */

                                                    if ( !strcmp( JSON_Key_String[a].json, Rules[rule_position].search_string[i][k] ) )
                                                        {

                                                            if ( Rules[rule_position].search_not[i] == false )
                                                                {
                                                                    total_hits++;
                                                                    break;
                                                                }
                                                            else
                                                                {
                                                                    return(false);
                                                                }
                                                        }
                                                }

                                        } /* for ( k = 0; k < Rules[rule_position] */

                                    /* If exclude and not found, its a "hit" */

                                    if ( Rules[rule_position].search_not[i] == true )
                                        {
                                            total_hits++;
                                        }
                                }
                        }

                } /* for ( a = 0; a < json_count */

        }

//    printf("Total hits: %d of %d\n", total_hits, Rules[rule_position].search_string_count);

    if ( total_hits == Rules[rule_position].search_string_count )
        {
//	    printf("RETURN TRUE\n");
            return(true);
        }

    return(false);
}


