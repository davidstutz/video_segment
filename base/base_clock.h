/* 
 * File:   base_clock.h
 * Author: david
 *
 * Created on January 30, 2015, 9:57 AM
 */

#ifndef VIDEO_SEGMENT_BASE_BASE_CLOCK_H
#define	VIDEO_SEGMENT_BASE_BASE_CLOCK_H

#include <stdio.h>
#include <time.h>

/**
 * @file debug.h
 *
 * Some clock counters are available at the compilation unit (source
 * file) level. Each counter can be toggled on/off with
 * BASE_CLOCK_TOGGLE(N), with N between 0 and BASE_CLOCK_NB - 1.
 * Successive on/off toggle of the same counter, even from different
 * functions or different calls to the same function, will add clock
 * time until the counter is reset with BASE_CLOCK_RESET(N). The
 * counter time can be read in seconds (float) with BASE_CLOCK_S(N) and
 * its raw value (long) with BASE_CLOCK(N).
 *
 * Between the two toggles, the counter values are meaningless.
 * BASE_CLOCK_TOGGLE() must be called an even number of times to make
 * sense.
 *
 * These macros use the C89 libc clock() call, they measure the CPU
 * time used. Note that this is the total CPU time, on all CPUs, not
 * the "wall clock" time: a process running on 8 CPUs for 2 seconds
 * uses 16s of CPU time. The CPU time is not affected directly by
 * other programs running on the machine, but there may be some
 * side-effect because of the ressource conflicts like CPU cache reload.
 * These macros are suitable when the measured time is around a second
 * or more.
 *
 * The clock counters are not thread-safe. Your program will not crash
 * and die and burn if you use clock macros in a parallel program,
 * but the numbers may be wrong be if clock macros are called in parallel.
 *
 * Usage:
 *   BASE_CLOCK_RESET(N);
 *   for(i = 0; i < large_number; i++) {
 *     BASE_CLOCK_TOGGLE(N);
 *     some_operations
 *     BASE_CLOCK_TOGGLE(N);
 *     other_operations
 *   }
 *   BASE_PRINTF1("CPU time spent in some_ops: %0.3fs\n", BASE_CLOCK_S(N));
 *
 * @author Nicolas Limare <nicolas.limare@cmla.ens-cachan.fr>
 * @see http://www.ipol.im/](http://www.ipol.im/
 */

/** number of clock counters */
#define BASE_CLOCK_NB 16

/** clock counter array, initialized to 0 (K&R2, p.86) */
static clock_t _base_clock_counter[BASE_CLOCK_NB];

/**
 * @brief reset a CPU clock counter
 */
#define BASE_CLOCK_RESET(N) { _base_clock_counter[N] = 0; }

/**
 * @brief toggle (start/stop) a CPU clock counter
 *
 * To measure the CPU clock time used by an instruction block, call
 * BASE_CLOCK_TOGGLE() before and after the block.
 *
 * The two successive substractions will increase the counter by the
 * difference of the successive CPU clocks. There is no overflow,
 * _base_clock_counter always stays between 0 and 2x the total
 * execution time.
 */
#define BASE_CLOCK_TOGGLE(N) { \
        _base_clock_counter[N] = clock() - _base_clock_counter[N]; }

/**
 * @brief reset and toggle the CPU clock counter
 */
#define BASE_CLOCK_START(N) { \
        BASE_CLOCK_RESET(N); BASE_CLOCK_TOGGLE(N); }

/**
 * @brief CPU clock counter
 */
#define BASE_CLOCK(N) ((long) _base_clock_counter[N])

/**
 * @brief CPU clock time in seconds
 */
#define BASE_CLOCK_S(N) ((float) _base_clock_counter[N] / CLOCKS_PER_SEC)

#endif	/* BASE_CLOCK_H */

