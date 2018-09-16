//cbox
#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2);
unsigned char g_rb=0;
unsigned char g_wb=0;

void setup() {
  // put your setup code here, to run once:
//  Serial.begin(112500);
  Serial2.begin(19200);
  lcd.begin(20, 4);
  lcd.setCursor(0,0);
  lcd.print("wd6 test");
}

void loop() {
  // put your main code here, to run repeatedly:
//Serial.println("aaaaaa\n");
  if(Serial2.available() > 0) {
    g_rb=Serial2.read();
//    Serial.println(g_rb);
    lcd.setCursor(0,1);
    lcd.print(g_rb);
    g_rb+=50;
//    delay(250);
    Serial2.write((byte*)&g_rb,1);
  }

//    g_wb++;
//    Serial2.write((byte*)&g_wb,1);
//    delay(500);

}
