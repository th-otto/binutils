typedef enum {
	O0, OT, O1, OC, O2, X1, OI, OB, SI, I7, LS, BR, LI, LR, LJ, OM, OQ, JSR
} rce_opclass;

typedef struct inst
{	char *name;
	unsigned short opclass;
	unsigned short inst;
} rce_opcode_info;

#ifdef DEFINE_TABLE
rce_opcode_info rce_table[]={
{ "bkpt",	O0,	0x0000 },
{ "sync",	O0,	0x0001 },
{ "rte",	O0,	0x0002 },
{ "rfi",	O0,	0x0003 },
{ "stop",	O0,	0x0004 },
{ "wait",	O0,	0x0005 },

{ "trap",	OT,	0x0010 },
{ "mvc",	O1,	0x0020 },
{ "mvcv",	O1,	0x0030 },
{ "jmp",	O1,	0x0040 },
{ "jsr",	JSR,	0x0050 },
/*****
{ "jmpt",	O1,	0x0060 },
{ "jmpf",	O1,	0x0070 },
*****/
{ "tstne",	O1,	0x0080 },
{ "tstgt",	O1,	0x0090 },
{ "tstlt",	O1,	0x00A0 },
{ "tstnbz",	O1,	0x00B0 },
{ "inct",	O1,	0x00C0 },
{ "incf",	O1,	0x00D0 },
{ "dect",	O1,	0x00E0 },
{ "decf",	O1,	0x00F0 },
{ "zextb",	O1,	0x0100 },
{ "sextb",	O1,	0x0110 },
{ "zexth",	O1,	0x0120 },
{ "sexth",	O1,	0x0130 },
{ "xtrb3",	X1,	0x0140 },
{ "xtrb2",	X1,	0x0150 },
{ "xtrb1",	X1,	0x0160 },
{ "xtrb0",	X1,	0x0170 },

{ "decgt",	O1,	0x01C0 },
{ "declt",	O1,	0x01D0 },
{ "brev",	O1,	0x01E0 },
{ "not",	O1,	0x01F0 },
{ "mfcr",	OC,	0x0400 },
{ "mtcr",	OC,	0x0600 },
{ "mov",	O2,	0x0200 },
{ "movnc",	O2,	0x0C00 },
{ "tst",	O2,	0x0B00 },
{ "cmpne",	O2,	0x0800 },
{ "cmplt",	O2,	0x0900 },
{ "cmphs",	O2,	0x0A00 },
{ "bgenr",	O2,	0x0D00 },
{ "movt",	O2,	0x0E00 },
{ "movf",	O2,	0x0F00 },
{ "and",	O2,	0x1000 },
{ "andn",	O2,	0x1100 },
{ "xor",	O2,	0x1200 },
{ "xnor",	O2,	0x1300 },
{ "or",		O2,	0x1400 },
{ "rsub",	O2,	0x1500 },
{ "addu",	O2,	0x1600 },
{ "subu",	O2,	0x1700 },
{ "ixh",	O2,	0x1800 },
{ "ixw",	O2,	0x1900 },
{ "addc",	O2,	0x1A00 },
{ "subc",	O2,	0x1B00 },
{ "lsr",	O2,	0x1C00 },
{ "asr",	O2,	0x1D00 },
{ "lsl",	O2,	0x1E00 },
/*****
{ "rotl",	O2,	0x1F00 },
*****/

{ "movi",	I7,	0x6800 },

{ "andi",	OB,	0x2200 },
{ "movit",	OB,	0x2400 },
{ "movif",	OB,	0x2600 },
{ "btsti",	OB,	0x2800 },
{ "bclri",	OB,	0x2A00 },
{ "bseti",	OB,	0x2C00 },
{ "bmaski",	OB,	0x2E00 },
{ "bgeni",	OB,	0x3000 },
{ "rsubi",	OB,	0x3200 },
{ "addi",	OI,	0x3400 },
{ "subi",	OI,	0x3600 },
{ "lsri",	SI,	0x3800 },
{ "lsrc",	O1,	0x3800 },
{ "asri",	SI,	0x3A00 },
{ "asrc",	O1,	0x3A00 },
{ "lsli",	SI,	0x3C00 },
{ "lslc",	O1,	0x3C00 },
{ "rotli",	SI,	0x3E00 },
{ "xsr",	O1,	0x3E00 },

{ "ldm",	OM,	0x6000 },
{ "stm",	OM,	0x6010 },
{ "ldq",	OQ,	0x6000 },
{ "stq",	OQ,	0x6010 },
{ "mult",	O2,	0x6100 },

/* { "mvir1",	LI,	0x6800 }, */
{ "lrw",	LR,	0x7000 },
{ "jmpi",	LJ,	0x7000 },
{ "jsri",	LJ,	0x7F00 },

{ "ld",		LS,	0x8000 },
{ "st",		LS,	0x9000 },
{ "ld.w",	LS,	0x8000 },
{ "st.w",	LS,	0x9000 },
{ "ld.b",	LS,	0xA000 },
{ "st.b",	LS,	0xB000 },
{ "ld.h",	LS,	0xC000 },
{ "st.h",	LS,	0xD000 },
{ "br",		BR,	0xE000 },
{ "bsr",	BR,	0xE800 },
{ "bt",		BR,	0xF000 },
{ "bf",		BR,	0xF800 },
{ 0,		0,	0 }
};
#endif
