/*
------------------------------------------------------------------------------
		  Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

audiosys.h - v0.1 - Sound and music playback (mixing only) for C/C++.

Do this:
	#define AUDIOSYS_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/

#ifndef audiosys_h
#define audiosys_h

#ifndef AUDIOSYS_S16
	#define AUDIOSYS_S16 signed short
#endif

#ifndef AUDIOSYS_U64
	#define AUDIOSYS_U64 unsigned long long
#endif

#define AUDIOSYS_DEFAULT_VOICE_COUNT 16

typedef struct audiosys_audio_source_t {
	void* instance;
	void (*release)( void* instance );
	int (*read_samples)( void* instance, float* sample_pairs, int sample_pairs_count );
	void (*restart)( void* instance );
	void (*set_position)( void* instance, int position_in_sample_pairs );
	int (*get_position)( void* instance );
} audiosys_audio_source_t;


typedef struct audiosys_t audiosys_t;

audiosys_t* audiosys_create( int active_voice_count, void* memctx );
void audiosys_destroy( audiosys_t* audiosys );

void audiosys_render( audiosys_t* audiosys, AUDIOSYS_S16* output_sample_pairs, int sample_pairs_count );

void audiosys_master_volume_set( audiosys_t* audiosys, float volume );
float audiosys_master_volume( audiosys_t* audiosys );

void audiosys_gain_set( audiosys_t* audiosys, float gain );
float audiosys_gain( audiosys_t* audiosys );

void audiosys_pause( audiosys_t* audiosys );
void audiosys_resume( audiosys_t* audiosys );

void audiosys_stop_all( audiosys_t* audiosys );

typedef enum audiosys_paused_t {
	AUDIOSYS_NOT_PAUSED,
	AUDIOSYS_PAUSED,
} audiosys_paused_t;

audiosys_paused_t audiosys_paused( audiosys_t* audiosys );


typedef enum audiosys_loop_t {
	AUDIOSYS_LOOP_OFF,
	AUDIOSYS_LOOP_ON,
} audiosys_loop_t;

void audiosys_music_play( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_in_time );
void audiosys_music_stop( audiosys_t* audiosys, float fade_out_time );
void audiosys_music_pause( audiosys_t* audiosys );
void audiosys_music_resume( audiosys_t* audiosys );
void audiosys_music_switch( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_out_time, float fade_in_time );
void audiosys_music_cross_fade( audiosys_t* audiosys, audiosys_audio_source_t source, float cross_fade_time );
void audiosys_music_position_set( audiosys_t* audiosys, float position );
float audiosys_music_position( audiosys_t* audiosys );
audiosys_audio_source_t audiosys_music_source( audiosys_t* audiosys );
void audiosys_music_loop_set( audiosys_t* audiosys, audiosys_loop_t loop );
audiosys_loop_t audiosys_music_loop( audiosys_t* audiosys );
void audiosys_music_volume_set( audiosys_t* audiosys, float volume );
float audiosys_music_volume( audiosys_t* audiosys );
void audiosys_music_pan_set( audiosys_t* audiosys, float pan );
float audiosys_music_pan( audiosys_t* audiosys );

void audiosys_ambience_play( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_in_time );
void audiosys_ambience_stop( audiosys_t* audiosys, float fade_out_time );
void audiosys_ambience_pause( audiosys_t* audiosys );
void audiosys_ambience_resume( audiosys_t* audiosys );
void audiosys_ambience_switch( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_out_time, float fade_in_time );
void audiosys_ambience_cross_fade( audiosys_t* audiosys, audiosys_audio_source_t source, float cross_fade_time );
void audiosys_ambience_position_set( audiosys_t* audiosys, float position );
float audiosys_ambience_position( audiosys_t* audiosys );
audiosys_audio_source_t audiosys_ambience_source( audiosys_t* audiosys );
void audiosys_ambience_loop_set( audiosys_t* audiosys, audiosys_loop_t loop );
audiosys_loop_t audiosys_ambience_loop( audiosys_t* audiosys );
void audiosys_ambience_volume_set( audiosys_t* audiosys, float volume );
float audiosys_ambience_volume( audiosys_t* audiosys );
void audiosys_ambience_pan_set( audiosys_t* audiosys, float pan );
float audiosys_ambience_pan( audiosys_t* audiosys );

AUDIOSYS_U64 audiosys_sound_play( audiosys_t* audiosys, audiosys_audio_source_t source, float priority, float fade_in_time );
void audiosys_sound_stop( audiosys_t* audiosys, AUDIOSYS_U64 handle, float fade_out_time );
void audiosys_sound_pause( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_resume( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_position_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float position );
float audiosys_sound_position( audiosys_t* audiosys, AUDIOSYS_U64 handle );
audiosys_audio_source_t audiosys_sound_source( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_loop_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, audiosys_loop_t loop );
audiosys_loop_t audiosys_sound_loop( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_volume_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float volume );
float audiosys_sound_volume( audiosys_t* audiosys, AUDIOSYS_U64 handle );
void audiosys_sound_pan_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float pan );
float audiosys_sound_pan( audiosys_t* audiosys, AUDIOSYS_U64 handle );


typedef enum audiosys_sound_valid_t {
	AUDIOSYS_SOUND_INVALID,
	AUDIOSYS_SOUND_VALID,
} audiosys_sound_valid_t;

audiosys_sound_valid_t audiosys_sound_valid(audiosys_t* audiosys, AUDIOSYS_U64 handle );

#endif /* audiosys_h */

/*
----------------------
	IMPLEMENTATION
----------------------
*/

#ifdef AUDIOSYS_IMPLEMENTATION
#undef AUDIOSYS_IMPLEMENTATION

#ifndef AUDIOSYS_MALLOC
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <stdlib.h>
	#define AUDIOSYS_MALLOC( ctx, size ) ( malloc( size ) )
	#define AUDIOSYS_FREE( ctx, ptr ) ( free( ptr ) )
#endif

#ifndef AUDIOSYS_ASSERT
	#undef _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#undef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#include <assert.h>
	#define AUDIOSYS_ASSERT( expression, message ) assert( ( expression ) && ( message ) )
#endif

#ifndef AUDIOSYS_MEMCPY
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define AUDIOSYS_MEMCPY( dst, src, cnt ) ( memcpy( (dst), (src), (cnt) ) )
#endif 

#ifndef AUDIOSYS_MEMSET
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define AUDIOSYS_MEMSET( dst, val, cnt ) ( memset( (dst), (val), (cnt) ) )
#endif 

#ifndef AUDIOSYS_MEMMOVE
	#define _CRT_NONSTDC_NO_DEPRECATE 
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define AUDIOSYS_MEMMOVE( dst, src, cnt ) ( memmove((dst), (src), (cnt) ) )
#endif 


typedef struct audiosys_internal_handles_data_t {
	int index;
	int counter;
} audiosys_internal_handles_data_t;


typedef struct audiosys_internal_handles_t {
	void* memctx;
	struct audiosys_internal_handles_data_t* data;
	int capacity;
	int count;
	int freelist_head;
	int freelist_tail;
} audiosys_internal_handles_t;


static void audiosys_internal_handles_init( audiosys_internal_handles_t* handles, int initial_capacity, void* memctx ) {
	handles->memctx = memctx;
	handles->count = 0;
	handles->capacity = initial_capacity <= 0 ? 1 : initial_capacity;
	handles->freelist_head = -1;
	handles->freelist_tail = -1;
	handles->data = (struct audiosys_internal_handles_data_t*) AUDIOSYS_MALLOC( memctx, handles->capacity * sizeof( *handles->data ) );
	AUDIOSYS_ASSERT( handles->data, "Allocation failed" );
}


static void audiosys_internal_handles_term( audiosys_internal_handles_t* handles ) {
	//AUDIOSYS_FREE( handles->memctx, handles->data );
}


static int audiosys_internal_handles_alloc( audiosys_internal_handles_t* handles, int index ) {
	int handle;

	if( handles->count < handles->capacity ) {
		handle = handles->count;
		handles->data[ handles->count ].counter = 0;
		++handles->count;           
	} else if( handles->freelist_head >= 0 ) {
		handle = handles->freelist_head;
		if( handles->freelist_tail == handles->freelist_head ) handles->freelist_tail = handles->data[ handles->freelist_head ].index;
		handles->freelist_head = handles->data[ handles->freelist_head ].index;                     
	} else {
		handles->capacity *= 2;
		struct audiosys_internal_handles_data_t* new_data = (struct audiosys_internal_handles_data_t*) AUDIOSYS_MALLOC( handles->memctx, handles->capacity * sizeof( *handles->data ) );
		AUDIOSYS_ASSERT( new_data, "Allocation failed" );
		AUDIOSYS_MEMCPY( new_data, handles->data, handles->count * sizeof( *handles->data ) );
		AUDIOSYS_FREE( handles->memctx, handles->data );
		handles->data = new_data;
		handle = handles->count;
		handles->data[ handles->count ].counter = 0;
		++handles->count;           
	}

	handles->data[ handle ].index = index;
	return handle;
}


static void audiosys_internal_handles_release( audiosys_internal_handles_t* handles, int handle ) {
	if( handles->freelist_tail < 0 ) {
		AUDIOSYS_ASSERT( handles->freelist_head < 0, "Freelist invalid" );
		handles->freelist_head = handle;
		handles->freelist_tail = handle;
	} else {
		handles->data[ handles->freelist_tail ].index = handle;
		handles->freelist_tail = handle;
	}
	++handles->data[ handle ].counter; // invalidate handle via counter
	handles->data[ handle ].index = -1;
}


static int audiosys_internal_handles_index( audiosys_internal_handles_t* handles, int handle ) {
	if( handle >= 0 && handle < handles->count ) {
		return handles->data[ handle ].index;
	} else {
		return -1;
	}
}


static void audiosys_internal_handles_update( audiosys_internal_handles_t* handles, int handle, int index ) {
	handles->data[ handle ].index = index;
}


static uint64_t audiosys_internal_to_u64( int handle, int counter ) {
	uint64_t i = (uint64_t) ( handle + 1 );
	uint64_t c = (uint64_t) counter;
	return ( c << 32ull ) | ( i );
}


static int audiosys_internal_u64_to_counter( uint64_t u ) {
	return (int) ( u >> 32ull ) ;
}
	

static int audiosys_internal_u64_to_handle( uint64_t u ) {
	return ( (int) ( u & 0xffffffffull ) ) - 1;
}


static uint64_t audiosys_internal_handles_to_u64( audiosys_internal_handles_t* handles, int handle ) {
	return audiosys_internal_to_u64( handle, handles->data[ handle ].counter );
}


static int audiosys_internal_handles_from_u64( audiosys_internal_handles_t* handles, uint64_t u ) {
	int handle = audiosys_internal_u64_to_handle( u );
	if( handle >= 0 && handle < handles->count && handles->data[ handle ].counter == audiosys_internal_u64_to_counter( u ) ) {
		return handle;
	} else {
		return -1;
	}
}


typedef enum audiosys_internal_voice_state_t {
	AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED,
	AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING,
	AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN,
	AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT,
	AUDIOSYS_INTERNAL_VOICE_STATE_CROSSFADING,
	AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED,
} audiosys_internal_voice_state_t;


typedef struct audiosys_internal_voice_t {
	AUDIOSYS_U64 handle;
	audiosys_internal_voice_state_t state;
	audiosys_audio_source_t source;
	int initialized;
	int paused;
	int loop;
	float priority;
	float volume;
	float pan;
	float fade_in_time;
	float fade_out_time;
	float fade_progress;

	float current_fade_volume;
	float current_fade_delta;
} audiosys_internal_voice_t;


struct audiosys_t {
	void* memctx;

	int use_soft_clip;
	int active_voice_count;
	float master_volume;
	float gain;
	int paused;
	
	audiosys_internal_voice_t music;
	audiosys_internal_voice_t music_crossfade;
	float music_crossfade_time;
	
	audiosys_internal_voice_t ambience;
	audiosys_internal_voice_t ambience_crossfade;
	float ambience_crossfade_time;
	
	int sounds_capacity;
	int sounds_count;
	AUDIOSYS_U64* sounds_by_priority;
	audiosys_internal_voice_t* sounds;
	audiosys_internal_handles_t sounds_handles;
	
	int mixing_buffer_size;
	float* mixing_buffer;
	float* sample_buffer;
};


static void audiosys_internal_release_source( audiosys_audio_source_t* source ) {
	if( source->release ) {
		source->release( source->instance );
	}
	source->instance = NULL;
	source->release = NULL;
	source->read_samples = NULL;
	source->restart = NULL;
	source->set_position = NULL;
	source->get_position = NULL;
}


audiosys_t* audiosys_create( int active_voice_count, void* memctx ) {
	audiosys_t* audiosys = (audiosys_t*) AUDIOSYS_MALLOC( memctx, sizeof( audiosys_t ) );
	AUDIOSYS_MEMSET( audiosys, 0, sizeof( audiosys_t ) );

	audiosys->memctx = memctx;

	audiosys->use_soft_clip = true;
	audiosys->active_voice_count= active_voice_count;
	audiosys->master_volume = 1.0f;
	audiosys->gain = 1.0f;
	audiosys->paused = 0;

	audiosys->music.volume = 1.0f;
	audiosys->music.loop = true;
	audiosys->music_crossfade.volume = 1.0f;
	audiosys->music_crossfade.loop = true;

	audiosys->ambience.volume = 1.0f;
	audiosys->ambience.loop = true;
	audiosys->ambience_crossfade.volume = 1.0f;
	audiosys->ambience_crossfade.loop = true;

	audiosys->sounds_capacity = 64;
	audiosys->sounds_count = 0;
	audiosys->sounds_by_priority = (AUDIOSYS_U64*) AUDIOSYS_MALLOC( memctx, sizeof( AUDIOSYS_U64 ) * audiosys->sounds_capacity );
	AUDIOSYS_MEMSET( audiosys->sounds_by_priority, 0, sizeof( AUDIOSYS_U64 ) * audiosys->sounds_capacity );
	audiosys->sounds = (audiosys_internal_voice_t*) AUDIOSYS_MALLOC( memctx, sizeof( audiosys_internal_voice_t ) * audiosys->sounds_capacity );
	audiosys_internal_handles_init( &audiosys->sounds_handles, audiosys->sounds_capacity, memctx );   

	return audiosys;
}


void audiosys_destroy( audiosys_t* audiosys ) {
	audiosys_internal_release_source( &audiosys->music.source );
	audiosys_internal_release_source( &audiosys->music_crossfade.source );
	audiosys_internal_release_source( &audiosys->ambience.source );
	audiosys_internal_release_source( &audiosys->ambience_crossfade.source );
	for( int i = 0; i < audiosys->sounds_count; ++i ) {
		audiosys_internal_release_source( &audiosys->sounds[ i ].source );
	}

	audiosys_internal_handles_term( &audiosys->sounds_handles );
	AUDIOSYS_FREE( audiosys->memctx, audiosys->sounds_by_priority );
	AUDIOSYS_FREE( audiosys->memctx, audiosys->sounds );

	if( audiosys->mixing_buffer ) {
		AUDIOSYS_FREE( audiosys->memctx, audiosys->mixing_buffer );
	}

	if( audiosys->sample_buffer ) {
		AUDIOSYS_FREE( audiosys->memctx, audiosys->sample_buffer );
	}

	AUDIOSYS_FREE( audiosys->memctx, audiosys );
}


static audiosys_internal_voice_t* audiosys_internal_get_sound( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	int index = audiosys_internal_handles_index( &audiosys->sounds_handles, audiosys_internal_handles_from_u64( &audiosys->sounds_handles, handle ) );
	if( index < 0 ) { 
		return 0;
	}
	return &audiosys->sounds[ index ];
}


static AUDIOSYS_U64 audiosys_internal_add_sound( audiosys_t* audiosys, audiosys_internal_voice_t** sound, float priority ) {
	if( audiosys->sounds_count >= audiosys->sounds_capacity ) {
		int new_capacity = audiosys->sounds_capacity * 2;
		AUDIOSYS_U64* new_sounds_by_priority = (AUDIOSYS_U64*) AUDIOSYS_MALLOC( memctx, sizeof( AUDIOSYS_U64 ) * new_capacity );
		audiosys_internal_voice_t* new_sounds = (audiosys_internal_voice_t*) AUDIOSYS_MALLOC( memctx, sizeof( audiosys_internal_voice_t ) * new_capacity );
		AUDIOSYS_MEMCPY( new_sounds_by_priority, audiosys->sounds_by_priority, sizeof( AUDIOSYS_U64 ) * audiosys->sounds_count );
		AUDIOSYS_MEMCPY( new_sounds, audiosys->sounds, sizeof( audiosys_internal_voice_t ) * audiosys->sounds_count );
		AUDIOSYS_FREE( audiosys->memctx, audiosys->sounds_by_priority );
		AUDIOSYS_FREE( audiosys->memctx, audiosys->sounds );
		audiosys->sounds_capacity = new_capacity;
		audiosys->sounds_by_priority = new_sounds_by_priority;
		audiosys->sounds = new_sounds;
	}

	int index = audiosys->sounds_count;
	if( audiosys->sounds_count > 0 ) {
		int min_index = 0; 
		int max_index = audiosys->sounds_count - 1;
		while( min_index <= max_index ) {
			int center = ( max_index - min_index ) / 2 + min_index;
			audiosys_internal_voice_t* snd = audiosys_internal_get_sound( audiosys, audiosys->sounds_by_priority[ center ] );
			float center_prio = snd->priority;
			if( center_prio <= priority ) {
				max_index = center - 1;
			} else {
				min_index = center + 1;
			}
		}
		index = min_index;
	}
	
	++audiosys->sounds_count;
	*sound = &audiosys->sounds[ audiosys->sounds_count - 1 ];
	AUDIOSYS_U64 handle = audiosys_internal_handles_to_u64( &audiosys->sounds_handles, audiosys_internal_handles_alloc( &audiosys->sounds_handles, audiosys->sounds_count - 1 ) );

	for( int i = audiosys->sounds_count - 1; i > index ; --i ) {
		audiosys->sounds_by_priority[ i ] = audiosys->sounds_by_priority[ i - 1 ];
	}

	audiosys->sounds_by_priority[ index ] = handle;
	return handle;
}

	
static void audiosys_internal_remove_sound( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	int index = audiosys_internal_handles_index( &audiosys->sounds_handles, audiosys_internal_handles_from_u64( &audiosys->sounds_handles, handle ) );
	if( index < 0 ) {
		return;
	}

	audiosys_internal_handles_release( &audiosys->sounds_handles, audiosys_internal_handles_from_u64( &audiosys->sounds_handles, handle ) );
	audiosys_internal_voice_t* sound_to_remove = &audiosys->sounds[ index ];
	audiosys_internal_release_source( &sound_to_remove->source );

	for( int j = 0; j < audiosys->sounds_count; ++j ) {
		if( audiosys->sounds_by_priority[ j ] == handle ) {
			for( int i = j; i < audiosys->sounds_count - 1; ++i ) {
				audiosys->sounds_by_priority[ i ] = audiosys->sounds_by_priority[ i + 1 ];
			}
			break;
		}
	}

	audiosys_internal_voice_t* last_sound = &audiosys->sounds[ audiosys->sounds_count - 1 ];
	audiosys_internal_handles_update( &audiosys->sounds_handles, audiosys_internal_handles_from_u64( &audiosys->sounds_handles, last_sound->handle ), index );
	--audiosys->sounds_count;

	*sound_to_remove = *last_sound;
}
	

void audiosys_internal_update_fading( audiosys_internal_voice_t* voice, float delta_time ) {
	if( !voice ) {
		return;
	}

	float fade_volume = 1.0f;
	float fade_delta = 0.0f;
	if( voice->state == AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT && voice->fade_out_time > 0.0f ) {
		voice->fade_progress -= delta_time / voice->fade_out_time;
		if( voice->fade_progress <= 0.0f ) {
			voice->fade_progress = 0.0f;
			voice->state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
			fade_volume = 0.0f;
			audiosys_internal_release_source( &voice->source );
		} else {
			float volume_dec_per_second = 1.0f / voice->fade_out_time;
			float volume_dec_per_sample = volume_dec_per_second / 44100.0f;
			fade_volume = voice->fade_progress;
			fade_volume = fade_volume < 0.0f ? 0.0f : fade_volume > 1.0f ? 1.0f : fade_volume;			
			fade_delta = -volume_dec_per_sample;
		}
	} else if( voice->state == AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN && voice->fade_in_time > 0.0f ) {
		voice->fade_progress += delta_time / voice->fade_in_time;
		if( voice->fade_progress >= 1.0f ) {				
			voice->fade_progress = 1.0f;
			voice->state = AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING;
			fade_volume = 1.0f;
			} else {
			float volume_inc_per_second = 1.0f / voice->fade_in_time;
			float volume_inc_per_sample = volume_inc_per_second / 44100.0f;
			fade_volume = voice->fade_progress;
			fade_volume = fade_volume < 0.0f ? 0.0f : fade_volume > 1.0f ? 1.0f : fade_volume;			
			fade_delta = volume_inc_per_sample;
		}
	}

	voice->current_fade_volume = fade_volume;
	voice->current_fade_delta = fade_delta;
}


void audiosys_internal_update_from_source( audiosys_t* audiosys, audiosys_internal_voice_t* voice, float* output_sample_pairs, int sample_pairs_count ) {
	int samples_to_write = sample_pairs_count;
	if( !voice->initialized ) {
		audiosys_internal_update_fading( voice, 0.0f );
		voice->initialized = 1;
	} else {        
		audiosys_internal_update_fading( voice, sample_pairs_count / 44100.0f );
	}

	if( !voice->source.read_samples || voice->state == AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED ) {
		AUDIOSYS_MEMSET( output_sample_pairs, 0, sample_pairs_count * sizeof( float ) * 2 );
		return;
	}

	float* out = output_sample_pairs;
	int count_written = voice->source.read_samples( voice->source.instance, out, samples_to_write );
	out += count_written * 2;
	while( count_written < samples_to_write ) {
		if( voice->loop && voice->source.restart ) {
			voice->source.restart( voice->source.instance );
			int written = voice->source.read_samples( voice->source.instance, out, samples_to_write - count_written );
			out += written * 2;
			count_written += written;
		} else {
			audiosys_internal_release_source( &voice->source );
			for( int i = 0; i < ( samples_to_write - count_written ) * 2; ++i ) {
				out[ i ] = 0.0f;	
			}
			return;
		}
	}
}

   
void audiosys_internal_mix_voice( audiosys_t* audiosys, audiosys_internal_voice_t* voice, float* sample_pairs, int sample_pairs_count ) {
	if( !voice ) {
		return;
	}

	audiosys_internal_update_from_source( audiosys, voice, sample_pairs, sample_pairs_count );

	float fade_volume = voice->current_fade_volume;
	float fade_delta = voice->current_fade_delta;

	if( voice->pan < 0.0f && fade_delta != 0.0f ) {
		float a = -voice->pan; 
		float ia = 1.0f - a;
		for( int i = 0; i < sample_pairs_count; ++i ) {
		    float l = sample_pairs[ i * 2 + 0 ];
		    float r = sample_pairs[ i * 2 + 1 ];
			float left = l + a * r;
			float right = r * ia;
		    left *= voice->volume * fade_volume * audiosys->master_volume;
		    right *= voice->volume * fade_volume * audiosys->master_volume;
		    audiosys->mixing_buffer[ i * 2 + 0 ] += left;
		    audiosys->mixing_buffer[ i * 2 + 1 ] += right;
			fade_volume += fade_delta;
			fade_volume = fade_volume < 0.0f ? 0.0f : fade_volume > 1.0f ? 1.0f : fade_volume;			
		}
	} else if( voice->pan > 0.0f && fade_delta != 0.0f ) {
		float a = voice->pan;
		float ia = 1.0f - a;
		for( int i = 0; i < sample_pairs_count; ++i ) {
		    float l = sample_pairs[ i * 2 + 0 ];
		    float r = sample_pairs[ i * 2 + 1 ];
			float right = l * a + r;
			float left = l * ia;
		    left *= voice->volume * fade_volume * audiosys->master_volume;
		    right *= voice->volume * fade_volume * audiosys->master_volume;
		    audiosys->mixing_buffer[ i * 2 + 0 ] += left;
		    audiosys->mixing_buffer[ i * 2 + 1 ] += right;
			fade_volume += fade_delta;
			fade_volume = fade_volume < 0.0f ? 0.0f : fade_volume > 1.0f ? 1.0f : fade_volume;			
		}
	} else if( voice->pan < 0.0f && fade_delta == 0.0f ) {
		float a = -voice->pan; 
		float ia = 1.0f - a;
		for( int i = 0; i < sample_pairs_count; ++i ) {
		    float l = sample_pairs[ i * 2 + 0 ];
		    float r = sample_pairs[ i * 2 + 1 ];
			float left = l + a * r;
			float right = r * ia;
		    left *= voice->volume * fade_volume * audiosys->master_volume;
		    right *= voice->volume * fade_volume * audiosys->master_volume;
		    audiosys->mixing_buffer[ i * 2 + 0 ] += left;
		    audiosys->mixing_buffer[ i * 2 + 1 ] += right;
		}
	} else if( voice->pan > 0.0f && fade_delta == 0.0f ) {
		float a = voice->pan;
		float ia = 1.0f - a;
		for( int i = 0; i < sample_pairs_count; ++i ) {
		    float l = sample_pairs[ i * 2 + 0 ];
		    float r = sample_pairs[ i * 2 + 1 ];
			float right = l * a + r;
			float left = l * ia;
		    left *= voice->volume * fade_volume * audiosys->master_volume;
		    right *= voice->volume * fade_volume * audiosys->master_volume;
		    audiosys->mixing_buffer[ i * 2 + 0 ] += left;
		    audiosys->mixing_buffer[ i * 2 + 1 ] += right;
		}
	} else if( voice->pan == 0.0f && fade_delta != 0.0f ) {
		for( int i = 0; i < sample_pairs_count; ++i ) {
		    float left = sample_pairs[ i * 2 + 0 ];
		    float right = sample_pairs[ i * 2 + 1 ];
		    left *= voice->volume * fade_volume * audiosys->master_volume;
		    right *= voice->volume * fade_volume * audiosys->master_volume;
		    audiosys->mixing_buffer[ i * 2 + 0 ] += left;
		    audiosys->mixing_buffer[ i * 2 + 1 ] += right;
			fade_volume += fade_delta;
			fade_volume = fade_volume < 0.0f ? 0.0f : fade_volume > 1.0f ? 1.0f : fade_volume;			
		}
	} else if( voice->pan == 0.0f && fade_delta == 0.0f ) {
		for( int i = 0; i < sample_pairs_count; ++i ) {
		    float left = sample_pairs[ i * 2 + 0 ];
		    float right = sample_pairs[ i * 2 + 1 ];
		    left *= voice->volume * fade_volume * audiosys->master_volume;
		    right *= voice->volume * fade_volume * audiosys->master_volume;
		    audiosys->mixing_buffer[ i * 2 + 0 ] += left;
		    audiosys->mixing_buffer[ i * 2 + 1 ] += right;
		}
	} else {
		AUDIOSYS_ASSERT( false, "Invalid mix parameters" );
	}
}


void audiosys_render( audiosys_t* audiosys, AUDIOSYS_S16* output_sample_pairs, int sample_pairs_count ) {
	if( audiosys->paused ) {
		AUDIOSYS_MEMSET( output_sample_pairs, 0, sample_pairs_count * sizeof( AUDIOSYS_S16 ) * 2 );
		return;
	}

	if( audiosys->mixing_buffer_size < sample_pairs_count ) {
		if( audiosys->mixing_buffer ) {
			AUDIOSYS_FREE( audiosys->memctx, audiosys->mixing_buffer );
		}
		if( audiosys->sample_buffer ) {
			AUDIOSYS_FREE( audiosys->memctx, audiosys->sample_buffer );
		}
		audiosys->mixing_buffer_size = sample_pairs_count;
		audiosys->mixing_buffer = (float*) AUDIOSYS_MALLOC( audiosys->memctx, sample_pairs_count * sizeof( float ) * 2 );
		audiosys->sample_buffer = (float*) AUDIOSYS_MALLOC( audiosys->memctx, sample_pairs_count * sizeof( float ) * 2 );
	}

	// remove sounds which have finished playing
	for( int i = audiosys->sounds_count - 1; i >= 0; --i ) {
		audiosys_internal_voice_t* voice = &audiosys->sounds[ i ];
		if( voice->source.read_samples == NULL ) audiosys_internal_remove_sound( audiosys, voice->handle );
	}

	// clear buffer for mixing
	for( int i = 0; i < sample_pairs_count * 2; ++i ) {
		audiosys->mixing_buffer[ i ] = 0.0f;
	}
	
	// mix all active voices
	audiosys_internal_mix_voice( audiosys, &audiosys->music, audiosys->sample_buffer, sample_pairs_count );
	audiosys_internal_mix_voice( audiosys, &audiosys->music_crossfade, audiosys->sample_buffer, sample_pairs_count );
	audiosys_internal_mix_voice( audiosys, &audiosys->ambience, audiosys->sample_buffer, sample_pairs_count );
	audiosys_internal_mix_voice( audiosys, &audiosys->ambience_crossfade, audiosys->sample_buffer, sample_pairs_count );

	int sounds_count = audiosys->active_voice_count;
	for( int i = 0; i < sounds_count; ++i ) {
	   audiosys_internal_mix_voice( audiosys, audiosys_internal_get_sound( audiosys, audiosys->sounds_by_priority[ i ] ), audiosys->sample_buffer, sample_pairs_count );
	}

	if( audiosys->use_soft_clip ) {
		for( int i = 0; i < sample_pairs_count * 2; ++i ) {
			float s = audiosys->mixing_buffer[ i ];
			s *= audiosys->gain * 0.5f;
			s = s < -1.0f ? -2.0f / 3.0f : s > 1.0f ? 2.0f / 3.0f : s - ( s * s * s ) / 3; // soft clip 
			s *= 32000.0f;
			output_sample_pairs[ i ] = (AUDIOSYS_S16)( s );
		}
	} else {
		for( int i = 0; i < sample_pairs_count * 2; ++i ) {
			float s = audiosys->mixing_buffer[ i ];
			s *= audiosys->gain * 0.5f;
			s = s < -1.0f ? -1.0f : s > 1.0f ? 1.0f : s;
			s *= 32000.0f;
			output_sample_pairs[ i ] = (AUDIOSYS_S16)( s );
		}
	}
}


void audiosys_master_volume_set( audiosys_t* audiosys, float volume ) {
	audiosys->master_volume = volume < 0.0f ? 0.0f : volume > 1.0f ? 1.0f : volume;
}


float audiosys_master_volume( audiosys_t* audiosys ) {
	return audiosys->master_volume;
}


void audiosys_gain_set( audiosys_t* audiosys, float gain ) {
	audiosys->gain = gain;
}


float audiosys_gain( audiosys_t* audiosys ) {
	return audiosys->gain;
}


void audiosys_pause( audiosys_t* audiosys ) {
	audiosys->paused = 1;
}


void audiosys_resume( audiosys_t* audiosys ) {
	audiosys->paused = 0;
}


void audiosys_stop_all( audiosys_t* audiosys ) {
	audiosys_music_stop( audiosys, 0.0f );
	audiosys_ambience_stop( audiosys, 0.0f );

	for( int i = 0; i < audiosys->sounds_count; ++i ) {
		audiosys_internal_voice_t* sound = &audiosys->sounds[ i ];
		sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
		audiosys_internal_remove_sound( audiosys, sound->handle );
		audiosys_internal_release_source( &sound->source );
	}
}


audiosys_paused_t audiosys_paused( audiosys_t* audiosys ) {
	return audiosys->paused ? AUDIOSYS_PAUSED : AUDIOSYS_NOT_PAUSED;
}


static void audiosys_internal_init_voice( audiosys_internal_voice_t* voice, audiosys_audio_source_t source, int is_sound ) {
	voice->handle = 0;
	voice->initialized = 0;
	voice->paused = 0;
	voice->state = AUDIOSYS_INTERNAL_VOICE_STATE_PLAYING;
	voice->source = source;
	if( is_sound ) {
	    voice->loop = 0;
	    voice->volume = 1.0f;
	    voice->pan = 0.0f;
	}
	voice->fade_in_time = 0.0f;
	voice->fade_out_time = 0.0f;
	voice->priority = 0.0f;
	voice->fade_progress = 1.0f;
	voice->current_fade_volume = 1.0f;
	voice->current_fade_delta = 0.0f;
}


void audiosys_music_play( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_in_time ) {
	audiosys_internal_release_source( &audiosys->music.source );
	audiosys_internal_init_voice( &audiosys->music, source, 0 );
	audiosys->music.fade_in_time = fade_in_time;
	if( fade_in_time > 0.0f ) {
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
		audiosys->music.fade_progress = 0.0f;
	}
}


void audiosys_music_stop( audiosys_t* audiosys, float fade_out_time ) {
	if( fade_out_time > 0.0f ) {
		audiosys->music.fade_out_time = fade_out_time;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		audiosys->music.fade_progress = 1.0f;
	} else {
		audiosys_internal_release_source( &audiosys->music.source );
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
	}
}


void audiosys_music_pause( audiosys_t* audiosys ) {
	if( audiosys->music.paused == 1 ) {
		return;
	}
	audiosys->music.paused = 1;
}


void audiosys_music_resume( audiosys_t* audiosys ) {
	if( audiosys->music.paused == 0 ) { 
		return;
	}
	audiosys->music.paused = 0;
}


void audiosys_music_switch( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_out_time, float fade_in_time ) {
	if( fade_out_time > 0.0f ) {
		audiosys->music.fade_out_time = fade_out_time;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		audiosys->music.fade_progress = 1.0f;
	} else {
		audiosys_internal_release_source( &audiosys->music.source );
		audiosys_music_play( audiosys, source, fade_in_time );
		return;
	}
	audiosys_internal_voice_t temp = audiosys->music;
	audiosys->music = audiosys->music_crossfade;
	audiosys->music_crossfade = temp;

	audiosys_internal_init_voice( &audiosys->music, source, 0 );
	audiosys->music.fade_in_time = fade_in_time;
	if( fade_in_time > 0.0f ) {
		audiosys->music.fade_progress = 0.0f;
	}
	audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED;
}


void audiosys_music_cross_fade( audiosys_t* audiosys, audiosys_audio_source_t source, float cross_fade_time ) {
	if( cross_fade_time > 0.0f ) {
		audiosys->music.fade_out_time = cross_fade_time;
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		audiosys->music.fade_progress = 1.0f;
	} else {
		audiosys_internal_release_source( &audiosys->music.source );
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
	}
	audiosys_internal_voice_t temp = audiosys->music;
	audiosys->music = audiosys->music_crossfade;
	audiosys->music_crossfade = temp;

	audiosys_internal_init_voice( &audiosys->music, source, 0 );
	audiosys->music.fade_in_time = cross_fade_time;
	if( cross_fade_time > 0.0f ) {
		audiosys->music.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
		audiosys->music.fade_progress = 0.0f;
	}
}


void audiosys_music_position_set( audiosys_t* audiosys, float position ) {
	if( !audiosys->music.source.set_position ) {
		return;
	}

	audiosys->music.source.set_position( audiosys->music.source.instance, (int)( position * 44100.0f ) );
}


float audiosys_music_position( audiosys_t* audiosys ) {
	if( !audiosys->music.source.get_position ) {
		return 0.0f;
	}

	return audiosys->music.source.get_position( audiosys->music.source.instance ) / 44100.0f;
}


audiosys_audio_source_t audiosys_music_source( audiosys_t* audiosys ) {
	return audiosys->music.source;
}


void audiosys_music_loop_set( audiosys_t* audiosys, audiosys_loop_t loop ) {
	audiosys->music.loop = loop == AUDIOSYS_LOOP_ON ? 1 : 0;	
}


audiosys_loop_t audiosys_music_loop( audiosys_t* audiosys ) {
	return audiosys->music.loop ? AUDIOSYS_LOOP_ON : AUDIOSYS_LOOP_OFF;
}


void audiosys_music_volume_set( audiosys_t* audiosys, float volume ) {
	audiosys->music.volume = volume;
}


float audiosys_music_volume( audiosys_t* audiosys ) {
	return audiosys->music.volume;
}


void audiosys_music_pan_set( audiosys_t* audiosys, float pan ) {
	audiosys->music.pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
}


float audiosys_music_pan( audiosys_t* audiosys ) {
	return audiosys->music.pan;
}


void audiosys_ambience_play( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_in_time ) {
	audiosys_internal_release_source( &audiosys->ambience.source );
	audiosys_internal_init_voice( &audiosys->ambience, source, 0 );
	audiosys->ambience.fade_in_time = fade_in_time;
	if( fade_in_time > 0.0f ) {
		audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
		audiosys->ambience.fade_progress = 0.0f;
	}
}


void audiosys_ambience_stop( audiosys_t* audiosys, float fade_out_time ) {
	if( fade_out_time > 0.0f ) {
		audiosys->ambience.fade_out_time = fade_out_time;
		audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		audiosys->ambience.fade_progress = 1.0f;
	} else {
		audiosys_internal_release_source( &audiosys->ambience.source );
		audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
	}
}


void audiosys_ambience_pause( audiosys_t* audiosys ) {
	if( audiosys->ambience.paused == 1 ) {
		return;
	}
	audiosys->ambience.paused = 1;
}


void audiosys_ambience_resume( audiosys_t* audiosys ) {
	if( audiosys->ambience.paused == 0 ) {
		return;
	}
	audiosys->ambience.paused = 0;
}


void audiosys_ambience_switch( audiosys_t* audiosys, audiosys_audio_source_t source, float fade_out_time, float fade_in_time ) {
	if( fade_out_time > 0.0f ) {
		audiosys->ambience.fade_out_time = fade_out_time;
		audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
	} else {
		audiosys_internal_release_source( &audiosys->ambience.source );
		audiosys_ambience_play( audiosys, source, fade_in_time );
		return;
	}
	audiosys_internal_voice_t temp = audiosys->ambience;
	audiosys->ambience = audiosys->ambience_crossfade;
	audiosys->ambience_crossfade = temp;

	audiosys_internal_init_voice( &audiosys->ambience, source, 0 );
	audiosys->ambience.fade_in_time = fade_in_time;
	if( fade_in_time > 0.0f ) {
		audiosys->ambience.fade_progress = 0.0f;
	}
	audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_QUEUED;
}


void audiosys_ambience_cross_fade( audiosys_t* audiosys, audiosys_audio_source_t source, float cross_fade_time ) {
	if( cross_fade_time > 0.0f ) {
		audiosys->ambience.fade_out_time = cross_fade_time;
		audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		audiosys->ambience.fade_progress = 1.0f;
	} else {
		audiosys_internal_release_source( &audiosys->ambience.source );
		audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
	}
	audiosys_internal_voice_t temp = audiosys->ambience;
	audiosys->ambience = audiosys->ambience_crossfade;
	audiosys->ambience_crossfade = temp;

	audiosys_internal_init_voice( &audiosys->ambience, source, 0 );
	audiosys->ambience.fade_in_time = cross_fade_time;
	if( cross_fade_time > 0.0f ) {
		audiosys->ambience.state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
		audiosys->ambience.fade_progress = 0.0f;
	}
}


void audiosys_ambience_position_set( audiosys_t* audiosys, float position ) {
	if( !audiosys->ambience.source.set_position ) {
		return;
	}

	audiosys->ambience.source.set_position( audiosys->ambience.source.instance, (int)( position * 44100.0f ) );
}


float audiosys_ambience_position( audiosys_t* audiosys ) {
	if( !audiosys->ambience.source.get_position ) {
		return 0.0f;
	}

	return audiosys->ambience.source.get_position( audiosys->ambience.source.instance ) / 44100.0f;
}

audiosys_audio_source_t audiosys_ambience_source( audiosys_t* audiosys ) {
	return audiosys->ambience.source;
}


void audiosys_ambience_loop_set( audiosys_t* audiosys, audiosys_loop_t loop ) {
	audiosys->ambience.loop = loop == AUDIOSYS_LOOP_ON ? 1 : 0;	
}


audiosys_loop_t audiosys_ambience_loop( audiosys_t* audiosys ) {
	return audiosys->ambience.loop ? AUDIOSYS_LOOP_ON : AUDIOSYS_LOOP_OFF;
}


void audiosys_ambience_volume_set( audiosys_t* audiosys, float volume ) {
	audiosys->ambience.volume = volume;
}


float audiosys_ambience_volume( audiosys_t* audiosys ) {
	return audiosys->ambience.volume;
}


void audiosys_ambience_pan_set( audiosys_t* audiosys, float pan ) {
	audiosys->ambience.pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
}


float audiosys_ambience_pan( audiosys_t* audiosys )
	{
	return audiosys->ambience.pan;
	}


AUDIOSYS_U64 audiosys_sound_play( audiosys_t* audiosys, audiosys_audio_source_t source, float priority, float fade_in_time ) {
	audiosys_internal_voice_t* sound;
	AUDIOSYS_U64 handle = audiosys_internal_add_sound( audiosys, &sound, priority );
	audiosys_internal_init_voice( sound, source, 1 );
	sound->handle = handle;
	sound->priority = priority;
	sound->fade_in_time = fade_in_time;
	if( fade_in_time > 0.0f )  {
		sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_IN;
		sound->fade_progress = 0.0f;
	}
	return handle;
}


void audiosys_sound_stop( audiosys_t* audiosys, AUDIOSYS_U64 handle, float fade_out_time ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return;
	}

	if( fade_out_time > 0.0f ) {
		sound->fade_out_time = fade_out_time;
		sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_FADING_OUT;
		sound->fade_progress = 1.0f;
	} else {
		sound->state = AUDIOSYS_INTERNAL_VOICE_STATE_STOPPED;
		audiosys_internal_remove_sound( audiosys, handle );
		audiosys_internal_release_source( &sound->source );
	}
}


void audiosys_sound_pause( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return;
	}
	if( sound->paused == 1 ) {
		return;
	}
	sound->paused = 1;
}


void audiosys_sound_resume( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return;
	}
	if( sound->paused == 0 ) {
		return;
	}
	sound->paused = 0;
}


void audiosys_sound_position_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float position ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return;
	}
	if( !sound->source.set_position ) {
		return;
	}

	sound->source.set_position( sound->source.instance, (int)( position * 44100.0f ) );
}


float audiosys_sound_position( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return 0.0f;
	}
	if( !sound->source.get_position ) {
		return 0.0f;
	}

	return sound->source.get_position( sound->source.instance ) / 44100.0f;
}


audiosys_audio_source_t audiosys_sound_source( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		audiosys_audio_source_t null_source;
		AUDIOSYS_MEMSET( &null_source, 0, sizeof( null_source ) );
		return null_source;
	}
	return sound->source;
}


void audiosys_sound_loop_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, audiosys_loop_t loop ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return;
	}
	sound->loop = loop == AUDIOSYS_LOOP_ON ? 1 : 0;	
}


audiosys_loop_t audiosys_sound_loop( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return AUDIOSYS_LOOP_OFF;
	}
	return sound->loop ? AUDIOSYS_LOOP_ON : AUDIOSYS_LOOP_OFF;
}


void audiosys_sound_volume_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float volume ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return;
	}
	sound->volume = volume;
}


float audiosys_sound_volume( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return 0.0f;
	}
	return sound->volume;
}


void audiosys_sound_pan_set( audiosys_t* audiosys, AUDIOSYS_U64 handle, float pan ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return;
	}
	sound->pan = pan < -1.0f ? -1.0f : pan > 1.0f ? 1.0f : pan;
}


float audiosys_sound_pan( audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return 0.0f;
	}
	return sound->pan;
}


audiosys_sound_valid_t audiosys_sound_valid(audiosys_t* audiosys, AUDIOSYS_U64 handle ) {
	audiosys_internal_voice_t* sound = audiosys_internal_get_sound( audiosys, handle );
	if( !sound ) {
		return AUDIOSYS_SOUND_INVALID;
	}
	return AUDIOSYS_SOUND_VALID;
}

#endif /* AUDIOSYS_IMPLEMENTATION */

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2017 Mattias Gustavsson

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
