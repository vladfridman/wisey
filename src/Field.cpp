//
//  Field.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Field.hpp"

using namespace std;
using namespace wisey;

Field::Field(FieldKind fieldKind,
             const IType* type,
             std::string name,
             InjectionArgumentList injectionArgumentList) :
mFieldKind(fieldKind),
mType(type),
mName(name),
mInjectionArgumentList(injectionArgumentList) { }

Field::~Field() {
  // Injection arguments are deleted with field delcarations
  mInjectionArgumentList.clear();
}

FieldKind Field::getFieldKind() const {
  return mFieldKind;
}

const IType* Field::getType() const {
  return mType;
}

string Field::getName() const {
  return mName;
}

InjectionArgumentList Field::getInjectionArguments() const {
  return mInjectionArgumentList;
}

bool Field::isAssignable() const {
  return mFieldKind == FieldKind::STATE_FIELD || mFieldKind == FieldKind::RECEIVED_FIELD;
}

ObjectElementType Field::getObjectElementType() const {
  return OBJECT_ELEMENT_FIELD;
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

  stream << getType()->getName() << " " << getName();
  
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

