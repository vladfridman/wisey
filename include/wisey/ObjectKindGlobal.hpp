//
//  ObjectKindGlobal.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectKindGlobal_h
#define ObjectKindGlobal_h

#include <llvm/IR/Instructions.h>

namespace wisey {
  
  class IRGenerationContext;
  
  class ObjectKindGlobal {
    
  public:
    
    static llvm::Constant* getController(IRGenerationContext& context);
    
    static llvm::Constant* getModel(IRGenerationContext& context);
    
    static llvm::Constant* getNode(IRGenerationContext& context);
    
  private:
    
    static llvm::Constant* getName(IRGenerationContext& context, std::string name);
    
  };
  
} /* namespace wisey */

#endif /* ObjectKindGlobal_h */
