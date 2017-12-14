//
//  TestPrefix.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "TestPrefix.hpp"
#include "wisey/ControllerDefinition.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void TestPrefix::generateIR(IRGenerationContext& context) {
  vector<IObjectElementDeclaration*> modelElements;
  defineModel(context, Names::getNPEModelName(), modelElements);
  InjectionArgumentList arguments;
  PrimitiveTypeSpecifier* longTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  modelElements.push_back(new FieldDeclaration(FIXED_FIELD,
                                               longTypeSpecifier,
                                               "mReferenceCount",
                                               arguments));
  defineModel(context, Names::getReferenceCountExceptionName(), modelElements);
  defineThreadController(context);
}

void TestPrefix::defineModel(IRGenerationContext& context,
                             string modelName,
                             vector<IObjectElementDeclaration*> modelElements) {
  vector<IInterfaceTypeSpecifier*> modelParentInterfaces;
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
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

void TestPrefix::defineThreadController(IRGenerationContext& context) {
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifierFull* controllerTypeSpecifier =
    new ControllerTypeSpecifierFull(packageExpression, Names::getThreadControllerName());
  vector<IObjectElementDeclaration*> elementDeclarations;
  vector<IInterfaceTypeSpecifier*> interfaceSpecifiers;
  
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
                                                             "pushStack",
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
                                                            "popStack",
                                                            arguments,
                                                            exceptions,
                                                            compoundStatement,
                                                            0);

  elementDeclarations.push_back(pushStackMethod);
  elementDeclarations.push_back(popStackMethod);

  vector<IObjectDefinition*> innerObjectDefinitions;
  ControllerDefinition threadControllerDefinition(AccessLevel::PUBLIC_ACCESS,
                                                  controllerTypeSpecifier,
                                                  elementDeclarations,
                                                  interfaceSpecifiers,
                                                  innerObjectDefinitions);
  threadControllerDefinition.prototypeObject(context);
  threadControllerDefinition.prototypeMethods(context);
}
