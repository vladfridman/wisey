//
//  IRGenerationContext.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Scalar.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

IRGenerationContext::~IRGenerationContext() {
  for(map<string, Model*>::iterator iterator = mModels.begin();
      iterator != mModels.end();
      iterator++) {
    Model* model = iterator->second;
    delete model;
  }
  for(map<string, Controller*>::iterator iterator = mControllers.begin();
      iterator != mControllers.end();
      iterator++) {
    Controller* controller = iterator->second;
    delete controller;
  }
  for(map<string, Interface*>::iterator iterator = mInterfaces.begin();
      iterator != mInterfaces.end();
      iterator++) {
    Interface* interface = iterator->second;
    delete interface;
  }
  mBindings.clear();
  mImports.clear();
}

GenericValue IRGenerationContext::runCode() {
  ExecutionEngine *executionEngine = EngineBuilder(move(mOwner)).create();
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

void IRGenerationContext::addModel(Model* model) {
  string name = model->getName();
  if (mModels.count(name)) {
    Log::e("Redefinition of model " + name);
    exit(1);
  }
  
  mModels[name] = model;
}

Model* IRGenerationContext::getModel(string name) {
  bool isFullName = name.find('.') != string::npos;
  if (!isFullName && mImports.count(name)) {
    return (Model*) getImport(name);
  }
  
  string fullName = isFullName ? name : mPackage + "." + name;
  if (!mModels.count(fullName)) {
    Log::e("Model " + fullName + " is not defined");
    exit(1);
  }
  
  return mModels.at(fullName);
}

void IRGenerationContext::addController(Controller* controller) {
  string name = controller->getName();
  if (mControllers.count(name)) {
    Log::e("Redefinition of controller " + name);
    exit(1);
  }
  
  mControllers[name] = controller;
}

Controller* IRGenerationContext::getController(string name) {
  bool isFullName = name.find('.') != string::npos;
  if (!isFullName && mImports.count(name)) {
    return (Controller*) getImport(name);
  }
  
  string fullName = isFullName ? name : mPackage + "." + name;
  if (!mControllers.count(fullName)) {
    Log::e("Controller " + fullName + " is not defined");
    exit(1);
  }
  
  return mControllers.at(fullName);
}

void IRGenerationContext::addNode(Node* node) {
  string name = node->getName();
  if (mNodes.count(name)) {
    Log::e("Redefinition of node " + name);
    exit(1);
  }
  
  mNodes[name] = node;
}

Node* IRGenerationContext::getNode(string name) {
  bool isFullName = name.find('.') != string::npos;
  if (!isFullName && mImports.count(name)) {
    return (Node*) getImport(name);
  }
  
  string fullName = isFullName ? name : mPackage + "." + name;
  if (!mNodes.count(fullName)) {
    Log::e("Node " + fullName + " is not defined");
    exit(1);
  }
  
  return mNodes.at(fullName);
}

void IRGenerationContext::addInterface(Interface* interface) {
  string name = interface->getName();
  if (mInterfaces.count(name)) {
    Log::e("Redefinition of interface " + name);
    exit(1);
  }
  
  mInterfaces[name] = interface;
}

Interface* IRGenerationContext::getInterface(string name) {
  bool isFullName = name.find('.') != string::npos;
  if (!isFullName && mImports.count(name)) {
    return (Interface*) getImport(name);
  }
  
  string fullName = isFullName ? name : mPackage + "." + name;
  if (!mInterfaces.count(fullName)) {
    Log::e("Interface " + fullName + " is not defined");
    exit(1);
  }
  
  return mInterfaces.at(fullName);
}

void IRGenerationContext::bindInterfaceToController(Interface* interface, Controller* controller) {
  if (mBindings.count(interface)) {
    Log::e("Interface " + interface->getName() + " is already bound to " +
           mBindings[interface]->getName() + " and can not be bound to " + controller->getName());
    exit(1);
  }
  mBindings[interface] = controller;
}

Controller* IRGenerationContext::getBoundController(Interface* interface) {
  if (!mBindings.count(interface)) {
    Log::e("No controller is bound to interface " + interface->getName());
    exit(1);
  }
  return mBindings[interface];
}

void IRGenerationContext::setPackage(string package) {
  if (any_of(begin(package),
             end(package),
             []( char c ) { return (isupper(c)); })) {
    Log::e("Package names should only conain lowercase characters");
    exit(1);
  }
    
  mPackage = package;
}

string IRGenerationContext::getPackage() {
  return mPackage;
}

void IRGenerationContext::addImport(IObjectType* object) {
  mImports[object->getShortName()] = object;
}

IObjectType* IRGenerationContext::getImport(string objectName) {
  if (!mImports.count(objectName)) {
    Log::e("Could not find definition for " + objectName + ". Perhaps it was not imported");
    exit(1);
  }
  return mImports[objectName];
}

void IRGenerationContext::clearAndAddDefaultImports() {
  mImports.clear();
  addImport(getInterface("wisey.lang.IProgram"));
}

Scopes& IRGenerationContext::getScopes() {
  return mScopes;
}

LLVMContext& IRGenerationContext::getLLVMContext() {
  return mLLVMContext;
}

void IRGenerationContext::printAssembly(raw_ostream &outputStream) {
  legacy::PassManager passManager;
  passManager.add(createPrintModulePass(outputStream));
  passManager.run(*mModule);
}

void IRGenerationContext::optimizeIR() {
  legacy::PassManager passManager;
  
  // Optimization: Constant Propagation transform
  passManager.add(createConstantPropagationPass());
  // Optimization: Dead Instruction Elimination transform
  passManager.add(createDeadInstEliminationPass());
  
  passManager.run(*mModule);
}
