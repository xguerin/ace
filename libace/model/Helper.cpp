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

#include <ace/model/Helper.h>
#include <ace/model/Model.h>
#include <ace/engine/Master.h>
#include <ace/tree/Utils.h>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

namespace ace { namespace model { namespace Helper {

bool
validate(std::string const& mp, std::string const& ver, const bool strict,
         tree::Value::Ref& svr)
{
  model::Model::Ref mdl;
  if (ver != version()) {
    ACE_LOG(Error, "Model version \"", ver, "\" and ACE version \"", version(),
            "\" mismatch");
    return false;
  }
  if (not MASTER.hasModel(mp)) {
    ACE_LOG(Error, "Cannot find model \"" + mp + "\"");
    return false;
  }
  if (not MASTER.isInlinedModel(mp)) {
    ACE_LOG(Error, "Model \"" + mp + "\" is not inline");
    return false;
  }
  if (not Model::check(nullptr, mp)) {
    ACE_LOG(Error, "Check model \"" + mp + "\" failed");
    return false;
  }
  mdl = Model::load(nullptr, mp);
  if (not mdl->flattenModel()) {
    ACE_LOG(Error, "Flatten model \"" + mp + "\" failed");
    return false;
  }
  if (not mdl->validateModel()) {
    ACE_LOG(Error, "Validate model \"" + mp + "\" failed");
    return false;
  }
  if (not mdl->checkInstance(*svr)) {
    ACE_LOG(Error, "Check instance failed");
    return false;
  }
  mdl->expandInstance(*svr);
  if (not mdl->flattenInstance(*svr)) {
    ACE_LOG(Error, "Flatten instance failed");
    return false;
  }
  if (not mdl->resolveInstance(*svr)) {
    ACE_LOG(Error, "Resolve instance failed");
    return false;
  }
  if (strict and not MASTER.unexpected().empty()) {
    for (auto const& v : MASTER.unexpected()) {
      ACE_LOG(Error, "Unexpected value: ", v);
    }
    return false;
  }
  return true;
}

bool
printAsTree(std::string const& mp, std::string const& ver)
{
  model::Model::Ref mdl;
  if (ver != version()) {
    ACE_LOG(Error, "Model version \"", ver, "\" and ACE version \"", version(),
            "\" mismatch");
    return false;
  }
  if (not MASTER.hasModel(mp)) {
    ACE_LOG(Error, "Cannot find model \"" + mp + "\"");
    return false;
  }
  if (not Model::check(nullptr, mp)) {
    ACE_LOG(Error, "Check model \"" + mp + "\" failed");
    return false;
  }
  mdl = Model::load(nullptr, mp);
  if (not mdl->flattenModel()) {
    ACE_LOG(Error, "Flatten model \"" + mp + "\" failed");
    return false;
  }
  if (not mdl->validateModel()) {
    ACE_LOG(Error, "Validate model \"" + mp + "\" failed");
    return false;
  }
  mdl->display(model::Coach::Branch::Root);
  return true;
}

bool
explain(std::string const& mp, std::string const& ver, std::string const& cp)
{
  model::Model::Ref mdl;
  if (ver != version()) {
    ACE_LOG(Error, "Model version \"", ver, "\" and ACE version \"", version(),
            "\" mismatch");
    return false;
  }
  if (not MASTER.hasModel(mp)) {
    ACE_LOG(Error, "Cannot find model \"" + mp + "\"");
    return false;
  }
  if (not Model::check(nullptr, mp)) {
    ACE_LOG(Error, "check model \"" + mp + "\" failed");
    return false;
  }
  mdl = Model::load(nullptr, mp);
  if (not mdl->flattenModel()) {
    ACE_LOG(Error, "Flatten model \"" + mp + "\" failed");
    return false;
  }
  if (not mdl->validateModel()) {
    ACE_LOG(Error, "Validate model \"" + mp + "\" failed");
    return false;
  }
  tree::Path path = tree::Path::parse(cp);
  return mdl->explain(path);
}

std::string
version()
{
  return std::string(ACE_VERSION);
}

}}}
