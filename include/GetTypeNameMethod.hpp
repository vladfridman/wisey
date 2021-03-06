//
//  GetTypeNameMethod.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/12/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef GetTypeNameMethod_h
#define GetTypeNameMethod_h

#include "IMethodDescriptor.hpp"
#include "MethodQualifier.hpp"

namespace wisey {
  
  /**
   * Represents getTypeName() method that can be executed on all objects
   */
  class GetTypeNameMethod : public IMethodDescriptor {
    
    MethodQualifiers* mMethodQualifiers;
    
    GetTypeNameMethod();
    
  public:
    
    ~GetTypeNameMethod();
    
    /**
     * The only instance of GetTypeNameMethod
     */
    static const GetTypeNameMethod* GET_TYPE_NAME_METHOD;
    
    /**
     * Name of the getObjectName method
     */
    static const std::string GET_TYPE_NAME_METHOD_NAME;
    
    /**
     * Generates IR to get array size
     */
    static llvm::Value* generateIR(IRGenerationContext& context, const IObjectType* objectType);
    
    std::string getName() const override;
    
    bool isPublic() const override;
    
    const IType* getReturnType() const override;
    
    std::vector<const Argument*> getArguments() const override;
    
    std::vector<const Model*> getThrownExceptions() const override;
    
    bool isStatic() const override;
    
    bool isOverride() const override;
    
    llvm::FunctionType* getLLVMType(IRGenerationContext& context) const override;
    
    const IObjectType* getParentObject() const override;
    
    MethodQualifiers* getMethodQualifiers() const override;
    
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
    
    llvm::Value* inject(IRGenerationContext& context,
                        const InjectionArgumentList injectionArgumentList,
                        int line) const override;
    
    const wisey::ArrayType* getArrayType(IRGenerationContext& context,
                                         int line) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* GetTypeNameMethod_h */
