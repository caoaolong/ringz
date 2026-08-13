// ===== Timeline: Bottom panel for animation keyframes and playback =====

import { useRef, useCallback, useState, useEffect } from 'react'
import type { CSSProperties } from 'react'
import type { AnimationTrack, LayerInfo } from '../types'

interface TimelineProps {
  tracks: Map<string, AnimationTrack>
  layers: LayerInfo[]
  currentTime: number
  duration: number
  isPlaying: boolean
  selectedId: string | null
  onSeek: (time: number) => void
  onPlay: () => void
  onPause: () => void
  onStop: () => void
  onSetDuration: (d: number) => void
  onAddKeyframe: () => void
  style?: CSSProperties
}

export function Timeline(props: TimelineProps) {
  const rulerRef = useRef<HTMLDivElement>(null)
  const [dragging, setDragging] = useState(false)

  const calcTime = useCallback(
    (clientX: number) => {
      const ruler = rulerRef.current
      if (!ruler) return 0
      const rect = ruler.getBoundingClientRect()
      const x = Math.max(0, Math.min(rect.width, clientX - rect.left))
      return (x / rect.width) * props.duration
    },
    [props.duration],
  )

  // Global mouse handlers for playhead dragging
  useEffect(() => {
    if (!dragging) return
    const onMove = (e: MouseEvent) => {
      props.onSeek(calcTime(e.clientX))
    }
    const onUp = () => setDragging(false)
    document.addEventListener('mousemove', onMove)
    document.addEventListener('mouseup', onUp)
    return () => {
      document.removeEventListener('mousemove', onMove)
      document.removeEventListener('mouseup', onUp)
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [dragging, calcTime])

  const playheadPercent = (props.currentTime / props.duration) * 100

  // Generate time markers
  const markers: number[] = []
  const step = props.duration <= 5 ? 0.5 : props.duration <= 10 ? 1 : props.duration <= 20 ? 2 : 5
  for (let t = 0; t <= props.duration + 0.01; t += step) {
    markers.push(Math.round(t * 100) / 100)
  }

  const trackEntries = Array.from(props.tracks.entries())

  return (
    <div className="timeline" style={props.style}>
      {/* Controls bar */}
      <div className="timeline-controls">
        <div className="tl-btn-group">
          <button
            className="tb-btn icon-only"
            onClick={props.isPlaying ? props.onPause : props.onPlay}
            title={props.isPlaying ? '暂停' : '播放'}
          >
            {props.isPlaying ? '⏸' : '▶'}
          </button>
          <button className="tb-btn icon-only" onClick={props.onStop} title="停止">
            ⏹
          </button>
        </div>

        <div className="time-display">
          <span className="time-current">{props.currentTime.toFixed(2)}s</span>
          <span className="time-sep">/</span>
          <span className="time-total">{props.duration.toFixed(1)}s</span>
        </div>

        <div className="duration-control">
          <label>时长</label>
          <input
            type="number"
            min={1}
            max={120}
            step={1}
            value={props.duration}
            onChange={(e) => props.onSetDuration(parseFloat(e.target.value) || 5)}
          />
          <span>s</span>
        </div>

        <button
          className="kf-btn"
          onClick={props.onAddKeyframe}
          disabled={!props.selectedId}
          title="在当前时间添加关键帧"
        >
          ◆ 添加关键帧
        </button>
      </div>

      {/* Timeline body */}
      <div className="timeline-body">
        {/* Ruler */}
        <div
          className="timeline-ruler-row"
          ref={rulerRef}
          onClick={(e) => props.onSeek(calcTime(e.clientX))}
        >
          <div className="ruler-label-col">时间轴</div>
          <div className="ruler-track-col">
            <div className="timeline-ruler">
              {markers.map((t) => (
                <div
                  key={t}
                  className="time-marker"
                  style={{ left: `${(t / props.duration) * 100}%` }}
                >
                  <div className="time-tick" />
                  <span className="time-label">{t.toFixed(1)}s</span>
                </div>
              ))}
            </div>
            {/* Playhead */}
            <div
              className="playhead"
              style={{ left: `${playheadPercent}%` }}
              onMouseDown={(e) => {
                e.stopPropagation()
                setDragging(true)
              }}
            >
              <div className="playhead-handle" />
              <div className="playhead-line" />
            </div>
          </div>
        </div>

        {/* Tracks */}
        <div className="timeline-tracks">
          {trackEntries.length === 0 ? (
            <div className="empty-hint" style={{ padding: '12px' }}>
              选择元素后点击「添加关键帧」创建动画轨道
            </div>
          ) : (
            trackEntries.map(([elementId, track]) => {
              const layer = props.layers.find((l) => l.id === elementId)
              const isSelected = props.selectedId === elementId
              return (
                <div
                  key={elementId}
                  className={`tl-track-row ${isSelected ? 'selected' : ''}`}
                >
                  <div className="track-label-col">
                    <span className="track-name" title={layer?.name || elementId}>
                      {layer?.name || elementId.substring(0, 12)}
                    </span>
                  </div>
                  <div className="track-bar-col">
                    <div className="track-bar">
                      {track.keyframes.map((kf) => (
                        <div
                          key={kf.id}
                          className="keyframe-marker"
                          style={{ left: `${(kf.time / props.duration) * 100}%` }}
                          title={`时间: ${kf.time.toFixed(2)}s\n属性: ${Object.keys(kf.props).join(', ')}`}
                          onClick={(e) => {
                            e.stopPropagation()
                            props.onSeek(kf.time)
                          }}
                        >
                          ◆
                        </div>
                      ))}
                    </div>
                  </div>
                </div>
              )
            })
          )}
        </div>
      </div>
    </div>
  )
}
