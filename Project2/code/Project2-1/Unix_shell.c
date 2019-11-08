#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_LINE 80
void *get_the_input(char *input);

/* judge whether the command has ended and record the command */
void *get_the_input(char *input){
    int i, enter, first_non_space, last_non_space;
    char *output;
    first_non_space = 0;
    last_non_space = 0;
    output = malloc(sizeof(char) * MAX_LINE);
    for(i=0;i<MAX_LINE;i++){
        if(input[i] == '\n'){
            enter = i;
            break;
        }
    }
    if(enter == -1){
        printf("Error! Can not find enter!\n");
        exit(1);
    }
    for(i=0;i<MAX_LINE && input[i] == ' ';i++)  ;
    first_non_space = i;
    for(i=enter-1;i>=0 && input[i] == ' ';i--)   ;
    last_non_space = i;
    strcpy(output, input + first_non_space);
    output[last_non_space - first_non_space+1] = '\0';

    input = output;
}

int main(){
    char *args[MAX_LINE/2 + 1];
    /* store the history */

    char *input, *find, *redirect_path;
    char *last_input;
    int i, last,  num_args, res, fd;
    /* initialization */
    /* flag for the while loop */
    int should_run = 1;
    int wait_flag = 1;
    int in_redirect_flag = 0;
    int out_redirect_flag = 0;
    int pipe_flag = 0;
    int pipe_idx = 0;
    pid_t pid;

    for(i=0;i<MAX_LINE/2+1;i++){
        args[i] = malloc(sizeof(char) * MAX_LINE);
    }
    input = malloc(sizeof(char) * MAX_LINE);
    last_input = malloc(sizeof(char) * MAX_LINE);
    redirect_path = malloc(sizeof(char) * MAX_LINE);
    last_input[0] = '\0';

    while(should_run){
        printf("osh>");
        fflush(stdout);

        /* we must reset the input to avoid the effect of the last input*/
        for(i=0;i<MAX_LINE/2+1;i++) memset(args[i], 0, sizeof(char) * MAX_LINE);

        /* parse the input */
        /* get rid of the '\n' */
        fgets(input, MAX_LINE, stdin);
        input = get_the_input(input);

        if(strcmp(input, "exit") == 0 ){
            printf("Bye.\n");
            should_run = 0;
            break;
        }else if(strcmp(input, "!!") == 0 && last_input[0] != '\0'){
            strcpy(input, last_input);
        }else if(strcmp(input, "!!") == 0 && last_input[0] == '\0'){
            printf("No recent command!\n");
            continue;
        }
        strcpy(last_input, input);

        i = last = num_args = 0;
        in_redirect_flag = out_redirect_flag = 0;
        pipe_idx = pipe_flag = 0;
        while(i++<MAX_LINE-1){
            if(input[i] == '\0'){
                /* if the second last is & *
                    * set the flag and exclude the & character */
                if(input[i-1] == '&')   wait_flag = 0;
                else    strncpy(args[num_args++], input+last, i-last);
                
                if(strcmp(args[num_args-1], "<") == 0){
                    in_redirect_flag = 1;
                    /* get the filepath */
                    for(;input[i] == ' ' && i<MAX_LINE;i++) ;
                    strcpy(redirect_path, input+i);
                }
                break;
            }
            if(input[i] == ' '){
                strncpy(args[num_args++], input+last, i-last);
                if(strcmp(args[num_args-1], ">") == 0){
                    printf("capture the output redirection...\n");
                    out_redirect_flag = 1;
                    /* get the redirect file */
                    for(;input[i] == ' ' && i<MAX_LINE;i++) ;
                    strcpy(redirect_path, input+i);
                }else if(strcmp(args[num_args-1], "<") == 0){
                    printf("capture the input redirectoion...\n");
                    in_redirect_flag = 1;
                    /* get the redirect file */
                    for(;input[i] == ' ' && i<MAX_LINE;i++) ;
                    strcpy(redirect_path, input+i);
                }else if(strcmp(args[num_args-1], "|") == 0){
                    /* add support for the pipeline */
                    pipe_flag = 1;
                    /* use an index to record the place of the pipe char */
                    pipe_idx = num_args-1;
                }
                for(;input[i] == ' ' && i<MAX_LINE;i++) ;
                last = i;
            } 
        }
        
        if(!pipe_flag){
            /* indicate we don't have a | char */
            pipe_idx = num_args;
        }

        pid = fork();
        if(pid < 0){
            perror("fork error!\n");
        }else if(pid == 0){
            /* child process */
            char *valid_args[pipe_idx];
            char *pipe_args[num_args - pipe_idx + 1];
            
            for(i=0;i<num_args;i++){
                /* exclude the redirection char */
                if(strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0) break;
                if(strcmp(args[i], "|") == 0 )  continue;
                /* store the main args */
                if(i < pipe_idx)   valid_args[i] = args[i];
                /* store the pipe args (note that we skip the | char)*/
                else if(i > pipe_idx)   pipe_args[i-pipe_idx-1] = args[i];
            }
            
            /* add NULL to the end */
            valid_args[pipe_idx] = NULL;
            pipe_args[num_args - pipe_idx - 1] = NULL;
            
            /* handle the redirection */
            if(out_redirect_flag){
                fd = open(redirect_path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
                if(fd < 0)  {perror("open error!"); exit(1);}
                /* dup2(oldfd, newfd) (close the stdout) */
                if(dup2(fd, 1) < 0) {perror("dup2 error!"); exit(1);}
                close(fd);
            }else if(in_redirect_flag){
                fd = open(redirect_path, O_RDONLY, 0644);
                if(fd < 0)  {perror("open error!"); exit(1);}
                /* dup2(oldfd, newfd) (close the stdin) */
                if(dup2(fd, 0) < 0) {perror("dup2 error!");exit(1);}
                close(fd);
            }
            
            if(!pipe_flag)  res = execvp(valid_args[0], valid_args);
            else{
                /* for a cmd1 | cmd2 simple pipeline problem */
                /* we create a child for the child and use IPC between them to finish the pipeline */
                int p[2], ppid;

                pipe(p);
                ppid = fork();
                if(ppid < 0)    {perror("ppid create error!");exit(1);}
                else if(ppid == 0){
                    /* for the child of the child process, set it as the executor of the second commmand */
                    close(0);
                    close(p[1]);
                    if(dup(p[0]) < 0)   {perror("dup error!");exit(1);}
                    res = execvp(pipe_args[0], pipe_args);
                    if(res < 0) { perror("cmd2 execution error!"); exit(1); }
                }else{
                    /* for the parent process, it will execute the first command */
                    /* close stdout */
                    close(1);
                    /* close one port */
                    close(p[0]);
                    if(dup(p[1]) < 0)   {perror("dup error!");exit(1);}
                    res = execvp(valid_args[0], valid_args);
                    if(res < 0) { perror("cmd1 execution error!"); exit(1); }
                }
            }
            if(res < 0){
                perror("Execution error!");
            }
        }else{
            /* parent process */
            /* if wait: wait the child process
             * else use signal to avoid zombie */
            if(wait_flag) wait(NULL);
            else signal(SIGCHLD, SIG_IGN);
        }
    }
    return 0;
}


