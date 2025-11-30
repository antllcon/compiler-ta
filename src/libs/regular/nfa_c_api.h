#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct CState CState;
	struct CState
	{
		int c;
		CState* out;
		CState* out1;
		int lastlist;
	};

	extern CState matchstate;

	char* re2post(char* re);
	CState* post2nfa(char* postfix);

#ifdef __cplusplus
} // extern "C"
#endif