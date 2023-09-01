#include <unity.h>
#include <Arduino.h>
#include <DataFrame.h>
#include <ESP_SPI.h>

uint8_t buf[32];
int32_t bufIndex;

void setUp(void) {
  bufIndex = 0;
  for (int i = 0; i < 32; i++) {
    buf[i] = 0;
  }
}

void tearDown(void) {
}

int runUnityTests(void) {
  UNITY_BEGIN();

  return UNITY_END();
}

int main( int argc, char **argv) {
    return runUnityTests();
}
/**
  * For Arduino framework
  */
void setup() {
  // Wait ~2 seconds before the Unity test runner
  // establishes connection with a board Serial interface
  delay(2000);

  runUnityTests();
}
void loop() {}

