#pragma once
#ifndef SYSTEM_BUS_H

#include "emulator32bit/emulator32bit_util.h"
#include "emulator32bit/memory.h"
#include "emulator32bit/virtual_memory.h"

#include <vector>
class SystemBus
{
	public:
		SystemBus(RAM ram, ROM rom, VirtualMemory& mmu);

		/* expose for now */
		RAM ram;
		ROM rom;
		VirtualMemory& mmu;				// todo, we don't need MMU here, we could just called

		struct Exception {
			enum class Type {
				AOK,
				INVALID_ADDRESS,
				CONFLICT_ADDRESSES
			} type = Type::AOK;
			word address = 0;
		};

		static Exception hide_sys_bus_exception;
		static Memory::ReadException hide_mem_read_exception;
		static Memory::WriteException hide_mem_write_exception;

		/**
		 * Read a byte from the system bus
		 *
		 * @param address The address to read from
		 * @param exception The exception raised by the read operation
		 * @return The byte read from the address
		 */
		byte read_byte(word address, Exception &bus_exception = hide_sys_bus_exception,
				Memory::ReadException &mem_exception = hide_mem_read_exception, bool memory_mapped = true);
		hword read_hword(word address, Exception &bus_exception = hide_sys_bus_exception,
				Memory::ReadException &mem_exception = hide_mem_read_exception, bool memory_mapped = true);
		word read_word(word address, Exception &bus_exception = hide_sys_bus_exception,
				Memory::ReadException &mem_exception = hide_mem_read_exception, bool memory_mapped = true);
		word read_word_aligned_ram(word address, Exception &bus_exception = hide_sys_bus_exception,
				Memory::ReadException &mem_exception = hide_mem_read_exception, bool memory_mapped = true);


		dword read_val(word address, int n_bytes, Exception &bus_exception = hide_sys_bus_exception,
				Memory::ReadException &mem_exception = hide_mem_read_exception, bool memory_mapped = true);

		/**
		 * Write a byte to the system bus
		 *
		 * @param address The address to write to
		 * @param exception The exception raised by the write operation
		 * @param data The byte to write
		 */
		void write_byte(word address, byte data, Exception &bus_exception = hide_sys_bus_exception,
				Memory::WriteException &mem_exception = hide_mem_write_exception, bool memory_mapped = true);
		void write_hword(word address, hword data, Exception &bus_exception = hide_sys_bus_exception,
				Memory::WriteException &mem_exception = hide_mem_write_exception, bool memory_mapped = true);
		void write_word(word address, word data, Exception &bus_exception = hide_sys_bus_exception,
				Memory::WriteException &mem_exception = hide_mem_write_exception, bool memory_mapped = true);

		void write_val(word address, dword val, int n_bytes, Exception &bus_exception = hide_sys_bus_exception,
				Memory::WriteException &mem_exception = hide_mem_write_exception, bool memory_mapped = true);

		void reset();

	private:
		std::vector<Memory*> mems;

		word map_address(word address, VirtualMemory::Exception& exception);
		inline word map_read_address(word address, SystemBus::Exception &bus_exception, Memory::ReadException &mem_exception) {
			VirtualMemory::Exception vm_exception;
			//todo handle exceptions
			return map_address(address, vm_exception);
		}
		inline word map_write_address(word address, SystemBus::Exception &bus_exception, Memory::WriteException &mem_exception) {
			VirtualMemory::Exception vm_exception;
			// todo, handle exceptions
			return map_address(address, vm_exception);
		}
		Memory* route_memory(const word address, Exception &bus_exception);
};

#endif /* SYSTEM_BUS */