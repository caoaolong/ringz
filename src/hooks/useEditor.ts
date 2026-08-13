// ===== Core Editor Hook: Manages Leafer App, state, and all editor operations =====

import { useState, useRef, useEffect, useCallback } from 'react'
import {
  App,
  Rect,
  Ellipse,
  Path,
  Text,
  Star,
  type IUI,
} from 'leafer-ui'
import '@leafer-in/editor'
import '@leafer-in/viewport'
import '@leafer-in/state'
import '@leafer-in/export'
import { parseSVG, generateElementId } from '../utils/svgParser'
import { AnimationPlayer } from '../utils/animationEngine'
import { exportToSVG, downloadFile } from '../utils/svgExporter'
import type {
  LayerInfo,
  ElementProps,
  AnimationTrack,
  Keyframe,
  ElementSnapshot,
  EditorSnapshot,
} from '../types'

/** Extract editable properties from a Leafer element */
function extractProps(el: IUI): ElementProps {
  const fill = el.fill
  const stroke = el.stroke
  return {
    x: Math.round(el.x || 0),
    y: Math.round(el.y || 0),
    width: Math.round(el.width || 0),
    height: Math.round(el.height || 0),
    rotation: Math.round((el.rotation || 0) * 100) / 100,
    scaleX: Math.round((el.scaleX || 1) * 100) / 100,
    scaleY: Math.round((el.scaleY || 1) * 100) / 100,
    opacity: el.opacity ?? 1,
    fill: typeof fill === 'string' ? fill : '#4A90D9',
    stroke: typeof stroke === 'string' ? stroke : 'none',
    strokeWidth: typeof el.strokeWidth === 'number' ? el.strokeWidth : Number(el.strokeWidth) || 0,
    cornerRadius: typeof (el as any).cornerRadius === 'number' ? (el as any).cornerRadius : 0,
  }
}

/** Recursively track elements in the map with unique IDs */
function trackElement(el: IUI, map: Map<string, IUI>): void {
  const id = String(el.id || `auto_${el.innerId}`)
  if (!el.id) el.id = id
  map.set(id, el)
  const children = (el as any).children as IUI[] | undefined
  if (children) {
    for (const child of children) {
      trackElement(child, map)
    }
  }
}

/** Recursively build a flat layer list with depth info */
function buildLayerList(elements: IUI[], depth: number, parentId: string | null = null): LayerInfo[] {
  const layers: LayerInfo[] = []
  const arr = elements as IUI[]
  for (let i = arr.length - 1; i >= 0; i--) {
    const el = arr[i]
    const id = String(el.id || el.innerId)
    const children = (el as any).children as IUI[] | undefined
    const hasChildren = !!(children && children.length > 0)
    layers.push({
      id,
      name: (el as any).name || el.tag || 'Element',
      tag: el.tag || 'UI',
      visible: el.visible !== false,
      locked: (el as any).locked === true,
      opacity: el.opacity ?? 1,
      depth,
      parentId,
      hasChildren,
    })
    if (hasChildren) {
      layers.push(...buildLayerList(children, depth + 1, id))
    }
  }
  return layers
}

/** Serialize a Leafer element to a plain snapshot */
function serializeElement(el: IUI): ElementSnapshot {
  const tag = el.tag || 'UI'
  const id = String(el.id || el.innerId)
  const snap: ElementSnapshot = {
    id,
    tag,
    name: (el as any).name || tag,
    props: extractProps(el),
    visible: el.visible !== false,
    locked: (el as any).locked === true,
  }
  if (tag === 'Path') snap.path = (el as any).path
  if (tag === 'Text') {
    snap.text = (el as any).text
    snap.fontSize = (el as any).fontSize
  }
  if (tag === 'Star') snap.corners = (el as any).corners
  return snap
}

/** Recreate a Leafer element from a snapshot */
function createElementFromSnapshot(snap: ElementSnapshot): IUI {
  const { tag, props, name } = snap
  const base = { ...props, editable: true, name }
  let element: IUI
  switch (tag) {
    case 'Ellipse':
      element = new Ellipse(base)
      break
    case 'Star':
      element = new Star({ ...base, corners: snap.corners ?? 5 } as any)
      break
    case 'Path':
      element = new Path({ ...base, path: snap.path || 'M 0 0 L 50 0 L 25 50 Z' } as any)
      break
    case 'Text':
      element = new Text({ ...base, text: snap.text || '', fontSize: snap.fontSize ?? 32 } as any)
      break
    default:
      element = new Rect(base)
  }
  ;(element as any).id = snap.id
  element.visible = snap.visible
  ;(element as any).locked = snap.locked
  ;(element as any).hittable = !snap.locked
  return element
}

export function useEditor() {
  const containerRef = useRef<HTMLDivElement>(null)
  const appRef = useRef<App | null>(null)
  const elementMap = useRef<Map<string, IUI>>(new Map())
  const playerRef = useRef<AnimationPlayer | null>(null)
  const updateScheduled = useRef(false)

  const [isReady, setIsReady] = useState(false)
  const [selectedId, setSelectedId] = useState<string | null>(null)
  const [selectedProps, setSelectedProps] = useState<ElementProps | null>(null)
  const [layers, setLayers] = useState<LayerInfo[]>([])
  const [tracks, setTracks] = useState<Map<string, AnimationTrack>>(new Map())
  const [currentTime, setCurrentTime] = useState(0)
  const [duration, setDurationState] = useState(5)
  const [isPlaying, setIsPlaying] = useState(false)

  // ===== History (undo/redo) =====
  const historyRef = useRef<EditorSnapshot[]>([])
  const historyIndexRef = useRef(-1)
  const isRestoringRef = useRef(false)
  const historyDebounceRef = useRef<ReturnType<typeof setTimeout> | null>(null)
  const [canUndo, setCanUndo] = useState(false)
  const [canRedo, setCanRedo] = useState(false)

  // ===== Clipboard =====
  const clipboardRef = useRef<ElementSnapshot | null>(null)

  // ===== Refs mirroring state for use in stable callbacks =====
  const tracksRef = useRef<Map<string, AnimationTrack>>(new Map())
  const selectedIdRef = useRef<string | null>(null)
  const pushHistoryDebouncedRef = useRef<(() => void) | null>(null)
  const pushHistoryRef = useRef<(() => void) | null>(null)
  tracksRef.current = tracks
  selectedIdRef.current = selectedId

  // ===== Initialize Leafer App =====
  useEffect(() => {
    if (!containerRef.current) return

    const app = new App({
      view: containerRef.current,
      editor: {
        boxSelect: true,
        hover: true,
      },
      tree: {},
      sky: {},
    })

    appRef.current = app

    // Set transparent canvas background so CSS grid shows through
    const tree = (app as any).tree
    if (tree) {
      tree.fill = 'transparent'
    }

    // Set up editor event listeners
    const editor = (app as any).editor

    // ===== Hierarchical (drill-down) selection =====
    // Single click selects the top-level parent; double-click drills into children.
    let isOverriding = false // re-entry guard for editor.select override
    let isDrilling = false    // set by dblclick to allow deeper selection

    /** Walk up parent chain to find the direct child of tree */
    const findTopLevelParent = (el: any): any => {
      let current = el
      while (current?.parent && current.parent !== tree) {
        current = current.parent
      }
      return current
    }

    /** Check if a world-space point is inside an element's world bounds */
    const containsPoint = (el: any, wx: number, wy: number): boolean => {
      const b = el.worldBounds || el.boxBounds
      if (!b) return false
      return wx >= b.x && wx <= b.x + b.width && wy >= b.y && wy <= b.y + b.height
    }

    editor.on('editor.select', () => {
      // If drilling (from dblclick), allow the selection as-is
      if (isDrilling) {
        isDrilling = false
        const element = editor.element
        if (element) {
          const id = String(element.id || element.innerId)
          setSelectedId(id)
          setSelectedProps(extractProps(element))
        } else {
          setSelectedId(null)
          setSelectedProps(null)
        }
        return
      }

      // If this is from our own override call, just sync state
      if (isOverriding) {
        isOverriding = false
        const element = editor.element
        if (element) {
          const id = String(element.id || element.innerId)
          setSelectedId(id)
          setSelectedProps(extractProps(element))
        } else {
          setSelectedId(null)
          setSelectedProps(null)
        }
        return
      }

      const element = editor.element
      if (!element) {
        setSelectedId(null)
        setSelectedProps(null)
        return
      }

      // Override: select the top-level parent instead of the deep child
      const topLevel = findTopLevelParent(element)
      if (topLevel && topLevel !== element) {
        isOverriding = true
        editor.select(topLevel)
      } else {
        const id = String(element.id || element.innerId)
        setSelectedId(id)
        setSelectedProps(extractProps(element))
      }
    })

    // Double-click: drill into the child at the click point
    containerRef.current!.addEventListener('dblclick', (e: MouseEvent) => {
      const element = editor.element
      if (!element) return

      const children = (element as any).children as any[]
      if (!children || children.length === 0) return

      // Convert screen coords to world coords
      const rect = containerRef.current!.getBoundingClientRect()
      const mx = e.clientX - rect.left
      const my = e.clientY - rect.top
      const wx = (mx - (tree.x ?? 0)) / (tree.scaleX ?? 1)
      const wy = (my - (tree.y ?? 0)) / (tree.scaleY ?? 1)

      // Find topmost child containing the point
      for (let i = children.length - 1; i >= 0; i--) {
        if (containsPoint(children[i], wx, wy)) {
          isDrilling = true
          editor.select(children[i])
          break
        }
      }
    })

    editor.on('editor.update', () => {
      if (updateScheduled.current) return
      updateScheduled.current = true
      requestAnimationFrame(() => {
        updateScheduled.current = false
        const element = editor.element
        if (element) {
          setSelectedProps(extractProps(element))
        }
      })
      // Debounced history push for canvas interactions (drag, resize, rotate)
      if (!isRestoringRef.current) {
        pushHistoryDebouncedRef.current?.()
      }
    })

    // Create animation player
    playerRef.current = new AnimationPlayer((id) => elementMap.current.get(id))
    playerRef.current.setOnTimeUpdate(setCurrentTime)

    // ===== Wheel zoom around mouse position =====
    const container = containerRef.current!

    // Force editor selection box to follow tree transform changes
    const syncEditor = () => {
      if (editor && editor.element) {
        editor.update()
      }
    }

    const onWheel = (e: WheelEvent) => {
      e.preventDefault()
      const tree = (app as any).tree
      if (!tree) return

      const rect = container.getBoundingClientRect()
      const mouseX = e.clientX - rect.left
      const mouseY = e.clientY - rect.top

      const oldScale = tree.scaleX ?? 1
      const factor = e.deltaY > 0 ? 0.9 : 1.1
      const newScale = Math.max(0.05, Math.min(20, oldScale * factor))

      if (Math.abs(newScale - oldScale) < 0.001) return

      // World coordinates of mouse before zoom
      const worldX = (mouseX - (tree.x ?? 0)) / oldScale
      const worldY = (mouseY - (tree.y ?? 0)) / oldScale

      // Apply new scale
      tree.scaleX = newScale
      tree.scaleY = newScale

      // Adjust position to keep mouse point stable
      tree.x = mouseX - worldX * newScale
      tree.y = mouseY - worldY * newScale

      syncEditor()
      if (!isRestoringRef.current) {
        pushHistoryDebouncedRef.current?.()
      }
    }
    container.addEventListener('wheel', onWheel, { passive: false })

    // ===== Middle mouse button drag to pan =====
    let panning = false
    let panStartX = 0
    let panStartY = 0
    let panOriginX = 0
    let panOriginY = 0

    const onMouseDown = (e: MouseEvent) => {
      if (e.button !== 1) return // middle button only
      e.preventDefault()
      const tree = (app as any).tree
      if (!tree) return
      panning = true
      panStartX = e.clientX
      panStartY = e.clientY
      panOriginX = tree.x ?? 0
      panOriginY = tree.y ?? 0
      container.style.cursor = 'grabbing'
    }

    const onMouseMove = (e: MouseEvent) => {
      if (!panning) return
      const tree = (app as any).tree
      if (!tree) return
      tree.x = panOriginX + (e.clientX - panStartX)
      tree.y = panOriginY + (e.clientY - panStartY)
      syncEditor()
    }

    const onMouseUp = () => {
      if (!panning) return
      panning = false
      container.style.cursor = ''
      if (!isRestoringRef.current) {
        pushHistoryRef.current?.()
      }
    }

    container.addEventListener('mousedown', onMouseDown)
    window.addEventListener('mousemove', onMouseMove)
    window.addEventListener('mouseup', onMouseUp)

    // Prevent middle-click auto-scroll cursor
    const onAuxClick = (e: MouseEvent) => {
      if (e.button === 1) e.preventDefault()
    }
    container.addEventListener('auxclick', onAuxClick)

    setIsReady(true)
    refreshLayers()

    return () => {
      container.removeEventListener('wheel', onWheel)
      container.removeEventListener('mousedown', onMouseDown)
      window.removeEventListener('mousemove', onMouseMove)
      window.removeEventListener('mouseup', onMouseUp)
      container.removeEventListener('auxclick', onAuxClick)
      playerRef.current?.pause()
      app.destroy()
      appRef.current = null
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [])

  // ===== Sync tracks to animation player =====
  useEffect(() => {
    if (!playerRef.current) return
    const trackMap = new Map<string, Keyframe[]>()
    for (const [id, track] of tracks) {
      trackMap.set(id, track.keyframes)
    }
    playerRef.current.setTracks(trackMap)
  }, [tracks])

  // ===== Helper: Refresh layer list =====
  const refreshLayers = useCallback(() => {
    const app = appRef.current
    if (!app) return
    const tree = (app as any).tree
    if (!tree) return
    const children = (tree.children as IUI[]) || []
    setLayers(buildLayerList(children, 0))
  }, [])

  // ===== History: Take snapshot of current state =====
  const takeSnapshot = useCallback((): EditorSnapshot => {
    const app = appRef.current as any
    if (!app) return { elements: [], tracks: [], selectedId: null }
    const children = (app.tree?.children as IUI[]) || []
    const elements = children.map((el) => serializeElement(el))
    const tracksArr = Array.from(tracksRef.current.entries()) as [string, AnimationTrack][]
    const tree = app.tree
    const viewport = tree
      ? { x: tree.x ?? 0, y: tree.y ?? 0, scaleX: tree.scaleX ?? 1, scaleY: tree.scaleY ?? 1 }
      : undefined
    return {
      elements,
      tracks: tracksArr,
      selectedId: selectedIdRef.current,
      viewport,
    }
  }, [])

  // ===== History: Restore a snapshot =====
  const restoreSnapshot = useCallback(
    (snap: EditorSnapshot) => {
      const app = appRef.current as any
      if (!app) return
      isRestoringRef.current = true

      // Clear existing elements
      const tree = app.tree
      if (tree) {
        const existing = [...(tree.children as IUI[])]
        for (const child of existing) child.remove()
      }
      elementMap.current.clear()

      // Recreate elements
      for (const snap_el of snap.elements) {
        const el = createElementFromSnapshot(snap_el)
        tree.add(el)
        elementMap.current.set(snap_el.id, el)
      }

      // Restore tracks
      const newTracks = new Map<string, AnimationTrack>()
      for (const [id, track] of snap.tracks) {
        newTracks.set(id, track)
      }
      setTracks(newTracks)

      // Restore selection
      if (snap.selectedId) {
        const el = elementMap.current.get(snap.selectedId)
        if (el) {
          app.editor?.select(el)
          setSelectedId(snap.selectedId)
          setSelectedProps(extractProps(el))
        } else {
          app.editor?.select(null)
          setSelectedId(null)
          setSelectedProps(null)
        }
      } else {
        app.editor?.select(null)
        setSelectedId(null)
        setSelectedProps(null)
      }

      // Restore viewport (pan/zoom)
      if (snap.viewport && app.tree) {
        app.tree.x = snap.viewport.x
        app.tree.y = snap.viewport.y
        app.tree.scaleX = snap.viewport.scaleX
        app.tree.scaleY = snap.viewport.scaleY
        if (app.editor?.element) {
          app.editor.update()
        }
      }

      refreshLayers()
      isRestoringRef.current = false
    },
    [refreshLayers],
  )

  // ===== History: Update canUndo/canRedo flags =====
  const updateCanUndoRedo = useCallback(() => {
    setCanUndo(historyIndexRef.current > 0)
    setCanRedo(historyIndexRef.current < historyRef.current.length - 1)
  }, [])

  // ===== History: Push current state (immediate) =====
  const pushHistory = useCallback(() => {
    if (isRestoringRef.current) return
    // Clear pending debounce
    if (historyDebounceRef.current) {
      clearTimeout(historyDebounceRef.current)
      historyDebounceRef.current = null
    }
    const snap = takeSnapshot()
    const history = historyRef.current
    // Truncate redo entries
    history.length = historyIndexRef.current + 1
    history.push(snap)
    // Cap at 50 entries
    if (history.length > 50) {
      history.shift()
    } else {
      historyIndexRef.current++
    }
    updateCanUndoRedo()
  }, [takeSnapshot, updateCanUndoRedo])

  // ===== History: Push with debounce (for property edits & canvas drags) =====
  const pushHistoryDebounced = useCallback(() => {
    if (isRestoringRef.current) return
    if (historyDebounceRef.current) {
      clearTimeout(historyDebounceRef.current)
    }
    historyDebounceRef.current = setTimeout(() => {
      historyDebounceRef.current = null
      pushHistory()
    }, 600)
  }, [pushHistory])
  pushHistoryDebouncedRef.current = pushHistoryDebounced
  pushHistoryRef.current = pushHistory

  // ===== History: Undo =====
  const undo = useCallback(() => {
    if (historyIndexRef.current <= 0) return
    historyIndexRef.current--
    restoreSnapshot(historyRef.current[historyIndexRef.current])
    updateCanUndoRedo()
  }, [restoreSnapshot, updateCanUndoRedo])

  // ===== History: Redo =====
  const redo = useCallback(() => {
    if (historyIndexRef.current >= historyRef.current.length - 1) return
    historyIndexRef.current++
    restoreSnapshot(historyRef.current[historyIndexRef.current])
    updateCanUndoRedo()
  }, [restoreSnapshot, updateCanUndoRedo])

  // ===== Push initial empty-state snapshot once editor is ready =====
  useEffect(() => {
    if (isReady && historyRef.current.length === 0) {
      pushHistory()
    }
  }, [isReady, pushHistory])

  // ===== Helper: Get canvas center =====
  const getCanvasCenter = useCallback(() => {
    const app = appRef.current as any
    if (!app) return { x: 400, y: 300 }
    const w = app.canvas?.width || containerRef.current?.clientWidth || 800
    const h = app.canvas?.height || containerRef.current?.clientHeight || 600
    return { x: w / 2, y: h / 2 }
  }, [])

  // ===== SVG Import =====
  const importSVGFile = useCallback(async (file: File) => {
    const text = await file.text()
    const name = file.name.replace(/\.svg$/i, '') || 'SVG'
    await importSVGString(text, name)
  }, [])

  const importSVGString = useCallback(
    async (svgString: string, name?: string) => {
      const app = appRef.current as any
      if (!app) return

      pushHistory()
      const { group } = parseSVG(svgString)

      // Add the entire group as a single wrapped node
      const id = generateElementId()
      ;(group as any).id = id
      ;(group as any).name = name || 'SVG'
      ;(group as any).set({ editable: true })
      app.tree.add(group)
      trackElement(group, elementMap.current)

      refreshLayers()
    },
    [refreshLayers, pushHistory],
  )

  // ===== Add Shape =====
  const addShape = useCallback(
    (type: string) => {
      const app = appRef.current as any
      if (!app) return

      pushHistory()
      const center = getCanvasCenter()
      let element: IUI

      const fill = '#4A90D9'
      const stroke = '#2c5f8d'

      switch (type) {
        case 'Rect':
          element = new Rect({
            x: center.x - 60,
            y: center.y - 40,
            width: 120,
            height: 80,
            fill,
            stroke,
            strokeWidth: 2,
            cornerRadius: 8,
            editable: true,
            name: 'Rect',
          })
          break
        case 'Ellipse':
          element = new Ellipse({
            x: center.x - 50,
            y: center.y - 50,
            width: 100,
            height: 100,
            fill,
            stroke,
            strokeWidth: 2,
            editable: true,
            name: 'Ellipse',
          })
          break
        case 'Star':
          element = new Star({
            x: center.x - 50,
            y: center.y - 50,
            width: 100,
            height: 100,
            fill: '#F5A623',
            stroke: '#c97f0e',
            strokeWidth: 2,
            corners: 5,
            editable: true,
            name: 'Star',
          } as any)
          break
        case 'Path':
          element = new Path({
            path: 'M 0 0 L 50 0 L 25 50 Z',
            x: center.x - 25,
            y: center.y - 25,
            fill: '#7ED321',
            stroke: '#4a8b0c',
            strokeWidth: 2,
            editable: true,
            name: 'Path',
          } as any)
          break
        case 'Text':
          element = new Text({
            x: center.x - 60,
            y: center.y - 16,
            text: 'Hello',
            fontSize: 32,
            fill: '#333333',
            editable: true,
            name: 'Text',
          } as any)
          break
        default:
          return
      }

      const id = generateElementId()
      ;(element as any).id = id
      app.tree.add(element)
      elementMap.current.set(id, element)
      refreshLayers()

      // Select the new element
      app.editor?.select(element)
    },
    [getCanvasCenter, refreshLayers, pushHistory],
  )

  // ===== Delete Selected =====
  const deleteSelected = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const element = app.editor?.element
    if (!element) return

    pushHistory()
    const id = String(element.id || element.innerId)

    // Remove from map
    elementMap.current.delete(id)

    // Remove animation track
    setTracks((prev) => {
      const next = new Map(prev)
      next.delete(id)
      return next
    })

    // Remove from canvas
    element.remove()
    app.editor?.select(null)

    setSelectedId(null)
    setSelectedProps(null)
    refreshLayers()
  }, [refreshLayers, pushHistory])

  // ===== Duplicate Selected =====
  const duplicateSelected = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const element = app.editor?.element
    if (!element) return

    pushHistory()
    const clone = element.clone()
    clone.move(20, 20)
    const id = generateElementId()
    ;(clone as any).id = id
    ;(clone as any).set({ editable: true })
    app.tree.add(clone)
    elementMap.current.set(id, clone)
    refreshLayers()
    app.editor?.select(clone)
  }, [refreshLayers, pushHistory])

  // ===== Clear Canvas =====
  const clearCanvas = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const tree = app.tree
    if (!tree) return

    pushHistory()
    const children = [...(tree.children as IUI[])]
    for (const child of children) {
      child.remove()
    }
    elementMap.current.clear()
    setTracks(new Map())
    setSelectedId(null)
    setSelectedProps(null)
    refreshLayers()
  }, [refreshLayers, pushHistory])

  // ===== Select Element =====
  const selectElement = useCallback((id: string) => {
    const app = appRef.current as any
    if (!app) return
    const element = elementMap.current.get(id)
    if (!element) return
    app.editor?.select(element)
    setSelectedId(id)
    setSelectedProps(extractProps(element))
  }, [])

  // ===== Deselect =====
  const deselectAll = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    app.editor?.select(null)
    setSelectedId(null)
    setSelectedProps(null)
  }, [])

  // ===== Update Property =====
  const updateProperty = useCallback(
    <K extends keyof ElementProps>(prop: K, value: ElementProps[K]) => {
      const app = appRef.current as any
      if (!app) return
      const element = app.editor?.element
      if (!element) return

      const data: any = {}
      if (prop === 'cornerRadius') {
        data.cornerRadius = value
      } else if (prop === 'fill') {
        data.fill = value === 'none' ? 'transparent' : value
      } else if (prop === 'stroke') {
        data.stroke = value === 'none' ? '' : value
      } else if (prop === 'strokeWidth') {
        data.strokeWidth = value
        if (value === 0) data.stroke = ''
      } else {
        data[prop] = value
      }

      element.set(data)

      // Update selected props without triggering editor.update loop
      setSelectedProps((prev) => (prev ? { ...prev, [prop]: value } : null))
      pushHistoryDebounced()
    },
    [pushHistoryDebounced],
  )

  // ===== Toggle Layer Visibility =====
  const toggleLayerVisibility = useCallback(
    (id: string) => {
      const element = elementMap.current.get(id)
      if (!element) return
      element.visible = !element.visible
      refreshLayers()
    },
    [refreshLayers],
  )

  // ===== Toggle Layer Lock =====
  const toggleLayerLock = useCallback(
    (id: string) => {
      const element = elementMap.current.get(id)
      if (!element) return
      ;(element as any).locked = !(element as any).locked
      ;(element as any).hittable = !(element as any).locked
      refreshLayers()
    },
    [refreshLayers],
  )

  // ===== Rename Layer =====
  const renameLayer = useCallback(
    (id: string, name: string) => {
      const element = elementMap.current.get(id)
      if (!element) return
      ;(element as any).name = name
      refreshLayers()
    },
    [refreshLayers],
  )

  // ===== Animation: Add Keyframe =====
  const addKeyframe = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const element = app.editor?.element
    if (!element) return

    pushHistory()
    const id = String(element.id || element.innerId)

    const keyframe: Keyframe = {
      id: generateElementId(),
      time: currentTime,
      props: {
        x: element.x,
        y: element.y,
        rotation: element.rotation,
        scaleX: element.scaleX,
        scaleY: element.scaleY,
        opacity: element.opacity ?? 1,
      },
      easing: 'linear',
    }

    setTracks((prev) => {
      const next = new Map(prev)
      const existing = next.get(id)
      if (existing) {
        const filtered = existing.keyframes.filter(
          (kf) => Math.abs(kf.time - currentTime) > 0.05,
        )
        next.set(id, {
          ...existing,
          keyframes: [...filtered, keyframe].sort((a, b) => a.time - b.time),
        })
      } else {
        next.set(id, {
          elementId: id,
          keyframes: [keyframe],
        })
      }
      return next
    })
  }, [currentTime, pushHistory])

  // ===== Animation: Remove Keyframe =====
  const removeKeyframe = useCallback((elementId: string, keyframeId: string) => {
    pushHistory()
    setTracks((prev) => {
      const next = new Map(prev)
      const track = next.get(elementId)
      if (track) {
        const filtered = track.keyframes.filter((kf) => kf.id !== keyframeId)
        if (filtered.length === 0) {
          next.delete(elementId)
        } else {
          next.set(elementId, { ...track, keyframes: filtered })
        }
      }
      return next
    })
  }, [pushHistory])

  // ===== Animation: Play =====
  const playAnimation = useCallback(() => {
    const player = playerRef.current
    if (!player) return
    const app = appRef.current as any
    app?.editor?.select(null)
    player.setDuration(duration)
    player.play()
    setIsPlaying(true)
  }, [duration])

  // ===== Animation: Pause =====
  const pauseAnimation = useCallback(() => {
    playerRef.current?.pause()
    setIsPlaying(false)
  }, [])

  // ===== Animation: Stop =====
  const stopAnimation = useCallback(() => {
    playerRef.current?.stop()
    setIsPlaying(false)
    setCurrentTime(0)
  }, [])

  // ===== Animation: Seek =====
  const seekAnimation = useCallback((time: number) => {
    playerRef.current?.seek(time)
    setCurrentTime(time)
  }, [])

  // ===== Animation: Set Duration =====
  const setDuration = useCallback((d: number) => {
    setDurationState(d)
    playerRef.current?.setDuration(d)
  }, [])

  // ===== Export SVG =====
  const exportSVG = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const children = (app.tree?.children as IUI[]) || []
    if (children.length === 0) return

    const center = getCanvasCenter()
    const svg = exportToSVG(children, center.x * 2, center.y * 2)
    downloadFile(svg, 'export.svg', 'image/svg+xml')
  }, [getCanvasCenter])

  // ===== Export PNG =====
  const exportPNG = useCallback(async () => {
    const app = appRef.current as any
    if (!app) return
    try {
      const result = await app.tree.export('png', { pixelRatio: 2 })
      if (result?.data) {
        const a = document.createElement('a')
        a.href = result.data
        a.download = 'export.png'
        a.click()
      }
    } catch (e) {
      console.error('PNG export failed:', e)
    }
  }, [])

  // ===== Clipboard: Copy Selected =====
  const copySelected = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const element = app.editor?.element
    if (!element) return
    clipboardRef.current = serializeElement(element)
  }, [])

  // ===== Clipboard: Cut Selected =====
  const cutSelected = useCallback(() => {
    const app = appRef.current as any
    if (!app) return
    const element = app.editor?.element
    if (!element) return
    clipboardRef.current = serializeElement(element)
    deleteSelected()
  }, [deleteSelected])

  // ===== Clipboard: Paste =====
  const paste = useCallback(() => {
    const app = appRef.current as any
    if (!app || !clipboardRef.current) return

    pushHistory()
    const snap = clipboardRef.current
    const newId = generateElementId()
    const newSnap: ElementSnapshot = {
      ...snap,
      id: newId,
      props: {
        ...snap.props,
        x: snap.props.x + 20,
        y: snap.props.y + 20,
      },
    }
    const el = createElementFromSnapshot(newSnap)
    app.tree.add(el)
    elementMap.current.set(newId, el)
    refreshLayers()
    app.editor?.select(el)
  }, [pushHistory, refreshLayers])

  return {
    containerRef,
    isReady,
    selectedId,
    selectedProps,
    layers,
    tracks,
    currentTime,
    duration,
    isPlaying,
    canUndo,
    canRedo,
    importSVGFile,
    importSVGString,
    addShape,
    deleteSelected,
    duplicateSelected,
    clearCanvas,
    selectElement,
    deselectAll,
    updateProperty,
    toggleLayerVisibility,
    toggleLayerLock,
    renameLayer,
    addKeyframe,
    removeKeyframe,
    playAnimation,
    pauseAnimation,
    stopAnimation,
    seekAnimation,
    setDuration,
    exportSVG,
    exportPNG,
    undo,
    redo,
    copySelected,
    cutSelected,
    paste,
  }
}

export type UseEditorReturn = ReturnType<typeof useEditor>
