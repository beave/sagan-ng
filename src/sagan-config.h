
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

    char		sensor_name[MAX_SENSOR_NAME];
    char		cluster_name[MAX_CLUSTER_NAME];

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

    bool input_named_pipe_flag;
    char input_named_pipe[MAX_PATH];
    uint32_t input_named_pipe_size;
    bool input_named_pipe_chown;


    /***********************************************************************/
    /* Output                                                              */
    /***********************************************************************/

    bool output_file_flag;
    char output_file[MAX_PATH];
    bool output_file_flatten_json;
    bool output_file_append_alert_data;
    FILE *output_file_stream;



};
