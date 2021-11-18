#include <stdio.h>
#include <stdlib.h>
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

int main()
{
    int ch;

    disable_waiting_for_enter();
    
    printf("This is a robot simulator which simulates a joist. If want to move it, press the following buttons on the keyboard!!\n");
    printf(YELLOW "To move UP, press W" RESET "\n");
    printf(YELLOW "To move DOWN, press S" RESET "\n");
    printf(YELLOW "To move RIGHT, press D" RESET "\n");
    printf(YELLOW "To move LEFT, press A" RESET "\n");
    printf(YELLOW "To STOP z axis, press Z" RESET "\n");
    printf(YELLOW "To STOP x axis, press X" RESET "\n");
    
	/* Se schiaccio una freccetta, Wrong command! esce fuori 3 volte!! */
    /* Key reading loop: entering the loop of putting char from keyboard, without exit from program (no return in infinite while loop) */
	while (1) {
        ch = getchar();
        if (ch != 'w' && ch != 's' && ch != 'a' && ch != 'd' && ch != 'x' && ch != 'z')
        printf("Wrong command!\n");
        if (ch == 'w')
        printf("Increase Z\n");
        if (ch == 's')
        printf("Decrease Z\n");
        if (ch == 'a')
        printf("Decrease X\n");
        if (ch == 'd')
        printf("Increase X\n");
        if (ch == 'x')
        printf("Stop X\n");
        if (ch == 'z')
        printf("Stop Z\n");
        
	}

	return 0;
}
