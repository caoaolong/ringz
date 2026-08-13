// ===== Canvas Area: Container for the Leafer rendering surface =====

import { type RefObject } from 'react'

interface CanvasAreaProps {
  containerRef: RefObject<HTMLDivElement | null>
  isReady: boolean
  isEmpty: boolean
  onLoadSample?: () => void
}

export function CanvasArea({ containerRef, isReady, isEmpty, onLoadSample }: CanvasAreaProps) {
  return (
    <div className="canvas-area">
      <div ref={containerRef} className="canvas-container" />
      {isEmpty && isReady && (
        <div className="canvas-hint">
          <div className="hint-icon">🎨</div>
          <p className="hint-title">SVG 动画编辑器</p>
          <p className="hint-text">点击上方「导入SVG」按钮导入 SVG 文件</p>
          <p className="hint-text">或从工具栏添加形状开始创作</p>
          {onLoadSample && (
            <button className="hint-btn" onClick={onLoadSample}>
              加载示例 SVG
            </button>
          )}
        </div>
      )}
    </div>
  )
}
