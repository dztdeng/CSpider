typedef enum {
  URI_SCHEME = 0,
  URI_AUTH,
  URI_HOST,
  URI_PORT,
  URI_PATH,
  URI_QUERY,
  URI_FRAGMENT
} URIACTION;

void
cspider_joinall(char *, char **, int);

char *
cspider_join(char *, char*);

char *
cspider_uri_component(char *, URIACTION);

void
cspider_rstrip(char *);

void
cspider_lstrip(char *);

