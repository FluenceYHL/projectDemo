#ifndef LOGCALL_H
#define LOGCALL_H
#include <iostream>
#define _PATH __FILE__ , __LINE__

namespace YHL {

inline void logCall(const char* record) {
    std::cout << record << std::endl;
}

}

#endif // LOGCALL_H
