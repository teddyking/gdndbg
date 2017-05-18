#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "namespace.h"

Namespace *Namespace_create(char *name, int pid) {
  Namespace *n = malloc(sizeof(Namespace));

  n->name = strdup(name);
  n->inode = 0;

  if (pid > 0) {
    char *inode = Inode(name, pid);
    char *inode_stripped = strdup(&inode[strlen(name) + 2]);
    inode_stripped[(strlen(inode_stripped)-1)] = '\0';
    n->inode = strtoull(inode_stripped, NULL, 10);
    free(inode);
    free(inode_stripped);
  }

  return n;
}

void Namespace_destroy(Namespace *n) {
  assert(n != NULL);
  free(n->name);
  free(n);
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
