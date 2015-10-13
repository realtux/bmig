#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json.h>

#include "util.h"

enum json_type type;

char *read_config() {
	long config_size;
	char *json = read_file("config.json", &config_size);

	return json;
}

char *get_value(char *json, char *key_name) {
	json_object *jobj = json_tokener_parse(json);

	json_object_object_foreach(jobj, key, val) {
        type = json_object_get_type(val);

        if (type == json_type_string) {
			if (strcmp(key, key_name) == 0) {
				return (char *) json_object_get_string(val);
			}
		}
    }

    return NULL;
}
