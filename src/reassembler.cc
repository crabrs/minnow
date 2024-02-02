#include "reassembler.hh"
#include "byte_stream.hh"
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <sys/types.h>
#include <utility>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  uint64_t first_unassembled_index = writer().bytes_pushed();
  uint64_t first_unacceptable_index = first_unassembled_index + writer().available_capacity();
  if ( first_index >= first_unacceptable_index ) {
    return;
  }
  if ( first_index > first_unassembled_index ) {
    uint64_t n_available_bytes = writer().available_capacity() - ( first_index - first_unassembled_index );
    if ( n_available_bytes < data.size() ) {
      data.erase( n_available_bytes );
      is_last_substring = false;
    }
    merge_internal( Segment { first_index, data, is_last_substring } );
  } else {
    if ( first_index < first_unassembled_index ) {
      data.erase( 0, first_unassembled_index - first_index );
      first_index = first_unassembled_index;
    }
    bool close = is_last_substring;
    if ( data.size() > writer().available_capacity() ) {
      close = false;
      data.erase( writer().available_capacity() );
    }
    output_.writer().push( std::move( data ) );
    if ( close ) {
      output_.writer().close();
      return;
    }
    try_flush_internal();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  uint64_t n = 0;
  for ( const auto& seg : unassembled_buffer_ ) {
    n += seg.data.size();
  }
  return n;
}

void Reassembler::try_flush_internal()
{
  while ( !unassembled_buffer_.empty() ) {
    auto [first_index, data, is_last_substring] = unassembled_buffer_.front();
    uint64_t first_unassembled_index = writer().bytes_pushed();
    if ( first_index > first_unassembled_index ) {
      break;
    }
    if ( first_index <= first_unassembled_index ) {
      data.erase( 0, first_unassembled_index - first_index );
      output_.writer().push( std::move( data ) );
      unassembled_buffer_.pop_front();
      if ( is_last_substring ) {
        output_.writer().close();
        return;
      }
    }
  }
}
void Reassembler::merge_internal( Segment seg )
{
  auto it = std::upper_bound(
    unassembled_buffer_.begin(), unassembled_buffer_.end(), seg, []( const Segment& lhs, const Segment& rhs ) {
      if ( lhs.first_index == rhs.first_index ) {
        return lhs.data.size() < rhs.data.size();
      }
      return lhs.first_index < rhs.first_index;
    } );
  unassembled_buffer_.insert( it, seg );
  auto current = unassembled_buffer_.begin();
  auto next = std::next( current );
  while ( next != unassembled_buffer_.end() ) {
    if ( current->first_index <= next->first_index
         && next->first_index < current->first_index + current->data.size() ) {
      if ( next->first_index + next->data.size() > current->first_index + current->data.size() ) {
        current->data.append( next->data.begin() + current->first_index + current->data.size() - next->first_index,
                              next->data.end() );
      }
      current->is_last_substring = current->is_last_substring || next->is_last_substring;
      auto next_next = std::next( next );
      unassembled_buffer_.erase( next );
      next = next_next;
    } else {
      current = next;
      next = std::next( current );
    }
  }
}
