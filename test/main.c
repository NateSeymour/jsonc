#include "jsonc/jsonc.h"
#include <stdio.h>

int main()
{
	json_document_t document = json_parse_file("build/test-big-1.json");

	release_json(&document);

	return 0;
}
