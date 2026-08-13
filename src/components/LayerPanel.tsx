// ===== Layer Panel: Left sidebar showing the element tree =====

import { useState } from 'react'
import type { CSSProperties } from 'react'
import type { LayerInfo } from '../types'

interface LayerPanelProps {
  layers: LayerInfo[]
  selectedId: string | null
  onSelect: (id: string) => void
  onToggleVisibility: (id: string) => void
  onToggleLock: (id: string) => void
  onRename: (id: string, name: string) => void
  style?: CSSProperties
}

const TAG_ICONS: Record<string, string> = {
  Rect: '▭',
  Ellipse: '◯',
  Path: '✎',
  Polygon: '⬠',
  Line: '╱',
  Text: 'T',
  Star: '★',
  Group: '📁',
  UI: '◻',
}

export function LayerPanel(props: LayerPanelProps) {
  const [editingId, setEditingId] = useState<string | null>(null)
  const [editValue, setEditValue] = useState('')

  const commitRename = () => {
    if (editingId) {
      props.onRename(editingId, editValue || 'Untitled')
      setEditingId(null)
    }
  }

  return (
    <div className="panel layer-panel" style={props.style}>
      <div className="panel-header">
        <span>图层</span>
        <span className="panel-count">{props.layers.length}</span>
      </div>
      <div className="panel-body layer-list">
        {props.layers.length === 0 ? (
          <div className="empty-hint">暂无图层</div>
        ) : (
          props.layers.map((layer) => (
            <div
              key={layer.id}
              className={`layer-item ${props.selectedId === layer.id ? 'selected' : ''} ${
                !layer.visible ? 'hidden' : ''
              }`}
              style={{ paddingLeft: 8 + layer.depth * 16 }}
              onClick={() => props.onSelect(layer.id)}
              onDoubleClick={() => {
                setEditingId(layer.id)
                setEditValue(layer.name)
              }}
            >
              <span className="layer-icon">{TAG_ICONS[layer.tag] || '◻'}</span>
              {editingId === layer.id ? (
                <input
                  className="layer-name-input"
                  value={editValue}
                  autoFocus
                  onChange={(e) => setEditValue(e.target.value)}
                  onBlur={commitRename}
                  onKeyDown={(e) => {
                    if (e.key === 'Enter') commitRename()
                    if (e.key === 'Escape') setEditingId(null)
                  }}
                  onClick={(e) => e.stopPropagation()}
                />
              ) : (
                <span className="layer-name">{layer.name}</span>
              )}
              <span className="layer-actions">
                <button
                  className="icon-btn"
                  onClick={(e) => {
                    e.stopPropagation()
                    props.onToggleVisibility(layer.id)
                  }}
                  title={layer.visible ? '隐藏' : '显示'}
                >
                  {layer.visible ? '👁' : '🚫'}
                </button>
                <button
                  className="icon-btn"
                  onClick={(e) => {
                    e.stopPropagation()
                    props.onToggleLock(layer.id)
                  }}
                  title={layer.locked ? '解锁' : '锁定'}
                >
                  {layer.locked ? '🔒' : '🔓'}
                </button>
              </span>
            </div>
          ))
        )}
      </div>
    </div>
  )
}
