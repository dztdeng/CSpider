TARGET=douban

all:$(TARGET)
.PHONY:all
$(TARGET):$(TARGET).o
	gcc -o $@ $< -lcspider -I /usr/include/libxml2
	mv $(TARGET) $(TARGET)-$(shell cm)
%.o:%.c
	gcc -c $< -lcspider -I /usr/include/libxml2

clean:
	rm $(TARGET)-$(shell cm) $(TARGET).o
