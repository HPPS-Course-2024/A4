#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "coord_query.h"
#include "log.h"
#include "record.h"
#include <math.h>

double dist(double lon1, double lon2, double lat1, double lat2) {
  double sum = 0;
  sum += (lon1 - lon2) * (lon1 - lon2);
  sum += (lat1 - lat2) * (lat1 - lat2);
  return sqrt(sum);
}

struct naive_data {
  struct record* rs;
  int            n;
};

struct naive_data* mk_naive(struct record* rs, int n) {
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
}

const struct record* lookup_naive(struct naive_data* data, double lon_query,
                                  double lat_query) {
  if (!data) {
    LOG_ERROR("Attempted to look up in NULL naive_data");
    return NULL;
  }

  if (data->n < 1) {
    LOG_ERROR("Attempted to look up in empty naive_data");
    return NULL;
  }

  struct record* best      = data->rs;
  double         best_dist = dist(best->lon, lon_query, best->lat, lat_query);

  for (int i = 1; i < data->n; i++) {
    double dist_candidate =
        dist(data->rs[i].lon, lon_query, data->rs[i].lat, lat_query);

    if (dist_candidate < best_dist) {
      best      = data->rs + i;
      best_dist = dist_candidate;
    }
  }
  return best;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv, (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive, (lookup_fn)lookup_naive);
}
