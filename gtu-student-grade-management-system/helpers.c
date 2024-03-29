#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#include "utils.h"

void welcome()
{
    printf("\n+----------------------------------------------------------+\n");
    printf("|      Welcome to GTU Student Grade Management System      |\n");
    printf("| Type 'gtuStudentGrades' for usage. ('exit' to terminate) |\n");
    printf("+----------------------------------------------------------+\n\n");
}

void print_manual_instruction()
{
    printf("Type 'gtuStudentGrades' for usage\n\n");
}

void safe_strcpy(char *dest, const char *src)
{
    dest[0] = '\0';
    strcpy(dest, src);
}

int parse_input(char *input, char *args[])
{
    int i = 0;

    char *token = strtok(input, " \n");
    while (token != NULL)
    {
        args[i] = token;
        token = strtok(NULL, " \n");
        i++;
    }
    args[i] = NULL;

    return i;
}

int is_command_valid(char *command)
{
    int i = 0;

    while (COMMANDS[i] != NULL)
    {
        if (strcmp(command, COMMANDS[i]) == 0)
        {
            return TRUE;
        }
        i++;
    }

    return FALSE;
}

int is_grade_valid(char *grade)
{
    int i = 0;

    while (GRADES[i] != NULL)
    {
        if (strcmp(grade, GRADES[i]) == 0)
        {
            return TRUE;
        }
        i++;
    }

    return FALSE;
}

int is_order_valid(char *order)
{
    if (strcmp(order, "asc") == 0 || strcmp(order, "desc") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

int is_sort_by_valid(char *sort_by)
{
    if (strcmp(sort_by, "name") == 0 || strcmp(sort_by, "grade") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

// compare functions for qsort
int compare_asc(const void *a, const void *b)
{
    return strcmp((char *)a, (char *)b);
}

int compare_desc(const void *a, const void *b)
{
    return strcmp((char *)b, (char *)a);
}

int is_number(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return FALSE;
        }
        i++;
    }

    return TRUE;
}

// from: the index where the name starts
// to: the index where the lastname ends
void get_fullname(char fullname[MAX_LINE], char *args[], int from, int to)
{
    int i;

    fullname[0] = '\0'; // empty the fullname
    // merge the name and last name into a single string
    for (i = from; i <= to; i++)
    {
        strcat(fullname, args[i]);
        if (i != to)
        {
            strcat(fullname, " ");
        }
    }
    fullname[strlen(fullname)] = '\0'; // null terminate the string
}

void fork_failed()
{
    printf("Error: Fork failed\n");
    logToFile("", 0, "Fork failed");
}