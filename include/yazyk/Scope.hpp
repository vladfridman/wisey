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

public:

  Scope() { }
  
  /**
   * Returns map of local variables associated with this program block
   */
  std::map<std::string, llvm::Value*>& getLocals();
};
  
} // namespace yazyk

#endif /* Scope_h */
