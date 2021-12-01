#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
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

#define RED "\033[31m"
#define RESET "\033[0m"

int main(int argc, char* argv[]){

	printf("Inspection console, press q to stop the hoist and r to reset\n");

	static struct termios oldt;

	void restore_terminal_settings(void){

		tcsetattr(0, TCSANOW, &oldt);  // Apply saved settings 
	}

	void disable_waiting_for_enter(void){

		struct termios newt;

		// Make terminal read 1 char at a time 
		tcgetattr(0, &oldt);  // Save terminal settings 
		newt = oldt;  // Init new settings 
		newt.c_lflag &= ~(ICANON | ECHO);  // Change settings 
		tcsetattr(0, TCSANOW, &newt);  // Apply settings 
		atexit(restore_terminal_settings); // Make sure settings will be restored when program ends  
	}

	// declaration of neede variables 
	
	disable_waiting_for_enter();
	
	char ch;

	pid_t pid_motor_x;
	pid_t pid_motor_z;
	pid_t pid_wd;
	pid_t pid_command;

	double x;
	double z;
	
	int fd_from_mx;
	int fd_from_mz;
	int fd_from_comm;
	int retval;

	struct timeval tv; 

	fd_set rdset;
	
	// pipes opening 
	
	CHECK(fd_from_mx = open("/tmp/inspx", O_RDONLY));
	CHECK(fd_from_mz = open("/tmp/inspz", O_RDONLY));
	CHECK(fd_from_comm = open("/tmp/cti", O_RDONLY));
	
	// convert the pid from string to int 
	
	pid_motor_x = atoi(argv[2]);
	pid_motor_z = atoi(argv[3]);
	pid_wd = atoi(argv[4]);

	CHECK(read(fd_from_comm, &pid_command, sizeof(pid_command)));

	FILE *out = fopen("debug.txt", "a");
 
 	if(out == NULL){
        printf("Error opening FILE");
    }

	while(1){
	
		FD_ZERO(&rdset);
		FD_SET(fd_from_mx, &rdset);
		FD_SET(fd_from_mz, &rdset);
		FD_SET(0,&rdset);
		
		// we want to read instantly the datas from motors 
	
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		
		retval = select(FD_SETSIZE, &rdset, NULL, NULL, &tv);
		
		if (retval == -1){

			perror("select()");
		}
		
		else if (retval >= 0){
			
			if(FD_ISSET(fd_from_mx, &rdset) != 0){

				CHECK(read(fd_from_mx, &x, sizeof(x)));
			}
			
			if(FD_ISSET(fd_from_mz, &rdset) != 0){

				CHECK(read(fd_from_mz, &z, sizeof(z)));
			}
		}
		
		if (FD_ISSET(0,&rdset)>0){
			CHECK(read(0, &ch, sizeof(char)));
		
			if(ch == 'q'){
				
				printf(RED "\nEmergency stop pressed!" RESET "\n");

				CHECK(kill(pid_wd, SIGUSR1));
				CHECK(kill(pid_motor_x, SIGUSR1));
				CHECK(kill(pid_motor_z, SIGUSR1));
				CHECK(kill(pid_command, SIGUSR1));
			
				fprintf(out, RED "Stop button pressed" RESET);
				fflush(stdout);
					
			}
				
			if(ch == 'r'){

				printf(RED "\nEmergency reset pressed!" RESET "\n");

				CHECK(kill(pid_wd, SIGUSR1));
				CHECK(kill(pid_motor_x,SIGUSR2));
				CHECK(kill(pid_motor_z,SIGUSR2));
				CHECK(kill(pid_command,SIGUSR2));
				
				fprintf(out, RED "Reset button pressed" RESET);
				fflush(stdout);
			}
		}

		if (x == 0 && z == 0){

			kill(pid_command, SIGUSR1);	
		}
		
		printf("\rX position: %f meter, Y position: %f meter", x, z);
		fflush(stdout);
		fflush(out);
	}
	
	CHECK(close(fd_from_mx));
	CHECK(close(fd_from_mz));
	CHECK(close(fd_from_comm));
	
	return 0;
}
	
	
