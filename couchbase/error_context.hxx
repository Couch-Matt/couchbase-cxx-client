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

#include <couchbase/retry_reason.hxx>

#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <system_error>

namespace couchbase
{
/**
 * The error_context is the parent interface for all service-specific error contexts that are returned by operation handler.
 *
 * @since 1.0.0
 * @committed
 */
class error_context
{
  public:
    /**
     * Creates empty error context.
     *
     * @since 1.0.0
     * @committed
     */
    error_context() = default;

    /**
     * Creates and initializes error context with given parameters.
     *
     * @param ec
     * @param last_dispatched_to
     * @param last_dispatched_from
     * @param retry_attempts
     * @param retry_reasons
     *
     * @since 1.0.0
     * @internal
     */
    error_context(std::error_code ec,
                  std::optional<std::string> last_dispatched_to,
                  std::optional<std::string> last_dispatched_from,
                  int retry_attempts,
                  std::set<retry_reason> retry_reasons)
      : ec_{ ec }
      , last_dispatched_to_{ std::move(last_dispatched_to) }
      , last_dispatched_from_{ std::move(last_dispatched_from) }
      , retry_attempts_{ retry_attempts }
      , retry_reasons_{ std::move(retry_reasons) }
    {
    }

    /**
     * Returns status of the operation.
     *
     * @return error code or false-like value for success
     *
     * @since 1.0.0
     * @committed
     */
    [[nodiscard]] auto ec() const -> std::error_code
    {
        return ec_;
    }

    /**
     * Override error code.
     *
     * @deprecated extract error code from the error context or refactor to avoid mutable object
     *
     * @param ec
     * @volatile
     */
    void override_ec(std::error_code ec)
    {
        ec_ = ec;
    }

    /**
     * The hostname/ip where this request got last dispatched to.
     *
     * @return address encoded as a string
     *
     * @since 1.0.0
     * @committed
     */
    [[nodiscard]] auto last_dispatched_to() const -> const std::optional<std::string>&
    {
        return last_dispatched_to_;
    }

    /**
     * The hostname/ip where this request got last dispatched from.
     *
     * @return address encoded as a string
     *
     * @since 1.0.0
     * @committed
     */
    [[nodiscard]] auto last_dispatched_from() const -> const std::optional<std::string>&
    {
        return last_dispatched_from_;
    }

    /**
     * The number of times the attached request has been retried.
     *
     * @return number of retries.
     *
     * @since 1.0.0
     * @committed
     */
    [[nodiscard]] auto retry_attempts() const -> int
    {
        return retry_attempts_;
    }

    /**
     * Set of reasons recorded during retrying the operation.
     *
     * @return set of reasons (empty if the operation was not retried)
     *
     * @since 1.0.0
     * @committed
     */
    [[nodiscard]] auto retry_reasons() const -> const std::set<retry_reason>&
    {
        return retry_reasons_;
    }

    /**
     * Check if the operation was retried because of given reason.
     *
     * @param reason reason to check
     * @return true if the reason exists in set of recorded reasons.
     *
     * @since 1.0.0
     * @uncommitted
     */
    [[nodiscard]] auto retried_because_of(retry_reason reason) const -> bool
    {
        return retry_reasons_.count(reason) > 0;
    }

  private:
    std::error_code ec_{};
    std::optional<std::string> last_dispatched_to_{};
    std::optional<std::string> last_dispatched_from_{};
    int retry_attempts_{ 0 };
    std::set<retry_reason> retry_reasons_{};
};
} // namespace couchbase
