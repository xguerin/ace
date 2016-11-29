/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <ace/common/Log.h>
#include <ace/filesystem/Directory.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#define FILENAME_LEN 24
#define FILELINE_LEN 5

namespace ace {
namespace common {

// Log constants

const char Log::Plain[]      = "\x1b[0m";
const char Log::Bold[]       = "\x1b[1m";
const char Log::Faint[]      = "\x1b[2m";
const char Log::Italic[]     = "\x1b[3m";
const char Log::Underline[]  = "\x1b[4m";
const char Log::Red[]        = "\x1b[31m";
const char Log::Yellow[]     = "\x1b[32m";
const char Log::Green[]      = "\x1b[33m";
const char Log::Blue[]       = "\x1b[34m";
const char Log::Magenta[]    = "\x1b[35m";
const char Log::Cyan[]       = "\x1b[36m";
const char Log::White[]      = "\x1b[37m";

static const char levelCode[7] = {
  'N', // None
  'E', // Error
  'W', // Warning
  'I', // Info
  'D', // Debug
  'X', // Extra
  'A'  // All
};

// Channel class

Log::Channel::Channel()
  : m_file()
  , m_fileName(defaultOutputPath())
  , m_outStream(toStream(getenv("ACE_LOG_STREAM"))) {
  if (isFileStream()) m_file.open(m_fileName);
}

Log::Channel::~Channel() {
  if (isFileStream()) m_file.close();
}

bool
Log::Channel::changeFileStreamDestination(std::string const & dest) {
  if (not isFileStream()) return false;
  m_file.close();
  m_file.clear();
  if (::rename(m_fileName.c_str(), dest.c_str()) != 0) return false;
  m_file.open(dest);
  if (m_file.fail()) return false;
  m_fileName = dest;
  return true;
}

std::string
Log::Channel::defaultOutputPath() {
  std::stringstream ss;
#if defined(__linux__)
  char name[32] = { 0 };
  pthread_getname_np(pthread_self(), name, 32);
  ss << name << "_";
  ss << syscall(SYS_gettid) << ".log";
#elif defined(__OpenBSD__)
  ss << "thread_";
  ss << getthrid() << ".log";
#elif defined(__MACH__)
  uint64_t id;
  pthread_threadid_np(pthread_self(), &id);
  ss << "thread_";
  ss << id << ".log";
#else
#error "Operating system not supported"
#endif
  return (fs::Directory().path() / ss.str()).toString();
}

std::ostream &
Log::Channel::toStream(const char * description) {
  if (description == 0) {
    return std::cout;
  } else if (strcmp(description, "STDOUT") == 0) {
    return std::cout;
  } else if (strcmp(description, "STDERR") == 0) {
    return std::cerr;
  } else if (strcmp(description, "FILE") == 0) {
    return m_file;
  } else {
    return std::cout;
  }
}

// Log class

Log::Log()
  : m_lock(PTHREAD_MUTEX_INITIALIZER)
  , m_level()
  , m_channels() {
  const char * d = getenv("ACE_LOG_LEVEL");
  if (d == 0)                         m_level = Warning;
  else if (strcmp(d, "ERROR") == 0)   m_level = Error;
  else if (strcmp(d, "WARNING") == 0) m_level = Warning;
  else if (strcmp(d, "INFO") == 0)    m_level = Info;
  else if (strcmp(d, "DEBUG") == 0)   m_level = Debug;
  else if (strcmp(d, "EXTRA") == 0)   m_level = Extra;
  else if (strcmp(d, "ALL") == 0)     m_level = All;
  else                                m_level = None;
}

Log::~Log() { }

bool
Log::isFileStream() {
  return channel().isFileStream();
}

bool
Log::changeFileStreamDestination(std::string const & dest) {
  return channel().changeFileStreamDestination(dest);
}

Log::Channel &
Log::channel() {
  static __thread Channel * c = 0;
  if (c == 0) {
    pthread_t self = pthread_self();
    pthread_mutex_lock(& m_lock);
    /**
     * First clean-up the channel list
     */
    struct sched_param params;
    int policy;
    auto it = m_channels . begin(), cur = it;
    while (it != m_channels . end()) {
      switch (pthread_getschedparam(it -> first, & policy, & params)) {
        case ESRCH    : cur = it++;
                        m_channels . erase(cur);
                        break;
        default       : ++it;
                        break;
      }
    }
    /**
     * Then create a new channel for the thread
     */
    m_channels[self] = std::shared_ptr<Channel>(new Channel());
    c = m_channels[self] . get();
    pthread_mutex_unlock(& m_lock);
  }
  return *c;
}

void
Log::doHeader(Channel & c, Level l, std::string const & f, int n, std::ostringstream & oss) {
  char buffer[128] = { 0 };
  /**
   * Color the line depending on the log level
   */
  if (not c.isFileStream() and isatty(1)) switch (l) {
    case Error    : oss << Red;
                    break;
    case Warning  : oss << Yellow;
                    break;
    case Info     : oss << Blue;
                    break;
    default       : oss << Plain;
                    break;
  }
  /**
   * Add a timestamp
   */
  time_t rawtime;
  time(&rawtime);
  auto timeinfo = localtime(&rawtime);
  strftime(buffer, 128, "%T", timeinfo);
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  oss << buffer << "." << std::setw(6) << std::setfill('0') << std::right << tv.tv_usec << " ";
  oss << std::setfill(' ') << std::left;
  /**
   * Show the log level
   */
  oss << levelCode[l];
  oss << " ";
  /**
   * Write thread name
   */
  oss << std::setfill('.') << std::setw(15);
#if defined(__linux__)
  pthread_getname_np(pthread_self(), buffer, 40);
  oss << buffer << std::setfill(' ') << " ";
#elif defined(__OpenBSD__)
  oss << getthrid() << std::setfill(' ') << " ";
#elif defined(__MACH__)
  uint64_t id;
  pthread_threadid_np(pthread_self(), &id);
  oss << id << std::setfill(' ') << " ";
#else
#error "Operating system not supported"
#endif
  /**
   * Write the file info
   */
  size_t len = f.length();
  size_t off =(len > FILENAME_LEN) ? len - FILENAME_LEN : 0;
  std::string fnfo = f . substr(off) + ":" + std::to_string(n);
  oss
    << std::left
    << std::setfill('.')
    << std::setw(FILENAME_LEN + FILELINE_LEN + 1)
    << fnfo
    << std::left << std::setfill(' ') << std::setw(2)
    << "] ";
  /**
   * Reset the coloring to Plain
   */
  if (not c.isFileStream() and isatty(1)) oss << Plain;
}

Log &
Log::get() {
  static Log singleton;
  return singleton;
}

} // namespace common
} // namespace ace

