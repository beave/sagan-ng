
void Sagan_Log (int type, const char *format,... );
void To_LowerC(char *const s);
void Remove_Return(char *s);
bool Validate_IP(const char *ip);
void Replace_String(char *in_str, char *orig, char *rep, char *str, size_t size);
void Droppriv(void);


#if defined(HAVE_GETPIPE_SZ) && defined(HAVE_SETPIPE_SZ)
void Set_Pipe_Size ( FILE *fd );
#endif
