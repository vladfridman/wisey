//
//  TestPrefix.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "TestPrefix.hpp"
#include "wisey/ControllerDefinition.hpp"
#include "wisey/FakeExpressionWithCleanup.hpp"
#include "wisey/FixedFieldDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadDefinition.hpp"
#include "wisey/ThreadTypeSpecifierFull.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void TestPrefix::generateIR(IRGenerationContext& context) {
  ImportProfile* importProfile = new ImportProfile("systems.vos.wisey.compiler.tests");
  context.setImportProfile(importProfile);
  importProfile->setSourceFileName(context, "/tmp/source.yz");
  
  defineStdErrGlobal(context);
  
  vector<IObjectElementDefinition*> modelElements;
  defineModel(context, Names::getNPEModelName(), modelElements);
  const PrimitiveTypeSpecifier* longTypeSpecifier = PrimitiveTypes::LONG_TYPE->newTypeSpecifier(0);
  modelElements.push_back(new FixedFieldDefinition(longTypeSpecifier, "mReferenceCount", 0));
  defineModel(context, Names::getReferenceCountExceptionName(), modelElements);
  modelElements.clear();
  
  const PrimitiveTypeSpecifier* stringTypeSpecifier =
  PrimitiveTypes::STRING_TYPE->newTypeSpecifier(0);
  modelElements.push_back(new FixedFieldDefinition(stringTypeSpecifier, "mFromType", 0));
  stringTypeSpecifier = PrimitiveTypes::STRING_TYPE->newTypeSpecifier(0);
  modelElements.push_back(new FixedFieldDefinition(stringTypeSpecifier, "mToType", 0));
  defineModel(context, Names::getCastExceptionName(), modelElements);
  modelElements.clear();

  longTypeSpecifier = PrimitiveTypes::LONG_TYPE->newTypeSpecifier(0);
  modelElements.push_back(new FixedFieldDefinition(longTypeSpecifier, "mArraySize", 0));
  longTypeSpecifier = PrimitiveTypes::LONG_TYPE->newTypeSpecifier(0);
  modelElements.push_back(new FixedFieldDefinition(longTypeSpecifier, "mIndex", 0));
  defineModel(context, Names::getArrayIndexOutOfBoundsModelName(), modelElements);
  
  InterfaceDefinition* threadInterfaceDefinition = defineThreadInterface(context);
  ControllerDefinition* callStackDefinition = defineCallStackController(context);
  ThreadDefinition* mainThreadDefinition = defineMainThread(context);
  
  threadInterfaceDefinition->prototypeObject(context);
  threadInterfaceDefinition->prototypeMethods(context);
  callStackDefinition->prototypeObject(context);
  mainThreadDefinition->prototypeObject(context);
  callStackDefinition->prototypeMethods(context);
  mainThreadDefinition->prototypeMethods(context);
  
  defineThreadGuardFunctions(context);
}

void TestPrefix::defineThreadGuardFunctions(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Controller* callStackController =
  context.getController(Names::getCallStackControllerFullName(), 0);
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(threadInterface->getLLVMType(context));
  argumentTypes.push_back(callStackController->getLLVMType(context));
  argumentTypes.push_back(threadInterface->getLLVMType(context));
  FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext),
                                                 argumentTypes,
                                                 false);
  
  Function::Create(functionType,
                   GlobalValue::ExternalLinkage,
                   Names::getCheckConcealedMethodCallFunctionName(),
                   context.getModule());
  
  Function::Create(functionType,
                   GlobalValue::ExternalLinkage,
                   Names::getCheckRevealedMethodCallFunctionName(),
                   context.getModule());
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

void TestPrefix::defineModel(IRGenerationContext& context,
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
                                  0);
  modelDefinition.prototypeObject(context);
  modelDefinition.prototypeMethods(context);
  Model* model = context.getModel(Names::getLangPackageName() + "." + modelName, 0);
  model->createRTTI(context);
}

ControllerDefinition* TestPrefix::defineCallStackController(IRGenerationContext& context) {
  const PrimitiveTypeSpecifier* stringTypeSpecifier;
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  stringTypeSpecifier = PrimitiveTypes::STRING_TYPE->newTypeSpecifier(0);
  VariableDeclaration* declaration;
  declaration = VariableDeclaration::create(stringTypeSpecifier,
                                            new Identifier("objectName", 0),
                                            0);
  arguments.push_back(declaration);
  stringTypeSpecifier = PrimitiveTypes::STRING_TYPE->newTypeSpecifier(0);
  declaration = VariableDeclaration::create(stringTypeSpecifier,
                                            new Identifier("methodName", 0),
                                            0);
  arguments.push_back(declaration);
  stringTypeSpecifier = PrimitiveTypes::STRING_TYPE->newTypeSpecifier(0);
  declaration = VariableDeclaration::create(stringTypeSpecifier, new Identifier("fileName", 0), 0);
  arguments.push_back(declaration);
  const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
  declaration = VariableDeclaration::create(intTypeSpecifier, new Identifier("lineNumber", 0), 0);
  arguments.push_back(declaration);
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier(0);
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* pushStackMethod = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                           voidTypeSpecifier,
                                                           Names::getThreadPushStack(),
                                                           arguments,
                                                           exceptions,
                                                           new MethodQualifiers(0),
                                                           compoundStatement,
                                                           0);
  
  arguments.clear();
  voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier(0);
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* popStackMethod = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                          voidTypeSpecifier,
                                                          Names::getThreadPopStack(),
                                                          arguments,
                                                          exceptions,
                                                          new MethodQualifiers(0),
                                                          compoundStatement,
                                                          0);
  
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* controllerTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getCallStackControllerName(), 0);
  vector<IObjectElementDefinition*> elementDeclarations;
  elementDeclarations.push_back(pushStackMethod);
  elementDeclarations.push_back(popStackMethod);
  
  vector<IInterfaceTypeSpecifier*> interfaceSpecifiers;
  vector<IObjectDefinition*> innerObjectDefinitions;
  
  return new ControllerDefinition(AccessLevel::PUBLIC_ACCESS,
                                  controllerTypeSpecifier,
                                  elementDeclarations,
                                  interfaceSpecifiers,
                                  innerObjectDefinitions,
                                  0);
}

ThreadDefinition* TestPrefix::defineMainThread(IRGenerationContext& context) {
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ThreadTypeSpecifierFull* threadTypeSpecifier =
  new ThreadTypeSpecifierFull(packageExpression, Names::getMainThreadShortName(), 0);
  vector<IObjectElementDefinition*> elementDeclarations;
  
  packageType = new PackageType(Names::getLangPackageName());
  packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* callStackTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getCallStackControllerName(), 0);
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
  methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::CONCEAL);
  MethodDefinition* getCallStackMethod = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                              callStackTypeSpecifier,
                                                              Names::getCallStackMethodName(),
                                                              arguments,
                                                              exceptions,
                                                              methodQualifiers,
                                                              compoundStatement,
                                                              0);
  
  arguments.clear();
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier(0);
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  methodQualifiers = new MethodQualifiers(0);
  methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::CONCEAL);
  MethodDefinition* runMethod = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                     voidTypeSpecifier,
                                                     "run",
                                                     arguments,
                                                     exceptions,
                                                     methodQualifiers,
                                                     compoundStatement,
                                                     0);
  
  elementDeclarations.push_back(getCallStackMethod);
  elementDeclarations.push_back(runMethod);
  
  vector<IObjectDefinition*> innerObjectDefinitions;
  vector<IInterfaceTypeSpecifier*> interfaceSpecifiers;
  packageType = new PackageType(Names::getThreadsPackageName());
  packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  InterfaceTypeSpecifierFull* interfaceTypeSpecifer =
  new InterfaceTypeSpecifierFull(packageExpression, Names::getThreadInterfaceName(), 0);
  interfaceSpecifiers.push_back(interfaceTypeSpecifer);
  
  return new ThreadDefinition(AccessLevel::PUBLIC_ACCESS,
                              threadTypeSpecifier,
                              elementDeclarations,
                              interfaceSpecifiers,
                              innerObjectDefinitions,
                              0);
}

InterfaceDefinition* TestPrefix::defineThreadInterface(IRGenerationContext& context) {
  PackageType* packageType = new PackageType(Names::getThreadsPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  InterfaceTypeSpecifierFull* interfaceTypeSpecifier =
  new InterfaceTypeSpecifierFull(packageExpression, Names::getThreadInterfaceName(), 0);
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

