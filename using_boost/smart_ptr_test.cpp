#include "smart_ptr_test.h"
#include <memory>
#include <boost\smart_ptr.hpp>
//���ڱ�������˵������ָ��ʵ������һ��ջ����
//����ָ�����ͣ���ջ���������ڼ�������ʱ������ָ��ͨ�����������ͷ���������Ķ��ڴ档
//��������ָ�붼�����ˡ�operator->����������ֱ�ӷ��ض�������ã����Բ�������
//��������ָ��ԭ���ķ�����ʹ�á�.��������


/*
�ܽ᣺std::auto_ptr ����STL,�ܹ�����Ĺ��������ڴ����,ע�����¼��㣺
��1�� ������Ҫʹ�á�operator = �������ʹ���ˣ��Ͳ�Ҫ��ʹ����ǰ����
��2�� release() ���������ͷŶ��󣬽����黹����Ȩ��
��3�� std::auto_ptr ��ò�Ҫ���ɲ������ݣ�.get()����ʵ��ָ�룩��
��4�� ���� std::auto_ptr �ġ�operator = �����⣬�������Ķ����ܷ��� std::vector�ȸ��������С�
ʹ��һ�� std::auto_ptr �����ƻ���࣬����������������ڴ����飬��Ӧ������Ŀǰ���������ɣ���Ҳ��������ͦ��ģ�����һ����С�ģ��͵��������ˡ�
���� std::auto_ptr ������������⣬һЩ��Ʋ����Ƿǳ����� C++ ���˼��
*/
void auto_ptr_test()
{
    std::auto_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));   // �������������num��1
    if (pAPTR.get()) {                    // �ж�����ָ���Ƿ�Ϊ��
        pAPTR->print_();                  // ʹ�� operator-> ��������ָ������еĺ���
        pAPTR.get()->info_ = "Addition";  // ʹ�� get() ������ָ�룬Ȼ����ڲ�����ֵ
        pAPTR->print_();                  // �ٴδ�ӡ������������ֵ�ɹ�
        (*pAPTR).info_     += " other";   // ʹ�� operator* ��������ָ���ڲ�����Ȼ���á�.����������ָ������еĺ���
        pAPTR->print_();                  // �ٴδ�ӡ������������ֵ�ɹ�
    }
}

void auto_ptr_test2() {
    std::auto_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));
    if (pAPTR.get()) {
        std::auto_ptr<smart_ptr_test> pAPTR2;
        pAPTR2 = pAPTR; //�ص�����һ��,��ֻ�Ǹ�ֵ����ֵ����release���ӵ�������ʲô���ں�,ע�����release��һ������Ŀ�
        pAPTR2->print_();
        pAPTR->print_();
    }
}

void auto_ptr_test3() {
    std::auto_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));
    if (pAPTR.get()) {
#if 0//���������smart_ptr_test���ڴ治�ᱻ�ͷţ�ֻ��auto_ptrָ�뱻����˶���
        pAPTR.release();
#endif // 0
        //������ȷ������
        delete pAPTR.release();
        //or:pAPTR.reset();
    }
}


//boost::scoped_ptr���Է���Ĺ��������ڴ����,��������Ȩ,auto_ptr���������
void scoped_ptr_test() {
    boost::scoped_ptr<smart_ptr_test> pAPTR(new smart_ptr_test(1));
    if (pAPTR.get()) {
        pAPTR->print_();
        pAPTR.get()->info_ = "Addition";
        pAPTR->print_();
        (*pAPTR).info_ += " other";
        pAPTR->print_();
        //pAPTR.release();           // error:scoped_ptr û�� release ����
        std::auto_ptr<smart_ptr_test> pAPTR2;
        //pAPTR2 = pAPTR;        // error:scoped_ptr û������ operator=�����ᵼ������Ȩת��
    }
}

//boost::shared_ptr��ר�����ڹ�������Ȩ��,��ΪҪ��������Ȩ�����ڲ�ʹ�������ü���
void use_shaded_ptr(boost::shared_ptr<smart_ptr_test> p) {  // ����ʹ�� reference (�� const reference)
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
    //pAPTR.release();// error: shared_ptrû��release����
}

//boost::scoped_array���ڹ���̬�����,��������Ȩ
void scoped_array_test() {
    boost::scoped_array<smart_ptr_test> sArr(new smart_ptr_test[2]); // ʹ���ڴ���������ʼ��
    if (sArr.get()) {
        sArr[0].print_();
        sArr.get()[0].info_ = "Addition";
        sArr[0].print_();
        //(*sArr)[0].info_ += " other";            // error:scoped_ptr û������operator*
        //sArr[0].release();                       // error:û�� release ����
        boost::scoped_array<smart_ptr_test> sArr2;
        //sArr2 = sArr;                            // error:û������ operator=
    }
}

//boost::weak_ptr��boost::shared_ptr�Ĺ۲��ߣ�Observer������
//boost::weak_ptr ֻ�� boost::shared_ptr �������ã������ı������ü�����
//�����۲�� boost::shared_ptr ʧЧ����Ӧ�� boost::weak_ptr Ҳ��ӦʧЧ
void weak_ptr_test() {
    boost::weak_ptr<smart_ptr_test> weak_p;
    boost::shared_ptr<smart_ptr_test> shared_p(new smart_ptr_test(1));
    std::cout << "boost::shared_ptr UseCount: " << shared_p.use_count() << std::endl;
    weak_p = shared_p;
    std::cout << "boost::shared_ptr UseCount: " << shared_p.use_count() << std::endl;
}