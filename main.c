#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

void sig_handler(int sign);

int main()
{
     //user input command
     char entry[100];

     //string split requirements
     char delim[] = " ";
     int i;
     int lenBefore,lenAfter;

     //first will be command, the rest arguments
     char *splitted[15];

     //to know if background or not
     int j;
     int ampersand;

     //for execution
     pid_t pid;
     int child_pid;
     int status;

     //log
     FILE * fptr;


    while(1)
    {
      //read user input
      reread:
      printf("\nShel>");
      gets(entry);

      lenBefore = strlen(entry);
      if(lenBefore==0)
      goto reread;

      ampersand=0;

      for(j=0;j<lenBefore;j++)
      {
        if(entry[j]=='&'){
        ampersand=1;
        entry[j]=NULL;
        }
      }

      //split command and arguments
      splitted[0]=strtok(entry,delim);

      // initialize i and get lenAfter for if exist arguments
      i=1;
      lenAfter = strlen(entry);

      //check if command without arguments
      if(lenBefore==lenAfter)
      {
        splitted[1]=NULL;
        goto oneArg;
      }

      //if input has arguments
      while((splitted[i]= strtok(NULL,delim)) != NULL)
       {
            i++;
       }

       splitted[i]=NULL;

      //get command
      oneArg:

      if(strcmp(splitted[0],"exit")==0)
      exit(0);

      else if(strcmp(splitted[0],"cd")==0)
      {
        if(chdir(splitted[1])!=0)
        printf("Error occured->%s\n",strerror(errno));
      }

      else
      {
        pid=fork();

        if(pid < 0)
        printf("Error Forking\n");

        else if(pid==0) //child execute command
        {
            if(execvp(splitted[0],splitted)<0)
            printf("Error in execution!\n");
        }

        else
        {
            if(ampersand==0)
            {
             child_pid=wait(&status);
             fptr = fopen("log.txt","a+");
             if(fptr!=NULL)
             {
              fprintf(fptr,"Child %d Terminated\n",child_pid);
              fclose(fptr);
             }
            }

            else
            signal(SIGCHLD,sig_handler);

        }

      }

    }

    return 0;
}

void sig_handler(int sig) {

  int status ;
  pid_t childbk;

  FILE *fptr= fopen("log.txt","a+");

  //wait for any process
  if ((childbk = waitpid(-1, &status, WNOHANG)) > 0)
  {
    if(fptr!=NULL)
    {
        fprintf(fptr,"%d terminated\n",childbk);
        fclose(fptr);
    }
  }

}
