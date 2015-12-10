/*
 * Copyright 2008-2015 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <aerospike/as_bin.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *	TYPES
 *****************************************************************************/

typedef enum as_cdt_paramtype_e {

	AS_CDT_PARAM_INDEX = 3,
	AS_CDT_PARAM_COUNT = 4,
	AS_CDT_PARAM_PAYLOAD = 5,
	AS_CDT_PARAM_LAST_INDEX = 6,

	AS_CDT_PARAM_MAP_KEY = 11,
	AS_CDT_PARAM_MAP_VALUE = 12,

} as_cdt_paramtype;

typedef enum as_cdt_optype_e {

	// ------------------------------------------------------------------------
	// List Operation

	// Add to list
	AS_CDT_OP_LIST_APPEND        = 1,
	AS_CDT_OP_LIST_APPEND_ITEMS  = 2,
	AS_CDT_OP_LIST_INSERT        = 3,
	AS_CDT_OP_LIST_INSERT_ITEMS  = 4,

	// Remove from list
	AS_CDT_OP_LIST_POP           = 5,
	AS_CDT_OP_LIST_POP_RANGE     = 6,
	AS_CDT_OP_LIST_REMOVE        = 7,
	AS_CDT_OP_LIST_REMOVE_RANGE  = 8,

	// Other list modifies
	AS_CDT_OP_LIST_SET           = 9,
	AS_CDT_OP_LIST_TRIM          = 10,
	AS_CDT_OP_LIST_CLEAR         = 11,
	AS_CDT_OP_LIST_INCREMENT_BY  = 12,

	// Read from list
	AS_CDT_OP_LIST_SIZE          = 16,
	AS_CDT_OP_LIST_GET           = 17,
	AS_CDT_OP_LIST_GET_RANGE     = 18,

	// ------------------------------------------------------------------------
    // Map Operation

	// Adding <key, value> to the Map
	AS_CDT_OP_MAP_PUT            = 32,
	AS_CDT_OP_MAP_PUT_ITEMS      = 33,

	// Op by key
	AS_CDT_OP_MAP_GET            = 34,
	AS_CDT_OP_MAP_GET_MATCHING   = 35,
	AS_CDT_OP_MAP_REMOVE         = 36,
	AS_CDT_OP_MAP_REMOVE_ITEMS   = 37,
	AS_CDT_OP_MAP_CONTAINS_KEY   = 38,
	AS_CDT_OP_MAP_INCREMENT_BY   = 39,
	AS_CDT_OP_MAP_CONTAINS_VALUE = 40,

	// Misc
	AS_CDT_OP_MAP_GET_ITEMS      = 41,
	AS_CDT_OP_MAP_KEYS           = 42,
	AS_CDT_OP_MAP_VALUES         = 43,
	AS_CDT_OP_MAP_CLEAR          = 44,
	AS_CDT_OP_MAP_SIZE           = 45,

} as_cdt_optype;

/**
 *	Operation Identifiers
 */
typedef enum as_operator_e {

	/**
	 *	Return the bin from the cluster.
	 */
	AS_OPERATOR_READ       = 1,
	
	/**
	 *	Update the bin.
	 */
	AS_OPERATOR_WRITE      = 2,

	AS_OPERATOR_CDT_READ   = 3,
	AS_OPERATOR_CDT_MODIFY = 4,

	/**
	 *	Increment a bin containing an
	 *	integer value.
	 */
	AS_OPERATOR_INCR       = 5,

	/**
	 *	Append bytes to the bin containing
	 *	either a string or blob.
	 */
	AS_OPERATOR_APPEND     = 9,
	
	/**
	 *	Prepend bytes to the bin containing
	 *	either a string or blob.
	 */
	AS_OPERATOR_PREPEND    = 10,

	/**
	 *	Touch the record's ttl.
	 */
	AS_OPERATOR_TOUCH      = 11

} as_operator;

/**
 *	Operation on a bin.
 *	The value for the bin will be applied according to the operation.
 */
typedef struct as_binop_s {

	/**
	 *	The operation to be performed on the bin.
	 */
	as_operator op;

	/**
	 *	The bin the operation will be performed on.
	 */
	as_bin bin;

} as_binop;

/**
 *	Sequence of operations.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 *	as_operations_inita(&ops, 2);
 *	as_operations_add_incr(&ops, "bin1", 123);
 *	as_operations_add_append_str(&ops, "bin2", "abc");
 *	...
 *	as_operations_destroy(&ops);
 *	~~~~~~~~~~
 *
 */
typedef struct as_binops_s {

	/**
	 *	@private
	 *	If true, then as_binops_destroy() will free the entries.
	 */
	bool _free;

	/**
	 *	Number of entries allocated
	 */
	uint16_t capacity;

	/**
	 *	Number of entries used
	 */
	uint16_t size;

	/**
	 *	Sequence of entries
	 */
	as_binop * entries;

} as_binops;

/**
 *	The `aerospike_key_operate()` function provides the ability to execute
 *	multiple operations on a record in the database as a single atomic 
 *	transaction.
 *
 *	The `as_operations` object is used to define the operations to be performed
 *	on the record.
 *
 *	## Initialization
 *
 *	Before using as_operations, you must first initialize it via either:
 *	- as_operations_inita()
 *	- as_operations_init()
 *	- as_operations_new()
 *
 *	as_operations_inita() is a macro that initializes a stack allocated 
 *	as_operations and allocates an internal array of operations. The macro
 *	accepts a pointer to the stack allocated as_operations and the number of
 *	operations to be added.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 *	as_operations_inita(&ops, 2);
 *	~~~~~~~~~~
 *
 *	as_operations_init() is a function that initializes a stack allocated 
 *	as_operations. It differes from as_operations_inita() in that it allocates
 *	the internal array of operations on the heap. It accepts a pointer to the 
 *	stack allocated as_operations and the number of operations to be added.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 *	as_operations_init(&ops, 2);
 *	~~~~~~~~~~
 *	
 *	as_operations_new() is a function that will allocate a new as_operations
 *	on the heap. It will also allocate the internal array of operation on the 
 *	heap.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations * ops = as_operations_new(2);
 *	~~~~~~~~~~
 *
 *	When you no longer needthe as_operations, you can release the resources 
 *	allocated to it via as_operations_destroy().
 *
 *	## Destruction
 *	
 *	When you no longer require an as_operations, you should call 
 *	`as_operations_destroy()` to release it and associated resources.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_destroy(ops);
 *	~~~~~~~~~~
 *
 *	## Usage
 *
 *	as_operations is a sequence of operations to be applied to a record.
 *	
 *	Each of the following operations is added to the end of the sequence
 *	of operations.
 *
 *	When you have compiled the sequence of operations you want to execute,
 *	then you will send it to aerospike_key_operate().
 *
 *
 *	### Modifying a String
 *
 *	Aerospike allows you to append a string to a bin containing
 *	a string.
 *
 *	The following appends a "abc" to bin "bin1".
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_append_str(ops, "bin1", "abc");
 *	~~~~~~~~~~
 *	
 *	There is also a prepend operation, which will add the string
 *	to the beginning of the bin's current value.
 *	
 *	~~~~~~~~~~{.c}
 *	as_operations_add_prepend_str(ops, "bin1", "abc");
 *	~~~~~~~~~~
 *
 *	### Modifying a Byte Array
 *
 *	Aerospike allows you to append a byte array to a bin containing
 *	a byte array.
 *
 *	The following appends a 4 byte sequence to bin "bin1".
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t raw[4] = { 1, 2, 3, 4 };
 *	as_operations_add_append_raw(ops, "bin1", raw, 4);
 *	~~~~~~~~~~
 *	
 *	There is also a prepend operation, which will add the bytes
 *	to the beginning of the bin's current value.
 *
 *	~~~~~~~~~~{.c}
 *	uint8_t raw[4] = { 1, 2, 3, 4 };
 *	as_operations_add_prepend_raw(ops, "bin1", raw, 4);
 *	~~~~~~~~~~
 *
 *	### Increment an Integer
 *
 *	Aerospike allows you to increment the value of a bin 
 *
 *	The following increments the value in bin "bin1" by 4.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_incr(ops, "bin1", 4);
 *	~~~~~~~~~~
 *	
 *	### Write a Value
 *
 *	Write a value into a bin. Overwriting previous value.
 *
 *	The following writes a string "xyz" to "bin1".
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_write_str(ops, "bin1", "xyz");
 *	~~~~~~~~~~
 *	
 *	### Read a Value
 *
 *	Read a value from a bin. This is ideal, if you performed an 
 *	operation on a bin, and want to read the new value.
 *
 *	The following reads the value of "bin1"
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_read(ops, "bin1", "xyz");
 *	~~~~~~~~~~
 *
 *	### Touch a Record
 *
 *	Touching a record will refresh its ttl and increment the generation
 *	of the record.
 *
 *	The following touches a record.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations_add_touch(ops);
 *	~~~~~~~~~~
 *
 *	@ingroup client_objects
 */
typedef struct as_operations_s {

	/**
	 *	@private
	 *	If true, then as_operations_destroy() will free this instance.
	 */
	bool _free;

	/**
	 *	The generation of the record.
	 */
	uint16_t gen;

	/**
	 *	The time-to-live (expiration) of the record in seconds.
	 */
	uint32_t ttl;

	/**
	 * Operations to be performed on the bins of a record.
	 */
	as_binops binops;

} as_operations;

/******************************************************************************
 *	MACROS
 *****************************************************************************/

/**
 *	Initializes a stack allocated `as_operations` (as_operations) and allocates
 *	`__nops` number of entries on the stack.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 * 	as_operations_inita(&ops, 2);
 *	as_operations_add_incr(&ops, "bin1", 123);
 *	as_operations_add_append_str(&ops, "bin2", "abc");
 *	~~~~~~~~~~
 *
 *	@param __ops		The `as_operations *` to initialize.
 *	@param __nops		The number of `as_binops.entries` to allocate on the
 *						stack.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
#define as_operations_inita(__ops, __nops) \
	(__ops)->_free = false;\
	(__ops)->gen = 0;\
	(__ops)->ttl = 0;\
	(__ops)->binops._free = false;\
	(__ops)->binops.capacity = __nops;\
	(__ops)->binops.size = 0;\
	(__ops)->binops.entries = (as_binop *) alloca(sizeof(as_binop) * __nops);

/******************************************************************************
 *	FUNCTIONS
 *****************************************************************************/

/**
 *	Intializes a stack allocated `as_operations`.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops;
 * 	as_operations_init(&ops, 2);
 *	as_operations_add_incr(&ops, "bin1", 123);
 *	as_operations_add_append_str(&ops, "bin2", "abc");
 *	~~~~~~~~~~
 *
 *	Use `as_operations_destroy()` to free the resources allocated to the
 *	`as_operations`.
 *
 *	@param ops 		The `as_operations` to initialize.
 *	@param nops		The number of `as_operations.binops.entries` to allocate on the heap.
 *
 *	@return The initialized `as_operations` on success. Otherwise NULL.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
as_operations * as_operations_init(as_operations * ops, uint16_t nops);

/**
 *	Create and initialize a heap allocated `as_operations`.
 *
 *	~~~~~~~~~~{.c}
 *	as_operations ops = as_operations_new(2);
 *	as_operations_add_incr(ops, "bin1", 123);
 *	as_operations_add_append_str(ops, "bin2", "abc");
 *	~~~~~~~~~~
 *
 *	Use `as_operations_destroy()` to free the resources allocated to the
 *	`as_operations`.
 *
 *	@param nops		The number of `as_operations.binops.entries` to allocate on the heap.
 *
 *	@return The new `as_operations` on success. Otherwise NULL.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
as_operations * as_operations_new(uint16_t nops);

/**
 *	Destroy an `as_operations` and release associated resources.
 *
 *	~~~~~~~~~~{.c}
 * 	as_operations_destroy(binops);
 *	~~~~~~~~~~
 *
 *	@param ops 	The `as_operations` to destroy.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
void as_operations_destroy(as_operations * ops);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write(as_operations * ops, const as_bin_name name, as_bin_value * value);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with an int64_t value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_int64(as_operations * ops, const as_bin_name name, int64_t value);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a double value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_double(as_operations * ops, const as_bin_name name, double value);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_write_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_write_strp(ops, name, value, false);
}

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a NULL-terminated GeoJSON string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name			The name of the bin to perform the operation on.
 *	@param value		The value to be used in the operation.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_geojson_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a NULL-terminated GeoJSON string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name			The name of the bin to perform the operation on.
 *	@param value		The value to be used in the operation. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_write_geojson_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_write_geojson_strp(ops, name, value, false);
}

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param size 		The size of the value.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_write_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);

/**
 *	Add a `AS_OPERATOR_WRITE` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param size 		The size of the value. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_write_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_write_rawp(ops, name, value, size, false);
}

/**
 *	Add a `AS_OPERATOR_READ` bin operation.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_read(as_operations * ops, const as_bin_name name);

/**
 *	Add a `AS_OPERATOR_INCR` bin operation with (required) int64_t value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_incr(as_operations * ops, const as_bin_name name, int64_t value);

/**
 *	Add a `AS_OPERATOR_INCR` bin operation with double value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_incr_double(as_operations * ops, const as_bin_name name, double value);

/**
 *	Add a `AS_OPERATOR_PREPEND` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_prepend_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);

/**
 *	Add a `AS_OPERATOR_PREPEND` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_prepend_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_prepend_strp(ops, name, value, false);
}

/**
 *	Add a `AS_OPERATOR_PREPEND` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param size 		The size of the value.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_prepend_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);

/**
 *	Add a `AS_OPERATOR_PREPEND` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *	@param size 		The size of the value.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_prepend_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_prepend_rawp(ops, name, value, size, false);
}

/**
 *	Add a `AS_OPERATOR_APPEND` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_append_strp(as_operations * ops, const as_bin_name name, const char * value, bool free);

/**
 *	Add a `AS_OPERATOR_APPEND` bin operation with a NULL-terminated string value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_append_str(as_operations * ops, const as_bin_name name, const char * value)
{
	return as_operations_add_append_strp(ops, name, value, false);
}

/**
 *	Add a `AS_OPERATOR_APPEND` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation.
 *	@param size 		The size of the value.
 *	@param free			If true, then the value will be freed when the operations is destroyed.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_append_rawp(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size, bool free);

/**
 *	Add a `AS_OPERATOR_APPEND` bin operation with a raw bytes value.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *	@param name 		The name of the bin to perform the operation on.
 *	@param value 		The value to be used in the operation. Must last for the lifetime of the operations.
 *	@param size 		The size of the value.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
static inline bool as_operations_add_append_raw(as_operations * ops, const as_bin_name name, const uint8_t * value, uint32_t size)
{
	return as_operations_add_append_rawp(ops, name, value, size, false);
}

/**
 *	Add a `AS_OPERATOR_TOUCH` record operation.
 *
 *	@param ops			The `as_operations` to append the operation to.
 *
 *	@return true on success. Otherwise an error occurred.
 *
 *	@relates as_operations
 *	@ingroup as_operations_object
 */
bool as_operations_add_touch(as_operations * ops);

/******************************************************************************
 *	CDT FUNCTIONS
 *****************************************************************************/

//-----------------------------------------------------------------------------
// Add to list

/**
 * Add element to end of list.
 * @param val	Consumes a reference of this as_val.
 */
bool as_operations_add_list_append(as_operations *ops, const as_bin_name name, as_val *val);
bool as_operations_add_list_append_int64(as_operations *ops, const as_bin_name name, int64_t value);
bool as_operations_add_list_append_double(as_operations *ops, const as_bin_name name, double value);
bool as_operations_add_list_append_strp(as_operations *ops, const as_bin_name name, const char *value, bool free);
bool as_operations_add_list_append_rawp(as_operations *ops, const as_bin_name name, const uint8_t *value, uint32_t size, bool free);
/**
 * Add list of elements to end of list.
 * @param list	Consumes a reference of this as_val.
 */
bool as_operations_add_list_append_items(as_operations *ops, const as_bin_name name, as_list *list);
/**
 * Add element to list at index.
 * @param val	Consumes a reference of this as_val.
 */
bool as_operations_add_list_insert(as_operations *ops, const as_bin_name name, int64_t index, as_val *val);
bool as_operations_add_list_insert_int64(as_operations *ops, const as_bin_name name, int64_t index, int64_t value);
bool as_operations_add_list_insert_double(as_operations *ops, const as_bin_name name, int64_t index, double value);
bool as_operations_add_list_insert_strp(as_operations *ops, const as_bin_name name, int64_t index, const char *value, bool free);
bool as_operations_add_list_insert_rawp(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size, bool free);
/**
 * Add list of elements to list at index.
 * @param list	Consumes a reference of this as_val.
 */
bool as_operations_add_list_insert_items(as_operations *ops, const as_bin_name name, int64_t index, as_list *list);

//-----------------------------------------------------------------------------
// Remove from list

/**
 * Remove and get back a list element at index.
 */
bool as_operations_add_list_pop(as_operations *ops, const as_bin_name name, int64_t index);
/**
 * Remove and get back list elements at index.
 */
bool as_operations_add_list_pop_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);
/**
 * Remove a list element at index.
 */
bool as_operations_add_list_remove(as_operations *ops, const as_bin_name name, int64_t index);
/**
 * Remove list elements at index.
 */
bool as_operations_add_list_remove_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);

//-----------------------------------------------------------------------------
// Other list modifies

/**
 * Remove all elements from list.
 */
bool as_operations_add_list_clear(as_operations *ops, const as_bin_name name);
/**
 * Set element of list at index.
 * @param val	Consumes a reference of this as_val.
 */
bool as_operations_add_list_set(as_operations *ops, const as_bin_name name, int64_t index, as_val *val);
bool as_operations_add_list_set_int64(as_operations *ops, const as_bin_name name, int64_t index, int64_t value);
bool as_operations_add_list_set_double(as_operations *ops, const as_bin_name name, int64_t index, double value);
bool as_operations_add_list_set_strp(as_operations *ops, const as_bin_name name, int64_t index, const char *value, bool free);
bool as_operations_add_list_set_rawp(as_operations *ops, const as_bin_name name, int64_t index, const uint8_t *value, uint32_t size, bool free);
/**
 * Remove elements not within range(index, count).
 */
bool as_operations_add_list_trim(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);

//-----------------------------------------------------------------------------
// Read operations

/**
 * Get element of list at index.
 */
bool as_operations_add_list_get(as_operations *ops, const as_bin_name name, int64_t index);
/**
 * Get elements of list at index, get back a list of items.
 */
bool as_operations_add_list_get_range(as_operations *ops, const as_bin_name name, int64_t index, uint64_t count);
/**
 * Get elements of list from index to end of list, get back a list of items.
 */
bool as_operations_add_list_get_range_from(as_operations *ops, const as_bin_name name, int64_t index);
/**
 * Get element count of list.
 */
bool as_operations_add_list_size(as_operations *ops, const as_bin_name name);

#ifdef __cplusplus
} // end extern "C"
#endif
