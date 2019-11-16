#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "malloc_plus.h"
#include <time.h>
typedef struct Malloc_Plus
{
	void *zone;
	int count;
	ULLONG size;
	char file_name[64];
	unsigned int line;
	struct Malloc_Plus *next;
}Malloc_Plus;
static Malloc_Plus *_head_node = NULL;
static Malloc_Plus *_current_node = NULL;

extern void add_node(Malloc_Plus *_node);
#undef malloc
#undef free
void *malloc_plus(ULLONG _byte_size, unsigned int _line, char *_file_name)
{
	if (_byte_size == 0)
	{
		return NULL;
	}
	Malloc_Plus *malloc_p = malloc(sizeof(Malloc_Plus));
	memset(malloc_p,0,sizeof(Malloc_Plus));
	malloc_p->zone = malloc(_byte_size);
	memset(malloc_p->zone,0,_byte_size);
	malloc_p->count = 1;
	malloc_p->next = NULL;
	malloc_p->size = _byte_size;
	memcpy(malloc_p->file_name, _file_name,strlen(_file_name));
	malloc_p->line = _line;
	if (_head_node == NULL)
	{
		_head_node = malloc_p;
		_current_node = malloc_p;
	}
	else
	{
		add_node(malloc_p);
	}
	return malloc_p->zone;
}
void add_node(Malloc_Plus *_node)
{
	_current_node->next = _node;
	_current_node = _node;
}
int malloc_count(void *_malloc_plus)
{
	if (_malloc_plus == NULL)
	{
		return 0;
	}
	Malloc_Plus *malloc_p = _malloc_plus;
	return malloc_p->count;
}
void print_malloc_plus(void)
{
	if (_head_node == NULL)
	{
		return;
	}
	Malloc_Plus **current_node_addr = &_head_node;
	Malloc_Plus *current_node = NULL;
	while ((current_node = *current_node_addr)!= NULL)
	{
		printf("%s %d内存泄漏\n",current_node->file_name,current_node->line);
		current_node_addr = &current_node->next;
	}
}
void create_leak_file(void)
{
	if (_head_node == NULL)
	{
		return;
	}
	Malloc_Plus **current_node_addr = &_head_node;
	Malloc_Plus *current_node = *current_node_addr;
	if (current_node == NULL)
	{
		return;
	}
	char file_name[128] = {0};
	sprintf(file_name,"%s%ld.txt",MALLOC_PLUS_LEAK_FILE_HEADER,time(NULL));
	FILE *f_w = fopen(file_name,"w");
	if (f_w == NULL)
	{
		printf("打开%s失败\n", file_name);
		return;
	}
	while ((current_node = *current_node_addr)!= NULL)
	{
		char buf[512] = {0};
		sprintf(buf, "%s %d行内存泄漏\n", current_node->file_name, current_node->line);
		fputs(buf,f_w);
		current_node_addr = &current_node->next;
	}
	fclose(f_w);
	f_w = NULL;
}
void free_plus(void *_zone)
{
	if (_zone == NULL)
	{
		return;
	}
	if (_head_node == NULL)
	{
		return;
	}
	//Malloc_Plus *malloc_p = _malloc_plus;
	Malloc_Plus **dst_node_address = &_head_node;
	Malloc_Plus *dst_node = NULL;
	while ((dst_node = *dst_node_address) != NULL && dst_node->zone != _zone)
	{
		dst_node_address = &dst_node->next;
	}
	if (dst_node != NULL)
	{
		Malloc_Plus *next_node = dst_node->next;
		if (next_node == NULL)
		{
			if (dst_node->zone != NULL)
			{
				printf("销毁了%s %d\n",dst_node->file_name,dst_node->line);
				free(dst_node->zone);
				dst_node->zone = NULL;
			}
			free(dst_node);
			dst_node = NULL;
		}
		else
		{
			/*dst_node->zone = realloc(dst_node->zone,sizeof(next_node->zone));
			memcpy(dst_node->zone, next_node->zone,sizeof(next_node->zone));*/

			if (dst_node->zone != NULL)
			{
				printf("销毁了%s %d\n", dst_node->file_name, dst_node->line);
				free(dst_node->zone);
				dst_node->zone = next_node->zone;
			}
			dst_node->count = next_node->count;
			dst_node->next = next_node->next;
			dst_node->size = next_node->size;
			dst_node->line = next_node->line;
			free(next_node);
			next_node = NULL;
		}
	}
}
void destroy_malloc(void)
{
	if (_head_node == NULL)
	{
		return;
	}
	Malloc_Plus **current_node_addr = &_head_node;
	Malloc_Plus *current_node = NULL;
	while ((current_node = *current_node_addr) != NULL)
	{
		*current_node_addr = current_node->next;
		if (current_node->zone != NULL)
		{
			free(current_node->zone);
			current_node->zone = NULL;
		}
		free(current_node);
		current_node = NULL;
	}
	if (current_node == NULL && _head_node == NULL)
	{
		printf("销毁了所有已经创建的内存\n");
	}
}