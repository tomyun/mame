// license:BSD-3-Clause
// copyright-holders:Raphael Nabet
/* register names for apexc_get_reg & apexc_set_reg */
#pragma once

#ifndef __APEXC_H__
#define __APEXC_H__

enum
{
	APEXC_CR =1,    /* control register */
	APEXC_A,        /* acumulator */
	APEXC_R,        /* register */
	APEXC_ML,       /* memory location */
	APEXC_WS,       /* working store */
	APEXC_STATE,    /* whether CPU is running */

	APEXC_ML_FULL,  /* read-only pseudo-register for exclusive use by the control panel code
	                in the apexc driver : enables it to get the complete address computed
	                from the contents of ML and WS */
	APEXC_PC    /* doesn't actually exist; is there for the disassembler */
};

class apexc_cpu_device : public cpu_device
{
public:
	// construction/destruction
	apexc_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual uint32_t execute_min_cycles() const override { return 2; }
	virtual uint32_t execute_max_cycles() const override { return 75; }
	virtual uint32_t execute_input_lines() const override { return 0; }
	virtual void execute_run() override;

	// device_memory_interface overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const override { return (spacenum == AS_PROGRAM) ? &m_program_config : ( (spacenum == AS_IO) ? &m_io_config : nullptr ); }

	// device_state_interface overrides
	virtual void state_import(const device_state_entry &entry) override;
	virtual void state_export(const device_state_entry &entry) override;
	virtual void state_string_export(const device_state_entry &entry, std::string &str) const override;

	// device_disasm_interface overrides
	virtual uint32_t disasm_min_opcode_bytes() const override { return 4; }
	virtual uint32_t disasm_max_opcode_bytes() const override { return 4; }
	virtual offs_t disasm_disassemble(char *buffer, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;

	inline uint32_t apexc_readmem(uint32_t address) { return m_program->read_dword((address)<<2); }
	inline void apexc_writemem(uint32_t address, uint32_t data) { m_program->write_dword((address)<<2, (data)); }
	inline void apexc_writemem_masked(uint32_t address, uint32_t data, uint32_t mask) { apexc_writemem((address), (apexc_readmem(address) & ~(mask)) | ((data) & (mask))); }

	uint32_t effective_address(uint32_t address);
	uint32_t word_read(uint32_t address, uint32_t special);
	void word_write(uint32_t address, uint32_t data, uint32_t mask);
	uint8_t papertape_read();
	void papertape_punch(uint8_t data);

	uint32_t load_ml(uint32_t address, uint32_t vector);
	void execute();

	address_space_config m_program_config;
	address_space_config m_io_config;

	uint32_t m_a;   /* accumulator */
	uint32_t m_r;   /* register */
	uint32_t m_cr;  /* control register (i.e. instruction register) */
	int m_ml;     /* memory location (current track in working store, and requested word position within track) (10 bits) */
	int m_working_store;  /* current working store (group of 16 tracks) (1-15) */
	int m_current_word;   /* current word position within track (0-31) */

	int m_running;    /* 1 flag: */
						/* running: flag implied by the existence of the stop instruction */
	uint32_t m_pc;  /* address of next instruction for the disassembler */

	address_space *m_program;
	address_space *m_io;
	int m_icount;

	// For state
	uint32_t m_ml_full;
	uint32_t m_genpc;
};


extern const device_type APEXC;


#endif /* __APEXC_H__ */
