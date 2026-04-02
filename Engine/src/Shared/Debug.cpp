#include "fow/Shared/Debug.hpp"
#include "fow/Shared/GameState.hpp"

#include <fstream>

#ifndef _WIN32

#include <X11/Xlib.h>
#include <X11/Xutil.h>

static void X11DrawWrappedText(Display* display, const Window window, const GC gc, const int x, const int y, const int maxWidth, const fow::String& text) {
    const auto fontInfo = XQueryFont(display, XGContextFromGC(gc));
    const int lineHeight = fontInfo->ascent + fontInfo->descent + 5; // Font height + spacing

    std::stringstream ss(text.as_std_str());
    std::string word;
    std::string line;
    auto currentY = y;

    while (ss >> word) {
        std::string testLine = line + (line.empty() ? "" : " ") + word;
        const int width = XTextWidth(fontInfo, testLine.c_str(), testLine.length());

        if (width > maxWidth && !line.empty()) {
            XDrawString(display, window, gc, x, currentY, line.c_str(), line.length());
            line = word;
            currentY += lineHeight;
        } else {
            line = testLine;
        }
    }
    XDrawString(display, window, gc, x, currentY, line.c_str(), line.length());
}

static void X11ShowPopupMessage(const fow::String& message) {
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        return;
    }

    constexpr auto width = 600;
    constexpr auto height = 400;
    constexpr auto text_x = 5;
    constexpr auto text_y = 15;

    const auto screen = DefaultScreen(display);
    const auto window = XCreateSimpleWindow(
        display, RootWindow(display, screen),
        100, 100, width, height, 1,
        BlackPixel(display, screen), WhitePixel(display, screen)
    );

    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
    XStoreName(display, window, "Fatal Error");

    XEvent event;
    bool running = true;

    auto gc = DefaultGC(display, screen);

    while (running) {
        XNextEvent(display, &event);

        if (event.type == Expose) {
            X11DrawWrappedText(display, window, gc, text_x, text_y, width - text_x, message);

            XDrawString(display, window, gc, 5, height - 20, "Press any key to close this message", 35);
        }

        if (event.type == KeyPress) {
            running = false;
        }
    }

    XCloseDisplay(display);
}

#endif

namespace fow::Debug {
    static std::ofstream s_log_output;
    static LogMessageCallback s_message_sent_callback;
    static LogLevel s_minimum_level =
#ifndef NDEBUG
        LogLevel::Debug
#else
        LogLevel::Warning
#endif
;

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <WinUser.h>

    static HANDLE s_console_handle = INVALID_HANDLE_VALUE;
    static WORD s_default_color = 0;
#endif

    void Initialize(const Path& log_base_dir) {
        if (!s_log_output.is_open()) {
            s_log_output.open((log_base_dir / std::format("log_{:%Y.%m.%d}.txt", Time::system_clock::now())).as_std_path());
        }
#ifdef _WIN32
        if (s_console_handle == INVALID_HANDLE_VALUE) {
            s_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO info;
            GetConsoleScreenBufferInfo(s_console_handle, &info);
            s_default_color = info.wAttributes;
        }
#endif
    }

    void Terminate() {
        if (s_log_output.is_open()) {
            s_log_output.flush();
            s_log_output.close();
        }
    }

    void SetMessageSentCallback(const LogMessageCallback& callback) {
        s_message_sent_callback = callback;
    }

    void SetMinimumLogLevel(const LogLevel level) {
        s_minimum_level = level;
    }

    void Log(const LogLevel level, const String& message, const std::source_location& location) {
        if (level < s_minimum_level) {
            return;
        }

        const auto timestamp = Time::Now();

        std::print("[");
#ifdef _WIN32
        if (s_console_handle != INVALID_HANDLE_VALUE) {
            WORD wColor = s_default_color | FOREGROUND_INTENSITY;
            switch (level) {
                case LogLevel::Debug:   wColor = FOREGROUND_BLUE | FOREGROUND_GREEN; break;
                case LogLevel::Warning: wColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
                case LogLevel::Error:
                case LogLevel::Fatal:   wColor = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
                default: wColor = s_default_color; break;
            }
            SetConsoleTextAttribute(s_console_handle, wColor);
        }
        std::print("{}", level);
        if (s_console_handle != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(s_console_handle, s_default_color);
        }
#else
        switch (level) {
            case LogLevel::Debug:   std::print("\x1B[36;1m{}\x1B[0m", level); break;
            case LogLevel::Info:    std::print("\x1B[1m{}\x1B[0m"   , level); break;
            case LogLevel::Warning: std::print("\x1B[33;1m{}\x1B[0m", level); break;
            case LogLevel::Error:
            case LogLevel::Fatal:   std::print("\x1B[31;1m{}\x1B[0m", level); break;
        }
#endif
        std::println("] - {:%c} - \"{}\":{} ==> {}", timestamp, location.file_name(), location.line(), message);
        if (s_log_output.is_open()) {
            std::println(s_log_output, "[{}] - {:%c} - \"{}\":{} ==> {}", level, timestamp, location.file_name(), location.line(), message);
#ifndef NDEBUG
            s_log_output.flush();
#endif
        }

        if (s_message_sent_callback != nullptr) {
            s_message_sent_callback(level, timestamp, message, location);
        }
        if (level == LogLevel::Fatal) {
#ifndef NDEBUG
    #ifdef _WIN32
            MessageBoxA(nullptr, message.as_cstr(), "Fatal error!", MB_OK | MB_ICONERROR);
    #else
            X11ShowPopupMessage(message);
    #endif
#endif
            CrashGame(1);
        }
    }
    bool Assert(const bool condition, const String& fail_message, const std::source_location& location) {
        if (!condition) {
            Log(LogLevel::Error, fail_message, location);
        }
        return !condition;
    }

    bool AssertWarn(const bool condition, const String& fail_message, const std::source_location& location) {
        if (!condition) {
            Log(LogLevel::Warning, fail_message, location);
        }
        return !condition;
    }

    bool AssertFatal(const bool condition, const String& fail_message, const std::source_location& location) {
        if (!condition) {
            Log(LogLevel::Fatal, fail_message, location);
        }
        return !condition;
    }
}
