# Framework Comparison Scope

This repository does not include a controlled cross-framework benchmark in these documentation pages. No throughput or latency ranking is asserted here. For a deployment decision, measure the application workload, coroutine interleaving and native blocking behavior against the alternatives.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_start.c:157](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L157) | `thread_worker(` |
| [lualib/skynet.lua:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L517) | `function skynet.sleep(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
