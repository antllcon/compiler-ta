#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	/*
	 * Определения из nfa.c, которые нужны C++ коду
	 * для понимания C-шной структуры автомата
	 */

	typedef struct CState CState;
	struct CState
	{
		int c;
		CState* out;
		CState* out1;
		int lastlist;
	};

	/* Глобальное состояние "Match" */
	extern CState matchstate;

	/* API, которое мы будем вызывать из C++ */
	char* re2post(char* re);
	CState* post2nfa(char* postfix);

#ifdef __cplusplus
} // extern "C"
#endif