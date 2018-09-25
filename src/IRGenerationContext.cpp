//
//  IRGenerationContext.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <sstream>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>

#include "IRGenerationContext.hpp"
#include "Log.hpp"
#include "Names.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IRGenerationContext::IRGenerationContext() :
mBasicBlock(NULL),
mImportProfile(NULL),
mIsDestructorDebugOn(false),
mShouldGenerateMain(true),
mObjectType(NULL),
mCurrentMethod(NULL),
mIsRunningComposingCallbacks(false) {
  mModuleOwner = llvm::make_unique<llvm::Module>("wisey", mLLVMContext);
  mModule = mModuleOwner.get();
}

IRGenerationContext::~IRGenerationContext() {
  for (map<string, ArrayType*>::iterator iterator = mArrayTypes.begin();
       iterator != mArrayTypes.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, ArrayExactType*>::iterator iterator = mArrayExactTypes.begin();
       iterator != mArrayExactTypes.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, LLVMArrayType*>::iterator iterator = mLLVMArrayTypes.begin();
       iterator != mLLVMArrayTypes.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, Model*>::iterator iterator = mModels.begin();
       iterator != mModels.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, Controller*>::iterator iterator = mControllers.begin();
       iterator != mControllers.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, Interface*>::iterator iterator = mInterfaces.begin();
       iterator != mInterfaces.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, Node*>::iterator iterator = mNodes.begin();
       iterator != mNodes.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, PackageType*>::iterator iterator = mPackageTypes.begin();
       iterator != mPackageTypes.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, LLVMStructType*>::iterator iterator = mLLVMStructTypes.begin();
       iterator != mLLVMStructTypes.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, LLVMFunctionType*>::iterator iterator = mLLVMFunctionTypes.begin();
       iterator != mLLVMFunctionTypes.end();
       iterator++) {
    delete iterator->second;
  }
  mLLVMInternalFunctionNamedTypes.clear();
  mLLVMExternalFunctionNamedTypes.clear();
  mBindings.clear();
}

int IRGenerationContext::runCode(int argc, char** argv) {
  ExecutionEngine* executionEngine = EngineBuilder(move(mModuleOwner)).create();
  uint64_t rawMainAddress = executionEngine->getFunctionAddress("main");
  if (!rawMainAddress) {
    Log::errorNoSourceFile("Function main is not defined. Exiting.");
    delete executionEngine;
    exit(1);
  }

  Log::i("Running program");
  int (*main)(int, char**) = (int (*)(int, char**)) rawMainAddress;
  int result = main(argc, argv);
  Log::i("Result: " + to_string(result));
  delete executionEngine;
  
  return result;
}

Module* IRGenerationContext::getModule() {
  return mModule;
}

BasicBlock* IRGenerationContext::getBasicBlock() {
  return mBasicBlock;
}

void IRGenerationContext::setBasicBlock(BasicBlock* block) {
  mBasicBlock = block;
}

BasicBlock* IRGenerationContext::getDeclarationsBlock() {
  return mDeclarationsBlock;
}

void IRGenerationContext::setDeclarationsBlock(BasicBlock* block) {
  mDeclarationsBlock = block;
}

void IRGenerationContext::reset() {
  mScopes.clear();
  mBasicBlock = NULL;
  mObjectType = NULL;
}

wisey::ArrayType* IRGenerationContext::getArrayType(const IType* elementType,
                                                    unsigned long numberOfDimensions) {
  string key = elementType->getTypeName();
  for (unsigned long i = 0u; i < numberOfDimensions; i++) {
    key = key + "[]";
  }
  if (mArrayTypes.count(key)) {
    return mArrayTypes.at(key);
  }
  ArrayType* arrayType = new ArrayType(elementType, numberOfDimensions);
  mArrayTypes[key] = arrayType;
  
  return arrayType;
}

ArrayExactType* IRGenerationContext::getArrayExactType(const IType* elementType,
                                                       list<unsigned long> dimensions) {
  string key = elementType->getTypeName();
  for (long dimension : dimensions) {
    key = key + "[" + to_string(dimension) + "]";
  }
  if (mArrayExactTypes.count(key)) {
    return mArrayExactTypes.at(key);
  }
  ArrayExactType* arrayExactType = new ArrayExactType(elementType, dimensions);
  mArrayExactTypes[key] = arrayExactType;
  
  return arrayExactType;
}

LLVMArrayType* IRGenerationContext::getLLVMArrayType(const IType* elementType,
                                                     list<unsigned long> dimensions) {
  string key = elementType->getTypeName();
  for (long dimension : dimensions) {
    key = key + "[" + to_string(dimension) + "]";
  }
  if (mLLVMArrayTypes.count(key)) {
    return mLLVMArrayTypes.at(key);
  }
  LLVMArrayType* llvmArrayType = new LLVMArrayType(elementType, dimensions);
  mLLVMArrayTypes[key] = llvmArrayType;
  
  return llvmArrayType;
}

void IRGenerationContext::addModel(Model* model, int line) {
  string name = model->getTypeName();
  if (mModels.count(name)) {
    reportError(line, "Redefinition of model " + name);
    throw 1;
  }
  
  mModels[name] = model;
}

Model* IRGenerationContext::getModel(string fullName, int line) const {
  if (!mModels.count(fullName)) {
    reportError(line, "Model " + fullName + " is not defined");
    throw 1;
  }
  
  Model* model = mModels.at(fullName);
  IObjectType::checkAccess(this, mImportProfile->getSourceFileName(), mObjectType, model, line);
  
  return model;
}

void IRGenerationContext::addController(Controller* controller, int line) {
  string name = controller->getTypeName();
  if (mControllers.count(name)) {
    reportError(line, "Redefinition of controller " + name);
    throw 1;
  }
  
  mControllers[name] = controller;
}

Controller* IRGenerationContext::getController(string fullName, int line) const {
  if (!mControllers.count(fullName)) {
    reportError(line, "Controller " + fullName + " is not defined");
    throw 1;
  }
  
  Controller* controller = mControllers.at(fullName);
  IObjectType::checkAccess(this, mImportProfile->getSourceFileName(), mObjectType, controller, line);
  
  return controller;
}

void IRGenerationContext::addNode(Node* node, int line) {
  string name = node->getTypeName();
  if (mNodes.count(name)) {
    reportError(line, "Redefinition of node " + name);
    throw 1;
  }
  
  mNodes[name] = node;
}

Node* IRGenerationContext::getNode(string fullName, int line) const {
  if (!mNodes.count(fullName)) {
    reportError(line, "Node " + fullName + " is not defined");
    throw 1;
  }
  
  Node* node = mNodes.at(fullName);
  IObjectType::checkAccess(this, mImportProfile->getSourceFileName(), mObjectType, node, line);
  
  return node;
}

void IRGenerationContext::addInterface(Interface* interface, int line) {
  string name = interface->getTypeName();
  if (mInterfaces.count(name)) {
    reportError(line, "Redefinition of interface " + name);
    throw 1;
  }
  
  mInterfaces[name] = interface;
}

Interface* IRGenerationContext::getInterface(string fullName, int line) const {
  if (!mInterfaces.count(fullName)) {
    reportError(line, "Interface " + fullName + " is not defined");
    throw 1;
  }
  
  Interface* interface = mInterfaces.at(fullName);
  IObjectType::checkAccess(this, mImportProfile->getSourceFileName(), mObjectType, interface, line);
  
  return interface;
}

void IRGenerationContext::addLLVMStructType(LLVMStructType* llvmStructType, int line) {
  string name = llvmStructType->getTypeName();
  if (mLLVMStructTypes.count(name)) {
    reportError(line, "Redefinition of llvm struct type " + name);
    throw 1;
  }
  
  mLLVMStructTypes[name] = llvmStructType;
}

LLVMStructType* IRGenerationContext::getLLVMStructType(string name, int line) {
  string fullName = LLVMStructType::LLVM_STRUCT_PREFIX + name;
  if (!mLLVMStructTypes.count(fullName)) {
    Log::e(getImportProfile()->getSourceFileName(),
           line,
           "llvm struct type " + fullName + " is not defined");
    throw 1;
  }
  
  return mLLVMStructTypes.at(fullName);
}

LLVMFunctionType* IRGenerationContext::getLLVMFunctionType(const IType* returnType,
                                                           vector<const IType*> argumentTypes) {
  string key = getLLVMFunctionKeyPrefix(returnType, argumentTypes) + ")";

  if (mLLVMFunctionTypes.count(key)) {
    return mLLVMFunctionTypes.at(key);
  }
  
  LLVMFunctionType* llvmFunctionType = LLVMFunctionType::create(returnType, argumentTypes);
  mLLVMFunctionTypes[key] = llvmFunctionType;
  return llvmFunctionType;
}

LLVMFunctionType* IRGenerationContext::
getLLVMFunctionTypeWithVarArg(const IType* returnType, vector<const IType*> argumentTypes) {
  string key = getLLVMFunctionKeyPrefix(returnType, argumentTypes);
  if (argumentTypes.size()) {
    key = key + ", ...)";
  } else {
    key = key + "...)";
  }

  if (mLLVMFunctionTypes.count(key)) {
    return mLLVMFunctionTypes.at(key);
  }
  
  LLVMFunctionType* llvmFunctionType = LLVMFunctionType::createWithVarArg(returnType,
                                                                          argumentTypes);
  mLLVMFunctionTypes[key] = llvmFunctionType;
  return llvmFunctionType;
}

string IRGenerationContext::getLLVMFunctionKeyPrefix(const IType* returnType,
                                                     vector<const IType *> argumentTypes) const {
  string key = returnType->getTypeName() + " (";
  for (const IType* argumentType : argumentTypes) {
    key = key + argumentType->getTypeName();
    if (argumentType != argumentTypes.back()) {
      key = key + ", ";
    }
  }

  return key;
}

void IRGenerationContext::setLLVMGlobalVariable(const IType* type, string name) {
  mGlobalVariables[name] = type;
}

const IType* IRGenerationContext::getLLVMGlobalVariableType(string name) const {
  if (mGlobalVariables.count(name)) {
    return mGlobalVariables.at(name);
  }
  return NULL;
}

void IRGenerationContext::registerLLVMInternalFunctionNamedType(string name,
                                                                const LLVMFunctionType*
                                                                functionType,
                                                                int line) {
  if (mLLVMInternalFunctionNamedTypes.count(name) || mLLVMExternalFunctionNamedTypes.count(name)) {
    reportError(line, "Can not register llvm function named " + name +
                " because it is already registered");
    throw 1;
  }
  mLLVMInternalFunctionNamedTypes[name] = functionType;
}

void IRGenerationContext::registerLLVMExternalFunctionNamedType(string name,
                                                                const LLVMFunctionType*
                                                                functionType,
                                                                int line) {
  if (mLLVMInternalFunctionNamedTypes.count(name) || mLLVMExternalFunctionNamedTypes.count(name)) {
    reportError(line, "Can not register llvm function named " + name +
                " because it is already registered");
    throw 1;
  }
  mLLVMExternalFunctionNamedTypes[name] = functionType;
}

const LLVMFunctionType* IRGenerationContext::lookupLLVMFunctionNamedType(string name, int line) {
  if (mLLVMInternalFunctionNamedTypes.count(name)) {
    return mLLVMInternalFunctionNamedTypes.at(name);
  }
  if (mLLVMExternalFunctionNamedTypes.count(name)) {
    return mLLVMExternalFunctionNamedTypes.at(name);
  }

  reportError(line, "Can not find llvm function named " + name);
  throw 1;
}

void IRGenerationContext::bindInterfaceToController(string interfaceName,
                                                    string controllerName,
                                                    int line) {
  if (mBindings.count(interfaceName)) {
    reportError(line, "Interface " + interfaceName + " is already bound to " +
                get<0>(mBindings[interfaceName]) + " and can not be bound to " + controllerName);
    throw 1;
  }
  mBindings[interfaceName] = make_tuple(controllerName, line);
}

const Controller* IRGenerationContext::getBoundController(const Interface* interface,
                                                          int line) const {
  if (!hasBoundController(interface)) {
    reportError(line, "No controller is bound to interface " + interface->getTypeName());
    throw 1;
  }
  return getController(get<0>(mBindings.at(interface->getTypeName())), line);
}

bool IRGenerationContext::hasBoundController(const Interface* interface) const {
  return mBindings.count(interface->getTypeName());
}

void IRGenerationContext::bindInterfaces(IRGenerationContext& context) const {
  for (auto iterator = mBindings.begin(); iterator != mBindings.end(); iterator++) {
    int line = get<1>(iterator->second);
    const Interface* interface = getInterface(iterator->first, line);
    const Controller* controller = getController(get<0>(iterator->second), line);
    if (!controller->getReceivedFields().size()) {
      interface->composeInjectFunctionWithController(context, controller, line);
    }
  }
}

PackageType* IRGenerationContext::getPackageType(string packageName) {
  if (mPackageTypes.count(packageName)) {
    return mPackageTypes.at(packageName);
  }
  PackageType* packageType = new PackageType(packageName);
  mPackageTypes[packageName] = packageType;
  
  return packageType;
}

void IRGenerationContext::setImportProfile(ImportProfile* importProfile) {
  mImportProfile = importProfile;
}

ImportProfile* IRGenerationContext::getImportProfile() const {
  return mImportProfile;
}

Scopes& IRGenerationContext::getScopes() {
  return mScopes;
}

IVariable* IRGenerationContext::getThis() {
  return getScopes().getVariable(IObjectType::THIS);
}

LLVMContext& IRGenerationContext::getLLVMContext() {
  return mLLVMContext;
}

void IRGenerationContext::printAssembly(raw_ostream &outputStream) {
  legacy::PassManager passManager;
  passManager.add(createPrintModulePass(outputStream));
  passManager.run(*mModule);
}

void IRGenerationContext::printToStream(IRGenerationContext& context, iostream& stream) const {
  
  stream << "/* Interfaces */" << endl << endl;
  for (map<string, Interface*>::const_iterator iterator = mInterfaces.begin();
       iterator != mInterfaces.end();
       iterator++) {
    Interface* interface = iterator->second;
    if (interface->isExternal()) {
      continue;
    }
    interface->printToStream(context, stream);
    stream << endl;
  }
  
  stream << "/* Models */" << endl << endl;
  for (map<string, Model*>::const_iterator iterator = mModels.begin();
       iterator != mModels.end();
       iterator++) {
    Model* model = iterator->second;
    if (model->isExternal() || model->isInner()) {
      continue;
    }
    model->printToStream(context, stream);
    stream << endl;
  }
  
  stream << "/* Controllers */" << endl << endl;
  for (map<string, Controller*>::const_iterator iterator = mControllers.begin();
       iterator != mControllers.end();
       iterator++) {
    Controller* controller = iterator->second;
    if (controller->isExternal() || controller->isInner()) {
      continue;
    }
    controller->printToStream(context, stream);
    stream << endl;
  }
  
  stream << "/* Nodes */" << endl << endl;
  for (map<string, Node*>::const_iterator iterator = mNodes.begin();
       iterator != mNodes.end();
       iterator++) {
    Node* node = iterator->second;
    if (node->isExternal() || node->isInner()) {
      continue;
    }
    node->printToStream(context, stream);
    stream << endl;
  }
  
  stream << "/* Bindings */" << endl << endl;
  for (auto iterator = mBindings.begin(); iterator != mBindings.end(); iterator++) {
    const string interfaceName = iterator->first;
    const string controllerName = get<0>(iterator->second);
    stream << "bind(" << interfaceName << ").to(" << controllerName << ");";
    stream << endl;
  }
  if (mBindings.size()) {
    stream << endl;
  }

  stream << "/* llvm Structs */" << endl << endl;
  for (map<string, LLVMStructType*>::const_iterator iterator = mLLVMStructTypes.begin();
       iterator != mLLVMStructTypes.end();
       iterator++) {
    LLVMStructType* structType = iterator->second;
    if (structType->isExternal()) {
      continue;
    }
    structType->printToStream(context, stream);
    stream << endl;
  }
  
  stream << "/* llvm Functions */" << endl << endl;
  bool hasPrintedFunctions = false;
  for (map<string, const LLVMFunctionType*>::const_iterator iterator =
       mLLVMInternalFunctionNamedTypes.begin();
       iterator != mLLVMInternalFunctionNamedTypes.end();
       iterator++) {
    string name = iterator->first;
    hasPrintedFunctions = true;
    const LLVMFunctionType* functionType = iterator->second;
    stream << "external ::llvm::function " << functionType->getReturnType()->getTypeName() << " ";
    stream << "\"" << name << "\"" << "(";
    vector<const IType*> argumentTypes = functionType->getArgumentTypes();
    for (vector<const IType*>::iterator iterator = argumentTypes.begin();
         iterator != argumentTypes.end();
         iterator++) {
      stream << (*iterator)->getTypeName();
      if (iterator + 1 != argumentTypes.end()) {
        stream << ", ";
      }
    }
    if (functionType->isVarArg()) {
      if (argumentTypes.size()) {
        stream << ", ...";
      } else {
        stream << "...";
      }
    }
    stream << ");";
    stream << endl;
  }
  if (hasPrintedFunctions) {
    stream << endl;
  }

  stream << "/* llvm Globals */" << endl << endl;
  for (map<string, const IType*>::const_iterator iterator = mGlobalVariables.begin();
       iterator != mGlobalVariables.end();
       iterator++) {
    string name = iterator->first;
    const IType* type = iterator->second;
    stream << "external " << type->getTypeName() << " " << name << ";";
    stream << endl;
  }
}

void IRGenerationContext::turnDestructorDebugOn() {
  mIsDestructorDebugOn = true;
}

bool IRGenerationContext::isDestructorDebugOn() const {
  return mIsDestructorDebugOn;
}

void IRGenerationContext::setShouldNotGenerateMain() {
  mShouldGenerateMain = false;
}

bool IRGenerationContext::shouldGenerateMain() const {
  return mShouldGenerateMain;
}

void IRGenerationContext::addComposingCallback0Objects(ComposingFunction0Objects callback,
                                                       Function* function) {
  mComposingCallbacks0Objects.push_back(make_tuple(callback, function));
}

void IRGenerationContext::addComposingCallback1Objects(ComposingFunction1Objects callback,
                                                       Function* function,
                                                       const void* object) {
  mComposingCallbacks1Objects.push_back(make_tuple(callback, function, object));
}

void IRGenerationContext::addComposingCallback2Objects(ComposingFunction2Objects callback,
                                                       Function* function,
                                                       const void* object1,
                                                       const void* object2) {
  mComposingCallbacks2Objects.push_back(make_tuple(callback, function, object1, object2));
}

void IRGenerationContext::runComposingCallbacks() {
  mIsRunningComposingCallbacks = true;
  while(mComposingCallbacks0Objects.size() ||
        mComposingCallbacks1Objects.size() ||
        mComposingCallbacks2Objects.size()) {
    auto callbacks0Objects = mComposingCallbacks0Objects;
    for (auto callback : callbacks0Objects) {
      ComposingFunction0Objects composingFunction = get<0>(callback);
      Function* function = get<1>(callback);
      composingFunction(*this, function);
    }
    for (unsigned i = 0; i < callbacks0Objects.size(); i++) {
      mComposingCallbacks0Objects.pop_front();
    }
    
    auto callbacks1Objects = mComposingCallbacks1Objects;
    for (auto callback : mComposingCallbacks1Objects) {
      ComposingFunction1Objects composingFunction = get<0>(callback);
      Function* function = get<1>(callback);
      const void* object = get<2>(callback);
      composingFunction(*this, function, object);
    }
    for (unsigned i = 0; i < callbacks1Objects.size(); i++) {
      mComposingCallbacks1Objects.pop_front();
    }

    auto callbacks2Objects = mComposingCallbacks2Objects;
    for (auto callback : mComposingCallbacks2Objects) {
      ComposingFunction2Objects composingFunction = get<0>(callback);
      Function* function = get<1>(callback);
      const void* object1 = get<2>(callback);
      const void* object2 = get<3>(callback);
      composingFunction(*this, function, object1, object2);
    }
    for (unsigned i = 0; i < callbacks2Objects.size(); i++) {
      mComposingCallbacks2Objects.pop_front();
    }
  }
}

void IRGenerationContext::setObjectType(const IObjectType* objectType) {
  mObjectType = objectType;
}

const IObjectType* IRGenerationContext::getObjectType() const {
  return mObjectType;
}

void IRGenerationContext::setCurrentMethod(const IMethod* method) {
  mCurrentMethod = method;
}

const IMethod* IRGenerationContext::getCurrentMethod() const {
  return mCurrentMethod;
}

bool IRGenerationContext::isRunningComposingCallbacks() const {
  return mIsRunningComposingCallbacks;
}

void IRGenerationContext::reportError(int line, string message) const {
  Log::e(mImportProfile->getSourceFileName(), line, message);
}
