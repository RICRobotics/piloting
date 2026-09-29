const int ENA = 5, IN1 = 8, IN2 = 9;    // left motor
const int ENB = 6, IN3 = 10, IN4 = 11;  // right motor
const int TRIG = A0, ECHO = A1;
const int BLUE = 2, RED = 3;

const int SPEED = 200;
const int STOP_CM = 20;
const int BACKUP_MS = 800;

void setMotors(int left, int right) {
  digitalWrite(IN1, left > 0);  digitalWrite(IN2, left < 0);
  digitalWrite(IN3, right > 0); digitalWrite(IN4, right < 0);
  analogWrite(ENA, abs(left));  analogWrite(ENB, abs(right));
}

float readCm() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long d = pulseIn(ECHO, HIGH, 30000);
  return (d == 0) ? 999 : d * 0.0343 / 2;
}

void setup() {
  int outs[] = {ENA, IN1, IN2, ENB, IN3, IN4, TRIG, BLUE, RED};
  for (int i = 0; i < 9; i++) pinMode(outs[i], OUTPUT);
  pinMode(ECHO, INPUT);
  delay(3000);
}

void loop() {
  if (readCm() < STOP_CM) {
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, HIGH);
    setMotors(0, 0);             // stop
    delay(300);
    setMotors(-SPEED, -SPEED);   // reverse
    delay(BACKUP_MS);
    setMotors(0, 0);             // stop for good
    while (true) {}
  }
  digitalWrite(BLUE, HIGH);
  setMotors(SPEED, SPEED);       // forward
  delay(30);
}
