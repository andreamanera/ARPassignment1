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
#include <termios.h>

#define YELLOW  "\033[33m"      /* Yellow */
#define RESET "\033[0m"

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

int main(void)
{
    int fd_to_m;
    int ch;
    
    
    char * myfifo = "/tmp/myfifo";
    mkfifo(myfifo, 0666);

    
    printf("This is a robot simulator which simulates a joist. If you want to move it, press the following buttons on the keyboard.\n");
    printf(YELLOW "To move UP, press W" RESET "\n");
    printf(YELLOW "To move DOWN, press S" RESET "\n");
    printf(YELLOW "To move RIGHT, press D" RESET "\n");
    printf(YELLOW "To move LEFT, press A" RESET "\n");
    printf(YELLOW "To STOP z axis, press Z" RESET "\n");
    printf(YELLOW "To STOP x axis, press X" RESET "\n");
    
    
    disable_waiting_for_enter();
    
    
	/* Se schiaccio una freccetta, Wrong command! esce fuori 3 volte!! */
    /* Key reading loop: entering the loop of putting char from keyboard, without exit from program (no return in infinite while loop) */
	while (1){
	
		fd_to_m = open(myfifo, O_WRONLY);
		
		ch = getchar();
		
		if (ch != 'w' && ch != 's' && ch != 'a' && ch != 'd' && ch != 'x' && ch != 'z')
			printf("Wrong command!\n");
		
		if (ch == 'w'){
			printf("Increase Z\n");
			write(fd_to_m, &ch, sizeof(ch));
		}
		
		if (ch == 's'){
			printf("Decrease Z\n");
			write(fd_to_m, &ch, sizeof(ch));
		}
		
		if (ch == 'a'){
			printf("Decrease X\n");
			write(fd_to_m, &ch, sizeof(ch));
		}
		
		if (ch == 'd'){
			printf("Increase X\n");
			write(fd_to_m, &ch, sizeof(ch));
		}
		
		if (ch == 'x'){
			printf("Stop X\n");
			write(fd_to_m, &ch, sizeof(ch));
		}
		
		if (ch == 'z'){
			printf("Stop Z\n");
			write(fd_to_m, &ch, sizeof(ch));
		}
		
		close(fd_to_m);
		
	}
	
	return 0;
}
