#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char* argv[]) {
  if (argc == 1) {
    DIR* d = opendir(".");
    if (d == NULL) {
      return -1;
    } else {
      struct dirent* dir = readdir(d);
      while (dir != NULL) {
        printf("%s\n", dir->d_name);
        dir = readdir(d);
      }
    }
  }

  return 0;
}
