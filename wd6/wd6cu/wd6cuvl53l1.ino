typedef struct tagVL53L1XDIST {
  unsigned long t;
  unsigned int d;
} VL53L1XDIST;

VL53L1XDIST g_vl53l1xdist_d[5]={0};
int g_vl53l1xdist_i=0;

#if 0
VL53L1_Dev_t                   dev;
VL53L1_DEV                     Dev = &dev;
int status;

void vl53l1_setup(void)
{
  uint8_t byteData;
  uint16_t wordData;

  Wire.begin();
  Wire.setClock(400000);
  Wire.setSCL(16);
  Wire.setSDA(17);

  Dev->I2cDevAddr = 0x52;

  VL53L1_software_reset(Dev);

  VL53L1_RdByte(Dev, 0x010F, &byteData);
  Serial.print(F("VL53L1X Model_ID: "));
  Serial.println(byteData, HEX);
  VL53L1_RdByte(Dev, 0x0110, &byteData);
  Serial.print(F("VL53L1X Module_Type: "));
  Serial.println(byteData, HEX);
  VL53L1_RdWord(Dev, 0x010F, &wordData);
  Serial.print(F("VL53L1X: "));
  Serial.println(wordData, HEX);

  Serial.println(F("Autonomous Ranging Test"));
  status = VL53L1_WaitDeviceBooted(Dev);
Serial.print("VL53L1_WaitDeviceBooted:");
Serial.println(status);
  status = VL53L1_DataInit(Dev);
Serial.print("VL53L1_DataInit:");
Serial.println(status);
  status = VL53L1_StaticInit(Dev);
Serial.print("VL53L1_StaticInit:");
Serial.println(status);
  status = VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_LONG);
//  status = VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_SHORT);
Serial.print("VL53L1_SetDistanceMode:");
Serial.println(status);
  status = VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 50000);
Serial.print("VL53L1_SetMeasurementTimingBudgetMicroSeconds:");
Serial.println(status);
  status = VL53L1_SetInterMeasurementPeriodMilliSeconds(Dev, 1000); // reduced to 50 ms from 500 ms in ST example
Serial.print("VL53L1_SetInterMeasurementPeriodMilliSeconds:");
Serial.println(status);
  status = VL53L1_StartMeasurement(Dev);
Serial.print("VL53L1_StartMeasurement:");
Serial.println(status);

  if(status) {
    Serial.println(F("VL53L1_StartMeasurement failed"));
    while(1);
  }
}

void vl53l1_read(void)
{
  static VL53L1_RangingMeasurementData_t RangingData;

  status = VL53L1_WaitMeasurementDataReady(Dev);
  if(status == VL53L1_ERROR_NONE) {
    status = VL53L1_GetRangingMeasurementData(Dev,&RangingData);
    if(status == VL53L1_ERROR_NONE) {
//      if(RangingData.RangeStatus == VL53L1_RANGESTATUS_RANGE_VALID) {
        Serial.print(RangingData.RangeStatus);
        Serial.print(F(","));
        Serial.print(RangingData.RangeMilliMeter);
        Serial.print(F(","));
        Serial.print(RangingData.SignalRateRtnMegaCps/65536.0);
        Serial.print(F(","));
        Serial.println(RangingData.AmbientRateRtnMegaCps/65336.0);
//      }
      status = VL53L1_ClearInterruptAndStartMeasurement(Dev);
    }
//    status = VL53L1_ClearInterruptAndStartMeasurement(Dev);
  } else {
    Serial.print(F("error waiting for data ready: "));
    Serial.println(status);
  }
}
#endif

#if 0
VL53L1X vl53l1x;

void vl53l1x_setup(void)
{
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C
  Wire.setSCL(16);
  Wire.setSDA(17);

  vl53l1x.setTimeout(500);
  if (!vl53l1x.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }
  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  vl53l1x.setDistanceMode(VL53L1X::Long);
  vl53l1x.setMeasurementTimingBudget(50000);
//  vl53l1x.setMeasurementTimingBudget(100000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  vl53l1x.startContinuous(50);

  tmr_init(&g_tmr_vl53l1x,50);
  
  Serial.println("vl53l1x ready!");
  
}

int vl53l1x_read(void)
{
//  if(tmr_do(&g_tmr_vl53l1x) != 1) return(0);

  if(!vl53l1x.dataReady()) return(0);

  g_vl53l1xdist_d[g_vl53l1xdist_i].t=g_millis;
  g_vl53l1xdist_d[g_vl53l1xdist_i].d=vl53l1x.read(false);

  Serial.print(vl53l1x.rangeStatusToString(vl53l1x.ranging_data.range_status));
  Serial.print(" ");
  Serial.print(g_vl53l1xdist_d[g_vl53l1xdist_i].d);
//  if (vl53l1x.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
  g_vl53l1xdist_i++;
  g_vl53l1xdist_i%=(sizeof(g_vl53l1xdist_d)/sizeof(g_vl53l1xdist_d[0]));
  
  return(1);
}
#endif


VL53L1X vl53l1x(&Wire);

int vl53l1x_calibration(void)
{
  VL53L1X_ERROR err;
  int16_t offset;
  uint16_t xtalk;
  
  err=vl53l1x.VL53L1X_CalibrateOffset(140,&offset);
  if(err != 0) return(-1);
  Serial.print("Offset1: ");
  Serial.println(offset);
  err=vl53l1x.VL53L1X_GetOffset(&offset);
  Serial.print("Offset2: ");
  Serial.println(offset);
  
  err=vl53l1x.VL53L1X_CalibrateXtalk(140,&xtalk);
  if(err != 0) return(-1);
  Serial.print("Xtalk1: ");
  Serial.println(offset);
  err=vl53l1x.VL53L1X_GetXtalk(&xtalk);
  Serial.print("Xtalk2: ");
  Serial.println(xtalk);

  err=vl53l1x.VL53L1X_StartTemperatureUpdate();
  
  return(0);
}

void vl53l1x_setup(void)
{
  uint8_t state;
  VL53L1X_ERROR err;
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
    if((err == 0) && (state == 0)) break;
    if((millis()-t0) > 1000) break;
    delay(10);
  }

  if(state == 0) {
    Serial.println("vl53l1x ready!");
  } else {
    Serial.println("vl53l1x boot failure!");
  }
  tmr_init(&g_tmr_vl53l1x,50);
  
  err=vl53l1x.VL53L1X_SensorInit();
  
//vl53l1x_calibration();

//1 short 1300 mm, 2 long 4000 mm
  err=vl53l1x.VL53L1X_SetDistanceMode(2);
  err=vl53l1x.VL53L1X_SetInterMeasurementInMs(50);
  err=vl53l1x.VL53L1X_SetTimingBudgetInMs(50);
  
//  err=vl53l1x.VL53L1X_SetOffset(g_vl53l1x_offset);
//  err=vl53l1x.VL53L1X_SetXtalk(g_vl53l1x_xtalk);

  err=vl53l1x.VL53L1X_GetOffset(&offset);
  Serial.print("Offset: ");
  Serial.println(offset);
  err=vl53l1x.VL53L1X_GetXtalk(&xtalk);
  Serial.print("Xtalk: ");
  Serial.println(xtalk);

  
  err=vl53l1x.VL53L1X_StartRanging();

  Serial.println("vl53l1x ready!");
  
}

int vl53l1x_read(void)
{
  VL53L1X_ERROR err;
  uint8_t dr=0,rs=0;
  uint16_t dist;
  
  if(tmr_do(&g_tmr_vl53l1x) != 1) return(0);

  err=vl53l1x.VL53L1X_CheckForDataReady(&dr);
  if(err != 0) return(0);
  if(dr != 1) return(0);
  err=vl53l1x.VL53L1X_GetRangeStatus(&rs);
  err=vl53l1x.VL53L1X_GetDistance(&dist);
  err=vl53l1x.VL53L1X_ClearInterrupt();

  g_vl53l1xdist_d[g_vl53l1xdist_i].t=g_millis;
  g_vl53l1xdist_d[g_vl53l1xdist_i].d=dist;

//  Serial.print(vl53l1x.rangeStatusToString(vl53l1x.ranging_data.range_status));
  Serial.print("range status: ");
  Serial.print(rs);
  Serial.print(", dist: ");
  Serial.print(g_vl53l1xdist_d[g_vl53l1xdist_i].d);
//  if (vl53l1x.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
  g_vl53l1xdist_i++;
  g_vl53l1xdist_i%=(sizeof(g_vl53l1xdist_d)/sizeof(g_vl53l1xdist_d[0]));
  
  return(1);
}
