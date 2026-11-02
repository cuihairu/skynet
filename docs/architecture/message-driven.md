# Message-Driven Design

skynet.send sends a one-way message. skynet.call associates a session with a request and yields for the reply. A handler returns serialized values with skynet.retpack. skynet.response returns a function for a delayed reply; discarding that function is not a general-purpose "ignore message" API. skynet.ignoreret clears the current request session association.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [lualib/skynet.lua:694](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L694) | `function skynet.send(` |
| [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) | `function skynet.call(` |
| [lualib/skynet.lua:843](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L843) | `function skynet.retpack(` |
| [lualib/skynet.lua:801](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L801) | `function skynet.response(` |
| [lualib/skynet.lua:796](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L796) | `function skynet.ignoreret(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
