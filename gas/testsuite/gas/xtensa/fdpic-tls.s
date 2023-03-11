	.literal_position
	.literal .L0, v0@gottpoff
#	.literal .L1, v1@tlsarg
	.text
	l32r	a2, .L0
#	l32r	a3, .L1
	movi	a3, v1@tlsarg
