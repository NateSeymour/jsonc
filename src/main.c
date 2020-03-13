#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>

#ifdef BENCHMARK
#include <time.h>
#endif

#include "include/jsonc.h"

void release_json(json_document_t* document)
{
	free(document->json_pool);
}

json_def_t json(json_obj_t* obj, const char* path)
{

}

void _json_allocate_memory(json_document_t* document, preparse_data_t* ppd)
{
	// Set ppd
	document->ppd = *ppd;

	// Calculate hash map size (number of elements)
	document->hash_map_size = nearest_power(ppd->def_count, 2);

	DEBUG_PRINT("Hashmap size: %i\n", document->hash_map_size);

	// Get allocation size
	int res_obj = ppd->obj_count * sizeof(json_obj_t);
	int res_def = ppd->def_count * sizeof(json_def_t);
	int res_hash_table = document->hash_map_size * sizeof(int);

	int pool_size = res_obj + res_def + res_hash_table + ppd->string_length;

	DEBUG_PRINT("Number of Objects: %i, Number of Definitions: %i, String length: %i, Array values: %i\n", ppd->obj_count, ppd->def_count, ppd->string_length, ppd->array_value_count);
	DEBUG_PRINT("Allocation size: %i Bytes\n", pool_size);

	// Allocate memory
	document->json_pool = malloc(pool_size);

	document->obj_pool = document->json_pool;
	document->def_pool = (char*)document->obj_pool + res_obj;
	document->string_pool = (char*)document->def_pool + res_def;
	document->def_index_hash_table = (char*)document->string_pool + ppd->string_length;

	// Set root object and the base string pointer
	document->string_ptr = document->string_pool;
	document->root_obj = document->obj_pool;
}

json_document_t json_parse(const char* json_string)
{
	json_document_t document;

	// Preparse
	preparse_data_t ppd = _json_preparse(json_string);

	// Allocate Ram
	_json_allocate_memory(&document, &ppd);

	// Parse the document
	_json_do_parse(&document, ppd, json_string);

	return document;
}

json_document_t json_parse_file(const char* file_name)
{
	FILE* input_file = fopen(file_name, "r");

	// Get file size
	struct stat input_stat;
	stat(file_name, &input_stat);

	long file_size = input_stat.st_size;

	DEBUG_PRINT("File size: %ld Bytes\n", file_size);

	// Allocate enough ram
	const char* buffer = (char*) malloc((size_t)file_size);

	// Read file
	fread((void*)buffer, file_size, 1, input_file);

	// Close file
	fclose(input_file);

#ifdef BENCHMARK
	clock_t start, end;
	double cpu_time_used;

	start = clock();
#endif

	// Parse
	json_document_t document = json_parse(buffer);

#ifdef BENCHMARK
	end = clock();
	cpu_time_used = (((double) (end - start)) / CLOCKS_PER_SEC) * (double)1000;

	printf("\nTime Taken: %fms\n", cpu_time_used);
#endif

	// Cleanup
	free((void*)buffer);

	return document;
}





















































