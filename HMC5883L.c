#include "HMC588L.h"
#include <libconfig.h>


int main(){
    cfg_init();
    HMC5883L_init();
    printOut();
    connectFifos();
    sample();
}

void HMC5883L_init(){
    wiringPiSetupSys();
    HMC5883L_Sensor =  wiringPiI2CSetup (HMC5883L_Address);
    wiringPiI2CWriteReg8(HMC5883L_Sensor, ModeRegister_Address, ContinuosMeasurementMode);
}

void cfg_init(){
    config_t cfg;
    config_setting_t *hmc588l, *root;
    const char *str;
    config_init(&cfg);
    
    if(! config_read_file(&cfg, "c.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        //return(EXIT_FAILURE);
    }
    root = config_root_setting(&cfg);
    hmc588l = config_lookup(&cfg, "HMC588L");
    
    config_setting_lookup_bool(hmc588l, "Debug", &debug);
    config_setting_lookup_int(hmc588l, "refreshrate", &refreshrate);
    config_setting_lookup_int(hmc588l, "offset_x", &offset_x);
    config_setting_lookup_int(hmc588l, "offset_y", &offset_y);
    config_setting_lookup_int(hmc588l, "offset_z", &offset_z);
    
    config_destroy(&cfg);
    
}

void printOut(){
    if(debug){
        printf("\n----HMC588L Configuration data---\n");
        printf("offset_x: %d\n", offset_x);
        printf("offset_y: %d\n", offset_y);
        printf("offset_z: %d\n", offset_z);
        printf("\n");
        printf("refreshrate: %d\n", refreshrate);
        printf("---------------------------------\n\n");
    }
}


void connectFifos(){
    char* from_mag_fifo = "/home/pi/tmp/from_mag_fifo";
    char* to_mag_fifo = "/home/pi/tmp/to_mag_fifo";
    //delete in case it already exists
    unlink(from_mag_fifo);
    delay(200);
    if (mkfifo(from_mag_fifo,0666)==-1){
        printf("m_make_from_mag=error: %s\n",strerror(errno));
    }
    from_mag_fd=open(from_mag_fifo, O_WRONLY);
    if (from_mag_fd==-1){
        printf("pipe_from_mag=error: %s\n",strerror(errno));
    } else {
        printf("pipe_from_mag=connected\n");
    }
    
    to_mag_fd=open(to_mag_fifo, O_RDONLY);
    if (to_mag_fd==-1){
        printf("pipe_to_mag=error: %s\n",strerror(errno));
    } else {
        // printf("pipe_to_mag=connected\n");
    }
    
    to_mag_fd=open(to_mag_fifo, O_RDONLY);
      if (to_mag_fd==-1){
        printf("pipe_to_mag=error: %s\n",strerror(errno));
      } else {
     // printf("pipe_to_mag=connected\n");
    }

 }
 void writeOutput(){
   char WriteBuf[128];
   sprintf(WriteBuf,"%lf",heading);
   write(from_mag_fd,WriteBuf,sizeof(WriteBuf));
 }
 void checkPipe(){
  char buffer[10];

   char str1[10];
   char str2[10];
   char str3[10];

   strcpy(str1,"ping");
   strcpy(str2, "read");
   strcpy(str3,"calibrate");
       char strlast[10];
   while (read(to_mag_fd, strlast, 10)>0){
        buffer=strlast;
   }

     if (strcmp(buffer,str1)==0){
       printf("Magnetometer says hi! :)\n");
       } else if(strcmp(buffer,str2)==0){
         writeOutput();
         } else if(strcmp(buffer,str3)==0){
           calibrate();
         }
     }

int GetX(){
    uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,XData_MSB);
    uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,XData_LSB);
    short x = (short)((MSB << 8) | LSB);
    return x;
}

int GetY(){
    uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,YData_MSB);
    uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,YData_LSB);
    
    short y = (short)((MSB << 8) | LSB);
    return y;
}

int GetZ(){
    uint8_t MSB = wiringPiI2CReadReg8(HMC5883L_Sensor,ZData_MSB);
    uint8_t LSB = wiringPiI2CReadReg8(HMC5883L_Sensor,ZData_LSB);
    
    short z = (short)((MSB << 8) | LSB);
    return z;
}

double computeHeading(int x, int y, int z){
    double hd;
    
    x=x-offset_x;
    y=y-offset_y;
    z=z-offset_z;
    
    if (y>0){
        hd=90-atan((double)x/(double)y)*(180/PI);
    } else if (y<0){
        hd=270-atan((double)x/(double)y)*(180/PI);
    } else if(y==0&&x<0){
        hd=180.0;
    } else if(y==0&&x>0){
        hd=0.0;
    }
    //printf("Heading: %lf\n",hd);
    return(hd);
}

void sample(){
    int x;
    int y;
    int z;
    while(sampling==1){
        checkPipe();
        x = GetX();
        y = GetY();
        z = GetZ();
        heading=computeHeading(x,y,z);
        // printf("Heading: %lf\n", heading);
        delay(20);
    }
}

void calibrate(){
    int calsamp=10000;
    int xvalues[calsamp];
    int yvalues[calsamp];
    int zvalues[calsamp];
    
    int max_x=0;
    int max_y=0;
    int max_z=0;
    int min_x=0;
    int min_y=0;
    int min_z=0;
    
    printf("Move magnetometer around in circles! All axes\n");
    for (int i=0; i<calsamp;++i){
        xvalues[i]=GetX();
        yvalues[i]=GetY();
        zvalues[i]=GetZ();
        
        
        if (xvalues[i]>max_x){
            max_x=xvalues[i];
        }
        if (yvalues[i]>max_y){
            max_y=yvalues[i];
        }
        if (zvalues[i]>max_z){
            max_z=zvalues[i];
        }
        if (xvalues[i]<min_x){
            min_x=xvalues[i];
        }
        if (yvalues[i]<min_y){
            min_y=yvalues[i];
        }
        if (zvalues[i]<min_z){
            min_z=zvalues[i];
        }
        
        if (i%(calsamp/10)==0){
            printf("Calibration %d%% done\n", i*100/calsamp);
        }
        if ((i%calsamp/100)==0){
            printf("X: %d Y: %d Z: %d current x:%d %d y:%d %d z:%d %d\n",xvalues[i],yvalues[i],zvalues[i],max_x,min_x,max_y,min_y,max_z,min_z);
        }
    }
    printf("Calibration 100%% done\n");
    offset_x=(min_x+max_x)/2;
    offset_y=(min_y+max_y)/2;
    offset_z=(min_z+max_z)/2;
    printf("x: %d %d\ny: %d %d\n z: %d %d\n",min_x, max_x, min_y, max_y, min_z, max_z);
    printf("Offsets:\nx=%d\ny=%d\nz=%d\nMagnetometer hard-iron compensated!\n", offset_x,offset_y,offset_z);
    
    config_t cfg;
    config_setting_t *hmc588l, *x, *y, *z;
    config_init(&cfg);
    
    if(! config_read_file(&cfg, "c.cfg")){
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        //return(EXIT_FAILURE);
    }

    x = config_lookup(&cfg, "HMC588L.offset_x");
    y = config_lookup(&cfg, "HMC588L.offset_y");
    z = config_lookup(&cfg, "HMC588L.offset_z");
    
    config_setting_set_int(x, offset_x);
    config_setting_set_int(y, offset_y);
    config_setting_set_int(z, offset_z);
    config_write_file(&cfg, "c.cfg");
    
    
    config_destroy(&cfg);
}
