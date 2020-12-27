#include <stdio.h>

int main(int argc, char **argv) {
	if (DEBUG_PRINT) {
		printf("%s (version: " VERSION ", release number: %d)\n", argv[0], RELEASE_NUMBER);
	}
	
	return 0;
}
