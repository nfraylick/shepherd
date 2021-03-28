#include <Servo.h>

SoftwareSerial wifiSerial(2, 3);

Servo cutServo, clampServo1, clampServo2;

bool DEBUG = true;   //show more logs
int responseTime = 10;

int pos = 0;

const int buttonPin = 2;
const int ledPin =  13;     

int buttonState = 0;
int count = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  wifiSerial.begin(115200);
  while (!wifiSerial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  sendToWifi("AT+CWMODE=2",responseTime,DEBUG); // configure as access point
  sendToWifi("AT+CIFSR",responseTime,DEBUG); // get ip address
  sendToWifi("AT+CIPMUX=1",responseTime,DEBUG); // configure for multiple connections
  sendToWifi("AT+CIPSERVER=1,80",responseTime,DEBUG); // turn on server on port 80
 
  sendToUno("Wifi connection is running!",responseTime,DEBUG);
  cutServo.attach(3);
  cutServo.write(0);
  clampServo1.attach(5);
  clampServo1.write(0);  
  clampServo2.attach(6);
  clampServo2.write(0);
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);  
  pinMode(ledPin, OUTPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {
    count++;
    delay(300);
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  if(count == 1){
    cutServo.write(180);
  }

  if(count == 2){
    clampServo1.write(180);              
    clampServo2.write(180);
  }
   if(Serial.available()>0){
     String message = readSerialMessage();
    if(find(message,"debugEsp8266:")){
      String result = sendToWifi(message.substring(13,message.length()),responseTime,DEBUG);
      if(find(result,"OK"))
        sendData("\nOK");
      else
        sendData("\nEr");
    }
  }
  if(wifiSerial.available()>0){
    
    String message = readWifiSerialMessage();
    
    if(find(message,"esp8266:")){
       String result = sendToWifi(message.substring(8,message.length()),responseTime,DEBUG);
      if(find(result,"OK"))
        sendData("\n"+result);
      else
        sendData("\nErrRead");               //At command ERROR CODE for Failed Executing statement
    }else
    if(find(message,"HELLO")){  //receives HELLO from wifi
        sendData("\\nHI!");    //arduino says HI
    }else if(find(message,"LEDON")){
      //turn on built in LED:
      digitalWrite(13,HIGH);
    }else if(find(message,"LEDOFF")){
      //turn off built in LED:
      digitalWrite(13,LOW);
    }
    else{
      sendData("\nErrRead");                 //Command ERROR CODE for UNABLE TO READ
    }
  }
  delay(responseTime);
}


void sendData(String str){
  String len="";
  len+=str.length();
  sendToWifi("AT+CIPSEND=0,"+len,responseTime,DEBUG);
  delay(100);
  sendToWifi(str,responseTime,DEBUG);
  delay(100);
  sendToWifi("AT+CIPCLOSE=5",responseTime,DEBUG);
}


boolean find(String string, String value){
  return string.indexOf(value)>=0;
}

String  readSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(Serial.available()>0){
    value[index_count]=Serial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}

String  readWifiSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(wifiSerial.available()>0){
    value[index_count]=wifiSerial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}


String sendToWifi(String command, const int timeout, boolean debug){
  String response = "";
  wifiSerial.println(command); // send the read character to the esp8266
  long int time = millis();
  while( (time+timeout) > millis())
  {
    while(wifiSerial.available())
    {
    // The esp has data so display its output to the serial window 
    char c = wifiSerial.read(); // read the next character.
    response+=c;
    }  
  }
  if(debug)
  {
    Serial.println(response);
  }
  return response;
}


String sendToUno(String command, const int timeout, boolean debug){
  String response = "";
  Serial.println(command); // send the read character to the esp8266
  long int time = millis();
  while( (time+timeout) > millis())
  {
    while(Serial.available())
    {
      // The esp has data so display its output to the serial window 
      char c = Serial.read(); // read the next character.
      response+=c;
    }  
  }
  if(debug)
  {
    Serial.println(response);
  }
  return response;
}
