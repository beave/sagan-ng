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
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#ifdef HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#include "version.h"
#include "sagan-ng-defs.h"
#include "sagan-ng.h"
#include "sagan-config.h"

/* Notes: 

   Command line...  (need --config option)
   Move *Config array to config-yaml.c? Or at least clear it there?
   Load config
   Signal Handler

*/

struct _SaganConfig *Config = NULL;


int main(int argc, char **argv)
{

#ifdef HAVE_SYS_PRCTL_H
    (void)SetThreadName("SaganMain");
#endif

    /* Allocate memory for global struct _SaganConfig */
/*
    Config = malloc(sizeof(_SaganConfig));

    if ( Config == NULL )
        {
            Sagan_Log(ERROR, "[%s, line %d] Failed to allocate memory for config. Abort!", __FILE__, __LINE__);
        }

    memset(Config, 0, sizeof(_SaganConfig));

*/

//Load_YAML_Config(); 


}
