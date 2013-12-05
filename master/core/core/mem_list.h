#pragma once

namespace klib {
namespace mem {


template<INT _MaxSize, INT _BlockSize, INT _bUseStatic>
class mem_block_lsit
{
public:
    struct pt
    {
        pt() : pnext (NULL) {}

        size_t block_size()
        {
            return _BlockSize;
        }

        void* data()
        {
            return ((char*)this + sizeof(*this));
        }

        pt*  pnext;
    };

public:
    typedef  mem_block_lsit<_MaxSize, _BlockSize, _bUseStatic>      self_type;
    typedef  size_t                                                 size_type;
    typedef  pt                                                     pointer_type;

    #define  INTERNAL_BLOCK_SIZE            (_BlockSize + sizeof(pointer_type))
    #define  MAX_ITEM_COUNT                 ((_MaxSize / INTERNAL_BLOCK_SIZE) + ((_MaxSize % INTERNAL_BLOCK_SIZE) > 0))

    enum {BLOCK_SIZE = INTERNAL_BLOCK_SIZE};

public:
    mem_block_lsit()  : m_item_size(0), m_nfree_size(0), 
        m_nused_size(0), m_buffer_length(0), 
        m_head(NULL), m_tail(NULL)
    {
        static_assert(_MaxSize >= _BlockSize, "error size");
    }

    ~mem_block_lsit()
    {
        uninit();
    }

    bool init()
    {
        if (_bUseStatic)
        {
            m_item_size = MAX_ITEM_COUNT;
            m_buffer_length = m_item_size * INTERNAL_BLOCK_SIZE;
            m_nfree_size = m_item_size;

            pointer_type* p = (pointer_type*) new char[m_buffer_length];
            if (nullptr == p) 
            {
                return false;
            }
        
            m_addr = p;
            m_head = p;
            m_tail = p;
            pointer_type* pNext = nullptr;

            for (size_t i=0; i < m_nfree_size - 1; ++i) 
            {
                pNext = (pointer_type*) ((char*)p + INTERNAL_BLOCK_SIZE);
                ((pointer_type *) p)->pnext = (pointer_type*) (pNext);
                p = pNext;
            }

            ((pointer_type *) p)->pnext = nullptr;
            m_tail = p;
        }

        return true;
    }

    void uninit()
    {
        if (_bUseStatic) 
        {
            if (m_addr != nullptr) 
            {
                delete m_addr;
                m_addr = nullptr;
            }
        }
        else
        {
            pointer_type* ptmp = NULL;

            while (m_head)
            {
                ptmp = m_head;
                m_head = m_head->pnext;

                delete ptmp;
            }
        }
    }

public:
    void* get()
    {
        if (_bUseStatic) 
        {
            if (0 == m_nfree_size) 
            {
                return nullptr;
            }

            pointer_type *p = (pointer_type *)m_head;
            m_head = m_head->pnext;
            ++ m_nused_size;
            -- m_nfree_size;

            return p->data();
        }
        else
        {
            char* pbuff =  new char[INTERNAL_BLOCK_SIZE];
            if (pbuff) 
            {
                ++ m_nused_size;
                pointer_type* p = (pointer_type*) pbuff;
                return p->data();
            }
            return nullptr;
        }
    }
    
    void free_(void* mem)
    {
        pointer_type* p = (pointer_type*) ((char *)mem - sizeof(pointer_type));

        -- m_nused_size;
        ++ m_nfree_size;

        if (m_head == nullptr) 
        {
            m_head = m_tail = p;
            p->pnext = nullptr;
        }
        else
        {
            m_tail->pnext = p;
            m_tail = p;
            p->pnext = nullptr;
        }
    }

    size_type buffser_size() const
    {
        if (_bUseStatic) 
        {
            return m_buffer_length;
        }
        else
        {
            return item_size() * INTERNAL_BLOCK_SIZE;
        }
    }

    size_type used_size() const
    {
        return m_nused_size;
    }

    size_type free_size() const
    {
        return m_nfree_size;
    }

    size_type item_size() const
    {
        if (_bUseStatic) 
        {
            return m_item_size;
        }
        else
        {
            return m_nfree_size + m_nused_size;
        }
    }

protected:
    size_type          m_nused_size;
    size_type          m_nfree_size;         /// ¿ÕÏÐµÄ¸öÊý
    size_type          m_item_size;
    size_type          m_buffer_length;

    pointer_type*      m_addr;
    pointer_type*      m_head;
    pointer_type*      m_tail;
};


}}

