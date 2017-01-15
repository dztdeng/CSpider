prom=douban

all:$(prom)
.PHONY:all
douban:douban.o
	gcc -o $@ $< -lcspider -I /usr/include/libxml2
%.o:%.c
	gcc -c $< -lcspider -I /usr/include/libxml2
