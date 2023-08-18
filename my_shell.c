#include  <stdio.h>
#include  <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_BG_PROCESSES 64

/* Splits the string by space and returns the array of tokens
*
*/
int bg_flag;
int Background[64];
int number_of_background_processes=-1;
int fg_pid=-2;
int repead_child=-1,ctrl_c=0;
int no_of_tokens=0;



char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0,tokenNo=0;
  bg_flag=0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } 
    else {
      token[tokenIndex++] = readChar;
    }
  }
  if(tokenNo>0)
   if((!strcmp(tokens[tokenNo-1],"&"))){
   	bg_flag=1;
   	//printf("this is background process\n");
   	tokens[tokenNo-1]=NULL;
   	
   }
  
  if(tokenNo>0){
  free(token);
  tokens[tokenNo] = NULL ;
  //printf("%d\n",tokenNo);
  no_of_tokens=tokenNo;
  
  }
  return tokens;
}

void sighandle(int signumber){
    
  
    setpgid(fg_pid,fg_pid);
    //printf("%d",fg_pid);
    //repead_child=fg_pid;
    kill(fg_pid,SIGTERM);
    //ctrl_c=1;
    
    
}

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;
	
	signal(SIGINT,sighandle);
	for(int x=0;x<64;x++){
	Background[x]=0;//Intializing background 
	}

	char path[MAX_TOKEN_SIZE]="$";
	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		//path="$";
		printf("%s $",getcwd(path,64));
		scanf("%[^\n]", line);
		getchar();

		//printf("Command entered: %s\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		
		if(no_of_tokens>0 &&(strcmp(tokens[0],"exit")==0)){
		    for(int i=0;i<65;i++){
		       if(Background[i]!=0){
		       	    kill(Background[i],SIGTERM);//SIGTERM will inform the process,so process will release all its resources
		       	}
		    }
		    
		    for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
			}
			free(tokens);
			
		     kill(getpid(),SIGTERM);
		    //return 0;    
		}
   
       //do whatever you want with the commands, here we just print them
		
		//for(i=0;tokens[i]!=NULL;i++){
			
			if(no_of_tokens>0 &&(!strcmp(tokens[0],"cd"))){
			
			    if(no_of_tokens>2){
			       printf("bash: cd: too many arguments\n");
			    }
			    else{
			    int status=chdir(tokens[1]);
			    
			    
			    if(status==-1){
				printf("No such file or directory:(\n");
				}
			    else{
			    	//strcat(path,"/");
			  	//strcat(path,tokens[i]);
			  	//printf("%s\n",path);
			    }
			    }
			}
			
			else if(no_of_tokens>0 && (tokens[0]!=0)){
				int newchild=fork();
				if(bg_flag==1)
					setpgid(newchild,newchild);
				if(newchild<0){
				    printf("fork is failed:(/n");	
				}
			else if(newchild==0){
			            if(no_of_tokens>0)
				    execvp(tokens[0],tokens);
				    printf("Command \'%s\' not found:(\n",tokens[0]);
				}
				else{
				if(bg_flag==0)
				   fg_pid=newchild;
				//printf("bg value %d \n",bg_flag);
					if(bg_flag==1){
						for(int i=0;i<65;i++){
					            if(Background[i]==0){
					                 Background[i]=newchild;
					                 break;
					            }	
					        }
						//Background[++number_of_background_processes]=getpid();
						continue;
						}
				        int *status=NULL;
				        
				        
/*					if(ctrl_c==1)
					  {
					  	repead_child=fg_pid;
					  	ctrl_c=0;
					  	kill(fg_pid,SIGTERM);
					  	
					  }*/
					  
					
					repead_child=waitpid(-1,status,0);//-1 -wait for a child process
					while(newchild!=repead_child && repead_child!=-1){
					        for(int i=0;i<65;i++){
					            if(Background[i]==repead_child){
					                 Background[i]=0;
					                 break;
					            }	
					        }
						printf("background process reaped with pid : %d\n",repead_child);
						repead_child=waitpid(-1,status,0);
						}
					 //setpgid(newchild,newchild);	
					 //kill(fg_pid,SIGTERM);	
					
				}
			}
			
		//}
       
		// Freeing the allocated memory	
		if(no_of_tokens>0)
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
		

	}
	return 0;
}
