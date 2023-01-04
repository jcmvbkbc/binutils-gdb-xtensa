	.literal_position
	.literal .L0, v0@got
	.literal .L1, v1@gotoff
	.literal .L2, v2@gotfuncdesc
	.literal .L3, v3@gotofffuncdesc
	.data
.L4:
	.word v4@funcdesc
	.text
	l32r	a2, .L0
	l32r	a2, .L1
	l32r	a2, .L2
	l32r	a2, .L3
