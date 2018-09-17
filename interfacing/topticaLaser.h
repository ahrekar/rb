int initializeLaser();
int setParameter(int sock, char* parameter, char* setValue);
int readParameter(int sock, char* parameter, char* returnValue);
int setScanOffset(int sock, float offset);
int setMasterCurrent(int sock, float current);
int setMasterTemperature(int sock, float temperature);
int setAmpCurrent(int sock, float current);

int turnOffLaser(int sock);
int turnOffDiodeLaser(int sock);
int turnOffAmplifier(int sock);
int turnOnLaser(int sock);
int turnOnDiodeLaser(int sock);
int turnOnAmplifier(int sock);
