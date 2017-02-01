//
//  ModelBuilder.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Log.hpp"
#include "yazyk/ModelBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ModelBuilder::generateIR(IRGenerationContext& context) const {
  bool areArgumentsWellFormed = true;
  
  for (vector<ModelBuilderArgument*>::iterator iterator = mModelBuilderArgumentList->begin();
       iterator != mModelBuilderArgumentList->end();
       iterator++) {
    ModelBuilderArgument* argument = *iterator;
    areArgumentsWellFormed &= argument->checkArgument();
  }
  
  if (!areArgumentsWellFormed) {
    Log::e("Some arguments for the model builder are not well formed");
    exit(1);
  }
  
  Type* pointerType = Type::getInt32Ty(context.getLLVMContext());
  Type* structType = mModelTypeSpecifier.getType(context);
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  allocSize = ConstantExpr::getTruncOrBitCast(allocSize, pointerType);
  Instruction* malloc = CallInst::CreateMalloc(context.getBasicBlock(),
                                               pointerType,
                                               structType,
                                               allocSize,
                                               nullptr,
                                               nullptr,
                                               "buildervar");
  
  context.getBasicBlock()->getInstList().push_back(malloc);

  context.getScopes().setHeapVariable(malloc->getName(), malloc);

  return malloc;
}
