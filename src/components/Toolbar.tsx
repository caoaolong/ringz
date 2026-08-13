// ===== Toolbar: Top bar with import, shapes, edit, animation, export controls =====

import { useRef } from 'react'
import { SHAPE_PRESETS } from '../types'

interface ToolbarProps {
  onImportSVG: (file: File) => void
  onAddShape: (type: string) => void
  onDelete: () => void
  onDuplicate: () => void
  onClear: () => void
  onExportSVG: () => void
  onExportPNG: () => void
  onPlay: () => void
  onPause: () => void
  onStop: () => void
  isPlaying: boolean
  hasSelection: boolean
  hasElements: boolean
}

export function Toolbar(props: ToolbarProps) {
  const fileInputRef = useRef<HTMLInputElement>(null)

  return (
    <div className="toolbar">
      <div className="toolbar-brand">
        <span className="brand-icon">✦</span>
        <span className="brand-text">SVG Animator</span>
      </div>

      <div className="toolbar-divider" />

      <div className="toolbar-group">
        <button className="tb-btn primary" onClick={() => fileInputRef.current?.click()}>
          <span className="tb-icon">📂</span>
          <span>导入SVG</span>
        </button>
        <input
          ref={fileInputRef}
          type="file"
          accept=".svg,image/svg+xml"
          style={{ display: 'none' }}
          onChange={(e) => {
            const file = e.target.files?.[0]
            if (file) props.onImportSVG(file)
            e.target.value = ''
          }}
        />
      </div>

      <div className="toolbar-divider" />

      <div className="toolbar-group">
        {SHAPE_PRESETS.map((preset) => (
          <button
            key={preset.type}
            className="tb-btn icon-only"
            onClick={() => props.onAddShape(preset.type)}
            title={preset.label}
          >
            {preset.icon}
          </button>
        ))}
      </div>

      <div className="toolbar-divider" />

      <div className="toolbar-group">
        <button
          className="tb-btn icon-only"
          disabled={!props.hasSelection}
          onClick={props.onDuplicate}
          title="复制 (Ctrl+D)"
        >
          ⧉
        </button>
        <button
          className="tb-btn icon-only danger"
          disabled={!props.hasSelection}
          onClick={props.onDelete}
          title="删除 (Delete)"
        >
          🗑
        </button>
        <button
          className="tb-btn icon-only"
          disabled={!props.hasElements}
          onClick={props.onClear}
          title="清空画布"
        >
          ✕
        </button>
      </div>

      <div className="toolbar-divider" />

      <div className="toolbar-group">
        {props.isPlaying ? (
          <button className="tb-btn" onClick={props.onPause} title="暂停">
            <span className="tb-icon">⏸</span>
            <span>暂停</span>
          </button>
        ) : (
          <button className="tb-btn primary" onClick={props.onPlay} title="播放动画">
            <span className="tb-icon">▶</span>
            <span>播放</span>
          </button>
        )}
        <button className="tb-btn icon-only" onClick={props.onStop} title="停止">
          ⏹
        </button>
      </div>

      <div className="toolbar-spacer" />

      <div className="toolbar-group">
        <button className="tb-btn" disabled={!props.hasElements} onClick={props.onExportSVG} title="导出 SVG">
          <span className="tb-icon">💾</span>
          <span>SVG</span>
        </button>
        <button className="tb-btn" disabled={!props.hasElements} onClick={props.onExportPNG} title="导出 PNG">
          <span className="tb-icon">🖼</span>
          <span>PNG</span>
        </button>
      </div>
    </div>
  )
}
