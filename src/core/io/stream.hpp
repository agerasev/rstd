#pragma once

#include <cstdint>
#include <span>

#include "error.hpp"
#include <core/result.hpp>

namespace core::io {

class Read {
public:
    //! Try to read at most `len` bytes into `data` buffer.
    //! @return Number of bytes read or error.
    virtual Result<size_t, Error> read(std::span<uint8_t> data) = 0;

    //! Try to read exactly `len` bytes into `data` buffer or return error.
    virtual Result<std::monostate, Error> read_exact(std::span<uint8_t> data);

    //! Repeatedly calls `read` until all data is read.
    virtual Result<std::monostate, Error> read_all(std::span<uint8_t> data);
};

class Write {
public:
    //! Try to write at most `len` bytes from `data` buffer.
    //! @return Number of bytes written or error.
    virtual Result<size_t, Error> write(std::span<const uint8_t> data) = 0;

    //! Try to write exactly `len` bytes from `data` buffer or return error.
    virtual Result<std::monostate, Error> write_exact(std::span<const uint8_t> data);

    //! Repeatedly calls `write` until all data is written.
    virtual Result<std::monostate, Error> write_all(std::span<const uint8_t> data);
};


} // namespace core::io
