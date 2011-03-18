/*****************************************************************************
 *
 * includes/pc.h
 *
 ****************************************************************************/

#ifndef GENPC_H_
#define GENPC_H_

#include "machine/ins8250.h"
#include "machine/i8255a.h"
#include "machine/8237dma.h"
#include "machine/isa.h"

#define MCFG_IBM5160_MOTHERBOARD_ADD(_tag, _cputag, _config) \
    MCFG_DEVICE_ADD(_tag, IBM5160_MOTHERBOARD, 0) \
    MCFG_DEVICE_CONFIG(_config) \
    ibm5160_mb_device_config::static_set_cputag(device, _cputag); \

// ======================> isabus_interface

struct motherboard_interface
{
    devcb_write_line	m_kb_set_clock_signal_func;
    devcb_write_line	m_kb_set_data_signal_func;
};
	
// ======================> ibm5160_mb_device_config
 
class ibm5160_mb_device_config : public device_config,
								 public motherboard_interface
{
	friend class ibm5160_mb_device;
 
	// construction/destruction
	ibm5160_mb_device_config(const machine_config &mconfig, const char *tag, const device_config *owner, UINT32 clock);
 
public:
	// allocators
	static device_config *static_alloc_device_config(const machine_config &mconfig, const char *tag, const device_config *owner, UINT32 clock);
	virtual device_t *alloc_device(running_machine &machine) const;
 
	// inline configuration
	static void static_set_cputag(device_config *device, const char *tag);
 
	// optional information overrides
	virtual machine_config_constructor machine_config_additions() const;
	virtual const input_port_token *input_ports() const;
	
	const char *m_cputag;

protected:
	// device_config overrides
	virtual void device_config_complete();				
};
 
 
// ======================> ibm5160_mb_device
class ibm5160_mb_device : public device_t
{
        friend class ibm5160_mb_device_config;
 
        // construction/destruction
        ibm5160_mb_device(running_machine &_machine, const ibm5160_mb_device_config &config);

protected:
        // device-level overrides
        virtual void device_start();
        virtual void device_reset();
		void install_device(device_t *dev, offs_t start, offs_t end, offs_t mask, offs_t mirror, read8_device_func rhandler, write8_device_func whandler);
		void install_device_write(device_t *dev, offs_t start, offs_t end, offs_t mask, offs_t mirror, write8_device_func whandler);
private:
        // internal state
        const ibm5160_mb_device_config &m_config;
public:
        required_device<cpu_device>  maincpu;
		required_device<device_t>  pic8259;
		required_device<device_t>  dma8237;
		required_device<device_t>  pit8253;
		required_device<device_t>  ppi8255;
		required_device<device_t>  speaker;
		required_device<isa8_device>  isabus;
		
    	devcb_resolved_write_line	m_kb_set_clock_signal_func;
    	devcb_resolved_write_line	m_kb_set_data_signal_func;
		
		/* U73 is an LS74 - dual flip flop */
		/* Q2 is set by OUT1 from the 8253 and goes to DRQ1 on the 8237 */
		UINT8	u73_q2;
		UINT8	out1;
		int dma_channel;
		UINT8 dma_offset[4];
		UINT8 pc_spkrdata;
		UINT8 pc_input;
		
		UINT8 nmi_enabled;

		int						ppi_portc_switch_high;
		int						ppi_speaker;
		int						ppi_keyboard_clear;
		UINT8					ppi_keyb_clock;
		UINT8					ppi_portb;
		UINT8					ppi_clock_signal;
		UINT8					ppi_data_signal;
		UINT8					ppi_shift_register;
		UINT8					ppi_shift_enable;
		
		static IRQ_CALLBACK(pc_irq_callback);
		
		// interface to the keyboard
		DECLARE_WRITE_LINE_MEMBER( keyboard_clock_w );
		DECLARE_WRITE_LINE_MEMBER( keyboard_data_w );

		DECLARE_READ8_MEMBER ( pc_ppi_porta_r );
		DECLARE_READ8_MEMBER ( pc_ppi_portc_r );
		DECLARE_WRITE8_MEMBER( pc_ppi_portb_w );

		DECLARE_WRITE_LINE_MEMBER( pc_pit8253_out1_changed );
		DECLARE_WRITE_LINE_MEMBER( pc_pit8253_out2_changed );
		
		DECLARE_WRITE_LINE_MEMBER( pc_dma_hrq_changed );
		DECLARE_WRITE_LINE_MEMBER( pc_dma8237_out_eop );
		DECLARE_READ8_MEMBER( pc_dma_read_byte );
		DECLARE_WRITE8_MEMBER( pc_dma_write_byte );
		DECLARE_READ8_MEMBER( pc_dma8237_1_dack_r );
		DECLARE_READ8_MEMBER( pc_dma8237_2_dack_r );
		DECLARE_READ8_MEMBER( pc_dma8237_3_dack_r );
		DECLARE_WRITE8_MEMBER( pc_dma8237_1_dack_w );
		DECLARE_WRITE8_MEMBER( pc_dma8237_2_dack_w );
		DECLARE_WRITE8_MEMBER( pc_dma8237_3_dack_w );
		DECLARE_WRITE8_MEMBER( pc_dma8237_0_dack_w );
		DECLARE_WRITE_LINE_MEMBER( pc_dack0_w );
		DECLARE_WRITE_LINE_MEMBER( pc_dack1_w );
		DECLARE_WRITE_LINE_MEMBER( pc_dack2_w );
		DECLARE_WRITE_LINE_MEMBER( pc_dack3_w );

		DECLARE_WRITE_LINE_MEMBER( pc_cpu_line );
		DECLARE_WRITE_LINE_MEMBER( pc_speaker_set_spkrdata );
		
};
 
 
// device type definition
extern const device_type IBM5160_MOTHERBOARD;

#endif /* GENPC_H_ */
