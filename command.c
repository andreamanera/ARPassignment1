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

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW  "\033[33m"  
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"       
#define CYAN "\033[36m"   
#define WHITE "\033[37m"  
#define RESET "\033[0m"     

int l = 0;

void handler(int sig){

	if(sig == SIGUSR2){

		l = 1;
		printf(RED "\nCOMMAND CONSOLE DISABLED!!" RESET "\n");
	}

	if(sig == SIGUSR1){

		l = 0;
	}
}

int main(int argc, char *argv[]){
	
	static struct termios oldt;

	void restore_terminal_settings(void){

		tcsetattr(0, TCSANOW, &oldt);  // Apply saved settings 
	}

	void disable_waiting_for_enter(void){

		struct termios newt;

		// Make terminal read 1 char at a time 

		tcgetattr(0, &oldt);  
		newt = oldt;  
		newt.c_lflag &= ~(ICANON | ECHO); 
		tcsetattr(0, TCSANOW, &newt);  
		atexit(restore_terminal_settings);  
	}
     
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
    
    int fd_to_mx;
    int fd_to_mz;
    int fd_to_wd;
	int fd_to_insp;

    int d = 1;
    int a = 2;
    int w = 3;
    int s = 4;
    int x = 5;
    int z = 6;
    
    pid_t pid_wd;
    pid_t pid_command;
    
    char ch;
    
    struct sigaction sa; 
    
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=&handler;
    sa.sa_flags=SA_RESTART;
    sigaction(SIGUSR2, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
    
    pid_wd = atoi(argv[1]);
    pid_command = getpid();
    
    // opens pipes to write the commands 
    
    fd_to_mx = open("/tmp/x", O_WRONLY);
    fd_to_mz = open("/tmp/z", O_WRONLY);
    fd_to_wd = open("/tmp/cwd", O_WRONLY);
	fd_to_insp = open("/tmp/cti", O_WRONLY);
    
    write(fd_to_wd, &pid_command, sizeof(pid_command));
	write(fd_to_insp, &pid_command, sizeof(pid_command));

	FILE *out = fopen("debug.txt", "a");
 
 	if(out == NULL){
		 
        printf("Error opening FILE");
    }

	// Key reading loop: entering the loop of putting char from keyboard, without exit from program (no return in infinite while loop) 
	
	while (1){

		ch = getchar();
		
		switch(l){
		
			case 1:
				printf("\n" RED "SYSTEM RESETTING!" RESET "\n");
				fflush(stdout);
	        break;
	        
			case 0:

	        	switch(ch){

				    case 119: // case w
						printf(GREEN "Z INCREASE" RESET "\n");
						fflush(stdout);
						write(fd_to_mz, &w, sizeof(ch));
						kill(pid_wd, SIGUSR1);
						fprintf(out, "Z INCREASED\n");
                        fflush(out);
				    break;

				    case 115: // case s
						printf(RED "Z DECREASE" RESET "\n");
						fflush(stdout);
						write(fd_to_mz, &s, sizeof(ch));
						kill(pid_wd, SIGUSR1);
						fprintf(out, "Z DECREASED\n");
                        fflush(out);
				    break;

				    case 122 :// case z
						printf(CYAN "Z STOP" RESET "\n");
						fflush(stdout);
						write(fd_to_mz, &z, sizeof(ch));
						kill(pid_wd, SIGUSR1);
						fprintf(out, "Z STOPPED\n");
                        fflush(out);
				    break;
						
				    case 100: // case d
						printf(WHITE "X INCREASE" RESET "\n");
						fflush(stdout);
						write(fd_to_mx, &d, sizeof(ch));
						kill(pid_wd, SIGUSR1);
						fprintf(out, "X INCREASED\n");
                        fflush(out);
				    break;

				    case 97: // case a
						printf(MAGENTA "X DECREASE" RESET "\n");
						fflush(stdout);
						write(fd_to_mx, &a, sizeof(ch));
						kill(pid_wd, SIGUSR1);
						fprintf(out, "X DECREASED\n");
                        fflush(out);
				    break;

				    case 120: // case x
						printf(BLUE "X STOP" RESET "\n");
						fflush(stdout);
						write(fd_to_mx, &x, sizeof(ch));
						kill(pid_wd, SIGUSR1);
						fprintf(out, "X STOPPED\n");
                        fflush(out);
				    break;
				}
			break;
			
		}
	}

	close(fd_to_mx);
	close(fd_to_mz);
	close(fd_to_wd);
	close(fd_to_insp);

	return 0;
}
