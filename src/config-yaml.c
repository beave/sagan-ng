
/* NOTES:

   SENSOR AND CLSUTER NOT BEING PARSED!
   NO DEFAULTS BEING SET

*/




#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <libgen.h>

#include "sagan-ng.h"
#include "sagan-ng-defs.h"
#include "sagan-config.h"
#include "config-yaml.h"
#include "util.h"
#include "var.h"
#include "counters.h"
#include "debug.h"
#include  "rules.h"

#ifdef HAVE_LIBYAML
#include <yaml.h>
#endif

#ifndef HAVE_LIBYAML
libyaml is required.  Get it from your distribution repo or http://pyyaml.org/wiki/LibYAML
#endif

struct _Config *Config;
struct _Counters *Counters;
struct _Debug *Debug;

struct _Var *Var = NULL;


void Load_YAML_Config( const char *yaml_file )
{

    bool done = false;

    char var_to_value[MAX_VAR_VALUE_SIZE] = { 0 };

    struct stat filecheck;

    unsigned char type = 0;
    unsigned char sub_type = 0;
    unsigned char toggle = 0;

    char *tok = NULL;

    char last_pass[128] = { 0 };

    yaml_parser_t parser;
    yaml_event_t  event;

    /* For lock file path */

    char *lf1 = NULL;
    char *lf2 = NULL;
    char *dir = NULL;
    char *filename = NULL;


    if (stat(yaml_file, &filecheck) != false )
        {
            Sagan_Log(ERROR, "[%s, line %d] The configuration file '%s' cannot be found! Abort!", __FILE__, __LINE__, yaml_file);
        }

    FILE *fh = fopen(yaml_file, "r");

    if (!yaml_parser_initialize(&parser))
        {
            Sagan_Log(ERROR, "[%s, line %d] Failed to initialize the libyaml parser. Abort!", __FILE__, __LINE__);
        }

    if (fh == NULL)
        {
            Sagan_Log(ERROR, "[%s, line %d] Failed to open the configuration file '%s' Abort!", __FILE__, __LINE__, yaml_file);
        }

    /* Set input file */

    yaml_parser_set_input_file(&parser, fh);

    while(!done)
        {

            if (!yaml_parser_parse(&parser, &event))
                {

                    /* Useful YAML vars: parser.context_mark.line+1, parser.context_mark.column+1, parser.problem, parser.problem_mark.line+1, parser.problem_mark.column+1 */

                    Sagan_Log(ERROR, "[%s, line %d] libyaml parse error at line %d in '%s'", __FILE__, __LINE__, parser.problem_mark.line+1, yaml_file);

                }

            /* Document start */

            if ( event.type == YAML_DOCUMENT_START_EVENT )
                {

                    if ( Debug->config )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] YAML_DOCUMENT_START_EVENT", __FILE__, __LINE__);
                        }

                    yaml_version_directive_t *ver = event.data.document_start.version_directive;

                    if ( ver == NULL )
                        {
                            Sagan_Log(ERROR, "[%s, line %d] Invalid configuration file. Configuration must start with \"%%YAML 1.1\"", __FILE__, __LINE__);
                        }

                    int major = ver->major;
                    int minor = ver->minor;

                    if (! (major == YAML_VERSION_MAJOR && minor == YAML_VERSION_MINOR) )
                        {
                            Sagan_Log(ERROR, "[%s, line %d] Configuration has a invalid YAML version.  Must be 1.1 or above", __FILE__, __LINE__);
                        }

                } /* Document start */

            /* Document end! */

            else if ( event.type == YAML_STREAM_END_EVENT )
                {

                    done = true;

                    if ( Debug->config )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] YAML_STREAM_END_EVENT", __FILE__, __LINE__);
                        }
                }

            else if ( event.type == YAML_MAPPING_START_EVENT )
                {

                    toggle = 1;

                    if ( Debug->config )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] YAML_MAPPING_START_EVENT", __FILE__, __LINE__);
                        }
                }

            else if ( event.type == YAML_MAPPING_END_EVENT )
                {

                    toggle = 0;
                    sub_type = 0;

                    if ( Debug->config )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] YAML_MAPPING_END_EVENT", __FILE__, __LINE__);
                        }
                }


            else if ( event.type == YAML_SCALAR_EVENT )
                {

                    char *value = (char *)event.data.scalar.value;

                    Var_To_Value(value, var_to_value, sizeof(var_to_value));


                    if ( Debug->config )
                        {
                            Sagan_Log(DEBUG, "[%s, line %d] YAML_SCALAR_EVENT - Value: \"%s\"", __FILE__, __LINE__, value);
                        }

                    /* Load variables! */

                    if ( type == YAML_TYPE_VAR )
                        {

                            if ( toggle == 1 )
                                {

                                    Var = (_Var *) realloc(Var, (Counters->var+1) * sizeof(_Var));

                                    if ( Var == NULL )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] Failed to reallocate memory for _Var. Abort!", __FILE__, __LINE__);
                                        }

                                    memset(&Var[Counters->var], 0, sizeof(struct _Var));

                                    snprintf(Var[Counters->var].key, sizeof(Var[Counters->var].key), "$%s", value);
                                    Var[Counters->var].key[sizeof(Var[Counters->var].key)-1] = '\0';
                                    toggle = 0;

                                }
                            else
                                {

                                    if (strcmp(Var[Counters->var].key, ""))
                                        {

                                            /* If "file:/" is found, we load values from a file */

                                            if (Sagan_strstr(value, "file:/"))
                                                {

                                                    strtok_r(value, ":", &tok);

                                                    char *filename = NULL;
                                                    char tmpbuf[MAX_CONFIG_LINE] = { 0 };
                                                    FILE *varfile;
                                                    bool check = 0;

                                                    filename = strtok_r(NULL, ":", &tok);

                                                    if ( filename == NULL )
                                                        {
                                                            Sagan_Log(ERROR, "[%s, line %d] Attempted to load variable value via file:/ but no file found. Abort!",  __FILE__, __LINE__);
                                                        }

                                                    if ((varfile = fopen(filename, "r")) == NULL)
                                                        {
                                                            Sagan_Log(ERROR, "[%s, line %d] Cannot open var file:%s\n", __FILE__,  __LINE__, filename);
                                                        }


                                                    while(fgets(tmpbuf, sizeof(tmpbuf), varfile) != NULL)
                                                        {


                                                            /* Stuff to skip */

                                                            if (tmpbuf[0] == '#') continue;
                                                            if (tmpbuf[0] == ';') continue;
                                                            if (tmpbuf[0] == 10 ) continue;
                                                            if (tmpbuf[0] == 32 ) continue;

                                                            /* Simple check to see if this is the first entry or not.  This is to keep our "," on mark */

                                                            Remove_Return(tmpbuf);

                                                            if ( Debug->config )
                                                                {

                                                                    Sagan_Log(DEBUG, "[%s, line %d] Variable from file \"%s\" var \"%s\" loaded: \"%s\"", __FILE__, __LINE__, filename, Var[Counters->var].value, tmpbuf);
                                                                }

                                                            if ( check == 0 )
                                                                {

                                                                    check = 1;

                                                                }

                                                            /* Append to the var */

                                                            strlcat(Var[Counters->var].value, tmpbuf, sizeof(Var[Counters->var].value));


                                                            Var[Counters->var].value[strlen(Var[Counters->var].value)] = ',';
                                                            Var[Counters->var].value[strlen(Var[Counters->var].value) + 1] = '\0';

                                                        }

                                                    Var[Counters->var].value[strlen(Var[Counters->var].value) - 1] = '\0';


                                                    fclose(varfile);

                                                    if ( Debug->config )
                                                        {

                                                            Sagan_Log(DEBUG, "[%s, line %d] Final load from file for \"%s\" value \"%s\"", __FILE__, __LINE__, Var[Counters->var].key, Var[Counters->var].value);

                                                        }

                                                    __atomic_add_fetch(&Counters->var, 1, __ATOMIC_SEQ_CST);

                                                    toggle = 1;

                                                }
                                            else
                                                {

                                                    /* If "file:/" is not found, we load like a normal variable */

                                                    strlcpy(Var[Counters->var].value, value, sizeof(Var[Counters->var].value));

                                                    if ( Debug->config )
                                                        {

                                                            Sagan_Log(DEBUG, "[%s, line %d] Variable: \"%s == %s\"", __FILE__, __LINE__, Var[Counters->var].key, Var[Counters->var].value);
                                                        }

                                                    __atomic_add_fetch(&Counters->var, 1, __ATOMIC_SEQ_CST);

                                                    toggle = 1;

                                                }
                                        }

                                }

                        }  /* if type == YAML_TYPE_VAR */

                    else if ( type == YAML_TYPE_CORE )
                        {

                            /*
                                                        if ( !strcmp(last_pass, "ip" ) )
                                                            {

                                                                if (  Validate_IP(var_to_value) == true )
                                                                    {
                                                                        strlcpy(Config->ip, var_to_value, sizeof(Config->ip));
                                                                    }
                                                                else
                                                                    {
                                                                        Sagan_Log(ERROR, "[%s, line %d] The configuration 'ip' is set to %s which is not a valid IPv4/IPv6 address. Abort", __FILE__, __LINE__, var_to_value);
                                                                    }
                                                            }

                                                        else if ( !strcmp(last_pass, "port" ) )
                                                            {

                                                                Config->port = atoi(var_to_value);

                                                                if (  Config->port == 0 )
                                                                    {
                                                                        Sagan_Log(ERROR, "[%s, line %d] The configuration 'port' is set to %s which is not a valid port. Abort", __FILE__, __LINE__, var_to_value);
                                                                    }

                                                            }

                                                        else if ( !strcmp(last_pass, "proto" ) )
                                                            {

                                                                if ( !strcasecmp(var_to_value, "udp") )
                                                                    {
                                                                        Config->proto[0] = 'U';
                                                                        Config->proto[1] = 'D';
                                                                        Config->proto[2] = 'P';
                                                                        Config->proto[3] = '\0';

                                                                        Config->proto_int = 17;
                                                                    }

                                                                else if ( !strcasecmp(var_to_value, "tcp") )
                                                                    {
                                                                        Config->proto[0] = 'T';
                                                                        Config->proto[1] = 'C';
                                                                        Config->proto[2] = 'P';
                                                                        Config->proto[3] = '\0';

                                                                        Config->proto_int = 6;
                                                                    }

                                                                else if ( !strcasecmp(var_to_value, "icmp") )
                                                                    {
                                                                        Config->proto[0] = 'I';
                                                                        Config->proto[1] = 'C';
                                                                        Config->proto[2] = 'M';
                                                                        Config->proto[3] = 'P';
                                                                        Config->proto[4] = '\0';

                                                                        Config->proto_int = 1;
                                                                    }


                                                                if ( Config->proto_int == 0 )
                                                                    {
                                                                        Sagan_Log(ERROR, "[%s, line %d] The configuration 'proto' is set to %s which is not a valid. Valid options are UDP, TCP or ICMP. Abort", __FILE__, __LINE__, var_to_value);
                                                                    }

                                                            }
                            			*/

                            if ( !strcmp(last_pass, "max-threads" ) )
                                {

                                    Config->max_threads = atoi(var_to_value);

                                    if ( Config->max_threads == 0 )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] The configuration 'max_threads' is set to %s which is invalid. Abort", __FILE__, __LINE__, var_to_value);
                                        }

                                }

                            else if ( !strcmp(last_pass, "batch-size" ) )
                                {

                                    Config->batch_size = atoi(var_to_value);

                                    if ( Config->batch_size == 0 )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] The configuration 'batch_size' is set to %s which is invalid. Abort", __FILE__, __LINE__, var_to_value);
                                        }

                                }

                            else if ( !strcmp(last_pass, "classifications" ) )
                                {
                                    strlcpy(Config->classifications_file, var_to_value, sizeof(Config->classifications_file));
                                    Remove_Return(Config->classifications_file);
                                }

                            else if ( !strcmp(last_pass, "reference" ) )
                                {
                                    strlcpy(Config->reference_file, var_to_value, sizeof(Config->reference_file));
                                    Remove_Return(Config->reference_file);
                                }

                            else if ( !strcmp(last_pass, "lockfile" ) )
                                {
                                    strlcpy(Config->lock_file, var_to_value, sizeof(Config->lock_file));
                                    Remove_Return(Config->lock_file);

                                    lf1 = strdup(Config->lock_file);
                                    lf2 = strdup(Config->lock_file);

                                    dir = dirname(lf1);

                                    if ( dir == NULL )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] Directory for lock file appears '%s' to be incorrect. Abort",  __FILE__, __LINE__, Config->lock_file);
                                        }

                                    strlcpy(Config->lock_file_path, dir, sizeof(Config->lock_file_path));


                                }

                            else if ( !strcmp(last_pass, "runas" ) )
                                {
                                    strlcpy(Config->runas, var_to_value, sizeof(Config->runas));
                                    Remove_Return(Config->runas);
                                }

                        }

                    /* Types with sub-types */

                    else if ( type == YAML_TYPE_INPUT )
                        {

                            if ( !strcmp(value, "named-pipe") )
                                {
                                    sub_type = YAML_SUBTYPE_INPUT_PIPE;
                                }

                        }


                    /* Sub types ! */

                    if ( sub_type == YAML_SUBTYPE_INPUT_PIPE && type == YAML_TYPE_INPUT )
                        {

                            if ( !strcmp(last_pass, "enabled" ))
                                {
                                    if (!strcasecmp(value, "yes") || !strcasecmp(value, "true") || !strcasecmp(value, "enabled") )
                                        {
                                            Config->named_pipe_flag = true;
                                        }
                                }

                            if ( !strcmp(last_pass, "chown" ))
                                {
                                    if (!strcasecmp(value, "yes") || !strcasecmp(value, "true") || !strcasecmp(value, "enabled") )
                                        {
                                            Config->named_pipe_chown = true;
                                        }
                                }


                            else if ( !strcmp(last_pass, "named-pipe" ) && Config->named_pipe_flag == true )
                                {
                                    strlcpy(Config->named_pipe, var_to_value, sizeof(Config->named_pipe));
                                    Remove_Return(Config->named_pipe);
                                }



#if defined(HAVE_GETPIPE_SZ) && defined(HAVE_SETPIPE_SZ)

                            else if ( !strcmp(last_pass, "size" ) && Config->named_pipe_flag == true )
                                {

                                    Config->named_pipe_size = atoi( var_to_value );

                                    if ( Config->named_pipe_size == 0 )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] input -> named_pipe has a bad pipe size value of %s. Abort!",  __FILE__, __LINE__, var_to_value);
                                        }

                                    if ( Config->named_pipe_size != 65536 &&
                                            Config->named_pipe_size != 131072 &&
                                            Config->named_pipe_size != 262144 &&
                                            Config->named_pipe_size != 524288 &&
                                            Config->named_pipe_size != 1048576 )
                                        {
                                            Sagan_Log(ERROR, "[%s, line %d] input -> named_pipe has a bad pipe size value of %s. The size can only be 65536, 131072, 262144, 524288, or 1048576. Abort!",  __FILE__, __LINE__, var_to_value);
                                        }

                                }

#endif

                        }

                    else if ( type == YAML_TYPE_RULES )
                        {
                            Load_Ruleset( var_to_value );
                        }

                    strlcpy(last_pass, var_to_value, sizeof(last_pass));


                    /**** Tag types *************************************************/

                    /**************/
                    /**** vars ****/
                    /**************/

                    if (!strcmp(value, "vars"))
                        {

                            if ( Debug->config )
                                {
                                    Sagan_Log(DEBUG, "[%s, line %d] **** Found variables ****", __FILE__, __LINE__);
                                }

                            type = YAML_TYPE_VAR;
                            toggle = 0;

                        } /* tag: var */

                    else if (!strcmp(value, "configuration"))
                        {

                            if ( Debug->config )
                                {
                                    Sagan_Log(DEBUG, "[%s, line %d] **** Found \"configuration\" ****", __FILE__, __LINE__);
                                }

                            type = YAML_TYPE_CORE;
                            toggle = 0;

                        } /* tag: var */

                    else if (!strcmp(value, "input"))
                        {

                            if ( Debug->config )
                                {
                                    Sagan_Log(DEBUG, "[%s, line %d] **** Found \"input\" ****", __FILE__, __LINE__);
                                }

                            type = YAML_TYPE_INPUT;
                            toggle = 0;

                        } /* tag: var */

                    else if ( !strcmp(value, "rule-files" ))
                        {

                            if ( Debug->config )
                                {
                                    Sagan_Log(DEBUG, "[%s, line %d] **** Found \"rule-files\" ****", __FILE__, __LINE__);
                                }

                            type = YAML_TYPE_RULES;
                            toggle = 0;

                        }

                } /* End of "while */

        }

    yaml_parser_delete(&parser);
    fclose(fh);


}
