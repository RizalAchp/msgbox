#include "msgbox/msgbox.hpp"

#include <cstdio>

int main(void)
{
    using namespace msgbox;
    auto ret = show("HELLO, WORLD", "GREETINGS", STYLE_ERROR, BUTTON_YESNO);
    switch (ret) {
    case SELECT_OK: printf("OK Selected!\n"); break;
    case SELECT_CANCEL: printf("CANCEL Selected!\n"); break;
    case SELECT_YES: printf("YES Selected!\n"); break;
    case SELECT_NO: printf("NO Selected!\n"); break;
    case SELECT_NONE: printf("NONE Selected!\n"); break;
    }
}
