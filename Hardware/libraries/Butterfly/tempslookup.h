#ifndef tempslookup_h
#define tempslookup_h

// These are defined in a .c file so that the compiler won't generate
// warning "only initialized variables can be placed into program memory area"
extern prog_uint16_t TEMP_Fahrenheit_pos[];
extern prog_uint16_t TEMP_Celsius_pos[];
extern prog_uint16_t TEMP_Celsius_neg[];

#endif