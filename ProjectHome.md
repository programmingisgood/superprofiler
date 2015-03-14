A C++ library to easily profile your code.

Features:

  1. Simply add a line to the beginning of any function you want to profile and SuperProfiler does the rest for you.
  1. Automatically "mold" to your code path and keep track of average time spent in and total calls to each function, along each code path.
  1. Outputs a list of every function profiled with the amount of time spent executing that function along with the total amount of times the function was called. Also outputs the "call tree" so you can see exactly where time was spent.
  1. Output to different formats after you are done profiling or  create your own output format. Current supported formats:
    1. Text
    1. Comma Separated Values (import into a spreadsheet)
    1. XML
  1. No required dependencies other than the STL.
  1. Unit tests included to automatically test SuperProfiler. (Requires optional http://unittest-cpp.sourceforge.net/ for unit tests)