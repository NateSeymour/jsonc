#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>

#include "include/jsonc.h"

// Lexer
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

json_document_t json_parse(const char* json_string)
{
	// PREPARSE
	printf("%s\n", json_string);

	json_document_t document;

	preparse_data_t ppd = _json_preparse(json_string);
	int res_obj = ppd.obj_count * sizeof(json_obj_t);
	int res_def = ppd.def_count * sizeof(json_def_t);

	int pool_size = res_obj + res_def + ppd.string_length;

	printf("Number of Objects: %i, Number of Definitions: %i, String length, %i\n", ppd.obj_count, ppd.def_count, ppd.string_length);
	printf("Allocation size: %i\n", pool_size);

	document.json_pool = malloc(pool_size);
	document.ppd = ppd;

	document.obj_pool = document.json_pool;
	document.def_pool = (char*)document.obj_pool + res_obj;
	document.string_pool = (char*)document.def_pool + res_def;

	// PARSE
	/*
	1. create objects
	2. populate their definitions
	*/

	return document;
}

json_document_t json_parse_file(const char* file_name)
{
	FILE* input_file = fopen(file_name, "r");

	// Get file size
	struct stat input_stat;
	stat(file_name, &input_stat);

	long file_size = input_stat.st_size;

	// Allocate enough ram
	const char* buffer = (char*) malloc((size_t)file_size);

	// Read file
	fread((void*)buffer, file_size, 1, input_file);

	// Close file
	fclose(input_file);

	// Parse
	json_document_t document = json_parse(buffer);

	// Cleanup
	free((void*)buffer);
	free(document.json_pool);

	return document;
}

__attribute__((constructor)) static void initializer(void) 
{
	printf("Json C Parser initialized...\n");
}





















































