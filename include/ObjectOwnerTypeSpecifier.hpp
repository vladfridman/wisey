//
//  ObjectOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectOwnerTypeSpecifier_h
#define ObjectOwnerTypeSpecifier_h

#include "IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents an object owner type specifier that is denoted by '*' in the wisey language
   */
  class ObjectOwnerTypeSpecifier : public ITypeSpecifier {
    
    const IObjectTypeSpecifier* mObjectTypeSpecifier;
    
  public:
    
    ObjectOwnerTypeSpecifier(const IObjectTypeSpecifier* objectTypeSpecifier);
    
    ~ObjectOwnerTypeSpecifier();
    
    const IObjectOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* ObjectOwnerTypeSpecifier_h */

