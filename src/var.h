
#define MAX_VAR_NAME_SIZE 64
#define MAX_VAR_VALUE_SIZE 32786

typedef struct _Var _Var;
struct _Var
{
    char key[MAX_VAR_NAME_SIZE];
    char value[MAX_VAR_VALUE_SIZE];
};

