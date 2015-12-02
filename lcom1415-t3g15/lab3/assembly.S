#define BUF_OUT 0x60

.global _dataASM
.data
_dataASM:
	.long 0

.global _kbd_test_scan_ASM

_kbd_test_scan_ASM:
	inb $BUF_OUT, %al
	mov %al, _dataASM
	ret

