//wd6 teensy

const int ledPin=13;
unsigned long g_sendtmo=0;
unsigned long g_millis=0;
unsigned char g_wb=0;
unsigned char g_rb=0;

void setup() {
  pinMode(ledPin,OUTPUT);
  Serial.begin(115200);     //usb
  Serial1.begin(500000);    //wd6 motor controller
  Serial3.begin(19200);     //wd6 RF
  delay(1000);
}

void loop() {
  g_millis=millis();

  if(g_sendtmo == 0) {
    Serial.println("Teensy serial test started.");
  }
  if(g_millis > (g_sendtmo+500)) {
    Serial1.write((byte*)&g_wb,1);
    g_wb=(g_wb+1)%10;
    g_sendtmo=g_millis;
    if(g_wb%2 == 0) {
      digitalWrite(ledPin,HIGH);
    } else {
      digitalWrite(ledPin,LOW);
    }
  }
/*  
  if(Serial1.available() > 0) {
    g_rb=(unsigned char)Serial1.read();
    Serial.println(g_rb);
  }
*/  
  if(Serial1.available() > 0) {
    g_rb=(unsigned char)Serial1.read();
    Serial.print("md: ");
    Serial.println(g_rb);
    Serial3.write((byte*)&g_rb,1);
  }
  
  if(Serial3.available() > 0) {
    g_rb=(unsigned char)Serial3.read();
    Serial.print("RF: ");
    Serial.println(g_rb);
  }
  
}

