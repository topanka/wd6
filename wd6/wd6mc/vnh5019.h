#ifndef VNH5019MotorShield_h
#define VNH5019MotorShield_h

#include <Arduino.h>

typedef void (*isr_ft)(void);

/*
typedef struct tagMYAVRTIMER {
  volatile uint8_t *tccr;
  volatile uint16_t *icr;
  volatile uint16_t *ocr;
} MYAVRTIMER;
*/

class VNH5019MD
{
  public:  
    // CONSTRUCTORS
    VNH5019MD(unsigned char INA, unsigned char INB, unsigned char PWM, unsigned char EN1DIAG, unsigned char EN2DIAG,
              unsigned char CS, volatile uint16_t *OCR, int REINT, isr_ft isr_f); // User-defined pin selection. 
    
    // PUBLIC METHODS
    void init(); // Initialize TIMER, set the PWM to 20kHZ. 
    void setSpeed(int speed); // Set speed.
    void incSpeed(int step, int dir); // Increment speed.
    int getSpeed(void); // Get speed.
    void setBrake(int brake); // Brake. 
    unsigned int getCurrentMilliamps(); // Get current reading. 
    unsigned char getFaultH1(); // Get fault reading.
    unsigned char getFaultH2(); // Get fault reading.
    
  private:
    unsigned char _INA;
    unsigned char _INB;
    unsigned char _PWM;
    unsigned char _EN1DIAG;
    unsigned char _EN2DIAG;
    unsigned char _CS;
    volatile uint16_t *_OCR;
    int _REINT;
    isr_ft _ISR_F;
    int _speed;
};

int vnh5019_initTimers(void);

#endif
