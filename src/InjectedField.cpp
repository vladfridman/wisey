//
//  InjectedField.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/InjectedField.hpp"

using namespace std;
using namespace wisey;

InjectedField::InjectedField(const IType* type,
                             const IType* injectedType,
                             std::string name,
                             InjectionArgumentList injectionArgumentList,
                             int line) :
mType(type),
mInjectedType(injectedType),
mName(name),
mInjectionArgumentList(injectionArgumentList),
mLine(line) { }

InjectedField::~InjectedField() {
  // Injection arguments are deleted with field delcarations
  mInjectionArgumentList.clear();
  if (mInjectedType && mInjectedType->isArray()) {
    delete ((const ArraySpecificOwnerType*) mInjectedType)->getArraySpecificType();
  }
}

int InjectedField::getLine() const {
  return mLine;
}

const IType* InjectedField::getType() const {
  return mType;
}

const IType* InjectedField::getInjectedType() const {
  return mInjectedType == NULL ? mType : mInjectedType;
}

string InjectedField::getName() const {
  return mName;
}

InjectionArgumentList InjectedField::getInjectionArguments() const {
  return mInjectionArgumentList;
}

bool InjectedField::isAssignable() const {
  return true;
}

bool InjectedField::isConstant() const {
  return false;
}

bool InjectedField::isField() const {
  return true;
}

bool InjectedField::isMethod() const {
  return false;
}

bool InjectedField::isStaticMethod() const {
  return false;
}

bool InjectedField::isMethodSignature() const {
  return false;
}

bool InjectedField::isLLVMFunction() const {
  return false;
}

bool InjectedField::isFixed() const {
  return false;
}

bool InjectedField::isInjected() const {
  return true;
}

bool InjectedField::isReceived() const {
  return false;
}

bool InjectedField::isState() const {
  return false;
}

void InjectedField::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "  inject ";
  
  (mInjectedType ? mInjectedType : mType)->printToStream(context, stream);
  stream << " " << getName();
  
  if (!mInjectionArgumentList.size()) {
    stream << ";" << endl;
    return;
  }
  
  for (InjectionArgument* argument : mInjectionArgumentList) {
    stream << ".";
    argument->printToStream(context, stream);
  }
  stream << ";" << endl;
}
