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
	char pid_command[20];
	char pid_watchdog[20];
	
	/* pipe's declaration */
	
	mkfifo("/tmp/x", 0666);
	mkfifo("/tmp/z", 0666);
	mkfifo("/tmp/inspx", 0666);
	mkfifo("/tmp/inspz", 0666);
	mkfifo("/tmp/cwd", 0666);
	
	/* arguments that i have to pass to the process that i want to execute through master's child */
	
	char *arg_list_m_x[] = { "./m_x", "/tmp/x", NULL };
	char *arg_list_m_z[] = { "./m_z", "/tmp/z" , NULL };
	char *arg_list_comm[] = { "/usr/bin/konsole",  "-e", "./command", pid_watchdog, (char*)NULL };
	
	/* fork to create various child processes */
	
	pid_m_x = spawn("./m_x", arg_list_m_x); 
	pid_m_z = spawn("./m_z", arg_list_m_z);
	pid_comm = spawn("/usr/bin/konsole", arg_list_comm);
	
	/* sprintf to convert pid from integer to string */
	
	sprintf(pid_motor_x, "%d", pid_m_x);
	sprintf(pid_motor_z, "%d", pid_m_z);
	sprintf(pid_command, "%d", pid_comm);

	char *arg_list_insp[] = { "/usr/bin/konsole",  "-e", "./inspection", pid_motor_x, pid_motor_z, pid_watchdog, (char*)NULL };
	pid_insp = spawn("/usr/bin/konsole", arg_list_insp);
	char *arg_list_watchdog[] = {"./watchdog", pid_motor_x, pid_motor_z, pid_command, NULL };
	pid_wd = spawn("./watchdog", arg_list_watchdog);
	
	sprintf(pid_watchdog, "%d", pid_wd);
	
	wait(NULL);
	
	unlink("/tmp/x");
	unlink("/tmp/z");
	unlink("/tmp/inspx");
	unlink("/tmp/inspz");
	unlink("/tmp/cwd");
	
	return 0;

}
		
