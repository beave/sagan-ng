#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


#include "util.h"
#include "sagan-ng-defs.h"
#include "sagan-config.h"
#include "var.h"
#include "counters.h"


struct _Config *Config;
struct _Counters *Counters;
struct _Var *Var;

/**********************************
 * Shift a string to all lowercase
 **********************************/

void To_LowerC(char *const s)
{
    char* cur = s;
    while (*cur)
        {
            *cur = tolower(*cur);
            ++cur;
        }
}


/******************************************************
 * Generic "sagan.log" style logging and screen output.
 *******************************************************/

void Sagan_Log (int type, const char *format,... )
{

    char buf[5128] = { 0 };
    va_list ap;
    va_start(ap, format);
    char *chr="*";
    char curtime[64];
    time_t t;
    struct tm *now;
    t = time(NULL);
    now=localtime(&t);
    strftime(curtime, sizeof(curtime), "%m/%d/%Y %H:%M:%S",  now);

    if ( type == ERROR )
        {
            chr="E";
        }

    if ( type == WARN )
        {
            chr="W";
        }

    if ( type == DEBUG )
        {
            chr="D";
        }

    vsnprintf(buf, sizeof(buf), format, ap);
    //fprintf(config->sagan_log_stream, "[%s] [%s] - %s\n", chr, curtime, buf);
    //fflush(config->sagan_log_stream);

    if ( Config->daemonize == 0 && Config->quiet == 0 )
        {
            printf("[%s] %s\n", chr, buf);
        }

    if ( type == ERROR )
        {
            exit(1);
        }

}

/********************
 * Remove new-lines
 ********************/

void Remove_Return(char *s)
{
    char *s1, *s2;
    for(s1 = s2 = s; *s1; *s1++ = *s2++ )
        while( *s2 == '\n' )s2++;
}


bool Validate_IP(const char *ip)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return result != 0;
}

/****************************************************************
 * String replacement function.  Used for things like $RULE_PATH
 ****************************************************************/

void Replace_String(char *in_str, char *orig, char *rep, char *str, size_t size)
{

    char buffer[4096] = { 0 };
    char *p = NULL;

    if(!(p = strstr(in_str, orig)))
        {
            snprintf(str, size, "%s", in_str);
            return;
        }

    strlcpy(buffer, in_str, p-in_str);
    buffer[p-in_str] = '\0';
    sprintf(buffer+(p-in_str), "%s%s", rep, p+strlen(orig));

    snprintf(str, size, "%s", buffer);

}

/****************************************************************************
 * Var_To_Value - Changes a variable in a configuration file (for
 * example - $RULE_PATH into it's true value.
 * ** README ** Don't use in live engine due to strstr.
 ****************************************************************************/

void Var_To_Value(char *in_str, char *str, size_t size)
{

    char *ptmp = NULL;
    char *tok = NULL;
    char tmp2[MAX_VAR_VALUE_SIZE] = { 0 };
    char tmp3[MAX_VAR_VALUE_SIZE] = { 0 };
    char tmp_result[MAX_VAR_VALUE_SIZE] = { 0 };
    char tmp[MAX_VAR_VALUE_SIZE] = { 0 };
    int i=0;

    snprintf(tmp, sizeof(tmp), "%s", in_str);           /* Segfault with strlcpy */

    for (i=0; i<Counters->var; i++)
        {

            ptmp = strtok_r(tmp, " ", &tok);

            while (ptmp != NULL )
                {

                    Replace_String(ptmp, Var[i].key, Var[i].value, tmp2, sizeof(tmp2));
                    snprintf(tmp3, sizeof(tmp3), "%s ", tmp2);
                    strlcat(tmp_result, tmp3, sizeof(tmp_result));
                    ptmp = strtok_r(NULL, " ", &tok);
                }

            strlcpy(tmp, tmp_result, sizeof(tmp));
            memset(tmp_result, 0, sizeof(tmp_result));
        }


    tmp[strlen(tmp)-1] = 0;             /* Remove trailing space */

    snprintf(str, size, "%s", tmp);

}

/*
void Droppriv(void)
{

    struct stat fifocheck;
    struct passwd *pw = NULL;
    int ret;

    pw = getpwnam(Config->runas);

    if (!pw)
        {
            Sagan_Log(ERROR, "Couldn't locate user '%s'. Aborting...", Config->runas);
        }

    if ( getuid() == 0 )
        {

            /*
             * We chown certain log files to our Sagan user.  This is done so no files are "owned"
             * by "root".  This prevents problems in the future when doing things like handling
                 * SIGHUP's and what not.
                 *
                 * Champ Clark (04/14/2015)
                 */

//            if ( config->sagan_is_file == false )       /* Don't change ownsership/etc if we're processing a file */
//                {

/*                    if ( Config->chown_fifo == true )
                        {

                            Sagan_Log(NORMAL, "Changing FIFO '%s' ownership to '%s'.", config->sagan_fifo, config->sagan_runas);

                            ret = chown(config->sagan_fifo, (unsigned long)pw->pw_uid,(unsigned long)pw->pw_gid);

                            if ( ret < 0 )
                                {
                                    Sagan_Log(ERROR, "[%s, line %d] Cannot change ownership of %s to username \"%s\" - %s", __FILE__, __LINE__, config->sagan_fifo, config->sagan_runas, strerror(errno));
                                }
                        }


                    if (stat(config->sagan_fifo, &fifocheck) != 0 )
                        {
                            Sagan_Log(ERROR, "[%s, line %d] Cannot open %s FIFO - %s!",  __FILE__, __LINE__, config->sagan_fifo, strerror(errno));
                        }

//                }


            Sagan_Log(NORMAL, "Dropping privileges! [UID: %lu GID: %lu]", (unsigned long)pw->pw_uid, (unsigned long)pw->pw_gid);

            if (initgroups(pw->pw_name, pw->pw_gid) != 0 ||
                    setgid(pw->pw_gid) != 0 || setuid(pw->pw_uid) != 0)
                {
                    Sagan_Log(ERROR, "[%s, line %d] Could not drop privileges to uid: %lu gid: %lu - %s!", __FILE__, __LINE__, (unsigned long)pw->pw_uid, (unsigned long)pw->pw_gid, strerror(errno));
                }

        }
    else
        {
            Sagan_Log(NORMAL, "Not dropping privileges.  Already running as a non-privileged user");
        }
}

*/
