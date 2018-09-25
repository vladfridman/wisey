//
//  ExternalModelDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalModelDefinition_h
#define ExternalModelDefinition_h

#include "IConcreteObjectDefinition.hpp"
#include "IInterfaceTypeSpecifier.hpp"
#include "IObjectElementDefinition.hpp"
#include "ModelTypeSpecifierFull.hpp"

namespace wisey {
  
  /**
   * Represents model definition implemented in a shared library
   */
  class ExternalModelDefinition : public IConcreteObjectDefinition {
    ModelTypeSpecifierFull* mModelTypeSpecifierFull;
    std::vector<IObjectElementDefinition*> mObjectElementDeclarations;
    std::vector<IInterfaceTypeSpecifier*> mInterfaceSpecifiers;
    std::vector<IObjectDefinition*> mInnerObjectDefinitions;
    int mLine;
    
  public:
    
    ExternalModelDefinition(ModelTypeSpecifierFull* modelTypeSpecifierFull,
                            std::vector<IObjectElementDefinition*> objectElementDeclarations,
                            std::vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                            std::vector<IObjectDefinition*> innerObjectDefinitions,
                            int line);
    
    ~ExternalModelDefinition();
    
    Model* prototypeObject(IRGenerationContext& context,
                           ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  private:
    
  };
  
} /* namespace wisey */

#endif /* ExternalModelDefinition_h */

