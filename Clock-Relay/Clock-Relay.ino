// include the library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <ArduinoWiFi.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <RTClib.h>



/*
on your borwser, you type http://<IP>/arduino/webserver/ or http://<hostname>.local/arduino/webserver/
*/
   
RTC_DS1307 RTC;

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define GREEN 0x2
#define BLUE 0x4
#define WHITE 0x7

// set the pin for the relay
const int RelayPin = 3;

int StartHour = 22;
int StopHour = 7;
 
void setup() {
  // put your setup code here, to run once:

    Serial.begin(57600);
    Wire.begin();
    RTC.begin();
 
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);

  // Print a message to the LCD. We track how long it takes since
  // this library has been optimized a bit and we're proud of it :)
  int time = millis();
  lcd.print("Hello, world!");
  time = millis() - time;
  Serial.print("Took "); Serial.print(time); Serial.println(" ms");
  lcd.setBacklight(WHITE);

  //setup relaypin

  pinMode(RelayPin,OUTPUT);
  digitalWrite(RelayPin, HIGH);
  
  Wifi.begin();

    lcd.clear();
    lcd.print("Server is up");
    delay(1000);
    

    
}

uint8_t i=0;
void loop() {
  
  DateTime now = RTC.now();
  
    // put your main code here, to run repeatedly:
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
      digitalClockDisplay();
    

  uint8_t buttons = lcd.readButtons();
unsigned long currentMillis = millis();
  if (StartHour <= now.hour() | now.hour() < StopHour){
    digitalWrite(RelayPin, LOW);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Relay UIT");
      lcd.setBacklight(RED);
      digitalClockDisplay();
  }
  if (buttons) {
    //lcd.clear();
    //lcd.setCursor(0,0);
    if (buttons & BUTTON_UP) {
      digitalWrite(RelayPin, HIGH);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Relay AAN");
      lcd.setBacklight(GREEN);
      digitalClockDisplay();
      delay(1000);
      lcd.setBacklight(WHITE);
    }
    if (buttons & BUTTON_DOWN) {
      digitalWrite(RelayPin, LOW);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Relay UIT");
      lcd.setBacklight(RED);
      digitalClockDisplay();
      delay(1000);
      lcd.setBacklight(WHITE);
    }

  }
      while(Wifi.available()){
      process(Wifi);
    }
  delay(50);

  
}

void process(WifiData client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "digital" command?
  if (command == "webserver") {
    WebServer(client);
  }
  
  if (command == "digital") {
    digitalCommand(client);
  }
}

void WebServer(WifiData client) {
  
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html>");
          
          client.println("<head> </head>");
          client.print("<body><center>");
                    
          client.print("Click &nbsp<input type=button onClick=\"var w=window.open('/arduino/digital/");
          client.print(RelayPin);
          client.print("/1','_parent');w.close();\"value='ON' style=\"width: 200px; height: 100px;\">&nbsp switch ON<br>");
          client.print("Click &nbsp<input type=button onClick=\"var w=window.open('/arduino/digital/");
          client.print(RelayPin);
          client.print("/0','_parent');w.close();\"value='OFF' style=\"width: 200px; height: 100px;\">&nbsp switch OFF<br>");
          
          client.print("</center></body>");
          client.println("</html>");
          client.print(DELIMITER); // very important to end the communication !!! 
 
}

void digitalCommand(WifiData client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  }
  
  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.print(value);
  client.print(EOL);
  lcd.setCursor(0, 0);
  lcd.clear();
  if (value == 0) {
    lcd.setBacklight(RED);
    lcd.print("relay UIT");
    digitalClockDisplay();
  } 
  else { lcd.setBacklight(GREEN);
  lcd.print("relay AAN");
  digitalClockDisplay();
  }
  delay(1000);
  lcd.setBacklight(WHITE);
}

void digitalClockDisplay(){
    DateTime now = RTC.now();  
    lcd.setCursor(0, 1);
    lcd.print(now.day(), DEC);    
    lcd.print('-');
    lcd.print(now.month(), DEC);
    lcd.print(' ');
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
}


