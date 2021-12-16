/**
 * C++ 语言: 二叉搜索树
 */

#ifndef _BINARY_TREE_HPP_
#define _BINARY_TREE_HPP_

#include <iomanip>
#include <iostream>
using namespace std;

template <class T>
class BSTNode{
    public:
        T key;            // 关键字(键值)
        BSTNode *left;    // 左孩子
        BSTNode *right;    // 右孩子
        BSTNode *parent; // 父结点

        BSTNode(T value, BSTNode *p, BSTNode *l, BSTNode *r):
            key(value),parent(),left(l),right(r) {}
};

template <class T>
class BSTree {
    private:
        BSTree<T> *mRoot;    // 根结点

    public:
        BSTree();
        ~BSTree();

        // 前序遍历
        void preOrder();
        // 中序遍历
        void inOrder();
        // 后序遍历
        void postOrder();

        // (递归实现)查找中键值为key的节点
        BSTNode<T>* search(T key);
        // (非递归实现)查找中键值为key的节点
        BSTNode<T>* iterativeSearch(T key);

        // 查找最小结点：返回最小结点的键值。
        T minimum();
        // 查找最大结点：返回最大结点的键值。
        T maximum();

        // 找结点(x)的后继结点。即，查找"数据值大于该结点"的"最小结点"。
        BSTNode<T>* successor(BSTNode<T> *x);
        // 找结点(x)的前驱结点。即，查找"数据值小于该结点"的"最大结点"。
        BSTNode<T>* predecessor(BSTNode<T> *x);

        // 将结点(key为节点键值)插入
        void insert(T key);

        // 删除结点(key为节点键值)
        void remove(T key);

        // 销毁
        void destroy();

        // 打印
        void print();
    private:
        // 前序遍历
        void preOrder(BSTNode<T>* tree) const;
        // 中序遍历
        void inOrder(BSTNode<T>* tree) const;
        // 后序遍历
        void postOrder(BSTNode<T>* tree) const;

        // (递归实现)查找"x"中键值为key的节点
        BSTNode<T>* search(BSTNode<T>* x, T key) const;
        // (非递归实现)查找"x"中键值为key的节点
        BSTNode<T>* iterativeSearch(BSTNode<T>* x, T key) const;

        // 查找最小结点：返回tree为根结点的最小结点。
        BSTNode<T>* minimum(BSTNode<T>* tree);
        // 查找最大结点：返回tree为根结点的最大结点。
        BSTNode<T>* maximum(BSTNode<T>* tree);
        // 插入函数
        void insert(BSTNode<T>* &root, BSTNode<T>* node);
        bool insertRecursively(BSTNode<T>* &root, BSTNode<T>* node)
        // 删除函数
        void remove(BSTNode<T>* &root, BSTNode<T> *node);
        // 销毁
        void destroy(BSTNode<T>* &tree);
        // 打印
        void print(BSTNode<T>* tree, T key, int direction);
};

/*
 * 构造函数
 */
template <class T>
BSTree<T>::BSTree():mRoot(NULL)
{
    mRoot = NULL;
}

/*
 * 析构函数
 */
template <class T>
BSTree<T>::~BSTree()
{
    destroy();
}

/*
 * 前序遍历   根左右
 */
template <class T>
void BSTree<T>::preOrder(BSTNode<T>* tree) const
{
    if(tree != NULL)
    {
        cout<< tree->key << " " ;
        preOrder(tree->left);
        preOrder(tree->right);
    }
}

template <class T>
void BSTree<T>::preOrder()
{
    preOrder(mRoot);
}

/*
 * 中序遍历   左根右
 */
template <class T>
void BSTree<T>::inOrder(BSTNode<T>* tree) const
{
    if(tree != NULL)
    {
        inOrder(tree->left);
        cout<< tree->key << " " ;
        inOrder(tree->right);
    }
}

template <class T>
void BSTree<T>::inOrder()
{
    inOrder(mRoot);
}

/*
 * 后序遍历   左右根
 */
template <class T>
void BSTree<T>::postOrder(BSTNode<T>* tree) const
{
    if(tree != NULL)
    {
        postOrder(tree->left);
        postOrder(tree->right);
        cout<< tree->key << " " ;
    }
}

template <class T>
void BSTree<T>::postOrder()
{
    postOrder(mRoot);
}

/*
 * (递归实现)查找"x"中键值为key的节点
 */
template <class T>
BSTNode<T>* BSTree<T>::search(BSTNode<T>* x, T key) const
{
    if (x==NULL || x->key==key)
        return x;

    if (key < x->key)
        return search(x->left, key);
    else
        return search(x->right, key);
}

template <class T>
BSTNode<T>* BSTree<T>::search(T key)
{
    search(mRoot, key);
}

/*
 * (非递归实现)查找"x"中键值为key的节点
 */
template <class T>
BSTNode<T>* BSTree<T>::iterativeSearch(BSTNode<T>* x, T key) const
{
    while ((x!=NULL) && (x->key!=key))
    {
        if (key < x->key)
            x = x->left;
        else
            x = x->right;
    }

    return x;
}

template <class T>
BSTNode<T>* BSTree<T>::iterativeSearch(T key)
{
    iterativeSearch(mRoot, key);
}

/*
 * 查找最小结点：返回tree为根结点的红黑树的最小结点。
 */
template <class T>
BSTNode<T>* BSTree<T>::minimum(BSTNode<T>* tree)
{
    if (tree == NULL)
        return NULL;

    while(tree->left != NULL)
        tree = tree->left;
    return tree;
}

template <class T>
T BSTree<T>::minimum()
{
    BSTNode<T> *p = minimum(mRoot);
    if (p != NULL)
        return p->key;

    return (T)NULL;
}

/*
 * 查找最大结点：返回tree为根结点的最大结点。
 */
template <class T>
BSTNode<T>* BSTree<T>::maximum(BSTNode<T>* tree)
{
    if (tree == NULL)
        return NULL;

    while(tree->right != NULL)
        tree = tree->right;
    return tree;
}

template <class T>
T BSTree<T>::maximum()
{
    BSTNode<T> *p = maximum(mRoot);
    if (p != NULL)
        return p->key;

    return (T)NULL;
}

/*
 * 找结点(x)的后继结点。即，查找"红黑树中数据值大于该结点"的"最小结点"。
 */
template <class T>
BSTNode<T>* BSTree<T>::successor(BSTNode<T> *x)
{
    // 如果x存在右孩子，则"x的后继结点"为 "以其右孩子为根的子树的最小结点"。
    if (x->right != NULL)
        return minimum(x->right);

    // 如果x没有右孩子。则x有以下两种可能：
    // (01) x是"一个左孩子"，则"x的后继结点"为 "它的父结点"。
    // (02) x是"一个右孩子"，则查找"x的最低的父结点，并且该父结点要具有左孩子"，找到的这个"最低的父结点"就是"x的后继结点"。
    BSTNode<T>* y = x->parent;
    while ((y!=NULL) && (x==y->right))
    {
        x = y;
        y = y->parent;
    }

    return y;
}

/*
 * 找结点(x)的前驱结点。即，查找"红黑树中数据值小于该结点"的"最大结点"。
 */
template <class T>
BSTNode<T>* BSTree<T>::predecessor(BSTNode<T> *x)
{
    // 如果x存在左孩子，则"x的前驱结点"为 "以其左孩子为根的子树的最大结点"。
    if (x->left != NULL)
        return maximum(x->left);

    // 如果x没有左孩子。则x有以下两种可能：
    // (01) x是"一个右孩子"，则"x的前驱结点"为 "它的父结点"。
    // (01) x是"一个左孩子"，则查找"x的最低的父结点，并且该父结点要具有右孩子"，找到的这个"最低的父结点"就是"x的前驱结点"。
    BSTNode<T>* y = x->parent;
    while ((y!=NULL) && (x==y->left))
    {
        x = y;
        y = y->parent;
    }

    return y;
}

template<class T>
void BSTree<T>::insert(BSTnode<T>* &root, BSTNode<T>* node)
{
    BSTNode<T> *y = NULL;
    BSTNode<T> *x = root;

    // 1. 将红黑树当作一颗二叉查找树，将节点添加到二叉查找树中。
    while (x != NULL)
    {
        y = x;
        if (node->key < x->key)
            x = x->left;
        else
            x = x->right;
    }

    node->parent = y;
    if (y!=NULL)
    {
        if (node->key < y->key)
            y->left = node;
        else
            y->right = node;
    }
    else
        root = node;
}

template<typename T>
bool BSTree<T>::insertRecursively(BSTNode<T>* &root, BSTNode<T>* node)
{
    if (root == nullptr)
    {
        root = node;
        return true;
    }
    if (root->key>node->key)
    {
        if (root->left == nullptr)
        {
            root->left = node;
            node->parent = root;
        }
        else
            return insertRecursively(root->left, node);
    }
    else if (root->key < node->key)
    {
        if (root->right == nullptr)
        {
            root->right = node;
            node->parent = root->right;
        }
        else
            return insertRecursively(root->right, node);
    }
    return false;
}