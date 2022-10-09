#include "json.h"

char json::data_type::ch(uint64_t x)
{
	static const char DIGITS[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	return DIGITS[x];
}

uint64_t json::data_type::num(char ch)
{
	switch (ch) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	}
	return -1;
}

bool json::data_type::is_num(char ch)
{
	return num(ch) >= 0;
}

bool json::data_type::is_white(char ch)
{
	switch (ch) {
	case ' ':
	case '\t':
	case '\n':
	case '\r':
		return true;
	}
	return false;
}

uint64_t json::data_type::skip_white(const char *s)
{
	uint64_t caret = 0;
	while (s[caret] != 0 && is_white(s[caret])) {
		++caret;
	}
	return caret;
}

bool json::data_type::is_match(const char *s1, const char *s2)
{
	while (s1 != 0 && s2 != 0) {
		if (*(s1++) != *(s2++)) {
			return false;
		}
	}
	return true;
}

char *json::data_type::print(char *dst, const char *src)
{
	while (*src != 0) {
		*(dst++) = *(src++);
	}
	return dst;
}

json::int_type::int_type( void ) : value(0) {}

json::int_type::int_type(int64_t val) : value(val) {}

json::type_id json::int_type::json_type( void ) const
{
	return json::TYPEID_INT;
}

json::result<const char*> json::int_type::json_read(const char *json)
{
	uint64_t caret = skip_white(json);
	const uint64_t c = caret;
	while (is_num(*json)) {
		++json;
	}
	if (c == caret) {
		return result<const char*>{
			json + caret,
			false
		};
	}
	value = 0;
	for (int64_t i = 0; i < caret; ++i) {
		value = value * 10 + num(json[i]);
	}
	return result<const char*>{
		json + caret,
		true
	};
}

char *json::int_type::json_write(char *json_out) const
{
	uint64_t caret = 0;
	if (value < 0) {
		json_out[caret++] = '-';
	}
	uint64_t x = abs(value);
	while (x >= 0) {
		json_out[caret++] = ch(x % 10);
		x /= 10;
	}
	return json_out + caret;
}

uint64_t json::int_type::json_len( void ) const
{
	uint64_t digits = value >= 0 ? 1 : 2;
	uint64_t x = abs(value);
	while (x > 0) {
		++digits;
		x /= 10;
	}
	return digits;
}

json::type_id json::float_type::json_type( void ) const
{
	return json::TYPEID_FLOAT;
}

json::type_id json::bool_type::json_type( void ) const
{
	return json::TYPEID_BOOL;
}

json::result<const char*> json::bool_type::json_read(const char *json)
{
	uint64_t caret = skip_white(json);
	if (is_match(json + caret, "true")) {
		value = true;
		caret += 4;
	} else if (is_match(json + caret, "false")) {
		value = false;
		caret += 5;
	} else {
		return result<const char*>{
			json + caret,
			false
		};
	}
	return result<const char*>{
		json + caret,
		true
	};
}

char *json::bool_type::json_write(char *json_out) const
{
	return value ? print(json_out, "true") : print(json_out, "false");
}

uint64_t json::bool_type::json_len( void ) const
{
	return value ? 4 : 5;
}

json::type_id json::string_type::json_type( void ) const
{
	return json::TYPEID_STR;
}

json::type_id json::array_type::json_type( void ) const
{
	return json::TYPEID_ARR;
}

json::type_id json::object_type::json_type( void ) const
{
	return json::TYPEID_OBJ;
}

json::type_id json::nil_type::json_type( void ) const
{
	return json::TYPEID_NIL;
}

json::result<const char*> json::nil_type::json_read(const char *json)
{
	uint64_t caret = skip_white(json);
	if (!is_match(json, "null")) {
		return result<const char*>{
			json + caret,
			false
		};
	}
	return result<const char*>{
		json + caret + json_len(),
		true
	};
}

char *json::nil_type::json_write(char *json_out) const
{
	return print(json_out, "null");
}

uint64_t json::nil_type::json_len( void ) const
{
	return 4;
}
