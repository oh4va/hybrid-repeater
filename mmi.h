/** @file mmi.h
    @brief Type definitions and global variables

    This contains the type definitions of the mixed mode repeater
    controller and eventually any macros, constants,
    or global variables you will need.

    @author Veijo Arponen, OH4VA <vepa@iki.fi>
    @bug No known bugs.
*/


#define VERSION "V1.0.0"

/** Supported boards */
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#define ARDUINO_UNO

#elif defined(__AVR_ATtiny85__)
#define ARDUINO_ATTINY

#else
#error "Unsupported board selected!"
#endif

/** Pin mappings for ARDUINO UNO */
#ifdef ARDUINO_UNO
#define GPIO_SVXLINK_PIN      2   // output
#define GPIO_MMDVM_PIN        3   // output
#define GPIO_PTT_PIN          4   // output
#define GPIO_DMKURI_PTT_PIN   5   // input
#define GPIO_MMDVM_PTT_PIN    6   // input
#endif

/** Pin mappings for ATtiny85 */
#ifdef ARDUINO_ATTINY
#define GPIO_SVXLINK_PIN      0   // output
#define GPIO_MMDVM_PIN        1   // output
#define GPIO_PTT_PIN          2   // output
#define GPIO_DMKURI_PTT_PIN   3   // input
#define GPIO_MMDVM_PTT_PIN    4   // input
#endif

/** Delay times */
#ifdef ARDUINO_UNO
#define TIME_START      0
#define TIME_LOOP       2000
#define TIME_LOOP_TOT   2000
#else
#define TIME_START      3000
#define TIME_LOOP       10
#define TIME_LOOP_TOT   100
#endif

/** How long in seconds the last mode is kept selected if mode hang is active */
#define MODE_HANG_TIME 20

/** Minimum transmit time in seconds after which mode hang is activated */
#define MODE_HANG_THRESHOLD 8

/** Maximum transmit time (TOT) in seconds*/
#define PTT_ON_MAX 3600

/** FSM states */
#define MMI_IDLE    0
#define SVX_TX      1
#define SVX_HANG    2
#define DVM_TX      3
#define DVM_HANG    4

/** Type definition for the MMI machine */
typedef struct {
  byte  Out;       // FSM output
  byte  Next[40];  // Next state
} fsm_t;


/** FSM output bits */
#define OUT_NON_BIT  B000  // All output bits are OFF
#define OUT_PTT_BIT  B100  // TX PTT ON
#define OUT_SVX_BIT  B001  // Select TX signal from SVXLINK
#define OUT_DVM_BIT  B010  // Select TX signal from MMDVM

/** Indexed transition table with output */
const fsm_t FSM[] = {
  //    Combined GPIO output, FSM input byte values:                                                               //
  //                           00000000, 00000001, 00000010, 00000011, 00000100, 00000101, 00000110,  00000111,    //
  {              OUT_NON_BIT, {MMI_IDLE,   SVX_TX,   DVM_TX,   SVX_TX, MMI_IDLE,   SVX_TX,   DVM_TX,    SVX_TX}},  // State 0 IDLE
  {OUT_PTT_BIT | OUT_SVX_BIT, {MMI_IDLE,   SVX_TX, MMI_IDLE,   SVX_TX, SVX_HANG,   SVX_TX, SVX_HANG,    SVX_TX}},  // State 1 SVX TX
  {              OUT_SVX_BIT, {MMI_IDLE, MMI_IDLE, MMI_IDLE,   SVX_TX, SVX_HANG,   SVX_TX, SVX_HANG,    SVX_TX}},  // State 2 SVX HANG
  {OUT_PTT_BIT | OUT_DVM_BIT, {MMI_IDLE, MMI_IDLE,   DVM_TX,   DVM_TX, DVM_HANG, DVM_HANG,   DVM_TX,    DVM_TX}},  // State 3 DVM TX
  {              OUT_DVM_BIT, {MMI_IDLE, MMI_IDLE,   DVM_TX,   DVM_TX, DVM_HANG, DVM_HANG,   DVM_TX,    DVM_TX}}   // State 4 DVM HANG
};

/** Global variables */
byte state = MMI_IDLE;              // FSM current state
unsigned long ptt_on_timer = 0;     // TX ON timer
unsigned long mode_hang_timer = 0;  // Mode hang countdown timer
byte mode_hang_on = 0;              // Mode hang timer is ON
byte ptt_tot_on = 0;                // TX ON max time exceeded
