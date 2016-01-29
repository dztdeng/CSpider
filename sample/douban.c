#include<cspider/spider.h>

typedef struct {
  char *getTitle[20];
  char *getDesc[20];
  int size;
} Movie;

/* begin url */
char *begin = "www.douban.com/tag/%E7%A7%91%E5%B9%BB/movie";
void p(cspider_t *cspider, char *d, char *url, void *user_data) {
  Movie *movie = (Movie*)malloc(sizeof(Movie));
  char *urls[20];
  /* xpath to get title, desc and url */
  int sizeTitle = xpath(d, "//div[@id='content']/div[@class='grid-16-8 clearfix']/div[@class='article']/div[@class='mod movie-list']/dl/dd/a", movie->getTitle, 20);
  int sizeDesc = xpath(d, "//div[@id='content']/div[@class='grid-16-8 clearfix']/div[@class='article']/div[@class='mod movie-list']/dl/dd/div[@class='desc']", movie->getDesc, 20);
  int sizeUrl = xpath(d, "//div[@id='content']/div[@class='grid-16-8 clearfix']/div[@class='article']/div[@class='paginator']/a/@href", urls, 20);
  movie->size = sizeTitle;
  /* data persistence */
  saveString(cspider, (void*)movie, LOCK);
  char *newUrl[sizeUrl];
  int i;
  /* get new url */
  for (i = 0; i < sizeUrl; i++) {
    newUrl[i] = (char*)malloc(sizeof(char) * (strlen(begin) + strlen(urls[i]) + 1));
    strcat(newUrl[i], begin);
    strcat(newUrl[i], urls[i]);
  }
  /* add new url to task queue*/
  if (movie->size > 0) {
    addUrls(cspider, newUrl, sizeUrl);
  }
  /* free */
  freeStrings(newUrl, sizeUrl);
  freeStrings(urls, sizeUrl);
  freeStrings(movie->getTitle, sizeTitle);
  freeStrings(movie->getDesc, sizeDesc);
  free(movie);
}
/**
   data persistence function
*/
void s(void *str, void *user_data) {
  Movie *get = (Movie*)str;
  FILE *file = (FILE*)user_data;
  int i;
  for (i = 0; i < get->size; i++) {
    fprintf(file, "name:%s\n", get->getTitle[i]);
    fprintf(file, "type:%s\n", get->getDesc[i]);
  }
}
int main() {
  cspider_t *spider = init_cspider(); 
  char *agent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.10; rv:42.0) Gecko/20100101 Firefox/42.0";
  cs_setopt_url(spider, begin);
  cs_setopt_useragent(spider, agent);
  FILE *file = fopen("./movies.txt", "wb+");
  /*
    define custom process function p,
    and custom data persistence function s
   */
  cs_setopt_process(spider, p, NULL);
  cs_setopt_save(spider, s, file);
  return cs_run(spider);
}
