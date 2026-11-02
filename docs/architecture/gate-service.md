# Gate Service

The native gate service and the Lua snax.gateserver library are distinct entry points. The native module initializes its protocol and socket state; snax.gateserver exposes start/openclient/closeclient callbacks for Lua services. Choose the matching protocol and example rather than assuming a universal application gateway.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [service-src/service_gate.c:344](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_gate.c#L344) | `gate_init(` |
| [lualib/snax/gateserver.lua:33](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/snax/gateserver.lua#L33) | `function gateserver.start(` |
| [lualib/snax/gateserver.lua:19](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/snax/gateserver.lua#L19) | `function gateserver.openclient(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
