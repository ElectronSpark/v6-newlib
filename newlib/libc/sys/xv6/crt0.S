/**
 * @file crt0.S
 * @brief C runtime startup for newlib on xv6
 *
 * This is the entry point for user programs compiled with newlib.
 * It sets up the stack, calls constructors, then main(), then exit().
 */

.section .text
.global _start
.type _start, @function

_start:
    # Clear the frame pointer register (s0/fp) for stack traces
    mv      fp, zero
    
    # The kernel sets up:
    #   a0 = argc
    #   a1 = argv
    # Stack is already set up by the kernel
    
    # Save argc and argv for later
    mv      s0, a0          # s0 = argc
    mv      s1, a1          # s1 = argv
    
    # Call any constructors (for C++ or __attribute__((constructor)))
    # newlib provides __libc_init_array
    # call    __libc_init_array
    
    # Set up arguments for main(argc, argv)
    mv      a0, s0          # a0 = argc
    mv      a1, s1          # a1 = argv
    
    # Call main
    call    main
    
    # main() returned, call exit with return value
    # a0 already contains main's return value
    call    exit
    
    # Should never reach here, but just in case
1:  j       1b

.size _start, .-_start
