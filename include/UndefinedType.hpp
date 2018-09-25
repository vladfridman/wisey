//
//  UndefinedType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef UndefinedType_h
#define UndefinedType_h

#include "IType.hpp"

namespace wisey {
  
  /**
   * Represents the type that could not be determined
   */
  class UndefinedType : public IType {
    
  public:
    
    UndefinedType() { }
    
    ~UndefinedType() { }
    
    std::string getTypeName() const override;
    
    llvm::Type* getLLVMType(IRGenerationContext& context) const override;
    
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

    static const UndefinedType* UNDEFINED;

  };
  
} /* namespace wisey */

#endif /* UndefinedType_h */

