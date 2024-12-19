#include <stdio.h>
#include <stdlib.h>

#include "record.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s FILE\n", argv[1]);
    return 1;
  }

  int            n;
  struct record* rs = read_records(argv[1], &n);

  if (!rs) {
    fprintf(stderr, "Failed to read records from %s\n", argv[1]);
    return 1;
  }
  while (1) {
    int i = rand() % n; 
    if (printf("%ld %ld\n", (long)rs[i].lon, (long)rs[i].lat) == 0) {
      break;
    }
  }
}
