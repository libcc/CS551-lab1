#DEBUG=-g

main_demo: my_malloc.o my_malloc_aux.o main_demo.o
	gcc $(DEBUG) -o main_demo my_malloc.o my_malloc_aux.o main_demo.o

main_demo.o: my_malloc.h my_malloc_aux.h main_demo.c
	gcc $(DEBUG) -c main_demo.c

my_malloc.o: my_malloc.h my_malloc_aux.h my_malloc.c
	gcc $(DEBUG) -c my_malloc.c
	
my_malloc_aux.o: my_malloc.h my_malloc_aux.h my_malloc_aux.c
	gcc $(DEBUG) -c my_malloc_aux.c
	
clean: 
	rm *.o
