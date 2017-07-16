//
//  ProgramPrefix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/InterfaceDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/ImportStatement.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* ProgramPrefix::generateIR(IRGenerationContext& context) const {
  context.setPackage("wisey.lang");

  vector<FieldDeclaration*> npeFields;
  vector<MethodDeclaration*> npeMethods;
  vector<InterfaceTypeSpecifier*> npeParentInterfaces;
  ModelDefinition npeModelDefinition("MNullPointerException",
                                     npeFields,
                                     npeMethods,
                                     npeParentInterfaces);
  npeModelDefinition.prototypeObjects(context);
  npeModelDefinition.prototypeMethods(context);
  npeModelDefinition.generateIR(context);
  
  context.addImport(context.getModel("MNullPointerException"));
  
  PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  VariableList variableList;
  vector<ModelTypeSpecifier*> thrownExceptions;
  vector<string> packageSpecifier;
  ModelTypeSpecifier* npeExceptionTypeSpecifier =
    new ModelTypeSpecifier(packageSpecifier, "MNullPointerException");
  thrownExceptions.push_back(npeExceptionTypeSpecifier);
  
  MethodSignatureDeclaration* runMethod = new MethodSignatureDeclaration(intTypeSpecifier,
                                                                         "run",
                                                                         variableList,
                                                                         thrownExceptions);
  
  vector<InterfaceTypeSpecifier*> parentInterfaces;
  vector<MethodSignatureDeclaration *> methodSignatureDeclarations;
  methodSignatureDeclarations.push_back(runMethod);
  InterfaceDefinition programInterface("IProgram", parentInterfaces, methodSignatureDeclarations);
  
  programInterface.prototypeObjects(context);
  programInterface.prototypeMethods(context);
  programInterface.generateIR(context);
  
  context.addImport(context.getInterface("IProgram"));
  
  return NULL;
}
