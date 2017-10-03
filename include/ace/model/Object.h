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

#ifndef ACE_MODEL_OBJECT_H_
#define ACE_MODEL_OBJECT_H_

#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/tree/Object.h>  // NOLINT
#include <ace/tree/Value.h>
#include <memory>
#include <string>

#define DEBUG_O(__o, __a...)    ACE_LOG(Debug, "[", __o->path(), "] ", __a)
#define DEBUG(__a...)           DEBUG_O(this, __a)

#define EXTRA_O(__o, __a...)    ACE_LOG(Extra, "[", __o->path(), "] ", __a)
#define EXTRA(__a...)           EXTRA_O(this, __a)

#define INFO_O(__o, __a...)     ACE_LOG(Info, "[", __o->path(), "] ", __a)
#define INFO(__a...)            INFO_O(this, __a)

#define ERROR_O(__o, __a...)    ACE_LOG(Error, "[", __o->path(), "] ", __a)
#define ERROR(__a...)           ERROR_O(this, __a)

#define WARNING_O(__o, __a...)  ACE_LOG(Warning, "[", __o->path(), "] ", __a)
#define WARNING(__a...)         WARNING_O(this, __a)

namespace ace {
namespace model {

class Object {
 public:

  Object();
  explicit Object(Object const & o);
  virtual ~Object() { }

  /**
   * @brief   Return object object ID
   * @return  return object ID
   */
  int id() const;

  /**
   * @brief   Return object object ID
   * @return  return object ID
   */
  std::string name() const;

  /**
   * @brief   Set the name of the object
   * @param n the name of the object
   */
  virtual void setName(std::string const & n);

  /**
   * @brief Get owning model
   * @return the owning model
   */
  virtual Object * owner();

  /**
   * @brief Get owning model
   * @return the owning model
   */
  virtual const Object * owner() const;

  /**
   * @brief   Get the Object's parent
   * @return  the object's parent
   */
  Object * parent();

  /**
   * @brief   Get the Object's parent
   * @return  the object's parent
   */
  const Object * parent() const;

  /**
   * @brief   Set the parent object of the object
   * @param p the parent object
   */
  virtual void setParent(const Object * p);

  /**
   * @brief   Get the path of the type in the model
   * @return  the the path of the type in the model
   */
  virtual tree::Path path(const bool local = false) const;

  /**
   * @brief   Check the validity of an object
   * @param t the Tokens for the objret
   * @return  true if the Object is valid, false otherwise
   */
  virtual bool checkModel(tree::Value const & t) const = 0;

  /**
   * @brief   Load an Object from a JSON Token
   * @param t the token
   */
  virtual void loadModel(tree::Value const & t) = 0;

  /**
   * @brief   Flatten the object
   * @return  true if the object has been flattened, false otherwise
   */
  virtual bool flattenModel();

  /**
   * @brief   Validate the object
   * @return  true if the dependencies are satisfied, false otherwise
   */
  virtual bool validateModel();

  /**
   * @brief   Inject inherited value for an instance if found
   * @param r the current root scope
   * @param i the instance requesting the injection
   * @param v the value of the instance
   * @return  true if the injection succeeded, false otherwise
   */
  virtual bool injectInherited(tree::Object const & r, Object const & o, tree::Value & v) const;

 protected:

  int         m_id;
  std::string m_name;
  Object *    m_parent;
};

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_OBJECT_H_
