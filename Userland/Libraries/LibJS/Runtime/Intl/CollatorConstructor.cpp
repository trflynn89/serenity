/*
 * Copyright (c) 2021, Tim Flynn <trflynn89@pm.me>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/AbstractOperations.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/Intl/Collator.h>
#include <LibJS/Runtime/Intl/CollatorConstructor.h>

namespace JS::Intl {

// 10.1 The Intl.Collator Constructor, https://tc39.es/ecma402/#sec-the-intl-collator-constructor
CollatorConstructor::CollatorConstructor(GlobalObject& global_object)
    : NativeFunction(vm().names.Collator.as_string(), *global_object.function_prototype())
{
}

void CollatorConstructor::initialize(GlobalObject& global_object)
{
    NativeFunction::initialize(global_object);

    auto& vm = this->vm();

    // 10.2.1 Intl.Collator.prototype, https://tc39.es/ecma402/#sec-intl.collator.prototype
    define_direct_property(vm.names.prototype, global_object.intl_collator_prototype(), 0);
    define_direct_property(vm.names.length, Value(0), Attribute::Configurable);
}

// 10.1.2 Intl.Collator ( [ locales [ , options ] ] ), https://tc39.es/ecma402/#sec-intl.collator
Value CollatorConstructor::call()
{
    // 1. If NewTarget is undefined, throw a TypeError exception.
    vm().throw_exception<TypeError>(global_object(), ErrorType::ConstructorWithoutNew, "Intl.Collator");
    return {};
}

// 10.1.2 Intl.Collator ( [ locales [ , options ] ] ), https://tc39.es/ecma402/#sec-intl.collator
Value CollatorConstructor::construct(FunctionObject& new_target)
{
    auto& vm = this->vm();
    auto& global_object = this->global_object();

    // 5. Let collator be ? OrdinaryCreateFromConstructor(newTarget, "%Collator.prototype%", internalSlotsList).
    auto* collator = ordinary_create_from_constructor<Collator>(global_object, new_target, &GlobalObject::intl_collator_prototype);
    if (vm.exception())
        return {};

    return collator;
}

}
