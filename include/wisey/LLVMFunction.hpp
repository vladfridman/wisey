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
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectElement.hpp"
#include "wisey/LLVMFunctionType.hpp"

namespace wisey {
  
  /**
   * Represents an llvm function defined within an object
   */
  class LLVMFunction : public IObjectElement, public IMethodDescriptor {
    const IObjectType* mObjectType;
    std::string mName;
    bool mIsPublic;
    const LLVMFunctionType* mLLVMFunctionType;
    const IType* mReturnType;
    std::vector<const Argument*> mArguments;
    CompoundStatement* mCompoundStatement;
    MethodQualifiers* mMethodQualifiers;
    int mLine;
    
  public:
    
    LLVMFunction(const IObjectType* objectType,
                 std::string name,
                 AccessLevel accessLevel,
                 const LLVMFunctionType* llvmFunctionType,
                 const IType* returnType,
                 std::vector<const Argument*> arguments,
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
     * Returns the actual llvm function
     */
    llvm::Function* getLLVMFunction(IRGenerationContext& context) const;
    
    /**
     * Returns function type
     */
    const LLVMFunctionType* getType() const;
    
    bool isPublic() const override;
    
    std::string getName() const override;
    
    const IType* getReturnType() const override;

    std::vector<const Argument*> getArguments() const override;
    
    std::vector<const Model*> getThrownExceptions() const override;
    
    bool isStatic() const override;
    
    bool isConceal() const override;
    
    bool isReveal() const override;
    
    bool isOverride() const override;
    
    llvm::FunctionType* getLLVMType(IRGenerationContext& context) const override;
    
    const IObjectType* getParentObject() const override;
    
    MethodQualifiers* getMethodQualifiers() const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

    std::string getTypeName() const override;
    
    bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
    
    bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
    
    llvm::Value* castTo(IRGenerationContext& context,
                        llvm::Value* fromValue,
                        const IType* toType,
                        int line) const override;
    
    bool isPrimitive() const override;
    
    bool isOwner() const override;
    
    bool isReference() const override;
    
    bool isArray() const override;
    
    bool isFunction() const override;
    
    bool isPackage() const override;
    
    bool isController() const override;
    
    bool isInterface() const override;
    
    bool isModel() const override;
    
    bool isNode() const override;
    
    bool isNative() const override;
    
    bool isPointer() const override;

    bool isImmutable() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;

    const ArrayType* getArrayType(IRGenerationContext& context) const override;
    
    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;

  private:
    
    void createArguments(IRGenerationContext& context, llvm::Function* function) const;
    
    void createSystemVariables(IRGenerationContext& context, llvm::Function* function) const;
    
    void maybeAddImpliedVoidReturn(IRGenerationContext& context, int line) const;

  };
  
} /* namespace wisey */

#endif /* LLVMFunction_h */
