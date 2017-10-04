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
  context.setPackage(Names::getLangPackageName());

  defineFreeIfNotNullFunction(context);
  defineNPEFunction(context);
  StructType* fileStructType = defineFileStruct(context);
  defineStderr(context, fileStructType);
  
  return NULL;
}

void ProgramPrefix::defineNPEFunction(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);

  Function::Create(ftype,
                   GlobalValue::InternalLinkage,
                   Names::getNPECheckFunctionName(),
                   context.getModule());
}

void ProgramPrefix::defineFreeIfNotNullFunction(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Type*> argumentTypes;
  PointerType* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  argumentTypes.push_back(int8PointerType);
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = Type::getVoidTy(llvmContext);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  Function* function = Function::Create(ftype,
                                        GlobalValue::InternalLinkage,
                                        Names::getFreeIfNotNullFunctionName(),
                                        context.getModule());
  
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument *llvmArgument = &*llvmArguments;
  llvmArgument->setName("pointer");
  
  BasicBlock* basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.setBasicBlock(basicBlock);
  
  Value* null = ConstantPointerNull::get(int8PointerType);
  Value* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, llvmArgument, null, "cmp");
  FakeExpression* fakeExpression = new FakeExpression(compare, PrimitiveTypes::BOOLEAN_TYPE);
  
  Block* thenBlock = new Block();
  ReturnVoidStatement* returnVoidStatement = new ReturnVoidStatement();
  thenBlock->getStatements().push_back(returnVoidStatement);
  CompoundStatement* thenStatement = new CompoundStatement(thenBlock);
  IfStatement ifStatement(fakeExpression, thenStatement);
  
  context.getScopes().pushScope();
  ifStatement.generateIR(context);
  context.getScopes().popScope(context);
  
  IRWriter::createFree(context, llvmArgument);
  IRWriter::createReturnInst(context, NULL);
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
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  sbufFileTypes.push_back(FunctionType::get(Type::getInt32Ty(llvmContext),
                                            argTypesArray,
                                            false)->getPointerTo());
  argumentTypes.clear();
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt32Ty(llvmContext));
  argTypesArray = ArrayRef<Type*>(argumentTypes);
  sbufFileTypes.push_back(FunctionType::get(Type::getInt32Ty(llvmContext),
                                            argTypesArray,
                                            false)->getPointerTo());
  argumentTypes.clear();
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt64Ty(llvmContext));
  argumentTypes.push_back(Type::getInt32Ty(llvmContext));
  argTypesArray = ArrayRef<Type*>(argumentTypes);
  sbufFileTypes.push_back(FunctionType::get(Type::getInt64Ty(llvmContext),
                                            argTypesArray,
                                            false)->getPointerTo());
  argumentTypes.clear();
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt8Ty(llvmContext)->getPointerTo());
  argumentTypes.push_back(Type::getInt32Ty(llvmContext));
  argTypesArray = ArrayRef<Type*>(argumentTypes);
  sbufFileTypes.push_back(FunctionType::get(Type::getInt32Ty(llvmContext),
                                            argTypesArray,
                                            false)->getPointerTo());
  sbufFileTypes.push_back(sbufTypeStructType);
  sbufFileTypes.push_back(sbufFileXStructType->getPointerTo());
  sbufFileTypes.push_back(Type::getInt32Ty(llvmContext));
  sbufFileTypes.push_back(ArrayType::get(Type::getInt8Ty(llvmContext), 3));
  sbufFileTypes.push_back(ArrayType::get(Type::getInt8Ty(llvmContext), 1));
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
