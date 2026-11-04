	.build_version macos, 27, 0	sdk_version 27, 0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z6get_u8PKh                   ; -- Begin function _Z6get_u8PKh
	.p2align	2
__Z6get_u8PKh:                          ; @_Z6get_u8PKh
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	str	x0, [sp, #8]
	ldr	x8, [sp, #8]
	ldrb	w0, [x8]
	add	sp, sp, #16
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z7get_u16PKt                  ; -- Begin function _Z7get_u16PKt
	.p2align	2
__Z7get_u16PKt:                         ; @_Z7get_u16PKt
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	str	x0, [sp, #8]
	ldr	x8, [sp, #8]
	ldrh	w0, [x8]
	add	sp, sp, #16
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z7get_u32PKj                  ; -- Begin function _Z7get_u32PKj
	.p2align	2
__Z7get_u32PKj:                         ; @_Z7get_u32PKj
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	str	x0, [sp, #8]
	ldr	x8, [sp, #8]
	ldr	w0, [x8]
	add	sp, sp, #16
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__Z7get_u64PKy                  ; -- Begin function _Z7get_u64PKy
	.p2align	2
__Z7get_u64PKy:                         ; @_Z7get_u64PKy
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	str	x0, [sp, #8]
	ldr	x8, [sp, #8]
	ldr	x0, [x8]
	add	sp, sp, #16
	ret
	.cfi_endproc
                                        ; -- End function
.subsections_via_symbols
