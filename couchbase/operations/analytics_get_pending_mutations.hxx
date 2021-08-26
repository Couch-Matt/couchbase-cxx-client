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

#include <tao/json.hpp>

#include <error_context/http.hxx>
#include <version.hxx>

namespace couchbase::operations
{
struct analytics_get_pending_mutations_response {
    struct problem {
        std::uint32_t code;
        std::string message;
    };

    error_context::http ctx;
    std::string status{};
    std::vector<problem> errors{};
    std::map<std::string, std::uint64_t> stats{};
};

struct analytics_get_pending_mutations_request {
    using response_type = analytics_get_pending_mutations_response;
    using encoded_request_type = io::http_request;
    using encoded_response_type = io::http_response;
    using error_context_type = error_context::http;

    static const inline service_type type = service_type::analytics;

    std::string client_context_id{ uuid::to_string(uuid::random()) };
    std::chrono::milliseconds timeout{ timeout_defaults::management_timeout };

    [[nodiscard]] std::error_code encode_to(encoded_request_type& encoded, http_context& /* context */) const
    {
        encoded.method = "GET";
        encoded.path = "/analytics/node/agg/stats/remaining";
        return {};
    }
};

analytics_get_pending_mutations_response
make_response(error_context::http&& ctx,
              const analytics_get_pending_mutations_request& /* request */,
              analytics_get_pending_mutations_request::encoded_response_type&& encoded)
{
    analytics_get_pending_mutations_response response{ std::move(ctx) };
    if (!response.ctx.ec) {
        tao::json::value payload{};
        try {
            payload = tao::json::from_string(encoded.body);
        } catch (const tao::pegtl::parse_error& e) {
            response.ctx.ec = error::common_errc::parsing_failure;
            return response;
        }
        if (encoded.status_code == 200) {
            if (payload.is_object()) {
                for (const auto& [dataverse, entry] : payload.get_object()) {
                    for (const auto& [dataset, counter] : entry.get_object()) {
                        response.stats.try_emplace(fmt::format("{}.{}", dataverse, dataset), counter.get_unsigned());
                    }
                }
            }
            return response;
        }
        auto* errors = payload.find("errors");
        if (errors != nullptr && errors->is_array()) {
            for (const auto& error : errors->get_array()) {
                analytics_get_pending_mutations_response::problem err{
                    error.at("code").as<std::uint32_t>(),
                    error.at("msg").get_string(),
                };
                response.errors.emplace_back(err);
            }
        }
        response.ctx.ec = error::common_errc::internal_server_failure;
    }
    return response;
}

} // namespace couchbase::operations