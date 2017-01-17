int PR = A0; // photoresisitor pin

void setup() {
  // put your setup code here, to run once:
  pinMode(PR, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(PR));
  delay(100);
}
