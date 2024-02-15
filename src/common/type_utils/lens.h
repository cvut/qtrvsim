/**
 * Tiny lenses library
 *
 * TL;DR: Getter function which works on nested structs.
 *
 * Example:
 * ```
 *  struct A {
 *    int b,
 *    struct {
 *      int d;
 *    } c;
 *  }
 *
 *  Lens<A, int> lens = LENS(A, c.d);
 * ```
 * `Lens<A, int>` is a function pointer to a function with a signature
 * `const A& -> const int&`
 * int`, i.e., it takes a reference to an instance of type A and "shows" you
 * something somewhere inside the object -- the integer `d`.
 *
 * In functional programming, the term lens is used and a generalized and
 * more flexible form of getter method. First, it is a function. Second, it
 * works on arbitrarily nested structures and it hides the inner structure.
 *
 * @file
 */
#ifndef QTRVSIM_LENS_H
#define QTRVSIM_LENS_H

template<typename BASE, typename FIELD_TYPE>
using Lens = const FIELD_TYPE &(*const)(const BASE &);

#define LENS(BASE_TYPE, MEMBER)                                                                    \
    ([](const BASE_TYPE &base) -> const auto & {                                                    \
        return base.MEMBER;                                                                        \
    })

template<typename BASE, typename FIELD_TYPE_A, typename FIELD_TYPE_B>
using LensPair = std::pair<const FIELD_TYPE_A &, const FIELD_TYPE_B &> (*const)(const BASE &);

#define LENS_PAIR(BASE_TYPE, MEMBER_A, MEMBER_B)                                                   \
    [](const BASE_TYPE &base) -> std::pair<typeof(base.MEMBER_A) &, typeof(base.MEMBER_B) &> {     \
        return { base.MEMBER_A, base.MEMBER_B };                                                   \
    }

#endif // QTRVSIM_LENS_H
