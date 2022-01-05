#include <iostream>
#include <string>
#include <assert.h>
namespace bite
{
    class string
    {
    public:
        typedef char* iterator;
    private:
        char* _str; //给出静态顺序表
        size_t _size; // 大小
        size_t _capacity; // 容量
        static size_t npos;
    public:
        string(const char* str = "")
        {
            if (nullptr == str) //判空
                str = "";
            
            _capacity = strlen(str);
            _size = _capacity;
            _str = new char[_capacity + 1]; //多一个‘\0’
            strcpy(_str, str);
        }

        string(const string& s)
            : _str(nullptr)
            , _capacity(0)
            , _size(0)
        {
            string strTemp(s._str);//利用深拷贝
            Swap (strTemp);
        }

        string(size_t n, char ch = char())//n个ch的字符
            : _str(new char[n + 1])
            , _capacity(n)
            , _size(n)
        {
            memset(_str, ch, n);
            _str[n] = '\0';
        }
        string& operator=(string& s)
        {
            Swap(s);
            return *this;
        }
        
        ~string()
        {
            if (_str)
            {
                delete[] _str;
                _str = nullptr;
                _capacity = 0;
                _size = 0;
            }
        }

        ////////////////iterator模块//////////
        // 只实现了正向Iterator
        iterator begin()
        {
            return _str;
        }

        iterator end()
        {
            return _str + size();
        }
        ////////////////capacity模块//////////
        size_t size()const
        {
            return _size;
        }

        size_t capacity()const
        {
            return _capacity;
        }

        bool empty()const
        {
            return _size == 0;
        }

        void clear()
        {
            _str[0] = '\0';
            _size = 0;
        }

        void resize(size_t newsize, char ch)
		{
			size_t oldsize = _size;
			if (newsize > oldsize)
			{
				if (newsize > _capacity)
				{
					reserve(_capacity * 2);
				}

				// 多出的位置需要使用ch进行填充
				memset(_str + oldsize, newsize - oldsize, ch);
			}

			_str[newsize] = '\0';
			_size = newsize;
		}

        void reserve(size_t newcapacity) //扩容
        {
            if (newcapacity > capacity())
            {
                char* temp = new char[newcapacity + 1];
                strcpy(temp, _str);
                delete[] _str;
                _str = temp;
                _capacity = newcapacity;
            }
        }
        /////////////acess模块////////
        char& operator[](size_t index)
        {
            assert(index < _size);
            return _str[index];
        }

        const char& operator[](size_t index)const
        {
            assert(index < _size);
            return _str[index];
        }
        ///////////////////////////////
        string& operator+= (char ch)
        {
            if (_size == _capacity)
                reserve(_capacity * 2);

            _str[_size++] = ch; 
            _str[_size] = '\0';
            return *this;
        }
        string& operator+= (const char* str)
        {
            *this += str;
            return *this;
        }
        string& operator+= (const string& s)
        {
            *this += s._str;
            return *this;
        }


        void push_back(char ch)
        {
            *this += ch;
        }
        void Swap(string& s)
        {
            std::swap(_str,s._str);
            std::swap(_capacity, s._capacity);
            std::swap(_size, s._size);
        }

        string& append(const string& s)
        {
            *this += s;
            return *this;
        }
        ////////////////////////
        const char* c_str()const
        {
            return _str;
        }

        size_t find(char ch, size_t pos = 0)
        {
            if (pos >= _size)
                return npos;
            
            for (size_t index = pos; index < _size;++index)
            {
                if (ch == _str[index])
                    return index;
            }
            return npos;
        }
		size_t rfind(char ch, size_t pos = npos)
		{
			if (pos == npos)
				pos = _size-1;
			
			for (int index = pos; index >= 0; --index)
			{
				if (_str[index] == ch)
					return index;
			}

			return npos;
		}


    };
    size_t string::npos = -1;
}
int main()
{
    return 0;
}