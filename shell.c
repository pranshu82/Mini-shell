#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>

//cwd = current working directory
//old_cwd = previous working directory 
//program directory 
//history location = address of history.txt
char cwd[256];
char old_cwd[256];
char program_directory[256];
char history_location[256];

//various internal commands
int shell_cd(char **args,char*line);
int shell_help(char **args,char*line);
int shell_exit(char **args,char*line);
int shell_quit(char **args,char*line);
int shell_X(char **args,char*line);
int shell_pwd(char **args,char*line);
int shell_setenv(char **args,char* line);
int shell_printenv(char **args,char* line);
int shell_$(char **args,char* line);
int shell_history();
int shell_echo(char **args,char* line);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "quit",
  "X",
  "pwd",
  "history",
  "setenv",
  "printenv",
  "echo"
};

//builtin function array
int (*builtin_func[]) (char **,char *) = {
  &shell_cd,
  &shell_help,
  &shell_exit,
  &shell_quit,
  &shell_X,
  &shell_pwd,
  &shell_history,
  &shell_setenv,
  &shell_printenv,
  &shell_echo
};

//number of builtin function
int shell_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  ************************************************All Builtin function implementatiom**************************************************
*/

//$USER 
int shell_$(char **args,char* line){
  int i=1;
  char var[64];;
  while(args[0][i] != '\0'){
    var[i-1] = args[0][i];
    i++;
  }
  var[i-1] = '\0';
  if(getenv(var) != NULL){
    printf("%s\n",getenv(var));
  }
  return 1;

}

//setenv USER = pranshu
int shell_setenv(char **args,char* line){
  if(args[1] == NULL || args[2] == NULL  || args[3] == NULL ){
    return 1;
  }
  if((strcmp(args[2],"=") == 0)){

    if(setenv(args[1], args[3], 1))
    {
      perror("shell");
      return 1;
    }
  }else{
    printf("setenv:syntax error\n");
  }
  return 1;
}

//printenv USER
int shell_printenv(char **args,char* line){
  if(args[1] == NULL){
    printf("HOME=%s\n",getenv("HOME"));
    printf("PATH=%s\n",getenv("PATH"));
    printf("USER=%s\n",getenv("USER"));
    printf("SHELL=%s\n",getenv("SHELL"));
    printf("TERM=%s\n",getenv("TERM"));
  }else{
    int i=1;
    while(args[i] != NULL){
      if(getenv(args[i]) != NULL){
        printf("%s\n",getenv(args[i]));
      }
      i++;
    }
  }
  return 1;
}

//echo hello 
int shell_echo(char **args,char* line)
{
  char var[1024];

  int i=5,index=0;
  while(line[i] != '\0'){

    if(line[i] == '"')
    {
      i++;
      while(line[i] != '"'){
        var[index] = line[i];
        index++;
        i++;
      }
      if(line[i+1] == ' '){
        var[index] = ' ';
        index++;
      }
    }

    if(line[i] != ' ' && line[i] != '"' && line[i] != '$'){
      var[index] = line[i];
      index++;
      if(line[i+1] == ' '){
        var[index] = ' ';
        index++;
      }
    }
    
    if(line[i] == '$'){
      i++;
      char envar[64];
      int j=0;
      while(line[i] != ' ' && line[i] != '\0'){
        envar[j] = line[i];
        j++;
        i++;
      }
      i--;

      if(line[i+1] == '\0'){
        envar[j-1] = '\0';
      }else{
        envar[j] = '\0';
      }
      
      if(getenv(envar) != NULL){
        char *arg = getenv(envar);
        j=0;
        while(arg[j] != '\0'){
          var[index] = arg[j];
          j++;
          index++;
        }
        if(line[i+1] == ' '){
          var[index] = ' ';
          index++;
        }
        if(line[i+1] == '\0'){
          var[index] = '\n';
          index++;
        }

      }
    }
    i++;
  }
  var[index] = '\0';
  printf("%s",var);
  return 1;
}

//history
int shell_history(){
  FILE* fp = fopen(history_location,"r");
  int c;
  if(fp == NULL){
    printf("error:; file opening.");
  }else{
    while((c = fgetc(fp)) != EOF){
      printf("%c",c);
    }
  }
  fclose(fp);
  return 1;
}

// cd <directory path>
int shell_cd(char **args,char * line)
{

  if (args[1] == NULL || (strcmp(args[1], "~")==0) || (strcmp(args[1], "~/")==0) || (strcmp(args[1], "--")==0)) {
    strcpy(old_cwd,cwd);
    if (chdir(program_directory) != 0) {
      perror("shell");
    }else{
      if(getcwd(cwd,sizeof(cwd)) == NULL){
        perror("shell");
      } 
    }
  } 
  else if((strcmp(args[1],"-")) == 0){
    char * dirn = strdup(old_cwd);
    strcpy(old_cwd,cwd);
    printf("%s\n",dirn);
    if (chdir(dirn) != 0) {
      perror("shell");
    }else{
      if(getcwd(cwd,sizeof(cwd)) == NULL){
        perror("shell");
      }      
    }
  }
  else {
    if (chdir(args[1]) != 0) {
      perror("shell");
    }else{
      strcpy(old_cwd,cwd);
      if(getcwd(cwd,sizeof(cwd)) == NULL){
        perror("shell");
      }      
    }
  }
  return 1;
}

//pwd 
int shell_pwd(char **args,char * line){
  if(getcwd(cwd,sizeof(cwd)) == NULL){
    perror("shell");
  }else{
    printf("%s\n",cwd);
  }
  return 1;
}

//help
int shell_help(char **args,char * line)
{
  printf("*******************************************************Pranshu Kandoi Minishell******************************************\n");
  printf("Type program names and arguments, and hit enter.\n\n");
  printf("Mostly all external commands are handled by the program and the given internal commands are handled.\n");
  printf("The following are built in:\n");
  for (int i = 0; i < shell_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  printf("\nFor using the external commands the syntax is exactly the same as used in the terminal.\n\n");
  printf("Some external commands are:\n");
  printf(" man,which,chsh,whereis,passwd,date,cal,clear,\n sleep,apropos,shutdown,ls,cat,more,less,touch,\n cp,mv,rm,script,find,mkdir,rmdir,chmod,grep,wc\n\n");
  printf("For internal commands also the syntax is mostly the same.\n");
  printf("The following implementation of internal commands are handled\n");
  printf("cd:\n");
  printf(" cd <directory path>,cd ~/,cd,cd -,cd ~,cd ..,cd .\n\n");
  printf("history:\n");
  printf(" history\n\n");
  printf("printenv:\n");
  printf(" printenv,printenv USER or multile printenv USER PATH HOME\n\n");
  printf("setenv:\n");
  printf(" setenv VAR = VALUE\n\n");
  printf("pwd:\n");
  printf(" pwd\n\n");
  printf("echo:\n");
  printf(" All variation of echo are handled like with " ", $ , and simple. Also the space are also handled accordingly in the input\n\n");
  printf("help:\n");
  printf(" help\n\n");
  printf("exit:\n");
  printf(" exit\n\n");
  printf("Onelevel piping is implemented along which all internal and exteranl commands are executed\n");
  printf("one input and output redirection is also handled like <,>,>>,< >>,< >\n\n");

  printf("Use the man command for information on other command.\n");
  return 1;
}

//exit
int shell_exit(char **args,char*line)
{
  if(remove(history_location)!=0){
    perror("shell");
  }
  return 0;
}
int shell_quit(char **args,char*line)
{
  if(remove(history_location)!=0){
    perror("shell");
  }
  return 0;
}
int shell_X(char **args,char*line)
{
  if(remove(history_location)!=0){
    perror("shell");
  }
  return 0;
}
//*****************************************************end of builtin functions**************************************************

//*****************************************************execute functions starts***************************************************
//function to execute external commands
int shell_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("shell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("shell");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

//execute function to execute all commands without redirection or piping
int execute(char **args,char *line)
{
    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    if(args[0][0] == '$'){
      return shell_$(args,line);
    }

    
    for (int i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
        return (*builtin_func[i])(args,line);
        }
    }

    return shell_launch(args);
}

//function to execute input redirection operation <
int execute_input(char **args,char **line,char *args_line)
{
    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }
    pid_t pid,wpid;
    int status;
    pid = fork();
    if(pid ==0){
      int fd;
      int i=1;
      char *input_file = line[0];
      while(line[i] != NULL){
        input_file = line[i];
        i++;
      }
      fd = open(input_file, O_RDONLY,0);
      if (fd < 0)
      {
        perror("shell");
        return 1;
      }
      dup2(fd, STDIN_FILENO);
      close(fd);
      execute(args,args_line);
      exit(0);
    }else if(pid < 0){
      perror("shell");
    }else{

      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));   

    }
    return 1;
}

//handle output redirection operation >
int execute_output(char **args,char **line,char *args_line)
{
    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }
    char * output_file = line[0];
    pid_t pid,wpid;
    int status;
    pid = fork();
    if(pid ==0){
      int fd;

      fd = creat(output_file , 0664) ;
      if (fd < 0)
      {
        perror("shell");
        return 1;
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
      execute(args,args_line);
      exit(0);
    }else if(pid < 0){
      perror("shell");
    }else{

      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));   

    }
    return 1;
}

//append redirection operation >>
int execute_ammend(char **args,char **line,char *args_line)
{
    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }
    char * ammend_file = line[0];
    pid_t pid,wpid;
    int status;
    pid = fork();
    if(pid ==0){
      int fd;

      fd = open(ammend_file,O_WRONLY | O_APPEND | O_CREAT,0664);

      if (fd < 0)
      {
        perror("shell");
        return 1;
      }

      dup2(fd, STDOUT_FILENO);
      close(fd);
      execute(args,args_line);
      exit(0);
    }else if(pid < 0){
      perror("shell");
    }else{

      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));   

    }
    return 1;
}

//both input and append redirection operation < >>
int execute_input_ammend(char **args,char **input_line,char **ammend_line,char *args_line)
{
    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }
    char * ammend_file = ammend_line[0];
    char * input_file = input_line[0];
    pid_t pid,wpid;
    int status;
    pid = fork();
    if(pid ==0){
      int fd0,fd1;

      fd0 = open(ammend_file,O_WRONLY | O_APPEND | O_CREAT,0664);
      fd1 = open(input_file, O_RDONLY,0);

      if (fd1 < 0 || fd0 < 0)
      {
        perror("shell");
        return 1;
      }
      dup2(fd1, STDIN_FILENO);
      dup2(fd0, STDOUT_FILENO);
      close(fd1);
      close(fd0);
      execute(args,args_line);
      exit(0);
    }else if(pid < 0){
      perror("shell");
    }else{

      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));   

    }
    return 1;
}

//handle bothinput and output redirection operation
int execute_input_output(char **args,char **input_line,char **output_line,char *args_line)
{
    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }
    char * output_file = output_line[0];
    char * input_file = input_line[0];
    pid_t pid,wpid;
    int status;
    pid = fork();
    if(pid ==0){
      int fd0,fd1;

      fd0 = creat(output_file , 0664) ;
      fd1 = open(input_file, O_RDONLY,0);

      if (fd1 < 0 || fd0 < 0)
      {
        perror("shell");
        return 1;
      }
      dup2(fd1, STDIN_FILENO);
      dup2(fd0, STDOUT_FILENO);
      close(fd1);
      close(fd0);
      execute(args,args_line);
      exit(0);
    }else if(pid < 0){
      perror("shell");
    }else{

      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));   

    }
    return 1;
}

//function to execute piping
int execute_pipe(char*** args,char** pipe_args){
    int pipefd[2];
    pid_t p1,p2,wpid;
    int status1,status2;

    if (pipe(pipefd) < 0) {
      fprintf(stderr, "shell: pipe not initialised\n");
      exit(EXIT_FAILURE);
    }

    p1 = fork();
    if(p1 == 0){
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execute(args[0],pipe_args[0]);
        exit(0);

    }else if(p1< 0){
        // Error forking
        perror("shell");
    }else{

        p2 = fork();

        if(p2 == 0){

            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            execute(args[1],pipe_args[1]);
            exit(0);

        }else if(p2 < 0){
          perror("shell");
        }else{
          close(pipefd[0]);
          close(pipefd[1]);
          waitpid(p1,&status1,0);
          waitpid(p2,&status2,0);  
        }
        return 1;
    }
}
//********************************************execute functions over*****************************************

//*******************************************parser begins**********************************************
//parsing all spaces in input string
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
char **split_line(char *line)
{
    char *cp_line = strdup(line);
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(cp_line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

//parser for the piping |
#define TOK_DELIM_PIPE "|"
char **pipe_line(char *line)
{
    char *cp_line = strdup(line);
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(cp_line, TOK_DELIM_PIPE);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM_PIPE);
    }
    tokens[position] = NULL;
    return tokens;
}

//parser for input redirection
#define TOK_DELIM_IN "<"
char **input_line_check(char *line)
{
    char *cp_line = strdup(line);
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(cp_line, TOK_DELIM_IN);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM_IN);
    }
    tokens[position] = NULL;
    return tokens;
}

//parser for output redirection
#define TOK_DELIM_OU ">"
char **output_line_check(char *line)
{
    char *cp_line = strdup(line);
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(cp_line, TOK_DELIM_OU);
    while (token != NULL) {

        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM_OU);
    }
    tokens[position] = NULL;
    return tokens;
}

//parser for append operation
#define TOK_DELIM_AMMEND ">>"
int ammend_line_check(char *line)
{
    char *cp_line = strdup(line);

    for(int i=0,j=0;i<strlen(cp_line)-1;i++){
      if(cp_line[i] == '>' && cp_line[i+1] == '>'){
        return 1;
      }
    }
    return 0;
}

//function for parsing each individual pipe 
#define BUFFER_SIZE 64
char*** split_pipe(char **pipe_args){
  char ***arg = (char ***)malloc(BUFFER_SIZE*sizeof(char**)) ;
  int i=0;
  while(pipe_args[i] != NULL){
    arg[i] = split_line(pipe_args[i]);
    i++;
  }
  arg[i] = NULL;
  return arg;
}

//function to read line by line from the terminal
char *read_line(void)
{
    char *line = NULL;
    ssize_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1){
        if (feof(stdin)) {
            exit(EXIT_SUCCESS); 
        } else  {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

//function to update history 
void update_history(char *line){
    FILE* fp = fopen(history_location,"a+");
    fseek(fp,0L,SEEK_SET);
    int c;
    long nl=0;
    while((c=fgetc(fp)) != EOF){
      if(c == '\n'){
        ++nl;
      }
    }
    fseek(fp,0L,SEEK_END);    
    fprintf(fp," %ld %s",nl+1,line);
    fclose(fp);
}

//the first line after each command execution
void base_line(){
  char relative[256];
  int i=0;
  while(program_directory[i] != '\0'){
    i++;
  }
  for(int j=i;cwd[j] != '\0';j++){
    relative[j-i] = cwd[j];
  }

  if(strcmp(cwd,program_directory) == 0){
    printf("minishell@minishell:~%s ",getenv("PS1"));
  }else if(strcmp(cwd,program_directory) < 0){
    printf("minishell@minishell:%s%s ",cwd,getenv("PS1"));    
  }else{
    printf("minishell@minishell:~%s%s ",relative,getenv("PS1"));    
  }
}

//handling signal for ctrl + C
// void sigintHandler(int sig_num)
// {
//     signal(SIGINT, sigintHandler);
//     fflush(stdout);
// }

// my minishell
void minishell(){
    char *line;
    char **args;
    char ***pipe_args;
    char **piped_line;
    char **input_args;
    char **input_line;
    char **input_files;
    char **output_line;
    char **output_args;
    char **output_files;
    int ammend_line;
    char **ammend_args;
    char **ammend_files;
    int status = 1;


    // signal(SIGINT, sigintHandler);
    do {
        base_line();
        line = read_line();
        update_history(line);
        piped_line = pipe_line(line);
        input_line = input_line_check(line);
        output_line = output_line_check(line);
        ammend_line = ammend_line_check(line);
        args = split_line(line);
        pipe_args = split_pipe(piped_line);


        if(pipe_args[1] != NULL){
          status = execute_pipe(pipe_args,piped_line);
        }
        else if(ammend_line == 1){
          if(input_line[1] != NULL){
            input_line = input_line_check(output_line[0]);
            input_args = split_line(input_line[0]);
            input_files = split_line(input_line[1]);
            ammend_files = split_line(output_line[1]);
            status = execute_input_ammend(input_args,input_files,ammend_files,input_line[0]);
          }else{
            ammend_args = split_line(output_line[0]);
            ammend_files = split_line(output_line[1]);
            status = execute_ammend(ammend_args,ammend_files,output_line[0]);
          }
        }
        else if(output_line[1] != NULL){
          if(input_line[1] != NULL){
            input_line = input_line_check(output_line[0]);
            input_args = split_line(input_line[0]);
            input_files = split_line(input_line[1]);
            output_files = split_line(output_line[1]);
            status = execute_input_output(input_args,input_files,output_files,input_line[0]);
          }else{
            output_args = split_line(output_line[0]);
            output_files = split_line(output_line[1]);
            status = execute_output(output_args,output_files,output_line[0]);
          }
        }
        else if(input_line[1] != NULL){
          input_args = split_line(input_line[0]);
          input_files = split_line(input_line[1]);
          status = execute_input(input_args,input_files,input_line[0]);
        }
        else
        {
          status = execute(args,line);
        }

        free(piped_line);
        free(line);
        free(args);
    } while (status);
}

//main program
int main(int argc, char* argv[])
{
    //getting the program directory
    if(getcwd(program_directory,sizeof(program_directory)) == NULL){
      perror("shell");
    }
    //also saving program directory in current working directory
    if(getcwd(cwd,sizeof(cwd)) == NULL){
      perror("shell");
    }
    //making history file
    FILE *fp = fopen("history.txt","a");
    fclose(fp);
    //history location making
    strcpy(history_location,program_directory);
    strcat(history_location,"/history.txt");
    //setting the PS1 to $ which can be changed
    setenv("PS1","$",1);

    minishell();
    return EXIT_FAILURE;
}