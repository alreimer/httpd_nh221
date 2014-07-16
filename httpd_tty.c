#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

char *file_name, *wr, *rd;
char buf[1024];
int flag;

void usage(void)
{
  printf("Httpd_tty emulator v1.00\n"
	"	<file name> [parm]\n"
	"\n"
	"	 parm :    Filename\n"
	"\n");
}

void save_file(void){
    FILE *f;
    int i = 0;
    if((f = fopen(file_name,"w")) == NULL){
        fprintf( stderr, "Save: Cannot open file\n" );
	exit(1);
    }else{
	if(flag == 0) rd = buf;
	else rd = wr+1;
	while((rd != wr) && i <= 1023){//i is here for shure
	    fputc(*rd, f);
	    rd++; i++;
	    if((rd-buf) >= 1023) rd = buf;
	}
	fclose(f);
    }

}

void sig_handler(int signo){
    switch (signo){
    case SIGALRM:
    /* got an alarm */
	alarm(10);
	save_file();
	break;
    default: printf("SIgnal %d\n", signo);
    }
}


int main(int argc, char *argv[])
{
    int i = 0, j, k = 0;
    char *new_line;
    char ch;	//maybe buffer is to small

  if (argc != 2) {
    usage();
    return (-1);
  }

  file_name = argv[1];
//  j = MIN(strlen(argv[1]), 255);
//  strncpy(file_name, argv[1], j);
//  file_name[j] = '\0';
  flag = 0;
  wr = buf;
  new_line = buf;

    signal(SIGALRM, sig_handler);	//hang functions on signals
    alarm(1);
    memset(buf, 0, 1024);
//        while((ch = fgetc(stdin)) != EOF){ //something is wrong here
        while((ch = fgetc(stdin)) != 0xff){
		switch(ch){
		    case '\n':
			new_line = wr;
			break;
		    case '\r':
			wr = new_line + 1;
			continue;
		    case 0x08:		//backspace
			//k = 1;
			wr--;
			if(wr == (buf-1)) wr = buf+1023;
			continue;
		}
		if((wr - buf) >= 1023) {flag = 1; wr = buf;}	//End of buffer
		if((new_line - buf) >= 1023) new_line = buf;	//End of buffer
		if(k == 0) *wr = ch;
		else k = 0;
		wr++;
	}
    save_file();

  return (0);
}

