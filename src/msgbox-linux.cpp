#include <gtk/gtk.h>
#include <msgbox/msgbox.hpp>
namespace msgbox
{
    namespace
    {
        inline static GtkMessageType getMessageType(Style style)
        {
            switch (style) {
            case Style::STYLE_INFO: return GTK_MESSAGE_INFO;
            case Style::STYLE_WARNING: return GTK_MESSAGE_WARNING;
            case Style::STYLE_ERROR: return GTK_MESSAGE_ERROR;
            case Style::STYLE_QUESTION: return GTK_MESSAGE_QUESTION;
            default: return GTK_MESSAGE_INFO;
            }
        }

        inline static GtkButtonsType getButtonsType(Buttons buttons)
        {
            switch (buttons) {
            case Buttons::BUTTON_OK: return GTK_BUTTONS_OK;
            case Buttons::BUTTON_OKCANCEL: return GTK_BUTTONS_OK_CANCEL;
            case Buttons::BUTTON_YESNO: return GTK_BUTTONS_YES_NO;
            default: return GTK_BUTTONS_OK;
            }
        }

        inline static Selection getSelection(gint response)
        {
            switch (response) {
            case GTK_RESPONSE_OK: return Selection::SELECT_OK;
            case GTK_RESPONSE_CANCEL: return Selection::SELECT_CANCEL;
            case GTK_RESPONSE_YES: return Selection::SELECT_YES;
            case GTK_RESPONSE_NO: return Selection::SELECT_NO;
            default: return Selection::SELECT_NONE;
            }
        }

    } // namespace

    Selection show(const char *msg, const char *title, Style style, Buttons buttons)
    {
        if (!gtk_init_check(0, NULL)) {
            return Selection::SELECT_NONE;
        }

        const GtkButtonsType resp = getButtonsType(buttons);
        const GtkMessageType tp   = getMessageType(style);

        GtkWidget *diag = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, tp, resp, "%s", msg);
        gtk_window_set_title(GTK_WINDOW(diag), title);

        auto ret_dialog = gtk_dialog_run(GTK_DIALOG(diag));
        gtk_widget_destroy(GTK_WIDGET(diag));
        while (g_main_context_iteration(NULL, false))
            ;

        return getSelection(ret_dialog);
    }

} // namespace msgbox
