#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
// #include <stdint.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
// #include <linux/kernel.h>

// Each environment variable could be stored in following struct
// Given in the assignment instruction
typedef struct {
    char *name;
    char *value;
} Environment;

// To store each executed command struct
// Given in the assignment instruvtion
typedef struct {
    char *name;
    struct tm time;
    char value; // 다시 체크해보기
} Command;

typedef struct {
    Command *cmd;
    int size;
} Command_list;

typedef struct {
    char **word;
    int size;
    int type;
} word_list;

typedef struct {
    int font_color;
} Color;


