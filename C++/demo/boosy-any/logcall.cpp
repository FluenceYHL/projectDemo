#include "logcall.h"

inline void YHL::logCall(const char* record) {
    std::cout << record << std::endl;
}

inline void YHL::logCerrCall(const char* record) {
    std::cerr << record << std::endl;
}
