#ifndef CODE_GENERTOR_HPP
#define CODE_GENERTOR_HPP

#include "NodeAnalyser.hpp"
#define DEBUG 0

void function_init()
{
    Type *int_type = new Type(Category::INT_VAL);

    Type *read = new Type(Category::FUNCTION, "read");
    read->returnType = int_type;
    symbolTable["read"] = read;

    Type *write = new Type(Category::FUNCTION, "write");
    write->returnType = int_type;
    write->varlist.push_back(int_type);
    symbolTable["write"] = write;
}

class Generator {
private:
    TreeNode *root;

public:
    Generator(TreeNode *node, string path)
    {
        root = node;
        out = ofstream(path);
    }

    void generate() {
        
    }

    /*
    Program: 
      ExtDefList 
    */
    void analyzeProgram(TreeNode *node)
    {
        if (DEBUG)
            cout << "Program" << endl;

        analyzeExtDefList(node->child[0]);
    }

    /*
    ExtDefList: 
      ExtDef ExtDefList
    | %empty
    */
    void analyzeExtDefList(TreeNode *node)
    {

    }

    /*
    ExtDef: 
      Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    */
    void analyzeExtDef(TreeNode *node)
    {

    }

    /*
    ExtDecList: 
      VarDec
    | VarDec COMMA ExtDecList
    */
    void analyzeExtDecList(TreeNode *node)
    {
    }

    /*
    Specifier: 
      TYPE
    | StructSpecifier
    */
    void analyzeSpecifier(TreeNode *node)
    {
    }

    /*
    StructSpecifier: 
      STRUCT ID LC DefList RC
    | STRUCT ID
    */
    void analyzeStructSpecifier(TreeNode *node)
    {
    }

    /*
    VarDec:
      ID
    | VarDec LB INT RB
    */
    void analyzeVarDec(TreeNode *node)
    {
    }

    /*
    FunDec: 
      ID LP VarList RP
    | ID LP RP
    */
    void analyzeFunDec(TreeNode *node)
    {
    }

    /*
    VarList: 
      ParamDec COMMA VarList
    | ParamDec
    */
    void analyzeVarList(TreeNode *node)
    {
    }

    /*
    ParamDec: 
      Specifier VarDec
    */
    void analyzeParamDec(TreeNode *node)
    {
    }

    /*
    CompSt: 
      LC DefList StmtList RC
    */
    void analyzeCompSt(TreeNode *node)
    {
    }

    /*
    StmtList: 
      Stmt StmtList
    | %empty
    */
    void analyzeStmtList(TreeNode *node)
    {
    }

    /*
    Stmt: 
      Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    */
    void analyzeStmt(TreeNode *node)
    {
    }

    /*
    DefList: 
      Def DefList
    | %empty
    */
    void analyzeDefList(TreeNode *node)
    {
    }

    /*
    Def: 
      Specifier DecList SEMI 
    */
    void analyzeDef(TreeNode *node)
    {
    }

    /*
    DecList: 
      Dec
    | Dec COMMA DecList
    */
    void analyzeDecList(TreeNode *node)
    {
    }

    /*
    Dec: 
      VarDec
    | VarDec ASSIGN Exp
    */
    void analyzeDec(TreeNode *node)
    {
    }

    /*
    Exp: 
      Exp ASSIGN Exp                type check
    | Exp AND Exp                   int, int -> int
    | Exp OR Exp                    int, int -> int
    | Exp LT Exp                    int, int -> int
    | Exp LE Exp                    int, int -> int
    | Exp GT Exp                    int, int -> int
    | Exp GE Exp                    int, int -> int
    | Exp NE Exp                    int, int -> int
    | Exp EQ Exp                    int, int -> int
    | Exp PLUS Exp                  T, T -> T
    | Exp MINUS Exp                 T, T -> T
    | Exp MUL Exp                   T, T -> T
    | Exp DIV Exp                   T, T -> T
    | LP Exp RP                     T -> T
    | MINUS Exp                     T -> -T
    | NOT Exp                       int -> int
    | ID LP Args RP                 func
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    | CHAR
    */
    void analyzeExp(TreeNode *node)
    {
    }

    /*
    Args: 
      Exp COMMA Args
    | Exp
    */
    void analyzeArgs(TreeNode *node)
    {
    }
};

void generateIntermidiateCode(char *file_path)
{
    string path = file_path;
    string p2_path = "/dev/stdout";
    string p3_path = "/dev/stdout";
    if (path.substr(path.length() - 4) == ".spl")
    {
        p2_path = path.substr(0, path.length() - 4) + ".out";
        p3_path = path.substr(0, path.length() - 4) + ".ir";
    }
    function_init();
    Analyser(root, p2_path).analyze();
    Generator(root, p3_path).generate();
}

#endif