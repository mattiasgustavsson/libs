/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

opl.h - v0.1 - OPL3 (SoundBlaster16) emulation with MIDI compatible interface

Do this:
    #define OP_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.

------------------------------------------------------------------------------

This file contains heavily modified code from two different open source 
projects: ymfm https://github.com/aaronsgiles/ymfm (a collection of Yamaha FM 
sound cores) and DOSMid http://dosmid.sourceforge.net (a MIDI and MUS player 
for DOS).

In the case of the ymfm code, I reduced it down to just the OPL3 parts, and
refactored the remaining code from OOP C++ to plain C, and prefixed everything
with OPL_EMU_ or opl_emu_.

For the DOSMidi code, I pretty much used the files OPL.H and OPL.C with only
minor changes, but put it together with the ymfm code to form a single file
library.

It is possible (even likely) that I have messed things up in this process, and
I recommend that you use the original source code versions linked above if you
want to use this for anything serious.

                        / Mattias Gustavsson ( mattias@mattiasgustavsson.com )
*/

#ifndef opl_h
#define opl_h

typedef struct opl_timbre_t {
  unsigned long modulator_E862, carrier_E862;
  unsigned char modulator_40, carrier_40;
  unsigned char feedconn;
  signed char finetune;
  unsigned char notenum;
  signed short noteoffset;
} opl_timbre_t;

typedef struct opl_t opl_t;

/* Initialize hardware upon startup - positive on success, negative otherwise
 * Returns 0 for OPL2 initialization, or 1 if OPL3 has been detected */
opl_t* opl_create( void );

/* close OPL device */
void opl_destroy( opl_t* opl );

/* turns off all notes */
void opl_clear( opl_t* opl );

/* turn note 'on', on emulated MIDI channel */
void opl_midi_noteon( opl_t* opl, int channel, int note, int velocity );

/* turn note 'off', on emulated MIDI channel */
void opl_midi_noteoff( opl_t* opl, int channel, int note );

/* adjust the pitch wheel on emulated MIDI channel */
void opl_midi_pitchwheel( opl_t* opl, int channel, int wheelvalue );

/* emulate MIDI 'controller' messages on the OPL */
void opl_midi_controller( opl_t* opl, int channel, int id, int value );

/* assign a new instrument to emulated MIDI channel */
void opl_midi_changeprog( opl_t* opl, int channel, int program );

void opl_loadinstrument(opl_t* opl, int voice, opl_timbre_t* timbre );

/* loads an IBK bank from file into an array of 128 opl_timbre_t structures.
 * returns 0 on success, non-zero otherwise */
int opl_loadbank_ibk( opl_t* opl, char const* file );

int opl_loadbank_op2(opl_t* opl, void const* data, int size );

void opl_render( opl_t* opl, short* sample_pairs, int sample_pairs_count, float volume );

void opl_write( opl_t* opl, int count, unsigned short* regs, unsigned char* data );

#endif /* opl_h */



#ifdef OPL_IMPLEMENTATION

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /* calloc() */
#include <string.h> /* strdup() */


//-------------------------------------------------
//  bitfield - extract a bitfield from the given
//  value, starting at bit 'start' for a length of
//  'length' bits
//-------------------------------------------------

uint32_t opl_emu_bitfield(uint32_t value, int start, int length )
{
	return (value >> start) & ((1 << length) - 1);
}


//-------------------------------------------------
//  clamp - clamp between the minimum and maximum
//  values provided
//-------------------------------------------------

int32_t opl_emu_clamp(int32_t value, int32_t minval, int32_t maxval)
{
	if (value < minval)
		return minval;
	if (value > maxval)
		return maxval;
	return value;
}

#define opl_min(a,b) (((a)<(b))?(a):(b))
#define opl_max(a,b) (((a)>(b))?(a):(b))


// various envelope states
enum opl_emu_envelope_state
{
	OPL_EMU_EG_ATTACK = 1,
	OPL_EMU_EG_DECAY = 2,
	OPL_EMU_EG_SUSTAIN = 3,
	OPL_EMU_EG_RELEASE = 4,
	OPL_EMU_EG_STATES = 6
};



// three different keyon sources; actual keyon is an OR over all of these
enum opl_emu_keyon_type
{
	OPL_EMU_KEYON_NORMAL = 0,
	OPL_EMU_KEYON_RHYTHM = 1,
	OPL_EMU_KEYON_CSM = 2
};


// ======================> opl_emu_opdata_cache

#define OPL_EMU_PHASE_STEP_DYNAMIC 1

// this class holds data that is computed once at the start of clocking
// and remains static during subsequent sound generation
struct opl_emu_opdata_cache
{
	// set phase_step to this value to recalculate it each sample; needed
	// in the case of PM LFO changes

	uint16_t const *waveform;         // base of sine table
	uint32_t phase_step;              // phase step, or OPL_EMU_PHASE_STEP_DYNAMIC if PM is active
	uint32_t total_level;             // total level * 8 + KSL
	uint32_t block_freq;              // raw block frequency value (used to compute phase_step)
	int32_t detune;                   // detuning value (used to compute phase_step)
	uint32_t multiple;                // multiple value (x.1, used to compute phase_step)
	uint32_t eg_sustain;              // sustain level, shifted up to envelope values
	uint8_t eg_rate[OPL_EMU_EG_STATES];       // envelope rate, including KSR
	uint8_t eg_shift;                 // envelope shift amount
};


// ======================> opl_emu_registers

//
// OPL/OPL2/OPL3/OPL4 register map:
//
//      System-wide registers:
//           01 xxxxxxxx Test register
//              --x----- Enable OPL compatibility mode [OPL2 only] (1 = enable)
//           02 xxxxxxxx Timer A value (4 * OPN)
//           03 xxxxxxxx Timer B value
//           04 x------- RST
//              -x------ Mask timer A
//              --x----- Mask timer B
//              ------x- Load timer B
//              -------x Load timer A
//           08 x------- CSM mode [OPL/OPL2 only]
//              -x------ Note select
//           BD x------- AM depth
//              -x------ PM depth
//              --x----- Rhythm enable
//              ---x---- Bass drum key on
//              ----x--- Snare drum key on
//              -----x-- Tom key on
//              ------x- Top cymbal key on
//              -------x High hat key on
//          101 --xxxxxx Test register 2 [OPL3 only]
//          104 --x----- Channel 6 4-operator mode [OPL3 only]
//              ---x---- Channel 5 4-operator mode [OPL3 only]
//              ----x--- Channel 4 4-operator mode [OPL3 only]
//              -----x-- Channel 3 4-operator mode [OPL3 only]
//              ------x- Channel 2 4-operator mode [OPL3 only]
//              -------x Channel 1 4-operator mode [OPL3 only]
//          105 -------x New [OPL3 only]
//              ------x- New2 [OPL4 only]
//
//     Per-channel registers (channel in address bits 0-3)
//     Note that all these apply to address+100 as well on OPL3+
//        A0-A8 xxxxxxxx F-number (low 8 bits)
//        B0-B8 --x----- Key on
//              ---xxx-- Block (octvate, 0-7)
//              ------xx F-number (high two bits)
//        C0-C8 x------- CHD output (to DO0 pin) [OPL3+ only]
//              -x------ CHC output (to DO0 pin) [OPL3+ only]
//              --x----- CHB output (mixed right, to DO2 pin) [OPL3+ only]
//              ---x---- CHA output (mixed left, to DO2 pin) [OPL3+ only]
//              ----xxx- Feedback level for operator 1 (0-7)
//              -------x Operator connection algorithm
//
//     Per-operator registers (operator in bits 0-5)
//     Note that all these apply to address+100 as well on OPL3+
//        20-35 x------- AM enable
//              -x------ PM enable (VIB)
//              --x----- EG type
//              ---x---- Key scale rate
//              ----xxxx Multiple value (0-15)
//        40-55 xx------ Key scale level (0-3)
//              --xxxxxx Total level (0-63)
//        60-75 xxxx---- Attack rate (0-15)
//              ----xxxx Decay rate (0-15)
//        80-95 xxxx---- Sustain level (0-15)
//              ----xxxx Release rate (0-15)
//        E0-F5 ------xx Wave select (0-3) [OPL2 only]
//              -----xxx Wave select (0-7) [OPL3+ only]
//

// constants
#define OPL_EMU_REGISTERS_OUTPUTS 2
#define OPL_EMU_REGISTERS_CHANNELS 18
#define OPL_EMU_REGISTERS_ALL_CHANNELS ( (1 << OPL_EMU_REGISTERS_CHANNELS) - 1 )
#define OPL_EMU_REGISTERS_OPERATORS ( OPL_EMU_REGISTERS_CHANNELS * 2 )
#define OPL_EMU_REGISTERS_WAVEFORMS 8
#define OPL_EMU_REGISTERS_REGISTERS 0x200
#define OPL_EMU_REGISTERS_REG_MODE 0x04
#define OPL_EMU_REGISTERS_DEFAULT_PRESCALE 8
#define OPL_EMU_REGISTERS_STATUS_BUSY 0

// this value is returned from the write() function for rhythm channels
#define OPL_EMU_REGISTERS_RHYTHM_CHANNEL 0xff

// this is the size of a full sin waveform
#define OPL_EMU_REGISTERS_WAVEFORM_LENGTH 0x400

struct opl_emu_registers
{
	// internal state
	uint16_t m_lfo_am_counter;            // LFO AM counter
	uint16_t m_lfo_pm_counter;            // LFO PM counter
	uint32_t m_noise_lfsr;                // noise LFSR state
	uint8_t m_lfo_am;                     // current LFO AM value
	uint8_t m_regdata[OPL_EMU_REGISTERS_REGISTERS];         // register data
	uint16_t m_waveform[OPL_EMU_REGISTERS_WAVEFORMS][OPL_EMU_REGISTERS_WAVEFORM_LENGTH]; // waveforms
};


// helper to encode four operator numbers into a 32-bit value in the
// operator maps for each register class
uint32_t opl_emu_registers_operator_list(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4)
{
	return o1 | (o2 << 8) | (o3 << 16) | (o4 << 24);
}

// helper to apply KSR to the raw ADSR rate, ignoring ksr if the
// raw value is 0, and clamping to 63
uint32_t opl_emu_registers_effective_rate(uint32_t rawrate, uint32_t ksr)
{
	return (rawrate == 0) ? 0 : opl_min(rawrate + ksr, 63);
}


// constructor
void opl_emu_registers_init( struct opl_emu_registers* regs );

// reset to initial state
void opl_emu_registers_reset(struct opl_emu_registers* regs);

// map channel number to register offset
uint32_t opl_emu_registers_channel_offset(uint32_t chnum)
{
    return (chnum % 9) + 0x100 * (chnum / 9);
}

// map operator number to register offset
uint32_t opl_emu_registers_operator_offset(uint32_t opnum)
{
    return (opnum % 18) + 2 * ((opnum % 18) / 6) + 0x100 * (opnum / 18);
}

// return an array of operator indices for each channel
struct opl_emu_registers_operator_mapping { uint32_t chan[OPL_EMU_REGISTERS_CHANNELS]; };
void opl_emu_registers_operator_map(struct opl_emu_registers* regs,struct opl_emu_registers_operator_mapping* dest) ;

// handle writes to the register array
int opl_emu_registers_write(struct opl_emu_registers* regs,uint16_t index, uint8_t data, uint32_t* chan, uint32_t* opmask);

// clock the noise and LFO, if present, returning LFO PM value
int32_t opl_emu_registers_clock_noise_and_lfo(struct opl_emu_registers* regs);

// reset the LFO
void opl_emu_registers_reset_lfo(struct opl_emu_registers* regs) { regs->m_lfo_am_counter = regs->m_lfo_pm_counter = 0; }

// return the AM offset from LFO for the given channel
// on OPL this is just a fixed value
uint32_t opl_emu_registers_lfo_am_offset(struct opl_emu_registers* regs,uint32_t choffs)  { return regs->m_lfo_am; }

// return LFO/noise states
uint32_t opl_emu_registers_noise_state(struct opl_emu_registers* regs)  { return regs->m_noise_lfsr >> 23; }

// caching helpers
void opl_emu_registers_cache_operator_data(struct opl_emu_registers* regs,uint32_t choffs, uint32_t opoffs, struct opl_emu_opdata_cache *cache);

// compute the phase step, given a PM value
uint32_t opl_emu_registers_compute_phase_step(struct opl_emu_registers* regs,uint32_t choffs, uint32_t opoffs, struct opl_emu_opdata_cache const *cache, int32_t lfo_raw_pm);

// return a bitfield extracted from a byte
uint32_t opl_emu_registers_byte(struct opl_emu_registers* regs,uint32_t offset, uint32_t start, uint32_t count, uint32_t extra_offset/* = 0*/) 
{
	return opl_emu_bitfield(regs->m_regdata[offset + extra_offset], start, count);
}

// return a bitfield extracted from a pair of bytes, MSBs listed first
uint32_t opl_emu_registers_word(struct opl_emu_registers* regs,uint32_t offset1, uint32_t start1, uint32_t count1, uint32_t offset2, uint32_t start2, uint32_t count2, uint32_t extra_offset/* = 0*/) 
{
	return (opl_emu_registers_byte(regs,offset1, start1, count1, extra_offset) << count2) | opl_emu_registers_byte(regs,offset2, start2, count2, extra_offset);
}



// system-wide registers
uint32_t opl_emu_registers_timer_a_value(struct opl_emu_registers* regs)                    { return opl_emu_registers_byte(regs,0x02, 0, 8, 0) * 4; } // 8->10 bits
uint32_t opl_emu_registers_timer_b_value(struct opl_emu_registers* regs)                    { return opl_emu_registers_byte(regs,0x03, 0, 8, 0); }
uint32_t opl_emu_registers_status_mask(struct opl_emu_registers* regs)                      { return opl_emu_registers_byte(regs,0x04, 0, 8, 0) & 0x78; }
uint32_t opl_emu_registers_note_select(struct opl_emu_registers* regs)                      { return opl_emu_registers_byte(regs,0x08, 6, 1, 0); }
uint32_t opl_emu_registers_lfo_am_depth(struct opl_emu_registers* regs)                     { return opl_emu_registers_byte(regs,0xbd, 7, 1, 0); }
uint32_t opl_emu_registers_lfo_pm_depth(struct opl_emu_registers* regs)                     { return opl_emu_registers_byte(regs,0xbd, 6, 1, 0); }
uint32_t opl_emu_registers_rhythm_enable(struct opl_emu_registers* regs)                    { return opl_emu_registers_byte(regs,0xbd, 5, 1, 0); }
uint32_t opl_emu_registers_newflag(struct opl_emu_registers* regs)                          { return opl_emu_registers_byte(regs,0x105, 0, 1, 0); }
uint32_t opl_emu_registers_fourop_enable(struct opl_emu_registers* regs)                    { return opl_emu_registers_byte(regs,0x104, 0, 6, 0); }

// per-channel registers
uint32_t opl_emu_registers_ch_block_freq(struct opl_emu_registers* regs,uint32_t choffs)     { return opl_emu_registers_word(regs,0xb0, 0, 5, 0xa0, 0, 8, choffs); }
uint32_t opl_emu_registers_ch_feedback(struct opl_emu_registers* regs,uint32_t choffs)       { return opl_emu_registers_byte(regs,0xc0, 1, 3, choffs); }
uint32_t opl_emu_registers_ch_algorithm(struct opl_emu_registers* regs,uint32_t choffs)      { return opl_emu_registers_byte(regs,0xc0, 0, 1, choffs) | ((8 | (opl_emu_registers_byte(regs,0xc3, 0, 1, choffs) << 1))); }
uint32_t opl_emu_registers_ch_output_any(struct opl_emu_registers* regs,uint32_t choffs)     { return opl_emu_registers_newflag(regs) ? opl_emu_registers_byte(regs,0xc0 + choffs, 4, 4, 0) : 1; }
uint32_t opl_emu_registers_ch_output_0(struct opl_emu_registers* regs,uint32_t choffs)       { return opl_emu_registers_newflag(regs) ? opl_emu_registers_byte(regs,0xc0 + choffs, 4, 1, 0) : 1; }
uint32_t opl_emu_registers_ch_output_1(struct opl_emu_registers* regs,uint32_t choffs)       { return opl_emu_registers_newflag(regs) ? opl_emu_registers_byte(regs,0xc0 + choffs, 5, 1, 0) : 1; }
uint32_t opl_emu_registers_ch_output_2(struct opl_emu_registers* regs,uint32_t choffs)       { return opl_emu_registers_newflag(regs) ? opl_emu_registers_byte(regs,0xc0 + choffs, 6, 1, 0) : 0; }
uint32_t opl_emu_registers_ch_output_3(struct opl_emu_registers* regs,uint32_t choffs)       { return opl_emu_registers_newflag(regs) ? opl_emu_registers_byte(regs,0xc0 + choffs, 7, 1, 0) : 0; }

// per-operator registers
uint32_t opl_emu_registers_op_lfo_am_enable(struct opl_emu_registers* regs,uint32_t opoffs)  { return opl_emu_registers_byte(regs,0x20, 7, 1, opoffs); }
uint32_t opl_emu_registers_op_lfo_pm_enable(struct opl_emu_registers* regs,uint32_t opoffs)  { return opl_emu_registers_byte(regs,0x20, 6, 1, opoffs); }
uint32_t opl_emu_registers_op_eg_sustain(struct opl_emu_registers* regs,uint32_t opoffs)     { return opl_emu_registers_byte(regs,0x20, 5, 1, opoffs); }
uint32_t opl_emu_registers_op_ksr(struct opl_emu_registers* regs,uint32_t opoffs)            { return opl_emu_registers_byte(regs,0x20, 4, 1, opoffs); }
uint32_t opl_emu_registers_op_multiple(struct opl_emu_registers* regs,uint32_t opoffs)       { return opl_emu_registers_byte(regs,0x20, 0, 4, opoffs); }
uint32_t opl_emu_registers_op_ksl(struct opl_emu_registers* regs,uint32_t opoffs)            { uint32_t temp = opl_emu_registers_byte(regs,0x40, 6, 2, opoffs); return opl_emu_bitfield(temp, 1,1) | (opl_emu_bitfield(temp, 0,1) << 1); }
uint32_t opl_emu_registers_op_total_level(struct opl_emu_registers* regs,uint32_t opoffs)    { return opl_emu_registers_byte(regs,0x40, 0, 6, opoffs); }
uint32_t opl_emu_registers_op_attack_rate(struct opl_emu_registers* regs,uint32_t opoffs)    { return opl_emu_registers_byte(regs,0x60, 4, 4, opoffs); }
uint32_t opl_emu_registers_op_decay_rate(struct opl_emu_registers* regs,uint32_t opoffs)     { return opl_emu_registers_byte(regs,0x60, 0, 4, opoffs); }
uint32_t opl_emu_registers_op_sustain_level(struct opl_emu_registers* regs,uint32_t opoffs)  { return opl_emu_registers_byte(regs,0x80, 4, 4, opoffs); }
uint32_t opl_emu_registers_op_release_rate(struct opl_emu_registers* regs,uint32_t opoffs)   { return opl_emu_registers_byte(regs,0x80, 0, 4, opoffs); }
uint32_t opl_emu_registers_op_waveform(struct opl_emu_registers* regs,uint32_t opoffs)       { return opl_emu_registers_byte(regs,0xe0, 0, opl_emu_registers_newflag(regs) ? 3 : 2, opoffs); }


// helper to determine if the this channel is an active rhythm channel
int opl_emu_registers_is_rhythm(struct opl_emu_registers* regs,uint32_t choffs) 
{
	return opl_emu_registers_rhythm_enable(regs) && (choffs >= 6 && choffs <= 8);
}

// ======================> opl_emu_fm_operator

// "quiet" value, used to optimize when we can skip doing working
#define OPL_EMU_FM_OPERATOR_EG_QUIET 0x200

// opl_emu_fm_operator represents an FM operator (or "slot" in FM parlance), which
// produces an output sine wave modulated by an envelope
struct opl_emu_fm_operator
{
	// internal state
	uint32_t m_choffs;                     // channel offset in registers
	uint32_t m_opoffs;                     // operator offset in registers
	uint32_t m_phase;                      // current phase value (10.10 format)
	uint16_t m_env_attenuation;            // computed envelope attenuation (4.6 format)
	enum opl_emu_envelope_state m_env_state;            // current envelope state
	uint8_t m_key_state;                   // current key state: on or off (bit 0)
	uint8_t m_keyon_live;                  // live key on state (bit 0 = direct, bit 1 = rhythm, bit 2 = CSM)
	struct opl_emu_opdata_cache m_cache;                  // cached values for performance
	struct opl_emu_registers* m_regs;                  // direct reference to registers
};

void opl_emu_fm_operator_init(struct opl_emu_fm_operator* fmop, struct opl_emu_registers* regs, uint32_t opoffs);

// reset the operator state
void opl_emu_fm_operator_reset(struct opl_emu_fm_operator* fmop);

// return the operator/channel offset
uint32_t opl_emu_fm_operator_opoffs(struct opl_emu_fm_operator* fmop)  { return fmop->m_opoffs; }
uint32_t opl_emu_fm_operator_choffs(struct opl_emu_fm_operator* fmop)  { return fmop->m_choffs; }

// set the current channel
void opl_emu_fm_operator_set_choffs(struct opl_emu_fm_operator* fmop,uint32_t choffs) { fmop->m_choffs = choffs; }

// prepare prior to clocking
int opl_emu_fm_operator_prepare(struct opl_emu_fm_operator* fmop);

// master clocking function
void opl_emu_fm_operator_clock(struct opl_emu_fm_operator* fmop,uint32_t env_counter, int32_t lfo_raw_pm);

// return the current phase value
uint32_t opl_emu_fm_operator_phase(struct opl_emu_fm_operator* fmop)  { return fmop->m_phase >> 10; }

// compute operator volume
int32_t opl_emu_fm_operator_compute_volume(struct opl_emu_fm_operator* fmop,uint32_t phase, uint32_t am_offset) ;

// key state control
void opl_emu_fm_operator_keyonoff(struct opl_emu_fm_operator* fmop,uint32_t on, enum opl_emu_keyon_type type);

// start the attack phase
void opl_emu_fm_operator_start_attack(struct opl_emu_fm_operator* fmop);

// start the release phase
void opl_emu_fm_operator_start_release(struct opl_emu_fm_operator* fmop);

// clock phases
void opl_emu_fm_operator_clock_keystate(struct opl_emu_fm_operator* fmop,uint32_t keystate);
void opl_emu_fm_operator_clock_envelope(struct opl_emu_fm_operator* fmop,uint32_t env_counter);
void opl_emu_fm_operator_clock_phase(struct opl_emu_fm_operator* fmop,int32_t lfo_raw_pm);

// return effective attenuation of the envelope
uint32_t opl_emu_fm_operator_envelope_attenuation(struct opl_emu_fm_operator* fmop,uint32_t am_offset) ;



// ======================> opl_emu_fm_channel

// opl_emu_fm_channel represents an FM channel which combines the output of 2 or 4
// operators into a final result
struct opl_emu_fm_channel
{
	// internal state
	uint32_t m_choffs;                     // channel offset in registers
	int16_t m_feedback[2];                 // feedback memory for operator 1
	int16_t m_feedback_in;         // next input value for op 1 feedback (set in output)
	struct opl_emu_fm_operator *m_op[4];    // up to 4 operators
	struct opl_emu_registers* m_regs;                  // direct reference to registers
};


void opl_emu_fm_channel_init(struct opl_emu_fm_channel* fmch,struct opl_emu_registers* regs, uint32_t choffs);

// reset the channel state
void opl_emu_fm_channel_reset(struct opl_emu_fm_channel* fmch);

// return the channel offset
uint32_t opl_emu_fm_channel_choffs(struct opl_emu_fm_channel* fmch) { return fmch->m_choffs; }

// assign operators
void opl_emu_fm_channel_assign(struct opl_emu_fm_channel* fmch,uint32_t index, struct opl_emu_fm_operator *op)
{
	fmch->m_op[index] = op;
	if (op != NULL)
		opl_emu_fm_operator_set_choffs(op, fmch->m_choffs);
}

// signal key on/off to our operators
void opl_emu_fm_channel_keyonoff(struct opl_emu_fm_channel* fmch,uint32_t states, enum opl_emu_keyon_type type, uint32_t chnum);

// prepare prior to clocking
int opl_emu_fm_channel_prepare(struct opl_emu_fm_channel* fmch);

// master clocking function
void opl_emu_fm_channel_clock(struct opl_emu_fm_channel* fmch,uint32_t env_counter, int32_t lfo_raw_pm);

// specific 2-operator and 4-operator output handlers
void opl_emu_fm_channel_output_2op(struct opl_emu_fm_channel* fmch,short *output, uint32_t rshift, int32_t clipmax);
void opl_emu_fm_channel_output_4op(struct opl_emu_fm_channel* fmch,short *output, uint32_t rshift, int32_t clipmax);

// compute the special OPL rhythm channel outputs
void opl_emu_fm_channel_output_rhythm_ch6(struct opl_emu_fm_channel* fmch,short *output, uint32_t rshift, int32_t clipmax);
void opl_emu_fm_channel_output_rhythm_ch7(struct opl_emu_fm_channel* fmch,uint32_t phase_select,short *output, uint32_t rshift, int32_t clipmax);
void opl_emu_fm_channel_output_rhythm_ch8(struct opl_emu_fm_channel* fmch,uint32_t phase_select,short *output, uint32_t rshift, int32_t clipmax);

// are we a 4-operator channel or a 2-operator one?
int opl_emu_fm_channel_is4op( struct opl_emu_fm_channel* fmch )
{
    return (fmch->m_op[2] != NULL);
}

// helper to add values to the outputs based on channel enables
void opl_emu_fm_channel_add_to_output(struct opl_emu_fm_channel* fmch,uint32_t choffs,short* output, int32_t value)
{
	// create these constants to appease overzealous compilers checking array
	// bounds in unreachable code (looking at you, clang)
	int out0_index = 0;
	int out1_index = 1 % OPL_EMU_REGISTERS_OUTPUTS;
	int out2_index = 2 % OPL_EMU_REGISTERS_OUTPUTS;
	int out3_index = 3 % OPL_EMU_REGISTERS_OUTPUTS;

	if (OPL_EMU_REGISTERS_OUTPUTS == 1 || opl_emu_registers_ch_output_0(fmch->m_regs, choffs)) {
        int s = output[out0_index] + value;
		output[out0_index] = s < -32767 ? -32767 : s > 32767 ? 32767 : s;
    }
	if (OPL_EMU_REGISTERS_OUTPUTS >= 2 && opl_emu_registers_ch_output_1(fmch->m_regs, choffs)) {
        int s = output[out1_index] + value;
		output[out1_index] = s < -32767 ? -32767 : s > 32767 ? 32767 : s;
    }
	//if (OPL_EMU_REGISTERS_OUTPUTS >= 3 && opl_emu_registers_ch_output_2(fmch->m_regs, choffs))
	//	output->data[out2_index] += value;
	//if (OPL_EMU_REGISTERS_OUTPUTS >= 4 && opl_emu_registers_ch_output_3(fmch->m_regs, choffs))
	//	output->data[out3_index] += value;
}




// ======================> ymf262

// ymf262 represents a set of operators and channels which together
// form a Yamaha FM core; chips that implement other engines (ADPCM, wavetable,
// etc) take this output and combine it with the others externally
struct opl_emu_t
{
	uint32_t m_env_counter;          // envelope counter; low 2 bits are sub-counter
	uint8_t m_status;                // current status register
	uint8_t m_timer_running[2];      // current timer running state
	uint32_t m_active_channels;      // mask of active channels (computed by prepare)
	uint32_t m_modified_channels;    // mask of channels that have been modified
	uint32_t m_prepare_count;        // counter to do periodic prepare sweeps
	struct opl_emu_registers m_regs;             // register accessor
	struct opl_emu_fm_channel m_channel[OPL_EMU_REGISTERS_CHANNELS]; // channel pointers
	struct opl_emu_fm_operator m_operator[OPL_EMU_REGISTERS_OPERATORS]; // operator pointers
};




//*********************************************************
//  YMF262
//*********************************************************

// set/reset bits in the status register, updating the IRQ status
uint8_t opl_emu_set_reset_status(struct opl_emu_t* emu, uint8_t set, uint8_t reset)
{
	emu->m_status = (emu->m_status | set) & ~(reset | OPL_EMU_REGISTERS_STATUS_BUSY);
	//m_intf.opl_emu_sync_check_interrupts();
	return emu->m_status & ~opl_emu_registers_status_mask(&emu->m_regs);
}

void opl_emu_assign_operators( struct opl_emu_t* emu );
void opl_emu_write( struct opl_emu_t* emu, uint16_t regnum, uint8_t data);

//-------------------------------------------------
//  ymf262 - constructor
//-------------------------------------------------

void opl_emu_init( struct opl_emu_t* emu ) 
{
	emu->m_env_counter = 0;
	emu->m_status = 0;
	emu->m_timer_running[ 0 ] = 0;
	emu->m_timer_running[ 1 ] = 0;
	emu->m_active_channels = OPL_EMU_REGISTERS_ALL_CHANNELS;
	emu->m_modified_channels = OPL_EMU_REGISTERS_ALL_CHANNELS;
	emu->m_prepare_count = 0;

	opl_emu_registers_init( &emu->m_regs );

	// create the channels
	for (uint32_t chnum = 0; chnum < OPL_EMU_REGISTERS_CHANNELS; chnum++)
		opl_emu_fm_channel_init(&emu->m_channel[chnum], &emu->m_regs, opl_emu_registers_channel_offset(chnum));

	// create the operators
	for (uint32_t opnum = 0; opnum < OPL_EMU_REGISTERS_OPERATORS; opnum++)
		opl_emu_fm_operator_init(&emu->m_operator[opnum],&emu->m_regs, opl_emu_registers_operator_offset(opnum));

	// do the initial operator assignment
	opl_emu_assign_operators( emu );
}


//-------------------------------------------------
//  reset - reset the overall state
//-------------------------------------------------

void opl_emu_reset( struct opl_emu_t* emu)
{
	// reset all status bits
	opl_emu_set_reset_status(emu, 0, 0xff);

	// register type-specific initialization
	opl_emu_registers_reset( &emu->m_regs );

	// explicitly write to the mode register since it has side-effects
	opl_emu_write(emu, OPL_EMU_REGISTERS_REG_MODE, 0);

	// reset the channels
	for (int i = 0; i < sizeof( emu->m_channel ) / sizeof( *emu->m_channel ); ++i )
		opl_emu_fm_channel_reset(&emu->m_channel[ i ]);

	// reset the operators
	for (int i = 0; i < sizeof( emu->m_operator ) / sizeof( *emu->m_operator ); ++i )
		opl_emu_fm_operator_reset(&emu->m_operator[ i ]);
}


//-------------------------------------------------
//  clock - iterate over all channels, clocking
//  them forward one step
//-------------------------------------------------

uint32_t opl_emu_clock( struct opl_emu_t* emu,uint32_t chanmask)
{
	// if something was modified, prepare
	// also prepare every 4k samples to catch ending notes
	if (emu->m_modified_channels != 0 || emu->m_prepare_count++ >= 4096)
	{
		// reassign operators to channels if dynamic
        opl_emu_assign_operators(emu);

		// call each channel to prepare
		emu->m_active_channels = 0;
		for (uint32_t chnum = 0; chnum < OPL_EMU_REGISTERS_CHANNELS; chnum++)
			if (opl_emu_bitfield(chanmask, chnum,1))
				if (opl_emu_fm_channel_prepare(&emu->m_channel[chnum]))
					emu->m_active_channels |= 1 << chnum;

		// reset the modified channels and prepare count
		emu->m_modified_channels = emu->m_prepare_count = 0;
	}

	// if the envelope clock divider is 1, just increment by 4;
    emu->m_env_counter += 4;

	// clock the noise generator
	int32_t lfo_raw_pm = opl_emu_registers_clock_noise_and_lfo(&emu->m_regs);

	// now update the state of all the channels and operators
	for (uint32_t chnum = 0; chnum < OPL_EMU_REGISTERS_CHANNELS; chnum++)
		if (opl_emu_bitfield(chanmask, chnum, 1))
			opl_emu_fm_channel_clock(&emu->m_channel[chnum], emu->m_env_counter, lfo_raw_pm);


	// return the envelope counter as it is used to clock ADPCM-A
	return emu->m_env_counter;
}


//-------------------------------------------------
//  output - compute a sum over the relevant
//  channels
//-------------------------------------------------

void opl_emu_out( struct opl_emu_t* emu, short* output, uint32_t rshift, int32_t clipmax, uint32_t chanmask)
{
	// mask out inactive channels
	chanmask &= emu->m_active_channels;

	// handle the rhythm case, where some of the operators are dedicated
	// to percussion (this is an OPL-specific feature)
	if (opl_emu_registers_rhythm_enable(&emu->m_regs))
	{
		// precompute the operator 13+17 phase selection value
		uint32_t op13phase = opl_emu_fm_operator_phase(&emu->m_operator[13]);
		uint32_t op17phase = opl_emu_fm_operator_phase(&emu->m_operator[17]);
		uint32_t phase_select = (opl_emu_bitfield(op13phase, 2, 1) ^ opl_emu_bitfield(op13phase, 7, 1)) | opl_emu_bitfield(op13phase, 3,1) | (opl_emu_bitfield(op17phase, 5,1) ^ opl_emu_bitfield(op17phase, 3,1));

		// sum over all the desired channels
		for (uint32_t chnum = 0; chnum < OPL_EMU_REGISTERS_CHANNELS; chnum++)
			if (opl_emu_bitfield(chanmask, chnum,1))
			{
				if (chnum == 6)
					opl_emu_fm_channel_output_rhythm_ch6(&emu->m_channel[chnum],output, rshift, clipmax);
				else if (chnum == 7)
					opl_emu_fm_channel_output_rhythm_ch7(&emu->m_channel[chnum],phase_select, output, rshift, clipmax);
				else if (chnum == 8)
					opl_emu_fm_channel_output_rhythm_ch8(&emu->m_channel[chnum],phase_select, output, rshift, clipmax);
				else if (opl_emu_fm_channel_is4op(&emu->m_channel[chnum]))
					opl_emu_fm_channel_output_4op(&emu->m_channel[chnum],output, rshift, clipmax);
				else
					opl_emu_fm_channel_output_2op(&emu->m_channel[chnum],output, rshift, clipmax);
			}
	}
	else
	{
		// sum over all the desired channels
		for (uint32_t chnum = 0; chnum < OPL_EMU_REGISTERS_CHANNELS; chnum++)
			if (opl_emu_bitfield(chanmask, chnum,1))
			{
				if (opl_emu_fm_channel_is4op(&emu->m_channel[chnum]))
					opl_emu_fm_channel_output_4op(&emu->m_channel[chnum],output, rshift, clipmax);
				else
					opl_emu_fm_channel_output_2op(&emu->m_channel[chnum],output, rshift, clipmax);
			}
	}
}


//-------------------------------------------------
//  write - handle writes to the OPN registers
//-------------------------------------------------

void opl_emu_write( struct opl_emu_t* emu, uint16_t regnum, uint8_t data)
{
	// special case: writes to the mode register can impact IRQs;
	// schedule these writes to ensure ordering with timers
	if (regnum == OPL_EMU_REGISTERS_REG_MODE)
	{
//		emu->m_intf.opl_emu_sync_mode_write(data);
		return;
	}

	// for now just mark all channels as modified
	emu->m_modified_channels = OPL_EMU_REGISTERS_ALL_CHANNELS;

	// most writes are passive, consumed only when needed
	uint32_t keyon_channel;
	uint32_t keyon_opmask;
	if (opl_emu_registers_write(&emu->m_regs,regnum, data, &keyon_channel, &keyon_opmask))
	{
		// handle writes to the keyon register(s)
		if (keyon_channel < OPL_EMU_REGISTERS_CHANNELS)
		{
			// normal channel on/off
			opl_emu_fm_channel_keyonoff(&emu->m_channel[keyon_channel],keyon_opmask, OPL_EMU_KEYON_NORMAL, keyon_channel);
		}
		else if (OPL_EMU_REGISTERS_CHANNELS >= 9 && keyon_channel == OPL_EMU_REGISTERS_RHYTHM_CHANNEL)
		{
			// special case for the OPL rhythm channels
			opl_emu_fm_channel_keyonoff(&emu->m_channel[6],opl_emu_bitfield(keyon_opmask, 4,1) ? 3 : 0, OPL_EMU_KEYON_RHYTHM, 6);
			opl_emu_fm_channel_keyonoff(&emu->m_channel[7],opl_emu_bitfield(keyon_opmask, 0,1) | (opl_emu_bitfield(keyon_opmask, 3,1) << 1), OPL_EMU_KEYON_RHYTHM, 7);
			opl_emu_fm_channel_keyonoff(&emu->m_channel[8],opl_emu_bitfield(keyon_opmask, 2,1) | (opl_emu_bitfield(keyon_opmask, 1,1) << 1), OPL_EMU_KEYON_RHYTHM, 8);
		}
	}
}


//-------------------------------------------------
//  assign_operators - get the current mapping of
//  operators to channels and assign them all
//-------------------------------------------------

void opl_emu_assign_operators( struct opl_emu_t* emu)
{
	struct opl_emu_registers_operator_mapping map;
	opl_emu_registers_operator_map(&emu->m_regs, &map);

	for (uint32_t chnum = 0; chnum < OPL_EMU_REGISTERS_CHANNELS; chnum++)
		for (uint32_t index = 0; index < 4; index++)
		{
			uint32_t opnum = opl_emu_bitfield(map.chan[chnum], 8 * index, 8);
			opl_emu_fm_channel_assign(&emu->m_channel[chnum],index, (opnum == 0xff) ? NULL : &emu->m_operator[opnum]);
		}
}


//-------------------------------------------------
//  update_timer - update the state of the given
//  timer
//-------------------------------------------------

void opl_emu_update_timer( struct opl_emu_t* emu, uint32_t tnum, uint32_t enable)
{
	// if the timer is live, but not currently enabled, set the timer
	if (enable && !emu->m_timer_running[tnum])
	{
		// period comes from the registers, and is different for each
		uint32_t period = (tnum == 0) ? (1024 - opl_emu_registers_timer_a_value(&emu->m_regs)) : 16 * (256 - opl_emu_registers_timer_b_value(&emu->m_regs));

		// reset it
		//emu->m_intf.opl_emu_set_timer(tnum, period * OPERATORS * emu->m_clock_prescale);
		emu->m_timer_running[tnum] = 1;
	}

	// if the timer is not live, ensure it is not enabled
	else if (!enable)
	{
		//emu->m_intf.opl_emu_set_timer(tnum, -1);
		emu->m_timer_running[tnum] = 0;
	}
}


//-------------------------------------------------
//  generate - generate samples of sound
//-------------------------------------------------

void opl_emu_generate( struct opl_emu_t* emu,short *output, uint32_t numsamples, float volume )
{
	volume = volume > 1.0f ? 1.0f : volume < 0.0f ? 0.0f : volume;
	for (uint32_t samp = 0; samp < numsamples; samp++, output+=2)
	{
		// clock the system
		opl_emu_clock(emu, OPL_EMU_REGISTERS_ALL_CHANNELS);

		// update the FM content; mixing details for YMF262 need verification
		opl_emu_out(emu, output, 0, 32767, OPL_EMU_REGISTERS_ALL_CHANNELS);
        
        *output = (short)((*output) * volume);
        *(output + 1) = (short)((*(output + 1)) * volume);
	}
}



//-------------------------------------------------
//  opl_emu_opl_key_scale_atten - converts an
//  OPL concatenated block (3 bits) and fnum
//  (10 bits) into an attenuation offset; values
//  here are for 6dB/octave, in 0.75dB units
//  (matching total level LSB)
//-------------------------------------------------

uint32_t opl_emu_opl_key_scale_atten(uint32_t block, uint32_t fnum_4msb)
{
	// this table uses the top 4 bits of FNUM and are the maximal values
	// (for when block == 7). Values for other blocks can be computed by
	// subtracting 8 for each block below 7.
	static uint8_t const fnum_to_atten[16] = { 0,24,32,37,40,43,45,47,48,50,51,52,53,54,55,56 };
	int32_t result = fnum_to_atten[fnum_4msb] - 8 * (block ^ 7);
	return opl_max(0, result);
}



//*********************************************************
//  GLOBAL TABLE LOOKUPS
//*********************************************************

//-------------------------------------------------
//  opl_emu_abs_sin_attenuation - given a sin (phase) input
//  where the range 0-2*PI is mapped onto 10 bits,
//  return the absolute value of sin(input),
//  logarithmically-adjusted and treated as an
//  attenuation value, in 4.8 fixed point format
//-------------------------------------------------

uint32_t opl_emu_abs_sin_attenuation(uint32_t input)
{
	// the values here are stored as 4.8 logarithmic values for 1/4 phase
	// this matches the internal format of the OPN chip, extracted from the die
	static uint16_t const s_sin_table[256] =
	{
		0x859,0x6c3,0x607,0x58b,0x52e,0x4e4,0x4a6,0x471,0x443,0x41a,0x3f5,0x3d3,0x3b5,0x398,0x37e,0x365,
		0x34e,0x339,0x324,0x311,0x2ff,0x2ed,0x2dc,0x2cd,0x2bd,0x2af,0x2a0,0x293,0x286,0x279,0x26d,0x261,
		0x256,0x24b,0x240,0x236,0x22c,0x222,0x218,0x20f,0x206,0x1fd,0x1f5,0x1ec,0x1e4,0x1dc,0x1d4,0x1cd,
		0x1c5,0x1be,0x1b7,0x1b0,0x1a9,0x1a2,0x19b,0x195,0x18f,0x188,0x182,0x17c,0x177,0x171,0x16b,0x166,
		0x160,0x15b,0x155,0x150,0x14b,0x146,0x141,0x13c,0x137,0x133,0x12e,0x129,0x125,0x121,0x11c,0x118,
		0x114,0x10f,0x10b,0x107,0x103,0x0ff,0x0fb,0x0f8,0x0f4,0x0f0,0x0ec,0x0e9,0x0e5,0x0e2,0x0de,0x0db,
		0x0d7,0x0d4,0x0d1,0x0cd,0x0ca,0x0c7,0x0c4,0x0c1,0x0be,0x0bb,0x0b8,0x0b5,0x0b2,0x0af,0x0ac,0x0a9,
		0x0a7,0x0a4,0x0a1,0x09f,0x09c,0x099,0x097,0x094,0x092,0x08f,0x08d,0x08a,0x088,0x086,0x083,0x081,
		0x07f,0x07d,0x07a,0x078,0x076,0x074,0x072,0x070,0x06e,0x06c,0x06a,0x068,0x066,0x064,0x062,0x060,
		0x05e,0x05c,0x05b,0x059,0x057,0x055,0x053,0x052,0x050,0x04e,0x04d,0x04b,0x04a,0x048,0x046,0x045,
		0x043,0x042,0x040,0x03f,0x03e,0x03c,0x03b,0x039,0x038,0x037,0x035,0x034,0x033,0x031,0x030,0x02f,
		0x02e,0x02d,0x02b,0x02a,0x029,0x028,0x027,0x026,0x025,0x024,0x023,0x022,0x021,0x020,0x01f,0x01e,
		0x01d,0x01c,0x01b,0x01a,0x019,0x018,0x017,0x017,0x016,0x015,0x014,0x014,0x013,0x012,0x011,0x011,
		0x010,0x00f,0x00f,0x00e,0x00d,0x00d,0x00c,0x00c,0x00b,0x00a,0x00a,0x009,0x009,0x008,0x008,0x007,
		0x007,0x007,0x006,0x006,0x005,0x005,0x005,0x004,0x004,0x004,0x003,0x003,0x003,0x002,0x002,0x002,
		0x002,0x001,0x001,0x001,0x001,0x001,0x001,0x001,0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000
	};

	// if the top bit is set, we're in the second half of the curve
	// which is a mirror image, so invert the index
	if (opl_emu_bitfield(input, 8,1))
		input = ~input;

	// return the value from the table
	return s_sin_table[input & 0xff];
}


//-------------------------------------------------
//  opl_emu_attenuation_to_volume - given a 5.8 fixed point
//  logarithmic attenuation value, return a 13-bit
//  linear volume
//-------------------------------------------------

uint32_t opl_emu_attenuation_to_volume(uint32_t input)
{
	// the values here are 10-bit mantissas with an implied leading bit
	// this matches the internal format of the OPN chip, extracted from the die

	// as a nod to performance, the implicit 0x400 bit is pre-incorporated, and
	// the values are left-shifted by 2 so that a simple right shift is all that
	// is needed; also the order is reversed to save a NOT on the input
#define X(a) (((a) | 0x400) << 2)
	static uint16_t const s_power_table[256] =
	{
		X(0x3fa),X(0x3f5),X(0x3ef),X(0x3ea),X(0x3e4),X(0x3df),X(0x3da),X(0x3d4),
		X(0x3cf),X(0x3c9),X(0x3c4),X(0x3bf),X(0x3b9),X(0x3b4),X(0x3ae),X(0x3a9),
		X(0x3a4),X(0x39f),X(0x399),X(0x394),X(0x38f),X(0x38a),X(0x384),X(0x37f),
		X(0x37a),X(0x375),X(0x370),X(0x36a),X(0x365),X(0x360),X(0x35b),X(0x356),
		X(0x351),X(0x34c),X(0x347),X(0x342),X(0x33d),X(0x338),X(0x333),X(0x32e),
		X(0x329),X(0x324),X(0x31f),X(0x31a),X(0x315),X(0x310),X(0x30b),X(0x306),
		X(0x302),X(0x2fd),X(0x2f8),X(0x2f3),X(0x2ee),X(0x2e9),X(0x2e5),X(0x2e0),
		X(0x2db),X(0x2d6),X(0x2d2),X(0x2cd),X(0x2c8),X(0x2c4),X(0x2bf),X(0x2ba),
		X(0x2b5),X(0x2b1),X(0x2ac),X(0x2a8),X(0x2a3),X(0x29e),X(0x29a),X(0x295),
		X(0x291),X(0x28c),X(0x288),X(0x283),X(0x27f),X(0x27a),X(0x276),X(0x271),
		X(0x26d),X(0x268),X(0x264),X(0x25f),X(0x25b),X(0x257),X(0x252),X(0x24e),
		X(0x249),X(0x245),X(0x241),X(0x23c),X(0x238),X(0x234),X(0x230),X(0x22b),
		X(0x227),X(0x223),X(0x21e),X(0x21a),X(0x216),X(0x212),X(0x20e),X(0x209),
		X(0x205),X(0x201),X(0x1fd),X(0x1f9),X(0x1f5),X(0x1f0),X(0x1ec),X(0x1e8),
		X(0x1e4),X(0x1e0),X(0x1dc),X(0x1d8),X(0x1d4),X(0x1d0),X(0x1cc),X(0x1c8),
		X(0x1c4),X(0x1c0),X(0x1bc),X(0x1b8),X(0x1b4),X(0x1b0),X(0x1ac),X(0x1a8),
		X(0x1a4),X(0x1a0),X(0x19c),X(0x199),X(0x195),X(0x191),X(0x18d),X(0x189),
		X(0x185),X(0x181),X(0x17e),X(0x17a),X(0x176),X(0x172),X(0x16f),X(0x16b),
		X(0x167),X(0x163),X(0x160),X(0x15c),X(0x158),X(0x154),X(0x151),X(0x14d),
		X(0x149),X(0x146),X(0x142),X(0x13e),X(0x13b),X(0x137),X(0x134),X(0x130),
		X(0x12c),X(0x129),X(0x125),X(0x122),X(0x11e),X(0x11b),X(0x117),X(0x114),
		X(0x110),X(0x10c),X(0x109),X(0x106),X(0x102),X(0x0ff),X(0x0fb),X(0x0f8),
		X(0x0f4),X(0x0f1),X(0x0ed),X(0x0ea),X(0x0e7),X(0x0e3),X(0x0e0),X(0x0dc),
		X(0x0d9),X(0x0d6),X(0x0d2),X(0x0cf),X(0x0cc),X(0x0c8),X(0x0c5),X(0x0c2),
		X(0x0be),X(0x0bb),X(0x0b8),X(0x0b5),X(0x0b1),X(0x0ae),X(0x0ab),X(0x0a8),
		X(0x0a4),X(0x0a1),X(0x09e),X(0x09b),X(0x098),X(0x094),X(0x091),X(0x08e),
		X(0x08b),X(0x088),X(0x085),X(0x082),X(0x07e),X(0x07b),X(0x078),X(0x075),
		X(0x072),X(0x06f),X(0x06c),X(0x069),X(0x066),X(0x063),X(0x060),X(0x05d),
		X(0x05a),X(0x057),X(0x054),X(0x051),X(0x04e),X(0x04b),X(0x048),X(0x045),
		X(0x042),X(0x03f),X(0x03c),X(0x039),X(0x036),X(0x033),X(0x030),X(0x02d),
		X(0x02a),X(0x028),X(0x025),X(0x022),X(0x01f),X(0x01c),X(0x019),X(0x016),
		X(0x014),X(0x011),X(0x00e),X(0x00b),X(0x008),X(0x006),X(0x003),X(0x000)
	};
#undef X

	// look up the fractional part, then shift by the whole
	return s_power_table[input & 0xff] >> (input >> 8);
}



//-------------------------------------------------
//  opl_emu_attenuation_increment - given a 6-bit ADSR
//  rate value and a 3-bit stepping index,
//  return a 4-bit increment to the attenutaion
//  for this step (or for the attack case, the
//  fractional scale factor to decrease by)
//-------------------------------------------------

uint32_t opl_emu_attenuation_increment(uint32_t rate, uint32_t index)
{
	static uint32_t const s_increment_table[64] =
	{
		0x00000000, 0x00000000, 0x10101010, 0x10101010,  // 0-3    (0x00-0x03)
		0x10101010, 0x10101010, 0x11101110, 0x11101110,  // 4-7    (0x04-0x07)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 8-11   (0x08-0x0B)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 12-15  (0x0C-0x0F)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 16-19  (0x10-0x13)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 20-23  (0x14-0x17)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 24-27  (0x18-0x1B)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 28-31  (0x1C-0x1F)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 32-35  (0x20-0x23)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 36-39  (0x24-0x27)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 40-43  (0x28-0x2B)
		0x10101010, 0x10111010, 0x11101110, 0x11111110,  // 44-47  (0x2C-0x2F)
		0x11111111, 0x21112111, 0x21212121, 0x22212221,  // 48-51  (0x30-0x33)
		0x22222222, 0x42224222, 0x42424242, 0x44424442,  // 52-55  (0x34-0x37)
		0x44444444, 0x84448444, 0x84848484, 0x88848884,  // 56-59  (0x38-0x3B)
		0x88888888, 0x88888888, 0x88888888, 0x88888888   // 60-63  (0x3C-0x3F)
	};
	return opl_emu_bitfield(s_increment_table[rate], 4*index, 4);
}


//*********************************************************
//  OPL REGISTERS
//*********************************************************

//-------------------------------------------------
//  opl_emu_registers - constructor
//-------------------------------------------------

void opl_emu_registers_init(struct opl_emu_registers* regs) 
{
	regs->m_lfo_am_counter = 0;
	regs->m_lfo_pm_counter = 0;
	regs->m_noise_lfsr = 1;
	regs->m_lfo_am = 0;

	// create these pointers to appease overzealous compilers checking array
	// bounds in unreachable code (looking at you, clang)
	uint16_t *wf0 = &regs->m_waveform[0][0];
	uint16_t *wf1 = &regs->m_waveform[1 % OPL_EMU_REGISTERS_WAVEFORMS][0];
	uint16_t *wf2 = &regs->m_waveform[2 % OPL_EMU_REGISTERS_WAVEFORMS][0];
	uint16_t *wf3 = &regs->m_waveform[3 % OPL_EMU_REGISTERS_WAVEFORMS][0];
	uint16_t *wf4 = &regs->m_waveform[4 % OPL_EMU_REGISTERS_WAVEFORMS][0];
	uint16_t *wf5 = &regs->m_waveform[5 % OPL_EMU_REGISTERS_WAVEFORMS][0];
	uint16_t *wf6 = &regs->m_waveform[6 % OPL_EMU_REGISTERS_WAVEFORMS][0];
	uint16_t *wf7 = &regs->m_waveform[7 % OPL_EMU_REGISTERS_WAVEFORMS][0];

	// create the waveforms
	for (uint32_t index = 0; index < OPL_EMU_REGISTERS_WAVEFORM_LENGTH; index++)
		wf0[index] = opl_emu_abs_sin_attenuation(index) | (opl_emu_bitfield(index, 9,1) << 15);

	if (OPL_EMU_REGISTERS_WAVEFORMS >= 4)
	{
		uint16_t zeroval = wf0[0];
		for (uint32_t index = 0; index < OPL_EMU_REGISTERS_WAVEFORM_LENGTH; index++)
		{
			wf1[index] = opl_emu_bitfield(index, 9,1) ? zeroval : wf0[index];
			wf2[index] = wf0[index] & 0x7fff;
			wf3[index] = opl_emu_bitfield(index, 8,1) ? zeroval : (wf0[index] & 0x7fff);
			if (OPL_EMU_REGISTERS_WAVEFORMS >= 8)
			{
				wf4[index] = opl_emu_bitfield(index, 9,1) ? zeroval : wf0[index * 2];
				wf5[index] = opl_emu_bitfield(index, 9,1) ? zeroval : wf0[(index * 2) & 0x1ff];
				wf6[index] = opl_emu_bitfield(index, 9,1) << 15;
				wf7[index] = (opl_emu_bitfield(index, 9,1) ? (index ^ 0x13ff) : index) << 3;
			}
		}
	}
}


//-------------------------------------------------
//  reset - reset to initial state
//-------------------------------------------------

void opl_emu_registers_reset(struct opl_emu_registers* regs)
{
    for( int i = 0; i < OPL_EMU_REGISTERS_REGISTERS; ++i )
        regs->m_regdata[ i ] = 0;
}


//-------------------------------------------------
//  opl_emu_registers_operator_map - return an array of operator
//  indices for each channel; for OPL this is fixed
//-------------------------------------------------

void opl_emu_registers_operator_map(struct opl_emu_registers* regs, struct opl_emu_registers_operator_mapping* dest)
{
    // OPL3/OPL4 can be configured for 2 or 4 operators
    uint32_t fourop = opl_emu_registers_fourop_enable(regs);

    dest->chan[ 0] = opl_emu_bitfield(fourop, 0,1) ? opl_emu_registers_operator_list(  0,  3,  6,  9 ) : opl_emu_registers_operator_list(  0,  3, 0xff, 0xff );
    dest->chan[ 1] = opl_emu_bitfield(fourop, 1,1) ? opl_emu_registers_operator_list(  1,  4,  7, 10 ) : opl_emu_registers_operator_list(  1,  4, 0xff, 0xff );
    dest->chan[ 2] = opl_emu_bitfield(fourop, 2,1) ? opl_emu_registers_operator_list(  2,  5,  8, 11 ) : opl_emu_registers_operator_list(  2,  5, 0xff, 0xff );
    dest->chan[ 3] = opl_emu_bitfield(fourop, 0,1) ? opl_emu_registers_operator_list( 0xff, 0xff, 0xff, 0xff) : opl_emu_registers_operator_list(  6,  9, 0xff, 0xff );
    dest->chan[ 4] = opl_emu_bitfield(fourop, 1,1) ? opl_emu_registers_operator_list( 0xff, 0xff, 0xff, 0xff) : opl_emu_registers_operator_list(  7, 10, 0xff, 0xff );
    dest->chan[ 5] = opl_emu_bitfield(fourop, 2,1) ? opl_emu_registers_operator_list( 0xff, 0xff, 0xff, 0xff) : opl_emu_registers_operator_list(  8, 11, 0xff, 0xff );
    dest->chan[ 6] = opl_emu_registers_operator_list( 12, 15, 0xff, 0xff );
    dest->chan[ 7] = opl_emu_registers_operator_list( 13, 16, 0xff, 0xff );
    dest->chan[ 8] = opl_emu_registers_operator_list( 14, 17, 0xff, 0xff );

    dest->chan[ 9] = opl_emu_bitfield(fourop, 3,1) ? opl_emu_registers_operator_list( 18, 21, 24, 27 ) : opl_emu_registers_operator_list( 18, 21, 0xff, 0xff );
    dest->chan[10] = opl_emu_bitfield(fourop, 4,1) ? opl_emu_registers_operator_list( 19, 22, 25, 28 ) : opl_emu_registers_operator_list( 19, 22, 0xff, 0xff );
    dest->chan[11] = opl_emu_bitfield(fourop, 5,1) ? opl_emu_registers_operator_list( 20, 23, 26, 29 ) : opl_emu_registers_operator_list( 20, 23, 0xff, 0xff );
    dest->chan[12] = opl_emu_bitfield(fourop, 3,1) ? opl_emu_registers_operator_list(0xff, 0xff, 0xff, 0xff) : opl_emu_registers_operator_list( 24, 27, 0xff, 0xff );
    dest->chan[13] = opl_emu_bitfield(fourop, 4,1) ? opl_emu_registers_operator_list(0xff, 0xff, 0xff, 0xff) : opl_emu_registers_operator_list( 25, 28, 0xff, 0xff );
    dest->chan[14] = opl_emu_bitfield(fourop, 5,1) ? opl_emu_registers_operator_list(0xff, 0xff, 0xff, 0xff) : opl_emu_registers_operator_list( 26, 29, 0xff, 0xff );
    dest->chan[15] = opl_emu_registers_operator_list( 30, 33, 0xff, 0xff );
    dest->chan[16] = opl_emu_registers_operator_list( 31, 34, 0xff, 0xff );
    dest->chan[17] = opl_emu_registers_operator_list( 32, 35, 0xff, 0xff );
}


//-------------------------------------------------
//  write - handle writes to the register array
//-------------------------------------------------

int opl_emu_registers_write(struct opl_emu_registers* regs,uint16_t index, uint8_t data, uint32_t *channel, uint32_t *opmask)
{
	// writes to the mode register with high bit set ignore the low bits
	if (index == OPL_EMU_REGISTERS_REG_MODE && opl_emu_bitfield(data, 7,1) != 0)
		regs->m_regdata[index] |= 0x80;
	else
		regs->m_regdata[index] = data;

	// handle writes to the rhythm keyons
	if (index == 0xbd)
	{
		*channel = OPL_EMU_REGISTERS_RHYTHM_CHANNEL;
		*opmask = opl_emu_bitfield(data, 5,1) ? opl_emu_bitfield(data, 0, 5) : 0;
		return 1;
	}

	// handle writes to the channel keyons
	if ((index & 0xf0) == 0xb0)
	{
		*channel = index & 0x0f;
		if (*channel < 9)
		{
            *channel += 9 * opl_emu_bitfield(index, 8,1);
			*opmask = opl_emu_bitfield(data, 5,1) ? 15 : 0;
			return 1;
		}
	}
	return 0;
}


//-------------------------------------------------
//  clock_noise_and_lfo - clock the noise and LFO,
//  handling clock division, depth, and waveform
//  computations
//-------------------------------------------------

static int32_t opl_emu_opl_clock_noise_and_lfo(uint32_t *noise_lfsr, uint16_t *lfo_am_counter, uint16_t *lfo_pm_counter, uint8_t *lfo_am, uint32_t am_depth, uint32_t pm_depth)
{
	// OPL has a 23-bit noise generator for the rhythm section, running at
	// a constant rate, used only for percussion input
	*noise_lfsr <<= 1;
	*noise_lfsr |= opl_emu_bitfield(*noise_lfsr, 23,1) ^ opl_emu_bitfield(*noise_lfsr, 9,1) ^ opl_emu_bitfield(*noise_lfsr, 8,1) ^ opl_emu_bitfield(*noise_lfsr, 1,1);

	// OPL has two fixed-frequency LFOs, one for AM, one for PM

	// the AM LFO has 210*64 steps; at a nominal 50kHz output,
	// this equates to a period of 50000/(210*64) = 3.72Hz
	uint32_t am_counter = *lfo_am_counter++;
	if (am_counter >= 210*64 - 1)
		*lfo_am_counter = 0;

	// low 8 bits are fractional; depth 0 is divided by 2, while depth 1 is times 2
	int shift = 9 - 2 * am_depth;

	// AM value is the upper bits of the value, inverted across the midpoint
	// to produce a triangle
	*lfo_am = ((am_counter < 105*64) ? am_counter : (210*64+63 - am_counter)) >> shift;

	// the PM LFO has 8192 steps, or a nominal period of 6.1Hz
	uint32_t pm_counter = *lfo_pm_counter++;

	// PM LFO is broken into 8 chunks, each lasting 1024 steps; the PM value
	// depends on the upper bits of FNUM, so this value is a fraction and
	// sign to apply to that value, as a 1.3 value
	static int8_t pm_scale[8] = { 8, 4, 0, -4, -8, -4, 0, 4 };
	return pm_scale[opl_emu_bitfield(pm_counter, 10, 3)] >> (pm_depth ^ 1);
}

int32_t opl_emu_registers_clock_noise_and_lfo(struct opl_emu_registers* regs)
{
	return opl_emu_opl_clock_noise_and_lfo(&regs->m_noise_lfsr, &regs->m_lfo_am_counter, &regs->m_lfo_pm_counter, &regs->m_lfo_am, opl_emu_registers_lfo_am_depth(regs), opl_emu_registers_lfo_pm_depth(regs));
}


//-------------------------------------------------
//  cache_operator_data - fill the operator cache
//  with prefetched data; note that this code is
//  also used by ymopna_registers, so it must
//  handle upper channels cleanly
//-------------------------------------------------

void opl_emu_registers_cache_operator_data(struct opl_emu_registers* regs,uint32_t choffs, uint32_t opoffs, struct opl_emu_opdata_cache *cache)
{
	// set up the easy stuff
	cache->waveform = &regs->m_waveform[opl_emu_registers_op_waveform(regs, opoffs) % OPL_EMU_REGISTERS_WAVEFORMS][0];

	// get frequency from the channel
	uint32_t block_freq = cache->block_freq = opl_emu_registers_ch_block_freq(regs,choffs);

	// compute the keycode: block_freq is:
	//
	//     111  |
	//     21098|76543210
	//     BBBFF|FFFFFFFF
	//     ^^^??
	//
	// the 4-bit keycode uses the top 3 bits plus one of the next two bits
	uint32_t keycode = opl_emu_bitfield(block_freq, 10, 3) << 1;

	// lowest bit is determined by note_select(); note that it is
	// actually reversed from what the manual says, however
	keycode |= opl_emu_bitfield(block_freq, 9 - opl_emu_registers_note_select(regs), 1);

	// no detune adjustment on OPL
	cache->detune = 0;

	// multiple value, as an x.1 value (0 means 0.5)
	// replace the low bit with a table lookup to give 0,1,2,3,4,5,6,7,8,9,10,10,12,12,15,15
	uint32_t multiple = opl_emu_registers_op_multiple(regs,opoffs);
	cache->multiple = ((multiple & 0xe) | opl_emu_bitfield(0xc2aa, multiple,1)) * 2;
	if (cache->multiple == 0)
		cache->multiple = 1;

	// phase step, or OPL_EMU_PHASE_STEP_DYNAMIC if PM is active; this depends on block_freq, detune,
	// and multiple, so compute it after we've done those
	if (opl_emu_registers_op_lfo_pm_enable(regs,opoffs) == 0)
		cache->phase_step = opl_emu_registers_compute_phase_step(regs,choffs, opoffs, cache, 0);
	else
		cache->phase_step = OPL_EMU_PHASE_STEP_DYNAMIC;

	// total level, scaled by 8
	cache->total_level = opl_emu_registers_op_total_level(regs,opoffs) << 3;

	// pre-add key scale level
	uint32_t ksl = opl_emu_registers_op_ksl(regs,opoffs);
	if (ksl != 0)
		cache->total_level += opl_emu_opl_key_scale_atten(opl_emu_bitfield(block_freq, 10, 3), opl_emu_bitfield(block_freq, 6, 4)) << ksl;

	// 4-bit sustain level, but 15 means 31 so effectively 5 bits
	cache->eg_sustain = opl_emu_registers_op_sustain_level(regs,opoffs);
	cache->eg_sustain |= (cache->eg_sustain + 1) & 0x10;
	cache->eg_sustain <<= 5;

	// determine KSR adjustment for enevlope rates
	uint32_t ksrval = keycode >> (2 * (opl_emu_registers_op_ksr(regs,opoffs) ^ 1));
	cache->eg_rate[OPL_EMU_EG_ATTACK] = opl_emu_registers_effective_rate(opl_emu_registers_op_attack_rate(regs,opoffs) * 4, ksrval);
	cache->eg_rate[OPL_EMU_EG_DECAY] = opl_emu_registers_effective_rate(opl_emu_registers_op_decay_rate(regs,opoffs) * 4, ksrval);
	cache->eg_rate[OPL_EMU_EG_SUSTAIN] = opl_emu_registers_op_eg_sustain(regs,opoffs) ? 0 : opl_emu_registers_effective_rate(opl_emu_registers_op_release_rate(regs,opoffs) * 4, ksrval);
	cache->eg_rate[OPL_EMU_EG_RELEASE] = opl_emu_registers_effective_rate(opl_emu_registers_op_release_rate(regs,opoffs) * 4, ksrval);
}


//-------------------------------------------------
//  compute_phase_step - compute the phase step
//-------------------------------------------------

static uint32_t opl_emu_opl_compute_phase_step(uint32_t block_freq, uint32_t multiple, int32_t lfo_raw_pm)
{
	// OPL phase calculation has no detuning, but uses FNUMs like
	// the OPN version, and computes PM a bit differently

	// extract frequency number as a 12-bit fraction
	uint32_t fnum = opl_emu_bitfield(block_freq, 0, 10) << 2;

	// apply the phase adjustment based on the upper 3 bits
	// of FNUM and the PM depth parameters
	fnum += (lfo_raw_pm * opl_emu_bitfield(block_freq, 7, 3)) >> 1;

	// keep fnum to 12 bits
	fnum &= 0xfff;

	// apply block shift to compute phase step
	uint32_t block = opl_emu_bitfield(block_freq, 10, 3);
	uint32_t phase_step = (fnum << block) >> 2;

	// apply frequency multiplier (which is cached as an x.1 value)
	return (phase_step * multiple) >> 1;
}

uint32_t opl_emu_registers_compute_phase_step(struct opl_emu_registers* regs,uint32_t choffs, uint32_t opoffs, struct opl_emu_opdata_cache const *cache, int32_t lfo_raw_pm)
{
	return opl_emu_opl_compute_phase_step(cache->block_freq, cache->multiple, opl_emu_registers_op_lfo_pm_enable(regs,opoffs) ? lfo_raw_pm : 0);
}



//*********************************************************
//  FM OPERATOR
//*********************************************************

//-------------------------------------------------
//  opl_emu_fm_operator - constructor
//-------------------------------------------------

void opl_emu_fm_operator_init(struct opl_emu_fm_operator* fmop, struct opl_emu_registers* regs, uint32_t opoffs) {
	fmop->m_choffs = 0;
	fmop->m_opoffs = opoffs;
	fmop->m_phase = 0;
	fmop->m_env_attenuation = 0x3ff;
	fmop->m_env_state = OPL_EMU_EG_RELEASE;
	fmop->m_key_state = 0;
	fmop->m_keyon_live = 0;
	fmop->m_regs = regs;
	fmop->m_cache.eg_shift = 0;
}


//-------------------------------------------------
//  reset - reset the channel state
//-------------------------------------------------

void opl_emu_fm_operator_reset(struct opl_emu_fm_operator* fmop)
{
	// reset our data
	fmop->m_phase = 0;
	fmop->m_env_attenuation = 0x3ff;
	fmop->m_env_state = OPL_EMU_EG_RELEASE;
	fmop->m_key_state = 0;
	fmop->m_keyon_live = 0;
}

//-------------------------------------------------
//  prepare - prepare for clocking
//-------------------------------------------------

int opl_emu_fm_operator_prepare(struct opl_emu_fm_operator* fmop)
{
	// cache the data
	opl_emu_registers_cache_operator_data(fmop->m_regs,fmop->m_choffs, fmop->m_opoffs, &fmop->m_cache);

	// clock the key state
	opl_emu_fm_operator_clock_keystate(fmop,(uint32_t)(fmop->m_keyon_live != 0));
	fmop->m_keyon_live &= ~(1 << OPL_EMU_KEYON_CSM);

	// we're active until we're quiet after the release
	return (fmop->m_env_state != OPL_EMU_EG_RELEASE || fmop->m_env_attenuation < OPL_EMU_FM_OPERATOR_EG_QUIET);
}


//-------------------------------------------------
//  clock - master clocking function
//-------------------------------------------------

void opl_emu_fm_operator_clock(struct opl_emu_fm_operator* fmop, uint32_t env_counter, int32_t lfo_raw_pm)
{
	// clock the envelope if on an envelope cycle; env_counter is a x.2 value
	if (opl_emu_bitfield(env_counter, 0, 2) == 0)
		opl_emu_fm_operator_clock_envelope(fmop,env_counter >> 2);

	// clock the phase
	opl_emu_fm_operator_clock_phase(fmop,lfo_raw_pm);
}


//-------------------------------------------------
//  compute_volume - compute the 14-bit signed
//  volume of this operator, given a phase
//  modulation and an AM LFO offset
//-------------------------------------------------

int32_t opl_emu_fm_operator_compute_volume(struct opl_emu_fm_operator* fmop, uint32_t phase, uint32_t am_offset)
{
	// the low 10 bits of phase represents a full 2*PI period over
	// the full sin wave

	// early out if the envelope is effectively off
	if (fmop->m_env_attenuation > OPL_EMU_FM_OPERATOR_EG_QUIET)
		return 0;

	// get the absolute value of the sin, as attenuation, as a 4.8 fixed point value
	uint32_t sin_attenuation = fmop->m_cache.waveform[phase & (OPL_EMU_REGISTERS_WAVEFORM_LENGTH - 1)];

	// get the attenuation from the evelope generator as a 4.6 value, shifted up to 4.8
	uint32_t env_attenuation = opl_emu_fm_operator_envelope_attenuation(fmop, am_offset) << 2;

	// combine into a 5.8 value, then convert from attenuation to 13-bit linear volume
	int32_t result = opl_emu_attenuation_to_volume((sin_attenuation & 0x7fff) + env_attenuation);

	// negate if in the negative part of the sin wave (sign bit gives 14 bits)
	return opl_emu_bitfield(sin_attenuation, 15,1) ? -result : result;
}


//-------------------------------------------------
//  keyonoff - signal a key on/off event
//-------------------------------------------------

void opl_emu_fm_operator_keyonoff(struct opl_emu_fm_operator* fmop, uint32_t on, enum opl_emu_keyon_type type)
{
	fmop->m_keyon_live = (fmop->m_keyon_live & ~(1 << (int)(type))) | (opl_emu_bitfield(on, 0,1) << (int)(type));
}


//-------------------------------------------------
//  start_attack - start the attack phase; called
//  when a keyon happens 
//-------------------------------------------------

void opl_emu_fm_operator_start_attack(struct opl_emu_fm_operator* fmop)
{
	// don't change anything if already in attack state
	if (fmop->m_env_state == OPL_EMU_EG_ATTACK)
		return;
	fmop->m_env_state = OPL_EMU_EG_ATTACK;

	// reset the phase when we start an attack due to a key on
    fmop->m_phase = 0;

	// if the attack rate >= 62 then immediately go to max attenuation
	if (fmop->m_cache.eg_rate[OPL_EMU_EG_ATTACK] >= 62)
		fmop->m_env_attenuation = 0;
}


//-------------------------------------------------
//  start_release - start the release phase;
//  called when a keyoff happens
//-------------------------------------------------

void opl_emu_fm_operator_start_release(struct opl_emu_fm_operator* fmop)
{
	// don't change anything if already in release state
	if (fmop->m_env_state >= OPL_EMU_EG_RELEASE)
		return;
	fmop->m_env_state = OPL_EMU_EG_RELEASE;
}


//-------------------------------------------------
//  clock_keystate - clock the keystate to match
//  the incoming keystate
//-------------------------------------------------

void opl_emu_fm_operator_clock_keystate(struct opl_emu_fm_operator* fmop, uint32_t keystate)
{
	// has the key changed?
	if ((keystate ^ fmop->m_key_state) != 0)
	{
		fmop->m_key_state = keystate;

		// if the key has turned on, start the attack
		if (keystate != 0)
		{
            opl_emu_fm_operator_start_attack(fmop);
		}

		// otherwise, start the release
		else
			opl_emu_fm_operator_start_release(fmop);
	}
}


//-------------------------------------------------
//  clock_envelope - clock the envelope state
//  according to the given count
//-------------------------------------------------

void opl_emu_fm_operator_clock_envelope(struct opl_emu_fm_operator* fmop, uint32_t env_counter)
{
	// handle attack->decay transitions
	if (fmop->m_env_state == OPL_EMU_EG_ATTACK && fmop->m_env_attenuation == 0)
		fmop->m_env_state = OPL_EMU_EG_DECAY;

	// handle decay->sustain transitions; it is important to do this immediately
	// after the attack->decay transition above in the event that the sustain level
	// is set to 0 (in which case we will skip right to sustain without doing any
	// decay); as an example where this can be heard, check the cymbals sound
	// in channel 0 of shinobi's test mode sound #5
	if (fmop->m_env_state == OPL_EMU_EG_DECAY && fmop->m_env_attenuation >= fmop->m_cache.eg_sustain)
		fmop->m_env_state = OPL_EMU_EG_SUSTAIN;

	// fetch the appropriate 6-bit rate value from the cache
	uint32_t rate = fmop->m_cache.eg_rate[fmop->m_env_state];

	// compute the rate shift value; this is the shift needed to
	// apply to the env_counter such that it becomes a 5.11 fixed
	// point number
	uint32_t rate_shift = rate >> 2;
	env_counter <<= rate_shift;

	// see if the fractional part is 0; if not, it's not time to clock
	if (opl_emu_bitfield(env_counter, 0, 11) != 0)
		return;

	// determine the increment based on the non-fractional part of env_counter
	uint32_t relevant_bits = opl_emu_bitfield(env_counter, (rate_shift <= 11) ? 11 : rate_shift, 3);
	uint32_t increment = opl_emu_attenuation_increment(rate, relevant_bits);

	// attack is the only one that increases
	if (fmop->m_env_state == OPL_EMU_EG_ATTACK)
	{
		// glitch means that attack rates of 62/63 don't increment if
		// changed after the initial key on (where they are handled
		// specially); nukeykt confirms this happens on OPM, OPN, OPL/OPLL
		// at least so assuming it is true for everyone
		if (rate < 62)
			fmop->m_env_attenuation += (~fmop->m_env_attenuation * increment) >> 4;
	}

	// all other cases are similar
	else
	{
		// non-SSG-EG cases just apply the increment
        fmop->m_env_attenuation += increment;

		// clamp the final attenuation
		if (fmop->m_env_attenuation >= 0x400)
			fmop->m_env_attenuation = 0x3ff;
	}
}


//-------------------------------------------------
//  clock_phase - clock the 10.10 phase value; the
//  OPN version of the logic has been verified
//  against the Nuked phase generator
//-------------------------------------------------

void opl_emu_fm_operator_clock_phase(struct opl_emu_fm_operator* fmop, int32_t lfo_raw_pm)
{
	// read from the cache, or recalculate if PM active
	uint32_t phase_step = fmop->m_cache.phase_step;
	if (phase_step == OPL_EMU_PHASE_STEP_DYNAMIC)
		phase_step = opl_emu_registers_compute_phase_step(fmop->m_regs,fmop->m_choffs, fmop->m_opoffs, &fmop->m_cache, lfo_raw_pm);

	// finally apply the step to the current phase value
	fmop->m_phase += phase_step;
}


//-------------------------------------------------
//  envelope_attenuation - return the effective
//  attenuation of the envelope
//-------------------------------------------------

uint32_t opl_emu_fm_operator_envelope_attenuation(struct opl_emu_fm_operator* fmop, uint32_t am_offset)
{
	uint32_t result = fmop->m_env_attenuation >> fmop->m_cache.eg_shift;

	// add in LFO AM modulation
	if (opl_emu_registers_op_lfo_am_enable(fmop->m_regs,fmop->m_opoffs))
		result += am_offset;

	// add in total level and KSL from the cache
	result += fmop->m_cache.total_level;

	// clamp to max, apply shift, and return
	return opl_min(result, 0x3ff);
}



//*********************************************************
//  FM CHANNEL
//*********************************************************

//-------------------------------------------------
//  opl_emu_fm_channel - constructor
//-------------------------------------------------

void opl_emu_fm_channel_init(struct opl_emu_fm_channel* fmch,struct opl_emu_registers* regs, uint32_t choffs) {
	fmch->m_choffs = choffs;
	fmch->m_feedback[ 0 ] = 0;
    fmch->m_feedback[ 1 ] = 0;
	fmch->m_feedback_in = 0;
	fmch->m_op[ 0 ] = NULL;
	fmch->m_op[ 1 ] = NULL;
	fmch->m_op[ 2 ] = NULL;
	fmch->m_op[ 3 ] = NULL;
	fmch->m_regs = regs;
}


//-------------------------------------------------
//  reset - reset the channel state
//-------------------------------------------------

void opl_emu_fm_channel_reset(struct opl_emu_fm_channel* fmch)
{
	// reset our data
	fmch->m_feedback[0] = fmch->m_feedback[1] = 0;
	fmch->m_feedback_in = 0;
}

//-------------------------------------------------
//  keyonoff - signal key on/off to our operators
//-------------------------------------------------

void opl_emu_fm_channel_keyonoff(struct opl_emu_fm_channel* fmch,uint32_t states, enum opl_emu_keyon_type type, uint32_t chnum)
{
	for (uint32_t opnum = 0; opnum < sizeof( fmch->m_op ) / sizeof( *fmch->m_op ); opnum++)
		if (fmch->m_op[opnum] != NULL)
			opl_emu_fm_operator_keyonoff(fmch->m_op[opnum],opl_emu_bitfield(states, opnum,1), type);
}


//-------------------------------------------------
//  prepare - prepare for clocking
//-------------------------------------------------

int opl_emu_fm_channel_prepare(struct opl_emu_fm_channel* fmch)
{
	uint32_t active_mask = 0;

	// prepare all operators and determine if they are active
	for (uint32_t opnum = 0; opnum < sizeof( fmch->m_op ) / sizeof( *fmch->m_op ); opnum++)
		if (fmch->m_op[opnum] != NULL)
			if (opl_emu_fm_operator_prepare(fmch->m_op[opnum]))
				active_mask |= 1 << opnum;

	return (active_mask != 0);
}


//-------------------------------------------------
//  clock - master clock of all operators
//-------------------------------------------------

void opl_emu_fm_channel_clock(struct opl_emu_fm_channel* fmch,uint32_t env_counter, int32_t lfo_raw_pm)
{
	// clock the feedback through
	fmch->m_feedback[0] = fmch->m_feedback[1];
	fmch->m_feedback[1] = fmch->m_feedback_in;

	for (uint32_t opnum = 0; opnum < sizeof( fmch->m_op ) / sizeof( *fmch->m_op ); opnum++)
		if (fmch->m_op[opnum] != NULL)
			opl_emu_fm_operator_clock(fmch->m_op[opnum],env_counter, lfo_raw_pm);
}


//-------------------------------------------------
//  output_2op - combine 4 operators according to
//  the specified algorithm, returning a sum
//  according to the rshift and clipmax parameters,
//  which vary between different implementations
//-------------------------------------------------

void opl_emu_fm_channel_output_2op(struct opl_emu_fm_channel* fmch,short *output, uint32_t rshift, int32_t clipmax)
{
	// The first 2 operators should be populated
	// AM amount is the same across all operators; compute it once
	uint32_t am_offset = opl_emu_registers_lfo_am_offset(fmch->m_regs,fmch->m_choffs);

	// operator 1 has optional self-feedback
	int32_t opmod = 0;
	uint32_t feedback = opl_emu_registers_ch_feedback(fmch->m_regs,fmch->m_choffs);
	if (feedback != 0)
		opmod = (fmch->m_feedback[0] + fmch->m_feedback[1]) >> (10 - feedback);

	// compute the 14-bit volume/value of operator 1 and update the feedback
	int32_t op1value = fmch->m_feedback_in = opl_emu_fm_operator_compute_volume(fmch->m_op[0], opl_emu_fm_operator_phase(fmch->m_op[0]) + opmod, am_offset);

	// now that the feedback has been computed, skip the rest if all volumes
	// are clear; no need to do all this work for nothing
	if (opl_emu_registers_ch_output_any(fmch->m_regs,fmch->m_choffs) == 0)
		return;

	// Algorithms for two-operator case:
	//    0: O1 -> O2 -> out
	//    1: (O1 + O2) -> out
	int32_t result;
	if (opl_emu_bitfield(opl_emu_registers_ch_algorithm(fmch->m_regs,fmch->m_choffs), 0,1) == 0)
	{
		// some OPL chips use the previous sample for modulation instead of
		// the current sample
		opmod = op1value >> 1;
		result = opl_emu_fm_operator_compute_volume(fmch->m_op[1], opl_emu_fm_operator_phase(fmch->m_op[1]) + opmod, am_offset) >> rshift;
	}
	else
	{
		result = op1value + (opl_emu_fm_operator_compute_volume(fmch->m_op[1], opl_emu_fm_operator_phase(fmch->m_op[1]), am_offset) >> rshift);
		int32_t clipmin = -clipmax - 1;
		result = opl_emu_clamp(result, clipmin, clipmax);
	}

	// add to the output
	opl_emu_fm_channel_add_to_output(fmch,fmch->m_choffs, output, result);
}


//-------------------------------------------------
//  output_4op - combine 4 operators according to
//  the specified algorithm, returning a sum
//  according to the rshift and clipmax parameters,
//  which vary between different implementations
//-------------------------------------------------

void opl_emu_fm_channel_output_4op(struct opl_emu_fm_channel* fmch,short *output, uint32_t rshift, int32_t clipmax)
{
	// AM amount is the same across all operators; compute it once
	uint32_t am_offset = opl_emu_registers_lfo_am_offset(fmch->m_regs,fmch->m_choffs);

	// operator 1 has optional self-feedback
	int32_t opmod = 0;
	uint32_t feedback = opl_emu_registers_ch_feedback(fmch->m_regs,fmch->m_choffs);
	if (feedback != 0)
		opmod = (fmch->m_feedback[0] + fmch->m_feedback[1]) >> (10 - feedback);

	// compute the 14-bit volume/value of operator 1 and update the feedback
	int32_t op1value = fmch->m_feedback_in = opl_emu_fm_operator_compute_volume(fmch->m_op[0], opl_emu_fm_operator_phase(fmch->m_op[0]) + opmod, am_offset);

	// now that the feedback has been computed, skip the rest if all volumes
	// are clear; no need to do all this work for nothing
	if (opl_emu_registers_ch_output_any(fmch->m_regs,fmch->m_choffs) == 0)
		return;

	// OPM/OPN offer 8 different connection algorithms for 4 operators,
	// and OPL3 offers 4 more, which we designate here as 8-11.
	//
	// The operators are computed in order, with the inputs pulled from
	// an array of values (opout) that is populated as we go:
	//    0 = 0
	//    1 = O1
	//    2 = O2
	//    3 = O3
	//    4 = (O4)
	//    5 = O1+O2
	//    6 = O1+O3
	//    7 = O2+O3
	//
	// The s_algorithm_ops table describes the inputs and outputs of each
	// algorithm as follows:
	//
	//      ---------x use opout[x] as operator 2 input
	//      ------xxx- use opout[x] as operator 3 input
	//      ---xxx---- use opout[x] as operator 4 input
	//      --x------- include opout[1] in final sum
	//      -x-------- include opout[2] in final sum
	//      x--------- include opout[3] in final sum
	#define ALGORITHM(op2in, op3in, op4in, op1out, op2out, op3out) \
		((op2in) | ((op3in) << 1) | ((op4in) << 4) | ((op1out) << 7) | ((op2out) << 8) | ((op3out) << 9))
	static uint16_t const s_algorithm_ops[8+4] =
	{
		ALGORITHM(1,2,3, 0,0,0),    //  0: O1 -> O2 -> O3 -> O4 -> out (O4)
		ALGORITHM(0,5,3, 0,0,0),    //  1: (O1 + O2) -> O3 -> O4 -> out (O4)
		ALGORITHM(0,2,6, 0,0,0),    //  2: (O1 + (O2 -> O3)) -> O4 -> out (O4)
		ALGORITHM(1,0,7, 0,0,0),    //  3: ((O1 -> O2) + O3) -> O4 -> out (O4)
		ALGORITHM(1,0,3, 0,1,0),    //  4: ((O1 -> O2) + (O3 -> O4)) -> out (O2+O4)
		ALGORITHM(1,1,1, 0,1,1),    //  5: ((O1 -> O2) + (O1 -> O3) + (O1 -> O4)) -> out (O2+O3+O4)
		ALGORITHM(1,0,0, 0,1,1),    //  6: ((O1 -> O2) + O3 + O4) -> out (O2+O3+O4)
		ALGORITHM(0,0,0, 1,1,1),    //  7: (O1 + O2 + O3 + O4) -> out (O1+O2+O3+O4)
		ALGORITHM(1,2,3, 0,0,0),    //  8: O1 -> O2 -> O3 -> O4 -> out (O4)         [same as 0]
		ALGORITHM(0,2,3, 1,0,0),    //  9: (O1 + (O2 -> O3 -> O4)) -> out (O1+O4)   [unique]
		ALGORITHM(1,0,3, 0,1,0),    // 10: ((O1 -> O2) + (O3 -> O4)) -> out (O2+O4) [same as 4]
		ALGORITHM(0,2,0, 1,0,1)     // 11: (O1 + (O2 -> O3) + O4) -> out (O1+O3+O4) [unique]
	};
	uint32_t algorithm_ops = s_algorithm_ops[opl_emu_registers_ch_algorithm(fmch->m_regs,fmch->m_choffs)];

	// populate the opout table
	int16_t opout[8];
	opout[0] = 0;
	opout[1] = op1value;

	// compute the 14-bit volume/value of operator 2
	opmod = opout[opl_emu_bitfield(algorithm_ops, 0, 1)] >> 1;
	opout[2] = opl_emu_fm_operator_compute_volume(fmch->m_op[1], opl_emu_fm_operator_phase(fmch->m_op[1]) + opmod, am_offset);
	opout[5] = opout[1] + opout[2];

	// compute the 14-bit volume/value of operator 3
	opmod = opout[opl_emu_bitfield(algorithm_ops, 1, 3)] >> 1;
	opout[3] = opl_emu_fm_operator_compute_volume(fmch->m_op[2], opl_emu_fm_operator_phase(fmch->m_op[2]) + opmod, am_offset);
	opout[6] = opout[1] + opout[3];
	opout[7] = opout[2] + opout[3];

	// compute the 14-bit volume/value of operator 4; this could be a noise
	// value on the OPM; all algorithms consume OP4 output at a minimum
    opmod = opout[opl_emu_bitfield(algorithm_ops, 4, 3)] >> 1;
    int32_t result = opl_emu_fm_operator_compute_volume(fmch->m_op[3], opl_emu_fm_operator_phase(fmch->m_op[3]) + opmod, am_offset);
	
	result >>= rshift;

	// optionally add OP1, OP2, OP3
	int32_t clipmin = -clipmax - 1;
	if (opl_emu_bitfield(algorithm_ops, 7,1) != 0)
		result = opl_emu_clamp(result + (opout[1] >> rshift), clipmin, clipmax);
	if (opl_emu_bitfield(algorithm_ops, 8,1) != 0)
		result = opl_emu_clamp(result + (opout[2] >> rshift), clipmin, clipmax);
	if (opl_emu_bitfield(algorithm_ops, 9,1) != 0)
		result = opl_emu_clamp(result + (opout[3] >> rshift), clipmin, clipmax);

	// add to the output
	opl_emu_fm_channel_add_to_output(fmch,fmch->m_choffs, output, result);
}


//-------------------------------------------------
//  output_rhythm_ch6 - special case output
//  computation for OPL channel 6 in rhythm mode,
//  which outputs a Bass Drum instrument
//-------------------------------------------------

void opl_emu_fm_channel_output_rhythm_ch6(struct opl_emu_fm_channel* fmch,short *output, uint32_t rshift, int32_t clipmax)
{
	// AM amount is the same across all operators; compute it once
	uint32_t am_offset = opl_emu_registers_lfo_am_offset(fmch->m_regs,fmch->m_choffs);

	// Bass Drum: this uses operators 12 and 15 (i.e., channel 6)
	// in an almost-normal way, except that if the algorithm is 1,
	// the first operator is ignored instead of added in

	// operator 1 has optional self-feedback
	int32_t opmod = 0;
	uint32_t feedback = opl_emu_registers_ch_feedback(fmch->m_regs,fmch->m_choffs);
	if (feedback != 0)
		opmod = (fmch->m_feedback[0] + fmch->m_feedback[1]) >> (10 - feedback);

	// compute the 14-bit volume/value of operator 1 and update the feedback
	int32_t opout1 = fmch->m_feedback_in = opl_emu_fm_operator_compute_volume(fmch->m_op[0], opl_emu_fm_operator_phase(fmch->m_op[0]) + opmod, am_offset);

	// compute the 14-bit volume/value of operator 2, which is the result
	opmod = opl_emu_bitfield(opl_emu_registers_ch_algorithm(fmch->m_regs,fmch->m_choffs), 0,1) ? 0 : (opout1 >> 1);
	int32_t result = opl_emu_fm_operator_compute_volume(fmch->m_op[1], opl_emu_fm_operator_phase(fmch->m_op[1]) + opmod, am_offset) >> rshift;

	// add to the output
	opl_emu_fm_channel_add_to_output(fmch, fmch->m_choffs, output, result * 2);
}


//-------------------------------------------------
//  output_rhythm_ch7 - special case output
//  computation for OPL channel 7 in rhythm mode,
//  which outputs High Hat and Snare Drum
//  instruments
//-------------------------------------------------

void opl_emu_fm_channel_output_rhythm_ch7(struct opl_emu_fm_channel* fmch,uint32_t phase_select,short *output, uint32_t rshift, int32_t clipmax)
{
	// AM amount is the same across all operators; compute it once
	uint32_t am_offset = opl_emu_registers_lfo_am_offset(fmch->m_regs,fmch->m_choffs);
	uint32_t noise_state = opl_emu_bitfield(opl_emu_registers_noise_state(fmch->m_regs), 0,1);

	// High Hat: this uses the envelope from operator 13 (channel 7),
	// and a combination of noise and the operator 13/17 phase select
	// to compute the phase
	uint32_t phase = (phase_select << 9) | (0xd0 >> (2 * (noise_state ^ phase_select)));
	int32_t result = opl_emu_fm_operator_compute_volume(fmch->m_op[0], phase, am_offset) >> rshift;

	// Snare Drum: this uses the envelope from operator 16 (channel 7),
	// and a combination of noise and operator 13 phase to pick a phase
	uint32_t op13phase = opl_emu_fm_operator_phase(fmch->m_op[0]);
	phase = (0x100 << opl_emu_bitfield(op13phase, 8,1)) ^ (noise_state << 8);
	result += opl_emu_fm_operator_compute_volume(fmch->m_op[1],phase, am_offset) >> rshift;
	result = opl_emu_clamp(result, -clipmax - 1, clipmax);

	// add to the output
	opl_emu_fm_channel_add_to_output(fmch, fmch->m_choffs, output, result * 2);
}


//-------------------------------------------------
//  output_rhythm_ch8 - special case output
//  computation for OPL channel 8 in rhythm mode,
//  which outputs Tom Tom and Top Cymbal instruments
//-------------------------------------------------

void opl_emu_fm_channel_output_rhythm_ch8(struct opl_emu_fm_channel* fmch,uint32_t phase_select,short *output, uint32_t rshift, int32_t clipmax)
{
	// AM amount is the same across all operators; compute it once
	uint32_t am_offset = opl_emu_registers_lfo_am_offset(fmch->m_regs,fmch->m_choffs);

	// Tom Tom: this is just a single operator processed normally
	int32_t result = opl_emu_fm_operator_compute_volume(fmch->m_op[0], opl_emu_fm_operator_phase(fmch->m_op[0]), am_offset) >> rshift;

	// Top Cymbal: this uses the envelope from operator 17 (channel 8),
	// and the operator 13/17 phase select to compute the phase
	uint32_t phase = 0x100 | (phase_select << 9);
	result += opl_emu_fm_operator_compute_volume(fmch->m_op[1], phase, am_offset) >> rshift;
	result = opl_emu_clamp(result, -clipmax - 1, clipmax);

	// add to the output
	opl_emu_fm_channel_add_to_output(fmch, fmch->m_choffs, output, result * 2);
}


// This is the number subtracted from the 2nd voice for an instrument for OP2 soundbanks
// which causes those second voices to be replaced before their (more important) first voices
// when the OPL voice channels are all used up
#define OP2_2NDVOICE_PRIORITY_PENALTY 0xFF

struct voicealloc_t {
  unsigned short priority;
  signed short timbreid;
  signed char channel;
  signed char note;
  unsigned char voiceindex; /* 1 if 2nd voice for OP2 soundbank instrument, 0 otherwise */
};

enum op2_flags_t {
  OP2_FIXEDPITCH = 1,
  OP2_UNUSED = 2, /* technically delayed vibrato https://moddingwiki.shikadi.net/wiki/OP2_Bank_Format */
  OP2_DOUBLEVOICE = 4,
};

struct opl_t {
  signed char notes2voices[16][128][2]; /* keeps the map of channel:notes -> voice allocations */
  unsigned short channelpitch[16];      /* per-channel pitch level */
  unsigned short channelvol[16];        /* per-channel pitch level */
  struct voicealloc_t voices2notes[18]; /* keeps the map of what voice is playing what note/channel currently */
  unsigned char channelprog[16];        /* programs (patches) assigned to channels */
  int opl3; /* flag indicating whether or not the sound module is OPL3-compatible or only OPL2 */
  struct opl_emu_t opl_emu;
  struct opl_timbre_t opl_gmtimbres[ 256 ];
  struct opl_timbre_t opl_gmtimbres_voice2[ 256 ]; /* second voice included in OP2 format */
  int is_op2; /* true if OP2 soundbank */
  enum op2_flags_t op2_flags[ 256 ]; /* OP2 format flags */
};


void oplregwr( opl_t* opl, uint16_t reg, uint8_t data ) {
    opl_emu_write( &opl->opl_emu, reg, data );
}


void opl_render( opl_t* opl, short* sample_pairs, int sample_pairs_count, float volume ) {
    memset( sample_pairs, 0, sample_pairs_count * 2 * sizeof( short ) );
    opl_emu_generate( &opl->opl_emu, sample_pairs, sample_pairs_count, volume );
}


void opl_write( opl_t* opl, int count, unsigned short* regs, unsigned char* data ) {
    struct opl_emu_t* emu = &opl->opl_emu;
    for( int i = 0; i < count; ++i ) {
        uint16_t regnum = regs[ i ];
        uint8_t value = data[ i ];
	    // special case: writes to the mode register can impact IRQs;
	    // schedule these writes to ensure ordering with timers
	    if (regnum == OPL_EMU_REGISTERS_REG_MODE)
	    {
    //		emu->m_intf.opl_emu_sync_mode_write(data);
		    continue;;
	    }

	    // for now just mark all channels as modified
	    emu->m_modified_channels = OPL_EMU_REGISTERS_ALL_CHANNELS;

	    // most writes are passive, consumed only when needed
	    uint32_t keyon_channel;
	    uint32_t keyon_opmask;
	    if (opl_emu_registers_write(&emu->m_regs,regnum, value, &keyon_channel, &keyon_opmask))
	    {
		    // handle writes to the keyon register(s)
		    if (keyon_channel < OPL_EMU_REGISTERS_CHANNELS)
		    {
			    // normal channel on/off
			    opl_emu_fm_channel_keyonoff(&emu->m_channel[keyon_channel],keyon_opmask, OPL_EMU_KEYON_NORMAL, keyon_channel);
		    }
		    else if (OPL_EMU_REGISTERS_CHANNELS >= 9 && keyon_channel == OPL_EMU_REGISTERS_RHYTHM_CHANNEL)
		    {
			    // special case for the OPL rhythm channels
			    opl_emu_fm_channel_keyonoff(&emu->m_channel[6],opl_emu_bitfield(keyon_opmask, 4,1) ? 3 : 0, OPL_EMU_KEYON_RHYTHM, 6);
			    opl_emu_fm_channel_keyonoff(&emu->m_channel[7],opl_emu_bitfield(keyon_opmask, 0,1) | (opl_emu_bitfield(keyon_opmask, 3,1) << 1), OPL_EMU_KEYON_RHYTHM, 7);
			    opl_emu_fm_channel_keyonoff(&emu->m_channel[8],opl_emu_bitfield(keyon_opmask, 2,1) | (opl_emu_bitfield(keyon_opmask, 1,1) << 1), OPL_EMU_KEYON_RHYTHM, 8);
		    }
	    }
    }
}


const unsigned short freqtable[128] = {                          /* note # */
        345, 365, 387, 410, 435, 460, 488, 517, 547, 580, 615, 651,  /*  0 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,  /* 12 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,  /* 24 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,  /* 36 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,  /* 48 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,  /* 60 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,  /* 72 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,  /* 84 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651,  /* 96 */
        690, 731, 774, 820, 869, 921, 975, 517, 547, 580, 615, 651, /* 108 */
        690, 731, 774, 820, 869, 921, 975, 517};                    /* 120 */

const unsigned char octavetable[128] = {                         /* note # */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                          /*  0 */
        0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,                          /* 12 */
        1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,                          /* 24 */
        2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3,                          /* 36 */
        3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4,                          /* 48 */
        4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5,                          /* 60 */
        5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6,                          /* 72 */
        6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7,                          /* 84 */
        7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8,                          /* 96 */
        8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9,                         /* 108 */
        9, 9, 9, 9, 9, 9, 9,10};                                    /* 120 */

const unsigned short pitchtable[256] = {                    /* pitch wheel */
         29193U,29219U,29246U,29272U,29299U,29325U,29351U,29378U,  /* -128 */
         29405U,29431U,29458U,29484U,29511U,29538U,29564U,29591U,  /* -120 */
         29618U,29644U,29671U,29698U,29725U,29752U,29778U,29805U,  /* -112 */
         29832U,29859U,29886U,29913U,29940U,29967U,29994U,30021U,  /* -104 */
         30048U,30076U,30103U,30130U,30157U,30184U,30212U,30239U,  /*  -96 */
         30266U,30293U,30321U,30348U,30376U,30403U,30430U,30458U,  /*  -88 */
         30485U,30513U,30541U,30568U,30596U,30623U,30651U,30679U,  /*  -80 */
         30706U,30734U,30762U,30790U,30817U,30845U,30873U,30901U,  /*  -72 */
         30929U,30957U,30985U,31013U,31041U,31069U,31097U,31125U,  /*  -64 */
         31153U,31181U,31209U,31237U,31266U,31294U,31322U,31350U,  /*  -56 */
         31379U,31407U,31435U,31464U,31492U,31521U,31549U,31578U,  /*  -48 */
         31606U,31635U,31663U,31692U,31720U,31749U,31778U,31806U,  /*  -40 */
         31835U,31864U,31893U,31921U,31950U,31979U,32008U,32037U,  /*  -32 */
         32066U,32095U,32124U,32153U,32182U,32211U,32240U,32269U,  /*  -24 */
         32298U,32327U,32357U,32386U,32415U,32444U,32474U,32503U,  /*  -16 */
         32532U,32562U,32591U,32620U,32650U,32679U,32709U,32738U,  /*   -8 */
         32768U,32798U,32827U,32857U,32887U,32916U,32946U,32976U,  /*    0 */
         33005U,33035U,33065U,33095U,33125U,33155U,33185U,33215U,  /*    8 */
         33245U,33275U,33305U,33335U,33365U,33395U,33425U,33455U,  /*   16 */
         33486U,33516U,33546U,33576U,33607U,33637U,33667U,33698U,  /*   24 */
         33728U,33759U,33789U,33820U,33850U,33881U,33911U,33942U,  /*   32 */
         33973U,34003U,34034U,34065U,34095U,34126U,34157U,34188U,  /*   40 */
         34219U,34250U,34281U,34312U,34343U,34374U,34405U,34436U,  /*   48 */
         34467U,34498U,34529U,34560U,34591U,34623U,34654U,34685U,  /*   56 */
         34716U,34748U,34779U,34811U,34842U,34874U,34905U,34937U,  /*   64 */
         34968U,35000U,35031U,35063U,35095U,35126U,35158U,35190U,  /*   72 */
         35221U,35253U,35285U,35317U,35349U,35381U,35413U,35445U,  /*   80 */
         35477U,35509U,35541U,35573U,35605U,35637U,35669U,35702U,  /*   88 */
         35734U,35766U,35798U,35831U,35863U,35895U,35928U,35960U,  /*   96 */
         35993U,36025U,36058U,36090U,36123U,36155U,36188U,36221U,  /*  104 */
         36254U,36286U,36319U,36352U,36385U,36417U,36450U,36483U,  /*  112 */
         36516U,36549U,36582U,36615U,36648U,36681U,36715U,36748U}; /*  120 */


/*
 * This file contains OPL fonts for all the General Midi level 1 instruments:
 *  - 128 instruments
 *  - 128 percussions
 */
static struct opl_timbre_t opl_gmtimbres_default[ 256 ] = {
/* ,---------+-------- Wave select settings                        *
 * | ,-------÷-+------ Sustain/release rates                       *
 * | | ,-----÷-÷-+---- Attack/decay rates                          *
 * | | | ,---÷-÷-÷-+-- AM/VIB/EG/KSR/Multiple bits                 *
 * | | | |   | | | |                                               *
 * | | | |   | | | |    ,----+-- KSL/attenuation settings          *
 * | | | |   | | | |    |    |    ,----- Feedback/connection bits  *
 * | | | |   | | | |    |    |    |   ,---- Percussion note number */
{0x223E133,0x4F4F131,0xA5,0x09,0x0E,0,0},
{0x433F133,0x0F4F131,0xA3,0x09,0x0C,0,0},
{0x4B3E131,0x0F4F130,0x1A,0x86,0x08,0,0},
{0x073FA32,0x4F4D111,0x80,0x08,0x0C,0,0},
{0x0E7F21C,0x0B8F201,0x6F,0x80,0x0C,0,0},
{0x0C7F437,0x0D7F230,0x5D,0x0A,0x08,0,0},
{0x303F232,0x1F6D131,0x44,0x00,0x08,0,0},
{0x559F101,0x0F7F111,0x44,0x08,0x06,0,0},
{0x087F607,0x0E4F231,0x54,0x08,0x09,0,0},
{0x0A5F33F,0x0F2C312,0xA1,0x06,0x0C,0,0},
{0x004561A,0x004F601,0x9E,0x08,0x08,0,0},
{0x0F6F2B2,0x0F6F281,0xE5,0x00,0x0F,0,0},
{0x0F6F618,0x0F7E500,0x63,0x80,0x06,0,0},
{0x096F616,0x0F5F111,0x1F,0x03,0x04,0,0},
{0x082F307,0x0E3F302,0x97,0x8A,0x06,0,0},
{0x4109130,0x3B5F321,0x52,0x88,0x08,0,0},
{0x116B1A2,0x117D160,0x88,0x80,0x07,0,0},
{0x0F8F032,0x0F8F001,0x65,0x00,0x0E,0,0},
{0x018AA70,0x0088AB1,0x44,0x0A,0x04,0,0},
{0x1043030,0x1145431,0x92,0x80,0x0D,0,0},
{0x0178000,0x1176081,0x54,0x8B,0x04,0,0},
{0x025A721,0x1264132,0x4D,0x08,0x06,0,0},
{0x4FAF022,0x01A6221,0x96,0x08,0x0C,0,0},
{0x107F020,0x2055231,0x92,0x07,0x08,0,0},
{0x274A613,0x4B8F401,0xDD,0x05,0x06,0,0},
{0x5E5F133,0x1E4F211,0x99,0x07,0x06,0,0},
{0x21FF021,0x088F211,0xA5,0x48,0x0A,0,0},
{0x132ED10,0x3E7D210,0x87,0x0A,0x06,0,0},
{0x0F4E430,0x0F5F330,0x92,0x80,0x08,0,0},
{0x0F78111,0x3F7F054,0x40,0x45,0x08,0,0},
{0x0F78111,0x2F7F054,0x40,0x45,0x0A,0,0},
{0x6F78AE4,0x649B1F2,0x03,0x0A,0x0A,0,0},
{0x0209220,0x0E6C130,0x97,0x05,0x00,0,0},
{0x0FFF030,0x0F8F131,0x9D,0x00,0x0A,0,0},
{0x026AA20,0x0D7F131,0xCF,0x80,0x0A,0,0},
{0x0007511,0x0B69212,0x1A,0x40,0x08,0,0},
{0x2E69419,0x5B6B311,0x5E,0x08,0x00,0,0},
{0x2E69515,0x1B6B211,0x17,0x08,0x00,0,0},
{0x0F5F430,0x0F6F330,0x0E,0x00,0x0A,0,0},
{0x1468330,0x017D231,0x15,0x00,0x0A,0,0},
{0x2257020,0x4266161,0x95,0x05,0x0A,0,0},
{0x2446070,0x2154130,0x4E,0x00,0x0A,0,0},
{0x214D070,0x1175222,0x0F,0x88,0x02,0,0},
{0x521F570,0x4166021,0x90,0x09,0x06,0,0},
{0x201C3B0,0x0058321,0x8D,0x08,0x08,0,0},
{0x0848521,0x074C411,0x69,0x07,0x0A,0,0},
{0x022E133,0x0F2F131,0xA2,0x09,0x0E,0,0},
{0x4C3C413,0x0B4D215,0x9B,0x09,0x0A,0,0},
{0x223F832,0x4055421,0x99,0x8A,0x0C,0,0},
{0x5059022,0x1055521,0x5B,0x85,0x00,0,0},
{0x1254732,0x40256B1,0xA4,0x0A,0x08,0,0},
{0x0031121,0x1046120,0x58,0x84,0x00,0,0},
{0x0055020,0x0F55021,0x1C,0x0F,0x07,0,0},
{0x239B420,0x0076121,0x50,0x05,0x06,0,0},
{0x05470F0,0x07460B0,0x5A,0x80,0x00,0,0},
{0x2436110,0x114D211,0x90,0x00,0x0C,0,0},
{0x017B561,0x0097F21,0x92,0x04,0x0C,0,0},
{0x0235270,0x0198160,0x1E,0x08,0x0E,0,0},
{0x0157621,0x0378261,0x94,0x00,0x0C,0,0},
{0x118537A,0x5177432,0x21,0x00,0x04,0,0},
{0x0364120,0x02B7220,0x21,0x08,0x0C,0,0},
{0x0577320,0x117C020,0x19,0x03,0x0C,0,0},
{0x036F120,0x337F120,0x92,0x08,0x0E,0,0},
{0x0A66120,0x0976120,0x9B,0x08,0x0E,0,0},
{0x0F37010,0x1F65051,0x51,0x04,0x0A,0,0},
{0x1067021,0x1165231,0x8A,0x00,0x06,0,0},
{0x00B9820,0x10B5330,0x8E,0x00,0x0A,0,0},
{0x10B8020,0x11B6330,0x87,0x00,0x08,0,0},
{0x0235030,0x0076C62,0x58,0x08,0x0A,0,0},
{0x2077820,0x2074331,0x97,0x00,0x06,0,0},
{0x0199030,0x01B6132,0x95,0x80,0x0A,0,0},
{0x0177531,0x0174530,0x93,0x03,0x0C,0,0},
{0x08D6EF1,0x02A3571,0xC0,0x00,0x0E,0,0},
{0x08860A1,0x02A6561,0xA6,0x00,0x08,0,0},
{0x2176522,0x0277421,0x5A,0x00,0x06,0,0},
{0x1273471,0x01745B0,0x8D,0x05,0x04,0,0},
{0x00457E1,0x0375760,0xA8,0x00,0x0E,0,0},
{0x6543739,0x25D67A1,0x28,0x00,0x0E,0,0},
{0x00F31D0,0x0053270,0xC7,0x00,0x0B,0,0},
{0x00581A1,0x0295230,0x37,0x00,0x06,0,0},
{0x20FFF22,0x60FFF21,0x7F,0x12,0x05,0,0},
{0x39BC120,0x368C030,0xBF,0x06,0x00,0,0},
{0x33357F0,0x00767E0,0x28,0x00,0x0E,0,0},
{0x40457E1,0x03D67E0,0x23,0x00,0x0E,0,0},
{0x32B7320,0x12BF131,0x40,0x00,0x08,0,0},
{0x5029071,0x0069060,0x96,0x09,0x08,0,0},
{0x195C120,0x1637030,0x43,0x80,0x0A,0,0},
{0x132ED10,0x3E7D210,0x87,0x08,0x06,0,0},
{0x2946374,0x005A0A1,0xA5,0x05,0x02,0,0},
{0x00521A1,0x0053360,0xC0,0x00,0x09,0,0},
{0x2A5A120,0x196A120,0x95,0x05,0x0C,0,0},
{0x005F0E0,0x0548160,0x44,0x00,0x0B,0,0},
{0x0336183,0x05452E0,0xA7,0x00,0x06,0,0},
{0x2529082,0x1534340,0x9D,0x80,0x0C,0,0},
{0x2345231,0x2135120,0x98,0x00,0x06,0,0},
{0x1521161,0x1632060,0x90,0x80,0x08,0,0},
{0x157B260,0x019F803,0x04,0x40,0x07,0,0},
{0x2322122,0x0133221,0x8C,0x92,0x06,0,0},
{0x074F624,0x0249303,0xC0,0x0D,0x00,0,0},
{0x3D2C091,0x1D2D130,0x8E,0x03,0x00,0,0},
{0x5F29052,0x0F2C240,0x96,0x06,0x08,0,0},
{0x05213E1,0x2131371,0x1A,0x88,0x07,0,0},
{0x0B67060,0x0928031,0x9C,0x11,0x0A,0,0},
{0x0025511,0x1748201,0x94,0x06,0x0E,0,0},
{0x0B37120,0x5F48220,0x1B,0x08,0x02,0,0},
{0x0127530,0x6F4F310,0x0D,0x0A,0x06,0,0},
{0x033F900,0x273F400,0x80,0x80,0x00,0,0},
{0x332F320,0x6E49423,0x0E,0x08,0x08,0,0},
{0x0328413,0x073B410,0xA1,0x00,0x0F,0,0},
{0x1397931,0x2099B22,0x80,0x00,0x06,0,0},
{0x302A130,0x0266221,0x1E,0x00,0x0E,0,0},
{0x0136030,0x1169130,0x12,0x80,0x08,0,0},
{0x032A115,0x172B212,0x00,0x80,0x01,0,0},
{0x001E795,0x0679616,0x81,0x00,0x04,0,0},
{0x4046303,0x005A901,0xCA,0x08,0x06,0,0},
{0x6D1F817,0x098F611,0xA7,0x00,0x06,0,0},
{0x008F312,0x004F600,0x08,0xC8,0x04,0,46},
{0x0C8A820,0x0B7E601,0x0B,0x00,0x00,0,0},
{0x00437D2,0x0343471,0xA1,0x07,0x0C,0,0},
{0x2114109,0x51D2101,0x05,0x80,0x0A,0,0},
{0x4543310,0x3574515,0x19,0x03,0x0E,0,0},
{0x00437D2,0x0343471,0xA1,0x07,0x0C,0,0},
{0x200C327,0x6021300,0x80,0x08,0x0E,0,65},
{0x003EBD7,0x06845D8,0xD4,0x00,0x07,0,0},
{0x62FDA20,0x614B009,0x42,0x48,0x04,0,0},
{0x101FE30,0x6142120,0x00,0x00,0x0C,0,17},
{0x200832F,0x6044020,0x80,0x00,0x0E,0,65},
{0x230732F,0x6E6F400,0x00,0x00,0x0E,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0E,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x06,0,0},
{0x0000000,0x0000000,0x00,0x00,0x06,0,0},
{0x0000000,0x0000000,0x00,0x00,0x06,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0A,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0A,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0E,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x06,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0A,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0C,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0A,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x02,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0E,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0C,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0E,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0E,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x08,0,0},
{0x0000000,0x0000000,0x00,0x00,0x0F,0,0},
{0x057FB00,0x046F800,0x00,0x00,0x0E,0,25},
{0x277F810,0x006FC11,0x86,0x00,0x0A,0,34},
{0x287F702,0x678F802,0x80,0x88,0x0A,0,61},
{0x6EF8801,0x608B502,0x0D,0x00,0x0C,0,38},
{0x05476C1,0x30892C5,0x80,0x08,0x06,0,37},
{0x6EF8800,0x608F502,0x13,0x00,0x08,0,38},
{0x508F601,0x104F600,0x08,0x00,0x06,0,32},
{0x254F307,0x307F905,0x04,0x0B,0x06,0,48},
{0x508F601,0x104F600,0x08,0x00,0x0A,0,34},
{0x254D307,0x3288905,0x04,0x08,0x0A,0,48},
{0x508F601,0x104F600,0x0C,0x00,0x08,0,37},
{0x2F2E327,0x3F5C525,0x04,0x08,0x08,0,48},
{0x508F601,0x104F600,0x0C,0x00,0x00,0,40},
{0x508F601,0x104F600,0x0C,0x00,0x00,0,43},
{0x292F108,0x354F201,0x00,0x08,0x00,0,61},
{0x508F601,0x104F600,0x0C,0x00,0x00,0,46},
{0x210F509,0x305FE03,0x8A,0x88,0x00,0,60},
{0x283E108,0x334D700,0x00,0x08,0x00,0,79},
{0x2E1F119,0x3F3F11B,0x04,0x08,0x00,0,62},
{0x2777603,0x3679601,0x87,0x08,0x00,0,80},
{0x251F206,0x263C504,0x04,0x09,0x00,0,67},
{0x366F905,0x099F701,0x00,0x00,0x00,0,58},
{0x292F108,0x354F201,0x00,0x03,0x00,0,62},
{0x422F120,0x056B40E,0x81,0x00,0x00,0,24},
{0x212FD04,0x305FD03,0x01,0x00,0x00,0,61},
{0x2A8F9E3,0x0779643,0x1E,0x08,0x00,0,41},
{0x2A8F9E3,0x0779643,0x1E,0x00,0x00,0,35},
{0x0A8F7E9,0x5D8990A,0x08,0x00,0x00,0,29},
{0x2A8F9E2,0x0779642,0x1E,0x00,0x00,0,41},
{0x2A8F9E2,0x0779642,0x1E,0x00,0x00,0,37},
{0x456FB02,0x017F700,0x81,0x00,0x00,0,55},
{0x456FB02,0x017F700,0x81,0x00,0x00,0,48},
{0x367FD01,0x098F601,0x00,0x08,0x00,0,78},
{0x367FD01,0x098F601,0x00,0x08,0x00,0,73},
{0x25E780C,0x32B8A0A,0x00,0x80,0x00,0,88},
{0x098600F,0x3FC8590,0x08,0xC0,0x00,0,40},
{0x009F020,0x37DA588,0x07,0x00,0x00,0,45},
{0x00FC020,0x32DA5A8,0x07,0x00,0x00,0,42},
{0x106F680,0x016F610,0x00,0x00,0x00,0,48},
{0x106F680,0x016F610,0x00,0x00,0x00,0,48},
{0x0D1F815,0x078F512,0x44,0x00,0x00,0,73},
{0x1D1F813,0x078F512,0x44,0x00,0x00,0,68},
{0x1D1F813,0x078F512,0x44,0x00,0x00,0,61},
{0x1DC5D01,0x06FF79F,0x0B,0x00,0x00,0,16},
{0x1C7C900,0x05FF49F,0x07,0x00,0x00,0,16},
{0x160F2C6,0x07AF4D4,0x4F,0x80,0x00,0,90},
{0x160F286,0x0B7F294,0x4F,0x80,0x00,0,90},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0},
{0x0000000,0x0000000,0x00,0x00,0x00,0,0}
};

/* tables below provide register offsets for each voice. note, that these are
 * NOT the registers IDs, but their direct offsets instead - this for simpler
 * and faster computations. */
const unsigned short op1offsets[18] = {0x00,0x01,0x02,0x08,0x09,0x0a,0x10,0x11,0x12,0x100,0x101,0x102,0x108,0x109,0x10a,0x110,0x111,0x112};
const unsigned short op2offsets[18] = {0x03,0x04,0x05,0x0b,0x0c,0x0d,0x13,0x14,0x15,0x103,0x104,0x105,0x10b,0x10c,0x10d,0x113,0x114,0x115};

/* number of melodic voices: 9 by default (OPL2), can go up to 18 (OPL3) */
static int voicescount = 9;


/* 'volume' is in range 0..127 - take care to change only the 'attenuation'
 * part of the register, and never touch the KSL bits */
static void calc_vol(unsigned char *regbyte, int volume) {
  int level;
  /* invert bits and strip out the KSL header */
  level = ~(*regbyte);
  level &= 0x3f;

  /* adjust volume */
  level = (level * volume) / 127;

  /* boundaries check */
  if (level > 0x3f) level = 0x3f;
  if (level < 0) level = 0;

  /* invert the bits, as expected by the OPL registers */
  level = ~level;
  level &= 0x3f;

  /* final result computation */
  *regbyte &= 0xC0;  /* zero out all attentuation bits */
  *regbyte |= level; /* fill in the new attentuation value */
}


/* Initialize hardware upon startup - positive on success, negative otherwise
 * Returns 0 for OPL2 initialization, or 1 if OPL3 has been detected */
opl_t* opl_create(void) {
  /* init memory */
  struct opl_t* opl = (struct opl_t*)calloc(1, sizeof(struct opl_t));
  if (opl == NULL) return NULL;
  memcpy( opl->opl_gmtimbres, opl_gmtimbres_default, sizeof( opl_gmtimbres_default ) );
  opl_emu_init( &opl->opl_emu );

  opl->is_op2 = 0;
  
  /* detect the hardware and return error if not found */
  oplregwr(opl, 0x04, 0x60); /* reset both timers by writing 60h to register 4 */
  oplregwr(opl, 0x04, 0x80); /* enable interrupts by writing 80h to register 4 (must be a separate write from the 1st one) */
  //x = inp(port) & 0xE0; /* read the status register (port 388h) and store the result */
  oplregwr(opl, 0x02, 0xff); /* write FFh to register 2 (Timer 1) */
  oplregwr(opl, 0x04, 0x21); /* start timer 1 by writing 21h to register 4 */
  /*udelay(500);*/ /* Creative Labs recommends a delay of at least 80 microseconds
                  I delay for 500us just to be sure. DO NOT perform inp()
                  calls for delay here, some cards do not initialize well then
                  (reported for CT2760) */
  //y = inp(port) & 0xE0;  /* read the upper bits of the status register */
  oplregwr(opl, 0x04, 0x60); /* reset both timers and interrupts (see steps 1 and 2) */
  oplregwr(opl, 0x04, 0x80); /* reset both timers and interrupts (see steps 1 and 2) */
  /* test the stored results of steps 3 and 7 by ANDing them with E0h. The result of step 3 should be */

//  if (x != 0) return(-1);    /* 00h, and the result of step 7 should be C0h. If both are     */
//  if (y != 0xC0) return(-2); /* ok, an AdLib-compatible board is installed in the computer   */


  /* is it an OPL3 or just an OPL2? */
  /*if ((inp(port) & 0x06) == 0) */opl->opl3 = 1;

  /* init the hardware */
  voicescount = 9; /* OPL2 provides 9 melodic voices */

  /* enable OPL3 (if detected) and put it into 36 operators mode */
  if (opl->opl3 != 0) {
    oplregwr(opl, 0x105, 1);  /* enable OPL3 mode (36 operators) */
    oplregwr(opl, 0x104, 0);  /* disable four-operator voices */
    voicescount = 18;          /* OPL3 provides 18 melodic channels */

    /* Init the secondary OPL chip
     * NOTE: this I don't do anymore, it turns my Aztech Waverider mute! */
    /* oplregwr(0x101, 0x20); */ /* enable Waveform Select */
    /* oplregwr(0x108, 0x40); */ /* turn off CSW mode and activate FM synth mode */
    /* oplregwr(0x1BD, 0x00); */ /* set vibrato/tremolo depth to low, set melodic mode */
  }

  oplregwr(opl, 0x01, 0x20);  /* enable Waveform Select */
  oplregwr(opl, 0x04, 0x00);  /* turn off timers IRQs */
  oplregwr(opl, 0x08, 0x40);  /* turn off CSW mode and activate FM synth mode */
  oplregwr(opl, 0xBD, 0x00);  /* set vibrato/tremolo depth to low, set melodic mode */

  for (int x = 0; x < voicescount; x++) {
    oplregwr(opl, 0x20 + op1offsets[x], 0x1);     /* set the modulator's multiple to 1 */
    oplregwr(opl, 0x20 + op2offsets[x], 0x1);     /* set the modulator's multiple to 1 */
    oplregwr(opl, 0x40 + op1offsets[x], 0x10);    /* set volume of all channels to about 40 dB */
    oplregwr(opl, 0x40 + op2offsets[x], 0x10);    /* set volume of all channels to about 40 dB */
  }

  opl_clear(opl);
  /* all done */
  return(opl);
}


/* close OPL device */
void opl_destroy(opl_t* opl) {
  int x;

  /* turns all notes 'off' */
  opl_clear(opl);

  /* set volume to lowest level on all voices */
  for (x = 0; x < voicescount; x++) {
    oplregwr(opl, 0x40 + op1offsets[x], 0x1f);
    oplregwr(opl, 0x40 + op2offsets[x], 0x1f);
  }

  /* if OPL3, switch the chip back into its default OPL2 mode */
  if (opl->opl3 != 0) oplregwr(opl, 0x105, 0);

  /* free state memory */
  free(opl);
  opl = NULL;
}

void opl_noteoff(opl_t* opl, unsigned short voice) {
  /* if voice is one of the OPL3 set, adjust it and route over secondary OPL port */
  if (voice >= 9) {
    oplregwr(opl, 0x1B0 + voice - 9, 0);
  } else {
    oplregwr(opl, 0xB0 + voice, 0);
  }
}


void opl_noteon(opl_t* opl, unsigned short voice, unsigned int note, int pitch) {
  unsigned int freq = freqtable[note];
  unsigned int octave = octavetable[note];

  if (pitch != 0) {
    if (pitch > 127) {
      pitch = 127;
    } else if (pitch < -128) {
      pitch = -128;
    }
    freq = ((unsigned long)freq * pitchtable[pitch + 128]) >> 15;
    if (freq >= 1024) {
      freq >>= 1;
      octave++;
    }
  }
  if (octave > 7) octave = 7;

  /* if voice is one of the OPL3 set, adjust it and route over secondary OPL port */
  if (voice >= 9) {
    voice -= 9;
    voice |= 0x100;
  }

  oplregwr(opl, 0xA0 + voice, freq & 0xff); /* set lowfreq */
  oplregwr(opl, 0xB0 + voice, (freq >> 8) | (octave << 2) | 32); /* KEY ON + hifreq + octave */
}


/* turns off all notes */
void opl_clear(opl_t* opl) {
  int x, y;
  for (x = 0; x < voicescount; x++) opl_noteoff(opl, x);

  /* reset the percussion bits at the 0xBD register */
  oplregwr(opl, 0xBD, 0);

  /* mark all voices as unused */
  for (x = 0; x < voicescount; x++) {
    opl->voices2notes[x].channel = -1;
    opl->voices2notes[x].note = -1;
    opl->voices2notes[x].timbreid = -1;
    opl->voices2notes[x].voiceindex = 0xFF;
  }

  /* mark all notes as unallocated */
  for (x = 0; x < 16; x++) {
    for (y = 0; y < 128; y++) {
      opl->notes2voices[x][y][0] = -1;
      opl->notes2voices[x][y][1] = -1;
    }
  }

  /* pre-set emulated channel patches to default GM ids and reset all
   * per-channel volumes */
  for (x = 0; x < 16; x++) {
    opl_midi_changeprog(opl,x, x);
    opl->channelvol[x] = 127;
  }
}



void opl_midi_pitchwheel(opl_t* opl, int channel, int pitchwheel) {
  int x;
  /* update the new pitch value for channel (used by newly played notes) */
//  opl->channelpitch[channel] = pitchwheel;
  /* check all active voices to see who is playing on given channel now, and
   * recompute all playing notes for this channel with the new pitch TODO */
  for (x = 0; x < voicescount; x++) {
    if (opl->voices2notes[x].channel != channel) continue;
    
    opl_timbre_t* timbre = opl->voices2notes[x].voiceindex == 0
      ? &(opl->opl_gmtimbres[opl->voices2notes[x].timbreid])
      : &(opl->opl_gmtimbres_voice2[opl->voices2notes[x].timbreid])
      ;
    opl_noteon(opl, x, opl->voices2notes[x].note + timbre->noteoffset, pitchwheel + timbre->finetune);
  }
}


void opl_midi_controller(opl_t* opl, int channel, int id, int value) {
  int x;
  switch (id) {
    case 11: /* "Expression" (meaning "channel volume") */
      opl->channelvol[channel] = value;
      break;
    case 123: /* 'all notes off' */
    case 120: /* 'all sound off' - I map it to 'all notes off' for now, not perfect but better than not handling it at all */
      for (x = 0; x < voicescount; x++) {
        if (opl->voices2notes[x].channel != channel) continue;
        opl_midi_noteoff(opl, channel, opl->voices2notes[x].note);
      }
      break;
  }
}


/* assign a new instrument to emulated MIDI channel */
void opl_midi_changeprog(opl_t* opl,int channel, int program) {
  if (channel == 9) return; /* do not allow to change channel 9, it is for percussions only */
  opl->channelprog[channel] = program;
}


void opl_loadinstrument(opl_t* opl, int voice, opl_timbre_t *timbre) {
  /* KSL (key level scaling) / attenuation */
  oplregwr(opl, 0x40 + op1offsets[voice], timbre->modulator_40);
  oplregwr(opl, 0x40 + op2offsets[voice], timbre->carrier_40 | 0x3f); /* force volume to 0, it will be reajusted during 'note on' */

  /* select waveform on both operators */
  oplregwr(opl, 0xE0 + op1offsets[voice], timbre->modulator_E862 >> 24);
  oplregwr(opl, 0xE0 + op2offsets[voice], timbre->carrier_E862 >> 24);

  /* sustain / release */
  oplregwr(opl, 0x80 + op1offsets[voice], (timbre->modulator_E862 >> 16) & 0xff);
  oplregwr(opl, 0x80 + op2offsets[voice], (timbre->carrier_E862 >> 16) & 0xff);

  /* attack rate / decay */
  oplregwr(opl, 0x60 + op1offsets[voice], (timbre->modulator_E862 >> 8) & 0xff);
  oplregwr(opl, 0x60 + op2offsets[voice], (timbre->carrier_E862 >> 8) & 0xff);

  /* AM / vibrato / envelope */
  oplregwr(opl, 0x20 + op1offsets[voice], timbre->modulator_E862 & 0xff);
  oplregwr(opl, 0x20 + op2offsets[voice], timbre->carrier_E862 & 0xff);

  /* feedback / connection */
  if (voice >= 9) {
    voice -= 9;
    voice |= 0x100;
  }
  if (opl->opl3 != 0) { /* on OPL3 make sure to enable LEFT/RIGHT unmute bits */
    oplregwr(opl, 0xC0 + voice, timbre->feedconn | 0x30);
  } else {
    oplregwr(opl, 0xC0 + voice, timbre->feedconn);
  }

}


/* adjust the volume of the voice (in the usual MIDI range of 0..127) */
static void voicevolume(opl_t* opl, unsigned short voice, const opl_timbre_t* timbre, int volume) {
  unsigned char carrierval = timbre->carrier_40;
  if (volume == 0) {
    carrierval |= 0x3f;
  } else {
    calc_vol(&carrierval, volume);
  }
  oplregwr(opl, 0x40 + op2offsets[voice], carrierval);
}


/* get the id of the instrument that relates to channel/note pair */
static int getinstrument(opl_t* opl, int channel, int note) {
  if ((note < 0) || (note > 127) || (channel > 15)) return(-1);
  if (channel == 9) { /* the percussion channel requires special handling */
    if (opl->is_op2)
      return 128 + note - 35;
    else
      return(128 | note);
  }
  return(opl->channelprog[channel]);
}

void opl_midi_noteoff_op2(opl_t* opl, int channel, int note, int vindex);

void opl_midi_noteon_op2(opl_t* opl, int channel, int note, int velocity, int vindex) {
  if( velocity == 0 ) {
      opl_midi_noteoff_op2( opl, channel, note, vindex );
      return;
  }
  int x, voice = -1;
  int lowestpriority = 0xFFFF;
  int highestvoiceindex = -1;
  int lowestpriorityvoice = -1;
  int instrument;

  /* get the instrument to play */
  instrument = getinstrument(opl, channel, note);
  if (instrument < 0) return;
  
  /* only play OP2 second voice when appropriate */
  if (vindex > 0 && (opl->op2_flags[instrument] & OP2_DOUBLEVOICE) == 0) return;
  
  opl_timbre_t* timbre = vindex == 0 ? &(opl->opl_gmtimbres[instrument]) : &(opl->opl_gmtimbres_voice2[instrument]);

  /* if note already playing, then reuse its voice to avoid leaving a stuck voice */
  if (opl->notes2voices[channel][note][vindex] >= 0) {
    voice = opl->notes2voices[channel][note][vindex];
  } else {
    /* else find a free voice, possibly with the right timbre, or at least locate the oldest note */
    for (x = 0; x < voicescount; x++) {
      if (opl->voices2notes[x].channel < 0) {
        voice = x; /* preselect this voice, but continue looking */
        /* if the instrument is right, do not look further */
        if (opl->voices2notes[x].timbreid == instrument && opl->voices2notes[x].voiceindex == vindex) {
          break;
        }
      }
      if (opl->voices2notes[x].priority < lowestpriority) {
        /* 2nd instrumental voice should not overwrite 1st instrumental voice */
        /* also prefer 2nd instrumental voices when possible */
        if (opl->voices2notes[x].voiceindex >= vindex && opl->voices2notes[x].voiceindex >= highestvoiceindex) {
          lowestpriorityvoice = x;
          lowestpriority = opl->voices2notes[x].priority;
          highestvoiceindex = opl->voices2notes[x].voiceindex;
        }
      }
    }
    /* if no free voice available, then abort the oldest one */
    if (voice < 0) {
      if (lowestpriorityvoice < 0) {
        /* no suitable voice found to abort */
        return;
      }
      voice = lowestpriorityvoice;
      opl_midi_noteoff_op2(opl, opl->voices2notes[voice].channel, opl->voices2notes[voice].note, opl->voices2notes[voice].voiceindex);
    }
  }

  /* load the proper instrument, if not already good */
  if (opl->voices2notes[voice].timbreid != instrument) {
    opl->voices2notes[voice].timbreid = instrument;
    opl_loadinstrument(opl, voice, timbre);
  }

  /* update states */
  opl->voices2notes[voice].channel = channel;
  opl->voices2notes[voice].note = note;
  opl->voices2notes[voice].priority = ((16 - channel) << 8) | 0xff; /* lower channels must have priority */
  opl->voices2notes[voice].voiceindex = vindex;
  opl->notes2voices[channel][note][vindex] = voice;
  
  /* second OP2 voice has lower priority */
  if (vindex != 0) {
    int reducedprio = (int)opl->voices2notes[voice].priority - OP2_2NDVOICE_PRIORITY_PENALTY;
    if (reducedprio < 0) reducedprio = 0;
    opl->voices2notes[voice].priority = (unsigned short)reducedprio;
  }

  /* set the requested velocity on the voice */
  voicevolume(opl, voice, timbre, velocity * opl->channelvol[channel] / 127);

  /* trigger NOTE_ON on the OPL, take care to apply the 'finetune' pitch correction, too */
  if (channel == 9) { /* percussion channel doesn't provide a real note, so I */
                      /* use a static one (MUSPLAYER uses C-5 (60), why not.  */
    opl_noteon(opl, voice, timbre->notenum + timbre->noteoffset, opl->channelpitch[channel] + timbre->finetune);
  } else {
    opl_noteon(opl, voice, note + timbre->noteoffset, opl->channelpitch[channel] + timbre->finetune);
  }

  /* reajust all priorities */
  for (x = 0; x < voicescount; x++) {
    if (opl->voices2notes[x].priority > 0) opl->voices2notes[x].priority -= 1;
  }
}

void opl_midi_noteon(opl_t* opl, int channel, int note, int velocity) {
  /* play 2nd instrumental voice first just in case */
  opl_midi_noteon_op2(opl, channel, note, velocity, 1);
  opl_midi_noteon_op2(opl, channel, note, velocity, 0);
}

void opl_midi_noteoff_op2(opl_t* opl, int channel, int note, int vindex) {
  int voice = opl->notes2voices[channel][note][vindex];

  if (voice >= 0) {
    opl_noteoff(opl, voice);
    opl->voices2notes[voice].channel = -1;
    opl->voices2notes[voice].note = -1;
    opl->voices2notes[voice].priority = -1;
    opl->voices2notes[voice].voiceindex = 0xFF;
    opl->notes2voices[channel][note][vindex] = -1;
  }
}

void opl_midi_noteoff(opl_t* opl, int channel, int note) {
    opl_midi_noteoff_op2(opl, channel, note, 0);
    opl_midi_noteoff_op2(opl, channel, note, 1);
}


static int opl_loadbank_internal(opl_t* opl, char const* file, int offset) {
  opl->is_op2 = 0;
  unsigned char buff[16];
  int i;
  /* open the IBK file */
  FILE* f = fopen( file, "rb" );
  if( !f ) return -1;
  /* file must be exactly 3204 bytes long */
  fseek( f, 0, SEEK_END );
  if (ftell(f) != 3204) {
    fclose(f);
    return(-2);
  }
  fseek( f, 0, SEEK_SET);
  /* file must start with an IBK header */
  if ((fread(buff, 1, 4,f) != 4) || (buff[0] != 'I') || (buff[1] != 'B') || (buff[2] != 'K') || (buff[3] != 0x1A)) {
    fclose(f);
    return(-3);
  }
  /* load 128 instruments from the IBK file */
  for (i = offset; i < 128 + offset; i++) {
    /* load instruments */
    if (fread(buff, 1, 16, f) != 16) {
      fclose(f);
      return(-4);
    }
    /* load modulator */
    opl->opl_gmtimbres[i].modulator_E862 = buff[8]; /* wave select */
    opl->opl_gmtimbres[i].modulator_E862 <<= 8;
    opl->opl_gmtimbres[i].modulator_E862 |= buff[6]; /* sust/release */
    opl->opl_gmtimbres[i].modulator_E862 <<= 8;
    opl->opl_gmtimbres[i].modulator_E862 |= buff[4]; /* attack/decay */
    opl->opl_gmtimbres[i].modulator_E862 <<= 8;
    opl->opl_gmtimbres[i].modulator_E862 |= buff[0]; /* AM/VIB... flags */
    /* load carrier */
    opl->opl_gmtimbres[i].carrier_E862 = buff[9]; /* wave select */
    opl->opl_gmtimbres[i].carrier_E862 <<= 8;
    opl->opl_gmtimbres[i].carrier_E862 |= buff[7]; /* sust/release */
    opl->opl_gmtimbres[i].carrier_E862 <<= 8;
    opl->opl_gmtimbres[i].carrier_E862 |= buff[5]; /* attack/decay */
    opl->opl_gmtimbres[i].carrier_E862 <<= 8;
    opl->opl_gmtimbres[i].carrier_E862 |= buff[1]; /* AM/VIB... flags */
    /* load KSL */
    opl->opl_gmtimbres[i].modulator_40 = buff[2];
    opl->opl_gmtimbres[i].carrier_40 = buff[3];
    /* feedconn & finetune */
    opl->opl_gmtimbres[i].feedconn = buff[10];
    opl->opl_gmtimbres[i].finetune = buff[12]; /* used only in some IBK files */
    opl->opl_gmtimbres[i].notenum = 60;
    opl->opl_gmtimbres[i].noteoffset = 0;
  }
  /* close file and return success */
  fclose(f);
  return(0);
}

/*
static void dump2file(void) {
  FILE *fd;
  int i;
  fd = fopen("dump.txt", "wb");
  if (fd == NULL) return;
  for (i = 0; i < 256; i++) {
    char *comma = "";
    if (i < 255) comma = ",";
    fprintf(fd, "{0x%07lX,0x%07lX,0x%02X,0x%02X,0x%02X,%d,%d}%s\r\n", opl->opl_gmtimbres[i].modulator_E862, opl->opl_gmtimbres[i].carrier_E862, opl->opl_gmtimbres[i].modulator_40, opl->opl_gmtimbres[i].carrier_40, opl->opl_gmtimbres[i].feedconn, opl->opl_gmtimbres[i].finetune, opl->opl_gmtimbres[i].notenum, comma);
  }
  fclose(fd);
}
*/
int opl_loadbank_ibk(opl_t* opl, char const* file) {
  char *instruments = NULL, *percussion = NULL;
  int i, res;
  instruments = strdup(file); /* duplicate the string so we can modify it */
  if (instruments == NULL) return(-64); /* out of mem */
  /* if a second file is provided, it's for percussion */
  for (i = 0; instruments[i] != 0; i++) {
    if (instruments[i] == ',') {
      instruments[i] = 0;
      percussion = instruments + i + 1;
      break;
    }
  }
  /* load the file(s) */
  res = opl_loadbank_internal(opl, instruments, 0);
  if ((res == 0) && (percussion != NULL)) {
    res = opl_loadbank_internal(opl, percussion, 128);
  }
  free(instruments);
  /*dump2file();*/ /* dump instruments to a 'dump.txt' file */
  return(res);
}

static void opl_load_op2_voice(opl_timbre_t* timbre, uint8_t const* buff) {
  /* load modulator */
  timbre->modulator_E862 = buff[3]; /* wave select */
  timbre->modulator_E862 <<= 8;
  timbre->modulator_E862 |= buff[2]; /* sust/release */
  timbre->modulator_E862 <<= 8;
  timbre->modulator_E862 |= buff[1]; /* attack/decay */
  timbre->modulator_E862 <<= 8;
  timbre->modulator_E862 |= buff[0]; /* AM/VIB... flags */
  /* load carrier */
  timbre->carrier_E862 = buff[10]; /* wave select */
  timbre->carrier_E862 <<= 8;
  timbre->carrier_E862 |= buff[9]; /* sust/release */
  timbre->carrier_E862 <<= 8;
  timbre->carrier_E862 |= buff[8]; /* attack/decay */
  timbre->carrier_E862 <<= 8;
  timbre->carrier_E862 |= buff[7]; /* AM/VIB... flags */
  /* load KSL */
  timbre->modulator_40 = ( buff[5] & 0x3f ) | ( buff[4] & 0xc0 );
  timbre->carrier_40 = ( buff[12] & 0x3f ) | ( buff[11] & 0xc0 );
  /* feedconn & finetune */
  timbre->feedconn = buff[6];
  timbre->finetune = 0;
  timbre->noteoffset = (int16_t)(buff[14] | ((uint16_t)buff[15] << 8));
}

int opl_loadbank_op2(opl_t* opl, void const* data, int size ) {
  if( size < 8 + 36 * 175 ) {
      return -3;
  }
  uint8_t const* buff = (uint8_t const*) data;
  int i;
  /* file must start with an #OPL_II# header */
  if ((buff[0] != '#') || (buff[1] != 'O') || (buff[2] != 'P') || (buff[3] != 'L') || (buff[4] != '_') || (buff[5] != 'I') || (buff[6] != 'I') || (buff[7] != '#')) {
    return(-3);
  }
  buff += 8;
  
  opl->is_op2 = 1;

  /* load 128 instruments from the IBK file */
  for (i = 0; i < 175; i++) {
    /* load instruments */
    
    /* OP2 instrument header */
    opl->op2_flags[i] = (enum op2_flags_t)( buff[0] | ((uint16_t)buff[1] << 8) );
    int finetune = buff[2];
    uint8_t fixednote = buff[3];
    buff += 4;
    
    /* first voice */
    opl_load_op2_voice(&opl->opl_gmtimbres[i], buff);
    opl->opl_gmtimbres[i].notenum = fixednote;
    buff += 16;

    /* second voice */
    opl_load_op2_voice(&opl->opl_gmtimbres_voice2[i], buff);
    opl->opl_gmtimbres_voice2[i].notenum = fixednote;
    opl->opl_gmtimbres_voice2[i].finetune += finetune - 128;
    buff += 16;
  }
  /* close file and return success */
  return(0);
}



#endif /* OPL_IMPLEMENTATION */

/*
OPL_EMU parts are from https://github.com/aaronsgiles/ymfm
License:

BSD 3-Clause License

Copyright (c) 2021, Aaron Giles
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
Other parts are from http://dosmid.sourceforge.net
License:

Library to access OPL2/OPL3 hardware (YM3812 / YMF262)

Copyright (C) 2015-2016 Mateusz Viste
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

