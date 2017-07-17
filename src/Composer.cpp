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
  LLVMContext& llvmContext = context.getLLVMContext();
  PointerType* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* bitcast = IRWriter::newBitCastInst(context, value, int8PointerType);
  
  Function* function = context.getModule()->getFunction(Names::getNPECheckFunctionName());
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  IRWriter::createInvokeInst(context, function, arguments, "");
}

