#pragma once

#ifndef __MSGBOX_NOTIFY_HEADER__
#define __MSGBOX_NOTIFY_HEADER__
#include <string>
#include <string_view>
namespace msgbox
{
    struct NotifyConfig {
        std::string AppName = "msgbox";
        std::string Icon    = "icon";
        std::string Title   = "title";
        std::string Body    = "body";
        std::string Key     = "msgbox";

        NotifyConfig()      = default;
    };

    bool notify(const NotifyConfig &config);

} // namespace msgbox
#endif
