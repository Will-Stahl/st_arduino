#define TRIGGER 6
#define NUM_SR04 2
#define XIN 4
#define YIN 3
#define ZIN 2
#define A0 0
#define SAMPLE_NUM 5

float position[NUM_SR04] = {0.0};
uint8_t echoes[NUM_SR04];
unsigned long period_end = 0;  // track end of each measurement cycle

// TODO: communicate with raspberri pi to save it to SD card, replicate on TM4C

void setup() {
  pinMode(TRIGGER, OUTPUT);
  pinMode(XIN, INPUT);
  pinMode(YIN, INPUT);
  // pinMode(ZIN, INPUT);
  Serial.begin(9600);
  echoes[0] = XIN;
  echoes[1] = YIN;
  // echoes[2] = ZIN;
}

void loop() {
  for (int i = 0; i < NUM_SR04; i++) {
    unsigned long sum = 0;
    // take average of SAMPLE_NUM measurements
    for (int j = 0; j < SAMPLE_NUM; j++) {
      triggerHCSR04(TRIGGER);
      // 40ms is plenty long for pulse to start
      unsigned long usPulse = pulseIn(echoes[i], HIGH, 40000);
      // take longer than 25ms as 25ms
      usPulse = usPulse <= 25000 ? usPulse : 25000;
      sum += usPulse;
    }
    position[i] = sum * 0.000343 / (2 * SAMPLE_NUM);
    delay(50);  // allow all sensors to run course
  }
  // temprature read
  int sum = 0;
  for (int i = 0; i < SAMPLE_NUM; i++) {
    sum += analogRead(A0);
  }
  float temp_c = ((sum * 5.0 / (1024 * SAMPLE_NUM)) - 0.5) * 100;
  Serial.print(position[0]);
  Serial.print(" ");
  Serial.print(position[1]);
  Serial.print(" ");
  Serial.println(temp_c);
  // Serial.print(" ");
  // Serial.println(position[2]);

  // wait until 1 second since loop started to proceed
  period_end += 10000;  // global tracker of intervals
  // delay for difference between end of period and current time
  if (period_end > millis())  // proceed if past end of period
    delay((period_end - millis()));
}

void triggerHCSR04(uint8_t pin) {
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);
}
