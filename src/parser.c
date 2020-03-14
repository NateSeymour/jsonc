#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "include/jsonc.h"

#define THREAD_COUNT 1

typedef struct {
	json_value_t value;
	json_value_types type;
} parse_value_data_t;

typedef struct {
	int obj_count;
	int def_count;
	int current_obj_index;
	int strings;
	int array_val_count;
} parse_data_t;

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

	if(json_cursor->character != '{' && json_cursor->character != '[')
	{
		_json_seek_to_obj(json_cursor);
	}

	while(obj_count != closing_count)
	{
		_cursor_push(json_cursor, 1);

		if(!json_cursor->in_string)
		{
			if(json_cursor->character == '{' || json_cursor->character == '[') obj_count++;
			if(json_cursor->character == '}' || json_cursor->character == ']') closing_count++;
		}
	}

	_cursor_push(json_cursor, 1);
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
		else if(json_cursor->postcursor == '{' || json_cursor->postcursor == '[')
		{
			//pd->obj_count++;
			//value.value.obj_val = &document->obj_pool[pd->obj_count - 1];
			//((json_obj_t*)value.value.obj_val)->start_index = json_cursor->position + 1;
			value.type = obj_t;
			_cursor_push(json_cursor, 1);
			_skip_object(json_cursor);
			break;
		}
		else if(json_cursor->postcursor == 't')
		{
			value.value.bool_val = 1;
			value.type = bool_t;
			_cursor_push(json_cursor, 1);
			break;
		}
		else if(json_cursor->postcursor == 'f')
		{
			value.value.bool_val = 0;
			value.type = bool_t;
			_cursor_push(json_cursor, 1);
			break;
		}
		else if(json_cursor->postcursor == 'n')
		{
			value.value.bool_val = 0;
			value.type = bool_t;
			_cursor_push(json_cursor, 1);
			break;
		}
		else if(json_cursor->postcursor == '}' 
			|| json_cursor->postcursor == ']')
		{
			value.type = none_t;
			break;
		}

		_cursor_push(json_cursor, 1);
	}

	return value;
}

#ifdef DEBUG
void _json_print_def(json_def_t* def)
{
	switch(def->type)
	{
		case _float_t: 
		{
			DEBUG_PRINT("Key: \"%s\"\t float value: %f\n", def->key, def->value.float_val);
			break;
		}

		case int_t:
		{
			DEBUG_PRINT("Key: \"%s\"\t int value: %i\n", def->key, def->value.int_val);
			break;
		}

		case string_t:
		{
			DEBUG_PRINT("Key: \"%s\"\t string value: \"%s\"\n", def->key, def->value.str_val);
			break;
		}

		case bool_t:
		{
			DEBUG_PRINT("Key: \"%s\"\t bool value: %i\n", def->key, def->value.bool_val);
			break;
		}

		default: 
		{
			DEBUG_PRINT("Key: \"%s\"\t Can't display this data type\n", def->key);
			break;
		}
	}
}
#endif

// Return 1 when done with object
int _json_parse_definition(json_document_t* document, json_cursor_t* json_cursor, parse_data_t* pd, json_obj_t* obj, int* parsed_defs)
{
	// Get named definiton
	char* definition_key = _parse_getstring(document, json_cursor);

	if(definition_key)
	{
		_json_seek_to(json_cursor, ':', 1);

		parse_value_data_t parse_value = _parse_getvalue(document, json_cursor, pd);

		json_def_t test = {
			.key = definition_key,
			.value = parse_value.value,
			.type = parse_value.type
		};

		obj->children[*parsed_defs] = test;
		json_def_t* my_def = &obj->children[*parsed_defs];

		*parsed_defs++;
		pd->def_count++;

		// Put index in hashmap
		//int index = hash_map_mapper(definition_key, document->hash_map_size);
		//DEBUG_PRINT("Index: %i\n", index);
		//document->def_index_hash_table[index] = my_def;
	#ifdef DEBUG
		_json_print_def(my_def);
	#endif
	}

	// set up for the next definition
	while(!json_cursor->end)
	{
		if(!json_cursor->in_string)
		{
			if(json_cursor->character == ',') return 0;
			if(json_cursor->character == '}') return 1;
		}
		
		_cursor_push(json_cursor, 1);
	}

	return 1;
}

int _json_parse_array_definition(json_document_t* document, json_cursor_t* json_cursor, parse_data_t* pd, json_obj_t* obj, int* parsed_defs)
{
	parse_value_data_t parse_value = _parse_getvalue(document, json_cursor, pd);

	if(parse_value.type != none_t)
	{
		json_def_t test = {
			.key = 0,
			.value = parse_value.value,
			.type = parse_value.type
		};

		obj->children[*parsed_defs] = test;
		json_def_t* my_def = &obj->children[*parsed_defs];

		*parsed_defs++;
		pd->def_count++;
	#ifdef DEBUG
		_json_print_def(my_def);
	#endif

		// set up for the next definition
		while(!json_cursor->end)
		{
			if(!json_cursor->in_string)
			{
				if(json_cursor->character == ',') return 0;
				if(json_cursor->character == ']') return 1;
			}
			
			_cursor_push(json_cursor, 1);
		}

		return 0;
	}

	return 1;
}

void _json_parse_object(json_document_t* document, json_cursor_t* json_cursor, parse_data_t* pd, int obj_index)
{
	DEBUG_PRINT("\nObject number %i\n", obj_index);
	json_obj_t* current_obj = &document->obj_pool[obj_index];

	current_obj->children = document->def_pool + current_obj->children_offset;

	_cursor_move_unsafe(json_cursor, current_obj->start_index);

	int obj_parsed_defs = 0;

	// Get first character
	if(json_cursor->character != '{' && json_cursor->character != '[')
	{
		_json_seek_to_obj(json_cursor);
	}

	if(json_cursor->character == '{')
	{
		while(1)
		{
			if(_json_parse_definition(document, json_cursor, pd, current_obj, &obj_parsed_defs) == 1) break;
		}
	}
	else if(json_cursor->character == '[')
	{
		while(1)
		{
			if(_json_parse_array_definition(document, json_cursor, pd, current_obj, &obj_parsed_defs) == 1) break;
		}
	}
	
}

typedef struct {
	json_document_t* document;
	const char* json_string;
	
	preparse_data_t ppd;

	int step;
	int start_index;
} parse_thread_args_t;

void* _json_parse_thread(void* argsraw)
{
	parse_thread_args_t* args = (parse_thread_args_t*)argsraw;

	// Create parsing cursor
	json_cursor_t json_cursor;
	_cursor_init(&json_cursor, args->json_string);

	// Create parse data obj
	parse_data_t pd = {
		.obj_count = 0,
		.def_count = 0,
		.current_obj_index = 0
	};

	// Parse Loop
	for(int i = args->start_index; i < args->ppd.obj_count; i += args->step)
	{
		_json_parse_object(args->document, &json_cursor, &pd, i);
	}

	return 0;
}

void _json_do_parse(json_document_t* document, preparse_data_t ppd, const char* json_string)
{
	DEBUG_PRINT("\nParsing results: \n");
/*
	parse_thread_args_t thread_args[THREAD_COUNT];
	pthread_t threads[THREAD_COUNT];

	for(int i = 0; i < THREAD_COUNT; i++)
	{
		thread_args[i].document = document;
		thread_args[i].json_string = json_string;
		thread_args[i].ppd = ppd;
		thread_args[i].step = THREAD_COUNT + 1;
		thread_args[i].start_index = i;

		pthread_create(&threads[i], 0, _json_parse_thread, (void*)&thread_args[i]);
	}*/

	parse_thread_args_t args = {
		.document = document,
		.json_string = json_string,
		.ppd = ppd,
		.step = THREAD_COUNT + 1,
		.start_index = THREAD_COUNT
	};

	_json_parse_thread(&args);

	/*for(int i = 0; i < THREAD_COUNT; i++)
	{
		pthread_join(threads[i], NULL);
	}*/
}























































