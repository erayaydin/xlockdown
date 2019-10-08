#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <crypt.h>
#include <shadow.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

using namespace std;

struct pam_response *reply;
int function_conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr)
{
    *resp = reply;
    return PAM_SUCCESS;
}

int main(int argc, char *argv[])
{
    const struct pam_conv local_conversation = { function_conversation, NULL };
    pam_handle_t *local_auth_handle = NULL; // this gets set by pam_start

    int retval;
    retval = pam_start("xlockdown", getpwuid(getuid())->pw_name, &local_conversation, &local_auth_handle);

    if (retval != PAM_SUCCESS)
    {
        printf("pam_start returned: %d\n ", retval);
        return 0;
    }

    Display *display;
    Window root;
    XEvent event;
    char keybuffer[1024];

    if((display = XOpenDisplay(nullptr)) == nullptr) {
        cerr << "Couldn't connect to DISPLAY" << endl;
        exit(1);
    }

    root = DefaultRootWindow(display);
    XGrabPointer(display, root, 1, ButtonPress, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    XGrabKeyboard(display, root, 0, GrabModeAsync, GrabModeAsync, CurrentTime);
    XSelectInput(display, root, KeyPressMask);

    int length = 0;
    while (XNextEvent(display, &event), 1) {
        if(event.type == KeyPress) {
            KeySym key;
            XComposeStatus compose;

            if (length > sizeof(keybuffer)-10)
                length = 0;
            XLookupString(&event.xkey, keybuffer+length, 10, &key, &compose);

            if (key == XK_Return) {
                keybuffer[length] = 0;

                if (length) {
                    reply = (struct pam_response *)malloc(sizeof(struct pam_response));

                    reply[0].resp = strdup(keybuffer);
                    reply[0].resp_retcode = 0;
                    retval = pam_authenticate(local_auth_handle, 0);

                    bool authenticated = true;
                    if (retval != PAM_SUCCESS)
                    {
                        if (retval == PAM_AUTH_ERR)
                        {
                            authenticated = false;
                        }
                        else
                        {
                            printf("pam_authenticate returned %d\n", retval);
                            authenticated = false;
                            exit(1);
                        }
                    }

                    if(authenticated) {
                        retval = pam_end(local_auth_handle, retval);
                        if (retval != PAM_SUCCESS)
                        {
                            printf("pam_end returned\n");
                            return 0;
                        }

                        XUngrabKeyboard(display, CurrentTime);
                        XUngrabPointer(display, CurrentTime);
                        exit(0);
                    } else {
                        length = 0;
                    }
                } else
                    length = 0;
            } else
                length++;
        }
    }
}