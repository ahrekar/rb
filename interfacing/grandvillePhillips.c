/*


*/


#include "grandvillePhillips.h"


int getConvectron(unsigned int chan, float* returnvalue){
    unsigned int kchan;
    float CVGauge;
    unsigned int rdata;
int status;
/*
    switch (chan){
        case GP_HE_CHAN:  // this is the analog out port on the grandville phillips machine
            kchan = 4; //that is connected to this analog channel on the board
            break;
        case GP_N2_CHAN:
            kchan = 3; //Nitrogen CV pressure
            break;
        default:
            return -1;
    }
    getADC(kchan,&rdata);
*/

	switch (chan)	{
		case GP_HE_CHAN:
			kchan=3;
			break;
		case GP_N2_CHAN:
			kchan=2;
			break;
		case GP_CHAMB_CHAN:
			kchan=1;
			break;
	default:
		kchan=0;

	}
	kchan = kchan + 0x0F0F; // 0x0F0F is the base 'register'
			//(RS485 address of the slave device, which register of the PIC, and the returned data)
	status = read_Modbus_RTU(0xD0,kchan,&rdata);
	if (status==0){
		    CVGauge=(float)rdata;
		    //printf("Chan: %d\tReading:%f\t",kchan,CVGauge);
		    CVGauge = pow(10,(0.00499*CVGauge - 4.05));
		    *returnvalue=CVGauge;
	} else {
		printf("error reading box \n");
		*returnvalue=0;
	}

    return 0;
}

int getIonGauge(float* returnvalue){
    int kchan=0x0F0F; // this is channel 0 of the box.  
/*

*/
    float IonGauge;
    unsigned int rdata;
	int status;


//    getADC(kchan,&rdata);


	status = read_Modbus_RTU(0xD0,kchan,&rdata);
	if (status==0){
	IonGauge = (float)rdata;
	//printf("Chan: %d\tReading:%f\t",kchan,IonGauge);
	
//	IonGauge = pow(10,(0.0107*IonGauge-9.97)); old
	IonGauge = pow(10,(0.00963*IonGauge-9.89));

	*returnvalue=IonGauge;



	} else {
		printf("error reading box \n");
		*returnvalue=0;
	}


    return 0;
}
