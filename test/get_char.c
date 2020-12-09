#include <stdio.h>
#include <stdlib.h>

#define TTY_PATH            "/dev/tty"
#define STTY_US             "stty raw -echo -F "
#define STTY_DEF            "stty -raw echo -F "

static int get_char();

static int get_char()
{
    fd_set rfds;
    struct timeval tv;
    int ch = -1;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 10; 

    if (select(1, &rfds, NULL, NULL, &tv) > 0)
    {
        ch = getchar(); 
    }

    return ch;
}

int main()
{
    int ch = 0;
    system(STTY_US TTY_PATH);
	int i = 0;
	
	while (i < 32) {
		ch = get_char();
		if (ch == -1) {
			continue;
		}
		if (ch == 10) {
			break;
		}
		if (ch == 8 && i > 0) {
			--i;
		//	printf("\b ");
			continue;
		}
		putchar('*');
		++i;
	}

	printf("\ninput number: %d\n", i);

	return 0;
}
