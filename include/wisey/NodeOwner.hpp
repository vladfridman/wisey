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
  
  NodeOwner(Node* node);
  
  ~NodeOwner();
  
  Node* getObject() const override;
  
  std::string getTypeName() const override;
  
  llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
  
  void free(IRGenerationContext& context, llvm::Value* value) const override;

  TypeKind getTypeKind() const override;
  
  bool canCastTo(const IType* toType) const override;
  
  bool canAutoCastTo(const IType* toType) const override;
  
  llvm::Value* castTo(IRGenerationContext& context,
                      llvm::Value* fromValue,
                      const IType* toType,
                      int line) const override;
  
};
  
} /* namespace wisey */


#endif /* NodeOwner_h */
