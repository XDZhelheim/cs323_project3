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

string createVar()
{
    return "v" + to_string(vCounter++);
}

string createTemp()
{
    return "t" + to_string(tCounter++);
}

string createLabel()
{
    return "label" + to_string(lCounter++);
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
    string translateStmt(TreeNode *node)
    {
        if (node->child.size() == 2)
        {
            // Exp SEMI
            string tp = createTemp();
            return translateExp(node->child[0], tp);
        }
        else if (node->child.size() == 1)
        {
            // CompSt
            translateCompSt(node->child[0]);
        }
        else if (node->child.size() == 3)
        {
            // RETURN Exp SEMI
            string tp = createTemp();
            return translateExp(node->child[1], tp) + "RETURN " + tp+"\n";
        }
        else if (node->child.size() == 5)
        {
            if (node->child[0]->name == "IF") {
                // IF LP Exp RP Stmt & WHILE LP Exp RP Stmt
                string lb1=createLabel();
                string lb2=createLabel();
                string code1=translateCondExp(node->child[2], lb1, lb2)+"LABEL "+lb1+" :\n";
                string code2=translateStmt(node->child[4])+"LABEL "+lb2+" :\n";
                return code1+code2;
            }
            else {
                string lb1=createLabel();
                string lb2=createLabel();
                string lb3=createLabel();
                string code1="LABEL "+lb1+" :\n"+translateCondExp(node->child[2], lb2, lb3);
                string code2="LABEL "+lb2+" :\n"+translateStmt(node->child[4])+"GOTO "+lb1+"\n";
                return code1+code2+"LABEL "+lb3+" :\n";
            }
        }
        else
        {
            // IF LP Exp RP Stmt ELSE Stmt
            string lb1=createLabel();
            string lb2=createLabel();
            string lb3=createLabel();
            string code1=translateCondExp(node->child[2], lb1, lb2)+"LABEL "+lb1+" :\n";
            string code2=translateStmt(node->child[4])+"GOTO "+lb3+"LABEL "+lb2+" :\n";
            string code3=translateStmt(node->child[6])+"LABEL "+lb3+" :\n";
            return code1+code2+code3;
        }
        return NULL;
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
    string translateExp(TreeNode *node, string place)
    {
        if (node->child.size() == 1)
        {
            if (node->child[0]->type == DataType::INT_TYPE)
            {
                string name = createTemp();
                return name + " := #" + to_string(strtol(node->child[0]->data.c_str(), NULL, 0)) + "\n";
            }
            else
            {
                return place + " := " + node->child[0]->data;
            }
        }
        else if (node->child.size() == 2)
        {
            if (node->child[0]->name == "MINUS")
            {
                string name = createTemp();
                string code1 = translateExp(node->child[1], name);
                string code2 = place + " := #0 - " + name + "\n";
                return code1 + code2;
            }
        }
        else
        {
            if (node->child[1]->name == "ASSIGN")
            {
                string name = createTemp();
                string code1 = translateExp(node->child[2], name);
                string code2 = node->child[0]->name + " := " + name + "\n";
                string code3 = place + " := " + node->child[0]->name + "\n";
                return code1 + code2 + code3;
            }
            else if (node->child[1]->name == "PLUS")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = place + " := " + name1 + " + " + name2 + "\n";
                return code1 + code2 + code3;
            }
            else if (node->child[1]->name == "MINUS")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = place + " := " + name1 + " - " + name2 + "\n";
                return code1 + code2 + code3;
            }
            else if (node->child[1]->name == "MUL")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = place + " := " + name1 + " * " + name2 + "\n";
                return code1 + code2 + code3;
            }
            else if (node->child[1]->name == "DIV")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = place + " := " + name1 + " / " + name2 + "\n";
                return code1 + code2 + code3;
            }
        }
        string label1 = createLabel();
        string label2 = createLabel();
        string code0 = place + " := #0\n";
        string code1 = translateCondExp(node, label1, label2);
        string code2 = "LABEL " + label1 + " :\n" + place + " := #1\nLABEL " + label2 + " :\n";
        return code0 + code1 + code2;
    }

    string translateCondExp(TreeNode *node, string label_true, string label_false)
    {
        if (node->child.size() == 2)
        {
            return translateCondExp(node, label_false, label_true);
        }
        else
        {
            if (node->child[1]->name == "AND")
            {
                string label = createLabel();
                string code1 = translateCondExp(node->child[0], label, label_false) + "LABEL " + label + " :\n";
                string code2 = translateCondExp(node->child[2], label_true, label_false);
                return code1 + code2;
            }
            else if (node->child[1]->name == "OR")
            {
                string label = createLabel();
                string code1 = translateCondExp(node->child[0], label_true, label) + "LABEL " + label + " :\n";
                string code2 = translateCondExp(node->child[2], label_true, label_false);
                return code1 + code2;
            }
            else if (node->child[1]->name == "EQ")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = "IF " + name1 + " == " + name2 + " GOTO " + label_true + "\nGOTO " + label_false + "\n";
                return code1 + code2 + code3;
            }
            else if (node->child[1]->name == "NE")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = "IF " + name1 + " != " + name2 + " GOTO " + label_true + "\nGOTO " + label_false + "\n";
                return code1 + code2 + code3;
            }
            else if (node->child[1]->name == "GT")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = "IF " + name1 + " > " + name2 + " GOTO " + label_true + "\nGOTO " + label_false + "\n";
                return code1 + code2 + code3;
            }
            else if (node->child[1]->name == "GE")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = "IF " + name1 + " >= " + name2 + " GOTO " + label_true + "\nGOTO " + label_false + "\n";
                return code1 + code2 + code3;
            }
            else if (node->child[1]->name == "LT")
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = "IF " + name1 + " < " + name2 + " GOTO " + label_true + "\nGOTO " + label_false + "\n";
                return code1 + code2 + code3;
            }
            else
            {
                string name1 = createTemp();
                string name2 = createTemp();
                string code1 = translateExp(node->child[0], name1);
                string code2 = translateExp(node->child[2], name2);
                string code3 = "IF " + name1 + " <= " + name2 + " GOTO " + label_true + "\nGOTO " + label_false + "\n";
                return code1 + code2 + code3;
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