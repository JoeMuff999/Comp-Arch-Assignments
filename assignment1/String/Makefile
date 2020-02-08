all: main grade

my_string_c.o: 
	gcc -c my_string.c -o my_string_c.o

main: my_string_c.o
	gcc main.c $^ -o main

clean:
	rm -f main
	rm -f *.o
	rm -f grade

grade: my_string_c.o
	gcc grade.c $^ -o grade
