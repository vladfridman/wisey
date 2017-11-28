//
//  TestPrefix.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "TestPrefix.hpp"
#include "wisey/ControllerDefinition.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
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
  defineModel(context, Names::getNPEModelName());
  defineModel(context, Names::getDestroyedObjectStillInUseName());
  defineThreadController(context);
}

void TestPrefix::defineModel(IRGenerationContext& context, string modelName) {
  vector<IObjectElementDeclaration*> modelElements;
  vector<InterfaceTypeSpecifier*> modelParentInterfaces;
  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(package, modelName);
  ModelDefinition modelDefinition(modelTypeSpecifier, modelElements, modelParentInterfaces);
  modelDefinition.prototypeObjects(context);
  modelDefinition.prototypeMethods(context);
}

void TestPrefix::defineThreadController(IRGenerationContext& context) {
  vector<string> package;
  package.push_back("wisey");
  package.push_back("lang");
  ControllerTypeSpecifier* controllerTypeSpecifier =
    new ControllerTypeSpecifier(package, Names::getThreadControllerName());
  vector<IObjectElementDeclaration*> elementDeclarations;
  vector<InterfaceTypeSpecifier*> interfaceSpecifiers;
  
  PrimitiveTypeSpecifier* stringTypeSpecifier;
  VariableList arguments;
  vector<ModelTypeSpecifier*> exceptions;
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  arguments.push_back(new VariableDeclaration(stringTypeSpecifier, new Identifier("objectName")));
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  arguments.push_back(new VariableDeclaration(stringTypeSpecifier, new Identifier("methodName")));
  stringTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  arguments.push_back(new VariableDeclaration(stringTypeSpecifier, new Identifier("fileName")));
  PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  arguments.push_back(new VariableDeclaration(intTypeSpecifier, new Identifier("lineNumber")));
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block);
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
  compoundStatement = new CompoundStatement(block);
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
