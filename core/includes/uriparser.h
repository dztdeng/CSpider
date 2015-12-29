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
joinall(char *, char **, int);

char *
join(char *, char*);

char *
uri_component(char *, URIACTION);
  
void
rstrip(char *);

void
lstrip(char *);

