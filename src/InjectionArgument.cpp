//
//  InjectionArgument.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/InjectionArgument.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InjectionArgument::InjectionArgument(string fieldSpecifier, IExpression* fieldExpression) :
mFieldSpecifier(fieldSpecifier),
mFieldExpression(fieldExpression) { }

InjectionArgument::~InjectionArgument() {
  delete mFieldExpression;
}

bool InjectionArgument::checkArgument(const IConcreteObjectType* object) {
  if (mFieldSpecifier.substr(0, 4).compare("with")) {
    Log::e("Injection argument should start with 'with'. e.g. .withField(value).");
    return false;
  }
  
  string fieldName = deriveFieldName();
  if (object->findField(fieldName) == NULL) {
    Log::e("Injector could not find field " + fieldName + " in object " + object->getTypeName());
    return false;
  }
  
  return true;
}

string InjectionArgument::deriveFieldName() const {
  return "m" + mFieldSpecifier.substr(4);
}

Value* InjectionArgument::getValue(IRGenerationContext& context, IRGenerationFlag flag) const {
  return mFieldExpression->generateIR(context, flag);
}

const IType* InjectionArgument::getType(IRGenerationContext& context) const {
  return mFieldExpression->getType(context);
}

void InjectionArgument::printToStream(IRGenerationContext& context,
                                          iostream &stream) const {
  stream << mFieldSpecifier << "(";
  mFieldExpression->printToStream(context, stream);
  stream << ")";
}

