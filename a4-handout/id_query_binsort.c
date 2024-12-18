
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

int cmp_record_pairs(const void* a, const void* b) {
  id_record_pair_t rp_a = *(id_record_pair_t*)a;
  id_record_pair_t rp_b = *(id_record_pair_t*)b;
  if (rp_a.osm_id < rp_b.osm_id) {
    return -1;
  } else if (rp_a.osm_id > rp_b.osm_id) {
    return 1;
  } else {
    return 0;
  }
}

// int cmp_record_pairs(const void* a, const void* b) {
//   return (*(id_record_pair_t*)a).osm_id - (*(id_record_pair_t*)b).osm_id;
// }

indexed_data_t* mk_binsort(const struct record* rs, int n) {
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

  for (int i = 0; i < n; i++) {
    data->id_record_pairs[i].osm_id = rs[i].osm_id;
    data->id_record_pairs[i].record = rs + i;
  }

  qsort(data->id_record_pairs, n, sizeof(id_record_pair_t), &cmp_record_pairs);

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

id_record_pair_t* binary_search(id_record_pair_t* pairs, int n,
                                int64_t needle) {
  int l = 0, r = n - 1;
  while (l <= r) {
    int m = l + (r - l) / 2;
    if (pairs[m].osm_id < needle)
      l = m + 1;
    else if (pairs[m].osm_id > needle)
      r = m - 1;
    else
      return pairs + m;
  }
  return NULL;
}

const struct record* lookup_binsort(indexed_data_t* data, int64_t needle) {
  id_record_pair_t* id_record_pair =
      binary_search(data->id_record_pairs, data->n, needle);
  if (!id_record_pair) {
    return NULL;
  }
  return id_record_pair->record;
}

int main(int argc, char** argv) {
  return id_query_loop(argc, argv, (mk_index_fn)mk_binsort,
                       (free_index_fn)free_binsort, (lookup_fn)lookup_binsort);
}
