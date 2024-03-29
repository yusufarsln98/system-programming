#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>

#include "utils.h"
#include "helpers.h"

const char *COMMANDS[] = {
    [GTU_STUDENT_GRADES] = "gtuStudentGrades",
    [ADD_STUDENT_GRADE] = "addStudentGrade",
    [SEARCH_STUDENT] = "searchStudent",
    [SORT_ALL] = "sortAll",
    [SHOW_ALL] = "showAll",
    [LIST_GRADES] = "listGrades",
    [LIST_SOME] = "listSome",
    [EXIT] = "exit"};

const char *GRADES[] = {
    [AA] = "AA",
    [BA] = "BA",
    [BB] = "BB",
    [CB] = "CB",
    [CC] = "CC",
    [DC] = "DC",
    [DD] = "DD",
    [FD] = "FD",
    [FF] = "FF",
    [NA] = "NA"};

const char *ORDERS[] = {
    [ASC] = "asc",
    [DESC] = "desc"};

const char *SORT_BY[] = {
    [NAME] = "name",
    [GRADE] = "grade"};

int LOG_FD;

void printManual()
{
    printf("\n+-----GTU Student Grade Management System Manual------+\n\n");
    printf("gtuStudentGrades [filename]\n");
    printf("    -Creates a new file with the given name\n\n");
    printf("addStudentGrade [filename] [name1 ...] [lastname1 ...] [grade]\n");
    printf("    -Adds a new student with the given name and grade to the file\n");
    printf("    -Grade options: AA, BA, BB, CB, CC, DC, DD, FD, FF, NA\n\n");
    printf("searchStudent [filename] [name1 ...] [lastname1 ...]\n");
    printf("    -Searches the file for the given student and displays the name and grade\n\n");
    printf("sortAll [filename]\n");
    printf("    -Sorts the file in ascending order by name.\n\n");
    printf("sortAll [filename] [acs/desc] [name/grade]\n");
    printf("    -Sorts the file in ascending/descending order by name/grade.\n\n");
    printf("showAll [filename]\n");
    printf("    -Displays all the students and their grades\n\n");
    printf("listGrades [filename]\n");
    printf("    -Displays the grades of the first 5 students\n\n");
    printf("listSome [filename] [numofEntries] [pageNumber]\n");
    printf("    -Displays the grades of the students in the given page\n\n");
    printf("Note: Max argument count is %d\n\n", MAX_ARGS);
    printf("exit\n");
    printf("    -Terminates the program\n");
    printf("+-----------------------------------------------------+\n\n");

    logToFile(COMMANDS[GTU_STUDENT_GRADES], TRUE, "Manual printed");
}

// check if student is exist in the file
int isStudentExist(int fd, char *fullname)
{
    char ch[1];          // for reading char by char
    char line[MAX_LINE]; // setting line by char
    char fullname_file[MAX_LINE];
    int i; // loop variable

    line[0] = '\0';
    fullname_file[0] = '\0';
    i = 0;

    // if the student is found, return TRUE
    while (read(fd, ch, 1) > 0)
    {
        if (ch[0] == '\n')
        {
            line[i] = '\0';                        // end the line with null terminator
            sscanf(line, "%[^,],", fullname_file); // helper for parsing the line by ','
            if (strcmp(fullname, fullname_file) == 0)
            {
                return TRUE;
            }

            line[0] = '\0';
            fullname_file[0] = '\0';
            i = 0;
        }
        else
        {
            line[i] = ch[0];
            i++;
        }
    }

    return FALSE;
}

void addStudent(int fd, char *fullname, char *grade)
{
    char student[MAX_LINE];

    printf("\n+-------------------Add Student-----------------------+\n\n");

    // if student is already exist, return
    if (isStudentExist(fd, fullname))
    {
        printf("Student already exists: %s\n\n", fullname);
        logToFile(COMMANDS[ADD_STUDENT_GRADE], FALSE, "Student already exists");
        return;
    }

    student[0] = '\0';
    sprintf(student, "%s, %s\n", fullname, grade); // helper for concatenating strings

    if (write(fd, student, strlen(student)) == -1)
    {
        printf("Error: Could not write to file\n");
        logToFile(COMMANDS[ADD_STUDENT_GRADE], FALSE, "Could not write to file");
    }
    else
    {
        printf("Student grade added: %s, %s\n\n", fullname, grade);
        logToFile(COMMANDS[ADD_STUDENT_GRADE], TRUE, "Student grade added");
    }
}

void searchStudent(int fd, char *fullname)
{
    char ch[1];          // for reading char by char
    char line[MAX_LINE]; // setting line by char
    char fullname_file[MAX_LINE];
    char grade[3];
    int found = FALSE;
    int i; // loop variable

    printf("\n+-------------------Search Student--------------------+\n\n");
    // set the file pointer to the beginning of the file
    line[0] = '\0';
    fullname_file[0] = '\0';
    i = 0;
    while (read(fd, ch, 1) > 0)
    {
        // format: yusuf emre arslan, AA
        if (ch[0] == '\n')
        {
            line[i] = '\0';                        // end the line with null terminator
            sscanf(line, "%[^,],", fullname_file); // helper for parsing the line by ','
            sscanf(line, "%*[^,], %s", grade);

            // compare the full name with the full name from the file
            if (strcmp(fullname, fullname_file) == 0)
            {
                printf("Student found: %s, %s\n\n", fullname_file, grade);
                logToFile(COMMANDS[SEARCH_STUDENT], TRUE, "Student found");
                found = TRUE;
                break;
            }

            // clear the line
            line[0] = '\0';
            fullname_file[0] = '\0';
            i = 0;
        }
        else
        {
            line[i] = ch[0];
            i++;
        }
    }

    if (!found)
    {
        printf("Student not found: %s\n\n", fullname);
        logToFile(COMMANDS[SEARCH_STUDENT], FALSE, "Student not found");
    }
}

void sortAll(int fd, char *order, char *sort_by)
{
    char students[MAX_STUDENTS][MAX_LINE]; // array to store the students
    char ch[1];                            // for reading char by char
    char line[MAX_LINE];                   // setting line by char
    char fullname[MAX_LINE];
    char grade[3];
    int student_count;
    int i;
    char message[MAX_LINE];

    printf("\n+-------------------Sort All--------------------------+\n\n");

    line[0] = '\0';
    fullname[0] = '\0';
    i = 0;
    student_count = 0;

    // read the file and store the students in the array
    while (read(fd, ch, 1) > 0)
    {
        if (ch[0] == '\n')
        {
            line[i] = '\0'; // end the line with null terminator
            // format: yusuf emre arslan, AA
            // get the fullname and grade from the line
            fullname[0] = '\0';
            sscanf(line, "%[^,],", fullname);

            grade[0] = '\0';
            sscanf(line, "%*[^,], %s", grade);

            // merge the fullname and grade into a single string
            // if the sort_by is name, merge the fullname and grade
            // if the sort_by is grade, merge the grade and fullname
            students[student_count][0] = '\0';
            if (strcmp(sort_by, SORT_BY[NAME]) == 0)
            {
                strcat(students[student_count], fullname);
                strcat(students[student_count], ", ");
                strcat(students[student_count], grade);
            }
            else
            {
                strcat(students[student_count], grade);
                strcat(students[student_count], ", ");
                strcat(students[student_count], fullname);
            }

            line[0] = '\0';     // clear the line
            fullname[0] = '\0'; // clear the fullname
            i = 0;              // reset the index
            student_count++;    // increment the index
        }
        else
        {
            line[i] = ch[0]; // set the line by char
            i++;             // increment the index
        }
    }
    // sort the students
    if (strcmp(order, ORDERS[ASC]) == 0)
    {
        qsort(students, student_count, MAX_LINE, compare_asc);
    }
    else if (strcmp(order, ORDERS[DESC]) == 0)
    {
        qsort(students, student_count, MAX_LINE, compare_desc);
    }
    else
    {
        printf("Error: Invalid order: %s\n", order);
        logToFile(COMMANDS[SORT_ALL], FALSE, "Invalid order");
        return;
    }

    // print the sorted students
    printf("Sorted students:\n");
    for (i = 0; i < student_count; i++)
    {
        printf("%s\n", students[i]);
    }

    message[0] = '\0';
    sprintf(message, "Sorted by %s in %s order", sort_by, order);
    logToFile(COMMANDS[SORT_ALL], TRUE, message);
}

void showAll(int fd)
{
    char ch[1];          // for reading char by char
    char line[MAX_LINE]; // setting line by char
    int i;               // loop variable

    printf("\n+-------------------Show All--------------------------+\n\n");

    line[0] = '\0';
    i = 0;

    // read the file and print the students
    while (read(fd, ch, 1) > 0)
    {
        if (ch[0] == '\n')
        {
            line[i] = '\0'; // end the line with null terminator
            printf("%s\n", line);

            line[0] = '\0'; // clear the line
            i = 0;          // reset the index
        }
        else
        {
            line[i] = ch[0]; // set the line by char
            i++;             // increment the index
        }
    }

    logToFile(COMMANDS[SHOW_ALL], TRUE, "All students shown");
}

void listGrades(int fd)
{
    // read first 5 students and print their grades or end of file
    char ch[1];          // for reading char by char
    char line[MAX_LINE]; // setting line by char
    int i;               // loop variable
    int student_count;   // count of students

    printf("\n+-------------------Show All--------------------------+\n\n");

    line[0] = '\0';
    i = 0;
    student_count = 0;

    // read the file and print the students
    while (read(fd, ch, 1) > 0 && student_count < 5)
    {
        if (ch[0] == '\n')
        {
            line[i] = '\0'; // end the line with null terminator
            printf("%d - %s\n", ++student_count, line);

            line[0] = '\0'; // clear the line
            i = 0;          // reset the index
        }
        else
        {
            line[i] = ch[0]; // set the line by char
            i++;             // increment the index
        }
    }

    logToFile(COMMANDS[LIST_GRADES], TRUE, "First 5 students shown");
}

void listSome(int fd, int num_of_entries, int page_number)
{
    // read the students and print their grades
    char ch[1];          // for reading char by char
    char line[MAX_LINE]; // setting line by char
    int i;               // loop variable
    int student_count;   // count of students
    int start_index;     // start index of the page
    int end_index;       // end index of the page
    char message[MAX_LINE];

    printf("\n+-------------------List Some-------------------------+\n\n");

    line[0] = '\0';
    i = 0;
    student_count = 0;
    start_index = (page_number - 1) * num_of_entries;
    end_index = start_index + num_of_entries;

    // read the file and print the students
    while (read(fd, ch, 1) > 0 && student_count < end_index)
    {
        if (ch[0] == '\n')
        {
            line[i] = '\0'; // end the line with null terminator
            student_count++;

            if (student_count > start_index && student_count <= end_index)
            {
                printf("%s\n", line);
            }

            line[0] = '\0'; // clear the line
            i = 0;          // reset the index
        }
        else
        {
            line[i] = ch[0]; // set the line by char
            i++;             // increment the index
        }
    }

    message[0] = '\0';
    sprintf(message, "Page %d with %d entries shown", page_number, num_of_entries);
    logToFile(COMMANDS[LIST_SOME], TRUE, message);
}

void logToFile(const char *command, int success, char *result)
{
    // fork a new process
    pid_t pid = fork();

    if (pid == -1)
    {
        // error occurred while forking
        printf("Error: Could not fork a new process\n");
    }
    else if (pid == 0)
    {
        // child process
        // write the log to the file
        // log format: [command],[filename],[success/failure],[message]
        char log[MAX_LOG_LINE];

        log[0] = '\0';
        sprintf(log, "%s,%s,%s\n", command, success ? "success" : "failure", result);

        if (write(LOG_FD, log, strlen(log)) == -1)
        {
            printf("Error: Could not write to log file\n");
        }

        // exit the child process
        exit(0);
    }
    else
    {
        // parent process
        // continue with the rest of the code
    }
}