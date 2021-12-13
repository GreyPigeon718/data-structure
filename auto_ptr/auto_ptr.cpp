#include <iostream>
#include <memory>
using namespace std;
// RAII（Resource Acquisition Is Initialization）:资源获取初始化---在构造看书中存放资源，在析构函数中将资源清理掉
// 问题：浅拷贝？此处深拷贝无法解决


// auto_ptr C++98
// 智能指针原理：RAII的方式将资源管理 + 具有指针类的操作 + 利用管理权限转移
// 资源的转移
namespace bite
{
    template<class T>
    class auto_ptr
    {
    public:
        auto_ptr(T* ptr = nullptr)
            :_ptr(ptr)
        {}

        //拷贝构造函数
        auto_ptr(auto_ptr<T>& ap)
            :_ptr(ap._ptr)
        {
            ap._ptr = nullptr;
        }

        //赋值运算符重载
        auto_ptr<T>& operator=(auto_ptr<T>& ap)
        {
            if (&ap != this)
            {
                if (_ptr)
                    delete _ptr;
                
                _ptr = ap._ptr;
                ap._ptr = nullptr;
            }
            return *this;
        }

        T& operator*()
        {
            return *_ptr;
        }

        T* operator->()
        {
            return _ptr;
        }

        ~auto_ptr()
        {
            if (_ptr)
            {
                delete _ptr;
            }
        }

        T* Get()
        {
            return _ptr;
        }

    private:
        T* _ptr;
    };
}
// C++11 auto_ptr
// 智能指针原理：RAII的方式将资源管理 + 具有指针类的操作 + 利用管理权限转移
// 资源的释放权利转移
namespace bite1
{
    template<class T>
    class auto_ptr
    {
    public:
        auto_ptr(T* ptr = nullptr)
            :_ptr(ptr)
            ,_owner(false)
        {
            if (_ptr)
                _owner = true;
        }

        auto_ptr(const auto_ptr<T>& ap)
            : _ptr(ap._ptr)
            , _owner(false)
        {
            ap._owner = true;
        }

        auto_ptr<T>& operator=(const auto_ptr<T>& ap)
        {
            if (&ap != this)
            {
                if (_ptr && _owner)
                    delete _ptr;

                _ptr = ap._ptr;
                _owner = ap._owner;
                ap._owner = false;
            }
            return this;
        }

        T& operator*()
        {
            return *_ptr;
        }

        T& operator->()
        {
            return _ptr;
        }

        T* Get()
        {
            return _ptr;
        }

        ~auto_ptr()
        {
            if (_ptr && _owner)
            {
                delete _ptr;
                _owner = false;
            }
        }
    private:
        T* _ptr;
        mutable bool _owner;
    };
}
void Test2()
{

	int* p1 = new int;
	int* p2 = p1;
	*p1 = 10;
	*p2 = 20;
	delete p1;

	bite::auto_ptr<int> sp1(new int);
	bite::auto_ptr<int> sp2(sp1);
	bite::auto_ptr<int> sp3;

	//sp3 = sp2;
	 sp2 = sp3; // 后果：sp1中关联的资源已经被释放了
	 *sp1 = 10;
	// *sp2 = 20;
	// *sp3 = 30;

	// 版本二：可能会造成野指针
	// if (true)
	// {
	// 	bite::auto_ptr<int> sp4(sp3);
	// }

	// // 操作
	// *sp1 = 10;
	// *sp2 = 20;
	// *sp3 = 30;
}
// auto_ptr： 
// 智能指针原理：RAII的方式将资源管理 + 具有指针类似的操作 + 利用管理权限转移
// 资源的转移：

// 建议：什么情况下都不要使用auto_ptr
int main()
{
	Test2();

    // std::auto_ptr<int> ap1(new int);
	// std::auto_ptr<int> ap2(ap1);
    cout << 1;
	return 0;
}