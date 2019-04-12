//angel solis 5000220181 CS370
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

#include <sys/stat.h>
#include <fcntl.h>

void execom(char**argv,pid_t pid,int status);
void pipe_call(char**argv,pid_t pid,int fd[]);
void merge(char**argv,pid_t pid);
int main()
{
	//char *cmdtorun =(char*)calloc(80,sizeof(char));
	char *cmdtorun = malloc(80*sizeof(char));
	char *dirbuff=malloc(80*sizeof(char));
	int minibuff,i=0,fd[2],maxup=0;
	char *token = (char*)calloc(80,sizeof(char));
	char *argv[10];
	int status,counter = 0,index=0,tempindex=0;
	char *history[10];
	char *pipe_c ="|"; // pipe identification
	int pipe_s = 0; //status of pipe command
	pid_t pid;


	//termios setup 
	struct termios origConfig;
	tcgetattr (0,&origConfig); //copy old configuration
	
	struct termios newConfig = origConfig;
	
	newConfig.c_lflag &= ~(ICANON|ECHO); //remove echo
	newConfig.c_cc[VMIN] = 10;
	newConfig.c_cc[VTIME] = 1;
	tcsetattr (0,TCSANOW,&newConfig); //sets new configuration
	
	//history allocation
	for(index=0;index<10;index++)
	  {
		history[index%10]= malloc(80*sizeof(char));
		argv[index]= malloc(80*sizeof(char));
		argv[index][0]='\0';
	  }


	while (1)
		{
				tempindex=index;
				maxup=0;
				getcwd(dirbuff,80);
				printf("%s->",dirbuff);
				counter = 0;
				pipe_s=0;
				
		
			while((minibuff= getchar())!='\n')
			{
			
				if (minibuff == 127||minibuff == 8) //checks for delete or backspace
				{
					if(counter>0)	//do not delete directory output
					  {
						printf("\b \b");
						minibuff=21;	//clears buffer 
						counter--;		//decrease the counter to simulate delete in string
					  }
			        }
				else
				{
					cmdtorun[counter] = minibuff;	//save the character
					
					
				
					if(minibuff==27)	//check for esc key
					{
						minibuff=getchar(); //get next to check for arrows 
						if(minibuff==91)	//second arrow code
						{
							minibuff=getchar();	//get last arrow code
						
							if(minibuff ==68 ||minibuff ==67 ) //left and right
							{
								cmdtorun[counter]='\0';	//reset the first command
							}
							else if (minibuff==65) //up 
							{
								
								//if(tempindex==10)
								//	tempindex=index;
								if(index>=20) //check to make sure there are 10 previous commands
								{
									for(;counter>0;counter--) //clear cmdtorun
									{
										cmdtorun[counter]='\0';
										printf("\b \b");
									}
									
									if(tempindex%10 != index%10 || maxup !=1)
										tempindex--;
									if(tempindex%10 == (index-1)%10)
										maxup=1;
									for(i=0;history[tempindex%10][i]!='\0';i++) //print and set the characters
									{
										putchar(history[tempindex%10][i]);
										cmdtorun[counter] = history[tempindex%10][i];
										counter++;
									}
								
								}
								else if(index<20&&index!=10) //if less then 10 commands but at least 1
								{
									for(;counter>0;counter--) //clear cmdtorun
									{
										cmdtorun[counter]='\0';
										printf("\b \b");
									}
									if(tempindex%10!=0) //does not allow going in around
										tempindex--;
									for(i=0;history[(tempindex%10)%(tempindex-9)][i]!='\0';i++)//cycle through given commands
									{
										putchar(history[tempindex%10][i]);
										cmdtorun[counter] = history[tempindex%10][i];
										counter++;
									}
								}
								else
								{
									
								}
							}
							else if(minibuff==66)//down 
							{
							if(tempindex%10 == index%10 && maxup == 0) // press down without an up do nothing
								continue;
									
							else if(tempindex%10 == (index-1)%10 ) //have previously presses up and now back at the new command 
							{
								for(;counter>0;counter--) //clear cmdtorun
									{
										cmdtorun[counter]='\0';
										printf("\b \b");
										
									}
									tempindex++;
									maxup=0;
							}
							else
							{
								
							
								if(index>=20) //check to make sure there are 10 previous commands
								{
									for(;counter>0;counter--) //clear cmdtorun
									{
										cmdtorun[counter]='\0';
										printf("\b \b");
									}
									if(tempindex%10!= index%10 || maxup==1) // makesure that we are not all the way around
									tempindex++;
									for(i=0;history[tempindex%10][i]!='\0';i++) //print and set the characters
									{
										putchar(history[tempindex%10][i]);
										cmdtorun[counter] = history[tempindex%10][i];
										counter++;
									}
								
								}
								else if(index<20&&index!=10) //if less then 10 commands but at least 1
								{
									for(;counter>0;counter--) //clear cmdtorun
									{
										cmdtorun[counter]='\0';
										printf("\b \b");
									}

									if(tempindex>=index)  // stops the tempindex from going beyond amount of commands
										tempindex= tempindex -index%10;	//set it down by number of commands
									else if(tempindex == index-1)	//case where above happend and now commands need to cycle
										tempindex=tempindex-index%10+1;
									else	//general case
										tempindex++;
									for(i=0;history[(tempindex%10)%(tempindex-9)][i]!='\0';i++)//cycle through given commands
									{
										putchar(history[tempindex%10][i]);
										cmdtorun[counter] = history[tempindex%10][i];
										counter++;
									}
								}
								else
								{
								  continue;
								}
							}
							}
							else	//should the last input not be an arrow code
							{
								counter++;				
								putchar(minibuff);
							}
						}	
						else    // incase the user just pressed esc act normally
						{
							counter++;				
							putchar(minibuff);
						}
					}
					else	//default case
					{
					
						counter++;				
						putchar(minibuff);
					}
				  }
			}
			cmdtorun[counter]='\0';	//add the null
			minibuff=2;	//reset buffer
			printf("\n");
			strcpy(history[index%10],cmdtorun);
			if(strpbrk (cmdtorun, pipe_c )!=NULL)
			  pipe_s=1;
			history[index%10][counter]='\0';	//add the null
			index++;
			
			argv[0] = NULL; //reset argument vectors first slot
			counter = 0;	//reset counter

			token = strtok(cmdtorun," ");
			while (token != NULL) //tokenize string
			{
				argv[counter] = token;
				counter = counter + 1;
				token = strtok(NULL," ");
			}
			
				argv[counter] =token;	//set last null
				argv[counter+1] =token; //just incase
			
			if(argv[0] == NULL) //only new line entered do nothing
			  continue;
				
			else if (strcmp("exit",argv[0]) == 0) // finish freeing the alocated mem
				{
				  printf("Are you sure you want to exit?(y) ");
				  if((minibuff=getchar())==121)
				    {
				      putchar(minibuff);
				      printf("\n");
				     
				      for(index=0;index<10;index++)
					{
					free(history[index%10]);
					argv[index]= malloc(80*sizeof(char));
					}
					free(cmdtorun);
					free(token);
					tcsetattr(0,TCSANOW,&origConfig); //return to original mode
					exit(0);
				    }
				 else if(minibuff==27)
				 {
					if(minibuff=getchar()==91)
					{
						minibuff=getchar();
						 printf("\n");
					}
				}					
				else 
				    {
				      putchar(minibuff);
				      printf("\n");
				    }
				}
				
				
				
			else if (strcmp("cd",argv[0]) == 0) //impliments the directory change
				{
					status = chdir(argv[1]);
					if (status == -1)
						perror(NULL);
				}
			else if (pipe_s==1 ) //pipe not functioning need to search entire array of char* 
			  {
			      pipe_call(argv,pid,fd);
			    
			  }

			else if(strcmp("merge",argv[0]) == 0) //not working yet
			  {

				merge(argv,pid);
			  }
		
			
		
			else		//default case
			  {
			    
			    execom(argv,pid,status);
			  }
			
		}

	
}
void execom(char*argv[],pid_t pid,int status)
{
  pid=fork(); 
  if (pid ==0) // check for child process
    {
     execvp(argv[0],argv); // actually runs the command
     perror(NULL);
     exit(1);
							
    }
  else if (pid >0)
    {
      waitpid(pid,&status,WUNTRACED); //waits for the child to finish first (blocking)
    }	
  else
    {
      perror(NULL);
      
    }
}
void pipe_call(char**argv,pid_t pid,int fd[])
{
 char* argv2[10];
 int i = 0;
 int j =0;
 int status;
 
 for(;i<10;i++)
   {
     argv2[i]=(char*)calloc(1024,sizeof(char));
	 argv2[i][0]='\0';
   }
 
for(i=0;i<10;i++)
   {
     if(strpbrk (argv[i], "|" )!=NULL)
       {
	 argv[i]=NULL;
 
	 i++;

	 for (;i<10;i++)
	   {
	     if(argv[i]!=NULL) 
	     {
	       strcpy(argv2[j],argv[i]);
	        argv[i][0]='\0';
	     j++;
	       }
	     else
	       {
		   argv2[j]=0;
	       i=10;
	       }
	   }
       }
   } 

		       	pipe(fd);
				pid=fork();
				if(pid==0)
				{
				  close(STDOUT_FILENO);
					dup(fd[1]);
					close(fd[0]);
					close(fd[1]); //added
					execvp(argv[0],argv);
					perror(NULL);
					exit(1);
				}
				else if(pid>0)//parent
				{
					//waitpid(pid,&status,WUNTRACED);
					//close(fd[0]);
					//close(fd[1]);
					pid = fork();
					if(pid == 0)
					{
						close(STDIN_FILENO);
						dup(fd[0]);
						close(fd[1]);
						close(fd[0]);
						execvp(argv2[0],argv2);
						perror(NULL);
						exit(1);
					}
					else if(pid>0)
					{
					close(fd[0]);
					close(fd[1]);
					waitpid(pid,&status,WUNTRACED);
					}
				}
				else
					{
						perror(NULL);
						
					}
	close (fd[0]);
	close (fd[1]);
   for(i=0;i<10;i++)
   {
     free(argv2[i]);
   }
}
void merge(char**argv,pid_t pid )
{
	char* filetoopen= argv[4];
	int fdtoopen,status;
	fdtoopen = open(filetoopen,O_RDWR | O_CREAT,0644);// S_IRUSR, S_IWUSR);//
	argv[0]="cat"; //change first comand to cat
	argv[3]=NULL; 

	pid=fork();
				if(pid==0)
				{
					close(STDOUT_FILENO); // close original stdout
					
					dup2(fdtoopen,1); // make the file the default output
					execvp(argv[0],argv);
					perror(NULL);
					exit(1);
				}
				else if(pid>0)//parent
				{
					waitpid(pid,&status,WUNTRACED);
					close(fdtoopen);	//close file
				}
				else
					{
						printf("fail\n");
						perror(NULL);
						
					}
	
}