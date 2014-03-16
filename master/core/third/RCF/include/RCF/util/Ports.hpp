
//*****************************************************************************
// Copyright (c) 2003. All rights reserved.
// Developed by Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//*****************************************************************************

#ifndef _UTIL_PORTNUMBERS_HPP_
#define _UTIL_PORTNUMBERS_HPP_

#include <fstream>
#include <string>

#include "AutoBuild.hpp"

namespace util {

    class Ports {

    private:

        static std::string getFileName()
        {
            return TEMP_DIR "port.txt";
        }

        
    public:

        static int getNext()
        {
            int port = -1;
            {
                std::ifstream fin(getFileName().c_str());
                fin >> port;
            }
            if (port <= 50000 || port >= 70000) port = 50001;
            {
                std::ofstream fout( getFileName().c_str() );
                fout << ++port;
            }
            return port;
        }

        static int getCurrent()
        {
            int port = -1;
            {
                std::ifstream fin( getFileName().c_str() );
                fin >> port;
            }
            return port;
        }

    };


} // namespace util

#endif //! _UTIL_PORTNUMBERS_HPP_
