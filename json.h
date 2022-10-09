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
		TYPEID_NIL, TYPEID_INT, TYPEID_FLOAT, TYPEID_BOOL, TYPEID_STR, TYPEID_ARR, TYPEID_OBJ
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
	protected:
		char ch(uint64_t x) const;

	public:
		// Returns the specific type of the data.
		virtual type_id json_type( void ) const = 0;

		// Reads a value from an input stream.
		virtual result<const char*> json_read(const char *json) = 0;

		// Writes the value to an out stream.
		virtual char* json_write(char *out_json) const = 0;

		// Computes the length of the JSON string.
		virtual uint64_t json_len( void ) const = 0;
	};

	class int_type : public data_type
	{
	private:
		int64_t value;

	public:
		type_id json_type( void ) const;
		result<const char*> json_read(const char *json);
		char* json_write(char *json_out) const;
		uint64_t json_len( void ) const;
	};

	class float_type : public data_type
	{
	private:
		double value;

	public:
		type_id json_type( void ) const;
	};

	class bool_type : public data_type
	{
	private:
		bool value;

	public:
		type_id json_type( void ) const;
	};

	class string_type : public data_type
	{
	private:
		char *value;

	public:
		type_id json_type( void ) const;
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
	class nil_type : public data_type
	{
	public:
		type_id json_type( void ) const;
	};

}

#endif
