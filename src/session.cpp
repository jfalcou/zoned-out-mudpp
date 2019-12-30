#include <mudpp/io/session_manager.hpp>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

namespace mudpp
{
  void session_manager::bailout (int sig)
  {
    std::cout << "**** Terminated by player on signal " << sig << " ****"
              << std::endl << std::endl;
    //bStopNow = true;
  }

  void session_manager::handle_connections()
  {
    // get ready for "select" function ...
    FD_ZERO (&in_set);
    FD_ZERO (&out_set);
    FD_ZERO (&exc_set);

    // add our control socket (for new connections)
    FD_SET (control_socket_, &in_set);

    int iMaxdesc = control_socket_;
    for(auto& c : connections)
    {
      if (c->is_active())
      {
        iMaxdesc = std::max(iMaxdesc, c->socket());

        // don't take input if they are closing down
        if (!c->is_closing())
        {
          FD_SET( c->socket(), &in_set  );
          FD_SET( c->socket(), &exc_set );
        }

        /* we are only interested in writing to sockets we have something for */
        if (c->is_pending())
          FD_SET( c->socket(), &out_set );
      }
    }

    // set up timeout interval
    timeval timeout;
    timeout.tv_sec  = 0;      // seconds
    timeout.tv_usec = 500000; // + 1000th. of second

    // check for activity, timeout after 'timeout' seconds
    if (select (iMaxdesc + 1, &in_set, &out_set, &exc_set, &timeout) > 0)
    {
      // New connection on control port?
      if( FD_ISSET (control_socket_, &in_set) )
      {
        auto c = process_connection();
        if(c) connections.push_back(std::move(c));

        // handle all player input/output
        for(auto& c : connections)
        {
          if (c->is_active())
          {
            if(FD_ISSET(c->socket(), &exc_set ) ) c->handle_errors();
            if(FD_ISSET(c->socket(), &in_set  ) ) c->read();
            if(FD_ISSET(c->socket(), &out_set ) ) c->write();
          }
        }
      }
    } // end of something happened
  }

  connection_t session_manager::process_connection()
  {
    int s;    /* incoming socket */
    socklen_t sa_len = sizeof sa;

    /* loop until all outstanding connections are accepted */
    while (true)
    {
      s = accept( control_socket_, (sockaddr*)(&sa), &sa_len);

      /* a bad socket probably means no more connections are outstanding */
      if (s == -1)
      {
        /* blocking is OK - we have accepted all outstanding connections */
        if(errno == EWOULDBLOCK ) return nullptr;
        std::cerr << "Error in accept" << std::endl;
        return nullptr;
      }

    /* here on successful accept - make sure socket doesn't block */
    if (fcntl (s, F_SETFL, FNDELAY) == -1)
    {
      std::cerr << "fcntl on player socket" << std::endl;
      return nullptr;
    }

    std::string address = inet_ntoa ( sa.sin_addr);
    int port = ntohs (sa.sin_port);

    // immediately close connections from blocked IP addresses
    /*
    if (blockedIP.find (address) != blockedIP.end ())
    {
      std::cerr << "Rejected connection from " << address << std::endl;
      close(s);
      continue;
    }
    */

      std::cout << "New player accepted on socket " << s << ", from address " << address
                << ", port " << port
                << std::endl;

      // *p << "\nWelcome to the Tiny MUD Server version " << VERSION << "\n";
      // *p << messagemap ["welcome"];   // message from message file
      // *p << p->prompt;    // initial prompt (Enter your name ...)

      return connection::make(s,port,address);
    }
  }

  session_manager::~session_manager()
  {
    std::cerr << "Closing all comms connections." << std::endl;

    // close listening socket
    if (control_socket_ != -1) close (control_socket_);
  }

  session_manager::session_manager(int port) : control_socket_(-1), last_reception_(0)
  {
    try
    {
      // Create the control socket
      if ( (control_socket_ = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error ("creating control socket");

      // make sure socket doesn't block
      if (fcntl( control_socket_, F_SETFL, FNDELAY ) == -1)
        throw std::runtime_error ("fcntl on control socket");

      linger ld = linger ();  // zero it

      // Don't allow closed sockets to linger
      if (setsockopt( control_socket_, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof ld ) == -1)
        throw std::runtime_error ("setsockopt (SO_LINGER)");

      int x = 1;

      // Allow address reuse
      if (setsockopt( control_socket_, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof x ) == -1)
        throw std::runtime_error ("setsockopt (SO_REUSEADDR)");

      sa.sin_family       = AF_INET;
      sa.sin_port         = htons (port);
      sa.sin_addr.s_addr  = INADDR_ANY;   /* change to listen on a specific adapter */

      // bind the socket to our connection port
      if ( bind (control_socket_, (struct sockaddr *) &sa, sizeof sa) == -1)
        throw std::runtime_error ("bind");

      // listen for connections
      if (listen (control_socket_, SOMAXCONN) == -1)   // SOMAXCONN is the backlog count
        throw std::runtime_error ("listen");

      last_reception_ = time (NULL);

      std::cerr << "Opened sessions ..." << std::endl;
    }
    catch (std::runtime_error & e)
    {
      std::cerr << "Cannot initialise comms ..." << std::endl;
      perror (e.what ());
    }

    // standard termination signals
    // signal (SIGINT,  bailout);
    // signal (SIGTERM, bailout);
    // signal (SIGHUP,  bailout);
  }
}
