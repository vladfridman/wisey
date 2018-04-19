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
   * Represents a function identifier in a wisey program of the form wisey.lang.CController.function
   * or CController.function when it is used as an identifier, e.g. an argument to some method
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

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    LLVMFunction* getLLVMFunctionObject(IRGenerationContext&  context) const;
    
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionIdentifier_h */
