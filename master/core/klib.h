#pragma once

#include <WinSock2.h>
#include <Windows.h>

#include "macro.h"
#include "istddef.h"
#include "inttype.h"

#include "core/klib_core.h"

#include "core/console_screen.h"
#include "core/ini_file.h"
#include "core/share_memory.h"
#include "core/DateTime.h"
#include "core/mini_dumper.h"
#include "core/logger.h"
#include "core/private_heap.h"
#include "core/ensure.h"
#include "core/scope_guard.h"
#include "core/fix_circle_buff.h"
#include "core/ip_seg_table.h"
#include "core/ini_file.h"
#include "core/console_screen.h"
#include "core/perf_counter.h"
#include "core/seqnumber.h"

#include "sys/sys.h"

#include "io/file.h"
#include "io/path.h"
#include "io/stream.h"

#include "kthread/kthreadpool.h"
#include "kthread/threadpool.h"
#include "kthread/ReadWriteSection.h"
#include "kthread/thread.h"
#include "kthread/auto_lock.h"

#include "net/WinSockInit.h"
#include "net/addr_resolver.h"
#include "net/tcp_socket.h"
#include "net/udp_client.h"


#include "encrypt/MD5.h"
#include "encrypt/md5_single.h"
#include "encrypt/sha1_single.h"
#include "encrypt/crc_single.h"
#include "encrypt/des_single.h"
#include "encrypt/rc4_single.h"

#include "ui/TrayIcon.h"

#include "algo/sort.h"
#include "algo/search.h"
#include "algo/kmp.h"
#include "algo/sunday.h"
#include "algo/ld.h"
#include "algo/string_hash.h"
#include "algo/wm_wrapper.h"
#include "algo/text_search.h"

#include "util/strutil.h"
#include "util/convert.h"
#include "util/atom.h"

#include "pattern/aspec.h"
#include "pattern/factory.h"
#include "pattern/fsm.h"
#include "pattern/object_reuser.h"
#include "pattern/object_pool.h"
#include "pattern/actor.h"

#include "aom/iframework_imp.h"

#include "debuglog.h"

namespace klib
{

using namespace klib::algo;
using namespace klib::debug;
using namespace klib::kthread;
using namespace klib::core;
using namespace klib::mem;
using namespace klib::net;
using namespace klib::util;
using namespace klib::encode;

}