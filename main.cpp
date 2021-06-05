#include "globals.h"
#include "util.h"
#include "parse.h"
#include "analyze.h"
#include "execution.h"
#include <cstring>

int lineno = 0;
FILE *source;
FILE *listing;

int EchoSource = false;
int TraceScan = false;
int TraceParse = true;
int TraceAnalyze = true;
int TraceCode = false;
int Error = false;

int main(int argc, char *argv[]) {
  TreeNode *syntaxTree;
  char pgm[120]; /* source code file name */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    exit(1);
  }
  strcpy(pgm, argv[1]);
  source = fopen(pgm, "r");
  if (source == NULL) {
    fprintf(stderr, "File %s not found\n", pgm);
    exit(1);
  }
  listing = stdout;
  fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);

  syntaxTree = parse();
  if (TraceParse) {
    fprintf(listing, "\nSyntax tree:\n");
    printTree(syntaxTree);
  }

  if (!Error) {
    if (TraceAnalyze)
      fprintf(listing, "\nBuilding Symbol Table...\n");
    buildSymtab(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nChecking Types...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze)
      fprintf(listing, "\nType Checking Finished\n");
  }
  if (!Error) {
    execution(syntaxTree);
  }
  return 0;
}