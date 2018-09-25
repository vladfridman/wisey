//
//  GlobalStringConstant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef GlobalStringConstant_h
#define GlobalStringConstant_h

#include <llvm/IR/Constants.h>

namespace wisey {
  
  class IRGenerationContext;
  
  class GlobalStringConstant {
    
  public:
    
    /**
     * Returns a string global constant defined internally with value containing the given string
     */
    static llvm::Constant* get(IRGenerationContext& context, std::string value);
    
  };
  
} /* namespace wisey */

#endif /* GlobalStringConstant_h */
