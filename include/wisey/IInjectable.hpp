//
//  IInjectable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IInjectable_h
#define IInjectable_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"

namespace wisey {
  
/**
 * Interface representing an injectable object type such as controller and interface
 */
class IInjectable {
    
public:
  
  virtual ~IInjectable() { }
  
  /**
   * Inject an instance of this controller into LLVM code
   */
  virtual llvm::Instruction* inject(IRGenerationContext& context,
                                    ExpressionList expressionList,
                                    int line) const = 0;

};

} /* namespace wisey */

#endif /* IInjectable_h */
