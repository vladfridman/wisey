//
//  ProgramPrefix.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/InterfaceDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Value* ProgramPrefix::generateIR(IRGenerationContext& context) const {
  PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  VariableList variableList;
  vector<ITypeSpecifier*> thrownExceptions;
  
  MethodSignatureDeclaration runMethod(intTypeSpecifier, "run", variableList, thrownExceptions);
  
  vector<InterfaceTypeSpecifier*> parentInterfaces;
  vector<MethodSignatureDeclaration *> methodSignatureDeclarations;
  methodSignatureDeclarations.push_back(&runMethod);
  InterfaceDefinition programInterface("IProgram", parentInterfaces, methodSignatureDeclarations);
  
  return programInterface.generateIR(context);
}
