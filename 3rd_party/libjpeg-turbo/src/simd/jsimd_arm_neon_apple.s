/* do not compile this code on ARM64 */
#if !defined(__arm64__)
# 1 "src/simd/jsimd_arm_neon.s"
# 1 "<built-in>" 1
# 1 "src/simd/jsimd_arm_neon.s" 2
# 29 "src/simd/jsimd_arm_neon.s"
.text
@.fpu neon
@.arch armv7a
@.object_arch armv4
.arm
# 42 "src/simd/jsimd_arm_neon.s"


# 185 "src/simd/jsimd_arm_neon.s"
.balign 16
jsimd_idct_islow_neon_consts:
    .short (7373)
    .short (4433)
    .short (20995)
    .short ((2446) - (7373))
    .short ((12299) - (7373))
    .short ((16819) - (20995))
    .short ((4433) + (6270))
    .short (9633)

    .short ((9633) - (3196))
    .short ((4433) - (15137))
    .short ((25172) - (20995))
    .short ((9633) - (16069))


    @.func _jsimd_idct_islow_neon
    .globl _jsimd_idct_islow_neon
_jsimd_idct_islow_neon:
# 56 "src/simd/jsimd_arm_neon.s"

    DCT_TABLE .req r0
    COEF_BLOCK .req r1
    OUTPUT_BUF .req r2
    OUTPUT_COL .req r3
    TMP1 .req r0
    TMP2 .req r1
    TMP3 .req r2
    TMP4 .req ip

    ROW0L .req d16
    ROW0R .req d17
    ROW1L .req d18
    ROW1R .req d19
    ROW2L .req d20
    ROW2R .req d21
    ROW3L .req d22
    ROW3R .req d23
    ROW4L .req d24
    ROW4R .req d25
    ROW5L .req d26
    ROW5R .req d27
    ROW6L .req d28
    ROW6R .req d29
    ROW7L .req d30
    ROW7R .req d31
# 242 "src/simd/jsimd_arm_neon.s"
    adr ip, jsimd_idct_islow_neon_consts
    vld1.16 {d16, d17, d18, d19}, [COEF_BLOCK, :128]!
    vld1.16 {d0, d1, d2, d3}, [DCT_TABLE, :128]!
    vld1.16 {d20, d21, d22, d23}, [COEF_BLOCK, :128]!
    vmul.s16 q8, q8, q0
    vld1.16 {d4, d5, d6, d7}, [DCT_TABLE, :128]!
    vmul.s16 q9, q9, q1
    vld1.16 {d24, d25, d26, d27}, [COEF_BLOCK, :128]!
    vmul.s16 q10, q10, q2
    vld1.16 {d0, d1, d2, d3}, [DCT_TABLE, :128]!
    vmul.s16 q11, q11, q3
    vld1.16 {d28, d29, d30, d31}, [COEF_BLOCK, :128]
    vmul.s16 q12, q12, q0
    vld1.16 {d4, d5, d6, d7}, [DCT_TABLE, :128]!
    vmul.s16 q14, q14, q2
    vmul.s16 q13, q13, q1
    vld1.16 {d0, d1, d2, d3}, [ip, :128]
    add ip, ip, #16
    vmul.s16 q15, q15, q3
    vpush {d8-d15}

    vadd.s16 d4, ROW7L, ROW3L
    vadd.s16 d5, ROW5L, ROW1L
    vmull.s16 q6, d4, d2[3]
    vmlal.s16 q6, d5, d1[3]
    vmull.s16 q7, d4, d1[3]

      push {r4, r5}
    vmlal.s16 q7, d5, d2[0]
    vsubl.s16 q3, ROW0L, ROW4L
      ldrd r4, r5, [COEF_BLOCK, #(-96 + 2 * (4 + 1 * 8))]
    vmull.s16 q2, ROW2L, d0[1]
    vmlal.s16 q2, ROW6L, d2[1]
      orr r0, r4, r5
    vmov q4, q6
    vmlsl.s16 q6, ROW5L, d0[2]
      ldrd r4, r5, [COEF_BLOCK, #(-96 + 2 * (4 + 2 * 8))]
    vmlal.s16 q6, ROW3L, d2[2]
    vshl.s32 q3, q3, #13
      orr r0, r0, r4
    vmlsl.s16 q4, ROW1L, d0[0]
      orr r0, r0, r5
    vadd.s32 q1, q3, q2
      ldrd r4, r5, [COEF_BLOCK, #(-96 + 2 * (4 + 3 * 8))]
    vmov q5, q7
    vadd.s32 q1, q1, q6
      orr r0, r0, r4
    vmlsl.s16 q7, ROW7L, d0[0]
      orr r0, r0, r5
    vmlal.s16 q7, ROW1L, d1[0]
    vrshrn.s32 ROW1L, q1, #11
      ldrd r4, r5, [COEF_BLOCK, #(-96 + 2 * (4 + 4 * 8))]
    vsub.s32 q1, q1, q6
    vmlal.s16 q5, ROW5L, d1[1]
      orr r0, r0, r4
    vmlsl.s16 q5, ROW3L, d0[2]
      orr r0, r0, r5
    vsub.s32 q1, q1, q6
    vmull.s16 q6, ROW2L, d1[2]
      ldrd r4, r5, [COEF_BLOCK, #(-96 + 2 * (4 + 5 * 8))]
    vmlal.s16 q6, ROW6L, d0[1]
    vsub.s32 q3, q3, q2
      orr r0, r0, r4
    vrshrn.s32 ROW6L, q1, #11
      orr r0, r0, r5
    vadd.s32 q1, q3, q5
      ldrd r4, r5, [COEF_BLOCK, #(-96 + 2 * (4 + 6 * 8))]
    vsub.s32 q3, q3, q5
    vaddl.s16 q5, ROW0L, ROW4L
      orr r0, r0, r4
    vrshrn.s32 ROW2L, q1, #11
      orr r0, r0, r5
    vrshrn.s32 ROW5L, q3, #11
      ldrd r4, r5, [COEF_BLOCK, #(-96 + 2 * (4 + 7 * 8))]
    vshl.s32 q5, q5, #13
    vmlal.s16 q4, ROW7L, d0[3]
      orr r0, r0, r4
    vadd.s32 q2, q5, q6
      orrs r0, r0, r5
    vsub.s32 q1, q5, q6
    vadd.s32 q6, q2, q7
      ldrd r4, r5, [COEF_BLOCK, #(-96 + 2 * (4 + 0 * 8))]
    vsub.s32 q2, q2, q7
    vadd.s32 q5, q1, q4
      orr r0, r4, r5
    vsub.s32 q3, q1, q4
      pop {r4, r5}
    vrshrn.s32 ROW7L, q2, #11
    vrshrn.s32 ROW3L, q5, #11
    vrshrn.s32 ROW0L, q6, #11
    vrshrn.s32 ROW4L, q3, #11

      beq 3f


    vld1.s16 {d2}, [ip, :64]
    vadd.s16 d10, ROW7R, ROW3R
    vadd.s16 d8, ROW5R, ROW1R

      vtrn.16 ROW6L, ROW7L
    vmull.s16 q6, d10, d2[3]
    vmlal.s16 q6, d8, d1[3]
      vtrn.16 ROW2L, ROW3L
    vmull.s16 q7, d10, d1[3]
    vmlal.s16 q7, d8, d2[0]
      vtrn.16 ROW0L, ROW1L
    vsubl.s16 q3, ROW0R, ROW4R
    vmull.s16 q2, ROW2R, d0[1]
    vmlal.s16 q2, ROW6R, d2[1]
      vtrn.16 ROW4L, ROW5L
    vmov q4, q6
    vmlsl.s16 q6, ROW5R, d0[2]
    vmlal.s16 q6, ROW3R, d2[2]
      vtrn.32 ROW1L, ROW3L
    vshl.s32 q3, q3, #13
    vmlsl.s16 q4, ROW1R, d0[0]
      vtrn.32 ROW4L, ROW6L
    vadd.s32 q1, q3, q2
    vmov q5, q7
    vadd.s32 q1, q1, q6
      vtrn.32 ROW0L, ROW2L
    vmlsl.s16 q7, ROW7R, d0[0]
    vmlal.s16 q7, ROW1R, d1[0]
    vrshrn.s32 ROW1R, q1, #11
      vtrn.32 ROW5L, ROW7L
    vsub.s32 q1, q1, q6
    vmlal.s16 q5, ROW5R, d1[1]
    vmlsl.s16 q5, ROW3R, d0[2]
    vsub.s32 q1, q1, q6
    vmull.s16 q6, ROW2R, d1[2]
    vmlal.s16 q6, ROW6R, d0[1]
    vsub.s32 q3, q3, q2
    vrshrn.s32 ROW6R, q1, #11
    vadd.s32 q1, q3, q5
    vsub.s32 q3, q3, q5
    vaddl.s16 q5, ROW0R, ROW4R
    vrshrn.s32 ROW2R, q1, #11
    vrshrn.s32 ROW5R, q3, #11
    vshl.s32 q5, q5, #13
    vmlal.s16 q4, ROW7R, d0[3]
    vadd.s32 q2, q5, q6
    vsub.s32 q1, q5, q6
    vadd.s32 q6, q2, q7
    vsub.s32 q2, q2, q7
    vadd.s32 q5, q1, q4
    vsub.s32 q3, q1, q4
    vrshrn.s32 ROW7R, q2, #11
    vrshrn.s32 ROW3R, q5, #11
    vrshrn.s32 ROW0R, q6, #11
    vrshrn.s32 ROW4R, q3, #11

    vtrn.16 ROW6R, ROW7R
    vtrn.16 ROW2R, ROW3R
    vtrn.16 ROW0R, ROW1R
    vtrn.16 ROW4R, ROW5R
    vtrn.32 ROW1R, ROW3R
    vtrn.32 ROW4R, ROW6R
    vtrn.32 ROW0R, ROW2R
    vtrn.32 ROW5R, ROW7R

1:
    vld1.s16 {d2}, [ip, :64]
    vmull.s16 q6, ROW1R, d1[3]
    vmlal.s16 q6, ROW1L, d1[3]
    vmlal.s16 q6, ROW3R, d2[3]
    vmlal.s16 q6, ROW3L, d2[3]
    vmull.s16 q7, ROW3R, d1[3]
    vmlal.s16 q7, ROW3L, d1[3]
    vmlal.s16 q7, ROW1R, d2[0]
    vmlal.s16 q7, ROW1L, d2[0]
    vsubl.s16 q3, ROW0L, ROW0R
    vmull.s16 q2, ROW2L, d0[1]
    vmlal.s16 q2, ROW2R, d2[1]
    vmov q4, q6
    vmlsl.s16 q6, ROW1R, d0[2]
    vmlal.s16 q6, ROW3L, d2[2]
    vshl.s32 q3, q3, #13
    vmlsl.s16 q4, ROW1L, d0[0]
    vadd.s32 q1, q3, q2
    vmov q5, q7
    vadd.s32 q1, q1, q6
    vmlsl.s16 q7, ROW3R, d0[0]
    vmlal.s16 q7, ROW1L, d1[0]
    vshrn.s32 ROW1L, q1, #16
    vsub.s32 q1, q1, q6
    vmlal.s16 q5, ROW1R, d1[1]
    vmlsl.s16 q5, ROW3L, d0[2]
    vsub.s32 q1, q1, q6
    vmull.s16 q6, ROW2L, d1[2]
    vmlal.s16 q6, ROW2R, d0[1]
    vsub.s32 q3, q3, q2
    vshrn.s32 ROW2R, q1, #16
    vadd.s32 q1, q3, q5
    vsub.s32 q3, q3, q5
    vaddl.s16 q5, ROW0L, ROW0R
    vshrn.s32 ROW2L, q1, #16
    vshrn.s32 ROW1R, q3, #16
    vshl.s32 q5, q5, #13
    vmlal.s16 q4, ROW3R, d0[3]
    vadd.s32 q2, q5, q6
    vsub.s32 q1, q5, q6
    vadd.s32 q6, q2, q7
    vsub.s32 q2, q2, q7
    vadd.s32 q5, q1, q4
    vsub.s32 q3, q1, q4
    vshrn.s32 ROW3R, q2, #16
    vshrn.s32 ROW3L, q5, #16
    vshrn.s32 ROW0L, q6, #16
    vshrn.s32 ROW0R, q3, #16

    vld1.s16 {d2}, [ip, :64]
    vmull.s16 q6, ROW5R, d1[3]
    vmlal.s16 q6, ROW5L, d1[3]
    vmlal.s16 q6, ROW7R, d2[3]
    vmlal.s16 q6, ROW7L, d2[3]
    vmull.s16 q7, ROW7R, d1[3]
    vmlal.s16 q7, ROW7L, d1[3]
    vmlal.s16 q7, ROW5R, d2[0]
    vmlal.s16 q7, ROW5L, d2[0]
    vsubl.s16 q3, ROW4L, ROW4R
    vmull.s16 q2, ROW6L, d0[1]
    vmlal.s16 q2, ROW6R, d2[1]
    vmov q4, q6
    vmlsl.s16 q6, ROW5R, d0[2]
    vmlal.s16 q6, ROW7L, d2[2]
    vshl.s32 q3, q3, #13
    vmlsl.s16 q4, ROW5L, d0[0]
    vadd.s32 q1, q3, q2
    vmov q5, q7
    vadd.s32 q1, q1, q6
    vmlsl.s16 q7, ROW7R, d0[0]
    vmlal.s16 q7, ROW5L, d1[0]
    vshrn.s32 ROW5L, q1, #16
    vsub.s32 q1, q1, q6
    vmlal.s16 q5, ROW5R, d1[1]
    vmlsl.s16 q5, ROW7L, d0[2]
    vsub.s32 q1, q1, q6
    vmull.s16 q6, ROW6L, d1[2]
    vmlal.s16 q6, ROW6R, d0[1]
    vsub.s32 q3, q3, q2
    vshrn.s32 ROW6R, q1, #16
    vadd.s32 q1, q3, q5
    vsub.s32 q3, q3, q5
    vaddl.s16 q5, ROW4L, ROW4R
    vshrn.s32 ROW6L, q1, #16
    vshrn.s32 ROW5R, q3, #16
    vshl.s32 q5, q5, #13
    vmlal.s16 q4, ROW7R, d0[3]
    vadd.s32 q2, q5, q6
    vsub.s32 q1, q5, q6
    vadd.s32 q6, q2, q7
    vsub.s32 q2, q2, q7
    vadd.s32 q5, q1, q4
    vsub.s32 q3, q1, q4
    vshrn.s32 ROW7R, q2, #16
    vshrn.s32 ROW7L, q5, #16
    vshrn.s32 ROW4L, q6, #16
    vshrn.s32 ROW4R, q3, #16

2:
    vqrshrn.s16 d16, q8, #2
    vqrshrn.s16 d17, q9, #2
    vqrshrn.s16 d18, q10, #2
    vqrshrn.s16 d19, q11, #2
    vpop {d8-d15}
    vqrshrn.s16 d20, q12, #2

      vtrn.16 q8, q9
    vqrshrn.s16 d21, q13, #2
    vqrshrn.s16 d22, q14, #2
      vmov.u8 q0, #(128)
    vqrshrn.s16 d23, q15, #2
      vtrn.8 d16, d17
      vtrn.8 d18, d19
      vadd.u8 q8, q8, q0
      vadd.u8 q9, q9, q0
      vtrn.16 q10, q11

        ldmia OUTPUT_BUF!, {TMP1, TMP2}
        add TMP1, TMP1, OUTPUT_COL
        add TMP2, TMP2, OUTPUT_COL
        vst1.8 {d16}, [TMP1]
      vtrn.8 d20, d21
        vst1.8 {d17}, [TMP2]
        ldmia OUTPUT_BUF!, {TMP1, TMP2}
        add TMP1, TMP1, OUTPUT_COL
        add TMP2, TMP2, OUTPUT_COL
        vst1.8 {d18}, [TMP1]
      vadd.u8 q10, q10, q0
        vst1.8 {d19}, [TMP2]
        ldmia OUTPUT_BUF, {TMP1, TMP2, TMP3, TMP4}
        add TMP1, TMP1, OUTPUT_COL
        add TMP2, TMP2, OUTPUT_COL
        add TMP3, TMP3, OUTPUT_COL
        add TMP4, TMP4, OUTPUT_COL
      vtrn.8 d22, d23
        vst1.8 {d20}, [TMP1]
      vadd.u8 q11, q11, q0
        vst1.8 {d21}, [TMP2]
        vst1.8 {d22}, [TMP3]
        vst1.8 {d23}, [TMP4]
    bx lr

3:


    vtrn.16 ROW6L, ROW7L
    vtrn.16 ROW2L, ROW3L
    vtrn.16 ROW0L, ROW1L
    vtrn.16 ROW4L, ROW5L
    vshl.s16 ROW0R, ROW0R, #2
    vtrn.32 ROW1L, ROW3L
    vtrn.32 ROW4L, ROW6L
    vtrn.32 ROW0L, ROW2L
    vtrn.32 ROW5L, ROW7L

    cmp r0, #0
    beq 4f


    vdup.s16 ROW1R, ROW0R[1]
    vdup.s16 ROW2R, ROW0R[2]
    vdup.s16 ROW3R, ROW0R[3]
    vdup.s16 ROW4R, ROW0R[0]
    vdup.s16 ROW5R, ROW0R[1]
    vdup.s16 ROW6R, ROW0R[2]
    vdup.s16 ROW7R, ROW0R[3]
    vdup.s16 ROW0R, ROW0R[0]
    b 1b

4:
    vld1.s16 {d2}, [ip, :64]
    vmull.s16 q6, ROW1L, d1[3]
    vmlal.s16 q6, ROW3L, d2[3]
    vmull.s16 q7, ROW3L, d1[3]
    vmlal.s16 q7, ROW1L, d2[0]
    vmull.s16 q2, ROW2L, d0[1]
    vshll.s16 q3, ROW0L, #13
    vmov q4, q6
    vmlal.s16 q6, ROW3L, d2[2]
    vmlsl.s16 q4, ROW1L, d0[0]
    vadd.s32 q1, q3, q2
    vmov q5, q7
    vmlal.s16 q7, ROW1L, d1[0]
    vadd.s32 q1, q1, q6
    vadd.s32 q6, q6, q6
    vmlsl.s16 q5, ROW3L, d0[2]
    vshrn.s32 ROW1L, q1, #16
    vsub.s32 q1, q1, q6
    vmull.s16 q6, ROW2L, d1[2]
    vsub.s32 q3, q3, q2
    vshrn.s32 ROW2R, q1, #16
    vadd.s32 q1, q3, q5
    vsub.s32 q3, q3, q5
    vshll.s16 q5, ROW0L, #13
    vshrn.s32 ROW2L, q1, #16
    vshrn.s32 ROW1R, q3, #16
    vadd.s32 q2, q5, q6
    vsub.s32 q1, q5, q6
    vadd.s32 q6, q2, q7
    vsub.s32 q2, q2, q7
    vadd.s32 q5, q1, q4
    vsub.s32 q3, q1, q4
    vshrn.s32 ROW3R, q2, #16
    vshrn.s32 ROW3L, q5, #16
    vshrn.s32 ROW0L, q6, #16
    vshrn.s32 ROW0R, q3, #16

    vld1.s16 {d2}, [ip, :64]
    vmull.s16 q6, ROW5L, d1[3]
    vmlal.s16 q6, ROW7L, d2[3]
    vmull.s16 q7, ROW7L, d1[3]
    vmlal.s16 q7, ROW5L, d2[0]
    vmull.s16 q2, ROW6L, d0[1]
    vshll.s16 q3, ROW4L, #13
    vmov q4, q6
    vmlal.s16 q6, ROW7L, d2[2]
    vmlsl.s16 q4, ROW5L, d0[0]
    vadd.s32 q1, q3, q2
    vmov q5, q7
    vmlal.s16 q7, ROW5L, d1[0]
    vadd.s32 q1, q1, q6
    vadd.s32 q6, q6, q6
    vmlsl.s16 q5, ROW7L, d0[2]
    vshrn.s32 ROW5L, q1, #16
    vsub.s32 q1, q1, q6
    vmull.s16 q6, ROW6L, d1[2]
    vsub.s32 q3, q3, q2
    vshrn.s32 ROW6R, q1, #16
    vadd.s32 q1, q3, q5
    vsub.s32 q3, q3, q5
    vshll.s16 q5, ROW4L, #13
    vshrn.s32 ROW6L, q1, #16
    vshrn.s32 ROW5R, q3, #16
    vadd.s32 q2, q5, q6
    vsub.s32 q1, q5, q6
    vadd.s32 q6, q2, q7
    vsub.s32 q2, q2, q7
    vadd.s32 q5, q1, q4
    vsub.s32 q3, q1, q4
    vshrn.s32 ROW7R, q2, #16
    vshrn.s32 ROW7L, q5, #16
    vshrn.s32 ROW4L, q6, #16
    vshrn.s32 ROW4R, q3, #16
    b 2b

.unreq dct_table
.unreq DCT_TABLE
.unreq coef_block
.unreq COEF_BLOCK
.unreq output_buf
.unreq OUTPUT_BUF
.unreq output_col
.unreq OUTPUT_COL
.unreq tmp1
.unreq TMP1
.unreq tmp2
.unreq TMP2
.unreq tmp3
.unreq TMP3
.unreq tmp4
.unreq TMP4

.unreq row0l
.unreq ROW0L
.unreq row0r
.unreq ROW0R
.unreq row1l
.unreq ROW1L
.unreq row1r
.unreq ROW1R
.unreq row2l
.unreq ROW2L
.unreq row2r
.unreq ROW2R
.unreq row3l
.unreq ROW3L
.unreq row3r
.unreq ROW3R
.unreq row4l
.unreq ROW4L
.unreq row4r
.unreq ROW4R
.unreq row5l
.unreq ROW5L
.unreq row5r
.unreq ROW5R
.unreq row6l
.unreq ROW6L
.unreq row6r
.unreq ROW6R
.unreq row7l
.unreq ROW7L
.unreq row7r
.unreq ROW7R
@.endfunc
# 699 "src/simd/jsimd_arm_neon.s"
.balign 16
jsimd_idct_ifast_neon_consts:
    .short (277 * 128 - 256 * 128)
    .short (362 * 128 - 256 * 128)
    .short (473 * 128 - 256 * 128)
    .short (669 * 128 - 512 * 128)


    @.func _jsimd_idct_ifast_neon
    .globl _jsimd_idct_ifast_neon
_jsimd_idct_ifast_neon:
# 56 "src/simd/jsimd_arm_neon.s"

    DCT_TABLE .req r0
    COEF_BLOCK .req r1
    OUTPUT_BUF .req r2
    OUTPUT_COL .req r3
    TMP1 .req r0
    TMP2 .req r1
    TMP3 .req r2
    TMP4 .req ip
# 730 "src/simd/jsimd_arm_neon.s"
    adr ip, jsimd_idct_ifast_neon_consts
    vld1.16 {d16, d17, d18, d19}, [COEF_BLOCK, :128]!
    vld1.16 {d0, d1, d2, d3}, [DCT_TABLE, :128]!
    vld1.16 {d20, d21, d22, d23}, [COEF_BLOCK, :128]!
    vmul.s16 q8, q8, q0
    vld1.16 {d4, d5, d6, d7}, [DCT_TABLE, :128]!
    vmul.s16 q9, q9, q1
    vld1.16 {d24, d25, d26, d27}, [COEF_BLOCK, :128]!
    vmul.s16 q10, q10, q2
    vld1.16 {d0, d1, d2, d3}, [DCT_TABLE, :128]!
    vmul.s16 q11, q11, q3
    vld1.16 {d28, d29, d30, d31}, [COEF_BLOCK, :128]
    vmul.s16 q12, q12, q0
    vld1.16 {d4, d5, d6, d7}, [DCT_TABLE, :128]!
    vmul.s16 q14, q14, q2
    vmul.s16 q13, q13, q1
    vld1.16 {d0}, [ip, :64]
    vmul.s16 q15, q15, q3
    vpush {d8-d13}

    vsub.s16 q2, q10, q14
    vadd.s16 q14, q10, q14
    vsub.s16 q1, q11, q13
    vadd.s16 q13, q11, q13
    vsub.s16 q5, q9, q15
    vadd.s16 q15, q9, q15
    vqdmulh.s16 q4, q2, d0[1]
    vqdmulh.s16 q6, q1, d0[3]
    vadd.s16 q3, q1, q1
    vsub.s16 q1, q5, q1
    vadd.s16 q10, q2, q4
    vqdmulh.s16 q4, q1, d0[2]
    vsub.s16 q2, q15, q13
    vadd.s16 q3, q3, q6
    vqdmulh.s16 q6, q2, d0[1]
    vadd.s16 q1, q1, q4
    vqdmulh.s16 q4, q5, d0[0]
    vsub.s16 q10, q10, q14
    vadd.s16 q2, q2, q6
    vsub.s16 q6, q8, q12
    vadd.s16 q12, q8, q12
    vadd.s16 q9, q5, q4
    vadd.s16 q5, q6, q10
    vsub.s16 q10, q6, q10
    vadd.s16 q6, q15, q13
    vadd.s16 q8, q12, q14
    vsub.s16 q3, q6, q3
    vsub.s16 q12, q12, q14
    vsub.s16 q3, q3, q1
    vsub.s16 q1, q9, q1
    vadd.s16 q2, q3, q2
    vsub.s16 q15, q8, q6
    vadd.s16 q1, q1, q2
    vadd.s16 q8, q8, q6
    vadd.s16 q14, q5, q3
    vsub.s16 q9, q5, q3
    vsub.s16 q13, q10, q2
    vadd.s16 q10, q10, q2

      vtrn.16 q8, q9
    vsub.s16 q11, q12, q1
      vtrn.16 q14, q15
    vadd.s16 q12, q12, q1
      vtrn.16 q10, q11
      vtrn.16 q12, q13
      vtrn.32 q9, q11
      vtrn.32 q12, q14
      vtrn.32 q8, q10
      vtrn.32 q13, q15
      vswp d28, d21
      vswp d26, d19

    vsub.s16 q2, q10, q14
      vswp d30, d23
    vadd.s16 q14, q10, q14
      vswp d24, d17
    vsub.s16 q1, q11, q13
    vadd.s16 q13, q11, q13
    vsub.s16 q5, q9, q15
    vadd.s16 q15, q9, q15
    vqdmulh.s16 q4, q2, d0[1]
    vqdmulh.s16 q6, q1, d0[3]
    vadd.s16 q3, q1, q1
    vsub.s16 q1, q5, q1
    vadd.s16 q10, q2, q4
    vqdmulh.s16 q4, q1, d0[2]
    vsub.s16 q2, q15, q13
    vadd.s16 q3, q3, q6
    vqdmulh.s16 q6, q2, d0[1]
    vadd.s16 q1, q1, q4
    vqdmulh.s16 q4, q5, d0[0]
    vsub.s16 q10, q10, q14
    vadd.s16 q2, q2, q6
    vsub.s16 q6, q8, q12
    vadd.s16 q12, q8, q12
    vadd.s16 q9, q5, q4
    vadd.s16 q5, q6, q10
    vsub.s16 q10, q6, q10
    vadd.s16 q6, q15, q13
    vadd.s16 q8, q12, q14
    vsub.s16 q3, q6, q3
    vsub.s16 q12, q12, q14
    vsub.s16 q3, q3, q1
    vsub.s16 q1, q9, q1
    vadd.s16 q2, q3, q2
    vsub.s16 q15, q8, q6
    vadd.s16 q1, q1, q2
    vadd.s16 q8, q8, q6
    vadd.s16 q14, q5, q3
    vsub.s16 q9, q5, q3
    vsub.s16 q13, q10, q2
    vpop {d8-d13}
    vadd.s16 q10, q10, q2
    vsub.s16 q11, q12, q1
    vadd.s16 q12, q12, q1

    vmov.u8 q0, #0x80
    vqshrn.s16 d16, q8, #5
    vqshrn.s16 d17, q9, #5
    vqshrn.s16 d18, q10, #5
    vqshrn.s16 d19, q11, #5
    vqshrn.s16 d20, q12, #5
    vqshrn.s16 d21, q13, #5
    vqshrn.s16 d22, q14, #5
    vqshrn.s16 d23, q15, #5
    vadd.u8 q8, q8, q0
    vadd.u8 q9, q9, q0
    vadd.u8 q10, q10, q0
    vadd.u8 q11, q11, q0

    vtrn.16 q8, q9
    vtrn.16 q10, q11
    vtrn.32 q8, q10
    vtrn.32 q9, q11
    vtrn.8 d16, d17
    vtrn.8 d18, d19

      ldmia OUTPUT_BUF!, {TMP1, TMP2}
      add TMP1, TMP1, OUTPUT_COL
      add TMP2, TMP2, OUTPUT_COL
      vst1.8 {d16}, [TMP1]
      vst1.8 {d17}, [TMP2]
      ldmia OUTPUT_BUF!, {TMP1, TMP2}
      add TMP1, TMP1, OUTPUT_COL
      add TMP2, TMP2, OUTPUT_COL
      vst1.8 {d18}, [TMP1]
    vtrn.8 d20, d21
      vst1.8 {d19}, [TMP2]
      ldmia OUTPUT_BUF, {TMP1, TMP2, TMP3, TMP4}
      add TMP1, TMP1, OUTPUT_COL
      add TMP2, TMP2, OUTPUT_COL
      add TMP3, TMP3, OUTPUT_COL
      add TMP4, TMP4, OUTPUT_COL
      vst1.8 {d20}, [TMP1]
    vtrn.8 d22, d23
      vst1.8 {d21}, [TMP2]
      vst1.8 {d22}, [TMP3]
      vst1.8 {d23}, [TMP4]
    bx lr

.unreq dct_table
.unreq DCT_TABLE
.unreq coef_block
.unreq COEF_BLOCK
.unreq output_buf
.unreq OUTPUT_BUF
.unreq output_col
.unreq OUTPUT_COL
.unreq tmp1
.unreq TMP1
.unreq tmp2
.unreq TMP2
.unreq tmp3
.unreq TMP3
.unreq tmp4
.unreq TMP4
@.endfunc
# 938 "src/simd/jsimd_arm_neon.s"
.balign 16
jsimd_idct_4x4_neon_consts:
    .short (15137)
    .short -(6270)
    .short -(1730)
    .short (11893)
    .short -(17799)
    .short (8697)
    .short -(4176)
    .short -(4926)
    .short (7373)
    .short (20995)
    .short 1 << (13 +1)
    .short 0



    @.func _jsimd_idct_4x4_neon
    .globl _jsimd_idct_4x4_neon
_jsimd_idct_4x4_neon:
# 56 "src/simd/jsimd_arm_neon.s"

    DCT_TABLE .req r0
    COEF_BLOCK .req r1
    OUTPUT_BUF .req r2
    OUTPUT_COL .req r3
    TMP1 .req r0
    TMP2 .req r1
    TMP3 .req r2
    TMP4 .req ip

    vpush {d8-d15}


    adr TMP4, jsimd_idct_4x4_neon_consts
    vld1.16 {d0, d1, d2, d3}, [TMP4, :128]
# 1029 "src/simd/jsimd_arm_neon.s"
    vld1.16 {d4, d5, d6, d7}, [COEF_BLOCK, :128]!
    vld1.16 {d8, d9, d10, d11}, [COEF_BLOCK, :128]!
    add COEF_BLOCK, COEF_BLOCK, #16
    vld1.16 {d12, d13, d14, d15}, [COEF_BLOCK, :128]!
    vld1.16 {d16, d17}, [COEF_BLOCK, :128]!

    vld1.16 {d18, d19, d20, d21}, [DCT_TABLE, :128]!
    vmul.s16 q2, q2, q9
    vld1.16 {d22, d23, d24, d25}, [DCT_TABLE, :128]!
    vmul.s16 q3, q3, q10
    vmul.s16 q4, q4, q11
    add DCT_TABLE, DCT_TABLE, #16
    vld1.16 {d26, d27, d28, d29}, [DCT_TABLE, :128]!
    vmul.s16 q5, q5, q12
    vmul.s16 q6, q6, q13
    vld1.16 {d30, d31}, [DCT_TABLE, :128]!
    vmul.s16 q7, q7, q14
    vmul.s16 q8, q8, q15


    vmull.s16 q14, d4, d2[2]
    vmlal.s16 q14, d8, d0[0]
    vmlal.s16 q14, d14, d0[1]

    vmull.s16 q13, d16, d1[2]
    vmlal.s16 q13, d12, d1[3]
    vmlal.s16 q13, d10, d2[0]
    vmlal.s16 q13, d6, d2[1]

    vmull.s16 q15, d4, d2[2]
    vmlsl.s16 q15, d8, d0[0]
    vmlsl.s16 q15, d14, d0[1]

    vmull.s16 q12, d16, d0[2]
    vmlal.s16 q12, d12, d0[3]
    vmlal.s16 q12, d10, d1[0]
    vmlal.s16 q12, d6, d1[1]

    vadd.s32 q10, q14, q13
    vsub.s32 q14, q14, q13

    vrshrn.s32 d4, q10, #12
    vrshrn.s32 d10, q14, #12

    vadd.s32 q10, q15, q12
    vsub.s32 q15, q15, q12

    vrshrn.s32 d6, q10, #12
    vrshrn.s32 d8, q15, #12

    vtrn.16 d4, d6
    vtrn.16 d8, d10
    vtrn.32 d4, d8
    vtrn.32 d6, d10
    vmull.s16 q14, d5, d2[2]
    vmlal.s16 q14, d9, d0[0]
    vmlal.s16 q14, d15, d0[1]

    vmull.s16 q13, d17, d1[2]
    vmlal.s16 q13, d13, d1[3]
    vmlal.s16 q13, d11, d2[0]
    vmlal.s16 q13, d7, d2[1]

    vmull.s16 q15, d5, d2[2]
    vmlsl.s16 q15, d9, d0[0]
    vmlsl.s16 q15, d15, d0[1]

    vmull.s16 q12, d17, d0[2]
    vmlal.s16 q12, d13, d0[3]
    vmlal.s16 q12, d11, d1[0]
    vmlal.s16 q12, d7, d1[1]

    vadd.s32 q10, q14, q13
    vsub.s32 q14, q14, q13

    vrshrn.s32 d5, q10, #12
    vrshrn.s32 d11, q14, #12

    vadd.s32 q10, q15, q12
    vsub.s32 q15, q15, q12

    vrshrn.s32 d7, q10, #12
    vrshrn.s32 d9, q15, #12

    vtrn.16 d5, d7
    vtrn.16 d9, d11
    vtrn.32 d5, d9
    vtrn.32 d7, d11


    vmull.s16 q14, d4, d2[2]
    vmlal.s16 q14, d8, d0[0]
    vmlal.s16 q14, d9, d0[1]

    vmull.s16 q13, d11, d1[2]
    vmlal.s16 q13, d7, d1[3]
    vmlal.s16 q13, d10, d2[0]
    vmlal.s16 q13, d6, d2[1]

    vmull.s16 q15, d4, d2[2]
    vmlsl.s16 q15, d8, d0[0]
    vmlsl.s16 q15, d9, d0[1]

    vmull.s16 q12, d11, d0[2]
    vmlal.s16 q12, d7, d0[3]
    vmlal.s16 q12, d10, d1[0]
    vmlal.s16 q12, d6, d1[1]

    vadd.s32 q10, q14, q13
    vsub.s32 q14, q14, q13

    vrshr.s32 q10, q10, #19
    vrshr.s32 q14, q14, #19
    vmovn.s32 d26, q10
    vmovn.s32 d29, q14

    vadd.s32 q10, q15, q12
    vsub.s32 q15, q15, q12

    vrshr.s32 q10, q10, #19
    vrshr.s32 q15, q15, #19
    vmovn.s32 d27, q10
    vmovn.s32 d28, q15

    vtrn.16 d26, d27
    vtrn.16 d28, d29
    vtrn.32 d26, d28
    vtrn.32 d27, d29


    vmov.u16 q15, #0x80
    vadd.s16 q13, q13, q15
    vadd.s16 q14, q14, q15
    vqmovun.s16 d26, q13
    vqmovun.s16 d27, q14


    ldmia OUTPUT_BUF, {TMP1, TMP2, TMP3, TMP4}
    add TMP1, TMP1, OUTPUT_COL
    add TMP2, TMP2, OUTPUT_COL
    add TMP3, TMP3, OUTPUT_COL
    add TMP4, TMP4, OUTPUT_COL
# 1081 "src/simd/jsimd_arm_neon.s"
    vst1.8 {d26[0]}, [TMP1]!
    vst1.8 {d27[0]}, [TMP3]!
    vst1.8 {d26[1]}, [TMP1]!
    vst1.8 {d27[1]}, [TMP3]!
    vst1.8 {d26[2]}, [TMP1]!
    vst1.8 {d27[2]}, [TMP3]!
    vst1.8 {d26[3]}, [TMP1]!
    vst1.8 {d27[3]}, [TMP3]!

    vst1.8 {d26[4]}, [TMP2]!
    vst1.8 {d27[4]}, [TMP4]!
    vst1.8 {d26[5]}, [TMP2]!
    vst1.8 {d27[5]}, [TMP4]!
    vst1.8 {d26[6]}, [TMP2]!
    vst1.8 {d27[6]}, [TMP4]!
    vst1.8 {d26[7]}, [TMP2]!
    vst1.8 {d27[7]}, [TMP4]!


    vpop {d8-d15}
    bx lr

.unreq dct_table
.unreq DCT_TABLE
.unreq coef_block
.unreq COEF_BLOCK
.unreq output_buf
.unreq OUTPUT_BUF
.unreq output_col
.unreq OUTPUT_COL
.unreq tmp1
.unreq TMP1
.unreq tmp2
.unreq TMP2
.unreq tmp3
.unreq TMP3
.unreq tmp4
.unreq TMP4
@.endfunc

# 1132 "src/simd/jsimd_arm_neon.s"
.balign 8
jsimd_idct_2x2_neon_consts:
    .short -(5906)
    .short (6967)
    .short -(10426)
    .short (29692)



    @.func _jsimd_idct_2x2_neon
    .globl _jsimd_idct_2x2_neon
_jsimd_idct_2x2_neon:
# 56 "src/simd/jsimd_arm_neon.s"

    DCT_TABLE .req r0
    COEF_BLOCK .req r1
    OUTPUT_BUF .req r2
    OUTPUT_COL .req r3
    TMP1 .req r0
    TMP2 .req ip

    vpush {d8-d15}


    adr TMP2, jsimd_idct_2x2_neon_consts
    vld1.16 {d0}, [TMP2, :64]
# 1188 "src/simd/jsimd_arm_neon.s"
    vld1.16 {d4, d5, d6, d7}, [COEF_BLOCK, :128]!
    add COEF_BLOCK, COEF_BLOCK, #16
    vld1.16 {d10, d11}, [COEF_BLOCK, :128]!
    add COEF_BLOCK, COEF_BLOCK, #16
    vld1.16 {d12, d13}, [COEF_BLOCK, :128]!
    add COEF_BLOCK, COEF_BLOCK, #16
    vld1.16 {d16, d17}, [COEF_BLOCK, :128]!

    vld1.16 {d18, d19, d20, d21}, [DCT_TABLE, :128]!
    vmul.s16 q2, q2, q9
    vmul.s16 q3, q3, q10
    add DCT_TABLE, DCT_TABLE, #16
    vld1.16 {d24, d25}, [DCT_TABLE, :128]!
    vmul.s16 q5, q5, q12
    add DCT_TABLE, DCT_TABLE, #16
    vld1.16 {d26, d27}, [DCT_TABLE, :128]!
    vmul.s16 q6, q6, q13
    add DCT_TABLE, DCT_TABLE, #16
    vld1.16 {d30, d31}, [DCT_TABLE, :128]!
    vmul.s16 q8, q8, q15
# 1216 "src/simd/jsimd_arm_neon.s"
    vmull.s16 q13, d6, d0[3]
    vmlal.s16 q13, d10, d0[2]
    vmlal.s16 q13, d12, d0[1]
    vmlal.s16 q13, d16, d0[0]
    vmull.s16 q12, d7, d0[3]
    vmlal.s16 q12, d11, d0[2]
    vmlal.s16 q12, d13, d0[1]
    vmlal.s16 q12, d17, d0[0]
    vshll.s16 q14, d4, #15
    vshll.s16 q15, d5, #15
    vadd.s32 q10, q14, q13
    vsub.s32 q14, q14, q13
    vrshrn.s32 d4, q10, #13
    vrshrn.s32 d6, q14, #13
    vadd.s32 q10, q15, q12
    vsub.s32 q14, q15, q12
    vrshrn.s32 d5, q10, #13
    vrshrn.s32 d7, q14, #13
    vtrn.16 q2, q3
    vtrn.32 q3, q5



    vshll.s16 q14, d4, #15
    vmull.s16 q13, d6, d0[3]
    vmlal.s16 q13, d10, d0[2]
    vmlal.s16 q13, d7, d0[1]
    vmlal.s16 q13, d11, d0[0]

    vadd.s32 q10, q14, q13
    vsub.s32 q14, q14, q13

    vrshr.s32 q10, q10, #20
    vrshr.s32 q14, q14, #20
    vmovn.s32 d26, q10
    vmovn.s32 d27, q14



    vmov.u16 q15, #0x80
    vadd.s16 q13, q13, q15
    vqmovun.s16 d26, q13
    vqmovun.s16 d27, q13


    ldmia OUTPUT_BUF, {TMP1, TMP2}
    add TMP1, TMP1, OUTPUT_COL
    add TMP2, TMP2, OUTPUT_COL

    vst1.8 {d26[0]}, [TMP1]!
    vst1.8 {d27[4]}, [TMP1]!
    vst1.8 {d26[1]}, [TMP2]!
    vst1.8 {d27[5]}, [TMP2]!

    vpop {d8-d15}
    bx lr

.unreq dct_table
.unreq DCT_TABLE
.unreq coef_block
.unreq COEF_BLOCK
.unreq output_buf
.unreq OUTPUT_BUF
.unreq output_col
.unreq OUTPUT_COL
.unreq tmp1
.unreq TMP1
.unreq tmp2
.unreq TMP2
@.endfunc

# 1285 "src/simd/jsimd_arm_neon.s"

















.balign 16
jsimd_ycc_extrgb_neon_consts:
    .short 0, 0, 0, 0
    .short 22971, -11277, -23401, 29033
    .short -128, -128, -128, -128
    .short -128, -128, -128, -128


    @.func _jsimd_ycc_extrgb_convert_neon
    .globl _jsimd_ycc_extrgb_convert_neon
_jsimd_ycc_extrgb_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    INPUT_ROW .req r2
    OUTPUT_BUF .req r3
    NUM_ROWS .req r4

    INPUT_BUF0 .req r5
    INPUT_BUF1 .req r6
    INPUT_BUF2 .req INPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_ycc_extrgb_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr INPUT_BUF0, [INPUT_BUF]
    ldr INPUT_BUF1, [INPUT_BUF, #4]
    ldr INPUT_BUF2, [INPUT_BUF, #8]
.unreq input_buf
.unreq INPUT_BUF


    vpush {d8-d15}


    vmov.u8 q5, #255
    vmov.u8 q6, #255


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [INPUT_BUF0, INPUT_ROW, lsl #2]
    ldr U, [INPUT_BUF1, INPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [INPUT_BUF2, INPUT_ROW, lsl #2]
    add INPUT_ROW, INPUT_ROW, #1
    ldr RGB, [OUTPUT_BUF], #4


    subs N, N, #8
    blt 3f
        vld1.8 {d4}, [U, :64]!
        vld1.8 {d5}, [V, :64]!
        vld1.8 {d0}, [Y, :64]!
        pld [U, #64]
        pld [V, #64]
        pld [Y, #64]
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    blt 2f
1:
    vld1.8 {d4}, [U, :64]!
      vrshrn.s32 d20, q10, #15
      vrshrn.s32 d21, q11, #15
      vrshrn.s32 d24, q12, #14
      vrshrn.s32 d25, q13, #14
      vrshrn.s32 d28, q14, #14
    vld1.8 {d5}, [V, :64]!
      vrshrn.s32 d29, q15, #14
      vaddw.u8 q10, q10, d0
      vaddw.u8 q12, q12, d0
      vaddw.u8 q14, q14, d0
      vqmovun.s16 d11, q10
    vld1.8 {d0}, [Y, :64]!
      vqmovun.s16 d10, q12
    pld [U, #64]
    pld [V, #64]
    pld [Y, #64]
      vqmovun.s16 d12, q14
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
            vst3.8 {d10, d11, d12}, [RGB]!
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d11, q10
    vqmovun.s16 d10, q12
    vqmovun.s16 d12, q14
            vst3.8 {d10, d11, d12}, [RGB]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
        vld1.8 {d4[0]}, [U]!
        vld1.8 {d4[1]}, [U]!
        vld1.8 {d4[2]}, [U]!
        vld1.8 {d4[3]}, [U]!
        vld1.8 {d5[0]}, [V]!
        vld1.8 {d5[1]}, [V]!
        vld1.8 {d5[2]}, [V]!
        vld1.8 {d5[3]}, [V]!
        vld1.8 {d0[0]}, [Y]!
        vld1.8 {d0[1]}, [Y]!
        vld1.8 {d0[2]}, [Y]!
        vld1.8 {d0[3]}, [Y]!
3:
    tst N, #2
    beq 4f
        vld1.8 {d4[4]}, [U]!
        vld1.8 {d4[5]}, [U]!
        vld1.8 {d5[4]}, [V]!
        vld1.8 {d5[5]}, [V]!
        vld1.8 {d0[4]}, [Y]!
        vld1.8 {d0[5]}, [Y]!
4:
    tst N, #1
    beq 5f
        vld1.8 {d4[6]}, [U]!
        vld1.8 {d5[6]}, [V]!
        vld1.8 {d0[6]}, [Y]!
5:
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d11, q10
    vqmovun.s16 d10, q12
    vqmovun.s16 d12, q14
    tst N, #4
    beq 6f
            vst3.8 {d10[0], d11[0], d12[0]}, [RGB]!
            vst3.8 {d10[1], d11[1], d12[1]}, [RGB]!
            vst3.8 {d10[2], d11[2], d12[2]}, [RGB]!
            vst3.8 {d10[3], d11[3], d12[3]}, [RGB]!
6:
    tst N, #2
    beq 7f
            vst3.8 {d10[4], d11[4], d12[4]}, [RGB]!
            vst3.8 {d10[5], d11[5], d12[5]}, [RGB]!
7:
    tst N, #1
    beq 8f
            vst3.8 {d10[6], d11[6], d12[6]}, [RGB]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq input_row
.unreq INPUT_ROW
.unreq output_buf
.unreq OUTPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq input_buf0
.unreq INPUT_BUF0
.unreq input_buf1
.unreq INPUT_BUF1
.unreq input_buf2
.unreq INPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc















.balign 16
jsimd_ycc_extbgr_neon_consts:
    .short 0, 0, 0, 0
    .short 22971, -11277, -23401, 29033
    .short -128, -128, -128, -128
    .short -128, -128, -128, -128


    @.func _jsimd_ycc_extbgr_convert_neon
    .globl _jsimd_ycc_extbgr_convert_neon
_jsimd_ycc_extbgr_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    INPUT_ROW .req r2
    OUTPUT_BUF .req r3
    NUM_ROWS .req r4

    INPUT_BUF0 .req r5
    INPUT_BUF1 .req r6
    INPUT_BUF2 .req INPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_ycc_extbgr_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr INPUT_BUF0, [INPUT_BUF]
    ldr INPUT_BUF1, [INPUT_BUF, #4]
    ldr INPUT_BUF2, [INPUT_BUF, #8]
.unreq input_buf
.unreq INPUT_BUF


    vpush {d8-d15}


    vmov.u8 q5, #255
    vmov.u8 q6, #255


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [INPUT_BUF0, INPUT_ROW, lsl #2]
    ldr U, [INPUT_BUF1, INPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [INPUT_BUF2, INPUT_ROW, lsl #2]
    add INPUT_ROW, INPUT_ROW, #1
    ldr RGB, [OUTPUT_BUF], #4


    subs N, N, #8
    blt 3f
        vld1.8 {d4}, [U, :64]!
        vld1.8 {d5}, [V, :64]!
        vld1.8 {d0}, [Y, :64]!
        pld [U, #64]
        pld [V, #64]
        pld [Y, #64]
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    blt 2f
1:
    vld1.8 {d4}, [U, :64]!
      vrshrn.s32 d20, q10, #15
      vrshrn.s32 d21, q11, #15
      vrshrn.s32 d24, q12, #14
      vrshrn.s32 d25, q13, #14
      vrshrn.s32 d28, q14, #14
    vld1.8 {d5}, [V, :64]!
      vrshrn.s32 d29, q15, #14
      vaddw.u8 q10, q10, d0
      vaddw.u8 q12, q12, d0
      vaddw.u8 q14, q14, d0
      vqmovun.s16 d11, q10
    vld1.8 {d0}, [Y, :64]!
      vqmovun.s16 d12, q12
    pld [U, #64]
    pld [V, #64]
    pld [Y, #64]
      vqmovun.s16 d10, q14
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
            vst3.8 {d10, d11, d12}, [RGB]!
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d11, q10
    vqmovun.s16 d12, q12
    vqmovun.s16 d10, q14
            vst3.8 {d10, d11, d12}, [RGB]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
        vld1.8 {d4[0]}, [U]!
        vld1.8 {d4[1]}, [U]!
        vld1.8 {d4[2]}, [U]!
        vld1.8 {d4[3]}, [U]!
        vld1.8 {d5[0]}, [V]!
        vld1.8 {d5[1]}, [V]!
        vld1.8 {d5[2]}, [V]!
        vld1.8 {d5[3]}, [V]!
        vld1.8 {d0[0]}, [Y]!
        vld1.8 {d0[1]}, [Y]!
        vld1.8 {d0[2]}, [Y]!
        vld1.8 {d0[3]}, [Y]!
3:
    tst N, #2
    beq 4f
        vld1.8 {d4[4]}, [U]!
        vld1.8 {d4[5]}, [U]!
        vld1.8 {d5[4]}, [V]!
        vld1.8 {d5[5]}, [V]!
        vld1.8 {d0[4]}, [Y]!
        vld1.8 {d0[5]}, [Y]!
4:
    tst N, #1
    beq 5f
        vld1.8 {d4[6]}, [U]!
        vld1.8 {d5[6]}, [V]!
        vld1.8 {d0[6]}, [Y]!
5:
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d11, q10
    vqmovun.s16 d12, q12
    vqmovun.s16 d10, q14
    tst N, #4
    beq 6f
            vst3.8 {d10[0], d11[0], d12[0]}, [RGB]!
            vst3.8 {d10[1], d11[1], d12[1]}, [RGB]!
            vst3.8 {d10[2], d11[2], d12[2]}, [RGB]!
            vst3.8 {d10[3], d11[3], d12[3]}, [RGB]!
6:
    tst N, #2
    beq 7f
            vst3.8 {d10[4], d11[4], d12[4]}, [RGB]!
            vst3.8 {d10[5], d11[5], d12[5]}, [RGB]!
7:
    tst N, #1
    beq 8f
            vst3.8 {d10[6], d11[6], d12[6]}, [RGB]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq input_row
.unreq INPUT_ROW
.unreq output_buf
.unreq OUTPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq input_buf0
.unreq INPUT_BUF0
.unreq input_buf1
.unreq INPUT_BUF1
.unreq input_buf2
.unreq INPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc















.balign 16
jsimd_ycc_extrgbx_neon_consts:
    .short 0, 0, 0, 0
    .short 22971, -11277, -23401, 29033
    .short -128, -128, -128, -128
    .short -128, -128, -128, -128


    @.func _jsimd_ycc_extrgbx_convert_neon
    .globl _jsimd_ycc_extrgbx_convert_neon
_jsimd_ycc_extrgbx_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    INPUT_ROW .req r2
    OUTPUT_BUF .req r3
    NUM_ROWS .req r4

    INPUT_BUF0 .req r5
    INPUT_BUF1 .req r6
    INPUT_BUF2 .req INPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_ycc_extrgbx_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr INPUT_BUF0, [INPUT_BUF]
    ldr INPUT_BUF1, [INPUT_BUF, #4]
    ldr INPUT_BUF2, [INPUT_BUF, #8]
.unreq input_buf
.unreq INPUT_BUF


    vpush {d8-d15}


    vmov.u8 q5, #255
    vmov.u8 q6, #255


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [INPUT_BUF0, INPUT_ROW, lsl #2]
    ldr U, [INPUT_BUF1, INPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [INPUT_BUF2, INPUT_ROW, lsl #2]
    add INPUT_ROW, INPUT_ROW, #1
    ldr RGB, [OUTPUT_BUF], #4


    subs N, N, #8
    blt 3f
        vld1.8 {d4}, [U, :64]!
        vld1.8 {d5}, [V, :64]!
        vld1.8 {d0}, [Y, :64]!
        pld [U, #64]
        pld [V, #64]
        pld [Y, #64]
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    blt 2f
1:
    vld1.8 {d4}, [U, :64]!
      vrshrn.s32 d20, q10, #15
      vrshrn.s32 d21, q11, #15
      vrshrn.s32 d24, q12, #14
      vrshrn.s32 d25, q13, #14
      vrshrn.s32 d28, q14, #14
    vld1.8 {d5}, [V, :64]!
      vrshrn.s32 d29, q15, #14
      vaddw.u8 q10, q10, d0
      vaddw.u8 q12, q12, d0
      vaddw.u8 q14, q14, d0
      vqmovun.s16 d11, q10
    vld1.8 {d0}, [Y, :64]!
      vqmovun.s16 d10, q12
    pld [U, #64]
    pld [V, #64]
    pld [Y, #64]
      vqmovun.s16 d12, q14
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
            vst4.8 {d10, d11, d12, d13}, [RGB]!
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d11, q10
    vqmovun.s16 d10, q12
    vqmovun.s16 d12, q14
            vst4.8 {d10, d11, d12, d13}, [RGB]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
        vld1.8 {d4[0]}, [U]!
        vld1.8 {d4[1]}, [U]!
        vld1.8 {d4[2]}, [U]!
        vld1.8 {d4[3]}, [U]!
        vld1.8 {d5[0]}, [V]!
        vld1.8 {d5[1]}, [V]!
        vld1.8 {d5[2]}, [V]!
        vld1.8 {d5[3]}, [V]!
        vld1.8 {d0[0]}, [Y]!
        vld1.8 {d0[1]}, [Y]!
        vld1.8 {d0[2]}, [Y]!
        vld1.8 {d0[3]}, [Y]!
3:
    tst N, #2
    beq 4f
        vld1.8 {d4[4]}, [U]!
        vld1.8 {d4[5]}, [U]!
        vld1.8 {d5[4]}, [V]!
        vld1.8 {d5[5]}, [V]!
        vld1.8 {d0[4]}, [Y]!
        vld1.8 {d0[5]}, [Y]!
4:
    tst N, #1
    beq 5f
        vld1.8 {d4[6]}, [U]!
        vld1.8 {d5[6]}, [V]!
        vld1.8 {d0[6]}, [Y]!
5:
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d11, q10
    vqmovun.s16 d10, q12
    vqmovun.s16 d12, q14
    tst N, #4
    beq 6f
            vst4.8 {d10[0], d11[0], d12[0], d13[0]}, [RGB]!
            vst4.8 {d10[1], d11[1], d12[1], d13[1]}, [RGB]!
            vst4.8 {d10[2], d11[2], d12[2], d13[2]}, [RGB]!
            vst4.8 {d10[3], d11[3], d12[3], d13[3]}, [RGB]!
6:
    tst N, #2
    beq 7f
            vst4.8 {d10[4], d11[4], d12[4], d13[4]}, [RGB]!
            vst4.8 {d10[5], d11[5], d12[5], d13[5]}, [RGB]!
7:
    tst N, #1
    beq 8f
            vst4.8 {d10[6], d11[6], d12[6], d13[6]}, [RGB]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq input_row
.unreq INPUT_ROW
.unreq output_buf
.unreq OUTPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq input_buf0
.unreq INPUT_BUF0
.unreq input_buf1
.unreq INPUT_BUF1
.unreq input_buf2
.unreq INPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc















.balign 16
jsimd_ycc_extbgrx_neon_consts:
    .short 0, 0, 0, 0
    .short 22971, -11277, -23401, 29033
    .short -128, -128, -128, -128
    .short -128, -128, -128, -128


    @.func _jsimd_ycc_extbgrx_convert_neon
    .globl _jsimd_ycc_extbgrx_convert_neon
_jsimd_ycc_extbgrx_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    INPUT_ROW .req r2
    OUTPUT_BUF .req r3
    NUM_ROWS .req r4

    INPUT_BUF0 .req r5
    INPUT_BUF1 .req r6
    INPUT_BUF2 .req INPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_ycc_extbgrx_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr INPUT_BUF0, [INPUT_BUF]
    ldr INPUT_BUF1, [INPUT_BUF, #4]
    ldr INPUT_BUF2, [INPUT_BUF, #8]
.unreq input_buf
.unreq INPUT_BUF


    vpush {d8-d15}


    vmov.u8 q5, #255
    vmov.u8 q6, #255


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [INPUT_BUF0, INPUT_ROW, lsl #2]
    ldr U, [INPUT_BUF1, INPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [INPUT_BUF2, INPUT_ROW, lsl #2]
    add INPUT_ROW, INPUT_ROW, #1
    ldr RGB, [OUTPUT_BUF], #4


    subs N, N, #8
    blt 3f
        vld1.8 {d4}, [U, :64]!
        vld1.8 {d5}, [V, :64]!
        vld1.8 {d0}, [Y, :64]!
        pld [U, #64]
        pld [V, #64]
        pld [Y, #64]
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    blt 2f
1:
    vld1.8 {d4}, [U, :64]!
      vrshrn.s32 d20, q10, #15
      vrshrn.s32 d21, q11, #15
      vrshrn.s32 d24, q12, #14
      vrshrn.s32 d25, q13, #14
      vrshrn.s32 d28, q14, #14
    vld1.8 {d5}, [V, :64]!
      vrshrn.s32 d29, q15, #14
      vaddw.u8 q10, q10, d0
      vaddw.u8 q12, q12, d0
      vaddw.u8 q14, q14, d0
      vqmovun.s16 d11, q10
    vld1.8 {d0}, [Y, :64]!
      vqmovun.s16 d12, q12
    pld [U, #64]
    pld [V, #64]
    pld [Y, #64]
      vqmovun.s16 d10, q14
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
            vst4.8 {d10, d11, d12, d13}, [RGB]!
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d11, q10
    vqmovun.s16 d12, q12
    vqmovun.s16 d10, q14
            vst4.8 {d10, d11, d12, d13}, [RGB]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
        vld1.8 {d4[0]}, [U]!
        vld1.8 {d4[1]}, [U]!
        vld1.8 {d4[2]}, [U]!
        vld1.8 {d4[3]}, [U]!
        vld1.8 {d5[0]}, [V]!
        vld1.8 {d5[1]}, [V]!
        vld1.8 {d5[2]}, [V]!
        vld1.8 {d5[3]}, [V]!
        vld1.8 {d0[0]}, [Y]!
        vld1.8 {d0[1]}, [Y]!
        vld1.8 {d0[2]}, [Y]!
        vld1.8 {d0[3]}, [Y]!
3:
    tst N, #2
    beq 4f
        vld1.8 {d4[4]}, [U]!
        vld1.8 {d4[5]}, [U]!
        vld1.8 {d5[4]}, [V]!
        vld1.8 {d5[5]}, [V]!
        vld1.8 {d0[4]}, [Y]!
        vld1.8 {d0[5]}, [Y]!
4:
    tst N, #1
    beq 5f
        vld1.8 {d4[6]}, [U]!
        vld1.8 {d5[6]}, [V]!
        vld1.8 {d0[6]}, [Y]!
5:
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d11, q10
    vqmovun.s16 d12, q12
    vqmovun.s16 d10, q14
    tst N, #4
    beq 6f
            vst4.8 {d10[0], d11[0], d12[0], d13[0]}, [RGB]!
            vst4.8 {d10[1], d11[1], d12[1], d13[1]}, [RGB]!
            vst4.8 {d10[2], d11[2], d12[2], d13[2]}, [RGB]!
            vst4.8 {d10[3], d11[3], d12[3], d13[3]}, [RGB]!
6:
    tst N, #2
    beq 7f
            vst4.8 {d10[4], d11[4], d12[4], d13[4]}, [RGB]!
            vst4.8 {d10[5], d11[5], d12[5], d13[5]}, [RGB]!
7:
    tst N, #1
    beq 8f
            vst4.8 {d10[6], d11[6], d12[6], d13[6]}, [RGB]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq input_row
.unreq INPUT_ROW
.unreq output_buf
.unreq OUTPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq input_buf0
.unreq INPUT_BUF0
.unreq input_buf1
.unreq INPUT_BUF1
.unreq input_buf2
.unreq INPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc















.balign 16
jsimd_ycc_extxbgr_neon_consts:
    .short 0, 0, 0, 0
    .short 22971, -11277, -23401, 29033
    .short -128, -128, -128, -128
    .short -128, -128, -128, -128


    @.func _jsimd_ycc_extxbgr_convert_neon
    .globl _jsimd_ycc_extxbgr_convert_neon
_jsimd_ycc_extxbgr_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    INPUT_ROW .req r2
    OUTPUT_BUF .req r3
    NUM_ROWS .req r4

    INPUT_BUF0 .req r5
    INPUT_BUF1 .req r6
    INPUT_BUF2 .req INPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_ycc_extxbgr_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr INPUT_BUF0, [INPUT_BUF]
    ldr INPUT_BUF1, [INPUT_BUF, #4]
    ldr INPUT_BUF2, [INPUT_BUF, #8]
.unreq input_buf
.unreq INPUT_BUF


    vpush {d8-d15}


    vmov.u8 q5, #255
    vmov.u8 q6, #255


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [INPUT_BUF0, INPUT_ROW, lsl #2]
    ldr U, [INPUT_BUF1, INPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [INPUT_BUF2, INPUT_ROW, lsl #2]
    add INPUT_ROW, INPUT_ROW, #1
    ldr RGB, [OUTPUT_BUF], #4


    subs N, N, #8
    blt 3f
        vld1.8 {d4}, [U, :64]!
        vld1.8 {d5}, [V, :64]!
        vld1.8 {d0}, [Y, :64]!
        pld [U, #64]
        pld [V, #64]
        pld [Y, #64]
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    blt 2f
1:
    vld1.8 {d4}, [U, :64]!
      vrshrn.s32 d20, q10, #15
      vrshrn.s32 d21, q11, #15
      vrshrn.s32 d24, q12, #14
      vrshrn.s32 d25, q13, #14
      vrshrn.s32 d28, q14, #14
    vld1.8 {d5}, [V, :64]!
      vrshrn.s32 d29, q15, #14
      vaddw.u8 q10, q10, d0
      vaddw.u8 q12, q12, d0
      vaddw.u8 q14, q14, d0
      vqmovun.s16 d12, q10
    vld1.8 {d0}, [Y, :64]!
      vqmovun.s16 d13, q12
    pld [U, #64]
    pld [V, #64]
    pld [Y, #64]
      vqmovun.s16 d11, q14
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
            vst4.8 {d10, d11, d12, d13}, [RGB]!
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d12, q10
    vqmovun.s16 d13, q12
    vqmovun.s16 d11, q14
            vst4.8 {d10, d11, d12, d13}, [RGB]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
        vld1.8 {d4[0]}, [U]!
        vld1.8 {d4[1]}, [U]!
        vld1.8 {d4[2]}, [U]!
        vld1.8 {d4[3]}, [U]!
        vld1.8 {d5[0]}, [V]!
        vld1.8 {d5[1]}, [V]!
        vld1.8 {d5[2]}, [V]!
        vld1.8 {d5[3]}, [V]!
        vld1.8 {d0[0]}, [Y]!
        vld1.8 {d0[1]}, [Y]!
        vld1.8 {d0[2]}, [Y]!
        vld1.8 {d0[3]}, [Y]!
3:
    tst N, #2
    beq 4f
        vld1.8 {d4[4]}, [U]!
        vld1.8 {d4[5]}, [U]!
        vld1.8 {d5[4]}, [V]!
        vld1.8 {d5[5]}, [V]!
        vld1.8 {d0[4]}, [Y]!
        vld1.8 {d0[5]}, [Y]!
4:
    tst N, #1
    beq 5f
        vld1.8 {d4[6]}, [U]!
        vld1.8 {d5[6]}, [V]!
        vld1.8 {d0[6]}, [Y]!
5:
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d12, q10
    vqmovun.s16 d13, q12
    vqmovun.s16 d11, q14
    tst N, #4
    beq 6f
            vst4.8 {d10[0], d11[0], d12[0], d13[0]}, [RGB]!
            vst4.8 {d10[1], d11[1], d12[1], d13[1]}, [RGB]!
            vst4.8 {d10[2], d11[2], d12[2], d13[2]}, [RGB]!
            vst4.8 {d10[3], d11[3], d12[3], d13[3]}, [RGB]!
6:
    tst N, #2
    beq 7f
            vst4.8 {d10[4], d11[4], d12[4], d13[4]}, [RGB]!
            vst4.8 {d10[5], d11[5], d12[5], d13[5]}, [RGB]!
7:
    tst N, #1
    beq 8f
            vst4.8 {d10[6], d11[6], d12[6], d13[6]}, [RGB]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq input_row
.unreq INPUT_ROW
.unreq output_buf
.unreq OUTPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq input_buf0
.unreq INPUT_BUF0
.unreq input_buf1
.unreq INPUT_BUF1
.unreq input_buf2
.unreq INPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc















.balign 16
jsimd_ycc_extxrgb_neon_consts:
    .short 0, 0, 0, 0
    .short 22971, -11277, -23401, 29033
    .short -128, -128, -128, -128
    .short -128, -128, -128, -128


    @.func _jsimd_ycc_extxrgb_convert_neon
    .globl _jsimd_ycc_extxrgb_convert_neon
_jsimd_ycc_extxrgb_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    INPUT_ROW .req r2
    OUTPUT_BUF .req r3
    NUM_ROWS .req r4

    INPUT_BUF0 .req r5
    INPUT_BUF1 .req r6
    INPUT_BUF2 .req INPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_ycc_extxrgb_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr INPUT_BUF0, [INPUT_BUF]
    ldr INPUT_BUF1, [INPUT_BUF, #4]
    ldr INPUT_BUF2, [INPUT_BUF, #8]
.unreq input_buf
.unreq INPUT_BUF


    vpush {d8-d15}


    vmov.u8 q5, #255
    vmov.u8 q6, #255


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [INPUT_BUF0, INPUT_ROW, lsl #2]
    ldr U, [INPUT_BUF1, INPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [INPUT_BUF2, INPUT_ROW, lsl #2]
    add INPUT_ROW, INPUT_ROW, #1
    ldr RGB, [OUTPUT_BUF], #4


    subs N, N, #8
    blt 3f
        vld1.8 {d4}, [U, :64]!
        vld1.8 {d5}, [V, :64]!
        vld1.8 {d0}, [Y, :64]!
        pld [U, #64]
        pld [V, #64]
        pld [Y, #64]
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    blt 2f
1:
    vld1.8 {d4}, [U, :64]!
      vrshrn.s32 d20, q10, #15
      vrshrn.s32 d21, q11, #15
      vrshrn.s32 d24, q12, #14
      vrshrn.s32 d25, q13, #14
      vrshrn.s32 d28, q14, #14
    vld1.8 {d5}, [V, :64]!
      vrshrn.s32 d29, q15, #14
      vaddw.u8 q10, q10, d0
      vaddw.u8 q12, q12, d0
      vaddw.u8 q14, q14, d0
      vqmovun.s16 d12, q10
    vld1.8 {d0}, [Y, :64]!
      vqmovun.s16 d11, q12
    pld [U, #64]
    pld [V, #64]
    pld [Y, #64]
      vqmovun.s16 d13, q14
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
            vst4.8 {d10, d11, d12, d13}, [RGB]!
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d12, q10
    vqmovun.s16 d11, q12
    vqmovun.s16 d13, q14
            vst4.8 {d10, d11, d12, d13}, [RGB]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
        vld1.8 {d4[0]}, [U]!
        vld1.8 {d4[1]}, [U]!
        vld1.8 {d4[2]}, [U]!
        vld1.8 {d4[3]}, [U]!
        vld1.8 {d5[0]}, [V]!
        vld1.8 {d5[1]}, [V]!
        vld1.8 {d5[2]}, [V]!
        vld1.8 {d5[3]}, [V]!
        vld1.8 {d0[0]}, [Y]!
        vld1.8 {d0[1]}, [Y]!
        vld1.8 {d0[2]}, [Y]!
        vld1.8 {d0[3]}, [Y]!
3:
    tst N, #2
    beq 4f
        vld1.8 {d4[4]}, [U]!
        vld1.8 {d4[5]}, [U]!
        vld1.8 {d5[4]}, [V]!
        vld1.8 {d5[5]}, [V]!
        vld1.8 {d0[4]}, [Y]!
        vld1.8 {d0[5]}, [Y]!
4:
    tst N, #1
    beq 5f
        vld1.8 {d4[6]}, [U]!
        vld1.8 {d5[6]}, [V]!
        vld1.8 {d0[6]}, [Y]!
5:
    vaddw.u8 q3, q1, d4
    vaddw.u8 q4, q1, d5
    vmull.s16 q10, d6, d1[1]
    vmlal.s16 q10, d8, d1[2]
    vmull.s16 q11, d7, d1[1]
    vmlal.s16 q11, d9, d1[2]
    vmull.s16 q12, d8, d1[0]
    vmull.s16 q13, d9, d1[0]
    vmull.s16 q14, d6, d1[3]
    vmull.s16 q15, d7, d1[3]
    vrshrn.s32 d20, q10, #15
    vrshrn.s32 d21, q11, #15
    vrshrn.s32 d24, q12, #14
    vrshrn.s32 d25, q13, #14
    vrshrn.s32 d28, q14, #14
    vrshrn.s32 d29, q15, #14
    vaddw.u8 q10, q10, d0
    vaddw.u8 q12, q12, d0
    vaddw.u8 q14, q14, d0
    vqmovun.s16 d12, q10
    vqmovun.s16 d11, q12
    vqmovun.s16 d13, q14
    tst N, #4
    beq 6f
            vst4.8 {d10[0], d11[0], d12[0], d13[0]}, [RGB]!
            vst4.8 {d10[1], d11[1], d12[1], d13[1]}, [RGB]!
            vst4.8 {d10[2], d11[2], d12[2], d13[2]}, [RGB]!
            vst4.8 {d10[3], d11[3], d12[3], d13[3]}, [RGB]!
6:
    tst N, #2
    beq 7f
            vst4.8 {d10[4], d11[4], d12[4], d13[4]}, [RGB]!
            vst4.8 {d10[5], d11[5], d12[5], d13[5]}, [RGB]!
7:
    tst N, #1
    beq 8f
            vst4.8 {d10[6], d11[6], d12[6], d13[6]}, [RGB]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq input_row
.unreq INPUT_ROW
.unreq output_buf
.unreq OUTPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq input_buf0
.unreq INPUT_BUF0
.unreq input_buf1
.unreq INPUT_BUF1
.unreq input_buf2
.unreq INPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc



# 1584 "src/simd/jsimd_arm_neon.s"













.balign 16
jsimd_extrgb_ycc_neon_consts:
    .short 19595, 38470, 7471, 11059
    .short 21709, 32768, 27439, 5329
    .short 32767, 128, 32767, 128
    .short 32767, 128, 32767, 128


    @.func _jsimd_extrgb_ycc_convert_neon
    .globl _jsimd_extrgb_ycc_convert_neon
_jsimd_extrgb_ycc_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    OUTPUT_BUF .req r2
    OUTPUT_ROW .req r3
    NUM_ROWS .req r4

    OUTPUT_BUF0 .req r5
    OUTPUT_BUF1 .req r6
    OUTPUT_BUF2 .req OUTPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_extrgb_ycc_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr OUTPUT_BUF0, [OUTPUT_BUF]
    ldr OUTPUT_BUF1, [OUTPUT_BUF, #4]
    ldr OUTPUT_BUF2, [OUTPUT_BUF, #8]
.unreq output_buf
.unreq OUTPUT_BUF


    vpush {d8-d15}


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [OUTPUT_BUF0, OUTPUT_ROW, lsl #2]
    ldr U, [OUTPUT_BUF1, OUTPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [OUTPUT_BUF2, OUTPUT_ROW, lsl #2]
    add OUTPUT_ROW, OUTPUT_ROW, #1
    ldr RGB, [INPUT_BUF], #4


    subs N, N, #8
    blt 3f
            vld3.8 {d10, d11, d12}, [RGB]!
            pld [RGB, #128]
    vmovl.u8 q2, d10
    vmovl.u8 q3, d11
    vmovl.u8 q4, d12
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    blt 2f
1:
      vrshrn.u32 d20, q7, #16
      vrshrn.u32 d21, q8, #16
      vshrn.u32 d22, q9, #16
    vrev64.32 q9, q1
      vshrn.u32 d23, q13, #16
    vrev64.32 q13, q1
      vshrn.u32 d24, q14, #16
      vshrn.u32 d25, q15, #16
            vld3.8 {d10, d11, d12}, [RGB]!
            pld [RGB, #128]
      vmovn.u16 d20, q10
    vmovl.u8 q2, d10
      vmovn.u16 d21, q11
    vmovl.u8 q3, d11
      vmovn.u16 d22, q12
    vmovl.u8 q4, d12
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
      vst1.8 {d20}, [Y]!
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
      vst1.8 {d21}, [U]!
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
      vst1.8 {d22}, [V]!
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
        vst1.8 {d20}, [Y]!
        vst1.8 {d21}, [U]!
        vst1.8 {d22}, [V]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
            vld3.8 {d10[0], d11[0], d12[0]}, [RGB]!
            vld3.8 {d10[1], d11[1], d12[1]}, [RGB]!
            vld3.8 {d10[2], d11[2], d12[2]}, [RGB]!
            vld3.8 {d10[3], d11[3], d12[3]}, [RGB]!
3:
    tst N, #2
    beq 4f
            vld3.8 {d10[4], d11[4], d12[4]}, [RGB]!
            vld3.8 {d10[5], d11[5], d12[5]}, [RGB]!
4:
    tst N, #1
    beq 5f
            vld3.8 {d10[6], d11[6], d12[6]}, [RGB]!
5:
    vmovl.u8 q2, d10
    vmovl.u8 q3, d11
    vmovl.u8 q4, d12
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
    tst N, #4
    beq 6f
        vst1.8 {d20[0]}, [Y]!
        vst1.8 {d20[1]}, [Y]!
        vst1.8 {d20[2]}, [Y]!
        vst1.8 {d20[3]}, [Y]!
        vst1.8 {d21[0]}, [U]!
        vst1.8 {d21[1]}, [U]!
        vst1.8 {d21[2]}, [U]!
        vst1.8 {d21[3]}, [U]!
        vst1.8 {d22[0]}, [V]!
        vst1.8 {d22[1]}, [V]!
        vst1.8 {d22[2]}, [V]!
        vst1.8 {d22[3]}, [V]!
6:
    tst N, #2
    beq 7f
        vst1.8 {d20[4]}, [Y]!
        vst1.8 {d20[5]}, [Y]!
        vst1.8 {d21[4]}, [U]!
        vst1.8 {d21[5]}, [U]!
        vst1.8 {d22[4]}, [V]!
        vst1.8 {d22[5]}, [V]!
7:
    tst N, #1
    beq 8f
        vst1.8 {d20[6]}, [Y]!
        vst1.8 {d21[6]}, [U]!
        vst1.8 {d22[6]}, [V]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq output_row
.unreq OUTPUT_ROW
.unreq input_buf
.unreq INPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq output_buf0
.unreq OUTPUT_BUF0
.unreq output_buf1
.unreq OUTPUT_BUF1
.unreq output_buf2
.unreq OUTPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc











.balign 16
jsimd_extbgr_ycc_neon_consts:
    .short 19595, 38470, 7471, 11059
    .short 21709, 32768, 27439, 5329
    .short 32767, 128, 32767, 128
    .short 32767, 128, 32767, 128


    @.func _jsimd_extbgr_ycc_convert_neon
    .globl _jsimd_extbgr_ycc_convert_neon
_jsimd_extbgr_ycc_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    OUTPUT_BUF .req r2
    OUTPUT_ROW .req r3
    NUM_ROWS .req r4

    OUTPUT_BUF0 .req r5
    OUTPUT_BUF1 .req r6
    OUTPUT_BUF2 .req OUTPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_extbgr_ycc_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr OUTPUT_BUF0, [OUTPUT_BUF]
    ldr OUTPUT_BUF1, [OUTPUT_BUF, #4]
    ldr OUTPUT_BUF2, [OUTPUT_BUF, #8]
.unreq output_buf
.unreq OUTPUT_BUF


    vpush {d8-d15}


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [OUTPUT_BUF0, OUTPUT_ROW, lsl #2]
    ldr U, [OUTPUT_BUF1, OUTPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [OUTPUT_BUF2, OUTPUT_ROW, lsl #2]
    add OUTPUT_ROW, OUTPUT_ROW, #1
    ldr RGB, [INPUT_BUF], #4


    subs N, N, #8
    blt 3f
            vld3.8 {d10, d11, d12}, [RGB]!
            pld [RGB, #128]
    vmovl.u8 q2, d12
    vmovl.u8 q3, d11
    vmovl.u8 q4, d10
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    blt 2f
1:
      vrshrn.u32 d20, q7, #16
      vrshrn.u32 d21, q8, #16
      vshrn.u32 d22, q9, #16
    vrev64.32 q9, q1
      vshrn.u32 d23, q13, #16
    vrev64.32 q13, q1
      vshrn.u32 d24, q14, #16
      vshrn.u32 d25, q15, #16
            vld3.8 {d10, d11, d12}, [RGB]!
            pld [RGB, #128]
      vmovn.u16 d20, q10
    vmovl.u8 q2, d12
      vmovn.u16 d21, q11
    vmovl.u8 q3, d11
      vmovn.u16 d22, q12
    vmovl.u8 q4, d10
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
      vst1.8 {d20}, [Y]!
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
      vst1.8 {d21}, [U]!
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
      vst1.8 {d22}, [V]!
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
        vst1.8 {d20}, [Y]!
        vst1.8 {d21}, [U]!
        vst1.8 {d22}, [V]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
            vld3.8 {d10[0], d11[0], d12[0]}, [RGB]!
            vld3.8 {d10[1], d11[1], d12[1]}, [RGB]!
            vld3.8 {d10[2], d11[2], d12[2]}, [RGB]!
            vld3.8 {d10[3], d11[3], d12[3]}, [RGB]!
3:
    tst N, #2
    beq 4f
            vld3.8 {d10[4], d11[4], d12[4]}, [RGB]!
            vld3.8 {d10[5], d11[5], d12[5]}, [RGB]!
4:
    tst N, #1
    beq 5f
            vld3.8 {d10[6], d11[6], d12[6]}, [RGB]!
5:
    vmovl.u8 q2, d12
    vmovl.u8 q3, d11
    vmovl.u8 q4, d10
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
    tst N, #4
    beq 6f
        vst1.8 {d20[0]}, [Y]!
        vst1.8 {d20[1]}, [Y]!
        vst1.8 {d20[2]}, [Y]!
        vst1.8 {d20[3]}, [Y]!
        vst1.8 {d21[0]}, [U]!
        vst1.8 {d21[1]}, [U]!
        vst1.8 {d21[2]}, [U]!
        vst1.8 {d21[3]}, [U]!
        vst1.8 {d22[0]}, [V]!
        vst1.8 {d22[1]}, [V]!
        vst1.8 {d22[2]}, [V]!
        vst1.8 {d22[3]}, [V]!
6:
    tst N, #2
    beq 7f
        vst1.8 {d20[4]}, [Y]!
        vst1.8 {d20[5]}, [Y]!
        vst1.8 {d21[4]}, [U]!
        vst1.8 {d21[5]}, [U]!
        vst1.8 {d22[4]}, [V]!
        vst1.8 {d22[5]}, [V]!
7:
    tst N, #1
    beq 8f
        vst1.8 {d20[6]}, [Y]!
        vst1.8 {d21[6]}, [U]!
        vst1.8 {d22[6]}, [V]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq output_row
.unreq OUTPUT_ROW
.unreq input_buf
.unreq INPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq output_buf0
.unreq OUTPUT_BUF0
.unreq output_buf1
.unreq OUTPUT_BUF1
.unreq output_buf2
.unreq OUTPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc











.balign 16
jsimd_extrgbx_ycc_neon_consts:
    .short 19595, 38470, 7471, 11059
    .short 21709, 32768, 27439, 5329
    .short 32767, 128, 32767, 128
    .short 32767, 128, 32767, 128


    @.func _jsimd_extrgbx_ycc_convert_neon
    .globl _jsimd_extrgbx_ycc_convert_neon
_jsimd_extrgbx_ycc_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    OUTPUT_BUF .req r2
    OUTPUT_ROW .req r3
    NUM_ROWS .req r4

    OUTPUT_BUF0 .req r5
    OUTPUT_BUF1 .req r6
    OUTPUT_BUF2 .req OUTPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_extrgbx_ycc_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr OUTPUT_BUF0, [OUTPUT_BUF]
    ldr OUTPUT_BUF1, [OUTPUT_BUF, #4]
    ldr OUTPUT_BUF2, [OUTPUT_BUF, #8]
.unreq output_buf
.unreq OUTPUT_BUF


    vpush {d8-d15}


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [OUTPUT_BUF0, OUTPUT_ROW, lsl #2]
    ldr U, [OUTPUT_BUF1, OUTPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [OUTPUT_BUF2, OUTPUT_ROW, lsl #2]
    add OUTPUT_ROW, OUTPUT_ROW, #1
    ldr RGB, [INPUT_BUF], #4


    subs N, N, #8
    blt 3f
            vld4.8 {d10, d11, d12, d13}, [RGB]!
            pld [RGB, #128]
    vmovl.u8 q2, d10
    vmovl.u8 q3, d11
    vmovl.u8 q4, d12
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    blt 2f
1:
      vrshrn.u32 d20, q7, #16
      vrshrn.u32 d21, q8, #16
      vshrn.u32 d22, q9, #16
    vrev64.32 q9, q1
      vshrn.u32 d23, q13, #16
    vrev64.32 q13, q1
      vshrn.u32 d24, q14, #16
      vshrn.u32 d25, q15, #16
            vld4.8 {d10, d11, d12, d13}, [RGB]!
            pld [RGB, #128]
      vmovn.u16 d20, q10
    vmovl.u8 q2, d10
      vmovn.u16 d21, q11
    vmovl.u8 q3, d11
      vmovn.u16 d22, q12
    vmovl.u8 q4, d12
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
      vst1.8 {d20}, [Y]!
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
      vst1.8 {d21}, [U]!
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
      vst1.8 {d22}, [V]!
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
        vst1.8 {d20}, [Y]!
        vst1.8 {d21}, [U]!
        vst1.8 {d22}, [V]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
            vld4.8 {d10[0], d11[0], d12[0], d13[0]}, [RGB]!
            vld4.8 {d10[1], d11[1], d12[1], d13[1]}, [RGB]!
            vld4.8 {d10[2], d11[2], d12[2], d13[2]}, [RGB]!
            vld4.8 {d10[3], d11[3], d12[3], d13[3]}, [RGB]!
3:
    tst N, #2
    beq 4f
            vld4.8 {d10[4], d11[4], d12[4], d13[4]}, [RGB]!
            vld4.8 {d10[5], d11[5], d12[5], d13[5]}, [RGB]!
4:
    tst N, #1
    beq 5f
            vld4.8 {d10[6], d11[6], d12[6], d13[6]}, [RGB]!
5:
    vmovl.u8 q2, d10
    vmovl.u8 q3, d11
    vmovl.u8 q4, d12
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
    tst N, #4
    beq 6f
        vst1.8 {d20[0]}, [Y]!
        vst1.8 {d20[1]}, [Y]!
        vst1.8 {d20[2]}, [Y]!
        vst1.8 {d20[3]}, [Y]!
        vst1.8 {d21[0]}, [U]!
        vst1.8 {d21[1]}, [U]!
        vst1.8 {d21[2]}, [U]!
        vst1.8 {d21[3]}, [U]!
        vst1.8 {d22[0]}, [V]!
        vst1.8 {d22[1]}, [V]!
        vst1.8 {d22[2]}, [V]!
        vst1.8 {d22[3]}, [V]!
6:
    tst N, #2
    beq 7f
        vst1.8 {d20[4]}, [Y]!
        vst1.8 {d20[5]}, [Y]!
        vst1.8 {d21[4]}, [U]!
        vst1.8 {d21[5]}, [U]!
        vst1.8 {d22[4]}, [V]!
        vst1.8 {d22[5]}, [V]!
7:
    tst N, #1
    beq 8f
        vst1.8 {d20[6]}, [Y]!
        vst1.8 {d21[6]}, [U]!
        vst1.8 {d22[6]}, [V]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq output_row
.unreq OUTPUT_ROW
.unreq input_buf
.unreq INPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq output_buf0
.unreq OUTPUT_BUF0
.unreq output_buf1
.unreq OUTPUT_BUF1
.unreq output_buf2
.unreq OUTPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc











.balign 16
jsimd_extbgrx_ycc_neon_consts:
    .short 19595, 38470, 7471, 11059
    .short 21709, 32768, 27439, 5329
    .short 32767, 128, 32767, 128
    .short 32767, 128, 32767, 128


    @.func _jsimd_extbgrx_ycc_convert_neon
    .globl _jsimd_extbgrx_ycc_convert_neon
_jsimd_extbgrx_ycc_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    OUTPUT_BUF .req r2
    OUTPUT_ROW .req r3
    NUM_ROWS .req r4

    OUTPUT_BUF0 .req r5
    OUTPUT_BUF1 .req r6
    OUTPUT_BUF2 .req OUTPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_extbgrx_ycc_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr OUTPUT_BUF0, [OUTPUT_BUF]
    ldr OUTPUT_BUF1, [OUTPUT_BUF, #4]
    ldr OUTPUT_BUF2, [OUTPUT_BUF, #8]
.unreq output_buf
.unreq OUTPUT_BUF


    vpush {d8-d15}


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [OUTPUT_BUF0, OUTPUT_ROW, lsl #2]
    ldr U, [OUTPUT_BUF1, OUTPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [OUTPUT_BUF2, OUTPUT_ROW, lsl #2]
    add OUTPUT_ROW, OUTPUT_ROW, #1
    ldr RGB, [INPUT_BUF], #4


    subs N, N, #8
    blt 3f
            vld4.8 {d10, d11, d12, d13}, [RGB]!
            pld [RGB, #128]
    vmovl.u8 q2, d12
    vmovl.u8 q3, d11
    vmovl.u8 q4, d10
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    blt 2f
1:
      vrshrn.u32 d20, q7, #16
      vrshrn.u32 d21, q8, #16
      vshrn.u32 d22, q9, #16
    vrev64.32 q9, q1
      vshrn.u32 d23, q13, #16
    vrev64.32 q13, q1
      vshrn.u32 d24, q14, #16
      vshrn.u32 d25, q15, #16
            vld4.8 {d10, d11, d12, d13}, [RGB]!
            pld [RGB, #128]
      vmovn.u16 d20, q10
    vmovl.u8 q2, d12
      vmovn.u16 d21, q11
    vmovl.u8 q3, d11
      vmovn.u16 d22, q12
    vmovl.u8 q4, d10
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
      vst1.8 {d20}, [Y]!
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
      vst1.8 {d21}, [U]!
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
      vst1.8 {d22}, [V]!
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
        vst1.8 {d20}, [Y]!
        vst1.8 {d21}, [U]!
        vst1.8 {d22}, [V]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
            vld4.8 {d10[0], d11[0], d12[0], d13[0]}, [RGB]!
            vld4.8 {d10[1], d11[1], d12[1], d13[1]}, [RGB]!
            vld4.8 {d10[2], d11[2], d12[2], d13[2]}, [RGB]!
            vld4.8 {d10[3], d11[3], d12[3], d13[3]}, [RGB]!
3:
    tst N, #2
    beq 4f
            vld4.8 {d10[4], d11[4], d12[4], d13[4]}, [RGB]!
            vld4.8 {d10[5], d11[5], d12[5], d13[5]}, [RGB]!
4:
    tst N, #1
    beq 5f
            vld4.8 {d10[6], d11[6], d12[6], d13[6]}, [RGB]!
5:
    vmovl.u8 q2, d12
    vmovl.u8 q3, d11
    vmovl.u8 q4, d10
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
    tst N, #4
    beq 6f
        vst1.8 {d20[0]}, [Y]!
        vst1.8 {d20[1]}, [Y]!
        vst1.8 {d20[2]}, [Y]!
        vst1.8 {d20[3]}, [Y]!
        vst1.8 {d21[0]}, [U]!
        vst1.8 {d21[1]}, [U]!
        vst1.8 {d21[2]}, [U]!
        vst1.8 {d21[3]}, [U]!
        vst1.8 {d22[0]}, [V]!
        vst1.8 {d22[1]}, [V]!
        vst1.8 {d22[2]}, [V]!
        vst1.8 {d22[3]}, [V]!
6:
    tst N, #2
    beq 7f
        vst1.8 {d20[4]}, [Y]!
        vst1.8 {d20[5]}, [Y]!
        vst1.8 {d21[4]}, [U]!
        vst1.8 {d21[5]}, [U]!
        vst1.8 {d22[4]}, [V]!
        vst1.8 {d22[5]}, [V]!
7:
    tst N, #1
    beq 8f
        vst1.8 {d20[6]}, [Y]!
        vst1.8 {d21[6]}, [U]!
        vst1.8 {d22[6]}, [V]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq output_row
.unreq OUTPUT_ROW
.unreq input_buf
.unreq INPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq output_buf0
.unreq OUTPUT_BUF0
.unreq output_buf1
.unreq OUTPUT_BUF1
.unreq output_buf2
.unreq OUTPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc











.balign 16
jsimd_extxbgr_ycc_neon_consts:
    .short 19595, 38470, 7471, 11059
    .short 21709, 32768, 27439, 5329
    .short 32767, 128, 32767, 128
    .short 32767, 128, 32767, 128


    @.func _jsimd_extxbgr_ycc_convert_neon
    .globl _jsimd_extxbgr_ycc_convert_neon
_jsimd_extxbgr_ycc_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    OUTPUT_BUF .req r2
    OUTPUT_ROW .req r3
    NUM_ROWS .req r4

    OUTPUT_BUF0 .req r5
    OUTPUT_BUF1 .req r6
    OUTPUT_BUF2 .req OUTPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_extxbgr_ycc_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr OUTPUT_BUF0, [OUTPUT_BUF]
    ldr OUTPUT_BUF1, [OUTPUT_BUF, #4]
    ldr OUTPUT_BUF2, [OUTPUT_BUF, #8]
.unreq output_buf
.unreq OUTPUT_BUF


    vpush {d8-d15}


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [OUTPUT_BUF0, OUTPUT_ROW, lsl #2]
    ldr U, [OUTPUT_BUF1, OUTPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [OUTPUT_BUF2, OUTPUT_ROW, lsl #2]
    add OUTPUT_ROW, OUTPUT_ROW, #1
    ldr RGB, [INPUT_BUF], #4


    subs N, N, #8
    blt 3f
            vld4.8 {d10, d11, d12, d13}, [RGB]!
            pld [RGB, #128]
    vmovl.u8 q2, d13
    vmovl.u8 q3, d12
    vmovl.u8 q4, d11
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    blt 2f
1:
      vrshrn.u32 d20, q7, #16
      vrshrn.u32 d21, q8, #16
      vshrn.u32 d22, q9, #16
    vrev64.32 q9, q1
      vshrn.u32 d23, q13, #16
    vrev64.32 q13, q1
      vshrn.u32 d24, q14, #16
      vshrn.u32 d25, q15, #16
            vld4.8 {d10, d11, d12, d13}, [RGB]!
            pld [RGB, #128]
      vmovn.u16 d20, q10
    vmovl.u8 q2, d13
      vmovn.u16 d21, q11
    vmovl.u8 q3, d12
      vmovn.u16 d22, q12
    vmovl.u8 q4, d11
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
      vst1.8 {d20}, [Y]!
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
      vst1.8 {d21}, [U]!
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
      vst1.8 {d22}, [V]!
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
        vst1.8 {d20}, [Y]!
        vst1.8 {d21}, [U]!
        vst1.8 {d22}, [V]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
            vld4.8 {d10[0], d11[0], d12[0], d13[0]}, [RGB]!
            vld4.8 {d10[1], d11[1], d12[1], d13[1]}, [RGB]!
            vld4.8 {d10[2], d11[2], d12[2], d13[2]}, [RGB]!
            vld4.8 {d10[3], d11[3], d12[3], d13[3]}, [RGB]!
3:
    tst N, #2
    beq 4f
            vld4.8 {d10[4], d11[4], d12[4], d13[4]}, [RGB]!
            vld4.8 {d10[5], d11[5], d12[5], d13[5]}, [RGB]!
4:
    tst N, #1
    beq 5f
            vld4.8 {d10[6], d11[6], d12[6], d13[6]}, [RGB]!
5:
    vmovl.u8 q2, d13
    vmovl.u8 q3, d12
    vmovl.u8 q4, d11
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
    tst N, #4
    beq 6f
        vst1.8 {d20[0]}, [Y]!
        vst1.8 {d20[1]}, [Y]!
        vst1.8 {d20[2]}, [Y]!
        vst1.8 {d20[3]}, [Y]!
        vst1.8 {d21[0]}, [U]!
        vst1.8 {d21[1]}, [U]!
        vst1.8 {d21[2]}, [U]!
        vst1.8 {d21[3]}, [U]!
        vst1.8 {d22[0]}, [V]!
        vst1.8 {d22[1]}, [V]!
        vst1.8 {d22[2]}, [V]!
        vst1.8 {d22[3]}, [V]!
6:
    tst N, #2
    beq 7f
        vst1.8 {d20[4]}, [Y]!
        vst1.8 {d20[5]}, [Y]!
        vst1.8 {d21[4]}, [U]!
        vst1.8 {d21[5]}, [U]!
        vst1.8 {d22[4]}, [V]!
        vst1.8 {d22[5]}, [V]!
7:
    tst N, #1
    beq 8f
        vst1.8 {d20[6]}, [Y]!
        vst1.8 {d21[6]}, [U]!
        vst1.8 {d22[6]}, [V]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq output_row
.unreq OUTPUT_ROW
.unreq input_buf
.unreq INPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq output_buf0
.unreq OUTPUT_BUF0
.unreq output_buf1
.unreq OUTPUT_BUF1
.unreq output_buf2
.unreq OUTPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc











.balign 16
jsimd_extxrgb_ycc_neon_consts:
    .short 19595, 38470, 7471, 11059
    .short 21709, 32768, 27439, 5329
    .short 32767, 128, 32767, 128
    .short 32767, 128, 32767, 128


    @.func _jsimd_extxrgb_ycc_convert_neon
    .globl _jsimd_extxrgb_ycc_convert_neon
_jsimd_extxrgb_ycc_convert_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    OUTPUT_WIDTH .req r0
    INPUT_BUF .req r1
    OUTPUT_BUF .req r2
    OUTPUT_ROW .req r3
    NUM_ROWS .req r4

    OUTPUT_BUF0 .req r5
    OUTPUT_BUF1 .req r6
    OUTPUT_BUF2 .req OUTPUT_BUF

    RGB .req r7
    Y .req r8
    U .req r9
    V .req r10
    N .req ip


    adr ip, jsimd_extxrgb_ycc_neon_consts
    vld1.16 {d0, d1, d2, d3}, [ip, :128]


    push {r4, r5, r6, r7, r8, r9, r10, lr}
    ldr NUM_ROWS, [sp, #(4 * 8)]
    ldr OUTPUT_BUF0, [OUTPUT_BUF]
    ldr OUTPUT_BUF1, [OUTPUT_BUF, #4]
    ldr OUTPUT_BUF2, [OUTPUT_BUF, #8]
.unreq output_buf
.unreq OUTPUT_BUF


    vpush {d8-d15}


    cmp NUM_ROWS, #1
    blt 9f
0:
    ldr Y, [OUTPUT_BUF0, OUTPUT_ROW, lsl #2]
    ldr U, [OUTPUT_BUF1, OUTPUT_ROW, lsl #2]
    mov N, OUTPUT_WIDTH
    ldr V, [OUTPUT_BUF2, OUTPUT_ROW, lsl #2]
    add OUTPUT_ROW, OUTPUT_ROW, #1
    ldr RGB, [INPUT_BUF], #4


    subs N, N, #8
    blt 3f
            vld4.8 {d10, d11, d12, d13}, [RGB]!
            pld [RGB, #128]
    vmovl.u8 q2, d11
    vmovl.u8 q3, d12
    vmovl.u8 q4, d13
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    blt 2f
1:
      vrshrn.u32 d20, q7, #16
      vrshrn.u32 d21, q8, #16
      vshrn.u32 d22, q9, #16
    vrev64.32 q9, q1
      vshrn.u32 d23, q13, #16
    vrev64.32 q13, q1
      vshrn.u32 d24, q14, #16
      vshrn.u32 d25, q15, #16
            vld4.8 {d10, d11, d12, d13}, [RGB]!
            pld [RGB, #128]
      vmovn.u16 d20, q10
    vmovl.u8 q2, d11
      vmovn.u16 d21, q11
    vmovl.u8 q3, d12
      vmovn.u16 d22, q12
    vmovl.u8 q4, d13
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
      vst1.8 {d20}, [Y]!
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
      vst1.8 {d21}, [U]!
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
      vst1.8 {d22}, [V]!
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    subs N, N, #8
    bge 1b
2:
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
        vst1.8 {d20}, [Y]!
        vst1.8 {d21}, [U]!
        vst1.8 {d22}, [V]!
    tst N, #7
    beq 8f
3:
    tst N, #4
    beq 3f
            vld4.8 {d10[0], d11[0], d12[0], d13[0]}, [RGB]!
            vld4.8 {d10[1], d11[1], d12[1], d13[1]}, [RGB]!
            vld4.8 {d10[2], d11[2], d12[2], d13[2]}, [RGB]!
            vld4.8 {d10[3], d11[3], d12[3], d13[3]}, [RGB]!
3:
    tst N, #2
    beq 4f
            vld4.8 {d10[4], d11[4], d12[4], d13[4]}, [RGB]!
            vld4.8 {d10[5], d11[5], d12[5], d13[5]}, [RGB]!
4:
    tst N, #1
    beq 5f
            vld4.8 {d10[6], d11[6], d12[6], d13[6]}, [RGB]!
5:
    vmovl.u8 q2, d11
    vmovl.u8 q3, d12
    vmovl.u8 q4, d13
    vmull.u16 q7, d4, d0[0]
    vmlal.u16 q7, d6, d0[1]
    vmlal.u16 q7, d8, d0[2]
    vmull.u16 q8, d5, d0[0]
    vmlal.u16 q8, d7, d0[1]
    vmlal.u16 q8, d9, d0[2]
    vrev64.32 q9, q1
    vrev64.32 q13, q1
    vmlsl.u16 q9, d4, d0[3]
    vmlsl.u16 q9, d6, d1[0]
    vmlal.u16 q9, d8, d1[1]
    vmlsl.u16 q13, d5, d0[3]
    vmlsl.u16 q13, d7, d1[0]
    vmlal.u16 q13, d9, d1[1]
    vrev64.32 q14, q1
    vrev64.32 q15, q1
    vmlal.u16 q14, d4, d1[1]
    vmlsl.u16 q14, d6, d1[2]
    vmlsl.u16 q14, d8, d1[3]
    vmlal.u16 q15, d5, d1[1]
    vmlsl.u16 q15, d7, d1[2]
    vmlsl.u16 q15, d9, d1[3]
    vrshrn.u32 d20, q7, #16
    vrshrn.u32 d21, q8, #16
    vshrn.u32 d22, q9, #16
    vshrn.u32 d23, q13, #16
    vshrn.u32 d24, q14, #16
    vshrn.u32 d25, q15, #16
    vmovn.u16 d20, q10
    vmovn.u16 d21, q11
    vmovn.u16 d22, q12
    tst N, #4
    beq 6f
        vst1.8 {d20[0]}, [Y]!
        vst1.8 {d20[1]}, [Y]!
        vst1.8 {d20[2]}, [Y]!
        vst1.8 {d20[3]}, [Y]!
        vst1.8 {d21[0]}, [U]!
        vst1.8 {d21[1]}, [U]!
        vst1.8 {d21[2]}, [U]!
        vst1.8 {d21[3]}, [U]!
        vst1.8 {d22[0]}, [V]!
        vst1.8 {d22[1]}, [V]!
        vst1.8 {d22[2]}, [V]!
        vst1.8 {d22[3]}, [V]!
6:
    tst N, #2
    beq 7f
        vst1.8 {d20[4]}, [Y]!
        vst1.8 {d20[5]}, [Y]!
        vst1.8 {d21[4]}, [U]!
        vst1.8 {d21[5]}, [U]!
        vst1.8 {d22[4]}, [V]!
        vst1.8 {d22[5]}, [V]!
7:
    tst N, #1
    beq 8f
        vst1.8 {d20[6]}, [Y]!
        vst1.8 {d21[6]}, [U]!
        vst1.8 {d22[6]}, [V]!
8:
    subs NUM_ROWS, NUM_ROWS, #1
    bgt 0b
9:

    vpop {d8-d15}
    pop {r4, r5, r6, r7, r8, r9, r10, pc}

.unreq output_width
.unreq OUTPUT_WIDTH
.unreq output_row
.unreq OUTPUT_ROW
.unreq input_buf
.unreq INPUT_BUF
.unreq num_rows
.unreq NUM_ROWS
.unreq output_buf0
.unreq OUTPUT_BUF0
.unreq output_buf1
.unreq OUTPUT_BUF1
.unreq output_buf2
.unreq OUTPUT_BUF2
.unreq rgb
.unreq RGB
.unreq y
.unreq Y
.unreq u
.unreq U
.unreq v
.unreq V
.unreq n
.unreq N
@.endfunc



# 1891 "src/simd/jsimd_arm_neon.s"

    @.func _jsimd_convsamp_neon
    .globl _jsimd_convsamp_neon
_jsimd_convsamp_neon:
# 56 "src/simd/jsimd_arm_neon.s"
    SAMPLE_DATA .req r0
    START_COL .req r1
    WORKSPACE .req r2
    TMP1 .req r3
    TMP2 .req r4
    TMP3 .req r5
    TMP4 .req ip

    push {r4, r5}
    vmov.u8 d0, #128

    ldmia SAMPLE_DATA!, {TMP1, TMP2, TMP3, TMP4}
    add TMP1, TMP1, START_COL
    add TMP2, TMP2, START_COL
    add TMP3, TMP3, START_COL
    add TMP4, TMP4, START_COL
    vld1.8 {d16}, [TMP1]
    vsubl.u8 q8, d16, d0
    vld1.8 {d18}, [TMP2]
    vsubl.u8 q9, d18, d0
    vld1.8 {d20}, [TMP3]
    vsubl.u8 q10, d20, d0
    vld1.8 {d22}, [TMP4]
    ldmia SAMPLE_DATA!, {TMP1, TMP2, TMP3, TMP4}
    vsubl.u8 q11, d22, d0
    vst1.16 {d16, d17, d18, d19}, [WORKSPACE, :128]!
    add TMP1, TMP1, START_COL
    add TMP2, TMP2, START_COL
    vst1.16 {d20, d21, d22, d23}, [WORKSPACE, :128]!
    add TMP3, TMP3, START_COL
    add TMP4, TMP4, START_COL
    vld1.8 {d24}, [TMP1]
    vsubl.u8 q12, d24, d0
    vld1.8 {d26}, [TMP2]
    vsubl.u8 q13, d26, d0
    vld1.8 {d28}, [TMP3]
    vsubl.u8 q14, d28, d0
    vld1.8 {d30}, [TMP4]
    vsubl.u8 q15, d30, d0
    vst1.16 {d24, d25, d26, d27}, [WORKSPACE, :128]!
    vst1.16 {d28, d29, d30, d31}, [WORKSPACE, :128]!
    pop {r4, r5}
    bx lr

.unreq sample_data
.unreq SAMPLE_DATA
.unreq start_col
.unreq START_COL
.unreq workspace
.unreq WORKSPACE
.unreq tmp1
.unreq TMP1
.unreq tmp2
.unreq TMP2
.unreq tmp3
.unreq TMP3
.unreq tmp4
.unreq TMP4
@.endfunc
# 1965 "src/simd/jsimd_arm_neon.s"
.balign 16
jsimd_fdct_ifast_neon_consts:
    .short (98 * 128)
    .short (139 * 128)
    .short (181 * 128)
    .short (334 * 128 - 256 * 128)


    @.func _jsimd_fdct_ifast_neon
    .globl _jsimd_fdct_ifast_neon
_jsimd_fdct_ifast_neon:
# 56 "src/simd/jsimd_arm_neon.s"

    DATA .req r0
    TMP .req ip

    vpush {d8-d15}


    adr TMP, jsimd_fdct_ifast_neon_consts
    vld1.16 {d0}, [TMP, :64]
# 1996 "src/simd/jsimd_arm_neon.s"
    vld1.16 {d16, d17, d18, d19}, [DATA, :128]!
    vld1.16 {d20, d21, d22, d23}, [DATA, :128]!
    vld1.16 {d24, d25, d26, d27}, [DATA, :128]!
    vld1.16 {d28, d29, d30, d31}, [DATA, :128]
    sub DATA, DATA, #(128 - 32)

    mov TMP, #2
1:

    vtrn.16 q12, q13
    vtrn.16 q10, q11
    vtrn.16 q8, q9
    vtrn.16 q14, q15
    vtrn.32 q9, q11
    vtrn.32 q13, q15
    vtrn.32 q8, q10
    vtrn.32 q12, q14
    vswp d30, d23
    vswp d24, d17
    vswp d26, d19

      vadd.s16 q2, q11, q12
    vswp d28, d21
      vsub.s16 q12, q11, q12
      vsub.s16 q6, q10, q13
      vadd.s16 q10, q10, q13
      vsub.s16 q7, q9, q14
      vadd.s16 q9, q9, q14
      vsub.s16 q1, q8, q15
      vadd.s16 q8, q8, q15
      vsub.s16 q4, q9, q10
      vsub.s16 q5, q8, q2
      vadd.s16 q3, q9, q10
      vadd.s16 q4, q4, q5
      vadd.s16 q2, q8, q2
      vqdmulh.s16 q4, q4, d0[2]
      vadd.s16 q11, q12, q6
      vadd.s16 q8, q2, q3
      vsub.s16 q12, q2, q3
      vadd.s16 q3, q6, q7
      vadd.s16 q7, q7, q1
      vqdmulh.s16 q3, q3, d0[2]
      vsub.s16 q6, q11, q7
      vadd.s16 q10, q5, q4
      vqdmulh.s16 q6, q6, d0[0]
      vsub.s16 q14, q5, q4
      vqdmulh.s16 q11, q11, d0[1]
      vqdmulh.s16 q5, q7, d0[3]
      vadd.s16 q4, q1, q3
      vsub.s16 q3, q1, q3
      vadd.s16 q7, q7, q6
      vadd.s16 q11, q11, q6
      vadd.s16 q7, q7, q5
      vadd.s16 q13, q3, q11
      vsub.s16 q11, q3, q11
      vadd.s16 q9, q4, q7
      vsub.s16 q15, q4, q7
    subs TMP, TMP, #1
    bne 1b


    vst1.16 {d16, d17, d18, d19}, [DATA, :128]!
    vst1.16 {d20, d21, d22, d23}, [DATA, :128]!
    vst1.16 {d24, d25, d26, d27}, [DATA, :128]!
    vst1.16 {d28, d29, d30, d31}, [DATA, :128]

    vpop {d8-d15}
    bx lr

.unreq data
.unreq DATA
.unreq tmp
.unreq TMP
@.endfunc
# 2084 "src/simd/jsimd_arm_neon.s"

    @.func _jsimd_quantize_neon
    .globl _jsimd_quantize_neon
_jsimd_quantize_neon:
# 56 "src/simd/jsimd_arm_neon.s"

    COEF_BLOCK .req r0
    DIVISORS .req r1
    WORKSPACE .req r2

    RECIPROCAL .req DIVISORS
    CORRECTION .req r3
    SHIFT .req ip
    LOOP_COUNT .req r4

    vld1.16 {d0, d1, d2, d3}, [WORKSPACE, :128]!
    vabs.s16 q12, q0
    add CORRECTION, DIVISORS, #(64 * 2)
    add SHIFT, DIVISORS, #(64 * 6)
    vld1.16 {d20, d21, d22, d23}, [CORRECTION, :128]!
    vabs.s16 q13, q1
    vld1.16 {d16, d17, d18, d19}, [RECIPROCAL, :128]!
    vadd.u16 q12, q12, q10
    vadd.u16 q13, q13, q11
    vmull.u16 q10, d24, d16
    vmull.u16 q11, d25, d17
    vmull.u16 q8, d26, d18
    vmull.u16 q9, d27, d19
    vld1.16 {d24, d25, d26, d27}, [SHIFT, :128]!
    vshrn.u32 d20, q10, #16
    vshrn.u32 d21, q11, #16
    vshrn.u32 d22, q8, #16
    vshrn.u32 d23, q9, #16
    vneg.s16 q12, q12
    vneg.s16 q13, q13
    vshr.s16 q2, q0, #15
    vshr.s16 q3, q1, #15
    vshl.u16 q14, q10, q12
    vshl.u16 q15, q11, q13

    push {r4, r5}
    mov LOOP_COUNT, #3
1:
    vld1.16 {d0, d1, d2, d3}, [WORKSPACE, :128]!
      veor.u16 q14, q14, q2
    vabs.s16 q12, q0
    vld1.16 {d20, d21, d22, d23}, [CORRECTION, :128]!
    vabs.s16 q13, q1
      veor.u16 q15, q15, q3
    vld1.16 {d16, d17, d18, d19}, [RECIPROCAL, :128]!
    vadd.u16 q12, q12, q10
    vadd.u16 q13, q13, q11
    vmull.u16 q10, d24, d16
    vmull.u16 q11, d25, d17
    vmull.u16 q8, d26, d18
    vmull.u16 q9, d27, d19
      vsub.u16 q14, q14, q2
    vld1.16 {d24, d25, d26, d27}, [SHIFT, :128]!
      vsub.u16 q15, q15, q3
    vshrn.u32 d20, q10, #16
    vshrn.u32 d21, q11, #16
      vst1.16 {d28, d29, d30, d31}, [COEF_BLOCK, :128]!
    vshrn.u32 d22, q8, #16
    vshrn.u32 d23, q9, #16
    vneg.s16 q12, q12
    vneg.s16 q13, q13
    vshr.s16 q2, q0, #15
    vshr.s16 q3, q1, #15
    vshl.u16 q14, q10, q12
    vshl.u16 q15, q11, q13
    subs LOOP_COUNT, LOOP_COUNT, #1
    bne 1b
    pop {r4, r5}

      veor.u16 q14, q14, q2
      veor.u16 q15, q15, q3
      vsub.u16 q14, q14, q2
      vsub.u16 q15, q15, q3
      vst1.16 {d28, d29, d30, d31}, [COEF_BLOCK, :128]!

    bx lr

.unreq coef_block
.unreq COEF_BLOCK
.unreq divisors
.unreq DIVISORS
.unreq workspace
.unreq WORKSPACE
.unreq reciprocal
.unreq RECIPROCAL
.unreq correction
.unreq CORRECTION
.unreq shift
.unreq SHIFT
.unreq loop_count
.unreq LOOP_COUNT
@.endfunc
# 2193 "src/simd/jsimd_arm_neon.s"
# 2219 "src/simd/jsimd_arm_neon.s"






    @.func _jsimd_h2v1_fancy_upsample_neon
    .globl _jsimd_h2v1_fancy_upsample_neon
_jsimd_h2v1_fancy_upsample_neon:
# 56 "src/simd/jsimd_arm_neon.s"

    MAX_V_SAMP_FACTOR .req r0
    DOWNSAMPLED_WIDTH .req r1
    INPUT_DATA .req r2
    OUTPUT_DATA_PTR .req r3
    OUTPUT_DATA .req OUTPUT_DATA_PTR

    OUTPTR .req r4
    INPTR .req r5
    WIDTH .req ip
    TMP .req lr

    push {r4, r5, r6, lr}
    vpush {d8-d15}

    ldr OUTPUT_DATA, [OUTPUT_DATA_PTR]
    cmp MAX_V_SAMP_FACTOR, #0
    ble 99f


    vmov.u8 d28, #3
    vmov.u16 q15, #1
11:
    ldr INPTR, [INPUT_DATA], #4
    ldr OUTPTR, [OUTPUT_DATA], #4
    mov WIDTH, DOWNSAMPLED_WIDTH

    sub WIDTH, WIDTH, #1
    add OUTPTR, OUTPTR, #1
    ldrb TMP, [INPTR, WIDTH]
    strb TMP, [OUTPTR, WIDTH, asl #1]
    ldrb TMP, [INPTR], #1
    strb TMP, [OUTPTR, #-1]
    vmov.8 d3[7], TMP

    subs WIDTH, WIDTH, #32
    blt 5f
0:

    vld1.8 {q0}, [INPTR]!
    vmovl.u8 q8, d0
    vext.8 q2, q1, q0, #15
    vmovl.u8 q9, d1
    vaddw.u8 q10, q15, d4
    vaddw.u8 q11, q15, d5
    vmlal.u8 q8, d4, d28
    vmlal.u8 q9, d5, d28
    vmlal.u8 q10, d0, d28
    vmlal.u8 q11, d1, d28

        vld1.8 {q1}, [INPTR]!
    vrshrn.u16 d6, q8, #2
    vrshrn.u16 d7, q9, #2
    vshrn.u16 d8, q10, #2
    vshrn.u16 d9, q11, #2
        vmovl.u8 q8, d2
        vext.8 q2, q0, q1, #15
        vmovl.u8 q9, d3
        vaddw.u8 q10, q15, d4
        vaddw.u8 q11, q15, d5
        vmlal.u8 q8, d4, d28
        vmlal.u8 q9, d5, d28
        vmlal.u8 q10, d2, d28
        vmlal.u8 q11, d3, d28
    vst2.8 {d6, d7, d8, d9}, [OUTPTR]!
        vrshrn.u16 d6, q8, #2
        vrshrn.u16 d7, q9, #2
        vshrn.u16 d8, q10, #2
        vshrn.u16 d9, q11, #2
        vst2.8 {d6, d7, d8, d9}, [OUTPTR]!
    subs WIDTH, WIDTH, #32
    bge 0b
5:
    adds WIDTH, WIDTH, #16
    blt 1f
0:
    vld1.8 {q0}, [INPTR]!
    vmovl.u8 q8, d0
    vext.8 q2, q1, q0, #15
    vmovl.u8 q9, d1
    vaddw.u8 q10, q15, d4
    vaddw.u8 q11, q15, d5
    vmlal.u8 q8, d4, d28
    vmlal.u8 q9, d5, d28
    vmlal.u8 q10, d0, d28
    vmlal.u8 q11, d1, d28
    vmov q1, q0
    vrshrn.u16 d6, q8, #2
    vrshrn.u16 d7, q9, #2
    vshrn.u16 d8, q10, #2
    vshrn.u16 d9, q11, #2
    vst2.8 {d6, d7, d8, d9}, [OUTPTR]!
    subs WIDTH, WIDTH, #16
1:
    adds WIDTH, WIDTH, #16
    beq 9f


    add INPTR, INPTR, WIDTH
    tst WIDTH, #1
    beq 2f
    sub INPTR, INPTR, #1
    vld1.8 {d0[0]}, [INPTR]
2:
    tst WIDTH, #2
    beq 2f
    vext.8 d0, d0, d0, #6
    sub INPTR, INPTR, #1
    vld1.8 {d0[1]}, [INPTR]
    sub INPTR, INPTR, #1
    vld1.8 {d0[0]}, [INPTR]
2:
    tst WIDTH, #4
    beq 2f
    vrev64.32 d0, d0
    sub INPTR, INPTR, #1
    vld1.8 {d0[3]}, [INPTR]
    sub INPTR, INPTR, #1
    vld1.8 {d0[2]}, [INPTR]
    sub INPTR, INPTR, #1
    vld1.8 {d0[1]}, [INPTR]
    sub INPTR, INPTR, #1
    vld1.8 {d0[0]}, [INPTR]
2:
    tst WIDTH, #8
    beq 2f
    vmov d1, d0
    sub INPTR, INPTR, #8
    vld1.8 {d0}, [INPTR]
2:
    vmovl.u8 q8, d0
    vext.8 q2, q1, q0, #15
    vmovl.u8 q9, d1
    vaddw.u8 q10, q15, d4
    vaddw.u8 q11, q15, d5
    vmlal.u8 q8, d4, d28
    vmlal.u8 q9, d5, d28
    vmlal.u8 q10, d0, d28
    vmlal.u8 q11, d1, d28
    vrshrn.u16 d10, q8, #2
    vrshrn.u16 d12, q9, #2
    vshrn.u16 d11, q10, #2
    vshrn.u16 d13, q11, #2
    vzip.8 d10, d11
    vzip.8 d12, d13

    tst WIDTH, #8
    beq 2f
    vst1.8 {d10, d11}, [OUTPTR]!
    vmov q5, q6
2:
    tst WIDTH, #4
    beq 2f
    vst1.8 {d10}, [OUTPTR]!
    vmov d10, d11
2:
    tst WIDTH, #2
    beq 2f
    vst1.8 {d10[0]}, [OUTPTR]!
    vst1.8 {d10[1]}, [OUTPTR]!
    vst1.8 {d10[2]}, [OUTPTR]!
    vst1.8 {d10[3]}, [OUTPTR]!
    vext.8 d10, d10, d10, #4
2:
    tst WIDTH, #1
    beq 2f
    vst1.8 {d10[0]}, [OUTPTR]!
    vst1.8 {d10[1]}, [OUTPTR]!
2:
9:
    subs MAX_V_SAMP_FACTOR, MAX_V_SAMP_FACTOR, #1
    bgt 11b

99:
    vpop {d8-d15}
    pop {r4, r5, r6, pc}

.unreq max_v_samp_factor
.unreq MAX_V_SAMP_FACTOR
.unreq downsampled_width
.unreq DOWNSAMPLED_WIDTH
.unreq input_data
.unreq INPUT_DATA
.unreq output_data_ptr
.unreq OUTPUT_DATA_PTR
.unreq output_data
.unreq OUTPUT_DATA

.unreq outptr
.unreq OUTPTR
.unreq inptr
.unreq INPTR
.unreq width
.unreq WIDTH
.unreq tmp
.unreq TMP

@.endfunc

.text
.align 2
#endif
