#ifndef _JSON_C_H_
#define _JSON_C_H_

// Types
typedef enum {
	string_t,
	int_t,
	_float_t,
	array_t
} json_value_types;

typedef union {
	char* str_val;
	int int_val;
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
	int children_count;
	void* children;
} json_obj_t;

typedef struct {
	int obj_count;
	int def_count;
	int string_length;
} preparse_data_t;

typedef struct {
	json_obj_t root_obj;
	preparse_data_t ppd;

	void* json_pool;

	json_obj_t* obj_pool;
	json_def_t* def_pool;
	void* string_pool;
} json_document_t;

typedef struct {
	const char* json_string;

	int precursor_escaped;
	char precursor;

	char character;
	int length;
	int position;

	int end;
	int escaped;
	int in_string;
} json_cursor_t;

// Math
int clamp(int x, int lower, int upper);

// Cursor
void _cursor_push(json_cursor_t* json_cursor, int step);
void _cursor_move(json_cursor_t* json_cursor, int position);

// Preparser
int _json_preparse_str_count(json_cursor_t* json_cursor);
int _json_preparse_obj_count(json_cursor_t* json_cursor);
int _json_preparse_def_count(json_cursor_t* json_cursor);
preparse_data_t _json_preparse(const char* json_string);

// Lexer
int _json_seek_to(json_cursor_t* json_cursor, char seek, int step);

// Make pub
extern json_document_t json_parse_file(const char* file_name);

#endif
