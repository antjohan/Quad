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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>

char *portname = "/dev/ttyGPS";
int fd_rtk;
int from_gps_fd;
int to_gps_fd;
char * teststring="2015/04/17 14:36:29.400   57.688079292   11.975502620    90.0067   5   5   3.3981   3.6186   7.1490  -1.5006   1.2235   1.5307   0.00    0.0";
int main(){
    //initialize();
    //sample();
    char * end;
    double d;
    for (int i =0;i<10;++i){
        d = strtod(teststring, &end);
        printf("Value%d=%lf\n",i,d);
        d=end;
    }
}

int initialize(){

    fd_rtk = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    set_interface_attribs (fd_rtk, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    fcntl(fd_rtk, F_SETFL, O_NONBLOCK);

}

int sample(){
    double date,time,
    while(1){
        char buf [300];
        if(read (fd_rtk, buf, sizeof buf)>0){  // read up to 100 characters if ready to read
            printf("%s\n",buf);

         }   
         delay(50);
    }
}

void connectFifos(){
   char* from_gps_fifo = "/home/pi/tmp/from_gps_fifo";
   char* to_gps_fifo = "/home/pi/tmp/to_gps_fifo";
  //delete in case it already exists
   unlink(from_gps_fifo);
   delay(200);
  if (mkfifo(from_gps_fifo,0666)==-1){
    printf("u_make_from_gps=error: %s\n",strerror(errno));
  } 
  from_gps_fd=open(from_gps_fifo, O_WRONLY);
  if (from_ultra_fd==-1){ 
      printf("pipe_from_gps=error: %s\n",strerror(errno));
  } else {
      printf("pipe_from_gps=connected\n");
  }


  to_gps_fd=open(to_gps_fifo, O_RDONLY);
  if (to_gps_fd==-1){
       printf("pipe_to_gps=error: %s\n",strerror(errno));
  } else {
      //printf("pipe_to_gps=connected\n");
  }

}

void writeOutput(){
   char WriteBuf[56];
   sprintf(WriteBuf,"%ld",currentHeight-offset);
   write(from_gps_fd,WriteBuf,sizeof(WriteBuf));
}

void checkPipe(){
   char buffer[10];

   char str1[10];
   char str2[10];

   strcpy(str1,"ping");
   strcpy(str2, "read");

      if (read(to_ultra_fd, buffer, 10)>0){
          if (strcmp(buffer,str1)==0){
            printf("GPS-process sends his regards\n");
          } else if(strcmp(buffer,str2)==0){
            writeOutput();
          }
      }
}

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;

        return 0;
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
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int socketxy()
{
    int portno=20000;
     int sockfd, newsockfd, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     /*if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }*/
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     printf("got this far!\n");
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     printf("something connected?!\n");

     while(1){
        char buffer[1];
            if (read(newsockfd,buffer,1)>0){   
            printf("something was read\n");
            printf("%s\n",buffer);     
        }
        delay(100);
     }
     return 0; 
}
int socketyy(){
    /* Sample TCP client */
   int sockfd,n;
   char * ip="172.0.0.1";
   struct sockaddr_in servaddr,cliaddr;
   char sendline[1000];
   char recvline[1000];

   sockfd=socket(AF_INET,SOCK_STREAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(ip);
   servaddr.sin_port=60;
   printf("attempting to connect\n");

   connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
   printf("something connected?\n");
 /*  while (fgets(sendline, 10000,stdin) != NULL)
   {
      sendto(sockfd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
      n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
      recvline[n]=0;
      fputs(recvline,stdout);
   }*/
      char BUF[500];
      while(1){
        read(sockfd,BUF,sizeof(BUF));
        printf("something read?\n");
        printf("%s\n",BUF);
      }
}
