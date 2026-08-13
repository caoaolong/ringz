// ===== Layer Panel: Left sidebar showing the element tree =====

import { useState, useMemo } from 'react'
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
  const [collapsedIds, setCollapsedIds] = useState<Set<string>>(new Set())

  // Filter out children of collapsed nodes
  const visibleLayers = useMemo(() => {
    if (collapsedIds.size === 0) return props.layers
    const result: LayerInfo[] = []
    let skipDepth = -1
    let skipParentId: string | null = null
    for (const layer of props.layers) {
      if (skipDepth >= 0 && layer.depth > skipDepth) {
        // Still inside a collapsed subtree
        continue
      }
      skipDepth = -1
      result.push(layer)
      if (collapsedIds.has(layer.id) && layer.hasChildren) {
        skipDepth = layer.depth
      }
    }
    return result
  }, [props.layers, collapsedIds])

  const toggleCollapse = (id: string) => {
    setCollapsedIds((prev) => {
      const next = new Set(prev)
      if (next.has(id)) next.delete(id)
      else next.add(id)
      return next
    })
  }

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
          visibleLayers.map((layer) => (
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
              {layer.hasChildren ? (
                <button
                  className="tree-toggle"
                  onClick={(e) => {
                    e.stopPropagation()
                    toggleCollapse(layer.id)
                  }}
                  title={collapsedIds.has(layer.id) ? '展开' : '折叠'}
                >
                  {collapsedIds.has(layer.id) ? '▶' : '▼'}
                </button>
              ) : (
                <span className="tree-toggle-placeholder" />
              )}
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
