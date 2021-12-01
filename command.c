// Including all the needed libraries.

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

// Defining some colors.

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW  "\033[33m"  
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"       
#define CYAN "\033[36m"   
#define WHITE "\033[37m"  
#define RESET "\033[0m"     

// The CHECK(X) function is usefull to write on shell whenever a system call return an error.
// The function will print the name of the file and the line at which it found the error.
// It will end the check exiting with code 1.

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

// In this part of the code we use the handler() function in order
// to handle signal coming from different processes. Like so we can disable
// the command console during the reset procedure and restart it once it finished the 
// reset routine. 

int l = 0;

void handler(int sig){

	if(sig == SIGUSR2){

		l = 1;
		printf(RED "\nSYSTEM RESETTING!" RESET "\n");
	}

	if(sig == SIGUSR1){

		l = 0;
	}
}

// Main in which we developed the principal code of the command console.

int main(int argc, char *argv[]){
	
	// We had put this structure code just to avoid pressing 
	// "Enter" every time we want to increase/decrease x and z of our joist
	// The tcgetattr() function will get the parameters associated with 
    // the terminal referred to by the first argument and store them in 
    // the termios structure referenced by the second argument.

	static struct termios oldt;

	void restore_terminal_settings(void){

		tcsetattr(0, TCSANOW, &oldt);  // Apply saved settings. 
	}

	void disable_waiting_for_enter(void){

		struct termios newt;

		// Make terminal read 1 char at a time.

		tcgetattr(0, &oldt);  
		newt = oldt;  
		newt.c_lflag &= ~(ICANON | ECHO); 
		tcsetattr(0, TCSANOW, &newt);  
		atexit(restore_terminal_settings);  
	}
     
	// Just some aesthetic stuff.

	printf(BLUE "CCCC  OOOO  M     M  M     M     A     N    N  DD   " RESET "\n");           
	printf(BLUE "C     O  O  M M M M  M M M M    A A    N N  N  D D  " RESET "\n");
    printf(BLUE "C     O  O  M  M  M  M  M  M   AAAAA   N  N N  D  D " RESET "\n");
    printf(BLUE "CCCC  OOOO  M     M  M     M  A     A  N    N  DDDD " RESET "\n");
	printf("\n");
	printf("\n");
    printf(GREEN "This is a robot simulator which simulates the movements of a joist. If you want to move it, press the following buttons on the keyboard.\n");
	printf("\n");
    printf(YELLOW "To move UP, press W" RESET "\n");
    printf(YELLOW "To move DOWN, press S" RESET "\n");
    printf(YELLOW "To move RIGHT, press D" RESET "\n");
    printf(YELLOW "To move LEFT, press A" RESET "\n");
    printf(YELLOW "To STOP z axis, press Z" RESET "\n");
    printf(YELLOW "To STOP x axis, press X" RESET "\n");
    printf("\n");
	
    disable_waiting_for_enter();
    
	// In this part we have declared some varibales usefull in the code.

	// Declaring int for pipes.
    int fd_to_mx;
    int fd_to_mz;
    int fd_to_wd;
	int fd_to_insp;

	// Declaring variables used for select().
    int d = 1;
    int a = 2;
    int w = 3;
    int s = 4;
    int x = 5;
    int z = 6;
    
	// Declaring pid of watchdog and command.
    pid_t pid_wd;
    pid_t pid_command;
    
    char ch;
    
	// Declaring for the use of signals.
    struct sigaction sa; 
    
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=&handler;
    sa.sa_flags=SA_RESTART;
    CHECK(sigaction(SIGUSR2, &sa, NULL));
	CHECK(sigaction(SIGUSR1, &sa, NULL));
    
    pid_wd = atoi(argv[1]);

	// Getting pid of the command, sending it to watchdog and inspection
	// just for using signals.
    CHECK(pid_command = getpid());
    
    // opens pipes to write the commands 
    
    CHECK(fd_to_mx = open("/tmp/x", O_WRONLY));
    CHECK(fd_to_mz = open("/tmp/z", O_WRONLY));
    CHECK(fd_to_wd = open("/tmp/cwd", O_WRONLY));
	CHECK(fd_to_insp = open("/tmp/cti", O_WRONLY));

	// Writing command pid to watchdog and inspection

    CHECK(write(fd_to_wd, &pid_command, sizeof(pid_command)));
	CHECK(write(fd_to_insp, &pid_command, sizeof(pid_command)));

	// It is usefull to use this command to append all the important 
	// things that we want to write in it from all processes, having a chronology
	// of what we press and do. We use the fopen() function to open the debug.txt 
	// and with fprintf we write in the file the specific comment.

	FILE *out = fopen("debug.txt", "a");
 
 	if(out == NULL){
		 
        printf("Error opening FILE");
    }

	// Key reading loop: entering the loop of putting char from keyboard, without exit from program (no return in infinite while loop) 
	
	while (1){

		// Function getchar() to get the button pressed on the keyboard
		ch = getchar();
		

		// Through the use of "l" we can understand in which state we are.
		// if we are in the reset state (case 1), the system is resetting and no command can be
		// insert, except for the q command---> stop command (in case we have to stop the system in an EMERGENCY
		// during the resetting routine), until the reset has terminated. Indeed in case 0 we are in the normal situation in which 
		// we can use the command console, checking through a switch() which button we have entered on the keyboard.

		switch(l){
		
			case 1:
				printf("\n" RED "WAIT, COMMAND CONSOLE DISABLED!" RESET "\n");
				fflush(stdout);
	        break;
	        
			// In the case 0 (normal situation) beyond printing some stuffs, we send a signal
			// to the watchdog (VERY IMPORTANT) in orhter to reset the timer of the alarm, thanks to
			// after 60 seconds the command console will be disabled.

			case 0:

	        	switch(ch){

				    case 119: // case w
						printf(GREEN "Z INCREASE" RESET "\n");
						fflush(stdout);
						CHECK(write(fd_to_mz, &w, sizeof(ch)));
						CHECK(kill(pid_wd, SIGUSR1));
						fprintf(out, "Z INCREASED\n");
                        fflush(out);
				    break;

				    case 115: // case s
						printf(RED "Z DECREASE" RESET "\n");
						fflush(stdout);
						CHECK(write(fd_to_mz, &s, sizeof(ch)));
						CHECK(kill(pid_wd, SIGUSR1));
						fprintf(out, "Z DECREASED\n");
                        fflush(out);
				    break;

				    case 122 :// case z
						printf(CYAN "Z STOP" RESET "\n");
						fflush(stdout);
						CHECK(write(fd_to_mz, &z, sizeof(ch)));
						CHECK(kill(pid_wd, SIGUSR1));
						fprintf(out, "Z STOPPED\n");
                        fflush(out);
				    break;
						
				    case 100: // case d
						printf(WHITE "X INCREASE" RESET "\n");
						fflush(stdout);
						CHECK(write(fd_to_mx, &d, sizeof(ch)));
						CHECK(kill(pid_wd, SIGUSR1));
						fprintf(out, "X INCREASED\n");
                        fflush(out);
				    break;

				    case 97: // case a
						printf(MAGENTA "X DECREASE" RESET "\n");
						fflush(stdout);
						CHECK(write(fd_to_mx, &a, sizeof(ch)));
						kill(pid_wd, SIGUSR1);
						fprintf(out, "X DECREASED\n");
                        fflush(out);
				    break;

				    case 120: // case x
						printf(BLUE "X STOP" RESET "\n");
						fflush(stdout);
						CHECK(write(fd_to_mx, &x, sizeof(ch)));
						CHECK(kill(pid_wd, SIGUSR1));
						fprintf(out, "X STOPPED\n");
                        fflush(out);
				    break;

					default: 
						printf(RED "Wrong command!" RESET "\n");
				}
			break;
			
		}
	}

	// Closing pipes

	CHECK(close(fd_to_mx));
	CHECK(close(fd_to_mz));
	CHECK(close(fd_to_wd));
	CHECK(close(fd_to_insp));

	return 0;
}
