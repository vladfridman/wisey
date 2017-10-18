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
             unsigned long index,
             ExpressionList arguments) :
mFieldKind(fieldKind),
mType(type),
mName(name),
mIndex(index),
mArguments(arguments) { }

Field::~Field() {
  // Argument expressions are deleted with delcarations
  mArguments.clear();
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

unsigned long Field::getIndex() const {
  return mIndex;
}

ExpressionList Field::getArguments() const {
  return mArguments;
}

bool Field::isAssignable() const {
  return mFieldKind == FieldKind::STATE_FIELD || mFieldKind == FieldKind::RECEIVED_FIELD;
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
  
  if (!mArguments.size()) {
    stream << ";" << endl;
    return;
  }
  
  stream << "(";
  for (IExpression* expression : mArguments) {
    expression->printToStream(context, stream);
    if (expression != mArguments.at(mArguments.size() - 1)) {
      stream << ", ";
    }
  }
  stream << ");" << endl;
}
