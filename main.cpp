#include <iostream>
#include "Tree.hpp"

int main()
{
    Tree<int> tree;


    tree.add(5);
    tree.add(3);
    tree.add(4);
    tree.add(1);
    tree.add(9);
    tree.add(7);
    tree.add(6);
    tree.add(8);
    tree.add(10);

    Tree<int> subTree = tree.findTree(7);

    std::cout << tree << std::endl;
    std::cout << subTree << std::endl;
    
    subTree.find(5);

    tree.del(5);

    std::cout << tree << std::endl;
    std::cout << subTree << std::endl;

    return 0;
}