//
//  ThreadOwner.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ThreadOwner_h
#define ThreadOwner_h

#include "wisey/Thread.hpp"
#include "wisey/IObjectOwnerType.hpp"

namespace wisey {
  
  /**
   * Owner type for thread
   */
  class ThreadOwner : public IObjectOwnerType {
    
    Thread* mThread;
    
  public:
    
    ThreadOwner(Thread* thread);
    
    ~ThreadOwner();
    
    Thread* getReference() const override;
    
    std::string getTypeName() const override;
    
    llvm::PointerType* getLLVMType(IRGenerationContext& context) const override;
    
    void free(IRGenerationContext& context, llvm::Value* value, int line) const override;
    
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
    
    bool isPointer() const override;

    llvm::Function* getDestructorFunction(IRGenerationContext& context, int line) const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    void createLocalVariable(IRGenerationContext& context, std::string name) const override;
    
    void createFieldVariable(IRGenerationContext& context,
                             std::string name,
                             const IConcreteObjectType* object) const override;
    
    void createParameterVariable(IRGenerationContext& context,
                                 std::string name,
                                 llvm::Value* value) const override;
    
    const ArrayType* getArrayType(IRGenerationContext& context) const override;

  };
  
} /* namespace wisey */

#endif /* ThreadOwner_h */
