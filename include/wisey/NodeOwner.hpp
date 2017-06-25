//
//  NodeOwner.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef NodeOwner_h
#define NodeOwner_h

#include "wisey/IObjectOwnerType.hpp"
#include "wisey/Node.hpp"

namespace wisey {
  
/**
 * Owner type for node
 */
class NodeOwner : public IObjectOwnerType {
    
  Node* mNode;
  
public:
  
  NodeOwner(Node* node) : mNode(node) { }
  
  ~NodeOwner() { }
  
  Node* getObject() const override;
  
  std::string getName() const override;
  
  llvm::PointerType* getLLVMType(llvm::LLVMContext& llvmContext) const override;
  
  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType) const override;
  
};
  
} /* namespace wisey */


#endif /* NodeOwner_h */
