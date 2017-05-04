#include <stdio.h>
#include <stdlib.h>

void print_usage(char *argv[]) {
  printf("\nUsage %s <command>\n\n", argv[0]);
  printf("Available commands:\n");
  printf("\tcontainers, c\n");
  printf("\thelp, h\n");

  exit(1);
}

int main(int argc, char *argv[]) {
  printf("=== gdndbg ===\n");

  if (argc < 2)
    print_usage(argv);

  char command = argv[1][0];
  switch (command) {
    case 'c':
      printf("TODO: implement containers command\n");
      exit(0);
    case 'h':
      print_usage(argv);
    default:
      print_usage(argv);
  }
}
