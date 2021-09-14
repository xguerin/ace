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

#include "Array.h"
#include "Object.h"
#include "Primitive.h"
#include <ace/common/Log.h>
#include <ace/common/Regex.h>
#include <ace/engine/Master.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ace { namespace tree { namespace utils {

bool processArrayString(std::string const& n, std::string const& in,
                        std::vector<std::string>& result);

tree::Value::Ref buildPrimitiveOrArray(std::string const& n,
                                       std::string const& content);

template<template<class, class> class C>
bool
apply(Value& v, C<std::string, std::allocator<std::string>> const& stms)
{
  if (not v.isObject()) {
    return false;
  }
  Object& obj = static_cast<Object&>(v);
  for (auto& stm : stms) {
    std::vector<std::string> parts;
    common::String::split(stm, '=', parts);
    if (parts.size() != 2) {
      return false;
    }
    std::string content(parts[1]);
    Value::Ref vrf = nullptr;
    if (not content.empty()) {
      vrf = buildPrimitiveOrArray("", content);
      if (vrf == nullptr) {
        return false;
      }
    }
    std::string pathValue("$." + parts[0]);
    tree::Path path = tree::Path::parse(pathValue);
    if (not obj.put(path, vrf)) {
      return false;
    }
  }
  return true;
}

template<typename T>
bool
parsePrimitive(Object const& r, std::string const& k, T& l)
{
  auto path = tree::Path::parse(k);
  if (not r.has(path)) {
    ACE_LOG(Info, "Path \"", path.toString(), "\" not found");
    return false;
  }
  tree::Value const& v = r.get(path);
  if (not v.isPrimitive()) {
    return false;
  }
  tree::Primitive const& w = static_cast<tree::Primitive const&>(v);
  l = w.value<T>();
  return true;
}

template<typename T>
void
parsePrimitive(Object const& r, std::string const& k, std::vector<T>& l)
{
  auto path = tree::Path::parse(k);
  if (not r.has(path)) {
    ACE_LOG(Info, "Path \"", path.toString(), "\" not found");
    return;
  }
  l.clear();
  tree::Value const& v = r.get(path);
  if (v.isPrimitive()) {
    tree::Primitive const& w = static_cast<tree::Primitive const&>(v);
    l.push_back(w.value<T>());
  } else if (v.type() == Value::Type::Array) {
    Array const& p = static_cast<tree::Array const&>(v);
    for (auto& e : p) {
      tree::Primitive const& w = static_cast<tree::Primitive const&>(*e);
      l.push_back(w.value<T>());
    }
  }
}

template<typename T>
bool
parseObject(Object const& r, std::string const& k, typename T::Ref& l)
{
  auto path = tree::Path::parse(k);
  if (not r.has(path)) {
    return false;
  }
  tree::Value const& v = r.get(path);
  if (not v.isObject()) {
    return false;
  }
  l = T::build(v);
  return true;
}

template<typename T>
void
parseObject(Object const& r, std::string const& k,
            std::vector<typename T::Ref>& l)
{
  auto path = tree::Path::parse(k);
  if (not r.has(path)) {
    return;
  }
  l.clear();
  tree::Value const& v = r.get(path);
  if (v.type() == Value::Type::Object) {
    l.push_back(T::build(v));
  } else if (v.type() == Value::Type::Array) {
    Array const& p = static_cast<tree::Array const&>(v);
    for (auto& e : p) {
      l.push_back(T::build(*e));
    }
  }
}

template<typename T>
bool
parsePlugin(Value const& v, std::string const& m, typename T::Ref& l)
{
  for (auto& p : MASTER.childrenForPath(m)) {
    if (MASTER.hasModelBuildersFor(p)) {
      for (auto& b : MASTER.modelBuildersFor(p)) {
        if (Path::parse(b.first).match(v.path())) {
          if (v.type() == Value::Type::Object) {
            l = typename T::Ref(reinterpret_cast<T*>(b.second(v)));
            return true;
          }
        }
      }
    }
  }
  return false;
}

template<typename T>
void
parsePlugin(Value const& v, std::string const& m,
            std::vector<typename T::Ref>& l)
{
  for (auto& p : MASTER.childrenForPath(m)) {
    if (MASTER.hasModelBuildersFor(p)) {
      for (auto& b : MASTER.modelBuildersFor(p)) {
        if (Path::parse(b.first).match(v.path())) {
          if (v.type() == Value::Type::Array) {
            Array const& p = static_cast<tree::Array const&>(v);
            for (auto& e : p) {
              l.push_back(typename T::Ref(reinterpret_cast<T*>(b.second(*e))));
            }
          }
        }
      }
    }
  }
}

void illegalValueAccess(std::string const& n);

}}}
