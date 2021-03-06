//
//  StringType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef StringType_h
#define StringType_h

#include "IPrimitiveType.hpp"

namespace wisey {
  
  /**
   * Represents string expression type
   */
  class StringType : public IPrimitiveType {
    
    const static std::string EMPTY_STRING_CONSTANT_NAME;
    
  public:
    
    StringType();
    
    ~StringType();
    
    /**
     * Tells whether the given type is one of the various representations of a string
     */
    static bool isStringVariation(IRGenerationContext& context, const IType* type, int line);

    /**
     * Tells if the type is a char array
     */
    static bool isCharArray(IRGenerationContext& context, const IType* type, int line);

    static llvm::Value* callGetContent(IRGenerationContext& context,
                                       const IType* type,
                                       llvm::Value* object,
                                       int line);
    
    std::string getTypeName() const override;
    
    llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
    
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

#endif /* StringType_h */

