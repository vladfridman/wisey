//
//  ByteType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ByteType_h
#define ByteType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {
  
  /**
   * Represents byte expression type
   */
  class ByteType : public IPrimitiveType {
    
  public:
    
    ByteType();
    
    ~ByteType();
    
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
    
    std::string getFormat() const override;
    
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
    
    const PrimitiveTypeSpecifier* newTypeSpecifier(int line) const override;
    
    llvm::Instruction* inject(IRGenerationContext& context,
                              const InjectionArgumentList injectionArgumentList,
                              int line) const override;
    
  };
  
} /* namespace wisey */

#endif /* ByteType_h */
