#include "emulator32bit/Emulator32bit.h"
#include "emulator32bit/VirtualMemory.h"
#include "util/Logger.h"
#include "util/Types.h"

#include <stdio.h>

const word Emulator32bit::RAM_MEM_SIZE = 16;
const word Emulator32bit::RAM_MEM_START = 0;
const byte Emulator32bit::ROM_DATA[16 << PAGE_PSIZE] = {};
const word Emulator32bit::ROM_MEM_SIZE = 16;
const word Emulator32bit::ROM_MEM_START = 16;

Emulator32bit::Emulator32bit(word ram_mem_psize, word ram_mem_pstart, const byte rom_data[], word rom_mem_psize, word rom_mem_pstart)
		: disk(new MockDisk()), mmu(new MockVirtualMemory(ram_mem_pstart, ram_mem_pstart + ram_mem_psize)),
		system_bus(RAM(ram_mem_psize, ram_mem_pstart), ROM(rom_data, rom_mem_psize, rom_mem_pstart), *mmu) {
	fill_out_instructions();
	reset();
}

Emulator32bit::Emulator32bit() : Emulator32bit(RAM_MEM_SIZE, RAM_MEM_START, ROM_DATA, ROM_MEM_SIZE, ROM_MEM_START) { }

Emulator32bit::Emulator32bit(RAM ram, ROM rom, Disk* disk) : disk(disk),
		mmu(new VirtualMemory(ram.get_lo_page(), ram.get_hi_page(), *disk)), system_bus(ram, rom, *mmu) {
	fill_out_instructions();
	reset();
}

Emulator32bit::~Emulator32bit() {
	disk->write_all();
	delete disk;
	delete mmu;
}

void Emulator32bit::fill_out_instructions() {
	/* fill out instruction functions and construct disassembler instruction mapping */
	#define _INSTR(op) _instructions[_op_##op] = Emulator32bit::_##op;

	_INSTR(hlt)

	_INSTR(add)
	_INSTR(sub)
	_INSTR(rsb)
	_INSTR(adc)
	_INSTR(sbc)
	_INSTR(rsc)
	_INSTR(mul)
	_INSTR(umull)
	_INSTR(smull)

	_INSTR(vabs_f32)
	_INSTR(vneg_f32)
	_INSTR(vsqrt_f32)
	_INSTR(vadd_f32)
	_INSTR(vsub_f32)
	_INSTR(vdiv_f32)
	_INSTR(vmul_f32)
	_INSTR(vcmp_f32)
	_INSTR(vsel_f32)
	_INSTR(vcint_f32)
	_INSTR(vcflo_f32)
	_INSTR(vmov_f32)

	_INSTR(and)
	_INSTR(orr)
	_INSTR(eor)
	_INSTR(bic)
	_INSTR(lsl)
	_INSTR(lsr)
	_INSTR(asr)
	_INSTR(ror)

	_INSTR(cmp)
	_INSTR(cmn)
	_INSTR(tst)
	_INSTR(teq)

	_INSTR(mov)
	_INSTR(mvn)

	_INSTR(ldr)
	_INSTR(ldrb)
	_INSTR(ldrh)
	_INSTR(str)
	_INSTR(strb)
	_INSTR(strh)
	_INSTR(swp)
	_INSTR(swpb)
	_INSTR(swph)

	_INSTR(b)
	_INSTR(bl)
	_INSTR(bx)
	_INSTR(blx)
	_INSTR(swi)

	_INSTR(adrp)

	_INSTR(hlt)

	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)
	// _INSTR(nop_)

	_INSTR(nop)
	#undef _INSTR
}

void Emulator32bit::print() {
	printf("32 bit emulator\nRegisters:\n");
	printf(" pc: %s\n sp: %s\nxzr: %s\n", to_color_hex_str(_pc).c_str(), to_color_hex_str(_x[SP]).c_str(), to_color_hex_str((word)0).c_str());
	for (int i = 0; i < SP; i++) {
		printf("x%.2d: %s\n", i, to_color_hex_str(_x[i]).c_str());
	}

	printf("\nMemory Dump: TODO");
}

void Emulator32bit::run(unsigned int instructions, EmulatorException &exception) {
	// Run the emulator for a given number of instructions
	while (instructions > 0 && exception.isOK()) {
		word instr = system_bus.read_word(_pc, exception.sys_bus_exception, exception.mem_read_exception);
		exception.instr = instr;
		execute(instr, exception);

		// print flags
		// printf("flags %d %d %d %d\n", test_bit(_pstate, N_FLAG), test_bit(_pstate, Z_FLAG), test_bit(_pstate, C_FLAG), test_bit(_pstate, V_FLAG));

		_pc += 4;
		instructions--;
	}

	if (!exception.isOK()) {
		handle_exception(exception);
	}
}

void Emulator32bit::run(unsigned int instructions) {
	EmulatorException exception;
	run(instructions, exception);
}

void Emulator32bit::reset() {
	system_bus.reset();
	for (int i = 0; i < sizeof(_x) / sizeof(_x[0]); i++) {
		_x[i] = 0;
	}
	_pstate = 0;
	_pc = 0;

}

void Emulator32bit::execute(word instr, EmulatorException &exception) {
	byte opcode = bitfield_u32(instr, 26, 6);

	if (!_instructions[opcode]) {
		exception.type = EmulatorException::Type::BAD_INSTR;
		return;
	}

	(this->*_instructions[opcode])(instr, exception);
}

word Emulator32bit::read_reg(byte reg, EmulatorException &exception) {
	if (reg == XZR) {
		return 0;
	} else if (reg < (sizeof(_x) / sizeof(word))) {
		return _x[reg];
	}

	exception.type = EmulatorException::Type::BAD_REG;
	return 0;
}

void Emulator32bit::write_reg(byte reg, word val, EmulatorException &exception) {
	lgr::log(lgr::Logger::LogType::INFO, std::stringstream() << "WRITING " << std::to_string(val) << " to reg " << std::to_string((word)reg));
	if (reg == XZR) {
		return;
	} else if (reg < (sizeof(_x) / sizeof(word))) {
		_x[reg] = val;
		return;
	}

	exception.type = EmulatorException::Type::BAD_REG;
}

void Emulator32bit::handle_exception(EmulatorException &exception) {
	// todo later
	lgr::log(lgr::Logger::LogType::INFO, std::stringstream() << "Emulator32bit exception at " << disassemble_instr(exception.instr));
}

void Emulator32bit::set_NZCV(bool N, bool Z, bool C, bool V) {
	_pstate = set_bit(_pstate, N_FLAG, N);
	_pstate = set_bit(_pstate, Z_FLAG, Z);
	_pstate = set_bit(_pstate, C_FLAG, C);
	_pstate = set_bit(_pstate, V_FLAG, V);
}


bool Emulator32bit::EmulatorException::isOK() {
	return type == EmulatorException::Type::AOK && sys_bus_exception.type == SystemBus::SystemBusException::AOK
		&& mem_read_exception.type == Memory::MemoryReadException::Type::AOK
		&& mem_write_exception.type == Memory::MemoryWriteException::Type::AOK;
}