#define PUMPFLAG 0xA2
#define PROBEFLAG 0xAA

#define BLOCKED 8
#define UNBLOCKED 0

int setFlag(int device, int pos);
int getFlag(int device);
