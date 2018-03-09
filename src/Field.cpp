//
//  Field.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/Field.hpp"

using namespace std;
using namespace wisey;

Field::Field(FieldKind fieldKind,
             const IType* type,
             const IType* injectedType,
             std::string name,
             InjectionArgumentList injectionArgumentList) :
mFieldKind(fieldKind),
mType(type),
mInjectedType(injectedType),
mName(name),
mInjectionArgumentList(injectionArgumentList) { }

Field::~Field() {
  // Injection arguments are deleted with field delcarations
  mInjectionArgumentList.clear();
  if (mFieldKind == INJECTED_FIELD && mInjectedType && mInjectedType->isArray()) {
    delete ((const ArraySpecificOwnerType*) mInjectedType)->getArraySpecificType();
  }
}

FieldKind Field::getFieldKind() const {
  return mFieldKind;
}

const IType* Field::getType() const {
  return mType;
}

const IType* Field::getInjectedType() const {
  return mInjectedType == NULL ? mType : mInjectedType;
}

string Field::getName() const {
  return mName;
}

InjectionArgumentList Field::getInjectionArguments() const {
  return mInjectionArgumentList;
}

bool Field::isAssignable() const {
  return mFieldKind != FieldKind::FIXED_FIELD;
}

bool Field::isConstant() const {
  return false;
}

bool Field::isField() const {
  return true;
}

bool Field::isMethod() const {
  return false;
}

bool Field::isStaticMethod() const {
  return false;
}

bool Field::isMethodSignature() const {
  return false;
}

void Field::printToStream(IRGenerationContext& context, iostream& stream) const {
  switch (mFieldKind) {
    case FieldKind::FIXED_FIELD:
      stream << "  fixed ";
      break;
    case FieldKind::INJECTED_FIELD:
      stream << "  inject ";
     break;
    case FieldKind::RECEIVED_FIELD:
      stream << "  receive ";
     break;
    case FieldKind::STATE_FIELD:
      stream << "  state ";
      break;
  }

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

