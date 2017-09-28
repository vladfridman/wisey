//
//  FieldKind.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldKind_h
#define FieldKind_h

namespace wisey {

/**
 * An object field can be either fixed, state, injected or received
 */
typedef enum FieldKindEnum {
  FIXED_FIELD,
  STATE_FIELD,
  INJECTED_FIELD,
  RECEIVED_FIELD,
} FieldKind;

} /* namespace wisey */

#endif /* FieldKind_h */
