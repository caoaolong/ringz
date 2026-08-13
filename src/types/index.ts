// ===== SVG Animation Editor - Type Definitions =====

/** Layer info displayed in the layer panel */
export interface LayerInfo {
  id: string
  name: string
  tag: string // 'Rect', 'Ellipse', 'Path', 'Group', etc.
  visible: boolean
  locked: boolean
  opacity: number
  depth: number
  parentId: string | null
  hasChildren: boolean
}

/** Animatable property keys */
export type AnimatableProp =
  | 'x'
  | 'y'
  | 'rotation'
  | 'scaleX'
  | 'scaleY'
  | 'opacity'
  | 'width'
  | 'height'

/** A single keyframe capturing element state at a point in time */
export interface Keyframe {
  id: string
  time: number // seconds
  props: Partial<Record<AnimatableProp, number>>
  easing: EasingType
}

export type EasingType =
  | 'linear'
  | 'easeIn'
  | 'easeOut'
  | 'easeInOut'
  | 'bounce'

/** Animation track for one element */
export interface AnimationTrack {
  elementId: string
  keyframes: Keyframe[]
}

/** Editable element properties shown in the property panel */
export interface ElementProps {
  x: number
  y: number
  width: number
  height: number
  rotation: number
  scaleX: number
  scaleY: number
  opacity: number
  fill: string
  stroke: string
  strokeWidth: number
  cornerRadius: number
}

/** Built-in shape presets for the toolbar */
export interface ShapePreset {
  type: 'Rect' | 'Ellipse' | 'Path' | 'Polygon' | 'Star' | 'Text'
  label: string
  icon: string
}

/** Snapshot of a single element for undo/redo and clipboard */
export interface ElementSnapshot {
  id: string
  tag: string
  name: string
  props: ElementProps
  visible: boolean
  locked: boolean
  // Type-specific extras
  path?: string
  text?: string
  fontSize?: number
  corners?: number
}

/** Full editor state snapshot for undo/redo */
export interface EditorSnapshot {
  elements: ElementSnapshot[]
  tracks: [string, AnimationTrack][]
  selectedId: string | null
  viewport?: { x: number; y: number; scaleX: number; scaleY: number }
}

export const SHAPE_PRESETS: ShapePreset[] = [
  { type: 'Rect', label: '矩形', icon: '▭' },
  { type: 'Ellipse', label: '圆形', icon: '◯' },
  { type: 'Star', label: '星形', icon: '★' },
  { type: 'Path', label: '路径', icon: '✎' },
  { type: 'Text', label: '文字', icon: 'T' },
]
