<script>
  import { onMount } from "svelte";
  import { getNativeFunction } from "@juce/index.js";

  let pluginName = "Harmonic Sequencer";
  let version = "0.0.1";

  onMount(async () => {
    try {
      const getPluginInfo = getNativeFunction("getPluginInfo");
      const info = await getPluginInfo();
      if (info?.name) pluginName = info.name;
      if (info?.version) version = info.version;
    } catch {
      // Dev server without JUCE bridge
    }
  });
</script>

<main class="flex h-full flex-col items-center justify-center gap-4 p-8 text-center">
  <h1 class="text-2xl font-semibold tracking-tight text-zinc-50">{pluginName}</h1>
  <p class="max-w-md text-sm text-zinc-400">
    MIDI effect focused on high-performance CC sequencing to your DAW. UI and engine wiring come next.
  </p>
  <p class="text-xs text-zinc-600">v{version}</p>
</main>
