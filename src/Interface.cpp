//
//  Interface.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Cast.hpp"
#include "wisey/Environment.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/Model.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Interface::~Interface() {
  mParentInterfaces.clear();
  mNameToMethodSignatureMap.clear();
}

Interface::Interface(string name,
                     string fullName,
                     StructType* structType,
                     vector<Interface*> parentInterfaces,
                     vector<MethodSignature*> methodSignatures) {
  mName = name;
  mFullName = fullName;
  mStructType = structType;
  mParentInterfaces = parentInterfaces;
  mMethodSignatures = methodSignatures;
  
  prepare();
}

void Interface::prepare() {
  for (MethodSignature* methodSignature : mMethodSignatures) {
    mNameToMethodSignatureMap[methodSignature->getName()] = methodSignature;
    mAllMethodSignatures.push_back(methodSignature);
  }

  unsigned long methodIndex = mMethodSignatures.size();
  for (Interface* parentInterface : mParentInterfaces) {
    methodIndex = includeInterfaceMethods(parentInterface, methodIndex);
  }
}

unsigned long Interface::includeInterfaceMethods(Interface* parentInterface,
                                                 unsigned long methodIndex) {
  vector<MethodSignature*> inheritedMethods = parentInterface->getAllMethodSignatures();
  for (MethodSignature* methodSignature : inheritedMethods) {
    MethodSignature* existingMethod = findMethod(methodSignature->getName());
    if (existingMethod && !IMethodDescriptor::compare(existingMethod, methodSignature)) {
      Log::e("Interface '" + mName + "' overrides method '" + existingMethod->getName()
             + "' of parent interface with a wrong signature.");
      exit(1);
    }
    if (existingMethod) {
      continue;
    }
    MethodSignature* copySignature = methodSignature->createCopyWithIndex(methodIndex);
    mAllMethodSignatures.push_back(copySignature);
    mNameToMethodSignatureMap[copySignature->getName()] = copySignature;
    methodIndex++;
  }
  return methodIndex;
}

vector<MethodSignature*> Interface::getAllMethodSignatures() const {
  return mAllMethodSignatures;
}

vector<Interface*> Interface::getParentInterfaces() const {
  return mParentInterfaces;
}

bool Interface::doesExtendInterface(Interface* interface) const {
  for (Interface* parentInterface : mParentInterfaces) {
    if (parentInterface == interface || parentInterface->doesExtendInterface(interface)) {
      return true;
    }
  }
  return false;
}

string Interface::getObjectNameGlobalVariableName() const {
  return mFullName + ".name";
}

string Interface::getInstanceOfFunctionName() const {
  return mFullName + ".instanceof";
}

MethodSignature* Interface::findMethod(std::string methodName) const {
  if (!mNameToMethodSignatureMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodSignatureMap.at(methodName);
}

vector<list<Constant*>> Interface::generateMapFunctionsIR(IRGenerationContext& context,
                                                          IObjectWithMethodsType* object,
                                                          map<string, Function*>& methodFunctionMap,
                                                          unsigned long interfaceIndex) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  list<Constant*> vTableArrayProtion;
  for (MethodSignature* methodSignature : mAllMethodSignatures) {
    Function* modelFunction = methodFunctionMap.count(methodSignature->getName())
      ? methodFunctionMap.at(methodSignature->getName()) : NULL;
    Function* function =
      generateMapFunctionForMethod(context, object, modelFunction, interfaceIndex, methodSignature);
    Constant* bitCast = ConstantExpr::getBitCast(function, pointerType);
    vTableArrayProtion.push_back(bitCast);
  }
  vector<list<Constant*>> vSubTable;
  vSubTable.push_back(vTableArrayProtion);
  for (Interface* parentInterface : mParentInterfaces) {
    vector<list<Constant*>> parentInterfaceTables =
      parentInterface->generateMapFunctionsIR(context,
                                              object,
                                              methodFunctionMap,
                                              interfaceIndex + vSubTable.size());
    for (list<Constant*> parentInterfaceTable : parentInterfaceTables) {
      vSubTable.push_back(parentInterfaceTable);
    }
  }
  return vSubTable;
}

Function* Interface::generateMapFunctionForMethod(IRGenerationContext& context,
                                                  IObjectWithMethodsType* object,
                                                  llvm::Function* modelFunction,
                                                  unsigned long interfaceIndex,
                                                  MethodSignature* interfaceMethodSignature) const {
  IMethodDescriptor* objectMethodDescriptor =
    object->findMethod(interfaceMethodSignature->getName());
  if (objectMethodDescriptor == NULL) {
    Log::e("Method '" + interfaceMethodSignature->getName() + "' of interface '" + mName +
           "' is not implemented by object '" + object->getName() + "'");
    exit(1);
  }
  
  if (objectMethodDescriptor->getReturnType() != interfaceMethodSignature->getReturnType()) {
    Log::e("Method '" + interfaceMethodSignature->getName() + "' of interface '" + mName +
           "' has different return type when implmeneted by object '"
           + object->getName() + "'");
    exit(1);
  }
  
  if (doesMethodHaveUnexpectedExceptions(interfaceMethodSignature,
                                         objectMethodDescriptor,
                                         object->getName())) {
    Log::e("Exceptions thrown by method '" +  interfaceMethodSignature->getName() +
           "' of interface '" + mName + "' do not reconcile with exceptions thrown by its " +
           "implementation in object '" + object->getName() + "'");
    exit(1);
  }
  
  if (!IMethodDescriptor::compare(objectMethodDescriptor, interfaceMethodSignature)) {
    Log::e("Method '" + interfaceMethodSignature->getName() + "' of interface '" + mName +
           "' has different argument types when implmeneted by object '"
           + object->getName() + "'");
    exit(1);
  }

  if (interfaceIndex == 0) {
    return modelFunction;
  }
  
  string functionName =
    MethodCall::translateInterfaceMethodToLLVMFunctionName(object,
                                                           this,
                                                           interfaceMethodSignature->getName());
  Function* function = Function::Create(modelFunction->getFunctionType(),
                                        GlobalValue::InternalLinkage,
                                        functionName,
                                        context.getModule());
  Function::arg_iterator arguments = function->arg_begin();
  llvm::Argument *argument = &*arguments;
  argument->setName("this");
  arguments++;
  vector<MethodArgument*> methodArguments = interfaceMethodSignature->getArguments();
  for (MethodArgument* methodArgument : interfaceMethodSignature->getArguments()) {
    llvm::Argument *argument = &*arguments;
    argument->setName(methodArgument->getName());
    arguments++;
  }
  
  generateMapFunctionBody(context,
                          object,
                          modelFunction,
                          function,
                          interfaceIndex,
                          interfaceMethodSignature);

  return function;
}

bool Interface::doesMethodHaveUnexpectedExceptions(MethodSignature* interfaceMethodSignature,
                                                   IMethodDescriptor* objectMethodDescriptor,
                                                   string objectName) const {
  map<string, IType*> interfaceExceptionsMap;
  for (IType* interfaceException : interfaceMethodSignature->getThrownExceptions()) {
    interfaceExceptionsMap[interfaceException->getName()] = interfaceException;
  }

  bool result = false;
  for (IType* objectException : objectMethodDescriptor->getThrownExceptions()) {
    if (!interfaceExceptionsMap.count(objectException->getName())) {
      Log::e("Method '" + objectMethodDescriptor->getName() + "' of object '" + objectName +
             "' throws an unexpected exception of type '" + objectException->getName() + "'");
      result = true;
    }
  }
  
  return result;
}

void Interface::generateMapFunctionBody(IRGenerationContext& context,
                                        IObjectWithMethodsType* object,
                                        Function* modelFunction,
                                        Function* mapFunction,
                                        unsigned long interfaceIndex,
                                        MethodSignature* methodSignature) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock *basicBlock = BasicBlock::Create(llvmContext, "entry", mapFunction, 0);
  context.setBasicBlock(basicBlock);
  
  Function::arg_iterator arguments = mapFunction->arg_begin();
  Value* interfaceThis = storeArgumentValue(context, basicBlock, "this", object, &*arguments);
  arguments++;
  vector<Value*> argumentPointers;
  for (MethodArgument* methodArgument : methodSignature->getArguments()) {
    Value* argumentPointer = storeArgumentValue(context,
                                                basicBlock,
                                                methodArgument->getName(),
                                                methodArgument->getType(),
                                                &*arguments);
    argumentPointers.push_back(argumentPointer);
    arguments++;
  }
  
  Value* interfaceThisLoaded = IRWriter::newLoadInst(context, interfaceThis, "this");
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo();
  Value* castedInterfaceThis = IRWriter::newBitCastInst(context, interfaceThisLoaded, pointerType);
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext),
                            -interfaceIndex * Environment::getAddressSizeInBytes());
  Value* modelThis = IRWriter::createGetElementPtrInst(context, castedInterfaceThis, index);
  Value* castedModelThis = IRWriter::newBitCastInst(context,
                                                    modelThis,
                                                    interfaceThisLoaded->getType());
  vector<Value*> callArguments;
  callArguments.push_back(castedModelThis);
  for (Value* argumentPointer : argumentPointers) {
    Value* loadedCallArgument = IRWriter::newLoadInst(context, argumentPointer, "");
    callArguments.push_back(loadedCallArgument);
  }
  
  if (modelFunction->getReturnType()->isVoidTy()) {
    IRWriter::createCallInst(context, modelFunction, callArguments, "");
    IRWriter::createReturnInst(context, NULL);
  } else {
    Value* result = IRWriter::createCallInst(context, modelFunction, callArguments, "call");
    IRWriter::createReturnInst(context, result);
  }
}

llvm::Value* Interface::storeArgumentValue(IRGenerationContext& context,
                                           BasicBlock* basicBlock,
                                           string variableName,
                                           IType* variableType,
                                           Value* variableValue) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* llvmType = variableType->getLLVMType(llvmContext);
  string newName = variableName + ".param";
  AllocaInst *alloc = IRWriter::newAllocaInst(context, llvmType, newName);
  IRWriter::newStoreInst(context, variableValue, alloc);
  return alloc;
}

string Interface::getName() const {
  return mName;
}

string Interface::getFullName() const {
  return mFullName;
}

llvm::Type* Interface::getLLVMType(LLVMContext& llvmContext) const {
  return mStructType->getPointerTo();
}

TypeKind Interface::getTypeKind() const {
  return INTERFACE_TYPE;
}

bool Interface::canCastTo(IType* toType) const {
  if (toType->getTypeKind() == PRIMITIVE_TYPE) {
    return false;
  }
  
  // Assume can perform cast and check at run time
  return true;
}

bool Interface::canAutoCastTo(IType* toType) const {
  if (toType->getTypeKind() == PRIMITIVE_TYPE) {
    return false;
  }
  
  if (toType->getTypeKind() == INTERFACE_TYPE &&
      doesExtendInterface(dynamic_cast<Interface*>(toType))) {
    return true;
  }
  
  return false;
}

string Interface::getCastFunctionName(IObjectWithMethodsType* toType) const {
  return "cast." + getFullName() + ".to." + toType->getFullName();
}

Value* Interface::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  IObjectWithMethodsType* toObjectWithMethodsType = dynamic_cast<IObjectWithMethodsType*>(toType);
  Function* castFunction =
    context.getModule()->getFunction(getCastFunctionName(toObjectWithMethodsType));
  
  if (castFunction == NULL) {
    castFunction = defineCastFunction(context, fromValue, toObjectWithMethodsType);
  }
  
  vector<Value*> arguments;
  arguments.push_back(fromValue);
  
  return IRWriter::createCallInst(context, castFunction, arguments, "castTo");
}

Function* Interface::defineCastFunction(IRGenerationContext& context,
                                        Value* fromValue,
                                        IObjectWithMethodsType* toType) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(getLLVMType(llvmContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = toType->getLLVMType(llvmContext);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        getCastFunctionName(toType),
                                        context.getModule());
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisArgument = &*functionArguments;
  thisArgument->setName("this");
  functionArguments++;
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* lessThanZero = BasicBlock::Create(llvmContext, "less.than.zero", function);
  BasicBlock* notLessThanZero = BasicBlock::Create(llvmContext, "not.less.than.zero", function);
  BasicBlock* moreThanOne = BasicBlock::Create(llvmContext, "more.than.one", function);
  BasicBlock* zeroOrOne = BasicBlock::Create(llvmContext, "zero.or.one", function);
  
  BasicBlock* lastBasicBlock = context.getBasicBlock();

  context.setBasicBlock(entryBlock);
  Value* instanceof = callInstanceOf(context, thisArgument, (IObjectWithMethodsType*) toType);
  Value* originalObject = getOriginalObject(context, thisArgument);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  ConstantInt* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  ICmpInst* compareToZero =
    IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, instanceof, zero, "cmp");
  IRWriter::createConditionalBranch(context, lessThanZero, notLessThanZero, compareToZero);
  
  context.setBasicBlock(lessThanZero);
  // TODO: throw a cast exception here once exceptions are implemented
  Value* nullPointer = ConstantPointerNull::get((PointerType*) toType->getLLVMType(llvmContext));
  IRWriter::createReturnInst(context, nullPointer);

  context.setBasicBlock(notLessThanZero);
  ICmpInst* compareToOne =
    IRWriter::newICmpInst(context, ICmpInst::ICMP_SGT, instanceof, one, "cmp");
  IRWriter::createConditionalBranch(context, moreThanOne, zeroOrOne, compareToOne);

  context.setBasicBlock(moreThanOne);
  ConstantInt* bytes = ConstantInt::get(Type::getInt32Ty(llvmContext),
                                        Environment::getAddressSizeInBytes());
  BitCastInst* bitcast =
  IRWriter::newBitCastInst(context, originalObject, int8Type->getPointerTo());
  Value* offset = IRWriter::createBinaryOperator(context, Instruction::Sub, instanceof, one, "");
  Value* thunkBy = IRWriter::createBinaryOperator(context, Instruction::Mul, offset, bytes, "");
  Value* index[1];
  index[0] = thunkBy;
  Value* thunk = IRWriter::createGetElementPtrInst(context, bitcast, index);
  Value* castValue = IRWriter::newBitCastInst(context, thunk, toType->getLLVMType(llvmContext));
  IRWriter::createReturnInst(context, castValue);

  context.setBasicBlock(zeroOrOne);
  castValue = IRWriter::newBitCastInst(context, originalObject, toType->getLLVMType(llvmContext));
  IRWriter::createReturnInst(context, castValue);
  
  context.setBasicBlock(lastBasicBlock);
  
  return function;
}

Value* Interface::getOriginalObject(IRGenerationContext& context, Value* value) {
  LLVMContext& llvmContext = context.getLLVMContext();

  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer = IRWriter::newBitCastInst(context, value, pointerType);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 0);
  GetElementPtrInst* unthunkPointer = IRWriter::createGetElementPtrInst(context, vTable, index);

  LoadInst* pointerToVal = IRWriter::newLoadInst(context, unthunkPointer, "unthunkbypointer");
  Value* unthunkBy = IRWriter::newPtrToIntInst(context,
                                               pointerToVal,
                                               Type::getInt64Ty(llvmContext),
                                               "unthunkby");

  BitCastInst* bitcast = IRWriter::newBitCastInst(context, value, int8Type->getPointerTo());
  index[0] = unthunkBy;
  return IRWriter::createGetElementPtrInst(context, bitcast, index);
}

CallInst* Interface::callInstanceOf(IRGenerationContext& context,
                                    Value* interfaceObject,
                                    IObjectWithMethodsType* callableObjectType) const {
  Function* function = context.getModule()->getFunction(getInstanceOfFunctionName());
  
  Constant* namePointer = IObjectWithMethodsType::getObjectNamePointer(callableObjectType, context);

  vector<Value*> arguments;
  arguments.push_back(interfaceObject);
  arguments.push_back(namePointer);
  
  return IRWriter::createCallInst(context, function, arguments, "instanceof");
}
