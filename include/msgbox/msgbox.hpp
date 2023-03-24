#pragma once
#ifndef __MSGBOX_HEADER_CPP__
#define __MSGBOX_HEADER_CPP__

#include <cstdint>
namespace msgbox
{
    enum Style : uint8_t {
        STYLE_INFO,
        STYLE_WARNING,
        STYLE_ERROR,
        STYLE_QUESTION,
    };
    enum Buttons : uint8_t {
        BUTTON_OK,
        BUTTON_OKCANCEL,
        BUTTON_YESNO,
    };
    enum Selection : uint8_t {
        SELECT_OK,
        SELECT_CANCEL,
        SELECT_YES,
        SELECT_NO,
        SELECT_NONE,
    };

    Selection show(const char *message, const char *title, Style style, Buttons buttons);

    inline Selection show(const char *message, const char *title, Style style)
    {
        return show(message, title, style, Buttons::BUTTON_OK);
    }

    inline Selection show(const char *message, const char *title, Buttons buttons)
    {
        return show(message, title, Style::STYLE_INFO, buttons);
    }

    inline Selection show(const char *message, const char *title)
    {
        return show(message, title, Style::STYLE_INFO, Buttons::BUTTON_OK);
    }

} // namespace msgbox

#endif
