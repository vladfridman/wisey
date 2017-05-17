//
//  ModelBuilder.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <sstream>
#include <llvm/IR/Constants.h>

#include "wisey/Environment.hpp"
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* ModelBuilder::generateIR(IRGenerationContext& context) const {
  
  Model* model = context.getModel(mModelTypeSpecifier.getName());
  Instruction* malloc = model->build(context, mModelBuilderArgumentList);
  
  LocalHeapVariable* heapVariable = new LocalHeapVariable(getVariableName(), model, malloc);
  context.getScopes().setVariable(heapVariable);
  
  return malloc;
}

void ModelBuilder::releaseOwnership(IRGenerationContext& context) const {
  context.getScopes().clearVariable(getVariableName());
}

string ModelBuilder::getVariableName() const {
  ostringstream stream;
  stream << "__tmp" << (long) this;

  return stream.str();
}

IType* ModelBuilder::getType(IRGenerationContext& context) const {
  return mModelTypeSpecifier.getType(context);
}
