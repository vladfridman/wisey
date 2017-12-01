//
//  TestPrefix.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "TestPrefix.hpp"
#include "wisey/ControllerDefinition.hpp"
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
  ExpressionList expressionList;
  PrimitiveTypeSpecifier* longTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  modelElements.push_back(new FieldDeclaration(FIXED_FIELD,
                                               longTypeSpecifier,
                                               "mReferenceCount",
                                               expressionList));
  defineModel(context, Names::getReferenceCountExceptionName(), modelElements);
  defineThreadController(context);
}

void TestPrefix::defineModel(IRGenerationContext& context,
                             string modelName,
                             vector<IObjectElementDeclaration*> modelElements) {
  vector<IInterfaceTypeSpecifier*> modelParentInterfaces;
  ModelTypeSpecifierFull* modelTypeSpecifier =
    new ModelTypeSpecifierFull(Names::getLangPackageName(), modelName);
  ModelDefinition modelDefinition(modelTypeSpecifier, modelElements, modelParentInterfaces);
  modelDefinition.prototypeObjects(context);
  modelDefinition.prototypeMethods(context);
  Model* model = context.getModel(Names::getLangPackageName() + "." + modelName);
  model->createRTTI(context);
}

void TestPrefix::defineThreadController(IRGenerationContext& context) {
  ControllerTypeSpecifierFull* controllerTypeSpecifier =
    new ControllerTypeSpecifierFull(Names::getLangPackageName(), Names::getThreadControllerName());
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

  ControllerDefinition threadControllerDefinition(controllerTypeSpecifier,
                                                  elementDeclarations,
                                                  interfaceSpecifiers);
  threadControllerDefinition.prototypeObjects(context);
  threadControllerDefinition.prototypeMethods(context);
}
