# This makefile uses implicit rules, see: http://ftp.gnu.org/old-gnu/Manuals/make-3.79.1/html_chapter/make_10.html

CC=gcc
CFLAGS=-g -Wall -I include
LDFLAGS=-pthread
EXECS=exercicio4-leitor exercicio4-pai exercicio4-multithreaded exercicio4-LEITOR-p3

all: $(EXECS)

exercicio4-LEITOR-p3: exercicio4-leitor-util.c exercicio4-LEITOR-p3.c  include/exercicio4.h include/exercicio4-leitor-util.h

exercicio4-multithreaded: exercicio4-multithreaded.c exercicio4-leitor-util.c include/exercicio4.h include/exercicio4-leitor-util.h

exercicio4-leitor: exercicio4-leitor.c exercicio4-leitor-util.c include/exercicio4.h include/exercicio4-leitor-util.h

exercicio4-pai: exercicio4-pai.c include/exercicio4.h

clean:
	rm *~ $(EXECS)
