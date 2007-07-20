/******************************************************************************
 PeT mess@utanet.at Nov 2000
Updated by Dan Boris, 3/4/2007

ToDo:
    - Printer. Tried to implement this but it was not working, currently disabled.
    - Artwork.
    - Tape interface
    - User VIA (not really necessary since it only interfaces to external devices)

******************************************************************************/

#include "driver.h"

#include "machine/6821pia.h"
#include "machine/6522via.h"
#include "machine/6532riot.h"
#include "includes/aim65.h"

static ADDRESS_MAP_START( aim65_mem , ADDRESS_SPACE_PROGRAM, 8)
	//     -03ff 1k version
	//     -0fff 4k version
	AM_RANGE( 0x0000, 0x0fff) AM_RAM
//  { 0xa000, 0xa00f, via_1_r }, // user via
	AM_RANGE( 0xa400, 0xa47f) AM_RAM // riot6532 ram
	AM_RANGE( 0xa480, 0xa49f) AM_READWRITE( r6532_0_r, r6532_0_w )
	AM_RANGE( 0xa800, 0xa80f) AM_READWRITE( via_0_r, via_0_w )
	AM_RANGE( 0xac00, 0xac03) AM_READWRITE( pia_0_r, pia_0_w )
	AM_RANGE( 0xac04, 0xac43) AM_RAM
	AM_RANGE( 0xb000, 0xffff) AM_ROM
ADDRESS_MAP_END

INPUT_PORTS_START( aim65 )
	PORT_START
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1     !") PORT_CODE(KEYCODE_1)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2     \"") PORT_CODE(KEYCODE_2)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3     #") PORT_CODE(KEYCODE_3)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4     $") PORT_CODE(KEYCODE_4)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5     %") PORT_CODE(KEYCODE_5)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6     &") PORT_CODE(KEYCODE_6)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7     '") PORT_CODE(KEYCODE_7)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8     (") PORT_CODE(KEYCODE_8)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9     )") PORT_CODE(KEYCODE_9)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(":     *") PORT_CODE(KEYCODE_MINUS)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F3") PORT_CODE(KEYCODE_EQUALS) PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("PRINT") PORT_CODE(KEYCODE_BACKSPACE) PORT_CODE(KEYCODE_PRTSCR)
	PORT_START
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ESC") PORT_CODE(KEYCODE_TAB) PORT_CODE(KEYCODE_ESC)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Q") PORT_CODE(KEYCODE_Q)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("W") PORT_CODE(KEYCODE_W)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E") PORT_CODE(KEYCODE_E)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("R") PORT_CODE(KEYCODE_R)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("T") PORT_CODE(KEYCODE_T)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y") PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("U") PORT_CODE(KEYCODE_U)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("I") PORT_CODE(KEYCODE_I)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("O") PORT_CODE(KEYCODE_O)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("P") PORT_CODE(KEYCODE_P)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-     =") PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F2    ]") PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CODE(KEYCODE_F2) //? maybe f1
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Return") PORT_CODE(KEYCODE_ENTER)
	PORT_START
    PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CRTL") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A") PORT_CODE(KEYCODE_A)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("S") PORT_CODE(KEYCODE_S)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D") PORT_CODE(KEYCODE_D)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("G") PORT_CODE(KEYCODE_G)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("H") PORT_CODE(KEYCODE_H)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("J") PORT_CODE(KEYCODE_J)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("K") PORT_CODE(KEYCODE_K)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("L") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(";     +") PORT_CODE(KEYCODE_COLON)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LF    @") PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F1    [") PORT_CODE(KEYCODE_BACKSLASH) PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DEL") PORT_CODE(KEYCODE_BACKSPACE)
	PORT_START
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Left Shift") PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Z") PORT_CODE(KEYCODE_Z)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X") PORT_CODE(KEYCODE_X)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C") PORT_CODE(KEYCODE_C)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("V") PORT_CODE(KEYCODE_V)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B") PORT_CODE(KEYCODE_B)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("N") PORT_CODE(KEYCODE_N)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(",    <") PORT_CODE(KEYCODE_COMMA)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".    >") PORT_CODE(KEYCODE_STOP)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/    ?") PORT_CODE(KEYCODE_SLASH)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Right Shift") PORT_CODE(KEYCODE_RSHIFT)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Space") PORT_CODE(KEYCODE_SPACE)
	PORT_START
    PORT_DIPNAME(0x08, 0x08, "Terminal")
	PORT_DIPSETTING( 0x00, "TTY")
	PORT_DIPSETTING( 0x08, "Keyboard")
INPUT_PORTS_END

static gfx_layout aim65_charlayout =
{
        32,2,
        256,                                    /* 256 characters */
        1,                      /* 1 bits per pixel */
        { 0,0 },                  /* no bitplanes; 1 bit per pixel */
        /* x offsets */
        {
			7, 7, 7, 7,
			6, 6, 6, 6,
			5, 5, 5, 5,
			4, 4, 4, 4,
			3, 3, 3, 3,
			2, 2, 2, 2,
			1, 1, 1, 1,
			0, 0, 0, 0
        },
        /* y offsets */
        { 0, 0 },
        1*8
};

static gfx_decode aim65_gfxdecodeinfo[] = {
	{ REGION_GFX1, 0x0000, &aim65_charlayout,                     0, 2 },
    { -1 } /* end of array */
};

static unsigned short aim65_colortable[1][2] = {
	{ 0, 1 },
};


static MACHINE_DRIVER_START( aim65 )
	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 1000000)
	MDRV_CPU_PROGRAM_MAP(aim65_mem, 0)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(1)

    /* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(800, 600)
	MDRV_SCREEN_VISIBLE_AREA(0, 800-1, 0, 600-1)
	MDRV_GFXDECODE( aim65_gfxdecodeinfo )
	MDRV_PALETTE_LENGTH(242 + 32768)
	MDRV_COLORTABLE_LENGTH(sizeof (aim65_colortable) / sizeof(aim65_colortable[0][0]))
	MDRV_PALETTE_INIT( aim65 )

	MDRV_VIDEO_START( aim65 )
	MDRV_VIDEO_UPDATE( aim65 )
MACHINE_DRIVER_END


ROM_START(aim65)
	ROM_REGION(0x10000,REGION_CPU1, 0)
	ROM_LOAD_OPTIONAL ("aim65bas.z26", 0xb000, 0x1000, CRC(36a61f39) SHA1(f5ce0126cb594a565e730973fd140d03c298cefa) )
	ROM_LOAD_OPTIONAL ("aim65bas.z25", 0xc000, 0x1000, CRC(d7b42d2a) SHA1(4bbdb28d332429825adea0266ed9192786d9e392) )
	ROM_LOAD_OPTIONAL ("aim65ass.z24", 0xd000, 0x1000, CRC(0878b399) SHA1(483e92b57d64be51643a9f6490521a8572aa2f68) )
	ROM_LOAD ("aim65mon.z23", 0xe000, 0x1000, CRC(90e44afe) SHA1(78e38601edf6bfc787b58750555a636b0cf74c5c))
	ROM_LOAD ("aim65mon.z22", 0xf000, 0x1000, CRC(d01914b0) SHA1(e5b5ddd4cd43cce073a718ee4ba5221f2bc84eaf))
	ROM_REGION(0x100,REGION_GFX1, 0)
ROM_END

SYSTEM_CONFIG_START(aim65)
SYSTEM_CONFIG_END

/*    YEAR  NAME    PARENT  COMPAT  MACHINE INPUT   INIT    CONFIG  COMPANY     FULLNAME */
COMP(197?,	aim65,	0,		0,		aim65,	aim65,	aim65,	aim65,	"Rockwell",	"AIM 65",0)
