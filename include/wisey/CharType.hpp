//
//  CharType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CharType_h
#define CharType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {

/**
 * Represents char expression type
 */
class CharType : public IPrimitiveType {
  
public:
  
  CharType() { }
  
  ~CharType() { }
  
  std::string getTypeName() const override;
  
  llvm::Type* getLLVMType(IRGenerationContext& context) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  bool canAutoCastTo(IRGenerationContext& context, const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  bool isOwner() const override;
  
  bool isReference() const override;

  std::string getFormat() const override;

  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
  void createLocalVariable(IRGenerationContext& context, std::string name) const override;
  
  void createFieldVariable(IRGenerationContext& context,
                           std::string name,
                           const IConcreteObjectType* object) const override;

  const ArrayType* getArrayType(IRGenerationContext& context) const override;

};

} /* namespace wisey */

#endif /* CharType_h */
