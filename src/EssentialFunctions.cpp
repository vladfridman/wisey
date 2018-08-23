//
//  EssentialFunctions.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/AdjustReferenceCounterForArrayFunction.hpp"
#include "wisey/AdjustReferenceCounterForImmutableArrayFunction.hpp"
#include "wisey/AdjustReferenceCountFunction.hpp"
#include "wisey/CastObjectFunction.hpp"
#include "wisey/CheckArrayNotReferencedFunction.hpp"
#include "wisey/CheckCastToObjectFunction.hpp"
#include "wisey/DestroyNativeObjectFunction.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/DestroyObjectOwnerFunction.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/GetOriginalObjectNameFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/InstanceOfFunction.hpp"
#include "wisey/IsObjectFunction.hpp"
#include "wisey/EssentialFunctions.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/ThrowArrayIndexExceptionFunction.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

EssentialFunctions::~EssentialFunctions() {
}

void EssentialFunctions::generateIR(IRGenerationContext& context) const {
  AdjustReferenceCounterForArrayFunction::get(context);
  AdjustReferenceCounterForImmutableArrayFunction::get(context);
  AdjustReferenceCountFunction::get(context);
  CastObjectFunction::get(context);
  CheckArrayNotReferencedFunction::get(context);
  CheckCastToObjectFunction::get(context);
  DestroyNativeObjectFunction::get(context);
  DestroyOwnerArrayFunction::get(context);
  DestroyObjectOwnerFunction::get(context);
  DestroyPrimitiveArrayFunction::get(context);
  DestroyReferenceArrayFunction::get(context);
  GetOriginalObjectFunction::get(context);
  GetOriginalObjectNameFunction::get(context);
  InstanceOfFunction::get(context);
  IsObjectFunction::get(context);
  ThrowArrayIndexExceptionFunction::get(context);
  ThrowReferenceCountExceptionFunction::get(context);
}
