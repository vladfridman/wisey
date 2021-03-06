//
//  ModelTypeSpecifierFull.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ModelTypeSpecifierFull_h
#define ModelTypeSpecifierFull_h

#include "IModelTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a fully qualified model type specifier
   */
  class ModelTypeSpecifierFull : public IModelTypeSpecifier {
    IExpression* mPackageExpression;
    const std::string mShortName;
    int mLine;
    
  public:
    
    ModelTypeSpecifierFull(IExpression* packageExpression, std::string shortName, int line);
    
    ~ModelTypeSpecifierFull();
    
    std::string getShortName() const override;
    
    IExpression* takePackage() override;
    
    std::string getName(IRGenerationContext& context) const override;
    
    Model* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* ModelTypeSpecifierFull_h */

