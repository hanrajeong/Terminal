#include "cshell.h"

int list_size = 1000;
int word_size = 1000;

// For making command
Command log_command(Command_list *cmd_list, char* name) {
    // making new command struct object
    Command cmd;
    time_t t;
    struct tm time_info;
    time(&t);
    time_info = *localtime(&t);

    // Store information
    cmd.time = time_info;
    cmd.value = 0;
    cmd.name = name;

    // Put that into the command array
    cmd_list->cmd[cmd_list->size] = cmd;
    cmd_list -> size++;

    return cmd;
}

// To get the input and properly store the inputs

char *getInputLine(){
    char *input_line = NULL;
    size_t len = 0;
    getline(&input_line, &len, stdin);
    return input_line;
}
char **getInput(char *input_line, word_list *word_list, Command_list *cmd_list, Environment *env) {
    char *line = malloc(sizeof(char)* strlen(input_line));
    char *s;
    // It should recognizes lines of the form $<VAR>=<value>
    // So, examine whether the given input starts with $ sign
    // If the input line starts with $ sign
    if(input_line[0] == '$') {
        // Store the line without $sign
        for(int i = 0; input_line[i]!='\0'; i++) {
            line[i] = input_line[i+1];
        }
        s = "=";
        word_list->type = 0;
    }
    else {
        // If not,
        // It recognizes lines of the form <command><arg0><arg1>...<argN>, where <command> is a name of built-in command
        word_list->type = 1;
        line = input_line;
        s = " ";
    }
    // Splite the given line by the "=" if it starts with $, otherwise, split it by " "
    char *word = strtok(line, s);
    while(word != NULL) {
        word_list->word[word_list->size] = strdup(word);
        word_list->size++;
        word = strtok(NULL, s);
    }
    // put the '\0' at the end
    word_list->word[word_list->size-1][strlen(word_list->word[word_list->size-1])-1] = '\0';
    // If it starts with $, so meaning assign the varaible, need to store in in env
    if(s == "=") {
        //check whether the given line is valid or not
        if(word_list->size != 2) {
            word_list->type = 0;
        }
        // According to the instruction,
        // $var = "foo" -> Variable value expected
        // meaning, we have to check for $<VAR>=<value> whether 1. VAR starts with space 2. VAR ends with space
        // 3. value starts with space 4. value ends with space
        else if(word_list->word[0][0] == ' ' || word_list->word[0][strlen(word_list->word[0])-1] == ' ' 
                || word_list->word[1][0] == ' ' || word_list->word[1][strlen(word_list->word[1])-1] == ' ') {}
        else {
            // properly assigned
            env->name = word_list->word[0];
            env->value = word_list->word[1];
        }
    }
    return word_list->word;
}

int checkInput(word_list *word_list, Command_list *cmd_list, Environment *env, Color *Color) {
    // four built-in commands exit, print, theme and log. 
    char **command_line = word_list->word;
    char *command_word = command_line[0];
    // 1. exit
    if(strcmp(command_word, "exit")==0) {
        printf("Bye!\n");
        // It is properly given = 0
        return 0;
    }
    // 2. print
    else if(strcmp(command_word, "print")==0) {
        if(word_list->size == 1) {
            // if there is only one word stored, it means it is not properly given
            printf("Missing keyword or command, or permission problem\n");
            // Have some error = 1
            return 1;
        }
        // Command log_command(Command_list *cmd_list, char* name)
        // put those in the log
        log_command(cmd_list, command_word);
        // If the user ask to print the assigned var value,
        // ex. $var=foo, print $var <= so check whether the second word starts with $
        if(command_line[1][0] == '$') {
            // store the given value
            char* assigned_name = malloc(sizeof(char)*strlen(command_line[1]));
            for(int i = 0; command_line[1][i]!='\0'; i++) {
                assigned_name[i] = command_line[1][i+1]; // i+1 because of the $ sign
            }
            // If the required variable is already stored
            if(strcmp(env->name, assigned_name)==0) {
                printf("%s", env->value);
            }
            // if is not stored
            else {
                int i = 1;
                while(command_line[i]!=NULL) {
                    printf("%s", command_line[i]);
                    if(command_line[i+1]!=NULL) {
                        printf(" ");
                    }
                    i+=1;
                }
            }
            free(assigned_name);
        }
        else {
            int i = 1;
            while(command_line[i]!=NULL) {
                printf("%s", command_line[i]);
                if(command_line[i+1]!=NULL) {
                    printf(" ");
                }
                i+=1;
            }
        }
        printf("\n");
        return 1;
    }
    // 3. theme
    // ex. theme red => turn the text color into red
   else if(strcmp(command_word, "theme")==0) {
       // Cshell should support three colours (red, green and blue)
       if(word_list->size == 1) {
            printf("Missing keyword or command, or permission problem\n");
            return 1;           
       }
       else if(word_list->size == 2) {
           int length = strlen(command_line[1]);
           char* lower = malloc(sizeof(char)*length);
           for(int i = 0; i<length; i++) {
               lower[i] = tolower((unsigned char)command_line[1][i]);
           }
           //red
           if(strcmp(lower, "red")==0) {
               printf("\033[1;31m");
               Color->font_color = 1;
               log_command(cmd_list, command_word);
           }
           // green
           else if(strcmp(lower, "green")==0) {
               printf("\033[1;32m");
               Color->font_color = 2;
               log_command(cmd_list, command_word);
           }
           // blue
           else if(strcmp(lower, "blue")==0) {
               printf("\033[1;34m");
               Color->font_color = 3;
               log_command(cmd_list, command_word);
           }
           else {
               printf("Unsupported theme: %s\n", command_line[1]);
               return 1;
           }
            // log_command(cmd_list, command_word);
       }
       else {
           printf("Unsupported theme: ");
           return 1;
       }
       return 1;
   }
   // 4. log
   else if(strcmp(command_word, "log")==0) {
       for(int i=0; i<cmd_list->size; i++) {
           printf("%s %s %d\n", asctime(&cmd_list->cmd[i].time), cmd_list->cmd[i].name, cmd_list->cmd[i].value);
       } 
       log_command(cmd_list, command_word);
       return 1;
   }
    // If $<VAR>=<value>,
    if(word_list->type == 0) {
        // If there is an error,
        if(word_list->word[0][0] == ' ' || word_list->word[0][strlen(word_list->word[0])-1] == ' ' 
                || word_list->word[1][0] == ' ' || word_list->word[1][strlen(word_list->word[1])-1] == ' ') {
                    printf("Variable value expected\n");
                    return 1;
                }
        else {
            // total length = variable name length + value length + $ + =
            int l = strlen(command_line[0]) + strlen(command_line[1]) + 2;
            char log_temp[l];
            log_temp[0] = '$';
            strcat(log_temp, command_line[0]);
            strcat(log_temp, "=");
            strcat(log_temp, command_line[1]);
            log_command(cmd_list, log_temp);
            return 1;
        }
    }
    // If <command><arg0><arg1>...<argN>,
    else {
        int fd[2];
        pid_t pid;
        pipe(fd);
        pid = fork();
        // Error  case
        if(pid < 0) {
            printf("Error : pipe\n");
            exit(1);
        }
        // Child process
        else if(pid == 0) {
            // If the input is correct, execute it
            execvp(command_line[0], &command_line[0]);
            // If the execvp cannot be executed successfully,
            // Return the error message
            printf("Error : unexpected input\n");
            exit(0);
        }
        // Parent process
        else {
            wait(NULL);
        }
        log_command(cmd_list, command_word);
        return 1;
    }
}

void script(char **argv, Command_list *cmd_list, Environment *env, Color *Color) {
        // Read the input
        // If the input argv[1] doesn't exist
        // Source for this idea : https://jdm.kr/blog/76
        if((access(argv[1], F_OK)) < 0) {
            printf("Inappropriate input");
            return;
        }
        else {
            FILE *input_file;
            input_file = fopen(argv[1], "r");
            char input_lines[1000];
            while(fgets(input_lines, sizeof(input_lines), input_file) != NULL) {
                word_list* line = malloc(sizeof(word_list));
                line->word = malloc(word_size * sizeof(char*));
                line->size = 0;
                // If it is full, need to resize,
                if(cmd_list->size == list_size) {
                    list_size = list_size * 2;
                    cmd_list->cmd = realloc(cmd_list->cmd, list_size*sizeof(Command));
                }
                char** handled_input = getInput(input_lines, line, cmd_list, env);
                int checked_result = checkInput(line, cmd_list, env, Color);
                free(line->word);
                free(line);
                if(checked_result == 0) {
                    // It means there is some erro
                    break;
                }
            }
            free(cmd_list->cmd);
            free(cmd_list);
            free(Color);
            free(env);
            free(input_file);
        }
}

int main(int argc, char** argv) {
    // declare the variables
    Command_list *com_list = malloc(sizeof(Command_list));
    com_list->cmd = malloc(list_size * sizeof(Command));
    com_list->size = 0;
    Color *color= malloc(sizeof(Color));
    Environment *env = malloc(sizeof(Environment));
    // printf("%d\n", argc);
    if(argc == 2) {
        script(argv, com_list, env, color);
    }
    else {
        int flag = 1;
        printf("cshell$ ");
        while(flag) {
            word_list * line = malloc(sizeof(word_list));
            line->word = malloc(word_size * sizeof(char*));
            line->size = 0;
            // If it is full, need to resize,
            if(com_list->size == list_size) {
                list_size = list_size * 2;
                com_list->cmd = realloc(com_list->cmd, list_size*sizeof(Command));
            }
            char* input_line = getInputLine();
            char** handled_input = getInput(input_line, line, com_list, env);
            switch(color->font_color)
            {
                case 1:
                // red
                    printf("%c[1;31m", 27);
                    break;
                case 2:
                // green
                    printf("%c[1;32m", 27);
                    break;
                case 3:
                // blue
                    printf("%c[1;34m", 27);
                    break;
                default:
                    break;
            }
            int checked = checkInput(line, com_list, env, color);
            for(int i = 0; i < word_size; i++) {
                line->word[i] = NULL;
            }
            free(line->word);
            free(line);
            if(checked == 0) {
                // meaning input is exit
                break;
            }
            printf("cshell$ ");
        }
        free(com_list->cmd);
        free(com_list);
        free(color);
        free(env);
    }
}
