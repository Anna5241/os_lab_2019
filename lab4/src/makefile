CC=gcc
CFLAGS=-I. -I$(LAB3_SRC)
VPATH=$(LAB3_SRC)

LAB3_SRC=../../lab3/src

all: parallel_min_max zombie_process process_memory parallel_sum

zombie_process : zombie_process.c
	$(CC) -o zombie_process zombie_process.c $(CFLAGS)

process_memory : process_memory.c
	$(CC) -o process_memory process_memory.c $(CFLAGS)

parallel_min_max : $(LAB3_SRC)/utils.o $(LAB3_SRC)/find_min_max.o parallel_min_max.c
	$(CC) -o parallel_min_max $(LAB3_SRC)/utils.o $(LAB3_SRC)/find_min_max.o parallel_min_max.c $(CFLAGS)

$(LAB3_SRC)utils.o : $(LAB3_SRC)/utils.c $(LAB3_SRC)/utils.h
	$(CC) -o $(LAB3_SRC)/utils.o -c $(LAB3_SRC)/utils.c $(CFLAGS)

$(LAB3_SRC)find_min_max.o : $(LAB3_SRC)/find_min_max.c $(LAB3_SRC)/find_min_max.h
	$(CC) -o $(LAB3_SRC)/find_min_max.o -c $(LAB3_SRC)/find_min_max.c $(CFLAGS)

libutils.a: $(LAB3_SRC)/utils.o
	ar rcs libutils.a $(LAB3_SRC)/utils.o

sum_lib.o: sum_lib.c
	$(CC) -c sum_lib.c -o sum_lib.o

libsum.a: sum_lib.o
	ar rcs libsum.a sum_lib.o

parallel_sum: parallel_sum.c libsum.a libutils.a
	$(CC) parallel_sum.c -o parallel_sum -lpthread -L. -lsum -lutils

clean :
	rm -f $(LAB3_SRC)/utils.o $(LAB3_SRC)/find_min_max.o parallel_min_max zombie_process process_memory parallel_sum libutils.a sum_lib.o libsum.a
