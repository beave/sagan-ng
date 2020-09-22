
typedef struct _Input_Batch _Input_Batch;
struct _Input_Batch
{
    char input[MAX_JSON_SIZE];
};


void Queue( const char *input );
void Batch_Init( void );



