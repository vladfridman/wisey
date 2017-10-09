//
//  InterfaceDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceDefinition_h
#define InterfaceDefinition_h

#include "wisey/Block.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IObjectDefinition.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"

namespace wisey {
  
/**
 * Represents an interface definition which is analogous to a pure virtual class in C++
 */
class InterfaceDefinition : public IObjectDefinition {
  InterfaceTypeSpecifier* mInterfaceTypeSpecifier;
  std::vector<InterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
  std::vector<MethodSignatureDeclaration *> mMethodSignatureDeclarations;
  
public:
  
  InterfaceDefinition(InterfaceTypeSpecifier* interfaceTypeSpecifier,
                      std::vector<InterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                      std::vector<MethodSignatureDeclaration *> methodSignatureDeclarations) :
  mInterfaceTypeSpecifier(interfaceTypeSpecifier),
  mParentInterfaceSpecifiers(parentInterfaceSpecifiers),
  mMethodSignatureDeclarations(methodSignatureDeclarations) { }
  
  ~InterfaceDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* InterfaceDefinition_h */
