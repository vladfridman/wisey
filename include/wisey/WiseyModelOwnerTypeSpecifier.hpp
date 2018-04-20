//
//  WiseyModelOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef WiseyModelOwnerTypeSpecifier_h
#define WiseyModelOwnerTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/WiseyModelOwnerType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents an llvm pointer that points to a wisey model that it owns
   */
  class WiseyModelOwnerTypeSpecifier : public ITypeSpecifier {
    
    int mLine;
    
  public:
    
    WiseyModelOwnerTypeSpecifier(int line);
    
    ~WiseyModelOwnerTypeSpecifier();
    
    const WiseyModelOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* WiseyModelOwnerTypeSpecifier_h */
