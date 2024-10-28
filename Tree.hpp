#ifndef TREE_HPP
#define TREE_HPP

#include <mutex>
#include <exception>
#include <initializer_list>
#include <ostream>
#include <functional>


template<class T>
class Tree
{
public:
    Tree() noexcept;
    Tree(const Tree& tree);
    Tree(Tree&& tree) noexcept;
    Tree(const std::initializer_list<T>& list);

    void add(T t);
    Tree& operator<<(T t);
    bool find(T t) const noexcept;
    Tree findTree(T t) const;

    template<class O>
    friend std::ostream& operator<<(std::ostream& os, Tree<O>& tree);

    void lrR() const noexcept;
    void Rlr() const noexcept;

    void del(T t) noexcept;

    void clear() noexcept;

    ~Tree() noexcept; //исключения/*

private:
    struct Node
    {
        bool isLeaf() const
        {
            return left == nullptr && right == nullptr;
        }

        Node* left = nullptr, * right = nullptr;

        T value;
    };

    Tree makeTree(Node* root) const;

    void recCopy(Node* from, Node* to) const;

    Node* _root;

    mutable std::mutex _mutex;
};
template<class T>
Tree<T>::Tree() noexcept : _root(nullptr)
{

}

template<class T>
Tree<T>::Tree(const Tree& tree) : _root(nullptr)
{
    if (tree._root != nullptr)
    {
        _root = new Node;
        _root->value = tree._root->value;
        recCopy(tree._root, _root);
    }
}
template<class T>
Tree<T>::Tree(Tree&& tree) noexcept : _root(tree._root)
{
    tree._root = nullptr;
}
template<class T>
Tree<T>::Tree(const std::initializer_list<T>& list)
{
    for (const auto& item : list)
    {
        add(item);
    }
}
template<class T>
void Tree<T>::recCopy(Node* from, Node* to) const{
    if (from->left != nullptr)
    {
        to->left = new Node;
        to->left->value = from->left->value;

        recCopy(from->left, to->left);
    }
    if (from->right != nullptr)
    {
        to->right = new Node;
        to->right->value = from->right->value;

        recCopy(from->right, to->right);
    }
}
template<class T>
void Tree<T>::add(T t)
{
    std::function<void(Node*, T)> recAdd = [&recAdd](Node* node, T t)
    {
        if (node->value == t)
        {
            return;
        }

        if (t < node->value)
        {
            if (node->left != nullptr)
            {
                recAdd(node->left, t);
            }
            else
            {
                node->left = new Node;
                node->left->value = t;
            }
        }
        else
        {
            if (node->right != nullptr)
            {
                recAdd(node->right, t);
            }
            else
            {
                node->right = new Node;
                node->right->value = t;
            }
        }
    };

    _mutex.lock();

    if (_root == nullptr)
    {
        _root = new Node;
        _root->value = t;
    }
    else
    {
        recAdd(_root, t);
    }

    _mutex.unlock();
}
template<class T>
Tree<T>& Tree<T>::operator<<(T t)
{
    add(t);

    return *this;
}
template<class T>
bool Tree<T>::find(T t) const noexcept
{
    std::function<bool(Node*, T t)> recFind = [&recFind](Node* node, T t)
    {
        if (node == nullptr)
        {
            return false;
        }
        else if (node->value == t)
        {
            return true;
        }
        else if (t < node->value)
        {
            recFind(node->left, t);
        }
        else
        {
            recFind(node->right, t);
        }
    };

    _mutex.lock();

    auto res = recFind(_root, t);

    _mutex.unlock();

    return res;
}
template<class T>
Tree<T> Tree<T>::findTree(T t) const
{
    std::function<Node* (Node*, T t)> recFind = [&recFind](Node* node, T t)
    {
        if (node == nullptr || node->value == t)
        {
            return node;
        }
        else if (t < node->value)
        {
            return recFind(node->left, t);
        }
        else
        {
            return recFind(node->right, t);
        }
    };

    _mutex.lock();

    auto tree = makeTree(recFind(_root, t));

    _mutex.unlock();

    return tree;
}
template<class T>
Tree<T> Tree<T>::makeTree(Tree::Node* root) const
{
    Tree tree;
    tree._root = root;
    Tree treeforCopy(tree);
    tree._root = nullptr;
    return { treeforCopy };
}
template<class O>
std::ostream& operator<<(std::ostream& os, Tree<O>& tree)
{
    std::function<void(typename Tree<O>::Node*, std::ostream&)> recOut = [&recOut](typename Tree<O>::Node* node, std::ostream& os)
    {
        if (node != nullptr)
        {
            recOut(node->left, os);
            os << node->value << ' ';
            recOut(node->right, os);
        }
    };

    tree._mutex.lock();

    recOut(tree._root, os);

    tree._mutex.unlock();

    return os;
}
template<class T>
void Tree<T>::del(T t) noexcept
{
    std::function<void(Node*, T t)> recFindDel = [&recFindDel](Node* node, T t)
    {
        if (t < node->value)
        {
            if (node->left != nullptr)
            {
                if (node->left->value == t)
                {
                    if (node->left->isLeaf())
                    {
                        delete node->left;
                        node->left = nullptr;
                    }
                    else if (node->left->right == nullptr)
                    {
                        auto oldNode = node->left;

                        node->left = node->left->left;

                        delete oldNode;
                    }
                    else if (node->left->left == nullptr)
                    {
                        auto oldNode = node->left;

                        node->left = node->left->right;

                        delete oldNode;
                    }
                    else
                    {
                        node = node->left;

                        auto leftmost = node->right;
                        Node* leftmostRoot = nullptr;

                        while (leftmost->left != nullptr)
                        {
                            leftmostRoot = leftmost;
                            leftmost = leftmost->left;
                        }

                        auto leftmostValue = leftmost->value;

                        if (leftmost->right != nullptr)
                        {
                            auto oldNode = leftmost;

                            if (leftmostRoot == nullptr)
                            {
                                node->right = leftmost->right;
                            }
                            else
                            {
                                leftmostRoot->left = leftmost->right;
                            }

                            delete oldNode;
                        }
                        else
                        {
                            if (leftmostRoot == nullptr)
                            {
                                node->right = nullptr;
                            }
                            else
                            {
                                leftmostRoot->left = nullptr;
                            }

                            delete leftmost;
                        }

                        node->value = leftmostValue;
                    }
                }
                else
                {
                    recFindDel(node->left, t);
                }
            }
        }
        else if (node->value < t)
        {
            if (node->right != nullptr)
            {
                if (node->right->value == t)
                {
                    if (node->right->isLeaf())
                    {
                        delete node->right;
                        node->right = nullptr;
                    }
                    else if (node->right->right == nullptr)
                    {
                        auto oldNode = node->right;

                        node->right = node->right->left;

                        delete oldNode;
                    }
                    else if (node->right->left == nullptr)
                    {
                        auto oldNode = node->right;

                        node->right = node->right->right;

                        delete oldNode;
                    }
                    else
                    {
                        node = node->right;

                        auto leftmost = node->right;
                        Node* leftmostRoot = nullptr;

                        while (leftmost->left != nullptr)
                        {
                            leftmostRoot = leftmost;
                            leftmost = leftmost->left;
                        }

                        auto leftmostValue = leftmost->value;

                        if (leftmost->right != nullptr)
                        {
                            auto oldNode = leftmost;

                            if (leftmostRoot == nullptr)
                            {
                                node->right = leftmost->right;
                            }
                            else
                            {
                                leftmostRoot->left = leftmost->right;
                            }

                            delete oldNode;
                        }
                        else
                        {
                            if (leftmostRoot == nullptr)
                            {
                                node->right = nullptr;
                            }
                            else
                            {
                                leftmostRoot->left = nullptr;
                            }

                            delete leftmost;
                        }

                        node->value = leftmostValue;
                    }
                }
                else
                {
                    recFindDel(node->right, t);
                }
            }
        }
    };

    _mutex.lock();

    if (_root != nullptr)
    {
        Node* bufferRoot;

        if (_root->value == t)
        {
            bufferRoot = new Node;
            bufferRoot->right = _root;
        }
        else
        {
            bufferRoot = _root;
        }

        recFindDel(bufferRoot, t);
    }
    _mutex.unlock();
}
template<class T>
void Tree<T>::clear() noexcept
{
    std::function<void(Node*)> recClear = [&recClear](Node* node)
    {
        if (node->left != nullptr)
        {
            recClear(node->left);
            delete node->left;
        }
        if (node->right != nullptr)
        {
            recClear(node->right);
            delete node->right;
        }
    };

    _mutex.lock();

    recClear(_root);

    delete _root;

    _root = nullptr;

    _mutex.unlock();
}
template<class T>
Tree<T>::~Tree() noexcept
{
    if (_root != nullptr) { clear(); }
}
template<class T>
void Tree<T>::lrR() const noexcept
{
    std::function<void(Node*)> lrRCout = [&lrRCout](Node* node)
    {
        if (node->left != nullptr)
        {
            lrRCout(node->left);
        }
        if (node->right != nullptr)
        {
            lrRCout(node->right);
        }
        std::cout << node->value << ' ';
    };

    _mutex.lock();

    lrRCout(_root);

    _mutex.unlock();
}
template<class T>
void Tree<T>::Rlr() const noexcept
{
    std::function<void(Node*)> RlrCout = [&RlrCout](Node* node)
    {
        std::cout << node->value << ' ';
        if (node->left != nullptr)
        {
            RlrCout(node->left);
        }
        if (node->right != nullptr)
        {
            RlrCout(node->right);
        }
    };

    _mutex.lock();

    RlrCout(_root);

    _mutex.unlock();
}


#endif //TREE_HPP