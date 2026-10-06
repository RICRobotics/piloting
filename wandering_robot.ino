const int ENA = 5, IN1 = 8, IN2 = 9;    // left motor
const int ENB = 6, IN3 = 10, IN4 = 11;  // right motor
const int TRIG = A0, ECHO = A1;
const int BLUE = 2, RED = 3;

const int SPEED = 200; // (0-255)
const int TURN_SPEED = 200; // (0-255)
const int STOP_CM = 20; // sensor sensitivity before triggers anything
const int BACKUP_MS = 800; // how long to back up
const float MS_PER_DEG = 4.0;  // just a rough guess, it's close enough. in theory should be calibrated reguarly. doesnt really matter for any of this yet. im sure will be annoying to be consistent w. our current cardboard setup and could become important depending on what we start doing.

void setMotors(int left, int right) {
  // this function takes two ints, -255 to 255, and sets the motors as needed
  // IN1 and IN2 are the two direction parts of motor A. They can recieve either 5v or ground - 
  // 5v, GND means motor goes one way; GND, 5v means go the other way, so this takes the signs of the ints given, 
  // and then also used the mag for ENA, which connects to the motor's third pin, and defines the speed, in whatever dir.

  // setMotors(255, -255) turns cw currently.
  digitalWrite(IN1, left > 0);  digitalWrite(IN2, left < 0);
  digitalWrite(IN3, right > 0); digitalWrite(IN4, right < 0);
  // the left > 0 or right < 0 etc is evaluated to a boolean, which is interpreted as HIGH / LOW, same as 0 or 1.

  analogWrite(ENA, abs(left));  analogWrite(ENB, abs(right));
}

float readCm() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2); // prep needed to sense

  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);  // while TRIG is HIGH, it's sending a sound pulse, so this sends a 10 microsecond pulse out
  // the rest of the process is done automatically by the sensor's chip - it sends out more signals, and then also listens for them to return.

  long d = pulseIn(ECHO, HIGH, 30000);
  // pulseIn measures how long it was high for - ECHO should be high until the sensor hears something bounce back, and then go low. the 30,000 is a timeout of 30,000 milliseconds at which point it returns 0

  return (d == 0) ? 999 : d * 0.0343 / 2; 
  // and then here we convert the time in microseconds to distance in cm
  // if d==0, sensor heard nothing so 999cm is subbed in
  // else, sound travels at ~343 m/s, or .0343 cm / microsecconds, and then / 2 because it travels there and back!
}

void setup() {
  // setup() is run once. everything below is just boilerplate, delay(3000) means it doesnt do anything for 3 seconds on startup
  int outs[] = {ENA, IN1, IN2, ENB, IN3, IN4, TRIG, BLUE, RED};
  for (int i = 0; i < 9; i++) pinMode(outs[i], OUTPUT);
  pinMode(ECHO, INPUT);
  randomSeed(analogRead(A2));  
  // A2 is not connected to anything, so we use it to seed the random-ness b/c it's just noise. unseeded cpp would behave the same each time
  delay(3000);
}

void loop() {
  if (readCm() < STOP_CM) {
    // if sensor sees something close in front of it:
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, HIGH);
    // i use blue = moving forward, red = stopping or reversing or turning

    setMotors(0, 0); // stop motors
    delay(300);
    setMotors(-SPEED, -SPEED); // go backwards
    delay(BACKUP_MS);
    setMotors(0, 0); // stop
    delay(300);

    int deg = random(45, 316);
    setMotors(TURN_SPEED, -TURN_SPEED); // spin in place (rn this is clockwise)
    delay(deg * MS_PER_DEG); // deg * (ms/deg) = ms, so this is how many ms to wait before it has turned deg degrees
    setMotors(0, 0); // stop
    delay(300);
    digitalWrite(RED, LOW);
  }
  digitalWrite(BLUE, HIGH); // blue LED = going forward
  setMotors(SPEED, SPEED); // onwards and upwards
  delay(30);
}
