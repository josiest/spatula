#pragma once

// type constraints
#include <type_traits>
#include <concepts>
#include <ranges>

// data types and algorithms
#include <cstddef>
#include <utility>
#include <tuple>
#include <algorithm>

namespace sp {

/** An atomic constraint for field operations. */
template<typename Field>
constexpr bool has_field_closure = requires(Field a, Field b) {
    { a + b } -> std::convertible_to<Field>;
    { a - b } -> std::convertible_to<Field>;
    { a * b } -> std::convertible_to<Field>;
    { a / b } -> std::convertible_to<Field>;

    { a += b } -> std::convertible_to<Field&>;
    { a -= b } -> std::convertible_to<Field&>;
    { a *= b } -> std::convertible_to<Field&>;
    { a /= b } -> std::convertible_to<Field&>;
};

/** A type that models the field axioms.
 * 
 * Semantic Requirements:
 * 
 * Adapted from https://mathworld.wolfram.com/FieldAxioms.html:
 * 
 * name             | addition          | multiplication
 * ----------------------------------------------------------------
 * associativity    | (a+b)+c=a+(b+c)   | (a*b)*c=a*(b*c)
 * commutativity    | a+b=b+a           | a*b=b*a
 * distributivity   | a(b+c)=a*b+a*c    | (a+b)*c=a*c+b*c
 * identity         | a+0=a=0+a         | a*1=a=1*a
 * inverses         | a-a=0             | a/a=1 if a!=0
*/
template<class Field>
concept field = std::regular<Field> and has_field_closure<Field>;

namespace ranges = std::ranges;

//
// Atomic component constraints
//

/** Require a type to have specific components subset to a field. */
template<class Vector>
constexpr bool has_x_component = requires(Vector v) {
    has_field_closure<decltype(v.x)>;
};
template<class Vector>
constexpr bool has_X_component = requires(Vector v) {
    has_field_closure<decltype(v.X)>;
};
template<class Vector>
constexpr bool has_q_component = requires(Vector v) {
    has_field_closure<decltype(v.q)>;
};

template<class Vector>
constexpr bool has_y_component = requires(Vector v) {
    has_field_closure<decltype(v.y)>;
};
template<class Vector>
constexpr bool has_Y_component = requires(Vector v) {
    has_field_closure<decltype(v.Y)>;
};
template<class Vector>
constexpr bool has_r_component = requires(Vector v) {
    has_field_closure<decltype(v.r)>;
};

template<class Vector>
constexpr bool has_z_component = requires(Vector v) {
    has_field_closure<decltype(v.z)>;
};
template<class Vector>
constexpr bool has_Z_component = requires(Vector v) {
    has_field_closure<decltype(v.Z)>;
};

template<class Vector>
constexpr bool has_w_component = requires(Vector v) {
    has_field_closure<decltype(v.w)>;
};
template<class Vector>
constexpr bool has_W_component = requires(Vector v) {
    has_field_closure<decltype(v.W)>;
};

template<class Vector>
constexpr bool has_i_component = requires(Vector v, std::size_t i) {
    has_field_closure<decltype(v[i])>;
};

template<class Vector>
constexpr bool has_static_size = requires {
    std::tuple_size<Vector>::value;
};

template<class Vector, std::size_t N>
struct dimension_query : public std::false_type{};

template<class Vector, std::size_t N>
    requires has_static_size<Vector>
struct dimension_query<Vector, N>
    : public std::bool_constant<(std::tuple_size_v<Vector> > N)>{};

/** Determine if a vector has a component in the Nth dimension */
template<class Vector, std::size_t N>
constexpr bool has_nd_component = dimension_query<Vector, N>::value;

template<class Vector>
constexpr bool has_1d_component = 
    has_x_component<Vector> or has_X_component<Vector> or
    has_q_component<Vector> or has_nd_component<Vector, 0>;

template<class Vector>
constexpr bool has_2d_component =
    has_y_component<Vector> or has_Y_component<Vector> or
    has_r_component<Vector> or has_nd_component<Vector, 1>;

template<class Vector>
constexpr bool has_3d_component =
    has_z_component<Vector> or has_Z_component<Vector> or
    has_nd_component<Vector, 2>;

template<class Vector>
constexpr bool has_4d_component =
    has_w_component<Vector> or has_W_component<Vector> or
    has_nd_component<Vector, 3>;

//
// Scalar fields
//

/** The scalar field type of a vector. */
template<class Vector> struct scalar_field;

template<class Vector>
    requires has_x_component<Vector>
struct scalar_field<Vector> {
    using type = std::remove_reference_t<decltype(std::declval<Vector>().x)>;
};
template<class Vector>
    requires has_X_component<Vector>
struct scalar_field<Vector> {
    using type = std::remove_reference_t<decltype(std::declval<Vector>().X)>;
};
template<class Vector>
    requires has_q_component<Vector>
struct scalar_field<Vector> {
    using type = std::remove_reference_t<decltype(std::declval<Vector>().q)>;
};

// it's possible that a vector type may support operator[] and have an x field
// so in order to avoid ambiguity, we must specify for operator[] and not x field
template<class Vector>
    requires has_i_component<Vector> and
            (not (has_x_component<Vector> or has_X_component<Vector>))

struct scalar_field<Vector> {
    using type = std::remove_reference_t<
        decltype(std::declval<Vector>()[std::declval<std::size_t>()])
    >;
};

template<class Vector>
using scalar_field_t = typename scalar_field<Vector>::type;

/** Get the x component of a vector */
template<class Vector> struct x_getter{
    auto const & operator()(Vector const & v) const;
    auto & operator()(Vector & v) const;
};

template<class Vector>
    requires has_x_component<Vector>
struct x_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.x;
    }
    auto & operator()(Vector & v) const
    {
        return v.x;
    }
};

template<class Vector>
    requires has_X_component<Vector>
struct x_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.X;
    }
    auto & operator()(Vector & v) const
    {
        return v.X;
    }
};

template<class Vector>
    requires has_q_component<Vector>
struct x_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.q;
    }
    auto & operator()(Vector & v) const
    {
        return v.q;
    }
};

template<class Vector>
    requires has_i_component<Vector> and
            (not (has_x_component<Vector> or has_X_component<Vector>))
struct x_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v[0];
    }
    auto & operator()(Vector & v) const
    {
        return v[0];
    }
};

/** Get the y component of a vector */
template<class Vector> struct y_getter{
    auto const & operator()(Vector const & v) const;
    auto & operator()(Vector & v) const;
};

template<class Vector>
    requires has_y_component<Vector>
struct y_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.y;
    }
    auto & operator()(Vector & v) const
    {
        return v.y;
    }
};

template<class Vector>
    requires has_Y_component<Vector>
struct y_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.Y;
    }
    auto & operator()(Vector & v) const
    {
        return v.Y;
    }
};

template <class Vector>
    requires has_r_component<Vector>
struct y_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.r;
    }
    auto & operator()(Vector & v) const
    {
        return v.r;
    }
};

template<class Vector>
    requires has_i_component<Vector> and
            (not (has_y_component<Vector> or has_Y_component<Vector>))
struct y_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v[1];
    }
    auto & operator()(Vector & v) const
    {
        return v[1];
    }
};

/** Get the z component of a vector */
template<class Vector> struct z_getter{
    auto const & operator()(Vector const & v) const;
    auto & operator()(Vector & v) const;
};

template<class Vector>
    requires has_z_component<Vector>
struct z_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.z;
    }
    auto & operator()(Vector & v) const
    {
        return v.z;
    }
};

template<class Vector>
    requires has_Z_component<Vector>
struct z_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.Z;
    }
    auto & operator()(Vector & v) const
    {
        return v.Z;
    }
};

template<class Vector>
    requires has_i_component<Vector> and 
            (not (has_z_component<Vector> or has_Z_component<Vector>))
struct z_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v[2];
    }
    auto & operator()(Vector & v) const
    {
        return v[2];
    }
};

/** Get the w component of a vector */
template<class Vector> struct w_getter{
    auto const & operator()(Vector const & v) const;
    auto & operator()(Vector & v) const;
};

template<class Vector>
    requires has_w_component<Vector>
struct w_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.w;
    }
    auto & operator()(Vector & v) const
    {
        return v.w;
    }
};

template<class Vector>
    requires has_W_component<Vector>
struct w_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v.W;
    }
    auto & operator()(Vector & v) const
    {
        return v.W;
    }
};

template<class Vector>
    requires has_i_component<Vector> and 
            (not (has_w_component<Vector> or has_W_component<Vector>))
struct w_getter<Vector> {
    auto const & operator()(Vector const & v) const
    {
        return v[3];
    }
    auto & operator()(Vector & v) const
    {
        return v[3];
    }
};

template<class Vector>
scalar_field_t<Vector> const & get_x(Vector const & v)
{
    static x_getter<Vector> _get_x;
    return _get_x(v);
}
template<class Vector>
scalar_field_t<Vector> & get_x(Vector & v)
{
    static x_getter<Vector> _get_x;
    return _get_x(v);
}

template<class Vector>
scalar_field_t<Vector> const & get_y(Vector const & v)
{
    static y_getter<Vector> _get_y;
    return _get_y(v);
}
template<class Vector>
scalar_field_t<Vector> & get_y(Vector & v)
{
    static y_getter<Vector> _get_y;
    return _get_y(v);
}

template<class Vector>
scalar_field_t<Vector> const & get_z(Vector const & v)
{
    static z_getter<Vector> _get_z;
    return _get_z(v);
}
template<class Vector>
scalar_field_t<Vector> & get_z(Vector & v)
{
    static z_getter<Vector> _get_z;
    return _get_z(v);
}

template<class Vector>
scalar_field_t<Vector> const & get_w(Vector const & v)
{
    static w_getter<Vector> _get_w;
    return _get_w(v);
}
template<class Vector>
scalar_field_t<Vector> & get_w(Vector & v)
{
    static w_getter<Vector> _get_w;
    return _get_w(v);
}

//
// Atomic numeric-type constraints
//

/** A numeric data type.
 * 
 * Syntactic Requirements:
 *   A numeric data type is an atomic constraint that requires a type to have a
 *   set of uniformly-typed components with field closure.
 */
template<class Vector>
constexpr bool is_2d_numeric =
    has_1d_component<Vector> and has_2d_component<Vector> and
    (not (has_3d_component<Vector> or has_4d_component<Vector>)) and
requires(Vector v) {
    { get_x(v) } -> std::same_as<scalar_field_t<Vector>&>;
    { get_y(v) } -> std::same_as<scalar_field_t<Vector>&>;
};

template<class Vector>
constexpr bool is_3d_numeric =
    has_1d_component<Vector> and has_2d_component<Vector> and
    has_3d_component<Vector> and (not has_4d_component<Vector>) and
requires(Vector v) {
    { get_x(v) } -> std::same_as<scalar_field_t<Vector>&>;
    { get_y(v) } -> std::same_as<scalar_field_t<Vector>&>;
    { get_z(v) } -> std::same_as<scalar_field_t<Vector>&>;
};

template<class Vector>
constexpr bool is_4d_numeric =
    has_1d_component<Vector> and has_2d_component<Vector> and
    has_3d_component<Vector> and has_4d_component<Vector> and
requires(Vector v) {
    { get_x(v) } -> std::same_as<scalar_field_t<Vector>&>;
    { get_y(v) } -> std::same_as<scalar_field_t<Vector>&>;
    { get_z(v) } -> std::same_as<scalar_field_t<Vector>&>;
    { get_w(v) } -> std::same_as<scalar_field_t<Vector>&>;
};

//
// Constructible concepts
//

/** A type that's constructible from a sequence of field values.
 * 
 * Semantic Requirements:
 * 
 * - A field_constructible type models a data type with public uniformly-typed
 *   members that can be directly initialized through a constructor.
 * 
 * - The type's members have a semantically defined order, which corresponds to
 *   the order of the constructor's parameters.
 * 
 * Example:
 *     struct point { int x, y; };
 *     static_assert<field_2d_constructible<point>>;
 * 
 *   The point class models field_2d_constructible because all of its members
 *   (x and y) have the same type, and the default constructor assigns them in
 *   the standard order of x before y.
 * 
 *   Note, a type such as `struct yfirst_point { int y, x; };` still models
 *   field_2d_constructible if the context it's used in orders y before x.
 *   However, the original point type would not model field_2d_constructible for
 *   this context because the programmer expects point(1, 2) to assign 1 to the
 *   y member and 2 to the x member.
 */
template<class Vector>
concept field_2d_constructible =
requires(scalar_field_t<Vector> x, scalar_field_t<Vector> y) {
    Vector{x, y};
};

template<class Vector>
concept field_3d_constructible =
requires(scalar_field_t<Vector> x, scalar_field_t<Vector> y,
         scalar_field_t<Vector> z) {
    Vector{x, y, z};
};

template<class Vector>
concept field_4d_constructible =
requires(scalar_field_t<Vector> x, scalar_field_t<Vector> y,
         scalar_field_t<Vector> z, scalar_field_t<Vector> w) {
    Vector{x, y, z, w};
};

template<class Vector>
concept field_constructible = field_2d_constructible<Vector> or
                              field_3d_constructible<Vector> or
                              field_4d_constructible<Vector>;

//
// Vector concepts
//

/** A basic "plain old" vector.
 * 
 * Useful for working with c-types that haven't defined vector operations.
 */
template<class Vector>
concept semivector2 = std::semiregular<Vector> and
                      field_2d_constructible<Vector> and
                      is_2d_numeric<Vector>;

template<class Vector>
concept semivector3 = std::semiregular<Vector> and
                      field_3d_constructible<Vector> and
                      is_3d_numeric<Vector>;

template<class Vector>
concept semivector4 = std::semiregular<Vector> and
                      field_4d_constructible<Vector> and
                      is_4d_numeric<Vector>;

template<class Vector>
concept semivector = semivector2<Vector> or semivector3<Vector> or
                     semivector4<Vector>;

/** An atomic contraint for vector operations. */
template<class Vector>
constexpr bool has_vector_closure =
requires(Vector a, Vector b, scalar_field_t<Vector> c) {
    { a + b } -> std::convertible_to<Vector>;
    { a - b } -> std::convertible_to<Vector>;
    { c * a } -> std::convertible_to<Vector>;
    { a * c } -> std::convertible_to<Vector>;

    { a += b } -> std::same_as<Vector&>;
    { a -= b } -> std::same_as<Vector&>;
    { a *= c } -> std::same_as<Vector&>;
};

/** A complete vector type.
 * 
 * Semantic Requirements:
 * 
 * for two n-dimensional vectors a and b of the form
 *  v = (v[1], v[2], ..., v[n])
 * and an element c of their scalar field
 * 
 * - a + b = (a[1] + b[1], a[2] + b[2], ..., a[n] + b[n])
 * - a - b = (a[1] - b[1], a[2] - b[2], ..., a[n] - b[n])
 * - c * a = (c * a[1], c * a[2], ..., c * a[3])
 */
template<class Vector>
concept vector2 = std::regular<Vector> and
                  field_2d_constructible<Vector> and
                  is_2d_numeric<Vector> and
                  has_vector_closure<Vector>;

template<class Vector>
concept vector3 = std::regular<Vector> and
                  field_3d_constructible<Vector> and
                  is_3d_numeric<Vector> and
                  has_vector_closure<Vector>;

template<class Vector>
concept vector4 = std::regular<Vector> and
                  field_4d_constructible<Vector> and
                  is_4d_numeric<Vector> and
                  has_vector_closure<Vector>;

//
// Math Utilities
//

//
// Ordering
//

/** Order vectors by their components. */
template<class Vector>
    requires has_x_component<Vector> and
             std::totally_ordered<scalar_field_t<Vector>>
bool least_x(Vector const & u, Vector const & v)
{
    return get_x(u) < get_x(v);
}
template<class Vector>
    requires has_y_component<Vector> and
             std::totally_ordered<scalar_field_t<Vector>>
bool least_y(Vector const & u, Vector const & v)
{
    return get_y(u) < get_y(v);
}
template<class Vector>
    requires has_z_component<Vector> and
             std::totally_ordered<scalar_field_t<Vector>>
bool least_z(Vector const & u, Vector const & v)
{
    return get_z(u) < get_z(v);
}
template<class Vector>
    requires has_w_component<Vector> and
             std::totally_ordered<scalar_field_t<Vector>>
bool least_w(Vector const & u, Vector const & v)
{
    return get_w(u) < get_w(v);
}

template<std::size_t i, class Vector>
    requires has_i_component<Vector> and
             std::totally_ordered<scalar_field_t<Vector>>
bool least_by_component(Vector const & u, Vector const & v)
{
    return u[i] < v[i];
}

/** Generate the bounding corners of a set of vectors.
 *
 * Return
 *   A pair of vectors (lower-left, upper-right), representing the bounding
 *   corners of the input points.
 *
 * Parameters
 *   begin - iterator to the start the input points to find the boundary of
 *   end - sentinel for begin that ends the input points
 */
template<semivector2 Vector, std::weakly_incrementable In, std::sentinel_for<In> S>
requires std::totally_ordered<scalar_field_t<Vector>>

auto bounding_corners2d(In begin, S end)
{
    // find the points with the least and greatest x and y coordinates
    auto const &[xmin, xmax] = std::minmax(begin, end, least_x);
    auto const &[ymin, ymax] = std::minmax(begin, end, least_y);

    Vector min(get_x(xmin), get_y(ymin));
    Vector max(get_x(xmax), get_y(ymax));
    return std::make_pair(min, max);
}

#ifdef __cpp_lib_ranges
/** Generate the bounding corners of a set of vectors.
 * 
 * Return
 *   A pair of vectors (lower-left, upper-right), representing the bounding
 *   corners of the input points.
 * 
 * Parameters
 *   points - the input points to find the boundary of
 */
template<ranges::input_range Range>
    requires semivector2<ranges::range_value_t<Range>> and
             std::totally_ordered<scalar_field_t<ranges::range_value_t<Range>>>

auto bounding_corners2d(Range && points)
{
    using Vector = ranges::range_value_t<Range>;

    // find the points with the least and greatest x and y coordinates
    auto const &[xmin, xmax] = ranges::minmax(points, least_x);
    auto const &[ymin, ymax] = ranges::minmax(points, least_y);

    Vector min(get_x(xmin), get_y(ymin));
    Vector max(get_x(xmax), get_y(ymax));
    return std::make_pair(min, max);
}

#endif
}
