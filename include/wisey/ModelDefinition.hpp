//
//  ModelDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelDefinition_h
#define ModelDefinition_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/ModelTypeSpecifierFull.hpp"

namespace wisey {
  
  /**
   * Represents model definition which is analogous to an immutable class in C++
   */
  class ModelDefinition : public IConcreteObjectDefinition {
    AccessLevel mAccessLevel;
    ModelTypeSpecifierFull* mModelTypeSpecifierFull;
    std::vector<IObjectElementDefinition*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    ModelDefinition(AccessLevel accessLevel,
                    ModelTypeSpecifierFull* modelTypeSpecifierFull,
                    std::vector<IObjectElementDefinition*> objectElementDeclarations,
                    std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                    std::vector<IObjectDefinition*> innerObjectDefinitions,
                    int line);
    
    ~ModelDefinition();
    
    Model* prototypeObject(IRGenerationContext& context,
                           ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* ModelDefinition_h */

