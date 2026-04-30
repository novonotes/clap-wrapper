#pragma once

#include "detail/shared/main_thread.h"

namespace Clap::AAX
{

template <typename Fn>
auto invokeOnMainThreadSync(Fn &&fn) -> std::invoke_result_t<Fn>
{
  return Clap::invokeOnMainThreadSync(std::forward<Fn>(fn));
}

}  // namespace Clap::AAX
