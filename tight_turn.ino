const int ENA = 5, IN1 = 8, IN2 = 9;    // left motor
const int ENB = 6, IN3 = 10, IN4 = 11;  // right motor

void setup() {
  int outs[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int i = 0; i < 6; i++) pinMode(outs[i], OUTPUT);
  delay(3000);

  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);   // left forward
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);  // right backward

  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
}

void loop() {}
