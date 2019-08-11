#define VL53L1X_RANGE_SHORT     1
#define VL53L1X_RANGE_LONG      2

typedef struct tagVL53L1XDIST {
  unsigned long t;
  unsigned int d;
} VL53L1XDIST;

VL53L1XDIST g_vl53l1xdist_d[5]={0};
int g_vl53l1xdist_i=0;
int16_t g_vl53l1x_offset=24;
uint16_t g_vl53l1x_xtalk=0;
uint8_t g_vl53l1x_rangemode=VL53L1X_RANGE_LONG;
volatile uint8_t g_vl53l1x_dataready=0;

#define VL53L1X_BOOT_READY     3

VL53L1X vl53l1x(&Wire);

void ISR_vl53l1x_dataready(void)
{
  g_vl53l1x_dataready=1;
}

int vl53l1x_calibration(void)
{
  VL53L1X_ERROR err;
  int16_t offset;
  uint16_t xtalk;
  
  Serial.println("Waiting for servo ...");
  delay(2000);
  Serial.println("Calibrating offset ...");
  err=vl53l1x.VL53L1X_CalibrateOffset(140,&offset);
  if(err != 0) return(-1);
  Serial.print("Offset1: ");
  Serial.println(offset);
  err=vl53l1x.VL53L1X_GetOffset(&offset);
  Serial.print("Offset2: ");
  Serial.println(offset);
  
  Serial.println("Calibrating xtalk ...");
  err=vl53l1x.VL53L1X_CalibrateXtalk(140,&xtalk);
  if(err != 0) return(-1);
  Serial.print("Xtalk1: ");
  Serial.println(xtalk);
  err=vl53l1x.VL53L1X_GetXtalk(&xtalk);
  Serial.print("Xtalk2: ");
  Serial.println(xtalk);

  err=vl53l1x.VL53L1X_StartTemperatureUpdate();
  
  return(0);
}

// convert a RangeStatus to a readable string
// Note that on an AVR, these strings are stored in RAM (dynamic memory), which
// makes working with them easier but uses up 200+ bytes of RAM (many AVR-based
// Arduinos only have about 2000 bytes of RAM). You can avoid this memory usage
// if you do not call this function in your sketch.
const char *vl53l1x_rangeStatusToString(uint8_t status)
{
  switch (status)
  {
    case 0:
      return "range valid";
    case 1:
      return "sigma fail";
    case 2:
      return "signal fail";
    case 4:
      return "out of bounds";
     case 7:
      return "wraparound";
    default:
      return "unknown status";
  }
}

int vl53l1x_setrange(uint16_t mode, uint32_t im, uint16_t tb)
{
  VL53L1X_ERROR err;

  if(im < tb) return(-1);
  err=vl53l1x.VL53L1X_SetDistanceMode(mode);
  if(err != 0) return(-1);
  err=vl53l1x.VL53L1X_SetInterMeasurementInMs(im);
  if(err != 0) return(-1);
  err=vl53l1x.VL53L1X_SetTimingBudgetInMs(tb);
  if(err != 0) return(-1);

  return(0);
}
int vl53l1x_shortrange(uint32_t im, uint16_t tb)
{
  if(vl53l1x_setrange(1,im,tb) != 0) return(-1);

  return(0);
}
int vl53l1x_longrange(uint32_t im, uint16_t tb)
{
  if(vl53l1x_setrange(2,im,tb) != 0) return(-1);
  
  return(0);
}

void showroi(void)
{
  VL53L1X_ERROR err;
  uint16_t x,y;

  err=vl53l1x.VL53L1X_GetROI_XY(&x,&y);
  if(err != 0) return;

  Serial.print("ROI.x=");
  Serial.println(x);
  Serial.print("ROI.y=");
  Serial.println(y);
  
}
  
void vl53l1x_setup(void)
{
  uint8_t state;
  VL53L1X_ERROR err;
  VL53L1X_Version_t ver={0};
  uint16_t id=0;
  unsigned long t0;
  uint16_t xtalk;
  int16_t offset;
  
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C
  Wire.setSCL(16);
  Wire.setSDA(17);

  t0=millis();
  for(;;) {
    err=vl53l1x.VL53L1X_BootState(&state);
    if((err == 0) && (state == VL53L1X_BOOT_READY)) break;
    if((millis()-t0) > 1000) break;
    delay(10);
  }

  if(state == VL53L1X_BOOT_READY) {
    Serial.println("vl53l1x ready!");
  } else {
    Serial.print(err);
    Serial.print(" ");
    Serial.print(state);
    Serial.print(" ");
    Serial.println("vl53l1x boot failure!");
  }
  tmr_init(&g_tmr_vl53l1x,1000);
  
  err=vl53l1x.VL53L1X_GetSWVersion(&ver);
  Serial.print("SW version: 0x");
  Serial.print(ver.major,HEX);
  Serial.print(" 0x");
  Serial.print(ver.minor,HEX);
  Serial.print(" 0x");
  Serial.print(ver.build,HEX);
  Serial.print(" 0x");
  Serial.println(ver.revision,HEX);

  err=vl53l1x.VL53L1X_GetSensorId(&id);
  Serial.print("ID: 0x");
  Serial.println(id,HEX);
  
  err=vl53l1x.VL53L1X_SensorInit();
  
//vl53l1x_calibration();

/*
//1 short 1300 mm, 2 long 4000 mm
  err=vl53l1x.VL53L1X_SetDistanceMode(2);
  err=vl53l1x.VL53L1X_SetInterMeasurementInMs(50);
  err=vl53l1x.VL53L1X_SetTimingBudgetInMs(50);
*/
  pinMode(24,INPUT_PULLUP);
  attachInterrupt(24,ISR_vl53l1x_dataready,RISING);
  
  vl53l1x_setrange(g_vl53l1x_rangemode,50,50);
  
  err=vl53l1x.VL53L1X_SetOffset(g_vl53l1x_offset);
  err=vl53l1x.VL53L1X_SetXtalk(g_vl53l1x_xtalk);

  err=vl53l1x.VL53L1X_GetOffset(&offset);
  Serial.print("Offset: ");
  Serial.println(offset);
  err=vl53l1x.VL53L1X_GetXtalk(&xtalk);
  Serial.print("Xtalk: ");
  Serial.println(xtalk);

  showroi();
  vl53l1x.VL53L1X_SetROI(16,16);
  showroi();
  
   
  err=vl53l1x.VL53L1X_StartRanging();

  Serial.println("vl53l1x setup OK!");
  
}

int vl53l1x_read(void)
{
  VL53L1X_ERROR err;
  uint8_t rs=0;
  uint16_t dist;
  
//  if(tmr_do(&g_tmr_vl53l1x) != 1) return(0);

/*
  err=vl53l1x.VL53L1X_CheckForDataReady(&dr);
  if(err != 0) return(0);
  if(dr != 1) return(0);
*/

  if(g_vl53l1x_dataready != 1) return(0);
    
  err=vl53l1x.VL53L1X_GetRangeStatus(&rs);
  if(err != 0) {
    Serial.println("rangestatuserror");
    return(0);
  }
  err=vl53l1x.VL53L1X_GetDistance(&dist);
  if(err != 0) {
    Serial.println("getdistanceerror");
    return(0);
  }
  vl53l1x.VL53L1X_ClearInterrupt();
  g_vl53l1x_dataready=0;

  g_vl53l1xdist_d[g_vl53l1xdist_i].t=g_millis;
  g_vl53l1xdist_d[g_vl53l1xdist_i].d=dist;
  g_vl53l1xdist_i++;
  g_vl53l1xdist_i%=(sizeof(g_vl53l1xdist_d)/sizeof(g_vl53l1xdist_d[0]));
  g_wd6_vl53l1x_dist=dist;

  if(tmr_do(&g_tmr_vl53l1x) != 1) return(0);

/*  
  Serial.print(vl53l1x_rangeStatusToString(rs));
  Serial.print(": ");
  Serial.print(dist);
  Serial.println();
*/  

  if((dist < 1000) && (g_vl53l1x_rangemode != VL53L1X_RANGE_SHORT)) {
    Serial.println("setting short range");
    vl53l1x_shortrange(50,50);
    g_vl53l1x_rangemode=VL53L1X_RANGE_SHORT;
  } else if((dist > 1300) && (g_vl53l1x_rangemode != VL53L1X_RANGE_LONG)) {
    Serial.println("setting long range");
    vl53l1x_longrange(100,100);
    g_vl53l1x_rangemode=VL53L1X_RANGE_LONG;
  }
  
  return(1);
}
