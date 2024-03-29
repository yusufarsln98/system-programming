#ifndef HELPERS_H
#define HELPERS_H

#include "utils.h"

enum FileDescriptor
{
    STDOUT_FD = 1,
    STDERR_FD = 2,
    STDIN_FD = 0
};

void welcome();
void print_manual_instruction();
void safe_strcpy(char *dest, const char *src);
int parse_input(char *input, char *args[]);
int is_command_valid(char *command);
int is_grade_valid(char *grade);
int is_order_valid(char *order);
int is_sort_by_valid(char *sort_by);
int compare_asc(const void *a, const void *b);
int compare_desc(const void *a, const void *b);
int is_number(char *str);
void get_fullname(char fullname[MAX_LINE], char *args[], int from, int to);
void fork_failed();

#endif