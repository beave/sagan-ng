#include <stdbool.h>

void Sagan_Log (int type, const char *format,... );
void To_LowerC(char *const s);
void Remove_Return(char *s);
void Remove_Spaces(char *s);
bool Validate_IP(const char *ip);
void Replace_String(char *in_str, char *orig, char *rep, char *str, size_t size);
void Droppriv(void);
void Between_Quotes(const char *in_str, char *str, size_t size);
bool Validate_HEX (const char *string);
bool Pipe_To_Value(const char *in_str, char *str, size_t size );
void Replace_Sagan(const char *in_str, char *replace, char *str, size_t size);

#ifdef __OpenBSD__
/* OpenBSD won't allow for this test:
 * "suricata(...): mprotect W^X violation" */
#ifndef PageSupportsRWX()
#define PageSupportsRWX() 0
#endif
#else
#ifndef HAVE_SYS_MMAN_H
#define PageSupportsRWX() 1
#else
int       PageSupportsRWX(void);
#endif /* HAVE_SYS_MMAN_H */
#endif


#if defined(HAVE_GETPIPE_SZ) && defined(HAVE_SETPIPE_SZ)
void Set_Pipe_Size ( FILE *fd );
#endif
