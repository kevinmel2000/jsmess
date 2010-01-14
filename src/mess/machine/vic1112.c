/**********************************************************************

    Commodore VIC-1112 IEEE-488 cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#include "driver.h"
#include "vic1112.h"
#include "cpu/m6502/m6502.h"
#include "machine/6522via.h"
#include "machine/ieee488.h"

/***************************************************************************
    PARAMETERS
***************************************************************************/

#define M6522_0_TAG		"u4"
#define M6522_1_TAG		"u5"

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef struct _vic1112_t vic1112_t;
struct _vic1112_t
{
	const device_config *via0;
	const device_config *via1;
	const device_config *bus;
};

/***************************************************************************
    INLINE FUNCTIONS
***************************************************************************/

INLINE vic1112_t *get_safe_token(const device_config *device)
{
	assert(device != NULL);
	assert(device->token != NULL);
	assert(device->type == VIC1112);
	return (vic1112_t *)device->token;
}

INLINE vic1112_config *get_safe_config(const device_config *device)
{
	assert(device != NULL);
	assert(device->type == VIC1112);
	return (vic1112_config *)device->inline_config;
}

/***************************************************************************
    IMPLEMENTATION
***************************************************************************/

/*-------------------------------------------------
    vic1112_ieee488_srq_w - IEEE-488 service
	request
-------------------------------------------------*/

WRITE_LINE_DEVICE_HANDLER( vic1112_ieee488_srq_w )
{
	vic1112_t *vic1112 = get_safe_token(device);

	via_cb1_w(vic1112->bus, state);
}

/*-------------------------------------------------
    via6522_interface vic1112_via0_intf
-------------------------------------------------*/

static WRITE_LINE_DEVICE_HANDLER( via_irq_w )
{
	cpu_set_input_line(device->machine->firstcpu, M6502_IRQ_LINE, state);
}

static READ8_DEVICE_HANDLER( via0_pb_r )
{
	/*

        bit     description

        PB0     _DAV OUT
        PB1     _NRFD OUT
        PB2     _NDAC OUT
        PB3     _EOI
        PB4     _DAV IN
        PB5     _NRFD IN
        PB6     _NDAC IN
        PB7     _ATN IN

    */

	vic1112_t *vic1112 = get_safe_token(device->owner);
	UINT8 data = 0;

	/* data valid in */
	data |= ieee488_dav_r(vic1112->bus) << 4;

	/* not ready for data in */
	data |= ieee488_nrfd_r(vic1112->bus) << 5;

	/* not data accepted in */
	data |= ieee488_ndac_r(vic1112->bus) << 6;

	/* attention in */
	data |= ieee488_atn_r(vic1112->bus) << 7;

	return data;
}

static WRITE8_DEVICE_HANDLER( via0_pb_w )
{
	/*

        bit     description

        PB0     _DAV OUT
        PB1     _NRFD OUT
        PB2     _NDAC OUT
        PB3     _EOI
        PB4     _DAV IN
        PB5     _NRFD IN
        PB6     _NDAC IN
        PB7     _ATN IN

    */

	vic1112_t *vic1112 = get_safe_token(device->owner);

	/* data valid out */
	ieee488_dav_w(vic1112->bus, device->owner, BIT(data, 0));

	/* not ready for data out */
	ieee488_nrfd_w(vic1112->bus, device->owner, BIT(data, 1));

	/* not data accepted out */
	ieee488_ndac_w(vic1112->bus, device->owner, BIT(data, 2));

	/* end or identify */
	ieee488_eoi_w(vic1112->bus, device->owner, BIT(data, 3));
}

static const via6522_interface vic1112_via0_intf =
{
	DEVCB_NULL,
	DEVCB_HANDLER(via0_pb_r),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_NULL,
	DEVCB_HANDLER(via0_pb_w),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_LINE(via_irq_w)
};

/*-------------------------------------------------
    via6522_interface vic1112_via1_intf
-------------------------------------------------*/

static READ8_DEVICE_HANDLER( via1_pa_r )
{
	/*

        bit     description

        PA0     DI1
        PA1     DI2
        PA2     DI3
        PA3     DI4
        PA4     DI5
        PA5     DI6
        PA6     DI7
        PA7     DI8

    */

	vic1112_t *vic1112 = get_safe_token(device->owner);

	return ieee488_dio_r(vic1112->bus, 0);
}

static WRITE8_DEVICE_HANDLER( via1_pb_w )
{
	/*

        bit     description

        PB0     DO1
        PB1     DO2
        PB2     DO3
        PB3     DO4
        PB4     DO5
        PB5     DO6
        PB6     DO7
        PB7     DO8

    */

	vic1112_t *vic1112 = get_safe_token(device->owner);

	ieee488_dio_w(vic1112->bus, 0, data);
}

static WRITE_LINE_DEVICE_HANDLER( via1_ca2_w )
{
	vic1112_t *vic1112 = get_safe_token(device->owner);

	/* attention out */
	ieee488_atn_w(vic1112->bus, device->owner, state);
}

static WRITE_LINE_DEVICE_HANDLER( via1_cb2_w )
{
	vic1112_t *vic1112 = get_safe_token(device->owner);

	/* end or identify out */
	ieee488_eoi_w(vic1112->bus, device->owner, state);
}

static const via6522_interface vic1112_via1_intf =
{
	DEVCB_HANDLER(via1_pa_r),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL, /* _SRQ IN */
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_NULL,
	DEVCB_HANDLER(via1_pb_w),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_LINE(via1_ca2_w),
	DEVCB_LINE(via1_cb2_w),

	DEVCB_LINE(via_irq_w)
};

/*-------------------------------------------------
    MACHINE_DRIVER( vic1112 )
-------------------------------------------------*/

static MACHINE_DRIVER_START( vic1112 )
	MDRV_VIA6522_ADD(M6522_0_TAG, 0, vic1112_via0_intf)
	MDRV_VIA6522_ADD(M6522_1_TAG, 0, vic1112_via1_intf)
MACHINE_DRIVER_END

/*-------------------------------------------------
    ROM( vic1112 )
-------------------------------------------------*/

ROM_START( vic1112 )
	ROM_REGION( 0x800, VIC1112_TAG, ROMREGION_LOADBYNAME )
	ROM_LOAD( "325329-03.u2", 0x000, 0x800, CRC(d37b6335) SHA1(828c965829d21c60e8c2d083caee045c639a270f) )
ROM_END

/*-------------------------------------------------
    DEVICE_START( vic1112 )
-------------------------------------------------*/

static DEVICE_START( vic1112 )
{
	vic1112_t *vic1112 = get_safe_token(device);
	const vic1112_config *config = get_safe_config(device);
	const address_space *program = cpu_get_address_space(device->machine->firstcpu, ADDRESS_SPACE_PROGRAM);

	/* find devices */
	vic1112->via0 = device_find_child_by_tag(device, M6522_0_TAG);
	vic1112->via1 = device_find_child_by_tag(device, M6522_1_TAG);
	vic1112->bus = devtag_get_device(device->machine, config->bus_tag);

	/* set VIA clocks */
	device_set_clock(vic1112->via0, cpu_get_clock(device->machine->firstcpu));
	device_set_clock(vic1112->via1, cpu_get_clock(device->machine->firstcpu));

	/* map VIAs to memory */
	memory_install_readwrite8_device_handler(program, vic1112->via0, 0x9200, 0x920f, 0, 0, via_r, via_w);
	memory_install_readwrite8_device_handler(program, vic1112->via1, 0x9210, 0x921f, 0, 0, via_r, via_w);

	/* map ROM to memory */
	memory_install_rom(program, 0xb000, 0xb7ff, 0, 0, memory_region(device->machine, VIC1112_TAG));
}

/*-------------------------------------------------
    DEVICE_RESET( vic1112 )
-------------------------------------------------*/

static DEVICE_RESET( vic1112 )
{
	vic1112_t *vic1112 = get_safe_token(device);

	/* reset VIAs */
	device_reset(vic1112->via0);
	device_reset(vic1112->via1);

	/* _IFC */
	ieee488_ifc_w(vic1112->bus, device, 0);
	ieee488_ifc_w(vic1112->bus, device, 1);
}

/*-------------------------------------------------
    DEVICE_GET_INFO( vic1112 )
-------------------------------------------------*/

DEVICE_GET_INFO( vic1112 )
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case DEVINFO_INT_TOKEN_BYTES:					info->i = sizeof(vic1112_t);								break;
		case DEVINFO_INT_INLINE_CONFIG_BYTES:			info->i = sizeof(vic1112_config);							break;
		case DEVINFO_INT_CLASS:							info->i = DEVICE_CLASS_PERIPHERAL;							break;

		/* --- the following bits of info are returned as pointers --- */
		case DEVINFO_PTR_ROM_REGION:					info->romregion = ROM_NAME(vic1112);						break;
		case DEVINFO_PTR_MACHINE_CONFIG:				info->machine_config = MACHINE_DRIVER_NAME(vic1112);		break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME(vic1112);					break;
		case DEVINFO_FCT_STOP:							/* Nothing */												break;
		case DEVINFO_FCT_RESET:							info->reset = DEVICE_RESET_NAME(vic1112);					break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case DEVINFO_STR_NAME:							strcpy(info->s, "Commodore VIC-1112");						break;
		case DEVINFO_STR_FAMILY:						strcpy(info->s, "Commodore VIC-20");						break;
		case DEVINFO_STR_VERSION:						strcpy(info->s, "1.0");										break;
		case DEVINFO_STR_SOURCE_FILE:					strcpy(info->s, __FILE__);									break;
		case DEVINFO_STR_CREDITS:						strcpy(info->s, "Copyright the MESS Team"); 				break;
	}
}
