## Linux Profiling ##

### Motivation: When + Why of Profiling ###

Real-time programs such as simulations and games require careful optimization in
order to maintain smooth frame rates. While premature optimization is of course to
be avoided, it's useful to tackle in later stages of development in order to ensure
that a simulation avoids stalling even during compute-intensive stages of processing.
This is where profiling comes in handy; it allows developers to more closely inspect
the inner workings of their application binaries to trace the causes of slowdowns and
(consequently) identify potential places of improvement.

The importance of code profiling has resulted in an abundance of options when it
comes to profiling tools. While options like [`Visual Studio Code`][vsc] provide comprehensive
suites of features including integrated development interfaces, lighter-weight options
such as [`perf`][prf] are often favorable for Linux developers due to their low overhead and
flexibility. The following section will detail instructions for installing and using `perf`
in order to perform profiling on Linux-deployed executables.

### Process: Installing + Configuring on Ubuntu ###

In order to use `perf`, it must first be installed alongside its dependent libraries.
This can be accomplished on Ubuntu with the following installation commands:

```
sudo apt-get install linux-tools-common linux-tools-generic
```

Once these binaries have been installed, they need to be configured to establish
proper permissions when collecting performance data. For most cases, this can be
accomplished by running the following command:

```
sudo sysctl -w kernel.perf_event_paranoid=1
```

After this configuration has been performed, `perf` can be executed to perform
simple profiling by running the following on a given binary `b`:

```
perf record b
perf report
```

More information on running `perf` and evaluating its outputs can be found on
[Evan Wilde's Perf article](https://dev.to/etcwilde/perf---perfect-profiling-of-cc-on-linux-of).

### Resources ###

The information for this listing was pulled from the following resources:

- [Stack Overflow: Run 'perf' w/o Root Permissions](https://superuser.com/a/980757)
- [Perf - Perfect Profiling of C/C++ on Linux](https://dev.to/etcwilde/perf---perfect-profiling-of-cc-on-linux-of)
- [Wikipedia: List of Performance Analysis Tools](https://en.wikipedia.org/wiki/List_of_performance_analysis_tools)


[vsc]: https://code.visualstudio.com "Visual Studio Code"
[prf]: https://en.wikipedia.org/wiki/Perf_(Linux) "Perf"
