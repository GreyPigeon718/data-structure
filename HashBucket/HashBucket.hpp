#pragma once

#include <iostream>
using namespace std;

#include <vector>
#include "Common.hpp"

// 哈希桶：数组+链表（无头单链表）

template<class T>
struct HashBucketNode
{
    HashBucketNode<T> * next;
    T data;

    HashBucketNode(const T& x = T())
        :next(nullptr)
        ,data(x)
        {}
};


//  T如果是整型
template<class T>
class T2IntDef
{
public:
	const T& operator()(const T& data)
	{
		return data;
	}
};

//网上字符串转整型方法
class Str2Int
{
public:
    size_t operator()(const string& s)
    {
        const char* str = s.c_str();
        unsigned int seed = 131;// 31 131 1313 13131 131313
        unsigned int hash = 0;
        while (*str)
        {
            hash = hash * seed + (*str++);
        }
        return (hash & 0x7fffffff);
    }
};


// 假设：当前实现的哈希表中元素唯一
// T:哈希表中存储的元素的类型
// T2Int：将T转换为整形的结果

template <class T, class T2Int = T2IntDef<T> >
class HashBucket
{
    typedef HashBucketNode<T> Node;
public:
    HashBucket(size_t capacity = 53)
        : table(GetNextPrime(capacity)) // n个值为data的构造方法在构造哈希桶
        , size(0)
        {}

        ~HashBucket()
        {
            Destroy();
        }

    /*
    虽然从代码层来看，Insert当中存在一个循环，即：遍历链表，检测data是否存在--->表面上Insert的时间复杂度O(N)
    但是实际认为Insert的时间复杂度仍旧位O(1)--->因为： 在哈希桶中，每个痛对应的链表当中的节点格式都不是非常多
    */
    bool Insert(const T& data)
    {
        // 0.检测是否需要扩容
        CheckCapacity();

        // 1. 通过哈希函数计算元素所在桶号
        size_t bucketNo = HashFunc(data);

        // 2. 检测元素是否存在
        Node* cur = table[bucketNo];
        while (cur)
        {
            if (data == cur->data)
            return false;

            cur = cur->next;
        }

        // 3.插入元素
        cur = new Node(data);
        cur->next = table[bucketNo];
        table[bucketNo] = cur;
        ++size;
        return true;
    }

    bool Erase (const T& data)
    {
        // 1. 先通过哈希函数计算元素所在的桶号
        size_t bucketNo = HashFunc(data);

        // 2. 找元素在bucketNo桶是否存在，存在则删除
        // 核心操作--->删除链表当中只为data的节点
        // 该节点可能是链表中的第一个节点
        // 该节点可能是链表中的非第一个节点

        Node* cur = table[bucketNo];
        Node* prev = nullptr; // 标记cur的前一个节点
        while (cur)
        {
            if (data == cur->data)
            {
                // 删除cur节点
                // 删除的节点如果是第一个节点
                if (prev == nullptr)
                    table[bucketNo] = cur->next;
                else
                    prev->next = cur->next;

                delete cur;
                --size;
                return true;
            }
            else
            {
                // 当前节点不是要找的data，则两个指针同时往后移动
                prev = cur;
                cur = cur->next;
            }
        }

        // 哈希桶中不存在值为data的元素，无法删除即删除失败
        return false;
    }

    Node* Find(const T& data)
    {
        // 1.先通过哈希函数计算元素所在的桶号
        size_t bucketNo = HashFunc(data);

        // 2. 在检测该元素是否存在对应的链表中
        Node* cur = table[bucketNo];
        while (cur)
        {
            if (data == cur->data)
                return cur;
            
            cur = cur->next;
        }

        return nullptr;
    }

    size_t Size() const
    {
        return size;
    }

    bool Empty()const
    {
        return size == 0;
    }

    ///////////////////////////////////////////////
    // 为了方便对哈希桶的理解，实现打印哈希桶的方法
    void Print()
    {
        for (size_t i = 0; i < table.capacity(); ++i)
        {
            Node* cur = table[i];

            cout << "table[" << i << "]:";

            while (cur)
            {
                cout << cur->data << "--->";
                cur = cur->next;
            }

            cout << "NULL" << endl;
        }
        cout << "=========================" << endl;
    }

    void Swap(HashBucket<T, T2Int>& ht)
    {
        table.swap(ht.table);
        std::swap(size, ht.size);
    }
private:
    //哈希函数---除留余数法
    size_t HashFunc(const T& data)
    {
        T2Int t2Int;
        return t2Int(data) % table.capacity();
    }

    void Destroy()
    {
        // 循环去销毁：table中的每个链表
        for (size_t i = 0; i < table.capacity(); ++i)
        {
            Node* cur = table[i];

            //采用：头删法删除链表中的每个节点
            while (cur)
            {
                table[i] = cur->next;
                delete cur;
                cur = table[i];
            }
        }
        size = 0;
    }

    void CheckCapacity()
    {
        if (size == table.capacity())
        {
            //扩容---将表格放大---然后将桶中的元素往新桶中插入
            HashBucket<T,T2Int> newHT (GetNextPrime(table.capacity()));

            //将旧哈希桶中的节点拆下来，移到新哈希桶中
            for (size_t i = 0; i < table.capacity(); ++i)
            {
                Node* cur = table[i];
                while(cur)
                {
                    // 1.将cur节点拆下来
                    table[i] = cur->next;

                    // 2.将cur节点newHT中插入
                    // 找位置
                    size_t bucketNo = newHT.HashFunc(cur->data);

                    // 头插法
                    cur->next = newHT.table[bucketNo];
                    newHT.table[bucketNo] = cur;
                    newHT.size++;

                    cur = table[i];
                }
            }

            //已经将旧哈希桶中的元素全部移动到新哈希桶中了
            this->Swap(newHT);
        }
    }
private:
// table当中将来存放所有的链表--->实际只需要存放链表首节点的地址
    std::vector<Node*> table;
    size_t size;    // 有效元素个数
};
void TestHashBucket1()
{
	HashBucket<int>  ht;

	int array[] = { 1, 4, 5, 6, 7, 9,11,14,15,16 };

	for (auto e : array)
	{
		ht.Insert(e);
	}

	cout << ht.Size() << endl;
	ht.Print();

	// 这步插入就需要扩容
	ht.Insert(17);
	ht.Print();

	HashBucketNode<int>* cur = ht.Find(5);
	if (cur)
	{
		cout << "5 is in HashBucket" << endl;
	}
	else
	{
		cout << "5 is not in HashBucket" << endl;
	}

	ht.Insert(44);
	ht.Print();

	ht.Erase(5);
	cur = ht.Find(5);
	if (cur)
	{
		cout << "5 is in HashBucket" << endl;
	}
	else
	{
		cout << "5 is not in HashBucket" << endl;
	}

	ht.Print();
}


void TestHashBucket2()
{
	HashBucket<string, Str2Int> ht;
	ht.Insert("apple");
	ht.Insert("orange");
	ht.Insert("peach");

	ht.Print();
}