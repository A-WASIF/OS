#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main(){
	for(int i = 0; i < 4; i++){
		fork();
	}
}
