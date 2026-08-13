// ===== Animation Engine: Keyframe interpolation and playback =====

import type { IUI } from 'leafer-ui'
import type { Keyframe, AnimatableProp, EasingType } from '../types'

export type ElementFinder = (id: string) => IUI | undefined

/** Easing functions */
const EASING_FUNCTIONS: Record<EasingType, (t: number) => number> = {
  linear: (t) => t,
  easeIn: (t) => t * t,
  easeOut: (t) => 1 - (1 - t) * (1 - t),
  easeInOut: (t) => (t < 0.5 ? 2 * t * t : 1 - Math.pow(-2 * t + 2, 2) / 2),
  bounce: (t) => {
    const n1 = 7.5625
    const d1 = 2.75
    if (t < 1 / d1) return n1 * t * t
    if (t < 2 / d1) return n1 * (t -= 1.5 / d1) * t + 0.75
    if (t < 2.5 / d1) return n1 * (t -= 2.25 / d1) * t + 0.9375
    return n1 * (t -= 2.625 / d1) * t + 0.984375
  },
}

/** Interpolate between two keyframes at a given time */
export function interpolateKeyframes(
  k1: Keyframe,
  k2: Keyframe,
  time: number,
): Partial<Record<AnimatableProp, number>> {
  const duration = k2.time - k1.time
  if (duration <= 0) return k1.props

  let progress = (time - k1.time) / duration
  progress = Math.max(0, Math.min(1, progress))

  const eased = EASING_FUNCTIONS[k2.easing](progress)
  const result: Partial<Record<AnimatableProp, number>> = {}

  // Interpolate all properties present in either keyframe
  const allProps = new Set([...Object.keys(k1.props), ...Object.keys(k2.props)]) as Set<AnimatableProp>
  for (const prop of allProps) {
    const v1 = k1.props[prop]
    const v2 = k2.props[prop]
    if (v1 !== undefined && v2 !== undefined) {
      result[prop] = v1 + (v2 - v1) * eased
    } else if (v2 !== undefined) {
      result[prop] = v2
    } else if (v1 !== undefined) {
      result[prop] = v1
    }
  }

  return result
}

/** Get the interpolated props for an element at a specific time */
export function getPropsAtTime(
  keyframes: Keyframe[],
  time: number,
): Partial<Record<AnimatableProp, number>> {
  if (keyframes.length === 0) return {}
  if (keyframes.length === 1) return keyframes[0].props

  const sorted = [...keyframes].sort((a, b) => a.time - b.time)

  // Before first keyframe
  if (time <= sorted[0].time) return sorted[0].props

  // After last keyframe
  if (time >= sorted[sorted.length - 1].time) return sorted[sorted.length - 1].props

  // Find surrounding keyframes
  for (let i = 0; i < sorted.length - 1; i++) {
    if (time >= sorted[i].time && time <= sorted[i + 1].time) {
      return interpolateKeyframes(sorted[i], sorted[i + 1], time)
    }
  }

  return sorted[0].props
}

/** Animation player that manages playback */
export class AnimationPlayer {
  private tracks: Map<string, Keyframe[]> = new Map()
  private originalStates: Map<string, Partial<Record<AnimatableProp, number>>> = new Map()
  private findElement: ElementFinder
  private currentTime = 0
  private duration = 5 // seconds
  private playing = false
  private loop = true
  private rafId: number | null = null
  private lastTimestamp = 0
  private onTimeUpdate: ((time: number) => void) | null = null

  constructor(findElement: ElementFinder) {
    this.findElement = findElement
  }

  setTracks(tracks: Map<string, Keyframe[]>): void {
    this.tracks = tracks
    // Update duration based on max keyframe time
    let maxTime = 0
    for (const kfs of tracks.values()) {
      for (const kf of kfs) {
        if (kf.time > maxTime) maxTime = kf.time
      }
    }
    if (maxTime > 0) this.duration = maxTime
  }

  setOnTimeUpdate(cb: (time: number) => void): void {
    this.onTimeUpdate = cb
  }

  /** Capture the original state of an element before animation */
  captureOriginalState(elementId: string, element: IUI): void {
    if (this.originalStates.has(elementId)) return
    this.originalStates.set(elementId, {
      x: element.x,
      y: element.y,
      rotation: element.rotation,
      scaleX: element.scaleX,
      scaleY: element.scaleY,
      opacity: element.opacity,
      width: element.width,
      height: element.height,
    })
  }

  /** Restore all elements to their original state */
  restoreOriginalStates(): void {
    for (const [id, state] of this.originalStates) {
      const el = this.findElement(id)
      if (el) {
        el.set(state as any)
      }
    }
  }

  /** Apply animation at a specific time */
  applyAtTime(time: number): void {
    for (const [elementId, keyframes] of this.tracks) {
      if (keyframes.length === 0) continue
      const el = this.findElement(elementId)
      if (!el) continue

      // Capture original state on first apply
      this.captureOriginalState(elementId, el)

      const props = getPropsAtTime(keyframes, time)
      if (Object.keys(props).length > 0) {
        el.set(props as any)
      }
    }
  }

  play(): void {
    if (this.playing) return
    if (this.currentTime >= this.duration) {
      this.currentTime = 0
    }
    this.playing = true
    this.lastTimestamp = performance.now()
    this.tick()
  }

  pause(): void {
    this.playing = false
    if (this.rafId !== null) {
      cancelAnimationFrame(this.rafId)
      this.rafId = null
    }
  }

  stop(): void {
    this.pause()
    this.currentTime = 0
    this.restoreOriginalStates()
    this.onTimeUpdate?.(0)
  }

  seek(time: number): void {
    this.currentTime = Math.max(0, Math.min(this.duration, time))
    this.applyAtTime(this.currentTime)
    this.onTimeUpdate?.(this.currentTime)
  }

  setDuration(duration: number): void {
    this.duration = duration
  }

  getDuration(): number {
    return this.duration
  }

  getCurrentTime(): number {
    return this.currentTime
  }

  isPlaying(): boolean {
    return this.playing
  }

  setLoop(loop: boolean): void {
    this.loop = loop
  }

  private tick = (): void => {
    if (!this.playing) return

    const now = performance.now()
    const delta = (now - this.lastTimestamp) / 1000 // seconds
    this.lastTimestamp = now

    this.currentTime += delta

    if (this.currentTime >= this.duration) {
      if (this.loop) {
        this.currentTime = this.currentTime % this.duration
      } else {
        this.currentTime = this.duration
        this.applyAtTime(this.currentTime)
        this.onTimeUpdate?.(this.currentTime)
        this.pause()
        return
      }
    }

    this.applyAtTime(this.currentTime)
    this.onTimeUpdate?.(this.currentTime)

    this.rafId = requestAnimationFrame(this.tick)
  }
}
