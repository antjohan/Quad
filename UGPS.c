 #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
 #include <wiringPiI2C.h>
 #include <wiringPi.h>
 #include <inttypes.h>
 #include <stdbool.h>
 #include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define ADDRESS "/home/pi/tmp/GPSsocket"
#define MAX_BUF 500
int from_gps_fd;

int main(){
    pipe1();
}
int pipe1(){
       char* from_gps_fifo = "/home/pi/tmp/from_gps_fifo";
  //delete in case it already exists
   unlink(from_gps_fifo);
   delay(200);
  if (mkfifo(from_gps_fifo,0666)==-1){
    printf("u_make_from_gps=error: %s\n",strerror(errno));
  } 
  from_gps_fd=open(from_gps_fifo, O_RDONLY);

  while(1){
    char gpsbuffer[MAX_BUF];
    int a = read(from_gps_fd,gpsbuffer,MAX_BUF);
    printf("%s\n", gpsbuffer);
    delay(100);
    printf("Not yet...\n");
    }
}
int socketxx()
{
	     printf("Program running\n");
    char c;
    FILE *fp;
    printf("0.1\n");

    int fromlen;
    int i, s, ns, len;
    printf("0.2\n");

    struct sockaddr_un saun, fsaun;

    /*
     * Get a socket to work with.  This socket will
     * be in the UNIX domain, and will be a
     * stream socket.
     */
    printf("1\n");

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("server: socket");
        exit(1);
    }
    printf("2\n");

    /*
     * Create the address we will be binding to.
     */
    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, ADDRESS);

    /*
     * Try to bind the address to the socket.  We
     * unlink the name first so that the bind won't
     * fail.
     *
     * The third argument indicates the "length" of
     * the structure, not just the length of the
     * socket name.
     */
     printf("3\n");
    unlink(ADDRESS);
    len = sizeof(saun.sun_family) + strlen(saun.sun_path);
    if (bind(s, &saun, len) < 0) {
        perror("server: bind");
        exit(1);
    }

    /*
     * Listen on the socket.
     */
     printf("4\n");
    if (listen(s, 5) < 0) {
        perror("server: listen");
        exit(1);
    }

    /*
     * Accept connections.  When we accept one, ns
     * will be connected to the client.  fsaun will
     * contain the address of the client.
     */
    printf("5\n");

    if ((ns = accept(s, &fsaun, &fromlen)) < 0) {
        perror("server: accept");
        exit(1);
    }

    /*
     * We'll use stdio for reading the socket.
     */
     printf("6\n");
    fp = fdopen(ns, "r");

    /*
     * Then we read some strings from the client and
     * print them out.
     */
     printf("7\n");
	while(1){
		printf("scanning for input data\n");
		delay(1000);

        while ((c = fgetc(fp)) != EOF) {
            putchar(c);

            if (c == '\n'){
                break;
        	}
    	}
    }
}
