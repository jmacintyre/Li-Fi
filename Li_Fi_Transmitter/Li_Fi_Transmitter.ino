int LED = 3;
int delta = 220;
int brightness = 35;
long wait = 5000;
long prevTime = 0;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  delay(5000);
}

void loop() {
  unsigned long currentTime = millis();
  if(currentTime - prevTime > wait){
    analogWrite(LED, 255);
    for(int i = 0; i < 9; i++){
      analogWrite(LED, brightness - delta);
      delta = -delta;
      delay(625);
    } 
    prevTime = millis();
    delay(5000);
  }
}
