#include <Servo.h>


const int ENA = 5, IN1 = 8, IN2 = 9;    // left motor
const int ENB = 6, IN3 = 10, IN4 = 11;  // right motor
const int TRIG = A0, ECHO = A1;
const int BLUE = 2, RED = 3;

const int SPEED = 170; // (0-255)
const int TURN_SPEED = 200; // (0-255)
const int STOP_CM = 20; // sensor sensitivity before triggers anything
const int BACKUP_MS = 800; // how long to back up
const float MS_PER_DEG = 4.0;  // just a rough guess, it's close enough. in theory should be calibrated reguarly. doesnt really matter for any of this yet. im sure will be annoying to be consistent w. our current cardboard setup and could become important depending on what we start doing.


const int SERVO_PIN = 13; // servo signal wire. pin 13 also has the board's L light, so that flickers a bit, harmless

// 90 = center, sweeps between SWEEP_MIN and SWEEP_MAX
const int CENTER_ANG = 90;
const int SWEEP_MIN = 60, SWEEP_MAX = 120;
const int deg_per_STEP = 5; // degrees servo rotates per loop


Servo neck; // servo object
int neckAng = CENTER_ANG;
int dir = 1; // dir that the thing is currently sweeping, 1 = right, -1 = left


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

  neck.attach(SERVO_PIN, 500, 2500); // start the servo - feed function the pin, and then 500 and 2500 define the range of input pulses, how the servo translates from signal to angle
  neck.write(CENTER_ANG); // point the servo forward

  delay(3000);
}

void loop() {

  neckAng += dir * deg_per_STEP; // update angle
  if (neckAng >= SWEEP_MAX || neckAng <= SWEEP_MIN) {
    dir = -dir; // if the angle is at either end, switch directions
  } 
  neck.write(neckAng);
  delay(40); // wait some time to let servo move before you measure

  float cm = readCm();

  // used to be "if (readCm() < STOP_CM) {", but now it just read off wherever the senor is rn
  if (cm < STOP_CM) {
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
