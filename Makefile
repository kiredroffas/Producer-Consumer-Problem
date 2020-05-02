all: pc

pc: pc.c
	gcc -o pc pc.c -pthread -lpthread -lrt -Werror

clean:
	rm pc