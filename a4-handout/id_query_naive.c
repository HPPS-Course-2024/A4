#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "id_query.h"
#include "log.h"
#include "record.h"

// This struct is allocated memory on the heap for an array of records and an
// integer. When allocating memory for a struct naive_data instance using
// malloc, it allocates sizeof(struct record*) + sizeof(int) bytes on the
// heap. On a 32-bit system, this would be 4 (pointer) + 4 (int) = 8 bytes. On
// a 64-bit system, this would be 8 (pointer) + 4 (int) = 12 bytes.

//------------------------

// naive_data in memory
// Mem[0-7]: rs pointer         ----> Mem[0-199][200, 399] ... n gange
// Mem[8-11]: n

struct naive_data {
  struct record* rs;
  int            n;
};

struct naive_data* mk_naive(struct record* rs, int n) {
  LOG_INFO("Creating naive index with %d records", n);

  struct naive_data* data = malloc(sizeof(struct naive_data));
  if (!data) {
    LOG_ERROR("Failed to allocate memory for naive_data: %s", strerror(errno));
    return NULL;
  }
  // *(struct record**)((char*)data + 0) = rs;
  data->rs = rs;
  // *(int*)((char*)data + 8)            = n;
  data->n = n;
  LOG_INFO("Naive index created successfully");
  return data;
}

void free_naive(struct naive_data* data) {
  if (!data) {
    LOG_ERROR("Attempted to free NULL naive_data");
    return;
  }
  free(data);
  LOG_INFO("Naive index freed successfully");
}
const struct record* lookup_naive(struct naive_data* data, int64_t needle) {
  struct record* rs = data->rs;
  for (int i = 0; i < data->n; i++) {
    if (rs[i].osm_id == needle) {
      LOG_INFO("Record found in naive index");
      return data->rs + i;
    }
  }
  LOG_INFO("Record not found in naive index");
  return NULL;
}

int main(int argc, char** argv) {
  return id_query_loop(argc, argv, (mk_index_fn)mk_naive,
                       (free_index_fn)free_naive, (lookup_fn)lookup_naive);
}
