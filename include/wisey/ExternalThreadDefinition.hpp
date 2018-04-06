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
#include "wisey/IObjectElementDefinition.hpp"

namespace wisey {
  
  /**
   * Represents thread definition implemented in a shared library
   */
  class ExternalThreadDefinition : public IConcreteObjectDefinition {
    ThreadTypeSpecifierFull* mThreadTypeSpecifierFull;
    std::vector<IObjectElementDefinition*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    ExternalThreadDefinition(ThreadTypeSpecifierFull* threadTypeSpecifierFull,
                             std::vector<IObjectElementDefinition*> objectElementDeclarations,
                             std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                             std::vector<IObjectDefinition*> innerObjectDefinitions,
                             int line);
    
    ~ExternalThreadDefinition();
    
    Thread* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ExternalThreadDefinition_h */

