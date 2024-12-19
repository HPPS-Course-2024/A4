
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

typedef struct {
  int64_t              osm_id;
  const struct record* record;
} id_record_pair_t;

typedef struct {
  id_record_pair_t* id_record_pairs;
  int               n;
} indexed_data_t;

indexed_data_t* mk_indexed(const struct record* rs, int n) {
  LOG_INFO("Creating indexed index with %d records", n);
  indexed_data_t* data = malloc(sizeof(indexed_data_t));
  if (!data) {
    LOG_ERROR("Failed to allocate memory for indexed_data: %s",
              strerror(errno));
    return NULL;
  }
  data->n               = n;
  data->id_record_pairs = malloc(n * sizeof(id_record_pair_t));

  if (!data->id_record_pairs) {
    LOG_ERROR("Failed to allocate memory for id_record_pairs: %s",
              strerror(errno));

    free(data);
    return NULL;
  }

  // Expensive on startup only but fast interms of lookup interactivity
  for (int i = 0; i < n; i++) {
    data->id_record_pairs[i].osm_id = rs[i].osm_id;
    data->id_record_pairs[i].record = rs + i;
  }

  LOG_INFO("Indexed index created successfully");
  return data;
}

void free_binsort(indexed_data_t* data) {
  if (!data) {
    LOG_ERROR("Attempted to free NULL indexed_data");
    return;
  }
  free(data->id_record_pairs);
  free(data);
  LOG_INFO("Indexed index freed successfully");
}

const struct record* lookup_binsort(indexed_data_t* data, int64_t needle) {
  for (int i = 0; i < data->n; i++) {
    if (data->id_record_pairs[i].osm_id == needle) {
      return data->id_record_pairs->record + i;
    }
  }
  return NULL;
}

int main(int argc, char** argv) {
  return id_query_loop(argc, argv, (mk_index_fn)mk_indexed,
                       (free_index_fn)free_binsort, (lookup_fn)lookup_binsort);
}
