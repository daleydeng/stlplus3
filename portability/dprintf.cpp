////////////////////////////////////////////////////////////////////////////////

//   Author:    Andy Rushton
//   Copyright: (c) Southampton University 1999-2004
//              (c) Andy Rushton           2004 onwards
//   License:   BSD License, see ../docs/license.html
//
//   Notes:
//
//   Original version did a lot of formatting itself, re-inventing the C-runtime
//   functions
//
//   Feb 2007: Rewritten in terms of platform-specific fixes to the
//   buffer-overflow problem. Using native functions for this has the added
//   benefit of giving access to other features of the C-runtime such as Unicode
//   support.
//
//   Jan 2017: Rewritten because a platform-specific fix has been made obsolete,
//   need to rethink the whole approach
//
////////////////////////////////////////////////////////////////////////////////

#include "dprintf.hpp"
#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////

namespace stlplus
{

  int vdprintf(std::string& formatted, const char* format, va_list args)
  {
    // Note: cannot reuse a va_list, need to restart it each time it's used
    // make a copy here - this means the copy is started but not traversed
    va_list copy_args;
    va_copy(copy_args, args);
    // first work out the size of buffer needed to receive the formatted output
    // do this by having a dummy run with a null buffer
    int length = vsnprintf(0, 0, format, args);
    // detect a coding error and give up straight away
    // TODO - error handling? errno may be set and could be made into an exception
    if (length < 0)
    {
      va_end(copy_args);
      return length;
    }

    // allocate a buffer just exactly the right size, adding an extra byto for null termination
    char* buffer = (char*)malloc(length+1);
    if (!buffer)
    {
      va_end(copy_args);
      return -1;
    }

    // now call the print function again to generate the actual formatted string
    int result = vsnprintf(buffer, length+1, format, copy_args);
    va_end(copy_args);
    // TODO - error handling?

    // now append this to the C++ string
    formatted += buffer;
    // recover the buffer memory
    free(buffer);
    return result;
  }

  int dprintf(std::string& formatted, const char* format, ...)
  {
    va_list args;
    va_start(args, format);
    int result = vdprintf(formatted, format, args);
    va_end(args);
    return result;
  }

  std::string vdformat(const char* format, va_list args) throw(std::invalid_argument)
  {
    std::string formatted;
    int length = vdprintf(formatted, format, args);
    if (length < 0) throw std::invalid_argument("dprintf");
    return formatted;
  }

  std::string dformat(const char* format, ...) throw(std::invalid_argument)
  {
    std::string formatted;
    va_list args;
    va_start(args, format);
    int length = vdprintf(formatted, format, args);
    va_end(args);
    if (length < 0) throw std::invalid_argument("dprintf");
    return formatted;
  }

} // end namespace stlplus
