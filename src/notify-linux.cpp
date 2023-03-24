#include <bits/types/sig_atomic_t.h>
#include <dbus/dbus.h>
#include <msgbox/notify.hpp>

namespace msgbox
{
    static sig_atomic_t done;
    static void notify_callback(DBusPendingCall *pending, void *user_data) { done = 1; }
    static int notify_build_message(DBusMessage *notify_msg, const NotifyConfig &config)
    {
        DBusMessageIter args[4];
        uint32_t replaces_id = -1;
        const char *array[]  = {"v1", "v2", NULL};
        int32_t timeout      = -1;
        dbus_bool_t m        = 0;
        int value            = 42;

        dbus_message_iter_init_append(notify_msg, &args[0]);
        m |= dbus_message_iter_append_basic(&args[0], DBUS_TYPE_STRING, config.AppName.c_str());
        m |= dbus_message_iter_append_basic(&args[0], DBUS_TYPE_UINT32, &replaces_id);
        m |= dbus_message_iter_append_basic(&args[0], DBUS_TYPE_STRING, config.Icon.c_str());
        m |= dbus_message_iter_append_basic(&args[0], DBUS_TYPE_STRING, config.Title.c_str());
        m |= dbus_message_iter_append_basic(&args[0], DBUS_TYPE_STRING, config.Body.c_str());

        m |= dbus_message_iter_open_container(&args[0], DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING,
                                              &args[1]);
        for (int i = 0; array[i]; i++)
            m |= dbus_message_iter_append_basic(&args[1], DBUS_TYPE_STRING, &array[i]);
        m |= dbus_message_iter_close_container(&args[0], &args[1]);

        m |= dbus_message_iter_open_container(&args[0], DBUS_TYPE_ARRAY, "{sv}",
                                              &args[1]); /* usually {sv} for dictionaries */
        m |= dbus_message_iter_open_container(&args[1], DBUS_TYPE_DICT_ENTRY, NULL, &args[2]);
        m |= dbus_message_iter_append_basic(&args[2], DBUS_TYPE_STRING, config.Key.c_str());
        m |= dbus_message_iter_open_container(&args[2], DBUS_TYPE_VARIANT,
                                              DBUS_TYPE_INT32_AS_STRING, &args[3]);
        m |= dbus_message_iter_append_basic(&args[3], DBUS_TYPE_INT32, &value);
        m |= dbus_message_iter_close_container(&args[2], &args[3]);
        m |= dbus_message_iter_close_container(&args[1], &args[2]);
        m |= dbus_message_iter_close_container(&args[0], &args[1]);
        m |= dbus_message_iter_append_basic(&args[0], DBUS_TYPE_INT32, &timeout);

        return m;
    }

#define NOTIFY_OBJECT    "/org/freedesktop/Notifications"
#define NOTIFY_INTERFACE "org.freedesktop.Notifications"
    bool notify(const NotifyConfig &config)
    {
        bool ret = true;
        DBusMessage *msg, *reply;
        DBusConnection *conn;
        DBusError err;
        DBusPendingCall *pending;

        dbus_error_init(&err);
        conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

        if (dbus_error_is_set(&err)) {
            fprintf(stderr, "Connection Error (%s)\n", err.message);
            dbus_error_free(&err);
        }
        if (!conn) {
            ret = false;
            goto return_exit;
        }
        msg = dbus_message_new_method_call(NULL, NOTIFY_OBJECT, NOTIFY_INTERFACE, "Notify");

        if (!msg) {
            fprintf(stderr, "Message Null\n");
            ret = false;
            goto return_exit;
        }

        dbus_message_set_auto_start(msg, TRUE);
        dbus_message_set_destination(msg, NOTIFY_INTERFACE);

        /* append arguments
        UINT32 org.freedesktop.Notifications.Notify (STRING app_name, UINT32 replaces_id, STRING
        app_icon, STRING summary, STRING body, ARRAY actions, DICT hints, INT32 expire_timeout); */

        if (!notify_build_message(msg, config)) {
            fprintf(stderr, "Out Of Memory!\n");
            ret = false;
            goto return_exit;
        }

        dbus_error_init(&err);

        if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
            fprintf(stderr, "Sending failed!\n");
            ret = false;
            goto return_exit;
        }

        if (!dbus_pending_call_set_notify(pending, &notify_callback, NULL, NULL)) {
            fprintf(stderr, "Callback failed!");
            ret = false;
            goto return_exit;
        }

        while (!done)
            dbus_connection_read_write_dispatch(conn, -1);

    return_exit:
        if (msg)
            dbus_message_unref(msg);
        if (conn)
            dbus_connection_unref(conn);
        return ret;
    }
} // namespace msgbox
