/*
 * Copyright (C) 2015-2016 Alexey Khokholov (Nuke.YKT)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "driver.h"
#include "gmtimbre.h"

#define OPL_LSI             0x00
#define OPL_TIMER           0x04
#define OPL_4OP             0x104
#define OPL_NEW             0x105
#define OPL_NTS             0x08
#define OPL_MULT            0x20
#define OPL_TL              0x40
#define OPL_AD              0x60
#define OPL_SR              0x80
#define OPL_WAVE            0xe0
#define OPL_FNUM            0xa0
#define OPL_BLOCK           0xb0
#define OPL_RHYTHM          0xbd
#define OPL_FEEDBACK        0xc0

#define MIDI_DRUMCHANNEL    9
#define MIDI_NOTEOFF        0x80
#define MIDI_NOTEON         0x90
#define MIDI_CONTROL        0xb0
#define MIDI_CONTROL_VOL    0x07
#define MIDI_CONTROL_BAL    0x08
#define MIDI_CONTROL_PAN    0x0a
#define MIDI_CONTROL_SUS    0x40
#define MIDI_CONTROL_ALLOFF 0x78
#define MIDI_PROGRAM        0xc0
#define MIDI_PITCHBEND      0xe0

const uint8_t opl_volume_map[32] = {
    80, 63, 40, 36, 32, 28, 23, 21,
    19, 17, 15, 14, 13, 12, 11, 10,
     9,  8,  7,  6,  5,  5,  4,  4,
     3,  3,  2,  2,  1,  1,  0,  0
};

const uint8_t opl_voice_map[9] = {
    0, 1, 2, 8, 9, 10, 16, 17, 18
};

typedef struct
{
    opl_timbre *timbre;
    int32_t pitch;
    uint16_t volume;
    uint16_t pan;
    bool sustained;
} opl_channel;

typedef struct
{
    uint16_t num;
    uint16_t mod, car;
    uint32_t freq;
    uint32_t freqpitched;
    uint32_t time;
    uint8_t note;
    uint8_t velocity;
    bool keyon;
    bool sustained;
    opl_timbre* timbre;
    opl_channel *channel;
} opl_voice;

#define opl_pitchfrac 8

static uint16_t opl_voice_num;
static opl_channel opl_channels[16];
static opl_voice opl_voices[18];
static uint32_t opl_freq[12];
static uint32_t opl_time;
static int16_t opl_uppitch;
static int16_t opl_downpitch;

static bool opl_opl3mode;

extern void opl_writereg(uint32_t reg, uint8_t data);

static void opl_buildfreqtable(void) {
    opl_freq[0]  = 2743;
    opl_freq[1]  = 2906;
    opl_freq[2]  = 3079;
    opl_freq[3]  = 3262;
    opl_freq[4]  = 3456;
    opl_freq[5]  = 3661;
    opl_freq[6]  = 3879;
    opl_freq[7]  = 4110;
    opl_freq[8]  = 4354;
    opl_freq[9]  = 4613;
    opl_freq[10] = 4888;
    opl_freq[11] = 5178;
    opl_uppitch = 31;
    opl_downpitch = 27;
}

static uint32_t opl_calcblock(uint32_t freq) {
    uint8_t block = 1;
    while (freq > 0x3ff) {
        block++;
        freq /= 2;
    }
    if (block > 0x07) {
        block = 0x07;
    }
    return (block << 10) | freq;
}

static uint32_t opl_applypitch(uint32_t freq, int32_t pitch) {
    int32_t diff;
    if (pitch > 0) {
        diff = (pitch * opl_uppitch) >> opl_pitchfrac;
        freq += (diff*freq) >> 15;
    } else if (pitch < 0) {
        diff = (-pitch * opl_downpitch) >> opl_pitchfrac;
        freq -= (diff*freq) >> 15;
    }
    return freq;
}

static opl_voice* opl_allocvoice(opl_timbre *timbre) {
    uint32_t time;
    int16_t id;
    uint16_t i;

    for (i = 0; i < opl_voice_num; i++) {
        if (opl_voices[i].time == 0) {
            return &opl_voices[i];
        }
    }

    time = 0xffffffffUL;
    id = -1;

    for (i = 0; i < opl_voice_num; i++) {
        if (!opl_voices[i].keyon && opl_voices[i].time < time) {
            id = i;
            time = opl_voices[i].time;
        }
    }
    if (id >= 0) {
        return &opl_voices[id];
    }

    for (i = 0; i < opl_voice_num; i++) {
        if (opl_voices[i].timbre == timbre && opl_voices[i].time < time) {
            id = i;
            time = opl_voices[i].time;
        }
    }
    if (id >= 0) {
        return &opl_voices[id];
    }

    for (i = 0; i < opl_voice_num; i++) {
        if (opl_voices[i].time < time) {
            id = i;
            time = opl_voices[i].time;
        }
    }

    return &opl_voices[id];
}

static opl_voice* opl_findvoice(opl_channel *channel, uint8_t note) {
    uint16_t i;
    for (i = 0; i < opl_voice_num; i++) {
        if (opl_voices[i].keyon && opl_voices[i].channel == channel && opl_voices[i].note == note) {
            return &opl_voices[i];
        }
    }
    return 0;
}

static void opl_midikeyon(opl_channel *channel, uint8_t note, opl_timbre *timbre, uint8_t velocity) {
    opl_voice *voice;
    uint32_t freq;
    uint32_t freqpitched;
    uint16_t octave;
    uint16_t carvol;
    uint16_t modvol;
    uint8_t fb;

    octave = note / 12;
    freq = opl_freq[note % 12];
    if (octave < 5) {
        freq >>= (5 - octave);
    } else if (octave > 5) {
        freq <<= (octave - 5);
    }

    if (timbre->octave < 4) {
        freq >>= (4 - timbre->octave);
    } else if (timbre->octave > 4) {
        freq >>= (timbre->octave - 4);
    }

    freqpitched = opl_calcblock(opl_applypitch(freq, channel->pitch));

    carvol = (timbre->tl[1] & 0x3f) + channel->volume + opl_volume_map[velocity >> 2];
    modvol = timbre->tl[0] & 0x3f;

    if (timbre->fb & 0x01) {
        modvol += channel->volume + opl_volume_map[velocity >> 2];
    }

    if (carvol > 0x3f) {
        carvol = 0x3f;
    }

    if (modvol > 0x3f) {
        modvol = 0x3f;
    }

    carvol |= (timbre->tl[1] & 0xc0);
    modvol |= (timbre->tl[0] & 0xc0);

    fb = timbre->fb & channel->pan;

    voice = opl_allocvoice(timbre);

    opl_writereg(OPL_BLOCK + voice->num, 0x00);

    opl_writereg(OPL_MULT + voice->mod, timbre->mult[0]);
    opl_writereg(OPL_TL + voice->mod, modvol);
    opl_writereg(OPL_AD + voice->mod, timbre->ad[0]);
    opl_writereg(OPL_SR + voice->mod, timbre->sr[0]);
    opl_writereg(OPL_WAVE + voice->mod, timbre->wf[0]);

    opl_writereg(OPL_MULT + voice->car, timbre->mult[1]);
    opl_writereg(OPL_TL + voice->car, carvol);
    opl_writereg(OPL_AD + voice->car, timbre->ad[1]);
    opl_writereg(OPL_SR + voice->car, timbre->sr[1]);
    opl_writereg(OPL_WAVE + voice->car, timbre->wf[1]);

    opl_writereg(OPL_FNUM + voice->num, freqpitched & 0xff);
    opl_writereg(OPL_FEEDBACK + voice->num, fb);
    opl_writereg(OPL_BLOCK + voice->num, (freqpitched >> 8) | 0x20);

    voice->freq = freq;
    voice->freqpitched = freqpitched;
    voice->note = note;
    voice->velocity = velocity;
    voice->timbre = timbre;
    voice->channel = channel;
    voice->time = opl_time++;
    voice->keyon = true;
    voice->sustained = false;
}

static void opl_midikeyoff(opl_channel *channel, uint8_t note, opl_timbre *timbre, bool sustained)
{
    opl_voice *voice;
    (void)timbre;
    voice = opl_findvoice(channel, note);
    if (!voice) {
        return;
    }

    if (sustained) {
        voice->sustained = true;
        return;
    }

    opl_writereg(OPL_BLOCK + voice->num, voice->freqpitched >> 8);
    voice->keyon = false;
    voice->time = opl_time;
}

static void opl_midikeyoffall(opl_channel *channel) {
    uint16_t i;
    for (i = 0; i < opl_voice_num; i++) {
        if (opl_voices[i].channel == channel) {
            opl_midikeyoff(opl_voices[i].channel, opl_voices[i].note, opl_voices[i].timbre, false);
        }
    }
}

static void opl_updatevolpan(opl_channel *channel) {
    uint16_t i;
    uint16_t modvol;
    uint16_t carvol;

    for (i = 0; i < opl_voice_num; i++) {
        if (opl_voices[i].channel == channel) {
            carvol = (opl_voices[i].timbre->tl[1] & 0x3f) + channel->volume + opl_volume_map[opl_voices[i].velocity >> 2];
            modvol = opl_voices[i].timbre->tl[0] & 0x3f;

            if (opl_voices[i].timbre->fb & 0x01) {
                modvol += channel->volume + opl_volume_map[opl_voices[i].velocity >> 2];
            }

            if (carvol > 0x3f) {
                carvol = 0x3f;
            }

            if (modvol > 0x3f) {
                modvol = 0x3f;
            }

            carvol |= (opl_voices[i].timbre->tl[1] & 0xc0);
            modvol |= (opl_voices[i].timbre->tl[0] & 0xc0);

            opl_writereg(OPL_TL + opl_voices[i].mod, modvol);
            opl_writereg(OPL_TL + opl_voices[i].car, carvol);

            opl_writereg(OPL_FEEDBACK + opl_voices[i].num, opl_voices[i].timbre->fb & channel->pan);
        }
    }
}

static void opl_updatevol(opl_channel *channel, uint8_t vol)
{
    channel->volume = opl_volume_map[vol >> 2];
    opl_updatevolpan(channel);
}

static void opl_updatepan(opl_channel *channel, uint8_t pan)
{
    if (pan < 48) {
        channel->pan = 0xdf;
    } else if(pan > 80) {
        channel->pan = 0xef;
    } else {
        channel->pan = 0xff;
    }
    opl_updatevolpan(channel);
}

static void opl_updatesustain(opl_channel *channel, uint8_t sustain) {
    uint16_t i;

    if (sustain >= 64) {
        channel->sustained = true;
    } else {
        channel->sustained = false;
        for (i = 0; i < opl_voice_num; i++) {
            if (opl_voices[i].channel == channel && opl_voices[i].sustained) {
                opl_midikeyoff(channel, opl_voices[i].note, opl_voices[i].timbre, false);
            }
        }
    }
}

static void opl_updatepitch(opl_channel *channel)
{
    uint16_t i;
    uint32_t freqpitch;

    for (i = 0; i < opl_voice_num; i++) {
        if (opl_voices[i].channel == channel) {
            freqpitch = opl_calcblock(opl_applypitch(opl_voices[i].freq, channel->pitch));
            opl_voices[i].freqpitched = freqpitch;
            opl_writereg(OPL_BLOCK + opl_voices[i].num, (freqpitch >> 8) | ((!!opl_voices[i].keyon) << 5));
            opl_writereg(OPL_FNUM + opl_voices[i].num, freqpitch & 0xff);
        }
    }
}

static void opl_midicontrol(opl_channel *channel, uint8_t type, uint8_t data)
{
    switch (type)
    {
    case MIDI_CONTROL_VOL:
        opl_updatevol(channel, data);
        break;
    case MIDI_CONTROL_BAL:
    case MIDI_CONTROL_PAN:
        opl_updatepan(channel, data);
        break;
    case MIDI_CONTROL_SUS:
        opl_updatesustain(channel, data);
        break;
    default:
        if (type >= MIDI_CONTROL_ALLOFF)
        {
            opl_midikeyoffall(channel);
        }
    }
}

static void opl_midiprogram(opl_channel *channel, uint8_t program)
{
    if (channel != &opl_channels[MIDI_DRUMCHANNEL]) {
        channel->timbre = &opl_timbres[program];
    }
}

static void opl_midipitchbend(opl_channel *channel, uint8_t parm1, uint8_t parm2)
{
    int16_t pitch;
    pitch = (parm2 << 9) | (parm1 << 2);
    pitch += 0x7fff;
    channel->pitch = pitch;
    opl_updatepitch(channel);
}


bool OPLWIN_MIDI_init(int16_t opltype)
{
    uint16_t i;
    opl_opl3mode = (opltype == 3) ? true : false;

    opl_writereg(OPL_LSI, 0x00);
    opl_writereg(OPL_TIMER, 0x60);
    opl_writereg(OPL_NTS, 0x00);
    if (opl_opl3mode) {
        opl_writereg(OPL_NEW, 0x01);
        opl_writereg(OPL_4OP, 0x00);
    }
    opl_writereg(OPL_RHYTHM, 0xc0);

    for (i = 0; i <= 0x15; i++) {
        opl_writereg(OPL_TL + i, 0x3f);
        if (opl_opl3mode) {
            opl_writereg(OPL_TL + 0x100 + i, 0x3f);
        }
    }

    for (i = 0; i < 9; i++) {
        opl_writereg(OPL_BLOCK + i, 0x00);
        if (opl_opl3mode) {
            opl_writereg(OPL_BLOCK + 0x100 + i, 0x00);
        }
    }

    opl_voice_num = 9;

    if (opl_opl3mode) {
        opl_voice_num = 18;
    }

    for (i = 0; i < opl_voice_num; i++) {
        opl_voices[i].num = i % 9;
        opl_voices[i].mod = opl_voice_map[i % 9];
        opl_voices[i].car = opl_voice_map[i % 9] + 3;
        if (i >= 9) {
            opl_voices[i].num += 0x100;
            opl_voices[i].mod += 0x100;
            opl_voices[i].car += 0x100;
        }
        opl_voices[i].freq = 0;
        opl_voices[i].freqpitched = 0;
        opl_voices[i].time = 0;
        opl_voices[i].note = 0;
        opl_voices[i].velocity = 0;
        opl_voices[i].keyon = false;
        opl_voices[i].sustained = false;
        opl_voices[i].timbre = &opl_timbres[0];
        opl_voices[i].channel = &opl_channels[0];
    }

    for (i = 0; i < 16; i++) {
        opl_channels[i].timbre = &opl_timbres[0];
        opl_channels[i].pitch = 0;
        opl_channels[i].volume = 0;
        opl_channels[i].pan = 0xff;
        opl_channels[i].sustained = false;
    }

    opl_buildfreqtable();

    opl_time = 1;

    return true;
}

void OPLWIN_MIDI_write(uint32_t data)
{
    uint8_t* msg = (uint8_t*)&data;
    uint8_t event_type = msg[1] & 0xf0;
    uint8_t channel = msg[1] & 0x0f;
    uint8_t parm1 = msg[2] & 0x7f;
    uint8_t parm2 = msg[3] & 0x7f;
    opl_channel *channelp = &opl_channels[channel];

    switch (event_type)
    {
    case MIDI_NOTEON:
        if (parm2 > 0) {
            if (channel == MIDI_DRUMCHANNEL) {
                if (opl_drum_maps[parm1].base != 0xff) {
                    opl_midikeyon(channelp, opl_drum_maps[parm1].note, &opl_timbres[opl_drum_maps[parm1].base + 128], parm2);
                }
            } else {
                opl_midikeyon(channelp, parm1, channelp->timbre, parm2);
            }
            break;
        }
    case MIDI_NOTEOFF:
        if (channel == MIDI_DRUMCHANNEL) {
            if (opl_drum_maps[parm1].base != 0xff) {
                opl_midikeyoff(channelp, opl_drum_maps[parm1].note, &opl_timbres[opl_drum_maps[parm1].base + 128], false);
            }
        } else {
            opl_midikeyoff(channelp, parm1, channelp->timbre, channelp->sustained);
        }
        break;
    case MIDI_CONTROL:
        opl_midicontrol(channelp, parm1, parm2);
        break;
    case MIDI_PROGRAM:
        opl_midiprogram(channelp, parm1);
        break;
    case MIDI_PITCHBEND:
        opl_midipitchbend(channelp, parm1, parm2);
        break;
    }
}

void OPLWIN_MIDI_panic(void)
{
    uint16_t c;
    for (c = 0; c < 16; ++c) {
        opl_midikeyoffall(&opl_channels[c]);
    }
}

void OPLWIN_MIDI_reset(void)
{
    uint16_t i;
    OPLWIN_MIDI_panic();
    for (i = 0; i < 16; i++) {
        opl_channels[i].timbre = &opl_timbres[0];
        opl_channels[i].pitch = 0;
        opl_channels[i].volume = 0;
        opl_channels[i].pan = 0xff;
        opl_channels[i].sustained = false;
    }
}
