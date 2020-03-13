#ifndef _JSON_C_H_
#define _JSON_C_H_

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) do {} while(0)
#endif

// Types
typedef enum {
	string_t,
	int_t,
	_float_t,
	array_t,
	obj_t,
	none_t,
	bool_t
} json_value_types;

typedef union {
	char* str_val;
	int int_val;
	int bool_val;
	float float_val;
	void* array_val;
	void* obj_val;
} json_value_t;

typedef struct {
	char* key;
	json_value_types type;
	json_value_t value;
} json_def_t;

typedef struct {
	int start_index;
	int children_count;
	json_def_t* children;
} json_obj_t;

typedef struct {
	int obj_count;
	int def_count;
	int string_length;
	int array_value_count;
} preparse_data_t;

typedef struct {
	json_obj_t* root_obj;
	preparse_data_t ppd;

	int hash_map_size;

	void* json_pool;

	void* string_ptr;

	json_obj_t* obj_pool;
	json_def_t* def_pool;
	int* def_index_hash_table;
	char* string_pool;
} json_document_t;

typedef struct {
	const char* json_string;

	int precursor_escaped;
	char precursor;

	int postcursor_escaped;
	char postcursor;

	char character;
	int length;
	int position;

	int end;
	int escaped;
	int in_string;
} json_cursor_t;

// Math
int clamp(int x, int lower, int upper);
int nearest_power(int x, int pow);

// Cursor
void _cursor_push(json_cursor_t* json_cursor, int step);
void _cursor_move(json_cursor_t* json_cursor, int position);
void _cursor_move_unsafe(json_cursor_t* json_cursor, int position);
void _cursor_init(json_cursor_t* json_cursor, const char* json_string);
void _cursor_update(json_cursor_t* json_cursor);

// Preparser
preparse_data_t _json_preparse(const char* json_string);

// Lexer
int _json_seek_to(json_cursor_t* json_cursor, char seek, int step);
int _json_seek_to_inclstr(json_cursor_t* json_cursor, char seek, int step);
int _json_seek_to_obj(json_cursor_t* json_cursor);

// Parser
void _json_do_parse(json_document_t* document, preparse_data_t ppd, const char* json_string);

// Make pub
extern json_document_t json_parse_file(const char* file_name);
extern json_document_t json_parse(const char* json_string);
extern void release_json(json_document_t* document);

#endif
