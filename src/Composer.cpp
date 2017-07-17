//
//  Composer.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Composer.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Names.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

void Composer::checkNullAndThrowNPE(IRGenerationContext& context, Value* value) {
  PointerType* int8PointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8PointerType);
  
  Function* function = context.getModule()->getFunction(Names::getNPECheckFunctionName());
  vector<Value*> arguments;
  arguments.push_back(bitcast);

  IRWriter::createInvokeInst(context, function, arguments, "");
}

void Composer::freeIfNotNull(IRGenerationContext& context, Value* pointer) {
  Type* int8TypePointer = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  Value* argument = pointer->getType() != int8TypePointer
  ? IRWriter::newBitCastInst(context, pointer, int8TypePointer)
  : pointer;

  Function* freeFunction = context.getModule()->getFunction(Names::getFreeIfNotNullFunctionName());
  vector<Value*> arguments;
  arguments.push_back(argument);

  IRWriter::createCallInst(context, freeFunction, arguments, "");
}
