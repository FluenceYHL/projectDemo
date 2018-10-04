#ifndef SCOPEGUARD_H
#define SCOPEGUARD_H
#include <functional>

/* 使用说明
    std::ofstream Out("doctor.txt");
    ON_SCOPE_EXIT([&] {
        Out.close();
        YHL::logCall("文件 doctor 已关闭\n");
    });
 */

namespace YHL {
    class ScopeGuard {
        using funType = std::function<void()> ;
    private:
        funType onExitScope ;
        bool dismissed ;
    public:
        explicit ScopeGuard(funType _onExitScope, funType acquire = []{})
            : onExitScope(_onExitScope), dismissed(false)
        { acquire() ; }
        ~ScopeGuard() noexcept {
            if ( dismissed == false )
                dismissed = true, onExitScope();
        }
        void Dismiss(const bool _dismissed) noexcept {
            dismissed = _dismissed ;
        }
    private:
        ScopeGuard(const ScopeGuard&) = delete ;
        ScopeGuard& operator=(const ScopeGuard&) = delete ;
    } ;

    // 宏定义控制 ON_SCOPE_EXIT 命名
    #define SCOPEGUARD_LINENAME_CAT(name, line) name##line
    #define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
    #define ON_SCOPE_EXIT(callback) YHL::ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)
    #define ON_SCOPE_EXIT2(callback, acquire) YHL::ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback, acquire)

}

#endif // SCOPEGUARD_H
