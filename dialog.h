/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

dialog.h - v0.1 - Loading and management of dialog definitions.

This library provides functionality to load and manage dialogs written in a 
custom markup language designed for simplicity and readability.

The dialog definition files contain any number of "conversations". Each
conversation is made up of any number of entries. An entry can be any of the
following: a line of speech, an option, a redirect command, or an event 
command.

A conversation is defined by giving it an ID within square brackets ( [, ] ).
Two conversation IDs should be considered reserved: EXIT and RETURN. The 
first of these means that the dialog is over for now and should be stopped,
and the second means to return to the conversation from which we came (if 
there is no conversation to return to, the dialog should be stopped).

A line of speech consists of an actor name within angled brackets ( <, > ) 
followed by the actual ine.

An option (as may be selected by a player) starts with the character ^. If it
starts with two of that character, ^^, it means that the option is persistent
and thus shall not be removed (the normal behaviour is that options are 
removed from the list after being chosen once). After the ^ or ^^ is the line
which will be spoken if the option is chosen. Last, the line has the 
character @ followed by the ID of the conversation to jump to should that 
option be chosen.

A redirect command is simply the @ character followed by the ID of the 
conversation to jump to when execution reaches this command.

Finally, an event command is any string surrounded by # characters. This can
be used to send a custom message to the game engine, to signal that something
should happen.

Here follows an example dialog file from "The Strangler":

	[TalkToBambi]
	^ Sorry to *bother* you, ma'am. I need to ask you some *questions*. @YouAcop
	^ Well well well... ain't you a *classy* one. @Classy
	^ Excuse me, do you know a woman by the name of Amber? @Amber
	^^ I don't have any more questions right now. @EXIT

	[YouAcop]
	<Bambi> You a *cop* or something? Fuck off.
	<O'Reilly> *Believe* me, I hate those *bastards* just as much as you do.
	@RETURN

	[Classy]
	<Bambi> Errr.. No *offense*, pal, but you're not exactly one to *talk*.
	@RETURN

	[Amber]
	<Bambi> I knew Amber. But Amber is *dead*. 
	@RETURN

	[ShowPicture]
	<O'Reilly> Have you seen this man around?
	<Bambi> Yeah, I've seen him with some of the *girls*.
	<O'Reilly> Did you see him with Amber last night?
	<Bambi> I *did*, now that you *mention* it...
	<O'Reilly> Any idea where he might have taken her?
	<Bambi> Probably to this *motel*... that's where he'd usually go....	
	#UnlockMotel#

*/

#ifndef dialog_h
#define dialog_h

typedef struct dialog_t dialog_t;

dialog_t* dialog_create( char const* input, int input_size );
void dialog_destroy( dialog_t* dialog );

int dialog_save( dialog_t const* dialog, void* data, int capacity );
dialog_t* dialog_load( void const* data, int size );
void dialog_unload( dialog_t* dialog );

typedef struct dialog_conversation_t { int handle; } dialog_conversation_t;
typedef struct dialog_entry_t { int handle; } dialog_entry_t;
typedef struct dialog_line_t { int handle; } dialog_line_t;
typedef struct dialog_option_t { int handle; } dialog_option_t;
typedef struct dialog_redirect_t { int handle; } dialog_redirect_t;
typedef struct dialog_event_t { int handle; } dialog_event_t;

typedef enum dialog_entry_type_t { 
    DIALOG_ENTRY_TYPE_INVALID = -1,
    DIALOG_ENTRY_TYPE_LINE, 
    DIALOG_ENTRY_TYPE_OPTION, 
    DIALOG_ENTRY_TYPE_REDIRECT, 
    DIALOG_ENTRY_TYPE_EVENT,
} dialog_entry_type_t;

int dialog_find_conversation( dialog_t const* dialog, char const* id );

int dialog_conversation_count( dialog_t const* dialog );
dialog_conversation_t dialog_conversation( dialog_t const* dialog, int index );

char const* dialog_conversation_id( dialog_t const* dialog, dialog_conversation_t conversation );
int dialog_conversation_entry_count( dialog_t const* dialog, dialog_conversation_t conversation );
dialog_entry_t dialog_conversation_entry( dialog_t const* dialog, dialog_conversation_t conversation, int index );

char const* dialog_entry_id( dialog_t const* dialog, dialog_entry_t entry );
dialog_entry_type_t dialog_entry_type( dialog_t const* dialog, dialog_entry_t entry );
dialog_line_t dialog_entry_line( dialog_t const* dialog, dialog_entry_t entry );
dialog_option_t dialog_entry_option( dialog_t const* dialog, dialog_entry_t entry );
dialog_redirect_t dialog_entry_redirect( dialog_t const* dialog, dialog_entry_t entry );
dialog_event_t dialog_entry_event( dialog_t const* dialog, dialog_entry_t entry );

char const* dialog_line_actor( dialog_t const* dialog, dialog_line_t line );
char const* dialog_line_text( dialog_t const* dialog, dialog_line_t line );

char const* dialog_option_text( dialog_t const* dialog, dialog_option_t option );
char const* dialog_option_target( dialog_t const* dialog, dialog_option_t option );
int dialog_option_is_persistent( dialog_t const* dialog, dialog_option_t option );

char const* dialog_redirect_target( dialog_t const* dialog, dialog_redirect_t redirect );

char const* dialog_event_event( dialog_t const* dialog, dialog_event_t event );


#endif /* dialog_h */


/*
void test_dialog( dialog_t* dialog ) {
    for( int i = 0; i < dialog_conversation_count( dialog ); ++i ) {
        dialog_conversation_t conversation = dialog_conversation( dialog, i );
        printf( "\n[%s]\n", dialog_conversation_id( dialog, conversation ) );      
        for( int j = 0; j < dialog_conversation_entry_count( dialog, conversation ); ++j ) {
            dialog_entry_t entry = dialog_conversation_entry( dialog, conversation, j );
            dialog_entry_type_t type = dialog_entry_type( dialog, entry );
            char const* id = dialog_entry_id( dialog, entry );
            if( id ) {
                printf( "{%s}", id );
            }

            if( type == DIALOG_ENTRY_TYPE_LINE ) {
                dialog_line_t line = dialog_entry_line( dialog, entry );
                printf("<%s>%s", dialog_line_actor( dialog, line ), dialog_line_text( dialog, line ) );
            } else if( type == DIALOG_ENTRY_TYPE_OPTION ) {
                dialog_option_t option = dialog_entry_option( dialog, entry );
                if( dialog_option_is_persistent( dialog, option ) ) {
                    printf( "^" );
                }
                printf("^%s@%s", dialog_option_text( dialog, option ), dialog_option_target( dialog, option ) );
            } else if( type == DIALOG_ENTRY_TYPE_REDIRECT ) {
                dialog_redirect_t redirect = dialog_entry_redirect( dialog, entry );
                printf("@%s", dialog_redirect_target( dialog, redirect ) );
            } else if( type == DIALOG_ENTRY_TYPE_EVENT ) {
                dialog_event_t event = dialog_entry_event( dialog, entry );
                printf("#%s#", dialog_event_event( dialog, event ) );
            }
            printf( "|\n" );
        }
    }
}
*/
/*
----------------------
    IMPLEMENTATION
----------------------
*/


#ifdef DIALOG_IMPLEMENTATION

#define _CRT_NONSTDC_NO_DEPRECATE 
#define _CRT_SECURE_NO_WARNINGS
#define wait PIXIE_STD_WAIT /* osx defines a conflicting "wait" */
#include <stdlib.h>
#undef wait
#include <string.h>


typedef struct dialog_parse_line_t {
	char* actor;
    char* text;
} dialog_parse_line_t;


typedef struct dialog_parse_option_t {
	char* text;
    char* target;
    int is_persistant;
} dialog_parse_option_t;


typedef struct dialog_parse_redirect_t {
	char* target;
} dialog_parse_redirect_t;


typedef struct dialog_parse_event_t {
	char* event;
} dialog_parse_event_t;


typedef struct dialog_parse_entry_t {
	char* id; // TODO: change this to "condition",  which should work similar to events - the player will be in a "waiting for condition" state, and once it gets the result, either do the line or not
    dialog_entry_type_t type;
    union {
	    struct dialog_parse_line_t line; 
	    struct dialog_parse_option_t option;
	    struct dialog_parse_redirect_t redirect;
	    struct dialog_parse_event_t event;
    } data;
} dialog_parse_entry_t;


typedef struct dialog_parse_convo_t {
    char* id;
    int entries_count;
    int entries_capacity;
    dialog_parse_entry_t* entries;
} dialog_parse_convo_t;


typedef struct dialog_parse_t {
    int conversations_count;
    int conversations_capacity;
    dialog_parse_convo_t* conversations;
} dialog_parse_t;


int dialog_parse_line( dialog_parse_line_t* parse, char const* start, char const* end ) {
	// A Line is defined like this:
	//     <ActorName>LineText

	// If the row doesn't start with '<', it's not a valid line
	if( *start != '<' ) return 0;
	++start; // Skip leading bracket
		    
	// Find end bracket
	char const* actor_end = start;
	while( *actor_end != '>' && actor_end < end ) ++actor_end;

	// If there's no ending '>', it's not a valid line
	if( (*actor_end) != '>' ) return 0;

    // Store actor name
	parse->actor = (char*) malloc( (size_t)( actor_end - start + 1 ) );
    strncpy( parse->actor, start, (size_t)( actor_end - start ) );
    parse->actor[ actor_end - start ] = '\0';
		    
	// Parse line text
	start = actor_end + 1; // Skip past the ending bracket of the actor name    
	while( start < end && *start <= ' ' ) ++start; // Skip leading whitespace

    // Store line text
	parse->text = (char*) malloc( (size_t)( end - start + 1 ) );
    strncpy( parse->text, start, (size_t)( end - start ) );
    parse->text[ end - start ] = '\0';

    // Respect escape codes for line breaks
	char* str = parse->text;
    char* out = parse->text;
	while( *str ) {
		if( *str == '\\' && *( str + 1 ) == 'n' ) {
			*out++ = '\n';
			str += 2;
		} else {
            *out++ = *str++;
        }
	}
		    
	// If we don't have both a valid actor and a valid line, this row is invalid
	if( !parse->actor || *parse->actor == '\0' || !parse->text || *parse->text == '\0' ) {
        if( parse->actor ) free( parse->actor );
        if( parse->text ) free( parse->text );
        parse->actor = NULL;
        parse->text = NULL;
        return 0;
    }

	// This is a valid row
	return 1;
}


int dialog_parse_option( dialog_parse_option_t* parse, char const* start, char const* end ) {
	// An Option is defined like this:
	//     ^OptionText@TargetId
	//
	// Or, for a persistent option:
	//     ^^OptionText@TargetId

	// If the row doesn't start with '^', it's not a valid option
	if( *start != '^' ) return 0;
	++start; // Skip leading '^'

	// If the second character is also '^', this is a persistent option
	if( *start == '^' ) { 
        parse->is_persistant = 1; 
        ++start; // Skip second '^'
    } 
	else {
        parse->is_persistant = 0; // This is not a persistent option
    }

	// Skip leading whitespace
	while( start < end && *start <= ' ' ) ++start;
			
	// If the rest of the row is empty, it is not a valid option
	if( start == end ) return 0;

	// Find the '@' character
	char const* target = start;
	while( *target != '@' && target < end ) ++target;

	// If there isn't an '@' character, this is not a valid option
	if( *target != '@' ) return 0;
		    
	// Trim trailing whitespace
	char const* trim = target - 1;
	while( trim > start && *trim <= ' ' ) --trim;

    // Store option text
	parse->text = (char*) malloc( (size_t)( trim - start + 1 ) );
    strncpy( parse->text, start, (size_t)( trim - start ) );
    parse->text[ trim - start ] = '\0';
		    
	// Parse target id
	++target; // Skip the '@' character
			
	// Skip leading whitespace
	while( target < end && *target <= ' ' ) ++target;
		    
	// Store target id
	parse->target = (char*) malloc( (size_t)( end - target + 1 ) );
    strncpy( parse->target, target, (size_t)( end - target ) );
    parse->target[ end - target ] = '\0';
		    
	// If option text or target id are empty, this is not a valid option
	// If we don't have both a valid actor and a valid line, this row is invalid
	if( !parse->text || *parse->text == '\0' || !parse->target || *parse->target == '\0' ) {
        if( parse->text ) free( parse->text );
        if( parse->target ) free( parse->target );
        parse->text = NULL;
        parse->target = NULL;
        return 0;
    }

	return 1;
}


int dialog_parse_redirect( dialog_parse_redirect_t* parse, char const* start, char const* end ) {
	// A Redirect is defined like this:
	//     @TargetId

	// If row doesn't start with '@', it is not a valid redirect
	if( *start != '@' ) return 0;
	++start; // Skip the '@' character

	// Skip leading whitespace
	while( start < end && *start <= ' ' ) ++start;

	// Store target id
	parse->target = (char*) malloc( (size_t)( end - start + 1 ) );
    strncpy( parse->target, start, (size_t)( end - start ) );
    parse->target[ end - start ] = '\0';

	// If we don't have a valid target id, this is not a valid redirect
	if( !parse->target || *parse->target == '\0' ) {
        if( parse->target ) free( parse->target );
        parse->target = NULL;
        return 0;
    }

	return 1;
}


int dialog_parse_event( dialog_parse_event_t* parse, char const* start, char const* end ) {
	// An Event is defined like this:
	//     #eventName#

	// If the row doesn't start with '#', it's not a valid event
	if( *start != '#' ) return 0;
	++start; // Skip leading '#'

	// If there's no trailing '#', this is not a valid event
	if( *--end != '#' ) return 0;

    // Store event string
	parse->event = (char*) malloc( (size_t)( end - start + 1 ) );
    strncpy( parse->event, start, (size_t)( end - start ) );
    parse->event[ end - start ] = '\0';

	// If we don't have a valid event name, it's not a valid event
	if( !parse->event || *parse->event == '\0' ) {
        if( parse->event ) free( parse->event );
        parse->event = NULL;
        return 0;
    }

	// Indicate that this is a valid event
	return 1;			
}


int dialog_parse_entry( dialog_parse_entry_t* parse, char const* start, char const* end ) {
    char const* row = start;

	// Strip id (text defined {like_this} is an id) from the start of the row
	if( *row == '{' && strchr( row, '}' ) != 0 )
		{
		const char* id = ++row;
		while( row < end && *row != '}' ) ++row;
		parse->id = (char*) malloc( (size_t)( row - id + 1 ) );
        strncpy( parse->id, id, (size_t)( row - id ) );
        parse->id[ row - id ] = '\0';
		++row;
		while( row < end && *row <= ' ' ) ++row;
		}
 	
 	if( *row == '<' ) { // Check if this is a line
 		parse->type = DIALOG_ENTRY_TYPE_LINE;
 		return dialog_parse_line( &parse->data.line, row, end );
    } else if( *row == '^' ) { // Check if this is an option
 		parse->type = DIALOG_ENTRY_TYPE_OPTION;
 		return dialog_parse_option( &parse->data.option, row, end );
 	} else if( *row == '@' ) { // Check if this is a redirect
 		parse->type = DIALOG_ENTRY_TYPE_REDIRECT;
 		return dialog_parse_redirect( &parse->data.redirect, row, end );
 	} else if( *row == '#' && *( end - 1 ) == '#' && ( end - row ) > 2 ) { // Check if this is an event
 		parse->type = DIALOG_ENTRY_TYPE_EVENT;
 		return dialog_parse_event( &parse->data.event, row, end );
 	} else { // Not a valid row
 		return 0;
 	}
}


static int dialog_parse_row( dialog_parse_convo_t* parse, char const* start, char const* end ) {
    // Block definition?
	size_t length = (size_t)( end - start );
	if( *start == '[' && start[ length - 1 ] == ']' )
		{        
		if( !parse->id ) // We only deal with the id if this is a new block
			{
            parse->id = (char*) malloc( (size_t)( length - 1 ) );
            strncpy( parse->id, start + 1, length - 2 );
            parse->id[ length - 2 ] = '\0';
			return 1; // Indicate that row was parsed ok
			}

		// We have reached the end of the conversation 
		return 0; // Indicate that row has not been parsed (new conversation)
		}

	// Entry
	dialog_parse_entry_t entry;
    memset( &entry, 0, sizeof( entry ) );
	if( dialog_parse_entry( &entry, start, end ) ) { // Parse the row as an entry
        if( parse->entries_count >= parse->entries_capacity ) {
            parse->entries_capacity *= 2;
            parse->entries = (dialog_parse_entry_t*) realloc( parse->entries, 
                sizeof( dialog_parse_entry_t ) * parse->entries_capacity );
        }
        parse->entries[ parse->entries_count++ ] = entry;
    } else {
        // TODO: Warn about failed parse
        if( entry.id ) free( entry.id );
    }

	return 1; // Indicate that row was parsed ok
}


static void dialog_parse( dialog_parse_t* parse, char const* buffer, int size ) {
    dialog_parse_convo_t current = { 0 };
    current.entries_count = 0;
    current.entries_capacity = 256;
    current.entries = (dialog_parse_entry_t*) malloc( sizeof( dialog_parse_entry_t ) * current.entries_capacity );

	// extract rows
	for( char const* ptr = buffer; ptr - buffer < size; ++ptr ) {
		char const* row_start = ptr;
				
		// skip forward until next line break
		while( *ptr != '\n' && ptr - buffer < size ) ++ptr;

		// only bother with non-zero length rows
		if( ptr - row_start > 1 ) {
			char const* row = row_start;
			char const* row_end = ptr; 

			// skip leading whitespace
			char const* start = row;
			while ( start < row_end && *start <= ' ' )  ++start;					
			row = start;

			// skip trailing whitespace
			start = row;
			char const* end = row_end;
			while( *end <= ' ' && end > start ) --end; 
			++end;
					
			// only bother with non-zero length rows
			if( end - start > 0 ) {
				// parse row
				int parse_result = dialog_parse_row( &current, start, end );
				if( !parse_result ) { // if we're on a new conversation
					// new conversation, so insert the current one into the list if it is valid
	                if( current.id ) {
                        if( parse->conversations_count >= parse->conversations_capacity ) {
                            parse->conversations_capacity *= 2;
                            parse->conversations = (dialog_parse_convo_t*) realloc( parse->conversations, 
                                sizeof( dialog_parse_convo_t ) * parse->conversations_capacity );
                        }
                        parse->conversations[ parse->conversations_count++ ] = current;
                    } else {
                        free( current.entries );
                    }

					// and then start a new conversation
					current.id = 0;
                    current.entries_count = 0;
                    current.entries_capacity = 256;
                    current.entries = (dialog_parse_entry_t*) malloc( sizeof( dialog_parse_entry_t ) * current.entries_capacity );
					dialog_parse_row( &current, start, end );
				}
			}
		}
	}
			
	// if the current conversation is valid, add it to the list
	if( current.id ) {
        if( parse->conversations_count >= parse->conversations_capacity ) {
            parse->conversations_capacity *= 2;
            parse->conversations = (dialog_parse_convo_t*) realloc( parse->conversations, 
                sizeof( dialog_parse_convo_t ) * parse->conversations_capacity );
        }
        parse->conversations[ parse->conversations_count++ ] = current;
    } else {
        free( current.entries );
    }
}


typedef struct dialog_data_t {
    int type;
    union {
        struct {
	        int actor_offset;
            int text_offset;
        } line;

        struct {
	        int text_offset;
            int target_offset;
            int is_persistant;
        } option;

        struct {
	        int target_offset;
        } redirect;

        struct {
	        int event_offset;
        } event;

        struct {
            int id_offset;
            int entries_count;
            int entry_list_start_index;
        } conversation;
        
        struct {
            int id_offset;
            int entry_index;
            int entry_list_next_index;
        } entry_list;

    } data;
} dialog_data_t;


struct dialog_t {
    int size_in_bytes;
    int conversation_count;
    int data_count;
    dialog_data_t data[ 1 ];
};


#define DIALOG_ENTRY_TYPE_CONVERSATION 4 
#define DIALOG_ENTRY_TYPE_ENTRY_LIST 5


dialog_t* dialog_create( char const* input, int input_size ) {
    dialog_parse_t parse;
    parse.conversations_count = 0;
    parse.conversations_capacity = 256;
    parse.conversations = (dialog_parse_convo_t*) malloc( sizeof( dialog_parse_convo_t ) * parse.conversations_capacity );
    dialog_parse( &parse, input, input_size );
    int total_size = sizeof( dialog_t ) - sizeof( dialog_data_t );
    total_size += parse.conversations_count * sizeof( dialog_data_t );
    int entry_list_count = 0;
    int entries_count = 0;
    for( int i = 0; i < parse.conversations_count; ++i ) {
        total_size += (int)strlen( parse.conversations[ i ].id ) + 1;
        total_size += parse.conversations[ i ].entries_count * sizeof( dialog_data_t );
        entry_list_count += parse.conversations[ i ].entries_count;
        for( int j = 0; j < parse.conversations[ i ].entries_count; ++j ) {
            ++entries_count;
            total_size += sizeof( dialog_data_t );
            if( parse.conversations[ i ].entries[ j ].id ) {
                total_size += (int)strlen( parse.conversations[ i ].entries[ j ].id ) + 1;
            }
            if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_LINE ) {
                total_size += (int)strlen( parse.conversations[ i ].entries[ j ].data.line.actor ) + 1;
                total_size += (int)strlen( parse.conversations[ i ].entries[ j ].data.line.text ) + 1;
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_OPTION ) {
                total_size += (int)strlen( parse.conversations[ i ].entries[ j ].data.option.text ) + 1;
                total_size += (int)strlen( parse.conversations[ i ].entries[ j ].data.option.target ) + 1;
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_REDIRECT ) {
                total_size += (int)strlen( parse.conversations[ i ].entries[ j ].data.redirect.target ) + 1;
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_EVENT ) {
                total_size += (int)strlen( parse.conversations[ i ].entries[ j ].data.event.event ) + 1;
            }
        }
    }

    dialog_t* dialog = (dialog_t*) malloc( (size_t) total_size );
    dialog->size_in_bytes = total_size;
    dialog->conversation_count = parse.conversations_count;
    int next_index = parse.conversations_count;
    char* text_data = (char*)( dialog->data + parse.conversations_count + entry_list_count + entries_count );
    int text_offset = (int)( text_data - (char*)dialog );
    for( int i = 0; i < parse.conversations_count; ++i ) {
        dialog_data_t* conversation = &dialog->data[ i ];
        conversation->type = DIALOG_ENTRY_TYPE_CONVERSATION;
        conversation->data.conversation.id_offset = text_offset;
        conversation->data.conversation.entries_count = parse.conversations[ i ].entries_count;
        conversation->data.conversation.entry_list_start_index = next_index++;
        
        strcpy( text_data, parse.conversations[ i ].id );
        text_data += strlen( parse.conversations[ i ].id ) + 1;
        text_offset += (int)strlen( parse.conversations[ i ].id ) + 1;

        int index = conversation->data.conversation.entry_list_start_index;
        for( int j = 0; j < parse.conversations[ i ].entries_count; ++j ) {
            dialog_data_t* entry_list = &dialog->data[ index ];
            entry_list->type = DIALOG_ENTRY_TYPE_ENTRY_LIST;
            entry_list->data.entry_list.entry_index = next_index++;
            index = j >= parse.conversations[ i ].entries_count - 1 ? 0 : next_index++;
            entry_list->data.entry_list.entry_list_next_index = index;
            if( parse.conversations[ i ].entries[ j ].id ) {
                entry_list->data.entry_list.id_offset = text_offset;
                strcpy( text_data, parse.conversations[ i ].entries[ j ].id );
                text_data += strlen( parse.conversations[ i ].entries[ j ].id ) + 1;
                text_offset += (int)strlen( parse.conversations[ i ].entries[ j ].id ) + 1;
            } else {
                entry_list->data.entry_list.id_offset = 0;
            }

            dialog_data_t* entry = &dialog->data[ entry_list->data.entry_list.entry_index ];
            if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_LINE ) {
                entry->type = DIALOG_ENTRY_TYPE_LINE;

                entry->data.line.actor_offset = text_offset;
                strcpy( text_data, parse.conversations[ i ].entries[ j ].data.line.actor );
                text_data += strlen( parse.conversations[ i ].entries[ j ].data.line.actor ) + 1;
                text_offset += (int)strlen( parse.conversations[ i ].entries[ j ].data.line.actor ) + 1;

                entry->data.line.text_offset = text_offset;
                strcpy( text_data, parse.conversations[ i ].entries[ j ].data.line.text );
                text_data += strlen( parse.conversations[ i ].entries[ j ].data.line.text ) + 1;
                text_offset += (int)strlen( parse.conversations[ i ].entries[ j ].data.line.text ) + 1;
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_OPTION ) {
                entry->type = DIALOG_ENTRY_TYPE_OPTION;
                entry->data.option.text_offset = text_offset;
                strcpy( text_data, parse.conversations[ i ].entries[ j ].data.option.text );
                text_data += strlen( parse.conversations[ i ].entries[ j ].data.option.text ) + 1;
                text_offset += (int)strlen( parse.conversations[ i ].entries[ j ].data.option.text ) + 1;

                entry->data.option.target_offset = text_offset;
                strcpy( text_data, parse.conversations[ i ].entries[ j ].data.option.target );
                text_data += strlen( parse.conversations[ i ].entries[ j ].data.option.target ) + 1;
                text_offset += (int)strlen( parse.conversations[ i ].entries[ j ].data.option.target ) + 1;

                entry->data.option.is_persistant = parse.conversations[ i ].entries[ j ].data.option.is_persistant;
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_REDIRECT ) {
                entry->type = DIALOG_ENTRY_TYPE_REDIRECT;
                entry->data.redirect.target_offset = text_offset;
                strcpy( text_data, parse.conversations[ i ].entries[ j ].data.redirect.target );
                text_data += strlen( parse.conversations[ i ].entries[ j ].data.redirect.target ) + 1;
                text_offset += (int)strlen( parse.conversations[ i ].entries[ j ].data.redirect.target ) + 1;
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_EVENT ) {
                entry->type = DIALOG_ENTRY_TYPE_EVENT;
                entry->data.event.event_offset = text_offset;
                strcpy( text_data, parse.conversations[ i ].entries[ j ].data.event.event );
                text_data += strlen( parse.conversations[ i ].entries[ j ].data.event.event ) + 1;
                text_offset += (int)strlen( parse.conversations[ i ].entries[ j ].data.event.event ) + 1;
            }
        }
    }
    dialog->data_count = next_index;

    for( int i = 0; i < parse.conversations_count; ++i ) {
        free( parse.conversations[ i ].id );
        for( int j = 0; j < parse.conversations[ i ].entries_count; ++j ) {
            if( parse.conversations[ i ].entries[ j ].id ) {
                free( parse.conversations[ i ].entries[ j ].id );
            }
            if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_LINE ) {
                free( parse.conversations[ i ].entries[ j ].data.line.actor );
                free( parse.conversations[ i ].entries[ j ].data.line.text );
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_OPTION ) {
                free( parse.conversations[ i ].entries[ j ].data.option.text );
                free( parse.conversations[ i ].entries[ j ].data.option.target );
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_REDIRECT ) {
                free( parse.conversations[ i ].entries[ j ].data.redirect.target );
            } else if( parse.conversations[ i ].entries[ j ].type == DIALOG_ENTRY_TYPE_EVENT ) {
                free( parse.conversations[ i ].entries[ j ].data.event.event );
            }
        }
        free( parse.conversations[ i ].entries );
    }
    free( parse.conversations );

    return dialog;
}


void dialog_destroy( dialog_t* dialog ) {
    free( dialog );
}


int dialog_save( dialog_t const* dialog, void* data, int capacity ) {
    if( data && capacity >= dialog->size_in_bytes ) {
        memcpy( data, dialog, (size_t)( dialog->size_in_bytes ) );
    }
    return dialog->size_in_bytes;
}


dialog_t* dialog_load( void const* data, int size ) {
    dialog_t* dialog = (dialog_t*) malloc( (size_t) size );
    memcpy( dialog, data, (size_t) size );
    return dialog;
}


void dialog_unload( dialog_t* dialog ) {
    free( dialog );
}


int dialog_find_conversation( dialog_t const* dialog, char const* id ) {
    for( int i = 0; i < dialog->conversation_count; ++i ) {
        char const* conv_id = ( (char const*) dialog ) + dialog->data[ i ].data.conversation.id_offset;
		#ifdef _WIN32
		if( stricmp( id, conv_id ) == 0 ) {
		#else
		if( strcasecmp( id, conv_id ) == 0 ) {
		#endif
            return i;
        }
    }

    return -1;
}


int dialog_conversation_count( dialog_t const* dialog ) {
    return dialog->conversation_count;
}


dialog_conversation_t dialog_conversation( dialog_t const* dialog, int index ) {
    dialog_conversation_t conversation;
    conversation.handle = index >= 0 && index < dialog->conversation_count ? index : -1;
    return conversation;
}


char const* dialog_conversation_id( dialog_t const* dialog, dialog_conversation_t conversation ) {
    if( conversation.handle >= 0 && conversation.handle < dialog->conversation_count ) {
        return ( (char*) dialog ) + dialog->data[ conversation.handle ].data.conversation.id_offset;
    } else {
        return NULL;
    }
}


int dialog_conversation_entry_count( dialog_t const* dialog, dialog_conversation_t conversation ) {
    if( conversation.handle >= 0 && conversation.handle < dialog->conversation_count ) {
        return dialog->data[ conversation.handle ].data.conversation.entries_count;
    } else {
        return 0;
    }
}


dialog_entry_t dialog_conversation_entry( dialog_t const* dialog, dialog_conversation_t conversation, int index ) {
    dialog_entry_t entry;
    entry.handle = -1;
    if( conversation.handle >= 0 && conversation.handle < dialog->conversation_count ) {
        if( index < 0 || index >= dialog->data[ conversation.handle ].data.conversation.entries_count ) {
            return entry;
        }
        int entry_index = dialog->data[ conversation.handle ].data.conversation.entry_list_start_index;
        for( int i = 0; i < index; ++i ) {
            entry_index = dialog->data[ entry_index ].data.entry_list.entry_list_next_index;
        }
        if( !entry_index ) {
            return entry;
        }
        entry.handle = entry_index;
        return entry;
    } else {
        return entry;
    }
}


char const* dialog_entry_id( dialog_t const* dialog, dialog_entry_t entry ) {
    if( entry.handle < 0 || entry.handle >= dialog->data_count ) {
        return NULL;
    }
    if( dialog->data[ entry.handle ].type != DIALOG_ENTRY_TYPE_ENTRY_LIST ) {
        return NULL;
    }
    if( dialog->data[ entry.handle ].data.entry_list.id_offset ) {
        return ( (char*) dialog ) + dialog->data[ entry.handle ].data.entry_list.id_offset;        
    } else {
        return NULL;
    }    
}


dialog_entry_type_t dialog_entry_type( dialog_t const* dialog, dialog_entry_t entry ) {
    if( entry.handle < 0 || entry.handle >= dialog->data_count ) {
        return DIALOG_ENTRY_TYPE_INVALID;
    }
    if( dialog->data[ entry.handle ].type != DIALOG_ENTRY_TYPE_ENTRY_LIST ) {
        return DIALOG_ENTRY_TYPE_INVALID;
    }
    int entry_index = dialog->data[ entry.handle ].data.entry_list.entry_index;
    if( entry_index < 0 || entry_index >= dialog->data_count ) {
        return DIALOG_ENTRY_TYPE_INVALID;
    }
    dialog_entry_type_t type = (dialog_entry_type_t) dialog->data[ entry_index ].type;
    if( type == DIALOG_ENTRY_TYPE_LINE || type == DIALOG_ENTRY_TYPE_OPTION || type == DIALOG_ENTRY_TYPE_REDIRECT || 
        type == DIALOG_ENTRY_TYPE_EVENT ) {
            return type;
    }
    return DIALOG_ENTRY_TYPE_INVALID;
}


dialog_line_t dialog_entry_line( dialog_t const* dialog, dialog_entry_t entry ) {
    dialog_line_t line;
    line.handle = -1;
    if( entry.handle < 0 || entry.handle >= dialog->data_count ) {
        return line;
    }
    if( dialog->data[ entry.handle ].type != DIALOG_ENTRY_TYPE_ENTRY_LIST ) {
        return line;
    }
    int entry_index = dialog->data[ entry.handle ].data.entry_list.entry_index;
    if( entry_index < 0 || entry_index >= dialog->data_count ) {
        return line;
    }
    if( dialog->data[ entry_index ].type != DIALOG_ENTRY_TYPE_LINE ) {
        return line;
    }
    line.handle = entry_index;
    return line;
}


dialog_option_t dialog_entry_option( dialog_t const* dialog, dialog_entry_t entry ) {
    dialog_option_t option;
    option.handle = -1;
    if( entry.handle < 0 || entry.handle >= dialog->data_count ) {
        return option;
    }
    if( dialog->data[ entry.handle ].type != DIALOG_ENTRY_TYPE_ENTRY_LIST ) {
        return option;
    }
    int entry_index = dialog->data[ entry.handle ].data.entry_list.entry_index;
    if( entry_index < 0 || entry_index >= dialog->data_count ) {
        return option;
    }
    if( dialog->data[ entry_index ].type != DIALOG_ENTRY_TYPE_OPTION ) {
        return option;
    }
    option.handle = entry_index;
    return option;
}


dialog_redirect_t dialog_entry_redirect( dialog_t const* dialog, dialog_entry_t entry ) {
    dialog_redirect_t redirect;
    redirect.handle = -1;
    if( entry.handle < 0 || entry.handle >= dialog->data_count ) {
        return redirect;
    }
    if( dialog->data[ entry.handle ].type != DIALOG_ENTRY_TYPE_ENTRY_LIST ) {
        return redirect;
    }
    int entry_index = dialog->data[ entry.handle ].data.entry_list.entry_index;
    if( entry_index < 0 || entry_index >= dialog->data_count ) {
        return redirect;
    }
    if( dialog->data[ entry_index ].type != DIALOG_ENTRY_TYPE_REDIRECT ) {
        return redirect;
    }
    redirect.handle = entry_index;
    return redirect;
}


dialog_event_t dialog_entry_event( dialog_t const* dialog, dialog_entry_t entry ) {
    dialog_event_t event;
    event.handle = -1;
    if( entry.handle < 0 || entry.handle >= dialog->data_count ) {
        return event;
    }
    if( dialog->data[ entry.handle ].type != DIALOG_ENTRY_TYPE_ENTRY_LIST ) {
        return event;
    }
    int entry_index = dialog->data[ entry.handle ].data.entry_list.entry_index;
    if( entry_index < 0 || entry_index >= dialog->data_count ) {
        return event;
    }
    if( dialog->data[ entry_index ].type != DIALOG_ENTRY_TYPE_EVENT ) {
        return event;
    }
    event.handle = entry_index;
    return event;
}


char const* dialog_line_actor( dialog_t const* dialog, dialog_line_t line ) {
    if( line.handle < 0 || line.handle >= dialog->data_count ) {
        return NULL;
    }
    if( dialog->data[ line.handle ].type != DIALOG_ENTRY_TYPE_LINE ) {
        return NULL;
    }
    
    return ( (char*) dialog ) + dialog->data[ line.handle ].data.line.actor_offset;        
}


char const* dialog_line_text( dialog_t const* dialog, dialog_line_t line ) {
    if( line.handle < 0 || line.handle >= dialog->data_count ) {
        return NULL;
    }
    if( dialog->data[ line.handle ].type != DIALOG_ENTRY_TYPE_LINE ) {
        return NULL;
    }
    
    return ( (char*) dialog ) + dialog->data[ line.handle ].data.line.text_offset;        
}


char const* dialog_option_text( dialog_t const* dialog, dialog_option_t option ) {
    if( option.handle < 0 || option.handle >= dialog->data_count ) {
        return NULL;
    }
    if( dialog->data[ option.handle ].type != DIALOG_ENTRY_TYPE_OPTION ) {
        return NULL;
    }
    
    return ( (char*) dialog ) + dialog->data[ option.handle ].data.option.text_offset;        
}


char const* dialog_option_target( dialog_t const* dialog, dialog_option_t option ) {
    if( option.handle < 0 || option.handle >= dialog->data_count ) {
        return NULL;
    }
    if( dialog->data[ option.handle ].type != DIALOG_ENTRY_TYPE_OPTION ) {
        return NULL;
    }
    
    return ( (char*) dialog ) + dialog->data[ option.handle ].data.option.target_offset;        
}


int dialog_option_is_persistent( dialog_t const* dialog, dialog_option_t option ) {
    if( option.handle < 0 || option.handle >= dialog->data_count ) {
        return 0;
    }
    if( dialog->data[ option.handle ].type != DIALOG_ENTRY_TYPE_OPTION ) {
        return 0;
    }
    
    return dialog->data[ option.handle ].data.option.is_persistant;        
}


char const* dialog_redirect_target( dialog_t const* dialog, dialog_redirect_t redirect ) {
    if( redirect.handle < 0 || redirect.handle >= dialog->data_count ) {
        return NULL;
    }
    if( dialog->data[ redirect.handle ].type != DIALOG_ENTRY_TYPE_REDIRECT ) {
        return NULL;
    }
    
    return ( (char*) dialog ) + dialog->data[ redirect.handle ].data.redirect.target_offset;        
}


char const* dialog_event_event( dialog_t const* dialog, dialog_event_t event ) {
    if( event.handle < 0 || event.handle >= dialog->data_count ) {
        return NULL;
    }
    if( dialog->data[ event.handle ].type != DIALOG_ENTRY_TYPE_EVENT ) {
        return NULL;
    }
    
    return ( (char*) dialog ) + dialog->data[ event.handle ].data.event.event_offset;        
}


#endif /* DIALOG_IMPLEMENTATION */

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2019 Mattias Gustavsson

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
