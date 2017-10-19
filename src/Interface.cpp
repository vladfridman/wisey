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
#include "wisey/InstanceOf.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/Model.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Interface::Interface(string name,
                     StructType* structType,
                     bool isExternal,
                     vector<InterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                     vector<IObjectElementDeclaration *> elementDelcarations) :
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mInterfaceOwner(new InterfaceOwner(this)),
mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
mElementDeclarations(elementDelcarations),
mIsComplete(false) { }

Interface::~Interface() {
  mParentInterfaces.clear();
  for (MethodSignature* methodSignature : mMethodSignatures) {
    delete methodSignature;
  }
  mMethodSignatures.clear();
  mAllMethodSignatures.clear();
  mNameToMethodSignatureMap.clear();
}

Interface* Interface::newInterface(string name,
                                   StructType *structType,
                                   vector<InterfaceTypeSpecifier *> parentInterfaceSpecifiers,
                                   vector<IObjectElementDeclaration *>
                                   elementDeclarations) {
  return new Interface(name,
                       structType,
                       false,
                       parentInterfaceSpecifiers,
                       elementDeclarations);
}

Interface* Interface::newExternalInterface(string name,
                                           StructType *structType,
                                           vector<InterfaceTypeSpecifier *>
                                           parentInterfaceSpecifiers,
                                           vector<IObjectElementDeclaration *>
                                           elementDeclarations) {
  return new Interface(name,
                       structType,
                       true,
                       parentInterfaceSpecifiers,
                       elementDeclarations);
}

void Interface::buildMethods(IRGenerationContext& context) {
  if (mIsComplete) {
    return;
  }

  LLVMContext& llvmContext = context.getLLVMContext();

  mMethodSignatures = createElements(context, mElementDeclarations);
  
  for (InterfaceTypeSpecifier* parentInterfaceSpecifier : mParentInterfaceSpecifiers) {
    Interface* parentInterface = (Interface*) parentInterfaceSpecifier->getType(context);
    if (!parentInterface->isComplete()) {
      parentInterface->buildMethods(context);
    }
    mParentInterfaces.push_back(parentInterface);
  }
  
  for (MethodSignature* methodSignature : mMethodSignatures) {
    mNameToMethodSignatureMap[methodSignature->getName()] = methodSignature;
    mAllMethodSignatures.push_back(methodSignature);
  }
  for (Interface* parentInterface : mParentInterfaces) {
    includeInterfaceMethods(parentInterface);
  }
  unsigned long index = 0;
  for (MethodSignature* methodSignature : mAllMethodSignatures) {
    mMethodIndexes[methodSignature] = index;
    index++;
  }

  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vtableType = functionType->getPointerTo()->getPointerTo();
  vector<Type*> types;
  types.push_back(vtableType);
  
  for (Interface* parentInterface : mParentInterfaces) {
    types.push_back(parentInterface->getLLVMType(llvmContext)
                    ->getPointerElementType()->getPointerElementType());
  }
  
  mStructType->setBody(types);
  mElementDeclarations.clear();
  mParentInterfaceSpecifiers.clear();
  mIsComplete = true;
}

bool Interface::isComplete() const {
  return mIsComplete;
}

void Interface::includeInterfaceMethods(Interface* parentInterface) {
  vector<MethodSignature*> inheritedMethods = parentInterface->getAllMethodSignatures();
  for (MethodSignature* methodSignature : inheritedMethods) {
    MethodSignature* existingMethod = findMethod(methodSignature->getName());
    if (existingMethod && !IMethodDescriptor::compare(existingMethod, methodSignature)) {
      Log::e("Interface " + mName + " overrides method " + existingMethod->getName()
             + " of parent interface with a wrong signature.");
      exit(1);
    }
    if (existingMethod) {
      continue;
    }
    MethodSignature* copySignature = methodSignature->createCopy();
    mAllMethodSignatures.push_back(copySignature);
    mNameToMethodSignatureMap[copySignature->getName()] = copySignature;
  }
}

vector<MethodSignature*> Interface::getAllMethodSignatures() const {
  return mAllMethodSignatures;
}

vector<Interface*> Interface::getParentInterfaces() const {
  return mParentInterfaces;
}

unsigned long Interface::getMethodIndex(IMethodDescriptor* methodDescriptor) const {
  if (!mMethodIndexes.count(methodDescriptor)) {
    Log::e("Method " + methodDescriptor->getName() + " not found in interface " + getName());
    exit(1);
  }
  return mMethodIndexes.at(methodDescriptor);
}

bool Interface::doesExtendInterface(const Interface* interface) const {
  for (Interface* parentInterface : mParentInterfaces) {
    if (parentInterface == interface || parentInterface->doesExtendInterface(interface)) {
      return true;
    }
  }
  return false;
}

string Interface::getObjectNameGlobalVariableName() const {
  return mName + ".name";
}

MethodSignature* Interface::findMethod(std::string methodName) const {
  if (!mNameToMethodSignatureMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodSignatureMap.at(methodName);
}

vector<list<Constant*>> Interface::generateMapFunctionsIR(IRGenerationContext& context,
                                                          const IObjectType* object,
                                                          map<string, Function*>& methodFunctionMap,
                                                          unsigned long interfaceIndex) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  list<Constant*> vTableArrayProtion;
  for (MethodSignature* methodSignature : mAllMethodSignatures) {
    Function* concreteObjectFunction = methodFunctionMap.count(methodSignature->getName())
      ? methodFunctionMap.at(methodSignature->getName()) : NULL;
    Function* function = generateMapFunctionForMethod(context,
                                                      object,
                                                      concreteObjectFunction,
                                                      interfaceIndex,
                                                      methodSignature);
    Constant* bitCast = ConstantExpr::getBitCast(function, int8Pointer);
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
                                                  const IObjectType* object,
                                                  llvm::Function* concreteObjectFunction,
                                                  unsigned long interfaceIndex,
                                                  MethodSignature* interfaceMethodSignature) const {
  IMethodDescriptor* objectMethodDescriptor =
    object->findMethod(interfaceMethodSignature->getName());
  if (objectMethodDescriptor == NULL) {
    Log::e("Method " + interfaceMethodSignature->getName() + " of interface " + mName +
           " is not implemented by object " + object->getName());
    exit(1);
  }
  
  if (objectMethodDescriptor->getReturnType() != interfaceMethodSignature->getReturnType()) {
    Log::e("Method " + interfaceMethodSignature->getName() + " of interface " + mName +
           " has different return type when implmeneted by object " + object->getName());
    exit(1);
  }
  
  if (doesMethodHaveUnexpectedExceptions(interfaceMethodSignature,
                                         objectMethodDescriptor,
                                         object->getName())) {
    Log::e("Exceptions thrown by method " +  interfaceMethodSignature->getName() +
           " of interface " + mName + " do not reconcile with exceptions thrown by its " +
           "implementation in object " + object->getName());
    exit(1);
  }
  
  if (!IMethodDescriptor::compare(objectMethodDescriptor, interfaceMethodSignature)) {
    Log::e("Method " + interfaceMethodSignature->getName() + " of interface " + mName +
           " has different argument types when implmeneted by object " + object->getName());
    exit(1);
  }
  
  if (interfaceIndex == 0) {
    return concreteObjectFunction;
  }

  string functionName =
    MethodCall::translateInterfaceMethodToLLVMFunctionName(object,
                                                           this,
                                                           interfaceMethodSignature->getName());
  Function* function = Function::Create(concreteObjectFunction->getFunctionType(),
                                        GlobalValue::ExternalLinkage,
                                        functionName,
                                        context.getModule());
  Function::arg_iterator arguments = function->arg_begin();
  llvm::Argument *argument = &*arguments;
  argument->setName("this");
  arguments++;
  argument->setName(ThreadExpression::THREAD);
  arguments++;
  vector<MethodArgument*> methodArguments = interfaceMethodSignature->getArguments();
  for (MethodArgument* methodArgument : interfaceMethodSignature->getArguments()) {
    llvm::Argument *argument = &*arguments;
    argument->setName(methodArgument->getName());
    arguments++;
  }
  
  generateMapFunctionBody(context,
                          object,
                          concreteObjectFunction,
                          function,
                          interfaceIndex,
                          interfaceMethodSignature);

  return function;
}

bool Interface::doesMethodHaveUnexpectedExceptions(MethodSignature* interfaceMethodSignature,
                                                   IMethodDescriptor* objectMethodDescriptor,
                                                   string objectName) const {
  map<string, const IType*> interfaceExceptionsMap;
  for (const IType* interfaceException : interfaceMethodSignature->getThrownExceptions()) {
    interfaceExceptionsMap[interfaceException->getName()] = interfaceException;
  }

  bool result = false;
  for (const IType* objectException : objectMethodDescriptor->getThrownExceptions()) {
    if (!interfaceExceptionsMap.count(objectException->getName())) {
      Log::e("Method " + objectMethodDescriptor->getName() + " of object " + objectName +
             " throws an unexpected exception of type " + objectException->getName());
      result = true;
    }
  }
  
  return result;
}

void Interface::generateMapFunctionBody(IRGenerationContext& context,
                                        const IObjectType* object,
                                        Function* concreteObjectFunction,
                                        Function* mapFunction,
                                        unsigned long interfaceIndex,
                                        MethodSignature* methodSignature) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock *basicBlock = BasicBlock::Create(llvmContext, "entry", mapFunction, 0);
  context.setBasicBlock(basicBlock);
  
  Function::arg_iterator arguments = mapFunction->arg_begin();
  Value* interfaceThis = &*arguments;
  arguments++;
  Value* threadReference = &*arguments;
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
  Value* concreteOjbectThis =
    IRWriter::createGetElementPtrInst(context, castedInterfaceThis, index);
  Value* castedObjectThis = IRWriter::newBitCastInst(context,
                                                     concreteOjbectThis,
                                                     interfaceThisLoaded->getType());
  Value* castedObjectThisStore = IRWriter::newAllocaInst(context, castedObjectThis->getType(), "");
  IRWriter::newStoreInst(context, castedObjectThis, castedObjectThisStore);
  
  vector<Value*> callArguments;
  callArguments.push_back(castedObjectThisStore);
  callArguments.push_back(threadReference);
  for (Value* argumentPointer : argumentPointers) {
    Value* loadedCallArgument = IRWriter::newLoadInst(context, argumentPointer, "");
    callArguments.push_back(loadedCallArgument);
  }
  
  if (concreteObjectFunction->getReturnType()->isVoidTy()) {
    IRWriter::createCallInst(context, concreteObjectFunction, callArguments, "");
    IRWriter::createReturnInst(context, NULL);
  } else {
    Value* result =
      IRWriter::createCallInst(context, concreteObjectFunction, callArguments, "call");
    IRWriter::createReturnInst(context, result);
  }
}

llvm::Value* Interface::storeArgumentValue(IRGenerationContext& context,
                                           BasicBlock* basicBlock,
                                           string variableName,
                                           const IType* variableType,
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

string Interface::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::PointerType* Interface::getLLVMType(LLVMContext& llvmContext) const {
  return mStructType->getPointerTo()->getPointerTo();
}

TypeKind Interface::getTypeKind() const {
  return INTERFACE_TYPE;
}

bool Interface::canCastTo(const IType* toType) const {
  TypeKind typeKind = toType->getTypeKind();
  if (typeKind == PRIMITIVE_TYPE || typeKind == NULL_TYPE_KIND) {
    return false;
  }
  
  // Assume can perform cast and check at run time
  return true;
}

bool Interface::canAutoCastTo(const IType* toType) const {
  TypeKind typeKind = toType->getTypeKind();
  if (typeKind == PRIMITIVE_TYPE || typeKind == NULL_TYPE_KIND) {
    return false;
  }
  
  if (typeKind == INTERFACE_TYPE && doesExtendInterface((Interface*) toType)) {
    return true;
  }
  
  return false;
}

string Interface::getCastFunctionName(IObjectType* toType) const {
  return "cast." + getName() + ".to." + toType->getName();
}

Value* Interface::castTo(IRGenerationContext& context,
                         Value* fromValue,
                         const IType* toType) const {
  IObjectType* toObjectWithMethodsType = (IObjectType*) (toType);
  Function* castFunction =
    context.getModule()->getFunction(getCastFunctionName(toObjectWithMethodsType));
  
  if (castFunction == NULL) {
    castFunction = defineCastFunction(context, fromValue, toObjectWithMethodsType);
  }
  
  vector<Value*> arguments;
  arguments.push_back(fromValue);
  
  Value* result = IRWriter::createCallInst(context, castFunction, arguments, "castTo");
  Value* resultStore = IRWriter::newAllocaInst(context, result->getType(), "");
  IRWriter::newStoreInst(context, result, resultStore);

  return resultStore;
}

Function* Interface::defineCastFunction(IRGenerationContext& context,
                                        Value* fromValue,
                                        IObjectType* toType) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(getLLVMType(llvmContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  PointerType* llvmReturnType = (PointerType*) toType->getLLVMType(llvmContext)
    ->getPointerElementType();
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::ExternalLinkage,
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
  Value* instanceof = InstanceOf::call(context, this, thisArgument, toType);
  Value* originalObject = getOriginalObject(context, thisArgument);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  ConstantInt* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  ICmpInst* compareToZero =
    IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, instanceof, zero, "cmp");
  IRWriter::createConditionalBranch(context, lessThanZero, notLessThanZero, compareToZero);
  
  context.setBasicBlock(lessThanZero);
  // TODO: throw a cast exception here once exceptions are implemented
  Value* nullPointer = ConstantPointerNull::get(llvmReturnType);
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
  Value* castValue = IRWriter::newBitCastInst(context, thunk, llvmReturnType);
  IRWriter::createReturnInst(context, castValue);

  context.setBasicBlock(zeroOrOne);
  castValue = IRWriter::newBitCastInst(context, originalObject, llvmReturnType);
  IRWriter::createReturnInst(context, castValue);
  
  context.setBasicBlock(lastBasicBlock);
  
  return function;
}

Value* Interface::getOriginalObject(IRGenerationContext& context, Value* value) {
  LLVMContext& llvmContext = context.getLLVMContext();

  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo()->getPointerTo()->getPointerTo();
  Value* valueLoaded = IRWriter::newLoadInst(context, value, "");
  BitCastInst* vTablePointer = IRWriter::newBitCastInst(context, valueLoaded, pointerType);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 0);
  GetElementPtrInst* unthunkPointer = IRWriter::createGetElementPtrInst(context, vTable, index);

  LoadInst* pointerToVal = IRWriter::newLoadInst(context, unthunkPointer, "unthunkbypointer");
  Value* unthunkBy = IRWriter::newPtrToIntInst(context,
                                               pointerToVal,
                                               Type::getInt64Ty(llvmContext),
                                               "unthunkby");

  BitCastInst* bitcast = IRWriter::newBitCastInst(context, valueLoaded, int8Type->getPointerTo());
  index[0] = unthunkBy;
  return IRWriter::createGetElementPtrInst(context, bitcast, index);
}

const IObjectOwnerType* Interface::getOwner() const {
  return mInterfaceOwner;
}

bool Interface::isExternal() const {
  return mIsExternal;
}

void Interface::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "external interface " << getName();
  if (mParentInterfaces.size()) {
    stream << endl << "  extends";
  }
  for (Interface* interface : mParentInterfaces) {
    stream << endl << "    " << interface->getName();
    if (interface != mParentInterfaces.at(mParentInterfaces.size() - 1)) {
      stream << ",";
    }
  }
  stream << " {" << endl;
  for (MethodSignature* methodSignature : mMethodSignatures) {
    methodSignature->printToStream(context, stream);
  }
  stream << "}" << endl;
}

void Interface::defineInterfaceTypeName(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, getName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     getObjectNameGlobalVariableName());
}

Instruction* Interface::inject(IRGenerationContext& context, ExpressionList expressionList) const {
  Controller* controller = context.getBoundController(context.getInterface(getName()));
  return controller->inject(context, expressionList);
}

vector<MethodSignature*> Interface::createElements(IRGenerationContext& context,
                                                   vector<IObjectElementDeclaration*>
                                                   elementDeclarations) {
  vector<MethodSignature*> methodSignatures;
  for (IObjectElementDeclaration* elementDeclaration : elementDeclarations) {
    IObjectElement* objectElement = elementDeclaration->declare(context);
    if (objectElement->getObjectElementType() == OBJECT_ELEMENT_FIELD) {
      Log::e("Interfaces can not contain fields");
      exit(1);
    }
    if (objectElement->getObjectElementType() == OBJECT_ELEMENT_METHOD) {
      Log::e("Interfaces can not contain method implmentations");
      exit(1);
    }
    methodSignatures.push_back((MethodSignature*) objectElement);
  }
  return methodSignatures;
}
