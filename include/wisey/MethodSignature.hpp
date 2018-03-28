//
//  MethodSignature.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodSignature_h
#define MethodSignature_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectElement.hpp"

namespace wisey {
  
  class Interface;
  
  class MethodSignature : public IMethodDescriptor, public IObjectElement {
    
    const IObjectType* mObjectType;
    std::string mName;
    const IType* mReturnType;
    std::vector<MethodArgument*> mArguments;
    std::vector<const Model*> mThrownExceptions;
    
  public:
    
    MethodSignature(const IObjectType* objectType,
                    std::string name,
                    const IType* returnType,
                    std::vector<MethodArgument*> arguments,
                    std::vector<const Model*> thrownExceptions);
    
    ~MethodSignature();
    
    /**
     * Creates a copy of the object with a different index
     */
    MethodSignature* createCopy(const Interface* interface) const;
    
    bool isStatic() const override;
    
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
    
    const IObjectType* getReferenceType() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context) const override;
    
    const ILLVMPointerType* getPointerType() const override;

  };
  
} /* namespace wisey */

#endif /* MethodSignature_h */

