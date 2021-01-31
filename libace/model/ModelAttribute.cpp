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

#include <ace/model/ModelAttribute.h>
#include <string>
#include <fcntl.h>

namespace ace { namespace model {

ModelAttribute::ModelAttribute(std::string const& n, bool o)
  : Attribute(n, o), m_value(), m_model(nullptr)
{}

bool
ModelAttribute::checkModel(tree::Value const& t) const
{
  std::string n = static_cast<tree::Primitive const&>(t).value<std::string>();
  DEBUG("Check \"", n, "\"");
  if (not Model::check(this, n)) {
    ERROR(ERR_INVALID_MODEL(n));
    return false;
  }
  return true;
}

void
ModelAttribute::loadModel(tree::Value const& t)
{
  m_value = static_cast<tree::Primitive const&>(t).value<std::string>();
  m_model = Model::load(this, m_value);
}

void
ModelAttribute::load(Attribute const& a)
{
  ModelAttribute const& ra = static_cast<ModelAttribute const&>(a);
  m_value = ra.m_value;
  m_model = ra.m_model;
}

bool
ModelAttribute::flattenModel()
{
  return m_model->flattenModel();
}

bool
ModelAttribute::validateModel()
{
  return m_model->validateModel();
}

ModelAttribute::operator tree::Checker::Pattern() const
{
  return tree::Checker::Pattern(tree::Value::Type::String, false);
}

ModelAttribute::operator std::string() const
{
  return m_value;
}

Attribute::Ref
ModelAttribute::clone() const
{
  ModelAttribute* attr = new ModelAttribute(*this);
  attr->m_model = Model::Ref(new Model(*m_model));
  attr->m_model->setParent(attr);
  return Attribute::Ref(attr);
}

bool
ModelAttribute::merge(Attribute const& b)
{
  if (not Attribute::merge(b)) {
    return false;
  }
  ModelAttribute const& mattr = static_cast<ModelAttribute const&>(b);
  if (m_model->filePath() == mattr.model().filePath()) {
    return true;
  }
  if (m_model->isAnAncestor(mattr.model())) {
    return true;
  } else if (mattr.model().isAnAncestor(*m_model)) {
    m_model = mattr.m_model;
    m_model->setParent(this);
    return true;
  }
  ERROR(ERR_NOT_A_BASE_CLASS_OF(mattr.model().name(), m_model->name()));
  return false;
}

std::string const&
ModelAttribute::value() const
{
  return m_value;
}

Model&
ModelAttribute::model()
{
  return *m_model;
}

Model const&
ModelAttribute::model() const
{
  return *m_model;
}

}}
