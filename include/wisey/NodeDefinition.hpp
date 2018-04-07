//
//  NodeDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NodeDefinition_h
#define NodeDefinition_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/NodeTypeSpecifierFull.hpp"

namespace wisey {
  
  /**
   * Represents a node definition which is an object that can have immutable model fields
   * and mutable fields that point to other nodes.
   */
  class NodeDefinition : public IConcreteObjectDefinition {
    AccessLevel mAccessLevel;
    NodeTypeSpecifierFull* mNodeTypeSpecifierFull;
    std::vector<IObjectElementDefinition*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    NodeDefinition(AccessLevel accessLevel,
                   NodeTypeSpecifierFull* nodeTypeSpecifierFull,
                   std::vector<IObjectElementDefinition*> objectElementDeclarations,
                   std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                   std::vector<IObjectDefinition*> innerObjectDefinitions,
                   int line);
    
    ~NodeDefinition();
    
    Node* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* NodeDefinition_h */

