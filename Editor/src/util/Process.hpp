#ifndef FOW_EDITOR_PROCESS_HPP
#define FOW_EDITOR_PROCESS_HPP

#include <cstdio>
#include <cstdlib>

#include "fow/Shared.hpp"

class Process {
    FILE* m_pProc = nullptr;
    int m_iExitCode = 0;
public:
    Process() = default;
    explicit Process(const fow::Vector<fow::String>& command);
    ~Process() { close(); }

    [[nodiscard]] FOW_CONSTEXPR bool is_valid() const noexcept { return m_pProc != nullptr; }
    [[nodiscard]] bool poll() const;
    [[nodiscard]] bool poll(fow::String& stdoutValue) const;
    int run_and_wait();
    int close();
    [[nodiscard]] FOW_CONSTEXPR int exit_code() const noexcept { return m_iExitCode; }

    bool open(const fow::Vector<fow::String>& command);
};

#endif