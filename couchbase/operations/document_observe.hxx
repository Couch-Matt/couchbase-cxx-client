/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *   Copyright 2020-2021 Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#pragma once

#include <couchbase/error_context/key_value.hxx>
#include <couchbase/io/mcbp_context.hxx>
#include <couchbase/io/retry_context.hxx>
#include <couchbase/protocol/client_request.hxx>
#include <couchbase/timeout_defaults.hxx>

#include <couchbase/protocol/cmd_exists.hxx>

namespace couchbase::operations
{

struct exists_response {
    enum class observe_status { invalid, found, not_found, persisted, logically_deleted };

    error_context::key_value ctx;
    std::uint16_t partition_id{};
    protocol::cas cas{};
    observe_status status{ observe_status::invalid };

    [[nodiscard]] constexpr bool exists() const
    {
        return status == couchbase::operations::exists_response::observe_status::found ||
               status == couchbase::operations::exists_response::observe_status::persisted;
    }
};

struct exists_request {
    using response_type = exists_response;
    using encoded_request_type = protocol::client_request<protocol::exists_request_body>;
    using encoded_response_type = protocol::client_response<protocol::exists_response_body>;

    document_id id;
    std::uint16_t partition{};
    std::uint32_t opaque{};
    std::chrono::milliseconds timeout{ timeout_defaults::key_value_timeout };
    io::retry_context<io::retry_strategy::best_effort> retries{ false };

    [[nodiscard]] std::error_code encode_to(encoded_request_type& encoded, mcbp_context&& context) const;

    [[nodiscard]] exists_response make_response(error_context::key_value&& ctx, const encoded_response_type& encoded) const;
};

} // namespace couchbase::operations