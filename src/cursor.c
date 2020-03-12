#include <stdlib.h>
#include <string.h>

#include "include/jsonc.h"

void _cursor_update(json_cursor_t* json_cursor)
{
	if(json_cursor->position == 0 || json_cursor->position == json_cursor->length) 
	{
		json_cursor->end = 1;
	} 
	else {
		json_cursor->end = 0;
	}

	json_cursor->character = json_cursor->json_string[json_cursor->position];

	if(!json_cursor->end)
	{
		json_cursor->precursor = json_cursor->json_string[json_cursor->position - 1];
		json_cursor->postcursor = json_cursor->json_string[json_cursor->position + 1];

		if(json_cursor->json_string[json_cursor->position - 2] == '\\')
		{
			json_cursor->precursor_escaped = 1;
		}
		else {
			json_cursor->precursor_escaped =  0;
		}
	}
	else {
		json_cursor->precursor = 0;
		json_cursor->postcursor = 0;
		json_cursor->precursor_escaped = 0;
	}

	if(json_cursor->character == '\\')
	{
		json_cursor->postcursor_escaped = 1;
	}
	else {
		json_cursor->postcursor_escaped = 0;
	}

	if(json_cursor->precursor != '\\')
	{
		json_cursor->escaped = 0;

		if(json_cursor->character == '"')
		{
			if(json_cursor->in_string)
			{
				json_cursor->in_string = 0;
			}
			else {
				json_cursor->in_string = 1;
			}
		}
	}
	else {
		json_cursor->escaped = 1;
	}
}

void _cursor_init(json_cursor_t* json_cursor, const char* json_string)
{
	json_cursor->position = 0;
	json_cursor->json_string = json_string;
	json_cursor->length = strlen(json_string);
	json_cursor->push_direction = 1;
	json_cursor->in_string = 0;

	_cursor_update(json_cursor);
}

// Cursor functions
void _cursor_push(json_cursor_t* json_cursor, int step)
{
	json_cursor->position = clamp(json_cursor->position + step, 0, json_cursor->length);
	json_cursor->push_direction = step/abs(step);

	_cursor_update(json_cursor);
}

void _cursor_move(json_cursor_t* json_cursor, int position)
{
	int movement = position - json_cursor->position;
	int steps = abs(movement);

	for(int i = 0; i < steps; i++)
	{
		_cursor_push(json_cursor, movement/steps);
	}
}
