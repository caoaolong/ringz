// ===== ResizeHandle: Draggable divider for resizing panels =====

import { useRef, useCallback } from 'react'

interface ResizeHandleProps {
  /** 'horizontal' = drag left/right (adjusts width), 'vertical' = drag up/down (adjusts height) */
  orientation: 'horizontal' | 'vertical'
  /** Current size of the panel being resized (captured at drag start) */
  initialSize: number
  /** Minimum size in px */
  min: number
  /** Maximum size in px */
  max: number
  /** If true, mouse delta is inverted (for right/bottom panels where dragging left/up increases size) */
  invert?: boolean
  /** Called with the new size on every mousemove */
  onResize: (newSize: number) => void
}

export function ResizeHandle({
  orientation,
  initialSize,
  min,
  max,
  invert = false,
  onResize,
}: ResizeHandleProps) {
  // Use ref to always call the latest onResize without re-registering listeners
  const onResizeRef = useRef(onResize)
  onResizeRef.current = onResize

  const handleMouseDown = useCallback(
    (e: React.MouseEvent) => {
      e.preventDefault()
      e.stopPropagation()

      const startPos = orientation === 'horizontal' ? e.clientX : e.clientY
      const startSize = initialSize
      const factor = invert ? -1 : 1

      const onMove = (ev: MouseEvent) => {
        const currentPos = orientation === 'horizontal' ? ev.clientX : ev.clientY
        const delta = (currentPos - startPos) * factor
        const newSize = Math.max(min, Math.min(max, startSize + delta))
        onResizeRef.current(newSize)
      }

      const onUp = () => {
        document.removeEventListener('mousemove', onMove)
        document.removeEventListener('mouseup', onUp)
        document.body.style.cursor = ''
        document.body.style.userSelect = ''
      }

      document.addEventListener('mousemove', onMove)
      document.addEventListener('mouseup', onUp)
      document.body.style.cursor = orientation === 'horizontal' ? 'col-resize' : 'row-resize'
      document.body.style.userSelect = 'none'
    },
    [orientation, initialSize, min, max, invert],
  )

  return (
    <div
      className={`resize-handle resize-${orientation}`}
      onMouseDown={handleMouseDown}
    />
  )
}
