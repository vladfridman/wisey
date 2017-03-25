//
//  InterfaceDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Interface.hpp"
#include "yazyk/InterfaceDefinition.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/SafeBranch.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* InterfaceDefinition::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<MethodSignature*> methodSignatures;
  unsigned int methodIndex = 0;
  for (MethodSignatureDeclaration* methodSignatureDeclaration : mMethodSignatureDeclarations) {
    MethodSignature* methodSignature =
      methodSignatureDeclaration->createMethodSignature(context, methodIndex);
    methodSignatures.push_back(methodSignature);
    methodIndex++;
  }
  
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vtableType = functionType->getPointerTo()->getPointerTo();
  vector<Type*> types;
  types.push_back(vtableType);
  
  vector<Interface*> parentInterfaces;
  for (string parentInterfaceName : mParentInterfaceNames) {
    Interface* parentInterface = context.getInterface(parentInterfaceName);
    types.push_back(parentInterface->getLLVMType(context.getLLVMContext())
                    ->getPointerElementType());
    parentInterfaces.push_back(parentInterface);
  }
  
  StructType *structType = StructType::create(llvmContext, "interface." + mName);
  structType->setBody(types);
  Interface* interface = new Interface(mName, structType, parentInterfaces, methodSignatures);
  context.addInterface(interface);
  
  defineInterfaceTypeName(context, interface);
  defineInstanceOf(context, interface);
  
  return NULL;
}

void InterfaceDefinition::defineInterfaceTypeName(IRGenerationContext& context,
                                                  Interface* interface) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, interface->getName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());
}

void InterfaceDefinition::defineInstanceOf(IRGenerationContext& context,
                                           Interface* interface) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function* function = createInstanceOfFunction(context, interface);
  
  BasicBlock* lastBasicBlock = context.getBasicBlock();
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  BasicBlock* whileCond = BasicBlock::Create(llvmContext, "while.cond", function);
  BasicBlock* whileBody = BasicBlock::Create(llvmContext, "while.body", function);
  BasicBlock* returnNotFound = BasicBlock::Create(llvmContext, "return.notfound", function);
  BasicBlock* returnFound = BasicBlock::Create(llvmContext, "return.found", function);
  
  Value* iterator = new AllocaInst(Type::getInt32Ty(llvmContext), "iterator", entryBlock);
  new StoreInst(ConstantInt::get(Type::getInt32Ty(llvmContext), 0), iterator, entryBlock);
  
  Value* arrayOfStrings = composeInstanceOfEntryBlock(context, entryBlock, whileCond, function);

  Value* stringPointer = composeInstanceOfWhileConditionBlock(context,
                                                              whileCond,
                                                              whileBody,
                                                              returnNotFound,
                                                              iterator,
                                                              arrayOfStrings);

  composeInstanceOfWhileBodyBlock(context,
                                  whileBody,
                                  whileCond,
                                  returnFound,
                                  iterator,
                                  stringPointer,
                                  function);
  
  context.setBasicBlock(returnFound);
  LoadInst* iteratorLoaded = new LoadInst(iterator, "", returnFound);
  ReturnInst::Create(llvmContext, iteratorLoaded, returnFound);
  
  context.setBasicBlock(returnNotFound);
  ReturnInst::Create(llvmContext,
                     ConstantInt::get(Type::getInt32Ty(llvmContext), -1),
                     returnNotFound);
  
  context.setBasicBlock(lastBasicBlock);
}

Function* InterfaceDefinition::createInstanceOfFunction(IRGenerationContext& context,
                                                        Interface* interface) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);

  vector<Type*> argumentTypes;
  argumentTypes.push_back(interface->getLLVMType(llvmContext));
  argumentTypes.push_back(int8Type->getPointerTo());
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = PrimitiveTypes::INT_TYPE->getLLVMType(llvmContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        interface->getInstanceOfFunctionName(),
                                        context.getModule());

  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisArgument = &*functionArguments;
  thisArgument->setName("this");
  functionArguments++;
  Argument* compareToArgument = &*functionArguments;
  compareToArgument->setName("compareto");
  
  return function;
}

BitCastInst* InterfaceDefinition::composeInstanceOfEntryBlock(IRGenerationContext& context,
                                                              BasicBlock* entryBlock,
                                                              BasicBlock* whileCond,
                                                              Function* function) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);

  context.setBasicBlock(entryBlock);
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisArgument = &*functionArguments;
  Value* originalObject = Interface::getOriginalObject(context, thisArgument);
  
  Type* pointerToArrayOfStrings = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  
  
  BitCastInst* vTablePointer = new BitCastInst(originalObject,
                                               pointerToArrayOfStrings,
                                               "",
                                               entryBlock);
  LoadInst* vTable = new LoadInst(vTablePointer, "vtable", entryBlock);
  Value *Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 1);
  GetElementPtrInst* typeArrayPointerI8 = GetElementPtrInst::Create(int8Type->getPointerTo(),
                                                                    vTable,
                                                                    Idx,
                                                                    "typeArrayPointerI8",
                                                                    entryBlock);
  LoadInst* typeArrayI8 = new LoadInst(typeArrayPointerI8, "typeArrayI8", entryBlock);
  BitCastInst* arrayOfStrings =  new BitCastInst(typeArrayI8,
                                                 int8Type->getPointerTo()->getPointerTo(),
                                                 "arrayOfStrings",
                                                 entryBlock);
  
  SafeBranch::newBranch(whileCond, context);
  
  return arrayOfStrings;
}

LoadInst* InterfaceDefinition::composeInstanceOfWhileConditionBlock(IRGenerationContext& context,
                                                                    BasicBlock* whileCond,
                                                                    BasicBlock* whileBody,
                                                                    BasicBlock* returnFalse,
                                                                    Value* iterator,
                                                                    Value* arrayOfStrings) const {
  Type* int8Type = Type::getInt8Ty(context.getLLVMContext());
  
  context.setBasicBlock(whileCond);
  
  LoadInst* iteratorLoaded = new LoadInst(iterator, "", whileCond);
  Value* Idx[1];
  Idx[0] = iteratorLoaded;
  Value* stringPointerPointer = GetElementPtrInst::Create(int8Type->getPointerTo(),
                                                          arrayOfStrings,
                                                          Idx,
                                                          "stringPointerPointer",
                                                          whileCond);
  
  LoadInst* stringPointer = new LoadInst(stringPointerPointer, "stringPointer", whileCond);
  
  Value* nullPointerValue = ConstantPointerNull::get(int8Type->getPointerTo());
  Value* checkStringIsNull = new ICmpInst(*whileCond,
                                          ICmpInst::ICMP_EQ,
                                          stringPointer,
                                          nullPointerValue,
                                          "cmpnull");

  SafeBranch::newConditionalBranch(returnFalse, whileBody, checkStringIsNull, context);
  
  return stringPointer;
}

void InterfaceDefinition::composeInstanceOfWhileBodyBlock(IRGenerationContext& context,
                                                          BasicBlock* whileBody,
                                                          BasicBlock* whileCond,
                                                          BasicBlock* returnTrue,
                                                          Value* iterator,
                                                          Value* stringPointer,
                                                          Function* function) const {
  LLVMContext& llvmContext = context.getLLVMContext();

  context.setBasicBlock(whileBody);
  
  LoadInst* iteratorLoaded = new LoadInst(iterator, "", whileBody);
  Value* increment = BinaryOperator::Create(Instruction::Add,
                                            iteratorLoaded,
                                            ConstantInt::get(Type::getInt32Ty(llvmContext), 1),
                                            "inc",
                                            whileBody);
  new StoreInst(increment, iterator, whileBody);

  Function::arg_iterator functionArguments = function->arg_begin();
  functionArguments++;
  Argument* compareToArgument = &*functionArguments;

  Value* doesTypeMatch = new ICmpInst(*whileBody,
                                      ICmpInst::ICMP_EQ,
                                      stringPointer,
                                      compareToArgument,
                                      "cmp");
  SafeBranch::newConditionalBranch(returnTrue, whileCond, doesTypeMatch, context);
}