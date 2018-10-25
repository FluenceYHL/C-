#ifndef SCOPEGUARD_H
#define SCOPEGUARD_H
#include <functional>
#include <mutex>
#include <boost/noncopyable.hpp>

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
    #define ON_EXIT_SCOPE(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)
    #define ON_SCOPE_EXIT2(callback, acquire) YHL::ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback, acquire)

}

/*  使用说明
    std::ofstream out("RAII.txt");
    YHL::scopeGuard0< std::function<void()> > guard
            = YHL::makeGuard<std::function<void()> >([&]{
        std::cout << "文件已关闭\n\n";
        out.close ();
    });
    out << "YHL, good afternoon\n";
 */


namespace YHL {

    template<typename funType>
    class scopeGuard final : boost::noncopyable {
    private:
        funType onExitScope;
        bool dismissed;
        std::mutex mtx;

        explicit scopeGuard(funType _onExitScope, funType acquire = []{})
            : onExitScope(_onExitScope), dismissed(false)
        { acquire(); }
    public:

        ~scopeGuard() noexcept {
            std::lock_guard<std::mutex> lck(mtx);

            if(dismissed == false) {
                dismissed = true;
                onExitScope();
            }
        }
        // 转移构造
        scopeGuard(scopeGuard&& other)
            : onExitScope(other.onExitScope),
                dismissed(other.dismissed) {
            other.Dismiss (true);
        }
        // 转移操作符
        scopeGuard& operator=(scopeGuard&& other) {
            if( this not_eq &other ) {
                this->onExitScope = other.onExitScope;
                this->dismissed = other.dismissed;
                other.dismissed = true;
            }
            return *this;
        }

        void Dismiss(const bool _dismissed) {
            std::lock_guard<std::mutex> lck(mtx);
            dismissed = _dismissed;
        }

        static scopeGuard<funType> makeGuard(funType _onExitScope, funType acquire = []{}) {
            return scopeGuard<funType>(_onExitScope, acquire);
        }
    };

    /*
    template <typename funType>
    inline scopeGuard0<funType> makeGuard(funType onExitScope, funType acquire = []{}) {
        return scopeGuard0<funType>::makeGuard (onExitScope, acquire);
    }*/

    template <typename funType, typename... Args>
    inline scopeGuard<std::function<void()> > makeGuard(funType&& onExitScope, Args&&... args) {
        return scopeGuard<std::function<void()> >::makeGuard([&]{
            onExitScope(std::forward<Args>(args)...);
        });
    }

}

/*
namespace YHL {

    // 多参数 ? 暂时实现不了，直接在上面实现了
    template<typename funType, typename... Args>
    class scopeGuard final : boost::noncopyable {
    private:
        funType onExitScope;
        bool dismissed;
        std::mutex mtx;

        explicit scopeGuard(funType _onExitScope)
            : onExitScope(_onExitScope), dismissed(false)
        {}
    public:

        ~scopeGuard() noexcept {

            std::lock_guard<std::mutex> lck(mtx);

            if(dismissed == false) {
               dismissed = true;
                onExitScope();
            }
        }
         // 转移构造
        scopeGuard(scopeGuard&& other)
            : onExitScope(other.onExitScope),
             dismissed(other.dismissed) {
            other.Dismiss (true);
        }

        void Dismiss(const bool _dismissed) {
            std::lock_guard<std::mutex> lck(mtx);
            dismissed = _dismissed;
        }

        static scopeGuard<funType> makeGuard(funType _onExitScope []) {
            return scopeGuard<funType>(_onExitScope);
        }
    };

    template <typename funType, typename... Args>
    inline scopeGuard0<funType> makeGuard(funType onExitScope) {
        return scopeGuard0<funType>::makeGuard (onExitScope);
    }
}*/

#endif // SCOPEGUARD_H
