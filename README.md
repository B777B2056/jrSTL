# MINI-STL概览  
本项目遵循C++11标准，实现了C++11标准库中的所有容器与容器适配器，algorithm头文件中所有泛型算法，迭代器相关内容（迭代器原语，迭代器特性萃取机），空间配置器框架，
functional头文件大部分功能（缺function和bind），以及三个智能指针（unique/shared/weak ptr）；采用Google gtest测试框架，以函数为单元进行了单元测试。
# 文件目录结构与文件说明  
.  
├── algorithm  
│   ├── jr_algo_buffer.h  采用RAII方式管理资源的、在algorithm中存在的缓存数据段       
│   ├── jr_algorithm.h  算法库，包含了除未初始化内存操作外的所有C++11中的算法  
│   └── jr_numeric.h  数值库的一部分，包含一些简单的数值算法  
├── CMakeLists.txt  
├── CMakeLists.txt.user  
├── container  
│   ├── adapter  容器适配器  
│   │   ├── jr_priority_queue.h  优先队列（二叉堆）    
│   │   ├── jr_queue.h  队列  
│   │   └── jr_stack.h  栈  
│   ├── associate  关联容器  
│   │   ├── jr_map.h  包含map/multimap  
│   │   ├── jr_set.h  包含set/multiset  
│   │   ├── jr_unordered_map.h  包含unordered_map/unordered_multimap  
│   │   └── jr_unordered_set.h  包含unordered_set/unordered_multiset  
│   ├── sequence  顺序容器  
│   │   ├── jr_array.h  
│   │   ├── jr_deque.h  
│   │   ├── jr_forward_list.h  
│   │   ├── jr_list.h  
│   │   └── jr_vector.h  
│   └── utils  单独拿出来的一些容器中的工具类  
│       ├── jr_hashtable.h  哈希表实现  
│       ├── jr_heap.h  二叉堆实现    
│       ├── jr_iterators.h  各种容器的迭代器实现  
│       ├── jr_nodes.h  在容器中用到的链式结构的节点定义  
│       └── jr_tree.h  AVL平衡二叉搜素树实现  
├── functional  
│   └── jr_functional.h  
├── iterator  
│   └── jr_iterator.h  包含迭代器原语、特性萃取机、迭代器适配器等内容  
├── main.cpp  单元测试的主函数  
├── memory  
│   ├── jr_allocator.h  空间分配器  
│   └── jr_smart_ptr.h  智能指针  
└── test  单元测试文件  
    ├── algorithm_test.cpp  
    ├── array_test.cpp  
    ├── deque_test.cpp  
    ├── forward_list_test.cpp  
    ├── hash_map_test.cpp  
    ├── hash_multimap_test.cpp  
    ├── hash_multiset_test.cpp  
    ├── hashset_test.cpp  
    ├── list_test.cpp  
    ├── map_test.cpp  
    ├── multimap_test.cpp  
    ├── multiset_test.cpp  
    ├── priority_queue_test.cpp  
    ├── queue_test.cpp  
    ├── set_test.cpp  
    ├── shared_ptr_test.cpp  
    ├── stack_test.cpp  
    ├── unique_ptr_test.cpp  
    ├── vector_test.cpp  
    └── weak_ptr_test.cpp  
# 具体实现中的一些特点  
1.采用AVL平衡二叉树而非红黑树作为set/map的底层存储结构； 
2.为了保证删除某个迭代器后不影响其余迭代器，在AVL树中的删除操作中，并非简单交换待删除的节点的值与其右子树最小节点的值，而是将二者节点进行交换；  
3.在set/multiset中，使用一个基类来统一二者共有的操作，一个bool类型模板参数来控制是否保留相同的键值（其余所有关联容器中均如此），有效降低了重复代码率；  
4.unordered容器（哈希容器）中，采用桶方法解决哈希冲突，目前每个桶内为单向链表，但若想转换为查找性能更好的AVL树仅需改动少数几处即可；
5.search算法中，若分配缓存成功，则采用KMP算法进行模式匹配；  
6.sort算法中，若递归深度小于阈值且剩余元素个数大于阈值，则采用快速排序算法；若递归深度大于阈值则采用归并排序；若剩余元素个数较少则采用插入排序；  
7.在shared_ptr智能指针中，Alloc参数并不起作用（我认为这个用于分配控制块的空间配置器比较鸡肋，于是没有实现...）；  
8.可在jr_iterator.h文件中通过USE_STD_TRAITS宏定义来控制该库是否支持std的迭代器原语tag。  
# 安装与使用
## 安装
通过cmake编译为动态库即可使用
## 使用
1.使用全局命名空间定义  
在你的代码最前面加上```using namespace panzer;```  
2.使用限定符  
在你想使用该库的每个地方都使用panzer进行限定；例如```panzer::vector<int> v;```  
# 未完成的内容
1.functional头文件  
  function，bind  
2.allocator内存池实现  
  之前实现过，但后来发现的bug暂无法解决，于是现暂用malloc/free代替  
# 未来的工作  
有空的话就继续完善一下测试文件，然后把内存池实现了，以后系统学了元编程再把functional剩下的内容整了哈哈哈    
