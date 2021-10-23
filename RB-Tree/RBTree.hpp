#pragma once
#include <iostream>
using namespace std;
enum Color{RED,BLACK};


template<class T>
struct RBTreeNode
{
    RBTreeNode(const T& val = T(),Color c = RED)
    : left(nullptr)
    , right(nullptr)
    , parent(nullptr)
    , data(val)
    , color(c)
    {}

    RBTreeNode<T>* left;
    RBTreeNode<T>* right;
    RBTreeNode<T>* parent;
    T data;
    Color color;
};

template<class T>
class RBTree
{
    typedef RBTreeNode<T> Node;
public:
    RBTree()
    {
        head = new Node();
        head->left = head;
        head->right = head;
    }

    ~RBTree()
    {
        _Destroy(head->parent);
    }

    bool Insert(const T& data)
    {
        //红黑树也是二叉搜索树
        //1.按照二叉搜索树的规则插入新节点
        Node*& root = GetRoot();
        if (root == nullptr)
        {
            root = new Node(data,BLACK);
            head->left = root;
            head->right = root;
            root->parent = head;
            return true;
        }

        //找待插入节点在BST中的位置，
        //并保存其双亲
        Node* cur = root;
        Node* parent = head;
        while (cur)
        {
            parent = cur;
            if (data > cur->data)
                cur = cur->right;
            else if (data < cur->data)
                cur = cur->left;
            else
                return false;
        }
        //插入新节点
        cur = new Node(data);
        if (data > parent->data)
            parent->right = cur;
        else
            parent->left = cur;
        cur->parent = parent;


        // 新节点插入成功后，红黑树的性质3可能会遭到破坏
        //（不能存在连在一起的红色节点）因为cur新节点默认红色
        // 如果parent节点的颜色为黑色，结束，未违反任何性质
        // 如果parent节点的颜色为红色，违反性质三，向上调整
        while (parent != head && RED == parent->color)
        {
            //parent!=head:该条件说明parent一定是树中有效的节点
            //parent->color == RED:该条件可以保证parent一定不是根节点
            Node* grandFather = parent->parent;
            if (parent == grandFather->left)
            {
                //三种情况
                Node* uncle = grandFather->right;
                if (uncle && uncle->color == RED)
                {
                    //情况一
                    // 插入节点，parent节点为红色，grandFather为黑色
                    // uncle存在且为红色
                    //解决方案：
                    //将parent和uncle节点的颜色修改为黑色，
                    //将grandFather节点颜色修改为红色
                    //需要继续向上更新
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandFather->color = RED;

                    //向上更新
                    cur = grandFather;
                    parent = cur->parent;
                }
                else
                {

                    if (cur == parent->right)
                    {
                        //情况三
                        //情况二的基础下，插入节点在parent的右侧
                        //解决方案：
                        //将parent子树进行左单旋，
                        //将parent和current节点的指向交换
                        //然后同情况二处理
                        RotateLeft(parent);
                        swap(parent, cur);
                    }
                    //情况二
                    //uncle节点不存在 || unclue颜色为BLACK
                    //插入节点在parent的左侧
                    //解决方案：
                    //将parent和grandFather的颜色进行交换，
                    //对grandFather位的子树进行右单旋
                    parent->color = BLACK;
                    grandFather->color = RED;
                    RotateRight(grandFather);
                }
            }
            else
            {
                //情况123的反情况
                Node* uncle = grandFather->left;
                if (uncle && RED == uncle->color)
                {
                    //情况一反情况
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandFather->color = RED;

                    //需要继续往上更新
                    cur = grandFather;
                    parent = cur->parent;
                }
                else
                {
                    //情况二和情况三的反情况
                    if (cur == parent->left)
                    {
                        //情况三的反情况
                        RotateRight(parent);
                        swap(parent,cur);
                    }

                    //情况二的反情况
                    parent->color = BLACK;
                    grandFather->color = RED;
                    RotateLeft(grandFather);
                }
            }
        }

        root->color = BLACK;
        head->left = LeftMost();
        head->right = RightMost();
        return true;
    }

    //中序遍历验证是否为二叉搜索树
    void InOrder()
    {
        _InOrder(head->parent);
        cout << endl;
    }

    bool IsValidRBTree()
    {
        Node* root = GetRoot();
        //空树
        if (nullptr == root)
            return true;

        //2.非空----按照红黑树的性质来进行检测
        if (BLACK != root->color)
        {
            cout << "违反红黑树性质二：根节点不是黑色！"<<endl;
            return false;
        }

        // 验证性质三和性质四
        // 优先验证性质四：每条路径中黑色节点个数相同---遍历找到每条路径中黑色节点个数然后对比
		// 找到一条路径中黑色节点的个数---统计最左侧路径中黑色节点个数
        size_t blackCount = 0;
        Node* cur = root;
        while (cur)
        {
            if (BLACK == cur->color)
                blackCount++;

            cur = cur->left;
        }
        
        // 在检测其他路径中黑色节点的格式与最左侧路径中黑色节点个数是否相同
        size_t pathCount = 0;
        return _IsValidRBTree(root, blackCount, pathCount);
    }

    
private:

    bool _IsValidRBTree(Node* root, size_t blackCount, size_t pathCount)
    {
        if (nullptr == root)
            return true;
        
        if (BLACK == root->color)
            pathCount++;

        Node* parent = root->parent;
        if (parent != head && RED == parent->color && RED == root->color)
        {
            cout << "违反性质三：有连在一起的红色节点！！！" << endl;
            return false;
        }

        // 如果root现在是叶子节点---说明一条路走到底了
        if (nullptr == root->left && nullptr == root->right)
        {
            if (pathCount != blackCount)
            {
                cout << blackCount <<endl;
                cout << pathCount <<endl;
                cout << "违反性质四：路径中黑色节点的个数不一样！！！"<<endl;
                return false;
            }
        }

        return _IsValidRBTree(root->left, blackCount, pathCount)&&
        _IsValidRBTree(root->right, blackCount, pathCount);
    }
    //获取最小节点--->最左节点
    Node* LeftMost()
    {
        Node* cur = GetRoot();
        if (nullptr == cur)
            return head;
        while (cur->left)
            cur = cur->left;

        return cur;
    }

    //获取最大节点--->最右节点
    Node* RightMost()
    {
        Node* cur = GetRoot();
        if (nullptr == cur)
            return head;
        while (cur->right)
            cur = cur->right;

            return cur;
    }

    // 左单旋
    void RotateLeft(Node* parent)
    {
        Node* subR = parent->right;
        Node* subRL = subR->left;

        parent->right = subRL;
        if (subRL)
            subRL->parent = parent;

        subR->left = parent;

        Node* pparent = parent->parent;

        // 更新subR以及parent的双亲
        parent->parent = subR;
        subR->parent = pparent;

        // 处理旋转之前parent双亲的左孩子孩子的指针域
        if (pparent == head)
        {
            //说明旋转之前parent就是跟节点
            // 旋转完成之后新的根节点应该为subR
            head->parent = subR;
        }
        else
        {
            // 旋转之前parent的跟节点是存在的
            if (parent == pparent->left)
                pparent->left = subR;
            else
                pparent->right = subR;
        }
    }

    // 右单旋
    void RotateRight(Node* parent)
    {
        Node* subL = parent->left;
        Node* subLR = subL->right;

        parent->left = subLR;
        if (subLR)
            subLR->parent = parent;

        subL->right = parent;

        // 更新parent和subL的双亲
        Node* pparent = parent->parent;
        parent->parent = subL;
        subL->parent = pparent;

        // 需要处理pparent的指针情况
        if (pparent == head)
            head->parent = subL;
        else
        {
            if (parent == pparent->left)
                pparent->left = subL;
            else
                pparent->right = subL;
        }
    }

    void _InOrder(Node* root)
    {
        if (root)
        {
            _InOrder(root->left);
            cout << root->data << " ";
            _InOrder(root->right);
        }
    }

    void _Destroy(Node*& root)
    {
        if(root)
        {
            _Destroy(root->left);
            _Destroy(root->right);
            delete root;
            root = nullptr;
        }
    }

    Node*& GetRoot()
    {
        return head->parent;
    }
    Node* head;
};

void TestRBTree()
{
	// 0. 创建红黑树
	int a[] = { 5, 3, 4, 1, 7, 8, 2, 6, 0, 9 };
	RBTree<int> t;
	for (auto e : a)
	{
		t.Insert(e);
	}

	//1. 验证是否为二叉搜索树
	t.InOrder();

	//2. 验证是否满则红黑树的性质
	if (t.IsValidRBTree())
	{
		cout << "红黑树ok" << endl;
	}
	else
	{
		cout << "红黑树error!!!" << endl;
	}
}