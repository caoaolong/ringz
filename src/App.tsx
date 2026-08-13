// ===== Main App: Assembles the SVG Animation Editor layout =====

import { useEffect, useRef, useState } from 'react'
import { useEditor } from './hooks/useEditor'
import { CanvasArea } from './components/CanvasArea'
import { Toolbar } from './components/Toolbar'
import { LayerPanel } from './components/LayerPanel'
import { PropertyPanel } from './components/PropertyPanel'
import { Timeline } from './components/Timeline'
import { ResizeHandle } from './components/ResizeHandle'
import { SAMPLE_SVG } from './utils/sampleSVG'
import './App.css'

export default function App() {
  const editor = useEditor()
  const editorRef = useRef(editor)
  editorRef.current = editor

  // Panel sizing state
  const [layerWidth, setLayerWidth] = useState(220)
  const [propertyWidth, setPropertyWidth] = useState(280)
  const [timelineHeight, setTimelineHeight] = useState(200)

  // Keyboard shortcuts
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      const target = e.target as HTMLElement
      if (target.tagName === 'INPUT' || target.tagName === 'TEXTAREA') return

      const ed = editorRef.current
      switch (e.key) {
        case 'Delete':
        case 'Backspace':
          e.preventDefault()
          ed.deleteSelected()
          break
        case 'd':
        case 'D':
          if (e.ctrlKey || e.metaKey) {
            e.preventDefault()
            ed.duplicateSelected()
          }
          break
        case ' ':
          e.preventDefault()
          if (ed.isPlaying) {
            ed.pauseAnimation()
          } else {
            ed.playAnimation()
          }
          break
      }
    }

    window.addEventListener('keydown', handleKeyDown)
    return () => window.removeEventListener('keydown', handleKeyDown)
  }, [])

  const hasElements = editor.layers.length > 0

  const loadSample = () => {
    editor.importSVGString(SAMPLE_SVG)
  }

  return (
    <div className="editor-app">
      <Toolbar
        onImportSVG={editor.importSVGFile}
        onAddShape={editor.addShape}
        onDelete={editor.deleteSelected}
        onDuplicate={editor.duplicateSelected}
        onClear={editor.clearCanvas}
        onExportSVG={editor.exportSVG}
        onExportPNG={editor.exportPNG}
        onPlay={editor.playAnimation}
        onPause={editor.pauseAnimation}
        onStop={editor.stopAnimation}
        isPlaying={editor.isPlaying}
        hasSelection={!!editor.selectedId}
        hasElements={hasElements}
      />
      <div className="main-content">
        <LayerPanel
          layers={editor.layers}
          selectedId={editor.selectedId}
          onSelect={editor.selectElement}
          onToggleVisibility={editor.toggleLayerVisibility}
          onToggleLock={editor.toggleLayerLock}
          onRename={editor.renameLayer}
          style={{ width: layerWidth }}
        />
        <ResizeHandle
          orientation="horizontal"
          initialSize={layerWidth}
          min={150}
          max={450}
          onResize={setLayerWidth}
        />
        <CanvasArea
          containerRef={editor.containerRef}
          isReady={editor.isReady}
          isEmpty={!hasElements}
          onLoadSample={loadSample}
        />
        <ResizeHandle
          orientation="horizontal"
          initialSize={propertyWidth}
          min={200}
          max={500}
          invert
          onResize={setPropertyWidth}
        />
        <PropertyPanel
          selectedProps={editor.selectedProps}
          selectedId={editor.selectedId}
          tracks={editor.tracks}
          currentTime={editor.currentTime}
          onUpdate={editor.updateProperty}
          onAddKeyframe={editor.addKeyframe}
          onRemoveKeyframe={editor.removeKeyframe}
          style={{ width: propertyWidth }}
        />
      </div>
      <ResizeHandle
        orientation="vertical"
        initialSize={timelineHeight}
        min={100}
        max={500}
        invert
        onResize={setTimelineHeight}
      />
      <Timeline
        tracks={editor.tracks}
        layers={editor.layers}
        currentTime={editor.currentTime}
        duration={editor.duration}
        isPlaying={editor.isPlaying}
        selectedId={editor.selectedId}
        onSeek={editor.seekAnimation}
        onPlay={editor.playAnimation}
        onPause={editor.pauseAnimation}
        onStop={editor.stopAnimation}
        onSetDuration={editor.setDuration}
        onAddKeyframe={editor.addKeyframe}
        style={{ height: timelineHeight }}
      />
    </div>
  )
}
