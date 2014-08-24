#ifndef _klib_mac_extractor_
#define _klib_mac_extractor_

#include <string>
#include <vector>


struct mac_info
{
    mac_info() {}
    mac_info(mac_info&& other) 
    {
        this->type_ = other.type_;
        this->mac_  = std::move(other.mac_);
        this->name_ = std::move(other.name_);
    }

    unsigned int  type_;
    std::string mac_;
    std::string name_;
};
typedef std::vector<mac_info> mac_info_list;

enum extract_mac_method
{
    e_extract_netbios,
    e_extract_helpapi,
};

class mac_extractor
{
public:
    mac_extractor(void);
    ~mac_extractor(void);

    const mac_info_list& get_mac_list();

    void extract_mac_lst(extract_mac_method);
protected:
    void extract_mac_lst_netbios();
    void extract_mac_lst_helpapi();

protected:
    mac_info_list mac_lst_;
};

#endif // _klib_mac_extractor_