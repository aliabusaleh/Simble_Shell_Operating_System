## Programming Assignment: Writing a Shell


 The goal of this assignment is to get everyone some familiarity with the system call interface. A secondary goal is to use some of the programming tools provided in the Unix environment. 
In this assignment you are to implement a Unix shell program. A shell is simply a program that conveniently allows you to run other programs.

 Shell is a user interface between user and the internal of OS. So a shell's job is to intercept user's command and then trigger system calls to ask OS to accomplish  the user's tasks. 
A shell mainly consists of two parts: parsing user requests and accomplishing user request with system calls' help.

 In this assignment, you will write your own command shell to gain experience

 with some advanced programming techniques like process creation and control, file descriptors, signals and possibly pipes.

 Each entered command is either a word containing the letters, numbers,. , and /,

 or a single character string containing one of the special characters: < > | &.

## Your shell must support the following:

# (1) A built-in command is one for which no new process is created but instead the functionality is built directly into the shell itself. You should support the following built-in commands: jobs, cd, and exit.

# a. jobs provide a numbered list of processes currently executing in the

# background.
Try waitpid with WNOHANG option to check without blocking. You can either check when jobs called or each time through the main while loop.

# b. cd should change the working directory.

# c. exit should terminate your shell process.

# External commands: which requires forking new process and executing command.

 Your shell must support the following:

## a. A command with no arguments

## Example: ls


## Details: Your shell must block until the command completes and, if the return

## code is abnormal, print out a message indicating that.

## Concepts: Forking a child process, waiting for it to complete, System calls: fork(),

## execvp(), exit(), wait()

## b. A command with arguments

## Example: ls -l

## Details: Argument 0 is the name of the command

## Concepts: Command-line parameters

## c. A command, with or without arguments, executed in the background using &.

## For simplicity, assume that if present the & is always the last thing on the line.

## Example: xemacs &

## Details: In this case, your shell must execute the command and return

## immediately, not blocking until the command finishes.

## Concepts: Background execution, signals, signal handlers, processes,

## asynchronous execution

## System calls: sigset()

## d. A command, with or without arguments, whose output is redirected to a file

## Example: ls -l > foo

## Details: This takes the output of the command and put it in the named file

## Concepts: File operations, output redirection

## System calls: freopen()

## e. A command, with or without arguments, whose input is redirected from a file

## Example: sort < testfile

## Details: This takes the named file as input to the command

## Concepts: Input redirection, more file operations

## System calls: freopen()

## f. A command, with or without arguments, whose output is piped to the input of another command.

## Example: ls -l | more

## Details: This takes the output of the first command and makes it the input to the second command

## Concepts: Pipes, synchronous operation System calls: pipe()

## Note: You must check and correctly handle all return values. This means that you need to read

## the man pages for each function to figure out what the possible return values are, what errors

## they indicate, and what you must do when you get that error.

## Following is a pseudocode of shell program:


int main (int argc, char **argv)
{
while (1){
int childPid;
char * cmdLine;
printPrompt();
cmdLine= readCommandLine(); //or GNU readline("");
cmd = parseCommand(cmdLine);
record command in history list (GNU readline history ?)
if ( isBuiltInCommand(cmd)){
executeBuiltInCommand(cmd);
} else {
childPid = fork();
if (childPid == 0){
executeCommand(cmd); //calls execvp
} else {
if (isBackgroundJob(cmd)){
record in list of background jobs
} else {
waitpid (childPid);
}
}
}
}

# Between this simple pseudocode and full featured shells, there are many optional

# features must program as discussed above. Here are the features you should

# support in your shell. (Note: These are not necessarily in order of easy to hard,

# don't feel that you have to implement them in the order as they are.):

# The prompt you print should indicate the current working directory. For

# example: The directory: /usr/home/abc%


