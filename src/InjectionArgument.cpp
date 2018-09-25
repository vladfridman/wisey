//
//  InjectionArgument.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "IConcreteObjectType.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "InjectionArgument.hpp"
#include "Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

InjectionArgument::InjectionArgument(string fieldSpecifier, IExpression* fieldExpression) :
mFieldSpecifier(fieldSpecifier),
mFieldExpression(fieldExpression) { }

InjectionArgument::~InjectionArgument() {
  delete mFieldExpression;
}

bool InjectionArgument::checkArgument(IRGenerationContext& context,
                                      const IConcreteObjectType* object,
                                      int line) {
  if (mFieldSpecifier.substr(0, 4).compare("with")) {
    context.reportError(line, "Injection argument should start with 'with'. "
                        "e.g. .withField(value).");
    return false;
  }
  
  string fieldName = deriveFieldName();
  if (object->findField(fieldName) == NULL) {
    context.reportError(line, "Injector could not find field " + fieldName + " in object " +
                        object->getTypeName());
    return false;
  }
  
  return true;
}

string InjectionArgument::deriveFieldName() const {
  return "m" + mFieldSpecifier.substr(4);
}

Value* InjectionArgument::getValue(IRGenerationContext& context, const IType* assignToType) const {
  return mFieldExpression->generateIR(context, assignToType);
}

const IType* InjectionArgument::getType(IRGenerationContext& context) const {
  return mFieldExpression->getType(context);
}

void InjectionArgument::printToStream(IRGenerationContext& context, iostream &stream) const {
  stream << mFieldSpecifier << "(";
  mFieldExpression->printToStream(context, stream);
  stream << ")";
}

