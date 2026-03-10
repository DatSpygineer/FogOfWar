#include "util/Process.hpp"

#ifdef _WIN32
    #define c_popen _popen
    #define c_pclose _pclose
#else
    #define c_popen popen
    #define c_pclose pclose
#endif

Process::Process(const fow::Vector<fow::String>& command) : m_pProc(c_popen(fow::String::Join(command, ' ').as_cstr(), "r")) { }

bool Process::poll() const {
    fow::String dummy { };
    return poll(dummy);
}
bool Process::poll(fow::String& stdoutValue) const {
    const auto result = fgets(stdoutValue.data(), static_cast<int>(stdoutValue.capacity()), m_pProc) != nullptr;
    stdoutValue.recalculate_size();
    return result;
}

int Process::run_and_wait() {
    while (poll()) { }
    return close();
}

int Process::close() {
    if (m_pProc != nullptr) {
        m_iExitCode = c_pclose(m_pProc);
        m_pProc = nullptr;
    }
    return m_iExitCode;
}

bool Process::open(const fow::Vector<fow::String>& command) {
    if (m_pProc != nullptr) {
        close();
    }
    m_pProc = c_popen(fow::String::Join(command, ' ').as_cstr(), "r");
    return m_pProc != nullptr;
}
