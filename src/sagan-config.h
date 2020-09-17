

/* Sagan configuration struct (global) */

typedef struct _Config _Config;
struct _Config
{

    /* Non-dependent var's */

//    bool         sagan_reload;
    bool         daemonize;
    bool         quiet;

    char 	 config_yaml[MAX_PATH];


};
