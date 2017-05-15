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
  char *namespaces[7];
};

struct Container *Container_create(char *handle) {
  struct Container *c = malloc(sizeof(struct Container));
  assert(c != NULL);

  char *bundle_path = container_bundle_path(handle);

  c->handle = strdup(handle);
  c->pid = container_pid(bundle_path);
  c->namespaces[0] = Inode("cgroup", c->pid);
  c->namespaces[1] = Inode("ipc", c->pid);
  c->namespaces[2] = Inode("mnt", c->pid);
  c->namespaces[3] = Inode("net", c->pid);
  c->namespaces[4] = Inode("pid", c->pid);
  c->namespaces[5] = Inode("user", c->pid);
  c->namespaces[6] = Inode("uts", c->pid);

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
  printf("Handle: %s\n", c->handle);
  printf("PID: %d\n", c->pid);
  printf("Namespaces:\n");
  printf("\t%s\n", c->namespaces[0]);
  printf("\t%s\n", c->namespaces[1]);
  printf("\t%s\n", c->namespaces[2]);
  printf("\t%s\n", c->namespaces[3]);
  printf("\t%s\n", c->namespaces[4]);
  printf("\t%s\n", c->namespaces[5]);
  printf("\t%s\n", c->namespaces[6]);
}

void Container_destroy(struct Container *c) {
  assert(c != NULL);
  free(c->handle);
  free(c->namespaces[0]);
  free(c->namespaces[1]);
  free(c->namespaces[2]);
  free(c->namespaces[3]);
  free(c->namespaces[4]);
  free(c->namespaces[5]);
  free(c->namespaces[6]);
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
