
#include <stdint.h>
#include <stdbool.h>

/* Sagan configuration struct (global) */

typedef struct _Config _Config;
struct _Config
{

    /***********************************************************************/
    /* Non-dependent var's                                                 */
    /***********************************************************************/

//    bool         sagan_reload;
    bool         daemonize;
    bool         quiet;

    char 	 config_yaml[MAX_PATH];

    /***********************************************************************/
    /* Configuration 							   */
    /***********************************************************************/

    char		runas[32];
//    char		ip[46];
//    char		proto[5];
//    unsigned char	proto_int;
//    uint32_t            port;
    uint32_t		max_threads;
    unsigned char	batch_size;
    char	        classifications_file[MAX_PATH];
    char		reference_file[MAX_PATH];
    char		lock_file[MAX_PATH];
    char		lock_file_path[MAX_PATH];

    /***********************************************************************/
    /* Input								   */
    /***********************************************************************/

    /* named pipe */

    bool named_pipe_flag;
    char named_pipe[MAX_PATH];
    uint32_t named_pipe_size;
    bool named_pipe_chown;








};
