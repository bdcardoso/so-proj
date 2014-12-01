CC = gcc
EX5_ESCRITOR_FILES = ex5_escritor.c
EX5_ESCRITOR_EXE = ex5_escritor
EX5_LEITOR_FILES = ex5_leitor.c
EX5_LEITOR_EXE = ex5_leitor
EX5_FINAL_FILES = ex5_final.c
EX5_FINAL_EXE = ex5_final

THREAD_FLAGS = -pthread

all:build1 build2 build3

build1: $(EX5_ESCRITOR_FILES);$(CC) -o $(EX5_ESCRITOR_EXE) $(EX5_ESCRITOR_FILES) $(THREAD_FLAGS)

build2: $(EX5_LEITOR_FILES);$(CC) -o $(EX5_LEITOR_EXE) $(EX5_LEITOR_FILES) $(THREAD_FLAGS)

build3: $(EX5_FINAL_FILES);$(CC) -o $(EX5_FINAL_EXE) $(EX5_FINAL_FILES) $(THREAD_FLAGS)

clean:rm -f *.o core

rebuild: clean build
