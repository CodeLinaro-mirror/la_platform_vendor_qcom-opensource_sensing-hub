/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include "qshPb.h"

#include <cstdio>
#include <cstring>  // for std::strlen

namespace qshPb {

    /* -------------------- Encoding -------------------- */

    bool encode_bytes_callback(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
        const auto* buffer = static_cast<const pb_buffer_arg*>(*arg);
        if (!pb_encode_tag_for_field(stream, field)) {
            return false;
        }
        // For nanopb, "bytes" and "string" both use pb_encode_string on raw bytes.
        return pb_encode_string(stream,
                                reinterpret_cast<const pb_byte_t*>(buffer->buf),
                                buffer->buf_len);
    }

    bool encode_cstring_cb(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
        const char* str = static_cast<const char*>(*arg);
        if (str == nullptr) {
            str = "";
        }
        const size_t len = std::strlen(str);

        if (!pb_encode_tag_for_field(stream, field)) {
            return false;
        }
        return pb_encode_string(stream,
                                reinterpret_cast<const pb_byte_t*>(str),
                                len);
    }

    bool encode_nowakeup_msg_ids(pb_ostream_t* stream, const pb_field_t* field, void* const* arg) {
        const auto* msg_ids = static_cast<const std::vector<uint32_t>*>(*arg);
        for (uint32_t id : *msg_ids) {
            if (!pb_encode_tag_for_field(stream, field)) {
            return false;
            }
            if (!pb_encode_fixed32(stream, &id)) {
            return false;
            }
        }
        return true;
    }

    bool encode_string_cb(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
        if (!stream || !field || !arg || !*arg) return false;
        const auto* s = static_cast<const pb_buffer_arg*>(*arg);
        if (!pb_encode_tag_for_field(stream, field)) return false;
        if (!pb_encode_varint(stream, s->buf_len))     return false;
        if (s->buf_len > 0 && !pb_write(stream, reinterpret_cast<const pb_byte_t*>(s->buf), s->buf_len)) 
            return false;
    return true;
}

    /* -------------------- Decoding -------------------- */

    bool decode_string(pb_istream_t* stream, const pb_field_t* /*field*/, void** arg) {
        auto* out_str = static_cast<std::string*>(*arg);

        // Read the remaining bytes in the stream for this field.
        const size_t len = stream->bytes_left;
        if (len == 0) {
            out_str->clear();
            return true;
        }
        std::string tmp;
        tmp.resize(len);

        if (!pb_read(stream, reinterpret_cast<pb_byte_t*>(&tmp[0]), len)) {
            return false;
        }
        *out_str = std::move(tmp);
        return true;
    }

    bool decode_payload(pb_istream_t* stream, const pb_field_t* /*field*/, void** arg) {
        auto* data = static_cast<pb_buffer_arg*>(*arg);
        data->buf_len = stream->bytes_left;
        data->buf = stream->state;
        // Advance the stream by consuming the remaining bytes.
        return pb_read(stream, nullptr, stream->bytes_left);
    }

    bool decode_suid(pb_istream_t* stream, const pb_field_t* /*field*/, void** arg) {
        sns_std_suid uid = sns_std_suid_init_default;
        auto* suid_list = static_cast<std::vector<sns_std_suid>*>(*arg);

        if (!pb_decode_noinit(stream, sns_std_suid_fields, &uid)) {
            return false;
        }

        suid_list->push_back(uid);
        return true;
    }

    bool decode_float_array(pb_istream_t* stream, const pb_field_t* /*field*/, void** arg) {
        auto* vec = static_cast<std::vector<float>*>(*arg);
        float value;
        while (stream->bytes_left > 0) {
            if (!pb_decode_fixed32(stream, &value)) {
            return false;
            }
            vec->push_back(value);
        }
        return true;
    }

    bool decode_repeated_enum(pb_istream_t *stream,
                            const pb_field_t * /*field*/,
                            void **arg) {
        auto *ctx = static_cast<std::vector<uint32_t> *>(*arg);
        if (!ctx) {
            return false;
        }
        while (stream->bytes_left > 0) {
            uint64_t raw = 0;
            if (!pb_decode_varint(stream, &raw)) {
            return false; // Propagate decode error.
            }
            ctx->push_back(static_cast<uint32_t>(raw));
        }
        return true;
    }


    /* -------------------- Utilities -------------------- */

    void printHex(const uint8_t* data, size_t size, const char* title) {
        for (size_t i = 0; i < size; ++i) {
            std::printf("%02X ", data[i]);
            if ((i + 1) % 16 == 0) {
            std::printf("\n");
            }
        }
    }
}

