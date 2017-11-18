#include "MockSleeper.h"

void MockSleeper::sleep(long currentTime) {
    sleepInvokedTime = currentTime;
}
