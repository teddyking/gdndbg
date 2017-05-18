typedef struct Namespace Namespace;

struct Namespace {
  char *name;
  unsigned long long inode;
};

struct Namespace *Namespace_create(char *name, int pid);
void Namespace_destroy(struct Namespace *n);

char *Inode(char *ns, int pid);
