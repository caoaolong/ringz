// ===== Canvas Area: Container for the Leafer rendering surface =====

import { useRef, type RefObject } from 'react'

interface CanvasAreaProps {
  containerRef: RefObject<HTMLDivElement | null>
  isReady: boolean
  isEmpty: boolean
  onImportSVG: (file: File) => void
}

export function CanvasArea({ containerRef, isReady, isEmpty, onImportSVG }: CanvasAreaProps) {
  const fileInputRef = useRef<HTMLInputElement>(null)

  return (
    <div className="canvas-area">
      <div ref={containerRef} className="canvas-container" />
      {isEmpty && isReady && (
        <div className="canvas-hint">
          <div className="hint-icon">🎨</div>
          <p className="hint-title">SVG 动画编辑器</p>
          <p className="hint-text">点击下方按钮导入 SVG 文件</p>
          <p className="hint-text">或从工具栏添加形状开始创作</p>
          <button className="hint-btn" onClick={() => fileInputRef.current?.click()}>
            导入 SVG
          </button>
          <input
            ref={fileInputRef}
            type="file"
            accept=".svg,image/svg+xml"
            style={{ display: 'none' }}
            onChange={(e) => {
              const file = e.target.files?.[0]
              if (file) onImportSVG(file)
              e.target.value = ''
            }}
          />
        </div>
      )}
    </div>
  )
}
