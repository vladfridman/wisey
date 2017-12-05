//
//  BooleanType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanType_h
#define BooleanType_h

#include "wisey/IPrimitiveType.hpp"

namespace wisey {
  
/**
 * Represents char expression type
 */
class BooleanType : public IPrimitiveType {
    
public:
  
  BooleanType() { }
  
  ~BooleanType() { }
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;

  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  std::string getFormat() const override;

};

} /* namespace wisey */

#endif /* BooleanType_h */
