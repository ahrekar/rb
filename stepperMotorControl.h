#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

// Motor 0 -> Polarimeter
// Motor 1 -> Absorption Analyzer
// Motor 2 -> QWP
void setupMotorVariables(int motor);

void homeMotor(int motor);
void moveMotor(int motor,int dir, int steps);
void _moveMotor(int motor,int dir, int steps);
void setMotor(int motor, int location);

void stepMotor(int dir, int steps);
void writeMotorPosition();
