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
#include "wisey/FixedFieldDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDeclaration.hpp"
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
  vector<IObjectElementDeclaration*> modelElements;
  defineModel(context, Names::getNPEModelName(), modelElements);
  PrimitiveTypeSpecifier* longTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  modelElements.push_back(new FixedFieldDeclaration(longTypeSpecifier, "mReferenceCount"));
  defineModel(context, Names::getReferenceCountExceptionName(), modelElements);
  modelElements.clear();
  
  longTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  modelElements.push_back(new FixedFieldDeclaration(longTypeSpecifier, "mArraySize"));
  longTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  modelElements.push_back(new FixedFieldDeclaration(longTypeSpecifier, "mIndex"));
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
                             vector<IObjectElementDeclaration*> modelElements) {
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
  PrimitiveTypeSpecifier* stringTypeSpecifier;
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  VariableDeclaration* declaration;
  declaration = VariableDeclaration::create(stringTypeSpecifier, new Identifier("objectName"), 0);
  arguments.push_back(declaration);
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  declaration = VariableDeclaration::create(stringTypeSpecifier, new Identifier("methodName"), 0);
  arguments.push_back(declaration);
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  declaration = VariableDeclaration::create(stringTypeSpecifier, new Identifier("fileName"), 0);
  arguments.push_back(declaration);
  PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  declaration = VariableDeclaration::create(intTypeSpecifier, new Identifier("lineNumber"), 0);
  arguments.push_back(declaration);
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDeclaration* pushStackMethod = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                                             voidTypeSpecifier,
                                                             Names::getThreadPushStack(),
                                                             arguments,
                                                             exceptions,
                                                             compoundStatement,
                                                             0);

  arguments.clear();
  voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  MethodDeclaration* popStackMethod = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
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
  vector<IObjectElementDeclaration*> elementDeclarations;
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
  vector<IObjectElementDeclaration*> elementDeclarations;
  
  packageType = new PackageType(Names::getLangPackageName());
  packageExpression = new FakeExpressionWithCleanup(NULL, packageType);
  ControllerTypeSpecifierFull* callStackTypeSpecifier =
  new ControllerTypeSpecifierFull(packageExpression, Names::getCallStackControllerName());
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDeclaration* getCallStackMethod = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                                                callStackTypeSpecifier,
                                                                Names::getCallStackMethodName(),
                                                                arguments,
                                                                exceptions,
                                                                compoundStatement,
                                                                0);

  arguments.clear();
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  block = new Block();
  compoundStatement = new CompoundStatement(block, 0);
  MethodDeclaration* runMethod = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                                       voidTypeSpecifier,
                                                       "run",
                                                       arguments,
                                                       exceptions,
                                                       compoundStatement,
                                                       0);

  elementDeclarations.push_back(getCallStackMethod);
  elementDeclarations.push_back(runMethod);

  voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  
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
  vector<IObjectElementDeclaration *> elementDeclarations;
  vector<IObjectDefinition*> innerObjectDefinitions;

  return new InterfaceDefinition(AccessLevel::PUBLIC_ACCESS,
                                 interfaceTypeSpecifier,
                                 parentInterfaceSpecifiers,
                                 elementDeclarations,
                                 innerObjectDefinitions);
}
