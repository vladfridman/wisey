//
//  WiseyObjectOwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef WiseyObjectOwnerTypeSpecifier_h
#define WiseyObjectOwnerTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents an llvm pointer that points to a wisey object that it owns
   */
  class WiseyObjectOwnerTypeSpecifier : public ITypeSpecifier {
    
    int mLine;
    
  public:
    
    WiseyObjectOwnerTypeSpecifier(int line);
    
    ~WiseyObjectOwnerTypeSpecifier();
    
    const WiseyObjectOwnerType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* WiseyObjectOwnerTypeSpecifier_h */
