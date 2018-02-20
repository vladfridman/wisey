//
//  FloatType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FloatType_h
#define FloatType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {
  
/**
 * Represents float expression type
 */
class FloatType : public IPrimitiveType {
    
public:
  
  FloatType() { }
  
  ~FloatType() { }
  
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

#endif /* FloatType_h */
