#include "smart_ptr_test.h"
#include <memory>
#include <boost\smart_ptr.hpp>
//对于编译器来说，智能指针实际上是一个栈对象，
//并非指针类型，在栈对象生命期即将结束时，智能指针通过析构函数释放有它管理的堆内存。
//所有智能指针都重载了“operator->”操作符，直接返回对象的引用，用以操作对象。
//访问智能指针原来的方法则使用“.”操作符


/*
总结：std::auto_ptr 属于STL,能够方便的管理单个堆内存对象,注意如下几点：
（1） 尽量不要使用“operator = ”。如果使用了，就不要再使用先前对象。
（2） release() 函数不会释放对象，仅仅归还所有权。
（3） std::auto_ptr 最好不要当成参数传递（.get()才是实际指针）。
（4） 由于 std::auto_ptr 的“operator = ”问题，有其管理的对象不能放入 std::vector等各种容器中。
使用一个 std::auto_ptr 的限制还真多，还不能用来管理堆内存数组，这应该是你目前在想的事情吧，我也觉得限制挺多的，哪天一个不小心，就导致问题了。
由于 std::auto_ptr 引发了诸多问题，一些设计并不是非常符合 C++ 编程思想
*/
void auto_ptr_test()
{
    std::auto_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));   // 创建对象，输出：num：1
    if (pAPTR.get()) {                    // 判断智能指针是否为空
        pAPTR->print_();                  // 使用 operator-> 调用智能指针对象中的函数
        pAPTR.get()->info_ = "Addition";  // 使用 get() 返回裸指针，然后给内部对象赋值
        pAPTR->print_();                  // 再次打印，表明上述赋值成功
        (*pAPTR).info_     += " other";   // 使用 operator* 返回智能指针内部对象，然后用“.”调用智能指针对象中的函数
        pAPTR->print_();                  // 再次打印，表明上述赋值成功
    }
}

void auto_ptr_test2() {
    std::auto_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));
    if (pAPTR.get()) {
        std::auto_ptr<smart_ptr_test> pAPTR2;
        pAPTR2 = pAPTR; //重点在这一句,不只是赋值，右值还会release，坑爹啊，用什么等于号,注意这个release，一个更大的坑
        pAPTR2->print_();
        pAPTR->print_();
    }
}

void auto_ptr_test3() {
    std::auto_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));
    if (pAPTR.get()) {
#if 0//如果这样，smart_ptr_test的内存不会被释放，只是auto_ptr指针被清空了而已
        pAPTR.release();
#endif // 0
        //如下正确操作：
        delete pAPTR.release();
        //or:pAPTR.reset();
    }
}


//boost::scoped_ptr可以方便的管理单个堆内存对象,独享所有权,auto_ptr被完美替代
void scoped_ptr_test() {
    boost::scoped_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));
    if (pAPTR.get()) {
        pAPTR->print_();
        pAPTR.get()->info_ = "Addition";
        pAPTR->print_();
        (*pAPTR).info_ += " other";
        pAPTR->print_();
        //pAPTR.release();           // error:scoped_ptr 没有 release 函数
        std::auto_ptr<smart_ptr_test> pAPTR2;
        //pAPTR2 = pAPTR;        // error:scoped_ptr 没有重载 operator=，不会导致所有权转移
    }
}

//boost::shared_ptr是专门用于共享所有权的,因为要共享所有权，在内部使用了引用计数
void use_shaded_ptr(boost::shared_ptr<smart_ptr_test> p) {  // 无需使用 reference (或 const reference)
    p->print_();
    std::cout << "TestSharedPtr UseCount: " << p.use_count() << std::endl;
}
void shaded_ptr_test() {
    boost::shared_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));
    if (pAPTR.get()) {
        pAPTR->print_();
        pAPTR.get()->info_ = "Addition";
        pAPTR->print_();
        (*pAPTR).info_ += " other";
        pAPTR->print_();
    }
    std::cout << "shaded_ptr_test UseCount: " << pAPTR.use_count() << std::endl;
    use_shaded_ptr(pAPTR);
    std::cout << "shaded_ptr_test UseCount: " << pAPTR.use_count() << std::endl;
    //pAPTR.release();// error: shared_ptr没有release函数
}

//boost::scoped_array用于管理动态数组的,独享所有权
void scoped_array_test() {
    boost::scoped_array<smart_ptr_test> sArr(new smart_ptr_test[2]); // 使用内存数组来初始化
    if (sArr.get()) {
        sArr[0].print_();
        sArr.get()[0].info_ = "Addition";
        sArr[0].print_();
        //(*sArr)[0].info_ += " other";            // error:scoped_ptr 没有重载operator*
        //sArr[0].release();                       // error:没有 release 函数
        boost::scoped_array<smart_ptr_test> sArr2;
        //sArr2 = sArr;                            // error:没有重载 operator=
    }
}

//boost::weak_ptr是boost::shared_ptr的观察者（Observer）对象，
//boost::weak_ptr 只对 boost::shared_ptr 进行引用，而不改变其引用计数，
//当被观察的 boost::shared_ptr 失效后，相应的 boost::weak_ptr 也相应失效
void weak_ptr_test() {
    boost::weak_ptr<smart_ptr_test> weak_p;
    boost::shared_ptr<smart_ptr_test> shared_p(new smart_ptr_test(1));
    std::cout << "boost::shared_ptr UseCount: " << shared_p.use_count() << std::endl;
    weak_p = shared_p;
    std::cout << "boost::shared_ptr UseCount: " << shared_p.use_count() << std::endl;
}