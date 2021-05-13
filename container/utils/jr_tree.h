#ifndef JR_TREE_H
#define JR_TREE_H

#include <iostream>
#include "../../functional/jr_functional.h"
#include "../../memory/jr_allocator.h"

#define _DEBUG

namespace jr_std {
    // 平衡二叉搜索树节点定义
    template<class T>
    struct _tree_node{
        int cnt;
        T data;
        _tree_node *left, *right;
        _tree_node *parent;
        _tree_node()
            : cnt(0), left(nullptr),
              right(nullptr),
              parent(nullptr)
        {}
    };

    // 平衡二叉搜索树（现为AVL树而非红黑树）
    template<class T, bool isMutli = false,
             class Compare = jr_std::less<T>,
             class Allocator = jr_std::allocator<T> >
    class _Balance_BST{
        // 友元类声明，放出访问根节点、_header权限（因为swap的缘故）
        template<class T1, class T2, class T3> friend class set;
        template<class T1, class T2, class T3> friend class multiset;
        template<class T1, class T2, class T3, class T4 > friend class map;

    private:
        typedef _tree_node<T> tnode;
        tnode *_root;
        tnode *_header; // _header标记迭代器end位置（仅用nullptr代表end会造成未查明的指针错误...）
        Compare comp;
        Allocator _alloc_data;
        bool isInsert;
        typename Allocator::template rebind<_tree_node<T> >::other _alloc_node;
        // 求以r为根节点的二叉树高度（与计算平衡因子有关）
        int _height(tnode *r) {
            if(!r)
                return 0;
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
            // 调整各节点的父亲
            x->parent = y->parent;
            y->parent = x;
            if(y->right)
                y->right->parent = y;
            // 改变子树根指向
            y = x;
        }
        // 向右单向旋转, n指向新插入的节点
        // 新插入节点位于不平衡节点左子树的左子树上
        void _right_rotation(tnode*& y) {
            tnode *x = y->left;
            // 交换父节点和祖父节点
            y->left = x->right;
            x->right = y;
            // 调整各节点的父亲
            x->parent = y->parent;
            y->parent = x;
            if(y->left)
                y->left->parent = y;
            // 改变子树根指向
            y = x;
        }
        // 插入节点的递归实现
        tnode *_insert_helper(tnode *r, const T& value) {
            if(!r) {
                // 若为空树，则直接插入
                r = _alloc_node.allocate(1);
                _alloc_data.construct(&(r->data), value);
                r->cnt = 1;
                r->parent = nullptr;
            } else if(comp(value, r->data)) {
                // 若插入位置在左子树，则先递归向左子树插入
                r->left = _insert_helper(r->left, value);
                if(r->left) r->left->parent = r;
                // 检查平衡因子，判断是否需要进行旋转
                int balance_factor = _height(r->left) - _height(r->right);
                // 平衡因子为2,说明以r为根的子树的左子树比右子树高2,进行相应调整
                if(balance_factor == 2) {
                    // 若新增节点插入在r的左子树的左子树上，则r子树向右旋转
                    if(comp(value, r->left->data))
                        _right_rotation(r);
                    // 若新增节点插入在r的左子树的右子树上，则r子树左-右旋转
                    else {
                        _left_rotation(r->left);
                        _right_rotation(r);
                    }
                }
            } else if(comp(r->data, value)) {
                // 若插入位置在右子树，则先递归向右子树插入
                r->right = _insert_helper(r->right, value);
                if(r->right) r->right->parent = r;
                // 检查平衡因子，判断是否需要进行旋转
                int balance_factor = _height(r->left) - _height(r->right);
                // 平衡因子为-2,说明以r为根的子树的右子树比左子树高2,进行相应调整
                if(balance_factor == -2) {
                    // 若新增节点插入在r的右子树的右子树上，则r子树向左旋转
                    if(comp(r->right->data, value))
                        _left_rotation(r);
                    // 若新增节点插入在r的右子树的左子树上，则r子树右-左旋转
                    else {
                        _right_rotation(r->right);
                        _left_rotation(r);
                    }
                }
            } else {
                isInsert = false;
                if(isMutli) {
                    r->cnt++;
                    isInsert = true;
                }
            }
            return r;
        }
        // 插入节点的递归实现(右值引用)
        tnode *_insert_helper(tnode *r, T&& value) {
            if(!r) {
                // 若为空树，则直接插入
                r = _alloc_node.allocate(1);
                _alloc_data.construct(&(r->data), value);
                r->cnt = 1;
                r->parent = nullptr;
            } else if(comp(value, r->data)) {
                // 若插入位置在左子树，则先递归向左子树插入
                r->left = _insert_helper(r->left, value);
                if(r->left) r->left->parent = r;
                // 检查平衡因子，判断是否需要进行旋转
                int balance_factor = _height(r->left) - _height(r->right);
                // 平衡因子为2,说明以r为根的子树的左子树比右子树高2,进行相应调整
                if(balance_factor == 2) {
                    // 若新增节点插入在r的左子树的左子树上，则r子树向右旋转
                    if(comp(value, r->left->data))
                        _right_rotation(r);
                    // 若新增节点插入在r的左子树的右子树上，则r子树左-右旋转
                    else {
                        _left_rotation(r->left);
                        _right_rotation(r);
                    }
                }
            } else if(comp(r->data, value)) {
                // 若插入位置在右子树，则先递归向右子树插入
                r->right = _insert_helper(r->right, value);
                if(r->right) r->right->parent = r;
                // 检查平衡因子，判断是否需要进行旋转
                int balance_factor = _height(r->left) - _height(r->right);
                // 平衡因子为-2,说明以r为根的子树的右子树比左子树高2,进行相应调整
                if(balance_factor == -2) {
                    // 若新增节点插入在r的右子树的右子树上，则r子树向左旋转
                    if(comp(r->right->data, value))
                        _left_rotation(r);
                    // 若新增节点插入在r的右子树的左子树上，则r子树右-左旋转
                    else {
                        _right_rotation(r->right);
                        _left_rotation(r);
                    }
                }
            } else {
                isInsert = false;
                if(isMutli) {
                    r->cnt++;
                    isInsert = true;
                }
            }
            return r;
        }
        // 删除节点的递归实现
        tnode *_erase_helper(tnode *r, const T& value) {
            /*普通BST删除操作*/
            if(!r)  return nullptr;
            if(comp(value, r->data)) {
                r->left = _erase_helper(r->left, value);
            } else if(comp(r->data, value)) {
                r->right = _erase_helper(r->right, value);
            } else {
                r->cnt--;
                if(r->cnt <= 0) {
                    if(!r->left && !r->right) {
                        // 待删除节点为叶节点, 直接删除
                        _alloc_data.destroy(&(r->data));
                        _alloc_node.deallocate(r, 1);
                        r = nullptr;
                    } else if(r->left && !r->right) {
                        // 待删除节点只有左子树, 返回左子树根节点, 删除目标节点
                        tnode *rleft = r->left;
                        rleft->parent = r->parent;
                        _alloc_data.destroy(&(r->data));
                        _alloc_node.deallocate(r, 1);
                        r = rleft;
                    } else if(!r->left && r->right) {
                        // 待删除节点只有右子树, 返回右子树根节点, 删除目标节点
                        tnode *rright = r->right;
                        rright->parent = r->parent;
                        _alloc_data.destroy(&(r->data));
                        _alloc_node.deallocate(r, 1);
                        r = rright;
                    } else {
                        // 待删除节点有左右子树,
                        // 交换右子树最小节点与当前节点:
                        tnode *right_min = r->right;
                        while(right_min->left)
                            right_min = right_min->left;
                        tnode *r_parent = r->parent;
                        tnode *rmin_parent = right_min->parent;
                        tnode *t1 = right_min->left, *t2 = right_min->right;
                        if(rmin_parent != r) {
                            rmin_parent->left = r;
                            r->parent = rmin_parent;
                            if(r_parent->left == r)
                                r_parent->left = right_min;
                            else
                                r_parent->right = right_min;
                            right_min->right = r->right;
                        }else{
                            r->parent = right_min;
                            if(r_parent->left == r)
                                r_parent->left = right_min;
                            else
                                r_parent->right = right_min;
                            right_min->right = r;
                        }
                        right_min->parent = r_parent;
                        right_min->left = r->left;
                        r->left = t1;
                        r->right = t2;
                        r = right_min;
                        r->right = _erase_helper(r->right, value);
                        if(r->left)
                            r->left->parent = r;
                        if(r->right)
                            r->right->parent = r;
                    }
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
        void _delete_all(tnode *&r) {
            if(!r)
                return;
            _delete_all(r->left);
            _delete_all(r->right);
            _alloc_data.destroy(&(r->data));
            _alloc_node.deallocate(r, 1);
            r = nullptr;
        }

    public:
        // 构造
        _Balance_BST() : _root(nullptr) {
            _header = _alloc_node.allocate(1);
            _header->left = _header;
        }

        _Balance_BST(Compare c) : _root(nullptr), comp(c) {
            _header = _alloc_node.allocate(1);
            _header->left = _header;
        }

        template<class InputIt>
        _Balance_BST(InputIt first, InputIt last, Compare c = Compare())
            : _root(nullptr), comp(c) {
            _header = _alloc_node.allocate(1);
            _header->left = _header;
            while(first != last) {
                this->insert(*first);
                ++first;
            }
        }

        _Balance_BST(_Balance_BST&& x, Compare c = Compare()) : comp(c){
            _root = x._root;
            x._root = nullptr;
            _header = _alloc_node.allocate(1);
            _header->left = _root;
            _root->parent = _header;
            x._header->left = x._header;
        }

        ~_Balance_BST() {
            _delete_all(_root);
            _alloc_node.deallocate(_header, 1);
        }

    private:
        // 阻止生成复制运算符
        _Balance_BST& operator=(const _Balance_BST& x);

    public:
        // 移动运算符
        _Balance_BST& operator=(_Balance_BST&& x) {
            _root = x._root;
            x._root = nullptr;
            _header->left = _root;
            _root->parent = _header;
            x._header->left = x._header;
            return *this;
        }

        // 返回_header标志节点，方便构造end迭代器
        tnode* get_header() const { return _header; }

        // 查找，返回空指针说明目标元素不存在
        tnode *search(const T& target) const {
            tnode *tmp = _root;
            while(tmp) {
                if(comp(tmp->data, target)) {
                    tmp = tmp->right;
                } else if(comp(target, tmp->data)) {
                    tmp = tmp->left;
                } else{
                    return tmp;
                }
            }
            return _header;
        }

        tnode *search(T&& target) const {
            tnode *tmp = _root;
            while(tmp) {
                if(comp(tmp->data, target)) {
                    tmp = tmp->right;
                } else if(comp(target, tmp->data)) {
                    tmp = tmp->left;
                } else{
                    return tmp;
                }
            }
            return _header;
        }

        tnode* get_min() const {
            if(!_root)
                return _header;
            tnode *tmp = _root;
            while(tmp->left)
                tmp = tmp->left;
            return tmp;
        }

        tnode* get_max() const {
            if(!_root)
                return _header;
            tnode *tmp = _root;
            while(tmp->right)
                tmp = tmp->right;
            return tmp;
        }

        // 插入值为value的节点
        bool insert(const T& value) {
            isInsert = true;
            _root = _insert_helper(_root, value);
            _root->parent = _header;
            _header->left = _root;
            return isInsert;
        }

        // 插入值为value的节点(右值引用)
        bool insert(T&& value) {
            isInsert = true;
            _root = _insert_helper(_root, static_cast<T&&>(value));
            _root->parent = _header;
            _header->left = _root;
            return isInsert;
        }

        // 范围插入
        template<class InputIt>
        void insert(InputIt first, InputIt last) {
            while(first != last) {
                insert(*first);
                ++first;
            }
        }

        // 插入value到尽可能接近，正好前于hint 的位置
        bool insert_hint(const T& value, tnode *&hint) {
            if(!_root)
                return insert(value);
            isInsert = true;
            hint->left = _insert_helper(hint->left, value);
            if(hint->left)
                hint->left->parent = hint;
            _root->parent = _header;
            _header->left = _root;
            return isInsert;
        }

        bool insert_hint(T&& value, tnode *&hint) {
            if(!_root)
                return insert(static_cast<T&&>(value));
            isInsert = true;
            hint->left = _insert_helper(hint->left, static_cast<T&&>(value));
            if(hint->left)
                hint->left->parent = hint;
            _root->parent = _header;
            _header->left = _root;
            return isInsert;
        }

        // 删除值与value相等的节点
        void erase(const T& value) {
            // 递归删除目标节点
            _root = _erase_helper(_root, value);
            if(_root)
                _root->parent = _header;
            _header->left = _root;
        }

        // 删除所有元素
        void erase_all() {
            _delete_all(_root);
            _root = nullptr;
            _header->left = _root;
        }
#ifdef _DEBUG
        // 先序遍历方便debug
        void _pre_order(tnode *n) {
            if(!n) return;
            std::cout << "data = " << n->data.first << ", cnt = " << n->cnt;
            if(n->parent)
                std::cout << ", parent = " << n->parent->data.first;
            std::cout << std::endl;
            _pre_order(n->left);
            _pre_order(n->right);
        }

        void print_tree() {
            std::cout << "Tree:\n";
            _pre_order(_root);
            std::cout << std::endl;
        }
#endif
    };
}

#endif // JR_TREE_H
