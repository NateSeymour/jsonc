#include <string.h>
#include <stdio.h>

#include "include/jsonc.h"

preparse_data_t _json_preparse(const char* json_string)
{
	// Create preparsing cursor
	json_cursor_t json_cursor;
	_cursor_init(&json_cursor, json_string);

	// Create preparsing data structure
	preparse_data_t ppd = { 
		.obj_count = 0, 
		.def_count = 0, 
		.string_length = 0,
		.array_value_count = 0
	};

	do
	{
		if(!json_cursor.escaped)
		{
			if(!json_cursor.in_string)
			{
				if(json_cursor.character == '{') 
				{
					ppd.obj_count++;
				}
				else if(json_cursor.character == ':') 
				{
					ppd.def_count++;
				} 
				else if(json_cursor.character == '[')
				{
					ppd.obj_count++;
					if(json_cursor.postcursor != ']') 
					{
						ppd.def_count++;
						while(json_cursor.character != ']')
						{
							if(json_cursor.character == ',') ppd.def_count++;
							if(json_cursor.character == '"') ppd.string_length += _json_seek_to_inclstr(&json_cursor, '"', 1);
							_cursor_push(&json_cursor, 1);
						}
					}
				}
			} else if(json_cursor.character == '"')
			{
				ppd.string_length += _json_seek_to_inclstr(&json_cursor, '"', 1);
			}
		}

		_cursor_push(&json_cursor, 1);
	} while(!json_cursor.end);

	return ppd;
}
