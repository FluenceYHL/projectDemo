#ifndef LOGCALL_H
#define LOGCALL_H
#include <iostream>

namespace YHL {

inline void logCall(const char* record) {
    std::cout << record << std::endl;
}

}

#endif // LOGCALL_H
