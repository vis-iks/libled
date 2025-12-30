#include "core/Assert.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#define assert_custom(_Expression, _Message, _File, _Line) (void)( (!!(_Expression)) || (__assert(_Message, _File, _Line), 0) )

#ifdef __cplusplus
extern "C" {
#endif
	void __OnAssert(const char* file, unsigned line, const char* description)
	{
		printf("Requirement (%s) failed in file %s line %u\r\n", description, file, line);
		assert_custom(0, description, file, line);
	}
#ifdef __cplusplus
}
#endif
