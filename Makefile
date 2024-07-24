src = $(wildcard *.c)
head = $(wildcard *.h)
obj = $(src:%.c=%.o)
target = gomoku
CFLAGS = -Wall -Wextra -Wshadow -O2
CPPFLAGS = 
#LDFLAGS = -g -fsanitize=undefined,address,leak,null,bounds
LDFLAGS = -lm

%.o: %.c $(head) Makefile 
	$(CC) -c $< -o $@ $(CFLAGS)

$(target): $(obj)
	$(CC) $(obj) -o $(target) $(LDFLAGS)

.PHONY: clean rebuild run all

clean:
	rm -rf $(obj) $(target) 

rebuild: clean $(target)

run: $(target)
	./$(target)

all: $(target)
