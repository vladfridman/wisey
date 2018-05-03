//
//  StaticMethod.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef StaticMethod_h
#define StaticMethod_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/MethodQualifier.hpp"

namespace wisey {
  
  class CompoundStatement;
  class Model;
  
  /**
   * Represents concrete object's static method
   */
  class StaticMethod : public IMethod {
    const IObjectType* mObjectType;
    std::string mName;
    bool mIsPublic;
    const IType* mReturnType;
    std::vector<const Argument*> mArguments;
    std::vector<const Model*> mThrownExceptions;
    MethodQualifiers* mMethodQualifiers;
    CompoundStatement* mCompoundStatement;
    int mLine;
    
  public:
    
    StaticMethod(const IObjectType* objectType,
                 std::string name,
                 AccessLevel accessLevel,
                 const IType* returnType,
                 std::vector<const Argument*> arguments,
                 std::vector<const Model*> thrownExceptions,
                 MethodQualifiers* methodQualifiers,
                 CompoundStatement* compoundStatement,
                 int line);
    
    ~StaticMethod();
    
    bool isStatic() const override;
    
    llvm::Function* defineFunction(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
    std::string getName() const override;
    
    bool isPublic() const override;

    const IType* getReturnType() const override;
    
    std::vector<const Argument*> getArguments() const override;
    
    std::vector<const Model*> getThrownExceptions() const override;
    
    int getLine() const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;
    
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
    
    bool isNative() const override;
    
    bool isPointer() const override;

    bool isImmutable() const override;

    const IObjectType* getParentObject() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context,
                             std::string name,
                             int line) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object,
                             int line) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value,
                                 int line) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context, int line) const override;

    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;

  private:
    
    void createArguments(IRGenerationContext& context, llvm::Function* function) const;
    
  };
  
} /* namespace wisey */

#endif /* StaticMethod_h */

