//
//  BuilderArgument.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "IConcreteObjectType.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "BuilderArgument.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BuilderArgument::BuilderArgument(string fieldSpecifier, IExpression* fieldExpression) :
mFieldSpecifier(fieldSpecifier),
mFieldExpression(fieldExpression) { }

BuilderArgument::~BuilderArgument() {
  delete mFieldExpression;
}

bool BuilderArgument::checkArgument(IRGenerationContext& context,
                                    const IConcreteObjectType* object,
                                    int line) {
  if (mFieldSpecifier.substr(0, 4).compare("with")) {
    context.reportError(line, "Object builder argument should start with 'with'. "
                        "e.g. .withField(value).");
    return false;
  }
  
  string fieldName = deriveFieldName();
  if (object->findField(fieldName) == NULL) {
    context.reportError(line, "Object builder could not find field " + fieldName + " in object " +
                        object->getTypeName());
    return false;
  }
  
  return true;
}

string BuilderArgument::deriveFieldName() const {
  return "m" + mFieldSpecifier.substr(4);
}

Value* BuilderArgument::getValue(IRGenerationContext& context, const IType* assignToType) const {
  return mFieldExpression->generateIR(context, assignToType);
}

const IType* BuilderArgument::getType(IRGenerationContext& context) const {
  return mFieldExpression->getType(context);
}

void BuilderArgument::printToStream(IRGenerationContext& context,
                                          iostream &stream) const {
  stream << mFieldSpecifier << "(";
  mFieldExpression->printToStream(context, stream);
  stream << ")";
}
