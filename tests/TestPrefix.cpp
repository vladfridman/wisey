//
//  TestPrefix.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/TypeBuilder.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockReferenceVariable.hpp"
#include "TestPrefix.hpp"
#include "wisey/ConstantDefinition.hpp"
#include "wisey/ControllerDefinition.hpp"
#include "wisey/FakeExpressionWithCleanup.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LLVMStructType.hpp"
#include "wisey/LLVMStructSpecifier.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedFieldDefinition.hpp"
#include "wisey/StaticMethodDefinition.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/WiseyObjectTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

void TestPrefix::generateIR(IRGenerationContext& context) {
  ImportProfile* importProfile = new ImportProfile("systems.vos.wisey.compiler.tests");
  context.setImportProfile(importProfile);
  importProfile->setSourceFileName(context, "/tmp/source.yz");
  
  defineStdErrGlobal(context);

  InterfaceDefinition* exceptionInterfaceDefinition = defineIException(context);
  exceptionInterfaceDefinition->prototypeObject(context, importProfile);
  InterfaceDefinition* threadInterfaceDefinition = defineIThread(context);
  threadInterfaceDefinition->prototypeObject(context, importProfile);
  ControllerDefinition* callStackDefinition = defineCCallStack(context);
  callStackDefinition->prototypeObject(context, importProfile);

  vector<IObjectElementDefinition*> modelElements;
  defineExceptionModel(context, Names::getNPEModelName(), modelElements);
  const PrimitiveTypeSpecifier* longTypeSpecifier = PrimitiveTypes::LONG->newTypeSpecifier(0);
  modelElements.push_back(ReceivedFieldDefinition::create(longTypeSpecifier, "mReferenceCount", 0));
  const PrimitiveTypeSpecifier* stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  modelElements.push_back(ReceivedFieldDefinition::create(stringTypeSpecifier, "mObjectType", 0));
  
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  InterfaceTypeSpecifierFull* interfaceTypeSpecifier =
  new InterfaceTypeSpecifierFull(packageExpression, Names::getExceptionInterfaceName(), 0);
  modelElements.push_back(ReceivedFieldDefinition::create(interfaceTypeSpecifier,
                                                          "mNestedException",
                                                          0));
  defineExceptionModel(context, Names::getReferenceCountExceptionName(), modelElements);
  modelElements.clear();
  stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  modelElements.push_back(ReceivedFieldDefinition::create(stringTypeSpecifier, "mInterfaceName", 0));
  defineExceptionModel(context, Names::getInterfaceNotBoundExceptionName(), modelElements);
  modelElements.clear();

  stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  modelElements.push_back(ReceivedFieldDefinition::create(stringTypeSpecifier, "mFromType", 0));
  stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  modelElements.push_back(ReceivedFieldDefinition::create(stringTypeSpecifier, "mToType", 0));
  defineExceptionModel(context, Names::getCastExceptionName(), modelElements);
  modelElements.clear();

  longTypeSpecifier = PrimitiveTypes::LONG->newTypeSpecifier(0);
  modelElements.push_back(ReceivedFieldDefinition::create(longTypeSpecifier, "mArraySize", 0));
  longTypeSpecifier = PrimitiveTypes::LONG->newTypeSpecifier(0);
  modelElements.push_back(ReceivedFieldDefinition::create(longTypeSpecifier, "mIndex", 0));
  defineExceptionModel(context, Names::getArrayIndexOutOfBoundsModelName(), modelElements);
  
  ControllerDefinition* contextManagerDefinition = defineCContextManager(context);
  
  contextManagerDefinition->prototypeObject(context, importProfile);
  threadInterfaceDefinition->prototypeMethods(context);
  callStackDefinition->prototypeMethods(context);
  contextManagerDefinition->prototypeMethods(context);
  
  defineStructs(context);

  ControllerDefinition* memoryPoolDefinition = defineCMemoryPool(context);
  memoryPoolDefinition->prototypeObject(context, importProfile);
  memoryPoolDefinition->prototypeMethods(context);

  defineIntrinsicFunctions(context);

  context.getScopes().pushScope();
  
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(context));
  NiceMock<MockReferenceVariable>* threadVariable = new NiceMock<MockReferenceVariable>();
  ON_CALL(*threadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
  ON_CALL(*threadVariable, getType()).WillByDefault(Return(threadInterface));
  ON_CALL(*threadVariable, generateIdentifierIR(_, _)).WillByDefault(Return(threadObject));
  context.getScopes().setVariable(context, threadVariable);
  
  Controller* callStack = context.getController(Names::getCallStackControllerFullName(), 0);
  Value* callStackValue = ConstantPointerNull::get(callStack->getLLVMType(context));
  NiceMock<MockReferenceVariable>*  callStackVariable = new NiceMock<MockReferenceVariable>();
  ON_CALL(*callStackVariable, getName()).WillByDefault(Return(ThreadExpression::CALL_STACK));
  ON_CALL(*callStackVariable, getType()).WillByDefault(Return(callStack));
  ON_CALL(*callStackVariable, generateIdentifierIR(_, _)).WillByDefault(Return(callStackValue));
  context.getScopes().setVariable(context, callStackVariable);
  
  ::testing::Mock::AllowLeak(threadVariable);
  ::testing::Mock::AllowLeak(callStackVariable);
}

void TestPrefix::defineStdErrGlobal(IRGenerationContext& context) {
  StructType* structType = StructType::create(context.getLLVMContext(), "__sFILE");
  new GlobalVariable(*context.getModule(),
                     structType,
                     false,
                     GlobalValue::ExternalLinkage,
                     nullptr,
                     "__stderrp");
}

void TestPrefix::defineIntrinsicFunctions(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Module* module = context.getModule();
  
  FunctionType* functionType = llvm::TypeBuilder<int(char *, ...), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "printf", module);
  
  functionType = llvm::TypeBuilder<int(types::i<8>*, char *, ...), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "fprintf", module);
  
  functionType = llvm::TypeBuilder<long(types::i<8>*), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "strlen", module);

  functionType = TypeBuilder<void (types::i<8>*,
                                   types::i<8>*,
                                   types::i<8>*), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "__cxa_throw", module);

  functionType = TypeBuilder<types::i<8>* (types::i<64>), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "__cxa_allocate_exception", module);

  functionType = TypeBuilder<void (types::i<8>*), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "__cxa_call_unexpected", module);

  functionType = TypeBuilder<types::i<8>* (types::i<8>*), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "__cxa_begin_catch", module);
  
  functionType = TypeBuilder<void (), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "__cxa_end_catch", module);
  
  functionType = TypeBuilder<types::i<8>* (types::i<8>*), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "__cxa_get_exception_ptr", module);

  functionType = TypeBuilder<types::i<32> (...), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "__gxx_personality_v0", module);

  functionType = TypeBuilder<types::i<32> (types::i<8>*), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "llvm.eh.typeid.for", module);

  functionType = TypeBuilder<void (types::i<8>*,
                                   types::i<8>*,
                                   types::i<64>,
                                   types::i<32>,
                                   types::i<1>), false>::get(llvmContext);
  Function::Create(functionType, GlobalValue::ExternalLinkage, "llvm.memcpy.p0i8.p0i8.i64", module);

  functionType = TypeBuilder<void (types::i<8>*,
                                   types::i<8>,
                                   types::i<64>,
                                   types::i<32>,
                                   types::i<1>), false>::get(llvmContext);
  
  Function::Create(functionType, GlobalValue::ExternalLinkage, "llvm.memset.p0i8.i64", module);
}

void TestPrefix::defineExceptionModel(IRGenerationContext& context,
                                      string modelName,
                                      vector<IObjectElementDefinition*> modelElements) {
  vector<IInterfaceTypeSpecifier*> modelParentInterfaces;
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ModelTypeSpecifierFull* modelTypeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, modelName, 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  modelTypeSpecifier,
                                  modelElements,
                                  modelParentInterfaces,
                                  innerObjectDefinitions,
                                  false,
                                  0);
  modelDefinition.prototypeObject(context, context.getImportProfile());
  modelDefinition.prototypeMethods(context);
  Model* model = context.getModel(Names::getLangPackageName() + "." + modelName, 0);
  model->declareRTTI(context);
  Interface* exceptionInterface = context.getInterface(Names::getExceptionInterfaceFullName(), 0);
  vector<Interface*> interfaces;
  interfaces.push_back(exceptionInterface);
  model->setInterfaces(interfaces);
}

ControllerDefinition* TestPrefix::defineCCallStack(IRGenerationContext& context) {
  vector<IModelTypeSpecifier*> exceptions;
  VariableList arguments;
  
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID->newTypeSpecifier(0);
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  StaticMethodDefinition* throwExceptionMethod =
  new StaticMethodDefinition(PUBLIC_ACCESS,
                             voidTypeSpecifier,
                             Names::getThrowStackOverflowMethodName(),
                             arguments,
                             exceptions,
                             compoundStatement,
                             0);

  PackageType* packageType = new PackageType(Names::getThreadsPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* controllerTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getCallStackControllerName(), 0);
  
  ConstantDefinition* constantDefinition =
  new ConstantDefinition(PUBLIC_ACCESS,
                         PrimitiveTypes::INT->newTypeSpecifier(0),
                         Names::getCallStackSizeConstantName(),
                         new IntConstant(1024, 0),
                         0);

  vector<IObjectElementDefinition*> elementDeclarations;
  elementDeclarations.push_back(constantDefinition);
  elementDeclarations.push_back(throwExceptionMethod);

  vector<IInterfaceTypeSpecifier*> interfaceSpecifiers;
  vector<IObjectDefinition*> innerObjectDefinitions;
  
  return new ControllerDefinition(AccessLevel::PUBLIC_ACCESS,
                                  controllerTypeSpecifier,
                                  elementDeclarations,
                                  interfaceSpecifiers,
                                  innerObjectDefinitions,
                                  NULL,
                                  0);
}

ControllerDefinition* TestPrefix::defineCContextManager(IRGenerationContext& context) {
  const PrimitiveTypeSpecifier* stringTypeSpecifier;
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  VariableDeclaration* declaration;
  stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  declaration = VariableDeclaration::create(stringTypeSpecifier,
                                            new Identifier("contextName", 0),
                                            0);
  arguments.push_back(declaration);
  stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  declaration = VariableDeclaration::create(stringTypeSpecifier,
                                            new Identifier("objectName", 0),
                                            0);
  arguments.push_back(declaration);
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* getInstance = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                       new WiseyObjectTypeSpecifier(0),
                                                       Names::getGetInstanceMethodName(),
                                                       arguments,
                                                       exceptions,
                                                       new MethodQualifiers(0),
                                                       compoundStatement,
                                                       0);

  arguments.clear();
  stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  declaration = VariableDeclaration::create(stringTypeSpecifier,
                                            new Identifier("contextName", 0),
                                            0);
  arguments.push_back(declaration);
  stringTypeSpecifier = PrimitiveTypes::STRING->newTypeSpecifier(0);
  declaration = VariableDeclaration::create(stringTypeSpecifier,
                                            new Identifier("objectName", 0),
                                            0);
  arguments.push_back(declaration);
  declaration = VariableDeclaration::create(new WiseyObjectTypeSpecifier(0),
                                            new Identifier("instance", 0),
                                            0);
  arguments.push_back(declaration);
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* setInstance = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                       PrimitiveTypes::VOID->newTypeSpecifier(0),
                                                       Names::getSetInstanceMethodName(),
                                                       arguments,
                                                       exceptions,
                                                       new MethodQualifiers(0),
                                                       compoundStatement,
                                                       0);

  arguments.clear();
  
  PackageType* packageType = new PackageType(Names::getThreadsPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* controllerTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getContextManagerName(), 0);
  vector<IObjectElementDefinition*> elementDeclarations;
  elementDeclarations.push_back(getInstance);
  elementDeclarations.push_back(setInstance);
  
  vector<IInterfaceTypeSpecifier*> interfaceSpecifiers;
  vector<IObjectDefinition*> innerObjectDefinitions;
  
  return new ControllerDefinition(AccessLevel::PUBLIC_ACCESS,
                                  controllerTypeSpecifier,
                                  elementDeclarations,
                                  interfaceSpecifiers,
                                  innerObjectDefinitions,
                                  NULL,
                                  0);
}

ControllerDefinition* TestPrefix::defineCMemoryPool(IRGenerationContext& context) {
  const PrimitiveTypeSpecifier* longSpecifier = PrimitiveTypes::LONG->newTypeSpecifier(0);
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  LLVMPointerTypeSpecifier* pointerSpecifier =
  new LLVMPointerTypeSpecifier(LLVMPrimitiveTypes::I8->newTypeSpecifier(0), 0);
  MethodDefinition* methodAllocate = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                          pointerSpecifier,
                                                          Names::getInitializeMethodName(),
                                                          arguments,
                                                          exceptions,
                                                          new MethodQualifiers(0),
                                                          compoundStatement,
                                                          0);
  
  arguments.clear();
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* methodClear = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                       PrimitiveTypes::VOID->newTypeSpecifier(0),
                                                       Names::getClearMethodName(),
                                                       arguments,
                                                       exceptions,
                                                       new MethodQualifiers(0),
                                                       compoundStatement,
                                                       0);
  
  arguments.clear();
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* methodDestroy = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                         PrimitiveTypes::VOID->newTypeSpecifier(0),
                                                         Names::getDestroyMethodName(),
                                                         arguments,
                                                         exceptions,
                                                         new MethodQualifiers(0),
                                                         compoundStatement,
                                                         0);
  
  pointerSpecifier =
  new LLVMPointerTypeSpecifier(LLVMPrimitiveTypes::I8->newTypeSpecifier(0), 0);
  arguments.clear();
  LLVMPointerTypeSpecifier* poolSpecifier =
  new LLVMPointerTypeSpecifier(new LLVMStructSpecifier("AprPool", 0), 0);
  arguments.push_back(VariableDeclaration::create(poolSpecifier, new Identifier("pool", 0), 0));
  longSpecifier = PrimitiveTypes::LONG->newTypeSpecifier(0);
  VariableDeclaration* declaration =
  VariableDeclaration::create(longSpecifier, new Identifier("size", 0), 0);
  arguments.push_back(declaration);
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  StaticMethodDefinition* methodPalloc = new StaticMethodDefinition(PUBLIC_ACCESS,
                                                                    pointerSpecifier,
                                                                    Names::getPallocateMethodName(),
                                                                    arguments,
                                                                    exceptions,
                                                                    compoundStatement,
                                                                    0);

  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* controllerTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getCMemoryPoolName(), 0);
  vector<IObjectElementDefinition*> elementDeclarations;
  elementDeclarations.push_back(methodAllocate);
  elementDeclarations.push_back(methodClear);
  elementDeclarations.push_back(methodDestroy);
  elementDeclarations.push_back(methodPalloc);

  vector<IInterfaceTypeSpecifier*> interfaceSpecifiers;
  vector<IObjectDefinition*> innerObjectDefinitions;
  
  return new ControllerDefinition(AccessLevel::PUBLIC_ACCESS,
                                  controllerTypeSpecifier,
                                  elementDeclarations,
                                  interfaceSpecifiers,
                                  innerObjectDefinitions,
                                  NULL,
                                  0);
}

InterfaceDefinition* TestPrefix::defineIThread(IRGenerationContext& context) {
  PackageType* packageType = new PackageType(Names::getThreadsPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  InterfaceTypeSpecifierFull* interfaceTypeSpecifier =
  new InterfaceTypeSpecifierFull(packageExpression, Names::getThreadInterfaceName(), 0);
  vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers;
  vector<IObjectElementDefinition *> elementDeclarations;
  vector<IObjectDefinition*> innerObjectDefinitions;
  
  packageType = new PackageType(Names::getThreadsPackageName());
  packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* contextManagerSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getContextManagerName(), 0);
  VariableList arguments;
  vector<IModelTypeSpecifier*> thrownExceptions;
  MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
  MethodSignatureDeclaration* getContextManager =
  new MethodSignatureDeclaration(contextManagerSpecifier,
                                 Names::getGetContextManagerMethodName(),
                                 arguments,
                                 thrownExceptions,
                                 methodQualifiers,
                                 0);
  
  elementDeclarations.push_back(getContextManager);

  return new InterfaceDefinition(AccessLevel::PUBLIC_ACCESS,
                                 interfaceTypeSpecifier,
                                 parentInterfaceSpecifiers,
                                 elementDeclarations,
                                 innerObjectDefinitions,
                                 0);
}

InterfaceDefinition* TestPrefix::defineIException(IRGenerationContext& context) {
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  InterfaceTypeSpecifierFull* interfaceTypeSpecifier =
  new InterfaceTypeSpecifierFull(packageExpression, Names::getExceptionInterfaceName(), 0);
  vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers;
  vector<IObjectElementDefinition *> elementDeclarations;
  vector<IObjectDefinition*> innerObjectDefinitions;

  return new InterfaceDefinition(AccessLevel::PUBLIC_ACCESS,
                                 interfaceTypeSpecifier,
                                 parentInterfaceSpecifiers,
                                 elementDeclarations,
                                 innerObjectDefinitions,
                                 0);
}

void TestPrefix::defineStructs(IRGenerationContext& context) {
  StructType* aprPool = StructType::create(context.getLLVMContext(), "AprPool");
  LLVMStructType* aprPoolLLVM = LLVMStructType::newLLVMStructType(aprPool);
  context.addLLVMStructType(aprPoolLLVM, 0);
}
