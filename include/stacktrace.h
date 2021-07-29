#ifndef __STACKTRACE_H__
#define __STACKTRACE_H__
#include <ios>
#include <ostream>
#include <stdexcept>
#include "config.h"
#include "stacktrace_fwd.h"

#pragma message("Dumping backend info")

#ifdef USE_WINAPI_STACKTRACE
#pragma message ("Using WINAPI")
#include "detail/stacktrace_windows_impl.h"
#elif defined(USE_EXECINFO_STACKTRACE)
#pragma message("Using EXECINFO")
#include "detail/stacktrace_execinfo_impl.h"
#endif

#ifdef USE_LIBBFD_DECODE
#pragma message("Using LIBBFD")
#include "detail/stacktrace_libbfd_impl.h"
#endif

namespace stacktrace
{
    namespace detail {
        // ostream stuff
        inline int geti() {
            static int i = std::ios_base::xalloc();
            return i;
        }
    }

    // generates a raw stacktrace
    pointer_stacktrace stacktrace(size_t capture);
    symbol_stacktrace get_traced(const pointer_stacktrace& trace);

    class stack_aware_exception : std::runtime_error
    {
        symbol_stacktrace trace;
        int line;
        const char* file;
        const char* func;
        const char* long_func;
        std::string long_msg;

    public:
        stack_aware_exception(
            const char* what,
            int line,
            const char* file,
            const char* func,
            const char* long_func
        );

        inline const char* get_file() const { return file; }
        inline const char* get_func() const { return func; }
        inline const char* get_long_func() const { return long_func; }
        inline int get_line() const { return line; }
        inline const symbol_stacktrace& get_stacktrace() const { return trace; }
        inline std::string get_long_msg() const { return long_msg; }
        
        friend std::ostream& operator<<(std::ostream& os, const stack_aware_exception& dt);
    };

    inline std::ostream& shortexcept(std::ostream& os) { os.iword(detail::geti()) = 0; return os; }
    inline std::ostream& longexcept(std::ostream& os) { os.iword(detail::geti()) = 1; return os; }
    inline std::ostream& stacktrace(std::ostream& os) { os.iword(detail::geti()) = 2; return os; }
}

#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__ 
#endif

#define throw_dbg(exception, what, ...) throw exception(what, __LINE__, __FILE__, __func__, __PRETTY_FUNCTION__)
#endif