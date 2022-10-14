//
// Created by tammd on 10/10/22.
//

#ifndef DTL_THREAD_POOL_HPP
#define DTL_THREAD_POOL_HPP

#include <thread>
#include <vector>
#include <condition_variable>
#include <list>
#include <iostream>
#include <state.hpp>
#include <atomic>

namespace http10 {
    namespace server {
        class thread_pool {

        public:
            explicit thread_pool(const size_t num_thread, bool is_detach = false) : num_thread_(num_thread)
            {
                create_thread();
            }

            void assign(std::shared_ptr<connection> conn)
            {
                if(conn->get_thread_idx() == -1) conn->set_thread_idx(count++ % num_thread_);
                auto idx = conn->get_thread_idx();
                auto& info = thread_info_[idx];
                //std::lock_guard guard(info->mt_);
                std::cout << "distribute connection " << conn->get_fd() << " in  the thread " << idx << "\n";
                info->conn_.push_back(conn);
                info->state_ = true;
                info->var_.notify_all();
            }

            void stop()
            {
                for(std::shared_ptr<thread_info> info : thread_info_)
                {
                    std::lock_guard guard(info->mt_);
                    info->stop_ = true;
                    info->state_ = true;
                    info->var_.notify_all();
                }
                for(std::thread& t : thread_)
                {
                    t.join();
                }
            }

        private:
            struct thread_info {
                std::mutex mt_;
                std::condition_variable var_;
                std::atomic<bool> state_ = false;
                std::list<std::weak_ptr<connection>> conn_;
                std::atomic<bool> stop_ = false;
                size_t idx = -1;
            };

            void create_thread() noexcept {
                for (size_t i = 0; i < num_thread_; i++) {
                    auto &info = thread_info_.emplace_back(std::make_shared<thread_info>());
                    info->idx = i;
                    auto &th = thread_.emplace_back(std::thread(this->thread_run, info));
                    if (is_detach) th.detach();
                }
            }

            static void thread_run(std::weak_ptr<thread_info> info) {
                using namespace std::chrono_literals;
                std::cout << "thread id " << std::this_thread::get_id() << std::endl;
                for (;;) {
                    if(auto ptr = info.lock(); ptr && !ptr->stop_)
                    {
                        std::unique_lock<std::mutex> guard_(ptr->mt_);
                        while(!ptr->state_) ptr->var_.wait(guard_);
                        if(ptr->stop_)
                        {
                            break;
                        }
                        while(!ptr->conn_.empty())
                        {
                            std::cout << "thread " << ptr->idx << " size " << ptr->conn_.size() << std::endl;
                            auto conn_ptr = ptr->conn_.front(); ptr->conn_.pop_front();
                            if(auto conn = conn_ptr.lock(); conn)
                            {
                                conn->start();
                                std::cout << "thread " << ptr->idx << " processing " << conn->get_fd() << " in " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - conn->get_tp()).count() << "\n";
                            }
                        }
                        ptr->state_ = false;

                    }
                    else
                    {
                        std::cout << "done\n";
                        break;
                    }
                }
                std::cout << "complete thread " << std::this_thread::get_id() << std::endl;
            }

            size_t num_thread_;
            std::vector<std::shared_ptr<thread_info>> thread_info_;
            std::vector<std::thread> thread_;
            bool is_detach;
            int count = 0;
        };
    }
}
#endif //DTL_THREAD_POOL_HPP
