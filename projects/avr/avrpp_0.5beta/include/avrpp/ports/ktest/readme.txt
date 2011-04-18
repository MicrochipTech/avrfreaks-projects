The "ktest" port is used when code is built for HOST system (e.g. Linux x86 or
x86_64 with GCC toolchain). No cross-compiling it that case. Though host
version is not suitable for end-users, it is helpful for a developer
who may test the library kernel in a more comfortable environment (e.g. with
normal assertion failure detection).
