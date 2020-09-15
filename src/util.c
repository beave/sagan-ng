#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>

#include "util.h"
#include "sagan-ng-defs.h"

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

    /*
    if ( config->daemonize == 0 && config->quiet == 0 )
        {
            printf("[%s] %s\n", chr, buf);
        }
    */

    if ( type == ERROR )
        {
            exit(1);
        }

}

