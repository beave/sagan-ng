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

#include "processors/engine.h"

struct _Rules *Rules;
struct _Counters *Counters;



void Engine( struct _JSON_Key_String *JSON_Key_String, uint16_t json_count )
{

    uint32_t rule_position = 0;
    bool results = false;

    for ( rule_position = 0; rule_position < Counters->rules; rule_position++ )
        {

            results = false;

            /* Do we need to do a "search" */

            if ( Rules[rule_position].search_string_count != 0 )
                {
                    results = Search ( rule_position, json_count, JSON_Key_String);
                }


            if ( results == true )
                {
                    printf("%d ***** Fire event! ******\n", results);
                }
            else
                {
                    printf("- MISS -\n");
                }

//	printf("Results: %d\n", results);

        }


}

