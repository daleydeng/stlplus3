////////////////////////////////////////////////////////////////////////////////

//   Author:    Andy Rushton
//   Copyright: (c) Southampton University 1999-2004
//              (c) Andy Rushton           2004 onwards
//   License:   BSD License, see ../docs/license.html

////////////////////////////////////////////////////////////////////////////////
#include "string_float.hpp"
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

namespace stlplus
{

  // added as a local copy to break the dependency on the portability library
  static int local_vdprintf(std::string& formatted, const char* format, va_list args)
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

  static int local_dprintf(std::string& formatted, const char* format, ...)
  {
    va_list args;
    va_start(args, format);
    int result = local_vdprintf(formatted, format, args);
    va_end(args);
    return result;
  }

  static std::string local_vdformat(const char* format, va_list args) throw(std::invalid_argument)
  {
    std::string formatted;
    int length = local_vdprintf(formatted, format, args);
    if (length < 0) throw std::invalid_argument("dprintf");
    return formatted;
  }

  static std::string local_dformat(const char* format, ...) throw(std::invalid_argument)
  {
    std::string formatted;
    va_list args;
    va_start(args, format);
    int length = local_vdprintf(formatted, format, args);
    va_end(args);
    if (length < 0) throw std::invalid_argument("dprintf");
    return formatted;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // floating-point types

  std::string float_to_string(float f, real_display_t display, unsigned width, unsigned precision)
    throw(std::invalid_argument)
  {
    return double_to_string((double)f, display, width, precision);
  }

  std::string double_to_string(double f, real_display_t display, unsigned width, unsigned precision)
    throw(std::invalid_argument)
  {
    switch(display)
    {
    case display_fixed:
      return local_dformat("%*.*f", width, precision, f);
    case display_floating:
      return local_dformat("%*.*e", width, precision, f);
    case display_mixed:
      return local_dformat("%*.*g", width, precision, f);
    default:
      throw std::invalid_argument("invalid radix display value");
    }
  }

  ////////////////////////////////////////////////////////////////////////////////

  float string_to_float(const std::string& value)
    throw(std::invalid_argument)
  {
    return (float)string_to_double(value);
  }

  double string_to_double(const std::string& value)
    throw(std::invalid_argument)
  {
    // TODO - error checking
    return strtod(value.c_str(), 0);
  }

  ////////////////////////////////////////////////////////////////////////////////

} // end namespace stlplus
