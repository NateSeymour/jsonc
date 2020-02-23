#include <string.h>

#include "include/jsonc.h"

int _json_preparse_str_count(json_cursor_t* json_cursor)
{
	int str_count = 0;

	_cursor_move(json_cursor, 0);

	do {
		_json_seek_to(json_cursor, '"', 1);

		// The string count includes the closing quote,
		// But that is fine because it will be used to
		// store the null terminator of the strings ;P
		str_count += _json_seek_to(json_cursor, '"', 1);
	} while(!json_cursor->end);

	return str_count;
}

int _json_preparse_obj_count(json_cursor_t* json_cursor)
{
	int obj_count = 1;

	_cursor_move(json_cursor, 0);

	do {
		int obj_start_pos = _json_seek_to(json_cursor, '{', 1);

		if(obj_start_pos)
		{
			obj_count++;
		}
	} while(!json_cursor->end);

	return obj_count;
}

int _json_preparse_def_count(json_cursor_t* json_cursor)
{
	int def_count = 0;

	_cursor_move(json_cursor, 0);

	do {
		int obj_start_pos = _json_seek_to(json_cursor, ':', 1);

		if(obj_start_pos)
		{
			def_count++;
		}
	} while(!json_cursor->end);

	return def_count;
}

preparse_data_t _json_preparse(const char* json_string)
{
	// Create preparsing cursor
	json_cursor_t json_cursor = { 
		.json_string = json_string, 
		.length = strlen(json_string),
		.position = 0 
	};

	// Create preparsing data structure
	preparse_data_t ppd = { 0, 0, 0 };

	// Head to the first object
	_json_seek_to(&json_cursor, '{', 1);

	// Preprocess this stuff
	ppd.string_length = _json_preparse_str_count(&json_cursor);
	ppd.obj_count = _json_preparse_obj_count(&json_cursor);
	ppd.def_count = _json_preparse_def_count(&json_cursor);

	return ppd;
}
