#ifndef MOTHERBOARD_H
#define MOTHERBOARD_H

#include <../src/util/Types.h>
#include <../src/Emulator/Memory/RAM.h>
#include <../src/Emulator/Memory/ROM.h>
#include <../src/Emulator/BIOS.h>
#include <../src/Device/Storage/HDD.h>
#include <../src/Device/Storage/SSD.h>

class Motherboard;

class Motherboard {
    private:
        RAM ram;
        ROM rom;

    public:
        void reset();
        void writeByte(Word address, Byte byte);
        Byte readByte(Word address);

        // overload subscript operator to write to ram
        Byte& operator[](Word address);

        // overload subscript operator to read from ram
        const Byte& operator[](Word address) const;
        
};

#endif // MOTHERBOARD_H