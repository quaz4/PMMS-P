OBJECTS = pmms.o

pmms : $(OBJECTS)
	gcc $(OBJECTS) -o pmms -pthread

pmms.o : pmms.c
	gcc -c pmms.c -Wall -pedantic

clean : 
	rm -f pmms $(OBJECTS)
