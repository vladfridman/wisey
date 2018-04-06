//
//  ThreadTypeSpecifierFull.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadTypeSpecifierFull_h
#define ThreadTypeSpecifierFull_h

#include "wisey/IThreadTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a fully qualified thread object type specifier
   */
  class ThreadTypeSpecifierFull : public IThreadTypeSpecifier {
    IExpression* mPackageExpression;
    const std::string mShortName;
    int mLine;
    
  public:
    
    ThreadTypeSpecifierFull(IExpression* packageExpression, std::string shortName, int line);
    
    ~ThreadTypeSpecifierFull();
    
    std::string getShortName() const override;
    
    IExpression* takePackage() override;
    
    std::string getName(IRGenerationContext& context) const override;
    
    Thread* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ThreadTypeSpecifierFull_h */
