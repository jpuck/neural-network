// This file is licensed under the CC0 license (See http://creativecommons.org/publicdomain/zero/1.0/).
// And just to make sure you get the idea, it is also licensed under the WTFPL (See http://en.wikipedia.org/wiki/WTFPL).

#include "string.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>


std::string to_str(const std::vector<bool>& vv)
{
	std::deque<bool> v(vv.begin(), vv.end());
	return to_str(v.begin(), v.end(),"vector");
}

