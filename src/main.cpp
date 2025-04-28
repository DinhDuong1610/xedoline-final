#include <Arduino.h>

// Chân cảm biến siêu âm
const int trig = 21;
const int echo = 4;

// Chân động cơ
int mRight = 23;
int dirRight = 17;
int dirRight1 = 5;
int mLeft = 22;
int dirLeft = 18;
int dirLeft1 = 19;

// Cảm biến line từ trái -> phải
const uint8_t SENSORS_PIN[] = {13, 12, 14, 27, 26};

// Cờ dừng chương trình
bool stopFlag = false;

int error = 0;

// Khai báo hàm
void motor_control(String sensor);
String read_sensors();
void moveForward(int speedLeft, int speedRight);
void turnLeft(int speedLeft, int speedRight);
void turnRight(int speedLeft, int speedRight);
void stopMotors();
long readUltrasonicDistance();

#define SOUND_SPEED 0.034

void setup()
{
  Serial.begin(115200);

  pinMode(dirRight, OUTPUT);
  pinMode(dirRight1, OUTPUT);
  pinMode(dirLeft, OUTPUT);
  pinMode(dirLeft1, OUTPUT);
  pinMode(mRight, OUTPUT);
  pinMode(mLeft, OUTPUT);

  for (auto pin : SENSORS_PIN)
    pinMode(pin, INPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  stopMotors();
}

void loop()
{
  if (stopFlag)
    return;

  long distance = readUltrasonicDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance <= 3)
  {
    // Có vật gần, tăng tốc
    moveForward(240, 240);
    delay(1500);
    return;
  }

  // Nếu không đo được khoảng cách (distance == 0), tiếp tục theo line
  String sensor = read_sensors();
  motor_control(sensor);
  delay(5);
}

// Đọc cảm biến line
String read_sensors()
{
  String sensor = "";
  for (auto pin : SENSORS_PIN)
    sensor += (char)(digitalRead(pin) + '0');
  Serial.print("Sensor: ");
  Serial.println(sensor);
  return sensor;
}

// Đọc khoảng cách bằng cảm biến siêu âm
long readUltrasonicDistance()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 30000); // timeout 30ms
  long distanceCm = duration * SOUND_SPEED / 2;
  return distanceCm;
}

// Xử lý điều khiển theo line
void motor_control(String sensor)
{
  if (sensor == "00100")
  {
    moveForward(100, 100);
    error = 0;
  }
  else if (sensor == "01000" || sensor == "01100")
  {
    moveForward(60, 100);
    error = -1;
  }
  else if (sensor == "10000" || sensor == "11000")
  {
    unsigned long startTime = millis();
    turnLeft(190, 190);
    while (millis() - startTime < 900)
    {
      String s = read_sensors();
      if (s == "00100" || s == "01100" || s == "01000")
        break;
    }
    error = -1;
  }
  else if (sensor == "00010" || sensor == "00110")
  {
    moveForward(100, 60);
    error = 1;
  }
  else if (sensor == "00001" || sensor == "00011")
  {
    unsigned long startTime = millis();
    turnRight(190, 190);
    while (millis() - startTime < 900)
    {
      String s = read_sensors();
      if (s == "00100" || s == "00110" || s == "00010")
        break;
    }
    error = 1;
  }
  // else if (sensor == "11100")
  else if (sensor == "11100")
  // else if (sensor == "11100" || sensor == "11110")
  {
    unsigned long startTime = millis();
    turnLeft(210, 210);
    while (millis() - startTime < 1000)
    {
      String s = read_sensors();
      if (s == "00100" || s == "01000" || s == "01100")
        break;
    }
    error = -1;
  }
  else if (sensor == "00111")
  // else if (sensor == "00111" || sensor == "01111")
  {
    unsigned long startTime = millis();
    turnRight(210, 210);
    while (millis() - startTime < 1000)
    {
      String s = read_sensors();
      if (s == "00100" || s == "00010" || s == "00110")
        break;
    }
    error = 1;
  }
  else if (sensor == "11111")
  {
    stopMotors();
    stopFlag = true;
  }
  else if (sensor == "00000")
  {
    // stopMotors();
    if (error == -1)
    {
      turnLeft(100, 160);
    }
    else if (error == 1)
    {
      turnRight(160, 100);
    }
    else
    {
      moveForward(80, 80);
    }
  }
  else
  {
    moveForward(80, 80);
  }
}

// Điều khiển cơ bản
void moveForward(int speedLeft, int speedRight)
{
  digitalWrite(dirLeft1, HIGH);
  digitalWrite(dirLeft, LOW);
  digitalWrite(dirRight1, HIGH);
  digitalWrite(dirRight, LOW);
  analogWrite(mLeft, speedLeft);
  analogWrite(mRight, speedRight);
}

void turnLeft(int speedLeft, int speedRight)
{
  digitalWrite(dirLeft1, HIGH);
  digitalWrite(dirLeft, LOW);
  digitalWrite(dirRight1, LOW);
  digitalWrite(dirRight, HIGH);
  analogWrite(mLeft, speedLeft);
  analogWrite(mRight, speedRight);
}

void turnRight(int speedLeft, int speedRight)
{
  digitalWrite(dirLeft1, LOW);
  digitalWrite(dirLeft, HIGH);
  digitalWrite(dirRight1, HIGH);
  digitalWrite(dirRight, LOW);
  analogWrite(mLeft, speedLeft);
  analogWrite(mRight, speedRight);
}

void stopMotors()
{
  digitalWrite(dirLeft1, LOW);
  digitalWrite(dirLeft, LOW);
  digitalWrite(dirRight1, LOW);
  digitalWrite(dirRight, LOW);
  analogWrite(mLeft, 0);
  analogWrite(mRight, 0);
}