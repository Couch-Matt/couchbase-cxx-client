/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *   Copyright 2020-Present Couchbase, Inc.
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

#include <couchbase/common_options.hxx>
#include <couchbase/get_replica_result.hxx>
#include <couchbase/key_value_error_context.hxx>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace couchbase
{
/**
 * Options for @ref collection#get_all_replicas().
 *
 * @since 1.0.0
 * @committed
 */
struct get_all_replicas_options : public common_options<get_all_replicas_options> {
    /**
     * Immutable value object representing consistent options.
     *
     * @since 1.0.0
     * @internal
     */
    struct built : public common_options<get_all_replicas_options>::built {
    };

    /**
     * Validates options and returns them as an immutable value.
     *
     * @return consistent options as an immutable value
     *
     * @exception std::system_error with code errc::common::invalid_argument if the options are not valid
     *
     * @since 1.0.0
     * @internal
     */
    [[nodiscard]] auto build() const -> built
    {
        return { build_common_options() };
    }
};

/**
 * The result for the @ref collection#get_all_replicas() operation
 *
 * @since 1.0.0
 * @uncommitted
 */
using get_all_replicas_result = std::vector<get_replica_result>;

/**
 * The signature for the handler of the @ref collection#get_all_replicas() operation
 *
 * @since 1.0.0
 * @uncommitted
 */
using get_all_replicas_handler = std::function<void(couchbase::key_value_error_context, get_all_replicas_result)>;

#ifndef COUCHBASE_CXX_CLIENT_DOXYGEN
namespace core
{
class cluster;
namespace impl
{

/**
 * @since 1.0.0
 * @internal
 */
void
initiate_get_all_replicas_operation(std::shared_ptr<couchbase::core::cluster> core,
                                    const std::string& bucket_name,
                                    const std::string& scope_name,
                                    const std::string& collection_name,
                                    std::string document_key,
                                    get_all_replicas_options::built options,
                                    get_all_replicas_handler&& handler);
#endif
} // namespace impl
} // namespace core
} // namespace couchbase