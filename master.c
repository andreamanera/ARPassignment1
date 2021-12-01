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
#include <sys/wait.h>

#define CHECK(X) (                                                 \
    {                                                              \
        int __val = (X);                                           \
        (__val == -1 ? (                                           \
                           {                                       \
                               fprintf(stderr, "ERROR ("__FILE__   \
                                               ":%d) -- %s\n",     \
                                       __LINE__, strerror(errno)); \
                               exit(EXIT_FAILURE);                 \
                               -1;                                 \
                           })                                      \
                     : __val);                                     \
    })

// We create each child process that will execute the processes instead of the father process. 

int spawn(const char * program, char ** arg_list) {
	
	pid_t child_pid = fork();

  	if (child_pid != 0){

    	return child_pid;
    }

  	else {
		CHECK(execvp(program, arg_list));
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
	char pid_watchdog[20];
	
	// pipe's declaration 
	
	CHECK(mkfifo("/tmp/x", 0666));
	CHECK(mkfifo("/tmp/z", 0666));
	CHECK(mkfifo("/tmp/inspx", 0666));
	CHECK(mkfifo("/tmp/inspz", 0666));
	CHECK(mkfifo("/tmp/cwd", 0666));
	CHECK(mkfifo("/tmp/cti", 0666));
	
	// arguments that i have to pass to the process that i want to execute through master's child 
	
	char *arg_list_m_x[] = { "./m_x", "/tmp/x", NULL };
	char *arg_list_m_z[] = { "./m_z", "/tmp/z" , NULL };
	
	// fork to create various child processes 
	
	pid_m_x = spawn("./m_x", arg_list_m_x); 
	pid_m_z = spawn("./m_z", arg_list_m_z);

	sprintf(pid_motor_x, "%d", pid_m_x);
	sprintf(pid_motor_z, "%d", pid_m_z);
	
	char *arg_list_watchdog[] = {"./watchdog", "/tmp/cwd", pid_motor_x, pid_motor_z, (char*)NULL };
	pid_wd = spawn("./watchdog", arg_list_watchdog);
	sprintf(pid_watchdog, "%d", pid_wd);
	
	char *arg_list_comm[] = { "/usr/bin/konsole",  "-e", "./command", pid_watchdog, (char*)NULL };
	pid_comm = spawn("/usr/bin/konsole", arg_list_comm);

	char *arg_list_insp[] = { "/usr/bin/konsole",  "-e", "./inspection", "/tmp/cti", pid_motor_x, pid_motor_z, pid_watchdog, (char*)NULL };
	pid_insp = spawn("/usr/bin/konsole", arg_list_insp);
	
	CHECK(wait(NULL));
	
	CHECK(unlink("/tmp/x"));
	CHECK(unlink("/tmp/z"));
	CHECK(unlink("/tmp/inspx"));
	CHECK(unlink("/tmp/inspz"));
	CHECK(unlink("/tmp/cwd"));
	CHECK(unlink("/tmp/cti"));
	
	
	return 0;

}
		
