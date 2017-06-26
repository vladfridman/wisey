//
//  IBuildableConcreteObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IBuildableConcreteObjectType_h
#define IBuildableConcreteObjectType_h

#include "wisey/BuilderArgument.hpp"
#include "wisey/IConcreteObjectType.hpp"

namespace wisey {
  
/**
 * Interface representing a buildable object
 */
class IBuildableConcreteObjectType : public IConcreteObjectType {

public:
  
  /**
   * Builds an instance of this object and initializes all fields
   */
  virtual llvm::Instruction* build(IRGenerationContext& context,
                                   const BuilderArgumentList& builderArgumentList) const = 0;

};

} /* namespace wisey */

#endif /* IBuildableConcreteObjectType_h */