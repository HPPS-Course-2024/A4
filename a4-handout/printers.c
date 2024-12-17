#include "printers.h"
#include <assert.h>

// ====== int8 conversions ======
void to_int8_base10(void* value, char* buffer, size_t buffer_size) {
  snprintf(buffer, buffer_size, "%d", *(int8_t*)value);
}

// ====== uint8 conversions ======
void to_uint8_base10(void* value, char* buffer, size_t buffer_size) {
  snprintf(buffer, buffer_size, "%u", *(uint8_t*)value);
}

void uint8_to_base2(void* value, char* buffer, size_t buffer_size) {
  assert(buffer_size >= 9);
  uint8_t v = *(uint8_t*)value;
  for (int i = 0; i < 8; i++) {
    buffer[i] = (v >> (7 - i)) & 1 ? '1' : '0';
  }
  buffer[8] = '\0';
}

// ====== bits8 conversions ======
void bits8_to_base2(void* value, char* buffer, size_t buffer_size) {
  assert(buffer_size >= 9);
  struct bits8* bits = (struct bits8*)value;
  bits8_to_ascii(*bits, buffer);
}

void bits8_to_base10(void* value, char* buffer, size_t buffer_size) {
  assert(buffer_size >= 9);
  struct bits8* bits = (struct bits8*)value;
  snprintf(buffer, buffer_size, "%u", bits8_to_int(*bits));
}

void bits8_to_base10_signed(void* value, char* buffer, size_t buffer_size) {
  assert(buffer_size >= 9);
  struct bits8* bits = (struct bits8*)value;
  snprintf(buffer, buffer_size, "%d", (int8_t)bits8_to_int(*bits));
}
