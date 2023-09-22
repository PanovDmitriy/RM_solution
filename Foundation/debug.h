#pragma once

inline void LogConsole(auto s)
{
#if _DEBUG 
    std::cout << s << std::endl;
#endif
}

