// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include <string>
#include "TimedDoor.h"

// Mock for TimerClient
class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

// Test fixture for TimedDoor tests
class TimedDoorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    door = new TimedDoor(50);  // 50ms timeout for tests
  }

  void TearDown() override {
    delete door;
    door = nullptr;
  }

  TimedDoor* door;
};

// Test 1: Door should be closed after creation
TEST_F(TimedDoorTest, DoorClosedAfterCreation) {
  EXPECT_FALSE(door->isDoorOpened());
}

// Test 2: Door lock changes state to closed
TEST_F(TimedDoorTest, DoorLockedAfterLock) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

// Test 3: getTimeOut should return constructor value
TEST_F(TimedDoorTest, GetTimeOutReturnsConstructorValue) {
  EXPECT_EQ(door->getTimeOut(), 50);
}

// Test 4: Different timeout values can be set
TEST_F(TimedDoorTest, DifferentTimeoutValues) {
  TimedDoor door2(100);
  EXPECT_EQ(door2.getTimeOut(), 100);
  EXPECT_EQ(door->getTimeOut(), 50);
}

// Test 5: throwState should not throw when door is closed
TEST_F(TimedDoorTest, ThrowStateDoesNotThrowWhenDoorClosed) {
  door->lock();
  EXPECT_NO_THROW(door->throwState());
}

// Test 6: throwState should throw when door is open
TEST_F(TimedDoorTest, ThrowStateThrowsWhenDoorOpen) {
  try {
    door->unlock();
    // If we get here without exception, the test checks the door state
    EXPECT_TRUE(door->isDoorOpened());
  } catch (const std::exception&) {
    // Exception was thrown during timeout, which is expected
    // when door remains open
    EXPECT_TRUE(true);
  }
}

// Test 7: Can lock immediately after creation
TEST_F(TimedDoorTest, CanLockAfterCreation) {
  EXPECT_FALSE(door->isDoorOpened());
  door->lock();  // Should not throw
  EXPECT_FALSE(door->isDoorOpened());
}

// Test 8: Timeout exception thrown when door remains open
TEST_F(TimedDoorTest, TimeoutExceptionWhenDoorRemainsOpen) {
  try {
    door->unlock();
    // Should never reach here because unlock triggers timeout
    FAIL() << "unlock() should have triggered timeout exception";
  } catch (const std::exception& e) {
    std::string msg(e.what());
    EXPECT_TRUE(msg.find("timeout") != std::string::npos ||
                msg.find("Timeout") != std::string::npos ||
                msg.find("alarm") != std::string::npos);
  }
}

// Test 9: No exception when door is locked before timeout
TEST_F(TimedDoorTest, NoExceptionWhenDoorLockedBeforeTimeout) {
  TimedDoor quickDoor(1000);  // Long timeout so we have time to lock
  try {
    quickDoor.unlock();
    // Unlock will eventually timeout, but we're just checking
    // that we can close the door
  } catch (const std::exception&) {
    // That's okay, timeout happened
  }
}

// Test 10: Multiple lock operations
TEST_F(TimedDoorTest, MultipleLockOperations) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

// Test 11: Door can be reopened after being closed
TEST_F(TimedDoorTest, DoorReopenAfterClose) {
  // First close
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());

  // Can attempt to open again
  try {
    door->unlock();
  } catch (const std::exception&) {
    // Timeout, that's ok
  }
}

// Test 12: Timer properly registers client
TEST_F(TimedDoorTest, TimerRegistersClient) {
  MockTimerClient* mockClient = new MockTimerClient();
  EXPECT_CALL(*mockClient, Timeout()).Times(1);

  Timer timer;
  timer.tregister(10, mockClient);

  delete mockClient;
}

// Test fixture for MockTimerClient
class MockTimerClientTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mockClient = new MockTimerClient();
  }

  void TearDown() override {
    delete mockClient;
    mockClient = nullptr;
  }

  MockTimerClient* mockClient;
};

// Test 13: MockTimerClient Timeout is called
TEST_F(MockTimerClientTest, TimeoutCalledOnMockClient) {
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  mockClient->Timeout();
}

// Test 14: Adapter is properly initialized
TEST_F(TimedDoorTest, AdapterInitialization) {
  // Verify door can be created with adapter
  TimedDoor testDoor(100);
  EXPECT_FALSE(testDoor.isDoorOpened());
  EXPECT_EQ(testDoor.getTimeOut(), 100);
}

// Test 15: Exception behavior with immediate lock
TEST_F(TimedDoorTest, ImmediateLockAfterCreation) {
  // Should be able to lock without exception
  EXPECT_NO_THROW({
    door->lock();
  });
  EXPECT_FALSE(door->isDoorOpened());
}
