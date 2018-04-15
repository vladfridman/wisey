//
//  LLVMFunctionIdentifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionIdentifier_h
#define LLVMFunctionIdentifier_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a method identifier in a wisey program of the form wisey.lang.CController.method
   * or CController.method
   */
  class LLVMFunctionIdentifier : public IExpression {
    
    const IObjectTypeSpecifier* mObjectTypeSpecifier;
    const std::string mLLVMFunctionName;
    int mLine;
    
  public:
    
    LLVMFunctionIdentifier(const IObjectTypeSpecifier* objectTypeSpecifier,
                           std::string llvmFunctionName,
                           int line);
    
    ~LLVMFunctionIdentifier();
    
    int getLine() const override;

    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    LLVMFunction* getLLVFunction(IRGenerationContext&  context) const;
    
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionIdentifier_h */
