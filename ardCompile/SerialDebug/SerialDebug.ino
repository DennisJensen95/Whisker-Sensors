void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  while ( !Serial );
}

void loop() {
  digitalWrite(13, HIGH);
  Serial.println("High");
  Serial.send_now();

  delay(1000);

  digitalWrite(13, LOW);
  Serial.println("Low");
  Serial.send_now();
  
  delay(1000);

}
