// Copyright (c) 2025
// Minimal COM activator for Windows toast foreground activation.
// Implements INotificationActivationCallback so Windows can deliver toast
// activation when the Electron process is not running. On activation we relay
// arguments via a small helper that will (re)launch the app with --toast-activation.

#ifndef ELECTRON_SHELL_BROWSER_NOTIFICATIONS_WIN_NOTIFICATION_TOAST_ACTIVATOR_H_
#define ELECTRON_SHELL_BROWSER_NOTIFICATIONS_WIN_NOTIFICATION_TOAST_ACTIVATOR_H_

#include <windows.h>
#include <NotificationActivationCallback.h>
#include <wrl/implements.h>

#include <string>

namespace electron {

// CLSID will be defined/declared elsewhere (registry + a .cc definition).
// {FAKE0000-0000-0000-0000-000000000001} placeholder; replace with stable GUID.
extern const CLSID kCLSID_NotificationToastActivator;

class NotificationToastActivator
    : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
                                          INotificationActivationCallback> {
 public:
  NotificationToastActivator() = default;

  // INotificationActivationCallback
  IFACEMETHODIMP Activate(HSTRING app_user_model_id,
                          HSTRING invoked_args,
                          const NOTIFICATION_USER_INPUT_DATA* data,
                          ULONG data_count) override;

  // Utility to register/unregister the COM class object at runtime (optional).
  static HRESULT RegisterComServer();
  static HRESULT UnregisterComServer();
  static bool IsRegistered();
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_NOTIFICATIONS_WIN_NOTIFICATION_TOAST_ACTIVATOR_H_
