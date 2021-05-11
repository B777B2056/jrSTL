#ifndef JR_TREE_H
#define JR_TREE_H

#include <iostream>
#include "../../container/adapter/jr_queue.h"
#include "../../memory/jr_allocator.h"

#define _DEBUG

namespace jr_std {
    // 平衡二叉搜索树节点定义
    template<class T>
    struct _tree_node{
        T data;
        _tree_node *left, *right;
        _tree_node *parent;
        _tree_node()
            : left(nullptr),
              right(nullptr),
              parent(nullptr)
        {}
    };

    // 平衡二叉搜索树（现为AVL树而非红黑树）
    template<class T, class Allocator = jr_std::allocator<T> >
    class _Balance_BST{
    private:
        typedef _tree_node<T> tnode;
        tnode *_root;
        Allocator _alloc_data;
        typename Allocator::template rebind<_tree_node<T> >::other _alloc_node;
        // 求以r为根节点的二叉树高度（与计算平衡因子有关）
        int _height(tnode *r) {
            if(!r)  return 0;
            int lh = _height(r->left);
            int rh = _height(r->right);
            return 1 + (lh > rh ? lh : rh);
        }
        // 向左单向旋转, n指向不平衡节点
        // 新插入节点位于不平衡节点右子树的右子树上
        void _left_rotation(tnode*& y) {
            tnode *x = y->right;
            // 交换不平衡节点与其左子节点
            y->right = x->left;
            x->left = y;
            // 改变子树根指向
            y = x;
            // 调整各节点的父亲和祖父
            x->parent = y->parent;
            y->parent = x;
        }
        // 向右单向旋转, n指向新插入的节点
        // 新插入节点位于不平衡节点左子树的左子树上
        void _right_rotation(tnode*& y) {
            tnode *x = y->left;
            // 交换父节点和祖父节点
            y->left = x->right;
            x->right = y;
            // 改变子树根指向
            y = x;
            // 调整各节点的父亲和祖父
            x->parent = y->parent;
            y->parent = x;
        }
        // 插入节点的递归实现
        tnode *_insert_helper(tnode *r, const T& value) {
            if(!r) {
                // 若为空树，则直接插入
                r = _alloc_node.allocate(1);
                _alloc_data.construct(&(r->data), value);
                return r;
            } else if(r->data > value) {
                // 若插入位置在左子树，则先递归向左子树插入
                r->left = _insert_helper(r->left, value);
                // 检查平衡因子，判断是否需要进行旋转
                int balance_factor = _height(r->left) - _height(r->right);
                // 平衡因子为2,说明以r为根的子树的左子树比右子树高2,进行相应调整
                if(balance_factor == 2) {
                    // 若新增节点插入在r的左子树的左子树上，则r子树向右旋转
                    if(r->left->data > value)
                        _right_rotation(r);
                    // 若新增节点插入在r的左子树的右子树上，则r子树左-右旋转
                    else {
                        _left_rotation(r->left);
                        _right_rotation(r);
                    }
                }
            } else if(r->data < value) {
                // 若插入位置在右子树，则先递归向右子树插入
                r->right = _insert_helper(r->right, value);
                // 检查平衡因子，判断是否需要进行旋转
                int balance_factor = _height(r->left) - _height(r->right);
                // 平衡因子为-2,说明以r为根的子树的右子树比左子树高2,进行相应调整
                if(balance_factor == -2) {
                    // 若新增节点插入在r的右子树的右子树上，则r子树向左旋转
                    if(r->right->data < value)
                        _left_rotation(r);
                    // 若新增节点插入在r的右子树的左子树上，则r子树右-左旋转
                    else {
                        _right_rotation(r->right);
                        _left_rotation(r);
                    }
                }
            }
            return r;
        }
        // 删除节点的递归实现
        tnode *_erase_helper(tnode *r, const T& value) {
            /*普通BST删除操作*/
            if(!r)  return nullptr;
            if(r->data > value) {
                r->left = _erase_helper(r->left, value);
            } else if(r->data < value) {
                r->right = _erase_helper(r->right, value);
            } else {
                if(!r->left && !r->right) {
                    // 待删除节点为叶节点, 直接删除
                    _alloc_data.destroy(&(r->data));
                    _alloc_node.deallocate(r, 1);
                    r = nullptr;
                } else if(r->left && !r->right) {
                    // 待删除节点只有左子树, 返回左子树根节点, 删除目标节点
                    tnode *rleft = r->left;
                    _alloc_data.destroy(&(r->data));
                    _alloc_node.deallocate(r, 1);
                    r = rleft;
                } else if(!r->left && r->right) {
                    // 待删除节点只有右子树, 返回右子树根节点, 删除目标节点
                    tnode *rright = r->right;
                    _alloc_data.destroy(&(r->data));
                    _alloc_node.deallocate(r, 1);
                    r = rright;
                } else {
                    // 待删除节点有左右子树,
                    // 以右子树最小节点代替当前节点:
                    // 只交换二者的data, 然后递归删除交换后的最小节点
                    tnode *right_min = r->right;
                    while(right_min->left)
                        right_min = right_min->left;
                    T tmp = r->data;
                    r->data = right_min->data;
                    right_min->data = tmp;
                    r->right = _erase_helper(r->right, value);
                }
            }
            /*将不平衡节点调整为平衡*/
            if(r) {
                // 检查平衡因子，判断是否需要进行旋转
                int balance_factor = _height(r->left) - _height(r->right);
                // 平衡因子为2,说明以r为根的子树的左子树比右子树高2,进行相应调整
                if(balance_factor == 2) {
                    // 若删除节点后，左子树的左子树不低于左子树的右子树，则r子树向右旋转
                    if(_height(r->left->left) >= _height(r->left->right))
                        _right_rotation(r);
                    // 若删除节点后，左子树的左子树低于左子树的右子树，则r子树左-右旋转
                    else {
                        _left_rotation(r->left);
                        _right_rotation(r);
                    }
                // 平衡因子为-2,说明以r为根的子树的右子树比左子树高2,进行相应调整
                } else if(balance_factor == -2) {
                    // 若删除节点后，右子树的右子树不低于右子树的左子树，则r子树向左旋转
                    if(_height(r->right->right) >= _height(r->right->right))
                        _left_rotation(r);
                    // 若删除节点后，右子树的右子树低于右子树的左子树，则r子树右-左旋转
                    else {
                        _right_rotation(r->right);
                        _left_rotation(r);
                    }
                }
            }
            return  r;
        }
        // 析构每个节点的数据域，再释放每个节点所占空间
        void _delete_all(tnode *r) {
            if(!r)
                return;
            _delete_all(r->left);
            _delete_all(r->right);
            _alloc_data.destroy(&(r->data));
            _alloc_node.deallocate(r, 1);
        }

    public:
        // 构造
        _Balance_BST() : _root(nullptr) {}

        template<class InputIt>
        _Balance_BST(InputIt first, InputIt last) : _root(nullptr) {
            while(first != last) {
                this->insert(first++, last);
            }
        }

        ~_Balance_BST() {
            _delete_all(_root);
        }

        // 查找，返回空指针说明目标元素不存在
        tnode *search(const T& target) {
            tnode *tmp = _root, *result = nullptr;
            while(tmp) {
                if(tmp->data < target) {
                    tmp = tmp->right;
                } else if(tmp->data > target) {
                    tmp = tmp->left;
                } else{
                    result = tmp;
                    break;
                }
            }
            return result;
        }

        const T& get_min() {
            tnode *tmp = _root;
            while(tmp->left)
                tmp = tmp->left;
            return tmp->data;
        }

        const T& get_max() {
            tnode *tmp = _root;
            while(tmp->right)
                tmp = tmp->right;
            return tmp->data;
        }

        // 插入值为value的节点
        void insert(const T& value) {
            _root = _insert_helper(_root, value);
        }

        // 删除值与value相等的节点
        void erase(const T& value) {
            // 递归删除目标节点
            _root = _erase_helper(_root, value);
        }
#ifdef _DEBUG
        // 先序遍历方便debug
        void _pre_order(tnode *n) {
            if(!n) return;
            std::cout << n->data << " ";
            _pre_order(n->left);
            _pre_order(n->right);
        }

        void print_tree() {
            _pre_order(_root);
        }
#endif
    };
}

#endif // JR_TREE_H
