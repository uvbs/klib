
//*****************************************************************************
// RCF - Remote Call Framework
// Copyright (c) 2005, Jarl Lindrud.
// Contact: jlindrud@hotmail.com .
//
// Distributed under the so-called MIT license, see accompanying file license.txt.
//*****************************************************************************

#ifndef _RCF_PROTOCOL_PROTOCOL_HPP
#define _RCF_PROTOCOL_PROTOCOL_HPP

#include <iosfwd>
#include <RCF/Tools.hpp>

namespace RCF {

    extern const int DefaultProtocol;

    template<int N>
    class Protocol
    {
    public:

        static std::string getName()
        {
            return "";
        }

        class In
        {
        public:
            void bind(std::istream &is) 
            {}

            void unbind()
            {}
            
            template<typename T>
            In &operator>>(T &t)
            {
                RCF_THROW(RCF::SerializationException, "unsupported serialization protocol")(N);
                return *this;
            }
        };

        class Out
        {
        public:
            void bind(std::ostream &os) 
            {}

            void unbind()
            {}

            template<typename T>
            Out &operator<<(const T &t)
            {
                RCF_THROW(RCF::SerializationException, "unsupported serialization protocol")(N);
                return *this;
            }
        };
    };


    template<typename IS, typename OS>
    class ProtocolBase
    {
    public:
        class In
        {
        public:
            void bind(std::istream &is)                            
            {
                ar.reset( new IS(is) );
            }

            void unbind()
            {
                ar.reset();
            }
            
            template<typename T> In &operator>>(T &t)
            { 
                *ar >> t; 
                return *this; 
            }
        
        private:
            std::auto_ptr<IS> ar;
        };

        class Out
        {
        public:
            void bind(std::ostream &os)                            
            {
                ar.reset( new OS(os) );
            }

            void unbind()
            {
                ar.reset();
            }
            
            template<typename T> Out &operator<<(const T &t)
            { 
                *ar << t; 
                return *this; 
            }

        private:
            std::auto_ptr<OS> ar;
        };
    };
    

    // theoretically this one should be faster, since we reuse the memory instead of new'ing it again
    template<typename IS, typename OS>
    class ProtocolBaseWithPlacementNew
    {
    public:
        class In
        {
        public:
            In() : ar() 
            {
                ar = (IS *) malloc(sizeof(IS));
                valid = false;
            }

            ~In()
            {
                if (valid)
                {
                    ar->~IS();
                    valid = false;
                }
                free(ar);
            }
            
            void bind(std::istream &is)                            
            {
                RCF_ASSERT(!valid);
                new (ar) IS(is);
                valid = true;
            }

            void unbind()
            {
                if (valid)
                {
                    ar->~IS();
                    valid = false;
                }
            }
            
            template<typename T> In &operator>>(T &t)
            { 
                *ar >> t; 
                return *this; 
            }
        
        private:
            IS *ar;
            bool valid;
        };

        class Out
        {
        public:
            Out() : ar() 
            {
                ar = (OS *) malloc(sizeof(OS));
                valid = false;
            }

            ~Out()
            {
                if (valid)
                {
                    ar->~OS();
                    valid = false;
                }
                free(ar);
            }
            
            void bind(std::ostream &os)                            
            {
                RCF_ASSERT(!valid);
                new (ar) OS(os);
                valid = true;
            }

            void unbind()
            {
                if (valid)
                {
                    ar->~OS();
                    valid = false;
                }
            }
            
            template<typename T> Out &operator<<(const T &t)
            { 
                *ar << t; 
                return *this; 
            }

        private:
            OS *ar;
            bool valid;
        };
    };


} // namespace RCF

#endif //! _RCF_PROTOCOL_PROTOCOL_HPP
