#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

#include "liburi.h"
#include "uriparser.h"

static URI *parseuris(const char *, const char *);
static char *get_uri(URI *uri);

static URI *
parseuris(const char* basestr, const char *uristr){
  URI *uri, *rel, *base;

  if(basestr){
    base = uri_create_str(basestr, NULL);
    if(!base)
      return NULL;

    rel = uri_create_str(uristr, NULL);
    if(!rel)
      return NULL;

    uri = uri_create_uri(rel, base);
    if(!uri)
      return NULL;

    uri_destroy(base);
    uri_destroy(rel);

    return uri;
  }

  uri = uri_create_str(uristr, NULL);
  if(!uri)
    return NULL;

  return uri;
}

static char *
get_uri(URI *uri) {
  size_t len;
  char *buffer;

  len = uri_str(uri, NULL, 0);
  if(len == (size_t) -1)
    return NULL;

  buffer = malloc(len);
  if(!buffer)
    return NULL;

  len = uri_str(uri, buffer, len);
  if(len == (size_t) -1)
    return NULL;

  return buffer;
}

static int
getcomponent(URI *uri, size_t (*fn)(URI *, char *, size_t), char **buffer, size_t *len){
  size_t r;
  char *p;

  r = fn(uri,*buffer,*len);
  if(r == (size_t)-1)
    return -1;

  if(r > *len){
    p = realloc(*buffer,r);
    if(p == NULL)
      return -1;

    *buffer = p;
    *len = r;
    r = fn(uri,*buffer,*len);
    if(r == (size_t)-1)
      return -1;
  }

  if(r == 0)
    return -1;

  return 1;
}

char *
cspider_join(char *baseuri, char *reluri){

  URI *uri;
  char *res;
  int r;

  uri = parseuris(baseuri,reluri);

  if(!uri)
    return NULL;

  res = get_uri(uri);
  uri_destroy(uri);

  return res;
}

void
cspider_rstrip(char *string){
  int l;
  if (!string)
    return;
  l = strlen(string) - 1;
  while (isspace(string[l]) && l >= 0) string[l--] = 0;
}

void
cspider_lstrip(char *string){
  int i, l;
  if (!string)
    return;
  l = strlen(string);
  while (isspace(string[(i = 0)])) while(i++ < l) string[i-1] = string[i];
}


void
cspider_joinall(char *baseuri, char **uris, int size){
  int i;
  char *parsed = NULL;

  for (i = 0; i < size; i++){
    cspider_lstrip(uris[i]); cspider_rstrip(uris[i]);
    parsed = cspider_join(baseuri,uris[i]);
    if (parsed == NULL) {
      continue;
    }
    free(uris[i]);
    uris[i] = NULL;
    uris[i] = parsed;
    parsed = NULL;
  }
}

char *
cspider_uri_component(char *adr, URIACTION action){
  URI *uri = NULL;
  char *buffer = NULL;
  size_t len = 0;
  int r ;

  if (adr == NULL || strlen(adr) == 0)
    return buffer;
  uri = uri_create_str(adr,NULL);

  switch(action){
      case URI_SCHEME:
	r = getcomponent(uri,uri_scheme,&buffer,&len);
	break;
      case URI_AUTH:
	r = getcomponent(uri,uri_auth,&buffer,&len);
	break;
      case URI_HOST:
	r = getcomponent(uri,uri_host,&buffer,&len);
	break;
      case URI_PORT:
	r = getcomponent(uri,uri_port,&buffer,&len);
	break;
      case URI_PATH:
	r = getcomponent(uri,uri_path,&buffer,&len);
	break;
      case URI_QUERY:
	r = getcomponent(uri,uri_query,&buffer,&len);
	break;
      case URI_FRAGMENT:
	r = getcomponent(uri,uri_fragment,&buffer,&len);
	break;
 }

  uri_destroy(uri);

  return r == 1 ?
    buffer : NULL;
}
