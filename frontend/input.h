#ifndef INPUT_H
#define INPUT_H

#include "lexems.h"
#include "tree.h"
#include "stack.h"

#define debug_print(...)                                                                            \
do                                                                                                  \
{                                                                                                   \
    fprintf (stderr, __VA_ARGS__);                                                                  \
    fprintf (stderr, "In func %s, file %s, line %d.\n\n", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
}while (0)

void skip_spaces (char **str);
Lexem **lexer (char *text, Stack *lexems);
void print_lexems (Stack *lexems, FILE *output_file);


Node *tree_fill (Tree *tree, Stack *lexems);
Node *GetNodeG (Tree *tree, Stack *lexems);
Node *GetNodeDefs (Stack *lexems, int *index);
Node *GetNodeNfun (Stack *lexems, int *index);
Node *GetNodeBlock (Stack *lexems, int *index);
Node *GetNodeBranch (Stack *lexems, int *index);
Node *GetNodePar (Stack *lexems, int *index);
Node *GetNodeNvar (Stack *lexems, int *index);
Node *GetNodeAss (Stack *lexems, int *index);
Node *GetNodeSeq (Stack *lexems, int *index);

Node *GetNodeIf (Stack *lexems, int *index);
Node *GetNodeWhile (Stack *lexems, int *index);

Node *GetNodeCall (Stack *lexems, int *index);
Node *GetNodeRet (Stack *lexems, int *index);
Node *GetNodeArg (Stack *lexems, int *index);

Node *GetNodeE (Stack *lexems, int *index);
Node *GetNodeF (Stack *lexems, int *index);
Node *GetNodeT (Stack *lexems, int *index);
Node *GetNodeD (Stack *lexems, int *index);
Node *GetNodeP (Stack *lexems, int *index);
Node *GetNodeN (Stack *lexems, int *index);
Node *GetNodeV (Stack *lexems, int *index);

void swap_defs (Node *node);

#endif /* INPUT_H */
