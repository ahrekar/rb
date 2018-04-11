/*


*/


#include "grandvillePhillips.h"


int getConvectron(unsigned int chan, float* returnvalue){
    int kchan;
    float CVGauge;
    unsigned int rdata;

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
    CVGauge=(float)rdata;
    printf("Chan: %d\tReading:%f\n",kchan,CVGauge);
    CVGauge = pow(10,(0.00499*CVGauge - 4.05));
    *returnvalue=CVGauge;
    return 0;
}

int getIonGauge(float* returnvalue){
    int kchan=1;
    float IonGauge;
    unsigned int rdata;

    getADC(kchan,&rdata);
    IonGauge = (float)rdata;
    printf("Chan: %d\tReading:%f\n",kchan,IonGauge);
    IonGauge = pow(10,(0.0107*IonGauge-9.97));
    *returnvalue=IonGauge;

    return 0;
}
