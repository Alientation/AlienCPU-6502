#include <gtest/gtest.h>

#include <AlienCPUTest.h>

class StackTest : public testing::Test {
    public: 
        AlienCPU cpu;

    virtual void SetUp() {
        cpu.reset();
    }

    virtual void TearDown() {

    }
};


// TODO
