//
//  ExternalModelDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalModelDefinition_h
#define ExternalModelDefinition_h

#include "wisey/IConcreteObjectDefinition.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/ModelTypeSpecifierFull.hpp"

namespace wisey {
  
  /**
   * Represents model definition implemented in a shared library
   */
  class ExternalModelDefinition : public IConcreteObjectDefinition {
    ModelTypeSpecifierFull* mModelTypeSpecifierFull;
    std::vector<IObjectElementDeclaration*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    
  public:
    
    ExternalModelDefinition(ModelTypeSpecifierFull* modelTypeSpecifierFull,
                            std::vector<IObjectElementDeclaration*> objectElementDeclarations,
                            std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                            std::vector<IObjectDefinition*> innerObjectDefinitions);
    
    ~ExternalModelDefinition();
    
    Model* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  private:
    
  };
  
} /* namespace wisey */

#endif /* ExternalModelDefinition_h */

