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

#define YELLOW  "\033[33m"      /* Yellow */
#define RESET "\033[0m"

char ch;

void handler(int sig){
	if(sig==SIGUSR1){
	}
	if(sig==SIGUSR2){
		ch = 'o';
		}
}


int main(int argc, char *argv[])
{
	static struct termios oldt;

	void restore_terminal_settings(void)
	{
		tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
	}

	void disable_waiting_for_enter(void)
	{
		struct termios newt;

		/* Make terminal read 1 char at a time */
		tcgetattr(0, &oldt);  /* Save terminal settings */
		newt = oldt;  /* Init new settings */
		newt.c_lflag &= ~(ICANON | ECHO);  /* Change settings */
		tcsetattr(0, TCSANOW, &newt);  /* Apply settings */
		atexit(restore_terminal_settings); /* Make sure settings will be restored when program ends  */
	}
    
    printf("This is a robot simulator which simulates a joist. If you want to move it, press the following buttons on the keyboard.\n");
    printf(YELLOW "To move UP, press W" RESET "\n");
    printf(YELLOW "To move DOWN, press S" RESET "\n");
    printf(YELLOW "To move RIGHT, press D" RESET "\n");
    printf(YELLOW "To move LEFT, press A" RESET "\n");
    printf(YELLOW "To STOP z axis, press Z" RESET "\n");
    printf(YELLOW "To STOP x axis, press X" RESET "\n");
    
    disable_waiting_for_enter();
    
    int fd_to_mx;
    int fd_to_mz;
    
    int pid_wd;
    
    int d = 1;
    int a = 2;
    int w = 3;
    int s = 4;
    int x = 5;
    int z = 6;
    
    struct sigaction sa; 
    
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=&handler;
    sa.sa_flags=SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2,&sa,NULL);
    
    pid_wd=atoi(argv[1]);
    // opens pipe to write the commands 
    
    fd_to_mx = open("/tmp/x", O_WRONLY);
    fd_to_mz = open("/tmp/z", O_WRONLY);
    
    /* Key reading loop: entering the loop of putting char from keyboard, without exit from program (no return in infinite while loop) */
	
	while (1){
	
		ch = getchar();
		
		switch(ch){

		    case 119: // case w
				printf("Z increase\n");
				fflush(stdout);
				write(fd_to_mz, &w, sizeof(ch));
				kill(pid_wd, SIGUSR1);
		    break;

		    case 115: // case s
				printf("Z decrease\n");
				fflush(stdout);
				write(fd_to_mz, &s, sizeof(ch));
				kill(pid_wd, SIGUSR1);
		    break;

		    case 122 :// case z
				printf("Z stop\n");
				fflush(stdout);
				write(fd_to_mz, &z, sizeof(ch));
				kill(pid_wd, SIGUSR1);
		    break;
		                
		    case 100: // case d
				printf("X increase\n");
				fflush(stdout);
				write(fd_to_mx, &d, sizeof(ch));
				kill(pid_wd, SIGUSR1);
		    break;

		    case 97: // case a
				printf("X decrease\n");
				fflush(stdout);
				write(fd_to_mx, &a, sizeof(ch));
				kill(pid_wd, SIGUSR1);
		    break;

		    case 120: // case x
				printf("X stop\n");
				fflush(stdout);
				write(fd_to_mx, &x, sizeof(ch));
				kill(pid_wd, SIGUSR1);
		    break;
		    
		    case 111:
		               printf("Command console disabled");
		    break;
		    
		}
			 
		
	}
	close(fd_to_mx);
	close(fd_to_mz);
	return 0;
}
