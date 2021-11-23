#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>


int spawn(const char * program, char ** arg_list) {
  pid_t child_pid = fork();
  if (child_pid != 0)
    return child_pid;

  else {
    execvp (program, arg_list);
    fprintf (stderr, "an error occurred in execvp\n");
    abort ();
  }
}


void build_fifo (const char * name){ 
    if(mkfifo(name, 0666)==-1){
        if (errno != EEXIST){
          perror("There is an error in the creation of the named pipe\n");
          exit (1);
        }
    }
}

int main(){
	pid_t pid_comm, pid_m_x, pid_m_z, pid_insp, pid_wd;
	char pid_motor_x[20], pid_motor_z[20], pid_command[20];
	
	char * arg_list_m_x[] = { "./motor_x", "fd_comm_to_m_x", NULL };
	char * arg_list_m_z[] = { "./motor_z", "fd_comm_to_m_z", NULL };
	char * arg_list_comm[] = { "/usr/bin/konsole",  "-e", "./command", (char*)NULL };
	
	pid_m_x = spawn("./motor_x", arg_list_m_x); 
	pid_m_z = spawn("./motor_z", arg_list_m_z);
	pid_command = spawn("/usr/bin/konsole", arg_list_comm);
	
	
