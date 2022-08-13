#pragma once

#include "stream.hpp"

namespace core::io {

Result<std::monostate, Error> Read::read_exact(std::span<uint8_t> data) {
    auto res = this->read(data);
    if (res.is_ok()) {
        if (res.ok() == data.size()) {
            return Ok(std::monostate);
        } else {
            return Err(Error(ErrorKind::UnexpectedEof));
        }
    } else {
        return Err(std::move(res.err()));
    }
}

Result<std::monostate, Error> Read::read_all(std::span<uint8_t> data) {
    size_t total_len = 0;
    while (true) {
        auto res = this->read(data);
        if (res.is_ok()) {
            size_t len = res.ok();
            total_len += len;
            data = std::span(data.data() + len, data.size() - len);
            if (total_len == data.size()) {
                return Ok(std::monostate);
            }
        } else {
            return Err(std::move(res.err()));
        }
    }
}

Result<std::monostate, Error> Write::write_exact(std::span<const uint8_t> data) {
    auto res = this->write(data);
    if (res.is_ok()) {
        if (res.ok() == data.size()) {
            return Ok(std::monostate);
        } else {
            return Err(Error(ErrorKind::UnexpectedEof));
        }
    } else {
        return Err(std::move(res.err()));
    }
}

Result<std::monostate, Error> Write::write_all(std::span<const uint8_t> data) {
    size_t total_len = 0;
    while (true) {
        auto res = this->write(data);
        if (res.is_ok()) {
            size_t len = res.ok();
            total_len += len;
            data = std::span(data.data() + len, data.size() - len);
            if (total_len == data.size()) {
                return Ok(std::monostate);
            }
        } else {
            return Err(std::move(res.err()));
        }
    }
}

} // namespace core::io
