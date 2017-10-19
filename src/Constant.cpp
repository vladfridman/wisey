//
//  Constant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Constant.hpp"

using namespace std;
using namespace wisey;

Constant::Constant(const AccessLevel accessLevel,
                   const IType* type,
                   std::string name,
                   IExpression* expression) :
mAccessLevel(accessLevel),
mType(type),
mName(name),
mExpression(expression) { }

Constant::~Constant() { }

ObjectElementType Constant::getObjectElementType() const {
  return OBJECT_ELEMENT_CONSTANT;
}

void Constant::printToStream(IRGenerationContext& context, iostream& stream) const {
  if (mAccessLevel == PRIVATE_ACCESS) {
    return;
  }
  
  stream << "  public constant " << mType->getName() << " " << mName << " = ";
  mExpression->printToStream(context, stream);
  stream << ";\n";
}


