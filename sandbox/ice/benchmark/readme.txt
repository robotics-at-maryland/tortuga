Method Invocation Benchmark for ICE in C++

This package measures the speed of method invocation in ICE on the local host
and compares it with native method invocation in C++.  A trivial method is invoked
four different ways: by direct method call, by smart pointer method call, by
ICE proxy invocation, and by idempotent ICE proxy invocation.

Type `scons' at the command line to build the benchmark program with
optimization levels -O0 through -O3.  To run, type (for example)

# build/O0/bench
Direct invoke: 0.317252       1
Handle invoke: 0.348929 1.09985
Proxy invoke:   2.35869 7.43474
Idemp. invoke:  2.36859 7.46595

The results above suggest that ICE method invocation incurs around 7.5 times the
overhead as native method invocation.  Its performance level is substantially
higher than Python's, but not as high as Java's or C++'s.

These numbers were recorded on 2.16 GHz Intel Core 2 Duo MacBook Pro.

Longer run times, but almost identical run time ratios,
have been observed on a single core 1.25 GHz PowerMac G4.

(see: http://blog.dhananjaynene.com/2008/07/performance-comparison-c-java-python-ruby-jython-jruby-groovy/)

Note that this measures only time overhead.  ICE invocation may make use of other
system resources, including memory, threads, and sockets.
