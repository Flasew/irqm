#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>


void test(char * msg){
	
	int fd;
	
	fd = open("/dev/acb",O_WRONLY);
	
	write(fd,msg,128);
	
	printf("write\n");
	fflush(stdout);
	
	fsync(fd);
	
	printf("fsync\n");	
	fflush(stdout);
	
	close(fd);
	
	printf("close\n");
	fflush(stdout);
}


int main() {
	
	test("a\n");
	test("b\n");
	test("c\n");

	return 0;
	
}
