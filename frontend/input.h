#ifndef INPUT_H
#define INPUT_H

#include "lexems.h"
#include "tree.h"

#define debug_print(...)                                                                            \
do                                                                                                  \
{                                                                                                   \
    printf (__VA_ARGS__);                                                                           \
    fprintf (stderr, ", func %s in file %s, line %d.\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);  \
}while (0)

void skip_spaces (char **str);
Lexem **lexer (char *text, Lexem **lexems);
void print_lexems (Lexem **lexems, FILE *output_file);
void free_lexems (Lexem **lexems);


Node *tree_fill (Tree *tree, Lexem **lexems);
Node *GetNodeG (Tree *tree, Lexem **lexems);
Node *GetNodeDefs (Lexem **lexems, int *index);
Node *GetNodeNfun (Lexem **lexems, int *index);
Node *GetNodeBlock (Lexem **lexems, int *index);
Node *GetNodePar (Lexem **lexems, int *index);
Node *GetNodeNvar (Lexem **lexems, int *index);
Node *GetNodeAss (Lexem **lexems, int *index);
Node *GetNodeSeq (Lexem **lexems, int *index);

Node *GetNodeIf (Lexem **lexems, int *index);
Node *GetNodeWhile (Lexem **lexems, int *index);

Node *GetNodeCall (Lexem **lexems, int *index);
Node *GetNodeRet (Lexem **lexems, int *index);
Node *GetNodeArg (Lexem **lexems, int *index);

Node *GetNodeE (Lexem **lexems, int *index);
Node *GetNodeT (Lexem **lexems, int *index);
Node *GetNodeD (Lexem **lexems, int *index);
Node *GetNodeP (Lexem **lexems, int *index);
Node *GetNodeN (Lexem **lexems, int *index);
Node *GetNodeV (Lexem **lexems, int *index);

#endif /* INPUT_H */
