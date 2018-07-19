//
//  HeapBuilder.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef HeapBuilder_h
#define HeapBuilder_h

#include "wisey/IBuilder.hpp"
#include "wisey/IBuildableObjectTypeSpecifier.hpp"
#include "wisey/BuilderArgument.hpp"

namespace wisey {
  
  /**
   * Represents builder used to allocate buildable objects on heap
   *
   * The object builder is used as follows in the wisey language:
   *
   * ModelType* modelA = build(ModelType).withField(1).onHeap();
   */
  class HeapBuilder : public IBuilder {
    
    IBuildableObjectTypeSpecifier* mTypeSpecifier;
    BuilderArgumentList mBuilderArgumentList;
    int mLine;
    
  public:
    
    HeapBuilder(IBuildableObjectTypeSpecifier* typeSpecifier,
                BuilderArgumentList BuilderArgumentList,
                int line);
    
    ~HeapBuilder();
    
    int getLine() const override;

    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    llvm::Value* build(IRGenerationContext& context,
                       const IBuildableObjectType* buildable) const;

  };
  
} /* namespace wisey */

#endif /* HeapBuilder_h */

