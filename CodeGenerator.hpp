#ifndef CODE_GENERTOR_HPP
#define CODE_GENERTOR_HPP

#include "NodeAnalyser.hpp"

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
};

void generateIntermidiateCode(char *file_path)
{
    string path = file_path;
    string out_path = "/dev/stdout";
    if (path.substr(path.length() - 4) == ".spl")
    {
        out_path = path.substr(0, path.length() - 4) + ".ir";
    }
    Analyser(root, out_path).analyze();
    Generator(root, out_path).generate();
}

#endif