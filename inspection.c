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


int main(void)
{
	
	
	double x;
	double z;
	int fd_from_mx;
	int fd_from_mz;
	
	char * myfifo2 = "/tmp/myfifo2"; 
	mkfifo(myfifo2, 0666); 
	
	
	char * myfifo3 = "/tmp/myfifo3"; 
	mkfifo(myfifo3, 0666); 

	
	while(1){
		
		fd_from_mx = open(myfifo2, O_RDONLY);
		fd_from_mz = open(myfifo3, O_RDONLY);
			
		read(fd_from_mx, &x, sizeof(x));
		read(fd_from_mz, &z, sizeof(z));
			
		printf("%f\n", x);
		printf("%f\n", z);
		fflush(stdout);
		
		close(fd_from_mx);
		close(fd_from_mz);
	}
	
	return 0;
}
	
	
