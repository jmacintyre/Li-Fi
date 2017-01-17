
char message[38];
int i;

void setup() {
  Serial.begin(9600); // set the baud rate
  Serial.println("Ready"); // print "Ready" once
}
void loop() {
  
  if(Serial.available()){ // only send data back if data has been sent
    
    message[i] = Serial.read(); // read the incoming data
    Serial.print(message[i]); // send the data back in a new line so that it is not all one long line
  }

  if(i == 38){
    i = 0;
    Serial.println();
  }
  delay(5); // delay for 1/10 of a second
}
