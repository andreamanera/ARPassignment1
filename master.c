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
#include <time.h>

/* We create each child process that will execute the processes instead of the father process. */

int spawn(const char * program, char ** arg_list) {
  pid_t child_pid = fork();
  if (child_pid != 0){
    return child_pid;
  }

  else {
    execvp (program, arg_list);
    fprintf (stderr, "an error occurred in execvp\n");
    abort ();
  }
}

int main(){

	pid_t pid_comm;
	pid_t pid_m_x;
	pid_t pid_m_z; 
	pid_t pid_insp; 
	pid_t pid_wd;
	
	char pid_motor_x[20];
	char pid_motor_z[20];
	
	/* pipe's declaration */
	
	char *fd_comm_to_m_x = "/tmp/x";
	char *fd_comm_to_m_z = "/tmp/z";
	char *fd_to_insp_x = "/tmp/inspx";
	char *fd_to_insp_z = "/tmp/inspz";

	mkfifo(fd_comm_to_m_x, 0666);
	mkfifo(fd_comm_to_m_z, 0666);
	mkfifo(fd_to_insp_x, 0666);
	mkfifo(fd_to_insp_z, 0666);
	
	/* arguments that i have to pass to the process that i want to execute through master's child */
	
	char *arg_list_m_x[] = { "./m_x", fd_comm_to_m_x , NULL };
	char *arg_list_m_z[] = { "./m_z", fd_comm_to_m_z , NULL };
	char *arg_list_comm[] = { "/usr/bin/konsole",  "-e", "./command", (char*)NULL };
	
	/* fork to create various child processes */
	
	pid_m_x = spawn("./m_x", arg_list_m_x); 
	pid_m_z = spawn("./m_z", arg_list_m_z);
	pid_comm = spawn("/usr/bin/konsole", arg_list_comm);
	
	/* sprintf to convert pid from integer to string */
	
	sprintf(pid_motor_x, "%d", pid_m_x);
	sprintf(pid_motor_z, "%d", pid_m_z);

	char *arg_list_insp[] = { "/usr/bin/konsole",  "-e", "./inspection", pid_motor_x, pid_motor_z, (char*)NULL };
	pid_insp = spawn("/usr/bin/konsole", arg_list_insp);
	/*char * arg_list_watchdog[] = {"./watchdog", NULL, NULL };
	pid_watchdog = spawn("./watchdog", arg_list_watchdog); */
	
	

	wait(NULL);

	fflush(stdout);
	
	unlink("fd_comm_to_m_x");
	unlink("fd_comm_to_m_z");
	unlink("fd_to_insp_x");
	unlink("fd_to_insp_z");
	
	return 0;

}
		
