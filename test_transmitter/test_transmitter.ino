int LED = 3;
int brightness = 220;
int delta = 35;
long wait = 330;
long finish = 3125;
long prevTime = 0;
long startTime = prevTime;
int i = 0;
char msg;

void setup() {
  
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  analogWrite(LED, 35);
}

void loop() {

  if(Serial.available()){ // only send data back if data has been sent
    delay(45);
    msg = Serial.read();
    Serial.print(int(msg)-48);
    analogWrite(LED, ((int(msg) - 48) * brightness) + delta);
  }
  else{
    analogWrite(LED, delta);
  }
 }
