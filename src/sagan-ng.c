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
#include <unistd.h>
#include <getopt.h>

#ifdef HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#include "version.h"
#include "sagan-ng-defs.h"
#include "sagan-ng.h"
#include "sagan-config.h"
#include "counters.h"
#include "debug.h"

/* Notes:

   Command line...  (need --config option)
   Move *Config array to config-yaml.c? Or at least clear it there?
   Load config
   Signal Handler

*/

struct _Config *Config = NULL;
struct _Counters *Counters = NULL;
struct _Debug *Debug = NULL;


int main(int argc, char **argv)
{

#ifdef HAVE_SYS_PRCTL_H
    (void)SetThreadName("SaganMain");
#endif

    signed char c = 0;


    /* Allocate memory for global struct _Config */

    Config = malloc(sizeof(_Config));

    if ( Config == NULL )
        {
            Sagan_Log(ERROR, "[%s, line %d] Failed to allocate memory for _Config. Abort!", __FILE__, __LINE__);
        }

    memset(Config, 0, sizeof(_Config));

    /* Allocate memory for global struct _Counters */

    Counters = malloc(sizeof(_Counters));

    if ( Counters == NULL )
        {
            Sagan_Log(ERROR, "[%s, line %d] Failed to allocate memory for _Counters. Abort!", __FILE__, __LINE__);
        }

    memset(Counters, 0, sizeof(_Counters));

    /* Allocate memory for global struct _Debug */

    Debug = malloc(sizeof(_Debug));

    if ( Debug == NULL )
        {
            Sagan_Log(ERROR, "[%s, line %d] Failed to allocate memory for _Debug. Abort!", __FILE__, __LINE__);
        }

    memset(Debug, 0, sizeof(_Debug));

    /**********************************************************************
     * Defaults
     **********************************************************************/

    strlcpy(Config->config_yaml, CONFIG_FILE_PATH, sizeof(Config->config_yaml));   /* From config.h */


    /**********************************************************************
     * Command line
     **********************************************************************/

    const struct option long_options[] =
    {
        { "help",         no_argument,          NULL,   'h' },
        { "debug",        required_argument,    NULL,   'd' },
        { "daemon",       no_argument,          NULL,   'D' },
        { "user",         required_argument,    NULL,   'u' },
        { "chroot",       required_argument,    NULL,   'C' },
        { "credits",      no_argument,          NULL,   'X' },
        { "config",       required_argument,    NULL,   'c' },
        { "log",          required_argument,    NULL,   'l' },
        { "quiet",        no_argument,          NULL,   'q' },
        {0, 0, 0, 0}
    };

    static const char *short_options =
        "l:f:u:d:c:pDhCQ";

    int option_index = 0;

    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
        {

            switch(c)
                {

                case 'h':
                    //               Usage();
                    exit(0);
                    break;

                case 'C':
                    //              Credits();
                    exit(0);
                    break;

                case 'q':
                    Config->quiet = true;
                    break;

                case 'D':
                    Config->daemonize = true;
                    break;

                case 'd':

                    if (Sagan_strstr(optarg, "config"))
                        {
                            Debug->config = true;
                        }

                    break;


                default:
                    fprintf(stderr, "Invalid argument! See below for command line switches.\n");
                    //             Usage();
                    exit(0);
                    break;

                }
        }


    Load_YAML_Config( Config->config_yaml );


}
