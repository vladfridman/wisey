//
//  TestPrefix.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "TestPrefix.hpp"
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
