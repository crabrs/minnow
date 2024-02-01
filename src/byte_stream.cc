#include "byte_stream.hh"
#include <algorithm>
#include <cstdint>
#include <string_view>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return is_closed_;
}

void Writer::push( string data )
{
  // Your code here.
  uint64_t n_pushed = std::min(data.size(), capacity_ - buffer_.size());
  buffer_.append(data, 0, n_pushed);
  bytes_pushed_ += n_pushed;
  return;
}

void Writer::close()
{
  // Your code here.
  is_closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return is_closed_ && buffer_.empty();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_;
}

string_view Reader::peek() const
{
  // Your code here.
  return std::string_view{buffer_.cbegin(), buffer_.cend()};
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  uint64_t n_poped = std::min(buffer_.size(), len);
  buffer_.erase(0, n_poped);
  bytes_popped_ += n_poped;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffer_.size();
}
