#include "encrypt.h"
#include <stdio.h>
#include <string.h>

//char * sftt_encrypt_func(char * source);
//char * sftt_decrypt_func(char * aim);
void  print_bytes(char *aim);


void print_bytes(char * aim) {
	int len = strlen(aim);
	int j;
        for (j=0; j < len; j++) {
                printf("%d ",aim[j]);
        }
        printf("\n");
}



char * sftt_encrypt_func(char * source){

//	print_bytes(source);
	int len = strlen(source);
	int i;
	for(i = 0; i < len; i++){
		if (source[i] != '\0'){
			source[i] = source[i] + i + 5;
		} else {
			break;
		}
	}
	
	return source;



}







char * sftt_decrypt_func(char * aim){
	int len = strlen(aim);
	int i;
	for(i = 0; i < len; i++ ){
		if (aim[i] !=  '\0'){
			aim[i] = aim[i] - i - 5;
		} else{
			break;
		}
	
	}
//	print_bytes(aim);
	return aim;

}

/*
int main(){
	char source[] = {255,'e','l','l','o','s','s','3','\0'};
	char * tmp = sftt_encrypt_func(source);
	sftt_decrypt_func(tmp);


}
*/


