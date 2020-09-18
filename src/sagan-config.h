
#include <stdint.h>

/* Sagan configuration struct (global) */

typedef struct _Config _Config;
struct _Config
{

    /* Non-dependent var's */

//    bool         sagan_reload;
    bool         daemonize;
    bool         quiet;

    char 	 config_yaml[MAX_PATH];

    /* Configuration */

    char		ip[46];
    char		proto;
    unsigned char	proto_int;
    uint32_t            port;
    uint32_t		max_threads;
    char	        classifications_file[MAX_PATH];
    char		reference_file[MAX_PATH];
    
    




};
