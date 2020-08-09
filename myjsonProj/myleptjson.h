#ifndef MYLEPTJSON_H__
#define MYLEPTJSON_H__

typedef enum {
    LEPT_NULL,
    LEPT_FALSE,
    LEPT_TRUE,
    LEPT_NUMBER,
    LEPT_STRING,
    LEPT_ARRAY,
    LEPT_OBJECT
}lept_type;

typedef struct {
    lept_type type;
}lept_value;

// 返回值
enum {
    LEPT_PARSE_OK = 0,              //无错误
    LEPT_PARSE_EXCEPT_VALUE,        //一个json只含有空白
    LEPT_PARSE_INVALID_VALUE,       //若值不是那三种字符
    LEPT_PARSE_ROOT_NOT_SINGULAR    //若一个值之后，在空白之后还有其他字符
};

int lept_parse(lept_value* v, const char* json);

lept_type lept_get_type(const lept_value* v);

#endif /*MYLEPTJSON_H__*/