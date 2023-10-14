#include <gtest/gtest.h>

#include <AlienCPUTest.h>

class DEYTest : public testing::Test {
    public: 
        AlienCPU cpu;

    virtual void SetUp() {
        cpu.reset();
    }

    virtual void TearDown() {

    }
};

TEST_F(DEYTest, DecrementYImplied_Normal) {
    // setting reset vector to begin processing instructions at 0x0001023
    cpu.writeWord(AlienCPU::POWER_ON_RESET_VECTOR, 0x00001023);
    cpu.writeByte(0x00001023, AlienCPU::INS_DEY_IMPL);
    cpu.Y = 0x1234;

    cpu.start(2);

    EXPECT_EQ(cpu.Y, 0x1233); // check decremented memory value
    EXPECT_EQ(cpu.PC, 0x00001024); // check PC points to next instruction
    EXPECT_EQ(cpu.cycles, 2);
    EXPECT_EQ(cpu.P, 0b00100000); // only default flag is set
}

TEST_F(DEYTest, DecrementYImplied_ZEROFLAG) {
    // setting reset vector to begin processing instructions at 0x0001023
    cpu.writeWord(AlienCPU::POWER_ON_RESET_VECTOR, 0x00001023);
    cpu.writeByte(0x00001023, AlienCPU::INS_DEY_IMPL);
    cpu.Y = 0x0001;

    cpu.start(2);

    EXPECT_EQ(cpu.Y, 0x0000); // check decremented memory value
    EXPECT_EQ(cpu.PC, 0x00001024); // check PC points to next instruction
    EXPECT_EQ(cpu.cycles, 2);
    EXPECT_EQ(cpu.P, 0b00100010); // only default flag is set
}

TEST_F(DEYTest, DecrementYImplied_NEGATIVEFLAG) {
    // setting reset vector to begin processing instructions at 0x0001023
    cpu.writeWord(AlienCPU::POWER_ON_RESET_VECTOR, 0x00001023);
    cpu.writeByte(0x00001023, AlienCPU::INS_DEY_IMPL);
    cpu.Y = 0xFFFF;

    cpu.start(2);

    EXPECT_EQ(cpu.Y, 0xFFFE); // check decremented memory value
    EXPECT_EQ(cpu.PC, 0x00001024); // check PC points to next instruction
    EXPECT_EQ(cpu.cycles, 2);
    EXPECT_EQ(cpu.P, 0b10100000); // only default flag is set
}