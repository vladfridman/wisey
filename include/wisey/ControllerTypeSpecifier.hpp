//
//  ControllerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerTypeSpecifier_h
#define ControllerTypeSpecifier_h

#include "wisey/IControllerTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents controller type specifier
   */
  class ControllerTypeSpecifier : public IControllerTypeSpecifier {
    IExpression* mPackageExpression;
    const std::string mShortName;
    
  public:
    
    ControllerTypeSpecifier(IExpression* packageExpression, std::string shortName);
    
    ~ControllerTypeSpecifier();
    
    std::string getShortName() const override;
    
    IExpression* takePackage() override;
    
    std::string getName(IRGenerationContext& context) const override;
    
    const Controller* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ControllerTypeSpecifier_h */

