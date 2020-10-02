/* $Id$ */
/*
** Copyright (C) 2009-2020 Quadrant Information Security <quadrantsec.com>
** Copyright (C) 2009-2020 Champ Clark III <cclark@quadrantsec.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or                                    ** distribute this program under any other version of the GNU General                                        ** Public License.                                                                                           **                                                                                                           ** This program is distributed in the hope that it will be useful,
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
    uint16_t a = 0;

    for ( i = 0; i < Rules[rule_position].search_string_count; i++ )
        {

            for ( a = 0; a < json_count; a++ )
                {

                    if ( Rules[rule_position].search_not[i] == false )
                        {

                            if ( !strcmp(JSON_Key_String[a].key, Rules[rule_position].search_key[i]) )
                                {

                                    if ( Rules[rule_position].search_case[i] == false )
                                        {

                                            /* Case _insensitive_ search */

                                            if ( Rules[rule_position].search_type[i] == SEARCH_TYPE_CONTAINS )
                                                {

                                                    /* Search type is "contains" */

                                                    if ( !Sagan_stristr( JSON_Key_String[a].json, Rules[rule_position].search_string[i], true ) )
                                                        {
                                                            return(false);
                                                        }

                                                }
                                            else
                                                {

                                                    /* Search type is "exact" */

                                                    if ( strcasecmp( JSON_Key_String[a].json, Rules[rule_position].search_string[i] ) )
                                                        {
                                                            return(false);
                                                        }

                                                }

                                        }
                                    else
                                        {

                                            /* Case sensitive */

                                            if ( Rules[rule_position].search_type[i] == SEARCH_TYPE_CONTAINS )
                                                {

                                                    /* Rule type is "contains" */

                                                    if ( !Sagan_strstr( JSON_Key_String[a].json, Rules[rule_position].search_string[i] ) )
                                                        {
                                                            return(false);
                                                        }

                                                }
                                            else
                                                {

                                                    /* Rule type is "exact */

                                                    if ( strcmp( JSON_Key_String[a].json, Rules[rule_position].search_string[i] ) )
                                                        {
                                                            return(false);
                                                        }

                                                }
                                        }

                                } /* strcmp(JSON_Key_String[a].key ... */

                        } else {  /* Rules[rule_position].search_not[i] == false */

				printf("GOT NOT RULE\n");


				}
			

                } /* for ( a = 0; a < json_count; a++ ) */

        } /* for ( i = 0; i < Rules[rule_position].search_string_count */


    /* If everything lines up,  we have a full json_content match */

    return(true);

}


