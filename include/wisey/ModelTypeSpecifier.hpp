//
//  ModelTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelTypeSpecifier_h
#define ModelTypeSpecifier_h

#include <string>

#include "wisey/IModelTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents model type specifier
   */
  class ModelTypeSpecifier : public IModelTypeSpecifier {
    IExpression* mPackageExpression;
    const std::string mShortName;
    int mLine;
    
  public:
    
    ModelTypeSpecifier(IExpression* packageExpression, std::string shortName, int line);
    
    ~ModelTypeSpecifier();
    
    std::string getShortName() const override;
    
    IExpression* takePackage() override;
    
    std::string getName(IRGenerationContext& context) const override;
    
    Model* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* ModelTypeSpecifier_h */

