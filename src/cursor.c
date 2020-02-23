#include <stdlib.h>

#include "include/jsonc.h"

// Cursor functions
void _cursor_push(json_cursor_t* json_cursor, int step)
{
	json_cursor->position = clamp(json_cursor->position + step, 0, json_cursor->length);

	if(json_cursor->position == 0 || json_cursor->position == json_cursor->length) 
	{
		json_cursor->end = 1;
	} 
	else {
		json_cursor->end = 0;
	}

	json_cursor->character = json_cursor->json_string[json_cursor->position];
	json_cursor->precursor = json_cursor->json_string[json_cursor->position - 1];

	if(json_cursor->json_string[json_cursor->position - 2] == '\\')
	{
		json_cursor->precursor_escaped = 1;
	}
	else {
		json_cursor->precursor_escaped =  0;
	}

	if(json_cursor->precursor != '\\' && !json_cursor->precursor_escaped)
	{
		json_cursor->escaped = 0;

		if(json_cursor->precursor == '"')
		{
			json_cursor->in_string = 1;
		}

		if(json_cursor->character == '"')
		{
			json_cursor->in_string = 0;
		}
	}
	else {
		json_cursor->escaped = 1;
	}
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
