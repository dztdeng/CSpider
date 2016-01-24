#include<cspider/spider.h>
typedef struct {
  char *getTitle[20];
  char *getDesc[20];
  int size;
} Movie;
//开始入口的 url
char *begin = "www.douban.com/tag/%E7%A7%91%E5%B9%BB/movie";
void p(cspider_t *cspider, char *d, char *url, void *user_data) {
  Movie *movie = (Movie*)malloc(sizeof(Movie));
  char *urls[20];
  int sizeTitle = xpath(d, "//div[@id='content']/div[@class='grid-16-8 clearfix']/div[@class='article']/div[@class='mod movie-list']/dl/dd/a", movie->getTitle, 20);
  int sizeDesc = xpath(d, "//div[@id='content']/div[@class='grid-16-8 clearfix']/div[@class='article']/div[@class='mod movie-list']/dl/dd/div[@class='desc']", movie->getDesc, 20);
  int sizeUrl = xpath(d, "//div[@id='content']/div[@class='grid-16-8 clearfix']/div[@class='article']/div[@class='paginator']/a/@href", urls, 20);
  movie->size = sizeTitle;
  saveString(cspider, (void*)movie, LOCK);
  char *newUrl[sizeUrl];
  int i;
  //拼接成新的 url
  for (i = 0; i < sizeUrl; i++) {
    newUrl[i] = (char*)malloc(sizeof(char) * (strlen(begin) + strlen(urls[i]) + 1));
    strcat(newUrl[i], begin);
    strcat(newUrl[i], urls[i]);
  }
  //添加新 url 到任务队列
  if (movie->size > 0) {
    addUrls(cspider, newUrl, sizeUrl);
  }
  //回收
  freeStrings(newUrl, sizeUrl);
  freeStrings(urls, sizeUrl);
  freeStrings(movie->getTitle, sizeTitle);
  freeStrings(movie->getDesc, sizeDesc);
  free(movie);
}
void s(void *str, void *user_data) {
  Movie *get = (Movie*)str;
  FILE *file = (FILE*)user_data;
  int i;
  for (i = 0; i < get->size; i++) {
    fprintf(file, "名称:%s\n", get->getTitle[i]);
    fprintf(file, "类别:%s\n", get->getDesc[i]);
  }
}
int main() {
  cspider_t *spider = init_cspider(); 
  char *agent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.10; rv:42.0) Gecko/20100101 Firefox/42.0";
  char *cookie = "bid=s3/yuH5Jd/I; _pk_ref.100001.8cb4=%5B%22%22%2C%22%22%2C1450940218%2C%22http%3A%2F%2Fmovie.douban.com%2Ftag%2F%22%5D; _pk_id.100001.8cb4=8196f325b29ea5c3.1444265431.9.1450943478.1449364495.; ll=108288; viewed=1130500_24708145_6433169_4843567_1767120_5318823_1899158_1271597; __utma=30149280.927537245.1446813674.1449139583.1450940286.5; __utmz=30149280.1450940286.5.5.utmcsr=book.douban.com|utmccn=(referral)|utmcmd=referral|utmcct=/; ps=y; ue=965166527@qq.com; push_noty_num=0; push_doumail_num=7; ap=1; _pk_ses.100001.8cb4=*; gr_user_id=5f4ee24f-d7bc-4b0b-9322-ceb1d208ee36; __utmb=30149280.17.10.1450940286; __utmc=30149280; ct=y; as=http://www.douban.com/tag/%E7%A7%91%E5%B9%BB/movie";
  cs_setopt_url(spider, begin);
  cs_setopt_useragent(spider, agent);
  //cs_setopt_cookie(spider, cookie);
  FILE *file = fopen("./movies.txt", "wb+");
  //指向自定义的解析函数，和数据持久化函数
  cs_setopt_process(spider, p, NULL);
  cs_setopt_save(spider, s, file);
  return cs_run(spider);
}
