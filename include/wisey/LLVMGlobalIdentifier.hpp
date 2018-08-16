//
//  LLVMGlobal.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMGlobal_h
#define LLVMGlobal_h

#include "wisey/IExpression.hpp"

namespace wisey {
  
  /**
   * Represents a global llvm type variable
   */
  class LLVMGlobalIdentifier : public IExpression {
    std::string mName;
    int mLine;
    
  public:
    
    LLVMGlobalIdentifier(std::string name, int line);
    
    ~LLVMGlobalIdentifier();
    
    int getLine() const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    void reportError(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* LLVMGlobal_h */
