#include <emulator6502_tests\AlienCPUTest.h>

class SEITest : public testing::Test {
    public: 
        AlienCPU cpu;

    virtual void SetUp() {
        cpu.reset();
    }

    virtual void TearDown() {

    }
};