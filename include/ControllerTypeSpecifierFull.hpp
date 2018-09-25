//
//  ControllerTypeSpecifierFull.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ControllerTypeSpecifierFull_h
#define ControllerTypeSpecifierFull_h

#include "IControllerTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a fully qualified controller type specifier
   */
  class ControllerTypeSpecifierFull : public IControllerTypeSpecifier {
    IExpression* mPackageExpression;
    const std::string mShortName;
    int mLine;
    
  public:
    
    ControllerTypeSpecifierFull(IExpression* packageExpression,
                                std::string shortName,
                                int line);
    
    ~ControllerTypeSpecifierFull();
    
    std::string getShortName() const override;
    
    IExpression* takePackage() override;
    
    std::string getName(IRGenerationContext& context) const override;
    
    const Controller* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* ControllerTypeSpecifierFull_h */

