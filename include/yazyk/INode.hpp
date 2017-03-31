//
//  INode.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef INode_h
#define INode_h

#include <llvm/IR/Value.h>

namespace yazyk {

class IRGenerationContext;

/**
 * Interface representing a node in AST of yazyk parser.
 */
class INode {
  
public:
  
  virtual ~INode() { }

  /**
   * Generate LLVM Intermediate Reprentation code
   */
  virtual llvm::Value* generateIR(IRGenerationContext& context) const = 0;
};
  
}

#endif /* INode_h */
