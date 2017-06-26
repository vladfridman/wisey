//
//  BuilderArgument.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/BuilderArgument.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BuilderArgument::~BuilderArgument() {
  delete mFieldExpression;
}

bool BuilderArgument::checkArgument(const IConcreteObjectType* object) {
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

string BuilderArgument::deriveFieldName() const {
  return "m" + mFieldSpecifier.substr(4);
}

Value* BuilderArgument::getValue(IRGenerationContext& context) const {
  return mFieldExpression->generateIR(context);
}

const IType* BuilderArgument::getType(IRGenerationContext& context) const {
  return mFieldExpression->getType(context);
}
