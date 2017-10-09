/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <pthread.h>

#define ACE_LOG(__l, __a...)  \
  ace::common::Log::get().write(ace::common::Log::__l, __FILE__, __LINE__, __a)

namespace ace {
namespace common {

class Log {
 public:

  // Constants

  static const char Plain[];
  static const char Bold[];
  static const char Faint[];
  static const char Italic[];
  static const char Underline[];
  static const char Red[];
  static const char Yellow[];
  static const char Green[];
  static const char Blue[];
  static const char Magenta[];
  static const char Cyan[];
  static const char White[];

  // Level

  enum Level {
    None    = 0,
    Error   = 1,
    Warning = 2,
    Info    = 3,
    Debug   = 4,
    Extra   = 5,
    All     = 6
  };

 public:

  ~Log();

  static Log & get();

  bool isFileStream();
  bool changeFileStreamDestination(std::string const & dest);

  template<typename... Args>
  void write(Level l, std::string const & f, int n, Args const & ... a);

  bool recycle();

  static Level parseLogLevel(std::string const & l);
  void setLogLevel(const Level l);

 private:

  class Channel {
   public:

    Channel();
    explicit Channel(Channel const & o) = delete;
    ~Channel();

    bool isFileStream() const;
    bool changeFileStreamDestination(std::string const & dest);

    template<typename T, typename... Args>
      void write(T const & e, Args const & ... args);

   private:

    static std::string defaultOutputPath();

    std::ostream & toStream(const char * description);
    void write();

    std::ofstream   m_file;
    std::string     m_fileName;
    std::ostream &  m_outStream;
  };

  using Channels = std::map<pthread_t, std::shared_ptr<Channel> >;

  Log();

  Channel & channel(bool recycle = false);

  void doHeader(Channel & c, Level l, std::string const & f, int n,
                 std::ostringstream & o);

  pthread_mutex_t m_lock;
  Level           m_level;
  Channels        m_channels;
};

inline bool
Log::Channel::isFileStream() const {
  return &m_outStream == &m_file;
}

template<typename T, typename... Args>
void
Log::Channel::write(T const & e, Args const & ... args) {
  m_outStream << e;
  write(args...);
}

inline void
Log::Channel::write() {
  m_outStream << std::endl;
}

template<typename... Args>
void
Log::write(Level l, std::string const & f, int n, Args const & ... a) {
  if (__builtin_expect(l <= m_level and m_level != Level::None, 0)) {
    std::ostringstream oss;
    Channel & c = channel();
    doHeader(c, l, f, n, oss);
    c.write(oss.str(), a...);
  }
}

} // namespace common
} // namespace ace
