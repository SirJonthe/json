#ifndef JSON_H_INCLUDED__
#define JSON_H_INCLUDED__

#include <stdint.h>
#include <map>
#include <string>

namespace json
{
	// Constants representing all JSON types.
	enum type_id
	{
		TYPEID_NULL,
		TYPEID_BOOL,
		TYPEID_INT,
		TYPEID_FLOAT,
		TYPEID_ARR,
		TYPEID_STR,
		TYPEID_OBJ
	};

	namespace parse
	{
		char      to_ch(uint64_t x);
		uint64_t  to_num(char ch);
		bool      is_num(char ch);
		bool      is_white(char ch);
		uint64_t  skip_white(const char *s);
		bool      is_match(const char *s1, const char *s2);
		char     *print(char *dst, const char *src);
	};

	// The result from a parsing operation.
	template < typename type_t >
	struct result
	{
		type_t out;
		bool   ok;
	};

	// The base class for all data types in JSON.
	class data_type
	{
	public:
		// Returns the specific type of the data.
		virtual type_id json_type( void ) const = 0;

		// Reads a value from an input stream.
		virtual result<const char*> read_json(const char *json) = 0;

		// Writes the value to an out stream.
		virtual char* print_json(char *out_json) const = 0;

		// Computes the length of the JSON string.
		virtual uint64_t json_len( void ) const = 0;
	};

	class int_type : public data_type
	{
	private:
		int64_t value;

	public:
		int_type( void );
		int_type(int64_t val);
		int_type(const int_type&) = default;
		int_type &operator=(const int_type&) = default;

		type_id              json_type( void ) const;
		result<const char*>  read_json(const char *json);
		char                *print_json(char *json_out) const;
		uint64_t             json_len( void ) const;
	};

	class float_type : public data_type
	{
	private:
		double value;

	public:
		float_type( void );
		float_type(double val);
		float_type(const float_type&) = default;
		float_type &operator=(const float_type&) = default;

		type_id              json_type( void ) const;
		result<const char*>  read_json(const char *json);
		char                *print_json(char *json_out) const;
		uint64_t             json_len( void ) const;
	};

	class bool_type : public data_type
	{
	private:
		bool value;

	public:
		bool_type( void );
		bool_type(bool val);
		bool_type(const bool_type&) = default;
		bool_type &operator=(const bool_type&) = default;

		type_id              json_type( void ) const;
		result<const char*>  read_json(const char *json);
		char                *print_json(char *json_out) const;
		uint64_t             json_len( void ) const;
	};

	class string_type : public data_type
	{
	private:
		char     *value;
		uint64_t  length;

	public:
		string_type( void );
		string_type(const char *val, uint64_t len);
		string_type(const string_type&) = default;
		string_type &operator=(const string_type&) = default;
		~string_type( void );

		type_id              json_type( void ) const;
		result<const char*>  read_json(const char *json);
		char                *print_json(char *json_out) const;
		uint64_t             json_len( void ) const;
	};

	class array_type : public data_type
	{
	private:
		data_type *value;

	public:
		type_id json_type( void ) const;

		// NOTE that since elements in the array can be any object the stored elements will always be 'data' even if it is a pure int array or similar. However, writing will always try to infer correct the type.
	};

	// The data type for objects.
	class object_type : public data_type
	{
	private:
		std::map<std::string, data_type*> value;
		// NOTE: Basically a map of 'data' objects
	public:
		type_id json_type( void ) const;
	};

	// The data type for null values.
	class null_type : public data_type
	{
	public:
		type_id              json_type( void ) const;
		result<const char*>  read_json(const char *json);
		char                *print_json(char *json_out) const;
		uint64_t             json_len( void ) const;
	};

	char *print(char *json_out, int64_t value);
	char *print(char *json_out, double value);
	char *print(char *json_out, bool value);
	char *print(char *json_out, const char *value, uint64_t len);
	char *print(char *json_out, const data_type &value);
}

#endif
