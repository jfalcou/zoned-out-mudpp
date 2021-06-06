//==================================================================================================
/**
  MudPP - MUD engine for C++
  Copyright : MudPP Contributors & Maintainers
  SPDX-License-Identifier: MIT
**/
//==================================================================================================
#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

namespace mudpp::system::network
{
  template<typename T> class safe_queue
  {
    public:
    safe_queue() = default;
    safe_queue(const safe_queue<T>&) = delete;

    using reference       = T&;
    using const_reference = T const&;

    //==============================================================================================
    // Read-only access to queue front & back
    //==============================================================================================
    const_reference front()
    {
      std::scoped_lock lock(operation_mutex_);
      return storage_.front();
    }

    const_reference back()
    {
      std::scoped_lock lock(operation_mutex_);
      return storage_.back();
    }

    //==============================================================================================
    // Removal of queue front & back
    //==============================================================================================
    T pop_front()
    {
      std::scoped_lock lock(operation_mutex_);
      auto that = std::move(storage_.front());
      storage_.pop_front();
      return that;
    }

    T pop_back()
    {
      std::scoped_lock lock(operation_mutex_);
      auto that = std::move(storage_.back());
      storage_.pop_back();
      return that;
    }

    //==============================================================================================
    // Insertion of values into the queue front & back
    //==============================================================================================
    void push_back(const_reference item)
    {
      std::scoped_lock lock(operation_mutex_);
      storage_.emplace_back(std::move(item));

      std::unique_lock<std::mutex> ul{blocking_mutex_};
      blocking_notifier_.notify_one();
    }

    void push_front(const_reference item)
    {
      std::scoped_lock lock(operation_mutex_);
      storage_.emplace_front(std::move(item));

      std::unique_lock<std::mutex> ul(blocking_mutex_);
      blocking_notifier_.notify_one();
    }

    //==============================================================================================
    // Status of the queue
    // TODO: Should they be const and have mutable on the mutex related element ?
    //==============================================================================================
    bool empty()
    {
      std::scoped_lock lock(operation_mutex_);
      return storage_.empty();
    }

    std::size_t count()
    {
      std::scoped_lock lock(operation_mutex_);
      return storage_.size();
    }

    void clear()
    {
      std::scoped_lock lock(operation_mutex_);
      storage_.clear();
    }

    void wait()
    {
      while(empty())
      {
        std::unique_lock<std::mutex> ul(blocking_mutex_);
        blocking_notifier_.wait(ul);
      }
    }

    private:
    std::deque<T>           storage_;
    std::condition_variable blocking_notifier_;
    std::mutex              operation_mutex_;
    std::mutex              blocking_mutex_;
  };
}
