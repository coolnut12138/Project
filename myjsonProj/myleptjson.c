#include "myleptjson.h"
#include <assert.h>
#include <stdlib.h>

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

int lept_parse(lept_value* v, const char* json)
{
    lept_context c;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    return lept_parse_value(&c, v);
}

lept_type lept_get_type(const lept_value* v)
{
    assert(v != NULL);
    return v->type;
}