// ===== Property Panel: Right sidebar for editing selected element properties =====

import type { CSSProperties } from 'react'
import type { ElementProps, AnimationTrack } from '../types'

interface PropertyPanelProps {
  selectedProps: ElementProps | null
  selectedId: string | null
  tracks: Map<string, AnimationTrack>
  currentTime: number
  onUpdate: <K extends keyof ElementProps>(prop: K, value: ElementProps[K]) => void
  onAddKeyframe: () => void
  onRemoveKeyframe: (elementId: string, keyframeId: string) => void
  style?: CSSProperties
}

export function PropertyPanel(props: PropertyPanelProps) {
  if (!props.selectedProps) {
    return (
      <div className="panel property-panel" style={props.style}>
        <div className="panel-header">属性</div>
        <div className="panel-body">
          <div className="empty-hint">选择一个元素以编辑属性</div>
        </div>
      </div>
    )
  }

  const p = props.selectedProps
  const track = props.selectedId ? props.tracks.get(props.selectedId) : null
  const hasKeyframeAtTime =
    track?.keyframes.some((kf) => Math.abs(kf.time - props.currentTime) < 0.05) ?? false

  const numInput = (
    label: string,
    prop: keyof ElementProps,
    step = 1,
  ) => (
    <div className="prop-field" key={prop}>
      <label className="prop-label">{label}</label>
      <input
        className="prop-input"
        type="number"
        step={step}
        value={p[prop] as number}
        onChange={(e) => {
          const val = parseFloat(e.target.value)
          props.onUpdate(prop, (isNaN(val) ? 0 : val) as any)
        }}
      />
    </div>
  )

  return (
    <div className="panel property-panel" style={props.style}>
      <div className="panel-header">
        <span>属性</span>
        <button
          className={`kf-btn ${hasKeyframeAtTime ? 'active' : ''}`}
          onClick={props.onAddKeyframe}
          title="在当前时间添加关键帧"
          disabled={!props.selectedId}
        >
          ◆ 关键帧
        </button>
      </div>
      <div className="panel-body">
        {/* Position & Size */}
        <div className="prop-section">
          <div className="prop-section-title">位置 & 尺寸</div>
          <div className="prop-grid">
            {numInput('X', 'x')}
            {numInput('Y', 'y')}
            {numInput('宽', 'width')}
            {numInput('高', 'height')}
          </div>
        </div>

        {/* Transform */}
        <div className="prop-section">
          <div className="prop-section-title">变换</div>
          <div className="prop-grid">
            {numInput('旋转°', 'rotation')}
            {numInput('圆角', 'cornerRadius')}
            {numInput('缩放X', 'scaleX', 0.1)}
            {numInput('缩放Y', 'scaleY', 0.1)}
          </div>
        </div>

        {/* Appearance */}
        <div className="prop-section">
          <div className="prop-section-title">外观</div>
          <div className="prop-row">
            <label className="prop-label">填充</label>
            <div className="color-group">
              <input
                type="color"
                className="color-picker"
                value={p.fill === 'transparent' || p.fill === 'none' ? '#000000' : p.fill}
                onChange={(e) => props.onUpdate('fill', e.target.value as any)}
              />
              <input
                className="prop-input flex-1"
                type="text"
                value={p.fill}
                onChange={(e) => props.onUpdate('fill', e.target.value as any)}
              />
            </div>
          </div>
          <div className="prop-row">
            <label className="prop-label">描边</label>
            <div className="color-group">
              <input
                type="color"
                className="color-picker"
                value={
                  p.stroke === 'none' || p.stroke === '' ? '#000000' : p.stroke
                }
                onChange={(e) => props.onUpdate('stroke', e.target.value as any)}
              />
              <input
                className="prop-input flex-1"
                type="text"
                value={p.stroke}
                onChange={(e) => props.onUpdate('stroke', e.target.value as any)}
              />
            </div>
          </div>
          <div className="prop-grid">
            {numInput('描边宽', 'strokeWidth')}
            {numInput('透明度', 'opacity', 0.05)}
          </div>
        </div>

        {/* Animation Track */}
        {track && (
          <div className="prop-section">
            <div className="prop-section-title">
              动画轨道
              <span className="badge">{track.keyframes.length} 帧</span>
            </div>
            <div className="kf-list">
              {track.keyframes.map((kf) => (
                <div key={kf.id} className="kf-list-item">
                  <span className="kf-diamond">◆</span>
                  <span className="kf-time">{kf.time.toFixed(2)}s</span>
                  <span className="kf-props">
                    {Object.keys(kf.props).join(', ')}
                  </span>
                  <button
                    className="icon-btn danger"
                    onClick={() =>
                      props.selectedId &&
                      props.onRemoveKeyframe(props.selectedId, kf.id)
                    }
                    title="删除关键帧"
                  >
                    ✕
                  </button>
                </div>
              ))}
            </div>
          </div>
        )}
      </div>
    </div>
  )
}
