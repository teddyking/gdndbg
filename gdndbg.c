#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "gdndbg.h"

static const char *DEPOT_DIR_PATH = "/var/vcap/data/garden/depot";
static const int MAX_CONTAINERS = 250;

struct Container {
  char *handle;
  int pid;
  char *mnt_inode;
};

struct Container *Container_create(char *handle) {
  struct Container *c = malloc(sizeof(struct Container));
  assert(c != NULL);

  char *bundle_path = container_bundle_path(handle);

  c->handle = strdup(handle);
  c->pid = container_pid(bundle_path);
  c->mnt_inode = Inode("mnt", c->pid);

  free(bundle_path);

  return c;
}

char *Inode(char *ns, int pid) {
  char readlink_ns_buf[24] = "";
  char ns_link_path[24] = "";
  ssize_t len = 0;

  sprintf(ns_link_path, "/proc/%d/ns/%s", pid, ns);

  if ((len = readlink(ns_link_path, readlink_ns_buf, sizeof(readlink_ns_buf)-1)) > 0) {
    readlink_ns_buf[len] = '\0';

    return strdup(readlink_ns_buf);
  }

  return strdup("N/A");
}

void Container_show(struct Container *c) {
  printf(
    "%s - %d - '%s'\n", c->handle, c->pid, c->mnt_inode
  );
}

void Container_destroy(struct Container *c) {
  assert(c != NULL);
  free(c->handle);
  free(c->mnt_inode);
  free(c);
}

void print_usage(char *argv[]) {
  printf("\nUsage %s <command>\n\n", argv[0]);
  printf("Available commands:\n");
  printf("\tcontainers, c\n");
  printf("\thelp, h\n");

  exit(1);
}

void Containers() {
  DIR *depot;
  struct dirent *dir;
  int container_count = 0;
  struct Container *containers[MAX_CONTAINERS];

  depot = opendir(DEPOT_DIR_PATH);

  if (depot) {
    while ((dir = readdir(depot)) != NULL) {
      char *dir_name = dir->d_name;

      if (is_container_dir(dir_name)) {
        if (container_count < MAX_CONTAINERS) {
          struct Container *c = Container_create(dir_name);

          containers[container_count] = c;
          container_count++;
        }
      }
    }

    printf("container count: %d\n\n", container_count);
    for (int i = 0; i < container_count; i++) {
      Container_show(containers[i]);
      Container_destroy(containers[i]);
    }

    closedir(depot);
  }
}

int is_container_dir(const char *dir_name) {
  int ret;

  ret = strcmp(dir_name, ".");
  if (ret != 0) {
    ret = strcmp(dir_name, "..");
    if (ret != 0) {
      if (dir_name[0] != '.') {
        return 1;
      }
    }
  }

  return 0;
}

int container_pid(const char *bundle_path) {
  char pidfile_path[strlen(bundle_path) + 9];
  strcpy(pidfile_path, bundle_path);
  strcat(pidfile_path, "/");
  strcat(pidfile_path, "pidfile");

  int pid = 0;
  FILE *pidfile;
  pidfile = fopen(pidfile_path, "r");
  if (pidfile != NULL) {
    fscanf(pidfile, "%d", &pid);
    fclose(pidfile);
  }

  return pid;
}

char *container_bundle_path(const char *handle) {
  char *bundle_path = malloc(strlen(DEPOT_DIR_PATH) + strlen(handle) + 2);
  strcpy(bundle_path, DEPOT_DIR_PATH);
  strcat(bundle_path, "/");
  strcat(bundle_path, handle);

  return bundle_path;
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    print_usage(argv);

  char command = argv[1][0];
  switch (command) {
    case 'c':
      Containers();
      exit(0);
    case 'h':
      print_usage(argv);
    default:
      print_usage(argv);
  }
}
