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
  
  StringType() { }
  
  ~StringType() { }
  
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

#endif /* StringType_h */
