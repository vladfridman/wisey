//
//  IObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Composer.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string IObjectType::THIS = "this";

llvm::Constant* IObjectType::getObjectNamePointer(const IObjectType* object,
                                                  IRGenerationContext& context) {
  GlobalVariable* nameGlobal =
    context.getModule()->getNamedGlobal(object->getObjectNameGlobalVariableName());
  assert(nameGlobal && "Object name global constant not found");
  ConstantInt* zeroInt32 = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 0);
  Value* Idx[2];
  Idx[0] = zeroInt32;
  Idx[1] = zeroInt32;
  Type* elementType = nameGlobal->getType()->getPointerElementType();
  
  return ConstantExpr::getGetElementPtr(elementType, nameGlobal, Idx);
}

Value* IObjectType::getReferenceCountForObject(IRGenerationContext& context, Value* object) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int64Pointer = Type::getInt64Ty(llvmContext)->getPointerTo();
  Value* genericPointer = IRWriter::newBitCastInst(context, object, int64Pointer);
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), -1);
  Value* counterPointer = IRWriter::createGetElementPtrInst(context, genericPointer, index);
  return IRWriter::newLoadInst(context, counterPointer, "refCounter");
}

bool IObjectType::checkAccess(const IObjectType* from, const IObjectType* to) {
  if (to->isPublic()) {
    return true;
  }
  
  if (from == to) {
    return true;
  }
  
  if (from->getInnerObject(to->getShortName()) == to) {
    return true;
  }
  
  Log::e_deprecated("Object " + to->getTypeName() + " is not accessable from object " + from->getTypeName());
  exit(1);
}
