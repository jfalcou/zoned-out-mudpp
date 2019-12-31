#include <mudpp/system/session_manager.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <cstdint>
#include <memory>

namespace mudpp
{
  session_manager::session_manager(boost::asio::io_service& ios, uint16_t port)
    : io_service(ios)
    , acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , event(ios)
  {
    event.setup ( [this]()
                  {
                    // Cleanup old sessions
                    sessions.erase( std::remove_if( sessions.begin(), sessions.end()
                                                  , [](auto const& s) { return !s->is_valid(); }
                                                  )
                                  , sessions.end()
                                  );

                    std::cout << sessions.size()-1 << " alive sessions." << std::endl;
                  }
                , boost::asio::chrono::seconds(1)
                );

    start_accept();
  }

  void session_manager::start_accept()
  {
    // Keep session alive in our list of sessions
    sessions.push_back(std::make_unique<session>(io_service));

    // Set acceptor function
    acceptor.async_accept ( sessions.back()->socket(),
                            [this](auto const& error)
                            {
                              if (!error)
                              {
                                std::cout << "Starting session:" << std::endl;
                                sessions.back()->start();
                              }
                              else
                              {
                                std::cout << "session_manager::handle_accept error: " << error << std::endl;
                              }

                              start_accept();
                            }
                          );
  }
}
