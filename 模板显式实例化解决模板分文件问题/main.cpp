#include "test_class_template.hpp"
#include "test_function_template.hpp"

int main()
{
    f_t(16); // int 
    f_t('A'); // char
    f_t(3.14); // double 
    // f_t("1"); // 链接错误

    N::X1<int> x1;
    x1.f1();
    // x1.f2(); // 链接错误

    N::X1<double> x4;
    x4.f1();
    // x4.f2(); // 链接错误

    N::X2<int> x2;
    x2.f1();
    x2.f2();

    N::X2<double> x3; 
    // x3.f1(); // 链接错误
}