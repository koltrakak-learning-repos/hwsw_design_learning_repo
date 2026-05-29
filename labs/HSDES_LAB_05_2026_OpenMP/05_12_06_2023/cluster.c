#include "pmsis.h"
#include "omp.h"
#include "math.h"
#include "perf.h"

#define M  60
#define N 350

PI_L2 int A[M * N];
PI_L2 int B[M * N];
PI_L2 int C[M * N];

/* Stubs — implementations not required for this exercise */
static void update(int *a, int *b) { (void)a; (void)b; }  /* 20000 instructions, sequential */
static void send(int *c)           { (void)c; }            /* 400 instructions, sequential   */

/*
 * [2.2] Parallelize func() using OpenMP directives.
 *
 * ── Dependency analysis ──────────────────────────────────────────────────────
 *
 *   The outer loop (i) carries a dependence:
 *     C[i*N+j] reads C[(i-1)*N+j]  →  row i depends on row i-1.
 *   ∴ The OUTER loop cannot be parallelized.
 *
 *   The inner loop (j) for a fixed i writes only C[i*N+j] and reads
 *   C[(i-1)*N+j], which is fully computed from the PREVIOUS outer iteration.
 *   There is NO dependence between iterations of j.
 *   ∴ The INNER loop CAN be parallelized.
 *
 * ── Design choices ───────────────────────────────────────────────────────────
 *
 *   (a) Hoist #pragma omp parallel OUTSIDE the outer loop.
 *       Opening/closing a parallel region has significant overhead (120/40 instr).
 *       Placing the pragma inside the outer loop would pay this cost M-1=59 times.
 *       Opening it once, before the outer loop, pays it only once.
 *
 *   (b) Place #pragma omp for schedule(static) on the inner loop.
 *       Each inner iteration performs the same amount of work (17 instr), so
 *       static scheduling distributes load evenly with minimal runtime overhead.
 *
 *   (c) The implicit barrier at the end of each #pragma omp for is REQUIRED
 *       for correctness: all cores must finish writing row i before any core
 *       reads row i in the computation of row i+1.
 *
 * ── Data clauses ─────────────────────────────────────────────────────────────
 *
 *   shared(A, B, C)  — global arrays; all cores read/write disjoint elements
 *   i is the outer loop variable, declared in the for — implicitly private
 *   j is the inner loop variable, declared in the for — implicitly private
 *
 * ── Iteration distribution (schedule(static), N=350, 8 cores) ────────────────
 *
 *   base = 350/8 = 43,  rem = 350%8 = 6
 *   cores 0–5 : 44 iterations     core 0: j ∈ [0,   43]
 *                                  core 1: j ∈ [44,  87]
 *                                  core 2: j ∈ [88,  131]
 *                                  core 3: j ∈ [132, 175]
 *                                  core 4: j ∈ [176, 219]
 *                                  core 5: j ∈ [220, 263]
 *   cores 6–7 : 43 iterations     core 6: j ∈ [264, 306]
 *                                  core 7: j ∈ [307, 349]
 *
 * ── [2.3] Parallel speedup (calculation — no code required) ──────────────────
 *
 *   Sequential instructions:
 *     update:       20 000
 *     outer*inner:  (M-1) * N * (5+12) = 59 * 350 * 17 = 350 350
 *     send:            400
 *     Total T_seq = 370 750 instructions
 *
 *   Parallel critical path (worst-case core: 44 inner iters × (M-1) outer iters):
 *     update (seq):        20 000
 *     open parallel:          120
 *     per outer iter:  (M-1) * [ 5*N (loop setup, all cores) +
 *                                44*(5+12) (inner work, worst core) +
 *                                5 (static for overhead per chunk) +
 *                                10 (implicit barrier) ]
 *                    = 59 * [ 1750 + 748 + 15 ] = 59 * 2513 = 148 267
 *     send (seq):             400
 *     close parallel:          40
 *   Speedup = T_seq / T_par  (apply CPI from [2.1] to convert to time)
 *
 * ── [2.4] Energy gain (calculation — no code required) ───────────────────────
 *
 *   Sequential:  P = Puncore + Pactive + 7*Pclock-gating = 2 + 1 + 0.7 = 3.70 mW
 *   Parallel (sequential phases): same as above for update/send durations
 *   Parallel (parallel phase):    P = Puncore + 8*Pactive = 2 + 8 = 10.00 mW
 *   Energy gain = E_seq / E_par  = (P_seq * T_seq) / (P_seq*T_seq_parts + P_par*T_par_part)
 *
 */
void func() {
    update(A, B);

    for (int i = 1; i < M; i++) {
        for (int j = 0; j < N; j++) {  /* Loop setup -> 5 instructions */
            /* Update C[i*N+j] -> 12 instructions */
            C[i * N + j] = 2 * A[i * N + j] + 2 * B[i * N + j] + C[(i - 1) * N + j];
        }
    }

    send(C);
}

void cluster_fn() {
    /* Data initialization (core 0 only — before the parallel region in func) */
    for (int i = 0; i < M * N; i++) {
        A[i] = i;
        B[i] = M * N - i;
        C[i] = 0;
    }

    INIT_STATS();
    ENTER_STATS_LOOP()
        START_STATS()

        func();

        STOP_STATS()
    EXIT_STATS_LOOP()

    if (omp_get_thread_num() == 0)
        printf("C[0]: %d\n", C[0]);
}
