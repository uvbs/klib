#ifndef _klib_scope_guard_h
#define _klib_scope_guard_h

#include <functional>
#include <windows.h>

/*
Usage:

HANDLE h = 0;       //CreateFile(...);
scope_guard onExit([&] { 
    CloseHandle(h);
    MessageBox(NULL, _T("sss"), _T("sss"), 0);
});
onExit.dismiss();

ON_SCOPE_EXIT
(
    ::MessageBox(NULL, L"sdf", L"dsfdf", 0);
);


ON_SCOPE_EXIT
(
    CloseHandle(h);
);

*/

namespace klib {
namespace core {

/**
 * 范围退出时可执行相关操作()
 */
class scope_guard
{
public:
    explicit scope_guard(std::function<void()> onExitScope)
        : onExitScope_(onExitScope), dismissed_(false)
    { }

    ~scope_guard()
    {
        if(!dismissed_)
        {
            onExitScope_();
        }
    }

    void dismiss()
    {
        dismissed_ = true;
    }

private:
    std::function<void()> onExitScope_;
    bool dismissed_;

private:
    scope_guard(scope_guard const&);
    scope_guard& operator=(scope_guard const&);
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define ON_SCOPE_EXIT(FUNCTION) scope_guard SCOPEGUARD_LINENAME(EXIT, __LINE__)          \
    ([&]{  FUNCTION; });


}}


#endif
