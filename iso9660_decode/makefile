all:
	gcc main.c iso9660.c -o program

.PHONY: clean run

clean:
	rm -rf ../root
	rm program

run: all
	./program