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
#include "wisey/InterfaceTypeSpecifierFull.hpp"
#include "wisey/IObjectDefinition.hpp"
#include "wisey/IObjectElementDeclaration.hpp"

namespace wisey {
  
/**
 * Represents an interface definition which is analogous to a pure virtual class in C++
 */
class InterfaceDefinition : public IObjectDefinition {
  InterfaceTypeSpecifierFull* mInterfaceTypeSpecifierFull;
  std::vector<IInterfaceTypeSpecifier*> mParentInterfaceSpecifiers;
  std::vector<IObjectElementDeclaration *> mElementDeclarations;
  std::vector<IObjectDefinition*> mInnerObjectDefinitions;

public:
  
  InterfaceDefinition(InterfaceTypeSpecifierFull* interfaceTypeSpecifierFull,
                      std::vector<IInterfaceTypeSpecifier*> parentInterfaceSpecifiers,
                      std::vector<IObjectElementDeclaration *> elementDeclarations,
                      std::vector<IObjectDefinition*> innerObjectDefinitions);
  
  ~InterfaceDefinition();
  
  const Interface* prototypeObject(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};
  
} /* namespace wisey */

#endif /* InterfaceDefinition_h */
