// DEF_OP (name, num, sign, sign len, sign for gnuplot, tex_pre_sign, tex_left_cond, tex_mid_sign, tex_right_cond, tex_end_sign)

DEF_OP (ADD, 1, +, 1, +, "", 0, "+", 0, "" )
DEF_OP (SUB, 2, -, 1, -, "", 0, "-", node->right->type == TYPE_OP, "")
DEF_OP (MUL, 3, *, 1, *, "", (node->left->type == TYPE_OP) &&
                             (node->left->value.op_val == OP_ADD ||
                              node->left->value.op_val == OP_SUB),
                             "\\cdot",
                             (node->right->type == TYPE_OP) &&
                              (node->right->value.op_val == OP_ADD ||
                               node->right->value.op_val == OP_SUB), "")
DEF_OP (DIV, 4, /, 1, /, "\\frac{", 0, "}{", 0, "}")
DEF_OP (DEG, 5, ^, 2, **, "", node->left->type == TYPE_OP, "^{", 0, "}")
DEF_OP (GT, 6, gt, 1, >, "", node->left->type == TYPE_OP, ">", 0, "")
DEF_OP (LT, 7, lt, 1, <, "", node->left->type == TYPE_OP, "<", 0, "")
DEF_OP (LEQ, 8, leq, 2, <=, "", node->left->type == TYPE_OP, "<=", 0, "")
DEF_OP (GEQ, 9, geq, 2, >=, "", node->left->type == TYPE_OP, ">=", 0, "")
DEF_OP (EQ, 10, eq, 2, ==, "", node->left->type == TYPE_OP, "==", 0, "")
DEF_OP (NEQ, 11, neq, 2, !=, "", node->left->type == TYPE_OP, "!=", 0, "")
DEF_OP (AND, 12, and, 2, &&, "", node->left->type == TYPE_OP, "&&", 0, "")
DEF_OP (OR, 13, or, 2, ||, "", node->left->type == TYPE_OP, "||", 0, "")
DEF_OP (NEG, 14, neg, 1, -, "", node->left->type == TYPE_OP, "-", 0, "")

//DEF_OP (SIN, 6, sin, 3, sin, "\\sin(", 0, "", 0, ")")
//DEF_OP (COS, 7, cos, 3, cos, "\\cos(", 0, "", 0, ")")
//DEF_OP (LN, 8, ln, 2, ln,    "\\ln(", 0, "", 0, ")")
