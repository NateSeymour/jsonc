#include "include/jsonc.h"

int _json_seek_to(json_cursor_t* json_cursor, char seek, int step)
{
	int steps_taken = 0;
	do {
		_cursor_push(json_cursor, step);
		steps_taken++;

		if(json_cursor->character == seek 
			&& !json_cursor->escaped 
			&& !json_cursor->in_string) 
		{
			return steps_taken;
		}
	} while(!json_cursor->end);

	return 0;
}

int _json_seek_to_obj(json_cursor_t* json_cursor)
{
	int steps_taken = 0;
	do {
		_cursor_push(json_cursor, 1);
		steps_taken++;

		if((json_cursor->character == '{' || json_cursor->character == '[')
			&& !json_cursor->escaped 
			&& !json_cursor->in_string) 
		{
			return steps_taken;
		}
	} while(!json_cursor->end);

	return 0;
}

int _json_seek_to_inclstr(json_cursor_t* json_cursor, char seek, int step)
{
	int steps_taken = 0;
	do {
		_cursor_push(json_cursor, step);
		steps_taken++;

		if(json_cursor->character == seek 
			&& !json_cursor->escaped) 
		{
			return steps_taken;
		}
	} while(!json_cursor->end);

	return 0;
}
