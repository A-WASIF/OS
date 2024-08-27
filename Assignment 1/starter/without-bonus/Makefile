all:
	gcc  -m32 -no-pie -nostdlib -o prime prime.c
	gcc -m32 -o loader loader.c
	./loader prime

clean:
	-@rm -f prime loader
