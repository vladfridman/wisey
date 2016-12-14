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
  llvm::BasicBlock* getBlock() {
    return mBlock;
  }
  
  void setBlock(llvm::BasicBlock* block) {
    mBlock = block;
  }
  
  std::map<std::string, llvm::Value*>& getLocals() {
    return mLocals;
  }
};
  
} // namespace yazyk

#endif /* IRGenerationBlock_h */
