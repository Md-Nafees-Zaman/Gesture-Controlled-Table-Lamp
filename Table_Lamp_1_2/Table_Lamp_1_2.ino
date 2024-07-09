
#include <Servo.h>

// constants won't change
const int TRIG_PIN = 5;   // Arduino pin connected to Ultrasonic Sensor's TRIG pin
const int ECHO_PIN = 6;   // Arduino pin connected to Ultrasonic Sensor's ECHO pin
const int SERVO_PIN = 9;  // Arduino pin connected to Servo Motor's pin

int arr[10] = { 0 };
int arrayCount = 0;
const int ledn = 7;  // this LED (front blue) lights when hand moves away from table lamp and lamp head moves forward
const int ledm = 4;  // this LED (middle green) lights when someone is within range
const int ledf = 8;  // this LED (back blue) lights when hand moves towards table lamp and lamp head moves backward
const int relay = 11;
const int potpin = A4;  // potentiometer for setting sensor range(distance)
//const int set = 2;
int buzzerPin = 2;
int pot_range = 0;
int range = 90;
int val2 = 20;
int time_flag, hold_counter, hold_on, hold, servo_position = 100, position_Locked, start_flag, backward_counter, load_off, position, seize_backward, load_on, new_position, new_position_flag, load_flag;
int gesPos = 0, gesCount = 0, prevPos = 0, gesDistance = 10, gesDifference = 4;
int ledFlag = 0;

Servo servo;  // create servo object to control a servo

// variables will change:
float duration_us, distance_cm;

void setup() {
  Serial.begin(9600);         // initialize serial port
  pinMode(TRIG_PIN, OUTPUT);  // set arduino pin to output mode
  pinMode(ECHO_PIN, INPUT);
  pinMode(ledn, OUTPUT);
  pinMode(ledm, OUTPUT);
  pinMode(ledf, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(potpin, INPUT);
  //  pinMode(set, INPUT);
  servo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  time_flag = 0, hold_counter = 0, hold_on = 0, hold = 0, position_Locked = 0, start_flag = 0, backward_counter = 0, load_off = 0, load_on = 1, seize_backward = 0, position = 0, new_position = 0, new_position_flag = 0, load_flag = 0;
}

void startSignal() {
  if (start_flag == 0) {
    tone(buzzerPin, 3000);
    delay(200);
    tone(buzzerPin, 3100);
    delay(200);
    noTone(buzzerPin);
    start_flag = 1;
  }
}

void setges() {
  if (position < 65) {
    gesDifference = 4;
    gesDistance = 10;
  } else {
    gesDifference = 6;
    gesDistance = 15;
  }
}

void moveUp() {
  if (servo_position > 20) {

    servo_position -= 20;
    servo.write(servo_position);
    delay(200);  // dalay for slowing down the servo speed
  }
}


void moveDown() {

  if (servo_position < 160) {
    servo_position += 20;
    servo.write(servo_position);
    delay(200);  // dalay for slowing down the servo speed
  }
}

void calculateDistance() {
  digitalWrite(TRIG_PIN, LOW);  //setting trigger pin in LOW state
  delayMicroseconds(10);
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
  // calculate the distance
  distance_cm = 0.017 * duration_us;
  pot_range = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  range = map(pot_range, 0, 1023, 350, 15);  // range is set inverse with potentiometer input
  //range = 100;
}

void lockPosition() {
  digitalWrite(ledn, HIGH);
  digitalWrite(ledm, HIGH);
  digitalWrite(ledf, HIGH);
  delay(100);
  position = distance_cm;
  new_position_flag = 0;
  tone(buzzerPin, 4000);  // buzzer for Locking position
  delay(200);
  noTone(buzzerPin);
  position_Locked = 1;
}

void hold_func(int h) {
  if (!h) {
    tone(buzzerPin, 3000);  // buzzer for hold off.
    hold = 0;
  } else {
    tone(buzzerPin, 3100);
    hold = 1;
  }
  delay(200);
  noTone(buzzerPin);
}

void loop() {

  startSignal();
  calculateDistance();
  setges();

  if (distance_cm < range) {
    if (time_flag < 15) time_flag++;
    if (load_on == 1) {
      digitalWrite(relay, HIGH);
      load_flag = 20;
    }
    if (time_flag >= 10 && position_Locked == 0) lockPosition();
    // else if (distance_cm < position - 35)  // move backward
    // {
    //   if (load_on == 0) {
    //     load_on = 1;
    //     backward_counter = 10;      // for avoiding hold on mode
    //     digitalWrite(relay, HIGH);  // turn on light from off
    //     load_flag = 20;
    //     seize_backward = 5;
    //   }

    //   digitalWrite(ledn, LOW);
    //   digitalWrite(ledm, LOW);
    //   digitalWrite(ledf, HIGH);

    //   if (hold_on == 1 && backward_counter == 0 && hold == 1) {  // hold off
    //     tone(buzzerPin, 3000);                                   // buzzer for hold off.
    //     delay(200);
    //     noTone(buzzerPin);
    //     hold = 0;
    //     hold_on = 0;
    //     backward_counter = 10;
    //     if (hold_counter > 15)
    //       seize_backward = 5;
    //     hold_counter = 0;
    //   }

    //   if (backward_counter < 15)
    //     backward_counter++;

    //   if (backward_counter < 7) {  // set hold on
    //     hold = 1;
    //   } else if (seize_backward == 0) {
    //     hold = 0;
    //     hold_counter = 0;
    //     else load_off++;  // the servo is at 20th position and user wants to turn the light off
    //   }
    // } else if (distance_cm < position - 20)  // move forward
    // {
    //   if (hold == 0)
    //     digitalWrite(ledf, LOW);

    //   digitalWrite(ledm, LOW);
    //   digitalWrite(ledn, HIGH);

    //   seize_backward = 7;
    //   backward_counter = 10;

    //   if (servo_position < 160) {
    //     servo_position += 20;
    //     servo.write(servo_position);
    //     delay(200);  // dalay for slowing down the servo speed

    //   } else load_off++;  // the servo is at 160th position and user wants to turn the light off

    // }

    else if (distance_cm > position - 10 && distance_cm < position + 10)  // stabilizing position of user

    {
      if (hold == 0)
        digitalWrite(ledf, LOW);

      digitalWrite(ledn, LOW);
      digitalWrite(ledm, HIGH);

      if (load_off > 0) load_off--;

      position = distance_cm;
      if (backward_counter > 0)
        backward_counter--;
    }

    else  // user is within range

    {
      if (backward_counter > 0)
        backward_counter--;
      new_position = distance_cm;
      new_position_flag++;
      if (new_position_flag > 200) position_Locked = 0;
      if (load_off > 0) load_off--;
      if (hold == 0)
        digitalWrite(ledf, LOW);

      digitalWrite(ledn, LOW);
      digitalWrite(ledm, HIGH);
    }
  }

  else  // user is out of range
  {
    if (hold == 0)
      digitalWrite(ledf, LOW);

    digitalWrite(ledn, LOW);
    digitalWrite(ledm, LOW);



    if (time_flag == 0) {
      position_Locked = 0;
      position = 0;
      backward_counter = 0;
      arrayCount = 0;
      if (hold == 0 && load_flag == 0)
        digitalWrite(relay, LOW);
    } else {
      time_flag--;
      if (backward_counter > 0) backward_counter--;
    }
  }


  if (hold == 1) {
    digitalWrite(ledf, HIGH);
    if (hold_on == 0) {
      hold_counter++;
      if (hold_counter > 200) {
        // tone(buzzerPin, 3100);  // buzzer for hold on
        delay(200);
        noTone(buzzerPin);
        hold_on = 1;
      }
    }
  }

  if (load_off > 5) {
    load_on = 0;
    digitalWrite(relay, LOW);  //  load off within range
    digitalWrite(ledf, LOW);
  }

  if (seize_backward > 0) seize_backward--;

  if (distance_cm <= position + 10 || distance_cm > range)
    if (new_position_flag > 0)
      new_position_flag--;

  if (load_flag > 0) load_flag--;

  //print the value to Serial Monitor
  // Serial.print("pot_range= ");
  // Serial.print(pot_range);
  // Serial.print(", range = ");
  // Serial.print(range);
  // Serial.print(",  position = ");
  // Serial.print(position);
  if (distance_cm < position - gesDistance) {

    digitalWrite(ledf, HIGH);
    digitalWrite(ledn, HIGH);

    if (gesCount < 10) gesCount++;
    if (gesPos == 0 && gesCount > 2) hold = 1;
    if (gesCount > 5) {
      if (gesPos == 0) {
        gesPos = distance_cm;
      }
    }
    if (gesCount > 5 && hold) hold = 0;
    if (gesPos) {  //
      if (distance_cm > gesPos + gesDifference) {
        if (!hold)
          digitalWrite(ledf, LOW);
        moveDown();

      } else if (distance_cm < gesPos - gesDifference) {
        digitalWrite(ledn, LOW);
        moveUp();
      }
    }

  } else if (gesCount > 0) {
    gesCount--;
    digitalWrite(ledn, LOW);
    if (!hold)
      digitalWrite(ledf, LOW);
  }

  if (gesCount == 0) gesPos = 0;

  // Serial.print(hold_counter);
  // Serial.print(" ");
  // Serial.print(hold_on);
  // Serial.print(" ");
  // Serial.println(seize_backward);
  // Serial.print(",  distance in cm = ");
  // Serial.println(distance_cm);
  delay(100);
}
