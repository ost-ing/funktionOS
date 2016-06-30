#include "utilities.h"

// Converts an array of bytes and converts them to an unsigned int
uint utilities_chararr_to_uint(uchar arr[])
{
	return ((uint)arr[0]
	     | ((uint)arr[1]) << 8
		 | ((uint)arr[2]) << 16
		 | ((uint)arr[3]) << 24);
}

