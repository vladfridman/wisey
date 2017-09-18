//
//  IPrintStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IPrintStatement_h
#define IPrintStatement_h

#include "wisey/IExpression.hpp"
#include "wisey/IStatement.hpp"

namespace wisey {

/**
 * Rrepresents a print statement such as printout and printerr
 */
class IPrintStatement : public IStatement {

public:
 
  /**
   * Returns a printf type format string corresponding to expression types in the given list
   */
  static llvm::Value* getFormatString(IRGenerationContext& context,
                                      std::vector<IExpression*> expressionList);
  
};
  
} /* namespace wisey */


#endif /* IPrintStatement_h */
