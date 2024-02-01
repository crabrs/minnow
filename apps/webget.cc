#include "address.hh"
#include "exception.hh"
#include "socket.hh"

#include <format>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  const Address addr{host, "http"};
  TCPSocket sock;
  try {
    sock.connect(addr);
  } catch (const unix_error& e) {
    cerr << "Connect to addr: " << addr.to_string() << ", failed due to: " << e.what() << endl;
  }
  sock.write(std::format("GET {} HTTP/1.1\r\n", path));
  sock.write(std::format("Host: {}\r\n", host));
  sock.write("Connection: close\r\n");
  sock.write("\r\n");

  
  while (true) {
    std::string buffer;
    sock.read(buffer);
    if (buffer.empty()) {
      break;
    }
    cout << buffer;
  }
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
