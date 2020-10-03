#include <stdio.h>

char * sftt_encrypt_func(char * source , int size){

//	print_bytes(source);
	
	int i;
	for(i = 0; i < size; i++){
		source[i] = source[i] + i + 5;
		
	}
	
	return source;



}

char * sftt_decrypt_func(char * aim, int size){
	int i;
	for(i = 0; i < size; i++ ){
		aim[i] = aim[i] - i - 5;
		
	}
//	print_bytes(aim);
	return aim;

}
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: %s binary_file\n", argv[0]);
		return ;
	}	
	char buffer[102400];
	FILE *fp = fopen(argv[1], "rb");
	int read_count = fread(buffer, 1, 102400, fp);
	fclose(fp);
	sftt_encrypt_func(buffer, read_count);
	sftt_decrypt_func(buffer, read_count);
	fp = fopen("./test", "w");
	fwrite(buffer, 1, read_count, fp);
	fclose(fp);
	return 0;

	
	
	
	 
		
}
