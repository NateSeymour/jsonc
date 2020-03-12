#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/jsonc.h"

typedef struct {
	int obj_count;
	int current_obj_index;
	int strings;
} parse_data_t;

typedef struct {
	json_value_t value;
	json_value_types type;
} parse_value_data_t;

char* _parse_getstring(json_document_t* document, json_cursor_t* json_cursor)
{
	_json_seek_to_inclstr(json_cursor, '"', 1);
	int start_pos = json_cursor->position + 1;

	int length = _json_seek_to_inclstr(json_cursor, '"', 1) - 1;

	if(length != -1)
	{
		char* dest = strncpy((char*)document->string_ptr, json_cursor->json_string + start_pos, length);

		document->string_ptr = (char*)document->string_ptr + length + 1;

		return dest;
	} 
	else {
		return 0;
	}
}

parse_value_data_t _parse_getnumber(json_cursor_t* json_cursor)
{
	char buffer[50];
	int buffer_pos = 0;
	json_value_t value;
	json_value_types type = int_t;

	parse_value_data_t ret;

	_cursor_push(json_cursor, 1);
	while((json_cursor->character >= 48 && json_cursor->character <= 57) || json_cursor->character == '.')
	{
		if(json_cursor->character == '.')
		{
			type = _float_t;
		}

		buffer[buffer_pos] = json_cursor->character;
		buffer_pos++;
		_cursor_push(json_cursor, 1);
	}

	if(type == _float_t)
	{
		value.float_val = strtof(buffer, 0);
	} 
	else {
		value.int_val = atoi(buffer);
	}

	ret.value = value;
	ret.type = type;
	return ret;
}

void _skip_object(json_cursor_t* json_cursor)
{
	int obj_count = 1;
	int closing_count = 0;

	if(json_cursor->character != '{')
	{
		_json_seek_to(json_cursor, '{', 1);
	}

	while(obj_count != closing_count)
	{
		_cursor_push(json_cursor, 1);

		if(json_cursor->character == '{') obj_count++;
		if(json_cursor->character == '}') closing_count++;
	}
}

parse_value_data_t _parse_getvalue(json_document_t* document, json_cursor_t* json_cursor, parse_data_t* pd)
{
	parse_value_data_t value;

	while(1)
	{
		if(json_cursor->postcursor == '"')
		{ 
			value.value.str_val = _parse_getstring(document, json_cursor);
			value.type = string_t;
			break;
		}
		else if(json_cursor->postcursor >= 48 && json_cursor->postcursor <= 57) 
		{
			value = _parse_getnumber(json_cursor);
			break;
		}
		else if(json_cursor->postcursor == '{')
		{
			pd->obj_count++;
			value.value.obj_val = &document->obj_pool[pd->obj_count - 1];
			value.type = obj_t;
			_skip_object(json_cursor);
			break;
		}

		_cursor_push(json_cursor, 1);
	}

	return value;
}

void _json_do_parse(json_document_t* document, preparse_data_t ppd, const char* json_string)
{
	// Create parsing cursor
	json_cursor_t json_cursor;
	_cursor_init(&json_cursor, json_string);

	// Create parse data obj
	parse_data_t pd = {
		.obj_count = 1,
		.current_obj_index = 0, 
		.strings = 0
	};

	// Get first character
	if(json_cursor.character != '{')
	{
		_json_seek_to(&json_cursor, '{', 1);
	}

	// Create first object
	json_obj_t root_obj = {
		.children_count = 0,
		.children = document->def_pool
	};

	printf("\n\nParsing results: \n");

	// Parse Loop
	do
	{
		// Get named definiton
		char* definition_key = _parse_getstring(document, &json_cursor);

		if(definition_key)
		{
			_json_seek_to(&json_cursor, ':', 1);

			parse_value_data_t parse_value = _parse_getvalue(document, &json_cursor, &pd);

			json_def_t test = {
				.key = definition_key,
				.value = parse_value.value,
				.type = parse_value.type
			};

			*(json_def_t*)((char*)root_obj.children + (root_obj.children_count * sizeof(json_def_t))) = test;

			root_obj.children_count++;

			json_def_t* my_def = (json_def_t*)((char*)root_obj.children + ((root_obj.children_count - 1) * sizeof(json_def_t)));

			switch(parse_value.type)
			{
				case _float_t: 
				{
					printf("Key: \"%s\"\t float value: %f\n", my_def->key, my_def->value.float_val);
					break;
				}

				case int_t:
				{
					printf("Key: \"%s\"\t int value: %i\n", my_def->key, my_def->value.int_val);
					break;
				}

				case string_t:
				{
					printf("Key: \"%s\"\t string value: \"%s\"\n", my_def->key, my_def->value.str_val);
					break;
				}

				default: 
				{
					printf("Key: \"%s\"\t Can't display this data type\n", my_def->key);
					break;
				}
			}
		} 
	} while(!json_cursor.end);
}























































