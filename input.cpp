#include <atomic>
#include <thread>
#include <unistd.h>

#include "input.h"

extern "C" {
#include <libu8g2arm/gpio.h>
}

const int ROT_S1 = 20;
const int ROT_S2 = 19;
const int ROT_KEY = 18;

std::thread input_thread;
std::atomic<InputValue> input_value = InputValue::Unknown;
std::atomic_bool thread_stop = false;

void *input_thread_func() {
  int ss1 = 0, skey = 0;
  int s1 = 0, s2 = 0, key = 0;

  while (!thread_stop) {
    s1 = getGPIOValue(ROT_S1);
    s2 = getGPIOValue(ROT_S2);
    key = getGPIOValue(ROT_KEY);

    if (ss1 != s1 && !s1) {
      if (s1 != s2) {
        input_value.store(InputValue::Up);
      } else {
        input_value.store(InputValue::Down);
      }
    }
    if (skey != key && !key)
      input_value.store(InputValue::Enter);
    ss1 = s1;
    skey = key;
    usleep(5000);
  }
  return nullptr;
}

InputValue input_get() { return input_value.exchange(InputValue::Unknown); }

void input_wait_enter() {
  InputValue i = Unknown;
  while (i != Back && i != Enter) {
    sleep(0);
    i = input_get();
  }
}

int input_init() {
  exportGPIOPin(ROT_S1);
  exportGPIOPin(ROT_S2);
  exportGPIOPin(ROT_KEY);
  setGPIODirection(ROT_S1, GPIO_IN);
  setGPIODirection(ROT_S2, GPIO_IN);
  setGPIODirection(ROT_KEY, GPIO_IN);

  input_thread = std::thread(input_thread_func);
  return 1;
}

void input_stop() {
  thread_stop.store(true);
  input_thread.join();
}