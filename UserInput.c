int input_fd;

void connectFifo();
void inputloop();

int main(){
	connectFifo();
	inputloop();
}

void connectFifo(){
	   char* input_fifo = "/home/pi/tmp/input_fifo";
      //delete in case it already exists
       unlink(input_fifo);
       delay(200);
      if (mkfifo(input_fifo,0666)==-1){
        printf("input_make=error: %s\n",strerror(errno));
      } 
      input_fd=open(input_fifo, O_WRONLY);
      if (input_fd==-1){ 
          printf("input_pipe=error: %s\n",strerror(errno));
      } else {
          printf("input_pipe=connected\n");
      }
}

void inputloop(){
	int in;
	while(sscanf("%d",&in)!=0){
		char WriteBuf[8];
		sprintf(WriteBuf,"%ld",in);
		write(input_fd,WriteBuf,sizeof(WriteBuf));
	}
}