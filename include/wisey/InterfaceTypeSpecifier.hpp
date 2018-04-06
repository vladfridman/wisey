
//
//  InterfaceTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InterfaceTypeSpecifier_h
#define InterfaceTypeSpecifier_h

#include "wisey/IInjectable.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/Interface.hpp"

namespace wisey {
  
  /**
   * Represents interface type specifier
   */
  class InterfaceTypeSpecifier : public IInterfaceTypeSpecifier {
    IExpression* mPackageExpression;
    const std::string mShortName;
    int mLine;
    
  public:
    
    InterfaceTypeSpecifier(IExpression* packageExpression, std::string shortName, int line);
    
    ~InterfaceTypeSpecifier();
    
    std::string getShortName() const override;
    
    IExpression* takePackage() override;
    
    std::string getName(IRGenerationContext& context) const override;
    
    Interface* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* InterfaceTypeSpecifier_h */

