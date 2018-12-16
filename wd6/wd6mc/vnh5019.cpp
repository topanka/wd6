#include "vnh5019.h"

int vnh5019_initTimers(void)
{
  // Timer 1 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz

  TCCR4A = 0b10101000;
  TCCR4B = 0b00010001;
  ICR4 = 400;
  
  TCCR5A = 0b10101000;
  TCCR5B = 0b00010001;
  ICR5 = 400;
  
  return(0);
}

// Constructors ////////////////////////////////////////////////////////////////

VNH5019MD::VNH5019MD(unsigned char INA, unsigned char INB, unsigned char PWM, unsigned char EN1DIAG, unsigned char EN2DIAG,
                     unsigned char CS, volatile uint16_t *OCR, int REINT, isr_ft isr_f)
{
  //Pin map
  _INA=INA;
  _INB=INB;
  _PWM=PWM;
  _EN1DIAG=EN1DIAG;
  _EN2DIAG=EN2DIAG;
  _CS=CS;
  _OCR=OCR;
  _REINT=REINT;
  _ISR_F=isr_f;
  _speed=0;
  _prev_speed=0;
}

// Public Methods //////////////////////////////////////////////////////////////
void VNH5019MD::init(int ena)
{
// Define pinMode for the pins and set the frequency for timer1.

  pinMode(_INA,OUTPUT);
  pinMode(_INB,OUTPUT);
  pinMode(_PWM,OUTPUT);
//  pinMode(_EN1DIAG,INPUT_PULLUP);
//  pinMode(_EN2DIAG,INPUT_PULLUP);
  if(ena == 0) {
    disable();
  } else {
    enable();
  }
  pinMode(_CS,INPUT);
  attachInterrupt(_REINT,_ISR_F,CHANGE);
}
// Set speed for motor 1, speed is a number betwenn -400 and 400
void VNH5019MD::setSpeed(int speed)
{
  unsigned char reverse=0;

  if(_prev_speed == speed) return;

  if(speed < 0) {
    speed=-speed;  // Make speed a positive quantity
    reverse=1;  // Preserve the direction
  }
  if(speed > 400)  {// Max PWM dutycycle
    speed=400;
  }
  *_OCR = speed;
  if(reverse) {
    digitalWrite(_INA,LOW);
    digitalWrite(_INB,HIGH);
    _speed=-speed;
  } else {
    digitalWrite(_INA,HIGH);
    digitalWrite(_INB,LOW);
    _speed=speed;
  }
  _prev_speed=_speed;
}

void VNH5019MD::incSpeed(int step, int dir)
{
  int speed;

//Serial.print("speed=");
//Serial.println(_speed);

/*
  if(_speed >= 0) {
    if(step >= 0) {
      speed=_speed+step;
    } else {
      if(-step <= _speed) {
        speed=_speed+step;
      } else {
        speed=0;
      }
    }
  } else {
    if(step >= 0) {
      speed=_speed-step;
    } else {
      if(step > _speed) {
        speed=_speed-step;
      } else {
        speed=0;
      }
    }
  }
*/  
  if(_speed >= 0) {
    speed=_speed+step;
    if(speed < 0) speed=0;
  } else {
    speed=_speed-step;
    if(speed > 0) speed=0;
    speed=-speed;
  }
/*  
  if((step > 0) && (speed < 60)) {
    speed=60;
  }
*/  
  if(speed < 60) {
    speed=60;
  }

  if(dir < 0) {
    speed=-speed;
  }
  
  setSpeed(speed);
}

int VNH5019MD::getSpeed(void)
{
  return(_speed);
}

// Brake motor 1, brake is a number between 0 and 400
void VNH5019MD::setBrake(int brake)
{
  // normalize brake
  if(brake < 0) {
    brake=-brake;
  }
  if(brake > 400)  {// Max brake
    brake=400;
  }
  digitalWrite(_INA,LOW);
  digitalWrite(_INB,LOW);
  *_OCR=brake;
}

// Return motor 1 current value in milliamps.
unsigned int VNH5019MD::getCurrentMilliamps()
{
  // 5V / 1024 ADC counts / 144 mV per A = 34 mA per count
  return analogRead(_CS) * 34;
}

// Return error status for half bridge 1 
unsigned char VNH5019MD::getFaultH1()
{
  if(_enabledH1 == 0) return(0xFB);
  return !digitalRead(_EN1DIAG);
}
// Return error status for half bridge 2 
unsigned char VNH5019MD::getFaultH2()
{
  if(_enabledH2 == 0) return(0xFB);
  return !digitalRead(_EN2DIAG);
}
void VNH5019MD::enableH1()
{
  pinMode(_EN1DIAG,INPUT_PULLUP);
  _enabledH1=1;
}
void VNH5019MD::enableH2()
{
  pinMode(_EN2DIAG,INPUT_PULLUP);
  _enabledH2=1;
}
void VNH5019MD::enable()
{
  VNH5019MD::enableH1();
  VNH5019MD::enableH2();
}
void VNH5019MD::disableH1()
{
  pinMode(_EN1DIAG,OUTPUT);
  _enabledH1=0;
}
void VNH5019MD::disableH2()
{
  pinMode(_EN2DIAG,OUTPUT);
  _enabledH2=0;
}
void VNH5019MD::disable()
{
  VNH5019MD::disableH1();
  VNH5019MD::disableH2();
}
