// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UPDATE_ENGINE_PROXY_RESOLVER_H_
#define UPDATE_ENGINE_PROXY_RESOLVER_H_

#include <deque>
#include <string>

#include <base/logging.h>
#include <chromeos/message_loops/message_loop.h>

#include "update_engine/utils.h"

namespace chromeos_update_engine {

extern const char kNoProxy[];

// Callback for a call to GetProxiesForUrl().
// Resultant proxies are in |out_proxy|. Each will be in one of the
// following forms:
// http://<host>[:<port>] - HTTP proxy
// socks{4,5}://<host>[:<port>] - SOCKS4/5 proxy
// kNoProxy - no proxy
typedef void (*ProxiesResolvedFn)(const std::deque<std::string>& proxies,
                                  void* data);

class ProxyResolver {
 public:
  ProxyResolver() {}
  virtual ~ProxyResolver() {}

  // Finds proxies for the given URL and returns them via the callback.
  // |data| will be passed to the callback.
  // Returns true on success.
  virtual bool GetProxiesForUrl(const std::string& url,
                                ProxiesResolvedFn callback,
                                void* data) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(ProxyResolver);
};

// Always says to not use a proxy
class DirectProxyResolver : public ProxyResolver {
 public:
  DirectProxyResolver() = default;
  ~DirectProxyResolver() override;
  bool GetProxiesForUrl(const std::string& url,
                        ProxiesResolvedFn callback,
                        void* data) override;

  // Set the number of direct (non-) proxies to be returned by resolver.
  // The default value is 1; higher numbers are currently used in testing.
  inline void set_num_proxies(size_t num_proxies) {
    num_proxies_ = num_proxies;
  }

 private:
  // The ID of the main loop callback.
  chromeos::MessageLoop::TaskId idle_callback_id_{
      chromeos::MessageLoop::kTaskIdNull};

  // Number of direct proxies to return on resolved list; currently used for
  // testing.
  size_t num_proxies_{1};

  // The MainLoop callback, from here we return to the client.
  void ReturnCallback(ProxiesResolvedFn callback, void* data);
  DISALLOW_COPY_AND_ASSIGN(DirectProxyResolver);
};

}  // namespace chromeos_update_engine

#endif  // UPDATE_ENGINE_PROXY_RESOLVER_H_