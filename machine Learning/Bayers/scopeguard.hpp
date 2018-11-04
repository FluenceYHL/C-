#ifndef SCOPEGUARD_H
#define SCOPEGUARD_H
#include <functional>
#include <mutex>

/* 使用说明
    std::ofstream Out("doctor.txt");
    ON_SCOPE_EXIT([&] {
        Out.close();
        YHL::logCall("文件 doctor 已关闭\n");
    });
 */

namespace YHL {

    class ScopeGuard final {
        using funType = std::function<void()> ;

    private:
        funType onExitScope ;
        bool dismissed ;

        std::mutex mtx;  // 是否需要枷锁呢 ?

    public:
        explicit ScopeGuard(funType _onExitScope, funType acquire = []{})
            : onExitScope(_onExitScope), dismissed(false)
        { acquire() ; }

        ~ScopeGuard() noexcept {
            std::lock_guard<std::mutex> lck(mtx);

            if ( dismissed == false ) {
                dismissed = true;
                onExitScope();
            }
        }
        // 2018.10.5 更新转移构造
        ScopeGuard(ScopeGuard&& other)
            : onExitScope(std::move(other.onExitScope)),
                 dismissed(other.dismissed) {
            other.Dismiss(true);
        }

        ScopeGuard& operator=(ScopeGuard&& other) {
            if( this not_eq &other ) {
                this->onExitScope = other.onExitScope;
                this->dismissed = other.dismissed;
                other.dismissed = true;
            }
            return *this;
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
    #define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)
    #define ON_SCOPE_EXIT2(callback, acquire) YHL::ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback, acquire)

}

namespace SFINAE {
    typedef char yes ;
    typedef struct{ char str[2] ; } no ;

    template<typename T>
    struct has_iterator {
        template<typename other>
        static yes judge(typename other::iterator* x) ;
        template<typename other>
        static no judge(...) ;
        const static bool value = sizeof(judge<T>(0)) == sizeof(yes) ;
    } ;
}


#endif // SCOPEGUARD_H