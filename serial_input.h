// if there is data pending, it will fill the data array
// with as much data is available
// the return value is the number of bytes written to the data array
// data should be 32 byte in size
// if computer has sent more than 32 bytes after last call, the extra data has been dropped
int serial_receiveData(uint8_t * data);

// returns a string if the computer has sent one
// returns 0 if no string is available
// returns 1 if there is a string and it has been copied to the str array
// Don't use this if you expect data rather than a string, otherwise this will scramble the data
// The strings that the PC sends must be below 32 bytes
int serial_getString(uint8_t * str);
