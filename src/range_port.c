#include<stdio.h>
#include<time.h>
#include <stdlib.h>

int Get_Range_Port();

int Get_Range_Port(){
	time_t t;
	int port;

	//t = time(NULL);
	srand((unsigned)(time(NULL)));
	port = rand()%(65535-1025) + 1025;
	printf("time is %d\n",port);
	return port;

}

void main(){
	Get_Range_Port();
}
