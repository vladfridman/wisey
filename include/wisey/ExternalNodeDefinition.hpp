//
//  ExternalNodeDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalNodeDefinition_h
#define ExternalNodeDefinition_h

#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/NodeTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents node definition implemented in a shared library
 */
class ExternalNodeDefinition : public IConcreteObjectDefinition {
  NodeTypeSpecifier* mNodeTypeSpecifier;
  std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  ExternalNodeDefinition(NodeTypeSpecifier* nodeTypeSpecifier,
                         std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                         std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);
  
  ~ExternalNodeDefinition();
  
  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
};

} /* namespace wisey */


#endif /* ExternalNodeDefinition_h */
