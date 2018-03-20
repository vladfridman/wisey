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
  StructType* fileStructType = defineFileStruct(context);
  defineStderr(context, fileStructType);
  defineEmptyString(context);
  
  return NULL;
}

StructType* ProgramPrefix::defineFileStruct(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Type*> sbufTypes;
  sbufTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  sbufTypes.push_back(Type::getInt32Ty(llvmContext));
  StructType* sbufTypeStructType = StructType::create(llvmContext, "struct.__sbuf");
  sbufTypeStructType->setBody(sbufTypes);

  StructType* sbufFileXStructType = StructType::create(llvmContext, "struct.__sFILEX");
  
  vector<Type*> sbufFileTypes;
  sbufFileTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  sbufFileTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(Type::getInt16Ty(llvmContext));
  sbufFileTypes.push_back(Type::getInt16Ty(llvmContext));
  sbufFileTypes.push_back(sbufTypeStructType);
  sbufFileTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  sbufFileTypes.push_back(FunctionType::get(Type::getInt32Ty(llvmContext),
                                            argumentTypes,
                                            false)->getPointerTo());
  argumentTypes.clear();
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(FunctionType::get(Type::getInt32Ty(llvmContext),
                                            argumentTypes,
                                            false)->getPointerTo());
  argumentTypes.clear();
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  argumentTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(FunctionType::get(Type::getInt64Ty(llvmContext),
                                            argumentTypes,
                                            false)->getPointerTo());
  argumentTypes.clear();
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(FunctionType::get(Type::getInt32Ty(llvmContext),
                                            argumentTypes,
                                            false)->getPointerTo());
  sbufFileTypes.push_back(sbufTypeStructType);
  sbufFileTypes.push_back(sbufFileXStructType->getPointerTo());
  sbufFileTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(llvm::ArrayType::get(Type::getInt8Ty(llvmContext), 3));
  sbufFileTypes.push_back(llvm::ArrayType::get(Type::getInt8Ty(llvmContext), 1));
  sbufFileTypes.push_back(sbufTypeStructType);
  sbufFileTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(Type::getInt64Ty(llvmContext));
  
  StructType* sbufFileStructType = StructType::create(llvmContext, "struct.__sFILE");
  sbufFileStructType->setBody(sbufFileTypes);
  
  return sbufFileStructType;
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
