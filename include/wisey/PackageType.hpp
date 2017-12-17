//
//  PackageType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef PackageType_h
#define PackageType_h

#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents a type that repressents a package identifier
 */
class PackageType : public IType {
  
  std::string mPackageName;
  
public:
  
  PackageType(std::string packageName);
  
  ~PackageType();
  
  std::string getTypeName() const override;
  
  llvm::Type* getLLVMType(IRGenerationContext& context) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
  const ArrayElementType* getArrayElementType() const override;

};

} /* namespace wisey */

#endif /* PackageType_h */
