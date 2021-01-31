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

#include <ace/tree/Path.h>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace ace { namespace tree {

class Value
{
public:
  enum class Type
  {
    Undefined,
    Boolean,
    String,
    Float,
    Integer,
    Array,
    Object
  };

  using Ref = std::shared_ptr<Value>;

  using Callback = std::function<void(Value&)>;
  using ConstCallback = std::function<void(Value const&)>;

public:
  virtual ~Value() {}

  /**
   * @brief   Call the callback with each instance of contained values
   * @param c the callback
   */
  virtual void each(Callback const& c);

  /**
   * @brief   Call the callback with each instance of contained values
   * @param c the callback
   */
  virtual void each(ConstCallback const& c) const;

  /**
   * @brief   Dump the object into its readable format
   * @param o the stream to dump the object
   */
  virtual void dump(std::ostream& o, int l, bool i) const;

  /**
   * @brief   Merge two values together
   * @param o the other value
   */
  virtual void merge(Value const& o);

  /**
   * @brief Clone the value.
   */
  virtual Ref clone() const = 0;

  // Path crawlers

  virtual bool has(std::string const& k) const;

  virtual bool has(Path const& p) const;
  virtual bool has(Path const& p, Path::const_iterator const& i) const;

  virtual Value& operator[](std::string const& k);
  virtual Value const& operator[](std::string const& k) const;

  virtual Value& get(std::string const& k);
  virtual Value const& get(std::string const& k) const;

  virtual Value& get(Path const& p);
  virtual Value const& get(Path const& p) const;

  virtual void get(Path const& p, std::vector<Value::Ref>& r);
  virtual void get(Path const& p, std::vector<Value::Ref>& r) const;

  virtual void get(Path const& p, Callback const& op);
  virtual void get(Path const& p, ConstCallback const& op) const;

  virtual void get(Path const& p, Path::const_iterator const& i,
                   std::vector<Value::Ref>& r);
  virtual void get(Path const& p, Path::const_iterator const& i,
                   std::vector<Value::Ref>& r) const; // NOLINT

  virtual void get(Path const& p, Path::const_iterator const& i,
                   Callback const& op);
  virtual void get(Path const& p, Path::const_iterator const& i,
                   ConstCallback const& op) const;

  virtual void erase(std::string const& k);

  virtual void erase(Path const& p);
  virtual void erase(Path const& p, Path::const_iterator const& i);

  // Setters and getters

  void setName(std::string const& n);
  std::string const& name() const;

  Type type() const;
  template<typename T>
  static Type typeOf();

  bool isPrimitive() const;
  bool isArray() const;
  bool isObject() const;

  Value* parent();
  const Value* parent() const;

  virtual Path path() const;

protected:
  Value() = default;
  explicit Value(Value const&) = default;
  explicit Value(std::string const& name, Type type);

  virtual bool put(Path const& p, Path::const_iterator const& i,
                   Value::Ref const& r);

  std::string m_name;
  Type m_type;
  Value* m_parent;

  friend class Array;
  friend class Object;
};

template<typename T>
Value::Type
Value::typeOf()
{
  return Type::Undefined;
}

template<>
inline Value::Type
Value::typeOf<bool>()
{
  return Type::Boolean;
}

template<>
inline Value::Type
Value::typeOf<long>()
{
  return Type::Integer;
}

template<>
inline Value::Type
Value::typeOf<double>()
{
  return Type::Float;
}

template<>
inline Value::Type
Value::typeOf<std::string>()
{
  return Type::String;
}

}}
