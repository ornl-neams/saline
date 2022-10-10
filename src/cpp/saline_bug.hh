#ifndef SALINE_BUG_HH_
#define SALINE_BUG_HH_

#ifndef SALINE_DEBUG_OUTPUT
#define SALINE_DEBUG_OUTPUT 0
#define saline_debug_call(c)
#else
#define saline_debug_call(c) c
#endif

// default to c++ compiler
/*
 * @brief Debug functionality for including debug output \
 * in your source without a performance penalty.
 * This is an include ONLY header. There is no library to link against
 * Each function is preprocessed to be included or not.
 * If SALINE_DEBUG_OUTPUT is defined the MACROs are included.
 *
 * Available MACROs:
 * saline(arg) - std::cerr << arg - Pushes content to stderr
 * saline_line(arg) - std::cerr << arg << std::endl - Push newline terminated
 * content to stderr
 * saline(arg) - std::cerr << arg - Pushes content to stderr
 * saline_warning(arg) - std::cerr << arg << std::endl - Push newline terminated
 * content to stderr
 * saline_tagged_line(arg) - Same as saline_line, prefixed with FILE and LINE
 * saline_tagged(arg) - Same as saline, prefixed with FILE and LINE
 * saline_tagged_warning(arg) - Same as saline_warning, prefixed with FILE and Line
 * saline_block(block) - block - Simply places code block in preprocessor
 */
#include <iostream>
#include <cstdio>
#if SALINE_DEBUG_OUTPUT & 1
#define saline_line(arg) std::cerr << arg << std::endl
#define saline_flush_line(arg)       \
    std::cerr << arg << std::endl; \
    fflush(stderr)
#define saline(arg) std::cerr << arg
#define saline_warning(arg) std::cerr << arg << std::endl
#define saline_tagged_warning(arg) \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg << std::endl
#define saline_flush_warning(arg)    \
    std::cerr << arg << std::endl; \
    fflush(stderr)
#define saline_tagged_line(arg) \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg << std::endl
#define saline_flush_tagged_line(arg)                                       \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg << std::endl; \
    fflush(stderr)
#define saline_tagged(arg) std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg
#define saline_flush_tagged_warning(arg)                                    \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << arg << std::endl; \
    fflush(stderr)
#define saline_tagged_block(block)                                  \
    std::cerr << __FILE__ << ":" << __LINE__ << ":" << std::endl; \
    block
#define saline_block(block) block
#else
#define saline(arg)
#define saline_line(arg)
#define saline_flush_line(arg)
#define saline_warning(arg)
#define saline_flush_warning(arg)
#define saline_tagged_line(arg)
#define saline_flush_tagged_line(arg)
#define saline_tagged(arg)
#define saline_tagged_warning(arg)
#define saline_flush_tagged_warning(arg)
#define saline_tagged_block(block)
#define saline_block(block)
#endif /* SALINE_DEBUG_OUTPUT */

#include <stdexcept>
#include <sstream>
#ifndef SALINE_DBC
#define SALINE_DBC 0
#endif

//
// Defined all levels of SALINE_DBC
// SALINE_DBC = 1 enables Require
// SALINE_DBC = 2 enables Check
// SALINE_DBC = 4 enables Remember & Ensure
// SALINE_DBC = 7 enables all
// Insist is always enabled
#if SALINE_DBC & 1
#define saline_require(c)                                                 \
    if (!(c))                                                           \
    {                                                                   \
        std::ostringstream stream;                                      \
        stream << __FILE__ << ":" << __LINE__ << " saline_require(" << #c \
               << ") failed." << std::endl;                             \
        throw std::runtime_error(stream.str());                         \
    }
#else
#define saline_require(c)
#endif
#if SALINE_DBC & 2
#define saline_check(c)                                                 \
    if (!(c))                                                         \
    {                                                                 \
        std::ostringstream stream;                                    \
        stream << __FILE__ << ":" << __LINE__ << " saline_check(" << #c \
               << ") failed." << std::endl;                           \
        throw std::runtime_error(stream.str());                       \
    }
#else
#define saline_check(c)
#endif
#if SALINE_DBC & 4
#define saline_ensure(c)                                                 \
    if (!(c))                                                          \
    {                                                                  \
        std::ostringstream stream;                                     \
        stream << __FILE__ << ":" << __LINE__ << " saline_ensure(" << #c \
               << ") failed." << std::endl;                            \
        throw std::runtime_error(stream.str());                        \
    }
#define saline_remember(c) c
#else
#define saline_ensure(c)
#define saline_remember(c)
#endif

#define saline_insist(c, msg)                                           \
    if (!(c))                                                         \
    {                                                                 \
        std::ostringstream stream;                                    \
        stream << __FILE__ << ":" << __LINE__ << "saline_insist(" << #c \
               << ") failed with this message:" << std::endl          \
               << msg << std::endl;                                   \
        throw std::runtime_error(stream.str());                       \
    }

#define saline_not_implemented(msg)                             \
    {                                                         \
        std::ostringstream stream;                            \
        stream << __FILE__ << ":" << __LINE__ << " : " << msg \
               << " is not implemented. " << std::endl;       \
        throw std::runtime_error(stream.str());               \
    }

/// set default timing to off
#ifndef SALINE_TIMING
#define SALINE_TIMING 0
#endif

/// provide for necessary includes if timing is on
#if SALINE_TIMING
#include <chrono>
#include <ctime>
namespace saline
{
class Timer
{
  private:
    bool                                  m_running;
    std::chrono::steady_clock::time_point m_start;
    std::chrono::steady_clock::time_point m_end;
    std::chrono::nanoseconds              m_duration;
    size_t                                m_intervals;

  public:
    Timer() : m_running(false), m_duration(0), m_intervals(0) {}
    void start()
    {
        saline_check(!m_running);
        m_running = true;
        m_intervals++;
        m_start = std::chrono::steady_clock::now();
    }
    void stop()
    {
        saline_check(m_running);
        m_end     = std::chrono::steady_clock::now();
        m_running = false;
        m_duration +=
            std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start);
    }

    double duration() const
    {
        saline_check(!m_running);
        return m_duration.count();
    }
    size_t intervals() const { return m_intervals; }
};  // class Timer
}  // namespace saline
#endif

#if SALINE_TIMING > 0
#define saline_timer(name) saline::Timer name
#define saline_timer_start(name) name.start()
#define saline_timer_stop(name) name.stop()
#define saline_timer_block(content) content
#else
#define saline_timer(name)
#define saline_timer_start(name)
#define saline_timer_stop(name)
#define saline_timer_block(content)
#endif
#if SALINE_TIMING > 1
#define saline_timer_2(name) saline::Timer name
#define saline_timer_start_2(name) name.start()
#define saline_timer_stop_2(name) name.stop()
#define saline_timer_block_2(content) content
#else
#define saline_timer_2(name)
#define saline_timer_start_2(name)
#define saline_timer_stop_2(name)
#define saline_timer_block_2(content)
#endif

#if SALINE_TIMING > 2
#define saline_timer_3(name) saline::Timer name
#define saline_timer_start_3(name) name.start()
#define saline_timer_stop_3(name) name.stop()
#define saline_timer_block_3(content) content
#else
#define saline_timer_3(name)
#define saline_timer_start_3(name)
#define saline_timer_stop_3(name)
#define saline_timer_block_3(content)
#endif

#endif /* SALINE_BUG_HH_*/
