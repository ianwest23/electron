# NotificationAction Object

* `type` string - The type of action. Currently supported: `button` (macOS, Windows).
* `text` string (optional) - The label for the given action.
* `activationType` string (optional) _Windows_ - Overrides the toast action activation type. Allowed values: `foreground`, `background`, `protocol`. Defaults to `foreground` when omitted or invalid. (`system` is reserved for Electron’s internal dismiss action on persistent toasts.)

## Platform / Action Support

| Action Type | Platform Support | Usage of `text` | Default `text` | Limitations |
|-------------|------------------|-----------------|----------------|-------------|
| `button`    | macOS, Windows   | Used as the label for the button | macOS: "Show" (or localized default if first), Windows: none (your text is required) | macOS: Only first primary button shown (others as additional). Windows: All provided buttons rendered in order (OS may limit count); ignored if custom `toastXml` supplied. `hasReply` incompatibility applies only to macOS. Windows supports optional `activationType` (foreground/background/protocol). |

### Button support on macOS

In order for extra notification buttons to work on macOS your app must meet the
following criteria.

* App is signed
* App has it's `NSUserNotificationAlertStyle` set to `alert` in the `Info.plist`.

If either of these requirements are not met the button won't appear.
