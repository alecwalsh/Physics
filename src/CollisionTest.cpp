#include "CollisionTest.hpp"

#include <glm/geometric.hpp>

#include <type_traits>
#include <utility>

namespace Physics {

    namespace {
        constexpr bool RangesOverlap(float min1, float max1, float min2, float max2) {
            return min1 <= max2 && max1 >= min2;
        }

        constexpr bool CheckRanges(float f1, float size1, float f2, float size2) {
            return RangesOverlap(f1 - size1 / 2, f1 + size1 / 2, f2 - size2 / 2, f2 + size2 / 2);
        }

        constexpr bool CubesCollideSimple(const Physics::SimpleCubeCollider& cube1, const Physics::SimpleCubeCollider& cube2) {
            auto position1 = cube1.position;
            auto size1 = cube1.size;
            auto position2 = cube2.position;
            auto size2 = cube2.size;

            bool xOverlap = CheckRanges(position1.x, size1.x, position2.x, size2.x);
            bool yOverlap = CheckRanges(position1.y, size1.y, position2.y, size2.y);
            bool zOverlap = CheckRanges(position1.z, size1.z, position2.z, size2.z);

            return xOverlap && yOverlap && zOverlap;
        }
    }

    namespace {
        template<typename T, typename U>
        struct SupportsCollisionImpl;

        template<typename T, typename U, typename = void>
        struct IsDefined : std::false_type {};

        template<typename T, typename U>
        struct IsDefined<T, U, std::void_t<decltype(SupportsCollisionImpl<T, U>::value)>> : std::true_type {};
    }

#if __cpp_concepts >= 201907L
    template<std::derived_from<Collider> T, std::derived_from<Collider> U>
    bool SupportsCollision() noexcept {
#else
    template<typename T, typename U>
    bool SupportsCollision() noexcept {
#endif
        if constexpr(IsDefined<T, U>::value) {
            return SupportsCollisionImpl<T, U>::value;
        } else {
            return SupportsCollisionImpl<U, T>::value;
        }
    }

// TODO: Find a better solution than these ugly macros

#define INSTANTIATE_SUPPORTS_COLLISION_BASE(Type1, Type2) \
    template bool SupportsCollision<Type1, Type2>() noexcept;

#define INSTANTIATE_SUPPORTS_COLLISION1(Type1, Type2) \
    INSTANTIATE_SUPPORTS_COLLISION_BASE(Type1, Type2)

#define INSTANTIATE_SUPPORTS_COLLISION2(Type1, Type2) \
    INSTANTIATE_SUPPORTS_COLLISION_BASE(Type1, Type2) \
    INSTANTIATE_SUPPORTS_COLLISION_BASE(Type2, Type1)

#define SPECIALIZE_SUPPORTS_COLLISION_IMPL(Type1, Type2, Value) \
    namespace { \
        template<> struct SupportsCollisionImpl<Type1, Type2> { static constexpr bool value = Value; };\
    }

#define IMPLEMENT_METHOD_SIGNATURE(Type1, Type2) \
    CollisionResult CollidesImpl::operator()(const Type1& collider1, const Type2& collider2)

#define NOTIMPLEMENTED_METHOD_DEFINITION(Type1, Type2) \
    IMPLEMENT_METHOD_SIGNATURE(Type1, Type2) { \
        throw NotImplementedException{collider1, collider2}; \
    }

#define IMPLEMENT_COMMON(Type1, Type2, Same) \
    SPECIALIZE_SUPPORTS_COLLISION_IMPL(Type1, Type2, true) \
    INSTANTIATE_SUPPORTS_COLLISION##Same(Type1, Type2) \
    IMPLEMENT_METHOD_SIGNATURE(Type1, Type2)

#define IMPLEMENT1(Type1) \
    IMPLEMENT_COMMON(Type1, Type1, 1)

#define IMPLEMENT2(Type1, Type2) \
    IMPLEMENT_COMMON(Type1, Type2, 2)

#define NOTIMPLEMENTED_COMMON(Type1, Type2, Same) \
    SPECIALIZE_SUPPORTS_COLLISION_IMPL(Type1, Type2, false) \
    NOTIMPLEMENTED_METHOD_DEFINITION(Type1, Type2) \
    INSTANTIATE_SUPPORTS_COLLISION##Same(Type1, Type2)

#define NOTIMPLEMENTED1(Type1) \
    NOTIMPLEMENTED_COMMON(Type1, Type1, 1)

#define NOTIMPLEMENTED2(Type1, Type2) \
    NOTIMPLEMENTED_COMMON(Type1, Type2, 2)



    NOTIMPLEMENTED1(SimplePlaneCollider);

    IMPLEMENT2(SimplePlaneCollider, SimpleCubeCollider) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        return (height - collider2.size.y / 2) < planeHeight && (height + collider2.size.y / 2) > planeHeight;
    }

    IMPLEMENT2(SimplePlaneCollider, SphereCollider) {
        auto planeHeight = collider1.position.y;
        auto height = collider2.position.y;

        return (height - collider2.size.y / 2) < planeHeight && (height + collider2.size.y / 2) > planeHeight;
    }

    IMPLEMENT1(SimpleCubeCollider) {
        return CubesCollideSimple(collider1, collider2);
    }
    NOTIMPLEMENTED2(SimpleCubeCollider, SphereCollider);

    IMPLEMENT1(SphereCollider) {
        auto v = collider1.position - collider2.position;

        auto r = (collider1.size + collider2.size).x / 2;

        return glm::dot(v, v) < r * r;
    }

#undef IMPLEMENT
#undef NOTIMPLEMENTED
}