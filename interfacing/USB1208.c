/*
 *
*/

#include "USB1208.h"

void delayMicrosecondsHard(unsigned int howLong);

//global variables for USB1208 but not for public
hid_device*  hid = 0x0;
int ret;

int closeUSB1208(){
    //cleanly close USB
    hid_close(hid);
    hid_exit();
    return 0;
}

int initializeUSB1208(){
    ret = hid_init();
    if (ret < 0) {
        fprintf(stderr, "hid_init failed with return code %d\n", ret);
        return -1;
    }

    if ((hid = hid_open(MCC_VID, USB1208LS_PID, NULL)) >  0) {
        //printf("USB-1208LS Device is found!\n");
    } else {
        fprintf(stderr, "USB-1208LS not found.\n");
        exit(1);
    }

    return 0;
}

int getUSB1208AnalogIn(unsigned short chan, float* returnvalue){
    signed short svalue;
    int gain;
    if(chan==PROBE_LASER){
        gain = BP_5_00V;
        //gain = BP_2_00V;
    }else{
        gain = BP_2_50V;
    }


    svalue = usbAIn_USB1208LS(hid,chan,gain);  //channel = 0 for k617

    *returnvalue = volts_LS(gain,svalue);
    return 0;
}

//dwell is time in tenth's of seconds.  So, if want to count for 1 sec, pass '10'
int getUSB1208Counter(unsigned short dwell, long int * returncounts){
    int counts,i;
    counts=0;
    for (i=0;i<dwell;i++){ 
        /* the for loop is used instead of putting dwell in the delayMicroseconds.
           In high count rates (not likely for us, but could be) the USB counter could 'roll over'
           during a very long dwell time.  So, by doing smaller chunks and adding, less 
           chance of roll over. 
           */
        usbInitCounter_USB1208LS(hid);
        delayMicrosecondsHard(100000); // wiringPi
        counts+=usbReadCounter_USB1208LS(hid);
    }
    *returncounts=counts;
    return 0;
}

int setUSB1208AnalogOut(unsigned short chan, unsigned int outValue){
    // out value ranges from 0 to 1024
    outValue = outValue & 0x3FF;
    // this caps at 1023. 10 bits.
    usbAOut_USB1208LS(hid, chan, outValue);
    
    return 0;
}
