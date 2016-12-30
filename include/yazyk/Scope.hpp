//
//  Scope.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef Scope_h
#define Scope_h

namespace yazyk {

/**
 * Represents block scope inside yazyk program.
 *
 * Each scope has local variables associated with it
 */
class Scope {
  std::map<std::string, llvm::Value*> mLocals;
  llvm::BasicBlock* mBreakToBlock;

public:

  Scope() : mBreakToBlock(NULL) { }
  
  /**ß
   * Returns map of local variables associated with this program block
   */
  std::map<std::string, llvm::Value*>& getLocals();
  
  /**
   * Set block to break to out of a loop or a switch statement
   */
  void setBreakToBlock(llvm::BasicBlock* block);
  
  /**
   * Get the block to break to out of a loop or a switch statement
   */
  llvm::BasicBlock* getBreakToBlock();

};
  
} // namespace yazyk

#endif /* Scope_h */
