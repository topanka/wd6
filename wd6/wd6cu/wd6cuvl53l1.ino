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

VL53L1X sensor;

void vl53l1_setup(void)
{
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C
  Wire.setSCL(16);
  Wire.setSDA(17);

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }
  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor.startContinuous(500);
}

void vl53l1_read(void)
{
  Serial.print(sensor.read());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
}
