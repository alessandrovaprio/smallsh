#include "smallsh.h"
#include <sys/types.h>
#include <string.h>

char *prompt = "";


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
  
    pid_t pid;
    int exitstat,ret;
    
    pid = fork();
    if (pid == (pid_t) -1) {
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
    if (!where == BACKGROUND)
      ret = wait(&exitstat);


    if (ret == -1) perror("wait");
  
}

char* getUserNameAndWorkingDir()
{
  //char *buf;
  char stringToRet[100] = "%";
  char *pStringtoRet;
  // buf = (char *)malloc(20 * sizeof(char));
  // buf = getlogin();
  
  //char *percentageString="%";

  //strcat(stringToRet, buf);
  strcat(stringToRet, getenv("USER"));
  strcat(stringToRet, ":");
  strcat(stringToRet, getenv("HOME"));
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
