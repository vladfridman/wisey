//
//  ExternalThreadDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalThreadDefinition_h
#define ExternalThreadDefinition_h

#include <string>
#include <vector>

#include "wisey/ThreadTypeSpecifierFull.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDeclaration.hpp"

namespace wisey {
  
  /**
   * Represents thread definition implemented in a shared library
   */
  class ExternalThreadDefinition : public IConcreteObjectDefinition {
    ThreadTypeSpecifierFull* mThreadTypeSpecifierFull;
    std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    
  public:
    
    ExternalThreadDefinition(ThreadTypeSpecifierFull* threadTypeSpecifierFull,
                             std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                             std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                             std::vector<IObjectDefinition*> innerObjectDefinitions);
    
    ~ExternalThreadDefinition();
    
    Thread* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ExternalThreadDefinition_h */

