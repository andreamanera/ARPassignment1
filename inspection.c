#include<stdio.h>
#include <termios.h>              
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char* argv[])
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
	/* declaration of neede variables */
	
	disable_waiting_for_enter();
	
	char ch;
	int pid_motor_x;
	int pid_motor_z;
	double x;
	double z;
	int fd_from_mx;
	int fd_from_mz;
	int retval;
	struct timeval tv; 
	fd_set rdset;
	
	/* pipes opening */
	
	fd_from_mx = open("fd_to_insp_x", O_RDONLY);
	fd_from_mz = open("fd_to_insp_z", O_RDONLY);
			
	if(fd_from_mx == -1){
		printf("Error opening FIFO from command to motor x");
		return(1);
	}
	
	if(fd_from_mz == -1){
		printf("Error opening FIFO from motor x to inspection");
		return(1);
	}
	
	/* convert the pid from string to int */
	
	pid_motor_x = atoi(argv[1]);
	pid_motor_z = atoi(argv[2]);

	while(1){
		
		FD_ZERO(&rdset);
		FD_SET(fd_from_mx, &rdset);
		FD_SET(fd_from_mz, &rdset);
		
		/* we want to read instantly the datas from command */
	
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		
		retval = select(FD_SETSIZE, &rdset, NULL, NULL, &tv);
		
		if (retval == -1)
			perror("select()");
		
		else if (retval >= 0){
			if(FD_ISSET(fd_from_mx, &rdset) != 0)
				read(fd_from_mx, &x, sizeof(x));
			
			if(FD_ISSET(fd_from_mz, &rdset) != 0)
				read(fd_from_mz, &z, sizeof(z));
		}
		
		ch = getchar();
		
		if (ch == 'r'){
			kill(pid_motor_x, SIGUSR2);
			kill(pid_motor_z, SIGUSR2);
		}
		
		if (ch == 'q'){
			kill(pid_motor_x, SIGUSR1);
			kill(pid_motor_z, SIGUSR1);
		}
		
		printf("X position : %f meter, Y position: %f meter", x, z);
		fflush(stdout);
	}
	
	close(fd_from_mx);
	close(fd_from_mz);
	
	return 0;
}
	
	
