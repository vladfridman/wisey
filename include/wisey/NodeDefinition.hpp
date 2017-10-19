//
//  NodeDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NodeDefinition_h
#define NodeDefinition_h

#include "wisey/Field.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/NodeTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a node definition which is an object that can have immutable model fields
 * and mutable fields that point to other nodes.
 */
class NodeDefinition : public IConcreteObjectDefinition {
  NodeTypeSpecifier* mNodeTypeSpecifier;
  std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
  std::vector<InterfaceTypeSpecifier*> mInterfaceSpecifiers;
  
public:
  
  NodeDefinition(NodeTypeSpecifier* nodeTypeSpecifier,
                 std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                 std::vector<InterfaceTypeSpecifier*> interfaceSpecifiers);
  
  ~NodeDefinition();

  void prototypeObjects(IRGenerationContext& context) const override;
  
  void prototypeMethods(IRGenerationContext& context) const override;
  
  llvm::Value* generateIR(IRGenerationContext& context) const override;

};
  
} /* namespace wisey */

#endif /* NodeDefinition_h */
