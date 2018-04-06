//
//  ProgramPrefix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/FakeExpression.hpp"
#include "wisey/IfStatement.hpp"
#include "wisey/InterfaceDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/ImportStatement.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/RelationalExpression.hpp"
#include "wisey/ReturnVoidStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* ProgramPrefix::generateIR(IRGenerationContext& context) const {
  StructType* fileStructType = context.getModule()->getTypeByName("__sFILE");
  defineStderr(context, fileStructType);
  defineEmptyString(context);
  
  return NULL;
}

void ProgramPrefix::defineStderr(IRGenerationContext& context, StructType* fileStructType) const {
  GlobalVariable* global = new GlobalVariable(*context.getModule(),
                                              fileStructType->getPointerTo(),
                                              false,
                                              GlobalValue::ExternalLinkage,
                                              nullptr,
                                              Names::getStdErrName());
  global->setAlignment(8);
}

void ProgramPrefix::defineEmptyString(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, "");
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::InternalLinkage,
                     stringConstant,
                     Names::getEmptyStringName());
}
