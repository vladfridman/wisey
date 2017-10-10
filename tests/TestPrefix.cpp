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
#include "wisey/ModelDefinition.hpp"
#include "wisey/Names.hpp"

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
  ControllerDefinition threadControllerDefinition(controllerTypeSpecifier,
                                                  fieldDeclarations,
                                                  methodDeclarations,
                                                  interfaceSpecifiers);
  threadControllerDefinition.prototypeObjects(context);
  threadControllerDefinition.prototypeMethods(context);
}
