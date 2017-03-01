//
//  Interface.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Interface_hpp
#define Interface_hpp

#include <set>

#include <llvm/IR/Instructions.h>

#include "yazyk/IType.hpp"
#include "yazyk/Method.hpp"

namespace yazyk {
  
/**
 * Contains information about an Interface including the llvm::StructType and method information
 */
class Interface : public IType {
  std::string mName;
  llvm::StructType* mStructType;
  std::map<std::string, Method*>* mMethods;
  
public:
  
  Interface(std::string name,
        llvm::StructType* structType,
        std::map<std::string, Method*>* methods) :
  mName(name), mStructType(structType), mMethods(methods) {}
  
  ~Interface();
  
  /**
   * Findsa method with a given name
   */
  Method* findMethod(std::string methodName) const;
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(IType* toType) const override;
  
  bool canCastLosslessTo(IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      IType* toType) const override;
};
  
} /* namespace yazyk */

#endif /* Interface_hpp */
