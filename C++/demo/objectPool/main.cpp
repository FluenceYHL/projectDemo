#include <fstream>
#include "scopeguard.h"
#include "threadpool.h"
#include "objectpool.h"
#include "logcall.h"
#include "test.h"

int main() {
    YHL::objecePool<int> pool;
    {
        for(int i = 0;i < 10; ++i) {
            pool.emplace (new int(7));
            auto example = pool.get();
            std::cout << example.get () << std::endl;
        }
    }
    pool.emplace (new int(8));
    auto example = pool.get ();
    std::cout << example.get() << std::endl;

    std::cout << "size = " << pool.size () << std::endl;

    auto example2 = pool.get ();
    return 0;
}
