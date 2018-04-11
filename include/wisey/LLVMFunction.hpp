//
//  LLVMFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunction_h
#define LLVMFunction_h

#include "wisey/AccessLevel.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/IObjectElement.hpp"
#include "wisey/LLVMFunctionArgument.hpp"
#include "wisey/LLVMFunctionType.hpp"

namespace wisey {
  
  /**
   * Represents an llvm function defined within an object
   */
  class LLVMFunction : public IObjectElement {
    std::string mName;
    AccessLevel mAccessLevel;
    const LLVMFunctionType* mLLVMFunctionType;
    const IType* mReturnType;
    std::vector<const LLVMFunctionArgument*> mArguments;
    CompoundStatement* mCompoundStatement;
    int mLine;
    
  public:
    
    LLVMFunction(std::string name,
                 AccessLevel accessLevel,
                 const LLVMFunctionType* llvmFunctionType,
                 const IType* returnType,
                 std::vector<const LLVMFunctionArgument*> arguments,
                 CompoundStatement* compoundStatement,
                 int line);

    ~LLVMFunction();
    
    /**
     * Declares llvm function
     */
    llvm::Value* declareFunction(IRGenerationContext& context, const IObjectType* objectType) const;

    /**
     * Generates llvm function body IR code
     */
    void generateBodyIR(IRGenerationContext& context, const IObjectType* objectType) const;
    
    /**
     * Returns function name
     */
    std::string getName() const;
    
    /**
     * Returns the actual llvm function
     */
    llvm::Function* getLLVMFunction(IRGenerationContext& context,
                                    const IObjectType* objectType) const;
    
    /**
     * Returns function type
     */
    const LLVMFunctionType* getType() const;

    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  private:
    
    void createArguments(IRGenerationContext& context, llvm::Function* function) const;
    
    void createSystemVariables(IRGenerationContext& context, llvm::Function* function) const;
    
    void maybeAddImpliedVoidReturn(IRGenerationContext& context, int line) const;

  };
  
} /* namespace wisey */

#endif /* LLVMFunction_h */
