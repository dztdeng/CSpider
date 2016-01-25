#ifndef CS_H
#define CS_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<uv.h>
#include<assert.h>
#include<inttypes.h>
#include<math.h>
#include<float.h>
#include<limits.h>
#include<stdarg.h>
#include<ctype.h>
#include<curl/curl.h>
#include<pcre.h>
#include<libxml/tree.h>
#include<libxml/parser.h>
#include<libxml/xpath.h>
#include<libxml/xpathInternals.h>
#include<libxml/HTMLparser.h>
#include<libxml/HTMLtree.h>


#define PANIC(x) assert(x != NULL)
/*
the max number of the buffers
*/
#define BUFFER_MAX_NUMBER 1024
#define bool char

#ifndef TRUE
    #define TRUE 1
    #define FALSE 0
#endif

typedef struct cspider_struct cspider_t;

typedef struct cs_page_struct cs_page;
typedef struct cs_page_queue_struct cs_page_queue;


#endif
