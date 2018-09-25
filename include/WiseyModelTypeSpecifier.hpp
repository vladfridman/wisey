//
//  WiseyModelTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef WiseyModelTypeSpecifier_h
#define WiseyModelTypeSpecifier_h

#include "ITypeSpecifier.hpp"
#include "WiseyModelType.hpp"

namespace wisey {
  
  class IRGenerationContext;
  
  /**
   * Represents a llvm pointer that points to a wisey model
   */
  class WiseyModelTypeSpecifier : public ITypeSpecifier {
    
    int mLine;
    
  public:
    
    WiseyModelTypeSpecifier(int line);
    
    ~WiseyModelTypeSpecifier();
    
    const WiseyModelType* getType(IRGenerationContext& context) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;

  };
  
} /* namespace wisey */

#endif /* WiseyModelTypeSpecifier_h */
