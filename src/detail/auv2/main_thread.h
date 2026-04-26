#pragma once

#include <dispatch/dispatch.h>
#include <pthread.h>

#include <optional>
#include <type_traits>
#include <utility>

namespace Clap::AUv2
{

template <typename Fn>
auto invokeOnMainThreadSync(Fn &&fn) -> std::invoke_result_t<Fn>
{
  using Result = std::invoke_result_t<Fn>;

  if (pthread_main_np() != 0)
  {
    return std::forward<Fn>(fn)();
  }

  auto work = std::forward<Fn>(fn);

  if constexpr (std::is_void_v<Result>)
  {
    dispatch_sync_f(
        dispatch_get_main_queue(), &work,
        [](void *ctx)
        {
          auto *callable = static_cast<std::remove_reference_t<decltype(work)> *>(ctx);
          (*callable)();
        });
  }
  else
  {
    std::optional<Result> result;
    struct Context
    {
      std::remove_reference_t<decltype(work)> *callable;
      std::optional<Result> *result;
    } context{&work, &result};

    dispatch_sync_f(
        dispatch_get_main_queue(), &context,
        [](void *ctx)
        {
          auto *context = static_cast<Context *>(ctx);
          context->result->emplace((*context->callable)());
        });

    return *result;
  }
}

}  // namespace Clap::AUv2
