#ifndef EXERCICIO4_LEITOR_UTIL_H
#define EXERCICIO4_LEITOR_UTIL_H


int reader(char  *file_to_open);

void* reader_wrapper(void* ptr);

void init_myfiles ();

int isLineLegal(const char* str);

#endif
