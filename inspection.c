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


int main(void){
	
	float x;
	float z;
	int fd_from_mx;
	int fd_from_mz;
	
	char * myfifo2x = "/tmp/myfifo2x"; 
	mkfifo(myfifo2x, 0666); 
	fd_from_mx = open(myfifo2x, O_RDONLY);
	
	char * myfifo2z = "/tmp/myfifo2z"; 
	mkfifo(myfifo2z, 0666); 
	fd_from_mz = open(myfifo2x, O_RDONLY);
	
	while(1){
		read(fd_from_mx, &x, sizeof(x));
		read(fd_from_mz, &z, sizeof(z));
		
		printf("%f\n", x);
		printf("%f\n", z);
		printf("\n");
		fflush(stdout);
	}
	close(fd_from_mx);
	close(fd_from_mz);
	
	return 0;
}
	
	
