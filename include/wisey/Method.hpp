//
//  Method.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Method_h
#define Method_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/MethodQualifier.hpp"

namespace wisey {
  
  class CompoundStatement;
  class Model;
  
  /**
   * Contains information about a method including its return type and all of its arguments
   */
  class Method : public IMethod {
    const IObjectType* mObjectType;
    std::string mName;
    AccessLevel mAccessLevel;
    const IType* mReturnType;
    std::vector<MethodArgument*> mArguments;
    std::vector<const Model*> mThrownExceptions;
    MethodQualifiers* mMethodQualifiers;
    CompoundStatement* mCompoundStatement;
    int mLine;
    
  public:
    
    Method(const IObjectType* objectType,
           std::string name,
           AccessLevel accessLevel,
           const IType* returnType,
           std::vector<MethodArgument*> arguments,
           std::vector<const Model*> thrownExceptions,
           MethodQualifiers* methodQualifiers,
           CompoundStatement* compoundStatement,
           int line);
    
    ~Method();
    
    bool isStatic() const override;
    
    llvm::Function* defineFunction(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
    std::string getName() const override;
    
    AccessLevel getAccessLevel() const override;
    
    const IType* getReturnType() const override;
    
    std::vector<MethodArgument*> getArguments() const override;
    
    std::vector<const Model*> getThrownExceptions() const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;
    
    bool isReveal() const override;
    
    bool isOverride() const override;
    
    MethodQualifiers* getMethodQualifiers() const override;

    std::string getTypeName() const override;
    
    llvm::FunctionType* getLLVMType(IRGenerationContext& context) const override;
    
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
    
    bool isThread() const override;
    
    bool isNative() const override;
    
    bool isPointer() const override;

    const IObjectType* getParentObject() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context) const override;

  private:
    
    void createArguments(IRGenerationContext& context, llvm::Function* function) const;
    
    void addThreadGuard(IRGenerationContext& context, llvm::Function* function) const;
    
  };
  
} /* namespace wisey */

#endif /* Method_h */

