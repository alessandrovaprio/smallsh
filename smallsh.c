#include "smallsh.h"
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

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
char * removePid(int pid){
  //controllo che la variabile contenga pid+,
  if (strstr(getenv("BPID"), srtcat(convertIntToString(pid),",")) != NULL)
  {
    return strremove(getenv("BPID"), srtcat(convertIntToString(pid), ","));
  }
  //controllo che la variabile contenga pid (ultimo elemento o primo senza ulteriori)
  if (strstr(getenv("BPID"), convertIntToString(pid)) != NULL){
    return strremove(getenv("BPID"), convertIntToString(pid));
  }
  return;
}
char * insertPid(int pid)
{
  int size = strlen(getenv("BPID"));
  if(size==0)
    size=100;
  char myenv[size] = getenv("BPID");
  if (getNumberOfChar(myenv,",")>0){
    myenv = strcat(myenv, ",");
        setenv("BPID", )
  }
  //char myenv[100] = getenv("BPID");
}

int getNumberOfChar(char * str,char c){
  int ret =0;
  for (int i = 0; i<strlen(str);i++){
    if (str[i] == c)
      ret++;
  }
  return ret;
}
char *strremove(char *str, const char *sub)
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

char * convertIntToString(int n){
  char str[12];
  sprintf(str, "%d", n);
  return &str;
}

int procline(void) /* tratta una riga di input */
{
  char *arg[MAXARG+1];	/* array di puntatori per runcommand */
  int toktype;  	/* tipo del simbolo nel comando */
  int narg;		/* numero di argomenti considerati finora */
  int type;		/* FOREGROUND o BACKGROUND */
	
  narg=0;

  while (1) {	/* ciclo da cui si esce con il return */
	
  /* esegue un'azione a seconda del tipo di simbolo */
	
    /* mette un simbolo in arg[narg] */

    switch (toktype = gettok(&arg[narg])) {
	
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

        p = fork();
        if (p == -1)
        {
          perror("smallsh: fork fallita");
          return;
        }

        if (p == 0)
        { /* processo figlio */
          setenv("BPID",convertIntToString(p),1);
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
                printf("The command %s is terminated ", *cline);
              }
              else if (WIFSIGNALED(status))
                printf("Child ended because of an uncaught signal.n");
              else if (WIFSTOPPED(status))
                printf("Child process has stopped.n");
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
  
      prompt = getUserNameAndWorkingDir();
  while (userin(prompt) != EOF){
    //to fix prompt after first init
    prompt = getUserNameAndWorkingDir();
    procline();
  }
  
    
}
