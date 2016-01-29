#include <cspider/spider.h>

// This struct represents a Github Repository
typedef struct github_repo {
	char *url;
	char *data;
} GithubRepo;

void process(cspider_t *cspider, char *data, char *url, void *user_data) {
	// Buffer for extracting links in each page.
	char *get[100];

	// Basestring to check againt relative links such as /torvalds/linux
	char *base = "https://www.github.com/";

	// This struct will be passed to save function which can be used
	//  as a persistency function
	GithubRepo repo = {.url = url, .data = data};

	// Extract all repositories
	int size = xpath(data, "//ol[@class=\"repo-list\"]//a/@href", get, 100);

	// Join relative urls to the base, otherwise next requests are invalid.
	// 	If not joined, it contains only paths like : /torvalds/linux
	cspider_joinall(base,get,size);

	// Add extracted urls to uri front queue
	addUrls(cspider, get, size);

	// Pass GithubRepo struct to save function ( data persistency )
	saveString(cspider, (void *)&repo, LOCK);

	// Free allocated strings ( they are copied to the queue by addUrls function,
	//  So at this point we can release the buffer )
	freeStrings(get, size);
}

void save(void *user_data, void *output) {
	// @user_data -> GithubRepo passed from process function
	// @output -> Output stream defined by cs_setopt_save

	GithubRepo *repo = (GithubRepo *)user_data;
	char *data = repo->data;
	char *recv[1];
	FILE *file = (FILE*)output;

	// Find the description in repository.
	int size = xpath(data,"//span[@class=\"repository-meta-content\"]",recv,1);

	// In case description is given or the url is a repository
	//  Print requested url along with description
	if (size >0)
		fprintf(file,"Url :[ %s] \nDescription: \n%s\n",repo->url,recv[0]);

	return;
}

int main() {

	// Initialize CSpider
	cspider_t *spider = init_cspider();

	char *agent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.10; rv:42.0) Gecko/20100101 Firefox/42.0";

	// Set Crawl URL
	cs_setopt_url(spider, "https://github.com/trending?l=c");

	// Set User Agent
	cs_setopt_useragent(spider, agent);

	// Set callback for process function. It will be called when data is available
	cs_setopt_process(spider, process, NULL);

	// Set callback for persistency. third parameter can be adjusted for output.
	cs_setopt_save(spider, save, stdout);

	// Number of downloader threads
	cs_setopt_threadnum(spider, 7);

	return cs_run(spider);

}
