//
//  ModelBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/HeapVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ModelBuilder::~ModelBuilder() {
  delete mModelTypeSpecifier;
  for (BuilderArgument* argument : mBuilderArgumentList) {
    delete argument;
  }
  mBuilderArgumentList.clear();
}

Value* ModelBuilder::generateIR(IRGenerationContext& context) const {
  Model* model = (Model*) mModelTypeSpecifier->getType(context);
  Instruction* malloc = model->build(context, mBuilderArgumentList);
  
  HeapVariable* heapVariable = new HeapVariable(IVariable::getTemporaryVariableName(this),
                                                model->getOwner(),
                                                malloc);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

void ModelBuilder::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(IVariable::getTemporaryVariableName(this));
}

const IType* ModelBuilder::getType(IRGenerationContext& context) const {
  return mModelTypeSpecifier->getType(context)->getOwner();
}

bool ModelBuilder::existsInOuterScope(IRGenerationContext& context) const {
  IVariable* variable = context.getScopes().getVariable(IVariable::getTemporaryVariableName(this));
  return variable->existsInOuterScope();
}
