#include "any.h"

// 关键方法, 虚表会定义在关键方法（第一个虚函数）被定义的源文件中，作为共享
// 如果 inline 在 .h 文件，极其容易被扩散到 include 该 .h 的所有文件中, 会增加连接器的处理时间

YHL::any::base_holder::~base_holder() {}

const char* YHL::bad_any_cast::what () noexcept {
    return "bad_cast Happens in YHL::any";
}
