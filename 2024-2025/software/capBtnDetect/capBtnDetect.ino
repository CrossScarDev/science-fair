#include <Adafruit_MPR121.h>

Adafruit_MPR121 cap = Adafruit_MPR121();

void setup() {
  Serial.begin(9600); 
  if (!cap.begin(0x5A, &Wire, 3, 8)) for (;;);
}

void loop() {
  Serial.write(cap.touched());
  delay(50);
}
