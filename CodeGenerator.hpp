#ifndef CODE_GENERTOR_HPP
#define CODE_GENERTOR_HPP

#include "NodeAnalyser.hpp"
#define DEBUG 0

int tCounter = 0;
int vCounter = 0;
int lCounter = 0;

using std::endl;
using std::map;
using std::string;
using std::to_string;

map<string, string> vmap;

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

class Generator
{
private:
    TreeNode *root;

public:
    Generator(TreeNode *node, string path)
    {
        root = node;
        out = ofstream(path);
    }

    void generate()
    {
        translateProgram(root);
    }

    /*
    Program: 
      ExtDefList 
    */
    void translateProgram(TreeNode *node)
    {
        translateExtDefList(node->child[0]);
    }

    /*
    ExtDefList: 
      ExtDef ExtDefList
    | %empty
    */
    void translateExtDefList(TreeNode *node)
    {
        if (node->child.empty())
        {
            return;
        }
        translateExtDef(node->child[0]);
        translateExtDefList(node->child[1]);
    }

    /*
    ExtDef: 
      Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    */
    void translateExtDef(TreeNode *node)
    {
        if (node->child.size() == 2)
        {
            // Specifier SEMI
            translateSpecifier(node->child[0]);
        }
        else
        {
            if (node->child[2]->child.empty())
            {
                // Specifier ExtDecList SEMI
                translateExtDecList(node->child[1]);
            }
            else
            {
                translateFunDec(node->child[1]);
                translateCompSt(node->child[2]);
            }
        }
    }

    /*
    ExtDecList: 
      VarDec
    | VarDec COMMA ExtDecList
    */
    void translateExtDecList(TreeNode *node)
    {
        translateVarDec(node->child[0]);

        if (node->child.size() == 3)
        {
            // VarDec COMMA ExtDecList
            translateExtDecList(node->child[2]);
        }
    }

    /*
    Specifier: 
      TYPE
    | StructSpecifier
    */
    void translateSpecifier(TreeNode *node)
    {
        if (node->child[0]->child.empty())
        {
            return;
        }
        else
        {
            // StructSpecifier
            translateStructSpecifier(node->child[0]);
        }
    }

    /*
    StructSpecifier: 
      STRUCT ID LC DefList RC
    | STRUCT ID
    */
    void translateStructSpecifier(TreeNode *node)
    {
        //! not implemented
        if (node->child.size() == 5)
        {
            // STRUCT ID LC DefList RC
            translateDefList(node->child[3]);
        }
    }

    /*
    VarDec:
      ID
    | VarDec LB INT RB
    */
    void translateVarDec(TreeNode *node)
    {
        if (node->child.size() == 4)
        {
            //! not implemented
            translateVarDec(node->child[0]);
        }
        else
        {
            vmap[node->child[0]->name]="v"+to_string(vCounter++);
        }
    }

    /*
    FunDec: 
      ID LP VarList RP
    | ID LP RP
    */
    void translateFunDec(TreeNode *node)
    {
        // TODO

        if (node->child.size() == 4)
        {
            // ID LP VarList RP
            translateVarList(node->child[2]);
        }
    }

    /*
    VarList: 
      ParamDec COMMA VarList
    | ParamDec
    */
    void translateVarList(TreeNode *node)
    {
        translateParamDec(node->child[0]);
        if (node->child.size() == 3)
        {
            // ParamDec COMMA VarList
            translateVarList(node->child[2]);
        }
    }

    /*
    ParamDec: 
      Specifier VarDec
    */
    void translateParamDec(TreeNode *node)
    {
        translateSpecifier(node->child[0]);
        translateVarDec(node->child[1]);
    }

    /*
    CompSt: 
      LC DefList StmtList RC
    */
    void translateCompSt(TreeNode *node)
    {
        translateDefList(node->child[1]);
        translateStmtList(node->child[2]);
    }

    /*
    StmtList: 
      Stmt StmtList
    | %empty
    */
    void translateStmtList(TreeNode *node)
    {
        if (node->child.empty())
        {
            return;
        }
        else
        {
            // Stmt StmtList
            translateStmt(node->child[0]);
            translateStmtList(node->child[1]);
        }
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
    void translateStmt(TreeNode *node)
    {
        if (node->child.size() == 2)
        {
            // Exp SEMI
            translateExp(node->child[0]);
        }
        else if (node->child.size() == 1)
        {
            // CompSt
            translateCompSt(node->child[0]);
        }
        else if (node->child.size() == 3)
        {
            // RETURN Exp SEMI
            // TODO
            translateExp(node->child[1]);
        }
        else if (node->child.size() == 5)
        {
            // IF LP Exp RP Stmt & WHILE LP Exp RP Stmt
            translateExp(node->child[2]);
            translateStmt(node->child[4]);
        }
        else
        {
            // IF LP Exp RP Stmt ELSE Stmt
            translateExp(node->child[2]);
            translateStmt(node->child[4]);
            translateStmt(node->child[6]);
        }
    }

    /*
    DefList: 
      Def DefList
    | %empty
    */
    void translateDefList(TreeNode *node)
    {
        if (node->child.size() == 2)
        {
            // Def DefList
            translateDef(node->child[0]);
            translateDefList(node->child[1]);
        }
    }

    /*
    Def: 
      Specifier DecList SEMI 
    */
    void translateDef(TreeNode *node)
    {
        translateSpecifier(node->child[0]);
        translateDecList(node->child[1]);
    }

    /*
    DecList: 
      Dec
    | Dec COMMA DecList
    */
    void translateDecList(TreeNode *node)
    {
        translateDec(node->child[0]);
        if (node->child.size() == 3)
        {
            translateDecList(node->child[2]);
        }
    }

    /*
    Dec: 
      VarDec
    | VarDec ASSIGN Exp
    */
    void translateDec(TreeNode *node)
    {
        translateVarDec(node->child[0]);
        if (node->child.size() == 3)
        {
            translateExp(node->child[2]);
        }
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
    string translateExp(TreeNode *node)
    {
        if (node->child.size() == 1)
        {
            if (node->child[0]->type == DataType::INT_TYPE)
            {
                string name = "t" + to_string(tCounter++);
                out << name << " := #" << to_string(strtol(node->child[0]->data.c_str(), NULL, 0)) << endl;
                return name;
            }
            else
            {
                return node->child[0]->data;
            }
        }
        else if (node->child.size() == 2)
        {
            if (node->child[0]->name == "MINUS")
            {
                string name = "t" + to_string(tCounter++);
                out << name << " := #0 - " << translateExp(node->child[0]) << endl;
                return name;
            }
            else
            {
            }
        }
    }

    /*
    Args: 
      Exp COMMA Args
    | Exp
    */
    void translateArgs(TreeNode *node)
    {
        translateExp(node->child[0]);

        if (node->child.size() == 3)
        {
            // Exp COMMA Args
            translateArgs(node->child[2]);
        }
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