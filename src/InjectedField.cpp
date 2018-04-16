//
//  InjectedField.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/InjectedField.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/ThreadOwner.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

InjectedField::InjectedField(const IType* type,
                             const IType* injectedType,
                             std::string name,
                             InjectionArgumentList injectionArgumentList,
                             std::string sourceFileName,
                             int line) :
mType(type),
mInjectedType(injectedType),
mIsArrayType(injectedType->isArray()),
mName(name),
mInjectionArgumentList(injectionArgumentList),
mSourceFileName(sourceFileName),
mLine(line) {
  assert(injectedType);
}

InjectedField::~InjectedField() {
  // Injection arguments are deleted with field delcarations
  mInjectionArgumentList.clear();
  if (mIsArrayType) {
    delete ((const ArraySpecificOwnerType*) mInjectedType)->getArraySpecificType();
  }
}

int InjectedField::getLine() const {
  return mLine;
}

const IType* InjectedField::getType() const {
  return mType;
}

string InjectedField::getName() const {
  return mName;
}

Value* InjectedField::inject(IRGenerationContext& context) const {
  if (mInjectedType->isReference()) {
    Log::e(mSourceFileName, mLine, "Injected fields must have owner type denoted by '*'");
    exit(1);
  }
  
  return mInjectedType->inject(context, mInjectionArgumentList, mLine);
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
  
  mInjectedType->printToStream(context, stream);
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
