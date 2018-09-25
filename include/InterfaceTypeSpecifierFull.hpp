//
//  InterfaceTypeSpecifierFull.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceTypeSpecifierFull_h
#define InterfaceTypeSpecifierFull_h

#include "IInterfaceTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a fully qualified interface type specifier
   */
  class InterfaceTypeSpecifierFull : public IInterfaceTypeSpecifier {
    IExpression* mPackageExpression;
    const std::string mShortName;
    int mLine;
    
  public:
    
    InterfaceTypeSpecifierFull(IExpression* packageExpression,
                               std::string shortName,
                               int line);
    
    ~InterfaceTypeSpecifierFull();
    
    std::string getShortName() const override;
    
    IExpression* takePackage() override;
    
    std::string getName(IRGenerationContext& context) const override;
    
    Interface* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* InterfaceTypeSpecifierFull_h */

