//
//  Interface.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCountFunction.hpp"
#include "wisey/Argument.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/Cast.hpp"
#include "wisey/CastObjectFunction.hpp"
#include "wisey/Composer.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/InstanceOfFunction.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/LocalSystemReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectBuilder.hpp"
#include "wisey/ParameterPrimitiveVariable.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrintOutStatement.hpp"
#include "wisey/StringLiteral.hpp"
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
                     vector<IObjectElementDefinition *> elementDelcarations,
                     ImportProfile* importProfile,
                     int line) :
mIsPublic(accessLevel == PUBLIC_ACCESS),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false),
mInterfaceOwner(new InterfaceOwner(this)),
mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
mElementDeclarations(elementDelcarations),
mImportProfile(importProfile),
mIsComplete(false),
mLine(line) {
  assert(importProfile && "Import profile can not be NULL at Interface creation");
}

Interface::~Interface() {
  delete mInterfaceOwner;
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
  for (LLVMFunction* llvmFunction : mLLVMFunctions) {
    delete llvmFunction;
  }
  mLLVMFunctions.clear();
  mLLVMFunctionMap.clear();
}

Interface* Interface::newInterface(AccessLevel accessLevel,
                                   string name,
                                   StructType *structType,
                                   vector<IInterfaceTypeSpecifier *> parentInterfaceSpecifiers,
                                   vector<IObjectElementDefinition *>
                                   elementDeclarations,
                                   ImportProfile* importProfile,
                                   int line) {
  return new Interface(accessLevel,
                       name,
                       structType,
                       false,
                       parentInterfaceSpecifiers,
                       elementDeclarations,
                       importProfile,
                       line);
}

Interface* Interface::newExternalInterface(string name,
                                           StructType *structType,
                                           vector<IInterfaceTypeSpecifier *>
                                           parentInterfaceSpecifiers,
                                           vector<IObjectElementDefinition *>
                                           elementDeclarations,
                                           ImportProfile* importProfile,
                                           int line) {
  return new Interface(AccessLevel::PUBLIC_ACCESS,
                       name,
                       structType,
                       true,
                       parentInterfaceSpecifiers,
                       elementDeclarations,
                       importProfile,
                       line);
}

bool Interface::isPublic() const {
  return mIsPublic;
}

void Interface::buildMethods(IRGenerationContext& context) {
  if (mIsComplete) {
    return;
  }

  LLVMContext& llvmContext = context.getLLVMContext();
  assert(mImportProfile && "Import profile is not set in an interface");
  context.setImportProfile(mImportProfile);

  tuple<vector<MethodSignature*>, vector<wisey::Constant*>, vector<StaticMethod*>,
  vector<LLVMFunction*>> elements = createElements(context, mElementDeclarations);

  mMethodSignatures = get<0>(elements);
  mConstants = get<1>(elements);
  for (Constant* constant : mConstants) {
    mNameToConstantMap[constant->getName()] = constant;
  }
  mStaticMethods = get<2>(elements);
  for (StaticMethod* staticMethod : mStaticMethods) {
    mNameToStaticMethodMap[staticMethod->getName()] = staticMethod;
  }
  mLLVMFunctions = get<3>(elements);
  for (LLVMFunction* llvmFunction : mLLVMFunctions) {
    mLLVMFunctionMap[llvmFunction->getName()] = llvmFunction;
  }

  processParentInterfaces(context);
  processMethodSignatures(context);
  
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

void Interface::processParentInterfaces(IRGenerationContext& context) {
  for (IInterfaceTypeSpecifier* parentInterfaceSpecifier : mParentInterfaceSpecifiers) {
    Interface* parentInterface = (Interface*) parentInterfaceSpecifier->getType(context);
    if (mParentInterfacesMap.count(parentInterface->getTypeName())) {
      context.reportError(mLine, "Circular interface dependency between interfaces " +
                          getTypeName() + " and " + parentInterface->getTypeName());
      throw 1;
    }
    mParentInterfacesMap[parentInterface->getTypeName()] = parentInterface;
    if (!parentInterface->isComplete()) {
      parentInterface->buildMethods(context);
    }
    mParentInterfaces.push_back(parentInterface);
  }
}

void Interface::processMethodSignatures(IRGenerationContext& context) {
  for (MethodSignature* methodSignature : mMethodSignatures) {
    mNameToMethodSignatureMap[methodSignature->getName()] = methodSignature;
    mAllMethodSignatures.push_back(methodSignature);
  }
  set<string> methodOverrides;
  for (MethodSignature* methodSignature : mMethodSignatures) {
    if (methodSignature->isOverride()) {
      methodOverrides.insert(methodSignature->getName());
    }
  }
  for (Interface* parentInterface : mParentInterfaces) {
    includeInterfaceMethods(context, parentInterface, methodOverrides);
  }
  for (string methodName : methodOverrides) {
    MethodSignature* methodSignature = mNameToMethodSignatureMap[methodName];
    context.reportError(methodSignature->getMethodQualifiers()->getLine(),
                        "Method signature '" + methodName + "' is marked override but it does not "
                        "override any signatures from parent interfaces");
    throw 1;
  }
  unsigned long index = 0;
  for (const MethodSignature* methodSignature : mAllMethodSignatures) {
    mMethodIndexes[methodSignature] = index;
    index++;
  }
}

wisey::Constant* Interface::findConstant(IRGenerationContext& context,
                                         string constantName,
                                         int line) const {
  if (!mNameToConstantMap.count(constantName)) {
    context.reportError(line, "Interface " + mName + " does not have constant named " +
                        constantName);
    throw 1;
  }
  return mNameToConstantMap.at(constantName);
}

bool Interface::isComplete() const {
  return mIsComplete;
}

void Interface::includeInterfaceMethods(IRGenerationContext& context,
                                        Interface* parentInterface,
                                        set<string>& methodOverrides) {
  vector<MethodSignature*> inheritedMethods = parentInterface->getAllMethodSignatures();
  for (MethodSignature* methodSignature : inheritedMethods) {
    methodOverrides.erase(methodSignature->getName());
    IMethodDescriptor* existingMethod = findMethod(methodSignature->getName());
    if (existingMethod && !IMethodDescriptor::compare(existingMethod, methodSignature)) {
      context.reportError(existingMethod->getMethodQualifiers()->getLine(),
                          "Interface " + mName + " overrides method '" + existingMethod->getName()
                          + "' of parent interface with a wrong signature");
      throw 1;
    }
    if (existingMethod && !existingMethod->isOverride()) {
      context.reportError(existingMethod->getMethodQualifiers()->getLine(),
                          "Method '" + existingMethod->getName() + "' in interface " + mName
                          + " must be marked override because it overrides a method of the same "
                          "name from " + methodSignature->getOriginalParentName());
      throw 1;
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

unsigned long Interface::getMethodIndex(IRGenerationContext& context,
                                        const IMethodDescriptor* methodDescriptor,
                                        int line) const {
  if (!mMethodIndexes.count(methodDescriptor)) {
    context.reportError(line, "Method " + methodDescriptor->getName() + " not found in interface " +
                        getTypeName());
    throw 1;
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

IMethodDescriptor* Interface::findMethod(string methodName) const {
  if (mNameToMethodSignatureMap.count(methodName)) {
    return mNameToMethodSignatureMap.at(methodName);
  }
  
  if (mNameToStaticMethodMap.count(methodName)) {
    return mNameToStaticMethodMap.at(methodName);
  }
  
  return NULL;
}

IMethod* Interface::findStaticMethod(string methodName) const {
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

void Interface::defineLLVMFunctions(IRGenerationContext& context) const {
  for (LLVMFunction* function : mLLVMFunctions) {
    function->declareFunction(context, this);
  }
}

LLVMFunction* Interface::findLLVMFunction(string functionName) const {
  if (mLLVMFunctionMap.count(functionName)) {
    return mLLVMFunctionMap.at(functionName);
  }
  return NULL;
}

void Interface::defineCurrentObjectNameVariable(IRGenerationContext& context) const {
  Value* objectName = IObjectType::getObjectNamePointer(this, context);
  ParameterPrimitiveVariable* objectNameVariable =
  new ParameterPrimitiveVariable(Names::getCurrentObjectVariableName(),
                                 PrimitiveTypes::STRING,
                                 objectName,
                                 0);
  context.getScopes().setVariable(context, objectNameVariable);
}

void Interface::generateStaticMethodsIR(IRGenerationContext& context) const {
  for (IMethod* method : mStaticMethods) {
    method->generateIR(context);
  }
}

void Interface::generateLLVMFunctionsIR(IRGenerationContext& context) const {
  for (LLVMFunction* function : mLLVMFunctions) {
    function->generateBodyIR(context, this);
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
    context.reportError(object->getLine(),
                        "Method " + interfaceMethodSignature->getName() + " of interface " +
                        interfaceMethodSignature->getOriginalParentName() +
                        " is not implemented by object " + object->getTypeName());
    throw 1;
  }
  
  if (objectMethodDescriptor->getReturnType() != interfaceMethodSignature->getReturnType()) {
    context.reportError(objectMethodDescriptor->getMethodQualifiers()->getLine(),
                        "Method " + interfaceMethodSignature->getName() + " of interface " + mName +
                        " has different return type when implmeneted by object " +
                        object->getTypeName());
    throw 1;
  }
  
  if (doesMethodHaveUnexpectedExceptions(context,
                                         interfaceMethodSignature,
                                         objectMethodDescriptor,
                                         object->getTypeName())) {
    context.reportError(objectMethodDescriptor->getMethodQualifiers()->getLine(),
                        "Exceptions thrown by method " +  interfaceMethodSignature->getName() +
                        " of interface " + mName +
                        " do not reconcile with exceptions thrown by its " +
                        "implementation in object " + object->getTypeName());
    throw 1;
  }
  
  if (!objectMethodDescriptor->isOverride()) {
    context.reportError(objectMethodDescriptor->getMethodQualifiers()->getLine(),
                        "Object " + object->getTypeName() + " should mark method '" +
                        interfaceMethodSignature->getName() +
                        "' with 'override' qualifier because it overrides the method defined in "
                        "the parent interface " +
                        interfaceMethodSignature->getOriginalParentName());
    throw 1;
  }
  
  if (!IMethodDescriptor::compare(objectMethodDescriptor, interfaceMethodSignature)) {
    context.reportError(objectMethodDescriptor->getMethodQualifiers()->getLine(),
                        "Method " + interfaceMethodSignature->getName() + " of interface " + mName +
                        " has different argument types when implmeneted by object " +
                        object->getTypeName());
    throw 1;
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
  FunctionType* functionType = FunctionType::get(concreteObjectFunctionType->getReturnType(),
                                                 argumentTypes,
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
  vector<const Argument*> methodArguments = interfaceMethodSignature->getArguments();
  for (const Argument* methodArgument : interfaceMethodSignature->getArguments()) {
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

bool Interface::doesMethodHaveUnexpectedExceptions(IRGenerationContext& context,
                                                   MethodSignature* interfaceMethodSignature,
                                                   IMethodDescriptor* objectMethodDescriptor,
                                                   string objectName) const {
  map<string, const IType*> interfaceExceptionsMap;
  for (const IType* interfaceException : interfaceMethodSignature->getThrownExceptions()) {
    interfaceExceptionsMap[interfaceException->getTypeName()] = interfaceException;
  }

  bool result = false;
  for (const IType* objectException : objectMethodDescriptor->getThrownExceptions()) {
    if (!interfaceExceptionsMap.count(objectException->getTypeName())) {
      context.reportError(objectMethodDescriptor->getMethodQualifiers()->getLine(),
                          "Method " + objectMethodDescriptor->getName() + " of object " +
                          objectName + " throws an unexpected exception of type " +
                          objectException->getTypeName());
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
  Value* callStackReference = &*arguments;
  arguments++;
  vector<Value*> argumentPointers;
  for (const  Argument* methodArgument : methodSignature->getArguments()) {
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
                              -interfaceIndex * Environment::getAddressSizeInBytes());
  Value* concreteOjbectThis =
    IRWriter::createGetElementPtrInst(context, castedInterfaceThis, index);
  Value* castedObjectThis = IRWriter::newBitCastInst(context,
                                                     concreteOjbectThis,
                                                     object->getLLVMType(context));
  
  vector<Value*> callArguments;
  callArguments.push_back(castedObjectThis);
  callArguments.push_back(threadReference);
  callArguments.push_back(callStackReference);
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

/**
 * If object type assume we can perform cast and check at run time
 */
bool Interface::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (canAutoCastTo(context, toType)) {
    return true;
  }

  if (!IType::isObjectType(toType)) {
    return false;
  }
  
  return true;
}

bool Interface::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType == this) {
    return true;
  }

  if (toType->isNative() && (toType->isReference() || toType->isPointer())) {
    return true;
  }

  if (toType == PrimitiveTypes::BOOLEAN) {
    return true;
  }

  if (!IType::isObjectType(toType)) {
    return false;
  }

  if (toType->isInterface() && doesExtendInterface((const Interface*) toType)) {
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
  if (toType->isNative() && (toType->isReference() || toType->isPointer())) {
    return IRWriter::newBitCastInst(context, fromValue, toType->getLLVMType(context));
  }
  if (toType == PrimitiveTypes::BOOLEAN) {
    return IRWriter::newICmpInst(context,
                                 ICmpInst::ICMP_NE,
                                 fromValue,
                                 ConstantPointerNull::get(getLLVMType(context)),
                                 "");
  }

  
  assert(toType->isReference());
  assert(IObjectType::isObjectType(toType));

  const IObjectType* toObjectType = (const IObjectType*) (toType);
  
  Composer::setLineNumber(context, line);
  Value* result = CastObjectFunction::call(context, fromValue, toObjectType, line);
  
  return result;
}

bool Interface::isPrimitive() const {
  return false;
}

bool Interface::isOwner() const {
  return false;
}

bool Interface::isReference() const {
  return true;
}

bool Interface::isArray() const {
  return false;
}

bool Interface::isFunction() const {
  return false;
}

bool Interface::isPackage() const {
  return false;
}

bool Interface::isController() const {
  return false;
}

bool Interface::isInterface() const {
  return true;
}

bool Interface::isModel() const {
  return false;
}

bool Interface::isNode() const {
  return false;
}

bool Interface::isNative() const {
  return false;
}

bool Interface::isPointer() const {
  return false;
}

bool Interface::isImmutable() const {
  return false;
}

const IObjectOwnerType* Interface::getOwner() const {
  return mInterfaceOwner;
}

bool Interface::isExternal() const {
  return mIsExternal;
}

void Interface::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "external interface ";
  stream << getTypeName();
  if (!isPublic()) {
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
  stream << "}" << endl;
  map<string, const IObjectType*> innerObjects = getInnerObjects();
  for (map<string, const IObjectType*>::iterator iterator = innerObjects.begin();
       iterator != innerObjects.end();
       iterator++) {
    stream << endl;
    iterator->second->printToStream(context, stream);
  }
}

void Interface::defineInterfaceTypeName(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  string typeType = getTypeName();
  llvm::Constant* stringConstant = ConstantDataArray::getString(llvmContext, typeType);
  new GlobalVariable(*context.getModule(),
                     llvm::ArrayType::get(Type::getInt8Ty(llvmContext), typeType.length() + 1),
                     true,
                     GlobalValue::LinkageTypes::ExternalLinkage,
                     mIsExternal ? nullptr : stringConstant,
                     getObjectNameGlobalVariableName());
}

void Interface::defineExternalInjectionFunctionPointer(IRGenerationContext& context) const {
  FunctionType* functionType = FunctionType::get(getLLVMType(context), false);
  new GlobalVariable(*context.getModule(),
                     functionType->getPointerTo(),
                     false,
                     GlobalValue::ExternalLinkage,
                     NULL,
                     getInjectFunctionVariableName());
}

void Interface::defineInjectionFunctionPointer(IRGenerationContext& context) const {
  FunctionType* functionType = FunctionType::get(getLLVMType(context), false);
  new GlobalVariable(*context.getModule(),
                     functionType->getPointerTo(),
                     false,
                     GlobalValue::ExternalLinkage,
                     ConstantPointerNull::get(functionType->getPointerTo()),
                     getInjectFunctionVariableName(),
                     nullptr,
                     GlobalValue::NotThreadLocal,
                     0,
                     true);
}

Value* Interface::inject(IRGenerationContext& context,
                         InjectionArgumentList injectionArgumentList,
                         int line) const {
  const Interface* interface = context.getInterface(getTypeName(), line);
  if (context.hasBoundController(interface) && injectionArgumentList.size()) {
    const Controller* controller = context.getBoundController(interface, line);
    Value* controllerValue = controller->inject(context, injectionArgumentList, line);
    return controller->castTo(context, controllerValue, this, line);
  }
  if (injectionArgumentList.size()) {
    context.reportError(line, "Arguments are not allowed for injection of interfaces "
                        "that are not bound to controllers");
    throw 1;
  }
  
  Function* function = getOrCreateEmptyInjectFunction(context, line);
  vector<Value*> arguments;
  return IRWriter::createCallInst(context, function, arguments, "");
}

Function* Interface::getOrCreateEmptyInjectFunction(IRGenerationContext& context, int line) const {
  Function* function = context.getModule()->getFunction(getInjectWrapperFunctionName());
  if (function) {
    return function;
  }
  FunctionType* functionType = FunctionType::get(getLLVMType(context), false);
  function = Function::Create(functionType,
                              GlobalValue::ExternalLinkage,
                              getInjectWrapperFunctionName(),
                              context.getModule());
  context.addComposingCallback1Objects(composeEmptyInjectFunction, function, this);

  return function;
}

void Interface::composeEmptyInjectFunction(IRGenerationContext& context,
                                           Function* function,
                                           const void* object) {
  const Interface* interface = (const Interface*) object;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function, 0);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.not.null", function, 0);

  context.getScopes().pushScope();
  context.setBasicBlock(entryBlock);
  Value* actualInjectFunctionPointer =
  context.getModule()->getNamedGlobal(interface->getInjectFunctionVariableName());
  Value* actualInjectFunction = IRWriter::newLoadInst(context, actualInjectFunctionPointer, "");
  FunctionType* functionType = FunctionType::get(interface->getLLVMType(context), false);
  Value* null = ConstantPointerNull::get(functionType->getPointerTo());
  Value* condition = IRWriter::newICmpInst(context,
                                           ICmpInst::ICMP_EQ,
                                           actualInjectFunction,
                                           null,
                                           "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNotNullBlock);
  vector<Value*> callArguments;
  Value* injectValue = IRWriter::createCallInst(context,
                                                (Function*) actualInjectFunction,
                                                callArguments,
                                                "");
  IRWriter::createReturnInst(context, injectValue);
  
  context.setBasicBlock(ifNullBlock);
  ExpressionList printOutArguments;
  StringLiteral* stringLiteral = new StringLiteral("Fatal error: Interface " +
                                                   interface->getTypeName() +
                                                   " is not bound to any controllers\n", 0);
  printOutArguments.push_back(stringLiteral);
  PrintOutStatement::printExpressionList(context, printOutArguments, 0);

  ConstantInt* one = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Function* exitFunction = context.getModule()->getFunction("exit");
  vector<Value*> arguments;
  arguments.push_back(one);
  IRWriter::createCallInst(context, exitFunction, arguments, "");
  IRWriter::newUnreachableInst(context);
  
  context.getScopes().popScope(context, 0);
}

string Interface::getInjectWrapperFunctionName() const {
  return getTypeName() + ".inject";
}

string Interface::getInjectFunctionVariableName() const {
  return getTypeName() + ".inject.pointer";
}

string Interface::getInjectFunctionName() const {
  return getTypeName() + ".inject.function";
}

Value* Interface::composeInjectFunctionWithController(IRGenerationContext& context,
                                                      const Controller* controller) const {
  FunctionType* functionType = FunctionType::get(getLLVMType(context), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::ExternalLinkage,
                                        getInjectFunctionName(),
                                        context.getModule());
  context.addComposingCallback2Objects(composeInjectWithControllerFunction,
                                       function,
                                       this,
                                       controller);
  GlobalVariable* functionPointer = context.getModule()->
  getNamedGlobal(getInjectFunctionVariableName());
  assert(functionPointer && "Global containing interface inject function pointer is not found");
  IRWriter::newStoreInst(context, function, functionPointer);

  return function;
}

void Interface::composeInjectWithControllerFunction(IRGenerationContext& context,
                                                    Function* function,
                                                    const void* object1,
                                                    const void* object2) {
  const Interface* interface = (const Interface*) object1;
  const Controller* controller = (const Controller*) object2;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function, 0);
  
  context.getScopes().pushScope();
  context.setBasicBlock(basicBlock);
  
  InjectionArgumentList injectionArgumentList;
  Value* value = controller->inject(context, injectionArgumentList, 0);
  Value* bitcast = controller->castTo(context, value, interface, 0);
  IRWriter::createReturnInst(context, bitcast);
  
  context.getScopes().popScope(context, 0);
}

tuple<vector<MethodSignature*>, vector<wisey::Constant*>, vector<StaticMethod*>,
vector<LLVMFunction*>>
Interface::createElements(IRGenerationContext& context,
                          vector<IObjectElementDefinition*> elementDeclarations) {
  vector<MethodSignature*> methodSignatures;
  vector<wisey::Constant*> constants;
  vector<StaticMethod*> staticMethods;
  vector<LLVMFunction*> functions;
  map<string, IObjectElement*> nameToElementMap;
  for (IObjectElementDefinition* elementDeclaration : elementDeclarations) {
    IObjectElement* objectElement = elementDeclaration->define(context, this);
    if (objectElement->isField()) {
      context.reportError(objectElement->getLine(), "Interfaces can not contain fields");
      throw 1;
    }
    if (objectElement->isMethod()) {
      context.reportError(objectElement->getLine(),
                          "Interfaces can not contain method implmentations");
      throw 1;
    }
    if (objectElement->isConstant()) {
      if (methodSignatures.size() || staticMethods.size()) {
        context.reportError(objectElement->getLine(),
                            "In interfaces constants should be declared before methods");
        throw 1;
      }
      wisey::Constant* constant = (wisey::Constant*) objectElement;
      if (nameToElementMap.count(constant->getName())) {
        context.reportError(constant->getLine(), "Constant named '" + constant->getName() +
                            "' was already defined on line " +
                            to_string(nameToElementMap.at(constant->getName())->getLine()));
        throw 1;
      }
      nameToElementMap[constant->getName()] = constant;
      constants.push_back(constant);
    } else if (objectElement->isMethodSignature()) {
      MethodSignature* methodSignature = (MethodSignature*) objectElement;
      if (nameToElementMap.count(methodSignature->getName())) {
        context.reportError(methodSignature->getLine(), "Method signature '" +
                            methodSignature->getName() + "' was already defined on line " +
                            to_string(nameToElementMap.at(methodSignature->getName())->getLine()));
        throw 1;
      }
      nameToElementMap[methodSignature->getName()] = methodSignature;
      methodSignatures.push_back(methodSignature);
    } else if (objectElement->isStaticMethod()) {
      StaticMethod* staticMethod = (StaticMethod*) objectElement;
      if (nameToElementMap.count(staticMethod->getName())) {
        context.reportError(staticMethod->getLine(), "Static method '" +
                            staticMethod->getName() + "' was already defined on line " +
                            to_string(nameToElementMap.at(staticMethod->getName())->getLine()));
        throw 1;
      }
      nameToElementMap[staticMethod->getName()] = staticMethod;
      staticMethods.push_back(staticMethod);
    } else if (objectElement->isLLVMFunction()) {
      LLVMFunction* llvmFunction = (LLVMFunction*) objectElement;
      if (nameToElementMap.count(llvmFunction->getName())) {
        context.reportError(llvmFunction->getLine(), "Function or method '" +
                            llvmFunction->getName() + "' was already defined on line " +
                            to_string(nameToElementMap.at(llvmFunction->getName())->getLine()));
        throw 1;
      }
      nameToElementMap[llvmFunction->getName()] = llvmFunction;
      functions.push_back((LLVMFunction*) objectElement);
    } else {
      context.reportError(objectElement->getLine(),
                          "Unexpected element in interface definition");
      throw 1;
    }
  }
  return make_tuple(methodSignatures, constants, staticMethods, functions);
}

void Interface::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, 1);
}

void Interface::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCountFunction::call(context, object, -1);
}

Value* Interface::getReferenceCount(IRGenerationContext& context, Value* object) const {
  Value* originalObject = GetOriginalObjectFunction::call(context, object);
  return getReferenceCountForObject(context, originalObject);
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

void Interface::createLocalVariable(IRGenerationContext& context,
                                    string name,
                                    int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = name == ThreadExpression::THREAD
    ? (IVariable*) new LocalSystemReferenceVariable(name, this, alloca, line)
    : (IVariable*) new LocalReferenceVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, uninitializedVariable);
}

void Interface::createFieldVariable(IRGenerationContext& context,
                                    string name,
                                    const IConcreteObjectType* object,
                                    int line) const {
  IVariable* variable = new FieldReferenceVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void Interface::createParameterVariable(IRGenerationContext& context,
                                        string name,
                                        Value* value,
                                        int line) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value, line);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* Interface::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

int Interface::getLine() const {
  return mLine;
}
