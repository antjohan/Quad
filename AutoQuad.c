#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <libconfig.h>
#include <termios.h>
#include <fcntl.h>

#include "SensorFusion.c"
#include "FlightControl.c"
#include "testCases.c"

int input_fd;

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}


int main(){

	system("./UGPS &");
	system("./HMC5883L &");
	system("./BMP180 &");
	system("./HC-SR04 &");

    wiringPiSetupSys();
    sfinit();
    
    while(1){
        
        system("clear");
        printf("---------------------------------\n");
        printf("Main Menu\n");
        printf("---------------------------------\n");
        printf("[1]	Arm FlightController\n");
        printf("[2]	Disarm FlightController\n");
        printf("[3]	Servo menu\n");
        printf("[4]	Test menu\n");
        printf("[5]	Quit\n");
        printf("---------------------------------\n");
        
        int main_menu;
        scanf("%d", &main_menu);
        
        system("clear");
        
        if(main_menu == 1){
            printf("Arming...\n");
            Arm_FlightController();
            printf("Done!\n");
            
            
        }else if(main_menu == 2){
            printf("Disarming...\n");
            Disarm_FlightController();
            printf("Done!\n");
            
        }else if(main_menu == 3){
            int servo;
            int speed;
            
            printf("---------------------------------\n");
            printf("Servo Menu\n");
            printf("---------------------------------\n");
            printf("[1] Set Aileron\n");
            printf("[2] Set Elevation\n");
            printf("[3] Set Throttle\n");
            printf("[4] Set Ruder\n");
            printf("---------------------------------\n");
            
            scanf("%d",&servo);
            
            printf("Set Speed (0-100)\n");
            printf("---------------------------------\n");
            
            scanf("%d", &speed);
            
            Set_Servo(servo, speed);

            system("clear");

            
        }else if(main_menu == 4){
            test_cfg_init();
            test_cfg_print();

            int test_menu;
            system("clear");
            printf("---------------------------------\n");
            printf("Test Menu\n");
            printf("---------------------------------\n");
            printf("[1]	ping...\n");
            printf("[2]	recieve data\n");
            printf("[3]	Testa hoverToStep\n");
            printf("[4]	Testa ett varv rotation\n");
            printf("[5]	Calibrate magnetometer\n");
            printf("[6]	Test PID regulator\n");
            printf("[7]	Set hover\n");
            printf("[8]	Pid test 0.5m\n");
            printf("[9]	Back\n");
            printf("---------------------------------\n");
            scanf("%d",&test_menu);
            system("clear");

            
            if (test_menu == 1){
                int pingprompt=0;
                while (1){
                    
                    printf("---------------------------------\n");
                    printf("Ping Menu\n");
                    printf("---------------------------------\n");
                    printf("[1]	Barometer\n");
                    printf("[2]	Ultrasonic sensor\n");
                    printf("[3]	Magnetometer\n");
                    printf("[4]	GPS\n");
                    printf("[5]	All\n");
                    printf("[6]	Back\n");
                    printf("---------------------------------\n");
                    scanf("%d",&pingprompt);
					system("clear");


                    
                    if(pingprompt==1){
                        commandSensor("baro\0", "ping\0");
                    } else if (pingprompt==2){
                        commandSensor("ultra\0", "ping\0");
                        
                    } else if (pingprompt==3){
                        commandSensor("mag\0", "ping\0");
                        
                    } else if (pingprompt==4){
                        commandSensor("gps\0", "ping\0");
                        
                    } else if (pingprompt==5){
                        commandSensor("baro\0", "ping\0");
                        commandSensor("ultra\0", "ping\0");
                        commandSensor("mag\0", "ping\0");
                        commandSensor("gps\0", "ping\0");
                        
                    } else if (pingprompt==6){
                        break;
                    }else{
                        printf("Invalid\n");
                    }
                }
            }
            else if(test_menu == 2){
                
                int recdataprompt=0;
                while (1){
                    
                    printf("---------------------------------\n");
                    printf("Data Menu\n");
                    printf("---------------------------------\n");
                    printf("[1]	Barometer\n");
                    printf("[2]	Ultrasonic sensor\n");
                    printf("[3]	Magnetometer\n");
                    printf("[4]	GPS\n");
                    printf("[5]	All\n");
                    printf("[6]	Loop all sensors and log data\n");
                    printf("[7]	Loop height fast\n");
                    printf("[8]	Back\n");
                    printf("---------------------------------\n");
                    scanf("%d",&recdataprompt);
                    system("clear");

                    
                    if(recdataprompt==1){
                        printf("Magnetometer heading(deg): %lf\n", getHeading());
             
                    } else if (recdataprompt==4){
                    	 void refreshGPS();
                        printf("lat: %lf long: %lf quality: %d nsat: %d sdn: %lf sde: %lf\n", longitude(),latitude(),quality(),nsat(), sdn(), sde());
                        
                    } else if (recdataprompt==5){
                        printf("Barometer height(m): %lf\n", getBHeight());
                        printf("Ultrasonic height(cm): %lf\n", getUHeight());
                        printf("Magnetometer heading(deg): %lf\n", getHeading());
                        printf("GPS coordinates(lat/long/quality): %lf\n", getBHeight());
                        
                    } else if (recdataprompt==6){
                        double starttime=millis();
                        int refrate=50;
                        while (1){
                            int timec=(millis()-starttime);
                            if(timec>refrate){
                                double uh=getUHeight();
                                double bh=getBHeight();
                                double h=getHeight(uh,bh);
                                double hd=getHeading();
                                void refreshGPS();
                                double lat=latitude();
                                double lon=longitude();
                                double qual=quality();
                                double ns=nsat();
                                double sn=sdn();
                                double se=sde();

                                updateLog(bh,uh,h,hd,lat,lon,qual,ns,sn,se);
                                double samplingspeed=millis()-starttime;
                                //double bheight, double uheight, double height, double heading, double latitude, double longitude, double satquality, double nsat, double sdn, double sde
                                printf("Baro: %5.2lf   Ultra: %5.2lf  H: %5.2lf  Mag: %3.0lf Lat: %7lf Long: %7lf Q: %1.0lf nsat:%2.0lf sdn: %3.2lf sde: %3.2lf ms: %lf\n",bh,uh,h,hd,lat,lon,qual,ns,sn,se,samplingspeed);
                                starttime=millis();
                                delay(40);
                            }
                        }
                        
                    } else if (recdataprompt==7){
                        while(1){
                            printf("H: %lf\n",getHeight(getUHeight(),getBHeight()));
                            delay(100);
                        }
                        
                    }else if (recdataprompt==8){
                        break;
                    
                    }else{
                        printf("Invalid\n");
                    }
                }
            }
            
            else if(test_menu == 3){
                testHoverToStep();
                
            }else if(test_menu == 4){
                testRotation();
                
            }else if(test_menu == 5){
                commandSensor("mag", "calibrate");
                
            }else if(test_menu == 6){
                PIDcal(1.2);
                printf("Test klart");
                
            }else if(test_menu == 7){
                setHover();
                
            }else if(test_menu == 8){
                pidHeightTest(0.3);
            }else{
                printf("Invalid\n");
            }
            
            
        }else if (main_menu == 5){

        	system("pkill HC-SR04");
			system("pkill BMP180");
			system("pkill HMC5883L");
			system("pkill UGPS");
            break;
            
        }
        else{
            printf("Invalid\n");
        }
    }
    printf("Program stopping\n");
    return 1;
}

void linkInputFifo(){
	  char* input_fifo = "/home/pi/tmp/input_fifo";
      input_fd=open(input_fifo, O_RDONLY);
      if (input_fd==-1){ 
          printf("AQ_input_pipe=error: %s\n",strerror(errno));
      } 
      fcntl(input_fd, F_SETFL, O_NONBLOCK); //set non blocking
}
int checkForInput(){
	 char buffer[8];
	 int input=-1;

      if (read(input_fd, buffer, sizeof(buffer))>0){
          input=atoi(buffer);
      }
	return(input);
}
