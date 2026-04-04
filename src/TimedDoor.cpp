// Copyright 2021 GHA Test Team
#include "TimedDoor.h"
#include <thread>
#include <chrono>
#include <stdexcept>

// Custom exception for door timeout
class DoorTimeoutException : public std::exception {
 public:
  const char* what() const noexcept override {
    return "Door timeout: alarm triggered!";
  }
};

// TimedDoor implementation
TimedDoor::TimedDoor(int timeout) 
    : adapter(nullptr), iTimeout(timeout), isOpened(false) {
  adapter = new DoorTimerAdapter(*this);
}

TimedDoor::~TimedDoor() {
  delete adapter;
}

void TimedDoor::lock() {
  isOpened = false;
}

void TimedDoor::unlock() {
  isOpened = true;
  Timer timer;
  timer.tregister(iTimeout, adapter);
}

bool TimedDoor::isDoorOpened() {
  return isOpened;
}

int TimedDoor::getTimeOut() const {
  return iTimeout;
}

void TimedDoor::throwState() {
  if (isOpened) {
    throw DoorTimeoutException();
  }
}

// DoorTimerAdapter implementation
DoorTimerAdapter::DoorTimerAdapter(TimedDoor& d) : door(d) {
}

void DoorTimerAdapter::Timeout() {
  door.throwState();
}

// Timer implementation
Timer::Timer() : client(nullptr) {
}

void Timer::tregister(int timeout, TimerClient* timerClient) {
  client = timerClient;
  sleep(timeout);
  if (client) {
    client->Timeout();
  }
}

void Timer::sleep(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
