//
//  ExternalModelDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalModelDefinition_h
#define ExternalModelDefinition_h


#include "wisey/FieldDeclaration.hpp"
#include "wisey/IField.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents model definition implemented in a shared library
 */
class ExternalModelDefinition : public IGlobalStatement {
  ModelTypeSpecifier* mModelTypeSpecifier;
  std::vector<FieldDeclaration*> mFieldDeclarations;
  std::vector<MethodSignatureDeclaration*> mMethodSignatures;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ExternalModelDefinition(ModelTypeSpecifier* modelTypeSpecifier,
                          std::vector<FieldDeclaration*> fieldDeclarations,
                          std::vector<MethodSignatureDeclaration *> methodSignatures,
                          std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
  mModelTypeSpecifier(modelTypeSpecifier),
  mFieldDeclarations(fieldDeclarations),
  mMethodSignatures(methodSignatures),
  mInterfaceSpecifiers(interfaceSpecifiers) { }
  
  ~ExternalModelDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */

#endif /* ExternalModelDefinition_h */
