#ifndef __LIBBACKTRACE_HANDLE_H__
#define __LIBBACKTRACE_HANDLE_H__
#include "../stacktrace_fwd.h"
#include <backtrace.h>

namespace stacktrace
{
    namespace detail
    {
        class backtrace_wrapper
        {
            backtrace_state* state;

        public:
            inline backtrace_wrapper()
            {
                state = backtrace_create_state("/proc/self/exe", 0, nullptr, this);
            }

            inline pointer_stacktrace get_stack()
            {
                pointer_stacktrace st;

                get_with_callback([&, st](uintptr_t pc) mutable {
                    st.push_back(pc);
                });

                return st;
            }


            template<typename Callback>
            void get_with_callback(Callback cb)
            {
                if (!state)
                    return;
                backtrace_simple(
                    state, 0,
                    [](void* buf, uintptr_t pc) {
                        (*((Callback*)buf))(pc);
                        return 0;
                    },
                    [](void* buf, const char*, int) { (*((Callback*)buf))(0x0);}, &cb
                );
            }

            inline entry get_info(uintptr_t ptr)
            {
                entry e{ptr, 0, "UNK", "UNK"};
                if (!state)
                    return e;

                backtrace_pcinfo(
                    state, ptr,
                    [](void* data, uintptr_t pc, const char* fname, int lineno, const char* fn) {
                        entry* buf = (entry*)data;
                        std::string file{fname ? fname : "UNK"};
                        std::string func{fn ? fn : "UNK"};

                        if (file.size() == 0)
                            file = "UNK";
                        if (func.size() == 0)
                            func = "UNK";
                        else
                            demangle(func);

                        *buf = {pc, (size_t)lineno, file, func};
                        return 0;
                    },
                    nullptr, &e);

                return e;
            }
        };

        inline backtrace_wrapper get_instance()
        {
            static backtrace_wrapper* ptr = new backtrace_wrapper();
            return *ptr;
        }

    } // namespace detail
} // namespace stacktrace
#endif
