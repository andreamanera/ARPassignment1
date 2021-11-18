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

int main(void){
	char ch;
	int fd_from_comm;
	char * myfifo = "/tmp/myfifo"; 
	mkfifo(myfifo, 0666); 
	fd_from_comm=open(myfifo, O_RDONLY);
	
	while(1){
	read(fd_from_comm, &ch, sizeof(ch));
	printf("%c",ch);
	fflush(stdout);
	}
	close(fd_from_comm);
	return 0;
	}
