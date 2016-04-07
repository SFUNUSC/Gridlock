CFLAGS 	= -I./gnuplot_i -I./lin_eq_solver -I./gridlock_functions -o2

all: lib gridlock

gridlock: gridlock.c gridlock.h gnuplot_i.o lin_eq_solver.o
	@echo Making gridlock...
	gcc $(CFLAGS) gridlock.c -Wall -o gridlock gnuplot_i.o lin_eq_solver.o
	@echo Tidying up...
	rm -rf *~ *.o
	
lib:gnuplot_i/gnuplot_i.c gnuplot_i/gnuplot_i.h lin_eq_solver/lin_eq_solver.c lin_eq_solver/lin_eq_solver.h
	@echo Making libraries...
	gcc -I./gnuplot_i -o2 -c -o gnuplot_i.o gnuplot_i/gnuplot_i.c
	gcc -I./lin_eq_solver -o2 -c -o lin_eq_solver.o lin_eq_solver/lin_eq_solver.c

clean:
	rm -rf *~ gridlock *.o
