#ifndef JR_NODES_H
#define JR_NODES_H

namespace jrSTL {
    // 单向链表节点定义
    template<class U>
    struct _forward_node {
        U data;
        _forward_node<U> *next;
        _forward_node() : next(nullptr) {}
        _forward_node(const U& val) : data(val), next(nullptr) {}
    };

    // 双向链表节点定义
    template<class U>
    struct _node {
        U data;
        _node<U> *prev, *next;
        _node() : prev(nullptr), next(nullptr) {}
        _node(const U& val) : data(val), prev(nullptr), next(nullptr) {}
    };

    // 平衡二叉搜索树节点定义
    template<class U>
    struct _tree_node{
        U data;
        _tree_node *left, *right;
        _tree_node *parent;
        _tree_node()
            : left(nullptr),
              right(nullptr),
              parent(nullptr)
        {}
    };
}

#endif // JR_NODES_H
