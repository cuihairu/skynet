<template>
  <div class="mermaid-diagram">
    <div v-if="error" class="mermaid-error">
      <p>图表渲染错误: {{ error }}</p>
    </div>
    <div v-else ref="chartRef" class="mermaid-content"></div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch, nextTick } from 'vue'
import mermaid from 'mermaid'

const props = defineProps({
  code: {
    type: String,
    required: true
  },
  id: {
    type: String,
    default: () => `mermaid-${Math.random().toString(36).substr(2, 9)}`
  }
})

const chartRef = ref(null)
const error = ref(null)

const renderChart = async () => {
  if (!chartRef.value) return

  try {
    error.value = null
    mermaid.initialize({
      startOnLoad: false,
      theme: document.documentElement.classList.contains('dark') ? 'dark' : 'default',
      securityLevel: 'loose',
      fontFamily: 'sans-serif'
    })

    const { svg } = await mermaid.render(props.id, props.code)
    chartRef.value.innerHTML = svg
  } catch (e) {
    error.value = e.message
    console.error('Mermaid render error:', e)
  }
}

onMounted(() => {
  nextTick(renderChart)
})

watch(() => props.code, () => {
  nextTick(renderChart)
})

// Watch for theme changes
if (typeof MutationObserver !== 'undefined') {
  const observer = new MutationObserver(() => {
    nextTick(renderChart)
  })

  onMounted(() => {
    observer.observe(document.documentElement, {
      attributes: true,
      attributeFilter: ['class']
    })
  })
}
</script>

<style scoped>
.mermaid-diagram {
  margin: 1rem 0;
  padding: 1rem;
  background: var(--vp-c-bg-soft);
  border-radius: 8px;
  overflow-x: auto;
}

.mermaid-error {
  padding: 1rem;
  background: #fee;
  border: 1px solid #fcc;
  border-radius: 4px;
  color: #c00;
}

.mermaid-content {
  display: flex;
  justify-content: center;
}

.mermaid-content :deep(svg) {
  max-width: 100%;
  height: auto;
}
</style>
