;
;  file: ex2.s
;
  .text
  .global _asmSGDFT
_asmSGDFT:
  mov #0,w0
  mov w0,_cVariable
  return
  .global _begin
_main:
  call _foo
  return
  .bss
  .global _asmVariable
  .align 2
_asmVariable: .space 2
 .end
