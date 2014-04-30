#include "stdafx.h"
#include "push_upgrade_test.h"


#include <aom/iframework_imp.h>


push_upgrade_test::push_upgrade_test(void)
{
}

push_upgrade_test::~push_upgrade_test(void)
{
}

void push_upgrade_test::test_upgrade()
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("upgrade.dll"));
    _ASSERT(S_OK == hr);
    if (S_OK != hr) {
        return ;
    }

    pframework->start();
}