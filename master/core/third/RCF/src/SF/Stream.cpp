
#include <SF/Stream.hpp>

#include <RCF/ByteOrdering.hpp>
#include <SF/DataPtr.hpp>
#include <SF/Encoding.hpp>
#include <SF/Exception.hpp>
#include <SF/Node.hpp>
#include <SF/Tools.hpp>

#include <vector>

namespace SF {

    // ContextRead

    ContextRead::ContextRead() : 
        bEnabled_(true) 
    {}

    ContextRead::~ContextRead()
    {}

    void ContextRead::add(UInt32 nid, const ObjectId &id)
    {
        if (bEnabled_)
            nid_id_[nid] = id;
    }

    void ContextRead::add(void *ptr, const std::type_info &objType, void *pObj)
    {
        if (bEnabled_)
            type_ptr_obj_[ objType.name() ][ ptr ] = pObj;
    }

    bool ContextRead::query(UInt32 nid, ObjectId &id)
    {
        if (nid_id_.find( nid ) != nid_id_.end()) 
        {
            id = nid_id_[ nid ];
            return true;
        }
        else
            return false;
    }

    bool ContextRead::query(void *ptr, const std::type_info &objType, void *&pObj)
    {
        if ( type_ptr_obj_[ objType.name() ].find(ptr) != type_ptr_obj_[ objType.name() ].end() ) 
        {
            pObj = type_ptr_obj_[ objType.name() ][ ptr ];
            return true;
        }
        else
            return false;
    }

    void ContextRead::clear()
    { 
        nid_id_.clear(); 
        type_ptr_obj_.clear(); 
    }

    // ContextWrite

    // TODO: enable or disable by default???
    ContextWrite::ContextWrite() : 
        bEnabled_(false), 
        currentId_(1) 
    {}

    ContextWrite::~ContextWrite()
    {}

    void ContextWrite::setEnabled(bool enabled) 
    { 
        bEnabled_ = enabled; 
    }

    bool ContextWrite::getEnabled() 
    { 
        return bEnabled_; 
    }

    void ContextWrite::add(const ObjectId &id, UInt32 &nid)
    {
        if (bEnabled_) 
        {
            if (id_nid_.find( id ) != id_nid_.end())
            {
                nid = id_nid_[ id ];
            }
            else 
            {
                nid = id_nid_[ id ] = currentId_++;
            }
        }
    }
    bool ContextWrite::query(const ObjectId &id, UInt32 &nid)
    {
        if ( bEnabled_ && id_nid_.find( id ) != id_nid_.end() ) 
        {
            nid = id_nid_[ id ];
            return true;
        }
        else
        {
            return false;
        }
    }

    void ContextWrite::clear()
    { 
        id_nid_.clear();
        currentId_ = 1;
    }

    // WithContextRead

    WithContextRead::~WithContextRead()
    {}

    I_ContextRead &WithContextRead::getContext()
    {
        return context;
    }

    // WithContextWrite

    WithContextWrite::~WithContextWrite()
    {}

    I_ContextWrite &WithContextWrite::getContext()
    {
        return context;
    }

    // LocalStorage

    LocalStorage::LocalStorage() 
    {}

    LocalStorage::~LocalStorage()
    {}

    void LocalStorage::set(const std::type_info &objType, void *pObj) 
    { 
        type_ptr_[ objType.name() ] = pObj; 
    }

    void *LocalStorage::get(const std::type_info &objType)
    {
        if ( type_ptr_.find( objType.name() ) != type_ptr_.end() )
            return type_ptr_[ objType.name() ];
        else
            return NULL;
    }

    // WithLocalStorage

    WithLocalStorage::~WithLocalStorage()
    {}

    LocalStorage &WithLocalStorage::getLocalStorage() 
    { 
        return localStorage_; 
    }


    enum {
        BEGIN,
        DATA,
        END,
        BLANK
    };


    // IStream

    IStream::IStream(std::istream &is) : 
        is(is)
    {}

    bool IStream::begin(Node &node)
    {
        Byte8 byte = 0;
        read_byte(byte);
        if (byte != BEGIN) 
        {
            if (byte == BLANK) 
            {
                Byte8 count = 0;
                read_byte(count);
                std::vector<Byte8> buffer(count);
                UInt32 bytesRead = read( &(buffer[0]), count);
                if (bytesRead != static_cast<UInt32>(count)) 
                    SF_THROW(Exception, "Stream format error")(bytesRead)(count);
                return begin(node);
            }
            else 
            {
                putback_byte(byte);
                if (byte != END) 
                {
                    SF_THROW(Exception, "Stream format error")(byte);
                }
            }
        }
        else 
        {
            read_byte( byte );
            Byte8 attrSpec = byte;

            // id
            if (attrSpec & 1)
            {
                read_int(node.id);
            }

            // ref
            attrSpec = attrSpec >> 1;
            if (attrSpec & 1)
            {
                node.ref = 1;
            }

            // type
            attrSpec = attrSpec >> 1;
            if (attrSpec & 1)
            {
                UInt32 length = 0;
                read_int(length);
                node.type.allocate(length);
                read(node.type.get(), length );
            }

            // label
            attrSpec = attrSpec >> 1;
            if (attrSpec & 1)
            {
                UInt32 length = 0;
                read_int(length);
                node.label.allocate(length);
                read(node.label.get(), length);
            }

        }

        return true;
    }

    bool IStream::get(DataPtr &value)
    {
        Byte8 byte;
        read_byte( byte );
        if (byte == DATA) 
        {
            UInt32 length = 0;
            read_int(length);
            value.allocate(length);
            read(value.get(), length);
            return true;
        }
        else 
        {
            putback_byte(byte);
            return false;
        }
    }

    void IStream::end()
    {
        Byte8 byte;
        read_byte(byte);
        if (byte != END)
        {
            SF_THROW(Exception, "did not find end symbol")(byte);
        }
    }

    UInt32 IStream::read_int(UInt32 &n)
    {
        static const int nBytes = sizeof(UInt32);
        Byte8 pBytes[nBytes];
        memset(pBytes, 0, nBytes);
        UInt32 bytesRead = 0;
        bytesRead += read(pBytes, nBytes);
        RCF::networkToMachineOrder(pBytes, 4, 1);
        n = * (UInt32 *) pBytes;
        return bytesRead;
    }

    UInt32 IStream::read_byte(Byte8 &byte)
    {
        UInt32 bytesRead = 0;
        Byte8 byte_ = 0;
        bytesRead += read(&byte_, 1);
        byte = * ((Byte8 *) &byte_);
        return bytesRead;
    }

    UInt32 IStream::read(Byte8 *pBytes, UInt32 nLength)
    {
        is.read(pBytes, nLength);
        if (is.fail()) 
        {
            SF_THROW(Exception, "fail bit set on std::istream")(nLength)(is.gcount());
        }
        return is.gcount();
    }

    void IStream::putback_byte( Byte8 byte )
    {
        is.putback(byte);
    }

    // OStream

    OStream::OStream(std::ostream &os) : os(os)
    {
    }

    void OStream::begin( const Node &node )
    {
        write_byte( (Byte8) BEGIN );

        Byte8 attrSpec = 0;
        if (node.id != 0)
        {
            attrSpec |= 1<<0;
        }
        if (node.ref != 0)
        {
            attrSpec |= 1<<1;
        }
        if (node.type.get() != NULL)
        {
            attrSpec |= 1<<2;
        }
        if (node.label.get() != NULL)
        {
            attrSpec |= 1<<3;
        }

        write_byte( attrSpec );

        if (node.id != 0)
        {
            write_int(node.id);
        }
        if (node.type.get() != NULL)
        {
            write_chararray(node.type.get(), node.type.length());
        }
        if (node.label.get() != NULL)
        {
            write_chararray(node.label.get(), node.label.length() );
        }
    }

    void OStream::put(const DataPtr &value)
    {
        write_byte( (Byte8) DATA );
        write(value.get(), value.length());
    }

    void OStream::end()
    {
        write_byte( (Byte8) END );
    }

    UInt32 OStream::write_int(UInt32 n)
    {
        BOOST_STATIC_ASSERT( sizeof(n) == 4 );
        RCF::machineToNetworkOrder(&n, 4, 1);
        os.write( reinterpret_cast<char*>(&n), 4);
        if (os.fail())
        {
            SF_THROW(Exception, "fail bit set on std::ostream")(n);
        }
        return 4;
    }

    UInt32 OStream::write_byte(Byte8 byte)
    {
        os.write(&byte, 1);
        if (os.fail())
        {
            SF_THROW(Exception, "fail bit set on std::ostream");
        }
        return 1;
    }

    UInt32 OStream::write_chararray(const Char8 *pChar, UInt32 nLength)
    {
        return write(pChar, nLength);
    }

    UInt32 OStream::write(const Byte8 *pBytes, UInt32 nLength)
    {
        UInt32 bytesWritten = 0;
        bytesWritten += write_int(nLength);
        os.write(pBytes, nLength);
        if (os.fail())
        {
            SF_THROW(Exception, "fail bit set on std::ostream")(nLength);
        }
        bytesWritten += nLength;
        return bytesWritten;
    }

} // namespace SF
