//
//  ProgramPrefix.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/InterfaceDefinition.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"
#include "yazyk/ProgramPrefix.hpp"
#include "yazyk/MethodSignatureDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ProgramPrefix::generateIR(IRGenerationContext& context) const {
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT_TYPE);
  VariableList variableList;
  vector<ITypeSpecifier*> thrownExceptions;
  
  MethodSignatureDeclaration runMethod(intTypeSpecifier, "run", variableList, thrownExceptions);
  
  vector<string> parentInterfaces;
  vector<MethodSignatureDeclaration *> methodSignatureDeclarations;
  methodSignatureDeclarations.push_back(&runMethod);
  InterfaceDefinition programInterface("IProgram", parentInterfaces, methodSignatureDeclarations);
  
  return programInterface.generateIR(context);
}
