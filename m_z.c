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

/* fd_from_comm / fd_to_mx helps in identifying an open file within a process while using input/output resources like network sockets or pipes */

int main(void){
	
	/* FIFO that reads from command console */
	char ch;
	int fd_from_comm;
	char * myfifo = "/tmp/myfifo"; 
	mkfifo(myfifo, 0666); 
	fd_from_comm = open(myfifo, O_RDONLY);
	
	/* FIFO that writes to inspection console */
	int fd_to_insp;
	char * myfifo2z = "/tmp/myfifo2z"; 
	mkfifo(myfifo2z, 0666); 
	fd_to_insp = open(myfifo2z, O_WRONLY);
	
	float z = 0.0;
	float z_end = 10.0;
	float error;
	
	/* rimango nel while e ogni volta eseguo solo un if e poi il ciclo si ripete */
	while(1){
	read(fd_from_comm, &ch, sizeof(ch));
	error = (float) rand() /(float) (RAND_MAX/0.025);
	if (ch == 's' && z > 0.25){
		z -= 0.25;
		z = z + error; 
		write(fd_to_insp, &z, sizeof(z));
		sleep(2.5);		
		}
	if (ch == 'w' && z < 9.75){
		z += 0.25;
		z = z + error;
		write(fd_to_insp, &z, sizeof(z));	
		sleep(2.5);		
		}
	}
	
	close(fd_from_comm);
	close(fd_to_insp);
	return 0;
	}
