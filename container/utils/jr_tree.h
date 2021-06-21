#ifndef JR_TREE_H
#define JR_TREE_H

#include "../../functional/jr_functional.h"
#include "../../memory/jr_allocator.h"
#include "jr_nodes.h"

namespace panzer {
    // 平衡二叉搜索树（AVL树）
    template<class T,
             bool isMulti,
             class Compare,
             class Allocator >
    class _AVL_Tree{
        // 友元类声明，放出访问根节点、_header权限（因为swap的缘故）
        template<class T1, class T2, class T3, bool a> friend class _set_base;
        template<class T1, class T2, class T3, class T4, bool a > friend class _map_base;

    private:
        typedef _tree_node<T> tnode;
        tnode *_root;
        tnode *_header; // _header标记迭代器end位置（仅用nullptr代表end会造成未查明的指针错误...）
        Compare comp;
        Allocator _alloc_data;
        typename Allocator::template rebind<tnode>::other _alloc_node;

        // 求以r为根节点的二叉树高度（与计算平衡因子有关）
        int _height(tnode *r) {
            if(!r)
                return 0;
            int lh = _height(r->left);
            int rh = _height(r->right);
            return 1 + (lh > rh ? lh : rh);
        }

        // 先序遍历计数目标元素
        size_t _dfs(tnode *r, const T target) const {
            if(!r)
                return 0;
            int cur = 0;
            if(!comp(r->data, target) && !comp(target, r->data))
                cur = 1;
            return cur + _dfs(r->left, target) + _dfs(r->right, target);
        }

        // 向左单向旋转, n指向不平衡节点
        // 新插入节点位于不平衡节点右子树的右子树上
        void _left_rotation(tnode*& y) {
            if(!y || !y->right)  return;
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
            if(!y || !y->left)  return;
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

        // 将不平衡的子树调整为平衡树
        void _adjust_to_balance(tnode *&r) {
            if(r) {
                // 检查平衡因子，判断是否需要进行旋转
                int balance_factor = _height(r->left) - _height(r->right);
                // 平衡因子为2,说明以r为根的子树的左子树比右子树高2,进行相应调整
                if(balance_factor == 2) {
                    // 若删除节点后，左子树的左子树不低于左子树的右子树，则r子树向右旋转
                    if(r->left && (_height(r->left->left)
                                   >= _height(r->left->right)))
                        _right_rotation(r);
                    // 若删除节点后，左子树的左子树低于左子树的右子树，则r子树左-右旋转
                    else {
                        _left_rotation(r->left);
                        _right_rotation(r);
                    }
                // 平衡因子为-2,说明以r为根的子树的右子树比左子树高2,进行相应调整
                } else if(balance_factor == -2) {
                    // 若删除节点后，右子树的右子树不低于右子树的左子树，则r子树向左旋转
                    if(r->right && (_height(r->right->right)
                                    >= _height(r->right->right)))
                        _left_rotation(r);
                    // 若删除节点后，右子树的右子树低于右子树的左子树，则r子树右-左旋转
                    else {
                        _right_rotation(r->right);
                        _left_rotation(r);
                    }
                }
            }
            if(_root) {
                _header->left = _header->right = _root;
                _root->parent = _header;
            } else {
                _header->left = _header->right = _header;
            }
        }

        // 交换两个节点
        void _swap_node(tnode* a, tnode* b) {
            if(!a || !b)
                return;
            tnode *a_parent = a->parent;
            tnode *b_parent = b->parent;
            tnode *a_left = a->left;
            tnode *a_right = a->right;
            tnode *b_left = b->left;
            tnode *b_right = b->right;
            if(a_parent->left == a)
                a_parent->left = b;
            else
                a_parent->right = b;
            if(b_parent->left == b)
                b_parent->left = a;
            else
                b_parent->right = a;
            if(a_parent == _header) {
                a_parent->left = b;
                a_parent->right = b;
                _root = b;
            }
            if(b_parent == a) {
                if(b == a->left) {
                    b->left = a;
                    b->right = a_right;
                } else {
                    b->left = a_left;
                    b->right = a;
                }
                a->left = b_left;
                a->right = b_right;
                a->parent = b;
                b->parent = a_parent;
            } else {
                a->left = b_left;
                a->right = b_right;
                b->left = a_left;
                b->right = a_right;
                a->parent = b_parent;
                b->parent = a_parent;
            }
            if(a->left)
                a->left->parent = a;
            if(a->right)
                a->right->parent = a;
            if(b->left)
                b->left->parent = b;
            if(b->right)
                b->right->parent = b;
        }

        // 插入节点的递归实现
        tnode *_insert_helper(tnode *&r, const T& value, bool& flag) {
            tnode *ret = nullptr;
            flag = true;
            if(!r) {
                // 若为空树，则直接插入
                r = _alloc_node.allocate(1);
                _alloc_data.construct(&(r->data), value);
                r->left = r->right = r->parent = nullptr;
                ret = r;
            } else if(comp(value, r->data)) {
                // 若插入位置在左子树，则先递归向左子树插入
                ret = _insert_helper(r->left, value, flag);
            } else {
                if (comp(r->data, value) || isMulti) {
                    // 若插入位置在右子树，则先递归向右子树插入
                    ret = _insert_helper(r->right, value, flag);
                } else {
                    ret = r;
                    flag = false;
                }
            }
            // 将父节点存储进节点内
            if(r->left)
                r->left->parent = r;
            if(r->right)
                r->right->parent = r;
            // 调整平衡
            _adjust_to_balance(r);
            return ret;
        }

        // 插入节点的递归实现(右值引用)
        tnode *_insert_helper(tnode *&r, T&& value, bool& flag) {
            tnode *ret = nullptr;
            flag = true;
            if(!r) {
                // 若为空树，则直接插入
                r = _alloc_node.allocate(1);
                _alloc_data.construct(&(r->data),
                                      static_cast<T&&>(value));
                r->left = r->right = r->parent = nullptr;
                ret = r;
            } else if(comp(value, r->data)) {
                // 若插入位置在左子树，则先递归向左子树插入
                ret = _insert_helper(r->left,
                                     static_cast<T&&>(value),
                                     flag);
            } else {
                if (comp(r->data, value) || isMulti) {
                    // 若插入位置在右子树，则先递归向右子树插入
                    ret = _insert_helper(r->right,
                                         static_cast<T&&>(value),
                                         flag);
                } else {
                    ret = r;
                    flag = false;
                }
            }
            // 将父节点存储进节点内
            if(r->left)
                r->left->parent = r;
            if(r->right)
                r->right->parent = r;
            // 调整平衡
            _adjust_to_balance(r);
            return ret;
        }

        // 删除节点的递归实现
        void _erase_helper(tnode*& r, const T& key,
                           tnode *hint = nullptr) {
            /*普通BST删除操作*/
            if(!r)
                return;
            if(comp(key, r->data)) {
                _erase_helper(r->left, key, hint);
            } else if(comp(r->data, key)) {
                _erase_helper(r->right, key, hint);
            } else {
                if(hint && (r != hint)) {
                    _erase_helper(r->left, key, hint);
                    _erase_helper(r->right, key, hint);
                } else {
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
                        // 将右子树最小节点的值覆盖当前节点的值，然后递归删除右子树最小节点:
                        tnode *right_min = r->right;
                        while(right_min->left)
                            right_min = right_min->left;
                        _swap_node(r, right_min);
                        r = right_min;
                        _erase_helper(r->right, key, hint);
                    }
                }
            }
            // 调整平衡
            _adjust_to_balance(r);
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
        _AVL_Tree()
            : _root(nullptr), comp(Compare()) {
            _header = _alloc_node.allocate(1);
            _header->left = _header->right = _header;
        }

        _AVL_Tree(Compare c)
            : _root(nullptr), comp(c) {
            _header = _alloc_node.allocate(1);
            _header->left = _header->right = _header;
        }

        template<class InputIt>
        _AVL_Tree(InputIt first, InputIt last,
                  Compare c = Compare())
            : _root(nullptr), comp(c) {
            _header = _alloc_node.allocate(1);
            bool tmp;
            while(first != last) {
                insert(*first, tmp);
                ++first;
            }
        }

        _AVL_Tree(_AVL_Tree&& x,
                  Compare c = Compare()) : comp(c){
            _root = x._root;
            x._root = nullptr;
            _header = _alloc_node.allocate(1);
            _header->left = _header->right = _root;
            if(_root)
                _root->parent = _header;
            x._header->left = x._header->right = x._header;
        }

        ~_AVL_Tree() {
            _delete_all(_root);
            _alloc_node.deallocate(_header, 1);
        }

        // 阻止生成复制运算符
        _AVL_Tree& operator=(const _AVL_Tree& x) = delete;

        // 移动运算符
        _AVL_Tree& operator=(_AVL_Tree&& x) {
            if(this == &x)
                return *this;
            _root = x._root;
            x._root = nullptr;
            _header->left = _header->right = _root;
            if(_root)
                _root->parent = _header;
            x._header->left = x._header->right = x._header;
            return *this;
        }

        // 返回_header标志节点，方便构造end迭代器
        tnode* get_header() const {
            return _header;
        }

        // 查找，返回头指针说明目标元素不存在
        tnode *search(const T& target) {
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

        tnode *search(T&& target) {
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

        size_t count(const T& target) const {
            return _dfs(_root, target);
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
        tnode *insert(const T& value, bool& flag) {
            return _insert_helper(_root, value, flag);
        }

        // 插入值为value的节点(右值引用)
        tnode *insert(T&& value, bool& flag) {
            return _insert_helper(_root,
                                  static_cast<T&&>(value),
                                  flag);
        }

        // 插入value到尽可能前于hint 的位置
        tnode *insert_hint(const T& value, tnode *&hint, bool& flag) {
            if(!comp(value, hint->data))
                return insert(value, flag);
            return _insert_helper(hint->left, value, flag);
        }

        tnode *insert_hint(T&& value, tnode *&hint, bool& flag) {
            if(!comp(value, hint->data))
                return insert(static_cast<T&&>(value), flag);
            return _insert_helper(hint->left,
                                  static_cast<T&&>(value),
                                  flag);
        }

        // 删除值与value相等的节点,主要用于无重复键值
        void erase(const T& key) {
            _erase_helper(_root, key);
        }

        // 删除值与value相等且地址与给定地址相同的节点,主要用于重复键值
        void erase(const T& key, tnode *hint) {
            _erase_helper(_root, key, hint);
        }

        // 删除所有元素
        void erase_all() {
            _delete_all(_root);
            _root = nullptr;
            _header->left = _root;
        }
    };
}

#endif // JR_TREE_H
