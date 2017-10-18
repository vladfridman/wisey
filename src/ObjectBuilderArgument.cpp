//
//  ObjectBuilderArgument.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ObjectBuilderArgument::ObjectBuilderArgument(string fieldSpecifier, IExpression* fieldExpression) :
mFieldSpecifier(fieldSpecifier),
mFieldExpression(fieldExpression) { }

ObjectBuilderArgument::~ObjectBuilderArgument() {
  delete mFieldExpression;
}

bool ObjectBuilderArgument::checkArgument(const IConcreteObjectType* object) {
  if (mFieldSpecifier.substr(0, 4).compare("with")) {
    Log::e("Object builder argument should start with 'with'. e.g. .withField(value).");
    return false;
  }
  
  string fieldName = deriveFieldName();
  if (object->findField(fieldName) == NULL) {
    Log::e("Object builder could not find field " + fieldName + " in object " + object->getName());
    return false;
  }
  
  return true;
}

string ObjectBuilderArgument::deriveFieldName() const {
  return "m" + mFieldSpecifier.substr(4);
}

Value* ObjectBuilderArgument::getValue(IRGenerationContext& context) const {
  return mFieldExpression->generateIR(context);
}

const IType* ObjectBuilderArgument::getType(IRGenerationContext& context) const {
  return mFieldExpression->getType(context);
}

void ObjectBuilderArgument::releaseOwnership(IRGenerationContext& context) const {
  mFieldExpression->releaseOwnership(context);
}

void ObjectBuilderArgument::printToStream(IRGenerationContext& context,
                                          iostream &stream) const {
  stream << mFieldSpecifier << "(";
  mFieldExpression->printToStream(context, stream);
  stream << ")";
}
