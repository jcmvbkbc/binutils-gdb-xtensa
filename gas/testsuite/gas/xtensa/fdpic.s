	.text
	.align	4
	.globl	global0
global0:
	ret

	.align	4
	.globl	global1
global1:
	ret

	.align	4
local0:
	ret

	.align	4
local1:
	ret

	.align	4
.L0:
	ret

	.align	4
.L1:
	ret


	.literal_position
	.literal .LC0, v0@got
	.literal .LC1, v1@gotoff
	.literal .LC2, v2@gotfuncdesc
	.literal .LC3, v3@gotofffuncdesc

	.literal .LC10, local0@gotofffuncdesc
	.literal .LC11, local1@gotofffuncdesc
	.literal .LC12, .L0@gotofffuncdesc
	.literal .LC13, .L1@gotofffuncdesc

	.literal .LC20, .L4@got + 1
	.literal .LC21, .L4@gotoff + 1
	.data
.L4:
	.word v4@funcdesc
	.text
	l32r	a2, .LC0
	l32r	a2, .LC1
	l32r	a2, .LC2
	l32r	a2, .LC3
