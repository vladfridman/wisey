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
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/NodeTypeSpecifierFull.hpp"

namespace wisey {
  
  /**
   * Represents node definition implemented in a shared library
   */
  class ExternalNodeDefinition : public IConcreteObjectDefinition {
    NodeTypeSpecifierFull* mNodeTypeSpecifierFull;
    std::vector<IObjectElementDefinition*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    ExternalNodeDefinition(NodeTypeSpecifierFull* nodeTypeSpecifierFull,
                           std::vector<IObjectElementDefinition*> objectElementDeclarations,
                           std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                           std::vector<IObjectDefinition*> innerObjectDefinitions,
                           int line);
    
    ~ExternalNodeDefinition();
    
    Node* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */


#endif /* ExternalNodeDefinition_h */

