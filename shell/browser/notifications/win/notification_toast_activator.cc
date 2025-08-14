// Copyright (c) 2025
// Implementation of NotificationToastActivator.

#include "shell/browser/notifications/win/notification_toast_activator.h"

#include <shlobj.h>
#include <wrl/wrappers/corewrappers.h>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/string_util_win.h"
#include "base/strings/utf_string_conversions.h"
#include "base/win/com_init_util.h"
#include "shell/common/application_info.h"

using Microsoft::WRL::ClassFactory;

namespace electron {

// NOTE: Replace this GUID with a stable, product-specific one and keep it in
// sync with installer/shortcut registration.
// {FAKE0000-0000-0000-0000-000000000001}
const CLSID kCLSID_NotificationToastActivator =
    {0xFAKE0000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}};

namespace {

// Keep global cookie for revocation.
DWORD g_com_cookie = 0;

class ToastActivatorFactory : public Microsoft::WRL::AgileActivationFactory, public IClassFactory {
 public:
  // IActivationFactory (unused for COM local server scenario)
  IFACEMETHODIMP ActivateInstance(IInspectable** instance) override {
    if (!instance)
      return E_POINTER;
    auto* activator = Microsoft::WRL::Make<NotificationToastActivator>().Detach();
    *instance = reinterpret_cast<IInspectable*>(activator);
    return S_OK;
  }
  // IClassFactory
  IFACEMETHODIMP CreateInstance(IUnknown* outer, REFIID riid, void** obj) override {
    if (outer)
      return CLASS_E_NOAGGREGATION;
    auto activator = Microsoft::WRL::Make<NotificationToastActivator>();
    return activator.CopyTo(riid, obj);
  }
  IFACEMETHODIMP LockServer(BOOL) override { return S_OK; }
};

}  // namespace

IFACEMETHODIMP NotificationToastActivator::Activate(
    HSTRING app_user_model_id,
    HSTRING invoked_args,
    const NOTIFICATION_USER_INPUT_DATA* data,
    ULONG data_count) {
  // Convert arguments and user input into command-line form. We launch (or
  // signal) Electron with a --toast-activation flag and key/value pairs so the
  // main process can locate the original notification.

  std::wstring args_w = invoked_args ? std::wstring(
                                          WindowsGetStringRawBuffer(invoked_args, nullptr))
                                     : L"";

  base::CommandLine command_line(base::CommandLine::NO_PROGRAM);
  // We'll rely on the shell executing the actual app path via shortcut/AUMID.
  command_line.AppendSwitch("toast-activation");
  if (!args_w.empty())
    command_line.AppendSwitchNative("toast-args", args_w);

  // Inline reply data (single text input id="reply").
  for (ULONG i = 0; i < data_count; ++i) {
    std::wstring key(data[i].Key ? data[i].Key : L"");
    std::wstring val(data[i].Value ? data[i].Value : L"");
    if (!key.empty()) {
      std::wstring sw = L"toast-input-" + key;
      command_line.AppendSwitchNative(base::WideToUTF8(sw).c_str(), val);
    }
  }

  // For now we just log; production code would pass this through a single
  // instance mechanism or relaunch logic.
  LOG(INFO) << "Toast activation: " << base::WideToUTF8(args_w);

  return S_OK;
}

HRESULT NotificationToastActivator::RegisterComServer() {
  if (g_com_cookie != 0)
    return S_OK;  // Already registered.
  Microsoft::WRL::ComPtr<IClassFactory> factory = Microsoft::WRL::Make<ToastActivatorFactory>();
  HRESULT hr = CoRegisterClassObject(kCLSID_NotificationToastActivator, factory.Get(), CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &g_com_cookie);
  if (FAILED(hr))
    LOG(ERROR) << "Failed to register toast activator COM server: " << std::hex << hr;
  return hr;
}

HRESULT NotificationToastActivator::UnregisterComServer() {
  if (g_com_cookie != 0) {
    CoRevokeClassObject(g_com_cookie);
    g_com_cookie = 0;
  }
  return S_OK;
}

bool NotificationToastActivator::IsRegistered() { return g_com_cookie != 0; }

}  // namespace electron
