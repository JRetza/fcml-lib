/*
 * fcml_trace.h
 *
 *  Created on: 23 lip 2013
 *      Author: tAs
 */

#ifndef FCML_TRACE_H_
#define FCML_TRACE_H_

#include "stdio.h"

#ifdef _FCML_TRACE
#define FCML_TRACE(pattern, ...) { printf("%s %d ", __FILE__,  __LINE__); printf(pattern, ##__VA_ARGS__); printf("\n");}
#else
#define FCML_TRACE(pattern, ...)
#endif

#endif /* FCML_TRACE_H_ */
