//
//  IMethodCall.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IMethodCall_h
#define IMethodCall_h

#include "IExpression.hpp"
#include "IObjectType.hpp"

namespace wisey {
  
  /**
   * Represents a method call that could either be static or non-static method call
   */
  class IMethodCall : public IExpression {
    
  public:
    
    /**
     * Translate object method name into its LLVM implemenation function name
     */
    static std::string translateObjectMethodToLLVMFunctionName(const IObjectType* object,
                                                               std::string methodName);
    
  };
  
} /* namespace wisey */

#endif /* IMethodCall_h */

