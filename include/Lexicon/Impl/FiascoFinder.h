#ifndef FIASCO_H
#define FIASCO_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
// [WS 2010-07-30] Detect the infamous "Static initialization order fiasco"
// email warrenstevens --> [initials]@[firstnamelastname].com 
// read --> http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.12 if you haven't suffered
// To enable this feature --> define E-N-A-B-L-E-_-F-I-A-S-C-O-_-F-I-N-D-E-R, rebuild, and run
#ifndef NDEBUG
#define ENABLE_FIASCO_FINDER
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_FIASCO_FINDER

#include <iostream>
#include <fstream>

inline bool WriteFiasco(const std::string& fileName)
{
    //if (fileName.ends_with(".cpp"))
    {
        static int counter = 0;
        bool inc = counter++;

        std::ofstream file;
        file.open("fiasco-finder.txt", std::ios::out | std::ios::app);
        
        if (!inc)
            file << "--------------------------------------------------------------------------" << std::endl;
        file << "Starting to initialize file - number: [" << counter << "] filename: [" << fileName.c_str() << "]" << std::endl;
        file.flush();
        file.close();
    }
    return true;
}

// [WS 2010-07-30] If you get a name collision on the following line, your usage is likely incorrect
#define FIASCO_FINDER static const bool g_psuedoUniqueName = WriteFiasco(__FILE__);

#else // ENABLE_FIASCO_FINDER
// do nothing
#define FIASCO_FINDER

#endif // ENABLE_FIASCO_FINDER

#endif //FIASCO_H
