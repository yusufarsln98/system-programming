#ifndef UTILS_H
#define UTILS_H

#define TRUE 1
#define FALSE 0

#define MAX_LINE 100
#define MAX_LOG_LINE 200
#define MAX_STUDENTS 100
#define MAX_ARGS 10
#define MIN_ARGS 1
#define LOG_FILENAME "log.txt"

typedef enum
{
    GTU_STUDENT_GRADES,
    ADD_STUDENT_GRADE,
    SEARCH_STUDENT,
    SORT_ALL,
    SHOW_ALL,
    LIST_GRADES,
    LIST_SOME,
    EXIT
} Command;

typedef enum
{
    AA,
    BA,
    BB,
    CB,
    CC,
    DC,
    DD,
    FD,
    FF,
    NA
} Grade;

typedef enum
{
    ASC,
    DESC
} Order;

typedef enum
{
    NAME,
    GRADE
} SortBy;

// constants for input validation
extern const char *COMMANDS[];
extern const char *GRADES[];
extern const char *ORDERS[];
extern const char *SORT_BY[];

extern int LOG_FD;

int isStudentExist(int fd, char *fullname);
void printManual();
void addStudent(int fd, char *fullname, char *grade);
void searchStudent(int fd, char *fullname);
void sortAll(int fd, char *order, char *sort_by);
void showAll(int fd);
void listGrades(int fd);
void listSome(int fd, int num_of_entries, int page_number);
void logToFile(const char *command, int success, char *result);

#endif
