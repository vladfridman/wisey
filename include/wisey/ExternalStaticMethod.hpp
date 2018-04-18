//
//  ExternalStaticMethod.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ExternalStaticMethod_h
#define ExternalStaticMethod_h

#include <llvm/IR/DerivedTypes.h>

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethod.hpp"
#include "wisey/MethodQualifier.hpp"

namespace wisey {
  
  class Model;
  
  /**
   * Contains information about an external static method implemented in a library
   */
  class ExternalStaticMethod : public IMethod {
    const IObjectType* mObjectType;
    std::string mName;
    const IType* mReturnType;
    std::vector<MethodArgument*> mArguments;
    std::vector<const Model*> mThrownExceptions;
    MethodQualifiers* mMethodQualifiers;
    int mLine;

  public:
    
    ExternalStaticMethod(const IObjectType* objectType,
                         std::string name,
                         const IType* returnType,
                         std::vector<MethodArgument*> arguments,
                         std::vector<const Model*> thrownExceptions,
                         MethodQualifiers* methodQualifiers,
                         int line);
    
    ~ExternalStaticMethod();
    
    bool isStatic() const override;
    
    llvm::Function* defineFunction(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
    std::string getName() const override;
    
    bool isPublic() const override;

    const IType* getReturnType() const override;
    
    std::vector<MethodArgument*> getArguments() const override;
    
    std::vector<const Model*> getThrownExceptions() const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;
    
    bool isConceal() const override;

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

    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;

  };
  
} /* namespace wisey */

#endif /* ExternalStaticMethod_h */

