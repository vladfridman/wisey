//
//  IRGenerationBlock.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef IRGenerationBlock_h
#define IRGenerationBlock_h

namespace yazyk {
  
class IRGenerationBlock {
  llvm::BasicBlock* mBlock;
  std::map<std::string, llvm::Value*> mLocals;

public:

  IRGenerationBlock(llvm::BasicBlock* block) : mBlock(block) { }
  
  /**
   * Return the LLVM basic block associated with this program block
   */
  llvm::BasicBlock* getBlock();
  
  /**
   * Set the LLVM basic block associated with this program block
   */
  void setBlock(llvm::BasicBlock* block);
  
  /**
   * Returns map of local variables associated with this program block
   */
  std::map<std::string, llvm::Value*>& getLocals();
};
  
} // namespace yazyk

#endif /* IRGenerationBlock_h */
