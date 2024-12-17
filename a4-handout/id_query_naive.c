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

struct naive_data {
  struct record* r<s;
  int            n;
};

struct naive_data* mk_naive(struct record* rs, int n) {
  LOG_INFO("Creating naive index with %d records", n);
  struct naive_data* data = malloc(sizeof(struct naive_data));
  if (!data) {
    LOG_ERROR("Failed to allocate memory for naive_data: %s", strerror(errno));
    return NULL;
  }
  data->rs = rs;
  data->n  = n;
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
  for (int i = 0; i < data->n; i++) {
    if (data->rs[i].osm_id == needle) {
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
