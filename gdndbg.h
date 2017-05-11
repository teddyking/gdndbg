void Containers();

struct Container *Container_create(char *handle);
void Container_show(struct Container *c);
void Container_destroy(struct Container *c);

void print_usage(char *argv[]);
int is_directory(const char *path);
int container_pid(const char *bundle_path);
char *container_bundle_path(const char *handle);
