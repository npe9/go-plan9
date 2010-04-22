TEXT sysr1(SB), 1, $0
MOVL $0, AX
INT $64
RET
TEXT _errstr(SB), 1, $0
MOVL $1, AX
INT $64
RET
TEXT bind(SB), 1, $0
MOVL $2, AX
INT $64
RET
TEXT chdir(SB), 1, $0
MOVL $3, AX
INT $64
RET
TEXT close(SB), 1, $0
MOVL $4, AX
INT $64
RET
TEXT dup(SB), 1, $0
MOVL $5, AX
INT $64
RET
TEXT alarm(SB), 1, $0
MOVL $6, AX
INT $64
RET
TEXT exec(SB), 1, $0
MOVL $7, AX
INT $64
RET
TEXT _exits(SB), 1, $0
MOVL $8, AX
INT $64
RET
TEXT _fsession(SB), 1, $0
MOVL $9, AX
INT $64
RET
TEXT fauth(SB), 1, $0
MOVL $10, AX
INT $64
RET
TEXT _fstat(SB), 1, $0
MOVL $11, AX
INT $64
RET
TEXT segbrk(SB), 1, $0
MOVL $12, AX
INT $64
RET
TEXT _mount(SB), 1, $0
MOVL $13, AX
INT $64
RET
TEXT open(SB), 1, $0
MOVL $14, AX
INT $64
RET
TEXT _read(SB), 1, $0
MOVL $15, AX
INT $64
RET
TEXT oseek(SB), 1, $0
MOVL $16, AX
INT $64
RET
TEXT sleep(SB), 1, $0
MOVL $17, AX
INT $64
RET
TEXT _stat(SB), 1, $0
MOVL $18, AX
INT $64
RET
TEXT rfork(SB), 1, $0
MOVL $19, AX
INT $64
RET
TEXT _write(SB), 1, $0
MOVL $20, AX
INT $64
RET
TEXT pipe(SB), 1, $0
MOVL $21, AX
INT $64
RET
TEXT create(SB), 1, $0
MOVL $22, AX
INT $64
RET
TEXT fd2path(SB), 1, $0
MOVL $23, AX
INT $64
RET
TEXT brk_(SB), 1, $0
MOVL $24, AX
INT $64
RET
TEXT remove(SB), 1, $0
MOVL $25, AX
INT $64
RET
TEXT _wstat(SB), 1, $0
MOVL $26, AX
INT $64
RET
TEXT _fwstat(SB), 1, $0
MOVL $27, AX
INT $64
RET
TEXT notify(SB), 1, $0
MOVL $28, AX
INT $64
RET
TEXT noted(SB), 1, $0
MOVL $29, AX
INT $64
RET
TEXT segattach(SB), 1, $0
MOVL $30, AX
INT $64
RET
TEXT segdetach(SB), 1, $0
MOVL $31, AX
INT $64
RET
TEXT segfree(SB), 1, $0
MOVL $32, AX
INT $64
RET
TEXT segflush(SB), 1, $0
MOVL $33, AX
INT $64
RET
TEXT rendezvous(SB), 1, $0
MOVL $34, AX
INT $64
RET
TEXT unmount(SB), 1, $0
MOVL $35, AX
INT $64
RET
TEXT _wait(SB), 1, $0
MOVL $36, AX
INT $64
RET
TEXT semacquire(SB), 1, $0
MOVL $37, AX
INT $64
RET
TEXT semrelease(SB), 1, $0
MOVL $38, AX
INT $64
RET
TEXT seek(SB), 1, $0
MOVL $39, AX
INT $64
CMPL AX,$-1
			JNE 4(PC)
			MOVL a+0(FP),CX
			MOVL AX,0(CX)
			MOVL AX,4(CX)
RET
TEXT fversion(SB), 1, $0
MOVL $40, AX
INT $64
RET
TEXT errstr(SB), 1, $0
MOVL $41, AX
INT $64
RET
TEXT stat(SB), 1, $0
MOVL $42, AX
INT $64
RET
TEXT fstat(SB), 1, $0
MOVL $43, AX
INT $64
RET
TEXT wstat(SB), 1, $0
MOVL $44, AX
INT $64
RET
TEXT fwstat(SB), 1, $0
MOVL $45, AX
INT $64
RET
TEXT mount(SB), 1, $0
MOVL $46, AX
INT $64
RET
TEXT await(SB), 1, $0
MOVL $47, AX
INT $64
RET
TEXT pread(SB), 1, $0
MOVL $50, AX
INT $64
RET
TEXT pwrite(SB), 1, $0
MOVL $51, AX
INT $64
RET
