void setup() 
{
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop() 
{
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(13, HIGH);
  
}
