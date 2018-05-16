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
    
    /**
     * Returns a string constant that contains string "controller"
     */
    static llvm::Constant* getController(IRGenerationContext& context);
    
    /**
     * Returns a string constant that contains string "model"
     */
    static llvm::Constant* getModel(IRGenerationContext& context);
    
    /**
     * Returns a string constant that contains string "node"
     */
    static llvm::Constant* getNode(IRGenerationContext& context);
    
    /**
     * Returns a string constant that contains string "thread"
     */
   static llvm::Constant* getThread(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* ObjectKindGlobal_h */
