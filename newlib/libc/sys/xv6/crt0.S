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
    # Initialize global pointer (gp) for gp-relative addressing.
    # __global_pointer$ is defined by the linker script.
    # Use weak reference so this works even if the linker script
    # doesn't define it (e.g. user.ld for simple xv6 programs).
    .weak   __global_pointer$
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    # Clear the frame pointer register (fp = s0)
    mv      fp, zero

    # Save argc and argv in callee-saved regs
    mv      s0, a0          # s0 = argc
    mv      s1, a1          # s1 = argv

    # NOTE: BSS is already zeroed by the kernel's exec() which allocates
    # zero-filled pages for the BSS segment. No memset needed.
    
    # The kernel sets up:
    #   a0 = argc
    #   a1 = argv
    #   envp = &argv[argc + 1]  (on the stack, after argv NULL terminator)
    # Stack is already set up by the kernel
    
    # Compute envp = &argv[argc + 1] and store in global __xv6_envp
    addi    t0, s0, 1       # t0 = argc + 1
    slli    t0, t0, 3       # t0 = (argc + 1) * 8
    add     t0, s1, t0      # t0 = argv + (argc + 1) = &argv[argc + 1] = envp
    la      t1, __xv6_envp
    sd      t0, 0(t1)       # __xv6_envp = envp
    
    # Call constructors (__attribute__((constructor)), C++ static init, etc.)
    # Required for CPython and any code using constructor attributes.
    call    __libc_init_array
    
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

.section .bss
.global __xv6_envp
.align 3
__xv6_envp:
    .quad 0
