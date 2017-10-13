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
#include "wisey/IMethodDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void TestPrefix::run(IRGenerationContext& context) {
  defineNPEModel(context);
  defineThreadController(context);
}

void TestPrefix::defineNPEModel(IRGenerationContext& context) {
  vector<FieldDeclaration*> npeFields;
  vector<IMethodDeclaration*> npeMethods;
  vector<InterfaceTypeSpecifier*> npeParentInterfaces;
  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(package,
                                                                  Names::getNPEModelName());
  ModelDefinition npeModelDefinition(modelTypeSpecifier,
                                     npeFields,
                                     npeMethods,
                                     npeParentInterfaces);
  npeModelDefinition.prototypeObjects(context);
  npeModelDefinition.prototypeMethods(context);
}

void TestPrefix::defineThreadController(IRGenerationContext& context) {
  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");
  ControllerTypeSpecifier* controllerTypeSpecifier =
    new ControllerTypeSpecifier(package, Names::getThreadControllerName());
  vector<FieldDeclaration*> fieldDeclarations;
  vector<IMethodDeclaration*> methodDeclarations;
  vector<InterfaceTypeSpecifier*> interfaceSpecifiers;
  
  PrimitiveTypeSpecifier* stringTypeSpecifier;
  VariableList arguments;
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  arguments.push_back(new VariableDeclaration(stringTypeSpecifier, new Identifier("objectName")));
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  arguments.push_back(new VariableDeclaration(stringTypeSpecifier, new Identifier("methodName")));
  vector<ModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block);
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  MethodDeclaration* setObjectAndMethodMethod = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                                                      voidTypeSpecifier,
                                                                      "setObjectAndMethod",
                                                                      arguments,
                                                                      exceptions,
                                                                      compoundStatement);

  arguments.clear();
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  arguments.push_back(new VariableDeclaration(stringTypeSpecifier, new Identifier("fileName")));
  PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  arguments.push_back(new VariableDeclaration(intTypeSpecifier, new Identifier("lineNumber")));
  voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  block = new Block();
  compoundStatement = new CompoundStatement(block);
  MethodDeclaration* pushStackMethodMethod = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                                                      voidTypeSpecifier,
                                                                      "pushStack",
                                                                      arguments,
                                                                      exceptions,
                                                                      compoundStatement);

  methodDeclarations.push_back(setObjectAndMethodMethod);
  methodDeclarations.push_back(pushStackMethodMethod);

  ControllerDefinition threadControllerDefinition(controllerTypeSpecifier,
                                                  fieldDeclarations,
                                                  methodDeclarations,
                                                  interfaceSpecifiers);
  threadControllerDefinition.prototypeObjects(context);
  threadControllerDefinition.prototypeMethods(context);
}
