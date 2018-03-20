//
//  LLVMFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunction_h
#define LLVMFunction_h

#include "wisey/CompoundStatement.hpp"
#include "wisey/ILLVMType.hpp"
#include "wisey/IObjectElement.hpp"
#include "wisey/LLVMFunctionArgument.hpp"

namespace wisey {
  
  /**
   * Represents an llvm function defined within an object
   */
  class LLVMFunction : public IObjectElement {
    std::string mName;
    const ILLVMType* mReturnType;
    std::vector<const LLVMFunctionArgument*> mArguments;
    CompoundStatement* mComoundStatement;
    int mLine;
    
  public:
    
    LLVMFunction(std::string name,
                 const ILLVMType* returnType,
                 std::vector<const LLVMFunctionArgument*> arguments,
                 CompoundStatement* comoundStatement,
                 int line);

    ~LLVMFunction();
    
    /**
     * Generates IR defining llvm function
     */
    llvm::Value* generateIR(IRGenerationContext& context, const IObjectType* objectType) const;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* LLVMFunction_h */
