#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdarg.h>

#include "helpers.h"
#include "utils.h"

int main()
{
    char input[MAX_LINE]; // to store the user input
    char *args[MAX_ARGS]; // to store the parsed user input
    int arg_count;
    char fullname[MAX_LINE];
    char order[4];
    char sort_by[6];
    int fd; // file descriptor for grades file
    // int LOG_FD; // file descriptor for log file
    pid_t pid;

    welcome();

    // open the log file in append mode, if it does not exist, create a new one
    // log format: [command],[success/failure],[message]

    // LOG_FD comes from utils.h as global to don't pass it to every function
    // since without opening the log file, the program cannot continue, I used a global variable
    LOG_FD = open(LOG_FILENAME, O_CREAT | O_RDWR | O_APPEND, 0644);
    if (LOG_FD == -1)
    {
        printf("Error: log file could not be opened\n");
        exit(1);
    }

    input[0] = '\0'; // empty the input
    while (fgets(input, MAX_LINE, stdin) != NULL)
    {
        // parse input and get argument number
        arg_count = parse_input(input, args);

        // check if too many arguments are given
        if (arg_count > MAX_ARGS)
        {
            printf("Error: Too many argument count '%d'\n", arg_count);
            logToFile("", FALSE, "Too many arguments");
            print_manual_instruction();
            continue;
        }
        // check if number of arguments is less than minimum (continue if [whitespace + enter] is pressed)
        else if (arg_count < MIN_ARGS)
        {
            continue;
        }

        if (strcmp(args[0], COMMANDS[GTU_STUDENT_GRADES]) == 0)
        {
            // Either one or two arguments are valid
            if (arg_count > 2)
            {
                printf("Error: Invalid argument count for gtuStudentGrades\n");
                logToFile(COMMANDS[GTU_STUDENT_GRADES], FALSE, "Invalid argument count");
                print_manual_instruction();
                continue;
            }

            // if one argument is given, print the manual
            if (arg_count == 1)
            {
                printManual();
            }
            else
            {
                // usage: gtuStudentGrades [filename]
                pid = fork();
                if (pid == -1)
                {
                    fork_failed();
                }
                // in child process, create a new file with the given name
                else if (pid == 0) // child process
                {
                    // create a new file with the given name, if it does not exist
                    fd = open(args[1], O_CREAT | O_RDWR | O_EXCL, 0644);
                    if (fd == -1)
                    {
                        printf("Error: '%s' file already exists\n", args[1]);
                        logToFile(args[0], FALSE, "File already exists");
                        exit(1);
                    }

                    printf("File created: %s\n", args[1]);
                    logToFile(args[0], TRUE, "File created");
                    close(fd);

                    exit(0);
                }
                else
                {
                    // just move on, allows application to run in parallel
                }
            }
        }
        else if (strcmp(args[0], COMMANDS[ADD_STUDENT_GRADE]) == 0)
        {
            // usage: addStudentGrade [filename] [name1 ...] [lastname1 ...] [grade]
            if (arg_count < 5)
            {
                printf("Error: Invalid argument count for addStudentGrade\n");
                logToFile(args[0], FALSE, "Invalid argument count");
                print_manual_instruction();
                continue;
            }

            // check if grade is valid
            if (!is_grade_valid(args[arg_count - 1]))
            {
                printf("Error: Invalid grade: %s", args[arg_count - 1]);
                logToFile(args[0], FALSE, "Invalid grade");
                print_manual_instruction();
                continue;
            }

            pid = fork();
            if (pid == -1)
            {
                fork_failed();
            }
            else if (pid == 0) // child process
            {
                // open the file in append mode
                fd = open(args[1], O_APPEND | O_RDWR);
                if (fd == -1)
                {
                    printf("Error: '%s' file does not exist\n", args[1]);
                    logToFile(args[0], FALSE, "File does not exist");
                    exit(1);
                }

                get_fullname(fullname, args, 2, arg_count - 2);

                addStudent(fd, fullname, args[arg_count - 1]);

                close(fd);
                exit(0);
            }
            else // parent process
            {
                // just move on, allows application run in parallel
            }
        }
        else if (strcmp(args[0], COMMANDS[SEARCH_STUDENT]) == 0)
        {
            // usage: searchStudent [filename] [name1 ...] [lastname1 ...]
            if (arg_count < 4)
            {
                printf("Error: Invalid argument count for searchStudent\n");
                logToFile(args[0], FALSE, "Invalid argument count");
                print_manual_instruction();
                continue;
            }

            pid = fork();
            if (pid == -1)
            {
                fork_failed();
            }
            else if (pid == 0) // child process
            {
                // open the file in read mode
                fd = open(args[1], O_RDONLY);
                if (fd == -1)
                {
                    printf("File does not exist: %s\n\n", args[1]);
                    logToFile(args[0], FALSE, "File does not exist");
                    exit(1);
                }

                get_fullname(fullname, args, 2, arg_count - 1);

                searchStudent(fd, fullname);

                close(fd);
                exit(0);
            }
            else
            { // parent process
              // just move on, allow application run in parallel
            }
        }
        else if (strcmp(args[0], COMMANDS[SORT_ALL]) == 0)
        {
            /*
            usage:
            sortAll [filename]
                -Sorts the file in ascending order by name.

            sortAll [filename] [acs/desc] [name/grade]
                -Sorts the file in ascending/descending order by name/grade.
            */
            if (arg_count < 2 || arg_count > 4)
            {
                printf("Error: Invalid argument count for sortAll\n");
                logToFile(args[0], FALSE, "Invalid argument count");
                print_manual_instruction();
                continue;
            }

            pid = fork();
            if (pid == -1)
            {
                fork_failed();
            }
            else if (pid == 0) // child process
            {
                fd = open(args[1], O_RDONLY); // open file on read only mode
                if (fd == -1)
                {
                    printf("Error: File does not exist: %s\n\n", args[1]);
                    logToFile(args[0], FALSE, "File does not exist");
                    exit(1);
                }

                if (arg_count == 2)
                {
                    safe_strcpy(order, ORDERS[ASC]);
                    safe_strcpy(sort_by, SORT_BY[NAME]);

                    sortAll(fd, order, sort_by);
                }
                else if (arg_count == 4)
                {
                    // check if order is valid
                    if (!is_order_valid(args[2]))
                    {
                        printf("Error: Invalid order: %s\n", args[2]);
                        logToFile(args[0], FALSE, "Invalid order");
                        print_manual_instruction();
                        continue;
                    }

                    // check if sort_by is valid
                    if (!is_sort_by_valid(args[3]))
                    {
                        printf("Error: Invalid sort_by: %s\n", args[3]);
                        logToFile(args[0], FALSE, "Invalid sort_by");
                        print_manual_instruction();
                        continue;
                    }

                    sortAll(fd, args[2], args[3]);
                }

                close(fd);
                exit(0);
            }
            else
            {
                // just move on, allows application run in parallel
            }
        }
        else if (strcmp(args[0], COMMANDS[SHOW_ALL]) == 0)
        {
            // usage: showAll [filename]
            if (arg_count != 2)
            {
                printf("Error: Invalid argument count for showAll\n");
                logToFile(args[0], FALSE, "Invalid argument count");
                print_manual_instruction();
                continue;
            }

            pid = fork();
            if (pid == -1)
            {
                fork_failed();
            }
            else if (pid == 0) // child process
            {
                fd = open(args[1], O_RDONLY); // open file on read only mode
                if (fd == -1)
                {
                    printf("Error: File does not exist: %s\n\n", args[1]);
                    logToFile(args[0], FALSE, "File does not exist");
                    exit(1);
                }

                showAll(fd);

                close(fd);
                exit(0);
            }
            else
            {
                // just move on, allows application run in parallel
            }
        }
        else if (strcmp(args[0], COMMANDS[LIST_GRADES]) == 0)
        {
            // usage: listGrades [filename]
            if (arg_count != 2)
            {
                printf("Error: Invalid argument count for listGrades\n");
                logToFile(args[0], FALSE, "Invalid argument count");
                print_manual_instruction();
                continue;
            }

            pid = fork();
            if (pid == -1)
            {
                fork_failed();
            }
            else if (pid == 0) // child process
            {
                fd = open(args[1], O_RDONLY); // open file on read only mode
                if (fd == -1)
                {
                    printf("Error: File does not exist: %s\n\n", args[1]);
                    logToFile(args[0], FALSE, "File does not exist");
                    exit(1);
                }

                listGrades(fd);

                close(fd);
                exit(0);
            }
            else
            {
                // just move on, allows application run in parallel
            }
        }
        else if (strcmp(args[0], COMMANDS[LIST_SOME]) == 0)
        {
            // usage: listSome [filename] [numofEntries] [pageNumber]
            if (arg_count != 4)
            {
                printf("Error: Invalid argument count for listSome\n");
                logToFile(args[0], FALSE, "Invalid argument count");
                print_manual_instruction();
                continue;
            }

            // check if numofEntries and pageNumber are valid numbers
            if (!is_number(args[2]) || !is_number(args[3]))
            {
                printf("Error: Invalid numofEntries or pageNumber\n");
                logToFile(args[0], FALSE, "Invalid numofEntries or pageNumber");
                print_manual_instruction();
                continue;
            }

            pid = fork();
            if (pid == -1)
            {
                fork_failed();
            }
            else if (pid == 0) // child process
            {
                fd = open(args[1], O_RDONLY); // open file on read only mode
                if (fd == -1)
                {
                    printf("Error: File does not exist: %s\n\n", args[1]);
                    logToFile(args[0], FALSE, "File does not exist");
                    exit(1);
                }

                listSome(fd, atoi(args[2]), atoi(args[3]));

                close(fd);
                exit(0);
            }
            else
            {
                // just move on, allows application run in parallel
            }
        }
        else if (strcmp(args[0], COMMANDS[EXIT]) == 0)
        {
            // close the log file
            printf("\nProgram terminated, Goodbye!\n\n");
            logToFile(args[0], TRUE, "Program terminated");
            close(LOG_FD);
            exit(0);
        }
        else
        {
            printf("Error: Invalid command '%s'\n", args[0]);
            logToFile(args[0], FALSE, "Invalid command");
            print_manual_instruction();
        }
        input[0] = '\0'; // empty the input
    }

    close(LOG_FD);

    return 0;
}
