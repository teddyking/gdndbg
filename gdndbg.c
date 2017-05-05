#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *DEPOT_DIR_PATH = "/var/vcap/data/garden/depot";

void print_usage(char *argv[]) {
  printf("\nUsage %s <command>\n\n", argv[0]);
  printf("Available commands:\n");
  printf("\tcontainers, c\n");
  printf("\thelp, h\n");

  exit(1);
}

void containers() {
  DIR *depot;
  struct dirent *dir;

  depot = opendir(DEPOT_DIR_PATH);

  if (depot) {
    while ((dir = readdir(depot)) != NULL) {
      char *dir_name = dir->d_name;
      int ret;

      ret = strcmp(dir_name, ".");
      if (ret != 0)
        ret = strcmp(dir_name, "..");
          if (ret != 0)
            if (dir_name[0] != '.')
              printf("%s\n", dir_name);
    }

    closedir(depot);
  }
}

int is_directory(const char *path) {
  struct stat path_stat;
  stat(path, &path_stat);
  return  S_ISDIR(path_stat.st_mode);
}

int main(int argc, char *argv[]) {
  printf("=== gdndbg ===\n");

  if (argc < 2)
    print_usage(argv);

  char command = argv[1][0];
  switch (command) {
    case 'c':
      containers();
      exit(0);
    case 'h':
      print_usage(argv);
    default:
      print_usage(argv);
  }
}
