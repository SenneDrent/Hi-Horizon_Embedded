#include <unity.h>
#include <Arduino.h>
#include <DataFrame.h>
#include <SpiControl.h>

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

void noStuffingNeededTest() {
  append_with_stuffing(buf, 2, &bufIndex);
  TEST_ASSERT_EQUAL_UINT8( buf[0], 2 );
  TEST_ASSERT_EQUAL_INT32( bufIndex , 1);
}

void headerByteTest() {
  append_with_stuffing(buf, SpiHeaderByte, &bufIndex);
  TEST_ASSERT_EQUAL_UINT8( buf[0], SpiFlagByte );
  TEST_ASSERT_EQUAL_UINT8( buf[1], SpiHeaderByte );
  TEST_ASSERT_EQUAL_INT32( bufIndex , 2);
}

void generateMessageWithStuffingNoTimeSyncTest() {
  int statusByte = 1;
  int signalByte = 255;
  bufIndex = generateMessageWithStuffing(buf, 0, false, statusByte, signalByte);
  TEST_ASSERT_EQUAL_UINT8( buf[0], SpiHeaderByte );
  TEST_ASSERT_EQUAL_UINT8( buf[1], statusByte );
  TEST_ASSERT_EQUAL_UINT8( buf[2], signalByte );
  TEST_ASSERT_EQUAL_UINT8( buf[3], SpiTrailerByte );
  TEST_ASSERT_EQUAL_INT32(4, bufIndex);
}

void generateMessageWithStuffingTimeSyncTest() {
  int statusByte = 1;
  int signalByte = 255;
  uint32_t time = 1687818365;
  bufIndex = generateMessageWithStuffing(buf, time, true, statusByte, signalByte);
  TEST_ASSERT_EQUAL_UINT8( buf[0], SpiHeaderByte );
  TEST_ASSERT_EQUAL_UINT8( buf[1], statusByte );
  TEST_ASSERT_EQUAL_UINT8( buf[2], signalByte );
  TEST_ASSERT_EQUAL_UINT32(time, (buf[3] << 24) + (buf[4] << 16) + (buf[5] << 8) + buf[6]);
  TEST_ASSERT_EQUAL_UINT8( buf[7], SpiTrailerByte );
  TEST_ASSERT_EQUAL_INT32(8, bufIndex);
}

int runUnityTests(void) {
  UNITY_BEGIN();

  RUN_TEST(noStuffingNeededTest);
  RUN_TEST(headerByteTest);
  RUN_TEST(generateMessageWithStuffingNoTimeSyncTest);
  RUN_TEST(generateMessageWithStuffingTimeSyncTest);

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

