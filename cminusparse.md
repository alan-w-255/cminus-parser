## c-minus EBNF

1. program -> declaration_list

2. declaration_list -> {declaration}

1. declaration -> var_declaration | fun_declaration
2. var_declaration -> type_specifier ID | type_specifier ID[NUM]

<!-- > declaration -> type_specifier ID | type_specifier ID [NUM] | type_specifier ID (params) compound_stmt -->

3. declaration -> type_specifier ID declaration2

4. declaration2 -> [NUM] | (params) compound_stmt | #

5. type_specifier -> INT | VOID

6. params -> params_list | VOID

7. params_list -> param {, param}

8. param -> type_specifier ID param2

1. param2 -> [] | #
2. compound_stmt -> {local_declarations statement_list }
3. local_declarations -> {var_declaration}
4. statement_list -> {statement}

1. statement -> expression_stmt | compound_stmt | selection_stmt | iteration_stmt | return_stmt
2. expression_stmt -> expression; | ;
3. selection_stmt -> IF (expression) statement selection_stmt2
4. selection_stmt2 -> # | ELSE statement
5. iteration_stmt -> WHILE(expression) statement
6. return_stmt -> RETURN return_stmt2
7. return_stmt2 -> # | expression
8. expression -> var = expression | simple_expression // note simple_expression 有可能以ID 开始
9. var -> ID | ID[expression]
10. simple_expression -> additive_expression {relop additive_expression}
11. relop -> LE | LT | GT | GE | EQ | NE

13. additive_expression -> term {addop term}
14. addop -> + | -
15. term -> factor {mulop factor}
16. mulop -> * | /
17. factor -> (expression) | NUM | ID factor2//id
18. factor2 -> # | [expression] | (args)
19. args -> arg_list | empty
20. arg_list -> expression{ , expression}



