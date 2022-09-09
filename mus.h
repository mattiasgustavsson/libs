/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

mus.h - v0.1 - Parsing library for MUS music files (as used in DOS games).

Do this:
    #define MUS_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef mus_h
#define mus_h

#include <stddef.h>

typedef struct mus_t mus_t;
    
mus_t* mus_create( void const* data, size_t size, void* memctx );
void mus_destroy( mus_t* mus ); 

typedef enum mus_cmd_t {
    MUS_CMD_RELEASE_NOTE,
    MUS_CMD_PLAY_NOTE,
    MUS_CMD_PITCH_BEND,
    MUS_CMD_SYSTEM_EVENT,
    MUS_CMD_CONTROLLER,
    MUS_CMD_END_OF_MEASURE,
    MUS_CMD_FINISH,
    MUS_CMD_RENDER_SAMPLES,
} mus_cmd_t;

struct mus_cmd_release_note_t {
    int note;
};

struct mus_cmd_play_note_t {
    int note;
    int volume;
};

struct mus_cmd_pitch_bend_t {
    int bend_amount;
};

enum mus_system_event_t {
    MUS_SYSTEM_EVENT_ALL_SOUNDS_OFF,
    MUS_SYSTEM_EVENT_ALL_NOTES_OFF,
    MUS_SYSTEM_EVENT_MONO,
    MUS_SYSTEM_EVENT_POLY,
    MUS_SYSTEM_EVENT_RESET_ALL_CONTROLLERS,
};

struct mus_cmd_system_event_t {
    enum mus_system_event_t event;
};

enum mus_controller_t {
    MUS_CONTROLLER_CHANGE_INSTRUMENT,
    MUS_CONTROLLER_BANK_SELECT,
    MUS_CONTROLLER_MODULATION,
    MUS_CONTROLLER_VOLUME,
    MUS_CONTROLLER_PAN,
    MUS_CONTROLLER_EXPRESSION,
    MUS_CONTROLLER_REVERB_DEPTH,
    MUS_CONTROLLER_CHORUS_DEPTH,
    MUS_CONTROLLER_SUSTAIN_PEDAL,
    MUS_CONTROLLER_SOFT_PEDAL,
};

struct mus_cmd_controller_t {
    enum mus_controller_t controller;
    int value;
};

struct mus_cmd_render_samples_t {
    int samples_count;
};

union mus_cmd_data_t {
    struct mus_cmd_release_note_t release_note;
    struct mus_cmd_play_note_t play_note;
    struct mus_cmd_pitch_bend_t pitch_bend;
    struct mus_cmd_system_event_t system_event;
    struct mus_cmd_controller_t controller;
    struct mus_cmd_render_samples_t render_samples;
};

typedef struct mus_event_t {
    int channel;
    enum mus_cmd_t cmd;
    union mus_cmd_data_t data;
} mus_event_t;


void mus_next_event( mus_t* mus, mus_event_t* event );

void mus_restart( mus_t* mus );

#endif /* mus_h */


/*
----------------------
    IMPLEMENTATION
----------------------
*/

#ifdef MUS_IMPLEMENTATION
#undef MUS_IMPLEMENTATION

#if !defined( MUS_MALLOC ) || !defined( MUS_FREE )
    #include <stdlib.h>
    #define MUS_MALLOC  malloc
    #define MUS_FREE    free
#endif

struct mus_t {
    int note_volume[ 16 ];
    int accumulated_delay;
    int pos;
    int length;
    uint8_t data[ 1 ]; // "open" array
};

mus_t* mus_create( void const* data, size_t size, void* memctx ) {
    (void) memctx, (void) size;
    uintptr_t ptr = (uintptr_t) data;
    uint32_t sig = *(uint32_t*) ptr;
    ptr += 4;
    if( sig != 0x1a53554d ) { // Identifier "MUS" followed by 0x1A
        return NULL;
    }
    int length = *(uint16_t*) ptr;
    ptr += 2;

    int offset = *(uint16_t*) ptr;
    ptr += 2;

    if( (size_t)( length + offset ) > size ) {
        return NULL;
    }

    mus_t* mus = (mus_t*) MUS_MALLOC( sizeof( mus_t ) + length - 1 );
    mus->length = length;
    memcpy( mus->data, (void*)( ((uintptr_t)data) + offset ), length );
    mus_restart( mus );
    return mus;
}


void mus_destroy( mus_t* mus ) {
    MUS_FREE( mus );
}


void mus_next_event( mus_t* mus, mus_event_t* event ) {
    if( mus->accumulated_delay ) {
        int samples_count = ( mus->accumulated_delay * 44100 ) / 140;
        event->channel = 0;
        event->cmd = MUS_CMD_RENDER_SAMPLES;
        event->data.render_samples.samples_count = samples_count;
        mus->accumulated_delay = 0;
        return;
    }
    while( mus->pos < mus->length ) {
        uint8_t data = mus->data[ mus->pos++ ];
        int channel = data & 15;
        int type = ( data >> 4 ) & 7;
        int last = ( data >> 7 ) & 1;
        int event_valid = 0;
        switch( type ) {
            case 0: { // Release Note
                data = mus->data[ mus->pos++ ];
                int note = data & 127;
                event->channel = channel;
                event->cmd = MUS_CMD_RELEASE_NOTE;
                event->data.release_note.note = note;
                event_valid = 1;
            } break;

            case 1: { // Play Note
                data = mus->data[ mus->pos++ ];
                int note = data & 127;
                int has_vol = ( data >> 7 );
                if( has_vol ) {
                    data = mus->data[ mus->pos++ ];
                    mus->note_volume[ channel ] = data;
                }
                event->channel = channel;
                event->cmd = MUS_CMD_PLAY_NOTE;
                event->data.play_note.note = note;
                event->data.play_note.volume = mus->note_volume[ channel ];
                event_valid = 1;
            } break;

            case 2: { // Pitch Bend
                data = mus->data[ mus->pos++ ];
                int bend_amount = data;
                event->channel = channel;
                event->cmd = MUS_CMD_PITCH_BEND;
                event->data.pitch_bend.bend_amount = bend_amount;
                event_valid = 1;
            } break;

            case 3: { // System Event
                data = mus->data[ mus->pos++ ];
                int sysevent = data & 127;
                event->channel = channel;
                event->cmd = MUS_CMD_SYSTEM_EVENT;
                event->data.system_event.event = (enum mus_system_event_t)sysevent;
                event_valid = 1;
            } break;

            case 4: { // Controller
                data = mus->data[ mus->pos++ ];
                int controller = data & 127;
                data = mus->data[ mus->pos++ ];
                int value = data & 127;
                event->channel = channel;
                event->cmd = MUS_CMD_CONTROLLER;
                event->data.controller.controller = (enum mus_controller_t)controller;
                event->data.controller.value = value;
                event_valid = 1;
            } break;

            case 5: { // End of Measure
                event->channel = channel;
                event->cmd = MUS_CMD_END_OF_MEASURE;
                event_valid = 1;
            } break;

            case 6: { // Finish
                event->channel = channel;
                event->cmd = MUS_CMD_END_OF_MEASURE;
                event_valid = 1;
            } break;

            case 7: { // Unused
                data = mus->data[ mus->pos++ ];
            } break;
        }

        if( last ) {
            int delay = 0;
            for( ; ; ) {
                data = mus->data[ mus->pos++ ];
                delay = delay * 128 + ( data & 127 );
                if( ( data >> 7 ) == 0 ) {
                    if( event_valid ) {
                        mus->accumulated_delay = delay;
                    } else {
                        int samples_count = ( delay * 44100 ) / 140;
                        event->channel = 0;
                        event->cmd = MUS_CMD_RENDER_SAMPLES;
                        event->data.render_samples.samples_count = samples_count;
                        event_valid = 1;
                    }
                    break;
                }
            }
        }
        if( event_valid ) {
            return;
        }
    }

    event->channel = 0;
    event->cmd = MUS_CMD_FINISH;
}


void mus_restart( mus_t* mus ) {
    mus->accumulated_delay = 0;
    for( int i = 0; i < sizeof( mus->note_volume ) / sizeof( *mus->note_volume ); ++i ){
        mus->note_volume[ i ] = 127;
    }
    mus->pos = 0;
}


#endif /* MUS_IMPLEMENTATION */

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2021 Mattias Gustavsson

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

------------------------------------------------------------------------------

ALTERNATIVE B - Public Domain (www.unlicense.org)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this 
software, either in source code form or as a compiled binary, for any purpose, 
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this 
software dedicate any and all copyright interest in the software to the public 
domain. We make this dedication for the benefit of the public at large and to 
the detriment of our heirs and successors. We intend this dedication to be an 
overt act of relinquishment in perpetuity of all present and future rights to 
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

------------------------------------------------------------------------------
*/
