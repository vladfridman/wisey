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
#include <llvm/Transforms/Scalar.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IRGenerationContext::IRGenerationContext() :
mMainFunction(NULL),
mBasicBlock(NULL),
mImportProfile(NULL),
mIsDestructorDebugOn(false),
mObjectType(NULL) {
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
  for (map<string, Thread*>::iterator iterator = mThreads.begin();
       iterator != mThreads.end();
       iterator++) {
    delete iterator->second;
  }
  for (map<string, PackageType*>::iterator iterator = mPackageTypes.begin();
       iterator != mPackageTypes.end();
       iterator++) {
    delete iterator->second;
  }
  mBindings.clear();
}

GenericValue IRGenerationContext::runCode() {
  ExecutionEngine *executionEngine = EngineBuilder(move(mModuleOwner)).create();
  vector<GenericValue> noargs;
  if (mMainFunction == NULL) {
    Log::e("Function main is not defined. Exiting.");
    delete executionEngine;
    exit(1);
  }
  Log::i("Running program");
  GenericValue result = executionEngine->runFunction(mMainFunction, noargs);
  Log::i("Result: " + result.IntVal.toString(10, true));
  delete executionEngine;
  
  return result;
}

Module* IRGenerationContext::getModule() {
  return mModule;
}

void IRGenerationContext::setMainFunction(llvm::Function* function) {
  mMainFunction = function;
}

Function* IRGenerationContext::getMainFunction() {
  return mMainFunction;
}

BasicBlock* IRGenerationContext::getBasicBlock() {
  return mBasicBlock;
}

void IRGenerationContext::setBasicBlock(BasicBlock* block) {
  mBasicBlock = block;
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

void IRGenerationContext::addModel(Model* model) {
  string name = model->getTypeName();
  if (mModels.count(name)) {
    Log::e("Redefinition of model " + name);
    exit(1);
  }
  
  mModels[name] = model;
}

Model* IRGenerationContext::getModel(string fullName) {
  if (!mModels.count(fullName)) {
    Log::e("Model " + fullName + " is not defined");
    exit(1);
  }
  
  Model* model = mModels.at(fullName);
  IObjectType::checkAccess(mObjectType, model);
  
  return model;
}

void IRGenerationContext::addController(Controller* controller) {
  string name = controller->getTypeName();
  if (mControllers.count(name)) {
    Log::e("Redefinition of controller " + name);
    exit(1);
  }
  
  mControllers[name] = controller;
}

Controller* IRGenerationContext::getController(string fullName) {
  if (!mControllers.count(fullName)) {
    Log::e("Controller " + fullName + " is not defined");
    exit(1);
  }
  
  Controller* controller = mControllers.at(fullName);
  IObjectType::checkAccess(mObjectType, controller);
  
  return controller;
}

void IRGenerationContext::addNode(Node* node) {
  string name = node->getTypeName();
  if (mNodes.count(name)) {
    Log::e("Redefinition of node " + name);
    exit(1);
  }
  
  mNodes[name] = node;
}

Node* IRGenerationContext::getNode(string fullName) {
  if (!mNodes.count(fullName)) {
    Log::e("Node " + fullName + " is not defined");
    exit(1);
  }
  
  Node* node = mNodes.at(fullName);
  IObjectType::checkAccess(mObjectType, node);
  
  return node;
}

void IRGenerationContext::addThread(Thread* thread) {
  string name = thread->getTypeName();
  if (mThreads.count(name)) {
    Log::e("Redefinition of thread " + name);
    exit(1);
  }
  
  mThreads[name] = thread;
}

Thread* IRGenerationContext::getThread(string fullName) {
  if (!mThreads.count(fullName)) {
    Log::e("Thread " + fullName + " is not defined");
    exit(1);
  }
  
  Thread* thread = mThreads.at(fullName);
  IObjectType::checkAccess(mObjectType, thread);
  
  return thread;
}

void IRGenerationContext::addInterface(Interface* interface) {
  string name = interface->getTypeName();
  if (mInterfaces.count(name)) {
    Log::e("Redefinition of interface " + name);
    exit(1);
  }
  
  mInterfaces[name] = interface;
}

Interface* IRGenerationContext::getInterface(string fullName) {
  if (!mInterfaces.count(fullName)) {
    Log::e("Interface " + fullName + " is not defined");
    exit(1);
  }
  
  Interface* interface = mInterfaces.at(fullName);
  IObjectType::checkAccess(mObjectType, interface);
  
  return interface;
}

void IRGenerationContext::bindInterfaceToController(Interface* interface, Controller* controller) {
  if (mBindings.count(interface)) {
    Log::e("Interface " + interface->getTypeName() + " is already bound to " +
           mBindings[interface]->getTypeName() + " and can not be bound to " +
           controller->getTypeName());
    exit(1);
  }
  mBindings[interface] = controller;
}

Controller* IRGenerationContext::getBoundController(Interface* interface) {
  if (!hasBoundController(interface)) {
    Log::e("No controller is bound to interface " + interface->getTypeName());
    exit(1);
  }
  return mBindings[interface];
}

bool IRGenerationContext::hasBoundController(Interface* interface) {
  return mBindings.count(interface);
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
  return getScopes().getVariable("this");
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
  for (map<Interface*, Controller*>::const_iterator iterator = mBindings.begin();
       iterator != mBindings.end();
       iterator++) {
    Interface* interface = iterator->first;
    Controller* controller = iterator->second;
    stream << "bind(" << controller->getTypeName() << ").to(" << interface->getTypeName() << ");";
    stream << endl;
  }
  if (mBindings.size()) {
    stream << endl;
  }
}

void IRGenerationContext::optimizeIR() {
  legacy::PassManager passManager;
  
  // Optimization: Constant Propagation transform
  passManager.add(createConstantPropagationPass());
  // Optimization: Dead Instruction Elimination transform
  passManager.add(createDeadInstEliminationPass());
  
  passManager.run(*mModule);
}

void IRGenerationContext::turnDestructorDebugOn() {
  mIsDestructorDebugOn = true;
}

bool IRGenerationContext::isDestructorDebugOn() const {
  return mIsDestructorDebugOn;
}

void IRGenerationContext::addComposingCallback0Objects(ComposingFunction0Objects callback,
                                                       Function* function) {
  mComposingCallbacks0Objects.push_back(make_tuple(callback, function));
}

void IRGenerationContext::addComposingCallback1Objects(ComposingFunction1Objects callback,
                                                       Function* function,
                                                       const IObjectType* objectType) {
  mComposingCallbacks1Objects.push_back(make_tuple(callback, function, objectType));
}

void IRGenerationContext::addComposingCallback2Objects(ComposingFunction2Objects callback,
                                                       Function* function,
                                                       const IObjectType* objectType1,
                                                       const IObjectType* objectType2) {
  mComposingCallbacks2Objects.push_back(make_tuple(callback, function, objectType1, objectType2));
}

void IRGenerationContext::runComposingCallbacks() {
  
  while(mComposingCallbacks0Objects.size() ||
        mComposingCallbacks1Objects.size() ||
        mComposingCallbacks2Objects.size()) {
    unsigned long callbacks0ObjectsSize = mComposingCallbacks0Objects.size();
    for (tuple<ComposingFunction0Objects, Function*> callback : mComposingCallbacks0Objects) {
      ComposingFunction0Objects composingFunction = get<0>(callback);
      Function* function = get<1>(callback);
      composingFunction(*this, function);
    }
    for (unsigned i = 0; i < callbacks0ObjectsSize; i++) {
      mComposingCallbacks0Objects.pop_front();
    }
    
    unsigned long callbacks1ObjectsSize = mComposingCallbacks1Objects.size();
    for (tuple<ComposingFunction1Objects, Function*, const IObjectType*> callback :
         mComposingCallbacks1Objects) {
      ComposingFunction1Objects composingFunction = get<0>(callback);
      Function* function = get<1>(callback);
      const IObjectType* objectType = get<2>(callback);
      composingFunction(*this, function, objectType);
    }
    for (unsigned i = 0; i < callbacks1ObjectsSize; i++) {
      mComposingCallbacks1Objects.pop_front();
    }

    unsigned long callbacks2ObjectsSize = mComposingCallbacks2Objects.size();
    for (tuple<ComposingFunction2Objects, Function*,
         const IObjectType*, const IObjectType*> callback : mComposingCallbacks2Objects) {
      ComposingFunction2Objects composingFunction = get<0>(callback);
      Function* function = get<1>(callback);
      const IObjectType* objectType1 = get<2>(callback);
      const IObjectType* objectType2 = get<3>(callback);
      composingFunction(*this, function, objectType1, objectType2);
    }
    for (unsigned i = 0; i < callbacks2ObjectsSize; i++) {
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

