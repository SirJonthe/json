#include "json.h"

char json::data_type::ch(uint64_t x) const
{
	static const char DIGITS[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	return DIGITS[x];
}

json::type_id json::int_type::json_type( void ) const
{
	return json::TYPEID_INT;
}


json::result<const char*> json::int_type::json_read(const char *json)
{

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
