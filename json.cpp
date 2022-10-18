#include "json.h"

char json::parse::to_ch(uint64_t x)
{
	static const char DIGITS[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	return DIGITS[x];
}

uint64_t json::parse::to_num(char ch)
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

bool json::parse::is_num(char ch)
{
	return to_num(ch) >= 0;
}

bool json::parse::is_white(char ch)
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

uint64_t json::parse::skip_white(const char *s)
{
	uint64_t caret = 0;
	while (s[caret] != 0 && is_white(s[caret])) {
		++caret;
	}
	return caret;
}

bool json::parse::is_match(const char *s1, const char *s2)
{
	while (s1 != 0 && s2 != 0) {
		if (*(s1++) != *(s2++)) {
			return false;
		}
	}
	return true;
}

char *json::parse::print(char *dst, const char *src)
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

json::result<const char*> json::int_type::read_json(const char *json)
{
	uint64_t caret = parse::skip_white(json);
	const uint64_t c = caret;
	while (parse::is_num(*json)) {
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
		value = value * 10 + parse::to_num(json[i]);
	}
	return result<const char*>{
		json + caret,
		true
	};
}

char *json::int_type::print_json(char *json_out) const
{
	uint64_t caret = 0;
	if (value < 0) {
		json_out[caret++] = '-';
	}
	uint64_t x = abs(value);
	while (x >= 0) {
		json_out[caret++] = parse::to_ch(x % 10);
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


json::float_type::float_type( void ) : value(0.0)
{}

json::float_type::float_type(double val) : value(val)
{}

json::type_id json::float_type::json_type( void ) const
{
	return json::TYPEID_FLOAT;
}

json::result<const char*> json::float_type::read_json(const char *json_in)
{
	return json::result<const char*>{
		json_in,
		false
	};
}

char *json::float_type::print_json(char *json_out) const
{
	// TODO: number can start with + or -
	// TODO: E/e can start with + or -
	uint64_t caret = json::parse::skip_white(json_out);

	int64_t i = 0;
	while (caret < len && json::parse::is_num(json_out[caret])) {
		i = i * 10 + json::parse::to_num(json_out[caret]);
		++caret;
	}

	double f = double(i);
	if (caret < len && json_out[caret] == '.') {
		++caret;
		double r = 0;
		uint64_t d = 0;
		while (caret < len && json::parse::is_num(json_out[caret])) {
			r = r * 10 + json::parse::to_num(json_out[caret]);
			++caret;
			++d;
		}
		for (uint64_t j = 0; j < d; ++j) {
			f *= 0.1;
		}
		f += r;
	}
	if (caret < len && json_out[caret] == 'e' || json_out[caret] == 'E') {
		++caret;
		int64_t e = 0;
		while (caret < len && json::parse::is_num(json_out[caret])) {
			e = e * 10 + json::parse::to_num(json_out[caret]);
			++caret;
		}
		if (e >= 0) {
			for (int64_t j = 0; j < e; ++j) {
				f = f * 10.0;
			}
		} else {
			for (int64_t j = 0; j > e; --j) {
				f = f * 0.1;
			}
		}
	}

	return json_out + caret;
}

uint64_t json::float_type::json_len( void ) const
{
	return 0;
}


json::bool_type::bool_type( void ) : value(false)
{}

json::bool_type::bool_type(bool val) : value(val)
{}

json::type_id json::bool_type::json_type( void ) const
{
	return json::TYPEID_BOOL;
}

json::result<const char*> json::bool_type::read_json(const char *json)
{
	uint64_t caret = parse::skip_white(json);
	if (parse::is_match(json + caret, "true")) {
		value = true;
		caret += 4;
	} else if (parse::is_match(json + caret, "false")) {
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

char *json::bool_type::print_json(char *json_out) const
{
	return value ? parse::print(json_out, "true") : parse::print(json_out, "false");
}

uint64_t json::bool_type::json_len( void ) const
{
	return value ? 4 : 5;
}


json::string_type::string_type( void ) : value(nullptr), length(0)
{}

json::string_type::string_type(const char *val, uint64_t len) : value(val), length(len)
{}

json::type_id json::string_type::json_type( void ) const
{
	return json::TYPEID_STR;
}

json::result<const char*> json::string_type::read_json(const char *json_in)
{
	return json::result<const char*>{
		json_in,
		false
	};
}


json::type_id json::array_type::json_type( void ) const
{
	return json::TYPEID_ARR;
}


json::type_id json::object_type::json_type( void ) const
{
	return json::TYPEID_OBJ;
}


json::type_id json::null_type::json_type( void ) const
{
	return json::TYPEID_NULL;
}

json::result<const char*> json::null_type::read_json(const char *json)
{
	uint64_t caret = parse::skip_white(json);
	if (!parse::is_match(json, "null")) {
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

char *json::null_type::print_json(char *json_out) const
{
	return parse::print(json_out, "null");
}

uint64_t json::null_type::json_len( void ) const
{
	return 4;
}


char *json::print(char *json_out, int64_t value)
{
	return json::int_type(value).print_json(json_out);
}

char *json::print(char *json_out, double value)
{
	return json::float_type(value).print_json(json_out);
}

char *json::print(char *json_out, bool value)
{
	return json::bool_type(value).print_json(json_out);
}

char *json::print(char *json_out, const char *value, uint64_t len)
{
	return json::string_type(value, len).print_json(json_out);
}

char *json::print(char *json_out, const data_type &type)
{
	return type.print_json(json_out);
}

/*json::data_type *tokenize_num(const char *json, uint64_t len)
{
	// TODO: number can start with + or -
	// TODO: E/e can start with + or -
	uint64_t caret = json::parse::skip_white(json);

	int64_t i = 0;
	while (caret < len && json::parse::is_num(json[caret])) {
		i = i * 10 + json::parse::to_num(json[caret]);
		++caret;
	}
	bool is_float = false;

	double f = double(i);
	if (caret < len && json[caret] == '.') {
		++caret;
		double r = 0;
		uint64_t d = 0;
		while (caret < len && json::parse::is_num(json[caret])) {
			r = r * 10 + json::parse::to_num(json[caret]);
			++caret;
			++d;
		}
		for (uint64_t j = 0; j < d; ++j) {
			f *= 0.1;
		}
		f += r;
		is_float = true;
	}
	if (caret < len && json[caret] == 'e' || json[caret] == 'E') {
		++caret;
		int64_t e = 0;
		while (caret < len && json::parse::is_num(json[caret])) {
			e = e * 10 + json::parse::to_num(json[caret]);
			++caret;
		}
		if (e >= 0) {
			for (int64_t j = 0; j < e; ++j) {
				f = f * 10.0;
			}
		} else {
			for (int64_t j = 0; j > e; --j) {
				f = f * 0.1;
			}
		}
		is_float = true;
	}

	return !is_float ? json::int_type(i) : json::float_type(f);
}*/


