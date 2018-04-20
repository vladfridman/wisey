//
//  WiseyObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef WiseyObjectTypeSpecifier_h
#define WiseyObjectTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/WiseyObjectType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a llvm pointer that points to a wisey object type specifier
   */
  class WiseyObjectTypeSpecifier : public ITypeSpecifier {
    
    int mLine;
    
  public:
    
    WiseyObjectTypeSpecifier(int line);
    
    ~WiseyObjectTypeSpecifier();
    
    const WiseyObjectType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* WiseyObjectTypeSpecifier_h */
