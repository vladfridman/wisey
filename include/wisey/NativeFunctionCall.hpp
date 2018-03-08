//
//  NativeFunctionCall.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef NativeFunctionCall_h
#define NativeFunctionCall_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
  class NativeFunctionCall : public IStatement {
    
    llvm::Function* mFunction;
    std::vector<IExpression*> mArguments;
    
  public:
    
    NativeFunctionCall(llvm::Function* function, std::vector<IExpression*> arguments);
    
    ~NativeFunctionCall();
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* NativeFunctionCall_h */
