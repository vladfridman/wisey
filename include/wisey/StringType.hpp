//
//  StringType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef StringType_h
#define StringType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {
  
  /**
   * Represents string expression type
   */
  class StringType : public IPrimitiveType {
    
  public:
    
    StringType();
    
    ~StringType();
    
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
    
    bool isThread() const override;
    
    bool isNative() const override;
    
    std::string getFormat() const override;
    
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

    const PrimitiveTypeSpecifier* newTypeSpecifier() const override;

  };
  
} /* namespace wisey */

#endif /* StringType_h */

