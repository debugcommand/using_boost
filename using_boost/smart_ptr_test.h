#ifndef _SMART_PTR_TEST_H_
#define _SMART_PTR_TEST_H_
#include <iostream>
#include <string>

class smart_ptr_test
{
public:
    smart_ptr_test(int n = 0) {
        num_ = n;
        std::cout << "enter num:" << num_ << std::endl;
        info_ = "inited";
    }
    ~smart_ptr_test() {
        std::cout << "out num:" << num_ << std::endl;
    }

    void print_() {
        std::cout << "print:" << info_.c_str() << std::endl;
    }

    std::string info_;
    int         num_;
};
extern void auto_ptr_test();
extern void auto_ptr_test2(); //错误比较
extern void auto_ptr_test3(); //release这个魂淡
extern void scoped_ptr_test();
extern void shaded_ptr_test();
extern void scoped_array_test();
extern void weak_ptr_test();
#endif // _SMART_PTR_TEST_H_


