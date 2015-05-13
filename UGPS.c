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
    #include <libconfig.h>

    void initialize();
    void sample();
    void connectFifos();
    void writeOutput();
    void checkPipe();
    int set_interface_attribs();


    char *portname = "/dev/ttyGPS";
    int fd_rtk;
    int from_gps_fd;
    int to_gps_fd;
    char * teststring = "2015/04/17 14:36:29.400   57.688079292   11.975502620    90.0067   5   5   3.3981   3.6186   7.1490  -1.5006   1.2235   1.5307   0.00    0.0";
    double data[13];

    int main(){
        connectFifos();
        initialize();
        sample();
    }

    void initialize(){
        fopen(portname,"w");
        fd_rtk = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
        set_interface_attribs (fd_rtk, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
        fcntl(fd_rtk, F_SETFL, O_NONBLOCK);
                    /*/only for testing
                    char * str;
                    strcpy(str, teststring);
                    char * end;
                    str = strstr(str,"  ");
                    for (int i =0;i<13;++i){
                        data[i] = strtod(str, &end);
                       // printf("Value%d=%lf\n",i,d);
                        str=end;
                    }
                    //----------------*/
    }

    void sample(){
        double date,tim;
        while(1){
            char buf [300];
            char * str;
            checkPipe();
            if(read (fd_rtk, buf, sizeof buf)>0){  // read up to 100 characters if ready to read
                    strcpy(str, buf);
                    char * end;
                    str = strstr(str,"  ");
                    for (int i =0;i<13;++i){
                        data[i] = strtod(str, &end);
                        //printf("Value%d=%lf\n",i,d);
                        str=end;
                    }
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
      if (from_gps_fd==-1){ 
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
        char * WriteBuf[128];
        double latitude=data[1];
        double longitude=data[2];
        double satquality=data[4];
        double nsat=data[5];
        double sdn=data[6];
        double sde=data[7];

        sprintf(WriteBuf,"%ld %ld %ld %ld %ld %ld",latitude, longitude, satquality, nsat, sdn, sde);
      //  printf("Writing to pipe: %s\n", WriteBuf);
        write(from_gps_fd,WriteBuf,sizeof(WriteBuf));
    }

    void checkPipe(){
    char buffer[10]={""};

       char str1[10];
       char str2[10];

       strcpy(str1,"ping");
       strcpy(str2, "read");

       char strlast[10]={""};
       while (read(to_gps_fd, strlast, 10)>0){
            buffer=strlast;
       }
          if (strcmp(buffer,str1)==0){
            printf("GPS-process sends his finest regards\n");
          } else if(strcmp(buffer,str2)==0){
            writeOutput();
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