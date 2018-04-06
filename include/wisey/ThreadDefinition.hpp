//
//  ThreadDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadDefinition_h
#define ThreadDefinition_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/ThreadTypeSpecifierFull.hpp"

namespace wisey {
  
  /**
   * Represents model definition which is analogous to an immutable class in C++
   */
  class ThreadDefinition : public IConcreteObjectDefinition {
    AccessLevel mAccessLevel;
    ThreadTypeSpecifierFull* mThreadTypeSpecifierFull;
    std::vector<IObjectElementDefinition*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    ThreadDefinition(AccessLevel accessLevel,
                     ThreadTypeSpecifierFull* threadTypeSpecifierFull,
                     std::vector<IObjectElementDefinition*> objectElementDeclarations,
                     std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                     std::vector<IObjectDefinition*> innerObjectDefinitions,
                     int line);
    
    ~ThreadDefinition();
    
    Thread* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ThreadDefinition_h */

