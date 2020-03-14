#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "include/jsonc.h"

preparse_data_t _json_preparse(json_document_t* document, const char* json_string)
{
	// Create preparsing cursor
	json_cursor_t json_cursor;
	_cursor_init(&json_cursor, json_string);

	// Create object space
	int object_guess = ((strlen(json_string) / 1000000) * 1000) + 20; // 1000 obj per megabyte
	document->obj_pool = calloc(object_guess, sizeof(json_obj_t));

	DEBUG_PRINT("Object count guess: %i\n", object_guess);

	// Create preparsing data structure
	preparse_data_t ppd = { 
		.obj_count = 0, 
		.def_count = 0, 
		.string_length = 0
	};

	int current_object = -1;
	int parent_object = -1;
	int allocated_def_count = 0;
	do
	{
		// Increase size by 20% if reached max
		if(ppd.obj_count >= object_guess)
		{
			object_guess = object_guess * 1.2;
			document->obj_pool = realloc(document->obj_pool, object_guess * sizeof(json_obj_t));
		}

		switch(json_cursor.character)
		{
			case '"': {
				ppd.string_length += _json_seek_to_inclstr(&json_cursor, '"', 1);
				break;
			}

			case '{': {
				parent_object = current_object;
				current_object = ppd.obj_count;

				document->obj_pool[current_object].parent_object_index = parent_object;

				document->obj_pool[current_object].start_index = json_cursor.position;
				document->obj_pool[current_object].children_count++;

				ppd.def_count++;
				ppd.obj_count++;
				break;
			}

			case '}': {
				document->obj_pool[current_object].children_offset = allocated_def_count;
				allocated_def_count += document->obj_pool[current_object].children_count;

				current_object = document->obj_pool[current_object].parent_object_index;
				parent_object = document->obj_pool[current_object].parent_object_index;
				break;
			}

			case ',': {
				document->obj_pool[current_object].children_count++;

				ppd.def_count++;
				break;
			}

			case '[': {
				parent_object = current_object;
				current_object = ppd.obj_count;

				document->obj_pool[current_object].parent_object_index = parent_object;

				document->obj_pool[current_object].start_index = json_cursor.position;
				document->obj_pool[current_object].children_count++;

				ppd.def_count++;
				ppd.obj_count++;
				break;
			}

			case ']': {
				document->obj_pool[current_object].children_offset = allocated_def_count;
				allocated_def_count += document->obj_pool[current_object].children_count;

				current_object = document->obj_pool[current_object].parent_object_index;
				parent_object = document->obj_pool[current_object].parent_object_index;
				break;
			}
		}

		_cursor_push(&json_cursor, 1);
	} while(!json_cursor.end);

	return ppd;
}
