//
//  ITypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ITypeSpecifier_h
#define ITypeSpecifier_h

#include "yazyk/IHasType.hpp"
#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {

/**
 * Interfaces representing a type that could either be a primitive type, a model or a controller
 */
class ITypeSpecifier : public IHasType { };

} /* namespace yazyk */

#endif /* ITypeSpecifier_h */
