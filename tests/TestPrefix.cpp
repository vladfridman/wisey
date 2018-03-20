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
  vector<IObjectElementDefinition*> modelElements;
  defineModel(context, Names::getNPEModelName(), modelElements);
  const PrimitiveTypeSpecifier* longTypeSpecifier = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
  modelElements.push_back(new FixedFieldDefinition(longTypeSpecifier, "mReferenceCount"));
  defineModel(context, Names::getReferenceCountExceptionName(), modelElements);
  modelElements.clear();
  
  longTypeSpecifier = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
  modelElements.push_back(new FixedFieldDefinition(longTypeSpecifier, "mArraySize"));
  longTypeSpecifier = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
  modelElements.push_back(new FixedFieldDefinition(longTypeSpecifier, "mIndex"));
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
}

void TestPrefix::defineModel(IRGenerationContext& context,
                             string modelName,
                             vector<IObjectElementDefinition*> modelElements) {
  vector<IInterfaceTypeSpecifier*> modelParentInterfaces;
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ModelTypeSpecifierFull* modelTypeSpecifier =
    new ModelTypeSpecifierFull(packageExpression, modelName);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  modelTypeSpecifier,
                                  modelElements,
                                  modelParentInterfaces,
                                  innerObjectDefinitions);
  modelDefinition.prototypeObject(context);
  modelDefinition.prototypeMethods(context);
  Model* model = context.getModel(Names::getLangPackageName() + "." + modelName);
  model->createRTTI(context);
}

ControllerDefinition* TestPrefix::defineCallStackController(IRGenerationContext& context) {
  const PrimitiveTypeSpecifier* stringTypeSpecifier;
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  stringTypeSpecifier = PrimitiveTypes::STRING_TYPE->newTypeSpecifier();
  VariableDeclaration* declaration;
  declaration = VariableDeclaration::create(stringTypeSpecifier, new Identifier("objectName"), 0);
  arguments.push_back(declaration);
  stringTypeSpecifier = PrimitiveTypes::STRING_TYPE->newTypeSpecifier();
  declaration = VariableDeclaration::create(stringTypeSpecifier, new Identifier("methodName"), 0);
  arguments.push_back(declaration);
  stringTypeSpecifier = PrimitiveTypes::STRING_TYPE->newTypeSpecifier();
  declaration = VariableDeclaration::create(stringTypeSpecifier, new Identifier("fileName"), 0);
  arguments.push_back(declaration);
  const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  declaration = VariableDeclaration::create(intTypeSpecifier, new Identifier("lineNumber"), 0);
  arguments.push_back(declaration);
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* pushStackMethod = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                             voidTypeSpecifier,
                                                             Names::getThreadPushStack(),
                                                             arguments,
                                                             exceptions,
                                                             compoundStatement,
                                                             0);

  arguments.clear();
  voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* popStackMethod = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                            voidTypeSpecifier,
                                                            Names::getThreadPopStack(),
                                                            arguments,
                                                            exceptions,
                                                            compoundStatement,
                                                            0);

  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* controllerTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getCallStackControllerName());
  vector<IObjectElementDefinition*> elementDeclarations;
  elementDeclarations.push_back(pushStackMethod);
  elementDeclarations.push_back(popStackMethod);

  vector<IInterfaceTypeSpecifier*> interfaceSpecifiers;
  vector<IObjectDefinition*> innerObjectDefinitions;
  
  return new ControllerDefinition(AccessLevel::PUBLIC_ACCESS,
                                  controllerTypeSpecifier,
                                  elementDeclarations,
                                  interfaceSpecifiers,
                                  innerObjectDefinitions);
}

ThreadDefinition* TestPrefix::defineMainThread(IRGenerationContext& context) {
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ThreadTypeSpecifierFull* threadTypeSpecifier =
    new ThreadTypeSpecifierFull(packageExpression, Names::getMainThreadShortName());
  vector<IObjectElementDefinition*> elementDeclarations;
  
  packageType = new PackageType(Names::getLangPackageName());
  packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* callStackTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getCallStackControllerName());
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* getCallStackMethod = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                                callStackTypeSpecifier,
                                                                Names::getCallStackMethodName(),
                                                                arguments,
                                                                exceptions,
                                                                compoundStatement,
                                                                0);

  arguments.clear();
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* runMethod = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                                       voidTypeSpecifier,
                                                       "run",
                                                       arguments,
                                                       exceptions,
                                                       compoundStatement,
                                                       0);

  elementDeclarations.push_back(getCallStackMethod);
  elementDeclarations.push_back(runMethod);

  voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  
  vector<IObjectDefinition*> innerObjectDefinitions;
  vector<IInterfaceTypeSpecifier*> interfaceSpecifiers;
  packageType = new PackageType(Names::getLangPackageName());
  packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  InterfaceTypeSpecifierFull* interfaceTypeSpecifer =
  new InterfaceTypeSpecifierFull(packageExpression, Names::getThreadInterfaceName());
  interfaceSpecifiers.push_back(interfaceTypeSpecifer);
  
  return new ThreadDefinition(AccessLevel::PUBLIC_ACCESS,
                              threadTypeSpecifier,
                              voidTypeSpecifier,
                              elementDeclarations,
                              interfaceSpecifiers,
                              innerObjectDefinitions);
}

InterfaceDefinition* TestPrefix::defineThreadInterface(IRGenerationContext& context) {
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpressionWithCleanup* packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  InterfaceTypeSpecifierFull* interfaceTypeSpecifier =
    new InterfaceTypeSpecifierFull(packageExpression, Names::getThreadInterfaceName());
  vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers;
  vector<IObjectElementDefinition *> elementDeclarations;
  vector<IObjectDefinition*> innerObjectDefinitions;

  return new InterfaceDefinition(AccessLevel::PUBLIC_ACCESS,
                                 interfaceTypeSpecifier,
                                 parentInterfaceSpecifiers,
                                 elementDeclarations,
                                 innerObjectDefinitions);
}
