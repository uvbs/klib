#ifndef _klib_mac_extractor_
#define _klib_mac_extractor_

#include <string>
#include <vector>


struct mac_info
{
    mac_info() {}
    mac_info(mac_info&& other) 
    {
        this->mac_  = std::move(other.mac_);
        this->name_ = std::move(other.name_);
    }

    std::string mac_;
    std::string name_;
};
typedef std::vector<mac_info> mac_info_list;

class mac_extractor
{
public:
    mac_extractor(void);
    ~mac_extractor(void);

    const mac_info_list& get_mac_list();

protected:
    void extract_mac_lst();

protected:
    mac_info_list mac_lst_;
};

#endif // _klib_mac_extractor_