// File: libminimk/runtime/handle.hpp
// Purpose: handle management
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LIBMINIMK_RUNTIME_HANDLE_HPP
#define LIBMINIMK_RUNTIME_HANDLE_HPP

#include <minimk/assert.h> // for MINIMK_ASSERT
#include <minimk/cdefs.h>  // for MINIMK_BEGIN_DECLS

#include <stdint.h> // for uint8_t

/*-
  Handles
  -------

  Externally, we represent handles using uint64_t. The zero
  handles is the canonically invalid handle.

  We never reuse socket IDs. Each new allocation uses the
  next available ID. Until we run out of handle IDs.

  Each handle contains the following information:

   64---------56---------------8----------0
    | type: 8 | generation: 48 | index: 8 |
    +---------+----------------+----------+

  Where type is the handle type, generation is the generation
  to which the handle belongs, index is the index inside the
  handle table used for that handle type.
*/

/// The maximum number of handles we can store in a table.
#define MAX_HANDLES 256

/// The invalid handle type
#define HANDLE_TYPE_NULL 0

/// The socket handle type
#define HANDLE_TYPE_SOCKET 1

MINIMK_BEGIN_DECLS

/// Function to extract the type from a handle.
static inline uint8_t handle_type(uint64_t handle) MINIMK_NOEXCEPT {
    return static_cast<uint8_t>((handle & 0xff00000000000000) >> 56);
}

/// Function to extract the generation from a handle.
static inline uint64_t handle_generation(uint64_t handle) MINIMK_NOEXCEPT {
    return (handle & 0x00ffffffffffff00) >> 8;
}

/// Function to extract the index from a handle.
static inline uint64_t handle_index(uint64_t handle) MINIMK_NOEXCEPT {
    return (handle & 0x00000000000000ff);
}

/// Function returning whether a given generation value is valid.
static inline int handle_generation_valid(uint64_t generation) MINIMK_NOEXCEPT {
    return (generation & 0xffff000000000000) == 0;
}

/// Function to create a handle given type, generation, and index.
static inline uint64_t make_handle(uint8_t type, uint64_t generation,
                                   uint8_t index) MINIMK_NOEXCEPT {
    uint64_t handle = 0;

    // Add the handle type
    handle |= (static_cast<uint64_t>(type)) << 56;

    // Add the handle generation
    MINIMK_ASSERT(handle_generation_valid(generation));
    handle |= generation << 8;

    // Add the handle index
    handle |= static_cast<uint64_t>(index);

    return handle;
}

MINIMK_END_DECLS

#endif // LIBMINIMK_RUNTIME_HANDLE_HPP
