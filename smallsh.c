#include "smallsh.h"
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
char *strremove(char *str, char *sub);
char *convertIntToString(int n);

  void insertPid(int pid);
  char *prompt = "";
  pid_t pid;
  struct sigaction sa;
  int exitstat, ret;

  void sig_handler(int signo)
  {

    if (signo == SIGINT)
    {
      //kill(pid, SIGTERM);
      //signal(SIGINT, sig_handler);
      //printf("received SIGINT\n");
      fflush(stdout);
      //kill(pid, SIGINT);
      //return EXIT_FAILURE;
    }
  }
  void removePid(int pid)
  {
    //controllo che la variabile contenga pid+,
    char *str = convertIntToString(pid);
    char *strWithComma = convertIntToString(pid);
    strcat(strWithComma, ",");
    if (strstr(getenv("BPID"), strWithComma) != NULL)
    {
      char *strRemoved = strremove(getenv("BPID"), strWithComma);
      setenv("BPID", strRemoved, 1);
    }
    //controllo che la variabile contenga pid (ultimo elemento o primo senza ulteriori)
    if (strstr(getenv("BPID"), str) != NULL)
    {
      setenv("BPID", strremove(getenv("BPID"), str), 1);
    }
    return;
  }
  void insertPid(int pid)
  {
    printf("insert pid %d", pid);
    int size = 100;
    char *env = getenv("BPID");
    if (env != NULL)
    {
      if (strlen(env > 0))
      {
        size = strlen(getenv("BPID"));
      }
    }
    printf("size is %d", size);
    char myenv[size];
    if (getenv("BPID") == NULL)
    {
      myenv[size] = "";
    }
    else
    {
      myenv[size] = getenv("BPID");
    }

    if (getNumberOfComma(myenv) > 0 || strlen(getenv("BPID")) > 0)
    {
      myenv[size] = strcat(myenv, ",");
      myenv[size] = strcat(myenv, convertIntToString(pid));
      setenv("BPID", myenv, 1);
    }
    else
    {
      myenv[size] = strcat(myenv, convertIntToString(pid));
      setenv("BPID", myenv, 1);
    }
  }

  int getNumberOfComma(char str[])
  {
    int ret = 0;
    for (int i = 0; i < strlen(str); i++)
    {
      if (str[i] == ',')
        ret++;
    }
    return ret;
  }
  char *strremove(char *str, char *sub)
  {
    size_t len = strlen(sub);
    if (len > 0)
    {
      char *p = str;
      while ((p = strstr(p, sub)) != NULL)
      {
        memmove(p, p + len, strlen(p + len) + 1);
      }
    }
    return str;
  }

  char *convertIntToString(int n)
  {
    char str[12];
    sprintf(str, "%d", n);
    return &str;
  }

  int procline(void) /* tratta una riga di input */
  {
    char *arg[MAXARG + 1]; /* array di puntatori per runcommand */
    int toktype;           /* tipo del simbolo nel comando */
    int narg;              /* numero di argomenti considerati finora */
    int type;              /* FOREGROUND o BACKGROUND */

    narg = 0;

    while (1)
    { /* ciclo da cui si esce con il return */

      /* esegue un'azione a seconda del tipo di simbolo */

      /* mette un simbolo in arg[narg] */

      switch (toktype = gettok(&arg[narg]))
      {
	
      /* se argomento: passa al prossimo simbolo */
      case ARG:
		
        if (narg < MAXARG)
	  narg++;
	break;

      /* se fine riga o ';' o '&' esegue il comando ora contenuto in arg,
	 mettendo NULL per segnalare la fine degli argomenti: serve a execvp */
      case EOL:
      case SEMICOLON:
      case AMPERSAND:
        type = (toktype == AMPERSAND) ? BACKGROUND : FOREGROUND;
      
        if (narg != 0) {
	        arg[narg] = NULL;
    
          runcommand(arg,type);
        }
      
	/* se fine riga, procline e' finita */

        if (toktype == EOL) return 1;
      
	/* altrimenti (caso del comando terminato da ';' o '&') 
           bisogna ricominciare a riempire arg dall'indice 0 */

        narg = 0;
        break;
    }
    
  }
}

void runcommand(char **cline,int where)	/* esegue un comando */
{

  fflush(stdout);
  pid = fork();
  if (pid == (pid_t)-1)
  {
    perror("smallsh: fork fallita");
    return;
    }

    if (pid == (pid_t) 0) { 	/* processo figlio */

      /* esegue il comando il cui nome e' il primo elemento di cline,
        passando cline come vettore di argomenti */
      if (where == BACKGROUND)
      {
        pid_t p;
        int exitstat, ret;

        fflush(stdout);

        p = fork();
        if (p == -1)
        {
          perror("smallsh: fork fallita");
          return;
        }

        if (p == 0)
        { /* processo figlio */
          //setenv("BPID",convertIntToString(p),1);
          //insertPid(p);
          execvp(*cline, cline);
          exit(EXIT_SUCCESS);
        }else{
          int status;
          int endID = waitpid(p, &status, WNOHANG | WUNTRACED);
          while(endID ==0){
            //printf("Child still running");
            endID = waitpid(p, &status, WNOHANG | WUNTRACED);
            if (endID == -1)
            { /* error calling waitpid       */
              perror("waitpid error");
              exit(EXIT_FAILURE);
            }
            else if (endID == p)
            { /* child ended                 */
              if (WIFEXITED(status)){
                printf("The command %s is terminated \n", *cline);
              }
              else if (WIFSIGNALED(status))
                printf("Child ended because of an uncaught signal.n \n");
              else if (WIFSTOPPED(status))
                printf("Child process has stopped.n \n");
              else
                printf("Child");
              //removePid()
              fflush(stdout);
              exit(EXIT_SUCCESS);
            }
          }
          
          
          
        }
          //perror(*cline);
       
      }else{
        execvp(*cline,cline);
        perror(*cline);
        exit(1);
      }
    }

    /* processo padre: avendo messo exec e exit non serve "else" */
    
    /* la seguente istruzione non tiene conto della possibilita'
      di comandi in background  (where == BACKGROUND) */
    if (!where == BACKGROUND){
      struct sigaction signalAction;
      signalAction.sa_handler = sig_handler;
      sigemptyset(&signalAction.sa_mask);
      signalAction.sa_flags = 0; // SA_RESTART;
      sigaction(SIGINT, &signalAction, NULL);
      //  if(signal(SIGINT, sig_handler)== SIGINT){
      //     printf("my pid %d \n",pid);
      //     //kill(pid,SIGINT);
      //   }else{
      //     printf("not  %d \n", signal(SIGINT, sig_handler));
      //   }
      ret = wait(&exitstat);

    }
      


    if (ret == -1) perror("wait");
  
}

char* getUserNameAndWorkingDir()
{
  char stringToRet[100] = "%";
  char *pStringtoRet;
  
  strcat(stringToRet, getenv("USER"));
  strcat(stringToRet, ":");
  strcat(stringToRet, getenv("HOME"));
  strcat(stringToRet, ":");
  pStringtoRet = &stringToRet;
  return pStringtoRet;
}


int main()
{
  setenv("BPID","",1);
  prompt = getUserNameAndWorkingDir();
  while (userin(prompt) != EOF){
    //to fix prompt after first init
    fflush(stdout);
    prompt = getUserNameAndWorkingDir();
    procline();
  }
  
    
}
