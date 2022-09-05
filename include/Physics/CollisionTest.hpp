#pragma once

#include "Collider.hpp"

#include <type_traits>
#include <concepts>

namespace Physics {
    // Making this a struct with operator() allows us to use std::is_invocable
    struct CollidesImpl {
        CollisionResult operator()(const SimplePlaneCollider&, const SimplePlaneCollider&);
        CollisionResult operator()(const SimplePlaneCollider&, const SimpleCubeCollider&);
        CollisionResult operator()(const SimplePlaneCollider&, const SphereCollider&);

        CollisionResult operator()(const SimpleCubeCollider&, const SimpleCubeCollider&);
        CollisionResult operator()(const SimpleCubeCollider&, const SphereCollider&);

        CollisionResult operator()(const SphereCollider&, const SphereCollider&);
    };

    namespace {
        template<typename T, typename U>
        struct SupportsCollisionImpl;

        namespace detail {
            template<typename T, typename U, typename = void>
            struct IsDefined : std::false_type {};

            template<typename T, typename U>
            struct IsDefined<T, U, std::void_t<decltype(SupportsCollisionImpl<T, U>::value)>> : std::true_type {};
        }
    }

    template<std::derived_from<Collider> T, std::derived_from<Collider> U>
    bool SupportsCollision() noexcept {
        if constexpr(detail::IsDefined<T, U>::value) {
            return SupportsCollisionImpl<T, U>::value;
        }
        else {
            return SupportsCollisionImpl<U, T>::value;
        }
    }

    // Collision testing is symmetric
    // This function switches the order of arguments if necessary
    // This allows us to cut the number of required collision testing functions by half
    template<std::derived_from<Collider> T, std::derived_from<Collider> U>
    CollisionResult Collides(const T& t, const U& u) {
        constexpr bool invokableTU = std::is_invocable_v<CollidesImpl, T, U>;
        constexpr bool invokableUT = std::is_invocable_v<CollidesImpl, U, T>;

        static_assert(invokableTU || invokableUT);

        if constexpr(invokableTU) {
            return CollidesImpl{}(t, u);
        }
        else {
            auto result = CollidesImpl{}(u, t);

            // If the order of the arguments are switched, then the collision normal must be reversed
            result.normal = -result.normal;

            return result;
        }
    }

// TODO: Find a better solution than these ugly macros

#define SPECIALIZE_SUPPORTS_COLLISION_IMPL(Type1, Type2, Value) \
    namespace { \
        template<> struct SupportsCollisionImpl<Type1, Type2> { static constexpr bool value = Value; };\
    }

#define IMPLEMENT(Type1, Type2) \
    SPECIALIZE_SUPPORTS_COLLISION_IMPL(Type1, Type2, true)

#define NOTIMPLEMENTED(Type1, Type2) \
    SPECIALIZE_SUPPORTS_COLLISION_IMPL(Type1, Type2, false)


    NOTIMPLEMENTED(SimplePlaneCollider, SimplePlaneCollider);

    IMPLEMENT(SimplePlaneCollider, SimpleCubeCollider);

    IMPLEMENT(SimplePlaneCollider, SphereCollider);

    IMPLEMENT(SimpleCubeCollider, SimpleCubeCollider);

    NOTIMPLEMENTED(SimpleCubeCollider, SphereCollider);

    IMPLEMENT(SphereCollider, SphereCollider);

}

#undef SPECIALIZE_SUPPORTS_COLLISION_IMPL
#undef IMPLEMENT
#undef NOTIMPLEMENTED