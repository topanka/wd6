unsigned long g_pmw3901_doit_tmo=100;
int16_t g_deltaX=0,g_deltaY=0;
int32_t g_pmw3901_sumX=0;
int32_t g_pmw3901_sumY=0;
int g_pmw3901_rotinprogress=0;

Bitcraze_PMW3901 pmw3901(WD6CU_SPI_CS_PIN); // Instantiate PMW3901

int pmw3901_setup()
{
  uint8_t product_ID;
  uint8_t revision_ID;
  uint8_t inverse_product_ID;

Serial.println("pmw3901 setup started");
  
  SPI.beginTransaction(SPISettings(2000000,MSBFIRST,SPI_MODE0));
  digitalWrite(WD6CU_SPI_CS_PIN,LOW);
  SPI.transfer(0x00);
  product_ID=SPI.transfer(0);
  digitalWrite(WD6CU_SPI_CS_PIN,HIGH);
  SPI.endTransaction();
  Serial.print("Product ID = 0x");
  Serial.print(product_ID,HEX);
  Serial.println(" should be 0x49");
  if(product_ID != 0x49) {
    Serial.println("Initialization of the flow sensor failed1");
    return(-1);
  }

  SPI.beginTransaction(SPISettings(2000000,MSBFIRST,SPI_MODE0));
  digitalWrite(WD6CU_SPI_CS_PIN,LOW);
  SPI.transfer(0x01);
  revision_ID=SPI.transfer(0);
  digitalWrite(WD6CU_SPI_CS_PIN,HIGH);
  SPI.endTransaction();
  Serial.print("Revision ID = 0x");
  Serial.println(revision_ID, HEX); 

  SPI.beginTransaction(SPISettings(2000000,MSBFIRST,SPI_MODE0));
  digitalWrite(WD6CU_SPI_CS_PIN,LOW);
  SPI.transfer(0x5F);
  inverse_product_ID=SPI.transfer(0);
  digitalWrite(WD6CU_SPI_CS_PIN,HIGH);
  SPI.endTransaction();
  Serial.print("Inverse Product ID = 0x");
  Serial.print(inverse_product_ID,HEX);
  Serial.println(" should be 0xB6"); 
  if(inverse_product_ID != 0xB6) {
    Serial.println("Initialization of the flow sensor failed2");
    return(-1);
  }

  if(!pmw3901.begin()) {
    Serial.println("Initialization of the flow sensor failed3");
    return(-1);
  }

  tmr_init(&g_tmr_pmw3901,g_pmw3901_doit_tmo);
  
Serial.println("pmw3901 setup completed");
  /* end of setup */
  return(0);
}

int pmw3901_read(void)
{
  if(tmr_do(&g_tmr_pmw3901) != 1) return(0);
 
  pmw3901.readMotionCount(&g_deltaX,&g_deltaY);

    Serial.print("X: ");
    Serial.print(g_deltaX);
    Serial.print(", Y: ");
    Serial.print(g_deltaY);
    Serial.print("\n");
 
  return(0);
}

int pmw3901_rot_start(void)
{
  g_pmw3901_sumX=0;
  g_pmw3901_sumY=0;
  g_pmw3901_rotinprogress=1;

  return(0);
}

int pmw3901_rot_stop(int angle)
{
  if(g_pmw3901_rotinprogress != 1) return(0);
  
  g_pmw3901_sumX+=g_deltaX;
  g_pmw3901_sumY+=g_deltaY;

  if(g_pmw3901_sumX >= angle) {
    g_pmw3901_rotinprogress=0;
  }

  return(1);
}
