#pragma once
typedef unsigned long long ULLONG;

void *malloc_plus(ULLONG _byte_size, unsigned int _line,char *_file_name);
void free_plus(void *_malloc_plus);
void destroy_malloc(void);
void create_leak_file(void);
void print_malloc_plus(void);

#define file_name(x) strrchr(x,'\\')? strrchr(x,'\\')+1:x
#define malloc(size) malloc_plus(size,__LINE__,file_name(__FILE__))
#define free(zone) free_plus(zone)
#define MALLOC_PLUS_LEAK_FILE_HEADER "log_"
