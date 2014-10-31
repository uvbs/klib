
#ifndef _klib_buff_holder_h_
#define _klib_buff_holder_h_



/* ·µ»ØµÄ»º³åÇø */
class buffer_holder
{
protected:
    buffer_holder(buffer_holder&);
    buffer_holder& operator = (const buffer_holder&);

public:
    buffer_holder() : buff_(0), len_(0) {}

    buffer_holder(buffer_holder&& other)
    {
        this->buff_ = other.buff_;
        this->len_  = other.len_;

        other.buff_ = 0;
        other.len_  = 0;
    }

    ~buffer_holder ()
    {
        free();
    }

    unsigned char* get() const { return buff_; }
    unsigned int size() const { return len_; }
    bool is_valid() const { return buff_ != 0 && len_ > 0;}

    operator unsigned char* () { return get(); }
    operator char* () const { return (char*) get(); }
    operator bool () const { return is_valid(); }
    operator string ()
    {
        if (NULL == buff_)
        {
            return "";
        }
        return std::string((const char*)buff_, len_);
    }

    // detach buffer 
    void attach(unsigned char* buf, unsigned int len)
    {
        free();
        this->buff_ = buf;
        this->len_ = len;
    }

    void detach() { buff_ = 0; len_ = 0;}

    void free()
    {
        if (this->buff_) 
        {
            delete []this->buff_;
            this->buff_ = 0;
        }
    }

protected:
    unsigned char* buff_;
    unsigned int len_;
};



#endif  // _klib_buff_holder_h_