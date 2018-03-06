//
//  ThreadTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadTypeSpecifier_h
#define ThreadTypeSpecifier_h

#include <string>

#include "wisey/IThreadTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents thread type specifier
   */
  class ThreadTypeSpecifier : public IThreadTypeSpecifier {
    IExpression* mPackageExpression;
    const std::string mShortName;
    
  public:
    
    ThreadTypeSpecifier(IExpression* packageExpression, std::string shortName);
    
    ~ThreadTypeSpecifier();
    
    std::string getShortName() const override;
    
    IExpression* takePackage() override;
    
    std::string getName(IRGenerationContext& context) const override;
    
    Thread* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ThreadTypeSpecifier_h */
