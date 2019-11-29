#include <iostream>
#include"stdio.h"
#include"stdlib.h"
#include"unistd.h"
#include"string.h"
#include"sys/types.h"
#include"sys/wait.h"
#include"sys/stat.h"
#include"fcntl.h"
#include"vector"
#include <algorithm>
using namespace std;

#define BUFFERSIZE 200
#define MAX_WORD 10
#define MAX_CHAR 100
char rootPath[BUFFERSIZE];
int input_redirection_flag;
int output_redirection_flag;
int pipe_flag;
int append_flag;
int bg_flag;//background indicator
char * input_file=NULL;
char * output_file=NULL;
vector <int> bgList;
////////////////////////////////
/*
1- the remove_endOf_line function check if the command for backend and
2- put new line insted 
*/
void remove_endOf_line(char line[]){
int i=0;
while(line[i]!='\n'){
if(line[i]=='&'){
bg_flag=1;
break; }// end of if 
i++; } // end of while
line[i]='\0';
} // end of funcion
////////////////////////////////

/*
1- the read_line function will reads the line and call remove_endOf_line function then
2- if it's exit then it will destroy everything by calling exit(1)
*/
void read_line(char line[]){
char * ret=fgets(line,MAX_CHAR,stdin);
remove_endOf_line(line);
if(strcmp(line,"exit")==0||ret==NULL) exit(1);
} //end of function 
////////////////////////////////

/*
1- the process_line function will take the line what we checked if it background or foreground and start processing
2- if line is empty then just go back
*/
int process_line(char * temp[],char line[])
{
int i=0;
temp[i]=strtok(line," ");
if(temp[i]==NULL) return 1;
while(temp[i]!=NULL){
i++;
temp[i]=strtok(NULL," "); } // end of while loop 
return 1;
} // end if function
////////////////////////////////

/*
1- the pipe_and_redirection_checking will take care for piping by setting flags
*/
int pipe_and_redirection_checking(char * temp[]){
int i=0;
while(temp[i]!=NULL){
if(strcmp(temp[i],">>")==0){
append_flag=1;
output_file=temp[i+1];
return i;
} // end if for >> 
if(strcmp(temp[i],">")==0){
output_redirection_flag=1;
output_file=temp[i+1];
return i;
} // end if for > 
if(strcmp(temp[i],"<")==0){
input_redirection_flag=1;
input_file=temp[i+1];
return i;
} // end if for < 
if(strcmp(temp[i],"|")==0){
pipe_flag=1;
return i;
}// end if for | 
i++;
} // end for main while loop 
return i;
} // end of function 
////////////////////////////////

/*
1- the function check line will check the command line to check if the command is wrong in redirection 
2- for example if any unused like * of () then it will print that it's wrong
*/
void check_line(char * temp[])
{

int pipe_cnt=0;
int output_redirection_cnt=0;
int input_redirection_cnt=0;
int i=0;
int total_cnt=0;
if(temp[i]==NULL) return ; // empty 
while(temp[i]!=NULL){
if(strcmp(temp[i],">")==0) output_redirection_cnt++; 
if(strcmp(temp[i],"<")==0) input_redirection_cnt++;
if(strcmp(temp[i],"|")==0) pipe_cnt++;
i++;
}// end of while loop 
//*****
total_cnt = pipe_cnt + output_redirection_cnt + input_redirection_cnt + bg_flag; // if >1  this means that there's unknown symbol
if(total_cnt>1){
cout<<"ERROR:command syntax is incorrect \n";
temp[0]=NULL;}
}// end of function
////////////////////////////////


/*
1- the function will read the input and parse it using previos functions
2- it will check if anything is wrong
3- it will check if any pipeling and check the symols
*/
int read_parse_line(char * args[],char * pipe_array[],char line[])
{
char * temp[MAX_WORD];
int pos;
int i=0;
read_line(line);
process_line(temp,line);
//check if << or >> or <> or empty line or || or
//<| or >| this is error line
check_line(temp);
pos=pipe_and_redirection_checking(temp);
while(i<pos){
args[i]=temp[i];
i++;
}// end if while 
args[i]=NULL;
i++;
if(pipe_flag==1){
int j=0;
while(temp[i]!=NULL){
pipe_array[j]=temp[i];
i++;j++;
}// end of while
}// end of if statement
return 1;
}// end of function
//////////////////////////////



/*
1- the following function will handle the pipe if any exist 
*/
void pipe_handler(char * args[],char * pipe_array[],int pipefd[])
{
int pid;
int i;
pid=fork();
int status;
if(pid==0)
{
  dup2(pipefd[1],1); // These system calls create a copy of the file descriptor oldfd. 
  close(pipefd[0]);//this parent dose not need this end of pipe
  /* The execv(), execvp(), and execvpe() functions provide an array of pointers to null-terminated strings 
   that represent the argument list available to the new program.
   The first argument, by convention, should point to the filename associated with the file being executed.
   The array of pointers must be terminated by a NULL pointer.  */
  if(execvp(args[0],args)==-1) cout<<"this is not command\n";
}
else{
  waitpid(pid,&status,0);
  dup2(pipefd[0],0);
  close(pipefd[1]);//this child dose not need this end of pipe
  if(execvp(pipe_array[0],pipe_array)==-1)
  cout<<"Command Error - Not a Command- \n";
  //perror(pipe_array[0]);
}
}// end of function
////////////////////////////////


/*
1- the folloing function will change the directory for the CWD
*/
void cd(char *pth)
{
    char path[BUFFERSIZE];
    char cwd[BUFFERSIZE];
    if(pth[0]=='\0'){
       strcpy(cwd,rootPath);
       chdir(cwd);
       return;}
    strcpy(path,pth);
    if(pth[0] != '/')
    {// true for the dir in cwd
        getcwd(cwd,sizeof(cwd));
        strcat(cwd,"/");
        strcat(cwd,path);
        string str(cwd);
       
        if(chdir(cwd)==-1)
        {
            string str1(path);
            cout<<str1<<" : No file or Directory \n";
        }
    }
    else
    {//true for dir w.r.t. /
       string str(path);
       cout<<str<<" : No file or Directory \n";
    }

} // end of fucntion
////////////////////////////////


/*
1- the following function for printing the prompt 
*/
void printPrompt()
{
      char cwd[BUFFERSIZE];
      getcwd(cwd,sizeof(cwd));
      string str(cwd);
      cout<<str<<" : ";
      return;
}
////////////////////////////////
void handler(int sig)
{
  pid_t pid=0;
  int   status;
  pid=waitpid(-1, &status, WNOHANG);
  if(pid>=1)
    bgList.erase(std::remove(bgList.begin(), bgList.end(), (int)pid), bgList.end());
}
////////////////////////////////
int main(int argc, char *argv[])
{
    char * args[MAX_WORD];
    char * pipe_array[MAX_WORD];
    char   line[MAX_CHAR];
    getcwd(rootPath,sizeof(rootPath));
    signal(SIGCHLD, handler);
    while(1)
    {
      for(int i=0;i<MAX_WORD;i++)
         args[i]=NULL;
      for(int i=0;i<MAX_WORD;i++)
         pipe_array[i]=NULL;
      printPrompt();
      read_parse_line(args,pipe_array,line);
      if(line[0]=='\0')
      continue;
      if(strcmp(args[0],"jobs")==0)
      {
         cout<<"Number of processes currently executing in background : "<<bgList.size()<<endl;
         continue;
      }
      if(strcmp(args[0],"cd")!=0)
      {
        pid_t pid=fork();
        //signal(SIGCHLD, handler);
        int status=0;
        if(pid==0)
        {
            //i'm in child
            if(append_flag==1 && output_file!=NULL)
              dup2(open(output_file,O_RDWR|O_APPEND,0777),1);
            if(input_redirection_flag==1 && input_file!=NULL)
              dup2(open(input_file,O_RDWR|O_CREAT,0777),0);
            if(output_redirection_flag==1 && output_file!=NULL)
              dup2(open(output_file,O_RDWR|O_CREAT|O_TRUNC,0777),1);
            if(pipe_flag==1)
            {
            int pipefd[2];
            pipe(pipefd);
              pipe_handler(args,pipe_array,pipefd);
              //exit(0);
            }
            else if(execvp(args[0],args)==-1)
            cout<<"Command Error - not found- \n";
        }
        else
        {
            if(bg_flag==0)
            {
               //i'm in parent
               waitpid(pid,&status,0);
            }
            else
            {
               bgList.push_back((int)pid);
            }
            input_redirection_flag=0;
            output_redirection_flag=0;
            pipe_flag=0;
            input_file=NULL;
            output_file=NULL;
            append_flag=0;
        }
        bg_flag=0;
      }
      else
      {
         cd(args[1]);
      }
      cout<<'\n';
    }
    return 0;
}
