//
//  Interface.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForInterfaceFunction.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/Cast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/InstanceOf.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Interface::Interface(AccessLevel accessLevel,
                     string name,
                     StructType* structType,
                     bool isExternal,
                     vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                     vector<IObjectElementDeclaration *> elementDelcarations) :
mAccessLevel(accessLevel),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false),
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
  mStaticMethods.clear();
  mNameToMethodSignatureMap.clear();
  mNameToStaticMethodMap.clear();
  for (Constant* constant : mConstants) {
    delete constant;
  }
  mConstants.clear();
  mNameToConstantMap.clear();
  mInnerObjects.clear();
  mParentInterfacesMap.clear();
}

Interface* Interface::newInterface(AccessLevel accessLevel,
                                   string name,
                                   StructType *structType,
                                   vector<IInterfaceTypeSpecifier *> parentInterfaceSpecifiers,
                                   vector<IObjectElementDeclaration *>
                                   elementDeclarations) {
  return new Interface(accessLevel,
                       name,
                       structType,
                       false,
                       parentInterfaceSpecifiers,
                       elementDeclarations);
}

Interface* Interface::newExternalInterface(string name,
                                           StructType *structType,
                                           vector<IInterfaceTypeSpecifier *>
                                           parentInterfaceSpecifiers,
                                           vector<IObjectElementDeclaration *>
                                           elementDeclarations) {
  return new Interface(AccessLevel::PUBLIC_ACCESS,
                       name,
                       structType,
                       true,
                       parentInterfaceSpecifiers,
                       elementDeclarations);
}

AccessLevel Interface::getAccessLevel() const {
  return mAccessLevel;
}

void Interface::buildMethods(IRGenerationContext& context) {
  if (mIsComplete) {
    return;
  }

  LLVMContext& llvmContext = context.getLLVMContext();

  tuple<vector<MethodSignature*>, vector<wisey::Constant*>, vector<StaticMethod*>> elements =
  createElements(context, mElementDeclarations);

  mMethodSignatures = get<0>(elements);
  mConstants = get<1>(elements);
  for (Constant* constant : mConstants) {
    mNameToConstantMap[constant->getName()] = constant;
  }
  mStaticMethods = get<2>(elements);
  for (StaticMethod* staticMethod : mStaticMethods) {
    mNameToStaticMethodMap[staticMethod->getName()] = staticMethod;
  }
  
  for (IInterfaceTypeSpecifier* parentInterfaceSpecifier : mParentInterfaceSpecifiers) {
    Interface* parentInterface = (Interface*) parentInterfaceSpecifier->getType(context);
    if (mParentInterfacesMap.count(parentInterface->getTypeName())) {
      Log::e("Circular interface dependency between interfaces " +
             getTypeName() + " and " + parentInterface->getTypeName());
      exit(1);
    }
    mParentInterfacesMap[parentInterface->getTypeName()] = parentInterface;
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
    types.push_back(parentInterface->getLLVMType(context)->getPointerElementType());
  }
  
  mStructType->setBody(types);
  mElementDeclarations.clear();
  mParentInterfaceSpecifiers.clear();
  mIsComplete = true;
}

wisey::Constant* Interface::findConstant(string constantName) const {
  if (!mNameToConstantMap.count(constantName)) {
    Log::e("Interface " + mName + " does not have constant named " + constantName);
    exit(1);
  }
  return mNameToConstantMap.at(constantName);
}

bool Interface::isComplete() const {
  return mIsComplete;
}

void Interface::includeInterfaceMethods(Interface* parentInterface) {
  vector<MethodSignature*> inheritedMethods = parentInterface->getAllMethodSignatures();
  for (MethodSignature* methodSignature : inheritedMethods) {
    IMethodDescriptor* existingMethod = findMethod(methodSignature->getName());
    if (existingMethod && !IMethodDescriptor::compare(existingMethod, methodSignature)) {
      Log::e("Interface " + mName + " overrides method " + existingMethod->getName()
             + " of parent interface with a wrong signature.");
      exit(1);
    }
    if (existingMethod) {
      continue;
    }
    MethodSignature* copySignature = methodSignature->createCopy(this);
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
    Log::e("Method " + methodDescriptor->getName() + " not found in interface " +
           getTypeName());
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

IMethodDescriptor* Interface::findMethod(std::string methodName) const {
  if (mNameToMethodSignatureMap.count(methodName)) {
    return mNameToMethodSignatureMap.at(methodName);
  }
  
  if (mNameToStaticMethodMap.count(methodName)) {
    return mNameToStaticMethodMap.at(methodName);
  }
  
  return NULL;
}

void Interface::generateConstantsIR(IRGenerationContext& context) const {
  for (Constant* constant : mConstants) {
    constant->generateIR(context, this);
  }
}

void Interface::defineStaticMethodFunctions(IRGenerationContext& context) const {
  for (IMethod* method : mStaticMethods) {
    method->defineFunction(context);
  }
}

void Interface::defineCurrentObjectNameVariable(IRGenerationContext& context) const {
  Value* objectName = IObjectType::getObjectNamePointer(this, context);
  ParameterPrimitiveVariable* objectNameVariable =
  new ParameterPrimitiveVariable(Names::getCurrentObjectVariableName(),
                                 PrimitiveTypes::STRING_TYPE,
                                 objectName);
  context.getScopes().setVariable(objectNameVariable);
}

void Interface::generateStaticMethodsIR(IRGenerationContext& context) const {
  for (IMethod* method : mStaticMethods) {
    method->generateIR(context);
  }
}

vector<list<llvm::Constant*>> Interface::defineMapFunctions(IRGenerationContext& context,
                                                            const IObjectType* object,
                                                            map<string, Function*>&
                                                            methodFunctionMap,
                                                            unsigned long interfaceIndex) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Pointer = Type::getInt8Ty(llvmContext)->getPointerTo();
  list<llvm::Constant*> vTableArrayProtion;
  for (MethodSignature* methodSignature : mAllMethodSignatures) {
    Function* concreteObjectFunction = methodFunctionMap.count(methodSignature->getName())
      ? methodFunctionMap.at(methodSignature->getName()) : NULL;
    Function* function = defineMapFunctionForMethod(context,
                                                    object,
                                                    concreteObjectFunction,
                                                    interfaceIndex,
                                                    methodSignature);
    llvm::Constant* bitCast = ConstantExpr::getBitCast(function, int8Pointer);
    vTableArrayProtion.push_back(bitCast);
  }
  vector<list<llvm::Constant*>> vSubTable;
  vSubTable.push_back(vTableArrayProtion);
  for (Interface* parentInterface : mParentInterfaces) {
    vector<list<llvm::Constant*>> parentInterfaceTables =
      parentInterface->defineMapFunctions(context,
                                          object,
                                          methodFunctionMap,
                                          interfaceIndex + vSubTable.size());
    for (list<llvm::Constant*> parentInterfaceTable : parentInterfaceTables) {
      vSubTable.push_back(parentInterfaceTable);
    }
  }
  return vSubTable;
}

unsigned long Interface::composeMapFunctions(IRGenerationContext& context,
                                             const IObjectType* object,
                                             unsigned long interfaceIndex) const {
  for (MethodSignature* methodSignature : mAllMethodSignatures) {
    string concreteObjectFunctionName =
    IMethodCall::translateObjectMethodToLLVMFunctionName(object, methodSignature->getName());
    Function* concreteObjectFunction = context.getModule()->getFunction(concreteObjectFunctionName);
    composeMapFunctionBody(context,
                           object,
                           concreteObjectFunction,
                           interfaceIndex,
                           methodSignature);
  }
  unsigned long offset = 1u;
  for (Interface* parentInterface : mParentInterfaces) {
    unsigned long parentOffset = parentInterface->composeMapFunctions(context,
                                                                      object,
                                                                      interfaceIndex + offset);
    offset += parentOffset;
  }
  return offset;
}

Function* Interface::defineMapFunctionForMethod(IRGenerationContext& context,
                                                const IObjectType* object,
                                                llvm::Function* concreteObjectFunction,
                                                unsigned long interfaceIndex,
                                                MethodSignature* interfaceMethodSignature) const {
  IMethodDescriptor* objectMethodDescriptor =
    object->findMethod(interfaceMethodSignature->getName());
  if (objectMethodDescriptor == NULL) {
    Log::e("Method " + interfaceMethodSignature->getName() + " of interface " + mName +
           " is not implemented by object " + object->getTypeName());
    exit(1);
  }
  
  if (objectMethodDescriptor->getReturnType() != interfaceMethodSignature->getReturnType()) {
    Log::e("Method " + interfaceMethodSignature->getName() + " of interface " + mName +
           " has different return type when implmeneted by object " + object->getTypeName());
    exit(1);
  }
  
  if (doesMethodHaveUnexpectedExceptions(interfaceMethodSignature,
                                         objectMethodDescriptor,
                                         object->getTypeName())) {
    Log::e("Exceptions thrown by method " +  interfaceMethodSignature->getName() +
           " of interface " + mName + " do not reconcile with exceptions thrown by its " +
           "implementation in object " + object->getTypeName());
    exit(1);
  }
  
  if (!IMethodDescriptor::compare(objectMethodDescriptor, interfaceMethodSignature)) {
    Log::e("Method " + interfaceMethodSignature->getName() + " of interface " + mName +
           " has different argument types when implmeneted by object " + object->getTypeName());
    exit(1);
  }
  
  string functionName =
    MethodCall::translateInterfaceMethodToLLVMFunctionName(object,
                                                           this,
                                                           interfaceMethodSignature->getName());
  FunctionType* concreteObjectFunctionType = concreteObjectFunction->getFunctionType();
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(getLLVMType(context));
  for (unsigned int i = 1; i < concreteObjectFunctionType->getNumParams(); i++) {
    argumentTypes.push_back(concreteObjectFunctionType->getParamType(i));
  }
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType* functionType = FunctionType::get(concreteObjectFunctionType->getReturnType(),
                                                 argTypesArray,
                                                 true);

  Function* function = Function::Create(functionType,
                                        GlobalValue::ExternalLinkage,
                                        functionName,
                                        context.getModule());
  return function;
}

void Interface::composeMapFunctionBody(IRGenerationContext& context,
                                       const IObjectType* object,
                                       llvm::Function* concreteObjectFunction,
                                       unsigned long interfaceIndex,
                                       MethodSignature* interfaceMethodSignature) const {
  string functionName =
  MethodCall::translateInterfaceMethodToLLVMFunctionName(object,
                                                         this,
                                                         interfaceMethodSignature->getName());
  Function* function = context.getModule()->getFunction(functionName);

  Function::arg_iterator arguments = function->arg_begin();
  llvm::Argument *argument = &*arguments;
  argument->setName("thisLoaded");
  arguments++;
  argument = &*arguments;
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
  
}

bool Interface::doesMethodHaveUnexpectedExceptions(MethodSignature* interfaceMethodSignature,
                                                   IMethodDescriptor* objectMethodDescriptor,
                                                   string objectName) const {
  map<string, const IType*> interfaceExceptionsMap;
  for (const IType* interfaceException : interfaceMethodSignature->getThrownExceptions()) {
    interfaceExceptionsMap[interfaceException->getTypeName()] = interfaceException;
  }

  bool result = false;
  for (const IType* objectException : objectMethodDescriptor->getThrownExceptions()) {
    if (!interfaceExceptionsMap.count(objectException->getTypeName())) {
      Log::e("Method " + objectMethodDescriptor->getName() + " of object " + objectName +
             " throws an unexpected exception of type " + objectException->getTypeName());
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
  context.getScopes().pushScope();
  
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
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* pointerType = int8Type->getPointerTo();
  Value* castedInterfaceThis = IRWriter::newBitCastInst(context, interfaceThis, pointerType);
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext),
                              -(interfaceIndex + 1) * Environment::getAddressSizeInBytes());
  Value* concreteOjbectThis =
    IRWriter::createGetElementPtrInst(context, castedInterfaceThis, index);
  Value* castedObjectThis = IRWriter::newBitCastInst(context,
                                                     concreteOjbectThis,
                                                     object->getLLVMType(context));
  
  vector<Value*> callArguments;
  callArguments.push_back(castedObjectThis);
  callArguments.push_back(threadReference);
  for (Value* argumentPointer : argumentPointers) {
    Value* loadedCallArgument = IRWriter::newLoadInst(context, argumentPointer, "");
    callArguments.push_back(loadedCallArgument);
  }
  
  if (concreteObjectFunction->getReturnType()->isVoidTy()) {
    IRWriter::createInvokeInst(context, concreteObjectFunction, callArguments, "", 0);
    IRWriter::createReturnInst(context, NULL);
  } else {
    Value* result =
      IRWriter::createInvokeInst(context, concreteObjectFunction, callArguments, "call", 0);
    IRWriter::createReturnInst(context, result);
  }

  context.getScopes().popScope(context, 0);
}

llvm::Value* Interface::storeArgumentValue(IRGenerationContext& context,
                                           BasicBlock* basicBlock,
                                           string variableName,
                                           const IType* variableType,
                                           Value* variableValue) const {
  Type* llvmType = variableType->getLLVMType(context);
  string newName = variableName + ".param";
  AllocaInst *alloc = IRWriter::newAllocaInst(context, llvmType, newName);
  IRWriter::newStoreInst(context, variableValue, alloc);
  return alloc;
}

string Interface::getTypeName() const {
  return mName;
}

string Interface::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::PointerType* Interface::getLLVMType(IRGenerationContext& context) const {
  return mStructType->getPointerTo();
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

string Interface::getCastFunctionName(const IObjectType* toType) const {
  return "cast." + getTypeName() + ".to." + toType->getTypeName();
}

Value* Interface::castTo(IRGenerationContext& context,
                         Value* fromValue,
                         const IType* toType,
                         int line) const {
  const IObjectType* toObjectType = (const IObjectType*) (toType);
  Function* castFunction =
    context.getModule()->getFunction(getCastFunctionName(toObjectType));
  
  if (castFunction == NULL) {
    InstanceOf::getOrCreateFunction(context, this);

    castFunction = defineCastFunction(context, toObjectType);
    context.addComposingCallback2Objects(composeCastFunction, castFunction, this, toObjectType);
  }
  
  vector<Value*> arguments;
  arguments.push_back(fromValue);
  
  Composer::pushCallStack(context, line);
  Value* result = IRWriter::createInvokeInst(context, castFunction, arguments, "castTo", line);
  Composer::popCallStack(context);
  
  return result;
}

Function* Interface::defineCastFunction(IRGenerationContext& context,
                                        const IObjectType* toType) const {
  GetOriginalObjectFunction::get(context);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(getLLVMType(context));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  PointerType* llvmReturnType = (PointerType*) toType->getLLVMType(context);
  FunctionType* functionType = FunctionType::get(llvmReturnType, argTypesArray, false);
  return Function::Create(functionType,
                          GlobalValue::ExternalLinkage,
                          getCastFunctionName(toType),
                          context.getModule());
}

void Interface::composeCastFunction(IRGenerationContext& context,
                                    llvm::Function* function,
                                    const IObjectType* interfaceType,
                                    const IObjectType* toObjectType) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8Type = Type::getInt8Ty(llvmContext);
  Type* llvmReturnType = function->getReturnType();
  context.getScopes().pushScope();

  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisArgument = &*functionArguments;
  thisArgument->setName("this");
  functionArguments++;
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* lessThanZero = BasicBlock::Create(llvmContext, "less.than.zero", function);
  BasicBlock* notLessThanZero = BasicBlock::Create(llvmContext, "not.less.than.zero", function);
  BasicBlock* moreThanZero = BasicBlock::Create(llvmContext, "more.than.zero", function);
  BasicBlock* zeroExactly = BasicBlock::Create(llvmContext, "zero.exactly", function);
  
  context.setBasicBlock(entryBlock);
  const Interface* interface = (const Interface *) interfaceType;
  Value* instanceof = InstanceOf::call(context, interface, thisArgument, toObjectType);
  Value* originalObject = GetOriginalObjectFunction::callGetObject(context, thisArgument);
  ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  ICmpInst* compareLessThanZero =
    IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, instanceof, zero, "cmp");
  IRWriter::createConditionalBranch(context, lessThanZero, notLessThanZero, compareLessThanZero);
  
  context.setBasicBlock(lessThanZero);
  ModelTypeSpecifier* modelTypeSpecifier =
    new ModelTypeSpecifier(Names::getLangPackageName(), Names::getCastExceptionName());
  ObjectBuilderArgumentList objectBuilderArgumnetList;
  llvm::Constant* fromTypeName = IObjectType::getObjectNamePointer(interfaceType, context);
  FakeExpression* fromTypeValue = new FakeExpression(fromTypeName, PrimitiveTypes::STRING_TYPE);
  ObjectBuilderArgument* fromTypeArgument = new ObjectBuilderArgument("withFromType",
                                                                      fromTypeValue);
  llvm::Constant* toTypeName = IObjectType::getObjectNamePointer(toObjectType, context);
  FakeExpression* toTypeValue = new FakeExpression(toTypeName, PrimitiveTypes::STRING_TYPE);
  ObjectBuilderArgument* toTypeArgument = new ObjectBuilderArgument("withToType", toTypeValue);
  objectBuilderArgumnetList.push_back(fromTypeArgument);
  objectBuilderArgumnetList.push_back(toTypeArgument);

  ObjectBuilder* objectBuilder = new ObjectBuilder(modelTypeSpecifier,
                                                   objectBuilderArgumnetList,
                                                   0);
  ThrowStatement throwStatement(objectBuilder, 0);
  context.getScopes().pushScope();
  throwStatement.generateIR(context);
  context.getScopes().popScope(context, 0);

  context.setBasicBlock(notLessThanZero);
  ICmpInst* compareGreaterThanZero =
    IRWriter::newICmpInst(context, ICmpInst::ICMP_SGT, instanceof, zero, "cmp");
  IRWriter::createConditionalBranch(context, moreThanZero, zeroExactly, compareGreaterThanZero);

  context.setBasicBlock(moreThanZero);
  ConstantInt* bytes = ConstantInt::get(Type::getInt32Ty(llvmContext),
                                        Environment::getAddressSizeInBytes());
  BitCastInst* bitcast =
  IRWriter::newBitCastInst(context, originalObject, int8Type->getPointerTo());
  Value* thunkBy = IRWriter::createBinaryOperator(context, Instruction::Mul, instanceof, bytes, "");
  Value* index[1];
  index[0] = thunkBy;
  Value* thunk = IRWriter::createGetElementPtrInst(context, bitcast, index);
  Value* castValue = IRWriter::newBitCastInst(context, thunk, llvmReturnType);
  IRWriter::createReturnInst(context, castValue);

  context.setBasicBlock(zeroExactly);
  castValue = IRWriter::newBitCastInst(context, originalObject, llvmReturnType);
  IRWriter::createReturnInst(context, castValue);
  
  context.getScopes().popScope(context, 0);
}

const IObjectOwnerType* Interface::getOwner() const {
  return mInterfaceOwner;
}

bool Interface::isExternal() const {
  return mIsExternal;
}

void Interface::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "external interface ";
  stream << (isInner() ? getShortName() : getTypeName());
  if (getAccessLevel() == PRIVATE_ACCESS) {
    stream << " {" << endl << "}" << endl;
    return;
  }
  if (mParentInterfaces.size()) {
    stream << endl << "  extends";
  }
  for (Interface* interface : mParentInterfaces) {
    stream << endl << "    " << interface->getTypeName();
    if (interface != mParentInterfaces.at(mParentInterfaces.size() - 1)) {
      stream << ",";
    }
  }
  stream << " {" << endl;
  
  if (mConstants.size()) {
    stream << endl;
  }
  for (Constant* constant : mConstants) {
    constant->printToStream(context, stream);
  }

  if (mMethodSignatures.size()) {
    stream << endl;
  }
  for (MethodSignature* methodSignature : mMethodSignatures) {
    methodSignature->printToStream(context, stream);
  }
  for (StaticMethod* staticMethod : mStaticMethods) {
    staticMethod->printToStream(context, stream);
  }
  map<string, const IObjectType*> innerObjects = getInnerObjects();
  if (innerObjects.size()) {
    stream << endl;
  }
  for (map<string, const IObjectType*>::iterator iterator = innerObjects.begin();
       iterator != innerObjects.end();
       iterator++) {
    iterator->second->printToStream(context, stream);
    stream << endl;
  }
  stream << "}" << endl;
}

void Interface::defineInterfaceTypeName(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, getTypeName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     getObjectNameGlobalVariableName());
}

Instruction* Interface::inject(IRGenerationContext& context,
                               InjectionArgumentList injectionArgumentList,
                               int line) const {
  Controller* controller = context.getBoundController(context.getInterface(getTypeName()));
  return controller->inject(context, injectionArgumentList, line);
}

tuple<vector<MethodSignature*>, vector<wisey::Constant*>, vector<StaticMethod*>>
Interface::createElements(IRGenerationContext& context,
                          vector<IObjectElementDeclaration*> elementDeclarations) {
  vector<MethodSignature*> methodSignatures;
  vector<wisey::Constant*> constants;
  vector<StaticMethod*> staticMethods;
  for (IObjectElementDeclaration* elementDeclaration : elementDeclarations) {
    IObjectElement* objectElement = elementDeclaration->declare(context, this);
    if (objectElement->getObjectElementType() == OBJECT_ELEMENT_FIELD) {
      Log::e("Interfaces can not contain fields");
      exit(1);
    }
    if (objectElement->getObjectElementType() == OBJECT_ELEMENT_METHOD) {
      Log::e("Interfaces can not contain method implmentations");
      exit(1);
    }
    if (objectElement->getObjectElementType() == OBJECT_ELEMENT_CONSTANT) {
      if (methodSignatures.size() || staticMethods.size()) {
        Log::e("In interfaces constants should be declared before methods");
        exit(1);
      }
      constants.push_back((wisey::Constant*) objectElement);
    } else if (objectElement->getObjectElementType() == OBJECT_ELEMENT_METHOD_SIGNATURE) {
      methodSignatures.push_back((MethodSignature*) objectElement);
    } else if (objectElement->getObjectElementType() == OBJECT_ELEMENT_STATIC_METHOD) {
      staticMethods.push_back((StaticMethod*) objectElement);
    } else {
      Log::e("Unexpected element in interface definition");
      exit(1);
    }
  }
  return make_tuple(methodSignatures, constants, staticMethods);
}

void Interface::incremenetReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForInterfaceFunction::call(context, object, 1);
}

void Interface::decremenetReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForInterfaceFunction::call(context, object, -1);
}

Value* Interface::getReferenceCount(IRGenerationContext& context, Value* object) const {
  Value* originalObject = GetOriginalObjectFunction::callGetObject(context, object);
  return getReferenceCountForObject(context, originalObject);
}

string Interface::getDestructorFunctionName() const {
  return "destructor." + mName;
}

Function* Interface::defineDestructorFunction(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(getOwner()->getLLVMType(context));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* voidType = Type::getVoidTy(llvmContext);
  FunctionType* functionType = FunctionType::get(voidType, argTypesArray, false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::ExternalLinkage,
                                        getDestructorFunctionName(),
                                        context.getModule());

  return function;
}

void Interface::composeDestructorFunctionBody(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getModule()->getFunction(getDestructorFunctionName());
  context.getScopes().pushScope();
  
  Function::arg_iterator functionArguments = function->arg_begin();
  Argument* thisArgument = &*functionArguments;
  thisArgument->setName("this");
  functionArguments++;
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.notnull", function);
  
  context.setBasicBlock(entryBlock);
  
  Value* nullValue = ConstantPointerNull::get(getLLVMType(context));
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, thisArgument, nullValue, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNullBlock);
  IRWriter::createReturnInst(context, NULL);
  
  context.setBasicBlock(ifNotNullBlock);
  
  Value* originalObjectVTable = GetOriginalObjectFunction::callGetVTable(context, thisArgument);
  
  Type* pointerToVTablePointer = function->getFunctionType()
    ->getPointerTo()->getPointerTo()->getPointerTo();
  BitCastInst* vTablePointer =
  IRWriter::newBitCastInst(context, originalObjectVTable, pointerToVTablePointer);
  LoadInst* vTable = IRWriter::newLoadInst(context, vTablePointer, "vtable");
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 2);
  GetElementPtrInst* virtualFunction = IRWriter::createGetElementPtrInst(context, vTable, index);
  Function* objectDestructor = (Function*) IRWriter::newLoadInst(context, virtualFunction, "");
  
  ConstantInt* bytes = ConstantInt::get(Type::getInt32Ty(llvmContext),
                                        -Environment::getAddressSizeInBytes());
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* pointerToVTable = IRWriter::newBitCastInst(context,
                                                          originalObjectVTable,
                                                          int8Type->getPointerTo());
  index[0] = bytes;
  Value* thisPointer = IRWriter::createGetElementPtrInst(context, pointerToVTable, index);
  
  Type* argumentType = getLLVMType(context);
  Value* bitcast = IRWriter::newBitCastInst(context, thisPointer, argumentType);
  
  vector<Value*> arguments;
  arguments.push_back(bitcast);
  
  IRWriter::createInvokeInst(context, objectDestructor, arguments, "", 0);
  IRWriter::createReturnInst(context, NULL);
  
  context.getScopes().popScope(context, 0);
}

void Interface::setImportProfile(ImportProfile* importProfile) {
  mImportProfile = importProfile;
}

ImportProfile* Interface::getImportProfile() const {
  return mImportProfile;
}

void Interface::addInnerObject(const IObjectType* innerObject) {
  mInnerObjects[innerObject->getShortName()] = innerObject;
}

const IObjectType* Interface::getInnerObject(string shortName) const {
  if (mInnerObjects.count(shortName)) {
    return mInnerObjects.at(shortName);
  }
  return NULL;
}

map<string, const IObjectType*> Interface::getInnerObjects() const {
  return mInnerObjects;
}

void Interface::markAsInner() {
  mIsInner = true;
}

bool Interface::isInner() const {
  return mIsInner;
}
